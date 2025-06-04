/* code to manage the stuff on the version display.
 * first thing is our modification history, then code to put up the dialog.
 */

/*
 * 2.8	8/24	solsys: fix pick bug when some objects are turned off.
 *	8/30	skyview: change All labels to Bright labels.
 *	9/1	skyview: find picked objects some 4-5x faster.
 *		skyview: better culling.
 *      9/11	sites: split out from earthmenu handling code into sites.[ch].
 *		plotmenu: improve portability.
 *	9/14	mainmenu: add Sites field and rearrange a few fields.
 *	9/18	earthmenu: fix cylindrical sunlight near equinoxes.
 *	9/28	moon: support for gray-scale image; improve bitmap.
 *		objmenu: honor bell preference if search not found.
 *		skyview: put up generic popup if no objects in FOV.
 *	9/30	moon: mag glass.
 *	10/10	moon: show better moon shadow; support more pixel depths.
 *	10/11	moon: grid; apollo; map features.
 *	10/14	moon: flip; improve phase; add solar angle.
 *	10/16	skyview: maintain flip lr/tb in history.
 *	10/17	moon: move tracking to More info dialog.
 *		sat and jup: add scale calibration tag.
 *	10/18	moon: longitudes are +E but selenographic colongs are +W!!
 *		main: say <No site defined> if no site.
 *	10/28	skyview: add Bright mags view option.
 *	10/31	skyview: just-dots sizes now agree better.
 *		skyview: galaxies as open ellipses, bright neb as open hexagons.
 *		improve planet magnitude model a little.
 *	11/1	jupmenu: update sky background when db changes :-)
 *	11/2	moonmenu: change to linear earthshine brightness model.
 *		skyview: constellation boundaries, and sep color from figures.
 *	11/4	constel: cache the constellation boundaries.
 *		skyview history: also restore mag limits; show RA in hours.
 *		moon: border all around; xor color message; better drtn labels.
 *	11/13	skyview: trail ticks/stamps no longer shrunk in to edge.
 *		moon: use white if can't get annotation or glass border color.
 *		misc: skip leading blanks in all filename opens.
 *		asteroids.edb updated.
 *	11/14	dataview: add HA airmass and columns.
 *		moon: support real sky background.
 *	11/15	moon: add ability to pick sky object for id.
 *		skyview: fix bug in Oph/Sco constellation boundary.
 *		constel: remove redundent edges.
 *	11/16	misc: allow NULL arg to solve_sphere() and use elsewhere.
 *		basic.edb updated with new YBS catalog.
 *	11/18	sky: decrease LOCEP to 1e-6 rads. no dups now based on location.
 *		sky: display Bayer designations in symbol (greek) font.
 *	11/20	sky: separate View->Labels cascade menu.
 *	11/21	objmenu: make Search label a PB.
 *		objmenu: don't make new list just for update.
 *	11/22	split out basic.edb into ybs.edb and messier.edb, change DBinit.
 *		include labeling options in history control.
 *		db.c: dups also mean same type, mag and spectral class.
 *		objmenu: eliminate some needless resorting.
 *	11/24	skyview: start to add printing.
 *		solsys: print from here too.. it's easy now.
 *	11/28	skyhist: add History->delete one; init list with first setting.
 *	11/29	tweak up printing a little.
 *	11/30	skyview: add magnitude scale.
 *	12/4	skyview: mag scale format tweaks.
 *	12/5	ps: provide separate save and print controls.
 *		earth: add print support -- all but sunlight.
 *	12/6	skyview: get greek working in postscript.
 *		constel.c: remove extra edge through RA=18:25.3 edge of Se2.
 *	12/7	update a few fallback settings.
 *	12/14	earthmenu: get sunlight printing working, tweak up labeling.
 *	12/18	ps: optional clipping.
 *	12/19	db: set checkpoint after reading in initial set of files.
 * 2.7.1 8/10	solsys: fix stereo expose and extra ecliptic lines.
 *		clean up help for Triad format and some typos.
 *		skyview: fix overlapping RA/Az grid marks near poles.
 *		formats: fix times near 24:00
 *	8/11	earthmenu: improve changing from cyl <-> sph
 *	8/17	dbmenu: slight change for Ultrix.
 *		solsys: improve help for stereo.
 *	8/19	trails: tweak the character set just a bit.
 *	8/21	jupmenu: delete unused variable.
 * 2.7	5/18	add close objects facility.
 *	5/19	db.c: no "/" between filename components #ifdef VMS
 *	5/22	sky filt: slightly larger symbols and tweak up centering.
 *	5/23	sky view: more tweaking of grid spacing.
 *	5/24	satmenu: display database objects behind saturn.
 *	5/26	allow fractional seconds in all ra/dec entries.
 *	5/30	clean up external function declarations (touched all files).
 *		scan TZName/TZone first so other fields may use when no OS sprt.
 *		data menu: add earth- and sun-object light travel time columns.
 *	5/31	saturn view: add more features.
 *		help menu: don't rely on XmText widget to expand tabs.
 *	6/1	sky view: streamline trail setup with its own dialog.
 *	6/2	sky view: add trail help.
 *	6/13	support new fixed class L for Pulsars and add a sample database.
 *	6/14	minor changes for sunos.
 *		add otherStellarColor resource.
 *		tweak pulsar symbol.
 *	6/15	skyviewmenu: add "Point + Zoom" to pulldown.
 *		reading from DBFIFO no longer pre-deletes back to checkpoint.
 *	6/19	closemenu: add Auto option.
 *		add a simple progress meter.
 *	6/20	dbmenu: open DBFIFO each time the button is pressed.
 *	6/21	dbmenu: add No Dups feature.
 *	6/22	support new fixed class J for Radio and add sample vla databases
 *	6/26	dbmenu: include user-defined objects in tallys now too.
 *	6/28	earthsat: PI2 conflicted with one in linux's math.h.
 *		jupmenu: add features much like satmenu.
 *	6/29	skyview: add zoom factors to popup.
 *	6/30	skyview: move Locate to main menu bar.
 *		jupmenu: add GRS
 *		closing also implies turning off movie demos.
 *	7/4	skyview: add file list control.
 *	7/7	closing views also stops movies.
 *	7/8	data: allow user input of rise/set horizon displacement.
 *	7/10	close: tweak up algorithm, add file write option.
 *	7/11	main: LT "Mightnight Tonight" shortcut and zone name under cal.
 *		add What's up help.
 *	7/12	sky view: added Go back view command
 *		earthmenu: fine tune drawing precision.
 *		earthmenu: trails now connected and use tickmarks like sky view.
 *		closemenu: discard objects below horizon when in topo mode.
 *	7/14	main: fix display of negative timezone and step sizes.
 *	7/15	add trail control (ala sky view) to earth too.
 *	7/16	earthmenu: fix Create Trail prompt.
 *		xephem.c: add basic Trail fallbacks.
 *		solsys: add watch cursor (!)
 *	7/17	misc changes for UnixWare C compiler.
 *		retain trails setup between calls.
 *	7/20	trails: add control over rounding.
 *	7/22	start work on rotated text for trails.
 *	8/3	add more trail options.
 *	8/4	earthmenu: fine-tune circle drawing a bit more.
 *		solsys: graft new trail facility here too.
 *	8/6	solsys: fix trail effect on current object circumstances.
 *		skyview: add explicit Save to compliment Go back.
 *		earth: don't discard trails at Update if Show Trail is on.
 *	8/7	skyview: add richer history facility than just goback.
 * 2.6.3 4/26	set/get_something(): change value from (char *) to (XtArgVal)
 *	5/1	allow ! to introduce a message coming over DBFIFO.
 *		add gscdbd to Manifest.
 *	5/9	add (void *) casts to mem* function args.
 *		XEphem.ad: *background to gray30 and other minor changes.
 *		earth view: add crude+fast option
 *		sky view: allow .1 degree FOV and pointing control.
 *		sky view: allow constellation names without figures.
 *	5/10	sky view: tweak up grid for very small fov.
 *	5/11	sky view: add labels to grid.
 *	5/17	saturn info: add ring tilt from sun (and improve algorithm).
 *		plot: option to draw X axis as dates (not just decimal years)
 *	5/18	fs_date(): allow for %g rounding up day
 *		sky view: send to LOCFIFO via Button3 popup, not Button2.
 *		sky view: use mediumStarColor when spectral class unknown.
 *		start breaking up some help sections into smaller chunks.
 * 2.6.2 1/30	earthmenu.c, line 448: init sites_w (fixes bombing if no sites).
 *	2/28	add comments explaining TZone and TZName to XEphem.ad.
 *		remove obsolete entries for AltAzMap in XEphem.ad.
 *		give a better explanation of discarding sol sys trails in help.
 *		sky and earth views close properly from direct windw mngr cmd.
 *		improve mktime() in time.c (thanks to madler@vssad.enet.dec.com)
 *		surround all colors in XEphem.ad with #ifdef COLOR for xrdb.
 *		add help comment for dawn/dusk needing reasonable TZ Offset.
 * 	4/24	libration.c: change some statics to #defines.
 *		skyviewmenu.c: make popup child of svda_w (fixes X Grab error)
 *		solsysmenu.c: make popup child of svss_w (fixes X Grab error)
 * 2.6.1 1/23	minor portability patches for VMS, OSF/1
 *		fix bug when XEphem.DBinitialFiles was set to nothing.
 *	1/24	scrub xephem.hlp for typos
 *		fixed skyview initial garbage and earthview new-mode half-paint.
 * 2.6  4/11	add Movie shortcuts to some of the graphical views for demos.
 *		message menu now starts in an unmapped state.
 *	4/13	various performance improvments: SkyView is 25% faster now.
 *      5/3	display all timestamp updates, even during movie loops.
 *	5/12	draw Sun and Moon to actual size in SkyView (great for eclipses)
 *      8/2	improve earth trig near poles.
 *		improve anomaly.c (thanks to Richard Clark).
 *      8/4	allow searching for db object by name.
 *		improve stability of ObjX/Y menu sizing behavior.
 *		add top/bottom and left/right flip controls to sky view.
 *	8/7	improve placement of plot labels.
 *	8/23	continue object name searches from where left off last.
 *		add display of LMT (UT - radhr(LNG)) to earth popup and tracking
 *	8/31	add cylindrical projection to earth menu.
 *	9/27	updated ephem.db with epoch 10/5/1994 asteroids.
 *	10/2	fix coord tracking bug in earthview.
 *	11/2	improved earth map data.
 *		improve stability of earthview menu sizing behavior.
 *		add N/S and E/W flippers to saturn and jupiter views.
 *		just draw bullseye for newest trailing entry in earthview.
 *	11/14	show solar eclipse path projection on earth view.
 *		s_edist is now in AU throughout (and added constants to support)
 *		add display of LST (GST- radhr(LNG)) to earth popup and tracking
 *		gray out fields that are not updated when looping with Pause=0
 *		Timezone preference now effects all time fields, not just stamp.
 *		Auto timezone tracking mode (maintains local conventions)
 *		support for longer time zone names (up to 7 characters)
 *	11/16	add constellation figures to skyview.
 *	11/17	Pause is now a float number of seconds.
 *	11/21	support fixed class "R": supernova remnant
 *		improve some of the skyviewmenu object symbols.
 *		add size to sky view popup menu.
 *		ignore case when sorting objx/y list.
 *		add a basic set of built-in objects.
 *		provide a general-purpose file selection box to manage db files.
 *		break up ephem.db into several *.edb files.
 *	11/25	add angular separation to skyview tracking.
 *		add FOV and limiting mag to loc_fifo message.
 *		improved support for finite sized objects.
 *		retain az better at zenith if switch to ra/dec and right back.
 *	12/5	faster object culling in skyviewmenu.
 *	12/8	don't init aggregate in earthmenu.c (choked a Sun compiler).
 *		fix constellation figures coords that had dec < 0.
 *		round skyview symbol sizes down from just dots, not up.
 *		remove use of HUGE from misc.c (outdated and some are bogus).
 *	12/12	report fractional days in some dates (fs_date(); +mjd_day()).
 *		fix spacecraft.edb.
 *		set skyview FOV scaleMultiple=10 and try to give it KB focus.
 *	12/13	ignore xephem_sites lines that do not begin with alpha or digit.
 *		change to DBdirectory/DBpattern/DBinitialFiles; drop dbbuiltin.c
 *		fix bug in faster skyviewmenu object culling.
 *		retain ra better at poles if switch to alt/az and right back.
 *	12/16	change layout of most views to use menu bar.
 *	12/28	lots more menu layout changes.
 *	12/29	fix bug in hadec_aa() when lat was near either pole.
 *	12/30	make skyview trailed objects immune to filtering.
 *      1/5/95	menu bar for db menu too.
 *		move opening of DB fifo into db menu with it's own button
 *	1/6	change round earth projection name from polar to spherical.
 *	1/12	switched to Dan Bruton's code for saturn's moons and ring tilt.
 *	1/13	eliminate not-so-portable union initialization from earthmenu.c
 *		use menu bar in plot view too.
 *	1/16	improved constants for Iapetus elements.
 *		fix bug in hours up when using UTZ timezone preference.
 * 2.5a 4/6/94	mods to plot_aux.c and moonmenu.c for monochrome displays.
 * 2.5  6/8	make mainmenu prompt shortcuts context-sensitive.
 *		get timezone name and offset during "Now" ifdef __STDC__.
 *		increase max timezone name length from 3 to 4.
 *	6/10	provide for showing Earth from any sun vantage.
 *      6/10	support first char of '~' in filenames means HOME.
 *      6/16	support osfHelp function key.
 *		allow even more for VMS' lack of timezone info.
 *      6/21	improve Earth map resolution a little.
 *	7/12	two help dialogs were coming up from Help in db menu.
 *              add Earth sites file.
 *	7/13	was not dstrying main or objmenu's prompt dialogs when cncelled.
 *		add timestamp zone preference.
 *      7/17	add Earth object tracking mode.
 *	7/24	add "set main" control to Earth.
 *		draw tracking data in earth and sky views without using labels.
 *	8/10	update from oahirsch@southpower.co.nz re: his VMS UTC correction
 *	8/18	improve button/label switching appearances for b/w monitors.
 * 		get Earth satellite support well along.
 *	8/27	more goodies for the Earth menu.
 *      8/31	add satellite rise/set times.
 *	9/7	add the adaptive time step control options.
 *	9/8	add lunar libration.
 *	9/9	add DBFIFO.
 *	9/15	add sky view object tracking.
 *	9/16	add SKYLOCFIFO.
 *	9/16	add SKYOUTFIFO.
 *		DB menu: change Replace to Delete in prep for db Checkpointing.
 *		NEW CIRCUMSTANCES flashes.
 *	9/23	change method of destroying dialogs.
 *	9/27	add SKYINFIFO.
 *	9/28	confirm quitting.
 *	10/5	mainmenu.c: take out "Now n = now;" for old SPARC compilers.
 *	10/6	add angle format preference and simplified the formatting code.
 *	10/7	earthmenu: use site location, not cursor, for object alt/az.
 *	10/12	add a preference to ring the bell for each new message.
 *	10/27	add matherr().
 *		check for Earth satellite elements being used for too long.
 *	10/29	another tweak for VMS (nonblocking fifo i/o)
 *	11/2	reduced potential for memory fragmentation from db operations.
 *		added notion of a "checkpoint" to the database.
 *		don't remove trails if objects still exist after a db delete.
 *	11/4	don't beep more often than once every few seconds.
 *      11/16	add ecliptic grid and leg options to solar system view.
 *	11/20	fix a problem on monochrome X terminals.
 *	11/22	avoid difftime() for GNU.
 *	11/28	a few tweaks to the manual and help text.
 *	11/30	get rid of most long lines.
 *	12/14	force message box to up for first message.
 *		check that each Earth color is unique.
 * 2.4e 6/1	can not list rise/set/transit times while looping: fixed.
 *		decreased loop delay when pause is set to 0.
 *		changed troublesome comment in skyviewmenu.c.
 *		another change for DEC's alpha in time.c.
 *		added constellation name to Sky View cursor tracking report.
 *		patch to bring v2.4b to v2.4e posted to comp.sources.x.
 * 2.4d 5/27	Sky Mark in ObjX/Y before Sky View seg faults: fixed.
 * 2.4c 5/19	portability enhancements, particularly to VMS and Motif 1.2.
 * 2.4b 5/10	changed to using memcpy() instead of struct assign (for Alpha).
 *		2.4b was released to comp.sources.x May 1993 in v19i090-v19i110.
 * 2.4a 4/23    changed usage of time() (just needed for DEC's Alpha CPUs)
 * 2.4 4/21     fixed blank jup and sat views (unsigned).
 * 2.3 4/12	add ecliptic option to sky view.
 *     4/15	ephem.db: fix Yale names and improve asteroid a/n accuracy.
 *     4/20	add cursor location tracking to sky view.
 *		add "All labels" to sky view.
 *     4/21	add central cross-hair to Earth subsolar view.
 * 2.2 4/3/93	fixed infinite loop when assigning new objx/y with trails on.
 *		removed erroneous grid lines at some pointing directions.
 *		reinstate neglected nutation correction (!) cir_sky().
 *		just show blanks for sun's SnDst and Phase.
 *		fast buttons for 00:00:00 for UT and "Today" under the calendar.
 *		change Epoch-of-date prompts a bit.
 *		cleaned up type casting and add function prototypes.
 *		fixed bad mix of malloc() with XtFree() in db.c
 *     4/5	use pixmap in skyview for much smoother display.
 *     4/6	fix bug that pops up skyfilt when pop down skyview from main.
 *     4/7	improve conversion from helio period to daily motion.
 *     4/8	all datamenu fields are now fixed-width for more stable sizing.
 * 2.1 3/17/93	fixed bug in reading dates from the database.
 *              main menu fields are fixed-width for more stable size.
 * 2.0 3/15/93	major release
 * 2.0A 2/16/93	2.0 alpha put on export and notice posted to sci.astro.
 * 1.1		update sent to export.lcs.mit.edu
 * 1.0 2/24/92	baseline release to comp.sources.x and export.
 * 0.1 12/13/90 first viable beginnings. main three menus working ok.
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
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Text.h>
#include <Xm/Scale.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "patchlevel.h"

extern Widget toplevel_w;
extern XtAppContext xe_app;

extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void set_something P_((Widget w, char *resource, XtArgVal value));

static void v_makew P_((void));
static void fill_msg P_((Widget w));
static void v_unmap_cb P_((Widget w, XtPointer client, XtPointer call));
static void v_da_exp_cb P_((Widget w, XtPointer client, XtPointer call));
static void v_draw P_((void));
static void v_timer_cb P_((XtPointer client, XtIntervalId *id));
static void drawComet P_((Display *dsp, Window win, GC gc, double ang, int rad,
    int tlen, int w, int h));
static void drawPlanet P_((Display *dsp, Window win, GC gc, int sx, int sy,
    int w, int h));
static void v_define_fgc P_((void));

static char *msg[] = {
"xephem - An Interactive Astronomical Ephemeris Program for X",
PATCHLEVEL,
"",
"Copyright (c) 1990,1991,1992,1993,1994,1995 by Elwood Charles Downey",
"ecdowney@noao.edu",
"",
"Permission is granted to make and distribute copies of this program free of",
"charge, provided the copyright notice and this permission notice are",
"preserved on all copies.  All other rights reserved.  No representation is",
"made about the suitability of this software for any purpose.  It is provided",
"\"as is\" without express or implied warranty of any kind.",
"",
"Although NOAO graciously provides me with email support they do not",
"endorse xephem in any way."
};


#define	NMSGR	(sizeof(msg)/sizeof(msg[0]))

/* generate a random number between min and max, of the same type as the
 * highest type of either.
 */
#define	RAND(min,max)	(((rand() & 0xfff)*((max)-(min))/0xfff) + min)

static Widget v_w;
static Widget vda_w;
static XtIntervalId v_timer_id;
static GC v_fgc;
static double rotrate;	/* rotation rate constant - filled on first manage */

/* table of circular orbit radii to portray and the last screen coords.
 * the real solar system has planet radii from .3 to 30, but the 100:1 ratio is
 * so large we don't try and include everything.
 */
typedef struct {
    double r;		/* radius, AU */
    double theta;	/* angle */
    int x, y;		/* last X x,y coord drawn */
} Orbit;

#define	UNDEFX	(-1)		/* value of x when never drawn yet */
static Orbit orbit[] = {
    {1.6, 0.0, UNDEFX, 0},
    {5.4, 0.0, UNDEFX, 0},
    {10., 0.0, UNDEFX, 0},
    {19., 0.0, UNDEFX, 0},
    {30., 0.0, UNDEFX, 0}
};
#define NORBIT	(sizeof(orbit)/sizeof(orbit[0]))
#define	MAXRAD	(orbit[NORBIT-1].r)	/* N.B.use orbit[] with largest radius*/
#define	MINRAD	(orbit[0].r)	/* N.B. use orbit[] with smallest radius */
#define PR 	4		/* radius of planet, pixels */
#define	DT	100		/* pause between screen steps, ms */
#define	NSTARS	100		/* number of background stars to sprinkle in */
#define	DPI	30		/* inner orbit motion per step, degrees*/

/* comet state and info */
#define	CMAXPERI 30		/* max comet perihelion, pixels */
#define	CMAXTAIL 50		/* max comet tail length, pixels */
#define	CMINTAIL 3		/* min comet tail length, pixels */
#define	CMAXDELA 20		/* max comet area per step, sqr pixels */
#define	CMINDELA 10		/* min comet area per step, sqr pixels */

static double angle;		/* angle ccw from straight right, rads */
static double rotation;		/* whole scene rot, rads */
static int radius;		/* dist from sun, pixels (0 means undefined) */
static int taillen;		/* tail length, pixels */
static int delta_area;		/* change in area per step, sqr pixels */
static int perihelion;		/* min dist from sun, pixels */
static int maxtail;		/* max tail len (ie, tail@peri), pixels */

/* called when mainmenu "About.." help is selected.
 */
void
version()
{
	/* make the version form if this is our first time.
	 * also take this opportunity to do things once to init the
	 * planet locations and set the rotation rate.
	 */
	if (!v_w) {
	    int i;
	    v_makew();
	    for (i = 0; i < NORBIT; i++)
		orbit[i].theta = RAND(0,2*PI);
	    rotrate = degrad(DPI)/pow(MINRAD/MAXRAD, -3./2.);
	}
	    
	/* toggle whether up now.
	 * autoUnmanage can bring back down too.
	 */
	if (XtIsManaged(v_w))
	    XtUnmanageChild (v_w);
	else
	    XtManageChild (v_w);
}

/* called to put up or remove the watch cursor.  */
void
v_cursor (c)
Cursor c;
{
	Window win;

	if (v_w && (win = XtWindow(v_w))) {
	    Display *dsp = XtDisplay(v_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}

/* make the v_w widget.
 */
static void
v_makew()
{
	Widget ok_w;
	Widget frame_w;
	Widget text_w;
	XmString str;
	Arg args[20];
	int n;

	/* create form */
	n = 0;
	XtSetArg (args[n], XmNresizePolicy, XmRESIZE_ANY); n++;
	XtSetArg (args[n], XmNautoUnmanage, True); n++;
	XtSetArg (args[n], XmNhorizontalSpacing, 5); n++;
	XtSetArg (args[n], XmNverticalSpacing, 5); n++;
	v_w = XmCreateFormDialog (toplevel_w, "About", args, n);
	XtAddCallback (v_w, XmNunmapCallback, v_unmap_cb, 0);

	/* set some stuff in the parent DialogShell.
	 * setting XmNdialogTitle in the Form didn't work..
	 */
	n = 0;
	XtSetArg (args[n], XmNtitle, "xephem About"); n++;
	XtSetValues (XtParent(v_w), args, n);

	/* make text widget for the version info */

	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNbottomPosition, 50); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg (args[n], XmNeditable, False); n++;
	XtSetArg (args[n], XmNcolumns, 80); n++;
	XtSetArg (args[n], XmNrows, NMSGR); n++;
	text_w = XmCreateScrolledText (v_w, "VText", args, n);
	fill_msg (text_w);
	XtManageChild (text_w);

	/* make the "Ok" push button */

	str = XmStringCreate("Ok", XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNshowAsDefault, True); n++;
	XtSetArg (args[n], XmNlabelString, str); n++;
	ok_w = XmCreatePushButton (v_w, "VOk", args, n);
	XtManageChild (ok_w);
	XmStringFree (str);
	set_something (v_w, XmNdefaultButton, (XtArgVal)ok_w);

	/* make a frame for the drawing area */

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, XtParent(text_w)); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNbottomWidget, ok_w); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNleftPosition, 20); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
	XtSetArg (args[n], XmNrightPosition, 80); n++;
	XtSetArg (args[n], XmNshadowType, XmSHADOW_ETCHED_OUT); n++;
	frame_w = XmCreateFrame (v_w, "VFrame", args, n);
	XtManageChild (frame_w);

	/* make a drawing area for drawing the solar system */

	n = 0;
	vda_w = XmCreateDrawingArea (frame_w, "AboutMap", args, n);
	XtAddCallback (vda_w, XmNexposeCallback, v_da_exp_cb, 0);
	XtAddCallback (vda_w, XmNresizeCallback, v_da_exp_cb, 0);
	XtManageChild (vda_w);
}

static void
fill_msg (w)
Widget w;
{
	char m[100*NMSGR], *mp = m;
	int i;

	/* Generate message to display as one string */
	for (i = 0; i < NMSGR; i++) {
	    (void) sprintf (mp, "%*s%s\n", (78 - (int)strlen(msg[i]))/2, "",
									msg[i]);
	    mp += strlen(mp);
	}

	/* remove final \n to avoid extra blank line at end */
	*--mp = '\0';

	XmTextSetString (w, m);
}

/* version dialog is going away.
 * stop the rotation timer.
 */
/* ARGSUSED */
static void
v_unmap_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	if (v_timer_id) {
	    XtRemoveTimeOut (v_timer_id);
	    v_timer_id = 0;
	}
}

/* expose version drawing area.
 * redraw the scene to the (new?) size.
 */
/* ARGSUSED */
static void
v_da_exp_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	XmDrawingAreaCallbackStruct *c = (XmDrawingAreaCallbackStruct *)call;

	switch (c->reason) {
	case XmCR_RESIZE:
	    /* seems we can get one resize before the first expose.
	     * hence, we don't have a good window to use yet. just let it
	     * go; we'll get the expose soon.
	     */
	    if (!XtWindow(w))
		return;
	    break;
	case XmCR_EXPOSE: {
	    XExposeEvent *e = &c->event->xexpose;
	    /* wait for the last in the series */
	    if (e->count != 0)
		return;
	    break;
	    }
	default:
	    printf ("Unexpected v_w event. type=%d\n", c->reason);
	    exit(1);
	}

	v_draw();
}

static void
v_draw()
{
	Display *dsp = XtDisplay(vda_w);
	Window win = XtWindow(vda_w);
	unsigned int w, h;
	Window root;
	int x, y;
	unsigned int bw, d;
	int i;

	if (!v_fgc)
	    v_define_fgc();

	XGetGeometry(dsp, win, &root, &x, &y, &w, &h, &bw, &d);
	XClearWindow (dsp, win);

	/* draw the orbit ellipsii and forget last drawn locs */
	for (i = 0; i < NORBIT; i++) {
	    int lx, ty;	/* left and top x */
	    int nx, ny; /* width and height */
	    lx = w/2 - orbit[i].r/MAXRAD*w/2 + 0.5;
	    nx = orbit[i].r/MAXRAD*w + 0.5;
	    ty = h/2 - orbit[i].r/MAXRAD*h/2 + 0.5;
	    ny = orbit[i].r/MAXRAD*h + 0.5;
	    XDrawArc (dsp, win, v_fgc, lx, ty, nx-1, ny-1, 0, 360*64);
	    orbit[i].x = UNDEFX;
	}

	/* forget the comet */
	radius = 0;

	/* draw sun at the center */
	drawPlanet (dsp, win, v_fgc, w/2-PR, h/2-PR, 2*PR-1, 2*PR-1);

	/* draw some background stars */
	for (i = 0; i < NSTARS; i++) {
	    int sx, sy;
	    sx = RAND(0,w-1);
	    sy = RAND(0,h-1);
	    XDrawPoint (dsp, win, v_fgc, sx, sy);
	}

	if (!v_timer_id)
	    v_timer_cb (0, 0);

}

/* called whenever the timer goes off.
 * we advance all the planets, draw any that have moved at least a few
 * pixels, and restart a timer.
 */
/* ARGSUSED */
static void
v_timer_cb (client, id)
XtPointer client;
XtIntervalId *id;
{
	Display *dsp = XtDisplay(vda_w);
	Window win = XtWindow(vda_w);
	unsigned int w, h;
	Window root;
	int x, y;
	unsigned int bw, d;
	int i;

	XGetGeometry(dsp, win, &root, &x, &y, &w, &h, &bw, &d);

	for (i = 0; i < NORBIT; i++) {
	    int px, py;	/* planets new center position */
	    double f = orbit[i].r/MAXRAD;	/* fraction of largest radius */
	    orbit[i].theta += rotrate*pow(f, -3./2.);
	    px = w/2 + cos(orbit[i].theta)*w*f/2 + 0.5;
	    py = h/2 - sin(orbit[i].theta)*h*f/2 + 0.5;
	    if (px != orbit[i].x || py != orbit[i].y) {
		/* erase then redraw at new pos, using the XOR GC */
		if (orbit[i].x != UNDEFX)
		    drawPlanet (dsp, win, v_fgc,
				orbit[i].x-PR, orbit[i].y-PR, 2*PR-1, 2*PR-1);
		drawPlanet (dsp, win, v_fgc, px-PR, py-PR, 2*PR-1, 2*PR-1);
		orbit[i].x = px;
		orbit[i].y = py;
	    }
	}

	/* erase last comet position.
	 * N.B. use radius == 0 to mean the very first loop.
	 */
	if (radius != 0)
	    drawComet (dsp, win, v_fgc, angle, radius, taillen, w, h);

	/* comet is definitely outside scene, set fresh initial conditions.
	 */
	if (radius <= 0 || radius > (w+h)/2) {
	    radius = (w+h)/2;
	    rotation = RAND(0,2*PI);
	    perihelion = RAND(PR,CMAXPERI);
	    maxtail = RAND(CMINTAIL,CMAXTAIL);
	    delta_area = RAND(CMINDELA,CMAXDELA);
	    angle = acos(1.0 - 2.0*perihelion/radius) + rotation;
#if 0
	    printf ("initial rad=%d rot=%g peri=%d maxt=%d da=%d angle=%g\n",
		    radius, rotation, perihelion, maxtail, delta_area, angle);
#endif
	}

	/* recompute next step location and draw new comet
	 */
#if 0
	printf ("rad=%d rot=%g peri=%d maxt=%d da=%d angle=%g\n",
		    radius, rotation, perihelion, maxtail, delta_area, angle);
#endif
	angle += (double)delta_area/(radius*radius);
	radius = 2*perihelion/(1.0 - cos(angle - rotation));
	taillen = (maxtail*perihelion*perihelion)/(radius*radius);
	drawComet (dsp, win, v_fgc, angle, radius, taillen, w, h);

	v_timer_id = XtAppAddTimeOut (xe_app, DT, v_timer_cb, 0);
}

/* draw the comet
 */
static void
drawComet (dsp, win, gc, ang, rad, tlen, w, h)
Display *dsp;
Window win;
GC gc;
double ang;	/* desired angle ccw from +x, in rads */
int rad;	/* in pixels from center */
int tlen;	/* length of tail, in pixels */
int w, h;	/* window width and height */
{
	double ca, sa;
	int sx, sy;
	int ex, ey;

	if (tlen < CMINTAIL)
	    tlen = CMINTAIL;

	/* angle is made <0 to get ccw rotation with X's y-down coord system */
	ang = -ang;
	ca = cos(ang);
	sa = sin(ang);

	sx = w/2 + rad * ca;
	sy = h/2 + rad * sa;
	ex = w/2 + (rad+tlen) * ca;
	ey = h/2 + (rad+tlen) * sa;

	XDrawLine (dsp, win, gc, sx, sy, ex, ey);
}

/* draw the planet.
 */
static void
drawPlanet (dsp, win, gc, sx, sy, w, h)
Display *dsp;
Window win;
GC gc;
int sx, sy, w, h;
{
	XFillArc (dsp, win, gc, sx, sy, w, h, 0, 360*64);
}

static void
v_define_fgc()
{
	Display *dsp = XtDisplay(vda_w);
	Window win = XtWindow(vda_w);
	XGCValues gcv;
	unsigned int gcm;
	Pixel fg, bg;

	gcm = GCForeground | GCFunction;
	get_something (vda_w, XmNforeground, (XtArgVal)&fg);
	get_something (vda_w, XmNbackground, (XtArgVal)&bg);
	gcv.foreground = fg ^ bg;
	gcv.function = GXxor;
	v_fgc = XCreateGC (dsp, win, gcm, &gcv);
}
