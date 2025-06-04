/* misc handy X Windows functions.
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#if defined(__STDC__)
#include <stdlib.h>
#endif
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "preferences.h"

extern Widget	toplevel_w;
extern char	*myclass;
#define	XtD	XtDisplay(toplevel_w)

extern void xe_msg P_((char *msg, int app_modal));

void get_views_font P_((Display *dsp, XFontStruct **fspp));

/* handy way to set one resource for a widget.
 * shouldn't use this if you have several things to set for the same widget.
 */
void
set_something (w, resource, value)
Widget w;
char *resource;
XtArgVal value;
{
	Arg a[1];

	if (!w) {
	    printf ("set_something (%s) called with w==0\n", resource);
	    exit(1);
	}

	XtSetArg (a[0], resource, value);
	XtSetValues (w, a, 1);
}

/* handy way to get one resource for a widget.
 * shouldn't use this if you have several things to get for the same widget.
 */
void
get_something (w, resource, value)
Widget w;
char *resource;
XtArgVal value;
{
	Arg a[1];

	if (!w) {
	    printf ("get_something (%s) called with w==0\n", resource);
	    exit(1);
	}

	XtSetArg (a[0], resource, value);
	XtGetValues (w, a, 1);
}

/* return the given XmString resource from the given widget as a char *.
 * N.B. based on a sample in Heller, pg 178, the string back from
 *   XmStringGetLtoR should be XtFree'd. Therefore, OUR caller should always
 *   XtFree (*txtp).
 */
void
get_xmstring (w, resource, txtp)
Widget w;
char *resource;
char **txtp;
{
	static char me[] = "get_xmstring()";
	static char hah[] = "??";

	if (!w) {
	    printf ("%s: called for %s with w==0\n", me, resource);
	    exit(1);
	} else {
	    XmString str;
	    get_something(w, resource, (XtArgVal)&str); 
	    if (!XmStringGetLtoR (str, XmSTRING_DEFAULT_CHARSET, txtp)) {
		/*
		fprintf (stderr, "%s: can't get string resource %s\n", me,
								resource);
		exit (1);
		*/
		(void) strcpy (*txtp = XtMalloc(sizeof(hah)), hah);
	    }
	    XmStringFree (str);
	}
}

void
set_xmstring (w, resource, txt)
Widget w;
char *resource;
char *txt;
{
	XmString str;

	if (!w) {
	    printf ("set_xmstring called for %s with w==0\n", resource);
	    return;
	}

	str = XmStringCreateLtoR (txt, XmSTRING_DEFAULT_CHARSET);
	set_something (w, resource, (XtArgVal)str);
	XmStringFree (str);
}

/* may be connected as the mapCallback to any convenience Dialog to
 * position it centered the cursor (allowing for the screen edges).
 */
/* ARGSUSED */
void
prompt_map_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	Window root, child;
	int rx, ry, wx, wy;	/* rx/y: cursor loc on root window */
	unsigned sw, sh;	/* screen width/height */
	Dimension ww, wh;	/* this widget's width/height */
	Position x, y;		/* final location */
	unsigned mask;
	Arg args[20];
	int n;

	XQueryPointer (XtDisplay(w), XtWindow(w),
				&root, &child, &rx, &ry, &wx, &wy, &mask);
	sw = WidthOfScreen (XtScreen(w));
	sh = HeightOfScreen(XtScreen(w));
	n = 0;
	XtSetArg (args[n], XmNwidth, &ww); n++;
	XtSetArg (args[n], XmNheight, &wh); n++;
	XtGetValues (w, args, n);

	x = rx - ww/2;
	if (x < 0)
	    x = 0;
	else if (x + ww >= sw)
	    x = sw - ww;
	y = ry - wh/2;
	if (y < 0)
	    y = 0;
	else if (y + wh >= sh)
	    y = sh - wh;

	n = 0;
	XtSetArg (args[n], XmNx, x); n++;
	XtSetArg (args[n], XmNy, y); n++;
	XtSetValues (w, args, n);
}

/* get the named color for w's colormap in *p, else set to w's XmNforeground.
 * return 0 if the color was found, -1 if w had to be used.
 */
int
get_color_resource (w, cname, p)
Widget w;
char *cname;
Pixel *p;
{
	Display *dsp = XtDisplay(w);
	Colormap cm;
	XColor defxc, dbxc;
	Arg arg;
	char *cval;

	XtSetArg (arg, XmNcolormap, &cm);
	XtGetValues (w, &arg, 1);
	cval = XGetDefault (dsp, myclass, cname);

	if (!cval || !XAllocNamedColor (dsp, cm, cval, &defxc, &dbxc)) {
	    char msg[128];
	    if (!cval)
		(void) sprintf (msg, "Can not find resource `%.80s'", cname);
	    else
		(void) sprintf (msg, "Can not XAlloc color `%.80s'", cval);
	    (void) sprintf (msg+strlen(msg), " ... so using %s.foreground.\n",
								    XtName(w));
	    xe_msg(msg, 0);
	    get_something (w, XmNforeground, (XtArgVal)p);
	    return (-1);
	} else {
	    *p = defxc.pixel;
	    return (0);
	}
}

/* given an object, return a GC for it.
 * Use the colors defined in the X resoucres, else use the foreground of the
 * given widget.
 */
void
obj_pickgc(op, w, gcp)
Obj *op;
Widget w;
GC *gcp;
{
	/* names of resource colors for the planets.
	 * N.B. should be in the same order as the defines in astro.h
	 */
	static char *objcolnames[NOBJ-2] = {
	    "mercuryColor", "venusColor", "marsColor", "jupiterColor",
	    "saturnColor", "uranusColor", "neptuneColor", "plutoColor",
	    "sunColor", "moonColor"
	};
	/* name of resource for all other solar system objects */
	static char solsyscolname[] = "solSysColor";
	/* names of resource colors for stellar types.
	 */
	static char *starcolnames[] = {
	    "hotStarColor", "mediumStarColor", "coolStarColor",
	    "otherStellarColor"
	};

	static GC objgcs[XtNumber(objcolnames)];
	static GC stargcs[XtNumber(starcolnames)];
	static GC solsys_gc;
	static GC other_gc;

	/* make all the gcs the first time through */
	if (!other_gc) {
	    Display *dsp = XtDisplay(w);
	    Window win = XtWindow(w);
	    unsigned long gcm = GCForeground;
	    XFontStruct *fsp;
	    XGCValues gcv;
	    Pixel p;
	    int i;

	    get_views_font (dsp, &fsp);
	    gcm |= GCFont;
	    gcv.font = fsp->fid;

	    /* make the planet gcs */
	    for (i = 0; i < XtNumber(objgcs); i++) {
		(void) get_color_resource (w, objcolnames[i], &p);
		gcv.foreground = p;
		objgcs[i] = XCreateGC (dsp, win, gcm, &gcv);
	    }

	    /* make the gc for other solar system objects */
	    (void) get_color_resource (w, solsyscolname, &p);
	    gcv.foreground = p;
	    solsys_gc = XCreateGC (dsp, win, gcm, &gcv);

	    /* make the star color gcs */
	    for (i = 0; i < XtNumber(stargcs); i++) {
		(void) get_color_resource (w, starcolnames[i], &p);
		gcv.foreground = p;
		stargcs[i] = XCreateGC (dsp, win, gcm, &gcv);
	    }

	    /* make the gc for everything else from foreground */
	    get_something (w, XmNforeground, (XtArgVal)&p);
	    gcv.foreground = p;
	    other_gc = XCreateGC (dsp, win, gcm, &gcv);
	}

	if (op->type == PLANET && op->pl.code < XtNumber(objgcs))
	    *gcp = objgcs[op->pl.code];
	else if (is_ssobj(op))
	    *gcp = solsys_gc;
	else if (op->type == FIXED)
	    switch (op->f_class) {
	    case 'Q': case 'J': case 'L': case 'T': case '\0':
		*gcp = stargcs[3];
		break;
	    case 'S': case 'B': case 'D': case 'M': case 'V':
		switch (op->f_spect[0]) {
		case 'O': case 'B': case 'A': case 'W':
		    *gcp = stargcs[0];
		    break;
		case 'F': case 'G':	/* FALLTHRU */
		default:
		    *gcp = stargcs[1];
		    break;
		case 'K': case 'M': case 'N': case 'R': case 'S':
		    *gcp = stargcs[2];
		    break;
		}
		break;
	    default:
		*gcp = other_gc;
		break;
	    }
	else
	    *gcp = other_gc;
}

/* given any widget built from an XmLabel return pointer to the first
 * XFontStruct in its XmFontList.
 */
void
get_xmlabel_font (w, f)
Widget w;
XFontStruct **f;
{
	static char me[] = "get_xmlable_font";
	XmFontList fl;
	XmFontContext fc;
	XmStringCharSet charset;

	get_something (w, XmNfontList, (XtArgVal)&fl);
	if (XmFontListInitFontContext (&fc, fl) != True) {
	    printf ("%s: No Font context!\n", me);
	    exit(1);
	}
	if (XmFontListGetNextFont (fc, &charset, f) != True) {
	    printf ("%s: no font!\n", me);
	    exit(1);
	}
	XmFontListFreeFontContext (fc);
}

/* get the XFontStruct we want to use when drawing text for the display views.
 * if can't open it, use the default font in a GC.
 */
void
get_views_font (dsp, fspp)
Display *dsp;
XFontStruct **fspp;
{
	static XFontStruct *fsp;

	if (!fsp) {
	    static char resname[] = "viewsFont";
	    char *fname = XGetDefault (dsp, myclass, resname);

	    /* use XLoadQueryFont because it returns gracefully if font is not
	     * found; XloadFont calls the default X error handler.
	     */
	    if (!fname || !(fsp = XLoadQueryFont (dsp, fname))) {
		char msg[256];
		GC tmpgc;

		if (!fname)
		    (void) sprintf(msg,"Can't find resource `%.180s'", resname);
		else
		    (void) sprintf (msg, "Can't load font `%.180s'", fname);
		(void) strcat (msg, "... using default GC font");
		xe_msg(msg, 0);

		tmpgc = XCreateGC (dsp, XtWindow(toplevel_w), 0L, NULL);
		if (!tmpgc) {
		    printf ("Can't create a temp GC!\n");
		    exit(1);
		}
		fsp = XQueryFont (dsp, XGContextFromGC (tmpgc));
		if (!fsp) {
		    printf ("Can't even get a default Font!\n");
		    exit(1);
		}
		XFreeGC (dsp, tmpgc);
	    }
	}
	
	*fspp = fsp;
}

/* load the greek font into *greekfspp then create a new gc at *greekgcp and
 *   set its font to the font id..
 * leave *greekgcp and greekfssp unchanged if there's any problems.
 */
void
loadGreek (dsp, win, greekgcp, greekfspp)
Display *dsp;
Drawable win;
GC *greekgcp;
XFontStruct **greekfspp;
{
	static char grres[] = "viewsGreekFont";
	XFontStruct *fsp;	/* local fast access */
	GC ggc;			/* local fast access */
	unsigned long gcm;
	XGCValues gcv;
	char buf[1024];
	char *greekfn;
	
	greekfn = XGetDefault (dsp, myclass, grres);
	if (!greekfn) {
	    (void) sprintf (buf, "No resource: %s", grres);
	    xe_msg (buf, 0);
	    return;
	}

	fsp = XLoadQueryFont (dsp, greekfn);
	if (!fsp) {
	    (void) sprintf (buf, "No font %.100s: %.800s", grres, greekfn);
	    xe_msg (buf, 0);
	    return;
	}

	gcm = GCFont;
	gcv.font = fsp->fid;
	ggc = XCreateGC (dsp, win, gcm, &gcv);
	if (!ggc) {
	    XFreeFont (dsp, fsp);
	    xe_msg ("Can not make Greek GC", 0);
	    return;
	}

	*greekgcp = ggc;
	*greekfspp = fsp;

	return;
}
