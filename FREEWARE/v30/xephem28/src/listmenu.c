/* code to manage the stuff on the "listing" menu.
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
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Separator.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>

#include "P_.h"

extern Widget toplevel_w;

extern FILE *fopenh P_((char *name, char *how));
extern char *syserrstr P_((void));
extern int existsh P_((char *filename));
extern int listing_ison P_((void));
extern void all_selection_mode P_((int whether));
extern void f_string P_((Widget w, char *s));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void query P_((Widget tw, char *msg, char *label1, char *label2,
    char *label3, void (*func1)(), void (*func2)(), void (*func3)()));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void xe_msg P_((char *msg, int app_modal));

static void lst_select P_((int whether));
static void lst_create_form P_((void));
static void lst_activate_cb P_((Widget w, XtPointer client, XtPointer call));
static void lst_close_cb P_((Widget w, XtPointer client, XtPointer call));
static void lst_help_cb P_((Widget w, XtPointer client, XtPointer call));
static void lst_reset P_((void));
static void lst_stop_selecting P_((void));
static void lst_turn_off P_((void));
static void lst_try_append P_((void));
static void lst_try_overwrite P_((void));
static void lst_try_cancel P_((void));
static void lst_try_turn_on P_((void));
static void lst_turn_on P_((char *how));

/* max number of fields we can keep track of at once to list */
#define MAXLSTFLDS	10
#define MAXLSTSTR	32	/* longest string we can list */
#define MAXFLDNAM	32	/* longest allowed field name */

static Widget lstform_w;
static Widget select_w, active_w, prompt_w;
static Widget title_w, filename_w;
static Widget table_w[MAXLSTFLDS];	/* row indeces follow.. */

#define	DEF_LSTFN	"ephem.lst"	/* default plot file name */
static FILE *lst_fp;            /* the listing file; == 0 means don't plot */


/* lst_activate_cb client values. */
#define	SELECT	0
#define	ACTIVE	1

/* store the name and string value of each field to track.
 * we get the label straight from the Text widget in the table as needed.
 */
typedef struct {
    char l_name[MAXFLDNAM];	/* name of field we are listing */
    char l_str[MAXLSTSTR];	/* last know string value of field */
} LstFld;
static LstFld lstflds[MAXLSTFLDS];
static int nlstflds;		/* number of lstflds[] in actual use */

/* called when the list menu is activated via the main menu pulldown.
 * if never called before, create and manage all the widgets as a child of a
 * form. otherwise, just toggle whether the form is managed.
 */
void
lst_manage ()
{
	if (!lstform_w)
	    lst_create_form();
	
	if (XtIsManaged(lstform_w))
	    XtUnmanageChild (lstform_w);
	else
	    XtManageChild (lstform_w);
}

/* called by the other menus (data, etc) as their buttons are
 * selected to inform us that that button is to be included in a listing.
 */
void
lst_selection (name)
char *name;
{
	Widget tw;


	if (!lstform_w
		|| !XtIsManaged(lstform_w)
		|| !XmToggleButtonGetState(select_w))
		    return;

	tw = table_w[nlstflds];
	set_xmstring (tw, XmNlabelString, name);
	XtManageChild (tw);

	(void) strncpy (lstflds[nlstflds].l_name, name, MAXFLDNAM);
	if (++nlstflds == MAXLSTFLDS)
	    lst_stop_selecting();
}

/* called as each different field is written -- just save in lstflds[]
 * if we are potentially interested.
 */
void
lst_log (name, str)
char *name;
char *str;
{
	if (listing_ison()) {
	    LstFld *lp;
	    for (lp = lstflds; lp < &lstflds[nlstflds]; lp++)
		if (strcmp (name, lp->l_name) == 0) {
		    (void) strncpy (lp->l_str, str, MAXLSTSTR-1);
		    break;
		}
	}
}

/* called when all fields have been updated and it's time to
 * write the active listing to the current listing file, if one is open.
 */
void
listing()
{
	if (lst_fp) {
	    /* list in order of original selection */
	    LstFld *lp;
	    for (lp = lstflds; lp < &lstflds[nlstflds]; lp++)
		(void) fprintf (lst_fp, "%s  ", lp->l_str);
	    (void) fprintf (lst_fp, "\n");
	}
}

int
listing_ison()
{
	return (lst_fp != 0);
}

/* called to put up or remove the watch cursor.  */
void
lst_cursor (c)
Cursor c;
{
	Window win;

	if (lstform_w && (win = XtWindow(lstform_w))) {
	    Display *dsp = XtDisplay(lstform_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}

/* inform the other menues whether we are setting up for them to tell us
 * what fields to list.
 */
static void
lst_select(whether)
int whether;
{
	all_selection_mode(whether);
}

static void
lst_create_form()
{
	static struct {
	    char *title;
	    int cb_data;
	    Widget *wp;
	} tbs[] = {
	    {"Select fields", SELECT, &select_w},
	    {"List to file", ACTIVE, &active_w}
	};
	XmString str;
	Widget w, rc_w, f_w;
	Arg args[20];
	char *deffn;
	int i, n;

	/* create form dialog */
	n = 0;
	XtSetArg (args[n], XmNautoUnmanage, False); n++;
	XtSetArg (args[n], XmNdefaultPosition, False); n++;
	lstform_w = XmCreateFormDialog (toplevel_w, "List", args, n);
	XtAddCallback (lstform_w, XmNhelpCallback, lst_help_cb, 0);

	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Listing Control"); n++;
	XtSetValues (XtParent(lstform_w), args, n);

	/* make a RowColumn to hold everything */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNisAligned, False); n++;
	XtSetArg (args[n], XmNadjustMargin, False); n++;
	rc_w = XmCreateRowColumn (lstform_w, "ListRC", args, n);
	XtManageChild (rc_w);

	/* make the control toggle buttons */

	for (i = 0; i < XtNumber(tbs); i++) {
	    str = XmStringCreate(tbs[i].title, XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg (args[n], XmNlabelString, str); n++;
	    w = XmCreateToggleButton(rc_w, "ListTB", args, n);
	    XmStringFree (str);
	    XtManageChild (w);
	    XtAddCallback(w, XmNvalueChangedCallback, lst_activate_cb,
						    (XtPointer)tbs[i].cb_data);
	    if (tbs[i].wp)
		*tbs[i].wp = w;
	}

	/* create filename text area and its label */

	n = 0;
	str = XmStringCreate("File name:", XmSTRING_DEFAULT_CHARSET);
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	XtSetArg (args[n], XmNlabelString, str); n++;
	w = XmCreateLabel (rc_w, "ListFnL", args, n);
	XmStringFree (str);
	XtManageChild (w);

	n = 0;
	filename_w = XmCreateText (rc_w, "Filename", args, n);
	deffn = XmTextGetString (filename_w);
	if (strlen(deffn) == 0)
	    XmTextSetString (filename_w, DEF_LSTFN);
	XtFree (deffn);
	XtManageChild (filename_w);

	/* create title text area and its label */

	n = 0;
	str = XmStringCreate("Title:", XmSTRING_DEFAULT_CHARSET);
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	XtSetArg (args[n], XmNlabelString, str); n++;
	w = XmCreateLabel (rc_w, "ListTL", args, n);
	XtManageChild (w);
	XmStringFree (str);

	n = 0;
	title_w = XmCreateText (rc_w, "ListTitle", args, n);
	XtManageChild (title_w);

	/* create prompt line -- it will be managed as necessary */

	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	prompt_w = XmCreateLabel (rc_w, "ListPrompt", args, n);

	/* make the field name table, but don't manage them now */
	for (i = 0; i < MAXLSTFLDS; i++) {
	    n = 0;
	    table_w[i] = XmCreateLabel(rc_w, "ListLabel", args, n);
	}

	/* create a separator */

	n = 0;
	w = XmCreateSeparator (rc_w, "Sep", args, n);
	XtManageChild (w);

	/* make a form to hold the close and help buttons evenly */

	n = 0;
	XtSetArg (args[n], XmNfractionBase, 7); n++;
	f_w = XmCreateForm (rc_w, "ListCF", args, n);
	XtManageChild(f_w);

	    n = 0;
	    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNleftPosition, 1); n++;
	    XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNrightPosition, 3); n++;
	    w = XmCreatePushButton (f_w, "Close", args, n);
	    XtAddCallback (w, XmNactivateCallback, lst_close_cb, 0);
	    XtManageChild (w);

	    n = 0;
	    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNleftPosition, 4); n++;
	    XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNrightPosition, 6); n++;
	    w = XmCreatePushButton (f_w, "Help", args, n);
	    XtAddCallback (w, XmNactivateCallback, lst_help_cb, 0);
	    XtManageChild (w);
}

/* callback from any of the listing menu toggle buttons being activated.
 */
/* ARGSUSED */
static void
lst_activate_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmToggleButtonCallbackStruct *t = (XmToggleButtonCallbackStruct *) call;
	int what = (int) client;

	switch (what) {
	case SELECT:
	    if (t->set) {
		/* first turn off listing, if on, while we change things */
		if (XmToggleButtonGetState(active_w))
		    XmToggleButtonSetState(active_w, False, True);
		lst_reset();	/* reset lstflds array and unmanage the table*/
		lst_select(1);	/* inform other menus to inform us of fields */
		XtManageChild (prompt_w);
		f_string (prompt_w, "Select quantity for next column...");
	    } else
		lst_stop_selecting();
	    break;
	case ACTIVE:
	    if (t->set) {
		/* first turn off selecting, if on */
		if (XmToggleButtonGetState(select_w))
		    XmToggleButtonSetState(select_w, False, True);
		lst_try_turn_on();
	    } else
		lst_turn_off();
	    break;
	}
}

/* callback from the Close button.
 */
/* ARGSUSED */
static void
lst_close_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (lstform_w);
}

/* callback from the Help
 */
/* ARGSUSED */
static void
lst_help_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static char *msg[] = {
"Select fields to become each column of a listing, then run xephem. Each step",
"will yield one line in the output file. The filename may be specified in the",
"text area provided."
};

	hlp_dialog ("Listing", msg, sizeof(msg)/sizeof(msg[0]));
}

/* forget our list, and unmanage the table.
 */
static void
lst_reset()
{
	int i;

	for (i = 0; i < nlstflds; i++)
	    XtUnmanageChild (table_w[i]);

	nlstflds = 0;
}

/* stop selecting: tell everybody else to drop their buttons, make sure toggle
 * is off.
 */
static void
lst_stop_selecting()
{
	XmToggleButtonSetState (select_w, False, False);
	lst_select(0);
	XtUnmanageChild (prompt_w);
}

static void
lst_turn_off ()
{
	if (lst_fp) {
	    (void) fclose (lst_fp);
	    lst_fp = 0;
	}
}

/* called from the query routine when want to append to an existing list file.*/
static void
lst_try_append()
{
	lst_turn_on("a");
}

/* called from the query routine when want to overwrite to an existing list
 * file.
 */
static void
lst_try_overwrite()
{
	lst_turn_on("w");
}

/* called from the query routine when decided not to make a listing file.  */
static void
lst_try_cancel()
{
	XmToggleButtonSetState (active_w, False, False);
}

/* attempt to open file for use as a listing file.
 * if it doesn't exist, then go ahead and make it.
 * but if it does, first ask wheher to append or overwrite.
 */
static void
lst_try_turn_on()
{
	char *txt = XmTextGetString (filename_w);

	if (existsh (txt) == 0) {
	    char *buf;
	    buf = XtMalloc (strlen(txt)+100);
	    (void) sprintf (buf, "%s exists: Append or Overwrite?", txt);
	    query (toplevel_w, buf, "Append", "Overwrite", "Cancel",
			    lst_try_append, lst_try_overwrite, lst_try_cancel);
	    XtFree (buf);
	} else
	    lst_turn_on("w");
	XtFree (txt);
}

/* turn on listing facility.
 * establish a file to use (and thereby set lst_fp, the "listing-is-on" flag).
 */
static void
lst_turn_on (how)
char *how;	/* fopen how argument */
{
	char *txt;

	/* listing is on if file opens ok */
	txt = XmTextGetString (filename_w);
	lst_fp = fopenh (txt, how);
	if (!lst_fp) {
	    char *buf;
	    XmToggleButtonSetState (active_w, False, False);
	    buf = XtMalloc (strlen(txt)+100);
	    (void) sprintf (buf, "Can not open %s: %s", txt, syserrstr());
	    xe_msg (buf, 1);
	    XtFree (buf);
	}
	XtFree (txt);
	
	if (lst_fp) {
	    /* add a title if desired */
	    txt = XmTextGetString (title_w);
	    if (txt[0] != '\0')
		(void) fprintf (lst_fp, "* %s\n", txt);
	    XtFree (txt);
	}
}
