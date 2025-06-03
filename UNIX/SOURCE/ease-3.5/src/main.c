#ifdef FLUKE
# ifndef LINT
    static char RCSid[] = "@(#)FLUKE  $Header: /home/kreskin/u0/barnett/Src/ease/src/RCS/main.c,v 3.2 1991/05/16 10:45:25 barnett Exp $";
# endif LINT
#endif FLUKE

/*
 *  	main.c     -- Main procedure for Ease Translator.
 *
 *  	author     -- James S. Schoner, Purdue University Computing Center
 *				        West Lafayette, Indiana  47907
 *
 *  	date       -- July 9, 1985
 *
 *	Copyright (c) 1985 by Purdue Research Foundation
 *
 *	All rights reserved.
 *
 * $Log: main.c,v $
 * Revision 3.2  1991/05/16  10:45:25  barnett
 * Better support for System V machines
 * Support for machines with read only text segments
 *
 * Revision 3.1  1991/02/25  22:09:52  barnett
 * Fixed some portability problems
 *
 * Revision 2.2  1990/05/07  11:14:02  jeff
 * Add support for the "-q" flag which controls whether input lines
 * are passed through as comments in the output stream.
 *
 * Version 2.1  90/01/30  15:37:16  jeff
 * Filter input file through cpp before processing it.
 * 
 * Revision 2.0  88/06/15  14:42:41  root
 * Baseline release for net posting. ADR.
 * 
 */


#ifndef CPP	/* filename of preprocessor */
#	define CPP	"/lib/cpp"
#endif	CPP

#ifndef CPPARGS		/* valid arguments  of preprocessor */
#ifdef sun		/* Sun's cpp has more options - I guess */
#	define CPPARGS	"BCHpPRTDIUY"
#else
#	define CPPARGS	"CDEIPU"
#endif sun
#endif	CPP

#include "fixstrings.h"
#include <stdio.h>
#include <ctype.h>
#ifdef SYSV
#define index strchr
#define rindex strrchr
#endif
#ifdef MALLOC_DEBUG
extern int malloc_debug();
extern int malloc_verify();
#endif /* MALLOC_DEBUG */

extern FILE *DIAGf;			/* diagnostic file */
char *infile = 0;			/* input file name */
char *outfile = 0;			/* output file name */
extern void InitError (), 
#ifndef	DATA_RW
	    InitStrOps (),
	    InitParser (),
#endif
	    InitSymbolTable (),
	    DefScan (),
	    FatalError (),
            PreLoad ();

int EchoInputAsComments = 1;		/* should input lines be echoed
					 * as comments?
					 */
int ErrorCount;				/* translation error count */
void GetArgs ();			/* gets arguments to "et"  */

#ifdef YYDEBUG
extern int yydebug;
#else
static int yydebug;
#endif

/*
 *	main () -- Main procedure for the Ease Translator et.  If no files are 
 *	       	   given as arguments to et, stdin is translated and written to 
 *	           stdout.  If one file is given, it is translated and written 
 *	           to stdout.  If two files are given, the first is translated
 *	           and written to the second.  If the first filename is "-",
 *	           standard input is assumed.  A translation is performed on 
 *	           valid Ease input only, producing a regular sendmail 
 *		   configuration file. 
 *
 */
void
main (argc, argv)
int argc;		/* argument count for "et"  */
char *argv[];		/* argument vector for "et" */
{
#ifdef MALLOC_DEBUG
        malloc_debug(1);
#endif MALLOC_DEBUG
	InitError ();			/* initialize error conditions */
#ifndef	DATA_RW
	InitParser ();
	InitStrOps();
	
#endif
	InitSymbolTable ();		/* initialize the symbol table */
	PreLoad ();			/* preload special identifiers */
	GetArgs (argc, argv);		/* set up argument files       */
	(void) yyparse ();		/* perform translation	       */
	if (fflush (stdout) == EOF)
		FatalError ("Cannot flush output stream/file", (char *) NULL);
	DefScan ();		        /* warn about undefined idents */
	if (ErrorCount)
		fprintf (DIAGf, "\n\n*** %d error(s) detected.\n", ErrorCount);
	exit (ErrorCount);
}


/*
 *	GetArgs () -- Processes arguments to the Ease translator "et".  The
 *		      arguments are files (margv) which may replace either/both
 *		      of the files standard input and standard output.  The 
 *		      following cases are possible:
 *			
 *		      -- et f.e f.cf
 *				Translate Ease file f.e and write result
 *				to f.cf.
 *
 *		      -- et f.e
 *				Translate Ease file f.e and write result to
 *				standard output.
 *
 *		      -- et - f.cf
 *				Translate standard input and write result to
 *				f.cf.
 *
 *		      -- et
 *				Translate standard input and write result to
 *				standard output.
 *
 *		      et also accepts arguments. These include the /lib/cpp arguments
 *		      and the -d argument for debugging grammars.
 *
 *		      Finally, a message indicating the volatility of the 
 *		      Ease output is written.
 *
 */
void
GetArgs (margc, margv)
register int   margc;		/* argument count  */
register char **margv;		/* argument vector */
{
     int	cppflags = 0;
     int	otherflags = 0;
     int arg;
 
     for (arg = 1; arg < margc; ++arg) {		/* scan arguments */
 	if (margv[arg][0] == '-') {		/* a flag?*/
 	    if (isalpha(margv[arg][1])) {	/* yes - a flag */
 		if (index(CPPARGS,margv[arg][1])) ++cppflags;	/* one belonging to the CPP */
 		else if (margv[arg][1] == 'd' ) yydebug = 1;
 		else if (margv[arg][1] == 'q' ) EchoInputAsComments = 0;
 		else ++otherflags;
 	    } else if (! margv[arg][1]) { 	/* this argument is just a '-' */
 		if ( (arg - yydebug - cppflags - otherflags) == 1 )
 		  infile = margv[arg];	
 		else if ( (arg - yydebug - cppflags - otherflags) == 2 )
 		  outfile = margv[arg];
 		else
 		  FatalError ("Usage: et [-%s] [infile [outfile]]", CPPARGS);
 	    } else {
 		FatalError ("Usage: et [-%s] [infile [outfile]]", CPPARGS);
 	    } /* end if a -argument */
 	} else {	/* a filename - i guess */
 	    if ( (arg - yydebug - cppflags - otherflags) == 1 )
 	      infile = margv[arg];	
 	    else if ( (arg - yydebug - cppflags - otherflags) == 2 )
 	      outfile = margv[arg];
 	    else
 	      FatalError ("Usage: et [-d] [-q] [-%s] [infile [outfile]]", CPPARGS);
 	} /* end if a filename argument */
     }	/* done with parsing all of the arguments */
     if (otherflags)
       FatalError ("Usage: et [-%s] [infile [outfile]]", CPPARGS);
     if (infile && strcmp(infile,"-") )
       if (freopen (infile, "r", stdin) == NULL)
 	FatalError ("Cannot open input stream/file:", infile);
     if (outfile && strcmp(outfile,"-") )
       if (freopen (outfile, "w", stdout) == NULL)
 	FatalError ("Cannot open output stream/file:", outfile);
     if (cppflags && cpp(margc,margv))
 	FatalError ("Cannot open preprocessor", CPP);
 
	printf ("###################################################\n");
	printf ("##                                               ##\n");
	printf ("##  WARNING: THIS FILE IS THE OUTPUT OF THE      ##\n");
	printf ("##           `EASE' PRECOMPILER FOR SENDMAIL     ##\n");
	printf ("##           CONFIGURATION FILES.                ##\n");
	printf ("##                                               ##\n");
	printf ("##           MAKE MODIFICATIONS TO THE SOURCE    ##\n");
	printf ("##           FILE ONLY.  CHANGES MADE DIRECTLY   ##\n");
	printf ("##           TO THIS FILE WILL DISAPPEAR THE     ##\n");
	printf ("##           NEXT TIME THAT EASE IS RUN.         ##\n");
	printf ("##                                               ##\n");
	printf ("##           $Revision: 3.2 $                    ##\n");
	printf ("##                                               ##\n");
	printf ("###################################################\n");
}

/* cpp preprocessor code
 * copied from Schreiner and Friedman's book:
 * Introduction to Compiler Construction with Unix
 *
 * Bruce Barnett
 */

int cpp(argc,argv)
     int argc;
     char **argv;
{
    char **argp, *cmd;
    extern FILE *yyin;	/* for lex input */
    extern FILE *popen();
    int i;

    for (i = 0, argp = argv; *++argp; )
      if (**argp == '-' &&
	  index(CPPARGS, (*argp)[1]))
	i+=strlen(*argp) + 1;
    if ( ! (cmd = (char *) calloc(i + sizeof CPP, sizeof(char))))
      return -1;	/* no room */
    (void ) strcpy(cmd,CPP);
    for (argp = argv; *++argp; )
      if (**argp == '-' &&
	  index(CPPARGS, (*argp)[1]))
	strcat(cmd, " "), strcat(cmd, *argp);
    if (yyin = popen(cmd,"r"))
      i = 0;	/* all's well */
    else
      i = -1;	/* no preprocessor */
#ifdef __bsdi__						    /* and possibly others */
    free(cmd);
#else
    cfree(cmd);
#endif
    return i;
}
