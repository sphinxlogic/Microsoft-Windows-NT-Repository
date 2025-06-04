/* misc handy X Windows functions.
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#if defined(__STDC__)
#include <stdlib.h>
#else
extern void *malloc();
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
#define	XtD	XtDisplay(toplevel_w)

extern void xe_msg P_((char *msg, int app_modal));

#define	MAXGRAY	30		/* max colors in a grayscale ramp */

void get_views_font P_((Display *dsp, XFontStruct **fspp));
char *getXRes P_((char *name, char *def));
int alloc_ramp P_((Display *dsp, XColor *basep, Colormap cm, Pixel pix[],
    int maxn));

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
	cval = getXRes (cname, NULL);

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
	static char *objcolnames[NOBJ-3] = {
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
	    unsigned long gcm;
	    XFontStruct *fsp;
	    XGCValues gcv;
	    Pixel p;
	    int i;

	    gcm = GCFont | GCForeground;
	    get_views_font (dsp, &fsp);
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

	if (op->o_type == PLANET && op->pl.pl_code < XtNumber(objgcs))
	    *gcp = objgcs[op->pl.pl_code];
	else if (is_ssobj(op))
	    *gcp = solsys_gc;
	else if (is_type (op, FIXEDM))
	    switch (op->f_class) {
	    case 'Q': case 'J': case 'L': case 'T': case '\0':
		*gcp = stargcs[3];
		break;
	    case 'S': case 'B': case 'D': case 'M': case 'V':
		switch (op->f_spect[0]) {
		case 'O': case 'B': case 'A': case 'W':
		    *gcp = stargcs[0];
		    break;
		case 'F': case 'G': case 'K':	/* FALLTHRU */
		default:
		    *gcp = stargcs[1];
		    break;
		case 'M': case 'N': case 'R': case 'S': case 'C':
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


/* get the font named by the given X resource, else fixed, else bust */
XFontStruct *
getXResFont (rn)
char *rn;
{
	static char fixed[] = "fixed";
	char *fn = getXRes (rn, NULL);
	Display *dsp = XtD;
	XFontStruct *fsp;
	char msg[1024];

	if (!fn) {
	    (void) sprintf (msg, "No resource `%s' .. using fixed", rn);
	    xe_msg (msg, 0);
	    fn = fixed;
	}
	
	/* use XLoadQueryFont because it returns gracefully if font is not
	 * found; XLoadFont calls the default X error handler.
	 */
	fsp = XLoadQueryFont (dsp, fn);
	if (!fsp) {
	    (void) sprintf (msg, "No font `%s' for `%s' .. using fixed", fn,rn);
	    xe_msg(msg, 0);

	    fsp = XLoadQueryFont (dsp, fixed);
	    if (!fsp) {
		printf ("Can't even get %s!\n", fixed);
		exit(1);
	    }
	}

	return (fsp);
}


/* get the XFontStruct we want to use when drawing text for the display views.
 */
void
get_views_font (dsp, fspp)
Display *dsp;
XFontStruct **fspp;
{
	static XFontStruct *fsp;

	if (!fsp)
	    fsp = getXResFont ("viewsFont");

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
	
	greekfn = getXRes (grres, NULL);
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

/* return a gray-scale ramp of pixels at *pixp, and the number in the ramp
 * N.B. don't change the pixels -- they are shared with other users.
 */
int
gray_ramp (dsp, cm, pixp)
Display *dsp;
Colormap cm;
Pixel **pixp;
{
	static Pixel gramp[MAXGRAY];
	static int ngray;

	if (ngray == 0) {
	    /* get gray ramp pixels once */
	    XColor white;

	    white.red = white.green = white.blue = ~0;
	    ngray = alloc_ramp (dsp, &white, cm, gramp, MAXGRAY);
	    if (ngray < MAXGRAY) {
		char msg[1024];
		(void) sprintf (msg, "Wanted %d but only found %d grays.",
							     MAXGRAY, ngray);
		xe_msg (msg, 0);
	    }
	}

	*pixp = gramp;
	return (ngray);
}

/* try to fill pix[maxn] with linear ramp from black to whatever is in base.
 * each entry will be unique; return said number, which can be <= maxn.
 * N.B. if we end up with just 2 colors, we set pix[0]=0 and pix[1]=1 in
 *   anticipation of caller using a XYBitmap and thus XPutImage for which
 *   color 0/1 uses the background/foreground of a GC.
 */
int
alloc_ramp (dsp, basep, cm, pix, maxn)
Display *dsp;
XColor *basep;
Colormap cm;
Pixel pix[];
int maxn;
{
	int nalloc, nunique;
	XColor xc;
	int i, j;

	/* first just try to get them all */
	for (nalloc = 0; nalloc < maxn; nalloc++) {
	    xc.red = nalloc*(int)basep->red/(maxn-1);
	    xc.green = nalloc*(int)basep->green/(maxn-1);
	    xc.blue = nalloc*(int)basep->blue/(maxn-1);
	    if (XAllocColor (dsp, cm, &xc))
		pix[nalloc] = xc.pixel;
	    else
		break;
	}

	/* see how many are actually unique */
	nunique = 0;
	for (i = 0; i < nalloc; i++) {
	    for (j = i+1; j < nalloc; j++)
		if (pix[i] == pix[j])
		    break;
	    if (j == nalloc)
		nunique++;
	}

	if (nunique < maxn) {
	    /* rebuild the ramp using just nunique entries.
	     * N.B. we assume we can get nunique colors again right away.
	     */
	    XFreeColors (dsp, cm, pix, nalloc, 0);

	    if (nunique <= 2) {
		/* we expect caller to use a XYBitmap via GC */
		pix[0] = 0;
		pix[1] = 1;
		nunique = 2;
	    } else {
		for (i = 0; i < nunique; i++) {
		    xc.red = i*(int)basep->red/(nunique-1);
		    xc.green = i*(int)basep->green/(nunique-1);
		    xc.blue = i*(int)basep->blue/(nunique-1);

		    if (!XAllocColor (dsp, cm, &xc)) {
			nunique = i;
			break;
		    }
		    pix[i] = xc.pixel;
		}
	    }
	}

	return (nunique);
}

/* search for  the named X resource from all the usual places.
 * this looks in more places than XGetDefault().
 * we just return it as a string -- caller can do whatever.
 * return def if can't find it anywhere.
 * N.B. memory returned is _not_ malloced so leave it be.
 */
char *
getXRes (name, def)
char *name;
char *def;
{
	static char notfound[] = "_Not_Found_";
	char *res = NULL;
	XtResource xr;

	xr.resource_name = name;
	xr.resource_class = "AnyClass";
	xr.resource_type = XmRString;
	xr.resource_size = sizeof(String);
	xr.resource_offset = 0;
	xr.default_type = XmRImmediate;
	xr.default_addr = (XtPointer)notfound;

	XtGetApplicationResources (toplevel_w, (void *)&res, &xr, 1, NULL, 0);
	if (!res || strcmp (res, notfound) == 0)
	    res = def;

	return (res);
}

/* build and return a private colormap for toplevel_w.
 * nnew is how many colors we expect to add.
 */
Colormap
createCM(nnew)
int nnew;
{
#define	NPRECM	50  /* try to preload new cm with NPRECM colors from def cm */
	Display *dsp = XtDisplay (toplevel_w);
	Window win = RootWindow (dsp, DefaultScreen(dsp));
	Colormap defcm = DefaultColormap (dsp, DefaultScreen(dsp));
	int defcells = DisplayCells (dsp, DefaultScreen(dsp));
	Visual *v = DefaultVisual (dsp, DefaultScreen(dsp));
	Colormap newcm;

	/* make a new colormap */
	newcm = XCreateColormap (dsp, win, v, AllocNone);

	/* preload with some existing colors to hedge flashing, if room */
	if (nnew + NPRECM < defcells) {
	    XColor preload[NPRECM];
	    int i;

	    for (i = 0; i < NPRECM; i++)
		preload[i].pixel = (unsigned long) i;
	    XQueryColors (dsp, defcm, preload, NPRECM);
	    for (i = 0; i < NPRECM; i++)
		(void) XAllocColor (dsp, newcm, &preload[i]);
	}

	return (newcm);
}

/* depending on the "install" resource and whether cm can hold nwant more
 * colors, return a new colormap or cm again.
 */
Colormap
checkCM(cm, nwant)
Colormap cm;
int nwant;
{
	Display *dsp = XtD;
	char *inst;

	/* get the install resource value */
	inst = getXRes ("install", "guess");

	/* check each possible value */
	if (strcmp (inst, "no") == 0)
	    return (cm);
	else if (strcmp (inst, "yes") == 0)
	    return (createCM (nwant));
	else if (strcmp (inst, "guess") == 0) {
	    /* get a smattering of colors and opt for private cm if can't.
	     * we use alloc_ramp() because it verifies unique pixels.
	     * we use three to not overstress the resolution of colors.
	     */
	    Pixel *rr, *gr, *br;
	    int neach, nr, ng, nb;
	    XColor xc;

	    /* grab some to test */
	    neach = nwant/3;
	    xc.red = 255 << 8;
	    xc.green = 0;
	    xc.blue = 0;
	    rr = (Pixel *) malloc (neach * sizeof(Pixel));
	    nr = alloc_ramp (dsp, &xc, cm, rr, neach);
	    xc.red = 0;
	    xc.green = 255 << 8;
	    xc.blue = 0;
	    gr = (Pixel *) malloc (neach * sizeof(Pixel));
	    ng = alloc_ramp (dsp, &xc, cm, gr, neach);
	    xc.red = 0;
	    xc.green = 0;
	    xc.blue = 255 << 8;
	    br = (Pixel *) malloc (neach * sizeof(Pixel));
	    nb = alloc_ramp (dsp, &xc, cm, br, neach);

	    /* but don't keep them.
	     * N.B. alloc_ramp just cheats us with B&W if it returns 2.
	     */
	    if (nr > 2)
		XFreeColors (dsp, cm, rr, nr, 0);
	    if (ng > 2)
		XFreeColors (dsp, cm, gr, ng, 0);
	    if (nb > 2)
		XFreeColors (dsp, cm, br, nb, 0);
	    free ((void *)rr);
	    free ((void *)gr);
	    free ((void *)br);

	    if (nr + ng + nb < 3*neach)
		return (createCM(nwant));
	} else
	    printf ("Unknown install `%s' -- defaulting to No\n", inst);

	return (cm);
}

/* explicitly handle pending X events when otherwise too busy */
void
XCheck (app)
XtAppContext app;
{
        while ((XtAppPending (app) & XtIMXEvent) == XtIMXEvent)
	    XtAppProcessEvent (app, XtIMXEvent);
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: xmisc.c,v $ $Date: 1999/02/25 06:56:12 $ $Revision: 1.2 $ $Name:  $"};
