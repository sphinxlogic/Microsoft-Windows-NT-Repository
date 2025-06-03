/*
 * Copyright 1992 by Orest Zborowski <obz@Kodak.com>
 * Copyright 1993 by David Dawes <dawes@physics.su.oz.au>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Orest Zborowski and David Dawes 
 * not be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  Orest Zborowski
 * and David Dawes make no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * OREST ZBOROWSKI AND DAVID DAWES DISCLAIMS ALL WARRANTIES WITH REGARD 
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL OREST ZBOROWSKI OR DAVID DAWES BE LIABLE 
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN 
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF 
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/os-support/linux/lnx_io.c,v 2.3 1993/08/08 06:42:35 dawes Exp $ */

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "inputstr.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "x386Procs.h"
#include "xf86_OSlib.h"

void xf86SoundKbdBell(loudness, pitch, duration)
int loudness;
int pitch;
int duration;
{
	if (loudness && pitch)
	{
		ioctl(x386Info.consoleFd, KDMKTONE,
		      ((1193190 / pitch) & 0xffff) |
		      (((unsigned long)duration *
			loudness / 50) << 16));
	}
}

void xf86SetKbdLeds(leds)
int leds;
{
	ioctl(x386Info.consoleFd, KDSETLED, leds);
}

int xf86GetKbdLeds()
{
	int leds;

	ioctl(x386Info.consoleFd, KDGETLED, &leds);
	return(leds);
}

void xf86SetKbdRepeat(rad)
char rad;
{
	return;
}

static int kbdtrans;
static struct termios kbdtty;

void xf86KbdInit()
{
	ioctl (x386Info.consoleFd, KDGKBMODE, &kbdtrans);
	tcgetattr (x386Info.consoleFd, &kbdtty);
}

int xf86KbdOn()
{
	struct termios nTty;

	ioctl(x386Info.consoleFd, KDSKBMODE, K_RAW);
	nTty = kbdtty;
	nTty.c_iflag = (IGNPAR | IGNBRK) & (~PARMRK) & (~ISTRIP);
	nTty.c_oflag = 0;
	nTty.c_cflag = CREAD | CS8;
	nTty.c_lflag = 0;
	nTty.c_cc[VTIME]=0;
	nTty.c_cc[VMIN]=1;
	cfsetispeed(&nTty, 9600);
	cfsetospeed(&nTty, 9600);
	tcsetattr(x386Info.consoleFd, TCSANOW, &nTty);
	return(x386Info.consoleFd);
}

int xf86KbdOff()
{
	ioctl(x386Info.consoleFd, KDSKBMODE, kbdtrans);
	tcsetattr(x386Info.consoleFd, TCSANOW, &kbdtty);
	return(x386Info.consoleFd);
}

void xf86MouseInit()
{
	return;
}

int xf86MouseOn()
{
	if ((x386Info.mseFd = open(x386Info.mseDevice, O_RDWR | O_NDELAY)) < 0)
	{
		FatalError("Cannot open mouse (%s)\n", strerror(errno));
	}

	xf86SetupMouse();

	/* Flush any pending input */
	tcflush(x386Info.mseFd, TCIFLUSH);

	return(x386Info.mseFd);
}
