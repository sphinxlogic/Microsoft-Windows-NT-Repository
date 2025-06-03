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

void jed_memset(char *p, char space, int n)
{
#ifndef msdos
   register char *pmax;

   pmax = p + (n - 4);
   n = n % 4;
   while(p <= pmax) 
     {
	*p++ = space; *p++ = space; *p++ = space; *p++ = space; 
     }
   while (n--) *p++ = space;
#else
   asm mov al, space
   asm mov dx, di
   asm mov cx, n
   asm les di, p
   asm cld
   asm rep stosb
   asm mov di, dx
#endif
}
