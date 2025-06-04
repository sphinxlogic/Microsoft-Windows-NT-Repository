/* this is a simple progress meter facility.
 * the graphics are easy; the harder part is using it well.
 */

#include <stdio.h>
#if defined(__STDC__)
#include <stdlib.h>
#endif

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>

#include "P_.h"

extern Widget toplevel_w;
#define	XtD	XtDisplay(toplevel_w)

#define	PERPREC	3		/* percentage precision of display */
#if PERPREC<1
#define	PERPREC	1
#endif

extern void get_something P_((Widget w, char *resource, XtArgVal value));

static void pm_create_formw P_((void));
static void pm_exp_cb P_((Widget w, XtPointer client, XtPointer call));
static void show_progress P_((int p, int all));
static void make_gc P_((void));

static Widget pm_formw;		/* the main dialog form */
static Widget pm_da;		/* graphics drawing area */

static unsigned long pm_fgpix;	/* drawing meter foreground color */
static unsigned long pm_bgpix;	/* drawing meter background color */
static int last_per;		/* last percentage drawn */
static GC pm_gc;		/* GC */

/* called to manage or unmanage the progress meter */
void
pm_manage()
{
	if (!pm_formw)
	    pm_create_formw();

	if (XtIsManaged(pm_formw))
	    XtUnmanageChild (pm_formw);
	else
	    XtManageChild (pm_formw);
}

/* set the given percentage on the progress meter.
 */
void
pm_set (p)
int p;
{
	if (!pm_formw || !XtIsManaged(pm_formw))
	    return;

	show_progress (p, 0);
}

void
pm_cursor(c)
Cursor c;
{
	Window win;

	if (pm_formw && (win = XtWindow(pm_formw))) {
	    Display *dsp = XtDisplay(pm_formw);
	    if (c)
		XDefineCursor(dsp, win, c);
	    else
		XUndefineCursor(dsp, win);
	}
}

/* create the progress meter dialog */
static void
pm_create_formw()
{
	Arg args[20];
	int n;

	/* create form */

	n = 0;
	XtSetArg (args[n], XmNverticalSpacing, 2); n++;
	XtSetArg (args[n], XmNhorizontalSpacing, 2); n++;
	pm_formw = XmCreateFormDialog (toplevel_w, "Progress", args,n);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Progress"); n++;
	XtSetValues (XtParent(pm_formw), args, n);

	/* create the drawing area */
	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	pm_da = XmCreateDrawingArea (pm_formw, "DA", args, n);
	XtAddCallback (pm_da, XmNexposeCallback, pm_exp_cb, NULL);
	XtManageChild (pm_da);
}

/* called when the drawing area gets an expose.
 */
/* ARGSUSED */
static void
pm_exp_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;

	switch (c->reason) {
	case XmCR_EXPOSE: {
	    /* turn off gravity so we get expose events for either shrink or
	     * expand.
	     */
	    static before;
	    XExposeEvent *e = &c->event->xexpose;

	    if (!before) {
		XSetWindowAttributes swa;
                unsigned long mask = CWBitGravity;

		swa.bit_gravity = ForgetGravity;
		XChangeWindowAttributes (e->display, e->window, mask, &swa);
		before = 1;
	    }

	    /* wait for the last in the series */
	    if (e->count != 0)
		return;
	    break;
	    }
	default:
	    printf ("Unexpected pm_formw event. type=%d\n", c->reason);
	    exit(1);
	}

	show_progress (last_per, 1);
}

/* display the given percentage graphically to the nearest PERPREC percentage.
 * redraw everything if all is set or if per < last_per.
 * do as little as possible if we aren't going to do any drawing.
 * update last_per when finished.
 */
static void
show_progress (p, all)
int p;
int all;
{
	int pbin;

	if (!pm_gc)
	    make_gc();

	/* round to nearest PERPREC*/
	pbin = ((p+(PERPREC/2))/PERPREC) * PERPREC;

	if (all || pbin != last_per) {
	    Window win = XtWindow(pm_da);
	    Dimension daw, dah;
	    int gap;
	    int perw;

	    get_something (pm_da, XmNwidth, (XtArgVal)&daw);
	    get_something (pm_da, XmNheight, (XtArgVal)&dah);

	    perw = pbin*(int)daw/100;

	    if (all || pbin < last_per) {
		XSetForeground (XtD, pm_gc, pm_bgpix);
		XFillRectangle (XtD, win, pm_gc, 0, 0, (int)daw, (int)dah);
	    }
 

	    gap = dah/6;
	    XSetForeground (XtD, pm_gc, pm_fgpix);
	    XFillRectangle (XtD, win, pm_gc, 0, gap, perw, (int)dah-2*gap);

	    XSync (XtD, False);

	    last_per = pbin;
	}
}

/* make the pm_gc GC and extablish the pm_{f,b}gpix pixels. */
static void
make_gc()
{
	pm_gc = XCreateGC (XtD, XtWindow(pm_da), 0L, NULL);

	get_something (pm_da, XmNforeground, (XtArgVal)&pm_fgpix);
	get_something (pm_da, XmNbackground, (XtArgVal)&pm_bgpix);
}
