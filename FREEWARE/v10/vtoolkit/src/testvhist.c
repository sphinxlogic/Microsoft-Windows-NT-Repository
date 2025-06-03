/* testvhist.c - Sample program using the VHist widget

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This program is an example of how to use the VHist widget.

MODIFICATION HISTORY:

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

03-Sep-1993 (sjk/nfc) Run through Motif porting tools, cleaned up, #...def MOTIF
	conditionals removed

14-Nov-1991 (sjk) Use VtkCreateCallback.

06-Nov-1991 (sjk) More cleanup.

06-Feb-1991 (sjk) Reformat.

01-Jun-1990 (sjk) Add support for colored bars.

09-Apr-1990 (sjk) Add MOTIF support.  Use VtkUtil convenience routines.

05-Apr-1990 (sjk) Add support for VHistOrientationBestFit.

22-Feb-1990 (sjk) Clean up packaging.

12-Sep-1989 (sjk) Initial entry.
*/

#include <stdio.h>
#include <stdlib.h>
#include "vtoolkit.h"
#include "vhist.h"

#define minValue 	0			/* low end of range */
#define maxValue 	100			/* high end of range */

#define appName		"TestVHist"
#define appClass	"testvhist_defaults"
#define uidFile		"testvhist.uid"

static MrmHierarchy mrmHierarchy;
static int counter = 0;				/* iteration counter */
static int baseValue = 0;			/* initial "baseline" value */
static int showLabels = 1;			/* show bar labels (T/F) */
static VHistBar barA[20];			/* array of bars */
static int barCnt = XtNumber (barA) / 2;	/* number of bars being shown */

static Widget widgetA[32];
#define appW		widgetA[0]
#define mainW		widgetA[1]
#define vHistW		widgetA[2]

static VHistGrid gridA[] = {			/* array of ten equally ...   */
    {0, 10, 3, 2, 0},				/*   spaced grid lines ...    */
    {0, 20, 2, 1, 1},				/*   initialized with a ...   */
    {0, 30, 3, 2, 2},				/*   variety of dash ...      */
    {0, 40, 2, 2, 3},				/*   patterns and colors      */
    {0, 50, 4, 2, 4},
    {0, 60, 2, 2, 5},
    {0, 70, 3, 2, 6},
    {0, 80, 2, 1, 7},
    {0, 90, 3, 2, 8},
    {0, 100, 7, 4, 9},
};

static void TestAddBarCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    /* Increment the bar count (if the array is big enough).  The VHistNbarCount
     * resource will be updated the next time UpdateCallback is called. */

    if (barCnt != XtNumber (barA)) barCnt++;
}

static void TestRemoveBarCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    /* Decrement the bar count (but leave at least one).  The bar will
     * be removed the next time the UpdateCallback is called. */

    if (barCnt != 1) barCnt--;
}

static void TestSelectCallback(Widget unused_w, Opaque unused_closure, VHistSelectCallbackStruct *cbDataP)
{
    printf ("SelectCallback for bar #%d\n", cbDataP->bar);
}

static void TestSelectConfirmCallback(Widget unused_w, Opaque unused_closure, VHistSelectCallbackStruct *cbDataP)
{
    printf ("SelectConfirmCallback for bar #%d\n", cbDataP->bar);
}

static void SetGrids(void)
{
    int i;
    VHistGrid *gridP;

    /* Reformat the grid labels.  Alternate between the low and high labels. */

    for (i = 0, gridP = gridA; i < XtNumber(gridA); i++, gridP++) {
	gridP->mask = (VHistGridValueMask | VHistGridDashMask | VHistGridColoridxMask);

	if (i % 2) {
	    sprintf (gridP->highLabelP, "%d", gridP->value);
	    gridP->mask |= VHistGridHighLabelMask;
	} else {
	    sprintf (gridP->lowLabelP, "%d", gridP->value);
	    gridP->mask |= VHistGridLowLabelMask;
	}
    }

    /* Pass the new grid array to the VHist widget. */

    XtVaSetValues (vHistW,
	VHistNgrids, 	 gridA,
	VHistNgridCount, XtNumber (gridA),
	0);
}

static void TestShiftGridCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    int i;
    VHistGrid *gridP;

    /* Add 5 to each grid's value, with wraparound. */

    for (i = 0, gridP = gridA; i < XtNumber(gridA); i++, gridP++)
	if ((gridP->value += 5) > maxValue) gridP->value -= maxValue;

    SetGrids ();
}

static void TestOrientationCallback(Widget unused_w, int *valueP, XmToggleButtonCallbackStruct *cbDataP)
{
    VtkChangeRadioResource (vHistW, valueP, cbDataP, VHistNorientation);
}

static void TestLabelsCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    /* Toggle the value of the showLabels flag.  The widget will be updated the next time UpdateCallback is called. */

    showLabels ^= 1;
}

static void UpdateCallback(XtPointer unused_closure, XtIntervalId *unused_idP)
{
    int i, r;
    VHistBar *barP;

    /* Every fifth iteration, change the base value. */

    if (!(counter % 5)) {
	baseValue += (rand () % 11 - 5);	/* add or subtract a little */
	if (baseValue < minValue) baseValue = minValue;
	if (baseValue > maxValue) baseValue = maxValue;
    }

    /* Each time through, randomly adjust the bars' high values.  On average,
     * only change a given bar's value half the time. */

    for (i = 0, barP = barA; i < barCnt; i++, barP++) {
	/* Set all the low values to the common base value.  The widget does not
         * require that all bars have the same base value, but it makes the
         * sample program look better. */

	barP->mask     = VHistBarLowValueMask;
	barP->lowValue = baseValue;

	barP->mask    |= VHistBarHighValueMask;
	r = rand () % 22;
	if (r < 11) 			     barP->highValue += (r - 5);
	if (barP->highValue < minValue)      barP->highValue = minValue;
	else if (barP->highValue > maxValue) barP->highValue = maxValue;

 	barP->mask    |= VHistBarColoridxMask;
	barP->coloridx = (barP->highValue - barP->lowValue) / 10;

	/* Reformat the bar labels.  The widget will switch the bar's
	 * high and low values if highValue is less than lowValue, but
	 * we need to switch the labels. */

	if (showLabels) {
	    barP->mask |= (VHistBarLowLabelMask | VHistBarHighLabelMask);

	    if (barP->lowValue < barP->highValue) {
		sprintf (barP->lowLabelP,  "%d", barP->lowValue);	
		sprintf (barP->highLabelP, "%d", barP->highValue);	
	    } else {
		sprintf (barP->lowLabelP,  "%d", barP->highValue);	
		sprintf (barP->highLabelP, "%d", barP->lowValue);	
	    }
	}
    }

    /* Tell the VHist widget about the new bar values and labels. */

    XtVaSetValues (vHistW,
	VHistNbars, 	barA,
	VHistNbarCount, barCnt,
	0);

    counter++;					/* maintain iteration counter */
    XtAddTimeOut (100, UpdateCallback, 0);	/* another update in 0.1 sec */
}

static void TestQuitCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    exit (1);
}

static MrmRegisterArg mrmRegisterArgs[] = {
    {"TestAddBarCallback",		(char *)TestAddBarCallback},
    {"TestRemoveBarCallback",		(char *)TestRemoveBarCallback},
    {"TestSelectCallback",		(char *)TestSelectCallback},
    {"TestSelectConfirmCallback",	(char *)TestSelectConfirmCallback},
    {"TestShiftGridCallback",		(char *)TestShiftGridCallback},
    {"TestOrientationCallback",		(char *)TestOrientationCallback},
    {"TestLabelsCallback",		(char *)TestLabelsCallback},
    {"TestQuitCallback",		(char *)TestQuitCallback}};

int main(int argc, char **argv)
{
    VHistGrid *gridP;
    VHistBar *barP;
    int i;

    srand (1);			/* initialize random number generator */

    MrmInitialize ();		/* initialize MRM */
    VHistInitializeForMRM();	/* tell MRM about the VHist widget class */

    /* Initialize the application. */

    if (!VtkInitialize (appName, appClass, &argc, argv, uidFile, mrmRegisterArgs, XtNumber (mrmRegisterArgs), &mrmHierarchy, &appW))
	exit (0);

    /* Fetch the application widget hierarchy. */

    (void)VtkFetchWidget (mrmHierarchy, "testVHistMain", appW, widgetA, 0);

    /* Allocate small buffers for the dynamic grid and bar labels. */

    for (i=0, gridP=gridA; i<XtNumber(gridA); i++, gridP++) {
	gridP->lowLabelP  = XtMalloc (8); *gridP->lowLabelP  = 0;
	gridP->highLabelP = XtMalloc (8); *gridP->highLabelP = 0;
    }

    for (i=0, barP=barA; i<XtNumber(barA); i++, barP++) {
	barP->lowLabelP  = XtMalloc (8); *barP->lowLabelP  = 0;
	barP->highLabelP = XtMalloc (8); *barP->highLabelP = 0;
    }

    SetGrids ();	    /* tell the VHist widget about the grids */

    XtManageChild (mainW);			/* manage the main window */

    /* Work around a (V2) toolkit bug that fails to properly position the
     * application shell based on the main window's position. */

    XtVaSetValues (XtParent (mainW),
	XtNx, mainW->core.x,
	XtNy, mainW->core.y,
	0);

    XtRealizeWidget (appW);			/* realize the hierarchy */
    XtAddTimeOut (1000, UpdateCallback, 0);	/* schedule first update */

    XtMainLoop ();				/* process events */
}
