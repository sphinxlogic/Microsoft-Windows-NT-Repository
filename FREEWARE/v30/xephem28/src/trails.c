/* code to handle setting up and generating trails.
 *
 * to use: let user define a trail with tr_setup().
 * when user hits Ok we calls your function and you build the actual trails
 *   and do the drawing with tr_draw().
 *
 * ok to have several of these at once; they each maintain their own context.
 */

#include <stdio.h>
#include <math.h>

#if defined(__STDC__)
#include <stdlib.h>
#endif
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/SelectioB.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "trails.h"
#include "ps.h"

extern Widget toplevel_w;

extern Now *mm_get_now P_((void));
extern void f_scansex P_((double o, char bp[], double *np));
extern void fs_date P_((char out[], double jd));
extern void fs_mtime P_((char out[], double t));
extern void fs_pangle P_((char buf[], double a));
extern void fs_ra P_((char buf[], double ra));
extern void fs_sexa P_((char *out, double a, int w, int fracbase));
extern void fs_time P_((char out[], double t));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void get_xmstring P_((Widget w, char *resource, char **txtp));
extern void prompt_map_cb P_((Widget w, XtPointer client, XtPointer call));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *txtp));
extern void xe_msg P_((char *msg, int app_modal));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));

#define	SMALL_SIZE	4	/* small char width, pixels */
#define	MEDIUM_SIZE	5	/* medium char width, pixels */
#define	LARGE_SIZE	8	/* large char width, pixels */
#define	HUGE_SIZE	12	/* huge char width, pixels */
#define	GAP		4	/* interchar-gap, pixels */

/* context information per dialog.
 * one of these gets malloced and saved in userData of each SelectionBox.
 */
typedef struct {
    Widget p_w;			/* the overall prompt dialog (SelBox actually)*/
    Widget l_w[TRLR_N];		/* TBs for time stamp label rate options */
    Widget i_w[TRI_N];		/* TBs for interval options */
    Widget f_w[TRF_N];		/* TBs for format options */
    Widget r_w[TRR_N];		/* TBs for the rounding options */
    Widget s_w[TRS_N];		/* TBs for the size options */
    Widget o_w[TRO_N];		/* TBs for the orientation options */
    Widget custom_w;		/* TextField for custom interval */
    TrCB cb;			/* saved caller's callback function */
    XtPointer client;		/* saved in tr_setup() and sent back in callbk*/
} TrContext;

/* make an array of these to create a row/col for the options */
typedef struct {
    char *name;			/* widget instance name */
    char *label;		/* widget's label */
} RCOption;

/* N.B. must correspond to the entries in TrLR */
static RCOption l_options[TRLR_N] = {
    {"every1",	"Every 1"},
    {"every2",	"Every 2"},
    {"every5",	"Every 5"},
    {"every10",	"Every 10"},
    {"fl",	"First+Last"},
    {"ml",	"Mid+Last"},
    {"fml",	"F+M+L"},
    {"none",	"None"},
};

/* N.B. must correspond to the entries in TrInt */
static RCOption i_options[TRI_N] = {
    {"5mins",	"5 Minutes"},
    {"hour",	"1 Hour"},
    {"day",	"1 Day"},
    {"week",	"1 Week"},
    {"month",	"1 Month"},
    {"year",	"1 Year"},
    {"custom",	"Custom (H:M)"},
};

/* N.B. must correspond to the entries in TrFormat */
static RCOption f_options[TRF_N] = {
    {"time",	"H:M"},
    {"date",	"Date"},
};

/* N.B. must correspond to the entries in TrRound */
static RCOption r_options[TRR_N] = {
    {"min",	"Whole min"},
    {"day",	"Whole day"},
    {"whole",	"Whole interval"},
    {"now",	"Now"},
};

/* N.B. must correspond to the entries in TrOrient */
static RCOption o_options[TRO_N] = {
    {"up",	"Up"},
    {"down",	"Down"},
    {"left",	"Left"},
    {"right",	"Right"},
    {"above",	"Above"},
    {"below",	"Below"},
    {"upr",	"Up 45"},
    {"downr",	"Down 45"},
    {"pathl",	"Path-Left"},
    {"pathr",	"Path-Right"},
};

/* N.B. must correspond to the entries in TrSize */
static RCOption s_options[TRS_N] = {
    {"small",	"Small"},
    {"medium",	"Medium"},
    {"large",	"Large"},
    {"huge",	"Huge"},
};

static Widget create_form P_((char *title, char *hdr, TrState *init, TrCB cb,
    XtPointer client));
static void make_rb P_((Widget rc_w, char *title, char *name, RCOption *op,
    int nop, Widget savew[]));
static void customint_cb P_((Widget w, XtPointer client, XtPointer call));
static void ok_cb P_((Widget w, XtPointer client, XtPointer call));
static int get_options P_((TrContext *tcp, TrState *statep));
static void unmap_cb P_((Widget w, XtPointer client, XtPointer call));
static void destroy_cb P_((Widget w, XtPointer client, XtPointer call));
static void help_cb P_((Widget w, XtPointer client, XtPointer call));
static int draw_stamp P_((TrTS *tp, TrState *sp, int ticklen, int lx, int ly,
    int x, int y, XSegment *xsp));
static int draw_tickmark P_((int ticklen, int x1, int y1, int x2, int y2,
    XSegment *xsp));
static int draw_char P_((int x, int w, int c, XSegment *xsp));
static void xform_segs P_((int x, int y, double angle, XSegment *xsp,int nxs));


/* put up a dialog to ask how to make a trail. OK will call (*cb)().
 * ok to have several at once -- they keep their own context in userData.
 */
void
tr_setup(title, hdr, init, cb, client)
char *title;		/* window manager title */
char *hdr;		/* label across top */
TrState *init;		/* default setup */
TrCB cb;		/* callback when/if user hits Ok */
XtPointer client;	/* saved and passed back to (*cb)() */
{
	static char me[] = "tr_setup()";
	Widget w;

	/* do some sanity checks */
	if (init->l >= TRLR_N) {
	    printf ("%s: Bogus label rate: %d\n", me, init->l);
	    exit (1);
	}
	if (init->i >= TRI_N) {
	    printf ("%s: Bogus initerval: %d\n", me, init->i);
	    exit (1);
	}
	if (init->f >= TRF_N) {
	    printf ("%s: Bogus format: %d\n", me, init->f);
	    exit (1);
	}
	if (init->r >= TRR_N) {
	    printf ("%s: Bogus round: %d\n", me, init->r);
	    exit (1);
	}
	if (init->o >= TRO_N) {
	    printf ("%s: Bogus orientation: %d\n", me, init->o);
	    exit (1);
	}
	if (init->s >= TRS_N) {
	    printf ("%s: Bogus size: %d\n", me, init->s);
	    exit (1);
	}
	if (!cb) {
	    printf ("%s: No cb()\n", me);
	    exit (1);
	}

	/* ok, go for it */
	w = create_form (title, hdr, init, cb, client);
	if (w)
	    XtManageChild (w);
}

/* draw a line from [lx,ly] to [x,y]. iff tp, draw a perpendicular tickmark of
 * size ticklen pixels and draw a time stamp according to tp and sp at [x,y].
 * iff ltp, do the same according to ltp and sp at [lx,ly].
 */
void tr_draw (dsp, win, gc, ticklen, tp, ltp, sp, lx, ly, x, y)
Display *dsp;
Drawable win;
GC gc;
int ticklen;
TrTS *tp;
TrTS *ltp;
TrState *sp;
int lx, ly;
int x, y;
{
	XSegment xs[30*7+10];	/* a pool of XSegments "big enough" */
	XSegment *xsp = xs;

	/* the base line */
	xsp->x1 = lx;  xsp->y1 = ly;  xsp->x2 = x;  xsp->y2 = y; xsp++;

	/* if tp, draw tick mark and stamp at [x,y] */
	if (tp) {
	    xsp += draw_tickmark (ticklen, lx, ly, x, y, xsp);
	    xsp += draw_stamp (tp, sp, ticklen, lx, ly, x, y, xsp);
	}

	/* same for other end if ltp */
	if (ltp) {
	    xsp += draw_tickmark (ticklen, x, y, lx, ly, xsp);
	    xsp += draw_stamp (ltp, sp, ticklen, x-2*(x-lx), y-2*(y-ly),
	    							lx, ly, xsp);
	}

	if (xsp >= &xs[XtNumber(xs)]) {
	    printf ("tr_draw(): too many segments\n");
	    exit (1);	/* stack is blown anyway */
	}

	/* phew! */
	XPSDrawSegments (dsp, win, gc, xs, xsp-xs);
}

/* create a trail control dialog,
 * a pointer to malloc'd context is kept in userData resource. be sure to free
 * this whenever the dialog is destroyed.
 */
static Widget
create_form (title, hdr, init, cb, client)
char *title;
char *hdr;
TrState *init;
TrCB cb;
XtPointer client;
{
	TrContext *tcp;
	Widget t_w, hdr_w, rc_w, f_w, fr_w;
	Widget ofr_w, ifr_w, lrfr_w;
	Widget w;
	XmString str;
	char buf[64];
	Arg args[20];
	int n;

	/* make the context */
	tcp = (TrContext *) malloc (sizeof(TrContext));
	if (!tcp) {
	    xe_msg ("Can not malloc for trail setup context.", 1);
	    return (NULL);
	}

	/* save caller's callback function and client data */
	tcp->cb = cb;
	tcp->client = client;

	/* make the general dialog */
	str = XmStringCreateSimple (title);
	n = 0;
	XtSetArg(args[n], XmNdialogTitle, str);  n++;
	XtSetArg(args[n], XmNdefaultPosition, False);  n++;
	t_w = XmCreatePromptDialog(toplevel_w, "TrailPrompt", args, n);
	XmStringFree (str);
	set_xmstring (t_w, XmNselectionLabelString, "Number of tick marks:");
	XtAddCallback (t_w, XmNmapCallback, prompt_map_cb, NULL);
	XtAddCallback (t_w, XmNunmapCallback, unmap_cb, NULL);
	XtAddCallback (t_w, XmNdestroyCallback, destroy_cb, NULL);
	XtAddCallback (t_w, XmNhelpCallback, help_cb, NULL);
	XtAddCallback (t_w, XmNokCallback, ok_cb, (XtPointer)tcp);

	/* set desired default tick mark count */

	(void) sprintf (buf, "%d", init->nticks);
	set_xmstring (t_w, XmNtextString, buf);

	/* save overall widget id */
	tcp->p_w = t_w;

	/* also store tcp in the prompt dialog -- freed when it's destroyed */
	set_something (t_w, XmNuserData, (XtArgVal)tcp);

	/* enable Apply button -- same callback as Ok because of autoUnmanage */
	XtManageChild (XmSelectionBoxGetChild(t_w, XmDIALOG_APPLY_BUTTON));
	XtAddCallback (t_w, XmNapplyCallback, ok_cb, (XtPointer)tcp);

	/* prefer Close to Cancel */
	set_xmstring (XmSelectionBoxGetChild(t_w, XmDIALOG_CANCEL_BUTTON),
						    XmNlabelString, "Close");

	/* add our stuff at the top in a form */
	n = 0;
	XtSetArg (args[n], XmNhorizontalSpacing, 5); n++;
	XtSetArg (args[n], XmNverticalSpacing, 10); n++;
	XtSetArg (args[n], XmNmarginHeight, 10); n++;
	f_w = XmCreateForm (t_w, "TF1", args, n);
	XtManageChild (f_w);

	/* make the header label across the top */
	str = XmStringCreateSimple (hdr);
	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	XtSetArg (args[n], XmNlabelString, str); n++;
	hdr_w = XmCreateLabel (f_w, "Header", args, n);
	XtManageChild (hdr_w);
	XmStringFree (str);


	/* make a radio box for orientations in a frame. */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, hdr_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 2); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 33); n++;
	ofr_w = XmCreateFrame (f_w, "OFR", args, n);
	XtManageChild (ofr_w);

	    n = 0;
	    XtSetArg (args[n], XmNadjustMargin, False); n++;
	    rc_w = XmCreateRowColumn (ofr_w, "ORC", args, n);
	    XtManageChild (rc_w);

	    /* make rb for orientation options */
	    make_rb(rc_w, "Orientation:", "Orientation", o_options, TRO_N,
								    tcp->o_w);

	    /* set desired default orientation */
	    XmToggleButtonSetState (tcp->o_w[init->o], True, True);


 	/* make a radio box for intervals in a frame.
	 * also put the custom TF and "whole" tb in the rc.
	 */
	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, hdr_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 35); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 66); n++;
	ifr_w = XmCreateFrame (f_w, "IFR", args, n);
	XtManageChild (ifr_w);

	    n = 0;
	    XtSetArg (args[n], XmNadjustMargin, False); n++;
	    rc_w = XmCreateRowColumn (ifr_w, "IRC", args, n);
	    XtManageChild (rc_w);

	    /* make rb for interval options */
	    make_rb (rc_w, "Interval:", "Interval", i_options, TRI_N, tcp->i_w);

	    /* set desired default interval */
	    XmToggleButtonSetState (tcp->i_w[init->i], True, True);

	    /* add the TextField for entering custom interval,
	     * add callback to custom to let it change TextField sense,
	     * and init TF sens here now.
	     */
	    n = 0;
	    XtSetArg (args[n], XmNcolumns, 10); n++;
	    w = XmCreateTextField (rc_w, "CustomInterval", args, n);
	    XtManageChild (w);
	    tcp->custom_w = w;
	    XtSetSensitive (w, XmToggleButtonGetState(tcp->i_w[TRI_CUSTOM]));
	    XtAddCallback (tcp->i_w[TRI_CUSTOM], XmNvalueChangedCallback,
						customint_cb, tcp->custom_w);
	    if (init->i == TRI_CUSTOM) {
		fs_sexa (buf, init->customi*24.0, 4, 60);
		XmTextFieldSetString (tcp->custom_w, buf);
	    }

 	/* make a radio box for label rates in a frame. */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, hdr_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 68); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 98); n++;
	lrfr_w = XmCreateFrame (f_w, "LRFR", args, n);
	XtManageChild (lrfr_w);

	    n = 0;
	    XtSetArg (args[n], XmNadjustMargin, False); n++;
	    rc_w = XmCreateRowColumn (lrfr_w, "LRRC", args, n);
	    XtManageChild (rc_w);

	    /* make rb for label rates options */
	    make_rb(rc_w, "Time stamp:", "TimeStamp",l_options,TRLR_N,tcp->l_w);

	    /* set desired default label rate */
	    XmToggleButtonSetState (tcp->l_w[init->l], True, True);



 	/* make a radio box for format options in a frame */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, ofr_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNbottomPosition, 95); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 2); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 33); n++;
	fr_w = XmCreateFrame (f_w, "FFR", args, n);
	XtManageChild (fr_w);

	    n = 0;
	    XtSetArg (args[n], XmNadjustMargin, False); n++;
	    rc_w = XmCreateRowColumn (fr_w, "FRC", args, n);
	    XtManageChild (rc_w);

	    /* make rb for format options */
	    make_rb (rc_w, "Format:", "Format", f_options, TRF_N, tcp->f_w);

	    /* set desired default format */
	    XmToggleButtonSetState (tcp->f_w[init->f], True, True);

 	/* make a radio box for size options in a frame */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, ifr_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNbottomPosition, 95); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 35); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 66); n++;
	fr_w = XmCreateFrame (f_w, "SFR", args, n);
	XtManageChild (fr_w);

	    n = 0;
	    XtSetArg (args[n], XmNadjustMargin, False); n++;
	    rc_w = XmCreateRowColumn (fr_w, "SRC", args, n);
	    XtManageChild (rc_w);

	    /* make rb for size options */
	    make_rb (rc_w, "Size:", "Size", s_options, TRS_N, tcp->s_w);

	    /* set desired default rounding */
	    XmToggleButtonSetState (tcp->s_w[init->s], True, True);


 	/* make a radio box for rounding options in a frame */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, lrfr_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNbottomPosition, 95); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 68); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 98); n++;
	fr_w = XmCreateFrame (f_w, "RFR", args, n);
	XtManageChild (fr_w);

	    n = 0;
	    XtSetArg (args[n], XmNadjustMargin, False); n++;
	    rc_w = XmCreateRowColumn (fr_w, "RRC", args, n);
	    XtManageChild (rc_w);

	    /* make rb for rounding options */
	    make_rb (rc_w, "Start:", "Start", r_options, TRR_N, tcp->r_w);

	    /* set desired default rounding */
	    XmToggleButtonSetState (tcp->r_w[init->r], True, True);


	return (t_w);
}

/* make a title and radio box inside rc.
 * save each toggle button widget in savew[] array.
 * would use frame's new title child but don't want to lock into 1.2 (yet)
 */
static void
make_rb (rc_w, title, name, op, nop, savew)
Widget rc_w;
char *title;
char *name;
RCOption *op;
int nop;
Widget savew[];
{
	Widget l_w, rb_w, w;
	XmString str;
	Arg args[20];
	int n;
	int i;

	str = XmStringCreateSimple (title);
	n = 0;
	XtSetArg (args[n], XmNlabelString, str); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	l_w = XmCreateLabel (rc_w, "L", args, n);
	XtManageChild (l_w);
	XmStringFree (str);

	n = 0;
	rb_w = XmCreateRadioBox (rc_w, name, args, n);
	XtManageChild (rb_w);

	    for (i = 0; i < nop; i++, op++) {
		str = XmStringCreateSimple (op->label);
		n = 0;
		XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
		XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY); n++;
		XtSetArg (args[n], XmNlabelString, str); n++;
		w = XmCreateToggleButton (rb_w, op->name, args, n);
		XtManageChild (w);
		XmStringFree (str);
		savew[i] = w;
	    }
}

/* called when the Custom time interval TB changes.
 * client is the TextField widget used to enter custom intervals.
 */
/* ARGSUSED */
static void
customint_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtSetSensitive ((Widget)client, XmToggleButtonGetState(w));
}

/* called when the Ok or Apply button is hit.
 * client is pointer to TrContext.
 * pull out desired trail parameters then call the callback function.
 * rely on autoUnmanage to handle overall dialog mapping.
 */
/* ARGSUSED */
static void
ok_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	Now *np = mm_get_now();
	TrContext *tcp;
	TrState state;
	TrTS *ttp;
	int m, y;
	double d;
	int dow;
	double t;
	int cbret;
	int i;

	/* fetch the TrContext */
	tcp = (TrContext *)client;

	/* gather trail configuration */
	if (get_options(tcp, &state) < 0)
	    return;

	/* get memory for list of time stamps */
	ttp = (TrTS *) malloc (state.nticks * sizeof(TrTS));
	if (!ttp) {
	    xe_msg ("Can not malloc tickmarks array.", 1);
	    return;
	}

	/* set initial time, t, according to rounding options.
	 */
	switch (state.r) {
	case TRR_MIN:
	    t = ceil(mjd*24*60)/(24*60);
	    break;

	case TRR_DAY:
	    t = ceil(mjd-0.5)+0.5;
	    break;

	case TRR_INTER:
	    switch (state.i) {
	    case TRI_5MIN:
		t = ceil(mjd*24*(60/5))/(24*(60/5));
		break;
	    case TRI_HOUR:
		t = ceil(mjd*24)/24;
		break;
	    case TRI_DAY:
		t = ceil(mjd-0.5)+0.5;
		break;
	    case TRI_WEEK:
		t = mjd_day (mjd);
		while (mjd_dow (t, &dow) == 0 && dow != 0)
		    t += 1;
		break;
	    case TRI_MONTH:
		mjd_cal (mjd, &m, &d, &y);
		d = 1;
		if (++m > 12) {
		    m = 1;
		    y += 1;
		}
		cal_mjd (m, d, y, &t);
		break;
	    case TRI_YEAR:
		mjd_cal (t, &m, &d, &y);
		d = 1;
		m = 1;
		y++;
		cal_mjd (m, d, y, &t);
		break;
	    case TRI_CUSTOM:
		/* round to next nearest multiple of custom */
		t = ceil(mjd*(1.0/state.customi))/(1.0/state.customi);
		break;
	    default:
		break;
	    }
	    break;

	case TRR_NONE:
	    t = mjd;
	    break;

	default:
	    printf ("Bogus trail rounding code: %d\n", state.r);
	    exit (1);
	}

	/* now fill in the time stamp times and formats */
	for (i = 0; i < state.nticks; i++) {
	    TrTS *tp = &ttp[i];

	    /* time is just t */
	    tp->t = t;

	    /* determine whether to stamp this tickmark */
	    switch (state.l) {
	    case TRLR_1:
		tp->lbl = 1;
		break;
	    case TRLR_2:
		tp->lbl = !(i % 2);
		break;
	    case TRLR_5:
		tp->lbl = !(i % 5);
		break;
	    case TRLR_10:
		tp->lbl = !(i % 10);
		break;
	    case TRLR_FL:
		tp->lbl = (i == 0 || i == state.nticks-1);
		break;
	    case TRLR_ML:
		tp->lbl = (i == state.nticks/2 || i == state.nticks-1);
		break;
	    case TRLR_FML:
		tp->lbl = (i==0 || i==state.nticks/2 || i==state.nticks-1);
		break;
	    case TRLR_NONE:
		tp->lbl = 0;
		break;
	    default:
		printf ("Bogus trail label rate code=%d\n", state.l);
		exit (1);
	    }

	    /* advance time */
	    switch (state.i) {
	    case TRI_CUSTOM: t += state.customi;	break;
	    case TRI_5MIN:   t += 5./(24.*60.);		break;
	    case TRI_HOUR:   t += 1./24.;		break;
	    case TRI_DAY:    t += 1.0;			break;
	    case TRI_WEEK:   t += 7.0;			break;
	    case TRI_MONTH:
		mjd_cal (t, &m, &d, &y);
		if (++m > 12) {
		    m = 1;
		    y += 1;
		}
		cal_mjd (m, d, y, &t);
		break;
	    case TRI_YEAR:
		mjd_cal (t, &m, &d, &y);
		y += 1;
		cal_mjd (m, d, y, &t);
		break;
	    default:
		/* already checked default in prior switch() */
		break;
	    }
	}

	/* now call the users callback */
	cbret = (*tcp->cb) (ttp, &state, tcp->client);

	/* clean up what we malloced then upmap the dialog (which will
	 * free the TrContext and destroy the whole dialog).
	 */
	free ((char *)ttp);


	/* thanks to autoUnmanage, the dialog will close if this is Ok, stay
	 * up if Apply. all we have to do it close if there was something bad.
	 */
	if (cbret < 0)
	    XtUnmanageChild (w);
}

/* pull the options from tcp.
 * return 0 if ok, else xe_msg() wny not and return -1.
 */
static int
get_options(tcp, statep)
TrContext *tcp;
TrState *statep;
{
	char *str;
	int i;

	/* find label rate */
	i = 0;
	for (i = 0; i < TRLR_N; i++) {
	    if (XmToggleButtonGetState(tcp->l_w[i]))
		break;
	}
	if (i == TRLR_N) {
	    xe_msg ("Please select a tick mark label rate.", 1);
	    return(-1);
	}
	statep->l = (TrLR)i;

		
	/* find interval -- might be custom */
	i = 0;
	for (i = 0; i < TRI_N; i++) {
	    if (XmToggleButtonGetState(tcp->i_w[i]))
		break;
	}
	if (i == TRI_N) {
	    xe_msg ("Please select a tick mark interval.", 1);
	    return(-1);
	}
	statep->i = (TrInt)i;
	if (statep->i == TRI_CUSTOM) {
	    double d;

	    str = XmTextFieldGetString (tcp->custom_w);
	    f_scansex (0.0, str, &d);
	    XtFree (str);
	    if (d == 0.0) {
		xe_msg ("Please specify a non-zero custom interval.", 1);
		return(-1);
	    }

	    statep->customi = d/24.0;	/* hours to days */
	}


	/* find rounding mode */
	i = 0;
	for (i = 0; i < TRR_N; i++) {
	    if (XmToggleButtonGetState(tcp->r_w[i]))
		break;
	}
	if (i == TRR_N) {
	    xe_msg ("Please select a rounding mode.", 1);
	    return(-1);
	}
	statep->r = (TrRound)i;


	/* find format mode */
	i = 0;
	for (i = 0; i < TRF_N; i++) {
	    if (XmToggleButtonGetState(tcp->f_w[i]))
		break;
	}
	if (i == TRF_N) {
	    xe_msg ("Please select a format mode.", 1);
	    return(-1);
	}
	statep->f = (TrFormat)i;

	/* find size */
	i = 0;
	for (i = 0; i < TRS_N; i++) {
	    if (XmToggleButtonGetState(tcp->s_w[i]))
		break;
	}
	if (i == TRS_N) {
	    xe_msg ("Please select a size.", 1);
	    return(-1);
	}
	statep->s = (TrSize)i;

	/* find orientation */
	i = 0;
	for (i = 0; i < TRO_N; i++) {
	    if (XmToggleButtonGetState(tcp->o_w[i]))
		break;
	}
	if (i == TRO_N) {
	    xe_msg ("Please select an orientation.", 1);
	    return(-1);
	}
	statep->o = (TrOrient)i;


	/* get number of tickmarks */
	get_xmstring(tcp->p_w, XmNtextString, &str);
	i = atoi (str);
	XtFree (str);
	if (i < 2) {
	    xe_msg ("Please specify at least 2 tick marks.", 1);
	    return(-1);
	}
	statep->nticks = i;


	/* ok */
	return (0);
}

/* called when unmapped */
/* ARGSUSED */
static void
unmap_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	/* N.B. don't free the userData here -- wait for destroy.
	 * turns out unmap callbacks run before ok_cb!
	 */
	XtDestroyWidget (w);
}

/* called when destroyed */
/* ARGSUSED */
static void
destroy_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtPointer ud;

	get_something (w, XmNuserData, (XtArgVal)&ud);
	if (ud)	/* just paranoid */
	    free ((char *)ud);
}

/* called when the Help button is hit */
/* ARGSUSED */
static void
help_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
        hlp_dialog ("Trails", NULL, 0);
}

/* fill xsp with a line perpendicular to [x1,y1]-[x2,y2] at [x2,y2].
 * return 1 if really added segment else 0.
 */
static int
draw_tickmark (ticklen, x1, y1, x2, y2, xsp)
int ticklen;
int x1, y1;
int x2, y2;
XSegment *xsp;
{
	int dx = x2-x1;
	int dy = y2-y1;
	int dxt, dyt;

	if (ticklen == 0)
	    return (0);

	if (dx == 0 && dy == 0) {
	    /* no base line -- vertical default */
	    dxt = 0;
	    dyt = ticklen;
	} else if (abs(dx) > abs(dy)) {
	    /* base line is more horizontal than vertical */
	    dxt = ticklen*dy/dx;
	    dyt = ticklen;
	} else {
	    /* base line is more vertical than horizontal */
	    dxt = ticklen;
	    dyt = ticklen*dx/dy;
	}

	xsp->x1 = x2-dxt;
	xsp->y1 = y2+dyt;
	xsp->x2 = x2+dxt;
	xsp->y2 = y2-dyt;

	return (1);
}

/* draw a timestamp for the given trail object at x, y.
 * return number of segments used.
 */
static int
draw_stamp (tp, sp, ticklen, lx, ly, x, y, xsp)
TrTS *tp;
TrState *sp;
int ticklen;
int lx, ly;
int x, y;
XSegment xsp[];
{
	XSegment *xsp_orig = xsp;
	char buf[64], *bp;
	double a = 0, ca, sa;
	int w = 0;
	int i, l;
	int nxs;

	/* do nothing if no labeling at all or none for this stamp */
	if (sp->l == TRLR_NONE || !tp->lbl)
	    return(0);

	/* establish character width */
	switch (sp->s) {
	case TRS_SMALL:	 w = SMALL_SIZE;  break;
	case TRS_MEDIUM: w = MEDIUM_SIZE; break;
	case TRS_LARGE:	 w = LARGE_SIZE;  break;
	case TRS_HUGE:	 w = HUGE_SIZE;   break;
	case TRS_N:                       break;
	}

	/* compute the segments depending on format */
	switch (sp->f) {
	case TRF_TIME:
	    fs_mtime (buf, mjd_hr(tp->t));
	    break;
	case TRF_DATE:
	    fs_date (buf, tp->t);
	    break;
	case TRF_N:
	    break;
	}
	for (bp = buf; *bp == ' '; bp++)
	    continue;
	l = strlen(bp);
	for (i = 0; i < l; i++)
	    xsp += draw_char ((w+GAP)*i, w, bp[i], xsp);
	nxs = xsp - xsp_orig;

	/* make l the string length in pixels */
	l *= w+GAP;

	/* set up x/y/a to rotate and translate into position.
	 * one rule is to never ask the user to look upside down even a little.
	 */
	switch (sp->o) {
	case TRO_UP:
	    x += w; y -= ticklen + GAP; a = 90;
	    break;

	case TRO_DOWN:
	    x -= w; y += ticklen + GAP; a = -90;
	    break;

	case TRO_LEFT:
	    x -= l + ticklen; y += w; a = 0;
	    break;

	case TRO_RIGHT:
	    x += ticklen + GAP; y += w; a = 0;
	    break;

	case TRO_ABOVE:
	    x -= (l-GAP)/2; y -= ticklen + GAP; a = 0;
	    break;

	case TRO_BELOW:
	    x -= (l-GAP)/2; y += ticklen + GAP + 2*w; a = 0;
	    break;

	case TRO_UPR:
	    x += w; y -= w; a = 45;
	    break;

	case TRO_DOWNR:
	    x -= w; y += 2*w + GAP; a = -45;
	    break;

	case TRO_PATHL:
	    if (ly-y == 0 && x-lx == 0)
		a = 0.0;
	    else
		a = atan2((double)(ly-y),(double)(x-lx));	/* -PI .. +PI */
	    ca = cos(a);
	    sa = sin(a);
	    a = raddeg(a);
	    if (a >= 0) {
		x -= w*ca + (l+ticklen)*sa;
		y += w*sa - (l+ticklen)*ca;
		a -= 90;
	    } else {
		x += w*ca - (ticklen+GAP)*sa;
		y -= w*sa + (ticklen+GAP)*ca;
		a += 90;
	    }
	    break;

	case TRO_PATHR:
	    if (ly-y == 0 && x-lx == 0)
		a = 0.0;
	    else
		a = atan2((double)(ly-y),(double)(x-lx));	/* -PI .. +PI */
	    ca = cos(a);
	    sa = sin(a);
	    a = raddeg(a);
	    if (a >= 0) {
		x -= w*ca - (ticklen+GAP)*sa;
		y += w*sa + (ticklen+GAP)*ca;
		a -= 90;
	    } else {
		x += w*ca + (l+ticklen)*sa;
		y -= w*sa - (l+ticklen)*ca;
		a += 90;
	    }
	    break;

	case TRO_N:
	    break;
	}

	xform_segs (x, y, degrad(a), xsp_orig, nxs);

	return (nxs);
}

/* draw character c with lower left corner at [x,0] with width w and height 2*w.
 * we only support the 10 digits and the characters ".:/ ".
 * remember that in X Windows, y grows positive downwards.
 * return number of segments used; worst-case is 7.
 */
static int
draw_char (x, w, c, xsp)
int x, w, c;
XSegment *xsp;
{
	XSegment *xsp_orig = xsp;
	int xc, yc;
	int s, i;

	/* first at [0,0] then add x to everything */
	switch (c) {
	case ' ':
	    return (0);

	case ':':
	    xc = w/2;
	    yc = -3*w/2;
	    xsp->x1=xc-1; xsp->y1=yc-1; xsp->x2=xc+1; xsp->y2=yc;   xsp++;
	    xsp->x1=xc+1; xsp->y1=yc;   xsp->x2=xc-1; xsp->y2=yc+1; xsp++;
	    xsp->x1=xc-1; xsp->y1=yc+1; xsp->x2=xc-1; xsp->y2=yc-1; xsp++;

	    yc = -w/2;
	    xsp->x1=xc-1; xsp->y1=yc-1; xsp->x2=xc+1; xsp->y2=yc;   xsp++;
	    xsp->x1=xc+1; xsp->y1=yc;   xsp->x2=xc-1; xsp->y2=yc+1; xsp++;
	    xsp->x1=xc-1; xsp->y1=yc+1; xsp->x2=xc-1; xsp->y2=yc-1; xsp++;
	    break;

	case '/':
	    yc = -2*w + 1;
	    xsp->x1=1;    xsp->y1= -1;   xsp->x2=w-1;  xsp->y2=yc;   xsp++;
	    break;

	case '.':
	    xc = w/2;
	    yc = -2;
	    xsp->x1=xc-1; xsp->y1=0;    xsp->x2=xc;   xsp->y2=yc;   xsp++;
	    xsp->x1=xc;   xsp->y1=yc;   xsp->x2=xc+1; xsp->y2=0;    xsp++;
	    xsp->x1=xc+1; xsp->y1=0;    xsp->x2=xc-1; xsp->y2=0;    xsp++;
	    break;

	case '0': 
	    yc = -2*w;
	    xsp->x1=0;    xsp->y1=0;    xsp->x2=0;    xsp->y2=yc;   xsp++;
	    xsp->x1=0;    xsp->y1=yc;   xsp->x2=w;    xsp->y2=yc;   xsp++;
	    xsp->x1=w;    xsp->y1=yc;   xsp->x2=w;    xsp->y2=0;    xsp++;
	    xsp->x1=w;    xsp->y1=0;    xsp->x2=0;    xsp->y2=0;    xsp++;
	    break;

	case '1': 
	    xc = w/2;
	    yc = -2*w;
	    xsp->x1=xc;   xsp->y1=0;    xsp->x2=xc;   xsp->y2=yc;   xsp++;
	    break;

	case '2':
	    yc = -2*w;
	    xsp->x1=0;    xsp->y1=yc;   xsp->x2=w;   xsp->y2=yc;   xsp++;
	    xsp->x1=w;    xsp->y1=yc;   xsp->x2=w;   xsp->y2= -w;   xsp++;
	    xsp->x1=w;    xsp->y1= -w;   xsp->x2=0;   xsp->y2= -w;   xsp++;
	    xsp->x1=0;    xsp->y1= -w;   xsp->x2=0;   xsp->y2=0;    xsp++;
	    xsp->x1=0;    xsp->y1=0;    xsp->x2=w;   xsp->y2=0;    xsp++;
	    break;

	case '3':
	    yc = -2*w;
	    xsp->x1=0;    xsp->y1=yc;   xsp->x2=w;   xsp->y2=yc;   xsp++;
	    xsp->x1=w;    xsp->y1=yc;   xsp->x2=w;   xsp->y2=0;    xsp++;
	    xsp->x1=w;    xsp->y1= -w;   xsp->x2=0;   xsp->y2= -w;   xsp++;
	    xsp->x1=w;    xsp->y1=0;    xsp->x2=0;   xsp->y2=0;    xsp++;
	    break;

	case '4':
	    yc = -2*w;
	    xsp->x1=0;    xsp->y1=yc;   xsp->x2=0;   xsp->y2= -w;   xsp++;
	    xsp->x1=0;    xsp->y1= -w;   xsp->x2=w;   xsp->y2= -w;   xsp++;
	    xsp->x1=w;    xsp->y1=yc;   xsp->x2=w;   xsp->y2=0;    xsp++;
	    break;

	case '5':
	    yc = -2*w;
	    xsp->x1=w;    xsp->y1=yc;   xsp->x2=0;   xsp->y2=yc;   xsp++;
	    xsp->x1=0;    xsp->y1=yc;   xsp->x2=0;   xsp->y2= -w;   xsp++;
	    xsp->x1=0;    xsp->y1= -w;   xsp->x2=w;   xsp->y2= -w;   xsp++;
	    xsp->x1=w;    xsp->y1= -w;   xsp->x2=w;   xsp->y2=0;    xsp++;
	    xsp->x1=w;    xsp->y1=0;    xsp->x2=0;   xsp->y2=0;    xsp++;
	    break;

	case '6':
	    yc = -2*w;
	    xsp->x1=w;    xsp->y1=yc;   xsp->x2=0;   xsp->y2=yc;   xsp++;
	    xsp->x1=0;    xsp->y1=yc;   xsp->x2=0;   xsp->y2=0;    xsp++;
	    xsp->x1=0;    xsp->y1=0;    xsp->x2=w;   xsp->y2=0;    xsp++;
	    xsp->x1=w;    xsp->y1=0;    xsp->x2=w;   xsp->y2= -w;   xsp++;
	    xsp->x1=w;    xsp->y1= -w;   xsp->x2=0;   xsp->y2= -w;   xsp++;
	    break;

	case '7':
	    xc = w;
	    yc = -2*w;
	    xsp->x1=0;    xsp->y1=yc;   xsp->x2=w;   xsp->y2=yc;   xsp++;
	    xsp->x1=w;    xsp->y1=yc;   xsp->x2=w/2; xsp->y2=0;    xsp++;
	    break;

	case '8':
	    yc = -2*w;
	    xsp->x1=0;    xsp->y1=yc;   xsp->x2=w;   xsp->y2=yc;   xsp++;
	    xsp->x1=0;    xsp->y1=yc;   xsp->x2=0;   xsp->y2=0;    xsp++;
	    xsp->x1=w;    xsp->y1=yc;   xsp->x2=w;   xsp->y2=0;    xsp++;
	    xsp->x1=0;    xsp->y1= -w;   xsp->x2=w;   xsp->y2= -w;   xsp++;
	    xsp->x1=0;    xsp->y1=0;    xsp->x2=w;   xsp->y2=0;    xsp++;
	    break;

	case '9':
	    yc = -2*w;
	    xsp->x1=0;    xsp->y1=yc;   xsp->x2=w;   xsp->y2=yc;   xsp++;
	    xsp->x1=w;    xsp->y1=yc;   xsp->x2=w;   xsp->y2=0;    xsp++;
	    xsp->x1=0;    xsp->y1=yc;   xsp->x2=0;   xsp->y2= -w;   xsp++;
	    xsp->x1=0;    xsp->y1= -w;   xsp->x2=w;   xsp->y2= -w;   xsp++;

	    break;

	default:

	    /* rectange with an X in it to indicate unsupported char */

	    yc = -2*w;
	    xsp->x1=0;   xsp->y1=0;  xsp->x2=2;   xsp->y2=yc; xsp++;
	    xsp->x1=2;   xsp->y1=yc; xsp->x2=w+2; xsp->y2=yc; xsp++;
	    xsp->x1=w+2; xsp->y1=yc; xsp->x2=w;   xsp->y2=0;  xsp++;
	    xsp->x1=w;   xsp->y1=0;  xsp->x2=0;   xsp->y2=0;  xsp++;
	    xsp->x1=0;   xsp->y1=0;  xsp->x2=w+2; xsp->y2=yc; xsp++;
	    xsp->x1=2;   xsp->y1=yc; xsp->x2=w;   xsp->y2=0;   xsp++;

	    break;
	}

	/* now shift everything to start at [x,0] */
	s = xsp - xsp_orig;
	for (i = 0; i < s; i++) {
	    xsp_orig[i].x1 += x;
	    xsp_orig[i].x2 += x;
	}

	return (s);
}

/* rotate the segments in xsp by angle about [0,0], then translate to [x,y].
 */
static void
xform_segs (x, y, angle, xsp, nxs)
int x, y;
double angle;
XSegment *xsp;
int nxs;
{
#define	FIXSCALE	1000	/* used fixed-point arithmetic */
	int c = FIXSCALE*cos(angle);
	int s = FIXSCALE*sin(angle);

	for (; --nxs >= 0; xsp++) {
	    int x1p = ( xsp->x1*c + xsp->y1*s)/FIXSCALE + x;
	    int y1p = (-xsp->x1*s + xsp->y1*c)/FIXSCALE + y;
	    int x2p = ( xsp->x2*c + xsp->y2*s)/FIXSCALE + x;
	    int y2p = (-xsp->x2*s + xsp->y2*c)/FIXSCALE + y;
	    xsp->x1 = (short)x1p;
	    xsp->y1 = (short)y1p;
	    xsp->x2 = (short)x2p;
	    xsp->y2 = (short)y2p;
	}
}
