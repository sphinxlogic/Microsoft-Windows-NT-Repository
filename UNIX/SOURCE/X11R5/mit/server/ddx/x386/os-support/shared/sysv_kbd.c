/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany
 * Copyright 1993 by David Dawes <dawes@physics.su.oz.au>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Thomas Roell and David Dawes 
 * not be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  Thomas Roell and
 * David Dawes makes no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * THOMAS ROELL AND DAVID DAWES DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THOMAS ROELL OR DAVID DAWES BE LIABLE FOR 
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/os-support/shared/sysv_kbd.c,v 2.2 1993/10/16 17:32:14 dawes Exp $ */

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "inputstr.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "x386Procs.h"
#include "xf86_OSlib.h"

int xf86GetKbdLeds()
{
	int leds;

	ioctl(x386Info.consoleFd, KDGETLED, &leds);
	return(leds);
}

#ifdef __STDC__
void xf86SetKbdRepeat(char rad)
#else
void xf86SetKbdRepeat(rad)
char rad;
#endif
{
#ifdef KDSETRAD
	ioctl(x386Info.consoleFd, KDSETRAD, rad);
#endif
}

static int kbdtrans;
static struct termio kbdtty;
static char *kbdemap = NULL;

void xf86KbdInit()
{
#ifdef KDGKBMODE
	ioctl (x386Info.consoleFd, KDGKBMODE, &kbdtrans);
#endif
	ioctl (x386Info.consoleFd, TCGETA, &kbdtty);
#ifdef E_TABSZ
	kbdemap = (char *)xalloc(E_TABSZ);
	if (ioctl(x386Info.consoleFd, LDGMAP, kbdemap) < 0)
	{
		xfree(kbdemap);
		kbdemap = NULL;
	}
#endif
}

int xf86KbdOn()
{
	struct termio nTty;

	ioctl(x386Info.consoleFd, KDSKBMODE, K_RAW);
	ioctl(x386Info.consoleFd, LDNMAP, 0); /* disable mapping completely */
	nTty = kbdtty;
	nTty.c_iflag = (IGNPAR | IGNBRK) & (~PARMRK) & (~ISTRIP);
	nTty.c_oflag = 0;
	nTty.c_cflag = CREAD | CS8 | B9600;
	nTty.c_lflag = 0;
	nTty.c_cc[VTIME]=0;
	nTty.c_cc[VMIN]=1;
	ioctl(x386Info.consoleFd, TCSETA, &nTty);
	return(x386Info.consoleFd);
}

int xf86KbdOff()
{
	if (kbdemap)
	{
		ioctl(x386Info.consoleFd, LDSMAP, kbdemap);
	}
	ioctl(x386Info.consoleFd, KDSKBMODE, kbdtrans);
	ioctl(x386Info.consoleFd, TCSETA, &kbdtty);
	return(x386Info.consoleFd);
}
