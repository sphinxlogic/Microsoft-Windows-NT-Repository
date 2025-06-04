/*
# X-BASED MASTERBALL(tm)
#
#  Mball.c
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

/* Methods file for Mball */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef VMS
#define M_PI 3.14159265358979323846
#endif
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
#include "MballP.h"

#ifndef DATAFILE
#define DATAFILE "/usr/games/lib/mball.data"
#endif

static void InitializeMball();
static void ExposeMball();
static void ResizeMball();
static void DestroyMball();
static Boolean SetValuesMball();
static void quit_mball();
static void practice_mball();
static void maybe_practice_mball();
static void randomize_mball();
static void maybe_randomize_mball();
static void enter_mball();
static void write_mball();
static void undo_mball();
static void solve_mball();
static void increment_mball();
static void decrement_mball();
static void orientize_mball();
static void wedge2_mode_mball();
static void wedge4_mode_mball();
static void wedge6_mode_mball();
static void wedge8_mode_mball();
static void move_mball_tl();
static void move_mball_top();
static void move_mball_tr();
static void move_mball_left();
static void move_mball_cw();
static void move_mball_right();
static void move_mball_bl();
static void move_mball_bottom();
static void move_mball_br();
static void move_mball_ccw();
static void move_mball_input();
static void get_color();
static void move_control_cb();
static void check_wedges();
static void reset_wedges();
static void resize_wedges();
static int position_wedges();
static void no_move_wedges();
static void practice_wedges();
static void randomize_wedges();
static void move_wedges();
/* rcd : row, column, or diagonal */
static void swap_wedges();
static void draw_frame();
static void draw_wedge();
static void draw_sector();
static void draw_radar();
static void draw_sect();
static void letter_position();
static void offset_sect();
static void XFillSector();

#ifdef DEBUG
static void print_mball();
static void print_wedge();
static void print_row();
#endif

static char defaultTranslationsMball[] =
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
   <KeyPress>o: orientize()\n\
   <KeyPress>2: wedge2()\n\
   <KeyPress>4: wedge4()\n\
   <KeyPress>6: wedge6()\n\
   <KeyPress>8: wedge8()";
 
static XtActionsRec actionsListMball[] =
{
  {"quit", (XtActionProc) quit_mball},
  {"move_tl", (XtActionProc) move_mball_tl},
  {"move_top", (XtActionProc) move_mball_top},
  {"move_tr", (XtActionProc) move_mball_tr},
  {"move_left", (XtActionProc) move_mball_left},
  {"move_cw", (XtActionProc) move_mball_cw},
  {"move_right", (XtActionProc) move_mball_right},
  {"move_bl", (XtActionProc) move_mball_bl},
  {"move_bottom", (XtActionProc) move_mball_bottom},
  {"move_br", (XtActionProc) move_mball_br},
  {"move_ccw", (XtActionProc) move_mball_ccw},
  {"practice", (XtActionProc) practice_mball},
  {"maybe_practice", (XtActionProc) maybe_practice_mball},
  {"randomize", (XtActionProc) randomize_mball},
  {"maybe_randomize", (XtActionProc) maybe_randomize_mball},
  {"enter", (XtActionProc) enter_mball},
  {"write", (XtActionProc) write_mball},
  {"undo", (XtActionProc) undo_mball},
  {"solve", (XtActionProc) solve_mball},
  {"increment", (XtActionProc) increment_mball},
  {"decrement", (XtActionProc) decrement_mball},
  {"orientize", (XtActionProc) orientize_mball},
  {"wedge2", (XtActionProc) wedge2_mode_mball},
  {"wedge4", (XtActionProc) wedge4_mode_mball},
  {"wedge6", (XtActionProc) wedge6_mode_mball},
  {"wedge8", (XtActionProc) wedge8_mode_mball}
};

static XtResource resourcesMball[] =
{
  {XtNwedgeColor0, XtCLabel, XtRString, sizeof(String),
   XtOffset(MballWidget, mball.wedge_name[0]), XtRString, "Yellow"},
  {XtNwedgeColor1, XtCLabel, XtRString, sizeof(String),
   XtOffset(MballWidget, mball.wedge_name[1]), XtRString, "Blue"},
  {XtNwedgeColor2, XtCLabel, XtRString, sizeof(String),
   XtOffset(MballWidget, mball.wedge_name[2]), XtRString, "Red"},
  {XtNwedgeColor3, XtCLabel, XtRString, sizeof(String),
   XtOffset(MballWidget, mball.wedge_name[3]), XtRString, "Magenta"},
  {XtNwedgeColor4, XtCLabel, XtRString, sizeof(String),
   XtOffset(MballWidget, mball.wedge_name[4]), XtRString, "Green"},
  {XtNwedgeColor5, XtCLabel, XtRString, sizeof(String),
   XtOffset(MballWidget, mball.wedge_name[5]), XtRString, "Orange"},
  {XtNwedgeColor6, XtCLabel, XtRString, sizeof(String),
   XtOffset(MballWidget, mball.wedge_name[6]), XtRString, "Cyan"},
  {XtNwedgeColor7, XtCLabel, XtRString, sizeof(String),
   XtOffset(MballWidget, mball.wedge_name[7]), XtRString, "DarkGreen"},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
   XtOffset(MballWidget, mball.foreground), XtRString, XtDefaultForeground},
  {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
   XtOffset(MballWidget, core.width), XtRString, "200"},
  {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
   XtOffset(MballWidget, core.height), XtRString, "400"},
  {XtNwedges, XtCWedges, XtRInt, sizeof(int),
   XtOffset(MballWidget, mball.wedges), XtRString, "8"}, /*DEFAULTWEDGES*/
  {XtNrings, XtCRings, XtRInt, sizeof(int),
   XtOffset(MballWidget, mball.rings), XtRString, "4"}, /*DEFAULTRINGS*/
  {XtNorient, XtCOrient, XtRBoolean, sizeof(Boolean),
   XtOffset(MballWidget, mball.orient), XtRString, "FALSE"}, /*DEFAULTORIENT*/
  {XtNmono, XtCMono, XtRBoolean, sizeof(Boolean),
   XtOffset(MballWidget, mball.mono), XtRString, "FALSE"},
  {XtNpractice, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(MballWidget, mball.practice), XtRString, "FALSE"},
  {XtNstart, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(MballWidget, mball.started), XtRString, "FALSE"},
  {XtNselectCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
   XtOffset(MballWidget, mball.select), XtRCallback, NULL}
};

MballClassRec mballClassRec =
{
  {
    (WidgetClass) &widgetClassRec,	/* superclass */
    "Mball",				/* class name */
    sizeof(MballRec),			/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeMball,			/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    actionsListMball,			/* actions */
    XtNumber(actionsListMball),		/* num actions */
    resourcesMball,			/* resources */
    XtNumber(resourcesMball),		/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    DestroyMball,			/* destroy */
    ResizeMball,			/* resize */
    ExposeMball,			/* expose */
    SetValuesMball,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    NULL,				/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    defaultTranslationsMball,		/* tm table */
    NULL,				/* query geometry */
    NULL,				/* display accelerator */
    NULL				/* extension */
  },
  {
    0					/* ignore */
  }
};

static int map_key_dir[(MAXWEDGES - MINWEDGES) / 2 + 1][COORD] = 
{
  { 0, 4, 0, 4,  4, 4, 4, 4},
  { 0, 1, 0, 1,  4, 4, 4, 4},
  { 0, 4, 0, 4,  1, 2, 1, 2},
  { 0, 2, 0, 2,  1, 3, 1, 3}
};

WidgetClass mballWidgetClass = (WidgetClass) &mballClassRec;

static void InitializeMball(request, new)
  Widget request, new;
{
  MballWidget w = (MballWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  int wedge;
  
  check_wedges(w);
  init_moves();
  reset_wedges(w);
  w->mball.practice = FALSE;
  (void) SRAND(getpid());
  valueMask = GCForeground | GCBackground;
  values.foreground = w->mball.foreground;
  values.background = w->core.background_pixel;
  w->mball.puzzle_GC = XtGetGC(new, valueMask, &values);
  w->mball.depth = DefaultDepthOfScreen(XtScreen(w));
  valueMask = GCForeground | GCBackground;
  values.foreground = w->core.background_pixel;
  values.background = w->mball.foreground;
  w->mball.inverse_GC = XtGetGC(new, valueMask, &values);
  for (wedge = 0; wedge < MAXWEDGES; wedge++)
    get_color(w, wedge, TRUE);
  ResizeMball(w);
}

static void DestroyMball(old)
  Widget old;
{
  MballWidget w = (MballWidget) old;
  int wedge;

  for (wedge = 0; wedge < MAXWEDGES; wedge++)
    XtReleaseGC(old, w->mball.wedge_GC[wedge]);
  XtReleaseGC(old, w->mball.puzzle_GC);
  XtReleaseGC(old, w->mball.inverse_GC);
  XtRemoveCallbacks(old, XtNselectCallback, w->mball.select);
}

static void ResizeMball(w)
  MballWidget w;
{
  int temp_length;

  w->mball.delta = 4;
  w->mball.vertical = (w->core.height >= w->core.width);
  if (w->mball.vertical)
    temp_length = MIN(w->core.height / 2, w->core.width);
  else
    temp_length = MIN(w->core.height, w->core.width / 2);
  w->mball.mball_length = MAX((temp_length - w->mball.delta + 1) /
    w->mball.wedges, 0);
  w->mball.wedge_length = w->mball.wedges * w->mball.mball_length;
  w->mball.view_length = w->mball.wedge_length + w->mball.delta;
  w->mball.view_middle = w->mball.view_length / 2;
  if (w->mball.vertical) {
    w->mball.puzzle_width = w->mball.view_length - 1;
    w->mball.puzzle_height = 2 * w->mball.view_length - w->mball.delta - 2;
  } else {
    w->mball.puzzle_width = 2 * w->mball.view_length - w->mball.delta - 2;
    w->mball.puzzle_height = w->mball.view_length - 1;
  }
  w->mball.puzzle_offset.x = ((int) w->core.width - w->mball.puzzle_width) / 2;
  w->mball.puzzle_offset.y = ((int) w->core.height - w->mball.puzzle_height) / 2;
  resize_wedges(w);
}

static void ExposeMball(new, event, region)
  Widget new;
  XEvent *event;
  Region region; /* Not used */
{
  MballWidget w = (MballWidget) new;

  if (w->core.visible) {
    draw_frame(w, w->mball.puzzle_GC);
    draw_all_wedges(w);
  }
}

static Boolean SetValuesMball(current, request, new)
  Widget current, request, new;
{
  MballWidget c = (MballWidget) current, w = (MballWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  Boolean redraw = FALSE;
  int wedge;

  check_wedges(w);
  if (w->mball.foreground != c->mball.foreground) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->mball.foreground;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->mball.puzzle_GC);
    w->mball.puzzle_GC = XtGetGC(new, valueMask, &values);
    if (w->mball.mono || w->mball.depth == 1) {
      values.background = w->core.background_pixel;
      values.foreground = w->mball.foreground;
      for (wedge = 0; wedge < MAXWEDGES; wedge++) {
        XtReleaseGC(new, w->mball.wedge_GC[wedge]);
        w->mball.wedge_GC[wedge] = XtGetGC(new, valueMask, &values);
      }
      c->mball.mono = w->mball.mono;
    }
    redraw = TRUE;
  }
  if (w->core.background_pixel != c->core.background_pixel) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->core.background_pixel;
    values.background = w->mball.foreground;
    XtReleaseGC(new, w->mball.inverse_GC);
    w->mball.inverse_GC = XtGetGC(new, valueMask, &values);
    redraw = TRUE;
  }
  for (wedge = 0; wedge < MAXWEDGES; wedge++) {
    if (strcmp(w->mball.wedge_name[wedge], c->mball.wedge_name[wedge]))
      get_color(w, wedge, FALSE);
  }
  if (w->mball.orient != c->mball.orient) {
    reset_wedges(w);
    w->mball.practice = FALSE;
    redraw = TRUE;
  } else if (w->mball.practice != c->mball.practice) {
    reset_wedges(w);
    redraw = TRUE;
  }
  if (w->mball.wedges != c->mball.wedges ||
      w->mball.rings != c->mball.rings) {
    reset_wedges(w);
    w->mball.practice = FALSE;
    ResizeMball(w);
    redraw = TRUE;
  }
  if (w->mball.mball_length != c->mball.mball_length) {
    ResizeMball(w);
    redraw = TRUE;
  }
  return (redraw);
}

static void quit_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  XtCloseDisplay(XtDisplay(w));
  exit(0);
}

static void practice_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  practice_wedges(w);
}
 
static void maybe_practice_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->mball.started)
    practice_wedges(w);
}
 
static void randomize_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  randomize_wedges(w);
}
 
static void maybe_randomize_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->mball.started)
    randomize_wedges(w);
}

static void enter_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;
  char c;
  int i, wedge, ring, orient, practice, moves;
  mballCallbackStruct cb;

  if ((fp = fopen(DATAFILE, "r")) == NULL)
    (void) printf("Can not read %s to enter.\n", DATAFILE);
  else {
    flush_moves(w);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &wedge);
    if (wedge >= MINWEDGES && wedge <= MAXWEDGES && !(wedge % 2)) {
      if (w->mball.wedges != wedge) {
        cb.reason = (wedge - MINWEDGES) / 2 + MBALL_WEDGE2;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    } else
      (void) printf("%s corrupted: ", DATAFILE);
      (void) printf("wedge %d should be even and between %d and %d\n",
         wedge, MINWEDGES, MAXWEDGES);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &ring);
    if (ring >= MINRINGS && ring <= MAXRINGS) {
      for (i = w->mball.rings; i < ring; i++) {
        cb.reason = MBALL_INC;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
      for (i = w->mball.rings; i > ring; i--) {
        cb.reason = MBALL_DEC;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    } else
      (void) printf("%s corrupted: ring %d should be between %d and %d\n",
         DATAFILE, ring, MINRINGS, MAXRINGS);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &orient);
    if (w->mball.orient != (Boolean) orient) {
      cb.reason = MBALL_ORIENT;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &practice);
    if (w->mball.practice != (Boolean) practice) {
      cb.reason = MBALL_PRACTICE;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &moves);
    scan_start_position(fp, w);
    cb.reason = MBALL_RESTORE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    set_start_position(w);
    scan_moves(fp, w, moves);
    (void) fclose(fp);
    (void) printf("%s: wedge %d, ring %d, orient %d, ",
      DATAFILE, wedge, ring, orient);
    (void) printf("practice %d, moves %d", practice, moves);
  }
}

static void write_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;

  if ((fp = fopen(DATAFILE, "w")) == NULL)
    (void) printf("Can not write to %s.\n", DATAFILE);
  else {
    (void) fprintf(fp, "wedge: %d\n", w->mball.wedges);
    (void) fprintf(fp, "ring: %d\n", w->mball.rings);
    (void) fprintf(fp, "orient: %d\n", (w->mball.orient) ? 1 : 0);
    (void) fprintf(fp, "practice: %d\n", (w->mball.practice) ? 1 : 0);
    (void) fprintf(fp, "moves: %d\n", num_moves());
    print_start_position(fp, w);
    print_moves(fp);
    (void) fclose(fp);
    (void) printf("Saved to %s.\n", DATAFILE);
  }
}

static void undo_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (moves_made()) {
    int wedge, ring, direction, control;

    get_move(&wedge, &ring, &direction, &control);
    direction = (direction < COORD) ? ((direction < CUTS) ?
      (direction + CUTS / 2) % CUTS :
      (direction + CUTS / 2) % CUTS + CUTS) : 3 * COORD - direction;
    if (control)
      move_control_cb(w, wedge, direction);
    else {
      mballCallbackStruct cb;

      move_wedges(w, wedge, ring, direction);
      cb.reason = MBALL_UNDO;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}
 
static void solve_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  /* solve_wedges(w); */ /* Sorry, unimplemented */
}
 
static void increment_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  mballCallbackStruct cb;
 
  if (w->mball.rings == MAXRINGS)
    return;
  cb.reason = MBALL_INC;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
static void decrement_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  mballCallbackStruct cb;
 
  if (w->mball.rings == MINRINGS)
    return;
  cb.reason = MBALL_DEC;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
static void orientize_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  mballCallbackStruct cb;
 
  cb.reason = MBALL_ORIENT;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void wedge2_mode_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  mballCallbackStruct cb;
 
  cb.reason = MBALL_WEDGE2;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void wedge4_mode_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  mballCallbackStruct cb;
 
  cb.reason = MBALL_WEDGE4;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void wedge6_mode_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  mballCallbackStruct cb;
 
  cb.reason = MBALL_WEDGE6;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void wedge8_mode_mball(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  mballCallbackStruct cb;
 
  cb.reason = MBALL_WEDGE8;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void move_mball_ccw(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mball_input(w, event->xbutton.x, event->xbutton.y, CCW,
    (int) (event->xbutton.state & ControlMask));
}

static void move_mball_tl(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mball_input(w, event->xbutton.x, event->xbutton.y, TL,
    (int) (event->xkey.state & ControlMask));
}

static void move_mball_top(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mball_input(w, event->xbutton.x, event->xbutton.y, TOP,
    (int) (event->xkey.state & ControlMask));
}

static void move_mball_tr(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mball_input(w, event->xbutton.x, event->xbutton.y, TR,
    (int) (event->xkey.state & ControlMask));
}

static void move_mball_left(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mball_input(w, event->xbutton.x, event->xbutton.y, LEFT,
    (int) (event->xkey.state & ControlMask));
}

static void move_mball_cw(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mball_input(w, event->xbutton.x, event->xbutton.y, CW,
    (int) (event->xkey.state & ControlMask));
}

static void move_mball_right(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mball_input(w, event->xbutton.x, event->xbutton.y, RIGHT,
    (int) (event->xkey.state & ControlMask));
}

static void move_mball_bl(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mball_input(w, event->xbutton.x, event->xbutton.y, BL,
    (int) (event->xkey.state & ControlMask));
}

static void move_mball_bottom(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mball_input(w, event->xbutton.x, event->xbutton.y, BOTTOM,
    (int) (event->xkey.state & ControlMask));
}

static void move_mball_br(w, event, args, n_args)
  MballWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_mball_input(w, event->xbutton.x, event->xbutton.y, BR,
    (int) (event->xkey.state & ControlMask));
}

static void move_mball_input(w, x, y, direction, control)
  MballWidget w;
  int x, y, direction, control;
{
  int wedge, ring;
 
  if (check_solved(w) && !w->mball.practice && !control) {
    no_move_wedges(w);
    return;
  }
  if (!position_wedges(w, x, y, &wedge, &ring, &direction))
    return;
  control = (control) ? 1 : 0;
  move_mball(w, wedge, ring, direction, control);
  if (!control && check_solved(w)) {
    mballCallbackStruct cb;
 
    cb.reason = MBALL_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

void move_mball(w, wedge, ring, direction, control)
  MballWidget w;
  int wedge, ring, direction, control;
{
  mballCallbackStruct cb;

  if (control)
    move_control_cb(w, wedge, direction);
  else {
    move_wedges(w, wedge, ring, direction);
    cb.reason = MBALL_MOVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  put_move(wedge, ring, direction, control);
}

static void get_color(w, wedge, init)
  MballWidget w;
  int wedge, init;
{
  XGCValues values;
  XtGCMask valueMask;
  XColor color_cell, rgb;
 
  valueMask = GCForeground | GCBackground;
  values.background = w->core.background_pixel;
  if (w->mball.depth > 1 && !w->mball.mono) {
    if (XAllocNamedColor(XtDisplay(w),
        DefaultColormap(XtDisplay(w), XtWindow(w)),
        w->mball.wedge_name[wedge], &color_cell, &rgb)) {
      values.foreground = w->mball.wedge_color[wedge] = color_cell.pixel;
      if (!init)
        XtReleaseGC((Widget) w, w->mball.wedge_GC[wedge]);
      w->mball.wedge_GC[wedge] = XtGetGC((Widget) w, valueMask, &values);
      return;
    } else {
      char buf[121];
 
      (void) sprintf(buf, "Color name \"%s\" is not defined %d",
               w->mball.wedge_name[wedge], wedge);
      XtWarning(buf);
    }
  }
  values.foreground = w->mball.foreground;
  if (!init)
    XtReleaseGC((Widget) w, w->mball.wedge_GC[wedge]);
  w->mball.wedge_GC[wedge] = XtGetGC((Widget) w, valueMask, &values);
}

static void move_control_cb(w, wedge, direction)
  MballWidget w;
  int wedge, direction;
{
  mballCallbackStruct cb;
  int ring;

  if (direction > COORD)
    for (ring = 0; ring < w->mball.rings; ring++) {
      move_wedges(w, wedge, ring, direction);
      cb.reason = MBALL_CONTROL;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  else {
    move_wedges(w, 0, 0, direction);
    cb.reason = MBALL_CONTROL;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    move_wedges(w, w->mball.wedges / 2, 0, direction);
    cb.reason = MBALL_CONTROL;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void check_wedges(w)
  MballWidget w;
{
  if (w->mball.wedges < MINWEDGES || w->mball.wedges > MAXWEDGES ||
      w->mball.wedges % 2) {
    char buf[121];

    (void) sprintf(buf,
      "Number of wedges out of bounds, use even %d..%d", MINWEDGES, MAXWEDGES);
    XtWarning(buf);
    w->mball.wedges = DEFAULTWEDGES;
  }
  if (w->mball.rings < MINRINGS || w->mball.rings > MAXRINGS) {
    char buf[121];

    (void) sprintf(buf,
      "Number of rings out of bounds, use %d..%d", MINRINGS, MAXRINGS);
    XtWarning(buf);
    w->mball.rings = DEFAULTRINGS;
  }
}

static void reset_wedges(w)
  MballWidget w;
{
  int wedge, ring;

  for (wedge = 0; wedge < w->mball.wedges; wedge++)
    for (ring = 0; ring < w->mball.rings; ring++) {
      w->mball.mball_loc[wedge][ring].wedge = wedge;
      w->mball.mball_loc[wedge][ring].direction = DOWN; 
    }
  flush_moves(w);
  w->mball.started = FALSE;
}

static void resize_wedges(w)
  MballWidget w;
{
  w->mball.mball_length = w->mball.wedge_length / (2 * w->mball.wedges) -
    w->mball.delta - 1;
  w->mball.letter_offset.x = -2;
  w->mball.letter_offset.y = 4;
  w->mball.dr = w->mball.wedges;
}

static int position_wedges(w, x, y, wedge, ring, direction)
  MballWidget w;
  int x, y;
  int *wedge, *ring, *direction;
{
  int view;
  double angle, radius;

  x -= w->mball.puzzle_offset.x;
  y -= w->mball.puzzle_offset.y;
  if (w->mball.vertical && y > w->mball.view_length - 1) {
    y -= (w->mball.view_length - 1);
    view = DOWN;
  } else if (!w->mball.vertical && x > w->mball.view_length - 1) {
    x -= (w->mball.view_length - 1);
    view = DOWN;
  } else
    view = UP;
  x -= (w->mball.wedge_length + 1) / 2;
  y -= (w->mball.wedge_length + 1) / 2;
  radius = sqrt((double) x * x + y * y);
  if ((*direction == CW || *direction == CCW) &&
      radius > w->mball.wedge_length / 2 + w->mball.delta)
    return FALSE;
  radius = sqrt((double) x * x + y * y);
  if (y >= 0)
    angle = atan2((double) -x, (double) y) + M_PI;
  else if (x < 0)
    angle = 2 * M_PI - atan2((double) -x, (double) -y);
  else
    angle = -atan2((double) -x, (double) -y);
  *ring = (int) (radius * (double) w->mball.rings /
    ((double) w->mball.wedge_length));
  *wedge = (int) (angle * (double) w->mball.wedges / (2.0 * M_PI));
  if (view == DOWN) {
    if (*direction == CCW)
      *direction = CW;
    else if (*direction == CW)
      *direction = CCW;
    else if (w->mball.vertical) {
      if (*direction < CUTS)
        *direction = (*direction + CUTS / 2) % CUTS;
      else if (*direction < COORD)
        *direction = (COORD + 1 - *direction) % CUTS + CUTS;
    } else {
      if (*direction == LEFT)
        *direction = RIGHT;
      else if (*direction == RIGHT)
        *direction = LEFT;
      else if (*direction >= CUTS && *direction < COORD)
        *direction = (COORD - 1 - *direction) % CUTS + CUTS;
    }
    if (w->mball.vertical)
      *wedge = (3 * w->mball.wedges / 2 - 1 - *wedge) % w->mball.wedges;
    else
      *wedge = (w->mball.wedges - 1 - *wedge) % w->mball.wedges;
    *ring = w->mball.rings - 1 - *ring;
  }
  if (w->mball.wedges % 4 && (*direction == LEFT || *direction == RIGHT))
    return FALSE;
  if (w->mball.wedges <= 4 && *direction > CUTS && *direction < COORD)
    return FALSE;
  return TRUE;
}

static void no_move_wedges(w)
  MballWidget w;
{
  mballCallbackStruct cb;
 
  cb.reason = MBALL_IGNORE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void practice_wedges(w)
  MballWidget w;
{
  mballCallbackStruct cb;

  cb.reason = MBALL_PRACTICE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void randomize_wedges(w)
  MballWidget w;
{
  mballCallbackStruct cb;
  int random_direction, wedge, ring;
  int big = w->mball.wedges * (w->mball.rings + 1) + NRAND(2);

  if (w->mball.practice)
    practice_wedges(w);
  cb.reason = MBALL_RESET;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
 
#ifdef DEBUG
  big = 3;
#endif

  while (big--) {
    wedge = NRAND(w->mball.wedges);
    ring = NRAND(w->mball.rings);
    do
      random_direction = NRAND(2 * COORD);
    while (random_direction < COORD &&
           map_key_dir[(w->mball.wedges - MINWEDGES) / 2][random_direction] ==
             CUTS);
    if (random_direction >= COORD) {
      if (random_direction - COORD < CUTS)
        random_direction = CW;
      else
        random_direction = CCW;
    }
    move_mball(w, wedge, ring, random_direction, FALSE);
  }
  flush_moves(w);
  cb.reason = MBALL_RANDOMIZE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  if (check_solved(w)) {
    cb.reason = MBALL_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void swap_wedges(w, wedge1, wedge2)
  MballWidget w;
  int wedge1, wedge2;
{
  MballLoc temp1[MAXRINGS], temp2[MAXRINGS];
  int i;

  if (wedge1 == wedge2) {
    for (i = 0; i < w->mball.rings; i++)
      temp1[w->mball.rings - 1 - i] = w->mball.mball_loc[wedge1][i];
    for (i = 0; i < w->mball.rings; i++) {
      w->mball.mball_loc[wedge1][i] = temp1[i];
      w->mball.mball_loc[wedge1][i].direction = !temp1[i].direction;
    }
    draw_wedge(w, wedge1);
  } else {
    for (i = 0; i < w->mball.rings; i++) {
      temp1[w->mball.rings - 1 - i] = w->mball.mball_loc[wedge2][i];
      temp2[w->mball.rings - 1 - i] = w->mball.mball_loc[wedge1][i];
    }
    for (i = 0; i < w->mball.rings; i++) {
      w->mball.mball_loc[wedge1][i] = temp1[i];
      w->mball.mball_loc[wedge2][i] = temp2[i];
      w->mball.mball_loc[wedge1][i].direction = !temp1[i].direction;
      w->mball.mball_loc[wedge2][i].direction = !temp2[i].direction;
    }
    draw_wedge(w, wedge1);
    draw_wedge(w, wedge2);
  }
}

static void move_wedges(w, wedge, ring, direction)
  MballWidget w;
  int wedge, ring, direction;
{
  int i;

  if (direction == CW || direction == CCW) { /* rotate ring */
    int new_i;
    MballLoc temp1, temp2;
    for (i = 0; i < w->mball.wedges; i++) {
      new_i = (direction == CW) ? i : w->mball.wedges - 1 - i;
      if (new_i == ((direction == CW) ? 0 : w->mball.wedges - 1)) {
        temp1 = w->mball.mball_loc[new_i][ring]; 
        w->mball.mball_loc[new_i][ring] = w->mball.mball_loc
          [((direction == CW) ? w->mball.wedges - 1 : 0)][ring];
      } else {
        temp2 = temp1;
        temp1 = w->mball.mball_loc[new_i][ring];
        w->mball.mball_loc[new_i][ring] = temp2;
      }
      draw_sector(w, new_i, ring);
    }
  } else { /* flip */
    int sphere_dir = map_key_dir[(w->mball.wedges - MINWEDGES) / 2][direction];
    int offset = w->mball.wedges / 2;
    int wedge1, wedge2;

    for (i = 0; i < w->mball.wedges / 2; i++)
      if (wedge == i + sphere_dir)
        offset = 0;
    for (i = 0; i < (w->mball.wedges + 2) / 4; i++) {
      wedge1 = (i + sphere_dir + offset) % w->mball.wedges;
      wedge2 = (w->mball.wedges / 2 - 1 - i + sphere_dir + offset) %
        w->mball.wedges;
      swap_wedges(w, wedge1, wedge2);
    }
  }
}

static void draw_frame(w, gc)
  MballWidget w;
  GC gc;
{
  int startx, starty, lengthx, lengthy;

  startx = 1 + w->mball.puzzle_offset.x;
  starty = 1 + w->mball.puzzle_offset.y;
  lengthx = w->mball.view_length - w->mball.delta + w->mball.puzzle_offset.x;
  lengthy = w->mball.view_length - w->mball.delta + w->mball.puzzle_offset.y;
  draw_radar(w, gc, startx, starty, lengthx - startx, lengthy - starty);
  if (w->mball.vertical) {
    XDrawLine(XtDisplay(w), XtWindow(w), gc, 0, lengthy + 1,
      (int) w->core.width - 1, lengthy + 1);
    draw_radar(w, gc, startx, lengthy + 3, lengthx - startx, lengthy - starty);
  } else {
    XDrawLine(XtDisplay(w), XtWindow(w), gc, lengthx + 1, 0,
      lengthx + 1, (int) w->core.height - 1);
    draw_radar(w, gc, lengthx + 3, starty, lengthx - startx, lengthy - starty);
  }
}   

void draw_all_wedges(w)
  MballWidget w;
{
  int wedge;

  for (wedge = 0; wedge < w->mball.wedges; wedge++)
    draw_wedge(w, wedge);
}

static void draw_wedge(w, wedge)
  MballWidget w;
  int wedge;
{
  int ring;

  for (ring = 0; ring < w->mball.rings; ring++)
    draw_sector(w, wedge, ring);
}

static void letter_position(w, wedge, ring, lengthx, lengthy, dx, dy)
  MballWidget w;
  int wedge, ring, lengthx, lengthy, *dx, *dy;
{
  double angle, radius;

  angle = (double) (2 * wedge + 1) * M_PI / w->mball.wedges;
  if (w->mball.rings % 2 && ring == w->mball.rings / 2)
    radius = ((double) 4.0 * ring + 1.0) / ((double) 4.0 * w->mball.rings);
  else
    radius = ((double) 2.0 * ring + 1.0) / ((double) 2.0 * w->mball.rings);
  *dx = lengthx / 2 + (int) ((double) lengthx * radius * cos(angle - M_PI / 2));
  *dy = lengthy / 2 + (int) ((double) lengthy * radius * sin(angle - M_PI / 2));
}

static void offset_sect(w, wedge, dx, dy)
  MballWidget w;
  int wedge;
  int *dx, *dy;
{
  double angle = (double) (2 * wedge + 1) * M_PI / w->mball.wedges;

  *dx = (int) ((double) w->mball.dr * cos(angle - M_PI / 2));
  *dy = (int) ((double) w->mball.dr * sin(angle - M_PI / 2));
}

static void draw_sector(w, wedge, ring)
  MballWidget w;
  int wedge, ring;
{
  int startx, starty, lengthx, lengthy;

  startx = 1 + w->mball.puzzle_offset.x;
  starty = 1 + w->mball.puzzle_offset.y;
  lengthx = w->mball.view_length - w->mball.delta + w->mball.puzzle_offset.x;
  lengthy = w->mball.view_length - w->mball.delta + w->mball.puzzle_offset.y;
  if (ring < (w->mball.rings + 1) / 2)
    draw_sect(w, w->mball.wedge_GC[w->mball.mball_loc[wedge][ring].wedge],
      ring, wedge,
      startx, starty, lengthx - startx, lengthy - starty);
  if (ring + 1 > w->mball.rings / 2) {
    if (w->mball.vertical)
      draw_sect(w, w->mball.wedge_GC[w->mball.mball_loc[wedge][ring].wedge],
        w->mball.rings - 1 - ring,
        (3 * w->mball.wedges / 2 - 1 - wedge) % w->mball.wedges,
        startx, lengthy + 3, lengthx - startx, lengthy - starty);
    else
      draw_sect(w, w->mball.wedge_GC[w->mball.mball_loc[wedge][ring].wedge],
        w->mball.rings - 1 - ring, w->mball.wedges - 1 - wedge,
        lengthx + 3, starty, lengthx - startx, lengthy - starty);
  }
  if (w->mball.depth == 1 || w->mball.mono) {
    int letter_x, letter_y;
    char buf[3];

    if (ring < (w->mball.rings + 1) / 2) {
      letter_position(w, wedge, ring, lengthx - startx, lengthy - starty,
        &letter_x, &letter_y);
      letter_x += startx + w->mball.letter_offset.x;
      letter_y += starty + w->mball.letter_offset.y;
      if (w->mball.orient && !w->mball.mball_loc[wedge][ring].direction) {
        (void) sprintf(buf, "%d%c", w->mball.mball_loc[wedge][ring].wedge,
          w->mball.wedge_name[w->mball.mball_loc[wedge][ring].wedge][0]);
        letter_x += w->mball.letter_offset.x;
        XDrawString(XtDisplay(w), XtWindow(w), w->mball.inverse_GC,
          letter_x, letter_y, buf, 2);
      } else {
        (void) sprintf(buf, "%c",
          w->mball.wedge_name[w->mball.mball_loc[wedge][ring].wedge][0]);
        XDrawString(XtDisplay(w), XtWindow(w), w->mball.inverse_GC,
          letter_x, letter_y, buf, 1);
      }
    }
    if (ring + 1 > w->mball.rings / 2) {
      if (w->mball.vertical) {
        letter_position(w,
          (3 * w->mball.wedges / 2 - 1 - wedge) % w->mball.wedges,
          w->mball.rings - 1 - ring,
          lengthx - startx, lengthy - starty,
          &letter_x, &letter_y);
        letter_x += startx + w->mball.letter_offset.x;
        letter_y += lengthy + 3 + w->mball.letter_offset.y;
      } else {
        letter_position(w,
          w->mball.wedges - 1 - wedge,
          w->mball.rings - 1 - ring,
          lengthx - startx, lengthy - starty,
          &letter_x, &letter_y);
        letter_x += lengthx + 3 + w->mball.letter_offset.x;
        letter_y += starty + w->mball.letter_offset.y;
      }
      if (w->mball.orient && w->mball.mball_loc[wedge][ring].direction) {
        (void) sprintf(buf, "%d%c", w->mball.mball_loc[wedge][ring].wedge,
          w->mball.wedge_name[w->mball.mball_loc[wedge][ring].wedge][0]);
        letter_x += w->mball.letter_offset.x;
        XDrawString(XtDisplay(w), XtWindow(w), w->mball.inverse_GC,
          letter_x, letter_y, buf, 2);
      } else {
        (void) sprintf(buf, "%c",
          w->mball.wedge_name[w->mball.mball_loc[wedge][ring].wedge][0]);
        XDrawString(XtDisplay(w), XtWindow(w), w->mball.inverse_GC,
          letter_x, letter_y, buf, 1);
      }
    }
  } else if (w->mball.orient) {
    int letter_x, letter_y;
    char buf[2];

    if (ring < (w->mball.rings + 1) / 2 &&
        !w->mball.mball_loc[wedge][ring].direction) {
      letter_position(w, wedge, ring, lengthx - startx, lengthy - starty,
        &letter_x, &letter_y);
      letter_x += startx + w->mball.letter_offset.x;
      letter_y += starty + w->mball.letter_offset.y;
      (void) sprintf(buf, "%d", w->mball.mball_loc[wedge][ring].wedge);
      XDrawString(XtDisplay(w), XtWindow(w), w->mball.inverse_GC,
        letter_x, letter_y, buf, 1);
    }
    if (ring + 1 > w->mball.rings / 2 &&
        w->mball.mball_loc[wedge][ring].direction) {
      if (w->mball.vertical) {
        letter_position(w,
          (3 * w->mball.wedges / 2 - 1 - wedge) % w->mball.wedges,
          w->mball.rings - 1 - ring,
          lengthx - startx, lengthy - starty,
          &letter_x, &letter_y);
        letter_x += startx + w->mball.letter_offset.x;
        letter_y += lengthy + 3 + w->mball.letter_offset.y;
      } else {
        letter_position(w,
          w->mball.wedges - 1 - wedge,
          w->mball.rings - 1 - ring,
          lengthx - startx, lengthy - starty,
          &letter_x, &letter_y);
        letter_x += lengthx + 3 + w->mball.letter_offset.x;
        letter_y += starty + w->mball.letter_offset.y;
      }
      (void) sprintf(buf, "%d", w->mball.mball_loc[wedge][ring].wedge);
      XDrawString(XtDisplay(w), XtWindow(w), w->mball.inverse_GC,
        letter_x, letter_y, buf, 1);
    }
  }
}

static void draw_radar(w, gc, startx, starty, lengthx, lengthy)
  MballWidget w;
  GC gc;
  int startx, starty, lengthx, lengthy;
{
  int r, i;
  double angle, increment;

  XDrawArc(XtDisplay(w), XtWindow(w), gc, startx, starty,
    lengthx, lengthy, 0, CIRCLE);
  if (w->mball.rings % 2)
    for (r = 1; r < w->mball.rings / 2 + 1; r++)
      XDrawArc(XtDisplay(w), XtWindow(w), gc,
        startx - lengthx / (2 * w->mball.rings) +
          (w->mball.rings / 2 + 1 - r) * lengthx / w->mball.rings,
        starty - lengthy / (2 * w->mball.rings) +
          (w->mball.rings / 2 + 1 - r) * lengthy / w->mball.rings,
        r * 2 * lengthx / w->mball.rings,
        r * 2 * lengthy / w->mball.rings,
        0, CIRCLE);
  else
    for (r = 1; r < w->mball.rings / 2; r++)
      XDrawArc(XtDisplay(w), XtWindow(w), gc,
        startx + (w->mball.rings / 2 - r) * lengthx / w->mball.rings,
        starty + (w->mball.rings / 2 - r) * lengthy / w->mball.rings,
        r * 2 * lengthx / w->mball.rings,
        r * 2 * lengthy / w->mball.rings,
        0, CIRCLE);
  increment = RADIANS(NUM_DEGREES) / (double) w->mball.wedges;
  angle = RADIANS(RT_ANG);
  for (i = 0; i < w->mball.wedges; i++) {
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      startx + lengthx / 2, starty + lengthy / 2,
      startx + lengthx / 2 + (int) ((double) lengthx * cos(angle) / 2.0),
      starty + lengthy / 2 + (int) ((double) lengthy * sin(angle) / 2.0));
    angle += increment;
  }
}

static void draw_sect(w, gc, r, wedge, startx, starty, lengthx, lengthy)
  MballWidget w;
  GC gc;
  int r, wedge;
  int startx, starty, lengthx, lengthy;
{
  int dx, dy;

  offset_sect(w, wedge, &dx, &dy);
  if (w->mball.rings % 2) {
    if (r == w->mball.rings / 2)
      XFillSector(XtDisplay(w), XtWindow(w), gc,
        startx + lengthx / 2 + dx, starty + lengthy / 2 + dy,
        r * 2 * lengthx / w->mball.rings - 2 * w->mball.dr,
        r * 2 * lengthy / w->mball.rings - 2 * w->mball.dr,
        (r + 1) * 2 * lengthx / (w->mball.rings + 1) - 2 * w->mball.dr,
        (r + 1) * 2 * lengthy / (w->mball.rings + 1) - 2 * w->mball.dr,
        CIRCLE_4 - CIRCLE * wedge / w->mball.wedges,
        -CIRCLE / w->mball.wedges);
    else
      XFillSector(XtDisplay(w), XtWindow(w), gc,
        startx + lengthx / 2 + dx, starty + lengthy / 2 + dy,
        r * 2 * lengthx / w->mball.rings - 2 * w->mball.dr,
        r * 2 * lengthy / w->mball.rings - 2 * w->mball.dr,
        (r + 1) * 2 * lengthx / w->mball.rings - 2 * w->mball.dr,
        (r + 1) * 2 * lengthy / w->mball.rings - 2 * w->mball.dr,
        CIRCLE_4 - CIRCLE * wedge / w->mball.wedges,
        -CIRCLE / w->mball.wedges);
  } else
    XFillSector(XtDisplay(w), XtWindow(w), gc,
      startx + lengthx / 2 + dx, starty + lengthy / 2 + dy,
      r * 2 * lengthx / w->mball.rings - 2 * w->mball.dr,
      r * 2 * lengthy / w->mball.rings - 2 * w->mball.dr,
      (r + 1) * 2 * lengthx / w->mball.rings - 2 * w->mball.dr,
      (r + 1) * 2 * lengthy / w->mball.rings - 2 * w->mball.dr,
      CIRCLE_4 - CIRCLE * wedge / w->mball.wedges,
      -CIRCLE / w->mball.wedges);
}

static void XFillSector(display, drawable, gc,
    xo, yo, width1, height1, width2, height2, angle1, angle2)
  Display *display;
  Drawable drawable;
  GC gc;
  int xo, yo;
  int width1, height1, width2, height2;
  int angle1, angle2;
{

  int d, r1 = MIN(width1, height1) / 2, r2 = MIN(width2, height2) / 2;

  if (r1 > r2) {
    d = r1; r1 = r2; r2 = d;
  }
  if (r1 < 0)
    r1 = -3;
  for (d = 2 * (r1 + 3); d < 2 * (r2 - 1); d++) {
    XDrawArc(display, drawable, gc, xo - d / 2, yo - d / 2, d, d,
      angle1, angle2);
  }
}

Boolean check_solved(w)
  MballWidget w;
{
  int wedge, ring;
  MballLoc test;

  if (w->mball.orient)
    for (wedge = 0; wedge < w->mball.wedges; wedge++) {
      if (wedge == 0) {
        test.wedge = w->mball.mball_loc[wedge][0].wedge;
        test.direction = w->mball.mball_loc[wedge][0].direction;
      }
      for (ring = 0; ring < w->mball.rings; ring++) {
        if (test.direction != w->mball.mball_loc[wedge][ring].direction)
          return FALSE;
        if (test.direction) {
          if ((w->mball.wedges + w->mball.mball_loc[wedge][ring].wedge -
               test.wedge) % w->mball.wedges != wedge)
            return FALSE;
        } else {
          if ((w->mball.wedges - w->mball.mball_loc[wedge][ring].wedge +
               test.wedge) % w->mball.wedges != wedge)
            return FALSE;
        }
      }
    }
  else
    for (wedge = 0; wedge < w->mball.wedges; wedge++)
      for (ring = 0; ring < w->mball.rings; ring++)
        if (ring == 0) {
          test.wedge = w->mball.mball_loc[wedge][ring].wedge;
          test.direction = w->mball.mball_loc[wedge][ring].direction;
        } else if (test.wedge != w->mball.mball_loc[wedge][ring].wedge)
          return FALSE;
  return TRUE;
}

#ifdef DEBUG

static void print_mball(w)
  MballWidget w;
{
  int wedge, ring;

  for (wedge = 0; wedge < w->mball.wedges; wedge++) {
    for (ring = 0; ring < w->mball.rings; ring++) {
      (void) printf("%d %d  ", w->mball.mball_loc[wedge][ring].wedge,
               w->mball.mball_loc[wedge][ring].direction);
    }
    (void) printf("\n");
  }
  (void) printf("\n");
}

#endif
