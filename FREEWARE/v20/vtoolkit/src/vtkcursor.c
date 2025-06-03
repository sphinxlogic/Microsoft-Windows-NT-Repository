/* vtkcursor.c - Set and unset application cursor

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains routines that set and unset an application-wide cursor.
The application's widget tree is walked recursively and the specified cursor
is defined for each of the realized shell widgets found.

The application is responsible for creating (loading) the cursor.  For example,
this creates the X standard watch cursor:

	...
	#include <X11/cursorfont.h>
	...
	Cursor busyCursor;
	...
	busyCursor = XCreateFontCursor (appInfoP->dpy, XC_watch);
	...

To set the application cursor prior to executing an action:

	VtkDefineCursor (appW, busyCursor, 1);

To reset it after completion:

	VtkDefineCursor (appW, None, 0);

If the application uses multiple top level application shell widgets,
VtkDefineCursor must be called separately for each of them.

MODIFICATION HISTORY:

03-Sep-1993 (sjk/nfc) VToolkit V2.

03-Mar-1992 (sjk) Use VtkIsWidget macro.

11-Feb-1992 (sjk) Use XtIsWindowObject instead of XtIsWidget so that code will compile under old XUI and MOTIF baselevels.

18-Jun-1991 (sjk) Initial entry.
*/

#include "vtoolkit.h"
#include "vtkcursor.h"

void VtkDefineCursor(Widget w, Cursor cursor, int flush)
{
    Widget *childWP;
    int child, childCount;

    if (!VtkIsWidget (w)) return;					/* ignore gadgets */

    /* If this is a realized shell, set the cursor. */

    if (XtIsShell (w) && VtkIsRealized (w)) XDefineCursor (XtDisplay (w), XtWindow (w), cursor);

    /* Recursively walk the list of popup children. */

    childCount = w->core.num_popups;

    for (child = 0, childWP = w->core.popup_list; child < childCount; child++, childWP++) VtkDefineCursor (*childWP, cursor, 0);

    /* Recursively walk the list of composite children. */

    if (XtIsComposite (w)) {
	int numChildren = (int)VtkGetOneValue (w, XmNnumChildren);
	Widget *childrenP = (Widget *)VtkGetOneValue (w, XmNchildren);

	for (child = 0, childWP = childrenP; child < numChildren; child++, childWP++)
	    VtkDefineCursor (*childWP, cursor, 0);
    }

    if (flush) XFlush (XtDisplay (w));
}
