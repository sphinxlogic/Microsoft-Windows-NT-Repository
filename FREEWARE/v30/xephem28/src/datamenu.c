/* code to manage the stuff on the "data" menu.
 * functions for the main data table are prefixed with dm.
 * functions for the setup menu are prefixed with ds.
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#if defined(__STDC__)
#include <stdlib.h>
#endif
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "preferences.h"

extern Widget	toplevel_w;

extern Now *mm_get_now P_((void));
extern Obj *db_basic P_((int id));
extern char *cns_name P_((int id));
extern double atod P_((char *buf));
extern int any_ison P_((void));
extern int cns_pick P_((double ra, double dec, double e));
extern void db_update P_((Obj *op));
extern void dm_update P_((Now *np, int how_much));
extern void f_dm_angle P_((Widget w, double a));
extern void f_double P_((Widget w, char *fmt, double f));
extern void f_mtime P_((Widget w, double t));
extern void f_pangle P_((Widget w, double a));
extern void f_ra P_((Widget w, double ra));
extern void f_sexa P_((Widget wid, double a, int w, int fracbase));
extern void f_showit P_((Widget w, char *s));
extern void f_string P_((Widget w, char *s));
extern void f_time P_((Widget w, double t));
extern void field_log P_((Widget w, double value, int logv, char *str));
extern void fs_mtime P_((char out[], double t));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void now_lst P_((Now *np, double *lst));
extern void prompt_map_cb P_((Widget w, XtPointer client, XtPointer call));
extern void register_selection P_((char *name));
extern void riset_cir P_((Now *np, Obj *op, double dis, RiseSet *rp));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void timestamp P_((Now *np, Widget w));
extern void watch_cursor P_((int want));
extern void xe_msg P_((char *msg, int app_modal));

static void dm_create_form P_((void));
static void dm_set_buttons P_((int whether));
static void dm_create_table P_((Widget parent));
static void dm_activate_cb P_((Widget w, XtPointer client, XtPointer call));
static void dm_close_cb P_((Widget w, XtPointer client, XtPointer call));
static void dm_setup_cb P_((Widget w, XtPointer client, XtPointer call));
static void dm_help_cb P_((Widget w, XtPointer client, XtPointer call));
static void dm_compute P_((int r, int force, Now *np));
static void dm_format P_((Now *np, Obj *op, RiseSet *rp, int c, Widget w));
static void dm_addplus P_((Now *np, Widget w, double tm, int tzpref,
    int addplus));
static void dm_rs_hrsup P_((Now *np, Obj *op, Widget w, RiseSet *rp));
static void show_constellation P_((Now *np, Obj *op, Widget w));
static void dm_separation P_((Obj *p, Obj *q, int how, Widget w));
static void ds_create_selection P_((Widget parent));
static void ds_setup_row_selections P_((void));
static void ds_setup_col_selections P_((int what));
static void ds_apply_selections P_((void));
static void ds_ctl_cb P_((Widget w, XtPointer client, XtPointer call));
static void ds_help P_((void));
static void ds_refr_cb P_((Widget w, XtPointer client, XtPointer call));
static void ds_row_toggle_cb P_((Widget w, XtPointer client, XtPointer call));
static void ds_col_toggle_cb P_((Widget w, XtPointer client, XtPointer call));
static void ds_row_all_cb P_((Widget w, XtPointer client, XtPointer call));
static void ds_col_all_cb P_((Widget w, XtPointer client, XtPointer call));
static void ds_row_reset_cb P_((Widget w, XtPointer client, XtPointer call));
static void ds_col_reset_cb P_((Widget w, XtPointer client, XtPointer call));

typedef struct {
    int dbidx;		/* db index of object on this row */
    int on;		/* whether this row is currently to be on */
    Widget lw;		/* label widget for this row's header */
    Widget sw;		/* pushbutton widget for this row in selection menu */
} RowHdr;

static RowHdr row[NOBJ] = {
    {SUN},
    {MOON},
    {MERCURY},
    {VENUS},
    {MARS},
    {JUPITER},
    {SATURN},
    {URANUS},
    {NEPTUNE},
    {PLUTO},
    {OBJX},
    {OBJY},
};

typedef struct {
    int type;		/* one of XXX_COL, below */
    char *name;		/* name of column, unless SEP_COL then use db_name */
    int dbidx;		/* if type == SEP_COL, db index of cross object */
    int on;		/* whether this column is currently to be on */
    Widget rcw;		/* RowColumn widget for this column */
    Widget lw;		/* label widget for this column's header */
    Widget sw;		/* pushbutton widget for this col in selection menu */
} ColHdr;

/* possible values for ColHdr.type. */
enum {
    MISC_COL, RISET_COL, SEP_COL
};

/* identifiers for each entry in col[]. these must match the order therein.
 */
enum {
    CONSTEL_ID, RA_ID, HA_ID, DEC_ID, AZ_ID, ALT_ID, Z_ID, HLONG_ID, HLAT_ID,
    EDST_ID, ELGHT_ID, SDST_ID, SLGHT_ID, ELONG_ID, SIZE_ID, VMAG_ID, PHS_ID,
    RSTIME_ID, RSAZ_ID, TRTIME_ID, TRALT_ID, SETTIME_ID, SETAZ_ID, HRSUP_ID
};

/* tags for the various Data Selection control panel buttons */
enum {OK, APPLY, CANCEL, HELP};

static ColHdr col[] = {
    {MISC_COL,	"Cns"},
    {MISC_COL,	"RA"},
    {MISC_COL,	"HA"},
    {MISC_COL,	"Dec"},
    {MISC_COL,	"Az"},
    {MISC_COL,	"Alt"},
    {MISC_COL,	"Air"},
    {MISC_COL,	"HeLong"},
    {MISC_COL,	"HeLat"},
    {MISC_COL,	"EaDst"},
    {MISC_COL,	"EaLght"},
    {MISC_COL,	"SnDst"},
    {MISC_COL,	"SnLght"},
    {MISC_COL,	"Elong"},
    {MISC_COL,	"Size"},
    {MISC_COL,	"VMag"},
    {MISC_COL,	"Phase"},
    {RISET_COL,	"RiseTm"},
    {RISET_COL,	"RiseAz"},
    {RISET_COL,	"TrnTm"},
    {RISET_COL,	"TrnAlt"},
    {RISET_COL,	"SetTm"},
    {RISET_COL,	"SetAz"},
    {RISET_COL,	"HrsUp"},
    {SEP_COL, (char *)0, SUN},
    {SEP_COL, (char *)0, MOON},
    {SEP_COL, (char *)0, MERCURY},
    {SEP_COL, (char *)0, VENUS},
    {SEP_COL, (char *)0, MARS},
    {SEP_COL, (char *)0, JUPITER},
    {SEP_COL, (char *)0, SATURN},
    {SEP_COL, (char *)0, URANUS},
    {SEP_COL, (char *)0, NEPTUNE},
    {SEP_COL, (char *)0, PLUTO},
    {SEP_COL, (char *)0, OBJX},
    {SEP_COL, (char *)0, OBJY},
};

#define	NR	XtNumber(row)
#define	NC	XtNumber(col)

static Widget t_w[NR][NC];	/* pushbuttons within table */
static Widget dataform_w;	/* the overall table form */
static Widget table_w;		/* the overall RowColumn table */
static Widget corner_w;		/* upper left corner of table */
static Widget hdrcol_w;		/* RowColumn for first column */
static Widget sel_w;		/* setup menu */
static Widget dt_w;		/* date/time stamp label widget */

/* separation perspective */
enum {GEO_CEN, TOPO_CEN};

enum {STDREFR, ADPREFR};
enum {LIMB, CENTER};

static Widget stdrefr_w;	/* the StdRefr toggle button */
static Widget adprefr_w;	/* the text holding the adaptive ref horizon */
static Widget limb_w;		/* the Center/Limb toggle button */
static Widget refr_w;		/* the horizon label on the data table */
static Widget limbl_w;		/* the Limb label on the data table */
static int horizon;		/* one of STDREFR or ADPREFR */
static int limb;		/* one of CENTER or LIMB */
static Widget geocen_w;		/* the Geocentric toggle button */
static Widget centric_w;	/* the centric label on the data table */
static int centric;		/* one of GEO_CEN or TOPO_CEN */

static int dm_selecting;	/* set while our fields are being selected */

/* called when the data menu is activated via the main menu pulldown.
 * if never called before, create all the widgets form;
 * otherwise, just toggle whether the form is managed.
 */
void
dm_manage ()
{
	if (!dataform_w)
	    dm_create_form();
	
	if (XtIsManaged(dataform_w)) {
	    XtUnmanageChild (dataform_w);
	    if (XtIsManaged(sel_w))
		XtUnmanageChild(sel_w);
	} else {
	    XtManageChild (dataform_w);
	    dm_update (mm_get_now(), 1);
	    dm_set_buttons (dm_selecting);
	}
}

/* user-defined object dbidx has changed.
 * might have a new name, or might be defined or undefined now.
 * must check both the data table and the selection menu.
 * N.B. no need to recompute math -- dm_update() will be called for us.
 */
void
dm_newobj(dbidx)
int dbidx;
{
	static char me[] = "dm_newobj()";
	int i, c;

	/* might get called before we have been managed the first time */
	if (!dataform_w)
	    return;

	for (i = 0; i < NR; i++)
	    if (row[i].dbidx == dbidx) {
		Obj *op = db_basic (dbidx);
		if (op->type == UNDEFOBJ) {
		    /* it's now undefined so turn off */
		    row[i].on = False;
		    for (c = 0; c < NC; c++)
			XtUnmanageChild (t_w[i][c]);
		    XtUnmanageChild (row[i].lw);
		    XtUnmanageChild (row[i].sw);
		    XmToggleButtonSetState (row[i].sw, False, False);
		} else {
		    f_string (row[i].lw, op->o_name);
		    XtManageChild (row[i].sw);
		    f_string (row[i].sw, op->o_name);
		}
		break;
	    }
	if (i == NR) {
	    printf ("Bug: %s: dbidx not in row[]: 0x%x\n", me, dbidx);
	    exit (1);
	}

	for (i = 0; i < NC; i++)
	    if (col[i].type == SEP_COL && col[i].dbidx == dbidx) {
		Obj *op = db_basic (dbidx);
		if (op->type == UNDEFOBJ) {
		    /* it's now undefined so turn off */
		    col[i].on = False;
		    XtUnmanageChild (col[i].rcw);
		    XtUnmanageChild (col[i].sw);
		    XmToggleButtonSetState (col[i].sw, False, False);
		} else {
		    f_string (col[i].lw, op->o_name);
		    XtManageChild (col[i].sw);
		    f_string (col[i].sw, op->o_name);
		}
		break;
	    }
	if (i == NC) {
	    printf ("Bug: %s: dbidx not in col[]: 0x%x\n", me, dbidx);
	    exit (1);
	}
}

/* called to recompute and fill in values for the data menu.
 * don't bother if it doesn't exist or is unmanaged now or no one is logging.
 */
void
dm_update (np, how_much)
Now *np;
int how_much;
{
	int i;

	if (!dataform_w)
	    return;
	if (!XtIsManaged(dataform_w) && !any_ison() && !how_much)
	    return;

	/* update each row that is on */
	for (i = 0; i < NR; i++)
	    if (row[i].on)
		dm_compute (i, how_much, np);

	/* update the datestamp */
	timestamp (np, dt_w);
}

/* called by other menus as they want to hear from our buttons or not.
 * the "on"s and "off"s stack - only really redo the buttons if it's the
 * first on or the last off.
 */
void
dm_selection_mode (whether)
int whether;	/* whether setting up for plotting or for not plotting */
{
	dm_selecting += whether ? 1 : -1;

	if (dataform_w && XtIsManaged(dataform_w))
	    if ((whether && dm_selecting == 1)     /* first one to want on */
		|| (!whether && dm_selecting == 0) /* last one to want off */)
		dm_set_buttons (whether);
}

/* given the Now and Obj, fill in the RiseSet.
 * this takes into account the options currently in effect with the Data menu.
 */
void
dm_riset (np, op, rsp)
Now *np;
Obj *op;
RiseSet *rsp;
{
	double dis;	/* rads apparent horizon is above true */
	int close = is_planet(op,MOON) || is_planet(op,SUN); 

	/* refraction correction */
	if (horizon == STDREFR) {
	    /* `nominal' atmospheric refraction. */
	    dis = STDREF;
	} else {
	    /* `Adaptive:' actual refraction conditions  */
	    char *str;

	    str = XmTextFieldGetString (adprefr_w);
	    dis = degrad(atod (str));
	    XtFree (str);
	    unrefract (pressure, temp, dis, &dis);
	    dis = -dis;	/* downwards */
	}

	/* add object's semi-diameter if want upper limb.
	 * only worth it for SUN and MOON.
	 */
	if (limb == LIMB && close)
	    dis += degrad((double)op->s_size/3600./2.0);

	/* add effect of being above surface
	 * TODO: this works but refraction model breaks down.
	dis += asin (sqrt(surfalt*surfalt + 2.0*ERAD*surfalt)
		      /(ERAD + surfalt));
	 */
	    
	riset_cir (np, op, dis, rsp);
}

/* called to put up or remove the watch cursor.  */
void
dm_cursor (c)
Cursor c;
{
	Window win;

	if (dataform_w && (win = XtWindow(dataform_w))) {
	    Display *dsp = XtDisplay(dataform_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}

	if (sel_w && (win = XtWindow(sel_w))) {
	    Display *dsp = XtDisplay(sel_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}

static void
dm_create_form()
{
	Widget ctlrc_w, w;
	Arg args[20];
	int n;

	/* create the form */
	n = 0;
	XtSetArg (args[n], XmNallowShellResize, True); n++;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNdefaultPosition, False); n++;
	XtSetArg (args[n], XmNallowOverlap, False); n++;
	dataform_w = XmCreateFormDialog (toplevel_w, "Data", args, n);
	XtAddCallback (dataform_w, XmNhelpCallback, dm_help_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem General Data Table"); n++;
	XtSetValues (XtParent(dataform_w), args, n);

	/* make a rowcolumn for the bottom control panel */

	n = 0;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
	XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
	ctlrc_w = XmCreateRowColumn (dataform_w, "DataTblRC", args, n);
	XtManageChild (ctlrc_w);

	    /* make the Setup button */

	    n = 0;
	    w = XmCreatePushButton (ctlrc_w, "Setup", args, n);
	    XtAddCallback (w, XmNactivateCallback, dm_setup_cb, 0);
	    XtManageChild (w);

	    /* make the close button */

	    n = 0;
	    w = XmCreatePushButton (ctlrc_w, "Close", args, n);
	    XtAddCallback (w, XmNactivateCallback, dm_close_cb, 0);
	    XtManageChild (w);

	    /* make the help pushbutton */

	    n = 0;
	    w = XmCreatePushButton (ctlrc_w, "Help", args, n);
	    XtAddCallback (w, XmNactivateCallback, dm_help_cb, 0);
	    XtManageChild (w);

	    /* make the horizon, limb and centric indicators in frames.
	     * turn them on and off by managing the frames -- but not yet!
	     */

	    n = 0;
	    w = XmCreateFrame (ctlrc_w, "DRefrF", args, n);
	    n = 0;
	    refr_w = XmCreateLabel (w, "DRefrL", args, n);
	    XtManageChild (refr_w);
	    n = 0;
	    w = XmCreateFrame (ctlrc_w, "DLimblF", args, n);
	    n = 0;
	    limbl_w = XmCreateLabel (w, "DLimblL", args, n);
	    XtManageChild (limbl_w);
	    n = 0;
	    w = XmCreateFrame (ctlrc_w, "DCentricF", args, n);
	    n = 0;
	    centric_w = XmCreateLabel (w, "DCentricL", args, n);
	    XtManageChild (centric_w);

	    /* make a label for the date/time stamp */

	    n = 0;
	    dt_w = XmCreateLabel (ctlrc_w, "DateStamp", args, n);
	    timestamp (mm_get_now(), dt_w);	/* sets initial size correctly*/
	    XtManageChild (dt_w);

	/* create the table */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, ctlrc_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
	table_w = XmCreateRowColumn (dataform_w, "DataTable", args, n);
	XtManageChild (table_w);

	dm_create_table (table_w);

	/* create the selection dialog.
	 * don't manage it yet but its state info is used right off.
	 */
	ds_create_selection(toplevel_w);
	ds_apply_selections();
}

/* go through all the buttons and set whether they
 * should appear to look like buttons or just flat labels.
 */
static void
dm_set_buttons (whether)
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
	int r, c;
	Arg *ap;
	int na;

	if (!called) {
	    /* get baseline label and shadow appearances.
	     * also make the corner and headers look like labels forever.
	     */
            Pixel topshadcol, botshadcol, bgcol;
            Pixmap topshadpm, botshadpm;
	    Arg args[20];
	    int n;
	    int i;

	    n = 0;
            XtSetArg (args[n], XmNtopShadowColor, &topshadcol); n++;
            XtSetArg (args[n], XmNbottomShadowColor, &botshadcol); n++;
            XtSetArg (args[n], XmNtopShadowPixmap, &topshadpm); n++;
            XtSetArg (args[n], XmNbottomShadowPixmap, &botshadpm); n++;
	    XtSetArg (args[n], XmNbackground, &bgcol); n++;
	    XtGetValues (corner_w, args, n);

            look_like_button[0].value = topshadcol;
            look_like_button[1].value = botshadcol;
            look_like_button[2].value = topshadpm;
            look_like_button[3].value = botshadpm;
            look_like_label[0].value = bgcol;
            look_like_label[1].value = bgcol;
            look_like_label[2].value = XmUNSPECIFIED_PIXMAP;
            look_like_label[3].value = XmUNSPECIFIED_PIXMAP;

	    ap = look_like_label;
	    na = XtNumber(look_like_label);
	    XtSetValues (corner_w, ap, na);
	    for (i = 0; i < NR; i++)
		XtSetValues (row[i].lw, ap, na);
	    for (i = 0; i < NC; i++)
		XtSetValues (col[i].lw, ap, na);

	    called = 1;
	}

	if (whether) {
	    ap = look_like_button;
	    na = XtNumber(look_like_button);
	} else {
	    ap = look_like_label;
	    na = XtNumber(look_like_label);
	}
	for (r = 0; r < NR; r++)
	    for (c = 0; c < NC; c++)
		if (t_w[r][c])
		    XtSetValues (t_w[r][c], ap, na);
}

/* create the main data table - everything but the first column is unmanaged.
 */
static void
dm_create_table(parent)
Widget parent;	/* overall RowColumn */
{
	Arg args[20];
	XmString str;
	Widget w;
	int r, c;
	int n;

	/* first column is the row headers.
	 * it's always managed so init what rows we can too.
	 */
	n = 0;
	hdrcol_w = XmCreateRowColumn (parent, "DataHdrC", args, n);
	XtManageChild (hdrcol_w);

	    /* first row is a dummy */
	    n = 0;
	    str = XmStringCreateLtoR (" ", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    corner_w = XmCreatePushButton (hdrcol_w, "DataCorner", args, n);
	    XmStringFree (str);
	    XtManageChild (corner_w);

	    /* remaining rows are per object */
	    for (r = 0; r < NR; r++) {
		Obj *op = db_basic (row[r].dbidx);
		n = 0;
		if (op->type != UNDEFOBJ)
		    w = XmCreatePushButton (hdrcol_w, op->o_name, args, n);
		else
		    w = XmCreatePushButton (hdrcol_w, "DRow", args, n);
		row[r].lw = w;

	    }

	/* remaining columns.
	 * don't manage any but set names of what we can now too.
	 */
	for (c = 0; c < NC; c++) {
	    Widget rcw;

	    n = 0;
	    XtSetArg (args[n], XmNadjustMargin, False); n++;
	    XtSetArg (args[n], XmNisAligned, False); n++;
	    rcw = col[c].rcw = XmCreateRowColumn (parent, "DataCol", args, n);

	    /* first row is column header */
	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    if (col[c].type != SEP_COL)
		w = XmCreatePushButton (rcw, col[c].name, args, n);
	    else {
		Obj *op = db_basic (col[c].dbidx);
		if (op->type != UNDEFOBJ)
		    w = XmCreatePushButton (rcw, op->o_name, args, n);
		else
		    w = XmCreatePushButton (rcw, "DCHdr", args, n);
	    }
	    col[c].lw = w;
	    XtManageChild (w);

	    /* remaining rows are per object */
	    for (r = 0; r < NR; r++) {
		n = 0;
		XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
		w = t_w[r][c] = XmCreatePushButton (rcw, "DataPB", args, n);
		XtAddCallback(w, XmNactivateCallback, dm_activate_cb,
					(XtPointer)((c<<8)|r));
	    }
	}
}

/* callback from any of the data menu buttons being activated.
 * do nothing unless we are being used to set up a selection collection.
 * if the latter, make a name for our field, put it in UserData and inform
 * all interested parties.
 * to form the name, client data is (col<<8)|row
 * N.B. we assume we can't be called if our row and column are not on.
 */
static void
/* ARGSUSED */
dm_activate_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static char me[] = "dm_activate_cb()";

	if (dm_selecting) {
	    int r = (unsigned)client & 0xff;
	    int c = (unsigned)client >> 8;
	    char *name, *rname, *cname;
	    Obj *op;
	    char *userD;   /* Heller, pg 852, say's this is type Pointer?? */
	    int len;

	    /* figure out our row name */
	    if (!row[r].on) {
		printf ("Bug: %s: row[%d] not on\n", me, r);
		exit (1);
	    }
	    op = db_basic (row[r].dbidx);
	    rname = op->o_name;
	    len = strlen (rname);

	    /* figure out our col name */
	    if (!col[c].on) {
		printf ("Bug: %s: col[%d] not on\n", me, c);
		exit (1);
	    }
	    if (col[c].type == SEP_COL) {
		op = db_basic (col[c].dbidx);
		cname = op->o_name;
	    } else
		cname = col[c].name;
	    len += strlen(cname);

	    name = XtMalloc (len + 2);	/* '.' plus '\0' */
	    (void) sprintf (name, "%s.%s", rname, cname);

	    /* set XmNuserData to be the name we want to go by */
	    get_something (w, XmNuserData, (XtArgVal)&userD);
	    if (userD)
		XtFree (userD);
	    userD = name;
	    set_something (w, XmNuserData, (XtArgVal)userD);

	    /* tell the world our name */
	    register_selection (name);
	}
}

/* callback from the Data table Close button
 */
static void
/* ARGSUSED */
dm_close_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (dataform_w);
	if (XtIsManaged (sel_w))
	    XtUnmanageChild (sel_w);
}

/* callback from the Data table Setup button.
 */
static void
/* ARGSUSED */
dm_setup_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (XtIsManaged(sel_w))
	    XtUnmanageChild (sel_w);
	else {
	    ds_setup_row_selections();
	    ds_setup_col_selections(MISC_COL);
	    ds_setup_col_selections(RISET_COL);
	    ds_setup_col_selections(SEP_COL);
	    XtManageChild (sel_w);
	}
}

/* callback from the Data table Help button
 */
static void
/* ARGSUSED */
dm_help_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static char *msg[] = {
"This table displays various information about the planets and objects.",
"To reduce computation and save screen space, each row and column may be",
"individually turned off or on using the Select button."
};

	hlp_dialog ("Data Table", msg, XtNumber(msg));
}

/* compute and print body info in data menu format */
/* ARGSUSED */
static void
dm_compute (r, force, np)
int r;		/* which row */
int force;	/* whether to print for sure or only if things have changed */
Now *np;
{
	RiseSet rs;
	Obj *op;
	int c;
	int did_rs = 0;

	op = db_basic (row[r].dbidx);
	db_update (op);

	for (c = 0; c < NC; c++)
	    if (col[c].on) {
		if (col[c].type == RISET_COL && !did_rs) {
		    dm_riset (np, op, &rs);
		    did_rs = 1;
		}
		dm_format(np, op, &rs, c, t_w[r][c]);
	    }
}

static void
dm_format (np, op, rp, c, w)
Now *np;
Obj *op;
RiseSet *rp;
int c;
Widget w;
{
	static char me[] = "dm_format()";

	switch (c) {
	case CONSTEL_ID:
	    show_constellation (np, op, w);
	    break;
	case RA_ID:
	    f_ra (w, op->s_ra);
	    break;
	case HA_ID: {
	    double raeod = op->s_ra;
	    double deceod = op->s_dec;
	    double tmp;

	    if (epoch != EOD)
		precess (epoch, mjd, &raeod, &deceod);
	    now_lst (np, &tmp);
	    tmp = tmp - radhr(raeod);	/* HA = LST - RA */
	    if (tmp > 12)
		tmp -= 24;
	    if (tmp < -12)
		tmp += 24;
	    if (pref_get(PREF_DPYPREC) == PREF_LOPREC)
		f_sexa (w, tmp, 3, 600);
	    else
		f_sexa (w, tmp, 3, 36000);
	    }
	    break;
	case DEC_ID:
	    f_pangle (w, op->s_dec);
	    break;
	case AZ_ID:
	    f_pangle (w, op->s_az);
	    break;
	case ALT_ID:
	    f_pangle (w, op->s_alt);
	    break;
	case Z_ID: /* airmass */
	    if (op->s_alt > 0.1)
		f_double (w, "%4.2f", 1./sin(op->s_alt));
	    else
		f_double (w, "%4.0f", 99.0);
	    break;
	case HLONG_ID:
	    if (is_ssobj(op))
		f_pangle (w, op->s_hlong);
	    else {
		if (pref_get(PREF_DPYPREC) == PREF_LOPREC)
		    f_string (w, "      ");
		else
		    f_string (w, "         ");
	    }
	    break;
	case HLAT_ID:
	    if (is_ssobj(op))
		f_pangle (w, op->s_hlat);
	    else {
		if (pref_get(PREF_DPYPREC) == PREF_LOPREC)
		    f_string (w, "      ");
		else
		    f_string (w, "         ");
	    }
	    break;
	case EDST_ID:
	    if (is_planet(op, MOON)) {
		double tmp = op->s_edist;
		if (pref_get(PREF_UNITS) == PREF_ENGLISH) {
		    /* s_edist is stored in au, want miles */
		    tmp *= MAU*FTPM/5280.0;
		} else {
		    /* s_edist is stored in au, want km */
		    tmp *= MAU/1000.0;
		}
		f_double (w, "%6.0f", tmp);
	    } else if (is_ssobj(op)) {
		/* show distance in au */
		f_double (w, op->s_edist >= 9.99995 ? "%6.3f" : "%6.4f",
								op->s_edist);
	    } else
		f_string (w, "      ");
	    break;
	case ELGHT_ID:
	    if (is_planet(op,MOON)) {
	        double m = op->s_edist*LTAU;	/* seconds */
		if (pref_get(PREF_DPYPREC) == PREF_LOPREC)
		    f_double (w, "%5.3f", m);
		else
		    f_double (w, "% 7.5f", m);	/* 8.6 is ridiculous */
	    } else if (is_ssobj(op)) {
	        double m = (op->s_edist*LTAU)/3600.0;	/* hours */
		if (pref_get(PREF_DPYPREC) == PREF_LOPREC)
		    f_mtime (w, m);	/* hh:mm */
		else
		    f_time (w, m);	/* hh:mm:ss */
	    } else {
		if (pref_get(PREF_DPYPREC) == PREF_LOPREC)
		    f_string (w, "     ");
		else
		    f_string (w, "        ");
	    }
	    break;
	case SDST_ID:
	    if (is_ssobj(op) && !is_planet(op, SUN))
		f_double (w, op->s_sdist >= 9.99995 ? "%6.3f" : "%6.4f",
								op->s_sdist);
	    else
		f_string (w, "      ");
	    break;
	case SLGHT_ID:
	    if (is_ssobj(op) && !is_planet(op, SUN)) {
	        double m = (op->s_sdist*LTAU)/3600.0;	/* hours */
		if (pref_get(PREF_DPYPREC) == PREF_LOPREC)
		    f_mtime (w, m);	/* hh:mm */
		else
		    f_time (w, m);	/* hh:mm:ss */
	    } else {
		if (pref_get(PREF_DPYPREC) == PREF_LOPREC)
		    f_string (w, "     ");
		else
		    f_string (w, "        ");
	    }
	    break;
	case ELONG_ID:
	    if (is_ssobj(op) && !is_planet(op, SUN))
		f_double (w, "%6.1f", op->s_elong);
	    else
		f_string (w, "      ");
	    break;
	case SIZE_ID:
	    if (op->type != EARTHSAT)
		f_double (w, "%5.0f", (double)(op->s_size));
	    else
		f_string (w, "     ");
	    break;
	case VMAG_ID:
	    if (op->type != EARTHSAT) {
	        double m = (double) (op->s_mag / MAGSCALE);
	        f_double (w, m <= -9.95 ? "%4.0f" : "%4.1f", m);
	    } else
		f_string (w, "    ");
	    break;
	case PHS_ID:
	    if (is_ssobj(op) && !is_planet(op, SUN))
		f_double (w, " %3.0f", op->s_phase);
	    else
		f_string (w, "    ");
	    break;

	case RSTIME_ID:
	    if (rp->rs_flags & RS_ERROR)
		f_string (w, "Error ");
	    else if (rp->rs_flags & RS_CIRCUMPOLAR)
		f_string (w, "CirPol");
	    else if (rp->rs_flags & RS_NEVERUP)
		f_string (w, "NvrUp ");
	    else if (rp->rs_flags & RS_NORISE)
		f_string (w, "NoRise");
	    else
		dm_addplus (np, w, rp->rs_risetm, 1, rp->rs_flags & RS_2RISES);
	    break;

	case RSAZ_ID:
	    if (rp->rs_flags & RS_ERROR)
		f_string (w, "Error ");
	    else if (rp->rs_flags & RS_CIRCUMPOLAR)
		f_string (w, "CirPol");
	    else if (rp->rs_flags & RS_NEVERUP)
		f_string (w, "NvrUp ");
	    else if (rp->rs_flags & RS_NORISE)
		f_string (w, "NoRise");
	    else
		f_dm_angle (w, rp->rs_riseaz);	/* 6 chars wide */
	    break;

	case SETTIME_ID:
	    if (rp->rs_flags & RS_ERROR)
		f_string (w, "Error ");
	    else if (rp->rs_flags & RS_CIRCUMPOLAR)
		f_string (w, "CirPol");
	    else if (rp->rs_flags & RS_NEVERUP)
		f_string (w, "NvrUp ");
	    else if (rp->rs_flags & RS_NOSET)
		f_string (w, "NoSet ");
	    else
		dm_addplus (np, w, rp->rs_settm, 1, rp->rs_flags & RS_2SETS);
	    break;

	case SETAZ_ID:
	    if (rp->rs_flags & RS_ERROR)
		f_string (w, "Error ");
	    else if (rp->rs_flags & RS_CIRCUMPOLAR)
		f_string (w, "CirPol");
	    else if (rp->rs_flags & RS_NEVERUP)
		f_string (w, "NvrUp ");
	    else if (rp->rs_flags & RS_NOSET)
		f_string (w, "NoSet ");
	    else
		f_dm_angle (w, rp->rs_setaz);	/* 6 chars wide */
	    break;

	case TRTIME_ID:
	    if (rp->rs_flags & RS_ERROR)
		f_string (w, "Error ");
	    else if (rp->rs_flags & RS_NEVERUP)
		f_string (w, "NvrUp ");
	    else if (rp->rs_flags & RS_NOTRANS)
		f_string (w, "NoTran");
	    else
		dm_addplus (np, w, rp->rs_trantm, 1, rp->rs_flags & RS_2TRANS);
	    break;

	case TRALT_ID:
	    if (rp->rs_flags & RS_ERROR)
		f_string (w, "Error ");
	    else if (rp->rs_flags & RS_NEVERUP)
		f_string (w, "NvrUp ");
	    else if (rp->rs_flags & RS_NOTRANS)
		f_string (w, "NoTran");
	    else {
		f_dm_angle (w, rp->rs_tranalt);	/* 6 chars wide */
	    }
	    break;

	case HRSUP_ID:
	    dm_rs_hrsup (np, op, w, rp);
	    break;

	default:
	    /* these are effectively all the separation columns */
	    if (c < 0 || c >= NC) {
		printf ("Bug: %s: c = %d but max = %d\n", me, c, NC-1);
		exit (1);
	    }
	    if (col[c].type != SEP_COL) {
		printf ("Bug: %s: col[%d].type = 0x%x\n", me, c, col[c].type);
		exit (1);
	    }
	    if (op != db_basic(col[c].dbidx)) {
		db_update(db_basic(col[c].dbidx));
		dm_separation (op, db_basic(col[c].dbidx), centric, w);
	    } else
		f_string (w, "     ");
	    break;
	}
}

/* display the rise/set/transit time tm (in hours) in widget w.
 * if addplus is != 0 then add a '+' to the time string.
 * tm is hours; convert to UTC if tzpref && PREF_ZONE == PREF_UTCTZ.
 * N.B. be sure the total string length is the same regardless of addplus.
 */
static void
dm_addplus (np, w, tm, tzpref, addplus)
Now *np;
Widget w;
double tm;
int tzpref;
int addplus;
{
	char str[32];

	if (tzpref && pref_get(PREF_ZONE) == PREF_UTCTZ) {
	    tm += tz;
	    range (&tm, 24.0);
	}

	fs_mtime (str, tm);				/* 5 chars: "hh:mm" */
	(void) strcat (str, addplus ? "+" : " ");	/* always add 1 */
	field_log (w, tm, 1, str);
	f_showit (w, str);
}

/* display the total hours this object has been up.
 * N.B. insure string length is always 6 chars wide.
 * N.B. Earth satellites are up from rise to set, period, and either or both
 *   of these may be tomorrow if they are later than the current time because
 *   we scan for earth sat rise/set events up to 24 hours in the future
 *   regardless of whether it crosses over into tomorrow. Also, since we can't
 *   assume a nominal diurnal period who's to say how much it's up if all we
 *   have is a rise time that is later than a set time,
 */
/* ARGSUSED */
static void
dm_rs_hrsup (np, op, w, rp)
Now *np;
Obj *op;
Widget w;
RiseSet *rp;
{
	double r, s, hrs;

	if (rp->rs_flags & (RS_ERROR|RS_RISERR)) {
	    f_string (w, "Error ");
	    return;
	}
	if (rp->rs_flags & RS_CIRCUMPOLAR) {
	    f_double (w, "%3.0f:00", 24.0); /* f_mtime() changes to 00:00 */
	    return;
	}
	if (rp->rs_flags & RS_NEVERUP) {
	    dm_addplus (np, w, 0.0, 0, 0);
	    return;
	}

	if (op->type == EARTHSAT && op->es_n > FAST_SAT_RPD) {
	    double local_hr;

	    /* can't do much if both times aren't well known */
	    if (rp->rs_flags & (RS_NORISE|RS_NOSET)) {
		f_string (w, "      ");
		return;
	    }

	    /* times earlier than now are really tomorrow */
	    local_hr = mjd_hr (mjd - tz/24.0);
	    r = rp->rs_risetm;
	    if (r < local_hr) r += 24.0;
	    s = rp->rs_settm;
	    if (s < local_hr) s += 24.0;

	    /* only if set time is really after rise time can we figure hrsup */
	    if (s > r)
		dm_addplus(np, w, s - r, 0, 0);
	    else
		f_string (w, "      ");

	    return;
	}

	r = (rp->rs_flags & RS_NORISE) ?  0.0 : rp->rs_risetm;
	s = (rp->rs_flags & RS_NOSET)  ? 24.0 : rp->rs_settm;
	hrs = s - r;
	if (hrs < 0)
	    hrs += 24.0;

	dm_addplus(np, w, hrs, 0,
			rp->rs_flags&(RS_NORISE|RS_NOSET|RS_2RISES|RS_2SETS));
}

static void
show_constellation (np, op, w)
Now *np;
Obj *op;
Widget w;
{
	char nm[10], *name;
	int id;

        id = cns_pick (op->s_ra, op->s_dec, epoch == EOD ? mjd : epoch);
	name = cns_name (id);
	(void) sprintf (nm, "%.3s", name);
	f_string(w, nm);
}

/* compute and display the separation between the two sky locations */
static void
dm_separation (p, q, how, w)
Obj *p, *q;
int how;	/* GEO_CEN or TOPO_CEN */
Widget w;
{
	double spy, cpy, px, qx, sqy, cqy;
	double sep;

	if (how == GEO_CEN) {
	    /* use ra for "x", dec for "y". */
	    spy = sin (p->s_dec);
	    cpy = cos (p->s_dec);
	    px = p->s_ra;
	    qx = q->s_ra;
	    sqy = sin (q->s_dec);
	    cqy = cos (q->s_dec);
	} else {
	    /* use azimuth for "x", altitude for "y". */
	    spy = sin (p->s_alt);
	    cpy = cos (p->s_alt);
	    px = p->s_az;
	    qx = q->s_az;
	    sqy = sin (q->s_alt);
	    cqy = cos (q->s_alt);
	}

	sep = acos(spy*sqy + cpy*cqy*cos(px-qx));
	f_pangle (w, sep);
}

/* create the selections dialog */
static void
ds_create_selection(parent)
Widget parent;
{
	static char me[] = "ds_create_selection()";
	static struct { /* info to streamline creation of control buttons */
	    int id;
	    int lpos, rpos;
	    char *name;
	} ctlbtns[] = {
	    {OK, 1, 3, "Ok"},
	    {APPLY, 4, 6, "Apply"},
	    {CANCEL, 7, 9, "Close"},
	    {HELP, 10, 12, "Help"}
	};
	Arg args[20];
	XmString str;
	Widget rl_w, cl_w;
	Widget rf_w, mf_w, rsf_w, sf_w, ctlf_w, rowrc_w, mrc_w, rsrc_w, src_w;
	Widget sep_w, w, tb1, tb2;
	Widget rb_w, rc_w;
	int n;
	int i;

	/* create form */
	n = 0;
	XtSetArg (args[n], XmNallowShellResize, True); n++;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNdefaultPosition, False); n++;
	XtSetArg (args[n], XmNallowOverlap, False); n++;
	sel_w = XmCreateFormDialog (parent, "DataSelection", args, n);
	XtAddCallback (sel_w, XmNmapCallback, prompt_map_cb, NULL);
	XtAddCallback (sel_w, XmNhelpCallback, ds_ctl_cb, (XtPointer)HELP);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Data Table setup"); n++;
	XtSetValues (XtParent(sel_w), args, n);

	/* make a form for bottom control panel */
	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNfractionBase, 13); n++;
	XtSetArg (args[n], XmNverticalSpacing, 5); n++;
	ctlf_w = XmCreateForm (sel_w, "DataSelF", args, n);
	XtManageChild (ctlf_w);

	    /* make the control buttons */

	    for (i = 0; i < XtNumber(ctlbtns); i++) {
		n = 0;
		XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
		XtSetArg (args[n], XmNleftPosition, ctlbtns[i].lpos); n++;
		XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
		XtSetArg (args[n], XmNrightPosition, ctlbtns[i].rpos); n++;
		w = XmCreatePushButton (ctlf_w, ctlbtns[i].name, args, n);
		XtAddCallback (w, XmNactivateCallback, ds_ctl_cb,
						    (XtPointer)ctlbtns[i].id);
		XtManageChild (w);
	    }

	/* make a top separator */
	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (sel_w, "DSSep", args, n);
	XtManageChild (sep_w);

	/* make the Rows heading */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	rl_w = XmCreateLabel (sel_w, "Rows:", args, n);
	XtManageChild (rl_w);

	/* make the row selection rc in a frame */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, rl_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, ctlf_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	rf_w = XmCreateFrame (sel_w, "DSRFrame", args, n);
	XtManageChild (rf_w);

	n = 0;
	XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg (args[n], XmNadjustMargin, False); n++;
	XtSetArg (args[n], XmNisAligned, False); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
	rowrc_w = XmCreateRowColumn (rf_w, "DataSelRows", args, n);
	XtManageChild (rowrc_w);

	    /* fill up with buttons for each possible row.
	     * fill in name if it's a planet since that won't change.
	     */

	    /* make the "Toggle" push button */

	    str = XmStringCreate("Toggle", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    w = XmCreatePushButton (rowrc_w, "DSRToggle", args, n);
	    XtAddCallback (w, XmNactivateCallback, ds_row_toggle_cb, 0);
	    XtManageChild (w);
	    XmStringFree (str);

	    /* make the "All" push button */

	    str = XmStringCreate("All", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    w = XmCreatePushButton (rowrc_w, "DSRAll", args, n);
	    XtAddCallback (w, XmNactivateCallback, ds_row_all_cb, 0);
	    XtManageChild (w);
	    XmStringFree (str);

	    /* make the "Reset" push button */

	    str = XmStringCreate("Reset", XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    w = XmCreatePushButton (rowrc_w, "DSRReset", args, n);
	    XtAddCallback (w, XmNactivateCallback, ds_row_reset_cb, 0);
	    XtManageChild (w);
	    XmStringFree (str);

	    for (i = 0; i < NR; i++) {
		Obj *op = db_basic(row[i].dbidx);
		n = 0;
		XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
		if (op->type != UNDEFOBJ) {
		    w = row[i].sw = XmCreateToggleButton(rowrc_w, op->o_name,
								    args, n);
		    XtManageChild (w);
		} else
		    row[i].sw = XmCreateToggleButton(rowrc_w, "RowSelObj",
								    args, n);
	    }

	/* make the Columns heading */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNleftWidget, rf_w); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	cl_w = XmCreateLabel (sel_w, "Columns:", args, n);
	XtManageChild (cl_w);

	/* make the misc col selection rc in a frame */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, cl_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, ctlf_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNleftWidget, rf_w); n++;
	mf_w = XmCreateFrame (sel_w, "DSMCFrame", args, n);
	XtManageChild (mf_w);

	n = 0;
	XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg (args[n], XmNadjustMargin, False); n++;
	XtSetArg (args[n], XmNisAligned, False); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
	mrc_w = XmCreateRowColumn (mf_w, "DataSelMiscCols", args, n);
	XtManageChild (mrc_w);

	    /* fill up with buttons for each possible col in misc range.
	     * set those columns names that are stable.
	     */

	    /* make the "Toggle" push button */

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    w = XmCreatePushButton (mrc_w, "Toggle", args, n);
	    XtAddCallback (w, XmNactivateCallback, ds_col_toggle_cb,
							(XtPointer)MISC_COL);
	    XtManageChild (w);

	    /* make the "All" push button */

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    w = XmCreatePushButton (mrc_w, "All", args, n);
	    XtAddCallback (w, XmNactivateCallback, ds_col_all_cb,
							(XtPointer)MISC_COL);
	    XtManageChild (w);

	    /* make the "Reset" push button */

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    w = XmCreatePushButton (mrc_w, "Reset", args, n);
	    XtAddCallback (w, XmNactivateCallback, ds_col_reset_cb,
							(XtPointer)MISC_COL);
	    XtManageChild (w);

	/* make the rise/set col selection rc in a frame */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, cl_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, ctlf_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNleftWidget, mf_w); n++;
	rsf_w = XmCreateFrame (sel_w, "DSRCFrame", args, n);
	XtManageChild (rsf_w);

	n = 0;
	XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg (args[n], XmNadjustMargin, False); n++;
	XtSetArg (args[n], XmNisAligned, False); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
	rsrc_w = XmCreateRowColumn (rsf_w, "DataSelRisetCols", args, n);
	XtManageChild (rsrc_w);

	    /* fill up with buttons for each possible col in rise/set range.
	     * set those columns names that are stable.
	     */

	    /* make the "Toggle" push button */

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    w = XmCreatePushButton (rsrc_w, "Toggle", args, n);
	    XtAddCallback (w, XmNactivateCallback, ds_col_toggle_cb,
							(XtPointer)RISET_COL);
	    XtManageChild (w);

	    /* make the "All" push button */

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    w = XmCreatePushButton (rsrc_w, "All", args, n);
	    XtAddCallback (w, XmNactivateCallback, ds_col_all_cb,
							(XtPointer)RISET_COL);
	    XtManageChild (w);

	    /* make the "Reset" push button */

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    w = XmCreatePushButton (rsrc_w, "Reset", args, n);
	    XtAddCallback (w, XmNactivateCallback, ds_col_reset_cb,
							(XtPointer)RISET_COL);
	    XtManageChild (w);

	    /* make the STDREFR/ADPREFR radio box and horizon offset text */

	    n = 0;
	    w = XmCreateFrame (rsrc_w, "DSRefrF", args, n);
	    XtManageChild (w);
	    n = 0;
	    rc_w = XmCreateRowColumn (w, "DSRefrRC", args, n);
	    XtManageChild (rc_w);

		n = 0;
		rb_w = XmCreateRadioBox (rc_w, "DSRefrRB", args, n);
		XtManageChild (rb_w);

		    n = 0;
		    tb1 = XmCreateToggleButton (rb_w, "StdRefr", args,n);
		    XtAddCallback (tb1, XmNvalueChangedCallback, ds_refr_cb, 0);
		    XtManageChild (tb1);
		    stdrefr_w = tb1;

		    n = 0;
		    tb2 = XmCreateToggleButton (rb_w, "AdpRefr", args, n);
		    XtManageChild (tb2);

		n = 0;
		adprefr_w = XmCreateTextField (rc_w, "AdpDispl", args, n);
		XtManageChild (adprefr_w);

		/* if neither or both is set up in defaults, set for StdRefr
		 * otherwise just trigger one to set the text field.
		 */
		if (XmToggleButtonGetState(tb1) == XmToggleButtonGetState(tb2)){
		    xe_msg (
		    "Conflicting Refraction resources -- defaulting to StdRefr",
									    0);
		    XmToggleButtonSetState (stdrefr_w, True, True);
		} else {
		    /* must toggle state to insure callbacks really called */
		    XmToggleButtonSetState (stdrefr_w,
				    !XmToggleButtonGetState(stdrefr_w), True);
		    XmToggleButtonSetState (stdrefr_w,
				    XmToggleButtonGetState(stdrefr_w), True);
		}

	    /* make the CENTER/LIMB radio box */

	    n = 0;
	    w = XmCreateFrame (rsrc_w, "DSLimbF", args, n);
	    XtManageChild (w);
	    n = 0;
	    rb_w = XmCreateRadioBox (w, "DSLimbRB", args, n);
	    XtManageChild (rb_w);

		n = 0;
		limb_w = tb1 = XmCreateToggleButton (rb_w, "Limb", args, n);
		XtManageChild (tb1);
		n = 0;
		tb2 = XmCreateToggleButton (rb_w, "Center", args, n);
		XtManageChild (tb2);
		/* if neither or both is set up in defaults, set for Limb */
		if (XmToggleButtonGetState(tb1) == XmToggleButtonGetState(tb2)){
		    xe_msg (
		      "Conflicting Limb resources -- defaulting to Limb\n",
									    0);
		    XmToggleButtonSetState (tb1, True, True);
		}

	/* make the separations col selection rc in a frame */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, cl_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, ctlf_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNleftWidget, rsf_w); n++;
	sf_w = XmCreateFrame (sel_w, "DSRCFrame", args, n);
	XtManageChild (sf_w);

	n = 0;
	XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg (args[n], XmNadjustMargin, False); n++;
	XtSetArg (args[n], XmNisAligned, False); n++;
	XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
	src_w = XmCreateRowColumn (sf_w, "DataSelSepCols", args, n);
	XtManageChild (src_w);

	    /* fill up with buttons for each possible col in sep range.
	     * set those columns names that are stable.
	     */

	    /* make the "Toggle" push button */

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    w = XmCreatePushButton (src_w, "Toggle", args, n);
	    XtAddCallback (w, XmNactivateCallback, ds_col_toggle_cb,
							(XtPointer)SEP_COL);
	    XtManageChild (w);

	    /* make the "All" push button */

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    w = XmCreatePushButton (src_w, "All", args, n);
	    XtAddCallback (w, XmNactivateCallback, ds_col_all_cb,
							(XtPointer)SEP_COL);
	    XtManageChild (w);

	    /* make the "Reset" push button */

	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	    w = XmCreatePushButton (src_w, "Reset", args, n);
	    XtAddCallback (w, XmNactivateCallback, ds_col_reset_cb,
							(XtPointer)SEP_COL);
	    XtManageChild (w);

	    /* make the GEOCENTRIC/TOPOCENTRIC radio box */

	    n = 0;
	    w = XmCreateFrame (src_w, "DSCentricF", args, n);
	    XtManageChild (w);
	    n = 0;
	    rb_w = XmCreateRadioBox (w, "DSCentricRB", args, n);
	    XtManageChild (rb_w);

		n = 0;
		geocen_w = tb1 = XmCreateToggleButton (rb_w, "Geocentric",
								    args, n);
		XtManageChild (tb1);
		n = 0;
		tb2 = XmCreateToggleButton (rb_w, "Topocentric", args, n);
		XtManageChild (tb2);
		/* if neither or both is set up in defaults, set for geocntrc */
		if (XmToggleButtonGetState(tb1) == XmToggleButtonGetState(tb2)){
		    xe_msg ("conflicting Centric resources -- defaulting to Geocentric\n", 0);
		    XmToggleButtonSetState (tb1, True, True);
		}

	/* now fill in the column entries */
	for (i = 0; i < NC; i++) {
	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    switch (col[i].type) {
	    case MISC_COL:
		/* one of the misc columns */
		w = col[i].sw = XmCreateToggleButton(mrc_w, col[i].name,args,n);
		XtManageChild (w);
		break;
	    case RISET_COL:
		/* one of the rise/set columns */
		w = col[i].sw = XmCreateToggleButton(rsrc_w,col[i].name,args,n);
		XtManageChild (w);
		break;
	    case SEP_COL: {
		/* one of the separation columns */
		Obj *op = db_basic (col[i].dbidx);
		if (op->type != UNDEFOBJ) {
		    w = col[i].sw =
				XmCreateToggleButton(src_w,op->o_name,args,n);
		    XtManageChild (w);
		} else
		    col[i].sw = XmCreateToggleButton(src_w, "ColSelObj",args,n);
		break;
		}
	    default:
		printf ("Bug: %s: col[%d].type = 0x%x\n", me, i, col[i].type);
		exit (1);
		break;
	    }
	}
}

/* set up the Data selection row menu based on what is currently on and defined.
 */
static void
ds_setup_row_selections()
{
	int i;

	for (i = 0; i < NR; i++) {
	    Widget sw = row[i].sw;
	    Obj *op = db_basic (row[i].dbidx);
	    if (op->type == UNDEFOBJ)
		XtUnmanageChild (sw);
	    else 
		XtManageChild (sw);
	    XmToggleButtonSetState (sw, row[i].on, False);
	}
}

/* set up a Data selection col menu based on what is currently on and defined.
 */
static void
ds_setup_col_selections(what)
int what;
{
	int i;

	for (i = 0; i < NC; i++) {
	    if (col[i].type == what) {
		Widget sw = col[i].sw;
		if (col[i].type == SEP_COL) {
		    Obj *op = db_basic (col[i].dbidx);
		    if (op->type == UNDEFOBJ)
			XtUnmanageChild (sw);
		    else 
			XtManageChild (sw);
		}
		XmToggleButtonSetState (sw, col[i].on, False);
	    }
	}

	switch (what) {
	case RISET_COL:
	    XmToggleButtonSetState (stdrefr_w, horizon == STDREFR, True);
	    XmToggleButtonSetState (limb_w, limb == LIMB, True);
	    break;
	case SEP_COL:
	    XmToggleButtonSetState (geocen_w, centric == GEO_CEN, True);
	    break;
	}
}


/* change the Data table according to what is now defined and set up in the
 * Selection menu.
 * N.B. can be called before we are managed.
 */
static void
ds_apply_selections()
{
	int i, c;
	int n_riset, n_sep;
	int wasman;

	watch_cursor(1);

	if ((wasman = XtIsManaged(dataform_w)))
	    XtUnmanageChild (dataform_w);

	for (i = 0; i < NR; i++) {
	    int wantset = XmToggleButtonGetState(row[i].sw);
	    if (wantset != row[i].on) {
		if (wantset) {
		    for (c = 0; c < NC; c++)
			XtManageChild (t_w[i][c]);
		    XtManageChild (row[i].lw);
		} else {
		    for (c = 0; c < NC; c++)
			XtUnmanageChild (t_w[i][c]);
		    XtUnmanageChild (row[i].lw);
		}
		row[i].on = wantset;
	    }
	}

	n_riset = n_sep = 0;
	for (i = 0; i < NC; i++) {
	    int wantset = XmToggleButtonGetState(col[i].sw);
	    if (wantset != col[i].on) {
		if (wantset)
		    XtManageChild (col[i].rcw);
		else
		    XtUnmanageChild (col[i].rcw);
		col[i].on = wantset;
	    }
	    if (col[i].type == RISET_COL && col[i].on)
		n_riset++;
	    if (col[i].type == SEP_COL && col[i].on)
		n_sep++;
	}

	horizon = XmToggleButtonGetState (stdrefr_w) ? STDREFR : ADPREFR;
	limb = XmToggleButtonGetState (limb_w) ? LIMB : CENTER;
	if (n_riset) {
	    f_showit (refr_w, horizon==STDREFR ? "StdRefr" : "AdpRefr");
	    XtManageChild (XtParent(refr_w));
	    f_showit (limbl_w, limb==LIMB ? "UpLimb" : "Center");
	    XtManageChild (XtParent(limbl_w));
	} else {
	    XtUnmanageChild (XtParent(refr_w));
	    XtUnmanageChild (XtParent(limbl_w));
	}

	centric = XmToggleButtonGetState (geocen_w) ? GEO_CEN : TOPO_CEN;
	if (n_sep) {
	    f_string (centric_w,
		    centric==GEO_CEN ? "GeoSeps" : "TopoSeps");
	    XtManageChild (XtParent(centric_w));
	} else
	    XtUnmanageChild (XtParent(centric_w));

	if (wasman)
	    XtManageChild (dataform_w);

	watch_cursor(0);
}

/* callback from any of the Data selection control panel buttons.
 * which is in client.
 */
static void
/* ARGSUSED */
ds_ctl_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int id = (int) client;

	switch (id) {
	case OK:
	    ds_apply_selections();
	    dm_update (mm_get_now(), 1);
	    XtUnmanageChild (sel_w);
	    break;
	case APPLY:
	    ds_apply_selections();
	    dm_update (mm_get_now(), 1);
	    break;
	case CANCEL:
	    XtUnmanageChild (sel_w);
	    break;
	case HELP:
	    ds_help();
	    break;
	}
}

/* called from the Data selection table Help button
 */
static void
ds_help ()
{
	static char *msg[] = {
"This table lets you configure the rows and columns of the data table."
};

	hlp_dialog ("DataSelection Table", msg, XtNumber(msg));
}

/* callback from the stdrefr_w tb.
 */
/* ARGSUSED */
static void
ds_refr_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtSetSensitive (adprefr_w, !XmToggleButtonGetState(w));
}

/* callback from the Data selection row toggle button.
 */
/* ARGSUSED */
static void
ds_row_toggle_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int i;

	for (i = 0; i < NR; i++) {
	    Widget sw = row[i].sw;
	    if (XtIsManaged(sw))
		XmToggleButtonSetState (sw,
		    !XmToggleButtonGetState(sw), False);
	}
}

/* callback from any of the Data selection col toggle button.
 */
static void
/* ARGSUSED */
ds_col_toggle_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int what = (int)client;
	int i;

	for (i = 0; i < NC; i++)
	    if (col[i].type == what) {
		Widget sw = col[i].sw;
		if (XtIsManaged(sw))
		    XmToggleButtonSetState (sw,
			!XmToggleButtonGetState(sw), False);
	    }
}

/* callback from the Data selection row all toggle button.
 */
static void
/* ARGSUSED */
ds_row_all_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int i;

	for (i = 0; i < NR; i++) {
	    Widget sw = row[i].sw;
	    if (XtIsManaged(sw) && !XmToggleButtonGetState (sw))
		XmToggleButtonSetState(sw, True, False);
	}
}

/* callback from any of the Data selection col all toggle buttons.
 */
static void
/* ARGSUSED */
ds_col_all_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int what = (int)client;
	int i;

	for (i = 0; i < NC; i++)
	    if (col[i].type == what) {
	        Widget sw = col[i].sw;
		if (XtIsManaged(sw) && !XmToggleButtonGetState (sw))
		    XmToggleButtonSetState(sw, True, False);
	    }
}

/* callback from the Data selection row reset button.
 */
static void
/* ARGSUSED */
ds_row_reset_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	ds_setup_row_selections();
}

/* callback from any of the Data selection col reset buttons.
 */
/* ARGSUSED */
static void
ds_col_reset_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	ds_setup_col_selections((int)client);
}
