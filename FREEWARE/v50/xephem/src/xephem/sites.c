/* code to read and manipulate the file of sites.
 * N.B. we only read the file once. users of this code rely on this.
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#if defined(__STDC__)
#include <stdlib.h>
typedef const void * qsort_arg;
#else
typedef void * qsort_arg;
extern void *malloc(), *realloc();
#endif

#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>
#include <Xm/SelectioB.h>
#include <Xm/List.h>
#include <Xm/TextF.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "preferences.h"
#include "sites.h"

extern Widget	toplevel_w;
#define XtD     XtDisplay(toplevel_w)
extern Colormap xe_cm;

extern FILE *fopenh P_((char *name, char *how));
extern char *getShareDir P_((void));
extern char *syserrstr P_((void));
extern void mm_setsite P_((double slat, double slng, Site *sp, int update));
extern void prompt_map_cb P_((Widget w, XtPointer client, XtPointer call));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *text));
extern void xe_msg P_((char *msg, int app_modal));

static int compstr P_((char *str, char *comp));
static void create_sq_w P_((void));
static void sites_fillsl P_((Widget sl_w));
static int sites_read_file P_((void));
static int sites_cmpf P_((qsort_arg v1, qsort_arg v2));
static void set_selected P_((void));
static void sq_ok_cb P_((Widget w, XtPointer client, XtPointer call));
static void sq_cancel_cb P_((Widget w, XtPointer client, XtPointer call));
static void sq_tf_cb P_((Widget w, XtPointer client, XtPointer call));
static void sq_actlist_cb P_((Widget w, XtPointer client, XtPointer call));

#define	MAXSITELLEN	256	/* maximum site file line length */
static Site **sitesp;		/* malloced list of malloced sites read file */
static int nsitesp;		/* number of entries in sitesp array */
static Widget sq_w;		/* sites query form dialog */
static Widget sql_w;		/* sites query scrolled list */

/* if sitesp has been set return nsitesp, else try again to read a sites file.
 * if still no luck, return -1.
 * also, if sippp != NULL set *sippp to sitesp.
 */
int
sites_get_list (sippp)
Site ***sippp;
{
	if (!sitesp && sites_read_file() < 0)
	    return (-1);

	if (sippp)
	    *sippp = sitesp;
	return (nsitesp);
}

/* let user choose a site from a scrolled list, if there is one */
void
sites_query()
{
	if (!sitesp && sites_read_file() < 0)
	    return;

	if (!sq_w) {
	    create_sq_w();
	    sites_fillsl (sql_w);
	}

	XtManageChild (sq_w);
}

/* search the sitesp list for first one which contains str, ignoring case and
 *   whitespace. start at the given position.
 * if found, scroll to it and return index into sites[], else return -1.
 */
int
sites_search (startpos, str)
int startpos;
char *str;
{
	char s1[MAXSITELLEN];
	int s1l;
	int i, n;

	if (!sitesp && sites_read_file() < 0)
	    return (-1);

	if (!sq_w) {
	    create_sq_w();
	    sites_fillsl (sql_w);
	}

	/* make copy of str in s1[] without whitespace and all upper case */
	s1l = compstr (str, s1);

	/* after doing the same for each name in sites[], check each
	 * possible offset location starting at startpos.
	 */
	for (n = 0, i = startpos; n < nsitesp; n++, i = (i+1)%nsitesp) {
	    Site *sip = sitesp[i];
	    char s2[MAXSITELLEN];
	    int s2l;
	    int dl;

	    s2l = compstr (sip->si_name, s2);

	    for (dl = s2l - s1l; dl >= 0; dl--)
		if (strncmp (s2+dl, s1, s1l) == 0) {
		    XmListSetPos (sql_w, i+1);	 	/* scroll it to top */
		    XmListSelectPos (sql_w, i+1, True);	/* highlight it */
		    return (i);
		}
	}

	return (-1);
}

/* fill ab[maxn] with an abbreviated version of full.
 * N.B. allow for full == NULL or full[0] == '\0'.
 */
void
sites_abbrev (full, ab, maxn)
char *full, ab[];
int maxn;
{
	int fl;
	int n;

	/* check edge conditions */
	if (!full || (fl = strlen(full)) == 0)
	    return;

	/* just copy if it all fits ok */
	if (fl < maxn) {
	    (void) strcpy (ab, full);
	    return;
	}

	/* clip off words from the right until short enough.
	 * n is an index, not a count.
	 */
	for (n = fl-1; n >= maxn-4; ) {
	    while (n > 0 && isalnum(full[n]))
		n--;
	    while (n > 0 && (ispunct(full[n]) || isspace(full[n])))
		n--;
	}
	(void) sprintf (ab, "%.*s...", n+1, full);
}

/* copy str to comp, removing all whitespace and converting all lower case
 * to uppercase.
 * return length of comp (not counting final \0, which we insure).
 */
static int
compstr (str, comp)
char *str;
char *comp;
{
	int i;
	char c;

	for (i = 0; (c = *str++) != '\0'; ) {
	    if (isspace(c))
		continue;
	    if (islower(c))
		c = toupper(c);
	    if (i < MAXSITELLEN-1)
		comp[i++]  = c;
	}
	comp[i] = '\0';

	return (i);
}

/* make the site selection dialog */
static void
create_sq_w()
{
	Widget cf_w, sp_w, stf_w;
	Widget w;
	Arg args[20];
	int n;
	
	/* create outter form dialog */

	n = 0;
	XtSetArg (args[n], XmNdefaultPosition, False);  n++;
	XtSetArg (args[n], XmNverticalSpacing, 7); n++;
	XtSetArg (args[n], XmNwidth, 300);
	XtSetArg (args[n], XmNcolormap, xe_cm); n++;
	XtSetArg (args[n], XmNmarginHeight, 10); n++;
	XtSetArg (args[n], XmNmarginWidth, 10); n++;
	sq_w = XmCreateFormDialog(toplevel_w, "Sites", args, n);
	set_something (sq_w, XmNcolormap, (XtArgVal)xe_cm);
	XtAddCallback (sq_w, XmNmapCallback, prompt_map_cb, NULL);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem Site Selection"); n++;
	XtSetValues (XtParent(sq_w), args, n);

	/* make the controls at the bottom */
	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNfractionBase, 15); n++;
	XtSetArg (args[n], XmNverticalSpacing, 5); n++;
	cf_w = XmCreateForm (sq_w, "SitesCF", args, n);
	XtManageChild (cf_w);

	    n = 0;
	    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNleftPosition, 3); n++;
	    XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNrightPosition, 6); n++;
	    w = XmCreatePushButton (cf_w, "Ok", args, n);
	    XtAddCallback (w, XmNactivateCallback, sq_ok_cb, NULL);
	    XtManageChild (w);

	    n = 0;
	    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNleftPosition, 9); n++;
	    XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	    XtSetArg (args[n], XmNrightPosition, 12); n++;
	    w = XmCreatePushButton (cf_w, "Cancel", args, n);
	    XtAddCallback (w, XmNactivateCallback, sq_cancel_cb, NULL);
	    XtManageChild (w);

	/* make a text field above the sep to enter the search string */
	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, cf_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	stf_w = XmCreateTextField (sq_w, "SitesTF", args, n);
	XtAddCallback (stf_w, XmNactivateCallback, sq_tf_cb, NULL);
	XtManageChild (stf_w);

	/* make a prompt above the text field */
	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, stf_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	sp_w = XmCreateLabel (sq_w, "SitesSL", args, n);
	set_xmstring (sp_w, XmNlabelString, "Search:");
	XtManageChild (sp_w);

	/* make the scrolled list at the top */
	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, sp_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNselectionPolicy, XmBROWSE_SELECT); n++;
	sql_w = XmCreateScrolledList (sq_w, "SiteSL", args, n);
	XtAddCallback (sql_w, XmNdefaultActionCallback, sq_actlist_cb, NULL);
	XtManageChild (sql_w);
}

/* read the sites file and set sitesp and nsitesp if successful to the list
 *   sorted by name.
 * return 0 if ok, else write to xe_msg() and return -1.
 * N.B. we assume this is never called again once a good file is found.
 */
static int
sites_read_file ()
{
	char buf[MAXSITELLEN];
	char msg[1024];
	char fn[1024];
	FILE *fp;

	/* open the sites file */
#ifndef VMS
	(void) sprintf (fn, "%s/auxil/xephem_sites",  getShareDir());
#else
	(void) sprintf (fn, "%s[auxil]xephem_sites",  getShareDir());
#endif
	fp = fopenh (fn, "r");
	if (!fp) {
	    (void) sprintf(msg, "%s: %s", fn, syserrstr());
	    xe_msg (msg, 1);
	    return (-1);
	}

	/* read each entry, building up list */
	while (fgets (buf, sizeof(buf), fp) != NULL) {
	    char name[MAXSITELLEN];
	    char tzdefn[128];
	    int latd, latm, lats;
	    int lngd, lngm, lngs;
	    char latNS, lngEW;
	    double lt, lg;
	    char *newmem;
	    double ele;
	    Site *sp;
	    int l;
	    int nf;

	    /* read line.. skip if not complete. tz is optional */
	    tzdefn[0] = '\0';
	    nf = sscanf (buf, "%[^;]; %3d %2d %2d %c   ; %3d %2d %2d %c   ;%lf ; %s",
				    name, &latd, &latm, &lats, &latNS,
				    &lngd, &lngm, &lngs, &lngEW, &ele, tzdefn);
	    if (nf < 10)
		continue;

	    /* strip trailing blanks off name */
	    for (l = strlen (name); --l >= 0; )
		if (isspace(name[l]))
		    name[l] = '\0';
		else
		    break;

	    /* crack location */
	    lt = degrad (latd + latm/60.0 + lats/3600.0);
	    if (latNS == 'S')
		lt = -lt;
	    lg = degrad (lngd + lngm/60.0 + lngs/3600.0);
	    if (lngEW == 'W')
		lg = -lg;

	    /* malloc and fill a new Site record */
	    sp = (Site *) calloc (1, sizeof(Site) + strlen(name));
	    if (!sp) {
		(void) sprintf(msg, "Out of memory -- partial sites only.");
		xe_msg (msg, 1);
		break;
	    }
	    sp->si_lat = lt;
	    sp->si_lng = lg;
	    sp->si_elev = ele;
	    (void) strncpy (sp->si_tzdefn, tzdefn, sizeof(sp->si_tzdefn));
	    (void) strcpy (sp->si_name, name);

	    /* extend sitesp array */
	    if (sitesp)
		newmem= realloc((char *)sitesp, (nsitesp+1)*sizeof(Site *));
	    else
		newmem = malloc (sizeof(Site *));
	    if (!newmem) {
		(void) sprintf(msg, "Out of memory -- partial sites only.");
		free ((void *)sp);
		xe_msg (msg, 1);
		break;
	    }
	    sitesp = (Site **) newmem;
	    sitesp[nsitesp++] = sp;
	}

	(void) fclose (fp);

	/* sort by name */
	if (nsitesp > 0)
	    qsort ((void *)sitesp, nsitesp, sizeof(Site *), sites_cmpf);

	return (0);
}

/* fill the given ScrolledList with the set of sites.
 * N.B. we assume this is only ever called once.
 */
static void
sites_fillsl (sl_w)
Widget sl_w;
{
	XmString *xms;
	int i;

	if (!sitesp)
	    return;

	/* build array of XmStrings for fast updating of the ScrolledList */
	xms = (XmString *) XtMalloc (nsitesp * sizeof(XmString));
	for (i = 0; i < nsitesp; i++)
	    xms[i] = XmStringCreateSimple (sitesp[i]->si_name);
	XmListAddItems (sl_w, xms, nsitesp, 0);

	/* finished with the XmStrings table */
	for (i = 0; i < nsitesp; i++)
	    XmStringFree (xms[i]);
	XtFree ((void *)xms);
}

/* compare name portions of two pointers to pointers to Sites in qsort fashion.
 */
static int
sites_cmpf (v1, v2)
qsort_arg v1;
qsort_arg v2;
{
	char *name1 = (*(Site **)v1)->si_name;
	char *name2 = (*(Site **)v2)->si_name;

	return (strcmp (name1, name2));
}

/* find the current list selection, if exactly one, and set main.
 */
static void
set_selected ()
{
	int *pos, npos;

	/* register the selected location */
	if (XmListGetSelectedPos (sql_w, &pos, &npos)) {
	    if (npos == 1 && pos[0] > 0 && pos[0] <= nsitesp) {
		Site *sip = sitesp[pos[0]-1]; /* pos is 1-based */
		mm_setsite(sip->si_lat, sip->si_lng, sip, 0);
	    } else
		xe_msg ("Please select one site to load.", 1);
	    XtFree((void *)pos);
	} else
	    xe_msg ("Site disappeared!", 0);
}

/* called when the Ok button is hit on the sites query prompt */
static void
sq_ok_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	set_selected ();
	XtUnmanageChild (sq_w);
}

/* called when the Cancel button is hit on the sites query prompt */
static void
sq_cancel_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XtUnmanageChild (sq_w);
}

/* called when CR is hit in the search text field */
static void
sq_tf_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	static int lastpos;
	char *str;
	int pos;

	str = XmTextFieldGetString (w);
	pos = sites_search (lastpos, str);
	if (pos >= 0)
	    lastpos = (pos + 1)%nsitesp;
	else
	    xe_msg ("No matching site found.", 1);

	XtFree (str);
}

/* called when an item in the scrolled list is double-clicked */
static void
sq_actlist_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	set_selected ();
	XtUnmanageChild (sq_w);
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: sites.c,v $ $Date: 1999/02/08 08:19:18 $ $Revision: 1.3 $ $Name:  $"};
