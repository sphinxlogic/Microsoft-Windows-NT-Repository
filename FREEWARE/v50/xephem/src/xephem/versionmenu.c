/* code to manage the stuff on the version display.
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#if defined(__STDC__)
#include <stdlib.h>
#endif
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Text.h>
#include <Xm/Scale.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "ps.h"
#include "patchlevel.h"

extern Widget toplevel_w;
extern Colormap xe_cm;
extern XtAppContext xe_app;

extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void set_something P_((Widget w, char *resource, XtArgVal value));

static void v_makew P_((void));
static void fill_msg P_((Widget w));
static void v_unmap_cb P_((Widget w, XtPointer client, XtPointer call));
static void v_da_exp_cb P_((Widget w, XtPointer client, XtPointer call));
static void v_draw P_((void));
static void v_timer_cb P_((XtPointer client, XtIntervalId *id));
static void drawComet P_((Display *dsp, Window win, GC gc, double ang, int rad,
    int tlen, int w, int h));
static void drawPlanet P_((Display *dsp, Window win, GC gc, int sx, int sy,
    int w, int h));
static void v_define_fgc P_((void));

static char *msg[] = {
"XEphem - An Interactive Astronomical Ephemeris Program for X Windows",
NULL,	/* filled in dynamically */
"",
"Copyright (c) 1990-1999 by Elwood Charles Downey",
"ecdowney@ClearSkyInstitute.com",
"",
"Permission is granted to use XEphem and its source code for personal",
"non-profit non-commercial purposes only, provided this notice is prominently",
"displayed. All other rights reserved. Contact the author before using XEphem",
"for any other purposes.",
"",
"No representation is made about the suitability of this software for any",
"purpose. It is provided \"as is\" without express or implied warranty.",
"",
"Stay in touch with the latest XEphem developments at",
"http://www.ClearSkyInstitute.com/xephem",
"",
};


#define	NMSGR	(sizeof(msg)/sizeof(msg[0]))

/* generate a random number between min and max, of the same type as the
 * highest type of either.
 */
#define	RAND(min,max)	(((rand() & 0xfff)*((max)-(min))/0xfff) + min)

static Widget v_w;
static Widget vda_w;
static XtIntervalId v_timer_id;
static GC v_fgc;
static double rotrate;	/* rotation rate constant - filled on first manage */

/* table of circular orbit radii to portray and the last screen coords.
 * the real solar system has planet radii from .3 to 30, but the 100:1 ratio is
 * so large we don't try and include everything.
 */
typedef struct {
    double r;		/* radius, AU */
    double theta;	/* angle */
    int x, y;		/* last X x,y coord drawn */
} Orbit;

#define	UNDEFX	(-1)		/* value of x when never drawn yet */
static Orbit orbit[] = {
    {1.6, 0.0, UNDEFX, 0},
    {5.4, 0.0, UNDEFX, 0},
    {10., 0.0, UNDEFX, 0},
    {19., 0.0, UNDEFX, 0},
    {30., 0.0, UNDEFX, 0}
};
#define NORBIT	(sizeof(orbit)/sizeof(orbit[0]))
#define	MAXRAD	(orbit[NORBIT-1].r)	/* N.B.use orbit[] with largest radius*/
#define	MINRAD	(orbit[0].r)	/* N.B. use orbit[] with smallest radius */
#define PR 	4		/* radius of planet, pixels */
#define	DT	100		/* pause between screen steps, ms */
#define	NSTARS	100		/* number of background stars to sprinkle in */
#define	DPI	30		/* inner orbit motion per step, degrees*/

/* comet state and info */
#define	CMAXPERI 30		/* max comet perihelion, pixels */
#define	CMAXTAIL 50		/* max comet tail length, pixels */
#define	CMINTAIL 3		/* min comet tail length, pixels */
#define	CMAXDELA 20		/* max comet area per step, sqr pixels */
#define	CMINDELA 10		/* min comet area per step, sqr pixels */

static double angle;		/* angle ccw from straight right, rads */
static double rotation;		/* whole scene rot, rads */
static int radius;		/* dist from sun, pixels (0 means undefined) */
static int taillen;		/* tail length, pixels */
static int delta_area;		/* change in area per step, sqr pixels */
static int perihelion;		/* min dist from sun, pixels */
static int maxtail;		/* max tail len (ie, tail@peri), pixels */

/* called when mainmenu "About.." help is selected.
 */
void
version()
{
	/* make the version form if this is our first time.
	 * also take this opportunity to do things once to init the
	 * planet locations and set the rotation rate.
	 */
	if (!v_w) {
	    int i;
	    v_makew();
	    for (i = 0; i < NORBIT; i++)
		orbit[i].theta = RAND(0,2*PI);
	    rotrate = degrad(DPI)/pow(MINRAD/MAXRAD, -3./2.);
	}
	    
	/* toggle whether up now.
	 * autoUnmanage can bring back down too.
	 */
	if (XtIsManaged(v_w))
	    XtUnmanageChild (v_w);
	else
	    XtManageChild (v_w);
}

/* called to put up or remove the watch cursor.  */
void
v_cursor (c)
Cursor c;
{
	Window win;

	if (v_w && (win = XtWindow(v_w)) != 0) {
	    Display *dsp = XtDisplay(v_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}

/* make the v_w widget.
 */
static void
v_makew()
{
	Widget ok_w;
	Widget frame_w;
	Widget text_w;
	XmString str;
	Arg args[20];
	int n;

	/* create form */
	n = 0;
	XtSetArg (args[n], XmNresizePolicy, XmRESIZE_ANY); n++;
	XtSetArg (args[n], XmNautoUnmanage, True); n++;
	XtSetArg (args[n], XmNhorizontalSpacing, 5); n++;
	XtSetArg (args[n], XmNverticalSpacing, 5); n++;
	XtSetArg (args[n], XmNcolormap, xe_cm); n++;
	v_w = XmCreateFormDialog (toplevel_w, "About", args, n);
	set_something (v_w, XmNcolormap, (XtArgVal)xe_cm);
	XtAddCallback (v_w, XmNunmapCallback, v_unmap_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem About"); n++;
	XtSetValues (XtParent(v_w), args, n);

	/* make text widget for the version info */

	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNbottomPosition, 50); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg (args[n], XmNeditable, False); n++;
	XtSetArg (args[n], XmNcolumns, 80); n++;
	XtSetArg (args[n], XmNrows, NMSGR); n++;
	text_w = XmCreateScrolledText (v_w, "VText", args, n);
	fill_msg (text_w);
	XtManageChild (text_w);

	/* make the "Ok" push button */

	str = XmStringCreate("Ok", XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 5); n++;
	XtSetArg (args[n], XmNlabelString, str); n++;
	ok_w = XmCreatePushButton (v_w, "VOk", args, n);
	set_something (v_w, XmNdefaultButton, (XtArgVal)ok_w);
	XtManageChild (ok_w);
	XmStringFree (str);

	/* make a frame for the drawing area */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, XtParent(text_w)); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, ok_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 20); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 80); n++;
	XtSetArg (args[n], XmNshadowType, XmSHADOW_ETCHED_OUT); n++;
	frame_w = XmCreateFrame (v_w, "VFrame", args, n);
	XtManageChild (frame_w);

	/* make a drawing area for drawing the solar system */

	n = 0;
	vda_w = XmCreateDrawingArea (frame_w, "AboutMap", args, n);
	XtAddCallback (vda_w, XmNexposeCallback, v_da_exp_cb, 0);
	XtAddCallback (vda_w, XmNresizeCallback, v_da_exp_cb, 0);
	XtManageChild (vda_w);
}

static void
fill_msg (w)
Widget w;
{
	static char line1[100];
	char m[100*NMSGR], *mp = m;
	int i;

	(void) sprintf (line1, "Version %s %s", PATCHLEVEL, PATCHDATE);
	msg[1] = line1;

	/* Generate message to display as one string */
	for (i = 0; i < NMSGR; i++) {
	    (void) sprintf (mp, "%*s%s\n", (78 - (int)strlen(msg[i]))/2, "",
									msg[i]);
	    mp += strlen(mp);
	}

	/* remove final \n to avoid extra blank line at end */
	*--mp = '\0';

	XmTextSetString (w, m);
}

/* version dialog is going away.
 * stop the rotation timer.
 */
/* ARGSUSED */
static void
v_unmap_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (v_timer_id) {
	    XtRemoveTimeOut (v_timer_id);
	    v_timer_id = 0;
	}
}

/* expose version drawing area.
 * redraw the scene to the (new?) size.
 * start timer if not going already.
 */
/* ARGSUSED */
static void
v_da_exp_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;

	switch (c->reason) {
	case XmCR_RESIZE:
	    /* seems we can get one resize before the first expose.
	     * hence, we don't have a good window to use yet. just let it
	     * go; we'll get the expose soon.
	     */
	    if (!XtWindow(w))
		return;
	    break;
	case XmCR_EXPOSE: {
	    XExposeEvent *e = &c->event->xexpose;
	    /* wait for the last in the series */
	    if (e->count != 0)
		return;
	    break;
	    }
	default:
	    printf ("Unexpected v_w event. type=%d\n", c->reason);
	    exit(1);
	}

	v_draw();

	if (!v_timer_id)
	    v_timer_id = XtAppAddTimeOut (xe_app, DT, v_timer_cb, 0);
}

static void
v_draw()
{
	Display *dsp = XtDisplay(vda_w);
	Window win = XtWindow(vda_w);
	unsigned int w, h;
	Window root;
	int x, y;
	unsigned int bw, d;
	int i;

	if (!v_fgc)
	    v_define_fgc();

	XGetGeometry(dsp, win, &root, &x, &y, &w, &h, &bw, &d);
	XClearWindow (dsp, win);

	/* draw the orbit ellipsii and forget last drawn locs */
	for (i = 0; i < NORBIT; i++) {
	    int lx, ty;	/* left and top x */
	    int nx, ny; /* width and height */
	    lx = w/2 - orbit[i].r/MAXRAD*w/2 + 0.5;
	    nx = orbit[i].r/MAXRAD*w + 0.5;
	    ty = h/2 - orbit[i].r/MAXRAD*h/2 + 0.5;
	    ny = orbit[i].r/MAXRAD*h + 0.5;
	    XDrawArc (dsp, win, v_fgc, lx, ty, nx-1, ny-1, 0, 360*64);
	    orbit[i].x = UNDEFX;
	}

	/* forget the comet */
	radius = 0;

	/* draw sun at the center */
	drawPlanet (dsp, win, v_fgc, w/2-PR, h/2-PR, 2*PR-1, 2*PR-1);

	/* draw some background stars */
	for (i = 0; i < NSTARS; i++) {
	    int sx, sy;
	    sx = RAND(0,w-1);
	    sy = RAND(0,h-1);
	    XDrawPoint (dsp, win, v_fgc, sx, sy);
	}
}

/* called whenever the timer goes off.
 * we advance all the planets, draw any that have moved at least a few
 * pixels, and restart a timer.
 */
/* ARGSUSED */
static void
v_timer_cb (client, id)
XtPointer client;
XtIntervalId *id;
{
	Display *dsp = XtDisplay(vda_w);
	Window win = XtWindow(vda_w);
	unsigned int w, h;
	Window root;
	int x, y;
	unsigned int bw, d;
	int i;

	XGetGeometry(dsp, win, &root, &x, &y, &w, &h, &bw, &d);

	for (i = 0; i < NORBIT; i++) {
	    int px, py;	/* planets new center position */
	    double f = orbit[i].r/MAXRAD;	/* fraction of largest radius */
	    orbit[i].theta += rotrate*pow(f, -3./2.);
	    px = w/2 + cos(orbit[i].theta)*w*f/2 + 0.5;
	    py = h/2 - sin(orbit[i].theta)*h*f/2 + 0.5;
	    if (px != orbit[i].x || py != orbit[i].y) {
		/* erase then redraw at new pos, using the XOR GC */
		if (orbit[i].x != UNDEFX)
		    drawPlanet (dsp, win, v_fgc,
				orbit[i].x-PR, orbit[i].y-PR, 2*PR-1, 2*PR-1);
		drawPlanet (dsp, win, v_fgc, px-PR, py-PR, 2*PR-1, 2*PR-1);
		orbit[i].x = px;
		orbit[i].y = py;
	    }
	}

	/* erase last comet position.
	 * N.B. use radius == 0 to mean the very first loop.
	 */
	if (radius != 0)
	    drawComet (dsp, win, v_fgc, angle, radius, taillen, w, h);

	/* comet is definitely outside scene, set fresh initial conditions.
	 */
	if (radius <= 0 || radius > (w+h)/2) {
	    radius = (w+h)/2;
	    rotation = RAND(0,2*PI);
	    perihelion = RAND(PR,CMAXPERI);
	    maxtail = RAND(CMINTAIL,CMAXTAIL);
	    delta_area = RAND(CMINDELA,CMAXDELA);
	    angle = acos(1.0 - 2.0*perihelion/radius) + rotation;
#if 0
	    printf ("initial rad=%d rot=%g peri=%d maxt=%d da=%d angle=%g\n",
		    radius, rotation, perihelion, maxtail, delta_area, angle);
#endif
	}

	/* recompute next step location and draw new comet
	 */
#if 0
	printf ("rad=%d rot=%g peri=%d maxt=%d da=%d angle=%g\n",
		    radius, rotation, perihelion, maxtail, delta_area, angle);
#endif
	angle += (double)delta_area/(radius*radius);
	radius = 2*perihelion/(1.0 - cos(angle - rotation));
	taillen = (maxtail*perihelion*perihelion)/(radius*radius);
	drawComet (dsp, win, v_fgc, angle, radius, taillen, w, h);

	/* restart timer */
	v_timer_id = XtAppAddTimeOut (xe_app, DT, v_timer_cb, 0);
}

/* draw the comet
 */
static void
drawComet (dsp, win, gc, ang, rad, tlen, w, h)
Display *dsp;
Window win;
GC gc;
double ang;	/* desired angle ccw from +x, in rads */
int rad;	/* in pixels from center */
int tlen;	/* length of tail, in pixels */
int w, h;	/* window width and height */
{
	double ca, sa;
	int sx, sy;
	int ex, ey;

	if (tlen < CMINTAIL)
	    tlen = CMINTAIL;

	/* angle is made <0 to get ccw rotation with X's y-down coord system */
	ang = -ang;
	ca = cos(ang);
	sa = sin(ang);

	sx = w/2 + rad * ca;
	sy = h/2 + rad * sa;
	ex = w/2 + (rad+tlen) * ca;
	ey = h/2 + (rad+tlen) * sa;

	XDrawLine (dsp, win, gc, sx, sy, ex, ey);
}

/* draw the planet.
 */
static void
drawPlanet (dsp, win, gc, sx, sy, w, h)
Display *dsp;
Window win;
GC gc;
int sx, sy, w, h;
{
	XPSFillArc (dsp, win, gc, sx, sy, w, h, 0, 360*64);
}

static void
v_define_fgc()
{
	Display *dsp = XtDisplay(vda_w);
	Window win = XtWindow(vda_w);
	XGCValues gcv;
	unsigned int gcm;
	Pixel fg, bg;

	gcm = GCForeground | GCFunction;
	get_something (vda_w, XmNforeground, (XtArgVal)&fg);
	get_something (vda_w, XmNbackground, (XtArgVal)&bg);
	gcv.foreground = fg ^ bg;
	gcv.function = GXxor;
	v_fgc = XCreateGC (dsp, win, gcm, &gcv);
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: versionmenu.c,v $ $Date: 1999/10/08 15:45:48 $ $Revision: 1.6 $ $Name:  $"};
