/* vdiskquota.c - VDiskQuota sample program using the VList widget

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This is the main module of the VDiskQuota utility.

MODIFICATION HISTORY:

07-Sep-1993 (sjk/nfc) VToolkit V2.

06-Nov-1991 (sjk) Cleanup.

05-Feb-1991 (sjk) Reformat.

01-Jun-1990 (sjk) Portability tweak.

14-May-1990 (sjk) Read Histogram dialog box values into memory before doing update so that SaveCurrentSettings during
	an update saves the visible values.

10-Apr-1990 (sjk) Add MOTIF support.  Use VtkUtil convenience routines.

15-Mar-1990 (sjk) Initial entry.
*/

#include <descrip.h>
#include <rms.h>
#include <rmsdef.h>
#include <ssdef.h>
#include <fibdef.h>
#include <iodef.h>
#include <libdef.h>
#include <math.h>
#include <starlet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sor$routines.h>
#include <Xm/Text.h>
#include "vtoolkit.h"
#include "vlist.h"
#include "vframe.h"
#include "vheader.h"

#define appClass		"vdiskquota_defaults"
#define appName			"VDiskQuota"
#define defWidth		500
#define defHeight		700
#define defMaxValue		300000
#define defRedPercent		90
#define defYellowPercent	50
#define uidFile			"vdiskquota.uid"

static XrmDatabase database = 0;
static MrmHierarchy mrmHierarchy = 0;

static int maxValue, yellowThreshold, redThreshold;
static int updateInProgress = 0;

static Widget widgetA[100];
#define appW			(widgetA[0])
#define vListW			(widgetA[1])
#define mainW			(widgetA[2])
#define devSTextW		(widgetA[3])
#define histDialogW		(widgetA[4])
#define histMaxSTextW		(widgetA[5])
#define histRedSTextW		(widgetA[6])
#define histYellowSTextW	(widgetA[7])
#define updateButtonW		(widgetA[8])

#define COLOR_foreground	0
#define COLOR_red		1
#define COLOR_orange		2
#define COLOR_yellow		3
#define COLOR_green		4
#define COLOR_blue		5
#define COLOR_magenta		6

typedef struct {
    unsigned long	dqf$l_flags;
    unsigned long	dqf$l_uic;
    unsigned long	dqf$l_usage;
    unsigned long	dqf$l_permquota;
    unsigned long	dqf$l_overdraft;
    unsigned long	reservedA[3];
} Dqf;

typedef struct {
    unsigned long	filler1A[4];
    unsigned long	fib$l_wcc;
    unsigned short	filler2A[1];
    unsigned short	fib$w_cntrlfunc;
    unsigned long	fib$l_cntrlvalx;
    unsigned long	filler3A[9];
} Fib;

static int OkStatus(unsigned long status)
{
    return (status & 1);
}

static void CheckStatus(unsigned long status)
{
    if (!OkStatus (status)) sys$exit (status);
}

static void CreateCallback(Widget w, int *indexP, XmAnyCallbackStruct *unused_cbDataP)
{
    widgetA[*indexP] = w;
}

static int GetSTextInteger(Widget w)
{
    char *textP;
    int value;

    textP = (char *)XmTextGetString (w);
    value = atoi (textP);
    XtFree (textP);
    return (value);
}

static void SetSTextString(Widget w, char *valueP)
{
    if (valueP) XmTextSetString (w, valueP); 	/* set new contents */
    else        XmTextSetString (w, "");	/* clear the contents */
}

static void SetSTextInteger(Widget w, int value)
{
    char bufA[20];
    sprintf (bufA, "%d", value);
    SetSTextString (w, bufA);
}

static void GetHistValues(void)
{
    maxValue = GetSTextInteger (histMaxSTextW);
    if (!maxValue) maxValue = defMaxValue;
    SetSTextInteger (histMaxSTextW, maxValue);

    redThreshold = GetSTextInteger (histRedSTextW);
    if (!redThreshold) redThreshold = maxValue * defRedPercent / 100;
    if (redThreshold > maxValue) redThreshold = maxValue;
    SetSTextInteger (histRedSTextW, redThreshold);

    yellowThreshold = GetSTextInteger (histYellowSTextW);
    if (!yellowThreshold) yellowThreshold = maxValue * defYellowPercent / 100;
    if (yellowThreshold > redThreshold) yellowThreshold = redThreshold;
    SetSTextInteger (histYellowSTextW, yellowThreshold);
}

static void SetVListFields(int actualMaxValue)
{
    VListField fieldA[4];
    VListLabel labelA[4];
    VListGrid gridA[4];

    if (actualMaxValue < maxValue) actualMaxValue = maxValue;

    fieldA[0].mask = VListFieldAlignmentMask;
    fieldA[0].alignment = VListAlignmentBeginning;

    fieldA[1].mask = VListFieldAlignmentMask;
    fieldA[1].alignment = VListAlignmentEnd;

    gridA[0].mask = VListGridValueMask | VListGridDashMask;
    gridA[0].value = 0;
    gridA[0].dashOn = 255;
    gridA[0].dashOff = 1;

    gridA[1].mask = VListGridValueMask | VListGridColoridxMask;
    gridA[1].value = yellowThreshold;
    gridA[1].coloridx = COLOR_green;

    gridA[2].mask = VListGridValueMask | VListGridColoridxMask;
    gridA[2].value = redThreshold;
    gridA[2].coloridx = COLOR_red;

    gridA[3].mask = VListGridValueMask | VListGridDashMask;
    gridA[3].value = actualMaxValue;
    gridA[3].dashOn = 255;
    gridA[3].dashOff = 1;

    fieldA[2].mask = VListFieldTypeMask | VListFieldMinValueMask | VListFieldMaxValueMask | VListFieldGridsMask;
    fieldA[2].type = VListFieldTypeHist;
    fieldA[2].minValue = 0;
    fieldA[2].maxValue = actualMaxValue;
    fieldA[2].gridsP = gridA;
    fieldA[2].gridCount = XtNumber (gridA);

    fieldA[3].mask = VListFieldAlignmentMask;
    fieldA[3].alignment = VListAlignmentEnd;

    labelA[0].mask = VListLabelTextMask | VListLabelRowMask | VListLabelAlignmentMask | VListLabelFirstFieldMask
	| VListLabelPadMask;
    labelA[0].textP = "UIC";
    labelA[0].row = 1;
    labelA[0].alignment = VListAlignmentCenter;
    labelA[0].firstField = 0;
    labelA[0].pad = VListLabelPadWithBlanks;

    labelA[1].mask = VListLabelTextMask | VListLabelRowMask | VListLabelAlignmentMask | VListLabelFirstFieldMask
 	| VListLabelPadMask;
    labelA[1].textP = "Usage";
    labelA[1].row = 1;
    labelA[1].alignment = VListAlignmentCenter;
    labelA[1].firstField = 1;
    labelA[1].pad = VListLabelPadWithBlanks;

    labelA[2].mask = VListLabelTextMask | VListLabelRowMask | VListLabelAlignmentMask | VListLabelFirstFieldMask
 	| VListLabelPadMask;
    labelA[2].textP = "Quota";
    labelA[2].row = 1;
    labelA[2].alignment = VListAlignmentCenter;
    labelA[2].firstField = 3;
    labelA[2].pad = VListLabelPadWithBlanks;

    labelA[3].mask = VListLabelTextMask | VListLabelRowMask | VListLabelAlignmentMask | VListLabelFirstFieldMask
	| VListLabelLastFieldMask | VListLabelPadMask;
    labelA[3].textP = "Disk Usage in Blocks";
    labelA[3].row = 0;
    labelA[3].alignment = VListAlignmentCenter;
    labelA[3].firstField = 0;
    labelA[3].lastField = 3;
    labelA[3].pad = VListLabelPadWithDashes;

    XtVaSetValues (vListW,
	VListNfieldCount, 	XtNumber (fieldA),
	VListNfields, 		fieldA,
	VListNlabelCount, 	XtNumber (labelA),
	VListNlabels, 		labelA,
	0);
}

static int ValueToColoridx(int value)
{
    if (value < yellowThreshold) return (COLOR_green);
    if (value < redThreshold) return (COLOR_yellow);
    return (COLOR_red);
}

static void GetIdentifierText(unsigned long uic, char *uicP, int bufLen)
{
    struct dsc$descriptor faoDsc, outDsc;
    unsigned short outLen;

    faoDsc.dsc$b_dtype = DSC$K_DTYPE_T;
    faoDsc.dsc$b_class = DSC$K_CLASS_S;
    faoDsc.dsc$a_pointer = "!%I";
    faoDsc.dsc$w_length = 3;

    outDsc.dsc$b_dtype = DSC$K_DTYPE_T;
    outDsc.dsc$b_class = DSC$K_CLASS_S;
    outDsc.dsc$a_pointer = uicP;
    outDsc.dsc$w_length = bufLen;

    CheckStatus (sys$fao (&faoDsc, &outLen, &outDsc, uic));

    uicP[outLen] = 0;
}

static void Beep(void)
{
    XBell (XtDisplay (appW), 0);
}

static void AddOneLine(unsigned long uic, unsigned long usage, unsigned long quota)
{
    VListBar vBarA[2];
    VListLine vLineA[4];	
    char uicA[128], usageA[16], quotaA[16];

    GetIdentifierText (uic, uicA, sizeof (uicA) - 1);
    vLineA[0].mask = VListLineTextMask;
    vLineA[0].textP = uicA;

    sprintf (usageA, "%d", usage);
    vLineA[1].mask = VListLineTextMask;
    vLineA[1].textP = usageA;

    vBarA[0].mask = VListBarLowValueMask | VListBarHighValueMask | VListBarStippledMask | VListBarColoridxMask;
    vBarA[0].lowValue = usage;
    vBarA[0].highValue = quota;
    vBarA[0].stippled = 1;
    vBarA[0].coloridx = COLOR_foreground;

    vBarA[1].mask = VListBarLowValueMask | VListBarHighValueMask;
    vBarA[1].lowValue = 0;
    vBarA[1].highValue = usage;

    vLineA[2].mask = VListLineColoridxMask | VListLineBarsMask;
    vLineA[2].barsP = vBarA;
    vLineA[2].barCount = XtNumber (vBarA);
    vLineA[2].coloridx = ValueToColoridx (usage);

    sprintf (quotaA, "%d", quota);
    vLineA[3].mask = VListLineTextMask;
    vLineA[3].textP = quotaA;

    VListAddLine (vListW, vLineA, VListAddAtBottom);
}

static int ParseDeviceNames(char *devP, char *parsedDevP)
{
    struct FAB fab;
    struct NAM nam;
    char esaA[256];
    int c, len;
    int devCount = 0;

    fab = cc$rms_fab;
    fab.fab$l_nam = &nam;

    nam = cc$rms_nam;
    nam.nam$l_esa = esaA;
    nam.nam$b_ess = 255;
    nam.nam$b_nop |= NAM$M_SYNCHK;

    while (1) {
	fab.fab$l_fna = devP;

	/* Gobble up to null or comma and leave separator in c. */

	for (len = 0; (c = (*(devP++))) && (c != ','); len++) {}
	fab.fab$b_fns = len;
    
	if (!OkStatus (sys$parse (&fab))) {		/* problem? */
	    nam.nam$l_dev = fab.fab$l_fna;		/* use original */
	    nam.nam$b_dev = fab.fab$b_fns;
	}
    
	memcpy (parsedDevP, nam.nam$l_dev, nam.nam$b_dev);
        parsedDevP += nam.nam$b_dev;
	devCount++;

	if (!((*(parsedDevP++)) = c)) break;		/* copy separator */
    }

    return (devCount);
}    

static unsigned short OpenChannel(char *devP, int len)
{
    unsigned short chan;
    struct dsc$descriptor dsc;

    dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    dsc.dsc$b_class = DSC$K_CLASS_S;
    dsc.dsc$a_pointer = devP;
    dsc.dsc$w_length = len;

    if (!OkStatus (sys$assign (&dsc, &chan, 0, 0))) return (0);

    return (chan);
}

static void SetUpdateLabel(char *valueP, int flush)
{
    char *xValueP;

    xValueP = (char *)XmStringCreate (valueP, XmSTRING_DEFAULT_CHARSET);
    XtVaSetValues (updateButtonW, XmNlabelString, xValueP, 0);
    XtFree (xValueP);

    if (flush) XFlush (XtDisplay (appW));
}

static void HandlePendingEvents(void)
{
    XEvent event;
    int doFlush = 0;

    while (XEventsQueued (XtDisplay (appW), QueuedAlready)) {
	XNextEvent (XtDisplay (appW), &event);
	XtDispatchEvent (&event);
	doFlush = 1;
    }

    if (doFlush) XFlush (XtDisplay (appW));
}

#define ReleaseRecord							\
    if (uic) {								\
        dstDqf.dqf$l_uic = uic;						\
        dstDqf.dqf$l_usage = usage;					\
	dstDqf.dqf$l_permquota = quota;					\
        CheckStatus (sor$release_rec (&sortRecDsc, &uicSortClosure));	\
        newCount++;							\
        if (actualMaxValue < usage) actualMaxValue = usage;		\
    }

static void UpdateCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    char parsedDevA[128], *devP, *tDevP;
    unsigned long status, uic, newUic;
    unsigned short chan, iosbA[4], sortKeyA[1+4];
    Fib fib;
    Dqf srcDqf, dstDqf;
    struct dsc$descriptor fibDsc, srcDsc, dstDsc, sortRecDsc;
    int i, c, len, devCount, usage, newUsage, quota, newQuota;
    unsigned long sortClosure = 0;
    unsigned long uicSortClosure = 0;
    unsigned short lrl = (unsigned short) sizeof (Dqf);
    int count = 0;
    int newCount = 0;
    int actualMaxValue = 0;

    if (updateInProgress) {Beep (); return;}

    updateInProgress = 1;
    SetUpdateLabel ("Reading...", 1);

    devP = (char *)XmTextGetString (devSTextW);
    devCount = ParseDeviceNames (devP, parsedDevA);
    XtFree (devP);
    devP = parsedDevA;
    SetSTextString (devSTextW, devP);

    GetHistValues ();
    VListRemoveAllLines (vListW);

    fib.fib$w_cntrlfunc = FIB$C_EXA_QUOTA;
    fib.fib$l_cntrlvalx = FIB$M_ALL_MEM | FIB$M_ALL_GRP;
    fib.fib$l_wcc = 0;

    fibDsc.dsc$w_length = sizeof (Fib);
    fibDsc.dsc$a_pointer = (char *)&fib;

    srcDsc.dsc$w_length = sizeof (Dqf);
    srcDsc.dsc$a_pointer = (char *)&srcDqf;

    dstDsc.dsc$w_length = sizeof (Dqf);
    dstDsc.dsc$a_pointer = (char *)&dstDqf;

    sortRecDsc.dsc$w_length = sizeof (Dqf);
    sortRecDsc.dsc$a_pointer = (char *)&dstDqf;
    sortRecDsc.dsc$b_dtype = DSC$K_DTYPE_T;
    sortRecDsc.dsc$b_class = DSC$K_CLASS_S;

    sortKeyA[0] = 1;
    sortKeyA[1] = (unsigned short) DSC$K_DTYPE_LU;
    sortKeyA[4] = (unsigned short) sizeof (unsigned long);

    if (devCount == 1) {
        sortKeyA[2] = (unsigned short) 1;	/* descending */
        sortKeyA[3] = (unsigned short) 8;	/* offset of dqf$l_usage */
    } else {
        sortKeyA[2] = (unsigned short) 0;	/* ascending */
        sortKeyA[3] = (unsigned short) 4;	/* offset of dqf$l_uic */
    }

    CheckStatus (sor$begin_sort (sortKeyA, &lrl, 0, 0, 0, 0, 0, 0, &sortClosure));

    while (1) {
        for (tDevP = devP, len = 0; (c = (*(tDevP++))) && (c != ','); len++) {}
        chan = OpenChannel (devP, len);
        devP += (len + 1);

        if (chan) {
	    while (1) {
		status = sys$qiow (
		    0,						/* efn */
		    chan,					/* chan */
		    IO$_ACPCONTROL,				/* func */
		    iosbA,					/* iosb */
		    0, 0,					/* ast */
		    &fibDsc, &srcDsc, 0, &dstDsc, 0, 0);	/* p1 : p6 */
		
		if (OkStatus (status)) status = iosbA[0];
		if (!OkStatus (status)) break;
	
		CheckStatus (sor$release_rec (&sortRecDsc, &sortClosure));

		if (actualMaxValue < dstDqf.dqf$l_usage) actualMaxValue = dstDqf.dqf$l_usage;

		count++;

		HandlePendingEvents ();
	    }

	    CheckStatus (sys$dassgn (chan));
	}

	if (!c) break;
    }

    SetUpdateLabel ("Sorting...", 1);

    CheckStatus (sor$sort_merge (&sortClosure));

    HandlePendingEvents ();

    if (devCount != 1) {
        sortKeyA[2] = (unsigned short) 1;	/* descending */
        sortKeyA[3] = (unsigned short) 8;	/* offset of dqf$l_usage */

	CheckStatus (sor$begin_sort (sortKeyA, &lrl, 0, 0, 0, 0, 0, 0, &uicSortClosure));

	uic = 0;
	for (i = 0; i < count; i++) {
	    CheckStatus (sor$return_rec (&sortRecDsc, 0, &sortClosure));

	    newUic = dstDqf.dqf$l_uic;
	    newUsage = dstDqf.dqf$l_usage;
	    newQuota = dstDqf.dqf$l_permquota;

	    if (newUic == uic) {
		usage += newUsage;
		quota += newQuota;
	    } else {
		ReleaseRecord

		uic = newUic;
		usage = newUsage;
		quota = newQuota;
	    }

	    HandlePendingEvents ();
	}

	ReleaseRecord

	CheckStatus (sor$end_sort (&sortClosure));
	CheckStatus (sor$sort_merge (&uicSortClosure));

	sortClosure = uicSortClosure;
	count = newCount;

	HandlePendingEvents ();
    }

    SetUpdateLabel ("Loading...", 1);

    SetVListFields (actualMaxValue);

    VListStartBatchUpdate (vListW);

    for (i = 0; i < count; i++) {
	CheckStatus (sor$return_rec (&sortRecDsc, 0, &sortClosure));
	AddOneLine (dstDqf.dqf$l_uic, dstDqf.dqf$l_usage, dstDqf.dqf$l_permquota);
	HandlePendingEvents ();
    }

    CheckStatus (sor$end_sort (&sortClosure));

    VListMakeLineVisible (vListW, VListFieldRowToLine (vListW, 0, 0), VListVisibleAtTop);

    VListEndBatchUpdate (vListW);

    updateInProgress = 0;
    SetUpdateLabel ("Update", 0);
}

static void HistDialogCancelCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    XtUnmanageChild (histDialogW);
}

static void HistDialogOkCallback(Widget w, Opaque closure, XmAnyCallbackStruct *cbDataP)
{
    XtUnmanageChild (histDialogW);
    UpdateCallback (w, closure, cbDataP);
}

static void CustomizeHistogramCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    if (XtIsManaged (histDialogW)) XRaiseWindow (XtDisplay (histDialogW), XtWindow (XtParent (histDialogW)));
    else 			   XtManageChild (histDialogW);
}

static void SaveSettingsCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    VtkSaveNumericResource (&database, "vdiskquota_defaults.x", 	appW->core.x);
    VtkSaveNumericResource (&database, "vdiskquota_defaults.y", 	appW->core.y);
    VtkSaveNumericResource (&database, "*vdqMain.width", 		mainW->core.width);
    VtkSaveNumericResource (&database, "*vdqMain.height", 		mainW->core.height);
    VtkSaveNumericResource (&database, "*vdqHistMaxSText.value", 	maxValue);
    VtkSaveNumericResource (&database, "*vdqHistYellowSText.value", 	yellowThreshold);
    VtkSaveNumericResource (&database, "*vdqHistRedSText.value", 	redThreshold);

    XrmPutFileDatabase (database, VtkGetDefaultsName (appW));
}

static void QuitCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    exit (1);
}

static MrmRegisterArg mrmRegisterArgs[] = {
    {"VdqCreateCallback",		(char *)CreateCallback},
    {"VdqCustomizeHistogramCallback",	(char *)CustomizeHistogramCallback},
    {"VdqHistDialogCancelCallback",	(char *)HistDialogCancelCallback},
    {"VdqHistDialogOkCallback",		(char *)HistDialogOkCallback},
    {"VdqQuitCallback",			(char *)QuitCallback},
    {"VdqSaveSettingsCallback",		(char *)SaveSettingsCallback},
    {"VdqUpdateCallback",		(char *)UpdateCallback},
    };

main(int argc, char **argv)
{
    Widget dummyW;
    MrmCode dummyC;

    MrmInitialize ();			/* register toolkit widgets */
    VListInitializeForMRM ();		/* register VList widget */
    VFrameInitializeForMRM ();		/* register VFrame widget */
    VHeaderInitializeForMRM ();		/* register VHeader widget */

    /* Initialize the application. */

    if (!VtkInitialize (appName, appClass, &argc, argv, uidFile, mrmRegisterArgs, XtNumber (mrmRegisterArgs), &mrmHierarchy, &appW))
	return;

    /* Fetch the UI objects. */

    (void)MrmFetchWidget (mrmHierarchy, "vdqMain", appW, &dummyW, &dummyC);

    /* Load the resource database.  If there is none, set the defaults. */

    if (!(database = XrmGetFileDatabase (VtkGetDefaultsName (appW))))
	XtVaSetValues (mainW,
	    XtNwidth,  defWidth,
	    XtNheight, defHeight,
	    0);

    /* Propagate main window's size to its parent (workaround TK bug). */

    XtVaSetValues (XtParent (mainW),
	XtNwidth,  mainW->core.width,
	XtNheight, mainW->core.height,
	0);

    XtManageChild (mainW);
    XtRealizeWidget (appW);

    XtMainLoop ();
}
