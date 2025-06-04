
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/xthelper.c,v 1.4 1993/01/11 02:15:55 ricks Exp $";
#endif

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988-1993, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * xthelper.c: routines for simplifying the use of the X toolkit
 *
 */

#include "copyright.h"
#include "config.h"
#include <stdio.h>
#include "utils.h"
#ifndef VMS
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#else
#include <decw$include/Intrinsic.h>
#include <decw$include/StringDefs.h>
#endif
#ifdef MOTIF
#include <Xm/Xm.h>
#endif
#include "xmisc.h"
#include "xrn.h"
#include "xthelper.h"


void
xthCenterWidget(widget, x, y)
Widget widget;
int x, y;
{
#ifdef MOTIF
    Arg sargs[3];
#else
    Arg sargs[2];
#endif
    Arg gargs[2];
    Dimension height, width;

    XtSetArg(gargs[0], XtNwidth, &width);
    XtSetArg(gargs[1], XtNheight, &height);
    XtGetValues(widget, gargs, XtNumber(gargs));

    x -= width / 2;
    y -= height / 2;
    if (x +  width  > WidthOfScreen(XtScreen(widget))) {
	x = WidthOfScreen(XtScreen(widget)) - width;
    }
    if (y + height > HeightOfScreen(XtScreen(widget))) {
	y = HeightOfScreen(XtScreen(widget)) - height;
    }
    if (x < 0) {
	x = 0;
    }
    if (y < 0) {
	y = 0;
    }
    XtSetArg(sargs[0], XtNx, x);
    XtSetArg(sargs[1], XtNy, y);
#ifdef MOTIF
    XtSetArg(sargs[2], XmNdefaultPosition, False);
#endif
    XtSetValues(widget, sargs, XtNumber(sargs));
    return;
}

void
xthCenterWidgetOverCursor(widget)
Widget widget;
/*
 * center a window over the cursor
 *
 *   returns: void
 *
 */
{
    Window root, child;
    int x, y, dummy;
    unsigned int mask;

    (void) XQueryPointer(XtDisplay(TopLevel), XtWindow(TopLevel),
			 &root, &child,
			 &x, &y, &dummy, &dummy,
			 &mask);

    xthCenterWidget(widget, x, y);
    return;
}

void
xthHandleAllPendingEvents()
{
    
    if ((XRNState & XRN_X_UP) == XRN_X_UP) {
	while (XtPending()) {
	    XtProcessEvent(XtIMAll);
	}
    }
    return;
}
#ifdef MOTIF

int xthTextWidth(widget)
Widget widget;
{
    Arg arg[1];
    short int cols = 0;

    XtSetArg(arg[0], XmNcolumns, &cols);
    XtGetValues(widget, arg, 1);
    return cols;
}

/*
 * Fit a frame to its contents.
 */
void fitFrameToContents(frame, contents)
Widget frame;
Widget contents;
{
    Arg args[3];
    Dimension width, height, borderWidth;

    /* get width & height from contents */
    XtSetArg(args[0], XmNheight,&height);
    XtSetArg(args[1], XmNwidth, &width);
    XtGetValues(contents, args, 2);

    /* get borderWidth from frame */
    XtSetArg(args[0], XmNborderWidth, &borderWidth);
    XtGetValues(frame, args, 1);

    XtSetArg(args[0], XmNwidth, width);
    XtSetArg(args[1], XmNheight, height);
    XtSetArg(args[2], XmNborderWidth, borderWidth);
    XtSetValues(frame, args, 3);
}
#endif

/*
 * Tell an ICCCM-compliant window manager that we are
 * withdrawing the window.  This is done by sending 
 * a fake unmap-notify event.
 */
void xthUnmapWidget(w)
Widget w;
{
    XEvent event;
    Screen *scr;

    XtUnmapWidget(w);
    scr = XtScreen(w);
    event.xunmap.type = UnmapNotify;
    event.xunmap.serial = XLastKnownRequestProcessed(XtDisplay(w));
    event.xunmap.send_event = True;
    event.xunmap.display = XtDisplay(w);
    event.xunmap.event = XRootWindowOfScreen(scr);
    event.xunmap.window = XtWindow(w);
    event.xunmap.from_configure = False;

    XSendEvent(
	XtDisplay(w), XRootWindowOfScreen(scr), 
	False, 
	SubstructureRedirectMask|SubstructureNotifyMask, 
	&event
	);
}

/* 
 * Translate a geometry string and load up x/y/w/h arg list
 */

void
xthTranslateGeometry(geometry, frameArgs)
char *geometry;
Arg frameArgs[];
{
    if (geometry != NIL(char)) {
	int bmask;
	bmask = XParseGeometry(geometry,        /* geometry specification */
		       (int *) &frameArgs[0].value,    /* x      */
		       (int *) &frameArgs[1].value,    /* y      */
	      (unsigned int *) &frameArgs[2].value,    /* width  */
	      (unsigned int *) &frameArgs[3].value);   /* height */

	/* handle negative x and y values */
	if ((bmask & XNegative) == XNegative) {
	    frameArgs[0].value = (XtArgVal) ((int) frameArgs[0].value + 
	    			DisplayWidth(XtDisplay(TopLevel),
			        DefaultScreen(XtDisplay(TopLevel))));
	    frameArgs[0].value = (XtArgVal) ((int) frameArgs[0].value -
					     (int) frameArgs[2].value);
	}
	if ((bmask & YNegative) == YNegative) {
	    frameArgs[1].value = (XtArgVal) ((int) frameArgs[1].value +
				DisplayHeight(XtDisplay(TopLevel),
				DefaultScreen(XtDisplay(TopLevel))));
	    frameArgs[1].value = (XtArgVal) ((int) frameArgs[1].value -
					     (int) frameArgs[3].value);
	}
    }
}    
