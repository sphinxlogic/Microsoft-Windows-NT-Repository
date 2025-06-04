/*
# X-BASED DINO
#
#  Dino3d.c
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

/* Methods file for Dino3d */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include "DinoP.h"
#include "Dino3dP.h"

static void InitializeDino3D();
static void ExposeDino3D();
static void ResizeDino3D();
static Boolean SetValuesDino3D();
static void move_dino3d_tl();
static void move_dino3d_top();
static void move_dino3d_tr();
static void move_dino3d_left();
static void move_dino3d_cw();
static void move_dino3d_right();
static void move_dino3d_bl();
static void move_dino3d_bottom();
static void move_dino3d_br();
static void move_dino3d_ccw();
static void resize_polyhedrons();
static void draw_frame();
static void diamond_offset_3d();
static void map_from_3d();
static void map_to_3d();
static void cube_offset_3d();

static char defaultTranslationsDino3D[] =
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
   <KeyPress>o: orientize()\n\
   <KeyPress>2: period2()\n\
   <KeyPress>3: period3()\n\
   <KeyPress>b: both()";

static XtActionsRec actionsListDino3D[] =
{
  {"quit", (XtActionProc) quit_dino},
  {"move_tl", (XtActionProc) move_dino3d_tl},
  {"move_top", (XtActionProc) move_dino3d_top},
  {"move_tr", (XtActionProc) move_dino3d_tr},
  {"move_left", (XtActionProc) move_dino3d_left},
  {"move_cw", (XtActionProc) move_dino3d_cw},
  {"move_right", (XtActionProc) move_dino3d_right},
  {"move_bl", (XtActionProc) move_dino3d_bl},
  {"move_bottom", (XtActionProc) move_dino3d_bottom},
  {"move_br", (XtActionProc) move_dino3d_br},
  {"move_ccw", (XtActionProc) move_dino3d_ccw},
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

static XtResource resourcesDino3D[] =
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
   XtOffset(DinoWidget, core.width), XtRString, "250"},
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

Dino3DClassRec dino3dClassRec =
{
  {
    (WidgetClass) &dinoClassRec,	/* superclass */
    "Dino3D",				/* class name */
    sizeof(Dino3DRec),			/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeDino3D,			/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    actionsListDino3D,			/* actions */
    XtNumber(actionsListDino3D),	/* num actions */
    resourcesDino3D,			/* resources */
    XtNumber(resourcesDino3D),		/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    NULL,				/* destroy */
    ResizeDino3D,			/* resize */
    ExposeDino3D,			/* expose */
    SetValuesDino3D,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    XtInheritAcceptFocus,		/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    defaultTranslationsDino3D,		/* tm table */
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

WidgetClass dino3dWidgetClass = (WidgetClass) &dino3dClassRec;

static XPoint face_loc_3d[MAXFACES][MAXORIENT];
static XPoint cube_loc_3d[MAXFACES][MAXORIENT];
static XPoint diamond_loc_3d[MAXFACES][MAXORIENT];
static XPoint tiny_diamond_loc_3d[MAXFACES][MAXORIENT];
static XPoint triangle_loc_3d[MAXFACES][MAXORIENT][3];
static XPoint letter_3d_list[MAXFACES][MAXORIENT];
static XPoint orient_triangle[MAXFACES][MAXORIENT][2];

static void InitializeDino3D(request, new)
  Widget request, new;
{
  Dino3DWidget w = (Dino3DWidget) new;

  w->dino.dim = 3;
  ResizeDino3D(w);
}

static void ResizeDino3D(w)
  Dino3DWidget w;
{
  int temp_width, temp_height;

  w->dino.delta = 4;
  w->dino.vertical = (w->core.height >= w->core.width);
  if (w->dino.vertical) {
    temp_height = w->core.height / MAXVIEWS;
    temp_width = w->core.width;
    if (temp_width >= DIVIDE(temp_height))
    {
      w->dino3d.cube_height = MAX((temp_height - 3 * w->dino.delta) / 2 -
        w->dino.delta - 2, 0);
      w->dino3d.cube_width = DIVIDE(w->dino3d.cube_height);
    } else {
      w->dino3d.cube_width = MAX((temp_width - 2 * w->dino.delta - 7) / 2 -
        w->dino.delta, 0);
      w->dino3d.cube_height = MULTIPLY(w->dino3d.cube_width);
    }
    w->dino3d.cube_diagonal = w->dino3d.cube_width / 2;
    w->dino3d.face_width = w->dino3d.cube_width + 2 * w->dino.delta + 1;
    w->dino3d.face_height = w->dino3d.cube_height + 2 * w->dino.delta + 1;
    w->dino3d.face_diagonal = w->dino3d.face_width / 2;
    w->dino3d.view_width = 2 * w->dino3d.face_width + 3;
    w->dino3d.view_height = 2 * w->dino3d.face_height + 3;
    w->dino.puzzle_width = w->dino3d.view_width + 1;
    w->dino.puzzle_height = MAXVIEWS * w->dino3d.view_height + 1;
  } else {
    temp_width = w->core.width / MAXVIEWS;
    temp_height = w->core.height;
    if (temp_height >= DIVIDE(temp_width)) {
      w->dino3d.cube_width = MAX((temp_width - 3 * w->dino.delta) / 2 -
        w->dino.delta - 2, 0);
      w->dino3d.cube_height = DIVIDE(w->dino3d.cube_width);
    } else {
      w->dino3d.cube_height = MAX((temp_height - 2 * w->dino.delta - 7) / 2 -
        w->dino.delta, 0);
      w->dino3d.cube_width = MULTIPLY(w->dino3d.cube_height);
    }
    w->dino3d.cube_diagonal = w->dino3d.cube_height / 2;
    w->dino3d.face_height = w->dino3d.cube_height + 2 * w->dino.delta + 1;
    w->dino3d.face_width = w->dino3d.cube_width + 2 * w->dino.delta + 1;
    w->dino3d.face_diagonal = w->dino3d.face_height / 2;
    w->dino3d.view_height = 2 * w->dino3d.face_height + 3;
    w->dino3d.view_width = 2 * w->dino3d.face_width + 3;
    w->dino.puzzle_height = w->dino3d.view_height + 1;
    w->dino.puzzle_width = MAXVIEWS * w->dino3d.view_width + 1;
  }
  w->dino.puzzle_offset.x = ((int) w->core.width - w->dino.puzzle_width) / 2;
  w->dino.puzzle_offset.y = ((int) w->core.height - w->dino.puzzle_height) /
    2;
  resize_polyhedrons(w);
}

static void ExposeDino3D(new, event, region)
  Widget new;
  XEvent *event;
  Region region; /* Not used */
{
  Dino3DWidget w = (Dino3DWidget) new;

  if (w->core.visible) {
    draw_frame(w, w->dino.puzzle_GC);
    draw_all_polyhedrons((DinoWidget) w);
  }
}

static Boolean SetValuesDino3D(current, request, new)
  Widget current, request, new;
{
  Dino3DWidget c = (Dino3DWidget) current, w = (Dino3DWidget) new;
  Boolean redraw = FALSE;

  if (w->dino3d.cube_width != c->dino3d.cube_width) {
    ResizeDino3D(w);
    redraw = TRUE;
  }
  return (redraw);
}

static void move_dino3d_tl(w, event, args, n_args)
  Dino3DWidget w;
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

static void move_dino3d_top(w, event, args, n_args)
  Dino3DWidget w;
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

static void move_dino3d_tr(w, event, args, n_args)
  Dino3DWidget w;
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

static void move_dino3d_left(w, event, args, n_args)
  Dino3DWidget w;
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

static void move_dino3d_cw(w, event, args, n_args)
  Dino3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_dino_input((DinoWidget) w, event->xbutton.x, event->xbutton.y, CW,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask),
    (int) (event->xkey.state &
      (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask)));
}

static void move_dino3d_right(w, event, args, n_args)
  Dino3DWidget w;
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

static void move_dino3d_bl(w, event, args, n_args)
  Dino3DWidget w;
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

static void move_dino3d_bottom(w, event, args, n_args)
  Dino3DWidget w;
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

static void move_dino3d_br(w, event, args, n_args)
  Dino3DWidget w;
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

static void move_dino3d_ccw(w, event, args, n_args)
  Dino3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_dino_input((DinoWidget) w, event->xbutton.x, event->xbutton.y, CCW,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask),
    (int) (event->xkey.state &
      (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask)));
}

static void resize_polyhedrons(w)
  Dino3DWidget w;
{
  int face, orient, side, corner;
  XPoint subcube_loc_3d[MAXFACES][MAXORIENT];
  XPoint orient_cube_loc_3d[2][MAXFACES][MAXORIENT];
  XPoint subdiamond_loc_3d[MAXFACES][MAXORIENT];
  XPoint orient_diamond_loc_3d[2][MAXFACES][MAXORIENT];

  w->dino.letter_offset.x = -2;
  w->dino.letter_offset.y = 3;
  w->dino3d.view_middle.x = w->dino3d.face_width +
    w->dino.puzzle_offset.x;
  w->dino3d.view_middle.y = w->dino3d.face_height +
    w->dino.puzzle_offset.y;
  for (face = 0; face < MAXFACES; face++) {
    face_loc_3d[face][0].x = w->dino3d.view_middle.x;
    face_loc_3d[face][0].y = w->dino3d.view_middle.y;
    for (orient = 1; orient < MAXORIENT; orient++) {
      face_loc_3d[face][orient].x = w->dino3d.face_width;
      face_loc_3d[face][orient].y = w->dino3d.face_height;
    }
  }
  if (w->dino.vertical) {
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
      face_loc_3d[face][0].y += w->dino3d.view_height + 3;

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
       face_loc_3d[face][0].x += w->dino3d.view_width + 3;

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
    orient_cube_loc_3d[0][face][0].x = orient_cube_loc_3d[1][face][0].x = 0;
    orient_cube_loc_3d[0][face][0].y = orient_cube_loc_3d[1][face][0].y = 0;
    for (orient = 1; orient < MAXORIENT; orient++) {
      cube_loc_3d[face][orient].x = face_loc_3d[face][orient].x - 3 *
        w->dino.delta * face_loc_3d[face][orient].x /
        w->dino3d.face_width;
      cube_loc_3d[face][orient].y = face_loc_3d[face][orient].y - 3 *
        w->dino.delta * face_loc_3d[face][orient].y /
        w->dino3d.face_height;
      subcube_loc_3d[face][orient].x = face_loc_3d[face][orient].x -
        4 * face_loc_3d[face][orient].x * w->dino.delta /
        w->dino3d.face_width;
      subcube_loc_3d[face][orient].y = face_loc_3d[face][orient].y -
        4 * face_loc_3d[face][orient].y * w->dino.delta /
        w->dino3d.face_height;
      orient_cube_loc_3d[0][face][orient].x = (face_loc_3d[face][orient].x -
        5 * face_loc_3d[face][orient].x * w->dino.delta /
        w->dino3d.face_width) / 3;
      orient_cube_loc_3d[0][face][orient].y = (face_loc_3d[face][orient].y -
        5 * face_loc_3d[face][orient].y * w->dino.delta /
        w->dino3d.face_height) / 3;
      orient_cube_loc_3d[1][face][orient].x = (face_loc_3d[face][orient].x -
        7 * face_loc_3d[face][orient].x * w->dino.delta /
        w->dino3d.face_width) / 6;
      orient_cube_loc_3d[1][face][orient].y = (face_loc_3d[face][orient].y -
        7 * face_loc_3d[face][orient].y * w->dino.delta /
        w->dino3d.face_height) / 6;
    }

    map_from_3d(face, 1, &corner);
    orient_triangle[face][corner][0].x = -orient_cube_loc_3d[0][face][2].x / 2;
    orient_triangle[face][corner][0].y = -orient_cube_loc_3d[0][face][2].y / 2;
    orient_triangle[face][corner][1].x = -orient_cube_loc_3d[1][face][2].x / 2;
    orient_triangle[face][corner][1].y = -orient_cube_loc_3d[1][face][2].y / 2;
    for (orient = 0; orient < MAXORIENT - 1; orient++) {
      diamond_loc_3d[face][orient].x = (cube_loc_3d[face][orient].x +
        cube_loc_3d[face][orient + 1].x) / 2;
      diamond_loc_3d[face][orient].y = (cube_loc_3d[face][orient].y +
        cube_loc_3d[face][orient + 1].y) / 2;
      subdiamond_loc_3d[face][orient].x = (subcube_loc_3d[face][orient].x +
        subcube_loc_3d[face][orient + 1].x) / 2;
      subdiamond_loc_3d[face][orient].y = (subcube_loc_3d[face][orient].y +
        subcube_loc_3d[face][orient + 1].y) / 2;
      tiny_diamond_loc_3d[face][orient + 1].x = diamond_loc_3d[face][orient].x -
        subdiamond_loc_3d[face][orient].x; 
      tiny_diamond_loc_3d[face][orient + 1].y = diamond_loc_3d[face][orient].y -
        subdiamond_loc_3d[face][orient].y; 
           side = corner;
      orient_diamond_loc_3d[0][face][orient].x =
        (orient_cube_loc_3d[0][face][orient].x +
        orient_cube_loc_3d[0][face][orient + 1].x) / 2;
      orient_diamond_loc_3d[0][face][orient].y =
        (orient_cube_loc_3d[0][face][orient].y +
        orient_cube_loc_3d[0][face][orient + 1].y) / 2;
      orient_diamond_loc_3d[1][face][orient].x =
        (orient_cube_loc_3d[1][face][orient].x +
        orient_cube_loc_3d[1][face][orient + 1].x) / 2;
      orient_diamond_loc_3d[1][face][orient].y =
        (orient_cube_loc_3d[1][face][orient].y +
        orient_cube_loc_3d[1][face][orient + 1].y) / 2;
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
    tiny_diamond_loc_3d[face][0].x = diamond_loc_3d[face][orient].x -
      subdiamond_loc_3d[face][orient].x; 
    tiny_diamond_loc_3d[face][0].y = diamond_loc_3d[face][orient].y -
      subdiamond_loc_3d[face][orient].y; 
    orient_diamond_loc_3d[0][face][orient].x =
      (orient_cube_loc_3d[0][face][MAXORIENT - 1].x -
      orient_cube_loc_3d[0][face][MAXORIENT / 2].x) / 2;
    orient_diamond_loc_3d[0][face][orient].y =
      (orient_cube_loc_3d[0][face][MAXORIENT - 1].y -
      orient_cube_loc_3d[0][face][MAXORIENT / 2].y) / 2;
    orient_diamond_loc_3d[1][face][orient].x =
      (orient_cube_loc_3d[1][face][MAXORIENT - 1].x -
      orient_cube_loc_3d[1][face][MAXORIENT / 2].x) / 2;
    orient_diamond_loc_3d[1][face][orient].y =
      (orient_cube_loc_3d[1][face][MAXORIENT - 1].y -
      orient_cube_loc_3d[1][face][MAXORIENT / 2].y) / 2;

    map_from_3d(face, 1, &corner);
    orient_triangle[face][corner][0].x = -orient_cube_loc_3d[0][face][2].x / 2;
    orient_triangle[face][corner][0].y = -orient_cube_loc_3d[0][face][2].y / 2;
    orient_triangle[face][corner][1].x = -orient_cube_loc_3d[1][face][2].x / 2;
    orient_triangle[face][corner][1].y = -orient_cube_loc_3d[1][face][2].y / 2;

    triangle_loc_3d[face][0][1].x = subdiamond_loc_3d[face][2].x;
    triangle_loc_3d[face][0][2].x = -subdiamond_loc_3d[face][1].x -
      subdiamond_loc_3d[face][2].x;
    triangle_loc_3d[face][0][1].y = subdiamond_loc_3d[face][2].y;
    triangle_loc_3d[face][0][2].y = -subdiamond_loc_3d[face][1].y -
      subdiamond_loc_3d[face][2].y;
    triangle_loc_3d[face][1][1].x = -subdiamond_loc_3d[face][1].x;
    triangle_loc_3d[face][1][2].x = subdiamond_loc_3d[face][1].x -
      subdiamond_loc_3d[face][2].x;
    triangle_loc_3d[face][1][1].y = -subdiamond_loc_3d[face][1].y;
    triangle_loc_3d[face][1][2].y = subdiamond_loc_3d[face][1].y -
      subdiamond_loc_3d[face][2].y;
    triangle_loc_3d[face][2][1].x = -subdiamond_loc_3d[face][2].x;
    triangle_loc_3d[face][2][2].x = subdiamond_loc_3d[face][1].x +
      subdiamond_loc_3d[face][2].x;
    triangle_loc_3d[face][2][1].y = -subdiamond_loc_3d[face][2].y;
    triangle_loc_3d[face][2][2].y = subdiamond_loc_3d[face][1].y +
      subdiamond_loc_3d[face][2].y;
    triangle_loc_3d[face][3][1].x = subdiamond_loc_3d[face][1].x;
    triangle_loc_3d[face][3][2].x = subdiamond_loc_3d[face][2].x -
      subdiamond_loc_3d[face][1].x;
    triangle_loc_3d[face][3][1].y = subdiamond_loc_3d[face][1].y;
    triangle_loc_3d[face][3][2].y = subdiamond_loc_3d[face][2].y -
      subdiamond_loc_3d[face][1].y;
    for (orient = 0; orient < MAXORIENT; orient++) {
      letter_3d_list[face][orient].x =
        (2 * triangle_loc_3d[face][orient][1].x +
         triangle_loc_3d[face][orient][2].x) / 3;
      letter_3d_list[face][orient].y =
        (2 * triangle_loc_3d[face][orient][1].y +
         triangle_loc_3d[face][orient][2].y) / 3;
    }
    for (orient = 1; orient < MAXORIENT; orient++) {
      side = corner;
      map_from_3d(face, (orient + 1) % MAXORIENT, &corner);
      orient_triangle[face][corner][0].x =
        orient_triangle[face][side][0].x +
        orient_diamond_loc_3d[0][face][orient].x;
      orient_triangle[face][corner][0].y =
        orient_triangle[face][side][0].y +
        orient_diamond_loc_3d[0][face][orient].y;
      orient_triangle[face][corner][1].x =
        orient_triangle[face][side][1].x +
        orient_diamond_loc_3d[1][face][orient].x;
      orient_triangle[face][corner][1].y =
        orient_triangle[face][side][1].y +
        orient_diamond_loc_3d[1][face][orient].y;
    }
  }
  w->dino3d.cube_width = w->dino3d.face_width - 2 * w->dino.delta;
  w->dino3d.cube_height = w->dino3d.face_height - 2 * w->dino.delta;
  w->dino3d.cube_diagonal = w->dino3d.face_diagonal - 2 * w->dino.delta;
  w->dino3d.cube_diag = w->dino3d.face_diagonal + 2 * w->dino.delta;
}

int position_polyhedrons3d(w, x, y, face, side, direction)
  Dino3DWidget w;
  int x, y;
  int *face, *side, *direction;
{
  int u, v, front, tl, ur, ul, found, corner, x1, y1, x2, y2, dx, dy;

  x1 = x;
  y1 = y;
  if (w->dino.vertical) {
    x -= w->dino3d.view_middle.x;
    front = (y < w->dino3d.view_height);
    if (!front)
      y -= (w->dino3d.view_height);
    tl = (y < w->dino3d.view_middle.y); 
    y -= w->dino3d.view_middle.y;
    u = -w->dino3d.face_height * x + w->dino3d.face_diagonal * y;
    v = w->dino3d.face_height * x + w->dino3d.face_diagonal * y;
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
    y -= w->dino3d.view_middle.y;
    front = (x < w->dino3d.view_width);
    if (!front)
      x -= (w->dino3d.view_width);
    tl = (x < w->dino3d.view_middle.x); 
    x -= w->dino3d.view_middle.x;
    u = -w->dino3d.face_width * y + w->dino3d.face_diagonal * x;
    v = w->dino3d.face_width * y + w->dino3d.face_diagonal * x;
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
  x = x1;
  y = y1;
  switch (*direction) {
    case TR:
    case BR:
    case BL:
    case TL:
      if (w->dino.vertical) {
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
      if (*face == 3 || *face == 4)
        *direction = (TR + *direction - CW) % MAXORIENT;
      else if (*face == 0)
        *direction = (BR + *direction - CW) % MAXORIENT;
      else if (*face == 2)
        *direction = (BL + *direction - CW) % MAXORIENT;
      else if (*face == 1 || *face == 5)
        *direction = (TL + *direction - CW) % MAXORIENT;
      break;
    case TOP:
    case RIGHT:
    case BOTTOM:
    case LEFT:
      if (w->dino.vertical) {
        if (*face == 1 || *face == 2 || *face == 4)
          *direction = (TL + *direction) % MAXORIENT;
        else if (*face == 0 || *face == 5)
          *direction = (TR + *direction) % MAXORIENT;
        else /* (*face == 3) */
          *direction = (BL + *direction) % MAXORIENT;
      } else {
        if (*face == 0 || *face == 2 || *face == 3)
          *direction = (TR + *direction) % MAXORIENT;
        else if (*face == 1)
          *direction = (TL + *direction) % MAXORIENT;
        else if (*face == 4)
          *direction = (BR + *direction) % MAXORIENT;
        else /* (*face == 5) */
          *direction = (BL + *direction) % MAXORIENT;
      }
      break;
    default:
      return FALSE;
  }
  found = 0;
  diamond_offset_3d(w, *face, &dx, &dy);
  for (corner = 0; corner < MAXORIENT; corner++) {
    int ok = 0;

    x1 = dx + triangle_loc_3d[*face][corner][1].x;
    y1 = dy + triangle_loc_3d[*face][corner][1].y;
    x2 = x1 + triangle_loc_3d[*face][corner][2].x;
    y2 = y1 + triangle_loc_3d[*face][corner][2].y;
    if ((x1 - dx) * (y2 - dy) <= (y1 - dy) * (x2 - dx)) {
      if ((x1 - dx) * (y - dy) <= (y1 - dy) * (x - dx))
        ok++;
    } else if ((x1 - dx) * (y - dy) >= (y1 - dy) * (x - dx))
      ok++;
    if ((x2 - dx) * (y1 - dy) <= (y2 - dy) * (x1 - dx)) {
      if ((x2 - dx) * (y - dy) <= (y2 - dy) * (x - dx))
        ok++;
    } else if ((x2 - dx) * (y - dy) >= (y2 - dy) * (x - dx))
      ok++;
    if (ok == 2) {
      map_from_3d(*face, corner, side);
      found++;
    }
  }
  if (found != 1)
    return FALSE;
  return TRUE;
}

static void draw_frame(w, gc)
  Dino3DWidget w;
  GC gc;
{
  int face, dx, dy;

  dx = w->dino3d.view_width + w->dino.puzzle_offset.x;
  dy = w->dino3d.view_height + w->dino.puzzle_offset.y;
  if (w->dino.vertical) {
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      0, dy, dx + w->dino.puzzle_offset.x + 1, dy);
    XDrawString(XtDisplay(w), XtWindow(w), gc,
      (int) (2 * w->dino.delta),
      (int) (3 * w->dino.delta + w->dino.letter_offset.y),
      "Front", 5);
    XDrawString(XtDisplay(w), XtWindow(w), gc, (int)
      (-4 * w->dino.delta + 2 * 4 * w->dino.letter_offset.x + w->core.width), 
      (int) (-w->dino.delta - 2 * w->dino.letter_offset.y + w->core.height),
      "Back", 4);
  } else {
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      dx, 0, dx, dy + w->dino.puzzle_offset.y + 1);
    XDrawString(XtDisplay(w), XtWindow(w), gc,
      (int) (2 * w->dino.delta),
      (int) (3 * w->dino.delta + w->dino.letter_offset.y),
      "Front", 5);
    XDrawString(XtDisplay(w), XtWindow(w), gc, (int)
      (-4 * w->dino.delta + 2 * 4 * w->dino.letter_offset.x + w->core.width), 
      (int) (-w->dino.delta - 2 * w->dino.letter_offset.y + w->core.height),
      "Back", 4);
  }
  for (face = 0; face < MAXFACES; face++)
    XDrawLines(XtDisplay(w), XtWindow(w), gc,
      face_loc_3d[face], MAXORIENT, CoordModePrevious);
}   

void draw_triangle3d(w, face, side)
  Dino3DWidget w;
  int face, side;
{
  int corner, dx, dy, letter_x, letter_y;

  map_to_3d(face, side, &corner);
  cube_offset_3d(w, face, corner, &dx, &dy);
  letter_x = dx + letter_3d_list[face][corner].x;
  letter_y = dy + letter_3d_list[face][corner].y;
  triangle_loc_3d[face][corner][0].x = dx;
  triangle_loc_3d[face][corner][0].y = dy;
  XFillPolygon(XtDisplay(w), XtWindow(w),
    w->dino.face_GC[w->dino.cube_loc[face][side].face],
    triangle_loc_3d[face][corner], 3, Convex, CoordModePrevious);
  if (w->dino.depth == 1 || w->dino.mono) {
    char buf[2];

    (void) sprintf (buf, "%c",
      w->dino.face_name[w->dino.cube_loc[face][side].face][0]);
    XDrawString(XtDisplay(w), XtWindow(w), w->dino.inverse_GC,
      letter_x + w->dino.letter_offset.x, letter_y + w->dino.letter_offset.y,
      buf, 1);
  }
  if (w->dino.orient)
    XDrawLine(XtDisplay(w), XtWindow(w), w->dino.inverse_GC,
      letter_x +
        orient_triangle[face][w->dino.cube_loc[face][side].rotation][0].x,
      letter_y +
        orient_triangle[face][w->dino.cube_loc[face][side].rotation][0].y,
      letter_x +
        orient_triangle[face][w->dino.cube_loc[face][side].rotation][1].x,
      letter_y +
        orient_triangle[face][w->dino.cube_loc[face][side].rotation][1].y);
}

static void map_to_3d(face, corner, side)
  int face, corner, *side;
{
  switch (face) {
    case 0:
      *side = (corner + 2) % MAXORIENT;
      break;
    case 1:
    case 5:
      *side = corner;
      break;
    case 2:
      *side = (corner + 1) % MAXORIENT;
      break;
    case 3:
    case 4:
      *side = (corner + 3) % MAXORIENT;
      break;
    default:
      (void) printf ("map_to_3d: face %d\n", face);
  }
}

static void map_from_3d(face, side, corner)
  int face, side, *corner;
{
  switch (face) {
    case 0:
      *corner = (side + 2) % MAXORIENT;
      break;
    case 1:
    case 5:
      *corner = side;
      break;
    case 2:
      *corner = (side + 3) % MAXORIENT;
      break;
    case 3:
    case 4:
      *corner = (side + 1) % MAXORIENT;
      break;
    default:
      (void) printf ("map_from_3d: face %d\n", face);
  }
}

static void diamond_offset_3d(w, face, dx, dy)
  Dino3DWidget w;
  int face, *dx, *dy;
{
  if (w->dino.vertical) {
    switch (face) {
      case 0:
        *dx = (2 * face_loc_3d[0][0].x + face_loc_3d[0][1].x +
               face_loc_3d[0][2].x) / 2;
        *dy = (2 * face_loc_3d[0][0].y + face_loc_3d[0][1].y) / 2;
        break;
      case 1:
        *dx = face_loc_3d[1][0].x + face_loc_3d[1][1].x;
        *dy = face_loc_3d[1][0].y;
        break;
      case 2:
        *dx = (2 * face_loc_3d[2][0].x + face_loc_3d[2][1].x +
               face_loc_3d[2][2].x) / 2;
        *dy = (2 * face_loc_3d[2][0].y + face_loc_3d[2][1].y +
               face_loc_3d[2][2].y) / 2;
        break;
      case 3:
        *dx = face_loc_3d[3][0].x + face_loc_3d[3][1].x;
        *dy = face_loc_3d[3][0].y;
        break;
      case 4:
        *dx = (2 * face_loc_3d[4][0].x + face_loc_3d[4][1].x +
               face_loc_3d[4][2].x) / 2;
        *dy = (2 * face_loc_3d[4][0].y + face_loc_3d[4][1].y +
               face_loc_3d[4][2].y) / 2;
        break;
      case 5:
        *dx = (2 * face_loc_3d[5][0].x + face_loc_3d[5][1].x +
               face_loc_3d[5][2].x) / 2;
        *dy = (2 * face_loc_3d[5][0].y + face_loc_3d[5][1].y) / 2;
        break;
      default:
        (void) printf ("diamond_offset_3d: face %d\n", face);
    }
  }
  else
  {
    switch (face)
    {
      case 0:
        *dx = face_loc_3d[0][0].x;
        *dy = face_loc_3d[0][0].y + face_loc_3d[0][1].y;
        break;
      case 1:
        *dx = (2 * face_loc_3d[1][0].x + face_loc_3d[1][1].x +
               face_loc_3d[1][2].x) / 2;
        *dy = (2 * face_loc_3d[1][0].y + face_loc_3d[1][1].y +
               face_loc_3d[1][2].y) / 2;
        break;
      case 2:
        *dx = (2 * face_loc_3d[2][0].x + face_loc_3d[2][1].x) / 2;
        *dy = (2 * face_loc_3d[2][0].y + face_loc_3d[2][1].y +
               face_loc_3d[2][2].y) / 2;
        break;
      case 3:
        *dx = (2 * face_loc_3d[3][0].x + face_loc_3d[3][1].x) / 2;
        *dy = (2 * face_loc_3d[3][0].y + face_loc_3d[3][1].y +
               face_loc_3d[3][2].y) / 2;
        break;
      case 4:
        *dx = face_loc_3d[4][0].x;
        *dy = face_loc_3d[4][0].y + face_loc_3d[4][1].y;
        break;
      case 5:
        *dx = (2 * face_loc_3d[5][0].x + face_loc_3d[5][1].x +
               face_loc_3d[5][2].x) / 2;
        *dy = (2 * face_loc_3d[5][0].y + face_loc_3d[5][1].y +
               face_loc_3d[5][2].y) / 2;
        break;
      default:
        (void) printf ("diamond_offset_3d: face %d\n", face);
    }
  }
}

static void cube_offset_3d(w, face, side, dx, dy)
  Dino3DWidget w;
  int face, side, *dx, *dy;
{
  int corner;

  diamond_offset_3d(w, face, dx, dy);
  for (corner = 1; corner <= side; corner++)
  {
    *dx += tiny_diamond_loc_3d[face][corner].x;
    *dy += tiny_diamond_loc_3d[face][corner].y;
  }
}
