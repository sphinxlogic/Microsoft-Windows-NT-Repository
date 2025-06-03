/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, 1993, David Koblas (koblas@netcom.com)            | */
/* |                                                                   | */
/* | Permission to use, copy, modify, and to distribute this software  | */
/* | and its documentation for any purpose is hereby granted without   | */
/* | fee, provided that the above copyright notice appear in all       | */
/* | copies and that both that copyright notice and this permission    | */
/* | notice appear in supporting documentation.  There is no           | */
/* | representations about the suitability of this software for        | */
/* | any purpose.  this software is provided "as is" without express   | */
/* | or implied warranty.                                              | */
/* |                                                                   | */
/* +-------------------------------------------------------------------+ */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <sys/time.h>
#include "xpaint.h"
#include "Paint.h"

#ifndef NOSTDHDRS
#include <stdlib.h>
#include <unistd.h>
#endif

#define	DELAY	200

typedef struct {
	Widget		w;
	int		x, y;
	XtIntervalId    id;
	Boolean		gcFlag, isTiled;
	/*
	**  Borrowed from the info
	*/
	GC		gc;
	Pixmap		drawable;
	Boolean		isFat;
} LocalInfo;

static int	radius = 10, density = 10, radius2 = 10 * 10;

long	random();

#define RANDOM(s, f)	(((double)(random() % 0x0fffffff) / (double)0x0fffffff) * (f - s) + s)

static void draw(LocalInfo *l)
{
	int		i;
	XRectangle	rect;
	union {
		XSegment	s[512];
		XPoint		p[512];
	} p;
    
	UndoGrow(l->w, l->x - radius, l->y - radius);
	UndoGrow(l->w, l->x + radius, l->y + radius);

	for (i = 0; i < density; i++) {
		int	rx, ry;
		do {
			rx = RANDOM(-radius, radius);
			ry = RANDOM(-radius, radius);
		} while (rx * rx + ry * ry > radius2);

		if (l->isTiled) {
			p.s[i].x1  = l->x + rx;
			p.s[i].y1  = l->y + ry;
			p.s[i].x2  = l->x + rx;
			p.s[i].y2  = l->y + ry;
		} else {
			p.p[i].x   = l->x + rx;
			p.p[i].y   = l->y + ry;
		}
	}

	XYtoRECT(l->x - radius, l->y - radius,
		 l->x + radius, l->y + radius, &rect);

	if (l->isTiled) {
		XDrawSegments(XtDisplay(l->w), l->drawable, l->gc, p.s, density);
		if (!l->isFat)
			XDrawSegments(XtDisplay(l->w), XtWindow(l->w), l->gc, p.s, density);
	} else {
		XDrawPoints(XtDisplay(l->w), l->drawable, l->gc, p.p, density, CoordModeOrigin);
		if (!l->isFat)
			XDrawPoints(XtDisplay(l->w), XtWindow(l->w), l->gc, p.p, density, CoordModeOrigin);
	}

	PwUpdate(l->w, &rect, False);
}

static void drawEvent(LocalInfo *l)
{
	draw(l);
	l->id = XtAppAddTimeOut(Global.appContext, DELAY, (XtTimerCallbackProc)drawEvent, l);
}

static void	press(Widget w, LocalInfo *l, XButtonEvent *event, OpInfo *info) 
{
	int	width, rule;

	l->x = event->x;
	l->y = event->y;

	UndoStartPoint(w, info, event->x, event->y);

	l->drawable = info->drawable;
	l->isFat    = info->isFat;

	XtVaGetValues(w, XtNlineWidth, &width, XtNfillRule, &rule, NULL);
	l->isTiled = (rule != FillSolid);

	if (!l->gcFlag) {
		l->gcFlag = (width != 0);
	}
	if (l->gcFlag) {
		if (l->gc == None)
			l->gc = XCreateGC(XtDisplay(w), info->drawable, 0, NULL);
		XCopyGC(XtDisplay(w), info->filled_gc, ~GCLineWidth, l->gc);
	} else {
		l->gc       = info->filled_gc;
	}

	if (l->id == (XtIntervalId)NULL)
		drawEvent(l);
	else
		draw(l);
}

static void	motion(Widget w, LocalInfo *l, XMotionEvent *event, OpInfo *info) 
{
	l->x = event->x;
	l->y = event->y;

	draw(l);
}

static void	release(Widget w, LocalInfo *l, XMotionEvent *event, OpInfo *info) 
{
	if (l->id != (XtIntervalId)NULL)
		XtRemoveTimeOut(l->id);

	l->id = (XtIntervalId)NULL;
}

/*
**  Those public functions
*/
void SpraySetParameters(int r, int d)
{
	radius = r;
	radius2 = r * r;
	density = d;
}
void *SprayAdd(Widget w)
{
	LocalInfo	*l = XtNew(LocalInfo);
	Cursor		c;
	static int	inited = False;

	if (!inited) {
		extern srandom(int);
		srandom((int)time(0));
		inited = True;
	}

	l->w  = w;
	l->id = (XtIntervalId)NULL;
	l->gc = None;
	l->gcFlag = False;

	XtVaSetValues(w, XtNcompress, False, NULL);

	c = XCreateFontCursor(XtDisplay(w), XC_spraycan);
	XDefineCursor(XtDisplay(w), XtWindow(w), c);

	OpAddEventHandler(w, opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpAddEventHandler(w, opPixmap, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpAddEventHandler(w, opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);

	return l;
}
void SprayRemove(Widget w, LocalInfo	*l)
{
	OpRemoveEventHandler(w, opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpRemoveEventHandler(w, opPixmap, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpRemoveEventHandler(w, opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);

	if (l->gcFlag)
		XFreeGC(XtDisplay(w), l->gc);
	if (l->id != (XtIntervalId)NULL)
		XtRemoveTimeOut(l->id);

	XtFree((XtPointer)l);
}
