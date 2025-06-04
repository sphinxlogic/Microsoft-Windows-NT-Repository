
/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, 1993, David Koblas (koblas@netcom.com)            | */
/* | Copyright 1995, 1996 Torsten Martinsen (bullestock@dk-online.dk)  | */
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

/* $Id: chroma.c,v 1.5 1996/04/19 08:53:58 torsten Exp $ */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#ifndef VMS
#include <X11/Xaw/Form.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Command.h>
#else
#include <X11Xaw/Form.h>
#include <X11Xaw/AsciiText.h>
#include <X11Xaw/Toggle.h>
#include <X11Xaw/Scrollbar.h>
#include <X11Xaw/Command.h>
#endif
#include <X11/Shell.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>

#include "xpaint.h"
#include "Paint.h"
#include "protocol.h"
#include "palette.h"
#include "color.h"
#include "misc.h"
#include "operation.h"
#include "ops.h"


static unsigned char backgroundRGB[3] =
{255, 255, 255};
static unsigned char varianceRGB[3] =
{0, 0, 0};

/*
**  Dialog box
 */
typedef struct {
    Widget redBar, greenBar, blueBar;
    Widget redText, greenText, blueText;
    Widget mode;		/* Only used if RANGEBUTTONS is defined */
    Widget shell;		/* Used for XtPopup() */
    Widget cpick;		/* The ColorPicker's Form widget */
    Palette *map;		/* Current palette */
    float r, g, b;
    float vr, vg, vb;
    Pixel pixel;
} DialogInfo;

static DialogInfo *dInfo = NULL;

static void 
closePopup(Widget w, Widget shell)
{
    XtPopdown(shell);
}

static void 
barCB(Widget bar, DialogInfo * l, float *percent)
{
    Widget t;
    char buf[20];

    if (l->redBar == bar) {
	l->vr = *percent;
	t = l->redText;
    } else if (l->greenBar == bar) {
	l->vg = *percent;
	t = l->greenText;
    } else if (l->blueBar == bar) {
	l->vb = *percent;
	t = l->blueText;
    } else {
	return;
    }
    sprintf(buf, "%d", (int) (255 * *percent));
    XtVaSetValues(t, XtNstring, buf, NULL);
}

static void 
textAction(Widget w, XEvent * event, String * prms, Cardinal * nprms)
{
    String val;
    int v;
    char buf[20];

    XtVaGetValues(w, XtNstring, &val, NULL);
    v = atoi(val);
    if (v > 255) {
	v = 255;
	XtVaSetValues(w, XtNstring, "255", NULL);
    }
    if (v < 0) {
	v = 0;
	XtVaSetValues(w, XtNstring, "0", NULL);
    }
    sprintf(buf, "%d", v);

    if (w == dInfo->redText) {
	dInfo->vr = v / 255.0;
	XawScrollbarSetThumb(dInfo->redBar, dInfo->vr, -1.0);
	XtVaSetValues(dInfo->redText, XtNstring, buf, NULL);
    }
    if (w == dInfo->greenText) {
	dInfo->vg = v / 255.0;
	XawScrollbarSetThumb(dInfo->greenBar, dInfo->vg, -1.0);
	XtVaSetValues(dInfo->greenText, XtNstring, buf, NULL);
    }
    if (w == dInfo->blueText) {
	dInfo->vb = v / 255.0;
	XawScrollbarSetThumb(dInfo->blueBar, dInfo->vb, -1.0);
	XtVaSetValues(dInfo->blueText, XtNstring, buf, NULL);
    }
}

/*
 * Add a Form containing a Label, Scrollbar and TextWidget.
 * Return the Form widget, and store the Scrollbar and TextWidgets in 'bar'
 * and 'text'.  'title' is the Label title.
 */
static Widget
addSB(Widget parent, Widget above, char *title, Widget * bar, Widget * text)
{
    Widget form, label;
    static String textTranslations =
    "#override\n\
	<Key>Return: delta-text-ok()\n\
	<Key>Linefeed: delta-text-ok()\n\
	Ctrl<Key>M: delta-text-ok()\n\
	Ctrl<Key>J: delta-text-ok()\n";
    static XtTranslations trans = None;

    if (trans == None) {
	static XtActionsRec act =
	{"delta-text-ok", (XtActionProc) textAction};

	XtAppAddActions(XtWidgetToApplicationContext(parent), &act, 1);

	trans = XtParseTranslationTable(textTranslations);
    }
    form = XtVaCreateManagedWidget("form", formWidgetClass, parent,
				   XtNborderWidth, 0,
				   XtNfromVert, above,
				   NULL);
    label = XtVaCreateManagedWidget("varianceLabel", labelWidgetClass, form,
				    XtNlabel, title,
				    XtNborderWidth, 0,
				    XtNright, XtChainLeft,
				    XtNleft, XtChainLeft,
				    NULL);
    *bar = XtVaCreateManagedWidget("varianceBar", scrollbarWidgetClass, form,
				   XtNorientation, XtorientHorizontal,
				   XtNwidth, 50,
				   XtNfromHoriz, label,
				   XtNleft, XtChainLeft,
				   NULL);
    *text = XtVaCreateManagedWidget("varianceText", asciiTextWidgetClass, form,
				    XtNfromHoriz, *bar,
				    XtNeditType, XawtextEdit,
				    XtNwrap, XawtextWrapNever,
				    XtNresize, XawtextResizeWidth,
				    XtNtranslations, trans,
				    XtNwidth, 50,
				    XtNlength, 5,
				    XtNstring, "0",
				    NULL);

    return form;
}

static void 
deltaSP(Widget form, XtPointer data, XtPointer p)
{
    ((DialogInfo *) data)->pixel = (Pixel) p;
}

/*
 * Callback for the 'Pixel Delta' button.
 * Grab a colour and calculate the delta relative to the background colour.
 */
static void 
deltaCB(Widget w, DialogInfo * l, XtPointer junk)
{
    XColor *xcol, xcol2;
    int dr, dg, db;
    char buf[20];

    xcol = DoGrabColor(w);

    xcol2.pixel = l->pixel;
    XQueryColor(XtDisplay(w), l->map->cmap, &xcol2);

    dr = xcol->red - xcol2.red;
    dg = xcol->green - xcol2.green;
    db = xcol->blue - xcol2.blue;
    if (dr < 0)
	dr = -dr;
    if (dg < 0)
	dg = -dg;
    if (db < 0)
	db = -db;

    l->vr = (float) ((dr >> 8) & 0xff) / 255.0;
    l->vg = (float) ((dg >> 8) & 0xff) / 255.0;
    l->vb = (float) ((db >> 8) & 0xff) / 255.0;

    sprintf(buf, "%d", (int) (255 * l->vr));
    XtVaSetValues(l->redText, XtNstring, buf, NULL);
    XawScrollbarSetThumb(l->redBar, l->vr, -1.0);
    sprintf(buf, "%d", (int) (255 * l->vg));
    XtVaSetValues(l->greenText, XtNstring, buf, NULL);
    XawScrollbarSetThumb(l->greenBar, l->vg, -1.0);
    sprintf(buf, "%d", (int) (255 * l->vb));
    XtVaSetValues(l->blueText, XtNstring, buf, NULL);
    XawScrollbarSetThumb(l->blueBar, l->vb, -1.0);
}

static void 
applyCB(Widget w, DialogInfo * l, XtPointer junk)
{
    XColor xcol;
    int t;

    xcol.pixel = l->pixel;
    xcol.flags = DoRed | DoGreen | DoBlue;
    XQueryColor(XtDisplay(w), l->map->cmap, &xcol);

    backgroundRGB[0] = (xcol.red >> 8) & 0xff;
    backgroundRGB[1] = (xcol.green >> 8) & 0xff;
    backgroundRGB[2] = (xcol.blue >> 8) & 0xff;

#define STUFF(src, dst) \
	t = (src) * 255.0; if (t < 0) t = 0; else if (t > 255) t = 255; dst = t

    STUFF(l->vr, varianceRGB[0]);
    STUFF(l->vg, varianceRGB[1]);
    STUFF(l->vb, varianceRGB[2]);

#undef STUFF

#if RANGEBUTTONS
    t = ((int) XawToggleGetCurrent(l->mode)) - 1;
    OperationSelectCallAcross(t);
    SelectSetCutMode(t);
#endif
}

static void 
okCB(Widget w, DialogInfo * l, XtPointer junk)
{
    applyCB(w, l, junk);
    closePopup(w, GetShell(w));
}

void 
ChromaDialog(Widget w, Palette * map)
{
    Widget topform, form, cpick, ok, cancel, apply;
    Widget above;
    DialogInfo *l = dInfo;
#if RANGEBUTTONS
    Widget label, tog;
    int cutmode = SelectGetCutMode();
#endif

    if (l != NULL && l->shell != None) {
#if RANGEBUTTONS
	XawToggleSetCurrent(l->mode, (XtPointer) (cutmode + 1));
#endif
	/*
	 * If the canvas has another colour map than the ColorPicker,
	 * update the ColorPicker's map (unless it is a TrueColor map)
	 */
	if ((map != l->map) && (l->map->isMapped)) {
	    ColorPickerUpdateMap(l->cpick, map);
	    l->map = map;
	    XtVaSetValues(l->shell, XtNcolormap, map->cmap, NULL);
	}
	XtPopup(l->shell, XtGrabNone);
	XMapRaised(XtDisplay(l->shell), XtWindow(l->shell));
	return;
    }
    dInfo = l = XtNew(DialogInfo);

    l->map = map;

    l->r = (float) backgroundRGB[0] / 255.0;
    l->g = (float) backgroundRGB[1] / 255.0;
    l->b = (float) backgroundRGB[2] / 255.0;
    l->vr = (float) varianceRGB[0] / 255.0;
    l->vg = (float) varianceRGB[1] / 255.0;
    l->vb = (float) varianceRGB[2] / 255.0;

    l->shell = XtVaCreatePopupShell("chroma", topLevelShellWidgetClass,
				    GetToplevel(w),
				    XtNcolormap, map->cmap,
				    NULL);

    topform = XtVaCreateManagedWidget("form", formWidgetClass, l->shell,
				      NULL);

    /*
    **  First create the list of toggle buttons for the mode selection
     */
#if RANGEBUTTONS
    form = XtVaCreateManagedWidget("form", formWidgetClass, topform,
				   XtNborderWidth, 0,
				   NULL);

    label = XtVaCreateManagedWidget("selectModeLabel", labelWidgetClass, form,
				    XtNborderWidth, 0,
				    NULL);

    /*
    **  radioData = mode + 1
     */
    tog = None;
    tog = XtVaCreateManagedWidget("mode0", toggleWidgetClass, form,
				  XtNfromVert, label,
				  XtNradioGroup, tog,
				  XtNradioData, 1,
				  XtNstate, (cutmode == 0),
				  NULL);
    tog = XtVaCreateManagedWidget("mode1", toggleWidgetClass, form,
				  XtNfromVert, tog,
				  XtNradioGroup, tog,
				  XtNradioData, 2,
				  XtNstate, (cutmode == 1),
				  NULL);
    tog = XtVaCreateManagedWidget("mode2", toggleWidgetClass, form,
				  XtNfromVert, tog,
				  XtNradioGroup, tog,
				  XtNradioData, 3,
				  XtNstate, (cutmode == 2),
				  NULL);
    l->mode = tog;
#else
    l->mode = None;
#endif

    /*
    **  Now the color chooser.
     */
    form = XtVaCreateManagedWidget("form", formWidgetClass, topform,
#if RANGEBUTTONS
				   XtNfromHoriz, form,
#endif
				   XtNborderWidth, 0,
				   NULL);

    l->cpick = cpick = ColorPickerPalette(form, map, NULL);
    l->pixel = ColorPickerGetPixel(cpick);

    above = XtVaCreateManagedWidget("delta", commandWidgetClass, form,
				    XtNfromVert, cpick,
				    NULL);
    XtAddCallback(above, XtNcallback, (XtCallbackProc) deltaCB, (XtPointer) l);
    ColorPickerSetFunction(cpick, deltaSP, (XtPointer) l);

    above = addSB(form, above, "Red Variance", &l->redBar, &l->redText);
    above = addSB(form, above, "Green Variance", &l->greenBar, &l->greenText);
    above = addSB(form, above, "Blue Variance", &l->blueBar, &l->blueText);

    XtAddCallback(l->redBar, XtNjumpProc, (XtCallbackProc) barCB, (XtPointer) l);
    XtAddCallback(l->greenBar, XtNjumpProc,
		  (XtCallbackProc) barCB, (XtPointer) l);
    XtAddCallback(l->blueBar, XtNjumpProc,
		  (XtCallbackProc) barCB, (XtPointer) l);

    AddDestroyCallback(l->shell, (DestroyCallbackFunc) closePopup, l->shell);

    ok = XtVaCreateManagedWidget("ok", commandWidgetClass, form,
				 XtNfromVert, above,
				 NULL);
    apply = XtVaCreateManagedWidget("apply", commandWidgetClass, form,
				    XtNfromVert, above,
				    XtNfromHoriz, ok,
				    NULL);
    cancel = XtVaCreateManagedWidget("cancel", commandWidgetClass, form,
				     XtNfromVert, above,
				     XtNfromHoriz, apply,
				     NULL);
    XtAddCallback(cancel, XtNcallback, (XtCallbackProc) closePopup,
		  (XtPointer) l->shell);
    XtAddCallback(apply, XtNcallback, (XtCallbackProc) applyCB, (XtPointer) l);
    XtAddCallback(ok, XtNcallback, (XtCallbackProc) okCB, (XtPointer) l);

    XtPopup(l->shell, XtGrabNone);
}

#if RANGEBUTTONS
void 
ChromaSetCutMode(int value)
{
    if (dInfo != NULL)
	XawToggleSetCurrent(dInfo->mode, (XtPointer) (value + 1));
}
#endif

void 
GetChromaBackground(int *rb, int *gb, int *bb)
{
    *rb = backgroundRGB[0];
    *gb = backgroundRGB[1];
    *bb = backgroundRGB[2];
}

void 
GetChromaDelta(int *rd, int *gd, int *bd)
{
    *rd = varianceRGB[0];
    *gd = varianceRGB[1];
    *bd = varianceRGB[2];
}
