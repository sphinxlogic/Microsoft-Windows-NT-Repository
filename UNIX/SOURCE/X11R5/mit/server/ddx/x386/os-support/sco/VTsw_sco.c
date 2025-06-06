/*
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 * Copyright 1993 by David McCullough <davidm@stallion.oz.au>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Wexelblat not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  David Wexelblat makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/os-support/sco/VTsw_sco.c,v 2.1 1993/07/29 02:22:01 dawes Exp $ */

#include "X.h"
#include "input.h"
#include "scrnintstr.h"

#include "x386.h"
#include "x386Priv.h"
#include "xf86_OSlib.h"

/*
 * Handle the VT-switching interface for SCO
 */

/*
 * This function is the signal handler for the VT-switching signal.  It
 * is only referenced inside the OS-support layer.
 */
void xf86VTRequest(sig)
int sig;
{
	signal(sig, (void(*)())xf86VTRequest);
	x386Info.vtRequestsPending = TRUE;
	return;
}

Bool xf86VTSwitchPending()
{
	return(x386Info.vtRequestsPending ? TRUE : FALSE);
}

Bool xf86VTSwitchAway()
{
	x386Info.vtRequestsPending = FALSE;
	if (ioctl(x386Info.consoleFd, VT_RELDISP, 1) < 0)
	{
		return(FALSE);
	}
	else
	{
		return(TRUE);
	}
}

Bool xf86VTSwitchTo()
{
	x386Info.vtRequestsPending = FALSE;
	if (ioctl(x386Info.consoleFd, VT_RELDISP, VT_ACKACQ) < 0)
	{
		return(FALSE);
	}
	else
	{
		/*
		 * make sure the console driver thinks the console is in
		 * graphics mode.  Under mono we have to do the two as the
		 * console driver only allows valid modes for the current
		 * video card and Herc or vga are the only devices currently
		 * supported.
		 */
		if (ioctl(x386Info.consoleFd, SW_VGA12, 0) < 0)
			if (ioctl(x386Info.consoleFd, SW_HGC_P0, 0) < 0)
			{
				ErrorF("Failed to set graphics mode : %s\n",
				       strerror(errno));
			}

		return(TRUE);
	}
}
