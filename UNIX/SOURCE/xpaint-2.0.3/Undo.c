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

#include <X11/IntrinsicP.h>
#include "xpaint.h"
#include "PaintP.h"

void UndoSwap(Widget w)
{
	PaintWidget	pw = (PaintWidget)w;

	if (pw->paint.undo == NULL)
		pw->paint.undo = pw->paint.top;
	else
		pw->paint.undo = pw->paint.undo->next;

	PwUpdate(w, NULL, True);
}

Pixmap PwUndoStart(Widget w)
{
	PaintWidget		pw = (PaintWidget)w;
	UndoStack		*cur;
	PaintWidget		orig = pw;

	/*
	**  If we are a view of another widget, get the parent
	*/
	if (pw->paint.paint != None)
		pw = (PaintWidget)(pw->paint.paint);

	pw->paint.dirty = True;

	if (pw->paint.top == NULL) 
		return pw->paint.base;

	if (pw->paint.undo != pw->paint.top) {
		/*
		**  In the middle of the undo list, get either the
		**   previous buffer or the last buffer.
		*/
		if (pw->paint.undo != NULL) {
			cur = pw->paint.undo->prev;
		} else {
			cur = pw->paint.top;
			while (cur->next != NULL)
				cur = cur->next;
		}
	} else {
		/*
		**  At the top, update the base, with
		**  the undo buffer above it.
		**  the make that buffer current,
		**  and move it to the top
		*/
		cur = pw->paint.undo;
		while (cur != NULL && cur->next != NULL)
			cur = cur->next;

		if (cur->box.width != 0 && cur->box.height != 0)
			XCopyArea(XtDisplay(pw), cur->pixmap, pw->paint.base,
					pw->paint.gc,
					cur->box.x, cur->box.y,
					cur->box.width, cur->box.height,
					cur->box.x, cur->box.y);

		if (cur->prev != NULL)
			cur->prev->next = NULL;
		if (pw->paint.undo == cur)
			pw->paint.undo = NULL;
		cur->prev = NULL;

		if (pw->paint.top != cur) {
			cur->next     = pw->paint.top;
			pw->paint.top = cur;
			if (cur->next != NULL)
				cur->next->prev = cur;
		}
	}

	PwUpdateDrawable(w, cur->pixmap, NULL);

	cur->box.x      = 0;
	cur->box.y      = 0;
	cur->box.width  = 0;
	cur->box.height = 0;

	pw->paint.undo = cur;

	if (orig != pw)
		orig->paint.undo = pw->paint.undo;

	return cur->pixmap;
}

void UndoStart(Widget w, OpInfo *info)
{
	if (info->surface != opPixmap)
		return;

	info->drawable = PwUndoStart(w);
}


void	UndoStartPoint(Widget w, OpInfo *info, int x, int y)
{
	PaintWidget	pw = (PaintWidget)w;

	if (XtClass(w) != paintWidgetClass || info->surface != opPixmap)
		return;

	UndoStart(w, info);

	pw->paint.undo->box.x      = x - pw->paint.lineWidth;
	pw->paint.undo->box.y      = y - pw->paint.lineWidth;
	pw->paint.undo->box.width  = 1 + pw->paint.lineWidth * 2;
	pw->paint.undo->box.height = 1 + pw->paint.lineWidth * 2;
}

void UndoGrow(Widget w, int x, int y)
{
	PaintWidget	pw = (PaintWidget)w;
	XRectangle	*rect;
	int		dx, dy;

	if (XtClass(pw) != paintWidgetClass)
		return;

	rect = &pw->paint.undo->box;

	rect->x     += pw->paint.lineWidth;
	rect->y     += pw->paint.lineWidth;
	rect->width -= pw->paint.lineWidth * 2;
	rect->height-= pw->paint.lineWidth * 2;

	dx = x - rect->x;
	dy = y - rect->y;

	if (dx > 0) {
		rect->width = MAX(rect->width, dx);
	} else {
		rect->width = (int)rect->width - dx + 1;
		rect->x     = x;
	}

	if (dy > 0) {
		rect->height = MAX(rect->height, dy) + 1;
	} else {
		rect->height = (int)rect->height - dy + 1;
		rect->y      = y;
	}

	rect->x     -= pw->paint.lineWidth;
	rect->y     -= pw->paint.lineWidth;
	rect->width += pw->paint.lineWidth * 2;
	rect->height+= pw->paint.lineWidth * 2;
}

void UndoSetRectangle(Widget w, XRectangle *rect)
{
	PaintWidget	pw = (PaintWidget)w;

	if (XtClass(w) != paintWidgetClass || pw->paint.undo == NULL)
		return;

	pw->paint.undo->box = *rect;

	pw->paint.undo->box.x     -= pw->paint.lineWidth;
	pw->paint.undo->box.y     -= pw->paint.lineWidth;
	pw->paint.undo->box.width += pw->paint.lineWidth * 2 + 1;
	pw->paint.undo->box.height+= pw->paint.lineWidth * 2 + 1;
}

void	UndoStartRectangle(Widget w, OpInfo *info, XRectangle *rect)
{
	if (XtClass(w) != paintWidgetClass || info->surface != opPixmap)
		return;

	UndoStart(w, info);
	UndoSetRectangle(w, rect);
}
