/* vbreak.c - Break window hierarchy

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This program is an example of how to manipulate the X window tree.
By pressing MB3 (MB2 for MOTIF) while holding down the shift and
control keys, any window can be broken out of its position in the
window hierarchy and controlled independently.

You can escape from VBreak by dragging the mouse to the upper left-hand
corner of the screen and clicking shift-control-MB3 (MB2 for MOTIF).

MODIFICATION HISTORY:

09-Sep-1993 (sjk) VToolkit V2.

21-Feb-1993 (sjk) Initial entry.
*/

#include <stdlib.h>
#include "vtoolkit.h"
#include "vfake.h"
#include <X11/cursorfont.h>

#define DragButton 		Button2
#define DragMask		(ShiftMask | ControlMask)

typedef struct {
    Widget 	w;
    int		button;
    int		modifiers;
} DragContext;

typedef struct _MungedWindow {
    struct _MungedWindow	*nextP;
    Window			win;
    Window			parentWin;
    int				x;
    int				y;
} MungedWindow;

static int dragIsActive = 0;
static int windowX, windowY, xGrabPoint, yGrabPoint;
static Cursor dragCursor = 0;
static Window dragWindow;
static MungedWindow *firstMungedWindowP;

static void CleanupAndExit(Display *dpy)
{
    MungedWindow *mungedWindowP;

    for (mungedWindowP = firstMungedWindowP; mungedWindowP; mungedWindowP = mungedWindowP->nextP)
	XReparentWindow (dpy, mungedWindowP->win, mungedWindowP->parentWin, mungedWindowP->x, mungedWindowP->y);

    XFlush (dpy);

    exit (0);
}

static void ButtonPressHandler(Widget w, DragContext *dragContextP, XButtonEvent *eventP)
{
    Window window, subwindow, newSubwindow, rootWindow;
    Widget trackW;
    int x, y, rootX, rootY;
    unsigned int dummyWidth, dummyHeight, dummyBorderWidth, dummyDepth;
    Window dummyChild;
    XSetWindowAttributes attr;
    MungedWindow *mungedWindowP;
    Window *childrenWindowsP;
    unsigned int childrenWindowCount;

    /* Are we already dragging?  Do a chorded cancel. */

    if (dragIsActive) {
	dragIsActive = 0; 
	return;
    }

    if (eventP->button != DragButton) return;

    /* quit when user clicks in upper left-hand corner of screen */

    if ((!eventP->x) && (!eventP->y)) CleanupAndExit (eventP->display);

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

    XGetGeometry (eventP->display, dragWindow, &rootWindow, &windowX, &windowY,
	&dummyWidth, &dummyHeight, &dummyBorderWidth, &dummyDepth);

    XTranslateCoordinates (eventP->display, dragWindow, rootWindow, 0, 0, &rootX, &rootY, &dummyChild);

    XUnmapWindow (eventP->display, window);

    attr.override_redirect = 1;
    XChangeWindowAttributes (eventP->display, window, CWOverrideRedirect, &attr);

    mungedWindowP = (MungedWindow *)XtCalloc (sizeof (MungedWindow), 1);
    mungedWindowP->nextP = firstMungedWindowP;
    firstMungedWindowP = mungedWindowP;

    mungedWindowP->win = window;

    XGetGeometry (eventP->display, window, &rootWindow, &mungedWindowP->x, &mungedWindowP->y,
	&dummyWidth, &dummyHeight, &dummyBorderWidth, &dummyDepth);

    XQueryTree (eventP->display, window, &rootWindow, &mungedWindowP->parentWin, &childrenWindowsP, &childrenWindowCount);
    XFree ((char *)childrenWindowsP);

    XReparentWindow (eventP->display, window, rootWindow, rootX, rootY);

    XMapRaised (eventP->display, window);

    XGetGeometry (eventP->display, dragWindow, &rootWindow, &windowX, &windowY,
	&dummyWidth, &dummyHeight, &dummyBorderWidth, &dummyDepth);

    /* Keep track of dragged widget and the relative position of the pointer within that widget. */

    xGrabPoint = eventP->x - windowX;
    yGrabPoint = eventP->y - windowY;

    dragIsActive = 1;
}

static void ButtonMotionHandler(Widget parentW, DragContext *dragContextP, XMotionEvent *eventP)
{
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

static void ButtonReleaseHandler(Widget w, DragContext *dragContextP, XButtonEvent *eventP)
{
    if (!dragIsActive) return;

    XLowerWindow (eventP->display, dragWindow);

    dragIsActive = 0;
}

static Opaque EnableGrab(Widget w, int button, int modifiers)
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

main(unsigned int argc, char **argv)
{
    Widget shellW, fakeW;

    shellW = XtInitialize ("VBreak", "vbreak_defaults", 0, 0, &argc, argv);

    /* Create a VFake widget, tell it to use the root window of the screen, and realize it. */

    {Arg args[1];
    XtSetArg (args[0], VFakeNwindowType, VFakeWindowTypeRoot);
    fakeW = (Widget)VFakeCreate (shellW, "vFake", args, XtNumber (args));}
    XtRealizeWidget (fakeW);

    /* Enable the VDrag option on the VFake widget. */

    EnableGrab (fakeW, DragButton, DragMask);

    XtMainLoop ();
}
