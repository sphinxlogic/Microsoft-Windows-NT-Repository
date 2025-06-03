/* simpletestvlist.c - Simplified sample program using the VList widget

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This program is a simplified example of how to use the VList widget.
It shows how to set up two lists and callbacks that allow the user
to move lines from one list to the other by clicking or dragging.

MODIFICATION HISTORY:

23-May-1994 (sjk) Bugfix - call XmStringFree instead of XtFree.  Thanks Bill!

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

28-Aug-1993 (sjk/nfc) Run through the Motif Porting tools.

12-Nov-1991 (sjk) Add drag capability and multi-select.  Use new VtkFetchWidget convenience routine.

31-Aug-1990 (sjk) Initial entry.
*/

#include <stdio.h>
#include <stdlib.h>
#include "vtoolkit.h"
#include "vlist.h"
#include "vframe.h"

#define appName		"SimpleTestVList"
#define appClass	"simpletestvlist_defaults"
#define uidFile 	"simpletestvlist.uid"

static MrmHierarchy mrmHierarchy;

static Widget widgetA[32];
#define leftVListW	widgetA[0]
#define rightVListW	widgetA[1]
#define dragLabelW	widgetA[2]
#define appW		widgetA[3]
#define mainW		widgetA[4]
#define dragFrameW	widgetA[5]

static void MoveOneLine(Widget fromW, Widget toW, Opaque closure, int toRow)
{
    VListLine vLine;

    VListGetLineInfo (fromW, closure, &vLine, 0, 0);		/* get information about line being moved */
    vLine.textP = XtNewString (vLine.textP);			/* make a copy of the line's text string */

    VListRemoveLine (fromW, closure);				/* remove the line from the list it was in */

    vLine.mask = VListLineClosureMask | VListLineTextMask;	/* use that information to add a corresponding line ... */
    VListAddLine (toW, &vLine, toRow);				/* ... to the other list ... */
    VListMakeLineVisible (toW, closure, VListVisibleInWindow);	/* ... and make it visible */

    XtFree (vLine.textP);					/* free our copy of the line's text string */
}

static void SelectCallback(Widget w, Opaque unused_closure, VListSelectCallbackStruct *unused_cbDataP)
{
    int selectionCount, i;
    Opaque *lineClosuresP;
    Widget toW;

    /* The User clicked over a VList widget.  The Widget w is the VList widget from which entry is being moved. */

    selectionCount = VListGetAllSelectedLines (w, &lineClosuresP);				/* get list of selected lines */

    if (w == leftVListW) toW = rightVListW;
    else                 toW = leftVListW;

    for (i = 0; i < selectionCount; i++) MoveOneLine (w, toW, lineClosuresP[i], VListAddAtBottom);

    XtFree ((char *)lineClosuresP);								/* free the list */
}

static void DragArmCallback(Widget w, Opaque unused_closure, VListDragArmCallbackStruct *cbDataP)
{
    XmString xValueP;

    if (!cbDataP->vLine.closure) {			/* click over deadspace? */
        XtVaSetValues (w, VListNdragWidget, 0, 0);	/* disable drag widget */
        return;
    }

    /* Set the label in the drag widget. */

    xValueP = XmStringCreate (cbDataP->vLine.textP, XmSTRING_DEFAULT_CHARSET);
    XtVaSetValues (dragLabelW, XmNlabelString, xValueP, 0);
    XmStringFree (xValueP);

    XtVaSetValues (w, VListNdragWidget, dragFrameW, 0);	/* enable drag widget */
}

static void DragDropCallback(Widget w, Opaque unused_closure, VListDragDropCallbackStruct *cbDataP)
{
    if (!cbDataP->dropWidgetIsVList) return;						/* not dropped over a VList widget */

    if (!cbDataP->dropVLine.closure) cbDataP->dropRow = VListAddAtBottom;		/* dropped over deadspace */

    MoveOneLine (w, cbDataP->dropWidget, cbDataP->vLine.closure, cbDataP->dropRow);	/* move the line */
}

static void LoadLeftVList(void)
{
    VListLine vLine;
    int i;
    char bufA[64];

    /* Load the left VList with a bunch of lines to get started. */

    for (i = 1; i <= 25; i++) {
	sprintf (bufA, "Line %d", i);
	vLine.mask = VListLineTextMask;
	vLine.textP = bufA;
	VListAddLine (leftVListW, &vLine, VListAddAtBottom);
    }
}

static void QuitCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    exit (1);
}

static MrmRegisterArg mrmRegisterArgs[] = {
    {"DragArmCallback",			(char *)DragArmCallback},
    {"DragDropCallback",		(char *)DragDropCallback},
    {"QuitCallback",			(char *)QuitCallback},
    {"SelectCallback",			(char *)SelectCallback},
    };

int main(int argc, char **argv)
{
    MrmInitialize ();			/* register toolkit widgets */
    VListInitializeForMRM ();		/* register VList widget */
    VFrameInitializeForMRM ();		/* register VFrame widget */

    /* Initialize the application. */

    if (!VtkInitialize (appName, appClass, &argc, argv, uidFile, mrmRegisterArgs, XtNumber (mrmRegisterArgs), &mrmHierarchy, &appW))
	exit (0);

    /* Load the main window and its children. */

    (void)VtkFetchWidget (mrmHierarchy, "main", appW, widgetA, 0);
    XtManageChild (mainW);

    LoadLeftVList ();			/* load the left VList widget */

    /* Work around a (DECwindows V2) toolkit bug to position application shell based on main window's position. */

    XtVaSetValues (XtParent (mainW),
	XtNx, mainW->core.x,
	XtNy, mainW->core.y,
	0);

    XtRealizeWidget (appW);		/* realize widget tree */
    XtMainLoop ();			/* Handle events.  Never returns. */
}
