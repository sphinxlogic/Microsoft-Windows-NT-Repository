#ifndef lint
static char	*RCSid = "$Header$";
#endif

/*
 * Plaid.c - code for the plaid widget.
 *
 * Based on the Template widget from the X11R3 distribution.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94086
 * davy@riacs.edu
 *
 * $Log$
 */

/* Modified by:
            Jatin Desai (VIA::DESAI) , Core Applications Group
	    110 Spitbrook Road, Nashua, NH 03060
	    Digital Equipment Corporation

   to work with VMS DECwindows environment. Look for ifndef VMS and
   ifdef VMS to see changes.
*/

#ifndef VMS
#include <X11/copyright.h>
#endif

#include <X11/DwtWidget.h>
#include <X11/Vendor.h>

#include "PlaidP.h"


#define plaid_width	22
#define plaid_height	22
#define iconize_width	17
#define iconize_height	17
#define icon_width	32
#define icon_height	32

/*
 * The bits representing the plaid background.
 */
static char plaid_bits[] = {
   0x75, 0xfd, 0x3f, 0xaa, 0xfa, 0x3e, 0x75, 0xfd, 0x3f, 0xaa, 0xfa, 0x3e,
   0x75, 0xfd, 0x3f, 0xff, 0x57, 0x15, 0x75, 0xfd, 0x3f, 0xaa, 0xfa, 0x3e,
   0x75, 0xfd, 0x3f, 0xaa, 0xfa, 0x3e, 0x75, 0xfd, 0x3f, 0x20, 0xa8, 0x2b,
   0x20, 0x50, 0x15, 0x20, 0xa8, 0x2b, 0x20, 0x50, 0x15, 0x20, 0xa8, 0x2b,
   0xff, 0xff, 0x3f, 0x20, 0xa8, 0x2b, 0x20, 0x50, 0x15, 0x20, 0xa8, 0x2b,
   0x20, 0x50, 0x15, 0x20, 0xa8, 0x2b
};

/*
 * The resources specific to the plaid widget.
 */
static XtResource resources[] = {
#define offset(field)	XtOffset(PlaidWidget,plaid.field)
    { XtNcallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	offset(input_callback), XtRCallback, NULL },
    { XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
        offset(foreground), XtRString, "XtDefaultForeground" }
#undef offset
};

/*
 * ButtonAction - called when a button is pressed.
 */
static void
ButtonAction(w, event, params, num_params)
Cardinal *num_params;	/* unused */
String *params;		/* unused */
XEvent *event;
Widget w;
{
	/*
	 * Just pass it off to the user's callback function.
	 */
	XtCallCallbacks(w, XtNcallback, (caddr_t) event);
}

/*
 * RealizePlaid - realize the window by creating it, and by creating the
 *		  plaid background pixmap.
 */
static void
RealizePlaid(w, value_mask, attributes)
XSetWindowAttributes *attributes;
Mask *value_mask;
Widget w;
{
	Pixmap p;
	Window window;
	Display *display;
	PlaidWidget plaidw;

	plaidw = (PlaidWidget) w;

	/*
	 * Create the window.
	 */
	XtCreateWindow(w, (unsigned int) InputOutput,
			(Visual *) CopyFromParent, *value_mask, attributes);

	display = XtDisplay(w);
	window = XtWindow(w);

	/*
	 * Create the plaid pixmap.
	 */
	p = XCreatePixmapFromBitmapData(display, window, plaid_bits,
			plaid_width, plaid_height, plaidw->plaid.foreground,
			w->core.background_pixel, w->core.depth);

	/*
	 * Tile the window.
	 */
	XSetWindowBackgroundPixmap(display, window, p);

	/*
	 * Now save the pixmap in the core widget.
	 */
	w->core.background_pixmap = p;
}


/*
 * SetIcons - build and set the icon and iconify pixmaps.
 */

void SetIcons(Widget w)
{
    Pixmap p;
    Pixmap p_iconize;
    Pixmap p_icon;
    GC pgc;
    XGCValues gcv;
    Window window;
    Display *display;
    Screen  *scrn;
    Arg arg[2];

    display = XtDisplay(w);
    scrn = XtScreen(w);
    window = RootWindowOfScreen (scrn);
    p = XCreateBitmapFromData(display, window, plaid_bits,
		    plaid_width, plaid_height);
    p_iconize = XCreatePixmap(display, window, iconize_width,
		    iconize_height, 1);
    p_icon = XCreatePixmap(display, window, icon_width, icon_height, 1);
    gcv.tile = p;
    gcv.fill_style = FillTiled;
    pgc = XCreateGC(display, p_iconize, GCFillStyle | GCTile, &gcv);
    XFillRectangle(display, p_iconize, pgc, 0, 0, iconize_width,
	iconize_height);
    XFillRectangle(display, p_icon, pgc, 0, 0, icon_width, icon_height);

    XtSetArg(arg[0], XtNiconPixmap, p_icon);
    XtSetArg(arg[1], XtNiconifyPixmap, p_iconize);
    XtSetValues(w, arg, 2);
}


/*
 * DestroyPlaid - free up server resources when widget is destroyed.
 */
static void
DestroyPlaid(w)
Widget w;
{
	/*
	 * All we need to do is get rid of the pixmap we created.
	 */
	XFreePixmap(XtDisplay(w), w->core.background_pixmap);
}

/*
 * Action table.  We're only interested in button presses.
 */
static XtActionsRec actions[] =
{
	{ "button",	ButtonAction }
};

/*
 * Translation table.  We're only interested in button presses.
 */
static char translations[] =
	"<BtnDown>:		button()	\n\
";

/*
 * The plaid widget class record.   Initialization of values.
 */
PlaidClassRec plaidClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"Plaid",
    /* widget_size		*/	sizeof(PlaidRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	NULL,
    /* initialize_hook		*/	NULL,
    /* realize			*/	RealizePlaid,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	DestroyPlaid,
    /* resize			*/	NULL,
    /* expose			*/	NULL,
    /* set_values		*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* plaid fields */
    /* empty			*/	0
  }
};

/*
 * The class declaration.
 */
WidgetClass plaidWidgetClass = (WidgetClass)&plaidClassRec;
