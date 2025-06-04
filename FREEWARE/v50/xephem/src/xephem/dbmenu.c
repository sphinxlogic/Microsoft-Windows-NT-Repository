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
#include <Xm/Text.h>
#include <Xm/Separator.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"

extern Widget	toplevel_w;
#define	XtD	XtDisplay(toplevel_w)
extern Colormap xe_cm;

extern FILE *fopenh P_((char *name, char *how));
extern Obj *db_scan P_((DBScan *sp));
extern char *expand_home P_((char *path));
extern char *getShareDir P_((void));
extern char *getXRes P_((char *name, char *def));
extern char *syserrstr P_((void));
extern int confirm P_((void));
extern int db_n P_((void));
extern int db_n_cp P_((void));
extern int db_read P_((FILE *fp, int nodups));
extern void all_newdb P_((int appended));
extern void db_clr_cp P_((void));
extern void db_connect_fifo P_((void));
extern void db_del_all P_((void));
extern void db_del_cp P_((void));
extern void db_scaninit P_((DBScan *sp, int mask, ObjF *op, int nop));
extern void db_set_cp P_((void));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void prompt_map_cb P_((Widget w, XtPointer client, XtPointer call));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void query P_((Widget tw, char *msg, char *label1, char *label2,
    char *label3, void (*func1)(void), void (*func2)(void),
    void (*func3)(void)));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void watch_cursor P_((int want));
extern void wtip P_((Widget w, char *tip));
extern void xe_msg P_((char *msg, int app_modal));

static void db_create_form P_((void));
static void initFSB P_((Widget fsb_w));
static void initPubShared P_((Widget rb_w, Widget fsb_w));
static void db_set_report P_((void));
static FILE *fileopen P_((char *name));
static void db_append_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_del_cp_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_set_cp_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_clr_cp_cb P_((Widget w, XtPointer client, XtPointer data));
static void delete_all_confirmed P_((void));
static void db_del_all_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_openfifo_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_help_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_helpon_cb P_((Widget w, XtPointer client, XtPointer data));
static void db_close_cb P_((Widget w, XtPointer client, XtPointer data));

static Widget dbform_w;		/* main Form dialog */
static Widget report_w;		/* label with the dbstats */
static Widget nodups_w;		/* TB whether to avoid dups on file reads */

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
 *   when initial db files could be loaded automatically and we introduced
 *   fields stars in 2.9.
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

/* called when any of the user-defined objects have changed.
 * all we do is update our tally, if we are up at all.
 */
/* ARGSUSED */
void
db_newobj (id)
int id;	/* OBJXYZ */
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

	if (dbform_w && (win = XtWindow(dbform_w)) != 0) {
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
	Widget sep_w, fsb_w, rc_w;
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
	XtSetArg (args[n], XmNcolormap, xe_cm); n++;
	dbform_w = XmCreateFormDialog(toplevel_w, "DBManage", args, n);
	set_something (dbform_w, XmNcolormap, (XtArgVal)xe_cm);
	XtAddCallback (dbform_w, XmNmapCallback, prompt_map_cb, NULL);
	XtAddCallback (dbform_w, XmNhelpCallback, db_help_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Database Load/Delete"); n++;
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

	    /* make the open fifo button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "OpenFIFO", args, n);
	    XtAddCallback (w, XmNactivateCallback, db_openfifo_cb, 0);
	    set_xmstring(w, XmNlabelString, "Open DB Fifo");
	    wtip (w, "Activate import path for Object definitions");
	    XtManageChild (w);

	    /* make the no-dups toggle button */

	    n = 0;
	    XtSetArg (args[n], XmNvisibleWhenOff, True);  n++;
	    nodups_w = XmCreateToggleButton (pd_w, "NoDups", args, n);
	    set_xmstring(nodups_w, XmNlabelString, "No Dups");
	    wtip (nodups_w, "When on, skip objects whose name already exists in memory when reading");
	    XtManageChild (nodups_w);

	    /* add a separator */

	    n = 0;
	    w = XmCreateSeparator (pd_w, "Sep", args, n);
	    XtManageChild (w);

	    /* add the close button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "Close", args, n);
	    XtAddCallback (w, XmNactivateCallback, db_close_cb, 0);
	    wtip (w, "Close this dialog");
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
	    wtip (w, "Tag the current set of objects in memory");
	    XtManageChild (w);

	    /* make the clear-checkpoint button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "ClrCP", args, n);
	    XtAddCallback (w, XmNactivateCallback, db_clr_cp_cb, 0);
	    set_xmstring(w, XmNlabelString, "Clear Checkpoint");
	    wtip (w, "Discard tags set on objects currently in memory");
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
	    wtip (w, "Delete all objects in memory which have not been tagged");
	    XtManageChild (w);

	    /* make the delete-all button */

	    n = 0;
	    w = XmCreatePushButton (pd_w, "DelAll", args, n);
	    XtAddCallback (w, XmNactivateCallback, db_del_all_cb, 0);
	    wtip (w, "Delete all objects from memory");
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

	/* make the report output-only text */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, mb_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNeditable, False); n++;
	XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg (args[n], XmNcursorPositionVisible, False); n++;
	XtSetArg (args[n], XmNblinkRate, 0); n++;
	XtSetArg (args[n], XmNrows, 17); n++;
	XtSetArg (args[n], XmNcolumns, 32); n++;
	report_w = XmCreateText (dbform_w, "Report", args, n);
	wtip (report_w, "Breakdown of number and types of objects in memory");
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

	/* make the Share/User rapid dir choices */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNspacing, 5); n++;
	rc_w = XmCreateRowColumn (dbform_w, "DRB", args, n);
	XtManageChild (rc_w);

	/* make the file selection box */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
	fsb_w = XmCreateFileSelectionBox (dbform_w, "FSB", args, n);
	initFSB (fsb_w);	/* do this before managing for Ultrix :-) */
	initPubShared (rc_w, fsb_w);
	wtip (fsb_w, "Select a file to read and add objects to memory");
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
	char buf[1024];

#ifndef VMS
	(void) sprintf (buf, "%s/catalogs",  getShareDir());
#else
	(void) sprintf (buf, "%s[catalogs]",  getShareDir());
#endif
	set_xmstring (fsb_w, XmNdirectory, expand_home(buf));
	set_xmstring (fsb_w, XmNpattern, "*.edb");
}

/* callback from the Public dir PB */
/* ARGSUSED */
static void
sharedDirCB (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	Widget fsb_w = (Widget)client;
	char buf[1024];

#ifndef VMS
	(void) sprintf (buf, "%s/catalogs", getShareDir());
#else
	(void) sprintf (buf, "%s[catalogs]", getShareDir());
#endif
	set_xmstring (fsb_w, XmNdirectory, expand_home(buf));
}

/* callback from the Private dir PB */
/* ARGSUSED */
static void
privateDirCB (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	Widget fsb_w = (Widget)client;

	set_xmstring (fsb_w, XmNdirectory,
				    expand_home(getXRes("PrivateDir", "work")));
}

/* build a set of PB in RC rc_w so that they
 * set the XmNdirectory resource in the FSB fsb_w and invoke the Filter.
 */
static void
initPubShared (rc_w, fsb_w)
Widget rc_w, fsb_w;
{
	Arg args[20];
	Widget w;
	int n;

	n = 0;
	w = XmCreateLabel (rc_w, "Dir", args, n);
	set_xmstring (w, XmNlabelString, "Go to:");
	XtManageChild (w);

	n = 0;
	w = XmCreatePushButton (rc_w, "Private", args, n);
	XtAddCallback(w, XmNactivateCallback, privateDirCB, (XtPointer)fsb_w);
	wtip (w,"Set Filter directory to value of XEphem.PrivateDir resource");
	XtManageChild (w);

	n = 0;
	w = XmCreatePushButton (rc_w, "Shared", args, n);
	XtAddCallback(w, XmNactivateCallback, sharedDirCB, (XtPointer)fsb_w);
#ifndef VMS
	wtip (w,"Set Filter directory to XEphem.ShareDir/catalogs");
#else
	wtip (w,"Set Filter directory to XEphem.ShareDir[catalogs]");
#endif
	XtManageChild (w);
}

/* compile the stats report into report_w.
 * N.B. we do not include the planets nor the user objects.
 */
static void
db_set_report()
{
	DBScan dbs;
	char report[1024];
	int mask = ALLM & ~PLANETM;
	Obj *op;
	int nes=0, ne=0, np=0, nh=0;
	int nc=0, ng=0, nj=0, nn=0, npn=0, nl=0, nq=0, nr=0, ns=0, no=0;
	int ndash;
	int t=0;

	for (db_scaninit(&dbs, mask, NULL, 0); (op = db_scan(&dbs))!=NULL; ){
	    switch (op->o_type) {
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
	    case UNDEFOBJ: break;
	    default:
		printf ("Unknown object type: %d\n", op->o_type);
		exit (1);
	    }
	}

	ndash = t > 0 ? (int)(log10((double)t) + 1) : 1;

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
%*s%.*s\n\
%6d Total Objects in Memory\n\
%6d Objects Checkpointed\n\
",
	ne, nh, np, nes, nc, ng, npn, nn, nl, nq, nj, nr, ns, no,
	6-ndash, "", ndash, "------",
	t, db_n_cp()-NOBJ);

	set_something (report_w, XmNvalue, (XtArgVal)report);
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
	    if (db_read (fp, XmToggleButtonGetState (nodups_w)) < 0) {
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
	db_clr_cp();
	db_set_report();
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
	if (confirm())
	    query (toplevel_w, "Delete all objects in memory?",
				"Yes -- delete all", "No -- delete none", NULL,
				delete_all_confirmed, NULL, NULL);
	else 
	    delete_all_confirmed();
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

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: dbmenu.c,v $ $Date: 1999/03/17 23:47:09 $ $Revision: 1.3 $ $Name:  $"};
