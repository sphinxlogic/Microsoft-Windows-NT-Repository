/*
# X-BASED RUBIK'S CUBE(tm)
#
#  Rubik.c
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

/* Methods file for Rubik */

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
#include "RubikP.h"
#include "Rubik2dP.h"
#include "Rubik3dP.h"

#ifndef DATAFILE
#define DATAFILE "/usr/games/lib/rubik.data"
#endif

static void InitializeRubik();
static void DestroyRubik();
static Boolean SetValuesRubik();
static void get_color();
static void move_control_cb();
static void check_polyhedrons();
static void no_move_polyhedrons();
static void practice_polyhedrons();
static void randomize_polyhedrons();
static void move_polyhedrons();
/* rc : row or column */
static void read_rc();
static void rotate_rc();
static void reverse_rc();
static void write_rc();
static void rotate_face();
static void draw_square();

RubikClassRec rubikClassRec =
{
  {
    (WidgetClass) &widgetClassRec,	/* superclass */
    "Rubik",				/* class name */
    sizeof(RubikRec),			/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeRubik,			/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    NULL,				/* actions */
    0,					/* num actions */
    NULL,				/* resources */
    0,					/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    DestroyRubik,			/* destroy */
    NULL,				/* resize */
    NULL,				/* expose */
    SetValuesRubik,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    NULL,				/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    NULL,				/* tm table */
    NULL,				/* query geometry */
    NULL,				/* display accelerator */
    NULL				/* extension */
  },
  {
    0					/* ignore */
  }
};

WidgetClass rubikWidgetClass = (WidgetClass) &rubikClassRec;

static RubikLoc slide_next_row[MAXFACES][MAXORIENT] =
{
  {{5, STRT}, {3,   CW}, {2, STRT}, {1,  CCW}},
  {{0,   CW}, {2, STRT}, {4,  CCW}, {5, HALF}},
  {{0, STRT}, {3, STRT}, {4, STRT}, {1, STRT}},
  {{0,  CCW}, {5, HALF}, {4,   CW}, {2, STRT}},
  {{2, STRT}, {3,  CCW}, {5, STRT}, {1,   CW}},
  {{4, STRT}, {3, HALF}, {0, STRT}, {1, HALF}}
};
static int row_to_rotate[MAXFACES][MAXORIENT] =
{
  {3, 2, 1, 5},
  {2, 4, 5, 0},
  {3, 4, 1, 0},
  {5, 4, 2, 0},
  {3, 5, 1, 2},
  {3, 0, 1, 4}
};

static void InitializeRubik(request, new)
  Widget request, new;
{
  RubikWidget w = (RubikWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  int face;

  check_polyhedrons(w);
  init_moves();
  reset_polyhedrons(w);
  w->rubik.practice = FALSE;
  (void) SRAND(getpid());
  valueMask = GCForeground | GCBackground;
  values.foreground = w->rubik.foreground;
  values.background = w->core.background_pixel;
  w->rubik.puzzle_GC = XtGetGC(new, valueMask, &values);
  w->rubik.depth = DefaultDepthOfScreen(XtScreen(w));
  valueMask = GCForeground | GCBackground;
  values.foreground = w->core.background_pixel;
  values.background = w->rubik.foreground;
  w->rubik.inverse_GC = XtGetGC(new, valueMask, &values);
  for (face = 0; face < MAXFACES; face++)
    get_color(w, face, TRUE);
}

static void DestroyRubik(old)
  Widget old;
{
  RubikWidget w = (RubikWidget) old;
  int face;

  for (face = 0; face < MAXFACES; face++)
    XtReleaseGC(old, w->rubik.face_GC[face]);
  XtReleaseGC(old, w->rubik.puzzle_GC);
  XtReleaseGC(old, w->rubik.inverse_GC);
  XtRemoveCallbacks(old, XtNselectCallback, w->rubik.select);
}

static Boolean SetValuesRubik(current, request, new)
  Widget current, request, new;
{
  RubikWidget c = (RubikWidget) current, w = (RubikWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  Boolean redraw = FALSE;
  int face;

  check_polyhedrons(w);
  if (w->rubik.foreground != c->rubik.foreground) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->rubik.foreground;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->rubik.puzzle_GC);
    w->rubik.puzzle_GC = XtGetGC(new, valueMask, &values);
  }
  if (w->core.background_pixel != c->core.background_pixel) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->core.background_pixel;
    values.background = w->rubik.foreground;
    XtReleaseGC(new, w->rubik.inverse_GC);
    w->rubik.inverse_GC = XtGetGC(new, valueMask, &values);
    redraw = TRUE;
  }
  if (w->rubik.mono || w->rubik.depth == 1) {
    valueMask = GCForeground | GCBackground;
    values.background = w->core.background_pixel;
    values.foreground = w->rubik.foreground;
    for (face = 0; face < MAXFACES; face++) {
      XtReleaseGC(new, w->rubik.face_GC[face]);
      w->rubik.face_GC[face] = XtGetGC(new, valueMask, &values);
    }
    redraw = TRUE;
  }
  for (face = 0; face < MAXFACES; face++) {
    if (strcmp(w->rubik.face_name[face], c->rubik.face_name[face]))
      get_color(w, face, FALSE);
  }
  if (w->rubik.orient != c->rubik.orient) {
    reset_polyhedrons(w);
    w->rubik.practice = FALSE;
    redraw = TRUE;
  } else if (w->rubik.practice != c->rubik.practice) {
    reset_polyhedrons(w);
    redraw = TRUE;
  }
  if (w->rubik.face == RUBIK_RESTORE) {
    set_start_position(w);
    w->rubik.face = RUBIK_IGNORE;
  } else if (w->rubik.face != RUBIK_IGNORE) {
    move_polyhedrons(w, w->rubik.face, w->rubik.i, w->rubik.j,
      w->rubik.direction);
    w->rubik.face = RUBIK_IGNORE;
  }
  return redraw;
}

void quit_rubik(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  XtCloseDisplay(XtDisplay(w));
  exit(0);
}

void practice_rubik(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  practice_polyhedrons(w);
}

void maybe_practice_rubik(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->rubik.started)
    practice_polyhedrons(w);
}

void randomize_rubik(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  randomize_polyhedrons(w);
}

void maybe_randomize_rubik(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->rubik.started)
    randomize_polyhedrons(w);
}

void enter_rubik(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;
  char c;
  int i, size, orient, practice, moves;
  rubikCallbackStruct cb;

  if ((fp = fopen(DATAFILE, "r")) == NULL)
    (void) printf("Can not read %s to enter.\n", DATAFILE);
  else {
    flush_moves(w);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &size);
    if (size >= MINCUBES && size <= MAXCUBES) {
      for (i = w->rubik.size; i < size; i++) {
        cb.reason = RUBIK_INC;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }  
      for (i = w->rubik.size; i > size; i--) {
        cb.reason = RUBIK_DEC;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    } else
      (void) printf("%s corrupted: size %d should be between %d and %d\n",
         DATAFILE, size, MINCUBES, MAXCUBES);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &orient);
    if (w->rubik.orient != (Boolean) orient) {
      cb.reason = RUBIK_ORIENT;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &practice);
    if (w->rubik.practice != (Boolean) practice) {
      cb.reason = RUBIK_PRACTICE;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &moves);
    scan_start_position(fp, w);
    cb.reason = RUBIK_RESTORE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    scan_moves(fp, w, moves);
    (void) fclose(fp);
    (void) printf("%s: size %d, orient %d, practice %d, moves %d.\n",
      DATAFILE, size, orient, practice, moves);
  }
}

void write_rubik(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;

  if ((fp = fopen(DATAFILE, "w")) == NULL)
    (void) printf("Can not write to %s.\n", DATAFILE);
  else {
    (void) fprintf(fp, "size: %d\n", w->rubik.size);
    (void) fprintf(fp, "orient: %d\n", (w->rubik.orient) ? 1 : 0);
    (void) fprintf(fp, "practice: %d\n", (w->rubik.practice) ? 1 : 0);
    (void) fprintf(fp, "moves: %d\n", num_moves());
    print_start_position(fp, w);
    print_moves(fp);
    (void) fclose(fp);
    (void) printf("Saved to %s.\n", DATAFILE);
  }
}

void undo_rubik(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (moves_made()) {
    int face, i, j, direction, control;

    get_move(&face, &i, &j, &direction, &control);
    direction = (direction < MAXORIENT) ? (direction + MAXORIENT / 2) %
      MAXORIENT : 3 * MAXORIENT - direction;
    if (control)
      move_control_cb(w, face, direction);
    else {
      rubikCallbackStruct cb;

      move_polyhedrons(w, face, i, j, direction);
      cb.reason = RUBIK_UNDO;
      cb.face = face;
      cb.i = i;
      cb.j = j;
      cb.direction = direction;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}

void solve_rubik(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (w->rubik.size <= 2 || (w->rubik.size == 3 && !w->rubik.orient))
    solve_polyhedrons(w);
}

void increment_rubik(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  rubikCallbackStruct cb;

  if (w->rubik.size == MAXCUBES)
    return;
  cb.reason = RUBIK_INC;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

void decrement_rubik(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  rubikCallbackStruct cb;

  if (w->rubik.size == MINCUBES)
    return;
  cb.reason = RUBIK_DEC;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

void orientize_rubik(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  rubikCallbackStruct cb;

  cb.reason = RUBIK_ORIENT;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

void move_rubik_ccw(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input(w, event->xbutton.x, event->xbutton.y, CCW,
    (int) (event->xbutton.state & ControlMask));
}

void move_rubik_cw(w, event, args, n_args)
  RubikWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input(w, event->xbutton.x, event->xbutton.y, CW,
    (int) (event->xkey.state & ControlMask));
}

void move_rubik_input(w, x, y, direction, control)
  RubikWidget w;
  int x, y, direction, control;
{
  int face, i, j;

  if (!w->rubik.practice && !control && check_solved(w)) {
    no_move_polyhedrons(w);
    return;
  }
  if ((w->rubik.dim == 2 && !position_polyhedrons2d((Rubik2DWidget) w,
         x, y, &face, &i, &j, &direction)) ||
      (w->rubik.dim == 3 && !position_polyhedrons3d((Rubik3DWidget) w,
         x, y, &face, &i, &j, &direction)))
    return;
  control = (control) ? 1 : 0;
  move_rubik(w, face, i, j, direction, control);
  if (!control && check_solved(w)) {
    rubikCallbackStruct cb;

    cb.reason = RUBIK_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

void move_rubik(w, face, i, j, direction, control)
  RubikWidget w;
  int face, i, j, direction, control;
{
  if (control)
    move_control_cb(w, face, direction);
  else {
    rubikCallbackStruct cb;

    move_polyhedrons(w, face, i, j, direction);
    cb.reason = RUBIK_MOVED;
    cb.face = face;
    cb.i = i;
    cb.j = j;
    cb.direction = direction;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  put_move(face, i, j, direction, control);
}

static void get_color(w, face, init)
  RubikWidget w;
  int face, init;
{
  XGCValues values;
  XtGCMask valueMask;
  XColor color_cell, rgb;
 
  valueMask = GCForeground | GCBackground;
  values.background = w->core.background_pixel;
  if (w->rubik.depth > 1 && !w->rubik.mono) {
    if (XAllocNamedColor(XtDisplay(w),
        DefaultColormap(XtDisplay(w), XtWindow(w)),
        w->rubik.face_name[face], &color_cell, &rgb)) {
      values.foreground = w->rubik.face_color[face] = color_cell.pixel;
      if (!init)
        XtReleaseGC((Widget) w, w->rubik.face_GC[face]);
      w->rubik.face_GC[face] = XtGetGC((Widget) w, valueMask, &values);
      return;
    } else {
      char buf[121];

      (void) sprintf(buf, "Color name \"%s\" is not defined",
               w->rubik.face_name[face]);
      XtWarning(buf);
    }
  }
  values.foreground = w->rubik.foreground;
  if (!init)
    XtReleaseGC((Widget) w, w->rubik.face_GC[face]);
  w->rubik.face_GC[face] = XtGetGC((Widget) w, valueMask, &values);
}

static void move_control_cb(w, face, direction)
  RubikWidget w;
  int face, direction;
{
  rubikCallbackStruct cb;
  int k;

  for (k = 0; k < w->rubik.size; k++) {
    move_polyhedrons(w, face, k, k, direction);
    cb.reason = RUBIK_CONTROL;
    cb.face = face;
    cb.i = k;
    cb.j = k;
    cb.direction = direction;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void check_polyhedrons(w)
  RubikWidget w;
{
  if (w->rubik.size < MINCUBES || w->rubik.size > MAXCUBES) {
    char buf[121];

    (void) sprintf(buf, "Number of Cubes on edge out of bounds, use %d..%d",
             MINCUBES, MAXCUBES);
    XtWarning(buf);
    w->rubik.size = DEFAULTCUBES;
  }
}

void reset_polyhedrons(w)
  RubikWidget w;
{
  int face, i, j;

  w->rubik.size_size = w->rubik.size * w->rubik.size;
  for (face = 0; face < MAXFACES; face++)
    for (i = 0; i < w->rubik.size; i++)
      for (j = 0; j < w->rubik.size; j++) {
        w->rubik.cube_loc[face][i][j].face = face; 
        w->rubik.cube_loc[face][i][j].rotation = STRT - MAXORIENT;
      }
  flush_moves(w);
  w->rubik.started = FALSE;
}

static void no_move_polyhedrons(w)
  RubikWidget w;
{
  rubikCallbackStruct cb;

  cb.reason = RUBIK_IGNORE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void practice_polyhedrons(w)
  RubikWidget w;
{
  rubikCallbackStruct cb;

  cb.reason = RUBIK_PRACTICE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void randomize_polyhedrons(w)
  RubikWidget w;
{
  rubikCallbackStruct cb;
  int face, i, j, direction;
  int big = w->rubik.size_size * 3 + NRAND(2);

  if (w->rubik.practice)
    practice_polyhedrons(w);
  cb.reason = RUBIK_RESET;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);

#ifdef DEBUG
  big = 3;
#endif

  while (big--) {
    face = NRAND(MAXFACES);
    i = NRAND(w->rubik.size);
    j = NRAND(w->rubik.size);
    direction = NRAND(MAXORIENT);
    move_rubik(w, face, i, j, direction, FALSE);
  }
  flush_moves(w);
  cb.reason = RUBIK_RANDOMIZE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  if (check_solved(w)) {
    cb.reason = RUBIK_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void move_polyhedrons(w, face, i, j, direction)
  RubikWidget w;
  int face, i, j, direction;
{
  int new_face, new_direction, rotate, reverse = FALSE, h, k, new_h = 0;

  h = (direction == TOP || direction == BOTTOM) ? i : j;
  /* rotate sides CW or CCW */
  if (h == w->rubik.size - 1) {
    new_direction = (direction == TOP || direction == BOTTOM) ?
      TOP : RIGHT;
    if (direction == TOP || direction == RIGHT)
      rotate_face(w, row_to_rotate[face][new_direction], CW);
    else /* direction == BOTTOM || direction == LEFT */
      rotate_face(w, row_to_rotate[face][new_direction], CCW);
  }
  if (h == 0) {
    new_direction = (direction == TOP || direction == BOTTOM) ?
      BOTTOM : LEFT;
    if (direction == TOP || direction == RIGHT)
      rotate_face(w, row_to_rotate[face][new_direction], CCW);
    else /* direction == BOTTOM  || direction == LEFT */
      rotate_face(w, row_to_rotate[face][new_direction], CW);
  }
  /* Slide rows */
  read_rc(w, face, direction, h, 0);
  for (k = 1; k <= MAXORIENT; k++) {
    new_face = slide_next_row[face][direction].face;
    rotate = slide_next_row[face][direction].rotation;
    new_direction = (rotate + direction) % MAXORIENT;
    switch (rotate) {
      case STRT:
        new_h = h;
        reverse = FALSE;
        break;
      case CW:
        if (new_direction == TOP || new_direction == BOTTOM) {
          new_h = w->rubik.size - 1 - h;
          reverse = FALSE;
        } else /* new_direction == RIGHT || new_direction == LEFT */ {
          new_h = h;
          reverse = TRUE;
        }
        break;
      case HALF:
        new_h = w->rubik.size - 1 - h;
        reverse = TRUE;
        break;
      case CCW:
        if (new_direction == TOP || new_direction == BOTTOM) {
          new_h = h;
          reverse = TRUE;
        } else /* new_direction == RIGHT || new_direction == LEFT */ {
          new_h = w->rubik.size - 1 - h;
          reverse = FALSE;
        }
        break;
      default:
        (void) printf ("move_polyhedrons: rotate %d\n", rotate);
    }
    if (k != MAXORIENT)
      read_rc(w, new_face, new_direction, new_h, k);
    rotate_rc(w, rotate, k - 1);
    if (reverse == TRUE)
      reverse_rc(w, k - 1);
    write_rc(w, new_face, new_direction, new_h, k - 1);
    face = new_face;
    direction = new_direction;
    h = new_h;
  }
}

static void read_rc(w, face, dir, h, orient)
  RubikWidget w;
  int face, dir, h, orient;
{
  int g;

  if (dir == TOP || dir == BOTTOM)
    for (g = 0; g < w->rubik.size; g++)
      w->rubik.row_loc[orient][g] = w->rubik.cube_loc[face][h][g];
  else /* dir == RIGHT || dir == LEFT */
    for (g = 0; g < w->rubik.size; g++)
      w->rubik.row_loc[orient][g] = w->rubik.cube_loc[face][g][h];
}

static void rotate_rc(w, rotate, orient)
  RubikWidget w;
  int rotate, orient;
{
  int g;

  for (g = 0; g < w->rubik.size; g++)
    w->rubik.row_loc[orient][g].rotation =
      (w->rubik.row_loc[orient][g].rotation + rotate) % MAXORIENT;
}

static void reverse_rc(w, orient)
  RubikWidget w;
  int orient;
{
  int g;
  RubikLoc Temp[MAXCUBES_SQ];

  for (g = 0; g < w->rubik.size; g++)
    Temp[g] = w->rubik.row_loc[orient][g];
  for (g = 0; g < w->rubik.size; g++)
    w->rubik.row_loc[orient][g] = Temp[w->rubik.size - 1 - g];
}

static void write_rc(w, face, dir, h, orient)
  RubikWidget w;
  int face, dir, h, orient;
{
  int g;
 
  if (dir == TOP || dir == BOTTOM) {
    for (g = 0; g < w->rubik.size; g++) {
      w->rubik.cube_loc[face][h][g] = w->rubik.row_loc[orient][g];
      draw_square(w, face, h, g);
    }
  } else /* dir == RIGHT || dir == LEFT */ {
    for (g = 0; g < w->rubik.size; g++) {
      w->rubik.cube_loc[face][g][h] = w->rubik.row_loc[orient][g];
      draw_square(w, face, g, h);
    }
  }
}

static void rotate_face(w, face, direction)
  RubikWidget w;
  int face, direction;
{
  int i, j;
 
  /* Read Face */
  for (j = 0; j < w->rubik.size; j++)
    for (i = 0; i < w->rubik.size; i++)
      w->rubik.face_loc[i][j] = w->rubik.cube_loc[face][i][j];
  /* Write Face */
  for (j = 0; j < w->rubik.size; j++)
    for (i = 0; i < w->rubik.size; i++) {
      w->rubik.cube_loc[face][i][j] = (direction == CW) ?
        w->rubik.face_loc[j][w->rubik.size - i - 1]:
        w->rubik.face_loc[w->rubik.size - j - 1][i];
      w->rubik.cube_loc[face][i][j].rotation =
        (w->rubik.cube_loc[face][i][j].rotation + direction - MAXORIENT) %
        MAXORIENT;
      draw_square(w, face, i, j);
    }
}

void draw_all_polyhedrons(w)
  RubikWidget w;
{
  int face, i, j;
 
  for (face = 0; face < MAXFACES; face++)
    for (j = 0; j < w->rubik.size; j++)
      for (i = 0; i < w->rubik.size; i++)
        draw_square(w, face, i, j);
}

static void draw_square(w, face, i, j)
  RubikWidget w;
  int face, i, j;
{
  if (w->rubik.dim == 2)
    draw_square2d((Rubik2DWidget) w, face, i, j);
  else if (w->rubik.dim == 3)
    draw_square3d((Rubik3DWidget) w, face, i, j);
}

Boolean check_solved(w)
  RubikWidget w;
{
  int face, i, j;
  RubikLoc test;

  for (face = 0; face < MAXFACES; face++)
    for (j = 0; j < w->rubik.size; j++)
      for (i = 0; i < w->rubik.size; i++) {
        if (i + j == 0) {
          test.face =
            w->rubik.cube_loc[face][i][j].face;
          test.rotation =
            w->rubik.cube_loc[face][i][j].rotation;
        } else if (test.face != /*face*/
                 w->rubik.cube_loc[face][i][j].face ||
                 (w->rubik.orient && test.rotation != /*STRT - MAXORIENT*/
                  w->rubik.cube_loc[face][i][j].rotation))
          return FALSE;
      }
  return TRUE;
}


#ifdef DEBUG

void print_cube(w)
  RubikWidget w;
{
  int face, i, j;

  for (face = 0; face < MAXFACES; face++) {
    for (j = 0; j < w->rubik.size; j++) {
      for (i = 0; i < w->rubik.size; i++) {
         (void) printf("%d %d  ", w->rubik.cube_loc[face][i][j].face,
                  w->rubik.cube_loc[face][i][j].rotation);
      }
      (void) printf("\n");
    }
    (void) printf("\n");
  }
  (void) printf("\n");
}

void print_face(w)
  RubikWidget w;
{
  int i, j;

  for (j = 0; j < w->rubik.size; j++) {
    for (i = 0; i < w->rubik.size; i++) {
       (void) printf("%d %d  ", w->rubik.face_loc[i][j].face,
                w->rubik.face_loc[i][j].rotation);
     }
    (void) printf("\n");
  }
  (void) printf("\n");
}

void print_row(w, orient)
  RubikWidget w;
  int orient;
{
  int i;

  (void) printf("Row %d:\n", orient);
  for (i = 0; i < w->rubik.size; i++)
    (void) printf("%d %d  ", w->rubik.row_loc[orient][i].face,
             w->rubik.row_loc[orient][i].rotation);
  (void) printf("\n");
}

#endif
