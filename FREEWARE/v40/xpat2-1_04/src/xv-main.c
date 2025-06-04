/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module Xv-main.c			     */
/*									     */
/*	main function for the Xview interface				     */
/*	written by Brandon S. Allbery					     */
/*	based on code by Heiko Eissfeldt and Michael Bischoff		     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/

#ifdef useXview

#undef ONLINE_HELP	/* not supported yet */

#include "X-pat.h"
#include "version.h"
#include "pat.bm"

Window table;
Frame mainwindow;
Panel buttonbox;
Canvas canvas;
Menu cmenu;
Event *bp_event;

static int audio = 1;
static Xv_Notice confirm_dialog;
static Panel_item sound_button;
static void (*execfunc)(void);

#define C_TABLE CMS_CONTROL_COLORS + 0
#define C_WHITE CMS_CONTROL_COLORS + 1
#define C_RED CMS_CONTROL_COLORS + 2
#define C_BACK CMS_CONTROL_COLORS + 3
#define C_ARROW CMS_CONTROL_COLORS + 4
#define C_MARK CMS_CONTROL_COLORS + 5
#define C_BLACK	CMS_CONTROL_COLORS + 6

int
checksound(void)
{
    return audio;
}

void
cmd_LeavePat(void)
{
    play_sound("goodbye");
    xv_destroy_safe(mainwindow);
}

void cmd_Confirm(void) {
    if (execfunc) {
	void (*execfunc2)(void) = execfunc;	/* erase it first! */
	execfunc = NULL;
	/* XtPopdown(popup); */
	(*execfunc2)();	/* finally execute the desired function */
    }
}
void cmd_Cancel(void) {
    if (execfunc) {
	execfunc = NULL;
	/* XtPopdown(popup); */
    }
}

static void
confirm_action(Xv_Notice notice, int value, Event ev)
{
    if (value == NOTICE_YES)
	(*execfunc)();
    execfunc = 0;
}

void
request_confirm(void (*dofunc)(void), const char *prompt)
{
    if (execfunc)
	return;
    execfunc = dofunc;
    xv_set(confirm_dialog,
	   NOTICE_MESSAGE_STRING, prompt,
	   XV_SHOW, TRUE,
	   0);
}

static void
set_sound(Xv_Notice notice, Event ev)
{
    audio = !audio;
    xv_set(sound_button,
	   PANEL_LABEL_STRING, (audio? "Quiet": "Sound"),
	   0);
}

static void
selectrules(Menu mnu, Menu_item mi)
{
    change_rules((char *) xv_get(mi, MENU_STRING));
}

static const char *
d_mk_name(const char *owner, const char *name)
{
    static char buf[128];

    strcpy(buf, owner);
    strcat(buf, ".");
    strcat(buf, name);
    return buf;
}

static const char *
d_get_string(const char *cmd, const char *res, const char *dft)
{
    char cbuf[128], Cbuf[128];

    strcpy(cbuf, d_mk_name(cmd, res));
    strcpy(Cbuf, d_mk_name("XPat", res));
    return defaults_get_string(cbuf, Cbuf, (char *) dft);
}

static int
d_get_number(const char *cmd, const char *res, const char *dftv, int minv,
	     int maxv)
{
    char cbuf[128], Cbuf[128];
    int dft;

    if (dftv && *dftv)
	dft = atoi(dftv);
    else
	dft = -1;
    strcpy(cbuf, d_mk_name(cmd, res));
    strcpy(Cbuf, d_mk_name("XPat", res));
    return defaults_get_integer_check(cbuf, Cbuf, dft, minv, maxv);
}

static void
usage(const char *arg)
{
    fprintf(stderr, "usage: pat [options] [<seed> or <savedgame>]\n");
    fprintf(stderr, "valid options are XView options ans:\n");
#ifdef LOADCARDS
    fprintf(stderr, "-cards <cardset>        set cardset to external file\n");
#endif
    fprintf(stderr, "-xpmdir <directory>     for internal cardset: load xpm files\n");
    fprintf(stderr, "-sound <0/1>            enables/disables sound\n");

    fprintf(stderr, "-tb <backgroundcolor>   set background color of tableau\n");
    fprintf(stderr, "-cb <cardbackcolor>     set cardback color\n");
    fprintf(stderr, "-markcolor <markcolor>  set color of card marks\n");
    fprintf(stderr, "-markwidth <markwidth>  set width of card marks\n");
    fprintf(stderr, "-gapx, -gapy            set space between cards (0 to 20)\n");
    fprintf(stderr, "\ngame customization:\n");
    fprintf(stderr, "-rules <ruleset>        set rules to HM or Spider or Stairs\n");
    fprintf(stderr, "-slots <slots>          set number of slots (2 to 60)\n");
    fprintf(stderr, "-decks <decks>          set number of decks (1 to 9)\n");
    fprintf(stderr, "-tmps <tmps>            set number of tmps (1 to 9)\n");
    fprintf(stderr, "-faceup <faceup>        set initial deal parameter (0 to 20)\n");
    fprintf(stderr, "-facedown <facedown>    set initial deal parameter (0 to 20)\n");
    fprintf(stderr, "-jokers <jokers>        set number of jokers (do not use, it's too easy)\n");
    fprintf(stderr, "-flips <flips>          limit number of flips (0 to 99)\n");
    fprintf(stderr, "-relaxed 0|1            choose easy or hard variant\n");
    fprintf(stderr, "-turn <turn>            set number of cards to turn (1 to 9)\n");
    fprintf(stderr, "-rotations <num>        set maximum number of slot rotations (0 to 9)\n");
    fprintf(stderr, "-p{0,1,2,3} <param>     set various rule parameters\n");
#if 0	/* for insiders only */
    fprintf(stderr, "-autolayout <0/1>       0: to avoid automatic re-layout at resize time\n");
    fprintf(stderr, "-mem <0/1>              0: do not store card images at full depth internally\n");
    fprintf(stderr, "-cround <cornersize>    set size of round card corners (0 to 20)\n");
#endif
    if (arg)
 	fprintf(stderr,"argument \"%s\" caused this message\n", arg);
    exit(EXIT_FAILURE);
}

/* enum { String, Number, Bool } argtype; */
#define String 0
#define Number 1

int
main(int argc, char **argv)
{
    int i;
    long seed = -1L;
    const char *restoregame = NULL;
    XColor pixels[CMS_CONTROL_COLORS + 7];
    Cms cms;
    Server_image icon_map;
    Icon pat_icon;
    Menu mnu;
    static const char *cmdname;

    /* these must be static, since their address is taken */
    static int slots, faceup, facedown, decks, p0, p1, p2, p3; /* rules customization */
    static int linewidth, cround, gapx, gapy, autolayout, mem;
    static int tmps, arrw, arrh, jokers, bx, by, sound = 1;
    static const char *ruleset, *cbcolor, *bkcolor = "DarkKhaki";
    static const char *cardset = NULL, *markcolor, *xpmdir;
    static const char *arrowcolor;

static struct option {
    const char *option;
    const char *resource;
    const char *value;
    void *where;
    int type;
    int minval;       /* only for Number */
    int maxval;       /* only for Number */
    const char *cmdarg;
} options[] = {
    /* first option must be -rules */
    { "-rules",	    "Rules",	     "Gypsy",     &ruleset,   String },
    { "-autolayout","AutoLayout",    "1",         &autolayout,Number,  0,  1 },
    { "-mem",       "Mem",           "1",         &mem,       Number,  0,  1 },
    { "-xpmdir",    "XpmDir",        NULL,        &xpmdir,    String },
#ifdef LOADCARDS
    { "-cards",     "Cards",         NULL,        &cardset,   String },
#endif
    { "-slots",     "Slots",	     NULL,        &slots,     Number, -1, MAXPILES },
    { "-relaxed",   "relaxed",	     NULL,        &p0,        Number, -1,  9 },
    { "-flips",     "flips",	     NULL,        &p1,        Number, -1, 99 },
    { "-turn",      "turn",	     NULL,        &p2,        Number, -1,  9 },
    { "-rotations", "rotations",     NULL,        &p3,        Number, -1,  9 },
    { "-decks",     "Decks",         NULL,        &decks,     Number, -1,  9 },
    { "-tmps",      "Tmps",          NULL,        &tmps,      Number, -1, 16 },
    { "-faceup",    "Faceup",        NULL,        &faceup,    Number, -1, 16 },
    { "-facedown",  "Facedown",      NULL,        &facedown,  Number, -1, 16 },
    { "-jokers",    "Jokers", 	     NULL,        &jokers,    Number, -1, 16 },
    { "-cround",    "CardRound",     NULL,        &cround,    Number, -1, 99 },
    { "-sound",     "Sound",         "1",         &sound,     Number,  0,  1 },
    { "-bx",        "ButtonGapX",    "4",         &bx,        Number,  0, 99 },
    { "-by",        "ButtonGapY",    "4",         &by,        Number,  0, 99 },
    { "-aw",        "ArrowWidth",    NULL,        &arrw,      Number, -1, 30 },
    { "-ah",        "ArrowHeight",   NULL,        &arrh,      Number, -1, 30 },
    { "-gapx",      "GapX",          "4",         &gapx,      Number,  0, 99 },
    { "-gapy",      "GapY",          "4",         &gapy,      Number,  0, 99 },
    { "-cb",        "CardbackColor", "SlateGrey", &cbcolor,   String },
    { "-tb",   	    "TableColor",    "LightSlateGrey", &bkcolor,   String },
    { "-markcolor", "MarkColor",     "black",        &markcolor, String },
    { "-markwidth", "MarkWidth",     "999",       &linewidth, Number,  1,999 },
    { "-arrowcolor","ArrowColor",    "Gold",      &arrowcolor,String },
};
#define NUMOPTIONS      (sizeof(options) / sizeof(options[0]) - 1)

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);

    for (i = 0; i <= NUMOPTIONS; i++)
        options[i].cmdarg = NULL;

    if ((cmdname = strrchr(argv[0], '/')))
	++cmdname;
    else
	cmdname = argv[0];
    if (strncmp(cmdname, "xpat", 4))
	options[0].cmdarg = cmdname;

    for (i = 1; i < argc - 1; i++)    /* merge command line options */
    {
	int j;

	if (*argv[i] != '-')
	    break;
	for (j = 0; j <= NUMOPTIONS; j++)
	{
	    if (strcmp(argv[i], options[j].option) == 0)
	    {
		/* should range-check it */
		options[j].cmdarg = argv[++i];
		break;
	    }
        }
        if (j > NUMOPTIONS)
	    usage(argv[i]);
    }
    if (i < argc)		/* seed or savegame left */
    {
	if (i != argc - 1)
	    usage(argv[i]);
	if (isdigit(*argv[i]))
	{
	    seed = atol(argv[i]);
	    if (seed < 0L || seed >= 1000000000L)
		usage(argv[i]);
	}
	else if (*argv[i] != '-')
	    restoregame = argv[i];
	else
	    usage(argv[i]);
    }

    /*
     * XView doesn't automatically use the resource database.  The next
     * step after getting this working is to add XV_INSTANCE_NAME and
     * XV_USE_DB entries such that resources are automatically used.
     *
     * One problem here is that I don't think I can get away with just
     * passing a background color a' la Xlib/Xt.  :-(
     */
    for (i = 0; i <= NUMOPTIONS; i++)    /* get resources from server */
    {
	switch (options[i].type)
	{
	case String:
	    *((const char **) options[i].where) =
		(options[i].cmdarg? options[i].cmdarg:
		 d_get_string(cmdname, options[i].resource, options[i].value));
	    break;
	case Number:
	    *((int *) options[i].where) =
		(options[i].cmdarg? atoi(options[i].cmdarg):
		 d_get_number(cmdname, options[i].resource,
			      options[i].value, options[i].minval,
			      options[i].maxval));
	    break;
	}
    }

    if (!sound)
	audio = 0;

    if (restoregame &&
	(decks != -1 || slots != -1 || faceup != -1 || facedown != -1))
	fprintf(stderr, "warning: customization parameters are ignored\n");

    {
	const char *s;

	langdir = (s = getenv("LANG")) ? s : "";
    }
    read_message_file("messages");
    read_keyboard_file("keys");

    graphics_control(Disable);
    if (restoregame)
        load_game(restoregame);
    else
    {
	new_rules(ruleset, decks, slots, faceup, facedown, jokers, tmps, p0,
		  p1, p2, p3);
	newgame(seed);
    }

    dpy = (Display *) xv_get(xv_default_server, XV_DISPLAY);
    screen = DefaultScreen(dpy);
    graphic.is_color = (DisplayCells(dpy, screen) > 2);
    init_gfx();
    if (!graphic.is_color)
    {
	cms = 0;
	pixels[C_ARROW].pixel = whitepixel;
	pixels[C_WHITE].pixel = whitepixel;
	pixels[C_RED].pixel = blackpixel;
	pixels[C_BACK].pixel = blackpixel;
	pixels[C_TABLE].pixel = blackpixel;
	pixels[C_MARK].pixel = blackpixel;
	pixels[C_BLACK].pixel = blackpixel;
    }
    else
    {
	cms = (Cms) xv_create(0, CMS,
			      CMS_SIZE, CMS_CONTROL_COLORS + 7,
			      CMS_CONTROL_CMS, TRUE,
			      CMS_NAMED_COLORS,
    			          bkcolor, "white", "Red3", cbcolor,
			          arrowcolor, markcolor, "black", 0,
			      0);
	xv_get(cms, CMS_X_COLORS, pixels);
    }
    
    init_cards(cardset, cround, cround, pixels[C_RED].pixel,
	       pixels[C_BACK].pixel, mem, xpmdir);

    graphic.xgap = (XSize_t) gapx;
    graphic.ygap = (XSize_t) gapy;
    graphic.autolayout = autolayout;
    /* compute minimum size needed */
    generic_minwindow((XSize_t *) &graphic.min_width,
		      (XSize_t *) &graphic.min_height);
    graphic.width = graphic.min_width;
    graphic.height = graphic.min_height;
    mainwindow = (Frame) xv_create(0, FRAME,
				   XV_INSTANCE_NAME, "xpat",
#if 0
				   FRAME_LABEL, cmdname,
#endif
				   FRAME_LABEL,
				       (rules.longname? rules.longname:
							rules.shortname),
				   FRAME_SHOW_HEADER, TRUE,
				   FRAME_SHOW_FOOTER, TRUE,
				   FRAME_LEFT_FOOTER, "",
				   FRAME_RIGHT_FOOTER, "",
				   FRAME_WM_COMMAND_ARGC_ARGV, argc, argv,
				   0);
    icon_map = (Server_image) xv_create(0, SERVER_IMAGE,
					XV_WIDTH, pat_width,
					XV_HEIGHT, pat_height,
					SERVER_IMAGE_X_BITS, pat_bits,
					0);
    pat_icon = (Icon) xv_create(mainwindow, ICON,
				ICON_IMAGE, icon_map,
				0);
    xv_set(mainwindow, FRAME_ICON, pat_icon, 0);
    init_mark(pixels[C_MARK].pixel, linewidth);
    init_arrow(pixels[C_ARROW].pixel, arrw, arrh);

    buttonbox = (Panel) xv_create(mainwindow, PANEL,
				  XV_INSTANCE_NAME, "buttonpanel",
				  PANEL_ITEM_X_GAP, bx,
				  PANEL_ITEM_Y_GAP, by,
				  0);
    mnu = (Menu) xv_create(0, MENU,
			   MENU_GEN_PIN_WINDOW, mainwindow, "Game",
			   MENU_ACTION_ITEM, "Drop Bookmark", cmd_DropBookmark,
			   MENU_ACTION_ITEM, "Goto Bookmark", cmd_GotoBookmark,
			   MENU_ACTION_ITEM, "Replay", cmd_ReplayGame,
			   MENU_ACTION_ITEM, "Restart", cmd_RestartGame,
			   MENU_ACTION_ITEM, "New Game", rq_AnotherGame,
			   MENU_ACTION_ITEM, "Quit", rq_LeavePat,
			   0);
    xv_create(buttonbox, PANEL_BUTTON,
	      XV_INSTANCE_NAME, "game",
	      PANEL_LABEL_STRING, "Game",
	      PANEL_ITEM_MENU, mnu,
	      0);
    cmenu = (Menu) xv_create(0, MENU,
			     MENU_GEN_PIN_WINDOW, mainwindow, "XPat",
			     MENU_TITLE_ITEM, "XPat",
			     MENU_ACTION_ITEM, "New Game", rq_AnotherGame,
			     MENU_ACTION_ITEM, "Bookmark", cmd_DropBookmark,
			     MENU_ACTION_ITEM, "Deal", cmd_DealCards,
			     MENU_ACTION_ITEM, "Stack", cmd_ToStack,
			     0);
    mnu = (Menu) xv_create(0, MENU,
			   MENU_GEN_PIN_WINDOW, mainwindow, "Rules",
			   0);
    {
	extern struct rules *rulepool[];
	struct rules **rp;
	Menu_item mi;

	for (rp = rulepool; *rp; rp++)
	{
	    mi = (Menu_item) xv_create(0, MENUITEM,
				       MENU_STRING, (*rp)->shortname,
				       MENU_NOTIFY_PROC, selectrules,
				       MENU_RELEASE,
				       0);
	    xv_set(mnu, MENU_APPEND_ITEM, mi, 0);
	}
    }
    xv_create(buttonbox, PANEL_BUTTON,
	      XV_INSTANCE_NAME, "rules",
	      PANEL_LABEL_STRING, "Rules",
	      PANEL_ITEM_MENU, mnu,
	      0);
    
    xv_create(buttonbox, PANEL_BUTTON,
	      XV_INSTANCE_NAME, "cards",
	      PANEL_LABEL_STRING, "Cards",
	      PANEL_NOTIFY_PROC, cmd_DealCards,
	      0);
    xv_create(buttonbox, PANEL_BUTTON,
	      XV_INSTANCE_NAME, "undo",
	      PANEL_LABEL_STRING, "Undo",
	      PANEL_NOTIFY_PROC, cmd_UndoMove,
	      0);
    xv_create(buttonbox, PANEL_BUTTON,
	      XV_INSTANCE_NAME, "redo",
	      PANEL_LABEL_STRING, "Redo",
	      PANEL_NOTIFY_PROC, cmd_RedoMove,
	      0);
    xv_create(buttonbox, PANEL_BUTTON,
	      XV_INSTANCE_NAME, "score",
	      PANEL_LABEL_STRING, "Score",
	      PANEL_NOTIFY_PROC, cmd_ShowScore,
	      0);
    xv_create(buttonbox, PANEL_BUTTON,
	      XV_INSTANCE_NAME, "hint",
	      PANEL_LABEL_STRING, "Hint",
	      PANEL_NOTIFY_PROC, cmd_NextHint,
	      0);
    xv_create(buttonbox, PANEL_BUTTON,
	      XV_INSTANCE_NAME, "stack",
	      PANEL_LABEL_STRING, "Move to Stack",
	      PANEL_NOTIFY_PROC, cmd_ToStack,
	      0);
#ifdef ONLINE_HELP
    /* should be a window button... */
    /* The Help window contains a Textsw and a "Topics" menubutton. */
    xv_create(buttonbox, PANEL_BUTTON,
	      XV_INSTANCE_NAME, "help",
	      PANEL_LABEL_STRING, "Help...",
	      PANEL_NOTIFY_PROC, popup_help,
	      0);
#endif
    xv_create(buttonbox, PANEL_BUTTON,
	      XV_INSTANCE_NAME, "save",
	      PANEL_LABEL_STRING, "Save",
	      PANEL_NOTIFY_PROC, cmd_SaveGame,
	      0);
#ifdef SOUND
#if 0
    /* this is ugly.  UGLY! */
    xv_create(buttonbox, PANEL_TOGGLE,
	      XV_INSTANCE_NAME, "sound",
	      PANEL_LABEL_STRING, "",
	      PANEL_CHOOSE_ONE, FALSE,
	      PANEL_CHOICE_STRINGS,
	          "Sound",
	          0,
	      PANEL_VALUE, sound,
	      PANEL_NOTIFY_PROC, set_sound,
	      0);
#endif
    sound_button = xv_create(buttonbox, PANEL_BUTTON,
			     XV_INSTANCE_NAME, "sound",
			     PANEL_LABEL_STRING, (audio? "Quiet": "Sound"),
			     PANEL_NOTIFY_PROC, set_sound,
			     0);
#endif
    window_fit(buttonbox);

    confirm_dialog = (Xv_Notice) xv_create(mainwindow, NOTICE,
					   XV_INSTANCE_NAME, "prompt",
					   NOTICE_NO_BEEPING, TRUE,
					   NOTICE_BUTTON_YES, "Yes",
					   NOTICE_BUTTON_NO, "No",
					   NOTICE_EVENT_PROC, confirm_action,
					   0);

    /* frame size forwarding fails when borders are on */
    canvas = (Canvas) xv_create(mainwindow, CANVAS,
				XV_INSTANCE_NAME, "desktop",
				CANVAS_MIN_PAINT_WIDTH, graphic.min_width,
				CANVAS_MIN_PAINT_HEIGHT, graphic.min_height,
				CANVAS_WIDTH, graphic.min_width,
				CANVAS_HEIGHT, graphic.min_height,
				CANVAS_RESIZE_PROC, table_resize,
				CANVAS_REPAINT_PROC, table_repaint,
				WIN_BELOW, buttonbox,
				XV_X, 0,
				0);
    xv_set(canvas_paint_window(canvas),
	   WIN_EVENT_PROC, table_events,
	   WIN_CONSUME_EVENTS,
	       WIN_ASCII_EVENTS, WIN_MOUSE_BUTTONS, 0,
	   WIN_CONSUME_X_EVENT_MASK, Button3MotionMask,
	   (cms? WIN_CMS: 0), cms,
	   WIN_BACKGROUND_COLOR, C_TABLE,
	   0);
    table = (Window) xv_get(canvas_paint_window(canvas), XV_XID);
    frame_pack_all(mainwindow);

    graphics_control(Enable);
    init_layout();			/* position the piles in a nice way */
    for (i = 0; i < game.numpiles; ++i)
	pile_resize(i);
    show_message(TXT_WELCOME, VERSION);
    xv_main_loop(mainwindow);
    return 0;
}

void
show_message(const char *fmt, ...)
{
    static char msg[512];
    va_list args;

    if (!fmt)
	return;
    va_start(args, fmt);
    vsprintf(msg, fmt, args);
    va_end(args);
    xv_set(mainwindow, FRAME_LEFT_FOOTER, msg, 0);
}

#endif
