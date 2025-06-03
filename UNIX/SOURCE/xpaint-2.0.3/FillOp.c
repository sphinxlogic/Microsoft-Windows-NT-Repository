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
#ifndef NOSTDHDRS
#include <stdlib.h>
#include <unistd.h>
#endif
#include "Paint.h"
#include "Misc.h"
#include "xpaint.h"

extern void bzero(void *, int);

typedef struct {
	Boolean		done;
	Pixmap		pix;
} LocalInfo;

static XImage	*source, *mask;
static int	xMin, yMin, xMax, yMax;

/*
 * A Seed Fill Algorithm
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 */

/*
 * fill.c : simple seed fill program
 * Calls pixelread() to read pixels, pixelwrite() to write pixels.
 *
 * Paul Heckbert        13 Sept 1982, 28 Jan 1987
 */

#define pixelwrite(x,y)	{ 			\
		XPutPixel(mask, x, y, True); 	\
		xMin = MIN(xMin, x);		\
		yMin = MIN(yMin, y);		\
		xMax = MAX(xMax, x);		\
		yMax = MAX(yMax, y);		\
	}
#define pixelread(x,y)	(XGetPixel(mask, x, y) ? ~ov : XGetPixel(source, x, y))

typedef struct {short y, xl, xr, dy;} Segment;
/*
 * Filled horizontal segment of scanline y for xl<=x<=xr.
 * Parent segment was on line y-dy.  dy=1 or -1
 */

#define STACKSIZE 20000              /* max depth of stack */

#define PUSH(Y, XL, XR, DY)  /* push new segment on stack */ \
    if (sp<stack+STACKSIZE && Y+(DY)>=0 && Y+(DY)<=height) \
    {sp->y = Y; sp->xl = XL; sp->xr = XR; sp->dy = DY; sp++;}

#define POP(Y, XL, XR, DY)    /* pop segment off stack */ \
    {sp--; Y = sp->y+(DY = sp->dy); XL = sp->xl; XR = sp->xr;}

/*
 * fill: set the pixel at (x,y) and all of its 4-connected neighbors
 * with the same pixel value to the new pixel value nv.
 * A 4-connected neighbor is a pixel above, below, left, or right of a pixel.
 */

static void fill(int x, int y, int width, int height)
{
    int start, x1, x2, dy = 0;
    Pixel ov;      /* old pixel value */
    Segment stack[STACKSIZE], *sp = stack;   /* stack of filled segments */

    ov = pixelread(x, y);              /* read pv at seed point */
    PUSH(y, x, x, 1);                      /* needed in some cases */
    PUSH(y+1, x, x, -1);                /* seed segment (popped 1st) */

    while (sp>stack) {
        /* pop segment off stack and fill a neighboring scan line */
        POP(y, x1, x2, dy);
        /*
         * segment of scan line y-dy for x1<=x<=x2 was previously filled,
         */
        for (x = x1; x >= 0 && pixelread(x, y) == ov; x--)
            pixelwrite(x, y);
        if (x >= x1) {
          for (x++; x<=x2 && pixelread(x, y)!=ov; x++);
          start = x;
	  if (x > x2)
	    continue;
	} else {
          start = x+1;
          if (start<x1) PUSH(y, start, x1-1, -dy);       /* leak on left? */
          x = x1+1;
	}
        do {
            for (; x<=width && pixelread(x, y)==ov; x++)
                pixelwrite(x, y);
            PUSH(y, start, x-1, dy);
            if (x>x2+1) PUSH(y, x2+1, x-1, -dy);       /* leak on right? */
            for (x++; x<=x2 && pixelread(x, y)!=ov; x++);
            start = x;
        } while (x<=x2);
    }
}

static void     press(Widget w, LocalInfo *l, XButtonEvent *event, OpInfo *info)
{
	XRectangle	undo;
	char		*mData;
	int		width, height;
	GC		gc;
	int		fillWidth, fillHeight;
	XGCValues	values;

	UndoStart(w, info);

	SetWatchCursor(w);

	XtVaGetValues(w, XtNdrawWidth, &width, XtNdrawHeight, &height, NULL);

	if ((mData = XtMalloc(width * height * sizeof(char))) == NULL)
		return;

	bzero(mData, width * height * sizeof(char));

	if ((mask   = XCreateImage(XtDisplay(w), NULL,
				1, XYBitmap, 0, mData, 
				width, height, 8, 0)) == NULL) {
		XtFree(mData);
		return;
	}

	if ((source = XGetImage(XtDisplay(w), info->drawable, 0, 0,
				width, height, ~0, ZPixmap)) == NULL) {
		XDestroyImage(mask);
		return;
	}

	xMin = xMax = info->realX;
	yMin = yMax = info->realY;

	fill(info->realX, info->realY, width - 1, height - 1);

	XDestroyImage(source);

	fillWidth  = (xMax - xMin) + 1;
	fillHeight = (yMax - yMin) + 1;

	l->pix = XCreatePixmap(XtDisplay(w), XtWindow(w), fillWidth,fillHeight,1);

	values.foreground = True;
	values.background = False;
	gc = XCreateGC(XtDisplay(w), l->pix, GCForeground|GCBackground, &values);
	XPutImage(XtDisplay(w), l->pix, gc, mask, xMin, yMin,
				0, 0, fillWidth, fillHeight);
	XFreeGC(XtDisplay(w),gc);

	XDestroyImage(mask);

	XSetClipOrigin(XtDisplay(w), info->filled_gc, xMin, yMin);
	XSetClipMask(XtDisplay(w), info->filled_gc, l->pix);

	XFillRectangle(XtDisplay(w), info->drawable, info->filled_gc, 
			xMin, yMin, fillWidth, fillHeight);
	if (!info->isFat)
		XFillRectangle(XtDisplay(w), XtWindow(w), info->filled_gc, 
			xMin, yMin, fillWidth, fillHeight);

	XSetClipMask(XtDisplay(w), info->filled_gc, None);
	XFreePixmap(XtDisplay(w), l->pix);

	XYtoRECT(xMin, yMin, xMax + 1, yMax + 1, &undo);
	UndoSetRectangle(w, &undo);
	PwUpdate(w, &undo, False);

	SetCrossHairCursor(w);
}

void *FillAdd(Widget w)
{
	LocalInfo	*l = (LocalInfo*)XtMalloc(sizeof(LocalInfo));
	l->done = False;
	OpAddEventHandler(w, opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, l);
	SetCrossHairCursor(w);
	return l;
}
void FillRemove(Widget w, LocalInfo *l)
{
	OpRemoveEventHandler(w, opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, l);
	XtFree((XtPointer)l);
}
