/*
 * Copyright 1993 Johannes Sixt
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose other than its commercial exploitation
 * is hereby granted without fee, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation. The author
 * makes no representations about the suitability of this software for
 * any purpose. It is provided "as is" without express or implied warranty.
 *
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Johannes Sixt <jsixt@risc.uni-linz.ac.at>
 */

#include <stdio.h>		/* also for sprintf */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Paned.h>
/* #include <X11/Xaw/Command.h> included by <X11/Xaw/Toggle.h> */
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Form.h>
#include "patchlevel.h"

#define BOARD_SIZE	6
#define INITIAL_EUMELS	2
#define MAX_EUMELS	BOARD_SIZE
#define FIELD_WIDTH	80
#define FIELD_HEIGHT	FIELD_WIDTH
#define WIN_WIDTH	(FIELD_WIDTH * BOARD_SIZE)
#define WIN_HEIGHT	(FIELD_HEIGHT * BOARD_SIZE)
#define MESS_TIME_OUT	4000

typedef enum {
	BlackSide, WhiteSide, Empty
	} FieldType;
#define MAX_FIELD_TYPE	((int) Empty)

typedef struct {
	FieldType type;		/* empty, black, white */
	int count;		/* number of Eumels */
	int r, c;		/* row, column of this field */
} Field;

typedef struct {
	FieldType side;		/* whose turn is it? Empty is not allowed */
	FieldType other_side;	/* whose turn is it NOT? For ease of program */
#if BOARD_SIZE < 4
#define MAX_POSS	4
#else
#define MAX_POSS	BOARD_SIZE
#endif
	Field *possibility[MAX_POSS];	/* possible destinations */
	int no_possibilities;	/* number of possible destinations */
	Field *source;		/* field involved in this turn */
	Field board[BOARD_SIZE][BOARD_SIZE];
	int remain[MAX_FIELD_TYPE];	/* remaining eumels */
} MoveData;

typedef struct {
	Pixmap bitmap;		/* bitmap to draw Field in */
	Widget widget;		/* the toggle widget */
} DisplayField;

typedef struct {
	Display *d;
	GC cleargc, gc;		/* Graphics Contexts for clearing and drawing */
	Widget top_level;	/* application shell of the display */
	Widget new_game, quit;
	Widget turnMove, turnDistr, turnCapture, turnDo, turnCancel;
	Widget message;
	XtIntervalId messTimeOut;
	char *rulesHint;
	DisplayField board[BOARD_SIZE][BOARD_SIZE];
} PerDisplay;

typedef struct {
	MoveData *move;
	/* what is player going to do in this turn? */
#if NeedFunctionPrototypes
	Boolean (*executeTurn)(void);
#else
	Boolean (*executeTurn)();
#endif
	PerDisplay pd[MAX_FIELD_TYPE];	/* don't provide for Empty */
	FieldType requests_new, requests_quit;
} DisplayData;

static void CreateUserInterface();
static void InitDrawing();
static void DrawField();
static void InitTurn();
static void NewGame();
static void InitGame();
static void Quit();
static void DoMove();
static void DoDistribute();
static void DoCapture();
static void DoTurn();
static void PrepareDoTurn();
static void CancelTurn();
static Boolean FindSingleSelected();
static void FindPossibilities();
static Boolean Move();
static Boolean Distribute();
static Boolean Capture();
static Boolean IsSet();
static Boolean GameEnds();
#define OtherSide(side) \
		((FieldType)(((int) BlackSide + (int) WhiteSide) - (int) side))
static void BeepMessage();
static void SetRulesHint();
static void RestoreMessage();

static XtAppContext app_context;

static MoveData move_data;		/* data of next move */
static DisplayData dd = { &move_data };	/* data to display field */

static struct _app_res {
	String black_side;
	String white_side;
	int message_time_out;
} app_res;

#define offset(field) XtOffsetOf(struct _app_res, field)
static XtResource resources[] = {
	{"black", "Black", XtRString, sizeof(String),
		offset(black_side), XtRImmediate, NULL},
	{"white", "White", XtRString, sizeof(String),
		offset(white_side), XtRImmediate, NULL},
	{"messageTimeOut", "MessageTimeOut", XtRInt, sizeof(int),
		offset(message_time_out), XtRImmediate,
						(XtPointer) MESS_TIME_OUT},
};

static XrmOptionDescRec options[] = {
	{"-black", ".black",		XrmoptionSepArg,	NULL},
	{"-white", ".white",		XrmoptionSepArg,	NULL},
	{"-mto", ".messageTimeOut",	XrmoptionSepArg,	NULL},
};

static String fallback_res[] = {
	"*quit.fromHoriz:	new",
	"*distribute.fromHoriz:	move",
	"*capture.fromHoriz:	distribute",
	"*do.fromHoriz:		capture",
	"*cancel.fromHoriz:	do",
	NULL
};

static char app_class[] = "Malawi";

void main(argc, argv)
int argc;
char *argv[];
{
    Display *d;
    Widget top_level;
    Window w;
    int i, j;
    static char white_title[50], black_title[50];
    Boolean top_level_used;

    (void) sprintf(white_title, "Malawi %s, White side", VERSION);
    (void) sprintf(black_title, "Malawi %s, Black side", VERSION);

    XtToolkitInitialize();
    app_context = XtCreateApplicationContext();
    XtAppSetFallbackResources(app_context, fallback_res);

    d = XtOpenDisplay(app_context, NULL, argv[0], app_class,
			options, XtNumber(options), &argc, argv);
    if (d == NULL) {
	fprintf(stderr, "Can't open display\n");
	exit(1);
    }
    if (argc > 1) {
	fprintf(stderr,
		"Usage: %s [-black display] [-white display] [-mto time-out]\n",
		argv[0]);
	exit(1);
    }

    top_level = XtVaAppCreateShell(NULL, app_class,
			applicationShellWidgetClass, d, 
			NULL);

    XtGetApplicationResources(top_level, (XtPointer) &app_res,
			resources, XtNumber(resources), NULL, 0);

    top_level_used = False;
    /* find white side's display */
    if (app_res.white_side == NULL) {
	dd.pd[WhiteSide].d = d;
	dd.pd[WhiteSide].top_level = top_level;
	XtVaSetValues(top_level, XtNtitle, (XtArgVal) white_title, NULL);
	top_level_used = True;
    } else {
	dd.pd[WhiteSide].d = XtOpenDisplay(app_context, app_res.white_side,
				argv[0], app_class, NULL, 0, &argc, argv);
	if (dd.pd[WhiteSide].d == NULL) {
	    fprintf(stderr, "Can't open display %s\n", app_res.white_side);
	    exit(1);
	}
	dd.pd[WhiteSide].top_level = XtVaAppCreateShell(NULL, app_class,
			applicationShellWidgetClass, dd.pd[WhiteSide].d,
			XtNtitle,	(XtArgVal) white_title,
			NULL);
    }
    CreateUserInterface(WhiteSide);

    /* find black side's display */
    if (!top_level_used && app_res.black_side == NULL) {
	dd.pd[BlackSide].d = d;
	dd.pd[BlackSide].top_level = top_level;
	XtVaSetValues(top_level, XtNtitle, (XtArgVal) black_title, NULL);
	top_level_used = True;
    } else {
	dd.pd[BlackSide].d = XtOpenDisplay(app_context, app_res.black_side,
				argv[0], app_class, NULL, 0, &argc, argv);
	if (dd.pd[BlackSide].d == NULL) {
	    fprintf(stderr, "Can't open display %s\n", app_res.black_side);
	    exit(1);
	}
	dd.pd[BlackSide].top_level = XtVaAppCreateShell(NULL, app_class,
			applicationShellWidgetClass, dd.pd[BlackSide].d,
			XtNtitle,	(XtArgVal) black_title,
			NULL);
    }
    CreateUserInterface(BlackSide);

    XtRealizeWidget(dd.pd[WhiteSide].top_level);
    XtRealizeWidget(dd.pd[BlackSide].top_level);

    /* draw board */
    w = XtWindow(dd.pd[WhiteSide].top_level);
    for (i = 0; i < BOARD_SIZE; i++) {
	for (j = 0; j < BOARD_SIZE; j++) {
	    dd.pd[WhiteSide].board[i][j].bitmap =
		XCreatePixmap(dd.pd[WhiteSide].d, w,
					FIELD_WIDTH, FIELD_HEIGHT, 1);
	}
    }

    w = XtWindow(dd.pd[BlackSide].top_level);
    for (i = 0; i < BOARD_SIZE; i++) {
	for (j = 0; j < BOARD_SIZE; j++) {
	    dd.pd[BlackSide].board[i][j].bitmap =
		XCreatePixmap(dd.pd[BlackSide].d, w,
					FIELD_WIDTH, FIELD_HEIGHT, 1);
	}
    }

    InitDrawing(WhiteSide);
    InitDrawing(BlackSide);

    InitGame(NULL, NULL, NULL);

    XtAppMainLoop(app_context);
    /*NOTREACHED*/
}


#define HIGHTHICK	2

static void CreateUserInterface(side)
FieldType side;
{
    Widget panel, game, form, turn;
    int i, j;

    panel = XtVaCreateManagedWidget("panel", panedWidgetClass,
				dd.pd[side].top_level, NULL);

    game = XtVaCreateManagedWidget("game", formWidgetClass, panel,
			NULL);

    dd.pd[side].new_game = XtVaCreateManagedWidget("new",
			toggleWidgetClass, game,
			XtNstate,	(XtArgVal) False,
			NULL);
    XtAddCallback(dd.pd[side].new_game, XtNcallback, NewGame, (XtPointer) side);

    dd.pd[side].quit = XtVaCreateManagedWidget("quit", toggleWidgetClass, game,
			XtNstate,	(XtArgVal) False,
			NULL);
    XtAddCallback(dd.pd[side].quit, XtNcallback, Quit, (XtPointer) side);

    dd.pd[side].message = XtVaCreateManagedWidget("message",
			labelWidgetClass, panel,
			XtNlabel,	(XtArgVal) "",
			NULL);

    dd.pd[side].messTimeOut = NULL;

    turn = XtVaCreateManagedWidget("turn", formWidgetClass, panel,
			NULL);
    dd.pd[side].turnMove = XtVaCreateManagedWidget("move",
			commandWidgetClass, turn, NULL);
    dd.pd[side].turnDistr = XtVaCreateManagedWidget("distribute",
			commandWidgetClass, turn, NULL);
    dd.pd[side].turnCapture = XtVaCreateManagedWidget("capture",
			commandWidgetClass, turn, NULL);
    dd.pd[side].turnDo = XtVaCreateManagedWidget("do",
			commandWidgetClass, turn, NULL);
    dd.pd[side].turnCancel = XtVaCreateManagedWidget("cancel",
			commandWidgetClass, turn, NULL);

    XtAddCallback(dd.pd[side].turnMove, XtNcallback, DoMove, NULL);
    XtAddCallback(dd.pd[side].turnDistr, XtNcallback, DoDistribute, NULL);
    XtAddCallback(dd.pd[side].turnCapture, XtNcallback, DoCapture, NULL);
    XtAddCallback(dd.pd[side].turnDo, XtNcallback, DoTurn, NULL);
    XtAddCallback(dd.pd[side].turnCancel, XtNcallback, CancelTurn, NULL);

    form = XtVaCreateManagedWidget("form", formWidgetClass, panel,
			NULL);

    /* create buttons for fields */
    for (i = 0; i < BOARD_SIZE; i++) {
	for (j = 0; j < BOARD_SIZE; j++) {
	    dd.pd[side].board[i][j].widget =
		XtVaCreateManagedWidget("field", toggleWidgetClass, form,
			XtNlabel,	(XtArgVal) "",
			XtNwidth,	(XtArgVal) (FIELD_WIDTH+(2*HIGHTHICK)),
			XtNheight,	(XtArgVal) (FIELD_HEIGHT+(2*HIGHTHICK)),
			XtNinternalWidth,	(XtArgVal) 0,
			XtNinternalHeight,	(XtArgVal) 0,
			XtNhighlightThickness,	(XtArgVal) HIGHTHICK,
			XtNfromVert,	(XtArgVal) (i == 0 ?
				NULL : dd.pd[side].board[i-1][0].widget),
			XtNfromHoriz,	(XtArgVal) (j == 0 ?
				NULL : dd.pd[side].board[i][j-1].widget),
			NULL);
	}
    }
}


#define SIDE_MARKER_THICK	8

static XRectangle rect[MAX_EUMELS + 1];

static void InitDrawing(side)
FieldType side;
{
    XGCValues values;
    unsigned long mask;
    int i;

    values.function = GXcopy;
    values.foreground = 1;
    values.background = 0;
    values.line_width = 0;
    values.line_style = LineSolid;
    values.fill_style = FillSolid;
    mask = GCFunction | GCForeground | GCBackground |
		GCLineWidth | GCLineStyle | GCFillStyle;
    dd.pd[side].gc = XCreateGC(dd.pd[side].d,
			dd.pd[side].board[0][0].bitmap, mask, &values);

    values.foreground = 0;
    mask = GCFunction | GCForeground | GCBackground | GCFillStyle;
    dd.pd[side].cleargc = XCreateGC(dd.pd[side].d,
			dd.pd[side].board[0][0].bitmap, mask, &values);

    /* initialize rectangles array */
    rect[0].x = SIDE_MARKER_THICK;
    rect[0].y = FIELD_HEIGHT - ((3 * SIDE_MARKER_THICK) / 2);
    rect[0].width = FIELD_WIDTH - 2 * SIDE_MARKER_THICK;
    rect[0].height = SIDE_MARKER_THICK;
    /* try hard to make the rects nicly aligned */
    for (i = 1; i <= MAX_EUMELS; i++) {
	rect[i].x = i * (FIELD_WIDTH / (MAX_EUMELS + 3));
	rect[i].y = FIELD_HEIGHT - ((3 * SIDE_MARKER_THICK) / 2) -
			i * ((FIELD_WIDTH - ((3 * SIDE_MARKER_THICK) / 2)) /
				(MAX_EUMELS + 3));
	rect[i].width = 2 * (FIELD_WIDTH / (MAX_EUMELS + 3));
	rect[i].height = ((FIELD_WIDTH - ((3 * SIDE_MARKER_THICK) / 2)) /
				(MAX_EUMELS + 3)) - 1;
    }
}


static void DrawField(r, c)
int r, c;
{
#if NeedFunctionPrototypes
    int (*fun)(Display *display, Drawable d, GC gc, XRectangle *rect, int c);
#else
    int (*fun)();
#endif

    /* clear drawing area */
    XFillRectangle(dd.pd[WhiteSide].d, dd.pd[WhiteSide].board[r][c].bitmap,
			dd.pd[WhiteSide].cleargc,
			0, 0, FIELD_WIDTH, FIELD_HEIGHT);
    XFillRectangle(dd.pd[BlackSide].d, dd.pd[BlackSide].board[r][c].bitmap,
			dd.pd[BlackSide].cleargc,
			0, 0, FIELD_WIDTH, FIELD_HEIGHT);
    if (dd.move->board[r][c].type == Empty) {
	/* we're done */
	XtVaSetValues(dd.pd[WhiteSide].board[r][c].widget,
		XtNbitmap, (XtArgVal) dd.pd[WhiteSide].board[r][c].bitmap,
		NULL);
	XtVaSetValues(dd.pd[BlackSide].board[r][c].widget,
		XtNbitmap, (XtArgVal) dd.pd[BlackSide].board[r][c].bitmap,
		NULL);
	return;
    }

    switch (dd.move->board[r][c].type) {
    case BlackSide:
	fun = XFillRectangles;
	break;

    case WhiteSide:
	fun = XDrawRectangles;
	break;

    default:
	BeepMessage(BlackSide, "Default case must not occur. Call maintainer.");
	BeepMessage(WhiteSide, "Default case must not occur. Call maintainer.");
	return;
    }

    (*fun)(dd.pd[WhiteSide].d, dd.pd[WhiteSide].board[r][c].bitmap,
			dd.pd[WhiteSide].gc,
			rect, dd.move->board[r][c].count + 1);
    XtVaSetValues(dd.pd[WhiteSide].board[r][c].widget,
		XtNbitmap, (XtArgVal) dd.pd[WhiteSide].board[r][c].bitmap,
		NULL);
    (*fun)(dd.pd[BlackSide].d, dd.pd[BlackSide].board[r][c].bitmap,
			dd.pd[BlackSide].gc,
			rect, dd.move->board[r][c].count + 1);
    XtVaSetValues(dd.pd[BlackSide].board[r][c].widget,
		XtNbitmap, (XtArgVal) dd.pd[BlackSide].board[r][c].bitmap,
		NULL);
}


/*ARGSUSED*/
static void NewGame(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
    FieldType side = (FieldType) client_data;
    FieldType other_side = OtherSide(side);
    static Arg arg = { XtNstate };

    arg.value = (XtArgVal) IsSet(w);
    /* set button on opponent's display */
    XtSetValues(dd.pd[other_side].new_game, &arg, 1);

    if (arg.value) {	/* i.e. if (IsSet(w)) */
	BeepMessage(other_side, "Opponent requests new game");
	dd.requests_new = side;
    } else {
	if (dd.requests_new != side) {
	    /* request confirmed */
	    InitGame();

	    /* withdraw quit requests; arg.value is False */
	    XtSetValues(dd.pd[WhiteSide].quit, &arg, 1);
	    XtSetValues(dd.pd[BlackSide].quit, &arg, 1);
	}
    }
}


static void InitGame()
{
    int i, j;

    dd.move->remain[BlackSide] = dd.move->remain[WhiteSide] =
				BOARD_SIZE * INITIAL_EUMELS;
    dd.move->side = WhiteSide;
    dd.move->other_side = BlackSide;

    for (j = 0; j < BOARD_SIZE; j++) {
	dd.move->board[0][j].type = BlackSide;
	dd.move->board[0][j].count = INITIAL_EUMELS;
	dd.move->board[0][j].r = 0;
	dd.move->board[0][j].c = j;
	dd.move->board[BOARD_SIZE - 1][j].type = WhiteSide;
	dd.move->board[BOARD_SIZE - 1][j].count = INITIAL_EUMELS;
	dd.move->board[BOARD_SIZE - 1][j].r = BOARD_SIZE - 1;
	dd.move->board[BOARD_SIZE - 1][j].c = j;
    }
    for (i = 1; i < BOARD_SIZE - 1; i++) {
	for (j = 0; j < BOARD_SIZE; j++) {
	    dd.move->board[i][j].type = Empty;
	    dd.move->board[i][j].count = 0;
	    dd.move->board[i][j].r = i;
	    dd.move->board[i][j].c = j;
	}
    }

    CancelTurn(NULL, NULL, NULL);

    for (i = 0; i < BOARD_SIZE; i++) {
	for (j = 0; j < BOARD_SIZE; j++) {
	    DrawField(i, j);
	}
    }
}


/*ARGSUSED*/
static void Quit(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
    FieldType side = (FieldType) client_data;
    FieldType other_side = OtherSide(side);

    if (IsSet(w)) {
	XtVaSetValues(dd.pd[other_side].quit,
			XtNstate, (XtArgVal) True, NULL);
	BeepMessage(other_side, "Opponent requests quitting");
	dd.requests_quit = side;
    } else {
	XtVaSetValues(dd.pd[other_side].quit,
			XtNstate, (XtArgVal) False, NULL);
	if (dd.requests_quit != side) {
	    /* request confirmed */
	    exit(0);
	}
    }
}


/*ARGSUSED*/
static void DoMove(w, clientData, notused)
Widget w;
XtPointer clientData, notused;
{
    int i, j, k;
    int r, c;		/* position of source field */
    int move_possibilities;
    Field *source, *field;
    Widget wid;

    /* prepare board for move */
    dd.executeTurn = Move;
    if (FindSingleSelected(&r, &c)) {
	source = &dd.move->board[r][c];
	if (source->count == 0) {
	    BeepMessage(dd.move->side, "This field cannot move, no eumels!");
	    return;
	}
	dd.move->source = source;

	FindPossibilities(dd.move, r, c);

	move_possibilities = dd.move->no_possibilities;
	for (k = dd.move->no_possibilities; k > 0;) {
	    if (dd.move->possibility[--k]->type != Empty) {
		move_possibilities--;
	    }
	}
	if (move_possibilities == 0) {
	    BeepMessage(dd.move->side, "These eumels cannot move: blocked!");
	    return;
	}

	k = 0;
	for (i = 0; i < BOARD_SIZE; i++) {
	    for (j = 0; j < BOARD_SIZE; j++) {
		field = &dd.move->board[i][j];
		wid = dd.pd[dd.move->side].board[i][j].widget;
		XtVaSetValues(wid, XtNstate, (XtArgVal) False, NULL);
		if (dd.move->possibility[k] == field) {
		    /* the field may or may not be empty;
			FindPossibilities doesn't check for that */
		    XtSetSensitive(wid, field->type == Empty);
		    k++;
		} else {
		    XtSetSensitive(wid, False);
		}
	    }
	}

	SetRulesHint(dd.move->side, "Select destination field");
	PrepareDoTurn();
    }
}


/*ARGSUSED*/
static void DoDistribute(w, clientData, notused)
Widget w;
XtPointer clientData, notused;
{
    int i, j;
    int r, c;		/* position of source field */
    Field *source, *field;
    Widget wid;

    /* prepare board for distribution */
    dd.executeTurn = Distribute;
    if (FindSingleSelected(&r, &c)) {
	source = &dd.move->board[r][c];
	if (source->count == 0) {
	    BeepMessage(dd.move->side, "This field has no eumels!");
	    return;
	}
	dd.move->source = source;

	dd.move->no_possibilities = 0;
	for (i = 0; i < BOARD_SIZE; i++) {
	    for (j = 0; j < BOARD_SIZE; j++) {
		field = &dd.move->board[i][j];
		wid = dd.pd[dd.move->side].board[i][j].widget;
		XtVaSetValues(wid, XtNstate, (XtArgVal) False, NULL);
		if (field != source &&
			field->type == dd.move->side &&
			field->count < BOARD_SIZE) {
		    XtSetSensitive(wid, True);
		    dd.move->possibility[dd.move->no_possibilities++] = field;
		} else {
		    XtSetSensitive(wid, False);
		}
	    }
	}	

	SetRulesHint(dd.move->side,
			"Select sticks onto which to distribute eumels");
	PrepareDoTurn();
    }
}


/*ARGSUSED*/
static void DoCapture(w, clientData, notused)
Widget w;
XtPointer clientData, notused;
{
    int i, j, k;
    int r, c;		/* position of source field */
    int capture_possibilities;
    Field *source, *field;
    Widget wid;

    /* prepare board for capture */
    dd.executeTurn = Capture;
    if (FindSingleSelected(&r, &c)) {
	source = &dd.move->board[r][c];
	if (source->count == 0) {
	    BeepMessage(dd.move->side, "This stick has no eumels!");
	    return;
	}
	dd.move->source = source;

	FindPossibilities(dd.move, r, c);

	capture_possibilities = dd.move->no_possibilities;
	for (k = dd.move->no_possibilities - 1; k >= 0; k--) {
	    if (dd.move->possibility[k]->count == 0 ||
		dd.move->possibility[k]->type != dd.move->other_side) {
		capture_possibilities--;
	    }
	}
	if (capture_possibilities == 0) {
	    BeepMessage(dd.move->side, "This stick cannot capture eumels!");
	    return;
	}

	k = 0;
	for (i = 0; i < BOARD_SIZE; i++) {
	    for (j = 0; j < BOARD_SIZE; j++) {
		field = &dd.move->board[i][j];
		wid = dd.pd[dd.move->side].board[i][j].widget;
		XtVaSetValues(wid, XtNstate, (XtArgVal) False, NULL);
		if (dd.move->possibility[k] == field) {
		    /* the field may or may not be removable;
			FindPossibilities doesn't check for that */
		    XtSetSensitive(wid, field->count != 0 &&
					field->type == dd.move->other_side);
		    k++;
		} else {
		    XtSetSensitive(wid, False);
		}
	    }
	}

	SetRulesHint(dd.move->side,
			"Select the field from which to capture eumels");
	PrepareDoTurn();
    }
}


static void PrepareDoTurn()
{
    PerDisplay *me = &dd.pd[dd.move->side];

    XtSetSensitive(me->turnMove, False);
    XtSetSensitive(me->turnDistr, False);
    XtSetSensitive(me->turnCapture, False);
    XtSetSensitive(me->turnDo, True);
    XtSetSensitive(me->turnCancel, True);
}


/*ARGSUSED*/
static void DoTurn(w, client_data, unused)
Widget w;
XtPointer client_data, unused;
{
    PerDisplay *me, *other;
    FieldType won_by;
    static char buf[100];

    if ((*dd.executeTurn)()) {
	dd.move->other_side = dd.move->side;
	dd.move->side = OtherSide(dd.move->side);

	if (GameEnds(dd.move, &won_by)) {
	    /* game ends */
	    BeepMessage(WhiteSide, "GAME OVER");
	    BeepMessage(BlackSide, "GAME OVER");
	    (void) sprintf(buf, "Game won by %s side",
			won_by == BlackSide ? "Black" : "White");
	    dd.pd[WhiteSide].rulesHint = buf;
	    dd.pd[BlackSide].rulesHint = buf;

	    me = &dd.pd[dd.move->side];
	    other = &dd.pd[dd.move->other_side];
	    XtSetSensitive(me->turnDo, False);
	    XtSetSensitive(me->turnCancel, False);
	    XtSetSensitive(other->turnDo, False);
	    XtSetSensitive(other->turnCancel, False);
	} else {
	    CancelTurn(w, NULL, NULL);
	}
    }
}


/*ARGSUSED*/
static void CancelTurn(w, client_data, unused)
Widget w;
XtPointer client_data, unused;
{
    PerDisplay *me, *other;

    me = &dd.pd[dd.move->side];
    other = &dd.pd[dd.move->other_side];

    XtSetSensitive(me->turnMove, True);
    XtSetSensitive(me->turnDistr, True);
    XtSetSensitive(me->turnCapture, True);
    XtSetSensitive(me->turnDo, False);
    XtSetSensitive(me->turnCancel, False);
    XtSetSensitive(other->turnMove, False);
    XtSetSensitive(other->turnDistr, False);
    XtSetSensitive(other->turnCapture, False);
    XtSetSensitive(other->turnDo, False);
    XtSetSensitive(other->turnCancel, False);
    InitTurn();
}


/* Initialize for new turn: Only the side's fields are sensitive. */
static void InitTurn()
{
    int i, j;
    PerDisplay *me, *other;

    me = &dd.pd[dd.move->side];
    other = &dd.pd[dd.move->other_side];
    for (i = 0; i < BOARD_SIZE; i++) {
	for (j = 0; j < BOARD_SIZE; j++) {
	    XtSetSensitive(me->board[i][j].widget,
			dd.move->board[i][j].type == dd.move->side);
	    XtVaSetValues(me->board[i][j].widget,
			XtNstate, (XtArgVal) False, NULL);
	    XtSetSensitive(other->board[i][j].widget, False);
	    XtVaSetValues(other->board[i][j].widget,
			XtNstate, (XtArgVal) False, NULL);
	}
    }

    SetRulesHint(dd.move->side, "Select one stick");
    SetRulesHint(dd.move->other_side, "Opponent's turn");
}


static Boolean FindSingleSelected(r, c)
int *r, *c;
{
    int i, j;
    Boolean found = False;
    PerDisplay *me = &dd.pd[dd.move->side];

    for (i = 0; i < BOARD_SIZE; i++) {
	for (j = 0; j < BOARD_SIZE; j++) {
	    if (IsSet(me->board[i][j].widget)) {
		if (found) {
		    /* more than one field selected */
		    BeepMessage(dd.move->side,
					"More than one field is selected!");
		    return False;
		}
		found = True;
		*r = i;
		*c = j;
	    }
	}
    }
    if (!found) {
	/* no field is selected */
	BeepMessage(dd.move->side, "No field is selected!");
	return False;
    }
    return True;
}


/**
Find the fields which can be reached from a given field.
*/
static void FindPossibilities(move, r, c)
MoveData *move;
int r, c;		/* position of source field */
{
    int i;
    int dpr, dpc;	/* positive row, column */
    int dnr, dnc;	/* negative row, coulmn */
    Boolean north_ok, east_ok, south_ok, west_ok;

	north_ok = east_ok = south_ok = west_ok = True;
	dpr = dnr = r, dpc = dnc = c;

	i = move->board[r][c].count;
	if (i == 0) {
	    move->no_possibilities = 0;
	    return;
	}

	/* check for occupied fields BETWEEN source and possible destinations */
	for (i--; i; i--) {
	    ++dpr, ++dpc, --dnr, --dnc;
	    if (north_ok &&
			(dnr < 0 || move->board[dnr][c].type != Empty))
		north_ok = False;
	    if (west_ok &&
			(dnc < 0 || move->board[r][dnc].type != Empty))
		west_ok = False;
	    if (east_ok &&
			(dpc>=BOARD_SIZE || move->board[r][dpc].type != Empty))
		east_ok = False;
	    if (south_ok &&
			(dpr>=BOARD_SIZE || move->board[dpr][c].type != Empty))
		south_ok = False;
	}
	++dpr, ++dpc, --dnr, --dnc;

	/* check that possible destinations don't fall off board */
	if (dnr < 0)
		north_ok = False;
	if (dnc < 0)
		west_ok = False;
	if (dpc >= BOARD_SIZE)
		east_ok = False;
	if (dpr >= BOARD_SIZE)
		south_ok = False;


	/* ensure that the possiblities are stored in "increasing" order if
		board is scanned with column index changing fastest */
	i = 0;
	if (north_ok)
	    move->possibility[i++] = &move->board[dnr][c];
	if (west_ok)
	    move->possibility[i++] = &move->board[r][dnc];
	if (east_ok)
	    move->possibility[i++] = &move->board[r][dpc];
	if (south_ok)
	    move->possibility[i++] = &move->board[dpr][c];
	move->no_possibilities = i;

	/* erase remaining array */
	while (i < MAX_POSS)
	    move->possibility[i++] = NULL;
}


static Boolean Move()
{
    int i;
    Field *field = NULL, *f;
    PerDisplay *me = &dd.pd[dd.move->side];

    /* this should not occur */ if (dd.move->no_possibilities == 0) return True;

    for (i = dd.move->no_possibilities; i > 0;) {
	f = dd.move->possibility[--i];
	if (IsSet(me->board[f->r][f->c].widget)) {
	    if (field != NULL) {
		BeepMessage(dd.move->side, "More than one field is selected!");
		return False;
	    }
	    field = f;
	}
    }
    if (field == NULL) {
	BeepMessage(dd.move->side, "No field is selected!");
	return False;
    }
    field->type = dd.move->source->type;
    field->count = dd.move->source->count;
    dd.move->source->type = Empty;
    dd.move->source->count = 0;
    DrawField(dd.move->source->r, dd.move->source->c);
    DrawField(field->r, field->c);
    return True;
}


static Boolean Distribute()
{
    int i;
    int must_distribute;	/* number of fields that must be selected */
    int selected = 0;
    Field *field;
    PerDisplay *me = &dd.pd[dd.move->side];

    must_distribute = dd.move->source->count < dd.move->no_possibilities ?
			dd.move->source->count : dd.move->no_possibilities;

    for (i = dd.move->no_possibilities; i > 0;) {
	field = dd.move->possibility[--i];
	if (IsSet(me->board[field->r][field->c].widget)) {
	    selected++;
	}
    }

    if (selected != must_distribute) {
	char buf[100];
	(void) sprintf(buf, "Exactly %d fields must be selected!",
			must_distribute);
	BeepMessage(dd.move->side, buf);
	return False;
    }

    for (i = dd.move->no_possibilities; i > 0;) {
	field = dd.move->possibility[--i];
	if (IsSet(me->board[field->r][field->c].widget)) {
	    field->count++;
	    dd.move->source->count--;
	    DrawField(field->r, field->c);
	}
    }

    /* remaining eumels are lost */
    dd.move->remain[dd.move->side] -= dd.move->source->count;
    dd.move->source->count = 0;
    DrawField(dd.move->source->r, dd.move->source->c);

    /* This should not lead to end of game. Correct me if I'm wrong. */

    return True;
}


static Boolean Capture()
{
    int i;
    Field *field = NULL, *f;
    PerDisplay *me = &dd.pd[dd.move->side];

    /* this should not occur */ if (dd.move->no_possibilities == 0) return True;

    for (i = dd.move->no_possibilities; i > 0;) {
	f = dd.move->possibility[--i];
	if (IsSet(me->board[f->r][f->c].widget)) {
	    if (field != NULL) {
		BeepMessage(dd.move->side, "More than one field is selected!");
		return False;
	    }
	    field = f;
	}
    }
    if (field == NULL) {
	BeepMessage(dd.move->side, "No field is selected!");
	return False;
    }
    dd.move->remain[field->type] -= field->count;
    field->count = 0;
    DrawField(field->r, field->c);

    return True;
}


static Boolean IsSet(w)
Widget w;
{
    Boolean state;

    XtVaGetValues(w, XtNstate, (XtArgVal) &state, NULL);
    return state;
}


static Boolean GameEnds(move, winning_side)
MoveData *move;
FieldType *winning_side;
{
    int i, j;

    if (move->remain[move->side] == 0) {
	*winning_side = move->other_side;
	return True;
    }

    i = move->side == BlackSide ? BOARD_SIZE - 1 : 0;
    for (j = 0; j < BOARD_SIZE; j++) {
	if (move->board[i][j].type == move->side &&
		move->board[i][j].count != 0) {
	    *winning_side = move->side;
	    return True;
	}
    }
    return False;
}


static void BeepMessage(side, text)
FieldType side;
char *text;
{
    PerDisplay *me = &dd.pd[side];

    XtVaSetValues(me->message, XtNlabel, (XtArgVal) text, NULL);
    XBell(me->d, 0);
    if (me->messTimeOut != NULL) {
	XtRemoveTimeOut(me->messTimeOut);
    }
    me->messTimeOut =
	XtAppAddTimeOut(app_context, app_res.message_time_out,
			RestoreMessage, (XtPointer) side);
}


static void SetRulesHint(side, text)
FieldType side;
char *text;
{
    PerDisplay *me = &dd.pd[side];

    me->rulesHint = text;
    XtVaSetValues(me->message, XtNlabel, (XtArgVal) text, NULL);
    if (me->messTimeOut != NULL) {
	XtRemoveTimeOut(me->messTimeOut);
    }
}


/*ARGSUSED*/
static void RestoreMessage(client_data, id)
XtPointer client_data;
XtIntervalId *id;
{
    FieldType side = (FieldType) client_data;
    PerDisplay *me = &dd.pd[side];

    XtVaSetValues(me->message, XtNlabel, (XtArgVal) me->rulesHint, NULL);
    me->messTimeOut = NULL;
}
