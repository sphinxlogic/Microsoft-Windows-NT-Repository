/* btrap.c - Broadcast message trapper - main program

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This program traps VMS broadcast messages and displays them in a scrolling TList
widget. It only works under the VMS operating system.

A common application requirement is to communicate information from an AST
routine to a MOTIF widget.  Because the MOTIF toolkit is not callable from
AST level, a mechanism must be implemented (by each application developer) to
queue information from AST routines to the MOTIF toolkit's main event loop.

VToolkit contains a module (vtkaddinput.c) to satisfy this common requirement.
It uses XtAddInput to transform completion ASTs (or any other kind of AST) into
a callback routine that is compatible with (and may call) the MOTIF toolkit.  

This module (Btrap.c) shows how to call these routines.  It also uses the
custom TList widget for display of the scrolling message list, and it shows
how to save simple customization (window geometry) settings in a resource
(.DAT) file.

MODIFICATION HISTORY:

03-Sep-1993 (sjk) V2.

14-Nov-1991 (sjk) Use VtkActivatePopupMenu routine.

14-Nov-1991 (sjk) Use VtkCreateCallback.

06-Nov-1991 (sjk) Cleanup.

16-Jul-1991 (sjk) Conditionalize popup button for MOTIF vs. XUI.

03-Apr-1991 (sjk) Upgrade to use MOTIF-compatible TList widget.

06-Sep-1990 (sjk) Upgrade to use the VtkAddInput routines instead of direct calls to XtAddInput.  The VtkAddInput module
	can be used with any application that needs to turn VMS completion ASTs into non-AST level routine callbacks that
	are compatible with the X-toolkit.

09-May-1990 (sjk) Bugfix - When XtMalloc did not return aligned memory,  XtFree was called with a different address
	than was returned by XtMalloc.

26-Mar-1990 (sjk) Use TListReset special routine to clear the message window.

31-Jan-1990 (sjk) Don't depend on XtMalloc returning aligned memory.

03-Nov-1989 (sjk) Initial entry.
*/

#include <descrip.h>
#include <jpidef.h>
#include <ssdef.h>
#include <iodef.h>
#include <libdef.h>
#include <dvidef.h>
#include <psldef.h>
#include <prcdef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <msgdef.h>
#include <starlet.h>
#include <stdlib.h>
#include <stdio.h>
#include <lib$routines.h>

#include "vtoolkit.h"
#include "tlist.h"
#include "vframe.h"
#include "vtkaddinput.h"

#define appName         "BTrap - Broadcast Message Trapper"
#define appClass        "btrap$defaults"
#define popupButton     Button3
#define uidFile         "btrap.uid"

static char *defaultsNameP = "decw$user_defaults:btrap$defaults.dat";

typedef struct {
    unsigned short	type;
    unsigned short	unit;
    unsigned char	controllerNameLen;
    char		controllerNameA[15];
    unsigned short	messageLen;
    char		messageA[256];
} VmsMailboxMessage;

/* Define a control block to contain information about the mailbox message.
 * This control block will be passed to the I/O completion routine. */

typedef struct _MessageRec {
    unsigned short	iosbA[4];
    VmsMailboxMessage	mailboxMessage;
} MessageRec;

static MessageRec messageRec;
static int bellPercent = 0;
static short devChan, mbChan;
static MrmHierarchy mrmHierarchy;
static XrmDatabase database;	/* only read it once */

static Widget widgetA[4];
#define appW		widgetA[0]
#define mainW		widgetA[1]
#define tListW		widgetA[2]
#define popupMenuW	widgetA[3]

static unsigned long StartReadQIO(MessageRec *messageRecP);

static void ProcessMessageRec(MessageRec *messageRecP)
{
    VmsMailboxMessage *mailboxMessageP = &messageRecP->mailboxMessage;
    int bell = 0;
    char c, bufA[256];
    char *fromBufP = mailboxMessageP->messageA;
    int fromBufLen = mailboxMessageP->messageLen;
    char *toBufP;

    /* If this is a non-null broadcast message, pass it to the TList widget. */

    if ((mailboxMessageP->type == MSG$_TRMBRDCST) && fromBufLen) {
	if (fromBufP[fromBufLen-1] != '\n') fromBufP[fromBufLen++] = '\n';
    
	while (fromBufLen) {
	    toBufP = bufA;
	    bell = 0;
    
	    while (1) {
		c = *(fromBufP++); fromBufLen--;
    
		if (c == 7) bell++;
		else if (c == '\t') *(toBufP++) = ' ';
		else if (c == '\n') {*toBufP = 0; break;}
		else *(toBufP++) = c;
	    }
    
	    /* Use the number of bell characters in the message to determine its color. */

	    if (bufA[0]) TListAddLine (tListW, bufA, 0, bell);

	    while (bell--) XBell (XtDisplay (tListW), bellPercent);
	}
    }

    /* Start another asynchronous read. */

    StartReadQIO (messageRecP);
}

static unsigned long StartReadQIO(MessageRec *messageRecP)
{
    unsigned long status;

    /* Start an asynchronous read on the broadcast mailbox.
     *
     * Always use VTK_MISC_EFN to avoid possible event flag conflicts.
     *
     * The IOSB should be embedded in the message control block so that
     * it will be available to the completion callback routine.
     *
     * Always use VtkCompletionAst as the ASTADR.  This completion AST
     * routine (which is part of vtkaddinput.c) will schedule your own
     * completion callback routine for execution at non-AST level as soon
     * as it is allowed by the X-toolkit.
     *
     * Use VtkAllocateAddInputRec in place of ASTPRM to set up your own
     * completion callback routine and its argument.
     */

    status = sys$qio (
	VTK_MISC_EFN,								/* always use this EFN */
	mbChan,									/* mailbox channel */
	IO$_READVBLK,								/* function code */
	messageRecP->iosbA,							/* IOSB (in message control block) */
	VtkCompletionAst,							/* always use this ASTADR */
	VtkAllocateAddInputRec (ProcessMessageRec, (Opaque)messageRecP),	/* callback and its argument */
	&messageRecP->mailboxMessage, 						/* buffer address */
	sizeof(VmsMailboxMessage), 						/* buffer length */
	0, 0, 0, 0);								/* unused QIO parameters */

    return (status);
}

typedef struct {
    short		bufferLength;
    short		itemCode;
    char		*bufP;
    unsigned short	*bufLenP;
} GetjpiItemList;

static unsigned long masterPid;
static GetjpiItemList masterPidItemListA[2] = {
    {4, JPI$_MASTER_PID, (char *)&masterPid, 0},
    {0, 0, 0, 0}};
    
static char devNameBufA[64];
static unsigned short devNameLen;
static GetjpiItemList devNameItemListA[2] = {
    {sizeof(devNameBufA)-1, JPI$_TERMINAL, devNameBufA, &devNameLen},
    {0, 0, 0, 0}};

#define Check(s)	if ((status = (s)) != SS$_NORMAL) return (status)

static unsigned long StartTrappingMessages(void)
{
    unsigned long status;
    unsigned long modeBufA[3];
    unsigned short dviBufA[2];
    unsigned short iosbA[4];
    struct dsc$descriptor_s devNameDsc;
    int maximumMessageSize;
    int bufferQuota;
    char dummyBufA[4];
   
    /* Get the terminal name owned by the master process of our job tree. */

    Check (sys$getjpiw (VTK_MISC_EFN, 0, 0, masterPidItemListA, iosbA, 0, 0));
    Check (iosbA[0]);

    Check (sys$getjpiw (VTK_MISC_EFN, &masterPid, 0, devNameItemListA, iosbA, 0, 0));
    Check (iosbA[0]);

    /* Assign a channel (with mailbox) to that terminal device, and enable the mailbox so that messages will be sent to it. */

    devNameDsc.dsc$w_length  = devNameLen;
    devNameDsc.dsc$b_dtype   = DSC$K_DTYPE_T;
    devNameDsc.dsc$b_class   = DSC$K_CLASS_S;
    devNameDsc.dsc$a_pointer = devNameBufA;

    maximumMessageSize = sizeof(VmsMailboxMessage);
    bufferQuota = sizeof(VmsMailboxMessage)*32;
    Check (lib$asn_wth_mbx (&devNameDsc, &maximumMessageSize, &bufferQuota, &devChan, &mbChan));

    Check (sys$qiow (VTK_MISC_EFN, devChan, IO$_WRITEVBLK | IO$M_ENABLMBX, iosbA, 0, 0, dummyBufA, 0, 0, 0, 0, 0));
    Check (iosbA[0]);

    /* Set the terminal NOBROADCAST since messages will be displayed in our window. */

    Check (sys$qiow (VTK_MISC_EFN, devChan, IO$_SENSEMODE, iosbA, 0, 0, modeBufA, sizeof(modeBufA), 0, 0, 0, 0));
    Check (iosbA[0]);

    modeBufA[1] |= TT$M_NOBRDCST;
    modeBufA[2] |= TT2$M_BRDCSTMBX;
    Check (sys$qiow (VTK_MISC_EFN, devChan, IO$_SETMODE, iosbA, 0, 0, modeBufA, sizeof(modeBufA), 0, 0, 0, 0));
    Check (iosbA[0]);

    /* Start the first asynchronous mailbox read. */

    Check (StartReadQIO (&messageRec));

    return (SS$_NORMAL);
}

static void CreateTListCallback(Widget w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    tListW = w;		/* save the ID of the TList widget for later use */
}

static void SaveSettingsCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    if (!database) database = XrmGetFileDatabase (defaultsNameP);

    VtkSaveNumericResource (&database, "btrap$defaults.x", appW->core.x);
    VtkSaveNumericResource (&database, "btrap$defaults.y", appW->core.y);
    VtkSaveNumericResource (&database, "*bTrapMain.width", mainW->core.width);
    VtkSaveNumericResource (&database, "*bTrapMain.height", mainW->core.height);

    XrmPutFileDatabase (database, defaultsNameP);
}

static void ResetCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    TListReset (tListW);
}

static void QuitCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    exit (1);
}

static void ButtonPressHandler(Widget unused_w, Opaque unused_closure, XButtonEvent *eventP)
{
    if (eventP->button != popupButton) return;

    VtkActivatePopupMenu (popupMenuW, (XEvent *)eventP);
}

static MrmRegisterArg mrmRegisterArgs[] = {
    {"BTrapCreateTListCallback",	(char *)CreateTListCallback},
    {"BTrapResetCallback",		(char *)ResetCallback},
    {"BTrapSaveSettingsCallback",	(char *)SaveSettingsCallback},
    {"BTrapQuitCallback",		(char *)QuitCallback},
};

main(int argc, char **argv)
{
    unsigned long status;

    MrmInitialize ();			/* initialize MRM */
    TListInitializeForMRM ();		/* tell MRM about the TList widget class */
    VFrameInitializeForMRM ();     	/* tell MRM about the VFrame widget class */

    /* Initialize the application. */

    if (!VtkInitialize (appName, appClass, &argc, argv, uidFile, mrmRegisterArgs, XtNumber (mrmRegisterArgs), &mrmHierarchy, &appW))
        return;

    if ((status = StartTrappingMessages ()) != SS$_NORMAL) {
        printf ("BTrap - Unable to trap broadcast messages");
    	return (status);
    }

    (void)VtkFetchWidget (mrmHierarchy, "bTrapMain", appW, widgetA, 0);
    XtManageChild (mainW);			/* manage the main window */

    /* If it is an unreasonable size, give it the default. */

    if (mainW->core.height < 100)
	XtVaSetValues (mainW,
	    XtNwidth,	400,
	    XtNheight, 	150,
	    0);

    XtRealizeWidget (appW);			/* realize the widget tree */

    /* Because of a toolkit restriction, the popup menu cannot be fetched until its parent has been realized. */

    (void)VtkFetchWidget (mrmHierarchy, "bTrapPopupMenu", mainW, widgetA, 0);
    XtAddEventHandler (mainW, ButtonPressMask, 0, ButtonPressHandler, 0);

    XtMainLoop ();				/* and go to work */
}
