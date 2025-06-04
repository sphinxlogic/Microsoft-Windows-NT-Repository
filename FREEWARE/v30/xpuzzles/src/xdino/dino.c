/*
# X-BASED DINO
#
#  Dino.c
#
###
#
#  Copyright (c) 1995	David Albert Bagley, bagleyd@source.asset.com
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

/* Methods file for Dino */

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
#include "DinoP.h"
#include "Dino2dP.h"
#include "Dino3dP.h"

#ifndef DATAFILE
#define DATAFILE "/usr/games/lib/dino.data"
#endif

static void InitializeDino();
static void DestroyDino();
static Boolean SetValuesDino();
static void get_color();
static void move_control_cb();
static void check_polyhedrons();
static void reset_polyhedrons();
static void no_move_polyhedrons();
static void practice_polyhedrons();
static void randomize_polyhedrons();
static void move_polyhedrons();
static void move_inside_corners();
static void move_outside_corners();
static void move_edges();
static void move_faces();
static void read_diagonal();
static void write_diagonal();
static void read_face();
static void write_face();
static void rotate_face();
static void draw_triangle();

DinoClassRec dinoClassRec =
{
  {
    (WidgetClass) &widgetClassRec,	/* superclass */
    "Dino",				/* class name */
    sizeof(DinoRec),			/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeDino,			/* initialize */
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
    DestroyDino,			/* destroy */
    NULL,				/* resize */
    NULL,				/* expose */
    SetValuesDino,			/* set values */
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

WidgetClass dinoWidgetClass = (WidgetClass) &dinoClassRec;

static DinoLoc slide_corner[MAXFACES][MAXORIENT][MAXORIENT/2] =
{
  {
    {{3, TR, 0}, {5, BR, 0}},
    {{3, TL, 1}, {2, TR, 0}},
    {{2, TL, 1}, {1, TR, 0}},
    {{5, BL, 1}, {1, TL, 1}}
  },
  {
    {{2, TL, 0}, {0, BL, 0}},
    {{2, BL, 0}, {4, TL, 0}},
    {{4, BL, 0}, {5, TL, 0}},
    {{0, TL, 0}, {5, BL, 0}}
  },
  {
    {{3, TL, 0}, {0, BR, 0}},
    {{3, BL, 0}, {4, TR, 0}},
    {{4, TL, 1}, {1, BR, 1}},
    {{0, BL, 1}, {1, TR, 1}}
  },
  {
    {{5, BR, 1}, {0, TR, 1}},
    {{5, TR, 1}, {4, BR, 1}},
    {{4, TR, 1}, {2, BR, 1}},
    {{0, BR, 1}, {2, TR, 1}}
  },
  {
    {{3, BL, 1}, {2, BR, 0}},
    {{3, BR, 0}, {5, TR, 0}},
    {{5, TL, 1}, {1, BL, 1}},
    {{2, BL, 1}, {1, BR, 0}}
  },
  {
    {{3, BR, 1}, {4, BR, 0}},
    {{3, TR, 1}, {0, TR, 0}},
    {{0, TL, 1}, {1, TL, 0}},
    {{4, BL, 1}, {1, BL, 0}}
  }
};

static int opp_face[MAXFACES] = {4, 3, 5, 1, 0, 2};

static DinoCornerLoc opp_corner[MAXFACES][MAXORIENT] =
{
  {{4, 3}, {4, 2}, {4, 1}, {4, 0}},
  {{3, 1}, {3, 0}, {3, 3}, {3, 2}},
  {{5, 3}, {5, 2}, {5, 1}, {5, 0}},
  {{1, 1}, {1, 0}, {1, 3}, {1, 2}},
  {{0, 3}, {0, 2}, {0, 1}, {0, 0}},
  {{2, 3}, {2, 2}, {2, 1}, {2, 0}}
};

/*static int slide_next_face2[MAXFACES] = {4, 3, 5, 1, 0, 2};*/

static DinoCornerLoc slide_next_face[MAXFACES][MAXORIENT] =
{
  {{5, STRT}, {3,   CW}, {2, STRT}, {1,  CCW}},
  {{0,   CW}, {2, STRT}, {4,  CCW}, {5, HALF}},
  {{0, STRT}, {3, STRT}, {4, STRT}, {1, STRT}},
  {{0,  CCW}, {5, HALF}, {4,   CW}, {2, STRT}},
  {{2, STRT}, {3,  CCW}, {5, STRT}, {1,   CW}},
  {{4, STRT}, {3, HALF}, {0, STRT}, {1, HALF}}
};

static int face_to_rotate2[MAXFACES][MAXORIENT][2] =
{
  {{3, 5}, {2, 3}, {1, 2}, {1, 5}},
  {{0, 2}, {2, 4}, {4, 5}, {0, 5}},
  {{3, 0}, {4, 3}, {1, 4}, {0, 1}},
  {{0, 5}, {4, 5}, {2, 4}, {0, 2}},
  {{2, 3}, {3, 5}, {1, 5}, {1, 2}},
  {{4, 3}, {3, 0}, {0, 1}, {1, 4}}
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

static void InitializeDino(request, new)
  Widget request, new;
{
  DinoWidget w = (DinoWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  int face;

  check_polyhedrons(w);
  init_moves();
  reset_polyhedrons(w);
  w->dino.practice = FALSE;
  (void) SRAND(getpid());
  valueMask = GCForeground | GCBackground;
  values.foreground = w->dino.foreground;
  values.background = w->core.background_pixel;
  w->dino.puzzle_GC = XtGetGC(new, valueMask, &values);
  w->dino.depth = DefaultDepthOfScreen(XtScreen(w));
  valueMask = GCForeground | GCBackground;
  values.foreground = w->core.background_pixel;
  values.background = w->dino.foreground;
  w->dino.inverse_GC = XtGetGC(new, valueMask, &values);
  for (face = 0; face < MAXFACES; face++)
    get_color(w, face, TRUE);
}

static void DestroyDino(old)
  Widget old;
{
  DinoWidget w = (DinoWidget) old;
  int face;

  for (face = 0; face < MAXFACES; face++)
    XtReleaseGC(old, w->dino.face_GC[face]);
  XtReleaseGC(old, w->dino.puzzle_GC);
  XtReleaseGC(old, w->dino.inverse_GC);
  XtRemoveCallbacks(old, XtNselectCallback, w->dino.select);
}

static Boolean SetValuesDino(current, request, new)
  Widget current, request, new;
{
  DinoWidget c = (DinoWidget) current, w = (DinoWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  Boolean redraw = FALSE;
  int face;

  if (w->dino.foreground != c->dino.foreground) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->dino.foreground;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->dino.puzzle_GC);
    w->dino.puzzle_GC = XtGetGC(new, valueMask, &values);
    redraw = TRUE;
  }
  if (w->core.background_pixel != c->core.background_pixel) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->core.background_pixel;
    values.background = w->dino.foreground;
    XtReleaseGC(new, w->dino.inverse_GC);
    w->dino.inverse_GC = XtGetGC(new, valueMask, &values);
    redraw = TRUE;
  }
  if (w->dino.mono || w->dino.depth == 1) {
    valueMask = GCForeground | GCBackground;
    values.background = w->core.background_pixel;
    values.foreground = w->dino.foreground;
    for (face = 0; face < MAXFACES; face++) {
      XtReleaseGC(new, w->dino.face_GC[face]);
      w->dino.face_GC[face] = XtGetGC(new, valueMask, &values);
    }
    redraw = TRUE;
  }
  for (face = 0; face < MAXFACES; face++) {
    if (strcmp(w->dino.face_name[face], c->dino.face_name[face]))
      get_color(w, face, FALSE);
  }
  if (w->dino.orient != c->dino.orient) {
    reset_polyhedrons(w);
    w->dino.practice = FALSE;
    redraw = TRUE;
  } else if (w->dino.practice != c->dino.practice) {
    reset_polyhedrons(w);
    redraw = TRUE;
  }
  if (w->dino.mode != c->dino.mode) {
    reset_polyhedrons(w);
    w->dino.practice = FALSE;
    redraw = TRUE;
  }
  if (w->dino.face == DINO_RESTORE) {
    set_start_position(w);
    w->dino.face = DINO_IGNORE;
  } else if (w->dino.face != DINO_IGNORE) {
    move_polyhedrons(w,
      w->dino.face, w->dino.side, w->dino.direction, w->dino.style);
    w->dino.face = DINO_IGNORE;
  }
  return redraw;
}

void quit_dino(w, event, args, n_args)
  DinoWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  XtCloseDisplay(XtDisplay(w));
  exit(0);
}

void practice_dino(w, event, args, n_args)
  DinoWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  practice_polyhedrons(w);
}

void maybe_practice_dino(w, event, args, n_args)
  DinoWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->dino.started)
    practice_polyhedrons(w);
}

void randomize_dino(w, event, args, n_args)
  DinoWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  randomize_polyhedrons(w);
}

void maybe_randomize_dino(w, event, args, n_args)
  DinoWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->dino.started)
    randomize_polyhedrons(w);
}

void enter_dino(w, event, args, n_args)
  DinoWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;
  char c;
  int mode, orient, practice, moves;
  dinoCallbackStruct cb;

  if ((fp = fopen(DATAFILE, "r")) == NULL)
    (void) printf("Can not read %s to enter.\n", DATAFILE);
  else {
    flush_moves(w);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &mode);
    if (mode >= PERIOD2 && mode <= BOTH)
      switch (mode) {
        case PERIOD2:
          cb.reason = DINO_PERIOD2;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          break;
        case PERIOD3:
          cb.reason = DINO_PERIOD3;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          break;
        case BOTH:
          cb.reason = DINO_BOTH;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    else
      (void) printf("%s corrupted: mode %d should be between %d and %d\n",
         DATAFILE, mode, PERIOD2, BOTH);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &orient);
    if (w->dino.orient != (Boolean) orient) {
      cb.reason = DINO_ORIENT;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &practice);
    if (w->dino.practice != (Boolean) practice) {
      cb.reason = DINO_PRACTICE;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &moves);
    scan_start_position(fp, w);
    cb.reason = DINO_RESTORE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    scan_moves(fp, w, moves);
    (void) fclose(fp);
    (void) printf("%s: mode %d, orient %d, practice %d, moves %d.\n",
      DATAFILE, mode, orient, practice, moves);
  }
}

void write_dino(w, event, args, n_args)
  DinoWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;

  if ((fp = fopen(DATAFILE, "w")) == NULL)
    (void) printf("Can not write to %s.\n", DATAFILE);
  else {
    (void) fprintf(fp, "dino: %d\n", w->dino.mode);
    (void) fprintf(fp, "orient: %d\n", (w->dino.orient) ? 1 : 0);
    (void) fprintf(fp, "practice: %d\n", (w->dino.practice) ? 1 : 0);
    (void) fprintf(fp, "moves: %d\n", num_moves());
    print_start_position(fp, w);
    print_moves(fp);
    (void) fclose(fp);
    (void) printf("Saved to %s.\n", DATAFILE);
  }
}

void undo_dino(w, event, args, n_args)
  DinoWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (moves_made()) {
    int face, side, direction, style, control;

    get_move(&face, &side, &direction, &style, &control);
    direction = (direction < MAXORIENT) ? (direction + MAXORIENT / 2) %
      MAXORIENT : 5 * MAXORIENT - direction;
    if (control)
      move_control_cb(w, face, side, direction, style);
    else {
      dinoCallbackStruct cb;

      move_polyhedrons(w, face, side, direction, style);
      cb.reason = DINO_UNDO;
      cb.face = face;
      cb.side = side;
      cb.direction = direction;
      cb.style = style;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}

void solve_dino(w, event, args, n_args)
  DinoWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  /* solve_polyhedrons(w); */ /* Sorry, unimplemented */
}

void orientize_dino(w, event, args, n_args)
  DinoWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  dinoCallbackStruct cb;
 
  cb.reason = DINO_ORIENT;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

void period2_mode_dino(w, event, args, n_args)
  DinoWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  dinoCallbackStruct cb;
 
  cb.reason = DINO_PERIOD2;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
void period3_mode_dino(w, event, args, n_args)
  DinoWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  dinoCallbackStruct cb;
 
  cb.reason = DINO_PERIOD3;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
void both_mode_dino(w, event, args, n_args)
  DinoWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  dinoCallbackStruct cb;
 
  cb.reason = DINO_BOTH;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
void move_dino_input(w, x, y, direction, shift, control, alt)
  DinoWidget w;
  int x, y, direction, shift, control, alt;
{
  int face, side, style;

  if (!w->dino.practice && !control && check_solved(w)) {
    no_move_polyhedrons(w);
    return;
  }
  if (w->dino.dim == 2 && direction >= 2 * MAXORIENT) {
    if (control)
      style = FACE;
    else
       return;
  } else if (w->dino.mode != BOTH) {
    if (control && alt) {
      if (w->dino.mode == PERIOD3)
        style = EDGE;
      else if (shift)
        style = MIDDLE;
      else
        style = CORNER;
    } else if (w->dino.mode == PERIOD2)
      style = EDGE;
    else if (shift)
      style = MIDDLE;
    else
      style = CORNER;
  } else {
    if (alt)
      style = EDGE;
    else {
      if (shift)
        style = MIDDLE;
      else
        style = CORNER;
    }
  }
  if ((w->dino.dim == 2 && !position_polyhedrons2d((Dino2DWidget) w,
       x, y, &face, &side, &direction)) ||
      (w->dino.dim == 3 && !position_polyhedrons3d((Dino3DWidget) w,
       x, y, &face, &side, &direction)))
    return;
  control = (control) ? 1 : 0;
  move_dino(w, face, side, direction, style, control);
  if (!control && check_solved(w)) {
    dinoCallbackStruct cb;

    cb.reason = DINO_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

void move_dino(w, face, side, direction, style, control)
  DinoWidget w;
  int face, side, direction, style, control;
{
  if (control)
    move_control_cb(w, face, side, direction, style);
  else {
    dinoCallbackStruct cb;

    move_polyhedrons(w, face, side, direction, style);
    cb.reason = DINO_MOVED;
    cb.face = face;
    cb.side = side;
    cb.direction = direction;
    cb.style = style;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  put_move(face, side, direction, style, control);
}

static void get_color(w, face, init)
  DinoWidget w;
  int face, init;
{
  XGCValues values;
  XtGCMask valueMask;
  XColor color_cell, rgb;
 
  valueMask = GCForeground | GCBackground;
  values.background = w->core.background_pixel;
  if (w->dino.depth > 1 && !w->dino.mono) {
    if (XAllocNamedColor(XtDisplay(w),
        DefaultColormap(XtDisplay(w), XtWindow(w)),
        w->dino.face_name[face], &color_cell, &rgb)) {
      values.foreground = w->dino.face_color[face] = color_cell.pixel;
      if (!init)
        XtReleaseGC((Widget) w, w->dino.face_GC[face]);
      w->dino.face_GC[face] = XtGetGC((Widget) w, valueMask, &values);
      return;
    } else {
      char buf[121];

      (void) sprintf(buf, "Color name \"%s\" is not defined",
               w->dino.face_name[face]);
      XtWarning(buf);
    }
  }
  values.foreground = w->dino.foreground;
  if (!init)
    XtReleaseGC((Widget) w, w->dino.face_GC[face]);
  w->dino.face_GC[face] = XtGetGC((Widget) w, valueMask, &values);
}

static void move_control_cb(w, face, side, direction, style)
  DinoWidget w;
  int face, side, direction, style;
{
  dinoCallbackStruct cb;
  int new_face, new_side, new_direction, corner, new_corner;

  move_polyhedrons(w, face, side, direction, style);
  cb.reason = DINO_CONTROL;
  cb.face = face;
  cb.side = side;
  cb.direction = direction;
  cb.style = style;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  if (style == CORNER) {
    new_side = (side + 2) % MAXORIENT + 2 * MAXORIENT;
    move_polyhedrons(w, face, new_side, direction, MIDDLE);
    cb.face = face;
    cb.side = new_side;
    cb.direction = direction;
    cb.style = MIDDLE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    corner = (side - !((side + direction) % 2)) % MAXORIENT;
    new_face = opp_corner[face][corner].face;
    new_corner = opp_corner[face][corner].rotation;
    new_direction = 2 * ((direction / 2 + (face != 1 && face != 3)) % 2) +
      !(new_corner % 2);
    new_side = new_corner + 2 * MAXORIENT;
    move_polyhedrons(w, new_face, new_side, new_direction, CORNER);
    cb.face = new_face;
    cb.side = new_side;
    cb.direction = new_direction;
    cb.style = CORNER;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  } else if (style == MIDDLE) {
    new_side = (side + 2) % MAXORIENT + 2 * MAXORIENT; 
    move_polyhedrons(w, face, new_side, direction, CORNER);
    cb.face = face;
    cb.side = new_side;
    cb.direction = direction;
    cb.style = CORNER;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    corner = (side - !((side + direction) % 2)) % MAXORIENT;
    new_corner = opp_corner[face][corner].rotation;
    new_face = opp_corner[face][corner].face;
    if (new_face != 1 && new_face != 3)
      new_direction = (direction + 2) % MAXORIENT;
    else
      new_direction = direction;
    new_direction = 2 * (new_direction / 2) + !(new_direction % 2);
    new_side = (new_corner + 2) % MAXORIENT + 2 * MAXORIENT;
    move_polyhedrons(w, new_face, new_side, new_direction, CORNER);
    cb.face = new_face;
    cb.side = new_side;
    cb.direction = new_direction;
    cb.style = CORNER;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  } else if (style == EDGE) {
    new_side = (side + 2) % MAXORIENT; 
    move_polyhedrons(w, face, new_side, direction, EDGE);
    cb.face = face;
    cb.side = new_side;
    cb.direction = direction;
    cb.style = EDGE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void check_polyhedrons(w)
  DinoWidget w;
{
  if (w->dino.mode < PERIOD2 || w->dino.mode > BOTH) {
    XtWarning("Mode is in error, use 2 for Period2, 3 for Period3, 4 for Both");
    w->dino.mode = DEFAULTMODE;
  }
}

static void reset_polyhedrons(w)
  DinoWidget w;
{
  int face, side;

  for (face = 0; face < MAXFACES; face++)
    for (side = 0; side < MAXORIENT; side++) {
      w->dino.cube_loc[face][side].face = face;
      w->dino.cube_loc[face][side].rotation = STRT - MAXORIENT;
    }
  flush_moves(w);
  w->dino.started = FALSE;
}

static void no_move_polyhedrons(w)
  DinoWidget w;
{
  dinoCallbackStruct cb;

  cb.reason = DINO_IGNORE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void practice_polyhedrons(w)
  DinoWidget w;
{
  dinoCallbackStruct cb;

  cb.reason = DINO_PRACTICE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void randomize_polyhedrons(w)
  DinoWidget w;
{
  dinoCallbackStruct cb;
  int face, side, direction, style;
  int big = MAXORIENT * 3 + NRAND(2);

  if (w->dino.practice)
    practice_polyhedrons(w);
  cb.reason = DINO_RESET;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);

#ifdef DEBUG
  big = 3;
#endif

  while (big--) {
    face = NRAND(MAXFACES);
    side = NRAND(MAXORIENT);
    direction = ((NRAND(2)) ? side + 1 : side + 3) % MAXORIENT;
    if (w->dino.mode == PERIOD2)
      style = EDGE;
    else if (w->dino.mode == BOTH)
      style = NRAND(3);
    else
      style = NRAND(2);
    move_dino(w, face, side, direction, style, FALSE);
  }
  flush_moves(w);
  cb.reason = DINO_RANDOMIZE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  if (check_solved(w)) {
    cb.reason = DINO_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void move_polyhedrons(w, face, side, direction, style)
  DinoWidget w;
  int face, side, direction, style;
{
  int corner, new_corner;

  corner = (side - !((side + direction) % 2) + MAXORIENT) % MAXORIENT;
  if (style == CORNER) {
    move_inside_corners(w, face, corner, direction);
  } else if (style == MIDDLE) {
    move_outside_corners(w, face, corner, direction);
    new_corner = opp_corner[face][corner].rotation;
    face = opp_corner[face][corner].face;
    if (((face != 1 && face != 3) + corner) % 2)
      direction = (direction + 1) % MAXORIENT;
    else
      direction = (direction + 3) % MAXORIENT;
    corner = new_corner;
    move_outside_corners(w, face, corner, direction);
  } else if (style == EDGE) {
    move_edges(w, face, corner, direction);
    move_faces(w, face, corner,
      ((face == 2 || face == 5) ? CCW : HALF) % MAXORIENT);
  } else {
    int k, new_face, rotate, new_direction;

    rotate_face(w, face_to_rotate[face][direction % MAXORIENT], CW);
    rotate_face(w, face_to_rotate[face][(direction + 2) % MAXORIENT], CCW);
    read_face((DinoWidget) w, face, 0);
    for (k = 1; k <= MAXORIENT; k++) {
      new_face = slide_next_face[face][direction % MAXORIENT].face;
      rotate = slide_next_face[face][direction % MAXORIENT].rotation;
      new_direction = (rotate + direction) % MAXORIENT;
      if (k != MAXORIENT)
        read_face((DinoWidget) w, new_face, k);
      write_face(w, new_face, rotate, k - 1);
      face = new_face;
      direction = new_direction;
    }
  }
}

static void move_inside_corners(w, face, corner, direction)
  DinoWidget w;
  int face, corner, direction;
{
  int new_face, new_corner, new_direction, dir, k;

  read_diagonal((DinoWidget) w, face, corner, 0);
  for (k = 1; k <= MAXROTATE; k++) {
    dir = direction / 2;
    new_face = slide_corner[face][corner][dir].face;
    new_corner = slide_corner[face][corner][dir].side;
    new_direction = 2 * slide_corner[face][corner][dir].dir + !(new_corner % 2);
    if (k != MAXROTATE)
      read_diagonal((DinoWidget) w, new_face, new_corner, k);
    write_diagonal(w, new_face, new_corner,
      (new_direction - direction + MAXORIENT) % MAXORIENT, k - 1);
    face = new_face;
    corner = new_corner;
    direction = new_direction;
  }
}

static void move_outside_corners(w, face, corner, direction)
  DinoWidget w;
  int face, corner, direction;
{
  int new_face, new_corner, new_direction, dir, k;

  read_diagonal((DinoWidget) w, face, corner, 0);
  for (k = 1; k <= MAXROTATE; k++) {
    corner = (corner + 2) % MAXORIENT;
    dir = direction / 2;
    new_face = slide_corner[face][corner][dir].face;
    new_corner = (slide_corner[face][corner][dir].side + 2) % MAXORIENT;
    new_direction = 2 * slide_corner[face][corner][dir].dir + !(new_corner % 2);
    if (k != MAXROTATE)
      read_diagonal((DinoWidget) w, new_face, new_corner, k);
    write_diagonal(w, new_face, new_corner,
      (new_direction - direction + MAXORIENT) % MAXORIENT, k - 1);
    face = new_face;
    corner = new_corner;
    direction = new_direction;
  }
}

static void move_edges(w, face, corner, direction)
  DinoWidget w;
  int face, corner, direction;
{
  int k, new_face, rotate, new_corner, new_direction;

  read_diagonal((DinoWidget) w, face, corner, 0);
  for (k = 1; k <= 2; k++) {
    new_face = opp_face[face];
    /*rotate = (((face == 1 || face == 3) ? 1 : 3) + 3 * direction) %
      MAXORIENT;*/
    new_corner = ((((face == 1 || face == 3) + corner) % 2) ?
      (corner + 3) : (corner + 1)) % MAXORIENT;
   rotate = (new_corner - corner + MAXORIENT) % MAXORIENT; 
   new_direction = (rotate + direction) % MAXORIENT;
    if (k != 2)
      read_diagonal((DinoWidget) w, new_face, new_corner, k);
    write_diagonal(w, new_face, new_corner, rotate, k - 1);
    face = new_face;
    corner = new_corner;
    direction = new_direction;
  }
}

static void move_faces(w, f, d, rotate)
  DinoWidget w;
  int f, d, rotate;
{
  int k, face, new_face;

  face = face_to_rotate2[f][d][0],
  read_face((DinoWidget) w, face, 0);
  for (k = 1; k <= 2; k++) {
    new_face = face_to_rotate2[f][d][k % 2],
    rotate = MAXORIENT - rotate;
    if (k != 2)
      read_face((DinoWidget) w, new_face, k);
    write_face(w, new_face, rotate, k - 1);
    face = new_face;
  }
}

static void read_diagonal(w, face, corner, h)
  DinoWidget w;
  int face, corner, h; 
{
  w->dino.spindle_loc[h][0] = w->dino.cube_loc[face][corner];
  w->dino.spindle_loc[h][1] = w->dino.cube_loc[face][(corner + 1) % MAXORIENT];
}

static void write_diagonal(w, face, corner, rotate, h)
  DinoWidget w;
  int face, corner, rotate, h;
{
  w->dino.spindle_loc[h][0].rotation =
    (w->dino.spindle_loc[h][0].rotation + rotate) % MAXORIENT;
  w->dino.spindle_loc[h][1].rotation =
    (w->dino.spindle_loc[h][1].rotation + rotate) % MAXORIENT;
  w->dino.cube_loc[face][corner] = w->dino.spindle_loc[h][0];
  draw_triangle(w, face, corner); 
  w->dino.cube_loc[face][(corner + 1) % MAXORIENT] = w->dino.spindle_loc[h][1];
  draw_triangle(w, face, (corner + 1) % MAXORIENT); 
}

static void read_face(w, face, h)
  DinoWidget w;
  int face, h;
{
  int side;

  for (side = 0; side < MAXORIENT; side++)
    w->dino.row_loc[h][side] = w->dino.cube_loc[face][side];
}

static void write_face(w, face, rotate, h)
  DinoWidget w;
  int face, rotate, h;
{
  int side, new_side;

  for (side = 0; side < MAXORIENT; side++) {
    new_side = (side + rotate) % MAXORIENT;
    w->dino.cube_loc[face][new_side] = w->dino.row_loc[h][side];
    w->dino.cube_loc[face][new_side].rotation =
      (w->dino.cube_loc[face][new_side].rotation + rotate) % MAXORIENT;
    draw_triangle(w, face, (side + rotate) % MAXORIENT);
  }
}

static void rotate_face(w, face, direction)
  DinoWidget w;
  int face, direction;
{
  int side;

  /* Read Face */
  for (side = 0; side < MAXORIENT; side++)
    w->dino.face_loc[side] = w->dino.cube_loc[face][side];
  /* Write Face */
  for (side = 0; side < MAXORIENT; side++) {
    w->dino.cube_loc[face][side] = (direction == CW) ?
      w->dino.face_loc[(side + MAXORIENT - 1) % MAXORIENT]:
      w->dino.face_loc[(side + 1) % MAXORIENT];
    w->dino.cube_loc[face][side].rotation =
      (w->dino.cube_loc[face][side].rotation + direction) % MAXORIENT;
    draw_triangle(w, face, side);
  }
}

void draw_all_polyhedrons(w)
  DinoWidget w;
{
  int face, side;

  for (face = 0; face < MAXFACES; face++)
    for (side = 0; side < MAXORIENT; side++)
      draw_triangle(w, face, side);
}

static void draw_triangle(w, face, side)
  DinoWidget w;
  int face, side;
{
  if (w->dino.dim == 2)
    draw_triangle2d((Dino2DWidget) w, face, side);
  else if (w->dino.dim == 3)
    draw_triangle3d((Dino3DWidget) w, face, side);
}

Boolean check_solved(w)
  DinoWidget w;
{
  int face, side;
  DinoCornerLoc test;

  for (face = 0; face < MAXFACES; face++)
    for (side = 0; side < MAXORIENT; side++) {
      if (side == 0) {
        test.face = w->dino.cube_loc[face][side].face;
        test.rotation = w->dino.cube_loc[face][side].rotation;
      } else if (test.face != /*face*/
               w->dino.cube_loc[face][side].face ||
               (w->dino.orient && test.rotation != /*STRT - MAXORIENT*/
                w->dino.cube_loc[face][side].rotation))
        return FALSE;
    }
  return TRUE;
}

#ifdef DEBUG

void print_cube(w)
  DinoWidget w;
{
  int face, side;

  for (face = 0; face < MAXFACES; face++) {
    for (side = 0; side < MAXORIENT; side++)
      (void) printf("%d %d  ", w->dino.cube_loc[face][side].face,
               w->dino.cube_loc[face][side].rotation);
    (void) printf("\n");
  }
  (void) printf("\n");
}

#endif
