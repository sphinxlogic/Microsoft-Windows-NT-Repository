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

char *jed_memcpy(char *s1, char *s2, int n)
{
#ifdef msdos
   asm mov ax, ds
   asm mov bx, si
   asm mov dx, di
   asm mov cx, n
   asm les di, s1
   asm lds si, s2
   asm cld
   asm rep movsb
   asm mov ds, ax
   asm mov si, bx
   asm mov di, dx
   return(s1);
   
#else
   register char *smax, *s = s1;
   int n2;
   
   n2 = n % 4;
   smax = s + (n - 4);
   while(s <= smax) 
     {
	*s++ = *s2++; *s++ = *s2++; *s++ = *s2++; *s++ = *s2++;
     }
   while (n2--) *s++ = *s2++;
   return(s1); 
#endif
}
