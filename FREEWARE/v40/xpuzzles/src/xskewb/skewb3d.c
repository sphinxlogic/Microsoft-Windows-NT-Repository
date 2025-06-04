/*
# X-BASED SKEWB
#
#  Skewb3d.c
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

/* Methods file for Skewb3d */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include "SkewbP.h"
#include "Skewb3dP.h"

static void InitializeSkewb3D();
static void ExposeSkewb3D();
static void ResizeSkewb3D();
static Boolean SetValuesSkewb3D();
static void move_skewb3d_tl();
static void move_skewb3d_top();
static void move_skewb3d_tr();
static void move_skewb3d_left();
static void move_skewb3d_right();
static void move_skewb3d_bl();
static void move_skewb3d_bottom();
static void move_skewb3d_br();
static void resize_polyhedrons();
static void draw_frame();
static void diamond_offset_3d();
static void map_from_3d();
static void map_to_3d();
static void cube_offset_3d();

static char defaultTranslationsSkewb3D[] =
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
   <KeyPress>o: orientize()";

static XtActionsRec actionsListSkewb3D[] =
{
  {"quit", (XtActionProc) quit_skewb},
  {"move_tl", (XtActionProc) move_skewb3d_tl},
  {"move_top", (XtActionProc) move_skewb3d_top},
  {"move_tr", (XtActionProc) move_skewb3d_tr},
  {"move_left", (XtActionProc) move_skewb3d_left},
  {"move_cw", (XtActionProc) move_skewb_cw},
  {"move_right", (XtActionProc) move_skewb3d_right},
  {"move_bl", (XtActionProc) move_skewb3d_bl},
  {"move_bottom", (XtActionProc) move_skewb3d_bottom},
  {"move_br", (XtActionProc) move_skewb3d_br},
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

static XtResource resourcesSkewb3D[] =
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
   XtOffset(SkewbWidget, core.width), XtRString, "250"},
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

Skewb3DClassRec skewb3dClassRec =
{
  {
    (WidgetClass) &skewbClassRec,	/* superclass */
    "Skewb3D",				/* class name */
    sizeof(Skewb3DRec),			/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeSkewb3D,			/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    actionsListSkewb3D,			/* actions */
    XtNumber(actionsListSkewb3D),	/* num actions */
    resourcesSkewb3D,			/* resources */
    XtNumber(resourcesSkewb3D),		/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    NULL,				/* destroy */
    ResizeSkewb3D,			/* resize */
    ExposeSkewb3D,			/* expose */
    SetValuesSkewb3D,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    XtInheritAcceptFocus,		/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    defaultTranslationsSkewb3D,		/* tm table */
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

WidgetClass skewb3dWidgetClass = (WidgetClass) &skewb3dClassRec;

static XPoint face_loc_3d[MAXFACES][MAXORIENT];
static XPoint cube_loc_3d[MAXFACES][MAXORIENT];
static XPoint diamond_loc_3d[MAXFACES][MAXORIENT];
static XPoint triangle_loc_3d[MAXFACES][MAXORIENT][3];
static XPoint letter_3d_list[MAXFACES][MAXCUBES];
static XPoint orient_diamond[MAXFACES][MAXORIENT][2];
static XPoint orient_triangle[MAXFACES][MAXORIENT][2];

static void InitializeSkewb3D(request, new)
  Widget request, new;
{
  Skewb3DWidget w = (Skewb3DWidget) new;

  w->skewb.dim = 3;
  ResizeSkewb3D(w);
}

static void ResizeSkewb3D(w)
  Skewb3DWidget w;
{
  int temp_width, temp_height;

  w->skewb.delta = 4;
  w->skewb.vertical = (w->core.height >= w->core.width);
  if (w->skewb.vertical) {
    temp_height = w->core.height / MAXVIEWS;
    temp_width = w->core.width;
    if (temp_width >= DIVIDE(temp_height))
    {
      w->skewb3d.cube_height = MAX((temp_height - 3 * w->skewb.delta) / 2 -
        w->skewb.delta - 2, 0);
      w->skewb3d.cube_width = DIVIDE(w->skewb3d.cube_height);
    } else {
      w->skewb3d.cube_width = MAX((temp_width - 2 * w->skewb.delta - 7) / 2 -
        w->skewb.delta, 0);
      w->skewb3d.cube_height = MULTIPLY(w->skewb3d.cube_width);
    }
    w->skewb3d.cube_diagonal = w->skewb3d.cube_width / 2;
    w->skewb3d.face_width = w->skewb3d.cube_width + 2 * w->skewb.delta + 1;
    w->skewb3d.face_height = w->skewb3d.cube_height + 2 * w->skewb.delta + 1;
    w->skewb3d.face_diagonal = w->skewb3d.face_width / 2;
    w->skewb3d.view_width = 2 * w->skewb3d.face_width + 3;
    w->skewb3d.view_height = 2 * w->skewb3d.face_height + 3;
    w->skewb.puzzle_width = w->skewb3d.view_width + 1;
    w->skewb.puzzle_height = MAXVIEWS * w->skewb3d.view_height + 1;
  } else {
    temp_width = w->core.width / MAXVIEWS;
    temp_height = w->core.height;
    if (temp_height >= DIVIDE(temp_width)) {
      w->skewb3d.cube_width = MAX((temp_width - 3 * w->skewb.delta) / 2 -
        w->skewb.delta - 2, 0);
      w->skewb3d.cube_height = DIVIDE(w->skewb3d.cube_width);
    } else {
      w->skewb3d.cube_height = MAX((temp_height - 2 * w->skewb.delta - 7) / 2 -
        w->skewb.delta, 0);
      w->skewb3d.cube_width = MULTIPLY(w->skewb3d.cube_height);
    }
    w->skewb3d.cube_diagonal = w->skewb3d.cube_height / 2;
    w->skewb3d.face_height = w->skewb3d.cube_height + 2 * w->skewb.delta + 1;
    w->skewb3d.face_width = w->skewb3d.cube_width + 2 * w->skewb.delta + 1;
    w->skewb3d.face_diagonal = w->skewb3d.face_height / 2;
    w->skewb3d.view_height = 2 * w->skewb3d.face_height + 3;
    w->skewb3d.view_width = 2 * w->skewb3d.face_width + 3;
    w->skewb.puzzle_height = w->skewb3d.view_height + 1;
    w->skewb.puzzle_width = MAXVIEWS * w->skewb3d.view_width + 1;
  }
  w->skewb.puzzle_offset.x = ((int) w->core.width - w->skewb.puzzle_width) / 2;
  w->skewb.puzzle_offset.y = ((int) w->core.height - w->skewb.puzzle_height) /
    2;
  resize_polyhedrons(w);
}

static void ExposeSkewb3D(new, event, region)
  Widget new;
  XEvent *event;
  Region region; /* Not used */
{
  Skewb3DWidget w = (Skewb3DWidget) new;

  if (w->core.visible) {
    draw_frame(w, w->skewb.puzzle_GC);
    draw_all_polyhedrons((SkewbWidget) w);
  }
}

static Boolean SetValuesSkewb3D(current, request, new)
  Widget current, request, new;
{
  Skewb3DWidget c = (Skewb3DWidget) current, w = (Skewb3DWidget) new;
  Boolean redraw = FALSE;

  if (w->skewb3d.cube_width != c->skewb3d.cube_width) {
    ResizeSkewb3D(w);
    redraw = TRUE;
  }
  return (redraw);
}

static void move_skewb3d_tl(w, event, args, n_args)
  Skewb3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, TL,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb3d_top(w, event, args, n_args)
  Skewb3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, TOP,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb3d_tr(w, event, args, n_args)
  Skewb3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, TR,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb3d_left(w, event, args, n_args)
  Skewb3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, LEFT,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb3d_right(w, event, args, n_args)
  Skewb3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, RIGHT,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb3d_bl(w, event, args, n_args)
  Skewb3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, BL,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb3d_bottom(w, event, args, n_args)
  Skewb3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, BOTTOM,
    (int) (event->xkey.state & ControlMask));
}

static void move_skewb3d_br(w, event, args, n_args)
  Skewb3DWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_skewb_input((SkewbWidget) w, event->xbutton.x, event->xbutton.y, BR,
    (int) (event->xkey.state & ControlMask));
}

static void resize_polyhedrons(w)
  Skewb3DWidget w;
{
  int face, orient, side, corner;
  XPoint subcube_loc_3d[MAXFACES][MAXORIENT];
  XPoint orient_cube_loc_3d[2][MAXFACES][MAXORIENT];
  XPoint subdiamond_loc_3d[MAXFACES][MAXORIENT];
  XPoint orient_diamond_loc_3d[2][MAXFACES][MAXORIENT];

  w->skewb.letter_offset.x = -2;
  w->skewb.letter_offset.y = 3;
  w->skewb3d.view_middle.x = w->skewb3d.face_width +
    w->skewb.puzzle_offset.x;
  w->skewb3d.view_middle.y = w->skewb3d.face_height +
    w->skewb.puzzle_offset.y;
  for (face = 0; face < MAXFACES; face++) {
    face_loc_3d[face][0].x = w->skewb3d.view_middle.x;
    face_loc_3d[face][0].y = w->skewb3d.view_middle.y;
    for (orient = 1; orient < MAXORIENT; orient++) {
      face_loc_3d[face][orient].x = w->skewb3d.face_width;
      face_loc_3d[face][orient].y = w->skewb3d.face_height;
    }
  }
  if (w->skewb.vertical) {
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
      face_loc_3d[face][0].y += w->skewb3d.view_height + 3;

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
       face_loc_3d[face][0].x += w->skewb3d.view_width + 3;

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
        w->skewb.delta * face_loc_3d[face][orient].x /
        w->skewb3d.face_width;
      cube_loc_3d[face][orient].y = face_loc_3d[face][orient].y - 3 *
        w->skewb.delta * face_loc_3d[face][orient].y /
        w->skewb3d.face_height;
      subcube_loc_3d[face][orient].x = (face_loc_3d[face][orient].x -
        5 * face_loc_3d[face][orient].x * w->skewb.delta /
        w->skewb3d.face_width) / 2;
      subcube_loc_3d[face][orient].y = (face_loc_3d[face][orient].y -
        5 * face_loc_3d[face][orient].y * w->skewb.delta /
        w->skewb3d.face_height) / 2;
      orient_cube_loc_3d[0][face][orient].x = (face_loc_3d[face][orient].x -
        5 * face_loc_3d[face][orient].x * w->skewb.delta /
        w->skewb3d.face_width) / 4;
      orient_cube_loc_3d[0][face][orient].y = (face_loc_3d[face][orient].y -
        5 * face_loc_3d[face][orient].y * w->skewb.delta /
        w->skewb3d.face_height) / 4;
      orient_cube_loc_3d[1][face][orient].x = (face_loc_3d[face][orient].x -
        7 * face_loc_3d[face][orient].x * w->skewb.delta /
        w->skewb3d.face_width) / 6;
      orient_cube_loc_3d[1][face][orient].y = (face_loc_3d[face][orient].y -
        7 * face_loc_3d[face][orient].y * w->skewb.delta /
        w->skewb3d.face_height) / 6;
    }
    triangle_loc_3d[face][0][1].x = subcube_loc_3d[face][1].x;
    triangle_loc_3d[face][0][2].x = subcube_loc_3d[face][2].x -
      subcube_loc_3d[face][1].x;
    triangle_loc_3d[face][0][1].y = subcube_loc_3d[face][1].y;
    triangle_loc_3d[face][0][2].y = subcube_loc_3d[face][2].y -
      subcube_loc_3d[face][1].y;
    triangle_loc_3d[face][1][1].x = subcube_loc_3d[face][2].x;
    triangle_loc_3d[face][1][2].x = -subcube_loc_3d[face][1].x -
      subcube_loc_3d[face][2].x;
    triangle_loc_3d[face][1][1].y = subcube_loc_3d[face][2].y;
    triangle_loc_3d[face][1][2].y = -subcube_loc_3d[face][1].y -
      subcube_loc_3d[face][2].y;
    triangle_loc_3d[face][2][1].x = -subcube_loc_3d[face][1].x;
    triangle_loc_3d[face][2][2].x = subcube_loc_3d[face][1].x -
      subcube_loc_3d[face][2].x;
    triangle_loc_3d[face][2][1].y = -subcube_loc_3d[face][1].y;
    triangle_loc_3d[face][2][2].y = subcube_loc_3d[face][1].y -
      subcube_loc_3d[face][2].y;
    triangle_loc_3d[face][3][1].x = -subcube_loc_3d[face][2].x;
    triangle_loc_3d[face][3][2].x = subcube_loc_3d[face][1].x +
      subcube_loc_3d[face][2].x;
    triangle_loc_3d[face][3][1].y = -subcube_loc_3d[face][2].y;
    triangle_loc_3d[face][3][2].y = subcube_loc_3d[face][1].y +
      subcube_loc_3d[face][2].y;
    for (orient = 0; orient < MAXORIENT; orient++) {
      letter_3d_list[face][orient].x =
        (2 * triangle_loc_3d[face][orient][1].x +
         triangle_loc_3d[face][orient][2].x) / 3;
      letter_3d_list[face][orient].y =
        (2 * triangle_loc_3d[face][orient][1].y +
         triangle_loc_3d[face][orient][2].y) / 3;
    }
  }
  w->skewb3d.cube_width = w->skewb3d.face_width - 2 * w->skewb.delta;
  w->skewb3d.cube_height = w->skewb3d.face_height - 2 * w->skewb.delta;
  w->skewb3d.cube_diagonal = w->skewb3d.face_diagonal - 2 * w->skewb.delta;
  w->skewb3d.cube_diag = w->skewb3d.face_diagonal + 2 * w->skewb.delta;

  if (w->skewb.vertical) {
    letter_3d_list[0][MAXORIENT].x = w->skewb3d.cube_width / 4;
    letter_3d_list[0][MAXORIENT].y = -w->skewb3d.cube_height / 2 + 2;
    letter_3d_list[1][MAXORIENT].x = -w->skewb3d.cube_diagonal;
    letter_3d_list[1][MAXORIENT].y = 0;
    letter_3d_list[2][MAXORIENT].x = w->skewb3d.cube_width / 4;
    letter_3d_list[2][MAXORIENT].y = w->skewb3d.cube_height / 2 - 2;
    letter_3d_list[3][MAXORIENT].x = w->skewb3d.cube_diagonal;
    letter_3d_list[3][MAXORIENT].y = 0;
    letter_3d_list[4][MAXORIENT].x = -w->skewb3d.cube_width / 4;
    letter_3d_list[4][MAXORIENT].y = -w->skewb3d.cube_height / 2 + 2;
    letter_3d_list[5][MAXORIENT].x = -w->skewb3d.cube_width / 4;
    letter_3d_list[5][MAXORIENT].y = w->skewb3d.cube_height / 2 - 2;
  } else {
    letter_3d_list[0][MAXORIENT].x = 0;
    letter_3d_list[0][MAXORIENT].y = -w->skewb3d.cube_diagonal;
    letter_3d_list[1][MAXORIENT].x = -w->skewb3d.cube_width / 2 + 2;
    letter_3d_list[1][MAXORIENT].y = w->skewb3d.cube_height / 4;
    letter_3d_list[2][MAXORIENT].x = w->skewb3d.cube_width / 2 - 2;
    letter_3d_list[2][MAXORIENT].y = w->skewb3d.cube_height / 4;
    letter_3d_list[3][MAXORIENT].x = -w->skewb3d.cube_width / 2 + 2;
    letter_3d_list[3][MAXORIENT].y = -w->skewb3d.cube_height / 4;
    letter_3d_list[4][MAXORIENT].x = 0;
    letter_3d_list[4][MAXORIENT].y = w->skewb3d.cube_diagonal;
    letter_3d_list[5][MAXORIENT].x = w->skewb3d.cube_width / 2 - 2;
    letter_3d_list[5][MAXORIENT].y = -w->skewb3d.cube_height / 4;
  }

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
    orient_diamond[face][corner][0].x = cube_loc_3d[face][1].x / 2;
    orient_diamond[face][corner][0].y = cube_loc_3d[face][1].y / 2;
    orient_diamond[face][corner][1].x = orient_diamond[face][corner][0].x +
      (cube_loc_3d[face][2].x - subcube_loc_3d[face][2].x) / 2;
    orient_diamond[face][corner][1].y = orient_diamond[face][corner][0].y +
      (cube_loc_3d[face][2].y - subcube_loc_3d[face][2].y) / 2;
    orient_triangle[face][corner][0].x = -orient_cube_loc_3d[0][face][2].x / 2;
    orient_triangle[face][corner][0].y = -orient_cube_loc_3d[0][face][2].y / 2;
    orient_triangle[face][corner][1].x = -orient_cube_loc_3d[1][face][2].x / 2;
    orient_triangle[face][corner][1].y = -orient_cube_loc_3d[1][face][2].y / 2;
    for (orient = 1; orient < MAXORIENT; orient++) {
      side = corner;
      map_from_3d(face, (orient + 1) % MAXORIENT, &corner);
      orient_diamond[face][corner][0].x =
        orient_diamond[face][side][0].x +
        diamond_loc_3d[face][orient].x;
      orient_diamond[face][corner][0].y =
        orient_diamond[face][side][0].y +
        diamond_loc_3d[face][orient].y;
      orient_diamond[face][corner][1].x =
        orient_diamond[face][side][1].x +
        subdiamond_loc_3d[face][orient].x;
      orient_diamond[face][corner][1].y =
        orient_diamond[face][side][1].y +
        subdiamond_loc_3d[face][orient].y;
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
}

int position_polyhedrons3d(w, x, y, face, corner, direction)
  Skewb3DWidget w;
  int x, y;
  int *face, *corner, *direction;
{
  int u, v, front, tl, ur, ul, found, side, x1, y1, x2, y2, dx, dy;

  x1 = x;
  y1 = y;
  if (w->skewb.vertical) {
    x -= w->skewb3d.view_middle.x;
    front = (y < w->skewb3d.view_height);
    if (!front)
      y -= (w->skewb3d.view_height);
    tl = (y < w->skewb3d.view_middle.y); 
    y -= w->skewb3d.view_middle.y;
    u = -w->skewb3d.face_height * x + w->skewb3d.face_diagonal * y;
    v = w->skewb3d.face_height * x + w->skewb3d.face_diagonal * y;
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
    y -= w->skewb3d.view_middle.y;
    front = (x < w->skewb3d.view_width);
    if (!front)
      x -= (w->skewb3d.view_width);
    tl = (x < w->skewb3d.view_middle.x); 
    x -= w->skewb3d.view_middle.x;
    u = -w->skewb3d.face_width * y + w->skewb3d.face_diagonal * x;
    v = w->skewb3d.face_width * y + w->skewb3d.face_diagonal * x;
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
      if (w->skewb.vertical) {
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
    case TOP:
    case RIGHT:
    case BOTTOM:
    case LEFT:
      if (w->skewb.vertical) {
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
  for (side = 0; side < MAXORIENT; side++) {
    x1 = dx + orient_diamond[*face][side][0].x;
    y1 = dy + orient_diamond[*face][side][0].y;
    x2 = dx + orient_diamond[*face][(side + 1) % MAXORIENT][0].x;
    y2 = dy + orient_diamond[*face][(side + 1) % MAXORIENT][0].y;
    if ((x2 - x1) * (y - y1) <= (y2 - y1) * (x - x1)) {
      *corner = side;
      found++;
    }
  }
  if (*direction == CW)
    *direction = (*corner + 1) % MAXORIENT;
  else if (*direction == CCW)
    *direction = (*corner + 3) % MAXORIENT;
  if (found == 0) {
    *corner = MAXORIENT;
    return FALSE;
  } else if (found > 1 || ((*corner + *direction) % 2 == 0))
    return FALSE;
  return TRUE;
}

static void draw_frame(w, gc)
  Skewb3DWidget w;
  GC gc;
{
  int face, dx, dy;

  dx = w->skewb3d.view_width + w->skewb.puzzle_offset.x;
  dy = w->skewb3d.view_height + w->skewb.puzzle_offset.y;
  if (w->skewb.vertical) {
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      0, dy, dx + w->skewb.puzzle_offset.x + 1, dy);
    XDrawString(XtDisplay(w), XtWindow(w), gc,
      (int) (2 * w->skewb.delta),
      (int) (3 * w->skewb.delta + w->skewb.letter_offset.y),
      "Front", 5);
    XDrawString(XtDisplay(w), XtWindow(w), gc, (int)
      (-4 * w->skewb.delta + 2 * 4 * w->skewb.letter_offset.x + w->core.width), 
      (int) (-w->skewb.delta - 2 * w->skewb.letter_offset.y + w->core.height),
      "Back", 4);
  } else {
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      dx, 0, dx, dy + w->skewb.puzzle_offset.y + 1);
    XDrawString(XtDisplay(w), XtWindow(w), gc,
      (int) (2 * w->skewb.delta),
      (int) (3 * w->skewb.delta + w->skewb.letter_offset.y),
      "Front", 5);
    XDrawString(XtDisplay(w), XtWindow(w), gc, (int)
      (-4 * w->skewb.delta + 2 * 4 * w->skewb.letter_offset.x + w->core.width), 
      (int) (-w->skewb.delta - 2 * w->skewb.letter_offset.y + w->core.height),
      "Back", 4);
  }
  for (face = 0; face < MAXFACES; face++)
    XDrawLines(XtDisplay(w), XtWindow(w), gc,
      face_loc_3d[face], MAXORIENT, CoordModePrevious);
}   

void draw_diamond3d(w, face)
  Skewb3DWidget w;
  int face;
{
  int dx, dy;

  diamond_offset_3d(w, face, &dx, &dy);
  diamond_loc_3d[face][0].x = dx + cube_loc_3d[face][1].x / 2;
  diamond_loc_3d[face][0].y = dy + cube_loc_3d[face][1].y / 2;
  XFillPolygon(XtDisplay(w), XtWindow(w),
    w->skewb.face_GC[w->skewb.cube_loc[face][MAXORIENT].face],
    diamond_loc_3d[face], MAXORIENT, Convex, CoordModePrevious);
  if (w->skewb.depth == 1 || w->skewb.mono) {
    int letter_x, letter_y;
    char buf[2];

    (void) sprintf (buf, "%c",
      w->skewb.face_name[w->skewb.cube_loc[face][MAXORIENT].face][0]);
    letter_x = dx + letter_3d_list[face][MAXORIENT].x + w->skewb.letter_offset.x;
    letter_y = dy + letter_3d_list[face][MAXORIENT].y + w->skewb.letter_offset.y;
    XDrawString(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
      letter_x, letter_y, buf, 1);
  }
  if (w->skewb.orient)
    XDrawLine(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
      dx +
        orient_diamond[face][w->skewb.cube_loc[face][MAXORIENT].rotation][0].x,
      dy +
        orient_diamond[face][w->skewb.cube_loc[face][MAXORIENT].rotation][0].y,
      dx +
        orient_diamond[face][w->skewb.cube_loc[face][MAXORIENT].rotation][1].x,
      dy +
        orient_diamond[face][w->skewb.cube_loc[face][MAXORIENT].rotation][1].y);
}

void draw_triangle3d(w, face, corner)
  Skewb3DWidget w;
  int face, corner;
{
  int side, dx, dy, letter_x, letter_y;

  map_to_3d(face, corner, &side);
  cube_offset_3d(w, face, side, &dx, &dy);
  letter_x = dx + letter_3d_list[face][side].x;
  letter_y = dy + letter_3d_list[face][side].y;
  triangle_loc_3d[face][side][0].x = dx;
  triangle_loc_3d[face][side][0].y = dy;
  XFillPolygon(XtDisplay(w), XtWindow(w),
    w->skewb.face_GC[w->skewb.cube_loc[face][corner].face],
    triangle_loc_3d[face][side], 3, Convex, CoordModePrevious);
  if (w->skewb.depth == 1 || w->skewb.mono) {
    char buf[2];

    (void) sprintf (buf, "%c",
      w->skewb.face_name[w->skewb.cube_loc[face][corner].face][0]);
    XDrawString(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
      letter_x + w->skewb.letter_offset.x, letter_y + w->skewb.letter_offset.y,
      buf, 1);
  }
  if (w->skewb.orient)
    XDrawLine(XtDisplay(w), XtWindow(w), w->skewb.inverse_GC,
      letter_x +
        orient_triangle[face][w->skewb.cube_loc[face][corner].rotation][0].x,
      letter_y +
        orient_triangle[face][w->skewb.cube_loc[face][corner].rotation][0].y,
      letter_x +
        orient_triangle[face][w->skewb.cube_loc[face][corner].rotation][1].x,
      letter_y +
        orient_triangle[face][w->skewb.cube_loc[face][corner].rotation][1].y);
}

static void map_to_3d(face, side, corner)
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
      *corner = (side + 1) % MAXORIENT;
      break;
    case 3:
    case 4:
      *corner = (side + 3) % MAXORIENT;
      break;
    default:
      (void) printf ("map_to_3d: face %d\n", face);
  }
}

static void map_from_3d(face, corner, side)
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
      *side = (corner + 3) % MAXORIENT;
      break;
    case 3:
    case 4:
      *side = (corner + 1) % MAXORIENT;
      break;
    default:
      (void) printf ("map_from_3d: face %d\n", face);
  }
}

static void diamond_offset_3d(w, face, dx, dy)
  Skewb3DWidget w;
  int face, *dx, *dy;
{
  if (w->skewb.vertical) {
    switch (face) {
      case 0:
        *dx = w->skewb3d.view_middle.x + w->skewb.delta - 1;
        *dy = w->skewb3d.view_middle.y - w->skewb.delta - 2;
        break;
      case 1:
        *dx = w->skewb3d.view_middle.x - 2 * w->skewb.delta;
        *dy = w->skewb3d.view_middle.y;
        break;
      case 2:
        *dx = w->skewb3d.view_middle.x + w->skewb.delta - 1;
        *dy = w->skewb3d.view_middle.y + 2 * w->skewb.delta - 1;
        break;
      case 3:
        *dx = w->skewb3d.view_middle.x + 2 * w->skewb.delta;
        *dy = w->skewb3d.view_height + w->skewb3d.view_middle.y +
              w->skewb.delta - 1;
        break;
      case 4:
        *dx = w->skewb3d.view_middle.x - w->skewb.delta + 1;
        *dy = w->skewb3d.view_height + w->skewb3d.view_middle.y -
              w->skewb.delta + 1;
        break;
      case 5:
        *dx = w->skewb3d.view_middle.x - 2;
        *dy = w->skewb3d.view_height + w->skewb3d.view_middle.y +
              2 * w ->skewb.delta + 2;
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
        *dx = w->skewb3d.view_middle.x;
        *dy = w->skewb3d.view_middle.y - 2 * w->skewb.delta + 1;
        break;
      case 1:
        *dx = w->skewb3d.view_middle.x - w->skewb.delta - 2;
        *dy = w->skewb3d.view_middle.y + w->skewb.delta;
        break;
      case 2:
        *dx = w->skewb3d.view_middle.x  + 2 * w->skewb.delta - 1;
        *dy = w->skewb3d.view_middle.y + w->skewb.delta;
        break;
      case 3:
        *dx = w->skewb3d.view_width + w->skewb3d.view_middle.x -
              w->skewb.delta + 1;
        *dy = w->skewb3d.view_middle.y - w->skewb.delta;
        break;
      case 4:
        *dx = w->skewb3d.view_width + w->skewb3d.view_middle.x +
              w->skewb.delta - 1;
        *dy = w->skewb3d.view_middle.y  + 2 * w->skewb.delta;
        break;
      case 5:
        *dx = w->skewb3d.view_width + w->skewb3d.view_middle.x +
              2 * w->skewb.delta + 2;
        *dy = w->skewb3d.view_middle.y - w->skewb.delta;
        break;
      default:
        (void) printf ("diamond_offset_3d: face %d\n", face);
    }
  }
}

static void cube_offset_3d(w, face, corner, dx, dy)
  Skewb3DWidget w;
  int face, corner, *dx, *dy;
{
  int side;

  diamond_offset_3d(w, face, dx, dy);
  for (side = 1; side <= corner; side++)
  {
    *dx += cube_loc_3d[face][side].x;
    *dy += cube_loc_3d[face][side].y;
  }
}
