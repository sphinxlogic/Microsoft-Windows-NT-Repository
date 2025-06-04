/* handle the history feature of skyview */

#include <stdio.h>
#if defined(__STDC__)
#include <stdlib.h>
#endif
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "preferences.h"
#include "skyhist.h"

extern Widget toplevel_w;

extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void get_xmstring P_((Widget w, char *resource, char **txtp));
extern void msg_bell P_((void));
extern void query P_((Widget tw, char *msg, char *label1, char *label2,
    char *label3, void (*func1)(), void (*func2)(), void (*func3)()));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *txtp));
extern void xe_msg P_((char *msg, int app_modal));
extern void watch_cursor P_((int want));

/* N.B. if over ten, will need a different mnemonic system */
#define	MAXHIST	10	/* max history items */

/* a collection of PB widgets and the histories they represent.
 * the first nhist entries of hist[] are in use.
 */
typedef struct {
    Widget w;
    SvHistory h;
} Hist;
static Hist hist[MAXHIST];
static int nhist;

static Widget delone_w;		/* PB set to enable deleting one item */
static Widget delall_w;		/* PB set to delete all items */
static int delone_on;		/* set while delete-one mode is on */

static void svh_add_cb P_((Widget w, XtPointer client, XtPointer call));
static void svh_delall_cb P_((Widget w, XtPointer client, XtPointer call));
static void svh_delone_cb P_((Widget w, XtPointer client, XtPointer call));
static void svh_pick_cb P_((Widget w, XtPointer client, XtPointer call));
static void delone_setup P_((int enable));
static void delete_one P_((int n));
static void delete_all_confirmed P_((void));
static void add_one P_((void));
static void setup_pb P_((Hist *hp));

/* called to create the history cascade and pulldown off the given menubar.
 * also gets the current settings and adds it as the first item in the list.
 */
void
svh_create (mb_w)
Widget mb_w;
{
	Widget cb_w, pd_w;
	Widget w;
	Arg args[20];
	XmString acct;
	int i;
	int n;

	n = 0;
	pd_w = XmCreatePulldownMenu (mb_w, "HPD", args, n);

	n = 0;
	XtSetArg (args[n], XmNsubMenuId, pd_w); n++;
	XtSetArg (args[n], XmNmnemonic, 'H'); n++;
	cb_w = XmCreateCascadeButton (mb_w, "History", args, n);
	XtManageChild (cb_w);

	/* create the Add PB */
	acct = XmStringCreateSimple ("Alt-a");
	n = 0;
	XtSetArg (args[n], XmNaccelerator, "Alt<Key>a"); n++;
	XtSetArg (args[n], XmNacceleratorText, acct); n++;
	w = XmCreatePushButton (pd_w, "Add", args, n);
	XtAddCallback (w, XmNactivateCallback, svh_add_cb, NULL);
	XtManageChild (w);
	XmStringFree (acct);

	n = 0;
	w = XmCreateSeparator (pd_w, "HSep1", args, n);
	XtManageChild (w);

	/* create the Delete All PB -- make insensitive until an item is added*/
	n = 0;
	delall_w = XmCreatePushButton (pd_w, "DelAll", args, n);
	XtAddCallback (delall_w, XmNactivateCallback, svh_delall_cb, NULL);
	set_xmstring (delall_w, XmNlabelString, "Delete All");
	XtSetSensitive (delall_w, False);
	XtManageChild (delall_w);

	/* create the Delete One PB -- make insensitive until an item is added*/
	n = 0;
	XtSetArg (args[n], XmNindicatorOn, False); n++;
	delone_w = XmCreatePushButton (pd_w, "DelOne", args, n);
	XtAddCallback (delone_w, XmNactivateCallback, svh_delone_cb, NULL);
	set_xmstring (delone_w, XmNlabelString, "Delete One");
	XtSetSensitive (delone_w, False);
	XtManageChild (delone_w);

	n = 0;
	w = XmCreateSeparator (pd_w, "HSep2", args, n);
	XtManageChild (w);

	/* create the activation PBs. not managed until defined */

	for (i = 0; i < MAXHIST; i++) {
	    char acc[64];

	    (void) sprintf (acc, "Alt-%d", (i+1)%10);
	    acct = XmStringCreateSimple (acc);
	    (void) sprintf (acc, "Alt<Key>%c", i+'1');

	    n = 0;
	    XtSetArg (args[n], XmNaccelerator, acc); n++;
	    XtSetArg (args[n], XmNacceleratorText, acct); n++;
	    w = XmCreatePushButton (pd_w, "H", args, n);
	    XtAddCallback (w, XmNactivateCallback, svh_pick_cb, (XtPointer)i);
	    hist[i].w = w;
	    XmStringFree (acct);
	}

	/* init the list with the current settings */
	add_one();
}

/* called when the Add button is hit in the history pulldown */
/* ARGSUSED */
static void
svh_add_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	/* turn off delete-1 */
	delone_setup (0);

	/* check for room */
	if (nhist == MAXHIST) {
	    char msg[128];

	    (void) sprintf (msg, "No room for additional history -- max is %d",
								    MAXHIST);
	    xe_msg (msg, 1);
	    return;
	}

	/* do it */
	add_one();

	/* ring the bell to confirm (particularly helpful view the accel) */
	if (pref_get (PREF_MSG_BELL) == PREF_MSGBELL)
	    msg_bell();
}

/* called when the Delete all button is hit in the history pulldown */
/* ARGSUSED */
static void
svh_delall_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	/* turn off delete-1 */
	delone_setup (0);

	/* confirm */
	if (nhist > 0)
	    query (toplevel_w, "Delete all Sky view history entries?",
				"Yes -- delete all", "No -- delete none", NULL,
					    delete_all_confirmed, NULL, NULL);
	else
	    xe_msg ("No history entries to delete", 1);
}

/* called when the Delete one PB is activated */
/* ARGSUSED */
static void
svh_delone_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	/* complain if turning it on and nothing to delete */
	if (!delone_on && nhist == 0) {
	    xe_msg ("No history entries to delete", 1);
	    return;
	}

	delone_setup (!delone_on);
}

/* called when a history entry is activated.
 * client is an index into hist[].
 */
/* ARGSUSED */
static void
svh_pick_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int n = (int)client;

	if (n < 0 || n >= nhist) {
	    printf ("Bad history number: %d\n", n);
	    return;
	}


	if (delone_on) {
	    delete_one (n);
	    delone_setup (0);
	} else {
	    watch_cursor (1);
	    svh_goto (&hist[n].h);
	    watch_cursor (0);
	}
}

/* enable or disable deleting one item */
static void
delone_setup (enable)
int enable;
{
	static unsigned long fg, bg;
	Arg args[2];
	int i;

	/* get initial colors once */
	if (fg == bg) {
	    XtSetArg (args[0], XmNforeground, &fg);
	    XtSetArg (args[1], XmNbackground, &bg);
	    XtGetValues (delone_w, args, 2);
	}

	/* toggle colors depending on current mode, if new state */
	if (enable == delone_on)
	    return;
	delone_on = enable;
	if (enable) {
	    XtSetArg (args[0], XmNforeground, bg);
	    XtSetArg (args[1], XmNbackground, fg);
	} else {
	    XtSetArg (args[0], XmNforeground, fg);
	    XtSetArg (args[1], XmNbackground, bg);
	}
	for (i = 0; i < MAXHIST; i++)
	    XtSetValues (hist[i].w, args, 2);
	XtSetValues (delone_w, args, 2);
}

/* delete item n */
static void
delete_one (n)
int n;
{
	char *title;
	int i;

	/* copy all down one */
	for (i = n; i < nhist-1; i++) {
	    get_xmstring (hist[i+1].w, XmNlabelString, &title);
	    set_xmstring (hist[i].w,   XmNlabelString,  title);
	    XtFree (title);
	    hist[i].h = hist[i+1].h;
	}

	/* unuse the last */
	XtUnmanageChild (hist[--nhist].w);

	/* if no more entries, disable the delete buttons */
	if (nhist == 0) {
	    XtSetSensitive (delone_w, False);
	    XtSetSensitive (delall_w, False);
	}
}

/* delete all history entries */
static void
delete_all_confirmed()
{
	int i;

	for (i = 0; i < nhist; i++)
	    XtUnmanageChild (hist[i].w);
	
	nhist = 0;

	/* now that there are no entries, disable the delete buttons */
	XtSetSensitive (delone_w, False);
	XtSetSensitive (delall_w, False);
}

/* get current settings and assign to next position */
static void
add_one()
{
	svh_get (&hist[nhist].h);
	setup_pb (&hist[nhist]);
	nhist++;

	/* now that there is at least one entry, enable the delete buttons */
	XtSetSensitive (delone_w, True);
	XtSetSensitive (delall_w, True);
}

/* setup the widget for the given hist entry */
static void
setup_pb (hp)
Hist *hp;
{
	char buf[128];
	SvHistory *shp = &hp->h;

	if (shp->aa_mode)
	    (void) sprintf (buf, "Alt/Az %5.1f %5.1f %5.1f",
	    	raddeg(shp->altdec), raddeg(shp->azra), raddeg(shp->fov));
	else
	    (void) sprintf (buf, "RA/Dec %5.2f %5.1f %5.1f",
	    	radhr(shp->azra), raddeg(shp->altdec), raddeg(shp->fov));

	set_xmstring (hp->w, XmNlabelString, buf);
	XtManageChild (hp->w);
}
