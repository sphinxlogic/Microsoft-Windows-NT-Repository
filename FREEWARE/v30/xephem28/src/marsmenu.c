/* code to manage the stuff on the "mars" menu.
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#if defined(__STDC__)
#include <stdlib.h>
#endif
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawingA.h>
#include <Xm/Separator.h>
#include <Xm/ToggleB.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "map.h"


extern Widget	toplevel_w;
#define	XtD	XtDisplay(toplevel_w)

/* from marsmap.c */
extern MRegion mreg[];
extern int nmreg;

extern Now *mm_get_now P_((void));
extern Obj *db_basic P_((int id));
extern int any_ison P_((void));
extern void db_update P_((Obj *op));
extern void f_double P_((Widget w, char *fmt, double f));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void register_selection P_((char *name));
extern void timestamp P_((Now *np, Widget w));

static void mars_create_form P_((void));
static void mars_cml_cb P_((Widget w, XtPointer client, XtPointer call));
static void mars_close_cb P_((Widget w, XtPointer client, XtPointer call));
static void mars_help_cb P_((Widget w, XtPointer client, XtPointer call));
static void mars_exp_cb P_((Widget w, XtPointer client, XtPointer call));
static void mars_set_buttons P_((int whether));
static double mars_cml P_((Now *np));
static void mars_map P_((Now *np, double cml));

#define	M_CML0	degrad(325.845)		/* Mars' CML towards Aries at M_MJD0 */
#define	M_PER	degrad(350.891962)	/* Mars' rotation period, rads/day */
#define	M_MJD0	(2418322.0 - MJD0)	/* mjd date of M_CML0 */

static int mars_selecting;	/* set while our fields are being selected */

static Widget marsform_w;	/* main form dialog */
static Widget mars_cml_w;	/* CML numberic display widget */
static Widget mars_da_w;	/* map DrawingArea widget */
static Widget mars_dt_w;	/* data/time stamp label widget */

#define	DSIN(x)		sin(degrad(x))
#define	DCOS(x)		cos(degrad(x))

#ifdef __STDC__
static double mars_cml(Now *np);
#else
static double mars_cml();
#endif

void
mars_manage()
{
	if (!marsform_w)
	    mars_create_form();

	if (XtIsManaged(marsform_w))
	    XtUnmanageChild(marsform_w);
	else {
	    XtManageChild(marsform_w);
	    mars_set_buttons(mars_selecting);
	}
}

/* display the map and CML info for circumstances described by *np..
 * don't bother with the computation of CML unless we are managed or someone
 *   cares.
 * don't bother with the map unless we are managed.
 */
void
mars_update(np, force)
Now *np;
int force;
{
	double cml;

	if (!marsform_w)
	    return;
	if (!XtIsManaged(marsform_w) && !any_ison() && !force)
	    return;

	cml = mars_cml (np);
	f_double (mars_cml_w, "%5.1f", cml);

	if (XtIsManaged(marsform_w))
	    mars_map (np, cml);
}

int
mars_ison()
{
	return (marsform_w && XtIsManaged(marsform_w));
}

/* called by other menus as they want to hear from our buttons or not.
 * the ons and offs stack.
 */
void
mars_selection_mode (whether)
int whether;
{
	mars_selecting += whether ? 1 : -1;

	if (mars_ison())
	    if ((whether && mars_selecting == 1)     /* first one to want on */
		|| (!whether && mars_selecting == 0) /* first one to want off*/)
		mars_set_buttons (whether);
}

void
mars_cursor(c)
Cursor c;
{
	Window win;

	if (marsform_w && (win = XtWindow(marsform_w))) {
	    Display *dsp = XtDisplay(marsform_w);
	    if (c)
		XDefineCursor(dsp, win, c);
	    else
		XUndefineCursor(dsp, win);
	}
}

static void
mars_create_form()
{
	static struct {
	    char *name;
	    void (*cb)();
	} ctls[] = {
	    {"Close", mars_close_cb},
	    {"Help", mars_help_cb}
	};
	Widget w;
	Widget fr_w;
	Widget f_w;
	Widget sep_w;
	XmString str;
	Arg args[20];
	int n;
	int i;

	/* create form */
	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNdefaultPosition, False); n++;
	XtSetArg (args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
	XtSetArg (args[n], XmNhorizontalSpacing, 4); n++;
	XtSetArg (args[n], XmNverticalSpacing, 4); n++;
	marsform_w = XmCreateFormDialog (toplevel_w, "Mars", args, n);
	XtAddCallback (marsform_w, XmNhelpCallback, mars_help_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Mars view"); n++;
	XtSetValues (XtParent(marsform_w), args, n);

	/* make the bottom control panel form */

	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNfractionBase, 9); n++;
	XtSetArg (args[n], XmNverticalSpacing, 5); n++;
	f_w = XmCreateForm (marsform_w, "CtlF", args, n);
	XtManageChild (f_w);

	    for (i = 0; i < XtNumber(ctls); i++) {
		n = 0;
		XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
		XtSetArg (args[n], XmNleftPosition, 1+i*4); n++;
		XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
		XtSetArg (args[n], XmNrightPosition, 4+i*4); n++;
		w = XmCreatePushButton (f_w, ctls[i].name, args, n);
		XtManageChild (w);
		XtAddCallback (w, XmNactivateCallback, ctls[i].cb, NULL);
	    }

	/* make a separator */

	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, f_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (marsform_w, "Sep", args, n);
	XtManageChild(sep_w);

	/* make the "cml" label and push button pair */

	str = XmStringCreateLtoR("Central Meridian Longitude (degs): ",
					    XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	XtSetArg (args[n], XmNlabelString, str); n++;
	w = XmCreateLabel (marsform_w, "CMLLbl", args, n);
	XtManageChild (w);
	XmStringFree(str);

	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, sep_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNleftWidget, w); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	XtSetArg (args[n], XmNuserData, "Mars.CML"); n++;
	mars_cml_w = XmCreatePushButton (marsform_w, "CMLVal", args, n);
	XtAddCallback (mars_cml_w, XmNactivateCallback, mars_cml_cb, 0);
	XtManageChild (mars_cml_w);

	/* make the date/time indicator label */

	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, mars_cml_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	mars_dt_w = XmCreateLabel (marsform_w, "DTstamp", args, n);
	XtManageChild (mars_dt_w);

	/* make a drawing area in a frame on top */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, mars_dt_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNshadowType, XmSHADOW_ETCHED_OUT); n++;
	fr_w = XmCreateFrame (marsform_w, "MarsFrame", args, n);
	XtManageChild (fr_w);

	    n = 0;
	    mars_da_w = XmCreateDrawingArea (fr_w, "Map", args, n);
	    XtAddCallback (mars_da_w, XmNexposeCallback, mars_exp_cb, NULL);
	    XtManageChild (mars_da_w);
}

/* callback for when the mars CML button is activated. */
/* ARGSUSED */
static void
mars_cml_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (mars_selecting) {
	    char *userD;
	    get_something (w, XmNuserData, (XtArgVal)&userD);
	    register_selection (userD);
	}
}

/* callback for when the Close button is activated. */
/* ARGSUSED */
static void
mars_close_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (marsform_w);
}

/* callback from the Help button
 */
/* ARGSUSED */
static void
mars_help_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static char *msg[] = {
"This is a simple schematic depiction of the Martian surface at the given time.",
"Also included is the current Central Meridian Longitude, in degrees.",
"South is up and longitude increases to the right; therefore, the planet",
"appears to rotate CW as seen from above."
};

	hlp_dialog ("Mars", msg, XtNumber(msg));
}

/* called whenever the mars drawing area gets an expose.
 */
/* ARGSUSED */
static void
mars_exp_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;

	/* filter out a few oddball cases */
	switch (c->reason) {
	case XmCR_EXPOSE: {
	    /* turn off gravity so we get expose events for either shrink or
	     * expand.
	     */
	    static before;
	    XExposeEvent *e = &c->event->xexpose;

	    if (!before) {
		XSetWindowAttributes swa;
		swa.bit_gravity = ForgetGravity;
		XChangeWindowAttributes (e->display, e->window, 
							    CWBitGravity, &swa);
		before = 1;
	    }
	    /* wait for the last in the series */
	    if (e->count != 0)
		return;
	    break;
	    }
	default:
	    printf ("Unexpected marsform_w event. type=%d\n", c->reason);
	    exit(1);
	}

	mars_update (mm_get_now(), 1);
}

/* go through all the buttons and set whether they
 * should appear to look like buttons or just flat labels.
 */
static void
mars_set_buttons (whether)
int whether;	/* whether setting up for plotting or for not plotting */
{
	static Arg look_like_button[] = {
	    {XmNtopShadowColor, (XtArgVal) 0},
	    {XmNbottomShadowColor, (XtArgVal) 0},
            {XmNtopShadowPixmap, (XtArgVal) 0},
            {XmNbottomShadowPixmap, (XtArgVal) 0},
	    {XmNfillOnArm, (XtArgVal) True},
	    {XmNtraversalOn, (XtArgVal) True},
	};
	static Arg look_like_label[] = {
	    {XmNtopShadowColor, (XtArgVal) 0},
	    {XmNbottomShadowColor, (XtArgVal) 0},
            {XmNtopShadowPixmap, (XtArgVal) 0},
            {XmNbottomShadowPixmap, (XtArgVal) 0},
	    {XmNfillOnArm, (XtArgVal) False},
	    {XmNtraversalOn, (XtArgVal) False},
	};
	static int called;
	Arg *ap;
	int na;

	if (!called) {
	    /* get baseline label and shadow appearances.
	     */
            Pixel topshadcol, botshadcol, bgcol;
            Pixmap topshadpm, botshadpm;
	    Arg args[20];
	    int n;

	    n = 0;
	    XtSetArg (args[n], XmNtopShadowColor, &topshadcol); n++;
	    XtSetArg (args[n], XmNbottomShadowColor, &botshadcol); n++;
            XtSetArg (args[n], XmNtopShadowPixmap, &topshadpm); n++;
            XtSetArg (args[n], XmNbottomShadowPixmap, &botshadpm); n++;
	    XtSetArg (args[n], XmNbackground, &bgcol); n++;
	    XtGetValues (mars_cml_w, args, n);

            look_like_button[0].value = topshadcol;
            look_like_button[1].value = botshadcol;
            look_like_button[2].value = topshadpm;
            look_like_button[3].value = botshadpm;
            look_like_label[0].value = bgcol;
            look_like_label[1].value = bgcol;
            look_like_label[2].value = XmUNSPECIFIED_PIXMAP;
            look_like_label[3].value = XmUNSPECIFIED_PIXMAP;

	    called = 1;
	}

	if (whether) {
	    ap = look_like_button;
	    na = XtNumber(look_like_button);
	} else {
	    ap = look_like_label;
	    na = XtNumber(look_like_label);
	}

	XtSetValues (mars_cml_w, ap, na);
}

/* return the Martian central meridian longitude at np, in degrees. */
static double
mars_cml(np)
Now *np;
{
	Obj *sp, *mp;
	double a;	/* angle from Sun ccw to Earth seen from Mars, rads */
	double Ae;	/* planetocentric longitude of Earth from Mars, rads */
	double cml0;	/* Mar's CML towards Aries, rads */
	double lc;	/* Mars rotation correction for light travel, rads */
	double cml;

	sp = db_basic (SUN);
	db_update (sp);
	mp = db_basic (MARS);
	db_update (mp);

	a = asin (sp->s_edist/mp->s_edist*sin(mp->s_hlong-sp->s_hlong));
	Ae = mp->s_hlong + PI + a;
	cml0 = M_CML0 + M_PER*(mjd-M_MJD0) + PI/2;
	range(&cml0, 2*PI);
	lc = LTAU * mp->s_edist/SPD*M_PER;
	cml = cml0 - Ae - lc;
	range (&cml, 2*PI);
	cml = raddeg(cml);
	/*
	printf ("a=%g Ae=%g cml0=%g lc=%g cml=%g\n", raddeg(a), raddeg(Ae),
					    raddeg(cml0), raddeg(lc), cml);
	 */
	return (cml);
}

static void
mars_map(np, cml)
Now *np;
double cml;
{
	static unsigned long last_w, last_h;
	static Pixmap mars_pm;
	static GC mars_gc;
	static Pixel mars_fg, mars_bg;
	Display *dsp = XtDisplay(mars_da_w);
	Window win = XtWindow(mars_da_w);
	int x, y;
	unsigned w, h, b, d;
	MRegion *rp;
	XPoint p[128];
	int npts;
	Window root;
	int wb, hb, r;
	MCoord *cp;
	int maxx, minx, maxy, miny;

	/* make the GC if this is our first time */
	if (!mars_gc) {
	    XGCValues gcv;
	    unsigned long gcm;

	    gcm = 0;
	    mars_gc = XCreateGC (dsp, win, gcm, &gcv);

	    get_something (mars_da_w, XmNforeground, (XtArgVal)&mars_fg);
	    get_something (mars_da_w, XmNbackground, (XtArgVal)&mars_bg);
	}

	/* set up width and height borders and the radius so the
	 * planetary sphere is a circle centered within the window.
	 */
	XGetGeometry (dsp, win, &root, &x, &y, &w, &h, &b, &d);
	if (w > h) {
	    wb = (w - h)/2;
	    hb = 0;
	    r = h/2;
	} else {
	    wb = 0;
	    hb = (h - w)/2;
	    r = w/2;
	}

	/* make the pixmap if this is our first time or the size has changed */
	if (!mars_pm || w != last_w || h != last_h) {
	    last_w = w;
	    last_h = h;
	    if (mars_pm)
		XFreePixmap (dsp, mars_pm);
	    mars_pm = XCreatePixmap (dsp, win, w, h, d);
	}

	/* clear the scene, draw filled circle in foreground then fill in
	 * regions in background. Then make another pass and overlay the names.
	 * we draw south (negative latitudes) downwards (in X's +y diretion)
	 * and with increasing longitudes progressing to the right. this
	 * means the planet appears to rotate CW as seen from above.
	 */
	XSetFunction (dsp, mars_gc, GXcopy);
	XSetForeground (dsp, mars_gc, mars_bg);
	XFillRectangle (dsp, mars_pm, mars_gc, 0, 0, w, h);
	XSetForeground (dsp, mars_gc, mars_fg);
	XFillArc (dsp, mars_pm, mars_gc, wb, hb, 2*r, 2*r, 0, 360*64);
	XSetForeground (dsp, mars_gc, mars_bg);
	for (rp = mreg; rp < mreg+nmreg; rp++) {
	    npts = 0;
	    for (cp = rp->mcp; cp < rp->mcp+rp->nmcp; cp++) {
		double dlg = cp->lg - cml;
		range (&dlg, 360.0);
		if (dlg >= 90 && dlg <= 270)
		    goto skip_region;
		p[npts].x = wb + r*(1 + DSIN(dlg)*DCOS(cp->lt));
		p[npts].y = hb + r*(1 + DSIN(cp->lt));
		npts++;
	    }
	    XFillPolygon (dsp, mars_pm, mars_gc, p, npts,Nonconvex,CoordModeOrigin);

	  skip_region:
		continue;
	}

	XSetFunction (dsp, mars_gc, GXxor);
	XSetForeground (dsp, mars_gc, mars_fg^mars_bg);
	for (rp = mreg; rp < mreg+nmreg; rp++) {
	    maxx = 0; minx = w; maxy = 0; miny = h;
	    for (cp = rp->mcp; cp < rp->mcp+rp->nmcp; cp++) {
		double dlg = cp->lg - cml;
		range (&dlg, 360.0);
		if (dlg >= 90 && dlg <= 270)
		    goto skip_name;
		x = wb + r*(1 + DSIN(dlg)*DCOS(cp->lt));
		y = hb + r*(1 + DSIN(cp->lt));
		if (x > maxx) maxx = x;
		if (x < minx) minx = x;
		if (y > maxy) maxy = y;
		if (y < miny) miny = y;
	    }
	    XDrawString (dsp, mars_pm, mars_gc, (minx+maxx)/2, (miny+maxy)/2,
		rp->rname, strlen(rp->rname));

	  skip_name:
		continue;
	}

	XSetFunction (dsp, mars_gc, GXcopy);
	XCopyArea (dsp, mars_pm, win, mars_gc, 0, 0, w, h, 0, 0);

	timestamp (np, mars_dt_w);
}
