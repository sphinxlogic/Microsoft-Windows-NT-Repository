/*
# X-BASED RUBIK'S CUBE(tm)
#
#  Rubik2d.c
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

/* Methods file for Rubik2d */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include "RubikP.h"
#include "Rubik2dP.h"

static void InitializeRubik2D();
static void ResizeRubik2D();
static void ExposeRubik2D();
static Boolean SetValuesRubik2D();
static void move_rubik2d_top();
static void move_rubik2d_left();
static void move_rubik2d_right();
static void move_rubik2d_bottom();
static void resize_polyhedrons();
static void draw_frame();
static void draw_orient_line();

static char defaultTranslationsRubik2D[] =
  "<KeyPress>q: quit()\n\
   Ctrl<KeyPress>C: quit()\n\
   <KeyPress>Up: move_top()\n\
   <KeyPress>KP_8: move_top()\n\
   <KeyPress>R8: move_top()\n\
   <KeyPress>Left: move_left()\n\
   <KeyPress>KP_4: move_left()\n\
   <KeyPress>R10: move_left()\n\
   <KeyPress>Begin: move_cw()\n\
   <KeyPress>KP_5: move_cw()\n\
   <KeyPress>R11: move_cw()\n\
   <KeyPress>Right: move_right()\n\
   <KeyPress>KP_6: move_right()\n\
   <KeyPress>R12: move_right()\n\
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
   <KeyPress>o: orientize()";

static XtActionsRec actionsListRubik2D[] =
{
  {"quit", (XtActionProc) quit_rubik},
  {"move_top", (XtActionProc) move_rubik2d_top},
  {"move_left", (XtActionProc) move_rubik2d_left},
  {"move_cw", (XtActionProc) move_rubik_cw},
  {"move_right", (XtActionProc) move_rubik2d_right},
  {"move_bottom", (XtActionProc) move_rubik2d_bottom},
  {"move_ccw", (XtActionProc) move_rubik_ccw},
  {"practice", (XtActionProc) practice_rubik},
  {"maybe_practice", (XtActionProc) maybe_practice_rubik},
  {"randomize", (XtActionProc) randomize_rubik},
  {"maybe_randomize", (XtActionProc) maybe_randomize_rubik},
  {"enter", (XtActionProc) enter_rubik},
  {"write", (XtActionProc) write_rubik},
  {"undo", (XtActionProc) undo_rubik},
  {"solve", (XtActionProc) solve_rubik},
  {"increment", (XtActionProc) increment_rubik},
  {"decrement", (XtActionProc) decrement_rubik},
  {"orientize", (XtActionProc) orientize_rubik}
};

static XtResource resourcesRubik2D[] =
{
  {XtNfaceColor0, XtCLabel, XtRString, sizeof(String),
   XtOffset(RubikWidget, rubik.face_name[0]), XtRString, "Red"},
  {XtNfaceColor1, XtCLabel, XtRString, sizeof(String),
   XtOffset(RubikWidget, rubik.face_name[1]), XtRString, "Yellow"},
  {XtNfaceColor2, XtCLabel, XtRString, sizeof(String),
   XtOffset(RubikWidget, rubik.face_name[2]), XtRString, "White"},
  {XtNfaceColor3, XtCLabel, XtRString, sizeof(String),
   XtOffset(RubikWidget, rubik.face_name[3]), XtRString, "Green"},
  {XtNfaceColor4, XtCLabel, XtRString, sizeof(String),
   XtOffset(RubikWidget, rubik.face_name[4]), XtRString, "Orange"},
  {XtNfaceColor5, XtCLabel, XtRString, sizeof(String),
   XtOffset(RubikWidget, rubik.face_name[5]), XtRString, "Blue"},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
   XtOffset(RubikWidget, rubik.foreground), XtRString, XtDefaultForeground},
  {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
   XtOffset(RubikWidget, core.width), XtRString, "300"},
  {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
   XtOffset(RubikWidget, core.height), XtRString, "400"},
  {XtNsize, XtCSize, XtRInt, sizeof(int),
   XtOffset(RubikWidget, rubik.size), XtRString, "3"}, /* DEFAULTCUBES */
  {XtNorient, XtCOrient, XtRBoolean, sizeof(Boolean),
   XtOffset(RubikWidget, rubik.orient), XtRString, "FALSE"}, /* DEFAULTORIENT */
  {XtNmono, XtCMono, XtRBoolean, sizeof(Boolean),
   XtOffset(RubikWidget, rubik.mono), XtRString, "FALSE"},
  {XtNface, XtCFace, XtRInt, sizeof(int),
   XtOffset(RubikWidget, rubik.face), XtRString, "-1"},
  {XtNi, XtCI, XtRInt, sizeof(int),
   XtOffset(RubikWidget, rubik.i), XtRString, "-1"},
  {XtNj, XtCJ, XtRInt, sizeof(int),
   XtOffset(RubikWidget, rubik.j), XtRString, "-1"},
  {XtNdirection, XtCDirection, XtRInt, sizeof(int),
   XtOffset(RubikWidget, rubik.direction), XtRString, "-1"},
  {XtNpractice, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(RubikWidget, rubik.practice), XtRString, "FALSE"},
  {XtNstart, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(RubikWidget, rubik.started), XtRString, "FALSE"},
  {XtNselectCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
   XtOffset(RubikWidget, rubik.select), XtRCallback, NULL}
};

Rubik2DClassRec rubik2dClassRec =
{
  {
    (WidgetClass) &rubikClassRec,	/* superclass */
    "Rubik2D",				/* class name */
    sizeof(Rubik2DRec),			/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeRubik2D,			/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    actionsListRubik2D,			/* actions */
    XtNumber(actionsListRubik2D),	/* num actions */
    resourcesRubik2D,			/* resources */
    XtNumber(resourcesRubik2D),		/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    NULL,				/* destroy */
    ResizeRubik2D,			/* resize */
    ExposeRubik2D,			/* expose */
    SetValuesRubik2D,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    XtInheritAcceptFocus,		/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    defaultTranslationsRubik2D,		/* tm table */
    NULL,				/* query geometry */
    NULL,				/* display accelerator */
    NULL				/* extension */
  },
  {
    0					/* ignore */
  },
  {
    0					/* ignore */
  }
};

WidgetClass rubik2dWidgetClass = (WidgetClass) &rubik2dClassRec;

static RowNext rotate_to_row[MAXFACES] = /*CW to min face*/
{
  {1,   LEFT,    TOP},
  {0, BOTTOM,  RIGHT},
  {0,  RIGHT, BOTTOM},
  {0,    TOP,   LEFT},
  {1,  RIGHT, BOTTOM},
  {0,   LEFT,    TOP}
};
static int plane_to_cube[MAXRECT] = {6, 0, 6, 1, 2, 3, 6, 4, 6, 6, 5, 6};
static int cube_to_plane[MAXFACES] = {1, 3, 4, 5, 7, 10};

static void InitializeRubik2D(request, new)
  Widget request, new;
{
  Rubik2DWidget w = (Rubik2DWidget) new;

  w->rubik.dim = 2;
  ResizeRubik2D(w);
}

static void ResizeRubik2D(w)
  Rubik2DWidget w;
{
  int temp_length;

  w->rubik.delta = 3;
  w->rubik.vertical = (w->core.height >= w->core.width);
  if (w->rubik.vertical)
    temp_length = MIN(w->core.height / MAXY, w->core.width / MAXX);
  else
    temp_length = MIN(w->core.height / MAXX, w->core.width / MAXY);
  w->rubik2d.cube_length = MAX((temp_length - w->rubik.delta - 1) /
     w->rubik.size, 0);
  w->rubik2d.face_length = w->rubik.size * w->rubik2d.cube_length;
  w->rubik2d.view_length = w->rubik2d.face_length + w->rubik.delta;
  if (w->rubik.vertical) {
    w->rubik.puzzle_width = MAXX * (w->rubik2d.view_length - 1) +
      w->rubik.delta;
    w->rubik.puzzle_height = MAXY * (w->rubik2d.view_length - 1) +
      w->rubik.delta;
  } else {
    w->rubik.puzzle_width = MAXY * (w->rubik2d.view_length - 1) +
      w->rubik.delta;
    w->rubik.puzzle_height = MAXX * (w->rubik2d.view_length - 1) +
      w->rubik.delta;
  }
  w->rubik.puzzle_offset.x = ((int) w->core.width - w->rubik.puzzle_width)
    / 2;
  w->rubik.puzzle_offset.y = ((int) w->core.height - w->rubik.puzzle_height)
    / 2;
  resize_polyhedrons(w);
}

static void ExposeRubik2D(new, event, region)
  Widget new;
  XEvent *event;
  Region region; /* Not used */
{
  Rubik2DWidget w = (Rubik2DWidget) new;

  if (w->core.visible) {
    draw_frame(w, w->rubik.puzzle_GC);
    draw_all_polyhedrons((RubikWidget) w);
  }
}

static Boolean SetValuesRubik2D(current, request, new)
  Widget current, request, new;
{
  Rubik2DWidget c = (Rubik2DWidget) current, w = (Rubik2DWidget) new;
  Boolean redraw = FALSE;

  if (w->rubik.size != c->rubik.size) {
    reset_polyhedrons((RubikWidget) w);
    w->rubik.practice = FALSE;
    ResizeRubik2D(w);
    redraw = TRUE;
  }
  if (w->rubik2d.cube_length != c->rubik2d.cube_length) {
    ResizeRubik2D(w);
    redraw = TRUE;
  }
  return (redraw);
}

static void move_rubik2d_top(w, event, args, n_args)
  Rubik2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input((RubikWidget) w, event->xbutton.x, event->xbutton.y, TOP,
    (int) (event->xkey.state & ControlMask));
}

static void move_rubik2d_left(w, event, args, n_args)
  Rubik2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input((RubikWidget) w, event->xbutton.x, event->xbutton.y, LEFT,
    (int) (event->xkey.state & ControlMask));
}

static void move_rubik2d_right(w, event, args, n_args)
  Rubik2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input((RubikWidget) w, event->xbutton.x, event->xbutton.y, RIGHT,
    (int) (event->xkey.state & ControlMask));
}

static void move_rubik2d_bottom(w, event, args, n_args)
  Rubik2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input((RubikWidget) w, event->xbutton.x, event->xbutton.y, BOTTOM,
    (int) (event->xkey.state & ControlMask));
}

static void resize_polyhedrons(w)
  Rubik2DWidget w;
{
  w->rubik2d.cube_length = MAX(w->rubik2d.face_length /
    w->rubik.size - w->rubik.delta + 1, 0);
  w->rubik.orient_line_length = w->rubik2d.cube_length / 4;
  w->rubik.letter_offset.x = -2;
  w->rubik.letter_offset.y = 4;
}

int position_polyhedrons2d(w, x, y, face, i, j, direction)
  Rubik2DWidget w;
  int x, y;
  int *face, *i, *j, *direction;
{
  int face_x, face_y;

  x -= w->rubik.puzzle_offset.x;
  y -= w->rubik.puzzle_offset.y;
  face_x = x / w->rubik2d.view_length;
  face_y = y / w->rubik2d.view_length;
  *i = MAX((x - face_x * w->rubik2d.view_length - w->rubik.delta)  /
    (w->rubik2d.cube_length + w->rubik.delta - 1), 0); 
  *j = MAX((y - face_y * w->rubik2d.view_length - w->rubik.delta)  /
     (w->rubik2d.cube_length + w->rubik.delta - 1), 0);
  if ((face_x != 1 && face_y != 1) || 
      (face_x >= 3 && w->rubik.vertical) ||
      (face_y >= 3 && !w->rubik.vertical))
    return FALSE;
  if (*i >= w->rubik.size)
    *i = w->rubik.size - 1;
  if (*j >= w->rubik.size)
    *j = w->rubik.size - 1;
  *face = plane_to_cube[face_x + face_y * MAXX];
  if (face_x == 3) {
    *face = MAXFACES - 1;
    *i = w->rubik.size - 1 - *i;
    *j = w->rubik.size - 1 - *j;
    if (*direction < MAXORIENT)
      *direction = (*direction + HALF) % MAXORIENT;
  }
  /* Remap to row movement */
  if (*direction == CW || *direction == CCW) {
    *direction = (*direction == CCW) ?
      (rotate_to_row[*face].direction + 2) % MAXORIENT :
      rotate_to_row[*face].direction; 
    *i = *j = (rotate_to_row[*face].side_face == LEFT ||
               rotate_to_row[*face].side_face == BOTTOM) ?
      w->rubik.size - 1 : 0;
    *face = rotate_to_row[*face].face;
  }
  return TRUE;
}

static void draw_frame(w, gc)
  Rubik2DWidget w;
  GC gc;
{
  int i;
  XPoint pos[MAXXY + 1], letters;
  
  for (i = 0; i <= MAXXY; i++) {
    pos[i].x = i * w->rubik2d.view_length + w->rubik.puzzle_offset.x;
    pos[i].y = i * w->rubik2d.view_length + w->rubik.puzzle_offset.y;
  }
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    pos[1].x, pos[0].y, pos[2].x, pos[0].y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    pos[3].x, pos[1].y, pos[3].x, pos[2].y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    pos[1].x, pos[3].y, pos[2].x, pos[3].y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    pos[0].x, pos[1].y, pos[0].x, pos[2].y);
  letters.x = pos[0].x + w->rubik2d.view_length / 2 - w->rubik.delta;
  letters.y = pos[0].y + w->rubik2d.view_length / 2;
  XDrawString(XtDisplay(w), XtWindow(w), gc,
    (int) (letters.x + 5 * w->rubik.letter_offset.x),
    (int) (letters.y + w->rubik.letter_offset.y), "Front", 5);
  letters.x = pos[2].x + w->rubik2d.view_length / 2 - w->rubik.delta;
  letters.y = pos[2].y + w->rubik2d.view_length / 2;
  XDrawString(XtDisplay(w), XtWindow(w), gc,
    (int) (letters.x + 4 * w->rubik.letter_offset.x),
    (int) (letters.y + w->rubik.letter_offset.y), "Back", 4);
  if (w->rubik.vertical) {
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      pos[1].x, pos[0].y, pos[1].x, pos[4].y);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      pos[2].x, pos[0].y, pos[2].x, pos[4].y);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      pos[0].x, pos[1].y, pos[3].x, pos[1].y);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      pos[0].x, pos[2].y, pos[3].x, pos[2].y);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      pos[1].x, pos[4].y, pos[2].x, pos[4].y);
  } else {
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      pos[0].x, pos[1].y, pos[4].x, pos[1].y);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      pos[0].x, pos[2].y, pos[4].x, pos[2].y);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      pos[1].x, pos[0].y, pos[1].x, pos[3].y);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      pos[2].x, pos[0].y, pos[2].x, pos[3].y);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      pos[4].x, pos[1].y, pos[4].x, pos[2].y);
  }
}   

void draw_square2d(w, face, i, j)
  Rubik2DWidget w;
  int face, i, j;
{
  int dx, dy, orient;

  orient = w->rubik.cube_loc[face][i][j].rotation;
  if (w->rubik.vertical || face != MAXFACES - 1) {
    dx = (cube_to_plane[face] % MAXX) * w->rubik2d.view_length +
      i * (w->rubik2d.cube_length + w->rubik.delta - 1);
    dy = (cube_to_plane[face] / MAXX) * w->rubik2d.view_length +
      j * (w->rubik2d.cube_length + w->rubik.delta - 1);
  } else {
    dx = (cube_to_plane[face] / MAXX) * w->rubik2d.view_length +
      (w->rubik.size - 1 - i) * (w->rubik2d.cube_length + w->rubik.delta -
      1);
    dy = (cube_to_plane[face] % MAXX) * w->rubik2d.view_length +
      (w->rubik.size - 1 - j) * (w->rubik2d.cube_length + w->rubik.delta -
      1);
    orient = (orient + HALF) % STRT;
  }
  dx += w->rubik.puzzle_offset.x + w->rubik.delta;
  dy += w->rubik.puzzle_offset.y + w->rubik.delta;
  XFillRectangle (XtDisplay(w), XtWindow(w),
     w->rubik.face_GC[w->rubik.cube_loc[face][i][j].face],
     dx, dy, w->rubik2d.cube_length, w->rubik2d.cube_length);
  if (w->rubik.depth == 1 || w->rubik.mono) {
    int letter_x, letter_y;
    char buf[2];

    (void) sprintf(buf, "%c",
             w->rubik.face_name[w->rubik.cube_loc[face][i][j].face][0]);
    letter_x = dx + w->rubik2d.cube_length / 2 + w->rubik.letter_offset.x;
    letter_y = dy + w->rubik2d.cube_length / 2 + w->rubik.letter_offset.y;
    XDrawString(XtDisplay(w), XtWindow(w), w->rubik.inverse_GC,
      letter_x, letter_y, buf, 1);
  }
  if (w->rubik.orient)
    draw_orient_line(w, orient, dx, dy);
}

static void draw_orient_line(w, orient, dx, dy)
  Rubik2DWidget w;
  int orient, dx, dy;
{
  switch (orient) {
    case TOP:
      XDrawLine (XtDisplay(w), XtWindow(w), w->rubik.inverse_GC,
                 dx + w->rubik2d.cube_length / 2,
                 dy,
                 dx + w->rubik2d.cube_length / 2,
                 dy + w->rubik.orient_line_length);
      return;
    case RIGHT:
      XDrawLine (XtDisplay(w), XtWindow(w), w->rubik.inverse_GC,
                 dx + w->rubik2d.cube_length,
                 dy + w->rubik2d.cube_length / 2,
                 dx + w->rubik2d.cube_length - w->rubik.orient_line_length -
                   1,
                 dy + w->rubik2d.cube_length / 2);
      return;
    case BOTTOM:
      XDrawLine (XtDisplay(w), XtWindow(w), w->rubik.inverse_GC,
                 dx + w->rubik2d.cube_length / 2,
                 dy + w->rubik2d.cube_length,
                 dx + w->rubik2d.cube_length / 2,
                 dy + w->rubik2d.cube_length - w->rubik.orient_line_length -
                   1);
      return;
    case LEFT:
      XDrawLine (XtDisplay(w), XtWindow(w), w->rubik.inverse_GC,
                 dx,
                 dy + w->rubik2d.cube_length / 2,
                 dx + w->rubik.orient_line_length,
                 dy + w->rubik2d.cube_length / 2);
      return;
    default:
      (void) printf ("draw_orient_line: orient %d\n", orient);
  }
}
