/* code to support the preferences facility.
 */

#include <stdio.h>

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>

#include "P_.h"
#include "circum.h"
#include "preferences.h"


extern void redraw_screen P_((int how_much));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void wtip P_((Widget w, char *tip));
extern void xe_msg P_((char *msg, int app_modal));

static void pref_simplepair P_((Widget pd, int prefname, char *pdname,
    char *tip, char *cblabel, int cbmne, XtCallbackProc callback,
    int op1pref, char *op1name, int op1mne, char *op1tip,
    int op2pref, char *op2name, int op2mne, char *op2tip));

static void pref_topogeo_cb P_((Widget w, XtPointer client, XtPointer call));
static void pref_date_cb P_((Widget w, XtPointer client, XtPointer call));
static void pref_units_cb P_((Widget w, XtPointer client, XtPointer call));
static void pref_tz_cb P_((Widget w, XtPointer client, XtPointer call));
static void pref_dpy_prec_cb P_((Widget w, XtPointer client, XtPointer call));
static void pref_msg_bell_cb P_((Widget w, XtPointer client, XtPointer call));
static void pref_prefill_cb P_((Widget w, XtPointer client, XtPointer call));
static void pref_tips_cb P_((Widget w, XtPointer client, XtPointer call));
static void pref_confirm_cb P_((Widget w, XtPointer client, XtPointer call));


/* record of preferences values */
static int prefs[NPREFS];

/* Create "Preferences" PulldownMenu.
 * use the given menu_bar widget as a base.
 * this is called early when the main menu bar is being built..
 * initialize the prefs[] array from the initial state of the toggle buttons.
 */
void
pref_create_pulldown (menu_bar)
Widget menu_bar;
{
	Widget cascade, menu_pane, pull_right;
	Widget tb1, tb2, tb3;
	Arg args[20];
	int n;

	n = 0;
	menu_pane = XmCreatePulldownMenu (menu_bar, "Preferences", args, n);

	    pref_simplepair (menu_pane, PREF_EQUATORIAL, "Equatorial",
		    "Whether RA/Dec values are topocentric or geocentric",
		    "Equatorial", 'E', pref_topogeo_cb,
		    PREF_TOPO, "Topocentric", 'T', "local perspective",
		    PREF_GEO, "Geocentric", 'G', "Earth-centered perspective");

	    pref_simplepair (menu_pane, PREF_DPYPREC, "Precision",
       "Whether numeric values are shown with more or fewer significant digits",
		    "Precision", 'P', pref_dpy_prec_cb,
		    PREF_HIPREC, "Hi", 'H', "display full precision",
		    PREF_LOPREC, "Low", 'L', "use less room");

	    pref_simplepair (menu_pane, PREF_MSG_BELL, "MessageBell",
		"Whether to beep when a message is added to the Message dialog",
		    "Message Bell", 'M', pref_msg_bell_cb,
		    PREF_NOMSGBELL, "Off", 'f', "other people are busy",
		    PREF_MSGBELL, "On", 'O', "the beeps are useful");

	    pref_simplepair (menu_pane, PREF_PRE_FILL, "PromptPreFill",
		"Whether prompt dialogs are prefilled with their current value",
		    "Prompt Prefill", 'f', pref_prefill_cb,
		    PREF_NOPREFILL, "No", 'N', "fresh prompt each time",
		    PREF_PREFILL, "Yes", 'Y', "current value is often close");

	    pref_simplepair (menu_pane, PREF_UNITS, "Units",
		    "Whether to use english or metric units",
		    "Units", 'U', pref_units_cb,
		    PREF_ENGLISH, "English", 'E', "Feet, Fahrenheit",
		    PREF_METRIC, "Metric", 'M', "Meters, Celsius");

	    pref_simplepair (menu_pane, PREF_ZONE, "TZone",
		"Whether time stamps and the calendar are in local time or UTC",
		    "Time zone", 'z', pref_tz_cb,
		    PREF_LOCALTZ, "Local", 'L', "as per TZ Offset",
		    PREF_UTCTZ, "UTC", 'U', "Coordinated Universal Time");

	    pref_simplepair (menu_pane, PREF_TIPS, "Tips",
		    "Whether to display these little tip boxes!",
			"Show help tips", 't', pref_tips_cb,
			PREF_NOTIPS, "No", 'N', "they are in the way",
			PREF_TIPSON, "Yes", 'Y', "they are faster than reading Help");

	    pref_simplepair (menu_pane, PREF_CONFIRM, "Confirm",
		    "Whether to ask before performing irreversible actions",
			"Confirmations", 'C', pref_confirm_cb,
			PREF_NOCONFIRM, "No", 'N', "just do it",
			PREF_CONFIRMON, "Yes", 'Y', "ask first");

	    /* create the date formats pullright menu -- it has 3 options */

	    n = 0;
	    XtSetArg (args[n], XmNradioBehavior, True); n++;
	    pull_right = XmCreatePulldownMenu (menu_pane, "DateFormat",args,n);

		n = 0;
		XtSetArg (args[n], XmNmnemonic, 'M'); n++;
		XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
		tb1 = XmCreateToggleButton (pull_right, "MDY", args, n);
		XtManageChild (tb1);
		XtAddCallback (tb1, XmNvalueChangedCallback, pref_date_cb,
							(XtPointer)PREF_MDY);
		set_xmstring (tb1, XmNlabelString, "M/D/Y");
		wtip (tb1, "Month / Day / Year");

		n = 0;
		XtSetArg (args[n], XmNmnemonic, 'Y'); n++;
		XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
		tb2 = XmCreateToggleButton (pull_right, "YMD", args, n);
		XtAddCallback (tb2, XmNvalueChangedCallback, pref_date_cb,
							(XtPointer)PREF_YMD);
		XtManageChild (tb2);
		set_xmstring (tb2, XmNlabelString, "Y/M/D");
		wtip (tb2, "Year / Month / Day");

		n = 0;
		XtSetArg (args[n], XmNmnemonic, 'D'); n++;
		XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
		tb3 = XmCreateToggleButton (pull_right, "DMY", args, n);
		XtAddCallback (tb3, XmNvalueChangedCallback, pref_date_cb,
							(XtPointer)PREF_DMY);
		XtManageChild (tb3);
		set_xmstring (tb3, XmNlabelString, "D/M/Y");
		wtip (tb3, "Day / Month / Year");

		if (XmToggleButtonGetState(tb1))
		    prefs[PREF_DATE_FORMAT] = PREF_MDY;
		else if (XmToggleButtonGetState(tb2))
		    prefs[PREF_DATE_FORMAT] = PREF_YMD;
		else if (XmToggleButtonGetState(tb3))
		    prefs[PREF_DATE_FORMAT] = PREF_DMY;
		else {
		    xe_msg ("No DateFormat preference is set -- defaulting to MDY", 0);
		    XmToggleButtonSetState (tb1, True, False);
		    prefs[PREF_DATE_FORMAT] = PREF_MDY;
		}

		n = 0;
		XtSetArg (args[n], XmNsubMenuId, pull_right);  n++;
		XtSetArg (args[n], XmNmnemonic, 'D'); n++;
		cascade= XmCreateCascadeButton(menu_pane,"DateFormatCB",args,n);
		XtManageChild (cascade);
		set_xmstring (cascade, XmNlabelString, "Date Formats");
		wtip (cascade, "Format for displaying dates");

	n = 0;
	XtSetArg (args[n], XmNsubMenuId, menu_pane);  n++;
	XtSetArg (args[n], XmNmnemonic, 'P'); n++;
	cascade = XmCreateCascadeButton (menu_bar, "PreferencesCB", args, n);
	set_xmstring (cascade, XmNlabelString, "Preferences");
	XtManageChild (cascade);
	wtip (cascade, "Options effecting overall XEphem operation");
}

/* called anytime we want to know a preference.
 */
int
pref_get(pref)
Preferences pref;
{
	return (prefs[pref]);
}

/* return 1 if want to confirm, else 0 */
int
confirm()
{
	return (pref_get (PREF_CONFIRM) == PREF_CONFIRMON);
}

/* make one option pair.
 * the state of op1 determines the initial settings. to put it another way,
 * if neither option is set the *second* becomes the default.
 */
static void
pref_simplepair (pd, prefname, pdname, tip, cblabel, cbmne, callback,
op1pref, op1name, op1mne, op1tip, op2pref, op2name, op2mne, op2tip)
Widget pd;	/* parent pulldown menu */
int prefname;	/* one of Preferences enum */
char *pdname;	/* pulldown name */
char *tip;	/* tip text for the main cascade pair */
char *cblabel;	/* cascade button label */
int cbmne;	/* cascade button mnemonic character */
XtCallbackProc callback;	/* callback function */
int op1pref;	/* option 1 PREF code */
char *op1name;	/* option 1 TB name */
int op1mne;	/* option 1 TB mnemonic character */
char *op1tip;	/* option 1 tip string */
int op2pref;	/* option 2 PREF code */
char *op2name;	/* option 2 TB name */
int op2mne;	/* option 2 TB mnemonic character */
char *op2tip;	/* option 2 tip string */
{
	Widget pull_right, cascade;
	Widget tb1, tb2;
	Arg args[20];
	int t;
	int n;

	n = 0;
	XtSetArg (args[n], XmNradioBehavior, True); n++;
	pull_right = XmCreatePulldownMenu (pd, pdname, args,n);

	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, op1mne); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb1 = XmCreateToggleButton (pull_right, op1name, args, n);
	    XtAddCallback (tb1, XmNvalueChangedCallback, callback,
							    (XtPointer)op1pref);
	    if (op1tip)
		wtip (tb1, op1tip);
	    XtManageChild (tb1);

	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, op2mne); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb2 = XmCreateToggleButton (pull_right, op2name, args, n);
	    XtAddCallback (tb2, XmNvalueChangedCallback, callback,
							    (XtPointer)op2pref);
	    if (op2tip)
		wtip (tb2, op2tip);
	    XtManageChild (tb2);

	    t = XmToggleButtonGetState(tb1);
	    prefs[prefname] = t ? op1pref : op2pref;
	    XmToggleButtonSetState (tb2, !t, False);

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pull_right);  n++;
	    XtSetArg (args[n], XmNmnemonic, cbmne); n++;
	    cascade = XmCreateCascadeButton (pd, "PrefCB", args, n);
	    XtManageChild (cascade);
	    set_xmstring (cascade, XmNlabelString, cblabel);
	    if (tip)
		wtip (cascade, tip);
}

/* called when a PREF_DATE_FORMAT preference changes.
 * the new value is in client.
 */
/* ARGSUSED */
static void
pref_date_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmToggleButtonCallbackStruct *s = (XmToggleButtonCallbackStruct *)call;

	if (s->set) {
	    prefs[PREF_DATE_FORMAT] = (int)client;
	    redraw_screen (1);
	}
}

/* called when a PREF_UNITS preference changes.
 * the new value is in client.
 */
/* ARGSUSED */
static void
pref_units_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmToggleButtonCallbackStruct *s = (XmToggleButtonCallbackStruct *)call;

	if (s->set) {
	    prefs[PREF_UNITS] = (int)client;
	    redraw_screen (1);
	}
}

/* called when a PREF_ZONE preference changes.
 * the new value is in client.
 */
/* ARGSUSED */
static void
pref_tz_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmToggleButtonCallbackStruct *s = (XmToggleButtonCallbackStruct *)call;

	if (s->set) {
	    prefs[PREF_ZONE] = (int)client;
	    redraw_screen (1);
	}
}

/* called when a PREF_DPYPREC preference changes.
 * the new value is in client.
 */
/* ARGSUSED */
static void
pref_dpy_prec_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmToggleButtonCallbackStruct *s = (XmToggleButtonCallbackStruct *)call;

	if (s->set) {
	    prefs[PREF_DPYPREC] = (int)client;
	    redraw_screen (1);
	}
}

/* called when a PREF_EQUATORIAL preference changes.
 * the new value is in client.
 */
/* ARGSUSED */
static void
pref_topogeo_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmToggleButtonCallbackStruct *s = (XmToggleButtonCallbackStruct *)call;

	if (s->set) {
	    prefs[PREF_EQUATORIAL] = (int)client;
	    redraw_screen (1);
	}
}

/* called when a PREF_MSG_BELL preference changes.
 * the new value is in client.
 */
/* ARGSUSED */
static void
pref_msg_bell_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmToggleButtonCallbackStruct *s = (XmToggleButtonCallbackStruct *)call;

	if (s->set) {
	    prefs[PREF_MSG_BELL] = (int)client;
	}
}

/* called when a PREF_PRE_FILL preference changes.
 * the new value is in client.
 */
/* ARGSUSED */
static void
pref_prefill_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmToggleButtonCallbackStruct *s = (XmToggleButtonCallbackStruct *)call;

	if (s->set) {
	    prefs[PREF_PRE_FILL] = (int)client;
	}
}

/* called when a PREF_TIPS preference changes.
 * the new value is in client.
 */
/* ARGSUSED */
static void
pref_tips_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmToggleButtonCallbackStruct *s = (XmToggleButtonCallbackStruct *)call;

	if (s->set) {
	    prefs[PREF_TIPS] = (int)client;
	}
}

/* called when a PREF_CONFIRM preference changes.
 * the new value is in client.
 */
/* ARGSUSED */
static void
pref_confirm_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmToggleButtonCallbackStruct *s = (XmToggleButtonCallbackStruct *)call;

	if (s->set) {
	    prefs[PREF_CONFIRM] = (int)client;
	}
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: preferences.c,v $ $Date: 1998/04/21 16:43:20 $ $Revision: 1.1 $ $Name:  $"};
