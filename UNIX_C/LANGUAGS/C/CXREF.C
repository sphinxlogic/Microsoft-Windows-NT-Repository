24-Oct-85 02:48:52-MDT,9218;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Thu 24 Oct 85 02:48:26-MDT
Received: from usenet by TGR.BRL.ARPA id a022958; 24 Oct 85 2:44 EDT
From: wirch@puff.uucp
Newsgroups: net.sources
Subject: C cross reference, of sorts...
Message-ID: <493@puff.UUCP>
Date: 24 Oct 85 01:13:02 GMT
To:       unix-sources@BRL-TGR.ARPA

/*
This program scans a C source file, and sends a list of symbols
    to the output file.  This list is comprised of function
    and variable declarations.  A kind of cross reference.

The program tries to be intelligent about declarations of variables,
    and keeps the whole declaration (with modifiers) on one line in the output.
    (so that the symbols "char" and "foo" are output on one line, not two.)

usage: 
	fun infile.ext		(Output is to infile.FUN)

    (if you don't like this redirection scheme, change it yourself.)

from John Foust, 17 Oct 85, Madison, Wis. in Lattice C V2.15 under PC-DOS.


Everyone is welcome to improve and distribute this program, for
any reason.  It would be nice if you posted your improved versions.

This needs improvements.  It could be more useful if it could get
a little more effort put into it...  Now, it's a hack to a hack,
but hopefully tossing it out to usenet will make it all better...

possible future improvements:

    o  maybe there is an easier way to scan source... like only
	watching comment and function starts and ends.
    o  scan .h files, too?
    o  parse #include "stdio.h" and #include <stdio.h> the same way,
	and don't echo the filename twice.
    o  higher intelligence in recognizing popular declarations,
	like main(argc,argv) int argc; char **argv;
    o  recognition of pointer '*' in declarations (now, it just outputs
	symbols, with no * & ** (*foo)() etc. part to the symbols
    o  buffering each function as it's parsed isn't necessary - a relic
	of fxu.c
    

* mainly modified from public domain Lattice utility FXU.C
* This is it's header, which is not accurate for this program.

* name         fxu -- function extract utility
*
* usage        fxu filename function
*
*              where "filename" is the name of a file containing
*              several C functions, and "function" is the name of
*              the particular function to be extracted.  If the
*              named function is found, then (1) standard input is
*              copied to the standard output until EOF, and (2) the
*              text of the named function is written to the standard
*              output.  The first option allows header information
*              to be prepended to the output file.
*
**/

#include "stdio.h"
#include "ctype.h"

#define MAX 16        /* maximum characters in function name */
#define MAXBUF 9000   /* maximum characters buffered between functions */

char strs[3][20];

char strbuf[60];

char *stpchr();

unsigned _stack = MAXBUF + 2000;	/* reserve sufficient stack */

main(argc, argv)
int argc;
char *argv[];
{
char infn[30],	/* file name strings */
    otfn[30];

FILE *sfp, *outf;

char *dot;

int c, 
    brace, 
    cnest, 
    nc;
int i, 
    ns, 
    copy, 
    inlit, 
    delim, 
    pc;

char symbol[MAX+1];
char text[MAXBUF];

    if (--argc) {	/* at least one argv[] */
	strcpy(infn,argv[1]);	/* infn is name.DOC */
	strcpy(otfn,argv[1]);	/* otfn is name.FUN */
	dot = stpchr( otfn, '.' );
	*dot = 0;
	strcat(otfn,".FUN");
	if (!(sfp=fopen(infn,"r"))) {
	    fprintf(stderr,"Open error on input file %s\n",infn);
	    exit(1);
	}
	if (!(outf=fopen(otfn,"w"))) {
	    fprintf(stderr,"Open error on output file %s\n",otfn);
	    exit(1);
	}
    }
    else {
	fputs("Usage: fxu infile.ext\n", stderr);
	fputs("Output is to infile.FUN\n",stderr);
	exit(1);
    }

    fprintf(outf," \n");
    fprintf(outf,"%s declared symbols:\n",argv[1]);
    fprintf(outf," \n");

    *strs[0] = *strs[1] = *strs[2] = 0;

    brace = cnest = nc = ns = copy = inlit = pc = 0;

    c = getc(sfp);        /* get first char */
    while (c != EOF) {                 /* scan through source file */
	if (ns == MAXBUF) {
	    fputs("Maximum buffer size exceeded\n", stderr);
	    exit(1);
	}
	if (copy == 0) {
	    if (brace == 0)
		text[ns++] = c;  /* save chars between functions */
	}
	else
	    if (putchar(c) == EOF) {
		fputs("Copy error\n", stderr);
		exit(1);
	    }
	if (c == '/') {             /* possible comment */
	    nc = 0;
	    if ((c = getc(sfp)) == '*') {
		++cnest;   /* bump nesting level */
		if (copy)
		    putchar(c);
		else
		    if (brace == 0) 
			text[ns++] = c;
		c = getc(sfp);
	    }
	continue;
    }
    if (cnest != 0) {             /* inside comment */
	if (c == '*') {
	    if ((c = getc(sfp)) == '/') {
		--cnest;       /* reduce nesting level */
		if (copy) 
		    putchar(c);
		else 
		    if (brace == 0)
			text[ns++] = c;
		c = getc(sfp);
	    }
	    continue;
	}
	nc = 0;
    }
    else 
	if (inlit) {               /* inside literal string */
	    if (c == '\\' && pc == '\\') 
		c = 0;
	    if (c == delim && pc != '\\') 
		inlit = 0;
	    pc = c;         /* save previous character */
	}
	else 
	    if (c == '\'' || c == '\"') {               /* enter literal string */
		inlit = 1;
		pc = 0;
		delim = c;
	    }
	    else 
		if (c == '{')
		    ++brace;
		else 
		    if (c == '}') {             /* right brace */
			nc = 0;
			if (--brace == 0)
			    if (copy == 0) 
				ns = 0;      /* reset save index if not found */
			    else {               /* copy complete */
				putchar('\n');
				exit(0);
			    }
		    }
		    else 
			if (brace == 0) {
			    /* symbol not started yet */
			    if (nc == 0) {
				if (iscsymf(c))	/* start new symbol */
				    symbol[nc++] = c;
			    }
			    else 
				if (iscsym(c) || c == '$')	/* continue symbol */
				    if (nc < MAX) 
					symbol[nc++] = c;
				    else 
					symbol[0] = '\0';
				else
				    if (nc != 0) {             /* end of current symbol */
					symbol[nc++] = '\0';

					/* move stack */
					strcpy(strs[0],strs[1]);
					strcpy(strs[1],strs[2]);
					strcpy(strs[2],symbol);

    /* if symbol is !well-known, print it with any logical others on stack */
					if (!wellknown(symbol)) {
					    buildbuf();
					    fprintf(outf,"%s\n",strbuf);
					}
    
					nc = 0;
				    }
	}
    c = getc(sfp);      /* get next char */
    }
    exit(1);
}

/* wellknown(symbol)
    determines if a symbol is a known modifier
*/

int wellknown(p)
char *p;
{
    if    (symcmp(p,"int")==0
	|| symcmp(p,"struct")==0
	|| symcmp(p,"long")==0
	|| symcmp(p,"char")==0
	|| symcmp(p,"unsigned")==0
	|| symcmp(p,"static")==0
	|| symcmp(p,"extern")==0
	|| symcmp(p,"struct")==0
	|| symcmp(p,"include")==0
	|| symcmp(p,"define")==0
	|| symcmp(p,"ifdef")==0
	|| symcmp(p,"endif")==0
/* you can stick more common symbols here */
    	)
	return(1);
    else
	return(0);
}

    
/* create a string strbuf[] of the most logical interpretation of symbols:
    strs[0..2] is a stack of last symbols encountered, with
	strs[2] being the newest, strs[0] the oldest.
    */

buildbuf()
{
int x;

    strbuf[0] = 0;
    
	/* [define] [symbol] */
    if (symcmp(strs[1],"define")==0) {
	strcat(strbuf,"#");
	strcat(strbuf,strs[1]);
	strcat(strbuf," ");
	strcat(strbuf,strs[2]);
    }
	/* [include] [symbol] [h] */
    else if (symcmp(strs[0],"include")==0 && symcmp(strs[2],"h")==0) {
	strcat(strbuf,"#");
	strcat(strbuf,strs[0]);
	strcat(strbuf," ");
	strcat(strbuf,strs[1]);
	strcat(strbuf,".");
	strcat(strbuf,strs[2]);
    }
	/* [ifdef|undef] [symbol] */
    else if (symcmp(&strs[1][2],"def")==0) {
	strcat(strbuf,"#");
	strcat(strbuf,strs[1]);
	strcat(strbuf," ");
	strcat(strbuf,strs[2]);
    }
	/* [[extern|static]] [char|int|long|unsigned] [symbol] */
    else if ( symcmp(strs[1],"char")==0
		    || symcmp(strs[1],"int")==0
		    || symcmp(strs[1],"unsigned")==0
		    || symcmp(strs[1],"long")==0 ) {
	    if ( symcmp(strs[0],"extern")==0 
		    || symcmp(strs[0],"extern")==0 ) {
		strcat(strbuf,strs[0]);
		strcat(strbuf," ");
	    }
	strcat(strbuf,strs[1]);
	strcat(strbuf," ");
	strcat(strbuf,strs[2]);
    }
    else if ( symcmp(strs[1],"struct")==0 ) {
	strcat(strbuf,strs[1]);
	strcat(strbuf," ");
	strcat(strbuf,strs[2]);
    }
    else	/* unknown, so return latest symbol */
	strcat(strbuf,strs[2]);
}


/**
*
* name         symcmp -- compare symbols
*
* synopsis     retval = symcmp(p, q);
*              int retval;            return code: 0 if equal, 1 if not
*              char *p, *q;           symbols to be compared
*
* description  This function compares two alphanumeric symbols for
*              identity.  Upper and lower case alphabetic characters
*              are considered equivalent by this function.
*
**/

int symcmp(p,q)
char *p, *q;
{
int a, b;

    while ((toupper(*p)) == (toupper(*q))) {
	if (*p == '\0') 
	    return(0);
	p++;
	q++;
    }
    return(1);
}
