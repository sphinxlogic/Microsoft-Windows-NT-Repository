/* testvlist.c - Sample program using the VList widget

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This program is an example of how to use the VList widget.

MODIFICATION HISTORY:

29-Oct-1993 (sjk) Add test for VListNselectionIsLocked resource.

22-Oct-1993 (sjk) Add test for VListNgridStyle and VListNgridPixmap resources.

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

28-Aug-1993 (nfc) Run through the Motif porting tools

20-Nov-1991 (sjk) Add test for VListNspaceTiersEqually resource.

15-Nov-1991 (sjk) Add test for VListNspaceGroupsEqually resource.

14-Nov-1991 (sjk) Use VtkActivatePopupMenu.

06-Nov-1991 (sjk) Cleanup.

30-Oct-1991 (sjk) Don't clear SText widgets on enter, to support autorepeat load.

17-May-1991 (sjk) Integrate Ken Burechailo's MOTIF updates.  Thanks Ken!

29-Aug-1990 (sjk) Add test for per-line background colors.

28-Aug-1990 (sjk) Get rid of painful dash patterns that were crashing FireFox and messing up on other X servers.

23-Jul-1990 (sjk) Add icon support.

01-Jun-1990 (sjk) Portability tweak.

20-Apr-1990 (sjk) Portability work.

10-Apr-1990 (sjk) Use VtkUtil convenience routines.  Convert to portable C (not tested yet).

16-Mar-1990 (sjk) Use foreground color for coloridx zero.

09-Mar-1990 (sjk) Add histogram support.  Move auto-repeating pushbutton logic into a separate module, vautorepeat.c.
	Other, miscellaneous changes, including fixing a memory leak after calls to VListGetAllSelectedLines.

31-Jan-1990 (sjk) Make necessary changes to improve portability to RISC.

26-Jan-1990 (sjk) Add more drag logic.  Line items can be moved around within a single list by using MB3 (MB2 for MOTIF).

23-Jan-1990 (sjk) Add test for stippled text.  Start adding drag logic.

22-Jan-1990 (sjk) Add test for label pad options.

19-Jan-1990 (sjk) Add MOTIF support.

15-Jan-1990 (sjk) Add test for segmented strings.  Add test for all VListMakeLineVisible options.

09-Jan-1990 (sjk) Add test for VListComputePreferredWidth.

20-Nov-1989 (sjk) Version X2.0 update.

15-Nov-1988 (sjk) Initial entry.
*/

#include <stdio.h>
#include <stdlib.h>
#include "vtoolkit.h"
#include "vlist.h"
#include "vframe.h"
#include "vheader.h"
#include "vautorepeat.h"
#include <Xm/Text.h>

#define appName		"TestVList"
#define appClass	"testvlist_defaults"
#define popupButton 	Button3
#define uidFile 	"testvlist.uid"

static Widget widgetA[32];
#define appW		widgetA[0]
#define dragW		widgetA[1]
#define vListW		widgetA[2]
#define mainW		widgetA[3]
#define popupMenuW	widgetA[4]

static Widget sTextWA[3];			/* widgets with negative indices (in UIL) go into this array */
static MrmHierarchy mrmHierarchy;		/* UID file "channel" */
static Pixmap iconA[64];
static int timerIsOn, totalFields, iconField, histField;
static int firstTextField, secondTextField, thirdTextField;
static int iconCnt, nextLine;

/* Define literals for the colors defined in testvlist.uil. */

#define COLOR_black	0
#define COLOR_red	1
#define COLOR_orange	2
#define COLOR_yellow	3
#define COLOR_green	4
#define COLOR_blue	5
#define COLOR_magenta	6

static int GetFieldType(int field)
{
    if ((field == firstTextField) || (field == secondTextField) || (field == thirdTextField)) return (VListFieldTypeText);
    else if (field == iconField) 							      return (VListFieldTypeIcon);
    else 										      return (VListFieldTypeHist);
}

static int GetNamedIntFromUid(char *nameP)
{
    int *value;
    MrmCode type;

    if (!MrmFetchLiteral (mrmHierarchy, nameP, XtDisplay(appW), (char **)&value, &type)) return (0);

    return (*value);
}

static int RandomValue(int min, int max)
{
    return ((unsigned int)(rand ()) % (max - min + 1) + min);
}

static Pixmap RandomIcon(void)
{
    int iconidx = RandomValue (0, iconCnt);

    if (iconidx == iconCnt) return (0);		/* no icon */
    else 		    return (iconA[iconidx]);
}

static int ValueToColoridx(int value)
{
    if (value < 0)  return (COLOR_blue);
    if (value < 60) return (COLOR_green);
    if (value < 90) return (COLOR_yellow);
    		    return (COLOR_red);
}

static Opaque AddOneLine(char *text0P, char *text1P, char *text2P, int row)
{
    VListLine vLineA[10];		/* new line's description */
    int coloridx = rand ();		/* pick random color index */
    int fontidx  = rand ();		/* pick random font index */

    /* Initialize the VListLine array with the new line's description. */

    vLineA[iconField].mask 		= VListLineIconMask | VListLineColoridxMask;
    vLineA[iconField].icon 		= RandomIcon ();
    vLineA[iconField].coloridx 		= coloridx;

    vLineA[firstTextField].mask 	= VListLineTextMask | VListLineColoridxMask | VListLineFontidxMask;
    vLineA[firstTextField].textP 	= text0P;
    vLineA[firstTextField].coloridx 	= coloridx;
    vLineA[firstTextField].fontidx 	= fontidx;

    vLineA[secondTextField].mask 	= VListLineTextMask | VListLineColoridxMask | VListLineFontidxMask;
    vLineA[secondTextField].textP 	= text1P;
    vLineA[secondTextField].coloridx 	= coloridx;
    vLineA[secondTextField].fontidx 	= fontidx;

    vLineA[thirdTextField].mask 	= VListLineTextMask | VListLineColoridxMask | VListLineFontidxMask;
    vLineA[thirdTextField].textP 	= text2P;
    vLineA[thirdTextField].coloridx 	= coloridx;
    vLineA[thirdTextField].fontidx 	= fontidx;

    vLineA[histField].mask 		= VListLineLowValueMask | VListLineHighValueMask | VListLineColoridxMask;
    vLineA[histField].lowValue 		= 0;
    vLineA[histField].highValue 	= RandomValue ((-25), 100);
    vLineA[histField].coloridx 		= ValueToColoridx (vLineA[histField].highValue);

    /* Add the line to the list and return its closure value. */

    return ((Opaque)VListAddLine (vListW, vLineA, row));
}

static void SetSTextString(int field, char *valueP)
{
    Widget sTextW = sTextWA[field-1];

    if (!sTextW) return;			  /* SText widget not created yet */

    if (valueP) XmTextSetString (sTextW, valueP); /* set new contents */
    else        XmTextSetString (sTextW, "");	  /* clear the contents */
}

static char *GetSTextString(int field)
{
    return ((char *)XmTextGetString (sTextWA[field-1]));
}

static void ResetCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    int i;
    char text0A[100], text1A[100], text2A[100];

    VListStartBatchUpdate (vListW);	/* start "massive" VList update */
    VListRemoveAllLines (vListW);	/* empty out the old lines */

    /* Load the VList widget with individually formatted lines. */

    for (i=1; i <= 100; i++) {
	nextLine++;
	sprintf (text0A, "Row %d", nextLine);
	sprintf (text1A, "%d", nextLine*nextLine);
	sprintf (text2A, "%d", nextLine*nextLine*nextLine);

  	AddOneLine (text0A, text1A, text2A, VListAddAtBottom);
    }

    /* Make field/row 0/0 visible at the top of the window. */

    VListMakeLineVisible (vListW, VListFieldRowToLine (vListW, 0, 0), VListVisibleAtTop);

    VListEndBatchUpdate (vListW);	/* end update - repaint the window */
}

static void ResizeCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    int width = VListComputePreferredWidth (vListW);

    /* Change the width of the VList widget's (VFrame) parent to conform. */

    XtVaSetValues (vListW->core.parent, XtNwidth, width, XmNrightAttachment, XmATTACH_NONE, 0);
}

static void SegmentStringCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    Opaque *lineClosuresP;
    int selectionCnt, i, textStrlen, field, textPosition, segment, partitionRange;
    VListLine vLine;
    VListSegment vSegmentA[6], *vSegmentP;
    unsigned int random;

    selectionCnt = VListGetAllSelectedLines (vListW, &lineClosuresP);

    for (i = 0; i < selectionCnt; i++) {
	VListGetLineInfo (vListW, lineClosuresP[i], &vLine, &field, 0);
	if (GetFieldType (field) == VListFieldTypeText) {
	    textStrlen = strlen (vLine.textP);
	    textPosition = 0;

	    for (vSegmentP = vSegmentA, segment = 0; segment < XtNumber (vSegmentA); vSegmentP++, segment++) {
		vSegmentP->mask = 0;
		random = (unsigned int)(rand ());

		partitionRange = textStrlen * 2 / (XtNumber (vSegmentA) - segment);
		if (!partitionRange) partitionRange = textStrlen;

	        if (partitionRange) vSegmentP->textStrlen = random % partitionRange;
		else		    vSegmentP->textStrlen = 0;

		vSegmentP->textPosition = textPosition;

		if (random & 2) {
		    vSegmentP->mask 	|= VListSegmentColoridxMask;
		    vSegmentP->coloridx = rand ();
		}

		if (random & 4)  
		    vSegmentP->mask 	|= VListSegmentNewLineMask;

		if (random & 8)  {
		    vSegmentP->mask 	|= VListSegmentStippledMask;
		    vSegmentP->stippled = (rand () & 1);
		}

		if (random & 16) {
		    vSegmentP->mask 	|= VListSegmentFontidxMask;
		    vSegmentP->fontidx 	= rand ();
		}

		textPosition 	+= vSegmentP->textStrlen;
		textStrlen	-= vSegmentP->textStrlen;
	    }
	
	    VListChangeLineSegments (vListW, lineClosuresP[i], vSegmentA, XtNumber (vSegmentA));
	}
    }

    XtFree ((char *)lineClosuresP);
}

static void ChangeBackgroundCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    Opaque *lineClosuresP;
    int selectionCnt, i;
    VListLine vLine;

    selectionCnt = VListGetAllSelectedLines (vListW, &lineClosuresP);

    for (i = 0; i < selectionCnt; i++) {
	VListGetLineInfo (vListW, lineClosuresP[i], &vLine, 0, 0);
	VListChangeLineBackground (vListW, lineClosuresP[i], vLine.background+1);
    }

    XtFree ((char *)lineClosuresP);
}

static void ChangeColorCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    Opaque *lineClosuresP;
    int selectionCnt, i;
    VListLine vLine;

    selectionCnt = VListGetAllSelectedLines (vListW, &lineClosuresP);

    for (i = 0; i < selectionCnt; i++) {
	VListGetLineInfo (vListW, lineClosuresP[i], &vLine, 0, 0);
	VListChangeLineColoridx (vListW, lineClosuresP[i], vLine.coloridx+1);
    }

    XtFree ((char *)lineClosuresP);
}

static void ChangeStippledCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    Opaque *lineClosuresP;
    int selectionCnt, i;
    VListLine vLine;

    selectionCnt = VListGetAllSelectedLines (vListW, &lineClosuresP);

    for (i = 0; i < selectionCnt; i++) {
	VListGetLineInfo (vListW, lineClosuresP[i], &vLine, 0, 0);
	VListChangeLineStippled (vListW, lineClosuresP[i], 1 - vLine.stippled);
    }

    XtFree ((char *)lineClosuresP);
}

static void ChangeFontCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    Opaque *lineClosuresP;
    int selectionCnt, i, field, fieldType;
    VListLine vLine;

    selectionCnt = VListGetAllSelectedLines (vListW, &lineClosuresP);

    for (i = 0; i < selectionCnt; i++) {
	VListGetLineInfo (vListW, lineClosuresP[i], &vLine, &field, 0);

        fieldType = GetFieldType (field);

	if (fieldType == VListFieldTypeText)      VListChangeLineFontidx (vListW, lineClosuresP[i], vLine.fontidx + 1);
	else if (fieldType == VListFieldTypeIcon) VListChangeLineIcon    (vListW, lineClosuresP[i], RandomIcon ());
    }

    XtFree ((char *)lineClosuresP);
}

static void MakeLineVisibleCallback(Widget unused_w, int *visibleP, XmAnyCallbackStruct *unused_cbDataP)
{
    Opaque lineClosure;

   /* This routine is called when the user selects the "Make Line Visible" action.
    * If there is no line selected in the VList, this routine simply returns.
    * Otherwise, the VList widget is scrolled so that the first selected line is
    * positioned at specified by the visible option. */

    if (!(lineClosure = (Opaque)VListGetFirstSelectedLine (vListW))) return;

    VListMakeLineVisible (vListW, lineClosure, *visibleP);
}

static void MoveUpCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    Opaque *lineClosuresP;
    int selectionCnt, i;

    /* This routine is called when the user selects the "MoveUp" action.
     * All selected lines are moved up one row. */

    selectionCnt = VListGetAllSelectedLines (vListW, &lineClosuresP);

    for (i=0; i<selectionCnt; i++) VListMoveLine (vListW, lineClosuresP[i], VListMoveLineUp);

    /* Make sure the first selected line (if any) is still visible. */

    if (selectionCnt) VListMakeLineVisible (vListW, lineClosuresP[0], VListVisibleInWindow);

    XtFree ((char *)lineClosuresP);
}

static void MoveDownCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    Opaque *lineClosuresP;
    int selectionCnt, i;

   /* This routine is called when the user selects the "MoveDown" action.
    * All selected lines are moved down one row. */

    selectionCnt = VListGetAllSelectedLines (vListW, &lineClosuresP);

    /* Loop through in reverse order. */

    for (i = selectionCnt; i--;) VListMoveLine (vListW, lineClosuresP[i], VListMoveLineDown);

    /* Make sure the first selected line (if any) is still visible. */

    if (selectionCnt) VListMakeLineVisible (vListW, lineClosuresP[0], VListVisibleInWindow);

    XtFree ((char *)lineClosuresP);
}

static Opaque EnterOneLine(int row)
{
    char *textPA[3];
    Opaque closure;

    textPA[0] = GetSTextString (firstTextField);
    textPA[1] = GetSTextString (secondTextField);
    textPA[2] = GetSTextString (thirdTextField);

    closure = AddOneLine (textPA[0], textPA[1], textPA[2], row);

    XtFree (textPA[0]);			/* free text from SText */
    XtFree (textPA[1]);			/* free text from SText */
    XtFree (textPA[2]);			/* free text from SText */

    return (closure);
}

static void EnterNewLines(void)
{
    Opaque *lineClosuresP;
    int selectionCnt, i, row;

    selectionCnt = VListGetAllSelectedLines (vListW, &lineClosuresP);

    /* If no lines are selected, add one at the end and make it visible.  Otherwise, add a new line after each selected line. */

    if (!selectionCnt) VListMakeLineVisible (vListW, EnterOneLine (VListAddAtBottom), VListVisibleInWindow);
    else
	for (i = selectionCnt; i--;) {
	    VListGetLineInfo (vListW, lineClosuresP[i], 0, 0, &row);
   	    row++;
	    (void)EnterOneLine (row);
	}

    XtFree ((char *)lineClosuresP);
}

static void EnterCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    EnterNewLines ();
}

static void RemoveCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    Opaque *lineClosuresP;
    int selectionCnt, i;

   /* This routine is called when the user selects the "Remove" action.
    * A copy of the selection list is retrieved from the VList widget, and all
    * selected lines are removed.  Then, the copy of the selection list is freed. */

    selectionCnt = VListGetAllSelectedLines (vListW, &lineClosuresP);

    for (i = 0; i < selectionCnt; i++) VListRemoveLine (vListW, lineClosuresP[i]);

    XtFree ((char *)lineClosuresP);
}

static void QuitCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    exit (1);
}

static void SelectCallback(Widget w, Opaque closure, VListSelectCallbackStruct *cbDataP)
{
    /* This routine is called when the user single-clicks over the VList widget.
     * The callback structure pointed to by cbDataP contains information about
     * the selected line.  In this sample program, there is no special action
     * taken when the user selects a line, therefore this routine is empty. */
}

static void SelectConfirmCallback(Widget unused_w, Opaque unused_closure, VListSelectCallbackStruct *cbDataP)
{
    /* This routine is called when the user double-clicks over the VList widget.
     * The callback structure pointed to by cbDataP contains information about
     * the selected line.  In this example, if there is no selected line,
     * we ring the bell.  Otherwise, the text from the selected line item is
     * copied into the SText widget. */

    if (!cbDataP->vLine.closure) {
	XBell (XtDisplay (appW), 0);
	return;
    }

    if (GetFieldType (cbDataP->field) != VListFieldTypeText) {
	XBell (XtDisplay (appW), 0);
	return;
    }

    SetSTextString (cbDataP->field, cbDataP->vLine.textP);
}

static void InitializeVList(void)
{
    VListField fieldA[10];
    VListField *fieldP = fieldA;
    VListLabel labelA[10];
    VListLabel *labelP = labelA;
    VListGrid gridA[10];
    VListGrid *gridP = gridA;

    /* icon field */

    iconField = fieldP - fieldA;		/* save field number */

    fieldP->mask = VListFieldTypeMask;
    fieldP->type = VListFieldTypeIcon;
    fieldP++;

    /* first text field */

    firstTextField = fieldP - fieldA;

    labelP->mask = (VListLabelTextMask | VListLabelRowMask | VListLabelAlignmentMask | VListLabelFirstFieldMask
	| VListLabelPadMask);
    labelP->textP 	= "Field 1";
    labelP->row 	= 1;
    labelP->alignment 	= VListAlignmentEnd;
    labelP->firstField 	= firstTextField;
    labelP->pad 	= VListLabelPadWithBlanks;
    labelP++;

    fieldP->mask 	= VListFieldAlignmentMask;
    fieldP->alignment 	= VListAlignmentEnd;
    fieldP++;

    /* second text field */

    secondTextField = fieldP - fieldA;

    labelP->mask = (VListLabelTextMask | VListLabelRowMask | VListLabelAlignmentMask | VListLabelFirstFieldMask
	| VListLabelPadMask);
    labelP->textP 	= "Field 2";
    labelP->row 	= 1;
    labelP->alignment 	= VListAlignmentCenter;
    labelP->firstField 	= secondTextField;
    labelP->pad 	= VListLabelPadWithBlanks;
    labelP++;

    fieldP->mask 	= VListFieldAlignmentMask;
    fieldP->alignment 	= VListAlignmentCenter;
    fieldP++;

    /* third text field */

    thirdTextField = fieldP - fieldA;

    labelP->mask = (VListLabelTextMask | VListLabelRowMask | VListLabelAlignmentMask | VListLabelFirstFieldMask
	| VListLabelPadMask);
    labelP->textP 	= "Field 3";
    labelP->row 	= 1;
    labelP->alignment 	= VListAlignmentBeginning;
    labelP->firstField 	= thirdTextField;
    labelP->pad 	= VListLabelPadWithBlanks;
    labelP++;

    fieldP->mask 	= VListFieldAlignmentMask;
    fieldP->alignment 	= VListAlignmentBeginning;
    fieldP++;

    /* histogram field */

    histField = fieldP - fieldA;

    gridP->mask = VListGridValueMask | VListGridTextMask | VListGridRowMask | VListGridDashMask;
    gridP->value 	= 0;
    gridP->textP 	= "0";
    gridP->row 		= 1;
    gridP->dashOn 	= 1;
    gridP->dashOff 	= 1;
    gridP++;

    gridP->mask = VListGridValueMask | VListGridColoridxMask | VListGridTextMask | VListGridRowMask;
    gridP->value 	= 60;
    gridP->coloridx 	= COLOR_green;
    gridP->textP 	= "60";
    gridP->row 		= 1;
    gridP++;

    gridP->mask = VListGridValueMask | VListGridTextMask | VListGridRowMask | VListGridColoridxMask;
    gridP->value 	= 90;
    gridP->textP 	= "90";
    gridP->row 		= 1;
    gridP->coloridx 	= COLOR_red;
    gridP++;

    gridP->mask = VListGridValueMask | VListGridDashMask;
    gridP->value 	= 100;
    gridP->dashOn 	= 1;
    gridP->dashOff 	= 1;
    gridP++;

    fieldP->mask = VListFieldTypeMask | VListFieldMinValueMask | VListFieldMaxValueMask | VListFieldGridsMask;
    fieldP->type 	= VListFieldTypeHist;
    fieldP->minValue 	= (-25);
    fieldP->maxValue 	= 100;
    fieldP->gridsP 	= gridA;
    fieldP->gridCount 	= gridP - gridA;	/* number of grid lines */
    fieldP++;

    /* overall label */

    totalFields = fieldP - fieldA;

    labelP->mask = (VListLabelTextMask | VListLabelRowMask
	| VListLabelAlignmentMask | VListLabelFirstFieldMask
	| VListLabelLastFieldMask | VListLabelPadMask);
    labelP->textP 	= "Line Table";
    labelP->row 	= 0;
    labelP->alignment 	= VListAlignmentCenter;
    labelP->firstField 	= 0;
    labelP->lastField 	= totalFields - 1;
    labelP->pad 	= VListLabelPadWithDashes;
    labelP++;

    XtVaSetValues (vListW,
	VListNfieldCount, fieldP - fieldA, VListNfields, fieldA,
        VListNlabelCount, labelP - labelA, VListNlabels, labelA,
	0);
}

static void TimerUpdateCallback(XtPointer unused_closure, XtIntervalId *unused_idP)
{
    int row, rowCnt, delta;
    Opaque lineClosure;
    VListLine vLine;

    if (!(timerIsOn & 1)) return;			/* has been turned off */

    if (timerIsOn & 2) EnterNewLines ();
    else {
	rowCnt = VListGetLineCount (vListW) / 4;
    
	/* Randomly change some of the bars, randomly. */
    
	for (row = 0; row < rowCnt; row++)
	    if (((unsigned int)(rand ()) % 100) < 20) {
		delta = ((unsigned int)(rand ()) % 23) - 11;
    
		lineClosure = (Opaque)VListFieldRowToLine (vListW, histField, row);
		VListGetLineInfo (vListW, lineClosure, &vLine, 0, 0);
    
		vLine.mask = VListLineHighValueMask | VListLineColoridxMask;
		vLine.highValue += delta;
		if (vLine.highValue < (-25)) vLine.highValue = (-25);
		if (vLine.highValue > 100) vLine.highValue = 100;
		vLine.coloridx = ValueToColoridx (vLine.highValue);
    
		VListChangeLine (vListW, lineClosure, &vLine);
	    }
    }

    XtAddTimeOut (200, TimerUpdateCallback, 0);
}

static void TimerCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    timerIsOn++;

    if (!(timerIsOn & 1)) return;

    XtAddTimeOut (100, TimerUpdateCallback, 0);
}

static void SelectionIsLockedCallback(Widget unused_w, int *valueP, XmToggleButtonCallbackStruct *cbDataP)
{
    VtkChangeRadioResource (vListW, valueP, cbDataP, VListNselectionIsLocked);
}

static void SpaceGroupsCallback(Widget unused_w, int *valueP, XmToggleButtonCallbackStruct *cbDataP)
{
    VtkChangeRadioResource (vListW, valueP, cbDataP, VListNspaceGroupsEqually);
}

static void SpaceTiersCallback(Widget unused_w, int *valueP, XmToggleButtonCallbackStruct *cbDataP)
{
    VtkChangeRadioResource (vListW, valueP, cbDataP, VListNspaceTiersEqually);
}

static void SelectStyleCallback(Widget unused_w, int *valueP, XmToggleButtonCallbackStruct *cbDataP)
{
    VtkChangeRadioResource (vListW, valueP, cbDataP, VListNselectStyle);
}

static void SelectUnitCallback(Widget unused_w, int *valueP, XmToggleButtonCallbackStruct *cbDataP)
{
    VtkChangeRadioResource (vListW, valueP, cbDataP, VListNselectUnit);
}

static void VScrollBarPositionCallback(Widget unused_w, int *valueP, XmToggleButtonCallbackStruct *cbDataP)
{
    VtkChangeRadioResource (vListW, valueP, cbDataP, VListNvScrollBarPosition);
}

static void HScrollBarPolicyCallback(Widget unused_w, int *valueP, XmToggleButtonCallbackStruct *cbDataP)
{
    VtkChangeRadioResource (vListW, valueP, cbDataP, VListNhScrollBarPolicy);
}

static void VScrollBarPolicyCallback(Widget unused_w, int *valueP, XmToggleButtonCallbackStruct *cbDataP)
{
    VtkChangeRadioResource (vListW, valueP, cbDataP, VListNvScrollBarPolicy);
}

static void ColumnStyleCallback(Widget unused_w, int *valueP, XmToggleButtonCallbackStruct *cbDataP)
{ 
    VtkChangeRadioResource (vListW, valueP, cbDataP, VListNcolumnStyle);
}

static void AdjustIntResource(char *resNameP, int delta)
{
    int value;

    value = (int)VtkGetOneValue (vListW, resNameP);

    value += delta;
    if (value < 0) return;

    XtVaSetValues (vListW, resNameP, value, 0);
}

static void FieldSpacingWidthCallback(Widget unused_w, int *valueP, XmAnyCallbackStruct *unused_cbDataP)
{
    AdjustIntResource (VListNfieldSpacingWidth, *valueP);
}

static void RowSpacingHeightCallback(Widget unused_w, int *valueP, XmAnyCallbackStruct *unused_cbDataP)
{
    AdjustIntResource (VListNrowSpacingHeight, *valueP);
}

static void GridStyleCallback(Widget unused_w, int *valueP, XmAnyCallbackStruct *unused_cbDataP)
{
    XtVaSetValues (vListW, VListNgridStyle, *valueP, 0);
}

static void ButtonPressHandler(Widget w, XtPointer unused_closure, XEvent *eventOP, Boolean *unused_continueP)
{
    XButtonEvent *eventP = (XButtonEvent *)eventOP;
    Opaque lineClosure;	/* closure for target line */

    /* This routine is called when a ButtonPress event is received by the VList
     * widget.  If it is a simple, un-chorded Button2 (Button3 for MOTIF) event,
     * this routine selects the line over which the pointer is positioned and
     * pops up the popup menu. */

    if (eventP->button != popupButton) return;

    /* Unless the shift key is down, clear the selection. */

    if (!(eventP->state & ShiftMask)) VListSelectAll (w, 0);

    /* Determine the closure of the line we are over.  If we are not over
     * any line, beep and return.  Otherwise, select it. */

    if (!(lineClosure = (Opaque)VListXYToLine (vListW, eventP->x, eventP->y))) {
	XBell (XtDisplay (w), 0);
	return;
    }

    VListChangeLineSelect (w, lineClosure, 1);

    VtkActivatePopupMenu (popupMenuW, (XEvent *)eventP);
}

static void DragArmCallback(Widget w, Opaque unused_closure, VListDragArmCallbackStruct *cbDataP)
{
    int dragType;
    XmString xValueP;

    /* Click over deadspace or over a histogram?  Disable drag widget and return (to reject drag operation). */

    if ((!cbDataP->vLine.closure)
      || ((dragType = GetFieldType (cbDataP->field)) == VListFieldTypeHist)) {
	XtVaSetValues (w, VListNdragWidget, 0, 0);
	return;
    }

    /* Set the label in the drag widget. */

    if (dragType == VListFieldTypeIcon) {
	/* Do it twice to get around an XUI label widget resize bug. */

	if (cbDataP->vLine.icon) {
	    XtVaSetValues (dragW,
		XtNwidth, 	0,
		XtNheight, 	0,
		XmNlabelType, 	XmPIXMAP,
		XmNlabelPixmap, cbDataP->vLine.icon,
		0);
    
	    XtVaSetValues (dragW, XmNlabelPixmap, cbDataP->vLine.icon, 0);
        } else {
	    XtVaSetValues (dragW,
		XmNlabelType, 	XmSTRING,
		XmNlabelString, (xValueP = XmStringCreate ("", XmSTRING_DEFAULT_CHARSET)),
		0);
    
	    XmStringFree (xValueP);
        }
    } else {
        XtVaSetValues (dragW,
	    XmNlabelType, 	XmSTRING,
	    XmNlabelString,	(xValueP = XmStringCreate (cbDataP->vLine.textP, XmSTRING_DEFAULT_CHARSET)),
	    0);

        XtVaSetValues (dragW, XmNlabelString, xValueP, 0);

        XmStringFree (xValueP);
    }

    XtVaSetValues (w, VListNdragWidget, dragW, 0);		/* set widget to be dragged */
}

static void DragDropCallback(Widget unused_w, Opaque unused_closure, VListDragDropCallbackStruct *cbDataP)
{
    VListLine dragVLine, dropVLine;
    int dragType, dropType;

    if (!cbDataP->dropVLine.closure) return;

    dragType = GetFieldType (cbDataP->field);
    dropType = GetFieldType (cbDataP->dropField);

    if (dragType != dropType) return;

    dragVLine = cbDataP->vLine;
    dropVLine = cbDataP->dropVLine;

    if (dragType == VListFieldTypeIcon) {
	dragVLine.mask = dropVLine.mask =
	    ( VListLineIconMask 	| VListLineColoridxMask
	    | VListLineBackgroundMask 	| VListLineSelectMask
	    | VListLineSensitiveMask 	| VListLineStippledMask);
    
	VListChangeLine (vListW, dragVLine.closure, &dropVLine);
	VListChangeLine (vListW, dropVLine.closure, &dragVLine);
    } else if (dragType == VListFieldTypeText) {
	dragVLine.mask = dropVLine.mask =
	    ( VListLineTextMask 	| VListLineColoridxMask
	    | VListLineFontidxMask 	| VListLineSelectMask
	    | VListLineSensitiveMask 	| VListLineSegmentsMask
	    | VListLineStippledMask	| VListLineBackgroundMask);

	dragVLine.segmentCount = dropVLine.segmentCount = 0;
    
	dragVLine.textP = XtNewString (dragVLine.textP);
	VListChangeLine (vListW, dragVLine.closure, &dropVLine);
        VListChangeLine (vListW, dropVLine.closure, &dragVLine);
	XtFree (dragVLine.textP);
    }
}

static void LoadIconsFromUid(void)
{
    char iconNameA[16];
    int i;
    Pixel foreground, background;

    foreground = (Pixel)VtkGetOneValue (vListW, VListNforeground);
    background = (Pixel)VtkGetOneValue (vListW, XtNbackground);

    /* Load all icons of the name "iconNN", where NN are dense numbers starting with zero. */

    iconCnt = GetNamedIntFromUid ("iconCnt");

    for (i = 0; i < iconCnt; i++) {
	sprintf (iconNameA, "icon%d", i);

	MrmFetchIconLiteral (mrmHierarchy, iconNameA, XtScreen (appW), XtDisplay (appW), foreground, background, &iconA[i]);
    }
}

static MrmRegisterArg mrmRegisterArgs[] = {
    {"TestChangeBackgroundCallback",	(char *)ChangeBackgroundCallback},
    {"TestChangeColorCallback",		(char *)ChangeColorCallback},
    {"TestChangeFontCallback",		(char *)ChangeFontCallback},
    {"TestChangeStippledCallback",	(char *)ChangeStippledCallback},
    {"TestColumnStyleCallback",		(char *)ColumnStyleCallback},
    {"TestDragArmCallback",		(char *)DragArmCallback},
    {"TestDragDropCallback",		(char *)DragDropCallback},
    {"TestEnterCallback",		(char *)EnterCallback},
    {"TestFieldSpacingWidthCallback",	(char *)FieldSpacingWidthCallback},
    {"TestGridStyleCallback",		(char *)GridStyleCallback},
    {"TestHScrollBarPolicyCallback",	(char *)HScrollBarPolicyCallback},
    {"TestMakeLineVisibleCallback",	(char *)MakeLineVisibleCallback},
    {"TestMoveDownCallback",		(char *)MoveDownCallback},
    {"TestMoveUpCallback",		(char *)MoveUpCallback},
    {"TestQuitCallback",		(char *)QuitCallback},
    {"TestRemoveCallback",		(char *)RemoveCallback},
    {"TestResetCallback",		(char *)ResetCallback},
    {"TestResizeCallback",		(char *)ResizeCallback},
    {"TestRowSpacingHeightCallback",	(char *)RowSpacingHeightCallback},
    {"TestSegmentStringCallback",	(char *)SegmentStringCallback},
    {"TestSelectCallback",		(char *)SelectCallback},
    {"TestSelectConfirmCallback",	(char *)SelectConfirmCallback},
    {"TestSelectStyleCallback",		(char *)SelectStyleCallback},
    {"TestSelectUnitCallback",		(char *)SelectUnitCallback},
    {"TestSelectionIsLockedCallback",	(char *)SelectionIsLockedCallback},
    {"TestSpaceGroupsCallback",		(char *)SpaceGroupsCallback},
    {"TestSpaceTiersCallback",		(char *)SpaceTiersCallback},
    {"TestTimerCallback",		(char *)TimerCallback},
    {"TestVScrollBarPositionCallback",	(char *)VScrollBarPositionCallback},
    {"TestVScrollBarPolicyCallback",	(char *)VScrollBarPolicyCallback},
    };

int main(int argc, char **argv)
{
    srand (1);				/* initialize random number generator */

    MrmInitialize ();			/* register toolkit widgets */
    VListInitializeForMRM();		/* register VList widget */
    VFrameInitializeForMRM();		/* register VFrame widget */
    VHeaderInitializeForMRM();		/* register VHeader widget */
    VAutoRepeatInitializeForMRM();	/* register auto-repeater */

    /* Initialize the application. */

    if (!VtkInitialize (appName, appClass, &argc, argv, uidFile, mrmRegisterArgs, XtNumber (mrmRegisterArgs), &mrmHierarchy, &appW))
	exit (0);

    /* Load the main window and its children. */

    (void)VtkFetchWidget (mrmHierarchy, "testVListMain", appW, widgetA, sTextWA);
    XtManageChild (mainW);

    InitializeVList ();

    /* Work around a (DECwindows V2) toolkit bug to position the application shell based on the main window's position. */

    XtVaSetValues (XtParent (mainW),
	XtNx, mainW->core.x,
	XtNy, mainW->core.y,
	0);

    /* Realize the application shell widget and its children. */

    XtRealizeWidget (appW);

    /* Because of a toolkit restriction, a popup menu cannot be fetched until its parent has been realized, so fetch it now.
     * Add an event handler for ButtonPress to the VList widget so that we can pop up the menu.  */

    (void)VtkFetchWidget (mrmHierarchy, "testVListPopupMenu", vListW, widgetA, 0);
    XtAddEventHandler (vListW, ButtonPressMask, 0, ButtonPressHandler, 0);

    /* Load the icons from the UID file. */

    LoadIconsFromUid ();

    ResetCallback (NULL, NULL, NULL);	/* load data */
    TimerCallback (NULL, NULL, NULL);	/* start animating */

    XtMainLoop ();			/* Handle events.  Never returns. */
}
