/* vdrag.c - Movable windows utility module

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contain utility routines that support dragging windows around within their parents.

MODIFICATION HISTORY:

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

06-Sep-1993 (sjk/nfc) Run through the MOTIF porting tools, functions
	changed to prototype definitions, DragContext structure moved to vdrag.h.

07-Nov-1991 (sjk) Add VDragEnableGrab and VDragDisableGrab routines to allow application control of button and modifiers.

16-Jul-1991 (sjk) Add VDragDisable entrypoint.

23-May-1991 (sjk) Bugfix - Use DragButton symbol instead of hard-coded button number so that this code works under MOTIF.

22-May-1990 (sjk) Conditionalize button bindings to support MOTIF style.

17-May-1990 (sjk) Change to X-standard drag cursor.

05-Apr-1990 (sjk) Add MOTIF support.

21-Feb-1990 (sjk) Change decw$cursor include file spec.

31-May-1989 (sjk) Extract from FileView's VShell widget.
*/

#include "vtoolkit.h"
#include "vdrag.h"
#include <X11/cursorfont.h>

static int dragIsActive = 0;
static int windowX, windowY, xGrabPoint, yGrabPoint;
static Cursor dragCursor = 0;
static Window dragWindow;

typedef struct {
    Widget      w;
    int         button;
    int         modifiers;
} DragContext;

static void ButtonPressHandler(Widget w, XtPointer dragContextOP, XEvent *eventOP, Boolean *unused_continueP)
{
    DragContext *dragContextP = (DragContext *)dragContextOP;
    XButtonEvent *eventP = (XButtonEvent *)eventOP;
    Window window, subwindow, newSubwindow;
    int x, y;
    int dummyRoot, dummyWidth, dummyHeight, dummyBorderWidth, dummyDepth;

    /* Are we already dragging?  Do a chorded cancel. */

    if (dragIsActive) {
	dragIsActive = 0; 
	return;
    }

    /* Wrong modifiers?  Ignore it. */

    if ((dragContextP->modifiers != AnyModifier) && (dragContextP->modifiers & (~eventP->state))) return;

    /* Wrong button?  Ignore it. */

    if (eventP->button != dragContextP->button) return;

    /* Iterate, until the lowest-level window containing the event is found. */

    x         = eventP->x;
    y         = eventP->y;
    window    = eventP->window;
    subwindow = eventP->subwindow;

    while (subwindow) {
	XTranslateCoordinates (eventP->display, window, subwindow, x, y, &x, &y, &newSubwindow);
	
	window    = subwindow;
	subwindow = newSubwindow;
    }

    /* Get the window position. */

    dragWindow = window;

    XGetGeometry (eventP->display, window, (unsigned long *) &dummyRoot, &windowX, &windowY,
		(unsigned int *) &dummyWidth, (unsigned int *) &dummyHeight, 
		(unsigned int *) &dummyBorderWidth, (unsigned int *) &dummyDepth);

    /* Keep track of dragged widget and the relative position of the pointer within that widget. */

    xGrabPoint = eventP->x - windowX;
    yGrabPoint = eventP->y - windowY;

    dragIsActive = 1;
}

static void ButtonMotionHandler(Widget parentW, XtPointer dragContextOP, XEvent *eventOP, Boolean *unused_continueP)
{
    DragContext *dragContextP = (DragContext *)dragContextOP;
    XMotionEvent *eventP = (XMotionEvent *)eventOP;
    XWindowChanges changes;
    int changeMask = 0;
    XEvent event;

    /* Extraneous motion event?  Ignore it. */

    if (!dragIsActive) return;

    /* Poor-man's motion compression.  Scan event queue until we have the most recent motion event. */

    event.xmotion = *eventP;
    while (XCheckWindowEvent (eventP->display, eventP->window, ButtonMotionMask, &event) == True) ;

    /* If the pointer has moved, update the window location. */

    if (windowX != event.xmotion.x - xGrabPoint) {
	changes.x = windowX = event.xmotion.x - xGrabPoint;
	changeMask |= CWX;
    }

    if (windowY != event.xmotion.y - yGrabPoint) {
	changes.y = windowY = event.xmotion.y - yGrabPoint;
	changeMask |= CWY;
    }

    if (changeMask) XConfigureWindow (eventP->display, dragWindow, changeMask, &changes);

    /* Synch with the server, to help motion compression do its thing. */

    XSync (event.xmotion.display, 0);
}

static void ButtonReleaseHandler(Widget w, XtPointer unused_closure, XEvent *unused_eventP, Boolean *unused_continueP)
{
    dragIsActive = 0;
}

Opaque VDragEnableGrab(Widget w, int button, int modifiers)
{
    DragContext *dragContextP;

    if (!dragCursor) dragCursor = XCreateFontCursor (XtDisplay (w), XC_fleur);

    XGrabButton (
	XtDisplay (w),							/* display */
	button,								/* button_grab */
	modifiers,							/* modifiers */
	XtWindow (w),							/* window */
	0,								/* owner_events */
	(ButtonPressMask | ButtonMotionMask | ButtonReleaseMask),	/* event_mask */
	GrabModeAsync,							/* pointer_mode */
	GrabModeAsync,							/* keyboard_mode */
	None,								/* confine_to */
	dragCursor);							/* cursor */

    dragContextP = (DragContext *)XtMalloc (sizeof (DragContext));
    dragContextP->w		= w;
    dragContextP->button 	= button;
    dragContextP->modifiers 	= modifiers;

    XtAddRawEventHandler (w, ButtonPressMask,   0, ButtonPressHandler,   dragContextP);
    XtAddRawEventHandler (w, ButtonMotionMask,  0, ButtonMotionHandler,  dragContextP);
    XtAddRawEventHandler (w, ButtonReleaseMask, 0, ButtonReleaseHandler, dragContextP);

    return ((Opaque)dragContextP);
}

void VDragDisableGrab(Opaque dragContextOP)
{
    DragContext *dragContextP = (DragContext *)dragContextOP;
    Widget w = dragContextP->w;

    XUngrabButton (
	XtDisplay (w),                                  	/* display */
        dragContextP->button,					/* button_grab */
        dragContextP->modifiers,				/* modifiers */
        XtWindow (w));                                  	/* window */

    XtRemoveRawEventHandler (w, ButtonPressMask,   0, ButtonPressHandler,   dragContextP);
    XtRemoveRawEventHandler (w, ButtonMotionMask,  0, ButtonMotionHandler,  dragContextP);
    XtRemoveRawEventHandler (w, ButtonReleaseMask, 0, ButtonReleaseHandler, dragContextP);
}

/* Historical routines */

#define DragButton 		Button2
#define DragMask		(ShiftMask | ControlMask)

static Opaque defaultDragContextP;

void VDragEnable(Widget w)
{
    defaultDragContextP = VDragEnableGrab (w, DragButton, DragMask);
}

void VDragDisable(Widget unused_w)
{
    VDragDisableGrab (defaultDragContextP);
}
