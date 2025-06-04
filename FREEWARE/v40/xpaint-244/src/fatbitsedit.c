/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, 1993, David Koblas (koblas@netcom.com)	       | */
/* | Copyright 1995, 1996 Torsten Martinsen (bullestock@dk-online.dk)  | */
/* |								       | */
/* | Permission to use, copy, modify, and to distribute this software  | */
/* | and its documentation for any purpose is hereby granted without   | */
/* | fee, provided that the above copyright notice appear in all       | */
/* | copies and that both that copyright notice and this permission    | */
/* | notice appear in supporting documentation.	 There is no	       | */
/* | representations about the suitability of this software for	       | */
/* | any purpose.  this software is provided "as is" without express   | */
/* | or implied warranty.					       | */
/* |								       | */
/* +-------------------------------------------------------------------+ */

/* $Id: fatBitsEdit.c,v 1.14 1996/05/10 09:42:28 torsten Exp $ */

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>	/* for XtResizeWidget() */
#include <X11/StringDefs.h>
#ifndef VMS
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Scrollbar.h>
#else
#include <X11Xaw/Command.h>
#include <X11Xaw/Toggle.h>
#include <X11Xaw/Viewport.h>
#include <X11Xaw/Box.h>
#include <X11Xaw/Form.h>
#include <X11Xaw/Scrollbar.h>
#endif
#include <X11/Shell.h>
#include <X11/cursorfont.h>
#include <stdio.h>
#ifndef NOSTDHDRS
#include <stdlib.h>
#include <unistd.h>
#endif
#include "Paint.h"
#include "PaintP.h"
#include "xpaint.h"
#include "palette.h"
#include "menu.h"
#include "misc.h"
#include "cutCopyPaste.h"
#include "protocol.h"
#include "graphic.h"
#include "ops.h"

#define PADDING	4
#define BW	1

typedef struct LocalInfo_s {
    int curX, curY;
    int offX, offY, baseX, baseY;
    Widget cursor;		/* The Box widget */
    Widget subpaint;		/* the paint widget inside the box */
    Widget view;		/* the widget in the popup window */
    Widget paint;		/* The source of this zoom */
    Widget shell;

    int zoom;			/* paint widget (parent) zoom value */
    Position spX, spY;		/* subpaint x, y (relative to Box widget) */
    Dimension spBW;		/* subpaint borderwidth */
    struct LocalInfo_s *next;
} LocalInfo;

static LocalInfo *head = NULL;

typedef struct {
    int size;			/* Max # of line segments */
    int nsegs;			/* Current # of line segments */
    XSegment *segs;		/* The segments */
} FatLineCursor;

/*
 * Segments are allocated in chunks this size. The smallest brush
 * in the current brush palette uses 9 segments.
 */
#define FATCHUNK 16

/*
 * Info passed to FatCursorDraw().
 */
typedef struct fcinfo {
    Widget w;
    Boolean drawn;
    int lastX, lastY;
    GC gcx;
    int zoom;
    struct fcinfo *next;
} FatCursorInfo;

typedef struct {
    FatCursorInfo *info;
    Pixmap pixmap;		/* Pixmap used for building master */
    FatLineCursor cursor;
} FatMasterInfo;

static FatMasterInfo master =
{NULL};

/*
 * Add a line segment to fc.
 * Reallocs if necessary.
 */
static void 
AddSegment(FatLineCursor * fc, int x1, int y1, int x2, int y2)
{
    XSegment *p;

    if (fc->nsegs >= fc->size) {
	fc->size += FATCHUNK;
	fc->segs = realloc(fc->segs, fc->size * sizeof(XSegment));
    }
    p = &fc->segs[fc->nsegs++];
    p->x1 = x1;
    p->y1 = y1;
    p->x2 = x2;
    p->y2 = y2;
}

/*
 * Given a pixmap, create a master vector cursor.
 */
static void 
CreateFatCursor(Widget paint, Pixmap cursor, FatLineCursor * fc)
{
    XImage *src;
    int w, h, x, y, xoff, yoff;
    Display *dpy = XtDisplay(paint);

    fc->nsegs = 0;
    fc->size = FATCHUNK;
    fc->segs = malloc(FATCHUNK * sizeof(XSegment));

    GetPixmapWHD(dpy, cursor, &w, &h, NULL);
    src = XGetImage(dpy, cursor, 0, 0, w, h, AllPlanes, ZPixmap);
    xoff = w / 2;
    yoff = h / 2;
    for (y = 0; y < h; ++y)
	for (x = 0; x < w; ++x)
	    /*
	     * If we find a black pixel, check the four neighbours and
	     * place a line segment between the pixel and each white neighbour.
	     */
	    if (XGetPixel(src, x, y)) {
		/* If at the edge, always add a line */
		if ((x == 0) || (XGetPixel(src, x - 1, y) == 0))
		    AddSegment(fc, x - xoff, y - yoff, x - xoff, y - yoff + 1);
		if ((y == 0) || (XGetPixel(src, x, y - 1) == 0))
		    AddSegment(fc, x - xoff, y - yoff, x - xoff + 1, y - yoff);
		if ((x == w - 1) || (XGetPixel(src, x + 1, y) == 0))
		    AddSegment(fc, x - xoff + 1, y - yoff, x - xoff + 1, y - yoff + 1);
		if ((y == h - 1) || (XGetPixel(src, x, y + 1) == 0))
		    AddSegment(fc, x - xoff, y - yoff + 1, x - xoff + 1, y - yoff + 1);
	    }
}


static void 
FatCursorDraw(Widget w, FatCursorInfo * l, XMotionEvent * event)
{
    int x, y, i, n, z;
    XSegment *s;
    Display *dpy = XtDisplay(w);
    Window win = XtWindow(w);
    static int drawing = 0;

    /*
     * If someone is still drawing on the canvas, return
     */
    if (drawing) {
	if (event->type == ButtonRelease) {
	    drawing = 0;
	    l->drawn = 0;	/* Don't draw at old position */
	} else
	    return;
    }
    n = master.cursor.nsegs;
    z = l->zoom;

    /*
     * Erase the old cursor (if any)
     */
    if (l->drawn) {
	x = l->lastX;
	y = l->lastY;
	s = master.cursor.segs;
	for (i = 0; i < n; ++i, ++s)
	    XDrawLine(dpy, win, l->gcx,
		      x + z * s->x1, y + z * s->y1,
		      x + z * s->x2, y + z * s->y2);
    }
    /*
     * If a button is pressed, leave the cursor undrawn
     */
    if (event->type == ButtonPress) {
	drawing = 1;
	return;
    }
    /*
     * If we're leaving the window, only draw the cursor once
     * when we enter again
     */
    if (event->type == LeaveNotify) {
	l->drawn = 0;
	return;
    }
    l->drawn = 1;
    x = event->x;
    y = event->y;
    /* Snap to zoom grid */
    x = x / z * z;
    y = y / z * z;
#ifndef TRUNC_SNAP
    if (event->x - x > z / 2)
	x += z;
    if (event->y - y > z / 2)
	y += z;
#endif
    s = master.cursor.segs;
    for (i = 0; i < n; ++i, ++s)
	XDrawLine(dpy, win, l->gcx,
		  x + z * s->x1, y + z * s->y1,
		  x + z * s->x2, y + z * s->y2);

    l->lastX = x;
    l->lastY = y;
}

/*
 * Register a fat cursor for the specified widget.
 * If a fat cursor is already active, just update the zoom.
 */
void 
FatCursorAddZoom(int zoom, Widget winwid)
{
    FatCursorInfo *new, *p;

    for (p = master.info; p != NULL; p = p->next)
	if (p->w == winwid) {
	    p->zoom = zoom;
	    XtInsertEventHandler(winwid,
				 PointerMotionMask | ButtonPressMask |
				 ButtonReleaseMask | LeaveWindowMask,
				 False, (XtEventHandler) FatCursorDraw,
				 (XtPointer) p, XtListHead);
	    return;
	}
    new = XtNew(FatCursorInfo);
    new->next = master.info;
    master.info = new;
    new->zoom = zoom;
    new->w = winwid;
    new->gcx = GetGCX(winwid);
    new->drawn = False;

    /*
     * FatCursorDraw must be called before the drawing routine so that
     * it can remove the cursor in time.
     */
    XtInsertEventHandler(winwid,
			 PointerMotionMask | ButtonPressMask | ButtonReleaseMask | LeaveWindowMask,
		  False, (XtEventHandler) FatCursorDraw, (XtPointer) new,
			 XtListHead);
}

/*
 * Unregister a fat cursor for the specified widget's window.
 */
void 
FatCursorRemoveZoom(Widget winwid)
{
    FatCursorInfo *p, *pp;

    for (p = pp = master.info; p != NULL; pp = p, p = p->next)
	if (p->w == winwid)
	    break;

    if (p == NULL)		/* not found */
	return;

    XtRemoveEventHandler(winwid, PointerMotionMask | ButtonPressMask |
			 ButtonReleaseMask | LeaveWindowMask,
			 False, (XtEventHandler) FatCursorDraw, (XtPointer) p);
    if (pp == p)
	master.info = p->next;
    else
	pp->next = p->next;
    XtFree((XtPointer) p);
}

void 
FatCursorDestroyCallback(Widget w, XtPointer arg, XtPointer junk)
{
    FatCursorRemoveZoom((Widget) arg);
}

/*
 * Rebuild the master vector cursor corresponding to the specified pixmap.
 */
void 
FatCursorSet(Widget w, Pixmap cursor)
{
    if (master.pixmap != cursor) {	/* Avoid setting cursor more than once */
	master.pixmap = cursor;
	if (master.cursor.segs != NULL)
	    free(master.cursor.segs);
	CreateFatCursor(w, cursor, &master.cursor);
    }
}

/*
 * If the specified widget has a FatCursor, remove its event handler.
 */
void 
FatCursorOff(Widget w)
{
    FatCursorInfo *p;

    for (p = master.info; p != NULL; p = p->next)
	if (p->w == w) {
	    XtRemoveEventHandler(p->w,
				 PointerMotionMask | ButtonPressMask |
				 ButtonReleaseMask | LeaveWindowMask,
				 False, (XtEventHandler) FatCursorDraw,
				 (XtPointer) p);
	    XtRemoveCallback(p->w, XtNdestroyCallback,
			     FatCursorDestroyCallback, p->w);
	    return;
	}
}

/*
**  When the popup window is gone, free the storage
 */
static void 
destroyCallback(Widget w, XtPointer larg, void *junk2)
{
    LocalInfo *l = (LocalInfo *) larg;
    LocalInfo *c = head, **p = &head;

    while (c != NULL && c != l) {
	p = &c->next;
	c = c->next;
    }

    if (c != NULL)
	*p = l->next;

    XtFree((XtPointer) l);
}

/*
**  Done or WM-Close pressed, destroy widgets
 */
static void 
doneCallback(Widget w, XtPointer larg, void *junk2)
{
    LocalInfo *l = (LocalInfo *) larg;

    FatCursorRemoveZoom(l->view);

    /*
    **	Destroy both the cursor and the popup window
     */
    XtDestroyWidget(l->cursor);
    XtDestroyWidget(GetShell(w));
}

/*
**  Move the cursor and the view window to the position
**   specified by x,y
**  w, h are dimensions of the zoomed area
 */
static void 
moveCursor(LocalInfo * l, int x, int y, int w, int h)
{
    int dw, dh;
    int rx, ry;

    if (w == -1 || h == -1) {
	int z;
	Dimension wt, ht;

	XtVaGetValues(l->view, XtNzoom, &z,
		      XtNwidth, &wt,
		      XtNheight, &ht,
		      NULL);
	w = (wt + z - 1) / z;
	h = (ht + z - 1) / z;
    }
    XtVaGetValues(l->paint, XtNdrawWidth, &dw, XtNdrawHeight, &dh, NULL);

    if (x < 0)
	x = 0;
    if (y < 0)
	y = 0;
    if (x + w > dw)
	x = dw - w;
    if (y + h > dh)
	y = dh - h;
    if (x < 0 || y < 0)
	return;

    rx = x / l->zoom;		/* correct for parent's zoom factor */
    ry = y / l->zoom;
    x = rx * l->zoom;		/* snap view origin to zoom grid */
    y = ry * l->zoom;

    XtVaSetValues(l->view, XtNzoomX, rx, XtNzoomY, ry, NULL);
    XtVaSetValues(l->cursor, XtNx, x - l->spX - l->spBW - 1,
		  XtNy, y - l->spY - l->spBW - 1, NULL);
    XtVaSetValues(l->subpaint, XtNzoomX, rx, XtNzoomY, ry, NULL);
}

/*
**  Given the popup dimensions, set the "cursor" view to the correct size
 */
static void 
resizeCursor(LocalInfo * l)
{
    Dimension width, height;
    int zoom;
    int pw, ph;
    int dw, dh;
    int zx, zy;

    XtVaGetValues(l->view, XtNwidth, &width,
		  XtNheight, &height,
		  XtNzoom, &zoom,
		  NULL);
    XtVaGetValues(l->paint, XtNdrawWidth, &dw,
		  XtNdrawHeight, &dh,
		  XtNzoom, &l->zoom,
		  NULL);
    XtVaGetValues(l->subpaint, XtNzoomX, &zx,
		  XtNzoomY, &zy,
		  NULL);

    pw = (width + zoom - 1) / zoom;
    ph = (height + zoom - 1) / zoom;
    if ((pw + zx > dw) || (ph + zy > dh)) {
	int nx, ny;

	nx = (pw + zx > dw) ? dw - pw : zx;
	ny = (ph + zy > dh) ? dh - ph : zy;

	/*
	**  If the new x or y value off the screen, set it back.
	**    and resize view
	 */
	if (nx < 0 || ny < 0) {
	    if (nx < 0) {
		pw += nx;
		nx = 0;
	    }
	    if (ny < 0) {
		ph += ny;
		ny = 0;
	    }
	    /* XXX -- this really should be SetValues(width,height)
	    **	      but that doesn't work..?/
	     */
	    XtResizeWidget(l->view, pw * zoom, ph * zoom, 1);
	}
	moveCursor(l, nx, ny, pw, ph);
    }
    pw *= l->zoom;
    ph *= l->zoom;

    XtVaSetValues(l->cursor, XtNwidth, pw + 2 * (PADDING + BW),
		  XtNheight, ph + 2 * (PADDING + BW),
		  NULL);
    XtVaSetValues(l->subpaint, XtNwidth, pw, XtNheight, ph, NULL);
}

/*
**  Button down in the cursor window
 */
static void 
press(Widget w, LocalInfo * l, XButtonEvent * event, Boolean * flg)
{
    Position x, y;

    XtVaGetValues(w, XtNx, &x, XtNy, &y, NULL);

    l->offX = event->x;
    l->offY = event->y;
    l->baseX = event->x_root - x;
    l->baseY = event->y_root - y;
    XtVaGetValues(l->paint, XtNzoom, &l->zoom, NULL);
}

static void 
motion(Widget w, LocalInfo * l, XMotionEvent * event, Boolean * flg)
{
    int nx, ny;
    int px, py;

    /*
    **	Compress motion events.
     */
    while (XCheckTypedWindowEvent(XtDisplay(w), XtWindow(w),
				  MotionNotify, (XEvent *) event));

    nx = event->x_root - l->baseX;
    ny = event->y_root - l->baseY;

    px = nx + l->spX + l->spBW + 1;
    py = ny + l->spY + l->spBW + 1;

    moveCursor(l, px, py, -1, -1);
}

/*
**  If the paint view size changes, update the parent paint window cursor size
 */
static void 
sizeChanged(Widget w, XtPointer l, XtPointer junk)
{
    resizeCursor((LocalInfo *) l);
}

/*
**  The parent box widget changed size, resize to fit.
 */
static void 
boxChanged(Widget w, LocalInfo * l, XConfigureEvent * event, Boolean * flg)
{
    Dimension width, height;
    Dimension hpad, vpad, bw;
    int zoom;

    XtVaGetValues(l->view, XtNzoom, &zoom, XtNborderWidth, &bw,
		  NULL);
    XtVaGetValues(XtParent(l->view), XtNwidth, &width,
		  XtNheight, &height,
		  XtNhSpace, &hpad,
		  XtNvSpace, &vpad,
		  NULL);
    width -= (hpad + bw) * 2;
    height -= (vpad + bw) * 2;

    /* XXX -- this really should be SetValues(width,height)
    **	      but that doesn't work..?/
     */
    XtResizeWidget(l->view, width, height, 1);

    resizeCursor(l);
}

/*
**  One of the zoom percentage buttons pressed
 */
static void 
buttonCallback(Widget w, XtPointer lArg, void *junk2)
{
    LocalInfo *l = (LocalInfo *) lArg;
    char *lbl;
    int nz;
    int cx, cy, zx, zy, z, nw, nh;
    int x, y;
    Dimension width, height;
    Boolean state;

    XtVaGetValues(w, XtNstate, &state, XtNlabel, &lbl, NULL);

    if (state == False)
	return;

    nz = 0;
    sscanf(lbl, "%*d:%d", &nz);
    if (nz == 0)
	return;

    XtVaGetValues(l->view, XtNzoomX, &zx,
		  XtNzoomY, &zy,
		  XtNzoom, &z,
		  XtNwidth, &width,
		  XtNheight, &height,
		  NULL);

    if ((CurrentOp->add == BrushAdd) || (CurrentOp->add == EraseAdd) ||
	(CurrentOp->add == SmearAdd))
	FatCursorAddZoom(nz, l->view);

    cx = zx + ((width + z - 1) / z) / 2;
    cy = zy + ((height + z - 1) / z) / 2;
    nw = (width + nz - 1) / nz;
    nh = (height + nz - 1) / nz;

    XtVaSetValues(l->view, XtNzoom, nz, NULL);
    /*	center on image center */
    x = cx - nw / 2;
    y = cy - nh / 2;

    moveCursor(l, x, y, width / nz, height / nz);
}

/*
 * Called when the zoom factor or size of the canvas is changed,
 * so that the FatBits cursor window can be resized.
 * If zoom is -1, don't change zoom factor, just redisplay
 * cursor window.
 */
void 
FatbitsUpdate(Widget w, int zoom)
{
    LocalInfo *l;
    int x, y, vw, vh, dw, dh;


    /* find fatbits info structure for this popup */
    for (l = head; l != NULL; l = l->next)
	if (l->paint == w) {
	    if (zoom > 0)
		l->zoom = zoom;	/* new parent zoom factor */
	    else
		zoom = l->zoom;

	    resizeCursor(l);	/* correct cursor window size */

	    /* reposition cursor window in parent paint window */
	    XtVaGetValues(l->view, XtNzoomX, &x, XtNzoomY, &y, NULL);
	    XtVaSetValues(l->cursor, XtNx, x * zoom - l->spX - l->spBW - 1,
			  XtNy, y * zoom - l->spY - l->spBW - 1, NULL);

	    XtVaGetValues(w, XtNdrawWidth, &dw, XtNdrawHeight, &dh, NULL);
	    /* The Viewport is grandparent of the paint widget */
	    vw = vh = 0;
	    XtVaGetValues(XtParent(XtParent(w)),
			  XtNwidth, &vw, XtNheight, &vh, NULL);
	    if ((dw * zoom > vw) || (dh * zoom > vh))
		/*
		 * The zoomed canvas is larger than the viewport.
		 * Reposition view on canvas so cursor window remains visible.
		 * This solution is not perfect: the cursor window is simply
		 * positioned at (0,0) in the viewport window.
		 * Still, it's better than having a non-visible cursor window.
		 */
		XawViewportSetCoordinates(XtParent(XtParent(w)), x * zoom, y * zoom);

	    return;		/* there can only be one fatbits popup */
	}
}


/*
**  Construct the fatbits popup.
 */
void 
FatbitsEdit(Widget paint)
{
    Widget shell, form, fat;
    Widget button, box;
    int i, zoom;
    Colormap cmap;
    Position x, y, lx, ly;
    Dimension width = 48, height = 48;
    LocalInfo *l;
    static char *zoomList[] =
    {
	"zoomButton1",
	"zoomButton2",
	"zoomButton3",
	"zoomButton4",
	"zoomButton5",
    };
    XtTranslations trans =
    XtParseTranslationTable("<BtnDown>,<BtnUp>: set() notify()");
    Widget first = None;

    /* If a popup for this paint widget exists, raise it */
    for (l = head; l != NULL && l->paint != paint; l = l->next);

    if (l != NULL) {
	XMapRaised(XtDisplay(l->shell), XtWindow(l->shell));
	return;
    }
    /* Construct a new fatbits popup */

    l = XtNew(LocalInfo);

    l->paint = paint;
    l->next = head;
    head = l;

    XtVaGetValues(paint, XtNcolormap, &cmap, XtNzoom, &l->zoom,
		  XtNdownX, &lx, XtNdownY, &ly, NULL);

    shell = XtVaCreatePopupShell("fatbits", topLevelShellWidgetClass,
				 GetShell(paint), XtNcolormap, cmap,
				 NULL);
    l->shell = shell;

    PaletteAddUser(PaletteFind(shell, cmap), shell);
    form = XtVaCreateManagedWidget("form", formWidgetClass, shell, NULL);

    box = XtVaCreateManagedWidget("fatBox", boxWidgetClass, form,
			  XtNbackgroundPixmap, GetBackgroundPixmap(form),
				  NULL);
    XtAddEventHandler(box, StructureNotifyMask, False,
		      (XtEventHandler) boxChanged, (XtPointer) l);


    fat = XtVaCreateManagedWidget("paint", paintWidgetClass, box,
				  XtNpaint, paint,
				  XtNbottom, XtChainBottom,
				  NULL);
    l->view = fat;
    XtVaGetValues(fat, XtNzoom, &zoom, NULL);

    XtVaSetValues(fat, XtNwidth, width * zoom, XtNheight, height * zoom, NULL);

    button = XtVaCreateManagedWidget("done", commandWidgetClass, form,
				     XtNfromVert, box,
				     XtNtop, XtChainBottom,
				     XtNbottom, XtChainBottom,
				     XtNleft, XtChainLeft,
				     XtNright, XtChainLeft,
				     NULL);

    XtAddCallback(button, XtNcallback, doneCallback, (XtPointer) l);

    ccpAddStdPopup(fat);

    first = None;
    for (i = 0; i < XtNumber(zoomList); i++) {
	button = XtVaCreateManagedWidget(zoomList[i],
					 toggleWidgetClass, form,
					 XtNfromHoriz, button,
					 XtNfromVert, box,
					 XtNtop, XtChainBottom,
					 XtNbottom, XtChainBottom,
					 XtNleft, XtChainRight,
					 XtNright, XtChainRight,
					 XtNradioGroup, first,
					 XtNtranslations, trans,
					 NULL);
	first = button;
	XtAddCallback(button, XtNcallback, buttonCallback, (XtPointer) l);

	if (i == XtNumber(zoomList) / 2) {
	    XtVaSetValues(button, XtNstate, True, NULL);
	}
    }

    XtAddCallback(shell, XtNdestroyCallback, destroyCallback, (XtPointer) l);
    AddDestroyCallback(shell, (DestroyCallbackFunc) doneCallback, (XtPointer) l);

    XtAddCallback(fat, XtNdestroyCallback, FatCursorDestroyCallback, fat);

    XtPopup(shell, XtGrabNone);

    lx -= width / 2;
    ly -= height / 2;
    if (lx < 0)
	lx = 0;
    if (ly < 0)
	ly = 0;

    x = lx - (PADDING + 2);
    y = ly - (PADDING + 2);

    box = XtVaCreateManagedWidget("fatBox", boxWidgetClass, paint,
				  XtNx, x,
				  XtNy, y,
				  XtNwidth, width + 2 * (PADDING + 1),
				  XtNheight, height + 2 * (PADDING + 1),
				  XtNborderWidth, 1,
			 XtNbackgroundPixmap, GetBackgroundPixmap(paint),
				  NULL);

    l->cursor = box;

    XtAddEventHandler(box, ButtonPressMask, False,
		      (XtEventHandler) press, (XtPointer) l);
    XtAddEventHandler(box, ButtonMotionMask, False,
		      (XtEventHandler) motion, (XtPointer) l);

    l->subpaint = XtVaCreateManagedWidget("fatPaint", paintWidgetClass, box,
					  XtNpaint, paint,
					  XtNzoom, PwZoomParent,
					  XtNx, PADDING,
					  XtNy, PADDING,
					  XtNborderWidth, 1,
					  XtNwidth, width,
					  XtNheight, height,
					  XtVaTypedArg, XtNcursor,
				      XtRString, "fleur", sizeof(Cursor),
					  NULL);

    XtAddCallback(l->view, XtNsizeChanged, sizeChanged, (XtPointer) l);

    XtVaGetValues(l->subpaint, XtNx, &l->spX,
		  XtNy, &l->spY,
		  XtNborderWidth, &l->spBW,
		  NULL);

    XtVaSetValues(l->subpaint, XtNzoomX, lx, XtNzoomY, ly, NULL);
    XtVaSetValues(l->view, XtNzoomX, lx, XtNzoomY, ly, NULL);

    /*
    **	Set the current operator
     */
    GraphicAdd(fat);
    StateAddParent(shell, paint);

    return;
}
