/* code to handle jupiter's Great Red Spot. */

#include <stdio.h>
#include <math.h>

#include <Xm/Xm.h>

#include "P_.h"
#include "astro.h"

extern Widget 	toplevel_w;
extern char 	*myclass;
#define	XtD	XtDisplay(toplevel_w)

#define	GRSLNG	42	/* default GRS longitude, degs */

extern double atod P_((char *buf));
extern int get_color_resource P_((Widget w, char *cname, Pixel *p));
extern void xe_msg P_((char *msg, int app_modal));

static void make_gc P_((void));
static void set_grslng P_((void));

static GC jgrs_gc;	/* gc to use for spot -- set from JupiterGRSColor */

static double jgrs_lng;	/* longitude of GRS in Sys II coords, rads */
static int jgrs_lngset;	/* 0 until we set jgrs_lng from JupiterGRSLongitude */

/* draw the great red spot.
 * factoids: default orientation is E on right, S on top. So, GRS moves from
 *   right to left. Spot is in S hemisphere. CML increases with time.
 */
void
jgrs_draw (dr, rad, jx, jy, fliptb, fliplr, cmlII)
Drawable dr;	/* drawable to use */
int rad;	/* radius of jupiter, pixels */
int jx, jy;	/* center of jupiter circle, dr coords */
int fliptb;	/* whether image is flipped tb; default is S on top */
int fliplr;	/* whether image is flipped lr; default is E on right */
double cmlII;	/* current central merdian longitude, system II, rads */
{
	int dx, dy;	/* GRS center, relative to jx,jy, without flipping */
	int x, y;	/* GRS center, relative to dr, after flipping */
	int r;		/* radius of GRS, pixels */
	int h, w;	/* h is always r, but we flatten w at limbs */
	double a;	/* angle from center to GRS going towards right (+E) */
	double ca, sa;

	if (!jgrs_gc)
	    make_gc();

	if (!jgrs_lngset)
	    set_grslng();

	a = jgrs_lng - cmlII;
	ca = cos(a);
	if (ca < 0)
	    return;	/* behind */

	sa = sin(a);
	r = rad/5;	/* too large for real but looks better */
	h = 2*r;
	w = 1.5*h*ca;	/* oval but forshortened near limbs */

	dx = rad*sa;	/* E right */
	dy = -r;	/* S up */

	x = fliplr ? jx - dx : jx + dx;
	y = fliptb ? jy - dy : jy + dy;

	if (w > 0 && h > 0)
	    XFillArc (XtD, dr, jgrs_gc, x-w/2, y-h/2, w, h, 0, 360*64);
}

static void
make_gc()
{
	Pixel p;

	if (get_color_resource (toplevel_w, "JupiterGRSColor", &p) < 0) {
	    xe_msg ("Actually .. using Black", 0);
	    p = BlackPixel(XtD, 0);
	}

	jgrs_gc = XCreateGC (XtD, XtWindow(toplevel_w), 0L, (XGCValues *)0);
	XSetForeground (XtD, jgrs_gc, p);
}

/* set jgrs_lng from JupiterGRSLongitude and set jgrs_lngset */
static void
set_grslng()
{
	static char res[] = "JupiterGRSLongitude";
	char *val;

	val = XGetDefault (XtD, myclass, res);
	if (!val) {
	    char buf[128];

	    jgrs_lng = degrad(GRSLNG);
	    (void) sprintf (buf, "Can not get %s -- defaulting to %g degs",
							res, raddeg(jgrs_lng));
	    xe_msg (buf, 0);
	} else {
	    jgrs_lng = degrad (atod (val));
	}

	jgrs_lngset = 1;
}
