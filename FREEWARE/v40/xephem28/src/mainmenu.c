/* code to manage the stuff on the (permanent) main menu.
 * this is also where the single static Now struct is maintained.
 * the calendar is managed in calmenu.c.
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#if defined(__STDC__)
#include <stdlib.h>
#endif
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <Xm/Xm.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/SelectioB.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "sites.h"
#include "preferences.h"

extern Widget toplevel_w;
extern char *myclass;
extern XtAppContext xe_app;
#define	XtD XtDisplay(toplevel_w)

extern Obj *db_basic P_((int id));
extern Widget calm_create P_((Widget parent));
extern double atod P_((char *buf));
extern int listing_ison P_((void));
extern int plot_ison P_((void));
extern int srch_eval P_((double Mjd, double *tmincp));
extern int tz_fromsys P_((Now *np));
extern void all_update P_((Now *np, int how_much));
extern void calm_set P_((Now *np));
extern void db_invalidate P_((void));
extern void db_loadinitial P_((void));
extern void db_set_cp P_((void));
extern void dm_riset P_((Now *np, Obj *op, RiseSet *rsp));
extern void f_date P_((Widget w, double jd));
extern void f_dms_angle P_((Widget w, double a));
extern void f_double P_((Widget w, char *fmt, double f));
extern void f_mtime P_((Widget w, double t));
extern void f_off P_((void));
extern void f_on P_((void));
extern void f_scansex P_((double o, char bp[], double *np));
extern void f_sexa P_((Widget wid, double a, int w, int fracbase));
extern void f_showit P_((Widget w, char *s));
extern void f_sscandate P_((char *bp, int pref, int *m, double *d, int *y));
extern void f_string P_((Widget w, char *s));
extern void f_time P_((Widget w, double t));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void get_xmstring P_((Widget w, char *resource, char **txtp));
extern void inc_mjd P_((Now *np, double inc, int rtcflag));
extern void listing P_((void));
extern void mm_reset P_((void));
extern void moonnf P_((double Mjd, double *Mjdn, double *Mjdf));
extern void now_lst P_((Now *np, double *lst));
extern void plot P_((void));
extern void prompt_map_cb P_((Widget w, XtPointer client, XtPointer call));
extern void redraw_screen P_((int how_much));
extern void register_selection P_((char *name));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_t0 P_((Now *np));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void time_fromsys P_((Now *np));
extern void twilight_cir P_((Now *np, double dis, double *dawn, double *dusk,
    int *status));
extern void watch_cursor P_((int want));
extern void xe_msg P_((char *msg, int app_modal));

typedef struct {
    int id;		/* see below -- used just as a cross-check */
    char *prompt;	/* used when asking for new value or NULL if can't */
    char *label;	/* used for the menu label or NULL */
    char *name;		/* used when selecting for plotting or NULL if can't */
    char *altp[3];	/* alternate prompts */
    Widget pb_w;	/* pushbutton for display and changing/selecting.
    			 * N.B. but only if id != GAP
			 */
} Field;


static void create_main_form P_((Widget main_window));
static void read_default_resources P_((void));
static Widget fw P_((int fid));
static void mm_new_now P_((void));
static void mm_set_step_code P_((char *bp));
static void mm_set_buttons P_((int whether));
static void mm_activate_cb P_((Widget w, XtPointer client, XtPointer call));
static void mm_set_alt_prompts P_((Field *fp));
static void mm_timer_cb P_((XtPointer client, XtIntervalId *id));
static void mm_go_cb P_((Widget w, XtPointer client, XtPointer call));
static void mm_step_cb P_((Widget w, XtPointer client, XtPointer call));
static void mm_go P_((void));
static void print_tminc P_((int force));
static void print_updating P_((void));
static void print_idle P_((void));
static void print_running P_((void));
static void print_status P_((char *s));
static void print_nstep P_((int force));
static void print_spause P_((int force));
static int crack_fieldset P_((char *buf));
static int chg_fld P_((char *bp, Field *fp));
static void prompt_ok_cb P_((Widget w, XtPointer client, XtPointer call));
static void prompt P_((Field *fp));
static void mm_now P_((int all));
static void mm_twilight P_((int force));
static void mm_newcir P_((int y));
static void mm_newcir_cb P_((XtPointer client, XtIntervalId *id));

/* shorthands for fields of a Now structure, now.
 * first undo the ones for a Now pointer from circum.h.
 */
#undef mjd
#undef lat
#undef lng
#undef tz
#undef temp
#undef pressure
#undef elev
#undef dip
#undef epoch
#undef tznm

#define mjd	now.n_mjd
#define lat	now.n_lat
#define lng	now.n_lng
#define tz	now.n_tz
#define temp	now.n_temp
#define pressure now.n_pressure
#define elev	now.n_elev
#define	dip	now.n_dip
#define epoch	now.n_epoch
#define tznm	now.n_tznm

#define	XMJD	-8391.	/* any unlikely mjd that can be stored exactly */

static Now now = {XMJD};/* where when and how, right now */
static double tminc;	/* hrs to inc time by each loop, but see StepOptions */
static int nstep;	/* steps to go before stopping */
static int spause;	/* msecs to pause between steps */
static int newcir = 1;	/* set when circumstances change - means don't tminc */
static int movie;	/* true while a movie is running */
static int autotz = 0;	/* use tz_fromsys() each time step if this is on.
			 * set by using any Now; reset if explicitly set TZ Name
			 * or Offset.
			 */

/* strings used to command and prompt for turning autotz on and off */
static char AutoOn[] = "Auto on";
static char AutoOff[] = "Auto off";

static XtIntervalId mm_interval_id;	/* set while waiting in a pause loop */
static int mm_selecting;        /* set while our fields are being selected */

#define	NO_PAUSE_DELAY	50	/* min ms delay to use, regardless of spause */
#define	NC_BLINK_DELAY	200	/* ms between NEW CIRCUMSTANCES state changes */

/* field ids
 * N.B. must be in same order as they appear in mm_field_map[].
 */
enum {
    JD_FID, UD_FID, UT_FID, LST_FID,TZN_FID, TZONE_FID, LD_FID, LT_FID, 
    DIP_FID, DAWN_FID, DUSK_FID, LON_FID, GAP, STPSZ_FID, NSTEP_FID, PAUSE_FID,
    SITE_FID, LAT_FID, LONG_FID, ELEV_FID, TEMP_FID, PRES_FID, EPOCH_FID
};

/* array of label/button pairs.
 * N.B. these must be in the same order as the _FID enums so the XX_FID may
 *   be used as indices into the array.
 * N.B. some of the prompts get set based on preferences or other criteria but
 *   we need to put *something* in the prompt field of selectable fields to
 *   trigger making a button. See mm_set_alt_prompts().
 * N.B. if add or delete any then update fmi[] in create_main_form().
 */
static Field mm_field_map[] = {
    {JD_FID,	"Julian Date: ",			"Julian:","JD"},
    {UD_FID,	"dummy",				"UTC Date:","UD",
	{   "UTC date (m/d/y or year.d): ",
	    "UTC date (y/m/d or year.d): ",
	    "UTC date (d/m/y or year.d): "
	}
    },
    {UT_FID,	"UTC time (h:m:s): ",			"UTC Time:", "UT"},
    {LST_FID,	"Local sidereal time (h:m:s): ",	"Sidereal:", "LST"},
    {TZN_FID,	"dummy",				"TZ Name:", NULL,
	{    "Enter Timezone abbreviation (7 char max),\n\
or select Auto On to track OS: ",
	     "Enter Timezone abbreviation (7 char max),\n\
or select Auto Off to turn off tracking: ",
	     NULL
	}
    },
    {TZONE_FID,	"dummy",				"TZ Offset:", "TZ",
	{    "Enter Hours behind UTC (h:m:s),\n\
or select Auto On to track OS: ",
	     "Enter Hours behind UTC (h:m:s),\n\
or select Auto Off to turn off tracking: ",
	     NULL
	}
    },
    {LD_FID,	"dummy",				"Local Date:", "LD",
	{   "Local date (m/d/y or year.d): ",
	    "Local date (y/m/d or year.d): ",
	    "Local date (d/m/y or year.d): "
	}
    },
    {LT_FID,	"Local time (h:m:s): ",			"Local Time:", "LT"},
    {DIP_FID,	"Sun's twilight dip, degrees below",	"Sun Dip:", NULL},
    {DAWN_FID,	NULL,					"Dawn:", "Dawn"},
    {DUSK_FID,	NULL,					"Dusk:", "Dusk"},
    {LON_FID,	NULL,				    	"Night:", "NiteLen"},
    {GAP},
    {STPSZ_FID,	"\
Select one of the above shortcuts\n\
or enter any time step as follows:\n\
    h:m:s, or\n\
    <x>d for x days, or\n\
    <x>s for x sidereal days",				"Step:", NULL},
    {NSTEP_FID, "Number of steps to run: ",		"N Steps:", NULL},
    {PAUSE_FID,	"Seconds to pause between steps: ",	"Pause:", NULL},
    {SITE_FID,	"Search:",				NULL, NULL},
    {LAT_FID,	"Latitude (+ north) (d:m:s): ",		"Latitude:", "Lat"},
    {LONG_FID,	"Longitude (+ west) (d:m:s): ",		"Longitude:","Long"},
    {ELEV_FID,	"dummy",				"Elevation:","Elev",
	{   "Elevation above sea level (ft): ",
	    "Elevation above sea level (m): "
	}
    },
    {TEMP_FID,	"dummy",			       "Temperature:","Temp",
	{   "Temperature (degrees F): ",
	    "Temperature (degrees C): "
	}
    },
    {PRES_FID,	"dummy",				"Atm Pres:","AtmPr",
	{   "Atmospheric pressure (inches of Mercury):",
	    "Atmospheric pressure (mBar):"
	}
    },
    {EPOCH_FID,	"Epoch (decimal year): ",		"Epoch:", NULL},
};

#define	NFM	XtNumber(mm_field_map)
#define	LFM	(&mm_field_map[NFM])

static Widget newcir_w;
static Widget status_w;
static Widget go_w;

/* this is the list of keywords that allow some initial values to be set.
 * these are resources to the XEphem class as "XEphem.<keyword>: <value>".
 */
/* N.B. index of item is its switch/case value in crack_fieldset ()
 */
static char *keywords[] = {
    /*  0 */	"TZName",	/* these should be first so others may use */
    /*  1 */	"TZone",
    /*  2 */	"Epoch",
    /*  3 */	"Elevation",
    /*  4 */	"JD",
    /*  5 */	"Lat",
    /*  6 */	"Long",
    /*  7 */	"Sitename",
    /*  8 */	"NSteps",
    /*  9 */	"Pause",
    /* 10 */	"Pressure",
    /* 11 */	"StepSize",
    /* 12 */	"Temp",
    /* 13 */	"UD",
    /* 14 */	"UT",
    /* 15 */	"TwilightDip",
    /* 16 */	"LD",
    /* 17 */	"LT",
    /* 18 */	"LST",
};

/* stuff to manage the step control facility.
 * we support a variety of ways time can be incremented.
 * some are fixed but some are variable.
 * the fixed ones set the time change amount in tminc (in hours).
 * the variable ones compute it each iteration and don't use tminc.
 */
enum StepCodes {
    FIXED_SS, NEXTDAWN_SS, NEXTDUSK_SS, NEXTSUNRISE_SS, NEXTSUNSET_SS,
    NEXTFULLMOON_SS, NEXTNEWMOON_SS, RTC_SS
};

typedef struct {
    char *title;	/* name of this step option as a string */
    int stepcode;	/* one of the StepCodes enum codes */
    double inc;		/* if code is FIXED_SS: fixed time step, in hours */
} StepOption;
/* N.B. leave RT Clock first!! see the RTC_SS_OPTION macro */
static StepOption step_options[] = {
    {"RT Clock",	RTC_SS},
    {"24:00:00",	FIXED_SS,	24.0},
    {"1:00:00",		FIXED_SS,	1.0},
    {"0:05:00",		FIXED_SS,	5./60.},
    {"Next Dawn",	NEXTDAWN_SS},
    {"Next Dusk",	NEXTDUSK_SS},
    {"Next Sun rise",	NEXTSUNRISE_SS},
    {"Next Sun set",	NEXTSUNSET_SS},
    {"Next Full moon",	NEXTFULLMOON_SS},
    {"Next New moon",	NEXTNEWMOON_SS},
    {"Sidereal Day",	FIXED_SS,	24.0 * SIDRATE},
    {"Sidereal Month",	FIXED_SS,	27.3215 * 24.0},
};

/* make a way to refer to the RT clock entry; needed in order to implement
 * remaining compatable with putting "RTC" in resource files for the RT option.
 */
#define	RTC_SS_OPTION	(&step_options[0])

/* current step_option.
 * how we init it here determines is the default value.
 * if this is 0 it means just use tminc, period.
 */
static StepOption *mm_step_option = RTC_SS_OPTION;

/* sitename, or NULL if none.
 * N.B. this is a pointer into the real sitesp[] array. this is ok because the
 *   current implementation never rereads the sites and hence never changes
 *   the array. if that ever changes, this should become a malloced copy.
 */
static char *msitename;

/* called exactly once when the main form is made.
 * create and manage all the widgets as children of the form_w.
 */
void
mm_manage (main_window)
Widget main_window;
{
	create_main_form(main_window);
	mm_reset();

	/* if time/date still not set, set it to Now */
	if (mjd == XMJD) {
	    xe_msg ("No time resources set -- defaulting to \"Now\".", 0);
	    (void) chg_fld ("Now", &mm_field_map[UD_FID]);
	    redraw_screen (1);
	}

	/* load the initial set of objects and set checkpoint there. */
	db_loadinitial();
	db_set_cp();
}

/* reset things to their initial resource settings */
void
mm_reset()
{
	read_default_resources ();
	redraw_screen (1);
	mm_set_buttons (mm_selecting);
	print_idle();
}

/* called by the calendar subsystem to set a new UT date.
 * newmjd is the new UT date/time as a modifed Julian date.
 */
void
mm_newcaldate (newmjd)
double newmjd;
{
	mjd = newmjd;
	set_t0 (&now);
	if (autotz)
	    (void) tz_fromsys (&now);
	mm_now (1);
	mm_newcir(1);
	newcir = 1;
}

/* called by other menus as they want to hear from our buttons or not.
 * the "on"s and "off"s stack - only really redo the buttons if it's the
 * first on or the last off.
 */
void
mm_selection_mode (whether)
int whether;	/* whether setting up for plotting or for not plotting */
{
	mm_selecting += whether ? 1 : -1;

	if ((whether && mm_selecting == 1)     /* first one to want on */
	    || (!whether && mm_selecting == 0) /* last one to want off */)
	    mm_set_buttons (whether);
}

/* called to set new site information in Now.
 * if update we do a full update, else just set NEW CICUMSTANCES.
 * N.B. we assume sname points to stable long-term memory.
 */
void
mm_setsite (slat, slng, selev, sname, update)
double slat;
double slng;
double selev;
char *sname;
int update;
{
	/* update loc and evelation fields of Now */
	lat = slat;
	lng = slng;
	elev = selev/ERAD;	/* m to earth radii */

	/* also save the site name */
	msitename = sname;	/* NULL is OK */

	if (update)
	    redraw_screen(1);
	else {
	    mm_now (1);
	    mm_newcir(1);
	    newcir = 1;
	}
}

/* give the caller the current site name.
 * N.B. return NULL if no current site.
 * N.B. caller should not change anything.
 */
char *
mm_getsite ()
{
	return (msitename);
}

/* a way for anyone to know what now is */
Now *
mm_get_now()
{
	return (&now);
}

/* called to start or stop a movie sequence.
 * if a movie is currently running, we stop it, period.
 * then if stepsz != 0 we start one by set movie=1, spause=0, nstep=1000000,
 *   tm_inc=stepsz and do about what the Go button does.
 */
void
mm_movie (stepsz)
double stepsz;	/* step size, hours */
{
	/* stop any current movie */
	if (movie) {
	    if (mm_interval_id != 0) {
		XtRemoveTimeOut (mm_interval_id);
		mm_interval_id = 0;
	    }
	    nstep = 1;
	    redraw_screen (1);
	    print_idle();
	    movie = 0;
	    return;
	}

	/* that's it if zero step size */
	if (stepsz == 0.0)
	    return;

	/* note movie is about to be running */
	movie = 1;

	/* init nstep and spause to reasonable for long fast looping */
	nstep = 1000000;
	print_nstep(1);
	spause = 0;
	print_spause(1);

	/* set the time increment to stepsz */
	mm_step_option = NULL;	/* no special increment */
	tminc = stepsz;
	print_tminc(1);
	print_running();

	/* start fresh looping */
	mm_go();
}

/* draw all the stuff on the managed menus.
 * if how_much == 0 then just update fields that need it;
 * if how_much == 1 then redraw all fields;
 */
void
redraw_screen (how_much)
int how_much;
{
	watch_cursor(1);

	/* invalidate any cached values in the database */
	db_invalidate();

	/* print the single-step message if this is the last loop */
	if (nstep < 1)
	    print_updating();

	/* if just updating changed fields while plotting or listing
	 * unattended then suppress most screen updates except
	 * always show nstep to show plot loops to go and
	 * always show tminc to show search convergence progress.
	 */
	print_nstep(how_much);
	print_tminc(how_much);
	print_spause(how_much);
	if (how_much == 0 && nstep > 0 && spause == 0)
	    f_off();

	/* print all the time-related fields */
	mm_now (how_much);

	mm_twilight (how_much);

	/* print stuff on other menus */
	all_update(&now, how_much);

	/* everything is up to date now */
	newcir = 0;
	mm_newcir(0);

	f_on();

	watch_cursor(0);
}

static void
create_main_form(main_window)
Widget main_window;
{
	static int fmi[4] = {0, 8, 16, NFM};
	Widget mrc_w, hrc_w;
	Widget sep_w;
	Widget w;
	Arg args[20];
	int i, n;

	/* create the main vertical r/c */
	n = 0;
	XtSetArg (args[n], XmNisAligned, False); n++;
	mrc_w = XmCreateRowColumn (main_window, "MainRC", args, n);

	/* make the status label */
	n = 0;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	status_w = XmCreateLabel (mrc_w, "Status", args, n);
	XtManageChild (status_w);

	/* make a separator widget */
	n = 0;
	sep_w = XmCreateSeparator (mrc_w, "HSep2", args, n);
	XtManageChild (sep_w);

	/* make the "NEW CIRCUMSTANCES" label */
	n = 0;
	newcir_w = XmCreateLabel (mrc_w, "NewCir", args, n);
	set_xmstring (newcir_w, XmNlabelString, " ");
	XtManageChild(newcir_w);

	/* make a separator widget */
	n = 0;
	sep_w = XmCreateSeparator (mrc_w, "HSep3", args, n);
	XtManageChild (sep_w);

	/* make a horizontal r/c across the bottom.
	 * fill it in with three vertical r/cs and the calendar, with
	 * separators.
	 */

	n = 0;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
	hrc_w = XmCreateRowColumn (mrc_w, "BottomRC", args, n);
	XtManageChild (hrc_w);

	for (i = 0; i < XtNumber(fmi)-1; i++) {
	    Widget vrc_w;
	    int f;

	    n = 0;
	    XtSetArg (args[n], XmNisAligned, False); n++;
	    XtSetArg (args[n], XmNadjustLast, False); n++;
	    XtSetArg (args[n], XmNspacing, 3); n++;
	    vrc_w = XmCreateRowColumn (hrc_w, "MainRC", args, n);
	    XtManageChild (vrc_w);

	    for (f = fmi[i]; f < fmi[i+1]; f++) {
		Field *fp = &mm_field_map[f];
		Widget f_w;

		/* if GAP just mark a separator in the vrc */
		if (fp->id == GAP) {
		    n = 0;
		    w = XmCreateSeparator (vrc_w, "GapSep", args, n);
		    XtManageChild (w);
		    n = 0;
		    w = XmCreateLabel (vrc_w, "GL", args, n);
		    set_xmstring (w, XmNlabelString, " ");
		    XtManageChild (w);
		    continue;
		}

		/* if no label, center a PB directly in the vrc */
		if (!fp->label) {
		    n = 0;
		    XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
		    if (fp->name) {
			XtSetArg (args[n], XmNuserData, fp->name); n++;
		    };
		    fp->pb_w = XmCreatePushButton (vrc_w, "MainPB", args, n);
		    if (fp->prompt || fp->name)
			XtAddCallback (fp->pb_w, XmNactivateCallback,
						mm_activate_cb, (XtPointer)fp);
		    XtManageChild(fp->pb_w);
		    continue;
		}

		/* we have both a label and PB -- put them in their own form */
		n = 0;
		f_w = XmCreateForm (vrc_w, "MainHF", args, n);
		XtManageChild (f_w);

		/* make the pushbutton on the right side */
		n = 0;
		XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNalignment, XmALIGNMENT_END); n++;
		if (fp->name) {
		    XtSetArg (args[n], XmNuserData, fp->name); n++;
		};
		fp->pb_w = XmCreatePushButton (f_w, "MainPB", args, n);
		if (fp->prompt || fp->name)
		    XtAddCallback (fp->pb_w, XmNactivateCallback,
						mm_activate_cb, (XtPointer)fp);
		XtManageChild(fp->pb_w);

		/* make the label on the left side */
		n = 0;
		XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
		XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
		XtSetArg (args[n], XmNrightWidget, fp->pb_w); n++;
		XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
		w = XmCreateLabel (f_w, "MainLabel", args, n);
		XtManageChild(w);
		set_xmstring (w, XmNlabelString, fp->label);
	    }

	    /* make a vertical separator widget */
	    n = 0;
	    XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
	    sep_w = XmCreateSeparator (hrc_w, "VSep", args, n);
	    XtManageChild (sep_w);
	}

	/* make the calendar */

	w = calm_create (hrc_w);
	XtManageChild (w);

	/* make a separator widget */
	n = 0;
	sep_w = XmCreateSeparator (mrc_w, "HSep4", args, n);
	XtManageChild (sep_w);

	/* put a wide and thick "go" button below all */
	n = 0;
	XtSetArg (args[n], XmNshowAsDefault, True); n++;
	XtSetArg (args[n], XmNmarginTop, 3); n++;
	XtSetArg (args[n], XmNmarginBottom, 3); n++;
	go_w = XmCreatePushButton (mrc_w, "Update", args, n);
	XtAddCallback (go_w, XmNactivateCallback, mm_go_cb, 0);
	XtManageChild (go_w);

	XtManageChild (mrc_w);
}

/* set the initial stuff in the Now struct and looping params from the XEphem
 * resources.
 */
static void
read_default_resources()
{
	int i;

	for (i = 0; i < XtNumber(keywords); i++) {
	    char *dp = XGetDefault (XtD, myclass, keywords[i]);
	    if (dp) {
		char buf[128];
		(void) sprintf (buf, "%s=%s", keywords[i], dp);
		if (crack_fieldset (buf) < 0) {
		    printf ("Bad resource spec: %s", buf);
		    exit (1);
		}
	    }
	}
}

/* get the widget associated with this _FID.
 * We do some error checking.
 */
static Widget
fw(fid)
int fid;
{
	Field *fp;

	if (fid < 0 || fid >= NFM || (fp = &mm_field_map[fid])->id != fid) {
	    printf ("mainmenu:fw(): bad field id: %d\n", fid);
	    exit(1);
	}
	return (fp->pb_w);
}

/* go through all the buttons just pickable for plotting and set whether they
 * should appear to look like buttons or just flat labels.
 */
static void
mm_set_buttons (whether)
int whether;
{
	static Arg look_like_button[] = {
	    {XmNtopShadowColor, (XtArgVal) 0},
	    {XmNbottomShadowColor, (XtArgVal) 0},
	    {XmNtopShadowPixmap, (XtArgVal) 0},
	    {XmNbottomShadowPixmap, (XtArgVal) 0},
	    {XmNfillOnArm, (XtArgVal) True},
	    {XmNtraversalOn, (XtArgVal) True},
	};
	static Arg look_like_label[] = {
	    {XmNtopShadowColor, (XtArgVal) 0},
	    {XmNbottomShadowColor, (XtArgVal) 0},
	    {XmNtopShadowPixmap, (XtArgVal) 0},
	    {XmNbottomShadowPixmap, (XtArgVal) 0},
	    {XmNfillOnArm, (XtArgVal) False},
	    {XmNtraversalOn, (XtArgVal) False},
	};
	static int called;
	Field *fp;

	if (!called) {
	    /* get baseline label and shadow appearances.
	     * arbitrarily get these from the go_w button.
	     */
	    Pixel topshadcol, botshadcol, bgcol;
	    Pixmap topshadpm, botshadpm;
	    Arg args[20];
	    int n;

	    n = 0;
	    XtSetArg (args[n], XmNtopShadowColor, &topshadcol); n++;
	    XtSetArg (args[n], XmNbottomShadowColor, &botshadcol); n++;
	    XtSetArg (args[n], XmNtopShadowPixmap, &topshadpm); n++;
	    XtSetArg (args[n], XmNbottomShadowPixmap, &botshadpm); n++;
	    XtSetArg (args[n], XmNbackground, &bgcol); n++;
	    XtGetValues (go_w, args, n);

	    look_like_button[0].value = topshadcol;
	    look_like_button[1].value = botshadcol;
	    look_like_button[2].value = topshadpm;
	    look_like_button[3].value = botshadpm;
	    look_like_label[0].value = bgcol;
	    look_like_label[1].value = bgcol;
	    look_like_label[2].value = XmUNSPECIFIED_PIXMAP;
	    look_like_label[3].value = XmUNSPECIFIED_PIXMAP;

	    called = 1;
	}

	for (fp = mm_field_map; fp < LFM; fp++) {
	    if (fp->id == GAP)
		continue;	/* no pb_w */
	    if (whether) {
	      if (fp->name)
	        XtSetValues(fp->pb_w, look_like_button,
						    XtNumber(look_like_button));
	      else
	        XtSetValues(fp->pb_w, look_like_label,
						    XtNumber(look_like_label));
	    } else {
	      if (fp->prompt)
	        XtSetValues(fp->pb_w, look_like_button,
						    XtNumber(look_like_button));
	      else
	        XtSetValues(fp->pb_w, look_like_label,
						    XtNumber(look_like_label));
	    }
	}
}

/* callback from any of the main menu buttons being activated.
 * if we are currently selecting fields to plot and the field has a name
 *   then inform all the potentially interested parties;
 * else if the field has a prompt then ask the user for a new value.
 */
/* ARGSUSED */
static void
mm_activate_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	Field *fp = (Field *)client;

	if (mm_selecting) {
	    if (fp->name)
		register_selection (fp->name);
	} else {
	    if (fp->prompt) {
		mm_set_alt_prompts (fp);
		prompt (fp);
	    }
	}
}

/* set up those prompts that use the alternates.
 */
static void
mm_set_alt_prompts(fp)
Field *fp;
{
	switch (fp->id) {
	case UD_FID:
	case LD_FID:
	    fp->prompt = fp->altp[pref_get(PREF_DATE_FORMAT)];
	    break;
	case ELEV_FID:
	case TEMP_FID:
	case PRES_FID:
	    fp->prompt = fp->altp[pref_get(PREF_UNITS)];
	    break;
	case TZN_FID:
	case TZONE_FID:
	    fp->prompt = fp->altp[autotz];
	    break;
	}
}

/* function called from the interval timer used to implement the
 * auto repeat feature.
 */
/* ARGSUSED */
static void
mm_timer_cb (client, id)
XtPointer client;
XtIntervalId *id;
{
	mm_go();
}

/* callback from the main "go" button being armed.
 * if we are looping (as evidenced by an active timer) then stop, else go.
 */
/* ARGSUSED */
static void
mm_go_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (mm_interval_id != 0) {
	    XtRemoveTimeOut (mm_interval_id);
	    mm_interval_id = 0;
	    redraw_screen (1);
	    print_idle();
	    movie = 0;	/* no movie running now either :-) */
	} else {
	    if (nstep > 1)
		print_running();
	    mm_go();
	}
}

/* called when any of the canned step size buttons is activated.
 * client contains the StepOption pointer; use it to set mm_step_option and,
 * if it's a fixed value, tminc.
 * when finished, unmanage the dialog (which is the parent of our parent).
 */
/* ARGSUSED */
static void
mm_step_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	StepOption *sop = (StepOption *)client;

	mm_step_option = sop;
	if (sop->stepcode == FIXED_SS)
	    tminc = sop->inc;
	else
	    tminc = 0.0;	/* just to reset things for the next time */
	set_t0 (&now);
	print_tminc (1);

	XtUnmanageChild (XtParent(XtParent(w)));
}

/* increment, update all fields, and go again if more steps.
 */
static void
mm_go()
{
	int srchdone;

	/* increment time only if op didn't change cirumstances */
	if (!newcir)
	    mm_new_now();

	nstep -= 1;

	/* recalculate everything and update all the fields */
	redraw_screen(newcir);

	/* let searching functions change tminc and check for done */
	srchdone = srch_eval (mjd, &tminc) < 0;
	print_tminc(0); /* to show possibly new search increment */

	/* update plot and listing files, now that all fields are up
	 * to date and search function has been evaluated.
	 */
	plot();
	listing();

	/* stop loop to allow op to change parameters:
	 * if a search evaluation converges (or errors out),
	 * or if steps are done,
	 * or if op hits any key.
	 */
	newcir = 0;
	if (srchdone || nstep <= 0) {

	    /* update screen with the current stuff if stopped during
	     * searching, unattended plotting or listing since last
	     * redraw_screen() didn't.
	     */
	    if ((srchdone || plot_ison() || listing_ison()) && nstep > 0)
		redraw_screen (1);

	    /* return nstep to default of 1 */
	    if (nstep <= 0) {
		nstep = 1;
		print_nstep (0);
	    }
	    print_idle();
	} else {
	    int ms = spause < NO_PAUSE_DELAY ? NO_PAUSE_DELAY : spause;
	    mm_interval_id = XtAppAddTimeOut (xe_app, ms, mm_timer_cb, 0);
	}

	XSync (XtD, False);
}

/* set the time (in now.n_mjd) according to the desired step increment 
 * as coded in mm_step_option and possibly tminc.
 */
static void
mm_new_now()
{
#define	BADRS (RS_NORISE|RS_RISERR|RS_NOSET|RS_CIRCUMPOLAR|RS_NEVERUP)
	int stepcode = mm_step_option ? mm_step_option->stepcode : FIXED_SS;

	switch (stepcode) {
	case NEXTDAWN_SS: {
	    double dusk, dawn;
	    int status;

	    twilight_cir (&now, dip, &dawn, &dusk, &status);
	    if (!(status & BADRS)) {
		if (mjd_hr (mjd - tz/24.0) < dawn - 1./60.) {
		    /* next dawn is still today so just change to it */
		    mjd = mjd_day(mjd - tz/24.0) + (dawn + tz)/24.0;
		} else {
		    /* already later than today's dawn so use tomorrow's */
		    Now n;
		    n = now;
		    n.n_mjd += 1.0;
		    twilight_cir (&n, dip, &dawn, &dusk, &status);
		    if (!(status & BADRS))
			mjd = mjd_day(n.n_mjd - tz/24.0) + (dawn + tz)/24.0;
		    else {
			xe_msg ("No dawn tomorrow.", 1);
			nstep = 0;
		    }
		}
	    } else {
		xe_msg ("No dawn today.", 1);
		nstep = 0;
	    }
	    }
	    break;

	case NEXTDUSK_SS: {
	    double dusk, dawn;
	    int status;

	    twilight_cir (&now, dip, &dawn, &dusk, &status);
	    if (!(status & BADRS)) {
		if (mjd_hr (mjd - tz/24.0) < dusk - 1./60.) {
		    /* next dusk is still today so just change to it */
		    mjd = mjd_day(mjd - tz/24.0) + (dusk + tz)/24.0;
		} else {
		    /* already later than today's dusk so use tomorrow's */
		    Now n;
		    n = now;
		    n.n_mjd += 1.0;
		    twilight_cir (&n, dip, &dawn, &dusk, &status);
		    if (!(status & BADRS))
			mjd = mjd_day(n.n_mjd - tz/24.0) + (dusk + tz)/24.0;
		    else {
			xe_msg ("No dusk tomorrow.", 1);
			nstep = 0;
		    }
		}
	    } else {
		xe_msg ("No dusk today.", 1);
		nstep = 0;
	    }
	    }
	    break;

	case NEXTSUNRISE_SS: {
	    RiseSet rs;
	    Obj *op = db_basic(SUN);

	    dm_riset (&now, op, &rs);
	    if (!(rs.rs_flags & BADRS)) {
		if (mjd_hr (mjd - tz/24.0) < rs.rs_risetm - 1./60.) {
		    /* next sunrise is still today so just change to it */
		    mjd = mjd_day(mjd - tz/24.0) + (rs.rs_risetm + tz)/24.0;
		} else {
		    /* already later than today's sunrise so use tomorrow's */
		    Now n;
		    n = now;
		    n.n_mjd += 1.0;
		    dm_riset (&n, op, &rs);
		    if (!(rs.rs_flags & BADRS))
			mjd = mjd_day(n.n_mjd - tz/24.0) +
						    (rs.rs_risetm + tz)/24.0;
		    else {
			xe_msg ("No sunrise tomorrow.", 1);
			nstep = 0;
		    }
		}
	    } else {
		xe_msg ("No sunrise today.", 1);
		nstep = 0;
	    }
	    }
	    break;

	case NEXTSUNSET_SS: {
	    RiseSet rs;
	    Obj *op = db_basic(SUN);

	    dm_riset (&now, op, &rs);
	    if (!(rs.rs_flags & BADRS)) {
		if (mjd_hr (mjd - tz/24.0) < rs.rs_settm - 1./60.) {
		    /* next sunset is still today so just change to it */
		    mjd = mjd_day(mjd - tz/24.0) + (rs.rs_settm + tz)/24.0;
		} else {
		    /* already later than today's sunset so use tomorrow's */
		    Now n;
		    n = now;
		    n.n_mjd += 1.0;
		    dm_riset (&n, op, &rs);
		    if (!(rs.rs_flags & BADRS))
			mjd = mjd_day(n.n_mjd - tz/24.0) +
						    (rs.rs_settm + tz)/24.0;
		    else {
			xe_msg ("No sunset tomorrow.", 1);
			nstep = 0;
		    }
		}
	    } else {
		xe_msg ("No sunset today.", 1);
		nstep = 0;
	    }
	    }
	    break;

	case NEXTFULLMOON_SS: {
	    double M, mjdn, mjdf;

	    /* start over a month back and search for next full moon */
	    M = mjd - 32.0;
	    do {
		M += 1.0;
		moonnf (M, &mjdn, &mjdf);
	    } while (mjdf <= mjd);
	    mjd = mjdf;
	    }
	    break;

	case NEXTNEWMOON_SS: {
	    double M, mjdn, mjdf;

	    /* start over a month back and search for next new moon */
	    M = mjd - 32.0;
	    do {
		M += 1.0;
		moonnf (M, &mjdn, &mjdf);
	    } while (mjdn <= mjd);
	    mjd = mjdn;
	    }
	    break;

	/* FIXED_SS step options just use tminc */
	case FIXED_SS:
	    inc_mjd (&now, tminc, 0);
	    break;

	/* increment to the current time */
	case RTC_SS:
	    inc_mjd (&now, tminc, 1);
	    break;

	default:
	    printf ("mm_new_now(): bogus stepcode: %d\n", stepcode);
	    exit (1);
	}

	if (autotz)
	    (void) tz_fromsys (&now);
}

/* ARGSUSED */
static void
print_tminc(force)
int force;
{
	Widget stpsz_w = fw(STPSZ_FID);
	int stepcode = mm_step_option ? mm_step_option->stepcode : FIXED_SS;

	switch (stepcode) {

	/* step options that are variable and identified by a name */
	case NEXTDAWN_SS:	/* same as .. */
	case NEXTDUSK_SS:	/* same as .. */
	case NEXTSUNRISE_SS:	/* same as .. */
	case NEXTSUNSET_SS:	/* same as .. */
	case NEXTFULLMOON_SS:	/* same as .. */
	case NEXTNEWMOON_SS:	/* same as .. */
	case RTC_SS:
	    f_string (stpsz_w, mm_step_option->title);
	    break;

	/* FIXED_SS step options just use tminc */
	case FIXED_SS:
	    if (fabs(tminc) >= 24.0)
		f_double (stpsz_w, "%6.4g dy", tminc/24.0);
	    else
		f_sexa (stpsz_w, tminc, 3, 3600);
	    break;

	default:
	    printf ("print_tminc(): bogus stepcode: %d\n", stepcode);
	    exit (1);
	}
}

static void
print_updating()
{
	print_status ("Updating...");
}

static void
print_idle()
{
	print_status ("Make changes as desired or select Update to step.");
	f_string (go_w, "Update");
}

static void
print_running()
{
	print_status ("Running... select Stop to stop.");
	f_string (go_w, "Stop");
}

static void
print_status (s)
char *s;
{
	static char *last_s;

	if (s != last_s) {
	    f_string (status_w, s);
	    XSync (XtD, False);
	    last_s = s;
	}
}

static void
print_nstep(force)
int force;
{
	static int last;

	if (force || nstep != last) {
	    char buf[16];
	    (void) sprintf (buf, "%7d", nstep);
	    f_string (fw(NSTEP_FID), buf);
	    last = nstep;
	}
}

static void
print_spause(force)
int force;
{
	static int last;

	if (force || spause != last) {
	    char buf[16];
	    (void) sprintf (buf, "%7g", spause/1000.0);
	    f_string (fw(PAUSE_FID), buf);
	    last = spause;
	}
}

/* process a field spec in buf, either from config file or argv.
 * return 0 if recognized ok, else -1.
 */
static int
crack_fieldset (buf)
char *buf;
{
	int i;

	for (i = 0; i < XtNumber(keywords); i++) {
	    int l = strlen(keywords[i]);
	    if (strncmp (buf, keywords[i], l) == 0) {
		buf += l+1;	/* skip keyword and its subsequent delimiter */
		break;
	    }
	}

	/* N.B. the switch entries must match the order in keywords[] */
	switch (i) {
	case 0:  (void) chg_fld (buf, &mm_field_map[TZN_FID]); break;
	case 1:  (void) chg_fld (buf, &mm_field_map[TZONE_FID]); break;
	case 2:  (void) chg_fld (buf, &mm_field_map[EPOCH_FID]); break;
	case 3:  (void) chg_fld (buf, &mm_field_map[ELEV_FID]); break;
	case 4:  (void) chg_fld (buf, &mm_field_map[JD_FID]); break;
	case 5:  (void) chg_fld (buf, &mm_field_map[LAT_FID]); break;
	case 6:  (void) chg_fld (buf, &mm_field_map[LONG_FID]); break;
	case 7:  (void) chg_fld (buf, &mm_field_map[SITE_FID]); break;
	case 8:  (void) chg_fld (buf, &mm_field_map[NSTEP_FID]); break;
	case 9:  (void) chg_fld (buf, &mm_field_map[PAUSE_FID]); break;
	case 10:  (void) chg_fld (buf, &mm_field_map[PRES_FID]); break;
	case 11: (void) chg_fld (buf, &mm_field_map[STPSZ_FID]); break;
	case 12: (void) chg_fld (buf, &mm_field_map[TEMP_FID]); break;
	case 13: (void) chg_fld (buf, &mm_field_map[UD_FID]); break;
	case 14: (void) chg_fld (buf, &mm_field_map[UT_FID]); break;
	case 15: (void) chg_fld (buf, &mm_field_map[DIP_FID]); break;
	case 16: (void) chg_fld (buf, &mm_field_map[LD_FID]); break;
	case 17: (void) chg_fld (buf, &mm_field_map[LT_FID]); break;
	case 18: (void) chg_fld (buf, &mm_field_map[LST_FID]); break;
	default: return (-1);
	}
	return (0);
}

/* react to the field at *fp according to the string input at bp.
 * crack the buffer and update the corresponding (global) variable(s)
 * or do whatever a pick at that field should do.
 * return 1 if we change a field that invalidates any of the times or
 * to update all related fields.
 */
static int
chg_fld (bp, fp)
char *bp;
Field *fp;
{
	int new = 0;
	double tmp;

	switch (fp->id) {
	case JD_FID:
	    if (bp[0] == 'n' || bp[0] == 'N') {
		time_fromsys (&now);
		autotz = 1;
	    } else
		mjd = atod(bp) - MJD0;
	    set_t0 (&now);
	    new = 1;
	    break;
	case UD_FID:
	    if (bp[0] == 'n' || bp[0] == 'N') {
		time_fromsys (&now);
		autotz = 1;
	    } else {
		double day, newmjd0;
		int month, year;

		mjd_cal (mjd, &month, &day, &year); /* init with now */
		f_sscandate (bp, pref_get(PREF_DATE_FORMAT),
						    &month, &day, &year);
		cal_mjd (month, day, year, &newmjd0);

		/* if don't give a fractional part to days
		 * then retain current hours.
		 */
		if ((long)day == day)
		    mjd = newmjd0 + mjd_hr(mjd)/24.0;
		else
		    mjd = newmjd0;
	    }
	    set_t0 (&now);
	    new = 1;
	    break;
	case UT_FID:
	    if (bp[0] == 'n' || bp[0] == 'N') {
		time_fromsys (&now);
		autotz = 1;
	    } else {
		double newutc = (mjd-mjd_day(mjd)) * 24.0;
		rnd_second (&newutc);	/* match display precision */
		f_scansex (newutc, bp, &newutc);
		mjd = mjd_day(mjd) + newutc/24.0;
	    }
	    set_t0 (&now);
	    new = 1;
	    break;
	case LD_FID:
	    if (bp[0] == 'n' || bp[0] == 'N') {
		time_fromsys (&now);
		autotz = 1;
	    } else {
		double day, newlmjd0;
		int month, year;

		mjd_cal (mjd-tz/24.0, &month, &day, &year); /* now */
		f_sscandate (bp, pref_get(PREF_DATE_FORMAT),
						    &month, &day, &year);
		cal_mjd (month, day, year, &newlmjd0);

		/* if don't give a fractional part to days
		 * then retain current hours.
		 */
		if ((long)day == day)
		    mjd = newlmjd0 + mjd_hr(mjd-tz/24.0)/24.0;
		else
		    mjd = newlmjd0;
		mjd += tz/24.0;
	    }
	    set_t0 (&now);
	    new = 1;
	    break;
	case LT_FID:
	    if (bp[0] == 'n' || bp[0] == 'N') {
		time_fromsys (&now);
		autotz = 1;
	    } else if (bp[0] == 'M' || bp[0] == 'm') {
		/* midnight, tonight */
		mjd = mjd_day(mjd+1-tz/24.0) + tz/24.0;
	    } else {
		double newlt = (mjd-mjd_day(mjd)) * 24.0 - tz;
		range (&newlt, 24.0);
		rnd_second (&newlt);	/* match display precision */
		f_scansex (newlt, bp, &newlt);
		mjd = mjd_day(mjd-tz/24.0) + (newlt + tz)/24.0;
	    }
	    set_t0 (&now);
	    new = 1;
	    break;
	case LST_FID:
	    if (bp[0] == 'n' || bp[0] == 'N') {
		time_fromsys (&now);
		autotz = 1;
	    } else {
		double lst, utc;
		now_lst (&now, &lst);
		rnd_second (&lst);	/* match display precision */
		f_scansex (lst, bp, &lst);
		lst -= radhr(lng); /* convert to gst */
		range (&lst, 24.0);
		gst_utc (mjd_day(mjd), lst, &utc);
		mjd = mjd_day(mjd) + utc/24.0;
	    }
	    set_t0 (&now);
	    new = 1;
	    break;

	case TZN_FID:
	case TZONE_FID:
	    if (strncmp (bp, AutoOn, strlen(AutoOn)) == 0) {
		if (tz_fromsys (&now) < 0) {
		    xe_msg ("Can not get timezone info from OS", 0);
		    autotz = 0;
		} else {
		    autotz = 1;
		    new = 1;
		}
	    } else if (strncmp (bp, AutoOff, strlen(AutoOff)) == 0) {
		autotz = 0;
	    } else {
		if (fp->id == TZN_FID)
		    (void) strncpy (tznm, bp, sizeof(tznm)-1);
		else {
		    /* TZONE_FID */
		    rnd_second (&tz);	/* match display precision */
		    f_scansex (tz, bp, &tz);
		}
		autotz = 0;
		new = 1;
	    }
	    break;

	case SITE_FID: {
	    int pos = sites_search (0, bp);
	    if (pos >= 0) {
		Site **sipp, *sip;
		(void) sites_get_list (&sipp);
		sip = sipp[pos];
		mm_setsite (sip->si_lat, sip->si_lng,sip->si_elev,sip->si_name);
		new = 1;
	    } else {
		char buf[1024];
		sprintf (buf, "Site not found: %s", bp);
		xe_msg (buf, 0);
	    }
	    }
	    break;
	case LONG_FID:
	    tmp = -raddeg(lng);
	    rnd_second (&tmp);		/* match display precision */
	    f_scansex (tmp, bp, &tmp);
	    lng = degrad (-tmp); 	/* want <0 radians west */
	    msitename = NULL;
	    new = 1;
	    break;
	case LAT_FID:
	    tmp = raddeg(lat);
	    rnd_second (&tmp);		/* match display precision */
	    f_scansex (tmp, bp, &tmp);
	    lat = degrad (tmp);
	    msitename = NULL;
	    new = 1;
	    break;
	case ELEV_FID:
	    if (sscanf (bp, "%lf", &elev) == 1) {
		if (pref_get(PREF_UNITS) == PREF_ENGLISH)
		    elev /= (ERAD*FTPM);	/* ft to earth radii */
		else
		    elev /= ERAD;		/* m to earth radii */
		new = 1;
	    }
	    break;
	case DIP_FID:
	    if (sscanf (bp, "%lf", &tmp) == 1) {
		if (tmp < 0)
		    xe_msg ("Twilight dip must be at least 0", 1);
		else {
		    dip = degrad(tmp);
		    mm_twilight (1);
		}
	    }
	    break;
	case NSTEP_FID:
	    (void) sscanf (bp, "%d", &nstep);
	    print_nstep (0);
	    break;
	case PAUSE_FID:
	    (void) sscanf (bp, "%lf", &tmp);
	    spause = tmp*1000 + 0.5;
	    print_spause (0);
	    break;
	case TEMP_FID:
	    if (sscanf (bp, "%lf", &temp) == 1) {
		if (pref_get(PREF_UNITS) == PREF_ENGLISH)
		    temp = 5./9.*(temp - 32.0);	/* want degs C */
		if (temp < -100 || temp > 100) {
		    char msg[128];
		    (void) sprintf (msg,
				"NOTICE: very unusual temperature: %g C", temp);
		    xe_msg (msg, 0);
		}
		new = 1;
	    }
	    break;
	case PRES_FID:
	    if (sscanf (bp, "%lf", &pressure) == 1) {
		if (pref_get(PREF_UNITS) == PREF_ENGLISH) 
		    pressure *= 33.86;		/* want mBar */
		if (pressure < 0 || pressure > 4000) {
		    char msg[128];
		    (void) sprintf (msg,
			    "NOTICE: very unusual atmospheric pressure: %g mB",
								    pressure);
		    xe_msg (msg, 0);
		}
		new = 1;
	    }
	    break;
	case EPOCH_FID:
	    if (bp[0] == 'e' || bp[0] == 'E')
		epoch = EOD;
	    else {
		double e;
		e = atod(bp);
		year_mjd (e, &epoch);
	    }
	    new = 1;
	    break;
	case STPSZ_FID:
	    mm_set_step_code (bp);
	    set_t0 (&now);
	    print_tminc(0);
	    break;
	default:
	    printf ("chg_fld: unknown id: %d\n", fp->id);
	    exit (1);
	}

	return (new);
}

/* user selected OK or APPLY to a prompt for field at fp (in userData).
 * get his new value and use it.
 */
/* ARGSUSED */
static void
prompt_ok_cb (w, client, call)
Widget w;	/* PromptDialog "widget" */
XtPointer client;
XtPointer call;
{
	XmSelectionBoxCallbackStruct *s = (XmSelectionBoxCallbackStruct *)call;
	Field *fp;
	char *text;
	Widget apply_w;
	
	switch (s->reason) {
	case XmCR_OK:
	    /* new value is in the text string */
	    get_xmstring(w, XmNtextString, &text);
	    break;
	case XmCR_APPLY:	/* used for several special short cuts */
	    /* new value is in the Apply button text string */
	    apply_w = XmSelectionBoxGetChild (w, XmDIALOG_APPLY_BUTTON);
	    get_xmstring(apply_w, XmNlabelString, &text);
	    break;
	default:
	    printf ("main prompt_ok_cb: unknown reason: %d\n", s->reason);
	    exit(1);
	}

	get_something (w, XmNuserData, (XtArgVal)&fp);

	if (chg_fld (text, fp)) {
	    if (autotz)
		(void) tz_fromsys (&now);
	    mm_now (1);
	    mm_newcir(1);
	    newcir = 1;
	}

	XtFree (text);

	/* unmanage the prompt dialog in all cases.
	 * (autoUnmanage just does it for the Ok and Cancal buttons).
	 */
	XtUnmanageChild (w);
}

/* put up a prompt dialog near the cursor to ask about fp.
 * use the Apply button for special shortcuts.
 * put up a special one for STPSZ_FID and SITE_FID.
 */
static void
prompt (fp)
Field *fp;
{
	static Widget prompt_w, steps_w;
	Widget w, aw;
	XmString str;
	Arg args[20];
	int n;

	/* sites input is handled all separately */
	if (fp->id == SITE_FID) {
	    sites_query();
	    return;
	}

	if (!prompt_w) {
	    XmString title;
	    int i;

	    /* make the general dialog */
	    title = XmStringCreateLtoR ("xephem Main menu Prompt",
						XmSTRING_DEFAULT_CHARSET);
	    n = 0;
	    XtSetArg(args[n], XmNdefaultPosition, False);  n++;
	    XtSetArg(args[n], XmNdialogTitle, title);  n++;
	    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL);  n++;
	    prompt_w = XmCreatePromptDialog(toplevel_w, "MainPrompt", args, n);
	    XtAddCallback (prompt_w, XmNmapCallback, prompt_map_cb, NULL);
	    XtAddCallback (prompt_w, XmNokCallback, prompt_ok_cb, NULL);
	    XtAddCallback (prompt_w, XmNapplyCallback, prompt_ok_cb, NULL);
	    XmStringFree (title);

	    /* we don't use the Help button at all. */
	    w = XmSelectionBoxGetChild (prompt_w, XmDIALOG_HELP_BUTTON);
	    XtUnmanageChild (w);

	    /* add the special row/column of step controls.
	     * this is only managed for the STPSZ_FID prompt.
	     */
	    n = 0;
	    XtSetArg (args[n], XmNnumColumns, 3); n++;
	    XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
	    XtSetArg (args[n], XmNadjustLast, False); n++;
	    XtSetArg (args[n], XmNspacing, 3); n++;
	    steps_w = XmCreateRowColumn (prompt_w, "MMStepRC", args, n);

	    for (i = 0; i < XtNumber(step_options); i++) {
		StepOption *sop = &step_options[i];

		str = XmStringCreate(sop->title, XmSTRING_DEFAULT_CHARSET);
		n = 0;
		XtSetArg (args[n], XmNlabelString, str); n++;
		w = XmCreatePushButton (steps_w, "MMStepPB", args, n);
		XtAddCallback (w, XmNactivateCallback, mm_step_cb,
								(XtPointer)sop);
		XtManageChild (w);
		XmStringFree(str);
	    }
	}

	/* set the prompt string */
	set_xmstring (prompt_w, XmNselectionLabelString, fp->prompt);

	/* we don't use the Apply button except for some special shortcuts.
	 * for those, we manage it again.
	 */
	aw = XmSelectionBoxGetChild (prompt_w, XmDIALOG_APPLY_BUTTON);
	XtUnmanageChild (aw);

	/* similarly, we assume for now we won't be needing steps_w */
	XtUnmanageChild (steps_w);

	/* clear the text string */
	set_xmstring(prompt_w, XmNtextString, "");

	/* save fp in userData for the callbacks to get at */
	set_something (prompt_w, XmNuserData, (XtArgVal)fp);

	/* set up for the special shortcuts */
	switch (fp->id) {
	case TZN_FID: case TZONE_FID:
	    XtManageChild (aw);
	    set_xmstring (prompt_w, XmNapplyLabelString,
					    autotz ? AutoOff : AutoOn);
	    break;
	case JD_FID: case UD_FID:
	case LD_FID: case LST_FID:
	    XtManageChild (aw);
	    set_xmstring (prompt_w, XmNapplyLabelString, "Now");
	    break;
	case LT_FID:
	    XtManageChild (aw);
	    if (mjd_hr(mjd) - tz == 0.0)
		set_xmstring (prompt_w, XmNapplyLabelString, "Now");
	    else
		set_xmstring (prompt_w,XmNapplyLabelString,"Midnight\nTonight");
	    break;
	case UT_FID:
	    XtManageChild (aw);
	    if (mjd_hr(mjd) == 0.0)
		set_xmstring (prompt_w, XmNapplyLabelString, "Now");
	    else
		set_xmstring (prompt_w, XmNapplyLabelString, "00:00:00");
	    break;
	case STPSZ_FID:
	    XtManageChild (steps_w);
	    break;
	case EPOCH_FID:
	    XtManageChild (aw);
	    if (epoch == EOD)
		set_xmstring (prompt_w, XmNapplyLabelString, "2000");
	    else
		set_xmstring (prompt_w, XmNapplyLabelString, "Epoch Of Date");
	    break;
	case NSTEP_FID:
	    XtManageChild (aw);
	    if (nstep >= 25)
		set_xmstring (prompt_w, XmNapplyLabelString, "1");
	    else
		set_xmstring (prompt_w, XmNapplyLabelString, "1000000");
	    break;
	case PAUSE_FID:
	    XtManageChild (aw);
	    if (spause == 0)
		set_xmstring (prompt_w, XmNapplyLabelString, "1");
	    else
		set_xmstring (prompt_w, XmNapplyLabelString, "0");
	    break;
	case PRES_FID:
	    if (pressure > 0) {
		XtManageChild (aw);
		set_xmstring (prompt_w, XmNapplyLabelString,
							"0\n(No refraction)");
	    }
	    break;
	}

	XtManageChild (prompt_w);

#if XmVersion >= 1001
	w = XmSelectionBoxGetChild (prompt_w, XmDIALOG_TEXT);
	XmProcessTraversal (w, XmTRAVERSE_CURRENT);
	XmProcessTraversal (w, XmTRAVERSE_CURRENT); /* yes, twice!! */
#endif
}

/* go through the buffer bp and set the mm_step_option code.
 * if the code is one that defines a fixed tminc, set it too.
 * if bp doesn't match any of the step options, we assume it's a
 * literal time value.
 * N.B. continue to support RTC and rtc as code strings for compatability.
 */
static void
mm_set_step_code (bp)
char *bp;
{
	int i;

	for (i = 0; i < XtNumber(step_options); i++) {
	    StepOption *sop = &step_options[i];
	    if (strcmp (sop->title, bp) == 0) {
		mm_step_option = sop;
		if (sop->stepcode == FIXED_SS)
		    tminc = sop->inc;
		return;
	    }
	}

	if (bp[0] == 'r' || bp[0] == 'R')
	    mm_step_option = RTC_SS_OPTION;
	else {
	    char lastc = bp[strlen(bp)-1];

	    mm_step_option = NULL;

	    if (lastc == 'd' || lastc == 'D') {
		/* ends in d so treat as a number of days */
		double x;
		if (sscanf (bp, "%lf", &x) == 1)
		    tminc = x * 24.0;
	    } else if (lastc == 's' || lastc == 'S') {
		/* ends in s so treat as a number of sidereal days */
		double x;
		if (sscanf (bp, "%lf", &x) == 1)
		    tminc = x * 24.0 * SIDRATE;
	    } else {
		if (mm_step_option == RTC_SS_OPTION)
		    tminc = 0;	/* no prior */
		f_scansex (tminc, bp, &tminc);
	    }
	}
}

/* print all the time/date/where related stuff: the Now structure.
 * print in a nice order, based on the field locations, as much as possible.
 */
static void
mm_now (all)
int all;
{
	char buf[32];
	double lmjd = mjd - tz/24.0;
	double jd = mjd + MJD0;
	double tmp;

	(void) sprintf (buf, "%*.*s", (int)sizeof(tznm)-1, (int)sizeof(tznm)-1,
									tznm);
	f_string (fw(TZN_FID), buf);
	f_sexa (fw(TZONE_FID), tz, 3, 3600);
	f_time (fw(LT_FID), mjd_hr(lmjd));
	f_date (fw(LD_FID), mjd_day(lmjd));

	f_time (fw(UT_FID), mjd_hr(mjd));
	f_date (fw(UD_FID), mjd_day(mjd));

	f_double (fw(JD_FID), "%13.5f", jd);

	now_lst (&now, &tmp);
	f_time (fw(LST_FID), tmp);

	if (all) {
	    f_showit (fw(SITE_FID), msitename ? msitename : "<No site defined>");
	    f_dms_angle (fw(LAT_FID), lat);
	    f_dms_angle (fw(LONG_FID), -lng);	/* + west */

	    if (pref_get(PREF_UNITS) == PREF_ENGLISH) {
		tmp = elev * (ERAD*FTPM);	/* want ft, not earth radii*/
		f_double (fw(ELEV_FID), "%7.1f ft", tmp);
	    } else {
		tmp = elev * ERAD;		/* want m, not earth radii */
		f_double (fw(ELEV_FID), "%8.1f m", tmp);
	    }

	    tmp = temp;
	    if (pref_get(PREF_UNITS) == PREF_ENGLISH) {
		tmp = 9.*temp/5. + 32.0;   /* want to see degrees F, not C */
#ifdef XK_degree
		(void) sprintf (buf, "%%5.1f %cF", XK_degree);
	    } else
		(void) sprintf (buf, "%%5.1f %cC", XK_degree);
#else
		(void) sprintf (buf, "%%5.1f F");
	    } else
		(void) sprintf (buf, "%%5.1f C");
#endif
	    f_double (fw(TEMP_FID), buf, tmp);

	    tmp = pressure;
	    if (pref_get(PREF_UNITS) == PREF_ENGLISH) {
		tmp /= 33.86;    /* want to see in. Hg, not mBar */
		f_double (fw(PRES_FID), "%5.2f in", tmp);
	    } else {
		f_double (fw(PRES_FID), "%5.0f mB", tmp);
	    }

	    if (epoch == EOD)
		f_string (fw(EPOCH_FID), "Of Date");
	    else {
		mjd_year (epoch, &tmp);
		f_double (fw(EPOCH_FID), "%7.1f", tmp);
	    }
	}

	calm_set (&now);
}

/* display dawn/dusk/length-of-night times.
 */
/* ARGSUSED */
static void
mm_twilight (force)
int force;
{
	double dusk, dawn;
	int status;
	char buf[64];

	twilight_cir (&now, dip, &dawn, &dusk, &status);

	/* twilight_cir gives local times; switch to UTC if prefered */
	if (pref_get(PREF_ZONE) == PREF_UTCTZ) {
	    dawn += now.n_tz;
	    range (&dawn, 24.0);
	    dusk += now.n_tz;
	    range (&dusk, 24.0);
	}

	if (status & (RS_NORISE|RS_RISERR|RS_NOSET|RS_CIRCUMPOLAR|RS_NEVERUP)) {
	    static char nope[] = "-----";
	    f_string (fw(DAWN_FID), nope);
	    f_string (fw(DUSK_FID), nope);
	    f_string (fw(LON_FID), nope);
	} else {
	    double tmp;
	    f_mtime (fw(DAWN_FID), dawn);
	    f_mtime (fw(DUSK_FID), dusk);
	    tmp = dawn - dusk; range (&tmp, 24.0);
	    f_mtime (fw(LON_FID), tmp);
	}
#ifdef XK_degree
	(void) sprintf (buf, "%%g%c", XK_degree);
#else
	(void) sprintf (buf, "%%g degs");
#endif
	f_double (fw(DIP_FID), buf, raddeg(dip));
}

static void
mm_newcir (y)
int y;
{
	static int flag;	/* 0:erase/stop 1:draw/pause 2:erase/pause */

	if (y) {
	    if (!flag) {
		flag = 1;
		(void) XtAppAddTimeOut (xe_app, 1, mm_newcir_cb,
							(XtPointer)&flag);
	    }
	} else {
	    /* just setting flag to zero will stop the flashing soon */
	    flag = 0;
	}
}

/* callback from the timer that makes NEW CIRCUMSTANCES blink.
 * client is a pointer to static state variable.
 */
/* ARGSUSED */
static void
mm_newcir_cb (client, id)
XtPointer client;
XtIntervalId *id;
{
	static char ncmsg[] = "NEW CIRCUMSTANCES";
	static unsigned long fg, bg;
	static int before;
	int *flag = (int *)client;

	/* first time we load the string and get the colors.
	 * then we just fiddle with colors to make it blink.
	 */
	if (!before) {
	    f_showit (newcir_w, ncmsg);
	    get_something (newcir_w, XmNforeground, (XtArgVal)&fg);
	    get_something (newcir_w, XmNbackground, (XtArgVal)&bg);
	    before = 1;
	}

	switch (*flag) {
	case 0:	/* stop -- make text invisible */
	    set_something (newcir_w, XmNforeground, (XtArgVal)bg);
	    break;
	case 1:	/* make the message visible and repeat */
	    set_something (newcir_w, XmNforeground, (XtArgVal)fg);
	    *flag = 2;	/* toggle active state */
	    (void) XtAppAddTimeOut (xe_app, NC_BLINK_DELAY, mm_newcir_cb,
							    (XtPointer)client);
	    break;
	case 2:	/* make the message invisible and repeat */
	    set_something (newcir_w, XmNforeground, (XtArgVal)bg);
	    *flag = 1;	/* toggle active state */
	    (void) XtAppAddTimeOut (xe_app, NC_BLINK_DELAY, mm_newcir_cb,
							    (XtPointer)client);
	    break;
	}
}
