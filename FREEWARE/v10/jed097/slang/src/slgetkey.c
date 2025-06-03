/* 
 * Copyright (c) 1994 John E. Davis
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.   Permission is not granted to modify this
 * software for any purpose without written agreement from John E. Davis.
 *
 * IN NO EVENT SHALL JOHN E. DAVIS BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JOHN E. DAVIS
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JOHN E. DAVIS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND JOHN E. DAVIS HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */
#include <stdio.h>
#include "slang.h"
#include "_slang.h"

int SLang_Input_Buffer_Len = 0;
unsigned char SLang_Input_Buffer [MAX_INPUT_BUFFER_LEN];

int SLang_Abort_Char = 7;
volatile int SLKeyBoard_Quit = 0;
int SLang_Ignore_User_Abort = 0;


/* This has the effect of mapping all characters in the range 128-169 to
 * ESC [ something 
 */
#ifndef __GO32__
#if defined(unix) || defined(vms)
#define DEC_8BIT_HACK 64
#endif
#endif

unsigned int SLang_getkey (void)
{
   int imax;
   unsigned int ch;
   
   if (SLang_Input_Buffer_Len)
     {
	ch = (unsigned int) *SLang_Input_Buffer;
	SLang_Input_Buffer_Len--;
	imax = SLang_Input_Buffer_Len;
   
	MEMCPY ((char *) SLang_Input_Buffer, 
		(char *) (SLang_Input_Buffer + 1), imax);
     }
   else if (0xFFFF == (ch = SLsys_getkey ())) return ch;
   
#ifdef DEC_8BIT_HACK
   if (ch & 0x80)
     {
	unsigned char i;
	i = (unsigned char) (ch & 0x7F);
	if (i < ' ')
	  {
	     i += DEC_8BIT_HACK;
	     SLang_ungetkey (i);
	     ch = 27;
	  }
     }
#endif
   return(ch);
}


void SLang_ungetkey_string (unsigned char *s, int n)
{
   register unsigned char *bmax, *b, *b1;
   if (SLang_Input_Buffer_Len > MAX_INPUT_BUFFER_LEN - 3 - n) return;

   b = SLang_Input_Buffer;
   bmax = b + (SLang_Input_Buffer_Len - 1);
   b1 = bmax + n;
   while (bmax >= b) *b1-- = *bmax--;
   bmax = b + n;
   while (b < bmax) *b++ = *s++;
   SLang_Input_Buffer_Len += n;
}

void SLang_buffer_keystring (unsigned char *s, int n)
{

   if (n + SLang_Input_Buffer_Len > MAX_INPUT_BUFFER_LEN - 3) return;
   
   MEMCPY ((char *) SLang_Input_Buffer + SLang_Input_Buffer_Len, 
	   (char *) s, n);
   SLang_Input_Buffer_Len += n;
}

void SLang_ungetkey (unsigned char ch)
{
   SLang_ungetkey_string(&ch, 1);
}

int SLang_input_pending (int tsecs)
{
   int n;
   unsigned char c;
   if (SLang_Input_Buffer_Len) return SLang_Input_Buffer_Len;
   
   n = SLsys_input_pending (tsecs);
   if (n)
     {
	c = (unsigned char) SLang_getkey ();
	SLang_ungetkey_string (&c, 1);
     }
   return n;
}

void SLang_flush_input (void)
{
   int quit = SLKeyBoard_Quit;
   
   SLang_Input_Buffer_Len = 0;
   SLKeyBoard_Quit = 0;   
   while (SLsys_input_pending (0) > 0) (void) SLsys_getkey ();
   SLKeyBoard_Quit = quit;
}
