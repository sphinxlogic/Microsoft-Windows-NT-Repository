/*****************************************************************************
 *
 * FACILITY:
 *   XPool	DECwindows Pool game
 *	
 * ABSTRACT:
 *   This module contains the routines which process the Xdefault parameters
 *   which are used to set default values for various settings.
 *
 * AUTHOR:
 *   Doug Stefanelli
 *
 * CREATION DATE: 1-October-1989
 *
 * Edit History 
 *
 *  DS	20-Dec-1989	Original version
 *
 */

/*
 * Routines in this module
 */
void get_defaults();			/* returns basic xdefault info */
void get_default_colors();		/* returns color xdefault info */
static void CvtStringToToggle();	/* parses ON/OFF parameters */
static void CvtStringToGame();		/* parses game name parameter */
static void CvtStringToOpponent();	/* parses opponent type parameter */
static void CvtStringToNetwork();	/* parses network type parameter */	
static void CvtStringToColor();		/* parses color specifying parameters */
static void install_converter();	/* installs conversion routines */
static void get_defdir();		/* gets path to resource file */

/*
 * Include files
 */
#ifdef VMS
#include "decw$include:DwtWidget.h"
#include "decw$include:DwtAppl.h"
#include "decw$include:Xlib.h"
#include "decw$include:Xresource.h"
#include "decw$include:StringDefs.h"
#else
#include "X11/DwtWidget.h"
#include "X11/DwtAppl.h"
#include "X11/Xlib.h"
#include "X11/Xresource.h"
#include "X11/StringDefs.h"
#endif
#include "xpool.h"

/*
 * Local variables
 */
static int def_straight_limit = 50;
static int def_computer_skill = 50;
static Position def_pos = NOVALUE;

#ifdef VMS
#define RESDIR "decw$user_defaults"
#define RESFILE "xpool.dat"
#else
#define RESDIR "XAPPLRESDIR"
#define RESDIR_ALT "HOME"
#define RESFILE "default.Xpool"
#endif

typedef struct {
    int game;
    int opponent;
    Position x;
    Position y;
    int skill;
    int limit;
    int network_type;
    Boolean preset_velocity;
    Boolean confirm_shots;
    Boolean sound;
} GameRes, *GameResptr;

static GameRes game_res;

static XtResource game_res_list[] = {
    { "game", "Game", "RGame", sizeof(int), XtOffset(GameResptr, game),
	XtRImmediate, (caddr_t)EIGHT_BALL},
    { "opponent", "Opponent", "ROpponent", sizeof(int),
	XtOffset(GameResptr, opponent), XtRImmediate, (caddr_t)HUMAN},
    { "x", XtCX, XtRPosition, sizeof(Position), XtOffset(GameResptr, x),
	XtRPosition, (caddr_t)&def_pos},
    { "y", XtCY, XtRPosition, sizeof(Position), XtOffset(GameResptr, y),
	XtRPosition, (caddr_t)&def_pos},
    { "skill", "Skill", XtRInt, sizeof(int), XtOffset(GameResptr, skill),
	XtRInt, (caddr_t)&def_computer_skill},
    { "limit", "Limit", XtRInt, sizeof(int), XtOffset(GameResptr, limit),
	XtRInt, (caddr_t)&def_straight_limit},
    { "network", "Network", "RNetwork", sizeof(int),
	XtOffset(GameResptr, network_type), XtRImmediate, (caddr_t)LOCAL},
    { "presetVelocity", "PresetVelocity", "RToggle", sizeof(Boolean),
	XtOffset(GameResptr, preset_velocity), XtRImmediate, (caddr_t)FALSE},
    { "confirmShots", "ConfirmShots", "RToggle", sizeof(Boolean),
	XtOffset(GameResptr, confirm_shots), XtRImmediate, (caddr_t)FALSE},
    { "sound", "Sound", "RToggle", sizeof(Boolean),
	XtOffset(GameResptr, sound), XtRImmediate, (caddr_t)FALSE}
};

typedef struct {
    XColor background;
    XColor foreground;
    XColor felt;
    XColor table;
    XColor ball[max_balls];
} ColorRes, *ColorResptr;

static ColorRes color_res[MAX_SCREENS];

static char *color[] = {
	"Black",
	"White",
	"Medium Sea Green",
	"Brown",
	"Yellow",
	"Blue",
	"Red",
	"Blue Violet",
	"Orange",
	"Green",
	"Maroon"};

static int color_index[] = {0, 1, 2, 3, 1, 4, 5, 6, 7, 8, 9, 10,
					0, 4, 5, 6, 7, 8, 9, 10};
static int mono_index[] =  {0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
					1, 1, 1, 1, 1, 1, 1, 1};

static XtResource color_res_list[] = {
    { "background", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, background), XtRString, "Black"},
    { "foreground", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, foreground), XtRString, "White"},
    { "felt", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, felt), XtRString, "Medium Sea Green"},
    { "table", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, table), XtRString, "Brown"},
    { "cueBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[0]), XtRString, "White"},
    { "oneBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[1]), XtRString, "Yellow"},
    { "twoBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[2]), XtRString, "Blue"},
    { "threeBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[3]), XtRString, "Red"},
    { "fourBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[4]), XtRString, "Blue Violet"},
    { "fiveBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[5]), XtRString, "Orange"},
    { "sixBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[6]), XtRString, "Green"},
    { "sevenBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[7]), XtRString, "Maroon"},
    { "eightBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[8]), XtRString, "Black"},
    { "nineBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[9]), XtRString, "Yellow"},
    { "tenBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[10]), XtRString, "Blue"},
    { "elevenBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[11]), XtRString, "Red"},
    { "twelveBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[12]), XtRString, "Blue Violet"},
    { "thirteenBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[13]), XtRString, "Orange"},
    { "fourteenBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[14]), XtRString, "Green"},
    { "fifteenBall", "CColor", "RColor", sizeof(XColor),
	XtOffset(ColorResptr, ball[15]), XtRString, "Maroon"}
};

/*
 * External variables
 */
extern Widget toplevel_widget[];

void get_defaults(s, game, opponent, skill, limit, network_type,
	     preset_velocity, confirm_shots, sound, x, y)
int *game;
int *opponent;
int *skill;
int *limit;
int *network_type;
unsigned char *preset_velocity;
unsigned char *confirm_shots;
unsigned char *sound;
int *x, *y;
{
    Arg arglist[1];
    int i;
    XrmDatabase db, sdb;
    char resfile[80];

    db = XtDatabase(XtDisplay(toplevel_widget[s]));
    install_converter();
    get_defdir(resfile);
    strcat(resfile, RESFILE);
    sdb = XrmGetFileDatabase(resfile);
    if (sdb != NULL) XrmMergeDatabases(sdb, &db);

    XtGetApplicationResources(toplevel_widget[s], &game_res, game_res_list,
			XtNumber(game_res_list), arglist, 0);

    *game = game_res.game;
    *opponent = game_res.opponent;
    *skill = game_res.skill;
    *limit = game_res.limit;
    *network_type = game_res.network_type;
    *preset_velocity = game_res.preset_velocity;
    *confirm_shots = game_res.confirm_shots;
    *sound = game_res.sound;
    *x = game_res.x;
    *y = game_res.y;
}

void get_default_colors(s, monochrome, background, foreground, felt, table, ball)
unsigned char monochrome;
XColor **background;
XColor **foreground;
XColor **felt;
XColor **table;
XColor **ball;
{
    Arg arglist[1];
    int i;

    if (monochrome)
	for (i=0; i<XtNumber(mono_index); i++)
	    color_res_list[i].default_addr = color[mono_index[i]];
    else
	for (i=0; i<XtNumber(color_index); i++)
	    color_res_list[i].default_addr = color[color_index[i]];

    XtGetApplicationResources(toplevel_widget[s], &color_res[s],
		color_res_list, XtNumber(color_res_list), arglist, 0);

    *background = &color_res[s].background;
    *foreground = &color_res[s].foreground;
    *felt = &color_res[s].felt;
    *table = &color_res[s].table;
    for (i=0; i<max_balls; i++)
	*(ball+i) = &color_res[s].ball[i];
}

static void CvtStringToToggle(args, num_args, fromVal, toVal)
XrmValue *args, *fromVal, *toVal;
Cardinal *num_args;
{
    int i;
    static Boolean retval;
    char tmp[100];

    strcpy(tmp, (char *)fromVal->addr);
    for (i=0; i<fromVal->size; i++)
	tmp[i] = toupper(tmp[i]);

    if ( strcmp(tmp, "ON") == 0 || strcmp(tmp, "YES") == 0)
	retval = TRUE;
    else
	retval = FALSE;

    toVal->addr = (caddr_t) &retval;
    toVal->size = sizeof (Boolean);
}

static void CvtStringToGame(args, num_args, fromVal, toVal)
XrmValue *args, *fromVal, *toVal;
Cardinal *num_args;
{
    int i;
    static int retval;
    char tmp[100];

    strcpy(tmp, (char *)fromVal->addr);
    for (i=0; i<fromVal->size; i++)
	tmp[i] = toupper(tmp[i]);

    if ( strcmp(tmp, "STRAIGHTPOOL") == 0 || strcmp(tmp, "STRAIGHT POOL") == 0)
	retval = STRAIGHT_POOL;
    else if (strcmp(tmp, "NINEBALL") == 0 || strcmp(tmp, "NINE BALL") == 0)
	retval = NINE_BALL;
    else if (strcmp(tmp, "ROTATION") == 0)
	retval = ROTATION;
    else
	retval = EIGHT_BALL;

    toVal->addr = (caddr_t) &retval;
    toVal->size = sizeof (int);
}

static void CvtStringToOpponent(args, num_args, fromVal, toVal)
XrmValue *args, *fromVal, *toVal;
Cardinal *num_args;
{
    int i;
    static int retval;
    char tmp[100];

    strcpy(tmp, (char *)fromVal->addr);
    for (i=0; i<fromVal->size; i++)
	tmp[i] = toupper(tmp[i]);

    if ( strcmp(tmp, "COMPUTER") == 0 )
	retval = COMPUTER;
    else if (strcmp(tmp, "NETWORK") == 0)
	retval = NETWORK;
    else if (strcmp(tmp, "AUTOMATIC") == 0)
	retval = SELF;
    else if (strcmp(tmp, "AUTONET") == 0)
	retval = AUTONET;
    else if (strcmp(tmp, "AUTOLOCK") == 0)
	retval = AUTOLOCK;
    else
	retval = HUMAN;

    toVal->addr = (caddr_t) &retval;
    toVal->size = sizeof (int);
}

static void CvtStringToNetwork(args, num_args, fromVal, toVal)
XrmValue *args, *fromVal, *toVal;
Cardinal *num_args;
{
    int i;
    static int retval;
    char tmp[100];

    strcpy(tmp, (char *)fromVal->addr);
    for (i=0; i<fromVal->size; i++)
	tmp[i] = toupper(tmp[i]);

    if (strcmp(tmp, "REMOTE") == 0 || strcmp(tmp, "WIDE") == 0 ||
	strcmp(tmp, "WIDEAREA") == 0 || strcmp(tmp, "WIDE AREA") == 0)
	retval = REMOTE;
    else
	retval = LOCAL;

    toVal->addr = (caddr_t) &retval;
    toVal->size = sizeof (int);
}

static void CvtStringToColor(args,num_args,fromVal,toVal)
XrmValue *args,*fromVal,*toVal;
Cardinal *num_args;
{
    int i;
    static XColor retval;
    Screen *screen;
    Colormap colormap;

    screen = *((Screen **) args[0].addr);
    colormap = *((Colormap *) args[1].addr);

    XParseColor (DisplayOfScreen(screen), colormap,
		    (char *)fromVal->addr , &retval);

    toVal->addr = (caddr_t) &retval;
    toVal->size = sizeof (XColor);
}

static void install_converter()
{
    static XtConvertArgRec colorConvertArgs[] = {
	{XtBaseOffset, (caddr_t) XtOffset(Widget, core.screen), sizeof(Screen *)},
	{XtBaseOffset, (caddr_t) XtOffset(Widget, core.colormap), sizeof(Colormap)}
	};
    XtAddConverter(XtRString, "RToggle", CvtStringToToggle, NULL, 0);
    XtAddConverter(XtRString, "RGame", CvtStringToGame, NULL, 0);
    XtAddConverter(XtRString, "ROpponent", CvtStringToOpponent, NULL, 0);
    XtAddConverter(XtRString, "RNetwork", CvtStringToNetwork, NULL, 0);
    XtAddConverter(XtRString, "RColor", CvtStringToColor, colorConvertArgs,
			XtNumber(colorConvertArgs));
}

static void get_defdir(path)
char *path;
{
    char *buf;
#ifdef VMS
    buf = getenv(RESDIR);
    if (buf != NULL) {
	strcpy(path, RESDIR);
	strcat(path, ":");
    } else
	path[0] = 0;
#else
    buf = (char *) getenv(RESDIR);
    if (buf != NULL) {
	strcpy(path, buf);
	strcat(path, "/");
    } else {
	buf = (char *) getenv(RESDIR_ALT);
	if (buf != NULL) {
	    strcpy(path, buf);
	    strcat(path, "/");
	} else
	    path[0] = 0;
    }
#endif
}
