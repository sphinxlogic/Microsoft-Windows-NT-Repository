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
#include "PaintP.h"
#include "xpaint.h"

typedef void	(*func_t)(Widget, void *, XEvent *, OpInfo *);

typedef struct data_s {
	Widget		w;
	func_t		func;
        void            *data;
        int             mask, flag;
	int		surfMask;
        struct data_s   *next;
	OpInfo		*info;
} data_t;

static data_t	*list = NULL;

#if 0
static data_t	**widgetEvent(Widget w, data_t *look)
{
	static data_t	*d[10];
	data_t		*cur = list;
	int		n = 0;

	while (cur != NULL) {
		if (cur->mask == look->mask &&
		    cur->flag == look->flag &&
		    cur->func == look->func &&
		    cur->w    == w) 
			d[n++] = cur;
		cur = cur->next;
	}

	d[n] = NULL;

	return n == 0 ? NULL : d;
}
#endif

static void opHandleEvent(Widget w, XtPointer dataArg, XEvent *event, Boolean *junk)
{
	data_t		*data = (data_t*)dataArg;
	int		grid;
	PaintWidget	paint = (PaintWidget)w;
	OpInfo		*info = data->info;
	static Window	lastWindow  = None;
	static Display	*lastDisplay = None;
	static int	lastType;
	static int	lastX, lastY;
	Boolean		same;
	int		zoom;

	/*
	**  Figure out if there is a grid, either by
	**   choice or by zooming
	*/
	if ((info->zoom = paint->paint.zoom) == 0)
		info->zoom = 1;
	if (info->isFat = ((zoom = paint->paint.zoom) > 1))
		grid = paint->paint.zoom;
	else if (paint->paint.gridOn)
		grid = paint->paint.grid;
	else
		grid = 1;

	same = (event->xany.window == lastWindow) && 
	       (event->xany.display == lastDisplay) && 
               (event->xany.type == lastType);
	lastWindow  = event->xany.window;
	lastDisplay = event->xany.display;
	lastType    = event->xany.type;

	/*
	**  Snap events to the grid
	*/
	if (event->type == ButtonPress || event->type == ButtonRelease) {
		info->realX = event->xbutton.x;
		info->realY = event->xbutton.y;
	} else if (event->type == MotionNotify) {
		info->realX = event->xmotion.x;
		info->realY = event->xmotion.y;
	}

	if (((event->type == ButtonPress) ||
	     (event->type == ButtonRelease) ||
	     (event->type == MotionNotify)) && grid > 1) {
		switch (event->type) {
		case ButtonPress:
		case ButtonRelease:
			event->xbutton.x = event->xbutton.x - 
						(event->xbutton.x % grid);
			event->xbutton.y = event->xbutton.y - 
						(event->xbutton.y % grid);
			if (same && lastX == event->xbutton.x && lastY == event->xbutton.y)
				return;
			break;
		case MotionNotify:
			event->xmotion.x = event->xmotion.x - 
						(event->xmotion.x % grid);
			event->xmotion.y = event->xmotion.y - 
						(event->xmotion.y % grid);
			if (same && lastX == event->xmotion.x && lastY == event->xmotion.y)
				return;
			break;
		}
		lastX = event->xmotion.x;
		lastY = event->xmotion.y;
	} else {
		lastX = lastY = -1;
	}

	if (event->type == MotionNotify) {
		info->x = (event->xmotion.x / zoom) + paint->paint.zoomX;
		info->y = (event->xmotion.y / zoom) + paint->paint.zoomY;
	} else if (event->type == ButtonPress || event->type == ButtonRelease) {
		info->x = (event->xbutton.x / zoom) + paint->paint.zoomX;
		info->y = (event->xbutton.y / zoom) + paint->paint.zoomY;
	}
	/*
	**  In fatbits we can't tell the difference 
	**   between gridding an fat bits.
	*/
	if (info->isFat) {
		info->realX = info->x;
		info->realY = info->y;
	} else {
		info->realX = info->realX + paint->paint.zoomX;
		info->realY = info->realY + paint->paint.zoomY;
	}

	if (data->surfMask & opWindow) {
		info->surface  = opWindow;
		info->drawable = event->xany.window;
		data->func(w, data->data, event, info);
	}
	if (event->type == MotionNotify) {
		event->xmotion.x = info->x;
		event->xmotion.y = info->y;
	} else if (event->type == ButtonPress || event->type == ButtonRelease) {
		event->xbutton.x = info->x;
		event->xbutton.y = info->y;
	}

	if (data->surfMask & opPixmap) {
		info->surface = opPixmap;
		info->drawable = paint->paint.undo ? paint->paint.undo->pixmap : None;
		data->func(w, data->data, event, info);
	}
}

void OpAddEventHandler(Widget w, int surfMask, int mask, Boolean flag, 
		void (*func)(Widget,void *, XEvent*, OpInfo *), void *data)
{
	PaintWidget	paint = (PaintWidget)w;
	data_t		*new = (data_t *)XtMalloc(sizeof(data_t));
	OpInfo		*info;

	if (new == NULL)
		return;

	new->w        = w;
	new->func     = func;
	new->data     = data;
	new->mask     = mask;
	new->flag     = flag;
	new->surfMask = surfMask;

	info = new->info = (OpInfo*)XtMalloc(sizeof(OpInfo));
	info->refCount = 1;

	/*
	**  Now build the approprate info structure
	*/
	if (paint->paint.paint == None) {
		/*
		** If this is a paint widget, this is easy
		*/
		info->filled_gc = paint->paint.fgc;
		info->base_gc = paint->paint.igc;
	} else {
		/*
		** For a fatbits paint widget get paint information
		*/
		info->filled_gc = ((PaintWidget)paint->paint.paint)->paint.fgc;
		info->base_gc   = ((PaintWidget)paint->paint.paint)->paint.igc;
	}

	new->next = list;
	list      = new;

        XtAddEventHandler(w, mask, flag, opHandleEvent, (XtPointer)new);
}

void OpRemoveEventHandler(Widget w, int surfMask, int mask, Boolean flag, 
		void (*func)(Widget, void *, XEvent*, OpInfo *), void *data)
{
	data_t	*cur   = list;
	data_t	**prev = &list;

	while (cur != NULL) {
		if (cur->w        == w    &&
		    cur->data     == data &&
		    cur->mask     == mask &&
		    cur->flag     == flag &&
		    cur->surfMask == surfMask &&
		    cur->func     == func)
			break;
		prev = &cur->next;
		cur = cur->next;
	}

	if (cur == NULL)
		return;

        XtRemoveEventHandler(w, mask, flag, opHandleEvent, (XtPointer)cur);

	*prev = cur->next;

	cur->info->refCount--;
	if (cur->info->refCount == 0)
		XtFree((XtPointer)cur->info);

	XtFree((XtPointer)cur);
}
