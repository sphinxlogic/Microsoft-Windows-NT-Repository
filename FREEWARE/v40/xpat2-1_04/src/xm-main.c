/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module Xm-main.c			     */
/*									     */
/*	main function for the Athena Widget interface			     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	March-1994							     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#ifdef useMotif
#ifdef ONLINE_HELP
#undef ONLINE_HELP	/* not here yet */
#endif

#include "X-pat.h"
#include "Tableau.h"
#include "version.h"

#include <Xm/CascadeB.h>
#include <Xm/ToggleB.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>

void create_help(Widget);
void popup_help(Widget, XtPointer, XtPointer);
void popdown_help(Widget, XtPointer, XtPointer);

Window table, mainwindow;
Widget toplevel;
static Widget messagebox, container, desktop, sbh, sbv;
static void perform_command(Widget widget, XtPointer client_data, XtPointer call_data) {
    (*(void (*)(void))client_data)();	/* any questions? */
}

void show_message(const char *str, ...) {
    XmString str2;
    static char last_message[256];
    Arg Args;
    va_list args;
    va_start(args, str);

    if (!str) {
	memset(last_message, ' ', sizeof(last_message)-1);
	last_message[sizeof(last_message)-1] = '\0';
    } else
	vsprintf(last_message, str, args);

    str2 = XmStringCreateSimple(last_message);
    XtSetArg(Args, XmNlabelString, str2);
    XtSetValues(messagebox, &Args, 1);
}



static XtAppContext app_con;
static void (*execfunc)(void) = NULL;
static Widget dialog;

void cmd_LeavePat(void) {
    play_sound("goodbye");
    XtDestroyApplicationContext(app_con);
    exit(0);
}

static void popup_confirm(const char *prompt) {
    Arg Args[2];
    XmString str2;
    str2 = XmStringCreateSimple(prompt);
    XtSetArg(Args[0], XmNmessageString, str2);
    XtSetValues(dialog, Args, 1);
    XtManageChild(dialog);
}

static void Cancel(Widget widget, XtPointer client_data, XtPointer call_data) {
    XtUnmanageChild(dialog);
    execfunc = NULL;
}
static void Ok(Widget widget, XtPointer client_data, XtPointer call_data) {
    (*execfunc)();	/* execute the desired function */
    Cancel(NULL, dialog, NULL);
}
void cmd_Cancel(void) {
    if (execfunc)
	Cancel(NULL, dialog, NULL);
}
void cmd_Confirm(void) {
    if (execfunc) {
	(*execfunc)();	/* execute the desired function */
	Cancel(NULL, dialog, NULL);
    }
}

void request_confirm(void (*dofunc)(void), const char *prompt) {
    if (execfunc)
	return;		/* request pending => deny another one */
    execfunc = dofunc;
    popup_confirm(prompt);
}

static String fallback_resources[] = { 
    /* will be filled out later */
    NULL,
};

static XrmOptionDescRec options[] = {
    /* tableau resources */
    { "-xpmdir",	"*Tableau.xpmdir",	XrmoptionSepArg, NULL },
#ifdef LOADCARDS
    { "-cards",		"*Tableau.cardset",	XrmoptionSepArg, NULL },
#endif
    { "-rules",		"*Tableau.rules",	XrmoptionSepArg, NULL },
    { "-faceup",	"*Tableau.faceup",	XrmoptionSepArg, NULL },
    { "-facedown",	"*Tableau.facedown",	XrmoptionSepArg, NULL },
    { "-decks",		"*Tableau.decks",	XrmoptionSepArg, NULL },
    { "-slots", 	"*Tableau.slots",	XrmoptionSepArg, NULL },
    { "-jokers",	"*Tableau.jokers",	XrmoptionSepArg, NULL },
    { "-tmps",		"*Tableau.tmps",	XrmoptionSepArg, NULL },
    { "-p0",        	"*Tableau.param0",	XrmoptionSepArg, NULL },
    { "-p1",        	"*Tableau.param1",	XrmoptionSepArg, NULL },
    { "-p2",		"*Tableau.param2",	XrmoptionSepArg, NULL },
    { "-p3",		"*Tableau.param3",	XrmoptionSepArg, NULL },
    { "-cb",        	"*Tableau.cardbackColor",XrmoptionSepArg,NULL },
    { "-tb",        	"*Tableau.background",	XrmoptionSepArg,NULL },
    { "-red",        	"*Tableau.RedColor",    XrmoptionSepArg, NULL },
    { "-markcolor",	"*Tableau.markColor",	XrmoptionSepArg, NULL },
    { "-arrowcolor",	"*Tableau.arrowColor",	XrmoptionSepArg, NULL },

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

/* type converter functions: */
static void mXtAP_Cancel (Widget w, XEvent *xev, String *params, Cardinal *num) { cmd_Cancel(); }
static void mXtAP_Confirm(Widget w, XEvent *xev, String *params, Cardinal *num) { cmd_Confirm(); }

static XtActionsRec moreActions[] = {
    { "HintNotify",	HintNotify },
    { "Cancel",		mXtAP_Cancel },
    { "Ok",		mXtAP_Confirm }
};

char *loadfilename;

static void process_extra_args(int argc, char *argv[]) {
    /* check extra args */
    game.seed = -1L;
    loadfilename = NULL;
    if (argc > 2) {
    usage:
	fprintf(stderr, "xpat: invalid argument: %s\n", argv[1]);
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
    return XmToggleButtonGetState(sound);
}
#endif

static Widget paned;

int main(int argc, char *argv[]) {
    Widget buttonpanel, main;
    Widget gamebutton, gamemenu, rulesbutton, rulesmenu, coloursbutton;
    int i;
    Arg Args[4];
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
    switch_uid(1);
    toplevel = XtAppInitialize(&app_con, "XPat", options, XtNumber(options), &argc, argv,
			       fallback_resources, NULL, 0);
    switch_uid(0);
    process_extra_args(argc, argv);
    XtAppAddActions(app_con, moreActions, XtNumber(moreActions));
    graphics_control(Disable);


    /* ab hier neu */

        main = XmCreateMainWindow (toplevel, "main", NULL, 0);
        XtManageChild (main);

    /* basic elements */
    paned       = XtCreateManagedWidget("paned",	xmPanedWindowWidgetClass,    main,	  NULL, 0);
/*    XtSetArg(args[0], XmNorientation, XmHORIZONTAL); */
    buttonpanel = XtCreateManagedWidget("buttonpanel",	xmRowColumnWidgetClass,     paned,	  NULL, 0);
    messagebox = XtCreateManagedWidget("messages", xmLabelWidgetClass, paned, NULL, 0);
    show_message(" ");

    container   = XtCreateManagedWidget("container",	xmScrolledWindowWidgetClass, paned,	  NULL, 0);
    desktop     = XtCreateManagedWidget("desktop",	tableauWidgetClass,  container,	  NULL, 0);


    /* create the button panel and its menus */
    gamebutton  = XtCreateManagedWidget("Game", xmCascadeButtonWidgetClass, buttonpanel, NULL, 0);
    gamemenu    = XmCreatePulldownMenu(buttonpanel, "gamemenu", NULL, 0);
    for (bp = mbuttons, i = 0; i < XtNumber(mbuttons); ++i) {
	Widget w;
	w = XtCreateManagedWidget(bp->name, xmPushButtonWidgetClass, gamemenu, NULL, 0);
	if (bp->func) XtAddCallback(w, XmNactivateCallback, perform_command, (XtPointer)bp->func);
	++bp;
    }
    XtVaSetValues(gamebutton, XmNsubMenuId, gamemenu, NULL);

    /* create Rules button just right of the Game button */
    rulesbutton = XtCreateManagedWidget("Rules", xmCascadeButtonWidgetClass, buttonpanel, NULL, 0);
    rulesmenu   = XmCreatePulldownMenu(buttonpanel, "rulesmenu", NULL, 0);
    {   struct rules **rp;
	for (rp = rulepool; *rp; ++rp) {
	    Widget w;
	    w = XtCreateManagedWidget((*rp)->shortname, xmPushButtonWidgetClass, rulesmenu, NULL, 0);
	    XtAddCallback(w, XmNactivateCallback, selectrules, NULL);
	}
    }
    XtVaSetValues(rulesbutton, XmNsubMenuId, rulesmenu, NULL);

    /* rest of the buttons */
    for (bp = buttons, i = 0; i < XtNumber(buttons); ++i) {
	Widget w;
	w = XtCreateManagedWidget(bp->name, xmPushButtonWidgetClass, buttonpanel, NULL, 0);
	if (bp->func) XtAddCallback(w, XmNactivateCallback, perform_command, (XtPointer)bp->func);
	++bp;
    }

#ifdef SOUND
    sound = XtCreateManagedWidget("Sound", xmToggleButtonWidgetClass, buttonpanel, NULL, 0);
#endif

    dialog = XmCreateQuestionDialog(toplevel, "dialog", NULL, 0);
    XtAddCallback(dialog, XmNokCallback, Ok, NULL);
    XtAddCallback(dialog, XmNcancelCallback, Cancel, NULL);
    {   Widget tmp;	/* throw away help button */
	tmp = XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(tmp);
	XtSetArg(Args[0], XmNhorizontalScrollBar, &sbh);
	XtGetValues(container, Args, 1);
	XtSetArg(Args[0], XmNverticalScrollBar, &sbv);
	XtGetValues(container, Args, 1);
    }

#ifdef ONLINE_HELP
    create_help(toplevel);
#endif
    graphic.w_width = graphic.w_height = 0;
    XtRealizeWidget(toplevel);
    mainwindow = XtWindow(toplevel);
    table  = XtWindow(desktop);


    {
	XSetWindowAttributes winattr;
	long winmask;
	winattr.backing_store = card.cardset ? WhenMapped : NotUseful;
	winattr.border_pixel = blackpixel;
	winattr.bit_gravity = NorthWestGravity;
	winattr.event_mask = KeyPressMask | ExposureMask | ButtonPressMask | 
	    ButtonReleaseMask | StructureNotifyMask | Button3MotionMask;
	winmask = CWBorderPixel | CWEventMask | CWBackingStore | CWBitGravity;
	XChangeWindowAttributes(dpy, table, winmask, &winattr);
    }

    graphics_control(Enable);
#ifdef LABER
    printf("main function requests init_layout:\n");
#endif
    init_layout();			/* position the piles in a nice way */
    for (i = 0; i < game.numpiles; ++i)
	pile_resize(i);
    show_message(TXT_WELCOME, VERSION);
    XStoreName(dpy, mainwindow, rules.longname ? rules.longname : rules.shortname);
    if (!graphic.w_width) {
	graphic.w_width = graphic.width;
	graphic.w_height = graphic.height;
    }
    XtAppMainLoop(app_con);	/* does not return */
    return 0;			/* keep compiler happy */
}

void Force_Resize(XSize_t w, XSize_t h) {
    /* for Xlib, we can use this size. It does include space for buttons and messagebox */
    /* for Xaw, we have to add the size of the buttonbox and the message window */
    /* note that the result may have a wrong height, due to recalculation of the box
       widget. A second call to this function will always give the correct result. */
    /* The overall size can be computed by adding the difference between the current
       size of the mainwindow - current size of viewport window */
#if 0
    {   Arg args[1];
        int hh, ww;
	hh = 0; ww = 0;
        XtSetArg(args[0], XmNheight, &hh);
        XtGetValues(sbv, args, 1);
        XtSetArg(args[0], XmNwidth, &ww);
        XtGetValues(sbh, args, 1);
	printf("viewport hat groesse %d,%d\n", ww, hh);
        h += hh - graphic.w_height;	/* difference between overall size and Viewport size */
	w += ww - graphic.w_width;
    }
#endif
    w += 12;	/* quick, dirty */
    h += 83;
    XResizeWindow(dpy, mainwindow, w, h);
    /* or should XSetWMSizeHints() be used? */
}
#endif
