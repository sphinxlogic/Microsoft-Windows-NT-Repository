/* This program was adopted from David A Curry. It has gone through major
   changes in code, functionality and appearance, and may not resemble the 
   original code. Folloowing is the comment from the original program.
*/


/*
 * confirm.c - handle confirming requests made by the user.
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

/* Modified by:
            Jatin Desai (VIA::DESAI) , Core Applications Group
	    110 Spitbrook Road, Nashua, NH 03060
	    Digital Equipment Corporation

   To remove Athena widget set code. This version now only works with
   DECwindows toolkit widgets. It should work on both VMS and Ultrix
   although I haven't tried to even compile on Ultrix. You can no
   longer search for ifdef VMS to look for changes from original
   version.

*/

#include <decw$include/DwtWidget>
#include <stdio.h>
#include "xpostit.h"

static Widget	confirmwidget;

/*
 * ConfirmIt - put up a window asking for confirmation.
 */
void
ConfirmIt(msg, confirm_callbacks, cancel_callbacks)
char	* msg;
XtCallbackRec *confirm_callbacks, *cancel_callbacks;
{
	Arg args[20];
	Window root, child;
	register int nargs;
	static Boolean inited = False;
	int root_x, root_y, child_x, child_y, buttons;
	DwtCompString	msg_label = DwtLatin1String(msg);

	/*
	 * Find out where the mouse is, so we can put the confirmation
	 * box right there.
	 */
	XQueryPointer(display, XtWindow(toplevel), &root, &child,
			&root_x, &root_y, &child_x, &child_y, &buttons);

	root_x -= 50;
	root_y -= 70;

	if (root_x < 0) root_x = 0;
	if (root_y < 0) root_y = 0;

	/*
	 * If we need to construct the confirmation box do that,
	 * otherwise just reset the position and callbacks and
	 * put it up again.
	 */
	if (!inited) {
		DwtCompString	yes_label = DwtLatin1String("Confirm");
		DwtCompString	no_label = DwtLatin1String("");
		DwtCompString	cancel_label = DwtLatin1String("Cancel");

		nargs = 0;
		SetArg(XtNx, root_x);
		SetArg(XtNy, root_y);
		SetArg(DwtNlabel, msg_label);
		SetArg(DwtNyesLabel, yes_label);
		SetArg(DwtNnoLabel, no_label);
		SetArg(DwtNcancelLabel, cancel_label);
		SetArg(DwtNyesCallback, confirm_callbacks);
		SetArg(DwtNcancelCallback, cancel_callbacks);
		SetArg(DwtNdefaultPosition, False);
		SetArg(DwtNstyle, DwtModeless);
		confirmwidget = DwtCautionBoxCreate(toplevel,"Confirm",
				    args, nargs); 

		XtFree(yes_label);
		XtFree(no_label);
		XtFree(cancel_label);

		inited = True;
	}
	else {

		/*
		 * Reset the callbacks.
		 */
		nargs = 0;
		SetArg(DwtNyesCallback, confirm_callbacks);
		SetArg(DwtNcancelCallback, cancel_callbacks);
		SetArg(DwtNlabel, msg_label);
		XtSetValues(confirmwidget, args, nargs);

		if (XtIsManaged(confirmwidget))
		    XtUnmanageChild(confirmwidget);

		/*
		 * Reset the confirmation box position.
		 */
		nargs = 0;
		SetArg(XtNx, root_x);
		SetArg(XtNy, root_y);
		XtSetValues(XtParent(confirmwidget), args, nargs);
#ifdef NODEF
		XtMoveWidget( XtParent(confirmwidget),root_x, root_y);
#endif
	}

	XtFree(msg_label);

	/*
	 * Pop up the confirmation box.
	 */
	XtManageChild(confirmwidget);
}

/*
 * ClearConfirm - get rid of the confirmation box.
 */
void
ClearConfirm()
{
	XtUnmanageChild(confirmwidget);
}
