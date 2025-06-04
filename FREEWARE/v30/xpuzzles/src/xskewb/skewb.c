/*
# X-BASED SKEWB
#
#  Skewb.c
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

/* Methods file for Skewb */

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
#include "SkewbP.h"
#include "Skewb2dP.h"
#include "Skewb3dP.h"

#ifndef DATAFILE
#define DATAFILE "/usr/games/lib/skewb.data"
#endif

static void InitializeSkewb();
static void DestroySkewb();
static Boolean SetValuesSkewb();
static void get_color();
static void move_control_cb();
static void reset_polyhedrons();
static void no_move_polyhedrons();
static void practice_polyhedrons();
static void randomize_polyhedrons();
static void move_polyhedrons();
static void read_diagonal();
static void rotate_diagonal();
static void write_diagonal();
static void read_face();
static void write_face();
static void rotate_face();
static void draw_diamond();
static void draw_triangle();

SkewbClassRec skewbClassRec =
{
  {
    (WidgetClass) &widgetClassRec,	/* superclass */
    "Skewb",				/* class name */
    sizeof(SkewbRec),			/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeSkewb,			/* initialize */
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
    DestroySkewb,			/* destroy */
    NULL,				/* resize */
    NULL,				/* expose */
    SetValuesSkewb,			/* set values */
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

WidgetClass skewbWidgetClass = (WidgetClass) &skewbClassRec;

static SkewbLoc slide_next_row[MAXFACES][MAXORIENT][MAXORIENT / 2] =
{
  {
    {{2,   CW}, {1, HALF}},
    {{5,  CCW}, {1, STRT}},
    {{3, STRT}, {5,   CW}},
    {{3, HALF}, {2,  CCW}}
  },
  {
    {{4, STRT}, {5,   CW}},
    {{0, STRT}, {5,  CCW}},
    {{2,  CCW}, {0, HALF}},
    {{2,   CW}, {4, HALF}}
  },
  {
    {{4,   CW}, {1,  CCW}},
    {{0,  CCW}, {1,   CW}},
    {{3,  CCW}, {0,   CW}},
    {{3,   CW}, {4,  CCW}}
  },
  {
    {{4, HALF}, {2,  CCW}},
    {{0, HALF}, {2,   CW}},
    {{5,   CW}, {0, STRT}},
    {{5,  CCW}, {4, STRT}}
  },
  {
    {{5,   CW}, {1, STRT}},
    {{2,  CCW}, {1, HALF}},
    {{3, HALF}, {2,   CW}},
    {{3, STRT}, {5,  CCW}}
  },
  {
    {{0,   CW}, {1,   CW}},
    {{4,  CCW}, {1,  CCW}},
    {{3,   CW}, {4,   CW}},
    {{3,  CCW}, {0,  CCW}}
  }
};
static SkewbLoc min_to_maj[MAXFACES][MAXORIENT] =
{ /* other equivalent mappings possible */
  {{3,   CW}, {2, STRT}, {1,  CCW}, {5, STRT}},
  {{2, STRT}, {4,  CCW}, {5, HALF}, {0,   CW}},
  {{3, STRT}, {4, STRT}, {1, STRT}, {0, STRT}},
  {{5, HALF}, {4,   CW}, {2, STRT}, {0,  CCW}},
  {{3,  CCW}, {5, STRT}, {1,   CW}, {2, STRT}},
  {{3, HALF}, {0, STRT}, {1, HALF}, {4, STRT}}
};

static SkewbLoc slide_next_face[MAXFACES][MAXORIENT] =
{
  {{5, STRT}, {3,   CW}, {2, STRT}, {1,  CCW}},
  {{0,   CW}, {2, STRT}, {4,  CCW}, {5, HALF}},
  {{0, STRT}, {3, STRT}, {4, STRT}, {1, STRT}},
  {{0,  CCW}, {5, HALF}, {4,   CW}, {2, STRT}},
  {{2, STRT}, {3,  CCW}, {5, STRT}, {1,   CW}},
  {{4, STRT}, {3, HALF}, {0, STRT}, {1, HALF}}
};

static int face_to_rotate[MAXFACES][MAXORIENT] =
{
  {3, 2, 1, 5},
  {2, 4, 5, 0},
  {3, 4, 1, 0},
  {5, 4, 2, 0},
  {3, 5, 1, 2},
  {3, 0, 1, 4}
};

static void InitializeSkewb(request, new)
  Widget request, new;
{
  SkewbWidget w = (SkewbWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  int face;

  init_moves();
  reset_polyhedrons(w);
  w->skewb.practice = FALSE;
  (void) SRAND(getpid());
  valueMask = GCForeground | GCBackground;
  values.foreground = w->skewb.foreground;
  values.background = w->core.background_pixel;
  w->skewb.puzzle_GC = XtGetGC(new, valueMask, &values);
  w->skewb.depth = DefaultDepthOfScreen(XtScreen(w));
  valueMask = GCForeground | GCBackground;
  values.foreground = w->core.background_pixel;
  values.background = w->skewb.foreground;
  w->skewb.inverse_GC = XtGetGC(new, valueMask, &values);
  for (face = 0; face < MAXFACES; face++)
    get_color(w, face, TRUE);
}

static void DestroySkewb(old)
  Widget old;
{
  SkewbWidget w = (SkewbWidget) old;
  int face;

  for (face = 0; face < MAXFACES; face++)
    XtReleaseGC(old, w->skewb.face_GC[face]);
  XtReleaseGC(old, w->skewb.puzzle_GC);
  XtReleaseGC(old, w->skewb.inverse_GC);
  XtRemoveCallbacks(old, XtNselectCallback, w->skewb.select);
}

static Boolean SetValuesSkewb(current, request, new)
  Widget current, request, new;
{
  SkewbWidget c = (SkewbWidget) current, w = (SkewbWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  Boolean redraw = FALSE;
  int face;

  if (w->skewb.foreground != c->skewb.foreground) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->skewb.foreground;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->skewb.puzzle_GC);
    w->skewb.puzzle_GC = XtGetGC(new, valueMask, &values);
    redraw = TRUE;
  }
  if (w->core.background_pixel != c->core.background_pixel) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->core.background_pixel;
    values.background = w->skewb.foreground;
    XtReleaseGC(new, w->skewb.inverse_GC);
    w->skewb.inverse_GC = XtGetGC(new, valueMask, &values);
    redraw = TRUE;
  }
  if (w->skewb.mono || w->skewb.depth == 1) {
    valueMask = GCForeground | GCBackground;
    values.background = w->core.background_pixel;
    values.foreground = w->skewb.foreground;
    for (face = 0; face < MAXFACES; face++) {
      XtReleaseGC(new, w->skewb.face_GC[face]);
      w->skewb.face_GC[face] = XtGetGC(new, valueMask, &values);
    }
    redraw = TRUE;
  }
  for (face = 0; face < MAXFACES; face++) {
    if (strcmp(w->skewb.face_name[face], c->skewb.face_name[face]))
      get_color(w, face, FALSE);
  }
  if (w->skewb.orient != c->skewb.orient) {
    reset_polyhedrons(w);
    w->skewb.practice = FALSE;
    redraw = TRUE;
  } else if (w->skewb.practice != c->skewb.practice) {
    reset_polyhedrons(w);
    redraw = TRUE;
  }
  if (w->skewb.face == SKEWB_RESTORE) {
    set_start_position(w);
    w->skewb.face = SKEWB_IGNORE;
  } else if (w->skewb.face != SKEWB_IGNORE) {
    move_polyhedrons(w, w->skewb.face, w->skewb.cube, w->skewb.direction);
    w->skewb.face = SKEWB_IGNORE;
  }
  return redraw;
}

void quit_skewb(w, event, args, n_args)
  SkewbWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  XtCloseDisplay(XtDisplay(w));
  exit(0);
}

void practice_skewb(w, event, args, n_args)
  SkewbWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  practice_polyhedrons(w);
}

void maybe_practice_skewb(w, event, args, n_args)
  SkewbWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->skewb.started)
    practice_polyhedrons(w);
}

void randomize_skewb(w, event, args, n_args)
  SkewbWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  randomize_polyhedrons(w);
}

void maybe_randomize_skewb(w, event, args, n_args)
  SkewbWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->skewb.started)
    randomize_polyhedrons(w);
}

void enter_skewb(w, event, args, n_args)
  SkewbWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;
  char c;
  int orient, practice, moves;
  skewbCallbackStruct cb;

  if ((fp = fopen(DATAFILE, "r")) == NULL)
    (void) printf("Can not read %s to enter.\n", DATAFILE);
  else {
    flush_moves(w);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &orient);
    if (w->skewb.orient != (Boolean) orient) {
      cb.reason = SKEWB_ORIENT;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &practice);
    if (w->skewb.practice != (Boolean) practice) {
      cb.reason = SKEWB_PRACTICE;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &moves);
    scan_start_position(fp, w);
    cb.reason = SKEWB_RESTORE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    scan_moves(fp, w, moves);
    (void) fclose(fp);
    (void) printf("%s: orient %d, practice %d, moves %d.\n",
      DATAFILE, orient, practice, moves);
  }
}

void write_skewb(w, event, args, n_args)
  SkewbWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;

  if ((fp = fopen(DATAFILE, "w")) == NULL)
    (void) printf("Can not write to %s.\n", DATAFILE);
  else {
    (void) fprintf(fp, "orient: %d\n", (w->skewb.orient) ? 1 : 0);
    (void) fprintf(fp, "practice: %d\n", (w->skewb.practice) ? 1 : 0);
    (void) fprintf(fp, "moves: %d\n", num_moves());
    print_start_position(fp, w);
    print_moves(fp);
    (void) fclose(fp);
    (void) printf("Saved to %s.\n", DATAFILE);
  }
}

void undo_skewb(w, event, args, n_args)
  SkewbWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (moves_made()) {
    int face, corner, direction, control;

    get_move(&face, &corner, &direction, &control);
    if (direction < 2 * MAXORIENT)
      direction = (direction < MAXORIENT) ? (direction + MAXORIENT / 2) %
        MAXORIENT : 3 * MAXORIENT - direction;
    else
      direction = 5 * MAXORIENT - direction;
    if (control)
      move_control_cb(w, face, corner, direction);
    else {
      skewbCallbackStruct cb;

      move_polyhedrons(w, face, corner, direction);
      cb.reason = SKEWB_UNDO;
      cb.face = face;
      cb.corner = corner;
      cb.direction = direction;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}

void solve_skewb(w, event, args, n_args)
  SkewbWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  /* solve_polyhedrons(w); */ /* Sorry, unimplemented */
}

void orientize_skewb(w, event, args, n_args)
  SkewbWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  skewbCallbackStruct cb;

  cb.reason = SKEWB_ORIENT;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

void move_skewb_ccw(w, event, args, n_args)
  SkewbWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input(w, event->xbutton.x, event->xbutton.y, CCW,
    (int) (event->xbutton.state & ControlMask));
}

void move_skewb_cw(w, event, args, n_args)
  SkewbWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input(w, event->xbutton.x, event->xbutton.y, CW,
    (int) (event->xkey.state & ControlMask));
}

void move_skewb_input(w, x, y, direction, control)
  SkewbWidget w;
  int x, y, direction, control;
{
  int face, corner;

  if (!w->skewb.practice && !control && check_solved(w)) {
    no_move_polyhedrons(w);
    return;
  }
  if (w->skewb.dim == 2 && !control && direction >= 2 * MAXORIENT)
    return;
  if ((w->skewb.dim == 2 && !position_polyhedrons2d((Skewb2DWidget) w,
       x, y, &face, &corner, &direction)) ||
      (w->skewb.dim == 3 && !position_polyhedrons3d((Skewb3DWidget) w,
       x, y, &face, &corner, &direction)))
    return;
  control = (control) ? 1 : 0;
  move_skewb(w, face, corner, direction, control);
  if (!control && check_solved(w)) {
    skewbCallbackStruct cb;

    cb.reason = SKEWB_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

void move_skewb(w, face, corner, direction, control)
  SkewbWidget w;
  int face, corner, direction, control;
{
  if (control)
    move_control_cb(w, face, corner, direction);
  else {
    skewbCallbackStruct cb;

    move_polyhedrons(w, face, corner, direction);
    cb.reason = SKEWB_MOVED;
    cb.face = face;
    cb.corner = corner;
    cb.direction = direction;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  put_move(face, corner, direction, control);
}

static void get_color(w, face, init)
  SkewbWidget w;
  int face, init;
{
  XGCValues values;
  XtGCMask valueMask;
  XColor color_cell, rgb;
 
  valueMask = GCForeground | GCBackground;
  values.background = w->core.background_pixel;
  if (w->skewb.depth > 1 && !w->skewb.mono) {
    if (XAllocNamedColor(XtDisplay(w),
        DefaultColormap(XtDisplay(w), XtWindow(w)),
        w->skewb.face_name[face], &color_cell, &rgb)) {
      values.foreground = w->skewb.face_color[face] = color_cell.pixel;
      if (!init)
        XtReleaseGC((Widget) w, w->skewb.face_GC[face]);
      w->skewb.face_GC[face] = XtGetGC((Widget) w, valueMask, &values);
      return;
    } else {
      char buf[121];

      (void) sprintf(buf, "Color name \"%s\" is not defined",
               w->skewb.face_name[face]);
      XtWarning(buf);
    }
  }
  values.foreground = w->skewb.foreground;
  if (!init)
    XtReleaseGC((Widget) w, w->skewb.face_GC[face]);
  w->skewb.face_GC[face] = XtGetGC((Widget) w, valueMask, &values);
}

static void move_control_cb(w, face, corner, direction)
  SkewbWidget w;
  int face, corner, direction;
{
  skewbCallbackStruct cb;
  int new_face, rotate;

  move_polyhedrons(w, face, corner, direction);
  cb.reason = SKEWB_CONTROL;
  cb.face = face;
  cb.corner = corner;
  cb.direction = direction;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  if (direction < MAXORIENT) {
    new_face = min_to_maj[face][corner].face;
    rotate = min_to_maj[face][corner].rotation % MAXORIENT;
    direction = (rotate + direction) % MAXORIENT;
    corner = (corner + rotate + 2) % MAXORIENT;
    move_polyhedrons(w, new_face, corner, direction);
    cb.face = new_face;
    cb.corner = corner;
    cb.direction = direction;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void reset_polyhedrons(w)
  SkewbWidget w;
{
  int face, cube;

  for (face = 0; face < MAXFACES; face++)
    for (cube = 0; cube < MAXCUBES; cube++) {
      w->skewb.cube_loc[face][cube].face = face;
      w->skewb.cube_loc[face][cube].rotation = STRT - MAXORIENT;
    }
  flush_moves(w);
  w->skewb.started = FALSE;
}

static void no_move_polyhedrons(w)
  SkewbWidget w;
{
  skewbCallbackStruct cb;

  cb.reason = SKEWB_IGNORE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void practice_polyhedrons(w)
  SkewbWidget w;
{
  skewbCallbackStruct cb;

  cb.reason = SKEWB_PRACTICE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void randomize_polyhedrons(w)
  SkewbWidget w;
{
  skewbCallbackStruct cb;
  int face, corner, direction;
  int big = MAXCUBES * 3 + NRAND(2);

  if (w->skewb.practice)
    practice_polyhedrons(w);
  cb.reason = SKEWB_RESET;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);

#ifdef DEBUG
  big = 3;
#endif

  while (big--) {
    face = NRAND(MAXFACES);
    corner = NRAND(MAXORIENT);
    direction = ((NRAND(2)) ? corner + 1 : corner + 3) % MAXORIENT;
    move_skewb(w, face, corner, direction, FALSE);
  }
  flush_moves(w);
  cb.reason = SKEWB_RANDOMIZE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  if (check_solved(w)) {
    cb.reason = SKEWB_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void move_polyhedrons(w, face, corner, direction)
  SkewbWidget w;
  int face, corner, direction;
{
  int new_face, new_direction, new_corner, k, size, rotate;

  if (direction < 2 * MAXORIENT) {
    /* corner as MAXORIENT is ambiguous */
    for (size = MINOR; size <= MAJOR; size++) {
      read_diagonal((SkewbWidget) w, face, corner, 0, size);
      for (k = 1; k <= MAXROTATE; k++) {
        new_face = slide_next_row[face][corner][direction / 2].face;
        rotate = slide_next_row[face][corner][direction / 2].rotation %
          MAXORIENT;
        new_direction = (rotate + direction) %  MAXORIENT;
        new_corner = (rotate + corner) %  MAXORIENT;
        if (k != MAXROTATE)
          read_diagonal((SkewbWidget) w, new_face, new_corner, k, size);
        rotate_diagonal((SkewbWidget) w, rotate, k - 1, size);
        write_diagonal(w, new_face, new_corner, k - 1, size);
        face = new_face;
        corner = new_corner;
        direction = new_direction;
      }
      if (size == MINOR) {
        new_face = min_to_maj[face][corner].face;
        rotate = min_to_maj[face][corner].rotation % MAXORIENT;
        direction = (rotate + direction) % MAXORIENT;
        corner = (corner + rotate + 2) % MAXORIENT;
        face = new_face;
      }
    }
  } else {
    rotate_face(w, face_to_rotate[face][direction % MAXORIENT], CW);
    rotate_face(w, face_to_rotate[face][(direction + 2) % MAXORIENT], CCW);
    read_face((SkewbWidget) w, face, 0);
    for (k = 1; k <= MAXORIENT; k++) {
      new_face = slide_next_face[face][direction % MAXORIENT].face;
      rotate = slide_next_face[face][direction % MAXORIENT].rotation;
      new_direction = (rotate + direction) % MAXORIENT;
      if (k != MAXORIENT)
        read_face((SkewbWidget) w, new_face, k);
      write_face(w, new_face, rotate, k - 1);
      face = new_face;
      direction = new_direction;
    }
  }
}

static void read_diagonal(w, face, corner, orient, size)
  SkewbWidget w;
  int face, corner, orient, size;
{
  int g;

  if (size == MINOR)
    w->skewb.minor_loc[orient] = w->skewb.cube_loc[face][corner];
  else /* size == MAJOR */
  {
    for (g = 1; g < MAXORIENT; g++)
      w->skewb.major_loc[orient][g - 1] =
        w->skewb.cube_loc[face][(corner + g) % MAXORIENT];
    w->skewb.major_loc[orient][MAXORIENT - 1] =
      w->skewb.cube_loc[face][MAXORIENT];
  }
}

static void rotate_diagonal(w, rotate, orient, size)
  SkewbWidget w;
  int rotate, orient, size;
{
  int g;

  if (size == MINOR)
    w->skewb.minor_loc[orient].rotation =
      (w->skewb.minor_loc[orient].rotation + rotate) % MAXORIENT;
  else /* size == MAJOR */
    for (g = 0; g < MAXORIENT; g++)
      w->skewb.major_loc[orient][g].rotation =
        (w->skewb.major_loc[orient][g].rotation + rotate) % MAXORIENT;
}

static void write_diagonal(w, face, corner, orient, size)
  SkewbWidget w;
  int face, corner, orient, size;
{
  int g, h;

  if (size == MINOR) {
    w->skewb.cube_loc[face][corner] = w->skewb.minor_loc[orient];
    draw_triangle(w, face, corner);
  } else /* size == MAJOR */ {
    w->skewb.cube_loc[face][MAXORIENT] =
      w->skewb.major_loc[orient][MAXORIENT - 1];
    draw_diamond(w, face);
    for (g = 1; g < MAXORIENT; g++) {
      h = (corner + g) % MAXORIENT;
      w->skewb.cube_loc[face][h] = w->skewb.major_loc[orient][g - 1];
      draw_triangle(w, face, h);
    }
  }
}

static void read_face(w, face, h)
  SkewbWidget w;
  int face, h;
{
  int cube;

  for (cube = 0; cube < MAXCUBES; cube++)
    w->skewb.row_loc[h][cube] = w->skewb.cube_loc[face][cube];
}

static void write_face(w, face, rotate, h)
  SkewbWidget w;
  int face, rotate, h;
{
  int corner, new_corner;

  for (corner = 0; corner < MAXORIENT; corner++) {
    new_corner = (corner + rotate) % MAXORIENT;
    w->skewb.cube_loc[face][new_corner] = w->skewb.row_loc[h][corner];
    w->skewb.cube_loc[face][new_corner].rotation =
      (w->skewb.cube_loc[face][new_corner].rotation + rotate) % MAXORIENT;
    draw_triangle(w, face, (corner + rotate) % MAXORIENT);
  }
  w->skewb.cube_loc[face][MAXORIENT] = w->skewb.row_loc[h][MAXORIENT];
  w->skewb.cube_loc[face][MAXORIENT].rotation =
    (w->skewb.cube_loc[face][MAXORIENT].rotation + rotate) % MAXORIENT;
  draw_diamond(w, face);
}

static void rotate_face(w, face, direction)
  SkewbWidget w;
  int face, direction;
{
  int corner;

  /* Read Face */
  for (corner = 0; corner < MAXORIENT; corner++)
    w->skewb.face_loc[corner] = w->skewb.cube_loc[face][corner];
  /* Write Face */
  for (corner = 0; corner < MAXORIENT; corner++) {
    w->skewb.cube_loc[face][corner] = (direction == CW) ?
      w->skewb.face_loc[(corner + MAXORIENT - 1) % MAXORIENT]:
      w->skewb.face_loc[(corner + 1) % MAXORIENT];
    w->skewb.cube_loc[face][corner].rotation =
      (w->skewb.cube_loc[face][corner].rotation + direction) % MAXORIENT;
    draw_triangle(w, face, corner);
  }
  w->skewb.cube_loc[face][MAXORIENT].rotation =
    (w->skewb.cube_loc[face][MAXORIENT].rotation + direction) % MAXORIENT;
  draw_diamond(w, face);
}

void draw_all_polyhedrons(w)
  SkewbWidget w;
{
  int face, corner;

  for (face = 0; face < MAXFACES; face++) {
    draw_diamond(w, face);
    for (corner = 0; corner < MAXORIENT; corner++)
      draw_triangle(w, face, corner);
  }
}

static void draw_diamond(w, face)
  SkewbWidget w;
  int face;
{
  if (w->skewb.dim == 2)
    draw_diamond2d((Skewb2DWidget) w, face);
  else if (w->skewb.dim == 3)
    draw_diamond3d((Skewb3DWidget) w, face);
}

static void draw_triangle(w, face, corner)
  SkewbWidget w;
  int face, corner;
{
  if (w->skewb.dim == 2)
    draw_triangle2d((Skewb2DWidget) w, face, corner);
  else if (w->skewb.dim == 3)
    draw_triangle3d((Skewb3DWidget) w, face, corner);
}

Boolean check_solved(w)
  SkewbWidget w;
{
  int face, cube;
  SkewbLoc test;

  for (face = 0; face < MAXFACES; face++)
    for (cube = 0; cube < MAXCUBES; cube++) {
      if (cube == 0) {
        test.face = w->skewb.cube_loc[face][cube].face;
        test.rotation = w->skewb.cube_loc[face][cube].rotation;
      } else if (test.face != /*face*/
               w->skewb.cube_loc[face][cube].face ||
               (w->skewb.orient && test.rotation != /*STRT - MAXORIENT*/
                w->skewb.cube_loc[face][cube].rotation))
          return FALSE;
    }
  return TRUE;
}

#ifdef DEBUG

void print_cube(w)
  SkewbWidget w;
{
  int face, cube;

  for (face = 0; face < MAXFACES; face++) {
    for (cube = 0; cube < MAXCUBES; cube++)
      (void) printf("%d %d  ", w->skewb.cube_loc[face][cube].face,
               w->skewb.cube_loc[face][cube].rotation);
    (void) printf("\n");
  }
  (void) printf("\n");
}

#endif
