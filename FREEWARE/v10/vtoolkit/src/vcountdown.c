/* vcountdown.c - VCountdown sample program using the VHist widget

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This program implements the VCountdown sample program.  Using the VHist widget,
a second-by-second countdown window is displayed.

To run this program on VMS, type the following lines at a DCL prompt:

	VCountdown :== $sys$disk:[]vcountdown
	VCountdown 123 "Seconds until Lunch"

MODIFICATION HISTORY:

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

03-Sep-1993 (sjk) VToolkit V2.

14-Nov-1991 (sjk) Use VtkActivatePopupMenu.

14-Nov-1991 (sjk) Use VtkCreateCallback.

06-Nov-1991 (sjk) More cleanup.

05-Feb-1991 (sjk) Reformat.

13-Jun-1990 (sjk) Bugfix - Don't call XtFree for label strings that weren't really allocated.

01-Jun-1990 (sjk) Change to conform to new (incompatible) VHist programming interface.

20-Apr-1990 (sjk) Portability tweak.

09-Apr-1990 (sjk) MOTIF support.

03-Apr-1990 (sjk) Initial entry.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vtoolkit.h"
#include "vhist.h"

static int initialSeconds;
static unsigned long startingTime;
static XrmDatabase database;
static MrmHierarchy mrmHierarchy;

static Widget widgetA[32];
#define appW		widgetA[0]
#define mainW		widgetA[1]
#define vHistW		widgetA[2]
#define popupMenuW	widgetA[3]

#define defWidth		600
#define defHeight		100
#define defTitle		"VCountdown - Seconds Remaining"
#define appClass		"vcountdown_defaults"
#define resourceFileName	"decw$user_defaults:vcountdown_defaults.dat"
#define popupButton 		Button3
#define uidFile			"vcountdown.uid"

static void SaveSettingsCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    VtkSaveNumericResource (&database, "*myMain.width", 	mainW->core.width);
    VtkSaveNumericResource (&database, "*myMain.height", 	mainW->core.height);
    VtkSaveNumericResource (&database, "vcountdown_defaults.x", appW->core.x);
    VtkSaveNumericResource (&database, "vcountdown_defaults.y", appW->core.y);

    XrmPutFileDatabase (database, resourceFileName);
}

static void UpdateHistogram(int seconds)
{
    VHistBar barA[1];
    char textBufA[10];

    barA[0].mask	= (VHistBarHighValueMask | VHistBarHighLabelMask | VHistBarLowLabelMask);
    barA[0].highValue 	= seconds;
    barA[0].highLabelP 	= textBufA;
    barA[0].lowLabelP 	= textBufA;

    sprintf (textBufA, "%d", seconds);

    XtVaSetValues (vHistW,
	VHistNbars,	barA,
	VHistNbarCount, 1,
	0);
}

static void TimeOutCallback(XtPointer unused_closure, XtIntervalId *unused_idP)
{
    int remainingSeconds = startingTime + initialSeconds - time (0);

    if (remainingSeconds <= 0) exit (1);

    UpdateHistogram (remainingSeconds);

    XtAddTimeOut (1000, TimeOutCallback, 0);
}

static void RestartCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    (void)time (&startingTime);
    UpdateHistogram (initialSeconds);
}

static void QuitCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    exit (1);
}

static void ButtonPressHandler(Widget unused_w, XtPointer unused_closure, XEvent *eventOP, Boolean *unused_continueP)
{
    XButtonEvent *eventP = (XButtonEvent *)eventOP;

    if (eventP->button != popupButton) return;

    VtkActivatePopupMenu (popupMenuW, (XEvent *)eventP);
}

static void InitializeHistogram(void)
{
    VHistGrid *gridsP;
    int gridCnt, i, step;
    char *bufP;

    step = 10;
    gridCnt = (initialSeconds + step) / step;

    while (gridCnt < 5) {
	step = step / 2;
        gridCnt = (initialSeconds + step) / step;
    }

    while (gridCnt > 20) {
	step = step * 2;
        gridCnt = (initialSeconds + step) / step;
    }

    gridsP = (VHistGrid *)XtMalloc (gridCnt * sizeof (VHistGrid));

    for (i = 0; i < gridCnt; i++) {
	bufP = (char *)XtMalloc (6);
	sprintf (bufP, "%d", i*step);

	if (i & 1) {
	    gridsP[i].mask      = VHistGridLowLabelMask;
	    gridsP[i].lowLabelP = bufP;
	} else {
	    gridsP[i].mask       = VHistGridHighLabelMask;
	    gridsP[i].highLabelP = bufP;
	}

	gridsP[i].mask 	 |= (VHistGridValueMask | VHistGridDashMask);
	gridsP[i].value   = i*step;
	gridsP[i].dashOn  = 2;
	gridsP[i].dashOff = 2;
    }

    XtVaSetValues (vHistW,
	VHistNminValue,  0,
	VHistNmaxValue,  initialSeconds,
	VHistNgrids,     gridsP,
	VHistNgridCount, gridCnt,
	0);

    for (i = 0; i < gridCnt; i++) {
	if (i & 1) XtFree (gridsP[i].lowLabelP);
	else       XtFree (gridsP[i].highLabelP);
    }

    XtFree ((char *)gridsP);
}

static MrmRegisterArg mrmRegisterArgs[] = {
    {"RestartCallback",			(char *)RestartCallback},
    {"SaveSettingsCallback",		(char *)SaveSettingsCallback},
    {"QuitCallback",			(char *)QuitCallback}};

int main(int argc, char **argv)
{
    char *titleP;

    MrmInitialize ();
    VHistInitializeForMRM ();

    if (argc >= 2) initialSeconds = atoi (argv[1]);
    else           initialSeconds = 30;

    if (argc >= 3) titleP = argv[2];
    else 	   titleP = defTitle;

    /* Initialize the application. */

    if (!VtkInitialize (titleP, appClass, &argc, argv, uidFile, mrmRegisterArgs, XtNumber (mrmRegisterArgs), &mrmHierarchy, &appW))
	exit (0);

    (void)VtkFetchWidget (mrmHierarchy, "myMain", appW, widgetA, 0);

    if (!(database = XrmGetFileDatabase (resourceFileName)))
	XtVaSetValues (mainW,
	    XtNwidth,  defWidth,
	    XtNheight, defHeight,
	    0);

    XtVaSetValues (appW,
	XtNwidth,  mainW->core.width,
	XtNheight, mainW->core.height,
	0);

    XtManageChild (mainW);			/* manage the main window */
    XtRealizeWidget (appW);			/* realize the widget tree */

    /* Because of a toolkit restriction, the popup menu cannot be fetched until its parent has been realized. */

    (void)VtkFetchWidget (mrmHierarchy, "myPopupMenu", mainW, widgetA, 0);
    XtAddEventHandler (mainW, ButtonPressMask, 0, ButtonPressHandler, 0);

    (void)time (&startingTime);
    InitializeHistogram ();
    UpdateHistogram (initialSeconds);

    XtAddTimeOut (1000, TimeOutCallback, 0);	/* set the first timer */

    XtMainLoop ();				/* and go to work */
}
