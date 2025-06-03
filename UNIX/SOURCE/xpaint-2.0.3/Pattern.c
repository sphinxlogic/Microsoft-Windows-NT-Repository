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
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Command.h>
#include <X11/CoreP.h>
#include <X11/Xaw/ToggleP.h>
#include "Colormap.h"
#include "Paint.h"
#include "xpaint.h"
#include "Misc.h"
#include "CutCopyPaste.h"
#include "Image.h"

static PaintMenuItem    fileMenu[] = {
        { "read",   NULL, NULL, MF_NONE },
        { "save",   NULL, NULL, MF_NONE },
        { "close",  NULL, NULL, MF_NONE },
};
static PaintMenuItem    editMenu[] = {
        { "undo",   NULL, NULL, MF_NONE },
        { "",       NULL, NULL, MF_NONE },
        { "cut",    NULL, NULL, MF_NONE },
        { "copy",   NULL, NULL, MF_NONE },
        { "paste",  NULL, NULL, MF_NONE },
        { "clear",  NULL, NULL, MF_NONE },
};
static PaintMenuBar     menuBar[] = {
        { "file",  XtNumber(fileMenu),  fileMenu },
        { "edit",  XtNumber(editMenu),  editMenu },
};

#define	RED	1
#define	GREEN	2
#define	BLUE	3

typedef struct {
	int	add;
	Pixmap	pix;
	Widget	button;
	Widget	fat, paint;
} LocalInfo;

static void scrollCallback(Widget w, Widget cwidget, int val, int col)
{
	XColor	color;

	CwGetColor(cwidget, &color);

	switch (col) {
	case RED:	color.red = val << 8; break;
	case GREEN:	color.green = val << 8; break;
	case BLUE:	color.blue = val << 8; break;
	}

	CwSetColor(cwidget, &color);
}

static void redCallback(Widget w, XtPointer color, XtPointer percentArg)
{
	float	percent = *(float *)percentArg;
	int	val = percent * 255;

	scrollCallback(w, (Widget)color, val, RED);
}
static void greenCallback(Widget w, XtPointer color, XtPointer percentArg)
{
	float	percent = *(float *)percentArg;
	int	val = percent * 255;

	scrollCallback(w, (Widget)color, val, GREEN);
}
static void blueCallback(Widget w, XtPointer color, XtPointer percentArg)
{
	float	percent = *(float *)percentArg;
	int	val = percent * 255;

	scrollCallback(w, (Widget)color, val, BLUE);
}
static void colorCallback(Widget w, XtPointer barsArg, XtPointer colorArg)
{
	Widget	*bars = (Widget *)barsArg;
	XColor	*color = (XColor *)colorArg;
	float	r = (float)color->red   / 65536.0;
	float	g = (float)color->green / 65536.0;
	float	b = (float)color->blue  / 65536.0;

#if 0
	XtVaSetValues(bars[0], XtNtopOfThumb, &r, NULL);
	XtVaSetValues(bars[1], XtNtopOfThumb, &g, NULL);
	XtVaSetValues(bars[2], XtNtopOfThumb, &b, NULL);
#else
	XawScrollbarSetThumb(bars[0], r, -1.0);
	XawScrollbarSetThumb(bars[1], g, -1.0);
	XawScrollbarSetThumb(bars[2], b, -1.0);
#endif
	XtVaSetValues(bars[3], XtNforeground, color->pixel, NULL);
}

static void closeCallback(Widget w, XtPointer infoArg, XtPointer junk2)
{
	LocalInfo	*info = (LocalInfo *)infoArg;

	XtDestroyWidget(GetShell(info->fat));
	XtFree((XtPointer)info);
}
static void buttonCallback(Widget w, XtPointer infoArg, XtPointer junk2)
{
	LocalInfo	*info = (LocalInfo *)infoArg;
	Pixmap		pix;
	int		width, height;
	Widget		box, icon;

	if (info) {
		PwGetPixmap(info->fat, &pix, &width, &height);
		if (info->add) {
			box = XtParent(info->button);

			icon = XtVaCreateManagedWidget(NULL, 
				toggleWidgetClass, box,
				XtNbitmap, pix,
				XtNradioGroup, info->button,
				NULL);
			XtAddCallback(icon, XtNcallback, 
				(XtCallbackProc)ToggleSetPattern, (XtPointer)info->paint);
			XtVaSetValues(icon, XtNradioData, icon, NULL);
			XawToggleSetCurrent(info->button, icon);
		} else {
			XtVaSetValues(info->button, XtNbitmap, pix, NULL);
		}
		XtVaSetValues(info->paint, XtNpattern, pix, NULL);
		XtFree((XtPointer)info);
	}
	XtDestroyWidget(GetShell(info->fat));
}
static void undoCallback(Widget w, XtPointer paint, XtPointer junk)
{
	UndoSwap((Widget)paint);
}
static void readFileCallback(Widget paint, XtPointer fileArg, XtPointer imageArg)
{
	/*
	 Not used:
	char		*file = (char *)fileArg;
	*/
	Image		*image = (Image *)imageArg;
	Pixmap		pix;
	Colormap	cmap;

	if (ImageToPixmap(image, paint, &pix, &cmap)) {
		XtVaSetValues(paint, XtNpixmap, pix, 
				 XtNcolormap, cmap, NULL);
		PwUpdate(paint, NULL, True);
		return;
	}
}
static void readCallback(Widget w, XtPointer paint, XtPointer junk)
{
	GetFileName((Widget)paint, False, NULL, readFileCallback);
}

void PatternEdit(Widget w, Pixmap pix, Widget button)
{
	Widget		shell, form, topf;
	Widget		color, fat, bar;
	Widget		red, green, blue;
	Widget		okButton, cancelButton;
	Colormap	cmap;
	Widget		*bars = (Widget*)XtMalloc(sizeof(Widget)*4);
	LocalInfo	*info = (LocalInfo*)XtMalloc(sizeof(LocalInfo));

	info->paint  = w;
	info->add    = (pix == None);
	info->button = (Widget)XawToggleGetCurrent(button);

	XtVaGetValues(w, XtNcolormap, &cmap, NULL);

	shell = XtVaCreatePopupShell("pattern", transientShellWidgetClass, GetShell(w),
				XtNcolormap, cmap,
				NULL);

	topf  = XtVaCreateManagedWidget("form", formWidgetClass, shell, 
			NULL);

	bar   = MenuBarCreate(topf, XtNumber(menuBar), menuBar);

	form  = XtVaCreateManagedWidget("form", formWidgetClass, topf, 
			XtNfromVert, bar,
			NULL);

	if (pix == None)
		fat = XtVaCreateManagedWidget("paint", paintWidgetClass, form, 
				XtNpixmap, pix,
				XtNzoom, 8,
				XtNdrawWidth, 32,
				XtNdrawHeight, 32,
				XtNfillRule, FillSolid,
				NULL);
	else
		fat = XtVaCreateManagedWidget("paint", paintWidgetClass, form, 
				XtNpixmap, pix,
				XtNzoom, 8,
				XtNfillRule, FillSolid,
				NULL);
	color = XtVaCreateManagedWidget("cedit", colormapWidgetClass, form, 
				XtNwidth, 256,
				XtNheight, 256,
				XtNcolormap, cmap,
				XtNfromHoriz, fat,
				NULL);

	red = XtVaCreateManagedWidget("red", scrollbarWidgetClass, form,
				XtNfromHoriz, color,
				XtNlength, 255,
				NULL);
	green = XtVaCreateManagedWidget("green", scrollbarWidgetClass, form,
				XtNfromHoriz, red,
				XtNlength, 255,
				NULL);
	blue = XtVaCreateManagedWidget("blue", scrollbarWidgetClass, form,
				XtNfromHoriz, green,
				XtNlength, 255,
				NULL);

	XtAddCallback(red,   XtNjumpProc, redCallback, color);
	XtAddCallback(green, XtNjumpProc, greenCallback, color);
	XtAddCallback(blue,  XtNjumpProc, blueCallback, color);
	bars[0] = red;
	bars[1] = green;
	bars[2] = blue;
	bars[3] = fat;
	XtAddCallback(color, XtNcallback, colorCallback, bars);

	info->fat    = fat;

	okButton = XtVaCreateManagedWidget("ok", 
				commandWidgetClass, topf,
				XtNfromVert, form,
				NULL);

	cancelButton = XtVaCreateManagedWidget("cancel", 
				commandWidgetClass, topf,
				XtNfromVert, form,
				XtNfromHoriz, okButton,
				NULL);
	XtAddCallback(XtNameToWidget(bar, "edit.editMenu.undo"),
			XtNcallback, undoCallback, fat);
	XtAddCallback(XtNameToWidget(bar, "edit.editMenu.cut"),
			XtNcallback, StdCutCallback, fat);
	XtAddCallback(XtNameToWidget(bar, "edit.editMenu.copy"),
			XtNcallback, StdCopyCallback, fat);
	XtAddCallback(XtNameToWidget(bar, "edit.editMenu.paste"),
			XtNcallback, StdPasteCallback, fat);
	XtAddCallback(XtNameToWidget(bar, "edit.editMenu.clear"),
			XtNcallback, StdClearCallback, fat);
	XtAddCallback(XtNameToWidget(bar, "file.fileMenu.close"),
			XtNcallback, closeCallback, info);
	XtAddCallback(XtNameToWidget(bar, "file.fileMenu.save"),
			XtNcallback, StdSaveFile, fat);
	XtAddCallback(XtNameToWidget(bar, "file.fileMenu.read"),
			XtNcallback, readCallback, fat);

	XtAddCallback(okButton, XtNcallback, buttonCallback, (XtPointer)info);
	XtAddCallback(cancelButton, XtNcallback, closeCallback, (XtPointer)info);
	AddDestroyCallback(shell, (void (*)(Widget, void *, XEvent *))closeCallback, info);

	XtPopup(shell, XtGrabNone);

	GraphicAdd(fat);
}
