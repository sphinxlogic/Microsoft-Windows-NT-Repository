/* testvdrag.c - Sample program using the VDrag utility

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module is a sample program that demonstrates the child dragging
capability in the VDrag module.

The user can drag children widgets around by pressing Shift/Ctrl Button3
over the child.

MODIFICATION HISTORY:

07-Sep-1993 (sjk/nfc) Run through MOTIF porting tools, cleaned up and
	functions changed to prototype style definitions.

06-Nov-1991 (sjk) Cleanup (but still doesn't compile under MOTIF).

20-Apr-1990 (sjk) Improve portability.

29-Mar-1990 (sjk) Slight cleanup.

31-May-1989 (sjk) Edited sample program written by Hugh Beyer.
*/

#include <stdlib.h>

#include "vtoolkit.h"
#include "vdrag.h"

#include <Xm/BulletinB.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>

#define DragButton 		Button2
#define DragButtonName		"Use mouse button 2 to move objects", 

static void DoQuit(Widget unused_w, Opaque unused_closure, Opaque unused_reason)
{
    exit (1);
}

static XtCallbackRec DoQuitCB[] = {{DoQuit, 0}, {0, 0}};

main(unsigned int argc, char **argv)
{
    Widget db, shell, w[3];
    Arg al[15];
    int ac;
    XmString cP;

    /* Initialize and create shell widget. */

    shell = XtInitialize ("TestVDrag", "testvdrag$defaults", 0, 0, &argc, argv);

    /* Create dialog box and children. */

    ac = 0;
    XtSetArg(al[ac], XmNdefaultPosition,	1); 			ac++;
    XtSetArg(al[ac], XmNdialogStyle, 		XmDIALOG_WORK_AREA); 	ac++;
    db = XmCreateBulletinBoard(shell, "shell", al, ac);

    ac = 0;
    XtSetArg(al[ac], XmNx, 			10); 			ac++;
    XtSetArg(al[ac], XmNy, 			5); 			ac++;
    w[0] = XmCreateLabel(db, "Drag widgets with MB2", al, ac);

    ac = 0;
    XtSetArg(al[ac], XmNx, 			10); 			ac++;
    XtSetArg(al[ac], XmNy, 			100); 			ac++;
    XtSetArg(al[ac], XmNcolumns, 		20); 			ac++;
    XtSetArg(al[ac], XmNrows, 			1); 			ac++;
    XtSetArg(al[ac], XmNvalue, 			"you can edit here"); 	ac++;
    w[1] = XmCreateText(db, "stext", al, ac);

    cP = XmStringCreateLtoR ("Push to quit", XmSTRING_DEFAULT_CHARSET);
    ac = 0;
    XtSetArg(al[ac], XmNx, 			10); 			ac++;
    XtSetArg(al[ac], XmNy, 			200); 			ac++;
    XtSetArg(al[ac], XmNlabelString,		cP);			ac++;
    XtSetArg(al[ac], XmNactivateCallback, 	DoQuitCB); 		ac++;
    w[2] = XmCreatePushButton(db, "pushbutton", al, ac);
/*    XmStringFree (cP);	*/

    XtManageChildren (w, 3);
    XtManageChild (db);
    XtRealizeWidget (shell);

    /* Enable child dragging on the dialog box. */

    VDragEnableGrab (db, DragButton, AnyModifier);

    /* Loop, processing events.  Never returns. */

    XtMainLoop ();
}
