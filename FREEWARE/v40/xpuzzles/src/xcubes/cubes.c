/*
# X-BASED CUBES
#
#  Cubes.c
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

/* Methods file for Cubes */

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
#include "CubesP.h"

#ifndef DATAFILE
#define DATAFILE "/usr/games/lib/cubes.data"
#endif

static void InitializeCubes();
static void ExposeCubes();
static void ResizeCubes();
static void DestroyCubes();
static Boolean SetValuesCubes();
static void quit_cubes();
static void move_cubes_out();
static void move_cubes_top();
static void move_cubes_left();
static void move_cubes_in();
static void move_cubes_right();
static void move_cubes_bottom();
static void select_cubes();
static void randomize_cubes();
static void maybe_randomize_cubes();
static void enter_cubes();
static void write_cubes();
static void undo_cubes();
static void solve_cubes();
static int move_cubes();
static void select_bricks();
static void check_bricks();
static void reset_bricks();
static void resize_bricks();
static void no_move_bricks();
static int move_bricks_dir();
static void randomize_bricks();
static void move_bricks();
static int exchange_bricks();
static void draw_frame();
static void draw_brick();
static int row();
static int column();
static int stack();

static char defaultTranslationsCubes[] =
  "<KeyPress>q: quit()\n\
   Ctrl<KeyPress>C: quit()\n\
   <KeyPress>o: move_out()\n\
   <KeyPress>KP_Divide: move_out()\n\
   <KeyPress>R5: move_out()\n\
   <KeyPress>Up: move_top()\n\
   <KeyPress>KP_8: move_top()\n\
   <KeyPress>R8: move_top()\n\
   <KeyPress>Left: move_left()\n\
   <KeyPress>KP_4: move_left()\n\
   <KeyPress>R10: move_left()\n\
   <KeyPress>i: move_in()\n\
   <KeyPress>Begin: move_in()\n\
   <KeyPress>KP_5: move_in()\n\
   <KeyPress>R11: move_in()\n\
   <KeyPress>Right: move_right()\n\
   <KeyPress>KP_6: move_right()\n\
   <KeyPress>R12: move_right()\n\
   <KeyPress>Down: move_bottom()\n\
   <KeyPress>KP_2: move_bottom()\n\
   <KeyPress>R14: move_bottom()\n\
   <Btn1Down>: select()\n\
   <Btn1Motion>: select()\n\
   <KeyPress>r: randomize()\n\
   <Btn3Down>(2+): randomize()\n\
   <Btn3Down>: maybe_randomize()\n\
   <KeyPress>e: enter()\n\
   <KeyPress>w: write()\n\
   <KeyPress>u: undo()\n\
   <KeyPress>s: solve()";
 
static XtActionsRec actionsListCubes[] =
{
  {"quit", (XtActionProc) quit_cubes},
  {"move_out", (XtActionProc) move_cubes_out},
  {"move_top", (XtActionProc) move_cubes_top},
  {"move_left", (XtActionProc) move_cubes_left},
  {"move_in", (XtActionProc) move_cubes_in},
  {"move_right", (XtActionProc) move_cubes_right},
  {"move_bottom", (XtActionProc) move_cubes_bottom},
  {"select", (XtActionProc) select_cubes},
  {"randomize", (XtActionProc) randomize_cubes},
  {"maybe_randomize", (XtActionProc) maybe_randomize_cubes},
  {"enter", (XtActionProc) enter_cubes},
  {"write", (XtActionProc) write_cubes},
  {"undo", (XtActionProc) undo_cubes},
  {"solve", (XtActionProc) solve_cubes}
};

static XtResource resourcesCubes[] =
{
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
   XtOffset(CubesWidget, cubes.foreground), XtRString, XtDefaultForeground},
  {XtNbrickColor, XtCColor, XtRPixel, sizeof(Pixel),
   XtOffset(CubesWidget, cubes.brick_color), XtRString, XtDefaultForeground},
  {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
   XtOffset(CubesWidget, core.width), XtRString, "100"},
  {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
   XtOffset(CubesWidget, core.height), XtRString, "300"},
  {XtNsizeX, XtCSizeX, XtRInt, sizeof(int),
   XtOffset(CubesWidget, cubes.size_x), XtRString, "3"}, /* DEFAULTCUBES */
  {XtNsizeY, XtCSizeY, XtRInt, sizeof(int),
   XtOffset(CubesWidget, cubes.size_y), XtRString, "3"}, /* DEFAULTCUBES */
  {XtNsizeZ, XtCSizeZ, XtRInt, sizeof(int),
   XtOffset(CubesWidget, cubes.size_z), XtRString, "3"}, /* DEFAULTCUBES */
  {XtNbase, XtCBase, XtRInt, sizeof(int),
   XtOffset(CubesWidget, cubes.base), XtRString, "10"},
  {XtNstart, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(CubesWidget, cubes.started), XtRString, "FALSE"},
  {XtNselectCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
   XtOffset(CubesWidget, cubes.select), XtRCallback, NULL}
};

CubesClassRec cubesClassRec =
{
  {
    (WidgetClass) &widgetClassRec,	/* superclass */
    "Cubes",				/* class name */
    sizeof(CubesRec),			/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeCubes,			/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    actionsListCubes,			/* actions */
    XtNumber(actionsListCubes),		/* num actions */
    resourcesCubes,			/* resources */
    XtNumber(resourcesCubes),		/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    DestroyCubes,			/* destroy */
    ResizeCubes,			/* resize */
    ExposeCubes,			/* expose */
    SetValuesCubes,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    NULL,				/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    defaultTranslationsCubes,		/* tm table */
    NULL,				/* query geometry */
    NULL,				/* display accelerator */
    NULL				/* extension */
  },
  {
    0					/* ignore */
  }
};

WidgetClass cubesWidgetClass = (WidgetClass) &cubesClassRec;

static void InitializeCubes(request, new)
  Widget request, new;
{
  CubesWidget w = (CubesWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  
  check_bricks(w);
  init_moves();
  reset_bricks(w);
  (void) SRAND(getpid());
  valueMask = GCForeground | GCBackground;
  values.foreground = w->cubes.foreground;
  values.background = w->core.background_pixel;
  w->cubes.puzzle_GC = XtGetGC(new, valueMask, &values);
  values.foreground = w->cubes.brick_color;
  w->cubes.brick_GC = XtGetGC(new, valueMask, &values);
  valueMask = GCForeground | GCBackground;
  values.foreground = w->core.background_pixel;
  values.background = w->cubes.foreground;
  w->cubes.inverse_GC = XtGetGC(new, valueMask, &values);
  ResizeCubes(w);
}

static void DestroyCubes(old)
  Widget old;
{
  CubesWidget w = (CubesWidget) old;

  XtReleaseGC(old, w->cubes.brick_GC);
  XtReleaseGC(old, w->cubes.puzzle_GC);
  XtReleaseGC(old, w->cubes.inverse_GC);
  XtRemoveCallbacks(old, XtNselectCallback, w->cubes.select);
}

static void ResizeCubes(w)
  CubesWidget w;
{
  w->cubes.delta.x = 3;
  w->cubes.delta.y = 3;
  w->cubes.wid = MAX(((int) w->core.width - w->cubes.delta.x) /
    w->cubes.size_x, 0);
  w->cubes.ht = MAX(((int) w->core.height - w->cubes.delta.y) /
    w->cubes.size_y, 0);
  if (w->cubes.ht >= w->cubes.wid) {
    w->cubes.vertical = TRUE;
    w->cubes.ht = MAX(((int) w->core.height / w->cubes.size_z -
      w->cubes.delta.y) / w->cubes.size_y, 0);
  } else {
    w->cubes.vertical = FALSE;
    w->cubes.wid = MAX(((int) w->core.width / w->cubes.size_z -
      w->cubes.delta.x) / w->cubes.size_x, 0);
  }
  w->cubes.face_width = w->cubes.wid * w->cubes.size_x +
    w->cubes.delta.x + 2;
  w->cubes.face_height = w->cubes.ht * w->cubes.size_y +
    w->cubes.delta.y + 2;
  if (w->cubes.vertical) {
    w->cubes.puzzle_width = w->cubes.face_width;
    w->cubes.puzzle_height = (w->cubes.face_height - w->cubes.delta.y) *
      w->cubes.size_z + w->cubes.delta.y;
  } else {
    w->cubes.puzzle_width = (w->cubes.face_width - w->cubes.delta.x) *
      w->cubes.size_z + w->cubes.delta.x;
    w->cubes.puzzle_height = w->cubes.face_height;
  }
  w->cubes.puzzle_offset.x = ((int) w->core.width -
    w->cubes.puzzle_width + 2) / 2;
  w->cubes.puzzle_offset.y = ((int) w->core.height -
    w->cubes.puzzle_height + 2) / 2;
  w->cubes.brick_width = MAX(w->cubes.wid - w->cubes.delta.x, 0);
  w->cubes.brick_height = MAX(w->cubes.ht - w->cubes.delta.y , 0);
  resize_bricks(w);
}

static void ExposeCubes(new, event, region)
  Widget new;
  XEvent *event;
  Region region; /* Not used */
{
  CubesWidget w = (CubesWidget) new;

  if (w->core.visible) {
    draw_frame(w, w->cubes.puzzle_GC);
    draw_all_bricks(w, w->cubes.brick_GC);
  }
}

static Boolean SetValuesCubes(current, request, new)
  Widget current, request, new;
{
  CubesWidget c = (CubesWidget) current, w = (CubesWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  Boolean redraw = FALSE;
  Boolean redraw_bricks = FALSE;

  check_bricks(w);
  if (w->cubes.foreground != c->cubes.foreground) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->cubes.foreground;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->cubes.puzzle_GC);
    w->cubes.puzzle_GC = XtGetGC(new, valueMask, &values);
    redraw_bricks = TRUE;
  }
  if (w->core.background_pixel != c->core.background_pixel) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->core.background_pixel;
    values.background = w->cubes.foreground;
    XtReleaseGC(new, w->cubes.inverse_GC);
    w->cubes.inverse_GC = XtGetGC(new, valueMask, &values);
    redraw_bricks = TRUE;
  }
  if (w->cubes.brick_color != c->cubes.brick_color) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->cubes.brick_color;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->cubes.brick_GC);
    w->cubes.brick_GC = XtGetGC(new, valueMask, &values);
    redraw_bricks = TRUE;
  }
  if (w->cubes.size_x != c->cubes.size_x ||
      w->cubes.size_y != c->cubes.size_y ||
      w->cubes.size_z != c->cubes.size_z ||
      w->cubes.base != c->cubes.base) {
    reset_bricks(w);
    ResizeCubes(w);
    redraw = TRUE;
  }
  else if (w->cubes.wid != c->cubes.wid ||
           w->cubes.ht != c->cubes.ht) {
    ResizeCubes(w);
    redraw = TRUE;
  }
  if (redraw_bricks && !redraw && XtIsRealized(new) && new->core.visible) {
    draw_frame(c, c->cubes.inverse_GC);
    draw_all_bricks(c, c->cubes.inverse_GC);
    draw_frame(w, w->cubes.puzzle_GC);
    draw_all_bricks(w, w->cubes.brick_GC);
  }
  return (redraw);
}

static void quit_cubes(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  XtCloseDisplay(XtDisplay(w));
  exit(0);
}

static void select_cubes(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  int i, j, k;

  if (w->cubes.vertical) {
    i = (event->xbutton.x - w->cubes.delta.x / 2 -
      w->cubes.puzzle_offset.x) / w->cubes.wid;
    j = ((event->xbutton.y - w->cubes.delta.y / 2 -
      w->cubes.puzzle_offset.y) % (w->cubes.size_y * w->cubes.ht +
      w->cubes.delta.y - 1)) / w->cubes.ht;
    k = (event->xbutton.y - w->cubes.delta.y / 2 -
      w->cubes.puzzle_offset.y) / (w->cubes.size_y * w->cubes.ht +
      w->cubes.delta.y - 1);
  } else {
    i = ((event->xbutton.x - w->cubes.delta.x / 2 -
      w->cubes.puzzle_offset.x) % (w->cubes.size_x * w->cubes.wid +
      w->cubes.delta.x - 1)) / w->cubes.wid;
    j = (event->xbutton.y - w->cubes.delta.y / 2 -
      w->cubes.puzzle_offset.y) / w->cubes.ht;
    k = (event->xbutton.x - w->cubes.delta.x / 2 -
      w->cubes.puzzle_offset.x) / (w->cubes.size_x * w->cubes.wid +
      w->cubes.delta.x - 1);
  }
  if (check_solved(w))
    no_move_bricks(w);
  else {
    select_bricks(w, i, j, k);
    if (check_solved(w)) {
      cubesCallbackStruct cb;

      cb.reason = CUBES_SOLVED;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}

static void randomize_cubes(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  randomize_bricks(w);
}
 
static void maybe_randomize_cubes(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->cubes.started)
    randomize_bricks(w);
}

static void enter_cubes(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;
  char c;
  int i, size_x, size_y, size_z, moves;
  cubesCallbackStruct cb;

  if ((fp = fopen(DATAFILE, "r")) == NULL)
    (void) printf("Can not read %s to enter.\n", DATAFILE);
  else {
    flush_moves(w);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &size_x);
    if (size_x >= MINCUBES && size_x <= MAXCUBES) {
      for (i = w->cubes.size_x; i < size_x; i++) {
        cb.reason = CUBES_INC_X;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
      for (i = w->cubes.size_x; i > size_x; i--) {
        cb.reason = CUBES_DEC_X;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    } else
      (void) printf("%s corrupted: size_x %d should be between %d and %d\n",
         DATAFILE, size_x, MINCUBES, MAXCUBES);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &size_y);
    if (size_y >= MINCUBES && size_y <= MAXCUBES) {
      for (i = w->cubes.size_y; i < size_y; i++) {
        cb.reason = CUBES_INC_Y;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
      for (i = w->cubes.size_y; i > size_y; i--) {
        cb.reason = CUBES_DEC_Y;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    } else
      (void) printf("%s corrupted: size_y %d should be between %d and %d\n",
         DATAFILE, size_y, MINCUBES, MAXCUBES);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &size_z);
    if (size_y >= MINCUBES && size_y <= MAXCUBES) {
      for (i = w->cubes.size_z; i < size_z; i++) {
        cb.reason = CUBES_INC_Z;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
      for (i = w->cubes.size_z; i > size_z; i--) {
        cb.reason = CUBES_DEC_Z;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    } else
      (void) printf("%s corrupted: size_z %d should be between %d and %d\n",
         DATAFILE, size_z, MINCUBES, MAXCUBES);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &moves);
    scan_start_position(fp, w);
    cb.reason = CUBES_RESTORE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    set_start_position(w);
    scan_moves(fp, w, moves);
    (void) fclose(fp);
    (void) printf("%s: size_x %d, size_y %d, size_z %d, moves %d.\n",
      DATAFILE, size_x, size_y, size_z, moves);
  }
}

static void write_cubes(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;

  if ((fp = fopen(DATAFILE, "w")) == NULL)
    (void) printf("Can not write to %s.\n", DATAFILE);
  else {
    (void) fprintf(fp, "size_x: %d\n", w->cubes.size_x);
    (void) fprintf(fp, "size_y: %d\n", w->cubes.size_y);
    (void) fprintf(fp, "size_z: %d\n", w->cubes.size_z);
    (void) fprintf(fp, "moves: %d\n", num_moves());
    print_start_position(fp, w);
    print_moves(fp);
    (void) fclose(fp);
    (void) printf("Saved to %s.\n", DATAFILE);
  }
}

static void undo_cubes(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (moves_made()) {
    int direction;

    get_move(&direction);
    direction = (direction < 4) ? (direction + 2) % 4 :
      ((direction == IN) ?  OUT : IN);

    if (move_bricks_dir(w, direction)) {
      cubesCallbackStruct cb;

      cb.reason = CUBES_UNDO;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}

static void solve_cubes(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  /* solve_bricks(w); */ /* Sorry, unimplemented */
}
 
static void move_cubes_out(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_cubes(w, OUT, (int) (event->xkey.state & ControlMask));
}

static void move_cubes_top(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_cubes(w, TOP, (int) (event->xkey.state & ControlMask));
}

static void move_cubes_left(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_cubes(w, LEFT, (int) (event->xkey.state & ControlMask));
}

static void move_cubes_in(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_cubes(w, IN, (int) (event->xkey.state & ControlMask));
}

static void move_cubes_right(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_cubes(w, RIGHT, (int) (event->xkey.state & ControlMask));
}

static void move_cubes_bottom(w, event, args, n_args)
  CubesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_cubes(w, BOTTOM, (int) (event->xkey.state & ControlMask));
}

static int move_cubes(w, direction, control)
  CubesWidget w;
  int direction, control;
{
  cubesCallbackStruct cb;

  if (control) {
    cb.reason = CUBES_IGNORE;
    switch (direction) {
      case TOP:
        if (w->cubes.size_y != MINCUBES)
          cb.reason = CUBES_DEC_Y;
        else
          return FALSE;
        break;
      case RIGHT:
        if (w->cubes.size_x != MAXCUBES)
          cb.reason = CUBES_INC_X;
        else
          return FALSE;
        break;
      case BOTTOM:
        if (w->cubes.size_y != MAXCUBES)
          cb.reason = CUBES_INC_Y;
        else
          return FALSE;
        break;
      case LEFT:
        if (w->cubes.size_x != MINCUBES)
          cb.reason = CUBES_DEC_X;
        else
          return FALSE;
        break;
      case IN:
        if (w->cubes.size_z != MINCUBES)
          cb.reason = CUBES_DEC_Z;
        else
          return FALSE;
        break;
      case OUT:
        if (w->cubes.size_z != MAXCUBES)
          cb.reason = CUBES_INC_Z;
        else
          return FALSE;
        break;
      default:
        (void) printf("move_cubes: direction %d\n", direction);
    }
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    return FALSE;
  }
  if (check_solved(w)) {
    no_move_bricks(w);
    return FALSE;
  }
  if (!move_cubes_dir(w, direction)) {
    cb.reason = CUBES_BLOCKED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    return FALSE;
  }
  if (check_solved(w)) {
    cb.reason = CUBES_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  return TRUE;
}

int move_cubes_dir(w, direction)
  CubesWidget w;
  int direction;
{
  cubesCallbackStruct cb;

  if (move_bricks_dir(w, direction)) {
    cb.reason = CUBES_MOVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    put_move(direction);
    return TRUE;
  }
  return FALSE;
}

static void select_bricks(w, i, j, k)
  CubesWidget w;
  int i, j, k;
{
  cubesCallbackStruct cb;
  int l, m, n;

  if (i >= 0 && j >= 0 && k >= 0 &&
      i < w->cubes.size_x && j < w->cubes.size_y && k < w->cubes.size_z) {
    l = i + w->cubes.size_x * j - w->cubes.space_position %
      w->cubes.size_rect;
    m = i + w->cubes.size_x * j + w->cubes.size_rect * k -
      w->cubes.space_position;
    /* Order important if w->cubes.size_x = 1 */
    if (l % w->cubes.size_x == 0 && k == stack(w, w->cubes.space_position)) {
      if (l < 0) {
        for (n = 1; n <= -l / w->cubes.size_x; n++) {
          move_bricks(w, w->cubes.space_position - w->cubes.size_x);
          cb.reason = CUBES_MOVED;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          put_move(BOTTOM);
        }
      } else if (l > 0) {
        for (n = 1; n <= l / w->cubes.size_x; n++) {
          move_bricks(w, w->cubes.space_position + w->cubes.size_x);
          cb.reason = CUBES_MOVED;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          put_move(TOP);
        }
      } else /* (l == 0) */ {
        cb.reason = CUBES_SPACE;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
        return;
      }
    } else if (l / w->cubes.size_x == 0 &&
                j == column(w, w->cubes.space_position) &&
               k == stack(w, w->cubes.space_position)) {
      if (l < 0) {
        for (n = 1; n <= -l % w->cubes.size_x; n++) {
          move_bricks(w, w->cubes.space_position - 1);
          cb.reason = CUBES_MOVED;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          put_move(RIGHT);
        }
      } else /* (l > 0) */ {
        for (n = 1; n <= l % w->cubes.size_x; n++) {
          move_bricks(w, w->cubes.space_position + 1);
          cb.reason = CUBES_MOVED;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          put_move(LEFT);
        }
      }
    } else if (m % w->cubes.size_rect == 0) {
      if (m < 0) {
        for (n = 1; n <= -stack(w, m); n++) {
          move_bricks(w, w->cubes.space_position - w->cubes.size_rect);
          cb.reason = CUBES_MOVED;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          put_move(IN);
        }
      } else /* (m > 0) */ {
        for (n = 1; n <= stack(w, m); n++) {
          move_bricks(w, w->cubes.space_position + w->cubes.size_rect);
          cb.reason = CUBES_MOVED;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          put_move(OUT);
        }
      }
    } else {
      cb.reason = CUBES_BLOCKED;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      return;
    }
  }
}

static void check_bricks(w)
  CubesWidget w;
{
  char buf[121];

  if (w->cubes.size_x < MINCUBES || w->cubes.size_x > MAXCUBES) {
    (void) sprintf(buf,
             "Number of Cubes in X direction out of bounds, use %d..%d",
             MINCUBES, MAXCUBES);
    XtWarning(buf);
    w->cubes.size_x = DEFAULTCUBES;
  }
  if (w->cubes.size_y < MINCUBES || w->cubes.size_y > MAXCUBES) {
    (void) sprintf(buf,
             "Number of Cubes in Y direction out of bounds, use %d..%d",
             MINCUBES, MAXCUBES);
    XtWarning(buf);
    w->cubes.size_y = DEFAULTCUBES;
  }
  if (w->cubes.size_z < MINCUBES || w->cubes.size_z > MAXCUBES) {
    (void) sprintf(buf,
             "Number of Cubes in Z direction out of bounds, use %d..%d",
             MINCUBES, MAXCUBES);
    XtWarning(buf);
    w->cubes.size_z = DEFAULTCUBES;
  }
}

static void reset_bricks(w)
  CubesWidget w;
{
  int i;

  w->cubes.size_rect = w->cubes.size_x * w->cubes.size_y; 
  w->cubes.size_size = w->cubes.size_rect * w->cubes.size_z; 
  w->cubes.space_position = w->cubes.size_size - 1;
  w->cubes.brick_of_position[w->cubes.size_size - 1] = 0;
  for (i = 1; i < w->cubes.size_size; i++)
    w->cubes.brick_of_position[i - 1] = i;
  flush_moves(w);
  w->cubes.started = FALSE;
}

static void resize_bricks(w)
  CubesWidget w;
{
  w->cubes.digit_offset.x = 3;
  w->cubes.digit_offset.y = 4;
}

static void no_move_bricks(w)
  CubesWidget w;
{
  cubesCallbackStruct cb;
 
  cb.reason = CUBES_IGNORE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static int move_bricks_dir(w, direction)
  CubesWidget w;
  int direction;
{
  switch (direction) {
    case TOP:
      if (column(w, w->cubes.space_position) < w->cubes.size_y - 1) {
        move_bricks(w, w->cubes.space_position + w->cubes.size_x);
        return TRUE;
      }
      break;
    case RIGHT:
      if (row(w, w->cubes.space_position) > 0) {
        move_bricks(w, w->cubes.space_position - 1);
        return TRUE;
      }
      break;
    case BOTTOM:
      if (column(w, w->cubes.space_position) > 0) {
        move_bricks(w, w->cubes.space_position - w->cubes.size_x);
        return TRUE;
      }
      break;
    case LEFT:
      if (row(w, w->cubes.space_position) < w->cubes.size_x - 1) {
        move_bricks(w, w->cubes.space_position + 1);
        return TRUE;
      }
      break;
    case IN:
      if (stack(w, w->cubes.space_position) > 0) {
        move_bricks(w, w->cubes.space_position - w->cubes.size_rect);
        return TRUE;
      }
      break;
    case OUT:
      if (stack(w, w->cubes.space_position) < w->cubes.size_z - 1) {
        move_bricks(w, w->cubes.space_position + w->cubes.size_rect);
        return TRUE;
      }
      break;
    default:
      (void) printf("move_bricks_dir: direction %d\n", direction);
  }
  return FALSE;
}
 
static void randomize_bricks(w)
  CubesWidget w;
{
  cubesCallbackStruct cb;

  /* First interchange bricks an even number of times */
  if (w->cubes.size_x > 1 && w->cubes.size_y > 1 && w->cubes.size_z > 1 &&
      w->cubes.size_size > 4) {
    int pos, count = 0;

    for (pos = 0; pos < w->cubes.size_size; pos++) {
       int random_pos = pos;

       while (random_pos == pos)
         random_pos = NRAND(w->cubes.size_size);
       count += exchange_bricks(w, pos, random_pos);
    }
    if (count % 2)
      if (!exchange_bricks(w, 0, 1))
        if (!exchange_bricks(w, w->cubes.size_size - 2, w->cubes.size_size - 1))
          (void) printf("randomize_bricks: should not get here\n");
    draw_all_bricks(w, w->cubes.brick_GC);
  }
  /* Now move the space around randomly */
  if (w->cubes.size_x > 1 || w->cubes.size_y > 1 || w->cubes.size_z > 1) {
    int big = w->cubes.size_size + NRAND(2);
    int last_direction = 0;
    int random_direction;

    cb.reason = CUBES_RESET;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
 
#ifdef DEBUG
    big = 3;
#endif

    while (big--) {
      random_direction = NRAND(COORD);

#ifdef DEBUG
      sleep(1);
#endif

      if ((random_direction + COORD / 2) % COORD != last_direction ||
          (w->cubes.size_x == 1 && w->cubes.size_y == 1) ||
          (w->cubes.size_y == 1 && w->cubes.size_z == 1) ||
          (w->cubes.size_z == 1 && w->cubes.size_x == 1)) {
        if (move_cubes_dir(w, random_direction))
          last_direction = random_direction;
        else
          big++;
      }
    }
    flush_moves(w);
    cb.reason = CUBES_RANDOMIZE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  if (check_solved(w)) {
    cb.reason = CUBES_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void move_bricks(w, from)
  CubesWidget w;
  int from;
{
  int temp_brick;

  temp_brick = w->cubes.brick_of_position[from];
  w->cubes.brick_of_position[from] =
    w->cubes.brick_of_position[w->cubes.space_position];
  w->cubes.brick_of_position[w->cubes.space_position] = temp_brick;
  draw_brick(w, w->cubes.brick_GC, w->cubes.space_position);
  w->cubes.space_position = from;
  draw_brick(w, w->cubes.inverse_GC, w->cubes.space_position);
}

static int exchange_bricks(w, pos1, pos2)
  CubesWidget w;
  int pos1, pos2;
{
  int temp_brick;

  if (w->cubes.brick_of_position[pos1] <= 0)
    return FALSE;
  else if (w->cubes.brick_of_position[pos2] <= 0)
    return FALSE;
  temp_brick = w->cubes.brick_of_position[pos1];
  w->cubes.brick_of_position[pos1] = w->cubes.brick_of_position[pos2];
  w->cubes.brick_of_position[pos2] = temp_brick;
  return TRUE;
}

static void draw_frame(w, gc)
  CubesWidget w;
  GC gc;
{
  int sum_x, sum_y, sum_z, offset_x, offset_y, dz, k;

  sum_x = w->cubes.size_x * w->cubes.wid + w->cubes.delta.x / 2 + 1;
  sum_y = w->cubes.size_y * w->cubes.ht + w->cubes.delta.y / 2 + 1;
  offset_x = w->cubes.puzzle_offset.x;
  offset_y = w->cubes.puzzle_offset.y;
  if (w->cubes.vertical) {
    sum_z = offset_y;
    dz = sum_y;
  } else {
    sum_z = offset_x;
    dz = sum_x;
  }
  for (k = 0; k < w->cubes.size_z; k++)
  {
    if (w->cubes.vertical) {
      XFillRectangle(XtDisplay(w), XtWindow(w), gc, 
        offset_x, sum_z, 1, sum_y);
      XFillRectangle(XtDisplay(w), XtWindow(w), gc,
        offset_x, sum_z, sum_x, 1);
      XFillRectangle(XtDisplay(w), XtWindow(w), gc,
        sum_x + offset_x, sum_z, 1, sum_y + 1);
      XFillRectangle(XtDisplay(w), XtWindow(w), gc,
        offset_x, sum_y + sum_z, sum_x + 1, 1);
    } else {
      XFillRectangle(XtDisplay(w), XtWindow(w), gc, 
        sum_z, offset_y, 1, sum_y);
      XFillRectangle(XtDisplay(w), XtWindow(w), gc,
        sum_z, offset_y, sum_x, 1);
      XFillRectangle(XtDisplay(w), XtWindow(w), gc,
        sum_x + sum_z, offset_y, 1, sum_y + 1);
      XFillRectangle(XtDisplay(w), XtWindow(w), gc,
        sum_z, sum_y + offset_y, sum_x + 1, 1);
    }
    sum_z += dz;
  }
}   

void draw_all_bricks(w, gc)
  CubesWidget w;
  GC gc;
{
  int k;

  for (k = 0; k < w->cubes.size_size; k++)
    draw_brick(w, (w->cubes.brick_of_position[k] <= 0) ?
      w->cubes.inverse_GC : gc, k);
}

static void draw_brick(w, gc, pos)
  CubesWidget w;
  GC gc;
  int pos;
{
  int dx, dy, dz;

  if (w->cubes.vertical) {
    dx = row(w, pos) * w->cubes.wid + w->cubes.delta.x +
      w->cubes.puzzle_offset.x;
    dy = column(w, pos) * w->cubes.ht + w->cubes.delta.y +
      w->cubes.puzzle_offset.y;
    dz = stack(w, pos) * (w->cubes.size_y * w->cubes.ht + w->cubes.delta.y -
      1);
    dy += dz;
  } else {
    dx = row(w, pos) * w->cubes.wid + w->cubes.delta.x +
      w->cubes.puzzle_offset.x;
    dy = column(w, pos) * w->cubes.ht + w->cubes.delta.y +
      w->cubes.puzzle_offset.y;
    dz = stack(w, pos) * (w->cubes.size_x * w->cubes.wid + w->cubes.delta.x -
      1);
    dx += dz;
  }
  XFillRectangle(XtDisplay(w), XtWindow(w), gc, dx, dy,
    w->cubes.brick_width, w->cubes.brick_height);
  if (gc != w->cubes.inverse_GC) {
    int i = 0, offset = 0, brick = w->cubes.brick_of_position[pos];
    char buf[5];

    (void) sprintf(buf, "%d", brick);
    while (brick >= 1) {
      brick /= w->cubes.base;
      offset += w->cubes.digit_offset.x;
      i++;
    }
    XDrawString(XtDisplay(w), XtWindow(w), w->cubes.inverse_GC,
      dx + w->cubes.brick_width / 2 - offset,
      dy + w->cubes.brick_height / 2 + w->cubes.digit_offset.y, buf, i);
  }
}

static int row(w, pos)
  CubesWidget w;
  int pos;
{
  return ((pos % w->cubes.size_rect) % w->cubes.size_x);
}

static int column(w, pos)
  CubesWidget w;
  int pos;
{
  return ((pos % w->cubes.size_rect) / w->cubes.size_x);
}

static int stack(w, pos)
  CubesWidget w;
  int pos;
{
  return (pos / w->cubes.size_rect);
}

Boolean check_solved(w)
  CubesWidget w;
{
  int i;

  for (i = 1; i < w->cubes.size_size; i++)
    if (w->cubes.brick_of_position[i - 1] != i)
      return FALSE;
  return TRUE;
}
