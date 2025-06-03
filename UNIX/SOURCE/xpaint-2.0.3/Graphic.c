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
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/Paned.h>
#include <stdio.h>

#ifndef NOSTDHDRS
#include <stdlib.h>
#include <unistd.h>
#endif

#include "Paint.h"

#include "xpaint.h"
#include "Misc.h"
#include "Palette.h"
#include "Text.h"
#include "Image.h"
#include "CutCopyPaste.h"

extern void UndoSwap(Widget w);

#include "bitmaps/pattern2.xbm"
#include "bitmaps/pattern3.xbm"
#include "bitmaps/pattern4.xbm"
#include "bitmaps/pattern5.xbm"
#include "bitmaps/pattern6.xbm"
#include "bitmaps/pattern7.xbm"
#include "bitmaps/pattern8.xbm"
#include "bitmaps/pattern9.xbm"
#include "bitmaps/background.xbm"

#define DEFAULT_TITLE	"Untitled"

#define PATTERN(name)      \
	(char *)CONCAT(name,_bits), CONCAT(name,_width), CONCAT(name,_height)

struct callData {
	Widget	paint;
	Pixmap	pixmap;
};

struct paintWindows {
	Widget			paint;
	struct paintWindows	*next;
	void			*ldata;
};

static struct {
	char	*bits;
	int	width, height;
} patternList[] = {
	{ PATTERN(pattern2) },
	{ PATTERN(pattern3) },
	{ PATTERN(pattern4) },
	{ PATTERN(pattern5) },
	{ PATTERN(pattern6) },
	{ PATTERN(pattern7) },
	{ PATTERN(pattern8) },
	{ PATTERN(pattern9) },
};

static PaintMenuItem	fileMenu[] = {
	{ "save",   NULL, NULL, MF_NONE },
	{ "saveregion",   NULL, NULL, MF_NONE },
	{ "close",  NULL, NULL, MF_NONE },
};

static PaintMenuItem	editMenu[] = {
	{ "undo",   NULL, NULL, MF_NONE },
	{ "",       NULL, NULL, MF_NONE },
	{ "cut",    NULL, NULL, MF_NONE },
	{ "copy",   NULL, NULL, MF_NONE },
	{ "paste",  NULL, NULL, MF_NONE },
	{ "clear",  NULL, NULL, MF_NONE },
	{ "",       NULL, NULL, MF_NONE },
#if 0
	{ "lock",   NULL, NULL, MF_NONE },
	{ "unlock", NULL, NULL, MF_NONE },
#endif
};

static PaintMenuItem	otherMenu[] = {
	{ "fatbits",    NULL, NULL, MF_NONE  },
	{ "grid",       NULL, NULL, MF_CHECK },
	{ "gridSpacing",NULL, NULL, MF_NONE },
	{ "",           NULL, NULL, MF_NONE },
	{ "size",       NULL, NULL, MF_NONE },
	{ "zoom",       NULL, NULL, MF_NONE },
};

static PaintMenuBar	menuBar[] = {
	{ "file",  XtNumber(fileMenu),  fileMenu },
	{ "edit",  XtNumber(editMenu),  editMenu },
	{ "other", XtNumber(otherMenu), otherMenu },
};

static struct paintWindows	*head = NULL;

void GraphicRemove(Widget paint, XtPointer junk, XtPointer junk2)
{
	struct paintWindows	*cur, *prev = NULL;

	for (cur = head; cur != NULL; prev = cur, cur=cur->next) 
		if (cur->paint == paint) 
			break;

	if (cur == NULL)
		return;

	if (prev == NULL)
		head = cur->next;
	else
		prev = cur->next;

	CurrentOp[1](cur->paint, cur->ldata);
	XtFree((XtPointer)cur);
}

void GraphicAdd(Widget paint)
{
 	struct paintWindows	*new;

	new = (struct paintWindows *)XtMalloc(sizeof(struct paintWindows));

	if (new == NULL) {
		/* big error XXX */
		return;
	}

	new->next  = head;
	head       = new;
	new->paint = paint;

	if (CurrentOp != NULL && CurrentOp[0] != NULL)
		new->ldata = CurrentOp[0](paint);

	XtAddCallback(paint, XtNdestroyCallback, GraphicRemove, NULL);
}


void GraphicAll(void (*func)(), void *data)
{
	struct paintWindows	*cur;

	for (cur = head; cur != NULL; cur=cur->next)
		func(cur->paint, data);
}
void GraphicSetOp(void (*stop)(Widget, void *), void *(*start)(Widget))
{
	struct paintWindows	*cur;

	for (cur = head; cur != NULL; cur=cur->next) {
		if (stop != NULL)
			stop(cur->paint, cur->ldata);
		if (start != NULL)
			cur->ldata = start(cur->paint);
	}
}
void *GraphicGetData(Widget w)
{
	struct paintWindows	*cur;

	for (cur = head; cur != NULL; cur=cur->next)
		if (cur->paint == w)
			return cur->ldata;
	return NULL;
}

static void	setPatternCallback(Widget icon, XtPointer paintArg, XtPointer junk2)
{
	Widget	paint = (Widget)paintArg;
	Pixmap	pix;

	if (strncmp(XtName(icon), "solid", 5) == 0) {
		XImage	*xim;
		Pixel	p;
		XtVaGetValues(icon, XtNbitmap, &pix, NULL);
		xim = XGetImage(XtDisplay(icon), pix, 0, 0, 1, 1, ~0, XYPixmap);
		p = XGetPixel(xim, 0, 0);
		XtVaSetValues(paint, XtNforeground, p, XtNfillRule, FillSolid, NULL);
		XDestroyImage(xim);
	} else {
		XtVaGetValues(icon, XtNbitmap, &pix, NULL);
		XtVaSetValues(paint, XtNpattern, pix, XtNfillRule, FillTiled, NULL);
	}
}
void	ToggleSetPattern(Widget icon, Widget paint)
{
	setPatternCallback(icon, (XtPointer)paint, NULL);
}

/*
**  First level menu callbacks.
*/

static void undoCallback(Widget w, XtPointer paint, XtPointer junk2)
{
	UndoSwap((Widget)paint);
}
static void closeOkCallback(Widget shell, XtPointer junk, XtPointer junk2)
{
	XtDestroyWidget(shell);
}
static void closeCancelCallback(Widget shell, XtPointer junk, XtPointer junk2)
{
}

static void closeCallback(Widget w, XtPointer paintArg, XtPointer junk2)
{
	Widget	paint = (Widget)paintArg;
	Boolean	flg;

	XtVaGetValues(paint, XtNdirty, &flg, NULL);
	if (flg)
		AlertBox(GetShell(paint), "Unsaved changes!\nDo you really wish to close?", 
				closeOkCallback, closeCancelCallback, NULL);
	else
		XtDestroyWidget(GetShell(paint));
}

/*
**  Pattern edit/add callback
*/
static void editCallback(Widget w, XtPointer wlArg, XtPointer junk2)
{
	Widget	*wl = (Widget*)wlArg;

	if (strncmp(XtName((Widget)XawToggleGetCurrent(wl[1])), "solid", 5) == 0) {
		Notice(w, "Editing the solid colors.\nIs not currently supported");
	} else {
		void	PatternEdit(Widget, Pixmap, Widget);
		Pixmap	pix;

		XtVaGetValues(wl[0], XtNpattern, &pix, NULL);
		PatternEdit(wl[0], pix, wl[1]);
	}
}
static void addCallback(Widget w, XtPointer wlArg, XtPointer junk2)
{
	void	PatternEdit(Widget, Pixmap, Widget);
	Widget	*wl = (Widget*)wlArg;

	PatternEdit(wl[0], None, wl[1]);
}

/*
**
*/
static void fatCallback(Widget w, XtPointer paint, XtPointer junk2)
{
	void	FatbitsEdit(Widget);

	FatbitsEdit((Widget)paint);
}
static void gridCallback(Widget w, XtPointer paintArg, XtPointer junk2)
{
	Widget	paint = (Widget)paintArg;
	Boolean	v;

	XtVaGetValues(paint, XtNgridOn, &v, NULL);
	v = !v;
	XtVaSetValues(paint, XtNgridOn, v, NULL);

	XtVaSetValues(w, XtNleftBitmap, v ? CheckBitmap : None, NULL);
}
/*
**  
*/
static int  gridSpacing = 10;
static void changeGridOkCallback(Widget paint, void *junk, XtPointer infoArg)
{
	TextPromptInfo	*info = (TextPromptInfo*)infoArg;
	int		v = atoi(info->prompts[0].rstr);

	if (v < 1 || v > 100) {
		Notice(paint,"Bad grid spacing.\nShould be between 2 and 100");
		return;
	}
		
	gridSpacing = v;
	XtVaSetValues(paint, XtNgrid, v, NULL);
}
static void changeGridCallback(Widget w, XtPointer paintArg, XtPointer junk2)
{
	Widget paint = (Widget)paintArg;
	static TextPromptInfo		info;
	static struct textPromptInfo	value[1];
	static char	buf[10];

	sprintf(buf, "%d", gridSpacing);

	value[0].prompt = "Spacing:";
	value[0].str    = buf;
	value[0].len    = 4;
	info.prompts = value;
	info.title   = "Enter desired grid spacing";
	info.nprompt = 1;

	TextPrompt(paint, "linewidth", &info, changeGridOkCallback, NULL, NULL);
}
static void sizeCallback(Widget w, XtPointer paintArg, XtPointer junk2)
{
	Widget	paint = (Widget)paintArg;

	SizeSelect(GetShell(paint), paint, NULL);
}
static void zoomCallback(Widget w, XtPointer paintArg, XtPointer junk2)
{
	Widget	paint = (Widget)paintArg;

	ZoomSelect(GetShell(paint), paint);
}

Widget graphicCreate(Widget wid, int width, int height, int zoom, Pixmap pix, Colormap cmap)
{
	Widget			shell, form, viewport, pattern, bar;
	Widget			pane, pane1;
	Widget			paint, viewport2;
	Widget			icon, firstIcon = NULL;
	Widget			add, edit;
	Palette			*map;
	GC			gc;
	int			i, j;
	struct callData		*cd;
	XColor			col, rgb;
	Widget			*wl = (Widget*)XtMalloc(sizeof(Widget) * 2);
	Pixmap			bpix;
	static struct {
		char	*name;
		Pixel	pixel;
		Boolean	doit;
	} color[] = {
		{ "black", 0, False },
		{ "white", 0, False },
		{ "red", 0, False },
		{ "green", 0, False },
		{ "blue", 0, False },
		{ "cyan", 0, False },
		{ "magenta", 0, False },
		{ "yellow", 0, False },
	};

	/*
	**  Menu Bar
	*/
	shell = XtVaCreatePopupShell("graphic",
			topLevelShellWidgetClass, GetToplevel(wid),
			XtNtitle, DEFAULT_TITLE,
			XtNiconName, DEFAULT_TITLE,
			NULL);

	if (cmap == None) {
		map = PaletteCreate(shell);
		cmap = map->cmap;
	} else {
		map = PaletteFind(shell, cmap);
	}

	pane = XtVaCreateWidget("pane", 
			panedWidgetClass, shell,
			NULL);

	XtVaSetValues(shell, XtNcolormap, cmap, NULL);
	XtVaSetValues(pane, XtNcolormap, cmap, NULL);
	XtManageChild(pane);

	/*
	**  Allocate the color entries
	*/
	for (i = 0; i < XtNumber(color); i++) 
		color[i].doit  = False;

	for (i = 0; i < XtNumber(color); i++) {
		if (XLookupColor(XtDisplay(shell), cmap, color[i].name, &rgb, &col)) {
			color[i].pixel = PaletteAlloc(map, &col);
			color[i].doit  = True;
			for (j = 0; j < i; j++)
				if (color[i].pixel == color[j].pixel)
					color[i].doit = False;
				
		}
	}

	/*
	**  Menu area
	*/
	form = XtVaCreateManagedWidget("form1", 
			formWidgetClass, pane,
			NULL);
	bar = MenuBarCreate(form, XtNumber(menuBar), menuBar);
	XtVaSetValues(form, XtNshowGrip, False, NULL);

	/*
	**  Drawing Area
	*/
	viewport = XtVaCreateManagedWidget("viewport",
			viewportWidgetClass, pane,
			XtNfromVert, bar,
			XtNallowVert, True,
			XtNallowHoriz, True,
			XtNuseBottom, True,
			XtNuseRight, True,
			XtNtop, XtChainTop,
			NULL);

	/*
	**  Custom Drawing Widget here
	**   First read the file and convert to a bitmap
	*/
	bpix = XCreatePixmapFromBitmapData(
			XtDisplay(shell), 
			DefaultRootWindow(XtDisplay(shell)),
			(char *)background_bits,
			background_width,
			background_height,
			WhitePixelOfScreen(XtScreen(shell)),
			BlackPixelOfScreen(XtScreen(shell)),
			DefaultDepthOfScreen(XtScreen(shell)));
	
	pane1 = XtVaCreateManagedWidget("box", 
			boxWidgetClass, viewport,
			XtNbackground, BlackPixelOfScreen(XtScreen(shell)),
			XtNbackgroundPixmap, bpix,
			NULL);

	paint = XtVaCreateManagedWidget("paint",
			paintWidgetClass, pane1,
			XtNdrawWidth, width,
			XtNdrawHeight, height,
			XtNzoom, zoom,
			XtNpixmap, pix,
			XtNcolormap, cmap,
			XtNbackground, WhitePixelOfScreen(XtScreen(shell)), 
			XtNforeground, BlackPixelOfScreen(XtScreen(shell)), 
			XtNallowResize, True,
			/* XtNresizeToPreferred, True, */
			XtNshowGrip, False,
			NULL);

	XtAddCallback(XtNameToWidget(bar, "edit.editMenu.undo"), 
			XtNcallback, undoCallback, (XtPointer)paint);
	XtAddCallback(XtNameToWidget(bar, "edit.editMenu.cut"), 
			XtNcallback, StdCutCallback, (XtPointer)paint);
	XtAddCallback(XtNameToWidget(bar, "edit.editMenu.copy"), 
			XtNcallback, StdCopyCallback, (XtPointer)paint);
	XtAddCallback(XtNameToWidget(bar, "edit.editMenu.paste"), 
			XtNcallback, StdPasteCallback, (XtPointer)paint);
	XtAddCallback(XtNameToWidget(bar, "edit.editMenu.clear"), 
			XtNcallback, StdClearCallback, (XtPointer)paint);
	XtAddCallback(XtNameToWidget(bar, "file.fileMenu.save"), 
			XtNcallback, StdSaveFile, (XtPointer)paint);
	XtAddCallback(XtNameToWidget(bar, "file.fileMenu.saveregion"), 
			XtNcallback, StdSaveRegionFile, (XtPointer)paint);
	XtAddCallback(XtNameToWidget(bar, "file.fileMenu.close"), 
			XtNcallback, closeCallback, (XtPointer)paint);
	XtAddCallback(XtNameToWidget(bar, "other.otherMenu.grid"), 
			XtNcallback, gridCallback, (XtPointer)paint);
	XtAddCallback(XtNameToWidget(bar, "other.otherMenu.gridSpacing"), 
			XtNcallback, changeGridCallback, (XtPointer)paint);
	XtAddCallback(XtNameToWidget(bar, "other.otherMenu.fatbits"), 
			XtNcallback, fatCallback, (XtPointer)paint);
	XtAddCallback(XtNameToWidget(bar, "other.otherMenu.size"), 
			XtNcallback, sizeCallback, (XtPointer)paint);
	XtAddCallback(XtNameToWidget(bar, "other.otherMenu.zoom"), 
			XtNcallback, zoomCallback, (XtPointer)paint);

	form = XtVaCreateManagedWidget("form2", 
			formWidgetClass, pane,
			NULL);
	/*
	**  Pattern Area
	*/
	viewport2 = XtVaCreateManagedWidget("viewport2",
			viewportWidgetClass, form,
			XtNallowVert, TRUE,
			XtNallowHoriz, TRUE,
			XtNuseBottom, TRUE,
			XtNuseRight, TRUE,
			NULL);
	pattern = XtVaCreateManagedWidget("pattern",
			boxWidgetClass, viewport2,
			XtNheight, 40,
			XtNwidth, 512,
			NULL);
	
	cd = (struct callData *)XtMalloc(XtNumber(patternList) *
						sizeof(struct callData));

	gc = XCreateGC(XtDisplay(pattern), DefaultRootWindow(XtDisplay(pattern)), 0, 0);
	for (i = 0; i < XtNumber(color); i++) {
		Pixmap			pix;
		char			buf[40];

		if (!color[i].doit)
			continue;

		pix = XCreatePixmap(XtDisplay(pattern),
				DefaultRootWindow(XtDisplay(pattern)),
				32, 32,
				DefaultDepthOfScreen(XtScreen(pattern)));
		XSetForeground(XtDisplay(pattern), gc, color[i].pixel);
		XFillRectangle(XtDisplay(pattern), pix, gc, 0, 0, 32, 32);

		sprintf(buf, "solid%d", i);
		icon = XtVaCreateManagedWidget(XtNewString(buf),
				toggleWidgetClass, pattern,
				XtNbitmap, pix,
				XtNradioGroup, firstIcon,
				NULL);
		XtVaSetValues(icon, XtNradioData, icon, NULL);

		if (firstIcon == NULL) {
			XtVaSetValues(icon, XtNstate, True, NULL);
			XtVaSetValues(paint, XtNpattern, pix, NULL);
			firstIcon = icon;
		}

		XtAddCallback(icon, XtNcallback, setPatternCallback, (XtPointer)paint);
	}
	XFreeGC(XtDisplay(pattern), gc);

	for (i = 0; i < XtNumber(patternList); i++) {
		Pixmap			pix;
		char			buf[40];

		pix = XCreatePixmapFromBitmapData(
				XtDisplay(pattern), 
				DefaultRootWindow(XtDisplay(pattern)),
				patternList[i].bits,
				patternList[i].width,
				patternList[i].height,
				WhitePixelOfScreen(XtScreen(pattern)),
				BlackPixelOfScreen(XtScreen(pattern)),
				DefaultDepthOfScreen(XtScreen(pattern)));
		
		sprintf(buf, "pattern%d", i);
		icon = XtVaCreateManagedWidget(XtNewString(buf),
				toggleWidgetClass, pattern,
				XtNbitmap, pix,
				XtNradioGroup, firstIcon,
				NULL);
		XtVaSetValues(icon, XtNradioData, icon, NULL);

		if (firstIcon == NULL) {
			XtVaSetValues(icon, XtNstate, True, NULL);
			XtVaSetValues(paint, XtNpattern, pix, NULL);
			firstIcon = icon;
		}

		XtAddCallback(icon, XtNcallback, setPatternCallback, (XtPointer)paint);
	}

	edit = XtVaCreateManagedWidget("edit",
			commandWidgetClass, form,
			XtNfromHoriz, viewport2,
			XtNleft, XtChainRight,
			XtNtop, XtChainBottom,
			XtNbottom, XtChainBottom,
			NULL);
	add  = XtVaCreateManagedWidget("add",
			commandWidgetClass, form,
			XtNfromHoriz, viewport2,
			XtNfromVert,  edit,
			XtNleft, XtChainRight,
			XtNtop, XtChainBottom,
			XtNbottom, XtChainBottom,
			NULL);
	wl[0] = paint;
	wl[1] = firstIcon;
	XtAddCallback(edit, XtNcallback, editCallback, wl);
	XtAddCallback(add , XtNcallback, addCallback, wl);

	AddDestroyCallback(shell, 
		(void (*)(Widget, void *, XEvent *))closeCallback,(void*)paint);

	XtManageChild(shell);
	XtRealizeWidget(shell);

	GraphicAdd(paint);

	return shell;
}

void GraphicOpenFile(Widget w, XtPointer fileArg, XtPointer imageArg)
{
	char		*file = (char *)fileArg;
	Image		*image = (Image *)imageArg;
	Colormap	cmap;
	Pixmap		pix;
	Widget		shell;

	if (ImageToPixmap(image, w, &pix, &cmap)) {
		if ((shell = graphicCreate(w, 0, 0, -1, pix, cmap)) != None) {
			char	*cp = strrchr(file, '/');
			if (cp == NULL)
				cp = file;
			else
				cp++;

			XtVaSetValues(shell, XtNiconName, cp, XtNtitle, file, NULL);
		}
	} else {
		Notice(w, "Unable to create paint window with image");
	}
}

static void doCreate(Widget wid, int width, int height, int zoom)
{
	graphicCreate(wid, width, height, zoom, None, None);
}

void GraphicCreate(Widget wid, int value)
{
	switch (value) {
	case 0:
		graphicCreate(wid, 512, 512, -1, None, None);
		break;
	case 1:
		GetFileName(GetToplevel(wid), False, NULL, GraphicOpenFile);
		break;
	case 2:
		SizeSelect(wid, None, doCreate);
		break;
	}
}
