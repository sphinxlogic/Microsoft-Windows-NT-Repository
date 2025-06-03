/*	xcdbglog.c -- debug logging module for XC
	This file uses 4-character tabstops
*/

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#define NODEBUG 1	/* prevents xc.h from defining DEBUG */
#include "xc.h"

#if DEBUG
static FILE *dfp;

void
dbglog()
{
	long todnow;

	time(&todnow);
	if (!access("debug.log",0) && (dfp=fopen("debug.log","w")))
		setbuf(dfp, NIL(char)),
		fprintf(dfp,(char*)asctime(localtime(&todnow)));
	return;
}

Fputc(c, stream)
register c;
FILE *stream;
{
	if (!capture && dfp && c != '\r')
		fputc(c,dfp);
	return(fputc(c,stream));
}

Fputs(s, stream)
register char *s;
FILE *stream;
{
	if (!capture && dfp)
		fputs(s,dfp);
	return(fputs(s,stream));
}

Fprintf(stream, format, a, b, c, d, e, f, g)
FILE *stream;
char *format;
long a, b, c, d, e, f, g;
{
	if (!capture && dfp)
		fprintf(dfp,format,a,b,c,d,e,f,g);
	return(fprintf(stream,format,a,b,c,d,e,f,g));
}

#endif
