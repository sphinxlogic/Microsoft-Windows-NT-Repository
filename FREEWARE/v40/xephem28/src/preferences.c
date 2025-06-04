/* code to support the preferences facility.
 */

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>

#include "P_.h"
#include "circum.h"
#include "preferences.h"


extern char	*myclass;
extern Widget	toplevel_w;
#define	XtD	XtDisplay(toplevel_w)

extern void redraw_screen P_((int how_much));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void xe_msg P_((char *msg, int app_modal));

static void pref_algo_cb P_((Widget w, XtPointer client, XtPointer call));
static void pref_date_cb P_((Widget w, XtPointer client, XtPointer call));
static void pref_units_cb P_((Widget w, XtPointer client, XtPointer call));
static void pref_tz_cb P_((Widget w, XtPointer client, XtPointer call));
static void pref_dpy_prec_cb P_((Widget w, XtPointer client, XtPointer call));
static void pref_msg_bell_cb P_((Widget w, XtPointer client, XtPointer call));


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
	Widget tb1, tb2, tb3;
	Widget cascade, menu_pane, pull_right;
	Arg args[20];
	int n;

	n = 0;
	menu_pane = XmCreatePulldownMenu (menu_bar, "Preferences", args, n);

	    /* create the algorithms pullright menu */

	    n = 0;
	    XtSetArg (args[n], XmNradioBehavior, True); n++;
	    pull_right = XmCreatePulldownMenu (menu_pane, "Algorithms", args,n);

	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, 'A'); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb1 = XmCreateToggleButton (pull_right, "Accurate", args, n);
	    XtAddCallback (tb1, XmNvalueChangedCallback, pref_algo_cb,
						    (XtPointer)PREF_ACCURATE);
	    XtManageChild (tb1);

	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, 'F'); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb2 = XmCreateToggleButton (pull_right, "Fast", args, n);
	    XtAddCallback (tb2, XmNvalueChangedCallback, pref_algo_cb,
							(XtPointer)PREF_FAST);
	    XtManageChild (tb2);

	    if (XmToggleButtonGetState(tb1))
		prefs[PREF_ALGO] = PREF_ACCURATE;
	    else if (XmToggleButtonGetState(tb2))
		prefs[PREF_ALGO] = PREF_FAST;
	    else {
		xe_msg ("Neither Alogirthms preference is set -- defaulting to Accurate\n", 0);
		XmToggleButtonSetState (tb1, True, False);
		prefs[PREF_ALGO] = PREF_ACCURATE;
	    }

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pull_right);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'A'); n++;
	    cascade = XmCreateCascadeButton (menu_pane, "AlgorithmsCB",args,n);
	    XtManageChild (cascade);
	    set_xmstring (cascade, XmNlabelString, "Algorithms");

	    /* create the display precision pullright menu */

	    n = 0;
	    XtSetArg (args[n], XmNradioBehavior, True); n++;
	    pull_right = XmCreatePulldownMenu (menu_pane, "Precision", args,n);

	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, 'H'); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb1 = XmCreateToggleButton (pull_right, "Hi", args, n);
	    XtAddCallback (tb1, XmNvalueChangedCallback, pref_dpy_prec_cb,
						    (XtPointer)PREF_HIPREC);
	    XtManageChild (tb1);

	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, 'L'); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb2 = XmCreateToggleButton (pull_right, "Low", args, n);
	    XtAddCallback (tb2, XmNvalueChangedCallback, pref_dpy_prec_cb,
							(XtPointer)PREF_LOPREC);
	    XtManageChild (tb2);

	    if (XmToggleButtonGetState(tb1))
		prefs[PREF_DPYPREC] = PREF_HIPREC;
	    else if (XmToggleButtonGetState(tb2))
		prefs[PREF_DPYPREC] = PREF_LOPREC;
	    else {
		xe_msg ("Neither Precision preference is set -- defaulting to Low\n", 0);
		XmToggleButtonSetState (tb2, True, False);
		prefs[PREF_DPYPREC] = PREF_LOPREC;
	    }

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pull_right);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'P'); n++;
	    cascade = XmCreateCascadeButton (menu_pane, "PrecisionCB",args,n);
	    XtManageChild (cascade);
	    set_xmstring (cascade, XmNlabelString, "Precision");

	    /* create the message bell menu */

	    n = 0;
	    XtSetArg (args[n], XmNradioBehavior, True); n++;
	    pull_right = XmCreatePulldownMenu (menu_pane, "MessageBell",args,n);

	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, 'O'); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb1 = XmCreateToggleButton (pull_right, "On", args, n);
	    XtAddCallback (tb1, XmNvalueChangedCallback, pref_msg_bell_cb,
						    (XtPointer)PREF_MSGBELL);
	    XtManageChild (tb1);

	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, 'f'); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb2 = XmCreateToggleButton (pull_right, "Off", args, n);
	    XtAddCallback (tb2, XmNvalueChangedCallback, pref_msg_bell_cb,
						    (XtPointer)PREF_NOMSGBELL);
	    XtManageChild (tb2);

	    if (XmToggleButtonGetState(tb1))
		prefs[PREF_MSG_BELL] = PREF_MSGBELL;
	    else if (XmToggleButtonGetState(tb2))
		prefs[PREF_MSG_BELL] = PREF_NOMSGBELL;
	    else {
		xe_msg ("Neither Message Bell preference is set -- defaulting to Off\n", 0);
		XmToggleButtonSetState (tb2, True, False);
		prefs[PREF_MSG_BELL] = PREF_NOMSGBELL;
	    }

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pull_right);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'M'); n++;
	    cascade = XmCreateCascadeButton (menu_pane, "MessageBellCB",args,n);
	    XtManageChild (cascade);
	    set_xmstring (cascade, XmNlabelString, "Message Bell");

	    /* create the date formats pullright menu */

	    n = 0;
	    XtSetArg (args[n], XmNradioBehavior, True); n++;
	    pull_right = XmCreatePulldownMenu (menu_pane, "DateFormat",args,n);

	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, 'm'); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb1 = XmCreateToggleButton (pull_right, "MDY", args, n);
	    XtManageChild (tb1);
	    XtAddCallback (tb1, XmNvalueChangedCallback, pref_date_cb,
							(XtPointer)PREF_MDY);
	    set_xmstring (tb1, XmNlabelString, "M/D/Y");
	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, 'y'); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb2 = XmCreateToggleButton (pull_right, "YMD", args, n);
	    XtAddCallback (tb2, XmNvalueChangedCallback, pref_date_cb,
							(XtPointer)PREF_YMD);
	    XtManageChild (tb2);
	    set_xmstring (tb2, XmNlabelString, "Y/M/D");
	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, 'd'); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb3 = XmCreateToggleButton (pull_right, "DMY", args, n);
	    XtAddCallback (tb3, XmNvalueChangedCallback, pref_date_cb,
							(XtPointer)PREF_DMY);
	    XtManageChild (tb3);
	    set_xmstring (tb3, XmNlabelString, "D/M/Y");

	    if (XmToggleButtonGetState(tb1))
		prefs[PREF_DATE_FORMAT] = PREF_MDY;
	    else if (XmToggleButtonGetState(tb2))
		prefs[PREF_DATE_FORMAT] = PREF_YMD;
	    else if (XmToggleButtonGetState(tb3))
		prefs[PREF_DATE_FORMAT] = PREF_DMY;
	    else {
		xe_msg ("No DateFormat preference is set -- defaulting to MDY\n", 0);
		XmToggleButtonSetState (tb1, True, False);
		prefs[PREF_DATE_FORMAT] = PREF_MDY;
	    }

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pull_right);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'D'); n++;
	    cascade = XmCreateCascadeButton (menu_pane, "DateFormatCB",args, n);
	    XtManageChild (cascade);
	    set_xmstring (cascade, XmNlabelString, "Date formats");

	    /* create the units pullright menu */

	    n = 0;
	    XtSetArg (args[n], XmNradioBehavior, True); n++;
	    pull_right = XmCreatePulldownMenu (menu_pane, "Units", args,n);

	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, 'E'); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb1 = XmCreateToggleButton (pull_right, "English", args, n);
	    XtAddCallback (tb1, XmNvalueChangedCallback, pref_units_cb,
						    (XtPointer)PREF_ENGLISH);
	    XtManageChild (tb1);

	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, 'M'); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb2 = XmCreateToggleButton (pull_right, "Metric", args, n);
	    XtAddCallback (tb2, XmNvalueChangedCallback, pref_units_cb,
						    (XtPointer)PREF_METRIC);
	    XtManageChild (tb2);

	    if (XmToggleButtonGetState(tb1))
		prefs[PREF_UNITS] = PREF_ENGLISH;
	    else if (XmToggleButtonGetState(tb2))
		prefs[PREF_UNITS] = PREF_METRIC;
	    else {
		xe_msg ("Neither Units preference is set -- defaulting to English\n", 0);
		XmToggleButtonSetState (tb1, True, False);
		prefs[PREF_UNITS] = PREF_ENGLISH;
	    }

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pull_right);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'U'); n++;
	    cascade = XmCreateCascadeButton (menu_pane, "UnitsCB", args, n);
	    XtManageChild (cascade);
	    set_xmstring (cascade, XmNlabelString, "Units");

	    /* create the time zone pullright menu */

	    n = 0;
	    XtSetArg (args[n], XmNradioBehavior, True); n++;
	    pull_right = XmCreatePulldownMenu (menu_pane, "TZone", args,n);

	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, 'U'); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb1 = XmCreateToggleButton (pull_right, "UTC", args, n);
	    XtAddCallback (tb1, XmNvalueChangedCallback, pref_tz_cb,
						    (XtPointer)PREF_UTCTZ);
	    XtManageChild (tb1);

	    n = 0;
	    XtSetArg (args[n], XmNmnemonic, 'L'); n++;
	    XtSetArg (args[n], XmNvisibleWhenOff, True); n++;
	    tb2 = XmCreateToggleButton (pull_right, "Local", args, n);
	    XtAddCallback (tb2, XmNvalueChangedCallback, pref_tz_cb,
						    (XtPointer)PREF_LOCALTZ);
	    XtManageChild (tb2);

	    if (XmToggleButtonGetState(tb1))
		prefs[PREF_ZONE] = PREF_UTCTZ;
	    else if (XmToggleButtonGetState(tb2))
		prefs[PREF_ZONE] = PREF_LOCALTZ;
	    else {
		xe_msg ("Neither time zone is set -- defaulting to Local\n", 0);
		XmToggleButtonSetState (tb2, True, False);
		prefs[PREF_ZONE] = PREF_LOCALTZ;
	    }

	    n = 0;
	    XtSetArg (args[n], XmNsubMenuId, pull_right);  n++;
	    XtSetArg (args[n], XmNmnemonic, 'T'); n++;
	    cascade = XmCreateCascadeButton (menu_pane, "TZoneCB", args, n);
	    XtManageChild (cascade);
	    set_xmstring (cascade, XmNlabelString, "Zone Display");

	n = 0;
	XtSetArg (args[n], XmNsubMenuId, menu_pane);  n++;
	XtSetArg (args[n], XmNmnemonic, 'P'); n++;
	cascade = XmCreateCascadeButton (menu_bar, "PreferencesCB", args, n);
	set_xmstring (cascade, XmNlabelString, "Preferences");
	XtManageChild (cascade);
}

/* called anytime we want to know a preference.
 */
int
pref_get(pref)
int pref;
{
	return (prefs[pref]);
}

/* called when a PREF_ALGO preference changes.
 * the new value is in client.
 */
/* ARGSUSED */
static void
pref_algo_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmToggleButtonCallbackStruct *s = (XmToggleButtonCallbackStruct *)call;

	if (s->set) {
	    prefs[PREF_ALGO] = (int)client;
	    redraw_screen (1);
	}
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
