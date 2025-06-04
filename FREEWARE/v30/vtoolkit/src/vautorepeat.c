/* vautorepeat.c - Auto-repeating push-button utility module

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module implements auto-repeating pushbuttons.

MODIFICATION HISTORY:

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

03-Sep-1993 (sjk/nfc) VToolkit V2.

06-Nov-1991 (sjk) More cleanup.

05-Feb-1991 (sjk) Reformat.

09-Mar-1990 (sjk) Split from testvlist program.
*/

#include "vtoolkit.h"
#include "vautorepeat.h"

/* Define a structure to contain information about the auto-repeat. */

typedef struct {
    int			delay;		/* msecs until first repeat */
    int			initial;	/* initial interval in msecs */
    int			final;		/* final interval in msecs */
    int			accel;		/* acceleration rate in msecs per */
    Widget		w;		/* push button widget ID */
    int			next;		/* next timeout in msecs */
    XtIntervalId	timeout;	/* timeout ID */
} RepeatingButtonContext;

static void ButtonTimeoutCallback(XtPointer ctxOP, XtIntervalId *unused_idP)
{
    RepeatingButtonContext *ctxP = (RepeatingButtonContext *)ctxOP;

    /* This routine is called when the auto-repeat timer goes off.
     * Call the pushbutton's activate callback and start another timer. */

    XtCallCallbacks (ctxP->w, XmNactivateCallback, ctxP);

    /* Start another timeout with the interval already computed. */

    ctxP->timeout = XtAddTimeOut (ctxP->next, ButtonTimeoutCallback, ctxP);

    /* Compute the next interval.  Decrement by the acceleration rate, but don't go any faster than the final rate. */

    ctxP->next -= ctxP->accel;
    if (ctxP->next < ctxP->final) ctxP->next = ctxP->final;
}

static void ButtonArmCallback(Widget unused_ow, XtPointer ctxOP, XtPointer unused_cbDataP)
{
    RepeatingButtonContext *ctxP = (RepeatingButtonContext *)ctxOP;

    /* This routine is called when the user pressed down on the pushbutton.
     * Start the first auto-repeat timer.  Set the next timeout interval and wait for the first delay to pass. */

    ctxP->next    = ctxP->initial;
    ctxP->timeout = XtAddTimeOut (ctxP->delay, ButtonTimeoutCallback, ctxP);
}

static void ButtonDisarmCallback(Widget unused_ow, XtPointer ctxOP, XtPointer unused_cbDataP)
{
    RepeatingButtonContext *ctxP = (RepeatingButtonContext *)ctxOP;

    /* This routine is called when the user releases or drags out of the pushbutton. Cancel the outstanding auto-repeat timer. */

    XtRemoveTimeOut (ctxP->timeout);
}

void VAutoRepeatEnable(Widget w, int delay, int initial, int final, int accel)
{
    RepeatingButtonContext *ctxP = (RepeatingButtonContext *)XtMalloc (sizeof (RepeatingButtonContext));

    ctxP->delay 	= delay;
    ctxP->initial 	= initial;
    ctxP->final 	= final;
    ctxP->accel 	= accel;
    ctxP->w 		= w;

    XtAddCallback (w, XmNarmCallback,    ButtonArmCallback,    ctxP);
    XtAddCallback (w, XmNdisarmCallback, ButtonDisarmCallback, ctxP);
}

static void EnableRepeatingButton(Widget w, unsigned char timesA[4], XmAnyCallbackStruct *unused_cbDataP)
{
    VAutoRepeatEnable (w, timesA[0], timesA[1], timesA[2], timesA[3]);
}

static MrmRegisterArg reglist[] =
    {{"EnableRepeatingButton", (char *)EnableRepeatingButton}};

void VAutoRepeatInitializeForMRM(void)
{
    (void)MrmRegisterNames (reglist, XtNumber (reglist));
}
