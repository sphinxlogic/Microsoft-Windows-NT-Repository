/* code to manage the actual drawing of plots.
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
#include <Xm/DrawingA.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/Text.h>


#include "P_.h"
#include "astro.h"
#include "ps.h"

extern int get_color_resource P_((Widget w, char *cname, Pixel *p));
extern int tickmarks P_((double min, double max, int numdiv, double ticks[]));
extern void fs_date P_((char out[], double jd));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void get_views_font P_((Display *dsp, XFontStruct **fspp));
extern void xe_msg P_((char *msg, int app_modal));

static void mk_gcs P_((Widget w));
static int x_ticks P_((int asdate, double minx, double maxx, int maxticks,
    double ticks[]));
static int draw_x_label P_((Display *dsp, Window win, int asdate, double v,
    int x, int y));

/* maximum number of unique functions (ie tags) in the plot file.
 * to be fair, it should be at least as large as the value in plot.
 */
#define	MAXPLTLINES	10

static GC plt_gc;		/* the GC to use for everything */
static Pixel plt_p[MAXPLTLINES];/* colors, one per category */
static XFontStruct *plt_fs;	/* handy font metrix for placing text */
static Pixel da_p;		/* the foreground color of the drawing area */

/* plot the given file in the drawing area in cartesian coords.
 * drawing space is from 0..(nx-1) and 0..(ny-1).
 * TODO: add z tags somehow
 * return 0 if ok, else give a message about trouble and return -1.
 */
int
plot_cartesian (pfp, widget, nx, ny, flipx, flipy, grid, xasdate)
FILE *pfp;
Widget widget;
unsigned int nx, ny;
int flipx, flipy;	/* respectively true to flip their axis */
int grid;		/* whether to include a grid with the tick marks */
int xasdate;		/* whether to show x axis as dates */
{
#define	TL	5	/* tick mark length, in pixels */
#define	NT	10	/* rough number of tick marks on each axis */
	Display *dsp = XtDisplay(widget);
	Window win = XtWindow(widget);
	double ticks[NT+2];
	int nt;
	static char fmt[] = "%c,%lf,%lf";
	double x, y;	/* N.B. be sure these match what scanf's %lf wants*/
	double minx=0, maxx=0, miny=0, maxy=0, xscale, yscale;
	char buf[128];
	int lx[MAXPLTLINES], ly[MAXPLTLINES], one[MAXPLTLINES];
	char c, tags[MAXPLTLINES];
	XCharStruct overall;
	int sawtitle;
	int ylblw;	/* label width */
	int nlines;
	int ix, iy;	/* misc X drawing coords */
	int x0;		/* X x coord of lower left of plotting box, in pixels */
	int y0;		/* X y coord of lower left of plotting box, in pixels */
	int h, w;	/* width of height of plotting box, in pixels */
	int asc, desc;	/* font ascent and descent, in pixels */
	int maxlblw;	/* width of longest y axis label, in pixels */
	int i;
#define	XCORD(x)	(x0 + (int)((flipx?maxx-(x):(x)-minx)*xscale + 0.5))
#define	YCORD(y)	(y0 - (int)((flipy?maxy-(y):(y)-miny)*yscale + 0.5))

	/* find ranges and number of and tags for each unique line */
	nlines = 0;
	while (fgets (buf, sizeof(buf), pfp)) {
	    if (sscanf (buf, fmt, &c, &x, &y) != 3)
		continue;
	    if (nlines == 0) {
		maxx = minx = x;
		maxy = miny = y;
	    }
	    for (i = 0; i < nlines; i++)
		if (c == tags[i])
		    break;
	    if (i == nlines) {
		if (nlines == MAXPLTLINES) {
		   (void) sprintf (buf,
		     "Plot file contains more than %d functions.", MAXPLTLINES);
		   xe_msg (buf, 0);
		   return(-1);
		}
		tags[nlines++] = c;
	    }
	    if (x > maxx) maxx = x;
	    else if (x < minx) minx = x;
	    if (y > maxy) maxy = y;
	    else if (y < miny) miny = y;
	}

	if (nlines == 0) {
	    xe_msg ("Plot file appears to be empty.", 0);
	    return(-1);
	}
#define	SMALL	(1e-6)
	if (fabs(minx-maxx) < SMALL || fabs(miny-maxy) < SMALL) {
	    xe_msg ("Plot file values contain insufficient spread.", 0);
	    return(-1);
	}

	/* build the gcs, fonts etc if this is the first time. */
	if (!plt_gc)
	    mk_gcs (widget);
	XSetForeground (dsp, plt_gc, da_p);
	
	/* pick a font to use in case we are printing */
	XPSRegisterFont (plt_fs->fid, "Helvetica");

	/* add y-axis tick marks.
	 * first compute length of longest y-axis label and other char stuff.
	 */
	nt = tickmarks (miny, maxy, NT, ticks);
	maxlblw = 0;
	for (i = 0; i < nt; i++) {
	    int dir, l;

	    if (ticks[i] < miny || ticks[i] > maxy)
		continue;
	    (void) sprintf (buf, "%g", ticks[i]);
	    l = strlen(buf);
	    XTextExtents (plt_fs, buf, l, &dir, &asc, &desc, &overall);
	    if (overall.width > maxlblw)
		maxlblw = overall.width;
	}

	/* now we can compute border sizes and the scaling factors */
	x0 = maxlblw+TL+10;
	w = nx - x0 - 30;
	y0 = ny - (asc+desc+2+2*TL);
	h = y0 - (asc+desc+2);
	xscale = w/(maxx-minx);
	yscale = h/(maxy-miny);

	/* now draw y axis, its labels, and optionally the horizontal grid */
	for (i = 0; i < nt; i++) {
	    int l;

	    if (ticks[i] < miny || ticks[i] > maxy)
		continue;
	    (void) sprintf (buf, "%g", ticks[i]);
	    l = strlen(buf);
	    iy = YCORD(ticks[i]);
	    XPSDrawLine (dsp, win, plt_gc, x0-TL, iy, x0, iy);
	    XPSDrawString (dsp, win, plt_gc, 1, iy+(asc-desc)/2, buf, l);
	    if (grid)
		XPSDrawLine (dsp, win, plt_gc, x0, iy, x0+w-1, iy);
	}

	/* now draw x axis and label it's first and last tick mark.
	 * if there's room, label the center tickmark too.
	 * also grid, if requested.
	 */
	nt = x_ticks (xasdate, minx, maxx, NT, ticks);
	ylblw = 0;
	for (i = 0; i < nt; i++) {
	    if (ticks[i] < minx || ticks[i] > maxx)
		continue;
	    ix = XCORD(ticks[i]);
	    XPSDrawLine (dsp, win, plt_gc, ix, y0+TL, ix, y0);
	    if (grid)
		XPSDrawLine (dsp, win, plt_gc, ix, y0, ix, y0-h-1);
	}
	for (i = 0; i < nt; i++)
	    if (ticks[i] >= minx) {
		ylblw += draw_x_label (dsp, win, xasdate, ticks[i],
							XCORD(ticks[i]), y0);
		break;
	    }
	for (i = nt; --i >= 0;)
	    if (ticks[i] <= maxx) {
		ylblw += draw_x_label (dsp, win, xasdate, ticks[i],
							XCORD(ticks[i]), y0);
		break;
	    }
	if (ylblw < w/2)
	    (void) draw_x_label (dsp, win, xasdate, ticks[nt/2],
	    						XCORD(ticks[nt/2]), y0);

	/* draw border of actual plotting area */
	XPSDrawLine (dsp, win, plt_gc, x0, y0-h, x0, y0);
	XPSDrawLine (dsp, win, plt_gc, x0, y0, x0+w, y0);
	XPSDrawLine (dsp, win, plt_gc, x0+w, y0, x0+w, y0-h);
	XPSDrawLine (dsp, win, plt_gc, x0+w, y0-h, x0, y0-h);

	/* read file again, this time plotting the data (finally!).
	 * also, the first line we see that doesn't look like a point
	 * is put up as a title line (minus its first two char and trailing \n).
	 */
	sawtitle = 0;
	rewind (pfp);
	for (i = 0; i < nlines; i++)
	    one[i] = 0;
	while (fgets (buf, sizeof(buf), pfp)) {
	    if (sscanf (buf, fmt, &c, &x, &y) != 3) {
		/* a title line ? */
		int l;

		if (!sawtitle && (l = strlen(buf)) > 2) {
		    int di, as, de;
		    XCharStruct ovl;
		    XTextExtents (plt_fs, buf+2, l-2, &di, &as, &de, &ovl);
		    XSetForeground (dsp, plt_gc, da_p);
		    XPSDrawString (dsp, win, plt_gc, x0+(w-ovl.width)/2, asc+1,
								buf+2, l-3);
		    sawtitle = 1;
		}
		continue;
	    }
	    for (i = 0; i < nlines; i++)
		if (c == tags[i])
		    break;
	    ix = XCORD(x);
	    iy = YCORD(y);
	    XSetForeground (dsp, plt_gc, plt_p[i]);
	    if (one[i]++ > 0)
		XPSDrawLine (dsp, win, plt_gc, ix, iy, lx[i], ly[i]);
	    else {
		int ytop = y0 - h + asc;
		XPSDrawString (dsp, win, plt_gc, ix+2, (iy<ytop ? ytop : iy)-2,
									&c, 1);
	    }
	    lx[i] = ix;
	    ly[i] = iy;
	}
	return (0);
}

static void
mk_gcs (w)
Widget w;
{
	Display *dsp = XtDisplay(w);
	Window win = XtWindow(w);
	Pixel da_bgp;	/* the background color of the drawing area */
	int i;

	/* create the annotation and default plot color gc,
	 * using the foreground color of the PlotDA.
	 */
	get_something (w, XmNforeground, (XtArgVal)&da_p);
	get_something (w, XmNbackground, (XtArgVal)&da_bgp);
	plt_gc = XCreateGC (dsp, win, 0L, NULL);
	XSetForeground (dsp, plt_gc, da_p);
	get_views_font (dsp, &plt_fs);
	XSetFont (dsp, plt_gc, plt_fs->fid);

	/* fill in plt_p array with pixels to use for function plotting.
	 * Use colors defined in the plotColor[0-9] resources, else reuse fg.
	 * they all use the same font.
	 */
	for (i = 0; i < MAXPLTLINES; i++) {
	    Pixel *p = &plt_p[i];
	    char cname[32];

	    (void) sprintf (cname, "plotColor%d", i);
	    if (get_color_resource (w, cname, p) < 0 || *p == da_bgp)
		*p = da_p;
	}
}

static int
x_ticks (asdate, minx, maxx, maxticks, ticks)
int asdate;
double minx, maxx;
int maxticks;
double ticks[];
{
	double jd, minjd, maxjd;
	double d;
	int m, y;
	int nmonths;
	int nt;
	int i;

	if (!asdate)
	    return (tickmarks (minx, maxx, maxticks, ticks));

	year_mjd (minx, &minjd);
	year_mjd (maxx, &maxjd);

	nmonths = (int)floor((maxjd-minjd)/28.0); /* approx is ok */

	if (nmonths >= maxticks-1) {	/* counting fenceposts :-) */
	    /* put ticks on month boundaries */
	    int delmonths;

	    delmonths = (nmonths+maxticks-1)/maxticks; /* round up */
	    nt = nmonths/delmonths + 1;		/* include the end one */
	    if (nt > maxticks) nt = maxticks;	/* can't happen :-) */
	    mjd_cal (minjd, &m, &d, &y);
	    d = 1.0;
	    if (++m > 12) {
		m = 1;
		y++;
	    }
	    cal_mjd (m, d, y, &jd);
	    mjd_year (jd, &ticks[0]);
	    for (i = 1; i < nt; i++) {
		m += delmonths;
		while (m > 12) {
		    m -= 12;
		    y++;
		}
		cal_mjd (m, d, y, &jd);
		mjd_year (jd, &ticks[i]);
	    }
	} else {
	    /* put ticks on day boundaries */
	    int ndays, deldays;

	    ndays = (int)floor(maxjd-minjd);
	    deldays = (ndays+maxticks-1)/maxticks; /* round up */
	    nt = ndays/deldays + 1;		/* include the end one */
	    if (nt > maxticks) nt = maxticks;	/* can't happen :-) */
	    jd = mjd_day (minjd+0.999999);	/* round up */
	    for (i = 0; i < nt; i++) {
		mjd_year (jd, &ticks[i]);
		jd += deldays;
	    }
	}

	return (nt);
}

/* draw v at [x,y].
 * return width of string in pixels.
 */
static int
draw_x_label (dsp, win, asdate, v, x, y)
Display *dsp;
Window win;
int asdate;
double v;
int x, y;
{
	int dir, asc, des;
	XCharStruct ovl;
	char buf[128];
	int l;

	if (asdate) {
	    double jd;

	    year_mjd (v, &jd);
	    fs_date (buf, jd);
	} else
	    (void) sprintf (buf, "%g", v);
	l = strlen(buf);
	XTextExtents (plt_fs, buf, l, &dir, &asc, &des, &ovl);
	XSetForeground (dsp, plt_gc, da_p);
	XPSDrawLine (dsp, win, plt_gc, x, y+TL, x, y+2*TL);
	XPSDrawString (dsp, win, plt_gc, x-ovl.width/2, y+2*TL+asc+1, buf, l);

	return (ovl.width);
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: plot_aux.c,v $ $Date: 1997/04/21 23:19:45 $ $Revision: 1.1 $ $Name:  $"};
