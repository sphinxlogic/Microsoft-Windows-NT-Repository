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
#include "version.h"
#include "part.h"

#include <stdlib.h>	/* for atoi, exit, etc. B. Z. Lederman		*/
#include <unixio.h>	/* for mktemp, etc. B. Z. Lederman		*/

/* it would probably be better to find out how to trigger the
   definition in UNISTD.H, and check the difference in the defition of
   argv, but for now this should be o.k.  B. Z. Lederman		*/

int getopt ();

void usage();		/* avoid compiler warnings B. Z. Lederman	*/



extern int handleMessage();

extern int optind;
extern char *optarg;

extern int overwrite_files;
extern int didchat;
int quiet;

main(argc, argv)
int argc;
char **argv;
{
    int opt;
    FILE *file;
    int extractText = 0;
    
    while ((opt = getopt(argc, argv, "qftC:")) != EOF) {
	switch (opt) {
	case 'f':
	    overwrite_files = 1;
	    break;

	case 'q':
	    quiet = 1;
	    break;

	case 't':
	    extractText = 1;
	    break;

	case 'C':
	    if (chdir(optarg)) {
		perror(optarg);
		exit(1);
	    }
	    break;

	default:
	    usage();
	}
    }

    if (optind == argc) {
	fprintf(stderr, "munpack: reading from standard input\n");
	didchat = 0;
	handleMessage(part_init(stdin), "text/plain", 0, extractText);
	if (!didchat) {
	    fprintf(stdout,
		    "Did not find anything to unpack from standard input\n");
	}
	exit(0);
    }

    while (argv[optind]) {
	file = fopen(argv[optind], "r");
	if (!file) {
	    perror(argv[optind]);
	}
	else {
	    didchat = 0;
	    handleMessage(part_init(file), "text/plain", 0, extractText);
	    fclose(file);
	    if (!didchat) {
		fprintf(stdout, 
			"Did not find anything to unpack from %s\n",
			argv[optind]);
	    }
	}
	optind++;
    }
    exit(0);
}

void usage() {
    fprintf(stderr, "munpack version %s\n", MPACK_VERSION);
    fprintf(stderr, "usage: munpack [-f] [-q] [-C directory] [files...]\n");
    exit(1);
}

warn(s)
char *s;
{
    fprintf(stderr, "munpack: warning: %s\n", s);
}

chat(s)
char *s;
{
    didchat = 1;
    if (!quiet) fprintf(stdout, "%s\n", s);
}
