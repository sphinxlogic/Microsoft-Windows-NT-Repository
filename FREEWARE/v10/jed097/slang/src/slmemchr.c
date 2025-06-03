/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */


/* These routines are fast memcpy, memset routines.  When available, I
   use system rouines.  For msdos, I use inline assembly. */

/* The current versions only work in the forward direction only!! */
      
#include <stdio.h>
#include "slang.h"
#include "_slang.h"

char *jed_memchr(register char *p, register char c, register int n)
{
   int n2;
   register char *pmax;
   n2 = n % 32;
   pmax = p + (n - 32);
	
   while(p <= pmax)
     {
	if ((*p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c)
	    || (*++p == c))
	  {
	     return(p);
	  }
	p++;
     }
   
   while(n2--)
     {
	if (*p++ == c)
	  {
	     return p - 1;
	  }
     }
   return(NULL);
}
