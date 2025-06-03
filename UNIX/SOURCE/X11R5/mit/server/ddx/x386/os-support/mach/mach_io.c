/*
 * Copyright 1992 by Robert Baron <Robert.Baron@ernst.mach.cs.cmu.edu>
 * Copyright 1993 by David Dawes <dawes@physics.su.oz.au>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Robert Baron and David Dawes not 
 * be used in advertising or publicity pertaining to distribution of the 
 * software without specific, written prior permission.  Robert Baron and 
 * David Dawes make no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * ROBERT BARON AND DAVID DAWES DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL ROBERT BARON OR DAVID DAWES BE LIABLE FOR 
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/os-support/mach/mach_io.c,v 2.9 1993/08/18 16:41:55 dawes Exp $ */

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "inputstr.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "x386Procs.h"
#include "xf86_OSlib.h"

Bool xf86SupportedMouseTypes[] =
{
	TRUE,	/* Microsoft */
	FALSE,	/* MouseSystems */
	FALSE,	/* MMSeries */
	TRUE,	/* Logitech */
	FALSE,	/* BusMouse */
	FALSE,	/* MouseMan */
	TRUE,	/* PS/2 */
	FALSE,	/* Hitachi Tablet */
};

int xf86NumMouseTypes = sizeof(xf86SupportedMouseTypes) /
			sizeof(xf86SupportedMouseTypes[0]);

unsigned short xf86MouseCflags[] =
{
	/*
	 * MicroSoft mouse needs 7bit characters.
	 * See special seven_bit_hack code in com.c:comparam()
	 * in the kernel code.  Basically we use the
	 * special code RAW | PASS8 to mean the mythical
	 * "PASS7".
	 */
	-1,			/* MicroSoft */
	RAW,			/* MouseSystems */
	RAW,			/* MMSeries */
	RAW | EVENP | ODDP,	/* Logitech */
	0,			/* BusMouse */
	-1,			/* ??? MouseMan [CHRIS-211092] */
	0,			/* PS/2 */
	RAW,			/* Hitachi Tablet */
};

void xf86SoundKbdBell(loudness, pitch, duration)
int loudness;
int pitch;
int duration;
{
	if (loudness)
	{
#ifdef __OSF__
		ioctl(x386Info.consoleFd, KIOCSOUND, 1);
		usleep(duration * loudness * 20);
		ioctl(x386Info.consoleFd, KIOCSOUND, 0);
#else
		int i = KD_BELLON;
		ioctl(x386Info.consoleFd, KDSETBELL, &i);
		usleep(duration * loudness * 20);
		i = KD_BELLOFF;
		ioctl(x386Info.consoleFd, KDSETBELL, &i);
#endif
	}
}

void xf86SetKbdLeds(leds)
int leds;
{
#ifdef __OSF__
	ioctl(x386Info.kbdFd, KDSETLED, &leds);
#endif
}

int xf86GetKbdLeds()
{
#ifdef __OSF__
	int leds;

	ioctl(x386Info.kbdFd, KDGETLED, &leds);
	return(leds);
#else
	return(0);
#endif
}

void xf86SetKbdRepeat(rad)
char rad;
{
	return;
}

void xf86KbdInit()
{
	return;
}

int xf86KbdOn()
{
	int data = KB_EVENT;

	if ((x386Info.kbdFd = open ("/dev/kbd", O_RDONLY)) < 0)
		FatalError("can't open /dev/kbd (%s)", strerror(errno));

	/* Set the keyboard into non-blocking event mode. */
	if (ioctl(x386Info.kbdFd, KDSKBDMODE, &data) < 0)
		FatalError("Cannot set event mode on keyboard (%s)\n",
			   strerror(errno));
	data = 1;
	if (ioctl (x386Info.kbdFd, FIONBIO, &data) < 0)
		FatalError("Cannot keyboard non-blocking (%s)\n",
			   strerror(errno));
	return(x386Info.kbdFd);
}

int xf86KbdOff()
{
	int data = KB_ASCII;

	if (ioctl(x386Info.kbdFd, KDSKBDMODE, &data) < 0)
		FatalError("can't reset keyboard mode (%s)\n",
			    strerror(errno));
	return(x386Info.kbdFd);
}

void xf86KbdEvents()
{
	kd_event ke;

	/* This loop assumes non-blocking keyboard IO. */
	while (read (x386Info.kbdFd, &ke, sizeof(ke)) == sizeof(ke))
	{
		x386PostKbdEvent(ke.value.sc);
	}
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

int xf86MouseOn()
{
	if ((x386Info.mseFd = open(x386Info.mseDevice, O_RDONLY, 0)) < 0)
	{
		FatalError("Cannot open mouse (%s)\n", strerror(errno));
	}
	if (fcntl(x386Info.mseFd, F_SETFL, FNDELAY | FASYNC) < 0)
	{
		FatalError("Cannot set up mouse (%s)\n", strerror(errno));
	}

	xf86SetupMouse();

	{
		int data = 1;

		if (ioctl (x386Info.mseFd, FIONBIO, &data) < 0)
		{
			FatalError(
				"Cannot set mouse non-blocking (%s)\n",
				strerror(errno));
		}
	}
	return(x386Info.mseFd);
}

int xf86MouseOff(doclose)
Bool doclose;
{
	close(x386Info.mseFd);
	return(x386Info.mseFd);
}

#define EVENT_LIST_SIZE 32

void xf86MouseEvents()
{
	int total, buttons, dx, dy;
	static kd_event eventList[EVENT_LIST_SIZE];
	kd_event *event;

	total = read(x386Info.mseFd, eventList, sizeof(eventList));
	if (total < 0)
	{
		if (errno != EWOULDBLOCK)
		{
			FatalError("Cannot read from mouse (%s)\n",
				   strerror(errno));
		}
		return;
	}

	total /= sizeof(kd_event);
	event = eventList;
	while (total--)
	{
		buttons = x386Info.lastButtons;
		dx = dy = 0;

		switch (event->type)
		{
		case MOUSE_RIGHT:
			buttons = x386Info.lastButtons & 6 |
				  (event->value.up ? 0 : 1);
			break;
		case MOUSE_MIDDLE:
			buttons = x386Info.lastButtons & 5 |
				  (event->value.up ? 0 : 2);
			break;
		case MOUSE_LEFT:
			buttons = x386Info.lastButtons & 3 |
				  (event->value.up ? 0 : 4);
			break;
		case MOUSE_MOTION:
			dx = event->value.m_deltaX;
			dy = - event->value.m_deltaY;
			break;
		default:
			ErrorF("Bad mouse event\n");
			break;
		}
		x386PostMseEvent(buttons, dx, dy);
		++event;
	}
}
