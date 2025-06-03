/* vcolorizer.c - VColorizer sample program

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This program allows the user to colorize any window on the screen.

Many thanks to Malcolm Forbes and Mark Pratt whose code and ideas
were used in this example.  Any bugs you find are mine, however.

To use VColorizer, first run the vcolorizer executable.  A color palette
will appear on the screen.  Press MB1 over any color cell to pick up a color,
then drag the pointer to the window you want to colorize.  Releasing MB1
will colorize that window.  Clicking MB3 while MB1 is still down cancels the operation.

You can use MB2 to colorize more than one window without going back to the
color palette for each window.  Using MB1, pick up a color from the palette.
Then, while still holding down MB1, click MB2 over each window you want to
colorize.  Finally, release MB1.

Colorization effects made by this tool are not saved.  A future version of
the VColorizer may be able to save color settings in a resource file.

The VColorizer also includes VDragRoot functionality.  Press Shift/Control/MB3 (Shift/Control/MB2 for MOTIF)
over any window on the screen and drag to move it.

MODIFICATION HISTORY:

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

06-Sep-1993 (sjk/nfc) Run through the MOTIF porting tools, functions changed to prototype definitions.

27-Mar-1992 (sjk) Cast.

14-Nov-1991 (sjk) Use VtkActivatepopupMenu.

06-Nov-1991 (sjk) More colors and other cleanup.  Thanks to Dave Horner for the new colors!

16-Jul-1991 (sjk) Enable and disable root window grabs under user control.
    MOTIF properly shrink-wraps popup menu, conditionalize XUI workaround.

08-Aug-1990 (sjk) Partial implementation of application-local customization.
    By using Shift/Control/MB2, a popup menu can be activated allowing
    any color resource to be customized.

17-May-1990 (sjk) Include VDragRoot functionality.

15-May-1990 (sjk) Initial entry.
*/

#include <stdio.h>
#include <stdlib.h>
#include "vtoolkit.h"
#include "vfake.h"
#include "vdrag.h"
#include <X11/cursorfont.h>

#define appName			"VColorizer - Drag'n'drop colorizer"
#define appClass		"vcolorizer_defaults"

#define PaletteButton		Button1
#define PalettePaintButton	Button2
#define PaletteCancelButton	Button3

#define defWidth		300
#define defHeight		220
#define PopupButton 		Button3
#define DragButton 		Button2
#define uidFile			"vcolorizer.uid"

static Widget wA[32];			/* CreateCallback widget array */
#define paletteW		wA[0]
#define colorNameW		wA[1]
#define colorColorW		wA[2]
#define mainW			wA[3]
#define appW			wA[4]
#define vRootW			wA[5]
#define menuW			wA[6]

static char *colorNameA[] = {
"White",
"Black",
"Gray",
"Red",
"Orange",
"Yellow",
"Green",
"Aquamarine",
"Blue",
"Cyan",
"Turquoise",
"Purple",
"Violet",

"Brown",
"Tan",
"Wheat",

"DarkSlateGray",
"DimGray",
"LightGray",
"LightSlateGray",
"SlateGray",

"AntiqueWhite",
"FloralWhite",
"GhostWhite",
"NavajoWhite",
"WhiteSmoke",

"IndianRed",
"MediumVioletRed",
"PaleVioletRed",
"VioletRed",

"DeepPink",
"HotPink",
"LightPink",
"Pink",

"DarkOrange",
"OrangeRed",

"GreenYellow",
"LightGoldenrodYellow",
"LightYellow",
"YellowGreen",
"Thistle",

"DarkGoldenrod",
"Gold",
"Goldenrod",
"LightGoldenrod",
"MediumGoldenrod",
"PaleGoldenrod",

"DarkGreen",
"DarkOliveGreen",
"DarkSeaGreen",
"ForestGreen",
"LawnGreen",
"LightSeaGreen",
"LimeGreen",
"MediumForestGreen",
"MediumSeaGreen",
"MediumSpringGreen",
"PaleGreen",
"SeaGreen",
"SpringGreen",

"AliceBlue",
"BlueViolet",
"CadetBlue",
"CornflowerBlue",
"DECWblue",
"DarkSlateBlue",
"DeepSkyBlue",
"DodgerBlue",
"LightBlue",
"LightSkyBlue",
"LightSlateBlue",
"LightSteelBlue",
"MediumBlue",
"MediumSlateBlue",
"MidnightBlue",
"NavyBlue",
"PowderBlue",
"RoyalBlue",
"SkyBlue",
"SlateBlue",
"SteelBlue",

"MediumPurple",

"Azure",
"Beige",
"Bisque",
"BlanchedAlmond",
"Burlywood",
"Chartreuse",
"Chocolate",
"Coral",
"Cornsilk",
"DarkKhaki",
"DarkOrchid",
"DarkSalmon",
"DarkTurquoise",
"DarkViolet",
"Firebrick",
"Gainsboro",
"Honeydew",
"Ivory",
"Khaki",
"Lavender",
"LavenderBlush",
"LemonChiffon",
"LightCoral",
"LightCyan",
"LightSalmon",
"Linen",
"Magenta",
"Maroon",
"MediumAquamarine",
"MediumOrchid",
"MediumTurquoise",
"MintCream",
"MistyRose",
"Moccasin",
"Navy",
"OldLace",
"OliveDrab",
"Orchid",
"PaleTurquoise",
"PapayaWhip",
"PeachPuff",
"Peru",
"Plum",
"RosyBrown",
"SaddleBrown",
"Salmon",
"SandyBrown",
"Seashell",
"Sienna",
"Snow",
"Tomato",

"ScreenBackground",
"BorderBackground",
"BorderBottomshadow",
"BorderTopshadow",
"WindowBackground",
"WindowBottomshadow",
"WindowTopshadow",
};

static Widget cellWA[XtNumber (colorNameA)];
static XColor xcolorA[XtNumber (colorNameA)];		/* palette cell RGB colors */
static char *colorNamePA[XtNumber (colorNameA)];	/* name of each cell */
static XColor blackRGB, whiteRGB;			/* colors for the cursor background */
static Cursor paintCursor;
static MrmHierarchy mrmHierarchy;
static XrmDatabase database;
static int currentColorIndex, rootGrabsEnabled, colorCnt;
static int paletteButtonIsActive, colorizeOnPaletteButtonRelease;
static Window popupTargetWindow;
static Widget popupTargetWidget;

static void CreateCallback (Widget w, int *indexP, XmAnyCallbackStruct *unused_cbDataP)
{
    wA[*indexP] = w;
}

static void SaveGeometryCallback(Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    int i;
    char resourceNameA[64];

    for (i = 0; i < colorCnt; i++) {
	sprintf (resourceNameA, "cellColor%d", i);
	VtkSaveStringResource (&database, resourceNameA, colorNamePA[i]);
    }

    VtkSaveNumericResourceN (&database, mainW, XtNwidth);
    VtkSaveNumericResourceN (&database, mainW, XtNheight);
    VtkSaveNumericResourceN (&database, appW, XtNx);
    VtkSaveNumericResourceN (&database, appW, XtNy);

    XrmPutFileDatabase (database, VtkGetDefaultsName (appW));
}

static void QuitCallback (Widget unused_w, Opaque unused_closure, XmAnyCallbackStruct *unused_cbDataP)
{
    exit (0);
}

static void ShufflePalette(int fromIndex, int toIndex, int shuffleFlag)
{
    int i;
    Pixel tempColor;
    char *tempNameP;
    XColor tempXColor;

    tempColor  = XtBackground (cellWA[fromIndex]);
    tempNameP  = colorNamePA[fromIndex];
    tempXColor = xcolorA[fromIndex];

    if (!shuffleFlag) {
	XtVaSetValues (cellWA[fromIndex], XtNbackground, XtBackground (cellWA[toIndex]), 0);
	colorNamePA[fromIndex] = colorNamePA[toIndex];
	xcolorA[fromIndex]     = xcolorA[toIndex];
    } else {
	if (fromIndex > toIndex)
	    for (i = fromIndex; i > toIndex; i--) {
		XtVaSetValues (cellWA[i], XtNbackground, XtBackground (cellWA[i - 1]), 0);
		colorNamePA[i] = colorNamePA[i - 1];
		xcolorA[i]     = xcolorA[i - 1];
	    }
	else
	    for (i = fromIndex; i < toIndex; i++) {
		XtVaSetValues (cellWA[i], XtNbackground, XtBackground (cellWA[i + 1]), 0);
		colorNamePA[i] = colorNamePA[i + 1];
		xcolorA[i]     = xcolorA[i + 1];
	    }
    }

    XtVaSetValues (cellWA[toIndex], XtNbackground, tempColor, 0);
    colorNamePA[toIndex] = tempNameP;
    xcolorA[toIndex]     = tempXColor;
}

static void UpdateDisplay(void)
{
    char *labelP = (char *)XmStringCreate (colorNamePA[currentColorIndex], XmSTRING_DEFAULT_CHARSET);

    XtVaSetValues (colorNameW, XmNlabelString, labelP, 0);
    XtFree (labelP);
    XtVaSetValues (colorColorW, XtNbackground, XtBackground (cellWA[currentColorIndex]), 0);
}

static Window GetSubwindow(Window window, int x, int y, Window avoidWindow)
{
    Window subwindow, newSubwindow;

    subwindow = window;

    while (subwindow) {
	if (subwindow == avoidWindow) return (0);

	XTranslateCoordinates (XtDisplay (appW), window, subwindow, x, y, &x, &y, &newSubwindow);

	window = subwindow;
	subwindow = newSubwindow;
    }

    return (window);
}

static void PopupCallback(Widget w, int *indexP, XmAnyCallbackStruct *cbDataP)
{
    char *resourceNameP = (char *)VtkGetOneValue (w, XmNuserData);

    if (popupTargetWindow) {
	if (!strcmp (resourceNameP, XtNbackground)) {
	    XSetWindowBackground (XtDisplay (appW), popupTargetWindow, 	XtBackground (cellWA[currentColorIndex]));

            XClearArea (XtDisplay (appW), popupTargetWindow, 0, 0, 0, 0, 1);
	} else if (!strcmp (resourceNameP, XtNborderColor)) {
	    XSetWindowBorder (XtDisplay (appW), popupTargetWindow, XtBackground (cellWA[currentColorIndex]));
	}
    } else {
	XtVaSetValues (popupTargetWidget, resourceNameP, XtBackground (cellWA[currentColorIndex]), 0);
    }
}

static void RemoveAllMenuEntries(Widget popupMenuW)
{
    int numChildren = (int)VtkGetOneValue (popupMenuW, XmNnumChildren);
    Widget *childrenP = (Widget *)VtkGetOneValue (popupMenuW, XmNchildren);

    if (numChildren) XtUnmanageChildren (childrenP, numChildren);
}

static void AddMenuEntry(Widget popupMenuW, char *labelP, int slot)
{
    int numChildren = (int)VtkGetOneValue (popupMenuW, XmNnumChildren);
    Widget *childrenP = (Widget *)VtkGetOneValue (popupMenuW, XmNchildren);
    MrmCode class;
    Widget w;
    char *xValueP = (char *)XmStringCreate (labelP, XmSTRING_DEFAULT_CHARSET);

    if (numChildren <= slot) (void)MrmFetchWidget (mrmHierarchy, "vColorizerPopupEntry", popupMenuW, &w, &class);
    else w = childrenP[slot];

    XtVaSetValues (w, XmNlabelString, xValueP, XmNuserData, labelP, 0);
    XtFree (xValueP);
}

static int LoadWindowMenu(Widget popupMenuW, Window window)
{
    int cnt = 0;

    AddMenuEntry (popupMenuW, XtNbackground,  cnt++);
    AddMenuEntry (popupMenuW, XtNborderColor, cnt++);

    popupTargetWindow = window;
    popupTargetWidget = 0;

    return (cnt);
}

static XrmQuark pixelResourceTypeQ = 0;

static int LoadWidgetMenu(Widget popupMenuW, Widget w)
{
    CoreClassPart *coreClassP = (CoreClassPart *)w->core.widget_class;
    XtResource **resPP = (XtResource **)coreClassP->resources;
    int resCnt = coreClassP->num_resources;
    int i, cnt;
    XtResource *resP;

    if (!pixelResourceTypeQ) pixelResourceTypeQ = XrmStringToQuark (XtRPixel);

    for (i = 0, cnt = 0; i < resCnt; i++, resPP++) {
	resP = *resPP;

        if ((XrmQuark)resP->resource_type == pixelResourceTypeQ)
	    AddMenuEntry (popupMenuW, XrmQuarkToString ((int)resP->resource_name), cnt++);
    }

    popupTargetWindow = 0;
    popupTargetWidget = w;

    return (cnt);
}

static void PopupButtonPressHandler(Widget w, XtPointer popupMenuOW, XEvent *eventOP, Boolean *unused_continueP)
{
    Widget popupMenuW = (Widget)popupMenuOW;
    XButtonEvent *eventP = (XButtonEvent *)eventOP;
    Window window;
    Widget targetW;
    int cnt;

    /* Wrong modifiers?  Ignore it. */

    if ((!(eventP->state & ShiftMask)) || (!(eventP->state & ControlMask))) return;

    /* Wrong button?  Ignore it. */

    if (eventP->button != PopupButton) return;

    /* Determine the target window, but don't colorize within the palette. */

    window = GetSubwindow (eventP->window, eventP->x_root, eventP->y_root, XtWindow (paletteW));

    if (!window) {
	XBell (XtDisplay (appW), 0);
	return;
    }

    /* Load the popup menu. */

    RemoveAllMenuEntries (popupMenuW);

    if (targetW = XtWindowToWidget (XtDisplay (appW), window)) cnt = LoadWidgetMenu (popupMenuW, targetW);
    else 						       cnt = LoadWindowMenu (popupMenuW, window);

    XtManageChildren ((Widget *)VtkGetOneValue (popupMenuW, XmNchildren), cnt);

    VtkActivatePopupMenu (popupMenuW, (XEvent*) eventP);
}

static void ColorizeBackground(XButtonEvent *eventP)
{
    Window window;
    Widget widget;
    int i;

    window = GetSubwindow (RootWindowOfScreen (XtScreen (appW)), eventP->x_root, eventP->y_root, 0);
    if (!window) return;

    widget = XtWindowToWidget (XtDisplay (appW), window);
    if (widget && (XtParent (widget) == paletteW))
	for (i = 0; i < colorCnt; i++) if (cellWA[i] == widget) {
	    ShufflePalette (currentColorIndex, i, (eventP->state & ShiftMask));
	    return;
	}

    XSetWindowBackground (XtDisplay (appW), window, XtBackground (cellWA[currentColorIndex]));

    /* Clear the window and force exposure to apply the new background. */

    XClearArea (XtDisplay (appW), window, 0, 0, 0, 0, 1);
}

static void PaletteButtonReleaseHandler(Widget w, XtPointer indexO, XEvent *eventOP, Boolean *unused_continueP)
{
    int index = (int)indexO;
    XButtonEvent *eventP = (XButtonEvent *)eventOP;

    if (!paletteButtonIsActive) return;

    if (eventP->button != PaletteButton) return;

    if (colorizeOnPaletteButtonRelease) ColorizeBackground (eventP);

    paletteButtonIsActive = 0;
}

static void PaletteButtonPressHandler(Widget w, XtPointer unused_closure, XEvent *eventOP, Boolean *unused_continueP)
{
    XButtonEvent *eventP = (XButtonEvent *)eventOP;
    XColor *backP;

    if (paletteButtonIsActive) {
	if (eventP->button == PalettePaintButton) {
	    ColorizeBackground (eventP);
	    colorizeOnPaletteButtonRelease = 0;

	} else if (eventP->button == PaletteCancelButton) {
	    XUngrabPointer (XtDisplay (appW), eventP->time);
	    paletteButtonIsActive = 0;
	}

	return;
    }

    if (eventP->button != PaletteButton) return;

    /* If there is a subwindow (widget), it must be a palette cell.  The cell's user data contains its color index. */

    if (eventP->subwindow && (w = XtWindowToWidget (XtDisplay (appW), eventP->subwindow)))
	currentColorIndex = (int)VtkGetOneValue (w, XmNuserData);

    /* Pick an appropriate background color for the cursor.  Make sure it is not equal to the foreground color. */

    if (xcolorA[currentColorIndex].pixel != whiteRGB.pixel) backP = &whiteRGB;
    else 						    backP = &blackRGB;

    XRecolorCursor (XtDisplay (appW), paintCursor, backP, &xcolorA[currentColorIndex]);

    UpdateDisplay ();

    paletteButtonIsActive = 1;
    colorizeOnPaletteButtonRelease = 1;
}

static void CreatePalette(void)
{
    int i, ac, cellsHigh, cellsWide;
    XColor dummy;
    MrmCode class;
    Arg args[16];
    char resourceNameA[64];
    char *colorNameP;

    colorCnt = 0;

    XAllocNamedColor (XtDisplay (appW), appW->core.colormap, "white", &dummy, &whiteRGB);
    XAllocNamedColor (XtDisplay (appW), appW->core.colormap, "black", &dummy, &blackRGB);

    for (i = 0; i < XtNumber (colorNameA); i++) {
	sprintf (resourceNameA, "cellColor%d", i);
	if (!(colorNameP = (char *)VtkGetStringResource (database, resourceNameA))) colorNameP = colorNameA[i];

	if (XAllocNamedColor (XtDisplay (appW), appW->core.colormap, colorNameP, &dummy, &xcolorA[colorCnt])) {
	    colorNamePA[colorCnt] = colorNameP;
	    colorCnt++;
	}
    }

    cellsHigh = colorCnt;
    cellsWide = 1;

    while (cellsHigh > cellsWide) {
	cellsWide++;
	cellsHigh = (colorCnt + cellsWide - 1) / cellsWide;
    }

    XtVaSetValues (paletteW, XmNfractionBase, cellsWide * cellsHigh, 0);

    for (i = 0; i < colorCnt; i++) {
	ac = 0;
	XtSetArg (args[ac], XtNbackground,	   xcolorA[i].pixel); 				ac++;
	XtSetArg (args[ac], XmNleftPosition,   (i%cellsWide) * cellsHigh);			ac++;
	XtSetArg (args[ac], XmNrightPosition,  (i%cellsWide) * cellsHigh + cellsHigh);	ac++;
	XtSetArg (args[ac], XmNtopPosition,    (i/cellsWide) * cellsWide);			ac++;
	XtSetArg (args[ac], XmNbottomPosition, (i/cellsWide) * cellsWide + cellsWide);	ac++;
	XtSetArg (args[ac], XmNuserData,          i);						ac++;

        (void)MrmFetchWidgetOverride (mrmHierarchy, "vColorizerCell", paletteW, 0, args, ac, &cellWA[i], &class);
    }

    XtManageChildren (cellWA, XtNumber (cellWA));
}

static void DisableRootGrabs(void)
{
    if (!rootGrabsEnabled) return;

    VDragDisable (vRootW);

    XUngrabButton (
	XtDisplay (vRootW),				/* display */
	PopupButton,					/* button_grab */
	ShiftMask | ControlMask,			/* modifiers */
	XtWindow (vRootW));				/* window */

    XtRemoveRawEventHandler (vRootW, ButtonPressMask, 0, PopupButtonPressHandler, menuW);

    rootGrabsEnabled = 0;
}

static void EnableRootGrabs(void)
{
    MrmCode class;

    if (rootGrabsEnabled) return;

    /* Create a VFake widget and pass it the window ID of the root window on
     * the same screen as the application shell widget. */

    if (!vRootW) {
	vRootW = (Widget)VFakeCreate (appW, "vFake", 0, 0);
    	XtVaSetValues (vRootW, VFakeNwindow, RootWindowOfScreen (XtScreen (appW)), 0);
    	XtRealizeWidget (vRootW);
    }

    VDragEnableGrab (vRootW, DragButton, ShiftMask | ControlMask);

    if (!menuW) MrmFetchWidget (mrmHierarchy, "vColorizerPopupMenu", vRootW, &menuW, &class);

    XGrabButton (
	XtDisplay (vRootW),						/* display */
	PopupButton,							/* button_grab */
	ShiftMask | ControlMask,					/* modifiers */
	XtWindow (vRootW),						/* window */
	1,								/* owner_events */
	(ButtonPressMask | ButtonMotionMask | ButtonReleaseMask),	/* event_mask */
	GrabModeAsync,							/* pointer_mode */
	GrabModeAsync,							/* keyboard_mode */
	None,								/* confine_to */
	0);								/* cursor */

    XtAddRawEventHandler (vRootW, ButtonPressMask, 0, PopupButtonPressHandler, menuW);

    rootGrabsEnabled = 1;
}

static void RootCallback(Widget w, Opaque closure, XmToggleButtonCallbackStruct *cbDataP)
{
    int newValue;

    newValue = cbDataP->set;

    if (newValue) EnableRootGrabs ();
    else	  DisableRootGrabs ();
}

static void EstablishOnePaletteGrab(Widget w)
{
    XGrabButton (
	XtDisplay (appW),				/* display */
	PaletteButton,					/* button_grab */
	AnyModifier,					/* modifiers */
	XtWindow (w),					/* window */
	0,						/* owner_events */
	(ButtonPressMask | ButtonReleaseMask),		/* event_mask */
	GrabModeAsync,					/* pointer_mode */
	GrabModeAsync,					/* keyboard_mode */
	None,						/* confine_to */
	paintCursor);					/* cursor */

    XtAddRawEventHandler (w, ButtonPressMask,   0, PaletteButtonPressHandler,   0);
    XtAddRawEventHandler (w, ButtonReleaseMask, 0, PaletteButtonReleaseHandler, 0);
}

static MrmRegisterArg mrmRegisterArgs[] = {
    {"VColorizerCreateCallback",	(char *)CreateCallback},
    {"VColorizerPopupCallback",		(char *)PopupCallback},
    {"VColorizerQuitCallback",		(char *)QuitCallback},
    {"VColorizerRootCallback",		(char *)RootCallback},
    {"VColorizerSaveGeometryCallback",	(char *)SaveGeometryCallback},
    };

int main(int argc, char **argv)
{
    MrmCode class;

    MrmInitialize ();

    if (!VtkInitialize (appName, appClass, &argc, argv, uidFile, mrmRegisterArgs, XtNumber (mrmRegisterArgs), &mrmHierarchy, &appW))
	exit (0);

    database = XrmGetFileDatabase (VtkGetDefaultsName (appW));

    MrmFetchWidget (mrmHierarchy, "vColorizerMain", appW, &mainW, &class);

    CreatePalette ();
    UpdateDisplay ();	/* set initial values for colorName and colorColor */

    /* If there was no resource file, set the default window geometry. */

    if (!database) XtVaSetValues (mainW, XtNwidth, defWidth, XtNheight, defHeight, 0);

    /* Some versions of the toolkit need coaxing to propagate the size. */

    XtVaSetValues (appW,
	XtNwidth, 	mainW->core.width,
	XtNheight, 	mainW->core.height,
	0);

    /* Manage and realize the main window widget tree. */

    XtManageChild (mainW);
    XtRealizeWidget (appW);

    /* Establish all passive button grabs. */

    paintCursor = XCreateFontCursor (XtDisplay (appW), XC_spraycan);

    EstablishOnePaletteGrab (paletteW);
    EstablishOnePaletteGrab (colorColorW);

    XtMainLoop ();
}
