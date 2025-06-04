/* +-------------------------------------------------------------------+ */
/* | Copyright 1993, David Koblas (koblas@netcom.com)		       | */
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

/* $Id: dialog.c,v 1.3 1996/04/19 09:07:06 torsten Exp $ */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#ifndef VMS
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#else
#include <X11Xaw/Command.h>
#include <X11Xaw/Toggle.h>
#include <X11Xaw/Form.h>
#include <X11Xaw/Label.h>
#endif
#include <X11/StringDefs.h>
#include <stdio.h>
#ifdef MISSING_STDARG_H
#include <varargs.h>
#else
#include <stdarg.h>
#endif
#include "misc.h"
#include "xpaint.h"
#include "protocol.h"


/*
**  One standard generic dialog alert box.
 */
typedef struct {
    XtCallbackProc okFunc, cancelFunc;
    void *data;
    Widget parent, shell;
} arg_t;


static void 
commonCallback(XtCallbackProc f, arg_t * arg)
{
    void *d = arg->data;
    Widget p = arg->parent;

    XtDestroyWidget(arg->shell);
    XtFree((XtPointer) arg);

    if (f != NULL)
	f(p, d, NULL);
}
static void 
cancelCallback(Widget w, XtPointer argArg, XtPointer junk2)
{
    arg_t *arg = (arg_t *) argArg;

    commonCallback(arg->cancelFunc, arg);
}

static void 
okCallback(Widget w, XtPointer argArg, XtPointer junk2)
{
    arg_t *arg = (arg_t *) argArg;

    commonCallback(arg->okFunc, arg);
}

void 
AlertBox(Widget parent, char *msg, XtCallbackProc okProc,
	 XtCallbackProc nokProc, void *data)
{
    Position x, y;
    Widget shell;
    Widget form, title, okButton = None, cancelButton = None;
    arg_t *arg = XtNew(arg_t);

    XtVaGetValues(GetShell(parent), XtNx, &x, XtNy, &y, NULL);

    shell = XtVaCreatePopupShell("alert",
			     transientShellWidgetClass, GetShell(parent),
				 XtNx, x,
				 XtNy, y,
				 NULL);
    form = XtVaCreateManagedWidget(NULL,
				   formWidgetClass, shell,
				   XtNborderWidth, 0,
				   NULL);

    title = XtVaCreateManagedWidget("title",
				    labelWidgetClass, form,
				    XtNlabel, msg,
				    XtNborderWidth, 0,
				    NULL);

    arg->shell = shell;
    arg->okFunc = okProc;
    arg->cancelFunc = nokProc;
    arg->parent = parent;
    arg->data = data;

    okButton = XtVaCreateManagedWidget("ok",
				       commandWidgetClass, form,
				       XtNfromVert, title,
				       XtNlabel, "OK",
    /* XtNaccelerators, accel, */
				       NULL);
    XtAddCallback(okButton, XtNcallback, okCallback, (XtPointer) arg);

    if (nokProc != NULL) {
	cancelButton = XtVaCreateManagedWidget("cancel",
					       commandWidgetClass, form,
					       XtNfromVert, title,
					       XtNfromHoriz, okButton,
					       XtNlabel, "Cancel",
					       NULL);
	XtAddCallback(cancelButton, XtNcallback, cancelCallback, (XtPointer) arg);
    }
    AddDestroyCallback(shell, (DestroyCallbackFunc) cancelCallback,
		       NULL);
    XtPopup(shell, XtGrabExclusive);
}

#ifdef MISSING_STDARG_H
void 
Notice(va_alist)
va_dcl
{
    static char msg[512];
    va_list ap;
    char *fmt;
    Widget w;

    va_start(ap);
    w = va_arg(ap, Widget);
    fmt = va_arg(ap, char *);
    vsprintf(msg, fmt, ap);

    AlertBox(GetShell(w), msg, NULL, NULL, NULL);
}

#else
void 
Notice(Widget w,...)
{
    static char msg[512];
    va_list ap;
    char *fmt;

    va_start(ap, w);
    fmt = va_arg(ap, char *);
    vsprintf(msg, fmt, ap);

    AlertBox(GetShell(w), msg, NULL, NULL, NULL);
}
#endif
