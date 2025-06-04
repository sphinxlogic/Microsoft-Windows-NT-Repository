/* handle the file list feature of skyview */
/* also the eyepiece size dialog. */

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
#include <Xm/Scale.h>
#include <Xm/SelectioB.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "preferences.h"
#include "skylist.h"

extern Widget toplevel_w;
extern Colormap xe_cm;

extern FILE *fopenh P_((char *name, char *how));
extern Obj *db_scan P_((DBScan *sp));
extern Now *mm_get_now P_((void));
extern char *getXRes P_((char *name, char *def));
extern char *syserrstr P_((void));
extern char *obj_description P_((Obj *op));
extern double atod P_((char *buf));
extern int confirm P_((void));
extern int existsh P_((char *filename));
extern void db_scaninit P_((DBScan *sp, int mask, ObjF *op, int nop));
extern void fs_date P_((char out[], double jd));
extern void fs_prdec P_((char out[], double jd));
extern void fs_pangle P_((char buf[], double a));
extern void fs_ra P_((char buf[], double ra));
extern void fs_sexa P_((char *out, double a, int w, int fracbase));
extern void fs_time P_((char out[], double t));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void get_xmstring P_((Widget w, char *resource, char **txtp));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void prompt_map_cb P_((Widget w, XtPointer client, XtPointer call));
extern void query P_((Widget tw, char *msg, char *label1, char *label2,
    char *label3, void (*func1)(), void (*func2)(), void (*func3)()));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *txtp));
extern void xe_msg P_((char *msg, int app_modal));
extern void watch_cursor P_((int want));
extern void wtip P_((Widget w, char *tip));

static void sv_create_list_w P_((void));
static void sv_txt_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_edb_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_listclose_cb P_((Widget w, XtPointer client, XtPointer call));
static void sv_listhelp_cb P_((Widget w, XtPointer client, XtPointer call));
static void txt_append_cb P_((void));
static void txt_overwrite_cb P_((void));
static void make_txt P_((char *name, char *how));
static void write_txt P_((FILE *fp));
static void edb_append_cb P_((void));
static void edb_overwrite_cb P_((void));
static void make_edb P_((char *name, char *how));
static void write_edb P_((FILE *fp));

static Widget list_w;		/* object list dialog */
static Widget txtfn_w;		/* text-format file name */
static Widget edbfn_w;		/* edb-format file name */


/* callback from List control button. */
/* ARGSUSED */
void
sv_list_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (!list_w)
	    sv_create_list_w();

	if (XtIsManaged(list_w))
	    XtUnmanageChild (list_w);
	else
	    XtManageChild (list_w);
}

/* create the list filename prompt */
static void
sv_create_list_w()
{
	Widget lbl_w, sep_w;
	Widget txt_w, edb_w;
	Widget close_w, help_w;
	Arg args[20];
	int n;

	/* create form */

	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNdefaultPosition, False); n++;
	XtSetArg (args[n], XmNcolormap, xe_cm); n++;
	XtSetArg (args[n], XmNhorizontalSpacing, 10); n++;
	XtSetArg (args[n], XmNverticalSpacing, 10); n++;
	XtSetArg (args[n], XmNmarginHeight, 10); n++;
	XtSetArg (args[n], XmNmarginWidth, 10); n++;
	list_w = XmCreateFormDialog (toplevel_w, "SkyList", args, n);
	XtAddCallback (list_w, XmNhelpCallback, sv_listhelp_cb, 0);
	set_something (list_w, XmNcolormap, (XtArgVal)xe_cm);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "Sky list"); n++;
	XtSetValues (XtParent(list_w), args, n);

	/* instruction */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	lbl_w = XmCreateLabel (list_w, "TTB", args, n);
	set_xmstring (lbl_w, XmNlabelString, "Save Sky View objects to disk");
	XtManageChild (lbl_w);


	/* .txt file option */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, lbl_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	txt_w = XmCreatePushButton (list_w, "TPB", args, n);
	set_xmstring (txt_w, XmNlabelString, "Save as text:");
	XtAddCallback (txt_w, XmNactivateCallback, sv_txt_cb, NULL);
	XtManageChild (txt_w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, lbl_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNleftWidget, txt_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNcolumns, 30); n++;
	txtfn_w = XmCreateTextField (list_w, "TextFilename", args, n);
	XtManageChild (txtfn_w);

	/* .edb file option */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, txtfn_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	edb_w = XmCreatePushButton (list_w, "EPB", args, n);
	XtAddCallback (edb_w, XmNactivateCallback, sv_edb_cb, NULL);
	set_xmstring (edb_w, XmNlabelString, "Save as .edb:");
	XtManageChild (edb_w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, txtfn_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNleftWidget, edb_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNcolumns, 30); n++;
	edbfn_w = XmCreateTextField (list_w, "EdbFilename", args, n);
	XtManageChild (edbfn_w);

	/* bottom controls */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, edbfn_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	sep_w = XmCreateSeparator (list_w, "CSEP", args, n);
	XtManageChild (sep_w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 20); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 40); n++;
	close_w = XmCreatePushButton (list_w, "Close", args, n);
	XtAddCallback (close_w, XmNactivateCallback, sv_listclose_cb, NULL);
	XtManageChild (close_w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, sep_w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 60); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 80); n++;
	help_w = XmCreatePushButton (list_w, "Help", args, n);
	XtAddCallback (help_w, XmNactivateCallback, sv_listhelp_cb, NULL);
	XtManageChild (help_w);
}

/* called when the Close button is hit in the file list prompt */
/* ARGSUSED */
static void
sv_listclose_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (list_w);
}

/* called when the help button is hit in the file list prompt */
/* ARGSUSED */
static void
sv_listhelp_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static char *msg[] = {
	    "Save Sky View objects to files in two formats."
	};

	hlp_dialog ("Sky View - list", msg, sizeof(msg)/sizeof(msg[0]));

}

/* called when the Text-format button is hit */
/* ARGSUSED */
static void
sv_txt_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	char *fn;

	/* gather filename */
	fn = XmTextFieldGetString (txtfn_w);
	if (strlen(fn) == 0) {
	    xe_msg ("Please enter a text list file name.", 1);
	    XtFree (fn);
	    return;
	}

	/* go */
	if (existsh (fn) == 0 && confirm()) {
	    char buf[1024];
	    (void) sprintf (buf, "%s exists..\nAppend or Overwrite?", fn);
	    query (toplevel_w, buf, "Append", "Overwrite", "Cancel",
				txt_append_cb, txt_overwrite_cb, NULL);
	} else
	    txt_overwrite_cb();

	XtFree (fn);
}

/* called when the edb-format button is hit */
/* ARGSUSED */
static void
sv_edb_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	char *fn;

	/* gather filename */
	fn = XmTextFieldGetString (edbfn_w);
	if (strlen(fn) == 0) {
	    xe_msg ("Please enter a edb list file name.", 1);
	    XtFree (fn);
	    return;
	}

	/* go */
	if (existsh (fn) == 0 && confirm()) {
	    char buf[1024];
	    (void) sprintf (buf, "%s exists..\nAppend or Overwrite?", fn);
	    query (toplevel_w, buf, "Append", "Overwrite", "Cancel",
				edb_append_cb, edb_overwrite_cb, NULL);
	} else
	    edb_overwrite_cb();

	XtFree (fn);
}

/* called when we want to append to a text-format file */
static void
txt_append_cb ()
{
	char *fn = XmTextFieldGetString (txtfn_w);
	make_txt (fn, "a");
	XtFree (fn);
}

/* called when we want to ceate a new text-format file */
static void
txt_overwrite_cb ()
{
	char *fn = XmTextFieldGetString (txtfn_w);
	make_txt (fn, "w");
	XtFree (fn);
}

/* open the named text-format file "a" or "w" and fill it in. */
static void
make_txt (name, how)
char *name;
char *how;
{
	FILE *fp = fopenh (name, how);
	char buf[1024];

	if (fp) {
	    write_txt (fp);
	    (void) fclose (fp);
	    sprintf(buf, "%s %s ok.", name, how[0]=='a'?"appended":"written");
	    xe_msg (buf, 1);
	} else {
	    (void) sprintf (buf, "Can not open %s: %s", name, syserrstr());
	    xe_msg (buf, 1);
	}
}

/* write out all objects currently on sky view display in text-format */
static void
write_txt (fp)
FILE *fp;
{
	Now *np = mm_get_now();
	DBScan dbs;
	ObjF *fs;
	int nfs;
	double tmp;
	char buf[64];
	int aamode;
	double fov, alt, az, ra, dec;
	Obj *op;

	watch_cursor(1);

	/* line to summarize the current circumstances */
	sv_getcenter (&aamode, &fov, &alt, &az, &ra, &dec);
	(void) fprintf (fp, "Sky List at ");
	fs_time (buf, mjd_hr(mjd));
	(void) fprintf (fp, " %s", buf);
	fs_date (buf, mjd_day(mjd));
	(void) fprintf (fp, " %s UTC:", buf);
	(void) fprintf (fp, " FOV %g;", raddeg(fov));

	if (aamode) {
	    /* alt/az mode */
	    fs_pangle (buf, alt);
	    (void) fprintf (fp, " Alt %s;", buf);
	    fs_pangle (buf, az);
	    (void) fprintf (fp, " Az %s;", buf);
	} else {
	    /* ra/dec mode */
	    fs_ra (buf, ra);
	    (void) fprintf (fp, " RA %s;", buf);
	    fs_prdec (buf, dec);
	    (void) fprintf (fp, " Dec %s;", buf);
	}

	if (epoch == EOD)
	    tmp = mjd;
	else
	    tmp = epoch;
	mjd_year (tmp, &tmp);
	(void) fprintf (fp, " Ep %7.2f;", tmp);

	(void) fprintf (fp, "\n");

	/* column headings */

	(void) fprintf (fp, "____Name_____");
	(void) fprintf (fp, pref_get(PREF_DPYPREC) == PREF_LOPREC ? " __RA___"
							   : " ____RA_____");
	(void) fprintf (fp, pref_get(PREF_DPYPREC) == PREF_LOPREC ? " _Dec__"
							   : " ____Dec____");
	(void) fprintf (fp, pref_get(PREF_DPYPREC) == PREF_LOPREC ? " _Alt__"
							   : " ___Alt___");
	(void) fprintf (fp, pref_get(PREF_DPYPREC) == PREF_LOPREC ? " __Az__"
							   : " ___Az____");
	(void) fprintf (fp, " _Mag__");
	(void) fprintf (fp, " Sp");
	(void) fprintf (fp, " Size");
	(void) fprintf (fp, " Class");

	(void) fprintf (fp, "\n");

	/* scan db for all objects on Sky screen, including any field stars */
	sv_getfldstars (&fs, &nfs);
	for (db_scaninit(&dbs, ALLM, fs, nfs); (op = db_scan(&dbs)) != NULL; ) {

	    if (!(op->o_flags & OBJF_ONSCREEN))
		continue;

	    (void) fprintf (fp, "%-*.*s", MAXNM-1, MAXNM-1, op->o_name);

	    fs_ra (buf, op->s_ra);
	    (void) fprintf (fp, " %s", buf);

	    fs_prdec (buf, op->s_dec);
	    (void) fprintf (fp, " %s", buf);

	    fs_pangle (buf, op->s_alt);
	    (void) fprintf (fp, " %s", buf);

	    fs_pangle (buf, op->s_az);
	    (void) fprintf (fp, " %s", buf);

	    (void) fprintf (fp, " %6.2f", get_mag(op));

	    if (is_type (op, FIXEDM) && op->f_class != 'G')
		(void) fprintf (fp, " %c%c",
					op->f_spect[0] ? op->f_spect[0] : ' ',
					op->f_spect[1] ? op->f_spect[1] : ' ');
	    else
		(void) fprintf (fp, "   ");

	    if (op->s_size < 999.5)
		(void) fprintf (fp, "%5.1f", op->s_size);
	    else
		(void) fprintf (fp, "%5.0f", op->s_size);

	    (void) fprintf (fp, " %s", obj_description (op));

	    (void) fprintf (fp, "\n");
	}

	watch_cursor(0);
}

/* called when we want to append to a edb-format file */
static void
edb_append_cb ()
{
	char *fn = XmTextFieldGetString (edbfn_w);
	make_edb (fn, "a");
	XtFree (fn);
}

/* called when we want to ceate a new edb-format file */
static void
edb_overwrite_cb ()
{
	char *fn = XmTextFieldGetString (edbfn_w);
	make_edb (fn, "w");
	XtFree (fn);
}

/* open the named edb-format file "a" or "w" and fill it in. */
static void
make_edb (name, how)
char *name;
char *how;
{
	FILE *fp = fopenh (name, how);
	char buf[1024];

	if (fp) {
	    write_edb (fp);
	    (void) fclose (fp);
	    sprintf(buf, "%s %s ok.", name, how[0]=='a'?"appended":"written");
	    xe_msg (buf, 1);
	} else {
	    (void) sprintf (buf, "Can not open %s: %s", name, syserrstr());
	    xe_msg (buf, 1);
	}
}

/* write out all objects currently on sky view display in edb-format */
static void
write_edb (fp)
FILE *fp;
{
	char buf[1024];
	DBScan dbs;
	ObjF *fs;
	int nfs;
	Obj *op;

	watch_cursor(1);

	/* scan db for all objects on Sky screen, including any field stars */
	sv_getfldstars (&fs, &nfs);
	for (db_scaninit(&dbs, ALLM, fs, nfs); (op = db_scan(&dbs)) != NULL; ) {

	    if (!(op->o_flags & OBJF_ONSCREEN))
		continue;

	    db_write_line (op, buf);
	    fprintf (fp, "%s\n", buf);
	}

	watch_cursor(0);
}

/* stuff for the eyepiece size dialog */

/* a list of EyePieces */
static EyePiece *eyep;		/* malloced list of eyepieces */
static int neyep;		/* number of entries off eyep */

static Widget eyepd_w;		/* overall eyepiece dialog */
static Widget eyepws_w;		/* eyepiece width scale */
static Widget eyephs_w;		/* eyepiece height scale */
static Widget eyepwl_w;		/* eyepiece width label */
static Widget eyephl_w;		/* eyepiece height label */
static Widget eyer_w;		/* eyepiece Round/Square TB */
static Widget eyef_w;		/* eyepiece solid/border TB */
static Widget delep_w;		/* the delete all PB */
static Widget lock_w;		/* lock scales TB */

static void se_create_eyepd_w P_((void));
static void se_eyepsz P_((double *wp, double *hp, int *rp, int *fp));
static int se_scale_fmt P_((Widget s_w, Widget l_w));
static void se_wscale_cb P_((Widget w, XtPointer client, XtPointer call));
static void se_hscale_cb P_((Widget w, XtPointer client, XtPointer call));
static void se_delall_cb P_((Widget w, XtPointer client, XtPointer call));
static void se_close_cb P_((Widget w, XtPointer client, XtPointer call));
static void se_rmall P_((void));

int
se_ismanaged()
{
	return (eyepd_w && XtIsManaged(eyepd_w));
}

void 
se_unmanage()
{
	if (eyepd_w)
	    XtUnmanageChild (eyepd_w);
}

void 
se_manage()
{
	if (!eyepd_w)
	    se_create_eyepd_w();

	XtManageChild (eyepd_w);
}

/* add one eyepiece for the given location */
void
se_add (ra, dec, alt, az)
double ra, dec, alt, az;
{
	EyePiece *new;

	/* increase allocation at eyep by 1 */
	if (eyep)
	    new = (EyePiece *)realloc ((void *)eyep,(neyep+1)*sizeof(EyePiece));
	else
	    new = (EyePiece *) malloc (sizeof(EyePiece));
	if (!new) {
	    xe_msg ("No memory for another eyepiece", 1);
	    return;
	}
	eyep = new;

	/* reuse new to point to the one we're adding */
	new += neyep;
	neyep++;

	/* fill in the details */
	new->ra = ra;
	new->dec = dec;
	new->alt = alt;
	new->az = az;
	se_eyepsz (&new->eyepw, &new->eyeph, &new->round, &new->solid);

	/* at least one to delete now */
	XtSetSensitive (delep_w, True);
}

/* return the list of current eyepieces, if interested, and the count.
 */
int
se_getlist (ep)
EyePiece **ep;
{
	if (ep)
	    *ep = eyep;
	return (neyep);
}

/* fetch the current eyepiece diameter, in rads, whether it is round, and
 * whether it is filled from the dialog.
 */
static void
se_eyepsz(wp, hp, rp, fp)
double *wp, *hp;
int *rp;
int *fp;
{
	int eyepw, eyeph;

	if (!eyepd_w)
	    se_create_eyepd_w();

	XmScaleGetValue (eyepws_w, &eyepw);
	*wp = degrad(eyepw/60.0);

	XmScaleGetValue (eyephs_w, &eyeph);
	*hp = degrad(eyeph/60.0);

	*rp = XmToggleButtonGetState (eyer_w);
	*fp = XmToggleButtonGetState (eyef_w);
}

/* create the eyepiece size dialog */
static void
se_create_eyepd_w()
{
	Widget w;
	Widget l_w, rb_w;
	Arg args[20];
	int n;

	/* create form */

	n = 0;
	XtSetArg(args[n], XmNdefaultPosition, False); n++;
	XtSetArg(args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNcolormap, xe_cm); n++;
	XtSetArg (args[n], XmNverticalSpacing, 5); n++;
	XtSetArg (args[n], XmNmarginHeight, 10); n++;
	XtSetArg (args[n], XmNmarginWidth, 10); n++;
	eyepd_w = XmCreateFormDialog (toplevel_w, "SkyEyep", args, n);
	set_something (eyepd_w, XmNcolormap, (XtArgVal)xe_cm);
	XtAddCallback (eyepd_w, XmNmapCallback, prompt_map_cb, NULL);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Eyepiece Setup"); n++;
	XtSetValues (XtParent(eyepd_w), args, n);

	/* title label */
	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	w = XmCreateLabel (eyepd_w, "L", args, n);
	set_xmstring (w, XmNlabelString, "Eyepiece width and height, degrees:");
	XtManageChild (w);

	/* w scale and its label */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	eyepwl_w = XmCreateLabel (eyepd_w, "EyepWL", args, n);
	XtManageChild (eyepwl_w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, eyepwl_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNshowValue, False); n++;
	XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	eyepws_w = XmCreateScale (eyepd_w, "EyepW", args, n);
	XtAddCallback (eyepws_w, XmNdragCallback, se_wscale_cb, 0);
	XtAddCallback (eyepws_w, XmNvalueChangedCallback, se_wscale_cb, 0);
	wtip (eyepws_w, "Set to desired width of eyepiece");
	XtManageChild (eyepws_w);

	/* init the slave string */
	(void) se_scale_fmt (eyepws_w, eyepwl_w);

	/* h scale and its label */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, eyepws_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	eyephl_w = XmCreateLabel (eyepd_w, "EyepHL", args, n);
	XtManageChild (eyephl_w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, eyephl_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNshowValue, False); n++;
	XtSetArg (args[n], XmNscaleMultiple, 1); n++;
	eyephs_w = XmCreateScale (eyepd_w, "EyepH", args, n);
	XtAddCallback (eyephs_w, XmNdragCallback, se_hscale_cb, 0);
	XtAddCallback (eyephs_w, XmNvalueChangedCallback, se_hscale_cb, 0);
	wtip (eyephs_w, "Set to desired height of eyepiece");
	XtManageChild (eyephs_w);

	/* init the slave string */
	(void) se_scale_fmt (eyephs_w, eyephl_w);

	/* lock TB */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, eyephs_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftOffset, 10); n++;
	lock_w = XmCreateToggleButton (eyepd_w, "Lock", args, n);
	set_xmstring (lock_w, XmNlabelString, "Lock scales together");
	wtip (lock_w, "When on, width and height scales move as one");
	XtManageChild (lock_w);

	/* shape label */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, lock_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	l_w = XmCreateLabel (eyepd_w, "S", args, n);
	set_xmstring (l_w, XmNlabelString, "Shape:");
	XtManageChild (l_w);

	/* round or square Radio box */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, l_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftOffset, 10); n++;
	rb_w = XmCreateRadioBox (eyepd_w, "RSRB", args, n);
	XtManageChild (rb_w);

	    n = 0;
	    eyer_w = XmCreateToggleButton (rb_w, "Elliptical", args, n);
	    wtip (eyer_w, "When on, next eyepiece will be elliptical");
	    XtManageChild (eyer_w);

	    n = 0;
	    w = XmCreateToggleButton (rb_w, "Rectangular", args, n);
	    wtip (w, "When on, next eyepiece will be rectangular");
	    XtManageChild (w);

	/* style label */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, lock_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 50); n++;
	l_w = XmCreateLabel (eyepd_w, "St", args, n);
	set_xmstring (l_w, XmNlabelString, "Style:");
	XtManageChild (l_w);

	/* style Radio box */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, l_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 50); n++;
	rb_w = XmCreateRadioBox (eyepd_w, "FBRB", args, n);
	XtManageChild (rb_w);

	    n = 0;
	    eyef_w = XmCreateToggleButton (rb_w, "Solid", args, n);
	    wtip (eyef_w, "When on, next eyepiece will be solid");
	    XtManageChild (eyef_w);

	    n = 0;
	    w = XmCreateToggleButton (rb_w, "Outline", args, n);
	    wtip (w, "When on, next eyepiece will be just a border");
	    XtManageChild (w);

	/* delete PB */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, rb_w); n++;
	XtSetArg (args[n], XmNtopOffset, 10); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 20); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 80); n++;
	delep_w = XmCreatePushButton (eyepd_w, "DelE", args, n);
	XtAddCallback (delep_w, XmNactivateCallback, se_delall_cb, NULL);
	wtip (delep_w, "Delete all eyepieces");
	set_xmstring (delep_w, XmNlabelString, "Delete all Eyepieces");
	XtSetSensitive (delep_w, False);	/* works when there are some */
	XtManageChild (delep_w);

	/* separator */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, delep_w); n++;
	XtSetArg (args[n], XmNtopOffset, 10); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	w = XmCreateSeparator (eyepd_w, "Sep", args, n);
	XtManageChild (w);

	/* a close button */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, w); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 30); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 70); n++;
	w = XmCreatePushButton (eyepd_w, "Close", args, n);
	XtAddCallback (w, XmNactivateCallback, se_close_cb, NULL);
	wtip (w, "Close this dialog");
	XtManageChild (w);
}

/* read the given scale and write it's value in the givel label.
 * return the scale value.
 */
static int
se_scale_fmt (s_w, l_w)
Widget s_w, l_w;
{
	char buf[64];
	int v;

	/* format from value, in arc minutes */
	XmScaleGetValue (s_w, &v);
	fs_sexa (buf, v/60.0, 2, 60);
	set_xmstring (l_w, XmNlabelString, buf);

	return (v);
}

/* drag callback from the height scale */
static void
se_hscale_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int v = se_scale_fmt (eyephs_w, eyephl_w);

	/* slave the w scale to the h scale */
	if (XmToggleButtonGetState(lock_w)) {
	    XmScaleSetValue (eyepws_w, v);
	    se_scale_fmt (eyepws_w, eyepwl_w);
	}
}

/* drag callback from the width scale */
static void
se_wscale_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int v = se_scale_fmt (eyepws_w, eyepwl_w);

	/* slave the w scale to the h scale */
	if (XmToggleButtonGetState(lock_w)) {
	    XmScaleSetValue (eyephs_w, v);
	    se_scale_fmt (eyephs_w, eyephl_w);
	}
}

/* callback from the delete eyepieces control.
 */
/* ARGSUSED */
static void
se_delall_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (eyep) {
	    se_rmall();
	    sv_all(mm_get_now());
	    XtSetSensitive (delep_w, False);
	}
}

/* callback from the close PB.
 */
/* ARGSUSED */
static void
se_close_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (eyepd_w);
}

/* delete the entire list of eyepieces */
static void
se_rmall()
{
	if (eyep) {
	    free ((void *)eyep);
	    eyep = NULL;
	}
	neyep = 0;
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: skylist.c,v $ $Date: 1999/10/07 06:01:35 $ $Revision: 1.4 $ $Name:  $"};
