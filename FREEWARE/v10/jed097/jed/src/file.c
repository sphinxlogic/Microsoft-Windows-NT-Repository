/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#include <string.h>
#include <limits.h>


#ifdef unix
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/file.h>
# ifdef SYSV
#   include <sys/fcntl.h>
# endif
#endif

#ifdef __os2__
#include <fcntl.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
 
 typedef struct HOLDFEA *PHOLDFEA;
 PHOLDFEA QueryEAs (char *name);
 int WriteEAs (char *name, PHOLDFEA pHoldFEA);
#endif

#ifdef msdos
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#endif
#ifdef __GO32__
#include <fcntl.h>
#endif

/* Was anything missed? */
#ifndef O_RDONLY
#ifdef VMS
#include <file.h>
#else
#include <fcntl.h>
#endif
#endif

#include "config.h"
#include "buffer.h"
#include "file.h"
#include "misc.h"
#include "sysdep.h"
#include "paste.h"
#include "slang.h"
#include "ins.h"
#include "ledit.h"

#if defined (msdos) || defined (__os2_16__)
#define MAX_LINE_LEN 1024
#else
#define MAX_LINE_LEN 64 * 1024
#endif

#ifdef VMS
static int vms_max_rec_size;
#include <stat.h>
#include <rms.h>
static int VMS_write_rfm_fixed;
int vms_stupid_open(char *file)
{
   struct stat s;
   char rat_buf[80], rfm_buf[80], mrs_buf[40], *rfm = "var";
   unsigned short mode = 0, c;
   int ret;

   VMS_write_rfm_fixed = 0;

   strcpy(rfm_buf, "rfm=");
   
   
   if (0 == stat(file, &s))
     {
	strcpy(rat_buf, "rat");
	c = s.st_fab_rat;
	if (c & FAB$M_FTN)  strcat(rat_buf, ",ftn");
	if (c & FAB$M_CR)  strcat(rat_buf, ",cr");
	if (c & FAB$M_PRN)  strcat(rat_buf, ",prn");
	if (c & FAB$M_BLK)  strcat(rat_buf, ",blk");
	if (rat_buf[3] != 0) rat_buf[3] = '='; else *rat_buf = 0;

	c = s.st_fab_rfm;
	switch(c)
	  {
	   case FAB$C_UDF:  rfm = "udf"; break;
	   case FAB$C_FIX:  
	     rfm = "fix"; 
	     VMS_write_rfm_fixed = 1;
	     break;
	   case FAB$C_VAR:  rfm = "var"; break;
	   case FAB$C_VFC:  rfm = "vfc"; break;
	   case FAB$C_STM:  rfm = "stm"; break;
	   case FAB$C_STMLF:  rfm = "stmlf"; break;
	   case FAB$C_STMCR:  rfm = "stmcr"; break;
	  }
	mode = s.st_mode & 0777;
     }
   else strcpy (rat_buf, "rat=cr");
   
   strcat(rfm_buf, rfm);
   
   if (vms_max_rec_size <= 0) vms_max_rec_size = 255;
   sprintf(mrs_buf, "mrs=%d", vms_max_rec_size);
      
   if (*rfm == 's')		       /* stream */
     {
	ret = creat(file, mode, rfm_buf);
     }
   else
     {
	if (*rat_buf) ret = creat(file, mode, rfm_buf, mrs_buf, rat_buf);
	else ret = creat(file, mode, rfm_buf, mrs_buf);
     }
   if (ret >= 0) chmod(file, mode);
   return ret;
}
#endif

int Require_Final_Newline = 0;

/* 0 = read, 1 = write , 2 = append... */
int sys_open(char *file, int acces)
{
   int fp = -1;
   int flags;
   unsigned int mode;
#ifdef VMS
   char *p, neew[256];
#endif
   
#ifdef O_BINARY
	mode = O_BINARY;
#else
	mode = 0;
#endif

   flags = file_status(file);
   if ((flags < 0) || (flags > 1)) return(fp);
   
   /* on VMS I cheat since I do not want to deal with RMS at this point */
#ifdef VMS
   VMS_write_rfm_fixed = 0;
   strcpy(neew, file);
   p = neew; while (*p) if (*p == ';') *p = 0; else p++;
   
   if (acces == 0) fp = open(file, O_RDONLY, "ctx=rec","mbf=8","mbc=32","rop=RAH","shr=upi,get,put");
   else if (acces == 1)
     {
	fp = vms_stupid_open(neew);
     }
   
   else if (acces == 2) fp = open(file, O_WRONLY | O_APPEND | O_CREAT | mode);
#else
   
   switch(acces)
     {
      case 0: flags =  O_RDONLY; 
	break;
      case 1: flags =  O_WRONLY | O_CREAT | O_TRUNC;
	break;
      case 2: flags =  O_WRONLY | O_CREAT | O_APPEND; break;
      default: return(fp);
     }
   
   flags |= mode;
   
#if defined (msdos) || defined (__os2__)
   fp = open(file, flags, S_IREAD | S_IWRITE); 
#else
   fp = open(file, flags, 0666); 
#endif
#endif /* VMS */
   return(fp);
}

/* Leaves Point at last line inserted */
/* returns -1 if unable to open file,
           -2 if memory allocation error
           otherwise returns number of lines read */

char *file_type(char *file)
{
   char *p, *psave;
   if ((file == (char *) NULL) || (*file == 0)) return( (char *) NULL);

   file = extract_file(file);
   p = file; while (*p != 0) p++;
   psave = p;
   while((p != file) && (*p != '.')) p--;
   if (*p == '.') p++;
   if (p == file) return(psave); else return(p);
}

void set_file_modes()
{
   char *type;

   if (CBuf == NULL) return;
   CBuf->c_time = sys_time();
   if (CBuf->file[0])
     {
	CBuf->flags |= AUTO_SAVE_BUFFER;
	CBuf->hits = 0;
	type = file_type(CBuf->file);
     }
   else type = (char *) NULL;

   if (type == (char *) NULL) CBuf->modes = NO_MODE;
   else if (SLang_run_hooks("mode_hook", type, NULL));
   else CBuf->modes = NO_MODE;
}

int read_file(char *file)
{
   int fp;
   int n, status;

   if ((fp = sys_open(file, 0)) < 0)
     {
	status = file_status(file);
	if (!status) return(-1);  /* file does not exist */
	return(-2); /* exists but not readable */
     }

   n = read_file_pointer(fp);
   close(fp);
   eob();
   if ('\n' == *(CLine->data + Point)) make_line(2);

   VFile_Mode = VFILE_TEXT;
   return n;
}

int insert_file_pointer(VFILE *vp)
{
   int n = 0;
   unsigned int num;
   unsigned char *vbuf;
   
   Suspend_Screen_Update = 1;
   while(NULL != (vbuf = (unsigned char *) vgets(vp, &num)))
     {
	n++;
	if (SLang_Error) break;
	quick_insert(vbuf, (int) num);
     }
   return(n);
}

int insert_file(char *file)
{
   VFILE *vp;
   int n;
   
   if (NULL == (vp = vopen(file, 0, VFile_Mode))) return(-1);
   n = insert_file_pointer(vp);
   vclose(vp);
   return(n);
}


#define WRITE jed_write1

#if defined(unix) || defined(VMS)
#define BUFSIZE 0x7FFF
#else
#define BUFSIZE 512
#endif

static int Output_Buffer_Size = BUFSIZE;
static char output_buffer[BUFSIZE];
static char *output_bufferp;
static char *output_bufferp_max;


/* definitely perform the write.  Return number of chars written */
int jed_write1(int fd, char *b, unsigned int n)
{
#ifndef msdos   
   int len;
   unsigned int total = 0;
   while (total < n)
     {
	if ((len = write (fd, b, n - total)) <= 0) break;
	total += (unsigned int) len;
	b += len;
     }
   return total;
#else
   int num = -1;
   asm mov ah, 40h
   asm mov bx, fd
   asm mov cx, n
   asm push ds
   asm lds dx, dword ptr b
   asm int 21h
   asm pop ds
   asm jc L1
   asm mov num, ax		       /* number of bytes written */
   L1: 
   return(num);
#endif
}



/* RMS wants to start a NEW record after a write so just forget it! */
/* maybe do write-- return number of chars possibly written */

int jed_write(int fd, char *b, unsigned int n)
{
   int num, max, nl_flag = 0;
   unsigned int nsave = n;
   int cr_flag = CBuf->flags & ADD_CR_ON_WRITE_FLAG;

#ifdef VMS
   if (VMS_write_rfm_fixed == 0)
     {
	output_bufferp = output_buffer;
	return jed_write1(fd, b, n);
     }
   
#endif

   /* amount of space left in buffer */
   /* copy whats in b to the output buffer */
   while(n)
     {
	num = (int) (output_bufferp_max - output_bufferp);
	if (n > num)
	  {
	     max = num;
	     MEMCPY(output_bufferp, b, max);
	     output_bufferp += max;
	  }

	else if (cr_flag && 
		 (*(b + (n - 1)) == '\n') && (VFile_Mode == VFILE_TEXT))
	  {
	     max = n - 1;
	     MEMCPY(output_bufferp, b, max);
	     output_bufferp += max;
	     *output_bufferp++ = '\r';
	     max++;

	     /* can only write the \r */
	     if (n == num) nl_flag = 1; else *output_bufferp++ = '\n';
	  }
	else
	  {
	     max = n;
	     MEMCPY(output_bufferp, b, max);
	     output_bufferp += max;
	  }
	
	if (output_bufferp == output_bufferp_max)
	  {
	     output_bufferp = output_buffer;
	     if (Output_Buffer_Size != WRITE(fd, output_buffer, Output_Buffer_Size)) return(-1);
	     if (nl_flag)
	       {
		  nl_flag = 0;
		  *output_bufferp++ = '\n';
	       }
	  }
	b += max;
	n -= max;
     }
   return(nsave);
}

/* returns -1 on failure */
int write_region_to_fp(int fp)
{
   register int pnt, len;
   register Line *first, *last;
   int last_pnt, n = 0;
   char *err = "Write Failed!";

#ifndef VMS
   char nl = '\n';
#endif
   
   output_bufferp = output_buffer;
   output_bufferp_max = output_buffer + BUFSIZE;
   Output_Buffer_Size = BUFSIZE;

#ifdef VMS
   if (VMS_write_rfm_fixed && (vms_max_rec_size <= BUFSIZE))
     {
	Output_Buffer_Size = vms_max_rec_size;
     }
   else VMS_write_rfm_fixed = 0;
#endif   
   if (!check_region(&Number_One)) return(-1);
   last = CLine; last_pnt = Point;

   pop_mark(&Number_One);
   first = CLine; pnt = Point;

   /* first should never be null without hitting last first.  If this
      ever happens, check_region failed. */
   while (first != last)
     {
	len = first->len - pnt;
	if (len != jed_write(fp, (char *) (first->data + pnt), len))
	  {
	     msg_error(err);
	  }
	
	/* This goes here inside the loop because it is possible for external
	   events to set error_buffer */
	pnt = 0;
	if (SLang_Error) break;
	first = first->next;
	n++;
     }

   if (!SLang_Error && (last_pnt != 0))
     {
	len = last_pnt - pnt;
	if (len != jed_write(fp, (char *) (last->data + pnt), len))
	  {
	     msg_error(err);
	  }
	n++;
     }
#ifndef VMS
   if ((Require_Final_Newline) && (CBuf->end == last))
     {
	eob(); if (Point) jed_write(fp, &nl, 1);
     }
#endif
   

   /* Now flush output buffer if necessary */
   
   len = (int) (output_bufferp - output_buffer);
   if (!SLang_Error && len) if (len != WRITE(fp, output_buffer, len))
     {
	msg_error(err);
     }
   
   output_bufferp = output_buffer;

   
   pop_spot();
   VFile_Mode = VFILE_TEXT;
   if (SLang_Error) return(-1);
   return(n);
}

/* write current buffer to open file pointer. Return number of lines */

int write_region(char *file)
{
   int fp;
   int n;
   char msg[256];

   if (!check_region(&Number_Zero)) return(-1);
   if ((fp = sys_open(file, 1)) < 0)
     {
	sprintf(msg, "Unable to open %s for writing.", file);
	msg_error(msg);
	return(-1);
     }
   n = write_region_to_fp(fp);
   close(fp);

   return(n);
}



/* returns -1 on failure and number of lines on success */

int write_file(char *file)
{
   Mark *m;
   int n = -1;
   int fnl;
   
   
#ifdef VMS
   register Line *l = CBuf->beg;
   register int len = 0, max = 0;
   
   while (l != NULL)
     {
	len = l->len;
	if (len > max) max = len;
	l = l->next;
     }
   vms_max_rec_size = max;
#endif
   
   push_spot();
   bob();
   push_mark();  m = CBuf->marks;
   eob();
   fnl = Require_Final_Newline;
   if (CBuf->flags & BINARY_FILE) 
     {
	VFile_Mode = VFILE_BINARY;
	Require_Final_Newline = 0;

#ifdef VMS
	vms_max_rec_size = 512;
#endif
     }

   n = write_region(file);
   
   Require_Final_Newline = fnl;
   VFile_Mode = VFILE_TEXT;
   if (m == CBuf->marks) pop_mark(&Number_Zero);
   pop_spot();
   return(n);
}

int append_to_file(char *file)
{
   int fp;
   int n;

   if ((fp = sys_open(file, 2)) < 0) return(-1);
   n = write_region_to_fp(fp);
   close(fp);
   check_buffers();
   return(n);
}

int make_autosave_filename(char *save, char *dir, char *file)
{
   char *s;
   int dat;

   if (*file == 0) return(0);
      
   
   if (SLang_run_hooks("make_autosave_filename", dir, file) && !SLang_Error)
     {
	if (SLang_pop_string(&s, &dat)) return(0);
	strncpy(save, s, 254);
	save[255] = 0;
	if (dat == 1) SLFREE(s);
     }
   else
     {
#ifdef unix
	sprintf(save, "%s#%s#", dir, file);
#else
#if defined (msdos) || defined (__os2__)
	sprintf(save, "%s#%s", dir, file);
#else
	sprintf(save, "%s_$%s;1", dir, file);
#endif
#endif
     }
   return(1);
}

int write_file_with_backup(char *dir, char *file)
{
   char neew[256]; char save[256];
   int n;
   int mode, do_mode;
   short uid, gid;
#ifndef VMS
   char *old;
   int ok = 0, bu, do_free = 0;
#endif
#ifdef __os2__
   PHOLDFEA EAs;
#endif
   
   if (*file == 0) return(-1);

   sprintf(neew, "%s%s", dir, file);
   
   if (((CBuf->flags & AUTO_SAVE_BUFFER) == 0)
       || !make_autosave_filename(save, dir, file)) *save = 0;

   do_mode = sys_chmod(neew, 0, &mode, &uid, &gid);
   if ((do_mode < 0) ||  (do_mode > 1)) return(-1);
#ifndef VMS
   
#ifdef __os2__
   EAs = QueryEAs (neew);
#endif
   
   bu = 1;
   if (((CBuf->flags & NO_BACKUP_FLAG) == 0)
       && SLang_run_hooks("make_backup_filename", dir, file))
     {
	if (((bu = !SLang_pop_string(&old, &do_free)) != 0) && (*old))
	  {
	     unlink(old);
	     ok = !rename(neew, old);
	  }
     }
#endif
   
   if (-1 != (n = write_file(neew)))
     {
	if (*save)
	  {
	     sys_delete_file(save);
	  }
	     
	if (do_mode) /* must be an existing file, so preserve mode */
	  {
#ifdef msdos
	     /* Want the archive bit set since this is a new version */
	     mode |= 1 << 5;
#endif
	     sys_chmod (neew, 1, &mode, &uid, &gid);
#ifdef __os2__
	     WriteEAs (neew, EAs);
#endif
	  }
	/* This is for NFS time problems */
	CBuf->c_time = sys_file_mod_time(neew);
	/* CBuf->c_time = sys_time(); */
     }
#ifndef VMS
   else if (ok && bu && *old) rename(old, neew); /* error -- put it back */
   if ((do_free == 1) && bu) SLFREE(old);
#endif
   return(n);
}

/* warning-- this saves on the narrowed part of buffer.
   Here, I widen first.  I need a save_restriction type of thing because
   I do not narrow back.
   */
void auto_save_buffer(Buffer *b)
{
   char tmp[256];
   Buffer *old_buf;
   unsigned int vfm = VFile_Mode;
   
   if (b == NULL) return;
   b->hits = 0;
   if ((b->narrow != NULL) || !(b->flags & BUFFER_TRASHED)) return;

   old_buf = CBuf;
   switch_to_buffer(b);
   
   if (b->flags & BINARY_FILE)  VFile_Mode = VFILE_BINARY; 
   else VFile_Mode = VFILE_TEXT;

   if (b->flags & AUTO_SAVE_BUFFER)
     {
	if (make_autosave_filename(tmp, b->dir, b->file))
	  {
	     flush_message("autosaving..."); 
	     sys_delete_file(tmp);
	     write_file(tmp);
	     message("autosaving...done");
	  }
      }
   else if (b->flags & AUTO_SAVE_JUST_SAVE)
     {
	if (write_file_with_backup(b->dir, b->file) >= 0)
	  {
	     b->flags &= ~BUFFER_TRASHED;
	  }
     }
   switch_to_buffer(old_buf);
   VFile_Mode = vfm;
}

void auto_save_all()
{
    Buffer *b;

   if (NULL == (b = CBuf)) return;
   do
     {
	while (b->narrow != NULL) widen_buffer(b);
	if (*b->file != 0) auto_save_buffer(b);
	b = b->next;
     }
   while (b != CBuf);
}

#ifdef unix
#ifndef __GO32__
int is_link(char *f, char *f1)
{
   struct stat s;
   int l;
   int is_dir = 0;
   char work[256];
   
   l = strlen(f) - 1;
   if ((f[l] == '/') && (l > 1))
     {
	strcpy(work, f);
	is_dir = 1;
	f = work;
	f[l] = 0;
     }
   

   if (( lstat(f, &s) == -1 ) 
       /* || ((s.st_mode & S_IFMT)  S_IFLNK)) */
       || (((s.st_mode & S_IFMT) & S_IFLNK) == 0))
     return(0);
   
   if (-1 == (l = readlink(f, f1, 512))) return(0);
   if (is_dir) f1[l++] = '/';
   f1[l] = 0;
   return(1);
}

/* we do not worry about the dir--- only the filename */
char *expand_link(char *f)
{
   char work[256], lnk[256];
   char *d = expand_filename(f);
   
   if (is_link(d, lnk)) 
     {
	strcpy(work, d);
	*(extract_file(work)) = 0;
	strcat(work, lnk);
	d = expand_filename(work);
     }
   
   return (d);
}

#endif
#endif

void visit_file(char *dir, char *file)
{
#ifndef __os2__
   if (strcmp(file, CBuf->file) || strcmp(dir, CBuf->dir))
#else
   if (strcmpi(file, CBuf->file) || strcmp(dir, CBuf->dir))
#endif
     {
	if (NULL == find_buffer(file)) strcpy(CBuf->name, file); 
	else uniquely_name_buffer(file);
   
	strcpy(CBuf->dir, dir);
	strcpy(CBuf->file, file);
     }
   /* We have already taken care of this in write buffer function.
    */
   /* CBuf->c_time = sys_time(); */

   check_buffers();
}


char *dir_file_merge(char *dir, char *file)
/* 
 * returns result of merging dir with file. If file is empty, dir is
 * considered to be whole file.
 */
{
   char name[512];

   strcpy (name, dir);
   if ((file != NULL) && *file)
     {
	fixup_dir(name);
	strcat(name, file);
     }
   return expand_filename(name);
}

int file_status(char *file)
/*
 *  Returns a coded integer about file.  If the file does not exist, 0 is
 *  returned.  Meaning:
 *
 *     2 file is a directory
 *     1 file exists
 *     0 file does not exist.
 *    -1 no access.
 *    -2 path invalid
 *    -3 unknown error
 */
{
   int mode = 0;
   short uid, gid;
   return sys_chmod(file, 0, &mode, &uid, &gid);
}

int file_changed_on_disk(char *file)
{
   unsigned long t;
   Buffer *buf;
   if (NULL == (buf = find_file_buffer(file))) return(0);
   t = sys_file_mod_time(file);
   return(t > buf->c_time);
}

int file_time_cmp(char *file1, char *file2)
{
   unsigned long t1, t2;
   
   t1 = sys_file_mod_time(file1);
   t2 = sys_file_mod_time(file2);
    /*These times are modification  times from 1970.  Hence, the bigger 
     * number represents the more recent change.  Let '>' denote 
     * 'more recent than'.  This function returns:
     *	   1:  file1 > file2
     *	   0:  file 1 == file2
     *	   -1: otherwise. 
     *	So:
     */
   if (t1 == t2) return(0);
   if (t1 > t2) return(1); 
   return(-1);
}

void auto_save(void)
{
   auto_save_buffer(CBuf);
}

void check_buffer(Buffer *b)
{
   if ((*b->file != 0)
       && file_changed_on_disk(dir_file_merge(b->dir, b->file)))
     {
	b->flags |= FILE_MODIFIED;
     }
   else b->flags &= ~FILE_MODIFIED;
}

void check_buffers()
{
   Buffer *b = CBuf;
   do
     {
	check_buffer(b);
	b = b->next;
     }
   while (b != CBuf);
}

void set_file_trans(int *mode)
{
   if (*mode) VFile_Mode = VFILE_BINARY; else VFile_Mode = VFILE_TEXT;
}

int read_file_pointer(int fp)
{
   int n = 1;
   unsigned int num;
   unsigned char *vbuf;
   VFILE *vp;
   
   if (SLang_Error || (vp = vstream(fp, MAX_LINE_LEN, VFile_Mode)) == NULL) return(-1);
   
   if (NULL == (vbuf = (unsigned char *) vgets(vp, &num))) return(0);
   
   if (CLine->space < num)
     {
	remake_line(CLine->len + num + 1);
     }
   MEMCPY((char *) CLine->data, (char *) vbuf, (int) num);
   CLine->len = num;
   
   while(NULL != (vbuf = (unsigned char *) vgets(vp, &num)))
     {
	n++;
	if ((num == 1) && (*vbuf == '\n')) make_line(num); else make_line(num + 1);
	MEMCPY((char *) CLine->data, (char *) vbuf, (int) num);
	CLine->len = num;
	if (SLang_Error) break;
     }
   if (vp->buf != NULL) SLFREE(vp->buf);
   
   if (vp->cr_flag) CBuf->flags |= ADD_CR_ON_WRITE_FLAG;
   else CBuf->flags &= ~ADD_CR_ON_WRITE_FLAG;
   
   SLFREE(vp);
   return n;
}


