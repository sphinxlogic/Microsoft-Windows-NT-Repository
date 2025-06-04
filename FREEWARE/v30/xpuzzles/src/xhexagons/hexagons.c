/*
# X-BASED HEXAGONS
#
#  Hexagons.c
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

/* Methods file for Hexagons */

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
#include "HexagonsP.h"

#ifndef DATAFILE
#define DATAFILE "/usr/games/lib/hexagons.data"
#endif

static void InitializeHexagons();
static void ExposeHexagons();
static void ResizeHexagons();
static void DestroyHexagons();
static Boolean SetValuesHexagons();
static void quit_hexagons();
static void move_hexagons_tl();
static void move_hexagons_tr();
static void move_hexagons_left();
static void move_hexagons_right();
static void move_hexagons_bl();
static void move_hexagons_br();
static void select_hexagons();
static void randomize_hexagons();
static void maybe_randomize_hexagons();
static void enter_hexagons();
static void write_hexagons();
static void undo_hexagons();
static void solve_hexagons();
static void increment_hexagons();
static void decrement_hexagons();
static void corners_hexagons();
static int move_hexagons();
static void select_tiles_corners();
static void select_tiles_nocorn();
static void check_tiles();
static void reset_tiles();
static void resize_tiles();
static int movable_tiles_corners();
static void no_move_tiles();
static int move_tiles_dir();
static int move_tiles_corners_dir();
static int move_tiles_nocorn_dir();
static void randomize_tiles();
static void move_tiles_corners();
static void move_tiles_nocorn();
static int exchange_tiles();
#ifdef DEBUG
static int within_frame();
#endif
static int next_to_wall();
static int tile_next_to_space();
static int find_tile_triangle();
static int find_dir();
static int find_space_type();
static void find_movable_tile();
static void draw_frame();
static void draw_tile();
#ifdef DEBUG
static int position_in_row();
#endif
static int position_from_row();
static int Sqrt();
static void swap();

static char defaultTranslationsHexagons[] =
  "<KeyPress>q: quit()\n\
   Ctrl<KeyPress>C: quit()\n\
   <KeyPress>Home: move_tl()\n\
   <KeyPress>KP_7: move_tl()\n\
   <KeyPress>R7: move_tl()\n\
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
   <KeyPress>R13: move_bl()\n\
   <KeyPress>Next: move_br()\n\
   <KeyPress>KP_3: move_br()\n\
   <KeyPress>R15: move_br()\n\
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
   <KeyPress>i: increment()\n\
   <KeyPress>d: decrement()\n\
   <KeyPress>c: corners()";
 
static XtActionsRec actionsListHexagons[] =
{
  {"quit", (XtActionProc) quit_hexagons},
  {"move_tl", (XtActionProc) move_hexagons_tl},
  {"move_tr", (XtActionProc) move_hexagons_tr},
  {"move_left", (XtActionProc) move_hexagons_left},
  {"move_right", (XtActionProc) move_hexagons_right},
  {"move_bl", (XtActionProc) move_hexagons_bl},
  {"move_br", (XtActionProc) move_hexagons_br},
  {"select", (XtActionProc) select_hexagons},
  {"randomize", (XtActionProc) randomize_hexagons},
  {"maybe_randomize", (XtActionProc) maybe_randomize_hexagons},
  {"enter", (XtActionProc) enter_hexagons},
  {"write", (XtActionProc) write_hexagons},
  {"undo", (XtActionProc) undo_hexagons},
  {"solve", (XtActionProc) solve_hexagons},
  {"increment", (XtActionProc) increment_hexagons},
  {"decrement", (XtActionProc) decrement_hexagons},
  {"corners", (XtActionProc) corners_hexagons}
};

static XtResource resourcesHexagons[] =
{
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
   XtOffset(HexagonsWidget, hexagons.foreground), XtRString,
   XtDefaultForeground},
  {XtNtileColor, XtCColor, XtRPixel, sizeof(Pixel),
   XtOffset(HexagonsWidget, hexagons.tile_color), XtRString,
   XtDefaultForeground},
  {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
   XtOffset(HexagonsWidget, core.width), XtRString, "259"},
  {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
   XtOffset(HexagonsWidget, core.height), XtRString, "200"},
  {XtNsize, XtCSize, XtRInt, sizeof(int),
   XtOffset(HexagonsWidget, hexagons.size), XtRString, "3"}, /* DEFAULTHEXS */
  {XtNcorners, XtCCorners, XtRBoolean, sizeof(Boolean),
   XtOffset(HexagonsWidget, hexagons.corners), XtRString, "TRUE"},/*DEFAULTCORN*/
  {XtNbase, XtCBase, XtRInt, sizeof(int),
   XtOffset(HexagonsWidget, hexagons.base), XtRString, "10"},
  {XtNstart, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(HexagonsWidget, hexagons.started), XtRString, "FALSE"},
  {XtNselectCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
   XtOffset(HexagonsWidget, hexagons.select), XtRCallback, NULL}
};

HexagonsClassRec hexagonsClassRec =
{
  {
    (WidgetClass) &widgetClassRec,	/* superclass */
    "Hexagons",				/* class name */
    sizeof(HexagonsRec),		/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeHexagons,			/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    actionsListHexagons,		/* actions */
    XtNumber(actionsListHexagons),	/* num actions */
    resourcesHexagons,			/* resources */
    XtNumber(resourcesHexagons),	/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    DestroyHexagons,			/* destroy */
    ResizeHexagons,			/* resize */
    ExposeHexagons,			/* expose */
    SetValuesHexagons,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    NULL,				/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    defaultTranslationsHexagons,	/* tm table */
    NULL,				/* query geometry */
    NULL,				/* display accelerator */
    NULL				/* extension */
  },
  {
    0					/* ignore */
  }
};

WidgetClass hexagonsWidgetClass = (WidgetClass) &hexagonsClassRec;

static XPoint hexagon_unit[MAXORIENT][6] =
{
  { {0, 0}, {2, 0}, {1, 1}, {-1, 1}, {-2, 0}, {-1, -1} },
  { {0, 0}, {1, 1}, {0, 2}, {-1, 1}, {-1, -1}, {0, -2} }
};
static XPoint hexagon_list[MAXORIENT][6];

static void InitializeHexagons(request, new)
  Widget request, new;
{
  HexagonsWidget w = (HexagonsWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  
  check_tiles(w);
  init_moves();
  reset_tiles(w);
  (void) SRAND(getpid());
  valueMask = GCForeground | GCBackground;
  values.foreground = w->hexagons.foreground;
  values.background = w->core.background_pixel;
  w->hexagons.puzzle_GC = XtGetGC(new, valueMask, &values);
  values.foreground = w->hexagons.tile_color;
  w->hexagons.tile_GC = XtGetGC(new, valueMask, &values);
  valueMask = GCForeground | GCBackground;
  values.foreground = w->core.background_pixel;
  values.background = w->hexagons.foreground;
  w->hexagons.inverse_GC = XtGetGC(new, valueMask, &values);
  ResizeHexagons(w);
}

static void DestroyHexagons(old)
  Widget old;
{
  HexagonsWidget w = (HexagonsWidget) old;

  XtReleaseGC(old, w->hexagons.tile_GC);
  XtReleaseGC(old, w->hexagons.puzzle_GC);
  XtReleaseGC(old, w->hexagons.inverse_GC);
  XtRemoveCallbacks(old, XtNselectCallback, w->hexagons.select);
}

static void ResizeHexagons(w)
  HexagonsWidget w;
{
  double temp_wid, temp_ht;
  double sqrt_3 = 1.73205080756887729352744634150587237;

  w->hexagons.delta.x = 2;
  w->hexagons.delta.y = 2;
  w->hexagons.tile_width = MAX((2 * ((int) w->core.width +
    2 * w->hexagons.delta.x - 1) - 4 * w->hexagons.size *
    w->hexagons.delta.x) / (4 * w->hexagons.size - 1), 0);
  w->hexagons.tile_height = MAX(((int) w->core.height - 1 -
    2 * w->hexagons.size * w->hexagons.delta.y) /
    (3 * w->hexagons.size - 1), 0);
  w->hexagons.wid = w->hexagons.tile_width + w->hexagons.delta.x;
  w->hexagons.ht = w->hexagons.tile_height + 2 * w->hexagons.delta.y;
  temp_ht = (int) ((double) w->hexagons.wid / sqrt_3);
  temp_wid = (int) ((double) w->hexagons.ht * sqrt_3);
  if (temp_ht <  w->hexagons.ht) {
    w->hexagons.wid = w->hexagons.tile_width + w->hexagons.delta.x;
    w->hexagons.ht = temp_ht;
  } else /* temp_wid <=  w->hexagons.wid */ {
    w->hexagons.wid = temp_wid;
    w->hexagons.ht = w->hexagons.tile_height + 2 * w->hexagons.delta.y;
  }
  w->hexagons.tile_width = MAX(w->hexagons.wid - w->hexagons.delta.x, 0);
  w->hexagons.tile_height = MAX(w->hexagons.ht - 2 * w->hexagons.delta.y , 0);
  w->hexagons.puzzle_width = w->hexagons.size * 2 * w->hexagons.wid -
    w->hexagons.tile_width / 2 - 2 * w->hexagons.delta.x + 1;
  w->hexagons.puzzle_height = w->hexagons.size * (3 * w->hexagons.tile_height +
    2 * w->hexagons.delta.y) - w->hexagons.tile_height + 2;
  w->hexagons.puzzle_offset.x =
    ((int) w->core.width - w->hexagons.puzzle_width + 2) / 2;
  w->hexagons.puzzle_offset.y =
    ((int) w->core.height - w->hexagons.puzzle_height + 2) / 2;
  resize_tiles(w);
}

static void ExposeHexagons(new, event, region)
  Widget new;
  XEvent *event;
  Region region; /* Not used */
{
  HexagonsWidget w = (HexagonsWidget) new;

  if (w->core.visible) {
    draw_frame(w, w->hexagons.puzzle_GC);
    draw_all_tiles(w, w->hexagons.tile_GC);
  }
}

static Boolean SetValuesHexagons(current, request, new)
  Widget current, request, new;
{
  HexagonsWidget c = (HexagonsWidget) current, w = (HexagonsWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  Boolean redraw = FALSE;
  Boolean redraw_tiles = FALSE;

  check_tiles(w);
  if (w->hexagons.foreground != c->hexagons.foreground) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->hexagons.foreground;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->hexagons.puzzle_GC);
    w->hexagons.puzzle_GC = XtGetGC(new, valueMask, &values);
    redraw_tiles = TRUE;
  }
  if (w->core.background_pixel != c->core.background_pixel) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->core.background_pixel;
    values.background = w->hexagons.foreground;
    XtReleaseGC(new, w->hexagons.inverse_GC);
    w->hexagons.inverse_GC = XtGetGC(new, valueMask, &values);
    redraw_tiles = TRUE;
  }
  if (w->hexagons.tile_color != c->hexagons.tile_color) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->hexagons.tile_color;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->hexagons.tile_GC);
    w->hexagons.tile_GC = XtGetGC(new, valueMask, &values);
    redraw_tiles = TRUE;
  }
  if (w->hexagons.size != c->hexagons.size ||
      w->hexagons.corners != c->hexagons.corners ||
      w->hexagons.base != c->hexagons.base) {
    reset_tiles(w);
    ResizeHexagons(w);
    redraw = TRUE;
  }
  else if (w->hexagons.wid != c->hexagons.wid ||
           w->hexagons.ht != c->hexagons.ht) {
    ResizeHexagons(w);
    redraw = TRUE;
  }
  if (redraw_tiles && !redraw && XtIsRealized(new) && new->core.visible) {
    draw_frame(c, c->hexagons.inverse_GC);
    draw_all_tiles(c, c->hexagons.inverse_GC);
    draw_frame(w, w->hexagons.puzzle_GC);
    draw_all_tiles(w, w->hexagons.tile_GC);
  }
  return (redraw);
}

static void quit_hexagons(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  XtCloseDisplay(XtDisplay(w));
  exit(0);
}

static void select_hexagons(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  int x, y, i, j, k, l, mod_i, mod_j, space;

  space = (event->xbutton.button == Button1);
  x = event->xbutton.x - w->hexagons.puzzle_offset.x;
  y = event->xbutton.y - w->hexagons.puzzle_offset.y;
  /* First convert x and y coordinates to hexagon grid.  Keep in mind that
       the starting hexagon x position changes with "w->hexagons.size % 2". */
  if (x < w->hexagons.tile_width / 4)
    return;
  i = 2 * (x - w->hexagons.tile_width / 4) / w->hexagons.wid;
  j = 3 * (y - w->hexagons.delta.y) /
    (3 * w->hexagons.tile_height / 2 + w->hexagons.delta.y);
  mod_i = 2 * (x - w->hexagons.tile_width / 4) % w->hexagons.wid;
  mod_j = 3 * (y - w->hexagons.delta.y) %
   (3 * w->hexagons.tile_height / 2 + w->hexagons.delta.y);
  l = j / 3; /* Approximate to a rectangle just for now */
  if (j % 3 == 0) /* Then it is the triangle near bottom or top point */ {
    if ((w->hexagons.size - 1 + l + i) % 2) /* \ */ 
      l -= (mod_j * w->hexagons.wid < mod_i *
          (3 * w->hexagons.tile_height / 2 + w->hexagons.delta.y));
    else /* / */ 
      l -= (mod_j * w->hexagons.wid < (w->hexagons.wid - mod_i) *
          (3 * w->hexagons.tile_height / 2 + w->hexagons.delta.y));
  }
  if (i < (w->hexagons.size - 1 + l) % 2 || l < 0 ||
      l > 2 * (w->hexagons.size - 1)) 
    return;
  k = (i - ((w->hexagons.size - 1 + l) % 2)) / 2;
  /* Map the hexagon grid to hexagon position in puzzle. */
  i = (l < w->hexagons.size) ?
    k - (w->hexagons.size - 1 - l) / 2: k + (w->hexagons.size - 1 - l) / 2;
  j = (l < w->hexagons.size) ?
    w->hexagons.size - 1 + l : 3 * (w->hexagons.size - 1) - l;
  if  (i < 0 || i > j)
    return;
  w->hexagons.current_position = position_from_row(w, i, l);
  w->hexagons.current_row[ROW] = l;
  w->hexagons.current_row[TRBL] = trbl(w, w->hexagons.current_position, l);
  w->hexagons.current_row[TLBR] = tlbr(w, w->hexagons.current_position, l);
  if (check_solved(w))
    no_move_tiles(w);
  else if (w->hexagons.corners)
    select_tiles_corners(w, space);
  else
    select_tiles_nocorn(w);
}

static void randomize_hexagons(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  randomize_tiles(w);
}
 
static void maybe_randomize_hexagons(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->hexagons.started)
    randomize_tiles(w);
}

static void enter_hexagons(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;
  char c;
  int i, size, corners, moves;
  hexagonsCallbackStruct cb;

  if ((fp = fopen(DATAFILE, "r")) == NULL)
    (void) printf("Can not read %s to enter.\n", DATAFILE);
  else {
    flush_moves(w);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &size);
    if (size >= MINHEXAGONS && size <= MAXHEXAGONS) {
      for (i = w->hexagons.size; i < size; i++) {
        cb.reason = HEXAGONS_INC;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
      for (i = w->hexagons.size; i > size; i--) {
        cb.reason = HEXAGONS_DEC;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    } else
      (void) printf("%s corrupted: size %d should be between %d and %d\n",
         DATAFILE, size, MINHEXAGONS, MAXHEXAGONS);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &corners);
    if (w->hexagons.corners != (Boolean) corners) {
      cb.reason = HEXAGONS_CORNERS;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &moves);
    scan_start_position(fp, w);
    cb.reason = HEXAGONS_RESTORE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    set_start_position(w);
    scan_moves(fp, w, moves);
    (void) fclose(fp);
    (void) printf("%s: size %d, corners %d, moves %d.\n",
      DATAFILE, size, corners, moves);
  }
}

static void write_hexagons(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;

  if ((fp = fopen(DATAFILE, "w")) == NULL)
    (void) printf("Can not write to %s.\n", DATAFILE);
  else {
    (void) fprintf(fp, "size: %d\n", w->hexagons.size);
    (void) fprintf(fp, "corners: %d\n", (w->hexagons.corners) ? 1 : 0);
    (void) fprintf(fp, "moves: %d\n", num_moves());
    print_start_position(fp, w);
    print_moves(fp);
    (void) fclose(fp);
    (void) printf("Saved to %s.\n", DATAFILE);
  }
}

static void undo_hexagons(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (moves_made()) {
    int direction;

    get_move(&direction);
    direction = (direction + (COORD / 2)) % COORD;
    if (move_tiles_dir(w, direction)) {
      hexagonsCallbackStruct cb;

      cb.reason = HEXAGONS_UNDO;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}

static void solve_hexagons(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  /* solve_tiles(w); */ /* Sorry, unimplemented */
}
 
static void increment_hexagons(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  hexagonsCallbackStruct cb;
 
  if (w->hexagons.size == MAXHEXAGONS)
    return;
  cb.reason = HEXAGONS_INC;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
static void decrement_hexagons(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  hexagonsCallbackStruct cb;
 
  if (w->hexagons.size == MINHEXAGONS)
    return;
  cb.reason = HEXAGONS_DEC;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
static void corners_hexagons(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  hexagonsCallbackStruct cb;
 
  cb.reason = HEXAGONS_CORNERS;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void move_hexagons_tl(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_hexagons(w, TL);
}

static void move_hexagons_tr(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_hexagons(w, TR);
}

static void move_hexagons_left(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_hexagons(w, LEFT);
}

static void move_hexagons_right(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_hexagons(w, RIGHT);
}

static void move_hexagons_bl(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_hexagons(w, BL);
}

static void move_hexagons_br(w, event, args, n_args)
  HexagonsWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_hexagons(w, BR);
}

static int move_hexagons(w, direction)
  HexagonsWidget w;
  int direction;
{
  hexagonsCallbackStruct cb;

  if (check_solved(w)) {
    no_move_tiles(w);
    return FALSE;
  }
  if (!move_hexagons_dir(w, direction)) {
    cb.reason = HEXAGONS_BLOCKED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    return FALSE;
  }
  if (check_solved(w)) {
    cb.reason = HEXAGONS_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  return TRUE;
}

int move_hexagons_dir(w, direction)
  HexagonsWidget w;
  int direction;
{
  hexagonsCallbackStruct cb;

  if (move_tiles_dir(w, direction)) {
    cb.reason = HEXAGONS_MOVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    put_move(direction);
    return TRUE;
  }
  return FALSE;
}

static void select_tiles_corners(w, space)
  HexagonsWidget w;
  int space;
{
  int orient;
  hexagonsCallbackStruct cb;

  /* Are the spaces in a "row" with the mouse click?
  (If two, then one clicked on a space).*/
  if (w->hexagons.current_position == w->hexagons.space_position[LOW] ||
      w->hexagons.current_position == w->hexagons.space_position[HIGH]) {
    cb.reason = HEXAGONS_SPACE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    return;
  }
  if (find_tile_triangle(w,
       w->hexagons.current_position, w->hexagons.space_position[HIGH],
       w->hexagons.space_position[LOW], w->hexagons.current_row[ROW],
       w->hexagons.space_row[HIGH], w->hexagons.space_row[LOW]))
  {
    orient = (w->hexagons.space_position[HIGH] <
      w->hexagons.space_position[LOW]) ? !space : space;
    put_move(find_dir(w,
       w->hexagons.current_position, w->hexagons.space_position[orient],
       w->hexagons.current_row[ROW], w->hexagons.space_row[orient]));
    move_tiles_corners(w, w->hexagons.current_position,
      w->hexagons.current_row[ROW], orient);
    cb.reason = HEXAGONS_MOVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  } else {
    cb.reason = HEXAGONS_BLOCKED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    return;
  }
  if (check_solved(w)) {
    cb.reason = HEXAGONS_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void select_tiles_nocorn(w)
  HexagonsWidget w;
{
  hexagonsCallbackStruct cb;
  int row_type = -1, l, orient;

  /* Are the spaces in a "row" with the mouse click?
  (If two, then one clicked on a space).*/
  for (l = 0; l < ROWTYPES; l++)
    if (w->hexagons.current_row[l] == w->hexagons.space_row[l]) {
      if (row_type == -1)
        row_type = l;
      else {
        cb.reason = HEXAGONS_SPACE;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
        return;
      }
    }
  if (row_type != -1) {
    if (w->hexagons.current_position < w->hexagons.space_position[HIGH]) {
      while (w->hexagons.current_position < w->hexagons.space_position[HIGH]) {
        orient = (row_type == ROW) ?
          w->hexagons.space_row[ROW] : w->hexagons.space_row[ROW] - 1;
        move_tiles_nocorn(w, tile_next_to_space(w, row_type, HIGH), orient);
        cb.reason = HEXAGONS_MOVED;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
        switch (row_type) {
          case TLBR:
            put_move(BR);
            break;
          case TRBL:
            put_move(BL);
            break;
          case ROW:
            put_move(RIGHT);
            break;
          default:
            (void) printf ("select_tiles_nocorn: row_type %d\n", row_type);
        }
      }
    } else /*w->hexagons.current_position > w->hexagons.space_position[HIGH]*/ {
      while (w->hexagons.current_position > w->hexagons.space_position[HIGH]) {
        orient = (row_type == ROW) ?
          w->hexagons.space_row[ROW] : w->hexagons.space_row[ROW] + 1;
        move_tiles_nocorn(w, tile_next_to_space(w, row_type, LOW), orient);
        cb.reason = HEXAGONS_MOVED;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
        switch (row_type) {
          case TLBR:
            put_move(TL);
            break;
          case TRBL:
            put_move(TR);
            break;
          case ROW:
            put_move(LEFT);
            break;
          default:
            (void) printf ("select_tiles_nocorn: row_type %d\n", row_type);
        }
      }
    }
  } else {
    cb.reason = HEXAGONS_BLOCKED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    return;
  }
  if (check_solved(w)) {
    cb.reason = HEXAGONS_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void check_tiles(w)
  HexagonsWidget w;
{
  char buf[121];

  if (w->hexagons.size < MINHEXAGONS || w->hexagons.size > MAXHEXAGONS) {
    (void) sprintf(buf,
             "Number of Hexagons on a edge out of bounds, use %d..%d",
             MINHEXAGONS, MAXHEXAGONS);
    XtWarning(buf);
    w->hexagons.size = DEFAULTHEXAGONS;
  }
}

static void reset_tiles(w)
  HexagonsWidget w;
{
  int i;

  w->hexagons.size_size = 3 * w->hexagons.size * (w->hexagons.size - 1) + 1;
  w->hexagons.size_center = (w->hexagons.size_size - 1) / 2;
  w->hexagons.space_position[HIGH] = w->hexagons.size_size - 1;
  if (w->hexagons.corners) {
    w->hexagons.space_row[HIGH] = 2 * w->hexagons.size - 2;
    if (w->hexagons.size > 1) {
      w->hexagons.space_position[LOW] = w->hexagons.size_size - 2;
      w->hexagons.space_row[LOW] = 2 * w->hexagons.size - 2;
      w->hexagons.tile_of_position[w->hexagons.size_size - 2] = -1;
    }
  } else {
    w->hexagons.space_row[ROW] = w->hexagons.space_row[TRBL] =
      2 * w->hexagons.size - 2;
    w->hexagons.space_row[TLBR] = w->hexagons.size - 1;
  }
  w->hexagons.tile_of_position[w->hexagons.size_size - 1] = 0;
  for (i = 1; i < w->hexagons.size_size - ((w->hexagons.corners) ? 1 : 0);
       i++)
    w->hexagons.tile_of_position[i - 1] = i;
  flush_moves(w);
  w->hexagons.started = FALSE;
}

static void resize_tiles(w)
  HexagonsWidget w;
{
  int i;

  for (i = 0; i < 6; i++) { 
    hexagon_list[NOCORN][i].x = w->hexagons.tile_width *
      hexagon_unit[NOCORN][i].x / 4;
    hexagon_list[NOCORN][i].y = 3 * w->hexagons.tile_height *
      hexagon_unit[NOCORN][i].y / 4;
    hexagon_list[CORNERS][i].x = w->hexagons.tile_width *
      hexagon_unit[CORNERS][i].x / 2;
    hexagon_list[CORNERS][i].y = w->hexagons.tile_height *
      hexagon_unit[CORNERS][i].y / 2;
  }
  w->hexagons.digit_offset.x = 3;
  w->hexagons.digit_offset.y = 4;
}

static int movable_tiles_corners(w, direction, position, posrow, space)
  HexagonsWidget w;
  int direction, *position, *posrow, *space;
{
  int space_type, highest, side = -1;

  highest = (w->hexagons.space_position[HIGH] >
    w->hexagons.space_position[LOW]) ? HIGH : LOW;
  space_type = find_space_type(w, 
    w->hexagons.space_position[HIGH], w->hexagons.space_position[LOW],
    w->hexagons.space_row[HIGH], w->hexagons.space_row[LOW]);
  switch (space_type) {
    case TRBL:
      if (direction == TR || direction == BL)
        return FALSE;
      side = next_to_wall(w, w->hexagons.space_position[highest],
      w->hexagons.space_row[highest], space_type);
      if (side != -1)
      {
        if ((side == HIGH && direction == RIGHT) ||
            (side == HIGH && direction == BR) ||
            (side == LOW && direction == LEFT) ||
            (side == LOW && direction == TL))
          return FALSE;
      } else
        side = (direction == TL || direction == LEFT);
      *space = (direction == BR || direction == LEFT);
      break;
    case TLBR:
      if (direction == TL || direction == BR)
        return FALSE;
      side = next_to_wall(w, w->hexagons.space_position[highest],
      w->hexagons.space_row[highest], space_type);
      if (side != -1) {
        if ((side == LOW && direction == TR) ||
            (side == LOW && direction == RIGHT) ||
            (side == HIGH && direction == BL) ||
            (side == HIGH && direction == LEFT))
          return FALSE;
      } else
        side = (direction == TR || direction == RIGHT);
      *space = (direction == RIGHT || direction == BL);
      break;
    case ROW:
      if (direction == LEFT || direction == RIGHT)
        return FALSE;
      side = next_to_wall(w, w->hexagons.space_position[highest],
      w->hexagons.space_row[highest], space_type);
      if (side != -1) {
        if ((side == LOW && direction == TR) ||
            (side == HIGH && direction == BR) ||
            (side == HIGH && direction == BL) ||
            (side == LOW && direction == TL))
          return FALSE;
      } else
        side = (direction == TR || direction == TL);
      *space = (direction == TR || direction == BR);
      break;
  }
  find_movable_tile(w, w->hexagons.space_position[highest],
    w->hexagons.space_row[highest], space_type, side, position, posrow);
  return TRUE;
}

static void no_move_tiles(w)
  HexagonsWidget w;
{
  hexagonsCallbackStruct cb;
 
  cb.reason = HEXAGONS_IGNORE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static int move_tiles_dir(w, direction)
  HexagonsWidget w;
  int direction;
{
  if (w->hexagons.corners)
    return move_tiles_corners_dir(w, direction);
  else
    return move_tiles_nocorn_dir(w, direction);
}


static int move_tiles_corners_dir(w, direction)
  HexagonsWidget w;
  int direction;
{
  int position, posrow, space;
  
  if (movable_tiles_corners(w, direction, &position, &posrow, &space)) {
    move_tiles_corners(w, position, posrow, (w->hexagons.space_position[HIGH] <
      w->hexagons.space_position[LOW]) ? !space : space);
    return TRUE;
  }
  return FALSE;
}
 
static int move_tiles_nocorn_dir(w, direction)
  HexagonsWidget w;
  int direction;
{
  switch (direction) {
    case TR:
      if (w->hexagons.space_row[ROW] != 2 * w->hexagons.size - 2 &&
          w->hexagons.space_row[TLBR] != 2 * w->hexagons.size - 2) {
        move_tiles_nocorn(w, tile_next_to_space(w, TRBL, LOW),
          w->hexagons.space_row[ROW] + 1);
        return TRUE;
      }
      break;
    case RIGHT:
      if (w->hexagons.space_row[TRBL] != 0 &&
          w->hexagons.space_row[TLBR] != 2 * w->hexagons.size - 2) {
        move_tiles_nocorn(w, tile_next_to_space(w, ROW, HIGH),
          w->hexagons.space_row[ROW]);
        return TRUE;
      }
      break;
    case BR:
      if (w->hexagons.space_row[ROW] != 0 &&
          w->hexagons.space_row[TRBL] != 0) {
        move_tiles_nocorn(w, tile_next_to_space(w, TLBR, HIGH),
          w->hexagons.space_row[ROW] - 1);
        return TRUE;
      }
      break;
    case BL:
      if (w->hexagons.space_row[ROW] != 0 &&
          w->hexagons.space_row[TLBR] != 0)
      {
        move_tiles_nocorn(w, tile_next_to_space(w, TRBL, HIGH),
          w->hexagons.space_row[ROW] - 1);
        return TRUE;
      }
      break;
    case LEFT:
      if (w->hexagons.space_row[TLBR] != 0 &&
          w->hexagons.space_row[TRBL] != 2 * w->hexagons.size - 2)
      {
        move_tiles_nocorn(w, tile_next_to_space(w, ROW, LOW),
          w->hexagons.space_row[ROW]);
        return TRUE;
      }
      break;
    case TL:
      if (w->hexagons.space_row[ROW] != 2 * w->hexagons.size - 2 &&
          w->hexagons.space_row[TRBL] != 2 * w->hexagons.size - 2)
      {
        move_tiles_nocorn(w, tile_next_to_space(w, TLBR, LOW),
          w->hexagons.space_row[ROW] + 1);
        return TRUE;
      }
      break;
    default:
      (void) printf("move_tiles_nocorn_dir: direction %d\n", direction);
  }
  return FALSE;
}
 
static void randomize_tiles(w)
  HexagonsWidget w;
{
  hexagonsCallbackStruct cb;

  /* First interchange tiles an even number of times */
  if (w->hexagons.size > 1 + ((w->hexagons.corners) ? 1 : 0)) {
    int current_pos, random_pos;
    int count = 0;

    for (current_pos = 0; current_pos < w->hexagons.size_size; current_pos++) {
      random_pos = current_pos;
      while (current_pos == random_pos)
        random_pos = NRAND(w->hexagons.size_size);
      count += exchange_tiles(w, current_pos, random_pos);
    }
    if (count % 2 && w->hexagons.corners)
      if (!exchange_tiles(w, 0, 1))
        if (!exchange_tiles(w,
            w->hexagons.size_size - 2, w->hexagons.size_size - 1))
          (void) printf("randomize_bricks: should not get here\n");
    draw_all_tiles(w, w->hexagons.tile_GC);
  }
  /* Now move the spaces around randomly */
  if (w->hexagons.size > 1) {
    int big = w->hexagons.size_size + NRAND(2);
    int last_direction = 0;
    int random_direction;

    cb.reason = HEXAGONS_RESET;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
 
    if (w->hexagons.corners && w->hexagons.size == 2)
      big *= big;
 
#ifdef DEBUG
    big = 3;
#endif

    while (big--) {
      random_direction = NRAND(COORD);

#ifdef DEBUG
      sleep(1);
#endif

      if ((random_direction + COORD / 2) % COORD != last_direction) {
        if (move_hexagons_dir(w, random_direction))
          last_direction = random_direction;
        else
          big++;
      }
    }
    flush_moves(w);
    cb.reason = HEXAGONS_RANDOMIZE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  if (check_solved(w)) {
    cb.reason = HEXAGONS_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void move_tiles_corners(w, from, posrow, space)
  HexagonsWidget w;
  int from, posrow, space;
{
  int temp_tile;

  temp_tile = w->hexagons.tile_of_position[from];
  w->hexagons.tile_of_position[from] =
    w->hexagons.tile_of_position[w->hexagons.space_position[space]];
  w->hexagons.tile_of_position[w->hexagons.space_position[space]] =
    temp_tile;
  draw_tile(w, w->hexagons.tile_GC, w->hexagons.space_position[space]);
  w->hexagons.space_position[space] = from;
  w->hexagons.space_row[space] = posrow;
  draw_tile(w, w->hexagons.inverse_GC, w->hexagons.space_position[space]);
}

static void move_tiles_nocorn(w, from, posrow)
  HexagonsWidget w;
  int from, posrow;
{
  int temp_tile;

  temp_tile = w->hexagons.tile_of_position[from];
  w->hexagons.tile_of_position[from] =
     w->hexagons.tile_of_position[w->hexagons.space_position[HIGH]];
  w->hexagons.tile_of_position[w->hexagons.space_position[HIGH]] = temp_tile;
  draw_tile(w, w->hexagons.tile_GC, w->hexagons.space_position[HIGH]);
  w->hexagons.space_position[HIGH] = from;
  w->hexagons.space_row[ROW] = posrow;
  w->hexagons.space_row[TRBL] = trbl(w, from, posrow);
  w->hexagons.space_row[TLBR] = tlbr(w, from, posrow);
  draw_tile(w, w->hexagons.inverse_GC, w->hexagons.space_position[HIGH]);
}

static int exchange_tiles(w, pos1, pos2)
  HexagonsWidget w;
  int pos1, pos2;
{
  int temp_tile;

  if (w->hexagons.tile_of_position[pos1] <= 0)
    return FALSE;
  else if (w->hexagons.tile_of_position[pos2] <= 0)
    return FALSE;
  temp_tile = w->hexagons.tile_of_position[pos1];
  w->hexagons.tile_of_position[pos1] = w->hexagons.tile_of_position[pos2];
  w->hexagons.tile_of_position[pos2] = temp_tile;
  return TRUE;
}

#ifdef DEBUG
static int within_frame(w, x, y, dx, dy)
  HexagonsWidget w;
  int x, y, dx, dy;
{
  return
    (x < dx + w->hexagons.tile_width / 2 &&
     x > dx - w->hexagons.tile_width / 2 &&
     w->hexagons.tile_height * (x - dx) < w->hexagons.tile_width * (y - dy) &&
     w->hexagons.tile_height * (dx - x) < w->hexagons.tile_width * (y - dy) &&
     w->hexagons.tile_height * (x - dx + 2 * w->hexagons.tile_width) >
       w->hexagons.tile_width * (y - dy) &&
     w->hexagons.tile_height * (dx - x + 2 * w->hexagons.tile_width) >
       w->hexagons.tile_width * (y - dy));
}
#endif

static int next_to_wall(w, pos, posrow, space_type)
  HexagonsWidget w;
  int pos, posrow, space_type;
{
  switch (space_type) {
    case TRBL:
      if (posrow < w->hexagons.size  && pos ==
          w->hexagons.size * posrow + posrow * (posrow - 1) / 2)
        return (HIGH);
      else if (posrow >= w->hexagons.size && pos == w->hexagons.size *
          (posrow - w->hexagons.size) + 3 * w->hexagons.size - posrow -
          2 + 4 * w->hexagons.size * (w->hexagons.size - 1) / 2 -
          (2 * w->hexagons.size - posrow - 2) *
          (2 * w->hexagons.size - posrow - 1) / 2)
        return (LOW);
      else
        return (-1);
    case TLBR:
      if (posrow < w->hexagons.size && pos ==
          w->hexagons.size * (posrow + 1) + posrow * (posrow + 1) / 2 - 1)
        return (HIGH);
      else if (posrow >= w->hexagons.size && pos == w->hexagons.size *
          (posrow - w->hexagons.size) + 1 + 4 * w->hexagons.size *
          (w->hexagons.size - 1) / 2 - (2 * w->hexagons.size - posrow - 2) *
          (2 * w->hexagons.size - posrow - 1) / 2)
        return (LOW);
      else
        return (-1);
    case ROW:
      if (posrow == 0)
        return (HIGH);
      else if (posrow == 2 * (w->hexagons.size - 1))
        return (LOW);
      else
        return (-1);
  }
  return (-2); /*Unknown space formation.*/
}

static int tile_next_to_space(w, row_type, direction)
  HexagonsWidget w;
  int row_type, direction;
{
  if (direction == HIGH) {
    if (row_type == TRBL)
      return ((w->hexagons.space_row[ROW] < w->hexagons.size) ?
        w->hexagons.space_position[HIGH] - w->hexagons.size -
          w->hexagons.space_row[ROW] + 1 :
        w->hexagons.space_position[HIGH]- 3 * w->hexagons.size +
          w->hexagons.space_row[ROW] + 2);
    else if (row_type == TLBR)
      return ((w->hexagons.space_row[ROW] < w->hexagons.size) ?
             w->hexagons.space_position[HIGH] - w->hexagons.size -
               w->hexagons.space_row[ROW] :
             w->hexagons.space_position[HIGH] - 3 * w->hexagons.size +
               w->hexagons.space_row[ROW] + 1);
    else /* row_type == ROW */
      return (w->hexagons.space_position[HIGH] - 1);
  } else /* direction == LOW */ {
    if (row_type == TRBL)
      return ((w->hexagons.space_row[ROW] < w->hexagons.size - 1) ?
        w->hexagons.space_position[HIGH] + w->hexagons.size +
          w->hexagons.space_row[ROW] :
        w->hexagons.space_position[HIGH] + 3 * w->hexagons.size -
          w->hexagons.space_row[ROW] - 3);
    else if (row_type == TLBR)
      return ((w->hexagons.space_row[ROW] < w->hexagons.size - 1) ?
        w->hexagons.space_position[HIGH] + w->hexagons.size +
          w->hexagons.space_row[ROW] + 1 :
        w->hexagons.space_position[HIGH]+ 3 * w->hexagons.size -
          w->hexagons.space_row[ROW] - 2);
    else /* row_type == ROW */
      return (w->hexagons.space_position[HIGH] + 1);
  }
}

static int find_tile_triangle(w, p_i, p_j, p_k, r_i, r_j, r_k)
  HexagonsWidget w;
  int p_i, p_j, p_k, r_i, r_j, r_k;
{
  int found = TRUE, temp = 0, k = 0, row1 = 0, row2 = 0, pos;

  if (r_i == r_j) {
    if (p_i == p_j - 1)
      temp = p_j;
    else if (p_i == p_j + 1)
      temp = p_i;
    else
      found = FALSE;
    k = p_k;
    row1 = r_i;
    row2 = r_k;
  } else if (r_j == r_k) {
    if (p_j == p_k - 1)
      temp = p_k;
    else if (p_j == p_k + 1)
      temp = p_j;
    else
      found = FALSE;
    k = p_i;
    row1 = r_j;
    row2 = r_i;
  } else if (r_k == r_i) {
    if (p_k == p_i - 1)
      temp = p_i;
    else if (p_k == p_i + 1)
      temp = p_k;
    else
      found = FALSE;
    k = p_j;
    row1 = r_k;
    row2 = r_j;
  }
  if (found == FALSE)
    return (0);
  pos = -1;
  if (row1 == row2 + 1) {
    if (row1 <= w->hexagons.size - 1)
      pos = temp - w->hexagons.size - row1;
    else /* row1 > w->hexagons.size - 1 */
      pos = temp - 3 * w->hexagons.size + row1 + 1;
  } else if (row1 == row2 - 1) {
    if (row1 < w->hexagons.size - 1)
      pos = temp + w->hexagons.size + row1;
    else /* row1 >= w->hexagons.size - 1 */
      pos = temp + 3 * (w->hexagons.size - 1) - row1;
  }
  if (k == pos)
    return (1);
  return (0);
}

static int find_dir(w, pos_tile, pos_space, row_tile, row_space)
  HexagonsWidget w;
  int pos_tile, pos_space, row_tile, row_space;
{
  if (row_tile == row_space) {
    if (pos_tile > pos_space)
      return LEFT;
    else
      return RIGHT; 
  } else if (trbl(w, pos_tile, row_tile) == trbl(w, pos_space, row_space)) {
    if (pos_tile > pos_space)
      return TR;
    else
      return BL; 
  }  else {
    /* if (tlbr(w, pos_tile, row_tile) == tlbr(w, pos_space, row_space)) */
    if (pos_tile > pos_space)
      return TL;
    else
      return BR;
  }
}

static int find_space_type(w, pos1, pos2, row1, row2)
  HexagonsWidget w;
  int pos1, pos2, row1, row2;
{
  if (row1 == row2 && (pos1 == pos2 + 1 || pos1 == pos2 - 1))
    return (ROW);
  else if (row1 == row2 - 1) {
    swap(&row1, &row2);
    swap(&pos1, &pos2);
  }
  if (row1 == row2 + 1) {
    if (row1 <= w->hexagons.size - 1) {
      if (pos2 == pos1 - w->hexagons.size - row1)
        return (TLBR);
      else if (pos2 == pos1 - w->hexagons.size - row1 + 1)
        return (TRBL);
    } else /* row1 > w->hexagons.size - 1 */ {
      if (pos2 == pos1 - 3 * w->hexagons.size + row1 + 1)
        return (TLBR);
      else if (pos2 == pos1 - 3 * w->hexagons.size + row1 + 2)
        return (TRBL);
    }
  }
  return (-1);
}

static void find_movable_tile(w,
    pos, posrow, space_type, side, tile_pos, tile_row)
  HexagonsWidget w;
  int pos, posrow, space_type, side, *tile_pos, *tile_row;
{
  switch (space_type) {
    case TRBL:
      if (side == HIGH) {
        *tile_row = posrow;
        *tile_pos = pos + 1;
      } else /* side == LOW */ {
        *tile_row = posrow - 1;
        *tile_pos = (posrow <= w->hexagons.size - 1) ?
          pos - w->hexagons.size - posrow :
          pos - 3 * w->hexagons.size + posrow + 1;
      }
      break;
    case TLBR:
      if (side == HIGH) {
        *tile_row = posrow;
        *tile_pos = pos - 1;
      } else /* side == LOW */ {
        *tile_row = posrow - 1;
        *tile_pos = (posrow <= w->hexagons.size - 1) ?
          pos - w->hexagons.size - posrow + 1 :
          pos - 3 * w->hexagons.size + posrow + 2;
      }
      break;
    case ROW:
      if (side == HIGH) {
        *tile_row = posrow + 1;
        *tile_pos = (posrow < w->hexagons.size - 1) ?
          pos + w->hexagons.size + posrow :
          pos + 3 * w->hexagons.size - posrow - 3;
      } else /* side == LOW */ {
        *tile_row = posrow - 1;
        *tile_pos = (posrow <= w->hexagons.size - 1) ?
          pos - w->hexagons.size - posrow :
          pos - 3 * w->hexagons.size + posrow + 1;
      }
      break;
    default:
      (void) printf("find_movable_tile: space_type %d.\n", space_type);
   }
}
 
static void draw_frame(w, gc)
  HexagonsWidget w;
  GC gc;
{
  int sum_x, sum_y, sum_x4, sum_3x4, sum_y2, offset_x, offset_y;

  sum_x = w->hexagons.size * (2 * w->hexagons.wid) - w->hexagons.tile_width -
    2 * w->hexagons.delta.x + w->hexagons.tile_width / 2 - 1;
  sum_y = w->hexagons.size * (3 * w->hexagons.tile_height + 2 *
    w->hexagons.delta.y) - w->hexagons.tile_height - 1;
  offset_x = w->hexagons.puzzle_offset.x - 1;
  offset_y = w->hexagons.puzzle_offset.y;
  sum_x4 = sum_x / 4 + offset_x;
  sum_3x4 = 3 * sum_x / 4 + offset_x + 2;
  sum_y2 = sum_y / 2 + offset_y;
  sum_x += offset_x + 1 + w->hexagons.size / 2;
  sum_y += offset_y;
  offset_x += 1 - w->hexagons.size / 2;
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    sum_x4, offset_y, sum_3x4, offset_y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    sum_3x4, offset_y, sum_x, sum_y2);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    sum_x, sum_y2, sum_3x4, sum_y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    sum_3x4, sum_y, sum_x4, sum_y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    sum_x4, sum_y, offset_x, sum_y2);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    offset_x, sum_y2, sum_x4, offset_y);
}   

void draw_all_tiles(w, gc)
  HexagonsWidget w;
  GC gc;
{
  int k;

  for (k = 0; k < w->hexagons.size_size; k++)
    draw_tile(w, (w->hexagons.tile_of_position[k] <= 0) ?
       w->hexagons.inverse_GC : gc, k);
}

static void draw_tile(w, gc, pos)
  HexagonsWidget w;
  GC gc;
  int pos;
{
  int dx, dy, k = row(w, pos), orient = (w->hexagons.corners) ? 1 : 0;

  dx = w->hexagons.tile_width / 4 - 1 + (2 * trbl(w, pos, k) +
    w->hexagons.size - k) * w->hexagons.wid / 2 + w->hexagons.puzzle_offset.x;
  dy = k * (3 * w->hexagons.tile_height / 2 + w->hexagons.delta.y) +
    w->hexagons.delta.y - 1 + w->hexagons.puzzle_offset.y;
  if (orient) {
    hexagon_list[orient][0].x = dx;
    hexagon_list[orient][0].y = dy;
  } else {
    hexagon_list[orient][0].x = dx - w->hexagons.wid / 4;
    hexagon_list[orient][0].y = dy +
      (w->hexagons.tile_height + w->hexagons.delta.y) / 4;
  }
  XFillPolygon(XtDisplay(w), XtWindow(w), gc, hexagon_list[orient], 6,
    Convex, CoordModePrevious);
  if (gc != w->hexagons.inverse_GC) {
    int i = 0, offset_x = 0;
    int tile = w->hexagons.tile_of_position[pos];
    char buf[5];

    (void) sprintf(buf, "%d", tile);
    while (tile >= 1) {
      tile /= w->hexagons.base;
      offset_x += w->hexagons.digit_offset.x;
      i++;
    }
    XDrawString(XtDisplay(w), XtWindow(w), w->hexagons.inverse_GC,
      dx - offset_x,
      dy + w->hexagons.tile_height + w->hexagons.digit_offset.y, buf, i);
  }
}

#ifdef DEBUG
static int position_in_row(w, position, posrow)
  HexagonsWidget w;
  int position, posrow;
{
  return (posrow <= w->hexagons.size - 1) ?
    (position - w->hexagons.size * posrow - posrow * (posrow - 1) / 2) :
    (position - w->hexagons.size * (posrow - w->hexagons.size) -
      4 * w->hexagons.size * (w->hexagons.size - 1) / 2 + (2 *
      w->hexagons.size - posrow - 2) * (2 * w->hexagons.size - posrow - 1) /
      2 - 1);
}
#endif
 
static int position_from_row(w, row_position, posrow)
  HexagonsWidget w;
  int row_position, posrow;
{
  return (posrow <= w->hexagons.size - 1) ?
    (w->hexagons.size * posrow + posrow * (posrow - 1) / 2 + row_position) :
    (w->hexagons.size * (posrow - w->hexagons.size) + 4 *
      w->hexagons.size * (w->hexagons.size - 1) / 2 - (2 * w->hexagons.size -
      posrow - 2) * (2 * w->hexagons.size - posrow - 1) / 2 + 1 + row_position);
}

int row(w, pos)
  HexagonsWidget w;
  int pos;
{
  return (pos <= w->hexagons.size_center) ?
    (1 + Sqrt(1 + 8 * (pos + w->hexagons.size *
    (w->hexagons.size - 1) / 2))) / 2 - w->hexagons.size :
    3 * w->hexagons.size - 2 - (1 + Sqrt(1 + 8 * (w->hexagons.size_size - 1 +
    w->hexagons.size * (w->hexagons.size - 1) / 2 - pos))) / 2;
}

/* Passing row so there is no sqrt calculation again */
int trbl(w, pos, posrow)
  HexagonsWidget w;
  int pos, posrow;
{
  return (pos <= w->hexagons.size_center) ?
    (pos + w->hexagons.size * (w->hexagons.size - 1) / 2) -
    (posrow + w->hexagons.size) * (posrow + w->hexagons.size - 1) / 2 :
    2 * w->hexagons.size - 2 - (w->hexagons.size_size - 1 +
    w->hexagons.size * (w->hexagons.size - 1) / 2 - pos -
    (3 * w->hexagons.size - posrow - 2) * (3 * w->hexagons.size - posrow - 3) /
    2);
}

int tlbr(w, pos, posrow)
  HexagonsWidget w;
  int pos, posrow;
{
  return (pos <= w->hexagons.size_center) ?
    -1 - ((pos + w->hexagons.size * (w->hexagons.size - 1) / 2) -
    (posrow + w->hexagons.size + 1) * (posrow + w->hexagons.size) / 2):
    2 * w->hexagons.size - 1 + (w->hexagons.size_size - 1 +
    w->hexagons.size * (w->hexagons.size - 1) / 2 - pos -
    (3 * w->hexagons.size - posrow - 1) * (3 * w->hexagons.size - posrow - 2) /
    2);
}

/* This is fast for small i, a -1 is returned for negative i. */
static int Sqrt(i)
  int i;
{
  int j = 0;

  while (j * j <= i)
    j++;
  return (j - 1);
}

Boolean check_solved(w)
  HexagonsWidget w;
{
  int i;

  for (i = 1; i < w->hexagons.size_size - ((w->hexagons.corners) ? 1 : 0); i++)
    if (w->hexagons.tile_of_position[i - 1] != i)
      return FALSE;
  return TRUE;
}

static void swap(a, b)
  int *a, *b;
{
  int c;

  c = *b;
  *b = *a;
  *a = c;
}
