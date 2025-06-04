/* code to manage the stuff on the "jupiter" menu.
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
extern void jgrs_draw P_((Drawable dr, int rad, int jx, int jy, int fliptb,
    int fliplr, double cmlII));
extern void mm_movie P_((double stepsz));
extern void obj_pickgc P_((Obj *op, Widget w, GC *gcp));
extern void register_selection P_((char *name));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void jm_update P_((Now *np, int how_much));
extern void sv_draw_obj P_((Display *dsp, Drawable win, GC gc, Obj *op, int x,
    int y, int diam, int dotsonly));
extern void timestamp P_((Now *np, Widget w));
extern void watch_cursor P_((int want));
extern void xe_msg P_((char *buf, int app_modal));

#define	MINR	10	/* min distance for picking, pixels */

/* These values are from the Explanatory Supplement.
 * Precession degrades them gradually over time.
 */
#define	POLE_RA		degrad(268.04)	/* RA of Jupiter's north pole */
#define	POLE_DEC	degrad(64.49)	/* Dec of Jupiter's north pole */

/* local record of what is now on screen for easy id from mouse picking.
 */
typedef struct {
    Obj o;		/* copy of object info.
    			 * copy from DB or, if jupiter moon, we fill in just
			 * o_name and s_mag/ra/dec
			 */
    int x, y;		/* screen coord */
} ScreenObj;


static void jm_create_form_w P_((void));
static void jm_create_jsform_w P_((void));
static void jm_set_buttons P_((int whether));
static void jm_set_a_button P_((Widget pbw, int whether));
static void jm_sstats_cb P_((Widget w, XtPointer client, XtPointer call));
static void jm_option_cb P_((Widget w, XtPointer client, XtPointer call));
static void jm_scale_cb P_((Widget w, XtPointer client, XtPointer call));
static void jm_activate_cb P_((Widget w, XtPointer client, XtPointer call));
static void jm_close_cb P_((Widget w, XtPointer client, XtPointer call));
static void jm_movie_cb P_((Widget w, XtPointer client, XtPointer call));
static void jm_da_exp_cb P_((Widget w, XtPointer client, XtPointer call));
static void jm_da_input_cb P_((Widget w, XtPointer client, XtPointer call));
static void jm_create_popup P_((void));
static void jm_fill_popup P_((ScreenObj *sop));
static void jm_help_cb P_((Widget w, XtPointer client, XtPointer call));
static void jm_helpon_cb P_((Widget w, XtPointer client, XtPointer call));
static void jm_locfifo_cb P_((Widget w, XtPointer client, XtPointer call));
static void j_calibline P_((Display *dsp, Drawable win, GC gc, int xc, int yc,
    char *tag, int tw, int th, int l));

static void add_screenobj P_((Obj *op, int x, int y));
static void reset_screenobj P_((void));
static ScreenObj *close_screenobj P_((int x, int y));

static void sky_background P_((Drawable win, unsigned w, unsigned h, int fmag,
    double ra0, double dec0, double salpha, double calpha, double scale, double
    rad, int fliptb, int fliplr));

static Widget jupform_w;	/* main form */
static Widget jsform_w;		/* statistics form */
static Widget jda_w;		/* drawing area */
static Widget cmlI_w, cmlII_w;	/* labels for displaying GRS coords */
static Widget scale_w;		/* size scale */
static Widget limmag_w;		/* limiting magnitude scale */
static Widget dt_w;		/* main date/time stamp widget */
static Widget sdt_w;		/* statistics date/time stamp widget */
static Widget skybkg_w;		/* toggle for controlling sky background */
static int jm_selecting;	/* set while our fields are being selected */
static int brmoons;		/* whether we want to brightten the moons */
static int tags;		/* whether we want tags on the drawing */
static int flip_tb;		/* whether we want to flip top/bottom */
static int flip_lr;		/* whether we want to flip left/right */
static int skybkg;		/* whether we want sky background */
static int jswasman;		/* whether statistics form was managed */

static Widget jmpu_w;		/* main popup */
static Widget jmpu_name_w;	/* popup name label */
static Widget jmpu_ra_w;	/* popup RA label */
static Widget jmpu_dec_w;	/* popup Dec label */
static Widget jmpu_mag_w;	/* popup Mag label */

#define	MAXSCALE	10.0	/* max scale mag factor */
#define	MOVIE_STEPSZ	0.25	/* movie step size, hours */

/* arrays that use NM use [0] for the planet itself, and [1..NM-1] for moons.
 */
#define	NM	5		/* number of moons + 1 for planet */
static struct MoonNames {
    char *full;
    char *tag;
} mnames[NM] = {
    {"Jupiter", NULL},
    {"Io", "I"},
    {"Europa", "II"},
    {"Ganymede", "III"},
    {"Callisto", "IV"}
};

enum {CX, CY, CZ, CRA, CDEC, CMAG, CNum};	/* j_w column index */
static Widget	j_w[NM][CNum];			/* the data display widgets */

/* malloced array for each item on screen. used for picking. */
static ScreenObj *screenobj;
static int nscreenobj;


/* called when the jupiter menu is activated via the main menu pulldown.
 * if never called before, create and manage all the widgets as a child of a
 * form. otherwise, just toggle whether the form is managed.
 */
void
jm_manage ()
{
	if (!jupform_w) {
	    jm_create_form_w();
	    jm_create_jsform_w();
	}
	
	if (XtIsManaged(jupform_w)) {
	    XtUnmanageChild (jupform_w);
	    if ((jswasman = XtIsManaged(jsform_w)))
		XtUnmanageChild (jsform_w);
	} else {
	    XtManageChild (jupform_w);
	    if (jswasman) {
		XtManageChild (jsform_w);
		jm_set_buttons(jm_selecting);
	    }
	    /* rely on expose to do the first draw */
	}
}

/* called when the database has changed.
 * if we are drawing background, we'd best redraw everything.
 */
/* ARGSUSED */
void
jm_newdb (appended)
int appended;
{
	if (skybkg)
	    jm_update (mm_get_now(), 1);
}

int
jm_ison()
{
	return (jupform_w && XtIsManaged(jupform_w));
}

/* called by other menus as they want to hear from our buttons or not.
 * the "on"s and "off"s stack - only really redo the buttons if it's the
 * first on or the last off.
 */
void
jm_selection_mode (whether)
int whether;	/* whether setting up for plotting or for not plotting */
{
	jm_selecting += whether ? 1 : -1;

	if (jm_ison())
	    if ((whether && jm_selecting == 1)     /* first one to want on */
		|| (!whether && jm_selecting == 0) /* last one to want off */)
		jm_set_buttons (whether);
}

/* called to put up or remove the watch cursor.  */
void
jm_cursor (c)
Cursor c;
{
	Window win;

	if (jupform_w && (win = XtWindow(jupform_w))) {
	    Display *dsp = XtDisplay(jupform_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}

	if (jsform_w && (win = XtWindow(jsform_w))) {
	    Display *dsp = XtDisplay(jsform_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}

/* create the main form dialog */
static void
jm_create_form_w()
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
	    {"Intro...",	"Jupiter - intro"},
	    {"on Mouse...",	"Jupiter - mouse"},
	    {"on Control...",	"Jupiter - control"},
	    {"on View...",	"Jupiter - view"},
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
	jupform_w = XmCreateFormDialog (toplevel_w, "Jupiter", args, n);
	XtAddCallback (jupform_w, XmNhelpCallback, jm_help_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Jupiter view"); n++;
	XtSetValues (XtParent(jupform_w), args, n);

	/* create the menu bar across the top */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	mb_w = XmCreateMenuBar (jupform_w, "MB", args, n);
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
	    XtAddCallback (w, XmNactivateCallback, jm_locfifo_cb, 0);
	    XtManageChild (w);

	    /* add the "movie" push button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Movie", args, n);
	    set_xmstring (w, XmNlabelString, "Movie Demo");
	    XtAddCallback (w, XmNactivateCallback, jm_movie_cb, 0);
	    XtManageChild (w);

	    /* add the "close" push button beneath a separator */

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Close", args, n);
	    XtAddCallback (w, XmNactivateCallback, jm_close_cb, 0);
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
		XtAddCallback (w, XmNvalueChangedCallback, jm_option_cb, 
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
	    XtAddCallback (w, XmNactivateCallback, jm_sstats_cb, NULL);
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
		XtAddCallback (w, XmNactivateCallback, jm_helpon_cb,
							(XtPointer)(hp->key));
		XtManageChild (w);
		XmStringFree(str);
	    }

	/* make the date/time stamp label */

	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	dt_w = XmCreateLabel (jupform_w, "DateStamp", args, n);
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
	scale_w = XmCreateScale (jupform_w, "Scale", args, n);
	XtAddCallback (scale_w, XmNdragCallback, jm_scale_cb, 0);
	XtAddCallback (scale_w, XmNvalueChangedCallback, jm_scale_cb, 0);
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
	limmag_w = XmCreateScale (jupform_w, "LimMag", args, n);
	XtAddCallback (limmag_w, XmNdragCallback, jm_scale_cb, 0);
	XtAddCallback (limmag_w, XmNvalueChangedCallback, jm_scale_cb, 0);
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
	frame_w = XmCreateFrame (jupform_w, "JupFrame", args, n);
	XtManageChild (frame_w);

	    /* make a drawing area for drawing the little map */

	    n = 0;
	    jda_w = XmCreateDrawingArea (frame_w, "Map", args, n);
	    XtAddCallback (jda_w, XmNexposeCallback, jm_da_exp_cb, 0);
	    XtAddCallback (jda_w, XmNinputCallback, jm_da_input_cb, 0);
	    XtManageChild (jda_w);
}

/* make the statistics form dialog */
static void
jm_create_jsform_w()
{
	typedef struct {
	    int col;		/* C* column code */
	    char *rcname;	/* name of rowcolumn */
	    char *collabel;	/* column label */
	    char *suffix;	/* suffix for plot/list/search name */
	} MoonColumn;
	static MoonColumn mc[] = {
	    {CX,   "JupX",   "X (+E)",     "X"},
	    {CY,   "JupY",   "Y (+S)",     "Y"},
	    {CZ,   "JupZ",   "Z (+front)", "Z"},
	    {CRA,  "JupRA",  "RA",         "RA"},
	    {CDEC, "JupDec", "Dec",        "Dec"},
	    {CMAG, "JupMag", "Mag",        "Mag"},
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
	jsform_w = XmCreateFormDialog (toplevel_w, "JupiterStats", args, n);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Jupiter info"); n++;
	XtSetValues (XtParent(jsform_w), args, n);

	/* make GRS title label */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	title_w = XmCreateLabel (jsform_w, "JupTL", args, n);
	XtManageChild (title_w);
	set_xmstring (title_w, XmNlabelString,
					"Central Meridian Longitudes (degs):");

	/* make the Sys I r/c */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, title_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
	XtSetArg (args[n], XmNnumColumns, 1); n++;
	rc_w = XmCreateRowColumn (jsform_w, "IRC", args, n);
	XtManageChild (rc_w);

	    n = 0;
	    w = XmCreateLabel (rc_w, "JupCMLIMsg", args, n);
	    set_xmstring (w, XmNlabelString, "Sys I:");
	    XtManageChild (w);

	    n = 0;
	    XtSetArg (args[n], XmNuserData, "Jupiter.CMLI"); n++;
	    cmlI_w = XmCreatePushButton (rc_w, "JupCMLI", args, n);
	    XtAddCallback (cmlI_w, XmNactivateCallback, jm_activate_cb, 0);
	    XtManageChild (cmlI_w);

	/* make the Sys II r/c */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, title_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
	XtSetArg (args[n], XmNnumColumns, 1); n++;
	rc_w = XmCreateRowColumn (jsform_w, "IIRC", args, n);
	XtManageChild (rc_w);

	    n = 0;
	    w = XmCreateLabel (rc_w, "JupCMLIIMsg", args, n);
	    set_xmstring (w, XmNlabelString, "Sys II:");
	    XtManageChild (w);

	    n = 0;
	    XtSetArg (args[n], XmNuserData, "Jupiter.CMLII"); n++;
	    cmlII_w = XmCreatePushButton (rc_w, "JupCMLII", args, n);
	    XtAddCallback (cmlII_w, XmNactivateCallback, jm_activate_cb, 0);
	    XtManageChild (cmlII_w);

	/* make table title label */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNtopOffset, 10); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	title_w = XmCreateLabel (jsform_w, "JupML", args, n);
	XtManageChild (title_w);
	set_xmstring (title_w, XmNlabelString,"Moon Positions (Jupiter radii)");

	/* make the moon table, one column at a time */

	/* moon designator column */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, title_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
	XtSetArg (args[n], XmNisAligned, True); n++;
	rc_w = XmCreateRowColumn (jsform_w, "JupDes", args, n);
	XtManageChild (rc_w);

	    n = 0;
	    w = XmCreateLabel (rc_w, "TL", args, n);
	    XtManageChild (w);
	    set_xmstring (w, XmNlabelString, "Tag");

	    for (i = 0; i < NM; i++) {
		char *tag = mnames[i].tag;

		n = 0;
		w = XmCreatePushButton (rc_w, "JupTag", args, n); /*PB for sz */
		set_xmstring (w, XmNlabelString, tag ? tag : " ");
		jm_set_a_button (w, False);
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
	rc_w = XmCreateRowColumn (jsform_w, "JupName", args, n);
	XtManageChild (rc_w);

	    n = 0;
	    w = XmCreateLabel (rc_w, "NL", args, n);
	    XtManageChild (w);
	    set_xmstring (w, XmNlabelString, "Name");

	    for (i = 0; i < NM; i++) {
		n = 0;
		w = XmCreatePushButton (rc_w, "JupName", args, n); /*PB for sz*/
		set_xmstring (w, XmNlabelString, mnames[i].full);
		jm_set_a_button (w, False);
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
	    rc_w = XmCreateRowColumn (jsform_w, mp->rcname, args, n);
	    XtManageChild (rc_w);

	    n = 0;
	    w = XmCreateLabel (rc_w, "JupLab", args, n);
	    XtManageChild (w);
	    set_xmstring (w, XmNlabelString, mp->collabel);

	    for (j = 0; j < NM; j++) {
		char *sel;

		sel = XtMalloc (strlen(mnames[j].full) + strlen(mp->suffix)+2);
		(void) sprintf (sel, "%s.%s", mnames[j].full, mp->suffix);

		n = 0;
		XtSetArg (args[n], XmNuserData, sel); n++;
		w = XmCreatePushButton(rc_w, "JupPB", args, n);
		XtAddCallback(w, XmNactivateCallback, jm_activate_cb, 0);
		XtManageChild (w);
		j_w[j][mp->col] = w;
	    }
	}

	/* make a separator */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (jsform_w, "Sep1", args, n);
	XtManageChild (sep_w);

	/* make a date/time stamp */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sdt_w = XmCreateLabel (jsform_w, "JDateStamp", args, n);
	XtManageChild (sdt_w);

	/* make a separator */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sdt_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (jsform_w, "Sep2", args, n);
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
	w = XmCreatePushButton (jsform_w, "Close", args, n);
	XtAddCallback (w, XmNactivateCallback, jm_sstats_cb, 0);
	XtManageChild (w);
}

/* go through all the buttons pickable for plotting and set whether they
 * should appear to look like buttons or just flat labels.
 */
static void
jm_set_buttons (whether)
int whether;	/* whether setting up for plotting or for not plotting */
{
	int i, j;

	for (i = 0; i < NM; i++) {
	    for (j = 0; j < CNum; j++)
		jm_set_a_button (j_w[i][j], whether);
	}
	jm_set_a_button (cmlI_w, whether);
	jm_set_a_button (cmlII_w, whether);
}

/* set whether the given button looks like a label.
 */
static void
jm_set_a_button(pbw, whether)
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
	    tmpw = XmCreatePushButton (jupform_w, "tmp", args, n);

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
jm_sstats_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if ((jswasman = XtIsManaged(jsform_w)))
	    XtUnmanageChild (jsform_w);
	else {
	    XtManageChild (jsform_w);
	    jm_set_buttons(jm_selecting);
	}
}

/* callback from any of the option buttons.
 * client points to global flag to set; some don't have any.
 * in any case then just redraw everything.
 */
/* ARGSUSED */
static void
jm_option_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (client)
	    *((int *)client) = XmToggleButtonGetState(w);

	jm_update (mm_get_now(), 1);
}

/* callback from the scales changing.
 * just redraw everything.
 */
/* ARGSUSED */
static void
jm_scale_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	jm_update (mm_get_now(), 1);
}

/* callback from any of the data menu buttons being activated.
 */
/* ARGSUSED */
static void
jm_activate_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (jm_selecting) {
	    char *name;
	    get_something (w, XmNuserData, (XtArgVal)&name);
	    register_selection (name);
	}
}

/* callback from the main Close button */
/* ARGSUSED */
static void
jm_close_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (jupform_w);
	if ((jswasman = XtIsManaged(jsform_w)))
	    XtUnmanageChild (jsform_w);

	/* stop movie that might be running */
	mm_movie (0.0);
}

/* callback from the Movie button
 */
/* ARGSUSED */
static void
jm_movie_cb (w, client, call)
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
jm_da_exp_cb (w, client, call)
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
	    printf ("Unexpected jupform_w event. type=%d\n", c->reason);
	    exit(1);
	}

	jm_update (mm_get_now(), 1);
}

/* callback from mouse or keyboard input over drawing area */
/* ARGSUSED */
static void
jm_da_input_cb (w, client, call)
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

	/* put info in popup, jmpu_w, creating it first is necessary  */
	if (!jmpu_w)
	    jm_create_popup();
	jm_fill_popup (sop);

	/* put it on screen */
	XmMenuPosition (jmpu_w, (XButtonPressedEvent *)ev);
	XtManageChild (jmpu_w);
}

/* create the (unmanaged for now) popup menu in jmpu_w. */
static void
jm_create_popup()
{
	static Widget *puw[] = {
	    &jmpu_name_w,
	    &jmpu_ra_w,
	    &jmpu_dec_w,
	    &jmpu_mag_w,
	};
	Widget w;
	Arg args[20];
	int n;
	int i;

	n = 0;
	XtSetArg (args[n], XmNisAligned, True); n++;
	XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
	jmpu_w = XmCreatePopupMenu (jda_w, "JupPU", args, n);

	/* stack everything up in labels */
	for (i = 0; i < XtNumber(puw); i++) {
	    n = 0;
	    w = XmCreateLabel (jmpu_w, "SPUL", args, n);
	    XtManageChild (w);
	    *puw[i] = w;
	}
}

/* put up a popup at ev with info about sop */
static void
jm_fill_popup (sop)
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
	set_xmstring (jmpu_name_w, XmNlabelString, buf2);

	fs_ra (buf, ra);
	(void) sprintf (buf2, "  RA: %s", buf);
	set_xmstring (jmpu_ra_w, XmNlabelString, buf2);

	fs_pangle (buf, dec);
	(void) sprintf (buf2, " Dec: %s", buf);
	set_xmstring (jmpu_dec_w, XmNlabelString, buf2);

	(void) sprintf (buf2, " Mag: %.3g", mag);
	set_xmstring (jmpu_mag_w, XmNlabelString, buf2);
}

/* callback from the Help all button
 */
/* ARGSUSED */
static void
jm_help_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static char *msg[] = {
"This is a simple schematic depiction of Jupiter and its moons.",
};

	hlp_dialog ("Jupiter", msg, XtNumber(msg));
}

/* callback from a specific Help button.
 * client is a string to use with hlp_dialog().
 */
/* ARGSUSED */
static void
jm_helpon_cb (w, client, call)
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
jm_locfifo_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	Now *np = mm_get_now();
	Obj *op = db_basic (JUPITER);

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
static void jupiter_data();
static void moonradec();
static void jm_draw_map ();

/* called to recompute and fill in values for the jupiter menu.
 * don't bother if it doesn't exist or is unmanaged now or no one is logging.
 */
void
jm_update (np, how_much)
Now *np;
int how_much;
{
	static char fmt[] = "%7.3f";
	MoonData md[NM];
	int wantstats;
	double cmlI, cmlII;
	double jupsize;
	double theta, phi;
	double tvc, pvc;
	double salpha, calpha;
	int i;

	/* see if we should bother */
	if (!jupform_w)
	    return;
	wantstats = XtIsManaged(jsform_w) || any_ison() || how_much;
	if (!XtIsManaged(jupform_w) && !wantstats)
	    return;

	watch_cursor (1);

        /* compute md[0].x/y/z/mag/ra/dec and md[1..NM-1].x/y/z/mag info */
        jupiter_data(mjd, &jupsize, &cmlI, &cmlII, md);

	/* compute sky transformation angle as triple vector product */
	tvc = PI/2.0 - md[0].dec;
	pvc = md[0].ra;
	theta = PI/2 - POLE_DEC;
	phi = POLE_RA;
	salpha = sin(tvc)*sin(theta)*(cos(pvc)*sin(phi) - sin(pvc)*cos(phi));
	calpha = sqrt (1.0 - salpha*salpha);

        /* find md[1..NM-1].ra/dec from md[0].ra/dec and md[1..NM-1].x/y */
	moonradec (jupsize, salpha, calpha, md);

	if (wantstats) {
	    for (i = 0; i < NM; i++) {
		f_double (j_w[i][CX], fmt, md[i].x);
		f_double (j_w[i][CY], fmt, md[i].y);
		f_double (j_w[i][CZ], fmt, md[i].z);
		f_double (j_w[i][CMAG], "%4.1f", md[i].mag);
		f_ra (j_w[i][CRA], md[i].ra);
		f_pangle (j_w[i][CDEC], md[i].dec);
	    }

	    f_double (cmlI_w, fmt, raddeg(cmlI));
	    f_double (cmlII_w, fmt, raddeg(cmlII));
	    timestamp (np, sdt_w);
	}

	if (XtIsManaged(jupform_w)) {
	    jm_draw_map (jda_w, jupsize, salpha, calpha, cmlII, md);
	    timestamp (np, dt_w);
	}

	watch_cursor (0);
}

/* given the loc of the moons, draw a nifty little picture.
 * also save resulting screen locs of everything in the screenobj array.
 * scale of the locations is in terms of jupiter radii == 1.
 * unflipped view is S up, E right.
 * planet itself is in md[0], moons in md[1..NM-1].
 *     +md[].x: East, jup radii
 *     +md[].y: South, jup radii
 *     +md[].z: in front, jup radii
 */
static void
jm_draw_map (w, jupsize, salpha, calpha, cmlII, md)
Widget w;
double jupsize;
double salpha, calpha;
double cmlII;
MoonData md[NM];
{
	static GC j_fgc, j_bgc, j_xgc;
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
	unsigned int nx, ny, bw, d;
	int ns = flip_tb ? -1 : 1;
	int ew = flip_lr ? -1 : 1;
	int i;

#define	NORM	27.0	/* max Callisto orbit radius; used to normalize */
#define	MAPSCALE(r)	((r)*((int)nx)/NORM/2*scale)
#define	XCORD(x)	((int)(((int)nx)/2.0 + ew*MAPSCALE(x) + 0.5))
#define	YCORD(y)	((int)(((int)ny)/2.0 - ns*MAPSCALE(y) + 0.5))

	if (!j_fgc) {
	    XGCValues gcv;
	    unsigned int gcm;
	    Pixel fg, bg;

	    gcm = GCForeground;
	    get_something (w, XmNforeground, (XtArgVal)&fg);
	    gcv.foreground = fg;
	    j_fgc = XCreateGC (dsp, win, gcm, &gcv);
	    s_fs = XQueryFont (dsp, XGContextFromGC (j_fgc));
	    cw = s_fs->max_bounds.width;
	    ch = s_fs->max_bounds.ascent + s_fs->max_bounds.descent;

	    gcm = GCForeground;
	    get_something (w, XmNbackground, (XtArgVal)&bg);
	    gcv.foreground = bg;
	    j_bgc = XCreateGC (dsp, win, gcm, &gcv);

	    gcm = GCForeground | GCFunction;
	    gcv.foreground = fg ^ bg;
	    gcv.function = GXxor;
	    j_xgc = XCreateGC (dsp, win, gcm, &gcv);
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

	XFillRectangle (dsp, pm, j_bgc, 0, 0, nx, ny);

	/* prepare for a new list of things on the screen */
	reset_screenobj();

	/* draw background objects, if desired */
	if (skybkg)
	    sky_background(pm, nx, ny, fmag, md[0].ra, md[0].dec, salpha,calpha,
			jupsize/MAPSCALE(2), jupsize*NORM, flip_tb, flip_lr);

	/* draw labels */
	c = flip_lr ? 'W' : 'E';
	XDrawString(dsp, pm, j_fgc, nx-cw-1, ny/2-2, &c, 1);
	c = flip_tb ? 'N' : 'S';
	XDrawString(dsp, pm, j_fgc, (nx-cw)/2-1, s_fs->ascent, &c, 1);

	/* draw Jupiter in center with unit radius */
	XFillArc (dsp, pm, j_fgc, nx/2-(int)MAPSCALE(1), ny/2-(int)MAPSCALE(1),
			    (int)MAPSCALE(2), (int)MAPSCALE(2), 0, 360*64);
	add_screenobj (db_basic(JUPITER), nx/2, ny/2);

	/* draw the GRS */
	jgrs_draw (pm, (int)MAPSCALE(1), nx/2, ny/2, flip_tb, flip_lr, cmlII);

	/* draw 1' calibration line */
	if (tags)
	    j_calibline (dsp, pm, j_fgc, nx/2, 5*ny/6, "1'", cw*2, ch+4,
				(int)MAPSCALE(degrad(1.0/60.0)/(jupsize/2)));

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
		continue;	/* behind jupiter */
	    diam = magdiam (fmag, jupsize/MAPSCALE(2), mag, 0.0);
	    if (brmoons)
		diam += 3;
	    if (diam <= 0)
		continue;	/* too faint */

	    x = XCORD(mx);
	    y = YCORD(my);
	    if (diam == 1)
		XDrawPoint(dsp, pm, j_xgc, x, y);
	    else
		XFillArc (dsp, pm, j_xgc, x-diam/2, y-diam/2, diam, diam,
								    0, 360*64);

	    /* add object to list of screen objects drawn */
	    (void) strcpy (o.o_name, mnames[i].full);
	    o.s_mag = mag*MAGSCALE;
	    o.s_ra = md[i].ra;
	    o.s_dec = md[i].dec;
	    add_screenobj (&o, x, y);

	    if (tags && mnames[i].tag)
		XDrawString(dsp, pm, j_xgc, x-cw/2, y+2*ch,
					mnames[i].tag, strlen(mnames[i].tag));
	}

	XCopyArea (dsp, pm, win, j_fgc, 0, 0, nx, ny, 0, 0);
}

/* draw a calibration line l pixels long centered at [xc,yc] marked with tag
 * with is in a bounding box tw x th.
 */
static void
j_calibline (dsp, win, gc, xc, yc, tag, tw, th, l)
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

	    if (is_planet (op, JUPITER)) {
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

/* given jupiter loc in md[0].ra/dec and size, and location of each moon in 
 * md[1..NM-1].x/y in jup radii, find ra/dec of each moon in md[1..NM-1].ra/dec.
 */
static void
moonradec (jupsize, salpha, calpha, md)
double jupsize;		/* jup diameter, rads */
double salpha, calpha;	/* transormation parameters */
MoonData md[NM];	/* fill in RA and Dec */
{
	double juprad = jupsize/2;
	double jupra = md[0].ra;
	double jupdec = md[0].dec;
	int i;

	for (i = 1; i < NM; i++) {
	    double dra  = juprad * ( md[i].x*calpha + md[i].y*salpha);
	    double ddec = juprad * (-md[i].x*salpha + md[i].y*calpha);
	    md[i].ra  = jupra + dra;
	    md[i].dec = jupdec - ddec;
	}
}

#define	dsin(x)	sin(degrad(x))
#define	dcos(x)	cos(degrad(x))

/* given a modified julian date (ie, days since Jan .5 1900), d, return x, y, z
 *   location of each Galilean moon as a multiple of Jupiter's radius. on this
 *   scale, Callisto is never more than 26.5593. +x is easterly, +y is
 *   southerly, +z is towards earth. x and z are relative to the equator
 *   of Jupiter; y is further corrected for earth's position above or below
 *   this plane. also, return the system I and II central meridian longitude,
 *   in degress, relative to the true disk of jupiter and corrected for light
 *   travel time. also return jupiter ra/dec/mag in md[0] and diam, in rads.
 *   also return nominal magnitude of each moon.
 * from "Astronomical Formulae for Calculators", 2nd ed, by Jean Meeus,
 *   Willmann-Bell, Richmond, Va., U.S.A. (c) 1982, chapters 35 and 36.
 */
static void
jupiter_data(d, jupsize, cmlI, cmlII, md)
double d;
double *jupsize;
double *cmlI, *cmlII;	/* central meridian longitude, rads */
MoonData md[NM];	/* fill in md[0].ra/dec/mag/x/y/z for jupiter,
			 * md[1..NM-1].x/y/z/mag for each moon
			 */
{
	double A, B, Del, J, K, M, N, R, V;
	double cor_u1, cor_u2, cor_u3, cor_u4;
	double solc, tmp, G, H, psi, r, r1, r2, r3, r4;
	double u1, u2, u3, u4;
	double lam, Ds;
	double z1, z2, z3,  z4;
	double De, dsinDe;

	V = 134.63 + 0.00111587 * d;

	M = (358.47583 + 0.98560003*d);
	N = (225.32833 + 0.0830853*d) + 0.33 * dsin (V);

	J = 221.647 + 0.9025179*d - 0.33 * dsin(V);;

	A = 1.916*dsin(M) + 0.02*dsin(2*M);
	B = 5.552*dsin(N) + 0.167*dsin(2*N);
	K = (J+A-B);
	R = 1.00014 - 0.01672 * dcos(M) - 0.00014 * dcos(2*M);
	r = 5.20867 - 0.25192 * dcos(N) - 0.00610 * dcos(2*N);
	Del = sqrt (R*R + r*r - 2*R*r*dcos(K));
	psi = raddeg (asin (R/Del*dsin(K)));

	solc = (d - Del/173.);	/* speed of light correction */
	tmp = psi - B;

	u1 = 84.5506 + 203.4058630 * solc + tmp;
	u2 = 41.5015 + 101.2916323 * solc + tmp;
	u3 = 109.9770 + 50.2345169 * solc + tmp;
	u4 = 176.3586 + 21.4879802 * solc + tmp;

	G = 187.3 + 50.310674 * solc;
	H = 311.1 + 21.569229 * solc;
      
	cor_u1 =  0.472 * dsin (2*(u1-u2));
	cor_u2 =  1.073 * dsin (2*(u2-u3));
	cor_u3 =  0.174 * dsin (G);
	cor_u4 =  0.845 * dsin (H);
      
	r1 = 5.9061 - 0.0244 * dcos (2*(u1-u2));
	r2 = 9.3972 - 0.0889 * dcos (2*(u2-u3));
	r3 = 14.9894 - 0.0227 * dcos (G);
	r4 = 26.3649 - 0.1944 * dcos (H);

	md[1].x = -r1 * dsin (u1+cor_u1);
	md[2].x = -r2 * dsin (u2+cor_u2);
	md[3].x = -r3 * dsin (u3+cor_u3);
	md[4].x = -r4 * dsin (u4+cor_u4);

	lam = 238.05 + 0.083091*d + 0.33*dsin(V) + B;
	Ds = 3.07*dsin(lam + 44.5);
	De = Ds - 2.15*dsin(psi)*dcos(lam+24.)
		- 1.31*(r-Del)/Del*dsin(lam-99.4);
	dsinDe = dsin(De);

	z1 = r1 * dcos(u1+cor_u1);
	z2 = r2 * dcos(u2+cor_u2);
	z3 = r3 * dcos(u3+cor_u3);
	z4 = r4 * dcos(u4+cor_u4);

	md[1].y = z1*dsinDe;
	md[2].y = z2*dsinDe;
	md[3].y = z3*dsinDe;
	md[4].y = z4*dsinDe;

	md[1].z = z1;
	md[2].z = z2;
	md[3].z = z3;
	md[4].z = z4;

	md[1].mag = 5.0;	/* from Pasachoff/Menzel */
	md[2].mag = 5.3;
	md[3].mag = 4.6;
	md[4].mag = 5.6;

	*cmlI  = degrad(268.28 + 877.8169088*(d - Del/173) + psi - B);
	range (cmlI, 2*PI);
	*cmlII = degrad(290.28 + 870.1869088*(d - Del/173) + psi - B);
	range (cmlII, 2*PI);

	{
	    Obj *op = db_basic (JUPITER);

	    md[0].x = 0;
	    md[0].y = 0;
	    md[0].z = 0;
	    md[0].mag = op->s_mag/MAGSCALE;

	    md[0].ra = op->s_ra;
	    md[0].dec = op->s_dec;

	    *jupsize = degrad((double)op->s_size/3600.0);
	}
}
