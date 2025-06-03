/*	 
**  Xodometer
**  Written by Mark H. Granoff
**             PATHWORKS Server Engineering, Digital Equipment Corporation
**             Littleton, MA, USA
**             17 March 1993
**  
**  If you make improvements or enhancements to Xodometer, please send them
**  back to the author at any of the following addresses:
**
**		granoff@keptin.lkg.dec.com
**		granoff@UltraNet.com
**		72301.1177@CompuServe.com
**
**  Thanks to my friend and colleague Bob Harris for his suggestions and help.
**  
**  This software is provided as is with no warranty of any kind, expressed or
**  implied. Neither Digital Equipment Corporation nor Mark Granoff will be
**  held accountable for your use of this software.
**  
**  This software is released into the public domain and may be redistributed
**  freely provided that all source module headers remain intact in their
**  entirety, and that all components of this kit are redistributed together.
**  
**  Modification History
**  --------------------
**  19 Mar 1993	M. Granoff  v1.0  Initial release.
**   3 Aug 1993 M. Granoff  v1.2  Added automatic trip reset feature.
**   5 Jan 1994 M. Granoff  v1.3  Ported to Alpha; moved mi/km button into popup
**				  menu; removed 'Trip' fixed label; added font
**				  resource.
**   6 Jan 1994 M. Granoff  x1.4  Main window no longer grabs input focus when
**				  realized.
**  21 Nov 1994	M. Granoff  x1.4  Added saveFile resource to make location and
**				  name of odometer save file customizable.
**   6 Mar 1995 M. Granoff  x1.5  Added automatic html generation.
**   9 Mar 1995 M. Granoff  x1.6  Converted file to format with verion info!
**				  Changed .format resource to .units.
**				  Improved measurement unit handling and update
**				  accuracy. Removed unneccesary label updates.
**  19 Apr 1995 M. Granoff  V2.0  Removed OpenVMS-specific AST code in favor of
**				  XtAppAddTimeout. (Works a lot better now,
**				  too.)
*/	 

#define module_name XODOMETER
#define module_vers "2.0"

#if defined(__alpha) || defined(__decc)
#pragma module module_name module_vers
#pragma nostandard
#else  /* __vax */
#module module_name module_vers
#endif /* __alpha */

#ifdef __alpha
#pragma standard
#endif /* __alpha */

#include <stdio.h>
#include <descrip.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <file.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/MwmUtil.h>
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include "Xodometer.bit"
/* module level stuff */

typedef enum _units {
	      	  inch = 0, foot, mile, MAX_UNIT
	      } Units;

typedef struct conversionEntry {
	    Units fromUnit;
	    char *fromUnitTag;
	    char *fromUnitTagPlural;
	    double maxFromBeforeNext;
	    double conversionFactor;
	    char *toUnitTag;
	    char *toUnitTagPlural;
	    double maxToBeforeNext;
	    int *printPrecision;
	    }; conversionEntry;

static	struct conversionEntry ConversionTable[MAX_UNIT] = {
    { inch, "inch", "inches", 12.0,   2.54,     "cm",     "cm",     100.0,  3 },
    { foot, "foot", "feet",   5280.0, 0.3048,    "meter", "meters", 1000.0, 4 },
    { mile, "mile", "miles",  -1.0,   1.609344, "km",     "km",     -1.0,   5 } };

static	char progName[] = "Xodometer";
static	char progClass[] = "Xodometer";
static  char progVersion[] = "v2.0";
#define MAJOR_VERSION 2
#define MINOR_VERSION 0
XtAppContext appContext;
XtIntervalId updateTimerId;
static	Display *display;
static	Window root, *RootIDRet, *ChildIDRet;
static	int WinX, WinY;
static	unsigned int StateMask;
static  XrmDatabase db;
static	int XCoord = 0, YCoord = 0;
static	int lastXCoord = 0, lastYCoord = 0;
static	int pointerScreen = -1, lastPointerScreen = -1;
static	int Enabled = TRUE;
static	int UseMetric = FALSE;
static  int AutoReset = TRUE;
static  double Distance = 0.0, TripDistance = 0.0;
static	Units distanceUnit, tripDistanceUnit;
static  int debug = 0;
static	char *fontName;
static	char defaultFontName[] = "-adobe-helvetica-bold-r-normal--24-240-*-*-*-*-*-*";
static	char *saveFileName;
static	char defaultSaveFileName[] = "decw$user_defaults:xodometer.sav";
static	char startDate_s[128];
static	int cyclesSinceLastSave = 0;
static	int htmlEnable = TRUE;
static	char *htmlFileName;
static	char defaultHtmlFileName[] = "decw$user_defaults:xodometer.html";
static	char *htmlTitle;
static	char defaultHtmlTitle[] = "My Mouse!";
static	char *htmlHeader;
static	char defaultHtmlHeader[] = "My Mouse!";

typedef struct
    {
	struct tm lt;
	double Odometer;
    } _oldOdometerInfo;

typedef struct
    {
	int majVers, minVers;
	struct tm lt;
	double Odometer;
	Units unit;
    } _odometerInfo;
_odometerInfo odometerInfo;

static  Widget TopLevel, MainWindow, MainDialog, DistanceLabel, TripLabel, TripButton;
static  Widget puMenu, puMenuLabel, puMenuLabel1, puSep1, puOnOff, puAuto, puHtml,
	puFormat, puSaveSettings, puSep, puQuit;

static XrmOptionDescRec options[] =
	    {
	    {"-background", ".background", XrmoptionSepArg, (caddr_t) NULL},
	    {"-bg",         ".background", XrmoptionSepArg, (caddr_t) NULL},
	    {"-display",    ".display",    XrmoptionSepArg, (caddr_t) NULL},
	    {"-foreground", ".foreground", XrmoptionSepArg, (caddr_t) NULL},
	    {"-fg",         ".foreground", XrmoptionSepArg, (caddr_t) NULL},
	    {"-geometry",   ".geometry",   XrmoptionSepArg, (caddr_t) NULL},
	    {"-iconic",	    ".iconic",	   XrmoptionNoArg, "FALSE"},
	    {"-help",	    ".help",       XrmoptionNoArg, "FALSE"},
	    {"-units",	    ".units",	   XrmoptionSepArg, "us"},
	    {"-orientation",".orientation",XrmoptionSepArg, "left"},
	    {"-state",	    ".state",	   XrmoptionSepArg, "on"},
	    {"-autoreset",  ".autoreset",  XrmoptionSepArg,  "on"},
	    {"-font",       ".font",       XrmoptionSepArg, defaultFontName},
	    {"-saveFile",   ".saveFile",   XrmoptionSepArg, defaultSaveFileName},
	    {"-html",	    ".html",	   XrmoptionSepArg, "on"},
	    {"-htmlFile",   ".htmlFile",   XrmoptionSepArg, defaultHtmlFileName},
	    {"-htmlTitle",   ".htmlTitle",   XrmoptionSepArg, defaultHtmlTitle},
	    {"-htmlHeader",   ".htmlHeader",   XrmoptionSepArg, defaultHtmlHeader},
	    {"-debug",	    ".debug",	   XrmoptionNoArg, "FALSE"}, /* Leave last */
	    };
static optcount = (sizeof options) / (sizeof (XrmOptionDescRec));

static char *month[12] = {
    "Jan", "Feb", "Mar", "Apr",
    "May", "Jun", "Jul", "Aug",
    "Sep", "Oct", "Nov", "Dec"
};
static char *wday[7] = {
    "Sun", "Mon", "Tue", "Wed",
    "Thu", "Fri", "Sat"
};

#define MAX_SCREEN 16
typedef struct
    {
	Window root;
	Screen *scr;
	int height, width;
	double PixelsPerMM;
    } _screenInfo;
_screenInfo screenInfo[MAX_SCREEN];
static int screenCount;

$DESCRIPTOR (interval_d, "0 00:00:00.10");
unsigned int interval_q[2];
#define SAVE_FREQUENCY 10
#define MAXARGS 25
#define DEFAULTS "decw$user_defaults:xodometer.dat"
#define K_Left 1
#define K_Right 2
#define K_Top 3
#define K_Bottom 4
static int screenOrientation = K_Left;

char *GetResource(char *, char *);
void PutStringResource(char *, char *);
void PutNumericResource(char *, int);
void Usage(char **);
void CleanUp();
void QueryPointer();
void FindAllScreens();
void ReadOdometer();
void WriteOdometer();
void WriteHtml();
int CalcDistance();
void FormatDistance();
double multiplier(Units unit);

void OnOffCB();
void MilesKilosCB();
void QuitCB();
void SaveSettingsCB();
void puPostMenu();
void TripButtonCB();
void AutoResetCB();
void HtmlEnableCB();


main(unsigned int argc,char *argv[])
{
    Arg args[MAXARGS];
    Pixmap icon_pixmap;
    int gx, gy, gw, gh;		/* main window geometry x, y, width, height */

    int n, i, status;
    char geombuff[32];
    char windowTitle[128];
    char puMenuLabel_s[128];
    char *units_p;
    char *state_p;
    char *orientation_p;
    char *autoreset_p;
    char *html_p;
    char distance_s[256];
    long int decor;

    Font font;
    XFontStruct *fontStruct;
    XmFontList fontList;

    /* Initialize the resource manager */
    XrmInitialize();

    db = XrmGetFileDatabase(DEFAULTS);

    XrmParseCommand(&db, options, optcount, "xodometer", &argc, argv);

    if (ResourcePresent("debug")) debug = 1;
    if (debug) printf("Debug mode active.\n");

    if (ResourcePresent("help") || argc>1) {
	Usage(&argv[0]);
	CleanUp(0);
	exit(1);
    }
    
    /* Initialize the toolkit */
    TopLevel = XtAppInitialize(
	    &appContext,			/* App. context is returned */
	    progClass,			/* Root class name. */
	    options,			/* No option list. */
	    optcount,			/* Number of options. */
	    &argc,				/* Address of argc */
	    argv,				/* argv */
	    NULL,				/* Fallback resources */
	    NULL,				/* No override resources */
	    0);				/* Number of override resources */

    display = XtDisplay(TopLevel);
    if (display==NULL) {
	printf("can't open display\n");
	CleanUp(0);
	exit(-1);
    }

    FindAllScreens();

    saveFileName = GetResource("saveFile", defaultSaveFileName);
    if (!saveFileName)
	saveFileName = defaultSaveFileName;

    htmlFileName = GetResource("htmlFile", defaultHtmlFileName);
    if (!htmlFileName)
	htmlFileName = defaultHtmlFileName;

    htmlTitle = GetResource("htmlTitle", defaultHtmlTitle);
    if (!htmlTitle)
	htmlTitle = defaultHtmlTitle;

    htmlHeader = GetResource("htmlHeader", defaultHtmlHeader);
    if (!htmlHeader)
	htmlHeader = defaultHtmlHeader;

    fontName = GetResource("font", defaultFontName);
    font = XLoadFont(display, fontName);
    if (font==0) {
	printf("can't load font %s\n", fontName);
	CleanUp(0);
	exit(-1);
    }
    fontStruct = XLoadQueryFont(display, fontName);
    fontList = XmFontListCreate(fontStruct, XmSTRING_DEFAULT_CHARSET);

    root = RootWindow(display,DefaultScreen(display));
    icon_pixmap = XCreateBitmapFromData(display, root, xodometer_bits,
					xodometer_width, xodometer_height);

    units_p = GetResource("units","us");
    switch (*units_p)
    {
	case 'u':
	case 'U':   /* U.S. */
	    UseMetric = FALSE; break;
	case 'm':
	case 'M':   /* metric */
	    UseMetric = TRUE;  break;
	default:
	    printf("invalid units: %s\n", units_p);
	    CleanUp(0);
	    exit(20);
    }
    PutStringResource("units",units_p);

    state_p = GetResource("state","on");
    state_p++;
    switch (*state_p)
    {
	case 'f':
	case 'F':   /* off */
	    Enabled = FALSE; break;
	case 'n':
	case 'N':   /* on */
	    Enabled = TRUE;  break;
	default:
	    state_p--;
	    printf("invalid state: %s\n", state_p);
	    CleanUp(0);
	    exit(20);
    }
    state_p--;
    PutStringResource("state",state_p);

    autoreset_p = GetResource("autoreset","on");
    autoreset_p++;
    switch (*autoreset_p)
    {
	case 'f':
	case 'F':   /* off */
	    AutoReset = FALSE; break;
	case 'n':
	case 'N':   /* on */
	    AutoReset = TRUE;  break;
	default:
	    autoreset_p--;
	    printf("invalid autoreset: %s\n", autoreset_p);
	    CleanUp(0);
	    exit(20);
    }
    autoreset_p--;
    PutStringResource("autoreset",autoreset_p);

    orientation_p = GetResource("orientation","left");
    switch (*orientation_p)
    {
	case 'l':
	case 'L':
	    screenOrientation = K_Left; break;
	case 'r':
	case 'R':
	    screenOrientation = K_Right; break;
	case 't':
	case 'T':
	    screenOrientation = K_Top; break;
	case 'b':
	case 'B':
	    screenOrientation = K_Bottom; break;
	default:
	    printf("invalid screen orientation: %s\n", orientation_p);
	    CleanUp(0);
	    exit(20);
    }
    PutStringResource("orientation",orientation_p);

    html_p = GetResource("html","on");
    html_p++;
    switch (*html_p)
    {
	case 'f':
	case 'F':   /* off */
	    htmlEnable = FALSE; break;
	case 'n':
	case 'N':   /* on */
	    htmlEnable = TRUE;  break;
	default:
	    html_p--;
	    printf("invalid html: %s\n", html_p);
	    CleanUp(0);
	    exit(20);
    }
    html_p--;
    PutStringResource("html", html_p);

    ReadOdometer();

    /* Top level details */
    n = 0;
    XtSetArg(args[n],XtNallowShellResize,TRUE); n++;
    sprintf(geombuff,"%s",GetResource("geometry",""));
    XParseGeometry(geombuff, &gx, &gy, &gw, &gh);
    PutStringResource("geometry",geombuff);
    XtSetArg(args[n],XmNx,gx); n++;
    XtSetArg(args[n],XmNy,gy); n++;
    XtSetArg(args[n],XmNwidth,gw); n++;
    XtSetArg(args[n],XmNheight,gh); n++;
    XtSetArg(args[n],XtNiconPixmap, icon_pixmap); n++;
    if (ResourcePresent("iconic")) 
    {
	XtSetArg(args[n],XtNiconic,TRUE); n++;
    }
    XtSetArg(args[n],XmNiconName, progName); n++;
    sprintf(windowTitle, "%s %s", progName, progVersion);
    XtSetArg(args[n],XmNtitle, windowTitle);  n++;
    decor = MWM_DECOR_BORDER | MWM_DECOR_RESIZEH | MWM_DECOR_TITLE | MWM_DECOR_MINIMIZE;
    XtSetArg(args[n], XmNmwmDecorations, decor); n++;
    XtSetArg(args[n], XtNinput, FALSE); n++; /* Don't grab focus */
    XtSetValues(TopLevel,&args,n);

    /* Create the main window */
    n = 0;
    XtSetArg(args[n], XmNx, gx); n++;
    XtSetArg(args[n], XmNy, gy); n++;
    XtSetArg(args[n], XmNwidth, gw); n++;
    XtSetArg(args[n], XmNheight, gh); n++;
    MainWindow = XmCreateMainWindow(TopLevel, progName, args, n);
    XtManageChild(MainWindow);

    /* Create a dialog box to hold a label */
    n = 0;
    XtSetArg(args[n], XmNdefaultPosition, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_WORK_AREA); n++;
    MainDialog = XmCreateForm(MainWindow, "MainDialog", args, n);
    XtManageChild(MainDialog);

    /* Create a label for odometer value */
    n = 0;
    if (Enabled)
    {
	FormatDistance(Distance, distanceUnit, distance_s, sizeof(distance_s));
	XtSetArg(args[n], XmNlabelString, XmStringLtoRCreate(distance_s, XmSTRING_DEFAULT_CHARSET));
    }
    else
	XtSetArg(args[n], XmNlabelString, XmStringLtoRCreate("------", XmSTRING_DEFAULT_CHARSET));
    n++;
    XtSetArg(args[n], XmNfontList, fontList); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNrecomputeSize, True); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    DistanceLabel = XmCreateLabel(MainDialog, "DistanceLabel", args, n);
    XtManageChild(DistanceLabel);

    /* Create a label for trip odometer value */
    n = 0;
    if (Enabled)
	XtSetArg(args[n], XmNlabelString, XmStringLtoRCreate("0.00000", XmSTRING_DEFAULT_CHARSET));
    else
	XtSetArg(args[n], XmNlabelString, XmStringLtoRCreate("------", XmSTRING_DEFAULT_CHARSET));
    n++;
    XtSetArg(args[n], XmNfontList, fontList); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNrecomputeSize, True); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, DistanceLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    TripLabel = XmCreateLabel(MainDialog, "TripLabel", args, n);
    XtManageChild(TripLabel);

    /* Realize the widget heirarchy, creating and mapping windows. */
    XtRealizeWidget(TopLevel);

    /* Create Popup menu */

    puMenu = XmCreatePopupMenu(TopLevel, "popMenu", NULL, 0);
    XtAddEventHandler(TopLevel, ButtonPressMask, False, puPostMenu, puMenu);

    sprintf(puMenuLabel_s, "%s since", progName);
    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNlabelString, XmStringLtoRCreate(puMenuLabel_s, XmSTRING_DEFAULT_CHARSET)); n++;
    puMenuLabel = XmCreateLabelGadget(puMenu, "puMenuLabel", args, n);
    XtManageChild(puMenuLabel);

    sprintf(puMenuLabel_s, "%s", startDate_s);
    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNlabelString, XmStringLtoRCreate(puMenuLabel_s, XmSTRING_DEFAULT_CHARSET)); n++;
    puMenuLabel1 = XmCreateLabelGadget(puMenu, "puMenuLabel1", args, n);
    XtManageChild(puMenuLabel1);

    XtSetArg(args[0], XmNseparatorType, XmSHADOW_ETCHED_IN);
    puSep1 = XmCreateSeparatorGadget(puMenu, "separator", args, 1);
    XtManageChild(puSep1);

    n = 0;
    if (Enabled)
	XtSetArg(args[n], XmNlabelString, XmStringLtoRCreate("Disable", XmSTRING_DEFAULT_CHARSET));
    else
	XtSetArg(args[n], XmNlabelString, XmStringLtoRCreate("Enable", XmSTRING_DEFAULT_CHARSET));
    n++;
    puOnOff = XmCreatePushButtonGadget(puMenu, "pushbutton", args, n);
    XtAddCallback(puOnOff, XmNactivateCallback, OnOffCB, NULL);
    XtManageChild(puOnOff);

    /* Create a push button to reset Trip odometer */
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Reset Trip" , XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNaccelerator, "Alt<Key>T"); n++;
    XtSetArg(args[n], XmNacceleratorText, XmStringCreateLtoR("Alt/T" , XmSTRING_DEFAULT_CHARSET)); n++;
    TripButton = XmCreatePushButtonGadget(puMenu, "TripButton", args, n);
    XtAddCallback(TripButton, XmNactivateCallback, TripButtonCB, NULL);
    XtManageChild(TripButton);

    /* Create 'Auto Reset' toggle */
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringLtoRCreate("Auto Trip Reset", XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNvalue, AutoReset); n++;
    XtSetArg(args[n], XmNset, (AutoReset ? True : False)); n++;
    puAuto = XmCreateToggleButtonGadget(puMenu, "AutoReset", args, n);
    XtAddCallback(puAuto, XmNvalueChangedCallback, AutoResetCB, NULL);
    XtManageChild(puAuto);

    /* Create 'Generate HTML' toggle */
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringLtoRCreate("Generate HTML", XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNvalue, htmlEnable); n++;
    XtSetArg(args[n], XmNset, (htmlEnable ? True : False)); n++;
    puHtml = XmCreateToggleButtonGadget(puMenu, "htmlEnable", args, n);
    XtAddCallback(puHtml, XmNvalueChangedCallback, HtmlEnableCB, NULL);
    XtManageChild(puHtml);

    /* Create 'Metric Display' toggle */
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringLtoRCreate("Metric Display", XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNvalue, UseMetric); n++;
    XtSetArg(args[n], XmNset, (UseMetric ? True : False)); n++;
    puFormat = XmCreateToggleButtonGadget(puMenu, "UseMetric", args, n);
    XtAddCallback(puFormat, XmNvalueChangedCallback, MilesKilosCB, NULL);
    XtManageChild(puFormat);
    
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringLtoRCreate("Save Settings", XmSTRING_DEFAULT_CHARSET)); n++;
    puSaveSettings = XmCreatePushButtonGadget(puMenu, "pushbutton", args, n);
    XtAddCallback(puSaveSettings, XmNactivateCallback, SaveSettingsCB, NULL);
    XtManageChild(puSaveSettings);

    XtSetArg(args[0], XmNseparatorType, XmSHADOW_ETCHED_IN);
    puSep = XmCreateSeparatorGadget(puMenu, "separator", args, 1);
    XtManageChild(puSep);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringLtoRCreate("Quit", XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNaccelerator, "Alt<Key>Q"); n++;
    XtSetArg(args[n], XmNacceleratorText, XmStringCreateLtoR("Alt/Q" , XmSTRING_DEFAULT_CHARSET)); n++;
    puQuit = XmCreatePushButtonGadget(puMenu, "pushbutton", args, n);
    XtAddCallback(puQuit, XmNactivateCallback, QuitCB, NULL);
    XtManageChild(puQuit);
    
    lastXCoord = lastYCoord = XCoord = YCoord = 0;

    updateTimerId = XtAppAddTimeout(appContext, 10, QueryPointer, 0);

    /* Wait for something to happen */
    XtAppMainLoop(appContext);  /* Never returns */
}

void Usage(argv0)
char **argv0;
{
    int i = 0;
    
    printf("\nUsage:  %s [-option ...]\n\n", *argv0);
    printf("where -option includes:\n\n");
    while (i<optcount-1)
    {
	printf("\t%s\n", options[i].option);
	i++;
    }
    return;
}

/*
 * Routine to get one resource value, supplying
 * a default if it's not defined
 * 
 */
char *GetResource(resrc, def)
char *resrc, *def;
{
    XrmValue value;
    char *reptype;
    char fullname[64], fullclass[64];
    char *ptr;
    int up;
    
    strcpy(fullname, "xodometer.");
    strcat(fullname, resrc);
    strcpy(fullclass, fullname);

    ptr = fullclass;	/* capitalize class name! */
    up  = 1;
    while (*ptr) {
	if (up && islower(*ptr)) *ptr = _toupper(*ptr);
	up = *ptr++ == '.';
	}

    if (XrmGetResource(db, fullname, fullclass, &reptype, &value))
	return value.addr ;

    if (display) {
	ptr = XGetDefault(display, "xodometer", resrc);
	if (ptr) return ptr;
	}

    return def;
}


/*
 * Routine to put a string resource value into database
 */
void PutStringResource(resrc, val)
char *resrc, *val;
{
    XrmValue value;
    char *reptype;
    char fullname[64], fullclass[64];
    char *ptr;
    int up;
    
    strcpy(fullname, "xodometer.");
    strcat(fullname, resrc);
    strcpy(fullclass, fullname);

    ptr = fullclass;	/* capitalize class name! */
    up  = 1;
    while (*ptr) {
	if (up && islower(*ptr)) *ptr = _toupper(*ptr);
	up = *ptr++ == '.';
	}

    /* Set the new value */
    value.addr = val;
    value.size = strlen(val) + 1;
    /* Update the database */
    XrmPutResource(&db, fullname, XtRString, &value);
}


/*
 * Routine to put a numeric resource value into database
 */
void PutNumericResource(resrc, val)
char *resrc;
int val;
{
    XrmValue value;
    char *reptype;
    char fullname[64], fullclass[64];
    char *ptr;
    int up;
    char tmp[256];
    
    strcpy(fullname, "xodometer.");
    strcat(fullname, resrc);
    strcpy(fullclass, fullname);

    ptr = fullclass;	/* capitalize class name! */
    up  = 1;
    while (*ptr) {
	if (up && islower(*ptr)) *ptr = _toupper(*ptr);
	up = *ptr++ == '.';
	}

    tmp[0] = '\0';
    sprintf(tmp, "%d", val);
    /* Set the new value */
    value.addr = tmp;
    value.size = strlen(tmp) + 1;
    /* Update the database */
    XrmPutResource(&db, fullname, XtRString, &value);
}


/*
 * Routine to see if resource present.
 */
int ResourcePresent(resrc)
char *resrc;
{
    XrmValue value;
    char *reptype;
    char fullname[64], fullclass[64];
    char *ptr;
    int up;
    
    strcpy(fullname, "xodometer.");
    strcat(fullname, resrc);
    strcpy(fullclass, fullname);

    ptr = fullclass;	/* capitalize class name! */
    up  = 1;
    while (*ptr) {
	if (up && islower(*ptr)) *ptr = _toupper(*ptr);
	up = *ptr++ == '.';
	}

    if (XrmGetResource(db, fullname, fullclass, &reptype, &value))
	return 1 ;

    if (display) {
	ptr = XGetDefault(display, "xodometer", resrc);
	if (ptr) return 1;
	}

    return 0;
}

void FindAllScreens()
{
    int i = 0;
    int Dh, DhMM, Dw, DwMM;
    double vPixelsPerMM, hPixelsPerMM;
    
    screenCount = ScreenCount(display);
    if (debug) printf("Display has %d screen%s\n", screenCount, (screenCount == 1 ? "" : "s"));

    while (i < screenCount)
    {
	if (debug) printf("Screen %d\n", i);
	screenInfo[i].root = RootWindow(display, i);
	screenInfo[i].scr = XScreenOfDisplay(display, i);
    
	screenInfo[i].height = Dh = HeightOfScreen(screenInfo[i].scr);
	DhMM = HeightMMOfScreen(screenInfo[i].scr);
	screenInfo[i].width = Dw = WidthOfScreen(screenInfo[i].scr);
	DwMM = WidthMMOfScreen(screenInfo[i].scr);

	if (debug)
	{
	    printf("\tHeight is %d pixels (%d mm).\n", Dh, DhMM);
	    printf("\tWidth is %d pixels (%d mm).\n", Dw, DwMM);
	}
	
	vPixelsPerMM = (double)Dh / (double)DhMM;
	hPixelsPerMM = (double)Dw / (double)DwMM;
	screenInfo[i].PixelsPerMM = (vPixelsPerMM + hPixelsPerMM) / 2.0;

	if (debug)
	{
	    printf("\tVertical pixels/mm  are %2.5f mm\n", vPixelsPerMM);
	    printf("\tHorizontal pixels/mm are %2.5f mm\n", hPixelsPerMM);
	    printf("\tAverage pixels/mm are %.2f mm\n", screenInfo[i].PixelsPerMM);
	}
	
	i++;
    }
}

void QueryPointer(
    XtPointer data, /* not used */
    XtIntervalId *id)
{
    if (Enabled)
    {
	lastPointerScreen = pointerScreen;
	lastXCoord = XCoord;
	lastYCoord = YCoord;
	XQueryPointer (display, root, &RootIDRet, &ChildIDRet, &XCoord,
		    &YCoord, &WinX, &WinY, &StateMask);
	if (CalcDistance())
	{
	    refresh_window(TRUE);
	    cyclesSinceLastSave++;
	    if (debug)
		printf("Save Freq: %d  Cycles Since Last Save: %d\n", SAVE_FREQUENCY, cyclesSinceLastSave);
	    if (cyclesSinceLastSave >= SAVE_FREQUENCY)
	    {
		WriteOdometer();
		if (htmlEnable) WriteHtml();
	    }
	}
    }

    updateTimerId = XtAppAddTimeout(appContext, 10, QueryPointer, 0);
}

void OnOffCB(widget,tag,reason)
    Widget widget;
    char *tag;
    XmAnyCallbackStruct *reason;

{
    Arg args[1];
    char buf[32];
    
    if (Enabled)		/* then turn it off */
    {
	Enabled = FALSE;
	XtRemoveTimeout(updateTimerId);
	lastXCoord = lastYCoord = 0;
	strcpy(buf, "Enable");
    }
    else 								   /* turn it back on */
    {
	Enabled = TRUE;
	strcpy(buf, "Disable");
    }

    XtSetArg(args[0], XmNlabelString, XmStringLtoRCreate(buf, XmSTRING_DEFAULT_CHARSET));
    XtSetValues(puOnOff, &args, 1);
    refresh_window(Enabled);
    if (Enabled)
	updateTimerId = XtAppAddTimeout(appContext, 10, QueryPointer, 0);
    else
	WriteOdometer();
}

void MilesKilosCB(widget,tag,reason)
    Widget widget;
    char *tag;
    XmAnyCallbackStruct *reason;
{
    if (UseMetric)	/* switch to miles */
    {
	UseMetric = FALSE;
    }
    else
    {
	UseMetric = TRUE;
    }
    refresh_window(Enabled);
}

void TripButtonCB(widget,tag,reason)
    Widget widget;
    char *tag;
    XmAnyCallbackStruct *reason;
{
    TripDistance = 0.0;
    tripDistanceUnit = inch;
    refresh_window(Enabled);
}

void AutoResetCB(widget,tag,reason)
    Widget widget;
    char *tag;
    XmAnyCallbackStruct *reason;
{
    int status;

    if (AutoReset)
    {
	AutoReset = FALSE;
    }
    else
    {
	AutoReset = TRUE;
    }
    
    if (debug)
	printf("AutoResetCB: Auto Reset %sabled.\n", (AutoReset ? "En" : "Dis"));
    
}

void HtmlEnableCB(widget,tag,reason)
    Widget widget;
    char *tag;
    XmAnyCallbackStruct *reason;
{
    if (htmlEnable)
    {
	htmlEnable = FALSE;
    }
    else
    {
	htmlEnable = TRUE;
    }
    
    if (debug)
	printf("HtmlEnableCB: HTML %sabled.\n", (htmlEnable ? "En" : "Dis"));
    
}

static void SaveSettingsCB(widget,tag,reason)
    Widget widget;
    char *tag;
    XmAnyCallbackStruct *reason;
{
    char geom[25];
    int n;
    Position x, y;
    Dimension w, h;
    Arg args[MAXARGS];

    n = 0;
    XtSetArg(args[n], XmNx, &x); n++;
    XtSetArg(args[n], XmNy, &y); n++;
    XtSetArg(args[n], XmNwidth, &w); n++;
    XtSetArg(args[n], XmNheight, &h); n++;
    XtGetValues(TopLevel, &args, n);
    sprintf(geom, "%dx%d+%d+%d", w, h, x, y);
    PutStringResource("geometry", geom);

    if (UseMetric)
	PutStringResource("units","metric");
    else
	PutStringResource("units","us");

    if (Enabled)
	PutStringResource("state","on");
    else
	PutStringResource("state","off");
	
    switch (screenOrientation)
    {
	case K_Left : PutStringResource("orientation","left"); break;
	case K_Right : PutStringResource("orientation","right"); break;
	case K_Top : PutStringResource("orientation","top"); break;
	case K_Bottom : PutStringResource("orientation","bottom"); break;
    }

    if (AutoReset)
	PutStringResource("autoreset","on");
    else
	PutStringResource("autoreset","off");
    
    PutStringResource("font", fontName);

    PutStringResource("saveFile", saveFileName);

    if (htmlEnable)
	PutStringResource("html", "on");
    else
	PutStringResource("html", "off");
    PutStringResource("htmlFile", htmlFileName);
    PutStringResource("htmlTitle", htmlTitle);
    PutStringResource("htmlHeader", htmlHeader);

    XrmPutFileDatabase(db, DEFAULTS);
    return;
}

/*
 * Delete resource database and purge working set 
 */
void CleanUp(int saveOdometer)
{
    long inadr[2];
    int status;

    inadr[0] = 0x00000000;
    inadr[1] = 0x7fffffff;

    if (db) {
        XrmFreeDatabase(db);
        db = NULL;
    }

    status = sys$purgws(inadr);
    if ((status&1)==0) printf("purge failed\n");

    if (saveOdometer)
	WriteOdometer();

}


void QuitCB(widget,tag,reason)
    Widget widget;
    char *tag;
    XmAnyCallbackStruct *reason;
{
    sys$cantim(0, 0);
    CleanUp(1);
    exit(1);
}

static void  puPostMenu(w, popup, event)
    Widget         w;
    Widget         popup;
    XButtonEvent  *event;
{
    int         status;
    if (event->button != Button3)
        return;

    XmMenuPosition(puMenu, event);
    XtManageChild (puMenu);
}

refresh_window(int flag)
{
#define SECONDS_PER_DAY (60*60*24)
    static char prev_distance_s[256] = {""};
    static char prev_trip_s[256] = {""};
    static unsigned long daysSince1970 = 0;
    unsigned long newDaysSince1970;
    time_t time_now;
    char distance_s[256];
    char trip_s[256];
    int n;
    Arg args[10];

    if (AutoReset)
    {
	time(&time_now);
	if ((newDaysSince1970 = (time_now / SECONDS_PER_DAY)) > daysSince1970)
	{
	    if (debug) printf("daysSince1970 was: %d, now: %d\n", daysSince1970, newDaysSince1970);
	    TripDistance = 0.0;
	    daysSince1970 = newDaysSince1970;
	}
    }

    if (flag)
    {
	FormatDistance(Distance, distanceUnit, distance_s, sizeof(distance_s));
	FormatDistance(TripDistance, tripDistanceUnit, trip_s, sizeof(trip_s));
    }
    else
    {
	strcpy (distance_s, "------");
	strcpy (trip_s, "------");
    }

    if (strcmp(distance_s, prev_distance_s) != 0)
    {
	XFlush(display);
	XtSetArg(args[0], XmNlabelString, XmStringCreateLtoR(distance_s, XmSTRING_DEFAULT_CHARSET));
	XtSetValues(DistanceLabel, &args, 1);
	strcpy (prev_distance_s, distance_s);
    }

    if (strcmp(trip_s, prev_trip_s) != 0)
    {
	XFlush(display);
	XtSetArg(args[0], XmNlabelString, XmStringCreateLtoR(trip_s, XmSTRING_DEFAULT_CHARSET));
	XtSetValues(TripLabel, &args, 1);
	strcpy (prev_trip_s, trip_s);
    }
}

double multiplier(Units unit)
{
    double m = 10;

    switch (unit)
    {
	case mile : m *= 10.0;
	case foot : m *= 10.0;
	case inch : m *= 10.0; break;
    }
    return m;
}

#define MAX(_a,_b) ((_a) > (_b) ? (_a) : (_b))
int CalcDistance()
{
    double dist, sum;
    int X, Y;
    double distMM, distInches, finalNewDist;
    double oldDistance, oldTripDistance;
    double newDistance, newTripDistance;
    int i = 0, j, finalScreen, increment;
    Units oldDistanceUnit, oldTripDistanceUnit, currentUnit;
    int distanceChanged, tripDistanceChanged;
    
    if (lastXCoord == 0 && lastYCoord == 0)
	return FALSE;

    if (lastXCoord == XCoord && lastYCoord == YCoord)
	return FALSE;
	
    /* Figure out which screen the pointer is on */
    if (screenCount > 1)
	while (i < screenCount)
	    if (RootIDRet == screenInfo[i].root)
		break;
	    else
		i++;

    pointerScreen = i;
    
    if (debug) printf("CalcDistance: screen: %d, x: %d, y: %d\n", pointerScreen, XCoord, YCoord);

    /*	 
    **  Adjust XCoord or YCoord for the screen its on, relative to screen 0
    **	and screenOrientation.
    */

    if (lastPointerScreen != -1 && pointerScreen != lastPointerScreen)
    {
	switch (screenOrientation)
	{
	    case K_Left:
	    case K_Top:
		finalScreen = 0;
		j = MAX(pointerScreen,lastPointerScreen) - 1;
		increment = -1;
		break;
	    case K_Right:
	    case K_Bottom:
		finalScreen = MAX(pointerScreen,lastPointerScreen) - 1;
		j = 0;
		increment = 1;
		break;
	}
	
	do
	{
	    switch (screenOrientation)
	    {
		case K_Left:
		case K_Right:
		    if (pointerScreen > lastPointerScreen)
			XCoord += screenInfo[j].width;
		    else
			lastXCoord += screenInfo[j].width;
		    break;
		case K_Top:
		case K_Bottom:
		    if (pointerScreen > lastPointerScreen)
			YCoord += screenInfo[j].height;
		    else
			lastYCoord += screenInfo[j].height;
		    break;
	    }
	    if (j != finalScreen)
		j += increment;
	} while (j != finalScreen);
    	if (debug) printf("  Adjusted for screen ch: x: %d, y: %d\n", XCoord, YCoord);
    }

    if (debug) printf("In: Distance: %.5f, Trip Distance: %.5f\n", Distance, TripDistance);

    /* Calculate distance in pixels first */

    X = XCoord - lastXCoord;
    X = X*X;
    
    Y = YCoord - lastYCoord;
    Y = Y*Y;
    
    sum = (double)X + (double)Y;
    dist = sqrt(sum);

    if (debug) printf("  New dist: %.5fp, ", dist);

    /* Convert to millimeters */
    distMM = dist / screenInfo[pointerScreen].PixelsPerMM;
    if (debug) printf("%.5fmm, ", distMM);

    /* Convert to inches */
    distInches = distMM * 0.04;
    if (debug) printf("%.5f\"", distInches);

    /* Add an appropriate value to Distance, which may be in a unit other than inches */
    currentUnit = inch;
    finalNewDist = distInches;
    while (currentUnit < distanceUnit)
    {
	finalNewDist = finalNewDist / ConversionTable[currentUnit].maxFromBeforeNext;
	if (debug) printf(", %.5f %s", finalNewDist, ConversionTable[currentUnit+1].fromUnitTagPlural);
	currentUnit++;
    }
    if (debug) printf("\n");
    oldDistance = Distance * multiplier(distanceUnit);
    Distance += finalNewDist;
    oldDistanceUnit = distanceUnit;
    if (ConversionTable[distanceUnit].maxFromBeforeNext != -1.0 &&
	Distance >= ConversionTable[distanceUnit].maxFromBeforeNext)
    {
	Distance = Distance / ConversionTable[distanceUnit].maxFromBeforeNext;
	distanceUnit++;
    }
    newDistance = Distance * multiplier(distanceUnit);
    distanceChanged = (distanceUnit != oldDistanceUnit ||
		       (unsigned int)oldDistance != (unsigned int)newDistance);

    /* Add an appropriate value to TripDistance, which may be in a unit other than inches */
    currentUnit = inch;
    finalNewDist = distInches;
    while (currentUnit < tripDistanceUnit)
    {
	finalNewDist = finalNewDist / ConversionTable[currentUnit].maxFromBeforeNext;
	currentUnit++;
    }
    oldTripDistance = TripDistance * multiplier(tripDistanceUnit);
    TripDistance += finalNewDist;
    oldTripDistanceUnit = tripDistanceUnit;
    if (ConversionTable[tripDistanceUnit].maxFromBeforeNext != -1.0 &&
	TripDistance >= ConversionTable[tripDistanceUnit].maxFromBeforeNext)
    {
	TripDistance = TripDistance / ConversionTable[tripDistanceUnit].maxFromBeforeNext;
	tripDistanceUnit++;
    }
    newTripDistance = TripDistance * multiplier(tripDistanceUnit);
    tripDistanceChanged = (tripDistanceUnit != oldTripDistanceUnit ||
		       (unsigned int)oldTripDistance != (unsigned int)newTripDistance);
    
    if (debug) printf("Out: Distance: %.5f, Trip Distance: %.5f\n", Distance, TripDistance);

    if (distanceChanged || tripDistanceChanged)
	return TRUE;
    else
	return FALSE;
}

#define THERE_IS_A_NEXT (ConversionTable[unit].maxToBeforeNext != -1.0)
void FormatDistance(double in_dist, Units unit, char *buf, int buf_siz)
{
    double out_dist = in_dist;
    char *tag;
    int precision;

    memset(buf, 0, buf_siz);

    if (UseMetric)
    {
	out_dist = in_dist * ConversionTable[unit].conversionFactor;
	if (THERE_IS_A_NEXT && out_dist > ConversionTable[unit].maxToBeforeNext)
	{
	    out_dist = out_dist / ConversionTable[unit].maxToBeforeNext;
	    unit++;
	}
	if (out_dist == 1.0)
	    tag = ConversionTable[unit].toUnitTag;
	else
	    tag = ConversionTable[unit].toUnitTagPlural;
    }
    else
    {
	if (out_dist == 1.0)
	    tag = ConversionTable[unit].fromUnitTag;
	else
	    tag = ConversionTable[unit].fromUnitTagPlural;
    }
    precision = ConversionTable[unit].printPrecision;

    sprintf (buf, "%.*f %s", precision, out_dist, tag);
}

void ReadOdometer()
{
    int bt;
    struct tm *lt;
    _oldOdometerInfo oldInfo;
    int f;
    int bytes;

    memset (&odometerInfo, 0, sizeof(_odometerInfo));
    
    f = open (saveFileName, O_RDWR, 0600);
    if (f==-1)
    {
	f = creat (saveFileName, 0600);
	if (f==-1)
	{
	    printf("can't create file: %s\n", saveFileName);
	    CleanUp(0);
	    exit(0);
	}
	if (debug) printf("Creating new save file %s\n", saveFileName);
	time(&bt);
	lt = localtime (&bt);
	memcpy (&odometerInfo.lt, lt, sizeof(struct tm));
	odometerInfo.majVers = MAJOR_VERSION;
	odometerInfo.minVers = MINOR_VERSION;
	odometerInfo.unit = inch;
	write (f, &odometerInfo, sizeof(_odometerInfo));
    }
    else
    {
	if (debug) printf("Reading existing file %s\n", saveFileName);
	bytes = read (f, &odometerInfo, sizeof(_odometerInfo));
	if (bytes == sizeof(_oldOdometerInfo))
	{   /* Detect pre v1.6 save file; upgrade it */
	    if (debug) printf("Converting pre v1.6 file to v%d.%d format...\n", MAJOR_VERSION, MINOR_VERSION);
	    lseek (f, 0, 0);
	    read (f, &oldInfo, sizeof(_oldOdometerInfo));
	    memset (&odometerInfo, 0, sizeof(_odometerInfo));
	    odometerInfo.majVers = MAJOR_VERSION;
	    odometerInfo.minVers = MINOR_VERSION;
	    odometerInfo.lt = oldInfo.lt;
	    odometerInfo.Odometer = oldInfo.Odometer;
	    odometerInfo.unit = mile;

	    if (odometerInfo.Odometer < 1.0) /*	Convert less than a mile to feet */
	    {
		if (debug) printf("  Converted %.5f miles to ", odometerInfo.Odometer);
		odometerInfo.Odometer *= 5280.0;
		if (debug) printf(" %.5f feet", odometerInfo.Odometer);
		odometerInfo.unit = foot;

		if (odometerInfo.Odometer < 1.0) /* Convert less than a foot to inches */
		{
		    odometerInfo.Odometer *= 12.0;
		    if (debug) printf(" to %.5f inches.", odometerInfo.Odometer);
		    odometerInfo.unit = inch;
		}
		if (debug) printf("\n");
	    }

	    lseek (f, 0, 0);
	    write (f, &odometerInfo, sizeof(_odometerInfo));
	}
	Distance = odometerInfo.Odometer;
	distanceUnit = odometerInfo.unit;
    }
    close (f);

    sprintf (startDate_s, "%s %s %d 19%d %02d:%02d\n",
	    wday[odometerInfo.lt.tm_wday],
	    month[odometerInfo.lt.tm_mon],
	    odometerInfo.lt.tm_mday,
	    odometerInfo.lt.tm_year,
	    odometerInfo.lt.tm_hour,
	    odometerInfo.lt.tm_min);
}

void WriteOdometer()
{
    int f;

    f = open (saveFileName, O_WRONLY | O_CREAT, 0600);
    if (f==-1)
    {
	printf("can't open file: %s\n", saveFileName);
	CleanUp(0);
	exit(0);
    }

    if (debug) printf("Writing odometer %.5f ...", Distance);

    odometerInfo.Odometer = Distance;
    odometerInfo.unit = distanceUnit;
    write (f, &odometerInfo, sizeof(_odometerInfo));
    close (f);

    if (debug) printf("  Done.\n");
    
    cyclesSinceLastSave = 0;
}

void WriteHtml()
{
    char distance_s[256];
    char trip_s[256];
    FILE *fp;
    int bt;
    struct tm *lt;
    char time_s[256];

/*    fp = fopen (htmlFileName, "r+");
    if (!fp) 
*/  delete(htmlFileName);    
    fp = fopen(htmlFileName, "w");
    if (!fp)
    {
	printf("can't open file: %s\n", htmlFileName);
	CleanUp(0);
	exit(0);
    }

    if (debug) printf("Writing html ...\n");

    FormatDistance(Distance, distanceUnit, distance_s, sizeof(distance_s));
    FormatDistance(TripDistance, tripDistanceUnit, trip_s, sizeof(trip_s));
    
    time(&bt);
    lt = localtime (&bt);

    sprintf (time_s, "%s %s %d 19%d %02d:%02d",
	    wday[lt->tm_wday],
	    month[lt->tm_mon],
	    lt->tm_mday,
	    lt->tm_year,
	    lt->tm_hour,
	    lt->tm_min);

#define FPF(s) fprintf(fp, s);
    FPF("<HTML>\n");
    FPF("<BODY>\n");
    fprintf(fp, "<TITLE>%s</TITLE>\n", htmlTitle);
    fprintf(fp, "<H1>%s</H1>\n", htmlHeader);
    FPF("<HR>\n");
    FPF("Since its trip odometer was last reset, my mouse has traveled ");
    fprintf(fp, "%s.  \n", trip_s);
    fprintf(fp, "Since %s it has traveled %s.", startDate_s, distance_s);
    FPF("<HR>\n");
    fprintf(fp, "This page was generated dynamically by <EM>Xodometer</EM> at %s.\n", time_s);
    FPF("<HR>\n");
    FPF("<EM>Xodometer</EM> is a Motif application written by ");
    FPF("<A HREF=\"mailto:granoff@keptin.lkg.dec.com\">Mark H. Granoff</A>.\n");
    FPF("</BODY>\n");
    FPF("</HTML>\n");
    fclose(fp);
}
