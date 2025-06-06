/*
 * Copyright 1992 by Rich Murphey <Rich@Rice.edu>
 * Copyright 1993 by David Dawes <dawes@physics.su.oz.au>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Rich Murphey and David Dawes 
 * not be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  Rich Murphey and
 * David Dawes make no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * RICH MURPHEY AND DAVID DAWES DISCLAIM ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL RICH MURPHEY OR DAVID DAWES BE LIABLE FOR 
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/os-support/bsdi/bsdi_io.c,v 2.6 1993/08/29 14:31:18 dawes Exp $ */

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
		int data[2];

		data[0] = pitch;
		data[1] = (duration * loudness) / 50;
		ioctl(x386Info.consoleFd, PCCONIOCBEEP, data);
	}
}

void xf86SetKbdLeds(leds)
int leds;
{
	ioctl(x386Info.consoleFd, PCCONIOCSETLED, &leds);
}

int xf86GetKbdLeds()
{
	return(0);
}

#if __STDC__
void xf86SetKbdRepeat(char rad)
#else
void xf86SetKbdRepeat(rad)
char rad;
#endif
{
	return;
}

static struct termio kbdtty;

void xf86KbdInit()
{
	tcgetattr(x386Info.consoleFd, &kbdtty);
}

int xf86KbdOn()
{
	struct termios nTty;

	nTty = kbdtty;
	nTty.c_iflag = IGNPAR | IGNBRK;
	nTty.c_oflag = 0;
	nTty.c_cflag = CREAD | CS8;
	nTty.c_lflag = 0;
	nTty.c_cc[VTIME] = 0;
	nTty.c_cc[VMIN] = 1;
	cfsetispeed(&nTty, 9600);
	cfsetospeed(&nTty, 9600);
	tcsetattr(x386Info.consoleFd, TCSANOW, &nTty);
	return(x386Info.consoleFd);
}

int xf86KbdOff()
{
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
