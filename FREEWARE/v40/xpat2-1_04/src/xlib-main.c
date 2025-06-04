/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module Xlib-main.c			     */
/*									     */
/*	main function for the Xlib interface				     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#ifdef useXlib

#include "X-pat.h"
#include "version.h"

Window table, mainwindow;
char *loadfilename = NULL;

static int audio = 1;
int checksound(void) {
    return audio;
}

void cmd_LeavePat(void) {
    play_sound("goodbye");
    exit(0);
}

static unsigned long color_to_pixel(const char *colorname) {
    if (colorname) {
	XColor color;
	Colormap cmap;
	cmap = DefaultColormap(dpy, screen);
	color.flags = DoRed | DoGreen | DoBlue;
	XAllocNamedColor(dpy, cmap, colorname, &color, &color);
	return color.pixel;
    } else
	return BlackPixel(dpy, screen);
}


static void usage(const char *arg)
{   fprintf(stderr, "usage: pat [options] [<seed> or <savedgame>]\n");
    fprintf(stderr, "valid options are:\n");
    fprintf(stderr, "-display <display>      set display\n");
    fprintf(stderr, "-geometry <geometry>    set initial geometry\n");
#ifdef LOADCARDS
    fprintf(stderr, "-cards <cardset>        set cardset to external file\n");
#endif
    fprintf(stderr, "-xpmdir <directory>     for internal cardset: load xpm files\n");
    fprintf(stderr, "-sound <0/1>            enables/disables sound\n");

    fprintf(stderr, "-tb <backgroundcolor>   set background color of tableau\n");
    fprintf(stderr, "-cb <cardbackcolor>     set cardback color\n");
    fprintf(stderr, "-markcolor <markcolor>  set color of card marks\n");
    fprintf(stderr, "-markwidth <markwidth>  set width of card marks\n");
    fprintf(stderr, "-bfont <fontname>       set font for buttons\n");
    fprintf(stderr, "-sfont <fontname>       set font for status line\n");
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
    fprintf(stderr, "-by <buttongap>         set button gap y size\n");
    fprintf(stderr, "-bx <buttongap>         set button gap x size\n");
    fprintf(stderr, "-cround <cornersize>    set size of round card corners (0 to 20)\n");
    fprintf(stderr, "-bround <cornersize>    set size of round button corners (0 to 20)\n");
#endif
    if (arg)
 	fprintf(stderr,"argument \"%s\" caused this message\n", arg);
    exit(EXIT_FAILURE);
}

static void init_display(const char *display)
{
    if ((dpy = XOpenDisplay(display)) == NULL)	{
	fprintf(stderr,"Can't open display %s\n", 
		(display ? display : getenv("DISPLAY")));
	exit(EXIT_FAILURE);
    }
    screen = DefaultScreen(dpy);
    graphic.is_color = (DisplayCells(dpy, screen) > 2);
#ifdef DEBUG
    XSynchronize(dpy, True);
#endif
}



/* enum { String, Number, Bool } argtype; */
#define String 0
#define Number 1
#define Self   2

int main(int argc, char **argv)
{   int i;
    long seed = -1L;
    const char *restoregame = NULL;
    static const char *display = NULL, *cmdname;

    /* these must be static, since their address is taken */
    static int slots, faceup, facedown, decks, p0, p1, p2, p3; /* rules customization */
    static int linewidth, cround, bround, gapx, gapy, bx, by, autolayout, mem;
    static int tmps, arrw, arrh, jokers, sound = 1;
    static const char *geometry, *ruleset, *cbcolor, *bkcolor = "DarkKhaki";
    static const char *buttonfont, *statusfont, *cardset = NULL, *markcolor, *xpmdir;
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
    { "-flips",     "Flips",	     NULL,        &p1,	      Number, -1,999 },
    { "-turn",      "Turn",	     NULL,        &p2,        Number, -1,  9 },
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
    { "-bround",    "ButtonRound",   "4",         &bround,    Number,  0, 20 },
    { "-sound",     "Sound",         "1",         &sound,     Number,  0,  1 },
    { "-bx",        "ButtonGapX",    "4",         &bx,        Number,  0, 99 },
    { "-by",        "ButtonGapY",    "4",         &by,        Number,  0, 99 },
    { "-aw",        "ArrowWidth",    NULL,        &arrw,      Number, -1, 30 },
    { "-ah",        "ArrowHeight",   NULL,        &arrh,      Number, -1, 30 },
    { "-gapx",      "GapX",          "4",         &gapx,      Number,  0, 99 },
    { "-gapy",      "GapY",          "4",         &gapy,      Number,  0, 99 },
    { "-rng",       "Rng",	     "1",	  &rng,       Number,  0,  1 },
    { "-cb",        "CardbackColor", "SlateGrey", &cbcolor,   String },
    { "-tb",   	    "TableColor",    "DarkKhaki", &bkcolor,   String },
    { "-markcolor", "MarkColor",     NULL,        &markcolor, String },
    { "-markwidth", "MarkWidth",     "999",       &linewidth, Number,  1,999 },
    { "-arrowcolor","ArrowColor",    "Gold",      &arrowcolor,String },
    { "-bfont",     "ButtonFont",    NULL,        &buttonfont,String },
    { "-sfont",     "StatusFont",    "fixed",     &statusfont,String },
    { "-geometry",  "Geometry",      NULL,        &geometry,  String },
    { "-display",   NULL,            NULL,        &display,   String }
};
#define NUMOPTIONS      (sizeof(options) / sizeof(options[0]) - 1)

    for (i = 0; i <= NUMOPTIONS; ++i)
        options[i].cmdarg = NULL;

    if (!(cmdname = strrchr(argv[0], '/')))
	cmdname = argv[0];
    else
	++cmdname;
    if (strncmp(cmdname, "xpat", 4))
	options[0].cmdarg = cmdname;

    for (i = 1; i < argc - 1; ++i) {    /* merge command line options */
	int j;
	if (*argv[i] != '-')
	    break;
	for (j = 0; j <= NUMOPTIONS; ++j) {
	    if (!strcmp(argv[i], options[j].option)) {
		options[j].cmdarg = options[j].type == Self ? "1" : argv[++i];
		break;
	    }
        }
        if (j > NUMOPTIONS)
	    usage(argv[i]);
    }
    if (i < argc) {		/* seed or savegame left */
	if (i != argc - 1)
	    usage(argv[i]);
	if (isdigit(*argv[i])) {
	    seed = atol(argv[i]);
	    if (seed < 0L || seed >= 1000000000L)
		usage(argv[i]);
	} else
	    if (*argv[i] != '-')
		restoregame = loadfilename = argv[i];
	    else
		usage(argv[i]);
    }
    display = options[NUMOPTIONS].cmdarg;

    init_display(display);		/* connect to X-server */

    for (i = 0; i < NUMOPTIONS; ++i) {  /* get resources from server */
	char *s;
        if (options[i].cmdarg)
	    options[i].value = options[i].cmdarg;
        else if ((s = XGetDefault(dpy, "XPat", options[i].resource)))
	    options[i].value = s;
    }

    for (i = 0; i < NUMOPTIONS; ++i) {  /* value-check Numbers */
	int n;
	struct option *o;
	o = options + i;
        if (o->type == Number) {
	    n = o->value ? atoi(o->value) : -1;
	    if (n < o->minval || n > o->maxval) {
		char s[100];
		sprintf(s, "argument to option \"%s\" / resource \"%s\"",
                    o->option, o->resource);
                usage(s);
            }
            *(int *)(o->where) = n;
        } else if (o->type == Self) {
            *(int *)(o->where) = 1;
        } else /* type == String */
            *(const char **)(o->where) = o->value;
    }
    if (!sound)
	audio = 0;

    if (restoregame &&
       (decks != -1 || slots != -1 || faceup != -1 || facedown != -1))
	  fprintf(stderr, "warning: customization parameters are ignored\n");


    {  const char *s; langdir = (s = getenv("LANG")) ? s : ""; }
#ifndef VMS
    read_message_file("messages");
    read_keyboard_file("keys");
#else
    read_message_file("messages.");
    read_keyboard_file("keys.");
#endif

    graphics_control(Disable);
    if (restoregame)
        load_game(restoregame);
    else {
	new_rules(ruleset, decks, slots, faceup, facedown, jokers, tmps, p0, p1, p2, p3);
	newgame(seed);
    }

    init_gfx();
    init_fonts(statusfont, buttonfont);	/* make GCs */
    init_cards(cardset, cround, cround, color_to_pixel("Red3"), color_to_pixel(cbcolor),
	       mem, xpmdir);	/* set size of cards, make pixmaps */
    init_windows(argc, argv, geometry, gapx, gapy, bround, bround,
	bx, by, autolayout, bkcolor); /* size window */
    init_mark(color_to_pixel(markcolor), linewidth);	/* initialize card marks */
    init_arrow(arrowcolor, arrw, arrh);

    graphics_control(Enable);
    init_layout();			/* position the piles in a nice way */
    for (i = 0; i < game.numpiles; ++i)
	pile_resize(i);
    /* refresh_screen(); */
    show_message(TXT_WELCOME, VERSION);
    mainwindow = table;
    event_loop();		/* does not return */
    return 0;			/* keep compiler happy */
}

void Force_Resize(XSize_t w, XSize_t h) {
    /* for Xlib, we can use this size. It does include space for buttons and messagebox */
    XResizeWindow(dpy, mainwindow, w, h);
    /* or should XSetWMSizeHints() be used? */
}
#endif

