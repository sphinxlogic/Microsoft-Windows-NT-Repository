/*
# X-BASED TRIANGLES
#
#  Triangles.c
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

/* Methods file for Triangles */

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
#include "TrianglesP.h"

#ifndef DATAFILE
#define DATAFILE "/usr/games/lib/triangles.data"
#endif

static void InitializeTriangles();
static void ExposeTriangles();
static void ResizeTriangles();
static void DestroyTriangles();
static Boolean SetValuesTriangles();
static void quit_triangles();
static void move_triangles_tl();
static void move_triangles_tr();
static void move_triangles_left();
static void move_triangles_right();
static void move_triangles_bl();
static void move_triangles_br();
static void select_triangles();
static void randomize_triangles();
static void maybe_randomize_triangles();
static void enter_triangles();
static void write_triangles();
static void undo_triangles();
static void solve_triangles();
static void increment_triangles();
static void decrement_triangles();
static int move_triangles();
static void select_tiles();
static void check_tiles();
static void reset_tiles();
static void resize_tiles();
static void no_move_tiles();
static int move_tiles_dir();
static void randomize_tiles();
static void move_tiles();
static int exchange_tiles();
static int tile_next_to_space();
static void draw_frame();
static void draw_tile();
static int Sqrt();

static char defaultTranslationsTriangles[] =
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
   <KeyPress>r: randomize()\n\
   <Btn3Down>(2+): randomize()\n\
   <Btn3Down>: maybe_randomize()\n\
   <KeyPress>e: enter()\n\
   <KeyPress>w: write()\n\
   <KeyPress>u: undo()\n\
   <KeyPress>s: solve()\n\
   <KeyPress>i: increment()\n\
   <KeyPress>d: decrement()";
 
static XtActionsRec actionsListTriangles[] =
{
  {"quit", (XtActionProc) quit_triangles},
  {"move_tl", (XtActionProc) move_triangles_tl},
  {"move_tr", (XtActionProc) move_triangles_tr},
  {"move_left", (XtActionProc) move_triangles_left},
  {"move_right", (XtActionProc) move_triangles_right},
  {"move_bl", (XtActionProc) move_triangles_bl},
  {"move_br", (XtActionProc) move_triangles_br},
  {"select", (XtActionProc) select_triangles},
  {"randomize", (XtActionProc) randomize_triangles},
  {"maybe_randomize", (XtActionProc) maybe_randomize_triangles},
  {"enter", (XtActionProc) enter_triangles},
  {"write", (XtActionProc) write_triangles},
  {"undo", (XtActionProc) undo_triangles},
  {"solve", (XtActionProc) solve_triangles},
  {"increment", (XtActionProc) increment_triangles},
  {"decrement", (XtActionProc) decrement_triangles}
};

static XtResource resourcesTriangles[] =
{
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
   XtOffset(TrianglesWidget, triangles.foreground), XtRString,
   XtDefaultForeground},
  {XtNtileColor, XtCColor, XtRPixel, sizeof(Pixel),
   XtOffset(TrianglesWidget, triangles.tile_color), XtRString,
   XtDefaultForeground},
  {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
   XtOffset(TrianglesWidget, core.width), XtRString, "200"},
  {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
   XtOffset(TrianglesWidget, core.height), XtRString, "173"},
  {XtNsize, XtCSize, XtRInt, sizeof(int),
   XtOffset(TrianglesWidget, triangles.size), XtRString, "4"}, /* DEFAULTTRIS */
  {XtNbase, XtCBase, XtRInt, sizeof(int),
   XtOffset(TrianglesWidget, triangles.base), XtRString, "10"},
  {XtNstart, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(TrianglesWidget, triangles.started), XtRString, "FALSE"},
  {XtNselectCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
   XtOffset(TrianglesWidget, triangles.select), XtRCallback, NULL}
};

TrianglesClassRec trianglesClassRec =
{
  {
    (WidgetClass) &widgetClassRec,	/* superclass */
    "Triangles",			/* class name */
    sizeof(TrianglesRec),		/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeTriangles,		/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    actionsListTriangles,		/* actions */
    XtNumber(actionsListTriangles),	/* num actions */
    resourcesTriangles,			/* resources */
    XtNumber(resourcesTriangles),	/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    DestroyTriangles,			/* destroy */
    ResizeTriangles,			/* resize */
    ExposeTriangles,			/* expose */
    SetValuesTriangles,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    NULL,				/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    defaultTranslationsTriangles,	/* tm table */
    NULL,				/* query geometry */
    NULL,				/* display accelerator */
    NULL				/* extension */
  },
  {
    0					/* ignore */
  }
};

WidgetClass trianglesWidgetClass = (WidgetClass) &trianglesClassRec;

static XPoint triangle_unit[MAXORIENT][ROWTYPES] =
{
  {{0, 0}, {-1, -1}, { 2,  0}},
  {{0, 0}, { 1,  1}, {-2,  0}}
};
static XPoint triangle_list[MAXORIENT][ROWTYPES];

static void InitializeTriangles(request, new)
  Widget request, new;
{
  TrianglesWidget w = (TrianglesWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  
  check_tiles(w);
  init_moves();
  reset_tiles(w);
  (void) SRAND(getpid());
  valueMask = GCForeground | GCBackground;
  values.foreground = w->triangles.foreground;
  values.background = w->core.background_pixel;
  w->triangles.puzzle_GC = XtGetGC(new, valueMask, &values);
  values.foreground = w->triangles.tile_color;
  w->triangles.tile_GC = XtGetGC(new, valueMask, &values);
  valueMask = GCForeground | GCBackground;
  values.foreground = w->core.background_pixel;
  values.background = w->triangles.foreground;
  w->triangles.inverse_GC = XtGetGC(new, valueMask, &values);
  ResizeTriangles(w);
}

static void DestroyTriangles(old)
  Widget old;
{
  TrianglesWidget w = (TrianglesWidget) old;

  XtReleaseGC(old, w->triangles.tile_GC);
  XtReleaseGC(old, w->triangles.puzzle_GC);
  XtReleaseGC(old, w->triangles.inverse_GC);
  XtRemoveCallbacks(old, XtNselectCallback, w->triangles.select);
}

static void ResizeTriangles(w)
  TrianglesWidget w;
{
  double temp_wid, temp_ht;
  double sqrt_3 = 1.73205080756887729352744634150587237;

  w->triangles.delta.x = 5;
  w->triangles.delta.y = 3;
  w->triangles.wid = MAX(((int) w->core.width - w->triangles.delta.x) /
    w->triangles.size, 0);
  w->triangles.ht = MAX(((int) w->core.height - 2 * w->triangles.delta.y) /
    w->triangles.size, 0);
  temp_ht = (int) ((double) w->triangles.wid * sqrt_3 / 2.0);
  temp_wid = (int) ((double) w->triangles.ht * 2.0 / sqrt_3);
  if (temp_ht <  w->triangles.ht)
    w->triangles.ht = temp_ht;
  else /* temp_wid <=  w->triangles.wid */
    w->triangles.wid = temp_wid;
  w->triangles.puzzle_width = w->triangles.wid * w->triangles.size +
    w->triangles.delta.x + 2;
  w->triangles.puzzle_height = w->triangles.ht * w->triangles.size +
    w->triangles.delta.y + 2;
  w->triangles.puzzle_offset.x = ((int) w->core.width -
    w->triangles.puzzle_width + 2) / 2;
  w->triangles.puzzle_offset.y = ((int) w->core.height -
    w->triangles.puzzle_height + 2) / 2;
  w->triangles.tile_width = MAX(w->triangles.wid - w->triangles.delta.x, 0);
  w->triangles.tile_height = MAX(w->triangles.ht - w->triangles.delta.y , 0);
  resize_tiles(w);
}

static void ExposeTriangles(new, event, region)
  Widget new;
  XEvent *event;
  Region region; /* Not used */
{
  TrianglesWidget w = (TrianglesWidget) new;

  if (w->core.visible) {
    draw_frame(w, w->triangles.puzzle_GC);
    draw_all_tiles(w, w->triangles.tile_GC);
  }
}

static Boolean SetValuesTriangles(current, request, new)
  Widget current, request, new;
{
  TrianglesWidget c = (TrianglesWidget) current, w = (TrianglesWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  Boolean redraw = FALSE;
  Boolean redraw_tiles = FALSE;

  check_tiles(w);
  if (w->triangles.foreground != c->triangles.foreground) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->triangles.foreground;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->triangles.puzzle_GC);
    w->triangles.puzzle_GC = XtGetGC(new, valueMask, &values);
    redraw_tiles = TRUE;
  }
  if (w->core.background_pixel != c->core.background_pixel) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->core.background_pixel;
    values.background = w->triangles.foreground;
    XtReleaseGC(new, w->triangles.inverse_GC);
    w->triangles.inverse_GC = XtGetGC(new, valueMask, &values);
    redraw_tiles = TRUE;
  }
  if (w->triangles.tile_color != c->triangles.tile_color) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->triangles.tile_color;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->triangles.tile_GC);
    w->triangles.tile_GC = XtGetGC(new, valueMask, &values);
    redraw_tiles = TRUE;
  }
  if (w->triangles.size != c->triangles.size ||
      w->triangles.base != c->triangles.base) {
    reset_tiles(w);
    ResizeTriangles(w);
    redraw = TRUE;
  }
  else if (w->triangles.wid != c->triangles.wid ||
           w->triangles.ht != c->triangles.ht) {
    ResizeTriangles(w);
    redraw = TRUE;
  }
  if (redraw_tiles && !redraw && XtIsRealized(new) && new->core.visible) {
    draw_frame(c, c->triangles.inverse_GC);
    draw_all_tiles(c, c->triangles.inverse_GC);
    draw_frame(w, w->triangles.puzzle_GC);
    draw_all_tiles(w, w->triangles.tile_GC);
  }
  return (redraw);
}

static void quit_triangles(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  XtCloseDisplay(XtDisplay(w));
  exit(0);
}

static void select_triangles(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  int x, y, sum_x, sum_y, sum_x2, i, j, k;

  sum_x = w->triangles.size * w->triangles.wid + w->triangles.delta.x;
  sum_y = w->triangles.size * w->triangles.ht + w->triangles.delta.y + 1;
  sum_x2 = sum_x / 2;
  x = event->xbutton.x - w->triangles.puzzle_offset.x;
  y = event->xbutton.y - w->triangles.puzzle_offset.y;
  if (x * (sum_y + w->triangles.delta.y) + y * (sum_x2 + 1) <
      (sum_x2 + 1) * (sum_y + w->triangles.delta.y) ||
      x * (sum_y + w->triangles.delta.y) - y * (sum_x2 - 1) >
      (sum_x2 - 1) * (sum_y + w->triangles.delta.y) ||
      y > sum_y - w->triangles.delta.y)
    return;
  k = (y - w->triangles.delta.y) / w->triangles.ht;
  i = (x - sum_x2 - 1  + k * w->triangles.wid / 2) / w->triangles.wid;
  i += ((x - (i + 1) * w->triangles.wid) *
    (sum_y + w->triangles.delta.y) + y * (sum_x2 + 1)) /
    ((sum_x2 + 1) * (sum_y + w->triangles.delta.y));
  j = (-x + sum_x2 - 1  + k * w->triangles.wid / 2) / w->triangles.wid;
  j += 1 + ((-x - (j + 1) * w->triangles.wid) *
    (sum_y + w->triangles.delta.y) + y * (sum_x2 - 1)) /
    ((sum_x2 - 1) * (sum_y + w->triangles.delta.y));
  w->triangles.current_position = k * k + k + i - j;
  w->triangles.current_position_orient = (i + j == k);
  w->triangles.current_row[TRBL] = i;
  w->triangles.current_row[TLBR] = j;
  w->triangles.current_row[ROW] = k;
  if (check_solved(w))
    no_move_tiles(w);
  else
    select_tiles(w);
}

static void randomize_triangles(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  randomize_tiles(w);
}
 
static void maybe_randomize_triangles(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->triangles.started)
    randomize_tiles(w);
}

static void enter_triangles(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;
  char c;
  int i, size, moves;
  trianglesCallbackStruct cb;

  if ((fp = fopen(DATAFILE, "r")) == NULL)
    (void) printf("Can not read %s to enter.\n", DATAFILE);
  else {
    flush_moves(w);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &size);
    if (size >= MINTRIANGLES && size <= MAXTRIANGLES) {
      for (i = w->triangles.size; i < size; i++) {
        cb.reason = TRIANGLES_INC;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
      for (i = w->triangles.size; i > size; i--) {
        cb.reason = TRIANGLES_DEC;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    } else
      (void) printf("%s corrupted: size %d should be between %d and %d\n",
         DATAFILE, size, MINTRIANGLES, MAXTRIANGLES);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &moves);
    scan_start_position(fp, w);
    cb.reason = TRIANGLES_RESTORE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    set_start_position(w);
    scan_moves(fp, w, moves);
    (void) fclose(fp);
    (void) printf("%s: size %d, moves %d.\n", DATAFILE, size, moves);
  }
}

static void write_triangles(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;

  if ((fp = fopen(DATAFILE, "w")) == NULL)
    (void) printf("Can not write to %s.\n", DATAFILE);
  else {
    (void) fprintf(fp, "size: %d\n", w->triangles.size);
    (void) fprintf(fp, "moves: %d\n", num_moves());
    print_start_position(fp, w);
    print_moves(fp);
    (void) fclose(fp);
    (void) printf("Saved to %s.\n", DATAFILE);
  }
}

static void undo_triangles(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (moves_made()) {
    int direction;

    get_move(&direction);
    direction = (direction + (COORD / 2)) % COORD;
    if (move_tiles_dir(w, direction)) {
      trianglesCallbackStruct cb;

      cb.reason = TRIANGLES_UNDO;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}

static void solve_triangles(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  /* solve_tiles(w); */ /* Sorry, unimplemented */
}
 
static void increment_triangles(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  trianglesCallbackStruct cb;
 
  if (w->triangles.size == MAXTRIANGLES)
    return;
  cb.reason = TRIANGLES_INC;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
static void decrement_triangles(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  trianglesCallbackStruct cb;
 
  if (w->triangles.size == MINTRIANGLES)
    return;
  cb.reason = TRIANGLES_DEC;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
static void move_triangles_tl(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_triangles(w, TL);
}

static void move_triangles_tr(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_triangles(w, TR);
}

static void move_triangles_left(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_triangles(w, LEFT);
}

static void move_triangles_right(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_triangles(w, RIGHT);
}

static void move_triangles_bl(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_triangles(w, BL);
}

static void move_triangles_br(w, event, args, n_args)
  TrianglesWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  (void) move_triangles(w, BR);
}

static int move_triangles(w, direction)
  TrianglesWidget w;
  int direction;
{
  trianglesCallbackStruct cb;

  if (check_solved(w)) {
    no_move_tiles(w);
    return FALSE;
  }
  if (!move_triangles_dir(w, direction)) {
    cb.reason = TRIANGLES_BLOCKED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    return FALSE;
  }
  if (check_solved(w)) {
    cb.reason = TRIANGLES_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  return TRUE;
}

int move_triangles_dir(w, direction)
  TrianglesWidget w;
  int direction;
{
  trianglesCallbackStruct cb;

  if (move_tiles_dir(w, direction)) {
    cb.reason = TRIANGLES_MOVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    put_move(direction);
    return TRUE;
  }
  return FALSE;
}

static void select_tiles(w)
  TrianglesWidget w;
{
  trianglesCallbackStruct cb;
  int row_type = -1, l, orient, next;

  /* Are the spaces in a "row" with the mouse click?
  (If two, then one clicked on a space).*/
  for (l = 0; l < ROWTYPES; l++)
    if (w->triangles.current_row[l] == w->triangles.space_row[DOWN][l] &&
        w->triangles.current_row[l] == w->triangles.space_row[UP][l]) {
      if (row_type == -1)
        row_type = l;
      else {
        cb.reason = TRIANGLES_SPACE;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
        return;
      }
    }
  if (row_type >= 0) {
    if (w->triangles.current_position <
        w->triangles.space_position[w->triangles.current_position_orient]) {
      orient = (w->triangles.space_position[UP] + (row_type == TRBL) <
        w->triangles.space_position[DOWN]) ? UP : DOWN;
      while (w->triangles.current_position <
        w->triangles.space_position[w->triangles.current_position_orient]) {
        next = tile_next_to_space(w, row_type, orient, UP);
        orient = !orient;
        move_tiles(w, next, orient);
        cb.reason = TRIANGLES_MOVED;
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
            (void) printf ("select_tiles: row_type %d\n", row_type);
        }
      }
    } else /*w->triangles.current_position >
      w->triangles.space_position[w->triangles.current_position_orient]*/ {
      orient = (w->triangles.space_position[UP] + 2 * (row_type == TRBL) >
        w->triangles.space_position[DOWN]) ? UP : DOWN;
      while (w->triangles.current_position > 
        w->triangles.space_position[w->triangles.current_position_orient]) {
        next = tile_next_to_space(w, row_type, orient, DOWN);
        orient = !orient;
        move_tiles(w, next, orient);
        cb.reason = TRIANGLES_MOVED;
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
            (void) printf ("select_tiles: row_type %d\n", row_type);
        }
      }
    }
  } else {
    cb.reason = TRIANGLES_BLOCKED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    return;
  }
  if (check_solved(w)) {
    cb.reason = TRIANGLES_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void check_tiles(w)
  TrianglesWidget w;
{
  char buf[121];

  if (w->triangles.size < MINTRIANGLES || w->triangles.size > MAXTRIANGLES) {
    (void) sprintf(buf,
             "Number of Triangles on a edge out of bounds, use %d..%d",
             MINTRIANGLES, MAXTRIANGLES);
    XtWarning(buf);
    w->triangles.size = DEFAULTTRIANGLES;
  }
}

static void reset_tiles(w)
  TrianglesWidget w;
{
  int i;

  w->triangles.size_size = w->triangles.size * w->triangles.size;
  w->triangles.space_position[UP] = w->triangles.size_size - 1;
  w->triangles.space_row[UP][TRBL] = w->triangles.size - 1; /*i*/
  w->triangles.space_row[UP][TLBR] = 0; /*j*/
  w->triangles.space_row[UP][ROW] = w->triangles.size - 1; /*k*/
  if (w->triangles.size > 1) {
    w->triangles.space_position[DOWN] = w->triangles.size_size - 2;
    w->triangles.space_row[DOWN][TRBL] = w->triangles.size - 2; /*i*/
    w->triangles.space_row[DOWN][TLBR] = 0; /*j*/
    w->triangles.space_row[DOWN][ROW] = w->triangles.size - 1; /*k*/
    w->triangles.tile_of_position[w->triangles.size_size - 2] = -1;
  }
  w->triangles.tile_of_position[w->triangles.size_size - 1] = 0;
  for (i = 1; i < w->triangles.size_size - 1; i++)
    w->triangles.tile_of_position[i - 1] = i;
  flush_moves(w);
  w->triangles.started = FALSE;
}

static void resize_tiles(w)
  TrianglesWidget w;
{
  int i, j;

  for (j = 0; j < MAXORIENT; j++)
    for (i = 0; i < 3; i++) { 
      triangle_list[j][i].x = (w->triangles.tile_width / 2) *
        triangle_unit[j][i].x;
      triangle_list[j][i].y = w->triangles.tile_height *
        triangle_unit[j][i].y;      
    }
  w->triangles.digit_offset.x = 3;
  w->triangles.digit_offset.y = 2;
}

static void no_move_tiles(w)
  TrianglesWidget w;
{
  trianglesCallbackStruct cb;
 
  cb.reason = TRIANGLES_IGNORE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static int move_tiles_dir(w, direction)
  TrianglesWidget w;
  int direction;
{
  int orient;

  switch (direction) {
    case TR:
      if (w->triangles.space_row[UP][TRBL] == w->triangles.space_row[DOWN][TRBL]
          && w->triangles.space_row[UP][ROW] != w->triangles.size - 1) {
        orient = (w->triangles.space_position[UP] + 2 >
          w->triangles.space_position[DOWN]) ? UP : DOWN;
        move_tiles(w, tile_next_to_space(w, TRBL, orient, DOWN), !orient);
        return TRUE;
      }
      break;
    case RIGHT:
      if (w->triangles.space_row[UP][ROW] == w->triangles.space_row[DOWN][ROW]
          && w->triangles.space_row[UP][TRBL] != 0) {
        orient = (w->triangles.space_position[UP] <
          w->triangles.space_position[DOWN]) ? UP : DOWN;
        move_tiles(w, tile_next_to_space(w, ROW, orient, UP), !orient);
        return TRUE;
      }
      break;
    case BR:
      if (w->triangles.space_row[UP][TLBR] == w->triangles.space_row[DOWN][TLBR]
          && w->triangles.space_row[UP][TRBL] != 0) {
        orient = (w->triangles.space_position[UP] <
          w->triangles.space_position[DOWN]) ? UP : DOWN;
        move_tiles(w, tile_next_to_space(w, TLBR, orient, UP), !orient);
        return TRUE;
      }
      break;
    case BL:
      if (w->triangles.space_row[UP][TRBL] == w->triangles.space_row[DOWN][TRBL]
          && w->triangles.space_row[UP][TLBR] != 0) {
        orient = (w->triangles.space_position[UP] + 1 <
          w->triangles.space_position[DOWN]) ? UP : DOWN;
        move_tiles(w, tile_next_to_space(w, TRBL, orient, UP), !orient);
        return TRUE;
      }
      break;
    case LEFT:
      if (w->triangles.space_row[UP][ROW] == w->triangles.space_row[DOWN][ROW]
          && w->triangles.space_row[UP][TLBR] != 0) {
        orient = (w->triangles.space_position[UP] >
          w->triangles.space_position[DOWN]) ? UP : DOWN;
        move_tiles(w, tile_next_to_space(w, ROW, orient, DOWN), !orient);
        return TRUE;
      }
      break;
    case TL:
      if (w->triangles.space_row[UP][TLBR] == w->triangles.space_row[DOWN][TLBR]
          && w->triangles.space_row[UP][ROW] != w->triangles.size - 1) {
        orient = (w->triangles.space_position[UP] >
          w->triangles.space_position[DOWN]) ? UP : DOWN;
        move_tiles(w, tile_next_to_space(w, TLBR, orient, DOWN), !orient);
        return TRUE;
      }
      break;
    default:
      (void) printf ("move_tiles_dir: direction %d\n", direction);
  }
  return FALSE;
}
 
static void randomize_tiles(w)
  TrianglesWidget w;
{
  trianglesCallbackStruct cb;

  /* First interchange tiles but only with other tiles of the same
     orientation */
  if (w->triangles.size > 2) {
    int current_pos, random_pos, random_row;
    int current_orient = UP, random_orient = DOWN;
    int step = 1, fin = 1;

    for (current_pos = 0; current_pos < w->triangles.size_size; current_pos++) {
      random_pos = current_pos;
      while (current_pos == random_pos || current_orient != random_orient) {
        random_pos = NRAND(w->triangles.size_size);
        random_row = row(random_pos);
        random_orient = !((random_row + trbl(random_pos, random_row) / 2 +
          tlbr(random_pos, random_row) / 2) % 2);
      }
      (void) exchange_tiles(w, current_pos, random_pos);
      if (fin == current_pos + 1) {
        current_orient = UP;
        step += 2;
        fin += step;
      } else
        current_orient = !current_orient;
    }
    draw_all_tiles(w, w->triangles.tile_GC);
  }
  /* Now move the spaces around randomly */
  if (w->triangles.size > 1) {
    int big = w->triangles.size_size + NRAND(2);
    int last_direction = 0;
    int random_direction;

    cb.reason = TRIANGLES_RESET;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
 
    if (w->triangles.size == 2)
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
        if (move_triangles_dir(w, random_direction))
          last_direction = random_direction;
        else
          big++;
      }
    }
    flush_moves(w);
    cb.reason = TRIANGLES_RANDOMIZE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  if (check_solved(w)) {
    cb.reason = TRIANGLES_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void move_tiles(w, from, orient)
  TrianglesWidget w;
  int from, orient;
{
  int temp_tile;

  temp_tile = w->triangles.tile_of_position[from];
  w->triangles.tile_of_position[from] =
    w->triangles.tile_of_position[w->triangles.space_position[orient]];
  w->triangles.tile_of_position[w->triangles.space_position[orient]] =
    temp_tile;
  draw_tile(w, w->triangles.tile_GC,
    w->triangles.space_position[orient], orient);
  w->triangles.space_position[orient] = from;
  w->triangles.space_row[orient][ROW] = row(from);
  w->triangles.space_row[orient][TRBL] =
    trbl(from, w->triangles.space_row[orient][ROW]) / 2;
  w->triangles.space_row[orient][TLBR] =
    tlbr(from, w->triangles.space_row[orient][ROW]) / 2;
  draw_tile(w, w->triangles.inverse_GC,
    w->triangles.space_position[orient], orient);
}

static int exchange_tiles(w, pos1, pos2)
  TrianglesWidget w;
  int pos1, pos2;
{
  int temp_tile;

  if (w->triangles.tile_of_position[pos1] <= 0)
    return FALSE;
  else if (w->triangles.tile_of_position[pos2] <= 0)
    return FALSE;
  temp_tile = w->triangles.tile_of_position[pos1];
  w->triangles.tile_of_position[pos1] = w->triangles.tile_of_position[pos2];
  w->triangles.tile_of_position[pos2] = temp_tile;
  return TRUE;
}

static int tile_next_to_space(w, row_type, orient, direction)
  TrianglesWidget w;
  int row_type, orient, direction;
{
  if (direction == UP) {
    if (row_type == TRBL)
      return ((orient == UP) ? w->triangles.space_position[orient] + 1 : 
              w->triangles.space_position[orient] -
              2 * w->triangles.space_row[orient][ROW]);
    else if (row_type == TLBR)
      return ((orient == UP) ? w->triangles.space_position[orient] - 1 : 
              w->triangles.space_position[orient] -
              2 * w->triangles.space_row[orient][ROW]);
    else /* row_type == ROW */
      return (w->triangles.space_position[orient] - 1);
  } else /* direction == DOWN */ {
    if (row_type == TRBL)
      return ((orient == DOWN) ? w->triangles.space_position[orient] - 1 : 
              w->triangles.space_position[orient] +
              2 * (w->triangles.space_row[orient][ROW] + 1));
    else if (row_type == TLBR)
      return ((orient == DOWN) ? w->triangles.space_position[orient] + 1 : 
              w->triangles.space_position[orient] +
              2 * (w->triangles.space_row[orient][ROW] + 1));
    else /* row_type == ROW */
      return (w->triangles.space_position[orient] + 1);
  }
}

static void draw_frame(w, gc)
  TrianglesWidget w;
  GC gc;
{
  int sum_x, sum_y, sum_x2, offset_x, offset_y;

  sum_x = w->triangles.size * w->triangles.wid + w->triangles.delta.x + 1;
  sum_y = w->triangles.size * w->triangles.ht + w->triangles.delta.y + 1;
  offset_x = w->triangles.puzzle_offset.x;
  offset_y = w->triangles.puzzle_offset.y;
  sum_x2 = sum_x / 2 + offset_x;
  sum_x += offset_x;
  sum_y += offset_y;
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    sum_x2 - 1, offset_y, offset_x, sum_y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    sum_x2 - 2, offset_y, sum_x - 2, sum_y);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    offset_x, sum_y, sum_x - 2, sum_y);
}   

void draw_all_tiles(w, gc)
  TrianglesWidget w;
  GC gc;
{
  int k, side = UP, fin = 1, step = 1;

  for (k = 0; k < w->triangles.size_size; k++) {
    draw_tile(w, (w->triangles.tile_of_position[k] <= 0) ?
       w->triangles.inverse_GC : gc, k, side);
    if (fin == k + 1) {
      side = UP;
      step += 2;
      fin += step;
    } else
      side = !side;
  }
}

static void draw_tile(w, gc, pos, orient)
  TrianglesWidget w;
  GC gc;
  int pos, orient;
{
  int dx, dy, k = row(pos);

  dy = (orient == UP) ? 0 : w->triangles.tile_height;
  dy += k * w->triangles.ht + w->triangles.delta.y + 2 +
    w->triangles.puzzle_offset.y;
  dx = (trbl(pos, k) - k + w->triangles.size) * w->triangles.wid / 2 +
    w->triangles.delta.x / 2 + w->triangles.puzzle_offset.x;
  triangle_list[orient][0].x = dx;
  triangle_list[orient][0].y = dy;
  XFillPolygon(XtDisplay(w), XtWindow(w), gc, triangle_list[orient], 3,
    Convex, CoordModePrevious);
  if (gc != w->triangles.inverse_GC) {
    int i = 0, offset_x = 0, offset_y = 0;
    int tile = w->triangles.tile_of_position[pos];
    char buf[5];

    (void) sprintf(buf, "%d", tile);
    while (tile >= 1) {
      tile /= w->triangles.base;
      offset_x += w->triangles.digit_offset.x;
      i++;
    }
    offset_y = (orient == UP) ? w->triangles.digit_offset.y +
      2 * w->triangles.delta.y + 2 :
      -w->triangles.digit_offset.y - w->triangles.tile_height +
      w->triangles.delta.y - 2;
    XDrawString(XtDisplay(w), XtWindow(w), w->triangles.inverse_GC,
      dx - offset_x, dy + w->triangles.tile_height / 2 + offset_y, buf, i);
  }
}

int row(pos)
  int pos;
{
  return Sqrt(pos);
}

/* Passing row so there is no sqrt calculation again */
int trbl(pos, posrow)
  int pos, posrow;
{
  return (pos - posrow * posrow);
}

int tlbr(pos, posrow)
 int pos, posrow;
{
  return (posrow * posrow + 2 * posrow - pos);
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
  TrianglesWidget w;
{
  int i;

  for (i = 1; i < w->triangles.size_size - 1; i++)
    if (w->triangles.tile_of_position[i - 1] != i)
      return FALSE;
  return TRUE;
}
