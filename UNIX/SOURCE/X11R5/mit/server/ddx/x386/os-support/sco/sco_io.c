/*
 * Copyright 1993 by David McCullough <davidm@stallion.oz.au>
 * Copyright 1993 by David Dawes <dawes@physics.su.oz.au>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David McCullough and David Dawes 
 * not be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  David McCullough
 * and David Dawes makes no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * DAVID MCCULLOUGH AND DAVID DAWES DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL DAVID MCCULLOUGH OR DAVID DAWES BE LIABLE FOR 
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/os-support/sco/sco_io.c,v 2.7 1993/08/24 16:17:40 dawes Exp $ */

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "inputstr.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "x386Procs.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"

void xf86SoundKbdBell(loudness, pitch, duration)
int loudness;
int pitch;
int duration;
{
	if (loudness && pitch)
	{
		ioctl(x386Info.consoleFd, KIOCSOUND, 1193180 / pitch);
		usleep(duration * loudness * 20);
		ioctl(x386Info.consoleFd, KIOCSOUND, 0);
	}
}

void xf86SetKbdLeds(leds)
int leds;
{
	/*
	 * sleep the first time through under SCO.  There appears to be a
	 * timing problem in the driver which causes the keyboard to be lost.
	 * This sleep stops it from occurring.  The sleep could proably be
	 * a lot shorter as even trace can fix the problem.  You may
	 * prefer a usleep(100).
	 */
	static int once = 1;

	if (once)
	{
		sleep(1);
		once = 0;
	}
	ioctl(x386Info.consoleFd, KDSETLED, leds );
}

void xf86MouseInit()
{
	if ((x386Info.mseFd = open(x386Info.mseDevice, O_RDWR | O_NDELAY)) < 0)
	{
		FatalError("Cannot open mouse (%s)\n", strerror(errno));
	}
}

int xf86MouseOn()
{
	xf86SetupMouse();

	/* Flush any pending input */
	ioctl(x386Info.mseFd, TCFLSH, 0);

	return(x386Info.mseFd);
}

int xf86MouseOff(doclose)
Bool doclose;
{
	if (x386Info.mseFd >= 0)
	{
		if (x386Info.mseType == P_LOGI)
		{
			write(x386Info.mseFd, "U", 1);
			xf86SetMouseSpeed(x386Info.baudRate, 1200,
				  	  xf86MouseCflags[P_LOGI]);
		}
		if (doclose)
		{
			close(x386Info.mseFd);
		}
	}
	return(x386Info.mseFd);
}
