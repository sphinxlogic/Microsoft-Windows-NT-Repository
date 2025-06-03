/*
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Carlo Lisa
 *	   MIT Project Athena
 *	   carlo@athena.mit.edu
 *
 * $Header: /usr/sww/share/src/X11R5/local/clients/xless-1.4/RCS/popup.c,v 1.14 1993/02/26 01:12:17 dglo Exp dglo $
 */

#include "xless.h"

/* stole this chunk from xdm/dm.c */
#if defined(USE_PROTOTYPES) || defined(__STDC__)
#include <stdarg.h>
#define Va_start(a,b)	va_start(a,b)
#else
#include <varargs.h>
#define Va_start(a,b)	va_start(a)
#endif

/*
 *	Function Name: DialogBox
 *	Description:   This function creates a single dialog box.
 *	Arguments:  top		parent widget;
 *		    conf_func	confirm function (callback);
 *		    c_data	callback data (for conf_func);
 *		    label	label for the dialog box;
 *		    selection 	user's input (back).
 *	Returns:    popup	the dialogbox widget id.
 */

Widget
DialogBox(top, conf_func, c_data, label, conflab, selection)
Widget top;
XtCallbackProc conf_func;
XtPointer c_data;
const char *label;
const char *conflab;
char *selection;
{
  static XtActionsRec dbactions[2];
  static String myreturn = "#override <Key>Return:   set() notify() unset()\n";
  XtAccelerators accel;
  static int init = 0;
  Widget popup, form, lbl, input, confirm, cancel;
  Arg args[15];
  Cardinal i;
  XtCallbackRec callback[2];

  dbactions[0].string = "Nothing";
  dbactions[0].proc = (XtActionProc)NULL;

  if (!init) {
    XtAddActions(dbactions, XtNumber(dbactions));
    init = 1;
  }

  accel = XtParseAcceleratorTable(myreturn);

  callback[1].callback = NULL;
  callback[1].closure = (XtPointer)NULL;

  i = 0;
  XtSetArg(args[0], XtNallowShellResize, True); i++;
  popup = XtCreatePopupShell("popup", transientShellWidgetClass, top, args, i);

  i = 0;
  form = XtCreateManagedWidget("form", formWidgetClass, popup, args, i);

  i = 0;
  XtSetArg(args[i], XtNjustify, XtJustifyLeft); i++;
  XtSetArg(args[i], XtNborderWidth, 0); i++;
  XtSetArg(args[i], XtNlabel, label); i++;
  XtSetArg(args[i], XtNfont, labelfont); i++;
  XtSetArg(args[i], XtNright, XtChainLeft); i++;
  XtSetArg(args[i], XtNleft, XtChainLeft); i++;
  lbl = XtCreateManagedWidget("label", labelWidgetClass, form, args, i);
  i = 0;
  XtSetArg(args[i], XtNfont, textfont); i++;
  XtSetArg(args[i], XtNeditType, XawtextEdit); i++;
  XtSetArg(args[i], XtNstring, selection); i++;
  XtSetArg(args[i], XtNlength, MAX_INPUT); i++;
  XtSetArg(args[i], XtNwidth, DEF_INPUT_W); i++;
  XtSetArg(args[i], XtNresizable, True); i++;
#ifdef X11R4
  XtSetArg(args[i], XtNtextOptions, resizeWidth); i++;
#endif
  XtSetArg(args[i], XtNvertDistance, 0); i++;
  XtSetArg(args[i], XtNfromVert, lbl); i++;
  XtSetArg(args[i], XtNfromHoriz, NULL); i++;
  XtSetArg(args[i], XtNright, XtChainLeft); i++;
  XtSetArg(args[i], XtNleft, XtChainLeft); i++;
  XtSetArg(args[i], XtNuseStringInPlace, True); i++;
  input = XtCreateManagedWidget("input", asciiTextWidgetClass, form, args, i);
  XtSetKeyboardFocus(form, input);

  i = 0;
  XtSetArg(args[i], XtNfont, buttonfont); i++;
  XtSetArg(args[i], XtNcursor, dialogcur); i++;
  XtSetArg(args[i], XtNwidth, BUTTONWIDTH); i++;
  XtSetArg(args[i], XtNvertDistance, 5); i++;
  XtSetArg(args[i], XtNfromVert, input); i++;
  XtSetArg(args[i], XtNfromHoriz, NULL); i++;
  XtSetArg(args[i], XtNright, XtChainLeft); i++;
  XtSetArg(args[i], XtNleft, XtChainLeft); i++;
  callback[0].callback = conf_func;
  callback[0].closure = c_data;
  XtSetArg(args[i], XtNcallback, callback); i++;
  XtSetArg(args[i], XtNaccelerators, accel); i++;
  confirm = XtCreateManagedWidget(conflab, commandWidgetClass, form, args, i);

  i = 0;
  XtSetArg(args[i], XtNfont, buttonfont); i++;
  XtSetArg(args[i], XtNcursor, dialogcur); i++;
  XtSetArg(args[i], XtNwidth, BUTTONWIDTH); i++;
  XtSetArg(args[i], XtNvertDistance, 5); i++;
  XtSetArg(args[i], XtNhorizDistance, 2); i++;
  XtSetArg(args[i], XtNfromVert, input); i++;
  XtSetArg(args[i], XtNfromHoriz, confirm); i++;
  XtSetArg(args[i], XtNright, XtChainLeft); i++;
  XtSetArg(args[i], XtNleft, XtChainLeft); i++;
  callback[0].callback = Cancel;
  callback[0].closure = (XtPointer)popup;
  XtSetArg(args[i], XtNcallback, callback); i++;
  cancel = XtCreateManagedWidget("Cancel", commandWidgetClass, form,
				 args, i);

  XtInstallAccelerators(input, confirm);

  return(popup);
}

static void
popdownBox(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  XtPopdown((Widget)closure);
}

/*
 *	Function Name:	MessageBox
 *	Description:	This function creates the message dialog box.
 *	Arguments:  top		widget to use as message box parent
 *		    msg		message to display
 *		    btnlabel	label on next button
 *		    btnproc	callback executed when next button is pressed
 *	Returns:    widget	id of the message box.
 */

Widget
#if defined(USE_PROTOTYPES) || defined(__STDC__)
MessageBox(Widget top, const char *msg, ...)
#else
MessageBox(top, msg, va_alist)
Widget *top;
const char *msg;
va_dcl
#endif
{
  extern XtAppContext context;
  Widget msg_pop, form, msg_label, button;
  Arg args[15];
  Cardinal i;
  XtCallbackRec callback[2];
  static String myreturn = "#override <Key>Return: set() notify() unset()\n";
  XtAccelerators accel;
  const char *btntext;
  XtCallbackProc btnproc;
  va_list vap;

  /* can't popup a messagebox if widgets haven't been realized yet... */
  if (!XtIsRealized(top)) {
    XtAppWarning(context, msg);
    return 0;
  }

  accel = XtParseAcceleratorTable(myreturn);

  callback[1].callback = (XtCallbackProc) NULL;
  callback[1].closure = (XtPointer) NULL;

  i = 0;
  XtSetArg(args[0], XtNallowShellResize, True); i++;
  msg_pop = XtCreatePopupShell("msg_pop", transientShellWidgetClass, top,
			       args, i);

  i = 0;
  XtSetArg(args[0], XtNallowShellResize, True); i++;
  form = XtCreateManagedWidget("form", formWidgetClass, msg_pop, args, i);

  i = 0;
  XtSetArg(args[0], XtNallowShellResize, True); i++;
  XtSetArg(args[i], XtNjustify, XtJustifyLeft); i++;
  XtSetArg(args[i], XtNborderWidth, 0); i++;
  XtSetArg(args[i], XtNlabel, msg); i++;
  XtSetArg(args[i], XtNfont, labelfont); i++;
  msg_label = XtCreateManagedWidget("message", labelWidgetClass, form,
				    args, i);

  Va_start(vap, msg);
  btntext = (const char *)va_arg(vap, const char *);

  button = 0;
  while (btntext) {
    btnproc = (XtCallbackProc )va_arg(vap, XtCallbackProc);

    i = 0;
    XtSetArg(args[i], XtNfont, buttonfont); i++;
    XtSetArg(args[i], XtNcursor, dialogcur); i++;
    XtSetArg(args[i], XtNwidth, BUTTONWIDTH); i++;
    XtSetArg(args[i], XtNvertDistance, 1); i++;
    XtSetArg(args[i], XtNfromVert, msg_label); i++;
    XtSetArg(args[i], XtNfromHoriz, button); i++;
    XtSetArg(args[i], XtNright, XtChainRight); i++;
    XtSetArg(args[i], XtNleft, XtChainRight); i++;
    if (btnproc)
      callback[0].callback = btnproc;
    else
      callback[0].callback = popdownBox;
    callback[0].closure = (XtPointer)msg_pop;
    XtSetArg(args[i], XtNcallback, callback); i++;
    XtSetArg(args[i], XtNaccelerators, accel); i++;
    button = XtCreateManagedWidget(btntext, commandWidgetClass, form, args, i);

    XtInstallAccelerators(form, button);

    btntext = (const char *)va_arg(vap, const char *);
  }

  return(msg_pop);
}
