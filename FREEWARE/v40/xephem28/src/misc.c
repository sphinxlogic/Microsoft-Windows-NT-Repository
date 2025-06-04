/* misc handy functions.
 * every system has such, no?
 */

#include <stdio.h>
/* stdio.h only declares fdopen() for POSIX, not STDC */
#if !defined(_POSIX_SOURCE)
extern FILE *fdopen();
#endif
#include <ctype.h>
#include <math.h>

#if defined(__STDC__)
#include <stdlib.h>
#else
extern char *getenv();
extern void *malloc();
extern double atof();
extern double fabs(), fmod();
#endif

#ifdef _POSIX_SOURCE
#include <unistd.h>
#else
extern int close();
#endif

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "preferences.h"

/* look around for O_WRONLY. Older VMS had it in xlib.h; some VMS has it in
 * fcntl.h (but not all have this file); some now have it in file.h. geeze.
 */
#if defined(VMS) && !defined(O_WRONLY)
#include <file.h>
#endif
#if !defined(O_WRONLY)
#include <fcntl.h>
#endif

extern Widget	toplevel_w;
extern char	*myclass;
#define	XtD	XtDisplay(toplevel_w)

extern Now *mm_get_now P_((void));
extern int any_ison P_((void));
extern int listing_ison P_((void));
extern int plot_ison P_((void));
extern int srch_ison P_((void));
extern void c_cursor P_((Cursor c));
extern void c_update P_((Now *np, int force));
extern void db_cursor P_((Cursor c));
extern void db_newdb P_((int appended));
extern void db_newobj P_((int appended));
extern void db_update P_((Obj *op));
extern void dm_cursor P_((Cursor c));
extern void dm_newobj P_((int dbidx));
extern void dm_selection_mode P_((int whether));
extern void dm_update P_((Now *np, int how_much));
extern void e_cursor P_((Cursor c));
extern void e_newobj P_((int dbidx));
extern void e_selection_mode P_((int whether));
extern void e_update P_((Now *np, int force));
extern void fs_date P_((char out[], double jd));
extern void fs_time P_((char out[], double t));
extern void fs_tz P_((char *timezonename, int tzpref, Now *np));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void get_xmstring P_((Widget w, char *resource, char **txtp));
extern void jm_cursor P_((Cursor c));
extern void jm_newdb P_((int appended));
extern void jm_selection_mode P_((int whether));
extern void jm_update P_((Now *np, int how_much));
extern void lst_cursor P_((Cursor c));
extern void lst_log P_((char *name, char *str));
extern void lst_selection P_((char *name));
extern void m_cursor P_((Cursor c));
extern void m_selection_mode P_((int whether));
extern void m_update P_((Now *np, int how_much));
extern void main_cursor P_((Cursor c));
extern void mars_cursor P_((Cursor c));
extern void mars_selection_mode P_((int whether));
extern void mars_update P_((Now *np, int force));
extern void mm_selection_mode P_((int whether));
extern void msg_cursor P_((Cursor c));
extern void obj_cursor P_((Cursor c));
extern void obj_newdb P_((int appended));
extern void obj_pickgc P_((Obj *op, Widget w, GC *gcp));
extern void obj_update P_((Now *np, int howmuch));
extern void plt_cursor P_((Cursor c));
extern void plt_log P_((char *name, double value));
extern void plt_selection P_((char *name));
extern void pm_cursor P_((Cursor c));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void sm_cursor P_((Cursor c));
extern void sm_newdb P_((int appended));
extern void sm_selection_mode P_((int whether));
extern void sm_update P_((Now *np, int how_much));
extern void srch_cursor P_((Cursor c));
extern void srch_log P_((char *name, double value));
extern void srch_selection P_((char *name));
extern void srch_selection_mode P_((int whether));
extern void ss_cursor P_((Cursor c));
extern void ss_newobj P_((int dbidx));
extern void ss_update P_((Now *np, int how_much));
extern void sv_cursor P_((Cursor c));
extern void sv_newdb P_((int appended));
extern void sv_newobj P_((int dbidx));
extern void sv_update P_((Now *np, int how_much));
extern void svf_cursor P_((Cursor c));
extern void v_cursor P_((Cursor c));
extern void xe_msg P_((char *msg, int app_modal));

static char *home;


/* zero from loc for len bytes */
void
zero_mem (loc, len)
void *loc;
unsigned len;
{
	(void) memset (loc, 0, len);
}

/* called to set or unset the watch cursor on all menus.
 * allow for nested requests.
 */
void
watch_cursor(want)
int want;
{
	static Cursor wc;
	static nreqs;
	Cursor c;

	if (!wc)
	    wc = XCreateFontCursor (XtD, XC_watch);

	if (want) {
	    if (nreqs++ > 0)
		return;
	    c = wc;
	} else {
	    if (--nreqs > 0)
		return;
	    c = (Cursor)0;
	}

	c_cursor(c);
	db_cursor(c);
	dm_cursor(c);
	e_cursor(c);
	jm_cursor(c);
	lst_cursor(c);
	m_cursor(c);
	main_cursor(c);
	mars_cursor(c);
	msg_cursor(c);
	obj_cursor(c);
	plt_cursor(c);
	pm_cursor(c);
	sm_cursor(c);
	srch_cursor(c);
	ss_cursor(c);
	sv_cursor(c);
	svf_cursor(c);
	v_cursor(c);
 
	XFlush (XtD);
}

/* update stuff on all major views */
void
all_update(np, how_much)
Now *np;
int how_much;
{
	dm_update (np, how_much);
	mars_update (np, how_much);
	e_update (np, how_much);
	jm_update (np, how_much);
	sm_update (np, how_much);
	ss_update (np, how_much);
	sv_update (np, how_much);
	m_update  (np, how_much);
	obj_update (np, how_much);
	c_update (np, how_much);
}

/* tell everyone who might care that a user-defined object has changed
 * then recompute and redisplay new values.
 */
void
all_newobj(dbidx)
int dbidx;
{
	watch_cursor (1);

	db_newobj(dbidx);
	dm_newobj(dbidx);
	e_newobj (dbidx);
	ss_newobj(dbidx);
	sv_newobj(dbidx);
	all_update (mm_get_now(), 1);

	watch_cursor (0);
}

/* tell everyone who might care that the db (beyond NOBJ) has changed.
 * appended is true if it grew; else it was deleted.
 */
void
all_newdb(appended)
int appended;
{
	watch_cursor (1);

	obj_newdb(appended);
	sm_newdb(appended);
	jm_newdb(appended);
	sv_newdb(appended);
	db_newdb (appended);

	watch_cursor (0);
}

/* inform all menus that have something selectable for plotting/listing/srching
 * wether we are now in a mode that they should report when those fields are
 * selected.
 */
void
all_selection_mode(whether)
int whether;
{
	dm_selection_mode(whether);
	mm_selection_mode(whether);
	jm_selection_mode(whether);
	mars_selection_mode(whether);
	e_selection_mode(whether);
	sm_selection_mode(whether);
	srch_selection_mode(whether);
	m_selection_mode(whether);
}

/* inform all potentially interested parties of the name of a field that
 * it might want to use for latter.
 * this is just to collect in one place all the modules that gather care.
 */
void
register_selection (name)
char *name;
{
	plt_selection (name);
	lst_selection (name);
	srch_selection (name);
}

/* if we are plotting/listing/searching, send the current field info to them.
 * N.B. only send `value' to plot and search if logv is not 0.
 */
void
field_log (w, value, logv, str)
Widget w;
double value;
int logv;
char *str;
{
	char *name;

	if (!any_ison())
	    return;

	get_something (w, XmNuserData, (XtArgVal)&name);
	if (name) {
	    if (logv) {
		plt_log (name, value);
		srch_log (name, value);
	    }
	    lst_log (name, str);
	}
}

/* given min and max and an approximate number of divisions desired,
 * fill in ticks[] with nicely spaced values and return how many.
 * N.B. return value, and hence number of entries to ticks[], might be as
 *   much as 2 more than numdiv.
 */
int
tickmarks (min, max, numdiv, ticks)
double min, max;
int numdiv;
double ticks[];
{
        static int factor[] = { 1, 2, 5 };
        double minscale;
        double delta;
	double lo;
        double v;
        int n;

        minscale = fabs(max - min);
        delta = minscale/numdiv;
        for (n=0; n < sizeof(factor)/sizeof(factor[0]); n++) {
	    double scale;
	    double x = delta/factor[n];
            if ((scale = (pow(10.0, ceil(log10(x)))*factor[n])) < minscale)
		minscale = scale;
	}
        delta = minscale;

        lo = floor(min/delta);
        for (n = 0; (v = delta*(lo+n)) < max+delta; )
	    ticks[n++] = v;

	return (n);
}

/* given an Obj *, return its type as a descriptive string.
 * if it's of type fixed then return its class description.
 * N.B. we return the address of static storage -- do not free or change.
 */
char *
obj_description (op)
Obj *op;
{
	static struct {
	    char class;
	    char *desc;
	} fixed_class_map[] = {
	    {'A', "Cluster of Galaxies"},
	    {'B', "Binary Star"},
	    {'C', "Globular Cluster"},
	    {'D', "Double Star"},
	    {'F', "Diffuse Nebula"},
	    {'G', "Spiral Galaxy"},
	    {'H', "Spherical Galaxy"},
	    {'J', "Radio"},
	    {'K', "Dark Nebula"},
	    {'L', "Pulsar"},
	    {'M', "Multiple Star"},
	    {'N', "Bright Nebula"},
	    {'O', "Open Cluster"},
	    {'P', "Planetary Nebula"},
	    {'Q', "Quasar"},
	    {'R', "Supernova Remnant"},
	    {'S', "Star"},
	    {'T', "Stellar Object"},
	    {'U', "Cluster, with nebulosity"},
	    {'V', "Variable Star"},
	};

	switch (op->type) {
	case FIXED:
	    if (op->f_class) {
		int i;
		for (i = 0; i < XtNumber(fixed_class_map); i++)
		    if (fixed_class_map[i].class == op->f_class)
			return (fixed_class_map[i].desc);
	    }
	    return ("Fixed");
	case PARABOLIC:
	    return ("Solar - Parabolic");
	case HYPERBOLIC:
	    return ("Solar - Hyperbolic");
	case ELLIPTICAL:
	    return ("Solar - Elliptical");
	case PLANET:
	    return ("Planet");
	case EARTHSAT:
	    return ("Earth Sat");
	default:
	    printf ("obj_description: unknown type: 0x%x\n", op->type);
	    exit (1);
	    return (NULL);	/* for lint */
	}
}

/* set the XmNlabelString resource of the given widget to the date and time 
 * as given in the Now struct at *np.
 * avoid redrawing the string if it has not changed but don't use f_showit()
 *   because we want the time to always be updated even during movie loops.
 */
void
timestamp (np, w)
Now *np;
Widget w;
{
	char d[32], t[32], stamp[64];
	double lmjd;
	char *txtp;
	char timezonename[32];
	int tzpref = pref_get (PREF_ZONE);

	lmjd = mjd;
	if (tzpref == PREF_LOCALTZ)
	    lmjd -= tz/24.0;

	fs_date (d, mjd_day(lmjd));
	fs_time (t, mjd_hr(lmjd));

	fs_tz (timezonename, tzpref, np);
	(void) sprintf (stamp, "%s %s %s", d, t, timezonename);

	get_xmstring (w, XmNlabelString, &txtp);
	if (strcmp (txtp, stamp)) {
	    set_xmstring (w, XmNlabelString, stamp);
	    /* just XSync here doesn't get the time updated regularly enough
	     * though this Update causes Sky View to get extra exposes before
	     * it makes it's first pixmap.
	     */
	    XmUpdateDisplay (w);
	}
	XtFree (txtp);
}

/* given a Now *, find the local sidereal time */
void
now_lst (np, lst)
Now *np;
double *lst;
{
	static double last_mjd = -23243, last_lng = 121212, last_lst;

	if (last_mjd == mjd && last_lng == lng) {
	    *lst = last_lst;
	    return;
	}

	utc_gst (mjd_day(mjd), mjd_hr(mjd), lst);
	*lst += radhr(lng);
	range (lst, 24.0);

	last_mjd = mjd;
	last_lng = lng;
	last_lst = *lst;
}

/* return !0 if any of the button/data capture tools are active, else 0.
 */
int
any_ison()
{
	return (srch_ison() || plot_ison() || listing_ison());
}

/* given a circle and a line segment, find a segment of the line inside the 
 *   circle.
 * return 0 and the segment end points if one exists, else -1.
 * We use a parametric representation of the line:
 *   x = x1 + (x2-x1)*t and y = y1 + (y2-y1)*t, 0 < t < 1
 * and a centered representation of the circle:
 *   (x - xc)**2 + (y - yc)**2 = r**2
 * and solve for the t's that work, checking for usual conditions.
 */
int
lc (cx, cy, cw, x1, y1, x2, y2, sx1, sy1, sx2, sy2)
int cx, cy, cw;			/* circle bounding box corner and width */
int x1, y1, x2, y2;		/* line segment endpoints */
int *sx1, *sy1, *sx2, *sy2;	/* segment inside the circle */
{
	int dx = x2 - x1;
	int dy = y2 - y1;
	int r = cw/2;
	int xc = cx + r;
	int yc = cy + r;
	int A = x1 - xc;
	int B = y1 - yc;
	double a = dx*dx + dy*dy;	/* O(2 * 2**16 * 2**16) */
	double b = 2*(dx*A + dy*B);	/* O(4 * 2**16 * 2**16) */
	double c = A*A + B*B - r*r;	/* O(2 * 2**16 * 2**16) */
	double d = b*b - 4*a*c;		/* O(2**32 * 2**32) */
	double sqrtd;
	double t1, t2;

	if (d <= 0)
	    return (-1);	/* containing line is purely outside circle */

	sqrtd = sqrt(d);
	t1 = (-b - sqrtd)/(2.0*a);
	t2 = (-b + sqrtd)/(2.0*a);

	if (t1 >= 1.0 || t2 <= 0.0)
	    return (-1);	/* segment is purely outside circle */

	/* we know now that some part of the segment is inside,
	 * ie, t1 < 1 && t2 > 0
	 */

	if (t1 <= 0.0) {
	    /* (x1,y1) is inside circle */
	    *sx1 = x1;
	    *sy1 = y1;
	} else {
	    *sx1 = x1 + dx*t1;
	    *sy1 = y1 + dy*t1;
	}

	if (t2 >= 1.0) {
	    /* (x2,y2) is inside circle */
	    *sx2 = x2;
	    *sy2 = y2;
	} else {
	    *sx2 = x1 + dx*t2;
	    *sy2 = y1 + dy*t2;
	}

	return (0);
}

/* compute visual magnitude using the H/G parameters used in the Astro Almanac.
 * these are commonly used for asteroids.
 */
void
hg_mag (h, g, rp, rho, rsn, mp)
double h, g;
double rp;	/* sun-obj dist, AU */
double rho;	/* earth-obj dist, AU */
double rsn;	/* sun-earth dist, AU */
double *mp;
{
	double psi_t, Psi_1, Psi_2, beta;
	double tb2;

	beta = acos((rp*rp + rho*rho - rsn*rsn)/ (2*rp*rho));
	tb2 = tan(beta/2.0);
	/* psi_t = exp(log(tan(beta/2.0))*0.63); */
	psi_t = pow (tb2, 0.63);
	Psi_1 = exp(-3.33*psi_t);
	/* psi_t = exp(log(tan(beta/2.0))*1.22); */
	psi_t = pow (tb2, 1.22);
	Psi_2 = exp(-1.87*psi_t);
	*mp = h + 5.0*log10(rp*rho) - 2.5*log10((1-g)*Psi_1 + g*Psi_2);
}

/* given faintest desired mag, image scale and object magnitude and size,
 * return diameter to draw object, in pixels, or 0 if dimmer than fmag.
 */
int
magdiam (fmag, scale, mag, size)
int fmag;	/* faintest mag */
double scale;	/* rads per pixel */
double mag;	/* magnitude */
double size;	/* rads, or 0 */
{
	int diam, sized;
	
	if (mag > fmag)
	    return (0);
	diam = (int)(fmag - mag + 1);
	sized = (int)(size/scale + 0.5);
	if (sized > diam)
	    diam = sized;

	return (diam);
}

/* computer visual magnitude using the g/k parameters commonly used for comets.
 */
void
gk_mag (g, k, rp, rho, mp)
double g, k;
double rp;	/* sun-obj dist, AU */
double rho;	/* earth-obj dist, AU */
double *mp;
{
	*mp = g + 5.0*log10(rho) + 2.5*k*log10(rp);
}

/* given a string convert to floating point and return it as a double.
 * this is to isolate possible unportabilities associated with declaring atof().
 * it's worth it because atof() is often some 50% faster than sscanf ("%lf");
 */
double
atod (buf)
char *buf;
{
	return (atof (buf));
}

/* just like stdio fopen() but substitutes HOME if name starts with '~'.
 */
FILE *
fopenh (name, how)
char *name;
char *how;
{
	FILE *fp;

	/* get home, if we don't already know it */
	if (!home)
	    home = getenv ("HOME");

	/* skip leading blanks */
	while (*name == ' ')
	    name++;

	/* prepend home if name starts with ~
	 * open net filename.
	 */
	if (name[0] == '~' && home) {
	    char *fullname= (char *) malloc (strlen(home) + strlen(name+1) + 1);
	    (void) sprintf (fullname, "%s%s", home, name+1);
	    fp = fopen (fullname, how);
	    free (fullname);
	} else
	    fp = fopen (name, how);

	return (fp);
}

/* rather like open(2) but substitutes HOME if name starts with '~'.
 */
int
openh (name, flags)
char *name;
int flags;
{
	int fd;

	if (!home)
	    home = getenv ("HOME");

	if (name[0] == '~' && home) {
	    char *fullname= (char *) malloc (strlen(home) + strlen(name+1) + 1);
	    (void) sprintf (fullname, "%s%s", home, name+1);
	    fd = open (fullname, flags);
	    free (fullname);
	} else
	    fd = open (name, flags);

	return (fd);
}

/* return 0 if the given file exists, else -1.
 * substitute HOME if name starts with '~'.
 */
int
existsh (name)
char *name;
{
	int whether;
	int fd;

	if (!home)
	    home = getenv ("HOME");

	if (name[0] == '~' && home) {
	    char *fullname= (char *) malloc (strlen(home) + strlen(name+1) + 1);
	    (void) sprintf (fullname, "%s%s", home, name+1);
	    fd = open (fullname, O_RDONLY);
	    free (fullname);
	} else
	    fd = open (name, O_RDONLY);

	if (fd >= 0) {
	    whether = 0;
	    (void) close (fd);
	} else
	    whether = -1;

	return (whether);
}

/* return a string for whatever is in errno right now.
 * I never would have imagined it would be so hard to turn errno into a string!
 */
char *
syserrstr ()
{
#if defined(__STDC__) && !defined(__GNUC__)
/* gcc doesn't have strerror */
#include <errno.h>
#include <string.h>
	return (strerror (errno));
#else
#if defined(VMS)
#include <errno.h>
#include <perror.h>
#else
	extern char *sys_errlist[];
	extern int errno;
#endif
	return (sys_errlist[errno]);
#endif
}

/* solve a spherical triangle:
 *           A
 *          /  \
 *         /    \
 *      c /      \ b
 *       /        \
 *      /          \
 *    B ____________ C
 *           a
 *
 * given A, b, c find B and a in range 0..B..2PI and 0..a..PI, respectively..
 * Bp may be NULL if not interested in B.
 * N.B. we pass in cos(c) and sin(c) because in many problems one of the sides
 *   remains constant for many values of A and b.
 */
void
solve_sphere (A, b, cc, sc, cap, Bp)
double A, b;
double cc, sc;
double *cap, *Bp;
{
	double cA = cos(A), sA = sin(A);
	double cb = cos(b), sb = sin(b);
	double ca;
	double B;

	ca = cb*cc + sb*sc*cA;
	if (ca >  1.0) ca =  1.0;
	if (ca < -1.0) ca = -1.0;
	*cap = ca;

	if (!Bp)
	    return;

	if (cc > .99999) {
	    /* as c approaches 0, B approaches pi - A */
	    B = PI - A;
	} else if (cc < -.99999) {
	    /* as c approaches PI, B approaches A */
	    B = A;
	} else {
	    /* compute cB and sB and remove common factor of sa from quotient.
	     * be careful where B causes atan to blow.
	     */
	    double x, y;

	    y = sA*sb*sc;
	    x = cb - ca*cc;
	
	    if (fabs(x) < 1e-5)
		B = y < 0 ? 3*PI/2 : PI/2;
	    else
		B = atan2 (y, x);
	}

	*Bp = B;
	range (Bp, 2*PI);
}

/* open the fifo named by the given x resource for write-only.
 * complain and return -1 if trouble, else return 0 and fill in *fnp and *fpp;
 */
int
open_wfifores (xres, fnp, fpp)
char *xres;
char **fnp;
FILE **fpp;
{
#ifdef O_NONBLOCK               /* _POSIX_SOURCE */
#define NODELAY O_NONBLOCK
#else
#define NODELAY O_NDELAY
#endif
	char msg[256];
	int fd;
	char *fn;
	FILE *fp;

	/* get the name of the fifo from the fifores resource */
	fn = XGetDefault (XtD, myclass, xres);
	if (!fn) {
	    (void) sprintf (msg, "No `%.200s' resource.\n", xres);
	    xe_msg (msg, 0);
	    return (-1);
	}

	/* open for write-only, non-blocking in case their's no reader.
	 * this prevents clogged writes too.
	 */
	fd = openh (fn, O_WRONLY|NODELAY);
	if (fd < 0) {
	    (void) sprintf (msg,"Can not open %.100s: %.100s\n",fn,syserrstr());
	    xe_msg (msg, 1);
	    return (-1);
	}

	/* make into a FILE pointer for use with fprintf */
	fp = fdopen (fd, "w");
	if (!fp) {
	    (void) sprintf (msg, "Can not fdopen %.100s\n", fn);
	    xe_msg (msg, 0);
	    (void) close (fd);
	    return (-1);
	}

	*fnp = fn;
	*fpp = fp;

	return (0);
}

/* #define WANT_MATHERR if your system supports it. it gives SGI fits.
 */
#undef WANT_MATHERR
#if defined(WANT_MATHERR)
/* attempt to do *something* reasonable when a math function blows.
 */
matherr (xp)
struct exception *xp;
{
	static char *names[8] = {
	    "acos", "asin", "atan2", "pow",
	    "exp", "log", "log10", "sqrt"
	};
	int i;

	/* catch-all */
	xp->retval = 0.0;

	for (i = 0; i < sizeof(names)/sizeof(names[0]); i++)
	    if (strcmp (xp->name, names[i]) == 0)
		switch (i) {
		case 0:	/* acos */
		    xp->retval = xp->arg1 >= 1.0 ? 0.0 : -PI;
		    break;
		case 1: /* asin */
		    xp->retval = xp->arg1 >= 1.0 ? PI/2 : -PI/2;
		    break;
		case 2: /* atan2 */
		    if (xp->arg1 == 0.0)
			xp->retval = xp->arg2 < 0.0 ? PI : 0.0;
		    else if (xp->arg2 == 0.0)
			xp->retval = xp->arg1 < 0.0 ? -PI/2 : PI/2;
		    else
			xp->retval = 0.0;
		    break;
		case 3: /* pow */
		/* FALLTHRU */
		case 4: /* exp */
		    xp->retval = xp->type == OVERFLOW ? 1e308 : 0.0;
		    break;
		case 5: /* log */
		/* FALLTHRU */
		case 6: /* log10 */
		    xp->retval = xp->arg1 <= 0.0 ? -1e308 : 0;
		    break;
		case 7: /* sqrt */
		    xp->retval = 0.0;
		    break;
		}

        return (1);     /* suppress default error handling */
}
#endif

/* send these coords, epoch, sv_fov and mag to the SKYLOCFIFO.
 * open/close fifo each time.
 * if test is set then just test the fifo.
 * return 0 if all ok, else xe_msg() and return -1.
 * N.B. we assume SIGPIPE is being ignored.
 */
int
skyloc_fifo (test, ra, dec, y, fov, mag)
int test;	/* 0 for normal use; 1 to just test if fifo is ok */
double ra, dec, y;	/* center location (rads) and epoch (year) */
double fov;	/* field of view, rads */
int mag;	/* limiting magnitude */
{
	static char fmt[]="RA:%9.6f Dec:%9.6f Epoch:%9.3f FOV:%7.5f Mag:%3d\n";
	static char fifores[] = "SKYLOCFIFO";	/* name of X resource */
	static char *fn;	/* file name */
	FILE *fp;		/* FILE pointer */

	/* open fifo */
	if (open_wfifores (fifores, &fn, &fp) < 0)
	    return(-1);
	if (test) {
	    (void) fclose (fp);
	    return (0);
	}

	/* send the formatted location string */
	if (fprintf (fp, fmt, ra, dec, y, fov, mag) < 0 || fflush (fp) != 0) {
	    char msg[256];
	    (void) sprintf (msg, "Error writing to %.100s: %.100s\n", fn,
								syserrstr());
	    xe_msg (msg, 1);
	    (void) fclose (fp);
	    return (-1);
	}

	(void) fclose (fp);
	return (0);
}

/* given the difference in two RA's, in rads, return their difference,
 *   accounting for wrap at 2*PI. caller need *not* first force it into the
 *   range 0..2*PI.
 */
double
delra (dra)
double dra;
{
	double fdra = fmod(fabs(dra), 2*PI);

	if (fdra > PI)
	    fdra = 2*PI - fdra;
	return (fdra);
}
