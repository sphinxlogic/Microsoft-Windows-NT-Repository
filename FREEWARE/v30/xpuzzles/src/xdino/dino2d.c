/*
# X-BASED DINO
#
#  Dino2d.c
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

/* Methods file for Dino2d */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include "DinoP.h"
#include "Dino2dP.h"

static void InitializeDino2D();
static void ResizeDino2D();
static void ExposeDino2D();
static Boolean SetValuesDino2D();
static void move_dino2d_tl();
static void move_dino2d_top();
static void move_dino2d_tr();
static void move_dino2d_left();
static void move_dino2d_right();
static void move_dino2d_bl();
static void move_dino2d_bottom();
static void move_dino2d_br();
static void resize_polyhedrons();
static void draw_frame();
static void draw_orient_line();

static char defaultTranslationsDino2D[] =
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
   <KeyPress>o: orientize()\n\
   <KeyPress>2: period2()\n\
   <KeyPress>3: period3()\n\
   <KeyPress>b: both()";

static XtActionsRec actionsListDino2D[] =
{
  {"quit", (XtActionProc) quit_dino},
  {"move_tl", (XtActionProc) move_dino2d_tl},
  {"move_top", (XtActionProc) move_dino2d_top},
  {"move_tr", (XtActionProc) move_dino2d_tr},
  {"move_left", (XtActionProc) move_dino2d_left},
  {"move_right", (XtActionProc) move_dino2d_right},
  {"move_bl", (XtActionProc) move_dino2d_bl},
  {"move_bottom", (XtActionProc) move_dino2d_bottom},
  {"move_br", (XtActionProc) move_dino2d_br},
  {"practice", (XtActionProc) practice_dino},
  {"maybe_practice", (XtActionProc) maybe_practice_dino},
  {"randomize", (XtActionProc) randomize_dino},
  {"maybe_randomize", (XtActionProc) maybe_randomize_dino},
  {"enter", (XtActionProc) enter_dino},
  {"write", (XtActionProc) write_dino},
  {"undo", (XtActionProc) undo_dino},
  {"solve", (XtActionProc) solve_dino},
  {"orientize", (XtActionProc) orientize_dino},
  {"period2", (XtActionProc) period2_mode_dino},
  {"period3", (XtActionProc) period3_mode_dino},
  {"both", (XtActionProc) both_mode_dino}
};

static XtResource resourcesDino2D[] =
{
  {XtNfaceColor0, XtCLabel, XtRString, sizeof(String),
   XtOffset(DinoWidget, dino.face_name[0]), XtRString, "Orange"},
  {XtNfaceColor1, XtCLabel, XtRString, sizeof(String),
   XtOffset(DinoWidget, dino.face_name[1]), XtRString, "Pink"},
  {XtNfaceColor2, XtCLabel, XtRString, sizeof(String),
   XtOffset(DinoWidget, dino.face_name[2]), XtRString, "White"},
  {XtNfaceColor3, XtCLabel, XtRString, sizeof(String),
   XtOffset(DinoWidget, dino.face_name[3]), XtRString, "Violet"},
  {XtNfaceColor4, XtCLabel, XtRString, sizeof(String),
   XtOffset(DinoWidget, dino.face_name[4]), XtRString, "Yellow"},
  {XtNfaceColor5, XtCLabel, XtRString, sizeof(String),
   XtOffset(DinoWidget, dino.face_name[5]), XtRString, "Blue"},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
   XtOffset(DinoWidget, dino.foreground), XtRString, XtDefaultForeground},
  {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
   XtOffset(DinoWidget, core.width), XtRString, "300"},
  {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
   XtOffset(DinoWidget, core.height), XtRString, "400"},
  {XtNmode, XtCMode, XtRInt, sizeof(int),
   XtOffset(DinoWidget, dino.mode), XtRString, "3"}, /*DEFAULTMODE*/
  {XtNorient, XtCOrient, XtRBoolean, sizeof(Boolean),
   XtOffset(DinoWidget, dino.orient), XtRString, "FALSE"}, /* DEFAULTORIENT */
  {XtNmono, XtCMono, XtRBoolean, sizeof(Boolean),
   XtOffset(DinoWidget, dino.mono), XtRString, "FALSE"},
  {XtNface, XtCFace, XtRInt, sizeof(int),
   XtOffset(DinoWidget, dino.face), XtRString, "-1"},
  {XtNside, XtCSide, XtRInt, sizeof(int),
   XtOffset(DinoWidget, dino.side), XtRString, "-1"},
  {XtNdirection, XtCDirection, XtRInt, sizeof(int),
   XtOffset(DinoWidget, dino.direction), XtRString, "-1"},
  {XtNstyle, XtCStyle, XtRInt, sizeof(int),
   XtOffset(DinoWidget, dino.style), XtRString, "-1"},
  {XtNpractice, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(DinoWidget, dino.practice), XtRString, "FALSE"},
  {XtNstart, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(DinoWidget, dino.started), XtRString, "FALSE"},
  {XtNselectCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
   XtOffset(DinoWidget, dino.select), XtRCallback, NULL}
};

Dino2DClassRec dino2dClassRec =
{
  {
    (WidgetClass) &dinoClassRec,	/* superclass */
    "Dino2D",				/* class name */
    sizeof(Dino2DRec),			/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeDino2D,			/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    actionsListDino2D,			/* actions */
    XtNumber(actionsListDino2D),	/* num actions */
    resourcesDino2D,			/* resources */
    XtNumber(resourcesDino2D),		/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    NULL,				/* destroy */
    ResizeDino2D,			/* resize */
    ExposeDino2D,			/* expose */
    SetValuesDino2D,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    XtInheritAcceptFocus,		/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    defaultTranslationsDino2D,		/* tm table */
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

WidgetClass dino2dWidgetClass = (WidgetClass) &dino2dClassRec;

static int plane_to_cube[MAXRECT] = {6, 0, 6, 1, 2, 3, 6, 4, 6, 6, 5, 6};
static int cube_to_plane[MAXFACES] = {1, 3, 4, 5, 7, 10};
static XPoint triangle_unit[MAXORIENT][3] = {
                                  {{2, 0},  {-1, 1},  {-1, -1}},
                                  {{3, 2},  {-1, -1}, {1, -1}},
                                  {{1, 3},  {1, -1},  {1, 1}},
                                  {{0, 1},  {1, 1},   {-1, 1}}
                                },
  letter_unit[MAXORIENT] = {{1, 0}, {2, 1}, {1, 2}, {0, 1}};
static XPoint triangle_list[MAXORIENT][3],
  letter_list[MAXORIENT], offset_list[MAXORIENT];

static void InitializeDino2D(request, new)
  Widget request, new;
{
  Dino2DWidget w = (Dino2DWidget) new;

  w->dino.dim = 2;
  ResizeDino2D(w);
}

static void ResizeDino2D(w)
  Dino2DWidget w;
{
  w->dino.delta = 2;
  w->dino.vertical = (w->core.height >= w->core.width);
  if (w->dino.vertical)
    w->dino2d.face_length = MIN(w->core.height / MAXY, w->core.width / MAXX);
  else
    w->dino2d.face_length = MIN(w->core.height / MAXX, w->core.width / MAXY);
  w->dino2d.face_length = MAX(w->dino2d.face_length - w->dino.delta - 1, 0);
  w->dino2d.diamond_length = w->dino2d.face_length - w->dino.delta;
  w->dino2d.view_length = w->dino2d.face_length + w->dino.delta;
  if (w->dino.vertical) {
    w->dino.puzzle_width = MAXX * (w->dino2d.view_length - 1) +
      w->dino.delta;
    w->dino.puzzle_height = MAXY * (w->dino2d.view_length - 1) +
      w->dino.delta;
  } else {
    w->dino.puzzle_width = MAXY * (w->dino2d.view_length - 1) +
      w->dino.delta;
    w->dino.puzzle_height = MAXX * (w->dino2d.view_length - 1) +
      w->dino.delta;
  }
  w->dino.puzzle_offset.x = ((int) w->core.width - w->dino.puzzle_width)
    / 2;
  w->dino.puzzle_offset.y = ((int) w->core.height - w->dino.puzzle_height)
    / 2;
  resize_polyhedrons(w);
}

static void ExposeDino2D(new, event, region)
  Widget new;
  XEvent *event;
  Region region; /* Not used */
{
  Dino2DWidget w = (Dino2DWidget) new;

  if (w->core.visible) {
    draw_frame(w, w->dino.puzzle_GC);
    draw_all_polyhedrons((DinoWidget) w);
  }
}

static Boolean SetValuesDino2D(current, request, new)
  Widget current, request, new;
{
  Dino2DWidget c = (Dino2DWidget) current, w = (Dino2DWidget) new;
  Boolean redraw = FALSE;

  if (w->dino2d.diamond_length != c->dino2d.diamond_length) {
    ResizeDino2D(w);
    redraw = TRUE;
  }
  return (redraw);
}

static void move_dino2d_tl(w, event, args, n_args)
  Dino2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_dino_input((DinoWidget) w, event->xbutton.x, event->xbutton.y, TL,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask),
    (int) (event->xkey.state &
      (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask)));
}

static void move_dino2d_top(w, event, args, n_args)
  Dino2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_dino_input((DinoWidget) w, event->xbutton.x, event->xbutton.y, TOP,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask),
    (int) (event->xkey.state &
      (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask)));
}

static void move_dino2d_tr(w, event, args, n_args)
  Dino2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_dino_input((DinoWidget) w, event->xbutton.x, event->xbutton.y, TR,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask),
    (int) (event->xkey.state &
      (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask)));
}

static void move_dino2d_left(w, event, args, n_args)
  Dino2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_dino_input((DinoWidget) w, event->xbutton.x, event->xbutton.y, LEFT,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask),
    (int) (event->xkey.state &
      (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask)));
}

static void move_dino2d_right(w, event, args, n_args)
  Dino2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_dino_input((DinoWidget) w, event->xbutton.x, event->xbutton.y, RIGHT,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask),
    (int) (event->xkey.state &
      (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask)));
}

static void move_dino2d_bl(w, event, args, n_args)
  Dino2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_dino_input((DinoWidget) w, event->xbutton.x, event->xbutton.y, BL,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask),
    (int) (event->xkey.state &
      (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask)));
}

static void move_dino2d_bottom(w, event, args, n_args)
  Dino2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_dino_input((DinoWidget) w, event->xbutton.x, event->xbutton.y, BOTTOM,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask),
    (int) (event->xkey.state &
      (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask)));
}

static void move_dino2d_br(w, event, args, n_args)
  Dino2DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_dino_input((DinoWidget) w, event->xbutton.x, event->xbutton.y, BR,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask),
    (int) (event->xkey.state &
      (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask)));
}

static void resize_polyhedrons(w)
  Dino2DWidget w;
{
  int i, j;

  w->dino.orient_line_length = w->dino2d.diamond_length / 3;
  w->dino.letter_offset.x = -2;
  w->dino.letter_offset.y = 4;
  for (i = 0; i < MAXORIENT; i++)
  {
    for (j = 0; j < 3; j++)
    {
      triangle_list[i][j].x = triangle_unit[i][j].x *
        (w->dino2d.diamond_length / 2 - 2 * w->dino.delta);
      triangle_list[i][j].y = triangle_unit[i][j].y *
        (w->dino2d.diamond_length / 2 - 2 * w->dino.delta);
    }
    if (letter_unit[i].x == 0)
      letter_list[i].x = w->dino2d.diamond_length / 5 +
        w->dino.letter_offset.x;
    else if (letter_unit[i].x == 1)
      letter_list[i].x = w->dino2d.diamond_length / 2 - 1 +
        w->dino.letter_offset.x;
    else if (letter_unit[i].x == 2)
      letter_list[i].x = 4 * w->dino2d.diamond_length / 5 - 2 +
        w->dino.letter_offset.x;
    if (letter_unit[i].y == 0)
      letter_list[i].y = w->dino2d.diamond_length / 5 + 2 +
        w->dino.letter_offset.y;
    else if (letter_unit[i].y == 1)
      letter_list[i].y = w->dino2d.diamond_length / 2 - 1 +
        w->dino.letter_offset.y;
    else if (letter_unit[i].y == 2)
      letter_list[i].y = 4 * w->dino2d.diamond_length / 5 - 3 +
        w->dino.letter_offset.y;

    if (triangle_unit[i][0].x == 0)
      offset_list[i].x = w->dino.delta - 1;
    else if (triangle_unit[i][0].x == 1)
      offset_list[i].x = 2 * w->dino.delta - 1;
    else if (triangle_unit[i][0].x == 2)
      offset_list[i].x = w->dino2d.diamond_length -
        2 * w->dino.delta - 2;
    else if (triangle_unit[i][0].x == 3)
      offset_list[i].x = w->dino2d.diamond_length - w->dino.delta - 2;
    if (triangle_unit[i][0].y == 0)
      offset_list[i].y = w->dino.delta - 2;
    else if (triangle_unit[i][0].y == 1)
      offset_list[i].y = 2 * w->dino.delta - 1;
    else if (triangle_unit[i][0].y == 2)
      offset_list[i].y = w->dino2d.diamond_length -
        2 * w->dino.delta - 2;
    else if (triangle_unit[i][0].y == 3)
      offset_list[i].y = w->dino2d.diamond_length - w->dino.delta - 1;
   }
}

int position_polyhedrons2d(w, x, y, face, side, direction)
  Dino2DWidget w;
  int x, y;
  int *face, *side, *direction;
{
  int face_x, face_y;
  int i, j;

  x -= w->dino.puzzle_offset.x;
  y -= w->dino.puzzle_offset.y;
  face_x = x / w->dino2d.view_length;
  face_y = y / w->dino2d.view_length;
  i = x - face_x * w->dino2d.view_length;
  j = y - face_y * w->dino2d.view_length;
  if (i + j < w->dino2d.view_length && i - j > 0)
    *side = TOP;
  else if (i - j > 0 && i + j > w->dino2d.view_length)
    *side = RIGHT;
  else if (i + j > w->dino2d.view_length && j - i > 0)
    *side = BOTTOM;
  else if (j - i > 0 && i + j < w->dino2d.view_length)
    *side = LEFT;
  else if (*direction < 2 * MAXORIENT) /* ambiguous */
     return FALSE;
  if ((face_x != 1 && face_y != 1) ||
      (face_x >= 3 && w->dino.vertical) ||
      (face_y >= 3 && !w->dino.vertical))
    return FALSE;
  *face = plane_to_cube[face_x + face_y * MAXX];
  if (face_x == 3) {
    *face = MAXFACES - 1;
    *side = (*side + HALF) % MAXORIENT + 2 * MAXORIENT;
    if (*direction < MAXORIENT)
      *direction = (*direction + HALF) % MAXORIENT;
    else if (*direction >= 2 * MAXORIENT)
      *direction = 2 * MAXORIENT + (*direction + HALF) % MAXORIENT;
  }
  return TRUE;
}

static void draw_frame(w, gc)
  Dino2DWidget w;
  GC gc;
{
  int i;
  XPoint pos[MAXXY + 1], letters;
  
  for (i = 0; i <= MAXXY; i++) {
    pos[i].x = i * w->dino2d.view_length + w->dino.puzzle_offset.x;
    pos[i].y = i * w->dino2d.view_length + w->dino.puzzle_offset.y;
  }
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    pos[1].x, pos[0].y, pos[2].x, pos[0].y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    pos[3].x, pos[1].y, pos[3].x, pos[2].y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    pos[1].x, pos[3].y, pos[2].x, pos[3].y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    pos[0].x, pos[1].y, pos[0].x, pos[2].y);
  letters.x = pos[0].x + w->dino2d.view_length / 2 - w->dino.delta;
  letters.y = pos[0].y + w->dino2d.view_length / 2;
  XDrawString(XtDisplay(w), XtWindow(w), gc,
    (int) (letters.x + 5 * w->dino.letter_offset.x),
    (int) (letters.y + w->dino.letter_offset.y), "Front", 5);
  letters.x = pos[2].x + w->dino2d.view_length / 2 - w->dino.delta;
  letters.y = pos[2].y + w->dino2d.view_length / 2;
  XDrawString(XtDisplay(w), XtWindow(w), gc,
    (int) (letters.x + 4 * w->dino.letter_offset.x),
    (int) (letters.y + w->dino.letter_offset.y), "Back", 4);
  if (w->dino.vertical) {
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

void draw_triangle2d(w, face, side)
  Dino2DWidget w;
  int face, side;
{
  int dx, dy, letter_x, letter_y, new_side, orient;

  orient = w->dino.cube_loc[face][side].rotation;
  if (w->dino.vertical || face != MAXFACES - 1) {
    dx = (cube_to_plane[face] % MAXX) * w->dino2d.view_length;
    dy = (cube_to_plane[face] / MAXX) * w->dino2d.view_length;
    new_side = side;
  } else {
    dx = (cube_to_plane[face] / MAXX) * w->dino2d.view_length;
    dy = (cube_to_plane[face] % MAXX) * w->dino2d.view_length;
    new_side = (side + HALF) % STRT;
    orient = (orient + HALF) % STRT;
  }
  dx += w->dino.puzzle_offset.x + 2 * w->dino.delta;
  dy += w->dino.puzzle_offset.y + 2 * w->dino.delta;
  letter_x = dx + letter_list[new_side].x - 1;
  letter_y = dy + letter_list[new_side].y;
  triangle_list[new_side][0].x = offset_list[new_side].x + dx;
  triangle_list[new_side][0].y = offset_list[new_side].y + dy;
  XFillPolygon(XtDisplay(w), XtWindow(w),
    w->dino.face_GC[w->dino.cube_loc[face][side].face],
    triangle_list[new_side], 3, Convex, CoordModePrevious);
  if (w->dino.depth == 1 || w->dino.mono) {
    char buf[2];

    (void) sprintf(buf, "%c",
             w->dino.face_name[w->dino.cube_loc[face][side].face][0]);
    XDrawString(XtDisplay(w), XtWindow(w), w->dino.inverse_GC,
      letter_x, letter_y, buf, 1);
  }
  if (w->dino.orient)
    draw_orient_line(w, orient + 2 * MAXORIENT,
      letter_x - w->dino.letter_offset.x, letter_y - w->dino.letter_offset.y);
}

static void draw_orient_line(w, orient, dx, dy)
  Dino2DWidget w;
  int orient, dx, dy;
{
  switch (orient) {
    case TOP:
      XDrawLine(XtDisplay(w), XtWindow(w), w->dino.inverse_GC,
                dx,
                dy - w->dino.delta,
                dx,
                dy - w->dino.delta - w->dino.orient_line_length / 2);
      return;
    case RIGHT:
      XDrawLine(XtDisplay(w), XtWindow(w), w->dino.inverse_GC,
                dx + w->dino.delta,
                dy,
                dx + w->dino.delta + w->dino.orient_line_length / 2,
                dy);
      return;
    case BOTTOM:
      XDrawLine(XtDisplay(w), XtWindow(w), w->dino.inverse_GC,
                dx,
                dy + w->dino.delta,
                dx,
                dy + w->dino.delta + w->dino.orient_line_length / 2);
      return;
    case LEFT:
      XDrawLine(XtDisplay(w), XtWindow(w), w->dino.inverse_GC,
                dx - w->dino.delta,
                dy,
                dx - w->dino.delta - w->dino.orient_line_length / 2,
                dy);
      return;
    default:
      (void) printf("draw_orient_line: orient %d\n", orient);
  }
}
