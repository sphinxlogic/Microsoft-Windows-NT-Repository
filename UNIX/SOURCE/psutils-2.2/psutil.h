/* psutil.h
 * AJCD 29/1/91
 * utilities for PS programs
 */

#include <stdio.h>
#include <ctype.h>

#ifndef LOCAL
#define LOCAL extern
#endif

#define TMPDIR "/tmp"
#define MAXPAGES 5000 /* max pages in document */

LOCAL char *prog;
LOCAL long pageptr[MAXPAGES];
LOCAL int pages;
LOCAL int verbose;
LOCAL FILE *infile;
LOCAL FILE *outfile;

LOCAL FILE *seekable();
LOCAL int fcopy();
LOCAL writepage();
LOCAL seekpage();
LOCAL writepageheader();
LOCAL writepagebody();
LOCAL writeheader();
LOCAL writeprolog();
LOCAL writetrailer();
LOCAL writeemptypage();
LOCAL scanpages();
LOCAL writestring();
LOCAL message();

extern long lseek();
extern long ftell();
extern char *getenv();
