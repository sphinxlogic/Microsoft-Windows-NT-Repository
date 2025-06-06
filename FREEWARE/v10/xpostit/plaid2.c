#ifndef lint
static char	*RCSid = "$Header$";
#endif

/*
 * plaid.c - routines for manipulating the plaid widget.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94086
 * davy@riacs.edu
 *
 * $Log$
 */

/* Modified by:
            Jatin Desai (VIA::DESAI) , Core Applications Group
	    110 Spitbrook Road, Nashua, NH 03060
	    Digital Equipment Corporation

   to work with VMS DECwindows environment. Look for ifndef VMS and
   ifdef VMS to see changes.
*/

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <stdio.h>

#include "xpostit.h"
#include "Plaid.h"

Widget		plaidwidget;

static void	HandlePlaidButton();

/*
 * CreatePlaidWidget - create the plaid widget.
 */
void
CreatePlaidWidget()
{
	Arg args[4];
	register int nargs;
	XtCallbackRec callbacks[2];

	/*
	 * Set the callback.
	 */
	bzero(callbacks, sizeof(callbacks));
	SetCallback(HandlePlaidButton, NULL);

	/*
	 * Create the plaid widget.
	 */
	nargs = 0;
	SetArg(XtNcallback, callbacks);
	plaidwidget = XtCreateWidget("plaid", plaidWidgetClass, toplevel,
			args, nargs);

	/*
	 * Get the width and height of the widget.
	 */
	nargs = 0;
	SetArg(XtNwidth, NULL);
	SetArg(XtNheight, NULL);
	XtGetValues(plaidwidget, args, nargs);

	/*
	 * If the user didn't set them, then we
	 * should set them to the defaults.
	 */
	if ((args[0].value == 0) || (args[1].value == 0)) {
		if (args[0].value == 0)
			XtSetArg(args[0], XtNwidth, DefaultPlaidWidth);

		if (args[1].value == 0)
			XtSetArg(args[1], XtNheight, DefaultPlaidHeight);

		XtSetValues(plaidwidget, args, nargs);
	}

	/*
	 * Inform the application shell we're here.
	 */
	XtManageChild(plaidwidget);
}

/*
 * HandlePlaidButton - callback for the plaid widget to handle buttons.
 */
static void
HandlePlaidButton(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	Arg args[4];
	register int nargs;
	register XEvent *event;
	register Position x, y;

	/*
	 * Get the event.
	 */
	event = (XEvent *) call_data;

	/*
	 * We're only interested in button presses.
	 */
	if (event->type != ButtonPress)
		return;

	/*
	 * Dispatch a function for this button.
	 */
	switch (event->xbutton.button) {
	case Button1:		/* left */
		RaiseAllNotes();
		break;
#ifndef VMS
	case Button2:		/* Right */
		LowerAllNotes();
		break;

	case Button3:		/* right */
		/*
		 * Get position of mouse so we can put
		 * the menu there.
		 */
		x = event->xbutton.x_root;
		y = event->xbutton.y_root;

		/*
		 * Move the popup to that location.
		 */
		nargs = 0;
		SetArg(XtNx, x);
		SetArg(XtNy, y);
		XtSetValues(menuwidget, args, nargs);

		/*
		 * Bring up the menu.
		 */
		XtPopup(menuwidget, XtGrabExclusive);
		break;
#else
	case Button3:		/* Right */
		LowerAllNotes();
		break;

	case Button2:		/* Middle */
		/*
		 * Get position of mouse so we can put
		 * the menu there.
		 */
		DwtMenuPosition( menuwidget, event);
		XtManageChild(menuwidget);
		break;
#endif

	default:
		break;
	}
}
