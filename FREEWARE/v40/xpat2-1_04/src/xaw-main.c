/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module Xaw-main.c			     */
/*									     */
/*	main function for the Athena Widget interface			     */
/*	written by Michael Bischoff (mbi@mo.math.nat.tu-bs.de)		     */
/*	March-1994							     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#ifdef useXaw
#include "X-pat.h"
#include "Tableau.h"
#include "version.h"

Window table, mainwindow;
Widget toplevel;

static void perform_command(Widget widget, XtPointer client_data, XtPointer call_data) {
    (*(void (*)(void))client_data)();	/* any questions? */
}

void create_help(void);
void popup_help(void);
void popdown_help(Widget, XtPointer, XtPointer);

static Widget messagebox, container, desktop;

void show_message(const char *str, ...) {
    static char last_message[256];
    Arg Args;
    va_list args;
    va_start(args, str);

    if (!str) {
	memset(last_message, ' ', sizeof(last_message)-1);
	last_message[sizeof(last_message)-1] = '\0';
    } else
	vsprintf(last_message, str, args);

    XtSetArg(Args, XtNlabel, last_message);
    XtSetValues(messagebox, &Args, 1);
}



static XtAppContext app_con;
static void (*execfunc)(void) = NULL;
static Widget dialog, popup;

void cmd_LeavePat(void) {
    play_sound("goodbye");
    XtDestroyApplicationContext(app_con);
    exit(0);
}

static void popup_confirm(const char *prompt) {
    Arg args[2];
    Position x, y;
    Dimension xx, yy;

    XtSetArg(args[0], XtNlabel, prompt);
    XtSetValues(dialog, args, 1);
    XtVaGetValues(dialog, XtNwidth, &xx, XtNheight, &yy, NULL);
    x = (graphic.w_width/2) - xx/2;
    y = (graphic.w_height/3) - yy/2;

    XtTranslateCoords(toplevel, x, y, &x, &y);
    XtSetArg(args[0], XtNx, x);
    XtSetArg(args[1], XtNy, y);
    XtSetValues(popup, args, 2);
    XtPopup(popup, XtGrabNone);
}

void cmd_Confirm(void) {
    if (execfunc) {
	void (*execfunc2)(void) = execfunc;	/* erase it first! */
	execfunc = NULL;
	XtPopdown(popup);
	(*execfunc2)();	/* finally execute the desired function */
    }
}
void cmd_Cancel(void) {
    if (execfunc) {
	execfunc = NULL;
	XtPopdown(popup);
    }
}

/* type converter functions: */
static void mXtAP_Cancel (Widget w, XEvent *xev, String *params, Cardinal *num) { cmd_Cancel();  }
static void mXtAP_Confirm(Widget w, XEvent *xev, String *params, Cardinal *num) { cmd_Confirm(); }
static void Cancel(Widget widget, XtPointer client_data, XtPointer call_data)	{ cmd_Cancel();  }
static void Ok(Widget widget, XtPointer client_data, XtPointer call_data)	{ cmd_Confirm(); }

void request_confirm(void (*dofunc)(void), const char *prompt) {
    if (execfunc)
	return;		/* request pending => deny another one */
    execfunc = dofunc;
    popup_confirm(prompt);
}

static String fallback_resources[] = { 
    "*beNiceToColormap:			false",
    "*shapeStyle:			Rectangle",
    "*topShadowContrast:		20",
    "*bottomShadowContrast:		40",
    "*Scrollbar*background:		Grey70",
    "*Background:			grey85",
    "*Foreground:			black",
    "*resizeToPreferred:		True",
    "*input:				True",
    "*showGrip:				off",
    "*shadowWidth:                   	2",
    "*messages.justify:			Left",
    "*upperbox.orientation:		XtorientHorizontal",
    "*lowerbox.orientation:		XtorientHorizontal",
    "*Tableau.backingStore:		WhenMapped",
    "*Tableau.keyboardFile:		keys",
    "*Tableau.messageFile:		messages",
    "*Tableau.background:		LightSlateGray",
    "*Tableau.arrowColor:		Gold",
    "*Tableau.markColor:		Black",
    "*Tableau.rules:		        Gypsy",
    "*Tableau.rng:		        1",
    "*Viewport.allowHoriz:		True",
    "*Viewport.allowVert:		True",
    "*Viewport.useBottom:		True",
    "*Viewport.useRight:		True",
    "*Label.shadowWidth:		0",
    "*Label.BorderWidth:		2",
    "*Dialog*Translations: #override \n<Key>y: Ok()\n<Key>n: Cancel()\n",
    "XPat*title:			XPat",
    "XPat.prompt.allowShellResize:	True",
    "XPat.prompt.saveUnder:		True",
    "*Dialog*resizable:			True",
    "*Hint.Translations: #override\n<BtnDown>:set()\n<BtnUp>:HintNotify()unset()\n",
    "*Sound.state:			True",
    "XPat.help.width:			403",
    "XPat.help.height:			200",
    "XPat.help.title:			XPat Help Window",
    "XPat.help.saveUnder:		True",
    "XPat*Close Help.fromHoriz:		Topic",
    "*helptext*string:			Please choose a topic.",
    "*helptext*displayCaret:		False",
    "*helptext*scrollHorizontal:	whenNeeded",
    "*helptext*scrollVertical:		whenNeeded",
    "*helptext*editType:		read",
    NULL,
};

static XrmOptionDescRec options[] = {
    /* tableau resources */
    { "-xpmdir",	"*Tableau.xpmdir",	XrmoptionSepArg, NULL },
    { "-rules",		"*Tableau.rules",	XrmoptionSepArg, NULL },
    { "-faceup",	"*Tableau.faceup",	XrmoptionSepArg, NULL },
    { "-facedown",	"*Tableau.facedown",	XrmoptionSepArg, NULL },
    { "-decks",		"*Tableau.decks",	XrmoptionSepArg, NULL },
    { "-slots", 	"*Tableau.slots",	XrmoptionSepArg, NULL },
    { "-jokers",	"*Tableau.jokers",	XrmoptionSepArg, NULL },
    { "-tmps",		"*Tableau.tmps",	XrmoptionSepArg, NULL },
    { "-relaxed",       "*Tableau.relaxed",	XrmoptionSepArg, NULL },
    { "-flips",        	"*Tableau.flips",	XrmoptionSepArg, NULL },
    { "-turn",		"*Tableau.turn",	XrmoptionSepArg, NULL },
    { "-rotations",	"*Tableau.rotations",	XrmoptionSepArg, NULL },
    { "-rng",		"*Tableau.rng",		XrmoptionSepArg, NULL },
#ifdef GFX1
#ifdef LOADCARDS
    { "-cards",		"*Tableau.cardset",	XrmoptionSepArg, NULL },
#endif
    { "-cb",        	"*Tableau.cardbackColor",XrmoptionSepArg,NULL },
    { "-red",        	"*Tableau.RedColor",    XrmoptionSepArg, NULL },
#endif
    { "-tb",        	"*Tableau.background",	XrmoptionSepArg,NULL },
    { "-arrowcolor",	"*Tableau.arrowColor",	XrmoptionSepArg, NULL },
    { "-markcolor",	"*Tableau.markColor",	XrmoptionSepArg, NULL },
    { "-markwidth",	"*Tableau.markWidth",	XrmoptionSepArg, NULL },

    /* non-tableau resources */
#ifdef SOUND
    { "-sound", 	"*Sound.state",      	XrmoptionNoArg, (XtPointer)"True" },
    { "-nosound", 	"*Sound.state",      	XrmoptionNoArg, (XtPointer)"False" },
#endif
};

static void HintNotify(Widget w, XEvent *xev, String *params, Cardinal *num) {
    /* check to be sure state is still Set so that user can cancel
       the action (e.g. by moving outside the window, in the default
       bindings. */
    /* Nope, we cannot do this, the 'set' state is private data */
    /* CommandWidget cbw = (CommandWidget)w; 
    if (cbw->command.set)
	XtCallCallbackList(w, cbw->command.callbacks, (XtPointer) NULL); */
    switch (((XButtonPressedEvent *)xev)->button) {
    case Button1:
	cmd_NextHint();
	break;
    case Button2:
	cmd_DoHint();
	break;
    case Button3:
	cmd_PreviousHint();
    }
}


static XtActionsRec moreActions[] = {
    { "HintNotify",	HintNotify },
    { "Cancel",		mXtAP_Cancel },
    { "Ok",		mXtAP_Confirm }
};

static void reportfunc(Widget wid, XtPointer client_data, XtPointer call_data) {
    XawPannerReport *rep;
    if (game.graphic)
	cmd_ResetHints();
    rep = (XawPannerReport *)call_data;
    if (graphic.w_width != rep->slider_width || graphic.w_height != rep->slider_height) {
	static int busy = 0;	/* hope this fixes cycling */
	/* size was changed */
	if (busy)
	    ;/*printf("doing nothing, I\'m busy!\n"); */
	else {
	    graphic.w_width = rep->slider_width;
	    graphic.w_height = rep->slider_height;
	    if (game.graphic) {
		int w, h;
		busy = 1;
		init_layout();
		w = max(graphic.preferred_width, graphic.w_width);
		h = max(graphic.preferred_height, graphic.w_height);
		AskWidgetForResize(w, h);
		busy = 0;
	    }
	}
    }
}

char *loadfilename;

static void process_extra_args(int argc, char *argv[]) {
    /* check extra args */
    game.seed = -1L;
    loadfilename = NULL;
    if (argc > 2) {
    usage:
	fprintf(stderr, "xpat: invalid argument: %s\n", argv[1]);
	fprintf(stderr, "usage: xpat2 [options] [seed|loadfilename]\n"
		"options are all standard X11 toolkit options and\n"
		"-xpmdir (dir)       to set directory for xpm files\n"
                "-rules (ruleset)    to initially use specified rules\n"
		"-faceup|facedown|decks|slots|jokers|tmps (num) customize parameters\n"
		"-relaxed|flips|turn|rotations (num)            customize parameters\n"
		"-tb (color)         set background color of tableau\n"
		"-arrowcolor (color) set color of hint arrow\n"
		"-markcolor (color)  set color of frame to mark cards\n"
		"-markwidth (wdth)   set width of frame to mark cards\n"
#ifdef SOUND
		"-sound              sound toggle on\n"
		"-nosound            sound toggle off\n"
#endif
#ifdef GFX1
#ifdef LOADCARDS
		"-cards (filename)   to load external bitmap cardset\n"
#endif
		"-cb (color)         set cardback color\n"
		"-red (color)        use a different 'red' for static grey screens\n"
#endif
		);
	exit(EXIT_FAILURE);
    } else if (argc == 2) {
	/* seed or load file */
	if (*argv[1] == '-')
	    goto usage;
	else if (isdigit(*argv[1])) {
	    game.seed = atol(argv[1]);
	    if (game.seed < 0L || game.seed >= 1000000000L)
		goto usage;
	} else {
	    loadfilename = argv[1];
	    /* printf("loadfile is %s\n", loadfilename); */
	}
    }
    /* printf("in process_args: seed is %ld\n", game.seed); */
}

/*extern void cmd_DebugOut(void); */
extern struct rules *rulepool[];
static void selectrules(Widget w, XtPointer number, XtPointer garbage) {
    const char *s = XtName(w);
    /* printf("widget %s has been selected\n", s); */
    change_rules(s);
}

#ifdef SOUND
static Widget sound;

int checksound(void) {
    Boolean retval;
    Arg args[1];
    XtSetArg(args[0], XtNstate, &retval);
    XtGetValues(sound, args, 1);
    return retval & 0xff;
}
#endif

static Widget paned;

int main(int argc, char *argv[]) {
    Widget buttonpanel;
    Widget gamebutton, gamemenu, rulesbutton, rulesmenu;
    int i;
    struct button {
	const char *name; void (*func)(void);
    } *bp;
    static struct button buttons[] = {
	{ "Cards",	  	cmd_DealCards },
	{ "Undo",	  	cmd_UndoMove },
	{ "Redo",	  	cmd_RedoMove },
	{ "Score",	  	cmd_ShowScore },
	{ "Hint",	  	cmd_NextHint },
	{ "Move to Stack",	cmd_ToStack },
	/* { "Debug",		cmd_DebugOut }, */
#ifdef ONLINE_HELP
	{ "Help",	  	popup_help },
#endif
	{ "Save",	  	cmd_SaveGame }
    }, mbuttons[] = {
	{ "Drop Bookmark",	cmd_DropBookmark },
	{ "Goto Bookmark",	cmd_GotoBookmark },
	{ "Replay",		cmd_ReplayGame },
	{ "Restart",		cmd_RestartGame },
	{ "New Game",		rq_AnotherGame },
	{ "Quit",		rq_LeavePat }
    };

    /* use the command line arguments concerning the widgets */
    switch_uid(1);	/* switch to real uid for reading .Xauthority */
    toplevel = XtAppInitialize(&app_con, "XPat", options, XtNumber(options),
			       &argc, argv, fallback_resources, NULL, 0);
    switch_uid(0);	/* back to effective uid */
    process_extra_args(argc, argv);
    XtAppAddActions(app_con, moreActions, XtNumber(moreActions));
    graphics_control(Disable);

    /* basic elements */
    paned       = XtCreateManagedWidget("paned",	panedWidgetClass,    toplevel,	  NULL, 0);
    buttonpanel = XtCreateManagedWidget("buttonpanel",	boxWidgetClass,     paned,	  NULL, 0);
    messagebox = XtCreateManagedWidget("messages", labelWidgetClass, paned, NULL, 0);
    show_message(" ");

    container   = XtCreateManagedWidget("container",	viewportWidgetClass, paned,	  NULL, 0);
    desktop     = XtCreateManagedWidget("desktop",	tableauWidgetClass,  container,	  NULL, 0);
    XtAddCallback(container, XtNreportCallback, reportfunc, NULL);

    /* create the button panel and its menus */
    gamebutton  = XtCreateManagedWidget("Game", menuButtonWidgetClass, buttonpanel, NULL, 0);
    gamemenu    = XtCreatePopupShell("gamemenu", simpleMenuWidgetClass, gamebutton, NULL, 0);
    for (bp = mbuttons, i = 0; i < XtNumber(mbuttons); ++i) {
	Widget w;
	w = XtCreateManagedWidget(bp->name, smeBSBObjectClass, gamemenu, NULL, 0);
	if (bp->func) XtAddCallback(w, XtNcallback, perform_command, bp->func);
	++bp;
    }
    XtVaSetValues(gamebutton, XtNmenuName, "gamemenu", NULL);

    /* create Rules button just right of the Game button */
    rulesbutton = XtCreateManagedWidget("Rules", menuButtonWidgetClass, buttonpanel, NULL, 0);
    rulesmenu   = XtCreatePopupShell("rulesmenu", simpleMenuWidgetClass, rulesbutton, NULL, 0);
    {   struct rules **rp;
	for (rp = rulepool; *rp; ++rp) {
	    Widget w;
	    w = XtCreateManagedWidget((*rp)->shortname, smeBSBObjectClass, rulesmenu, NULL, 0);
	    XtAddCallback(w, XtNcallback, selectrules, NULL);
	}
    }
    XtVaSetValues(rulesbutton, XtNmenuName, "rulesmenu", NULL);

    /* rest of the buttons */
    for (bp = buttons, i = 0; i < XtNumber(buttons); ++i) {
	Widget w;
	w = XtCreateManagedWidget(bp->name, commandWidgetClass, buttonpanel, NULL, 0);
	if (bp->func) XtAddCallback(w, XtNcallback, perform_command, bp->func);
	++bp;
    }
#ifdef SOUND
    sound = XtCreateManagedWidget("Sound", toggleWidgetClass, buttonpanel, NULL, 0);
#endif

    /* OK. Now do the pop-up shells */
    popup = XtCreatePopupShell("prompt", transientShellWidgetClass, toplevel, NULL, 0);
    dialog = XtCreateManagedWidget("dialog", dialogWidgetClass, popup, NULL, 0);
    XawDialogAddButton(dialog, "ok",     Ok,     (XtPointer)dialog);
    XawDialogAddButton(dialog, "cancel", Cancel, (XtPointer)dialog);

#ifdef ONLINE_HELP
    create_help();
#endif
    graphic.w_width = graphic.w_height = 0;
    XtRealizeWidget(toplevel);
    mainwindow = XtWindow(toplevel);
    table  = XtWindow(desktop);
    XtRealizeWidget(popup);

    graphics_control(Enable);
    init_layout();			/* position the piles in a nice way */
    for (i = 0; i < game.numpiles; ++i)
	pile_resize(i);
    show_message(TXT_WELCOME, VERSION);
    XStoreName(dpy, mainwindow, rules.longname ? rules.longname : rules.shortname);
    if (!graphic.w_width) {
	graphic.w_width = graphic.width;
	graphic.w_height = graphic.height;
    }
    /* printf("before mainloop: seed is %ld\n", game.seed); */
    XtAppMainLoop(app_con);	/* does not return */
    return 0;			/* keep compiler happy */
}

void Force_Resize(XSize_t w, XSize_t h) {
    /* for Xlib, we can use this size. It does include space for buttons and messagebox */
#ifdef useXaw
    /* for Xaw, we have to add the size of the buttonbox and the message window */
    /* note that the result may have a wrong height, due to recalculation of the box
       widget. A second call to this function will always give the correct result. */
    /* The overall size can be computed by adding the difference between the current
       size of the mainwindow - current size of viewport window */
    {   Arg args[1];
        int hh;
	Dimension hhh = 0;
        XtSetArg(args[0], XtNheight, &hhh);
        XtGetValues(paned, args, 1);
	hh = hhh;		/* unsigned short => int */
        h += hh - graphic.w_height;	/* difference between overall size and Viewport size */
	/* sanity check! */
    }
#endif    
    XResizeWindow(dpy, mainwindow, w, h);
    /* or should XSetWMSizeHints() be used? */
}
#endif
