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

#define	FILL		0x02
#define CIRCLE		0x01
#define	ISFILL(x)	((x) & FILL)
#define ISCIRCLE(x)	((x) & CIRCLE)

typedef struct {
	int	rx, ry, startX, startY, endX, endY, drawn, typeFlag, flag;
} LocalInfo;

/*
**  Warning this define picks up "flag" from scope
*/
#define MKRECT(rect, sx, sy, ex, ey, flag) do {				\
	(rect)->x      = MIN(sx, ex);					\
	(rect)->y      = MIN(sy, ey);					\
	(rect)->width  = MAX(sx, ex) - (rect)->x;			\
	(rect)->height = MAX(sy, ey) - (rect)->y;			\
	if (ISCIRCLE(flag)) {						\
		(rect)->width = MIN((rect)->width, (rect)->height);	\
		(rect)->height = (rect)->width; 			\
	}								\
	if ((rect)->x != sx) (rect)->x = sx - (rect)->width;		\
 	if ((rect)->y != sy) (rect)->y = sy - (rect)->height;		\
	(rect)->angle1 = 0;						\
	(rect)->angle2 = 360*64;					\
	} while (0)

static void	press(Widget w, LocalInfo *l, 
			XButtonEvent *event, OpInfo *info) 
{
	l->rx   = info->x;
	l->ry   = info->y;
	l->endX = l->startX = event->x;
	l->endY = l->startY = event->y;

	l->typeFlag = (event->state & ShiftMask) ? CIRCLE : 0;

	l->drawn = False;
}

static void	motion(Widget w, LocalInfo *l, 
			XMotionEvent *event, OpInfo *info) 
{
	XArc	rect;

	if (l->drawn) {
		MKRECT(&rect, l->startX,l->startY,l->endX,l->endY,l->typeFlag);
		XDrawArcs(XtDisplay(w), info->drawable, Global.gcx, &rect, 1);
	}

	l->endX = event->x;
	l->endY = event->y;

	/*
	**  Really set this flag in the if statement
	*/
	if (l->drawn = (l->startX != l->endX || l->startY != l->endY)) {
		MKRECT(&rect, l->startX,l->startY,l->endX,l->endY,l->typeFlag);
		XDrawArcs(XtDisplay(w), info->drawable, Global.gcx, &rect, 1);
	}
}

static void	release(Widget w, LocalInfo *l, 
			XButtonEvent *event, OpInfo *info) 
{
	XArc		rect;
	XRectangle	undo;

	if (l->drawn && info->surface == opWindow) {
		MKRECT(&rect, l->startX,l->startY,l->endX,l->endY,l->typeFlag);
		XDrawArcs(XtDisplay(w), info->drawable, Global.gcx, &rect, 1);
	}

	if (info->surface == opWindow && info->isFat)
		return;

	MKRECT(&rect, l->rx, l->ry, event->x, event->y, l->typeFlag);

	UndoStart(w, info);

	if (ISFILL(l->flag)) {
		XFillArcs(XtDisplay(w), info->drawable, 
				info->filled_gc, &rect, 1);
	} else {
		XDrawArcs(XtDisplay(w), info->drawable, 
				info->filled_gc, &rect, 1);
	}

	if (info->surface == opPixmap) {
		XYtoRECT(l->rx, l->ry, event->x, event->y, &undo);
		UndoSetRectangle(w, &undo);
		PwUpdate(w, &undo, False);
	}
}

/*
**  Those public functions
*/
void *CircleAdd(Widget w)
{
	LocalInfo	*l = (LocalInfo*)XtMalloc(sizeof(LocalInfo));
	l->flag = CIRCLE;
        XtVaSetValues(w, XtNcompress, True, NULL);
	OpAddEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpAddEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpAddEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
	SetCrossHairCursor(w);
	return l;
}
void CircleRemove(Widget w, LocalInfo *l)
{
	OpRemoveEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpRemoveEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
	XtFree((XtPointer)l);
}
void *FCircleAdd(Widget w)
{
	LocalInfo	*l = (LocalInfo*)XtMalloc(sizeof(LocalInfo));
	l->flag = CIRCLE|FILL;
        XtVaSetValues(w, XtNcompress, True, NULL);
	OpAddEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpAddEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpAddEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
	SetCrossHairCursor(w);
	return l;
}
void FCircleRemove(Widget w, LocalInfo *l)
{
	OpRemoveEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpRemoveEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
	XtFree((XtPointer)l);
}
void *OvalAdd(Widget w)
{
	LocalInfo	*l = (LocalInfo*)XtMalloc(sizeof(LocalInfo));
	l->flag = 0;
        XtVaSetValues(w, XtNcompress, True, NULL);
	OpAddEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpAddEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpAddEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
	SetCrossHairCursor(w);
	return l;
}
void OvalRemove(Widget w, LocalInfo *l)
{
	OpRemoveEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpRemoveEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
	XtFree((XtPointer)l);
}
void *FOvalAdd(Widget w)
{
	LocalInfo	*l = (LocalInfo*)XtMalloc(sizeof(LocalInfo));
	l->flag = FILL;
        XtVaSetValues(w, XtNcompress, True, NULL);

	OpAddEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpAddEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpAddEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
	SetCrossHairCursor(w);
	return l;
}
void FOvalRemove(Widget w, LocalInfo *l)
{
	OpRemoveEventHandler(w, opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpRemoveEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
	XtFree((XtPointer)l);
}
