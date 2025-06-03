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
#include "xpaint.h"
#include "Paint.h"
#include "Misc.h"

typedef struct {
	int	rx, ry, startX, startY, endX, endY, drawn;
} LocalInfo;

#define MKRECT(rect, sx, sy, ex, ey) do {				\
	(rect)->x      = MIN(sx, ex);					\
	(rect)->y      = MIN(sy, ey);					\
	(rect)->width  = MAX(sx, ex) - (rect)->x;			\
	(rect)->height = MAX(sy, ey) - (rect)->y;			\
	if (flag) {							\
		(rect)->width = MIN((rect)->width, (rect)->height);	\
		(rect)->height = (rect)->width; 			\
	}								\
	if (sx < ex) {							\
		if (sy < ey) {						\
			/* +,+ */					\
			(rect)->x -= (rect)->width;			\
			(rect)->angle1 =   0*64;			\
			(rect)->angle2 =  90*64;			\
		} else {						\
			/* +,- */					\
			/* No Change */					\
			(rect)->angle1 =  90*64;			\
			(rect)->angle2 =  90*64;			\
		}							\
	} else {							\
		if (sy < ey) {						\
			/* -,+ */					\
			(rect)->x -= (rect)->width;			\
			(rect)->y -= (rect)->height;			\
			(rect)->angle1 =   0*64;			\
			(rect)->angle2 = -90*64;			\
		} else {						\
			/* -,- */					\
			(rect)->y -= (rect)->height;			\
			(rect)->angle1 = -90*64;			\
			(rect)->angle2 = -90*64;			\
		}							\
	}								\
	(rect)->width *= 2;						\
	(rect)->height *= 2;						\
	} while (0)

static void	press(Widget w, LocalInfo *l, XButtonEvent *event, OpInfo *info)
{
	l->rx   = info->x;
	l->ry   = info->y;
	l->endX = l->startX = event->x;
	l->endY = l->startY = event->y;

	l->drawn = False;
}


static void	motion(Widget w, LocalInfo *l, 
			XMotionEvent *event, OpInfo *info) 
{
	Boolean	flag = False;
	XArc	rect;

	if (l->drawn) {
		MKRECT(&rect, l->startX, l->startY, l->endX, l->endY);

		XDrawArcs(XtDisplay(w), XtWindow(w), Global.gcx,
				&rect, 1);
	}

	l->endX = event->x;
	l->endY = event->y;

	/*
	**  Really set this flag in the if statement
	*/
	if (l->drawn = (l->startX != l->endX && l->startY != l->endY)) {
		MKRECT(&rect, l->startX, l->startY, l->endX, l->endY);

		XDrawArcs(XtDisplay(w), XtWindow(w), Global.gcx,
				&rect, 1);
	}
}

static void	release(Widget w, LocalInfo *l, 
				XButtonEvent *event, OpInfo *info) 
{
	XArc		rect;
	Boolean		flag = False;
	XRectangle	undo;

	if (l->drawn && info->surface == opWindow)  {
		MKRECT(&rect, l->startX, l->startY, l->endX, l->endY);

		XDrawArcs(XtDisplay(w), XtWindow(w), Global.gcx,
				&rect, 1);
	}

        if (info->surface == opWindow && info->isFat)
		return;

	UndoStart(w, info);

	MKRECT(&rect, l->rx, l->ry, event->x, event->y);
	XDrawArcs(XtDisplay(w), info->drawable, info->filled_gc, &rect, 1);

	if (info->surface == opPixmap) {
		XYtoRECT(l->rx, l->ry, event->x, event->y, &undo);
		UndoSetRectangle(w, &undo);
		PwUpdate(w, &undo, False);
	}
}

/*
**  Those public functions
*/
void *ArcAdd(Widget w)
{
	LocalInfo	*l = (LocalInfo*)XtMalloc(sizeof(LocalInfo));

	XtVaSetValues(w, XtNcompress, True, NULL);
	OpAddEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpAddEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpAddEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
	SetCrossHairCursor(w);
	return l;
}
void ArcRemove(Widget w, LocalInfo *l)
{
	OpRemoveEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpRemoveEventHandler(w, opWindow, ButtonMotionMask,FALSE, (OpEventProc)motion, l);
	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
	XtFree((XtPointer)l);
}
