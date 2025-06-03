/*
 * texi2roff.c - mainline for Texinfo to nroff/troff translator texi2roff
 * 		Release 1.0a 	August 1988
 *
 * Copyright 1988 Beverly A.Erlebacher
 * erlebach@csri.toronto.edu	...uunet!utai!utcsri!erlebach
 *
 */

#include <stdio.h>
#ifndef __TURBOC__
#include <sys/types.h>
#endif
#include <sys/stat.h>
#include "texi2roff.h"

char *progname;
int transparent = NO;	/* for -t flag */

/*
 * main - parse arguments, handle options
 *	- initialize tables and other strings
 * 	- open files and pass them to process().
 */
main(argc, argv)
int argc;
char *argv[];
{
    int c, errflg = 0;
    FILE *in;
    char *inname;
    int macropkg = NONE;	/* user's choice of MS, ME or MM */
    int showInfo = NO; 	/* for -I flag: whether to display Info file material*/

    extern int optind;
    extern char *optarg;
    extern int process();
    extern void initialize();

    progname = argv[0];

    while ((c = getopt(argc, argv, "tIm:--")) != EOF)
	switch (c) {
  	case 't':
  	    transparent = YES;
  	    break;
	case 'I':
	    showInfo = YES;
	    break;
	case 'm':
	    if (macropkg != NONE) {
		errflg++;
	    } else {
		switch ( (char) *optarg) {
		case 's':
		    macropkg = MS;
		    break;
		case 'm':
		    macropkg = MM;
		    break;
		case 'e':
		    macropkg = ME;
		    break;
		default:
		    errflg++;
		    break;
		}
	    }
	    break;
	case '?':
	    errflg++;
	    break;
	}

    if (macropkg == NONE) {
	errflg++;
	}
    if (errflg) {
	(void) fprintf(stderr, "Usage: %s [-It] -m{e|m|s} [file ... ]\n",
								progname);
	exit(1);
    }

    (void) initialize(macropkg, showInfo);

    if (optind >= argc) {
	errflg += process(stdin, "stdin");
	}
    else
	for (; optind < argc; optind++) {
	    if (STREQ(argv[optind], "-")) {
		inname = "stdin";
		in = stdin;
		}
	    else {
		if (( in = fopen(argv[optind], "r")) == NULL) {
		    fprintf(stderr,"%s : can't open file %s\n",
			    progname, argv[optind]);
		    continue;
		}
		inname = argv[optind];
	    }
	    errflg += process(in, inname);
	    if (in != stdin)
		(void) fclose(in);
	}
    exit(errflg);
}

/*
 * process -  check opened files and pass them to translate().
 *	   -  report on disastrous translation failures
 */
int
process(fp, filename)
    FILE *fp;
    char *filename;
{
    struct stat statbuf;
    extern int translate(/* FILE *, char * */);

    if (fstat(fileno(fp), &statbuf) != 0){
	fprintf(stderr,"%s : can't fstat file %s\n", progname, filename);
	return 1;
    }
    if ((statbuf.st_mode & S_IFMT)==S_IFDIR) {
	(void) fprintf(stderr, "%s : %s is a directory\n", progname, filename);
	return 1;
    }
    /* translate returns 0 (ok) or -1 (disaster). to recover from a 
     * disaster and continue with next file would require fixing up the
     * state in translate.c, and the document would likely be useless anyway,
     * since it would have to be incredibly corrupted to blow stacks.
     */
    if (translate(fp, filename) < 0) {
	(void) fprintf(stderr,
		"%s: error while processing file %s, translation aborted\n",
		progname, filename);
	exit(1); 
    }
    return 0;
}
