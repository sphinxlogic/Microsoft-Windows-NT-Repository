/* vtksavegeometry.c - Save application geometry state

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains routines that save the application's geometry state.

MODIFICATION HISTORY:

07-Sep-1993 (sjk) VToolkit V2.

03-Mar-1992 (sjk) Use VtkIsWidget macro.

11-Feb-1992 (sjk) Use XtIsWindowObject instead of XtIsWidget so that code will compile under old XUI and MOTIF baselevels.

18-Jun-1991 (sjk) Initial entry.
*/

#include "vtoolkit.h"
#include "vtksavegeometry.h"

void VtkSaveGeometry(Widget w, XrmDatabase *databaseP)
{
    int isShell;
    Widget *childWP;
    int child, childCount;

    if (!VtkIsWidget (w)) return;		/* ignore gadgets */

    isShell = XtIsShell (w);

    if (isShell && VtkIsRealized (w)) {
	VtkSaveNumericResourceN (databaseP, w, XtNx);
        VtkSaveNumericResourceN (databaseP, w, XtNy);
	VtkSaveNumericResourceN (databaseP, w, XtNwidth);
	VtkSaveNumericResourceN (databaseP, w, XtNheight);
    }

    /* Recursively walk the list of popup children. */

    childCount = w->core.num_popups;

    for (child = 0, childWP = w->core.popup_list; child < childCount; child++, childWP++) VtkSaveGeometry (*childWP, databaseP);

    /* Recursively walk the list of composite children. */

    if (isShell) {
	int numChildren = (int)VtkGetOneValue (w, XmNnumChildren);
	Widget *childrenP = (Widget *)VtkGetOneValue (w, XmNchildren);

	for (child = 0, childWP = childrenP; child < numChildren; child++, childWP++)
	    VtkSaveGeometry (*childWP, databaseP);
    }
}
