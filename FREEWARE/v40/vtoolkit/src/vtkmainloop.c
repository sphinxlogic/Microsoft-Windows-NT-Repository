/* vtkmainloop.c - Alternative mainloop with event compression

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains an alternative to XtMainLoop.  This main loop implements
a variety of event compression techniques.

MODIFICATION HISTORY:

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

03-Sep-1993 (sjk/nfc) VToolkit V2.

11-Apr-1991 (sjk) Integrate into VToolkit.

15-Nov-1988 (sjk) Initial entry.
*/

#include "vtoolkit.h"
#include "vtkmainloop.h"

int VtkEventCnt = 1; 	/* global (current) event number */

typedef struct _SavedExposeEvent {
    struct _SavedExposeEvent	*nextSavedEventP;
    XExposeEvent		event;
} SavedExposeEvent;

static SavedExposeEvent *firstSavedEventP = 0;
static SavedExposeEvent *lastSavedEventP = 0;

static int DoOldestExposeEvent(void)
{
    SavedExposeEvent *savedEventP, *otherSavedEventP;

    if (!(savedEventP = firstSavedEventP)) return (0);	/* list is empty */
    firstSavedEventP = savedEventP->nextSavedEventP;

    /* If there are any other saved expose events for this window, then
     * set the count to 1 to encourage exposure compression. */

    if (!savedEventP->event.count)
	for (otherSavedEventP=firstSavedEventP; otherSavedEventP; otherSavedEventP=otherSavedEventP->nextSavedEventP)
	    if (otherSavedEventP->event.window == savedEventP->event.window) {
		savedEventP->event.count = 1;
		break;
	    }

    XtDispatchEvent ((XEvent *)(&savedEventP->event));

    XtFree ((char *)savedEventP);

    if (!firstSavedEventP) return (0);	/* list is now empty */
    return (1);				/* there are more */
}

static Boolean WorkProc(XtPointer displayOP)
{
    Display *displayP = (Display *)displayOP;

    /* Process saved expose events.  If none remain, return.  If there are any
     * new X events, cut our work short and return. */

    while (1) {
        if (!DoOldestExposeEvent ()) return (1);			/* all done */
	if (XEventsQueued (displayP, QueuedAlready)) return (0); 	/* aborted */
    }
}

static void SaveExposeEvent(XExposeEvent *eventP)
{
    SavedExposeEvent *savedEventP = (SavedExposeEvent *)XtMalloc (sizeof (SavedExposeEvent));

    savedEventP->event = *eventP;

    if (!firstSavedEventP) {
        firstSavedEventP = savedEventP;
	XtAddWorkProc (WorkProc, eventP->display);
    } else lastSavedEventP->nextSavedEventP = savedEventP;

    lastSavedEventP = savedEventP;
    savedEventP->nextSavedEventP = 0;
}

static void DoEvent(XEvent *eventP)
{
    /* If it is an expose event, save it for the next idle moment. */

    if (eventP->type == Expose) {
	SaveExposeEvent ((XExposeEvent *)eventP);
	return;
    }

    /* Process all pending exposures if not MotionNotify event. */

    if (eventP->type != MotionNotify) while (DoOldestExposeEvent ()) ;

    /* Increment the global event counter and process the event. */

    VtkEventCnt++;
    XtDispatchEvent (eventP);
}

void VtkMainLoop(void)
{
    XEvent event;
    
    while (1) {
	XtNextEvent (&event);
	DoEvent (&event);

	while (XEventsQueued (event.xany.display, QueuedAlready)) {
	    XNextEvent (event.xany.display, &event);
	    DoEvent (&event);
	}
    }
}
