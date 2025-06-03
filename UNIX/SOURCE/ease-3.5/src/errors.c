#ifdef FLUKE
# ifndef LINT
    static char RCSid[] = "@(#)FLUKE  $Header: /tmp_mnt/home/kreskin/u0/barnett/Src/Ease/ease/src/RCS/errors.c,v 2.3 1991/09/09 16:33:23 barnett Exp $";
# endif LINT
#endif FLUKE

/*
 *  	errors.c   -- Contains error initialization and reporting routines.
 *
 *  	author     -- James S. Schoner, Purdue University Computing Center,
 *				        West Lafayette, Indiana  47907
 *
 *  	date       -- July 9, 1985
 *
 *	Copyright (c) 1985 by Purdue Research Foundation
 *
 *	All rights reserved.
 *
 * $Log: errors.c,v $
 * Revision 2.3  1991/09/09  16:33:23  barnett
 * Minor bug fix release
 *
 * Revision 2.2  1991/05/16  10:45:25  barnett
 * Better support for System V machines
 * Support for machines with read only text segments
 *
 * Revision 2.1  1990/01/30  14:17:29  jeff
 * Bruce Barnett - extensions for SunOS/Ultrix.
 *
 * Revision 2.0  88/06/15  14:41:10  root
 * Baseline release for net posting. ADR.
 */

#include <stdio.h>
#include "fixstrings.h"
#include <ctype.h>
extern int  ErrorCount;	 /* error count	               */
extern char FNbuf[];	 /* input file name   	       */
extern int  Lcount;	 /* line count	     	       */
FILE *DIAGf = {stderr};  /* file for diagnostic output */
extern char yytext[];	/* current token */
extern int yyleng;	/* and it's length */
#ifndef USE_FLEX
extern int	yylineno;	/* current input line number */
#endif
extern char *infile;		/* input file name */
static char *source;

/*
 * yywhere() -- input position for yyparse()
 * from Schreiner and Friedman's book on compiler construction
 */
void
yywhere() 	/* position stamp */
{
    char	colon = 0;	/* flag */
    if (source && *source && strcmp(source,"\"\"")) {
	char	*cp = source;
	int	len = strlen(source);
	
	if (*cp == '"')
	  ++cp, len -= 2;
	if (strlen(cp, "./", 2) == 0)
	  cp += 2, len -= 2;
	if (len > 0 )
	  fprintf(DIAGf, "\"%.*s\"", len, cp);
	colon = 1;
    } 	else if (infile && strcmp(infile,"-")) {
	  fprintf(DIAGf, "\"%s\"",infile);
	  colon = 1;
      }
#ifndef USE_FLEX					    /* Flex can't do this (Greg Lehey, LEMIS, 19 August 1993 */
    if (yylineno > 0 ) {
	if (colon)
	  fputs(", ",DIAGf);
	fprintf(DIAGf, "line %d",
		yylineno - (*yytext == '\n' || ! *yytext));
	colon = 1;
/*	if ((yylineno - (*yytext == '\n' || ! *yytext)) != Lcount)
	  fprintf(DIAGf, "?%d:?", Lcount); */
    }
#endif
    if (*yytext) {
	register int i;
	for (i=0;i<20;++i)
	  if (!yytext[i] || yytext[i] == '\n')
	    break;
	if (i) {
	    if (colon)
	      putc(' ',DIAGf);
	    fprintf(DIAGf, "near \"%.*s\"",i,yytext);
	    colon = 1;
	}
    }
    if (colon)
      fputs(": ",DIAGf);
    fflush(DIAGf);
    fflush(stdout);
    fflush(stderr); /* just to make sure */
}


/*
 *	yymark - keep track of source file and line number 
 */

void
yymark()	/* retreive from '# digit text' */
{
    if (source)
#ifdef __bsdi__						    /* and possibly others (Grog, LEMIS, 19 August 1993) */
      free(source); 
#else
      cfree(source); 
#endif
    source = (char *) calloc(yyleng,sizeof(char));
#ifndef USE_FLEX					    /* no yylineno in Flex (Grog, LEMIS, 19 August 1993 */
    if (source) {
      sscanf(yytext, "# %d%s",&yylineno, source);
/*      fprintf(stderr,"source = '%s' on %s",source,yytext); */
      Lcount = yylineno;
      if (strcmp(source,"\"\""))
	strcpy(FNbuf,source);
      else if ( infile && strcmp(infile,"-"))
	sprintf(FNbuf,"\"%s\"",infile);
/*      fprintf(stderr,"FNbuf = '%s', infile = '%s'\n",FNbuf,infile); */
	       
  }
#endif
}



/*
 *	yyerror () -- Prints source file name (FNbuf), line number (Lcount),
 *		      and error message (sbErr) for each invokation.
 *		      it also prints out a message where the error is.
 *
 */
void
yyerror (sbErr)
char *sbErr;
{
    extern int yynerrs;
    ++ErrorCount;
    yywhere();
/*    fprintf(DIAGf, " %s\t[error %d]\n", sbErr, ErrorCount); */
    fprintf(DIAGf, " %s\n", sbErr);
/* yynerrs is the number of yacc errors, ErrorCount is larger */
}



/*
 *	ErrorReport () -- Prints source file name (FNbuf), line number (Lcount),
 *			  and error message (sbErr) for each invokation.
 *
 */
void
ErrorReport (sbErr)
char *sbErr;
{
/*	fprintf (DIAGf, "%s, line %d: %s", FNbuf, Lcount, sbErr);
	ErrorCount++; */
    yyerror(sbErr);
}


/*
 *	FatalError () -- Translator fatal error routine which prints 
 *			 error message (sbErr) and an argument (sbArg).
 *
 */
void
FatalError (sbErr, sbArg)
char *sbErr,
     *sbArg;
{
	fprintf (DIAGf, "%s, line %d: Fatal Error In Translator: %s %s\n", 
		 FNbuf, Lcount, sbErr, sbArg); 
	fflush(stderr);
	exit (1);
}


/*
 *	PrintError () -- Prints source file name (FNbuf), line number
 *			 (cline), error message (sbErr), and argument
 *			 (sbArg) for each invokation.
 *
 */
void
PrintError (sbErr, sbArg)
char *sbErr;
char *sbArg;
{
    char	Ebuffer[1000];
    sprintf(Ebuffer,sbErr,sbArg);
    yyerror(Ebuffer);
/*    fprintf (DIAGf, "%s, line %d: %s %s.\n", FNbuf, Lcount, sbErr, sbArg);
	ErrorCount++; */
}


/*
 *	PrintWarning () -- Prints a warning message with source file
 *			   name (FNbuf), line number (Lcount), warning
 *			   (sbWarn), and a possible identifier (sbID).
 *
 */
void
PrintWarning (sbWarn, sbID)
char *sbWarn;
char *sbID;
{
/*	fprintf (DIAGf, "%s, line %d: Warning: ", FNbuf, Lcount); */
        yywhere();
	fprintf(DIAGf,"Warning: ");
	if (sbID != NULL)
		fprintf (DIAGf, sbWarn, sbID);
	else
		fprintf (DIAGf, sbWarn);
}


/*
 *	InitError () -- Initialize line count (Lcount) to one and error count
 *		        (ErrorCount) to zero.
 *
 */
void
InitError ()
{
	Lcount     = 1;
	ErrorCount = 0;
}
