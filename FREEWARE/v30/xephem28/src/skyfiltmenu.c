/* code to manage the stuff on the sky view filter display.
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
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/Text.h>
#include <Xm/Scale.h>
#include <Xm/Separator.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"

extern Widget	toplevel_w;
#define	XtD	XtDisplay(toplevel_w)

extern Now *mm_get_now P_((void));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void gk_mag P_((double g, double k, double rp, double rho, double *mp));
extern void prompt_map_cb P_((Widget w, XtPointer client, XtPointer call));
extern void sv_all P_((Now *np));
extern void sv_draw_obj P_((Display *dsp, Drawable win, GC gc, Obj *op, int x,
    int y, int diam, int dotsonly));

/* info for each filter option toggle */
typedef struct {
    char *label;	/* label on the toggle button */
    char *name;		/* name of toggle button, for resource setting */
    int type;		/* as in Obj.type */
    char class;		/* as in Obj.f.f_class */
    char state;		/* 0 or 1 depending on whether this class is on */
    Widget tbw;		/* the toggle button widget */
} FilterTB;

/* info for a set of filters forming a category */
typedef struct {
    char *label;	/* label for this category */
    FilterTB *ftb;	/* array of toggle button filters in this class */
    int nftb;		/* entries in ftb[] */
    Widget pbw;		/* the category "toggle" pushbutton */
} FilterCat;

static void svf_create_filter P_((Widget mainrc, FilterCat *fcp));
static void svf_reset P_((void));
static void svf_apply P_((void));
static int svf_bright_ok P_((Obj *op));
static void svf_da_cb P_((Widget w, XtPointer client, XtPointer call));
static void svf_draw_symbol P_((FilterTB *ftbp, Widget daw));
static void svf_apply_cb P_((Widget w, XtPointer client, XtPointer call));
static void svf_ok_cb P_((Widget w, XtPointer client, XtPointer call));
static void svf_close_cb P_((Widget w, XtPointer client, XtPointer call));
static void svf_toggle_cb P_((Widget w, XtPointer client, XtPointer call));
static void svf_all_cb P_((Widget w, XtPointer client, XtPointer call));
static void svf_reset_cb P_((Widget w, XtPointer client, XtPointer call));
static void svf_cat_toggle_cb P_((Widget w, XtPointer client, XtPointer call));


static FilterTB solsys_f[] = {
    {"Planets",		"Planets",	PLANET,		0},
    {"Elliptical",	"Elliptical",	ELLIPTICAL,	0},
    {"Hyperbolic",	"Hyperbolic",	HYPERBOLIC,	0},
    {"Parabolic",	"Parabolic",	PARABOLIC,	0},
    {"Earth Sat",	"EarthSat",	EARTHSAT,	0}
};

static FilterTB other_f[] = {
    {"Q Quasars", 	"Quasars",	FIXED,		'Q'},
    {"L Pulsars", 	"Pulsars",	FIXED,		'L'},
    {"J Radio", 	"Radio",	FIXED,		'J'},
    {"R S/N Remnants", 	"SNRemnants",	FIXED,		'R'},
    {"T Stellar",	"Stellar",	FIXED,		'T'},
    {"Undefined",	"Undefined",	FIXED,		'\0'}
};

static FilterTB stars_f[] = {
    {"S Single",	"Stars",	FIXED,		'S'},
    {"B Binary",	"Binary",	FIXED,		'B'},
    {"D Double",	"Double",	FIXED,		'D'},
    {"M Multiple",	"Multiple",	FIXED,		'M'},
    {"V Variable",	"Variable",	FIXED,		'V'}
};

static FilterTB nebulae_f[] = {
    {"N Bright",	"BrightNeb",	FIXED,		'N'},
    {"F Diffuse",	"DiffuseNeb",	FIXED,		'F'},
    {"K Dark",		"DarkNeb",	FIXED,		'K'},
    {"P Planetary",	"PlanetaryNeb",	FIXED,		'P'}
};

static FilterTB galaxies_f[] = {
    {"G Spiral",	"SpiralGal",	FIXED,		'G'},
    {"H Spherical",	"SphericalGal",	FIXED,		'H'},
    {"A Clusters",	"GalClusters",	FIXED,		'A'}
};

static FilterTB clusters_f[] = {
    {"C Globular",	"GlobularCl",	FIXED,		'C'},
    {"O Open",		"OpenCl",	FIXED,		'O'},
    {"U in Nebula",	"ClInNeb",	FIXED,		'U'}
};

static FilterCat filters[] = {
    {"Solar System:",	solsys_f,	XtNumber(solsys_f)},
    {"Clusters:",	clusters_f,	XtNumber(clusters_f)},
    {"Galaxies:",	galaxies_f,	XtNumber(galaxies_f)},
    {"Nebulae:",	nebulae_f,	XtNumber(nebulae_f)},
    {"Stars:",		stars_f,	XtNumber(stars_f)},
    {"Other:",		other_f,	XtNumber(other_f)}
};

/* these form rapid lookup tables for the state of an object.
 * they can be directly indexed by Obj.type and Obj.f.f_class, respectively.
 */
static char fclass_table[128];
static char type_table[10];

#define BMAGLIMIT       (-28)   /* brightest setting for the Mag scales */
#define FMAGLIMIT       20      /* faintest setting for the Mag scales */
static int fmag, bmag;		/* current values of faintest/brightest mag */
static Widget fmag_w, bmag_w;	/* faintest and brightest mag scales */

static Widget filter_w;		/* main filter dialog */

/* create, but do not manage, the filter dialog.
 * we also apply its state info immediately to the initial resource
 * settings of the toggle buttons since they are used to set up the real filter
 * right away.
 */
void
svf_create()
{
	static struct {
	    char *name;
	    void (*cb)();
	} ctls[] = {
	    {"Ok", svf_ok_cb},
	    {"Apply", svf_apply_cb},
	    {"All", svf_all_cb},
	    {"Toggle All", svf_toggle_cb},
	    {"Reset", svf_reset_cb},
	    {"Close", svf_close_cb}
	};
	Arg args[20];
	XmString str;
	Widget w;
	Widget f_w;
	Widget ctlf_w;
	Widget topsep_w, botsep_w;
	Widget hrc_w;
	int n;
	int i;

	/* create form */
	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNverticalSpacing, 4); n++;
	filter_w = XmCreateFormDialog (toplevel_w, "SkyFilter", args, n);
	XtAddCallback (filter_w, XmNmapCallback, prompt_map_cb, NULL);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Sky view Filter"); n++;
	XtSetValues (XtParent(filter_w), args, n);


	/* put the mag limits in a form to center them across the top */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNfractionBase, 9); n++;
	f_w = XmCreateForm (filter_w, "MagF", args, n);
	XtManageChild (f_w);

	    str = XmStringCreate ("Faintest Magnitude",
						XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNleftPosition, 1); n++;
	    XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNrightPosition, 4); n++;
	    XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	    XtSetArg (args[n], XmNminimum, BMAGLIMIT); n++;
	    XtSetArg (args[n], XmNmaximum, FMAGLIMIT); n++;
	    XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_LEFT); n++;
	    XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	    XtSetArg (args[n], XmNshowValue, True); n++;
	    XtSetArg (args[n], XmNtitleString, str); n++;
	    fmag_w = XmCreateScale (f_w, "FaintMagScale", args, n);
	    XtManageChild (fmag_w);
	    XmStringFree (str);

	    str = XmStringCreate ("Brightest Magnitude",
						XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNleftPosition, 5); n++;
	    XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNrightPosition, 8); n++;
	    XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	    XtSetArg (args[n], XmNminimum, BMAGLIMIT); n++;
	    XtSetArg (args[n], XmNmaximum, FMAGLIMIT); n++;
	    XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_LEFT); n++;
	    XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	    XtSetArg (args[n], XmNshowValue, True); n++;
	    XtSetArg (args[n], XmNtitleString, str); n++;
	    bmag_w = XmCreateScale (f_w, "BrightMagScale", args, n);
	    XtManageChild (bmag_w);
	    XmStringFree (str);

	/* make a separator below the mag scales */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, f_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	topsep_w = XmCreateSeparator (filter_w, "TopSep", args, n);
	XtManageChild (topsep_w);

	/* make a form for bottom control panel */

	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNfractionBase, 25); n++;
	XtSetArg (args[n], XmNverticalSpacing, 5); n++;
	ctlf_w = XmCreateForm (filter_w, "SVControlF", args, n);
	XtManageChild (ctlf_w);

	    /* add the control buttons */

	    for (i = 0; i < XtNumber(ctls); i++) {
		str = XmStringCreateLtoR(ctls[i].name,XmSTRING_DEFAULT_CHARSET);
		n = 0;
		XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
		XtSetArg (args[n], XmNleftPosition, 1+i*4); n++;
		XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
		XtSetArg (args[n], XmNrightPosition, 4+i*4); n++;
		XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNlabelString, str); n++;
		w = XmCreatePushButton (ctlf_w, "FilterCB", args, n);
		XtManageChild(w);
		XmStringFree (str);
		XtAddCallback (w, XmNactivateCallback, ctls[i].cb, NULL);
	    }

	/* put a separator on top of the control panel */

	n = 0;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, ctlf_w); n++;
	botsep_w = XmCreateSeparator (filter_w, "BotSep", args, n);
	XtManageChild (botsep_w);

	/* make a horizontal rowcol to contain each column */

	n = 0;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, botsep_w); n++;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, topsep_w); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
	XtSetArg (args[n], XmNspacing, 0); n++;
	XtSetArg (args[n], XmNmarginWidth, 0); n++;
	hrc_w = XmCreateRowColumn (filter_w, "CategoryTableRC", args, n);
	XtManageChild (hrc_w);

	/* fill the horizontal rc with each FilterCat */

	for (i = 0; i < XtNumber(filters); i++)
	    svf_create_filter (hrc_w, &filters[i]);

	/* set the real filter according to the state of the toggle buttons */
	svf_apply();
}

/* return 1 if we are now up, else 0.
 */
int
svf_ismanaged()
{

	return (XtIsManaged(filter_w));
}

/* called to manage the filter dialog.
 * whenever we are managed, reset the state of the toggle buttons back
 * to the way the real filter is.
 */
void
svf_manage()
{
	svf_reset();
	XtManageChild(filter_w);
}

/* called to manage the filter dialog.
 */
void
svf_unmanage()
{
	XtUnmanageChild(filter_w);
}

/* return the current faint and bright mag limits */
void
svf_getmaglimits (fmagp, bmagp)
int *fmagp, *bmagp;
{
	*fmagp = fmag;
	*bmagp = bmag;
}

/* set the current faint and bright mag limits */
void
svf_setmaglimits (f, b)
int f, b;
{
	fmag = f;
	bmag = b;
	XmScaleSetValue (fmag_w, fmag);
	XmScaleSetValue (bmag_w, bmag);
}

/* return 1 if p satisfies the stuff in the filter dialog, else 0.
 * N.B. because svf_apply() has to have already been called we do not need
 *   to range check the type and class.
 */
int
svf_filter_ok (op)
Obj *op;
{
	int t = op->type;
	int tok;

	if (t == FIXED)
	    tok = fclass_table[(int)op->f_class];
	else
	    tok = type_table[t];

	if (tok)
	    return (svf_bright_ok (op));
	return (0);
}

/* called to put up or remove the watch cursor.  */
void
svf_cursor (c)
Cursor c;
{
	Window win;

	if (filter_w && (win = XtWindow(filter_w))) {
	    Display *dsp = XtDisplay(filter_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}

/* create one FilterCat filter category in the given RowColumn */
static void
svf_create_filter (mainrc, fcp)
Widget mainrc;
FilterCat *fcp;
{
	Arg args[20];
	Widget l_w;
	Widget rc_w;	/* rc for the label and controls */
	Widget fr_w, da_w;
	Widget w;
	int n;
	int i;

	/* make a rc within a frame for the tb rowcol */

	n = 0;
	fr_w = XmCreateFrame (mainrc, "Frame", args, n);
	XtManageChild (fr_w);
	n = 0;
	XtSetArg (args[n], XmNmarginWidth, 0); n++;
	XtSetArg (args[n], XmNisAligned, False); n++;
	rc_w = XmCreateRowColumn (fr_w, "CategoryRC", args, n);
	XtManageChild (rc_w);

	/* make a label for the category */

	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	l_w = XmCreateLabel (rc_w, fcp->label, args, n);
	XtManageChild (l_w);

	/* make a pushbutton used to toggle all entries in this category */
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	w = XmCreatePushButton (rc_w, "Toggle", args, n);
	XtAddCallback (w, XmNactivateCallback, svf_cat_toggle_cb,
								(XtPointer)fcp);
	XtManageChild (w);

	/* add each filter */

	for (i = 0; i < fcp->nftb; i++) {
	    Widget f_w;
	    Widget tb_w;
	    FilterTB *ftbp = &fcp->ftb[i];
	    XmString str;

	    n = 0;
	    f_w = XmCreateForm (rc_w, "FTBF", args, n);
	    XtManageChild (f_w);

		/* create a frame around a drawing area in which to show the
		 * object symbol
		 */
		n = 0;
		XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNshadowType, XmSHADOW_ETCHED_OUT); n++;
		fr_w = XmCreateFrame (f_w, "SymbolF", args, n);
		XtManageChild (fr_w);
		n = 0;
		da_w = XmCreateDrawingArea (fr_w, "SymbolDA", args, n);
		XtAddCallback (da_w, XmNexposeCallback, svf_da_cb,
							    (XtPointer)ftbp);
		XtManageChild (da_w);

		/* create the filter selection toggle button */

		str = XmStringCreateLtoR (ftbp->label,XmSTRING_DEFAULT_CHARSET);
		n = 0;
		XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
		XtSetArg (args[n], XmNleftWidget, fr_w); n++;
		XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNlabelString, str); n++;
		XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
		tb_w = XmCreateToggleButton (f_w, ftbp->name, args, n);
		XmStringFree(str);
		XtManageChild (tb_w);
		ftbp->tbw = tb_w;
	}
}

/* set the toggle button states according to the real filter states.
 * set the mag scales from the real mag limits.
 */
static void
svf_reset()
{
	int i, j;

	for (i = 0; i < XtNumber(filters); i++) {
	    FilterCat *fcp = &filters[i];
	    for (j = 0; j < fcp->nftb; j++) {
		FilterTB *ftbp = &fcp->ftb[j];
		XmToggleButtonSetState (ftbp->tbw, ftbp->state, True);
	    }
	}

	svf_setmaglimits (fmag, bmag);
}

/* set the filter according to the present state of the toggle buttons.
 * also set the mag limits from the scales.
 */
static void
svf_apply()
{
	int i, j;

	for (i = 0; i < XtNumber(filters); i++) {
	    FilterCat *fcp = &filters[i];
	    for (j = 0; j < fcp->nftb; j++) {
		FilterTB *ftbp = &fcp->ftb[j];
		int t = ftbp->type;
		if (t < 0 || t >= XtNumber(type_table)) {
		    printf ("svf_apply: type out of range: %d\n", t);
		    exit (1);
		}
		ftbp->state = XmToggleButtonGetState (ftbp->tbw);
		if (ftbp->type == FIXED) {
		    int c = ftbp->class;
		    if (c < 0 || c >= XtNumber(fclass_table)) {
			printf ("svf_apply: FIXED class out of range: %d\n", c);
			exit (1);
		    }
		    fclass_table[c] = ftbp->state;
		} else
		    type_table[t] = ftbp->state;
	    }
	}

	XmScaleGetValue (fmag_w, &fmag);
	XmScaleGetValue (bmag_w, &bmag);
}

/* return 1 if db object is ever possibly within the fmag/bmag range; else 0.
 */
static int
svf_bright_ok(op)
Obj *op;
{
	switch (op->type) {
	case PLANET:
	    /* always go for the planets for now but ..
	     * TODO: work up a table of extreme planet magnitudes.
	     */
	    return (1);
	    /* break; */
	case HYPERBOLIC: return (1);	/* and interlopers */
	case PARABOLIC: return (1);
	case ELLIPTICAL: {
	    double mag;		/* magnitude */
	    double per, aph;	/* perihelion and aphelion distance */

	    per = op->e_a*(1.0 - op->e_e);
	    aph = op->e_a*(1.0 + op->e_e);
	    if (per <= 1.1 && aph >= 0.9)
		return (1); /* might be blazing in the back yard some day */
	    if (op->e_mag.whichm == MAG_HG)
		 mag = op->e_mag.m1 + 5*log10(per*fabs(per-1.0));
	    else
		 gk_mag(op->e_mag.m1, op->e_mag.m2,
						per, fabs(per-1.0), &mag);
	    return (mag <= fmag && mag >= bmag);
	    /* break; */
	    }
	case FIXED:
	    return (op->f_mag/MAGSCALE <= fmag && op->f_mag/MAGSCALE >= bmag);
	    /* break; */
	case EARTHSAT:
	    /* TODO: work on satellite magnitudes someday */
	    return (1);
	default: 
	    printf ("sv_bright_ok(): bad type: %d\n", op->type);
	    exit (1);
	    return (0);	/* for lint */
	}
}

/* called when a symbol drawing area is exposed.
 * client is a pointer to the FilterTB.
 */
/* ARGSUSED */
static void
svf_da_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;
	FilterTB *ftbp = (FilterTB *) client;

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
	    }
	    break;
	default:
	    return;
	}

	svf_draw_symbol(ftbp, w);
}

/* draw the symbol used for the object described by ftbp into the
 * DrawingArea widget daw.
 */
static void
svf_draw_symbol(ftbp, daw)
FilterTB *ftbp;
Widget daw;
{
	static GC gc;
	Display *dsp = XtDisplay(daw);
	Window win = XtWindow(daw);
	Obj obj;
	Dimension w, h;
	int diam;

	obj.type = ftbp->type;
	if (ftbp->type == FIXED)
	    obj.f_class = ftbp->class;

	get_something (daw, XmNwidth, (XtArgVal)&w);
	get_something (daw, XmNheight, (XtArgVal)&h);
	diam = (w > h ? h : w) - 2; /* allow for a small border */

	if (!gc) {
	    XGCValues gcv;
	    unsigned long gcm;
	    Pixel p;

	    gcm = GCForeground;
	    get_something (daw, XmNforeground, (XtArgVal)&p);
	    gcv.foreground = p;
	    gc = XCreateGC (dsp, win, gcm, &gcv);
	}

	sv_draw_obj (dsp, win, gc, &obj, (int)w/2-1, (int)h/2-1, diam, 0);
	sv_draw_obj (dsp, win, gc, NULL, 0, 0, 0, 0);	/* flush */
}

/* called when Apply is pushed on the filter dialog */
/* ARGSUSED */
static void
svf_apply_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	svf_apply();
	sv_all(mm_get_now());
}

/* called when Ok is pushed on the filter dialog */
/* ARGSUSED */
static void
svf_ok_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	svf_apply();
	sv_all(mm_get_now());
	XtUnmanageChild (filter_w);
}

/* called when Close is pushed on the filter dialog */
/* ARGSUSED */
static void
svf_close_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (filter_w);
}

/* called when Toggle is pushed on the filter dialog */
/* ARGSUSED */
static void
svf_toggle_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int i, j;

	for (i = 0; i < XtNumber(filters); i++) {
	    FilterCat *fcp = &filters[i];
	    for (j = 0; j < fcp->nftb; j++) {
		FilterTB *ftbp = &fcp->ftb[j];
		XmToggleButtonSetState (ftbp->tbw, 
		    !XmToggleButtonGetState(ftbp->tbw), True);
	    }
	}
}

/* called when All is pushed on the filter dialog */
/* ARGSUSED */
static void
svf_all_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int i, j;

	for (i = 0; i < XtNumber(filters); i++) {
	    FilterCat *fcp = &filters[i];
	    for (j = 0; j < fcp->nftb; j++) {
		FilterTB *ftbp = &fcp->ftb[j];
		XmToggleButtonSetState (ftbp->tbw, True, True);
	    }
	}
}

/* called when Reset is pushed on the filter dialog */
/* ARGSUSED */
static void
svf_reset_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	svf_reset();
}

/* called when the "Toggle" button is activated on a particular category.
 * client is the FilterCat pointer.
 */
/* ARGSUSED */
static void
svf_cat_toggle_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	FilterCat *fcp = (FilterCat *) client;
	int i;

	for (i = 0; i < fcp->nftb; i++) {
	    FilterTB *ftbp = &fcp->ftb[i];
	    XmToggleButtonSetState (ftbp->tbw, 
		!XmToggleButtonGetState(ftbp->tbw), True);
	}
}
