/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module Xaw-widget.c			     */
/*									     */
/*	A pseudo-widget for the Athena Widget interface			     */
/*	written by Michael Bischoff (mbi@mo.math.nat.tu-bs.de)		     */
/*	March-1994							     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#if defined(useXaw) || defined(useMotif)
#include "X-pat.h"

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "TableauP.h"

static XtResource resources[] = {
#define offset(field) XtOffsetOf(TableauRec, tableau.field)
    /* {name, class, type, size, offset, default_type, default_addr}, */
    {XtNmessageFile,XtCMessageFile,XtRString,sizeof(String),  offset(messagefile), XtRString, NULL},
    {XtNkeyboardFile,XtCKeyboardFile,XtRString,sizeof(String),offset(keyboardfile),XtRString, NULL},
    {XtNxpmdir,  XtCXpmdir,  XtRString,    sizeof(String),    offset(xpmdir),   XtRString,    NULL},
#ifdef LOADCARDS
    {XtNcardset, XtCCardset, XtRString,    sizeof(String),    offset(cardset),  XtRString,    NULL},
#endif
    {XtNruleset, XtCRuleset, XtRString,    sizeof(String),    offset(rules),    XtRString,    NULL},
    {XtNfaceup,	 XtCFaceup,  XtRInt,	   sizeof(int),       offset(faceup),	XtRImmediate, (XtPointer)-1},
    {XtNfacedown,XtCFacedown,XtRInt,	   sizeof(int),       offset(facedown),	XtRImmediate, (XtPointer)-1},
    {XtNdecks,   XtCDecks,   XtRInt,	   sizeof(int),       offset(decks),	XtRImmediate, (XtPointer)-1},
    {XtNslots,   XtCSlots,   XtRInt,	   sizeof(int),       offset(slots),	XtRImmediate, (XtPointer)-1},
    {XtNjokers,  XtCJokers,  XtRInt,	   sizeof(int),       offset(jokers),	XtRImmediate, (XtPointer)-1},
    {XtNtmps,	 XtCTmps,    XtRInt,	   sizeof(int),       offset(tmps),	XtRImmediate, (XtPointer)-1},
    {XtNparam0,	 XtCParam,   XtRInt,	   sizeof(int),       offset(param0),	XtRImmediate, (XtPointer)-1},
    {XtNparam1,	 XtCParam,   XtRInt,	   sizeof(int),       offset(param1),	XtRImmediate, (XtPointer)-1},
    {XtNparam2,	 XtCParam,   XtRInt,	   sizeof(int),       offset(param2),	XtRImmediate, (XtPointer)-1},
    {XtNparam3,	 XtCParam,   XtRInt,	   sizeof(int),       offset(param3),	XtRImmediate, (XtPointer)-1},
    {XtNgapx,	 XtCGap,     XtRDimension, sizeof(Dimension), offset(xgap),	XtRImmediate, (XtPointer)4},
    {XtNgapy,	 XtCGap,     XtRDimension, sizeof(Dimension), offset(ygap),	XtRImmediate, (XtPointer)4},
    {XtNarrw,	 XtCArr,     XtRDimension, sizeof(Dimension), offset(arrowwidth),XtRImmediate, (XtPointer)255},
    {XtNarrh,	 XtCArr,     XtRDimension, sizeof(Dimension), offset(arrowheight),XtRImmediate, (XtPointer)255},
    {XtNmaWidth, XtCMaWidth, XtRDimension, sizeof(Dimension), offset(markwidth), XtRImmediate, (XtPointer)999},
    {XtNcbColor, XtCCbColor, XtRPixel,  sizeof(Pixel),  offset(cbcolor),    XtRString, "DarkGreen"},
    {XtNmaColor, XtCMaColor, XtRPixel,  sizeof(Pixel),  offset(markcolor),  XtRString, XtDefaultBackground},
    {XtNarColor, XtCArColor, XtRPixel,  sizeof(Pixel),  offset(arrowcolor), XtRString, "White"},
    {XtNredColor,XtCRedColor,XtRPixel,  sizeof(Pixel),  offset(redcolor),   XtRString, "Red3"},
    {XtNrng,	 XtCRng,   XtRInt,	   sizeof(int),       offset(rng),	XtRImmediate, (XtPointer)1},
#undef offset
};


/* actions on the desktop area */
/*ARGSUSED*/
static void TableauKey(Widget w, XEvent *xev, String *s, Cardinal *num) {
    key_press((XKeyPressedEvent *)xev);
}
/*ARGSUSED*/
static void TableauBtn_up(Widget w, XEvent *xev, String *s, Cardinal *num) {
    button_release((XButtonPressedEvent *)xev);
}
/*ARGSUSED*/
static void TableauBtn_down(Widget w, XEvent *xev, String *s, Cardinal *num) {
    button_press((XButtonPressedEvent *)xev);
}
/*ARGSUSED*/
static void TableauMove(Widget w, XEvent *xev, String *s, Cardinal *num) {
    mouse_motion((XPointerMovedEvent *)xev);
}
/*ARGSUSED*/
static void Redisplay(Widget w, XEvent *xev, Region region) {
    redraw_table((XExposeEvent *)xev);
}

static XtActionsRec actions[] = {
    /* {name, procedure}, */
    { "tableau_k",	TableauKey },
    { "tableau_d",	TableauBtn_down },
    { "tableau_u",	TableauBtn_up },
    { "tableau_m",	TableauMove },
};

static char translations[] = "\
<Key>:		tableau_k()	\n\
<BtnDown>:	tableau_d()	\n\
<BtnUp>:	tableau_u()	\n\
<Btn3Motion>:	tableau_m()	\n\
";


static Boolean SetValues(Widget current, Widget request, Widget desired,
			 ArgList args, Cardinal *num_args) {
    return FALSE;
}

static void Initialize(Widget request, Widget xnew, ArgList args, Cardinal *n);
static void Resize(Widget gw);


static void Realize(Widget w, XtValueMask *valuemask, XSetWindowAttributes *winattr) {
    *valuemask |= CWBorderPixel | CWEventMask | CWBackingStore | CWBitGravity;
    winattr->backing_store = WhenMapped;
    winattr->border_pixel = blackpixel;
    winattr->bit_gravity = NorthWestGravity;
    winattr->event_mask = KeyPressMask | ExposureMask | ButtonPressMask |
            ButtonReleaseMask | StructureNotifyMask | Button3MotionMask;
   (*(tableauClassRec.core_class.superclass)->core_class.realize)(w, valuemask, winattr);
}

TableauClassRec tableauClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"Tableau",
    /* widget_size		*/	sizeof(TableauRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	NULL,
    /* resize			*/	Resize,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* tableau fields */
    /* empty			*/	0
  }
};

WidgetClass tableauWidgetClass = (WidgetClass)&tableauClassRec;



static Widget toplev = NULL;

static void Resize(Widget gw) {
    TableauWidget w = (TableauWidget) gw;
    /* printf("Parent wants me to have size %d,%d\n",
	   w->core.width, w->core.height); */
    resize_event(w->core.width, w->core.height);
    /* (*pileWidgetClass->core_class.superclass->core_class.resize)(gw); */
}

void AskWidgetForResize(XSize_t x, XSize_t y) {
#ifdef useMotif
    Arg Args[2];
    XtSetArg(Args[0], XtNwidth, x);
    XtSetArg(Args[1], XtNheight, y);
    if (toplev)
	XtSetValues(toplev, Args, 2);
#else
    XtWidgetGeometry Geo;
    XtGeometryResult r;

    Geo.width = x;
    Geo.height = y;
    do {
	Geo.request_mode = CWWidth | CWHeight;
#ifdef LABER
	printf("resize to %d %d yielded ", Geo.width, Geo.height);
#endif
	r = XtMakeGeometryRequest(toplev, &Geo, &Geo);
#ifdef LABER
	switch (r) {
	case XtGeometryYes:	printf("YES!\n");break;
	case XtGeometryNo:	printf("NO!\n"); break;
	case XtGeometryAlmost:printf("Almost!\n"); break;
	case XtGeometryDone:	printf("Done!\n"); break;
	}
#endif
    } while (r == XtGeometryAlmost);
#endif
}

static GC simpleGC(Pixel bg, Pixel fg) {
    XGCValues gcv;
    long gcflags;

    gcv.background = bg;
    gcv.foreground = fg;
    gcv.graphics_exposures = True;
    gcflags = GCForeground | GCBackground | GCGraphicsExposures;
    return XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
}


#define CHECK(x, mi, ma)	{ if ((x) < mi || (x) > ma) (x) = (mi); }
extern char *loadfilename;
extern GC hintgc;

static void Initialize(Widget request, Widget xnew, ArgList args, Cardinal *n) {
    static int is_inited = 0;
    TableauWidget new = (TableauWidget)xnew;
    TableauPart *p = &new->tableau;
    toplev = xnew;
    if (is_inited) {
	fprintf(stderr, "Sorry, currently only one instance of Tableau is allowed\n");
	exit(EXIT_FAILURE);
    }

    {  const char *s; langdir = (s = getenv("LANG")) ? s : ""; }
#ifndef VMS
    read_message_file(p->messagefile ? p->messagefile : "messages");
    read_keyboard_file(p->keyboardfile ? p->keyboardfile : "Keys");
#else
    read_message_file(p->messagefile ? p->messagefile : "messages.");
    read_keyboard_file(p->keyboardfile ? p->keyboardfile : "Keys.");
#endif

    /* assign global data for old Xlib program */
    dpy    = XtDisplay(new);
    screen = DefaultScreen(dpy);
    graphic.is_color = (DisplayCells(dpy, screen) > 2);
    /* validity checks */
    CHECK(p->decks,	-1, 9);
    CHECK(p->slots,	-1, 32);
    CHECK(p->faceup, 	-1, 30);
    CHECK(p->facedown,	-1, 30);
    CHECK(p->jokers,	-1, 16);
    CHECK(p->tmps,	-1, 16);
    CHECK(p->param0,	-1, 99);
    CHECK(p->param1,	-1, 99);
    CHECK(p->param2,	-1, 99);
    CHECK(p->param3,	-1, 99);

    rng = p->rng;
    if (loadfilename)
	load_game(loadfilename);
    else {
	long orgseed;
	orgseed = game.seed;	/* this is destroyed by new_rules */
	if (!p->rules)
	    p->rules = "Gypsy";
	new_rules(p->rules, p->decks, p->slots, p->faceup,
	      p->facedown, p->jokers, p->tmps, p->param0, p->param1, p->param2, p->param3);
	newgame(orgseed);
    }
    init_gfx();		/* make GCs */
    init_cards(
#ifdef LOADCARDS
	       p->cardset,
#else
	       NULL,
#endif
	       -1, -1, p->redcolor, p->cbcolor, 1, p->xpmdir);
    if (p->arrowwidth <= 30 && p->arrowheight <= 30) {
	graphic.aw = p->arrowwidth;
	graphic.ah = p->arrowheight;
    }
    if (graphic.aw > 0 && graphic.ah > 0)
	hintgc = simpleGC(BlackPixel(dpy, screen), p->arrowcolor);

    graphic.xgap = p->xgap;
    graphic.ygap = p->ygap;
    graphic.autolayout = 1;
#ifdef LABER
    printf("size = (%d,%d) and (%d,%d) in Initialize(), computing minwindow:\n",
	   graphic.width, graphic.height, new->core.width, new->core.height);
#endif
    generic_minwindow((XSize_t *)&graphic.width, (XSize_t *)&graphic.height);
    graphic.min_width = graphic.width;
    graphic.min_height = graphic.height;
#ifdef LABER
    printf("result is (%d,%d)\n", graphic.width, graphic.height);
#endif

    init_mark(p->markcolor, p->markwidth);	/* initialize card marks */

    /* make icon here */
    /* now the size is known */
#ifdef LABER
    printf("Init widget: res %d grap %d\n", new->core.width, graphic.width);
#endif
    if (new->core.width < graphic.width)
	new->core.width = graphic.width;
    if (new->core.height < graphic.height)
	new->core.height = graphic.height;
}
#endif
