/*
 * Copyright 1992 by Robert Baron <Robert.Baron@ernst.mach.cs.cmu.edu>
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Robert Baron and David Wexelblat not 
 * be used in advertising or publicity pertaining to distribution of the 
 * software without specific, written prior permission.  Robert Baron and 
 * David Wexelblat make no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * ROBERT BARON AND DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL ROBERT BARON OR DAVID WEXELBLAT BE LIABLE FOR 
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/os-support/mach/mach_init.c,v 2.2 1993/08/08 06:42:39 dawes Exp $ */

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "x386.h"
#include "x386Procs.h"
#include "xf86_OSlib.h"

void xf86OpenConsole()
{
    if (serverGeneration == 1)
    {
	x386Config(FALSE); /* Read Xconfig */
	if ((x386Info.consoleFd = open("/dev/console",O_RDWR,0)) < 0) 
	{
	    FatalError("xf86OpenConsole: Can't open /dev/console (%s)\n",
		       strerror(errno));
	}
	return;
    }
}

void xf86CloseConsole()
{
	close(x386Info.consoleFd);
	return;
}

/* ARGSUSED */
int xf86ProcessArgument (argc, argv, i)
int argc;
char *argv[];
int i;
{
	return(0);
}

void xf86UseMsg()
{
	return;
}
