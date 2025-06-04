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
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>
#include "xmalloc.h"
#include "common.h"

#include <UNIXLIB.H>	/* B. Z. Lederman Define getpid			*/
#include <SOCKET.H>	/* B. Z. Lederman Define gethostname		*/
#include <STDLIB.H>	/* B. Z. Lederman Define exit, free		*/
#include <TIME.H>	/* B. Z. Lederman Define time			*/

extern void warn();	/* B. Z. Lederman avoid compiler warning	*/

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

extern int errno;
/* extern char *malloc();   B. Z. Lederman use STDLIB definition	*/
extern char *getenv();

int overwrite_files = 0;
int didchat;
int outcount = 1;

/* The name of the file we're writing */
static char *output_fname = 0;

/* Characters that shouldn't be in filenames */
#define GOODCHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_-$.;"

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

	/* If we don't have a FQDN, try canonicalizing with gethostbyname */
	if (!strchr(hostname, '.')) {
	    hp = gethostbyname(hostname);
	    if (hp) {
		strcpy(hostname, hp->h_name);
	    }
	}
    }

    result = malloc(25+strlen(hostname));
    sprintf(result, "%d.%d@%s", pid, curtime++, hostname);
    return result;
}

/* Create and return directory for a message-id */
char *os_idtodir(id)
char *id;
{
    static char buf[4096];
    char *p;

    if (getenv("TMPDIR")) {
	strcpy(buf, getenv("TMPDIR:"));
    }
    else {
	strcpy(buf, "SYS$SCRATCH:");
    }
    return buf;
}

/*
 * We are done with the directory returned by os_idtodir()
 * In openVMS implementation just leave it alone - always use existing
 * directories.
 */
os_donewithdir(dir)
char *dir;
{}

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
    char *p;
    char *shortname;
    static int filesuffix=0;
    char buf[128], *descfname=0;
    FILE *outfile = 0;
    int dots = 0;

    if (!fname || strlen(fname) == 0){
      sprintf(fname,"PART.%03d",outcount);
      outcount++;
    }


    /*strip the filename down, removing any directory/disk information,
     do this by keeping only what is RIGHT of any of ":]/\" */

    for (shortname = fname + strlen(fname) + 1;
         *(shortname - 1) != ':' && 
         *(shortname - 1) != ']' && 
         *(shortname - 1) != '/' &&
         *(shortname - 1) != '\\' &&
         shortname > fname ;
         shortname--){}

    /* Second pass, right to left, eliminate invalid file characters
       and also reduce to single period*/

    for (p=shortname + strlen(shortname); p >= shortname ; p--) {
	if (!strchr(GOODCHARS, toupper(*p))) *p = '_';
	if (*p == '.') {
	   if (dots == 0) dots++;
           else *p = '_';
	}
    }

    /* third pass left to right, eliminate version number - could be a
       security hole */

    for (p=shortname ; *p != '\0' ; p++) { if (*p == ';') *p = '\0'; }

   /* New file will be opened ONLY if a file of the same name does
      not already exist.  This is a safety measure - it means that
      nobody can sneak a LOGIN.COM as text into a 400 part MIME
      posting.  If an attempt is made to create a higher numbered
      version than a warning is issued, and the user can override with
      the -o qualifier on the command line.*/

    if(outfile = fopen(shortname, "r")){
      fclose(outfile);
      if(!overwrite_files){
        printf("MUNPACK: FATAL ERROR: file exists: %s\n",shortname);
        exit(0);
        }
      else
        printf("MUNPACK: WARNING: creating newer version of %s\n"
                ,shortname);
      }
      outfile = fopen(shortname, "w");
      if (!outfile) {
 	  perror(shortname);
    }

    if (output_fname) free(output_fname);
    output_fname = strsave(shortname);

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
