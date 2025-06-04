/* code to manage the stuff on the sky view display.
 * the filter menu is in filtermenu.c.
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#if defined(__STDC__)
#include <stdlib.h>
#include <string.h>
#else
extern void *malloc();
#endif 

#if defined(_POSIX_SOURCE)
#include <unistd.h>
#else
extern int close();
#endif

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/Text.h>
#include <Xm/Scale.h>
#include <Xm/Separator.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "trails.h"
#include "skyhist.h"
#include "skylist.h"
#include "ps.h"

extern Widget toplevel_w;
extern char *myclass;
extern XtAppContext xe_app;
#define XtD XtDisplay(toplevel_w)

extern Now *mm_get_now P_((void));
extern Obj *db_basic P_((int id));
extern Obj *db_scan P_((DBScan *sp));
extern char *cns_name P_((int id));
extern char *mm_getsite P_((void));
extern char *obj_description P_((Obj *op));
extern char *syserrstr P_((void));
extern double delra P_((double dra));
extern int cns_figure P_((int id, double e, double ra[], double dec[],
    int dcodes[]));
extern int cns_list P_((double ra, double dec, double e, double rad,
    int ids[]));
extern int cns_edges P_((double e, double **era0, double **edec0,
    double **era1, double **edec1));
extern int cns_pick P_((double ra, double dec, double e));
extern int db_n P_((void));
extern int f_ison P_((void));
extern int get_color_resource P_((Widget w, char *cname, Pixel *p));
extern int lc P_((int cx, int cy, int cw, int x1, int y1, int x2, int y2,
    int *sx1, int *sy1, int *sx2, int *sy2));
extern int magdiam P_((int fmag, double scale, double mag, double size));
extern int obj_cir P_((Now *np, Obj *op));
extern int open_wfifores P_((char *xres, char **fnp, FILE **fpp));
extern int openh P_((char *name, int flags));
extern int skyloc_fifo P_((int test, double ra, double dec, double yr,
    double fov, int mag));
extern int svf_filter_ok P_((Obj *op));
extern int svf_ismanaged P_((void));
extern int tickmarks P_((double min, double max, int numdiv, double ticks[]));
extern void all_newdb P_((int appended));
extern void db_scaninit P_((DBScan *sp));
extern void db_update P_((Obj *op));
extern void fs_date P_((char out[], double jd));
extern void fs_mtime P_((char out[], double t));
extern void fs_pangle P_((char out[], double a));
extern void fs_ra P_((char out[], double ra));
extern void fs_sexa P_((char *out, double a, int w, int fracbase));
extern void fs_time P_((char out[], double t));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void get_views_font P_((Display *dsp, XFontStruct **fspp));
extern void get_xmlabel_font P_((Widget w, XFontStruct **f));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void loadGreek P_((Display *dsp, Drawable win, GC *greekgcp,
    XFontStruct **greekfspp));
extern void now_lst P_((Now *np, double *lst));
extern void obj_pickgc P_((Obj *op, Widget w, GC *gcp));
extern void obj_set P_((Obj *op));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void solve_sphere P_((double A, double b, double cosc, double sinc,
    double *cosap, double *Bp));
extern void sv_all P_((Now *np));
extern void sv_draw_obj P_((Display *dsp, Drawable win, GC gc, Obj *op, int x,
    int y, int diam, int dotsonly));
extern void sv_update P_((Now *np, int how_much));
extern void svf_create P_((void));
extern void svf_getmaglimits P_((int *fmagp, int *bmagp));
extern void svf_manage P_((void));
extern void svf_setmaglimits P_((int fmag, int bmag));
extern void svf_unmanage P_((void));
extern void timestamp P_((Now *np, Widget w));
extern void watch_cursor P_((int want));
extern void xe_msg P_((char *msg, int app_modal));
extern void zero_mem P_((void *loc, unsigned len));

/* we keep a linked-list of TrailObj's we want trails for. these in turn
 *   contain an array of TSky's for each location in the trail.
 * objects are added and controlled from the popup; we only ever actually
 *   discard an object if the whole db changes or it is off when we get an
 *   update.
 */
typedef struct {
    unsigned flags;	/* OBJF_* flags (shared with Obj->o_flags) */
    TrTS trts;		/* mjd of o and whether to draw timestamp */
    Obj o;		/* copy of the object at ts_mjd */
} TSky;
struct trailObj {
    struct trailObj *ntop;	/* pointer to next, or NULL */
    int on;		/* turn trail on/off (discarded if off during update) */
    TrState trs;	/* general details of the trail setup */
    Obj *op;		/* pointer to actual db object being trailed */
    int nsky;		/* number of items in use within sky[] */
    TSky sky[1];	/* array of Objs (gotta love C :-) */
};
typedef struct trailObj TrailObj;

static void sv_copy_sky P_((void));
static void sv_create_svform P_((void));
static void sv_create_find P_((Widget parent));
static void sv_close_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_help_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_helpon_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_aa_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_filter_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_grid_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_out_fifo_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_in_fifo_cb P_((Widget w, XtPointer client, XtPointer call));
static void skyinfifo_cb P_((XtPointer client, int *fdp, XtInputId *idp));
static void sv_loc_fifo_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_print_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_print P_((void));
static void sv_ps_annotate P_((Now *np, int fs));
static void sv_track_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_set_aimscales P_((void));
static void sv_set_fovscale P_((void));
static void sv_da_exp_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_da_input_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_da_motion_cb P_((Widget w, XtPointer client, XEvent *ev,
    Boolean *continue_to_dispatch));
static void sv_scale_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_option_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_finding_cb P_((Widget wid, XtPointer client, XtPointer call));
static void sv_find_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_popup P_((XEvent *ev, Obj *op, TSky *tsp));
static void sv_create_popup P_((void));
static void sv_magscale P_((Display *dsp, Window win, unsigned r, unsigned xb,
    unsigned yb));
static void sv_create_zoomcascade P_((Widget pd_w));
static void sv_pu_activate_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_pu_zoom_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_pu_trail_cb P_((Widget wid, XtPointer client, XtPointer call));
static void sv_pu_label_cb P_((Widget wid, XtPointer client, XtPointer call));
static void sv_pu_track_cb P_((Widget wid, XtPointer client, XtPointer call));
static void sv_read_scale P_((int which));
static void sv_draw_track_coords P_((unsigned int w, unsigned int h,
    double altdec, double azra, double start_altdec, double start_azra));
static int sv_mark P_((Obj *op, int in_center, int out_center, int mark,
    double newfov));
static void sv_set_dashed_congc P_((Display *dsp, int dashed));
static void tobj_rmoff P_((void));
static void tobj_rmobj P_((Obj *op));
static TrailObj *tobj_addobj P_((Obj *op, int nsky));
static void tobj_newdb P_((void));
static TrailObj *tobj_find P_((Obj *op));
static void tobj_display_all P_((unsigned r, unsigned xb, unsigned yb));
static void sv_getcircle P_((unsigned int *wp, unsigned int *hp,
    unsigned int *rp, unsigned int *xbp, unsigned int *ybp));
static int sv_mktrail P_((TrTS ts[], TrState *statep, XtPointer client));
static int sv_dbobjloc P_((Obj *op, int r, int *xp, int *yp));
static int sv_trailobjloc P_((TSky *tsp, int r, int *xp, int *yp));
static int sv_precheck P_((Obj *op));
static int sv_infov P_((Obj *op));
static int sv_loc P_((int rad, double altdec, double azra, int *xp, int *yp));
static int sv_unloc P_((int rad, int x, int y, double *altdecp, double *azrap));
static void sv_fullwhere P_((Now *np, double altdec, double azra, double *altp,
    double *azp, double *rap, double *decp));
static void draw_ecliptic P_((Now *np, Display *dsp, Window win, GC gc,
    unsigned int r, unsigned int xb, unsigned int yb));
static void draw_cnsbounds P_((Now *np, Display *dsp, Window win,
    unsigned int r, unsigned int xb, unsigned int yb));
static void draw_cns P_((Now *np, Display *dsp, Window win, 
    unsigned int r, unsigned int xb, unsigned int yb));
static void draw_cnsname P_(( Display *dsp, Window win, int conid,
    int minx, int miny, int maxx, int maxy));
static void draw_grid P_((Display *dsp, Window win, GC gc, unsigned int r,
    unsigned int xb, unsigned int yb));
static void draw_label P_((Display *dsp, Window win, GC gc, Obj *op,
    int x, int y));
static int draw_greeklabel P_((Display *dsp, Window win, GC gc, char name[],
    int x, int y));
static int objdiam P_((Obj *op));
static void sv_mk_gcs P_((Display *dsp, Window win));
static void skyin_fifo P_((void));
static void skyout_fifo P_((Now *np));


#define	NVGRID		20	/* max alt/dec grid lines */
#define	NHGRID		20	/* max az/ra grid lines */
#define	NSEGS		3	/* piecewise segments per arc */
#define	ECL_TICS	2	/* period of points for ecliptic, pixels */
#define	MARKR		20	/* pointer marker half-width, pixels */
#define	TICKLEN		4	/* length of trail tickmarks, pixels */
#define	NBRLABELS	10	/* max number of brightest objects we label */
#define	PRFONTSZ	10	/* postscript font size */

/* set NGRID to max of NVGRID and NHGRID */
#if NVGRID > NHGRID
#define NGRID NVGRID		/* max possible grid lines any dimension */
#else
#define NGRID NHGRID		/* max possible grid lines any dimension */
#endif

static Widget svform_w;			/* main sky view form dialog */
static Widget svda_w;			/* sky view drawing area */
static Widget fov_w, altdec_w, azra_w;	/* scale widgets */
static Widget hgrid_w, vgrid_w;		/* h and v grid spacing labels */
static Widget aa_w, rad_w;		/* altaz/radec toggle buttons */
static Widget fliplr_w, fliptb_w;	/* orientation TBs */
static Widget dt_w;			/* the date/time stamp label widget */
static Widget find_w[2];		/* cascade buttons for "find" objx/y */
static Pixmap sv_pm;			/* off-screen pixmap we *really* draw */
static XFontStruct *sv_sf;		/* font for the tracking coord display*/
static XFontStruct *sv_pf;		/* font for all object labels */
static XFontStruct *sv_gf;		/* greek font */
static GC sv_ggc;			/* greek gc */

/* pixels and GCs
 */
static Pixel fg_p;		/* fg color */
static Pixel bg_p;		/* background color */
static Pixel cnsfig_p;		/* constellation figures color */
static Pixel cnsbnd_p;		/* constellation boundaries color */
static Pixel sky_p;		/* sky background color */
static GC sv_gc;		/* the default GC */
static GC sv_cnsgc;		/* the GC for constellations */
static GC sv_strgc;		/* gc for use in drawing text */

static int aa_mode = -1;	/* 1 for alt/az or 0 for ra/dec */
static double sv_altdec;	/* view center alt or dec, rads */
static double sv_azra;		/* view center az or ra, rads */
static double sv_fov;		/* field of view, rads */
static double sv_sov;		/* scale of view, pixels per arcsecond */
static int justdots;		/* set when only want to use dots on the map */
static int flip_lr;		/* set when want to flip left/right */
static int flip_tb;		/* set when want to flip top/bottom */
static int want_ecliptic;	/* set when want to see the ecliptic */
static int want_conbounds;	/* set when want to see the constel boundaries*/
static int want_configures;	/* set when want to see the constel figures */
static int want_connames;	/* set when want to see the constel names */
static int want_magscale;	/* set when want to see magnitude scale */
static TrailObj *trailobj;	/* head of a malloced linked-list -- 0 when
				 * empty
				 */
static TrState trstate = {	/* trail setup state */
    TRLR_FL, TRI_DAY, TRF_DATE, TRR_DAY, TRO_PATHL, TRS_MEDIUM, 10
};
static Obj *track_op;		/* object to track, when not 0 */
static Widget tracktb_w;	/* toggle button indicating tracking is on */
static int want_grid;		/* set when we want to draw the coord grid */
static int sv_ournewobj;	/* used to inhibit useless redrawing */
static int fwasup;		/* set when filter menu was up before */

static int lbl_names;		/* set when want to label names */
static int lbl_mags;		/* set when want to label magnitudes */
static int lbl_all;		/* set when want to label everything */
static int lbl_br;		/* set when want to label just the brightest */
static Widget lbl_n_w;		/* TB for lbl_names */
static Widget lbl_m_w;		/* TB for lbl_mags */
static Widget lbl_a_w;		/* TB for lbl_all */
static Widget lbl_b_w;		/* TB for lbl_br */
static char br_tb_lbl[20];	/* label for lbl_b_w */

static int want_outfifo;	/* set to send center coords to fifo */
static Widget outfifo_w;	/* toggle button controlling out fifo */
static int want_infifo;		/* set to accept aiming coord on fifo */
static Widget infifo_w;		/* toggle button controlling in fifo */
static int want_locfifo;	/* set to enable sending menu coord to fifo */
static Widget locfifo_w;	/* toggle button controlling loc fifo */

static char null_grid_str[] = "             ";

/* info for the popup widget.
 * we make one once and keep reusing it -- seems to be a bit faster that way.
 * but see sv_popup() for all the work to get it customized for each object.
 */
typedef struct {
    Widget pu_w;	/* the overall PopupMenu */
    Widget name_w;	/* label for object name */
    Widget desc_w;	/* label for object description */
    Widget spect_w;	/* label for object spectral class */
    Widget size_w;	/* label for object size */
    Widget ud_w;	/* label for object UT date */
    Widget ut_w;	/* label for object UT time */
    Widget ra_w;	/* label for object RA */
    Widget dec_w;	/* label for object Dec */
    Widget alt_w;	/* label for object Alt */
    Widget az_w;	/* label for object Az */
    Widget mag_w;	/* label for object mag */
    Widget locfifo_w;	/* PB to send coords to LOCFIFO */
    Widget label_w;	/* TB to select persistent label be drawn */
    Widget make_w;	/* PB to make this object x/y */
    Widget track_w;	/* TB to track this object */
    Widget trail_w;	/* TB to let existing trail be turned on/off */
    Widget newtrail_w;	/* PB to create new trail */
    Obj *op;		/* real database pointer, or possibly svobj */
    TSky *tsp;		/* this is used if we are displaying a trailed object */
} Popup;
static Popup pu;
static Obj svobj;	/* used to point when far from any real object */

/* popup button activate codes */
enum {
    GOBACK, AIM, LOCFIFO, MK_OBJX, NEWTRAIL
};

enum SCALES {			/* scale changed codes */
    FOV_S, ALTDEC_S, AZRA_S
};

/* called when the sky view is activated via the main menu pulldown.
 * if never called before, create and manage all the widgets as a child of a
 * form. otherwise, just toggle whether the form is managed.
 * also manage the corresponding filter dialog along with.
 * freeing the pixmap will force a fresh update on the next expose.
 */
void
sv_manage ()
{
	if (!svform_w) {
	    /* compose the label for the Views->Labels->Brightest TB dynamically
	     * to eliminate maintenance when NGBRLABELS changes.
	     */
	    (void) sprintf (br_tb_lbl, "Brightest %d", NBRLABELS);

	    /* create the main form, the filter and history list */
	    sv_create_svform();
	}
	
	if (XtIsManaged(svform_w)) {
	    XtUnmanageChild (svform_w);
	    if ((fwasup = svf_ismanaged()))
		svf_unmanage();
	    if (sv_pm) {
		XFreePixmap (XtDisplay(svda_w), sv_pm);
		sv_pm = (Pixmap) 0;
	    }
	} else {
	    XtManageChild (svform_w);
	    /* rely on expose to cause a fresh update */
	    if (fwasup)
		svf_manage();
	}
}

int
sv_ison()
{
	return (svform_w && XtIsManaged(svform_w));
}

/* called when a user-defined object has changed.
 * take it off the trailobj list, if it's there (it's ok if it's not).
 * then since we rely on knowing our update will be called we need do nothing
 *   more to redisplay without the object.
 */
void
sv_newobj(dbidx)
int dbidx;		/* OBJX or OBJY */
{
	Obj *op = db_basic(dbidx);

	tobj_rmobj (op);
}

/* called when the db (beyond NOBJ) has changed.
 * if it was appended to we can just redraw else it was reduced and we need to
 * discard any trails we are keeping for objects that no longer exist and
 * make sure no pending trail creations can come in and do something.
 */
void
sv_newdb(appended)
int appended;
{
	if (!appended) {
	    tobj_newdb();
	    pu.op = NULL;	/* effectively disables sv_mktrail() */
	}

	sv_update (mm_get_now(), 1);
}

/* called when we are to update our view.
 * don't bother if our own sv_ournewobj flag is set, if we are not managed or
 *   if fields are off.
 * discard any trails that have been turned off.
 * reaim if we are tracking an object.
 */
/* ARGSUSED */
void
sv_update (np, how_much)
Now *np;
int how_much;
{
	int up;

	if (sv_ournewobj)
	    return;

	up = svform_w && XtIsManaged(svform_w);
	if (!up || !f_ison())
	    return;

	/* remove trails not on now */
	tobj_rmoff();

	if (track_op) {
	    db_update (track_op);
	    if (sv_mark (track_op, 1, 1, 1, 0.0) < 0)
		sv_all(np);	/* still need it even if mark failed */
	} else 
	    sv_all(np);
}

/* called to reinstate a SvHistory */
void
svh_goto (hp)
SvHistory *hp;
{
	/* restore aa_mode.
	 * N.B. do this before calling sv_aim_scales().
	 */
	if (hp->aa_mode != aa_mode) {
	    aa_mode = hp->aa_mode;
	    XmToggleButtonSetState (aa_w, aa_mode, False);
	    XmToggleButtonSetState (rad_w, !aa_mode, False);
	}

	/* restore aim scales */
	sv_altdec = hp->altdec;;
	sv_azra = hp->azra;
	sv_set_aimscales();

	/* restore fov scale */
	sv_fov = hp->fov;
	sv_set_fovscale();

	/* restore flip orientation */
	XmToggleButtonSetState (fliplr_w, flip_lr = hp->flip_lr, False);
	XmToggleButtonSetState (fliptb_w, flip_tb = hp->flip_tb, False);

	/* restore mag limits */
	svf_setmaglimits (hp->fmag, hp->bmag);

	/* restore labeling options */
	lbl_names = hp->lbl_n;
	XmToggleButtonSetState (lbl_n_w, lbl_names, False);
	lbl_mags = hp->lbl_m;
	XmToggleButtonSetState (lbl_m_w, lbl_mags, False);
	lbl_all = hp->lbl_a;
	XmToggleButtonSetState (lbl_a_w, lbl_all, False);
	lbl_br = hp->lbl_b;
	XmToggleButtonSetState (lbl_b_w, lbl_br, False);

	/* do it */
	sv_all(mm_get_now());
}

/* called by the history mechanism when it needs to know the current settings.
 */
void
svh_get (hp)
SvHistory *hp;
{

	hp->fov = sv_fov;
	hp->altdec = sv_altdec;
	hp->azra = sv_azra;
	hp->aa_mode = aa_mode;
	hp->flip_lr = flip_lr;
	hp->flip_tb = flip_tb;
	svf_getmaglimits (&hp->fmag, &hp->bmag);
	hp->lbl_n = lbl_names;
	hp->lbl_m = lbl_mags;
	hp->lbl_a = lbl_all;
	hp->lbl_b = lbl_br;
}

/* point at the given object and mark it.
 * N.B. we do *not* update the s_ fields of op.
 */
void
sv_point (op)
Obj *op;
{
	if (!sv_ison() || !op || op->type == UNDEFOBJ)
	    return;

	(void) sv_mark (op, 1, 1, 1, 0.0);
}

/* show a marker at the location of the given object if it's in fov now.
 * N.B. we do *not* update the s_ fields of op.
 */
void
sv_id (op)
Obj *op;
{
	if (!sv_ison() || !op || op->type == UNDEFOBJ)
	    return;

	(void) sv_mark (op, 0, 0, 1, 0.0);
}

/* called to put up or remove the watch cursor.  */
void
sv_cursor (c)
Cursor c;
{
	Window win;

	if (svform_w && (win = XtWindow(svform_w))) {
	    Display *dsp = XtDisplay(svform_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}

/* draw everything subject to any filtering.
 * also emit the new center coords to the SKYOUTFIFO.
 */
void
sv_all(np)
Now *np;
{
	Display *dsp = XtDisplay(svda_w);
	Window win = sv_pm;
	typedef struct {
	    Obj *op;
	    int x, y;
	    GC gc;
	} LabelSummary;
	LabelSummary ls[NBRLABELS];
	int nls = 0;
	DBScan dbs;
	unsigned int w, h, r, xb, yb;
	Obj *op;
	int i;

	watch_cursor(1);

	/* put up the timestamp */
	timestamp (np, dt_w);

	sv_getcircle (&w, &h, &r, &xb, &yb);
	sv_sov = 2*r / (raddeg (sv_fov)*3600.0);

	/* rebuild the clean circle */
	XSetForeground (dsp, sv_gc, bg_p);
	XFillRectangle (dsp, win, sv_gc, 0, 0, w, h);
	XSetForeground (dsp, sv_gc, sky_p);
	XFillArc (dsp, win, sv_gc, xb+1, yb+1, 2*r-2, 2*r-2, 0, 360*64);
	XSetForeground (dsp, sv_gc, fg_p);
	XPSDrawArc (dsp, win, sv_gc, xb, yb, 2*r-1, 2*r-1, 0, 360*64);

	/* draw the magnitude scale if desired */
	if (want_magscale)
	    sv_magscale (dsp, win, r, xb, yb);

	/* draw the coord grid if desired */
	if (want_grid) {
	    XSetForeground (dsp, sv_gc, fg_p);
	    draw_grid(dsp, win, sv_gc, r, xb, yb);
	}

	/* draw the ecliptic if desired */
	if (want_ecliptic) {
	    XSetForeground (dsp, sv_gc, fg_p);
	    draw_ecliptic (np, dsp, win, sv_gc, r, xb, yb);
	}

	/* draw constellation boundaries if desired */
	if (want_conbounds) {
	    XSetForeground (dsp, sv_gc, fg_p);
	    draw_cnsbounds (np, dsp, win, r, xb, yb);
	}

	/* draw constellation figures and/or names if desired */
	if (want_configures || want_connames) {
	    XSetForeground (dsp, sv_gc, fg_p);
	    draw_cns (np, dsp, win, r, xb, yb);
	}
	    
	/* go through the database and display what we want.
	 * if needbr, keep the brightest in bropp list.
	 */
	for (db_scaninit(&dbs); (op = db_scan(&dbs)) != NULL; ) {
	    int x, y;

	    if (!sv_dbobjloc(op, r, &x, &y))
		op->o_flags &= ~OBJF_ONSCREEN;
	    else {
		int d;
		GC gc;

		/* yup, it's really supposed to be on the screen */
		op->o_flags |= OBJF_ONSCREEN;

		/* find object's size and location.
		 * N.B. we assume sv_dbobjloc() called db_update()
		 */
		d = objdiam(op);
		x += xb;
		y += yb;
		obj_pickgc(op, svda_w, &gc);
		sv_draw_obj (dsp, win, gc, op, x, y, d, justdots);

		/* draw each persistent label or just everything */
		if ((op->o_flags & OBJF_LABEL) || lbl_all) {
		    /* flush pending graphics so label more likely on top */
		    sv_draw_obj (dsp, win, (GC)0, NULL, 0, 0, 0, 0);
		    draw_label (dsp, win, gc, op, x+d/4, y-d/4);
		}

		/* check for Brightest */
		if (lbl_br) {
		    /* insert op into bropp by decreasing brightness (incr mag).
		     * optimized assuming most objects are too dim.
		     */
		    for (i = nls; --i >= 0 && op->s_mag < ls[i].op->s_mag; )
			if (i < NBRLABELS-1)
			    ls[i+1] = ls[i];
		    if (++i < NBRLABELS) {
			ls[i].op = op;
			ls[i].gc = gc;
			ls[i].x  = x+d/4;
			ls[i].y  = y-d/4;
			if (nls < NBRLABELS)
			    nls++;
		    }
		}
	    }
	}

	/* flush */
	sv_draw_obj (dsp, win, (GC)0, NULL, 0, 0, 0, 0);

	/* draw the Brightest labels or magnitudes */
	if (lbl_br) {
	    for (i = 0; i < nls; i++) {
		LabelSummary *lp = &ls[i];
		draw_label (dsp, win, lp->gc, lp->op, lp->x, lp->y);
	    }
	}

	/* go through the trailobj list and display that stuff too. */
	tobj_display_all(r, xb, yb);

	/* emit our center coords */
	skyout_fifo (np);

	/* and we're done */
	sv_copy_sky();

	watch_cursor(0);
}

/* draw the given object so it has a nominal diameter of diam pixels.
 * we maintain a static cache of common X drawing objects for efficiency.
 * (mallocing seemed to keep the memory arena too fragmented).
 * to force a flush of this cache, call with op == NULL.
 */
void
sv_draw_obj (dsp, win, gc, op, x, y, diam, dotsonly)
Display *dsp;
Drawable win;
GC gc;
Obj *op;
int x, y;
int diam;
int dotsonly;
{
#define	MINSYMDIA	7	/* never use fancy symbol at diam this small */
#define	CACHE_SZ	100	/* size of points/arcs/etc cache */
#define	CACHE_PAD	10	/* most we ever need in one call */
#define	CACHE_HWM	(CACHE_SZ - CACHE_PAD)	/* hi water mark */
	static XPoint   xpoints[CACHE_SZ],   *xp  = xpoints;
	static XArc     xdrawarcs[CACHE_SZ], *xda = xdrawarcs;
	static XArc     xfillarcs[CACHE_SZ], *xfa = xfillarcs;
	static XSegment xsegments[CACHE_SZ], *xs  = xsegments;
	static GC cache_gc;
	int force;
	int t;

	/* for sure if no op or different gc */
	force = !op || gc != cache_gc;

	if (force || xp >= xpoints + CACHE_HWM) {
	    int n;
	    if ((n = xp - xpoints) > 0) {
		XPSDrawPoints (dsp, win, cache_gc, xpoints, n, CoordModeOrigin);
		xp = xpoints;
	    }
	}
	if (force || xda >= xdrawarcs + CACHE_HWM) {
	    int n;
	    if ((n = xda - xdrawarcs) > 0) {
		XPSDrawArcs (dsp, win, cache_gc, xdrawarcs, n);
		xda = xdrawarcs;
	    }
	}
	if (force || xfa >= xfillarcs + CACHE_HWM) {
	    int n;
	    if ((n = xfa - xfillarcs) > 0) {
		XPSFillArcs (dsp, win, cache_gc, xfillarcs, n);
		xfa = xfillarcs;
	    }
	}
	if (force || xs >= xsegments + CACHE_HWM) {
	    int n;
	    if ((n = xs - xsegments) > 0) {
		XPSDrawSegments (dsp, win, cache_gc, xsegments, n);
		xs = xsegments;
	    }
	}

	cache_gc = gc;

	if (!op)
	    return;	/* just flushing, thanks */

	/* if dotsonly is on or object is smallish then we don't use the fancy
	 * symbols, just use dots or filled circles.
	 */
	if (dotsonly || diam <= MINSYMDIA) {
	    if (diam <= 1) {
		xp->x = x;
		xp->y = y;
		xp++;
	    } else {
		xfa->x = x - diam/2;
		xfa->y = y - diam/2;
		xfa->width = diam;
		xfa->height = diam;
		xfa->angle1 = 0;
		xfa->angle2 = 360*64;
		xfa++;
	    }
	    return;
	}

	switch (op->type) {
	case PLANET:
	    /* filled circle */
	    xfa->x = x - diam/2;
	    xfa->y = y - diam/2;
	    xfa->width = diam;
	    xfa->height = diam;
	    xfa->angle1 = 0;
	    xfa->angle2 = 360*64;
	    xfa++;
	    break;
	case FIXED:
	    switch (op->f_class) {
	    case 'G': case 'H': case 'A': /* galaxy */
		/* filled ellipse */
		xda->x = x - diam/2;
		xda->y = y - diam/4;
		xda->width = diam;
		xda->height = diam/2;
		xda->angle1 = 0;
		xda->angle2 = 360*64;
		xda++;
		break;
	    case 'C': case 'U': /* globular clusters */
		/* same as open cluster but with a central smaller ring */
		{
		    int r = diam/4;	/* ring radius */
		    int na = 2*PI*r/3;	/* number of segments spaced 3 dots */
		    double da = 2*PI/na;/* rads per segment */
		    int np;

		    while (--na >= 0) {
			/* be mindful of filling the xpoints cache */
			if ((np = xp - xpoints) > CACHE_HWM) {
			    XPSDrawPoints (dsp, win, cache_gc, xpoints, np,
							    CoordModeOrigin);
			    xp = xpoints;
			}
			xp->x = x + (int)floor(r*cos(na*da) + 0.5);
			xp->y = y + (int)floor(r*sin(na*da) + 0.5);
			xp++;
		    }
		}
		/* fall through */
		/* FALLTHROUGH */
	    case 'O': /* open cluster */
		/* ring of dots */
		{
		    int r = diam/2;	/* ring radius */
		    int na = 2*PI*r/3;	/* number of segments spaced 3 dots */
		    double da = 2*PI/na;/* rads per segment */
		    int np;

		    while (--na >= 0) {
			/* be mindful of filling the xpoints cache */
			if ((np = xp - xpoints) > CACHE_HWM) {
			    XPSDrawPoints (dsp, win, cache_gc, xpoints, np,
							    CoordModeOrigin);
			    xp = xpoints;
			}
			xp->x = x + (int)floor(r*cos(na*da) + 0.5);
			xp->y = y + (int)floor(r*sin(na*da) + 0.5);
			xp++;
		    }
		}
		break;
	    case 'P': /* planetary nebula */
		/* open square */
		t = diam/2;
		xs->x1 = x-t; xs->y1 = y-t; xs->x2 = x+t; xs->y2 = y-t; xs++;
		xs->x1 = x+t; xs->y1 = y-t; xs->x2 = x+t; xs->y2 = y+t; xs++;
		xs->x1 = x+t; xs->y1 = y+t; xs->x2 = x-t; xs->y2 = y+t; xs++;
		xs->x1 = x-t; xs->y1 = y+t; xs->x2 = x-t; xs->y2 = y-t; xs++;
		break;
	    case 'R': /* supernova remnant */
		/* two concentric circles */
		xda->x = x - diam/2;
		xda->y = y - diam/2;
		xda->width = diam+1;
		xda->height = diam+1;
		xda->angle1 = 0;
		xda->angle2 = 360*64;
		xda++;
		xda->x = x - diam/4;
		xda->y = y - diam/4;
		xda->width = diam/2+1;
		xda->height = diam/2+1;
		xda->angle1 = 0;
		xda->angle2 = 360*64;
		xda++;
		break;
	    case 'S': /* stars */
		/* filled circle */
		xfa->x = x - diam/2;
		xfa->y = y - diam/2;
		xfa->width = diam;
		xfa->height = diam;
		xfa->angle1 = 0;
		xfa->angle2 = 360*64;
		xfa++;
		break;
	    case 'B': case 'D': /* binary and double stars */
		/* filled circle with one horizontal line through it */
		xfa->x = x - diam/2;
		xfa->y = y - diam/2;
		xfa->width = diam;
		xfa->height = diam;
		xfa->angle1 = 0;
		xfa->angle2 = 360*64;
		xfa++;
		t = 3*diam/4;
		xs->x1 = x - t;
		xs->y1 = y;
		xs->x2 = x + t;
		xs->y2 = y;
		xs++;
		break;
	    case 'M': /* multiple stars */
		/* filled circle with two horizontal lines through it */
		xfa->x = x - diam/2;
		xfa->y = y - diam/2;
		xfa->width = diam;
		xfa->height = diam;
		xfa->angle1 = 0;
		xfa->angle2 = 360*64;
		xfa++;
		t = 3*diam/4;
		xs->x1 = x - t;
		xs->y1 = y - diam/6;
		xs->x2 = x + t;
		xs->y2 = y - diam/6;
		xs++;
		xs->x1 = x - t;
		xs->y1 = y + diam/6;
		xs->x2 = x + t;
		xs->y2 = y + diam/6;
		xs++;
		break;
	    case 'V': /* variable star */
		/* central dot with concentric circle */
		xfa->x = x - diam/4;
		xfa->y = y - diam/4;
		xfa->width = diam/2;
		xfa->height = diam/2;
		xfa->angle1 = 0;
		xfa->angle2 = 360*64;
		xfa++;
		xda->x = x - diam/2;
		xda->y = y - diam/2;
		xda->width = diam;
		xda->height = diam;
		xda->angle1 = 0;
		xda->angle2 = 360*64;
		xda++;
		break;
	    case 'F': case 'K': /* diffuse and dark nebulae */
		/* open diamond */
		t = diam/2;
		xs->x1 = x-t; xs->y1 = y; xs->x2 = x; xs->y2 = y-t; xs++;
		xs->x1 = x; xs->y1 = y-t; xs->x2 = x+t; xs->y2 = y; xs++;
		xs->x1 = x+t; xs->y1 = y; xs->x2 = x; xs->y2 = y+t; xs++;
		xs->x1 = x; xs->y1 = y+t; xs->x2 = x-t; xs->y2 = y; xs++;
		break;
	    case 'N': /* bright nebulae */
		/* open hexagon */
		t = diam/2;
		xs->x1 = x-t; xs->y1 = y; xs->x2 = x-t/2; xs->y2 = y-t; xs++;
		xs->x1 = x-t/2; xs->y1 = xs->y2 = y-t; xs->x2 = x+t/2;  xs++;
		xs->x1 = x+t/2; xs->y1 = y-t; xs->x2 = x+t; xs->y2 = y; xs++;
		xs->x1 = x+t; xs->y1 = y; xs->x2 = x+t/2; xs->y2 = y+t; xs++;
		xs->x1 = x+t/2; xs->y1 = xs->y2 = y+t; xs->x2 = x-t/2;  xs++;
		xs->x1 = x-t/2; xs->y1 = y+t; xs->x2 = x-t; xs->y2 = y; xs++;
		break;
	    case 'Q': /* Quasar */
		/* plus sign */
		xs->x1 = x-diam/2;	xs->y1 = y;
		    xs->x2 = x+diam/2;	    xs->y2 = y;		xs++;
		xs->x1 = x;		xs->y1 = y-diam/2;
		    xs->x2 = x;		    xs->y2 = y+diam/2;	xs++;
		break;
	    case 'L':	/* Pulsar */
		/* vertical line */
		xs->x1 = x;	xs->y1 = y-diam/2;
		    xs->x2 = x;	    xs->y2 = y+diam/2;		xs++;
		break;
	    case 'J':	/* Radio source */
		/* half-circle pointing up and to the left */
		xda->x = x - diam/2;
		xda->y = y - diam/2;
		xda->width = diam+1;
		xda->height = diam+1;
		xda->angle1 = 210*64;
		xda->angle2 = 180*64;
		xda++;
		break;
	    case 'T':	/* stellar object */
	    case '\0':	/* undefined type */
	    default:	/* unknown type */
		/* an X */
		xs->x1 = x-diam/3;	xs->y1 = y-diam/3;
		    xs->x2 = x+diam/3;	    xs->y2 = y+diam/3;	xs++;
		xs->x1 = x-diam/3;	xs->y1 = y+diam/3;
		    xs->x2 = x+diam/3;    	xs->y2 = y-diam/3;	xs++;
		break;
	    }
	    break;
	case HYPERBOLIC:
	case PARABOLIC:
	    /* usually cometose -- draw a filled circle with tail */
	    t = 3*diam/4;
	    xfa->x = x - diam/4;
	    xfa->y = y - diam/4;
	    xfa->width = diam/2;
	    xfa->height = diam/2;
	    xfa->angle1 = 0;
	    xfa->angle2 = 360*64;
	    xfa++;
	    xfa->x = x - t;
	    xfa->y = y - t;
	    xfa->width = 2*t;
	    xfa->height = 2*t;
	    xfa->angle1 = 120*64;
	    xfa->angle2 = 30*64;
	    xfa++;
	    break;
	case ELLIPTICAL: /* asteroids */
	    /* filled square */
	    XPSFillRectangle (dsp, win, gc, x-diam/2, y-diam/2, diam+1, diam+1);
	    break;
	case EARTHSAT:
	    /* open circle */
	    xda->x = x - diam/2;
	    xda->y = y - diam/2;
	    xda->width = diam+1;
	    xda->height = diam+1;
	    xda->angle1 = 0;
	    xda->angle2 = 360*64;
	    xda++;
	    break;
	default:
	    printf ("Bad type to sv_draw_obj: %d\n", op->type);
	    exit(1);
	}
}

/* called by skylist.c to get the current pointing info */
void
sv_getcenter (aamodep, fovp, altp, azp, rap, decp)
int *aamodep;
double *fovp;
double *altp;
double *azp;
double *rap;
double *decp;
{
	sv_fullwhere (mm_get_now(), sv_altdec, sv_azra, altp, azp, rap, decp);

	*aamodep = aa_mode;
	*fovp = sv_fov;
}

/* return the current circle info about svda_w */
static void
sv_getcircle (wp, hp, rp, xbp, ybp)
unsigned int *wp, *hp;		/* overall width and height */
unsigned int *rp;		/* circle radius */
unsigned int *xbp, *ybp;	/* x and y border */
{
	Display *dsp = XtDisplay(svda_w);
	Window win = XtWindow(svda_w);
	Window root;
	int x, y;
	unsigned int w, h;
	unsigned int bw, d;

	XGetGeometry (dsp, win, &root, &x, &y, wp, hp, &bw, &d);
	w = *wp/2;
	h = *hp/2;
	*rp = w > h ? h : w;	/* radius */
	*xbp = w - *rp;		/* x border */
	*ybp = h - *rp;		/* y border */
}

/* called by trails.c to create a new trail for pu.op.
 * client should still be pu.op.
 * return 0 if ok else -1.
 */
static int
sv_mktrail (ts, statep, client)
TrTS ts[];
TrState *statep;
XtPointer client;
{
	TrailObj *top;
	Now *np, now;
	int i;

	/* make sure pu.op really points to something */
	if ((Obj *)client != pu.op) {
	    xe_msg ("Object has changed -- no trail created", 0);
	    return (-1);
	}

	/* turn on watch cursor for really slow systems */
	watch_cursor (1);

	/* discard any preexisting trails for this object. */
	tobj_rmobj (pu.op);	/* silently does nothing if op not on list */

	/* allocate a new trailobj for this trail */
	top = tobj_addobj (pu.op, statep->nticks);

	/* work with a copy of Now because we modify n_mjd */
	np = mm_get_now();
	now = *np;

	/* make each entry */
	for (i = 0; i < statep->nticks; i++) {
	    TSky *tsp = &top->sky[i];
	    TrTS *tp = &ts[i];

	    /* save the timestamp info time */
	    tsp->trts = *tp;

	    /* compute circumstances */
	    tsp->o = *pu.op;
	    now.n_mjd = tp->t;
	    (void) obj_cir (&now, &tsp->o);
	}

	/* save the other setup details for this trail */
	top->trs = *statep;

	/* retain setup state as default for next time */
	trstate = *statep;

	/* redraw everything with the new trail, if we are up */
	if (XtIsManaged(svform_w))
	    sv_all (mm_get_now());

	watch_cursor (0);

	return (0);
}

/* create the main skyview form */
static void
sv_create_svform()
{
	typedef struct {
	    char *label;	/* toggle button label */
	    char *name;		/* instance name */
	    int *flagp;		/* address of flag it controls */
	    Widget *wp;		/* controlling widget, or NULL */
	} ViewOpt;
	static ViewOpt vopts[] = {
	    {"Just dots", "JustDots", &justdots},
	    {"Flip L/R",        "FlipLR",   &flip_lr, &fliplr_w},
	    {"Flip T/B",        "FlipTB",   &flip_tb, &fliptb_w},
	    {"Ecliptic",        "Ecliptic", &want_ecliptic},
	    {"Magnitude Key",   "MagKey",   &want_magscale},
	};
	static ViewOpt cnsopts[] = {
	    {"Boundaries", "CnsBoundaries",   &want_conbounds},
	    {"Figures",    "CnsFigures",      &want_configures},
	    {"Names",      "CnsNames",        &want_connames},
	};
	static ViewOpt lblopts[] = {
	    {"Names",       "LblNames",      &lbl_names, &lbl_n_w},
	    {"Magnitudes",  "LblMags",       &lbl_mags,  &lbl_m_w},
	    { NULL },
	    {"All",         "LblAll",        &lbl_all,   &lbl_a_w},
	    { br_tb_lbl,    "LblBrighttest", &lbl_br,    &lbl_b_w},
	};
	typedef struct {
	    char *label;	/* what goes on the help label */
	    char *key;		/* string to call hlp_dialog() */
	} HelpOn;
	static HelpOn helpon[] = {
	    {"Intro...",	"Sky View - intro"},
	    {"on Control...",	"Sky View - control"},
	    {"on Locate...",	"Sky View - locate"},
	    {"on View...",	"Sky View - view"},
	    {"on Fifos...",	"Sky View - fifos"},
	    {"on History...",	"Sky View - history"},
	    {"on Mouse...",	"Sky View - mouse"},
	    {"on Trails...",	"Sky View - trails"},
	    {"on Scales...",	"Sky View - scales"},
	    {"Misc...",		"Sky View - misc"},
	};
	Widget w;
	Widget mb_w, cb_w, pd_w;
	Widget ccb_w, cpd_w;
	Widget lcb_w, lpd_w;
	XmString str;
	Arg args[20];
	EventMask mask;
	int n;
	int i;

	/* create form */

	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNdefaultPosition, False); n++;
	svform_w = XmCreateFormDialog (toplevel_w, "SkyView", args, n);
	XtAddCallback (svform_w, XmNhelpCallback, sv_help_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Sky view"); n++;
	XtSetValues (XtParent(svform_w), args, n);

	/* make a menu bar across the top */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	mb_w = XmCreateMenuBar (svform_w, "MB", args, n);
	XtManageChild (mb_w);

	/* add the control pulldown */

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "CPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w); n++;
	    XtSetArg (args[n], XmNmnemonic, 'C'); n++;
	    cb_w = XmCreateCascadeButton (mb_w, "Control", args, n);
	    XtManageChild (cb_w);

	    /* create the list control */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "List", args, n);
	    XtAddCallback (w, XmNactivateCallback, sv_list_cb, NULL);
	    set_xmstring (w, XmNlabelString, "List...");
	    XtManageChild (w);

	    /* create the saveas and print controls */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "SVSave", args, n);
	    XtAddCallback (w, XmNactivateCallback, sv_print_cb, NULL);
	    set_xmstring (w, XmNlabelString, "Save...");
	    XtManageChild (w);

	    n = 0;
	    w = XmCreatePushButton (pd_w, "SVPrint", args, n);
	    XtAddCallback (w, XmNactivateCallback, sv_print_cb, (XtPointer)1);
	    set_xmstring (w, XmNlabelString, "Print...");
	    XtManageChild (w);

	    /* create the tracking tb */

	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    XtSetArg (args[n], XmNmarginHeight, 0); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNset, False); n++;	/* insure not overrode*/
	    tracktb_w = XmCreateToggleButton (pd_w, "Tracking", args, n);
	    XtAddCallback (tracktb_w, XmNvalueChangedCallback, sv_track_cb, 0);
	    XtSetSensitive (tracktb_w, False);
	    XtManageChild (tracktb_w);

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Close", args, n);
	    XtAddCallback (w, XmNactivateCallback, sv_close_cb, 0);
	    XtManageChild (w);

	/* make the "locate" cascade button and pulldown */
	sv_create_find (mb_w);

	/* make the view pulldown */

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "VPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w); n++;
	    XtSetArg (args[n], XmNmnemonic, 'V'); n++;
	    cb_w = XmCreateCascadeButton (mb_w, "View", args, n);
	    XtManageChild (cb_w);

	    /* simulate a radio box for alt/az vs ra/dec */

	    str = XmStringCreate ("Alt-Az", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    XtSetArg (args[n], XmNmarginHeight, 0); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY); n++;
	    aa_w = XmCreateToggleButton (pd_w, "AltAzMode", args, n);
	    XtAddCallback (aa_w, XmNvalueChangedCallback,sv_aa_cb,(XtPointer)1);
	    XtManageChild (aa_w);
	    XmStringFree (str);

	    str = XmStringCreate ("RA-Dec", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    XtSetArg (args[n], XmNmarginHeight, 0); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY); n++;
	    rad_w = XmCreateToggleButton(pd_w, "RADecMode", args, n);
	    XtAddCallback (rad_w,XmNvalueChangedCallback,sv_aa_cb,(XtPointer)0);
	    XtManageChild (rad_w);
	    XmStringFree (str);

	    if(XmToggleButtonGetState(aa_w)==XmToggleButtonGetState(rad_w)){
		xe_msg("Sky View display mode conflicts -- using Alt/Az",0);
		aa_mode = 1;	/* default to aa if they conflict */
		XmToggleButtonSetState (aa_w, aa_mode, False); 
		XmToggleButtonSetState (rad_w, !aa_mode, False); 
	    }
	    aa_mode = XmToggleButtonGetState(aa_w);

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    /* make the "grid" toggle button and calibration labels */

	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    XtSetArg (args[n], XmNmarginHeight, 0); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    w = XmCreateToggleButton (pd_w, "Grid", args, n);
	    XtAddCallback (w, XmNvalueChangedCallback, sv_grid_cb, 0);
	    XtManageChild (w);

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    vgrid_w = XmCreateLabel (pd_w, "SVGridV", args, n);
	    set_xmstring (vgrid_w, XmNlabelString, null_grid_str);
	    XtManageChild (vgrid_w);

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    hgrid_w = XmCreateLabel (pd_w, "SVGridH", args, n);
	    set_xmstring (hgrid_w, XmNlabelString, null_grid_str);
	    XtManageChild (hgrid_w);

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    /* make the collection of options */

	    for (i = 0; i < XtNumber(vopts); i++) {
		ViewOpt *vp = &vopts[i];

		str = XmStringCreate (vp->label, XmSTRING_DEFAULT_CHARSET);
		n = 0;
		XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
		XtSetArg (args[n], XmNmarginHeight, 0); n++;
		XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
		XtSetArg (args[n], XmNlabelString, str); n++;
		w = XmCreateToggleButton (pd_w, vp->name, args, n);
		XtAddCallback (w, XmNvalueChangedCallback, sv_option_cb,
							(XtPointer)(vp->flagp));
		XtManageChild (w);
		*(vp->flagp) = XmToggleButtonGetState (w);
		XmStringFree(str);

		if (vp->wp)
		    *vp->wp = w;
	    }

	    /* add the Constellations pulldown and pullright */

	    n = 0;
	    cpd_w = XmCreatePulldownMenu (pd_w, "CnsPD", args, n);

		n = 0;
		XtSetArg (args[n], XmNsubMenuId, cpd_w); n++;
		ccb_w = XmCreateCascadeButton (pd_w, "Constellation", args, n);
		XtManageChild (ccb_w);

	    for (i = 0; i < XtNumber(cnsopts); i++) {
		ViewOpt *vp = &cnsopts[i];

		str = XmStringCreate (vp->label, XmSTRING_DEFAULT_CHARSET);
		n = 0;
		XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
		XtSetArg (args[n], XmNmarginHeight, 0); n++;
		XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
		XtSetArg (args[n], XmNlabelString, str); n++;
		w = XmCreateToggleButton (cpd_w, vp->name, args, n);
		XtAddCallback (w, XmNvalueChangedCallback, sv_option_cb,
							(XtPointer)(vp->flagp));
		XtManageChild (w);
		*(vp->flagp) = XmToggleButtonGetState (w);
		XmStringFree(str);

		if (vp->wp)
		    *vp->wp = w;
	    }

	    /* add the Labels pulldown and pullright */

	    n = 0;
	    lpd_w = XmCreatePulldownMenu (pd_w, "LblPD", args, n);

		n = 0;
		XtSetArg (args[n], XmNsubMenuId, lpd_w); n++;
		lcb_w = XmCreateCascadeButton (pd_w, "Labels", args, n);
		XtManageChild (lcb_w);

	    for (i = 0; i < XtNumber(lblopts); i++) {
		ViewOpt *vp = &lblopts[i];

		if (vp->label == NULL) {
		    /* just make a separator */
		    n = 0;
		    w = XmCreateSeparator (lpd_w, "LSep", args, n);
		    XtManageChild (w);
		    continue;
		}

		str = XmStringCreate (vp->label, XmSTRING_DEFAULT_CHARSET);
		n = 0;
		XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
		XtSetArg (args[n], XmNmarginHeight, 0); n++;
		XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
		XtSetArg (args[n], XmNlabelString, str); n++;
		w = XmCreateToggleButton (lpd_w, vp->name, args, n);
		XtAddCallback (w, XmNvalueChangedCallback, sv_option_cb,
							(XtPointer)(vp->flagp));
		XtManageChild (w);
		*(vp->flagp) = XmToggleButtonGetState (w);
		XmStringFree(str);

		if (vp->wp)
		    *vp->wp = w;
	    }

	    if (!lbl_names && !lbl_mags) {
		xe_msg ("Neither Names nor Magnitudes labels is enabled -- defaulting to Names", 0);
		lbl_names = 1;
		XmToggleButtonSetState (lbl_n_w, True, False);
	    }

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    /* make the pb that controls the filter menu */

	    str = XmStringCreate ("Filter...", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    w = XmCreatePushButton (pd_w, "Filter", args, n);
	    XtAddCallback (w, XmNactivateCallback, sv_filter_cb, NULL);
	    XtManageChild (w);
	    XmStringFree(str);

	/* make the Fifos pulldown */

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "FPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w); n++;
	    XtSetArg (args[n], XmNmnemonic, 'F'); n++;
	    cb_w = XmCreateCascadeButton (mb_w, "Fifos", args, n);
	    XtManageChild (cb_w);

	    /* make the "Out FIFO" toggle button */

	    str = XmStringCreate ("Enable OUTFIFO", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    XtSetArg (args[n], XmNmarginHeight, 0); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    outfifo_w = XmCreateToggleButton (pd_w, "OutFIFO", args, n);
	    XtAddCallback (outfifo_w, XmNvalueChangedCallback, sv_out_fifo_cb,
									NULL);
	    XtManageChild (outfifo_w);
	    want_outfifo = XmToggleButtonGetState (outfifo_w);
	    skyout_fifo ((Now *)0);
	    XmStringFree(str);

	    /* make the "In FIFO" toggle button */

	    str = XmStringCreate ("Enable INFIFO", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    XtSetArg (args[n], XmNmarginHeight, 0); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    infifo_w = XmCreateToggleButton (pd_w, "InFIFO", args, n);
	    XtAddCallback (infifo_w, XmNvalueChangedCallback, sv_in_fifo_cb,
									NULL);
	    XtManageChild (infifo_w);
	    want_infifo = XmToggleButtonGetState (infifo_w);
	    /* TODO: react to initial state */
	    XmStringFree(str);

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    /* make the "Loc FIFO" toggle button */

	    str = XmStringCreate ("Enable LOCFIFO", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    XtSetArg (args[n], XmNmarginHeight, 0); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    locfifo_w = XmCreateToggleButton (pd_w, "LocFIFO", args, n);
	    XtAddCallback (locfifo_w, XmNvalueChangedCallback, sv_loc_fifo_cb,
									NULL);
	    XtManageChild (locfifo_w);
	    want_locfifo = XmToggleButtonGetState (locfifo_w);
	    XmStringFree(str);

	/* make a timestamp label across the bottom */

	n = 0;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	XtSetArg (args[n], XmNrecomputeSize, False); n++;
	dt_w = XmCreateLabel (svform_w, "TimeStamp", args, n);
	XtManageChild(dt_w);

	/* make the bottom scale above the time stamp */

	n = 0;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, dt_w); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	XtSetArg (args[n], XmNshowValue, True); n++;
	azra_w = XmCreateScale (svform_w, "AzRAScale", args, n);
	XtAddCallback (azra_w, XmNvalueChangedCallback, sv_scale_cb,
							    (XtPointer)AZRA_S);
	XtManageChild (azra_w);

	/* make the left scale */

	str = XmStringCreate ("FOV", XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, azra_w); n++;
	XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg (args[n], XmNminimum, 1); n++;
	XtSetArg (args[n], XmNmaximum, 1800); n++;
	XtSetArg (args[n], XmNdecimalPoints, 1); n++;
	XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_BOTTOM); n++;
	XtSetArg (args[n], XmNshowValue, True); n++;
	XtSetArg (args[n], XmNtitleString, str); n++;
	fov_w = XmCreateScale (svform_w, "FOVScale", args, n);
	XtAddCallback (fov_w, XmNvalueChangedCallback, sv_scale_cb, 
							    (XtPointer)FOV_S);
	XtManageChild (fov_w);
	XmStringFree (str);

	/* make the right scale */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, azra_w); n++;
	XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_TOP); n++;
	XtSetArg (args[n], XmNdecimalPoints, 1); n++;
	XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	XtSetArg (args[n], XmNshowValue, True); n++;
	altdec_w = XmCreateScale (svform_w, "AltDecScale", args, n);
	XtAddCallback (altdec_w, XmNvalueChangedCallback, sv_scale_cb,
							(XtPointer)ALTDEC_S);
	XtManageChild (altdec_w);

	/* make a drawing area inside the sliders for drawing the sky */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, azra_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNleftWidget, fov_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNrightWidget, altdec_w); n++;
	/* XmNdecimalPoints is set dynamically -- see sv_set_scales() */
	svda_w = XmCreateDrawingArea (svform_w, "Map", args, n);
	XtAddCallback (svda_w, XmNexposeCallback, sv_da_exp_cb, 0);
	XtAddCallback (svda_w, XmNinputCallback, sv_da_input_cb, 0);
	mask = Button1MotionMask | ButtonPressMask | ButtonReleaseMask
						   | PointerMotionHintMask,
	XtAddEventHandler (svda_w, mask, False, sv_da_motion_cb,0);
	XtManageChild (svda_w);

	/* create the filter dialog.
	 * it's not managed yet, but its state info is used right off.
	 */
	svf_create();

	/* create the popup */
	sv_create_popup();

	/* read the widget to init the real values.
	 * best to do these as a group near the end because some may rely on
	 * other widgets too.
	 */
	sv_read_scale(AZRA_S);
	sv_read_scale(ALTDEC_S);
	sv_read_scale(FOV_S);

	/* then read back the aim values to form a consistent set of widgets. */
	sv_set_aimscales();

	/* make the "history" cascade button and pulldown.
	 * must follow creating other stuff because it adds first item.
	 */
	svh_create (mb_w);

	/* make the help pulldown system.
	 * this should follow making the history pulldown so the keyboard
	 * traversal directions among the menu items remains sensible.
	 */
	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "HPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w); n++;
	    XtSetArg (args[n], XmNmnemonic, 'H'); n++;
	    cb_w = XmCreateCascadeButton (mb_w, "Help", args, n);
	    XtManageChild (cb_w);
	    set_something (mb_w, XmNmenuHelpWidget, (XtArgVal)cb_w);

	    for (i = 0; i < XtNumber(helpon); i++) {
		HelpOn *hp = &helpon[i];

		str = XmStringCreate (hp->label, XmSTRING_DEFAULT_CHARSET);
		n = 0;
		XtSetArg (args[n], XmNlabelString, str); n++;
		XtSetArg (args[n], XmNmarginHeight, 0); n++;
		w = XmCreatePushButton (pd_w, "Help", args, n);
		XtAddCallback (w, XmNactivateCallback, sv_helpon_cb,
							(XtPointer)(hp->key));
		XtManageChild (w);
		XmStringFree(str);
	    }

#if XmVersion == 1001
	/* try to connect arrow and page up/down keys to FOV initially */
	XmProcessTraversal (fov_w, XmTRAVERSE_CURRENT);
	XmProcessTraversal (fov_w, XmTRAVERSE_CURRENT); /* yes, twice!! */
#endif

#ifdef XmNinitialFocus
	/* try to connect arrow and page up/down keys to FOV initially */
	/* this approach showed up in Motif 1.2 */
	set_something (svform_w, XmNinitialFocus, (XtArgVal)fov_w);
#endif
}

/* make the "Locate" cascade button */
static void
sv_create_find (parent)
Widget parent;
{
	Arg args[20];
	Widget pd_w, cb_w;
	Widget w;
	int n;
	int i;

	/* create pulldown managed by the cascade button */

	n = 0;
	pd_w = XmCreatePulldownMenu (parent, "PointPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w); n++;
	    XtSetArg (args[n], XmNmarginHeight, 0); n++;
	    XtSetArg (args[n], XmNmnemonic, 'L'); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    cb_w = XmCreateCascadeButton (parent, "PointCB", args, n);
	    set_xmstring (cb_w, XmNlabelString, "Locate");
	    XtAddCallback (cb_w, XmNcascadingCallback, sv_finding_cb, 0);
	    XtManageChild (cb_w);

	    /* create the pushbuttons forming the cascade menu.
	     * go ahead and fill in and manage the planet names now.
	     * we do the user objects just as we are cascading.
	     */
	    for (i = 0; i < NOBJ; i++) {
		n = 0;
		w = XmCreatePushButton (pd_w, "PointPB", args, n);
		XtAddCallback (w, XmNactivateCallback, sv_find_cb,
								(XtPointer)i);
		if (i >= MERCURY && i <= MOON) {
		    Obj *op = db_basic (i);
		    set_xmstring (w, XmNlabelString, op->o_name);
		    XtManageChild (w);
		} else if (i == OBJX)
		    find_w[0] = w;
		else if (i == OBJY)
		    find_w[1] = w;
	    }
}

/* callback from the main Close button */
/* ARGSUSED */
static void
sv_close_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (svform_w);
	if ((fwasup = svf_ismanaged()))
	    svf_unmanage ();
	if (sv_pm) {
	    XFreePixmap (XtDisplay(svda_w), sv_pm);
	    sv_pm = (Pixmap) NULL;
	}
}

/* callback from the overall Help button.
 */
/* ARGSUSED */
static void
sv_help_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static char *msg[] = {
"This displays all database objects currently in memory onto the sky. The view",
"may be Alt/Az or RA/Dec. The three sliders adjust the field of fov, the",
"azimuth (or RA), and the altitude (or Dec). Objects may be filtered out by",
"type and magnitude."
};

	hlp_dialog ("Sky View", msg, sizeof(msg)/sizeof(msg[0]));
}

/* callback from a specific Help button.
 * client is a string to use with hlp_dialog().
 */
/* ARGSUSED */
static void
sv_helpon_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	hlp_dialog ((char *)client, NULL, 0);
}

/* callback when either the alt/az or ra/dec button changes state.
 * client will be 1 if the former, 0 if the latter.
 */
/* ARGSUSED */
static void
sv_aa_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
#define	POLEERR		degrad(90 - 1.0/3600.0)	/* polar ambiguous area */
	XmToggleButtonCallbackStruct *sp = (XmToggleButtonCallbackStruct *)call;
	static double last_az_polar_angle = 0.0;
	static double last_ra_polar_angle = 0.0;
	double alt, az, ra, dec;
	int which = (int)client;
	Now *np = mm_get_now();
	int wantaa;

	/* save the ambiguous polar angle if at the pole for later restore */
	if (fabs(sv_altdec) > POLEERR) {
	    if (aa_mode)
		last_az_polar_angle = sv_azra;
	    else
		last_ra_polar_angle = sv_azra;
	}

	/* prepare for current coords in opposite coord system */
	sv_fullwhere (np, sv_altdec, sv_azra, &alt, &az, &ra, &dec);

	wantaa = which == sp->set;	/* think about it :-) */

	if (wantaa) {
	    /* change from ra/dec to alt/az mode IF center is above horizon */
	    if (alt < 0.0) {
		xe_msg ("View center would go below horizon.", 1);
		XmToggleButtonSetState (aa_w, False, False);
		XmToggleButtonSetState (rad_w, True, False);
		return;
	    }
	    sv_altdec = alt;
	    sv_azra = az;
	    aa_mode = 1;
	    XmToggleButtonSetState (aa_w, True, False);
	    XmToggleButtonSetState (rad_w, False, False);
	} else {
	    /* change from alt/az to ra/dec mode */
	    sv_azra = ra;
	    sv_altdec = dec;
	    aa_mode = 0;
	    XmToggleButtonSetState (aa_w, False, False);
	    XmToggleButtonSetState (rad_w, True, False);
	}

	/* restore the ambiguous polar angle if came back */
	if (fabs(sv_altdec) > POLEERR) {
	    if (aa_mode)
		sv_azra = last_az_polar_angle;
	    else
		sv_azra = last_ra_polar_angle;
	}

	sv_set_aimscales();

	if (track_op) {
	    if (sv_mark (track_op, 1, 1, 1, 0.0) < 0)
		sv_all(np);	/* still need it even if mark failed */
	} else 
	    sv_all(np);
}

/* callback from the filter button.
 * just toggle the filter dialog.
 * always restore it to the real state before bringing it up.
 */
/* ARGSUSED */
static void
sv_filter_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (svf_ismanaged()) {
	    svf_unmanage();
	    fwasup = 0;
	} else {
	    svf_manage();
	    fwasup = 1;
	}
}

/* callback from the grid button.
 * if adding the grid just draw it, but if taking it away copy from pixmap.
 */
/* ARGSUSED */
static void
sv_grid_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	want_grid = XmToggleButtonGetState(w);

	sv_all(mm_get_now());

	if (!want_grid) {
	    set_xmstring (vgrid_w, XmNlabelString, null_grid_str);
	    set_xmstring (hgrid_w, XmNlabelString, null_grid_str);
	}
}

/* callback from the "Out FIFO" toggle button.
 */
/* ARGSUSED */
static void
sv_out_fifo_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	want_outfifo = XmToggleButtonGetState(w);
	skyout_fifo (mm_get_now());
}

/* callback from the "In FIFO" toggle button.
 */
/* ARGSUSED */
static void
sv_in_fifo_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	want_infifo = XmToggleButtonGetState(w);
	skyin_fifo ();
}

/* called whenever there is input waiting from the SKYINFIFO.
 * we gulp a lot and only use the last whole line we see that looks right.
 * N.B. do EXACTLY ONE read -- don't know that more won't block.
 */
/* ARGSUSED */
static void
skyinfifo_cb (client, fdp, idp)
XtPointer client;       /* file name */
int *fdp;               /* pointer to file descriptor */
XtInputId *idp;         /* pointer to input id */
{
	static char fmt[] = "RA:%9lf Dec:%9lf Epoch:%9lf";
	char buf[8096];
	Now *np = mm_get_now();
	char *fn = (char *)client;
	char msg[256];
	double ra, dec, y;
	char *bp, *lp;
	int gotone;
	int n;

	n = read (*fdp, buf, sizeof(buf));
	if (n < 0) {
	    (void) sprintf (msg, "Error reading from %.100s: %.100s\n", fn,
								syserrstr());
	    xe_msg (msg, 1);
	    return;
	}
	if (n == 0) {
	    /* EOF on a R/W open must mean it's really a plain file.  */
	    (void) sprintf (msg, "Closing %.200s at EOF.\n", fn);
	    xe_msg (msg, 0);
	    XmToggleButtonSetState (infifo_w, False, True); /* closes all */
	    return;
	}

	gotone = 0;
	for (bp = lp = buf; --n >= 0; )
	    if (*bp++ == '\n') {
		bp[-1] = '\0';
		if (sscanf (lp, fmt, &ra, &dec, &y) == 3) {
		    if (ra < 0 || ra >= 2*PI || dec > PI/2 || dec < -PI/2) {
			(void) sprintf (msg, "Bad %s coords: RA=%g Dec=%g\n",
								fn, ra, dec);
			xe_msg (msg, 0);
		    } else
			gotone = 1;
		}
		lp = bp;
	    }

	if (gotone) {
	    Obj obj;
	    double m;

	    obj.type = FIXED;
	    (void) strcpy (obj.o_name, "<Anon>");
	    obj.f_RA = ra;
	    obj.f_dec = dec;
	    year_mjd (y, &m);
	    obj.f_epoch = m;
	    if (obj_cir (np, &obj) == 0)
		(void) sv_mark (&obj, 1, 1, 0, 0.0);
	}
}

/* callback from the "Loc FIFO" toggle button.
 */
/* ARGSUSED */
static void
sv_loc_fifo_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	want_locfifo = XmToggleButtonGetState(w);

	if (want_locfifo) {
	    /* report if fifo is ok -- it's ok if it's not because
	     * it's retried on each attempt but we thought we'd be helpful.
	     */
	    (void) skyloc_fifo (1, 0.0, 0.0, 0.0, 0.0, 0);
	}
}

/* callback from Save and Print controls.
 * client is 0 if the former, 1 if the latter.
 */
/* ARGSUSED */
static void
sv_print_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XPSAsk ((int)client, "Sky View", sv_print);
}

/* proceed to generate a postscript file.
 * call XPSClose() when finished.
 */
static void
sv_print ()
{
	Now *np = mm_get_now();
	Window win = sv_pm;
	int fontsz = PRFONTSZ;
	unsigned wid, hei, r, xb, yb; /* wid, height, radius, x and y border */

	if (!sv_ison()) {
	    xe_msg ("Sky View must be open to save a file.", 1);
	    XPSClose();
	    return;
	}

	watch_cursor(1);

	/* get info about where we are drawing the main circle on the window */
	sv_getcircle (&wid, &hei, &r, &xb, &yb);

	/* fit view in square across the top and prepare to capture X calls*/
	XPSXBegin (win, xb, yb, 2*r, 0, 1*72, 10*72, (int)(6.5*72), fontsz);

	/* register the fonts */
	if (sv_gf)
	    XPSRegisterFont (sv_gf->fid, "Symbol");
	XPSRegisterFont (sv_pf->fid, "Helvetica");
	XPSRegisterFont (sv_sf->fid, "Helvetica");

	/* redraw */
	sv_all (np);

	/* no more X captures */
	XPSXEnd();

	/* add some extra info */
	sv_ps_annotate (np, fontsz);

	/* finished */
	XPSClose();

	watch_cursor(0);
}

static void
sv_ps_annotate (np, fs)
Now *np;
int fs;
{
	double alt, az, ra, dec;
	double fov;
	char dir[128];
	char buf[128];
	char *bp;
	int vs = fs+1;		/* nominal vertical line spacing */
	char *site;
	double tmp;
	int aamode;
	int y;

	sv_getcenter (&aamode, &fov, &alt, &az, &ra, &dec);

	/* title, of sorts */
	y = 72 + vs*13;
	(void) sprintf (dir, "(Xephem %s Sky View) 306 %d cstr\n",
		aa_mode ? "Topocentric Alt/Az" : "Geocentric RA/Dec", y);
	XPSDirect (dir);
	site = mm_getsite();
	if (site) {
	    y = 72 + vs*12;
	    (void) sprintf (dir, "(%s) 306 %d cstr\n", site, y);
	    XPSDirect (dir);
	}

	/* left column */

	y = 72 + vs*11;
	mjd_year (epoch == EOD ? mjd : epoch, &tmp);
	(void) sprintf (dir, "(Epoch:) 144 %d rstr (%g) 154 %d lstr\n",y,tmp,y);
	XPSDirect (dir);

	y = 72 + vs*10;
	fs_sexa (buf, radhr(ra), 2, 36000);
	for (bp = buf; *bp == ' '; bp++) continue;
	(void) sprintf (dir, "(RA:) 144 %d rstr (%s) 154 %d lstr\n", y, bp, y);
	XPSDirect (dir);

	y = 72 + vs*9;
	fs_sexa (buf, raddeg(dec), 3, 3600);
	for (bp = buf; *bp == ' '; bp++) continue;
	(void) sprintf (dir, "(Dec:) 144 %d rstr (%s) 154 %d lstr\n", y, bp, y);
	XPSDirect (dir);

	y = 72 + vs*8;
	fs_sexa (buf, raddeg(alt), 3, 3600);
	for (bp = buf; *bp == ' '; bp++) continue;
	(void) sprintf (dir, "(Alt:) 144 %d rstr (%s) 154 %d lstr\n", y, bp, y);
	XPSDirect (dir);

	y = 72 + vs*7;
	fs_sexa (buf, raddeg(az), 4, 3600);
	for (bp = buf; *bp == ' '; bp++) continue;
	(void) sprintf (dir, "(Az:) 144 %d rstr (%s) 154 %d lstr\n", y, bp, y);
	XPSDirect (dir);

	y = 72 + vs*6;
	(void) sprintf (dir, "(FOV:) 144 %d rstr (%.1f) 154 %d lstr\n",
							    y, raddeg(fov), y);
	XPSDirect (dir);

	/* right column */

	y = 72 + vs*11;
	(void) sprintf (dir,"(JD:) 410 %d rstr (%13.5f) 420 %d lstr\n", y,
								mjd+MJD0, y);
	XPSDirect (dir);

	y = 72 + vs*10;
	fs_date (buf, mjd_day(mjd));
	for (bp = buf; *bp == ' '; bp++) continue;
	(void) sprintf (dir, "(UT Date:) 410 %d rstr (%s) 420 %d lstr\n",
								    y, bp,y);
	XPSDirect (dir);

	y = 72 + vs*9;
	fs_time (buf, mjd_hr(mjd));
	for (bp = buf; *bp == ' '; bp++) continue;
	(void) sprintf (dir, "(UT Time:) 410 %d rstr (%s) 420 %d lstr\n",
								    y, bp,y);
	XPSDirect (dir);

	y = 72 + vs*8;
	now_lst (np, &tmp);
	fs_time (buf, tmp);
	for (bp = buf; *bp == ' '; bp++) continue;
	(void) sprintf (dir, "(LST:) 410 %d rstr (%s) 420 %d lstr\n", y, bp, y);
	XPSDirect (dir);

	y = 72 + vs*7;
	fs_sexa (buf, raddeg(fabs(lat)), 3, 3600);
	for (bp = buf; *bp == ' '; bp++) continue;
	(void) sprintf (dir, "(Lat:) 410 %d rstr (%s %c) 420 %d lstr\n", y, bp,
						    lat < 0 ? 'S' : 'N', y);
	XPSDirect (dir);

	y = 72 + vs*6;
	fs_sexa (buf, raddeg(fabs(lng)), 4, 3600);
	for (bp = buf; *bp == ' '; bp++) continue;
	(void) sprintf (dir,"(Long:) 410 %d rstr (%s %c) 420 %d lstr\n", y, bp,
							lng < 0 ? 'W' : 'E', y);
	XPSDirect (dir);
}

/* callback when the Control->tracking tb is toggled.
 * N.B. also triggered to implement turning tracking off from the popup too.
 */
/* ARGSUSED */
static void
sv_track_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (!XmToggleButtonGetState (w)) {
	    track_op = (Obj *)0;
	    XtSetSensitive (w, False);
	    sv_all (mm_get_now());	/* erase the mark */
	}
}

/* set altdec_w/azra_w scales from aa_mode/sv_altdec/sv_azra.
 */
static void
sv_set_aimscales()
{
	Arg args[20];
	int n;

	if (aa_mode) {
	    int altitude, azimuth;

	    altitude = raddeg(sv_altdec)*10.0 + 0.5;
	    azimuth = raddeg(sv_azra)*10.0 + 0.5;
	    if (azimuth >= 3600) azimuth -= 3600;
	    if (azimuth < 0) azimuth += 3600;

	    n = 0;
	    XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	    XtSetArg (args[n], XmNmaximum, 3599); n++;
	    XtSetArg (args[n], XmNminimum, 0); n++;
	    XtSetArg (args[n], XmNdecimalPoints, 1); n++;
	    XtSetArg (args[n], XmNvalue, azimuth); n++;
	    XtSetValues (azra_w, args, n);

	    n = 0;
	    XtSetArg (args[n], XmNmaximum, 900); n++;
	    XtSetArg (args[n], XmNminimum, 0); n++;
	    XtSetArg (args[n], XmNvalue, altitude); n++;
	    XtSetValues (altdec_w, args, n);

	} else {
	    int ra, dec;

	    ra = radhr(sv_azra)*100.0 + 0.5;
	    if (ra >= 2400) ra -= 2400;
	    dec = raddeg(sv_altdec)*10.0 + (sv_altdec >= 0 ? 0.5 : -0.5);

	    n = 0;
	    XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_LEFT); n++;
	    XtSetArg (args[n], XmNmaximum, 2399); n++;
	    XtSetArg (args[n], XmNminimum, 0); n++;
	    XtSetArg (args[n], XmNdecimalPoints, 2); n++;
	    XtSetArg (args[n], XmNvalue, ra); n++;
	    XtSetValues (azra_w, args, n);

	    n = 0;
	    XtSetArg (args[n], XmNmaximum, 900); n++;
	    XtSetArg (args[n], XmNminimum, -900); n++;
	    XtSetArg (args[n], XmNvalue, dec); n++;
	    XtSetValues (altdec_w, args, n);
	}


}

/* set fov_w to match sv_fov.
 * N.B. we also round sv_fov to match display precision IN-PLACE.
 */
static void
sv_set_fovscale()
{
	int fov;

	/* scale runs from 1..1800 to appear to go down to .1 degree */
	fov = (int)(raddeg(sv_fov)*10+0.5);
	if (fov < 1)
	    fov = 1;
	if (fov > 1800)
	    fov = 1800;
	XmScaleSetValue (fov_w, fov);

	sv_fov = degrad(fov/10.0);
}

/* expose event of sky view drawing area.
 * if same size just copy from pixmap, else recompute all (it's resized).
 * N.B. we set bit_gravity to ForgetGravity so we can just use Expose events.
 * N.B. we turn off backing store since we effectively do it in the pixmap.
 */
/* ARGSUSED */
static void
sv_da_exp_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static unsigned int wid_last, hei_last;
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;
	Display *dsp = XtDisplay(svda_w);
	Window win = XtWindow(svda_w);
	Window root;
	int x, y;
	unsigned int bw, d;
	unsigned int wid, hei;

	switch (c->reason) {
	case XmCR_EXPOSE: {
	    static before;
	    XExposeEvent *e = &c->event->xexpose;

	    if (!before) {
		XSetWindowAttributes swa;
		unsigned long mask = CWBitGravity | CWBackingStore;

		swa.bit_gravity = ForgetGravity;
		swa.backing_store = NotUseful;
		XChangeWindowAttributes (e->display, e->window, mask, &swa);
		before = 1;
	    }

	    /* wait for the last in the series */
	    if (e->count != 0)
		return;
	    break;
	    }
	default:
	    printf ("Unexpected svda_w event. type=%d\n", c->reason);
	    exit(1);
	}

	XGetGeometry (dsp, win, &root, &x, &y, &wid, &hei, &bw, &d);

	if (!sv_pm || wid != wid_last || hei != hei_last) {
	    if (sv_pm) {
		XFreePixmap (dsp, sv_pm);
		sv_pm = (Pixmap) NULL;
	    }

	    if (!sv_gc)
		sv_mk_gcs(dsp, win);

	    sv_pm = XCreatePixmap (dsp, win, wid, hei, d);
	    XSetForeground (dsp, sv_gc, bg_p);
	    XFillRectangle (dsp, sv_pm, sv_gc, 0, 0, wid, hei);
	    wid_last = wid;
	    hei_last = hei;

	    sv_all(mm_get_now());
	} else {
	    /* same size; just copy from the pixmap */
	    sv_copy_sky();
	}
}

/* draw the magnitude scale */
static void
sv_magscale (dsp, win, r, xb, yb)
Display *dsp;
Window win;
unsigned r, xb, yb;
{
	int cw, ch;		/* w and h of typical digit */
	int x0, y0;		/* ul corner of key box */
	int boxw, boxh;		/* overall key box size */
	int fmag, bmag;
	char buf[16];
	int dir, asc, dsc;
	XCharStruct all;
	GC gc;
	int n;
	Obj o;
	int i;

	/* get size of typical digit -- none have descenders */
	buf[0] = '2';	/* a typical digit :-) */
	XTextExtents (sv_pf, buf, 1, &dir, &asc, &dsc, &all);
	cw = all.rbearing;
	ch = asc;

	/* establish box location and size.
	 * this assumes box will be about 3:1 and vertical along ll corner.
	 * also want 3/2 vertical char spacing per symbol.
	 * don't do it if not big enough.
	 */
	boxh = 9*r/20;
	n = (2*boxh - ch)/(3*ch);
	if (n < 1)
	    return;
	boxh = ch*(3*n+1)/2;
	svf_getmaglimits (&fmag, &bmag);
	boxw = (fmag > 9 || fmag - (n-1) < 0) ? cw*5 : 7*cw/2;
	if (want_grid) {
	    x0 = (int)xb - boxw;
	    if (x0 < 0)
		x0 = 0;
	} else
	    x0 = xb;
	y0 = yb + 2*r - boxh;

	/* draw the key patch */
	XSetForeground (dsp, sv_gc, sky_p);
	XFillRectangle (dsp, win, sv_gc, x0, y0, boxw, boxh);
	XSetForeground (dsp, sv_gc, fg_p);

	/* ok, draw the scale */
	o.type = FIXED;
	o.f_class = 'S';
	o.f_spect[0] = 'G';
	o.s_size = 0;
	obj_pickgc (&o, svda_w, &gc);
	for (i = 0; i < n; i++) {
	    int d, y;

	    o.s_mag = floor((fmag - i)*MAGSCALE + 0.5);
	    d = objdiam(&o);
	    y = y0 + ch + 3*i*ch/2;
	    sv_draw_obj (dsp, win, gc, &o, x0 + cw, y, d, 1);
	    (void) sprintf (buf, "%g", o.s_mag/MAGSCALE);
	    XPSDrawString (dsp, win, gc, x0 + 2*cw, y + ch/2, buf, strlen(buf));
	}
	sv_draw_obj (dsp, win, (GC)0, NULL, 0, 0, 0, 0);	/* flush */
}

/* copy the pixmap to the drawing area
 */
static void
sv_copy_sky()
{
	Display *dsp = XtDisplay (svda_w);
	Window win = XtWindow (svda_w);
	unsigned wid, hei;	/* overall width and height */
	unsigned int r;		/* circle radius */
	unsigned int xb, yb;	/* x and y border */

	sv_getcircle (&wid, &hei, &r, &xb, &yb);

	XCopyArea (dsp, sv_pm, win, sv_gc, 0, 0, wid, hei, 0, 0);
}

/* called on receipt of a MotionNotify while Button1 is down or any
 * ButtonPress/ButonRelease while the cursor is over the svda_w window.
 */
/* ARGSUSED */
static void
sv_da_motion_cb (w, client, ev, continue_to_dispatch)
Widget w;
XtPointer client;
XEvent *ev;
Boolean *continue_to_dispatch;
{
	static dirty;
	unsigned int wide, h, r, xb, yb;
	double altdec, azra;
	Window root, child;
	int rx, ry, wx, wy;
	unsigned mask;
	int evt = ev->type;
	int m1, b1p, b1r;
	int inside;

	/* what happened? */
	m1  = evt == MotionNotify  && ev->xmotion.state  == Button1Mask;
	b1p = evt == ButtonPress   && ev->xbutton.button == Button1;
	b1r = evt == ButtonRelease && ev->xbutton.button == Button1;

	/* do we care? */
	if (!m1 && !b1p && !b1r)
	    return;

	/* where are we? */
	XQueryPointer (XtDisplay(w), XtWindow(w),
			    &root, &child, &rx, &ry, &wx, &wy, &mask);
	sv_getcircle (&wide, &h, &r, &xb, &yb);
	inside = sv_unloc (r, wx-xb, wy-yb, &altdec, &azra);

	if (inside && (m1 || b1p)) {
	    /* display the tracking stuff in the corners */
	    static double start_altdec, start_azra;

	    /* if just pressed b1, save loc for angle calc */
	    if (b1p) {
		start_altdec = altdec;
		start_azra = azra;
	    }

	    sv_draw_track_coords (wide, h, altdec, azra,
						start_altdec, start_azra);
	    dirty = 1;
	}
	
	if (dirty && (b1r || !inside)) {
	    /* release: clean off the tracking stuff */
	    sv_copy_sky();
	    dirty = 0;
	}
}

/* a dot has been picked:
 * find what it is and report stuff about it in a popup.
 */
/* ARGSUSED */
static void
sv_da_input_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
#define	PICKRANGE	10	/* dist allowed from pointer */
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;
	double csep, maxcsep;	/* cos(separation) and max so far */
	double altdec, azra;	/* cursor location */
	double saltdec, caltdec;/* sin/cos altdec */
	double ad, ar;		/* candidate location */
	TSky *mintsp = NULL;	/* set to a TSky if find one close */
	Obj *minop = NULL;	/* set to an Obj if find one closer */
	int wx, wy;		/* window coords of cursor */
	unsigned int wide, h, r, xb, yb;
	DBScan dbs;
	XEvent *ev;
	TrailObj *top;
	int x, y;
	Obj *op;
	int i;

	/* confirm button 3 press event */
	if (c->reason != XmCR_INPUT)
	    return;
	ev = c->event;
	if (ev->xany.type != ButtonPress || ev->xbutton.button != 3)
	    return;

	/* find world coord of cursor -- don't proceed if outside the circle */
	wx = ev->xbutton.x;
	wy = ev->xbutton.y;
	sv_getcircle (&wide, &h, &r, &xb, &yb);
	if (!sv_unloc (r, wx-xb, wy-yb, &altdec, &azra))
	    return;

	/* ok, own up to some work in progress */
	watch_cursor(1);

	/* find object with largest cos from current pos */
	caltdec = cos(altdec);
	saltdec = sin(altdec);
	maxcsep = 0.0;

	/* search the trailed stuff first */
	for (top = trailobj; top; top = top->ntop) {
	    if (!top->on)
		continue;
	    for (i = 0; i < top->nsky; i++) {
		op = &top->sky[i].o;
		ad = aa_mode ? op->s_alt : op->s_dec;
		ar = aa_mode ? op->s_az  : op->s_ra;
		csep = saltdec*sin(ad) + caltdec*cos(ad)*cos(azra-ar);
		if (csep > maxcsep) {
		    mintsp = &top->sky[i];
		    maxcsep = csep;
		}
	    }
	}

	/* search the database too -- might be something closer */
	for (db_scaninit(&dbs); (op = db_scan (&dbs)) != NULL; ) {
	    if (!(op->o_flags & OBJF_ONSCREEN))
		continue;
	    ad = aa_mode ? op->s_alt : op->s_dec;
	    ar = aa_mode ? op->s_az  : op->s_ra;
	    csep = saltdec*sin(ad) + caltdec*cos(ad)*cos(azra-ar);
	    if (csep > maxcsep) {
		minop = op;
		maxcsep = csep;
	    }
	}

	/* find screen loc of closest object, if any.
	 * if its x/y coords are on screen show some kind of popup.
	 */
	if (minop || mintsp) {
	    op = minop ? minop : &mintsp->o;
	    if (sv_loc (r, aa_mode ? op->s_alt : op->s_dec,
				    aa_mode ? op->s_az : op->s_ra, &x, &y)) {
		x += xb;
		y += yb;

		/* show info about object if it is close, else generic popup */
		if (abs(x-wx) <= PICKRANGE && abs(y-wy) <= PICKRANGE)
		    sv_popup (ev, minop, minop ? NULL : mintsp);
		else
		    sv_popup (ev, NULL, NULL);
	    }
	} else
	    sv_popup (ev, NULL, NULL);

	watch_cursor(0);
}

/* callback when any of the scales change value.
 * client is a SCALES enum to tell us which.
 */
/* ARGSUSED */
static void
sv_scale_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int which = (int) client;

	/* read the widget to set the real value */
	sv_read_scale (which);

	/* update the scene */
	sv_all(mm_get_now());
}

/* callback when any of the options toggle buttons that just causes a
 * redraw changes state.
 * client points to a state variable that should first be set to the new state.
 */
/* ARGSUSED */
static void
sv_option_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int *p = (int *)client;

	*p = XmToggleButtonGetState (w);

	/* special consideration for the labeling options to make the all/br
	 * pair work rather like a radio box (tho we let both be off).
	 * then also insure at least one of names and mags is on so 
	 * "Persistent label" alway shows something.
	 */
	if (p == &lbl_all && lbl_all) {
	    lbl_br = 0;
	    XmToggleButtonSetState (lbl_b_w, False, False);
	}
	if (p == &lbl_br && lbl_br) {
	    lbl_all = 0;
	    XmToggleButtonSetState (lbl_a_w, False, False);
	}
	if (!lbl_names && !lbl_mags) {
	    lbl_names = 1;
	    XmToggleButtonSetState (lbl_n_w, True, False);
	}

	sv_all(mm_get_now());
}

/* callback when the "Find" cascade button is activated.
 * check the used defined objects and adjust the cascade buttons accordingly.
 */
/* ARGSUSED */
static void
sv_finding_cb (wid, client, call)
Widget wid;
XtPointer client;
XtPointer call;
{
	int i;

	for (i = 0; i < 2; i++) {
	    Obj *op = db_basic (i == 0 ? OBJX : OBJY);
	    Widget w = find_w[i];
	    if (op->type == UNDEFOBJ)
		XtUnmanageChild (w);
	    else {
		set_xmstring (w, XmNlabelString, op->o_name);
		XtManageChild (w);
	    }
	}
}

/* callback when one of the "locate" cascade buttons changes state.
 * object dbidx is in client.
 */
/* ARGSUSED */
static void
sv_find_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	Obj *op = db_basic((int)client);

	(void) sv_mark (op, 0, 1, 1, 0.0);
}

/* given one of the SCALES enums, read it and compute the corresponding 
 * real value.
 */
static void
sv_read_scale (which)
int which;
{
	int i;

	switch (which) {
	case FOV_S:
	    XmScaleGetValue (fov_w, &i);
	    sv_fov = degrad(i/10.0);
	    break;
	case ALTDEC_S:
	    XmScaleGetValue (altdec_w, &i);
	    sv_altdec = degrad(i/10.0);
	    break;
	case AZRA_S:
	    XmScaleGetValue (azra_w, &i);
	    sv_azra = aa_mode ? degrad(i/10.0) : hrrad(i/100.0);
	    break;
	default:
	    printf ("sv_read_scale: bad which: %d\n", which);
	    exit (1);
	}
}

/* draw all the stuff in the coord tracking areas in the corners of the drawing
 *   area.
 * N.B. draw directly to the window, *NOT* the pixmap, so it can be erased by
 *   just copying the pixmap to the window again.
 */
static void
sv_draw_track_coords (w, h, altdec, azra, start_altdec, start_azra)
unsigned int w, h;
double altdec, azra;
double start_altdec, start_azra;
{
#define	V_GAP		4	/* vertical gap in rows, pixels */
	static last_cns_w;
	Now *np = mm_get_now();
	Window win = XtWindow(svda_w);
	Display *dsp = XtDisplay(svda_w);
	double alt, az, ra, dec;
	char alt_buf[64], az_buf[64];
	char ra_buf[64], dec_buf[64];
	char angle_buf[64];
	char *constellation;
	XCharStruct all;
	int len;
	double ca;
	int dir, asc, des;
	Pixel fg, bg;
	unsigned long gcm;
	XGCValues gcv;

	sv_fullwhere (np, altdec, azra, &alt, &az, &ra, &dec);

	(void) strcpy (alt_buf, "Alt: ");
	fs_pangle (alt_buf+5, alt);
	(void) strcpy (az_buf, "Az: ");
	fs_pangle (az_buf+4, az);
	(void) strcpy (ra_buf, "RA: ");
	fs_ra (ra_buf+4, ra);
	(void) strcpy (dec_buf, "Dec: ");
	fs_pangle (dec_buf+5, dec);

	constellation = cns_name(cns_pick (ra, dec, epoch==EOD ? mjd : epoch));
	constellation += 5;	/* skip the abbreviation */

	/* draw Alt in the upper left */
	len = strlen (alt_buf);
	XTextExtents (sv_sf, alt_buf, len, &dir, &asc, &des, &all);
	XDrawImageString (dsp, win, sv_strgc, 0, asc, alt_buf, len); 

	/* draw the constellation name in upper left below Alt.
	 * clear out previous in case it was longer.
	 */
	len = strlen (constellation);
	XTextExtents (sv_sf, constellation, len, &dir, &asc, &des, &all);
	gcm = GCForeground | GCBackground;
	XGetGCValues (dsp, sv_strgc, gcm, &gcv);
	fg = gcv.foreground;
	bg = gcv.background;
	XSetForeground (dsp, sv_strgc, bg);
	XFillRectangle (dsp, win, sv_strgc, 0, asc+des+V_GAP, last_cns_w,
								    asc+des);
	XSetForeground (dsp, sv_strgc, fg);
	XDrawImageString (dsp, win, sv_strgc, 0, 2*asc+des+V_GAP,
							constellation, len); 
	last_cns_w = all.rbearing;

	/* draw Az in upper right */
	len = strlen (az_buf);
	XTextExtents (sv_sf, az_buf, len, &dir, &asc, &des, &all);
	XDrawImageString (dsp, win, sv_strgc, w-all.rbearing, asc, az_buf, len);

	/* draw RA in lower left */
	len = strlen (ra_buf);
	XTextExtents (sv_sf, ra_buf, len, &dir, &asc, &des, &all);
	XDrawImageString (dsp, win, sv_strgc, 0, h-des, ra_buf, len); 

	/* draw Dec in lower right */
	len = strlen (dec_buf);
	XTextExtents (sv_sf, dec_buf, len, &dir, &asc, &des, &all);
	XDrawImageString (dsp, win, sv_strgc, w-all.rbearing, h-des, dec_buf,
									len);

	/* find and draw under Az the angle from start_altdec/azra to
	 * current loc
	 */
	solve_sphere (azra - start_azra, PI/2-start_altdec, sin(altdec),
						cos(altdec), &ca, NULL);
	(void) strcpy (angle_buf, "Sep: ");
	fs_pangle (angle_buf+5, acos(ca));
	len = strlen (angle_buf);
	XTextExtents (sv_sf, angle_buf, len, &dir, &asc, &des, &all);
	XDrawImageString (dsp, win, sv_strgc, w-all.rbearing, 2*asc+des+V_GAP,
								angle_buf, len);
}

/* mark the given object, possibly remaining depending on the center flags.
 * then if newfov is not 0, change to new fov, silently enforcing limits.
 * return 0 if did all that was asked else generate a message using xe_msg(*,1)
 *   and return -1.
 * N.B. we do *not* update the s_ fields of op.
 */
static int
sv_mark (op, in_center, out_center, mark, newfov)
Obj *op;
int in_center;	/* whether to center an object already within the fov */
int out_center;	/* whether to center an object not already within the fov */
int mark;	/* whether to mark the object if it ends up visible */
double newfov;	/* new fov if other conditions met. leave unchanged if == 0 */
{
	char msg[128];
	unsigned int wide, h, r, xb, yb;
	double altdec, azra;
	int infov;
	int x, y;

	/* never show something below the horizon if we're in alt/az mode */
	if (aa_mode && op->s_alt < 0.0) {
	    (void) sprintf (msg, "`%.100s' is below the horizon now.\nPlease switch to RA-Dec display mode.",
								    op->o_name);
	    xe_msg (msg, 1);
	    return (-1);
	}

	/* see if it's within the current fov.
	 * complain if it's not and we have no permission to reaim.
	 */
	sv_getcircle (&wide, &h, &r, &xb, &yb);
	altdec = aa_mode ? op->s_alt : op->s_dec;
	azra   = aa_mode ? op->s_az  : op->s_ra;
	infov = sv_loc (r, altdec, azra, &x, &y);
	if (!infov && !out_center) {
	    (void) sprintf (msg, "`%.100s' is outside the field of view.\n",
								    op->o_name);
	    xe_msg (msg, 1);
	    return (-1);
	}

	/* center the thing if we should */
	if ((infov && in_center) || (!infov && out_center)) {
	    if (newfov != 0.0) {
		sv_fov = newfov;
		sv_set_fovscale();
	    }
	    sv_altdec = altdec;
	    sv_azra = azra;
	    sv_set_aimscales();
	    sv_all(mm_get_now());

	    /* get window loc at new view */
	    if (sv_loc (r, altdec, azra, &x, &y) < 0) {
		printf ("sv_mark: object disappeared from view?!\n");
		exit (1);
	    } 
	}

	/* and mark if asked to.
	 * by now we are certainly visible.
	 */
	if (mark) {
	    Display *dsp = XtDisplay (svda_w);
	    Window win = XtWindow (svda_w);

	    x += xb;
	    y += yb;

	    XSetForeground (dsp, sv_gc, fg_p);
	    XDrawLine (dsp, win, sv_gc, x-MARKR, y-MARKR, x+MARKR, y+MARKR);
	    XDrawLine (dsp, win, sv_gc, x+MARKR, y-MARKR, x-MARKR, y+MARKR);
	    XDrawArc (dsp, win, sv_gc, x-MARKR, y-MARKR, 2*MARKR, 2*MARKR,
								    0, 64*360);
	}

	return (0);
}

/* we are called when the right mouse button is pressed because the user wants
 * to identify and possibly control an object in some way. we fill in the popup
 * with goodies and manage it.
 * three cases:
 *   1) we are called with op set and tsp == 0. this means we were called with
 *      a real db object so just use *op;
 *   2) we are called with op == 0 and tsp set. this means we were called with
 *      a trailed object and so use *tsp->op;
 *   3) we are called with neither op nor tsp set. this means we were called
 *      far from any object; compute the location and display it. also set
 *      pu.op to a static object with at least ra/dec/alt/az value for possible
 *      pointing.
 * position the popup as indicated by ev and display it.
 * it goes down by itself.
 */
static void
sv_popup (ev, op, tsp)
XEvent *ev;
Obj *op;
TSky *tsp;
{
	char buf[32], buf2[64];
	int noobj;
	int label = 0;
	int hastrail = 0, trailon = 0;
	int track = 0;
	double jd;

	if (tsp) {
	    /* we were given a trailed Obj.
	     * pu.op is the top object, pu.tsp is tsp.
	     */
	    TrailObj *top;

	    op = &tsp->o;
	    top = tobj_find (op);
	    jd = tsp->trts.t;
	    label = !!(tsp->flags & OBJF_LABEL);
	    track = top->op == track_op;
	    hastrail = 1;
	    trailon = 1;
	    pu.op = top->op;
	    pu.tsp = tsp;
	    noobj = 0;
	} else if (op) {
	    /* not called with tsp trail -- just op.
	     * pu.op is op, pu.tsp is NULL.
	     */
	    Now *np = mm_get_now();
	    TrailObj *top = tobj_find(op);

	    jd = mjd;
	    label = !!(op->o_flags & OBJF_LABEL);
	    noobj = 0;
	    track = op == track_op;
	    hastrail = top ? 1 : 0;
	    trailon = hastrail && top->on;
	    pu.op = op;
	    pu.tsp = NULL;
	} else {
	    /* nothing -- compute from ev and use svobj
	     * pu.op and op are &svobj, pu.tsp is NULL.
	     */
	    Now *np = mm_get_now();
	    double altdec, azra, alt, az, ra, dec;
	    unsigned int wide, h, r, xb, yb;
	    int wx, wy;

	    wx = ev->xbutton.x;
	    wy = ev->xbutton.y;
	    sv_getcircle (&wide, &h, &r, &xb, &yb);
	    if (!sv_unloc (r, wx-xb, wy-yb, &altdec, &azra))
		return;	/* outside the circle */
	    sv_fullwhere (np, altdec, azra, &alt, &az, &ra, &dec);
	    jd = mjd;
	    svobj.s_ra = ra;
	    svobj.s_dec = dec;
	    svobj.s_az = az;
	    svobj.s_alt = alt;
	    svobj.type = FIXED;
	    pu.op = op = &svobj;
	    pu.tsp = NULL;
	    noobj = 1;
	}

	if (noobj) {
	    XtUnmanageChild (pu.name_w);
	} else {
	    XtManageChild (pu.name_w);
	    (void) sprintf (buf2, "Name: %.20s", op->o_name);
	    set_xmstring (pu.name_w, XmNlabelString, buf2);
	}

	if (noobj) {
	    XtUnmanageChild (pu.desc_w);
	} else {
	    XtManageChild (pu.desc_w);
	    (void) sprintf (buf2, "Type: %.20s", obj_description(op));
	    set_xmstring (pu.desc_w, XmNlabelString, buf2);
	}

	if (!noobj && op->type == FIXED && op->f_spect[0]) {
	    (void) sprintf (buf2, "Spect: %.*s", (int)sizeof(op->f_spect),
								op->f_spect);
	    set_xmstring (pu.spect_w, XmNlabelString, buf2);
	    XtManageChild (pu.spect_w);
	} else
	    XtUnmanageChild (pu.spect_w);

	if (!noobj && op->s_size > 0) {
	    if (op->s_size >= 60)
		(void) sprintf (buf2, "Size: %.1f'", (int)op->s_size/60.0);
	    else
		(void) sprintf (buf2, "Size: %d\"", op->s_size);
	    set_xmstring (pu.size_w, XmNlabelString, buf2);
	    XtManageChild (pu.size_w);
	} else
	    XtUnmanageChild (pu.size_w);

	if (noobj) {
	    XtUnmanageChild (pu.ud_w);
	} else {
	    XtManageChild (pu.ud_w);
	    fs_date (buf, mjd_day(jd));
	    (void) sprintf (buf2, "UT Date: %s", buf);
	    set_xmstring (pu.ud_w, XmNlabelString, buf2);
	}

	if (noobj) {
	    XtUnmanageChild (pu.ut_w);
	} else {
	    XtManageChild (pu.ut_w);
	    fs_time (buf, mjd_hr(jd));
	    (void) sprintf (buf2, "UT Time: %s", buf);
	    set_xmstring (pu.ut_w, XmNlabelString, buf2);
	}

	fs_ra (buf, op->s_ra);
	(void) sprintf (buf2, "RA: %s", buf);
	set_xmstring (pu.ra_w, XmNlabelString, buf2);

	fs_pangle (buf, op->s_dec);
	(void) sprintf (buf2, "Dec: %s", buf);
	set_xmstring (pu.dec_w, XmNlabelString, buf2);

	fs_pangle (buf, op->s_alt);
	(void) sprintf (buf2, "Alt: %s", buf);
	set_xmstring (pu.alt_w, XmNlabelString, buf2);

	fs_pangle (buf, op->s_az);
	(void) sprintf (buf2, "Az: %s", buf);
	set_xmstring (pu.az_w, XmNlabelString, buf2);

	if (noobj) {
	    XtUnmanageChild (pu.mag_w);
	} else {
	    XtManageChild (pu.mag_w);
	    (void) sprintf (buf2, "Mag: %.3g", op->s_mag/MAGSCALE);
	    set_xmstring (pu.mag_w, XmNlabelString, buf2);
	}

	if (want_locfifo)
	    XtManageChild (pu.locfifo_w);
	else
	    XtUnmanageChild (pu.locfifo_w);

	if (noobj) {
	    XtUnmanageChild (pu.label_w);
	    XtUnmanageChild (pu.make_w);
	    XtUnmanageChild (pu.track_w);
	    XtUnmanageChild (pu.trail_w);
	    XtUnmanageChild (pu.newtrail_w);
	} else {
	    XtManageChild (pu.label_w);
	    XmToggleButtonSetState (pu.label_w, label, False);
	    XtManageChild (pu.make_w);
	    XtManageChild (pu.track_w);
	    XmToggleButtonSetState (pu.track_w, track, False);
	    if (hastrail) {
		XtManageChild (pu.trail_w);
		XmToggleButtonSetState (pu.trail_w, trailon, False);
		set_xmstring (pu.newtrail_w, XmNlabelString, "Change Trail...");
	    } else {
		XtUnmanageChild (pu.trail_w);
		set_xmstring (pu.newtrail_w, XmNlabelString, "Create Trail...");
	    }
	    XtManageChild (pu.newtrail_w);
	}

	XmMenuPosition (pu.pu_w, (XButtonPressedEvent *)ev);
	XtManageChild (pu.pu_w);
}

/* create the id popup */
static void
sv_create_popup()
{
	static Widget *puw[] = {
	    &pu.name_w,
	    &pu.desc_w,
	    &pu.ud_w,
	    &pu.ut_w,
	    &pu.ra_w,
	    &pu.dec_w,
	    &pu.alt_w,
	    &pu.az_w,
	    &pu.spect_w,
	    &pu.mag_w,
	    &pu.size_w,
	};
	Arg args[20];
	XmString str;
	Widget w;
	int i, n;

	/* create the outer form */
	n = 0;
	XtSetArg (args[n], XmNisAligned, True); n++;
	XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
	pu.pu_w = XmCreatePopupMenu (svda_w, "SVPopup", args, n);

	/* create the label widgets */

	for (i = 0; i < XtNumber(puw); i++) {
	    n = 0;
	    *puw[i] = XmCreateLabel (pu.pu_w, "SVPopValueL", args, n);
	    XtManageChild (*puw[i]);
	}

	/* add a nice separator */
	n = 0;
	w = XmCreateSeparator (pu.pu_w, "SVSep", args, n);
	XtManageChild(w);

	/* make the command buttons */

	str = XmStringCreateLtoR ("Point", XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg (args[n], XmNlabelString, str); n++;
	w = XmCreatePushButton (pu.pu_w, "SVPopPoint", args, n);
	XtAddCallback (w, XmNactivateCallback, sv_pu_activate_cb,
							    (XtPointer)AIM);
	XtManageChild (w);
	XmStringFree (str);

	/* make the zoom cascade */

	sv_create_zoomcascade (pu.pu_w);

	/* add a nice separator */
	n = 0;
	w = XmCreateSeparator (pu.pu_w, "SVSep", args, n);
	XtManageChild(w);

	str = XmStringCreateLtoR ("Make ObjX/Y", XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg (args[n], XmNlabelString, str); n++;
	pu.make_w = XmCreatePushButton (pu.pu_w, "SVPopMakeObj", args, n);
	XtAddCallback (pu.make_w, XmNactivateCallback, sv_pu_activate_cb,
							(XtPointer)MK_OBJX);
	XtManageChild (pu.make_w);
	XmStringFree(str);

	str = XmStringCreateLtoR ("Send to LOCFIFO", XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg (args[n], XmNlabelString, str); n++;
	pu.locfifo_w = XmCreatePushButton (pu.pu_w, "SVPopLocFifo", args, n);
	XtAddCallback (pu.locfifo_w, XmNactivateCallback, sv_pu_activate_cb,
							    (XtPointer)LOCFIFO);
	XmStringFree (str);

	/* add a nice separator */
	n = 0;
	w = XmCreateSeparator (pu.pu_w, "SVSep", args, n);
	XtManageChild(w);

	str = XmStringCreateLtoR ("Track", XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg (args[n], XmNlabelString, str); n++;
	XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	pu.track_w = XmCreateToggleButton (pu.pu_w, "SVPopTrack", args, n);
	XtAddCallback (pu.track_w, XmNvalueChangedCallback, sv_pu_track_cb, 0);
	XtManageChild (pu.track_w);
	XmStringFree (str);

	str = XmStringCreateLtoR ("Persistent label", XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg (args[n], XmNlabelString, str); n++;
	XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	pu.label_w = XmCreateToggleButton (pu.pu_w, "SVPopLabel", args, n);
	XtAddCallback(pu.label_w, XmNvalueChangedCallback, sv_pu_label_cb,NULL);
	XtManageChild (pu.label_w);
	XmStringFree(str);

	/* the Trail TB: must un/manage and set state on each use */
	str = XmStringCreateLtoR ("Trail", XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg (args[n], XmNlabelString, str); n++;
	XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	pu.trail_w = XmCreateToggleButton (pu.pu_w, "SVPopTrail", args, n);
	XtAddCallback(pu.trail_w, XmNvalueChangedCallback, sv_pu_trail_cb,NULL);
	XmStringFree(str);

	/* add a nice separator */
	n = 0;
	w = XmCreateSeparator (pu.pu_w, "SVSep", args, n);
	XtManageChild(w);

	/* the Change Trail PB: must un/manage and set labelString each use */
	n = 0;
	pu.newtrail_w = XmCreatePushButton (pu.pu_w, "SVPopNewTrail", args, n);
	XtAddCallback (pu.newtrail_w, XmNactivateCallback, sv_pu_activate_cb,
							(XtPointer)NEWTRAIL);
}

/* create the zoom cascade menu off pulldown menu pd_w */
static void
sv_create_zoomcascade (pd_w)
Widget pd_w;
{
	Widget cb_w, zpd_w, w;
	Arg args[20];
	int n;
	int i;

	n = 0;
	zpd_w = XmCreatePulldownMenu (pd_w, "ZPD", args, n);

	n = 0;
	XtSetArg (args[n], XmNsubMenuId, zpd_w); n++;
	cb_w = XmCreateCascadeButton (pd_w, "ZCB", args, n);
	set_xmstring (cb_w, XmNlabelString, " Point + Zoom");
	XtManageChild (cb_w);

	for (i = 0; i < 6; i++) {
	    char buf[64];
	    int z;

	    /* z is the numerator, 10 is the denominator of zoom-in ratio */
	    switch (i) {
	    case 0: z =  20; break;
	    case 1: z =  50; break;
	    case 2: z = 100; break;
	    case 3: z =   5; break;
	    case 4: z =   2; break;
	    case 5: z =   1; break;
	    default: printf ("sv zoom: bad setup %d\n", i); exit (1);
	    }

	    if (z >= 10)
		(void) sprintf (buf, "Zoom in %dX", z/10);
	    else
		(void) sprintf (buf, "Zoom out %dX", 10/z);

	    n = 0;
	    w = XmCreatePushButton (zpd_w, "Zoom", args, n);
	    XtAddCallback (w, XmNactivateCallback, sv_pu_zoom_cb, (XtPointer)z);
	    set_xmstring (w, XmNlabelString, buf);
	    XtManageChild (w);
	}
}

/* called when any of the popup's pushbuttons are activated.
 * (the zoom cascade is not done here though).
 * client is a code to indicate which.
 */
/* ARGSUSED */
static void
sv_pu_activate_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int code = (int)client;

	switch (code) {
	case AIM:
	    (void) sv_mark (pu.tsp ? &pu.tsp->o : pu.op, 1, 1, 0, 0.0);
	    break;

	case LOCFIFO: {
	    /* send the current coords to the SKYLOCFIFO. */
	    Now *np = mm_get_now();
	    Obj *op = pu.tsp ? &pu.tsp->o : pu.op;
	    double y;
	    int fmag, bmag;

	    mjd_year (epoch == EOD ? mjd : epoch, &y);
	    svf_getmaglimits (&fmag, &bmag);
	    (void) skyloc_fifo (0, op->s_ra, op->s_dec, y, sv_fov, fmag);
	    }
	    break;

	case MK_OBJX:
	    if (pu.op->type == PLANET)
		xe_msg ("User objects can not be of type PLANET.", 0);
	    else {
		sv_ournewobj = 1;
		obj_set (pu.op);
		sv_ournewobj = 0;
		break;
	    }
	    break;

	case NEWTRAIL: {
	    TrailObj *top;
	    TrState *sp;

	    sp= pu.tsp && (top = tobj_find (&pu.tsp->o)) ? &top->trs : &trstate;
	    tr_setup ("xephem Sky Trail setup", pu.op->o_name, sp, sv_mktrail,
							    (XtPointer)pu.op);

	    }
	    break;

	default:
	    printf ("sv_pu_activate_db(): code=%d\n", code);
	    exit (1);
	    break;
	}
}

/* called when any of the zoom cascade buttons is activated.
 * client is the zoom-in factor numerator -- the denominator is 10.
 */
/* ARGSUSED */
static void
sv_pu_zoom_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int factor = (int)client;
	double newfov;

	newfov = sv_fov*10.0/factor;
	(void) sv_mark (pu.tsp ? &pu.tsp->o : pu.op, 1, 1, 0, newfov);
}

/* called when the Trail popup toggle button changes.
 * when called, pu.op will point to the base object (which is known to have a
 *   trail, but may not be on)
 */
/* ARGSUSED */
static void
sv_pu_trail_cb (wid, client, call)
Widget wid;
XtPointer client;
XtPointer call;
{
	TrailObj *top = tobj_find (pu.op);

	if (!top) {
	    printf ("sv_pu_trail_cb() bug -- no trail!\n");
	    exit (1);
	}

	if (XmToggleButtonGetState(wid)) {
	    /* trail is being turned on. just display it */
	    unsigned int w, h, r, xb, yb;
	    sv_getcircle (&w, &h, &r, &xb, &yb);
	    top->on = 1;
	    tobj_display_all(r, xb, yb);
	    sv_copy_sky();
	} else {
	    /* trailing is being turned off. mark it as being off.
	     * it will get discarded at the next update if it's still off.
	     * redraw sky so it disappears.
	     */
	    top->on = 0;
	    sv_all (mm_get_now());
	}
}

/* called when the Label popup toggle button changes.
 * we get all context from the pu structure.
 */
/* ARGSUSED */
static void
sv_pu_label_cb (wid, client, call)
Widget wid;
XtPointer client;
XtPointer call;
{
	unsigned int w, h, r, xb, yb;
	int x, y;
	int label;
	Obj *op;

	/* if this is a trailed item then its TSky will be in pu.tsp
	 * otherwise it is a plain db object.
	 * either way, toggle the corresponding OBJF_LABEL bit too.
	 */
	if (pu.tsp) {
	    label = (pu.tsp->flags ^= OBJF_LABEL) & OBJF_LABEL;
	    if (label) {
		sv_getcircle (&w, &h, &r, &xb, &yb);
		(void) sv_trailobjloc (pu.tsp, r, &x, &y);
		op = &pu.tsp->o;
	    }
	} else {
	    label = (pu.op->o_flags ^= OBJF_LABEL) & OBJF_LABEL;
	    if (label) {
		sv_getcircle (&w, &h, &r, &xb, &yb);
		(void) sv_dbobjloc (pu.op, r, &x, &y);
		op = pu.op;
	    }
	}

	if (label) {
	    /* label is being turned on so draw it right now */
	    Display *dsp = XtDisplay(svda_w);
	    Window win = sv_pm;
	    int d = objdiam (op);
	    GC gc;

	    x += xb;
	    y += yb;
	    obj_pickgc (op, svda_w, &gc);
	    draw_label (dsp, win, gc, op, x+d/4, y-d/4);
	    sv_copy_sky();
	} else {
	    /* label is being turned off so redraw to erase */
	    sv_all (mm_get_now());
	}
}

/* called when the Track popup toggle button changes.
 * we get all context from the pu structure.
 */
/* ARGSUSED */
static void
sv_pu_track_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (XmToggleButtonGetState (w)) {
	    track_op = pu.op;
	    XtSetSensitive (tracktb_w, True);
	    XmToggleButtonSetState (tracktb_w, True, True);
	    (void) sv_mark (track_op, 0, 0, 1, 0.0);
	} else
	    XmToggleButtonSetState (tracktb_w, False, True);
}

/* remove trails which are no longer turned on. */
static void
tobj_rmoff()
{
	TrailObj **topp;	/* address to be changed if we decide to 
				 * remove *topp
				 */
	TrailObj *top;		/* handy *topp */

	for (topp = &trailobj; (top = *topp) != NULL; ) {
	    if (top->on) {
		topp = &top->ntop;
	    } else {
		*topp = top->ntop;
		XtFree ((char *)top);
	    }
	}
}

/* remove the trailobj list that contains the given pointer.
 * we have to search each trail list to find the one with this pointer.
 * it might be the one on TrailObj itself or one of the older ones on
 *    the TSky list.
 * it's no big deal if op isn't really on any trail list.
 */
static void
tobj_rmobj (op)
Obj *op;
{
	TrailObj **topp;	/* address to be changed if we decide to 
				 * remove *topp
				 */
	TrailObj *top;		/* handy *topp */

	for (topp = &trailobj; (top = *topp) != NULL; ) {
	    int i;
	    if (top->op == op)
		goto out;
	    for (i = 0; i < top->nsky; i++)
		if (&top->sky[i].o == op)
		    goto out;
	    topp = &top->ntop;
	}

    out:

	if (!top)
	    return;	/* oh well */

	*topp = top->ntop;
	XtFree ((char *)top);
}

/* add a new TrailObj entry to the trailobj list for db object op.
 * make enough room in the sky array for nsky entries.
 * return a pointer to the new TrailObj.
 * (we never return if there's no memory)
 */
static TrailObj *
tobj_addobj (op, nsky)
Obj *op;
int nsky;
{
	TrailObj *top;
	int nbytes;

	/* don't forget there is inherently room for one TSky in a TrailObj */
	nbytes = sizeof(TrailObj) + (nsky-1)*sizeof(TSky);
	top = (TrailObj *) XtMalloc (nbytes);
	zero_mem ((void *)top, nbytes);
	top->nsky = nsky;	  /* though none are in use now */
	top->op = op;
	top->on = 1;

	/* link directly off trailobj -- order is unimportant */
	top->ntop = trailobj;
	trailobj = top;

	return (top);
}

/* remove each trail that refers to a db object no longer in the db.
 * also reset the tracked object if it's gone now too.
 * this is done when the db is reduced.
 */
static void
tobj_newdb()
{
	TrailObj *top;
	DBScan dbs;
	Obj *op;

	for (top = trailobj; top; top = top->ntop) {
	    for (db_scaninit (&dbs); (op = db_scan (&dbs)) != NULL; )
		if (top->op == op)
		    break;
	    if (!op)
		tobj_rmobj (top->op);
	}

	if (track_op) {
	    for (db_scaninit (&dbs); (op = db_scan (&dbs)) != NULL; )
		if (track_op == op)
		    break;
	    if (!op)
		track_op = NULL;
	}
}

/* find the TrailObj that contains op.
 * return NULL if don't find it.
 */
static TrailObj *
tobj_find (op)
Obj *op;
{
	TrailObj *top;

	for (top = trailobj; top; top = top->ntop) {
	    int i;
	    if (top->op == op)
		return (top);
	    for (i = 0; i < top->nsky; i++)
		if (&top->sky[i].o == op)
		    return (top);
	}

	return (NULL);
}

/* display everything in the trailobj list that is marked on onto sv_pm
 * clipped to a circle of radius r, offset by xb and yb borders.
 */
static void
tobj_display_all(r, xb, yb)
unsigned r, xb, yb;
{
	Display *dsp = XtDisplay(svda_w);
	Window win = sv_pm;
	TrailObj *top;

	for (top = trailobj; top; top = top->ntop) {
	    int x1 = 0, y1 = 0, x2, y2;
	    GC gc;
	    int i;

	    if (!top->on)
		continue;

	    obj_pickgc(top->op, svda_w, &gc);

	    for (i = 0; i < top->nsky; i++) {
		TSky *sp = &top->sky[i];
		Obj *op = &sp->o;

		if (sv_trailobjloc (sp, r, &x2, &y2)) {
		    int d = objdiam(op);

		    if (sp->flags & OBJF_LABEL)
			draw_label (dsp, win, gc, op, x2+xb+d/4, y2+yb+d/4);
		}
		if (i > 0) {
		    int sx1, sy1, sx2, sy2;

		    if (lc(0,0,r*2,x1,y1,x2,y2,&sx1,&sy1,&sx2,&sy2) == 0)
			tr_draw(dsp, win, gc, TICKLEN,
			    x2==sx2 && y2==sy2 ? &sp->trts : NULL,
			    i==1 && x1==sx1 && y1==sy1 ? &top->sky[0].trts:NULL,
				    &top->trs, sx1+xb, sy1+yb, sx2+xb, sy2+yb);
		}
		x1 = x2;
		y1 = y2;
	    }

	}

	sv_draw_obj (dsp, win, (GC)0, NULL, 0, 0, 0, 0);	/* flush */
}

/* determine if the given object is visible and within a circle of radius r
 * pixels. if so, return 1 and compute the location in *xp / *yp, else return 0.
 * N.B. only call this for bona fide db objects -- *not* for objects in the
 *   TrailObj lists -- it will destroy their history.
 */
static int
sv_dbobjloc (op, r, xp, yp)
Obj *op;
int r;
int *xp, *yp;
{
	double altdec, azra;

	if (!sv_precheck(op))
	    return (0);

	/* remaining things need accurate s_* fields */
	db_update(op);

	if (aa_mode && op->s_alt < 0.0)
	    return(0);	/* it's below horizon and we're in alt-az mode */

	/* persistent labels are immune to faint cutoff */
	if (!(op->o_flags & OBJF_LABEL)) {
	    int fmag, bmag;

	    svf_getmaglimits (&fmag, &bmag);
	    if (op->s_mag/MAGSCALE > fmag || op->s_mag/MAGSCALE < bmag)
		return(0);	/* it's not within mag range after all */
	}

	altdec = aa_mode ? op->s_alt : op->s_dec;
	azra   = aa_mode ? op->s_az  : op->s_ra;
	if (!sv_loc (r, altdec, azra, xp, yp))
	    return(0); 	/* it's outside the fov after all */

	return (1);	/* yup, really within r */
}

/* given a TSky find its location in a circle of radius r, all in pixels.
 * return 1 if the resulting x/y is in fact within the circle, else 0 if it is
 *   outside or otherwise should not be shown now (but return the x/y anyway).
 * N.B. we take care to not change the tsp->o in any way.
 */
static int
sv_trailobjloc (tsp, r, xp, yp)
TSky *tsp;
int r;
int *xp, *yp;
{
	Obj *op = &tsp->o;
	double altdec, azra;
	int infov;

	altdec = aa_mode ? op->s_alt : op->s_dec;
	azra   = aa_mode ? op->s_az  : op->s_ra;
	infov = sv_loc (r, altdec, azra, xp, yp);
	return (infov && (!aa_mode || op->s_alt >= 0.0));
}

/* do as much as possible to pre-check whether op is on screen now
 * WITHOUT computing it's actual coordinates. put another way, we are not to
 * use any s_* fields in these tests.
 * return 0 if we know it's definitely not on screen, or 1 if it might be.
 */
static int
sv_precheck (op)
Obj *op;
{
	if (op->type == UNDEFOBJ)
	    return(0);

	/* persistent labels are immune to faint cutoff */
	if (!(op->o_flags & OBJF_LABEL) && !svf_filter_ok(op))
	    return(0);
	if (!sv_infov(op))
	    return(0);
	
	return (1);
}

/* return 1 if the object can potentially be within the current sv_fov, else 0.
 * N.B. this is meant to be cheap - we only do fixed objects and we don't
 *      precess. most importantly, we don't use any s_* fields.
 */
static int
sv_infov (op)
Obj *op;
{
#define	DELEP	3650		/* maximum epoch difference we dare go without
				 * precessing, days
				 */
#define	MARGIN	degrad(1.0)	/* border around fov still considered "in"
				 * in spite of having not precessed.
				 */
	double ra0, dec0;	/* ra/dec of our center of view */
	Now *np;
	double r;
	int polecap;

	np = mm_get_now();

	if (op->type != FIXED)
	    return (1);
	if (fabs (mjd - op->f_epoch) > DELEP)
	    return (1);

	if (aa_mode) {
	    /* compute ra/dec of view center; worth caching too */
	    static double last_lat = 9876, last_alt, last_az;
	    static double last_ha, last_dec0;
	    double ha, lst;

	    if(lat == last_lat && sv_altdec == last_alt && sv_azra == last_az) {
		ha = last_ha;
		dec0 = last_dec0;
	    } else {
		aa_hadec (lat, sv_altdec, sv_azra, &ha, &dec0);
		last_lat = lat;
		last_alt = sv_altdec;
		last_az = sv_azra;
		last_ha = ha;
		last_dec0 = dec0;
	    }

	    /* now_lst() already knows how to cache; others are very cheap */
	    now_lst (np, &lst);
	    ra0 = hrrad(lst) - ha;
	    range (&ra0, 2*PI);
	} else {
	    ra0 = sv_azra;
	    dec0 = sv_altdec;
	}

	r = sv_fov/2 + MARGIN;
	polecap = fabs(op->f_dec) > PI/2 - (r + fabs(dec0));

	/* just check a surrounding "rectangular" region. */
	return (fabs(op->f_dec - dec0) < r
		&& (polecap || delra(op->f_RA-ra0)*cos(op->f_dec) < r));
}

/* compute x/y loc of a point at azra/altdec on a circle of radius rad pixels
 *   as viewed from sv_azra/sv_altdec with sv_fov.
 * return 1 if fits on screen, else 0 (but still return x/y).
 */
static int
sv_loc (rad, altdec, azra, xp, yp)
int rad;	/* radius of target display, pixels */
double altdec;	/* angle up from spherical equator, such as alt or dec; rad */
double azra;	/* angle around spherical pole, such as az or ra; rad */
int *xp, *yp;	/* return X coords within circle */
{
#define	LOCEPS	(1e-6)	/* an angle too small to see on screen, rads */
	static double last_sv_altdec = 123.0, last_sa, last_ca;
	double a,sa,ca;	/* angle from viewpoint to pole */
	double b,sb,cb;	/* angle from object to pole */
	double c,sc,cc;	/* difference in polar angles of obj and viewpoint */
	double d,sd,cd;	/* angular separation of object and viewpoint */
	double r;	/* proportion of d to desired field of view */
	double se, ce;	/* angle between (viewpoint,pole) and (viewpoint,obj) */

	a = PI/2 - sv_altdec;
	if (sv_altdec == last_sv_altdec) {
	    sa = last_sa;
	    ca = last_ca;
	} else {
	    last_sv_altdec = sv_altdec;
	    last_sa = sa = sin(a);
	    last_ca = ca = cos(a);
	}

	b = PI/2 - altdec;
	sb = sin(b);
	cb = cos(b);

	c = aa_mode ? azra - sv_azra : sv_azra - azra;
	cc = cos(c);

	cd = ca*cb + sa*sb*cc;
	if (cd >  1.0) cd =  1.0;
	if (cd < -1.0) cd = -1.0;
	d = acos(cd);

	if (d < LOCEPS) {
	    *xp = *yp = rad;
	    return (1);
	}

	r = d/(sv_fov/2.0);

	sc = sin(c);
	sd = sin(d);
	se = sc*sb/sd;
	*xp = flip_lr ? rad*(1 - r*se) + 0.5 : rad*(1 + r*se) + 0.5;
	if (a < LOCEPS) {
	    /* as viewpoint approaches N pole, e approaches PI - c */
	    ce = -cc;
	} else if (a > PI - LOCEPS) {
	    /* as viewpoint approaches S pole, e approaches c */
	    ce = cc;
	} else {
	    /* ok (we've already checked for small d) */
	    ce = (cb - cd*ca)/(sd*sa);
	}
	*yp = flip_tb ? rad*(1 + r*ce) + 0.5 : rad*(1 - r*ce) + 0.5;

	return (r >= 1.0 ? 0 : 1);
}

/* compute azra/altdec loc of a point at x/y on a circle of radius rad pixels
 *   as viewed from sv_azra/sv_altdec with sv_fov.
 * return 1 if x/y are within the circle, else 0.
 */
static int
sv_unloc (rad, x, y, altdecp, azrap)
int rad;	/* radius of target display, pixels */
int x, y;	/* X coords within circle */
double *altdecp;/* angle up from spherical equator, such as alt or dec; rad */
double *azrap;	/* angle around spherical pole, such as az or ra; rad */
{
#define	UNLOCEPS (1e-4)	/* sufficiently close to pole to not know az/ra; rads */
	double a,sa,ca;	/* angle from viewpoint to pole */
	double r;	/* distance from center to object, pixels */
	double d,sd,cd;	/* angular separation of object and viewpoint */
	double   se,ce;	/* angle between (viewpoint,pole) and (viewpoint,obj) */
	double b,sb,cb;	/* angle from object to pole */
	double c,   cc;	/* difference in polar angles of obj and viewpoint */

	if (x == rad && y == rad) {
	    /* at the center -- avoids cases where r == 0 */
	    *altdecp = sv_altdec;
	    *azrap = sv_azra;
	    return (1);
	}

	if (flip_lr)
	    x = 2*rad - x;
	if (flip_tb)
	    y = 2*rad - y;

	a = PI/2 - sv_altdec;
	sa = sin(a);
	ca = cos(a);

	r = sqrt ((double)((x-rad)*(x-rad) + (y-rad)*(y-rad)));
	if (r > rad)
	    return(0); /* outside the circle */

	d = r/rad*(sv_fov/2.0);
	sd = sin(d);
	cd = cos(d);
	ce = (rad - y)/r;
	se = (x - rad)/r;
	cb = ca*cd + sa*sd*ce;
	b = acos(cb);
	*altdecp = PI/2 - b;

	/* find c, the polar angle between viewpoint and object */
	if (a < UNLOCEPS) {
	    /* as viewpoint approaches N pole, c approaches PI - e */
	    c = acos(-ce);
	} else if (a > PI - UNLOCEPS) {
	    /* as viewpoint approaches S pole, c approaches e */
	    c = acos(ce);
	} else if (b < UNLOCEPS || b > PI - UNLOCEPS) {
	    /* as object approaches either pole, c becomes arbitary */
	    c = 0.0;
	} else {
	    sb = sin(b);
	    cc = (cd - ca*cb)/(sa*sb);
	    if (cc < -1.0) cc = -1.0;	/* heh, it happens ... */
	    if (cc >  1.0) cc =  1.0;	/* heh, it happens ... */
	    c = acos (cc);		/* 0 .. PI; next step checks if c
					 * should be > PI
					 */
	}
	if (se < 0.0) 		/* if e > PI */
	    c = PI + (PI - c);	/*     so is c */

	if (aa_mode)
	    *azrap = sv_azra + c;
	else
	    *azrap = sv_azra - c;
	range (azrap, 2*PI);

	return (1);
}

/* given an altdec/azra pair, find all coords for our current location.
 * all values will be topocentric if we are currently in Alt/Az display mode,
 * else all values will be geocentric.
 */
static void
sv_fullwhere (np, altdec, azra, altp, azp, rap, decp)
Now *np;
double altdec, azra;
double *altp, *azp;
double *rap, *decp;
{
	double ha;
	double lst;

	now_lst (np, &lst);
	if (aa_mode) {
	    /* need to make the ra/dec entries */
	    *altp = altdec;
	    *azp = azra;
	    unrefract (pressure, temp, altdec, &altdec);
	    aa_hadec (lat, altdec, azra, &ha, decp);
	    *rap = hrrad(lst) - ha;
	    range (rap, 2*PI);
	    if (epoch != EOD)
		precess (mjd, epoch, rap, decp);
	} else {
	    /* need to make the alt/az entries */
	    *rap = azra;
	    *decp = altdec;
	    if (epoch != EOD)
		precess (epoch, mjd, &azra, &altdec);
	    ha = hrrad(lst) - azra;
	    range (&ha, 2*PI);
	    hadec_aa (lat, ha, altdec, altp, azp);
	    refract (pressure, temp, *altp, altp);
	}
}

/* draw a nice grid on a circle of radius r, x and y borders xb and yb.
 */
static void
draw_grid(dsp, win, gc, r, xb, yb)
Display *dsp;
Window win;
GC gc;
unsigned int r;
unsigned int xb, yb;
{
	double vticks[NVGRID], hticks[NHGRID];
	double vmin, vmax, hmin, hmax;
	XSegment xsegments[NGRID*NSEGS], *xs;
	double dv, dh;
	char msg[64], msg2[64];
	int nvt, nht;
	int smallfov = raddeg(sv_fov) <= 5.0;
	int yc = yb+r;
	int xc = xb+r;
	int i, j;
	int pole;

	/* find the vertical tick marks */

	/* set vertical min and max, and detect whether pole is within fov. */
	pole = 0;
	vmin = sv_altdec-sv_fov/2;
	if (vmin < 0 && aa_mode)
	    vmin = 0.0;
	if (vmin <= -PI/2) {
	    /* clamp at pole */
	    vmin = -PI/2;
	    pole = 1;
	}
	vmax = sv_altdec+sv_fov/2;
	if (vmax >= PI/2) {
	    /* clamp at pole */
	    vmax = PI/2;
	    pole = 1;
	}

	/* use degs for vertical tickmarks unless smallfov then use mins. */
	vmin = raddeg(vmin);
	vmax = raddeg(vmax);
	if (smallfov) {
	    vmin *= 60.0;
	    vmax *= 60.0;
	}

	/* find vertial tick marks.
	 * N.B. remember that tickmarks() can return up to 2 more than asked.
	 */
	nvt = tickmarks(vmin, vmax, NVGRID-2, vticks);

	/* convert vertical ticks back to rads */
	for (i = 0; i < nvt; i++) {
	    if (smallfov)
		vticks[i] *= 1.0/60.0;
	    vticks[i] = degrad(vticks[i]);
	}

	/* find the horizontal tick marks */

	/* if pole is visible, we do it all manually to insure a perfect fit.
	 * else base spacing on spherical angle spanned by fov "del" from pole.
	 */
	if (pole) {
	    nht = 12+1;	/* 12 works well in degrees and hours */
	    for (i = 0; i < nht; i++)
		hticks[i] = i * 2*PI/(nht-1);
	} else {
	    double del = PI/2 - fabs(sv_altdec);
	    double a= acos((cos(sv_fov) - cos(del)*cos(del))/sin(del)/sin(del));

	    hmin = sv_azra-a/2;
	    hmax = sv_azra+a/2;

	    /* use degs/hrs unless smallfov then use mins*/
	    hmin = aa_mode ? raddeg(hmin) : radhr(hmin);
	    hmax = aa_mode ? raddeg(hmax) : radhr(hmax);
	    if (smallfov) {
		hmin *= 60.0;
		hmax *= 60.0;
	    }

	    /* find tick marks.
	     * N.B. remember that tickmarks() can return up to 2 more.
	     */
	    nht = tickmarks(hmin, hmax, NHGRID-2, hticks);

	    /* convert back to rads */
	    for (i = 0; i < nht; i++) {
		if (smallfov)
		    hticks[i] *= 1.0/60.0;
		hticks[i] = aa_mode ? degrad(hticks[i]) : hrrad(hticks[i]);
	    }
	}

	/* report the vertical spacing */
	dv = (vticks[nvt-1]-vticks[0])/(nvt-1);
	fs_sexa (msg2, raddeg(dv), 2, smallfov ? 3600 : 60);
	(void) sprintf (msg, "%s: %s", aa_mode ? "Alt" : "Dec", msg2);
	set_xmstring (vgrid_w, XmNlabelString, msg);

	/* report the horizontal spacing */
	dh = (hticks[nht-1]-hticks[0])/(nht-1);
	fs_sexa(msg2, aa_mode ? raddeg(dh) : radhr(dh), 2, smallfov ? 3600:60);
	(void) sprintf (msg, " %s: %s", aa_mode ? "Az" : "RA", msg2);
	set_xmstring (hgrid_w, XmNlabelString, msg);

	/* do the vertical lines (constant ra or az):
	 * for each horizontal tick mark
	 *   for each vertical tick mark
	 *     compute coord on screen
	 *     if we've at least 2 pts now
	 *       connect the points with what is visible within the circle.
	 */
	for (i = 0; i < nht; i += 1) {
	    double h = hticks[i];
	    int before = 0;
	    int vis1 = 0, vis2;
	    int x1 = 0, y1 = 0, x2, y2;
	    xs = xsegments;
	    for (j = 0; j < nvt; j++) {
		double v = vticks[j];
		vis2 = sv_loc(r, v, h, &x2, &y2);
		if (before++ && (vis1 || vis2)) {
		    int sx1, sy1, sx2, sy2;
		    if (lc(0,0,r*2,x1,y1,x2,y2,&sx1,&sy1,&sx2,&sy2) == 0) {
			xs->x1 = sx1+xb; xs->y1 = sy1+yb;
			xs->x2 = sx2+xb; xs->y2 = sy2+yb;
			if (!vis1 || !vis2) {
			    int x = !vis1 ? xs->x1 : xs->x2;
			    int y = !vis1 ? xs->y1 : xs->y2;
			    int dir, asc, des, len;
			    XCharStruct all;
			    char *mp;

			    fs_sexa (msg, aa_mode ? raddeg(h) : radhr(h), 3,
							smallfov ? 3600 : 60);
			    for (mp = msg; *mp == ' '; mp++) continue;
			    len = strlen (mp);
			    XTextExtents(sv_sf, mp, len, &dir,&asc,&des,&all);
			    /* draw vertical labels in up-right or low-left */
			    if (y < yc && y - all.ascent >= 0
					    && x > xc && x + all.width <= 2*xc)
				XPSDrawString (dsp, win, sv_strgc, x, y,mp,len);
			    else {
				y += all.ascent; /* keep outside */
				if (y < 2*yc && y - all.ascent > yc
					    && x <= xc && x - all.width >= 0)
				    XPSDrawString (dsp, win, sv_strgc,
						    x-all.width, y, mp, len);
			    }
			}
			xs++;
		    }
		}
		x1 = x2;
		y1 = y2;
		vis1 = vis2;
	    }
	    XPSDrawSegments (dsp, win, gc, xsegments, xs - xsegments);
	}

	/* do the horizontal lines (constant dec or alt):
	 * for each vertical tick mark
	 *   for each horizontal tick mark
	 *     compute coord on screen
	 *     if we've at least 2 pts now
	 *       connect the points with what is visible within the circle.
	 *	 (break into smaller pieces because these lines tend to curve)
	 */
	for (i = 0; i < nvt; i+=1) {
	    double v = vticks[i];
	    double h1 = 0;
	    int before = 0;
	    int vis1 = 0, vis2;
	    int x1 = 0, y1 = 0, x2, y2;
	    xs = xsegments;
	    for (j = 0; j < nht; j++) {
		double h = hticks[j];
		vis2 = sv_loc(r, v, h, &x2, &y2);
		if (before++ && (vis1 || vis2)) {
		    /* last point is at (x1,y1) == (h1,v);
		     * this point is at (x2,y2) == (h, v);
		     * connect with NSEGS segments.
		     */
		    int sx1, sy1, sx2, sy2;
		    int xt, yt;
		    int vist, k;
		    for (k = 1; k <= NSEGS; k++) {
			if (k == NSEGS) {
			    xt = x2;
			    yt = y2;
			    vist = vis2;
			} else
			    vist = sv_loc(r, v, h1+k*(h-h1)/NSEGS, &xt, &yt);
			if ((vis1 || vist) &&
				lc(0,0,r*2,x1,y1,xt,yt,&sx1,&sy1,&sx2,&sy2)==0){
			    xs->x1 = sx1+xb; xs->y1 = sy1+yb;
			    xs->x2 = sx2+xb; xs->y2 = sy2+yb;
			    if (!vis1 || !vist) {
				int x = !vis1 ? xs->x1 : xs->x2;
				int y = !vis1 ? xs->y1 : xs->y2;
				int dir, asc, des, len;
				XCharStruct all;
				char *mp;

				fs_sexa (msg, raddeg(v), 4, smallfov ? 3600:60);
				for (mp = msg; *mp == ' '; mp++) continue;
				len = strlen (mp);
				XTextExtents(sv_sf,mp,len,&dir,&asc,&des,&all);
				/* draw horz labels in up-left or low-right */
				if (y < yc && y - all.ascent >= 0
					    && x <= xc && x - all.width >= 0)
				    XPSDrawString (dsp,win,sv_strgc,x-all.width,
								    y, mp, len);
				else if (y - all.ascent > yc &&
					x > xc && x + all.width <= 2*xc)
				    XPSDrawString (dsp,win,sv_strgc,x,y,mp,len);
			    }
			    xs++;
			}
			x1 = xt;
			y1 = yt;
			vis1 = vist;
		    }
		}
		x1 = x2;
		y1 = y2;
		h1 = h;
		vis1 = vis2;
	    }
	    XPSDrawSegments (dsp, win, gc, xsegments, xs - xsegments);
	}
}

/* draw the ecliptic on a circle of radius r, x and y borders xb and yb.
 */
static void
draw_ecliptic(np, dsp, win, gc, r, xb, yb)
Now *np;
Display *dsp;
Window win;
GC gc;
unsigned int r;
unsigned int xb, yb;
{
#define	ECL_CACHE_SZ	100
	XPoint ptcache[ECL_CACHE_SZ];
	double elat0, elng0;	/* ecliptic lat and long at center of fov */
	double elngmin, elngmax;/* ecliptic long limits */
	double ra, dec;
	double elng;
	double lst;
	int ncache;

	now_lst (np, &lst);

	/* find ecliptic coords of center of view */
	if (aa_mode) {
	    double ha0;		/* local hour angle */
	    aa_hadec (lat, sv_altdec, sv_azra, &ha0, &dec);
	    ra = hrrad(lst) - ha0;
	} else {
	    ra = sv_azra;
	    dec = sv_altdec;
	}
	eq_ecl (mjd, ra, dec, &elat0, &elng0);

	/* no ecliptic visible if ecliptic latitude at center of view 
	 * is not less than fov/2
	 */
	if (fabs(elat0) >= sv_fov/2.0)
	    return;

	/* worst-case elong limits is center elong += half fov */
	elngmin = elng0 - sv_fov/2.0;
	elngmax = elng0 + sv_fov/2.0;

	/* put a mark at every ECL_TICS pixels */
	ncache = 0;
	for (elng = elngmin; elng <= elngmax; elng += sv_fov/(2.0*r/ECL_TICS)) {
	    int x, y;
	    double altdec, azra;

	    /* convert longitude along the ecliptic to ra/dec */
	    ecl_eq (mjd, 0.0, elng, &azra, &altdec);

	    /* if in aa mode, we need it in alt/az */
	    if (aa_mode) {
		hadec_aa (lat, hrrad(lst) - azra, altdec, &altdec, &azra);
		refract (pressure, temp, altdec, &altdec);
	    }

	    /* if visible, display point */
	    if ((!aa_mode || altdec >= 0) && sv_loc (r, altdec, azra, &x, &y)) {
		XPoint *xp = &ptcache[ncache++];
		xp->x = x + xb;
		xp->y = y + yb;
		if (ncache == XtNumber(ptcache)) {
		    XPSDrawPoints(dsp,win,gc,ptcache,ncache,CoordModeOrigin);
		    ncache = 0;
		}
	    }
	}

	if (ncache > 0)
	    XPSDrawPoints (dsp, win, gc, ptcache, ncache, CoordModeOrigin);
}

/* draw the constellation lines on a circle of radius r, x and y borders xb and
 * yb.
 */
static void
draw_cnsbounds(np, dsp, win, r, xb, yb)
Now *np;
Display *dsp;
Window win;
unsigned int r;
unsigned int xb, yb;
{
#define	NCONSEGS    23	/* draw with this fraction of r (primes look best) */
	double alt, az, ra, dec;
	double e = epoch == EOD ? mjd : epoch;
	double segsize = sv_fov/NCONSEGS;
	double cdec, sdec;
	double *era0, *edec0, *era1, *edec1;
	double lst;
	int nedges;

	/* get all the edges, precessed to e.
	 */
	nedges = cns_edges (e, &era0, &edec0, &era1, &edec1);
	if (nedges <= 0) {
	    xe_msg ("Can't find constellation edges :-(", 1);
	    return;
	}

	/* prepare for drawing */
	XSetForeground (dsp, sv_cnsgc, cnsbnd_p);
	sv_fullwhere (np, sv_altdec, sv_azra, &alt, &az, &ra, &dec);
	now_lst (np, &lst);
	cdec = cos(dec);
	sdec = sin(dec);

	/* for each edge.. break into smaller segments
	 * and draw any that are even partially visible.
	 */
	while (--nedges >= 0) {
	    double ra0  = era0[nedges];
	    double dec0 = edec0[nedges];
	    double ra1  = era1[nedges];
	    double dec1 = edec1[nedges];
	    XPoint xpts[NCONSEGS+10];
	    int lastvis, lastx, lasty;
	    double dra, ddec;
	    double sep, csep;
	    int see0, see1;
	    int nsegs;
	    int npts;
	    int j;
	    
	    /* cull segments that aren't even close */
	    solve_sphere (ra-ra0, PI/2-dec0, sdec, cdec, &csep, NULL);
	    see0 = (acos(csep) < sv_fov);
	    solve_sphere (ra-ra1, PI/2-dec1, sdec, cdec, &csep, NULL);
	    see1 = (acos(csep) < sv_fov);
	    if (!see0 && !see1)
		continue;

	    /* find number of segments with which to draw this edge */
	    solve_sphere (ra1-ra0, PI/2-dec1, sin(dec0), cos(dec0), &csep,NULL);
	    sep = acos(csep);
	    nsegs = (int)(sep/segsize) + 1;

	    /* find step sizes.
	     * N.B. watch for RA going the long way through 0
	     */
	    dra = ra1 - ra0;
	    if (dra < -PI)
		dra += 2*PI;
	    else if (dra > PI)
		dra -= 2*PI;
	    dra /= nsegs;
	    ddec = (dec1 - dec0)/nsegs;

	    /* step along the segment ends */
	    lastvis = -1; /* illegal return value from sv_loc() */
	    lastx = lasty = 0;
	    npts = 0;
	    for (j = 0; j <= nsegs; j++) {
		int vis, x, y;
		double ad, ar;

		ad = dec0 + j*ddec;
		ar = ra0 + j*dra;

		/* sv_loc wants alt/az when we are in aa mode */
		if (aa_mode) {
		    hadec_aa (lat, hrrad(lst) - ar, ad, &ad, &ar);
		    refract (pressure, temp, ad, &ad);
		}

		vis = sv_loc (r, ad, ar, &x, &y) && (!aa_mode || ad > 0);
		x += xb;
		y += yb;

		if (lastvis >= 0 && vis != lastvis) {
		    int x1, y1, x2, y2;

		    /* at edge of circle -- find crossing point */
		    if (lc (xb,yb,2*r,lastx,lasty,x,y,&x1,&y1,&x2,&y2) == 0) {
			if (vis) {
			    xpts[npts].x = x1;
			    xpts[npts].y = y1;
			    npts++;
			} else {
			    xpts[npts].x = x2;
			    xpts[npts].y = y2;
			    npts++;
			    break;	/* end of visible portion */
			}
		    }
		}
		if (vis) {
		    xpts[npts].x = x;
		    xpts[npts].y = y;
		    npts++;
		}

		lastvis = vis;
		lastx = x;
		lasty = y;
	    }

	    if (npts > XtNumber(xpts)) {
		/* stack has already overflowed but try to show how much */
		printf ("Constellation Overflow: npts=%d N=%d\n",
						npts, XtNumber(xpts));
		exit(1);
	    }

	    if (npts > 0)
		XPSDrawLines(dsp, win, sv_cnsgc, xpts, npts, CoordModeOrigin);
	}
}

/* draw the constellation figures and/or names on a circle of radius r, x and y
 * borders xb and yb.
 */
static void
draw_cns(np, dsp, win, r, xb, yb)
Now *np;
Display *dsp;
Window win;
unsigned int r;
unsigned int xb, yb;
{
#define	BBOX(x1,y1,x2,y2)   {				\
	    if (want_connames) {			\
		if (!atall || x1 < minx) minx = x1;	\
		if (!atall || x1 > maxx) maxx = x1;	\
		if (!atall || y1 < miny) miny = y1;	\
		if (!atall || y1 > maxy) maxy = y1;	\
		if (x2 < minx) minx = x2;		\
		if (x2 > maxx) maxx = x2;		\
		if (y2 < miny) miny = y2;		\
		if (y2 > maxy) maxy = y2;		\
		atall = 1;				\
	    }						\
	}
	double alt, az, ra, dec;
	double altdec, azra;
	double e = aa_mode || epoch == EOD ? mjd : epoch;
	double fra[40], fdec[40];
	double lst;
	int dcodes[40];
	int conids[89];
	int ncns, ndc;
	int x, lastx = 0, y, lasty = 0;
	int vis, lastvis = 0;
	int sx1, sy1, sx2, sy2;
	int minx = 0, maxx = 0, miny = 0, maxy = 0;
	int atall;
	int i;

	XSetForeground (dsp, sv_cnsgc, cnsfig_p);
	sv_fullwhere (np, sv_altdec, sv_azra, &alt, &az, &ra, &dec);
	ncns = cns_list (ra, dec, epoch == EOD ? mjd : epoch, sv_fov/2, conids);
	now_lst (np, &lst);

	while (--ncns >= 0) {
	    ndc = cns_figure (conids[ncns], e, fra, fdec, dcodes);
	    if (ndc <= 0) {
		printf ("Bogus constellation id:%d\n", conids[ncns]);
		exit (1);
	    }
	    atall = 0;
	    for (i = 0; i < ndc; i++) {
		/* sv_loc() is too smart -- it wants alt/az if in alt/az mode */
		if (aa_mode) {
		    /* N.B. we assume the f* arrays are precessed to EOD */
		    hadec_aa (lat, hrrad(lst) - fra[i], fdec[i], &altdec,&azra);
		    refract (pressure, temp, altdec, &altdec);
		} else {
		    altdec = fdec[i];
		    azra = fra[i];
		}
		vis= sv_loc (r, altdec, azra, &x, &y) && (!aa_mode || altdec>0);
		x += xb;
		y += yb;
		if (dcodes[i]) {
		    if (want_configures)
			sv_set_dashed_congc (dsp, dcodes[i] == 2);
		    if (vis) {
			if (lastvis) {
			    if (want_configures)
				XPSDrawLine (dsp, win,sv_cnsgc,lastx,lasty,x,y);
			    BBOX(lastx, lasty, x, y);
			} else {
			    if (lc (xb, yb, 2*r, lastx, lasty, x, y,
						&sx1, &sy1, &sx2, &sy2) == 0) {
				if (want_configures)
				  XPSDrawLine(dsp,win,sv_cnsgc,sx1,sy1,sx2,sy2);
				BBOX(sx1, sy1, sx2, sy2);
			    }
			}
		    } else {
			if (lastvis) {
			    if (lc (xb, yb, 2*r, lastx, lasty, x, y,
						&sx1, &sy1, &sx2, &sy2) == 0){
				if (want_configures)
				  XPSDrawLine(dsp,win,sv_cnsgc,sx1,sy1,sx2,sy2);
				BBOX(sx1, sy1, sx2, sy2);
			    }
			}
		    }
		}
		lastx = x;
		lasty = y;
		lastvis = vis;
	    }

	    if (want_connames && atall)
		draw_cnsname (dsp, win, conids[ncns], minx, miny, maxx, maxy);
	}

	sv_set_dashed_congc (dsp, 0);
#undef	BBOX
}

/* draw the name of the given constellation centered in the bounding box */
static void
draw_cnsname (dsp, win, conid, minx, miny, maxx, maxy)
Display *dsp;
Window win;
int conid;
int minx, miny, maxx, maxy;
{
	char *name = cns_name (conid) + 5;	/* skip "XXX: " */
	XCharStruct all;
	int len;
	int dir, asc, des;
	int x, y;

	len = strlen (name);
	XTextExtents (sv_sf, name, len, &dir, &asc, &des, &all);

	x = minx + (maxx - minx - all.rbearing)/2;
	y = miny + (maxy - miny - (all.ascent + all.descent))/2 + all.ascent;

	XPSDrawString (dsp, win, sv_strgc, x, y, name, len); 
}

/* draw a label for an object that is located at [x,y].
 * label consists of name and/or magnitide, depending on lbl_names and lbl_mags.
 * Look for greek names at the front of the name.
 */
static void
draw_label (dsp, win, gc, op, x, y)
Display *dsp;
Window win;
GC gc;
Obj *op;
int x, y;
{
	char buf[64];
	int sx = x + 4;	/* a little offset */
	int sy = y - 4;	/* a little offset */

	/* check the trivial case */
	if (!lbl_names && !lbl_mags)
	    return;

	/* local working copy */
	strcpy (buf, op->o_name);

	/* process name portion */
	if (lbl_names)
	    sx += draw_greeklabel (dsp, win, gc, buf, sx, sy);

	/* set (or append) mag in buf (too) if desired */
	if (lbl_mags) {
	    int m = (int)floor(op->s_mag/MAGSCALE*10.0 + 0.5);
	    if (lbl_names)
		(void) sprintf (buf+strlen(buf), " (%d)", m);
	    else
		(void) sprintf (buf, "%d", m);
	}

	/* draw buf */
	XPSDrawString (dsp, win, gc, sx, sy, buf, strlen(buf));
}

/* see if the given name is of the form Const BayerN-Flams. If so, draw the
 * Bayer letter in greek, N as a superscript, and return with just -Flams in
 * name[]. If this was as planned, return amount by which to shift x, else
 * return 0.
 */
static int
draw_greeklabel (dsp, win, gc, name, x, y)
Display *dsp;	/* display */
Window win;	/* drawable */
GC gc;		/* gc for normal text */
char name[];	/* name */
int x, y;	/* starting text location, pixels */
{
	static char *greeks[] = {
	    "Alpha", "Beta",    "Gamma",   "Delta", "Epsilon", "Zeta",
	    "Eta",   "Theta",   "Iota",    "Kappa", "Lambda",  "Mu", 
	    "Nu",    "Xi",      "Omicron", "Pi",    "Rho",     "Sigma",
	    "Tau",   "Upsilon", "Phi",     "Chi",   "Psi",     "Omega",
	};
	static char grfontidx[] = "abgdezhqiklmnxoprstufcjw";
	static int glen[XtNumber(greeks)];
	int strl;
	int gl;
	int i;

	/* forget it if no greek gc available */
	if (!sv_ggc)
	    return (0);

	/* init glen array the first time */
	if (glen[0] == 0)
	    for (i = 0; i < XtNumber(greeks); i++)
		glen[i] = strlen(greeks[i]);

	/* fast preliminary checks */
	strl = strlen (name);
	if (strl < 6)	/* shortest greek entry is "Cns Pi" */
	    return (0);
	if (name[3] != ' ')
	    return (0);

	/* find length of potentionally greek portion */
	for (gl = 0; ; gl++)
	    if (!isalpha(name[4+gl]))
		break;
	if (gl < 2)	/* shortest greek name is 2 chars */
	    return (0);

	/* scan for greek name -- it may be truncated */
	for (i = 0; i < XtNumber(greeks); i++) {
	    if (gl <= glen[i] && strncmp (name+4, greeks[i], gl) == 0) {
		char g = grfontidx[i];
		unsigned long gcm;
		XCharStruct xcs;
		XGCValues gcv;
		int dir, asc, des;
		char s;
		int l;

		/* use same color in ggc as in gc */
		gcm = GCForeground;
		(void) XGetGCValues (dsp, gc, gcm, &gcv);
		XSetForeground (dsp, sv_ggc, gcv.foreground);

		/* draw real greek - skip constellation portion */
		XTextExtents (sv_gf, &g, 1, &dir, &asc, &des, &xcs);
		XPSDrawString (dsp, win, sv_ggc, x, y, &g, 1);
		l = xcs.width+1;

		/* draw supersript if there is one */
		s = name[4+gl];
		if (isdigit(s)) {
		    XPSDrawString (dsp, win, gc, x+l, y-xcs.ascent/2, &s, 1);
		    XTextExtents (sv_gf, &s, 1, &dir, &asc, &des, &xcs);
		    l += xcs.width+1;
		    (void) strcpy (name, name+4+gl+1); /* won't overlap */
		} else
		    (void) strcpy (name, name+4+gl);   /* won't overlap */

		return (l);
	    }
	}

	return (0);
}

/* given an object return its desired diameter, in pixels.
 * the size is the larger of the actual size at the current window scale or
 *   a size designed to be proportional to the objects visual magnitude.
 * N.B. we assume op is at least as bright as fmag.
 */
static int
objdiam(op)
Obj *op;
{
	int fmag, bmag;

	svf_getmaglimits (&fmag, &bmag);
	return (magdiam (fmag, degrad((1.0/sv_sov)/3600.0), op->s_mag/MAGSCALE,
					    degrad((double)op->s_size/3600.0)));
}

/* make the GCs and learn the colors.
 * also make the tracking text gc, sv_strgc and the XFontStruct sv_sf.
 */
static void
sv_mk_gcs(dsp, win)
Display *dsp;
Window win;
{
	XGCValues gcv;
	unsigned long gcm = 0;
	Pixel p;

	get_something (svda_w, XmNforeground, (XtArgVal)&fg_p);
	get_something (svda_w, XmNbackground, (XtArgVal)&sky_p);
	get_something (svform_w, XmNbackground, (XtArgVal)&bg_p);
	set_something (svda_w, XmNbackground, (XtArgVal)bg_p);

	(void) get_color_resource (svda_w, "SkyCnsFigColor", &cnsfig_p);
	(void) get_color_resource (svda_w, "SkyCnsBndColor", &cnsbnd_p);
	sv_cnsgc = XCreateGC (dsp, win, gcm, &gcv);

	sv_gc = XCreateGC (dsp, win, gcm, &gcv);

	gcm = GCForeground | GCBackground | GCFont;
	get_something (aa_w, XmNforeground, (XtArgVal)&p);
	gcv.foreground = p;
	get_something (aa_w, XmNbackground, (XtArgVal)&p);
	gcv.background = p;
	get_xmlabel_font (aa_w, &sv_sf);
	gcv.font = sv_sf->fid;
	sv_strgc = XCreateGC (dsp, win, gcm, &gcv);

	/* load the greek font */
	loadGreek (dsp, win, &sv_ggc, &sv_gf);

	/* get font used by all gcs from obj_pickgc() */
	get_views_font (dsp, &sv_pf);
}

/* set line_style of sv_cnsgc to LineSolid if dashed==0 else to LineOnOffDash.
 * keep a cache so we don't mess with it any more than necessary.
 */
static void
sv_set_dashed_congc (dsp, dashed)
Display *dsp;
int dashed;
{
	static int last_dashed = -1243;	/* anything bogus */
	XGCValues xgcv;
	unsigned long mask;

	if (last_dashed == dashed)
	    return;
	last_dashed = dashed;

	mask = GCLineStyle;
	xgcv.line_style = dashed ? LineOnOffDash : LineSolid;
	XChangeGC (dsp, sv_cnsgc, mask, &xgcv);
}

/* send current coords to the SKYOUTFIFO.
 * open/close fifo as required as per want_outfifo as well.
 * N.B. we assume SIGPIPE is being ignored.
 * N.B. may be called with np == 0 to mean just open the fifo.
 */
static void
skyout_fifo (np)
Now *np;
{
	static char fmt[]="RA:%9.6f Dec:%9.6f Epoch:%9.3f\n";
	static char fifores[] = "SKYOUTFIFO";	/* name of X resource */
	static char *fn;	/* file name */
	static FILE *fp;	/* FILE pointer */
	double alt, az, ra, dec;
	double y;

	if (!want_outfifo) {
	    if (fp) {
		(void) fclose (fp);
		fp = NULL;
	    }
	    return;
	}

	/* open fifo if not already */
	if (!fp && open_wfifores (fifores, &fn, &fp) < 0) {
	    XmToggleButtonSetState (outfifo_w, False, True);
	    return;
	}

	if (!np)
	    return;

	sv_fullwhere (np, sv_altdec, sv_azra, &alt, &az, &ra, &dec);
	mjd_year (epoch == EOD ? mjd : epoch, &y);

	if (fprintf (fp, fmt, ra, dec, y) < 0 || fflush (fp) != 0) {
	    char msg[256];
	    (void) sprintf (msg, "Closing %.100s due to write error: %.100s\n",
							    fn, syserrstr());
	    xe_msg (msg, 1);
	    (void) fclose (fp);
	    fp = NULL;	/* signal no fp for next time */
	    XmToggleButtonSetState (outfifo_w, False, True);
	}
}

/* set up whether to accept pointing coords from SKYINFIFO.
 * open/close fifo as required as per want_infifo as well.
 */
static void
skyin_fifo ()
{
	static char fifores[] = "SKYINFIFO";	/* name of X resource */
	static int infifo_fd;		/* file descriptor for SKYINFIFO */
	static XtInputId infifo_xid;	/* input id for SKYINFIFO */
	char *fn;

	if (!want_infifo) {
	    if (infifo_fd >= 0) {
		(void) close (infifo_fd);
		infifo_fd = -1;
	    }
	    if (infifo_xid) {
		XtRemoveInput (infifo_xid);
		infifo_xid = 0;
	    }
	    return;
	}

	fn = XGetDefault (XtD, myclass, fifores);
	if (!fn) {
	    char msg[256];
	    (void) sprintf (msg, "No `%.200s' resource.\n", fifores);
	    xe_msg (msg, 0);
	    return;
	}

	/* open for read/write. this assures open will never block, that
	 * reads (and hence select()) WILL block if it's empty, and let's
	 * processes using it come and go as they please.
	 */
	infifo_fd = openh (fn, 2);
	if (infifo_fd < 0) {
	    char msg[256];
	    (void) sprintf (msg, "Can not open %.150s: %.50s\n",fn,syserrstr());
	    xe_msg (msg, 1);
	    XmToggleButtonSetState (infifo_w, False, True);
	    return;
	}

	infifo_xid = XtAppAddInput (xe_app, infifo_fd,
		    (XtPointer)XtInputReadMask, skyinfifo_cb, (XtPointer)fn);
}
