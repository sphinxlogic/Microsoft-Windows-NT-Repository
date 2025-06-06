/*
 * Copyright 1993 by Vrije Universiteit, The Netherlands
 * Copyright 1993 by David Dawes <dawes@physics.su.oz.au>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of the Vrije Universiteit and David 
 * Dawes not be used in advertising or publicity pertaining to 
 * distribution of the software without specific, written prior permission.
 * The Vrije Universiteit and David Dawes make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THE VRIJE UNIVERSITEIT AND DAVID DAWES DISCLAIM ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE VRIJE UNIVERSITEIT OR 
 * DAVID DAWES BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR 
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/os-support/amoeba/am_io.c,v 2.8 1993/10/02 07:15:14 dawes Exp $ */

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "inputstr.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "x386Procs.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"


Bool xf86SupportedMouseTypes[] =
{
	TRUE,	/* Microsoft */
	TRUE,	/* MouseSystems */
	TRUE,	/* MMSeries */
	TRUE,	/* Logitech */
	TRUE,	/* BusMouse */
	TRUE,	/* MouseMan */
	TRUE,	/* PS/2 */
	FALSE,	/* Hitachi Tablet */
};

int xf86NumMouseTypes = sizeof(xf86SupportedMouseTypes) /
			sizeof(xf86SupportedMouseTypes[0]);

void xf86SoundKbdBell(loudness, pitch, duration)
int loudness;
int pitch;
int duration;
{
	if (loudness)
	{
		errstat err;

		if ((err = iop_ringbell(&iopcap, loudness, pitch, duration))
		    != STD_OK)
		{
			FatalError("iop_ringbell failed (%s)\n", err_why(err));
		}
	}
}

void xf86SetKbdLeds(leds)
int leds;
{
	errstat err;

	if ((err = iop_setleds(&iopcap, (int)leds)) != STD_OK)
	{
		FatalError("iop_set_leds failed (%s)\n", err_why(err));
	}
}

int xf86GetKbdLeds()
{
	int cur_leds;
	errstat err;

	err = iop_getleds(&iopcap, &cur_leds);
	if (err != STD_OK)
	{
		FatalError("iop_get_leds failed (%s)\n", err_why(err));
	}
	return cur_leds;
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

void xf86KbdInit()
{
	return;
}

int xf86KbdOn()
{
	return(-1);
}

int xf86KbdOff()
{
	return(-1);
}

/* Amoeba doesn't use this */
void xf86KbdEvents()
{
	return;
}

void xf86SetMouseSpeed(old, new, cflag)
int old;
int new;
unsigned cflag;
{
	return;
}

void xf86MouseInit()
{
	return;
}

/* Xserver/iopsvr mouse type translation table: */
static int mtypes[] = {
	IOP_MOUSE_MS,		/* P_MS */
	IOP_MOUSE_MM,		/* P_MSC */
	IOP_MOUSE_MMS,		/* P_MM */
	IOP_MOUSE_LOGI,		/* P_LOGI */
	IOP_MOUSE_LB,		/* P_BM */
	IOP_MOUSE_LOGIMAN,	/* P_LOGIMAN */
	IOP_MOUSE_PS2		/* P_PS2 */
};

int xf86MouseOn()
{
	int msetype;
	errstat err;
	
	msetype = x386Info.mseType;
	if (msetype >= 0 && msetype < (sizeof(mtypes) / sizeof(mtypes[0]))) {
		/* translate */
		msetype = mtypes[msetype];
	}

	if ((err = iop_mousecontrol(&iopcap, msetype, x386Info.baudRate,
				    x386Info.sampleRate)) != STD_OK)
	{
		FatalError("iop_mousecontrol failed (%s)\n", err_why(err));
	}
	return(-1);
}

int xf86MouseOff(doclose)
Bool doclose;
{
	return -1;
}

/* Amoeba doesn't use this */
void xf86MouseEvents()
{
	return;
}
