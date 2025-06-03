/* $XFree86: mit/server/ddx/x386/common/x386Procs.h,v 2.3 1993/09/30 17:49:37 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * $Header: /proj/X11/mit/server/ddx/x386/RCS/x386Procs.h,v 1.1 1991/06/02 22:36:19 root Exp $
 */

#ifndef _X386PROCS_H
#define _X386PROCS_H

#include <X11/Xfuncproto.h>
#include "x386.h"
#include "x386Priv.h"

_XFUNCPROTOBEGIN


/* x386Config.c */

extern void x386Config();

extern void x386LookupMode(
#if NeedFunctionPrototypes
	DisplayModePtr,		/* target */
	ScrnInfoPtr		/* driver */
#endif 
);

extern void xf86VerifyOptions(
#if NeedFunctionPrototypes
	OFlagSet *,		/* allowedOptions */
	ScrnInfoPtr		/* driver */
#endif
);

/* x386Cursor.c */

extern void x386InitViewport(
#if NeedFunctionPrototypes
	ScrnInfoPtr		/* pScr */
#endif 
);

extern void x386SetViewport(
#if NeedFunctionPrototypes
	ScreenPtr,		/* pScreen */
	int,			/* x */
	int			/* y */
#endif 
);

extern void x386ZoomViewport(
#if NeedFunctionPrototypes
	ScreenPtr,		/* pScreen */
	int			/* zoom */
#endif 
);


/* x386Events.c */

extern void ProcessInputEvents();

extern void x386PostKbdEvent(
#if NeedFunctionPrototypes
	unsigned		/* key */
#endif 
);

extern void x386PostMseEvent(
#if NeedFunctionPrototypes
	int,			/* buttons */
	int,			/* dx */
	int			/* dy */
#endif
);

extern void x386Block(
#if NeedFunctionPrototypes
	pointer,		/* blockData */
	pointer,		/* pTimeout */
	long *			/* pReadmask */
#endif
);

extern void x386Wakeup(
#if NeedFunctionPrototypes
	pointer,		/* blockData */
	unsigned long,		/* err */
	long *			/* pReadmask */
#endif
);

extern void x386VTRequest(
#if NeedFunctionPrototypes
	int			/* signo */
#endif
);

extern void x386SigHandler(
#if NeedFunctionPrototypes
	int		       /* signo */
#endif
);

/* x386Io.c */

extern void x386KbdLeds();

extern void x386InitKBD(
#if NeedFunctionPrototypes
	Bool			/* init */
#endif
);

#ifdef NEED_EVENTS
extern void x386KbdBell(
#if NeedFunctionPrototypes
	int,			/* loudness */
	DeviceIntPtr 		/* pKeyboard */
#endif
);
#endif

extern void x386KbdCtrl(
#if NeedFunctionPrototypes
	DevicePtr,		/* pKeyboard */
	KeybdCtrl *		/* ctrl */
#endif
);

extern int  x386KbdProc(
#if NeedFunctionPrototypes
	DevicePtr,		/* pKeyboard */
	int			/* what */
#endif
);

extern void x386KbdEvents();

extern void x386MseCtrl(
#if NeedFunctionPrototypes
	DevicePtr,		/* pPointer */
	PtrCtrl*		/* ctrl */
#endif
);

extern int  x386MseProc(
#if NeedFunctionPrototypes
	DevicePtr,		/* pPointer */
	int			/* what */
#endif
);

extern void x386MseEvents();

/* xf86_Mouse.c */

extern Bool xf86MouseSupported(
#if NeedFunctionPrototypes
	int			/* mousetype */
#endif
);

extern void xf86SetupMouse(
#if NeedFunctionPrototypes
	void
#endif
);

extern void xf86MouseProtocol(
#if NeedFunctionPrototypes
	unsigned char *,	/* rBuf */
	int			/* nBytes */
#endif
);


/* x386Kbd.c */

extern void x386KbdGetMapping(
#if NeedFunctionPrototypes
	KeySymsRec *,		/* pKeySyms */
	CARD8 *			/* pModMap */
#endif
);

_XFUNCPROTOEND

#endif /* _X386PROCS_H */


