/*
 * Copyright 1992 by Rich Murphey <Rich@Rice.edu>
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Rich Murphey and David Wexelblat 
 * not be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  Rich Murphey and
 * David Wexelblat make no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * RICH MURPHEY AND DAVID WEXELBLAT DISCLAIM ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL RICH MURPHEY OR DAVID WEXELBLAT BE LIABLE FOR 
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/os-support/bsdi/bsdi_init.c,v 2.5 1993/08/29 14:31:17 dawes Exp $ */

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "x386.h"
#include "x386Procs.h"
#include "xf86_OSlib.h"

static Bool KeepTty = FALSE;

void xf86OpenConsole()
{
    int i, fd;

    if (serverGeneration == 1)
    {
	/* check if we're run with euid==0 */
	if (geteuid() != 0)
	{
	    FatalError("xf86OpenConsole: Server must be suid root\n");
	}

	if (!KeepTty)
	{
	    /*
	     * detaching the controlling tty solves problems of kbd character
	     * loss.  This is not interesting for CO driver, because it is 
	     * exclusive.
	     */
	    setpgrp(0, getpid());
	    if ((i = open("/dev/tty",O_RDWR)) >= 0)
	    {
		ioctl(i,TIOCNOTTY,(char *)0);
		close(i);
	    }
	}

	if ((x386Info.consoleFd = open("/dev/kbd", O_RDWR|O_NDELAY,0)) < 0)
	{
	    FatalError("xf86OpenConsole: Cannot open /dev/kbd (%s)\n",
		       strerror(errno));
	}
	if ((x386Info.screenFd = open("/dev/vga", O_RDWR|O_NDELAY,0)) < 0)
	{
	    FatalError("xf86OpenConsole: Cannot open /dev/vga (%s)\n",
		       strerror(errno));
	}

	x386Config(FALSE); /* Read Xconfig */

	if (ioctl(x386Info.consoleFd, PCCONIOCRAW, 0) < 0)
	{
	    FatalError("%s: PCCONIOCRAW failed (%s)\n", 
		       "xf86OpenConsole", strerror(errno));
	}
    }
    return;
}

void xf86CloseConsole()
{
    ioctl (x386Info.consoleFd, PCCONIOCCOOK, 0);

    if (x386Info.screenFd != x386Info.consoleFd)
    {
	close(x386Info.screenFd);
    }
    close(x386Info.consoleFd);
    return;
}

int xf86ProcessArgument (argc, argv, i)
int argc;
char *argv[];
int i;
{
	/*
	 * Keep server from detaching from controlling tty.  This is useful 
	 * when debugging (so the server can receive keyboard signals.
	 */
	if (!strcmp(argv[i], "-keeptty"))
	{
		KeepTty = TRUE;
		return(1);
	}
	return(0);
}

void xf86UseMsg()
{
	ErrorF("-keeptty               ");
	ErrorF("don't detach controlling tty (for debugging only)\n");
	return;
}
