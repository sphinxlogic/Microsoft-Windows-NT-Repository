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
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Shell.h>
#include "Paint.h"
#include "xpaint.h"
#include "Misc.h"

#if 0
static void updateCallback(Widget w, Widget fat, XRectangle *rect)
{
	PwUpdate(fat, rect, True);
}
static void hbarCallback(Widget w, Widget fat, float *percent)
{
	printf("hbar = %f\n", *percent);
}
static void vbarCallback(Widget w, Widget fat, float *percent)
{
	printf("vbar = %f\n", *percent);
}
#endif
static void doneCallback(Widget w, XtPointer fat, void *junk2)
{
	Widget	paint;

	XtVaGetValues((Widget)fat, XtNpaint, &paint, NULL);

        XtDestroyWidget(GetShell(w));
}

void FatbitsEdit(Widget paint)
{
	Widget		shell, form, fat, viewport;
	Widget		doneButton;
	Colormap	cmap;

        XtVaGetValues(paint, XtNcolormap, &cmap, NULL);
#if 0
	XtVaGetValues(paint, XtNdrawWidth, &width,
			     XtNdrawHeight, &height,
			     NULL);
#endif

        shell = XtVaCreatePopupShell("fatbits", transientShellWidgetClass, 
				GetShell(paint), XtNcolormap, cmap,
                                NULL);
	form  = XtVaCreateManagedWidget("form", formWidgetClass, shell, NULL);
	
	viewport = XtVaCreateManagedWidget("viewport",
				viewportWidgetClass, form,
				XtNallowVert, True,
				XtNallowHoriz, True,
				XtNuseBottom, True,
				XtNuseRight, True,
				NULL);

	fat = XtVaCreateManagedWidget("paint", paintWidgetClass, viewport,
				XtNpaint, paint,
				XtNbottom, XtChainBottom,
				NULL);

	doneButton = XtVaCreateManagedWidget("done", commandWidgetClass, form,
				XtNfromVert, viewport,
				XtNtop, XtChainBottom,
				NULL);

	XtAddCallback(doneButton, XtNcallback, doneCallback, fat);
	AddDestroyCallback(shell, (void (*)(Widget, void *, XEvent *))doneCallback, fat);

        XtPopup(shell, XtGrabNone);

	/*
	**  Set the current operator
	*/
	GraphicAdd(fat);
}
