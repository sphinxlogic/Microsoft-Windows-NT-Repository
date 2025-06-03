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
	Boolean		area;		/* area or box */

	Widget		w;
	Drawable	drawable;

	Boolean		move, state;
	GC		gc, write_gc;
	int		id, pos;	/* Timeout id, current shift pos */

	int		pressX, pressY, offX, offY;
	Boolean		drawn;

	int		wx, wy, dx, dy;

	/*
	**  The point region for area operators
	*/
	XPoint		real_p[512];
	XPoint		window_p[512];
	int		npoint;

	/*
	**  The rectangles for a box
	*/
	int		lastX, lastY;
	XRectangle	rect;

	/*
	**  The interesting stuff, for the active region
	**    the important part of pixBox is the width, height
	**    the x,y is where it is presently located on the screen.
	*/
	Boolean		loose;		/* is it attached to the drawing area? */
	Pixmap		mask;
	XRectangle	pixBox, startRect;
} LocalInfo;

#define MKRECT(rect, sx, sy, ex, ey) do {                               \
                        (rect)->x      = MIN(sx, ex);                   \
                        (rect)->y      = MIN(sy, ey);                   \
                        (rect)->width  = MAX(sx, ex) - (rect)->x;       \
                        (rect)->height = MAX(sy, ey) - (rect)->y;       \
                } while (0)

#define	DELAY		150

/*
**  Generic drawers and flashers.
*/
static void	draw(LocalInfo *l, Boolean flag)
{
	if (l->state == flag)
		return;

	if (l->area) {
		XDrawLines(XtDisplay(l->w), XtWindow(l->w), l->gc,
				l->window_p, l->npoint, CoordModePrevious);
	} else {
		XDrawRectangles(XtDisplay(l->w), XtWindow(l->w), l->gc, &l->rect, 1);
	}
	l->state = flag;
}
static void	flash(LocalInfo *l)
{
	XGCValues	values;

	draw(l, False);

	values.dash_offset = (++l->pos) % 8;
	XChangeGC(XtDisplay(l->w), l->gc, GCDashOffset, &values);

	draw(l, True);

	l->id = XtAppAddTimeOut(Global.appContext, DELAY, (XtTimerCallbackProc)flash, l);
}

/*
**  Get/Write back the pixmap 
*/
void writePixmap(LocalInfo *l)
{
	if (!l->loose)
		return;

	l->loose = False;
	PwWriteRegion(l->w);
	PwSetDrawn(l->w, False);
	UndoSetRectangle(l->w, RectUnion(&l->pixBox, &l->startRect));
}

void getPixmap(LocalInfo *l, OpInfo *info)
{
	PwUndoStart(l->w);
	PwClearRegion(l->w, False);
	PwSetDrawn(l->w, True);

	l->startRect = l->pixBox;
	l->loose     = True;
}

/*
**  press
*/
static void	press(Widget w, LocalInfo *l, XButtonEvent *event, OpInfo *info) 
{
	if (info->surface == opWindow) {
		l->w     = w;
	
		l->wx = event->x;
		l->wy = event->y;

		l->drawn = False;

		return;
	}
	
	/*
	**  Must be on the surface == pixmap now
	*/

	l->move  = False;

	if (l->id != (XtIntervalId)NULL) {
		draw(l, False);
		XtRemoveTimeOut(l->id);

		l->id = (XtIntervalId)NULL;
		l->move = (event->x >= l->pixBox.x && 
			   event->x <= l->pixBox.x + l->pixBox.width &&
			   event->y >= l->pixBox.y && 
			   event->y <= l->pixBox.y + l->pixBox.height);
	}
	
	if (l->move) {
		l->offX     = info->x - l->pixBox.x;
		l->offY     = info->y - l->pixBox.y;

		l->pixBox.x = info->x - l->offX;
		l->pixBox.y = info->y - l->offY;

		if (!l->loose) 
			getPixmap(l, info);
		XtVaSetValues(w, XtNcompress, True, NULL);
	} else {
		if (l->loose)
			writePixmap(l);

		l->real_p[0].x = info->x;
		l->real_p[0].y = info->y;
		l->window_p[0].x = l->wx;
		l->window_p[0].y = l->wy;
		l->npoint = 1;
	}
}

/*
**  motion
*/
static void	motionBoxBand(Widget w, LocalInfo *l, XMotionEvent *event, OpInfo *info) 
{
	XRectangle	rect;

	if (l->drawn) {
		MKRECT(&rect, l->lastX, l->lastY, l->window_p[0].x, l->window_p[0].y);
		XDrawRectangles(XtDisplay(w), XtWindow(w), Global.gcx, &rect, 1);
	}

	l->lastX = event->x;
	l->lastY = event->y;

	if (l->drawn = (l->window_p[0].x != l->lastX && l->window_p[0].y != l->lastY)) {
		MKRECT(&rect, l->lastX, l->lastY, l->window_p[0].x, l->window_p[0].y);
		XDrawRectangles(XtDisplay(w), XtWindow(w), Global.gcx, &rect, 1);
	}
}
static void	motionAreaBand(Widget w, LocalInfo *l, XMotionEvent *event, OpInfo *info) 
{
	if (l->npoint == XtNumber(l->real_p) - 1)
		return;

	l->window_p[l->npoint].x = event->x;
	l->window_p[l->npoint].y = event->y;
	l->real_p[l->npoint].x = info->x;
	l->real_p[l->npoint].y = info->y;

	XDrawLine(XtDisplay(w), XtWindow(w), Global.gcx,
		l->window_p[l->npoint-1].x,
		l->window_p[l->npoint-1].y,
		l->window_p[l->npoint].x,
		l->window_p[l->npoint].y);
	XDrawPoint(XtDisplay(w), XtWindow(w), Global.gcx,
		l->window_p[l->npoint].x,
		l->window_p[l->npoint].y);
		
		
	l->npoint++;
}
static void	motionDrag(Widget w, LocalInfo *l, XMotionEvent *event, OpInfo *info) 
{
	l->pixBox.x = info->x - l->offX;
	l->pixBox.y = info->y - l->offY;

	PwMoveRegion(w, l->pixBox.x, l->pixBox.y);
}
static void	motion(Widget w, LocalInfo *l, XMotionEvent *event, OpInfo *info) 
{
	if (l->move)
		motionDrag(w, l, event, info);
	else if (l->area)
		motionAreaBand(w, l, event, info);
	else
		motionBoxBand(w, l, event, info);
}

static Boolean	releaseBoxBand(Widget w, LocalInfo *l, XButtonEvent *event, OpInfo *info) 
{
	XRectangle	rect;

	if (l->drawn) {
		MKRECT(&rect, l->lastX, l->lastY, l->window_p[0].x, l->window_p[0].y);
		XDrawRectangles(XtDisplay(w), XtWindow(w), Global.gcx, &rect, 1);
	}

	MKRECT(&l->rect, event->x, event->y, l->window_p[0].x, l->window_p[0].y);
	XYtoRECT(l->real_p[0].x, l->real_p[0].y, info->x, info->y, &l->pixBox);

	PwLoadRegion(w, &l->pixBox, None);

	return True;
}
static Boolean	releaseAreaBand(Widget w, LocalInfo *l, XButtonEvent *event, OpInfo *info) 
{
	int	xmax, xmin, ymax, ymin;
	int	i;
	GC	gc;
	XPoint	tmp;

	XDrawLines(XtDisplay(w), XtWindow(w), Global.gcx,
			l->window_p, l->npoint, CoordModeOrigin);
	
	if (l->npoint <= 2)
		return False;
	
	l->window_p[l->npoint] = l->window_p[0];
	l->real_p[l->npoint]   = l->real_p[0];
	l->npoint++;
	
	xmin = xmax = l->real_p[0].x;		
	ymin = ymax = l->real_p[0].y;
	for (i = l->npoint - 1; i > 0; i--) {
		xmin = MIN(xmin, l->real_p[i].x);
		ymin = MIN(ymin, l->real_p[i].y);
		xmax = MAX(xmax, l->real_p[i].x);
		ymax = MAX(ymax, l->real_p[i].y);

		l->window_p[i].x = l->window_p[i].x - l->window_p[i-1].x;
		l->window_p[i].y = l->window_p[i].y - l->window_p[i-1].y;
		l->real_p[i].x   = l->real_p[i].x   - l->real_p[i-1].x;
		l->real_p[i].y   = l->real_p[i].y   - l->real_p[i-1].y;
	}

	XYtoRECT(xmin, ymin, xmax, ymax, &l->pixBox);

	l->mask = XCreatePixmap(XtDisplay(w), XtWindow(w), 
				l->pixBox.width, l->pixBox.height, 1);
	gc = XCreateGC(XtDisplay(w), l->mask, 0, 0);
	XSetForeground(XtDisplay(w), gc, False);
	XFillRectangle(XtDisplay(w), l->mask, gc, 0, 0, l->pixBox.width, l->pixBox.height);
	XSetForeground(XtDisplay(w), gc, True);
	tmp = l->real_p[0];
	l->real_p[0].x = l->dx = l->real_p[0].x - xmin;
	l->real_p[0].y = l->dy = l->real_p[0].y - ymin;
	XFillPolygon(XtDisplay(w), l->mask, gc, l->real_p, l->npoint, 
				Complex, CoordModePrevious);
	l->real_p[0] = tmp;

	PwLoadRegion(w, &l->pixBox, l->mask);
	
	return True;
}
static void	releaseDrag(Widget w, LocalInfo *l, XButtonEvent *event, OpInfo *info) 
{
	if (l->area) {
		l->window_p[0].x = (l->pixBox.x + l->dx) * info->zoom;
		l->window_p[0].y = (l->pixBox.y + l->dy) * info->zoom;
	} else {
		l->rect.x  = (info->x - l->offX) * info->zoom;
		l->rect.y  = (info->y - l->offY) * info->zoom;
	}
}
static void	release(Widget w, LocalInfo *l, XButtonEvent *event, OpInfo *info) 
{
	Boolean	flag = True;

	if (l->move)
		releaseDrag(w, l, event, info);
	else if (l->area)
		flag = releaseAreaBand(w, l, event, info);
	else
		flag = releaseBoxBand(w, l, event, info);

	if (flag) {
		draw(l, True);

		l->id = XtAppAddTimeOut(Global.appContext, DELAY, (XtTimerCallbackProc)flash, l);
		XtVaSetValues(w, XtNcompress, !l->area, NULL);
	}
}

void finish(Widget w, LocalInfo *l)
{
	if (l->id != (XtIntervalId)NULL) {
		draw(l, False);
		XtRemoveTimeOut(l->id);

		l->id = (XtIntervalId)NULL;
	}

	if (l->loose)
		 writePixmap(l);
}

/*
**  Those public functions
*/
void *SelectBoxAdd(Widget w)
{
	LocalInfo	*l = (LocalInfo*)XtMalloc(sizeof(LocalInfo));
	static char     dashList[] = { 4, 4 };

	l->area = False;
	l->id   = (XtIntervalId)NULL;
	l->mask = None;
	l->pos  = 0;
	l->loose = False;

	XtVaSetValues(w, XtNcompress, True, NULL);

	OpAddEventHandler(w, opWindow|opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpAddEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpAddEventHandler(w, opWindow, ButtonReleaseMask, FALSE, (OpEventProc)release, l);

	l->gc = XCreateGC(XtDisplay(w), XtWindow(w), 0, NULL);
	XCopyGC(XtDisplay(w), Global.gcx, ~0, l->gc);
	XSetLineAttributes(XtDisplay(w), l->gc, 0,
				LineOnOffDash, CapButt, JoinMiter);
	XSetDashes(XtDisplay(w), l->gc, 0, dashList, XtNumber(dashList));

	SetCrossHairCursor(w);

	return l;
}
void SelectBoxRemove(Widget w, LocalInfo *l)
{
	finish(w, l);

	XFreeGC(XtDisplay(w), l->gc);

	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpRemoveEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpRemoveEventHandler(w, opWindow, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
}

void *SelectAreaAdd(Widget w)
{
	LocalInfo	*l = (LocalInfo*)XtMalloc(sizeof(LocalInfo));
	static char     dashList[] = { 4, 4 };

	l->area = True;
	l->mask = None;
	l->loose = False;
	l->id   = (XtIntervalId)NULL;
	l->pos  = 0;

	XtVaSetValues(w, XtNcompress, False, NULL);

	OpAddEventHandler(w, opWindow|opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpAddEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpAddEventHandler(w, opWindow, ButtonReleaseMask, FALSE, (OpEventProc)release, l);

	l->gc = XCreateGC(XtDisplay(w), XtWindow(w), 0, NULL);
	XCopyGC(XtDisplay(w), Global.gcx, ~0, l->gc);
	XSetLineAttributes(XtDisplay(w), l->gc, 0,
				LineOnOffDash, CapButt, JoinMiter);
	XSetDashes(XtDisplay(w), l->gc, 0, dashList, XtNumber(dashList));

	SetPencilCursor(w);

	return l;
}
void SelectAreaRemove(Widget w, LocalInfo *l)
{
	finish(w, l);

	XFreeGC(XtDisplay(w), l->gc);

	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpRemoveEventHandler(w, opWindow, ButtonMotionMask, FALSE, (OpEventProc)motion, l);
	OpRemoveEventHandler(w, opWindow, ButtonReleaseMask, FALSE, (OpEventProc)release, l);
}

void SelectPasteSave(Widget w, void *l)
{
	finish(w, (LocalInfo*)l);
}

void SelectPasteStart(Widget w, void *lArg, XRectangle *rect)
{
	LocalInfo	*l = (LocalInfo *)lArg;
	int		zoom;

	l->loose = True;
	l->pixBox = *rect;
	l->w = w;

	PwUndoStart(w);
	PwSetDrawn(w, True);

	l->startRect.x = 0;
	l->startRect.y = 0;
	l->startRect.width = 0;
	l->startRect.height = 0;

	/*
	**  For now fake this into an 'box' selection
	*/
	XtVaGetValues(w, XtNzoom, &zoom, NULL);

	l->area = False;
	l->rect.x = rect->x * zoom;
	l->rect.y = rect->y * zoom;
	l->rect.width = rect->width * zoom;
	l->rect.height = rect->height * zoom;

	draw(l, True);
	l->id = XtAppAddTimeOut(Global.appContext, DELAY, (XtTimerCallbackProc)flash, l);

	XtVaSetValues(w, XtNcompress, True, NULL);
}
