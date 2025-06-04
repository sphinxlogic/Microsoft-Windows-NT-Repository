/* code to manage the stuff on the "earth view" display.
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#if defined(__STDC__)
#include <stdlib.h>
#else
extern void *malloc(), *realloc();
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
#include <Xm/Scale.h>
#include <Xm/CascadeB.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "map.h"
#include "preferences.h"
#include "trails.h"
#include "sites.h"
#include "ps.h"

extern Widget	toplevel_w;
extern char	*myclass;
#define XtD     XtDisplay(toplevel_w)

extern Now *mm_get_now P_((void));
extern Obj *db_basic P_((int id));
extern char *mm_getsite P_((void));
extern int any_ison P_((void));
extern int get_color_resource P_((Widget w, char *cname, Pixel *p));
extern int lc P_((int cx, int cy, int cw, int x1, int y1, int x2, int y2,
    int *sx1, int *sy1, int *sx2, int *sy2));
extern int obj_cir P_((Now *np, Obj *op));
extern void f_dms_angle P_((Widget w, double a));
extern void f_double P_((Widget w, char *fmt, double f));
extern void f_showit P_((Widget w, char *s));
extern void fs_date P_((char out[], double jd));
extern void fs_dms_angle P_((char out[], double a));
extern void fs_mtime P_((char out[], double t));
extern void fs_pangle P_((char out[], double a));
extern void fs_sexa P_((char *out, double a, int w, int fracbase));
extern void fs_time P_((char out[], double t));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void get_xmlabel_font P_((Widget w, XFontStruct **f));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void mm_movie P_((double stepsz));
extern void mm_setsite P_((double slat, double slng, double slev, char *sname,
    int update));
extern void now_lst P_((Now *np, double *lst));
extern void register_selection P_((char *name));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void solve_sphere P_((double A, double b, double cosc, double sinc,
    double *cosap, double *Bp));
extern void timestamp P_((Now *np, Widget w));
extern void watch_cursor P_((int want));
extern void xe_msg P_((char *msg, int app_modal));

/* these are from earthmap.c */
extern MRegion ereg[];
extern int nereg;


/* info to keep track of object trails.
 */
typedef struct {
    Now t_now;		/* Now for object at a given time */
    Obj t_obj;		/* it's Obj at that time */
    double t_sublat, t_sublng;	/* sub lat/lng of object at that time */
    int t_lbl;		/* whether this tickmark gets labeled */
} Trail;


static void e_create_form P_((void));
static void e_create_esform P_((void));
static void e_point_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_stat_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_close_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_estats_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_movie_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_set_buttons P_((int whether));
static void e_help_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_helpon_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_print_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_print P_((void));
static void e_ps_annotate P_((Now *np, int fs));
static void e_ps_ll P_((char *tag, double lt, double lg, int x, int y));
static void e_exp_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_motion_cb P_((Widget w, XtPointer client, XEvent *ev,
    Boolean *continue_to_dispatch));
static void e_obj_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_objing_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_tb_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_trail_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_cyl_cb P_((Widget w, XtPointer client, XtPointer call));
static int e_set_dasize P_((int wantcyl));
static void e_setmain_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_latlong_cb P_((Widget w, XtPointer client, XtPointer call));
static void e_track_latlng P_((Widget w, XEvent *ev, unsigned r, unsigned xb, 
    unsigned yb, int x, int y));
static void e_popup P_((Widget w, XEvent *ev, unsigned r, unsigned xb,
    unsigned yb, int x, int y));
static void e_create_popup P_((void));
static void e_init_gcs P_((Display *dsp, Window win));
static void e_copy_pm P_((void));
static void e_setelatlng P_((double lt, double lg));
static void e_subobject P_((Now *np, Obj *op, double *latp, double *longp));
static void e_show_esat_stats P_((void));
static int e_coord P_((unsigned r, unsigned wb, unsigned hb, double pl,
    double pL, short *xp, short *yp));
static void e_drawgrid P_((unsigned r, unsigned wb, unsigned hb));
static void e_drawtrail P_((unsigned r, unsigned wb, unsigned hb));
static void e_drawobject P_((unsigned r, unsigned wb, unsigned hb));
static void e_drawfootprint P_((unsigned r, unsigned wb, unsigned hb,
    double slat, double slng, double el));
static void e_drawcircle P_((unsigned r, unsigned wb, unsigned hb, double slat,
    double slng, double rad));
static void e_drawcross P_((unsigned r, unsigned wb, unsigned hb, double lt,
    double lg, int style));
static int e_uncoord P_((unsigned r, unsigned xb, unsigned yb, int x, int y,
    double *ltp, double *lgp));
static void e_all P_((Now *np));
static void e_getcircle P_((unsigned int *wp, unsigned int *hp,
    unsigned int *rp, unsigned int *xbp, unsigned int *ybp));
static void e_map P_((Now *np));
static void e_mainmenuloc P_((Now *np, unsigned r, unsigned wb, unsigned hb));
static void e_soleclipse P_((Now *np, unsigned r, unsigned wb, unsigned hb));
static void e_sunlit P_((Now *np, unsigned int r, unsigned int wb,
    unsigned int hb));
static void e_msunlit P_((Now *np, unsigned int r, unsigned int wb,
    unsigned int hb));
static void e_ssunlit P_((Now *np, unsigned int r, unsigned int wb,
    unsigned int hb));
static void e_drawcontinents P_((unsigned r, unsigned wb, unsigned hb));
static void e_drawsites P_((unsigned r, unsigned wb, unsigned hb));
static int add_to_polyline P_((XPoint xp[], int xpsize, int i, int vis, int nxp,
    int max, int w, int x, int y));
static void e_viewrad P_((double height, double alt, double *radp));
static int e_resettrail P_((Now *np, int discard));
static int e_mktrail P_((TrTS ts[], TrState *statep, XtPointer client));
static Trail *e_growtrail P_((void));

#define	MOVIE_STEPSZ	0.5	/* movie step size, hours */

static int e_selecting;	/* set while our fields are being selected */
static Widget eform_w;	/* main form dialog */
static Widget e_da_w;	/* map DrawingArea widget */
static Widget e_dt_w;	/* main date/time stamp label widget */
static Widget e_sdt_w;	/* stats menu date/time stamp label widget */
static Pixmap e_pm;	/* use off screen pixmap for smoother drawing */
static Widget track_w;	/* toggle button to control tracking */
static Widget sites_w;	/* toggle button to control whether to show sites */
static Widget showobj_w;/* toggle button to control whether to show object */
static Widget objX_w, objY_w;	/* pulldown menu widgets for objs X and Y */
static Widget objcb_w;	/* the object name cascade button */
static Widget lat_w;	/* latitude scale widget */
static Widget long_w;	/* longitude scale widget */
static Widget cyl_w;	/* cylindrical view toggle button */
static Widget sph_w;	/* spherical view toggle button */
static Pixel e_fg, e_bg;/* fg and bg colors */
static GC e_strgc;	/* used for text within the drawing area */
static GC e_olgc;	/* used to draw map overlay details */
static GC e_gc;		/* used for all other GXCopy map drawing uses */
static XFontStruct *e_f;/* used to compute string extents */

/* this is to save info about the popup: widgets, and current pointing dir.
 */
#define	MAXPUL	7	/* max label widgets we ever need at once */
typedef struct {
    Widget pu_w;	/* main Popup parent widget */
    Widget pu_labels[MAXPUL];	/* the misc labels */
    double pu_lt;	/* popup's latitude */
    double pu_lg;	/* popup's longitude */
    double pu_elev;	/* popup's elevation */
    char *pu_site;	/* popup's site name, or NULL if none nearby */
} PopupInfo;
static PopupInfo pu_info;

/* info to get and keep track of the colors.
 * if we can't get all of them, we set them to fg/bg and use xor operations.
 * this is all set up in e_init_gcs().
 * N.B. the order of the entries in the enum must match the stuff in ecolors[].
 */
enum EColors {BORDERC, GRIDC, SITEC, OBJC, ECLIPSEC, SUNC, HEREC};
#define	NCOLORS 7
typedef struct {
    char *name;
    Pixel p;
} EColor;
static EColor ecolors[NCOLORS] = {
    {"EarthBorderColor"},
    {"EarthGridColor"},
    {"EarthSiteColor"},
    {"EarthObjColor"},
    {"EarthEclipseColor"},
    {"EarthSunColor"},
    {"EarthHereColor"}
};

/* info to keep track of all the "wants" toggle buttons. 
 */
enum Wants {		/* client codes for e_tb_cb and indices into wants[] */
    CYLINDRICAL=0, TRAIL, TRACK, GRID, OBJ, SITES, SUNLIGHT, LOWPREC,
    NWANTS		/* used to count number of entries -- must be last */
};
static wants[NWANTS];	/* fast state for each of the "wants" toggle buttons */

enum {SET_MAIN, SET_FROM_MAIN};	/* client codes for e_setmain_cb */

static double elat, elng;	/* view center lat/long: +N +E rads */
static double selat, celat;	/* sin and cos of elat */
static double eelev;		/* elevation from a pick, or 0.0 */
static char *esitename;		/* site name from a pick, or NULL */

/* following is to support the table of various statistics.
 */
typedef struct {
    char *selname;	/* name by which it is known for plotting etc */
    char *label;	/* label, else NULL if depends on preference */
    int enumcode;	/* one of the ES_ codes (just a check on right order) */
    Widget lw;		/* stat value label widget */
    Widget pbw;		/* stat value pushbutton widget */
} ESatStatTable;

/* code for each of the ess_ (s_*) fields unique to the Earth satellites.
 * N.B. must be in same order as the estattable entries!!
 */
enum estat {ES_SUBLAT, ES_SUBLONG, ES_ELEV, ES_RANGE, ES_RANGER, ES_ECLIPSED};

/* table to describe each statistic.
 * N.B. must be in same order as the estat entries!!
 */
static ESatStatTable estattable[] = {
    {"ESat.SubLat",	"Sub Latitude:",	ES_SUBLAT},
    {"ESat.SubLong",	"Sub Longitude:",	ES_SUBLONG},
    {"ESat.Elevation",	NULL,			ES_ELEV},
    {"ESat.Range",	NULL,			ES_RANGE},
    {"ESat.RangeRate",	NULL,			ES_RANGER},
    {"ESat.Eclipsed",	"In Sun Shadow:",	ES_ECLIPSED}
};

static Widget esform_w;		/* earth stats form widget */
static Widget esobjname_w;	/* earth stats object name widget */
static int eswasman;		/* whether earth stats was managed */

#define	DEFOBJ	SUN		/* default object to report/track */

/* constants that define details of various drawing actions */
#define	WIDE_LW		2		/* wide line width */
#define	NARROW_LW	0		/* narrow line width */
#define	LINE_ST		LineSolid	/* line style for lines */
#define	CAP_ST		CapRound	/* cap style for lines */
#define	JOIN_ST		JoinRound	/* join style for lines */
#define	CROSSH		1		/* drawcross code for a cross hair */
#define	PLUSS		2		/* drawcross code for a plus sign */
#define	PLLEN		degrad(4)	/* plussign arm radius, rads */
#define	CHLEN		degrad(4)	/* cross hair leg length, rads */
#define	LNGSTEP		15		/* longitude grid spacing, degrees */
#define	LATSTEP		15		/* latitude grid spacing, degrees */
#define	MAXDIST		degrad(3)	/* max popup search distance, rads */
#define	CYLASPECT	PI		/* cylndrical view w/h ratio */
#define	TICKLN		3		/* length of tickmarks on trails */
#define PRFONTSZ        10      	/* postscript font size */ 


/* Trails are kept as a variable-length malloc'd array of Trail objects.
 * N.B. the object in its current position is always the first entry.
 */
static Trail *trails;	/* malloc'ed array of Trails, or NULL if no trail yet */
static int ntrails;	/* number of entries in the trails[] array */
static TrState trstate = {	/* initial and retaining trails options */
    TRLR_2, TRI_5MIN, TRF_TIME, TRR_INTER, TRO_PATHL, TRS_MEDIUM, 20, 1
};

/* dbidx of current object */
static int object_dbidx;

void
e_manage()
{
	if (!eform_w) {
	    /* first call: create and init view to main menu's loc.
	     * rely on an expose to do the drawing.
	     */
	    Now *np = mm_get_now();

	    object_dbidx = DEFOBJ;
	    e_create_form();
	    e_create_esform();
	    e_setelatlng (lat, lng);
	    if (e_resettrail (np, 1) < 0)
		return;
	}

	if (XtIsManaged(eform_w)) {
	    XtUnmanageChild(eform_w);
	    if ((eswasman = XtIsManaged(esform_w)))
		XtUnmanageChild (esform_w);
	    if (e_pm) {
		XFreePixmap (XtDisplay(eform_w), e_pm);
		e_pm = (Pixmap) 0;
	    }
	} else {
	    XtManageChild(eform_w);
	    if (eswasman) {
		XtManageChild(esform_w);
		e_set_buttons(e_selecting);
	    }
	}
}

/* update the earth details.
 * remove all trail history if any.
 */
void
e_update(np, force)
Now *np;
int force;
{
	/* don't bother if we've never been created */
	if (!eform_w)
	    return;

	/* put np in trail list as current entry.
	 * also, discard trail info if not now visible.
	 */
	(void) e_resettrail(np, !wants[TRAIL]);

	/* don't bother with map if we're not up now.
	 * still do stats though if anyone cares about our values.
	 */
	if (!XtIsManaged(eform_w) && !force) {
	    if (any_ison())
		e_show_esat_stats ();
	    return;
	}

	e_all (np);
}

int
e_ison()
{
	return (eform_w && XtIsManaged(eform_w));
}

/* called when a user-defined object has changed.
 * we can assume that our update function will also be called subsequently.
 * if the object being changed is the one we are currently displaying
 *   and it has become undefined turn off tracking, discard any trails and
 *   change the current object back to DEFOBJ.
 */
void
e_newobj (dbidx)
int dbidx;	/* OBJX or OBJY */
{
	Now *np = mm_get_now();
	Obj *op;

	if (dbidx != object_dbidx) 
	    return;	/* doesn't effect us */

	/* change to the new object, or DEFOBJ if it's now undefined */
	op = db_basic (dbidx);
	if (op->type == UNDEFOBJ) {
	    object_dbidx = DEFOBJ;
	    op = db_basic (DEFOBJ);
	} else
	    object_dbidx = dbidx;

	/* set the current object name everywhere */
	set_xmstring (objcb_w, XmNlabelString, op->o_name);
	set_xmstring (esobjname_w, XmNlabelString, op->o_name);

	/* discard any old trail history and reset to current object */
	(void) e_resettrail(np, 1);
}

/* called by other menus as they want to hear from our buttons or not.
 * the ons and offs stack.
 */
void
e_selection_mode (whether)
int whether;
{
	e_selecting += whether ? 1 : -1;

	if (e_ison())
	    if ((whether && e_selecting == 1)	  /* first one to want on */
		|| (!whether && e_selecting == 0) /* first one to want off */)
		e_set_buttons (whether);
}

void
e_cursor(c)
Cursor c;
{
	Window win;

	if (eform_w && (win = XtWindow(eform_w))) {
	    Display *dsp = XtDisplay(eform_w);
	    if (c)
		XDefineCursor(dsp, win, c);
	    else
		XUndefineCursor(dsp, win);
	}

	if (esform_w && (win = XtWindow(esform_w))) {
	    Display *dsp = XtDisplay(esform_w);
	    if (c)
		XDefineCursor(dsp, win, c);
	    else
		XUndefineCursor(dsp, win);
	}
}

/* create the main form.
 * the earth stats form is created separately.
 */
static void
e_create_form()
{
	typedef struct {
	    char *label;	/* what goes on the help label */
	    char *key;		/* string to call hlp_dialog() */
	} HelpOn;
	static HelpOn helpon[] = {
	    {"Intro...",	"Earth - intro"},
	    {"on Mouse...",	"Earth - mouse"},
	    {"on Control...",	"Earth - control"},
	    {"on View...",	"Earth - view"},
	    {"on Object...",	"Earth - object"},
	    {"Misc...",		"Earth - misc"},
	};
	typedef struct {
	    char *name;
	    char *label;
	    void (*cb)();
	    int client;
	    Widget *wp;
	} BtnInfo;
	static BtnInfo ctl[] = {
	    {"ESave",   "Save...",       e_print_cb,   0,             NULL},
	    {"EPrint",  "Print...",      e_print_cb,   1,             NULL},
	    {"Sep0",    NULL,            NULL,         0,             NULL},
	    {"SetMain", "Set Main",      e_setmain_cb, SET_MAIN,      NULL},
	    {"SetFrom", "Set From Main", e_setmain_cb, SET_FROM_MAIN, NULL},
	    {"Movie",   "Movie Demo",    e_movie_cb,   0,             NULL},
	    {"Sep1",    NULL,            NULL,         0,             NULL},
	    {"Close",   "Close",         e_close_cb,   0,             NULL},
	};
	static BtnInfo view[] = {
	    {"grid",     "Grid",        e_tb_cb,  GRID,          NULL},
	    {"sites",    "Sites",       e_tb_cb,  SITES,         &sites_w},
	    {"sunlight", "Sunlight",    e_tb_cb,  SUNLIGHT,      NULL},
	    {"lowprec",  "Course+Fast", e_tb_cb,  LOWPREC,       NULL},
	    {"Sep2",     NULL,          NULL,     0,             NULL},
	};
	static BtnInfo obj[] = {
	    {"Sep2",   NULL,           NULL,         0,           NULL},
	    {"track",  "Track Object", e_tb_cb,      TRACK,       &track_w},
	    {"object", "Show Object",  e_tb_cb,      OBJ,         &showobj_w},
	    {"trail",  "Show Trail",   e_tb_cb,      TRAIL,       NULL},
	    {"Sep3",   NULL,           NULL,         0,           NULL},
	};
	Widget pd_w, cas_w, mb_w, pr_w;
	EventMask mask;
	XmString str;
	Widget w;
	Arg args[20];
	int n;
	int i;

	/* create form */

	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNdefaultPosition, False); n++;
	XtSetArg (args[n], XmNverticalSpacing, 2); n++;
	XtSetArg (args[n], XmNhorizontalSpacing, 2); n++;
	eform_w = XmCreateFormDialog (toplevel_w, "Earth", args,n);
	XtAddCallback (eform_w, XmNhelpCallback, e_help_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Earth view"); n++;
	XtSetValues (XtParent(eform_w), args, n);

	/* make the menu bar and all its pulldowns */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	mb_w = XmCreateMenuBar (eform_w, "MB", args, n);
	XtManageChild (mb_w);

	/* make the Control pulldown */

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "ControlPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'C'); n++;
	    cas_w = XmCreateCascadeButton (mb_w, "ControlCB", args, n);
	    set_xmstring (cas_w, XmNlabelString, "Control");
	    XtManageChild (cas_w);

	    for (i = 0; i < XtNumber(ctl); i++) {
		BtnInfo *bip = &ctl[i];

		n = 0;
		if (!bip->label)
		    w = XmCreateSeparator (pd_w, bip->name, args, n);
		else {
		    w = XmCreatePushButton (pd_w, bip->name, args, n);
		    set_xmstring (w, XmNlabelString, bip->label);
		    XtAddCallback (w, XmNactivateCallback, bip->cb,
						    (XtPointer)bip->client);
		}
		if (bip->wp)
		    *bip->wp = w;
		XtManageChild (w);
	    }

	/* make the View pulldown */

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "ViewPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'V'); n++;
	    cas_w = XmCreateCascadeButton (mb_w, "ViewCB", args, n);
	    set_xmstring (cas_w, XmNlabelString, "View");
	    XtManageChild (cas_w);

	    /* simulate a radio box for the cyl/spherical view selection */

	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    XtSetArg (args[n], XmNmarginHeight, 0); n++;
	    XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY); n++;
	    cyl_w = XmCreateToggleButton (pd_w, "cylindrical", args, n);
	    set_xmstring (cyl_w, XmNlabelString, "Cylindrical");
	    XtAddCallback (cyl_w,XmNvalueChangedCallback,e_cyl_cb,(XtPointer)1);
	    XtManageChild (cyl_w);

	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    XtSetArg (args[n], XmNmarginHeight, 0); n++;
	    XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY); n++;
	    sph_w = XmCreateToggleButton (pd_w, "spherical", args, n);
	    set_xmstring (sph_w, XmNlabelString, "Spherical");
	    XtAddCallback (sph_w,XmNvalueChangedCallback,e_cyl_cb,(XtPointer)0);
	    XtManageChild (sph_w);

	    if(XmToggleButtonGetState(cyl_w)==XmToggleButtonGetState(sph_w)){
		int cylmode = 0;	/* default to pol if they conflict */
		xe_msg("Earth View display mode conflicts -- using Spherical",0);
		XmToggleButtonSetState (cyl_w, cylmode, False); 
		XmToggleButtonSetState (sph_w, !cylmode, False); 
	    }
	    wants[CYLINDRICAL] = XmToggleButtonGetState(cyl_w);

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    for (i = 0; i < XtNumber(view); i++) {
		BtnInfo *bip = &view[i];

		n = 0;
		if (!bip->label)
		    w = XmCreateSeparator (pd_w, bip->name, args, n);
		else {
		    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
		    XtSetArg (args[n], XmNmarginHeight, 0); n++;
		    w = XmCreateToggleButton (pd_w, bip->name, args, n);
		    wants[bip->client] = XmToggleButtonGetState(w);
		    set_xmstring (w, XmNlabelString, bip->label);
		    XtAddCallback (w, XmNvalueChangedCallback, bip->cb,
						    (XtPointer)bip->client);
		}

		if (bip->wp)
		    *bip->wp = w;

		XtManageChild (w);
	    }

	    /* confirm ok for Sites to be on */
	    if (wants[SITES] && sites_get_list(NULL) < 0) {
		wants[SITES] = 0;
		XmToggleButtonSetState (sites_w, False, False);
		xe_msg ("No SITESFILE found.", 0);
	    }

	    /* add the earth Stats control */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Stats", args, n);
	    set_xmstring (w, XmNlabelString, "More info...");
	    XtAddCallback (w, XmNactivateCallback, e_estats_cb, NULL);
	    XtManageChild (w);

	/* make the Object pulldown */

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "ObjectPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'O'); n++;
	    cas_w = XmCreateCascadeButton (mb_w, "ObjectCB", args, n);
	    set_xmstring (cas_w, XmNlabelString, "Object");
	    XtManageChild (cas_w);

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Trail", args, n);
	    XtAddCallback (w, XmNactivateCallback, e_trail_cb, NULL);
	    set_xmstring (w, XmNlabelString, "Create Trail...");
	    XtManageChild (w);

	    for (i = 0; i < XtNumber(obj); i++) {
		BtnInfo *bip = &obj[i];

		n = 0;
		if (!bip->label)
		    w = XmCreateSeparator (pd_w, bip->name, args, n);
		else {
		    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
		    XtSetArg (args[n], XmNmarginHeight, 0); n++;
		    w = XmCreateToggleButton (pd_w, bip->name, args, n);
		    wants[bip->client] = XmToggleButtonGetState(w);
		    set_xmstring (w, XmNlabelString, bip->label);
		    XtAddCallback (w, XmNvalueChangedCallback, bip->cb,
						    (XtPointer)bip->client);
		}
		if (bip->wp)
		    *bip->wp = w;
		XtManageChild (w);
	    }

	    /* add the object name pull-right */

	    n = 0;
	    pr_w = XmCreatePulldownMenu (pd_w, "ObjectPD", args, n);

		n = 0;
		XtSetArg (args[n], XmNsubMenuId, pr_w); n++;
		objcb_w = XmCreateCascadeButton (pd_w, "ObjNamCB", args, n);
		XtAddCallback (objcb_w, XmNcascadingCallback, e_objing_cb, 0);
		XtManageChild (objcb_w);

		/* create all the pushbuttons forming the cascade menu.
		 * go ahead and fill in and manage the planet names now.
		 * we do the user objects just as we are cascading.
		 */
		for (i = 0; i < NOBJ; i++) {
		    Obj *op = NULL;

		    n = 0;
		    w = XmCreatePushButton (pr_w, "ObjectPB", args, n);
		    XtAddCallback (w, XmNactivateCallback, e_obj_cb,
								(XtPointer)i);
		    if (i >= MERCURY && i <= MOON) {
			op = db_basic (i);
			set_xmstring (w, XmNlabelString, op->o_name);
			XtManageChild (w);
		    } else if (i == OBJX)
			objX_w = w;
		    else if (i == OBJY)
			objY_w = w;
		    
		    if (i == DEFOBJ)
			set_xmstring (objcb_w, XmNlabelString, op->o_name);
		}

	/* make the help pulldown */

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "HelpPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'H'); n++;
	    cas_w = XmCreateCascadeButton (mb_w, "HelpCB", args, n);
	    set_xmstring (cas_w, XmNlabelString, "Help");
	    XtManageChild (cas_w);
	    set_something (mb_w, XmNmenuHelpWidget, (XtArgVal)cas_w);

	    for (i = 0; i < XtNumber(helpon); i++) {
		HelpOn *hp = &helpon[i];

		str = XmStringCreate (hp->label, XmSTRING_DEFAULT_CHARSET);
		n = 0;
		XtSetArg (args[n], XmNlabelString, str); n++;
		XtSetArg (args[n], XmNmarginHeight, 0); n++;
		w = XmCreatePushButton (pd_w, "Help", args, n);
		XtAddCallback (w, XmNactivateCallback, e_helpon_cb,
							(XtPointer)(hp->key));
		XtManageChild (w);
		XmStringFree(str);
	    }

	/* make the date/time indicator label */

	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	XtSetArg (args[n], XmNrecomputeSize, False); n++;
	e_dt_w = XmCreateLabel (eform_w, "DTstamp", args, n);
	XtManageChild (e_dt_w);

	/* make the long and lat scales */

	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, e_dt_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNminimum, -180); n++;
	XtSetArg (args[n], XmNmaximum, 179); n++;
	XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_LEFT); n++;
	XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNshowValue, True); n++;
	long_w = XmCreateScale (eform_w, "LongS", args, n);
	XtAddCallback (long_w, XmNvalueChangedCallback, e_latlong_cb, NULL);
	XtManageChild (long_w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, long_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNminimum, -90); n++;
	XtSetArg (args[n], XmNmaximum, 90); n++;
	XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_TOP); n++;
	XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg (args[n], XmNshowValue, True); n++;
	lat_w = XmCreateScale (eform_w, "LatS", args, n);
	XtAddCallback (lat_w, XmNvalueChangedCallback, e_latlong_cb, NULL);
	XtManageChild (lat_w);

	/* make a drawing area on top */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, long_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNrightWidget, lat_w); n++;
	e_da_w = XmCreateDrawingArea (eform_w, "Map", args, n);
	XtAddCallback (e_da_w, XmNexposeCallback, e_exp_cb, NULL);
	mask = Button1MotionMask | ButtonPressMask | ButtonReleaseMask
						    | PointerMotionHintMask,
	XtAddEventHandler (e_da_w, mask, False, e_motion_cb, 0);
	XtManageChild (e_da_w);

	/* init da's size */
	(void) e_set_dasize(wants[CYLINDRICAL]);
}

/* make the stats form dialog */
static void
e_create_esform()
{
	Obj *op;
	Arg args[20];
	Widget w, rc_w, sep_w;
	int n;
	int i;

	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNhorizontalSpacing, 2); n++;
	XtSetArg (args[n], XmNverticalSpacing, 2); n++;
	esform_w = XmCreateFormDialog (toplevel_w, "EarthStats", args, n);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "Earth info"); n++;
	XtSetValues (XtParent(esform_w), args, n);

	/* make a label at the top for the current object name */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	esobjname_w = XmCreateLabel (esform_w, "ObjName", args, n);
	XtManageChild (esobjname_w);

	op = db_basic (object_dbidx);
	set_xmstring (esobjname_w, XmNlabelString, op->o_name);

	/* add a separator */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, esobjname_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (esform_w, "Sep1", args, n);
	XtManageChild (sep_w);

	/* make a row/col for everything */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNnumColumns, XtNumber(estattable)); n++;
	rc_w = XmCreateRowColumn (esform_w, "ESRC", args, n);
	XtManageChild(rc_w);

	/* make each earth satellite statistic label/value pair.
	 * only manage now the ones that are always up.
	 */

	for (i = 0; i < XtNumber (estattable); i++) {
	    ESatStatTable *stp = estattable + i;
	    XmString str;

	    /* create the labels for the ones that don't change depending
	     * upon preferences.
	     */
	    n = 0;
	    str = 0;
	    if (stp->label) {
		str = XmStringCreateLtoR (stp->label,
						XmSTRING_DEFAULT_CHARSET);
		XtSetArg (args[n], XmNlabelString, str); n++;
	    }
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    stp->lw = XmCreateLabel (rc_w, "ESatStatLabel", args, n);
	    if (str)
		XmStringFree (str);

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    XtSetArg (args[n], XmNuserData, stp->selname); n++;
	    stp->pbw = XmCreatePushButton (rc_w, "ESatStatPB", args, n);
	    XtAddCallback (stp->pbw, XmNactivateCallback, e_stat_cb, 0);

	    switch (stp->enumcode) {
	    case ES_SUBLONG:
	    case ES_SUBLAT:
		XtManageChild (stp->lw);
		XtManageChild (stp->pbw);
		break;
	    }
	}

	/* add a separator */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (esform_w, "Sep2", args, n);
	XtManageChild (sep_w);

	/* add a label for the current date/time stamp */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	e_sdt_w = XmCreateLabel (esform_w, "SDTstamp", args, n);
	XtManageChild (e_sdt_w);

	/* add a separator */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, e_sdt_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (esform_w, "Sep3", args, n);
	XtManageChild (sep_w);

	/* add a close button */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 20); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 80); n++;
	w = XmCreatePushButton (esform_w, "Close", args, n);
	XtAddCallback (w, XmNactivateCallback, e_estats_cb, NULL);
	XtManageChild (w);
}

/* callback when any of the stat buttons are activated. */
/* ARGSUSED */
static void
e_stat_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (e_selecting) {
	    char *userD;

	    get_something (w, XmNuserData, (XtArgVal)&userD);
	    register_selection (userD);
	}
}

/* callback for when the main Close button is activated */
/* ARGSUSED */
static void
e_close_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if ((eswasman = XtIsManaged(esform_w)))
	    XtUnmanageChild(esform_w);
	XtUnmanageChild (eform_w);

	if (e_pm) {
	    XFreePixmap (XtDisplay(w), e_pm);
	    e_pm = (Pixmap) 0;
	}

	/* stop movie that might be running */
	mm_movie (0.0);
}

/* callback for when the stats dialog is closed or activated. */
/* ARGSUSED */
static void
e_estats_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if ((eswasman = XtIsManaged(esform_w)))
	    XtUnmanageChild (esform_w);
	else {
	    XtManageChild (esform_w);
	    e_set_buttons(e_selecting);
	}
}

/* callback for when the Movie button is activated. */
/* ARGSUSED */
static void
e_movie_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	mm_movie (MOVIE_STEPSZ);
}

/* go through all the buttons and set whether they
 * should appear to look like buttons or just flat labels.
 */
static void
e_set_buttons (whether)
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
	int i;

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
	    XtGetValues (track_w, args, n);

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

	for (i = 0; i < XtNumber(estattable); i++)
	    XtSetValues (estattable[i].pbw, ap, na);
}

/* callback from the Help all button
 */
/* ARGSUSED */
static void
e_help_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static char *msg[] = {
"This is a simple schematic depiction of the Earth surface at the given time.",
"The view shows the sunlit portion and the location of any object.",
};

	hlp_dialog ("Earth", msg, XtNumber(msg));
}

/* callback from a specific Help button.
 * client is a string to use with hlp_dialog().
 */
/* ARGSUSED */
static void
e_helpon_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	hlp_dialog ((char *)client, NULL, 0);
}

/* callback from Save and Print controls.
 * client is 0 if the former, 1 if the latter.
 */
/* ARGSUSED */
static void
e_print_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XPSAsk ((int)client, "Earth", e_print);
}

/* proceed to generate a postscript file.
 * call XPSClose() when finished.
 */
static void
e_print ()
{
	Now *np = mm_get_now();
	Window win = e_pm;
	int fontsz = PRFONTSZ;
	unsigned int w, h, r, wb, hb;

	if (!e_ison()) {
	    xe_msg ("Earth must be open to save a file.", 1);
	    XPSClose();
	    return;
	}

	watch_cursor(1);

	/* get size of the rendering area */
	e_getcircle (&w, &h, &r, &wb, &hb);

	/* fit view in square across the top and prepare to capture X calls */
	if (wants[CYLINDRICAL])
	    XPSXBegin (win, 0, 0, w, h, 1*72, 10*72, (int)(6.5*72), fontsz);
	else
	    XPSXBegin (win, wb, hb, 2*r, 0, 1*72, 10*72,(int)(6.5*72),fontsz);

	/* redraw */
	e_map (np);

	/* no more X captures */
	XPSXEnd();

	/* add some extra info */
	e_ps_annotate (np, fontsz);

	/* finished */
	XPSClose();

	watch_cursor(0);
}

static void
e_ps_annotate (np, fs)
Now *np;
int fs;
{
	char buf[32], buf2[32];
	char dir[128];
	int vs = fs+1;		/* nominal vertical line spacing */
	double sslat, sslong;
	Obj *op = &trails[0].t_obj;
	char *site = mm_getsite();
	int y;

	/* title */
	y = 72 + vs*14;
	(void) sprintf (dir, "(Xephem %s Earth View) 306 %d cstr\n",
			wants[CYLINDRICAL] ? "Cylindrical" : "Spherical", y);
	XPSDirect (dir);

	/* date, time, jd */
	y = 72 + vs*12;
	fs_time (buf, mjd_hr(mjd));
	fs_date (buf2, mjd_day(mjd));
	(void) sprintf (dir, "(UTC:) 144 %d rstr (%s  %s) 154 %d lstr\n",
							    y, buf, buf2, y);
	XPSDirect (dir);
	(void) sprintf (dir,"(JD:) 410 %d rstr (%13.5f) 420 %d lstr\n", y,
								mjd+MJD0, y);
	XPSDirect (dir);

	/* map center, subsolar loc */
	y = 72 + vs*11;
	e_ps_ll ("Map Center", elat, elng, 144, y);
	e_subobject (np, db_basic(SUN), &sslat, &sslong);
	e_ps_ll ("SubSolar", sslat, sslong, 410, y);

	y = 72 + vs*10;
	e_ps_ll ("Reference", lat, lng, 144, y);
	if (site) {
	    (void) sprintf (dir, "(, %s) show\n", site);
	    XPSDirect (dir);
	}

	/* remainder just if have an active object */
	if (!wants[OBJ])
	    return;

	/* name */
	y = 72 + vs*8;
	(void) sprintf (dir, "(Target: %s%s) 306 %d cstr\n", op->o_name,
		op->type == EARTHSAT
			? (op->s_eclipsed ? " [Eclipsed]" : " [Sunlit]")
			: "", y);
	XPSDirect (dir);

	/* location and alt/az */
	y = 72 + vs*7;
	e_ps_ll ("Location", trails[0].t_sublat, trails[0].t_sublng, 144, y);
	fs_sexa (buf, raddeg(op->s_alt), 3, 3600);
	fs_sexa (buf2, raddeg(op->s_az), 4, 3600);
	(void) sprintf (dir, "(Ref Alt/Az:) 410 %d rstr (%s %s) 420 %d lstr\n",
							    y, buf, buf2, y);
	XPSDirect (dir);

	/* elev range, if earthsat */
	y = 72 + vs*6;
	if (op->type == EARTHSAT) {
	    int unitspref = pref_get (PREF_UNITS);

	    if (unitspref == PREF_ENGLISH)
		(void) sprintf (dir,
			"(Elevation(mi):) 144 %d rstr ( %.0f) 154 %d lstr\n",
						y, op->s_elev*FTPM/5280.0, y);
	    else
		(void) sprintf (dir,
			"(Elevation(km):) 144 %d rstr ( %.0f) 154 %d lstr\n",
						    y, op->s_elev/1000.0, y);
	    XPSDirect (dir);

	    if (unitspref == PREF_ENGLISH)
		(void) sprintf (dir,
			"(Range(mi):) 410 %d rstr ( %.0f) 420 %d lstr\n",
						y, op->s_range*FTPM/5280.0, y);
	    else
		(void) sprintf (dir,
			"(Range(km):) 410 %d rstr ( %.0f) 420 %d lstr\n",
						    y, op->s_range/1000.0, y);
	    XPSDirect (dir);
	}
}

static void
e_ps_ll (tag, lt, lg, x, y)
char *tag;
double lt, lg;
int x, y;
{
	char ltstr[32], lgstr[32];
	char buf[128];

	fs_sexa (ltstr, raddeg(fabs(lt)), 3, 3600);
	strcat (ltstr, lt < 0 ? " S" : " N");

	fs_sexa (lgstr, raddeg(fabs(lg)), 4, 3600);
	strcat (lgstr, lg < 0 ? " W" : " E");

	(void) sprintf (buf, "(%s:) %d %d rstr (%s  %s) %d %d lstr\n",
					    tag, x, y, ltstr, lgstr, x+10, y);
	XPSDirect (buf);
}

/* called whenever the earth drawing area gets an expose.
 */
/* ARGSUSED */
static void
e_exp_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static unsigned int wid_last, hei_last;
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;
	Display *dsp = XtDisplay(w);
	Window win = XtWindow(w);
	Window root;
	int x, y;
	unsigned int bw, d;
	unsigned wid, hei;

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
	    printf ("Unexpected eform_w event. type=%d\n", c->reason);
	    exit(1);
	}

	XGetGeometry (dsp, win, &root, &x, &y, &wid, &hei, &bw, &d);

	/* if no pixmap or we're a new size then (re)build everything */
	if (!e_pm || wid != wid_last || hei != hei_last) {
	    if (e_pm)
		XFreePixmap (dsp, e_pm);

	    if (!e_gc)
		e_init_gcs(dsp, win);

	    e_pm = XCreatePixmap (dsp, win, wid, hei, d);
	    XSetForeground (dsp, e_gc, e_bg);
	    XFillRectangle (dsp, e_pm, e_gc, 0, 0, wid, hei);
	    wid_last = wid;
	    hei_last = hei;

	    e_all(mm_get_now());
	} else {
	    /* nothing new so just copy from the pixmap */
	    e_copy_pm();
	}
}

/* called on receipt of a MotionNotify or ButtonPress or ButtonRelease event
 *   while the cursor is wandering over the Earth and button 1 is down or
 *   Button3 is pressed.
 * button 1 causes a report of the current location in world coordinates;
 * button 3 causes a popup.
 */
/* ARGSUSED */
static void
e_motion_cb (w, client, ev, continue_to_dispatch)
Widget w;
XtPointer client;
XEvent *ev;
Boolean *continue_to_dispatch;
{
	int trackmotion, trackbutton, popupbutton;
	int evt = ev->type;


	trackmotion = evt == MotionNotify && ev->xmotion.state == Button1Mask;
	trackbutton = (evt == ButtonPress || evt == ButtonRelease)
					&& ev->xbutton.button == Button1;
	popupbutton = evt == ButtonPress && ev->xbutton.button == Button3;

	if (popupbutton || trackmotion || trackbutton) {
	    unsigned wide, h, r, xb, yb;
	    Window root, child;
	    int rx, ry, wx, wy;
	    unsigned mask;

	    XQueryPointer (XtDisplay(w), XtWindow(w),
				&root, &child, &rx, &ry, &wx, &wy, &mask);

	    e_getcircle (&wide, &h, &r, &xb, &yb);

	    if (popupbutton)
		e_popup (w, ev, r, xb, yb, wx, wy);
	    else
		e_track_latlng (w, ev, r, xb, yb, wx, wy);
	}
}

/* called when the one of the Object pulldown buttons is activated.
 * make that object the one to view now.
 * the object id is client.
 */
/* ARGSUSED */
static void
e_obj_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	Now *np = mm_get_now();
	Obj *op;
	int dbidx;

	dbidx = (int)client;
	if (!(op = db_basic (dbidx)) || op->type == UNDEFOBJ) {
	    printf ("e_obj_cb: bad dbidx: %d\n", dbidx);
	    exit (1);
	}

	object_dbidx = dbidx;
	set_xmstring (objcb_w, XmNlabelString, op->o_name);
	set_xmstring (esobjname_w, XmNlabelString, op->o_name);

	/* restart the trail history */
	(void) e_resettrail(np, 1);

	/* update display too if necessary */
	if (wants[OBJ] || wants[TRAIL])
	    e_all (np);
}

/* called when the pulldown menu from the Object cascade button is cascading.
 * check whether ObjX and Y are defined and if so set their names in the
 * pulldown else turn these selections off.
 */
/* ARGSUSED */
static void
e_objing_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	Obj *op;

	op = db_basic (OBJX);
	if (op->type != UNDEFOBJ) {
	    set_xmstring (objX_w, XmNlabelString, op->o_name);
	    XtManageChild (objX_w);
	} else
	    XtUnmanageChild (objX_w);

	op = db_basic (OBJY);
	if (op->type != UNDEFOBJ) {
	    set_xmstring (objY_w, XmNlabelString, op->o_name);
	    XtManageChild (objY_w);
	} else
	    XtUnmanageChild (objY_w);
}

/* called when any of the option toggle buttons changes state.
 * client is one of the Wants enum.
 */
/* ARGSUSED */
static void
e_tb_cb (wid, client, call)
Widget wid;
XtPointer client;
XtPointer call;
{
	Now *np = mm_get_now();
	int state = XmToggleButtonGetState (wid);
	int want = (int) client;
	unsigned int w, h, r, wb, hb;

	watch_cursor (1);

	/* set option to new value */
	wants[want] = state;

	/* prepare for graphics */
	e_getcircle (&w, &h, &r, &wb, &hb);

	/* generally we draw just what we need if turning on, everything if off.
	 * this is faster than e_all but can lead to a different overlay order.
	 */
	switch (want) {
	case TRAIL:
	    if (state) {
		e_drawtrail (r, wb, hb);
		e_copy_pm();
	    } else if (ntrails > 1) /* do nothing if no trail */
		e_all (np);
	    break;

	case TRACK:
	    if (state) {
		/* if turning on tracking, it's nice to also insure Show Object
		 * want is on too.
		 */
		if (!wants[OBJ]) {
		    /* don't call the callback -- that'd do two e_all()'s */
		    XmToggleButtonSetState (showobj_w, True, False);
		    wants[OBJ] = True;
		}
		e_all (np);
	    }
	    break;

	case GRID:
	    if (state) {
		e_drawgrid (r, wb, hb);
		e_copy_pm();
	    } else
		e_all (np);
	    break;

	case OBJ:
	    if (state) {
		e_drawobject (r, wb, hb);
		e_copy_pm();
	    } else
		e_all (np);
	    break;

	case SITES:
	    if (state) {
		if (sites_get_list(NULL) < 0) {
		    wants[SITES] = 0;
		    XmToggleButtonSetState (sites_w, False, False);
		    xe_msg ("No SITESFILE found.", 1);
		} else {
		    e_drawsites (r, wb, hb);
		    e_copy_pm();
		}
	    } else
		e_all (np);
	    break;

	case SUNLIGHT:	/* FALLTHRU */
	case LOWPREC:
	    /* these change too much to just do a portion */
	    e_all (np);
	    break;

	default:
	    printf ("e_tb_cb: bad client: %d\n", want);
	    exit (1);
	}

	watch_cursor (0);
}

/* called when the Trail PB is activated. */
/* ARGSUSED */
static void
e_trail_cb (wid, client, call)
Widget wid;
XtPointer client;
XtPointer call;
{
	tr_setup ("xephem Earth Trail setup", trails[0].t_obj.o_name, &trstate,
					e_mktrail, (XtPointer)object_dbidx);
}

/* called when either the cylindrical or spherical button changes state.
 * client will be 1 if the former, 0 if the latter.
 */
/* ARGSUSED */
static void
e_cyl_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmToggleButtonCallbackStruct *sp = (XmToggleButtonCallbackStruct *)call;
	int wantcyl;

	/* see which one we want */
	wantcyl = (int)client == sp->set;

	if (wantcyl) {
	    XmToggleButtonSetState (cyl_w, True, False);
	    XmToggleButtonSetState (sph_w, False, False);
	} else {
	    XmToggleButtonSetState (cyl_w, False, False);
	    XmToggleButtonSetState (sph_w, True, False);
	}

	/* update wants[] */
	wants[CYLINDRICAL] = wantcyl;

	/* recompute the new size.
	 * if no change we call e_all() else leave it up to the expose.
	 */
	if (e_set_dasize(wantcyl) == 0)
	    e_all (mm_get_now());
}

/* adjust the size of e_da_w for use as CYLINDRICAL or spherical viewing.
 * return 1 if the size really does change, else 0.
 */
static int
e_set_dasize(wantcyl)
int wantcyl;
{
	Dimension wid, hei;
	Dimension nwid, nhei;
	Arg args[10];
	double a;
	int n;

	n = 0;
	XtSetArg (args[n], XmNwidth, (XtArgVal)&wid); n++;
	XtSetArg (args[n], XmNheight, (XtArgVal)&hei); n++;
	XtGetValues (e_da_w, args, n);

	/* conserve area unless it would grow larger than screen. */
	a = (double)wid*(double)hei;
	if (wantcyl) {
	    /* spherical to cylindrical. */
	    int dw = DisplayWidth(XtD, 0);

	    nwid = (Dimension)(sqrt(a*CYLASPECT) + 0.5);
	    if ((int)nwid > dw) {
		nwid = (Dimension)dw;
		nhei = (Dimension)((double)nwid/CYLASPECT + 0.5);
	    } else
		nhei = (Dimension)((double)nwid/CYLASPECT + 0.5);
	} else {
	    /* cylindrical to spherical */
	    nwid = nhei = (Dimension)(sqrt(a) + 0.5);
	}

	if (nwid != wid || nhei != hei) {
	    /* change both at once to disuade multiple exposures */
	    n = 0;
	    XtSetArg (args[n], XmNwidth, (XtArgVal)nwid); n++;
	    XtSetArg (args[n], XmNheight, (XtArgVal)nhei); n++;
	    XtSetValues (e_da_w, args, n);
	    return (1);
	}

	return (0);
}

/* called when Set Main or Set from Main is activated.
 * client is set to SET_MAIN or SET_FROM_MAIN.
 */
/* ARGSUSED */
static void
e_setmain_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int which = (int)client;
	Now *np = mm_get_now();

	switch (which) {
	case SET_MAIN:
	    mm_setsite (elat, elng, eelev, esitename, 1);
	    break;
	case SET_FROM_MAIN:
	    e_setelatlng (lat, lng);
	    XmToggleButtonSetState (track_w, False, True);
	    e_all (np);
	    break;
	default:
	    printf ("e_setmain_cb: Bad client: %d\n", which);
	    exit (1);
	}
}

/* called when either the longitude or latitude scale moves.
 * doesn't matter which -- just update pointing direction and redraw.
 * as a convenience, also insure tracking is off now.
 * also forget any remembered sites name.
 */
/* ARGSUSED */
static void
e_latlong_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int lt, lg;

	XmScaleGetValue (lat_w, &lt);
	XmScaleGetValue (long_w, &lg);
	e_setelatlng (degrad(lt), degrad(-lg)); /* want +E but scale is +W */

	/* manually sliding lat or long certainly voids a valid site code */
	esitename = NULL;
	eelev = 0.0;

	XmToggleButtonSetState (track_w, False, True);

	e_all (mm_get_now());
}

/* called when the Point button is selected on the roaming popup.
 * get the details from pu_info.
 * center the point of view to point_lt/point_lg.
 * as a convenience, also turn off tracking.
 * save the site name, if there was one with this pick.
 */
/* ARGSUSED */
static void
e_point_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	e_setelatlng (pu_info.pu_lt, pu_info.pu_lg);
	XmToggleButtonSetState (track_w, False, True);
	esitename = pu_info.pu_site;	/* may be NULL */
	eelev = pu_info.pu_elev;

	e_all (mm_get_now());
}

/* make all the GCs, define all the e_*pix pixels and set the XFontStruct e_f
 *   from the track_w widget.
 * we get the color names and save the pixels in the global ecolors[] arrary.
 * if we can't get all of the special colors, just use foreground/background.
 * this is because we will use xor if if any color is missing.
 */
static void
e_init_gcs (dsp, win)
Display *dsp;
Window win;
{
	unsigned long gcm;
	XGCValues gcv;
	Pixel p;
	int all;
	int i;

	/* create the main gc. 
	 * it's GCForeground is set each time it's used.
	 */
	gcm = GCFunction;
	gcv.function = GXcopy;
	e_gc = XCreateGC (dsp, win, gcm, &gcv);

	/* get the foreground and background colors */
	get_something (e_da_w, XmNforeground, (XtArgVal)&e_fg);
	get_something (e_da_w, XmNbackground, (XtArgVal)&e_bg);

	/* collect all the other Earth colors, and check if we have them all.
	 * we also require them to all be unique.
	 */
	all = 1;
	for (i = 0; all && i < XtNumber(ecolors); i++) {
	    EColor *ecp = &ecolors[i];
	    int j;

	    if (get_color_resource (e_da_w, ecp->name, &ecp->p) < 0) {
		all = 0;
		break;
	    } else {
		for (j = i-1; j >= 0; --j) {
		    if (ecp->p == ecolors[j].p) {
			all = 0;
			break;
		    }
		}
	    }
	}

	/* make the overlay gc, e_olgc. it's GCFunction is set once (here)
	 * depending on whether we have all the desired earth colors or to xor
	 * if we must use fg/bg for everything.
	 */

	if (!all) {
	    /* couldn't get at least one color so use fg/bg and xor */
	    Pixel xorp = e_fg ^ e_bg;

	    xe_msg (
  "Can not get all unique Earth colors so using Earth.Map.fg/bg and xor.", 0);
	    for (i = 0; i < XtNumber(ecolors); i++)
		ecolors[i].p = xorp;

	    /* sun is not drawn with xor */
	    ecolors[SUNC].p = e_fg;

	    gcm = GCFunction;
	    gcv.function = GXxor;
	    e_olgc = XCreateGC (dsp, win, gcm, &gcv);
	} else {
	    /* got all the colors -- overlay can just be drawn directly */
	    gcm = GCFunction;
	    gcv.function = GXcopy;
	    e_olgc = XCreateGC (dsp, win, gcm, &gcv);
	}

	/* make the lat/long string gc and get's it's font */

	gcm = GCForeground | GCBackground | GCFont;
	get_something (track_w, XmNforeground, (XtArgVal)&p);
	gcv.foreground = p;
	gcv.background = e_bg;
	get_xmlabel_font (track_w, &e_f);
	gcv.font = e_f->fid;
	e_strgc = XCreateGC (dsp, win, gcm, &gcv);
}

/* copy the pixmap e_pm to the window of e_da_w. */
static void
e_copy_pm()
{
	Display *dsp = XtDisplay(e_da_w);
	Window win = XtWindow(e_da_w);
	unsigned int w, h;
	unsigned int wb, hb, r;

	e_getcircle (&w, &h, &r, &wb, &hb);
	XCopyArea (dsp, e_pm, win, e_gc, 0, 0, w, h, 0, 0);
}

/* right button has been hit: find the closest thing to x/y and display what we
 * know about it. if nothing is very close, at least show the lat/long.
 * always give the choice to point to the spot.
 */
/* ARGSUSED */
static void
e_popup (wid, ev, r, xb, yb, x, y)
Widget wid;
XEvent *ev;
unsigned r;
unsigned xb, yb;
int x, y;
{
	double lmt;			/* local mean time: UT - radhr(lng) */
	double gst, lst;		/* Greenich and local sidereal times */
	double A, b, cosc, sinc;	/* see solve_sphere() */
	double cosa;			/* see solve_sphere() */
	double lt, lg;
	double mind;
	Site *minsip;
	Trail *tp, *mintp;
	char buf[128];
	int want_altaz;
	Widget w;
	Now *np = mm_get_now();
	int i, nl;

	/* find the lat/long under the cursor.
	 * if not over the earth, forget it.
	 */
	if (!e_uncoord (r, xb, yb, x, y, &lt, &lg))
	    return;
	cosc = cos(PI/2 - lt);
	sinc = sin(PI/2 - lt);

	watch_cursor (1);

	/* make the popup if this is our first time */
	if (!pu_info.pu_w)
	    e_create_popup();

	/* find the closest site in minsip, if sites are enabled */
	minsip = NULL;
	mind = 1e9;
	if (wants[SITES]) {
	    Site **sipp;
	    int nssip;

	    for (nssip = sites_get_list(&sipp); --nssip >= 0; ) {
		Site *sip = sipp[nssip];
		double d;

		A = sip->si_lng - lg;
		b = PI/2 - sip->si_lat;
		solve_sphere (A, b, cosc, sinc, &cosa, NULL);
		d = acos (cosa);
		if (d < mind) {
		    minsip = sip;
		    mind = d;
		}
	    }
	}

	/* now see if there are any history entries even closer.
	 * if so, put it in mintp.
	 * remember: the object in its current position is also on trails[].
	 * don't do this if the object is not currently being shown.
	 */
	mintp = 0;
	if (wants[OBJ])
	    for (tp = trails; tp && tp < &trails[ntrails]; tp++) {
		double d;
		A = tp->t_sublng - lg;
		b = PI/2 - tp->t_sublat;
		solve_sphere (A, b, cosc, sinc, &cosa, NULL);
		d = acos (cosa);
		if (d < mind) {
		    mintp = tp;
		    mind = d;
		}
	    }

	/* build the popup */

	/* first unmanage all the labels -- we'll turn on what we want */
	for (i = 0; i < MAXPUL; i++)
	    XtUnmanageChild (pu_info.pu_labels[i]);
	nl = 0;

	/* assume no sites entry for now */
	pu_info.pu_site = NULL;
	pu_info.pu_elev = 0.0;

	want_altaz = 0;
	if (mind <= MAXDIST) {
	    /* found something, question is: a trailed object or a site?
	     * check them in the opposite order than which they were searched.
	     */
	    if (mintp) {
		/* a trail item was closest.
		 * put the name, time, lat and lng in the popup.
		 */
		w = pu_info.pu_labels[nl++];
		set_xmstring (w, XmNlabelString, mintp->t_obj.o_name);
		XtManageChild (w);

		w = pu_info.pu_labels[nl++];
		(void) sprintf (buf, "UT Date: ");
		fs_date (buf+strlen(buf), mjd_day(mintp->t_now.n_mjd));
		set_xmstring (w, XmNlabelString, buf);
		XtManageChild (w);

		w = pu_info.pu_labels[nl++];
		(void) sprintf (buf, "UT Time: ");
		fs_time (buf+strlen(buf), mjd_hr(mintp->t_now.n_mjd));
		set_xmstring (w, XmNlabelString, buf);
		XtManageChild (w);

		w = pu_info.pu_labels[nl++];
		(void) sprintf (buf, "LMT: ");
		lmt = mjd_hr(mintp->t_now.n_mjd) + radhr(mintp->t_sublng);
		range (&lmt, 24.0);
		fs_time (buf+strlen(buf), lmt);
		set_xmstring (w, XmNlabelString, buf);
		XtManageChild (w);

		w = pu_info.pu_labels[nl++];
		(void) sprintf (buf, "LST: ");
		utc_gst (mjd_day(mintp->t_now.n_mjd),
					    mjd_hr(mintp->t_now.n_mjd), &gst);
		lst = gst + radhr(mintp->t_sublng);
		range (&lst, 24.0);
		fs_time (buf+strlen(buf), lst);
		set_xmstring (w, XmNlabelString, buf);
		XtManageChild (w);

		w = pu_info.pu_labels[nl++];
		(void) sprintf (buf, "Sub Lat: ");
		fs_dms_angle (buf+strlen(buf), mintp->t_sublat);
		set_xmstring (w, XmNlabelString, buf);
		XtManageChild (w);

		w = pu_info.pu_labels[nl++];
		(void) sprintf (buf, "Sub Long: ");
		fs_dms_angle (buf+strlen(buf), -mintp->t_sublng); /* want +W */
		set_xmstring (w, XmNlabelString, buf);
		XtManageChild (w);

		/* save lat/long for Point and if want to display alt/az */
		pu_info.pu_lt = mintp->t_sublat;
		pu_info.pu_lg = mintp->t_sublng;
	    } else if (minsip) {
		/* a site entry was closest.
		 * put it's name, lat/long and alt/az in the popup.
		 */

		want_altaz = 1;

		w = pu_info.pu_labels[nl++];
		set_xmstring (w, XmNlabelString, minsip->si_name);
		XtManageChild (w);

		w = pu_info.pu_labels[nl++];
		(void) sprintf (buf, "LMT: ");
		lmt = mjd_hr(mjd) + radhr(minsip->si_lng);
		range (&lmt, 24.0);
		fs_time (buf+strlen(buf), lmt);
		set_xmstring (w, XmNlabelString, buf);
		XtManageChild (w);

		w = pu_info.pu_labels[nl++];
		(void) sprintf (buf, "LST: ");
		utc_gst (mjd_day(mjd), mjd_hr(mjd), &gst);
		lst = gst + radhr(minsip->si_lng);
		range (&lst, 24.0);
		fs_time (buf+strlen(buf), lst);
		set_xmstring (w, XmNlabelString, buf);
		XtManageChild (w);

		w = pu_info.pu_labels[nl++];
		(void) sprintf (buf, "Lat: ");
		fs_dms_angle (buf+strlen(buf), minsip->si_lat);
		set_xmstring (w, XmNlabelString, buf);
		XtManageChild (w);

		w = pu_info.pu_labels[nl++];
		(void) sprintf (buf, "Long: ");
		fs_dms_angle (buf+strlen(buf), -minsip->si_lng); /* want +W */
		set_xmstring (w, XmNlabelString, buf);
		XtManageChild (w);

		/* save lat/long for Point and if want to display alt/az */
		pu_info.pu_lt = minsip->si_lat;
		pu_info.pu_lg = minsip->si_lng;
		/* save the name and elevation too */
		pu_info.pu_site = minsip->si_name;
		pu_info.pu_elev = minsip->si_elev;
	    } else {
		printf ("e_popup: unknown closest! mind = %g\n", mind);
		exit (1);
	    }
	} else {
	    /* nothing was close enough.
	     * just put the lat/long and alt/az of the cursor loc in the popup.
	     */

	    want_altaz = 1;

	    w = pu_info.pu_labels[nl++];
	    (void) sprintf (buf, "LMT: ");
	    lmt = mjd_hr(mjd) + radhr(lg);
	    range (&lmt, 24.0);
	    fs_time (buf+strlen(buf), lmt);
	    set_xmstring (w, XmNlabelString, buf);
	    XtManageChild (w);

	    w = pu_info.pu_labels[nl++];
	    (void) sprintf (buf, "LST: ");
	    utc_gst (mjd_day(mjd), mjd_hr(mjd), &gst);
	    lst = gst + radhr(lg);
	    range (&lst, 24.0);
	    fs_time (buf+strlen(buf), lst);
	    set_xmstring (w, XmNlabelString, buf);
	    XtManageChild (w);

	    w = pu_info.pu_labels[nl++];
	    (void) sprintf (buf, "Lat: ");
	    fs_dms_angle (buf+strlen(buf), lt);
	    set_xmstring (w, XmNlabelString, buf);
	    XtManageChild (w);

	    w = pu_info.pu_labels[nl++];
	    (void) sprintf (buf, "Long: ");
	    fs_dms_angle (buf+strlen(buf), -lg);	/* want +W */
	    set_xmstring (w, XmNlabelString, buf);
	    XtManageChild (w);

	    /* save lat/long for Point and if want to display alt/az */
	    pu_info.pu_lt = lt;
	    pu_info.pu_lg = lg;
	}

	/* if the Object is enabled also display alt/az of the object from
	 * this location in the popup.
	 */
	if (want_altaz && wants[OBJ]) {
	    Now now_here;
	    Obj obj_here;

	    (void) memcpy ((void *)&now_here, (void *)&trails[0].t_now,
	    							sizeof (Now));
	    now_here.n_lat = pu_info.pu_lt;
	    now_here.n_lng = pu_info.pu_lg;
	    (void) memcpy ((void *)&obj_here, (void *)&trails[0].t_obj,
	    							sizeof (Obj));
	    (void) obj_cir (&now_here, &obj_here);

	    w = pu_info.pu_labels[nl++];
	    (void) sprintf (buf, "Obj Alt: ");
	    fs_pangle (buf+strlen(buf), obj_here.s_alt);
	    set_xmstring (w, XmNlabelString, buf);
	    XtManageChild (w);

	    w = pu_info.pu_labels[nl++];
	    (void) sprintf (buf, "Obj Az: ");
	    fs_pangle (buf+strlen(buf), obj_here.s_az);
	    set_xmstring (w, XmNlabelString, buf);
	    XtManageChild (w);
	}

	XmMenuPosition (pu_info.pu_w, (XButtonPressedEvent *)ev);
	XtManageChild (pu_info.pu_w);

	watch_cursor(0);
}

/* create the popup
 * save all its widgets in the pu_info struct.
 */
static void
e_create_popup()
{
	Widget w;
	Arg args[20];
	int n;
	int i;

	/* make the popup shell which includes a handy row/column */
	n = 0;
	XtSetArg (args[n], XmNisAligned, True); n++;
	XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
	pu_info.pu_w = XmCreatePopupMenu (e_da_w, "EPopup", args, n);

	/* add the max labels we'll ever need -- all unmanaged for now. */
	for (i = 0; i < MAXPUL; i++) {
	    n = 0;
	    pu_info.pu_labels[i] = XmCreateLabel (pu_info.pu_w, "EPUL",args,n);
	}

	/* and add the separator and the "Point" button.
	 * we always wants these so manage them now.
	 */
	w = XmCreateSeparator (pu_info.pu_w, "PUSep", args, 0);
	XtManageChild (w);
	w = XmCreatePushButton (pu_info.pu_w, "Point", args, 0);
	XtAddCallback (w, XmNactivateCallback, e_point_cb, 0);
	XtManageChild (w);
}

/* display the lat/long/lmt/lst under the cursor.
 * we are given the radius and x/y of the location, the pointer event and
 * the total window size.
 */
static void
e_track_latlng (w, ev, r, xb, yb, x, y)
Widget w;
XEvent *ev;
unsigned r;
unsigned xb, yb;
int x, y;
{
	static dirty;
	double lt, lg;
	int wide = 2*(r+xb);
	int hi = 2*(r+yb);

	if (ev->type != ButtonRelease && e_uncoord (r, xb, yb, x, y, &lt, &lg)){
	    XCharStruct all;
	    Now *np = mm_get_now();
	    Window win = XtWindow(w);
	    char lst_buf[64], lmt_buf[64], lng_buf[64], lat_buf[64];
	    double lst, gst, lmt;
	    int dir, asc, des;
	    int len;

	    (void) strcpy (lat_buf, "Lat: ");
	    fs_dms_angle (lat_buf+5, lt);

	    (void) strcpy (lng_buf, "Long: ");
	    fs_dms_angle (lng_buf+6, -lg);	/* want +W */

	    lmt = mjd_hr(mjd) + radhr(lg);
	    range (&lmt, 24.0);
	    (void) strcpy (lmt_buf, "LMT: ");
	    fs_time (lmt_buf+5, lmt);

	    utc_gst (mjd_day(mjd), mjd_hr(mjd), &gst);
	    lst = gst + radhr(lg);
	    range (&lst, 24.0);
	    (void) strcpy (lst_buf, "LST: ");
	    fs_time (lst_buf+5, lst);

	    len = strlen (lat_buf);
	    XTextExtents (e_f, lat_buf, len, &dir, &asc, &des, &all);
	    XDrawImageString (XtD, win, e_strgc, 0, asc, lat_buf, len); 

	    len = strlen (lng_buf);
	    XTextExtents (e_f, lng_buf, len, &dir, &asc, &des, &all);
	    XDrawImageString (XtD, win, e_strgc, wide-all.rbearing, asc,
								lng_buf, len);

	    len = strlen (lmt_buf);
	    XTextExtents (e_f, lmt_buf, len, &dir, &asc, &des, &all);
	    XDrawImageString (XtD, win, e_strgc, 0, hi-des, lmt_buf, len);

	    len = strlen (lst_buf);
	    XTextExtents (e_f, lst_buf, len, &dir, &asc, &des, &all);
	    XDrawImageString (XtD, win, e_strgc, wide-all.rbearing, hi-des,
								lst_buf, len);
	    
	    dirty = 1;
	} else if (dirty) {
	    e_copy_pm();
	    dirty = 0;
	}
}

/* set the elat/selat/celat/elng values, taking care to put them into ranges
 * -PI/2 .. elat .. PI/2 and -PI .. elng .. PI.
 */
static void
e_setelatlng (lt, lg)
double lt, lg;
{
	elat = lt;
	selat = sin(elat);
	celat = cos(elat);
	elng = lg;
	range (&elng, 2*PI);
	if (elng > PI)
	    elng -= 2*PI;
}

/* return the lat/lng of the subobject point (lat/long of where the object
 * is directly overhead) at np.
 * both are in rads, lat is +N, long is +E.
 */
static void
e_subobject(np, op, latp, longp)
Now *np;
Obj *op;
double *latp, *longp;
{
	if (op->type == EARTHSAT) {
	    /* these are all ready to go */
	    *latp = op->s_sublat;
	    *longp = op->s_sublng;
	} else {
	    double gst;
	    double ra, dec;
	    ra = op->s_ra;
	    dec = op->s_dec;
	    if (epoch != EOD)
		precess (epoch, mjd, &ra, &dec);

	    *latp = dec;

	    utc_gst (mjd_day(mjd), mjd_hr(mjd), &gst);
	    *longp = ra - hrrad(gst);	/* remember: we want +E */
	    range (longp, 2*PI);
	    if (*longp > PI)
		*longp -= 2*PI;
	}
}

/* show the stats for the object in its current position.
 * if it's of type EARTHSAT then display its s_ fields;
 * otherwise turn off the display of these items.
 */
static void
e_show_esat_stats ()
{
	ESatStatTable *stp;
	int unitspref;
	Obj *op = &trails[0].t_obj;

	if (op->type != EARTHSAT) {
	    XtUnmanageChild (estattable[ES_ELEV].lw);
	    XtUnmanageChild (estattable[ES_ELEV].pbw);
	    XtUnmanageChild (estattable[ES_RANGE].lw);
	    XtUnmanageChild (estattable[ES_RANGE].pbw);
	    XtUnmanageChild (estattable[ES_RANGER].lw);
	    XtUnmanageChild (estattable[ES_RANGER].pbw);
	    XtUnmanageChild (estattable[ES_ECLIPSED].lw);
	    XtUnmanageChild (estattable[ES_ECLIPSED].pbw);
	    return;
	} else {
	    XtManageChild (estattable[ES_ELEV].lw);
	    XtManageChild (estattable[ES_ELEV].pbw);
	    XtManageChild (estattable[ES_RANGE].lw);
	    XtManageChild (estattable[ES_RANGE].pbw);
	    XtManageChild (estattable[ES_RANGER].lw);
	    XtManageChild (estattable[ES_RANGER].pbw);
	    XtManageChild (estattable[ES_ECLIPSED].lw);
	    XtManageChild (estattable[ES_ECLIPSED].pbw);
	}

	unitspref = pref_get (PREF_UNITS);

	stp = &estattable[ES_ELEV];
	if (stp->enumcode != ES_ELEV) {
	    printf ("estattable order is wrong\n");
	    exit(1);
	}
	if (unitspref == PREF_ENGLISH) {
	    f_showit (stp->lw, "Elevation(mi):");
	    f_double (stp->pbw, "%6.0f", op->s_elev*FTPM/5280.0);
	} else {
	    f_showit (stp->lw, "Elevation(km):");
	    f_double (stp->pbw, "%6.0f", op->s_elev/1000.0);
	}

	stp = &estattable[ES_RANGE];
	if (stp->enumcode != ES_RANGE) {
	    printf ("estattable order is wrong\n");
	    exit(1);
	}
	if (unitspref == PREF_ENGLISH) {
	    f_showit (stp->lw, "Range(mi):");
	    f_double (stp->pbw, "%6.0f", op->s_range*FTPM/5280.0);
	} else {
	    f_showit (stp->lw, "Range(km):");
	    f_double (stp->pbw, "%6.0f", op->s_range/1000.0);
	}

	stp = &estattable[ES_RANGER];
	if (stp->enumcode != ES_RANGER) {
	    printf ("estattable order is wrong\n");
	    exit(1);
	}
	if (unitspref == PREF_ENGLISH) {
	    f_showit (stp->lw, "Range Rate(f/s):");
	    f_double (stp->pbw, "%6.0f", op->s_rangev*FTPM);
	} else {
	    f_showit (stp->lw, "Range Rate(m/s):");
	    f_double (stp->pbw, "%6.0f", op->s_rangev);
	}

	stp = &estattable[ES_ECLIPSED];
	if (stp->enumcode != ES_ECLIPSED) {
	    printf ("estattable order is wrong\n");
	    exit(1);
	}
	f_double (stp->pbw, "%.1g", op->s_eclipsed ? 1.0 : 0.0);
}

/* given:
 *   the current viewpoint coords (from globals selat/celat/elng)
 *   radius and borders of circle in current window
 *   latitude and longitude of a point p: pl, pL;
 * find:
 *   x and y coords of point on screen
 * always return the coordinates, but return 1 if the point is visible or 0 if
 *   it's over the limb.
 */   
static int
e_coord (r, wb, hb, pl, pL, xp, yp)
unsigned r;		/* radius of drawing surface circle */
unsigned wb, hb;	/* width/height borders between circle and edges */
double pl, pL;		/* point p lat, rads, +N; long, rads, +E */
short *xp, *yp;		/* projected location onto drawing surface */
{
	if (wants[CYLINDRICAL]) {
	    unsigned w = 2*(r + wb);
	    unsigned h = 2*(r + hb);
	    double dL;

	    *yp = (int)floor(h/2.0 * (1.0 - sin(pl)) + 0.5);

	    dL = pL - elng;
	    range(&dL, 2*PI);
	    if (dL > PI)  dL -= 2*PI;
	    *xp = (int)floor(w/2.0 * (1.0 + dL/PI) + 0.5);

	    return (1);
	} else {
	    double sR, cR;	/* R is radius from viewpoint to p */
	    double A, sA,cA;/* A is azimuth of p as seen from viewpoint */

	    solve_sphere (pL - elng, PI/2 - pl, selat, celat, &cR, &A);
	    sR = sqrt(1.0 - cR*cR);
	    sA = sin(A);
	    cA = cos(A);

	    *xp = wb + r + (int)floor(r*sR*sA + 0.5);
	    *yp = hb + r - (int)floor(r*sR*cA + 0.5);

	    return (cR > 0);
	}
}

/* draw circles onto e_pm within circle of radius r and width and height
 * borders wb and hb showing where the satellite is seen as being 0, 30 and 60
 * degrees altitude above horizon as well.
 * also draw a crosshair at the actual location.
 */
static void
e_drawfootprint (r, wb, hb, slat, slng, el)
unsigned r, wb, hb;	/* radius and width/height borders of circle to use */
double slat, slng;	/* satellite's lat and lng */
double el;		/* satellite's elevation above surface, m */
{
	double rad;	/* viewing altitude radius, rads */

	e_viewrad (el, degrad(0.0), &rad);
	e_drawcircle (r, wb, hb, slat, slng, rad);
	e_viewrad (el, degrad(30.0), &rad);
	e_drawcircle (r, wb, hb, slat, slng, rad);
	e_viewrad (el, degrad(60.0), &rad);
	e_drawcircle (r, wb, hb, slat, slng, rad);
	e_drawcross (r, wb, hb, slat, slng, CROSSH);
}

/* assuming the current vantage point of elat/elng, draw a circle around the
 *   given lat/long point of given angular radius.
 * all angles in rads.
 * we do it by sweeping an arc from the given location and collecting the end
 *   points. beware going over the horizon or wrapping around the edge.
 */
static void
e_drawcircle (r, wb, hb, slat, slng, rad)
unsigned r, wb, hb;	/* radius and width/height borders of circle to use */
double slat, slng;	/* lat/long of object */
double rad;		/* angular radius of circle to draw */
{
#define	MAXVCIRSEGS	52
	XPoint xp[MAXVCIRSEGS+1];
	double cosc = cos(PI/2 - slat);	/* cos/sin of N-pole-to-slat angle */
	double sinc = sin(PI/2 - slat);
	double b = rad;			/* sat-to-arc angle */
	double A;			/* subobject azimuth */
	double cosa;			/* cos of pole-to-arc angle */
	double B;			/* subobj-to-view angle from pole */
	int nvcirsegs = wants[LOWPREC] ? MAXVCIRSEGS/4 : MAXVCIRSEGS;
	int nxp = 0;
	int w = 2*(r+wb);
	int vis;
	int i;

	/* use the overlay gc, wide lines, the OBJC color */
	XSetLineAttributes (XtD, e_olgc, WIDE_LW, LINE_ST, CAP_ST, JOIN_ST);
	XSetForeground (XtD, e_olgc, ecolors[OBJC].p);

	for (i = 0; i <= nvcirsegs; i++) {
	    short x, y;
	    A = 2*PI/nvcirsegs * i;
	    solve_sphere (A, b, cosc, sinc, &cosa, &B);
	    vis = e_coord (r, wb, hb, PI/2-acos(cosa), B+slng, &x, &y);

	    nxp = add_to_polyline (xp, XtNumber(xp), i,vis,nxp,nvcirsegs,w,x,y);
	}
}

/* draw a little crosshair or plussign at the given location with the e_olgc */
static void
e_drawcross (r, wb, hb, lt, lg, style)
unsigned r, wb, hb;	/* radius and width/height borders of circle to use */
double lt, lg;		/* desired center location */
int style;		/* CROSSH or PLUSS */
{
	double lats[4], lngs[4];	/* lats and longs of endpoints */
	XSegment xs[4];			/* one for each cardinal direction */
	double a, cosa;			/* north-to-cross from view */
	double B;			/* cross-width from north */
	short sx, sy;			/* x and y of the center */
	int w = 2*(r+wb);
	int linew;
	int nxs;
	int vis;
	int xwrap;
	int i;

	/* find location of center of cross-hair */
	vis = e_coord (r, wb, hb, lt, lg, &sx, &sy);
	if (!vis)
	    return;	/* center is not visible so forget the rest */

	/* find longitude sweep to produce given e/w arc */

	switch (style) {
	case CROSSH:
	    solve_sphere (PI/4, CHLEN, sin(lt), cos(lt), &cosa, &B);
	    a = acos(cosa);
	    lats[0] = PI/2-a;		lngs[0] = lg+B;
	    lats[1] = PI/2-a;		lngs[1] = lg-B;
	    solve_sphere (3*PI/4, CHLEN, sin(lt), cos(lt), &cosa, &B);
	    a = acos(cosa);
	    lats[2] = PI/2-a;		lngs[2] = lg+B;
	    lats[3] = PI/2-a;		lngs[3] = lg-B;
	    linew = WIDE_LW;
	    break;
	case PLUSS:
	    solve_sphere (PI/2, PLLEN, sin(lt), cos(lt), &cosa, &B);
	    a = acos(cosa);
	    lats[0] = PI/2-a;		lngs[0] = lg+B;
	    lats[1] = PI/2-a;		lngs[1] = lg-B;
	    lats[2] = lt-PLLEN;		lngs[2] = lg;
	    if (lats[2] < -PI/2) {
		lats[2] = -PI - lats[2];lngs[2] += PI;	/* went under S pole */
	    }
	    lats[3] = lt+PLLEN;		lngs[3] = lg;
	    if (lats[3] > PI/2) {
		lats[3] = PI - lats[3];	lngs[3] += PI;	/* went over N pole */
	    }
	    linew = NARROW_LW;
	    break;
	default:
	    printf ("e_drawcross: bad style: %d\n", style);
	    exit (1);
	}

	nxs = 0;
	for (i = 0; i < 4; i++) {
	    short x, y;
	    vis = e_coord (r, wb, hb, lats[i], lngs[i], &x, &y);

	    if (wants[CYLINDRICAL] && (sx - x > w/2 || x - sx > w/2))
		xwrap = 1;
	    else
		xwrap = 0;

	    if (vis && !xwrap) {
		xs[nxs].x1 = x;
		xs[nxs].y1 = y;
		xs[nxs].x2 = sx;
		xs[nxs].y2 = sy;
		nxs++;
	    }
	}

	if (nxs > 0) {
	    XSetLineAttributes (XtD, e_olgc, linew, LINE_ST, CAP_ST, JOIN_ST);
	    XPSDrawSegments (XtD, e_pm, e_olgc, xs, nxs);
	}
}

/* draw the lat/long grid lines */
static void
e_drawgrid (r, wb, hb)
unsigned r, wb, hb;	/* radius and width/height borders of circle to use */
{
#define	MAXGRIDSEGS	60		/* max segments per grid line */
	XPoint xp[MAXGRIDSEGS+1];
	int w = 2*(r+wb);
	int nsegs = wants[LOWPREC] ? MAXGRIDSEGS/5 : MAXGRIDSEGS;
	int i, j;

	/* use the overlay GC, narrow lines, GRIDC color */
	XSetLineAttributes (XtD, e_olgc, NARROW_LW, LINE_ST, CAP_ST, JOIN_ST);
	XSetForeground (XtD, e_olgc, ecolors[GRIDC].p);

	/* draw each line of constant longitude */
	for (i = 0; i < 360/LNGSTEP; i++) {
	    double lg = i * degrad(LNGSTEP);
	    int nxp = 0;
	    for (j = 0; j <= nsegs; j++) {
		short x, y;
		double lt = degrad (90 - j*180.0/nsegs);
		int vis = e_coord (r, wb, hb, lt, lg, &x, &y);
		nxp = add_to_polyline (xp,XtNumber(xp),j,vis,nxp,nsegs,w,x,y);
	    }
	}

	/* draw each line of constant latitude -- beware of x wrap */
	for (i = 1; i < 180/LATSTEP; i++) {
	    double lt = degrad (90 - i*LATSTEP);
	    int nxp = 0;
	    for (j = 0; j <= nsegs; j++) {
		short x, y;
		double lg = j * degrad(360.0/nsegs);
		int vis = e_coord (r, wb, hb, lt, lg, &x, &y);
		nxp = add_to_polyline (xp,XtNumber(xp),j,vis,nxp,nsegs,w,x,y);
	    }
	}
}

/* given radius and size of window and X-coord x/y, return current Earth
 * lat/long.
 * return 1 if inside the circle, else 0.
 */
static int
e_uncoord (r, xb, yb, x, y, ltp, lgp)
unsigned int r;		/* radius of earth limb, pixels */
unsigned xb, yb;	/* borders around circle */
int x, y;		/* X-windows coords of loc */
double *ltp, *lgp;	/* resulting lat/long, rads */
{
	if (wants[CYLINDRICAL]) {
	    int maxx = 2*(r + xb)-1;
	    int maxy = 2*(r + yb)-1;

	    if (x < 0 || x > maxx || y < 0 || y > maxy)
		return (0);

	    *ltp = asin (1.0 - 2.0*y/maxy);

	    *lgp = 2*PI*(x - maxx/2)/maxx + elng;
	    if (*lgp >  PI) *lgp -= 2*PI;
	    if (*lgp < -PI) *lgp += 2*PI;

	    return (1);
	} else {
	    double X, Y;/* x/y but signed from center +x right +y up, pixels*/
	    double R;	/* pixels x/y is from center */
	    double b;	/* angle from viewpoint to x/y */
	    double A;	/* angle between N pole and x/y as seen from viewpt */
	    double a, ca;	/* angle from N pole to x/y */
	    double B;	/* angle from viewpoint to x/y as seen from N pole */

	    X = x - (int)(r + xb);
	    Y = (int)(r + yb) - y;
	    R = sqrt (X*X + Y*Y);
	    if (R >= r)
		return (0);

	    if (R < 1.0) {
		*ltp = elat;
		*lgp = elng;
	    } else {
		b = asin (R/r);
		A = atan2 (X, Y);
		solve_sphere (A, b, selat, celat, &ca, &B);
		a = acos (ca);
		*ltp = PI/2 - a;
		*lgp = elng + B;
		range (lgp, 2*PI);
		if (*lgp > PI)
		    *lgp -= 2*PI;
	    }
	    
	    return (1);
	}
}

/* draw everything: fields (always) and the picture (if managed) */
static void
e_all (np)
Now *np;
{
	double objlat, objlng;

	watch_cursor(1);

	if (!trails) {
	    printf ("e_all(): no trails\n");
	    exit (1);
	}

	/* put up the current time */
	timestamp (np, e_dt_w);
	timestamp (np, e_sdt_w);

	/* get the current location of the object */
	objlat = trails[0].t_sublat;
	objlng = trails[0].t_sublng;

	/* point at it if tracking is on */
	if (wants[TRACK])
	    e_setelatlng (objlat, objlng);

	/* update the scale -- remember we want +W longitude */
	XmScaleSetValue (lat_w, (int)floor(raddeg(elat)+0.5));
	XmScaleSetValue (long_w, (int)floor(raddeg(-elng)+0.5));

	/* display the lat and long of the current object.
	 * N.B. we want to display longitude as +W in range of -PI..+PI
	 */
	f_dms_angle (estattable[ES_SUBLONG].pbw, -objlng);
	f_dms_angle (estattable[ES_SUBLAT].pbw, objlat);

	/* show the other stats too if applicable */
	e_show_esat_stats ();

	/* display the picture if we are up now for sure now.
	 * N.B. don't just use whether eform_w is managed: if we are looping
	 * and get managed drawing occurs before the first expose makes the pm.
	 */
	if (e_pm) {
	    e_map (np);
	    e_copy_pm();
	}

	watch_cursor(0);
}

/* find size and borders of circle for drawing earth */
static void
e_getcircle (wp, hp, rp, xbp, ybp)
unsigned int *wp, *hp;          /* overall width and height */
unsigned int *rp;               /* circle radius */
unsigned int *xbp, *ybp;        /* x and y border */
{
	Display *dsp = XtDisplay(e_da_w);
	Window win = XtWindow(e_da_w);
	Window root;
	int x, y;
	unsigned int w, h;
	unsigned int bw, d;

	XGetGeometry (dsp, win, &root, &x, &y, wp, hp, &bw, &d);

	w = *wp/2;
	h = *hp/2;
	*rp = w > h ? h : w;    /* radius */
	*xbp = w - *rp;         /* x border */
	*ybp = h - *rp;         /* y border */

}

/* draw everything centered at elat/elng onto e_pm.
 */
static void
e_map (np)
Now *np;
{
	Display *dsp = XtDisplay(e_da_w);
	unsigned int w, h, r, wb, hb;

	/* get size of the rendering area */
	e_getcircle (&w, &h, &r, &wb, &hb);

	/* clear the window and draw initial border */
	XSetForeground (dsp, e_gc, e_bg);
	XFillRectangle (dsp, e_pm, e_gc, 0, 0, w, h);

	/* draw outter circle if in spherical mode */
	if (!wants[CYLINDRICAL]) {
	    XSetForeground (dsp, e_gc, e_fg);
	    XPSDrawArc (dsp, e_pm, e_gc, wb, hb, 2*r+1, 2*r+1, 0, 360*64);
	}

	/* draw sunlit portion of earth.
	 * not drawn with xor so put it first.
	 */
	if (wants[SUNLIGHT])
	    e_sunlit (np, r, wb, hb);

	/* draw coord grid */
	if (wants[GRID])
	    e_drawgrid (r, wb, hb);

	/* draw each continent border */
	e_drawcontinents (r, wb, hb);

	/* draw each site */
	if (wants[SITES])
	    e_drawsites (r, wb, hb);

	/* draw the main object */
	if (wants[OBJ])
	    e_drawobject (r, wb, hb);

	/* draw any trails */
	if (wants[TRAIL])
	    e_drawtrail (r, wb, hb);

	/* mark the mainmenu location */
	e_mainmenuloc (np, r, wb, hb);

	/* mark any solar eclipse location */
	e_soleclipse (np, r, wb, hb);
}

/* draw the portion of the earth lit by the sun */
static void
e_sunlit (np, r, wb, hb)
Now *np;		/* circumstances */
unsigned int r, wb, hb;	/* circle radius, width and height borders in pixmap */
{
	if (wants[CYLINDRICAL])
	    e_msunlit (np, r, wb, hb);
	else
	    e_ssunlit (np, r, wb, hb);
}

/* draw the sunlit portion of the Earth in cylindrical projection.
 */
static void
e_msunlit (np, r, wb, hb)
Now *np;		/* circumstances */
unsigned int r, wb, hb;	/* circle radius, width and height borders in pixmap */
{
#define MAXNMPTS	52	/* n segments -- larger is finer but slower */
	Display *dsp = XtDisplay(e_da_w);
	XPoint xp[MAXNMPTS+4];	/* extra room for top or bottom box */
	double sslat, sslong;	/* subsolar lat/long, rads, +N +E */
	double az0, daz;	/* initial and delta az so we move +x */
	int nmpts = wants[LOWPREC] ? MAXNMPTS/4 : MAXNMPTS;
	double ssl, csl;
	int w = 2*(r+wb);
	int h = 2*(r+hb);
	short x, y;
	int yi;
	int wrapn, i;

	e_subobject (np, db_basic(SUN), &sslat, &sslong);
	ssl = sin(sslat);
	csl = cos(sslat);

	if (sslat < 0) {
	    az0 = 0;
	    daz = 2*PI/nmpts;
	} else {
	    az0 = PI;
	    daz = -2*PI/nmpts;
	}

	/* fill in the circle of nmpts points that lie PI/2 from the
	 *   subsolar point and projecting from the viewpoint.
	 * when wrap, shuffle points up so final polygon doesn't wrap.
	 */
	XSetForeground (dsp, e_gc, ecolors[SUNC].p);
	wrapn = 0;
	for (i = 0; i < nmpts; i++) {
	    double A = az0 + i*daz;		/* azimuth of p from subsol */
	    double sA = sin(A), cA = cos(A);
	    double plat = asin (csl*cA);	/* latitude of p */
	    double plong = atan2(sA, -ssl*cA) + sslong;	/* long of p */
	    XPoint *xpp = &xp[i-wrapn];

	    (void) e_coord (r, wb, hb, plat, plong, &x, &y);

	    xpp->x = x;
	    xpp->y = y;

	    if (wrapn == 0 && i > 0 && x < xpp[-1].x) {
		/* when wrap copy points so far to end and this one is first.
		 * N.B. go backwards in case we are over half way.
		 */
		for ( ; wrapn < i; wrapn++)
		    xp[nmpts-1-wrapn] = xp[i-1-wrapn];
		xp[0].x = x;
		xp[0].y = y;
	    }
	}

	/* y-intercept at the edges is the average of the y's at each end */
	yi = (xp[0].y + xp[nmpts-1].y)/2;

	/* complete polygon across bottom if sun is below equator, or vv */
	if (sslat < 0) {
	    xp[i].x = w;    xp[i].y = yi;    i++;    /* right */
	    xp[i].x = w;    xp[i].y = h;     i++;    /* down */
	    xp[i].x = 0;    xp[i].y = h;     i++;    /* left */
	    xp[i].x = 0;    xp[i].y = yi;    i++;    /* up */
	} else {
	    xp[i].x = w;    xp[i].y = yi;    i++;    /* right */
	    xp[i].x = w;    xp[i].y = 0;     i++;    /* up */
	    xp[i].x = 0;    xp[i].y = 0;     i++;    /* left */
	    xp[i].x = 0;    xp[i].y = yi;    i++;    /* down */
	}

	XPSFillPolygon (dsp, e_pm, e_gc, xp, i, Complex, CoordModeOrigin);
}

/* draw the solid gibbous or crescent sun-lit portion of the Earth in spherical
 * projection.
 */
static void
e_ssunlit (np, r, wb, hb)
Now *np;		/* circumstances */
unsigned int r, wb, hb;	/* circle radius, width and height borders in pixmap */
{
#define MAXNSPTS	52	/* max number of polyline points */
#define	FULLANGLE	degrad(5)/* consider full if within this angle */
	Display *dsp = XtDisplay(e_da_w);
	double sslat, sslong;	/* subsolar lat/long, rads, +N +E */
	double ssl, csl;	/* sin/cos of sslat */
	int dx, dy;		/* slope from center to subsolar point */
	double ca;		/* angular sep from viewpoint to subsolar */
	double B;		/* cw angle up from center to subsolar point */
	int maxpts = wants[LOWPREC] ? MAXNSPTS/4 : MAXNSPTS;
	XPoint xp[MAXNSPTS];
	int npts;
	int i;

	/* find where the sun is */
	e_subobject (np, db_basic(SUN), &sslat, &sslong);

	/* set up color */
	XSetForeground (dsp, e_gc, ecolors[SUNC].p);

	/* find direction and length of line from center to subsolar point */
	solve_sphere (sslong - elng, PI/2-sslat, selat, celat, &ca, &B);

	/* check for special cases of eclipsed and directly over subsolar pt */
	if (acos(ca) < FULLANGLE) {
	    short x, y;
	    int vis = e_coord (r, wb, hb, sslat, sslong, &x, &y);
	    if (vis)
		XPSFillArc (dsp, e_pm, e_gc, wb, hb, 2*r, 2*r, 0, 360*64);
	    return;
	}

	/* find direction of line parallel to line from center to subsolar
	 * location and long enough to be well outside perimeter.
	 */
	dx = floor(3.0*r*sin(B) + 0.5);	/* +x to right */
	dy = floor(3.0*r*cos(B) + 0.5);	/* +y up */

	/* follow the circle of points which lie PI/2 from the subsolar point.
	 * those which are visible are used directly; those which are not are
	 * projected to limb on a line parallel to the center/subsolar point.
	 */
	ssl = sin(sslat);
	csl = cos(sslat);
	for (npts = i = 0; i < maxpts; i++) {
	    double T = degrad(i*360.0/maxpts);	/* pole to p as seen frm subso*/
	    double plat, plong;	/* lat/long of point on said circle */
	    short px, py;

	    solve_sphere (T, PI/2, ssl, csl, &ca, &B);
	    plat = PI/2 - acos(ca);
	    plong = sslong - B;
	    if (!e_coord (r, wb, hb, plat, plong, &px, &py)) {
		int tx, ty, lx, ly;	/* tmp and limb point */

		if (lc (wb, hb, 2*r, (int)px, (int)py, px+dx, py-dy,
						    &tx, &ty, &lx, &ly) == 0) {
		    px = lx;
		    py = ly;
		}
	    } 
	    xp[npts].x = px;
	    xp[npts].y = py;
	    npts++;
	}

	XPSFillPolygon (dsp, e_pm, e_gc, xp, npts, Complex, CoordModeOrigin);
}

/* draw each continent border */
static void
e_drawcontinents (r, wb, hb)
unsigned r, wb, hb;
{
#define	MINSEP		1000	/* min sep to draw in low prec, 100ths deg */
#define	PTCACHE		64	/* number of XPoints to cache */
	Display *dsp = XtDisplay(e_da_w);
	XPoint xp[PTCACHE];
	MRegion *rp;
	int w = 2*(r+wb);

	/* use the overlay GC, wide lines and the BORDERC color */
	XSetLineAttributes (dsp, e_olgc, WIDE_LW, LINE_ST, CAP_ST, JOIN_ST);
	XSetForeground (dsp, e_olgc, ecolors[BORDERC].p);

	for (rp = ereg; rp < ereg + nereg; rp++) {
	    int n = rp->nmcp;
	    int nxp = 0;
	    short lastlt = rp->mcp[0].lt, lastlg = rp->mcp[0].lg;
	    int i;

	    /* draw the region -- including closure at the end */
	    for (i = 0; i <= n; i++) {
		MCoord *cp = rp->mcp + (i%n);
		short x, y;
		int vis;

		/* don't draw things that are real close if in low prec mode */
		if (wants[LOWPREC] && i != 0 && i != n &&
		    abs(cp->lt-lastlt) < MINSEP && abs(cp->lg-lastlg) < MINSEP)
			continue;
		lastlt = cp->lt;
		lastlg = cp->lg;

		vis = e_coord (r, wb, hb, degrad(cp->lt/100.0),
						degrad(cp->lg)/100.0, &x, &y);
		nxp = add_to_polyline (xp, XtNumber(xp), i, vis, nxp, n, w,x,y);
	    }
	}
}

/* draw each site */
static void
e_drawsites (r, wb, hb)
unsigned r, wb, hb;
{
#define	NPCACHE		64		/* number of XPoints to cache */
	Display *dsp = XtDisplay(e_da_w);
	XPoint xps[NPCACHE];
	Site **sipp;
	int nsipp;
	int nxp;

	/* use the overlay GC and the SITEC color */
	XSetForeground (dsp, e_olgc, ecolors[SITEC].p);

	nxp = 0;
	for (nsipp = sites_get_list(&sipp); --nsipp >= 0; ) {
	    Site *sip = sipp[nsipp];
	    short x, y;
	    int vis;

	    vis = e_coord (r, wb, hb, sip->si_lat, sip->si_lng, &x, &y);
	    if (vis) {
		/* show each site as a little square */
		xps[nxp].x = x;		xps[nxp].y = y;		nxp++;
		xps[nxp].x = x+1;	xps[nxp].y = y;		nxp++;
		xps[nxp].x = x;		xps[nxp].y = y+1;	nxp++;
		xps[nxp].x = x+1;	xps[nxp].y = y+1;	nxp++;
	    }
	    if (nxp > XtNumber(xps)-4 || nsipp == 0) {
		if (nxp > 0)
		    XPSDrawPoints (dsp, e_pm, e_olgc, xps, nxp,CoordModeOrigin);
		nxp = 0;
	    }
	}
}

/* draw all but the first object in the trails list onto e_pm relative to
 * elat/elng connected lines, with tickmarks and possible time stamps.
 */
static void
e_drawtrail (r, wb, hb)
unsigned r;		/* Earth circle radius, pixels */
unsigned wb, hb;	/* width and height borders, pixels */
{
	int w = 2*(r+wb);
	short lx = 0, ly = 0, lv;
	int i;

	XSetLineAttributes (XtD, e_olgc, WIDE_LW, LINE_ST, CAP_ST, JOIN_ST);
	XSetForeground (XtD, e_olgc, ecolors[OBJC].p);
	XSetForeground (XtD, e_gc, ecolors[OBJC].p);

	lv = 0;
	for (i = 1; i < ntrails; i++) {
	    Trail *tp = &trails[i];
	    short x, y;
	    int v;

	    v = e_coord (r, wb, hb, tp->t_sublat, tp->t_sublng, &x, &y);

	    if (v && lv) {
		TrTS ts, lts, *ltsp;

		/* establish a TrTS for tr_draw() */
		ts.t = tp->t_now.n_mjd;
		ts.lbl = tp->t_lbl;

		/* and go back to pick up the first if this is the second tick*/
		if (i == 2) {
		    lts.t = trails[1].t_now.n_mjd;
		    lts.lbl = trails[1].t_lbl;
		    ltsp = &lts;
		} else
		    ltsp = NULL;

		if (wants[CYLINDRICAL] && abs(x-lx) > w/2) {
		    /* break in two when wraps */
		    if (x > lx) {
			/* wrapped over the left edge */
			tr_draw (XtD, e_pm, e_olgc, TICKLN, &ts, ltsp, &trstate,
							    lx, ly, x-w, y);
			tr_draw (XtD, e_pm, e_olgc, TICKLN, &ts, ltsp, &trstate,
								lx+w, ly, x, y);
		    } else {
			/* wrapped over the right edge */
			tr_draw (XtD, e_pm, e_olgc, TICKLN, &ts, ltsp, &trstate,
								lx, ly, x+w, y);
			tr_draw (XtD, e_pm, e_olgc, TICKLN, &ts, ltsp, &trstate,
								lx-w, ly, x, y);
		    }
		} else
		    tr_draw (XtD, e_pm, e_olgc, TICKLN, &ts, ltsp, &trstate,
		    						lx, ly, x, y);
	    }

	    lv = v;
	    lx = x;
	    ly = y;
	}
}

/* draw the first object in the trails list onto e_pm relative to elat/elng
 * with a full bullseye.
 */
static void
e_drawobject (r, wb, hb)
unsigned r;		/* Earth circle radius, pixels */
unsigned wb, hb;	/* width and height borders, pixels */
{
	Trail *tp;

	XSetForeground (XtD, e_olgc, ecolors[OBJC].p);
	XSetForeground (XtD, e_gc, ecolors[OBJC].p);

	/* draw the current location (first object on trails) */
	tp = trails;
	e_drawfootprint (r, wb, hb, tp->t_sublat, tp->t_sublng,
			tp->t_obj.type == EARTHSAT ? tp->t_obj.s_elev : 1e9);
}

/* this function just captures the code we found building over and over to draw
 * polylines allowing for wrapping on left and right sides of cyl projection.
 */
static int
add_to_polyline (xp, xpsize, i, vis, nxp, max, w, x, y)
XPoint xp[];	/* working array */
int xpsize;	/* entries in xp[] */
int i;		/* item we are on: 0..max */
int vis;	/* is this point visible */
int nxp;	/* number of items in xp[] in use -- next goes in xp[npx] */
int max;	/* largest item number we will draw */
int w;		/* window width -- used to check top wraps */
int x, y;	/* the point to add to polyline */
{
	int xwrap;
	int lx = 0, ly = 0;

	if (vis) {
	    xp[nxp].x = x;
	    xp[nxp].y = y;
	    nxp++;
	}

	if (wants[CYLINDRICAL] && nxp > 1) {
	    int dx;

	    lx = xp[nxp-2].x;
	    dx = lx - xp[nxp-1].x;

	    if (dx >= w/2) {
		/* wrapped around right edge */
		xp[nxp-1].x += w;
		ly = xp[nxp-2].y;
		xwrap = 1;
	    } else if (-dx >= w/2) {
		/* wrapped around left edge */
		xp[nxp-1].x -= w;
		ly = xp[nxp-2].y;
		xwrap = -1;
	    } else
		xwrap = 0;
	} else
	    xwrap = 0;

	/* draw the line if it just turned invisible, done or wrapped */
	if (!vis || i == max || nxp == xpsize || xwrap != 0) {
	    if (nxp > 1)
		XPSDrawLines (XtD, e_pm, e_olgc, xp, nxp, CoordModeOrigin);
	    nxp = 0;

	    /* if we wrapped then pick up fragment on opposite end */
	    if (xwrap > 0) {
		/* pick up fragment on left end */
		xp[nxp].x = lx-w; xp[nxp].y = ly; nxp++;
		xp[nxp].x = x;    xp[nxp].y = y;  nxp++;
	    } else if (xwrap < 0) {
		/* pick up fragment on right end */
		xp[nxp].x = lx+w; xp[nxp].y = ly; nxp++;
		xp[nxp].x = x;    xp[nxp].y = y;  nxp++;
	    }
	    /* leave at front of line but draw if this is all */
	    if (i == max)
		XPSDrawLines (XtD, e_pm, e_olgc, xp, nxp, CoordModeOrigin);
	}

	return (nxp);
}

/* mark the mainmenu location */
static void
e_mainmenuloc (np, r, wb, hb)
Now *np;
unsigned r, wb, hb;
{
	XSetForeground (XtD, e_olgc, ecolors[HEREC].p);
	e_drawcross (r, wb, hb, lat, lng, PLUSS);
}

/* mark the location of a solar ecipse, if any.
 *
 * N.B. this code is geometrically correct for all objects, but in practice
 * only the sun and moon are computed accurately enough by xephem to make this
 * worth while for now. in particular, I have tried tests against several S&T
 * tables of minor planet occultations and while the asteroids are computed
 * well enough for visual identification remember that at even 1 AU the earth
 * only subtends 18 arc seconds and the asteroids are not computed *that*
 * accurately (especially since we do not yet include perturbations).
 *
 * I will try to describe the algorithm: the exective summary is that we
 * are striving for the spherical arc subtended by the intersection of a line
 * from one object and the earth's center and the line from the other object
 * to the earth's center.
 *
 * N.B. I tried just computing the intersection of a line connecting the two
 * objects and a unit sphere but it suffered from terrible numerical instabilty.
 *
 * start in a plane defined by the center of the earth, the north pole and
 * object obj0. label the center of the earth as O, the location of object 0
 * as P0, and place object 1 someplace off the line O-P0 and label it P1.
 * what you have actually placed is the location of P1 as it projected onto
 * this place; ie, we are only working with dec here.  define decA as the
 * angle P1-O-P0; it is so named because it is the amount of declication
 * subtended from P0 to P1. Project the line P0-P1 back to a line
 * perpendicular to the line O-P0 at O. decD is the distance from O to the
 * point where P0-P1 intersects the line. if it is less than the earth radius
 * we have an occultation! now do all this again only this time place
 * yourself in a plane defined by the real locations of O, P0 and P1. and
 * repeat everything except this time use the real angled subtended in the
 * sky between P0 and P1 (not just the dec difference). this angle we define
 * as skyA (and its projection back onto a plane perpendicular to P0-P1
 * through O we call skyD). what we want next is the spherical angle
 * subtended between the point at which O-P0 intersects the earth's surface
 * (which is just the geocentric coords of P0) and the point where a line
 * from the tip of skyD to P1 intersects the earth's surface. we call this
 * skyT (I used tau in my original sketch) and I will let you work out the
 * trig (it's just planar trig since you are working in the O-P0-P1 plane).
 * this gives us the spherical angle between the two lines and the earth
 * surface; now all we need is the angle.image yourself at P0 now looking
 * right at O. we see decD as a vertical line and SkyD as a line going off
 * from O at an angle somewhere. the angle between these lines we define as
 * theta. knowing decD and skyD and knowing that there is a right angle at
 * the tip of decD between O and the tip of skyD we can compute the angle
 * between them. theta.  now just use a little spherical trig to find where
 * our arc ends up, compute the new RA, compute longitude by subtracting
 * gst, set latitude to dec, project and draw!
 */
static void
e_soleclipse (np, r, wb, hb)
Now *np;
unsigned r, wb, hb;
{
	Obj *op0 = db_basic (SUN);	/* op0 must be the further one */
	Obj *op1 = db_basic (MOON);
	Obj obj0, obj1;			/* use copies */
	double r0, r1;			/* dist to objects, in earth radii */
	double theta;			/* angle between projections */
	double decD, decA;		/* dec-only proj dist and angle */
	double skyD, skyA, skyP, skyT;	/* full sky projection */
	Now now;			/* local copy to compute EOD info */
	double lst, gst;		/* local and UTC time */
	double lt, lg;			/* lat/long */
	double sD, dRA;

	now = *np;
	obj0 = *op0;
	obj1 = *op1;

	now.n_epoch = EOD;
	(void) obj_cir (&now, &obj0);
	if (is_ssobj(&obj0))
	    r0 = obj0.s_edist*(MAU/ERAD);	/* au to earth radii */
	else
	    r0 = 1e7;				/* way past pluto */

	(void) obj_cir (&now, &obj1);
	if (is_ssobj(&obj1))
	    r1 = obj1.s_edist*(MAU/ERAD);	/* au to earth radii */
	else
	    r1 = 1e7;				/* way past pluto */

	decA = obj1.s_dec - obj0.s_dec;
	decD = r0*r1*sin(decA)/(r0 - r1);	/* similar triangles */
	if (fabs(decD) >= 1.0)
	    return;

	skyA = acos(sin(obj0.s_dec)*sin(obj1.s_dec) +
		cos(obj0.s_dec)*cos(obj1.s_dec)*cos(obj0.s_ra-obj1.s_ra));
	skyD = r0*r1*sin(skyA)/(r0 - r1);	/* similar triangles */
	if (fabs(skyD) >= 1.0)
	    return;

	/* skyP is angle subtended by skyD as seen from obj0 (I called it psi).
	 * skyT is angle subtended by line from earth center to obj0 to a
	 *   point where the line from obj0 to the tip of skyD intersects the
	 *   earth surface (I called it tau).
	 */
	skyP = atan(skyD/r0);
	skyT = asin(skyD*r0/sqrt(r0*r0+skyD*skyD)) - skyP;

	theta = acos(decD/skyD);
	solve_sphere (theta, skyT, sin(obj0.s_dec), cos(obj0.s_dec), &sD, &dRA);

	lt = asin(sD);

	if (obj1.s_ra > obj0.s_ra)
	    dRA = -dRA;	/* eastward */

	lst = obj0.s_ra - dRA;
	utc_gst (mjd_day(mjd), mjd_hr(mjd), &gst);
	lg = lst - hrrad(gst);
	while (lg < -PI) lg += 2*PI;
	while (lg >  PI) lg -= 2*PI;

	XSetForeground (XtD, e_olgc, ecolors[ECLIPSEC].p);
	e_drawcross (r, wb, hb, lt, lg, CROSSH);
}

/* given a height above the earth, in meters, and an altitude above the
 * horizon, in rads, return the great-circle angular distance from the subpoint
 * to the point at which the given height appears at the given altitude, in
 * rads.
 */
static void
e_viewrad (height, alt, radp)
double height;	/* satellite elevation, m above mean earth */
double alt;	/* viewing altitude, rads above horizon */
double *radp;	/* great-circle distance from subpoint to viewing circle, rads*/
{
	*radp = acos(ERAD/(ERAD+height)*cos(alt)) - alt;
}

/* make the first entry in the trail list, ie the current position of dbidx,
 * correct as of *np. then, if desired, discard any existing trail history.
 * return 0 if ok else -1 if no memory.
 */
static int
e_resettrail(np, discard)
Now *np;
int discard;
{
	Obj *op = db_basic (object_dbidx);
	Trail *tp;

	if (discard) {
	    if (trails) {
		free ((char *)trails);
		trails = NULL;
	    }
	    ntrails = 0;

	    if (e_growtrail() == NULL)
		return (-1);
	}

	/* first entry is for the current time, regardless of whether there
	 * are additional entries.
	 */
	tp = &trails[0];

	tp->t_lbl = 0;
	tp->t_now = *np;
	tp->t_obj = *op;
	e_subobject (&tp->t_now, &tp->t_obj, &tp->t_sublat, &tp->t_sublng);

	return (0);
}

/* called from the trails facilty to build up a trail.
 * do it and refresh the view.
 * return 0 if ok, else -1.
 */
/* ARGSUSED */
static int
e_mktrail (ts, statep, client)
TrTS ts[];
TrState *statep;
XtPointer client;
{
	Obj *op = db_basic (object_dbidx);
	Now *np = mm_get_now();
	int wastrail = ntrails > 1;
	int i;

	watch_cursor (1);

	/* erase all but the first entry (ie, leave current alone) */
	(void) e_resettrail(np, 1);

	for (i = 0; i < statep->nticks; i++, ts++) {
	    Trail *tp = e_growtrail();

	    if (!tp)
		return (-1);

	    tp->t_lbl = ts->lbl;
	    tp->t_now = *np;
	    tp->t_now.n_mjd = ts->t;
	    tp->t_obj = *op;
	    (void) obj_cir (&tp->t_now, &tp->t_obj);
	    e_subobject (&tp->t_now, &tp->t_obj, &tp->t_sublat, &tp->t_sublng);
	}

	/* save trail setup as next default */
	trstate = *statep;

	/* draw the trail display if desired but beware getting called while
	 * not up.
	 */
	if (wants[TRAIL] && XtIsManaged(eform_w)) {
	    if (wastrail)
		e_all(np);	/* erase old and draw new */
	    else {
		/* just draw new */
		unsigned int w, h, r, wb, hb;

		e_getcircle (&w, &h, &r, &wb, &hb);
		e_drawtrail (r, wb, hb);
		e_copy_pm();
	    }
	}

	watch_cursor (0);

	return (0);
}

/* grow the trails list by one and return the address of the new entry.
 * else NULL and xe_msg() if no more memory.
 */
static Trail *
e_growtrail ()
{
	char *newmem;

	if (!trails)
	    newmem = malloc (sizeof(Trail));
	else
	    newmem = realloc ((char *)trails, (ntrails+1)*sizeof(Trail));
	if (!newmem) {
	    xe_msg ("No memory for trail entry.", 0);
	    return (NULL);
	}

	trails = (Trail *) newmem;
	ntrails++;

	return (&trails[ntrails-1]);
}
