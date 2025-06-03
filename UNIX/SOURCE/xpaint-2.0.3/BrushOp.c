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
#include <X11/Shell.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Toggle.h>
#include "xpaint.h"
#include "Misc.h"
#include "Paint.h"

#include "bitmaps/paintA.xbm"
#include "bitmaps/paintB.xbm"
#include "bitmaps/paintC.xbm"
#include "bitmaps/paintD.xbm"
#include "bitmaps/paintE.xbm"
#include "bitmaps/paintF.xbm"
#include "bitmaps/paintG.xbm"
#include "bitmaps/paintH.xbm"
#include "bitmaps/paintI.xbm"
#include "bitmaps/paintJ.xbm"
#include "bitmaps/paintK.xbm"
#include "bitmaps/paintL.xbm"
#include "bitmaps/paintM.xbm"
#include "bitmaps/paintN.xbm"
#include "bitmaps/paintO.xbm"
#include "bitmaps/paintP.xbm"
#include "bitmaps/paintQ.xbm"
#include "bitmaps/paintR.xbm"
#include "bitmaps/paintS.xbm"

#define WIDTH		16
#define HEIGHT		16
#define WIDTH_2		8
#define HEIGHT_2	8

#define BRUSH(name)	 \
	(char *)CONCAT(name,_bits), CONCAT(name,_width), CONCAT(name,_height)

static struct {
	Pixmap	pixmap;
	char	*bits;
	int	width, height;
} brushList[] = {
	{ None, BRUSH(paintA) },
	{ None, BRUSH(paintB) },
	{ None, BRUSH(paintC) },
	{ None, BRUSH(paintD) },
	{ None, BRUSH(paintE) },
	{ None, BRUSH(paintF) },
	{ None, BRUSH(paintG) },
	{ None, BRUSH(paintH) },
	{ None, BRUSH(paintI) },
	{ None, BRUSH(paintJ) },
	{ None, BRUSH(paintK) },
	{ None, BRUSH(paintL) },
	{ None, BRUSH(paintM) },
	{ None, BRUSH(paintN) },
	{ None, BRUSH(paintO) },
	{ None, BRUSH(paintP) },
	{ None, BRUSH(paintQ) },
	{ None, BRUSH(paintR) },
	{ None, BRUSH(paintS) },
};

#define default_bits	paintA_bits
#define default_width	paintA_width
#define default_height	paintA_height

static Pixmap	brushPixmap = None;
static Boolean	isPoped = False;

static void 	draw(Widget w, OpInfo *info, int x, int y)
{
	XRectangle	undo;
	int		sx = x - WIDTH_2;
	int		sy = y - HEIGHT_2;

	XSetClipOrigin(XtDisplay(w), info->filled_gc, sx, sy);

	XFillRectangle(XtDisplay(w), info->drawable, info->filled_gc,
			sx, sy, WIDTH, HEIGHT);

	if (info->surface == opPixmap) {
		XYtoRECT(sx, sy, sx + WIDTH, sy + HEIGHT, &undo);

		UndoGrow(w, sx, sy);
		UndoGrow(w, sx + WIDTH, sy + HEIGHT);
		PwUpdate(w, &undo, False);
	}
}

static void	press(Widget w, Pixmap *mask, XButtonEvent *event, OpInfo *info) 
{
	if (info->surface == opWindow && info->isFat)
		return;

	XSetClipMask(XtDisplay(w), info->filled_gc, *mask);
	
	UndoStart(w, info);

	draw(w, info, event->x, event->y);
}

static void	motion(Widget w, void *junk, XMotionEvent *event, OpInfo *info) 
{
	if (info->surface == opWindow && info->isFat)
		return;

	draw(w, info, event->x, event->y);
}

static void	release(Widget w, void *junk, XButtonEvent *event, OpInfo *info) 
{
	XSetClipMask(XtDisplay(w), info->filled_gc, None);
}

static void	setCursor(Widget w, void *pixArg)
{
	Pixmap		pix = (Pixmap)pixArg;
	static int	inited = False;
	static XColor	white, black;
	Cursor		c;

	if (!inited) {
		Colormap	map;
		XColor		junk;

		XtVaGetValues(w, XtNcolormap, &map, NULL);

		XAllocNamedColor(XtDisplay(w), map, "black", &black, &junk);
		XAllocNamedColor(XtDisplay(w), map, "white", &white, &junk);

		inited = True;
	}

	c = XCreatePixmapCursor(XtDisplay(w), pix, pix, &black, &white, WIDTH_2, HEIGHT_2);
	XDefineCursor(XtDisplay(w), XtWindow(w), c);
}

/*
**  Those public functions
*/
void *BrushAdd(Widget w)
{
	XtVaSetValues(w, XtNcompress, False, NULL);

	OpAddEventHandler(w, opWindow|opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, &brushPixmap);
	OpAddEventHandler(w, opWindow|opPixmap, ButtonMotionMask, FALSE, (OpEventProc)motion, NULL);
	OpAddEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, NULL);

	setCursor(w, (void*)brushPixmap);

	return NULL;
}
void BrushRemove(Widget w, void *junk)
{
	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonPressMask, FALSE, (OpEventProc)press, &brushPixmap);
	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonMotionMask, FALSE, (OpEventProc)motion, NULL);
	OpRemoveEventHandler(w, opWindow|opPixmap, ButtonReleaseMask, FALSE, (OpEventProc)release, NULL);
}

/*
**  Initializer to create a default brush
*/

void	BrushInit(Widget toplevel)
{
	brushList[0].pixmap = XCreatePixmapFromBitmapData(
		XtDisplay(toplevel), 
		RootWindowOfScreen(XtScreen(toplevel)),
		brushList[0].bits,
		brushList[0].width,
		brushList[0].height,
		True, False, 1);

	brushPixmap = brushList[0].pixmap;
}

/*
**  The brush selection dialog
*/

static void	closePopup(Widget button, Widget shell)
{
	isPoped = False;

	XtPopdown(shell);
}

static void 	selectBrush(Widget shell, Pixmap pix)
{
	brushPixmap = pix;

	if ((int)CurrentOp[0] == (int)BrushAdd)
		GraphicAll(setCursor, (void*)brushPixmap);
}

static Widget	createDialog(Widget w)
{
	Widget		shell, form, box, icon, firstIcon = 0, close;
	GC		gc, igc;
	XGCValues	values;
	int		i;
	Pixel		fg, bg;
	Pixmap		pix;

	shell = XtVaCreatePopupShell("brush",
			topLevelShellWidgetClass, w,
			NULL);

	form = XtVaCreateManagedWidget(NULL,
			formWidgetClass, shell,
			NULL);

	box  = XtVaCreateManagedWidget("box",
			boxWidgetClass, form,
			NULL);

	values.foreground    = WhitePixelOfScreen(XtScreen(w));
	values.background    = BlackPixelOfScreen(XtScreen(w));

	gc = XCreateGC(XtDisplay(w), 
			RootWindowOfScreen(XtScreen(w)),
			GCForeground|GCBackground, &values);

	values.background    = WhitePixelOfScreen(XtScreen(w));
	values.foreground    = BlackPixelOfScreen(XtScreen(w));
	igc = XCreateGC(XtDisplay(w), 
			RootWindowOfScreen(XtScreen(w)),
			GCForeground|GCBackground, &values);

	for (i = 0; i < XtNumber(brushList); i++) {
		if (brushList[i].pixmap == None)
			brushList[i].pixmap = XCreatePixmapFromBitmapData(
				XtDisplay(box), 
				RootWindowOfScreen(XtScreen(box)),
				brushList[i].bits,
				brushList[i].width,
				brushList[i].height,
				True, False, 1);

		icon = XtVaCreateManagedWidget("icon",
				toggleWidgetClass, box,
				XtNradioGroup, firstIcon,
				NULL);

		pix = XCreatePixmap(XtDisplay(box), 
					RootWindowOfScreen(XtScreen(box)),
					brushList[i].width,
					brushList[i].height,
					DefaultDepthOfScreen(XtScreen(box)));
	
		XtVaGetValues(icon, XtNforeground, &fg,
				    XtNbackground, &bg,
				    NULL);

		/*
		**  Clear then draw the clipped rectangle in
		*/
		XSetClipMask(XtDisplay(w), gc, None);
		XSetForeground(XtDisplay(w), gc, fg);
		XFillRectangle(XtDisplay(w), pix, gc, 0, 0,
				brushList[i].width, brushList[i].height);
		XSetClipMask(XtDisplay(w), gc, brushList[i].pixmap);
		XSetForeground(XtDisplay(w), gc, bg);
		XFillRectangle(XtDisplay(w), pix, gc, 0, 0,
				brushList[i].width, brushList[i].height);

		XtVaSetValues(icon, XtNbitmap, pix, NULL);

		if (firstIcon == NULL) {
			XtVaSetValues(icon, XtNstate, True, NULL);
			firstIcon = icon;
		}

		XtAddCallback(icon, XtNcallback, 
				(XtCallbackProc)selectBrush, (XtPointer)brushList[i].pixmap);
	}

	close  = XtVaCreateManagedWidget("close",
			commandWidgetClass, form,
			XtNfromVert, box,
			XtNtop, XtChainBottom,
			NULL);

	XtAddCallback(close, XtNcallback, (XtCallbackProc)closePopup, shell);

	XFreeGC(XtDisplay(w), gc);
	XFreeGC(XtDisplay(w), igc);

	AddDestroyCallback(shell, (void (*)(Widget, void *, XEvent *))closePopup, shell);

	XtPopup(shell, XtGrabNone);

	isPoped = True;

	return shell;
}

void	BrushSelect(Widget w)
{
	static Widget	popup = NULL;

	if (popup == NULL) 
		popup = createDialog(GetToplevel(w));
	
	XtPopup(popup, XtGrabNone);
}
