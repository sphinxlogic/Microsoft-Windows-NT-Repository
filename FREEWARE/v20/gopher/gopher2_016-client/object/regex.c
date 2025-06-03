/********************************************************************
 * lindner
 * 3.1
 * 1993/10/19 20:48:23
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/Regex.c,v
 * $Status: $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: Regex.c
 * Portable method of doing regular expressions
 *********************************************************************
 * Revision History:
 * Regex.c,v
 * Revision 3.1  1993/10/19  20:48:23  lindner
 * Portable versions of Regular expression routines for System V and BSD..
 *
 *
 *********************************************************************/

/*
 * If you're using gcc on Solaris you might need to copy /usr/include/regexp.h
 * to /opt/whatever/regexp.h
 */

#define  REGEX_CODEIT    /* only include sysv regex code once.. */
#include "Regex.h"

#ifdef REGEX_SYSV

#define ESIZE 512
static char expbuf[ESIZE];

char *
re_comp(expr)
  char *expr;
{
     char *result;

     result = compile(expr, expbuf, &expbuf[ESIZE], '\0');

     return(NULL);
}

int
re_exec(string)
  char *string;
{
     return(step(string, expbuf));
}

#endif
