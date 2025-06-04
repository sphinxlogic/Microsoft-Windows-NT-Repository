/* main() for xephem.
 * Copyright (c) 1990,1991,1992,1993,1994,1995 by Elwood Charles Downey
 * Permission is granted to make and distribute copies of this program free of
 * charge, provided the copyright notice and this permission notice are
 * preserved on all copies.  All other rights reserved.  No representation is
 * made about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty, to the extent permitted by
 * applicable law.
 */

#include <stdio.h>
#include <signal.h>
#if defined(__STDC__)
#include <stdlib.h>
#endif

#include <X11/Xlib.h>
#include <X11/IntrinsicP.h> /* for XT_REVISION */

/* define WANT_EDITRES if want to try and support X11R5's EditRes feature.
 * this will require linking with -lXmu and -lXext too.
#define WANT_EDITRES
 */
#if defined(WANT_EDITRES) && (XT_REVISION >= 5)
#define	DO_EDITRES
#endif

#ifdef DO_EDITRES
#include <X11/Xmu/Editres.h>
#endif

#include <Xm/Xm.h>
#include <X11/Shell.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/Form.h>
#include <Xm/Separator.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>

#if XmVersion >= 1002
#include <Xm/RepType.h>
#endif /* XmVersion >= 1002 */


#include "P_.h"

extern void c_manage P_((void));
extern void db_manage P_((void));
extern void dm_manage P_((void));
extern void e_manage P_((void));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void jm_manage P_((void));
extern void lst_manage P_((void));
extern void m_manage P_((void));
extern void mars_manage P_((void));
extern void mm_manage P_((Widget main_window));
extern void mm_reset P_((void));
extern void msg_manage P_((void));
extern void obj_manage P_((void));
extern void plot_manage P_((void));
extern void pref_create_pulldown P_((Widget menu_bar));
extern void query P_((Widget tw, char *msg, char *label1, char *label2,
    char *label3, void (*func1)(), void (*func2)(), void (*func3)()));
extern void pm_manage P_((void));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void sm_manage P_((void));
extern void srch_manage P_((void));
extern void ss_manage P_((void));
extern void sv_manage P_((void));
extern void version P_((void));
extern void watch_cursor P_((int want));

/* these are used to describe and semi-automate making the main pulldown menus
 */
typedef struct {
    char *name;		/* button name, or separator name if !cb */
    			/* N.B. watch for a few special cases */
    char *label;	/* button label (use name if 0) */
    char *acc;		/* button accelerator, if any */
    char *acctext;	/* button accelerator text, if any */
    char mne;		/* button mnemonic */
    void (*cb)();	/* button callback, or NULL if none */
    XtPointer client;	/* button callback client data */
} ButtonInfo;
typedef struct {
    char *cb_name;	/* cascade button name */
    char *cb_label;	/* cascade button label (use name if 0) */
    char cb_mne;	/* cascade button mnemonic */
    char *pd_name;	/* pulldown menu name */
    ButtonInfo *bip;	/* array of ButtonInfos, one per button in pulldown */
    int nbip;		/* number of entries in bip[] */
} PullDownMenu;

#define	EXAMPLES_N	"Examples"	/* special ButtonInfo->name */
#define	SEPARATOR_N	"MainSep"	/* special ButtonInfo->name */


static Widget make_main_window P_((void));
static Widget make_pulldown P_((Widget mb_w, PullDownMenu *pdmp));
static void setup_window_properties P_((Widget w));
static void m_activate_cb P_((Widget w, XtPointer client, XtPointer call));
static void make_examples_pullright P_((Widget pulldown_w, Widget cascade_w));
static void examples_cb P_((Widget w, XtPointer client, XtPointer call));
static void alldone P_((void));


/* client arg to m_activate_cb().
 */
enum {
    PROGRESS, QUIT, XRESET, MSGTXT,
    DATA, MOON, EARTH, MARS, JUPMOON, SATMOON, SKYVIEW, SOLARSYS,
    PLOT, LIST, SEARCH,
    DB, OBJS, CLOSEOBJS,
    ABOUT, REFERENCES, INTRO, MAINMENU, CONFIGFILE, OPERATION, DATETIME, NOTES
};

Widget toplevel_w;
XtAppContext xe_app;
char *myclass = "XEphem";

#define xephem_width 50
#define xephem_height 50
static unsigned char xephem_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,
   0x00, 0x00, 0xf8, 0xff, 0xff, 0x7f, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00,
   0xa0, 0x0f, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x10, 0xf0, 0x00, 0x02, 0x00,
   0x00, 0x00, 0x0c, 0x00, 0x01, 0x01, 0x00, 0x00, 0x08, 0x0c, 0x00, 0x02,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x03,
   0x00, 0x0f, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0xf0, 0x0c, 0x00, 0x00,
   0x03, 0x00, 0x00, 0x00, 0x1f, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x1e,
   0x00, 0x30, 0x00, 0xf8, 0x7f, 0x00, 0x3c, 0x00, 0x08, 0xb0, 0x07, 0x80,
   0x07, 0x40, 0x00, 0x04, 0x78, 0x00, 0x00, 0x18, 0x80, 0x00, 0x04, 0x78,
   0x00, 0x00, 0x20, 0x80, 0x00, 0x02, 0x30, 0x00, 0x02, 0x20, 0x00, 0x01,
   0x02, 0x08, 0x80, 0x0f, 0x40, 0x00, 0x01, 0x82, 0x08, 0x00, 0x07, 0x40,
   0x00, 0x01, 0x02, 0x08, 0x80, 0x0f, 0x40, 0x00, 0x01, 0x02, 0x10, 0x00,
   0x02, 0x20, 0x04, 0x01, 0x04, 0x10, 0x00, 0x00, 0x20, 0x80, 0x00, 0x04,
   0x60, 0x00, 0x00, 0x18, 0x80, 0x00, 0x08, 0x80, 0x07, 0x80, 0x07, 0x40,
   0x00, 0x30, 0x00, 0xf8, 0x7f, 0x00, 0x30, 0x00, 0xc0, 0x00, 0x00, 0x00,
   0x00, 0x0c, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x3c,
   0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x0f, 0x00, 0x00,
   0x00, 0x00, 0xfc, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3c,
   0x00, 0x00, 0x00, 0x60, 0xf0, 0x00, 0xc0, 0x07, 0x00, 0x00, 0xf0, 0x0f,
   0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x20, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00};

static String fallbacks[] = {
    "XEphem*AdpDispl.columns: 5",
    "XEphem*AdpDispl.value: 0.0",
    "XEphem*Algorithms.Accurate.set: True",
    "XEphem*CloseList*textString: work/closelist.txt",
    "XEphem*CloseObjs*List.doubleClickInterval: 500",
    "XEphem*CloseObjs*List.visibleItemCount: 10",
    "XEphem*CloseObjs*Mag.value: 20.0",
    "XEphem*CloseObjs*OmitFixedPairs.set: True",
    "XEphem*CloseObjs*Sep.value: 2.0",
    "XEphem*CloseObjs*Topo.set: True",
    "XEphem*DBManage*NoDups.set: True",
    "XEphem*DataSelMiscCols.Alt.set: True",
    "XEphem*DataSelMiscCols.Az.set: True",
    "XEphem*DataSelMiscCols.Cns.set: True",
    "XEphem*DataSelMiscCols.Dec.set: True",
    "XEphem*DataSelMiscCols.RA.set: True",
    "XEphem*DataSelRisetCols*Limb.set: True",
    "XEphem*DataSelRisetCols.RiseTm.set: True",
    "XEphem*DataSelRisetCols.SetTm.set: True",
    "XEphem*DataSelRows.Moon.set: True",
    "XEphem*DataSelRows.Sun.set: True",
    "XEphem*DateFormat.MDY.set: True",
    "XEphem*Earth*Map.height: 300",
    "XEphem*Earth*Map.width: 500",
    "XEphem*Earth*cylindrical.set: True",
    "XEphem*Earth*grid.set: True",
    "XEphem*Earth*object.set: True",
    "XEphem*Earth*sites.set: True",
    "XEphem*Earth*sunlight.set: True",
    "XEphem*Earth*trail.set: True",
    "XEphem*Help*ScrolledText.columns: 80",
    "XEphem*Help*ScrolledText.rows: 24",
    "XEphem*Help.verticalSpacing: 20",
    "XEphem*Jupiter*BrightMoons.set: True",
    "XEphem*Jupiter*CtlForm.verticalSpacing: 5",
    "XEphem*Jupiter*LimMag.value: 10",
    "XEphem*Jupiter*Map.height: 200",
    "XEphem*Jupiter*Map.width: 300",
    "XEphem*Jupiter*Scale.value: 50",
    "XEphem*Jupiter*Tags.set: True",
    "XEphem*List*Filename.value: work/ephem.lst",
    "XEphem*ListRC.spacing: 5",
    "XEphem*Mars*Map.height: 300",
    "XEphem*Message*ScrolledText.columns: 80",
    "XEphem*Message*ScrolledText.rows: 10",
    "XEphem*Message.verticalSpacing: 20",
    "XEphem*MessageBell.On.set: True",
    "XEphem*Moon*FakeStars.set: True",
    "XEphem*Moon*Scale3X.set: True",
    "XEphem*MoonES*Earthshine.value: 4",
    "XEphem*Plot*Filename.value: work/ephem.plt",
    "XEphem*PlotDA.height: 400",
    "XEphem*PlotDA.width: 400",
    "XEphem*PlotRC.spacing: 5",
    "XEphem*Precision.Low.set: True",
    "XEphem*Precision.Low.set: True",
    "XEphem*Print*textString: lpr",
    "XEphem*Progress*DA.height: 50",
    "XEphem*Progress*DA.width: 200",
    "XEphem*Saturn*BrightMoons.set: True",
    "XEphem*Saturn*CtlForm.verticalSpacing: 5",
    "XEphem*Saturn*LimMag.value: 10",
    "XEphem*Saturn*Map.height: 200",
    "XEphem*Saturn*Map.width: 300",
    "XEphem*Saturn*Scale.value: 50",
    "XEphem*Saturn*Tags.set: True",
    "XEphem*SaveAs*textString: work/xephem.ps",
    "XEphem*SkyFilter*Binary.set: True",
    "XEphem*SkyFilter*BrightMagScale.value: -28",
    "XEphem*SkyFilter*BrightNeb.set: True",
    "XEphem*SkyFilter*ClInNeb.set: True",
    "XEphem*SkyFilter*DarkNeb.set: True",
    "XEphem*SkyFilter*DiffuseNeb.set: True",
    "XEphem*SkyFilter*Double.set: True",
    "XEphem*SkyFilter*EarthSat.set: True",
    "XEphem*SkyFilter*Elliptical.set: True",
    "XEphem*SkyFilter*FaintMagScale.value: 6",
    "XEphem*SkyFilter*GalClusters.set: True",
    "XEphem*SkyFilter*GlobularCl.set: True",
    "XEphem*SkyFilter*Hyperbolic.set: True",
    "XEphem*SkyFilter*Multiple.set: True",
    "XEphem*SkyFilter*OpenCl.set: True",
    "XEphem*SkyFilter*Parabolic.set: True",
    "XEphem*SkyFilter*PlanetaryNeb.set: True",
    "XEphem*SkyFilter*Planets.set: True",
    "XEphem*SkyFilter*Pulsars.set: True",
    "XEphem*SkyFilter*Quasars.set: True",
    "XEphem*SkyFilter*Radio.set: True",
    "XEphem*SkyFilter*SNRemnants.set: True",
    "XEphem*SkyFilter*SphericalGal.set: True",
    "XEphem*SkyFilter*SpiralGal.set: True",
    "XEphem*SkyFilter*Stars.set: True",
    "XEphem*SkyFilter*Stellar.set: True",
    "XEphem*SkyFilter*SymbolDA.width: 25",
    "XEphem*SkyFilter*Undefined.set: True",
    "XEphem*SkyFilter*Variable.set: True",
    "XEphem*SkyList*textString: work/skylist.txt",
    "XEphem*SkyView*AltAzMode.set: True",
    "XEphem*SkyView*AltDecScale.maximum: 900",
    "XEphem*SkyView*AltDecScale.minimum: 0",
    "XEphem*SkyView*AltDecScale.value: 900",
    "XEphem*SkyView*AzRAScale.maximum: 3599",
    "XEphem*SkyView*AzRAScale.minimum: 0",
    "XEphem*SkyView*AzRAScale.value: 1800",
    "XEphem*SkyView*CnsFigures.set: True",
    "XEphem*SkyView*CnsNames.set: True",
    "XEphem*SkyView*FOVScale.scaleMultiple: 20",
    "XEphem*SkyView*FOVScale.value: 1800",
    "XEphem*SkyView*JustDots.set: False",
    "XEphem*SkyView*LblNames.set: True",
    "XEphem*SkyView*Map.height: 500",
    "XEphem*SkyView*Map.width: 500",
    "XEphem*SolarSystem*Ecliptic.set: True",
    "XEphem*SolarSystem*HLatScale.value: 90",
    "XEphem*SolarSystem*Legs.set: True",
    "XEphem*SolarSystem*Names.set: True",
    "XEphem*SolarSystem*SolarDA.height: 300",
    "XEphem*SolarSystem*SolarDA.width: 300",
    "XEphem*SolarSystem*Trails.set: True",
    "XEphem*SolarSystem.DistScale.value: 50",
    "XEphem*SolarSystem.HLatScale.value: 90",
    "XEphem*SrchRC.spacing: 5",
    "XEphem*StdRefr.set: True",
    "XEphem*TZone.Local.set: True",
    "XEphem*Topocentric.set: True",
    "XEphem*TrailPrompt*CustomInterval.value: 0:30",
    "XEphem*Units.English.set: True",
    "XEphem*XmFrame.marginHeight: 0",
    "XEphem*XmFrame.marginWidth: 0",
    "XEphem*background: black",
    "XEphem*blinkRate: 0",
    "XEphem*fontList: fixed",
    "XEphem*foreground: white",
    "XEphem*highlightThickness: 0",
    "XEphem*marginHeight: 1",
    "XEphem*marginWidth: 1",
    "XEphem*spacing: 1",
    "XEphem.DBFIFO: fifos/xephem_db_fifo",
    "XEphem.DBdirectory: edb",
    "XEphem.DBinitialFiles: Messier.edb YBS.edb",
    "XEphem.DBpattern:  *.edb",
    "XEphem.Elevation: 800",
    "XEphem.Epoch: 2000",
    "XEphem.GlassBorderColor: white",
    "XEphem.HELPFILE: auxil/xephem.hlp",
    "XEphem.LT: Now",
    "XEphem.Lat: 40:00:00",
    "XEphem.Long: 90:00:00",
    "XEphem.MoonAnnotColor: white",
    "XEphem.MoonDB: auxil/moon_db",
    "XEphem.MoonFile: auxil/fullmoon.fts",
    "XEphem.MoonShadowDim: 4",
    "XEphem.NSteps: 1",
    "XEphem.Pause: 0",
    "XEphem.Pressure: 29.5",
    "XEphem.Sitename: Chicago, IL",
    "XEphem.SITESFILE: auxil/xephem_sites",
    "XEphem.SKYINFIFO: fifos/xephem_in_fifo",
    "XEphem.SKYLOCFIFO: fifos/xephem_loc_fifo",
    "XEphem.SKYOUTFIFO: fifos/xephem_out_fifo",
    "XEphem.StepSize: RTC",
    "XEphem.Temp: 60",
    "XEphem.TwilightDip: 18",
    "XEphem.allowShellResize: True",
    "XEphem.viewsFont: 6x12",
    "XEphem.viewsGreekFont: -*-symbol-*-*-*-*-12-*-*-*-*-*-*-*",
    "XEphem.JupiterGRSColor: Black",
    "XEphem.JupiterGRSLongitude: 42",
    NULL
};

int
main(argc, argv)
int argc;
char *argv[];
{
	Widget main_window_w;

	toplevel_w = XtAppInitialize (&xe_app, myclass, NULL, 0,
					    &argc, argv, fallbacks, NULL, 0);

#ifdef DO_EDITRES
	XtAddEventHandler (toplevel_w, (EventMask)0, True,
					_XEditResCheckMessages, NULL);
	xe_msg ("Can editres!\n", 0);
#endif

#if XmVersion >= 1002
	/* install converter so XmNtearOffModel can be set in resource files
	 * to XmTEAR_OFF_{EN,DIS}ABLED.
	 */
	XmRepTypeInstallTearOffModelConverter();
#endif

	/* ignore FPE, though we do have a matherr() handler in misc.c. */
	(void) signal (SIGFPE, SIG_IGN);

#ifdef SIGPIPE
	/* we deal with write errors directly -- don't want the signal */
	(void) signal (SIGPIPE, SIG_IGN);
#endif

	/* make the main menu bar and form (other stuff is in mainmenu.c) */
	main_window_w = make_main_window ();

	XtManageChild(main_window_w);
	XtRealizeWidget(toplevel_w);

	/* connect up the icon pixmap */
	setup_window_properties (toplevel_w);

	XtAppMainLoop(xe_app);

	printf ("XtAppMainLoop returned :-)\n");
	return (1);
}

/* called to put up or remove the watch cursor.  */
void
main_cursor (c)
Cursor c;
{
	Window win;

	if (toplevel_w && (win = XtWindow(toplevel_w))) {
	    Display *dsp = XtDisplay(toplevel_w);
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}

/* put together the menu bar, the main form, and fill in the form with the
 * initial xephem buttons.
 */
static Widget
make_main_window ()
{
	static ButtonInfo file_buttons[] = {
	    {"Reset", 0, 0, 0, 'R', m_activate_cb, (XtPointer)XRESET},
	    {"Progress Meter", 0, "Ctrl<Key>p:", "Ctrl+p", 'P', m_activate_cb,
							(XtPointer)PROGRESS},
	    {"Messages", 0, "Ctrl<Key>m:", "Ctrl+m", 'M', m_activate_cb,
							    (XtPointer)MSGTXT},
	    {SEPARATOR_N},
	    {"Quit", 0, "Ctrl<Key>d:", "Ctrl+d", 'Q', m_activate_cb,
							    (XtPointer)QUIT}
	};
	static ButtonInfo view_buttons[] = {
	    {"GenData", "Data Table...", 0, 0, 'D', m_activate_cb,
							    (XtPointer)DATA},
	    {SEPARATOR_N},
	    {"Moon", "Moon...", 0, 0, 'M', m_activate_cb, (XtPointer)MOON},
	    {"Earth", "Earth...", 0, 0, 'E', m_activate_cb, (XtPointer)EARTH},
	    {"Mars", "Mars...", 0, 0, 'r', m_activate_cb, (XtPointer)MARS},
	    {"Jupiter", "Jupiter...", 0, 0, 'J', m_activate_cb,
							(XtPointer)JUPMOON},
	    {"Saturn", "Saturn...",0,0, 'a', m_activate_cb, (XtPointer)SATMOON},
	    {SEPARATOR_N},
	    {"SkyV", "Sky View...",0,0, 'V', m_activate_cb, (XtPointer)SKYVIEW},
	    {"SolSys", "Solar system...", 0, 0, 'S', m_activate_cb,
							(XtPointer)SOLARSYS}
	};
	static ButtonInfo ctrl_buttons[] = {
	    {"Plot", "Plot...", 0, 0, 'P', m_activate_cb, (XtPointer)PLOT},
	    {"Listing", "Listing...", 0,0, 'L', m_activate_cb, (XtPointer)LIST},
	    {"Search", "Search...", 0,0, 'S', m_activate_cb, (XtPointer)SEARCH}
	};
	static ButtonInfo objs_buttons[] = {
	    {"DataBase", "Data Base...",0, 0, 'D', m_activate_cb,
								(XtPointer)DB},
	    {"ObjX_Y", "ObjX, ObjY...", 0, 0, 'O', m_activate_cb,
							    (XtPointer)OBJS},
	    {"CloseObjs", "Find Close Objects...", 0, 0, 'C', m_activate_cb,
							(XtPointer)CLOSEOBJS}
	};
	static ButtonInfo help_buttons[] = {
	    {"Introduction", "Introduction...", 0, 0, 'I', m_activate_cb,
							    (XtPointer)INTRO},
	    {SEPARATOR_N},
	    {"onInitialization", "on Initialization...", 0, 0, 'z', 
					m_activate_cb, (XtPointer)CONFIGFILE},
	    {"onMainMenu", "on Main Menu...", 0, 0, 'M', m_activate_cb,
							(XtPointer)MAINMENU},
	    {"onOperation", "on Operation...", 0, 0, 'O', m_activate_cb,
							(XtPointer)OPERATION},
	    {"onTriad", "on Triad formats...", 0, 0, 'T', m_activate_cb,
							(XtPointer)DATETIME},
	    {SEPARATOR_N},
	    {EXAMPLES_N, "Examples...", 0, 0, 'E'},
	    {"onReferences", "Credits...",  0, 0, 'C', m_activate_cb,
							(XtPointer)REFERENCES},
	    {"Notes", "Notes...", 0, 0, 'N', m_activate_cb, (XtPointer)NOTES},
	    {"About", "About...", 0, 0, 'A', m_activate_cb,
							    (XtPointer)ABOUT}
	};
	static PullDownMenu file_pd =
	    {"File", 0, 'F', "file_pd", file_buttons, XtNumber(file_buttons)};
	static PullDownMenu view_pd =
	    {"View", 0, 'V', "view_pd", view_buttons, XtNumber(view_buttons)};
	static PullDownMenu ctrl_pd =
	    {"Control", 0,'C', "ctrl_pd", ctrl_buttons, XtNumber(ctrl_buttons)};
	static PullDownMenu objs_pd =
	    {"Objects", 0,'O', "objs_pd", objs_buttons, XtNumber(objs_buttons)};
	static PullDownMenu help_pd =
	    {"Help", 0, 'H', "help_pd", help_buttons, XtNumber(help_buttons)};
	    
	Widget main_window;
	Widget menu_bar;
	Widget cb_w;
	Arg args[20];
	int n;

	/*	Create main window as a vertical r/c  */
	n = 0;
	XtSetArg (args[n], XmNmarginHeight, 0); n++;
	XtSetArg (args[n], XmNmarginWidth, 0); n++;
	main_window = XmCreateRowColumn (toplevel_w, "xephem_main", args, n);
	XtAddCallback (main_window, XmNhelpCallback, m_activate_cb,
							(XtPointer)MAINMENU);

	/*	Create MenuBar in main_window.  */

	n = 0;
	menu_bar = XmCreateMenuBar (main_window, "menu_bar", args, n); 
	XtManageChild (menu_bar);

	    /* create each pulldown */

	    (void) make_pulldown (menu_bar, &file_pd);
	    (void) make_pulldown (menu_bar, &view_pd);
	    (void) make_pulldown (menu_bar, &ctrl_pd);
	    (void) make_pulldown (menu_bar, &objs_pd);
	    pref_create_pulldown (menu_bar);
	    cb_w = make_pulldown (menu_bar, &help_pd);

	    n = 0;
	    XtSetArg (args[n], XmNmenuHelpWidget, cb_w);  n++;
	    XtSetValues (menu_bar, args, n);

	/* create and manage the main form */

	mm_manage(main_window);

	return (main_window);
}

/* create/manage a cascade button with a pulldown menu off a menu bar.
 * return the cascade button.
 * N.B. watch for special bip->names.
 */
static Widget
make_pulldown (mb_w, pdmp)
Widget mb_w;
PullDownMenu *pdmp;
{
	Widget pulldown_w;
	Widget button;
	Widget cascade;
	XmString accstr, labstr;
	Arg args[20];
	int n;
	int i;

	/* make the pulldown menu */

	n = 0;
	pulldown_w = XmCreatePulldownMenu (mb_w, pdmp->pd_name, args, n);

	/* fill it with buttons and/or separators */

	for (i = 0; i < pdmp->nbip; i++) {
	    ButtonInfo *bip = &pdmp->bip[i];
	    int examples = !strcmp (bip->name, EXAMPLES_N);
	    int separator = !strcmp (bip->name, SEPARATOR_N);

	    if (separator) {
		Widget s = XmCreateSeparator (pulldown_w, bip->name, args, n);
		XtManageChild (s);
		continue;
	    }

	    accstr = NULL;
	    labstr = NULL;

	    n = 0;
	    if (bip->acctext && bip->acc) {
		accstr = XmStringCreate(bip->acctext, XmSTRING_DEFAULT_CHARSET);
		XtSetArg (args[n], XmNacceleratorText, accstr); n++;
		XtSetArg (args[n], XmNaccelerator, bip->acc); n++;
	    }
	    if (bip->label) {
		labstr = XmStringCreate (bip->label, XmSTRING_DEFAULT_CHARSET);
		XtSetArg (args[n], XmNlabelString, labstr); n++;
	    }
	    XtSetArg (args[n], XmNmnemonic, bip->mne); n++;
	    if (examples)
		button = XmCreateCascadeButton (pulldown_w, bip->name, args, n);
	    else
		button = XmCreatePushButton (pulldown_w, bip->name, args, n);
	    XtManageChild (button);
	    if (bip->cb)
		XtAddCallback (button, XmNactivateCallback, bip->cb,
							(XtPointer)bip->client);
	    if (accstr)
		XmStringFree (accstr);
	    if (labstr)
		XmStringFree (labstr);

	    if (examples)
		make_examples_pullright (pulldown_w, button);
	}

	/* create a cascade button and glue them together */

	labstr = NULL;

	n = 0;
	if (pdmp->cb_label) {
	    labstr = XmStringCreate (pdmp->cb_label, XmSTRING_DEFAULT_CHARSET);
	    XtSetArg (args[n], XmNlabelString, labstr);  n++;
	}
	XtSetArg (args[n], XmNsubMenuId, pulldown_w);  n++;
	XtSetArg (args[n], XmNmnemonic, pdmp->cb_mne); n++;
	cascade = XmCreateCascadeButton (mb_w, pdmp->cb_name, args, n);
	if (labstr)
	    XmStringFree (labstr);
	XtManageChild (cascade);

	return (cascade);
}

static void
setup_window_properties (w)
Widget w;
{
	Pixmap icon_pm;
	XSizeHints xsh;
	Display *dsp = XtDisplay(w);
	Window win = XtWindow (w);
	
	icon_pm = XCreateBitmapFromData (dsp, win, (char *)xephem_bits,
						xephem_width, xephem_height);

	/* can set window size hints here if we like.
	 * N.B. without this, xephem starts in upper left corner.
	 */
	xsh.flags = 0;
	
	XSetStandardProperties (dsp, win, "xephem", "xephem", icon_pm,
							(char **)0, 0, &xsh);
}


/* ARGSUSED */
static void
m_activate_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	int code = (int)client;

	watch_cursor(1);

	switch (code) {
	case XRESET:	mm_reset(); break;
	case MSGTXT:	msg_manage(); break;
	case PROGRESS:	pm_manage(); break;
	case QUIT:	query (toplevel_w, "Quit xephem?", "Yes -- quit",
					    "No -- resume", 0, alldone, 0, 0);
			/* if this query returns, we just resume */
			break; 
	case DATA:	dm_manage(); break;
	case EARTH:	e_manage(); break;
	case MOON:	m_manage(); break;
	case MARS:	mars_manage(); break;
	case JUPMOON:	jm_manage(); break;
	case SATMOON:	sm_manage(); break;
	case SKYVIEW:	sv_manage(); break;
	case SOLARSYS:	ss_manage(); break;
	case PLOT:	plot_manage(); break;
	case LIST:	lst_manage(); break;
	case SEARCH:	srch_manage(); break;
	case OBJS:	obj_manage(); break;
	case DB:	db_manage(); break;
	case CLOSEOBJS:	c_manage(); break;
	case ABOUT:	version(); break;
	case REFERENCES:hlp_dialog ("Credits", NULL, 0); break;
	case INTRO:	hlp_dialog ("Intro", NULL, 0); break;
	case CONFIGFILE:hlp_dialog ("Initialization", NULL, 0); break;
	case MAINMENU:	hlp_dialog ("MainMenu", NULL, 0); break;
	case OPERATION:	hlp_dialog ("Operation", NULL, 0); break;
	case DATETIME:	hlp_dialog ("Date/time", NULL, 0); break;
	case NOTES:	hlp_dialog ("Notes", NULL, 0); break;
	default: 	printf ("Main menu bug: code=%d\n", code); exit(1);
	}

	watch_cursor(0);
}


/* build the examples pullright off the help pulldown driven by cascade_w */
static void
make_examples_pullright (pulldown_w, cascade_w)
Widget pulldown_w;
Widget cascade_w;
{
	typedef struct {
	    char *label;	/* what goes on the help label */
	    char *key;		/* string to call hlp_dialog() */
	} HelpOn;
	static HelpOn helpon[] = {
	    {"What's up?",	"Example - whats up"},
	    {"Eclipse path",	"Example - eclipse path"},
	    {"Sky trail",	"Example - sky trail"},
	    {"Sunrise Plot",	"Example - sun plot"},
	    {"Ring crossing",	"Example - ring plane"},
	    {"Lunar occultation","Example - lunar occultation"},
	};
	Widget pd_w;
	Widget w;
	Arg args[20];
	int n;
	int i;

	n = 0;
	pd_w = XmCreatePulldownMenu (pulldown_w, "ExPD", args, n);

	for (i = 0; i < XtNumber(helpon); i++) {
	    HelpOn *hop = &helpon[i];

	    n = 0;
	    w = XmCreatePushButton (pd_w, "ExPB", args, n);
	    XtAddCallback (w, XmNactivateCallback, examples_cb,
						    (XtPointer)(hop->key));
	    set_xmstring (w, XmNlabelString, hop->label);
	    XtManageChild (w);
	}

	n = 0;
	XtSetArg (args[n], XmNsubMenuId, pd_w);  n++;
	XtSetValues (cascade_w, args, n);
}

/* called from any of the Help->Example entries.
 * client is a tag for hlp_dialog().
 */
/* ARGSUSED */
static void
examples_cb (w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	char *tag = (char *)client;

	hlp_dialog (tag, NULL, 0);
}

/* outta here */
static void
alldone()
{
	XtCloseDisplay (XtDisplay (toplevel_w));
	exit(0);
}
