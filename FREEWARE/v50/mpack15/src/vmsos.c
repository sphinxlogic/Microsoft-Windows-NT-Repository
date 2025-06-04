/* (C) Copyright 1993,1994 by Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of Carnegie
 * Mellon University not be used in advertising or publicity
 * pertaining to distribution of the software without specific,
 * written prior permission.  Carnegie Mellon University makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * CARNEGIE MELLON UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#ifndef VMS
#define VMS
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#ifndef VMS
#include <sys/param.h>
#endif
#include <netdb.h>
#include "xmalloc.h"
#include "common.h"
#include "part.h" /*+ +*/

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

extern int errno;
extern char *malloc();
extern char *getenv();

int overwrite_files = 0;
int didchat;

/* The name of the file we're writing */
static char *output_fname = 0;

/* Characters that shouldn't be in filenames */
#ifndef VMS
#define BADCHARS "!$&*()|\'\";<>[]{}?/`\\ \t"
#else
#define BADCHARS "=!%&*():|\'\";<>[]{}?/`\\ \t"
#endif

/* Generate a message-id */
char *os_genid()
{
    static int pid = 0;
    static time_t curtime;
    static char hostname[MAXHOSTNAMELEN+1];
    char *result;
    struct hostent *hp;
    

    if (pid == 0) {
	pid = getpid();
	time(&curtime);
	gethostname(hostname, sizeof(hostname));

#ifndef VMS
	/* If we don't have a FQDN, try canonicalizing with gethostbyname */
	if (!strchr(hostname, '.')) {
	    hp = gethostbyname(hostname);
	    if (hp) {
		strcpy(hostname, hp->h_name);
	    }
	}
#endif /* 1 */
    }

    result = malloc(25+strlen(hostname));
#ifndef VMS
    sprintf(result, "%d.%d@%s", pid, curtime++, hostname);
#else
    sprintf(result, "%d_%d_%s", pid, curtime++, hostname);
#endif
    return result;
}

/* Create and return directory for a message-id */
char *os_idtodir(id)
char *id;
{
    static char buf[4096];
    char *p;

    if (getenv("TMPDIR")) {
	strcpy(buf, getenv("TMPDIR"));
    }
#ifndef VMS
    else {
	strcpy(buf, "/usr/tmp");
    }
    strcat(buf, "/m-prts-");
#else
    strcpy(buf, "m-prts-"); /* create on the current dir */
#endif
    p = getenv("USER");
    if (!p) p = getenv("LOGNAME");
    if (!p) p = "x";
    strcat(buf, p);
    
    (void)mkdir(buf, 0700);

    p = buf + strlen(buf);
    *p++ = '/';
    while (*id && p < buf+sizeof(buf)-10 ) {
	if (isprint(*id) && !strchr(BADCHARS, *id)) *p++ = *id;
	id++;
    }
    *p = '\0';
    if (mkdir(buf, 0700) == -1 && errno != EEXIST) {
	perror(buf);
	return 0;
    }
    *p++ = '/';
    *p = '\0';
    return buf;
}

/*
 * We are done with the directory returned by os_idtodir()
 * Remove it
 */
os_donewithdir(dir)
char *dir;
{
    char *p;

    /* Remove trailing slash */
    p = dir + strlen(dir) - 1;
    *p = '\0';

#ifndef VMS
    rmdir(dir);
#else
    delete(dir);
#endif
}

/*
 * Create a new file, with suggested filename "fname".
 * "fname" may have come from an insecure source, so clean it up first.
 * It may also be null.
 * "contentType" is passed in for use by systems that have typed filesystems.
 * "flags" contains a bit pattern describing attributes of the new file.
 */
FILE *os_newtypedfile(fname, contentType, flags, contentParams)
char *fname;
char *contentType;
int flags;
params contentParams;
{
    char *p,*p_v;
    static int filesuffix=0;
    char buf[128], *descfname=0;
    FILE *outfile = 0;
    int l_dot = 0;

    if (!fname) fname = "";

    /* If absolute path name, chop to tail */
    if (*fname == '/') {
	p = strrchr(fname, '/');
	fname = p+1;
    }

    /* Get rid of leading ~ or ~/ */
    while (*fname == '~' || *fname == '/') fname++;
    
    /* Clean out bad characters, create directories along path */
    for (p=fname; *p; p++) {
	if (*p == '/') {
	    if (!strncmp(p, "/../", 4)) {
		p[1] = p[2] = 'X';
	    }
	    *p = '\0';
	    (void) mkdir(fname, 0777);
	    *p = '/';
	}
#ifdef VMS
	/*	 
	**  allow only one dot in file spec
	*/	 
	else if (*p == '.')
	{
	    if (++l_dot > 1)
	    {
	    	 *p = 'X';
	    }
	}
#endif
	else if (!isprint(*p) || strchr(BADCHARS, *p)) *p = 'X';
    }

    if (!fname[0]) {
	do {
	    if (outfile) fclose(outfile);
	    sprintf(buf, "part%d", ++filesuffix);
	} while (outfile = fopen(buf, "r"));
	fname = buf;
    }
    else if (!overwrite_files && (outfile = fopen(fname, "r"))) {
	do {
	    fclose(outfile);
#ifndef VMS
	    sprintf(buf, "%s.%d", fname, ++filesuffix);
#else
	    /*	 
	    **  Version Making System .... allow also to parse version
	    */	 
	    sprintf(buf, "%s;%d", fname, ++filesuffix);
#endif
	 
	} while (outfile = fopen(buf, "r"));
	fname = buf;
    }

    outfile = fopen(fname, "w");
    if (!outfile) {
	perror(fname);
    }

    if (output_fname) free(output_fname);
    output_fname = strsave(fname);

    if (strlen(fname) > sizeof(buf)-6) {
	descfname = xmalloc(strlen(fname)+6);
    }
    else {
	descfname = buf;
    }
    strcpy(descfname, fname);

    p = strchr(descfname, '/');
    if (!p) p = descfname;
#ifndef VMS
    if (p = strrchr(p, '.')) *p = '\0';

    strcat(descfname, ".desc");
#else
    /*	 
    **  first searchs ';' to keep track of version
    **	then if no dot make an extension with '.desc'
    **	otherwise appends _desc to extension
    */	 
    if (p_v = strrchr(p, ';'))
    {
    	char t_buf[16];
	strcpy (t_buf, p_v);
	p_v = '\0';
	if (strrchr(p, '.'))
	{
	    strcat(descfname, "_desc");
	    strcat (descfname, t_buf);
	}
	else
	{
	    strcat(descfname, ".desc");
	    strcat (descfname, t_buf);
	}
    }
    else    /* no version */
    {
    	if (strrchr(p, '.'))
	{
	    strcat(descfname, "_desc");
	}
	else
	{
	    strcat(descfname, ".desc");
	}
    }
#endif

    (void) rename(TEMPFILENAME, descfname);
    if (descfname != buf) free(descfname);
    
    fprintf(stdout, "%s (%s)\n", output_fname, contentType);
    didchat = 1;

    return outfile;
}

/*
 * Close a file opened by os_newTypedFile()
 */
os_closetypedfile(outfile)
FILE *outfile;
{
    fclose(outfile);
}

/*
 * (Don't) Handle a BinHex'ed file
 */
int
os_binhex(inpart, part, nparts)
struct part *inpart;
int part;
int nparts;
{
    return 1;
}

/*
 * Warn user that the MD5 digest of the last file created by os_newtypedfile()
 * did not match that supplied in the Content-MD5: header.
 */
os_warnMD5mismatch()
{
    char *warning;

    warning = xmalloc(strlen(output_fname) + 100);
    sprintf(warning, "%s was corrupted in transit",
	    output_fname);
    warn(warning);
    free(warning);
}

/*
 * Report an error (in errno) concerning a filename
 */
os_perror(file)
char *file;
{
    perror(file);
}

#ifdef USE_GETHOSTNAME
/*
 *	gethostname(2) - VMS Version of the UNIX routine
 *
 *	This routine simply translates the system logical
 *	name SYS$NODE and returns the result. This does not
 *	exactly match UNIX behaviour. In particular, the
 *	double colon at the end of the node name may cause
 *	problems in ported code.
 */

#include <ssdef.h>
#include <lnmdef.h>
#include <descrip.h>
#include <errno.h>

#ifndef NULL
#define	NULL	0L
#endif


int gethostname(name, namelen)		/*--- Get node name ---*/
char name[];
int  namelen;
{
    /* Logical name to translate */
    char logical[] = "SYS$NODE";
    $DESCRIPTOR(logical_dsc, logical);

    /* These descriptors is necessary to select the logical name table */
    char sys_log_table[] = "LNM$SYSTEM_TABLE";
    $DESCRIPTOR(sys_log_table_dsc, sys_log_table);

    /* Item list as used by SYS$TRNLNM() */
    struct ITEM_LIST
    {
	unsigned short buflen;
	unsigned short code;
	void *buffaddr; /*+ +*/
	void *retaddr; /*+ +*/
    };

    struct ITEM_LIST log_list[2];

    unsigned short retlen;		/* Must be a WORD */

    /* 
	Buffer for the translation

	Max return length from SYS$TRNLNM is 255,
	but add a couple to be safe.
    */
    char string[257];

    int status, i;


    if (name == NULL)
    {
	errno = EFAULT;
	return (-1);
    }

    /* Setup the required item list for SYS$TRNLNM */

    log_list[0].buflen = 255;
    log_list[0].code   = LNM$_STRING;
    log_list[0].buffaddr = &string;
    log_list[0].retaddr  = &retlen;
    log_list[1].buflen = 0;
    log_list[1].code   = 0;

    /* Time to look for a logical name ... */

    status = SYS$TRNLNM(0, &sys_log_table_dsc,
		    &logical_dsc, 0, &log_list);

    if (status == SS$_NORMAL)
    {
	/* Got a translation */
	/* Copy the name back to the calling program */
	for (i=0; i<retlen && i<namelen; i++)
	{
	    name[i] = string[i];
	}
	if (i < namelen)
	    name[i] = 0;		/* Append a null */

	return (0);

    }
    else
    {
	errno = EFAULT;
	vaxc$errno = status;
	return (-1);
    }
}
#endif
