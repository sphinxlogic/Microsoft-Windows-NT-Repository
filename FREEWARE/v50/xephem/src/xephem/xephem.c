/* main() for xephem.
 * Copyright (c) 1990-1999 by Elwood Charles Downey
 * Permission is granted to make and distribute copies of this program free of
 * charge, provided the copyright notice and this permission notice are
 * preserved on all copies.  All other rights reserved.  No representation is
 * made about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 */

#include <stdio.h>
#include <signal.h>
#if defined(__STDC__)
#include <stdlib.h>
#else
extern char *malloc();
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
#include <Xm/Label.h>

#if XmVersion >= 1002
#include <Xm/RepType.h>
#endif /* XmVersion >= 1002 */


#include "P_.h"
#include "patchlevel.h"

extern String fallbacks[];
extern char Version_resource[];

#define	NMINCOL	150	/* min colors we want for the main colormap */

extern Colormap checkCM P_((Colormap cm, int nwant));
extern FILE *fopenh P_((char *name, char *how));
extern char *getShareDir P_((void));
extern char *getXRes P_((char *name, char *def));
extern char *syserrstr P_((void));
extern int confirm P_((void));
extern int explodeGIF P_((unsigned char *rawpix, int nrawpix, int *wp,
    int *hp, unsigned char *pixap[], unsigned char ra[], unsigned char ga[],
    unsigned char ba[], char errmsg[]));
extern void addXPath P_((void));
extern void c_manage P_((void));
extern void db_manage P_((void));
extern void dm_create_form P_((void));
extern void dm_manage P_((void));
extern void e_manage P_((void));
extern void fs_manage P_((void));
extern void get_something P_((Widget w, char *resource, XtArgVal value));
extern void hlp_dialog P_((char *tag, char *deflt[], int ndeflt));
extern void jm_manage P_((void));
extern void lst_manage P_((void));
extern void m_manage P_((void));
extern void mars_manage P_((void));
extern void mm_external P_((void));
extern void mm_create P_((Widget mainrc));
extern void mm_go_cb P_((Widget w, XtPointer client, XtPointer call));
extern void mm_reset P_((void));
extern void msg_manage P_((void));
extern void net_create P_((void));
extern void net_manage P_((void));
extern void obj_manage P_((void));
extern void plot_manage P_((void));
extern void pref_create_pulldown P_((Widget mb_w));
extern void query P_((Widget tw, char *msg, char *label1, char *label2,
    char *label3, void (*func1)(void), void (*func2)(void),
    void (*func3)(void)));
extern void pm_manage P_((void));
extern void set_something P_((Widget w, char *resource, XtArgVal value));
extern void set_xmstring P_((Widget w, char *resource, char *txt));
extern void sm_manage P_((void));
extern void um_manage P_((void));
extern void srch_manage P_((void));
extern void ss_manage P_((void));
extern void sv_manage P_((void));
extern void version P_((void));
extern void watch_cursor P_((int want));
extern void wtip P_((Widget w, char *tip));

/* these are used to describe and semi-automate making the main pulldown menus
 */
typedef struct {
    char *tip;		/* tip text, if any */
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
    char *tip;		/* tip text, if any */
    char *cb_name;	/* cascade button name */
    char *cb_label;	/* cascade button label (use name if 0) */
    char cb_mne;	/* cascade button mnemonic */
    char *pd_name;	/* pulldown menu name */
    ButtonInfo *bip;	/* array of ButtonInfos, one per button in pulldown */
    int nbip;		/* number of entries in bip[] */
} PullDownMenu;

#define	EXAMPLES_N	"Examples"	/* special ButtonInfo->name */
#define	SEPARATOR_N	"MainSep"	/* special ButtonInfo->name */


static void chk_args P_((int argc, char *argv[]));
static void chk_version P_((void));
static void set_title P_((void));
static void make_main_window P_((void));
static Widget make_pulldown P_((Widget mb_w, PullDownMenu *pdmp));
static void setup_icon P_((void));
static void m_activate_cb P_((Widget w, XtPointer client, XtPointer call));
static void make_examples_pullright P_((Widget pulldown_w, Widget cascade_w));
static void examples_cb P_((Widget w, XtPointer client, XtPointer call));
static void alldone P_((void));
static void x_quit P_((void));


/* client arg to m_activate_cb().
 */
enum {
    PROGRESS, QUIT, XRESET, MSGTXT, NETSETUP, EXTIN,
    DATA, MOON, EARTH, MARS, JUPMOON, SATMOON, UMOON, SKYVIEW, SOLARSYS,
    PLOT, LIST, SEARCH,
    DB, OBJS, FIELDSTARS, CLOSEOBJS, 
    ABOUT, REFERENCES, INTRO, MAINMENU, FILEHLP, VIEWHLP, TOOLSHLP, OBJHLP,
	PREFHLP, OPERATION, DATETIME, NOTES
};

Widget toplevel_w;
#define	XtD	XtDisplay(toplevel_w)
Colormap xe_cm;
XtAppContext xe_app;
static char myclass[] = "XEphem";

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

static XrmOptionDescRec options[] = {
    {"-install", ".install", XrmoptionSepArg, NULL},
    {"-prfb", ".prfb", XrmoptionIsArg, NULL},
    {"-help", ".help", XrmoptionIsArg, NULL},
};

int
main(argc, argv)
int argc;
char *argv[];
{
	/* add handy things to X search path */
	addXPath();

	/* set this before using fallbacks[] */
	(void)sprintf (Version_resource,"%s.Version: %s", myclass, PATCHLEVEL);

	toplevel_w = XtAppInitialize (&xe_app, myclass, options,
			    XtNumber(options), &argc, argv, fallbacks, NULL, 0);

	chk_args (argc, argv);
	chk_version();
	set_title();

	/* load xe_cm and toplevel_w with default or private colormap */
	xe_cm = checkCM (DefaultColormap(XtD,DefaultScreen(XtD)), NMINCOL);
	set_something (toplevel_w, XmNcolormap, (XtArgVal)xe_cm);

#ifdef DO_EDITRES
	XtAddEventHandler (toplevel_w, (EventMask)0, True,
					_XEditResCheckMessages, NULL);
#endif

#if WANT_TEAR_OFF
	/* install converter so tearOffModel can be set in resource files
	 * to TEAR_OFF_{EN,DIS}ABLED.
	 */
	XmRepTypeInstallTearOffModelConverter();
#endif

	/* ignore FPE, though we do have a matherr() handler in misc.c. */
	(void) signal (SIGFPE, SIG_IGN);

#ifdef SIGPIPE
	/* we deal with write errors directly -- don't want the signal */
	(void) signal (SIGPIPE, SIG_IGN);
#endif

	/* connect up the icon pixmap */
	setup_icon ();

	/* make the main menu bar and form (other stuff is in mainmenu.c) */
	make_main_window ();

	/* set up networking, but don't manage it here */
	net_create();

	/* define the data table, but don't manage it.
	 * we do this up front to avoid the delay on its first use later.
	 */
	dm_create_form();

	/* here we go */
	XtRealizeWidget(toplevel_w);
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

	if (toplevel_w && (win = XtWindow(toplevel_w)) != 0) {
	    Display *dsp = XtD;
	    if (c)
		XDefineCursor (dsp, win, c);
	    else
		XUndefineCursor (dsp, win);
	}
}
	
/* ARGSUSED */
static void
chk_args (argc, argv)
int argc;
char *argv[];
{
	if (getXRes ("prfb", NULL)) {
	    String *fbp = fallbacks;
	    for (fbp = fallbacks; *fbp != NULL; fbp++)
		printf ("%s\n", *fbp);
	    exit (0);
	}

	if (argc > 1 || getXRes ("help", NULL)) {
	    fprintf (stderr, "xephem: [options]\n");
	    fprintf (stderr, "  -install {yes no guess}: whether to install a private colormap\n");
	    fprintf (stderr, "  -prfb: print the fallback resources then exit\n");
	    exit (0);
	}
}


/* insure that resource version matches. */
static void
chk_version()
{
	char *v = getXRes ("Version", NULL);

	if (!v || strcmp (v, PATCHLEVEL)) {
	    printf ("X Resources out of date. Must be for %s\n", PATCHLEVEL);
	    exit (1);
	}
}

static void
set_title()
{
	char title[100];

	(void) sprintf (title, "XEphem %s", PATCHLEVEL);
	set_something (toplevel_w, XmNtitle, (XtArgVal) title);
}

/* create an XImage of size wXh.
 * return XImage * if ok else NULL and xe_msg().
 */
static XImage *
create_xim (w, h)
int w, h;
{
	Display *dsp = XtDisplay(toplevel_w);
	XImage *xip;
	int mdepth;
	int mbpp;
	int nbytes;
	char *data;

	/* establish depth and bits per pixel */
	get_something (toplevel_w, XmNdepth, (XtArgVal)&mdepth);
	if (mdepth < 8) {
	    fprintf (stderr, "Require at least 8 bit pixel depth\n");
	    return (NULL);
	}
	mbpp = mdepth>=17 ? 32 : (mdepth >= 9 ? 16 : 8);
	nbytes = w*h*mbpp/8;

	/* get memory for image pixels.  */
	data = malloc (nbytes);
	if (!data) {
	    fprintf(stderr,"Can not get %d bytes for logo pixels", nbytes);
	    return (NULL);
	}

	/* create the XImage */
	xip = XCreateImage (dsp, DefaultVisual (dsp, DefaultScreen(dsp)),
	    /* depth */         mdepth,
	    /* format */        ZPixmap,
	    /* offset */        0,
	    /* data */          data,
	    /* width */         w,
	    /* height */        h,
	    /* pad */           mbpp,
	    /* bpl */           0);
	if (!xip) {
	    fprintf (stderr, "Can not create logo XImage\n");
	    free ((void *)data);
	    return (NULL);
	}

        xip->bitmap_bit_order = LSBFirst;
	xip->byte_order = LSBFirst;

	/* ok */
	return (xip);
}

/* connect up logo.gif */
static void
make_logo (rc)
Widget rc;
{
	Display *dsp = XtDisplay(toplevel_w);
	Window win = RootWindow(dsp, DefaultScreen(dsp));
	char fn[1024];
	unsigned char gif[200000];
	char buf[1024];
	Arg args[20];
	unsigned char *gifpix;
	unsigned char gifr[256], gifg[256], gifb[256];
	unsigned long xpix[256];
	Widget f_w, l_w;
	XImage *xip;
	Pixmap pm;
	GC gc;
	FILE *fp;
	int w, h;
	int ngif;
	int i, n;
	int x, y;

	/* open and explode the gif */
#ifndef VMS
	(void) sprintf (fn, "%s/auxil/logo.gif",  getShareDir());
#else
        (void) sprintf (fn, "%s[auxil]logo.gif",  getShareDir());
#endif
	fp = fopenh (fn, "rb");
	if (!fp) {
	    (void) fprintf (stderr, "%s: %s\n", fn, syserrstr());
	    return; /* darn */
	}
	ngif = fread (gif, 1, sizeof(gif), fp);
	fclose (fp);
	if (ngif < 0) {
	    (void) fprintf (stderr, "%s: %s\n", fn, syserrstr());
	    return; /* darn again */
	}
	if (ngif == sizeof(gif)) {
	    (void) fprintf (stderr, "%s: Max size = %d\n", fn, sizeof(gif));
	    return; /* darn again */
	}
	n = explodeGIF(gif, ngif, &w, &h, &gifpix, gifr, gifg, gifb, buf);
	if (n < 0) {
	    (void) fprintf (stderr, "%s: %s\n", fn, buf);
	    return; /* darn again */
	}

	/* N.B. now must free gifpix */

	/* allocate colors -- don't be too fussy */
	for (i = 0; i < 256; i++) {
	    XColor xc;
	    xc.red   = ((gifr[i] & 0xf8) << 8) | 0x7ff;
	    xc.green = ((gifg[i] & 0xf8) << 8) | 0x7ff;
	    xc.blue  = ((gifb[i] & 0xf8) << 8) | 0x7ff;
	    if (XAllocColor (dsp, xe_cm, &xc))
		xpix[i] = xc.pixel;
	    else {
		fprintf (stderr, "Can not get all logo map colors\n");
		free ((void *)gifpix);
		if (i > 0)
		    XFreeColors (dsp, xe_cm, xpix, i, 0);
		return; /* darn again */
	    }
	}

	/* create XImage */
	xip = create_xim (w, h);
	if (!xip) {
	    XFreeColors (dsp, xe_cm, xpix, 256, 0);
	    free ((void *)gifpix);
	    return; /* darn again */
	}

	/* N.B. now must free xip */

	/* fill XImage with logo */
	for (y = 0; y < h; y++) {
	    int yrow = y*w;
	    for (x = 0; x < w; x++) {
		int gp = (int)gifpix[x + yrow];
		unsigned long p = xpix[gp];
		XPutPixel (xip, x, y, p);
	    }
	}

	/* create pixmap and fill with logo */
	pm = XCreatePixmap (dsp, win, w, h, xip->depth);
	gc = DefaultGC (dsp, DefaultScreen(dsp));
	XPutImage (dsp, pm, gc, xip, 0, 0, 0, 0, w, h);

	/* put pm in a label and let it handle it from now on.
	 * put label in a form so it stretches clear across the rc
	 */
	n = 0;
	f_w = XmCreateForm (rc, "LogoForm", args, n);
	XtManageChild (f_w);

	n = 0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg (args[n], XmNlabelType, XmPIXMAP); n++;
	XtSetArg (args[n], XmNlabelPixmap, pm); n++;
	XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
	l_w = XmCreateLabel (f_w, "LogoLabel", args, n);
#ifndef VMS
	wtip (l_w, "The file <XEphem.ShareDir>/auxil/logo.gif");
#else
        wtip (l_w, "The file <XEphem.ShareDir>[auxil]logo.gif");
#endif
	XtManageChild (l_w);

	/* free with gifpix and xip */
	free ((void *)gifpix);
	free ((void *)xip->data);
	xip->data = NULL;
	XDestroyImage (xip);
}

/* put together the menu bar, the main form, and fill in the form with the
 * initial xephem buttons.
 */
static void
make_main_window ()
{
	static ButtonInfo file_buttons[] = {
	    {"Restore many Main values to original settings",
		"Reset", "Reset", 0, 0, 'R', m_activate_cb, (XtPointer)XRESET},
	    {NULL, SEPARATOR_N},
	    {"Display a dialog containing supporting informational messages",
	        "Messages", "Messages...", 0, 0, 'M', m_activate_cb,
							    (XtPointer)MSGTXT},
	    {"Setup networking options", "Net", "Network setup...", 0, 0,
				    'N', m_activate_cb, (XtPointer)NETSETUP},
	    {"Set up to run xephem Time and Location from an external file",
		"ExtIn", "External Time/Loc...", 0, 0, 'E', m_activate_cb,
							    (XtPointer)EXTIN},
	    {"Display a simple Progress meter",
		"Progress", "Progress Meter...", 0, 0, 'P', m_activate_cb,
							   (XtPointer)PROGRESS},
	    {NULL, SEPARATOR_N},
	    {"Start and stop the main execution loop",
		"Update", "Update", 0, 0, 'U', mm_go_cb, (XtPointer)NULL},
	    {NULL, SEPARATOR_N},
	    {"Exit xephem",
		"Quit", "Quit...", "Ctrl<Key>d", "Ctrl+d", 'Q', m_activate_cb,
							    (XtPointer)QUIT}
	};
	static ButtonInfo view_buttons[] = {
	    {"Display many statistics for any objects",
		"GenData", "Data Table...", 0, 0, 'D', m_activate_cb,
							    (XtPointer)DATA},
	    {NULL, SEPARATOR_N},
	    {"Display an image of Moon and supporting information",
		"Moon", "Moon...", 0, 0, 'M', m_activate_cb, (XtPointer)MOON},
	    {"Display a map of Earth and supporting information",
		"Earth", "Earth...", 0, 0, 'E', m_activate_cb,(XtPointer)EARTH},
	    {"Display an image of Mars and supporting information",
		"Mars", "Mars...", 0, 0, 'r', m_activate_cb, (XtPointer)MARS},
	    {"Display a schematic of Jupiter, GRS, its moons, and other info",
		"Jupiter", "Jupiter...", 0, 0, 'J', m_activate_cb,
							(XtPointer)JUPMOON},
	    {"Display schematic of Saturn, its rings and moons, and other info",
		"Saturn", "Saturn...", 0, 0, 'a', m_activate_cb,
							(XtPointer)SATMOON},
	    {"Display schematic of Uranus, its moons, and other info",
		"Uranus", "Uranus...", 0, 0, 'U', m_activate_cb,
							(XtPointer)UMOON},
	    {NULL, SEPARATOR_N},
	    {"Display a full-featured map of the night sky",
		"SkyV", "Sky View...", 0, 0, 'V', m_activate_cb,
							    (XtPointer)SKYVIEW},
	    {"Display a map of the solar system",
		"SolSys", "Solar System...", 0, 0, 'S', m_activate_cb,
	    						(XtPointer)SOLARSYS}
	};
	static ButtonInfo ctrl_buttons[] = {
	    {"Capture any XEphem values for making and displaying plots",
		"Plot", "Plot values...", 0, 0, 'P', m_activate_cb,
							    (XtPointer)PLOT},
	    {"Capture any XEphem values in a tabular text file",
		"List", "List values...", 0, 0, 'L', m_activate_cb,
							    (XtPointer)LIST},
	    {"Define an equation of XEphem fields and solve for min, max or 0",
		"Solve", "Solve equation...", 0, 0, 'S', m_activate_cb,
							    (XtPointer)SEARCH}
	};
	static ButtonInfo objs_buttons[] = {
	    {"Load .edb files into memory, list current memory, or delete",
		"DataBase", "Load and delete objects in memory...",0, 0, 'D',
						m_activate_cb, (XtPointer)DB},
	    {"Search memory for an object, view definitions, assign ObjXYZ",
		"ObjX_Y", "Search memory and define ObjX,Y,Z...", 0, 0,'S',
					    m_activate_cb, (XtPointer)OBJS},
	    {"Setup field star options and catalog locations",
		"FieldStars", "Setup field stars...", 0, 0, 'F',
					m_activate_cb, (XtPointer)FIELDSTARS},
	    {"Find all pairs of close objects in memory",
		"CloseObjs", "Find close pairs of objects...", 0, 0, 'C',
					m_activate_cb, (XtPointer)CLOSEOBJS}
	};
	static ButtonInfo help_buttons[] = {
	    {"Overall features of xephem",
		"Introduction", "Introduction...", 0, 0, 'I', m_activate_cb,
							    (XtPointer)INTRO},
	    {"How to control xephem's running behavior, including looping",
		"onOperation", "on Operation...", 0, 0, 'e', m_activate_cb,
							(XtPointer)OPERATION},
	    {"Shortcuts to setting date and time formats",
		"onTriad", "on Triad formats...", 0, 0, 'f', m_activate_cb,
							(XtPointer)DATETIME},
	    {NULL, SEPARATOR_N},
	    {"Description of the overall Main xephem menu",
		"onMainMenu", "on Main Menu...", 0, 0, 'M', m_activate_cb,
							(XtPointer)MAINMENU},
	    {"Description of the options available under File",
		"onFile", "on File...", 0, 0, 'F', m_activate_cb,
							    (XtPointer)FILEHLP},
	    {"Description of the options available under View",
		"onView", "on View...", 0, 0, 'V', m_activate_cb,
							    (XtPointer)VIEWHLP},
	    {"Description of the options available under Tools",
		"onTools", "on Tools...", 0, 0, 'T', m_activate_cb,
							(XtPointer)TOOLSHLP},
	    {"Description of the options available under Objects",
		"onObjects", "on Objects...", 0, 0, 'O', m_activate_cb,
							(XtPointer)OBJHLP},
	    {"Description of the options available under Preferences",
		"onPrefs", "on Preferences...", 0, 0, 'P', m_activate_cb,
							(XtPointer)PREFHLP},
	    {NULL, SEPARATOR_N},
	    {NULL, EXAMPLES_N, "Examples", 0, 0, 'E'},
	    {"Credits, references and other kudos.",
		"onReferences", "Credits...",  0, 0, 'C', m_activate_cb,
							(XtPointer)REFERENCES},
	    {"A few supporting issues",
		"Notes", "Notes...", 0, 0, 'N', m_activate_cb,(XtPointer)NOTES},
	    {"Version, copyright, fun graphic",
		"About", "About...", 0, 0, 'A', m_activate_cb, (XtPointer)ABOUT}
	};
	static PullDownMenu file_pd =
	    {"Overall control functions",
		"File", 0, 'F', "file_pd", file_buttons,XtNumber(file_buttons)};
	static PullDownMenu view_pd =
	    {"Major display options",
		"View", 0, 'V', "view_pd", view_buttons,XtNumber(view_buttons)};
	static PullDownMenu ctrl_pd =
	    {"Supporting analysis tools",
		"Tools", 0,'T',"ctrl_pd",ctrl_buttons,XtNumber(ctrl_buttons)};
	static PullDownMenu objs_pd =
	    {"Add, delete and inspect objects in memory",
		"Objects", 0,'O',"objs_pd",objs_buttons,XtNumber(objs_buttons)};
	static PullDownMenu help_pd =
	    {"Additional information about xephem and the Main display",
		"Help", 0, 'H', "help_pd", help_buttons,XtNumber(help_buttons)};
	    
	Widget mainrc;
	Widget mb_w;
	Widget cb_w;
	Arg args[20];
	int n;

	/*	Create main window as a vertical r/c  */
	n = 0;
	XtSetArg (args[n], XmNmarginHeight, 0); n++;
	XtSetArg (args[n], XmNmarginWidth, 0); n++;
	mainrc = XmCreateRowColumn (toplevel_w, "XephemMain", args, n);
	XtAddCallback (mainrc, XmNhelpCallback, m_activate_cb,
							(XtPointer)MAINMENU);
	XtManageChild (mainrc);

	/*	Create MenuBar in mainrc  */

	n = 0;
	mb_w = XmCreateMenuBar (mainrc, "MainMB", args, n); 
	XtManageChild (mb_w);

	    /* create each pulldown */

	    (void) make_pulldown (mb_w, &file_pd);
	    (void) make_pulldown (mb_w, &view_pd);
	    (void) make_pulldown (mb_w, &ctrl_pd);
	    (void) make_pulldown (mb_w, &objs_pd);
	    pref_create_pulldown (mb_w);
	    cb_w = make_pulldown (mb_w, &help_pd);

	    n = 0;
	    XtSetArg (args[n], XmNmenuHelpWidget, cb_w);  n++;
	    XtSetValues (mb_w, args, n);

	/* make a spot for the logo */
	make_logo (mainrc);

	/* add the remainder of the main window */

	mm_create (mainrc);
}

/* create/manage a cascade button with a pulldown menu off a menu bar.
 * return the cascade button.
 * N.B. watch for special bip->name.
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
	    if (bip->tip)
		wtip (button, bip->tip);
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
	if (pdmp->tip)
	    wtip (cascade, pdmp->tip);

	return (cascade);
}

static void
setup_icon ()
{
	Display *dsp = XtDisplay (toplevel_w);
	Window win = RootWindow (dsp, DefaultScreen(dsp));
	Pixmap pm = XCreateBitmapFromData (dsp, win, (char *)xephem_bits,
						xephem_width, xephem_height);

	set_something (toplevel_w, XmNiconPixmap, (XtArgVal)pm);
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
	case NETSETUP:	net_manage(); break;
	case EXTIN:	mm_external(); break;
	case PROGRESS:	pm_manage(); break;
	case QUIT:	x_quit(); break;
	case DATA:	dm_manage(); break;
	case EARTH:	e_manage(); break;
	case MOON:	m_manage(); break;
	case MARS:	mars_manage(); break;
	case JUPMOON:	jm_manage(); break;
	case SATMOON:	sm_manage(); break;
	case UMOON:	um_manage(); break;
	case SKYVIEW:	sv_manage(); break;
	case SOLARSYS:	ss_manage(); break;
	case PLOT:	plot_manage(); break;
	case LIST:	lst_manage(); break;
	case SEARCH:	srch_manage(); break;
	case OBJS:	obj_manage(); break;
	case DB:	db_manage(); break;
	case CLOSEOBJS:	c_manage(); break;
	case FIELDSTARS:fs_manage(); break;
	case ABOUT:	version(); break;
	case REFERENCES:hlp_dialog ("Credits", NULL, 0); break;
	case INTRO:	hlp_dialog ("Intro", NULL, 0); break;
	case MAINMENU:	hlp_dialog ("MainMenu", NULL, 0); break;
	case FILEHLP:	hlp_dialog ("MainMenu -- file", NULL, 0); break;
	case VIEWHLP:	hlp_dialog ("MainMenu -- view", NULL, 0); break;
	case TOOLSHLP:	hlp_dialog ("MainMenu -- tools", NULL, 0); break;
	case OBJHLP:	hlp_dialog ("MainMenu -- objects", NULL, 0); break;
	case PREFHLP:	hlp_dialog ("MainMenu -- preferences", NULL, 0); break;
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
	    char *tip;		/* tip text */
	    char *label;	/* what goes on the help label */
	    char *key;		/* string to call hlp_dialog() */
	} HelpOn;
	static HelpOn helpon[] = {
	    {"Explore tonight's sky with the Sky View",
		"The Sky Tonight",	 "Example - whats up"},
	    {"Display a solar eclipse path on the Earth map",
		"Solar eclipse path",	 "Example - eclipse path"},
	    {"Making trails and using the Hubble GSC field stars",
		"Sky trail and GSC","Example - sky trail"},
	    {"Display a FITS file or retrieve a DSS image over the Internet",
		"Displaying images","Example - images"},
	    {"Make a plot of local sunrise times over the span of one year",
		"Sunrise Plot",	 "Example - sun plot"},
	    {"Solve for a Saturn ring-plane crossing event",
		"Ring crossing",	 "Example - ring plane"},
	    {"Display the moon overtaking a star",
		"Lunar occultation","Example - lunar occultation"},
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
	    wtip (w, hop->tip);
	    XtManageChild (w);
	}

	n = 0;
	XtSetArg (args[n], XmNsubMenuId, pd_w);  n++;
	XtSetValues (cascade_w, args, n);
	wtip (cascade_w, "Step-by-step examples using several xephem features");
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

/* user wants to quit */
static void
x_quit()
{
	if (confirm())
	    query (toplevel_w, "Quit xephem?", "Yes -- quit", "No -- resume", 0,
								alldone, 0, 0);
	else
	    alldone();

	/* if this function returns, we resume */
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: xephem.c,v $ $Date: 1999/02/28 05:05:38 $ $Revision: 1.9 $ $Name:  $"};
