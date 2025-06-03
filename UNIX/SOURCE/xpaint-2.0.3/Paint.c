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
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xaw/Viewport.h>
#include "PaintP.h"
#include "xpaint.h"

extern XRectangle *RectIntersect(XRectangle *, XRectangle *);

#define _GET_PIXMAP(pw) \
		(pw)->paint.undo ? (pw)->paint.undo->pixmap : (pw)->paint.base
#define GET_PIXMAP(pw) pw->paint.paint ? \
		_GET_PIXMAP((PaintWidget)pw->paint.paint) : _GET_PIXMAP(pw)

typedef	void		(*func_t)(Widget, void *, XEvent *, Boolean);
typedef struct data_s {
	func_t		func;
	void		*data;
	int		mask, flag;
	struct data_s	*next;
} data_t;

static void pwSetPixmap(PaintWidget w, Pixmap pix, int flag);
static void fatCallback(Widget, XtPointer, XtPointer);

static XtResource resources[] = {
#define offset(field) XtOffset(PaintWidget, paint.field)
    /* {name, class, type, size, offset, default_type, default_addr}, */
    { XtNcompress, XtCCompress, XtRBoolean, sizeof(Boolean),
	  offset(compress), XtRImmediate, (XtPointer)False },
    { XtNdrawWidth, XtCDrawWidth, XtRInt, sizeof(int),
	  offset(drawWidth), XtRImmediate, (XtPointer)512 },
    { XtNdrawHeight, XtCDrawHeight, XtRInt, sizeof(int),
	  offset(drawHeight), XtRImmediate, (XtPointer)512 },
    { XtNlineWidth, XtCLineWidth, XtRInt, sizeof(int),
	  offset(lineWidth), XtRImmediate, (XtPointer)0 },
    { XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct*),
	  offset(fontInfo), XtRString, XtDefaultFont },
    { XtNpattern, XtCPattern, XtRPixmap, sizeof(Pixmap),
	  offset(pattern), XtRPixmap, (XtPointer)None },
    { XtNpixmap, XtCPixmap, XtRPixmap, sizeof(Pixmap),
	  offset(sourcePixmap), XtRPixmap, (XtPointer)NULL },
    { XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	  offset(foreground), XtRString, XtDefaultForeground },
    { XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
	  offset(background), XtRString, XtDefaultBackground },
    { XtNgrid, XtCGrid, XtRInt, sizeof(int),
	  offset(grid), XtRImmediate, (XtPointer)10 },
    { XtNfatBack, XtCFatBack, XtRCallback, sizeof(XtPointer),
	  offset(fatcalls), XtRCallback, (XtPointer)None },
    { XtNundoSize, XtCUndoSize, XtRInt, sizeof(int),
	  offset(undoSize), XtRImmediate, (XtPointer)1 },
    { XtNzoom, XtCZoom, XtRInt, sizeof(int),
	  offset(zoom), XtRImmediate, (XtPointer)1 },
    { XtNzoomX, XtCZoomX, XtRInt, sizeof(int),
	  offset(zoomX), XtRImmediate, (XtPointer)0 },
    { XtNzoomY, XtCZoomY, XtRInt, sizeof(int),
	  offset(zoomY), XtRImmediate, (XtPointer)0 },
    { XtNgridOn, XtCGridOn, XtRBoolean, sizeof(Boolean),
	  offset(gridOn), XtRImmediate, (XtPointer)False },
    { XtNpaint, XtCPaint, XtRWidget, sizeof(Widget),
	  offset(paint), XtRImmediate, (XtPointer)None },
    { XtNdirty, XtCDirty, XtRBoolean, sizeof(Boolean),
	  offset(dirty), XtRImmediate, (XtPointer)False },
    { XtNfillRule, XtCFillRule, XtRInt, sizeof(int),
	  offset(fillRule), XtRImmediate, (XtPointer)FillSolid },
#undef offset
};

/*
**  Some forward defines.
*/
static void InitializeProc(Widget, Widget);
static void DestroyProc(Widget);
static void ResizeProc(Widget);
static void ExposeProc(Widget, XExposeEvent *);
static void RealizeProc(Widget, Mask *, XSetWindowAttributes *);
static Boolean SetValuesProc(Widget cur, Widget request, Widget new);
static int QueryGeometryProc(Widget w, XtWidgetGeometry *proposed,
				XtWidgetGeometry *reply);


PaintClassRec paintClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"Paint",
    /* widget_size		*/	sizeof(PaintRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	(XtInitProc)InitializeProc,
    /* initialize_hook		*/	NULL,
    /* realize			*/	RealizeProc,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	FALSE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	DestroyProc,
    /* resize			*/	ResizeProc,
    /* expose			*/	(XtExposeProc)ExposeProc,
    /* set_values		*/	(XtSetValuesFunc)SetValuesProc,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry		*/	(XtGeometryHandler)QueryGeometryProc,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* paint fields */		
				0
  }
};

WidgetClass paintWidgetClass = (WidgetClass)&paintClassRec;

static void undoBuffer(PaintWidget w) 
{
	UndoStack	*new = (UndoStack*)XtMalloc(sizeof(UndoStack));

	if (new == NULL)
		return;

	new->box.x      = 0;
	new->box.y      = 0;
	new->box.width  = 0;
	new->box.height = 0;
	new->pixmap	= None;
	new->next	= w->paint.top;
	new->prev       = NULL;
	w->paint.top	= new;

	if (new->next != NULL)
		new->next->prev = new;
}

static void InitializeProc(Widget w, Widget newArg)
{
	PaintWidget	new = (PaintWidget)newArg;
	int		i;
	XGCValues	values;
	int		zoom = new->paint.zoom;

	if (zoom <= 0)
		zoom = new->paint.zoom = 1;

	if (new->paint.sourcePixmap != None) {
		Window		root;
		int		x, y;
		unsigned int	width, height, bw, depth;

		XGetGeometry(XtDisplay(w), new->paint.sourcePixmap,
				&root, &x, &y, &width, &height, &bw, &depth);
		
		new->paint.drawWidth  = width;
		new->paint.drawHeight = height;
	} else if (new->paint.paint != None) {
		PaintWidget	parent = (PaintWidget)new->paint.paint;

		new->paint.drawWidth  = parent->paint.drawWidth;
		new->paint.drawHeight = parent->paint.drawHeight;
	}

	if (new->core.width == 0)
		new->core.width = new->paint.drawWidth * zoom;
	if (new->core.height == 0)
		new->core.height = new->paint.drawHeight * zoom;

	new->paint.foreground = BlackPixelOfScreen(XtScreen(new));
	new->paint.background = WhitePixelOfScreen(XtScreen(new));

	values.foreground = new->paint.foreground;
	values.background = new->paint.background;
	new->paint.gc  = XtGetGC((Widget)new, GCForeground|GCBackground, &values);
	values.foreground = new->paint.background;
	values.background = new->paint.foreground;
	new->paint.igc = XtGetGC((Widget)new, GCForeground|GCBackground, &values);

	new->paint.fgc = None;
	new->paint.mgc = None;
	new->paint.tgc = None;

	/*
	**  Init the undo stuff
	*/
	new->paint.undo = NULL;
	new->paint.top = NULL;


	/*
	**  The region information
	*/
	new->paint.regionDrawn = False;
	new->paint.regionPix   = None;
	new->paint.regionMask  = None;

	if (new->paint.paint == None)
		for (i = 0; i < new->paint.undoSize; i++)
			undoBuffer(new);
}

static XRectangle *GetRectangle(PaintWidget w)
{
	static XRectangle	rect;
	Widget			parent = XtParent((Widget)w);

	if (XtClass(parent) == viewportWidgetClass) {
		rect.x      = -w->core.x;
		rect.y      = -w->core.y;
		rect.width  = parent->core.width;
		rect.height = parent->core.height;
		if (rect.x + rect.width > w->paint.drawWidth * w->paint.zoom)
			rect.width = w->paint.drawWidth * w->paint.zoom - rect.x;
		if (rect.y + rect.height > w->paint.drawHeight * w->paint.zoom)
			rect.height = w->paint.drawHeight * w->paint.zoom - rect.y;
	} else {
		rect.x      = 0;
		rect.y      = 0;
		rect.width  = w->paint.drawWidth * w->paint.zoom;
		rect.height = w->paint.drawHeight * w->paint.zoom;
	}

	return &rect;
}

static void RealizeProc(Widget w, Mask *valueMask, 
				XSetWindowAttributes *attributes)
{
	PaintWidget	pw = (PaintWidget)w;
	UndoStack	*cur;

	XtCreateWindow(w, InputOutput,
			DefaultVisualOfScreen(XtScreen(w)),
			*valueMask, attributes);

	pw->paint.tgc = XCreateGC(XtDisplay(w), XtWindow(w), 0, 0);

	/*
	**  Create the filled gc information
	*/
	pw->paint.fgc = XCreateGC(XtDisplay(w), XtWindow(w), 0, 0);
	XSetForeground(XtDisplay(w), pw->paint.fgc, pw->paint.foreground);
	if (pw->paint.paint == None) {
		XSetFillStyle(XtDisplay(w), pw->paint.fgc, pw->paint.fillRule);
		XSetLineAttributes(XtDisplay(w), pw->paint.fgc, 
				pw->paint.lineWidth, LineSolid, 
				CapButt, JoinMiter);
		if (pw->paint.pattern != None)
			XSetTile(XtDisplay(w), pw->paint.fgc, pw->paint.pattern);
		if (pw->paint.fontInfo != NULL && pw->paint.fontInfo->fid != None)
			XSetFont(XtDisplay(w), pw->paint.fgc, pw->paint.fontInfo->fid);
	}

	/*
	**  Create the backing undo buffers
	*/
	for (cur = pw->paint.top; cur != NULL; cur = cur->next)
		cur->pixmap = XCreatePixmap(XtDisplay(w), XtWindow(w), 
				pw->paint.drawWidth, pw->paint.drawHeight,
				DefaultDepthOfScreen(XtScreen(w)));

	/*
	**  If this paint widgets is the child of another paint
	**   widget then, either creating our own unique information
	**   or just use the information from our parent.
	*/
	if (pw->paint.paint == None) {
		pw->paint.base = XCreatePixmap(XtDisplay(w), XtWindow(w), 
				pw->paint.drawWidth, pw->paint.drawHeight,
				DefaultDepthOfScreen(XtScreen(w)));
		if (pw->paint.sourcePixmap != None)
			pwSetPixmap(pw, pw->paint.sourcePixmap, FALSE);
		else
			XFillRectangle(XtDisplay(w), pw->paint.base, 
				pw->paint.igc, 0,0, 
				pw->paint.drawWidth, pw->paint.drawHeight);
	} else {
		XtAddCallback((Widget)pw->paint.paint, XtNfatBack, fatCallback, w);
	}
}

static void DestroyProc(Widget w)
{
	PaintWidget	pw = (PaintWidget)w;
	UndoStack	*cur, *nxt;

	if (pw->paint.fgc != None)
		XFreeGC(XtDisplay(pw), pw->paint.fgc);
	if (pw->paint.tgc != None)
		XFreeGC(XtDisplay(pw), pw->paint.tgc);
	if (pw->paint.mgc != None)
		XFreeGC(XtDisplay(pw), pw->paint.mgc);
	if (pw->paint.igc != None)
		XtReleaseGC(w, pw->paint.igc);
	if (pw->paint.gc != None)
		XtReleaseGC(w, pw->paint.gc);

	if (pw->paint.paint) {
		XtRemoveCallback((Widget)pw->paint.paint, XtNfatBack, fatCallback, (XtPointer)w);
		return;
	}

	if (pw->paint.base != None)
		XFreePixmap(XtDisplay(pw), pw->paint.base);

	cur = pw->paint.top;
	while (cur != NULL) {
		nxt = cur->next;
		if (cur->pixmap != None)
			XFreePixmap(XtDisplay(pw), cur->pixmap);
		XtFree((XtPointer)cur);
		cur = nxt;
	}
}

static void ResizeProc(Widget w)
{
	/* printf("Resize called, this shouldn't happen!!\n"); */
}

#if 0
static XRectangle *overlap(XRectangle *a, XRectangle *b)
{
        static XRectangle       out;
        int                     w, h;

        out.x = MAX(a->x, b->x);
        out.y = MAX(a->y, b->y);
        w     = MIN(a->x + a->width,  b->x + b->width)  - out.x;
        h     = MIN(a->y + a->height, b->y + b->height) - out.y;

        if (w <= 0 || h <= 0)
                return NULL;

        out.width  = w;
        out.height = h;

        return &out;
}
#endif

/*
**  Update the zoomed region as specified by rect.
**    rect is in screen coordinates.
*/
static void zoomUpdate(PaintWidget pw, XRectangle *rect)
{
	Display		*dpy = XtDisplay(pw);
	Window		win  = XtWindow(pw);
	Pixmap		pix  = GET_PIXMAP(pw);
	int		zoom = pw->paint.zoom;
	GC		gc   = pw->paint.tgc;
	int		X, Y, sx, sy, x, y, cx, cy, w, h;
	Pixel		opix, npix;
	XImage		*xim;
	XRectangle	*isec;

	if ((isec = RectIntersect(rect, GetRectangle(pw))) == NULL)
		return;

	sx = isec->x / zoom;
	sy = isec->y / zoom;
	w  = (isec->width  + (isec->x % zoom) + zoom - 1) / zoom;
	h  = (isec->height + (isec->y % zoom) + zoom - 1) / zoom;

	xim = XGetImage(dpy, pix, sx + pw->paint.zoomX, sy + pw->paint.zoomY,
				w, h, ~0, ZPixmap);

	if (pw->paint.regionDrawn) {
		XRectangle	real;

		real.x      = sx;
		real.y      = sy;
		real.width  = w;
		real.height = h;
		
		if ((isec = RectIntersect(&real, &pw->paint.regionRect)) != NULL) {
			if (pw->paint.regionMask != None) {
				XImage	*mask, *data;
				int	dx = isec->x - real.x;
				int	dy = isec->y - real.y;

				mask = XGetImage(dpy, pw->paint.regionMask, 
							isec->x - pw->paint.regionRect.x,
							isec->y - pw->paint.regionRect.y,
							isec->width, isec->height,
							~0, ZPixmap);
				data = XGetImage(dpy, pw->paint.regionPix, 
							isec->x - pw->paint.regionRect.x,
							isec->y - pw->paint.regionRect.y,
							isec->width, isec->height,
							~0, ZPixmap);

				for (y = 0; y < isec->height; y++)
					for (x = 0; x < isec->width; x++)
						if (XGetPixel(mask, x, y))
							XPutPixel(xim, dx + x, dy + y,
									XGetPixel(data, x, y));

				XDestroyImage(data);
				XDestroyImage(mask);
			} else {
				XGetSubImage(XtDisplay(pw), pw->paint.regionPix,
						isec->x - pw->paint.regionRect.x,
                                                isec->y - pw->paint.regionRect.y,
						isec->width, isec->height,
						~0, ZPixmap, xim, 
						isec->x - real.x, 
						isec->y - real.y);
			}
		}
	}

	XSetForeground(dpy, gc, opix = XGetPixel(xim, 0, 0));
	for (cy = 0, Y = (y = sy) * zoom; cy < h; cy++, y++, Y += zoom) {
		for (cx = 0, X = (x = sx) * zoom; cx < w; cx++,x++, X += zoom) {
			if ((npix = XGetPixel(xim, cx, cy)) != opix)
				XSetForeground(dpy, gc, opix = npix);
			XFillRectangle(dpy, win, gc, X, Y, zoom - 1, zoom - 1);
		}
	}

	XDestroyImage(xim);
}

void PwUpdateDrawable(Widget w, Drawable draw, XRectangle *rect)
{
	PaintWidget	pw = (PaintWidget)w;
	XRectangle	all;
	Pixmap		pix = GET_PIXMAP(pw);

	if (rect == NULL) {
		rect = &all;
		all.x = all.y = 0;
		all.width  = pw->core.width;
		all.height = pw->core.height;
	}
	XCopyArea(XtDisplay(w), pix, draw, pw->paint.gc,
			rect->x, rect->y, rect->width, rect->height,
			rect->x, rect->y);

	if (pw->paint.regionDrawn) {
		if (pw->paint.regionMask != None) {
		        XSetClipOrigin(XtDisplay(w), pw->paint.tgc,
					pw->paint.regionRect.x, pw->paint.regionRect.y);

			XSetClipMask(XtDisplay(w), pw->paint.tgc, pw->paint.regionMask);
		}

		XCopyArea(XtDisplay(w), pw->paint.regionPix, draw, pw->paint.tgc,
				0, 0,
				pw->paint.regionRect.width,
				pw->paint.regionRect.height,
				pw->paint.regionRect.x,
				pw->paint.regionRect.y);

		if (pw->paint.regionMask != None) 
			XSetClipMask(XtDisplay(w), pw->paint.tgc, None);
	}
}

static void ExposeProc(Widget w, XExposeEvent *event)
{
	PaintWidget	pw = (PaintWidget)w;
	int		x, y, width, height;
	XRectangle	rect;

	if (!XtIsRealized(w))
		return;

	/*
	**  Clean up the event, since I do simulate them
	*/
	x      = event->x;
	y      = event->y;
	width  = event->width;
	height = event->height;

	if (x < 0) {
		width += x;
		x = 0;
	}
	if (y < 0) {
		height += y;
		y = 0;
	}
	if (width < 0 || height < 0)
		return;
	if (x > w->core.width || y > w->core.height)
		return;
	if (x + width > w->core.width)
		width = w->core.width - x;
	if (y + height > w->core.height)
		height = w->core.height - y;

	rect.x      = x;
	rect.y      = y;
	rect.width  = width;
	rect.height = height;

	if (pw->paint.zoom <= 1)
		PwUpdateDrawable(w, XtWindow(w), &rect);
	else
		zoomUpdate(pw, &rect);
}

static void resizePixmap(PaintWidget w, Pixmap *pixmap)
{
	Pixmap	n;

	if (pixmap == NULL || *pixmap == None)
		return;

	n = XCreatePixmap(XtDisplay(w), XtWindow(w), 
				w->paint.drawWidth, w->paint.drawHeight,
				w->core.depth);
	XFillRectangle(XtDisplay(w), n, 
				w->paint.igc, 0,0, 
				w->paint.drawWidth, w->paint.drawHeight);
	XCopyArea(XtDisplay(w), *pixmap, n,
				w->paint.gc, 0, 0, 
				w->paint.drawWidth, w->paint.drawHeight, 0, 0);
	XFreePixmap(XtDisplay(w), *pixmap);
	*pixmap = n;
}

static Boolean SetValuesProc(Widget curArg, Widget request, 
				Widget newArg)
{
	PaintWidget	cur = (PaintWidget)curArg;
	PaintWidget	new = (PaintWidget)newArg;
	int		ret = False;

	if (cur->paint.sourcePixmap != new->paint.sourcePixmap) 
		pwSetPixmap(new, new->paint.sourcePixmap, True);

	if (cur->paint.compress != new->paint.compress) 
		new->core.widget_class->core_class.compress_motion = new->paint.compress;

	if (!XtIsRealized((Widget)new))
		return ret;

	if (cur->paint.fillRule != new->paint.fillRule) {
		XSetFillStyle(XtDisplay(new), new->paint.fgc, new->paint.fillRule);
	}
	if (cur->paint.foreground != new->paint.foreground) {
		XSetForeground(XtDisplay(new), new->paint.fgc, new->paint.foreground);
	}
	if (cur->paint.pattern != new->paint.pattern) {
		XSetTile(XtDisplay(new), new->paint.fgc, new->paint.pattern);
	}
	if (cur->paint.lineWidth != new->paint.lineWidth) {
		XSetLineAttributes(XtDisplay(new), new->paint.fgc, 
				new->paint.lineWidth, LineSolid, 
				CapButt, JoinMiter);
	}
	if (cur->paint.fontInfo->fid != new->paint.fontInfo->fid) {
		XSetFont(XtDisplay(new), new->paint.fgc, 
				new->paint.fontInfo->fid);
	}
	if (cur->paint.drawWidth  != new->paint.drawWidth || 
	    cur->paint.drawHeight != new->paint.drawHeight) {
		UndoStack	*cur;

		resizePixmap(new, &new->paint.base);
		for (cur = new->paint.top; cur != NULL; cur = cur->next) 
			resizePixmap(new, &cur->pixmap);

		XtMakeResizeRequest((Widget)new, 
				new->paint.drawWidth * new->paint.zoom,
				new->paint.drawHeight * new->paint.zoom,
				NULL, NULL);

		ret = True;
	}
	if (cur->paint.zoom != new->paint.zoom) {
		ret = True;

		XtMakeResizeRequest((Widget)new, 
				new->paint.drawWidth * new->paint.zoom,
				new->paint.drawHeight * new->paint.zoom,
				NULL, NULL);
	}

	return ret;
}

static int QueryGeometryProc(Widget w, XtWidgetGeometry *proposed,
				XtWidgetGeometry *reply)
{

/*
**  Use the fact that the return values are ordered by "importance"
*/
#define SETRET(ret,x)	ret = (x > ret) ? x : ret
	PaintWidget	pw = (PaintWidget)w;
	int		width  = pw->paint.drawWidth  * pw->paint.zoom;
	int		height = pw->paint.drawHeight * pw->paint.zoom;

	reply->request_mode = CWWidth | CWHeight;
	reply->width  = width;
	reply->height = height;

#if 0
	printf("In %d,%d   Out %d,%d\n",
			proposed->width, proposed->height,
			reply->width, reply->height);
#endif

	if (proposed->request_mode & (CWWidth | CWHeight) == (CWWidth | CWHeight) &&
	    proposed->width == reply->width && proposed->height == reply->height)
		return XtGeometryNo;
#if 0
	else if (reply->width == w->core.width && reply->height == w->core.height)
		return XtGeometryNo;
#endif
	else
		return XtGeometryAlmost;
}

void PwGetPixmap(Widget w, Pixmap *pix, int *width, int *height)
{
	PaintWidget	pw = (PaintWidget)w;
	*pix = None;

	if (pw->paint.paint != None) {
		PwGetPixmap((Widget)pw->paint.paint, pix, width, height);
		return;
	}

	if (width)
		*width  = pw->paint.drawWidth;
	if (height)
		*height = pw->paint.drawHeight;

	*pix= XCreatePixmap(XtDisplay(w), XtWindow(w), 
				pw->paint.drawWidth, pw->paint.drawHeight,
				pw->core.depth);
	PwUpdateDrawable(w, *pix, NULL);
}

static void pwSetPixmap(PaintWidget w, Pixmap pix, int flag)
{
	Window		root;
	int		x, y;
	unsigned int	width, height, bw, depth;
	
	XGetGeometry(XtDisplay(w), pix,
			&root, &x, &y, &width, &height, &bw, &depth);

	if (depth == 1) {
		XCopyPlane(XtDisplay(w), pix, GET_PIXMAP(w),
			w->paint.gc, 0, 0, width, height, 0, 0, 1);
	} else {
		XCopyArea(XtDisplay(w), pix, GET_PIXMAP(w),
			w->paint.gc, 0, 0, width, height, 0, 0);
	}
}

Boolean PwGetRegion(Widget w, Pixmap *pix, Pixmap *mask)
{
	PaintWidget	pw = (PaintWidget)w;

	if (pix)  *pix  = None;
	if (mask) *mask = None;

	if (pw->paint.regionPix == None)
		return False;

	if (pw->paint.regionPix != None && pix != NULL) {
		*pix = XCreatePixmap(XtDisplay(w), XtWindow(w), 
				pw->paint.regionRect.width,
				pw->paint.regionRect.height,
				pw->core.depth);
		XCopyArea(XtDisplay(w), pw->paint.regionPix, *pix, pw->paint.tgc,
				0, 0,
				pw->paint.regionRect.width,
				pw->paint.regionRect.height,
				0, 0);
	}
	if (pw->paint.regionMask != None && mask != NULL) {
		/*
		**  Just need a 1 bit gc
		*/
		if (pw->paint.mgc == None)
			pw->paint.mgc = XCreateGC(XtDisplay(w), pw->paint.regionMask, 0, 0);

		*mask = XCreatePixmap(XtDisplay(w), XtWindow(w), 
				pw->paint.regionRect.width,
				pw->paint.regionRect.height,
				1);
		XCopyArea(XtDisplay(w), pw->paint.regionMask, *mask, pw->paint.mgc,
				0, 0,
				pw->paint.regionRect.width,
				pw->paint.regionRect.height,
				0, 0);
	}
	return True;
}

void PwPutRegion(Widget w, Pixmap pix, Pixmap mask)
{
	PaintWidget	pw = (PaintWidget)w;
        Window          root;
        int             x, y;
        unsigned int    width, height, bw, depth;

	if (pix == None)
		return;

	if (pw->paint.regionPix != None)
		XFreePixmap(XtDisplay(w), pw->paint.regionPix);
	if (pw->paint.regionMask != None)
		XFreePixmap(XtDisplay(w), pw->paint.regionMask);

	XGetGeometry(XtDisplay(w), pix,
                        &root, &x, &y, &width, &height, &bw, &depth);

	pw->paint.regionRect.x = 0;
	pw->paint.regionRect.y = 0;
	pw->paint.regionRect.width  = width;
	pw->paint.regionRect.height = height;

	pw->paint.regionPix = XCreatePixmap(XtDisplay(w), XtWindow(w), 
						width, height, depth);
	XCopyArea(XtDisplay(w), pix, pw->paint.regionPix, pw->paint.tgc,
				0, 0, width, height, 0, 0);

	if (mask != None) {
		if (pw->paint.mgc == None)
			pw->paint.mgc = XCreateGC(XtDisplay(w), mask, 0, 0);
		pw->paint.regionMask = XCreatePixmap(XtDisplay(w), XtWindow(w), 
							width, height, 1);
		XCopyArea(XtDisplay(w), mask, pw->paint.regionMask, pw->paint.mgc,
					0, 0, width, height, 0, 0);
	} else {
		pw->paint.regionMask = None;
	}
}

Boolean PwClearRegion(Widget w, Boolean flag)
{
	PaintWidget	pw = (PaintWidget)w;
	GC		gc = pw->paint.igc;

	if (pw->paint.regionMask != None) {
		gc = XCreateGC(XtDisplay(w), XtWindow(w), 0, 0);
		XCopyGC(XtDisplay(w), pw->paint.igc, GCForeground|GCBackground, gc);

		XSetClipOrigin(XtDisplay(w), gc, pw->paint.regionRect.x, pw->paint.regionRect.y);
		XSetClipMask(XtDisplay(w), gc, pw->paint.regionMask);
	}

	XFillRectangles(XtDisplay(pw), GET_PIXMAP(pw), gc, &pw->paint.regionRect, 1);

	if (pw->paint.regionMask != None)
		XFreeGC(XtDisplay(w), gc);

	if (flag) {
		void UndoSetRectangle(Widget, XRectangle *);
		UndoSetRectangle(w, &pw->paint.regionRect);
	}

	return False;
}

void PwLoadRegion(Widget w, XRectangle *rect, Pixmap mask)
{
	PaintWidget	pw = (PaintWidget)w;

	if (pw->paint.regionPix != None)
		XFreePixmap(XtDisplay(pw), pw->paint.regionPix);
	if (pw->paint.regionMask != None)
		XFreePixmap(XtDisplay(pw), pw->paint.regionMask);

	pw->paint.regionMask = mask;
	pw->paint.regionRect = *rect;

	pw->paint.regionPix = XCreatePixmap(XtDisplay(pw), XtWindow(pw), 
						rect->width, rect->height,
						pw->core.depth);

	XCopyArea(XtDisplay(pw), GET_PIXMAP(pw), pw->paint.regionPix, 
			pw->paint.tgc,
			rect->x, rect->y,
			rect->width, rect->height,
			0, 0);
}

void PwWriteRegion(Widget w)
{
	PaintWidget	pw = (PaintWidget)w;

	if (pw->paint.regionPix == None)
		return;

	/*
	** Copy it to the pixmap
	*/
	if (pw->paint.regionMask != None) {
		XSetClipOrigin(XtDisplay(w), pw->paint.tgc,
				pw->paint.regionRect.x, pw->paint.regionRect.y);

		XSetClipMask(XtDisplay(w), pw->paint.tgc, pw->paint.regionMask);
	}

	XCopyArea(XtDisplay(w), pw->paint.regionPix, GET_PIXMAP(pw), pw->paint.tgc,
			0, 0,
			pw->paint.regionRect.width,
			pw->paint.regionRect.height,
			pw->paint.regionRect.x,
			pw->paint.regionRect.y);

	if (pw->paint.regionMask != None) 
		XSetClipMask(XtDisplay(w), pw->paint.tgc, None);

	/*
	**  If it wasn't drawn on the screen, draw it.
	*/
	if (!pw->paint.regionDrawn)
		PwUpdate((Widget)w, &pw->paint.regionRect, True);
}

void PwMoveRegion(Widget w, int x, int y)
{
	PaintWidget	pw = (PaintWidget)w;
	int		xmin, xmax, ymin, ymax;
	XRectangle	rect;

	xmin = MIN(x, pw->paint.regionRect.x);
	ymin = MIN(y, pw->paint.regionRect.y);
	xmax = MAX(x, pw->paint.regionRect.x) + pw->paint.regionRect.width;
	ymax = MAX(y, pw->paint.regionRect.y) + pw->paint.regionRect.height;

	rect.x = xmin;
	rect.y = ymin;
	rect.width = xmax - xmin;
	rect.height = ymax - ymin;

	pw->paint.regionRect.x = x;
	pw->paint.regionRect.y = y;

	PwUpdate((Widget)w, &rect, True);
}

void PwSetDrawn(Widget w, Boolean flag)
{
	PaintWidget	pw = (PaintWidget)w;

	Boolean	changed = (pw->paint.regionDrawn != flag);

	pw->paint.regionDrawn = flag;
	
	if (changed)
		PwUpdate(w, &pw->paint.regionRect, True);
}

static void fatCallback(Widget parent, XtPointer w, XtPointer rectArg)
{
	XRectangle	*rect = (XRectangle*)rectArg;
	PaintWidget	pw = (PaintWidget)w;
	XExposeEvent	event;

	/*
	**  Make this look like an expose event on the fatbits region
	*/
	event.x      = (rect->x - pw->paint.zoomX) * pw->paint.zoom;
	event.y      = (rect->y - pw->paint.zoomY) * pw->paint.zoom;
	event.width  = rect->width * pw->paint.zoom;
	event.height = rect->height * pw->paint.zoom;

	ExposeProc(w, &event);
}

void PwUpdate(Widget w, XRectangle *rect, Boolean force)
{
	PaintWidget	pw = (PaintWidget)w;
	PaintWidget	parent = (PaintWidget)pw->paint.paint;
	XRectangle	all;

	if (rect == NULL) {
		if (pw->paint.undo == NULL) {
			all.x      = 0;
			all.y      = 0;
			all.width  = pw->core.width;
			all.height = pw->core.height;
			rect = &all;
		} else {
			rect = &pw->paint.undo->box;
		}
	} else {
		all.x = rect->x - pw->paint.lineWidth;
		all.y = rect->y - pw->paint.lineWidth;
		all.width = rect->width + pw->paint.lineWidth * 2;
		all.height = rect->height + pw->paint.lineWidth * 2;
		rect = &all;
	}

	if (parent != None) {
		/*
		**  Return since we will get updated by our parent
		*/
		XtCallCallbackList((Widget)parent, parent->paint.fatcalls, rect);
		pw = parent;
	} else {
		XtCallCallbackList((Widget)pw, pw->paint.fatcalls, rect);
	}

	if (force || pw->paint.zoom > 1 || parent != None) 
		fatCallback((Widget)pw, (Widget)pw, rect);
}

/*
**  Update the current visual buffer with the contents
**   of the undo area, thus a pseudo-real time undo.
**   (it's not undo at all)
*/
void PwUpdateFromLast(Widget w, XRectangle *rect) 
{
	Pixmap		pix;
	PaintWidget	pw = (PaintWidget)w;
	Pixmap		draw = GET_PIXMAP(pw);
	
	if (pw->paint.paint != None)
		pw = (PaintWidget)pw->paint.paint;

	/*
	**  If there is undoing enabled
	*/
	if (pw->paint.undo == None) {
		pix = pw->paint.base;
	} else {
		if (pix = pw->paint.undo->next != NULL)
			pix = pw->paint.undo->next->pixmap;
		else
			pix = pw->paint.base;
	}

	XCopyArea(XtDisplay(w), pix, draw, pw->paint.gc,
			rect->x, rect->y, rect->width, rect->height,
			rect->x, rect->y);
	PwUpdate(w, rect, True);
}
