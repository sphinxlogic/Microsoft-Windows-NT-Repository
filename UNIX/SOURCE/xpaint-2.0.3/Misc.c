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
#include <X11/cursorfont.h>

#include "xpaint.h"

Widget GetToplevel(Widget w)
{
	Widget	p = w;

	while (w != None) {
		p = w;
		w = XtParent(w);
	}
	return p;
}

Widget GetShell(Widget w)
{
	while (!XtIsShell(w))
		w = XtParent(w);
	return w;
}

void SetCrossHairCursor(Widget w)
{
	Cursor	c;

        c = XCreateFontCursor(XtDisplay(w), XC_crosshair);
	XDefineCursor(XtDisplay(w), XtWindow(w), c);
}
void SetPencilCursor(Widget w)
{
	Cursor	c;

        c = XCreateFontCursor(XtDisplay(w), XC_pencil);
	XDefineCursor(XtDisplay(w), XtWindow(w), c);
}

void SetWatchCursor(Widget w)
{
	Cursor	c;

        c = XCreateFontCursor(XtDisplay(w), XC_watch);
	XDefineCursor(XtDisplay(w), XtWindow(w), c);
	XFlush(XtDisplay(w));
}

/*
**  Some useful XRectangle computation code.
*/
XRectangle *RectUnion(XRectangle *a, XRectangle *b)
{
	static XRectangle	out;
	int			sx, ex, sy, ey;

	sx = MIN(a->x, b->x);
	sy = MIN(a->y, b->y);
	ex = MAX(a->x + a->width,  b->x + b->width);
	ey = MAX(a->y + a->height, b->y + b->height);
	
	XYtoRECT(sx, sy, ex, ey, &out);
	
	return &out;
}

XRectangle *RectIntersect(XRectangle *a, XRectangle *b)
{
	static XRectangle	out;
	int			w, h;
 
	out.x = MAX(a->x, b->x);
	out.y = MAX(a->y, b->y);
	w     = MIN(a->x + a->width,  b->x + b->width)  - out.x;
	h     = MIN(a->y + a->height, b->y + b->height) - out.y;

	if (w <= 0 || h <= 0)
		return NULL;

	out.width  = w;
	out.height = h;

	return &out;
}
