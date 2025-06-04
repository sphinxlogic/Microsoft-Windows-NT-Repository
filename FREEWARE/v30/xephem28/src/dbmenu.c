/* code to manage the stuff on the "database" menu.
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
#include <Xm/CascadeB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/FileSB.h>
#include <Xm/Separator.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"

extern Widget	toplevel_w;
extern char	*myclass;
#define	XtD	XtDisplay(toplevel_w)

extern FILE *fopenh P_((char *name, char *how));
extern Obj *db_scan P_((DBScan *sp));
extern char *syserrstr P_((void));
extern int db_n P_((void));
extern int db_n_cp P_((void));
extern int db_read P_((FILE *fp));
extern void all_newdb P_((int appended));
extern void db_clr_cp P_((void));
extern void db_connect_fifo P_((void));
extern void db_del_all P_((void));
extern void db_del_cp P_((void));
extern void db_scaninit P_((DBScan *sp));
extern void db_set_cp P_((void));
extern void db_setnodups P_((int whether));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void prompt_map_cb P_((Widget w, XtPointer client, XtPointer call));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void query P_((Widget tw, char *msg, char *label1, char *label2,
    char *label3, void (*func1)(), void (*func2)(), void (*func3)()));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void watch_cursor P_((int want));
extern void xe_msg P_((char *msg, int app_modal));

static void db_create_form P_((void));
static void initFSB P_((Widget fsb_w));
static void db_set_report P_((void));
static FILE *fileopen P_((char *name));
static void db_append_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_del_cp_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_set_cp_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_clr_cp_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_nodups_cb P_((Widget w, XtPointer client, XtPointer data));
static void delete_all_confirmed P_((void));
static void db_del_all_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_openfifo_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_help_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_helpon_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_close_cb P_((Widget w, XtPointer client, XtPointer data));

static Widget dbform_w;			/* main Form dialog */
static Widget report_w;			/* label with the dbstats */

void
db_manage()
{
	if (!dbform_w)
	    db_create_form();

	if (XtIsManaged(dbform_w))
	    XtUnmanageChild (dbform_w);
	else {
	    db_set_report();	/* get a fresh count */
	    XtManageChild (dbform_w);
	}
}

/* called when the database beyond NOBJ has changed in any way.
 * as odd as this seems since *this* menu controls the db contents, this was
 *   added when the db fifo input path was added. it continued to be handy
 *   when initial db files could be loaded automatically and we started 
 *   to include info about the user-defined objects in the tallys in v2.7.
 * all we do is update our tally, if we are up at all.
 */
/* ARGSUSED */
void
db_newdb (appended)
int appended;
{
	if (dbform_w && XtIsManaged (dbform_w))
	    db_set_report();
}

/* called when either of the user-defined objects has changed.
 * all we do is update our tally, if we are up at all.
 */
/* ARGSUSED */
void
db_newobj (id)
int id;	/* OBJX or OBJY */
{
	if (dbform_w && XtIsManaged (dbform_w))
	    db_set_report();
}

/* called to put up or remove the watch cursor.  */
void
db_cursor (c)
Cursor c;
{
	Window win;

	if (dbform_w && (win = XtWindow(dbform_w))) {
	    Display *dsp = XtDisplay(dbform_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}

/* create a form dialog to allow user to enter a db file name. */
static void
db_create_form ()
{
	typedef struct {
	    char *label;	/* what goes on the help label */
	    char *key;		/* string to call hlp_dialog() */
	} HelpOn;
	static HelpOn helpon[] = {
	    {"Intro...", "Database - intro"},
	    {"on Control...", "Database - control"},
	    {"on Delete...", "Database - delete"},
	    {"File format...", "Database - files"},
	    {"Notes...", "Database - notes"},
	};
	Widget mb_w, pd_w, cb_w;
	Widget fsb_w, sep_w;
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
	dbform_w = XmCreateFormDialog(toplevel_w, "DBManage", args, n);
	XtAddCallback (dbform_w, XmNmapCallback, prompt_map_cb, NULL);
	XtAddCallback (dbform_w, XmNhelpCallback, db_help_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Data Base"); n++;
	XtSetValues (XtParent(dbform_w), args, n);

	/* create the menu bar across the top */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	mb_w = XmCreateMenuBar (dbform_w, "MB", args, n);
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

	    /* make the nodups toggle */

	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    w = XmCreateToggleButton (pd_w, "NoDups", args, n);
	    XtAddCallback (w, XmNvalueChangedCallback, db_nodups_cb, 0);
	    set_xmstring(w, XmNlabelString, "No dups");
	    XtManageChild (w);
	    db_setnodups (XmToggleButtonGetState(w));

	    /* make the open fifo button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "OpenFIFO", args, n);
	    XtAddCallback (w, XmNactivateCallback, db_openfifo_cb, 0);
	    set_xmstring(w, XmNlabelString, "Open DBFIFO");
	    XtManageChild (w);

	    /* add a separator */

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    /* add the close button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Close", args, n);
	    XtAddCallback (w, XmNactivateCallback, db_close_cb, 0);
	    XtManageChild (w);

	/* make the Delete pulldown */

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "DeletePD", args, n);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pd_w);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'D'); n++;
	    cb_w = XmCreateCascadeButton (mb_w, "DeleteCB", args, n);
	    set_xmstring (cb_w, XmNlabelString, "Delete");
	    XtManageChild (cb_w);

	    /* make the set-checkpoint button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "SetCP", args, n);
	    XtAddCallback (w, XmNactivateCallback, db_set_cp_cb, 0);
	    set_xmstring(w, XmNlabelString, "Set Checkpoint");
	    XtManageChild (w);

	    /* make the clear-checkpoint button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "ClrCP", args, n);
	    XtAddCallback (w, XmNactivateCallback, db_clr_cp_cb, 0);
	    set_xmstring(w, XmNlabelString, "Clear Checkpoint");
	    XtManageChild (w);

	    /* add a separator */

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    /* make the delete-to-checkpoint button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "DelToCP", args, n);
	    XtAddCallback (w, XmNactivateCallback, db_del_cp_cb, 0);
	    set_xmstring(w, XmNlabelString, "Delete to Checkpoint");
	    XtManageChild (w);

	    /* make the delete-all button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "DelAll", args, n);
	    XtAddCallback (w, XmNactivateCallback, db_del_all_cb, 0);
	    set_xmstring(w, XmNlabelString, "Delete All");
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
		XtAddCallback (w, XmNactivateCallback, db_helpon_cb,
							(XtPointer)(hp->key));
		XtManageChild (w);
		XmStringFree(str);
	    }

	/* make the report label */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	report_w = XmCreateLabel (dbform_w, "Report", args, n);
	XtManageChild (report_w);

	/* create separator below report label */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, report_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	sep_w = XmCreateSeparator (dbform_w, "Sep", args, n);
	XtManageChild (sep_w);

	/* make the file selection box */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	fsb_w = XmCreateFileSelectionBox (dbform_w, "FSB", args, n);
	initFSB (fsb_w);	/* do this before managing for Ultrix :-) */
	XtManageChild (fsb_w);

	    /* use the Ok button to mean Append */
	    XtAddCallback (fsb_w, XmNokCallback, db_append_cb, NULL);
	    set_xmstring (fsb_w, XmNokLabelString, "Append");

	    /* don't need Cancel or Help */
	    w = XmFileSelectionBoxGetChild (fsb_w, XmDIALOG_CANCEL_BUTTON);
	    XtUnmanageChild (w);
	    w = XmFileSelectionBoxGetChild (fsb_w, XmDIALOG_HELP_BUTTON);
	    XtUnmanageChild (w);

#if XmVersion >= 1001
	w = XmFileSelectionBoxGetChild (fsb_w, XmDIALOG_OK_BUTTON);
	XmProcessTraversal (w, XmTRAVERSE_CURRENT);
	XmProcessTraversal (w, XmTRAVERSE_CURRENT); /* yes, twice!! */
#endif
}

/* init the directory and pattern resources of the given FileSelectionBox.
 * we try to pull these from the basic program resources.
 */
static void
initFSB (fsb_w)
Widget fsb_w;
{
	static char dbd[] = "DBdirectory";
	static char dbp[] = "DBpattern";
	char msg[128];
	char *res;

	res = XGetDefault (XtD, myclass, dbd);
	if (!res) {
	    res = "edb";
	    (void)sprintf(msg,"Can not get %s.%s -- using %s", myclass,dbd,res);
	    xe_msg (msg, 0);
	}
	set_xmstring (fsb_w, XmNdirectory, res);

	res = XGetDefault (XtD, myclass, dbp);
	if (!res) {
	    res = "*.edb";
	    (void)sprintf(msg,"Can not get %s.%s -- using %s", myclass,dbp,res);
	    xe_msg (msg, 0);
	}
	set_xmstring (fsb_w, XmNpattern, res);
}

/* compile the stats report into report_w.
 * this is a cheap way to indicate the number of objects in the db.
 */
static void
db_set_report()
{
	DBScan dbs;
	char report[1024];
	Obj *op;
	int nes=0, ne=0, np=0, nh=0;
	int nc=0, ng=0, nj=0, nn=0, npn=0, nl=0, nq=0, nr=0, ns=0, no=0;
	int t=0;

	for (db_scaninit(&dbs); (op=db_scan(&dbs))!=NULL; ) {
	    switch (op->type) {
	    case FIXED:
		switch (op->f_class) {
		case 'C': case 'U': case 'O': nc++; t++; break;
		case 'G': case 'H': case 'A': ng++; t++; break;
		case 'N': case 'F': case 'K': nn++; t++; break;
		case 'J': nj++; t++; break;
		case 'P': npn++; t++; break;
		case 'L': nl++; t++; break;
		case 'Q': nq++; t++; break;
		case 'R': nr++; t++; break;
		case 'T': case 'B': case 'D': case 'M': case 'S': case 'V': 
		    ns++; t++; break;
		default: no++; t++; break;
		}
		break;
	    case ELLIPTICAL: ne++; t++; break;
	    case HYPERBOLIC: nh++; t++; break;
	    case PARABOLIC: np++; t++; break;
	    case EARTHSAT: nes++; t++; break;
	    case PLANET: break;
	    case UNDEFOBJ: break;
	    default:
		printf ("Unknown object type: %d\n", op->type);
		exit (1);
	    }
	}

	(void) sprintf (report, "\
%6d Solar -- elliptical\n\
%6d Solar -- hyperbolic\n\
%6d Solar -- parabolic\n\
%6d Earth satellites\n\
%6d Clusters (C,U,O)\n\
%6d Galaxies (G,H,A)\n\
%6d Planetary Nebulae (P)\n\
%6d Nebulae (N,F,K)\n\
%6d Pulsars (L)\n\
%6d Quasars (Q)\n\
%6d Radio sources (J)\n\
%6d Supernova Remnants (R)\n\
%6d Stars (S,V,D,B,M,T)\n\
%6d Undefined\n\
------\n\
%6d Total objects in memory\n\
%6d Objects checkpointed\n\
\n\
",
	ne, nh, np, nes, nc, ng, npn, nn, nl, nq, nj, nr, ns, no, t,
							    db_n_cp()-NOBJ);
	set_xmstring (report_w, XmNlabelString, report);
}

/* try to open name for read access.
 * if successful, return FILE *, else print a message and return NULL.
 */
static FILE *
fileopen (name)
char *name;
{
	FILE *fp;
	
	fp = fopenh (name, "r");
	if (!fp) {
	    char msg[128];
	    (void) sprintf (msg, "Can not open %.75s: %.25s", name,
							syserrstr());
	    xe_msg (msg, 1);
	}
	return (fp);
}

/* callback from the Ok button along the bottom of the FSB.
 * we take this to mean append.
 */
/* ARGSUSED */
static void
db_append_cb (w, client, data)
Widget w;
XtPointer client;
XtPointer data;
{
	XmFileSelectionBoxCallbackStruct *s =
				    (XmFileSelectionBoxCallbackStruct *)data;
	char *sp;
	FILE *fp;

	if (s->reason != XmCR_OK) {
	    printf ("%s: Unknown reason = 0x%x\n", "db_append_cb()", s->reason);
	    exit (1);
	}

	watch_cursor(1);

	XmStringGetLtoR (s->value, XmSTRING_DEFAULT_CHARSET, &sp);
	fp = fileopen (sp);
	if (fp) {
	    if (db_read (fp) < 0) {
		char msg[128];
		(void) sprintf (msg, "Error reading `%.100s'", sp);
		xe_msg (msg, 0);
	    }
	    all_newdb(1);	/* includes us! */
	    (void) fclose (fp);
	}

	XtFree (sp);

	watch_cursor(0);
}

/* callback from the Delete to Checkpoint button */
/* ARGSUSED */
static void
db_del_cp_cb (w, client, data)
Widget w;
XtPointer client;
XtPointer data;
{
	db_del_cp();
	all_newdb(0);
}

/* callback from the Set Checkpoint button */
/* ARGSUSED */
static void
db_set_cp_cb (w, client, data)
Widget w;
XtPointer client;
XtPointer data;
{
	db_set_cp();
	db_set_report();
}

/* callback from the Clear Checkpoint button */
/* ARGSUSED */
static void
db_clr_cp_cb (w, client, data)
Widget w;
XtPointer client;
XtPointer data;
{
	db_clr_cp();	/* also clears out dups if enabled */
	db_set_report();
}

/* callback from the nodups toggle */
/* ARGSUSED */
static void
db_nodups_cb (w, client, data)
Widget w;
XtPointer client;
XtPointer data;
{
	db_setnodups (XmToggleButtonGetState(w));
}

/* function to call when the Delete All confirmation is affirmative */
static void
delete_all_confirmed()
{
	db_del_all();
	all_newdb(0);
}

/* callback from the Delete All button */
/* ARGSUSED */
static void
db_del_all_cb (w, client, data)
Widget w;
XtPointer client;
XtPointer data;
{
	query (toplevel_w, "Delete all objects in memory?", "Yes -- delete all",
		"No -- delete none", NULL, delete_all_confirmed, NULL, NULL);
}

/* callback from the open fifo button */
/* ARGSUSED */
static void
db_openfifo_cb (w, client, data)
Widget w;
XtPointer client;
XtPointer data;
{
	db_connect_fifo();
}

/* ARGSUSED */
static void
db_help_cb (w, client, data)
Widget w;
XtPointer client;
XtPointer data;
{
	static char *msg[] = {
"This displays a count of the various types of objects currently in memory.",
"Database files may be read in to add to this list or the list may be deleted."
};

	hlp_dialog ("DataBase", msg, XtNumber(msg));
}

/* callback from a specific Help button.
 * client is a string to use with hlp_dialog().
 */
/* ARGSUSED */
static void
db_helpon_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	hlp_dialog ((char *)client, NULL, 0);
}

/* ARGSUSED */
static void
db_close_cb (w, client, data)
Widget w;
XtPointer client;
XtPointer data;
{
	XtUnmanageChild (dbform_w);
}
