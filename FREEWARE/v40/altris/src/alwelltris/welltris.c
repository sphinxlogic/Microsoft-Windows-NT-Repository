/*
# GENERIC X-BASED WELLTRIS
#
#	welltris.c
#
###
#
#  Copyright (c) 1993 - 95	David A. Bagley, bagleyd@perry.njit.edu
#
#  Taken from GENERIC X-WINDOW-BASED TETRIS
#
#  Copyright (c) 1992 - 95	 	Q. Alex Zhao, azhao@cc.gatech.edu
#
#			All Rights Reserved
#
#  Permission to use, copy, modify, and distribute this software and
#  its documentation for any purpose and without fee is hereby granted,
#  provided that the above copyright notice appear in all copies and
#  that both that copyright notice and this permission notice appear in
#  supporting documentation, and that the name of the author not be
#  used in advertising or publicity pertaining to distribution of the
#  software without specific, written prior permission.
#
#  This program is distributed in the hope that it will be "playable",
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
*/

#include "welltris.h"

/*** variables ***/
#ifdef VMS
char * vmsusername;
int    vmsuserlen;
#endif /* VMS */

static char     myDisplayName[256];
Display        *display;
int             screen_num;
static Visual  *visual;
Bool            useColor = True;
Colormap        colormap;
Window          mainWin, blockWin;
Cursor          theCursor;
XFontStruct    *bigFont, *tinyFont;
unsigned long   fg, bg;

XSizeHints      sizehints;
static XSizeHints iconsizehints;
XWMHints        wmhints = {
    InputHint | StateHint | IconPixmapHint,
    True,                       /* input mode */
    NormalState,                /* normal */
    0,                          /* icon pixmap */
    0,                          /* icon window */
    0, 0,                       /* icon position */
    0                           /* icon mask pixmap - not used */
};

static char     myHome[FILENAMELEN];
char            scorefile[FILENAMELEN];
int             level = 3, score = 0, rows = 0, squares = 4;
Bool            showNext = False, beep = False, bonusNow = False,
		diagonal = False, mixed = False, bonus = False,
		cw = True, gradualAppear = True, grid = True;
score_t         myscore;

static int      opTableEntries = 23;
static XrmOptionDescRec opTable[] = {
    {"-s", "*scoresOnly", XrmoptionNoArg, (caddr_t) "yes"},
    {"-l", "*startLevel", XrmoptionSepArg, (caddr_t) NULL},
    {"-n", "*numberSquares", XrmoptionSepArg, (caddr_t) NULL},
    {"-showNext", "*showNext", XrmoptionNoArg, (caddr_t) "on"},
    {"-beep", "*beep", XrmoptionNoArg, (caddr_t) "on"},
    {"-diagonal", "*diagonal", XrmoptionNoArg, (caddr_t) "on"},
    {"-mixed", "*mixed", XrmoptionNoArg, (caddr_t) "on"},
    {"-bonus", "*bonus", XrmoptionNoArg, (caddr_t) "on"},
    {"-ccw", "*ccw", XrmoptionNoArg, (caddr_t) "on"},
    {"-suddenAppear", "*suddenAppear", XrmoptionNoArg, (caddr_t) "on"},
    {"-noGrid", "*noGrid", XrmoptionNoArg, (caddr_t) "on"},
    {"-scorefile", "*scoreFile", XrmoptionSepArg, (caddr_t) NULL},
    {"-u", "*playerName", XrmoptionSepArg, (caddr_t) NULL},
    {"-display", ".display", XrmoptionSepArg, (caddr_t) NULL},
    {"-geometry", "*geometry", XrmoptionSepArg, (caddr_t) NULL},
    {"-iconGeometry", "*iconGeometry", XrmoptionSepArg, (caddr_t) NULL},
    {"-background", "*background", XrmoptionSepArg, (caddr_t) NULL},
    {"-bg", "*background", XrmoptionSepArg, (caddr_t) NULL},
    {"-foreground", "*foreground", XrmoptionSepArg, (caddr_t) NULL},
    {"-fg", "*foreground", XrmoptionSepArg, (caddr_t) NULL},
    {"-bigFont", "*bigFont", XrmoptionSepArg, (caddr_t) NULL},
    {"-tinyFont", "*tinyFont", XrmoptionSepArg, (caddr_t) NULL},
    {"-xrm", NULL, XrmoptionResArg, (caddr_t) NULL}
};

static XrmDatabase cmmDB = (XrmDatabase) NULL, rDB = (XrmDatabase) NULL;

static void parseOpenDisp();
static void Usage();
static void getDefaults();

/* ------------------------------------------------------------------ */

int
main(argc, argv)
    int             argc;
    char           *argv[];
{
    sizehints.flags = PMinSize | PMaxSize | PPosition | PSize | USSize;
    iconsizehints.flags = PPosition | PSize;
    iconsizehints.width = iconsizehints.height = 48;

    parseOpenDisp(&argc, argv);
    getDefaults();
    inits(argc, argv);
    playing();
    /* never come to here */
    return 0;
}

/* ------------------------------------------------------------------ */

static void
parseOpenDisp(argc, argv)
    int            *argc;
    char           *argv[];
{
    struct passwd  *pw;
    XrmValue        value;
    char           *str_type[20];
    XVisualInfo     vtmp, *vinfo;
    int             n = 1;

    XrmInitialize();

    myDisplayName[0] = '\0';

    XrmParseCommand(&cmmDB, opTable, opTableEntries, "welltris", argc, argv);

    /* check for any arguments left */
    if (*argc != 1) {
	Usage(argv[0]);
    }
    /* only print out the scores? */
    if (XrmGetResource(cmmDB, "welltris.scoresOnly", "Welltris.ScoresOnly",
		       str_type, &value) == True) {
        if (XrmGetResource(cmmDB, "welltris.scoreFile", "Welltris.ScoreFile",
                       str_type, &value) == True) {
            (void) strncpy(scorefile, value.addr, ZLIM(value.size, FILENAMELEN));
            scorefile[FILENAMELEN - 1] = '\0';
        } else
            (void) strncpy(scorefile, SCOREFILE, FILENAMELEN);
	showScores(0);
	exit(0);
    }
    /* get display now */
    if (XrmGetResource(cmmDB, "welltris.display", "Welltris.Display",
		       str_type, &value) == True)
	(void) strncpy(myDisplayName, value.addr, ZLIM(value.size, 255));
    myDisplayName[255] = '\0';

    if (!(display = XOpenDisplay(myDisplayName))) {
	(void) fprintf(stderr, "%s: Can't open display '%s'\n",
		       argv[0], XDisplayName(myDisplayName));
	exit(1);
    }
    screen_num = DefaultScreen(display);
    visual = DefaultVisual(display, screen_num);
    colormap = DefaultColormap(display, screen_num);
    vtmp.visualid = XVisualIDFromVisual(visual);

    if ((vinfo = XGetVisualInfo(display, VisualIDMask, &vtmp, &n)) != NULL) {
	if ((vinfo->class == GrayScale) || (vinfo->class == StaticGray)) {
	    useColor = False;
	}
	XFree((char *) vinfo);
    } else {
	useColor = False;
    }

    /* setup user information */
    (void) gethostname(myscore.myhost, NAMELEN);

#ifdef VMS
    vmsusername = getenv("USER");
    vmsuserlen  = strlen(vmsusername);
    strncpy(myscore.myname, (const) vmsusername,vmsuserlen);
#else
    setpwent();
    pw = getpwuid(getuid());
    if (pw == NULL) {		/* impossible? */
	(void) sprintf(myscore.myname, "%d", (int) getuid());
	myHome[0] = '.';
	myHome[1] = '/';
	myHome[2] = '\0';
    } else {
	(void) strncpy(myscore.myname, pw->pw_name, NAMELEN);
	(void) strncpy(myHome, pw->pw_dir, FILENAMELEN);
    }
    endpwent();
#endif /* VMS */
    myscore.myname[NAMELEN - 1] = '\0';
    myscore.myhost[NAMELEN - 1] = '\0';
    myHome[FILENAMELEN - 1] = '\0';
}

/* ------------------------------------------------------------------ */

static void
Usage(argv0)
    char           *argv0;
{
    (void) fprintf(stderr,
	"Usage: %s [-s] [-l <starting level>]\n", argv0);
    (void) fprintf(stderr,
	"   [-n <squares in each polyomino>] [-showNext] [-beep]\n");
    (void) fprintf(stderr,
	"   [-diagonal] [-mixed] [-bonus] [-ccw] [-suddenAppear]\n");
    (void) fprintf(stderr,
	"    [-noGrid] [-scorefile <filename>]\n");
    (void) fprintf(stderr,
	"   [-u <name>] [-display <display>] [-geometry <geometry>]\n");
    (void) fprintf(stderr,
	"   [-iconGeometry <icon geometry>] [-fg <foreground>]\n");
    (void) fprintf(stderr,
	"   [-bg <background>] [-bigFont <font>] [-tinyFont <font>]\n");
    (void) fprintf(stderr,
	"   [-xrm <resource specifications>]\n");

    exit(1);
}

/* ------------------------------------------------------------------ */

static unsigned long
getGround(name)
    char            name[];
{
    XColor          tmp;

    if (!useColor) {
	/* The following does not catch all stuff that could be white */
	if ((strcmp(name, "white") == 0) || (strcmp(name, "White") == 0))
	    return WhitePixel(display, screen_num);
	else
	    return BlackPixel(display, screen_num);
    }
    if (XParseColor(display, colormap, name, &tmp) == 0) {
	(void) fprintf(stderr, "Tetris: invalid color '%s'.\n", name);
	return BlackPixel(display, screen_num);
    }
    if (XAllocColor(display, colormap, &tmp) == 0) {
	(void) fprintf(stderr, "Tetris: can't allocate color '%s'.\n", name);
	return BlackPixel(display, screen_num);
    }
    return tmp.pixel;
}

/* ------------------------------------------------------------------ */

static void
getDefaults()
{
    XrmDatabase     homeDB, serverDB, appDB;
    char            filenamebuf[FILENAMELEN];
    char           *filename = &filenamebuf[0];
    char           *env;
    char           *classname = "Welltris";
    char            name[255], geoStr[20], icongeoStr[20];
    XrmValue        value;
    char           *str_type[20];
    int             x, y;
    unsigned int    w, h;
    long            flags;

    (void) strcpy(name, "/usr/lib/X11/app-defaults/");
    (void) strcat(name, classname);

    /* Get application defaults file, if any */
    appDB = XrmGetFileDatabase(name);
    (void) XrmMergeDatabases(appDB, &rDB);

    if (XResourceManagerString(display) != NULL) {
	serverDB = XrmGetStringDatabase(XResourceManagerString(display));
    } else {
	(void) strcpy(filename, myHome);
	(void) strcat(filename, "/.Xdefaults");
	serverDB = XrmGetFileDatabase(filename);
    }
    XrmMergeDatabases(serverDB, &rDB);

    if ((env = getenv("XENVIRONMENT")) == NULL) {
	int             len;

	env = &filenamebuf[0];
	(void) strcpy(env, myHome);
	len = strlen(env);
	env[len++] = '/';
	(void) gethostname(env + len, FILENAMELEN - len);
    }
    homeDB = XrmGetFileDatabase(env);
    XrmMergeDatabases(homeDB, &rDB);

    XrmMergeDatabases(cmmDB, &rDB);

    /* starting levels */

    if (XrmGetResource(rDB, "welltris.startLevel", "Welltris.StartLevel",
		       str_type, &value) == True) {
	if ((sscanf(value.addr, "%d", &level) <= 0) ||
		(level < 0) || (level >= NUM_LEVELS)) {
	    (void) fprintf(stderr, "Welltris: Invalid level.\n");
	    exit(1);
	}
    }

    /* diagonal */

    if (XrmGetResource(rDB, "welltris.diagonal", "Welltris.Diagonal",
			str_type, &value) == True) {
	diagonal = True;
    }

    /* mixed */

    if (XrmGetResource(rDB, "welltris.mixed", "Welltris.Mixed",
			str_type, &value) == True) {
	mixed = True;
    }

    /* bonus */

    if (XrmGetResource(rDB, "welltris.bonus", "Welltris.Bonus",
			str_type, &value) == True) {
	bonus = True;
    }

    /* number of squares */

    if (XrmGetResource(rDB,
                       "welltris.numberSquares", "Welltris.NumberSquares",
		       str_type, &value) == True) {
	if (sscanf(value.addr, "%d", &squares) <= 0) {
	    (void) fprintf(stderr,
                "Welltris: Invalid number of squares for current modes.\n");
	    exit(1);
	}
    }
    if (!LEGAL(squares, diagonal, mixed, bonus)) { 
	    (void) fprintf(stderr,
                "Welltris: Invalid number of squares for current modes.\n");
	    exit(1);
    }

    /* show next */

    if (XrmGetResource(rDB, "welltris.showNext", "Welltris.ShowNext",
			str_type, &value) == True) {
	showNext = True;
    }

    /* beep */

    if (XrmGetResource(rDB, "welltris.beep", "Welltris.Beep",
			str_type, &value) == True) {
	beep = True;
    }

    /* ccw */

    if (XrmGetResource(rDB, "welltris.ccw", "Welltris.Ccw",
                        str_type, &value) == True) {
        cw = False;
    }

    /* sudden appearance of polyomino */

    if (XrmGetResource(rDB, "welltris.suddenAppear", "Welltris.SuddenAppear",
                       str_type, &value) == True) {
        gradualAppear = False;
    }

    /* no grid */

    if (XrmGetResource(rDB, "welltris.noGrid", "Welltris.NoGrid",
                        str_type, &value) == True) {
        grid = False;
    }

    /* score file */

    if (XrmGetResource(rDB, "welltris.scoreFile", "Welltris.ScoreFile",
                       str_type, &value) == True) {
        (void) strncpy(scorefile, value.addr, ZLIM(value.size, FILENAMELEN));
        scorefile[FILENAMELEN - 1] = '\0';
    } else
        (void) strncpy(scorefile, SCOREFILE, FILENAMELEN);

    /* player name */

    if (XrmGetResource(rDB, "welltris.playerName", "Welltris.PlayerName",
		       str_type, &value) == True) {
	(void) strncpy(myscore.myname, value.addr, ZLIM(value.size, 20));
	myscore.myname[19] = '\0';
    }

    /*** get foreground/background colors ***/

    if (XrmGetResource(rDB, "welltris.foreground", "Welltris.Foreground",
		       str_type, &value) == True) {
	(void) strncpy(name, value.addr, ZLIM(value.size, 255));
	name[254] = '\0';
	fg = getGround(name);
    } else
	fg = BlackPixel(display, screen_num);

    if (XrmGetResource(rDB, "welltris.background", "Welltris.Background",
		       str_type, &value) == True) {
	(void) strncpy(name, value.addr, ZLIM(value.size, 255));
	name[254] = '\0';
	bg = getGround(name);
    } else
	bg = WhitePixel(display, screen_num);

    if (bg == fg) {
	bg = WhitePixel(display, screen_num);
	fg = BlackPixel(display, screen_num);
    }

    /*** get geometry info ***/

    if (XrmGetResource(rDB, "welltris.geometry", "Welltris.Geometry",
		       str_type, &value) == True) {
	(void) strncpy(geoStr, value.addr, ZLIM(value.size, 20));
	geoStr[19] = '\0';
    } else {
	geoStr[0] = '\0';
    }

    flags = XParseGeometry(geoStr, &x, &y, &w, &h);
    if ((WidthValue | HeightValue) & flags)
	Usage("welltris");

    if (XValue & flags) {
	if (XNegative & flags)
	    x = DisplayWidth(display, screen_num) + x - sizehints.width;
	sizehints.x = x;
    }
    if (YValue & flags) {
	if (YNegative & flags)
	    y = DisplayHeight(display, screen_num) + y - sizehints.height;
	sizehints.y = y;
    }

    /*** get icon geometry info ***/

    if (XrmGetResource(rDB, "welltris.iconGeometry", "Welltris.IconGeometry",
		       str_type, &value) == True) {
	(void) strncpy(icongeoStr, value.addr, ZLIM(value.size, 20));
	icongeoStr[19] = '\0';
    } else {
	icongeoStr[0] = '\0';
    }

    flags = XParseGeometry(icongeoStr, &x, &y, &w, &h);
    if ((WidthValue | HeightValue) & flags)
	Usage("welltris");

    if (XValue & flags) {
	if (XNegative & flags)
	    x = DisplayWidth(display, screen_num) + x - iconsizehints.width;
	wmhints.flags |= IconPositionHint;
	wmhints.icon_x = x;
	iconsizehints.x = x;
    }
    if (YValue & flags) {
	if (YNegative & flags)
	    y = DisplayHeight(display, screen_num) + y - iconsizehints.height;
	wmhints.flags |= IconPositionHint;
	wmhints.icon_y = y;
	iconsizehints.y = y;
    }

    /*** get fonts ***/

    if (XrmGetResource(rDB, "welltris.bigFont", "Welltris.BigFont",
		       str_type, &value) == True) {
	(void) strncpy(name, value.addr, ZLIM(value.size, 255));
	name[254] = '\0';
    } else {
	(void) strcpy(name, BIGFONT);
    }
    if ((bigFont = XLoadQueryFont(display, name)) == NULL) {
	(void) fprintf(stderr, "Welltris: can't open font '%s'.\n", name);
	exit(1);
    }
    if (XrmGetResource(rDB, "welltris.tinyFont", "Welltris.TinyFont",
		       str_type, &value) == True) {
	(void) strncpy(name, value.addr, ZLIM(value.size, 255));
	name[254] = '\0';
    } else {
	(void) strcpy(name, TINYFONT);
    }
    if ((tinyFont = XLoadQueryFont(display, name)) == NULL) {
	(void) fprintf(stderr, "Welltris: can't open font '%s'.\n", name);
	exit(1);
    }

    /*
     * clean up
     */
    XrmDestroyDatabase(rDB);
}

/* ------------------------------------------------------------------ */
