/*****************************************************************************
 *
 * FACILITY:
 *   XPool	DECwindows Pool game
 *	
 * ABSTRACT:
 *   This module contains the routines which interact with the DECwindows
 *   Toolkit.  It includes all of the widgets and their callbacks.
 *
 * AUTHOR:
 *   Doug Stefanelli
 *
 * CREATION DATE: 1-October-1989
 *
 * Edit History 
 *
 *  DS	1-Oct-1989	Original version
 *  DS	4-Dec-1989	Modified to run from XtMainLoop()
 *  DS	13-Mar-1990	Support 2 displays.  Add Ultrix support.  Modify
 *			velocity select widget - set spin by selecting a
 *			position on the cue ball.
 *
 */

/*
 * Routines included in this module
 */
static void handle_exposures();		/* exposure event callback */
void enable_button();			/* activate mouse button */
void disable_button();			/* deactivate mouse button */
void got_button();			/* process button events */
void got_position();			/* process button/cursor position info */
void enable_game_options();		/* activate options for during game */
void enable_select_options();		/* activate options for selecting play */
void enable_respot();			/* activate respot option */
void disable_respot();			/* deactivate respot option */
static void frame_complete();		/* frame timeout occurance callback */
void set_frame_timeout();		/* enable frame timeout */
void cancel_frame_timeout();		/* cancel frame timeout */
static void delay_complete();		/* delay timeout occurance callback */
void set_delay_timeout();		/* enable delay timeout */
void cancel_delay_timeout();		/* cancel delay timeout */
int init_screen();			/* initialize a display */
void destroy_remote_screen();		/* destroy a remote display */
void remove_menus();			/* take down any popup menus */
static void challenge_response();	/* challenge widget callback */
int accept_challenge();			/* set up challenge widget */
static void select_net();		/* net type widget callback */
static void string_changed();		/* node string processing callback */
void get_node();			/* set up challenge node widget */
static void set_skill();		/* skill level widget callback */
void get_skill();			/* set up skill level widget */
static void set_limit();		/* straight pool limit widget callback */
void get_limit();			/* set up straight pool limit widget */
static void menu_select();		/* menu selection widget callback */
void select_menu();			/* set up option menu selection widget */
static void velocity_select();		/* velocity selection widget callback */
void get_velocity();			/* set up velocity selection widget */
static void provide_help();		/* help widget display callback */
static void hide_help();		/* help widget removal callback */
void display_about();			/* set up about widget */
void display_help();			/* set up help widget */
void select_play();			/* select play main loop */
static void set_control();		/* control menubar widget callback */
static void select_option();		/* option menubar widget callback */
static void select_opponent();		/* opponent menubar widget callback */
static void select_game();		/* game menubar widget callback */
static void window_menubar();		/* set up menuar widget */
void open_window();			/* create & open graphics window widget */

/*
 * Include files
 */
#include <stdio.h>
#include "xpool.h"
#include "xpool_icon.h"
#include "xpool_help.h"
#ifdef VMS
#include <decw$include:DwtAppl.h>
#include <decw$include:Vendor.h>
#include <cda$def.h>
#else
#include <X11/DwtAppl.h>
#include <X11/Vendor.h>
#include <cda_def.h>
#endif

/*
 * Local variables
 */
static char *opponent_string[] = {
	"Two Player",
	"vs. Computer",
	"Network",
	"Automatic",
	"AutoNet",
	"AutoLock"
	};

static char *game_string[] = {
	"Straight Pool",
	"8 Ball",
	"9 Ball",
	"Rotation"
	};

static char *control_string[] = {
	"Start Game",
	"End Game",
	"Quit"
	};

static char *option_string[] = {
	"Preset Velocity",
	"Confirm Shots",
	"Set Computer Skill",
	"Straight Pool Limit",
	"Respot Ball"
	};

#define PRESET_VELOCITY		0
#define CONFIRM_SHOTS		1
#define COMPUTER_SKILL		2
#define STRAIGHT_LIMIT		3
#define RESPOT_BALL		4

static char *help_string[] = {
	"About",
	"More About"
	};

static XtAppContext application_context;	/* application context */

#define num_opps	XtNumber(opponent_string)
#define num_games	XtNumber(game_string)
#define num_controls	XtNumber(control_string)
#define num_options	XtNumber(option_string)
#define num_helps	XtNumber(help_string)

static Widget window_widget[MAX_SCREENS];	/* graphics window widget */
static Widget dialogbox_widget[MAX_SCREENS];	/* highest level dialog box */
static Widget helpbox_widget[MAX_SCREENS];	/* help text dialog box */
static Widget aboutbox_widget[MAX_SCREENS];	/* help/about dialog box */
static Widget controlitem_widget[MAX_SCREENS][num_controls];/* control options */
static Widget optionitem_widget[MAX_SCREENS][num_options];	/* misc. options */
static Widget menubox_widget[MAX_SCREENS];	/* hi/lo select menu dialog box */
static Widget velocitybox_widget[MAX_SCREENS];	/* velocity select dialog box */
static Widget velocity_widget[MAX_SCREENS];	/* velocity select slider widget */
static Widget spin_widget[MAX_SCREENS];		/* spin select window widget */
static Widget game_widget[MAX_SCREENS];		/* game select menu bar option */
static Widget opponent_widget[MAX_SCREENS];	/* opponent select menu bar option */

static Widget skillbox_widget;			/* set computer skill dialog box */
static Widget limitbox_widget;			/* set straight pool limit dialog box */
static Widget nodebox_widget;			/* challenge request dialog box */
static Widget challengebox_widget;		/* Network challenge dialog box */

#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))

static char *xpool_help;
static DwtFontList mnfont[MAX_SCREENS], abfont[MAX_SCREENS];
static XtIntervalId frame_timeout = NULL;
static XtIntervalId delay_timeout = NULL;
static unsigned char respot_ball;
static int x_offset[MAX_SCREENS];
static int y_offset[MAX_SCREENS];
static int num_velocities = 0;
static GC dgc[MAX_SCREENS];

/*
 * Global variables
 */
Widget toplevel_widget[MAX_SCREENS];	/* Root widget ID of our application. */
int num_screens = 0;			/* number of displays currently active */

int position;				/* button/cursor position variables */
int position_mask;
int pointer_x, pointer_y;

int game;				/* these params can be set via XRM */
int opponent;
int network_type;
int computer_skill;
int limit;
unsigned char sound[MAX_SCREENS];
unsigned char preset_velocity[MAX_SCREENS];
unsigned char confirm_shots[MAX_SCREENS];

int menu_selection;			/* these params are accessed by the */
int remote_velocity;			/* program upon completion of widget */
int velocity;				/* processing */
int roll;
int spin;
unsigned char abort_flag = 0;
unsigned char challenge_accepted;
unsigned char button_active = FALSE;
char node[35];

/*
 * External variables
 */
extern unsigned char action_started;
extern int system;

/*
 * External routines
 */
extern get_window();
extern void redraw_display();

/*******************************************************************************
 *
 * Exposure handling callback for the window widget
 *
 ******************************************************************************/

static void handle_exposures(w, tag, data)
Widget w;
DwtWindowCallbackStruct *data;
{
    int s;

    for (s=0; s<num_screens; s++) {
	if (XtDisplay(w) == XtDisplay(toplevel_widget[s])) {
	    redraw_display(s, data->event);
	    break;
	}
    }
}

/*******************************************************************************
 *
 * Button handling routines
 *
 ******************************************************************************/

void enable_button(mask)
{
    button_active = TRUE;
    position_mask = mask;
    if (position_mask & RESPOT_BALL)
	enable_respot();
}

void disable_button()
{
    button_active = FALSE;
    disable_respot();
}

void got_button(w, tag, event)
Widget w;
XEvent *event;
{
    int s;

    for (s=0; s<num_screens; s++)
	if (XtDisplay(w) == XtDisplay(toplevel_widget[s])) {
	    if (event->xbutton.button == Button1)
		if (button_active && system == s)
		    got_position(event->xbutton.x, event->xbutton.y);
	    break;
	}
}

void got_position(x, y)
{
    position = 0;

    if ((position_mask & TABLE) &&
	(x > left_edge-border && x < right_edge+border &&
	 y > bottom_edge-border && y < top_edge+border))
	position = OVER_TABLE;
    else if ((position_mask & FELT) &&
	(x > left_edge+radius && x < right_edge-radius &&
	 y > bottom_edge+radius && y < top_edge-radius))
	position = OVER_FELT;
    else if ((position_mask & HEAD) &&
	(x > left_edge+radius && x < break_point &&
	 y > bottom_edge+radius && y < top_edge-radius))
	position = OVER_HEAD;
    else if ((position_mask & BALLRACK) &&
	(x > left_edge-diameter && x < right_edge+diameter &&
	 y > windowH-diameter-2 && y < windowH))
	position = OVER_BALLRACK;

    if (position != 0) {	/* we got a valid position, so continue */
	pointer_x = x;
	pointer_y = y;
	pool_table();
    }
}

/*******************************************************************************
 *
 * Widget sensitivity select routines
 *
 ******************************************************************************/

void enable_game_options()
{
    int s;

    for (s=0; s<num_screens; s++) {
	XtSetSensitive(controlitem_widget[s][0], FALSE); /* no start game */
	XtSetSensitive(controlitem_widget[s][1], TRUE);  /* allow end game */
	XtSetSensitive(optionitem_widget[s][COMPUTER_SKILL], FALSE); /* no computer skill set */
	XtSetSensitive(optionitem_widget[s][STRAIGHT_LIMIT], FALSE); /* no straight limit set */
	XtSetSensitive(optionitem_widget[s][RESPOT_BALL], FALSE);    /* no ball respot */
	XtSetSensitive(game_widget[s], FALSE);		 /* no game selection */
	XtSetSensitive(opponent_widget[s], FALSE);	 /* no opponent selection */
    }
}

void enable_select_options()
{
    int s;

    for (s=0; s<num_screens; s++) {
	XtSetSensitive(controlitem_widget[s][0], TRUE);  /* allow start game */
	XtSetSensitive(controlitem_widget[s][1], FALSE); /* no end game */
	XtSetSensitive(optionitem_widget[s][COMPUTER_SKILL], TRUE); /* allow computer skill set */
	XtSetSensitive(optionitem_widget[s][STRAIGHT_LIMIT], TRUE); /* allow straight limit set */
	XtSetSensitive(optionitem_widget[s][RESPOT_BALL], FALSE);   /* no ball respot */
	XtSetSensitive(game_widget[s], TRUE);		 /* allow game selection */
	XtSetSensitive(opponent_widget[s], TRUE);	 /* allow opponent selection */
    }
    abort_flag = 0;
}

void enable_respot()		/* allow ball respot */
{
    XtSetSensitive(optionitem_widget[system][RESPOT_BALL], TRUE);
    respot_ball = FALSE;
}

void disable_respot()	/* disable ball respot */
{
    XtSetSensitive(optionitem_widget[system][RESPOT_BALL], FALSE);
    respot_ball = FALSE;
}

/*******************************************************************************
 *
 * Timing routines
 *
 ******************************************************************************/

static void frame_complete()
{
    frame_timeout = NULL;
    pool_table();
}

void set_frame_timeout(milliseconds)
int milliseconds;
{
    frame_timeout = XtAppAddTimeOut(application_context, milliseconds, frame_complete, 0);
}

void cancel_frame_timeout()
{
    if (frame_timeout != NULL) {
	XtRemoveTimeOut(frame_timeout);
	frame_timeout = NULL;
    }
}

static void delay_complete()
{
    delay_timeout = NULL;
    pool_table();
}

void set_delay_timeout(seconds)
int seconds;
{
    delay_timeout = XtAppAddTimeOut(application_context, 1000*seconds, delay_complete, 0);
}

void cancel_delay_timeout()
{
    if (delay_timeout != NULL) {
	XtRemoveTimeOut(delay_timeout);
	delay_timeout = NULL;
    }
}

/*******************************************************************************
 *
 * Widget initialization and termination routines
 *
 ******************************************************************************/

int init_screen(node_string)
char *node_string;
{
    Display *display;
    int ignore, s, i, len;
    int zero = 0;

    if (node_string == 0) {
	s = LOCAL;
	XtToolkitInitialize();
	application_context = XtCreateApplicationContext();
	for (i=0, len=0; i<XtNumber(xpool_help_strings); i++)
	    len += strlen(xpool_help_strings[i]);
	xpool_help = (char *)malloc(len);
	if (xpool_help == 0)
	    xpool_help = "No help available";
	else {
	    strcpy(xpool_help, xpool_help_strings[0]);
	    for (i=1; i<XtNumber(xpool_help_strings); i++)
		strcat(xpool_help, xpool_help_strings[i]);
	}
    } else
	s = REMOTE;

    display = XtOpenDisplay(
		application_context,	/* application context */
		node_string,		/* remote node string */
		header,			/* Main window banner text. */
		"Xpool",		/* Root class name. */
		NULL,			/* No option list. */
		0,			/* Number of options. */
		&zero,			/* Address of arg count */
		0);			/* arg list */

    if (display == 0) return 0;
    num_screens++;

    toplevel_widget[s] = XtAppCreateShell(
		header,
		"Xpool",
		applicationShellWidgetClass,
		display,
		NULL,
		0);

    /*
     * Clear all persistant widgets so they are recreated if they are reused.
     */
    menubox_widget[s] = NULL;
    velocitybox_widget[s] = NULL;
    aboutbox_widget[s] = NULL;
    helpbox_widget[s] = NULL;

    if (s == LOCAL) {
	get_defaults(s, &game, &opponent, &computer_skill, &limit,
		 &network_type, &preset_velocity[s], &confirm_shots[s],
		 &sound[s], &x_offset[s], &y_offset[s]);
	nodebox_widget = NULL;
	skillbox_widget = NULL;
	limitbox_widget = NULL;
    } else
	get_defaults(s, &ignore, &ignore, &ignore, &ignore, &ignore,
		 &preset_velocity[s], &confirm_shots[s], &sound[s],
		 &x_offset[s], &y_offset[s]);

    return 1;
}

void destroy_remote_screen()
{
    XtCloseDisplay(XtDisplay(toplevel_widget[REMOTE]));
    XtUnrealizeWidget(toplevel_widget[REMOTE]);
    XtDestroyWidget(toplevel_widget[REMOTE]);
    num_screens--;
}

void remove_menus()
{
    num_velocities = 0;
    if (velocitybox_widget[LOCAL] != NULL && XtIsManaged(velocitybox_widget[LOCAL]))
	XtUnmanageChild(velocitybox_widget[LOCAL]);
}

/*******************************************************************************
 *
 * Ask for acceptance of the challenge on the remote node
 *
 ******************************************************************************/

static void challenge_response(w, tag, data)
Widget w;
DwtAnyCallbackStruct *data;
{
    XtDestroyWidget(challengebox_widget);
    challenge_accepted = tag;
    pool_table();
}

int accept_challenge(s, string)
char *string;
{
    XEvent event;
    Arg arglist[10];
    Widget accept_widget;
    Widget refuse_widget;
    Widget label_widget;
    int n;

    n = 0;
    XtSetArg(arglist[n], DwtNunits, DwtFontUnits);		n++;
    XtSetArg(arglist[n], DwtNresize, DwtResizeShrinkWrap);	n++;
    XtSetArg(arglist[n], DwtNborderWidth, 0);			n++;
    XtSetArg(arglist[n], DwtNmarginWidth, 20);			n++;
    XtSetArg(arglist[n], DwtNmarginHeight, 10);			n++;
    XtSetArg(arglist[n], DwtNstyle, DwtModeless);		n++;
    challengebox_widget = DwtDialogBoxPopupCreate(toplevel_widget[s], header, arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNx, 4);				n++;
    XtSetArg(arglist[n], DwtNy, 4);				n++;
    XtSetArg(arglist[n], DwtNlabel, DwtLatin1String(string));	n++;
    label_widget = DwtLabelCreate(challengebox_widget, "Label", arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNbordHighlight, TRUE);		n++;
    XtSetArg(arglist[n], DwtNx, 50);				n++;
    XtSetArg(arglist[n], DwtNy, 25);				n++;
    accept_widget = DwtPushButtonCreate(challengebox_widget, "Accept", arglist, n);
    XtAddCallback(accept_widget, DwtNactivateCallback, challenge_response, 1);

    n = 0;
    XtSetArg(arglist[n], DwtNbordHighlight, TRUE);		n++;
    XtSetArg(arglist[n], DwtNx, 120);				n++;
    XtSetArg(arglist[n], DwtNy, 25);				n++;
    refuse_widget = DwtPushButtonCreate(challengebox_widget, "Refuse", arglist, n);
    XtAddCallback(refuse_widget, DwtNactivateCallback, challenge_response, 0);

    XtMoveWidget(challengebox_widget, 100, 100);

    XtManageChild(accept_widget);
    XtManageChild(refuse_widget);
    XtManageChild(label_widget);
    XtManageChild(challengebox_widget);
    XtRealizeWidget(challengebox_widget);

    if (s == 0)
	XtAppMainLoop(application_context);
    return;    
}

/*******************************************************************************
 *
 * Get the name of the node to challenge
 *
 ******************************************************************************/

static void select_net(w, tag, data)
Widget w;
DwtTogglebuttonCallbackStruct *data;
{
    if (data->value)
	network_type = tag;
}

static void string_changed(w, tag, data)
Widget w;
DwtAnyCallbackStruct *data;
{
    int string_len;
    char *new_string;

    new_string = DwtSTextGetString(w);
    if (strchr(new_string, '\n') != 0) {
	string_len = (int)strchr(new_string,'\n') - (int)new_string;
	if (string_len > 35) string_len = 35;
	strncpy(node, new_string, string_len);
	node[string_len] = '\0';
	XtUnmanageChild(nodebox_widget);
	DwtSTextSetString(w, "");
	enable_game_options();
	pool_table();
    }
}

void get_node()
{
    Arg arglist[10];
    Widget text_widget;		/* Simple text widget */
    Widget label_widget;	/* label widget */
    Widget nettype_widget;
    Widget netitem_widget[2];
    int n;

    if (nodebox_widget != NULL) {
	XtManageChild(nodebox_widget);
	return;
    }

    n = 0;
    XtSetArg(arglist[n], DwtNunits, DwtFontUnits);		n++;
    XtSetArg(arglist[n], DwtNtakeFocus, TRUE);			n++;
    XtSetArg(arglist[n], DwtNresize, DwtResizeShrinkWrap);	n++;
    XtSetArg(arglist[n], DwtNborderWidth, 0);			n++;
    XtSetArg(arglist[n], DwtNmarginWidth, 20);			n++;
    XtSetArg(arglist[n], DwtNmarginHeight, 10);			n++;
    XtSetArg(arglist[n], DwtNstyle, DwtModal);			n++;
    XtSetArg(arglist[n], DwtNdefaultPosition, TRUE);		n++;
    nodebox_widget = DwtDialogBoxPopupCreate(dialogbox_widget[LOCAL], header, arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNx, 4);				n++;
    XtSetArg(arglist[n], DwtNy, 4);				n++;
    XtSetArg(arglist[n], DwtNlabel,
	DwtLatin1String("Which node would you like to challenge?"));	n++;
    label_widget = DwtLabelCreate(nodebox_widget, "Label", arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNx, 4);				n++;
    XtSetArg(arglist[n], DwtNy, 16);				n++;
    XtSetArg(arglist[n], DwtNcols, 35);				n++;
    XtSetArg(arglist[n], DwtNrows, 1);				n++;
    XtSetArg(arglist[n], DwtNmaxLength, 35);			n++;
    text_widget = DwtSTextCreate(nodebox_widget, "Text", arglist, n);
    XtAddCallback(text_widget, DwtNvalueChangedCallback, string_changed, 0);

#ifdef VMS
    n = 0;
    XtSetArg(arglist[n], DwtNx, 4);				n++;
    XtSetArg(arglist[n], DwtNy, 35);				n++;
    XtSetArg(arglist[n], DwtNheight, 25);			n++;
    XtSetArg(arglist[n], DwtNmenuRadio, TRUE);			n++;
    XtSetArg(arglist[n], DwtNmenuPacking, DwtMenuPackingNone);	n++;
    nettype_widget = DwtMenuCreate(nodebox_widget, "Net Type", arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNvalue, network_type == LOCAL ? 1 : 0); n++;
    XtSetArg(arglist[n], DwtNbordHighlight, TRUE);		n++;
    XtSetArg(arglist[n], DwtNx, 40);				n++;
    XtSetArg(arglist[n], DwtNy, 5);				n++;
    netitem_widget[0] = DwtToggleButtonCreate(nettype_widget, "Local Area", arglist, n);
    XtAddCallback(netitem_widget[0], DwtNvalueChangedCallback, select_net, LOCAL);

    n = 0;
    XtSetArg(arglist[n], DwtNvalue, network_type == REMOTE ? 1 : 0); n++;
    XtSetArg(arglist[n], DwtNbordHighlight, TRUE);		n++;
    XtSetArg(arglist[n], DwtNx, 150);				n++;
    XtSetArg(arglist[n], DwtNy, 5);				n++;
    netitem_widget[1] = DwtToggleButtonCreate(nettype_widget, "Wide Area", arglist, n);
    XtAddCallback(netitem_widget[1], DwtNvalueChangedCallback, select_net, REMOTE);

    XtManageChild(nettype_widget);
    XtManageChildren(netitem_widget, 2);
#else
    network_type = LOCAL;
#endif

    XtManageChild(label_widget);
    XtManageChild(text_widget);
    XtManageChild(nodebox_widget);
}

/*******************************************************************************
 *
 * Get the computer skill level
 *
 ******************************************************************************/

static void set_skill(w, sw, data)
Widget w;
Widget sw;
DwtAnyCallbackStruct *data;
{
    Arg arglist[1];

    XtSetArg(arglist[0], DwtNvalue, &computer_skill);
    XtGetValues(sw, arglist, 1);
    XtUnmanageChild(skillbox_widget);
}

void get_skill()
{
    Arg arglist[10];
    Widget skill_widget;
    Widget label_widget[2];
    Widget ok_widget;
    int n;

    if (skillbox_widget != NULL) {
	XtManageChild(skillbox_widget);
	return;
    }

    n = 0;
    XtSetArg(arglist[n], DwtNunits, DwtFontUnits);		n++;
    XtSetArg(arglist[n], DwtNresize, DwtResizeShrinkWrap);	n++;
    XtSetArg(arglist[n], DwtNborderWidth, 0);			n++;
    XtSetArg(arglist[n], DwtNmarginWidth, 10);			n++;
    XtSetArg(arglist[n], DwtNmarginHeight, 5);			n++;
    XtSetArg(arglist[n], DwtNstyle, DwtModal);			n++;
    XtSetArg(arglist[n], DwtNdefaultPosition, TRUE);		n++;
    skillbox_widget = DwtDialogBoxPopupCreate(dialogbox_widget[LOCAL], header, arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNvalue, computer_skill);		n++;
    XtSetArg(arglist[n], DwtNtitle,
	DwtLatin1String("      Computer Skill Level"));		n++;
    XtSetArg(arglist[n], DwtNx, 6);				n++;
    XtSetArg(arglist[n], DwtNy, 12);				n++;
    XtSetArg(arglist[n], DwtNwidth, 175);			n++;
    XtSetArg(arglist[n], DwtNshowValue, False);			n++;
    skill_widget = DwtScaleCreate(skillbox_widget, "Skill", arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNlabel, DwtLatin1String("Bad"));	n++;
    XtSetArg(arglist[n], DwtNx, 6);				n++;
    XtSetArg(arglist[n], DwtNy, 2);				n++;
    label_widget[0] = DwtLabelCreate(skillbox_widget, "Label1", arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNlabel, DwtLatin1String("Good"));	n++;
    XtSetArg(arglist[n], DwtNx, 85);				n++;
    XtSetArg(arglist[n], DwtNy, 2);				n++;
    label_widget[1] = DwtLabelCreate(skillbox_widget, "Label2", arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNbordHighlight, TRUE);		n++;
    XtSetArg(arglist[n], DwtNx, 43);				n++;
    XtSetArg(arglist[n], DwtNy, 60);				n++;
    ok_widget = DwtPushButtonCreate(skillbox_widget, "Ok", arglist, n);
    XtAddCallback(ok_widget, DwtNactivateCallback, set_skill, skill_widget);

    XtManageChild(ok_widget);
    XtManageChildren(label_widget, 2);
    XtManageChild(skill_widget);
    XtManageChild(skillbox_widget);
}

/*******************************************************************************
 *
 * Get the straight pool limit
 *
 ******************************************************************************/

static void set_limit(w, lw, data)
Widget w;
Widget lw;
DwtAnyCallbackStruct *data;
{
    Arg arglist[1];

    XtSetArg(arglist[0], DwtNvalue, &limit);
    XtGetValues(lw, arglist, 1);
    XtUnmanageChild(limitbox_widget);
}

void get_limit()
{
    Arg arglist[10];
    Widget limit_widget;
    Widget ok_widget;
    int n;

    if (limitbox_widget != NULL) {
	XtManageChild(limitbox_widget);
	return;
    }

    n = 0;
    XtSetArg(arglist[n], DwtNunits, DwtFontUnits);		n++;
    XtSetArg(arglist[n], DwtNresize, DwtResizeShrinkWrap);	n++;
    XtSetArg(arglist[n], DwtNborderWidth, 0);			n++;
    XtSetArg(arglist[n], DwtNmarginWidth, 10);			n++;
    XtSetArg(arglist[n], DwtNmarginHeight, 5);			n++;
    XtSetArg(arglist[n], DwtNstyle, DwtModal);			n++;
    XtSetArg(arglist[n], DwtNdefaultPosition, TRUE);		n++;
    limitbox_widget = DwtDialogBoxPopupCreate(dialogbox_widget[LOCAL], header, arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNvalue, limit);			n++;
    XtSetArg(arglist[n], DwtNminValue, 10);			n++;
    XtSetArg(arglist[n], DwtNmaxValue, 200);			n++;
    XtSetArg(arglist[n], DwtNtitle,
	DwtLatin1String("      Straight Pool Limit"));		n++;
    XtSetArg(arglist[n], DwtNx, 6);				n++;
    XtSetArg(arglist[n], DwtNy, 2);				n++;
    XtSetArg(arglist[n], DwtNwidth, 175);			n++;
    limit_widget = DwtScaleCreate(limitbox_widget, "Limit", arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNbordHighlight, TRUE);		n++;
    XtSetArg(arglist[n], DwtNx, 43);				n++;
    XtSetArg(arglist[n], DwtNy, 50);				n++;
    ok_widget = DwtPushButtonCreate(limitbox_widget, "Ok", arglist, n);
    XtAddCallback(ok_widget, DwtNactivateCallback, set_limit, limit_widget);

    XtManageChild(ok_widget);
    XtManageChild(limit_widget);
    XtManageChild(limitbox_widget);
}

/*******************************************************************************
 *
 * Generic menu widget (Used for high/low ball selection)
 *
 ******************************************************************************/

static void menu_select(w, tag, data)
Widget w;
DwtAnyCallbackStruct *data;
{
    XtUnmanageChild(menubox_widget[system]);
    menu_selection = tag;
    pool_table();
}

void select_menu(strings)
char *strings[];
{
    XEvent event;
    Arg arglist[10];
    static Widget menu_widget;
    static Widget item_widget[2];
    int items;
    int x, y;
    int n;

    get_window(system, &x, &y);
    x += 700; y += 642;
    x = min(max(x,0),878);
    y = min(max(y,0),766);

    if (menubox_widget[system] != NULL) {
	for (items = 0; items < 2; items++) {
	    XtSetArg(arglist[0], DwtNlabel, DwtLatin1String(strings[items]));
	    XtSetValues(item_widget[items], arglist, 1);
	}
	XtMoveWidget(menubox_widget[system], x, y);
	XtManageChild(menubox_widget[system]);
	return;
    }

    n = 0;
    XtSetArg(arglist[n], DwtNunits, DwtFontUnits);		n++;
    XtSetArg(arglist[n], DwtNresize, DwtResizeShrinkWrap);	n++;
    XtSetArg(arglist[n], DwtNborderWidth, 0);			n++;
    XtSetArg(arglist[n], DwtNmarginWidth, 10);			n++;
    XtSetArg(arglist[n], DwtNmarginHeight, 10);			n++;
    XtSetArg(arglist[n], DwtNstyle, DwtModal);			n++;
    XtSetArg(arglist[n], DwtNx, x);				n++;
    XtSetArg(arglist[n], DwtNy, y);				n++;
    menubox_widget[system] = DwtDialogBoxPopupCreate(window_widget[system], header, arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNx, 4);				n++;
    XtSetArg(arglist[n], DwtNy, 4);				n++;
    XtSetArg(arglist[n], DwtNlabel, DwtLatin1String("Select"));	n++;
    menu_widget = DwtOptionMenuCreate(menubox_widget[system], "Menu", arglist, n);

    for (items = 0; items < 2; items++) {
	n = 0;
	XtSetArg(arglist[n], DwtNbordHighlight, TRUE);		n++;
	item_widget[items] = DwtPushButtonCreate(menu_widget, strings[items], arglist, n);
	XtAddCallback(item_widget[items], DwtNactivateCallback, menu_select, items+1);
    }
    XtManageChildren(item_widget, items);
    XtManageChild(menu_widget);
    XtManageChild(menubox_widget[system]);
    XtRealizeWidget(menubox_widget[system]);
}

/*******************************************************************************
 *
 * Get velocity and spin for each shot
 *
 ******************************************************************************/

static void draw_spinball(s)
{
    XClearWindow(XtDisplay(spin_widget[s]), XtWindow(spin_widget[s]));
    XDrawArc(XtDisplay(spin_widget[s]), XtWindow(spin_widget[s]), dgc[s],
	35, 35, 110, 110, 0, 360*64);
    XFillRectangle(XtDisplay(spin_widget[s]), XtWindow(spin_widget[s]), dgc[s],
	40+spin-5, 140-roll-1, 11, 3);
    XFillRectangle(XtDisplay(spin_widget[s]), XtWindow(spin_widget[s]), dgc[s],
	40+spin-1, 140-roll-5, 3, 11);
}

static void expose_spinball(w, tag, data)
Widget w;
DwtWindowCallbackStruct *data;
{
    int s;

    for (s=0; s<num_screens; s++) {
	if (XtDisplay(w) == XtDisplay(toplevel_widget[s])) {
	    draw_spinball(s);
	    break;
	}
    }
}

static void new_spin(w, tag, event)
Widget w;
XEvent *event;
{
    int s;
    int xdist, ydist;

    for (s=0; s<num_screens; s++)
	if (XtDisplay(w) == XtDisplay(toplevel_widget[s])) {
	    if (event->xbutton.button == Button1) {
		xdist = event->xbutton.x - 90;
		ydist = event->xbutton.y - 90;
		if (xdist*xdist+ydist*ydist<50*50) {
		    spin = xdist + 50;
		    roll = 50 - ydist;
		    draw_spinball(s);
		}
	    }
	    break;
	}
}

static void velocity_select(w, tag, data)
Widget w;
DwtAnyCallbackStruct *data;
{
    Arg arglist[1];
    int s;

    /*
     * If we are lagging for break, and we running in local network mode
     * and this is the remote screen, then use the variable remote_velocity
     * instead of velocity.
     */
    if (num_screens > 1 && !action_started &&
	XtDisplay(w) == XtDisplay(toplevel_widget[REMOTE])) {
	XtSetArg(arglist[0], DwtNvalue, &remote_velocity);
	s = REMOTE;
    } else {
	XtSetArg(arglist[0], DwtNvalue, &velocity);
	s = system;
    }
    XtGetValues(velocity_widget[s], arglist, 1);
    XtUnmanageChild(velocitybox_widget[s]);
    if (--num_velocities == 0)
	pool_table();
}

void get_velocity(remote)
{
    XEvent event;
    Arg arglist[10];
    Widget label_widget[3];
    Widget ok_widget;
    int x, y;
    int n, s;

    s = (remote ? REMOTE : system);
    num_velocities++;

    get_window(s, &x, &y);
    x += 800; y += 250;
    x = min(max(x,0),802);
    y = min(max(y,0),515);

    if (velocitybox_widget[s] != NULL) {
	XtSetArg(arglist[0], DwtNvalue, 50);
	XtSetValues(velocity_widget[s], arglist, 1);
	spin = 50; roll = 50;
	XtMoveWidget(velocitybox_widget[s], x, y);
	XtManageChild(velocitybox_widget[s]);
	return;
    }

    n = 0;
    XtSetArg(arglist[n], DwtNunits, DwtFontUnits);		n++;
    XtSetArg(arglist[n], DwtNresize, DwtResizeShrinkWrap);	n++;
    XtSetArg(arglist[n], DwtNborderWidth, 0);			n++;
    XtSetArg(arglist[n], DwtNmarginWidth, 10);			n++;
    XtSetArg(arglist[n], DwtNmarginHeight, 10);			n++;
    XtSetArg(arglist[n], DwtNstyle, DwtModeless);		n++;
    XtSetArg(arglist[n], DwtNx, x);				n++;
    XtSetArg(arglist[n], DwtNy, y);				n++;
    velocitybox_widget[s] = DwtDialogBoxPopupCreate(window_widget[s], header, arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNvalue, 50);			n++;
    XtSetArg(arglist[n], DwtNtitle,
	DwtLatin1String("               Velocity %"));		n++;
    XtSetArg(arglist[n], DwtNx, 6);				n++;
    XtSetArg(arglist[n], DwtNy, 12);				n++;
    XtSetArg(arglist[n], DwtNwidth, 175);			n++;
    velocity_widget[s] = DwtScaleCreate(velocitybox_widget[s], "Velocity", arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNlabel, DwtLatin1String("0%"));	n++;
    XtSetArg(arglist[n], DwtNx, 6);				n++;
    XtSetArg(arglist[n], DwtNy, 2);				n++;
    label_widget[0] = DwtLabelCreate(velocitybox_widget[s], "Label1", arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNlabel, DwtLatin1String("100%"));	n++;
    XtSetArg(arglist[n], DwtNx, 80);				n++;
    XtSetArg(arglist[n], DwtNy, 2);				n++;
    label_widget[1] = DwtLabelCreate(velocitybox_widget[s], "Label2", arglist, n);

    spin = 50; roll = 50;
    n = 0;
    XtSetArg(arglist[n], DwtNwidth, 180);			n++;
    XtSetArg(arglist[n], DwtNheight, 180);			n++;
    XtSetArg(arglist[n], DwtNx, 9);				n++;
    XtSetArg(arglist[n], DwtNy, 60);				n++;
    spin_widget[s] = DwtWindowCreate(velocitybox_widget[s], "Spin", arglist, n);
    XtAddCallback(spin_widget[s], DwtNexposeCallback, expose_spinball, 0);
    XtAddEventHandler(spin_widget[s], ButtonPressMask, FALSE, new_spin, NULL);

    n = 0;
    XtSetArg(arglist[n], DwtNlabel, DwtLatin1String("Contact Point")); n++;
    XtSetArg(arglist[n], DwtNx, 31);				n++;
    XtSetArg(arglist[n], DwtNy, 180);				n++;
    label_widget[2] = DwtLabelCreate(velocitybox_widget[s], "Label3", arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNbordHighlight, TRUE);		n++;
    XtSetArg(arglist[n], DwtNx, 43);				n++;
    XtSetArg(arglist[n], DwtNy, 200);				n++;
    ok_widget = DwtPushButtonCreate(velocitybox_widget[s], "Ok", arglist, n);
    XtAddCallback(ok_widget, DwtNactivateCallback, velocity_select, 0);

    XtManageChild(ok_widget);
    XtManageChildren(label_widget, 3);
    XtManageChild(velocity_widget[s]);
    XtManageChild(spin_widget[s]);

    XtManageChild(velocitybox_widget[s]);
}

/*******************************************************************************
 *
 * Help Routines
 *
 ******************************************************************************/

static void provide_help(w, tag, data)
Widget w;
DwtAnyCallbackStruct *data;
{
    int s;

    for (s=0; s<num_screens; s++)
	if (XtDisplay(w) == XtDisplay(toplevel_widget[s])) {
	    switch (tag) {
		case 0:					/* about... */
		    display_about(s);
		    break;
		case 1:					/* more about... */
		    display_help(s);
		    break;
	    }
	    break;
	}
}

static void hide_help(w, tag, data)
Widget w;
DwtAnyCallbackStruct *data;
{
    int s;

    for (s=0; s<num_screens; s++)
	if (XtDisplay(w) == XtDisplay(toplevel_widget[s])) {
	    if (XtIsManaged(helpbox_widget[s]))
		XtUnmanageChild(helpbox_widget[s]);
	    break;
	}
}

void display_about(s)
{
    Arg arglist[10];
    XFontStruct *about_font;
    int n;

#define ABFONT   "-Adobe-New Century Schoolbook-Bold-R-Normal--*-140-*"

    if (aboutbox_widget[s] == NULL) {
	about_font = XLoadQueryFont(XtDisplay(toplevel_widget[s]), ABFONT);
	abfont[s] = DwtCreateFontList(about_font, CDA$K_ISO_LATIN1);
	n = 0;
	XtSetArg(arglist[n], DwtNdefaultPosition, TRUE);	n++;
	XtSetArg(arglist[n], DwtNlabel, DwtLatin1String(ABOUT_XPOOL)); n++;
	XtSetArg(arglist[n], DwtNfont, abfont[s]);		n++;
	aboutbox_widget[s] = DwtMessageBoxCreate(dialogbox_widget[s], "About", arglist, n);
    }
    XtManageChild(aboutbox_widget[s]);
}

void display_help(s)
{
    Arg arglist[20];
    Widget helptext_widget;
    Widget ok_widget;
    XFontStruct *mono_font;
    int n;

#define MONOFONT "-Adobe-Courier-Medium-R-Normal--*-140-*"

    if (helpbox_widget[s] != NULL) {
	if (!XtIsManaged(helpbox_widget[s]))
	    XtManageChild(helpbox_widget[s]);
	return;
    }

    mono_font = XLoadQueryFont(XtDisplay(toplevel_widget[s]), MONOFONT);
    mnfont[s] = DwtCreateFontList(mono_font, CDA$K_ISO_LATIN1);

    n = 0;
    XtSetArg(arglist[n], DwtNresize, DwtResizeShrinkWrap);	n++;
    XtSetArg(arglist[n], DwtNborderWidth, 0);			n++;
    XtSetArg(arglist[n], DwtNdefaultPosition, TRUE);		n++;
    XtSetArg(arglist[n], DwtNdefaultHorizontalOffset, 3);	n++;
    XtSetArg(arglist[n], DwtNdefaultVerticalOffset, 3);		n++;
    helpbox_widget[s] = DwtAttachedDBPopupCreate(dialogbox_widget[s], "More About", arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNadbTopAttachment, DwtAttachAdb);	n++;
    XtSetArg(arglist[n], DwtNadbLeftAttachment,DwtAttachAdb);	n++;
    XtSetArg(arglist[n], DwtNrows, 25);				n++;
    XtSetArg(arglist[n], DwtNcols, 72);				n++;
    XtSetArg(arglist[n], DwtNborderWidth,1);			n++;
    XtSetArg(arglist[n], DwtNhalfBorder,FALSE);			n++;
    XtSetArg(arglist[n], DwtNinsertionPointVisible, FALSE);	n++;
    XtSetArg(arglist[n], DwtNresizeWidth, FALSE);		n++;
    XtSetArg(arglist[n], DwtNeditable, FALSE);			n++;
    XtSetArg(arglist[n], DwtNscrollVertical, TRUE);		n++;
    XtSetArg(arglist[n], DwtNfont, mnfont[s]);			n++;
    XtSetArg(arglist[n], DwtNvalue, xpool_help);		n++;
    helptext_widget = DwtSTextCreate(helpbox_widget[s], "Help Text", arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNadbTopAttachment, DwtAttachWidget); n++;
    XtSetArg(arglist[n], DwtNadbTopWidget, helptext_widget);	n++;
    XtSetArg(arglist[n], DwtNadbLeftAttachment, DwtAttachAdb);	n++;
    XtSetArg(arglist[n], DwtNshadow, FALSE);			n++;
    ok_widget = DwtPushButtonCreate(helpbox_widget[s], "Acknowledged", arglist, n);
    XtAddCallback(ok_widget, DwtNactivateCallback, hide_help, 0);

    XtManageChild(helptext_widget);
    XtManageChild(ok_widget);
    XtManageChild(helpbox_widget[s]);
}

/*******************************************************************************
 *
 * Main parameter selection routine
 *
 ******************************************************************************/

void select_play()
{
    enable_select_options();
    XtAppMainLoop(application_context);
}

/*******************************************************************************
 *
 * Graphics window handling widgets
 *
 ******************************************************************************/

static void set_control(w, tag, data)
Widget w;
DwtAnyCallbackStruct *data;
{
    if (tag == 0 && (opponent == NETWORK || opponent == AUTONET))
	get_node();
    else {
	if (tag > 0 && num_screens > 1 &&
	    XtDisplay(w) == XtDisplay(toplevel_widget[REMOTE]))
	    abort_flag = ABORT_REMOTE;
	else
	    switch (tag) {
		case 0:
		    enable_game_options();
		    break;
		case 1:
		    abort_flag = ABORT_GAME;
		    break;
		case 2:
		    abort_flag = ABORT_PROGRAM;
		    break;
	    }
	pool_table();
    }
}

static void select_option(w, tag, data)
Widget w;
DwtTogglebuttonCallbackStruct *data;
{
    int s;

    for (s=0; s<num_screens; s++)
	if (XtDisplay(w) == XtDisplay(toplevel_widget[s])) {
	    switch (tag) {
		case PRESET_VELOCITY:
		    preset_velocity[s] = data->value;
		    break;
		case CONFIRM_SHOTS:
		    confirm_shots[s] = data->value;
		    break;
		case COMPUTER_SKILL:		/* get computer skill */
		    get_skill();
		    break;
		case STRAIGHT_LIMIT:		/* get straight pool limit */
		    get_limit();
		    break;
		case RESPOT_BALL:
		    respot_ball = TRUE;		/* ask about respot */
		    position = OVER_RESPOT;
		    pool_table();
		    break;
	    }
	    break;
	}
}

static void select_opponent(w, tag, data)
Widget w;
DwtTogglebuttonCallbackStruct *data;
{
    if (data->value)
	opponent = tag;
}

static void select_game(w, tag, data)
Widget w;
DwtTogglebuttonCallbackStruct *data;
{
    if (data->value)
	game = tag;
}

static void window_menubar(s)
{
    Arg arglist[10];
    Widget menubar_widget;
    Widget control_widget;
    Widget controlmenu_widget;
    Widget gmenu_widget;
    Widget gitem_widget[num_games];
    Widget omenu_widget;
    Widget oitem_widget[num_opps];
    Widget option_widget;
    Widget optionmenu_widget;
    Widget help_widget;
    Widget helpmenu_widget;
    Widget helpitem_widget[num_helps];
    int control_items;
    int game_items;
    int opp_items;
    int help_items;
    int n;

    n = 0;
    XtSetArg(arglist[n], DwtNorientation, DwtOrientationHorizontal); n++;
    XtSetArg(arglist[n], DwtNborderWidth, 2);			n++;
    XtSetArg(arglist[n], DwtNspacing, 15);			n++;
    XtSetArg(arglist[n], DwtNadbLeftAttachment, DwtAttachAdb);	n++;
    XtSetArg(arglist[n], DwtNadbLeftOffset, 0);			n++;
    XtSetArg(arglist[n], DwtNadbRightAttachment, DwtAttachAdb);	n++;
    XtSetArg(arglist[n], DwtNadbRightOffset, 0);		n++;
    menubar_widget = DwtMenuBarCreate(dialogbox_widget[s], "Menu Bar", arglist, n);

    /* Control Menu Bar option */

    n = 0;
    controlmenu_widget =
	(Widget)DwtMenuPulldownCreate(menubar_widget, "Control Menu", arglist, n);

    for (control_items=0; control_items<num_controls; control_items++) {
	n = 0;
	controlitem_widget[s][control_items] =
		DwtPushButtonCreate(controlmenu_widget,
		control_string[control_items], arglist, n);
	XtAddCallback(controlitem_widget[s][control_items],
		DwtNactivateCallback, set_control, control_items);
    }

    n = 0;
    XtSetArg(arglist[n], DwtNsubMenuId, controlmenu_widget);	n++;
    control_widget =
	DwtPullDownMenuEntryCreate(menubar_widget, "Control", arglist, n);

    /* Game Menu Bar option */

    n = 0;
    XtSetArg(arglist[n], DwtNmenuRadio, TRUE);			n++;
    gmenu_widget =
	(Widget)DwtMenuPulldownCreate(menubar_widget, "Game Menu", arglist, n);

    for (game_items = 0; game_items < num_games; game_items++) {
	n = 0;
	XtSetArg(arglist[n], DwtNvalue, (game_items == game-1));n++;
	gitem_widget[game_items] =
	    DwtToggleButtonCreate(gmenu_widget, game_string[game_items],
	    arglist, n);
	XtAddCallback(gitem_widget[game_items], DwtNvalueChangedCallback,
	    select_game, game_items+1);
    }

    n = 0;
    XtSetArg(arglist[n], DwtNsubMenuId, gmenu_widget);		n++;
    game_widget[s] =
	DwtPullDownMenuEntryCreate(menubar_widget, "Game", arglist, n);

    /* Opponent Menu Bar option */

    n = 0;
    XtSetArg(arglist[n], DwtNmenuRadio, TRUE);			n++;
    omenu_widget =
	(Widget)DwtMenuPulldownCreate(menubar_widget, "Opponent Menu", arglist, n);

    for (opp_items = 0; opp_items < num_opps; opp_items++) {
	n = 0;
	XtSetArg(arglist[n], DwtNvalue, (opp_items == opponent-1));n++;
	oitem_widget[opp_items] =
	    DwtToggleButtonCreate(omenu_widget, opponent_string[opp_items],
	    arglist, n);
	XtAddCallback(oitem_widget[opp_items], DwtNvalueChangedCallback,
	    select_opponent, opp_items+1);
    }

    n = 0;
    XtSetArg(arglist[n], DwtNsubMenuId, omenu_widget);		n++;
    opponent_widget[s] =
	DwtPullDownMenuEntryCreate(menubar_widget, "Opponent", arglist, n);

    /* Option Menu Bar option */

    n = 0;
    optionmenu_widget =
	(Widget)DwtMenuPulldownCreate(menubar_widget, "Option Menu", arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNvalue, preset_velocity[s]);	n++;
    optionitem_widget[s][PRESET_VELOCITY] =
		DwtToggleButtonCreate(optionmenu_widget,
		option_string[PRESET_VELOCITY], arglist, n);
    XtAddCallback(optionitem_widget[s][PRESET_VELOCITY],
		DwtNvalueChangedCallback, select_option, PRESET_VELOCITY);

    n = 0;
    XtSetArg(arglist[n], DwtNvalue, confirm_shots[s]);		n++;
    optionitem_widget[s][CONFIRM_SHOTS] =
		DwtToggleButtonCreate(optionmenu_widget,
		option_string[CONFIRM_SHOTS], arglist, n);
    XtAddCallback(optionitem_widget[s][CONFIRM_SHOTS],
		DwtNvalueChangedCallback, select_option, CONFIRM_SHOTS);

    n = 0;
    optionitem_widget[s][COMPUTER_SKILL] = DwtPushButtonCreate(optionmenu_widget,
		option_string[COMPUTER_SKILL], arglist, n);
    XtAddCallback(optionitem_widget[s][COMPUTER_SKILL],
		DwtNactivateCallback, select_option, COMPUTER_SKILL);

    n = 0;
    optionitem_widget[s][STRAIGHT_LIMIT] = DwtPushButtonCreate(optionmenu_widget,
		option_string[STRAIGHT_LIMIT], arglist, n);
    XtAddCallback(optionitem_widget[s][STRAIGHT_LIMIT],
		DwtNactivateCallback, select_option, STRAIGHT_LIMIT);

    n = 0;
    optionitem_widget[s][RESPOT_BALL] = DwtPushButtonCreate(optionmenu_widget,
		option_string[RESPOT_BALL], arglist, n);
    XtAddCallback(optionitem_widget[s][RESPOT_BALL],
		DwtNactivateCallback, select_option, RESPOT_BALL);

    n = 0;
    XtSetArg(arglist[n], DwtNsubMenuId, optionmenu_widget);	n++;
    option_widget =
	DwtPullDownMenuEntryCreate(menubar_widget, "Options", arglist, n);

    /* Help Menu Bar option */

    n = 0;
    helpmenu_widget =
	(Widget)DwtMenuPulldownCreate(menubar_widget, "Help Menu", arglist, n);

    for (help_items=0; help_items<num_helps; help_items++) {
	n = 0;
	helpitem_widget[help_items] =
		DwtPushButtonCreate(helpmenu_widget,
		help_string[help_items], arglist, n);
	XtAddCallback(helpitem_widget[help_items],
		DwtNactivateCallback, provide_help, help_items);
    }

    n = 0;
    XtSetArg(arglist[n], DwtNsubMenuId, helpmenu_widget);	n++;
    help_widget =
	DwtPullDownMenuEntryCreate(menubar_widget, "Help", arglist, n);

    XtSetArg(arglist[0], DwtNmenuHelpWidget, help_widget);
    XtSetValues(menubar_widget, arglist, 1);

    enable_game_options();

    XtManageChildren(controlitem_widget[s], control_items);
    XtManageChild(control_widget);
    XtManageChildren(gitem_widget, game_items);
    XtManageChild(game_widget[s]);
    XtManageChildren(oitem_widget, opp_items);
    XtManageChild(opponent_widget[s]);
    XtManageChildren(optionitem_widget[s], num_options);
    XtManageChild(option_widget);
    XtManageChildren(helpitem_widget, help_items);
    XtManageChild(help_widget);
    XtManageChild(menubar_widget);
}

void open_window(s, width, height, display, window)
Display **display;
Window *window;
{
    Pixmap icon_pix;		/* pixmap to hold the icon definition */
    Pixmap bigicon_pix;		/* pixmap to hold the BIG icon definition */
    XGCValues gcv;
    Arg arglist[10];
    int n;

    n = 0;
    XtSetArg(arglist[n], DwtNunits, DwtPixelUnits);		n++;
    XtSetArg(arglist[n], DwtNresize, DwtResizeShrinkWrap);	n++;
    XtSetArg(arglist[n], DwtNborderWidth, 0);			n++;
    dialogbox_widget[s] = DwtAttachedDBCreate(toplevel_widget[s], header, arglist, n);

    window_menubar(s);

    n = 0;
    XtSetArg(arglist[n], DwtNwidth, width);			n++;
    XtSetArg(arglist[n], DwtNheight, height);			n++;
    window_widget[s] = DwtWindowCreate(dialogbox_widget[s], "Window", arglist, n);
    XtAddCallback(window_widget[s], DwtNexposeCallback, handle_exposures, 0);
    XtAddEventHandler(window_widget[s], ButtonPressMask, FALSE, got_button, NULL);

    XtMoveWidget(toplevel_widget[s],
	(x_offset[s] == NOVALUE ?
		(XWidthOfScreen(XtScreen(toplevel_widget[s]))-width)/2 :
		x_offset[s]),
	(y_offset[s] == NOVALUE ?
		(XHeightOfScreen(XtScreen(toplevel_widget[s]))-height)/2 :
		y_offset[s]));

    XtManageChild(window_widget[s]);
    XtManageChild(dialogbox_widget[s]);
    XtRealizeWidget(toplevel_widget[s]);

    icon_pix = XCreatePixmapFromBitmapData(
		XtDisplay(dialogbox_widget[s]),
		XtWindow(dialogbox_widget[s]),
		xpool_icon_bits,
		xpool_icon_width,
		xpool_icon_height,
		1, 0, 1);

    bigicon_pix = XCreatePixmapFromBitmapData(
		XtDisplay(dialogbox_widget[s]),
		XtWindow(dialogbox_widget[s]),
		xpool_bigicon_bits,
		xpool_bigicon_width,
		xpool_bigicon_height,
		1, 0, 1);

    n = 0;
    XtSetArg(arglist[n], XtNnoResize, TRUE);		n++;
    XtSetArg (arglist[n], XtNiconPixmap, bigicon_pix);	n++;
    XtSetArg (arglist[n], XtNiconifyPixmap, icon_pix);	n++;
    XtSetValues(toplevel_widget[s], arglist, n);

    n = 0;
    XtSetArg(arglist[n], DwtNforeground, &gcv.foreground);	n++;
    XtSetArg(arglist[n], DwtNbackground, &gcv.background);	n++;
    XtGetValues(dialogbox_widget[s], arglist, n);
    dgc[s] = XCreateGC(XtDisplay(dialogbox_widget[s]),
	XtWindow(dialogbox_widget[s]), GCForeground | GCBackground, &gcv);

    *window = XtWindow(window_widget[s]);
    *display = XtDisplay(window_widget[s]);
}
