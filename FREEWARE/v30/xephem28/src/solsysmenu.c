/* code to manage the stuff on the solar system display.
 * functions and data to support the main display begin with ss_.
 * function and data to support the stereo display begin with st_.
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#if defined(__STDC__)
#include <stdlib.h>
#else
extern void *calloc(), *malloc(), *realloc();
#endif
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/DrawingA.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/Separator.h>
#include <Xm/ToggleB.h>
#include <Xm/Text.h>
#include <Xm/Scale.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "trails.h"
#include "ps.h"

extern Widget	toplevel_w;
#define	XtD	XtDisplay(toplevel_w)

extern Now *mm_get_now P_((void));
extern Obj *db_basic P_((int id));
extern int any_ison P_((void));
extern int execute_expr P_((double *vp, char *errbuf));
extern int obj_cir P_((Now *np, Obj *op));
extern int prog_isgood P_((void));
extern int tickmarks P_((double min, double max, int numdiv, double ticks[]));
extern void db_update P_((Obj *op));
extern void f_date P_((Widget w, double jd));
extern void f_double P_((Widget w, char *fmt, double f));
extern void f_pangle P_((Widget w, double a));
extern void f_ra P_((Widget w, double ra));
extern void f_string P_((Widget w, char *s));
extern void f_time P_((Widget w, double t));
extern void fs_date P_((char out[], double jd));
extern void fs_sexa P_((char *out, double a, int w, int fracbase));
extern void fs_time P_((char out[], double t));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void get_views_font P_((Display *dsp, XFontStruct **fspp));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void mm_movie P_((double stepsz));
extern void obj_pickgc P_((Obj *op, Widget w, GC *gcp));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void timestamp P_((Now *np, Widget w));
extern void watch_cursor P_((int want));
extern void xe_msg P_((char *msg, int app_modal));

/* heliocentric coordinates, and enough info to locate it on screen */
typedef struct {
    Obj o;		/* copy of Obj at the given moment */
    TrTS trts;		/* mjd when Obj was valid and whether to timestamp */
    int sx, sy;		/* main view window coords of object */
    int stx;		/* stereo view x coord (y is the same in both) */
    float x, y, z;	/* heliocentric cartesian coords */
} HLoc;


static void ss_create_form P_((void));
static void st_create_form P_((void));
static void ss_trail_cb P_((Widget w, XtPointer client, XtPointer call));
static void ss_activate_cb P_((Widget w, XtPointer client, XtPointer call));
static void ss_obj_cb P_((Widget w, XtPointer client, XtPointer call));
static void ss_changed_cb P_((Widget w, XtPointer client, XtPointer call));
static void ss_close_cb P_((Widget w, XtPointer client, XtPointer call));
static void ss_unmap_cb P_((Widget w, XtPointer client, XtPointer call));
static void ss_print_cb P_((Widget w, XtPointer client, XtPointer call));
static void ss_print P_((void));
static void ss_ps_annotate P_((Now *np, int fontsz));
static void ss_movie_cb P_((Widget w, XtPointer client, XtPointer call));
static void ss_help_cb P_((Widget w, XtPointer client, XtPointer call));
static void ss_helpon_cb P_((Widget w, XtPointer client, XtPointer call));
static void ss_da_exp_cb P_((Widget w, XtPointer client, XtPointer call));
static void ss_da_input_cb P_((Widget w, XtPointer client, XtPointer call));
static void st_parallax_cb P_((Widget w, XtPointer client, XtPointer call));
static void st_map_cb P_((Widget wid, XtPointer client, XtPointer call));
static void st_unmap_cb P_((Widget wid, XtPointer client, XtPointer call));
static void st_da_exp_cb P_((Widget w, XtPointer client, XtPointer call));
static void st_da_input_cb P_((Widget w, XtPointer client, XtPointer call));
static void ss_popup P_((XEvent *ev, HLoc *lp));
static void ss_create_popup P_((void));
static void ss_all P_((void));
static void ss_ss_all P_((void));
static void ss_st_all P_((void));
static void solar_system P_((HLoc *lp, double scale, double selt, double celt,
    double selg, double celg, unsigned nx, unsigned ny));
static int ss_newtrail P_((TrTS ts[], TrState *statep, XtPointer client));
static void hloc_reset P_((Now *np, int discard));
static HLoc *hloc_grow P_((int dbidx));
static int hloc_add P_((int dbidx, Now *np, int lbl));

static Widget ssform_w;		/* main solar system form dialog */
static Widget hr_w, hlng_w, hlat_w; /* scales for heliocentric R, long, lat */
static Widget ssda_w;		/* solar system drawring area */
static Widget dt_w;		/* date/time stamp label widget */
static Pixmap ss_pm;		/* main view pixmap */

static Widget stform_w;		/* main stereo form dialog */
static Widget parallax_w;	/* scale to set amount of parallax */
static Widget stda_w;		/* stereo solar system drawring area */
static Pixmap st_pm;		/* stereo view pixmap */
static Widget stereo_w;		/* stereo option TB */

static TrState trstate = {
    TRLR_5, TRI_DAY, TRF_DATE, TRR_DAY, TRO_UPR, TRS_MEDIUM, 30
};

enum {TRAILS, ECLIPTIC, LEGS, NAMES, STEREO};	/* toggle ids */

#define	MINMAG	3.0		/* minimum mag factor, pixels/AU */
#define	MAXMAG	250.0		/* maximum mag factor, pixels/AU */

#define	GAP	4		/* from object to its name, pixels */

#define	NECLPT	10	/* approx number of circles in the ecliptic grid */
#define	NECLSEG	31	/* number of segments in each stereo ecliptic line */

#define	MOVIE_STEPSZ	120.0	/* movie step size, hours */

#define	CACHE_SZ	100	/* collect these many X commands */
#define	CACHE_PAD	10	/* flush when only this many left */
#define	CACHE_HWM	(CACHE_SZ - CACHE_PAD)	/* hi water mark */
#define	PRFONTSZ	10	/* font size to use for printing */

/* whether each option is currently on */
static int trails;
static int ecliptic;
static int legs;
static int nametags;
static int stereo;

/* current value of desired parallax */
static int parallax;

/* these are managed by the hloc_* functions.
 * there is always at least one to show the object at the current location.
 */
static HLoc *points[NOBJ];	/* malloc'd set of points on screen now */
static int npoints[NOBJ];	/* number of points */

static char obj_on[NOBJ];	/* 1 if object is on */
static Widget obj_w[NOBJ];	/* toggle buttons for each object */

/* info about the popup widget */
typedef struct {
    Widget pu_w;
    Widget name_w;
    Widget ud_w, udl_w;
    Widget ut_w, utl_w;
    Widget ra_w, ral_w;
    Widget dec_w, decl_w;
    Widget hlong_w, hlongl_w;
    Widget hlat_w, hlatl_w;
    Widget eadst_w, eadstl_w;
    Widget sndst_w, sndstl_w;
    Widget elong_w, elongl_w;
} Popup;
static Popup pu;

static char earthname[] = "Earth";

/* called when the solar system view is activated via the main menu pulldown.
 * if never called before, create and manage all the widgets as a child of a
 * form. otherwise, just toggle whether the form is managed.
 */
void
ss_manage ()
{
	if (!ssform_w) {
	    ss_create_form();
	    ss_create_popup();
	    st_create_form();
	    hloc_reset(mm_get_now(), 1);
	    timestamp (mm_get_now(), dt_w);
	}
	
	if (XtIsManaged(ssform_w)) {
	    if (XtIsManaged(stform_w))
		XtUnmanageChild(stform_w);
	    XtUnmanageChild (ssform_w);
	} else {
	    XtManageChild (ssform_w);
	    if (stereo)
		XtManageChild (stform_w);
	}
}

/* called when one of the user defined objects has changed.
 * discard the points for that object.
 * if the object is now defined in the SS manage the toggle button.
 * N.B. no need to rebuild the scene -- ss_update() will be called for us.
 */
void
ss_newobj (dbidx)
int dbidx;
{
	static char me[] = "ss_newobj()";
	Obj *op;

	/* we might get called before we are ever brought up the first time */
	if (!ssform_w)
	    return;

	if (dbidx >= NOBJ) {
	    printf ("%s: dbidx=%d but NOBJ=%d\n", me, dbidx, NOBJ);
	    exit (1);
	}

	if (points[dbidx])
	    XtFree ((char *)points[dbidx]);
	points[dbidx] = NULL;
	npoints[dbidx] = 0;

	op = db_basic(dbidx);
	switch (op->type) {
	case ELLIPTICAL: case HYPERBOLIC: case PARABOLIC:
	    XtManageChild (obj_w[dbidx]);
	    XmToggleButtonSetState (obj_w[dbidx], True, False);
	    set_xmstring (obj_w[dbidx], XmNlabelString, op->o_name);
	    obj_on[dbidx] = 1;
	    break;
	default:
	    XtUnmanageChild (obj_w[dbidx]);
	    XmToggleButtonSetState (obj_w[dbidx], False, False);
	    obj_on[dbidx] = 0;
	    break;
	}
	    
}

int
ss_ison()
{
	return (ssform_w && XtIsManaged(ssform_w));
}

/* called when we are to update our view.
 * don't bother if we are unmanaged.
 */
/* ARGSUSED */
void
ss_update (np, how_much)
Now *np;
int how_much;
{
	int up;

	up = ssform_w && XtIsManaged(ssform_w);
	if (!up)
	    return;

	watch_cursor (1);

	/* set the first (current) entry for each object to now.
	 * erase all other entries unless trails are currently on.
	 */
	hloc_reset (np, !trails);


	/* redraw everything */
	ss_all();
	timestamp (np, dt_w);

	watch_cursor (0);
}

/* called to put up or remove the watch cursor.  */
void
ss_cursor (c)
Cursor c;
{
	Window win;

	if (ssform_w && (win = XtWindow(ssform_w))) {
	    Display *dsp = XtDisplay(ssform_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}

/* create the main solarsystem form */
static void
ss_create_form()
{
	typedef struct {
	    char *label;	/* what goes on the help label */
	    char *key;		/* string to call hlp_dialog() */
	} HelpOn;
	static HelpOn helpon[] = {
	    {"Intro...",	"Solsys - intro"},
	    {"on Mouse...",	"Solsys - mouse"},
	    {"on Control...",	"Solsys - control"},
	    {"on View...",	"Solsys - view"},
	    {"on Objects...",	"Solsys - objects"},
	    {"Misc...",		"Solsys - misc"},
	};
	typedef struct {
	    char *name;		/* name of widget */
	    char *label;	/* label on toggle button */
	    int id;		/* one of the toggle ids */
	    int *state;		/* int we use to keep state */
	} DrCtrl;
	static DrCtrl drctrls[] = {
	    {"Trails",   "Trails",   TRAILS,   &trails},
	    {"Ecliptic", "Ecliptic", ECLIPTIC, &ecliptic},
	    {"Names",    "Names",    NAMES,    &nametags},
	    {"Legs",     "Legs",     LEGS,     &legs},
	};
	Widget frame_w;
	Widget mb_w, pd_w, cb_w;
	Widget w;
	XmString str;
	Arg args[20];
	int i;
	int n;

	/* create the master form */
	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNdefaultPosition, False); n++;
	XtSetArg (args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
	ssform_w = XmCreateFormDialog (toplevel_w, "SolarSystem", args, n);
	XtAddCallback (ssform_w, XmNhelpCallback, ss_help_cb, 0);
	XtAddCallback (ssform_w, XmNunmapCallback, ss_unmap_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Solar System view"); n++;
	XtSetValues (XtParent(ssform_w), args, n);

	/* put a menu bar across the top with the various pulldown menus */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	mb_w = XmCreateMenuBar (ssform_w, "MB", args, n);
	XtManageChild (mb_w);

	/* make the Control pulldown */

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "ControlPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'C'); n++;
	    cb_w = XmCreateCascadeButton (mb_w, "ControlCB", args, n);
	    set_xmstring (cb_w, XmNlabelString, "Control");
	    XtManageChild (cb_w);

	    /* add the "saveas" and print push buttons */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "SSSave", args, n);
	    set_xmstring (w, XmNlabelString, "Save...");
	    XtAddCallback (w, XmNactivateCallback, ss_print_cb, 0);
	    XtManageChild (w);

	    n = 0;
	    w = XmCreatePushButton (pd_w, "SSPrint", args, n);
	    set_xmstring (w, XmNlabelString, "Print...");
	    XtAddCallback (w, XmNactivateCallback, ss_print_cb, (XtPointer)1);
	    XtManageChild (w);

	    /* add the "movie" push button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Movie", args, n);
	    set_xmstring (w, XmNlabelString, "Movie Demo");
	    XtAddCallback (w, XmNactivateCallback, ss_movie_cb, 0);
	    XtManageChild (w);

	    /* add the "stereo" toggle button */

	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    XtSetArg (args[n], XmNmarginHeight, 0); n++;
	    XtSetArg (args[n], XmNindicatorType, XmN_OF_MANY); n++;
	    stereo_w = XmCreateToggleButton (pd_w, "Stereo", args, n);
	    XtAddCallback (stereo_w, XmNvalueChangedCallback, ss_activate_cb, 
							    (XtPointer)STEREO);
	    XtManageChild (stereo_w);
	    stereo = XmToggleButtonGetState (stereo_w);

	    /* add the "close" push button beneath a separator */

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Close", args, n);
	    XtAddCallback (w, XmNactivateCallback, ss_close_cb, 0);
	    XtManageChild (w);

	/* make the View pulldown */

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "ViewPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'V'); n++;
	    cb_w = XmCreateCascadeButton (mb_w, "ViewCB", args, n);
	    set_xmstring (cb_w, XmNlabelString, "View");
	    XtManageChild (cb_w);

	    /* make a PB to bring up the trail definer */

	    str = XmStringCreate("Create Trails...", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    w = XmCreatePushButton (pd_w, "TPB", args, n);
	    XtAddCallback (w, XmNactivateCallback, ss_trail_cb, NULL);
	    XtManageChild (w);
	    XmStringFree (str);

	    n = 0;
	    w = XmCreateSeparator (pd_w, "S", args, n);
	    XtManageChild (w);

	    /* make all the view options TBs */

	    for (i = 0; i < XtNumber(drctrls); i++) {
		DrCtrl *cp = &drctrls[i];

		str = XmStringCreate(cp->label, XmSTRING_DEFAULT_CHARSET);
		n = 0;
		XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
		XtSetArg (args[n], XmNmarginHeight, 0); n++;
		XtSetArg (args[n], XmNindicatorType, XmN_OF_MANY); n++;
		XtSetArg (args[n], XmNlabelString, str); n++;
		w = XmCreateToggleButton(pd_w, cp->name, args, n);
		XmStringFree (str);
		XtManageChild (w);
		*(cp->state) = XmToggleButtonGetState (w);
		XtAddCallback(w, XmNvalueChangedCallback, ss_activate_cb,
							    (XtPointer)cp->id);
	    }

	/* make the Objects pulldown */

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "ObjPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'O'); n++;
	    cb_w = XmCreateCascadeButton (mb_w, "ObjCB", args, n);
	    set_xmstring (cb_w, XmNlabelString, "Objects");
	    XtManageChild (cb_w);

	    /* make the toggle buttons.
	     * fill in the names we can now.
	     * default the planets to all on.
	     */
	    for (i = 0; i < NOBJ; i++) {
		Obj *op = db_basic(i);
		n = 0;
		XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
		XtSetArg (args[n], XmNmarginHeight, 0); n++;
		XtSetArg (args[n], XmNindicatorType, XmN_OF_MANY); n++;
		obj_w[i] = XmCreateToggleButton (pd_w, "ObjTB", args, n);
		XtAddCallback (obj_w[i], XmNvalueChangedCallback, ss_obj_cb,
							    (XtPointer)i);
		switch (op->type) {
		case ELLIPTICAL: case PARABOLIC: case HYPERBOLIC: case PLANET:
		    set_xmstring (obj_w[i], XmNlabelString,
			is_planet(op,MOON)? earthname : op->o_name);
		    XtManageChild (obj_w[i]);
		    XmToggleButtonSetState (obj_w[i], True, False);
		    break;
		}
		obj_on[i] = XmToggleButtonGetState (obj_w[i]);
	    }

	/* make the "help" pulldown */

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "HelpPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'H'); n++;
	    cb_w = XmCreateCascadeButton (mb_w, "HelpCB", args, n);
	    set_xmstring (cb_w, XmNlabelString, "Help");
	    XtManageChild (cb_w);
	    set_something (mb_w, XmNmenuHelpWidget, (XtArgVal)cb_w);

	    for (i = 0; i < XtNumber(helpon); i++) {
		HelpOn *hop = &helpon[i];

		n = 0;
		w = XmCreatePushButton (pd_w, "Help", args, n);
		XtManageChild (w);
		XtAddCallback (w, XmNactivateCallback, ss_helpon_cb, 
							(XtPointer)(hop->key));
		set_xmstring (w, XmNlabelString, hop->label);
	    }

	/* make the time/date stamp label across the bottom */

	n = 0;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	dt_w = XmCreateLabel (ssform_w, "DateStamp", args, n);
	XtManageChild (dt_w);

	/* make the bottom scale */

	n = 0;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, dt_w); n++;
	XtSetArg (args[n], XmNmaximum, 359); n++;
	XtSetArg (args[n], XmNminimum, 0); n++;
	XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg (args[n], XmNshowValue, True); n++;
	hlng_w = XmCreateScale (ssform_w, "HLongScale", args, n);
	XtAddCallback (hlng_w, XmNdragCallback, ss_changed_cb, 0);
	XtAddCallback (hlng_w, XmNvalueChangedCallback, ss_changed_cb, 0);
	XtManageChild (hlng_w);

	/* make the left scale */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, hlng_w); n++;
	XtSetArg (args[n], XmNdecimalPoints, 1); n++;
	XtSetArg (args[n], XmNmaximum, 100); n++;
	XtSetArg (args[n], XmNminimum, 0); n++;
	XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_TOP); n++;
	/* XtSetArg (args[n], XmNshowValue, True); n++; */
	hr_w = XmCreateScale (ssform_w, "DistScale", args, n);
	XtAddCallback (hr_w, XmNdragCallback, ss_changed_cb, 0);
	XtAddCallback (hr_w, XmNvalueChangedCallback, ss_changed_cb, 0);
	XtManageChild (hr_w);

	/* make the right scale */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, hlng_w); n++;
	XtSetArg (args[n], XmNmaximum, 90); n++;
	XtSetArg (args[n], XmNminimum, -90); n++;
	XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_TOP); n++;
	XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_TOP); n++;
	XtSetArg (args[n], XmNshowValue, True); n++;
	hlat_w = XmCreateScale (ssform_w, "HLatScale", args, n);
	XtAddCallback (hlat_w, XmNdragCallback, ss_changed_cb, 0);
	XtAddCallback (hlat_w, XmNvalueChangedCallback, ss_changed_cb, 0);
	XtManageChild (hlat_w);

	/* make a frame for the drawing area */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, hlng_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNleftWidget, hr_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNrightWidget, hlat_w); n++;
	XtSetArg (args[n], XmNshadowType, XmSHADOW_ETCHED_OUT); n++;
	frame_w = XmCreateFrame (ssform_w, "SolarFrame", args, n);
	XtManageChild (frame_w);

	/* make a drawing area for drawing the solar system */

	n = 0;
	ssda_w = XmCreateDrawingArea (frame_w, "SolarDA", args, n);
	XtAddCallback (ssda_w, XmNexposeCallback, ss_da_exp_cb, 0);
	XtAddCallback (ssda_w, XmNinputCallback, ss_da_input_cb, 0);
	XtManageChild (ssda_w);
}

/* create the stereo solarsystem form */
static void
st_create_form()
{
	Widget frame_w;
	Arg args[20];
	int n;

	/* create form */
	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNdefaultPosition, False); n++;
	XtSetArg (args[n], XmNnoResize, True); n++;	/* user can't resize */
	stform_w = XmCreateFormDialog (toplevel_w, "StereoSolarSystem", args,n);
	XtAddCallback (stform_w, XmNmapCallback, st_map_cb, NULL);
	XtAddCallback (stform_w, XmNunmapCallback, st_unmap_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Stereo Solar System"); n++;
	XtSetValues (XtParent(stform_w), args, n);

	/* make the parallax scale at the bottom.
	 * the value is the offset of the sun (at a=0), in pixels.
	 */

	n = 0;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNmaximum, 20); n++;
	XtSetArg (args[n], XmNminimum, -20); n++;
	XtSetArg (args[n], XmNscaleMultiple, 2); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	parallax_w = XmCreateScale (stform_w, "Parallax", args, n);
	XtAddCallback (parallax_w, XmNdragCallback, st_parallax_cb, 0);
	XtAddCallback (parallax_w, XmNvalueChangedCallback, st_parallax_cb, 0);
	XtManageChild (parallax_w);
	XmScaleGetValue (parallax_w, &parallax);

	/* make a frame for the drawing area */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, parallax_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNshadowType, XmSHADOW_ETCHED_OUT); n++;
	frame_w = XmCreateFrame (stform_w, "StereoFrame", args, n);
	XtManageChild (frame_w);

	/* make a drawing area for drawing the stereo solar system */

	n = 0;
	stda_w = XmCreateDrawingArea (frame_w, "StereoDA", args, n);
	XtAddCallback (stda_w, XmNexposeCallback, st_da_exp_cb, 0);
	XtAddCallback (stda_w, XmNinputCallback, st_da_input_cb, 0);
	XtManageChild (stda_w);
}

/* callback to bring up the "Create trails.." PB.
 */
/* ARGSUSED */
static void
ss_trail_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	tr_setup ("xephem Solar System trails setup", "Solar System", &trstate,
							    ss_newtrail, NULL);
}

/* callback from the control toggle buttons
 */
/* ARGSUSED */
static void
ss_activate_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int what = (int) client;

	switch (what) {
	case TRAILS:
	    trails = XmToggleButtonGetState(w);
	    ss_all ();
	    break;
	case ECLIPTIC:
	    ecliptic = XmToggleButtonGetState(w);
	    ss_all ();
	    break;
	case LEGS:
	    legs = XmToggleButtonGetState(w);
	    ss_all ();
	    break;
	case NAMES:
	    nametags = XmToggleButtonGetState(w);
	    ss_all ();
	    break;
	case STEREO:
	    stereo = XmToggleButtonGetState(w);
	    if (stereo)
		XtManageChild(stform_w); /* expose will update it */
	    else
		XtUnmanageChild(stform_w);
	    break;
	default:
	    printf ("solsysmenu.c: unknown toggle button\n");
	    exit(1);
	}
}

/* callback from the object on/off toggle buttons.
 * client is the dbidx.
 */
/* ARGSUSED */
static void
ss_obj_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int who = (int) client;

	obj_on[who] = XmToggleButtonGetState(w);
	ss_all();
}

/* callback when any of the scales change value.
 */
/* ARGSUSED */
static void
ss_changed_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (w != hr_w && w != hlng_w && w != hlat_w) {
	    printf ("solsysmenu.c: Unknown scaled callback\n");
	    exit(1);
	}

	ss_all();
}

/* callback from the Close button.
 */
/* ARGSUSED */
static void
ss_close_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (XtIsManaged(stform_w))
	    XtUnmanageChild (stform_w);
	XtUnmanageChild (ssform_w);

	/* stop movie that might be running */
	mm_movie (0.0);
}

/* callback from unmapping the main view */
/* ARGSUSED */
static void
ss_unmap_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (ss_pm) {
	    XFreePixmap (XtD, ss_pm);
	    ss_pm = (Pixmap) 0;
	}

	if (XtIsManaged(stform_w))
	    XtUnmanageChild (stform_w);
}

/* callback from the Print and Save button.
 * client is 1 if former, 0 if latter.
 */
/* ARGSUSED */
static void
ss_print_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
        XPSAsk ((int)client, "Solar System", ss_print);
}

/* proceed to generate a postscript file.
 * call XPSClose() when finished.
 */
static void
ss_print ()
{
	Now *np = mm_get_now();
	Window win = ss_pm;
	XFontStruct *fsp;
	int fontsz = PRFONTSZ;
        unsigned int nx, ny;
	unsigned int bw, d;
	Window root;
	int x, y;

	if (!ss_ison()) {
	    xe_msg ("Solar System View must be open to print.", 1);
	    XPSClose();
	    return;
	}

	watch_cursor(1);

	/* get info about window size */
        XGetGeometry(XtD, win, &root, &x, &y, &nx, &ny, &bw, &d);

	/* fit view across the top and prepare to capture X calls.
	 * beware of tall sizes.
	 */
	if (ny > nx) {
	    double aspect = (double)nx/ny;
	    int l = 6.5*(1 - aspect)/2;
	    XPSXBegin(win,0,0,nx,0,(1+l)*72,10*72,(int)(6.5*aspect*72),fontsz);
	} else
	    XPSXBegin (win, 0, 0, nx, 0, 1*72, 10*72, (int)(6.5*72), fontsz);

	/* get and register the font to use for all gcs from obj_pickgc() */
	get_views_font (XtD, &fsp);
	XPSRegisterFont (fsp->fid, "Helvetica");

	/* redraw the main view -- *not* the stereo view */
	ss_ss_all ();

	/* no more X captures */
	XPSXEnd();

	/* add some extra info */
	ss_ps_annotate (np, fontsz);

	/* finished */
	XPSClose();

	watch_cursor(0);
}

static void
ss_ps_annotate (np, fs)
Now *np;
int fs;
{
	char dir[128];
	char buf[128];
	char *bp;
	int vs = fs+2;		/* nominal vertical line spacing */
	double elt, elng;
	int sv;
	int y;

	XmScaleGetValue (hlat_w, &sv);
	elt = degrad(sv);
	XmScaleGetValue (hlng_w, &sv);
	elng = degrad(sv+90.0);

	/* title, of sorts */

	y = 72 + vs*9;
	(void) sprintf (dir, "(Xephem Solar System View) 306 %d cstr\n", y);
	XPSDirect (dir);

	/* left column */
	y = 72 + vs*7;
	fs_sexa (buf, raddeg(elt), 3, 60);
	for (bp = buf; *bp == ' '; bp++) continue;
	(void) sprintf (dir, "(H Lat:) 144 %d rstr (%s) 154 %d lstr\n",y, bp,y);
	XPSDirect (dir);

	y = 72 + vs*6;
	fs_sexa (buf, raddeg(elng), 4, 60);
	for (bp = buf; *bp == ' '; bp++) continue;
	(void) sprintf (dir, "(H Long:) 144 %d rstr (%s) 154 %d lstr\n",y,bp,y);
	XPSDirect (dir);

	/* right column */
	y = 72 + vs*7;
	fs_time (buf, mjd_hr(mjd));
	for (bp = buf; *bp == ' '; bp++) continue;
	(void) sprintf (dir, "(UT T:) 450 %d rstr (%s) 460 %d lstr\n", y, bp,y);
	XPSDirect (dir);

	y = 72 + vs*6;
	fs_date (buf, mjd_day(mjd));
	for (bp = buf; *bp == ' '; bp++) continue;
	(void) sprintf (dir, "(UT D:) 450 %d rstr (%s) 460 %d lstr\n", y, bp,y);
	XPSDirect (dir);
}

/* callback from the Movie button.
 */
/* ARGSUSED */
static void
ss_movie_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	mm_movie (MOVIE_STEPSZ);
}

/* callback from the Help button.
 */
/* ARGSUSED */
static void
ss_help_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static char *msg[] = {
"This displays the solar system. The sun is always at the center. The left",
"slider controls your distance from the sun - further up is closer. The",
"bottom slider controls your heliocentric longitude. The right slider controls",
"your heliocentric latitude - your angle above the ecliptic."
};

	hlp_dialog ("Solsys", msg, sizeof(msg)/sizeof(msg[0]));
}

/* callback from a specific Help button.
 * client is a string to use with hlp_dialog().
 */
/* ARGSUSED */
static void
ss_helpon_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	hlp_dialog ((char *)client, NULL, 0);
}

/* expose of solar system drawing area.
 */
/* ARGSUSED */
static void
ss_da_exp_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static unsigned ss_last_nx, ss_last_ny;
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;
	Display *dsp = XtDisplay (w);
	Window win = XtWindow (w);
	unsigned int ss_nx, ss_ny;
	unsigned int bw, d;
	Window root;
	int x, y;

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
	    printf ("Unexpected ssda_w event. type=%d\n", c->reason);
	    exit(1);
	}

	/* make the pixmap if new or changed size */
	XGetGeometry(dsp, win, &root, &x, &y, &ss_nx, &ss_ny, &bw, &d);
	if (!ss_pm || ss_nx != ss_last_nx || ss_ny != ss_last_ny) {
	    if (ss_pm)
		XFreePixmap (dsp, ss_pm);
	    ss_pm = XCreatePixmap (dsp, win, ss_nx, ss_ny, d);
	    ss_last_nx = ss_nx;
	    ss_last_ny = ss_ny;
	}

	ss_ss_all();
}

/* a dot has been picked: find what it is and report it. */
/* ARGSUSED */
static void
ss_da_input_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;
	XEvent *ev;
	
#define	PICKRANGE	100	/* sqr of dist allowed from pointer */
	int x, y, mind = 0;
	int dbidx;
	HLoc *lp;
	int i;

	if (c->reason != XmCR_INPUT)
	    return;
	ev = c->event;
	if (ev->xany.type != ButtonPress || ev->xbutton.button != Button3)
	    return;

	x = ((XButtonPressedEvent *)ev)->x;
	y = ((XButtonPressedEvent *)ev)->y;

	lp = NULL;
	for (dbidx = 0; dbidx < NOBJ; dbidx++) {
	    if (!obj_on[dbidx])
		continue;
	    for (i = 0; i < npoints[dbidx]; i++) {
		int d = (x-points[dbidx][i].sx)*(x-points[dbidx][i].sx) +
				(y-points[dbidx][i].sy)*(y-points[dbidx][i].sy);
		if (!lp || d < mind) {
		    lp = &points[dbidx][i];
		    mind = d;
		}
	    }
	}

	if (lp && mind <= PICKRANGE)
	    ss_popup (ev, lp);
}

/* ARGSUSED */
static void
st_parallax_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmScaleGetValue (w, &parallax);
	ss_all ();
}

/* called whenever the stereo scene is mapped.
 * we set the size of the DrawingArea the same as the main window's.
 * we also try to position it just to the left, but it doesn't always work :-(
 */
/* ARGSUSED */
static void
st_map_cb (wid, client, call)
Widget wid;
XtPointer client;
XtPointer call;
{
	Dimension w, h;
	Position x, y;
	Arg args[20];
	int n;

	n = 0;
	XtSetArg (args[n], XmNwidth, &w); n++;
	XtSetArg (args[n], XmNheight, &h); n++;
	XtGetValues (ssda_w, args, n);

	n = 0;
	XtSetArg (args[n], XmNwidth, w); n++;
	XtSetArg (args[n], XmNheight, h); n++;
	XtSetValues (stda_w, args, n);

	n = 0;
	XtSetArg (args[n], XmNx, &x); n++;
	XtSetArg (args[n], XmNy, &y); n++;
	XtGetValues (ssform_w, args, n);

	n = 0;
	XtSetArg (args[n], XmNx, x-w-30); n++;
	XtSetArg (args[n], XmNy, y); n++;
	XtSetValues (stform_w, args, n);
}

/* callback from unmapping the stereo view */
/* ARGSUSED */
static void
st_unmap_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	stereo = 0;
	XmToggleButtonSetState(stereo_w, False, False);

	if (st_pm) {
	    XFreePixmap (XtD, st_pm);
	    st_pm = (Pixmap) 0;
	}
}

/* expose of stereo solar system drawing area.
 */
/* ARGSUSED */
static void
st_da_exp_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static unsigned st_last_nx, st_last_ny;
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;
	Display *dsp = XtDisplay (w);
	Window win = XtWindow (w);
	unsigned int st_nx, st_ny;
	unsigned int bw, d;
	Window root;
	int x, y;

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
	    printf ("Unexpected stda_w event. type=%d\n", c->reason);
	    exit(1);
	}

	/* make the pixmap if new or changed size */
	XGetGeometry(dsp, win, &root, &x, &y, &st_nx, &st_ny, &bw, &d);
	if (!st_pm || st_nx != st_last_nx || st_ny != st_last_ny) {
	    if (st_pm)
		XFreePixmap (dsp, st_pm);
	    st_pm = XCreatePixmap (dsp, win, st_nx, st_ny, d);
	    st_last_nx = st_nx;
	    st_last_ny = st_ny;
	}

	ss_st_all();
}

/* a dot has been picked on the stereo map: find what it is and report it. */
/* ARGSUSED */
static void
st_da_input_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;
	XEvent *ev;
	
#define	PICKRANGE	100	/* sqr of dist allowed from pointer */
	int x, y, mind = 0;
	int dbidx;
	HLoc *lp;
	int i;

	if (c->reason != XmCR_INPUT)
	    return;
	ev = c->event;
	if (ev->xany.type != ButtonPress || ev->xbutton.button != Button3)
	    return;

	x = ((XButtonPressedEvent *)ev)->x;
	y = ((XButtonPressedEvent *)ev)->y;

	lp = NULL;
	for (dbidx = 0; dbidx < NOBJ; dbidx++)
	    for (i = 0; i < npoints[dbidx]; i++) {
		int d = (x-points[dbidx][i].stx)*(x-points[dbidx][i].stx) +
				(y-points[dbidx][i].sy)*(y-points[dbidx][i].sy);
		if (!lp || d < mind) {
		    lp = &points[dbidx][i];
		    mind = d;
		}
	    }

	if (lp && mind <= PICKRANGE)
	    ss_popup (ev, lp);
}

/* fill in the popup with goodies from lp.
 * display fields the same way they are in main data menu.
 * position the popup as indicated by ev and display it.
 * it goes down by itself.
 */
static void
ss_popup (ev, lp)
XEvent *ev;
HLoc *lp;
{
	Obj *op = &lp->o;
	double d;

	if (is_planet(op, MOON)) {
	    /* MOON is used to denote Earth */
	    f_string (pu.name_w, earthname);
	    XtManageChild (pu.ud_w);
	    XtManageChild (pu.udl_w);
	    XtManageChild (pu.ut_w);
	    XtManageChild (pu.utl_w);
	    XtManageChild (pu.ra_w);
	    XtManageChild (pu.ral_w);
	    set_xmstring (pu.ral_w, XmNlabelString, "Sun RA:");
	    XtManageChild (pu.dec_w);
	    XtManageChild (pu.decl_w);
	    set_xmstring (pu.decl_w, XmNlabelString, "Sun Dec:");
	    XtManageChild (pu.hlong_w);
	    XtManageChild (pu.hlongl_w);
	    XtUnmanageChild (pu.hlat_w);
	    XtUnmanageChild (pu.hlatl_w);
	    XtUnmanageChild (pu.eadst_w);
	    XtUnmanageChild (pu.eadstl_w);
	    XtManageChild (pu.sndst_w);
	    XtManageChild (pu.sndstl_w);
	    XtUnmanageChild (pu.elong_w);
	    XtUnmanageChild (pu.elongl_w);
	    set_something (pu.pu_w, XmNnumColumns, (XtArgVal)7);
	} else {
	    f_string (pu.name_w, op->o_name);
	    if (is_planet (op, SUN)) {
		XtUnmanageChild (pu.ud_w);
		XtUnmanageChild (pu.udl_w);
		XtUnmanageChild (pu.ut_w);
		XtUnmanageChild (pu.utl_w);
		XtUnmanageChild (pu.ra_w);
		XtUnmanageChild (pu.ral_w);
		XtUnmanageChild (pu.dec_w);
		XtUnmanageChild (pu.decl_w);
		XtUnmanageChild (pu.hlong_w);
		XtUnmanageChild (pu.hlongl_w);
		XtUnmanageChild (pu.hlat_w);
		XtUnmanageChild (pu.hlatl_w);
		XtUnmanageChild (pu.eadst_w);
		XtUnmanageChild (pu.eadstl_w);
		XtUnmanageChild (pu.sndst_w);
		XtUnmanageChild (pu.sndstl_w);
		XtUnmanageChild (pu.elong_w);
		XtUnmanageChild (pu.elongl_w);
		set_something (pu.pu_w, XmNnumColumns, (XtArgVal)1);
	    } else {
		XtManageChild (pu.ud_w);
		XtManageChild (pu.udl_w);
		XtManageChild (pu.ut_w);
		XtManageChild (pu.utl_w);
		XtManageChild (pu.ra_w);
		XtManageChild (pu.ral_w);
		set_xmstring (pu.ral_w, XmNlabelString, "RA:");
		XtManageChild (pu.dec_w);
		XtManageChild (pu.decl_w);
		set_xmstring (pu.decl_w, XmNlabelString, "Dec:");
		XtManageChild (pu.hlong_w);
		XtManageChild (pu.hlongl_w);
		XtManageChild (pu.hlat_w);
		XtManageChild (pu.hlatl_w);
		XtManageChild (pu.eadst_w);
		XtManageChild (pu.eadstl_w);
		XtManageChild (pu.sndst_w);
		XtManageChild (pu.sndstl_w);
		XtManageChild (pu.elong_w);
		XtManageChild (pu.elongl_w);
		set_something (pu.pu_w, XmNnumColumns, (XtArgVal)10);
	    }
	}

	f_date (pu.ud_w, mjd_day(lp->trts.t));
	f_time (pu.ut_w, mjd_hr(lp->trts.t));
	f_ra (pu.ra_w, op->s_ra);
	f_pangle (pu.dec_w, op->s_dec);
	f_pangle (pu.hlong_w, op->s_hlong);
	f_pangle (pu.hlat_w, op->s_hlat);

	d = op->s_edist;
	f_double (pu.eadst_w, d >= 9.99995 ? "%6.3f" : "%6.4f", d);

	d = is_planet(op, MOON) ? op->s_edist
				: op->s_sdist;
	f_double (pu.sndst_w, d >= 9.99995 ? "%6.3f" : "%6.4f", d);

	f_double (pu.elong_w, "%6.1f", op->s_elong);

	XmMenuPosition (pu.pu_w, (XButtonPressedEvent *)ev);
	XtManageChild (pu.pu_w);
}

/* create the id popup */
static void
ss_create_popup()
{
	Arg args[20];
	Widget w;
	int n;

	/* create the outer form */
	n = 0;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
	XtSetArg (args[n], XmNisAligned, False); n++;
	pu.pu_w = XmCreatePopupMenu (ssda_w, "SSPopup", args, n);

	/* create the widgets */

	/* name */
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	w = XmCreateLabel (pu.pu_w, "SSPopNameL", args, n);
	set_xmstring (w, XmNlabelString, "Name:");
	XtManageChild (w);
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	pu.name_w = XmCreateLabel (pu.pu_w, "SSPopName", args, n);
	XtManageChild (pu.name_w);

	/* UT date */
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	pu.udl_w = XmCreateLabel (pu.pu_w, "SSPopupUTDateL", args, n);
	set_xmstring (pu.udl_w, XmNlabelString, "UT Date:");
	XtManageChild (pu.udl_w);
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	pu.ud_w = XmCreateLabel (pu.pu_w, "SSPopUTDate", args, n);
	XtManageChild (pu.ud_w);

	/* UT time */
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	pu.utl_w = XmCreateLabel (pu.pu_w, "SSPopupUTTimeL", args, n);
	set_xmstring (pu.utl_w, XmNlabelString, "UT Time:");
	XtManageChild (pu.utl_w);
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	pu.ut_w = XmCreateLabel (pu.pu_w, "SSPopUTTime", args, n);
	XtManageChild (pu.ut_w);

	/* ra */
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	pu.ral_w = XmCreateLabel (pu.pu_w, "SSPopRAL", args, n);
	XtManageChild (pu.ral_w);
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	pu.ra_w = XmCreateLabel (pu.pu_w, "SSPopRA", args, n);
	XtManageChild (pu.ra_w);

	/* dec */
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	pu.decl_w = XmCreateLabel (pu.pu_w, "SSPopDecL", args, n);
	XtManageChild (pu.decl_w);
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	pu.dec_w = XmCreateLabel (pu.pu_w, "SSPopDec", args, n);
	XtManageChild (pu.dec_w);

	/* hlong */
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	pu.hlongl_w = XmCreateLabel (pu.pu_w, "SSPopupHLongL", args, n);
	set_xmstring (pu.hlongl_w, XmNlabelString, "HeLong:");
	XtManageChild (pu.hlongl_w);
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	pu.hlong_w = XmCreateLabel (pu.pu_w, "SSPopHLong", args, n);
	XtManageChild (pu.hlong_w);

	/* hlat */
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	pu.hlatl_w = XmCreateLabel (pu.pu_w, "SSPopupHLatL", args, n);
	set_xmstring (pu.hlatl_w, XmNlabelString, "HeLat:");
	XtManageChild (pu.hlatl_w);
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	pu.hlat_w = XmCreateLabel (pu.pu_w, "SSPopHLat", args, n);
	XtManageChild (pu.hlat_w);

	/* earth dist */
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	pu.eadstl_w = XmCreateLabel (pu.pu_w, "SSPopupEaDstL", args, n);
	set_xmstring (pu.eadstl_w, XmNlabelString, "EaDst:");
	XtManageChild (pu.eadstl_w);
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	pu.eadst_w = XmCreateLabel (pu.pu_w, "SSPopEaDst", args, n);
	XtManageChild (pu.eadst_w);

	/* sun dist */
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	pu.sndstl_w = XmCreateLabel (pu.pu_w, "SSPopupSnDstL", args, n);
	set_xmstring (pu.sndstl_w, XmNlabelString, "SnDst:");
	XtManageChild (pu.sndstl_w);
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	pu.sndst_w = XmCreateLabel (pu.pu_w, "SSPopSnDst", args, n);
	XtManageChild (pu.sndst_w);

	/* elong */
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	pu.elongl_w = XmCreateLabel (pu.pu_w, "SSPopupElongL", args, n);
	set_xmstring (pu.elongl_w, XmNlabelString, "Elong:");
	XtManageChild (pu.elongl_w);
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	pu.elong_w = XmCreateLabel (pu.pu_w, "SSPopElong", args, n);
	XtManageChild (pu.elong_w);
}

/* redraw the main view and, if enables, the stereo view.
 * ok to use this everywhere _except_ the individual window expose callbacks.
 */
static void
ss_all()
{
	watch_cursor (1);

	ss_ss_all();

	if (stereo)
	    ss_st_all();

	watch_cursor (0);
}

/* redraw the main sol system view.
 */
static void
ss_ss_all()
{
#define	CACHE_SZ	100	/* collect these many X commands */
#define	CACHE_PAD	10	/* flush when only this many left */
#define	CACHE_HWM	(CACHE_SZ - CACHE_PAD)	/* hi water mark */
	static GC ss_bgc;
	static int ss_asc;
	Display *dsp = XtDisplay(ssda_w);
	Window ss_win = XtWindow(ssda_w);
	unsigned int ss_nx, ss_ny;
	unsigned int bw, d;
	Window root;
	int x, y;
	int dbidx;
	int sv;			/* ScaleValue tmp */
	double scale;		/* pixels per au */
	double elt, selt, celt;	/* heliocentric lat of eye, rads */
	double elg, selg, celg;	/* heliocentric lng of eye, rads */
	int n;

	watch_cursor (1);

	/* get the sizes of the windows we'll use */
	XGetGeometry(dsp, ss_win, &root, &x, &y, &ss_nx, &ss_ny, &bw, &d);

	/* make sure the gc's are created */
	if (!ss_bgc) {
	    XFontStruct *fs;
	    XGCValues gcv;
	    unsigned int gcm;
	    Pixel p;

	    gcm = GCForeground;
	    get_something (ssda_w, XmNbackground, (XtArgVal)&p);
	    gcv.foreground = p;
	    ss_bgc = XCreateGC (dsp, ss_win, gcm, &gcv);
	    fs = XQueryFont (dsp, XGContextFromGC (ss_bgc));
	    ss_asc = fs->max_bounds.ascent;
	}

	/* clear */
	XFillRectangle (dsp, ss_pm, ss_bgc, 0, 0, ss_nx, ss_ny);

	/* establish the scales */
	XmScaleGetValue (hr_w, &sv);
	scale = MINMAG * pow (MAXMAG/MINMAG, sv/100.);
	XmScaleGetValue (hlat_w, &sv);
	elt = degrad(sv);
	selt = sin(elt);
	celt = cos(elt);
	XmScaleGetValue (hlng_w, &sv);
	elg = degrad(sv+90.0);
	selg = sin(elg);
	celg = cos(elg);

	/* draw the ecliptic plane, if desired */
	if (ecliptic) {
	    static GC egc;
	    double minau, maxau;
	    double aus[NECLPT+2];
	    char spacing[64];
	    int nau, i;

	    if (!egc) {
		XGCValues gcv;
		unsigned int gcm;
		Pixel p;

		gcm = GCForeground;
		get_something (ssda_w, XmNforeground, (XtArgVal)&p);
		gcv.foreground = p;
		egc = XCreateGC (dsp, ss_win, gcm, &gcv);
	    }

	    minau = 0;
	    maxau = ss_nx > ss_ny ? ss_nx/2.0/scale : ss_ny/2.0/scale;
	    nau = tickmarks (minau, maxau, NECLPT, aus);

	    /* draw tick mark spacing message in upper left corner */
	    (void) sprintf (spacing, "%g AU", aus[1] - aus[0]);
	    XPSDrawString(dsp, ss_pm, egc, 1, ss_asc+1,spacing,strlen(spacing));

	    /* draw each grid line.
	     * the main view uses simple elipses.
	     */
	    for (i = 0; i < nau; i++) {
		int arcx, arcy, arcw, arch;
		double au = aus[i];
		HLoc hhl, whl;		/* width and height */

		if (au <= 0.0)
		    continue;

		whl.x = au*celg;	/* a point to the right */
		whl.y = au*selg;
		whl.z = 0.0;
		solar_system(&whl, scale, selt, celt, selg, celg, ss_nx, ss_ny);
		hhl.x = -au*selg;	/* a point up */
		hhl.y = au*celg;
		hhl.z = 0.0;
		solar_system(&hhl, scale, selt, celt, selg, celg, ss_nx, ss_ny);
		if (selt < 0.0)
		    hhl.sy = ss_ny - hhl.sy;

		arcx = ss_nx - whl.sx;
		arcy = hhl.sy;
		arcw = 2*whl.sx - ss_nx - 1;
		arch = ss_ny - 2*hhl.sy - 1;
		if (arch <= 0)
		    arch = 1;	/* avoid pushing our luck with XDrawArc */
		XPSDrawArc (dsp, ss_pm, egc, arcx, arcy, arcw, arch, 0,360*64);
	    }
	}

	/* display each point and any additional points as trails. */
	for (dbidx = 0; dbidx < NOBJ; dbidx++) {
	    XPoint ss_xpoints[CACHE_SZ], *ss_xp = ss_xpoints;
	    GC gc;

	    obj_pickgc (db_basic(dbidx), ssda_w, &gc); /*moonColor for Earth?*/

	    if (npoints[dbidx] > 0 && obj_on[dbidx]) {
		int np = npoints[dbidx];
		int i;

		for (i = 0; i < np; i++) {
		    HLoc *lp = &points[dbidx][i];
		    HLoc leghl; /* for projection of planet on ecliptic plane */

		    if (!trails && i > 0)
			break;

		    /* compute screen location */
		    solar_system(lp, scale, selt, celt, selg, celg,ss_nx,ss_ny);

		    /* draw leg down to ecliptic if desired */
		    if (legs) {
			leghl = *lp;
			leghl.z = 0;
			solar_system(&leghl, scale, selt, celt, selg, celg,
								ss_nx, ss_ny);
			XPSDrawLine(dsp, ss_pm, gc, lp->sx, lp->sy, leghl.sx,
								    leghl.sy);
		    }

		    /* draw a blob for the object */
		    XPSDrawArc (dsp, ss_pm, gc, lp->sx-1, lp->sy-1, 2, 2,
								    0, 64*360);

		    /* connect and draw time stamp if more than one trail point.
		     * (first point is current pos; 2nd is first of trail set)
		     * we do our own tick marks.
		     */
		    if (i > 1) {
			HLoc *pp = &points[dbidx][i-1];	/* prior trail point */

			tr_draw(dsp, ss_pm, gc, 0, &lp->trts,
							i==2 ? &pp->trts : NULL,
				    &trstate, pp->sx, pp->sy, lp->sx, lp->sy);
		    }

		    /* draw the SUN as a circle at the center */
		    if (dbidx == SUN)
			XPSDrawArc (dsp, ss_pm, gc, lp->sx-3, lp->sy-3,
							    7, 7, 0, 64*360);

		    /* flush the point caches if full */
		    if ((n = ss_xp - ss_xpoints) >= CACHE_HWM) {
			XPSDrawPoints (dsp, ss_pm, gc, ss_xpoints, n,
							    CoordModeOrigin);
			ss_xp = ss_xpoints;
		    }
		}

		/* draw the object name at the current location if desired */
		if (nametags) {
		    HLoc *flp = &points[dbidx][0];
		    char *name;

		    if (dbidx == MOON)
			name = earthname;
		    else {
			Obj *op = db_basic (dbidx);
			name = op->o_name;
		    }
		    XPSDrawString (dsp, ss_pm, gc, flp->sx+GAP, flp->sy,
							name, strlen(name));
		}
	    }

	    /* one more flush to pick up any extra */
	    if ((n = ss_xp - ss_xpoints) > 0)
		XPSDrawPoints (dsp, ss_pm, gc, ss_xpoints, n, CoordModeOrigin);
	}

	/* copy the work pixmaps to the screen */
	XCopyArea (dsp, ss_pm, ss_win, ss_bgc, 0, 0, ss_nx, ss_ny, 0, 0);

	/* all finished */
	watch_cursor (0);
}

/* redraw the stereo view.
 */
static void
ss_st_all()
{
	static GC st_bgc;
	static int st_asc;
	Display *dsp = XtDisplay(stda_w);
	Window st_win = XtWindow(stda_w);
	unsigned int st_nx, st_ny;
	unsigned int bw, d;
	Window root;
	int x, y;
	int dbidx;
	int sv;			/* ScaleValue tmp */
	double scale;		/* pixels per au */
	double elt, selt, celt;	/* heliocentric lat of eye, rads */
	double elg, selg, celg;	/* heliocentric lng of eye, rads */
	int n;

	watch_cursor (1);

	/* get the sizes of the windows we'll use */
	XGetGeometry(dsp, st_win, &root, &x, &y, &st_nx, &st_ny, &bw, &d);

	/* make sure the gc's are created */
	if (!st_bgc) {
	    XFontStruct *fs;
	    XGCValues gcv;
	    unsigned int gcm;
	    Pixel p;

	    gcm = GCForeground;
	    get_something (stda_w, XmNbackground, (XtArgVal)&p);
	    gcv.foreground = p;
	    st_bgc = XCreateGC (dsp, st_win, gcm, &gcv);
	    fs = XQueryFont (dsp, XGContextFromGC (st_bgc));
	    st_asc = fs->max_bounds.ascent;
	}

	/* clear */
	XFillRectangle (dsp, st_pm, st_bgc, 0, 0, st_nx, st_ny);

	/* establish the scales */
	XmScaleGetValue (hr_w, &sv);
	scale = MINMAG * pow (MAXMAG/MINMAG, sv/100.);
	XmScaleGetValue (hlat_w, &sv);
	elt = degrad(sv);
	selt = sin(elt);
	celt = cos(elt);
	XmScaleGetValue (hlng_w, &sv);
	elg = degrad(sv+90.0);
	selg = sin(elg);
	celg = cos(elg);

	/* draw the ecliptic plane, if desired */
	if (ecliptic) {
	    static GC egc;
	    double minau, maxau;
	    double aus[NECLPT+2];
	    char spacing[64];
	    int nau, i;

	    if (!egc) {
		XGCValues gcv;
		unsigned int gcm;
		Pixel p;

		gcm = GCForeground;
		get_something (ssda_w, XmNforeground, (XtArgVal)&p);
		gcv.foreground = p;
		egc = XCreateGC (dsp, st_win, gcm, &gcv);
	    }

	    minau = 0;
	    maxau = st_nx > st_ny ? st_nx/2.0/scale : st_ny/2.0/scale;
	    nau = tickmarks (minau, maxau, NECLPT, aus);

	    /* draw tick mark spacing message in upper left corner */
	    (void) sprintf (spacing, "%g AU", aus[1] - aus[0]);
	    XDrawString(dsp, st_pm, egc, 1, st_asc+1, spacing, strlen(spacing));

	    /* draw each grid line.
	     * the stereo view uses polylines drawn in world coords -- slower!
	     */
	    for (i = 0; i < nau; i++) {
		double au = aus[i];
		XPoint xps[NECLSEG+1];	/* +1 to close */
		int s;

		if (au <= 0.0)
		    continue;

		for (s = 0; s < NECLSEG; s++) {
		    double hlng = s*2*PI/NECLSEG;
		    HLoc hloc;

		    hloc.x = au*cos(hlng);
		    hloc.y = au*sin(hlng);
		    hloc.z = 0.0;
		    solar_system(&hloc, scale, selt, celt, selg, celg,
							    st_nx, st_ny);
		    xps[s].x = (short) hloc.stx;
		    xps[s].y = (short) hloc.sy;
		}

		xps[NECLSEG] = xps[0];	/* close */
		XDrawLines (dsp, st_pm, egc, xps, NECLSEG+1, CoordModeOrigin);
	    }
	}

	/* display each point and any additional points as trails. */
	for (dbidx = 0; dbidx < NOBJ; dbidx++) {
	    XPoint st_xpoints[CACHE_SZ], *st_xp = st_xpoints;
	    GC gc;

	    obj_pickgc (db_basic(dbidx), ssda_w, &gc); /*moonColor for Earth?*/

	    if (npoints[dbidx] > 0 && obj_on[dbidx]) {
		int np = npoints[dbidx];
		int i;

		for (i = 0; i < np; i++) {
		    HLoc *lp = &points[dbidx][i];
		    HLoc leghl; /* for projection of planet on ecliptic plane */

		    if (!trails && i > 0)
			break;

		    /* compute screen location */
		    solar_system(lp, scale, selt, celt, selg, celg,st_nx,st_ny);

		    /* draw leg down to ecliptic if desired */
		    if (legs) {
			leghl = *lp;
			leghl.z = 0;
			solar_system(&leghl, scale, selt, celt, selg, celg,
							    st_nx, st_ny);
			XDrawLine(dsp, st_pm, gc, lp->stx, lp->sy,
						    leghl.stx, leghl.sy);
		    }

		    /* draw a blob for the object */
		    st_xp->x = lp->stx;	st_xp->y = lp->sy;	st_xp++;
		    st_xp->x = lp->stx+1; 	st_xp->y = lp->sy;	st_xp++;
		    st_xp->x = lp->stx;   	st_xp->y = lp->sy+1;	st_xp++;
		    st_xp->x = lp->stx+1; 	st_xp->y = lp->sy+1;	st_xp++;

		    /* connect and draw time stamp if more than one trail point.
		     * (first point is current pos; 2nd is first of trail set)
		     * we do our own tick marks.
		     */
		    if (i > 1) {
			HLoc *pp = &points[dbidx][i-1];

			tr_draw(dsp, st_pm, gc, 0, &lp->trts,
						    i==2 ? &pp->trts : NULL,
				&trstate, pp->stx, pp->sy, lp->stx, lp->sy);
		    }

		    /* draw the SUN as a circle at the center */
		    if (dbidx == SUN)
			XDrawArc (dsp, st_pm, gc, lp->stx-3, lp->sy-3,
							    7, 7, 0, 64*360);

		    /* flush the point caches if full */
		    if ((n = st_xp - st_xpoints) >= CACHE_HWM) {
			XDrawPoints (dsp,st_pm,gc,st_xpoints,n,CoordModeOrigin);
			st_xp = st_xpoints;
		    }
		}

		/* draw the object name at the current location if desired */
		if (nametags) {
		    HLoc *flp = &points[dbidx][0];
		    char *name;

		    if (dbidx == MOON)
			name = earthname;
		    else {
			Obj *op = db_basic (dbidx);
			name = op->o_name;
		    }
		    XDrawString (dsp, st_pm, gc, flp->stx+GAP, flp->sy,
						    name, strlen(name));
		}
	    }

	    /* one more flush to pick up any extra */
	    if ((n = st_xp - st_xpoints) > 0)
		XDrawPoints (dsp, st_pm, gc, st_xpoints, n, CoordModeOrigin);
	}

	/* copy the work pixmaps to the screen */
	XCopyArea (dsp, st_pm, st_win, st_bgc, 0, 0, st_nx, st_ny, 0, 0);

	/* all finished */
	watch_cursor (0);
}

/* compute location of HLoc in window of size [nx,ny].
 * N.B. others assume we only use lp->{x,y,z} and set lp->{sx,sy,sty}
 */
static void
solar_system(lp, scale, selt, celt, selg, celg, nx, ny)
HLoc *lp;
double scale;		/* mag factor */
double selt, celt;	/* sin/cos heliocentric lat of eye, rads */
double selg, celg;	/* sin/cos heliocentric lng of eye, rads */
unsigned nx, ny;	/* size of drawing area, in pixels */
{
	double x, y, z;	/* progressive transform values... */
	double xp, yp, zp;
	double xpp, ypp, zpp;
	double back;

	/* initial loc of points[i] */
	x = lp->x;
	y = lp->y;
	z = lp->z;

	/* rotate by -elg about z axis to get to xz plane.
	 * once we rotate up about x to the z axis (next step) that will put
	 * +x to the right and +y up.
	 * tmp = -elg;
	 * xp = x*cos(tmp) - y*sin(tmp);
	 * yp = x*sin(tmp) + y*cos(tmp);
	 */
	xp =  x*celg + y*selg;
	yp = -x*selg + y*celg;
	zp = z;

	/* rotate by -(PI/2-elt) about x axis to get to z axis.
	 * +x right, +y up, +z towards, all in AU.
	 * tmp = -(PI/2-elt);
	 * ypp = yp*cos(tmp) - zp*sin(tmp);
	 * zpp = yp*sin(tmp) + zp*cos(tmp);
	 */
	xpp = xp;
	ypp =  yp*selt + zp*celt;
	zpp = -yp*celt + zp*selt;

	/* now, straight ortho projection */
	lp->sx = nx/2 + xpp*scale;
	lp->sy = ny/2 - ypp*scale;

	/* back is y coord, in AU, behind which there is no parallax.
	 * parallax is the offset of the sun (at a=0), in pixels.
	 */
	back = (nx > ny ? nx : ny)/-2.0/scale;  /* based on screen size */
	if (zpp < back)
	    lp->stx = lp->sx;
	else
	    lp->stx = lp->sx + parallax*(back-zpp)/back;
}

/* called by trails.c to create a new set of trails for all solar sys objects.
 * TODO: client is unsed; any validation checks we can do?
 * return 0 if ok else -1.
 */
/* ARGSUSED */
static int
ss_newtrail (ts, statep, client)
TrTS ts[];
TrState *statep;
XtPointer client;
{
	Now *np = mm_get_now();
	int dbidx;
	Now n;

	watch_cursor (1);

	/* discard previous set and just insert the current position */
	hloc_reset(np, 1);

	/* work with a local copy since we change mjd */
	n = *np;

	/* add statep->nticks entries for each object.
	 * N.B. we tag earth's data (from SUN object) as object MOON.
	 */
	for (dbidx = 0; dbidx < NOBJ; dbidx++) {
	    int i;

	    for (i = 0; i < statep->nticks; i++) {
		TrTS *tp = &ts[i];

		/* set up n.n_mjd */
		n.n_mjd = tp->t;

		/* add an entry */
		if (hloc_add (dbidx, &n, tp->lbl) < 0)
		    break;
	    }
	}

	/* retain state for next time */
	trstate = *statep;

	/* redraw everything to display the new trails, if we are up */
	if (XtIsManaged(ssform_w))
	    ss_all ();

	watch_cursor (0);

	return (0);
}

/* set the first entry (the one for the current object location) for each
 * object at *np, and, if discard, erase any other entries.
 */
static void
hloc_reset (np, discard)
Now *np;
int discard;
{
	int dbidx;

	for (dbidx = 0; dbidx < NOBJ; dbidx++)  {
	    if (discard) {
		/* free all points memory */
		if (points[dbidx])
		    free ((char*)points[dbidx]);
		points[dbidx] = NULL;
		npoints[dbidx] = 0;
	    }

	    /* add a single entry for current location of object */
	    (void) hloc_add (dbidx, np, 0);
	}
}

/* make room for one more entry for the given db object in the points array and
 *   return its new address or NULL and xe_msg() if no more memory.
 */
static HLoc *
hloc_grow (dbidx)
int dbidx;
{
	char *newmem;
	int n = npoints[dbidx] + 1;

	if (points[dbidx])
	    newmem = realloc (points[dbidx], n*sizeof(HLoc));
	else
	    newmem = malloc (n*sizeof(HLoc));
	if (!newmem) {
	    xe_msg ("No memory for new point", 0);
	    return (NULL);
	}

	points[dbidx] = (HLoc *) newmem;
	npoints[dbidx] = n;

	return (&points[dbidx][n-1]);
}

/* add one entry for the given database object for np.
 * N.B. we tag earth's data (from SUN object) as object MOON.
 * return 0 if ok else xe_msg() and -1 if trouble.
 */
static int
hloc_add (dbidx, np, lbl)
int dbidx;
Now *np;
int lbl;
{
	Obj *op = db_basic (dbidx);
	HLoc *lp;
	double sd;

	/* skip objects not in solar system */
	if (!is_ssobj(op))
	    return(0);

	/* just one SUN entry will do since it is fixed at the center */
	if (dbidx == SUN && npoints[SUN] > 0)
	    return(0);

	/* add memory for one more entry */
	lp = hloc_grow (dbidx);
	if (!lp)
	    return (-1); /* go with what we have */

	/* compute the circumstances at np and save in lp->o.
	 * we get earth info from SUN then tag it as MOON.
	 */
	if (dbidx == MOON) {
	    /* really want earth info here; get it from SUN */
	    op = db_basic (SUN);
	    lp->o = *op;
	    op = &lp->o;
	    (void) obj_cir (np, op);
	    sd = op->s_edist;
	    op->pl.code = MOON;
	} else {
	    lp->o = *op;
	    op = &lp->o;
	    (void) obj_cir (np, op);
	    sd = op->s_sdist;
	}

	/* compute cartesian coords */
	lp->x = sd*cos(op->s_hlat)*cos(op->s_hlong);
	lp->y = sd*cos(op->s_hlat)*sin(op->s_hlong);
	lp->z = sd*sin(op->s_hlat);

	/* save the trail info */
	lp->trts.t = mjd;
	lp->trts.lbl = lbl;

	return (0);
}
