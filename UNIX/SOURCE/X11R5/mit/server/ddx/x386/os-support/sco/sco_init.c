/*
 * Copyright 1993 by David McCullough <davidm@stallion.oz.au>
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David McCullough and David Wexelblat 
 * not be used in advertising or publicity pertaining to distribution of the 
 * software without specific, written prior permission.  David McCullough and
 * David Wexelblat makes no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * DAVID MCCULLOUGH AND DAVID WEXELBLAT DISCLAIM ALL WARRANTIES WITH REGARD 
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS, IN NO EVENT SHALL DAVID MCCULLOUGH OR DAVID WEXELBLAT BE 
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN 
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF 
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/os-support/sco/sco_init.c,v 2.4 1993/09/27 12:24:56 dawes Exp $ */

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "x386.h"
#include "x386Procs.h"
#include "xf86_OSlib.h"

static Bool KeepTty = FALSE;
static int VTnum = -1;
static int sco_console_mode = -1;

extern void xf86VTRequest(
#if NeedFunctionPrototypes
	int
#endif
);

void xf86OpenConsole()
{
    int fd;
    struct vt_mode VT;
    struct stat status;
    char vtname[10];

    if (serverGeneration == 1) 
    {
	/* check if we're run with euid==0 */
	if (geteuid() != 0)
	{
	    FatalError("xf86OpenConsole: Server must be suid root\n");
	}

	/*
	 * setup the virtual terminal manager
	 *
	 * SCO vts start at tty01 which is vt00, if you could call them VT's.
	 * We use the numbers 1..X as it fits nicer with the device naming 
	 * scheme.
	 *
	 * In os/osinit.c we took the precuation of not closing stdin so that
	 * we can use the current vt if no vt was specified on the command line
	 *
	 * Under SCO VT_OPENQRY does nothing at all
	 * if nothing was specified we try to determine the VT from stdin
	 */
	if ((VTnum != -1) && (VTnum != 0))
	{
	    x386Info.vtno = VTnum - 1;
	}
	else 
	{
	    if ((fstat(0, &status) >= 0) && (status.st_mode & S_IFCHR))
	    {
		x386Info.vtno = minor(status.st_rdev);
	    } 
	    else 
	    {
		ErrorF("%s: Failed to stat stdin, using tty02 (%s)\n",
		       "xf86OpenConsole", strerror(errno));
		x386Info.vtno = 1; /* tty02 */
	    }
	}
	ErrorF("(using VT number %d)\n\n", x386Info.vtno + 1);

	sprintf(vtname,"/dev/tty%02d", x386Info.vtno+1); /* /dev/tty[01-12] */

	x386Config(FALSE); /* Read Xconfig */

	if (!KeepTty)
	{
	    setpgrp();
	}

	if ((x386Info.consoleFd = open(vtname, O_RDWR | O_NDELAY, 0)) < 0)
	{
	    FatalError("xf86OpenConsole: Cannot open %s (%s)\n",
		       vtname, strerror(errno));
	}

	/* now we can dispose of stdin */
	fclose(stdin);
    
	/*
	 * now get the VT
	 */
	if ((sco_console_mode = ioctl(x386Info.consoleFd, CONS_GET, 0L)) < 0) 
	{
	    FatalError("xf86OpenConsole: VT_GETMODE failed on console (%s)\n",
		       strerror(errno));
	}
	if (ioctl(x386Info.consoleFd, VGA_IOPRIVL, 1) < 0)
	{
	    FatalError("xf86OpenConsole: VGA_IOPRIVL failed for VGA acc (%s)\n",
		       strerror(errno));
	}
	if (ioctl(x386Info.consoleFd, VT_GETMODE, &VT) < 0) 
	{
	    FatalError("xf86OpenConsole: VT_GETMODE failed (%s)\n",
		       strerror(errno));
	}

	signal(SIGUSR1, xf86VTRequest);

	VT.mode = VT_PROCESS;
	VT.relsig = SIGUSR1;
	VT.acqsig = SIGUSR1;
	VT.frsig = SIGUSR1;
	VT.waitv = 0;
	if (ioctl(x386Info.consoleFd, VT_SETMODE, &VT) < 0) 
	{
	    FatalError("xf86OpenConsole: VT_SETMODE VT_PROCESS failed\n");
	}
	/*
	 * make sure the console driver thinks the console is in graphics
	 * mode.  Under mono we have to do the two as the console driver only
	 * allows valid modes for the current video card and Herc or vga are
	 * the only devices currently supported.
	 */
	if (ioctl(x386Info.consoleFd, SW_VGA12, 0) < 0)
		if (ioctl(x386Info.consoleFd, SW_HGC_P0, 0) < 0)
		{
			ErrorF("Failed to set graphics mode (%s)\n",
			       strerror(errno));
		}

    }
    else 
    {
	/* serverGeneration != 1 */
	/*
	 * now get the VT
	 */
	if (ioctl(x386Info.consoleFd, VT_ACTIVATE, x386Info.vtno - 1) != 0)
	{
	    ErrorF("xf86OpenConsole: VT_ACTIVATE failed\n");
	}
    }
    return;
}

void xf86CloseConsole()
{
    struct vt_mode   VT;

#if 0
    ioctl(x386Info.consoleFd, VT_ACTIVATE, x386Info.vtno - 1);
#endif
    ioctl(x386Info.consoleFd, VT_RELDISP, 1);
    if (sco_console_mode != -1)
    {
	ioctl(x386Info.consoleFd, MODESWITCH | sco_console_mode, 0L);
    }
    if (ioctl(x386Info.consoleFd, VT_GETMODE, &VT) != -1)
    {
	VT.mode = VT_AUTO;
	ioctl(x386Info.consoleFd, VT_SETMODE, &VT); /* set dflt vt handling */
    }
    close(x386Info.consoleFd);                 /* make the vt-manager happy */
    return;
}

int xf86ProcessArgument(argc, argv, i)
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
	if ((argv[i][0] == 'v') && (argv[i][1] == 't'))
	{
		if (sscanf(argv[i], "vt%2d", &VTnum) == 0)
		{
			UseMsg();
			VTnum = -1;
			return(0);
		}
		return(1);
	}
	return(0);
}

void xf86UseMsg()
{
	ErrorF("vtXX                   use the specified VT number\n");
	ErrorF("-keeptty               ");
	ErrorF("don't detach controlling tty (for debugging only)\n");
	return;
}
