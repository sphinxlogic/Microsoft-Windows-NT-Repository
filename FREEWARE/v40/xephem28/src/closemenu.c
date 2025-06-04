/* code to handle the close objects menu.
 *
 * the basic idea is to sort all objects into bands of dec (or alt).
 * then scan for pairs of close objects within one and adjacent bands.
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#if defined(__STDC__)
#include <stdlib.h>
typedef const void * qsort_arg;
#else
typedef void * qsort_arg;
extern void *calloc(), *malloc(), *realloc();
#endif

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/SelectioB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/List.h>
#include <Xm/Separator.h>
#include <Xm/TextF.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"

extern Widget	toplevel_w;
extern char	*myclass;

extern FILE *fopenh P_((char *name, char *how));
extern Now *mm_get_now P_((void));
extern Obj *db_scan P_((DBScan *sp));
extern char *syserrstr P_((void));
extern double atod P_((char *buf));
extern int db_n P_((void));
extern int existsh P_((char *filename));
extern void db_scaninit P_((DBScan *sp));
extern void db_update P_((Obj *op));
extern void fs_pangle P_((char out[], double a));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void get_xmstring P_((Widget w, char *resource, char **txtp));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void pm_set P_((int percentage));
extern void prompt_map_cb P_((Widget w, XtPointer client, XtPointer call));
extern void query P_((Widget tw, char *msg, char *label1, char *label2,
    char *label3, void (*func1)(), void (*func2)(), void (*func3)()));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void sv_point P_((Obj *op));
extern void timestamp P_((Now *np, Widget w));
extern void watch_cursor P_((int want));
extern void xe_msg P_((char *msg, int app_modal));

#define	MINSEP	degrad(.1)	/* smallest sep we allow.
				 * this is not a hard limit, just a sanity
				 * check. as sep shrinks, we make more and
				 * more tiny little malloced bands.
				 */

/* record of a band.
 * opp is malloced -- can hold nmem, nuse of which are in use.
 */
typedef struct {
    int nuse;		/* number of Obj * actually in use in opp now */
    int nmem;		/* number of Obj * there is room for in opp now */
    Obj **opp;		/* malloced list of Object pointers, or NULL */
} Band;

/* record of a pair.
 * used to collect pairs as the bands are scanned.
 */
typedef struct {
    Obj *op1, *op2;	/* two objects */
    float sep;		/* separation, rads (float to save memory) */
} Pair;

static void c_create_form P_((void));
static void c_help_cb P_((Widget w, XtPointer client, XtPointer call));
static void c_helpon_cb P_((Widget w, XtPointer client, XtPointer call));
static void c_close_cb P_((Widget w, XtPointer client, XtPointer call));
static void c_flist_cb P_((Widget w, XtPointer client, XtPointer call));
static void c_topo_cb P_((Widget w, XtPointer client, XtPointer call));
static void c_actlist_cb P_((Widget w, XtPointer client, XtPointer call));
static void c_skypoint_cb P_((Widget w, XtPointer client, XtPointer call));
static void c_go_cb P_((Widget w, XtPointer client, XtPointer call));
static int sky_point P_((void));
static void do_search P_((void));
static int get_options P_((double *sp, double *mp, int *wp, int *tp));
static void free_band P_((Band bp[], int nb));
static int init_bands P_((double sep, double mag, int topo, Band **bpp));
static int add_obj P_((Band *bp, Obj *op));
static int find_close P_((Band bp[], int nb, double maxsep, int topo,
    int wander, Pair **ppp));
static int add_pair P_((Obj *op1, Obj *op2, double sep, Pair **ppp,
    int *mp, int *np));
static int add_pair P_((Obj *op1, Obj *op2, double sep, Pair **ppp,
    int *mp, int *np));
static int dspl_pairs P_((Pair p[], int np, int topo, double sep));

static void c_create_flist_w P_((void));
static void c_flistok_cb P_((Widget w, XtPointer client, XtPointer call));
static void flistok_append_cb P_((Widget w, XtPointer client, XtPointer call));
static void flistok_overwrite_cb P_((Widget w,XtPointer client,XtPointer call));
static void make_flist P_((char *name, char *how));
static void write_flist P_((FILE *fp));


static Widget cform_w;		/* main Form dialog */
static Widget sep_w;		/* text widget to hold max sep */
static Widget mag_w;		/* text widget to hold mag limit */
static Widget timestmp_w;	/* label for time stamp */
static Widget count_w;		/* label for count of objects in list */
static Widget list_w;		/* the list of close objects */
static Widget flist_w;		/* the file list dialog */
static Widget topo_w;		/* toggle to choose topo/geo */
static Widget geo_w;		/* toggle to choose topo/geo */
static Widget wander_w;		/* toggle to omit pairs of fixed objs*/
static Widget autoup_w;		/* toggle to automatically redo on updates */


void
c_manage()
{
	if (!cform_w)
	    c_create_form();

	if (XtIsManaged(cform_w))
	    XtUnmanageChild (cform_w);
	else
	    XtManageChild (cform_w);
}

/* called when time has advanced.
 * compute a new Close list if desired.
 */
/* ARGSUSED */
void
c_update (np, force)
Now *np;
int force;
{
	if (!cform_w || !XtIsManaged(cform_w)
					|| !XmToggleButtonGetState (autoup_w))
	    return;

	do_search();
}


/* called to put up or remove the watch cursor.  */
void
c_cursor (c)
Cursor c;
{
	Window win;

	if (cform_w && (win = XtWindow(cform_w))) {
	    Display *dsp = XtDisplay(cform_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}

/* create a form dialog to allow user to work with the list. */
static void
c_create_form ()
{
	typedef struct {
	    char *label;	/* what goes on the help label */
	    char *key;		/* string to call hlp_dialog() */
	} HelpOn;
	static HelpOn helpon[] = {
	    {"Intro...",	"Close Objects - intro"},
	    {"on Control...",	"Close Objects - control"},
	    {"on Options...",	"Close Objects - options"},
	    {"Misc...",		"Close Objects - misc"},
	};
	Widget mb_w, pd_w, cb_w;
	Widget s_w;
	XmString str;
	Widget w;
	Arg args[20];
	int n;
	int i;
	
	/* create outter form dialog */

	n = 0;
	XtSetArg(args[n], XmNautoUnmanage, False);  n++;
	XtSetArg(args[n], XmNdefaultPosition, False);  n++;
	XtSetArg (args[n], XmNverticalSpacing, 5); n++;
	XtSetArg (args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
	cform_w = XmCreateFormDialog(toplevel_w, "CloseObjs", args, n);
	XtAddCallback (cform_w, XmNmapCallback, prompt_map_cb, NULL);
	XtAddCallback (cform_w, XmNhelpCallback, c_help_cb, NULL);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Close Objects"); n++;
	XtSetValues (XtParent(cform_w), args, n);

	/* create the menu bar across the top */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	mb_w = XmCreateMenuBar (cform_w, "MB", args, n);
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

	    /* make the Go button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Search", args, n);
	    XtAddCallback (w, XmNactivateCallback, c_go_cb, 0);
	    set_xmstring(w, XmNlabelString, "Go");
	    XtManageChild (w);

	    /* add a separator */

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep2", args, n);
	    XtManageChild (w);
	    
	    /* make the auto update button */

	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    autoup_w = XmCreateToggleButton (pd_w, "AutoUp", args, n);
	    set_xmstring(autoup_w, XmNlabelString, "Auto update");
	    XtManageChild (autoup_w);

	    /* make the sky point button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "SkyPt", args, n);
	    XtAddCallback (w, XmNactivateCallback, c_skypoint_cb, 0);
	    set_xmstring(w, XmNlabelString, "Sky point");
	    XtManageChild (w);

	    /* make the file list button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "File", args, n);
	    XtAddCallback (w, XmNactivateCallback, c_flist_cb, 0);
	    set_xmstring(w, XmNlabelString, "List to file...");
	    XtManageChild (w);

	    /* add a separator */

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep3", args, n);
	    XtManageChild (w);

	    /* add the close button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Close", args, n);
	    XtAddCallback (w, XmNactivateCallback, c_close_cb, 0);
	    XtManageChild (w);

	/* make the options pulldown */

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "OptionsPD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'D'); n++;
	    cb_w = XmCreateCascadeButton (mb_w, "OptionsCB", args, n);
	    set_xmstring (cb_w, XmNlabelString, "Options");
	    XtManageChild (cb_w);
	    
	    /* make the button to select all or just wanderers */

	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    wander_w = XmCreateToggleButton (pd_w, "OmitFixedPairs", args, n);
	    set_xmstring(wander_w, XmNlabelString, "Omit fixed pairs");
	    XtManageChild (wander_w);

	    /* simulate a radio box for the topo/geo pair */

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep1", args, n);
	    XtManageChild (w);

	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY); n++;
	    topo_w = XmCreateToggleButton (pd_w, "Topo", args, n);
	    set_xmstring (topo_w, XmNlabelString, "Topocentric separation");
	    XtAddCallback (topo_w, XmNvalueChangedCallback, c_topo_cb, NULL);
	    XtManageChild (topo_w);

	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY); n++;
	    geo_w = XmCreateToggleButton (pd_w, "Geo", args, n);
	    set_xmstring (geo_w, XmNlabelString, "Geocentric separation");
	    XtAddCallback (geo_w, XmNvalueChangedCallback, c_topo_cb, NULL);
	    XtManageChild (geo_w);
	    if(XmToggleButtonGetState(geo_w) == XmToggleButtonGetState(topo_w)){
		xe_msg("Topo and Geo buttons are the same -- using topo",0);
		XmToggleButtonSetState (topo_w, True, False);
		XmToggleButtonSetState (geo_w, False, False);
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
		XtAddCallback (w, XmNactivateCallback, c_helpon_cb,
							(XtPointer)(hp->key));
		XtManageChild (w);
		XmStringFree(str);
	    }

	/* make the separation label and entry field */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 20); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 47); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	w = XmCreateLabel (cform_w, "SepLabel", args, n);
	set_xmstring (w, XmNlabelString, "Max Sep (degs): ");
	XtManageChild (w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 80); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 53); n++;
	XtSetArg (args[n], XmNcolumns, 10); n++;
	sep_w = XmCreateTextField (cform_w, "Sep", args, n);
	XtManageChild (sep_w);

	/* make the limiting mag label and entry field */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 20); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 47); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
	w = XmCreateLabel (cform_w, "MagLabel", args, n);
	set_xmstring (w, XmNlabelString, "Mag limit: ");
	XtManageChild (w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 80); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 53); n++;
	XtSetArg (args[n], XmNcolumns, 10); n++;
	mag_w = XmCreateTextField (cform_w, "Mag", args, n);
	XtManageChild (mag_w);

	/* make a separator */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mag_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNseparatorType, XmDOUBLE_LINE); n++;
	s_w = XmCreateSeparator (cform_w, "Sep1", args, n);
	XtManageChild (s_w);

	/* make the list count label */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, s_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	count_w = XmCreateLabel (cform_w, "Count", args, n);
	set_xmstring (count_w, XmNlabelString, "0 Pairs");
	XtManageChild (count_w);

	/* make the time stamp label at the bottom */

	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	timestmp_w = XmCreateLabel (cform_w, "TSL", args, n);
	set_xmstring (timestmp_w, XmNlabelString, "    ");
	XtManageChild (timestmp_w);

	/* make the list between the count and the time */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, count_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, timestmp_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNselectionPolicy, XmBROWSE_SELECT); n++;
	XtSetArg (args[n], XmNlistSizePolicy, XmCONSTANT); n++;
	list_w = XmCreateScrolledList (cform_w, "List", args, n);
	XtAddCallback (list_w, XmNdefaultActionCallback, c_actlist_cb, NULL);
	XtManageChild (list_w);
}

/* called when the general help key is pressed */
/* ARGSUSED */
static void
c_help_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static char *msg[] = {
"Select desired max separation and viewpoint, then Control->Start to find all",
"pairs of objects separated by less than the given angular distance.",
};
	
	hlp_dialog ("Close Objects", msg, XtNumber(msg));
}

/* called when any of the individual help entries are selected.
 * client contains the string for hlp_dialog().
 */
/* ARGSUSED */
static void
c_helpon_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	hlp_dialog ((char *)client, NULL, 0);
}

/* callback from file List control button. */
/* ARGSUSED */
static void
c_flist_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (!flist_w)
	    c_create_flist_w();

	if (XtIsManaged(flist_w))
	    XtUnmanageChild (flist_w);
	else
	    XtManageChild (flist_w);
}

/* called when the Close pushbutton is activated */
/* ARGSUSED */
static void
c_close_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (cform_w);
}

/* called when either the topo or geo toggle button changes.
 * we simulate a radio box by flipping the other too.
 */
/* ARGSUSED */
static void
c_topo_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmToggleButtonSetState (w == topo_w ? geo_w : topo_w,
				!XmToggleButtonGetState(w), False);
}

/* called when a list item is double-clicked */
/* ARGSUSED */
static void
c_actlist_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	(void) sky_point();
}

/* called when the Sky Point pushbutton is activated */
/* ARGSUSED */
static void
c_skypoint_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (sky_point() < 0)
	    xe_msg ("First select a line from the list.", 1);
}

/* called when the Perform Search pushbutton is activated */
/* ARGSUSED */
static void
c_go_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	do_search();
}

/* tell sky view to point to the first object named in the current selection.
 * return 0 if ok, else -1.
 */
static int
sky_point()
{
	String sel;
	char objname[MAXNM];
	XmStringTable si;
	int sic;
	DBScan dbs;
	Obj *op;
	int i;

	/* get the current select item, if any */
	get_something (list_w, XmNselectedItemCount, (XtArgVal)&sic);
	if (sic < 1)
	    return (-1);
	get_something (list_w, XmNselectedItems, (XtArgVal)&si); /* don't free*/
	XmStringGetLtoR (si[0], XmSTRING_DEFAULT_CHARSET, &sel); /* free */

	/* extract the name of the first object -- see dspl_pairs's formating */
	(void) strncpy (objname, sel+7, sizeof(objname));
	XtFree (sel);
	objname[sizeof(objname)-1] = '\0';
	for (i = MAXNM-2; i >= 0; --i)
	    if (objname[i] == ' ')
		objname[i] = '\0';
	    else
		break;

	/* scan the db for the object */
	for (db_scaninit (&dbs); (op = db_scan(&dbs)) != NULL; )
	    if (op->type != UNDEFOBJ && strcmp (op->o_name, objname) == 0)
		break;

	/* point sky if found */
	if (op)
	    sv_point (op);
	else {
	    char msg[128];

	    (void) sprintf (msg, "Can not find `%s' in memory!", objname);
	    xe_msg (msg, 1);
	}

	return (0);
}

/* given two pointers to Obj *, return how they sort by alt in qsort-style.
 */
static int
azcmp_f (v1, v2)
qsort_arg v1;
qsort_arg v2;
{
	Obj *op1 = *((Obj **)v1);
	Obj *op2 = *((Obj **)v2);
	double az1 = op1->s_az;
	double az2 = op2->s_az;

	if (az1 < az2)
	    return (-1);
	if (az1 > az2)
	    return (1);
	return (0);
}

/* given two pointers to Obj *, return how they sort by dec in qsort-style.
 */
static int
racmp_f (v1, v2)
qsort_arg v1;
qsort_arg v2;
{
	Obj *op1 = *((Obj **)v1);
	Obj *op2 = *((Obj **)v2);
	double ra1 = op1->s_ra;
	double ra2 = op2->s_ra;

	if (ra1 < ra2)
	    return (-1);
	if (ra1 > ra2)
	    return (1);
	return (0);
}

static void
do_search()
{
	double sep;	/* desired max separation, rads */
	double mag;	/* limiting magnitude */
	int wander;	/* 1 if want to limit list to at least one wanderer */
	int topo;	/* work in topocentric alt/az, else geocentric ra/dec */
	Band *bp = NULL;/* malloced array of nb bands */
	int nb;		/* number of dec (or alt) bands */
	Pair *pp = NULL;/* malloced list of close pairs */
	int np;		/* number of pairs */

	/* retrieve user option settings */
	if (get_options (&sep, &mag, &wander, &topo) < 0)
	    return;

	/* ok, let's get to the real work */
	watch_cursor (1);

	/* compute band size, deal and sort objects into their own and adjacent
	 * bands.
	 */
	nb = init_bands (sep, mag, topo, &bp);
	if (nb < 0) {
	    watch_cursor (0);
	    return;
	}

	/* look through bands for all pairs closer than sep.
	 * form sorted lists of them in malloced pp array.
	 */
	np = find_close (bp, nb, sep, topo, wander, &pp);

	/* finished with the bands now */
	free_band (bp, nb);

	/* report what we found */
	if (np > 0)
	    (void) dspl_pairs (pp, np, topo, sep);
	else
	    xe_msg ("No such pairs found.", 1);

	/* finished */
	if (pp)
	    free ((char *)pp);
	watch_cursor (0);
}

/* retrieve the desired separation and other user option settings.
 * return 0 if ok, else -1
 */
static int
get_options (sp, mp, wp, tp)
double *sp;	/* max separation, rads */
double *mp;	/* limiting magnitude */
int *wp;	/* wanderer flag */
int *tp;	/* topocentric flag */
{
	char *str;

	str = XmTextFieldGetString (sep_w);
	*sp = degrad(atod(str));
	XtFree (str);
	if (*sp < MINSEP) {
	    char msg[128];

	    (void)sprintf(msg, "Please specify a max separation >= %g degrees.",
	    							raddeg(MINSEP));
	    xe_msg (msg, 1);
	    return (-1);
	}

	str = XmTextFieldGetString (mag_w);
	*mp = atod(str);
	XtFree (str);

	*wp = XmToggleButtonGetState (wander_w);
	*tp = XmToggleButtonGetState (topo_w);

	return (0);
}

/* free a list of nb Bands, including their opp.
 * allow for NULL pointers at any step.
 */
static void
free_band (bp, nb)
Band bp[];
int nb;
{
	Band *bpsav = bp;

	if (!bp)
	    return;

	for (bp += nb; --bp >= bpsav; )
	    if (bp->opp)
		free ((char *)bp->opp);

	free ((char *)bpsav);
}

/* given separation and other factors, build up the array of bands:
 *   find the number of bands and malloc array of bands at *bpp;
 *   deal each qualified object into its own and adjacent-up band;
 *   sort each band according to increasing ra (or az).
 * if all ok return number of bands created at *bpp, else -1.
 * N.B. caller must free_band(*bpp) if we return >= 0.
 */
static int
init_bands (sep, mag, topo, bpp)
double sep;
double mag;
int topo;
Band **bpp;
{
#define	BAND(x)		((int)floor(((x)+PI/2)/bandsiz))
	int (*cmp_f)();
	double bandsiz;
	Band *bp;
	DBScan dbs;
	char msg[1024];
	int nobj;
	int nb;
	Obj *op;
	int n, i;

	/* compute number of bands and band size */
	nb = (int)ceil(PI/sep);
	if (nb < 1) {
	    xe_msg ("Please decrease max separation to < 180 degrees.", 1);
	    return (-1);
	}
	bandsiz = PI/nb;

	/* malloc list of zero'd Bands */
	bp = (Band *) calloc (nb, sizeof(Band));
	if (!bp) {
	    (void) sprintf (msg,
	    	"Could not malloc %d bands -- try increasing max separation.",
									nb);
	    xe_msg (msg, 1);
	    return(-1);
	}

	/* scan db and fill in bands.
	 * each object goes in its own band and the one adjacent.
	 * we do not include undefined user objects nor those too dim or low.
	 * we also check for entries with bogus bands but should never be any.
	 */
	nobj = db_n();
	n = 0;
	for (db_scaninit(&dbs); (op = db_scan(&dbs)) != NULL; ) {
	    int b;

	    pm_set (33 * n++ / nobj);	/* about 0-33% of time here */

	    /* qualify object */
	    if (op->type == UNDEFOBJ)
		continue;
	    db_update (op);
	    if (op->s_mag/MAGSCALE > mag)
		continue;
	    if (topo && op->s_alt < 0.0)
		continue;

	    /* find which band op fits into */
	    b = topo ? BAND(op->s_alt) : BAND(op->s_dec);
	    if (b < 0 || b > nb) {
		(void) sprintf (msg, "%s is out of band: %d.", op->o_name, b);
		xe_msg (msg, 0);
		continue;
	    }

	    /* add op to its band */
	    if (add_obj (&bp[b], op) < 0) {
		free_band (bp, nb);
		return (-1);
	    }

	    /* unless we are at the top (N pole) add op to next band up too */
	    if (b < nb-1 && add_obj (&bp[b+1], op) < 0) {
		free_band (bp, nb);
		return (-1);
	    }
	}

	/* sort each band by ra (az)
	 */
	cmp_f = topo ? azcmp_f : racmp_f;
	for (i = 0; i < nb; i++)
	    if (bp[i].nuse > 0)
		qsort ((void *)bp[i].opp, bp[i].nuse, sizeof (Obj *), cmp_f);

	/* report answers and return */
	*bpp = bp;
	return (nb);
}

/* add one more op to Band bp, growing opp as necessary.
 * return 0 if ok, else xe_msg() and -1.
 */
static int
add_obj (bp, op)
Band *bp;
Obj *op;
{
#define	BCHUNKS	32	/* grow the Obj * list at opp this much at a time */

	/* init the list if this is the first time. */
	if (!bp->opp) {
	    char *newmem;

	    newmem = malloc (BCHUNKS * sizeof(Obj *));
	    if (!newmem) {
		xe_msg ("Can not malloc temp Objects.", 1);
		return (-1);
	    }

	    bp->opp = (Obj **) newmem;
	    bp->nmem = BCHUNKS;
	    bp->nuse = 0;
	}

	/* grow the list if necessary */
	if (bp->nuse == bp->nmem) {
	    char *newmem;

	    newmem = realloc((char *)bp->opp, (bp->nmem+BCHUNKS)*sizeof(Obj *));
	    if (!newmem) {
		xe_msg ("Can not realloc more temp Objects.", 1);
		return (-1);
	    }

	    bp->opp = (Obj **) newmem;
	    bp->nmem += BCHUNKS;
	}

	bp->opp[bp->nuse++] = op;

	return (0);
}

/* malloc and fill *ppp with the close pairs in the sorted bands.
 * return count of new items in *ppp if ok else -1.
 * N.B. caller must free *ppp if it is != NULL no matter what we return.
 */
static int
find_close (bp, nb, maxsep, topo, wander, ppp)
Band bp[];	/* list of bands to scan */
int nb;		/* number of bands */
double maxsep;	/* max sep we are looking for, rads */
int topo;	/* 1 if want topocentric alt/az or geocentric ra/dec */
int wander;	/* 1 if limit to pairs with at least one wanderer (ss object) */
Pair **ppp;	/* return list of pairs we malloc */
{
	double stopsep = maxsep*2.3; /* stop scan at maxsep*sqrt(1**2 + 2**2) */
	int m = 0;		/* n pairs there are room for in *ppp */
	int n = 0;		/* n pairs in use in *ppp */
	int b;


	/* be sure everyone knows nothing has been malloced yet */
	*ppp = NULL;

	/* scan each band */
	for (b = 0; b < nb; b++, bp++) {
	    int nuse = bp->nuse;
	    int i;

	    pm_set (33 + 33 * b / nb);	/* about 33-66% more of time here */

	    /* compare against each object in this band */
	    for (i = 0; i < nuse; i++) {
		Obj *opi = bp->opp[i];
		int j;

		/* set up for finding sep -- see solve_sphere() -- i is c */
		double cc = sin(topo ? opi->s_alt : opi->s_dec);
		double sc = cos(topo ? opi->s_alt : opi->s_dec);

		/* scan forward for close objects in the same band.
		 * wrap, but not so far as to reach self again.
		 * since bands are sorted we can cut off when find one too far.
		 * but this _can_ lead to dups for very short band lists.
		 */
		for (j = (i+1)%nuse; j != i; j = (j+1)%nuse) {
		    Obj *opj = bp->opp[j];
		    double cb, sb;
		    double A, cA;
		    double sep, csep;

		    /* omit if both are fixed and want at least one wanderer */
		    if (wander && opi->type == FIXED && opj->type == FIXED)
			continue;

		    /* find the exact separation */
		    cb = sin(topo ? opj->s_alt : opj->s_dec); /* j is b */
		    sb = cos(topo ? opj->s_alt : opj->s_dec);
		    A = topo ? opi->s_az - opj->s_az : opi->s_ra - opj->s_ra;
		    cA = cos(A);
		    csep = cb*cc + sb*sc*cA;
		    if (csep >  1.0) csep =  1.0;	/* just paranoid */
		    if (csep < -1.0) csep = -1.0;
		    sep = acos(csep);

		    /* stop when we're so far from opi there can be no more */
		    if (sep > stopsep)
			break;

		    if (sep <= maxsep)
			if (add_pair (opi, opj, sep, ppp, &m, &n) < 0)
			    return (-1);
		}
	    }
	}

	return (n);
}

/* add the two objects to the list of pairs.
 * we malloc/realloc as needed to grow *ppp, updating {m,n}p.
 * put the smaller of op1/2 in slot op1 for later dup checking.
 * return 0 if ok, else -1.
 * N.B. the caller must free *ppp if it is not NULL no matter what we return.
 */
static int
add_pair (op1, op2, sep, ppp, mp, np)
Obj *op1, *op2;	/* the two objects to report */
double sep;	/* separation, rads */
Pair **ppp;	/* pointer to malloced list of lines */
int *mp;	/* pointer to number of Pairs already in *ppp */
int *np;	/* pointer to number of *ppp actually in use */
{
#define	PCHUNKS	32	/* grow the report list this many at a time */
	Pair *newp;

	/* init the list if this is the first time. */
	if (!*ppp) {
	    char *newmem;

	    newmem = malloc (PCHUNKS * sizeof(Pair));
	    if (!newmem) {
		xe_msg ("Can not malloc any Pairs.", 1);
		return (-1);
	    }

	    *ppp = (Pair *) newmem;
	    *mp = PCHUNKS;
	    *np = 0;
	}

	/* grow the list if necessary */
	if (*np == *mp) {
	    char *newmem;

	    newmem = realloc((char *)*ppp, (*mp+PCHUNKS) * sizeof(Pair));
	    if (!newmem) {
		xe_msg ("Can not realloc more Pairs.", 1);
		return (-1);
	    }

	    *ppp = (Pair *) newmem;
	    *mp += PCHUNKS;
	}

	newp = &(*ppp)[*np];
	*np += 1;

	/* this lets us sort pairs of objects when their sep matches for
	 * later elimination of dups.
	 */
	if (op1 < op2) {
	    newp->op1 = op1;
	    newp->op2 = op2;
	} else {
	    newp->op1 = op2;
	    newp->op2 = op1;
	}
	newp->sep = (float)sep;

	return (0);
}

/* sort the two Pairs according to increasing separation in qsort() fashion
 * then by memory location of op1 then op2 -- this way two pairs of the same
 *   objects will sort adjacent, and is used later to elminate dups.
 */
static int
pair_cmpf (v1, v2)
qsort_arg v1;
qsort_arg v2;
{
	Pair *p1 = (Pair *)v1;
	Pair *p2 = (Pair *)v2;
	double s1 = (double)p1->sep;
	double s2 = (double)p2->sep;

	if (s1 < s2)
	    return (-1);
	else if (s1 > s2)
	    return (1);
	else {
	    Obj *opa, *opb;

	    opa = p1->op1;
	    opb = p2->op1;
	    if (opa < opb)
		return (-1);
	    else if (opa > opb)
		return (1);

	    opa = p1->op2;
	    opb = p2->op2;
	    if (opa < opb)
		return (-1);
	    else if (opa > opb)
		return (1);
	    else
		return (0);
	}
}

/* fill the list widget with the given set of pairs.
 * also update the attendent messages.
 * return 0 if ok else -1.
 */
static int
dspl_pairs (pp, np, topo, sep)
Pair pp[];
int np;
int topo;
double sep;
{
	XmString *xmstrtbl;
	char buf[128];
	char sepstr[64];
	int ns;
	int i;

	/* sort pairs by increasing separation then by object pointers */
	qsort ((void *)pp, np, sizeof(Pair), pair_cmpf);

	/* put into the list in one big push for best performance.
	 * beware of and remove any dups.
	 * ns ends up as the number of strings in xmstrtbl[].
	 */
	xmstrtbl = (XmString *) malloc (np * sizeof(XmString));
	if (!xmstrtbl) {
	    (void) sprintf (buf, "No memory for %d XmStrings", np);
	    xe_msg (buf, 1);
	    return (-1);
	}
	for (ns = i = 0; i < np; i++) {
	    Pair *ppi = &pp[i];

	    pm_set (66 + 29 * i / np);	/* about 66-95% time here */

	    /* can eliminate dups this way because of sort */
	    if (i > 0) {
		Pair *lpp = &pp[i-1];

		if (ppi->op1 == lpp->op1 && ppi->op2 == lpp->op2)
		    continue;
	    }

	    /* N.B. if you change this format, rework sky_point's extraction */

	    fs_pangle (sepstr, (double)ppi->sep);
	    (void) sprintf (buf, "%6.2f %-*.*s   %6.2f %-*.*s   %s",
					    ppi->op1->s_mag/MAGSCALE,
					    MAXNM-1, MAXNM-1, ppi->op1->o_name,
					    ppi->op2->s_mag/MAGSCALE,
					    MAXNM-1, MAXNM-1, ppi->op2->o_name,
					    sepstr);
	    xmstrtbl[ns++] = XmStringCreateSimple (buf);
	}

	/* reload the list */
	XtUnmanageChild (list_w);
	XmListDeleteAllItems (list_w);
	XmListAddItems (list_w, xmstrtbl, ns, 0);
	XtManageChild (list_w);

	/* finished with xmstrtbl */
	for (i = 0; i < ns; i++) 
	    XmStringFree (xmstrtbl[i]);
	free ((char *)xmstrtbl);

	/* update the messages */
	fs_pangle (sepstr, sep);
	(void) sprintf (buf, "%d %s Pair%s <=%s", ns,
		topo ? "Topocentric" : "Geocentric", ns==1 ? "" : "s", sepstr);
	set_xmstring (count_w, XmNlabelString, buf);
	timestamp (mm_get_now(), timestmp_w);

	pm_set (100);

	return (0);
}

/* create the list filename prompt */
static void
c_create_flist_w()
{
	Arg args[20];
	int n;

	n = 0;
	XtSetArg(args[n], XmNdefaultPosition, False);  n++;
	flist_w = XmCreatePromptDialog (toplevel_w, "CloseList", args,n);
	set_xmstring (flist_w, XmNdialogTitle, "xephem Close list");
	set_xmstring (flist_w, XmNselectionLabelString, "File name:");
	XtUnmanageChild (XmSelectionBoxGetChild(flist_w, XmDIALOG_HELP_BUTTON));
	XtAddCallback (flist_w, XmNokCallback, c_flistok_cb, NULL);
	XtAddCallback (flist_w, XmNmapCallback, prompt_map_cb, NULL);
}

/* called when the Ok button is hit in the file flist prompt */
/* ARGSUSED */
static void
c_flistok_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	char buf[1024];
	int icount;
	char *name;

	get_xmstring(w, XmNtextString, &name);

	if (strlen(name) == 0) {
	    xe_msg ("Please enter a file name.", 1);
	    XtFree (name);
	    return;
	}

	get_something (list_w, XmNitemCount, (XtArgVal)&icount);
	if (icount == 0) {
	    xe_msg ("No items in list -- no file action.", 1);
	    XtFree (name);
	    return;
	}

	if (existsh (name) == 0) {
	    (void) sprintf (buf, "%s exists: Append or Overwrite?", name);
	    query (toplevel_w, buf, "Append", "Overwrite", "Cancel",
				flistok_append_cb, flistok_overwrite_cb, NULL);
	} else
	    make_flist (name, "w");

	XtFree (name);
}

/* called when we want to append to a flist file */
/* ARGSUSED */
static void
flistok_append_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	char *name;

	get_xmstring (flist_w, XmNtextString, &name);
	make_flist (name, "a");
	XtFree (name);
}

/* called when we want to ceate a new flist file */
/* ARGSUSED */
static void
flistok_overwrite_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	char *name;

	get_xmstring (flist_w, XmNtextString, &name);
	make_flist (name, "w");
	XtFree (name);
}

/* open the named flist file "a" or "w" and fill it in. */
static void
make_flist (name, how)
char *name;
char *how;
{
	FILE *fp = fopenh (name, how);

	if (fp) {
	    write_flist (fp);
	    (void) fclose (fp);
	} else {
	    char buf[1024];

	    (void) sprintf (buf, "Can not open %s: %s", name, syserrstr());
	    xe_msg (buf, 1);
	}
}

/* write out all objects currently in the list, with header, to fp */
static void
write_flist (fp)
FILE *fp;
{
	XmStringTable items;
	int icount;
	char *p;
	int i;

	get_something (list_w, XmNitemCount, (XtArgVal)&icount);
	if (icount <= 0)
	    return;

	watch_cursor(1);

	get_xmstring (count_w, XmNlabelString, &p);
	(void) fprintf (fp, "%s", p);
	XtFree (p);
	get_xmstring (timestmp_w, XmNlabelString, &p);
	(void) fprintf (fp, " at %s\n", p);
	XtFree (p);

	get_something (list_w, XmNitems, (XtArgVal)&items);

	for (i = 0; i < icount; i++) {
	    XmStringGetLtoR (items[i], XmSTRING_DEFAULT_CHARSET, &p);
	    (void) fprintf (fp, "%s\n", p);
	    XtFree (p);
	}

	watch_cursor(0);
}
