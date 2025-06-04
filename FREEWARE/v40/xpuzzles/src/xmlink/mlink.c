/*
# X-BASED MISSING LINK(tm)
#
#  Mlink.c
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

/* Methods file for Mlink */

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
#include "MlinkP.h"

#ifndef DATAFILE
#define DATAFILE "/usr/games/lib/mlink.data"
#endif

static void InitializeMlink();
static void ExposeMlink();
static void ResizeMlink();
static void DestroyMlink();
static Boolean SetValuesMlink();
static void quit_mlink();
static void move_mlink_top();
static void move_mlink_left();
static void move_mlink_right();
static void move_mlink_bottom();
static void move_mlink_input();
static void select_mlink();
static void randomize_mlink();
static void maybe_randomize_mlink();
static void enter_mlink();
static void write_mlink();
static void undo_mlink();
static void solve_mlink();
static void orientize_mlink();
static void middle_mlink();
static void control_mlink();
static void rotate_mlink();
static void slide_mlink();
static void select_tiles();
static void get_color();
static void move_shift_cb();
static void check_tiles();
static void reset_tiles();
static void resize_tiles();
static void no_move_tiles();
static int move_tiles_dir();
static void randomize_tiles();
static void move_tiles();
static void rotate_tiles();
static int exchange_tiles();
static void draw_frame();
static void draw_tile();
static void draw_link();
static int position_tile();
static int row();
static int column();

static char defaultTranslationsMlink[] =
  "<KeyPress>q: quit()\n\
   Ctrl<KeyPress>C: quit()\n\
   <KeyPress>Up: move_top()\n\
   <KeyPress>KP_8: move_top()\n\
   <KeyPress>R8: move_top()\n\
   <KeyPress>Left: move_left()\n\
   <KeyPress>KP_4: move_left()\n\
   <KeyPress>R10: move_left()\n\
   <KeyPress>Right: move_right()\n\
   <KeyPress>KP_6: move_right()\n\
   <KeyPress>R12: move_right()\n\
   <KeyPress>Down: move_bottom()\n\
   <KeyPress>KP_2: move_bottom()\n\
   <KeyPress>R14: move_bottom()\n\
   <Btn1Down>: select()\n\
   <Btn1Motion>: select()\n\
   <Btn2Down>: select()\n\
   <Btn2Motion>: select()\n\
   <KeyPress>r: randomize()\n\
   <Btn3Down>(2+): randomize()\n\
   <Btn3Down>: maybe_randomize()\n\
   <KeyPress>e: enter()\n\
   <KeyPress>w: write()\n\
   <KeyPress>u: undo()\n\
   <KeyPress>s: solve()\n\
   <KeyPress>o: orientize()\n\
   <KeyPress>m: middle()";
 
static XtActionsRec actionsListMlink[] =
{
  {"quit", (XtActionProc) quit_mlink},
  {"move_top", (XtActionProc) move_mlink_top},
  {"move_left", (XtActionProc) move_mlink_left},
  {"move_right", (XtActionProc) move_mlink_right},
  {"move_bottom", (XtActionProc) move_mlink_bottom},
  {"select", (XtActionProc) select_mlink},
  {"randomize", (XtActionProc) randomize_mlink},
  {"maybe_randomize", (XtActionProc) maybe_randomize_mlink},
  {"enter", (XtActionProc) enter_mlink},
  {"write", (XtActionProc) write_mlink},
  {"undo", (XtActionProc) undo_mlink},
  {"solve", (XtActionProc) solve_mlink},
  {"orientize", (XtActionProc) orientize_mlink},
  {"middle", (XtActionProc) middle_mlink}
};

static XtResource resourcesMlink[] =
{
  {XtNfaceColor0, XtCLabel, XtRString, sizeof(String),
   XtOffset(MlinkWidget, mlink.face_name[0]), XtRString, "White"},
  {XtNfaceColor1, XtCLabel, XtRString, sizeof(String),
   XtOffset(MlinkWidget, mlink.face_name[1]), XtRString, "Yellow"},
  {XtNfaceColor2, XtCLabel, XtRString, sizeof(String),
   XtOffset(MlinkWidget, mlink.face_name[2]), XtRString, "Green"},
  {XtNfaceColor3, XtCLabel, XtRString, sizeof(String),
   XtOffset(MlinkWidget, mlink.face_name[3]), XtRString, "Red"},
  {XtNfaceColor4, XtCLabel, XtRString, sizeof(String),
   XtOffset(MlinkWidget, mlink.face_name[4]), XtRString, "Blue"},
  {XtNfaceColor5, XtCLabel, XtRString, sizeof(String),
   XtOffset(MlinkWidget, mlink.face_name[5]), XtRString, "Magenta"},
  {XtNfaceColor6, XtCLabel, XtRString, sizeof(String),
   XtOffset(MlinkWidget, mlink.face_name[6]), XtRString, "Cyan"},
  {XtNfaceColor7, XtCLabel, XtRString, sizeof(String),
   XtOffset(MlinkWidget, mlink.face_name[7]), XtRString, "Orange"},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
   XtOffset(MlinkWidget, mlink.foreground), XtRString, XtDefaultBackground},
  {XtNtileColor, XtCColor, XtRPixel, sizeof(Pixel),
   XtOffset(MlinkWidget, mlink.tile_color), XtRString, XtDefaultForeground},
  {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
   XtOffset(MlinkWidget, core.width), XtRString, "200"},
  {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
   XtOffset(MlinkWidget, core.height), XtRString, "200"},
  {XtNtiles, XtCTiles, XtRInt, sizeof(int),
   XtOffset(MlinkWidget, mlink.tiles), XtRString, "4"}, /* DEFAULTTILES */
  {XtNfaces, XtCFaces, XtRInt, sizeof(int),
   XtOffset(MlinkWidget, mlink.faces), XtRString, "4"}, /* DEFAULTFACES */
  {XtNorient, XtCOrient, XtRBoolean, sizeof(Boolean),
   XtOffset(MlinkWidget, mlink.orient), XtRString, "FALSE"}, /*DEFAULTORIENT*/
  {XtNmiddle, XtCMiddle, XtRBoolean, sizeof(Boolean),
   XtOffset(MlinkWidget, mlink.middle), XtRString,
   "TRUE"}, /*DEFAULTMIDDLE*/
  {XtNmono, XtCMono, XtRBoolean, sizeof(Boolean),
   XtOffset(MlinkWidget, mlink.mono), XtRString, "FALSE"},
  {XtNbase, XtCBase, XtRInt, sizeof(int),
   XtOffset(MlinkWidget, mlink.base), XtRString, "10"},
  {XtNstart, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(MlinkWidget, mlink.started), XtRString, "FALSE"},
  {XtNselectCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
   XtOffset(MlinkWidget, mlink.select), XtRCallback, NULL}
};

MlinkClassRec mlinkClassRec =
{
  {
    (WidgetClass) &widgetClassRec,	/* superclass */
    "Mlink",				/* class name */
    sizeof(MlinkRec),			/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeMlink,			/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    actionsListMlink,			/* actions */
    XtNumber(actionsListMlink),		/* num actions */
    resourcesMlink,			/* resources */
    XtNumber(resourcesMlink),		/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    DestroyMlink,			/* destroy */
    ResizeMlink,			/* resize */
    ExposeMlink,			/* expose */
    SetValuesMlink,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    NULL,				/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    defaultTranslationsMlink,		/* tm table */
    NULL,				/* query geometry */
    NULL,				/* display accelerator */
    NULL				/* extension */
  },
  {
    0					/* ignore */
  }
};

WidgetClass mlinkWidgetClass = (WidgetClass) &mlinkClassRec;

static void InitializeMlink(request, new)
  Widget request, new;
{
  MlinkWidget w = (MlinkWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  int face;
  
  check_tiles(w);
  init_moves();
  reset_tiles(w);
  (void) SRAND(getpid());
  valueMask = GCForeground | GCBackground;
  values.foreground = w->mlink.foreground;
  values.background = w->core.background_pixel;
  w->mlink.puzzle_GC = XtGetGC(new, valueMask, &values);
  w->mlink.depth = DefaultDepthOfScreen(XtScreen(w));
  values.foreground = w->mlink.tile_color;
  w->mlink.tile_GC = XtGetGC(new, valueMask, &values);
  valueMask = GCForeground | GCBackground;
  values.foreground = w->core.background_pixel;
  values.background = w->mlink.foreground;
  w->mlink.inverse_GC = XtGetGC(new, valueMask, &values);
  for (face = 0; face < MAXFACES; face++)
    get_color(w, face, TRUE);
  ResizeMlink(w);
}

static void DestroyMlink(old)
  Widget old;
{
  MlinkWidget w = (MlinkWidget) old;
  int face;

  for (face = 0; face < MAXFACES; face++)
    XtReleaseGC(old, w->mlink.face_GC[face]);
  XtReleaseGC(old, w->mlink.tile_GC);
  XtReleaseGC(old, w->mlink.puzzle_GC);
  XtReleaseGC(old, w->mlink.inverse_GC);
  XtRemoveCallbacks(old, XtNselectCallback, w->mlink.select);
}

static void ResizeMlink(w)
  MlinkWidget w;
{
  w->mlink.delta.x = 3;
  w->mlink.delta.y = 3;
  w->mlink.wid = MAX(((int) w->core.width - w->mlink.delta.x) /
    w->mlink.tiles, 0);
  w->mlink.ht = MAX(((int) w->core.height - w->mlink.delta.y) /
    w->mlink.faces, 0);
  w->mlink.face_width = w->mlink.wid * w->mlink.tiles +
    w->mlink.delta.x;
  w->mlink.face_height = w->mlink.ht + w->mlink.delta.y;
  w->mlink.puzzle_width = w->mlink.face_width + w->mlink.delta.x;
  w->mlink.puzzle_height = (w->mlink.face_height - w->mlink.delta.y) *
    w->mlink.faces + w->mlink.delta.y;
  w->mlink.puzzle_offset.x = ((int) w->core.width -
    w->mlink.puzzle_width + 2) / 2;
  w->mlink.puzzle_offset.y = ((int) w->core.height -
    w->mlink.puzzle_height + 2) / 2;
  w->mlink.tile_width = MAX(w->mlink.wid - w->mlink.delta.x, 0);
  w->mlink.tile_height = MAX(w->mlink.ht - w->mlink.delta.y , 0);
  resize_tiles(w);
}

static void ExposeMlink(new, event, region)
  Widget new;
  XEvent *event;
  Region region; /* Not used */
{
  MlinkWidget w = (MlinkWidget) new;

  if (w->core.visible) {
    draw_frame(w, w->mlink.puzzle_GC);
    draw_all_tiles(w, w->mlink.tile_GC);
  }
}

static Boolean SetValuesMlink(current, request, new)
  Widget current, request, new;
{
  MlinkWidget c = (MlinkWidget) current, w = (MlinkWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  Boolean redraw = FALSE;
  Boolean redraw_tiles = FALSE;
  int face;

  check_tiles(w);
  if (w->mlink.foreground != c->mlink.foreground) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->mlink.foreground;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->mlink.puzzle_GC);
    w->mlink.puzzle_GC = XtGetGC(new, valueMask, &values);
    if (w->mlink.mono || w->mlink.depth == 1) {
      values.foreground = w->core.background_pixel;
      values.background = w->mlink.tile_color;
      for (face = 0; face < MAXFACES; face++) {
        XtReleaseGC((Widget) w, w->mlink.face_GC[face]);
        w->mlink.face_GC[face] = XtGetGC((Widget) w, valueMask, &values);
      }
      c->mlink.mono = w->mlink.mono;
    }
    redraw_tiles = TRUE;
  }
  if (w->core.background_pixel != c->core.background_pixel) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->core.background_pixel;
    values.background = w->mlink.foreground;
    XtReleaseGC(new, w->mlink.inverse_GC);
    w->mlink.inverse_GC = XtGetGC(new, valueMask, &values);
    redraw_tiles = TRUE;
  }
  if (w->mlink.tile_color != c->mlink.tile_color) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->mlink.tile_color;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->mlink.tile_GC);
    w->mlink.tile_GC = XtGetGC(new, valueMask, &values);
    redraw_tiles = TRUE;
  }
  for (face = 0; face < MAXFACES; face++) {
    if (strcmp(w->mlink.face_name[face], c->mlink.face_name[face]))
      get_color(w, face, FALSE);
  }
  if (w->mlink.tiles != c->mlink.tiles ||
      w->mlink.faces != c->mlink.faces ||
      w->mlink.orient != c->mlink.orient ||
      w->mlink.middle != c->mlink.middle ||
      w->mlink.base != c->mlink.base) {
    reset_tiles(w);
    ResizeMlink(w);
    redraw = TRUE;
  }
  else if (w->mlink.wid != c->mlink.wid ||
           w->mlink.ht != c->mlink.ht) {
    ResizeMlink(w);
    redraw = TRUE;
  }
  if (redraw_tiles && !redraw && XtIsRealized(new) && new->core.visible) {
    draw_frame(w, c->mlink.inverse_GC);
    draw_all_tiles(c, c->mlink.inverse_GC);
    draw_frame(w, w->mlink.puzzle_GC);
    draw_all_tiles(w, c->mlink.tile_GC);
  }
  return (redraw);
}

static void quit_mlink(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  XtCloseDisplay(XtDisplay(w));
  exit(0);
}

static void select_mlink(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  int i, j, dir;

  i = (event->xbutton.x - w->mlink.delta.x / 2 -
    w->mlink.puzzle_offset.x) / w->mlink.wid;
  j = ((event->xbutton.y - w->mlink.delta.y / 2 -
    w->mlink.puzzle_offset.y) % (w->mlink.faces * w->mlink.ht +
    w->mlink.delta.y - 1)) / w->mlink.ht;
  dir = (event->xbutton.button == Button1); 
  if (check_solved(w))
    no_move_tiles(w);
  else {
    select_tiles(w, i, j, dir);
    if (check_solved(w)) {
      mlinkCallbackStruct cb;

      cb.reason = MLINK_SOLVED;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}

static void randomize_mlink(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  randomize_tiles(w);
}
 
static void maybe_randomize_mlink(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->mlink.started)
    randomize_tiles(w);
}

static void enter_mlink(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;
  char c;
  int i, tiles, faces, middle, orient, moves;
  mlinkCallbackStruct cb;

  if ((fp = fopen(DATAFILE, "r")) == NULL)
    (void) printf("Can not read %s to enter.\n", DATAFILE);
  else {
    flush_moves(w);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &tiles);
    if (tiles >= MINTILES && tiles <= MAXTILES) {
      for (i = w->mlink.tiles; i < tiles; i++) {
        cb.reason = MLINK_INC_X;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
      for (i = w->mlink.tiles; i > tiles; i--) {
        cb.reason = MLINK_DEC_X;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    } else
      (void) printf("%s corrupted: tiles %d should be between %d and %d\n",
         DATAFILE, tiles, MINTILES, MAXTILES);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &faces);
    if (faces >= MINFACES && faces <= MAXFACES) {
      for (i = w->mlink.faces; i < faces; i++) {
        cb.reason = MLINK_INC_Y;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
      for (i = w->mlink.faces; i > faces; i--) {
        cb.reason = MLINK_DEC_Y;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    } else
      (void) printf("%s corrupted: faces %d should be between %d and %d\n",
         DATAFILE, faces, MINFACES, MAXFACES);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &middle);
    if (w->mlink.middle != (Boolean) middle) {
      cb.reason = MLINK_MIDDLE;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &orient);
    if (w->mlink.orient != (Boolean) orient) {
      cb.reason = MLINK_ORIENT;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &moves);
    scan_start_position(fp, w);
    cb.reason = MLINK_RESTORE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    set_start_position(w);
    scan_moves(fp, w, moves);
    (void) fclose(fp);
    (void) printf("%s: tiles %d, faces %d, middle %d, orient %d, moves %d.\n",
      DATAFILE, tiles, faces, middle, orient, moves);
  }
}

static void write_mlink(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;

  if ((fp = fopen(DATAFILE, "w")) == NULL)
    (void) printf("Can not write to %s.\n", DATAFILE);
  else {
    (void) fprintf(fp, "tiles: %d\n", w->mlink.tiles);
    (void) fprintf(fp, "faces: %d\n", w->mlink.faces);
    (void) fprintf(fp, "middle: %d\n", (w->mlink.middle) ? 1 : 0);
    (void) fprintf(fp, "orient: %d\n", (w->mlink.orient) ? 1 : 0);
    (void) fprintf(fp, "moves: %d\n", num_moves());
    print_start_position(fp, w);
    print_moves(fp);
    (void) fclose(fp);
    (void) printf("Saved to %s.\n", DATAFILE);
  }
}

static void undo_mlink(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (moves_made()) {
    int direction, tile, shift;
    mlinkCallbackStruct cb;

    get_move(&direction, &tile, &shift);
    direction = (direction + COORD / 2) % COORD;
    if (shift && (direction == TOP || direction == BOTTOM))
      move_shift_cb(w, direction);
    else if (move_tiles_dir(w, direction, tile)) {
      cb.reason = MLINK_UNDO;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}
 
static void solve_mlink(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  /* solve_tiles(w); */ /* Sorry, unimplemented */
}
 
static void orientize_mlink(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  mlinkCallbackStruct cb;
 
  cb.reason = MLINK_ORIENT;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
static void middle_mlink(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  mlinkCallbackStruct cb;
 
  cb.reason = MLINK_MIDDLE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
static void move_mlink_top(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mlink_input(w, event->xbutton.x, TOP,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_mlink_left(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mlink_input(w, event->xbutton.x, LEFT,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_mlink_right(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mlink_input(w, event->xbutton.x, RIGHT,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_mlink_bottom(w, event, args, n_args)
  MlinkWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mlink_input(w, event->xbutton.x, BOTTOM,
    (int) (event->xkey.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_mlink_input(w, x, direction, shift, control)
  MlinkWidget w;
  int x, direction, shift, control;
{
  mlinkCallbackStruct cb;
  int r;

  if (control)
    control_mlink(w, direction);
  else if (shift && (direction == TOP || direction == BOTTOM)) {
    move_shift_cb(w, direction);
    put_move(direction, 0, 1);
  } else if (check_solved(w)) {
    no_move_tiles(w);
    return;
  } else if (direction == LEFT || direction == RIGHT) {
    slide_mlink(w, direction);
    if (check_solved(w)) {
      cb.reason = MLINK_SOLVED;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  } else {
    if (!position_tile(w, x, &r))
      return;
    rotate_mlink(w, direction, r);
    if (check_solved(w)) {
      cb.reason = MLINK_SOLVED;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}

int move_mlink(w, direction, tile, shift)
  MlinkWidget w;
  int direction, tile, shift;
{
  mlinkCallbackStruct cb;

  if (shift && (direction == TOP || direction == BOTTOM)) {
    move_shift_cb(w, direction);
    return TRUE;
  } else if (check_solved(w)) {
    no_move_tiles(w);
    return FALSE;
  } else if (move_tiles_dir(w, direction, tile)) {
    cb.reason = MLINK_MOVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    put_move(direction, tile, 0);
    if (check_solved(w)) {
      cb.reason = MLINK_SOLVED;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    return TRUE;
  }
  return FALSE;
}

static void control_mlink(w, direction)
  MlinkWidget w;
  int direction;
{
  mlinkCallbackStruct cb;

  cb.reason = MLINK_IGNORE;
  switch (direction) {
    case TOP:
      if (w->mlink.faces != MINFACES)
        cb.reason = MLINK_DEC_Y;
      else
        return;
      break;
    case RIGHT:
      if (w->mlink.tiles != MAXTILES)
        cb.reason = MLINK_INC_X;
      else
        return;
      break;
    case BOTTOM:
      if (w->mlink.faces != MAXFACES)
        cb.reason = MLINK_INC_Y;
      else
        return;
      break;
    case LEFT:
      if (w->mlink.tiles != MINTILES)
        cb.reason = MLINK_DEC_X;
      else
        return;
      break;
    default:
      (void) printf("control_mlink: direction %d\n", direction);
  }
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void rotate_mlink(w, direction, tile)
  MlinkWidget w;
  int tile, direction;
{
  mlinkCallbackStruct cb;

  if (check_solved(w)) {
    no_move_tiles(w);
    return;
  }
  (void) move_mlink(w, direction, tile, FALSE);
  if (check_solved(w)) {
    cb.reason = MLINK_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  return;
}

static void slide_mlink(w, direction)
  MlinkWidget w;
  int direction;
{
  mlinkCallbackStruct cb;

  if (check_solved(w)) {
    no_move_tiles(w);
    return;
  }
  if (!move_mlink(w, direction, 0, FALSE)) {
    cb.reason = MLINK_BLOCKED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    return;
  }
  if (check_solved(w)) {
    cb.reason = MLINK_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  return;
}

static void select_tiles(w, i, j, dir)
  MlinkWidget w;
  int i, j, dir;
{
  mlinkCallbackStruct cb;
  int l, n;

  if (i >= 0 && j >= 0 &&
      i < w->mlink.tiles && j < w->mlink.faces) {
    l = i + w->mlink.tiles * j - w->mlink.space_position %
      w->mlink.tile_faces;
    if (l / w->mlink.tiles == 0 &&
        j == column(w, w->mlink.space_position)) {
      if (l < 0) {
        for (n = 1; n <= -l % w->mlink.tiles; n++) {
          move_tiles(w, w->mlink.space_position - 1);
          cb.reason = MLINK_MOVED;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          put_move(RIGHT, 0, 0);
        }
      } else if (l > 0) {
        for (n = 1; n <= l % w->mlink.tiles; n++) {
          move_tiles(w, w->mlink.space_position + 1);
          cb.reason = MLINK_MOVED;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          put_move(LEFT, 0, 0);
        }
      } else /* (l == 0) */ {
        /*cb.reason = MLINK_SPACE;*/
        (void) move_mlink(w, dir * BOTTOM, i, FALSE);
        return;
      }
    } else {
      (void) move_mlink(w, dir * BOTTOM, i, FALSE);
      return;
    }
  }
}

static void get_color(w, face, init)
  MlinkWidget w;
  int face, init;
{
  XGCValues values;
  XtGCMask valueMask;
  XColor color_cell, rgb;

  valueMask = GCForeground | GCBackground;
  values.background = w->core.background_pixel;
  if (w->mlink.depth > 1 && !w->mlink.mono) {
    if (XAllocNamedColor(XtDisplay(w),
        DefaultColormap(XtDisplay(w), XtWindow(w)),
        w->mlink.face_name[face], &color_cell, &rgb)) {
      values.foreground = w->mlink.face_color[face] = color_cell.pixel;
      if (!init)
        XtReleaseGC((Widget) w, w->mlink.face_GC[face]);
      w->mlink.face_GC[face] = XtGetGC((Widget) w, valueMask, &values);
      return;
    } else {
      char buf[121];

      (void) sprintf(buf, "Color name \"%s\" is not defined",
               w->mlink.face_name[face]);
      XtWarning(buf);
    }
  }
  values.foreground = w->mlink.foreground;
  if (!init)
    XtReleaseGC((Widget) w, w->mlink.face_GC[face]);
  w->mlink.face_GC[face] = XtGetGC((Widget) w, valueMask, &values);
}

static void move_shift_cb(w, direction)
  MlinkWidget w;
  int direction;
{
  mlinkCallbackStruct cb;

  if (w->mlink.middle) {
    (void) move_tiles_dir(w, direction, 0);
    (void) move_tiles_dir(w, direction, 1);
    (void) move_tiles_dir(w, direction, w->mlink.tiles - 1);
  } else {
    int r;

    for (r = 0; r < w->mlink.tiles; r++)
      (void) move_tiles_dir(w, direction, r);
  }
  cb.reason = MLINK_CONTROL;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void check_tiles(w)
  MlinkWidget w;
{
  char buf[121];

  if (w->mlink.tiles < MINTILES || w->mlink.tiles > MAXTILES) {
    (void) sprintf(buf,
             "Number of Mlink in X direction out of bounds, use %d..%d",
             MINTILES, MAXTILES);
    XtWarning(buf);
    w->mlink.tiles = DEFAULTTILES;
  }
  if (w->mlink.faces < MINFACES || w->mlink.faces > MAXFACES) {
    (void) sprintf(buf,
             "Number of Mlink in Y direction out of bounds, use %d..%d",
             MINFACES, MAXFACES);
    XtWarning(buf);
    w->mlink.faces = DEFAULTFACES;
  }
}

static void reset_tiles(w)
  MlinkWidget w;
{
  int i;

  w->mlink.tile_faces = w->mlink.tiles * w->mlink.faces; 
  w->mlink.space_position = w->mlink.tile_faces - 1;
  w->mlink.tile_of_position[w->mlink.tile_faces - 1] = 0;
  for (i = 1; i < w->mlink.tile_faces; i++)
    w->mlink.tile_of_position[i - 1] = i;
  flush_moves(w);
  w->mlink.started = FALSE;
}

static void resize_tiles(w)
  MlinkWidget w;
{
  w->mlink.digit_offset.x = 3;
  w->mlink.digit_offset.y = 4;
}

static void no_move_tiles(w)
  MlinkWidget w;
{
  mlinkCallbackStruct cb;
 
  cb.reason = MLINK_IGNORE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static int move_tiles_dir(w, direction, tile)
  MlinkWidget w;
  int direction, tile;
{
  switch (direction) {
    case TOP:
      rotate_tiles(w, tile, TOP);
      return TRUE;
    case RIGHT:
      if (row(w, w->mlink.space_position) > 0) {
        move_tiles(w, w->mlink.space_position - 1);
        return TRUE;
      }
      break;
    case BOTTOM:
      rotate_tiles(w, tile, BOTTOM);
      return TRUE;
    case LEFT:
      if (row(w, w->mlink.space_position) < w->mlink.tiles - 1) {
        move_tiles(w, w->mlink.space_position + 1);
        return TRUE;
      }
      break;
    default:
      (void) printf("move_tiles_dir: direction %d\n", direction);
  }
  return FALSE;
}
 
static void randomize_tiles(w)
  MlinkWidget w;
{
  mlinkCallbackStruct cb;

  /* First interchange tiles an even number of times */
  if (w->mlink.tiles > 1 && w->mlink.faces > 1 && w->mlink.tile_faces > 4) {
    int pos, count = 0;

    for (pos = 0; pos < w->mlink.tile_faces; pos++) {
       int random_pos = pos;

       while (random_pos == pos)
         random_pos = NRAND(w->mlink.tile_faces);
       count += exchange_tiles(w, pos, random_pos);
    }
    if (count % 2)
      if (!exchange_tiles(w, 0, 1))
        if (!exchange_tiles(w,
              w->mlink.tile_faces - 2, w->mlink.tile_faces - 1))
          (void) printf("randomize_tiles: should not get here\n");
    draw_all_tiles(w, w->mlink.tile_GC);
  }
  /* Now move the space around randomly */
  if (w->mlink.tiles > 1 || w->mlink.faces > 1) {
    int big = w->mlink.tile_faces + NRAND(2);
    int last_direction = 0;
    int random_direction;

    cb.reason = MLINK_RESET;
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
          (w->mlink.tiles == 1 && w->mlink.faces == 1)) {
        if (move_mlink(w, random_direction, NRAND(w->mlink.tiles), FALSE))
          last_direction = random_direction;
        else
          big++;
      }
    }
    flush_moves(w);
    cb.reason = MLINK_RANDOMIZE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  if (check_solved(w)) {
    cb.reason = MLINK_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void move_tiles(w, from)
  MlinkWidget w;
  int from;
{
  int temp_tile;

  temp_tile = w->mlink.tile_of_position[from];
  w->mlink.tile_of_position[from] =
    w->mlink.tile_of_position[w->mlink.space_position];
  w->mlink.tile_of_position[w->mlink.space_position] = temp_tile;
  draw_tile(w, w->mlink.tile_GC, w->mlink.space_position);
  w->mlink.space_position = from;
  draw_tile(w, w->mlink.inverse_GC, w->mlink.space_position);
}

static void rotate_tiles(w, c, direction)
  MlinkWidget w;
  int c, direction;
{
  int new_r, k, temp_tile, curr_tile, pos, new_pos;
  int r = 0, space = -1, loop = FALSE;

  if (w->mlink.middle && c > 0 && c < w->mlink.tiles - 1) {
    loop = TRUE;
    c = 1;
  }
  do {
    curr_tile = w->mlink.tile_of_position[r * w->mlink.tiles + c];
    for (k = 1; k <= w->mlink.faces; k++) {
      new_r = (direction == TOP) ? (r + w->mlink.faces - 1) % w->mlink.faces:
        (r + 1) % w->mlink.faces;
      pos = r * w->mlink.tiles + c;
      new_pos = new_r * w->mlink.tiles + c;
      temp_tile = w->mlink.tile_of_position[new_pos];
      w->mlink.tile_of_position[new_pos] = curr_tile;
      curr_tile = temp_tile;
      if (w->mlink.space_position == pos) {
        draw_tile(w, w->mlink.inverse_GC, new_pos);
        space = new_pos;
      } else
        draw_tile(w, w->mlink.tile_GC, new_pos);
      r = new_r;
    }
    if (space >= 0)
      w->mlink.space_position = space;
  } while (loop && ++c < w->mlink.tiles - 1);
}

static int exchange_tiles(w, pos1, pos2)
  MlinkWidget w;
  int pos1, pos2;
{
  int temp_tile;

  if (w->mlink.tile_of_position[pos1] <= 0)
    return FALSE;
  else if (w->mlink.tile_of_position[pos2] <= 0)
    return FALSE;
  temp_tile = w->mlink.tile_of_position[pos1];
  w->mlink.tile_of_position[pos1] = w->mlink.tile_of_position[pos2];
  w->mlink.tile_of_position[pos2] = temp_tile;
  return TRUE;
}

static void draw_frame(w, gc)
  MlinkWidget w;
  GC gc;
{
  int sum_x, sum_y, offset_x, offset_y, r;

  sum_x = w->mlink.tiles * w->mlink.wid + w->mlink.delta.x / 2 + 1;
  sum_y = w->mlink.ht;
  offset_x = w->mlink.puzzle_offset.x;
  offset_y = w->mlink.puzzle_offset.y;
  for (r = 0; r < w->mlink.faces; r++) {
    XFillRectangle(XtDisplay(w), XtWindow(w), gc, 
      offset_x, offset_y, 1, sum_y);
    XFillRectangle(XtDisplay(w), XtWindow(w), gc,
      offset_x, offset_y, sum_x, 1);
    XFillRectangle(XtDisplay(w), XtWindow(w), gc,
      sum_x + offset_x, offset_y, 1, sum_y + 1);
    XFillRectangle(XtDisplay(w), XtWindow(w), gc,
      offset_x, sum_y + offset_y, sum_x + 1, 1);
    offset_y += sum_y;
  }
}   

void draw_all_tiles(w, gc)
  MlinkWidget w;
  GC gc;
{
  int k;

  for (k = 0; k < w->mlink.tile_faces; k++)
    draw_tile(w,
      (w->mlink.tile_of_position[k] > 0) ? gc : w->mlink.inverse_GC, k);
}

static void draw_tile(w, gc, pos)
  MlinkWidget w;
  GC gc;
  int pos;
{
  int dx, dy, tile = w->mlink.tile_of_position[pos];

  dx = row(w, pos) * w->mlink.wid + w->mlink.delta.x +
    w->mlink.puzzle_offset.x;
  dy = column(w, pos) * w->mlink.ht + w->mlink.delta.y +
    w->mlink.puzzle_offset.y;
  XFillRectangle(XtDisplay(w), XtWindow(w), gc, dx, dy,
    w->mlink.tile_width, w->mlink.tile_height - 2);
  if (gc != w->mlink.inverse_GC) {
    int i = 0, offset = 0, t = tile, mono;
    char buf[6];

    mono = (w->mlink.depth == 1 || w->mlink.mono);
    if (w->mlink.orient || mono) {
      if (mono) {
        if (w->mlink.orient) {
          (void) sprintf(buf, "%d%c", tile,
                   w->mlink.face_name[((tile - 1) / w->mlink.tiles + 1) %
                     w->mlink.faces][0]);
          t *= w->mlink.base;
        } else {
          (void) sprintf(buf, "%c", w->mlink.face_name[((tile - 1) /
                   w->mlink.tiles + 1) % w->mlink.faces][0]);
          t = 1;
        }
      } else
        (void) sprintf(buf, "%d", tile);
      while (t >= 1) {
        t /= w->mlink.base;
        offset += w->mlink.digit_offset.x;
        i++;
      }
      XDrawString(XtDisplay(w), XtWindow(w),
        w->mlink.face_GC[((tile - 1) / w->mlink.tiles + 1) % w->mlink.faces],
        dx + w->mlink.tile_width / 2 - offset,
        dy + w->mlink.tile_height / 2 + w->mlink.digit_offset.y, buf, i);
    }
  }
  if (tile != 0 && 
      ((tile != w->mlink.tile_faces - 1 && w->mlink.tiles > 1) ||
       w->mlink.tiles > 2))
    draw_link(w,
      w->mlink.face_GC[((tile - 1) / w->mlink.tiles + 1) % w->mlink.faces],
      pos);
}

#define MULT 64
#define THICKNESS 8 
static void draw_link(w, gc, pos)
  MlinkWidget w;
  GC gc;
  int pos;
{
  int dx, dy, sizex, sizey, i, tile = w->mlink.tile_of_position[pos];

  for (i = 0; i < THICKNESS; i++) {
    sizex = w->mlink.wid * 3 / 2 - i;
    sizey = w->mlink.ht * 3 / 4 - i;
    dx = row(w, pos) * w->mlink.wid + w->mlink.delta.x +
      w->mlink.puzzle_offset.x - sizex / 2;
    dy = column(w, pos) * w->mlink.ht + w->mlink.delta.y +
      w->mlink.puzzle_offset.y + w->mlink.tile_height / 2 - sizey / 2;
    if (tile % w->mlink.tiles == 0 || tile == w->mlink.tile_faces - 1)
      XDrawArc(XtDisplay(w), XtWindow(w), gc, dx, dy,
        sizex, sizey, 89 * MULT, -179 * MULT);
    else if (tile % w->mlink.tiles == 1)
      XDrawArc(XtDisplay(w), XtWindow(w), gc, dx + w->mlink.tile_width - 1, dy,
        sizex, sizey, 90 * MULT, 180 * MULT);
    else {
      XDrawArc(XtDisplay(w), XtWindow(w), gc, dx, dy,
        sizex, sizey, 89 * MULT, -32 * MULT);
      XDrawArc(XtDisplay(w), XtWindow(w), gc, dx, dy,
        sizex, sizey, -90 * MULT, 128 * MULT);
      dx += w->mlink.tile_width - 1;
      XDrawArc(XtDisplay(w), XtWindow(w), gc, dx, dy,
        sizex, sizey, 90 * MULT, 128 * MULT);
      XDrawArc(XtDisplay(w), XtWindow(w), gc, dx, dy,
        sizex, sizey, -90 * MULT, -33 * MULT);
    }
  }
}

static int position_tile(w, x, r)
  MlinkWidget w;
  int x;
  int *r;
{
  *r = (x - w->mlink.delta.x / 2 - w->mlink.puzzle_offset.x) / w->mlink.wid;
  if (*r < 0 || *r >= w->mlink.tiles)
    return FALSE;
  return TRUE;
}

static int row(w, pos)
  MlinkWidget w;
  int pos;
{
  return (pos % w->mlink.tiles);
}

static int column(w, pos)
  MlinkWidget w;
  int pos;
{
  return (pos / w->mlink.tiles);
}

Boolean check_solved(w)
  MlinkWidget w;
{
  int i, j, first_tile, last_tile, mid_tile;
  if (w->mlink.tiles < 2)
    return TRUE;
  if (w->mlink.orient) {
    first_tile = w->mlink.tile_of_position[0];
    if (first_tile != 0 && (first_tile - 1) % w->mlink.tiles == 0) {
      for (i = 1; i < w->mlink.tile_faces; i++) {
        mid_tile = w->mlink.tile_of_position[i];
        if (mid_tile && (mid_tile - first_tile + w->mlink.tile_faces) %
            w->mlink.tile_faces != i)
          return FALSE;
      }
    } else
      return FALSE;
  } else {
    for (i = 0; i < w->mlink.faces; i++) {
      first_tile = w->mlink.tile_of_position[i * w->mlink.tiles];
      if (first_tile == 0)
        first_tile = w->mlink.tile_of_position[i * w->mlink.tiles + 1];
      last_tile = w->mlink.tile_of_position[(i + 1) * w->mlink.tiles - 1];
      if (last_tile == 0)
        last_tile = w->mlink.tile_of_position[(i + 1) * w->mlink.tiles - 2];
      if (first_tile % w->mlink.tiles != 1 ||
          (last_tile % w->mlink.tiles != 0 &&
           last_tile != w->mlink.tile_faces - 1) ||
          (last_tile - 1) / w->mlink.tiles !=
          (first_tile - 1) / w->mlink.tiles)
        return FALSE;
      for (j = 1; j < w->mlink.tiles - 1; j++) {
        mid_tile = w->mlink.tile_of_position[i * w->mlink.tiles + j];
        if ((mid_tile - 1) / w->mlink.tiles !=
            (first_tile - 1) / w->mlink.tiles || mid_tile == 0)
          return FALSE;
      }
    }
  }
  return TRUE;
}
