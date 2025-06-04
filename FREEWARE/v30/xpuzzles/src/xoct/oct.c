/*
# X-BASED OCTAHEDRON
#
#  Oct.c
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

/* Methods file for Oct */

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
#include "OctP.h"

#ifndef DATAFILE
#define DATAFILE "/usr/games/lib/oct.data"
#endif

static void InitializeOct();
static void ExposeOct();
static void ResizeOct();
static void DestroyOct();
static Boolean SetValuesOct();
static void quit_oct();
static void practice_oct();
static void maybe_practice_oct();
static void randomize_oct();
static void maybe_randomize_oct();
static void enter_oct();
static void write_oct();
static void undo_oct();
static void solve_oct();
static void increment_oct();
static void decrement_oct();
static void orientize_oct();
static void period3_mode_oct();
static void period4_mode_oct();
static void both_mode_oct();
static void sticky_mode_oct();
static void move_oct_tl();
static void move_oct_top();
static void move_oct_tr();
static void move_oct_left();
static void move_oct_cw();
static void move_oct_right();
static void move_oct_bl();
static void move_oct_bottom();
static void move_oct_br();
static void move_oct_ccw();
static void move_oct_input();
static void get_color();
static void move_control_cb();
static void check_polyhedrons();
static void reset_polyhedrons();
static void resize_polyhedrons();
static int position_polyhedrons();
static void no_move_polyhedrons();
static void practice_polyhedrons();
static void randomize_polyhedrons();
static void move_polyhedrons();
static void rotate_face();
/* rcd : row, column, or diagonal */
static void read_rcd();
static void rotate_rcd();
static void reverse_rcd();
static void write_rcd();
static void draw_frame();
static void draw_triangle();
static void draw_orient_line();
static int length();
static int Sqrt();

#ifdef DEBUG
static void print_octa();
static void print_face();
static void print_row();
#endif

static char defaultTranslationsOct[] =
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
   <KeyPress>3: period3()\n\
   <KeyPress>4: period4()\n\
   <KeyPress>b: both()\n\
   <KeyPress>y: sticky()";
 
static XtActionsRec actionsListOct[] =
{
  {"quit", (XtActionProc) quit_oct},
  {"move_tl", (XtActionProc) move_oct_tl},
  {"move_top", (XtActionProc) move_oct_top},
  {"move_tr", (XtActionProc) move_oct_tr},
  {"move_left", (XtActionProc) move_oct_left},
  {"move_cw", (XtActionProc) move_oct_cw},
  {"move_right", (XtActionProc) move_oct_right},
  {"move_bl", (XtActionProc) move_oct_bl},
  {"move_bottom", (XtActionProc) move_oct_bottom},
  {"move_br", (XtActionProc) move_oct_br},
  {"move_ccw", (XtActionProc) move_oct_ccw},
  {"practice", (XtActionProc) practice_oct},
  {"maybe_practice", (XtActionProc) maybe_practice_oct},
  {"randomize", (XtActionProc) randomize_oct},
  {"maybe_randomize", (XtActionProc) maybe_randomize_oct},
  {"enter", (XtActionProc) enter_oct},
  {"write", (XtActionProc) write_oct},
  {"undo", (XtActionProc) undo_oct},
  {"solve", (XtActionProc) solve_oct},
  {"increment", (XtActionProc) increment_oct},
  {"decrement", (XtActionProc) decrement_oct},
  {"orientize", (XtActionProc) orientize_oct},
  {"period3", (XtActionProc) period3_mode_oct},
  {"period4", (XtActionProc) period4_mode_oct},
  {"both", (XtActionProc) both_mode_oct},
  {"sticky", (XtActionProc) sticky_mode_oct}
};

static XtResource resourcesOct[] =
{
  /* Beware color values are swapped */
  {XtNfaceColor0, XtCLabel, XtRString, sizeof(String),
   XtOffset(OctWidget, oct.face_name[4]), XtRString, "Red"},
  {XtNfaceColor1, XtCLabel, XtRString, sizeof(String),
   XtOffset(OctWidget, oct.face_name[5]), XtRString, "Blue"},
  {XtNfaceColor2, XtCLabel, XtRString, sizeof(String),
   XtOffset(OctWidget, oct.face_name[6]), XtRString, "White"},
  {XtNfaceColor3, XtCLabel, XtRString, sizeof(String),
   XtOffset(OctWidget, oct.face_name[7]), XtRString, "Magenta"},
  {XtNfaceColor4, XtCLabel, XtRString, sizeof(String),
   XtOffset(OctWidget, oct.face_name[0]), XtRString, "Orange"},
  {XtNfaceColor5, XtCLabel, XtRString, sizeof(String),
   XtOffset(OctWidget, oct.face_name[1]), XtRString, "Pink"},
  {XtNfaceColor6, XtCLabel, XtRString, sizeof(String),
   XtOffset(OctWidget, oct.face_name[2]), XtRString, "Green"},
  {XtNfaceColor7, XtCLabel, XtRString, sizeof(String),
   XtOffset(OctWidget, oct.face_name[3]), XtRString, "Yellow"},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
   XtOffset(OctWidget, oct.foreground), XtRString, XtDefaultForeground},
  {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
   XtOffset(OctWidget, core.width), XtRString, "200"},
  {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
   XtOffset(OctWidget, core.height), XtRString, "400"},
  {XtNsize, XtCSize, XtRInt, sizeof(int),
   XtOffset(OctWidget, oct.size), XtRString, "3"}, /*DEFAULTOCTAS*/
  {XtNmode, XtCMode, XtRInt, sizeof(int),
   XtOffset(OctWidget, oct.mode), XtRString, "4"}, /*DEFAULTMODE*/
  {XtNorient, XtCOrient, XtRBoolean, sizeof(Boolean),
   XtOffset(OctWidget, oct.orient), XtRString, "FALSE"}, /*DEFAULTORIENT*/
  {XtNsticky, XtCSticky, XtRBoolean, sizeof(Boolean),
   XtOffset(OctWidget, oct.sticky), XtRString, "FALSE"},
  {XtNmono, XtCMono, XtRBoolean, sizeof(Boolean),
   XtOffset(OctWidget, oct.mono), XtRString, "FALSE"},
  {XtNpractice, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(OctWidget, oct.practice), XtRString, "FALSE"},
  {XtNstart, XtCBoolean, XtRBoolean, sizeof(Boolean),
   XtOffset(OctWidget, oct.started), XtRString, "FALSE"},
  {XtNselectCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
   XtOffset(OctWidget, oct.select), XtRCallback, NULL}
};

OctClassRec octClassRec =
{
  {
    (WidgetClass) &widgetClassRec,	/* superclass */
    "Oct",				/* class name */
    sizeof(OctRec),			/* widget size */
    NULL,				/* class initialize */
    NULL,				/* class part initialize */
    FALSE,				/* class inited */
    InitializeOct,			/* initialize */
    NULL,				/* initialize hook */
    XtInheritRealize,			/* realize */
    actionsListOct,			/* actions */
    XtNumber(actionsListOct),		/* num actions */
    resourcesOct,			/* resources */
    XtNumber(resourcesOct),		/* num resources */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress exposure */
    TRUE,				/* compress enterleave */
    TRUE,				/* visible interest */
    DestroyOct,				/* destroy */
    ResizeOct,				/* resize */
    ExposeOct,				/* expose */
    SetValuesOct,			/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    NULL,				/* accept focus */
    XtVersion,				/* version */
    NULL,				/* callback private */
    defaultTranslationsOct,		/* tm table */
    NULL,				/* query geometry */
    NULL,				/* display accelerator */
    NULL				/* extension */
  },
  {
    0					/* ignore */
  }
};

WidgetClass octWidgetClass = (WidgetClass) &octClassRec;

typedef struct _RowNextP3
{
  int view_changed, face, direction;
} RowNextP3;
static OctLoc row_to_rotate[MAXVIEWS][MAXSIDES][COORD] = 
{
  {
    {{IGNORE, IGNORE}, {BOTTOM,     CW}, {IGNORE, IGNORE}, {BOTTOM,    CCW},
     { RIGHT,     CW}, {  LEFT,     CW}, { RIGHT,    CCW}, {  LEFT,    CCW}},

    {{  LEFT,    CCW}, {IGNORE, IGNORE}, {  LEFT,     CW}, {IGNORE, IGNORE},
     {   TOP,    CCW}, {BOTTOM,     CW}, {   TOP,     CW}, {BOTTOM,    CCW}},

    {{IGNORE, IGNORE}, {   TOP,    CCW}, {IGNORE, IGNORE}, {   TOP,     CW},
     {  LEFT,    CCW}, { RIGHT,    CCW}, {  LEFT,     CW}, { RIGHT,     CW}},

    {{ RIGHT,     CW}, {IGNORE, IGNORE}, { RIGHT,    CCW}, {IGNORE, IGNORE},
     {BOTTOM,     CW}, {   TOP,    CCW}, {BOTTOM,    CCW}, {   TOP,     CW}}
  },
  {
    {{IGNORE, IGNORE}, {BOTTOM,    CCW}, {IGNORE, IGNORE}, {BOTTOM,     CW},
     {  LEFT,    CCW}, { RIGHT,    CCW}, {  LEFT,     CW}, { RIGHT,     CW}},

    {{ RIGHT,     CW}, {IGNORE, IGNORE}, { RIGHT,    CCW}, {IGNORE, IGNORE},
     {   TOP,     CW}, {BOTTOM,    CCW}, {   TOP,    CCW}, {BOTTOM,     CW}},

    {{IGNORE, IGNORE}, {   TOP,     CW}, {IGNORE, IGNORE}, {   TOP,    CCW},
     { RIGHT,     CW}, {  LEFT,     CW}, { RIGHT,    CCW}, {  LEFT,    CCW}},

    {{  LEFT,    CCW}, {IGNORE, IGNORE}, {  LEFT,     CW}, {IGNORE, IGNORE},
     {BOTTOM,    CCW}, {   TOP,     CW}, {BOTTOM,     CW}, {  TOP,     CCW}}
  }
};
static RowNextP3 slide_next_row_p3[MAXSIDES][COORD] =
{
  {
    {IGNORE, IGNORE, IGNORE},
    { FALSE,  RIGHT,     BR},
    {IGNORE, IGNORE, IGNORE},
    { FALSE,   LEFT,     BL},
    {  TRUE, BOTTOM,     TR},
    { FALSE,  RIGHT, BOTTOM},
    { FALSE,   LEFT, BOTTOM},
    {  TRUE, BOTTOM,     TL}
  },
  {
    { FALSE,    TOP,     TL},
    {IGNORE, IGNORE, IGNORE},
    { FALSE, BOTTOM,     BL},
    {IGNORE, IGNORE, IGNORE},
    {  TRUE,  RIGHT,     BL},
    {  TRUE,  RIGHT,     TL},
    { FALSE, BOTTOM,   LEFT},
    { FALSE,    TOP,   LEFT}
  },
  {
    {IGNORE, IGNORE, IGNORE},
    { FALSE,  RIGHT,     TR},
    {IGNORE, IGNORE, IGNORE},
    { FALSE,   LEFT,     TL},
    { FALSE,  RIGHT,    TOP},
    {  TRUE,    TOP,     BR},
    {  TRUE,    TOP,     BL},
    { FALSE,   LEFT,    TOP},
  },
  {
    { FALSE,    TOP,     TR},
    {IGNORE, IGNORE, IGNORE},
    { FALSE, BOTTOM,     BR},
    {IGNORE, IGNORE, IGNORE},
    { FALSE,    TOP,  RIGHT},
    { FALSE, BOTTOM,  RIGHT},
    {  TRUE,   LEFT,     TR},
    {  TRUE,   LEFT,     BR}
  }
};
static int reverse_p3[MAXSIDES][COORD] =
{
  {IGNORE,   TRUE, IGNORE,  FALSE, FALSE,  TRUE,  TRUE,  TRUE},
  {  TRUE, IGNORE,  FALSE, IGNORE, FALSE,  TRUE,  TRUE,  TRUE},
  {IGNORE,  FALSE, IGNORE,   TRUE,  TRUE,  TRUE, FALSE,  TRUE},
  { FALSE, IGNORE,   TRUE, IGNORE,  TRUE,  TRUE, FALSE,  TRUE}
};
static int rotate_orient_p3[MAXSIDES][COORD] =
{
  {IGNORE,      7, IGNORE,      5, 11,  2, 10,  1},
  {    11, IGNORE,      1, IGNORE,  7,  5,  2, 10},
  {IGNORE,      5, IGNORE,      7, 10,  1, 11,  2},
  {     1, IGNORE,     11, IGNORE,  2, 10,  7,  5}
};

static XPoint triangle_unit[MAXSIDES][3] =
{
  {{0, 0}, { 1,  1}, {-2,  0}},
  {{0, 0}, {-1,  1}, { 0, -2}},
  {{0, 0}, {-1, -1}, { 2,  0}},
  {{0, 0}, { 1, -1}, { 0,  2}}
};
static XPoint triangle_list[MAXSIDES][3], letter_list[MAXSIDES];

static void InitializeOct(request, new)
  Widget request, new;
{
  OctWidget w = (OctWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  int face;
  
  check_polyhedrons(w);
  init_moves();
  reset_polyhedrons(w);
  w->oct.practice = FALSE;
  (void) SRAND(getpid());
  valueMask = GCForeground | GCBackground;
  values.foreground = w->oct.foreground;
  values.background = w->core.background_pixel;
  w->oct.puzzle_GC = XtGetGC(new, valueMask, &values);
  w->oct.depth = DefaultDepthOfScreen(XtScreen(w));
  valueMask = GCForeground | GCBackground;
  values.foreground = w->core.background_pixel;
  values.background = w->oct.foreground;
  w->oct.inverse_GC = XtGetGC(new, valueMask, &values);
  for (face = 0; face < MAXFACES; face++)
    get_color(w, face, TRUE);
  ResizeOct(w);
}

static void DestroyOct(old)
  Widget old;
{
  OctWidget w = (OctWidget) old;
  int face;

  for (face = 0; face < MAXFACES; face++)
    XtReleaseGC(old, w->oct.face_GC[face]);
  XtReleaseGC(old, w->oct.puzzle_GC);
  XtReleaseGC(old, w->oct.inverse_GC);
  XtRemoveCallbacks(old, XtNselectCallback, w->oct.select);
}

static void ResizeOct(w)
  OctWidget w;
{
  int temp_length;

  w->oct.delta = 4;
  w->oct.vertical = (w->core.height >= w->core.width);
  if (w->oct.vertical)
    temp_length = MIN(w->core.height / 2, w->core.width);
  else
    temp_length = MIN(w->core.height, w->core.width / 2);
  w->oct.octa_length = MAX((temp_length - w->oct.delta + 1) / w->oct.size, 0);
  w->oct.face_length = w->oct.size * w->oct.octa_length;
  w->oct.view_length = w->oct.face_length + w->oct.delta + 3;
  w->oct.view_middle = w->oct.view_length / 2;
  if (w->oct.vertical) {
    w->oct.puzzle_width = w->oct.view_length - 1;
    w->oct.puzzle_height = 2 * w->oct.view_length - w->oct.delta - 2;
  } else {
    w->oct.puzzle_width = 2 * w->oct.view_length - w->oct.delta - 2;
    w->oct.puzzle_height = w->oct.view_length - 1;
  }
  w->oct.puzzle_offset.x = ((int) w->core.width - w->oct.puzzle_width) / 2;
  w->oct.puzzle_offset.y = ((int) w->core.height - w->oct.puzzle_height) / 2;
  resize_polyhedrons(w);
}

static void ExposeOct(new, event, region)
  Widget new;
  XEvent *event;
  Region region; /* Not used */
{
  OctWidget w = (OctWidget) new;

  if (w->core.visible) {
    draw_frame(w, w->oct.puzzle_GC);
    draw_all_polyhedrons(w);
  }
}

static Boolean SetValuesOct(current, request, new)
  Widget current, request, new;
{
  OctWidget c = (OctWidget) current, w = (OctWidget) new;
  XGCValues values;
  XtGCMask valueMask;
  Boolean redraw = FALSE;
  int face;

  check_polyhedrons(w);
  if (w->oct.foreground != c->oct.foreground) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->oct.foreground;
    values.background = w->core.background_pixel;
    XtReleaseGC(new, w->oct.puzzle_GC);
    w->oct.puzzle_GC = XtGetGC(new, valueMask, &values);
    redraw = TRUE;
  }
  if (w->core.background_pixel != c->core.background_pixel) {
    valueMask = GCForeground | GCBackground;
    values.foreground = w->core.background_pixel;
    values.background = w->oct.foreground;
    XtReleaseGC(new, w->oct.inverse_GC);
    w->oct.inverse_GC = XtGetGC(new, valueMask, &values);
    redraw = TRUE;
  }
  if (w->oct.mono || w->oct.depth == 1) {
    valueMask = GCForeground | GCBackground;
    values.background = w->core.background_pixel;
    values.foreground = w->oct.foreground;
    for (face = 0; face < MAXFACES; face++) {
      XtReleaseGC(new, w->oct.face_GC[face]);
      w->oct.face_GC[face] = XtGetGC(new, valueMask, &values);
    }
    redraw = TRUE;
  }
  for (face = 0; face < MAXFACES; face++) {
    if (strcmp(w->oct.face_name[face], c->oct.face_name[face]))
      get_color(w, face, FALSE);
  }
  if (w->oct.orient != c->oct.orient) {
    reset_polyhedrons(w);
    w->oct.practice = FALSE;
    redraw = TRUE;
  } else if (w->oct.practice != c->oct.practice) {
    reset_polyhedrons(w);
    redraw = TRUE;
  }
  if (w->oct.size != c->oct.size ||
      w->oct.mode != c->oct.mode ||
      w->oct.sticky != c->oct.sticky) {
    reset_polyhedrons(w);
    w->oct.practice = FALSE;
    ResizeOct(w);
    redraw = TRUE;
  }
  if (w->oct.octa_length != c->oct.octa_length) {
    ResizeOct(w);
    redraw = TRUE;
  }
  return (redraw);
}

static void quit_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  XtCloseDisplay(XtDisplay(w));
  exit(0);
}

static void practice_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  practice_polyhedrons(w);
}
 
static void maybe_practice_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->oct.started)
    practice_polyhedrons(w);
}
 
static void randomize_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  randomize_polyhedrons(w);
}
 
static void maybe_randomize_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (!w->oct.started)
    randomize_polyhedrons(w);
}

static void enter_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;
  char c;
  int i, size, mode, sticky, orient, practice, moves;
  octCallbackStruct cb;

  if ((fp = fopen(DATAFILE, "r")) == NULL)
    (void) printf("Can not read %s to enter.\n", DATAFILE);
  else {
    flush_moves(w);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &size);
    if (size >= MINOCTAS && size <= MAXOCTAS) {
      for (i = w->oct.size; i < size; i++) {
        cb.reason = OCT_INC;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
      for (i = w->oct.size; i > size; i--) {
        cb.reason = OCT_DEC;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    } else
      (void) printf("%s corrupted: size %d should be between %d and %d\n",
         DATAFILE, size, MINOCTAS, MAXOCTAS);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &mode);
    if (mode >= PERIOD3 && mode <= BOTH)
      switch (mode) {
        case PERIOD3:
          cb.reason = OCT_PERIOD3;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          break;
        case PERIOD4:
          cb.reason = OCT_PERIOD4;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
          break;
        case BOTH:
          cb.reason = OCT_BOTH;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    else
      (void) printf("%s corrupted: mode %d should be between %d and %d\n",
         DATAFILE, mode, PERIOD3, BOTH);
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &sticky);
    if (w->oct.sticky != (Boolean) sticky) {
      cb.reason = OCT_STICKY;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &orient);
    if (w->oct.orient != (Boolean) orient) {
      cb.reason = OCT_ORIENT;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &practice);
    if (w->oct.practice != (Boolean) practice) {
      cb.reason = OCT_PRACTICE;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &moves);
    scan_start_position(fp, w);
    cb.reason = OCT_RESTORE;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    set_start_position(w);
    scan_moves(fp, w, moves);
    (void) fclose(fp);
    (void) printf("%s: size %d, mode %d, sticky %d, orient %d",
      DATAFILE, size, mode, sticky, orient);
    (void) printf(", practice %d, moves %d.\n", practice, moves);
  }
}

static void write_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  FILE *fp;

  if ((fp = fopen(DATAFILE, "w")) == NULL)
    (void) printf("Can not write to %s.\n", DATAFILE);
  else {
    (void) fprintf(fp, "size: %d\n", w->oct.size);
    (void) fprintf(fp, "mode: %d\n", w->oct.mode);
    (void) fprintf(fp, "sticky: %d\n", (w->oct.sticky) ? 1 : 0);
    (void) fprintf(fp, "orient: %d\n", (w->oct.orient) ? 1 : 0);
    (void) fprintf(fp, "practice: %d\n", (w->oct.practice) ? 1 : 0);
    (void) fprintf(fp, "moves: %d\n", num_moves());
    print_start_position(fp, w);
    print_moves(fp);
    (void) fclose(fp);
    (void) printf("Saved to %s.\n", DATAFILE);
  }
}

static void undo_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  if (moves_made()) {
    int view, face, i, j, k, direction, style, control;

    get_move(&face, &i, &j, &k, &direction, &style, &control);
    view = face / MAXSIDES;
    face = face % MAXSIDES;
    direction = (direction < COORD) ? ((direction < MAXSIDES) ?
      (direction + MAXSIDES / 2) % MAXSIDES :
      (direction + MAXSIDES / 2) % MAXSIDES + MAXSIDES) : 3 * COORD - direction;
    if (control)
      move_control_cb(w, view, face, direction, style);
    else {
      octCallbackStruct cb;

      move_polyhedrons(w, view, face, i, j, k, direction, style);
      cb.reason = OCT_UNDO;
      XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
    }
  }
}

static void solve_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  /* solve_polyhedrons(w); */ /* Sorry, unimplemented */
}
 
static void increment_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  octCallbackStruct cb;
 
  if (w->oct.size == MAXOCTAS)
    return;
  cb.reason = OCT_INC;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
static void decrement_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  octCallbackStruct cb;
 
  if (w->oct.size == MINOCTAS)
    return;
  cb.reason = OCT_DEC;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
 
static void orientize_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  octCallbackStruct cb;
 
  cb.reason = OCT_ORIENT;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void period3_mode_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  octCallbackStruct cb;
 
  cb.reason = OCT_PERIOD3;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void period4_mode_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  octCallbackStruct cb;
 
  cb.reason = OCT_PERIOD4;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void both_mode_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  octCallbackStruct cb;
 
  cb.reason = OCT_BOTH;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void sticky_mode_oct(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  octCallbackStruct cb;
 
  cb.reason = OCT_STICKY;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void move_oct_ccw(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_oct_input(w, event->xbutton.x, event->xbutton.y, CCW,
    (int) (event->xbutton.state & (ShiftMask | LockMask)),
    (int) (event->xbutton.state & ControlMask));
}

static void move_oct_tl(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_oct_input(w, event->xbutton.x, event->xbutton.y, TL,
    (int) (event->xbutton.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_oct_top(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_oct_input(w, event->xbutton.x, event->xbutton.y, TOP,
    (int) (event->xbutton.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_oct_tr(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_oct_input(w, event->xbutton.x, event->xbutton.y, TR,
    (int) (event->xbutton.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_oct_left(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_oct_input(w, event->xbutton.x, event->xbutton.y, LEFT,
    (int) (event->xbutton.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_oct_cw(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_oct_input(w, event->xbutton.x, event->xbutton.y, CW,
    (int) (event->xbutton.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_oct_right(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_oct_input(w, event->xbutton.x, event->xbutton.y, RIGHT,
    (int) (event->xbutton.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_oct_bl(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_oct_input(w, event->xbutton.x, event->xbutton.y, BL,
    (int) (event->xbutton.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_oct_bottom(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_oct_input(w, event->xbutton.x, event->xbutton.y, BOTTOM,
    (int) (event->xbutton.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_oct_br(w, event, args, n_args)
  OctWidget w;
  XEvent *event;
  char *args[];
  int n_args;
{
  move_oct_input(w, event->xbutton.x, event->xbutton.y, BR,
    (int) (event->xbutton.state & (ShiftMask | LockMask)),
    (int) (event->xkey.state & ControlMask));
}

static void move_oct_input(w, x, y, direction, shift, control)
  OctWidget w;
  int x, y, direction, shift, control;
{
  int style, view, face, i, j, k;
 
  if (w->oct.mode != BOTH) {
    if (control && shift)
      style = (w->oct.mode == PERIOD4) ? PERIOD3 : PERIOD4;
    else
      style = (w->oct.mode == PERIOD3) ? PERIOD3 : PERIOD4;
  } else
    style = (shift) ? PERIOD4 : PERIOD3;
  if (!w->oct.practice && !control && check_solved(w)) {
    no_move_polyhedrons(w);
    return;
  }
  if (!position_polyhedrons(w, x, y, style,
        &view, &face, &i, &j, &k, &direction))
      return;
  control = (control) ? 1 : 0;
  move_oct(w, view, face, i, j, k, direction, style, control);
  if (!control && check_solved(w)) {
    octCallbackStruct cb;
 
    cb.reason = OCT_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

void move_oct(w, view, face, i, j, k, direction, style, control)
  OctWidget w;
  int view, face, i, j, k, direction, style, control;
{
  if (control)
    move_control_cb(w, view, face, direction, style);
  else {
    octCallbackStruct cb;

    move_polyhedrons(w, view, face, i, j, k, direction, style);
    cb.reason = OCT_MOVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  put_move(view * MAXSIDES + face, i, j, k, direction, style, control);
}

static void get_color(w, face, init)
  OctWidget w;
  int face, init;
{
  XGCValues values;
  XtGCMask valueMask;
  XColor color_cell, rgb;
 
  valueMask = GCForeground | GCBackground;
  values.background = w->core.background_pixel;
  if (w->oct.depth > 1 && !w->oct.mono) {
    if (XAllocNamedColor(XtDisplay(w),
        DefaultColormap(XtDisplay(w), XtWindow(w)),
        w->oct.face_name[face], &color_cell, &rgb)) {
      values.foreground = w->oct.face_color[face] = color_cell.pixel;
      if (!init)
        XtReleaseGC((Widget) w, w->oct.face_GC[face]);
      w->oct.face_GC[face] = XtGetGC((Widget) w, valueMask, &values);
      return;
    } else {
      char buf[121];
 
      (void) sprintf(buf, "Color name \"%s\" is not defined",
               w->oct.face_name[face]);
      XtWarning(buf);
    }
  }
  values.foreground = w->oct.foreground;
  if (!init)
    XtReleaseGC((Widget) w, w->oct.face_GC[face]);
  w->oct.face_GC[face] = XtGetGC((Widget) w, valueMask, &values);
}

static void move_control_cb(w, view, face, direction, style)
  OctWidget w;
  int view, face, direction, style;
{
  octCallbackStruct cb;
  int i, j;

  if (w->oct.sticky) {
    if (style == PERIOD3)
      for (i = 0; i < 3; i++) {
        if (i == 2)
          i++;
        move_polyhedrons(w, view, face, i, i, i,
          direction, style);
        cb.reason = OCT_CONTROL;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    else /* (style == PERIOD4) */
      for (i = 0; i < 3; i++) {
        if (i < 2) {
          if (i == 1)
            j = i + 2;
          else
            j = i;
          move_polyhedrons(w, view, face, j, j, j, direction, style);
        } else {
          if (direction == CW || direction == CCW) {
            j = i - 2;
            move_polyhedrons(w, !view, (face % 2) ?
              (face + MAXSIDES / 2) % MAXSIDES : face, j, j, j,
              (direction == CW) ? CCW : CW, style);
          } else {
            j = i + 1;
            move_polyhedrons(w, !view, (face % 2) ?
              (face + MAXSIDES / 2) % MAXSIDES : face, j, j, j,
              (direction % 2) ? (direction + 1) % MAXSIDES + MAXSIDES :
              (direction + 3) % MAXSIDES + MAXSIDES, style);
          }
        }
        cb.reason = OCT_CONTROL;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
  } else {
    if (style == PERIOD3)
      for (i = 0; i < w->oct.size; i++) {
        move_polyhedrons(w, view, face, i, i, i, direction, style);
        cb.reason = OCT_CONTROL;
          XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
    else /* (style == PERIOD4) */
      for (i = 0; i < 2 * w->oct.size - 1; i++) {
        if (i < w->oct.size)
          move_polyhedrons(w, view, face, i, i, i, direction, style);
        else {
          if (direction == CW || direction == CCW) {
            j = i - w->oct.size;
            move_polyhedrons(w, !view, (face % 2) ?
              (face + MAXSIDES / 2) % MAXSIDES : face, j, j, j,
              (direction == CW) ? CCW : CW, style);
          } else {
            j = i - w->oct.size + 1;
            move_polyhedrons(w, !view, (face % 2) ?
              (face + MAXSIDES / 2) % MAXSIDES : face, j, j, j,
              (direction % 2) ? (direction + 1) % MAXSIDES + MAXSIDES :
              (direction + 3) % MAXSIDES + MAXSIDES, style);
          }
        }
        cb.reason = OCT_CONTROL;
        XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
      }
  }
}

static void check_polyhedrons(w)
  OctWidget w;
{
  if (w->oct.size < MINOCTAS || w->oct.size > MAXOCTAS) {
    char buf[121];

    (void) sprintf(buf,
             "Number of Octas on edge out of bounds, use %d..%d",
             MINOCTAS, MAXOCTAS);
    XtWarning(buf);
    w->oct.size = DEFAULTOCTAS;
  }
  if (w->oct.mode < PERIOD3 || w->oct.mode > BOTH) {
    XtWarning("Mode is in error, use 3 for Period2, 4 for Period3, 5 for Both");
    w->oct.mode = DEFAULTMODE;
  }
}

static void reset_polyhedrons(w)
  OctWidget w;
{
  int i, face;

  w->oct.size_size = w->oct.size * w->oct.size;
  for (face = 0; face < MAXFACES; face++)
    for (i = 0; i < w->oct.size_size; i++) {
      w->oct.octa_loc[face][i].face = face;
      w->oct.octa_loc[face][i].rotation = 3 * face % MAXORIENT;
    }
  flush_moves(w);
  w->oct.started = FALSE;
}

static void resize_polyhedrons(w)
  OctWidget w;
{
  int i, j;

  w->oct.octa_length = w->oct.face_length / (2 * w->oct.size) -
    w->oct.delta - 1;
  for (i = 0; i < 3; i++)
    for (j = 0; j < MAXSIDES; j++) { 
      triangle_list[j][i].x = triangle_unit[j][i].x * w->oct.octa_length;
      triangle_list[j][i].y = triangle_unit[j][i].y * w->oct.octa_length;
    }
  letter_list[TOP].x = -2;
  letter_list[TOP].y = 3 * w->oct.octa_length / 5 + 3;
  letter_list[RIGHT].x = -3 * w->oct.octa_length / 5 - 4;
  letter_list[RIGHT].y = 3;
  letter_list[BOTTOM].x = -2;
  letter_list[BOTTOM].y = -3 * w->oct.octa_length / 5 + 2;
  letter_list[LEFT].x = 3 * w->oct.octa_length / 5 - 1;
  letter_list[LEFT].y = 3;
  w->oct.orient_line_length = w->oct.octa_length / 4;
}

static int position_polyhedrons(w, x, y, style, view, face, i, j, k, direction)
  OctWidget w;
  int x, y, style;
  int *view, *face, *i, *j, *k, *direction;
{
  x -= w->oct.puzzle_offset.x;
  y -= w->oct.puzzle_offset.y;
  if (w->oct.vertical && y > w->oct.view_length - 1) {
    y -= (w->oct.view_length - 1);
    *view = DOWN;
  } else if (!w->oct.vertical && x > w->oct.view_length - 1) {
    x -= (w->oct.view_length - 1);
    *view = DOWN;
  } else
    *view = UP;
  if (x <= 0 || y <= 0 ||
      x >= w->oct.face_length + w->oct.delta ||
      y >= w->oct.face_length + w->oct.delta)
    return FALSE;
  else if (x + y > w->oct.face_length) {
    if (x > y)
      *face = RIGHT;
    else if (x < y)
      *face = BOTTOM;
    else
      return FALSE;
  } else {
    if (x > y)
      *face = TOP;
    else if (x < y)
      *face = LEFT;
    else
      return FALSE;
  }
  *k = 0;
  while ((x <= (w->oct.size - (*k + 1)) *
            (w->oct.octa_length + w->oct.delta) ||
          x >= w->oct.view_middle + (*k + 1) *
            (w->oct.octa_length + w->oct.delta) + 1 ||
          y <= (w->oct.size - (*k + 1)) *
            (w->oct.octa_length + w->oct.delta) ||
          y >= w->oct.view_middle + (*k + 1) *
            (w->oct.octa_length + w->oct.delta) + 1) &&
         *k < w->oct.size)
    (*k)++;
  *i = 0;
  while ((x + y) / 2 <= (w->oct.size - (*i + 1)) *
           (w->oct.octa_length + w->oct.delta) + 2 * w->oct.delta ||
         (x + y) / 2 >= w->oct.view_middle + (*i + 1) *
           (w->oct.octa_length + w->oct.delta))
    (*i)++;
  *j = 0;
  while (x <= y - 2 * (*j + 1) * (w->oct.octa_length + w->oct.delta) - 2 ||
         y <= x - 2 * (*j + 1) * (w->oct.octa_length + w->oct.delta) - 2)
    (*j)++;
  if (!w->oct.vertical && *view == DOWN) {
    *face = (*face + MAXSIDES / 2) % MAXSIDES;
    if (*direction < MAXSIDES)
      *direction = (*direction + MAXSIDES / 2) % MAXSIDES;
    else if (*direction < 2 * MAXSIDES)
      *direction = (*direction + MAXSIDES / 2) % MAXSIDES + MAXSIDES;
  }
  if (*direction <= LEFT && (*direction + *face) % 2 == 0)
    return FALSE;
  if (style == PERIOD3) {
    if (*direction == CW || *direction == CCW) /* Remap to row movement */ {
      *face = (*face + 2) % MAXSIDES;
      *direction = (*face + *direction) % MAXSIDES;
      *k = 0;
    }
  } else /* style == PERIOD4 */ {
    if (*direction < MAXSIDES) {
      if (*direction == (*face + 1) % MAXSIDES)
        *direction = CW;
      else /* *direction == (*face + 3) % MAXSIDES */
        *direction = CCW;
    }
/*    if (*direction > TL)
      h = *k;
    else if (*direction % 2 == 0)
      h = *i;
    else *//* *direction % 2 == 1 *//*
      h = *j;*/
  }
  return TRUE;
}

static void no_move_polyhedrons(w)
  OctWidget w;
{
  octCallbackStruct cb;
 
  cb.reason = OCT_IGNORE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void practice_polyhedrons(w)
  OctWidget w;
{
  octCallbackStruct cb;

  cb.reason = OCT_PRACTICE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}

static void randomize_polyhedrons(w)
  OctWidget w;
{
  octCallbackStruct cb;
  int i, j, k, random_direction, face, view, position, style;
  int big = w->oct.size * w->oct.size * 3 + NRAND(2);

  if (w->oct.practice)
    practice_polyhedrons(w);
  if (w->oct.sticky)
    big /= 3;
  cb.reason = OCT_RESET;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
 
#ifdef DEBUG
  big = 3;
#endif

  while (big--) {
    view = NRAND(MAXVIEWS);
    face = NRAND(MAXSIDES);
    if (w->oct.mode == BOTH)
      style = NRAND(MAXMODES - 1) + PERIOD3;
    else
      style = w->oct.mode;
    if (w->oct.sticky) {
      if (style == PERIOD3) {
        i = j = 1;
        k = 2;
        random_direction = NRAND(6);
        if (random_direction == 4)
          random_direction = CW;
        else if (random_direction == 5)
          random_direction = CCW;
        else
          random_direction += 4;
      } else /* style == PERIOD4 */ {
        if (NRAND(2)) /* a point */ {
          i = k = w->oct.size - 1;
          j = 0;
        } else /* the center */ {
          k = 2;
          i = j = 1;
        }
        random_direction = NRAND(6);
        if (random_direction == 4)
          random_direction = CW;
        else if (random_direction == 5)
          random_direction = CCW;
        else {
          random_direction += MAXSIDES;
          i = k = 0;
        }
      }
    } else /* (!w->oct.sticky) */ {
      if (style == PERIOD3) {
        random_direction = NRAND(MAXORIENT / 2) + 2;
        if (random_direction < MAXSIDES) {
          if (face % 2 == 0 && random_direction == BOTTOM)
            random_direction = RIGHT;
          else if (face % 2 == 1 && random_direction == LEFT)
            random_direction = TOP;
        }
      } else /* style == PERIOD4 */ {
        random_direction = NRAND(MAXORIENT / 2);
        if (random_direction == 4)
          random_direction = CW;
        else if (random_direction == 5)
          random_direction = CCW;
        else
          random_direction += MAXSIDES;
      }
      position = NRAND(w->oct.size_size);
      k = Sqrt(position);
      j = (position - k * k) / 2;
      i = ((k + 1) * (k + 1) - position - 1) / 2;
    }
    move_oct(w, view, face, i, j, k, random_direction, style, FALSE);
    cb.reason = OCT_MOVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
  flush_moves(w);
  cb.reason = OCT_RANDOMIZE;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  if (check_solved(w)) {
    cb.reason = OCT_SOLVED;
    XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  }
}

static void move_polyhedrons(w, view, face, i, j, k, direction, style)
  OctWidget w;
  int view, face, i, j, k, direction, style;
{
  int orient, new_view, rotate, g, h, len;
  int new_face, old_face, new_direction, bound, l = 0;

  if (style == PERIOD3) {
    if ((k == 0 && direction < MAXSIDES) ||
        (j == 0 && direction >= MAXSIDES && direction % 2 == 1) ||
        (i == 0 && direction >= MAXSIDES && direction % 2 == 0))
      rotate_face(w, view, row_to_rotate[0][face][direction].face,
                   row_to_rotate[0][face][direction].rotation);
    if ((k == w->oct.size - 1 && direction < MAXSIDES) ||
        (j == w->oct.size - 1 && direction >= MAXSIDES && direction % 2 == 1) ||
        (i == w->oct.size - 1 && direction >= MAXSIDES && direction % 2 == 0))
      rotate_face(w, !view, row_to_rotate[1][face][direction].face,
                   row_to_rotate[1][face][direction].rotation);
    if (direction < MAXSIDES)
      h = k;
    else if (direction % 2 == 0)
      h = i;
    else /* direction % 2 == 1 */
      h = j;
    if (w->oct.sticky && (h == 1 || h == 2)) {
      l = 0;
      bound = TRUE;
      h = 1;
    } else
      bound = FALSE;
    new_view = view;
    new_face = face;
    new_direction = direction;
    do {
      len = length(w, direction, h);
      rotate = rotate_orient_p3[face][direction];
      read_rcd(w, view, face, direction, h, len, 0);
      if (reverse_p3[face][direction])
        reverse_rcd(w, len, 0);
      rotate_rcd(w, rotate, len, 0);
      for (orient = 1; orient < 8; orient++) {
        if (slide_next_row_p3[face][direction].view_changed) {
           view = !view;
           h = w->oct.size - 1 - h;
        }
        old_face = slide_next_row_p3[face][direction].face;
        direction = slide_next_row_p3[face][direction].direction;
        face = old_face;
        len = length(w, direction, h);
        rotate = rotate_orient_p3[face][direction];
        if (orient < 6) {
          read_rcd(w, view, face, direction, h, len, orient);
          if (reverse_p3[face][direction])
            reverse_rcd(w, len, orient);
          rotate_rcd(w, rotate, len, orient);
        }
        if (orient >= 2)
          write_rcd(w, view, face, direction, h, len, orient - 2);
      }
      l++;
      h = 2;
      view = new_view;
      face = new_face;
      direction = new_direction;
    } while (bound && l < 2);
  } else /* style == PERIOD4 */ {
    if (direction > TL)
      h = k;
    else if (direction % 2 == 0)
      h = i;
    else /* direction % 2 == 1 */
      h = j;
  
    if (w->oct.sticky &&
         !((direction > TL && h == w->oct.size - 1) || 
         (direction <= TL && h == 0))) {
      l = 0;
      h = (direction <= TL);
      bound = TRUE;
    } else
      bound = FALSE;
    g = 0;
    do /* In case this is on an edge */ {
      len = length(w, direction, h);
      if (g == 1) {
        if (direction > TL) {
          direction = (direction == CW) ? CCW : CW;
          view = !view;
        } else
          face = (face + 2) % MAXSIDES;
      }
      read_rcd(w, view, face, direction, h, len, 0);
      for (orient = 1; orient <= 4; orient++) {
        if (direction <= TL) {
          if ((face - direction + COORD) % MAXSIDES < 2) {
            new_view = !view;
            new_face = (face % 2 == 0) ? (face + 2) % MAXSIDES : face;
            new_direction = (direction % 2 == 0) ?
              MAXSIDES + (direction + 3) % MAXSIDES:
              MAXSIDES + (direction + 1) % MAXSIDES;
            if (face % 2 == 0)
              rotate = ((new_direction - direction + MAXSIDES) % 
                MAXSIDES == 1) ? 2 : MAXORIENT - 2;
            else
              rotate = ((new_direction - direction + MAXSIDES) %
                MAXSIDES == 1) ?  4 : MAXORIENT - 4;
          } else /* Next is on same view */ {
            new_view = view;
            new_face = MAXSIDES - face - 1;
            if (direction % 2 == 1) 
              new_face = (new_face + 2) % MAXSIDES;
            new_direction = direction;
            rotate = ((face - new_face + MAXSIDES) % MAXSIDES == 1) ?
              1 : MAXORIENT - 1;
          }
        } else /* direction == CW || direction == CCW */ {
          new_view = view;
          new_face = (face + direction) % MAXSIDES;
          new_direction = direction;
          rotate = 3 * new_direction;
        }
        if (orient != 4)
          read_rcd(w, new_view, new_face, new_direction, h, len, orient);
        rotate_rcd(w, rotate, len, orient - 1);
        reverse_rcd(w, len, orient - 1);
        write_rcd(w, new_view, new_face, new_direction, h, len, orient - 1);
        view = new_view;
        face = new_face;
        direction = new_direction;
      }
      l++;
      if (w->oct.sticky &&
          !((direction > TL && h == w->oct.size - 1) ||
          (direction <= TL && h == 0)))
        h++;
      else
        g++;
   } while ((bound && l < w->oct.size - 1) ||
            (((direction > TL && h == w->oct.size - 1) ||
              (direction <= TL && h == 0)) && g < 2 && !bound));
  }
}

static void rotate_face(w, view, face, direction)
  OctWidget w;
  int view, face, direction;
{
  int g, square, s;
  int i = 0, j = 0, k, i1, j1, k1, position;

  /* Read Face */
  k = -1;
  square = 0;
  for (g = 0; g < w->oct.size_size; g++) {
    /* This is the old algorithm, its now more efficient
    k = Sqrt(g);
    j = (g - k * k) / 2;
    i = ((k + 1) * (k + 1) - g - 1) / 2;*/
    if (square <= g) {
      k++;
      square = (k + 1) * (k + 1);
      j = -1;
      i = k;
    }
    if ((square - g) % 2 == 0)
      i--;
    else
      j++;
    if (((direction == CW) + face) % 2 == 0) {
      k1 = w->oct.size - 1 - i;
      i1 = j;
      j1 = w->oct.size - 1 - k;
    } else /* ((direction == CCW + face) % 2 == 0 */ {
      k1 = w->oct.size - 1 - j;
      j1 = i;
      i1 = w->oct.size - 1 - k;
    }
    position = (j1 != k1 - i1) ? k1 * k1 + 2 * j1 + 1 : k1 * k1 + 2 * j1;
    w->oct.face_loc[position] = w->oct.octa_loc[view * MAXSIDES + face][g];
  }
  /* Write Face */
  square = 1;
  s = 0;
  for (g = 0; g < w->oct.size_size; g++) {
    w->oct.octa_loc[view * MAXSIDES + face][g] = w->oct.face_loc[g];
    w->oct.octa_loc[view * MAXSIDES + face][g].rotation = (direction == CW) ?
      (w->oct.octa_loc[view * MAXSIDES + face][g].rotation + 4) % MAXORIENT :
      (w->oct.octa_loc[view * MAXSIDES + face][g].rotation + 8) % MAXORIENT;
    draw_triangle(w, view, face, g);
    s = !s;
    if (g == square * square - 1) {
      s = 0;
      ++square;
    }
  }
}

static void read_rcd(w, view, face, dir, h, len, orient)
  OctWidget w;
  int view, face, dir, h, len, orient;
{
  int f, g, s;
  int base = h * h;

  if (dir < MAXSIDES || dir > COORD)
    for (g = 0; g < len; g++)
      w->oct.row_loc[orient][g] =
        w->oct.octa_loc[view * MAXSIDES + face][base + g];
  else if (dir % 2 == 0) {
    f = -1;
    for (g = 0; g < len; g++) {
      s = g % 2;
      w->oct.row_loc[orient][g] =
        w->oct.octa_loc[view * MAXSIDES + face][base + f + !s];
      if (s == SAME)
        f += g + 2 * (h + 1) + 1;
    }
  } else /* dir % 2 == 1 */ {
    base += 2 * h;
    f = 1;
    for (g = 0; g < len; g++) {
      s = g % 2;
      w->oct.row_loc[orient][g] =
        w->oct.octa_loc[view * MAXSIDES + face][base + f - !s];
      if (s == SAME)
        f += g + 2 * h + 1;
    }
  }
}

static void rotate_rcd(w, rotate, len, orient)
  OctWidget w;
  int rotate, len, orient;
{
  int g;

  for (g = 0; g < len; g++)
    w->oct.row_loc[orient][g].rotation =
      (w->oct.row_loc[orient][g].rotation + rotate) % MAXORIENT;
}

static void reverse_rcd(w, len, orient)
  OctWidget w;
  int len, orient;
{
  int g;
  OctLoc Temp[MAXOCTAS_SQ];

  for (g = 0; g < len; g++)
    Temp[g] = w->oct.row_loc[orient][g];
  for (g = 0; g < len; g++)
    w->oct.row_loc[orient][g] = Temp[len - 1 - g];
}

static void write_rcd(w, view, face, dir, h, len, orient)
  OctWidget w;
  int view, face, dir, h, len, orient;
{
  int f, g, s;
  int base = h * h;

  if (dir < MAXSIDES || dir > COORD) /* CW || CCW */ {
    for (g = 0; g < len; g++) {
      s = g % 2;
      w->oct.octa_loc[view * MAXSIDES + face][base + g] =
        w->oct.row_loc[orient][g];
      draw_triangle(w, view, face, base + g);
    }
  } else if (dir % 2 == 0) /* TR || BL */ {
    f = -1;
    for (g = 0; g < len; g++) {
      s = g % 2;
      w->oct.octa_loc[view * MAXSIDES + face][base + f + !s] =
        w->oct.row_loc[orient][g];
      draw_triangle(w, view, face, base + f + !s);
      if (s == SAME)
        f += g + 2 * (h + 1) + 1;
    }
  } else /* dir % 2 == 1 */ /* BR || TL */ {
    base += 2 * h;
    f = 1;
    for (g = 0; g < len; g++) {
      s = g % 2;
      w->oct.octa_loc[view * MAXSIDES + face][base + f - !s] =
        w->oct.row_loc[orient][g];
      draw_triangle(w, view, face, base + f - !s);
      if (s == SAME)
        f += g + 2 * h + 1;
    }
  }
}

static void draw_frame(w, gc)
  OctWidget w;
  GC gc;
{
  int startx, starty, lengthx, lengthy, longlength;

  startx = 1 + w->oct.puzzle_offset.x;
  starty = 1 + w->oct.puzzle_offset.y;
  lengthx = w->oct.view_length - w->oct.delta + w->oct.puzzle_offset.x;
  lengthy = w->oct.view_length - w->oct.delta + w->oct.puzzle_offset.y;
  XDrawLine(XtDisplay(w), XtWindow(w), gc, startx, starty, lengthx, starty);
  XDrawLine(XtDisplay(w), XtWindow(w), gc, startx, starty, startx, lengthy);
  XDrawLine(XtDisplay(w), XtWindow(w), gc, lengthx, starty, lengthx, lengthy);
  XDrawLine(XtDisplay(w), XtWindow(w), gc, startx, lengthy, lengthx, lengthy);
  XDrawLine(XtDisplay(w), XtWindow(w), gc, startx, lengthy, lengthx, starty);
  XDrawLine(XtDisplay(w), XtWindow(w), gc, startx, starty, lengthx, lengthy);
  if (w->oct.vertical) {
    longlength = 2 * w->oct.view_length - 2 * w->oct.delta - 1 +
      w->oct.puzzle_offset.y;
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      startx, lengthy, startx, longlength);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      lengthx, lengthy, lengthx, longlength);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      startx, longlength, lengthx, longlength);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      startx, longlength, lengthx, lengthy);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      startx, lengthy, lengthx, longlength);
  } else {
    longlength = 2 * w->oct.view_length - 2 * w->oct.delta - 1 +
      w->oct.puzzle_offset.x;
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      lengthx, starty, longlength, starty);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      lengthx, lengthy, longlength, lengthy);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      longlength, starty, longlength, lengthy);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      longlength, starty, lengthx, lengthy);
    XDrawLine(XtDisplay(w), XtWindow(w), gc,
      lengthx, starty, longlength, lengthy);
  }
}   

void draw_all_polyhedrons(w)
  OctWidget w;
{
  int i, view, face;

  for (view = DOWN; view <= UP; view++)
    for (face = 0; face < MAXSIDES; face++)
      for (i = 0; i < w->oct.size_size; i++)
        draw_triangle (w, view, face, i);
}

static void draw_triangle(w, view, face, k)
  OctWidget w;
  int view, face, k;
{
  int dx = 0, dy = 0;
  int side, h = Sqrt(k);
  int base = h * h;
  int g = k - base;
  int s = g % 2;
  int pos =  h * (w->oct.octa_length + w->oct.delta);
  int orient = (!w->oct.vertical && view == DOWN) ?
    (face + MAXSIDES / 2) % MAXSIDES : face;

  switch (orient) {
    case TOP:
      dy = w->oct.view_middle - w->oct.delta - 1 - pos;
      dx = w->oct.view_middle - pos + g * (w->oct.octa_length + w->oct.delta);
      break;
    case LEFT:
      dx = w->oct.view_middle - w->oct.delta - 1 - pos;
      dy = w->oct.view_middle - pos - 1 +
        g * (w->oct.octa_length + w->oct.delta);
      break;
    case BOTTOM:
      dy = w->oct.view_middle + w->oct.delta + pos;
      dx = w->oct.view_middle + pos - g * (w->oct.octa_length + w->oct.delta);
      break;
    case RIGHT:
      dx = w->oct.view_middle + w->oct.delta + pos;
      dy = w->oct.view_middle + pos - 1 -
        g * (w->oct.octa_length + w->oct.delta);
      break;
    default:
      (void) printf ("draw_triangle: orient %d\n", orient);
  }
  if (face == TOP || face == BOTTOM)
    side = (face == TOP) ? !s * BOTTOM : s * BOTTOM;
  else /* face == RIGHT || face == LEFT */
    side = (face == LEFT) ? s * BOTTOM + 1 : !s * BOTTOM + 1;
  side = (!w->oct.vertical && view == DOWN) ?
    (side + MAXSIDES / 2) % MAXSIDES : side;
  if (s == OPPOSITE)
    switch (side) {
      case TOP:
        dy -= w->oct.octa_length;
        break;
      case LEFT:
        dx -= w->oct.octa_length;
        break;
      case BOTTOM:
        dy += w->oct.octa_length;
        break;
      case RIGHT:
        dx += w->oct.octa_length;
        break;
      default:
        (void) printf ("draw_triangle: side %d\n", side);
     }
  dx += w->oct.puzzle_offset.x;
  dy += w->oct.puzzle_offset.y;
  if (view == DOWN) {
    if (w->oct.vertical)
      dy += w->oct.view_length - w->oct.delta - 1;
    else
      dx += w->oct.view_length - w->oct.delta - 1;
  }
  triangle_list[side][0].x = dx;
  triangle_list[side][0].y = dy;
  XFillPolygon(XtDisplay(w), XtWindow(w),
    w->oct.face_GC[w->oct.octa_loc[view * MAXSIDES + face][k].face],
    triangle_list[side], 3, Convex, CoordModePrevious);
  if (w->oct.depth == 1 || w->oct.mono) {
    int letter_x, letter_y;
    char buf[2];

    (void) sprintf(buf, "%c",
             w->oct.face_name[w->oct.octa_loc[view * MAXSIDES + face][k].face][0]);
    letter_x = dx + letter_list[side].x;
    letter_y = dy + letter_list[side].y;
    XDrawString(XtDisplay(w), XtWindow(w), w->oct.inverse_GC,
      letter_x, letter_y, buf, 1);
  }
  if (w->oct.orient)
    draw_orient_line(w, w->oct.octa_loc[view * MAXSIDES + face][k].rotation,
      face, side, dx, dy);
}

static void draw_orient_line(w, orient, face, side, dx, dy)
  OctWidget w;
  int orient, face, side, dx, dy;
{
  int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
  int temp1 = w->oct.octa_length + 1;
  int temp2 = w->oct.octa_length / 2 + 1;
  int temp3 = w->oct.orient_line_length / 3;

  /* clock positions */
  switch ((side == face) ? orient : (orient + MAXORIENT / 2) % MAXORIENT) {
    case 0:
      x2 = x1 = dx;
      y1 = dy + temp1;
      y2 = y1 - w->oct.orient_line_length;
      break;
    case 1:
      x1 = dx + temp2;
      y1 = dy + temp2;
      x2 = x1 + temp3;
      y2 = y1 - w->oct.orient_line_length;
      break;
    case 2:
      x1 = dx - temp2;
      y1 = dy - temp2;
      x2 = x1 + w->oct.orient_line_length;
      y2 = y1 - temp3;
      break;
    case 3:
      x1 = dx - temp1;
      x2 = x1 + w->oct.orient_line_length;
      y2 = y1 = dy;
      break;
    case 4:
      x1 = dx - temp2;
      y1 = dy + temp2;
      x2 = x1 + w->oct.orient_line_length;
      y2 = y1 + temp3;
      break;
    case 5:
      x1 = dx + temp2;
      y1 = dy - temp2;
      x2 = x1 + temp3;
      y2 = y1 + w->oct.orient_line_length;
      break;
    case 6:
      x2 = x1 = dx;
      y1 = dy - temp1;
      y2 = y1 + w->oct.orient_line_length;
      break;
    case 7:
      x1 = dx - temp2;
      y1 = dy - temp2;
      x2 = x1 - temp3;
      y2 = y1 + w->oct.orient_line_length;
      break;
    case 8:
      x1 = dx + temp2;
      y1 = dy + temp2;
      x2 = x1 - w->oct.orient_line_length;
      y2 = y1 + temp3;
      break;
    case 9:
      x1 = dx + temp1;
      x2 = x1 - w->oct.orient_line_length;
      y2 = y1 = dy;
      break;
    case 10:
      x1 = dx + temp2;
      y1 = dy - temp2;
      x2 = x1 - w->oct.orient_line_length;
      y2 = y1 - temp3;
      break;
    case 11:
      x1 = dx - temp2;
      y1 = dy + temp2;
      x2 = x1 - temp3;
      y2 = y1 - w->oct.orient_line_length;
      break;
    default:
      (void) printf("draw_orient_line: orient %d\n", orient);
  }
  XDrawLine(XtDisplay(w), XtWindow(w), w->oct.inverse_GC, x1, y1, x2, y2);
}

Boolean check_solved(w)
  OctWidget w;
{
  int face, i;
  OctLoc test;

  for (face = 0; face < MAXFACES; face++)
    for (i = 0; i < w->oct.size_size; i++) {
      if (i == 0) {
        test.face = w->oct.octa_loc[face][i].face;
        test.rotation = w->oct.octa_loc[face][i].rotation;
      } else if (test.face != /*MAXSIDES * view + face*/
                 w->oct.octa_loc[face][i].face ||
                 (w->oct.orient && test.rotation != /*3 * face*/
                  w->oct.octa_loc[face][i].rotation))
        return FALSE;
  }
  return TRUE;
}

static int length(w, dir, h)
  OctWidget w;
  int dir, h;
{
  if (dir < MAXSIDES || dir > COORD)
    return (2 * h + 1);
  else 
    return (2 * (w->oct.size - h) - 1);
}

static int Sqrt(x)
  int x;
{
  int i = 0;
  while (i * i <= x)
    i++;
  return (i - 1);
}

#ifdef DEBUG

static void print_octa(w)
  OctWidget w;
{
  int i, face, view, square;

  for (view = DOWN; view <= UP; view++) {
    for (face = 0; face < MAXSIDES; face++) {
      square = 1;
      for (i = 0; i < w->oct.size_size; i++) {
         (void) printf("%d %d  ", w->oct.octa_loc[!view * MAXSIDES + face][i].face,
                  w->oct.octa_loc[!view * MAXSIDES + face][i].rotation);
         if (i == square * square - 1) {
           (void) printf("\n");
           ++square;
         }
      }
      (void) printf("\n");
    }
    (void) printf("\n");
  }
  (void) printf("\n");
}

static void print_face(w)
  OctWidget w;
{
  int i;
  int square = 1;

  for (i = 0; i < w->oct.size_size; i++) {
    (void) printf("%d %d  ",
             w->oct.face_loc[i].face, w->oct.face_loc[i].rotation);
    if (i == square * square - 1) {
      (void) printf("\n");
      ++square;
    }
  }
  (void) printf("\n");
}

static void print_row(w, len, orient)
  OctWidget w;
  int len, orient;
{
  int i;

  (void) printf("Row %d:\n", orient);
  for (i = 0; i < len; i++)
    (void) printf("%d %d  ", w->oct.row_loc[orient][i].face,
            w->oct.row_loc[orient][i].rotation);
  (void) printf("\n");
}

#endif
