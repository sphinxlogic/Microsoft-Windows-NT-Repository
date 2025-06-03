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
#include <dos.h>
#include <bios.h>

#include "slang.h"
#include "_slang.h"

#ifdef __cplusplus
#define _DOTS_ ...
#else
#define _DOTS_ 
#endif

#ifndef __GO32__
static void interrupt (*oldint9)(_DOTS_);
static unsigned char far *shift = (unsigned char far *) 0x417;
static void interrupt int9_handler(_DOTS_)
{
   unsigned int scan = (unsigned int) 34;  /* 34 = scan code for ^G */
   unsigned char s, s1;
   

   s1 = *shift & 0xF;  /* ignoring caps, ins, num lock, scroll lock */
   s = inp(0x60);
   if (s1 & 0x04)      /* control key */
     {
	if (s == scan)
	  {
	     if (SLang_Ignore_User_Abort == 0) SLang_Error = USER_BREAK;
	     SLKeyBoard_Quit = 1;
	  }
     }
   (*oldint9)();
}
#endif

static void init_int9_handler(void)
{
#ifndef __GO32__
   oldint9 = getvect(9);
   setvect(9, int9_handler);
#endif
}

static void restore_int9_handler(void)
{
#ifndef __GO32__
   if (oldint9 != NULL) setvect(9, oldint9);
   oldint9 = NULL;
#endif
}


static void set_ctrl_break(int state)
{
   static int prev = 0;
#ifdef __GO32__
   if (state == 0)
     {
	prev = getcbrk ();
	setcbrk (0);
     }
   else setcbrk (prev);
#else
   asm {
      mov dl, byte ptr prev
      mov ax, state
      cmp ax, 0
      jne L1
      mov ah, 33h
      mov al, 0
      mov dl, byte ptr prev
      int 21h
      xor ax, ax
      mov al, dl
      mov prev, ax
      mov dl, 0
   }
   L1:
   asm {
      mov al, 1
      mov ah, 33h
      int 21h
   }
#endif
}

static unsigned int slbioskey (int x)
{
   union REGS in, out;
   in.h.ah = x;
   int86 (0x16, &in, &out);
   
   if (((x & 7) == 1) && (out.x.flags & 0x4)) return 0;
   return out.x.ax & 0xFFFF;
}

#define BIOSKEY(x) slbioskey((x) | bios_key_f)

static int bios_key_f;
int SLang_init_tty(int dum1, int dum2, int dum3)
{
   (void) dum2; (void) dum3;

#ifndef __GO32__
   bios_key_f = peekb(0x40,0x96) & 0x10; /* enhanced keyboard flag */
#else
   bios_key_f = 0x10; /* enhanced keyboard flag */
#endif
   
   set_ctrl_break (0);
   if (dum1) init_int9_handler ();
   
   /* clear keyboard buffer */
   /* while (BIOSKEY(1)) BIOSKEY(0); */
   
   return 0;
}

void SLang_reset_tty (void)
{
   restore_int9_handler();
   set_ctrl_break(1); 
}

/* sleep for *tsecs tenths of a sec waiting for input */
int SLsys_input_pending(int tsecs)
{
   int count = tsecs * 5;
   
   if (count)
     {
	while (count > 0)
	  {
	     delay(20);		       /* 20 ms or 1/50 sec */
	     if (BIOSKEY(1)) break;
	     count--;
	  }
	return(count);
     }
   else return(BIOSKEY(1));
}

unsigned int SLsys_getkey ()
{
   unsigned int c;
   unsigned int i;
   unsigned char scan;
   
   /* int tsecs = 300; */
   
   /* if (BIOSKEY(1) == 0) 
      while (!SLsys_input_pending(tsecs)); */
   
   i = BIOSKEY(0);
   c = i & 0xFF;
   scan = i >> 8;
   
   if ((c == 8) && (scan == 0x0e)) c = 127;
   if (c == 0xE0) c = 0;
   if (c == 0) SLang_ungetkey (scan);
   return (c);
}

void SLang_set_abort_signal (void (*dum)(int))
{
   (void) dum;
}

