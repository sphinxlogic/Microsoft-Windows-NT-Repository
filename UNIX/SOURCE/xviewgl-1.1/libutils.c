#include "patchlevel.h"
/* Written by Brad Daniels, February 1991

   This code is provided as is, with no warrantees, express or implied.
   I believe this code to be free of encumbrance, and offer it to the
   public domain.  I ask, however, that this paragraph and my name be
   retained in any modified versions of the file you may make, and
   that you notify me of any improvements you make to the code.

   Brad Daniels (daniels@bigred.enet.dec.com)
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#ifdef READDIR_SUPPORTED
#ifdef INCLUDE_SYSDIR
#include <sys/dir.h>
#endif
#ifdef INCLUDE_DIRENT
#ifdef DIRENT_IN_SYS
#include <sys/dirent.h>
#else
#include <dirent.h>
#endif
#endif
#ifndef READDIR_TYPE_DIRECT
#define direct dirent
#endif
#endif
#ifdef STDLIB
#include <stdlib.h>
#endif
#ifdef UNISTD
#include <unistd.h>
#endif
#include "my_types.h"
#include "parser.h"
#include "utils.h"

#ifdef VMS
/* On currently released versions of VMS, the fread(buf,1,n,fp) calls would
   crawl because each one generates a separate read. */
#define fread(buf,sz,n,fp) read(fileno(fp),(buf),(sz)*(n))
#endif

MY_FILE program_fp_struct;
BOOLEAN directory_mode;
static char lib_fname[FILENAME_MAX];
int prog_index;
int lib_filecnt;
struct lib_file_entry *lib_files;

#define RECLEN 17

BOOLEAN init_lib(char *s, char *script_name, char *err_msg)
{
char *ext;
char fname[FILENAME_MAX];
byte buf[RECLEN];
struct stat stb;
#ifdef READDIR_SUPPORTED
DIR *dirp;
struct direct *dp;
#endif
FILE *libfp;
BOOLEAN found_txt=FALSE;
int i, len;

    strcpy(fname,s);
    program_fp = &program_fp_struct;
    if ((ext=strrchr(fname,'/')) == NULL)
	ext = fname;
    /* Is it a directory? */
    if ((stat(fname,&stb) == -1) || !(stb.st_mode&S_IFDIR)) {
    /* No.  Supply .GL extension if it doesn't have one */
	if ((ext=strrchr(ext,'.')) == NULL) {
	    extend(fname,".gl");
	    ext=strrchr(fname,'.');
	}
    } else {
    /* Set directory mode and find first ".txt" file */
#ifdef READDIR_SUPPORTED
	directory_mode = TRUE;
	if (script_name == NULL) {
	    dirp = opendir(fname);
	    for (dp = readdir(dirp); (dp != NULL); dp = readdir(dirp)) {
		ext = strrchr(dp->d_name,'.');
		if ((ext != NULL) && !strcmp(ext,".txt")) {
		    break;
		}
	    }
	    if (dp==NULL) {
		sprintf(err_msg,"No text file in directory %s",fname);
		return FALSE;
	    }
	    strcpy(program_fname,dp->d_name);
	    closedir(dirp);
	} else {
	    strcpy(program_fname,script_name);
	}
	chdir(fname);
	program_fp->fp=fopen(program_fname,"r");
	if (program_fp->fp == NULL) {
	    sprintf(err_msg,"Error opening %s: %s",program_fname,
		    strerror(errno));
	    return FALSE;
	}
	return TRUE;
#else /*!READDIR_SUPPORTED*/
	sprintf(err_msg,"Directory names not supported on this system");
	return FALSE;
#endif
    }
    if (!strcmp(ext,".txt")) {
	directory_mode=TRUE;
	strcpy(program_fname,fname);
#ifdef VMS
	program_fp->fp=fopen(program_fname,"r","ctx=stm");
#else
	program_fp->fp=fopen(program_fname,"r");
#endif
	if (program_fp->fp == NULL) {
	    sprintf(err_msg,"Error opening %s: %s",program_fname,
		    strerror(errno));
	    return FALSE;
	}
	ext=strrchr(program_fname,'/');
	if (ext != NULL) {
	    *ext = 0;
	    chdir(program_fname);
	    strcpy(fname,ext+1);
	    strcpy(program_fname,fname);
	}
	return TRUE;
    }
    directory_mode = FALSE;
#ifdef VMS
    libfp = fopen(fname,"r","ctx=stm");
#else
    libfp = fopen(fname,"r");
#endif
    if (libfp == NULL) {
	sprintf(err_msg,"Error opening %s: %s",fname,strerror(errno));
	return FALSE;
    }
    strcpy(lib_fname,fname);

    if (fread(buf,1,2,libfp)!=2) {
	sprintf(err_msg,"Unexpected EOF reading library %s",fname);
	return FALSE;
    }
    lib_filecnt = (buf[0] + 256*buf[1])/RECLEN;
    lib_files = malloc((lib_filecnt+1)*sizeof(*lib_files));
    for(i=0;i<lib_filecnt;i++) {
	if (fread(buf,1,RECLEN,libfp) != RECLEN) {
	    sprintf(err_msg,"Unexpected EOF reading library %s",fname);
	    return FALSE;
	}
	lib_files[i].offset=buf[0]+(((int)buf[1])<<8)+(((int)buf[2])<<16)+(((int)buf[3])<<24);
	strcpy(lib_files[i].name,(char *)(buf+4));
	downcase(lib_files[i].name);
	if (!found_txt &&
	    (((script_name == NULL) &&
	      (((ext=strrchr(lib_files[i].name,'.')) != NULL) &&
	       !strcmp(ext,".txt"))) ||
	     ((script_name != NULL) &&
	      !strcmp(script_name,lib_files[i].name)))) {
	    strcpy(program_fname,lib_files[i].name);
	    prog_index = i;
	    found_txt = TRUE;
	}
        if (!(*lib_files[i].name))
	    lib_filecnt=i;
    }
    if (!found_txt) {
	sprintf(err_msg,"Library %s does not contain a .txt file",fname);
	return FALSE;
    }
    program_fp->fp = libfp;
    fseek(program_fp->fp,lib_files[prog_index].offset,SEEK_SET);
    fread(buf,4,1,program_fp->fp);
    program_fp->bytesleft = buf[0] + (((int)buf[1])<<8) + (((int)buf[2])<<16) + (((int)buf[3])<<24);
    return TRUE;
}

MY_FILE *find_file(char *fname)
{
MY_FILE *fp;
int i;
byte buf[4];
    if (directory_mode) {
	fp = calloc(1,sizeof(*fp));
#ifdef VMS
	fp->fp = fopen(fname,"r","ctx=stm");
#else
	fp->fp = fopen(fname,"r");
#endif
	return fp;
    } else {
	for (i=0;i<lib_filecnt;i++) {
	    if (!strcmp(fname,lib_files[i].name)) {
		fp = calloc(1,sizeof(*fp));
#ifdef VMS
		fp->fp = fopen(lib_fname,"r","ctx=stm");
#else
		fp->fp = fopen(lib_fname,"r");
#endif
		fseek(fp->fp,lib_files[i].offset,SEEK_SET);
		fread(buf,4,1,fp->fp);
		fp->bytesleft = buf[0] + (((int)buf[1])<<8) +
				(((int)buf[2])<<16) + (((int)buf[3])<<24);
		return fp;
	    }
	}
	errno = ENOENT;
	return FALSE;
    }
}

MY_FILE *reopen_prog(char *fname, char *mode)
{
MY_FILE *fp;
int i;
byte buf[4];
    if (directory_mode) {
	fp = calloc(1,sizeof(*fp));
#ifdef VMS
	fp->fp = fopen(fname,mode,"ctx=stm");
#else
	fp->fp = fopen(fname,mode);
#endif
	return fp;
    } else {
	for (i=0;i<lib_filecnt;i++) {
	    if (!strcmp(fname,lib_files[i].name)) {
		fp = calloc(1,sizeof(*fp));
#ifdef VMS
		fp->fp = fopen(lib_fname,mode,"ctx=stm");
#else
		fp->fp = fopen(lib_fname,mode);
#endif
		fseek(fp->fp,lib_files[i].offset,SEEK_SET);
		fread(buf,4,1,fp->fp);
		fp->bytesleft = buf[0] + (((int)buf[1])<<8) +
				(((int)buf[2])<<16) + (((int)buf[3])<<24);
		prog_index=i;
		return fp;
	    }
	}
	errno = ENOENT;
	return FALSE;
    }
}

void rewind_program()
{
byte buf[4];
    if (directory_mode)
	rewind(program_fp->fp);
    else {
	fseek(program_fp->fp,lib_files[prog_index].offset,SEEK_SET);
	fread(buf,4,1,program_fp->fp);
	program_fp->bytesleft = buf[0] + (((int)buf[1])<<8) +
				(((int)buf[2])<<16) + (((int)buf[3])<<24);
    }
}

int my_fgetc(MY_FILE *fp)
{
    if (directory_mode)
	return fgetc(fp->fp);
    if (--fp->bytesleft)
	return fgetc(fp->fp);
    else
	return -1;
}

char *my_fgets(char *s, int n, MY_FILE *fp)
{
char *r;
int c;
    if (n==0) return s;
    r = s;
    do {
	if (--n>0) {
	    c = my_getc(fp);
	    if (c == '\032')
		c = -1;
	    if ((c==-1) && (s==r))
		return NULL;
	    if (c>0)
		*s++ = c;
	}
    } while (n && (c>0) && (c != '\r') && (c != '\n'));
    if ((--n>0) && (c == '\r')) {
	if ((c=my_getc(fp)) == '\n')
	    *s++ = c;
	else
	    my_ungetc(c,fp);
    }
    *s = '\0';
    return r;
}

int my_fgetpos(MY_FILE *fp, my_fpos_t *pos)
{
    pos->bytesleft = fp->bytesleft;
    return fgetpos(fp->fp,&pos->pos);
}

int my_fsetpos(MY_FILE *fp, my_fpos_t *pos)
{
    fp->bytesleft = pos->bytesleft;
    return fsetpos(fp->fp,&pos->pos);
}

int my_read(MY_FILE *fp, void *buf, int n)
{
    if (directory_mode)
	return fread(buf,1,n,fp->fp);
    if (n>fp->bytesleft)
	n = fp->bytesleft;
    n = fread(buf,1,n,fp->fp);
    if (n==-1)
	return n;
    fp->bytesleft -= n;
    return n;
}

void my_fclose(MY_FILE *fp)
{
    fclose(fp->fp);
    free(fp);
}
