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
#include "Misc.h"
#include "Paint.h"

typedef struct {
	int	rx, ry;
	int	startX, startY, endX, endY;
	int	drawn;
	Boolean	type;		/* Is is a rectangle or square? */
	Boolean	fill;		/* Filled rectangle? */
} LocalInfo;

#define MKRECT(rect, sx, sy, ex, ey, typeFlag) do {			\
	if (typeFlag) {							\
		(rect)->width  = MIN(ABS(sx - ex),ABS(sy - ey));	\
		(rect)->height = (rect)->width;				\
		(rect)->x = (ex - sx < 0) ? sx - (rect)->width : sx;	\
		(rect)->y = (ey - sy < 0) ? sy - (rect)->height : sy;	\
	} else {							\
		(rect)->x      = MIN(sx, ex);				\
		(rect)->y      = MIN(sy, ey);				\
		(rect)->width  = MAX(sx, ex) - (rect)->x;		\
		(rect)->height = MAX(sy, ey) - (rect)->y;		\
	}								\
} while (0)


static void	press(Widget w, LocalInfo *l, 
				XButtonEvent *event, OpInfo *info) 
{
	l->rx   = info->x;
	l->ry   = info->y;
	l->endX = l->startX = event->x;
	l->endY = l->startY = event->y;

	l->type = (event->state & ShiftMask);

	l->drawn = False;
}

static void	motion(Widget w, LocalInfo *l, 
				XMotionEvent *event, OpInfo *info) 
{
	XRectangle	rect;

	if (l->drawn) {
		MKRECT(&rect, l->startX, l->startY, l->endX, l->endY, l->type);

		XDrawRectangles(XtDisplay(w), XtWindow(w), Global.gcx, &rect,1);
	}

	l->endX = event->x;
	l->endY = event->y;

	/*
	**  Really set this flag in the if statement
	*/
	if (l->drawn = (l->startX != l->endX || l->startY != l->endY)) {
		MKRECT(&rect, l->startX, l->startY, l->endX, l->endY, l->type);

		XDrawRectangles(XtDisplay(w), XtWindow(w), Global.gcx,&rect, 1);
	}
}

static void	release(Widget w, LocalInfo *l, 
				XButtonEvent *event, OpInfo *info) 
{
	XRectangle	rect;

	if (l->drawn && info->surface == opWindow) {
		MKRECT(&rect, l->startX, l->startY, l->endX, l->endY, l->type);

		XDrawRectangles(XtDisplay(w), XtWindow(w), Global.gcx,&rect, 1);
	}

	if (info->isFat && info->surface == opWindow)
		return;

	MKRECT(&rect, l->rx, l->ry, event->x, event->y, l->type);

	UndoStart(w, info);

	if (l->fill) {
		XFillRectangles(XtDisplay(w), info->drawable, 
					info->filled_gc, &rect, 1);
	} else {
		XDrawRectangles(XtDisplay(w), info->drawable, 
					info->filled_gc, &rect, 1);
	}

	if (info->surface == opPixmap) {
		rect.width++;
		rect.height++;
		UndoSetRectangle(w, &rect);
		PwUpdate(w, &rect, False);
	}
}

/*
**  Public routines
*/

void *BoxAdd(Widget w)
{
	LocalInfo	*l = (LocalInfo *)XtMalloc(sizeof(LocalInfo));

	l->fill = False;

        XtVaSetValues(w, XtNcompress, True, NULL);

	OpAddEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpAddEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpAddEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
	SetCrossHairCursor(w);

	return l;
}
void BoxRemove(Widget w, LocalInfo *l)
{
	OpRemoveEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpRemoveEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);

	XtFree((XtPointer)l);
}

void *FBoxAdd(Widget w)
{
	LocalInfo	*l = (LocalInfo *)XtMalloc(sizeof(LocalInfo));

	l->fill = True;

        XtVaSetValues(w, XtNcompress, False, NULL);

	OpAddEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpAddEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpAddEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
	SetCrossHairCursor(w);
	
	return l;
}
void FBoxRemove(Widget w, LocalInfo *l)
{
	OpRemoveEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpRemoveEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);

	XtFree((XtPointer)l);
}
