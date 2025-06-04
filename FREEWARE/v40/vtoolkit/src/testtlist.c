/* testtlist.c - Sample program using the TList widget

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This program is an example of how to use the TList widget.  It creates a
TList widget and continuously posts messages to it by using a timer callback
routine.

MODIFICATION HISTORY:

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

03-Sep-1993 (sjk) VToolkit V2.

14-Nov-1991 (sjk) Use VtkCreateCallback.

06-Nov-1991 (sjk) Cleanup.

03-Apr-1991 (sjk) Upgrade to MOTIF and exercise new TList widget features.

06-Aug-1990 (sjk) Add Reset and Quit callbacks.

20-Apr-1990 (sjk) Portability work.

12-Sep-1989 (sjk) Initial entry.
*/

#include <stdlib.h>
#include <stdio.h>
#include "vtoolkit.h"
#include "tlist.h"
#include "vframe.h"

#define appName		"TestTList"
#define appClass	"testtlist_defaults"
#define uidFile		"testtlist.uid"

static MrmHierarchy mrmHierarchy;
static int nextColoridx;		/* changing message color */
static int nextFontidx;			/* changing message font */
static int msgNum;			/* message counter */

static Widget widgetA[32];
#define appW		widgetA[0]
#define mainW		widgetA[1]
#define tListW		widgetA[2]

static void ResetCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    TListReset (tListW);
}

static void QuitCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    exit (1);
}

static void ScrollDelayCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    int scrollDelay = (int)VtkGetOneValue (tListW, TListNscrollDelay);

    /* If scrollDelay was previous enabled, disable it by setting it to zero.
     * It it was disabled, set it to 100 msec (1/10 of a second). */

    if (scrollDelay) scrollDelay = 0;
    else	     scrollDelay = 100;

    XtVaSetValues (tListW, TListNscrollDelay, scrollDelay, 0);
}

static void WordWrapCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    int wordWrap = (int)VtkGetOneValue (tListW, TListNwordWrap);

    /* Toggle the wordWrap resource setting. */

    XtVaSetValues (tListW, TListNwordWrap, !wordWrap, 0);
}

static void TimeOutCallback(XtPointer unused_closure, XtIntervalId *unused_idP)
{
    char textA[100];
    int i;

    /* Put out a burst of lines. */

    for (i = 0; i < 2; i++) {
	msgNum++;
	nextColoridx++;
    
	sprintf (textA, "Message number %d - Now is the time for all good men to come to the aid of their country", msgNum);
	textA[msgNum % 80] = 0;		/* chop it off to test short messages */
	TListAddLine (tListW, textA, nextFontidx++, nextColoridx);
    
	sprintf (textA, "    line %d:2 - there are three lines in this message, and this is the second", msgNum);
	textA[msgNum % 80] = 0;
	TListAddLine (tListW, textA, nextFontidx++, nextColoridx);
    
	sprintf (textA, "    line %d:3", msgNum);
	TListAddLine (tListW, textA, nextFontidx++, nextColoridx);
    }

    /* Set the next timer. */

    XtAddTimeOut (1000, TimeOutCallback, 0);
}

static MrmRegisterArg mrmRegisterArgs[] = {
    {"QuitCallback",		(char *)QuitCallback},
    {"ResetCallback",		(char *)ResetCallback},
    {"ScrollDelayCallback",	(char *)ScrollDelayCallback},
    {"WordWrapCallback",	(char *)WordWrapCallback},
};

int main(int argc, char **argv)
{
    MrmInitialize ();		/* tell MRM about the DWT widget classes */
    TListInitializeForMRM ();	/* tell MRM about the TList widget class */
    VFrameInitializeForMRM ();	/* tell MRM about the VFrame widget class */

    /* Initialize the application. */

    if (!VtkInitialize (appName, appClass, &argc, argv, uidFile, mrmRegisterArgs, XtNumber (mrmRegisterArgs), &mrmHierarchy, &appW))
	exit (0);

    /* Fetch the application widget hierarchy. */

    (void)VtkFetchWidget (mrmHierarchy, "testTListMain", appW, widgetA, 0);

    /* Position the main window.  (Workaround XUI V2 bug.) */

    XtVaSetValues (XtParent (mainW),
	XtNx, mainW->core.x,
	XtNy, mainW->core.y,
	0);

    XtManageChild (mainW);			/* manage the main window */
    XtRealizeWidget (appW);			/* realize the hierarchy */
    XtAddTimeOut (1000, TimeOutCallback, 0);	/* start the first timer */

    XtMainLoop ();				/* process events */
}
