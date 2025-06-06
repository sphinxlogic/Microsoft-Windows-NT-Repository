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
#include <sys/param.h>
#include <netdb.h>
#include "xmalloc.h"
#include "common.h"

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
#define BADCHARS "!$&*()|\'\";<>[]{}?/`\\ \t"

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
	strcpy(buf, getenv("TMPDIR"));
    }
    else {
	strcpy(buf, "/usr/tmp");
    }
    strcat(buf, "/m-prts-");
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

    rmdir(dir);
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
    char *p;
    static int filesuffix=0;
    char buf[128], *descfname=0;
    FILE *outfile = 0;

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
	    sprintf(buf, "%s.%d", fname, ++filesuffix);
	 
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
    if (p = strrchr(p, '.')) *p = '\0';

    strcat(descfname, ".desc");
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
