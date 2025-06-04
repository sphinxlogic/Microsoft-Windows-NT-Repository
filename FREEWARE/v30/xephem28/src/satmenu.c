/* code to manage the stuff on the "saturn" menu.
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
#include <Xm/CascadeB.h>
#include <Xm/Separator.h>
#include <Xm/Scale.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawingA.h>
#include <Xm/ToggleB.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"


extern Widget	toplevel_w;
#define	XtD	XtDisplay(toplevel_w)

extern Now *mm_get_now P_((void));
extern Obj *db_basic P_((int id));
extern Obj *db_scan P_((DBScan *sp));
extern double delra P_((double dra));
extern int any_ison P_((void));
extern int magdiam P_((int fmag, double scale, double mag, double size));
extern int obj_cir P_((Now *np, Obj *op));
extern int skyloc_fifo P_((int test, double ra, double dec, double yr,
    double fov, int mag));
extern void db_scaninit P_((DBScan *sp));
extern void db_update P_((Obj *op));
extern void f_double P_((Widget w, char *fmt, double f));
extern void f_pangle P_((Widget w, double a));
extern void f_ra P_((Widget w, double ra));
extern void fs_pangle P_((char out[], double a));
extern void fs_ra P_((char out[], double ra));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void mm_movie P_((double stepsz));
extern void obj_pickgc P_((Obj *op, Widget w, GC *gcp));
extern void register_selection P_((char *name));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void sm_update P_((Now *np, int how_much));
extern void sv_draw_obj P_((Display *dsp, Drawable win, GC gc, Obj *op, int x,
    int y, int diam, int dotsonly));
extern void timestamp P_((Now *np, Widget w));
extern void watch_cursor P_((int want));
extern void xe_msg P_((char *buf, int app_modal));

#define	MINR	10	/* min distance for picking, pixels */

/* These values are from the Explanatory Supplement.
 * Precession degrades them gradually over time.
 */
#define	POLE_RA		degrad(40.27)	/* RA of saturn's north pole */
#define	POLE_DEC	degrad(83.51)	/* Dec of saturn's north pole */

/* local record of what is now on screen for easy id from mouse picking.
 */
typedef struct {
    Obj o;		/* copy of object info.
    			 * copy from DB or, if saturn moon, we fill in just
			 * o_name and s_mag/ra/dec
			 */
    int x, y;		/* screen coord */
} ScreenObj;


static void sm_create_form_w P_((void));
static void sm_create_ssform_w P_((void));
static void sm_set_buttons P_((int whether));
static void sm_set_a_button P_((Widget pbw, int whether));
static void sm_sstats_cb P_((Widget w, XtPointer client, XtPointer call));
static void sm_option_cb P_((Widget w, XtPointer client, XtPointer call));
static void sm_scale_cb P_((Widget w, XtPointer client, XtPointer call));
static void sm_activate_cb P_((Widget w, XtPointer client, XtPointer call));
static void sm_close_cb P_((Widget w, XtPointer client, XtPointer call));
static void sm_movie_cb P_((Widget w, XtPointer client, XtPointer call));
static void sm_da_exp_cb P_((Widget w, XtPointer client, XtPointer call));
static void sm_da_input_cb P_((Widget w, XtPointer client, XtPointer call));
static void sm_create_popup P_((void));
static void sm_fill_popup P_((ScreenObj *sop));
static void sm_help_cb P_((Widget w, XtPointer client, XtPointer call));
static void sm_helpon_cb P_((Widget w, XtPointer client, XtPointer call));
static void sm_locfifo_cb P_((Widget w, XtPointer client, XtPointer call));
static void sm_calibline P_((Display *dsp, Drawable win, GC gc, int xc, int yc,
    char *tag, int tw, int th, int l));

static void add_screenobj P_((Obj *op, int x, int y));
static void reset_screenobj P_((void));
static ScreenObj *close_screenobj P_((int x, int y));

static void sky_background P_((Drawable win, unsigned w, unsigned h, int fmag,
    double ra0, double dec0, double salpha, double calpha, double scale, double
    rad, int fliptb, int fliplr));

static Widget satform_w;	/* main form */
static Widget ssform_w;		/* statistics form */
static Widget sda_w;		/* drawing area */
static Widget ringet_w, ringst_w;/* labels for displaying ring tilts */
static Widget scale_w;		/* size scale */
static Widget limmag_w;		/* limiting magnitude scale */
static Widget dt_w;		/* main date/time stamp widget */
static Widget sdt_w;		/* statistics date/time stamp widget */
static Widget skybkg_w;		/* toggle for controlling sky background */
static int sm_selecting;	/* set while our fields are being selected */
static int brmoons;		/* whether we want to brightten the moons */
static int tags;		/* whether we want tags on the drawing */
static int flip_tb;		/* whether we want to flip top/bottom */
static int flip_lr;		/* whether we want to flip left/right */
static int skybkg;		/* whether we want sky background */
static int sswasman;		/* whether statistics form was managed */

static Widget smpu_w;		/* main popup */
static Widget smpu_name_w;	/* popup name label */
static Widget smpu_ra_w;	/* popup RA label */
static Widget smpu_dec_w;	/* popup Dec label */
static Widget smpu_mag_w;	/* popup Mag label */

#define	MAXSCALE	10.0	/* max scale mag factor */
#define	MOVIE_STEPSZ	0.25	/* movie step size, hours */

/* arrays that use NM use [0] for the planet itself, and [1..NM-1] for moons.
 */
#define	NM	9		/* number of moons + 1 for planet */
static struct MoonNames {
    char *full;
    char *tag;
} mnames[NM] = {
    {"Saturn",	NULL},
    {"Mimas",	"I"},
    {"Enceladus","II"},
    {"Tethys",	"III"},
    {"Dione",	"IV"},
    {"Rhea",	"V"},
    {"Titan",	"VI"},
    {"Hyperion","VII"},
    {"Iapetus",	"VIII"},
};

enum {CX, CY, CZ, CRA, CDEC, CMAG, CNum};	/* s_w column index */
static Widget	s_w[NM][CNum];			/* the data display widgets */

/* malloced array for each item on screen. used for picking. */
static ScreenObj *screenobj;
static int nscreenobj;


/* called when the saturn menu is activated via the main menu pulldown.
 * if never called before, create and manage all the widgets as a child of a
 * form. otherwise, just toggle whether the form is managed.
 */
void
sm_manage ()
{
	if (!satform_w) {
	    sm_create_form_w();
	    sm_create_ssform_w();
	}
	
	if (XtIsManaged(satform_w)) {
	    XtUnmanageChild (satform_w);
	    if ((sswasman = XtIsManaged(ssform_w)))
		XtUnmanageChild(ssform_w);
	} else {
	    XtManageChild (satform_w);
	    if (sswasman) {
		XtManageChild (ssform_w);
		sm_set_buttons(sm_selecting);
	    }
	    /* rely on expose to do the first draw */
	}
}

/* called when the database has changed.
 * if we are drawing background, we'd best redraw everything.
 */
/* ARGSUSED */
void
sm_newdb (appended)
int appended;
{
	if (skybkg)
	    sm_update (mm_get_now(), 1);
}

int
sm_ison()
{
	return (satform_w && XtIsManaged(satform_w));
}

/* called by other menus as they want to hear from our buttons or not.
 * the "on"s and "off"s stack - only really redo the buttons if it's the
 * first on or the last off.
 */
void
sm_selection_mode (whether)
int whether;	/* whether setting up for plotting or for not plotting */
{
	sm_selecting += whether ? 1 : -1;

	if (sm_ison())
	    if ((whether && sm_selecting == 1)     /* first one to want on */
		|| (!whether && sm_selecting == 0) /* last one to want off */)
		sm_set_buttons (whether);
}

/* called to put up or remove the watch cursor.  */
void
sm_cursor (c)
Cursor c;
{
	Window win;

	if (satform_w && (win = XtWindow(satform_w))) {
	    Display *dsp = XtDisplay(satform_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}

	if (ssform_w && (win = XtWindow(ssform_w))) {
	    Display *dsp = XtDisplay(ssform_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}

/* create the main form dialog */
static void
sm_create_form_w()
{
	typedef struct {
	    char *name;		/* toggle button instance name */
	    char *label;	/* label */
	    int *flagp;		/* pointer to global flag, or NULL if none */
	    Widget *wp;		/* widget to save, or NULL */
	} Option;
	static Option options[] = {
	    {"SkyBkg",	   "Sky background",	&skybkg,  &skybkg_w},
	    {"BrightMoons","Bright moons",	&brmoons},
	    {"Tags",	   "Tags",		&tags},
	    {"FlipTB",	   "Flip T/B",		&flip_tb},
	    {"FlipLR",	   "Flip L/R",		&flip_lr},
	};
	typedef struct {
	    char *label;	/* what goes on the help label */
	    char *key;		/* string to call hlp_dialog() */
	} HelpOn;
	static HelpOn helpon[] = {
	    {"Intro...",	"Saturn - intro"},
	    {"on Mouse...",	"Saturn - mouse"},
	    {"on Control...",	"Saturn - control"},
	    {"on View...",	"Saturn - view"},
	};
	Widget w;
	Widget frame_w;
	Widget mb_w, pd_w, cb_w;
	XmString str;
	Arg args[20];
	int n;
	int i;

	/* create form */
	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNdefaultPosition, False); n++;
	XtSetArg (args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
	satform_w = XmCreateFormDialog (toplevel_w, "Saturn", args, n);
	XtAddCallback (satform_w, XmNhelpCallback, sm_help_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Saturn view"); n++;
	XtSetValues (XtParent(satform_w), args, n);

	/* create the menu bar across the top */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	mb_w = XmCreateMenuBar (satform_w, "MB", args, n);
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

	    /* add the "LOCFIFO" push button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "LOCFIFO", args, n);
	    set_xmstring (w, XmNlabelString, "Send to LOCFIFO");
	    XtAddCallback (w, XmNactivateCallback, sm_locfifo_cb, 0);
	    XtManageChild (w);

	    /* add the "movie" push button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Movie", args, n);
	    set_xmstring (w, XmNlabelString, "Movie Demo");
	    XtAddCallback (w, XmNactivateCallback, sm_movie_cb, 0);
	    XtManageChild (w);

	    /* add the "close" push button beneath a separator */

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Close", args, n);
	    XtAddCallback (w, XmNactivateCallback, sm_close_cb, 0);
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

	    for (i = 0; i < XtNumber(options); i++) {
		Option *op = &options[i];

		n = 0;
		XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
		XtSetArg (args[n], XmNmarginHeight, 0); n++;
		XtSetArg (args[n], XmNindicatorType, XmN_OF_MANY); n++;
		w = XmCreateToggleButton (pd_w, op->name, args, n);
		XtAddCallback (w, XmNvalueChangedCallback, sm_option_cb, 
						    (XtPointer)(op->flagp));
		set_xmstring (w, XmNlabelString, op->label);
		XtManageChild (w);
		if (op->flagp)
		    *(op->flagp) = XmToggleButtonGetState(w);
		if (op->wp)
		    *op->wp = w;
	    }

	    /* add a separator */

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    /* add the More Info control */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Stats", args, n);
	    set_xmstring (w, XmNlabelString, "More info...");
	    XtAddCallback (w, XmNactivateCallback, sm_sstats_cb, NULL);
	    XtManageChild (w);

	/* make the help pulldown */

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
		HelpOn *hp = &helpon[i];

		str = XmStringCreate (hp->label, XmSTRING_DEFAULT_CHARSET);
		n = 0;
		XtSetArg (args[n], XmNlabelString, str); n++;
		XtSetArg (args[n], XmNmarginHeight, 0); n++;
		w = XmCreatePushButton (pd_w, "Help", args, n);
		XtAddCallback (w, XmNactivateCallback, sm_helpon_cb,
							(XtPointer)(hp->key));
		XtManageChild (w);
		XmStringFree(str);
	    }

	/* make the date/time stamp label */

	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	dt_w = XmCreateLabel (satform_w, "DateStamp", args, n);
	timestamp (mm_get_now(), dt_w);	/* establishes size */
	XtManageChild (dt_w);

	/* make the scale widget.
	 * attach both top and bottom so it's the one to follow resizing.
	 */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, dt_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNmaximum, 100); n++;
	XtSetArg (args[n], XmNminimum, 0); n++;
	XtSetArg (args[n], XmNscaleMultiple, 10); n++;
	XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_TOP); n++;
	scale_w = XmCreateScale (satform_w, "Scale", args, n);
	XtAddCallback (scale_w, XmNdragCallback, sm_scale_cb, 0);
	XtAddCallback (scale_w, XmNvalueChangedCallback, sm_scale_cb, 0);
	XtManageChild (scale_w);

	/* make the limiting mag scale widget.
	 * attach both top and bottom so it's the one to follow resizing.
	 */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, dt_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNmaximum, 20); n++;
	XtSetArg (args[n], XmNminimum, 0); n++;
	XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_TOP); n++;
	XtSetArg (args[n], XmNshowValue, True); n++;
	limmag_w = XmCreateScale (satform_w, "LimMag", args, n);
	XtAddCallback (limmag_w, XmNdragCallback, sm_scale_cb, 0);
	XtAddCallback (limmag_w, XmNvalueChangedCallback, sm_scale_cb, 0);
	XtManageChild (limmag_w);

	/* make a frame for the drawing area.
	 * attach both top and bottom so it's the one to follow resizing.
	 */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, dt_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNleftWidget, scale_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNrightWidget, limmag_w); n++;
	XtSetArg (args[n], XmNshadowType, XmSHADOW_ETCHED_OUT); n++;
	frame_w = XmCreateFrame (satform_w, "SatFrame", args, n);
	XtManageChild (frame_w);

	    /* make a drawing area for drawing the little map */

	    n = 0;
	    sda_w = XmCreateDrawingArea (frame_w, "Map", args, n);
	    XtAddCallback (sda_w, XmNexposeCallback, sm_da_exp_cb, 0);
	    XtAddCallback (sda_w, XmNinputCallback, sm_da_input_cb, 0);
	    XtManageChild (sda_w);
}

/* make the statistics form dialog */
static void
sm_create_ssform_w()
{
	typedef struct {
	    int col;		/* C* column code */
	    char *rcname;	/* name of rowcolumn */
	    char *collabel;	/* column label */
	    char *suffix;	/* suffix for plot/list/search name */
	} MoonColumn;
	static MoonColumn mc[] = {
	    {CX,   "SatX",   "X (+E)",     "X"},
	    {CY,   "SatY",   "Y (+S)",     "Y"},
	    {CZ,   "SatZ",   "Z (+front)", "Z"},
	    {CRA,  "SatRA",  "RA",         "RA"},
	    {CDEC, "SatDec", "Dec",        "Dec"},
	    {CMAG, "SatMag", "Mag",        "Mag"},
	};
	Widget w;
	Widget rc_w, title_w, sep_w;
	Arg args[20];
	int n;
	int i;

	/* create form */
	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNresizePolicy, XmRESIZE_ANY); n++;
	XtSetArg (args[n], XmNverticalSpacing, 5); n++;
	ssform_w = XmCreateFormDialog (toplevel_w, "SaturnStats", args, n);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Saturn info"); n++;
	XtSetValues (XtParent(ssform_w), args, n);

	/* make ring tilt label */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	title_w = XmCreateLabel (ssform_w, "SatTL", args, n);
	XtManageChild (title_w);
	set_xmstring (title_w, XmNlabelString, "Ring tilt (degrees, front +S)");

	/* make the earth ring tilt r/c */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, title_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
	XtSetArg (args[n], XmNnumColumns, 1); n++;
	rc_w = XmCreateRowColumn (ssform_w, "SatERC", args, n);
	XtManageChild (rc_w);

	    n = 0;
	    w = XmCreateLabel (rc_w, "SatETMsg", args, n);
	    set_xmstring (w, XmNlabelString, "From Earth:");
	    XtManageChild (w);

	    n = 0;
	    XtSetArg (args[n], XmNuserData, "Saturn.ETilt"); n++;
	    ringet_w = XmCreatePushButton (rc_w, "SatET", args, n);
	    XtAddCallback (ringet_w, XmNactivateCallback, sm_activate_cb, 0);
	    XtManageChild (ringet_w);

	/* make the sun tilt r/c */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, title_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
	XtSetArg (args[n], XmNnumColumns, 1); n++;
	rc_w = XmCreateRowColumn (ssform_w, "SatSRC", args, n);
	XtManageChild (rc_w);

	    n = 0;
	    w = XmCreateLabel (rc_w, "SatSTMsg", args, n);
	    set_xmstring (w, XmNlabelString, "From Sun:");
	    XtManageChild (w);

	    n = 0;
	    XtSetArg (args[n], XmNuserData, "Saturn.STilt"); n++;
	    ringst_w = XmCreatePushButton (rc_w, "SatST", args, n);
	    XtAddCallback (ringst_w, XmNactivateCallback, sm_activate_cb, 0);
	    XtManageChild (ringst_w);

	/* make table title label */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNtopOffset, 10); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	title_w = XmCreateLabel (ssform_w, "SatML", args, n);
	XtManageChild (title_w);
	set_xmstring (title_w, XmNlabelString,"Moon Positions (Saturn radii)");

	/* make the moon table, one column at a time */

	/* moon designator column */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, title_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
	XtSetArg (args[n], XmNisAligned, True); n++;
	rc_w = XmCreateRowColumn (ssform_w, "SatDes", args, n);
	XtManageChild (rc_w);

	    n = 0;
	    w = XmCreateLabel (rc_w, "TL", args, n);
	    XtManageChild (w);
	    set_xmstring (w, XmNlabelString, "Tag");

	    for (i = 0; i < NM; i++) {
		char *tag = mnames[i].tag;

		n = 0;
		w = XmCreatePushButton (rc_w, "SatTag", args, n); /*PB for sz */
		set_xmstring (w, XmNlabelString, tag ? tag : " ");
		sm_set_a_button (w, False);
		XtManageChild (w);
	    }

	/* moon name column */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, title_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNleftWidget, rc_w); n++;
	XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
	XtSetArg (args[n], XmNisAligned, True); n++;
	rc_w = XmCreateRowColumn (ssform_w, "SatName", args, n);
	XtManageChild (rc_w);

	    n = 0;
	    w = XmCreateLabel (rc_w, "NL", args, n);
	    XtManageChild (w);
	    set_xmstring (w, XmNlabelString, "Name");

	    for (i = 0; i < NM; i++) {
		n = 0;
		w = XmCreatePushButton (rc_w, "SatName", args, n); /*PB for sz*/
		set_xmstring (w, XmNlabelString, mnames[i].full);
		sm_set_a_button (w, False);
		XtManageChild (w);
	    }

	/* make each of the X/Y/Z/Mag/RA/DEC information columns */

	for (i = 0; i < XtNumber (mc); i++) {
	    MoonColumn *mp = &mc[i];
	    int j;

	    n = 0;
	    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	    XtSetArg (args[n], XmNtopWidget, title_w); n++;
	    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	    XtSetArg (args[n], XmNleftWidget, rc_w); n++;
	    XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
	    XtSetArg (args[n], XmNisAligned, True); n++;
	    rc_w = XmCreateRowColumn (ssform_w, mp->rcname, args, n);
	    XtManageChild (rc_w);

	    n = 0;
	    w = XmCreateLabel (rc_w, "SatLab", args, n);
	    XtManageChild (w);
	    set_xmstring (w, XmNlabelString, mp->collabel);

	    for (j = 0; j < NM; j++) {
		char *sel;

		sel = XtMalloc (strlen(mnames[j].full) + strlen(mp->suffix)+2);
		(void) sprintf (sel, "%s.%s", mnames[j].full, mp->suffix);

		n = 0;
		XtSetArg (args[n], XmNuserData, sel); n++;
		w = XmCreatePushButton(rc_w, "SatPB", args, n);
		XtAddCallback(w, XmNactivateCallback, sm_activate_cb, 0);
		XtManageChild (w);
		s_w[j][mp->col] = w;
	    }
	}

	/* make a separator */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (ssform_w, "Sep1", args, n);
	XtManageChild (sep_w);

	/* make a date/time stamp */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sdt_w = XmCreateLabel (ssform_w, "JDateStamp", args, n);
	XtManageChild (sdt_w);

	/* make a separator */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sdt_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (ssform_w, "Sep2", args, n);
	XtManageChild (sep_w);

	/* make the close button */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 30); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 70); n++;
	w = XmCreatePushButton (ssform_w, "Close", args, n);
	XtAddCallback (w, XmNactivateCallback, sm_sstats_cb, 0);
	XtManageChild (w);
}

/* go through all the buttons pickable for plotting and set whether they
 * should appear to look like buttons or just flat labels.
 */
static void
sm_set_buttons (whether)
int whether;	/* whether setting up for plotting or for not plotting */
{
	int i, j;

	for (i = 0; i < NM; i++) {
	    for (j = 0; j < CNum; j++)
		sm_set_a_button (s_w[i][j], whether);
	}
	sm_set_a_button (ringet_w, whether);
	sm_set_a_button (ringst_w, whether);
}

/* set whether the given button looks like a label.
 */
static void
sm_set_a_button(pbw, whether)
Widget pbw;
int whether;
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
	    Widget tmpw;
	    int n;

	    n = 0;
	    tmpw = XmCreatePushButton (satform_w, "tmp", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNtopShadowColor, &topshadcol); n++;
	    XtSetArg (args[n], XmNbottomShadowColor, &botshadcol); n++;
            XtSetArg (args[n], XmNtopShadowPixmap, &topshadpm); n++;
            XtSetArg (args[n], XmNbottomShadowPixmap, &botshadpm); n++;
	    XtSetArg (args[n], XmNbackground, &bgcol); n++;
	    XtGetValues (tmpw, args, n);

            look_like_button[0].value = topshadcol;
            look_like_button[1].value = botshadcol;
            look_like_button[2].value = topshadpm;
            look_like_button[3].value = botshadpm;
            look_like_label[0].value = bgcol;
            look_like_label[1].value = bgcol;
            look_like_label[2].value = XmUNSPECIFIED_PIXMAP;
            look_like_label[3].value = XmUNSPECIFIED_PIXMAP;

	    XtDestroyWidget (tmpw);
	     
	    called = 1;
	}

	if (whether) {
	    ap = look_like_button;
	    na = XtNumber(look_like_button);
	} else {
	    ap = look_like_label;
	    na = XtNumber(look_like_label);
	}

	XtSetValues (pbw, ap, na);
}

/* callback when the Close button is activated on the stats menu or when
 * the More Info button is activated. they each do the same thing.
 */
/* ARGSUSED */
static void
sm_sstats_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if ((sswasman = XtIsManaged(ssform_w)))
	    XtUnmanageChild (ssform_w);
	else {
	    XtManageChild (ssform_w);
	    sm_set_buttons(sm_selecting);
	}
}

/* callback from any of the option buttons.
 * client points to global flag to set; some don't have any.
 * in any case then just redraw everything.
 */
/* ARGSUSED */
static void
sm_option_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (client)
	    *((int *)client) = XmToggleButtonGetState(w);

	sm_update (mm_get_now(), 1);
}

/* callback from the scales changing.
 * just redraw everything.
 */
/* ARGSUSED */
static void
sm_scale_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	sm_update (mm_get_now(), 1);
}

/* callback from any of the data menu buttons being activated.
 */
/* ARGSUSED */
static void
sm_activate_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (sm_selecting) {
	    char *name;
	    get_something (w, XmNuserData, (XtArgVal)&name);
	    register_selection (name);
	}
}

/* callback from the main Close button */
/* ARGSUSED */
static void
sm_close_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (satform_w);
	if ((sswasman = XtIsManaged(ssform_w)))
	    XtUnmanageChild (ssform_w);

	/* stop movie that might be running */
	mm_movie (0.0);
}

/* callback from the Movie button
 */
/* ARGSUSED */
static void
sm_movie_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	/* best effect if turn off worrying about the sky background */
	skybkg = 0;
	XmToggleButtonSetState (skybkg_w, False, False);

	mm_movie (MOVIE_STEPSZ);
}

/* callback from either expose or resize of the drawing area.
 */
/* ARGSUSED */
static void
sm_da_exp_cb (w, client, call)
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
	    printf ("Unexpected satform_w event. type=%d\n", c->reason);
	    exit(1);
	}

	sm_update (mm_get_now(), 1);
}

/* callback from mouse or keyboard input over drawing area */
/* ARGSUSED */
static void
sm_da_input_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;
	XEvent *ev;
	ScreenObj *sop;
	int x, y;

	/* get x and y value iff it was from mouse button 3 */
	if (c->reason != XmCR_INPUT)
	    return;
	ev = c->event;
	if (ev->xany.type != ButtonPress || ev->xbutton.button != 3)
	    return;
	x = ev->xbutton.x;
	y = ev->xbutton.y;

	/* find something close on the screen */
	sop = close_screenobj (x, y);
	if (!sop)
	    return;

	/* put info in popup, smpu_w, creating it first is necessary  */
	if (!smpu_w)
	    sm_create_popup();
	sm_fill_popup (sop);

	/* put it on screen */
	XmMenuPosition (smpu_w, (XButtonPressedEvent *)ev);
	XtManageChild (smpu_w);
}

/* create the (unmanaged for now) popup menu in smpu_w. */
static void
sm_create_popup()
{
	static Widget *puw[] = {
	    &smpu_name_w,
	    &smpu_ra_w,
	    &smpu_dec_w,
	    &smpu_mag_w,
	};
	Widget w;
	Arg args[20];
	int n;
	int i;

	n = 0;
	XtSetArg (args[n], XmNisAligned, True); n++;
	XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
	smpu_w = XmCreatePopupMenu (sda_w, "SatPU", args, n);

	/* stack everything up in labels */
	for (i = 0; i < XtNumber(puw); i++) {
	    n = 0;
	    w = XmCreateLabel (smpu_w, "SPUL", args, n);
	    XtManageChild (w);
	    *puw[i] = w;
	}
}

/* put up a popup at ev with info about sop */
static void
sm_fill_popup (sop)
ScreenObj *sop;
{
	char *name;
	double ra, dec, mag;
	char buf[64], buf2[64];

	name = sop->o.o_name;
	mag = sop->o.s_mag/MAGSCALE;
	ra = sop->o.s_ra;
	dec = sop->o.s_dec;

	(void) sprintf (buf2, "Name: %.*s", MAXNM, name);
	set_xmstring (smpu_name_w, XmNlabelString, buf2);

	fs_ra (buf, ra);
	(void) sprintf (buf2, "  RA: %s", buf);
	set_xmstring (smpu_ra_w, XmNlabelString, buf2);

	fs_pangle (buf, dec);
	(void) sprintf (buf2, " Dec: %s", buf);
	set_xmstring (smpu_dec_w, XmNlabelString, buf2);

	(void) sprintf (buf2, " Mag: %.3g", mag);
	set_xmstring (smpu_mag_w, XmNlabelString, buf2);
}

/* callback from the Help all button
 */
/* ARGSUSED */
static void
sm_help_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static char *msg[] = {
"This is a simple schematic depiction of Saturn and its moons.",
};

	hlp_dialog ("Saturn", msg, XtNumber(msg));
}

/* callback from a specific Help button.
 * client is a string to use with hlp_dialog().
 */
/* ARGSUSED */
static void
sm_helpon_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	hlp_dialog ((char *)client, NULL, 0);
}

/* callback from the loc fifo control.
 */
/* ARGSUSED */
static void
sm_locfifo_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	Now *np = mm_get_now();
	Obj *op = db_basic (SATURN);

	/* get everything nearby */
	(void) skyloc_fifo (0, op->s_ra, op->s_dec, epoch, degrad(1.0), 20);
}

/* add one entry to the global screenobj array.
 */
static void
add_screenobj (op, x, y)
Obj *op;
int x, y;
{
	ScreenObj *mem;
	ScreenObj *sop;

	/* grow screenobj by one */
	if (screenobj)
	    mem = (ScreenObj *) realloc ((char *)screenobj,
					    (nscreenobj+1)*sizeof (ScreenObj));
	else
	    mem = (ScreenObj *) malloc (sizeof (ScreenObj));
	if (!mem) {
	    char msg[256];
	    (void) sprintf (msg, "Out of memory -- %s will not be pickable",
								    op->o_name);
	    xe_msg (msg, 0);
	    return;
	}
	screenobj = mem;

	sop = &screenobj[nscreenobj++];

	/* fill new entry */
	sop->o = *op;
	sop->x = x;
	sop->y = y;
}

/* reclaim any existing screenobj entries */
static void
reset_screenobj()
{
	if (screenobj) {
	    free ((char *)screenobj);
	    screenobj = NULL;
	}
	nscreenobj = 0;
}

/* find the entry in screenobj closest to [x,y] within MINR.
 * if found return the ScreenObj *, else NULL.
 */
static ScreenObj *
close_screenobj (x, y)
int x, y;
{
	int minr;
	ScreenObj *sop, *minsop = NULL;

	minr = 2*MINR;
	for (sop = &screenobj[nscreenobj]; --sop >= screenobj; ) {
	    int r = abs(x - sop->x) + abs(y - sop->y);
	    if (r < minr) {
		minr = r;
		minsop = sop;
	    }
	}
	if (minr <= MINR)
	    return (minsop);
	else
	    return (NULL);
}

typedef struct {
    double x, y, z;	/* radii: +x:east +y:south +z:front */
    double ra, dec;
    double mag;
} MoonData;

/* functions */
static void saturn_data();
static void moonradec();
static void sm_draw_map ();

/* called to recompute and fill in values for the saturn menu.
 * don't bother if it doesn't exist or is unmanaged now or no one is logging.
 */
void
sm_update (np, how_much)
Now *np;
int how_much;
{
	static char fmt[] = "%7.3f";
	MoonData md[NM];
	int wantstats;
	double etilt, stilt;
	double satsize;
	double theta, phi;
	double tvc, pvc;
	double salpha, calpha;
	int i;

	/* see if we should bother */
	if (!satform_w)
	    return;
	wantstats = XtIsManaged(ssform_w) || any_ison() || how_much;
	if (!XtIsManaged(satform_w) && !wantstats)
	    return;

	watch_cursor (1);

	/* compute md[0].x/y/z/mag/ra/dec and md[1..NM-1].x/y/z/mag info */
        saturn_data(mjd + MJD0, &satsize, &etilt, &stilt, md);

	/* compute sky transformation angle as triple vector product */
	tvc = PI/2.0 - md[0].dec;
	pvc = md[0].ra;
	theta = PI/2 - POLE_DEC;
	phi = POLE_RA;
	salpha = sin(tvc)*sin(theta)*(cos(pvc)*sin(phi) - sin(pvc)*cos(phi));
	calpha = sqrt (1.0 - salpha*salpha);

	/* find md[1..NM-1].ra/dec from md[0].ra/dec and md[1..NM-1].x/y */
	moonradec (satsize, salpha, calpha, md);

	if (wantstats) {
	    for (i = 0; i < NM; i++) {
		f_double (s_w[i][CX], fmt, md[i].x);
		f_double (s_w[i][CY], fmt, md[i].y);
		f_double (s_w[i][CZ], fmt, md[i].z);
		f_double (s_w[i][CMAG], "%4.1f", md[i].mag);
		f_ra (s_w[i][CRA], md[i].ra);
		f_pangle (s_w[i][CDEC], md[i].dec);
	    }

	    f_double (ringet_w, fmt, raddeg(etilt));
	    f_double (ringst_w, fmt, raddeg(stilt));
	    timestamp (np, sdt_w);
	}

	if (XtIsManaged(satform_w)) {
	    sm_draw_map (sda_w, etilt, satsize, salpha, calpha, md);
	    timestamp (np, dt_w);
	}

	watch_cursor (0);
}

/* given the loc of the moons, draw a nifty little picture.
 * also save resulting screen locs of everything in the screenobj array.
 * scale of the locations is in terms of saturn radii == 1.
 * unflipped view is S up, E right.
 * positive tilt means the front rings are tilted southward, in rads.
 * planet itself is in md[0], moons in md[1..NM-1].
 *     +md[].x: East, sat radii
 *     +md[].y: South, sat radii
 *     +md[].z: in front, sat radii
 */
static void
sm_draw_map (w, etilt, satsize, salpha, calpha, md)
Widget w;
double etilt;
double satsize;
double salpha, calpha;
MoonData md[NM];
{
	static GC s_fgc, s_bgc, s_xgc;
	static XFontStruct *s_fs;
	static last_nx, last_ny;
	static int cw, ch;
	static Pixmap pm;
	Display *dsp = XtDisplay(w);
	Window win = XtWindow(w);
	Window root;
	double scale;
	int sv;
	int fmag;
	char c;
	int x, y;
	int irw, irh, orw, orh, irx, iry, orx, ory;
	unsigned int nx, ny, bw, d;
	int ns = flip_tb ? -1 : 1;
	int ew = flip_lr ? -1 : 1;
	int i;

#define RLW     3       /* ring line width, pixels */
#define NORM    60.0    /* max Iapetus orbit radius; used to normalize */
#define MAPSCALE(r)     ((r)*((int)nx)/NORM/2*scale)
#define	XCORD(x)	((int)(((int)nx)/2.0 + ew*MAPSCALE(x) + 0.5))
#define	YCORD(y)	((int)(((int)ny)/2.0 - ns*MAPSCALE(y) + 0.5))

	if (!s_fgc) {
	    XGCValues gcv;
	    unsigned int gcm;
	    Pixel fg, bg;

	    gcm = GCForeground;
	    get_something (w, XmNforeground, (XtArgVal)&fg);
	    gcv.foreground = fg;
	    s_fgc = XCreateGC (dsp, win, gcm, &gcv);
	    s_fs = XQueryFont (dsp, XGContextFromGC (s_fgc));
	    cw = s_fs->max_bounds.width;
	    ch = s_fs->max_bounds.ascent + s_fs->max_bounds.descent;

	    gcm = GCForeground;
	    get_something (w, XmNbackground, (XtArgVal)&bg);
	    gcv.foreground = bg;
	    s_bgc = XCreateGC (dsp, win, gcm, &gcv);

	    gcm = GCForeground | GCFunction;
	    gcv.foreground = fg ^ bg;
	    gcv.function = GXxor;
	    s_xgc = XCreateGC (dsp, win, gcm, &gcv);
	}

	XmScaleGetValue (limmag_w, &fmag);
	XmScaleGetValue (scale_w, &sv);
	scale = pow(MAXSCALE, sv/100.0);

	XGetGeometry(dsp, win, &root, &x, &y, &nx, &ny, &bw, &d);
	if (!pm || nx != last_nx || ny != last_ny) {
	    if (pm)
		XFreePixmap (dsp, pm);
	    pm = XCreatePixmap (dsp, win, nx, ny, d);
	    last_nx = nx;
	    last_ny = ny;
	}

	XFillRectangle (dsp, pm, s_bgc, 0, 0, nx, ny);

	/* prepare for a new list of things on the screen */
	reset_screenobj();

	/* draw background objects, if desired */
	if (skybkg)
	    sky_background(pm, nx, ny, fmag, md[0].ra, md[0].dec, salpha,calpha,
			satsize/MAPSCALE(2), satsize*NORM, flip_tb, flip_lr);

	/* draw labels */
	c = flip_lr ? 'W' : 'E';
	XDrawString(dsp, pm, s_fgc, nx-cw-1, ny/2-2, &c, 1);
	c = flip_tb ? 'N' : 'S';
	XDrawString(dsp, pm, s_fgc, (nx-cw)/2-1, s_fs->ascent, &c, 1);

	/* draw saturn in center with unit radius */
	XFillArc (dsp, pm, s_fgc, nx/2-(int)MAPSCALE(1), ny/2-(int)MAPSCALE(1),
			    (int)MAPSCALE(2), (int)MAPSCALE(2), 0, 360*64);
	add_screenobj (db_basic(SATURN), nx/2, ny/2);

	/* rings of radius IRR and ORR.
	 * draw rings in front of planet using xor.
	 * positive tilt means the front rings are tilted southward.
	 * always draw the solid s_fgc last in case we are near the ring plane.
	 */
#define	IRR	1.528	/* inner edge of ring system */
#define	ORR	2.267	/* outter edge of A ring */
	irh = MAPSCALE(2*IRR*fabs(sin(etilt)));
	irw = (int)MAPSCALE(2*IRR);
	orh = MAPSCALE(2*ORR*fabs(sin(etilt)));
	orw = (int)MAPSCALE(2*ORR);
	irx = nx/2 - MAPSCALE(IRR);
	iry = ny/2 - MAPSCALE(IRR*fabs(sin(etilt)));
	orx = nx/2 - MAPSCALE(ORR);
	ory = ny/2 - MAPSCALE(ORR*fabs(sin(etilt)));
	if (irh < RLW || orh < RLW) {
	    /* too near the ring plane to draw a fill ellipse */
	    XDrawLine (dsp, pm, s_fgc, orx, ny/2, nx-orx, ny/2);
	} else {
	    /* near rings are up if tilt is positive and we are not flipping n/s
	     * or if tilt is negative and we are flipping n/s.
	     */
	    int nearup = ((etilt>0.0) == !flip_tb);

	    XDrawArc (dsp, pm, s_xgc, irx, iry, irw, irh,
					     nearup ? 0 : 180*64, 180*64-1);
	    XDrawArc (dsp, pm, s_xgc, orx, ory, orw, orh,
					     nearup ? 0 : 180*64, 180*64-1);
	    XDrawArc (dsp, pm, s_fgc, irx, iry, irw, irh,
					     nearup ? 180*64 : 0, 180*64-1);
	    XDrawArc (dsp, pm, s_fgc, orx, ory, orw, orh,
					     nearup ? 180*64 : 0, 180*64-1);
	}

	/* draw 1' calibration line */
	if (tags)
	    sm_calibline (dsp, pm, s_fgc, nx/2, 5*ny/6, "1'", cw*2, ch+4,
				(int)MAPSCALE(degrad(1.0/60.0)/(satsize/2)));

	/* draw each moon
	 */
	for (i = 1; i < NM; i++) {
	    double mx = md[i].x;
	    double my = md[i].y;
	    double mz = md[i].z;
	    double mag = md[i].mag;
	    int outside = mx*mx + my*my > 1.0;
	    int infront = mz > 0.0;
	    int diam;
	    Obj o;

	    if (!outside && !infront)
		continue;	/* behind saturn */
	    diam = magdiam (fmag, satsize/MAPSCALE(2), mag, 0.0);
	    if (brmoons)
		diam += 3;
	    if (diam <= 0)
		continue;	/* too faint */

	    x = XCORD(mx);
	    y = YCORD(my);
	    if (diam == 1)
		XDrawPoint(dsp, pm, s_xgc, x, y);
	    else
		XFillArc (dsp, pm, s_xgc, x-diam/2, y-diam/2, diam, diam,
								    0, 360*64);

	    /* add object to list of screen objects drawn */
	    (void) strcpy (o.o_name, mnames[i].full);
	    o.s_mag = mag*MAGSCALE;
	    o.s_ra = md[i].ra;
	    o.s_dec = md[i].dec;
	    add_screenobj (&o, x, y);

	    if (tags && mnames[i].tag)
		XDrawString(dsp, pm, s_xgc, x-cw/2, y+2*ch,
					mnames[i].tag, strlen(mnames[i].tag));
	}

	XCopyArea (dsp, pm, win, s_fgc, 0, 0, nx, ny, 0, 0);
}

/* draw a calibration line l pixels long centered at [xc,yc] marked with tag
 * with is in a bounding box tw x th.
 */
static void
sm_calibline (dsp, win, gc, xc, yc, tag, tw, th, l)
Display *dsp;
Drawable win;
GC gc;
int xc, yc;
char *tag;
int tw, th;
int l;
{
	int lx = xc - l/2;
	int rx = lx + l;

	XDrawLine (dsp, win, gc, lx, yc, rx, yc);
	XDrawLine (dsp, win, gc, lx, yc-3, lx, yc+3);
	XDrawLine (dsp, win, gc, rx, yc-3, rx, yc+3);
	XDrawString (dsp, win, gc, xc-tw/2, yc+th, tag, strlen(tag));
}

/* draw all database objects in a small sky patch with the given orientation.
 * also save objects and screen locs in the global screenobj array.
 * this is used to draw the backgrounds for the planet closeups.
 * Based on work by: Dan Bruton <WDB3926@acs.tamu.edu>
 */
static void
sky_background (win, w, h, fmag, ra0,dec0,salpha,calpha,scale,rad,fliptb,fliplr)
Drawable win;		/* window to draw on */
unsigned w, h;		/* window size */
int fmag;		/* faintest magnitude to display */
double ra0, dec0;	/* center of patch, rads */
double salpha, calpha;	/* transformation parameters */
double scale;		/* rads per pixel */
double rad;		/* maximum radius to draw away from ra0/dec0, rads */
int fliptb, fliplr;	/* flip direction; default is S up E right */
{
	double cdec0 = cos(dec0);
	DBScan dbs;
	Obj *op;

	/* scan the database and draw whatever is near */
	for (db_scaninit(&dbs); (op = db_scan (&dbs)) != NULL; ) {
	    double dra, ddec;
	    GC gc;
	    int dx, dy, x, y;
	    int diam;

	    if (op->type == UNDEFOBJ)
		continue;

	    if (is_planet (op, SATURN)) {
		/* we draw it elsewhere */
		continue;
	    }

	    db_update (op);

	    /* find size, in pixels. */
	    diam = magdiam (fmag, scale, op->s_mag/MAGSCALE,
					    degrad((double)op->s_size/3600.0));
	    if (diam <= 0)
		continue;

	    /* find x/y location if it's in the general area */
	    dra = op->s_ra - ra0;	/* + E */
	    ddec = dec0 - op->s_dec;	/* + S */
	    if (fabs(ddec) > rad || delra(dra)*cdec0 > rad)
		continue;
	    dx = (dra*calpha - ddec*salpha)/scale;
	    dy = (dra*salpha + ddec*calpha)/scale;
	    x = fliplr ? (int)w/2-dx : (int)w/2+dx;
	    y = fliptb ? (int)h/2+dy : (int)h/2-dy;

	    /* pick a gc */
	    obj_pickgc(op, toplevel_w, &gc);

	    /* draw 'er */
	    sv_draw_obj (XtD, win, gc, op, x, y, diam, 0);

	    /* save 'er */
	    add_screenobj (op, x, y);
	}

	sv_draw_obj (XtD, win, (GC)0, NULL, 0, 0, 0, 0);	/* flush */
}

/* given saturn loc in md[0].ra/dec and size, and location of each moon in 
 * md[1..NM-1].x/y in sat radii, find ra/dec of each moon in md[1..NM-1].ra/dec.
 */
static void
moonradec (satsize, salpha, calpha, md)
double satsize;		/* sat diameter, rads */
double salpha, calpha;	/* transormation parameters */
MoonData md[NM];	/* fill in RA and Dec */
{
	double satrad = satsize/2;
	double satra = md[0].ra;
	double satdec = md[0].dec;
	int i;

	for (i = 1; i < NM; i++) {
	    double dra  = satrad * ( md[i].x*calpha + md[i].y*salpha);
	    double ddec = satrad * (-md[i].x*salpha + md[i].y*calpha);
	    md[i].ra  = satra + dra;
	    md[i].dec = satdec - ddec;
	}
}

/*  */
/*     SS2TXT.BAS                     Dan Bruton, astro@tamu.edu */
/*  */
/*       This is a text version of SATSAT2.BAS.  It is smaller, */
/*    making it easier to convert other languages (250 lines */
/*    compared to 850 lines). */
/*  */
/*       This BASIC program computes and displays the locations */
/*    of Saturn's Satellites for a given date and time.  See */
/*    "Practical Astronomy with your Calculator" by Peter */
/*    Duffett-Smith and the Astronomical Almanac for explanations */
/*    of some of the calculations here.  The code is included so */
/*    that users can make changes or convert to other languages. */
/*    This code was made using QBASIC (comes with DOS 5.0). */
/*  */
/*    ECD: merged with Sky and Tel, below, for better earth and sun ring tilt */
/*  */

/* ECD: BASICeze */
#define	FOR	for
#define	IF	if
#define	ELSE	else
#define	COS	cos
#define	SIN	sin
#define	TAN	tan
#define ATN	atan
#define ABS	fabs
#define SQR	sqrt

static void ring_tilt P_((double JD, double *etiltp, double *stiltp));

/* ECD: this originally computed +X:East +Y:North +Z:behind in [1..8] indeces.
 * ECD: and +tilt:front south, rads
 * ECD: then we adjust things in md[].x/y/z/mag to fit into our MoonData format.
 * ECD: we also add saturn info in md[0].x/y/z/ra/dec/mag.
 */
static void
saturn_data (JD, sizep, etiltp, stiltp, md)
double JD;
double *sizep;			/* saturn's angular diam, rads */
double *etiltp, *stiltp;	/* earth and sun tilts -- +S */
MoonData md[NM];        /* fill in md[0].ra/dec/mag/x/y/z for jupiter,
                         * md[1..NM-1].x/y/z/mag for each moon
			 */
{
     /* ECD: code does not use [0].
      * ECD and why 11 here? seems like 9 would do
      */
     double SMA[11], U[11], U0[11], PD[11];
     double X[NM], Y[NM], Z[NM], MAG[NM];

     double P,TP,TE,EP,EE,RE0,RP0,RS;
     double JDE,LPE,LPP,LEE,LEP;
     double NN,ME,MP,VE,VP;
     double LE,LP,RE,RP,DT,II,F,F1;
     double RA,DECL;
     double TVA,PVA,TVC,PVC,DOT1,INC,TVB,PVB,DOT2,INCI;
     double TRIP,GAM,TEMPX,TEMPY,TEMPZ;
     int I;

     /*  ******************************************************************** */
     /*  *                                                                  * */
     /*  *                        Constants                                 * */
     /*  *                                                                  * */
     /*  ******************************************************************** */
     P = PI / 180;
     /*  Orbital Rate of Saturn in Radians per Days */
     TP = 2 * PI / (29.45771 * 365.2422);
     /*  Orbital Rate of Earth in Radians per Day */
     TE = 2 * PI / (1.00004 * 365.2422);
     /*  Eccentricity of Saturn's Orbit */
     EP = .0556155;
     /*  Eccentricity of Earth's Orbit */
     EE = .016718;
     /*  Semimajor axis of Earth's and Saturn's orbit in Astronomical Units */
     RE0 = 1; RP0 = 9.554747;
     /*  Visual Magnitude of the Satellites */
     MAG[1] = 13; MAG[2] = 11.8; MAG[3] = 10.3; MAG[4] = 10.2;
     MAG[5] = 9.8; MAG[6] = 8.4; MAG[7] = 14.3; MAG[8] = 11.2;
     /*  Semimajor Axis of the Satellites' Orbit in Kilometers */
     SMA[1] = 185600; SMA[2] = 238100; SMA[3] = 294700; SMA[4] = 377500;
     SMA[5] = 527200; SMA[6] = 1221600; SMA[7] = 1483000; SMA[8] = 3560100;
     /*  Eccentricity of Satellites' Orbit [Program uses 0] */
     /*  Synodic Orbital Period of Moons in Days */
     PD[1] = .9425049;
     PD[2] = 1.3703731;
     PD[3] = 1.8880926;
     PD[4] = 2.7375218;
     PD[5] = 4.5191631;
     PD[6] = 15.9669028;
     PD[7] = 21.3174647;
     PD[8] = 79.9190206;	/* personal mail 1/14/95 */
     RS = 60330; /*  Radius of planet in kilometers */
    
     /*  ******************************************************************** */
     /*  *                                                                  * */
     /*  *                      Epoch Information                           * */
     /*  *                                                                  * */
     /*  ******************************************************************** */
     JDE = 2444238.5; /*  Epoch Jan 0.0 1980 = December 31,1979 0:0:0 UT */
     LPE = 165.322242 * P; /*  Longitude of Saturn at Epoch */
     LPP = 92.6653974 * P; /*  Longitude of Saturn`s Perihelion */
     LEE = 98.83354 * P; /*  Longitude of Earth at Epoch */
     LEP = 102.596403 * P; /*  Longitude of Earth's Perihelion */
     /*  U0[I] = Angle from inferior geocentric conjuction */
     /*          measured westward along the orbit at epoch */
     U0[1] = 18.2919 * P;
     U0[2] = 174.2135 * P;
     U0[3] = 172.8546 * P;
     U0[4] = 76.8438 * P;
     U0[5] = 37.2555 * P;
     U0[6] = 57.7005 * P;
     U0[7] = 266.6977 * P;
     U0[8] = 195.3513 * P;	/* from personal mail 1/14/1995 */
    
     /*  ******************************************************************** */
     /*  *                                                                  * */
     /*  *                    Orbit Calculations                            * */
     /*  *                                                                  * */
     /*  ******************************************************************** */
     /*  ****************** FIND MOON ORBITAL ANGLES ************************ */
     NN = JD - JDE; /*  NN = Number of days since epoch */
     ME = ((TE * NN) + LEE - LEP); /*  Mean Anomoly of Earth */
     MP = ((TP * NN) + LPE - LPP); /*  Mean Anomoly of Saturn */
     VE = ME; VP = MP; /*  True Anomolies - Solve Kepler's Equation */
     FOR (I = 1; I <= 3; I++) {
	 VE = VE - (VE - (EE * SIN(VE)) - ME) / (1 - (EE * COS(VE)));
	 VP = VP - (VP - (EP * SIN(VP)) - MP) / (1 - (EP * COS(VP)));
     }
     VE = 2 * ATN(SQR((1 + EE) / (1 - EE)) * TAN(VE / 2));
     IF (VE < 0) VE = (2 * PI) + VE;
     VP = 2 * ATN(SQR((1 + EP) / (1 - EP)) * TAN(VP / 2));
     IF (VP < 0) VP = (2 * PI) + VP;
     /*   Heliocentric Longitudes of Earth and Saturn */
     LE = VE + LEP; IF (LE > (2 * PI)) LE = LE - (2 * PI);
     LP = VP + LPP; IF (LP > (2 * PI)) LP = LP - (2 * PI);
     /*   Distances of Earth and Saturn from the Sun in AU's */
     RE = RE0 * (1 - EE * EE) / (1 + EE * COS(VE));
     RP = RP0 * (1 - EP * EP) / (1 + EP * COS(VP));
     /*   DT = Distance from Saturn to Earth in AU's - Law of Cosines */
     DT = SQR((RE * RE) + (RP * RP) - (2 * RE * RP * COS(LE - LP)));
     /*   II = Angle between Earth and Sun as seen from Saturn */
     II = RE * SIN(LE - LP) / DT;
     II = ATN(II / SQR(1 - II * II)); /*   ArcSIN and Law of Sines */
     /*    F = NN - (Light Time to Earth in days) */
     F = NN - (DT / 173.83);
     F1 = II + MP - VP;
     /*  U(I) = Angle from inferior geocentric conjuction measured westward */
     FOR (I = 1; I < NM; I++) {
	U[I] = U0[I] + (F * 2 * PI / PD[I]) + F1;
	U[I] = ((U[I] / (2 * PI)) - (int)(U[I] / (2 * PI))) * 2 * PI;

     }

     /*  ******************** FIND SATURN'S COORDINATES ********************* */
     /* ECD: and mag and x/y/z */
     {
	Obj *op = db_basic(SATURN);

	md[0].ra = RA = op->s_ra;
	md[0].dec = DECL = op->s_dec;
	md[0].mag = op->s_mag/MAGSCALE;

	md[0].x = 0;
	md[0].y = 0;
	md[0].z = 0;

	*sizep = degrad((double)op->s_size/3600.0);
     }

     /*  **************** FIND INCLINATION OF RINGS ************************* */
     /*  Use dot product of Earth-Saturn vector and Saturn's rotation axis */
     TVA = (90 - 83.51) * P; /*  Theta coordinate of Saturn's axis */
     PVA = 40.27 * P; /*  Phi coordinate of Saturn's axis */
     TVC = (PI / 2) - DECL;
     PVC = RA;
     DOT1 = SIN(TVA) * COS(PVA) * SIN(TVC) * COS(PVC);
     DOT1 = DOT1 + SIN(TVA) * SIN(PVA) * SIN(TVC) * SIN(PVC);
     DOT1 = DOT1 + COS(TVA) * COS(TVC);
     INC = ATN(SQR(1 - DOT1 * DOT1) / DOT1); /*    ArcCOS */
     IF (INC > 0) INC = (PI / 2) - INC; ELSE INC = -(PI / 2) - INC;

     /*  ************* FIND INCLINATION OF IAPETUS' ORBIT ******************* */
     /*  Use dot product of Earth-Saturn vector and Iapetus' orbit axis */
     /*  Vector B */
     TVB = (90 - 75.6) * P; /*  Theta coordinate of Iapetus' orbit axis (estimate) */
     PVB = 21.34 * 2 * PI / 24; /*  Phi coordinate of Iapetus' orbit axis (estimate) */
     DOT2 = SIN(TVB) * COS(PVB) * SIN(TVC) * COS(PVC);
     DOT2 = DOT2 + SIN(TVB) * SIN(PVB) * SIN(TVC) * SIN(PVC);
     DOT2 = DOT2 + COS(TVB) * COS(TVC);
     INCI = ATN(SQR(1 - DOT2 * DOT2) / DOT2); /*    ArcCOS */
     IF (INCI > 0) INCI = (PI / 2) - INCI; ELSE INCI = -(PI / 2) - INCI;

     /*  ************* FIND ROTATION ANGLE OF IAPETUS' ORBIT **************** */
     /*  Use inclination of Iapetus' orbit with respect to ring plane */
     /*  Triple Product */
     TRIP = SIN(TVC) * COS(PVC) * SIN(TVA) * SIN(PVA) * COS(TVB);
     TRIP = TRIP - SIN(TVC) * COS(PVC) * SIN(TVB) * SIN(PVB) * COS(TVA);
     TRIP = TRIP + SIN(TVC) * SIN(PVC) * SIN(TVB) * COS(PVB) * COS(TVA);
     TRIP = TRIP - SIN(TVC) * SIN(PVC) * SIN(TVA) * COS(PVA) * COS(TVB);
     TRIP = TRIP + COS(TVC) * SIN(TVA) * COS(PVA) * SIN(TVB) * SIN(PVB);
     TRIP = TRIP - COS(TVC) * SIN(TVB) * COS(PVB) * SIN(TVA) * SIN(PVA);
     GAM = -1 * ATN(TRIP / SQR(1 - TRIP * TRIP)); /*  ArcSIN */
    
     /*  ******************************************************************** */
     /*  *                                                                  * */
     /*  *                     Compute Moon Positions                       * */
     /*  *                                                                  * */
     /*  ******************************************************************** */
     FOR (I = 1; I < NM - 1; I++) {
	 X[I] = -1 * SMA[I] * SIN(U[I]) / RS;
	 Z[I] = -1 * SMA[I] * COS(U[I]) / RS;	/* ECD */
	 Y[I] = SMA[I] * COS(U[I]) * SIN(INC) / RS;
     }
     /*  ************************* Iapetus' Orbit *************************** */
     TEMPX = -1 * SMA[8] * SIN(U[8]) / RS;
     TEMPZ = -1 * SMA[8] * COS(U[8]) / RS;
     TEMPY = SMA[8] * COS(U[8]) * SIN(INCI) / RS;
     X[8] = TEMPX * COS(GAM) + TEMPY * SIN(GAM); /*       Rotation */
     Z[8] = TEMPZ * COS(GAM) + TEMPY * SIN(GAM);
     Y[8] = -1 * TEMPX * SIN(GAM) + TEMPY * COS(GAM);
    
#ifdef SHOWALL
     /*  ******************************************************************** */
     /*  *                                                                  * */
     /*  *                          Show Results                            * */
     /*  *                                                                  * */
     /*  ******************************************************************** */
     printf ("                           Julian Date : %g\n", JD);
     printf ("             Right Ascension of Saturn : %g Hours\n", RA * 24 / (2 * PI));
     printf ("                 Declination of Saturn : %g\n", DECL / P);
     printf ("   Ring Inclination as seen from Earth : %g\n", -1 * INC / P);
     printf ("      Heliocentric Longitude of Saturn : %g\n", LP / P);
     printf ("       Heliocentric Longitude of Earth : %g\n", LE / P);
     printf ("                Sun-Saturn-Earth Angle : %g\n", II / P);
     printf ("     Distance between Saturn and Earth : %g AU = %g million miles\n", DT, (DT * 93));
     printf ("       Light time from Saturn to Earth : %g minutes\n", DT * 8.28);
     TEMP = 2 * ATN(TAN(165.6 * P / (2 * 3600)) / DT) * 3600 / P;
     printf ("                Angular Size of Saturn : %g arcsec\n", TEMP);
     printf ("  Major Angular Size of Saturn's Rings : %g arcsec\n", RS4 * TEMP / RS);
     printf ("  Minor Angular Size of Saturn's Rings : %g arcsec\n", ABS(RS4 * TEMP * SIN(INC) / RS));
#endif

     /* ECD: get tilts from sky and tel code */
     ring_tilt (JD, etiltp, stiltp);

     /* copy into md[1..NM-1] with our sign conventions */
     for (I = 1; I < NM; I++) {
	md[I].x =  X[I];	/* we want +E */
	md[I].y = -Y[I];	/* we want +S */
	md[I].z = -Z[I];	/* we want +front */
	md[I].mag = MAG[I];	/* */
     }
}

/*
 *  RINGS OF SATURN by Olson, et al, BASIC Code from Sky & Telescope, May 1995.
 *  As converted from BASIC to C by pmartz@dsd.es.com (Paul Martz)
 *  Adapted to xephem by Elwood Charles Downey.
 */

static void
ring_tilt (JD, etiltp, stiltp)
double JD;			/* Julian date */
double *etiltp, *stiltp;	/* tilt from earth and sun, rads southward */
{
	double t, i, om;
	double x, y, z;
	double la, be;
	double s, b, sp, bp;
	double sr, sb, sl, er, eb, el;

	t = (JD-2451545.)/365250.;
	i = degrad(28.04922-.13*t+.0004*t*t);
	om = degrad(169.53+13.826*t+.04*t*t);

	{
	    Obj *op;

	    op = db_basic(SATURN);
	    sb = op->s_hlat;
	    sl = op->s_hlong;
	    sr = op->s_sdist;

	    op = db_basic(SUN);	/* for earth */
	    eb = 0.0;
	    el = op->s_hlong;
	    er = op->s_edist;
	}

	x = sr*cos(sb)*cos(sl)-er*cos(el);
	y = sr*cos(sb)*sin(sl)-er*sin(el);
	z = sr*sin(sb)-er*sin(eb);

	la = atan(y/x);
	if (x<0) la+=PI;
	be = atan(z/sqrt(x*x+y*y));

	s = sin(i)*cos(be)*sin(la-om)-cos(i)*sin(be);
	b = atan(s/sqrt(1.-s*s));
	sp = sin(i)*cos(sb)*sin(sl-om)-cos(i)*sin(sb);
	bp = atan(sp/sqrt(1.-sp*sp));

	*etiltp = b;
	*stiltp = bp;
}
