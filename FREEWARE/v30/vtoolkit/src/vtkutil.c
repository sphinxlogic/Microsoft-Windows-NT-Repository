/* vtkutil.c - VToolkit common convenience routines

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains common "convenience" routines for the VToolkit.

MODIFICATION HISTORY:

08-Aug-1994 (sjk) Bugfix - VtkGetCharWidthAndHeight was correctly calculating the maximum size of
        width and height in the given font list, but was always returning the last font's width and height.
	Thanks to Shinichiro IKEDA for this fix!

23-May-1994 (sjk) Bugfix - Call XmStringFree instead of XtFree.  Thanks Bill!

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

14-Sep-1993 (sjk) Fix a few more typedefs.

14-Sep-1993 (sjk) Add VtkAddDeleteWindowCallback and VtkFlushUntilDone routines.

14-Sep-1993 (sjk) Bugfix typedef of value in VtkGetOneValue to work on 64-bit systems.

28-Aug-1993 (nfc) Run through the Motif porting tools

10-Apr-1992 (sjk) Bugfix (another) memory allocation problem in VtkGetFontA.

26-Mar-1992 (sjk) Change include directory for XmP to Xm/.

02-Dec-1991 (sjk) Add VtkGetIntResource routine.

25-Nov-1991 (sjk) Add VtkSetIdentifierValue routine.

20-Nov-1991 (sjk) Add VtkGetStringResource routine.

14-Nov-1991 (sjk) Add VtkActivatePopupMenu routine.

14-Nov-1991 (sjk) Add VtkFetchWidget convenience routine.

08-Nov-1991 (sjk) Add VtkGetChildByClass and VtkGetSubwindow routines.

06-Nov-1991 (sjk) Add VtkGetDefaultsName and VtkUnitTypeDefault routines.

20-Aug-1991 (sjk) Bugfix - allocate enough memory in VtkWidgetResourceToName.

19-Jul-1991 (sjk) Bugfix - copying extra longword in VtkGetFontA (XUI).

09-Apr-1991 (sjk) Add VtkCopyColorNames routines.

28-Mar-1991 (sjk) Fix VtkComputeTextWidth to handle FIXED fonts that do not have per-char information.  Reformat source.
	Properly initialize some context variables as required by MOTIF V1.0 toolkit.  Add VtkGetDrawableWidthAndHeight
	and VtkGetCharWidthAndHeight routines.

01-Jun-1990 (sjk) Add VtkCopyColors, VtkSetFieldsToZero, VtkMinMax, VtkComputeTextWidth, VtkPermissiveGeometryManager,
	VtkNullRoutine, VtkGetStippleBitmap, and VtkRecloneString routines.  These routines are used for widget writing.

25-May-1990 (sjk) Add VtkWidgetResourceToName and VtkSaveNumericResourceN.

10-May-1990 (sjk) Add portable fontlist routines.

01-May-1990 (sjk) Use portable varargs macros.

05-Apr-1990 (sjk) Initial entry.
*/

#include <stdio.h>
#include <stdlib.h>
#include "vtoolkit.h"
#include <Xm/RowColumn.h>
#include <Xm/Protocols.h>
#include <X11/ShellP.h>

XtArgVal VtkGetOneValue(Widget w, char *nameP)
{
    Arg argA[1];
    XtArgVal value;

    value = 0;				/* in case short value is fetched */
    XtSetArg (argA[0], nameP, &value);
    XtGetValues (w, argA, 1);

    return value;
}

void VtkChangeRadioResource(Widget w, int *valueP, XmToggleButtonCallbackStruct *cbDataP, char *resourceP)
{
    if (!cbDataP->set) return;		/* only interested when toggle is set */
    XtVaSetValues (w, resourceP, *valueP, 0);
}

char *VtkWidgetResourceToName(Widget w, char *resourceNameP)
{
    char nameA[2048], tempNameA[2048], *nameP;
    int totLen, len;
    Widget pW;

    strcpy (nameA, resourceNameP);
    totLen = strlen (nameA);

    for (; w; w = pW) {
	if ((!(pW = XtParent (w))) && XtIsApplicationShell (w))
	    nameP = XrmQuarkToString (((ApplicationShellWidget)w)->application.xrm_class);
	else nameP = XrmQuarkToString (w->core.xrm_name);

	len = strlen (nameP);
	memcpy (tempNameA, nameP, len);
	tempNameA[len] = '.';
	tempNameA[len + 1] = 0;
	strcat (tempNameA, nameA);
	totLen += (len + 1);
	memcpy (nameA, tempNameA, totLen + 1);
    }

    nameP = (char *)XtMalloc (totLen + 1);
    memcpy (nameP, nameA, totLen + 1);

    return (nameP);
}

void VtkSaveNumericResource(XrmDatabase *databaseP, char *resourceNameP, int number)
{
    XrmValue value;
    char valueA[256];

    sprintf (valueA, "%d", number);
    value.addr = valueA;
    value.size = strlen (valueA) + 1;
    XrmPutResource (databaseP, resourceNameP, XtRString, &value);
}

void VtkSaveNumericResourceN(XrmDatabase *databaseP, Widget w, char *nameP)
{
    int number = (int)VtkGetOneValue (w, nameP);
    char *resourceNameP = VtkWidgetResourceToName (w, nameP);

    VtkSaveNumericResource (databaseP, resourceNameP, number);
    XtFree (resourceNameP);
}

void VtkSaveStringResource(XrmDatabase *databaseP, char *resourceNameP, char *valueP)
{
    XrmValue value;

    value.addr = valueP;
    value.size = strlen (valueP) + 1;
    XrmPutResource (databaseP, resourceNameP, XtRString, &value);
}

static Widget *createWidgetsP, *createNegativeWidgetsP;

static void CreateCallback(Widget w, int *indexP, XmAnyCallbackStruct *cbDataP)   /* a widget was created */
{
    int index = (*indexP);

    if (index >= 0) createWidgetsP[index] 	         = w;
    else	    createNegativeWidgetsP[(-1) - index] = w;
}

Widget VtkFetchWidget(MrmHierarchy mrmHierarchy, char *nameP, Widget parentW, Widget *widgetsP, Widget *negativeWidgetsP)
{
    MrmCode dummyClass;
    Widget newW;
 
    createWidgetsP         = widgetsP;
    createNegativeWidgetsP = negativeWidgetsP;

    (void)MrmFetchWidget (mrmHierarchy, nameP, parentW, &newW, &dummyClass);

    return (newW);
}

void VtkSetIdentifierValue(char *nameP, Opaque value)
{
    MrmRegisterArg registerArg;

    registerArg.name  = nameP;
    registerArg.value = (char *)value;

    MrmRegisterNames (&registerArg, 1);
}

static MrmRegisterArg vtkMrmRegisterArgs[] = {
    {"VtkCreateCallback",		(char *)CreateCallback},
};

int VtkInitialize(char *appNameP, char *appClassP, int *argcP, char **argv,  char *uidFileP, 
	MrmRegisterArg *mrmRegisterArgsP, int mrmRegisterArgCnt, MrmHierarchy *mrmHierarchyP, Widget *appWP)
{
    /* Initialize the toolkit and attach to the display. */

    (*appWP) = XtInitialize (appNameP, appClassP, 0, 0, argcP, argv);

    /* For REL4, set the title explicitly. */

    XtVaSetValues (*appWP, XtNtitle, appNameP, 0);

    /* Open the UID file. */

    (*mrmHierarchyP) = 0;	/* MOTIF requires this to be initialized to zero */

    if (MrmOpenHierarchy (1, &uidFileP, 0, mrmHierarchyP) != MrmSUCCESS) {
	printf ("Failure - Can't open %s file", uidFileP);
	return (0);
    }

    /* Register the application callback routines. */

    MrmRegisterNames (mrmRegisterArgsP, mrmRegisterArgCnt);

    /* Register VTK callback routines. */

    MrmRegisterNames (vtkMrmRegisterArgs, XtNumber (vtkMrmRegisterArgs));

    return (1);
}

XFontStruct *VtkGetFirstFont(struct _XmFontListRec *fontTableP)
{
    XmFontContext context = 0;
    XmStringCharSet charset;
    XFontStruct *fontP;

    XmFontListInitFontContext (&context, fontTableP);
    XmFontListGetNextFont (context, &charset, &fontP);
    XmStringFree ((unsigned char *)charset);
    XmFontListFreeFontContext (context);

    return (fontP);
}

void VtkGetFontA(struct _XmFontListRec *fontTableP, XFontStruct ***fontAP, int *fontCntP)
{
    int fontCnt = 0;
    XFontStruct **fontPP;
    XmFontContext context = 0;
    XFontStruct *fontP;
    XmStringCharSet charset;

    XmFontListInitFontContext (&context, fontTableP);
    while (XmFontListGetNextFont (context, &charset, &fontP)) {
	fontCnt++;
	XmStringFree ((unsigned char *)charset);
    }
    XmFontListFreeFontContext (context);

    (*fontAP) = fontPP = (XFontStruct **)XtMalloc (fontCnt * sizeof (XFontStruct *));
    *fontCntP = fontCnt;
    context = 0;
    XmFontListInitFontContext (&context, fontTableP);
    while (XmFontListGetNextFont (context, &charset, &fontP)) {
	*fontPP++ = fontP;
	XmStringFree ((unsigned char *)charset);
    }

    XmFontListFreeFontContext (context);
}

XmFontList VtkGetDefaultFontList(Widget w)
{
    while ((w = XtParent (w))) {
	if (XmIsBulletinBoard (w)) 		      return (XmFontListCopy ((XmFontList)VtkGetOneValue (w, XmNlabelFontList)));
	if (XmIsVendorShell (w) || XmIsMenuShell (w)) return (XmFontListCopy ((XmFontList)VtkGetOneValue (w, XmNdefaultFontList)));
    }

    return (0);
}

void VtkUnitTypeDefault(Widget w, int offset, XrmValue *valueP)
{
    static unsigned char unitType;

    valueP->size = sizeof (unitType);
    valueP->addr = (char *)&unitType;

    if (XmIsManager (w->core.parent)) unitType = (unsigned char)VtkGetOneValue (w->core.parent, XmNunitType);
    else			      unitType = XmPIXELS;
}

int VtkCopyColorNames(Widget w, char **colorNamesPP, Pixel foreground, Pixel **colorTablePP)
{
    char **colorNamePP;
    int colorCnt = 0;
    XColor color, dummy;
    Pixel pixel;
    Pixel *colorTableP;

    /* Determine the number of names in the color name list. */

    for (colorNamePP = colorNamesPP; *colorNamePP; colorNamePP++) colorCnt++;

    /* Resize the color table. */

    *colorTablePP = colorTableP = (Pixel *)XtRealloc ((char *)(*colorTablePP), colorCnt * sizeof (Pixel));

    /* For each entry in the color name list, try to get the pixel value of the named color.  Fallback to foreground color. */

    for (colorNamePP = colorNamesPP; *colorNamePP; colorNamePP++) {
	if (XAllocNamedColor (XtDisplay (w), w->core.colormap, *colorNamePP, &color, &dummy)) {
            pixel = color.pixel;
            if (pixel == w->core.background_pixel) pixel = foreground;
    	} else pixel = foreground;

	(*(colorTableP++)) = pixel;
    }

    return (colorCnt);
}

int VtkCopyColors(Widget w, Pixmap colorList, Pixel **colorTablePP)
{
    Window rootWindow;
    int i, x, y;
    unsigned int colorCnt, height, borderWidth, depth;
    XImage *imageP;
    Pixel *colorTableP;

    if (!colorList) return (0);          /* no table was supplied */

    /* What is the width of the color pixmap? */

    XGetGeometry (XtDisplay (w), colorList, &rootWindow, &x, &y, &colorCnt, &height, &borderWidth, &depth);

    /* Resize the color table. */

    *colorTablePP = colorTableP = (Pixel *)XtRealloc ((char *)(*colorTablePP), colorCnt * sizeof (Pixel));

    /* Get an image of the color pixmap, copy the pixel values from that image to the color table, and destroy the image. */

    imageP = XGetImage (XtDisplay (w), colorList, 0, 0, colorCnt, 1, -1, ZPixmap);

    for (i = 0; i < colorCnt; i++) colorTableP[i] = XGetPixel (imageP, i, 0);

    XDestroyImage (imageP);

    return (colorCnt);
}

void VtkSetFieldsToZero(char *firstP, char *lastP)
{
    char *cP = firstP;

    while (cP != lastP) *(cP++) = 0;
}

void VtkMinMax(int a, int b, int *minP, int *maxP)
{
    if (a < b) {*minP = a; *maxP = b;}
    else       {*minP = b; *maxP = a;}
}

int VtkComputeTextWidth(XFontStruct *fontP, char *textP)
{
    int width;
    unsigned char *chP;
    XCharStruct *charP;
    unsigned int firstCol, numCols, c;

    if (!textP) return (0);

    width 	= 0;
    chP 	= (unsigned char *)textP;
    charP 	= fontP->per_char;
    firstCol 	= fontP->min_char_or_byte2;
    numCols 	= fontP->max_char_or_byte2 - firstCol + 1;

    while (*chP) {
        c = *(chP++) - firstCol;
        if (c < numCols) 
            if (charP)  width += charP[c].width;
            else	width += fontP->max_bounds.width;	/* FIXED font has no per-char information */
    }

    return (width);
}

XtGeometryResult VtkPermissiveGeometryManager(Widget w, XtWidgetGeometry *requestP, XtWidgetGeometry *replyP)
{
    if (requestP->request_mode & CWX)		w->core.x		= requestP->x;
    if (requestP->request_mode & CWY) 		w->core.y 		= requestP->y;
    if (requestP->request_mode & CWWidth) 	w->core.width 		= requestP->width;
    if (requestP->request_mode & CWHeight) 	w->core.height 		= requestP->height;
    if (requestP->request_mode & CWBorderWidth) w->core.border_width 	= requestP->border_width;

    return (XtGeometryYes);		/* just say yes */
}

void VtkNullRoutine(void)
{
}

Pixmap VtkGetStippleBitmap(Widget w)
{
    int  x, y;
    unsigned int width, height;
    XGCValues values;
    GC tempgc;
    Pixmap stippleBitmap;

    XQueryBestStipple (XtDisplay (w), XtWindow (w), 2, 2, &width, &height);

    stippleBitmap = XCreatePixmap (XtDisplay (w), XtWindow (w), width, height, 1);

    values.foreground = 0;
    values.function = GXcopy;
    tempgc = XCreateGC (XtDisplay (w), stippleBitmap, GCForeground | GCFunction, &values);

    XFillRectangle (XtDisplay (w), stippleBitmap, tempgc, 0, 0, width, height);

    XSetForeground (XtDisplay (w), tempgc, 1);

    for (y = 0; y < height; y++)
	for (x = 1 - (y & 1); x < width; x += 2)
	    XDrawPoint (XtDisplay (w), stippleBitmap, tempgc, x, y);

    XFreeGC (XtDisplay (w), tempgc);

    return (stippleBitmap);
}

char *VtkRecloneString(char *oldP, char *newP)
{
    int len;
    char *toP, *toSP, *fromSP;

    len = strlen (newP) + 1;
    toP = toSP = XtRealloc (oldP, len);
    fromSP = newP;
    while (len--) *(toSP++) = *(fromSP++);

    return (toP);
}

void VtkGetDrawableWidthAndHeight(Display *dpy, Pixmap p, int *widthP, int *heightP)
{
    Window rootWindow;
    int x, y;
    unsigned int borderWidth, depth;

    XGetGeometry (dpy, p, &rootWindow, &x, &y, (unsigned int *)widthP, (unsigned int *)heightP, &borderWidth, &depth);
}

void VtkGetCharWidthAndHeight(XFontStruct **fontPP, int fontCnt, int *charWidthP, int *charHeightP)
{
    int i, charWidth, charHeight, maxCharWidth, maxCharHeight;
    XCharStruct *charP;
    XFontStruct *fontP;

    maxCharWidth = maxCharHeight = 0;

    for (i = 0; i < fontCnt; i++) {
        fontP = fontPP[i];
	charP = fontP->per_char;

	if (charP) charWidth = charP['0' - fontP->min_char_or_byte2].width;
	else	   charWidth = fontP->max_bounds.width;				/* FIXED font has no per-char information */

        if (maxCharWidth < charWidth) maxCharWidth = charWidth;

        charHeight = fontP->max_bounds.descent + fontP->max_bounds.ascent;
        if (maxCharHeight < charHeight) maxCharHeight = charHeight;
    }

    (*charWidthP)  = maxCharWidth;
    (*charHeightP) = maxCharHeight;
}

static char defaultsNameA[256];

char *VtkGetDefaultsName(Widget w)
{
    Widget pW;

    for (; w; w = pW)
        if ((!(pW = XtParent (w))) && XtIsApplicationShell (w)) {
#ifdef VMS
            strcpy (defaultsNameA, "decw$user_defaults:");
            strcat (defaultsNameA, XrmQuarkToString (((ApplicationShellWidget)w)->application.xrm_class));
            strcat (defaultsNameA, ".dat");
#else
            strcpy (defaultsNameA, (char *)getenv ("HOME"));
            strcat (defaultsNameA, "/");
            strcat (defaultsNameA, XrmQuarkToString (((ApplicationShellWidget)w)->application.xrm_class));
#endif
            return (defaultsNameA);
        }

    return ("");
}

Widget VtkGetChildByClass(Widget w, WidgetClass class)
{
    int numChildren = (int)VtkGetOneValue (w, XmNnumChildren);
    Widget *childrenP = (Widget *)VtkGetOneValue (w, XmNchildren);
    Widget siblingW;
    int i;

    for (i = 0; i < numChildren; i++) {
	siblingW = childrenP[i];
        if (XtIsSubclass (siblingW, class)) return (siblingW);
    }

    return ((Widget)0);
}

Window VtkGetSubwindow(Display *dpy, Window window, int x, int y, Window avoidWindow, int *subwindowXP, int *subwindowYP)
{
    Window subwindow, newSubwindow;

    subwindow = window;

    while (subwindow) {
        if (subwindow == avoidWindow) return (0);

        XTranslateCoordinates (dpy, window, subwindow, x, y, &x, &y, &newSubwindow);

        window = subwindow;
        subwindow = newSubwindow;
    }

    (*subwindowXP) = x;
    (*subwindowYP) = y;

    return (window);
}

void VtkActivatePopupMenu(Widget w, XEvent *eventP)
{
    XmMenuPosition (w, (XButtonPressedEvent *)eventP);

    if (w->core.x < 0) 					       w->core.x = 0;
    if ((w->core.x + w->core.width) > w->core.screen->width)   w->core.x = w->core.screen->width - w->core.width;

    if (w->core.y < 0) 					       w->core.y = 0;
    if ((w->core.y + w->core.height) > w->core.screen->height) w->core.y = w->core.screen->height - w->core.height;

    XtManageChild (w);
}

char *VtkGetStringResource(XrmDatabase database, char *resourceNameP)
{
    char *typeP;
    XrmValue value;

    if (!XrmGetResource (database, resourceNameP, resourceNameP, &typeP, &value)) return (0);

    return (XtNewString (value.addr));
}

int VtkGetIntResource(XrmDatabase database, char *resourceNameP, int defaultValue)
{
    char *typeP;
    XrmValue value;

    if (!XrmGetResource (database, resourceNameP, resourceNameP, &typeP, &value)) return (defaultValue);

    return (atoi (value.addr));
}

void VtkAddDeleteWindowCallback(Widget w, void (*callbackP)(), Opaque closure)
{
    Arg args[1];

    /* This routine establishes the callbackP routine as the CloseWindow callback for the shell widget w.
     * w must be an instance of a vendorShellWidgetClass or subclass thereof. */

    XtVaSetValues (w, XmNdeleteResponse, XmDO_NOTHING, 0);

    XmAddProtocolCallback (w,
        XmInternAtom (XtDisplay (w), "WM_PROTOCOLS",     0),
        XmInternAtom (XtDisplay (w), "WM_DELETE_WINDOW", 0),
        callbackP, closure);
}

void VtkFlushUntilDone(Display *dpy)
{
    XEvent event;

    /* Call this routine to *really* make sure that the request/event pipeline is flushed. */

    /* Loop until the event queue is empty. */

    while (1) {
	/* Flush out pending requests and wait for all response events to arrive. */

	XSync (dpy, 0);

	/* If there are no pending events, we are done. */

        if (!XEventsQueued (dpy, QueuedAlready)) return;

	/* Read an event and process it.  Continue until the event queue is empty.  Then, loop back to
	 * see if we've triggered any more events. */

	do {
	    XNextEvent (dpy, &event);
	    XtDispatchEvent (&event);
	} while (XEventsQueued (dpy, QueuedAlready));
    }
}
