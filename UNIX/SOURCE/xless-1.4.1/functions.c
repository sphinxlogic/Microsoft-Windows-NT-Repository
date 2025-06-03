/*
 * Author: Dave Glowacki
 *	   UCB Software Warehouse
 *
 * $Header: /usr/sww/src/X11R5/local/clients/xless-1.4/RCS/functions.c,v 1.8 1993/02/26 01:14:30 dglo Exp $
 */

#include "xless.h"

static void
okQuitAll(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  XtPopdown((Widget)closure);
  XtDestroyWidget((Widget)closure);
  DestroyAllWindows();
  exit(0);
}

static void
cancelQuitAll(widget, closure, callData)
Widget widget;
XtPointer closure;
XtPointer callData;
{
  XtPopdown((Widget)closure);
}

static int
QuitAllPrompt()
{
  Widget toplevel;
  static Widget quitall = 0;

  /* make sure there's at least one window */
  if (!windowlist)
    return(1);

  /* grab first base widget */
  toplevel = windowlist->base;

  /* popup warning box */
  if (!quitall)
    quitall = MessageBox(toplevel, "Quit ALL windows?",
		       "Cancel", cancelQuitAll, "OK", okQuitAll, 0);
  SetPopup(toplevel, quitall);
  return(0);
}

/*
 * clean up, exit application
 */
void
QuitFunction()
{
  if ((windowcount == 1) || QuitAllPrompt()) {
    DestroyAllWindows();
    exit(0);
  }
}
