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
#include <X11/Xaw/Form.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Toggle.h>
#include <X11/cursorfont.h>
#include <stdio.h>

#ifndef NOSTDHDRS
#include <stdlib.h>
#include <unistd.h>
#endif

#include "bitmaps/brushOp.xbm"
#if 0
#include "bitmaps/eraseOp.xbm"
#endif
#include "bitmaps/sprayOp.xbm"
#include "bitmaps/pencilOp.xbm"
#include "bitmaps/lineOp.xbm"
#include "bitmaps/arcOp.xbm"
#include "bitmaps/fillOp.xbm"
#include "bitmaps/textOp.xbm"
#include "bitmaps/selectOp.xbm"
#include "bitmaps/boxOp.xbm"
#include "bitmaps/fboxOp.xbm"
#include "bitmaps/ovalOp.xbm"
#include "bitmaps/fovalOp.xbm"
#include "bitmaps/clineOp.xbm"
#include "bitmaps/polyOp.xbm"
#include "bitmaps/fpolyOp.xbm"
#include "bitmaps/selareaOp.xbm"

#include "bitmaps/checkmark.xbm"

extern void AboutDialog(Widget);

#define	OP_BEGIN	0
#define	OP_END		1

#include "xpaint.h"
#include "Misc.h"
#include "Paint.h"
#include "Text.h"

static void setOperation(Widget w, XtPointer opArg, XtPointer junk2) 
{
	Operation_t	*op = (Operation_t*)opArg;
	Operation_t	stop, start;

	stop  = CurrentOp ? CurrentOp[OP_END]   : NULL;
	start = op        ? op       [OP_BEGIN] : NULL;

	GraphicSetOp((void (*)(Widget, void *))stop,
			(void *(*)(Widget))start);

	CurrentOp = op;
}


/*
**  The set line width callback pair
*/
static Widget	currentLineSelection;
static char	currentLineWidth[10] = "1";

static void setWidth(Widget w, void *width)
{
	XtVaSetValues(w, XtNlineWidth, (int)width, NULL);
}
static void okLineCallback(Widget w, XtPointer junk, XtPointer infoArg)
{
	TextPromptInfo	*info = (TextPromptInfo*)infoArg;
	int		width = atoi(info->prompts[0].rstr);

	if (width < 0 || width > 1000) {
		Notice(w, "Invalid width, must be greater than zero and less than 1000");
		return;
	}
	sprintf(currentLineWidth, "%d", width);

	XtVaSetValues(currentLineSelection, XtNleftBitmap, None, NULL);
	XtVaSetValues(currentLineSelection = w, XtNleftBitmap, CheckBitmap, NULL);
	GraphicAll(setWidth, (void*)width);
}
static void lineWidth(Widget w, int width)
{
	if (width < 0) {
		static TextPromptInfo		info;
		static struct textPromptInfo	value[1];

		value[0].prompt = "Width:";
		value[0].str    = currentLineWidth;
		value[0].len    = 4;
		info.prompts = value;
		info.title   = "Enter desired line width";
		info.nprompt = 1;

		TextPrompt(w, "linewidth", &info, okLineCallback, NULL, NULL);
	} else {
		XtVaSetValues(currentLineSelection, XtNleftBitmap, None, NULL);
		XtVaSetValues(currentLineSelection = w, XtNleftBitmap, CheckBitmap, NULL);
		GraphicAll(setWidth, (void *)width);
	}
}
/*
**
*/
static char	sprayDensityStr[10] = "10";
static char	sprayRadiusStr[10] = "10";
static void sprayOkCallback(Widget w, XtPointer junk, XtPointer infoArg)
{
	void		SpraySetParameters(int, int);
	TextPromptInfo	*info = (TextPromptInfo*)infoArg;
	int		radius  = atoi(info->prompts[0].rstr);
	int		density = atoi(info->prompts[1].rstr);

	if (radius < 1 || radius > 100) {
		Notice(w, "Radius should be between 1 and 100");
		return;
	}
	if (density < 1 || density > 500) {
		Notice(w, "Density should be between 1 and 500");
		return;
	}

	SpraySetParameters(radius, density);

	sprintf(sprayDensityStr,"%d",density);
	sprintf(sprayRadiusStr,"%d",radius);
}
static void sprayMenuCallback(Widget w)
{
		static TextPromptInfo		info;
		static struct textPromptInfo	value[2];

		value[0].prompt = "Radius:";
		value[0].str    = sprayRadiusStr;
		value[0].len    = 4;
		value[1].prompt = "Density:";
		value[1].str    = sprayDensityStr;
		value[1].len    = 4;
		info.prompts = value;
		info.title   = "Enter the desired spray parameters";
		info.nprompt = 2;

		TextPrompt(w, "sprayparams", &info, sprayOkCallback, NULL, NULL);
}

/*
**  Font menu callbacks.
*/
static Widget	fontChoiceCheck = None;
static void fontSetCallback(Widget paint, void *info)
{
	XtVaSetValues(paint, XtNfont, (XFontStruct*)info, NULL);
}
static void fontSet(Widget w, char *name)
{
	XFontStruct	*info;

	if (name == NULL) {
		FontSelect(w, None);
	} else {
		if ((info = XLoadQueryFont(XtDisplay(GetShell(w)), name)) == NULL) {
			XtVaSetValues(w, XtNsensitive, False, NULL);
			Notice(w, "Unable to load requested font");
			return;
		}
		GraphicAll(fontSetCallback, (void*)info);
	}
	XtVaSetValues(fontChoiceCheck, XtNleftBitmap, None, NULL);
	XtVaSetValues(fontChoiceCheck = w, XtNleftBitmap, CheckBitmap, NULL);
}

/*
**  Exit callback (simple)
*/
static void exitOkCallback(Widget paint, XtPointer junk, XtPointer junk2)
{
	exit(0);
}
static void exitCancelCallback(Widget paint, XtPointer junk, XtPointer junk2)
{
}
static void exitPaintCheck(Widget paint, void *sumArg)
{
	int	*sum = (int *)sumArg;
	Boolean	flg;
	XtVaGetValues(paint, XtNdirty, &flg, NULL);
	*sum += flg ? 1 : 0;
}
static void exitPaint(Widget w, XtPointer junk, XtPointer junk2)
{
	int	total = 0;

	GraphicAll(exitPaintCheck, (void*)&total);

	if (total == 0)
		exit(0);
	
	AlertBox(w, "There are unsaved changes, are you sure you wish to quit?",
				exitOkCallback, exitCancelCallback, NULL);
}

#define GENERATE_OP(name)				\
	extern void *CONCAT(name,Add)(Widget w,...),	\
		    *CONCAT(name,Remove)(Widget w,...);	\
	static Operation_t  CONCAT(name,Op)[] = {	\
		CONCAT(name,Add), CONCAT(name,Remove)	\
	};

GENERATE_OP(Pencil)
GENERATE_OP(Box)
GENERATE_OP(FBox)
GENERATE_OP(Line)
GENERATE_OP(Arc)
GENERATE_OP(Oval)
GENERATE_OP(FOval)
#if 0
GENERATE_OP(Erase)
#endif
GENERATE_OP(Brush)
GENERATE_OP(Font)
GENERATE_OP(Spray)
GENERATE_OP(Poly)
GENERATE_OP(FPoly)
GENERATE_OP(CLine)
GENERATE_OP(Fill)
GENERATE_OP(SelectBox)
GENERATE_OP(SelectArea)

#define	BITMAP(name)	\
	(char *)CONCAT(name,Op_bits) , CONCAT(name,Op_width), CONCAT(name,Op_height)

static struct {
	char	*name;
	char	*bitmap;
	int	width, height;
	void	(*callback)();		/* By default setOperation() */
	void	*data;
} iconList[] = {
	{ "Brush",	BITMAP(brush), NULL, BrushOp},
#if 0
	{ "Erase",	BITMAP(erase), NULL, EraseOp },
#endif
	{ "Spray",	BITMAP(spray), NULL, SprayOp },
	{ "Pencil",	BITMAP(pencil), NULL, PencilOp },
	{ "Line",	BITMAP(line), NULL, LineOp },
	{ "Arc",	BITMAP(arc), NULL, ArcOp },
	{ "Fill",	BITMAP(fill), NULL, FillOp },
	{ "Text",	BITMAP(text), NULL, FontOp },
	{ "SelectBox",	BITMAP(select), NULL, SelectBoxOp },
	{ "SelectArea",	BITMAP(selarea), NULL, SelectAreaOp },
	{ "Box",	BITMAP(box), NULL, BoxOp },
	{ "FBox",	BITMAP(fbox), NULL, FBoxOp },
	{ "Oval",	BITMAP(oval), NULL, OvalOp },
	{ "FOval",	BITMAP(foval), NULL, FOvalOp },
	{ "CLine",	BITMAP(cline), NULL, CLineOp },
	{ "Polygon",	BITMAP(poly), NULL, PolyOp },
	{ "FPolygon",	BITMAP(fpoly), NULL, FPolyOp },
};

static PaintMenuItem lineMenu[] = {
	{ "0", lineWidth, (void*)0, MF_CHECK },
	{ "1", lineWidth, (void*)1, MF_CHECK },
	{ "2", lineWidth, (void*)2, MF_CHECK },
	{ "4", lineWidth, (void*)4, MF_CHECK },
	{ "8", lineWidth, (void*)8, MF_CHECK },
	{ "select", lineWidth, (void*)-1, MF_CHECK },
};

/*
**  WARNING:  If you change the names of the fonts make sure you change
**            the XtNameToWidget call down at the bottom to make sure
**            it finds the right widget (it'll core dump otherwise)
*/
static PaintMenuItem	fontMenu[] = {
	{ "Times 8", fontSet, 
			"-*-times-medium-r-normal-*-*-80-*-*-p-*-*-*",      MF_CHECK },
	{ "Times 12", fontSet, 
			"-*-times-medium-r-normal-*-*-120-*-*-p-*-*-*",     MF_CHECK },
	{ "Times 18", fontSet, 
			"-*-times-medium-r-normal-*-*-180-*-*-p-*-*-*",     MF_CHECK },
	{ "Times Bold 12", fontSet, 
			"-*-times-bold-r-normal-*-*-120-*-*-p-*-*-*",       MF_CHECK },
	{ "Times Italic 12", fontSet, 
			"-*-times-medium-i-normal-*-*-120-*-*-p-*-*-*",     MF_CHECK },
	{ "Lucida 12", fontSet, 
			"-*-lucida-medium-r-normal-*-*-120-*-*-p-*-*-*",    MF_CHECK },
	{ "Helvetica 12", fontSet, 
			"-*-helvetica-medium-r-normal-*-*-120-*-*-p-*-*-*", MF_CHECK },
	{ "Helvetica Bold 12", fontSet, 
			"-*-helvetica-bold-r-normal-*-*-120-*-*-p-*-*-*",   MF_CHECK },
	{ "Fixed 12", fontSet, 
			"-*-fixed-medium-r-normal-*-*-120-*-*-m-*-*-*",     MF_CHECK },
	{ "Courier 12", fontSet, 
			"-*-courier-medium-r-normal-*-*-120-*-*-m-*-*-*",   MF_CHECK },
	{ "",           NULL, NULL, MF_NONE },
	{ "browse",     fontSet, NULL, MF_CHECK },
};

static PaintMenuItem otherMenu[] = {
	{ "brushSelect", BrushSelect, NULL, MF_NONE },
	{ "sprayEdit", sprayMenuCallback, NULL, MF_NONE },
};

static PaintMenuItem fileMenu[] = {
	{ "new",   (PaintMenuCallback)GraphicCreate, (void*)0, MF_NONE },
	{ "open",  (PaintMenuCallback)GraphicCreate, (void*)1, MF_NONE },
	{ "new-size",  (PaintMenuCallback)GraphicCreate, (void*)2, MF_NONE },
	{ "",	   NULL, NULL, MF_NONE },
	{ "about", (PaintMenuCallback)AboutDialog, NULL, MF_NONE },
	{ "",	   NULL, NULL, MF_NONE },
	{ "quit",  (PaintMenuCallback)exitPaint, NULL, MF_NONE },
};

static PaintMenuBar   menuBar[] = {
	{ "file",  XtNumber(fileMenu), fileMenu },
	{ "other",  XtNumber(otherMenu), otherMenu },
	{ "line",  XtNumber(lineMenu), lineMenu },
	{ "font",  XtNumber(fontMenu), fontMenu },
};

void OperationInit(Widget toplevel)
{
	int		i;
	Pixmap		pix;
	Widget		form, box, icon, bar, firstIcon = NULL;
	Pixel		fg, bg;
	XGCValues	values;

	form = XtVaCreateManagedWidget("toolbox", 
			formWidgetClass, toplevel,
			XtNborderWidth, 0,
			NULL);
	/*
	**  Create the menu bar
	*/
	bar = MenuBarCreate(form, XtNumber(menuBar), menuBar);

	/*
	**  Create the operation icon list
	*/
	box = XtVaCreateManagedWidget("box", 
			boxWidgetClass, form,
			XtNfromVert, bar,
			XtNwidth, 32 * 8,
			XtNtop, XtChainTop,
			NULL);

	for (i = 0; i < XtNumber(iconList); i++) {
		icon = XtVaCreateManagedWidget(iconList[i].name,
			toggleWidgetClass, box,
			XtNtranslations, 
				XtParseTranslationTable(
					"<BtnDown>,<BtnUp>: set() notify()"),
			XtNradioGroup, firstIcon,
			NULL);

		if (iconList[i].bitmap != NULL) {
			XtVaGetValues(icon, XtNforeground, &fg,
					    XtNbackground, &bg, 
					    NULL);

			pix = XCreatePixmapFromBitmapData(
					XtDisplay(box), 
					DefaultRootWindow(XtDisplay(box)),
					iconList[i].bitmap,
					iconList[i].width,
					iconList[i].height,
					fg, bg,
					DefaultDepthOfScreen(XtScreen(box)));

		} else {
			pix = None;
		}
		XtVaSetValues(icon, XtNbitmap, pix, NULL);

		if (firstIcon == NULL) {
			XtVaSetValues(icon, XtNstate, True, NULL);
			setOperation(icon, iconList[i].data, NULL);

			firstIcon = icon;
		}
		if (iconList[i].callback != NULL)
			XtAddCallback(icon, XtNcallback, 
				iconList[i].callback, iconList[i].data);
		else
			XtAddCallback(icon, XtNcallback, 
				setOperation, iconList[i].data);
	}

	CheckBitmap = XCreateBitmapFromData(XtDisplay(toplevel), 
				DefaultRootWindow(XtDisplay(toplevel)),
				(char *)checkmark_bits,
				checkmark_width,
				checkmark_height);

	currentLineSelection = XtNameToWidget(bar, "line.lineMenu.0");
        XtVaSetValues(currentLineSelection, XtNleftBitmap, CheckBitmap, NULL);
	fontChoiceCheck = XtNameToWidget(bar, "font.fontMenu.Times 12");
	XtVaSetValues(fontChoiceCheck, XtNleftBitmap, CheckBitmap, NULL);

	/*
	**  Init the global xor gc
	*/
        values.background = WhitePixelOfScreen(XtScreen(toplevel));
        values.foreground = BlackPixelOfScreen(XtScreen(toplevel));
        values.function   = GXinvert;
	Global.gcx  = XtGetGC(toplevel, GCFunction|GCForeground|GCBackground, &values);

	AddDestroyCallback(toplevel, 
			(void (*)(Widget, void *, XEvent *))exitPaint, NULL);
}
