/* handle the file list feature of skyview */

#include <stdio.h>
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
#include <Xm/Text.h>
#include <Xm/ToggleB.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "preferences.h"
#include "skylist.h"

extern Widget toplevel_w;

extern FILE *fopenh P_((char *name, char *how));
extern Obj *db_scan P_((DBScan *sp));
extern Now *mm_get_now P_((void));
extern char *syserrstr P_((void));
extern char *obj_description P_((Obj *op));
extern int existsh P_((char *filename));
extern void db_scaninit P_((DBScan *sp));
extern void fs_date P_((char out[], double jd));
extern void fs_pangle P_((char buf[], double a));
extern void fs_ra P_((char buf[], double ra));
extern void fs_time P_((char out[], double t));
extern void get_xmstring P_((Widget w, char *resource, char **txtp));
extern void prompt_map_cb P_((Widget w, XtPointer client, XtPointer call));
extern void query P_((Widget tw, char *msg, char *label1, char *label2,
    char *label3, void (*func1)(), void (*func2)(), void (*func3)()));
extern void set_xmstring P_((Widget w, char *resource, char *txtp));
extern void xe_msg P_((char *msg, int app_modal));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void watch_cursor P_((int want));


static Widget list_w;	/* object list dialog */
static void sv_create_list_w P_((void));
static void sv_listok_cb P_((Widget w, XtPointer client, XtPointer call));
static void listok_append_cb P_((Widget w, XtPointer client, XtPointer call));
static void listok_overwrite_cb P_((Widget w, XtPointer client,XtPointer call));
static void make_list P_((char *name, char *how));
static void write_list P_((FILE *fp));


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
	Arg args[20];
	int n;

	n = 0;
	XtSetArg(args[n], XmNdefaultPosition, False);  n++;
	XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL);  n++;
	list_w = XmCreatePromptDialog (toplevel_w, "SkyList", args,n);
	set_xmstring (list_w, XmNdialogTitle, "xephem Sky View list");
	set_xmstring (list_w, XmNselectionLabelString, "File name:");
	XtUnmanageChild (XmSelectionBoxGetChild(list_w, XmDIALOG_HELP_BUTTON));
	XtAddCallback (list_w, XmNokCallback, sv_listok_cb, NULL);
	XtAddCallback (list_w, XmNmapCallback, prompt_map_cb, NULL);
}

/* called when the Ok button is hit in the file list prompt */
/* ARGSUSED */
static void
sv_listok_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	char buf[1024];
	char *name;

	get_xmstring(w, XmNtextString, &name);

	if (strlen(name) == 0) {
	    xe_msg ("Please enter a Sky View list file name.", 1);
	    return;
	}

	if (existsh (name) == 0) {
	    (void) sprintf (buf, "%s exists: Append or Overwrite?", name);
	    query (toplevel_w, buf, "Append", "Overwrite", "Cancel",
				listok_append_cb, listok_overwrite_cb, NULL);
	} else
	    make_list (name, "w");

	XtFree (name);
}

/* called when we want to append to a list file */
/* ARGSUSED */
static void
listok_append_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	char *name;

	get_xmstring (list_w, XmNtextString, &name);
	make_list (name, "a");
	XtFree (name);
}

/* called when we want to ceate a new list file */
/* ARGSUSED */
static void
listok_overwrite_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	char *name;

	get_xmstring (list_w, XmNtextString, &name);
	make_list (name, "w");
	XtFree (name);
}

/* open the named list file "a" or "w" and fill it in. */
static void
make_list (name, how)
char *name;
char *how;
{
	FILE *fp = fopenh (name, how);

	if (fp) {
	    write_list (fp);
	    (void) fclose (fp);
	} else {
	    char buf[1024];

	    (void) sprintf (buf, "Can not open %s: %s", name, syserrstr());
	    xe_msg (buf, 1);
	}
}

/* write out all objects currently on sky view display */
static void
write_list (fp)
FILE *fp;
{
	Now *np = mm_get_now();
	DBScan dbs;
	double tmp;
	char buf[64];
	int aamode;
	double fov, alt, az, ra, dec;
	Obj *op;

	watch_cursor(1);

	/* line to summarize the current circumstances */
	sv_getcenter (&aamode, &fov, &alt, &az, &ra, &dec);
	(void) fprintf (fp, "Sky List ");
	fs_time (buf, mjd_hr(mjd));
	(void) fprintf (fp, " %s", buf);
	fs_date (buf, mjd_day(mjd));
	(void) fprintf (fp, " %s UT:", buf);
	(void) fprintf (fp, " FOV: %g", raddeg(fov));

	if (aamode) {
	    /* alt/az mode */
	    fs_pangle (buf, alt);
	    (void) fprintf (fp, " Alt: %s", buf);
	    fs_pangle (buf, az);
	    (void) fprintf (fp, " Az: %s", buf);
	} else {
	    /* ra/dec mode */
	    fs_ra (buf, ra);
	    (void) fprintf (fp, " RA: %s", buf);
	    fs_pangle (buf, dec);
	    (void) fprintf (fp, " Dec: %s", buf);
	}

	if (epoch == EOD)
	    tmp = mjd;
	else
	    tmp = epoch;
	mjd_year (tmp, &tmp);
	(void) fprintf (fp, " Ep: %7.2f", tmp);

	(void) fprintf (fp, "\n");

	/* column headings */

	(void) fprintf (fp, "Name.........");
	(void) fprintf (fp, pref_get(PREF_DPYPREC) == PREF_LOPREC ? " RA....."
							   : " RA........");
	(void) fprintf (fp, pref_get(PREF_DPYPREC) == PREF_LOPREC ? " Dec..."
							   : " Dec......");
	(void) fprintf (fp, pref_get(PREF_DPYPREC) == PREF_LOPREC ? " Alt..."
							   : " Alt......");
	(void) fprintf (fp, pref_get(PREF_DPYPREC) == PREF_LOPREC ? " Az...."
							   : " Az.......");
	(void) fprintf (fp, " Mag...");
	(void) fprintf (fp, " Size.");
	(void) fprintf (fp, " Class");

	(void) fprintf (fp, "\n");



	for (db_scaninit(&dbs); (op = db_scan(&dbs)) != NULL; ) {

	    if (op->type == UNDEFOBJ)
		continue;
	    if (!(op->o_flags & OBJF_ONSCREEN))
		continue;

	    (void) fprintf (fp, "%-*.*s", MAXNM-1, MAXNM-1, op->o_name);

	    fs_ra (buf, op->s_ra);
	    (void) fprintf (fp, " %s", buf);

	    fs_pangle (buf, op->s_dec);
	    (void) fprintf (fp, " %s", buf);

	    fs_pangle (buf, op->s_alt);
	    (void) fprintf (fp, " %s", buf);

	    fs_pangle (buf, op->s_az);
	    (void) fprintf (fp, " %s", buf);

	    (void) fprintf (fp, " %6.2f", op->s_mag/MAGSCALE);

	    (void) fprintf (fp, " %5d", op->s_size);

	    (void) fprintf (fp, " %s", obj_description (op));

	    (void) fprintf (fp, "\n");
	}

	watch_cursor(0);
}
