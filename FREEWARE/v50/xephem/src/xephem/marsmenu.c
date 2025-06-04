/* code to manage the stuff on the mars display.
 */

#include <stdio.h>
#include <math.h>
#include <ctype.h>

#if defined(__STDC__)
#include <stdlib.h>
#else
extern void *malloc(), *realloc();
#endif

#if defined(_POSIX_SOURCE)
#include <unistd.h>
#else
extern int close();
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
#include "preferences.h"
#include "fits.h"
#include "ps.h"


extern Widget toplevel_w;
extern XtAppContext xe_app;
#define XtD XtDisplay(toplevel_w)
extern Colormap xe_cm;


extern FILE *fopenh P_((char *name, char *how));
extern Now *mm_get_now P_((void));
extern Obj *db_basic P_((int id));
extern Obj *db_scan P_((DBScan *sp));
extern char *getShareDir P_((void));
extern char *mm_getsite P_((void));
extern double atod P_((char *buf));
extern double delra P_((double dra));
extern int alloc_ramp P_((Display *dsp, XColor *basep, Colormap cm,
    Pixel pix[], int maxn));
extern int any_ison P_((void));
extern int fs_fetch P_((Now *np, double ra, double dec, double fov,
    double mag, ObjF **opp));
extern int get_color_resource P_((Widget w, char *cname, Pixel *p));
extern int magdiam P_((int fmag, int magstp, double scale, double mag,
    double size));
extern int openh P_((char *name, int flags, ...));
extern void db_scaninit P_((DBScan *sp, int mask, ObjF *op, int nop));
extern void db_update P_((Obj *op));
extern void f_date P_((Widget w, double jd));
extern void f_double P_((Widget w, char *fmt, double f));
extern void f_mtime P_((Widget w, double t));
extern void f_pangle P_((Widget w, double a));
extern void f_showit P_((Widget w, char *s));
extern void f_dm_angle P_((Widget w, double a));
extern void f_sexa P_((Widget wid, double a, int w, int fracbase));
extern void fs_date P_((char out[], double jd));
extern void fs_dm_angle P_((char out[], double a));
extern void fs_manage P_((void));
extern void fs_time P_((char out[], double t));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void get_xmstring P_((Widget w, char *resource, char **txtp));
extern void get_views_font P_((Display *dsp, XFontStruct **fspp));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void marsm_manage P_((void));
extern void obj_pickgc P_((Obj *op, Widget w, GC *gcp));
extern void pm_set P_((int percentage));
extern void range P_((double *v, double r));
extern void register_selection P_((char *name));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void sv_draw_obj P_((Display *dsp, Drawable win, GC gc, Obj *op, int x,
    int y, int diam, int dotsonly));
extern char *syserrstr P_((void));
extern void timestamp P_((Now *np, Widget w));
extern void watch_cursor P_((int want));
extern void wtip P_((Widget w, char *tip));
extern void xe_msg P_((char *msg, int app_modal));
extern void zero_mem P_((void *loc, unsigned len));

#define	POLE_RA		degrad(317.61)
#define	POLE_DEC	degrad(52.85)

static void m_popup P_((XEvent *ep));
static void m_create_popup P_((void));

static void m_create_form P_((void));
static void m_create_msform P_((void));
static void m_set_buttons P_((int whether));
static void m_set_a_button P_((Widget pbw, int whether));
static void m_mstats_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_moons_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_selection_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_cml_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_slt_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_see_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_apply_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_aim_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_print_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_print P_((void));
static void m_imps P_((void));
static void m_ps_annotate P_((void));
static void m_unmap_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_close_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_init_gcs P_((void));
static void m_help_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_helpon_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_option_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_exp_cb P_((Widget w, XtPointer client, XtPointer call));
static void m_pointer_eh P_((Widget w, XtPointer client, XEvent *ev,
    Boolean *continue_to_dispatch));
static int m_fits P_((void));
static int xy2ll P_((int x, int y, double *ltp, double *lgp));
static int ll2xy P_((double l, double L, int *xp, int *yp));
static void m_redraw P_((void));
static void m_refresh P_((XExposeEvent *ep));
static void m_getsize P_((Drawable d, unsigned *wp, unsigned *hp,unsigned *dp));
static void m_stats P_((void));
static void m_draw P_((void));
static int mxim_create P_((void));
static void mxim_setup P_((void));
static void image_setup P_((void (*fp)(), void *arg));
static void mBWdither P_((void));
static void m_orientation P_((void));
static void m_sizecal P_((void));
static int m_labels P_((void));
static void m_grid P_((void));
static void m_reportloc P_((int x, int y));
static void m_eqproject P_((Now *np, double ra, double dec, int *xp, int *yp));
static void mars_cml P_((Now *np, double *cmlp, double *sltp, double *pap));

/* the FITS map image, read into mimage, is a 180x360 8bit FITS file of albedo
 * in 1 degree steps starting at longitude 0 on the left, latitude -PI/2 on the
 * top. use it to build a 360x360 view of the visible face in m_xim.
 */
#define	XEVERS		3	/* required version in header */
#define	IMSZ		360	/* n rows/cols in X image, n cols in FITS map */
#define	IMR	(IMSZ/2)	/* n rows in FITS map, handy radius */
#define	MAXCOLORS	40	/* max colors to use for image */
#define	BORD		80	/* extra drawing area border for labels/stars */
#define	LGAP		20	/* gap between NSEW labels and image edge */
#define	FMAG		16	/* faintest mag of sky background object */
#define	MAXR		10	/* max gap when picking sky objects, pixels */	
#define	GSP	degrad(15.0)	/* grid spacing */
#define	FSP	(GSP/5.)	/* fine spacing */
#define	XRAD		5	/* radius of central mark, pixels */
#define	SMPLSZ		2	/* pixel sample size */
#define	MAXOBJR 	50	/* clamp obj symbols to this max rad, pixels */

static FImage marsfits;		/* the Mars FITS images */
static unsigned char *mimage;	/* malloced array of raw mars image */
static Pixel mcolors[MAXCOLORS];/* color-scale ramp for drawing image */
static int nmcolors;		/* number of pixels usable in mcolors[] */
static Pixel mbg;		/* background color for image */
static int mdepth;		/* depth of image, in bits */
static int mbpp;		/* bits per pixel in image: 1, 8, 16 or 32 */
static XImage *m_xim;		/* XImage of mars now at current size */
static double m_cml;		/* current central meridian longitude, rads */
static double m_slt;		/* current subearth latitude, rads */
static double m_sslt, m_cslt;	/*   " handy sin/cos */
static double m_pa;		/* current N pole position angle, rads */
static double m_spa, m_cpa;	/*   " handy sin/cos */
static int m_seeing;		/* seeing, arc seconds */
static Obj *marsop;		/* current mars info */
static double cm_dec, sm_dec;	/* handy cos and sin of mars' dec */

/* main's widgets */
static Widget mform_w;		/* main mars form dialog */
static Widget mda_w;		/* image view DrawingArea */
static Pixmap m_pm;		/* image view staging pixmap */
static Widget dt_w;		/* main date/time stamp widget */

/* "More info" stats widgets */
static Widget msform_w;		/* statistics form dialog */
static Widget sdt_w;		/* statistics date/time stamp widget */
static Widget lat_w, lng_w;	/* lat/long under cursor */
static Widget cml_w;		/* central merdian longitude PB */
static Widget cmls_w;		/* central merdian longitude scale */
static Widget slt_w;		/* subearth latitude PB */
static Widget slts_w;		/* subearth latitude scale */
static Widget see_w;		/* seeing label */
static Widget sees_w;		/* seeing scale */
static Widget apply_w;		/* the Apply PB -- we fiddle with sensitive */
static int mswasman;            /* whether "More info" form was managed */
static int fakepos;		/* set when cml/slt/pa etc are not true */

/* sky background object's widgets */
static Widget skypu_w;		/* popup */
static Widget skypu_l_w;	/* popup lat label */
static Widget skypu_L_w;	/* popup Long label */
static Widget skypu_aim_w;	/* popup Point PB */
static double skypu_l;		/* latitude if Point PB is activated */
static double skypu_L;		/* longitude if Point PB is activated */

static GC m_fgc, m_bgc, m_agc;	/* various GCs */
static XFontStruct *m_fsp;	/* label font */

static int m_selecting;		/* set while our fields are being selected */

/* options list */
typedef enum {
    GRID_OPT, LABELS_OPT, FLIPLR_OPT, FLIPTB_OPT, COARSE_OPT,
    N_OPT
} Option;
static int option[N_OPT];
static Widget option_w[N_OPT];

/* Image to X Windows coord converter macros.
 * image coords have 0 in the center +x/right +y/down of the m_xim,
 * X Windows coords have upper left +x/right +y/down of the mda_w.
 */
#define	IX2XX(x)	(BORD + IMR + (option[FLIPLR_OPT] ? -(x) : (x)))
#define	IY2XY(y)	(BORD + IMR + (option[FLIPTB_OPT] ? -(y) : (y)))
#define	XX2IX(x)	(((x) - (BORD + IMR)) * (option[FLIPLR_OPT] ? -1 : 1))
#define	XY2IY(y)	(((y) - (BORD + IMR)) * (option[FLIPTB_OPT] ? -1 : 1))

/* called when the mars view is activated via the main menu pulldown.
 * if first time, build everything, else just toggle whether we are mapped.
 * allow for retrying to read the image file each time until find it.
 */
void
mars_manage ()
{
	if (!mform_w) {
	    /* one-time-only work */

	    /* build dialogs */
	    m_create_form();
	    m_create_msform();
	    m_create_popup();

	    /* establish depth, colors and bits per pixel */
	    get_something (mda_w, XmNdepth, (XtArgVal)&mdepth);
	    m_init_gcs();
	    mbpp = (mdepth == 1 || nmcolors == 2) ? 1 :
				    (mdepth>=17 ? 32 : (mdepth >= 9 ? 16 : 8));

	    /* establish initial mars circumstances */
	    m_stats();
	}

	/* make sure we can find the FITS file. */
	if (!mimage)
	    if (m_fits() < 0)
		return;

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

/* commanded from main to update with a new set of circumstances */
void
mars_update (np, how_much)
Now *np;
int how_much;
{
	if (!mform_w)
	    return;
	if (!XtIsManaged(mform_w) && !any_ison() && !how_much)
	    return;

	/* new mars stats */
	m_stats();

	/* only if we're up */
	if (m_pm)
	    m_redraw();
}

int
mars_ison()
{
	return (mform_w && XtIsManaged(mform_w));
}

/* called by other menus as they want to hear from our buttons or not.
 * the "on"s and "off"s stack - only really redo the buttons if it's the
 * first on or the last off.
 */
void
mars_selection_mode (whether)
int whether;	/* whether setting up for plotting or for not plotting */
{
	m_selecting += whether ? 1 : -1;

	if (mars_ison()) {
	    if ((whether && m_selecting == 1)     /* first one to want on */
		|| (!whether && m_selecting == 0) /* last one to want off */)
		m_set_buttons (whether);
	}
}

/* called to put up or remove the watch cursor.  */
void
mars_cursor (c)
Cursor c;
{
	Window win;

	if (mform_w && (win = XtWindow(mform_w)) != 0) {
	    Display *dsp = XtDisplay(mform_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}

	if (msform_w && (win = XtWindow(msform_w)) != 0) {
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
	    char *tip;		/* widget tip */
	} OpSetup;
	static OpSetup ops[] = {
	    {LABELS_OPT,	"Labels",	"Feature labels",
	    	"When on, major surface features will be labeled"},
	    {FLIPTB_OPT,	"FlipTB",	"Flip T/B",
	    	"Flip the map top-to-bottom"},
	    {FLIPLR_OPT,	"FlipLR",	"Flip L/R",
	    	"Flip the map left-to-right"},
	    {GRID_OPT,		"Grid",		"Grid",
	    	"When on, overlay 15 degree grid and mark Sub-Earth location"},
	    {COARSE_OPT,	"LowPrec",	"Coarse+Faster",
		"Draw map with lower precision, but somewhat faster"},
	};
	typedef struct {
	    char *label;	/* what goes on the help label */
	    char *key;		/* string to call hlp_dialog() */
	} HelpOn;
	static HelpOn helpon[] = {
	    {"Intro...",	"Mars - intro"},
	    {"on Mouse...",	"Mars - mouse"},
	    {"on Control...",	"Mars - control"},
	    {"on View...",	"Mars - view"},
	};
	Widget mb_w, pd_w, cb_w;
	Widget msw_w;
	Widget w;
	unsigned long mask;
	XmString str;
	Arg args[20];
	int i;
	int n;

	/* create master form */
	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNhorizontalSpacing, 5); n++;
	XtSetArg (args[n], XmNverticalSpacing, 5); n++;
	XtSetArg (args[n], XmNdefaultPosition, False); n++;
	XtSetArg (args[n], XmNcolormap, xe_cm); n++;
	mform_w = XmCreateFormDialog (toplevel_w, "Mars", args, n);
	set_something (mform_w, XmNcolormap, (XtArgVal)xe_cm);
	XtAddCallback (mform_w, XmNhelpCallback, m_help_cb, 0);
	XtAddCallback (mform_w, XmNunmapCallback, m_unmap_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Mars view"); n++;
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

	    /* the "Print" push button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "MPrint", args, n);
	    set_xmstring (w, XmNlabelString, "Print...");
	    XtAddCallback (w, XmNactivateCallback, m_print_cb, 0);
	    wtip (w, "Print the current Mars map");
	    XtManageChild (w);

	    /* add a separator */
	    n = 0;
	    w = XmCreateSeparator (pd_w, "CtS", args, n);
	    XtManageChild (w);

	    /* add the close button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Close", args, n);
	    XtAddCallback (w, XmNactivateCallback, m_close_cb, 0);
	    wtip (w, "Close this and all supporting dialogs");
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
		XtSetArg (args[n], XmNindicatorType, XmN_OF_MANY); n++;
		w = XmCreateToggleButton (pd_w, osp->name, args, n);
		XtAddCallback(w, XmNvalueChangedCallback, m_option_cb,
								(XtPointer)o);
		set_xmstring (w, XmNlabelString, osp->title);
		option[o] = XmToggleButtonGetState (w);
		option_w[o] = w;
		if (osp->tip)
		    wtip (w, osp->tip);
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
	    wtip (w, "Display additional information and controls");
	    XtManageChild (w);

	    /* add the Moons control */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Moons", args, n);
	    set_xmstring (w, XmNlabelString, "Moon info...");
	    XtAddCallback (w, XmNactivateCallback, m_moons_cb, NULL);
	    wtip (w, "Display schematic view of Mars and its moons");
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
	wtip (dt_w, "Date and Time for which map is computed");
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
	msw_w = XmCreateScrolledWindow (mform_w, "MarsSW", args, n);
	XtManageChild (msw_w);

	    n = 0;
	    XtSetArg (args[n], XmNmarginWidth, 0); n++;
	    XtSetArg (args[n], XmNmarginHeight, 0); n++;
	    XtSetArg (args[n], XmNwidth, 2*BORD+IMSZ); n++;
	    XtSetArg (args[n], XmNheight, 2*BORD+IMSZ); n++;
	    mda_w = XmCreateDrawingArea (msw_w, "Map", args, n);
	    XtAddCallback (mda_w, XmNexposeCallback, m_exp_cb, NULL);
            mask = Button1MotionMask | ButtonPressMask | PointerMotionHintMask;
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

/* create the "more info" stats dialog */
static void
m_create_msform()
{
	typedef struct {
	    char *label;
	    Widget *wp;
	    char *tip;
	} DItem;
	static DItem citems[] = {
	    {"Under Cursor:", NULL, NULL},
	    {"Latitude +N:",  &lat_w, "Martian Latitude under cursor"},
	    {"Longitude +W:", &lng_w, "Martian Longitude under cursor"},
	};
	Widget rc_w;
	Widget sep_w;
	Widget f_w;
	Widget w;
	char str[32];
	Arg args[20];
	int n;
	int i;

	/* create form */
	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNverticalSpacing, 10); n++;
	XtSetArg (args[n], XmNcolormap, xe_cm); n++;
	msform_w = XmCreateFormDialog (toplevel_w, "MarsStats", args, n);
	set_something (msform_w, XmNcolormap, (XtArgVal)xe_cm);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Mars info"); n++;
	XtSetValues (XtParent(msform_w), args, n);

	/* make a rowcolumn to hold the cursor tracking info */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNtopOffset, 10); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftOffset, 10); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightOffset, 10); n++;
	XtSetArg (args[n], XmNspacing, 5); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNnumColumns, XtNumber(citems)); n++;
	XtSetArg (args[n], XmNisAligned, False); n++;
	rc_w = XmCreateRowColumn (msform_w, "SRC", args, n);
	XtManageChild (rc_w);

	    for (i = 0; i < XtNumber(citems); i++) {
		DItem *dp = &citems[i];

		n = 0;
		XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
		w = XmCreateLabel (rc_w, "CLbl", args, n);
		set_xmstring (w, XmNlabelString, dp->label);
		XtManageChild (w);

		n = 0;
		XtSetArg (args[n], XmNrecomputeSize, False); n++;
		XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
		w = XmCreateLabel (rc_w, "CVal", args, n);
		set_xmstring (w, XmNlabelString, " ");

		if (dp->wp)
		    *(dp->wp) = w;
		if (dp->tip)
		    wtip (w, dp->tip);
		XtManageChild (w);
	    }


	/* make a separator between the 2 data sets */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftOffset, 10); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightOffset, 10); n++;
	sep_w = XmCreateSeparator (msform_w, "Sep1", args, n);
	XtManageChild(sep_w);

	/* make the slt row */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftOffset, 10); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	w = XmCreateLabel (msform_w, "SLTL", args, n);
	set_xmstring (w, XmNlabelString, "Sub Earth Lat:");
	XtManageChild (w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightOffset, 10); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	XtSetArg (args[n], XmNuserData, "Mars.SubLat"); n++;
	slt_w = XmCreatePushButton (msform_w, "SLTVal", args, n);
	XtAddCallback (slt_w, XmNactivateCallback, m_selection_cb, NULL);
	wtip (slt_w, "Martian latitude at center of map");
	XtManageChild (slt_w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, slt_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftOffset, 10); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightOffset, 10); n++;
	XtSetArg (args[n], XmNminimum, -90); n++;
	XtSetArg (args[n], XmNmaximum, 90); n++;
	XtSetArg (args[n], XmNscaleMultiple, 10); n++;
	XtSetArg (args[n], XmNshowValue, False); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	slts_w = XmCreateScale (msform_w, "SLTS", args, n);
	XtAddCallback (slts_w, XmNvalueChangedCallback, m_slt_cb, NULL);
	XtAddCallback (slts_w, XmNdragCallback, m_slt_cb, NULL);
	wtip (slts_w, "Set arbitrary central latitude, then use Apply");
	XtManageChild (slts_w);

	/* make the cml row */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, slts_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftOffset, 10); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	w = XmCreateLabel (msform_w, "CMLL", args, n);
	set_xmstring (w, XmNlabelString, "Central M Long:");
	XtManageChild (w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, slts_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightOffset, 10); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	XtSetArg (args[n], XmNuserData, "Mars.CML"); n++;
	cml_w = XmCreatePushButton (msform_w, "CMLVal", args, n);
	XtAddCallback (cml_w, XmNactivateCallback, m_selection_cb, NULL);
	wtip (cml_w, "Martian longitude at center of map");
	XtManageChild (cml_w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, cml_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftOffset, 10); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightOffset, 10); n++;
	XtSetArg (args[n], XmNminimum, 0); n++;
	XtSetArg (args[n], XmNmaximum, 359); n++;
	XtSetArg (args[n], XmNscaleMultiple, 10); n++;
	XtSetArg (args[n], XmNshowValue, False); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	cmls_w = XmCreateScale (msform_w, "CMLS", args, n);
	XtAddCallback (cmls_w, XmNvalueChangedCallback, m_cml_cb, NULL);
	XtAddCallback (cmls_w, XmNdragCallback, m_cml_cb, NULL);
	wtip (cmls_w, "Set arbitrary central longitude, then use Apply");
	XtManageChild (cmls_w);

	/* make the seeing row */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, cmls_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftOffset, 10); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	w = XmCreateLabel (msform_w, "SeeingL", args, n);
	set_xmstring (w, XmNlabelString, "Seeing (arc seconds):");
	XtManageChild (w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, cmls_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightOffset, 10); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	see_w = XmCreateLabel (msform_w, "SeeingV", args, n);
	wtip (see_w, "Image is blurred to simulate this seeing value");
	XtManageChild (see_w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, see_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftOffset, 10); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightOffset, 10); n++;
	XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	XtSetArg (args[n], XmNshowValue, False); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	sees_w = XmCreateScale (msform_w, "Seeing", args, n);
	XtAddCallback (sees_w, XmNvalueChangedCallback, m_see_cb, NULL);
	XtAddCallback (sees_w, XmNdragCallback, m_see_cb, NULL);
	wtip (sees_w, "Set desired seeing, then use Apply");
	XtManageChild (sees_w);

	/* pick up initial value */
	XmScaleGetValue (sees_w, &m_seeing);
	(void) sprintf (str, "%2d", m_seeing);
	set_xmstring (see_w, XmNlabelString, str);

	/* add a label for the current date/time stamp */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sees_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftOffset, 10); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightOffset, 10); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	sdt_w = XmCreateLabel (msform_w, "SDTstamp", args, n);
	wtip (sdt_w, "Date and Time for which data are computed");
	XtManageChild (sdt_w);

	/* add a separator */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sdt_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftOffset, 10); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightOffset, 10); n++;
	sep_w = XmCreateSeparator (msform_w, "Sep3", args, n);
	XtManageChild (sep_w);

	/* put the bottom controls in their own form */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomOffset, 10); n++;
	XtSetArg (args[n], XmNfractionBase, 13); n++;
	f_w = XmCreateForm (msform_w, "ACH", args, n);
	XtManageChild (f_w);

	    /* the apply button */

	    n = 0;
	    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNleftPosition, 1); n++;
	    XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNrightPosition, 4); n++;
	    apply_w = XmCreatePushButton (f_w, "Apply", args, n);
	    XtAddCallback (apply_w, XmNactivateCallback, m_apply_cb, NULL);
	    wtip (apply_w, "Apply the new values");
	    XtManageChild (apply_w);

	    /* the close button */

	    n = 0;
	    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNleftPosition, 5); n++;
	    XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNrightPosition, 8); n++;
	    w = XmCreatePushButton (f_w, "Close", args, n);
	    XtAddCallback (w, XmNactivateCallback, m_mstats_cb, NULL);
	    wtip (w, "Close this dialog");
	    XtManageChild (w);

	    /* the help button */

	    n = 0;
	    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNleftPosition, 9); n++;
	    XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNrightPosition, 12); n++;
	    w = XmCreatePushButton (f_w, "Help", args, n);
	    XtAddCallback (w, XmNactivateCallback, m_helpon_cb,
						(XtPointer)"Mars - more info");
	    wtip (w, "More info about this dialog");
	    XtManageChild (w);
}

/* go through all the buttons pickable for plotting and set whether they
 * should appear to look like buttons or just flat labels.
 */
static void
m_set_buttons (whether)
int whether;	/* whether setting up for plotting or for not plotting */
{
	m_set_a_button (cml_w, whether);
	m_set_a_button (slt_w, whether);
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

/* callback from the Moons button */
/* ARGSUSED */
static void
m_moons_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	marsm_manage();
}

/* callback from the Close button on the stats menu or when
 * the More Info button is activated. they each do the same thing.
 */
/* ARGSUSED */
static void
m_mstats_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if ((mswasman = XtIsManaged(msform_w)) != 0)
	    XtUnmanageChild (msform_w);
	else {
	    XtManageChild (msform_w);
	    m_set_buttons(m_selecting);
	}
}

/* store intensity v into array[x,y] */
static void
array_pixel (array, x, y, v)
void *array;	/* target of pixels */
int x, y;	/* coord of pixel */
int v;		/* value */
{
	unsigned short *a = (unsigned short *)array;

	a[y*IMSZ + x] = v;
}

/* callback from the Print PB */
/* ARGSUSED */
static void
m_print_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
        XPSAsk ("Mars View", m_print);
}

/* proceed to generate a postscript file.
 * call XPSClose() when finished.
 */
static void
m_print ()
{
	/* must be up */
	if (!mars_ison()) {
	    xe_msg ("Mars must be open to print.", 1);
	    XPSClose();
	    return;
	}

	watch_cursor(1);

	/* fit view in square across the top and prepare to capture X calls */
	XPSXBegin (m_pm, 0, 0, IMSZ+2*BORD, IMSZ+2*BORD, 1*72, 10*72,
								(int)(6.5*72));

        /* register the font to use for all gcs from obj_pickgc() */
	XPSRegisterFont (m_fsp->fid, "Helvetica");

	/* redraw everything now */
	m_redraw();

        /* no more X captures */
	XPSXEnd();

	/* add some extra info */
	m_ps_annotate ();

	/* finished */
	XPSClose();

	watch_cursor(0);
}

/* render the image in postscript */
static void
m_imps ()
{
	int npix = IMSZ * IMSZ;
	unsigned short *pix;
	char *cmap;
	int i;

	/* get colormap and pixel arrays */
	cmap = malloc (nmcolors);
	if (!cmap) {
	    xe_msg ("No memory for colormap to print", 1);
	    return;
	}
	pix = (unsigned short *) malloc (npix*sizeof(unsigned short));
	if (!pix) {
	    xe_msg ("No memory for pixel array to print", 1);
	    free ((void *)cmap);
	    return;
	}

	/* init colormap to inverse gray ramp. 
	 * then print inverse again, to get black text.
	 * set background to black (er, white) to avoid wasting ink.
	 */
	for (i = 0; i < nmcolors; i++)
	    cmap[i] = nmcolors - 1 - i;
	cmap[0] = 0;

	/* render image into pix[] */
	image_setup (array_pixel, (void *)pix);

	/* render in postscript -- it's already flipped */
	XPSImage (m_pm, pix, cmap, nmcolors-1, 1, BORD, BORD, IMSZ, IMSZ, 0, 0);

	/* ok */
	free ((void *)cmap);
	free ((void *)pix);
}

static void
m_ps_annotate ()
{
	Now *np = mm_get_now();
        char dir[128];
	char buf[128];
	int ctr = 306;  /* = 8.5*72/2 */
	int lx = 145, rx = 460;
	int y;

	/* caption */
	y = AROWY(13);
	(void) strcpy (buf, "XEphem Mars View");
	(void) sprintf (dir, "(%s) %d %d cstr", buf, ctr, y);
	XPSDirect (dir);

	y = AROWY(9);
	fs_date (buf, mjd_day(mjd));
	(void) sprintf (dir, "(UTC Date:) %d %d rstr (%s) %d %d lstr\n",
							lx, y, buf, lx+10, y);
	XPSDirect (dir);

	fs_dm_angle (buf, m_slt);
	(void) sprintf (dir,"(%s Latitude:) %d %d rstr (%s) %d %d lstr\n",
			fakepos ? "Center" : "Sub Earth", rx, y, buf, rx+10, y);
	XPSDirect (dir);

	y = AROWY(8);
	fs_time (buf, mjd_hr(mjd));
	(void) sprintf (dir, "(UTC Time:) %d %d rstr (%s) %d %d lstr\n",
							lx, y, buf, lx+10, y);
	XPSDirect (dir);

	fs_dm_angle (buf, m_cml);
	(void) sprintf (dir,"(%s Longitude:) %d %d rstr (%s) %d %d lstr\n",
			fakepos ? "Center" : "Sub Earth", rx, y, buf, rx+10, y);
	XPSDirect (dir);

	/* add site/lat/long if topocentric */
	if (pref_get(PREF_EQUATORIAL) == PREF_TOPO) {
	    char *site;

	    /* put site name under caption */
	    site = mm_getsite();
	    if (site) {
		y = AROWY(12);
		(void) sprintf (dir, "(%s) %d %d cstr\n",
	    				XPSCleanStr(site,strlen(site)), ctr, y);
		XPSDirect (dir);
	    }

	    /* then add lat/long */
	    y = AROWY(10);

	    fs_sexa (buf, raddeg(fabs(lat)), 3, 3600);
	    (void) sprintf (dir, "(Latitude:) %d %d rstr (%s %c) %d %d lstr\n",
				    lx, y, buf, lat < 0 ? 'S' : 'N', lx+10, y);
	    XPSDirect (dir);

	    fs_sexa (buf, raddeg(fabs(lng)), 4, 3600);
	    (void) sprintf (dir,"(Longitude:) %d %d rstr (%s %c) %d %d lstr\n",
				    rx, y, buf, lng < 0 ? 'W' : 'E', rx+10, y);
	    XPSDirect (dir);
	}

	/* add seeing if > 0 */
	if (m_seeing) {
	    y = AROWY(6);
	    (void) sprintf (dir, "(Simulated %d Arcsecond Seeing) %d %d cstr\n",
							    m_seeing, ctr, y);
	    XPSDirect (dir);
	}
    }

/* callback from CML or SLT button being activated.
 */
/* ARGSUSED */
static void
m_selection_cb (w, client, call)
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

/* callback from the CML scale */
/* ARGSUSED */
static void
m_cml_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmScaleCallbackStruct *sp = (XmScaleCallbackStruct *)call;
	int v = sp->value;

	f_dm_angle (cml_w, degrad((double)v));

	/* Apply button is now useful */
	XtSetSensitive (apply_w, True);
}

/* callback from the SLT scale */
/* ARGSUSED */
static void
m_slt_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmScaleCallbackStruct *sp = (XmScaleCallbackStruct *)call;
	int v = sp->value;

	f_dm_angle (slt_w, degrad((double)v));

	/* Apply button is now useful */
	XtSetSensitive (apply_w, True);
}

/* callback from the Seeing scale */
/* ARGSUSED */
static void
m_see_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmScaleCallbackStruct *sp = (XmScaleCallbackStruct *)call;
	char str[32];
	int v;

	v = sp->value;
	(void) sprintf (str, "%2d", v);
	set_xmstring (see_w, XmNlabelString, str);

	/* Apply button is now useful */
	XtSetSensitive (apply_w, True);
}

/* callback from the Apply PB */
/* ARGSUSED */
static void
m_apply_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int v;

	/* gather the new values */
	XmScaleGetValue (cmls_w, &v);
	m_cml = degrad(v);
	f_dm_angle (cml_w, m_cml);

	XmScaleGetValue (slts_w, &v);
	m_slt = degrad(v);
	m_sslt = sin(m_slt);
	m_cslt = cos(m_slt);
	f_dm_angle (slt_w, m_slt);

	XmScaleGetValue (sees_w, &v);
	m_seeing = v;

	/* force a redraw */
	fakepos = 1;
	m_redraw();
}

/* callback from the Point PB */
/* ARGSUSED */
static void
m_aim_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	/* fake looking at skypu_{l,L} with pa 0 */

	m_slt = skypu_l;
	m_sslt = sin(m_slt);
	m_cslt = cos(m_slt);
	f_dm_angle (slt_w, m_slt);
	XmScaleSetValue (slts_w, (int)(raddeg(m_slt)));

	m_cml = skypu_L;
	f_dm_angle (cml_w, m_cml);
	XmScaleSetValue (cmls_w, (int)(raddeg(m_cml)));

	m_pa = 0.0;
	m_spa = 0.0;
	m_cpa = 1.0;

	fakepos = 1;
	m_redraw();
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
	if ((mswasman = (msform_w && XtIsManaged(msform_w))) != 0)
	    XtUnmanageChild (msform_w);

#if 0
	/* N.B. don't free m_xim or mimage because there seems to be a new
	 * bug in newer servers (eg, VMS V7) which causes an expose after
	 * an unmap. it's ok to free the pixmap because it's rebuilt.
	 */
	if (m_xim) {
	    free ((void *)m_xim->data);
	    m_xim->data = NULL;
	    XDestroyImage (m_xim);
	    m_xim = NULL;
	}

	if (mimage) {
	    free (mimage);
	    mimage = NULL;
	}
#endif

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

	/* unmapping mform_w will do all the real work */
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

	case GRID_OPT:
	    if (set) {
		m_grid();
		m_refresh (NULL);
	    } else
		m_redraw();
	    break;

	case LABELS_OPT:
	    if (set) {
		if (m_labels() < 0) {
		    XmToggleButtonSetState (option_w[LABELS_OPT], False, False);
		    option[LABELS_OPT] = 0;
		} else
		    m_refresh (NULL);
	    } else
		m_redraw();
	    break;

	case FLIPTB_OPT:
	    m_redraw();
	    break;

	case FLIPLR_OPT:
	    m_redraw();
	    break;

	case COARSE_OPT:
	    m_redraw();
	    break;

	case N_OPT:
	    break;
	}


	watch_cursor (0);
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
	    "This is a map of Mars.",
	};

	hlp_dialog ("Mars", msg, XtNumber(msg));
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

/* expose (or reconfig) of mars image view drawing area.
 * N.B. since we are in ScrolledWindow we will never see resize events.
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
	    /* turn off backing store */
	    static int before;

	    if (!before) {
		XSetWindowAttributes swa;
		unsigned long mask = CWBackingStore;

		swa.backing_store = NotUseful;
		XChangeWindowAttributes (e->display, e->window, mask, &swa);
		before = 1;
	    }
	    break;
	    }
	default:
	    printf ("Unexpected mars mda_w event. type=%d\n", c->reason);
	    exit(1);
	}


	if (!m_pm) {
	    unsigned wid, hei, d;

	    m_getsize (win, &wid, &hei, &d);
	    if (wid != IMSZ+2*BORD || hei != IMSZ+2*BORD) {
		printf ("Mars exp_cb: Bad size: wid=%d IMSZ=%d hei=%d\n",
								wid, IMSZ, hei);
		exit(1);
	    }

	    m_pm = XCreatePixmap (dsp, win, wid, hei, d);
	    m_draw();
	}

	/* update exposed area */
	m_refresh (e);

	watch_cursor (0);
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
	int b3p;

	XQueryPointer (dsp, win, &root, &child, &rx, &ry, &x, &y, &mask);

        b3p = evt == ButtonPress   && ev->xbutton.button == Button3;

	m_reportloc (x, y);
	if (b3p)
	    m_popup (ev);
}

/* establish mimage and m_xim and return 0 else xe_msg() and return -1 */
static int
m_fits()
{
	char msg[1024];
	char fn[1024];
	int fd;
	int v;

	/* open mars map */
#ifndef VMS
	(void) sprintf (fn, "%s/auxil/marsmap.fts",  getShareDir());
#else
	(void) sprintf (fn, "%s[auxil]marsmap.fts",  getShareDir());
#endif
	fd = openh (fn, 0);
	if (fd < 0) {
	    (void) sprintf (msg, "%s: %s\n", fn, syserrstr());
	    xe_msg (msg, 1);
	    return (-1);
	}

	/* read mars file into marsfits */
	if (readFITS (fd, &marsfits, msg) < 0) {
	    char msg2[1024];
	    (void) sprintf (msg2, "%s: %s", fn, msg);
	    xe_msg (msg2, 1);
	    (void) close (fd);
	    return (-1);
	}
	(void) close (fd);

	/* make sure it's the new flipped version */
	if (getIntFITS (&marsfits, "XEVERS", &v) < 0 || v != XEVERS) {
	    (void) sprintf (msg, "%s: Incorrect version", fn);
	    xe_msg (msg, 1);
	    return (-1);
	}

	/* make some sanity checks */
	if (marsfits.bitpix != 8 || marsfits.sh != IMR || marsfits.sw != IMSZ) {
	    (void) sprintf (msg, "%s: Expected %d x %d but found %d x %d", fn,
					IMR, IMSZ, marsfits.sw, marsfits.sh);
	    xe_msg (msg, 1);
	    resetFImage (&marsfits);
	    return (-1);
	}
	mimage = (unsigned char *) marsfits.image;

	/* dither mimage if we only have 2 colors to work with */
	if (mbpp == 1) 
	    mBWdither();

	/* create m_xim */
	if (mxim_create () < 0) {
	    resetFImage (&marsfits);
	    mimage = NULL;
	    return (-1);
	}

	return(0);
}

/* create m_xim of size IMSZxIMSZ, depth mdepth and bit-per-pixel mbpp.
 * make a Bitmap if only have 1 bit per pixel, otherwise a Pixmap.
 * return 0 if ok else -1 and xe_msg().
 */
static int
mxim_create ()
{
	Display *dsp = XtDisplay (mda_w);
	int nbytes = IMSZ*IMSZ*mbpp/8;
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
	m_xim = XCreateImage (dsp, DefaultVisual (dsp, DefaultScreen(dsp)),
	    /* depth */         mbpp == 1 ? 1 : mdepth,
	    /* format */        mbpp == 1 ? XYBitmap : ZPixmap,
	    /* offset */        0,
	    /* data */          data,
	    /* width */         IMSZ,
	    /* height */        IMSZ,
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

/* redraw the current scene */
static void
m_redraw()
{
	watch_cursor (1);

	XmUpdateDisplay (toplevel_w);
	m_draw ();
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

	/* ignore of no pixmap now */
	if (!pm)
	    return;

	if (ep) {
	    x = ep->x;
	    y = ep->y;
	    w = ep->width;
	    h = ep->height;
	} else {
	    w = IMSZ+2*BORD;
	    h = IMSZ+2*BORD;
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

/* make the various gcs, handy pixel values and fill in mcolors[].
 * N.B. just call this once.
 */
static void
m_init_gcs()
{
	Display *dsp = XtD;
	Window win = XtWindow(toplevel_w);
	Colormap cm = xe_cm;
	XGCValues gcv;
	XColor xc;
	unsigned int gcm;
	Pixel fg;
	Pixel p;

	/* make gcs from Map colors
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
	if (get_color_resource (mda_w, "MarsAnnotColor", &p) < 0
						    || p == mbg || p == fg) {
	    xe_msg("Can not get Mars annotation color -- using white.", 0);
	    p = WhitePixel (dsp, 0);
	}
	gcm = GCForeground | GCBackground;
	gcv.foreground = p;
	gcv.background = mbg;
	m_agc = XCreateGC (dsp, win, gcm, &gcv);

	get_views_font (dsp, &m_fsp);
	XSetFont (dsp, m_agc, m_fsp->fid);

	/* build color ramp for image.
	 * base the scale off the foreground color.
	 */
	xc.pixel = fg;
	XQueryColor (dsp, cm, &xc);
	nmcolors = alloc_ramp (dsp, &xc, cm, mcolors, MAXCOLORS);

	/* unless we will be using a bitmap, force color 0 to background. */
	if (nmcolors > 2)
	    mcolors[0] = mbg;

	if (nmcolors < MAXCOLORS) {
	    char msg[1024];
	    (void) sprintf (msg, "Wanted %d but only found %d colors for Mars.",
							MAXCOLORS, nmcolors);
	    xe_msg (msg, 0);
	}
}

/* update mars info and draw the stat labels */
static void
m_stats ()
{
	Now *np = mm_get_now();

	/* get fresh mars info */
	marsop = db_basic (MARS);
	db_update (marsop);
	cm_dec = cos(marsop->s_gaedec);
	sm_dec = sin(marsop->s_gaedec);

	/* compute and display the CML and SLT and polar position angle */
	mars_cml (np, &m_cml, &m_slt, &m_pa);
	m_sslt = sin(m_slt);
	m_cslt = cos(m_slt);
	m_spa = sin(m_pa);
	m_cpa = cos(m_pa);

	f_dm_angle (cml_w, m_cml);
	XmScaleSetValue (cmls_w, (int)(raddeg(m_cml)));
	f_dm_angle (slt_w, m_slt);
	XmScaleSetValue (slts_w, (int)(raddeg(m_slt)));

	/* reset fake flag */
	fakepos = 0;

	/* update time stamps too */
	timestamp (np, dt_w);
	timestamp (np, sdt_w);
}

/* setup m_xim, copy to m_pm, then add all overlay info.
 * N.B. this just fills the pixmap; call m_refresh() to copy to the screen.
 */
static void
m_draw ()
{
	/* check assumptions */
	if (!mimage) {
	    printf ("No mars mimage!\n");
	    exit (1);
	}
	if (!m_xim) {
	    printf ("No mars m_xim!\n");
	    exit (1);
	}
	if (!m_pm) {
	    printf ("No mars m_pm Pixmap!\n");
	    exit(1);
	}

	/* Apply button is no longer useful */
	XtSetSensitive (apply_w, False);

	/* fill in m_xim from mimage */
	mxim_setup ();

	/* clear m_pm */
	XFillRectangle (XtD, m_pm, m_bgc, 0, 0, IMSZ+2*BORD, IMSZ+2*BORD);

	/* copy m_xim to p_pm */
	XPutImage (XtD, m_pm, m_fgc, m_xim, 0, 0, BORD, BORD, IMSZ, IMSZ);
	/* and print image if we are doing that */
	if (XPSDrawing())
	    m_imps();


	/* add grid, if enabled */
	if (option[GRID_OPT])
	    m_grid();

	/* add labels, if enabled */
	if (option[LABELS_OPT])
	    if (m_labels() < 0) {
		XmToggleButtonSetState (option_w[LABELS_OPT], False, False);
		option[LABELS_OPT] = 0;
	    }

	/* always add orientation markings */
	m_orientation();

	/* and the size calibration */
	m_sizecal();
}

/* put up the mars_db labels */
static int
m_labels()
{
	Display *dsp = XtDisplay (mda_w);
	Window win = m_pm;
	char buf[1024];
	char fn[1024];
	FILE *fp;

	/* open the file */
#ifndef VMS
	(void) sprintf (fn, "%s/auxil/mars_db",  getShareDir());
#else
	(void) sprintf (fn, "%s[auxil]mars_db",  getShareDir());
#endif
	fp = fopenh (fn, "r");
	if (!fp) {
	    (void) sprintf (buf, "%s: %s", fn, syserrstr());
	    xe_msg (buf, 1);
	    return (-1);
	}

	/* read and display each feature */
	while (fgets (buf, sizeof(buf), fp)) {
	    char name[256];
	    char type[256];
	    double lt, lg;
	    int dia;
	    int dir, asc, des;
	    XCharStruct all;
	    int x, y;
	    int nf;
	    int l;

	    /* ignore all lines that do not follow the pattern */
	    nf = sscanf(buf,"%[^|]|%[^|]|%lf|%lf|%d", name, type, &lt,&lg,&dia);
	    if (nf != 5)
		continue;

	    /* find map location in X windows coords */
	    if (ll2xy (degrad(lt), degrad(lg), &x, &y) < 0)
		continue;
	    x = IX2XX(x);
	    y = IY2XY(y);

	    /* center and display the name -- discard trailing white space */
	    l = strlen (name);
	    while (l > 0 && (name[l-1] == ' ' || name[l-1] == '\t'))
		l--;
	    name[l] = '\0';
	    XTextExtents (m_fsp, name, l, &dir, &asc, &des, &all);
	    y += all.ascent/2;
	    x -= all.width/2;
	    XPSDrawString (dsp, win, m_agc, x, y, name, l);
	}

	fclose(fp);
	return (0);
}

/* draw the N/S E/W labels on m_pm */
static void
m_orientation()
{
	Now *np = mm_get_now();
	double mr, mra, mdec;
	double ra, dec;
	int x, y;

	mr = degrad(marsop->s_size/3600.0)/2 * 1.1;
	mra = marsop->s_gaera;
	mdec = marsop->s_gaedec;

	ra = mra + mr/cm_dec;
	dec = mdec;
	m_eqproject (np, ra, dec, &x, &y);
	x = IX2XX(x);
	y = IY2XY(y);
	XPSDrawString (XtD, m_pm, m_agc, x, y, "E", 1);

	ra = mra - mr/cm_dec;
	m_eqproject (np, ra, dec, &x, &y);
	x = IX2XX(x);
	y = IY2XY(y);
	XPSDrawString (XtD, m_pm, m_agc, x, y, "W", 1);

	ra = mra;
	dec = mdec + mr;
	m_eqproject (np, ra, dec, &x, &y);
	x = IX2XX(x);
	y = IY2XY(y);
	XPSDrawString (XtD, m_pm, m_agc, x, y, "N", 1);

	dec = mdec - mr;
	m_eqproject (np, ra, dec, &x, &y);
	x = IX2XX(x);
	y = IY2XY(y);
	XPSDrawString (XtD, m_pm, m_agc, x, y, "S", 1);
}

/* draw the the size calibration */
static void
m_sizecal()
{
	XID fid = XGContextFromGC (m_agc);
	int dir, asc, des;
	XCharStruct xcs;
	char buf[64];
	int l;

	(void) sprintf (buf, "%.1f\"", marsop->s_size);
	l = strlen (buf);
	XQueryTextExtents (XtD, fid, buf, l, &dir, &asc, &des, &xcs);

	XPSDrawLine (XtD, m_pm, m_agc, BORD, IMSZ+3*BORD/2, IMSZ+BORD,
							    IMSZ+3*BORD/2);
	XPSDrawLine (XtD, m_pm, m_agc, BORD, IMSZ+3*BORD/2-3, BORD,
							    IMSZ+3*BORD/2+3);
	XPSDrawLine (XtD, m_pm, m_agc, BORD+IMSZ, IMSZ+3*BORD/2-3, BORD+IMSZ,
							    IMSZ+3*BORD/2+3);
	XPSDrawString (XtD, m_pm, m_agc, BORD+IMSZ/2-xcs.width/2,
					    IMSZ+3*BORD/2+xcs.ascent+6, buf, l);
}

/* draw a coordinate grid over the image already on m_pm */
static void
m_grid()
{
	Display *dsp = XtDisplay (mda_w);
	double fsp = option[COARSE_OPT] ? GSP : FSP;
	Window win = m_pm;
	double lt, lg;
	int x, y;

	/* lines of constant lat */
	for (lt = -PI/2 + GSP; lt < PI/2; lt += GSP) {
	    XPoint xpt[(int)(2*PI/FSP)+2];
	    int npts = 0;

	    for (lg = 0; lg <= 2*PI+fsp; lg += fsp) {
		if (ll2xy(lt, lg, &x, &y) < 0) {
		    if (npts > 0) {
			XPSDrawLines (dsp, win, m_agc,xpt,npts,CoordModeOrigin);
			npts = 0;
		    }
		    continue;
		}

		if (npts >= XtNumber(xpt)) {
		    printf ("Mars lat grid overflow\n");
		    exit (1);
		}
		xpt[npts].x = IX2XX(x);
		xpt[npts].y = IY2XY(y);
		npts++;
	    }

	    if (npts > 0)
		XPSDrawLines (dsp, win, m_agc, xpt, npts, CoordModeOrigin);
	}

	/* lines of constant longitude */
	for (lg = 0; lg < 2*PI; lg += GSP) {
	    XPoint xpt[(int)(2*PI/FSP)+1];
	    int npts = 0;

	    for (lt = -PI/2; lt <= PI/2; lt += fsp) {
		if (ll2xy(lt, lg, &x, &y) < 0) {
		    if (npts > 0) {
			XPSDrawLines (dsp, win, m_agc,xpt,npts,CoordModeOrigin);
			npts = 0;
		    }
		    continue;
		}

		if (npts >= XtNumber(xpt)) {
		    printf ("Mars lng grid overflow\n");
		    exit (1);
		}
		xpt[npts].x = IX2XX(x);
		xpt[npts].y = IY2XY(y);
		npts++;
	    }

	    if (npts > 0)
		XPSDrawLines (dsp, win, m_agc, xpt, npts, CoordModeOrigin);
	}

	/* X marks the center, unless rotated by hand */
	if (!fakepos) {
	    XPSDrawLine (dsp, win, m_agc, IX2XX(-XRAD), IY2XY(-XRAD),
						    IX2XX(XRAD), IY2XY(XRAD));
	    XPSDrawLine (dsp, win, m_agc, IX2XX(-XRAD), IY2XY(XRAD),
						    IX2XX(XRAD), IY2XY(-XRAD));
	}
}

/* store intensity v into array[x,y] (which is really m_xim) */
static void
mxim_pixel (array, x, y, v)
void *array;    /* target of pixels */
int x, y;       /* coord of pixel */
int v;          /* value */
{
        Pixel p = mcolors[v];

	XPutPixel ((XImage *)array, x, y, p);
}

/* fill in m_xim from mimage and current circumstances.
 * m_xim is IMSZxIMSZ, mimage is IMSZ wide x IMR high.
 */
static void
mxim_setup ()
{
        image_setup (mxim_pixel, (void *)m_xim);
}

/* compute a scene from mimage and current circumstances.
 * store the image using (*fp)(arg, x, y, gray_value).
 */
static void
image_setup (fp, arg)
void (*fp)();   /* function to set a pixel in arg */
void *arg;      /* where to store pixels */
{
#define	SQR(x)	((x)*(x))
	int tb = option[FLIPTB_OPT];
	int lr = option[FLIPLR_OPT];
	unsigned char pict[IMSZ/SMPLSZ][IMSZ/SMPLSZ];	/* sqsz copy */
	unsigned char see[IMSZ/SMPLSZ];			/* sqsz seeing map */
	int pixseeing = IMSZ*m_seeing/marsop->s_size;
	int sqsz = option[COARSE_OPT] ? 5*SMPLSZ : SMPLSZ;
	double csh;
	int lsh;
	int x, y;

	/* init the copy -- background is 0 */
	zero_mem ((void *)pict, sizeof(pict));

	/* scan to build up the sampled albedo map, and allow for flipping */
	for (y = -IMR; y < IMR; y++) {
	    int iy = (tb ? -y-1 : y) + IMR;
	    unsigned char *prow = pict[iy/sqsz];

	    pm_set ((y+IMR)*50/IMSZ);

	    for (x = -IMR; x < IMR; x++) {
		int ix = (lr ? -x-1 : x) + IMR;
		unsigned char *p = &prow[ix/sqsz];

		if (x*x + y*y < IMR*IMR && !*p) {
		    double l, L;
		    int mx, my;

		    (void) xy2ll (x, y, &l, &L);

		    my = (int)((l+PI/2)/PI*IMR);
		    mx = ((int)(L/(2*PI)*IMSZ));
		    *p = mimage[my*IMSZ+mx];
		}
	    }
	}

	/* find cos of shadow foreshortening angle based on planar
	 * sun-mars-earth triangle and earth-sun == 1.
	 * if we are faking it, turn off the shadow.
	 */
	csh = fakepos ? 1.0 : (SQR(marsop->s_sdist) + SQR(marsop->s_edist) - 1)/
					(2*marsop->s_sdist*marsop->s_edist);

	/* shadow is on left if elongation is positive and flipped lr
	 * or elongation is negative and not flipped lr.
	 */
	lsh = (marsop->s_elong > 0.0 && lr) || (marsop->s_elong < 0.0 && !lr);

	/* scan again to blur, add shadow and place real pixels */
	for (y = -IMR; y < IMR; y++) {
	    int iy = y + IMR;
	    int lx, rx;

	    pm_set (50+(y+IMR)*50/IMSZ);

	    if (lsh) {
		lx = -csh*sqrt((double)(IMR*IMR - y*y));
		rx = IMR;
	    } else {
		lx = -IMR;
		rx = csh*sqrt((double)(IMR*IMR - y*y));
	    }

	    /* fill in seeing table if first time in this sqsz-hi row */
	    if (pixseeing > 0 && (iy%sqsz) == 0) {
		for (x = -IMR; x < IMR; x += sqsz) {
		    int ix = x + IMR;
		    int s = (pixseeing+1)/2;
		    int nsc = 0;
		    int sc = 0;
		    int sx, sy;
		    int step;


		    /* establish a fairly sparce sampling step size */
		    step = 2*s/10;
		    if (step < sqsz)
			step = sqsz;

		    /* average legs of a sparse cross.
		     * tried just the corners -- leaves artifacts but fast
		     * tried filled square -- beautiful but dreadfully slow
		     */
		    for (sx = x-s; sx <= x+s; sx += step) {
			if (sx*sx + y*y <= IMR*IMR) {
			    sc += pict[iy/sqsz][(sx+IMR)/sqsz];
			    nsc++;
			}
		    }
		    for (sy = y-s; sy <= y+s; sy += step) {
			if (x*x + sy*sy <= IMR*IMR) {
			    sc += pict[(sy+IMR)/sqsz][ix/sqsz];
			    nsc++;
			}
		    }
		    see[ix/sqsz] = nsc > 0 ? sc/nsc : 0;
		}
	    }

	    for (x = -IMR; x < IMR; x++) {
		int ix = x + IMR;
		int c, v;

		if (x < lx || x > rx || x*x + y*y >= IMR*IMR) {
		    c = 0;
		} else if (pixseeing > 0) {
		    c = see[ix/sqsz];
		} else
		    c = pict[iy/sqsz][ix/sqsz];

		v = c*nmcolors/256;
		(*fp) (arg, ix, iy, v);
	    }
	}
}

/* convert [x,y] to true mars lat/long, in rads.
 *   x: centered, +right, -IMR .. x .. IMR
 *   y: centered, +down,  -IMR .. y .. IMR
 * return 0 if x,y are really over the planet, else -1.
 * caller can be assured -PI/2 .. l .. PI/2 and 0 .. L .. 2*PI.
 * N.B. it is up to the caller to deal wth flipping.
 */
static int
xy2ll (x, y, lp, Lp)
int x, y;
double *lp, *Lp;
{
	double R = sqrt ((double)(x*x + y*y));
	double a;
	double ca, B;

	if (R >= IMR)
	    return (-1);

	if (y == 0)
	    a = x < 0 ? -PI/2 : PI/2;
	else 
	    a = atan2((double)x,(double)y);
	solve_sphere (a, asin(R/IMR), m_sslt, m_cslt, &ca, &B);

	*lp = PI/2 - acos(ca);
	*Lp = m_cml + B;
	range (Lp, 2*PI);

	return (0);
}

/* convert true mars lat/long, in rads, to [x,y].
 *   x: centered, +right, -IMR .. x .. IMR
 *   y: centered, +down,  -IMR .. y .. IMR
 * return 0 if loc is on the front face, else -1 (but still compute x,y);
 * N.B. it is up to the caller to deal wth flipping of the resulting locs.
 */
static int
ll2xy (l, L, xp, yp)
double l, L;
int *xp, *yp;
{
	double sR, cR;
	double A, sA, cA;

	solve_sphere (L - m_cml, PI/2 - l, m_sslt, m_cslt, &cR, &A);
	sR = sqrt(1.0 - cR*cR);
	sA = sin(A);
	cA = cos(A);

	*xp = (int)floor(IMR*sR*sA + 0.5);
	*yp = (int)floor(IMR*sR*cA + 0.5);

	return (cR > 0 ? 0 : -1);
}

/* dither mimage into a 2-intensity image: 1 and 255.
 * form 2x2 tiles whose pattern depends on intensity peak and spacial layout.
 */
static void
mBWdither()
{
	int idx[4];
	int y;

	idx[0] = 0;
	idx[1] = 1;
	idx[2] = IMSZ;
	idx[3] = IMSZ+1;

	for (y = 0; y < IMR; y += 2) {
	    unsigned char *mp = &mimage[y*IMSZ];
	    unsigned char *lp;

	    for (lp = mp + IMSZ; mp < lp; mp += 2) {
		int sum, numon;
		int i;

		sum = 0;
		for (i = 0; i < 4; i++)
		    sum += (int)mp[idx[i]];
		numon = sum*5/1021;	/* 1021 is 255*4 + 1 */

		for (i = 0; i < 4; i++)
		    mp[idx[i]] = 0;

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

/* report the location of x,y, which are with respect to mda_w.
 * N.B. allow for flipping and the borders.
 */
static void
m_reportloc (x, y)
int x, y;
{
	double lt, lg;

	/* convert from mda_w X Windows coords to centered m_xim coords */
	x = XX2IX(x);
	y = XY2IY(y);

	if (xy2ll (x, y, &lt, &lg) == 0) {
	    f_dm_angle (lat_w, lt);
	    f_dm_angle (lng_w, lg);
	} else {
	    set_xmstring (lat_w, XmNlabelString, " ");
	    set_xmstring (lng_w, XmNlabelString, " ");
	}
}

/* called when hit button 3 over image */
static void
m_popup (ep)
XEvent *ep;
{
	XButtonEvent *bep;
	int overmars;
	int x, y;

	bep = &ep->xbutton;
	x = bep->x;
	y = bep->y;

	overmars = xy2ll (XX2IX(x), XY2IY(y), &skypu_l, &skypu_L) == 0;

	if (overmars) {
	    f_sexa (skypu_l_w, raddeg(skypu_l), 3, 3600);
	    f_sexa (skypu_L_w, raddeg(skypu_L), 3, 3600);
	    XmMenuPosition (skypu_w, (XButtonPressedEvent *)ep);
	    XtManageChild (skypu_w);
	}
}

/* create the surface popup menu */
static void
m_create_popup()
{
	Widget w;
	Arg args[20];
	int n;

	n = 0;
	XtSetArg (args[n], XmNisAligned, True); n++;
	XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
	skypu_w = XmCreatePopupMenu (mda_w, "MSKYPU", args, n);

	n = 0;
	skypu_l_w = XmCreateLabel (skypu_w, "MLat", args, n);
	wtip (skypu_l_w, "Latitude");
	XtManageChild (skypu_l_w);

	n = 0;
	skypu_L_w = XmCreateLabel (skypu_w, "MLong", args, n);
	wtip (skypu_L_w, "Longitude");
	XtManageChild (skypu_L_w);

	n = 0;
	w = XmCreateSeparator (skypu_w, "MSKYPS", args, n);
	XtManageChild (w);

	n = 0;
	skypu_aim_w = XmCreatePushButton (skypu_w, "Point", args, n);
	XtAddCallback (skypu_aim_w, XmNactivateCallback, m_aim_cb, NULL);
	wtip (skypu_aim_w, "Center this location in the view");
	XtManageChild (skypu_aim_w);
}

/* given geocentric ra and dec find image [xy] on martian equitorial projection.
 * [0,0] is center, +x martian right/west (celestial east) +y down/north.
 * this matches the raw albedo image coordinates without any flipping.
 * N.B. we do *not* allow for flipping here.
 * N.B. this only works when ra and dec are near mars.
 */
static void
m_eqproject (np, ra, dec, xp, yp)
Now *np;
double ra, dec;
int *xp, *yp;
{
	double scale = IMSZ/degrad(marsop->s_size/3600.0); /* pix/rad */
	double xr, yr;
	double x, y;

	/* find x and y so +x is celestial right/east and +y is down/north */
	x = scale*(ra - marsop->s_gaera)*cm_dec;
	y = scale*(dec - marsop->s_gaedec);

	/* rotate by position angle, m_pa.
	 */
	xr = x*m_cpa - y*m_spa;
	yr = x*m_spa + y*m_cpa;

	*xp = (int)floor(xr + 0.5);
	*yp = (int)floor(yr + 0.5);
}

/* return:
 *   *cmlp: Martian central meridian longitude;
 *   *sltp: subearth latitude
 *   *pap:  position angle of N pole (ie, rads E of N)
 * all angles in rads.
 */

#define M_CML0  degrad(325.845)         /* Mars' CML towards Aries at M_MJD0 */
#define M_MJD0  (2418322.0 - MJD0)      /* mjd date of M_CML0 */
#define M_PER   degrad(350.891962)      /* Mars' rotation period, rads/day */

static void
mars_cml(np, cmlp, sltp, pap)
Now *np;
double *cmlp;
double *sltp;
double *pap;
{
	Obj *sp;
	double a;	/* angle from Sun ccw to Earth seen from Mars, rads */
	double Ae;	/* planetocentric longitude of Earth from Mars, rads */
	double cml0;	/* Mar's CML towards Aries, rads */
	double lc;	/* Mars rotation correction for light travel, rads */
	double tmp;

	sp = db_basic (SUN);
	db_update (sp);

	a = asin (sp->s_edist/marsop->s_edist*sin(marsop->s_hlong-sp->s_hlong));
	Ae = marsop->s_hlong + PI + a;
	cml0 = M_CML0 + M_PER*(mjd-M_MJD0) + PI/2;
	range(&cml0, 2*PI);
	lc = LTAU * marsop->s_edist/SPD*M_PER;
	*cmlp = cml0 - Ae - lc;
	range (cmlp, 2*PI);

	solve_sphere (POLE_RA - marsop->s_gaera, PI/2-POLE_DEC, sm_dec, cm_dec,
								    &tmp, pap);

	/* from Green (1985) "Spherical Astronomy", Cambridge Univ. Press,
	 * p.428. Courtesy Jim Bell.
	 */
        *sltp = asin (-sin(POLE_DEC)*sin(marsop->s_gaedec) -
					cos(POLE_DEC)*cos(marsop->s_gaedec)
						* cos(marsop->s_gaera-POLE_RA));
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: marsmenu.c,v $ $Date: 1999/02/25 06:55:37 $ $Revision: 1.8 $ $Name:  $"};
