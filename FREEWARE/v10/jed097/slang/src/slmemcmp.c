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

/* This is an UNSIGNED comparison designed for systems that either do not have
* this function or performed a signed comparison (SunOS)
*/
int jed_memcmp(char *s1, char *s2, int n)
{
   int cmp;
   while(n--)
     {
	cmp = (unsigned char) *s1++ - (unsigned char) *s2++;
	if (cmp) return(cmp);
     }
   return(0);
}
