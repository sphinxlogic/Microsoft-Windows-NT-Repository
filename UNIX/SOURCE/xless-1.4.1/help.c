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
 *
 * $Header: /usr/sww/share/src/X11R5/local/clients/xless-1.4/RCS/help.c,v 1.11 1993/02/26 02:09:54 dglo Exp dglo $
 */

#include "xless.h"
#include "XLessHelp.icon"

static Widget createHelp ARGS((void));
static void PopdownHelp ARGS((Widget, XtPointer, XtPointer));

/*	Function Name: CreateHelp.
 *	Description: This function creates the help widget so that it will be
 *	             ready to be displayed.
 *	Arguments:
 *	Returns: the help widget
 */

static Widget
createHelp()
{
  extern Widget toplevel;
  FILE *help_file;
  Cardinal i;
  Arg args[4];
  const char *help_page;
  Pixmap helpicon;
  Widget base, pane, helptext, button;
  XtCallbackRec callback[2];
  XtAccelerators accel;
  const String quitstr = "#override <Key>Q: set() notify() unset()\n";

  /* try to open the help file */
  if ((help_file = fopen(resources.help_file, "r")) == NULL) {
    CouldntOpen(toplevel, resources.help_file);
    return 0;
  }
  help_page = InitData(help_file);

  /* create a new application shell */
  i = 0;
  helpicon = XCreateBitmapFromData(disp, XRootWindow(disp, 0), XLessHelp_bits,
				   XLessHelp_width, XLessHelp_height);
  XtSetArg(args[i], XtNiconPixmap, helpicon); i++;
  XtSetArg(args[i], XtNiconName, "xless: help"); i++;
  XtSetArg(args[i], XtNallowShellResize, TRUE); i++;
  base = XtAppCreateShell("help", XLESS_CLASS, applicationShellWidgetClass,
			  disp, args, i);

  pane = XtCreateManagedWidget("help_pane", panedWidgetClass, base, NULL, 0);

  helptext = MakeText(pane, help_page);

  /* set up Done button callback array */
  callback[0].callback = PopdownHelp;
  callback[0].closure = (XtPointer)base;
  callback[1].callback = NULL;
  callback[1].closure = (XtPointer)NULL;

  /* set up Done button accelerator */
  accel = XtParseAcceleratorTable(quitstr);

  /* create Done button */
  i = 0;
  XtSetArg(args[i], XtNfont, buttonfont); i++;
  XtSetArg(args[i], XtNlabel, "Done With Help"); i++;
  XtSetArg(args[i], XtNcallback, callback); i++;
  XtSetArg(args[i], XtNaccelerators, accel); i++;
  button = XtCreateManagedWidget("help_quit", commandWidgetClass, pane,
			       args, i);
  XtInstallAccelerators(helptext, button);

  XtRealizeWidget(base);

  return(base);
}

/*	Function Name: PopdownHelp
 *	Description: This function pops down the help widget.
 *	Arguments: w - the widget we are calling back from.
 *	           number - (closure) the number to switch on.
 *	           junk - (call data) not used.
 *	Returns: none.
 */

/* ARGSUSED */

static void
PopdownHelp(w, help_widget, junk)
Widget w;
XtPointer help_widget, junk;
{
  XtPopdown((Widget )help_widget);
}

/*	Function Name: PopupHelp
 *	Description: This function pops up the help widget, unless no
 *	             help could be found.
 *	Arguments: w - the widget we are calling back from.
 *	           number - (closure) the number to switch on.
 *	           junk - (call data) not used.
 *	Returns: none.
 */

/* ARGSUSED */

void
PopupHelp(w, number, junk)
Widget w;
XtPointer number, junk;
{
  static Widget help_widget = 0;

  if (!help_widget)
    help_widget = createHelp();
  if (help_widget)
    XtPopup(help_widget, XtGrabNone);
}
