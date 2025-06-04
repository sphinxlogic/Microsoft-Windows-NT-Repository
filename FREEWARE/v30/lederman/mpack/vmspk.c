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
#include <string.h>
#include <errno.h>
#include "common.h"
#include "version.h"
#include "xmalloc.h"

#define MAXADDRESS 100

extern char *getenv();

extern int errno;
extern int optind;
extern char *optarg;

#include <stdlib.h>	/* for atoi, exit, etc. B. Z. Lederman		*/
#include <unixio.h>	/* for mktemp, etc. B. Z. Lederman		*/
#include <unistd.h>	/* for getopt.  B. Z. Lederman			*/

/* it would probably be better to find out how to trigger the
   definition in UNISTD.H, and check the difference in the defition of
   argv, but for now this should be o.k.  B. Z. Lederman		*/

int getopt ();

void usage();		/* avoid compiler warnings B. Z. Lederman	*/
extern int cistrncmp();	/* ditto */
extern int cistrcmp();
extern void os_perror();
extern int encode();
void inews (char *infile);
void sendmail (char *infile, char *addresses);

main(argc, argv)
int argc;
char **argv;
{
    int opt;
    char *fname = 0;
    char *shortname = 0;
    char *subject = 0;
    char *descfname = 0;
    long maxsize = 0;
    char *outfname = 0;
    char *scratchout = 0;
    char *newsgroups = 0;
    char *ctype = 0;
    char *headers = 0;
    char *addresses = 0;
    int i;
    char *p;
    char sbuf[1024];
    char fnamebuf[4096];
    int part;
    FILE *infile;
    FILE *descfile = 0;

    if ((p = getenv("SPLITSIZE")) && *p >= '0' && *p <= '9') {
	maxsize = atoi(p);
    }

    while ((opt = getopt(argc, argv, "s:d:m:c:o:n:")) != EOF) {
	switch (opt) {
	case 's':
	    subject = optarg;
	    break;

	case 'd':
	    descfname = optarg;
	    break;

	case 'm':
	    maxsize = atoi(optarg);
	    break;

	case 'c':
	    ctype = optarg;
	    break;

	case 'o':
	    outfname = optarg;
	    break;

	case 'n':
	    newsgroups = optarg;
	    break;

	default:
	    usage();

	}
    }

    if (ctype) {
	if (!cistrncmp(ctype, "text/", 5)) {
	    fprintf(stderr, "This program is not appropriate for encoding textual data\n");
	    exit(1);
	}
	if (cistrncmp(ctype, "application/", 12) && cistrncmp(ctype, "audio/", 6) &&
	    cistrncmp(ctype, "image/", 6) && cistrncmp(ctype, "video/", 6)) {
	    fprintf(stderr, "Content type must be subtype of application, audio, image, or video\n");
	    exit(1);
	}
    }

    if (optind == argc) {
	fprintf(stderr, "An input file must be specified\n");
	usage();
    }
    fname = argv[optind++];

    /* Must have exactly one of -o, -n, or destination addrs */
    if (optind == argc) {
	if (outfname && newsgroups) {
	    fprintf(stderr, "The -o and -n switches are mutually exclusive.\n");
	    usage();
	}
	if (!outfname && !newsgroups) {
	    fprintf(stderr, "Either an address or one of the -o or -n switches is required\n");
	    usage();
	}
	if (newsgroups) {
	    headers = xmalloc(strlen(newsgroups) + 25);
	    sprintf(headers, "Newsgroups: %s\n", newsgroups);
	}
    }
    else {
	if (outfname) {
	    fprintf(stderr, "The -o switch and addresses are mutually exclusive.\n");
	    usage();
	}
	if (newsgroups) {
	    fprintf(stderr, "The -n switch and addresses are mutually exclusive.\n");
	    usage();
	}
	headers = xmalloc(strlen(argv[optind]) + 25);
	sprintf(headers, "To: %s", argv[optind]);
	for (i = optind+1; i < argc; i++) {
	    headers = xrealloc(headers, strlen(headers)+strlen(argv[i]) + 25);
	    strcat(headers, ",\n\t");
	    strcat(headers, argv[i]);
	}
	strcat(headers, "\n");

	addresses = xmalloc(strlen(argv[optind]) + 25);
	sprintf(addresses, "%s", argv[optind]);
	for (i = optind+1; i < argc; i++) {
	    addresses = xrealloc(addresses, strlen(addresses)+strlen(argv[i]) + 25);
	    strcat(addresses, ",");
	    strcat(addresses, argv[i]);
	}
    }

    if (!subject) {
	fputs("Subject: ", stdout);
	fflush(stdout);
	if (!fgets(sbuf, sizeof(sbuf), stdin)) {
	    fprintf(stderr, "A subject is required\n");
	    usage();
	}
	if (p = strchr(sbuf, '\n')) *p = '\0';
	subject = sbuf;
    }	

    if (!outfname) {
	if (getenv("TMPDIR")) {
	    strcpy(fnamebuf, getenv("TMPDIR"));
	}
	else {
	    strcpy(fnamebuf, "SYS$SCRATCH:");
	}
	strcat(fnamebuf, "mpackXXXXXX");
	mktemp(fnamebuf);
	outfname = strsave(fnamebuf);
    }

    infile = fopen(fname, "r");
    if (!infile) {
	os_perror(fname);
	exit(1);
    }

    if (descfname) {
	descfile = fopen(descfname, "r");
	if (!descfile) {
	    os_perror(descfname);
	    exit(1);
	}
    }

/*strip the filename down to just the filename, remove everything
right of any of ":]/" all valid in file descriptors (since DECC).*/

    for (shortname = fname + strlen(fname) + 1;
         *(shortname - 1) != ':' && 
         *(shortname - 1) != ']' && 
         *(shortname - 1) != '/' &&
         shortname > fname ;
         shortname--){}

    if (encode(infile, (FILE *)0, shortname, descfile, subject, headers,
	       maxsize, ctype, outfname)) exit(1);

    if (optind < argc || newsgroups) {
	for (part = 0;;part++) {
	    sprintf(fnamebuf, "%s.%02d", outfname, part);
	    scratchout =  (part ? fnamebuf : outfname);
            if(fopen(scratchout,"r")) {
	      if (newsgroups) {
	 	inews(scratchout);
	      }
	    else {
		sendmail(scratchout, addresses);
	      }
	    }
	    else {
              if(part)break;
            }
	}
    }

    exit(0);
}

void usage()
{
    fprintf(stderr, "mpack version %s\n", MPACK_VERSION);
    fprintf(stderr, 
"usage: mpack [-s subj] [-d file] [-m maxsize] [-c content-type] file address...\n");
    fprintf(stderr, 
"       mpack [-s subj] [-d file] [-m maxsize] [-c content-type] -o file file\n");
    fprintf(stderr, 
"       mpack [-s subj] [-d file] [-m maxsize] [-c content-type] -n groups file\n");
    exit(1);
}

void sendmail(infile,addresses)
char *infile;
char *addresses;
{
    char scratch[1024];
    sprintf(scratch,"MMAIL %s \"%s\" ",infile,addresses);
    system(scratch);
}

void inews(infile)
char *infile;
{
    char scratch[1024];
    sprintf(scratch,"MNEWS %s",infile);
    system(scratch);
}

warn()
{
    abort();
}

