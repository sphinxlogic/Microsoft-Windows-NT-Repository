/* vdragroot.c - VDragRoot sample program

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This program is an example of how to use the VFake widget.  When run, it
enables the VDrag option on the root window.  This allows you to rearrange
windows on the screen by pressing shift-control-MB3 (MB2 for MOTIF) and dragging.

You can escape from VDragRoot by dragging the mouse to the upper left-hand
corner of the screen and clicking shift-control-MB3 (MB2 for MOTIF).

MODIFICATION HISTORY:

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

07-Sep-1993 (sjk/nfc) Run through the MOTIF porting tools, cleaned up and
	functions changed to prototype definitions.

14-Nov-1991 (sjk) Set VFakeNwindowType to VFakeWindowTypeRoot instead of passing root window ID explicitly.

06-Nov-1991 (sjk) Add escape mechanism at zero-zero.

20-Apr-1990 (sjk) More portability work.

05-Apr-1990 (sjk) Add MOTIF support.

21-Feb-1990 (sjk) Initial public release.
*/

#include <stdlib.h>
#include "vtoolkit.h"
#include "vfake.h"
#include "vdrag.h"

#define DragButton 		Button2
#define DragMask		(ShiftMask | ControlMask)

static void ButtonPressHandler(Widget unused_w, XtPointer unused_closure, XEvent *eventOP, Boolean *unused_continueP)
{
    XButtonEvent *eventP = (XButtonEvent *)eventOP;

    if (eventP->button != DragButton) return;

    if ((!eventP->x) && (!eventP->y)) exit (0);		/* quit when user clicks in upper left-hand corner of screen */
}

int main(int argc, char **argv)
{
    Widget shellW, fakeW;

    shellW = XtInitialize ("VDragRoot", "vdragroot_defaults", 0, 0, &argc, argv);

    /* Create a VFake widget, tell it to use the root window of the screen, and realize it. */

    {Arg args[1];
    XtSetArg (args[0], VFakeNwindowType, VFakeWindowTypeRoot);
    fakeW = (Widget)VFakeCreate (shellW, "vFake", args, XtNumber (args));}
    XtRealizeWidget (fakeW);

    /* Enable the VDrag option on the VFake widget. */

    VDragEnableGrab (fakeW, DragButton, DragMask);

    /* Trap ButtonPress events to watch for exit signal. */

    XtAddRawEventHandler (fakeW, ButtonPressMask, 0, ButtonPressHandler, 0);

    XtMainLoop ();
}
