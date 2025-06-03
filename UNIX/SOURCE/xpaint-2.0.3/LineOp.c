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
} LocalInfo;

static void	press(Widget w, LocalInfo *l, XButtonEvent *event, OpInfo *info) 
{
	l->rx     = info->x;
	l->ry     = info->y;
	l->startX = event->x;
	l->startY = event->y;

	l->drawn = False;
}

static void	motion(Widget w, LocalInfo *l, XMotionEvent *event, OpInfo *info) 
{
	if (l->drawn) {
		XDrawLine(XtDisplay(w), info->drawable, Global.gcx,
				l->startX, l->startY, l->endX, l->endY);
	}

	l->endX = event->x;
	l->endY = event->y;

	/*
	**  Really set this flag in the if statement
	*/
	if (l->drawn = (l->startX != l->endX || l->startY != l->endY)) {
		XDrawLine(XtDisplay(w), info->drawable, Global.gcx,
				l->startX, l->startY, l->endX, l->endY);
	}
}

static void	release(Widget w, LocalInfo *l, XButtonEvent *event, OpInfo *info) 
{
	XRectangle	undo;

	if (l->drawn && info->surface == opWindow) 
		XDrawLine(XtDisplay(w), info->drawable, Global.gcx,
				l->startX, l->startY, l->endX, l->endY);

	if (info->surface == opWindow && info->isFat)
		return;

	UndoStart(w, info);

	XDrawLine(XtDisplay(w), info->drawable, info->filled_gc,
			l->rx, l->ry, event->x, event->y);

	if (info->surface == opPixmap) {
		XYtoRECT(l->rx, l->ry, event->x, event->y, &undo);
		UndoSetRectangle(w, &undo);
		PwUpdate(w, &undo, False);
	}
}

/*
**  Those public functions
*/
void *LineAdd(Widget w)
{
	LocalInfo	*l = (LocalInfo *)XtMalloc(sizeof(LocalInfo));
        XtVaSetValues(w, XtNcompress, True, NULL);

	OpAddEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpAddEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpAddEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
	SetCrossHairCursor(w);
	return l;
}
void LineRemove(Widget w, LocalInfo *l)
{
	OpRemoveEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpRemoveEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
}
