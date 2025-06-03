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
#include "xpaint.h"
#include "Misc.h"
#include "Paint.h"

typedef struct {
	int	startX, startY;
} LocalInfo;

static void	press(Widget w, LocalInfo *l, XButtonEvent *event, OpInfo *info) 
{
	XRectangle	undo;

	if (info->surface == opWindow)
		return;

	l->startX = event->x;
	l->startY = event->y;

	undo.x      = event->x;
	undo.y      = event->y;
	undo.width  = 1;
	undo.height = 1;

	UndoStartPoint(w, info, event->x, event->y);

	XDrawLine(XtDisplay(w), info->drawable, info->filled_gc,
			 l->startX, l->startY, event->x, event->y);
	if (!info->isFat)
		XDrawLine(XtDisplay(w), XtWindow(w), info->filled_gc,
			 l->startX, l->startY, event->x, event->y);

	PwUpdate(w, &undo, False);
}

static void	motion(Widget w, LocalInfo *l, XMotionEvent *event, OpInfo *info) 
{
	XRectangle	undo;

	if (info->surface == opWindow)
		return;

	XDrawLine(XtDisplay(w), info->drawable, info->filled_gc,
			 l->startX, l->startY, event->x, event->y);
	if (!info->isFat)
		XDrawLine(XtDisplay(w), XtWindow(w), info->filled_gc,
			 l->startX, l->startY, event->x, event->y);

	UndoGrow(w, event->x, event->y);

	undo.x      = MIN(l->startX, event->x);
	undo.y      = MIN(l->startY, event->y);
	undo.width  = MAX(l->startX, event->x) - undo.x + 1;
	undo.height = MAX(l->startY, event->y) - undo.y + 1;

	l->startX = event->x;
	l->startY = event->y;

	PwUpdate(w, &undo, False);
}

/*
**  Those public functions
*/
void *PencilAdd(Widget w)
{
	LocalInfo	*l = (LocalInfo*)XtMalloc(sizeof(LocalInfo));

	XtVaSetValues(w, XtNcompress, False, NULL);

	OpAddEventHandler(w, opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpAddEventHandler(w, opPixmap, ButtonMotionMask, FALSE, (OpEventProc)motion, l);

	SetPencilCursor(w);

	return l;
}
void PencilRemove(Widget w, LocalInfo *l)
{
	OpRemoveEventHandler(w, opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpRemoveEventHandler(w, opPixmap, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	XtFree((XtPointer)l);
}
