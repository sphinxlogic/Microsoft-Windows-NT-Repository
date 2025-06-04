/*
# X-BASED SKEWB
#
#  Skewb2d.c
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

/* Methods file for Skewb2d */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include "SkewbP.h"
#include "Skewb2dP.h"

static void InitializeSkewb2D();
static void ResizeSkewb2D();
static void ExposeSkewb2D();
static Boolean SetValuesSkewb2D();
static void move_skewb2d_tl();
static void move_skewb2d_top();
static void move_skewb2d_tr();
static void move_skewb2d_left();
static void move_skewb2d_right();
static void move_skewb2d_bl();
static void move_skewb2d_bottom();
static void move_skewb2d_br();
static void resize_polyhedrons();
static void draw_frame();
static void draw_orient_line();

static char defaultTranslationsSkewb2D[] =
  "<KeyPress>q: quit()\n\
   Ctrl<KeyPress>C: quit()\n\
   <KeyPress>Home: move_tl()\n\
   <KeyPress>KP_7: move_tl()\n\
   <KeyPress>R7: move_tl()\n\
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
   <KeyPress>R1: move_bl()\n\
   <KeyPress>Down: move_bottom()\n\
   <KeyPress>KP_2: move_bottom()\n\
   <KeyPress>R14: move_bottom()\n\
   <KeyPress>Next: move_br()\n\
   <KeyPress>KP_3: move_br()\n\
   <KeyPress>R3: move_br()\n\
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
   <KeyPress>o: orientize()";

static XtActionsRec actionsListSkewb2D[] =
{
  {"quit", (XtActionProc) quit_skewb},
  {"move_tl", (XtActionProc) move_skewb2d_tl},
  {"move_top", (XtActionProc) move_skewb2d_top},
  {"move_tr", (XtActionProc) move_skewb2d_tr},
  {"move_left", (XtActionProc) move_skewb2d_left},
  {"move_cw", (XtActionProc) move_skewb_cw},
  {"move_right", (XtActionProc) move_skewb2d_right},
  {"move_bl", (XtActionProc) move_skewb2d_bl},
  {"move_bottom", (XtActionProc) move_skewb2d_bottom},
  {"move_br", (XtActionProc) move_skewb2d_br},
  {"move_ccw", (XtActionProc) move_skewb_ccw},
  {"practice", (XtActionProc) practice_skewb},
  {"maybe_practice", (XtActionProc) maybe_practice_skewb},
  {"randomize", (XtActionProc) randomize_skewb},
  {"maybe_randomize", (XtActionProc) maybe_randomize_skewb},
  {"enter", (XtActionProc) enter_skewb},
  {"write", (XtActionProc) write_skewb},
  {"undo", (XtActionProc) undo_skewb},
  {"solve", (XtActionProc) solve_skewb},
  {"orientize", (XtActionProc) orientize_skewb}
};

static XtResource resourcesSkewb2D[] =
{
  {XtNfaceColor0, XtCLabel, XtRString, sizeof(String),
   XtOffset(SkewbWidget, skewb.face_name[0]), XtRString, "Red"},
  {XtNfaceColor1, XtCLabel, XtRString, sizeof(String),
   XtOffset(SkewbWidget, skewb.face_name[1]), XtRString, "Blue"},
  {XtNfaceColor2, XtCLabel, XtRString, sizeof(String),
   XtOffset(SkewbWidget, skewb.face_name[2]), XtRString, "White"},
  {XtNfaceColor3, XtCLabel, XtRString, sizeof(String),
   XtOffset(SkewbWidget, skewb.face_name[3]), XtRString, "Green"},
  {XtNfaceColor4, XtCLabel, XtRString, sizeof(String),
   XtOffset(SkewbWidget, skewb.face_name[4]), XtRString, "Pink"},
  {XtNfaceColor5, XtCLabel, XtRString, sizeof(String),
   XtOffset(SkewbWidget, skewb.face_name[5]), XtRString, "Yellow"},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
   XtOffset(SkewbWidget, skewb.foreground), XtRString, XtDefaultForeground},
  {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
   XtOffset(SkewbWidget, core.width), XtRString, "300"},
  {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
   XtOffset(SkewbWidget, core.height), XtRString, "400"},
  {XtNorient, XtCOrient, XtRBoolean, sizeof(Boolean),
   XtOffset(SkewbWidget, skewb.orient), XtRString, "FALSE"}, /* DEFAULTORIENT */
  {XtNmono, XtCMono, XtRBoolean, sizeof(Boolean),
   XtOffset(SkewbWidget, skewb.mono), XtRString, "FALSE"},
  {XtNface, XtCFace, XtRInt, sizeof(int),
   XtOffset(SkewbWidget, skewb.face), XtRString, "-1"},
  {XtNcube, XtCCube, XtRInt, sizeof(int),
   XtOffset(SkewbWidget, skewb.cube), XtRString, "-1"},
  {XtNdirection, XtCDirection, XtRInt, sizeof(int),
   XtOffset(SkewbWidget, skewb.direction), XtRString, "-1"},
  {XtNpractice, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(SkewbWidget, skewb.practice), XtRString, "FALSE"},
  {XtNstart, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(SkewbWidget, skewb.started), XtRString, "FALSE"},
  {XtNselectCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
   XtOffset(SkewbWidget, skewb.select), XtRCallback, NULL}
};

Skewb2DClassRec skewb2dClassRec =
{
  {
    (WidgetClass) &skewbClassRec,	/* superclass */
    "Skewb2D",				/* class name */
    sizeof(Skewb2DRec),			/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeSkewb2D,			/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    actionsListSkewb2D,			/* actions */
    XtNumber(actionsListSkewb2D),	/* num actions */
    resourcesSkewb2D,			/* resources */
    XtNumber(resourcesSkewb2D),		/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    NULL,				/* destroy */
    ResizeSkewb2D,			/* resize */
    ExposeSkewb2D,			/* expose */
    SetValuesSkewb2D,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    XtInheritAcceptFocus,		/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    defaultTranslationsSkewb2D,		/* tm table */
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

WidgetClass skewb2dWidgetClass = (WidgetClass) &skewb2dClassRec;

static int plane_to_cube[MAXRECT] = {6, 0, 6, 1, 2, 3, 6, 4, 6, 6, 5, 6};
static int cube_to_plane[MAXFACES] = {1, 3, 4, 5, 7, 10};
static XPoint diamond_unit[MAXORIENT] = {{0, 1}, {1, -1}, {1, 1}, {-1, 1}},
  triangle_unit[MAXORIENT][3] = {
                                  {{2, 0},  {1, 0},  {0, 1}},
                                  {{3, 2},  {0, 1}, {-1, 0}},
                                  {{1, 3}, {-1, 0}, {0, -1}},
                                  {{0, 1}, {0, -1},  {1, 0}}
                                },
  letter_unit[MAXCUBES] = {{2, 0}, {2, 2}, {0, 2}, {0, 0}, {1, 1}};
static XPoint diamond_list[MAXORIENT], triangle_list[MAXORIENT][3],
  letter_list[MAXCUBES], offset_list[MAXCUBES];

static void InitializeSkewb2D(request, new)
  Widget request, new;
{
  Skewb2DWidget w = (Skewb2DWidget) new;

  w->skewb.dim = 2;
  ResizeSkewb2D(w);
}

static void ResizeSkewb2D(w)
  Skewb2DWidget w;
{
  w->skewb.delta = 3;
  w->skewb.vertical = (w->core.height >= w->core.width);
  if (w->skewb.vertical)
    w->skewb2d.face_length = MIN(w->core.height / MAXY, w->core.width / MAXX);
  else
    w->skewb2d.face_length = MIN(w->core.height / MAXX, w->core.width / MAXY);
  w->skewb2d.face_length = MAX(w->skewb2d.face_length - w->skewb.delta - 1, 0);
  w->skewb2d.diamond_length = w->skewb2d.face_length - w->skewb.delta;
  w->skewb2d.view_length = w->skewb2d.face_length + w->skewb.delta;
  if (w->skewb.vertical) {
    w->skewb.puzzle_width = MAXX * (w->skewb2d.view_length - 1) +
      w->skewb.delta;
    w->skewb.puzzle_height = MAXY * (w->skewb2d.view_length - 1) +
      w->skewb.delta;
  } else {
    w->skewb.puzzle_width = MAXY * (w->skewb2d.view_length - 1) +
      w->skewb.delta;
    w->skewb.puzzle_height = MAXX * (w->skewb2d.view_length - 1) +
      w->skewb.delta;
  }
  w->skewb.puzzle_offset.x = ((int) w->core.width - w->skewb.puzzle_width)
    / 2;
  w->skewb.puzzle_offset.y = ((int) w->core.height - w->skewb.puzzle_height)
    / 2;
  resize_polyhedrons(w);
}

static void ExposeSkewb2D(new, event, region)
  Widget new;
  XEvent *event;
  Region region; /* Not used */
{
  Skewb2DWidget w = (Skewb2DWidget) new;

  if (w->core.visible) {
    draw_frame(w, w->skewb.puzzle_GC);
    draw_all_polyhedrons((SkewbWidget) w);
  }
}

static Boolean SetValuesSkewb2D(current, request, new)
  Widget current, request, new;
{
  Skewb2DWidget c = (Skewb2DWidget) current, w = (Skewb2DWidget) new;
  Boolean redraw = FALSE;

  if (w->skewb2d.diamond_length != c->skewb2d.diamond_length) {
    ResizeSkewb2D(w);
    redraw = TRUE;
  }
  return (redraw);
}

static void move_skewb2d_tl(w, event, args, n_args)
  Skewb2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, TL,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb2d_top(w, event, args, n_args)
  Skewb2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, TOP,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb2d_tr(w, event, args, n_args)
  Skewb2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, TR,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb2d_left(w, event, args, n_args)
  Skewb2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, LEFT,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb2d_right(w, event, args, n_args)
  Skewb2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, RIGHT,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb2d_bl(w, event, args, n_args)
  Skewb2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, BL,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb2d_bottom(w, event, args, n_args)
  Skewb2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, BOTTOM,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb2d_br(w, event, args, n_args)
  Skewb2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, BR,
    (int) (event->xkey.state & ControlMask));
}

static void resize_polyhedrons(w)
  Skewb2DWidget w;
{
  int i, j;

  w->skewb.orient_line_length = w->skewb2d.diamond_length / 8;
  w->skewb.letter_offset.x = -2;
  w->skewb.letter_offset.y = 4;
  for (i = 0; i < MAXORIENT; i++)
  {
    diamond_list[i].x = diamond_unit[i].x * (w->skewb2d.diamond_length / 2 -
      w->skewb.delta);
    diamond_list[i].y = diamond_unit[i].y * (w->skewb2d.diamond_length / 2 -
      w->skewb.delta);
    for (j = 0; j < 3; j++)
    {
      triangle_list[i][j].x = triangle_unit[i][j].x *
        (w->skewb2d.diamond_length / 2 - 3 * w->skewb.delta);
      triangle_list[i][j].y = triangle_unit[i][j].y *
        (w->skewb2d.diamond_length / 2 - 3 * w->skewb.delta);
    }
    if (letter_unit[i].x == 0)
      letter_list[i].x = w->skewb2d.diamond_length / 8 +
        w->skewb.letter_offset.x;
    else if (letter_unit[i].x == 2)
      letter_list[i].x = 7 * w->skewb2d.diamond_length / 8 - 2 +
        w->skewb.letter_offset.x;
    if (letter_unit[i].y == 0)
      letter_list[i].y = w->skewb2d.diamond_length / 8 + 2 +
        w->skewb.letter_offset.y;
    else if (letter_unit[i].y == 2)
      letter_list[i].y = 7 * w->skewb2d.diamond_length / 8 - 3 +
        w->skewb.letter_offset.y;

    if (triangle_unit[i][0].x == 0)
      offset_list[i].x = w->skewb.delta - 1;
    else if (triangle_unit[i][0].x == 1)
      offset_list[i].x = w->skewb2d.diamond_length / 2 -
        2 * w->skewb.delta - 1;
    else if (triangle_unit[i][0].x == 2)
      offset_list[i].x = w->skewb2d.diamond_length / 2 +
        2 * w->skewb.delta;
    else if (triangle_unit[i][0].x == 3)
      offset_list[i].x = w->skewb2d.diamond_length - w->skewb.delta - 1;
    if (triangle_unit[i][0].y == 0)
      offset_list[i].y = w->skewb.delta - 1;
    else if (triangle_unit[i][0].y == 1)
      offset_list[i].y = w->skewb2d.diamond_length / 2 -
        2 * w->skewb.delta - 1;
    else if (triangle_unit[i][0].y == 2)
      offset_list[i].y = w->skewb2d.diamond_length / 2 +
        2 * w->skewb.delta - 1;
    else if (triangle_unit[i][0].y == 3)
      offset_list[i].y = w->skewb2d.diamond_length - w->skewb.delta - 2;
   }
  if (diamond_unit[0].x == 0)
    offset_list[MAXORIENT].x = w->skewb.delta - 2;
  else if (diamond_unit[0].x == 1)
    offset_list[MAXORIENT].x = w->skewb2d.diamond_length / 2 - 1;
  if (diamond_unit[0].y == 0)
    offset_list[MAXORIENT].y = w->skewb.delta - 2;
  else if (diamond_unit[0].y == 1)
    offset_list[MAXORIENT].y = w->skewb2d.diamond_length / 2 - 2;
  if (letter_unit[MAXORIENT].x == 1)
    letter_list[MAXORIENT].x = w->skewb2d.diamond_length / 2 - 2 +
      w->skewb.letter_offset.x;
  if (letter_unit[MAXORIENT].y == 1)
    letter_list[MAXORIENT].y = w->skewb2d.diamond_length / 2 - 2 +
      w->skewb.letter_offset.y;
}

int position_polyhedrons2d(w, x, y, face, corner, direction)
  Skewb2DWidget w;
  int x, y;
  int *face, *corner, *direction;
{
  int face_x, face_y;
  int i, j;

  x -= w->skewb.puzzle_offset.x;
  y -= w->skewb.puzzle_offset.y;
  face_x = x / w->skewb2d.view_length;
  face_y = y / w->skewb2d.view_length;
  i = x - face_x * w->skewb2d.view_length;
  j = y - face_y * w->skewb2d.view_length;
  if (i - j > w->skewb2d.view_length / 2 - 3)
    *corner = TR;
  else if (i + j > 3 * w->skewb2d.view_length / 2)
    *corner = BR;
  else if (j - i > w->skewb2d.view_length / 2 - 2)
    *corner = BL;
  else if (i + j < w->skewb2d.view_length / 2 + 7)
    *corner = TL;
  else if (*direction < 2 * MAXORIENT)
     /* *corner == MAXORIENT : this is ambiguous */
     return FALSE;
  if (*direction == CW)
    *direction = (*corner + 1) % MAXORIENT;
  else if (*direction == CCW)
    *direction = (*corner + 3) % MAXORIENT;
  if ((face_x != 1 && face_y != 1) ||
      (face_x >= 3 && w->skewb.vertical) ||
      (face_y >= 3 && !w->skewb.vertical) ||
      ((*corner + *direction) % 2 == 0 && *direction < 2 * MAXORIENT))
    return FALSE;
  *face = plane_to_cube[face_x + face_y * MAXX];
  if (face_x == 3) {
    *face = MAXFACES - 1;
    *corner = (*corner + HALF) % MAXORIENT;
    if (*direction < MAXORIENT)
      *direction = (*direction + HALF) % MAXORIENT;
    else if (*direction >= 2 * MAXORIENT)
      *direction = 2 * MAXORIENT + (*direction + HALF) % MAXORIENT;
  }
  return TRUE;
}

static void draw_frame(w, gc)
  Skewb2DWidget w;
  GC gc;
{
  int i;
  XPoint pos[MAXXY + 1], letters;
  
  for (i = 0; i <= MAXXY; i++) {
    pos[i].x = i * w->skewb2d.view_length + w->skewb.puzzle_offset.x;
    pos[i].y = i * w->skewb2d.view_length + w->skewb.puzzle_offset.y;
  }
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    pos[1].x, pos[0].y, pos[2].x, pos[0].y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    pos[3].x, pos[1].y, pos[3].x, pos[2].y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    pos[1].x, pos[3].y, pos[2].x, pos[3].y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    pos[0].x, pos[1].y, pos[0].x, pos[2].y);
  letters.x = pos[0].x + w->skewb2d.view_length / 2 - w->skewb.delta;
  letters.y = pos[0].y + w->skewb2d.view_length / 2;
  XDrawString(XtDisplay(w), XtWindow(w), gc,
    (int) (letters.x + 5 * w->skewb.letter_offset.x),
    (int) (letters.y + w->skewb.letter_offset.y), "Front", 5);
  letters.x = pos[2].x + w->skewb2d.view_length / 2 - w->skewb.delta;
  letters.y = pos[2].y + w->skewb2d.view_length / 2;
  XDrawString(XtDisplay(w), XtWindow(w), gc,
    (int) (letters.x + 4 * w->skewb.letter_offset.x),
    (int) (letters.y + w->skewb.letter_offset.y), "Back", 4);
  if (w->skewb.vertical) {
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

void draw_diamond2d(w, face)
  Skewb2DWidget w;
  int face;
{
  int dx, dy, orient;

  orient = w->skewb.cube_loc[face][MAXORIENT].rotation;
  if (w->skewb.vertical || face != MAXFACES - 1) {
    dx = (cube_to_plane[face] % MAXX) * w->skewb2d.view_length +
      w->skewb.delta;
    dy = (cube_to_plane[face] / MAXX) * w->skewb2d.view_length +
      w->skewb.delta;
  } else {
    dx = (cube_to_plane[face] / MAXX) * w->skewb2d.view_length +
      w->skewb.delta;
    dy = (cube_to_plane[face] % MAXX) * w->skewb2d.view_length +
      w->skewb.delta;
    orient = (orient + HALF) % STRT;
  }
  dx += w->skewb.puzzle_offset.x + w->skewb.delta;
  dy += w->skewb.puzzle_offset.y + w->skewb.delta;
  diamond_list[0].x = offset_list[MAXORIENT].x + dx;
  diamond_list[0].y = offset_list[MAXORIENT].y + dy;
  XFillPolygon(XtDisplay(w), XtWindow(w),
     w->skewb.face_GC[w->skewb.cube_loc[face][MAXORIENT].face],
     diamond_list, 4, Convex, CoordModePrevious);
  if (w->skewb.depth == 1 || w->skewb.mono) {
    int letter_x, letter_y;
    char buf[2];

    (void) sprintf(buf, "%c",
             w->skewb.face_name[w->skewb.cube_loc[face][MAXORIENT].face][0]);
    letter_x = dx + letter_list[MAXORIENT].x;
    letter_y = dy + letter_list[MAXORIENT].y;
    XDrawString(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
      letter_x, letter_y, buf, 1);
  }
  if (w->skewb.orient)
    draw_orient_line(w, MAXORIENT, orient, dx, dy);
}

void draw_triangle2d(w, face, corner)
  Skewb2DWidget w;
  int face, corner;
{
  int dx, dy, letter_x, letter_y, orient, new_corner;

  orient = w->skewb.cube_loc[face][corner].rotation;
  if (w->skewb.vertical || face != MAXFACES - 1) {
    dx = (cube_to_plane[face] % MAXX) * w->skewb2d.view_length +
      w->skewb.delta - 1;
    dy = (cube_to_plane[face] / MAXX) * w->skewb2d.view_length +
      w->skewb.delta - 1;
    new_corner = corner;
  } else {
    dx = (cube_to_plane[face] / MAXX) * w->skewb2d.view_length +
      w->skewb.delta - 1;
    dy = (cube_to_plane[face] % MAXX) * w->skewb2d.view_length +
      w->skewb.delta - 1;
    new_corner = (corner + HALF) % STRT;
    orient = (orient + HALF) % STRT;
  }
  dx += w->skewb.puzzle_offset.x + w->skewb.delta;
  dy += w->skewb.puzzle_offset.y + w->skewb.delta;
  letter_x = dx + letter_list[new_corner].x;
  letter_y = dy + letter_list[new_corner].y;
  triangle_list[new_corner][0].x = offset_list[new_corner].x + dx;
  triangle_list[new_corner][0].y = offset_list[new_corner].y + dy;
  XFillPolygon(XtDisplay(w), XtWindow(w),
    w->skewb.face_GC[w->skewb.cube_loc[face][corner].face],
    triangle_list[new_corner], 3, Convex, CoordModePrevious);
  if (w->skewb.depth == 1 || w->skewb.mono) {
    char buf[2];

    (void) sprintf(buf, "%c",
             w->skewb.face_name[w->skewb.cube_loc[face][corner].face][0]);
    XDrawString(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
      letter_x, letter_y, buf, 1);
  }
  if (w->skewb.orient)
    draw_orient_line(w, new_corner, orient,
      letter_x - w->skewb.letter_offset.x, letter_y - w->skewb.letter_offset.y);
}

static void draw_orient_line(w, cube, orient, dx, dy)
  Skewb2DWidget w;
  int cube, orient, dx, dy;
{
  if (cube == MAXORIENT)
    switch (orient) {
      case TR:
        XDrawLine(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
                  dx + w->skewb2d.diamond_length / 2 - 2,
                  dy + w->skewb.delta - 2,
                  dx + w->skewb2d.diamond_length / 2 - 2,
                  dy + w->skewb.delta + w->skewb.orient_line_length);
        return;
      case BR:
        XDrawLine(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
                  dx + w->skewb2d.diamond_length - 2,
                  dy + w->skewb2d.diamond_length / 2 - 2,
                  dx + w->skewb2d.diamond_length -
                    w->skewb.orient_line_length - 7,
                  dy + w->skewb2d.diamond_length / 2 - 2);
        return;
      case BL:
        XDrawLine(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
                  dx + w->skewb2d.diamond_length / 2 - 2,
                  dy + w->skewb2d.diamond_length - 2,
                  dx + w->skewb2d.diamond_length / 2 - 2,
                  dy + w->skewb2d.diamond_length -
                    w->skewb.orient_line_length - 7);
        return;
      case TL:
        XDrawLine(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
                  dx + w->skewb.delta - 2,
                  dy + w->skewb2d.diamond_length / 2 - 2,
                  dx + w->skewb.delta + w->skewb.orient_line_length,
                  dy + w->skewb2d.diamond_length / 2 - 2);
        return;
      default:
        (void) printf("draw_orient_line: orient %d\n", orient);
    }
  else /* cube != MAXORIENT */
    switch (orient) {
      case TR:
        XDrawLine(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
                  dx, 
                  dy - w->skewb.delta,
                  dx,
                  dy - w->skewb.delta - w->skewb.orient_line_length / 2);
        return;
      case BR:
        XDrawLine(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
                  dx + w->skewb.delta,
                  dy,
                  dx + w->skewb.delta + w->skewb.orient_line_length / 2,
                  dy);
        return;
      case BL:
        XDrawLine(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
                  dx,
                  dy + w->skewb.delta,
                  dx,
                  dy + w->skewb.delta + w->skewb.orient_line_length / 2);
        return;
      case TL:
        XDrawLine(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
                  dx - w->skewb.delta,
                  dy,
                  dx - w->skewb.delta - w->skewb.orient_line_length / 2,
                  dy);
        return;
      default:
        (void) printf("draw_orient_line: orient %d\n", orient);
    }
}
