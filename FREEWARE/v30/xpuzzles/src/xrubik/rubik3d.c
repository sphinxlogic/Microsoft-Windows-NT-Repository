/*
# X-BASED RUBIK'S CUBE(tm)
#
#  Rubik3d.c
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

/* Methods file for Rubik3d */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include "RubikP.h"
#include "Rubik3dP.h"

static void InitializeRubik3D();
static void ExposeRubik3D();
static void ResizeRubik3D();
static Boolean SetValuesRubik3D();
static void move_rubik3d_tl();
static void move_rubik3d_top();
static void move_rubik3d_tr();
static void move_rubik3d_left();
static void move_rubik3d_right();
static void move_rubik3d_bl();
static void move_rubik3d_bottom();
static void move_rubik3d_br();
static void resize_polyhedrons();
static void draw_frame();
static void map_to_3d();
#ifdef DEBUG
static void map_from_3d();
#endif
static void map_orient_from_3d();
static void cube_offset_3d();

static char defaultTranslationsRubik3D[] =
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
   <KeyPress>R13: move_bl()\n\
   <KeyPress>Down: move_bottom()\n\
   <KeyPress>KP_2: move_bottom()\n\
   <KeyPress>R14: move_bottom()\n\
   <KeyPress>Next: move_br()\n\
   <KeyPress>KP_3: move_br()\n\
   <KeyPress>R15: move_br()\n\
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

static XtActionsRec actionsListRubik3D[] =
{
  {"quit", (XtActionProc) quit_rubik},
  {"move_tl", (XtActionProc) move_rubik3d_tl},
  {"move_top", (XtActionProc) move_rubik3d_top},
  {"move_tr", (XtActionProc) move_rubik3d_tr},
  {"move_left", (XtActionProc) move_rubik3d_left},
  {"move_cw", (XtActionProc) move_rubik_cw},
  {"move_right", (XtActionProc) move_rubik3d_right},
  {"move_bl", (XtActionProc) move_rubik3d_bl},
  {"move_bottom", (XtActionProc) move_rubik3d_bottom},
  {"move_br", (XtActionProc) move_rubik3d_br},
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

static XtResource resourcesRubik3D[] =
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
   XtOffset(RubikWidget, core.width), XtRString, "250"},
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

Rubik3DClassRec rubik3dClassRec =
{
  {
    (WidgetClass) &rubikClassRec,	/* superclass */
    "Rubik3D",				/* class name */
    sizeof(Rubik3DRec),			/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeRubik3D,			/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    actionsListRubik3D,			/* actions */
    XtNumber(actionsListRubik3D),	/* num actions */
    resourcesRubik3D,			/* resources */
    XtNumber(resourcesRubik3D),		/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    NULL,				/* destroy */
    ResizeRubik3D,			/* resize */
    ExposeRubik3D,			/* expose */
    SetValuesRubik3D,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    XtInheritAcceptFocus,		/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    defaultTranslationsRubik3D,		/* tm table */
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

WidgetClass rubik3dWidgetClass = (WidgetClass) &rubik3dClassRec;

static XPoint face_loc_3d[MAXFACES][MAXORIENT];
static XPoint cube_loc_3d[MAXFACES][MAXORIENT];
static XPoint letter_3d_list[MAXFACES];
static XPoint orient_3d_list[MAXFACES][MAXORIENT][2];
static RowNext rotate_to_row[MAXFACES] = /*CW to min face*/
{
  {1,   LEFT,    TOP},
  {0, BOTTOM,  RIGHT},
  {0,  RIGHT, BOTTOM},
  {0,    TOP,   LEFT},
  {1,  RIGHT, BOTTOM},
  {0,   LEFT,    TOP}
};

static void InitializeRubik3D(request, new)
  Widget request, new;
{
  Rubik3DWidget w = (Rubik3DWidget) new;

  w->rubik.dim = 3;
  ResizeRubik3D(w);
}

static void ResizeRubik3D(w)
  Rubik3DWidget w;
{
  int temp_width, temp_height;

  w->rubik.delta = 4;
  w->rubik.vertical = (w->core.height >= w->core.width);
  if (w->rubik.vertical) {
    temp_height = w->core.height / MAXVIEWS;
    temp_width = w->core.width;
    if (temp_width >= DIVIDE(temp_height)) {
      w->rubik3d.cube_height = MAX((temp_height - 3 * w->rubik.delta) /
        (2 * w->rubik.size) - w->rubik.delta - 2, 0);
      w->rubik3d.cube_width = DIVIDE(w->rubik3d.cube_height);
    } else {
      w->rubik3d.cube_width = MAX((temp_width - 2 * w->rubik.delta - 7) /
        (2 * w->rubik.size) - w->rubik.delta, 0);
      w->rubik3d.cube_height = MULTIPLY(w->rubik3d.cube_width);
    }
    w->rubik3d.cube_diagonal = w->rubik3d.cube_width / 2;
    w->rubik3d.face_width = w->rubik.size *
      (w->rubik3d.cube_width + w->rubik.delta) + w->rubik.delta + 1;
    w->rubik3d.face_height = w->rubik.size *
      (w->rubik3d.cube_height + w->rubik.delta) + w->rubik.delta + 1;
    w->rubik3d.face_diagonal = w->rubik3d.face_width / 2;
    w->rubik3d.view_width = 2 * w->rubik3d.face_width + 3;
    w->rubik3d.view_height = 2 * w->rubik3d.face_height + 3;
    w->rubik.puzzle_width = w->rubik3d.view_width + 1;
    w->rubik.puzzle_height = MAXVIEWS * w->rubik3d.view_height + 1;
  } else {
    temp_width = w->core.width / MAXVIEWS;
    temp_height = w->core.height;
    if (temp_height >= DIVIDE(temp_width)) {
      w->rubik3d.cube_width = MAX((temp_width - 3 * w->rubik.delta) /
        (2 * w->rubik.size) - w->rubik.delta - 2, 0);
      w->rubik3d.cube_height = DIVIDE(w->rubik3d.cube_width);
    } else {
      w->rubik3d.cube_height = MAX((temp_height - 2 * w->rubik.delta - 7) /
        (2 * w->rubik.size) - w->rubik.delta, 0);
      w->rubik3d.cube_width = MULTIPLY(w->rubik3d.cube_height);
    }
    w->rubik3d.cube_diagonal = w->rubik3d.cube_height / 2;
    w->rubik3d.face_height = w->rubik.size *
      (w->rubik3d.cube_height + w->rubik.delta) + w->rubik.delta + 1;
    w->rubik3d.face_width = w->rubik.size *
      (w->rubik3d.cube_width + w->rubik.delta) + w->rubik.delta + 1;
    w->rubik3d.face_diagonal = w->rubik3d.face_height / 2;
    w->rubik3d.view_height = 2 * w->rubik3d.face_height + 3;
    w->rubik3d.view_width = 2 * w->rubik3d.face_width + 3;
    w->rubik.puzzle_height = w->rubik3d.view_height + 1;
    w->rubik.puzzle_width = MAXVIEWS * w->rubik3d.view_width + 1;
  }
  w->rubik.puzzle_offset.x = ((int) w->core.width - w->rubik.puzzle_width) / 2;
  w->rubik.puzzle_offset.y = ((int) w->core.height - w->rubik.puzzle_height) /
    2;
  resize_polyhedrons(w);
}

static void ExposeRubik3D(new, event, region)
  Widget new;
  XEvent *event;
  Region region; /* Not used */
{
  Rubik3DWidget w = (Rubik3DWidget) new;

  if (w->core.visible) {
    draw_frame(w, w->rubik.puzzle_GC);
    draw_all_polyhedrons((RubikWidget) w);
  }
}

static Boolean SetValuesRubik3D(current, request, new)
  Widget current, request, new;
{
  Rubik3DWidget c = (Rubik3DWidget) current, w = (Rubik3DWidget) new;
  Boolean redraw = FALSE;

  if (w->rubik.size != c->rubik.size) {
    reset_polyhedrons((RubikWidget) w);
    w->rubik.practice = FALSE;
    ResizeRubik3D(w);
    redraw = TRUE;
  }
  if (w->rubik3d.cube_width != c->rubik3d.cube_width) {
    ResizeRubik3D(w);
    redraw = TRUE;
  }
  return (redraw);
}

static void move_rubik3d_tl(w, event, args, n_args)
  Rubik3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input((RubikWidget) w, event->xbutton.x, event->xbutton.y, TL,
    (int) (event->xkey.state & ControlMask));
}

static void move_rubik3d_top(w, event, args, n_args)
  Rubik3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input((RubikWidget) w, event->xbutton.x, event->xbutton.y, TOP,
    (int) (event->xkey.state & ControlMask));
}

static void move_rubik3d_tr(w, event, args, n_args)
  Rubik3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input((RubikWidget) w, event->xbutton.x, event->xbutton.y, TR,
    (int) (event->xkey.state & ControlMask));
}

static void move_rubik3d_left(w, event, args, n_args)
  Rubik3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input((RubikWidget) w, event->xbutton.x, event->xbutton.y, LEFT,
    (int) (event->xkey.state & ControlMask));
}

static void move_rubik3d_right(w, event, args, n_args)
  Rubik3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input((RubikWidget) w, event->xbutton.x, event->xbutton.y, RIGHT,
    (int) (event->xkey.state & ControlMask));
}

static void move_rubik3d_bl(w, event, args, n_args)
  Rubik3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input((RubikWidget) w, event->xbutton.x, event->xbutton.y, BL,
    (int) (event->xkey.state & ControlMask));
}

static void move_rubik3d_bottom(w, event, args, n_args)
  Rubik3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input((RubikWidget) w, event->xbutton.x, event->xbutton.y, BOTTOM,
    (int) (event->xkey.state & ControlMask));
}

static void move_rubik3d_br(w, event, args, n_args)
  Rubik3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_rubik_input((RubikWidget) w, event->xbutton.x, event->xbutton.y, BR,
    (int) (event->xkey.state & ControlMask));
}

static void resize_polyhedrons(w)
  Rubik3DWidget w;
{
  int face, orient, side, corner;
  XPoint subcube_loc_3d[MAXFACES][MAXORIENT];
  XPoint diamond_loc_3d[MAXFACES][MAXORIENT];
  XPoint subdiamond_loc_3d[MAXFACES][MAXORIENT];

  w->rubik.letter_offset.x = -2;
  w->rubik.letter_offset.y = 4;
  w->rubik3d.view_middle.x = w->rubik3d.face_width +
    w->rubik.puzzle_offset.x;
  w->rubik3d.view_middle.y = w->rubik3d.face_height +
    w->rubik.puzzle_offset.y;
  for (face = 0; face < MAXFACES; face++) {
    face_loc_3d[face][0].x = w->rubik3d.view_middle.x;
    face_loc_3d[face][0].y = w->rubik3d.view_middle.y;
    for (orient = 1; orient < MAXORIENT; orient++) {
      face_loc_3d[face][orient].x = w->rubik3d.face_width;
      face_loc_3d[face][orient].y = w->rubik3d.face_height;
    }
  }
  if (w->rubik.vertical) {
    face_loc_3d[0][1].x /= -2;
    face_loc_3d[0][1].y /= -1;
    face_loc_3d[0][2].y = 0;
    face_loc_3d[0][3].x /= 2;

    face_loc_3d[1][1].x /= -2;
    face_loc_3d[1][2].x /= -2;
    face_loc_3d[1][2].y /= -1;
    face_loc_3d[1][3].x /= 2;
    face_loc_3d[1][3].y /= -1;

    face_loc_3d[2][1].y = 0;
    face_loc_3d[2][2].x /= -2;
    face_loc_3d[2][3].x /= -1;
    face_loc_3d[2][3].y = 0;

    for (face = MAXFACES / 2; face < MAXFACES; face++)
      face_loc_3d[face][0].y += w->rubik3d.view_height + 3;

    face_loc_3d[3][1].x /= 2;
    face_loc_3d[3][1].y /= -1;
    face_loc_3d[3][2].x /= 2;
    face_loc_3d[3][3].x /= -2;

    face_loc_3d[4][1].x /= -1;
    face_loc_3d[4][1].y = 0;
    face_loc_3d[4][2].x /= 2;
    face_loc_3d[4][2].y /= -1;
    face_loc_3d[4][3].y = 0;

    face_loc_3d[5][1].x /= 2;
    face_loc_3d[5][2].x /= -1;
    face_loc_3d[5][2].y = 0;
    face_loc_3d[5][3].x /= -2;
    face_loc_3d[5][3].y /= -1;
  } else {
    face_loc_3d[0][1].x /= -1;
    face_loc_3d[0][1].y /= -2;
    face_loc_3d[0][2].y /= -2;
    face_loc_3d[0][3].y /= 2;

    face_loc_3d[1][1].x = 0;
    face_loc_3d[1][2].x /= -1;
    face_loc_3d[1][2].y /= -2;
    face_loc_3d[1][3].x = 0;
    face_loc_3d[1][3].y /= -1;

    face_loc_3d[2][1].y /= -2;
    face_loc_3d[2][2].x = 0;
    face_loc_3d[2][3].y /= 2;
    face_loc_3d[2][3].x /= -1;

    for (face = MAXFACES / 2; face < MAXFACES; face++)
       face_loc_3d[face][0].x += w->rubik3d.view_width + 3;

    face_loc_3d[3][1].x /= -1;
    face_loc_3d[3][1].y /= 2;
    face_loc_3d[3][2].x = 0;
    face_loc_3d[3][2].y /= -1;
    face_loc_3d[3][3].y /= -2;

    face_loc_3d[4][1].y /= 2;
    face_loc_3d[4][2].x /= -1;
    face_loc_3d[4][2].y /= 2;
    face_loc_3d[4][3].x /= -1;
    face_loc_3d[4][3].y /= -2;

    face_loc_3d[5][1].x = 0;
    face_loc_3d[5][1].y /= -1;
    face_loc_3d[5][2].y /= 2;
    face_loc_3d[5][3].x = 0;
  }
  for (face = 0; face < MAXFACES; face++) {
    cube_loc_3d[face][0].x = face_loc_3d[face][0].x;
    cube_loc_3d[face][0].y = face_loc_3d[face][0].y;
    subcube_loc_3d[face][0].x = face_loc_3d[face][0].x;
    subcube_loc_3d[face][0].y = face_loc_3d[face][0].y;
    for (orient = 1; orient < MAXORIENT; orient++) {
      cube_loc_3d[face][orient].x =
        (face_loc_3d[face][orient].x - (3 + w->rubik.size) *
         w->rubik.delta * face_loc_3d[face][orient].x /
         w->rubik3d.face_width) / w->rubik.size;
      cube_loc_3d[face][orient].y =
        (face_loc_3d[face][orient].y - (3 + w->rubik.size) *
         w->rubik.delta * face_loc_3d[face][orient].y /
         w->rubik3d.face_height) / w->rubik.size;
      subcube_loc_3d[face][orient].x =
        (face_loc_3d[face][orient].x - (5 + w->rubik.size) *
         w->rubik.delta * face_loc_3d[face][orient].x /
         w->rubik3d.face_width) / (2 * w->rubik.size);
      subcube_loc_3d[face][orient].y =
        (face_loc_3d[face][orient].y - (5 + w->rubik.size) *
         w->rubik.delta * face_loc_3d[face][orient].y /
         w->rubik3d.face_height) / (2 * w->rubik.size);
    }
  }
  w->rubik3d.cube_width = (w->rubik3d.face_width - w->rubik.delta) /
    w->rubik.size - w->rubik.delta;
  w->rubik3d.cube_diagonal = (w->rubik3d.face_diagonal - w->rubik.delta) /
    w->rubik.size - w->rubik.delta;
  w->rubik3d.cube_height = (w->rubik3d.face_height - w->rubik.delta) /
    w->rubik.size - w->rubik.delta;
  w->rubik3d.cube_diag = (w->rubik3d.face_diagonal + w->rubik.delta) /
    w->rubik.size + w->rubik.delta;
  if (w->rubik.vertical) {  
    letter_3d_list[0].x = w->rubik3d.cube_width / 4;
    letter_3d_list[0].y = -w->rubik3d.cube_height / 2;
    letter_3d_list[1].x = -w->rubik3d.cube_diagonal;
    letter_3d_list[1].y = 0;
    letter_3d_list[2].x = w->rubik3d.cube_width / 4;
    letter_3d_list[2].y = w->rubik3d.cube_height / 2;
    letter_3d_list[3].x = w->rubik3d.cube_diagonal;
    letter_3d_list[3].y = 0;
    letter_3d_list[4].x = -w->rubik3d.cube_width / 4;
    letter_3d_list[4].y = -w->rubik3d.cube_height / 2;
    letter_3d_list[5].x = -w->rubik3d.cube_width / 4;
    letter_3d_list[5].y = w->rubik3d.cube_height / 2;
  } else {
    letter_3d_list[0].x = 0;
    letter_3d_list[0].y = -w->rubik3d.cube_diagonal;
    letter_3d_list[1].x = -w->rubik3d.cube_width / 2;
    letter_3d_list[1].y = w->rubik3d.cube_height / 4;
    letter_3d_list[2].x = w->rubik3d.cube_width / 2;
    letter_3d_list[2].y = w->rubik3d.cube_height / 4;
    letter_3d_list[3].x = -w->rubik3d.cube_width / 2;
    letter_3d_list[3].y = -w->rubik3d.cube_height / 4;
    letter_3d_list[4].x = 0;
    letter_3d_list[4].y = w->rubik3d.cube_diagonal;
    letter_3d_list[5].x = w->rubik3d.cube_width / 2;
    letter_3d_list[5].y = -w->rubik3d.cube_height / 4;
  }
  /* The following figures out where to put the orient lines */
  for (face = 0; face < MAXFACES; face++) {
    for (orient = 0; orient < MAXORIENT - 1; orient++) {
      diamond_loc_3d[face][orient].x = (cube_loc_3d[face][orient].x +
        cube_loc_3d[face][orient + 1].x) / 2;
      diamond_loc_3d[face][orient].y = (cube_loc_3d[face][orient].y +
        cube_loc_3d[face][orient + 1].y) / 2;
      subdiamond_loc_3d[face][orient].x = (subcube_loc_3d[face][orient].x +
        subcube_loc_3d[face][orient + 1].x) / 2;
      subdiamond_loc_3d[face][orient].y = (subcube_loc_3d[face][orient].y +
        subcube_loc_3d[face][orient + 1].y) / 2;
    }
    /* Its a parallelagram so take advantage of that */
    diamond_loc_3d[face][orient].x = (cube_loc_3d[face][MAXORIENT - 1].x -
      cube_loc_3d[face][MAXORIENT / 2].x) / 2;
    diamond_loc_3d[face][orient].y = (cube_loc_3d[face][MAXORIENT - 1].y -
      cube_loc_3d[face][MAXORIENT / 2].y) / 2;
    subdiamond_loc_3d[face][orient].x = (subcube_loc_3d[face][MAXORIENT - 1].x -
      subcube_loc_3d[face][MAXORIENT / 2].x) / 2;
    subdiamond_loc_3d[face][orient].y = (subcube_loc_3d[face][MAXORIENT - 1].y -
      subcube_loc_3d[face][MAXORIENT / 2].y) / 2;

    map_orient_from_3d(face, 1, &corner);
    orient_3d_list[face][corner][0].x = cube_loc_3d[face][1].x / 2;
    orient_3d_list[face][corner][0].y = cube_loc_3d[face][1].y / 2;
    orient_3d_list[face][corner][1].x = orient_3d_list[face][corner][0].x +
      (cube_loc_3d[face][2].x - subcube_loc_3d[face][2].x) / 2;
    orient_3d_list[face][corner][1].y = orient_3d_list[face][corner][0].y +
      (cube_loc_3d[face][2].y - subcube_loc_3d[face][2].y) / 2;
    for (orient = 1; orient < MAXORIENT; orient++) {
      side = corner;
      map_orient_from_3d(face, (orient + 1) % MAXORIENT, &corner);
      orient_3d_list[face][corner][0].x =
        orient_3d_list[face][side][0].x + diamond_loc_3d[face][orient].x;
      orient_3d_list[face][corner][0].y =
        orient_3d_list[face][side][0].y + diamond_loc_3d[face][orient].y;
      orient_3d_list[face][corner][1].x =
        orient_3d_list[face][side][1].x + subdiamond_loc_3d[face][orient].x;
      orient_3d_list[face][corner][1].y =
        orient_3d_list[face][side][1].y + subdiamond_loc_3d[face][orient].y;
    }
  }
}

int position_polyhedrons3d(w, x, y, face, i, j, direction)
  Rubik3DWidget w;
  int x, y;
  int *face, *i, *j, *direction;
{
  int u, v, front, tl, ur, ul;

  if (w->rubik.vertical)
  {
    x -= w->rubik3d.view_middle.x;
    front = (y < w->rubik3d.view_height);
    if (!front)
      y -= (w->rubik3d.view_height);
    tl = (y < w->rubik3d.view_middle.y); 
    y -= w->rubik3d.view_middle.y;
    u = -w->rubik3d.face_height * x + w->rubik3d.face_diagonal * y;
    v = w->rubik3d.face_height * x + w->rubik3d.face_diagonal * y;
    ur = (u < 0);
    ul = (v < 0);
    if (front) {
      if (tl)
        *face = (ur) ? 0 : 1;
      else
        *face = (ul) ? 1 : 2;
    } else {
      if (tl)
        *face = (ul) ? 4 : 3;
      else
        *face = (ur) ? 3 : 5;
    }
  } else {
    y -= w->rubik3d.view_middle.y;
    front = (x < w->rubik3d.view_width);
    if (!front)
      x -= (w->rubik3d.view_width);
    tl = (x < w->rubik3d.view_middle.x); 
    x -= w->rubik3d.view_middle.x;
    u = -w->rubik3d.face_width * y + w->rubik3d.face_diagonal * x;
    v = w->rubik3d.face_width * y + w->rubik3d.face_diagonal * x;
    ur = (u < 0);
    ul = (v < 0);
    if (front) {
      if (tl)
        *face = (ur) ? 1 : 0;
      else
        *face = (ul) ? 0 : 2;
    } else {
      if (tl)
        *face = (ul) ? 3 : 4;
      else
        *face = (ur) ? 4 : 5;
    }
  }
  switch (*direction) {
    case TOP:
    case RIGHT:
    case BOTTOM:
    case LEFT:
      if (w->rubik.vertical) {
	if (*face == 1 || *face == 3)
          return FALSE;
      } else {
	if (*face == 0 || *face == 4)
          return FALSE;
        if (*face == 5)
	  *direction = (*direction + 2) % MAXORIENT;
      }
      break;
    case CCW:
    case CW:
      break;
    case TR:
      if (w->rubik.vertical) {
        if (*face == 0 || *face == 5)
          return FALSE;
        else if (*face == 1 || *face == 2 || *face == 4)
          *direction = TOP;
        else /* (*face == 3) */
          *direction = LEFT;
      } else {
        if (*face == 1 || *face == 5)
          return FALSE;
        else if (*face == 0 || *face == 2 || *face == 3)
          *direction = RIGHT;
        else /* (*face == 4) */
          *direction = BOTTOM;
      }
      break;
    case BR:
      if (w->rubik.vertical) {
        if (*face == 2 || *face == 4)
          return FALSE;
        else if (*face == 0 || *face == 5)
          *direction = BOTTOM;
        else if (*face == 1)
          *direction = RIGHT;
        else /* (*face == 3) */
          *direction = TOP;
      } else {
        if (*face == 2 || *face == 3)
          return FALSE;
        else if (*face == 4 || *face == 5)
          *direction = LEFT;
        else if (*face == 0)
          *direction = BOTTOM;
        else /* (*face == 1) */
          *direction = RIGHT;
      }
      break;
    case BL:
      if (w->rubik.vertical) {
        if (*face == 0 || *face == 5)
          return FALSE;
        else if (*face == 1 || *face == 2 || *face == 4)
          *direction = BOTTOM;
        else /* (*face == 3) */
          *direction = RIGHT;
      } else {
        if (*face == 1 || *face == 5)
          return FALSE;
        else if (*face == 0 || *face == 2 || *face == 3)
          *direction = LEFT;
        else /* (*face == 4) */
          *direction = TOP;
      }
      break;
    case TL:
      if (w->rubik.vertical) {
        if (*face == 2 || *face == 4)
          return FALSE;
        else if (*face == 0 || *face == 5)
          *direction = TOP;
        else if (*face == 1)
          *direction = LEFT;
        else /* (*face == 3) */
          *direction = BOTTOM;
      } else {
        if (*face == 2 || *face == 3)
          return FALSE;
        else if (*face == 4 || *face == 5)
          *direction = RIGHT;
        else if (*face == 0)
          *direction = TOP;
        else /* (*face == 1) */
          *direction = LEFT;
      }
      break;
    default:
      return FALSE;
  }
  /*cube_diag = (w->rubik3d.face_diagonal + w->rubik.delta) / w->rubik.size +
    w->rubik.delta;*/
  if (w->rubik.vertical)
    switch (*face) {
      case 0:
        *i = (x - 2 - (y * w->rubik3d.face_diagonal / w->rubik3d.face_height)) /
          (w->rubik3d.cube_width + w->rubik.delta);
        *j = (y + 2) / (w->rubik3d.cube_height + w->rubik.delta) +
	  w->rubik.size - 1;
        break;
      case 1:
        *i = (x + 4 + (y * w->rubik3d.face_diagonal / w->rubik3d.face_height))/
          (2 * (w->rubik3d.cube_diagonal + w->rubik.delta)) + w->rubik.size - 1;
        *j = (-x - 6 + (y * w->rubik3d.face_diagonal / w->rubik3d.face_height))/
          (2 * (w->rubik3d.cube_diagonal + w->rubik.delta));
        break;
      case 2:
        *i = (x - 4 + (y * w->rubik3d.face_diagonal / w->rubik3d.face_height))/
          (w->rubik3d.cube_width + w->rubik.delta);
        *j = (y - 4) / (w->rubik3d.cube_height + w->rubik.delta);
        break;
      case 3:
        *i = (-x + 5 + (y * w->rubik3d.face_diagonal / w->rubik3d.face_height))/
          (2 * (w->rubik3d.cube_diagonal + w->rubik.delta)) + w->rubik.size - 1;
        *j = (-x + 7 - (y * w->rubik3d.face_diagonal / w->rubik3d.face_height))/
          (2 * (w->rubik3d.cube_diagonal + w->rubik.delta)) + w->rubik.size - 1;
        break;
      case 4:
        *i = (x + (y * w->rubik3d.face_diagonal / w->rubik3d.face_height))/
          (w->rubik3d.cube_width + w->rubik.delta) + w->rubik.size - 1;
        *j = (y - 2) / (w->rubik3d.cube_height + w->rubik.delta) +
	  w->rubik.size - 1;
        break;
      case 5:
        *i = (x + 2 - (y * w->rubik3d.face_diagonal / w->rubik3d.face_height))/
          (w->rubik3d.cube_width + w->rubik.delta) + w->rubik.size - 1;
        *j = (y - 6) / (w->rubik3d.cube_height + w->rubik.delta);
        break;
     default:
        return FALSE;
   }
 else
   switch (*face) {
     case 0:
       *i = (-y - 3 + (x * w->rubik3d.face_diagonal / w->rubik3d.face_width)) /
         (2 * (w->rubik3d.cube_diagonal + w->rubik.delta));
       *j = (y + 1 + (x * w->rubik3d.face_diagonal / w->rubik3d.face_width)) /
         (2 * (w->rubik3d.cube_diagonal + w->rubik.delta)) + w->rubik.size - 1;
       break;
     case 1:
       *i = (x + 2) / (w->rubik3d.cube_width + w->rubik.delta) +
	 w->rubik.size - 1;
       *j = (y - 3 - (x * w->rubik3d.face_diagonal / w->rubik3d.face_width)) /
         (w->rubik3d.cube_height + w->rubik.delta);
       break;
     case 2:
       *i = (x - 4) / (w->rubik3d.cube_width + w->rubik.delta);
       *j = (y - 6 + (x * w->rubik3d.face_diagonal / w->rubik3d.face_width)) /
         (w->rubik3d.cube_height + w->rubik.delta);
       break;
     case 3:
       *i = x / (w->rubik3d.cube_width + w->rubik.delta) +
	 w->rubik.size - 1;
       *j = (y + (x * w->rubik3d.face_diagonal / w->rubik3d.face_width)) /
         (w->rubik3d.cube_height + w->rubik.delta) + w->rubik.size - 1;
       break;
     case 4:
       *i = (-y + 9 - (x * w->rubik3d.face_diagonal / w->rubik3d.face_width))/
         (2 * (w->rubik3d.cube_diagonal + w->rubik.delta)) + w->rubik.size - 1;
       *j = (-y + 7 + (x * w->rubik3d.face_diagonal / w->rubik3d.face_width))/
         (2 * (w->rubik3d.cube_diagonal + w->rubik.delta)) + w->rubik.size - 1;
       break;
     case 5:
       *i = (-x + 7) / (w->rubik3d.cube_width + w->rubik.delta) +
	 w->rubik.size - 1;
       *j = (-y - 4 + (x * w->rubik3d.face_diagonal / w->rubik3d.face_width))/
         (w->rubik3d.cube_height + w->rubik.delta);
       break;
     default:
        return FALSE;
    }
  if (*i < 0 || *j < 0 || *i >= w->rubik.size || *j >= w->rubik.size)
    return FALSE;
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
  Rubik3DWidget w;
  GC gc;
{
  int face, dx, dy;

  dx = w->rubik3d.view_width + w->rubik.puzzle_offset.x;
  dy = w->rubik3d.view_height + w->rubik.puzzle_offset.y;
  if (w->rubik.vertical) {
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      0, dy, dx + w->rubik.puzzle_offset.x + 1, dy);
    XDrawString(XtDisplay(w), XtWindow(w), gc,
      (int) (2 * w->rubik.delta),
      (int) (3 * w->rubik.delta + w->rubik.letter_offset.y),
      "Front", 5);
    XDrawString(XtDisplay(w), XtWindow(w), gc, (int)
      (-4 * w->rubik.delta + 2 * 4 * w->rubik.letter_offset.x + w->core.width), 
      (int) (-w->rubik.delta - 2 * w->rubik.letter_offset.y + w->core.height),
      "Back", 4);
  } else {
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      dx, 0, dx, dy + w->rubik.puzzle_offset.y + 1);
    XDrawString(XtDisplay(w), XtWindow(w), gc,
      (int) (2 * w->rubik.delta),
      (int) (3 * w->rubik.delta + w->rubik.letter_offset.y),
      "Front", 5);
    XDrawString(XtDisplay(w), XtWindow(w), gc, (int)
      (-4 * w->rubik.delta + 2 * 4 * w->rubik.letter_offset.x + w->core.width), 
      (int) (-w->rubik.delta - 2 * w->rubik.letter_offset.y + w->core.height),
      "Back", 4);
  }
  for (face = 0; face < MAXFACES; face++)
    XDrawLines(XtDisplay(w), XtWindow(w), gc,
      face_loc_3d[face], MAXORIENT, CoordModePrevious);
}   

void draw_square3d(w, face, i, j)
  Rubik3DWidget w;
  int face, i, j;
{
  int x, y, dx, dy;

  map_to_3d(w, face, i, j, &x, &y);
  cube_offset_3d(w, face, x, y, &dx, &dy);
  cube_loc_3d[face][0].x = dx;
  cube_loc_3d[face][0].y = dy;
  XFillPolygon(XtDisplay(w), XtWindow(w),
    w->rubik.face_GC[w->rubik.cube_loc[face][i][j].face],
    cube_loc_3d[face], MAXORIENT, Convex, CoordModePrevious);
  if (w->rubik.depth == 1 || w->rubik.mono) {
    int letter_x, letter_y;
    char buf[2];

    (void) sprintf (buf, "%c",
             w->rubik.face_name[w->rubik.cube_loc[face][i][j].face][0]);
    letter_x = dx + letter_3d_list[face].x + w->rubik.letter_offset.x;
    letter_y = dy + letter_3d_list[face].y + w->rubik.letter_offset.y;
    XDrawString(XtDisplay(w), XtWindow(w), w->rubik.inverse_GC,
      letter_x, letter_y, buf, 1);
  }
  if (w->rubik.orient)
    XDrawLine(XtDisplay(w), XtWindow(w), w->rubik.inverse_GC,
      dx + orient_3d_list[face][w->rubik.cube_loc[face][i][j].rotation][0].x,
      dy + orient_3d_list[face][w->rubik.cube_loc[face][i][j].rotation][0].y,
      dx + orient_3d_list[face][w->rubik.cube_loc[face][i][j].rotation][1].x,
      dy + orient_3d_list[face][w->rubik.cube_loc[face][i][j].rotation][1].y);
}

static void map_to_3d(w, face, i, j, x, y)
  Rubik3DWidget w;
  int face, i, j, *x, *y;
{
  switch (face) {
    case 0:
      *x = w->rubik.size - 1 - j;
      *y = i;
      break;
    case 1:
      *x = j;
      *y = w->rubik.size - 1 - i;
      break;
    case 2:
      *x = i;
      *y = j;
      break;
    case 3:
      *x = w->rubik.size - 1 - i;
      *y = w->rubik.size - 1 - j;
      break;
    case 4:
      *x = w->rubik.size - 1 - i;
      *y = w->rubik.size - 1 - j;
      break;
    case 5:
      *x = j;
      *y = w->rubik.size - 1 - i;
      break;
    default:
      (void) printf ("map_to_3d: face %d\n", face);
  }
}

#ifdef DEBUG
static void map_from_3d(w, face, x, y, i, j)
  Rubik3DWidget w;
  int face, x, y, *i, *j;
{
  switch (face) {
    case 0:
      *i = y;
      *j = w->rubik.size - 1 - x;
      break;
    case 1:
      *i = w->rubik.size - 1 - y;
      *j = x;
      break;
    case 2:
      *i = x;
      *j = y;
      break;
    case 3:
      *i = w->rubik.size - 1 - x;
      *j = w->rubik.size - 1 - y;
      break;
    case 4:
      *i = w->rubik.size - 1 - x;
      *j = w->rubik.size - 1 - y;
      break;
    case 5:
      *i = w->rubik.size - 1 - y;
      *j = x;
      break;
    default:
      (void) printf ("map_from_3d: face %d\n", face);
  }
}
#endif

static void map_orient_from_3d(face, corner, side)
  int face, corner, *side;
{
  switch (face) {
    case 0:
      *side = (corner + 2) % MAXORIENT;
      break;
    case 1:
      *side = corner;
      break;
    case 2:
      *side = (corner + 3) % MAXORIENT;
      break;
    case 3:
      *side = (corner + 1) % MAXORIENT;
      break;
    case 4:
      *side = (corner + 1) % MAXORIENT;
      break;
    case 5:
      *side = corner;
      break;
    default:
      (void) printf ("map_from_3d: face %d\n", face);
  }
}

static void cube_offset_3d(w, face, x, y, dx, dy)
  Rubik3DWidget w;
  int face, x, y, *dx, *dy;
{
  if (w->rubik.vertical)
    switch (face) {
      case 0:
        *dx = w->rubik3d.view_middle.x + w->rubik.delta - 1 +
          y * (w->rubik3d.cube_width + w->rubik.delta) -
          x * (w->rubik3d.cube_diagonal + w->rubik.delta);
        *dy = w->rubik3d.view_middle.y - w->rubik.delta - 2 -
          x * (w->rubik3d.cube_height + w->rubik.delta);
        break;
      case 1:
        *dx = w->rubik3d.view_middle.x - 2 * w->rubik.delta -
          x * (w->rubik3d.cube_diagonal + w->rubik.delta) -
          y * (w->rubik3d.cube_diagonal + w->rubik.delta);
        *dy = w->rubik3d.view_middle.y +
          x * (w->rubik3d.cube_height + w->rubik.delta) -
          y * (w->rubik3d.cube_height + w->rubik.delta);
        break;
      case 2:
        *dx = w->rubik3d.view_middle.x + w->rubik.delta - 1 +
          x * (w->rubik3d.cube_width + w->rubik.delta) -
          y * (w->rubik3d.cube_diagonal + w->rubik.delta);
        *dy = w->rubik3d.view_middle.y + 2 * w->rubik.delta - 1 +
          y * (w->rubik3d.cube_height + w->rubik.delta);
        break;
      case 3:
        *dx = w->rubik3d.view_middle.x + 2 * w->rubik.delta +
          x * (w->rubik3d.cube_diagonal + w->rubik.delta) +
          y * (w->rubik3d.cube_diagonal + w->rubik.delta);
        *dy = w->rubik3d.view_height + w->rubik3d.view_middle.y + 
              w->rubik.delta - 1 -
          x * (w->rubik3d.cube_height + w->rubik.delta) +
          y * (w->rubik3d.cube_height + w->rubik.delta);
        break;
      case 4:
        *dx = w->rubik3d.view_middle.x - w->rubik.delta + 1 -
          x * (w->rubik3d.cube_width + w->rubik.delta) +
          y * (w->rubik3d.cube_diagonal + w->rubik.delta);
        *dy = w->rubik3d.view_height + w->rubik3d.view_middle.y -
              w->rubik.delta + 1 -
          y * (w->rubik3d.cube_height + w->rubik.delta);
        break;
      case 5:
        *dx = w->rubik3d.view_middle.x - 2 -
          y * (w->rubik3d.cube_width + w->rubik.delta) +
          x * (w->rubik3d.cube_diagonal + w->rubik.delta);
        *dy = w->rubik3d.view_height + w->rubik3d.view_middle.y +
              2 * w->rubik.delta + 2 +
          x * (w->rubik3d.cube_height + w->rubik.delta);
        break;
      default:
        (void) printf ("cube_offset_3d: face %d\n", face);
    } else switch (face) {
      case 0:
        *dx = w->rubik3d.view_middle.x +
          y * (w->rubik3d.cube_width + w->rubik.delta) -
          x * (w->rubik3d.cube_width + w->rubik.delta);
        *dy = w->rubik3d.view_middle.y - 2 * w->rubik.delta + 1 -
          y * (w->rubik3d.cube_diagonal + w->rubik.delta) -
          x * (w->rubik3d.cube_diagonal + w->rubik.delta);
        break;
      case 1:
        *dx = w->rubik3d.view_middle.x - w->rubik.delta - 2 -
          y * (w->rubik3d.cube_width + w->rubik.delta);
        *dy = w->rubik3d.view_middle.y + w->rubik.delta +
          x * (w->rubik3d.cube_height + w->rubik.delta) -
          y * (w->rubik3d.cube_diagonal + w->rubik.delta);
        break;
      case 2:
        *dx = w->rubik3d.view_middle.x + 2 * w->rubik.delta - 1 +
          x * (w->rubik3d.cube_width + w->rubik.delta);
        *dy = w->rubik3d.view_middle.y + w->rubik.delta +
          y * (w->rubik3d.cube_height + w->rubik.delta) -
          x * (w->rubik3d.cube_diagonal + w->rubik.delta); 
        break;
      case 3:
        *dx = w->rubik3d.view_width + w->rubik3d.view_middle.x -
              w->rubik.delta + 1 -
          x * (w->rubik3d.cube_width + w->rubik.delta);
        *dy = w->rubik3d.view_middle.y - w->rubik.delta -
          y * (w->rubik3d.cube_height + w->rubik.delta) +
          x * (w->rubik3d.cube_diagonal + w->rubik.delta);
        break;
      case 4:
        *dx = w->rubik3d.view_width + w->rubik3d.view_middle.x +
              w->rubik.delta - 1 -
          y * (w->rubik3d.cube_width + w->rubik.delta) +
          x * (w->rubik3d.cube_width + w->rubik.delta);
        *dy = w->rubik3d.view_middle.y + 2 * w->rubik.delta +
          y * (w->rubik3d.cube_diagonal + w->rubik.delta) +
          x * (w->rubik3d.cube_diagonal + w->rubik.delta);
        break;
      case 5:
        *dx = w->rubik3d.view_width + w->rubik3d.view_middle.x +
              2 * w->rubik.delta + 2 +
          y * (w->rubik3d.cube_width + w->rubik.delta);
        *dy = w->rubik3d.view_middle.y - w->rubik.delta -
          x * (w->rubik3d.cube_height + w->rubik.delta) +
          y * (w->rubik3d.cube_diagonal + w->rubik.delta);
        break;
      default:
        (void) printf ("cube_offset_3d: face %d\n", face);
    }
}
