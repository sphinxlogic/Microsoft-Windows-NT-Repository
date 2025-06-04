/* code to manage the stuff on the moon display.
 */

#include <stdio.h>
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
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/Scale.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/ScrolledW.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"


extern Widget toplevel_w;
extern char *myclass;
extern XtAppContext xe_app;
#define XtD XtDisplay(toplevel_w)


extern FILE *fopenh P_((char *name, char *how));
extern Now *mm_get_now P_((void));
extern Obj *db_basic P_((int id));
extern Obj *db_scan P_((DBScan *sp));
extern double atod P_((char *buf));
extern double delra P_((double dra));
extern int any_ison P_((void));
extern int fits_read P_((char *fn, unsigned char **pixp, int *nrp, int *ncp));
extern int get_color_resource P_((Widget w, char *cname, Pixel *p));
extern int magdiam P_((int fmag, double scale, double mag, double size));
extern void db_scaninit P_((DBScan *sp));
extern void db_update P_((Obj *op));
extern void f_double P_((Widget w, char *fmt, double f));
extern void f_pangle P_((Widget w, double a));
extern void f_showit P_((Widget w, char *s));
extern void f_dm_angle P_((Widget w, double a));
extern void fs_dm_angle P_((char out[], double a));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void get_views_font P_((Display *dsp, XFontStruct **fspp));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void llibration P_((double JD, double *llatp, double *llonp));
extern void now_lst P_((Now *np, double *lst));
extern void obj_pickgc P_((Obj *op, Widget w, GC *gcp));
extern void range P_((double *v, double r));
extern void register_selection P_((char *name));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void solve_sphere P_((double A, double b, double cosc, double sinc,
    double *cosap, double *Bp));
extern void sv_draw_obj P_((Display *dsp, Drawable win, GC gc, Obj *op, int x,
    int y, int diam, int dotsonly));
extern char *syserrstr P_((void));
extern void timestamp P_((Now *np, Widget w));
extern void watch_cursor P_((int want));
extern void xe_msg P_((char *msg, int app_modal));


typedef struct {
    Obj o;			/* copy of sky object info */
    int x, y;			/* X location on mda_w */
} SkyObj;

static SkyObj *skyobjs;		/* malloced list of objects in sky background */
static int nskyobjs;		/* number of objects in skyobjs[] list */

#define	MAXR	10		/* max gap when picking sky objects, pixels */	

static void resetSkyObj P_((void));
static void addSkyObj P_((Obj *op, int x, int y));
static Obj *closeSkyObj P_((int x, int y));
static void fill_skypopup P_((Obj *op));
static void m_create_skypopup P_((void));

static void m_create_form P_((void));
static void m_create_msform P_((void));
static void m_create_esform P_((void));
static void m_set_buttons P_((int whether));
static void m_set_a_button P_((Widget pbw, int whether));
static void m_eshine_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_eshineclose_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_eshineup_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_mstats_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_activate_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_unmap_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_close_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_init_gcs P_((void));
static void m_help_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_helpon_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_option_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_shrink_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_sizewidgets P_((void));
static int xy2ll P_((int x, int y, double *ltp, double *lgp));
static int ll2xy P_((double lt, double lg, int *xp, int *yp));
static int overMoon P_((int x, int y));
static void m_pointer_eh P_((Widget w, XtPointer client, XEvent *ev,
    Boolean *continue_to_dispatch));
static void m_exp_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_redraw P_((void));
static void m_refresh P_((XExposeEvent *ep));
static void m_getsize P_((Drawable d, unsigned *wp, unsigned *hp,unsigned *dp));
static void m_draw P_((void));
static int mxim_create P_((void));
static void mxim_setup P_((double srlng));
static int m_shrink P_((int f));
static void mBWdither P_((void));
static void m_resize P_((unsigned char *in, int nr, int nc, int f,
    unsigned char *out));
static void m_stars P_((void));
static void fliptb P_((void));
static void fliplr P_((void));
static void mi_draw P_((double srlng, double limb));
static void m_features P_((void));
static void m_orientation P_((void));
static void m_labels P_((void));
static void m_sky P_((void));
static void m_grid P_((void));
static void m_mark_libr P_((double limb));
static void m_reportloc P_((Display *dsp, Window win, int x, int y));

static void makeGlassImage P_((Display *dsp));
static void makeGlassGC P_((Display *dsp, Window win));
static void doGlass P_((Display *dsp, Window win, int b1p, int m1,
    int b1r, int wx, int wy, unsigned wid, unsigned hei));
static void fillGlass P_((int wx, int wy));
static void m_readdb P_((void));
static void m_popup P_((XEvent *ep));

/* stuff for the moon image.
 * obviously, size and centering metrics are tuned to a particular image :-)
 */
#define MOONRAD		314	/* radius of moon image, pixels */
#define TOPMARG		21	/* top margin, pixels */
#define LEFTMARG	21	/* left margin, pixels */
#define	MNROWS		670	/* number of rows in moon image */
#define	MNCOLS		670	/* number of columns in moon image */
#define	MAXGRAY		30	/* max gray colors to use for image moon */
#define	BORD		100	/* extra drawing area border for labels/stars */
#define	CLNG	degrad(-4.3)	/* longitude of image center, rads east */
#define	CLAT	degrad(1.0)	/* latitude of image center, rads north */
static unsigned char *mimage;	/* malloced array of full-size moon image */
static Pixel mgray[MAXGRAY];	/* gray-scale ramp for drawing image */
static int nmgray;		/* number of pixels usable in mgray[] */
static Pixel mbg;		/* background color for image */
static char mfxr[] = "MoonFile";/* X resource naming moon file */
static int mdepth;		/* depth of image, in bits */
static int mbpp;		/* bits per pixel in image: 1, 8, 16 or 32 */
static int topmarg, leftmarg;	/* current topmarg/leftmarg after resize */
static int moonrad;		/* current moonrad after resize */
static int mnrows, mncols;	/* current size of image after resize */
static XImage *m_xim;		/* XImage of moon now at current size */

#define	MAXSHRINK	6	/* max shrink factor */
static int mshrink;		/* current shrink factor: 1 .. MAXSHRINK */
#define	MINKM		125	/* smallest feature we ever label, km */
#define	ESHINEF		10	/* earthshine factor range: 0 .. this */
#define	LMFRAC		40	/* libr marker is 1/x this of total size */
#define	LGAP		20	/* gap between NSEW labels and image edge */
#define	FMAG		6	/* faintest mag of sky background object */

/* main's widgets */
static Widget mform_w;		/* main moon form dialog */
static Widget msform_w;		/* statistics form dialog */
static Widget mda_w;		/* image view DrawingArea */
static Pixmap m_pm;		/* image view staging pixmap */
static Widget dt_w;		/* main date/time stamp widget */
static Widget sdt_w;		/* statistics date/time stamp widget */
static Widget msw_w;		/* the scrolled window */

/* "More info" widgets */
static Widget srlng_w;		/* sunrise longitude PB widget */
static Widget llat_w;		/* lib in lat PB widget */
static Widget llong_w;		/* lib in long PB widget */
static Widget limb_w;		/* limb angle PB widget */
static Widget lib_w;		/* total lib PB widget */
static Widget sunalt_w;		/* sun altitude under cursor */
static Widget lat_w, lng_w;	/* lat/long under cursor */
static int mswasman;            /* whether "More info" form was managed */

/* earthshine widgets */
static Widget esform_w;		/* main dialog */
static Widget eshine_w;		/* earthshine factor scale */

/* lunar surface popup's widgets */
static Widget pu_w;		/* main popup */
static Widget pu_name_w;	/* popup name label */
static Widget pu_type_w;	/* popup type label */
static Widget pu_size_w;	/* popup size label */
static Widget pu_lat_w;		/* popup latitude label */
static Widget pu_lng_w;		/* popup longitude label */
static Widget pu_alt_w;		/* popup sun altitude label */

/* sky background object's widgets */
static Widget skypu_w;		/* main popup */
static Widget skypu_name_w;	/* popup name label */

static GC m_fgc, m_bgc, m_agc;	/* various GCs */

static int m_selecting;		/* set while our fields are being selected */

static XImage *glass_xim;	/* glass XImage -- 0 means new or can't */
static GC glassGC;		/* GC for glass border */

#define	GLASSSZ		50	/* mag glass width and heigth, pixels */
#define	GLASSMAG	2	/* mag glass factor (may be any integer > 0) */

/* options list */
typedef enum {
    APOLLO_OPT, GRID_OPT, LABELS_OPT, FLIPLR_OPT, FLIPTB_OPT,
    STARS_OPT, SKY_OPT,
    N_OPT
} Option;
static int option[N_OPT];
static Widget option_w[N_OPT];

#define	NSTARS		150	/* total number of stars to sprinkle in */
#define	NBSTARS		20	/* number of bright stars to sprinkle in */
#define	NVBSTARS	5	/* number of very bright stars to sprinkle in */

/* return the effective x or y values, allowing for flip options.
 * N.B. this is with respect to the image, not the drawing area.
 */
#define	FX(x)	(option[FLIPLR_OPT] ? mncols-1-(x) : (x))
#define	FY(y)	(option[FLIPTB_OPT] ? mnrows-1-(y) : (y))

/* special features list */
typedef struct {
    double lt, lg;		/* degrees N, E */
    char *name;
} LFeatures;
static LFeatures lfeatures[] = {
    {  0.8,  23.5, "Apollo 11"},
    { -3.2, 336.6, "Apollo 12"},
    { -3.7, 342.5, "Apollo 14"},
    { 26.0,   3.6, "Apollo 15"},
    { -9.0,  15.6, "Apollo 16"},
    { 20.1,  30.8, "Apollo 17"},
};

/* lunar database */
typedef struct {
    char *name;			/* malloced name */
    char *type;			/* malloced type (may be shared) */
    short x, y;			/* image location, X pixels coords */
    short sz; 			/* diam without forshortening, km */
} MoonDB;
static MoonDB *moondb;		/* malloced array from file */
static int nmoondb;		/* entries in moondb[] */
static char mdbxr[] = "MoonDB";	/* name of X resource naming moon db file */
#define	MDIA	3476		/* moon diameter, km */
static int m_wantlabel P_((MoonDB *mp));
static void fill_popup P_((MoonDB *mp, int x, int y));
static void m_create_popup P_((void));
static MoonDB *closeMoonDB P_((int x, int y));

/* called when the moon view is activated via the main menu pulldown.
 * if first time, build everything, else just toggle whether we are mapped.
 * allow for retrying to read the image file each time until find it.
 */
void
m_manage ()
{
	if (!mform_w) {
	    /* menu one-time-only work */

	    /* build dialogs */
	    m_create_form();
	    m_create_msform();
	    m_create_popup();
	    m_create_skypopup();
	    m_create_esform();

	    /* establish depth, colors and bits per pixel */
	    get_something (mda_w, XmNdepth, (XtArgVal)&mdepth);
	    m_init_gcs();
	    mbpp = (mdepth == 1 || nmgray == 2) ? 1 :
				    (mdepth>=17 ? 32 : (mdepth >= 9 ? 16 : 8));
	}

	if (!mimage) {
	    /* read image and display at initial size */
	    if (m_shrink(mshrink) < 0)
		return;
	}
	
	if (XtIsManaged(mform_w)) {
	    XtUnmanageChild (mform_w);
	} else {
	    XtManageChild (mform_w);
	    if (mswasman) {
		XtManageChild (msform_w);
		m_set_buttons(m_selecting);
	    }
	    /* the expose will do the drawing */
	}
}

int
m_ison()
{
	return (mform_w && XtIsManaged(mform_w));
}

/* commanded from main to update with a new set of circumstances */
void
m_update (np, how_much)
Now *np;
int how_much;
{
	if (!mform_w)
	    return;
	if (!XtIsManaged(mform_w) && !any_ison() && !how_much)
	    return;

	watch_cursor (1);

	m_redraw();

	watch_cursor (0);
}

/* called by other menus as they want to hear from our buttons or not.
 * the "on"s and "off"s stack - only really redo the buttons if it's the
 * first on or the last off.
 */
void
m_selection_mode (whether)
int whether;	/* whether setting up for plotting or for not plotting */
{
	m_selecting += whether ? 1 : -1;

	if (m_ison()) {
	    if ((whether && m_selecting == 1)     /* first one to want on */
		|| (!whether && m_selecting == 0) /* last one to want off */)
		m_set_buttons (whether);
	}
}

/* called to put up or remove the watch cursor.  */
void
m_cursor (c)
Cursor c;
{
	Window win;

	if (mform_w && (win = XtWindow(mform_w))) {
	    Display *dsp = XtDisplay(mform_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}

	if (msform_w && (win = XtWindow(msform_w))) {
	    Display *dsp = XtDisplay(msform_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}

static void
m_create_form()
{
	typedef struct {
	    Option o;		/* which option */
	    char *name;		/* name of TB */
	    char *title;	/* title string of option */
	} OpSetup;
	static OpSetup ops[] = {
	    {APOLLO_OPT,	"Apollo",	"Apollo sites"},
	    {LABELS_OPT,	"Labels",	"More labels"},
	    {STARS_OPT,		"FakeStars",	"Fake stars"},
	    {SKY_OPT,		"SkyBkg",	"Sky background"},
	    {FLIPTB_OPT,	"FlipTB",	"Flip T/B"},
	    {FLIPLR_OPT,	"FlipLR",	"Flip L/R"},
	    {GRID_OPT,		"Grid",		"Grid"},
	};
	typedef struct {
	    char *label;	/* what goes on the help label */
	    char *key;		/* string to call hlp_dialog() */
	} HelpOn;
	static HelpOn helpon[] = {
	    {"Intro...",	"Moon - intro"},
	    {"on Mouse...",	"Moon - mouse"},
	    {"on Control...",	"Moon - control"},
	    {"on View...",	"Moon - view"},
	    {"on Scale...",	"Moon - scale"},
	};
	Widget mb_w, pd_w, cb_w;
	char msg[1024];
	Widget w;
	XmString str;
	unsigned long mask;
	Arg args[20];
	int i;
	int n;

	/* create master form */
	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNhorizontalSpacing, 5); n++;
	XtSetArg (args[n], XmNverticalSpacing, 5); n++;
	XtSetArg (args[n], XmNdefaultPosition, False); n++;
	mform_w = XmCreateFormDialog (toplevel_w, "Moon", args, n);
	XtAddCallback (mform_w, XmNhelpCallback, m_help_cb, 0);
	XtAddCallback (mform_w, XmNunmapCallback, m_unmap_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Moon view"); n++;
	XtSetValues (XtParent(mform_w), args, n);

	/* create the menu bar across the top */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	mb_w = XmCreateMenuBar (mform_w, "MB", args, n);
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

	    /* button to bring up the earthshine dialog */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "ESPB", args, n);
	    XtAddCallback (w, XmNactivateCallback, m_eshineup_cb, 0);
	    set_xmstring (w, XmNlabelString, "Set Earthshine...");
	    XtManageChild (w);

	    /* add a separator */
	    n = 0;
	    w = XmCreateSeparator (pd_w, "CtS", args, n);
	    XtManageChild (w);

	    /* add the close button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Close", args, n);
	    XtAddCallback (w, XmNactivateCallback, m_close_cb, 0);
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

	    /* add options */

	    for (i = 0; i < XtNumber(ops); i++) {
		OpSetup *osp = &ops[i];
		Option o = osp->o;

		n = 0;
		XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
		XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY); n++;
		w = XmCreateToggleButton (pd_w, osp->name, args, n);
		XtAddCallback(w, XmNvalueChangedCallback, m_option_cb,
								(XtPointer)o);
		set_xmstring (w, XmNlabelString, osp->title);
		option[o] = XmToggleButtonGetState (w);
		option_w[o] = w;
		XtManageChild (w);
	    }

	    /* add a separator */

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    /* add the More Info control */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Stats", args, n);
	    set_xmstring (w, XmNlabelString, "More info...");
	    XtAddCallback (w, XmNactivateCallback, m_mstats_cb, NULL);
	    XtManageChild (w);

	/* make the Scale pulldown */

	n = 0;
	XtSetArg (args[n], XmNradioBehavior, True); n++;
	pd_w = XmCreatePulldownMenu (mb_w, "ScalePD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'S'); n++;
	    cb_w = XmCreateCascadeButton (mb_w, "ScaleCB", args, n);
	    set_xmstring (cb_w, XmNlabelString, "Scale");
	    XtManageChild (cb_w);

	    for (i = 1; i <= MAXSHRINK; i++) {
		char name[64], title[64];
		int shrink;

		/* just advertise whole values */
		if (MAXSHRINK%i)
		    continue;
		shrink = MAXSHRINK/i;

		(void) sprintf (name, "Scale%dX", shrink);
		n = 0;
		w = XmCreateToggleButton (pd_w, name, args, n);
		XtAddCallback(w, XmNvalueChangedCallback, m_shrink_cb,
								(XtPointer)i);
		(void) sprintf (title, " Scale %d X", shrink);
		set_xmstring (w, XmNlabelString, title);
		XtManageChild (w);

		/* pick up user's default */
		if (XmToggleButtonGetState(w)) {
		    if (mshrink != 0) {
			(void)sprintf(msg,"Multiple setting: ignoring Moon*%s",
									name);
			xe_msg (msg, 0);
		    } else
			mshrink = i;
		}
	    }

	    /* if none found to be set, default to last one -- MAXSHRINK */
	    if (mshrink == 0) {
		xe_msg("No Moon*Scale<n>X resource found -- defaulting to 1",0);
		XmToggleButtonSetState (w, True, False);
		mshrink = MAXSHRINK;
	    }

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
		XtAddCallback (w, XmNactivateCallback, m_helpon_cb,
							(XtPointer)(hp->key));
		XtManageChild (w);
		XmStringFree(str);
	    }

	/* make a label for the date stamp */

	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrecomputeSize, False); n++;
	dt_w = XmCreateLabel (mform_w, "DateStamp", args, n);
	timestamp (mm_get_now(), dt_w);	/* sets initial size */
	XtManageChild (dt_w);

	/* make a drawing area in a scrolled window for the image view */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, dt_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
	XtSetArg (args[n], XmNvisualPolicy, XmVARIABLE); n++;
	msw_w = XmCreateScrolledWindow (mform_w, "MoonSW", args, n);
	XtManageChild (msw_w);

	    /* size gets set once we know image size and scale */
	    n = 0;
	    XtSetArg (args[n], XmNmarginWidth, 0); n++;
	    XtSetArg (args[n], XmNmarginHeight, 0); n++;
	    mda_w = XmCreateDrawingArea (msw_w, "MoonDA", args, n);
	    XtAddCallback (mda_w, XmNexposeCallback, m_exp_cb, NULL);
	    mask = Button1MotionMask | ButtonPressMask | ButtonReleaseMask
						    | PointerMotionHintMask;
	    XtAddEventHandler (mda_w, mask, False, m_pointer_eh, 0);
	    XtManageChild (mda_w);

	    /* SW assumes work is its child but just to be tiddy about it .. */
	    set_something (msw_w, XmNworkWindow, (XtArgVal)mda_w);

	/* match SW background to DA */
	get_something (msw_w, XmNclipWindow, (XtArgVal)&w);
	if (w) {
	    Pixel p;
	    get_something (mda_w, XmNbackground, (XtArgVal)&p);
	    set_something (w, XmNbackground, (XtArgVal)p);
	}
}

static void
m_create_msform()
{
	Widget rc_w;
	Widget sep_w;
	Widget w;
	XmString str;
	Arg args[20];
	int n;

	/* create form */
	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNverticalSpacing, 5); n++;
	msform_w = XmCreateFormDialog (toplevel_w, "MoonStats", args, n);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Moon info"); n++;
	XtSetValues (XtParent(msform_w), args, n);

	/* make a rowcolumn to hold the cursor tracking info */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNspacing, 3); n++; /* RC below has buttons */
	XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNnumColumns, 3); n++;
	XtSetArg (args[n], XmNisAligned, False); n++;
	rc_w = XmCreateRowColumn (msform_w, "SRC", args, n);
	XtManageChild (rc_w);

	    str = XmStringCreate ("Latitude +N:", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    w = XmCreateLabel (rc_w, "LatL", args, n);
	    XtManageChild (w);
	    XmStringFree (str);

	    n = 0;
	    XtSetArg (args[n], XmNrecomputeSize, False); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	    lat_w = XmCreateLabel (rc_w, "Lat", args, n);
	    set_xmstring (lat_w, XmNlabelString, "0:00");
	    XtManageChild (lat_w);

	    str = XmStringCreate ("Longitude +E:", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    w = XmCreateLabel (rc_w, "LngL", args, n);
	    XtManageChild (w);
	    XmStringFree (str);

	    n = 0;
	    XtSetArg (args[n], XmNrecomputeSize, False); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	    lng_w = XmCreateLabel (rc_w, "Lng", args, n);
	    set_xmstring (lng_w, XmNlabelString, "0:00");
	    XtManageChild (lng_w);

	    str = XmStringCreate ("Sun altitude:", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    w = XmCreateLabel (rc_w, "SunAL", args, n);
	    XtManageChild (w);
	    XmStringFree (str);

	    n = 0;
	    XtSetArg (args[n], XmNrecomputeSize, False); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	    sunalt_w = XmCreateLabel (rc_w, "SunA", args, n);
	    set_xmstring (sunalt_w, XmNlabelString, "0:00");
	    XtManageChild (sunalt_w);

	/* make a separator between the 2 data sets */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (msform_w, "Sep1", args, n);
	XtManageChild(sep_w);

	/* make a rowcolumn to hold the labels and info buttons */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNnumColumns, 5); n++;
	XtSetArg (args[n], XmNisAligned, False); n++;
	rc_w = XmCreateRowColumn (msform_w, "SRC", args, n);
	XtManageChild (rc_w);

	    /* make the srlng and libration in lat/long rows */

	    str = XmStringCreate ("Sunrise Long:", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    w = XmCreateLabel (rc_w, "MLCoL", args, n);
	    XtManageChild (w);
	    XmStringFree (str);

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	    XtSetArg (args[n], XmNuserData, "Moon.SunRiseLng"); n++;
	    srlng_w = XmCreatePushButton (rc_w, "MLCoLPB", args, n);
	    XtAddCallback (srlng_w, XmNactivateCallback, m_activate_cb, NULL);
	    XtManageChild (srlng_w);

	    str = XmStringCreate ("Libr in Lat:", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    w = XmCreateLabel (rc_w, "MLLatL", args, n);
	    XtManageChild (w);
	    XmStringFree (str);

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	    XtSetArg (args[n], XmNuserData, "Moon.LibrLat"); n++;
	    llat_w = XmCreatePushButton (rc_w, "MLLatPB", args, n);
	    XtAddCallback (llat_w, XmNactivateCallback, m_activate_cb, NULL);
	    XtManageChild (llat_w);

	    str = XmStringCreate ("Libr in Long:", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    w = XmCreateLabel (rc_w, "MLLongL", args, n);
	    XtManageChild (w);
	    XmStringFree (str);

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	    XtSetArg (args[n], XmNuserData, "Moon.LibrLong"); n++;
	    llong_w = XmCreatePushButton (rc_w, "MLLongPB", args, n);
	    XtAddCallback (llong_w, XmNactivateCallback, m_activate_cb, NULL);
	    XtManageChild (llong_w);

	    /* make the limb/tilt rows */

	    str = XmStringCreate ("Limb angle:", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    w = XmCreateLabel (rc_w, "MLimbL", args, n);
	    XtManageChild (w);
	    XmStringFree (str);

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	    XtSetArg (args[n], XmNuserData, "Moon.LibrLimb"); n++;
	    limb_w = XmCreatePushButton (rc_w, "MLimbPB", args, n);
	    XtAddCallback (limb_w, XmNactivateCallback, m_activate_cb, NULL);
	    XtManageChild (limb_w);

	    str = XmStringCreate ("Tilt:", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    w = XmCreateLabel (rc_w, "MLibL", args, n);
	    XtManageChild (w);
	    XmStringFree (str);

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	    XtSetArg (args[n], XmNuserData, "Moon.LibrTilt"); n++;
	    lib_w = XmCreatePushButton (rc_w, "MLibPB", args, n);
	    XtAddCallback (lib_w, XmNactivateCallback, m_activate_cb, NULL);
	    XtManageChild (lib_w);

	/* add a separator */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (msform_w, "Sep2", args, n);
	XtManageChild (sep_w);

	/* add a label for the current date/time stamp */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	sdt_w = XmCreateLabel (msform_w, "SDTstamp", args, n);
	XtManageChild (sdt_w);

	/* add a separator */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sdt_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (msform_w, "Sep3", args, n);
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
	w = XmCreatePushButton (msform_w, "Close", args, n);
	XtAddCallback (w, XmNactivateCallback, m_mstats_cb, NULL);
	XtManageChild (w);
}

/* create the little earthshine scale factor dialog */
static void
m_create_esform()
{
	Arg args[20];
	Widget sep_w;
	Widget w;
	int min, max, v;
	int n;

	/* create form */
	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNfractionBase, 10); n++;
	XtSetArg (args[n], XmNverticalSpacing, 5); n++;
	esform_w = XmCreateFormDialog (toplevel_w, "MoonES", args, n);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Moon Earthshine"); n++;
	XtSetValues (XtParent(esform_w), args, n);

	/* make a scale */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNshowValue, True); n++;
	XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	XtSetArg (args[n], XmNminimum, 0); n++;
	XtSetArg (args[n], XmNmaximum, ESHINEF); n++;
	eshine_w = XmCreateScale (esform_w, "Earthshine", args, n);
	XtManageChild (eshine_w);

	get_something (eshine_w, XmNminimum, (XtArgVal)&min);
	get_something (eshine_w, XmNmaximum, (XtArgVal)&max);
	get_something (eshine_w, XmNvalue, (XtArgVal)&v);

	if (min >= max || v < min || v > max || v == 0) {
	    xe_msg ("Bogus moon Earthshine values -- setting defaults", 0);
	    set_something (eshine_w, XmNminimum, (XtArgVal)1);
	    set_something (eshine_w, XmNmaximum, (XtArgVal)10);
	    set_something (eshine_w, XmNvalue, (XtArgVal)4);
	}

	/* make a separator */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, eshine_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (esform_w, "Sep", args, n);
	XtManageChild(sep_w);

	/* make the buttons at the bottom */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 2); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 4); n++;
	w = XmCreatePushButton (esform_w, "Apply", args, n);
	XtAddCallback (w, XmNactivateCallback, m_eshine_cb, NULL);
	XtManageChild (w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 6); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 8); n++;
	w = XmCreatePushButton (esform_w, "Close", args, n);
	XtAddCallback (w, XmNactivateCallback, m_eshineclose_cb, NULL);
	XtManageChild (w);
}

/* go through all the buttons pickable for plotting and set whether they
 * should appear to look like buttons or just flat labels.
 */
static void
m_set_buttons (whether)
int whether;	/* whether setting up for plotting or for not plotting */
{
	m_set_a_button (srlng_w, whether);
	m_set_a_button (llat_w, whether);
	m_set_a_button (llong_w, whether);
	m_set_a_button (limb_w, whether);
	m_set_a_button (lib_w, whether);
}

/* set whether the given button looks like a label.
 */
static void
m_set_a_button(pbw, whether)
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
	    tmpw = XmCreatePushButton (mform_w, "tmp", args, n);

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

/* callback from the Apply button in the earthshine dialog */
/* ARGSUSED */
static void
m_eshine_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	m_redraw();
}

/* callback from the Close button in the earthshine dialog */
/* ARGSUSED */
static void
m_eshineclose_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (esform_w);
}

/* called to toggle whether the earthshine eshineactor is dialog */
/* ARGSUSED */
static void
m_eshineup_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (XtIsManaged(esform_w))
	    XtUnmanageChild (esform_w);
	else
	    XtManageChild (esform_w);
}


/* callback from the Close button is activated on the stats menu or when
 * the More Info button is activated. they each do the same thing.
 */
/* ARGSUSED */
static void
m_mstats_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if ((mswasman = XtIsManaged(msform_w)))
	    XtUnmanageChild (msform_w);
	else {
	    XtManageChild (msform_w);
	    m_set_buttons(m_selecting);
	}
}

/* callback from any of the data menu buttons being activated.
 */
/* ARGSUSED */
static void
m_activate_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (m_selecting) {
	    char *name;
	    get_something (w, XmNuserData, (XtArgVal)&name);
	    register_selection (name);
	}
}

/* callback from mform_w being unmapped.
 */
/* ARGSUSED */
static void
m_unmap_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (esform_w);

	if ((mswasman = XtIsManaged(msform_w)))
	    XtUnmanageChild (msform_w);

	/* freeing the pixmap will prevent any useless updates while off and
	 * insure a fresh update on the next expose.
	 */
	if (m_pm) {
	    XFreePixmap (XtD, m_pm);
	    m_pm = 0;
	}
}

/* called from Close button */
static void
m_close_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (mform_w);

	/* unmapping mform_w will also unmap msform_w */
}

/* callback from the any of the option TBs.
 * Option enum is in client.
 */
/* ARGSUSED */
static void
m_option_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	Option opt = (Option)client;
	int set;

	watch_cursor (1);

	/* toggle the option */
	option[opt] = set = XmToggleButtonGetState (w);

	switch (opt) {
	case APOLLO_OPT:
	    if (set) {
		m_features();
		m_refresh (NULL);
	    } else
		m_redraw();
	    break;

	case GRID_OPT:
	    if (set) {
		m_grid();
		m_refresh (NULL);
	    } else
		m_redraw();
	    break;

	case SKY_OPT:
	    if (set) {
		m_sky();
		m_refresh (NULL);
	    } else
		m_redraw();
	    break;

	case STARS_OPT:
	    if (set) {
		m_stars();
		m_refresh (NULL);
	    } else
		m_redraw();
	    break;

	case LABELS_OPT:
	    if (set) {
		m_labels();
		m_refresh (NULL);
	    } else
		m_redraw();
	    break;

	case FLIPTB_OPT:
	    fliptb();
	    m_redraw();
	    break;

	case FLIPLR_OPT:
	    fliplr();
	    m_redraw();
	    break;

	case N_OPT:
	    break;
	}


	watch_cursor (0);
}

/* callback from any of the Scale TBs.
 * client is shrink factor: 1..MAXSHRINK
 */
/* ARGSUSED */
static void
m_shrink_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int f = (int)client;
	int set = XmToggleButtonGetState (w);

	if (!set || f == mshrink)
	    return;
	if (m_shrink (f) < 0)
	    return;
	mshrink = f;
}

/* callback from the Help all button
 */
/* ARGSUSED */
static void
m_help_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static char *msg[] = {
	    "This is a depiction of the Moon.",
	};

	hlp_dialog ("Moon", msg, XtNumber(msg));
}

/* callback from a specific Help button.
 * client is a string to use with hlp_dialog().
 */
/* ARGSUSED */
static void
m_helpon_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	hlp_dialog ((char *)client, NULL, 0);
}


/* given an [x,y] on m_xim, find latitude, rads north, and longitude, rads e.
 * N.B. [x,y] must be with respect to the _original_ image size and orientation.
 * return 0 if ok (inside image) else -1.
 */
static int
xy2ll (x, y, ltp, lgp)
int x, y;
double *ltp, *lgp;
{
	double cx, cy;
	double lt, lg;

	/* convert to image center, scaled to radius */
	cx = (double)(x - (leftmarg + moonrad))/moonrad;	/* + right */
	cy = (double)((topmarg + moonrad)  - y)/moonrad;	/* + up */

	if (cx*cx + cy*cy > 1.0)
	    return (-1);

	lt = asin (cy);
	lg = asin(cx/cos(lt));

	lt += CLAT;
	lg += CLNG;
	if (lt > PI/2) {
	    lt = PI - lt;
	    lg += PI;
	} else if (lt < -PI/2) {
	    lt = -PI - lt;
	    lg += PI;
	}

	*ltp = lt;
	*lgp = lg;
	range (lgp, 2*PI);

	return (0);
}

/* given a lat(+N)/long(+E) find its [x,y] location on mda_w.
 * N.B. the returned [x,y] is with respect to the _original_ size and
 *   orientation.
 * return 0 if ok (on front side) else -1.
 */
static int
ll2xy (lt, lg, xp, yp)
double lt, lg;
int *xp, *yp;
{
	double cx, cy, cz;
	double coslt, sinlt;

	lt -= CLAT;
	lg -= CLNG;
	if (lt > PI/2) {
	    lt = PI - lt;
	    lg += PI;
	} else if (lt < -PI/2) {
	    lt = -PI - lt;
	    lg += PI;
	}

	coslt = cos(lt);
	cz = moonrad*coslt*cos(lg);
	if (cz < 0.0)
	    return (-1);

	sinlt = sin(lt);
	cx = moonrad*coslt*sin(lg);
	cy = moonrad*sinlt;

	*xp = (int)(cx + (leftmarg + moonrad) + 0.5);
	*yp = (int)((topmarg + moonrad) -  cy + 0.5);

	return (0);
}

/* return True if the given [x,y], relative to mda_w, refers to a spot over the
 * lunar image, else return 0.
 */
static int
overMoon (x, y)
int x, y;
{
	/* convert to image center */
	x -= BORD + leftmarg + moonrad;
	y -= BORD + topmarg  + moonrad;

	return (x*x + y*y <= moonrad*moonrad);
}

/* event handler from all Button events on the mda_w */
static void
m_pointer_eh (w, client, ev, continue_to_dispatch)
Widget w;
XtPointer client;
XEvent *ev;
Boolean *continue_to_dispatch;
{
	Display *dsp = ev->xany.display;
	Window win = ev->xany.window;
	int evt = ev->type;
	Window root, child;
	int rx, ry, x, y;
	unsigned mask;
	int m1, b1p, b1r, b3p;

	/* what happened? */
	m1  = evt == MotionNotify  && ev->xmotion.state  == Button1Mask;
	b1p = evt == ButtonPress   && ev->xbutton.button == Button1;
	b1r = evt == ButtonRelease && ev->xbutton.button == Button1;
	b3p = evt == ButtonPress   && ev->xbutton.button == Button3;

	/* do we care? */
	if (!m1 && !b1p && !b1r && !b3p)
	    return;

	/* where are we? */
	XQueryPointer (dsp, win, &root, &child, &rx, &ry, &x, &y, &mask);

	/* dispatch */
	if (b3p)
	    m_popup (ev);
	if (b1p || m1 || b1r) {
	    doGlass (dsp, win, b1p, m1, b1r, x, y, mncols, mnrows);
	    m_reportloc (dsp, win, x, y);
	}
}

/* expose (or reconfig) of moon image view drawing area.
 */
/* ARGSUSED */
static void
m_exp_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;
	XExposeEvent *e = &c->event->xexpose;
	Display *dsp = e->display;
	Window win = e->window;

	watch_cursor (1);

	switch (c->reason) {
	case XmCR_EXPOSE: {
	    /* turn off gravity so we get expose events for either shrink or
	     * expand.
	     */
	    static before;

	    if (!before) {
		XSetWindowAttributes swa;
		unsigned long mask = CWBitGravity | CWBackingStore;

		swa.bit_gravity = ForgetGravity;
		swa.backing_store = NotUseful;
		XChangeWindowAttributes (e->display, e->window, mask, &swa);
		before = 1;
	    }
	    break;
	    }
	default:
	    printf ("Unexpected mda_w event. type=%d\n", c->reason);
	    exit(1);
	}


	/* insure pixmap exists -- it gets destroyed whenever mda_w is resized
	 * or the dialog is unmanaged.
	 */
	if (!m_pm) {
	    unsigned wid, hei, d;

	    m_getsize (win, &wid, &hei, &d);
	    if (wid != mncols+2*BORD || hei != mnrows+2*BORD) {
		printf ("m_da: Bad size: wid=%d mncols=%d hei=%d mnrows=%d\n",
					wid, mncols, hei, mnrows);
		exit(1);
	    }

	    m_pm = XCreatePixmap (dsp, win, wid, hei, d);
	    m_draw();
	}

	/* update exposed area */
	m_refresh (e);

	watch_cursor (0);
}

/* redraw the current scene */
static void
m_redraw()
{
	watch_cursor (1);

	m_draw ();
	if (m_pm)
	    m_refresh(NULL);

	watch_cursor (0);
}

/* copy the m_pm pixmap to the drawing area mda_w.
 * if ep just copy that much, else copy all.
 */
static void
m_refresh(ep)
XExposeEvent *ep;
{
	Display *dsp = XtDisplay(mda_w);
	Window win = XtWindow (mda_w);
	Pixmap pm = m_pm;
	unsigned w, h;
	int x, y;

	if (ep) {
	    x = ep->x;
	    y = ep->y;
	    w = ep->width;
	    h = ep->height;
	} else {
	    w = mncols+2*BORD;
	    h = mnrows+2*BORD;
	    x = y = 0;
	}

	XCopyArea (dsp, pm, win, m_fgc, x, y, w, h, x, y);
}

/* get the width, height and depth of the given drawable */
static void
m_getsize (d, wp, hp, dp)
Drawable d;
unsigned *wp, *hp, *dp;
{
	Window root;
	int x, y;
	unsigned int bw;

	XGetGeometry (XtD, d, &root, &x, &y, wp, hp, &bw, dp);
}

/* make the various gcs, handy pixel values and fill in mgray[].
 * N.B. just call this once.
 */
static void
m_init_gcs()
{
	Display *dsp = XtD;
	Window win = XtWindow(toplevel_w);
	Colormap cm = DefaultColormap(dsp, 0);
	XGCValues gcv;
	unsigned int gcm;
	XFontStruct *fsp;
	Pixel fg;
	Pixel p;

	/* make gcs from MoonDA colors
	 */
	get_something (mda_w, XmNforeground, (XtArgVal)&fg);
	get_something (mda_w, XmNbackground, (XtArgVal)&mbg);

	gcm = GCForeground | GCBackground;
	gcv.foreground = fg;
	gcv.background = mbg;
	m_fgc = XCreateGC (dsp, win, gcm, &gcv);

	gcv.foreground = mbg;
	gcv.background = fg;
	m_bgc = XCreateGC (dsp, win, gcm, &gcv);

	/* make the label marker gc.
	 * beware of monochrome screens that claim ok for any ol' color.
	 */
	if (get_color_resource (mda_w, "MoonAnnotColor", &p) < 0
						    || p == mbg || p == fg) {
	    xe_msg("Can not get moon annotation color -- using white.",0);
	    p = WhitePixel (dsp, 0);
	}
	gcm = GCForeground | GCBackground;
	gcv.foreground = p;
	gcv.background = mbg;
	m_agc = XCreateGC (dsp, win, gcm, &gcv);

	get_views_font (dsp, &fsp);
	XSetFont (dsp, m_agc, fsp->fid);

	/* build gray-scale ramp for image */
	if (mdepth == 1) {
	    /* will use an XYPixmap through m_fgc */
	    mgray[0] = 0;	/* use m_fgc->background */
	    mgray[1] = 1;	/* use m_fgc->foreground */
	    nmgray = 2;
	} else {
	    /* depth > 1 so try for MAXGRAY shades of gray.
	     * if can't get that many, build a gray ramp from whaty we can.
	     */
	    int i;

	    /* exit loop with i set to number of gray shades we could allocate*/
	    for (i = 0; i < MAXGRAY; i++) {
		XColor xc;

		xc.red = xc.green = xc.blue = (i*255/(MAXGRAY-1)) << 8;
		if (!XAllocColor(dsp,cm, &xc) || (i>0 && mgray[i-1]==xc.pixel)){
		    /* only found i unique shades -- ok, but recalibrate */

		    /* these aren't graded right */
		    XFreeColors (dsp, cm, mgray, i, 0);

		    /* rebuild ramp */
		    if (i == 1) {
			/* this will force a bitmap so set fg/bg */
			mgray[0] = 0;	/* use m_fgc->background */
			mgray[1] = 1;	/* use m_fgc->foreground */
			i = 2;
		    } else {
			/* start ramp over, this time with just i steps.
			 * N.B. we assume things haven't changed
			 */
			int j;
			for (j = 0; j < i; j++) {
			    xc.red = xc.green = xc.blue = (j*255/(i-1)) << 8;
			    (void) XAllocColor(dsp, cm, &xc);
			    mgray[j] = xc.pixel;
			}
		    }

		    /* stop trying for MAXGRAY colors */
		    break;
		}
		mgray[i] = xc.pixel;
	    }

	    nmgray = i;

	    if (nmgray < MAXGRAY) {
		char msg[1024];
		(void) sprintf(msg,"Wanted %d but only found %d shades of gray for moon.",
							    MAXGRAY, nmgray);
		xe_msg (msg, 0);
	    }

	    if (nmgray > 4) {
		/* jet black looks like we're leaving holes in the image */
		mgray[0] = mgray[1];
	    }
	}
}

/* update "more info" labels.
 * then, if m_pm is defined, compute a scene onto it.
 */
static void
m_draw ()
{
	Now *np = mm_get_now();
	double llat, llong;
	double limb, tilt;
	double colong, srlng;

	/* get libration info */
	llibration (mjd+MJD0, &llat, &llong);
	limb = atan2 (-llong, llat);
	if (limb < 0.0)
	    limb += 2*PI;	/* limb angle is traditionally 0..360 */
	tilt = raddeg(sqrt(llat*llat + llong*llong));

	/* get new terminator location as 0..360 +W */
	moon_colong (mjd+MJD0, 0.0, 0.0, &colong, NULL, NULL);

	/* convert to sunrise longitude */
	srlng = -colong;
	range (&srlng, 2*PI);

	/* update data labels */
	f_dm_angle (srlng_w, srlng);
	f_dm_angle (llat_w, llat);
	f_dm_angle (llong_w, llong);
	f_dm_angle (limb_w, limb);
	f_double (lib_w, "%6.3f", tilt);

	/* update time stamps too */
	timestamp (np, dt_w);
	timestamp (np, sdt_w);

	if (m_pm)
	    mi_draw (srlng-CLNG, limb);
}

/* draw moon onto m_pm.
 * srlng is the longitude of the rising sun, already corrected for image center.
 * N.B. this just fills the pixmap; call m_refresh() to copy to the screen.
 */
static void
mi_draw (srlng, limb)
double srlng;
double limb;
{
	/* check assumptions */
	if (!mimage) {
	    printf ("No moon mimage!\n");
	    exit (1);
	}
	if (!m_xim) {
	    printf ("No m_xim!\n");
	    exit (1);
	}
	if (!m_pm) {
	    printf ("No m_pm Pixmap!\n");
	    exit(1);
	}

	/* clear m_pm */
	XFillRectangle (XtD, m_pm, m_bgc, 0, 0, mncols+2*BORD, mnrows+2*BORD);

	/* fill in m_xim from mimage */
	mxim_setup (srlng);
	
	/* copy m_xim to p_pm */
	XPutImage (XtD, m_pm, m_fgc, m_xim, 0, 0, BORD, BORD, mncols, mnrows);

	/* add stars, if enabled */
	if (option[STARS_OPT])
	    m_stars();

	/* add the libration marker */
	m_mark_libr (limb);

	/* add grid, if enabled */
	if (option[GRID_OPT])
	    m_grid();

	/* add label special feature labels, if enabled */
	if (option[APOLLO_OPT])
	    m_features();

	/* add labels, if enabled */
	if (option[LABELS_OPT])
	    m_labels();

	/* add sky background objects */
	resetSkyObj();
	if (option[SKY_OPT])
	    m_sky();

	/* add orientation markings */
	m_orientation();
}

/* draw the special features list on m_pm */
static void
m_features()
{
	Display *dsp = XtDisplay (mda_w);
	Window win = m_pm;
	int i;

	for (i = 0; i < XtNumber(lfeatures); i++) {
	    LFeatures *lfp = &lfeatures[i];
	    int x, y;

	    if (ll2xy (degrad(lfp->lt), degrad(lfp->lg), &x, &y) == 0) {
		char *n = lfp->name;
		int l = strlen(n);

		x = FX(x) + BORD;
		y = FY(y) + BORD;

		XDrawArc (dsp, win, m_agc, x-2, y-2, 5, 5, 0, 360*64);
		if ((!strcmp (n, "Apollo 12") && !option[FLIPLR_OPT]) ||
			    (!strcmp (n, "Apollo 14") && option[FLIPLR_OPT])) {
		    XID fid = XGContextFromGC (m_agc);
		    int dir, asc, des;
		    XCharStruct ap;

		    XQueryTextExtents (XtD, fid, n, l, &dir, &asc, &des, &ap);
		    XDrawString (dsp, win, m_agc, x-ap.width-4, y-4, n, l);
		} else
		    XDrawString (dsp, win, m_agc, x+4, y-4, n, l);
	    }
	}
}

/* label the database entries marked for name display */
static void
m_labels()
{
	MoonDB *mp;

	for (mp = moondb; mp < &moondb[nmoondb]; mp++)
	    if (m_wantlabel(mp))
		XDrawString (XtD, m_pm, m_agc, FX(mp->x)+BORD, FY(mp->y)+BORD,
	    					mp->name, strlen(mp->name));
}

/* add background sky objects to m_pm and to skyobjs[] list */
static void
m_sky()
{
	Now *np = mm_get_now();
	double scale;		/* image scale, rads per pixel */
	double mra, mdec;	/* moon's geocentric ra and dec */
	double mtra, mtdec;	/* moon's topocentric ra and dec */
	double cmtdec, smtdec;	/*   "  sin/cos */
	double mtlg, mtlt, cmtlt;/* moon's topocentric ecliptic long/lat */
	double maxr;		/* max dst from center we want to draw, rads */
	double mrad, cmrad;	/* moon radius, rads */
	double lst;
	double alt;
	DBScan dbs;
	Obj *moonop;
	Obj *op;

	/* need lst now to convert HA to RA */
	now_lst (np, &lst);

	/* get current moon info and derive scale, etc */
	moonop = db_basic (MOON);
	db_update (moonop);
	mrad = degrad(moonop->s_size/3600.0)/2.0;
	cmrad = cos(mrad);
	scale = mrad/moonrad;
	mra = moonop->s_ra;
	mdec = moonop->s_dec;
	maxr = ((mnrows > mncols ? mnrows : mncols)/2 + BORD)*scale;
	unrefract (pressure, temp, moonop->s_alt, &alt);
	aa_hadec (lat, alt, moonop->s_az, &mtra, &mtdec);
	cmtdec = cos(mtdec);
	smtdec = sin(mtdec);
	mtra = hrrad(lst) - mtra;
	range (&mtra, 2*PI);
	eq_ecl (mjd, mtra, mtdec, &mtlt, &mtlg);
	cmtlt = cos(mtlt);

	/* scan the database and draw whatever is near */
	for (db_scaninit(&dbs); (op = db_scan (&dbs)) != NULL; ) {
	    double dra, ddec;
	    double ra, dec;
	    double lt, lg;
	    int dx, dy;
	    double csep;
	    int x, y;
	    int diam;
	    GC gc;

	    if (op->type == UNDEFOBJ)
		continue;

	    if (is_planet (op, MOON)) {
		/* we draw it better here */
		continue;
	    }

	    db_update (op);

	    /* find size, in pixels. */
	    diam = magdiam (FMAG, scale, op->s_mag/MAGSCALE,
					    degrad((double)op->s_size/3600.0));
	    /* reject if too faint */
	    if (diam <= 0)
		continue;

	    /* find topocentric ra and dec */
	    unrefract (pressure, temp, op->s_alt, &alt);
	    aa_hadec (lat, alt, op->s_az, &ra, &dec);
	    ra = hrrad(lst) - ra;
	    range (&ra, 2*PI);

	    /* reject if it's obviously outside field of view */
	    dra = mtra - ra;
	    ddec = mtdec - dec;
	    if (fabs(ddec) > maxr || delra(dra)*cmtdec > maxr)
		continue;

	    /* or behind the surface */
	    solve_sphere (mtra-ra, PI/2-dec, smtdec, cmtdec, &csep, NULL); 
	    if (csep > cmrad)
		continue;

	    /* find [x,y] relative to image center.
	     * the idea is to use ecliptic coords since that is within a
	     * few degrees of the moons shadow.
	     */
	    eq_ecl (mjd, ra, dec, &lt, &lg);
	    dx = (int)floor(((mtlg - lg)*cmtlt)/scale + 0.5);	/* + right */
	    dy = (int)floor((mtlt - lt)/scale + 0.5);		/* + down */

	    /* allow for flipping and shift to find window coords */
	    x = FX(dx+mncols/2) + BORD;
	    y = FY(dy+mnrows/2) + BORD;

	    /* pick a gc */
	    obj_pickgc(op, toplevel_w, &gc);

	    /* draw 'er */
	    sv_draw_obj (XtD, m_pm, gc, op, x, y, diam, 0);

	    /* add to skyobjs[] list */
	    addSkyObj (op, x, y);
	}

        sv_draw_obj (XtD, m_pm, (GC)0, NULL, 0, 0, 0, 0);        /* flush */
}

/* given a MoonDB entry, return 1 if we want to show its label now, else 0. */
static int
m_wantlabel (mp)
MoonDB *mp;
{
	double lt, lg;
	int ok;

	/* don't show any if just too small */
	if (mshrink > MAXSHRINK/2)
	    return (0);

	/* only show certain types */
	ok = !strcmp (mp->type, "Crater") || !strcmp (mp->type, "Mountain")
		|| !strcmp (mp->type, "Ocean") || !strncmp (mp->type, "Sea", 3);
	if (!ok)
	    return (0);

	/* don't show any near the limb */
	if (xy2ll (mp->x, mp->y, &lt, &lg) < 0)
	    return (0);
	ok = lt < degrad(70) && lt > degrad(-70) &&
					(lg < degrad(70) || lg > degrad(290));
	if (!ok)
	    return (0);

	/* always show these */
	ok = !strcmp(mp->name, "Tycho") || !strcmp (mp->name, "Copernicus") ||
	    !strcmp(mp->name, "Plato") || !strcmp (mp->name, "Aristarchus");

	/* show only features large enough for this scale */
	return (ok || mp->sz > MINKM*mshrink*mshrink);
}

/* draw the N/S E/W labels on the four edges of the m_pm */
static void
m_orientation()
{
	static int fw, fa, fd;

	if (fw == 0) {
	    XID fid = XGContextFromGC (m_agc);
	    int dir, asc, des;
	    XCharStruct xcs;

	    XQueryTextExtents (XtD, fid, "W", 1, &dir, &asc, &des, &xcs);
	    fw = xcs.width;
	    fa = xcs.ascent;
	    fd = xcs.descent;
	}

	XDrawString (XtD, m_pm, m_agc, BORD+(mncols-fw)/2, BORD-fd-LGAP,
					    option[FLIPTB_OPT] ? "S" : "N", 1);
	XDrawString (XtD, m_pm, m_agc, BORD+(mncols-fw)/2, BORD+mnrows+fa+LGAP,
					    option[FLIPTB_OPT] ? "N" : "S", 1);
	XDrawString (XtD, m_pm, m_agc, BORD-fw-LGAP, BORD+(mnrows+fa)/2,
					    option[FLIPLR_OPT] ? "E" : "W", 1);
	XDrawString (XtD, m_pm, m_agc, BORD+mncols+LGAP, BORD+(mnrows+fa)/2,
					    option[FLIPLR_OPT] ? "W" : "E", 1);
}

/* draw a coordinate grid over the image moon on m_pm */
static void
m_grid()
{
#define	GSP	degrad(15.0)	/* grid spacing */
#define	FSP	(GSP/4.)	/* fine spacing */
	Display *dsp = XtDisplay (mda_w);
	Window win = m_pm;
	double lt, lg;
	int x, y;

	/* lines of equal lat */
	for (lt = PI/2 - GSP; lt >= -PI/2 + GSP; lt -= GSP) {
	    XPoint xpt[(int)(PI/FSP)+1];
	    int npts = 0;

	    for (lg = -PI/2; lg <= PI/2; lg += FSP) {
		if (ll2xy(lt, lg, &x, &y) < 0)
		    continue;
		if (npts >= XtNumber(xpt)) {
		    printf ("Moon lat grid overflow\n");
		    exit (1);
		}
		xpt[npts].x = FX(x) + BORD;
		xpt[npts].y = FY(y) + BORD;
		npts++;
	    }
	    XDrawLines (dsp, win, m_agc, xpt, npts, CoordModeOrigin);
	}

	/* lines of equal longitude */
	for (lg = -PI/2; lg <= PI/2; lg += GSP) {
	    XPoint xpt[(int)(PI/FSP)+1];
	    int npts = 0;

	    for (lt = -PI/2; lt <= PI/2; lt += FSP) {
		if (ll2xy(lt, lg, &x, &y) < 0)
		    continue;
		if (npts >= XtNumber(xpt)) {
		    printf ("Moon lng grid overflow\n");
		    exit (1);
		}
		xpt[npts].x = FX(x) + BORD;
		xpt[npts].y = FY(y) + BORD;
		npts++;
	    }
	    XDrawLines (dsp, win, m_agc, xpt, npts, CoordModeOrigin);
	}

}

/* create m_xim of size mnrowsXmncols, depth mdepth and bit-per-pixel mbpp.
 * make a Bitmap if only have 1 bit per pixel, otherwise a Pixmap.
 * return 0 if ok else -1 and xe_msg().
 */
static int
mxim_create ()
{
	Display *dsp = XtDisplay (mda_w);
	int nbytes = mnrows*mncols*mbpp/8;
	char *data;

	/* get memory for image pixels.  */
	data = (char *) malloc (nbytes);
	if (!data) {
	    char msg[1024];
	    (void)sprintf(msg,"Can not get %d bytes for shadow pixels", nbytes);
	    xe_msg (msg, 1);
	    return (-1);
	}

	/* create the XImage */
	m_xim = XCreateImage (dsp, XDefaultVisual (dsp, 0),
	    /* depth */         mbpp == 1 ? 1 : mdepth,
	    /* format */        mbpp == 1 ? XYBitmap : ZPixmap,
	    /* offset */        0,
	    /* data */          data,
	    /* width */         mncols,
	    /* height */        mnrows,
	    /* pad */           mbpp < 8 ? 8 : mbpp,
	    /* bpl */           0);
	if (!m_xim) {
	    xe_msg ("Can not create shadow XImage", 1);
	    free ((void *)data);
	    return (-1);
	}

        m_xim->bitmap_bit_order = LSBFirst;
	m_xim->byte_order = LSBFirst;

	/* ok */
	return (0);
}

/* fill in m_xim from mimage and knowledge of the sunrise longitude.
 * get the earthshine from eshine_w scale widget.
 */
static void
mxim_setup (srlng)
double srlng;	/* sunrise longitude (0..360 +E), corrctd for image viewpoint */
{
	int right = cos(srlng) < 0;	/* whether shadw reaches to right limb*/
	double sinsrl = sin(srlng);
	unsigned char *mp;
	int x, y;
	int tmp;
	int esf;

	get_something (eshine_w, XmNvalue, (XtArgVal)&esf);

	/* copy intensities through mgray[] map to get pixels.
	 * N.B. intensity 0 is widget background, not necessarily black.
	 */
	for (y = 0; y < mnrows; y++) {
	    int lx, rx;		/* left and right edge of scan line to darken */
	    int yc;		/* y with respect to moon center */
	    int fy;		/* (possibly) flipped y */

	    yc = y - (topmarg + moonrad);

	    /* find left and right edge of shadow */
	    if (abs(yc) < moonrad) {
		int r;		/* pixels to limb at this y */
		r = sqrt((double)(moonrad*moonrad - yc*yc)) + 1; /* round up */

		/* compute shadow edges with respect to center */
		if (right) {
		    rx = r;
		    lx = -r * sinsrl;
		} else {
		    lx = -r;
		    rx = r * sinsrl;
		}

		/* convert to X coords */
		lx += leftmarg + moonrad;
		rx += leftmarg + moonrad;
	    } else {
		/* above or below moon image so no shadow */
		lx = -1;
		rx = mncols;
	    }

	    /* allow for flipping */
	    if (option[FLIPLR_OPT]) {
		tmp = lx;
		lx = FX(rx);
		rx = FX(tmp);
	    }
	    fy = FY(y);

	    /* scan across the whole row, drawing shadow between lx and rx */
	    mp = &mimage[fy*mncols];
	    for (x = 0; x < mncols; x++) {
		unsigned char i = *mp++;
		Pixel p;

		if (i == 0)
		    p = mbg;
		else if (x >= lx && x <= rx)
		    p = mgray[i*nmgray*esf/ESHINEF/256]; /* shadow */
		else
		    p = mgray[i*nmgray/256];

		XPutPixel (m_xim, x, fy, p);
	    }
	}
}

/* do everything necessary to see a fresh image, shrunk by factor f:
 * read full moon file; shrink by a factor of f; replace mimage with result;
 * resize mda_w and m_xim; reread database (so [x,y] values match new scale).
 * when done, set mimage, m_xim, mncols, mnrows, topmarg, leftmarg and moonrad.
 * return 0 if all ok, else xe_msg() and return -1.
 */
static int
m_shrink (f)
int f;
{
	char msg[1024];
	unsigned char *im;
	int imnr, imnc;
	unsigned char *newim;
	int newnr, newnc;
	char *fn;

	/* read moon file into im[] */
	fn = XGetDefault (XtD, myclass, mfxr);
	if (!fn) {
	    (void) sprintf (msg, "No %s resource so can't find moon image.",
								    mfxr);
	    xe_msg (msg, 1);
	    return (-1);
	}
	if (fits_read (fn, &im, &imnr, &imnc) < 0) 
	    return(-1);
	if (imnr != MNROWS || imnc != MNCOLS) {
	    (void) sprintf (msg, "%s: Expected %d x %d but found %d x %d",
				fn, MNROWS, MNCOLS, imnr, imnc);
	    xe_msg (msg, 1);
	    free ((void *)im);
	    return (-1);
	}

	/* set newim to resized version of im (it _is_ im if f is just 1) */
	if (f != 1) {
	    unsigned char *newmem;

	    newnr = imnr / f;
	    newnc = imnc / f;

	    /* get memory for resized copy */
	    newmem = (unsigned char *) malloc (newnr * newnc);
	    if (!newmem) {
		(void)sprintf (msg,"No mem for 1/%dX %d x %d -> %d x %d resize",
						f, imnr, imnc, newnr, newnc);
		xe_msg (msg, 1);
		free ((void *)im);
		return (-1);
	    }

	    m_resize (im, MNROWS, MNCOLS, f, newmem);
	    newim = newmem;
	    free ((void *)im);
	} else {
	    newim = im;
	    newnr = MNROWS;
	    newnc = MNCOLS;
	}

	/* commit newim to mimage -- set global metrics */
	if (mimage) {
	    free ((void *)mimage);
	    mimage = NULL;
	}
	mimage = newim;
	mnrows = newnr;
	mncols = newnc;
	topmarg = (double)TOPMARG/f + 0.5;
	leftmarg = (double)LEFTMARG/f + 0.5;
	moonrad = (double)MOONRAD/f + 0.5;

	/* dither mimage if we only have 2 colors to work with */
	if (mbpp == 1) 
	    mBWdither();

	/* flip mimage, as desired */
	if (option[FLIPTB_OPT])
	    fliptb();
	if (option[FLIPLR_OPT])
	    fliplr();

	/* (re)create the shadow image */
	if (m_xim) {
	    free ((void *)m_xim->data);
	    m_xim->data = NULL;
	    XDestroyImage (m_xim);
	    m_xim = NULL;
	}
	if (mxim_create() < 0) {
	    free ((void *)mimage);
	    mimage = NULL;
	    return (-1);
	}

	/* (re)read moon database */
	m_readdb();

	/* delete pixmap so new one will get built on next expose */
	if (m_pm) {
	    XFreePixmap (XtD, m_pm);
	    m_pm = 0;
	}

	/* size the drawing area to hold the new image plus a border.
	 * this will give us an expose event to show the new image.
	 */
	m_sizewidgets ();

	return(0);
}

/* set size of mda_w and msw_w as desired based on mnrows and mncols */
static void
m_sizewidgets ()
{
	static int before;
	int neww = mncols + 2*BORD;
	int newh = mnrows + 2*BORD;

	/* mda_w should be image size + BORD all around */
	set_something (mda_w, XmNwidth, (XtArgVal)neww);
	set_something (mda_w, XmNheight, (XtArgVal)newh);

	/* if first time set the SW too */
	if (!before) {
	    /* allow a little for the scollbars */
	    set_something (msw_w, XmNwidth, (XtArgVal)(neww+30));
	    set_something (msw_w, XmNheight, (XtArgVal)(newh+30));
	    before = 1;
	}
}

/* copy image in, of size nr x nr, to out by shrinking a factor f.
 * N.B. we assume out is (nr/f) * (nr/f) bytes.
 */
static void
m_resize(in, nr, nc, f, out)
unsigned char *in, *out;
int nr, nc;
int f;
{
	int outx, outy;
	int noutx, nouty;

	noutx = nc/f;
	nouty = nr/f;
	for (outy = 0; outy < nouty; outy++) {
	    unsigned char *inrp = &in[outy*f*nr];
	    for (outx = 0; outx < noutx; outx++) {
		*out++ = *inrp;
		inrp += f;
	    }
	}
}

/* dither mimage into a 2-intensity image: 1 and 255.
 * don't use 0 for black in case it's different from darkest gray.
 * form 2x2 tiles whose pattern depends on intensity peak and spacial layout.
 */
static void
mBWdither()
{
	int idx[4];
	int y;

	idx[0] = 0;
	idx[1] = 1;
	idx[2] = mncols;
	idx[3] = mncols+1;

	for (y = 0; y < mnrows; y += 2) {
	    unsigned char *mp = &mimage[y*mncols];
	    unsigned char *lp;

	    for (lp = mp + mncols; mp < lp; mp += 2) {
		int sum, numon;
		int i;

		sum = 0;
		for (i = 0; i < 4; i++)
		    sum += (int)mp[idx[i]];
		numon = sum*5/1021;	/* 1021 is 255*4 + 1 */

		for (i = 0; i < 4; i++)
		    mp[idx[i]] = 1;

		switch (numon) {
		case 0:
		    break;
		case 1:
		case 2:
		    mp[idx[0]] = 255;
		    break;
		case 3:
		    mp[idx[0]] = 255;
		    mp[idx[1]] = 255;
		    mp[idx[3]] = 255;
		    break;
		case 4:
		    mp[idx[0]] = 255;
		    mp[idx[1]] = 255;
		    mp[idx[2]] = 255;
		    mp[idx[3]] = 255;
		    break;
		default:
		    printf ("Bad numon: %d\n", numon);
		    exit(1);
		}
	    }
	}
}

/* draw some random stars onto m_pm */
static void
m_stars ()
{
#define	SQR(x) ((x)*(x))
	XPoint stars[NSTARS + NBSTARS + 2*NVBSTARS];
	int w, h;
	Pixmap pm;
	int xc, yc, rsqr;
	int ns;
	int i;

	w = mncols + 2*BORD;
	h = mnrows + 2*BORD;
	xc = BORD + leftmarg + moonrad;
	yc = BORD + topmarg + moonrad;
	rsqr = SQR(moonrad);
	pm = m_pm;

	/* make the stars */
	ns = 0;
	for (i = 0; i < NSTARS; i++) {
	    int x, y;

	    x = ((rand() >> 2) & 0xfff) * (w-1) / 0xfff;
	    y = ((rand() >> 2) & 0xfff) * (h-1) / 0xfff;

	    if (SQR(x-xc) + SQR(y-yc) > rsqr) {
		stars[ns].x = x; stars[ns].y = y; ns++;

		if (i < NBSTARS) {
		    stars[ns].x = x+1; stars[ns].y = y;   ns++;
		}
		if (i < NVBSTARS) {
		    stars[ns].x = x;   stars[ns].y = y+1; ns++;
		    stars[ns].x = x+1; stars[ns].y = y+1; ns++;
		}
	    }
	}

        XDrawPoints (XtD, pm, m_fgc, stars, ns, CoordModeOrigin);
}

/* flip mimage left/right */
static void
fliplr()
{
	int x, y;

	for (y = 0; y < mnrows; y++) {
	    unsigned char *rp = &mimage[y*mncols];
	    for (x = 0; x < mncols/2; x++) {
		unsigned char tmp = rp[x];
		rp[x] = rp[mncols-x-1];
		rp[mncols-x-1] = tmp;
	    }
	}
}

/* flip mimage top/bottom.
 * N.B. will flip back option if can't do it for some reason.
 */
static void
fliptb()
{
	char buf[2048];		/* plenty :-) */
	int y;

	if (mncols > sizeof(buf)) {
	    (void) sprintf (buf, "Can not flip -- rows are longer than %d",
	    							sizeof(buf));
	    xe_msg (buf, 1);
	    XmToggleButtonSetState (option_w[FLIPTB_OPT],
						option[FLIPTB_OPT] ^= 1, False);
	    return;
	}
	    

	for (y = 0; y < mnrows/2; y++) {
	    unsigned char *r0 = &mimage[y*mncols];
	    unsigned char *r1 = &mimage[(mnrows-y-1)*mncols];

	    (void) memcpy (buf, r0, mncols);
	    (void) memcpy (r0, r1, mncols);
	    (void) memcpy (r1, buf, mncols);
	}
}

/* draw a marker on m_pm to show the limb angle favored by libration */
static void
m_mark_libr (limb)
double limb;		/* limb angle, rads ccw from up */
{
	int r;		/* radius of marker */
	int x, y;	/* center of marker */

	r = moonrad/LMFRAC;
	x = leftmarg + moonrad*(1 - sin(limb));
	y = leftmarg + moonrad*(1 - cos(limb));

	x = FX(x);
	y = FY(y);

	XFillArc (XtD, m_pm, m_agc, BORD+x-r, BORD+y-r, 2*r, 2*r, 0, 360*64);
}

/* report the location of x,y, which are with respect to mda_w.
 * N.B. allow for flipping and the borders.
 */
static void
m_reportloc (dsp, win, x, y)
Display *dsp;
Window win;
int x, y;
{
	double lt, lg;

	/* convert from mda_w coords to m_xim coords */
	x -= BORD;
	y -= BORD;

	if (xy2ll (FX(x), FY(y), &lt, &lg) == 0) {
	    Now *np = mm_get_now();
	    double a;

	    f_dm_angle (lat_w, lt);
	    f_dm_angle (lng_w, lg);

	    moon_colong (mjd+MJD0, lt, lg, NULL, NULL, &a);
	    f_dm_angle (sunalt_w, a);
	} else {
	    set_xmstring (lat_w, XmNlabelString, " ");
	    set_xmstring (lng_w, XmNlabelString, " ");
	    set_xmstring (sunalt_w, XmNlabelString, " ");
	}
}

/* make glass_xim of size GLASSSZ*GLASSMAG and same genre as m_xim.
 * leave glass_xim NULL if trouble.
 */
static void
makeGlassImage (dsp)
Display *dsp;
{
	int nbytes = GLASSSZ*GLASSMAG * GLASSSZ*GLASSMAG * mbpp/8;
	char *glasspix = (char *) malloc (nbytes);

	if (!glasspix) {
	    char msg[1024];
	    (void) sprintf (msg, "Can not malloc %d for Glass pixels", nbytes);
	    xe_msg (msg, 0);
	    return;
	}

	glass_xim = XCreateImage (dsp, XDefaultVisual (dsp, 0),
	    /* depth */         m_xim->depth,
	    /* format */        m_xim->format,
	    /* offset */        0,
	    /* data */          glasspix,
	    /* width */         GLASSSZ*GLASSMAG,
	    /* height */        GLASSSZ*GLASSMAG,
	    /* pad */           mbpp < 8 ? 8 : mbpp,
	    /* bpl */           0);

	if (!glass_xim) {
	    free ((void *)glasspix);
	    xe_msg ("Can not make Glass XImage", 0);
	    return;
	}

        glass_xim->bitmap_bit_order = LSBFirst;
	glass_xim->byte_order = LSBFirst;
}

/* make glassGC */
static void
makeGlassGC (dsp, win)
Display *dsp;
Window win;
{
	XGCValues gcv;
	unsigned int gcm;
	Pixel p;

	if (get_color_resource (mda_w, "GlassBorderColor", &p) < 0) {
	    xe_msg ("Can not get GlassBorderColor -- using White", 0);
	    p = WhitePixel (dsp, 0);
	}
	gcm = GCForeground;
	gcv.foreground = p;
	glassGC = XCreateGC (dsp, win, gcm, &gcv);
}

/* handle the operation of the magnifying glass.
 * this is called whenever there is left button activity over the image.
 */
static void
doGlass (dsp, win, b1p, m1, b1r, wx, wy, wid, hei)
Display *dsp;
Window win;
int b1p, m1, b1r;	/* button/motion state */
int wx, wy;		/* window coords of cursor */
unsigned wid, hei;	/* window size */
{
	static int lastwx, lastwy;
	int rx, ry, rw, rh;		/* region */

	/* check for first-time stuff */
	if (!glass_xim)
	    makeGlassImage (dsp);
	if (!glass_xim)
	    return; /* oh well */
	if (!glassGC)
	    makeGlassGC (dsp, win);

	if (m1) {

	    /* motion: put back old pixels that won't just be covered again */

	    /* first the vertical strip that is uncovered */

	    rh = GLASSSZ*GLASSMAG;
	    ry = lastwy - (GLASSSZ*GLASSMAG/2);
	    if (ry < 0) {
		rh += ry;
		ry = 0;
	    }
	    if (wx < lastwx) {
		rw = lastwx - wx;	/* cursor moved left */
		rx = wx + (GLASSSZ*GLASSMAG/2);
	    } else {
		rw = wx - lastwx;	/* cursor moved right */
		rx = lastwx - (GLASSSZ*GLASSMAG/2);
	    }
	    if (rx < 0) {
		rw += rx;
		rx = 0;
	    }

	    if (rw > 0 && rh > 0)
		XCopyArea (dsp, m_pm, win, m_fgc, rx, ry, rw, rh, rx, ry);

	    /* then the horizontal strip that is uncovered */

	    rw = GLASSSZ*GLASSMAG;
	    rx = lastwx - (GLASSSZ*GLASSMAG/2);
	    if (rx < 0) {
		rw += rx;
		rx = 0;
	    }
	    if (wy < lastwy) {
		rh = lastwy - wy;	/* cursor moved up */
		ry = wy + (GLASSSZ*GLASSMAG/2);
	    } else {
		rh = wy - lastwy;	/* cursor moved down */
		ry = lastwy - (GLASSSZ*GLASSMAG/2);
	    }
	    if (ry < 0) {
		rh += ry;
		ry = 0;
	    }

	    if (rw > 0 && rh > 0)
		XCopyArea (dsp, m_pm, win, m_fgc, rx, ry, rw, rh, rx, ry);
	}

	if (b1p || m1) {

	    /* start or new location: show glass and save new location */

	    fillGlass (wx-BORD, wy-BORD);
	    XPutImage (dsp, win, m_fgc, glass_xim, 0, 0,
			wx-(GLASSSZ*GLASSMAG/2), wy-(GLASSSZ*GLASSMAG/2),
			GLASSSZ*GLASSMAG, GLASSSZ*GLASSMAG);
	    lastwx = wx;
	    lastwy = wy;

	    /* kinda hard to tell boundry of glass so draw a line around it */
	    XDrawRectangle (dsp, win, glassGC,
			wx-(GLASSSZ*GLASSMAG/2), wy-(GLASSSZ*GLASSMAG/2),
			GLASSSZ*GLASSMAG-1, GLASSSZ*GLASSMAG-1);
	}

	if (b1r) {

	    /* end: restore all old pixels */

	    rx = lastwx - (GLASSSZ*GLASSMAG/2);
	    rw = GLASSSZ*GLASSMAG;
	    if (rx < 0) {
		rw += rx;
		rx = 0;
	    }

	    ry = lastwy - (GLASSSZ*GLASSMAG/2);
	    rh = GLASSSZ*GLASSMAG;
	    if (ry < 0) {
		rh += ry;
		ry = 0;
	    }

	    if (rw > 0 && rh > 0)
		XCopyArea (dsp, m_pm, win, m_fgc, rx, ry, rw, rh, rx, ry);
	}
}

/* fill glass_xim with GLASSSZ*GLASSMAG view of m_xim centered at coords
 * xc,yc. take care at the edges (m_xim is mnrows x mncols)
 */
static void
fillGlass (xc, yc)
int xc, yc;
{
	int sx, sy;	/* coords in m_xim */
	int gx, gy;	/* coords in glass_xim */
	int i, j;

	gy = 0;
	gx = 0;
	for (sy = yc-GLASSSZ/2; sy < yc+GLASSSZ/2; sy++) {
	    for (sx = xc-GLASSSZ/2; sx < xc+GLASSSZ/2; sx++) {
		Pixel p;

		if (sx < 0 || sx >= mncols || sy < 0 || sy >= mnrows)
		    p = XGetPixel (m_xim, 0, 0);
		else
		    p = XGetPixel (m_xim, sx, sy);
		for (i = 0; i < GLASSMAG; i++)
		    for (j = 0; j < GLASSMAG; j++)
			XPutPixel (glass_xim, gx+i, gy+j, p);
		gx += GLASSMAG;
	    }
	    gx = 0;
	    gy += GLASSMAG;
	}
}

/* read Moon database: fill up moodb[] and set nmoondb.
 * N.B. to fill in x,y we assume the lat/long match the image orientation in
 *   its original form, ie, up N right E.
 * if fail, leave moondb NULL.
 */
static void
m_readdb()
{
#define	NDBCHUNKS	32	/* malloc room for these many more each time */
	char msg[1024];
	char buf[1024];
	int ndb;
	FILE *fp;
	char *fn;

	/* open file */
	fn = XGetDefault (XtD, myclass, mdbxr);
	if (!fn) {
	    (void) sprintf (msg, "No resource: %s", mdbxr);
	    xe_msg (msg, 1);
	    return;
	}
	fp = fopenh (fn, "r");
	if (!fp) {
	    (void) sprintf (msg, "%s: %s", fn , syserrstr());
	    xe_msg (msg, 1);
	    return;
	}

	/* add initial chunk to moondb[] */
	if (moondb) {
	    free ((void *)moondb);
	    nmoondb = 0;
	}
	moondb = (MoonDB *) malloc (NDBCHUNKS * sizeof(MoonDB));
	if (!moondb) {
	    xe_msg ("No memory for moon database", 1);
	    (void) fclose (fp);
	    return;
	}
	ndb = NDBCHUNKS;

	watch_cursor (1);

	/* read and add each line to list */
	while (fgets (buf, sizeof(buf), fp)) {
	    MoonDB *mp;
	    char name[128];
	    char type[128];
	    double lt, lg;
	    int x, y;
	    char *new;
	    int km;
	    int n;
	    int i;

	    /* crack the line -- just skip it if not recognized */
	    n = sscanf(buf,"%[^|]|%[^|]|%lf|%lf|%d", name, type, &lt, &lg, &km);
	    if (n != 5)
		continue;

	    /* grow moondb if now filled */
	    if (nmoondb == ndb) {
		new = realloc ((void *)moondb, (ndb+NDBCHUNKS)*sizeof(MoonDB));
		if (!new) {
		    xe_msg ("Short of moon DB memory", 1);
		    break;	/* go with what we have now */
		}

		ndb += NDBCHUNKS;
		moondb = (MoonDB *)new;
	    }
	    mp = &moondb[nmoondb];

	    /* build new entry at mp */

	    /* location */
	    lt = degrad(lt);
	    lg = degrad(lg);
	    if (ll2xy (lt, lg, &x, &y) < 0)
		continue;	/* off the image */
	    mp->x = x;
	    mp->y = y;

	    /* copy name */
	    new = malloc (strlen (name) + 1);
	    if (!new) {
		xe_msg ("Short of 'name' memory for moon DB", 1);
		break;	/* go with what we have */
	    }
	    mp->name = strcpy (new, name);

	    /* copy type, but try to reuse */
	    for (i = 0; i < nmoondb; i++)
		if (strcmp (moondb[i].type, type) == 0) {
		    mp->type = moondb[i].type;
		    break;	/* reuse type string */
		}
	    if (i == nmoondb) {
		/* first time we've seen this type */
		new = malloc (strlen (type) + 1);
		if (!new) {
		    xe_msg ("Short of 'type' memory for moon DB", 1);
		    free ((void *)mp->name);
		    break;	/* go with what we have */
		}
		mp->type = strcpy (new, type);
	    }

	    /* radius as seen overhead, km */
	    mp->sz = km;

	    /* all ok */
	    nmoondb++;
	}

	/* finished with file */
	(void) fclose (fp);

	/* warn if nothing found */
	if (nmoondb == 0) {
	    (void) sprintf (msg, "%s: No moon database entries found", fn);
	    xe_msg (msg, 0);
	}

	/* ok */
	watch_cursor(0);
}

/* free the list of sky objects, if any */
static void
resetSkyObj ()
{
	if (skyobjs) {
	    free ((void *) skyobjs);
	    skyobjs = NULL;
	}
	nskyobjs = 0;
}

/* add op at [x,y] to the list of background sky objects */
static void
addSkyObj (op, x, y)
Obj *op;
int x, y;
{
	char *newmem;

	if (skyobjs)
	    newmem = realloc ((void *)skyobjs, (nskyobjs+1) * sizeof(SkyObj));
	else
	    newmem = malloc (sizeof(SkyObj));
	if (!newmem) {
	    xe_msg ("No mem for more sky objects", 1);
	    return;
	}

	skyobjs = (SkyObj *) newmem;
	skyobjs[nskyobjs].o = *op;
	skyobjs[nskyobjs].x = x;
	skyobjs[nskyobjs].y = y;
	nskyobjs++;
}

/* find the object in skyobjs[] that is closest to [x,y].
 * return NULL if none within MAXR.
 */
static Obj *
closeSkyObj (x, y)
int x, y;
{
	SkyObj *closesp = NULL;
	int mind = 0;
	int i;

	for (i = 0; i < nskyobjs; i++) {
	    SkyObj *sp = &skyobjs[i];
	    int d = abs(sp->x - x) + abs(sp->y - y);

	    if (!closesp || d < mind) {
		mind = d;
		closesp = sp;
	    }
	}

	if (closesp && mind <= MAXR)
	    return (&closesp->o);
	return (NULL);
}

/* called when hit button 3 over image */
static void
m_popup (ep)
XEvent *ep;
{
	XButtonEvent *bep;
	int x, y;

	bep = &ep->xbutton;
	x = bep->x;
	y = bep->y;

	if (overMoon (x, y)) {
	    MoonDB *mp = closeMoonDB (x-BORD, y-BORD);
	    fill_popup (mp, x-BORD, y-BORD);
	    XmMenuPosition (pu_w, (XButtonPressedEvent *)ep);
	    XtManageChild (pu_w);
	} else {
	    Obj *op = closeSkyObj (x, y);
	    if (op) {
		fill_skypopup (op);
		XmMenuPosition (skypu_w, (XButtonPressedEvent *)ep);
		XtManageChild (skypu_w);
	    }
	}
}

/* create the popup menu */
static void
m_create_popup()
{
	Arg args[20];
	int n;

	n = 0;
	XtSetArg (args[n], XmNisAligned, True); n++;
	XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
	pu_w = XmCreatePopupMenu (mda_w, "MPU", args, n);

	n = 0;
	pu_name_w = XmCreateLabel (pu_w, "MPN", args, n);
	XtManageChild (pu_name_w);

	n = 0;
	pu_type_w = XmCreateLabel (pu_w, "MPT", args, n);
	XtManageChild (pu_type_w);

	n = 0;
	pu_size_w = XmCreateLabel (pu_w, "MPS", args, n);
	XtManageChild (pu_size_w);

	n = 0;
	pu_lat_w = XmCreateLabel (pu_w, "MPLat", args, n);
	XtManageChild (pu_lat_w);

	n = 0;
	pu_lng_w = XmCreateLabel (pu_w, "MPLng", args, n);
	XtManageChild (pu_lng_w);

	n = 0;
	pu_alt_w = XmCreateLabel (pu_w, "MPAlt", args, n);
	XtManageChild (pu_alt_w);
}

/* fill in popup widgets from mp. if mp == NULL use x,y and skip name.
 * x and y are in image coords, (as in m_xim), not mda_w.
 * N.B. since we assume the database matches the original image orientation
 *   there is no need to correct mp->{x,y} for flipping.
 */
static void
fill_popup (mp, x, y)
MoonDB *mp;
int x, y;
{
	Now *np = mm_get_now();
	double lt, lg;
	double a;
	char str[64];

	if (mp) {
	    XtManageChild (pu_name_w);
	    f_showit (pu_name_w, mp->name);
	    XtManageChild (pu_type_w);
	    f_showit (pu_type_w, mp->type);
	    XtManageChild (pu_size_w);
	    f_double (pu_size_w, "%.0f km", (double)mp->sz);
	    xy2ll (mp->x, mp->y, &lt, &lg);
	} else {
	    XtUnmanageChild (pu_name_w);
	    XtUnmanageChild (pu_type_w);
	    XtUnmanageChild (pu_size_w);
	    xy2ll (FX(x), FY(y), &lt, &lg);
	}

	(void) strcpy (str, "Lat: ");
	if (lt < 0) {
	    fs_dm_angle (str+5, -lt);
	    (void) strcat (str, " S");
	} else {
	    fs_dm_angle (str+5, lt);
	    (void) strcat (str, " N");
	}
	f_showit (pu_lat_w, str);

	(void) strcpy (str, "Long: ");
	if (lg > PI) {
	    fs_dm_angle (str+6, 2*PI-lg);
	    (void) strcat (str, " W");
	} else {
	    fs_dm_angle (str+6, lg);
	    (void) strcat (str, " E");
	}
	f_showit (pu_lng_w, str);

	moon_colong (mjd+MJD0, lt, lg, NULL, NULL, &a);
	(void) strcpy (str, "Sun alt: ");
	fs_dm_angle (str+9, a);
	f_showit (pu_alt_w, str);
}

/* find the smallest entry in moondb[] that [x,y] is within.
 * if find one return its *MoonDB else NULL.
 * x and y are in image coords (as in m_xim), not mda_w.
 * N.B. we allow for flipping.
 */
static MoonDB *
closeMoonDB (x, y)
int x, y;
#define	RSLOP	5
{
	MoonDB *mp, *smallp;
	double lt, lg;		/* location of [x,y] */
	double forsh;		/* foreshortening (cos of angle from center) */
	int minsz;
	double minr;

	/* allow for flipping */
	x = FX(x);
	y = FY(y);

	/* find forshortening */
	if (xy2ll (x, y, &lt, &lg) < 0)
	    return (NULL);
	solve_sphere (lg - CLNG, PI/2-CLAT, sin(lt), cos(lt), &forsh, NULL);

	watch_cursor(1);

	minsz = 100000;
	minr = 1e6;
	smallp = NULL;
	for (mp = moondb; mp < &moondb[nmoondb]; mp++) {
	    int dx, dy;
	    int sz = mp->sz*moonrad/MDIA; /* radius, in pixels */
	    double r;

	    dx = mp->x - x;
	    dy = mp->y - y;
	    r = sqrt((double)dx*dx + dy*dy);

	    /* it's a candidate if we are inside its (foreshortened) size
	     * or we are within RSLOP pixel of it.
	     */
	    if ((r <= sz*forsh && sz < minsz) || (r < RSLOP && r < minr)) {
		smallp = mp;
		minsz = sz;
		minr = r;
	    }
	}

	watch_cursor(0);

	return (smallp);
}

/* create the sky background object popup menu */
static void
m_create_skypopup()
{
	Arg args[20];
	int n;

	n = 0;
	XtSetArg (args[n], XmNisAligned, True); n++;
	XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
	skypu_w = XmCreatePopupMenu (mda_w, "MSKYPU", args, n);

	n = 0;
	skypu_name_w = XmCreateLabel (skypu_w, "MSKYPN", args, n);
	XtManageChild (skypu_name_w);
}

/* fill skypu_w with info from op */
static void
fill_skypopup (op)
Obj *op;
{
	f_showit (skypu_name_w, op->o_name);
}
