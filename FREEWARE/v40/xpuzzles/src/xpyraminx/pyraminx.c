/*
# X-BASED PYRAMINX(tm)
#
#  Pyraminx.c
#
###
#
#  Copyright (c) 1994 - 95	David Albert Bagley, bagleyd@source.asset.com
#
#                   All Rights Reserved
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

/* Methods file for Pyraminx */

#include <stdio.h>
#include <stdlib.h>
#ifdef VMS
#include <unixlib.h>
#else
#ifndef apollo
#include <unistd.h>
#endif
#endif
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include "PyraminxP.h"

#ifndef DATAFILE
#define DATAFILE "/usr/games/lib/pyraminx.data"
#endif

static void InitializePyraminx();
static void ExposePyraminx();
static void ResizePyraminx();
static void DestroyPyraminx();
static Boolean SetValuesPyraminx();
static void quit_pyraminx();
static void practice_pyraminx();
static void maybe_practice_pyraminx();
static void randomize_pyraminx();
static void maybe_randomize_pyraminx();
static void enter_pyraminx();
static void write_pyraminx();
static void undo_pyraminx();
static void solve_pyraminx();
static void increment_pyraminx();
static void decrement_pyraminx();
static void orientize_pyraminx();
static void period2_mode_pyraminx();
static void period3_mode_pyraminx();
static void both_mode_pyraminx();
static void sticky_mode_pyraminx();
static void move_pyraminx_top();
static void move_pyraminx_tr();
static void move_pyraminx_left();
static void move_pyraminx_cw();
static void move_pyraminx_right();
static void move_pyraminx_bl();
static void move_pyraminx_bottom();
static void move_pyraminx_ccw();
static void move_pyraminx_input();
static void get_color();
static void move_control_cb();
static void check_polyhedrons();
static void reset_polyhedrons();
static void resize_polyhedrons();
static int position_polyhedrons();
static void no_move_polyhedrons();
static void practice_polyhedrons();
static void randomize_polyhedrons();
static void move_polyhedrons();
static void rotate_face();
/* rcd : row, column, or diagonal */
static void read_rcd2();
static void read_rcd3();
static void rotate_rcd2();
static void rotate_rcd3();
static void reverse_rcd2();
static void reverse_rcd3();
static void write_rcd2();
static void write_rcd3();
static void draw_frame();
static void draw_triangle();
static void draw_orient_line();
static int rcd();
static int length();

#ifdef DEBUG
static void print_tetra();
static void print_face();
static void print_row2();
static void print_row3();
#endif

static char defaultTranslationsPyraminx[] =
  "<KeyPress>q: quit()\n\
   Ctrl<KeyPress>C: quit()\n\
   <KeyPress>Up: move_top()\n\
   <KeyPress>KP_8: move_top()\n\
   <KeyPress>R8: move_top()\n\
   <KeyPress>Prior: move_tr()\n\
   <KeyPress>KP_9: move_tr()\n\
   <KeyPress>R9: move_tr()\n\
   <KeyPress>Left: move_left()\n\
   <KeyPress>KP_4: move_left()\n\
   <KeyPress>R10: move_left()\n\
   <KeyPress>Begin: move_cw()\n\
   <KeyPress>KP_5: move_cw()\n\
   <KeyPress>R11: move_cw()\n\
   <KeyPress>Right: move_right()\n\
   <KeyPress>KP_6: move_right()\n\
   <KeyPress>R12: move_right()\n\
   <KeyPress>End: move_bl()\n\
   <KeyPress>KP_1: move_bl()\n\
   <KeyPress>R13: move_bl()\n\
   <KeyPress>Down: move_bottom()\n\
   <KeyPress>KP_2: move_bottom()\n\
   <KeyPress>R14: move_bottom()\n\
   <Btn1Down>: move_ccw()\n\
   <Btn1Motion>: move_ccw()\n\
   <KeyPress>p: practice()\n\
   <Btn2Down>(2+): practice()\n\
   <Btn2Down>: maybe_practice()\n\
   <KeyPress>r: randomize()\n\
   <Btn3Down>(2+): randomize()\n\
   <Btn3Down>: maybe_randomize()\n\
   <KeyPress>e: enter()\n\
   <KeyPress>w: write()\n\
   <KeyPress>u: undo()\n\
   <KeyPress>s: solve()\n\
   <KeyPress>i: increment()\n\
   <KeyPress>d: decrement()\n\
   <KeyPress>o: orientize()\n\
   <KeyPress>2: period2()\n\
   <KeyPress>3: period3()\n\
   <KeyPress>b: both()\n\
   <KeyPress>y: sticky()";
 
static XtActionsRec actionsListPyraminx[] =
{
  {"quit", (XtActionProc) quit_pyraminx},
  {"move_top", (XtActionProc) move_pyraminx_top},
  {"move_tr", (XtActionProc) move_pyraminx_tr},
  {"move_left", (XtActionProc) move_pyraminx_left},
  {"move_cw", (XtActionProc) move_pyraminx_cw},
  {"move_right", (XtActionProc) move_pyraminx_right},
  {"move_bl", (XtActionProc) move_pyraminx_bl},
  {"move_bottom", (XtActionProc) move_pyraminx_bottom},
  {"move_ccw", (XtActionProc) move_pyraminx_ccw},
  {"practice", (XtActionProc) practice_pyraminx},
  {"maybe_practice", (XtActionProc) maybe_practice_pyraminx},
  {"randomize", (XtActionProc) randomize_pyraminx},
  {"maybe_randomize", (XtActionProc) maybe_randomize_pyraminx},
  {"enter", (XtActionProc) enter_pyraminx},
  {"write", (XtActionProc) write_pyraminx},
  {"undo", (XtActionProc) undo_pyraminx},
  {"solve", (XtActionProc) solve_pyraminx},
  {"increment", (XtActionProc) increment_pyraminx},
  {"decrement", (XtActionProc) decrement_pyraminx},
  {"orientize", (XtActionProc) orientize_pyraminx},
  {"period2", (XtActionProc) period2_mode_pyraminx},
  {"period3", (XtActionProc) period3_mode_pyraminx},
  {"both", (XtActionProc) both_mode_pyraminx},
  {"sticky", (XtActionProc) sticky_mode_pyraminx}
};

static XtResource resourcesPyraminx[] =
{
  /* Beware color values are swapped */
  {XtNfaceColor0, XtCLabel, XtRString, sizeof(String),
   XtOffset(PyraminxWidget, pyraminx.face_name[2]), XtRString, "Blue"},
  {XtNfaceColor1, XtCLabel, XtRString, sizeof(String),
   XtOffset(PyraminxWidget, pyraminx.face_name[3]), XtRString, "Red"},
  {XtNfaceColor2, XtCLabel, XtRString, sizeof(String),
   XtOffset(PyraminxWidget, pyraminx.face_name[0]), XtRString, "Yellow"},
  {XtNfaceColor3, XtCLabel, XtRString, sizeof(String),
   XtOffset(PyraminxWidget, pyraminx.face_name[1]), XtRString, "Green"},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
   XtOffset(PyraminxWidget, pyraminx.foreground), XtRString,
   XtDefaultForeground},
  {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
   XtOffset(PyraminxWidget, core.width), XtRString, "200"},
  {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
   XtOffset(PyraminxWidget, core.height), XtRString, "400"},
  {XtNsize, XtCSize, XtRInt, sizeof(int),
   XtOffset(PyraminxWidget, pyraminx.size), XtRString, "3"}, /*DEFAULTTETRAS*/
  {XtNmode, XtCMode, XtRInt, sizeof(int),
   XtOffset(PyraminxWidget, pyraminx.mode), XtRString, "3"}, /*DEFAULTMODE*/
  {XtNorient, XtCOrient, XtRBoolean, sizeof(Boolean),
   XtOffset(PyraminxWidget, pyraminx.orient), XtRString,
   "FALSE"}, /*DEFAULTORIENT*/
  {XtNsticky, XtCSticky, XtRBoolean, sizeof(Boolean),
   XtOffset(PyraminxWidget, pyraminx.sticky), XtRString, "FALSE"},
  {XtNmono, XtCMono, XtRBoolean, sizeof(Boolean),
   XtOffset(PyraminxWidget, pyraminx.mono), XtRString, "FALSE"},
  {XtNpractice, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(PyraminxWidget, pyraminx.practice), XtRString, "FALSE"},
  {XtNstart, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(PyraminxWidget, pyraminx.started), XtRString, "FALSE"},
  {XtNselectCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
   XtOffset(PyraminxWidget, pyraminx.select), XtRCallback, NULL}
};

PyraminxClassRec pyraminxClassRec =
{
  {
    (WidgetClass) &widgetClassRec,	/* superclass */
    "Pyraminx",				/* class name */
    sizeof(PyraminxRec),		/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializePyraminx,			/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    actionsListPyraminx,		/* actions */
    XtNumber(actionsListPyraminx),	/* num actions */
    resourcesPyraminx,			/* resources */
    XtNumber(resourcesPyraminx),	/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    DestroyPyraminx,			/* destroy */
    ResizePyraminx,			/* resize */
    ExposePyraminx,			/* expose */
    SetValuesPyraminx,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    NULL,				/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    defaultTranslationsPyraminx,	/* tm table */
    NULL,				/* query geometry */
    NULL,				/* display accelerator */
    NULL				/* extension */
  },
  {
    0					/* ignore */
  }
};

WidgetClass pyraminxWidgetClass = (WidgetClass) &pyraminxClassRec;

typedef struct _RowNextP3
{
  int view_changed, face, direction, reverse;
} RowNextP3;
static PyraminxLoc slide_next_row_p2[MAXSIDES][3] =
{
  {{2, 0}, {1, 3}, {2, 3}},
  {{2, 0}, {0, 3}, {2, 3}}
};
static RowNextP3 slide_next_row_p3[MAXSIDES][MAXORIENT] =
{
  {
    { TRUE,   UP,     TR, FALSE},
    { TRUE,   UP,    TOP, FALSE},
    {FALSE,   UP, BOTTOM,  TRUE},
    {FALSE,   UP,  RIGHT,  TRUE},
    { TRUE, DOWN,  RIGHT,  TRUE},
    { TRUE, DOWN,     TR,  TRUE}
  },
  { 
    {FALSE, DOWN,   LEFT,  TRUE},
    { TRUE,   UP,   LEFT,  TRUE},
    { TRUE,   UP,     BL,  TRUE},
    { TRUE, DOWN,     BL, FALSE},
    { TRUE, DOWN, BOTTOM, FALSE},
    {FALSE, DOWN,    TOP,  TRUE}
  }
};
static int rot_orient_row_p3[3][MAXORIENT] =
/* current orient, rotation */
{
  {1, 5, 1, 5, 4, 2},
  {2, 0, 2, 0, 5, 3},
  {3, 1, 3, 1, 0, 4}
};

static XPoint triangle_unit[MAXSIDES][3] =
{
  {{0, 2}, { 1, 0}, {-1, 1}},
  {{1, 3}, {-1, 0}, {1, -1}}
};
static XPoint triangle_list[MAXSIDES][3],
  letter_list[MAXSIDES], offset_list[MAXSIDES];

static void InitializePyraminx(request, new)
  Widget request, new;
{
  PyraminxWidget w = (PyraminxWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  int face;
  
  check_polyhedrons(w);
  init_moves();
  reset_polyhedrons(w);
  w->pyraminx.practice = FALSE;
  (void) SRAND(getpid());
  valueMask = GCForeground | GCBackground;
  values.foreground = w->pyraminx.foreground;
  values.background = w->core.background_pixel;
  w->pyraminx.puzzle_GC = XtGetGC(new, valueMask, &values);
  w->pyraminx.depth = DefaultDepthOfScreen(XtScreen(w));
  valueMask = GCForeground | GCBackground;
  values.foreground = w->core.background_pixel;
  values.background = w->pyraminx.foreground;
  w->pyraminx.inverse_GC = XtGetGC(new, valueMask, &values);
  for (face = 0; face < MAXFACES; face++)
    get_color(w, face, TRUE);
  ResizePyraminx(w);
}

static void DestroyPyraminx(old)
  Widget old;
{
  PyraminxWidget w = (PyraminxWidget) old;
  int face;

  for (face = 0; face < MAXFACES; face++)
    XtReleaseGC(old, w->pyraminx.face_GC[face]);
  XtReleaseGC(old, w->pyraminx.puzzle_GC);
  XtReleaseGC(old, w->pyraminx.inverse_GC);
  XtRemoveCallbacks(old, XtNselectCallback, w->pyraminx.select);
}

static void ResizePyraminx(w)
  PyraminxWidget w;
{
  int temp_length;

  w->pyraminx.delta = 4;
  w->pyraminx.vertical = (w->core.height >= w->core.width);
  if (w->pyraminx.vertical)
    temp_length = MIN(w->core.height / 2, w->core.width);
  else
    temp_length = MIN(w->core.height, w->core.width / 2);
  w->pyraminx.tetra_length = MAX((temp_length - w->pyraminx.delta + 1) /
    w->pyraminx.size, 0);
  w->pyraminx.face_length = w->pyraminx.size * w->pyraminx.tetra_length;
  w->pyraminx.view_length = w->pyraminx.face_length + w->pyraminx.delta + 3;
  if (w->pyraminx.vertical) {
    w->pyraminx.puzzle_width = w->pyraminx.view_length - 1;
    w->pyraminx.puzzle_height = 2 * w->pyraminx.view_length -
      w->pyraminx.delta - 2;
  } else {
    w->pyraminx.puzzle_width = 2 * w->pyraminx.view_length -
      w->pyraminx.delta - 2;
    w->pyraminx.puzzle_height = w->pyraminx.view_length - 1;
  }
  w->pyraminx.puzzle_offset.x = ((int) w->core.width -
    w->pyraminx.puzzle_width) / 2;
  w->pyraminx.puzzle_offset.y = ((int) w->core.height -
    w->pyraminx.puzzle_height) / 2;
  resize_polyhedrons(w);
}

static void ExposePyraminx(new, event, region)
  Widget new;
  XEvent *event;
  Region region; /* Not used */
{
  PyraminxWidget w = (PyraminxWidget) new;

  if (w->core.visible) {
    draw_frame(w, w->pyraminx.puzzle_GC);
    draw_all_polyhedrons(w);
  }
}

static Boolean SetValuesPyraminx(current, request, new)
  Widget current, request, new;
{
  PyraminxWidget c = (PyraminxWidget) current, w = (PyraminxWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  Boolean redraw = FALSE;
  int face;

  check_polyhedrons(w);
  if (w->pyraminx.foreground != c->pyraminx.foreground) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->pyraminx.foreground;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->pyraminx.puzzle_GC);
    w->pyraminx.puzzle_GC = XtGetGC(new, valueMask, &values);
    redraw = TRUE;
  }
  if (w->core.background_pixel != c->core.background_pixel) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->core.background_pixel;
    values.background = w->pyraminx.foreground;
    XtReleaseGC(new, w->pyraminx.inverse_GC);
    w->pyraminx.inverse_GC = XtGetGC(new, valueMask, &values);
    redraw = TRUE;
  }
  if (w->pyraminx.mono || w->pyraminx.depth == 1) {
    valueMask = GCForeground | GCBackground;
    values.background = w->core.background_pixel;
    values.foreground = w->pyraminx.foreground;
    for (face = 0; face < MAXFACES; face++) {
      XtReleaseGC(new, w->pyraminx.face_GC[face]);
      w->pyraminx.face_GC[face] = XtGetGC(new, valueMask, &values);
    }
    redraw = TRUE;
  }
  for (face = 0; face < MAXFACES; face++) {
    if (strcmp(w->pyraminx.face_name[face], c->pyraminx.face_name[face]))
      get_color(w, face, FALSE);
  }
  if (w->pyraminx.orient != c->pyraminx.orient) {
    reset_polyhedrons(w);
    w->pyraminx.practice = FALSE;
    redraw = TRUE;
  } else if (w->pyraminx.practice != c->pyraminx.practice) {
    reset_polyhedrons(w);
    redraw = TRUE;
  }
  if (w->pyraminx.size != c->pyraminx.size ||
      w->pyraminx.mode != c->pyraminx.mode ||
      w->pyraminx.sticky != c->pyraminx.sticky) {
    reset_polyhedrons(w);
    w->pyraminx.practice = FALSE;
    ResizePyraminx(w);
    redraw = TRUE;
  }
  if (w->pyraminx.tetra_length != c->pyraminx.tetra_length) {
    ResizePyraminx(w);
    redraw = TRUE;
  }
  return (redraw);
}

static void quit_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  XtCloseDisplay(XtDisplay(w));
  exit(0);
}

static void practice_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  practice_polyhedrons(w);
}
 
static void maybe_practice_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->pyraminx.started)
    practice_polyhedrons(w);
}
 
static void randomize_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  randomize_polyhedrons(w);
}
 
static void maybe_randomize_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->pyraminx.started)
    randomize_polyhedrons(w);
}

static void enter_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;
  char c;
  int i, size, mode, sticky, orient, practice, moves;
  pyraminxCallbackStruct cb;

  if ((fp = fopen(DATAFILE, "r")) == NULL)
    (void) printf("Can not read %s to enter.\n", DATAFILE);
  else {
    flush_moves(w);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &size);
    if (size >= MINTETRAS && size <= MAXTETRAS) {
      for (i = w->pyraminx.size; i < size; i++) {
        cb.reason = PYRAMINX_INC;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
      for (i = w->pyraminx.size; i > size; i--) {
        cb.reason = PYRAMINX_DEC;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    } else
      (void) printf("%s corrupted: size %d should be between %d and %d\n",
         DATAFILE, size, MINTETRAS, MAXTETRAS);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &mode);
    if (mode >= PERIOD2 && mode <= BOTH)
      switch (mode) {
        case PERIOD2: 
          cb.reason = PYRAMINX_PERIOD2;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          break;
        case PERIOD3: 
          cb.reason = PYRAMINX_PERIOD3;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          break;
        case BOTH: 
          cb.reason = PYRAMINX_BOTH;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    else
      (void) printf("%s corrupted: mode %d should be between %d and %d\n",
         DATAFILE, mode, PERIOD2, BOTH);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &sticky);
    if (w->pyraminx.sticky != (Boolean) sticky) {
      cb.reason = PYRAMINX_STICKY;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &orient);
    if (w->pyraminx.orient != (Boolean) orient) {
      cb.reason = PYRAMINX_ORIENT;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &practice);
    if (w->pyraminx.practice != (Boolean) practice) {
      cb.reason = PYRAMINX_PRACTICE;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &moves);
    scan_start_position(fp, w);
    cb.reason = PYRAMINX_RESTORE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    set_start_position(w);
    scan_moves(fp, w, moves);
    (void) fclose(fp);
    (void) printf("%s: size %d, mode %d, sticky %d, orient %d",
      DATAFILE, size, mode, sticky, orient);
    (void) printf(", practice %d, moves %d.\n", practice,  moves);
  }
}

static void write_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;

  if ((fp = fopen(DATAFILE, "w")) == NULL)
    (void) printf("Can not write to %s.\n", DATAFILE);
  else {
    (void) fprintf(fp, "size: %d\n", w->pyraminx.size);
    (void) fprintf(fp, "mode: %d\n", w->pyraminx.mode);
    (void) fprintf(fp, "sticky: %d\n", (w->pyraminx.sticky) ? 1 : 0);
    (void) fprintf(fp, "orient: %d\n", (w->pyraminx.orient) ? 1 : 0);
    (void) fprintf(fp, "practice: %d\n", (w->pyraminx.practice) ? 1 : 0);
    (void) fprintf(fp, "moves: %d\n", num_moves());
    print_start_position(fp, w);
    print_moves(fp);
    (void) fclose(fp);
    (void) printf("Saved to %s.\n", DATAFILE);
  }
}

static void undo_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (moves_made()) {
    int view, i, j, side, direction, style, control;

    get_move(&view, &i, &j, &side, &direction, &style, &control);
    direction = (direction < MAXORIENT) ? (direction + MAXORIENT / 2) %
      MAXORIENT : 3 * MAXORIENT - direction;
    if (control)
      move_control_cb(w, view, i, j, side, direction, style);
    else {
      pyraminxCallbackStruct cb;

      move_polyhedrons(w, view, i, j, side, direction, style);
      cb.reason = PYRAMINX_UNDO;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}

static void solve_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  /* solve_polyhedrons(w); */ /* Sorry, unimplemented */
}
 
static void increment_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  pyraminxCallbackStruct cb;
 
  if (w->pyraminx.size == MAXTETRAS)
    return;
  cb.reason = PYRAMINX_INC;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
static void decrement_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  pyraminxCallbackStruct cb;
 
  if (w->pyraminx.size == MINTETRAS)
    return;
  cb.reason = PYRAMINX_DEC;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
static void orientize_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  pyraminxCallbackStruct cb;
 
  cb.reason = PYRAMINX_ORIENT;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void period2_mode_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  pyraminxCallbackStruct cb;
 
  cb.reason = PYRAMINX_PERIOD2;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void period3_mode_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  pyraminxCallbackStruct cb;
 
  cb.reason = PYRAMINX_PERIOD3;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void both_mode_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  pyraminxCallbackStruct cb;
 
  cb.reason = PYRAMINX_BOTH;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void sticky_mode_pyraminx(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  pyraminxCallbackStruct cb;
 
  cb.reason = PYRAMINX_STICKY;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void move_pyraminx_ccw(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_pyraminx_input(w, event->xbutton.x, event->xbutton.y, CCW,
    (int) (event->xbutton.state & (ShiftMask | LockMask)),
    (int) (event->xbutton.state & ControlMask));
}

static void move_pyraminx_top(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_pyraminx_input(w, event->xbutton.x, event->xbutton.y, TOP,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_pyraminx_tr(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_pyraminx_input(w, event->xbutton.x, event->xbutton.y, TR,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_pyraminx_left(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_pyraminx_input(w, event->xbutton.x, event->xbutton.y, LEFT,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_pyraminx_cw(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_pyraminx_input(w, event->xbutton.x, event->xbutton.y, CW,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_pyraminx_right(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_pyraminx_input(w, event->xbutton.x, event->xbutton.y, RIGHT,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_pyraminx_bl(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_pyraminx_input(w, event->xbutton.x, event->xbutton.y, BL,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_pyraminx_bottom(w, event, args, n_args)
  PyraminxWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_pyraminx_input(w, event->xbutton.x, event->xbutton.y, BOTTOM,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_pyraminx_input(w, x, y, direction, shift, control)
  PyraminxWidget w;
  int x, y, direction, shift, control;
{
  int style, view, i, j, side;

  if (w->pyraminx.mode != BOTH) {
    if (control && shift)
      style = (w->pyraminx.mode == PERIOD3) ? PERIOD2 : PERIOD3;
    else
      style = (w->pyraminx.mode == PERIOD2) ? PERIOD2 : PERIOD3;
  } else
    style = (shift) ? PERIOD3 : PERIOD2;
  if (!w->pyraminx.practice && !control && check_solved(w)) {
    no_move_polyhedrons(w);
    return;
  }
  if (!position_polyhedrons(w, x, y, style, &view, &i, &j, &side, &direction))
    return;
  control = (control) ? 1 : 0;
  move_pyraminx(w, view, i, j, side, direction, style, control);
  if (!control && check_solved(w)) {
    pyraminxCallbackStruct cb;

    cb.reason = PYRAMINX_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

void move_pyraminx(w, view, i, j, side, direction, style, control)
  PyraminxWidget w;
  int view, i, j, side, direction, style, control;
{
  if (control)
    move_control_cb(w, view, i, j, side, direction, style); 
  else {
    pyraminxCallbackStruct cb;

    move_polyhedrons(w, view, i, j, side, direction, style);
    cb.reason = PYRAMINX_MOVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  put_move(view, i, j, side, direction, style, control);
}

static void get_color(w, face, init)
  PyraminxWidget w;
  int face, init;
{
  XGCValues values;
  XtGCMask valueMask;
  XColor color_cell, rgb;
 
  valueMask = GCForeground | GCBackground;
  values.background = w->core.background_pixel;
  if (w->pyraminx.depth > 1 && !w->pyraminx.mono) {
    if (XAllocNamedColor(XtDisplay(w),
        DefaultColormap(XtDisplay(w), XtWindow(w)),
        w->pyraminx.face_name[face], &color_cell, &rgb)) {
      values.foreground = w->pyraminx.face_color[face] = color_cell.pixel;
      if (!init)
        XtReleaseGC((Widget) w, w->pyraminx.face_GC[face]);
      w->pyraminx.face_GC[face] = XtGetGC((Widget) w, valueMask, &values);
      return;
    } else {
      char buf[121];
 
      (void) sprintf(buf, "Color name \"%s\" is not defined",
               w->pyraminx.face_name[face]);
      XtWarning(buf);
    }
  }
  values.foreground = w->pyraminx.foreground;
  if (!init)
    XtReleaseGC((Widget) w, w->pyraminx.face_GC[face]);
  w->pyraminx.face_GC[face] = XtGetGC((Widget) w, valueMask, &values);
}

static void move_control_cb(w, view, i, j, side, direction, style)
  PyraminxWidget w;
  int view, i, j, side, direction, style;
{
  pyraminxCallbackStruct cb;
  int face;

  face = !(i + j + side < w->pyraminx.size);
  if (w->pyraminx.sticky) {
    if (style == PERIOD2)
      for (i = 0; i < 3; i++) {
        if (direction == TR || direction == BL)
          move_polyhedrons(w, view, 0, 3 * i / 2, i % 2, direction, style);
        else
          move_polyhedrons(w, view, 3 * i / 2, 3 * i / 2, DOWN,
            direction, style);
        cb.reason = PYRAMINX_CONTROL;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    else /* (style == PERIOD3) */
      for (i = 0; i < 2; i++) {
        if (direction == TR || direction == BL)
          move_polyhedrons(w, view, 1 + face, 1 + face, i, direction, style);
        else
          move_polyhedrons(w, view, 2 * face + i, 2 * face + i, face,
            direction, style);
        cb.reason = PYRAMINX_CONTROL;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
  } else {
    for (i = 0; i < w->pyraminx.size; i++) {
      if (direction == TR || direction == BL) {
        if (style == PERIOD2)
          move_polyhedrons(w, view, 0, i, DOWN, direction, style);
        else
          move_polyhedrons(w, view, face * (w->pyraminx.size - 1), i, face,
            direction, style);
      } else
        move_polyhedrons(w, view, i, w->pyraminx.size - 1 - i, face,
          direction, style);
      cb.reason = PYRAMINX_CONTROL;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}

static void check_polyhedrons(w)
  PyraminxWidget w;
{
  if (w->pyraminx.size < MINTETRAS || w->pyraminx.size > MAXTETRAS) {
    char buf[121];

    (void) sprintf(buf,
             "Number of Tetras on edge out of bounds, use %d..%d",
             MINTETRAS, MAXTETRAS);
    XtWarning(buf);
    w->pyraminx.size = DEFAULTTETRAS;
  }
  if (w->pyraminx.mode < PERIOD2 || w->pyraminx.mode > BOTH) {
    XtWarning("Mode is in error, use 2 for Period2, 3 for Period3, 4 for Both");
    w->pyraminx.mode = DEFAULTMODE;
  }
}

static void reset_polyhedrons(w)
  PyraminxWidget w;
{
  int view, i, j, side, face;

  for (i = 0; i < w->pyraminx.size; i++)
    for (j = 0; j < w->pyraminx.size; j++)
      for (side = 0; side < MAXSIDES; side++) {
        if (i + j < w->pyraminx.size - 1)
          face = DOWN;
        else if (i + j >= w->pyraminx.size)
          face = UP;
        else
          face = side;
        for (view = DOWN; view <= UP; view++) {
          w->pyraminx.tetra_loc[view][i][j][side].face = MAXSIDES * view +
            face;
          w->pyraminx.tetra_loc[view][i][j][side].rotation = TOP;
        }
      }
  flush_moves(w);
  w->pyraminx.started = FALSE;
}

static void resize_polyhedrons(w)
  PyraminxWidget w;
{
  int i, j;

  w->pyraminx.tetra_length = w->pyraminx.face_length / w->pyraminx.size -
    w->pyraminx.delta - 1;
  for (i = 0; i < 3; i++)
    for (j = 0; j < MAXSIDES; j++) { 
      triangle_list[j][i].x = triangle_unit[j][i].x *
        w->pyraminx.tetra_length;
      triangle_list[j][i].y = triangle_unit[j][i].y *
        w->pyraminx.tetra_length;
    }
  offset_list[DOWN].x = 0;
  offset_list[UP].x = w->pyraminx.tetra_length + 2;
  offset_list[DOWN].y = 0;
  offset_list[UP].y = w->pyraminx.tetra_length + 2;
  letter_list[DOWN].x =    w->pyraminx.tetra_length / 4 - 3;
  letter_list[UP].x =  3 * w->pyraminx.tetra_length / 4;
  letter_list[DOWN].y =    w->pyraminx.tetra_length / 4 + 5;
  letter_list[UP].y =  3 * w->pyraminx.tetra_length / 4 + 5;
  w->pyraminx.side_offset = 3 * w->pyraminx.size / 4;
  w->pyraminx.orient_line_length = w->pyraminx.tetra_length / 4;
  w->pyraminx.orient_diag_length = MAX(w->pyraminx.orient_line_length - 3, 0);
}


static int position_polyhedrons(w, x, y, style, view, i, j, side, direction)
  PyraminxWidget w;
  int x, y, style;
  int *view, *i, *j, *side, *direction;
{
  int offset, mod_i, mod_j;

  x -= w->pyraminx.puzzle_offset.x;
  y -= w->pyraminx.puzzle_offset.y;
  if (w->pyraminx.vertical && y > w->pyraminx.view_length - 1) {
    y -= (w->pyraminx.view_length - 1);
    *view = DOWN;
  } else if (!w->pyraminx.vertical && x > w->pyraminx.view_length - 1) {
    x -= (w->pyraminx.view_length - 1);
    *view = DOWN;
  } else
    *view = UP;
  if (x <= 0 || y <= 0 ||
      x >= w->pyraminx.face_length + w->pyraminx.delta ||
      y >= w->pyraminx.face_length + w->pyraminx.delta)
    return FALSE;
  else if (x + y > w->pyraminx.face_length)
    offset = 2 * w->pyraminx.delta + 1;
  else
    offset = w->pyraminx.delta;
  *i = (x - offset) / (w->pyraminx.tetra_length + w->pyraminx.delta);
  *j = (y - offset) / (w->pyraminx.tetra_length + w->pyraminx.delta);
  mod_i = (x - offset) % (w->pyraminx.tetra_length + w->pyraminx.delta);
  mod_j = (y - offset) % (w->pyraminx.tetra_length + w->pyraminx.delta);
  *side = (mod_i + mod_j > w->pyraminx.tetra_length + 1);
  if (!w->pyraminx.vertical && *view == DOWN) {
    *i = w->pyraminx.size - *i - 1;
    *j = w->pyraminx.size - *j - 1;
    *side = !*side;
    if (*direction < MAXORIENT)
      *direction = (*direction + MAXORIENT / 2) % MAXORIENT;
  }
  if (style == PERIOD2) {
    if (*direction == CW) 
      *direction = TR;
    else if (*direction == CCW)
      *direction = BL; 
  } else /* style == PERIOD3 */ { 
    if (*direction == CW || *direction == CCW) {
      *side = (*i + *j + *side < w->pyraminx.size);
      *direction = ((*direction ==  CW && *side == DOWN) ||
                    (*direction == CCW && *side == UP)) ? TR : BL;
      *i = w->pyraminx.size - 1;
      *j = 0;
    }
  }
  return TRUE;
}

static void no_move_polyhedrons(w)
  PyraminxWidget w;
{
  pyraminxCallbackStruct cb;
 
  cb.reason = PYRAMINX_IGNORE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void practice_polyhedrons(w)
  PyraminxWidget w;
{
  pyraminxCallbackStruct cb;
 
  cb.reason = PYRAMINX_PRACTICE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void randomize_polyhedrons(w)
  PyraminxWidget w;
{
  pyraminxCallbackStruct cb;
  int tetra_x = 0, tetra_y = 0, random_direction = 0, side, view, style;
  int big = w->pyraminx.size * w->pyraminx.size * 3 + NRAND(2);

  if (w->pyraminx.practice)
    practice_polyhedrons(w);
  if (w->pyraminx.sticky)
    big /= 3;
  cb.reason = PYRAMINX_RESET;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
 
#ifdef DEBUG
  big = 3;
#endif

  while (big--) {
    view = NRAND(MAXVIEWS);
    side = NRAND(MAXSIDES);
    if (w->pyraminx.mode == BOTH)
      style = NRAND(MAXMODES - 1) + PERIOD2;
    else
      style = w->pyraminx.mode;
    if (w->pyraminx.sticky) {
      if (style == PERIOD2) {
        switch (NRAND(3)) {
          case 0:
            tetra_x = tetra_y = 3 * NRAND(3) / 2;
            random_direction = (NRAND(2)) ? LEFT : RIGHT;
            break;
          case 1:
            tetra_x = tetra_y = 3 * NRAND(3) / 2;
            random_direction = (NRAND(2)) ? TOP : BOTTOM;
            break;
          case 2:
            side = NRAND(3);
            tetra_x = 0;
            tetra_y = 3 * side / 2;
            side = side % 2;
            random_direction = (NRAND(2)) ? TR : BL;
         }
      } else /* style == PERIOD3 */ {
        tetra_x = tetra_y = (NRAND(2)) ? 1 : 2;
        random_direction = NRAND(6);
      }
    } else /* (!w->pyraminx.sticky) */ {
      random_direction = NRAND(MAXORIENT);
      tetra_x = NRAND(w->pyraminx.size);
      tetra_y = NRAND(w->pyraminx.size);
      if (w->pyraminx.mode == BOTH)
        style = NRAND(BOTH);
      else
        style = w->pyraminx.mode;
    }
    move_pyraminx(w, view, tetra_x, tetra_y, side, random_direction,
      style, FALSE);
    cb.reason = PYRAMINX_MOVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  flush_moves(w);
  cb.reason = PYRAMINX_RANDOMIZE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  if (check_solved(w)) {
    cb.reason = PYRAMINX_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void move_polyhedrons(w, view, i, j, side, direction, style)
  PyraminxWidget w;
  int view, i, j, side, direction, style;
{
  int new_side, new_view, rotate, reverse, h, k, new_h, face, len;
  int new_face, new_direction, bound, l = 0;

  if (style == PERIOD2) {
    /* Period 2 Slide rows */
    h = rcd (direction, i, j, side);
    if (w->pyraminx.sticky && (h % 4 == 1 || h % 4 == 2)) {
      bound = TRUE;
      l = 0;
      if (h % 4 == 2)
        h = h - 1;
    } else
      bound = FALSE;
    do {
      read_rcd2(w, view, direction, h, 0);
      for (k = 1; k <= 2; k++) {
        rotate = slide_next_row_p2[side][direction % 3].rotation;
        if (direction == TR || direction == BL) {
          new_view = view;
          new_side = !side;
          new_h = 2 * w->pyraminx.size - 1 - h;
          reverse = FALSE;
        } else if (rotate == 0) {
          new_view = !view;
          new_side = side;
          new_h = h;
          reverse = FALSE;
        } else /* rotate == 3 */ {
          new_view = !view;
          new_side = !side;
          new_h = w->pyraminx.size - 1 - h;
          reverse = TRUE;
        }
        if (k != 2)
          read_rcd2(w, new_view, direction, new_h, k);
        rotate_rcd2(w, rotate, k - 1);
        if (reverse == TRUE)
          reverse_rcd2(w, k - 1);
        write_rcd2(w, new_view, direction, new_h, k - 1);
        view = new_view;
        h = new_h;
        side = new_side;
      }
      l++;
      h++;
    } while (bound && l < 2);
  } else /* style == PERIOD3 */ {
    face = !(i + j + side < w->pyraminx.size);
    h = rcd (direction, i, j, side);
    bound = FALSE;
    if (direction == TR || direction == BL) {
      if (h == w->pyraminx.size)
        rotate_face(w, view, DOWN, (direction == TR) ? CCW : CW);
      else if (h == w->pyraminx.size - 1)
        rotate_face(w, view,   UP, (direction == TR) ? CW : CCW);
      else if (w->pyraminx.sticky)
        bound = TRUE;
    } else if (i == 0 && !face && (direction == TOP || direction == BOTTOM))
      rotate_face(w, !view, DOWN,
                  (direction == TOP || direction == LEFT) ? CCW : CW);
    else if (i == w->pyraminx.size - 1 && face &&
             (direction == TOP || direction == BOTTOM))
      rotate_face(w, !view, UP,
                  (direction == BOTTOM || direction == RIGHT) ? CCW : CW);
    else if (j == 0 && !face && (direction == RIGHT || direction == LEFT))
      rotate_face(w, !view, UP,
                  (direction == BOTTOM || direction == RIGHT) ? CCW : CW);
    else if (j == w->pyraminx.size - 1 && face &&
             (direction == RIGHT || direction == LEFT))
      rotate_face(w, !view, DOWN,
                  (direction == TOP || direction == LEFT) ? CCW : CW);
    else if (w->pyraminx.sticky)
      bound = TRUE;
    /* Slide rows */
    if (bound == TRUE) {
      l = 0;
      if (direction == TR || direction == BL) {
        if (face == DOWN)
          h = 0;
        else /* face == UP */
          h = w->pyraminx.size + 1;
      } else {
        if (face == DOWN)
          h = 1;
        else /* face == UP */
          h = 0;
      }
    }
    do {
      len = length(w, face, direction, h);
      read_rcd3(w, view, face, direction, h, len, 0);
      for (k = 1; k <= 3; k++) {
        new_view = (slide_next_row_p3[face][direction].view_changed)
          ? !view : view;
        new_face = slide_next_row_p3[face][direction].face;
        new_direction = slide_next_row_p3[face][direction].direction;
        reverse = slide_next_row_p3[face][direction].reverse;
        new_h = w->pyraminx.size - 1 - h;
        if (face == DOWN) {
          if (direction == TOP)
            new_h = w->pyraminx.size + h;
          else if (direction == TR)
            new_h = h;
          else
            new_h = w->pyraminx.size - 1 - h;
        } else /* face == UP */ {
          if (direction == TR || direction == RIGHT)
            new_h = 2 * w->pyraminx.size - 1 - h;
          else if (direction == BOTTOM)
            new_h = h;
          else if (direction == BL)
            new_h = h - w->pyraminx.size;
          else
            new_h = w->pyraminx.size - 1 - h;
        }
        if (k != 3)
          read_rcd3(w, new_view, new_face, new_direction, new_h, len, k);
        rotate_rcd3(w, face, direction, len, k - 1);
        if (reverse == TRUE)
          reverse_rcd3(w, len, k - 1);
        write_rcd3(w, new_view, new_face, new_direction, new_h, len, k - 1);
        view = new_view;
        face = new_face;
        direction = new_direction;
        h = new_h;
      }
      h++;
      l++;
    } while (bound && l < w->pyraminx.size - 1);
  }
}

static void rotate_face(w, view, face, direction)
  PyraminxWidget w;
  int view, face, direction;
{
  int g, h, side;

  /* Read Face */
  for (g = 0; g < w->pyraminx.size; g++)
    for (h = 0; h < w->pyraminx.size - g; h++)
      for (side = 0; side <= UP; side++)
        if (g + h + side < w->pyraminx.size) {
          if (face == DOWN)
            w->pyraminx.face_loc[h][g][side] =
              w->pyraminx.tetra_loc[view][h][g][side];
          else /* face == UP */
            w->pyraminx.face_loc[h][g][side] =
              w->pyraminx.tetra_loc[view][w->pyraminx.size - 1 - h]
                       [w->pyraminx.size - g - 1][!side];
        }
  /* Write Face */
  if (face == DOWN) {
    for (g = 0; g < w->pyraminx.size; g++)
      for (h = 0; h < w->pyraminx.size - g; h++)
        for (side = DOWN; side <= UP; side++)
          if (g + h + side < w->pyraminx.size) {
            if (direction == CCW)
              w->pyraminx.tetra_loc[view][h][g][side] =
                w->pyraminx.face_loc[w->pyraminx.size - 1 - g - h - side]
                                   [h][side];
            else /* direction == CW */
              w->pyraminx.tetra_loc[view][h][g][side] =
                w->pyraminx.face_loc[g][w->pyraminx.size - 1 - g - h - side]
                                   [side];
            w->pyraminx.tetra_loc[view][h][g][side].rotation =
              (direction == CW) ?
                (w->pyraminx.tetra_loc[view][h][g][side].rotation + 2) %
                MAXORIENT:
                (w->pyraminx.tetra_loc[view][h][g][side].rotation + 4) %
                MAXORIENT;
            draw_triangle(w, view, h, g, side);
          }
  } else /* face == UP */ {
    for (g = w->pyraminx.size - 1; g >= 0; g--)
      for (h = w->pyraminx.size - 1; h >= w->pyraminx.size - 1 - g; h--)
        for (side = UP; side >= DOWN; side--)
          if (g + h + side >= w->pyraminx.size) {
            if (direction == CCW)
              w->pyraminx.tetra_loc[view][h][g][side] =
                w->pyraminx.face_loc[g + h - w->pyraminx.size + 1 - !side]
                        [w->pyraminx.size - 1 - h][!side];
            else /* (direction == CW) */
              w->pyraminx.tetra_loc[view][h][g][side] =
                w->pyraminx.face_loc[w->pyraminx.size - 1 - g]
                        [g + h - w->pyraminx.size + 1 - !side][!side];
            w->pyraminx.tetra_loc[view][h][g][side].rotation =
              (direction == CW) ?
                (w->pyraminx.tetra_loc[view][h][g][side].rotation + 2) %
                MAXORIENT:
                (w->pyraminx.tetra_loc[view][h][g][side].rotation + 4) %
                MAXORIENT;
            draw_triangle(w, view, h, g, side);
          }
  }
}

static void read_rcd2(w, view, dir, h, orient)
  PyraminxWidget w;
  int view, dir, h, orient;
{
  int g, i, j, side, face, s;

  if (dir == TOP || dir == BOTTOM)
    for (g = 0; g < w->pyraminx.size; g++)
      for (side = DOWN; side <= UP; side++)
        w->pyraminx.row_loc_p2[orient][g][side] =
          w->pyraminx.tetra_loc[view][h][g][side];
  else if (dir == RIGHT || dir == LEFT)
    for (g = 0; g < w->pyraminx.size; g++)
      for (side = 0; side < MAXSIDES; side++)
        w->pyraminx.row_loc_p2[orient][g][side] =
          w->pyraminx.tetra_loc[view][g][h][side];
  else /* dir == TR || dir == BL */ {
    face = (h >= w->pyraminx.size);
    i = (face == UP) ? w->pyraminx.size - 1 : 0;
    j = h % w->pyraminx.size;
    for (g = 0; g < w->pyraminx.size; g++) {
      for (side = DOWN; side <= UP; side++) {
        s = (side == UP) ? !face : face;
        w->pyraminx.row_loc_p2[orient][g][side] =
          w->pyraminx.tetra_loc[view][i][j][s];
        if (side == DOWN) {
          if (face == UP) {
            if (j == w->pyraminx.size - 1)
              view = !view;
            j = (j + 1) % w->pyraminx.size;
          } else /* face == DOWN */ {
            if (j == 0)
              view = !view;
            j = (j - 1 + w->pyraminx.size) % w->pyraminx.size;
          }
        }
      }
      i = (face == UP) ? i - 1 : i + 1;
    }
  }
}

static void read_rcd3(w, view, face, dir, h, len, orient)
  PyraminxWidget w;
  int view, face, dir, h, len, orient;
{
  int g, i, j, side, s;

  if (dir == TOP || dir == BOTTOM) {
    for (g = 0; g <= len; g++)
      for (side = DOWN; side <= UP; side++)
        if (side == DOWN || g < len)
          w->pyraminx.row_loc_p3[orient][g][side] = (face == DOWN) ?
            w->pyraminx.tetra_loc[view][h][g][side] :
            w->pyraminx.tetra_loc[view][h][w->pyraminx.size - 1 - g][!side];
  } else if (dir == RIGHT || dir == LEFT) {
    for (g = 0; g <= len; g++)
      for (side = DOWN; side <= UP; side++)
        if (side == DOWN || g < len)
          w->pyraminx.row_loc_p3[orient][g][side] = (face == DOWN) ?
            w->pyraminx.tetra_loc[view][g][h][side] : 
            w->pyraminx.tetra_loc[view][w->pyraminx.size - 1 - g][h][!side];
  } else /* dir == TR || dir == BL */ {
    i = (face == UP) ? w->pyraminx.size - 1 : 0;
    j = h % w->pyraminx.size;
    for (g = 0; g <= len; g++) {
      for (side = DOWN; side <= UP; side++) {
        if (side == DOWN || g < len) {
          s = (side == UP) ? !face : face;
          w->pyraminx.row_loc_p3[orient][g][side] =
            w->pyraminx.tetra_loc[view][i][j][s];
          if (side == DOWN)
            j = (face == UP) ? j + 1 : j - 1;
        }
      }
      i = (face == UP) ? i - 1 : i + 1;
    }
  }
}

static void rotate_rcd2(w, rotate, orient)
  PyraminxWidget w;
  int rotate, orient;
{
  int g, side;

  for (g = 0; g < w->pyraminx.size; g++)
    for (side = DOWN; side <= UP; side++)
      w->pyraminx.row_loc_p2[orient][g][side].rotation = 
        (w->pyraminx.row_loc_p2[orient][g][side].rotation + rotate) % MAXORIENT;
}

static void rotate_rcd3(w, face, dir, len, orient)
  PyraminxWidget w;
  int face, dir, len, orient;
{
  int g, side, direction, tetra_orient;

  for (g = 0; g <= len; g++)
    for (side = DOWN; side <= UP; side++)
      if (side == DOWN || g < len) {
        direction = (face == UP) ? (dir + 3) % MAXORIENT : dir;
        tetra_orient = w->pyraminx.row_loc_p3[orient][g][side].rotation;
        w->pyraminx.row_loc_p3[orient][g][side].rotation =
          (tetra_orient >= 3) ?
          (rot_orient_row_p3[tetra_orient - 3][direction] + 3) % MAXORIENT :
          rot_orient_row_p3[tetra_orient][direction];
      }
}

static void reverse_rcd2(w, orient)
  PyraminxWidget w;
  int orient;
{
  int g, side;
  PyraminxLoc temp[MAXTETRAS][MAXSIDES];

  for (g = 0; g < w->pyraminx.size; g++)
    for (side = DOWN; side <= UP; side++)
      temp[g][side] = w->pyraminx.row_loc_p2[orient][g][side];
  for (g = 0; g < w->pyraminx.size; g++)
    for (side = DOWN; side <= UP; side++)
      w->pyraminx.row_loc_p2[orient][g][side] =
        temp[w->pyraminx.size - 1 - g][!side];
}

static void reverse_rcd3(w, len, orient)
  PyraminxWidget w;
  int len, orient;
{
  int g, side;
  PyraminxLoc temp[MAXTETRAS][MAXSIDES];

  for (g = 0; g <= len; g++)
    for (side = DOWN; side <= UP; side++)
      if (side == DOWN || g < len)
        temp[g][side] = w->pyraminx.row_loc_p3[orient][g][side];
  for (g = 0; g <= len; g++)
    for (side = DOWN; side <= UP; side++)
      if (side == DOWN || g < len)
        w->pyraminx.row_loc_p3[orient][g][side] = temp[len - g - side][side];
}

static void write_rcd2(w, view, dir, h, orient)
  PyraminxWidget w;
  int view, dir, h, orient;
{
  int g, side, i, j, s, face;

  if (dir == TOP || dir == BOTTOM) {
    for (g = 0; g < w->pyraminx.size; g++)
      for (side = DOWN; side <= UP; side++) {
        w->pyraminx.tetra_loc[view][h][g][side] =
          w->pyraminx.row_loc_p2[orient][g][side];
        draw_triangle(w, view, h, g, side);
      }
  } else if (dir == RIGHT || dir == LEFT) {
    for (g = 0; g < w->pyraminx.size; g++)
      for (side = DOWN; side <= UP; side++) {
        w->pyraminx.tetra_loc[view][g][h][side] =
          w->pyraminx.row_loc_p2[orient][g][side];
        draw_triangle(w, view, g, h, side);
      }
  } else /* dir == TR || dir == BL */ {
    face = (h >= w->pyraminx.size);
    i = (face == UP) ? w->pyraminx.size - 1 : 0;
    j = h % w->pyraminx.size;
    for (g = 0; g < w->pyraminx.size; g++) {
      for (side = DOWN; side <= UP; side++) {
        s = (side == UP) ? !face : face;
        w->pyraminx.tetra_loc[view][i][j][s] =
          w->pyraminx.row_loc_p2[orient][g][side];
        draw_triangle(w, view, i, j, s);
        if (side == DOWN) {
          if (face == UP) {
            if (j == w->pyraminx.size - 1) {
              view = !view;
              j = 0;
            } else
              ++j;
          } else /* FACE == DOWN */ {
            if (j == 0) {
              view = !view;
              j = w->pyraminx.size - 1;
            } else
              --j;
          }
        }
      }
      if (face == UP)
        --i;
      else /* face == DOWN */
        ++i;
    }
  }
}

static void write_rcd3(w, view, face, dir, h, len, orient)
  PyraminxWidget w;
  int view, face, dir, h, len, orient;
{
  int g, side, i, j, k, s;

  if (dir == TOP || dir == BOTTOM) {
    for (k = 0; k <= len; k++)
      for (side = DOWN; side <= UP; side++)
        if (side == DOWN || k < len) {
          g = (face == UP) ? w->pyraminx.size - 1 - k : k;
          s = (face == UP) ? !side : side;
          w->pyraminx.tetra_loc[view][h][g][s] =
            w->pyraminx.row_loc_p3[orient][k][side];
          draw_triangle(w, view, h, g, s);
        }
  } else if (dir == RIGHT || dir == LEFT) {
    for (k = 0; k <= len; k++)
      for (side = DOWN; side <= UP; side++)
        if (side == DOWN || k < len) {
          g = (face == UP) ? w->pyraminx.size - 1 - k : k;
          s = (face == UP) ? !side : side;
          w->pyraminx.tetra_loc[view][g][h][s] =
            w->pyraminx.row_loc_p3[orient][k][side];
          draw_triangle(w, view, g, h, s);
        }
  } else /* dir == TR || dir == BL */ {
    face = (h >= w->pyraminx.size);
    i = (face == UP) ? w->pyraminx.size - 1 : 0;
    j = h % w->pyraminx.size;
    for (k = 0; k <= len; k++) {
      for (side = DOWN; side <= UP; side++)
        if (side == DOWN || k < len) {
          s = (side == UP) ? !face : face;
          w->pyraminx.tetra_loc[view][i][j][s] =
            w->pyraminx.row_loc_p3[orient][k][side];
          draw_triangle(w, view, i, j, s);
          if (side == DOWN) {
            if (face == UP) {
              if (j == w->pyraminx.size - 1) {
                view = !view;
                j = 0;
              } else
                ++j;
            } else /* FACE == DOWN */ {
              if (j == 0) {
                view = !view;
                j = w->pyraminx.size - 1;
              }
              else
                --j;
            }
          }
        }
      if (face == UP)
        --i;
      else /* face == DOWN */
        ++i;
    }
  }
}

static void draw_frame(w, gc)
  PyraminxWidget w;
  GC gc;
{
  int startx, starty, lengthx, lengthy, longlength;

  startx = 1 + w->pyraminx.puzzle_offset.x;
  starty = 1 + w->pyraminx.puzzle_offset.y;
  lengthx = w->pyraminx.view_length - w->pyraminx.delta +
    w->pyraminx.puzzle_offset.x;
  lengthy = w->pyraminx.view_length - w->pyraminx.delta +
    w->pyraminx.puzzle_offset.y;
  XDrawLine(XtDisplay(w), XtWindow(w), gc, startx, starty, lengthx, starty);
  XDrawLine(XtDisplay(w), XtWindow(w), gc, startx, starty, startx, lengthy);
  XDrawLine(XtDisplay(w), XtWindow(w), gc, lengthx, starty, lengthx, lengthy);
  XDrawLine(XtDisplay(w), XtWindow(w), gc, startx, lengthy, lengthx, lengthy);
  XDrawLine(XtDisplay(w), XtWindow(w), gc, startx, lengthy, lengthx, starty);
  if (w->pyraminx.vertical) {
    longlength = 2 * w->pyraminx.view_length - 2 * w->pyraminx.delta - 1 +
      w->pyraminx.puzzle_offset.y;
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      startx, lengthy, startx, longlength);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      lengthx, lengthy, lengthx, longlength);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      startx, longlength, lengthx, longlength);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      startx, longlength, lengthx, lengthy);
  } else {
    longlength = 2 * w->pyraminx.view_length - 2 * w->pyraminx.delta - 1 +
      w->pyraminx.puzzle_offset.x;
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      lengthx, starty, longlength, starty);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      lengthx, lengthy, longlength, lengthy);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      longlength, starty, longlength, lengthy);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      longlength, starty, lengthx, lengthy);
  }
}   

void draw_all_polyhedrons(w)
  PyraminxWidget w;
{
  int i, j, side, view;

  for (view = DOWN; view <= UP; view++)
    for (j = 0; j < w->pyraminx.size; j++)
      for (i = 0; i < w->pyraminx.size; i++)
        for (side = 0; side < MAXSIDES; side++)
          draw_triangle(w, view, i, j, side);
}

static void draw_triangle(w, view, i, j, side)
  PyraminxWidget w;
  int view, i, j, side;
{
  int dx, dy, face, orient;

  if (!w->pyraminx.vertical && view == DOWN) {
    dx = (w->pyraminx.size - i - 1) *
      (w->pyraminx.tetra_length + w->pyraminx.delta) + w->pyraminx.delta;
    dy = (w->pyraminx.size - j - 1) *
      (w->pyraminx.tetra_length + w->pyraminx.delta) + w->pyraminx.delta;
    face = !side;
    orient = (w->pyraminx.tetra_loc[view][i][j][side].rotation +
      MAXORIENT / 2) % MAXORIENT;
    if (2 * w->pyraminx.size - i - j - 2 + face >= w->pyraminx.size) {
      dx += w->pyraminx.side_offset;
      dy += w->pyraminx.side_offset;
    }
  } else {
    dx = i * (w->pyraminx.tetra_length + w->pyraminx.delta) + w->pyraminx.delta;
    dy = j * (w->pyraminx.tetra_length + w->pyraminx.delta) + w->pyraminx.delta;
    face = side;
    orient = w->pyraminx.tetra_loc[view][i][j][side].rotation;
    if (i + j + face >= w->pyraminx.size) /* face == DOWN */ {
      dx += w->pyraminx.side_offset;
      dy += w->pyraminx.side_offset;
    }
  }
  dx += w->pyraminx.puzzle_offset.x;
  dy += w->pyraminx.puzzle_offset.y;
  if (view == DOWN) {
    if (w->pyraminx.vertical)
      dy += w->pyraminx.view_length - w->pyraminx.delta - 1;
    else
      dx += w->pyraminx.view_length - w->pyraminx.delta - 1;
  }
  triangle_list[face][0].x = offset_list[face].x + dx;
  triangle_list[face][0].y = offset_list[face].y + dy;
  XFillPolygon(XtDisplay(w), XtWindow(w),
    w->pyraminx.face_GC[w->pyraminx.tetra_loc[view][i][j][side].face],
    triangle_list[face], 3, Convex, CoordModePrevious);
  if (w->pyraminx.depth == 1 || w->pyraminx.mono) {
    int letter_x, letter_y;
    char buf[2];

    (void) sprintf(buf, "%c",
      w->pyraminx.face_name[w->pyraminx.tetra_loc[view][i][j][side].face][0]);
    letter_x = dx + letter_list[face].x;
    letter_y = dy + letter_list[face].y;
    XDrawString(XtDisplay(w), XtWindow(w), w->pyraminx.inverse_GC,
      letter_x, letter_y, buf, 1);
  }
  if (w->pyraminx.orient)
    draw_orient_line(w, orient, dx, dy, face);
}

static void draw_orient_line(w, orient, dx, dy, side)
  PyraminxWidget w;
  int orient, dx, dy, side;
{
  int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
  int temp1 = w->pyraminx.tetra_length / 2 + 1;
  int temp2 = w->pyraminx.tetra_length + 1;
  int fix = (w->pyraminx.size == 1) ? 1 : 0;

  switch (orient) {
    case TOP:
      x2 = x1 = dx + temp1;
      y1 = (side == UP) ? dy + temp1 + 1 + fix: dy;
      y2 = y1 + w->pyraminx.orient_line_length;
      break;
    case TR:
      x1 = (side == UP) ? dx + temp2 + 2 : dx + temp1;
      x2 = x1 - w->pyraminx.orient_diag_length;
      y1 = (side == UP) ? dy + temp1 : dy - 1;
      y2 = y1 + w->pyraminx.orient_diag_length;
      break;
    case RIGHT:
      x1 = (side == UP) ? dx + temp2 + 1 : dx + temp1 - fix;
      x2 = x1 - w->pyraminx.orient_line_length;
      y2 = y1 = dy + temp1;
      break;
    case BOTTOM:
      x2 = x1 = dx + temp1;
      y1 = (side == UP) ? dy + temp2 + 1 : dy + temp1 - fix;
      y2 = y1 - w->pyraminx.orient_line_length;
      break;
    case BL:
      x1 = (side == UP) ? dx + temp1 : dx - 1;
      x2 = x1 + w->pyraminx.orient_diag_length;
      y1 = (side == UP) ? dy + temp2 + 1: dy + temp1;
      y2 = y1 - w->pyraminx.orient_diag_length;
      break;
    case LEFT:
      x1 = (side == UP) ? dx + temp1 + 1 + fix : dx;
      x2 = x1 + w->pyraminx.orient_line_length;
      y2 = y1 = dy + temp1;
      break;
    default:
      (void) printf("draw_orient_line: orient %d\n", orient);
  }
  XDrawLine(XtDisplay(w), XtWindow(w), w->pyraminx.inverse_GC, x1, y1, x2, y2);
}

Boolean check_solved(w)
  PyraminxWidget w;
{
  int view, side, face, i, j;
  PyraminxLoc test[MAXSIDES];

  for (view = 0; view < MAXVIEWS; view++) {
    for (side = 0; side < MAXSIDES; side++) {
      test[side].face = -1;
      test[side].rotation = -1;
    }
    for (j = 0; j < w->pyraminx.size; j++)
      for (i = 0; i < w->pyraminx.size; i++)
        for (side = 0; side < MAXSIDES; side++) {
          if (i + j < w->pyraminx.size - 1)
            face = DOWN;
          else if (i + j >= w->pyraminx.size)
            face = UP;
          else
            face = side;
          if (test[face].face == -1) {
            test[face].face =
              w->pyraminx.tetra_loc[view][i][j][side].face;
            test[face].rotation =
              w->pyraminx.tetra_loc[view][i][j][side].rotation;
          }
          else if (test[face].face != /*MAXSIDES * view + face*/
                   w->pyraminx.tetra_loc[view][i][j][side].face ||
                   (w->pyraminx.orient && test[face].rotation != /*TOP*/
                    w->pyraminx.tetra_loc[view][i][j][side].rotation))
            return FALSE;
        }
  }
  return TRUE;
}

static int rcd(dir, I, J, side)
  int dir, I, J, side;
{
  if (dir == TOP || dir == BOTTOM)
    return (I);
  else if (dir == RIGHT || dir == LEFT)
    return (J);
  else /* dir == TR || dir == BL */
    return (I + J + side);
}

static int length(w, face, dir, h)
  PyraminxWidget w;
  int face, dir, h;
{
  if (dir == TR || dir == BL) {
    if (face == DOWN)
      return (h);
    else /* face == UP */
      return (2 * w->pyraminx.size - 1 - h);
  } else {
    if (face == DOWN)
      return (w->pyraminx.size - 1 - h);
    else /* face == UP */
      return (h);
  }
}

#ifdef DEBUG

static void print_tetra(w)
  PyraminxWidget w;
{
  int g, h, view, side;

  for (view = DOWN; view <= UP; view++) {
    for (g = 0; g < w->pyraminx.size; g++) {
      for (h = 0; h < w->pyraminx.size; h++)
        for (side = 0; side < MAXSIDES; side++)
          (void) printf("%d %d  ", w->pyraminx.tetra_loc[!view][h][g][side].face,
            w->pyraminx.tetra_loc[!view][h][g][side].rotation);
      (void) printf("\n");
    }
    (void) printf("\n");
  }
  (void) printf("\n");
}

static void print_face(w)
  PyraminxWidget w;
{
  int g, h, side;

  for (g = 0; g < w->pyraminx.size; g++) {
    for (h = 0; h < w->pyraminx.size - g; h++)
      for (side = 0; side < MAXSIDES; side++)
        if (side == DOWN || h < w->pyraminx.size - g - 1)
          (void) printf("%d %d  ", w->pyraminx.face_loc[h][g][side].face,
            w->pyraminx.face_loc[h][g][side].rotation);
    (void) printf("\n");
  }
  (void) printf("\n");
}

static void print_row2(w, orient)
  PyraminxWidget w;
  int orient;
{
  int g, side;

  (void) printf ("Row %d:\n", orient);
  for (g = 0; g < w->pyraminx.size; g++)
    for (side = 0; side < MAXSIDES; side++)
      (void) printf("%d %d  ", w->pyraminx.row_loc_p2[orient][g][side].face,
              w->pyraminx.row_loc_p2[orient][g][side].rotation);
  (void) printf("\n");
}

static void print_row3(w, len, orient)
  PyraminxWidget w;
  int len, orient;
{
  int g, side;

  (void) printf("Row %d:\n", orient);
  for (g = 0; g <= len; g++)
    for (side = 0; side < MAXSIDES; side++)
      if (side == DOWN || g < len)
        (void) printf("%d %d  ", w->pyraminx.row_loc_p3[orient][g][side].face,
            w->pyraminx.row_loc_p3[orient][g][side].rotation);
  (void) printf("\n");
}

#endif
