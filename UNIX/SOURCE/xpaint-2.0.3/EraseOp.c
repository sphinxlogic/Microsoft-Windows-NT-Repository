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

#define WIDTH	15
#define HEIGHT	10

static void	press(Widget w, void *junk, XButtonEvent *event, OpInfo *info) 
{
	XRectangle	undo;

	if (info->surface == opWindow && info->isFat)
		return;

	UndoStart(w, info);

	XFillRectangle(XtDisplay(w), info->drawable, info->base_gc, 
			event->x, event->y, WIDTH, HEIGHT);

	if (info->surface == opPixmap) {
		XYtoRECT(event->x, event->y, event->x + WIDTH, event->y + HEIGHT, &undo);
		UndoSetRectangle(w, &undo);
		PwUpdate(w, &undo, False);
	}
}

static void	motion(Widget w, void *junk, XMotionEvent *event, OpInfo *info) 
{
	XRectangle	undo;

	if (info->surface == opWindow && info->isFat)
		return;

	XFillRectangle(XtDisplay(w), info->drawable, info->base_gc, 
			event->x, event->y, WIDTH, HEIGHT);

	if (info->surface == opPixmap) {
		XYtoRECT(event->x, event->y, event->x + WIDTH, event->y + HEIGHT, &undo);
		UndoGrow(w, event->x, event->y);
		UndoGrow(w, event->x + WIDTH, event->y + HEIGHT);
		PwUpdate(w, &undo, False);
	}
}

/*
**  Those public functions
*/
void *EraseAdd(Widget w)
{
        XtVaSetValues(w, XtNcompress, False, NULL);

	OpAddEventHandler(w, opWindow|opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, NULL);
	OpAddEventHandler(w, opWindow|opPixmap, ButtonMotionMask, FALSE, (OpEventProc)motion, NULL);
	return NULL;
}
void EraseRemove(Widget w)
{
	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, NULL);
	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonMotionMask, FALSE, (OpEventProc)motion, NULL);
}
