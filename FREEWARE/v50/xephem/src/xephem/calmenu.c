/* stuff to control the calendar in the main menu.
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
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/RowColumn.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "preferences.h"

extern Now *mm_get_now P_((void));
extern int f_ison P_((void));
extern int get_color_resource P_((Widget w, char *cname, Pixel *p));
extern void f_showit P_((Widget w, char *s));
extern void fs_tz P_((char *timezonename, int tzpref, Now *np));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void mm_newcaldate P_((double newmjd));
extern void moonnf P_((double Mjd, double *Mjdn, double *Mjdf));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void time_fromsys P_((Now *np));
extern void wtip P_((Widget w, char *tip));


static void today_cb P_((Widget w, XtPointer client, XtPointer call));
static void date_changed_cb P_((Widget w, XtPointer client, XtPointer call));
static void mm_nfmoon P_((double jd, double tzone, int m, int f));

#define	CAL_ROWS	6		/* rows in the date matrix */
#define	CAL_COLS	7		/* columns in the date matrix */
#define	NYEARS		12		/* num entries in the year pulldown */
#define	NMONTHS		12		/* num entries in the month pulldown */
static Widget m_w, mmenu_w[NMONTHS];	/* month cascade btn and pulldwn menu */
static Widget y_w, ymenu_w[NYEARS];	/* year cascade btn and pulldwn menu */
static Widget d_w[CAL_ROWS*CAL_COLS];	/* pushbtns in the date matrix */
static Widget tz_w;			/* timezone + title label */

/* must all be fixed-width since XmMENU_BAR RowColumns don't allow for resizing.
 */
static char mnames[][10] = {
    "January ", "February ", "March    ", "April    ", "May      ", "June     ",
    "July    ", "August   ", "September", "October  ", "November ", "December "
};
static char dnames[CAL_COLS][3] = {
    "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"
};

enum {DAY, MONTH, YEAR};

typedef enum {
    BACKBACK, BACK, TODAY, FORW, FORWFORW
} TodayCuts;

static Pixel fg_pix, bg_pix;		/* used to reverse colors "today" */

#define	NO_DATE	(999)			/* anything outside -31..31 */

/* create a calendar from parent and return the outtermost widget.
 */
Widget
calm_create (parent)
Widget parent;
{
	Arg args[20];
	Widget form_w;
	Widget rc_w;
	Widget menu_w;
	Widget bw, fw;
	Widget w;
	int n;
	int i;

	/* create the outter form */

	n = 0;
	form_w = XmCreateForm (parent, "CalForm", args, n);

	/* put a timezone and title at the top */
	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	tz_w = XmCreateLabel (form_w, "Calendar", args, n);
	wtip (tz_w, "Controls to set the date");
	XtManageChild (tz_w);

	/* create the Year cascade button/pulldown menu */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, tz_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightOffset, 3); n++;
	XtSetArg (args[n], XmNrowColumnType, XmMENU_BAR); n++;
	XtSetArg (args[n], XmNmarginHeight, 1); n++;
	XtSetArg (args[n], XmNmarginWidth, 1); n++;
	XtSetArg (args[n], XmNshadowThickness, 1); n++;
	rc_w = XmCreateRowColumn (form_w, "YearRC", args, n);
	XtManageChild (rc_w);

	n = 0;
	menu_w = XmCreatePulldownMenu (rc_w, "YearPD", args, n);
	/* managed by the CascadeButton */

	    for (i = 0; i < NYEARS; i++) {
		n = 0;
		w = XmCreatePushButton (menu_w, "YearPB", args, n);
		XtAddCallback (w, XmNactivateCallback, date_changed_cb,
							    (XtPointer)YEAR);
		XtManageChild (w);
		ymenu_w[i] = w;
	    }

	n = 0;
	XtSetArg (args[n], XmNsubMenuId, menu_w); n++;
	y_w = XmCreateCascadeButton (rc_w, "YearCB", args, n);
	XtManageChild (y_w);
	wtip (y_w, "Click to set a recent year by picking from a list");

	/* create the Month cascade button/pulldown menu.
	 */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, tz_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftOffset, 3); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNrightWidget, rc_w); n++;
	XtSetArg (args[n], XmNrowColumnType, XmMENU_BAR); n++;
	XtSetArg (args[n], XmNmarginHeight, 1); n++;
	XtSetArg (args[n], XmNmarginWidth, 1); n++;
	XtSetArg (args[n], XmNshadowThickness, 1); n++;
	rc_w = XmCreateRowColumn (form_w, "MonthRC", args, n);
	XtManageChild (rc_w);

	n = 0;
	menu_w = XmCreatePulldownMenu (rc_w, "MonthPD", args, n);
	/* managed by the CascadeButton */

	    for (i = 0; i < NMONTHS; i++) {
		n = 0;
		XtSetArg (args[n], XmNuserData, i+1); n++;
		w = XmCreatePushButton (menu_w, mnames[i], args, n);
		XtAddCallback (w, XmNactivateCallback, date_changed_cb,
							    (XtPointer)MONTH);
		XtManageChild (w);
		mmenu_w[i] = w;
	    }

	n = 0;
	XtSetArg (args[n], XmNsubMenuId, menu_w); n++;
	m_w = XmCreateCascadeButton (rc_w, "MonthCB", args, n);
	XtManageChild (m_w);
	wtip (m_w, "Click to set the month by picking from a list");

	/* create the rowcol for the main date matrix */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNnumColumns, CAL_ROWS+1); n++; /* +1 for dnames*/
	XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
	XtSetArg (args[n], XmNadjustMargin, False); n++;
	XtSetArg (args[n], XmNspacing, 0); n++;
	rc_w = XmCreateRowColumn (form_w, "CalRC", args, n);
	XtManageChild (rc_w);

	/* add the fixed day abbreviations */

	for (i = 0; i < XtNumber(dnames); i++) {
	    n = 0;
	    w = XmCreateLabel (rc_w, dnames[i], args, n);
	    XtManageChild (w);
	}

	/* add the calendar entries proper */

	for (i = 0; i < CAL_ROWS*CAL_COLS; i++) {
	    n = 0;
	    XtSetArg (args[n], XmNrecomputeSize, False); n++;
	    XtSetArg (args[n], XmNshadowThickness, 1); n++;
	    d_w[i] = XmCreatePushButton (rc_w, "CD", args, n);
	    XtAddCallback (d_w[i], XmNactivateCallback, date_changed_cb,
								(XtPointer)DAY);
	    XtManageChild (d_w[i]);
	}

	/* add the Today and surrounding buttons */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopOffset, 3); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	w = XmCreatePushButton (form_w, "<<", args, n);
	XtManageChild (w);
	XtAddCallback (w, XmNactivateCallback, today_cb, (XtPointer)BACKBACK);
	wtip (w, "Move back one week");

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopOffset, 3); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNleftWidget, w); n++;
	bw = XmCreatePushButton (form_w, "<", args, n);
	XtManageChild (bw);
	XtAddCallback (bw, XmNactivateCallback, today_cb, (XtPointer)BACK);
	wtip (bw, "Move back one day");

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopOffset, 3); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	w = XmCreatePushButton (form_w, ">>", args, n);
	XtManageChild (w);
	XtAddCallback (w, XmNactivateCallback, today_cb, (XtPointer)FORWFORW);
	wtip (w, "Move forward one week");

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopOffset, 3); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNrightWidget, w); n++;
	fw = XmCreatePushButton (form_w, ">", args, n);
	XtManageChild (fw);
	XtAddCallback (fw, XmNactivateCallback, today_cb, (XtPointer)FORW);
	wtip (fw, "Move forward one day");

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopOffset, 3); n++;
	XtSetArg (args[n], XmNtopWidget, rc_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNleftWidget, bw); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNrightWidget, fw); n++;
	w = XmCreatePushButton (form_w, "Today", args, n);
	XtManageChild (w);
	XtAddCallback (w, XmNactivateCallback, today_cb, (XtPointer)TODAY);
	wtip (w, "Set calendar to today's date");

	get_something (d_w[0], XmNforeground, (XtArgVal)&fg_pix);
	get_something (d_w[0], XmNbackground, (XtArgVal)&bg_pix);

	return (form_w);
}

/* set the calendar to the time of the given Now *np.
 * use local time if PREF_ZONE is PREF_LOCALTZ
 * only really do it if f_ison() changed or f_ison() now and day changed.
 */
void
calm_set (np)
Now *np;
{
	static double last_jd;
	static int wason = 0;
	Pixel om_pix;
	int localtz;
	char buf[64];
	int zonepref;
	int m, y;
	double d;
	int ison, ok;
	int f;		/* day of week of first day of month Sun=0 */
	int nd;		/* number of days in this month */
	double jd;	/* current mjd; corrected for TZ if PREF_LOCALTZ */
	double jd0;	/* mjd of first day of the month */
	int i;

	/* preliminaries for time zone */
	zonepref = pref_get(PREF_ZONE);
	localtz = (zonepref == PREF_LOCALTZ);
	jd = localtz ? mjd - tz/24.0 : mjd;

	/* set name of current timezone preference in title */
	fs_tz (buf, zonepref, np);
	(void) strcat (buf, " Calendar");
	f_showit (tz_w, buf);

	/* proceed if f_ison() changed or f_ison() and it's a different day */
	ison = f_ison();
	ok = ison != wason || (ison && mjd_day(last_jd) != mjd_day(jd));
	wason = ison;
	if (!ok)
	    return;

	/* remember the new jd we are working on */
	last_jd = jd;

	/* get today's month, day, year */
	mjd_cal (jd, &m, &d, &y);

	/* label the month */
	f_showit (m_w, mnames[m-1]);

	/* label the year and set the menu entries around it */
	(void) sprintf (buf, "%d", y);
	f_showit (y_w, buf);
	for (i = 0; i < NYEARS; i++) {
	    int ty = (y - NYEARS/3) + i; /* current year about 1/3 down list */
	    (void) sprintf (buf, "%d", ty);
	    f_showit (ymenu_w[i], buf);
	    set_something (ymenu_w[i], XmNuserData, (XtArgVal)ty);
	}

	/* find day of week of first day of month */
	cal_mjd (m, 1.0, y, &jd0);
	if (mjd_dow (jd0, &f) < 0) {
	    /* can't figure it out - too hard before Gregorian */
	    for (i = 0; i < CAL_ROWS*CAL_COLS; i++) {
		set_something (d_w[i], XmNuserData, (XtArgVal)NO_DATE);
		f_showit (d_w[i], "  ");
	    }
	    return;
	}

	/* find number of days in this month */
	mjd_dpm (jd0, &nd);

	/* print the calendar.
	 * set userData to the day of the month.
	 */
	(void) get_color_resource (d_w[0], "CalOtherMonthColor", &om_pix);
	for (i = 0; i < CAL_ROWS*CAL_COLS; i++) {
	    if (i < f || i > f + nd - 1) {
		/* last or next month */
		double tmpjd, tmpd;
		int tmpm, tmpy;
		cal_mjd (m, (double)(i-f+1), y, &tmpjd);
		mjd_cal (tmpjd, &tmpm, &tmpd, &tmpy);
		(void) sprintf (buf, "%2d", (int)floor(tmpd+0.5));
		set_something (d_w[i], XmNforeground, (XtArgVal)om_pix);
		set_something (d_w[i], XmNbackground, (XtArgVal)bg_pix);
	    } else {
		(void) sprintf (buf, "%2d", i-f+1);
		if (i == f + (int)d - 1) {
		    /* reverse colors on today */
		    set_something (d_w[i], XmNforeground, (XtArgVal)bg_pix);
		    set_something (d_w[i], XmNbackground, (XtArgVal)fg_pix);
		} else {
		    set_something (d_w[i], XmNforeground, (XtArgVal)fg_pix);
		    set_something (d_w[i], XmNbackground, (XtArgVal)bg_pix);
		}
	    }

	    f_showit (d_w[i], buf);
	    set_something (d_w[i], XmNuserData, (XtArgVal)(i-f+1));
	}

	/* over print the new and full moons for days near this month.
	 * TODO: don't really know which dates to use here (see moonnf())
	 *   so try several to be fairly safe. have to go back to 4/29/1988
	 *   to find the full moon on 5/1 for example.
	 */
	mm_nfmoon (jd0-3, tz, m, f);
	mm_nfmoon (jd0+15, tz, m, f);
	mm_nfmoon (jd0+30, tz, m, f);
}

/* called when Today or its helpers are pressed.
 * client is one of TodayCuts.
 * set m/d/y but retain any partion day.
 */
/* ARGSUSED */
static void
today_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	TodayCuts c = (TodayCuts)client;
	Now *np = mm_get_now();
	int localtz;
	double newmjd;
	double jd;
	int m, y;
	double d;

	switch (c) {
	case BACKBACK:
	    newmjd = mjd - 7;
	    break;

	case BACK:
	    newmjd = mjd - 1;
	    break;

	case TODAY:
	    localtz = pref_get(PREF_ZONE) == PREF_LOCALTZ;
	    jd = localtz ? mjd - tz/24.0 : mjd;

	    mjd_cal (jd, &m, &d, &y);
	    time_fromsys (np);
	    jd = localtz ? mjd - tz/24.0 : mjd;
	    newmjd = mjd_day(jd) + (d - floor(d));

	    if (localtz)
		newmjd += tz/24.0;
	    break;

	case FORW:
	    newmjd = mjd + 1;
	    break;

	case FORWFORW:
	    newmjd = mjd + 7;
	    break;

	default:
	    newmjd = mjd;
	    break;
	}

	/* this always wants UTC */
	mm_newcaldate (newmjd);
}

/* called when any of the calendar pushbuttons is activated.
 * client is one of DAY, MONTH or YEAR.
 * userData is new value, ie, a month, day or year.
 * N.B. we honor PREF_ZONE
 */
/* ARGSUSED */
static void
date_changed_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	Now *np = mm_get_now();
	int code = (int)client;
	double jd;
	double newmjd;
	int localtz;
	int x;
	int m, y;
	double d;

	get_something (w, XmNuserData, (XtArgVal)&x);
	if (x == NO_DATE)
	    return;

	localtz = pref_get (PREF_ZONE) == PREF_LOCALTZ;
	jd = localtz ? mjd - tz/24.0 : mjd;

	mjd_cal (jd, &m, &d, &y);

	switch (code) {
	case MONTH:
	    cal_mjd (x, d, y, &newmjd);
	    break;
	case DAY:
	    d = x + mjd_hr(jd)/24.0;	/* preserve any partial day */
	    cal_mjd (m, d, y, &newmjd);
	    break;
	case YEAR:
	    cal_mjd (m, d, x, &newmjd);
	    break;
	}

	if (localtz)
	    newmjd += tz/24.0;

	/* this function always wants UTC */
	mm_newcaldate (newmjd);
}

/* print the new and full moons for the months surrounding jd, where
 * m is the month-of-year for the current month and f is index into d_w[]
 * of the first day of this month.
 */
static void
mm_nfmoon (jd, tzone, m, f)
double jd;
double tzone;
int m, f;
{
	static char nms[] = "NM", fms[] = "FM";
	double jdn, jdf;	/* mjd of new and full moons near jd */
	int mm, ym;
	double dm;
	int ndays;
	int di;

	moonnf (jd, &jdn, &jdf);
	if (pref_get(PREF_ZONE) == PREF_LOCALTZ) {
	    jdn -= tzone/24.0;
	    jdf -= tzone/24.0;
	}

	mjd_cal (jdn, &mm, &dm, &ym);
	if (mm < m) {
	    mjd_dpm (jdn, &ndays);
	    dm -= ndays;
	} else if (mm > m) {
	    mjd_dpm (jd, &ndays);
	    dm += ndays;
	}
	di = dm + f - 1;
	if (di >= 0 && di < CAL_ROWS*CAL_COLS)
	    f_showit (d_w[di], nms);

	mjd_cal (jdf, &mm, &dm, &ym);
	if (mm < m) {
	    mjd_dpm (jdn, &ndays);
	    dm -= ndays;
	} else if (mm > m) {
	    mjd_dpm (jd, &ndays);
	    dm += ndays;
	}
	di = dm + f - 1;
	if (mm == m && di >= 0 && di < CAL_ROWS*CAL_COLS)
	    f_showit (d_w[di], fms);
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: calmenu.c,v $ $Date: 1999/10/06 21:34:24 $ $Revision: 1.5 $ $Name:  $"};
