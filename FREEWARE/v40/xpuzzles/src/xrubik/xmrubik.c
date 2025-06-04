/*
# MOTIF-BASED RUBIK'S CUBE(tm)
#
#  xmrubik.c
#
###
#
#  Copyright (c) 1993 - 95	David Albert Bagley, bagleyd@source.asset.com
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
#  This program is distributed in the hope that it will be "useful",
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
*/

/*
  Version 5: 95/10/06 Xt/Motif
  Version 4: 94/04/07 Xt
  Version 3: 93/05/20 Motif
  Version 2: 92/01/16 XView
  Version 1: 91/01/16 SunView
*/

#include <stdlib.h>
#include <stdio.h>
#ifdef VMS
#include <unixlib.h>
#define getlogin cuserid
#else
#ifndef apollo
#include <unistd.h>
#endif
#endif
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>
#include <Xm/PanedW.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Scale.h>
#include <Xm/ToggleB.h>
#include "Rubik.h"
#include "Rubik2d.h"
#include "Rubik3d.h"
#include "rubik.xbm"
#include "mouse-l.xbm"
#include "mouse-m.xbm"
#include "mouse-r.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/mball.scores"
#endif

/* The following are in RubikP.h also */
#define MINCUBES 1
#define MAXCUBES 6
#define MAXFACES 6

#define MAXRECORD 32767

static void initialize();
static void moves_text();

static void print_record();
static int handle_solved();
static void read_records();
static void write_records();

static void cubes_slider();
static void orient_toggle();
static void wprintf();

static Arg arg[5];
static Widget moves, record, message, rubik2d, rubik3d, orient_switch, cubes;
static int rubik_record[2][MAXCUBES - MINCUBES + 1], moves_dsp = 0;
static char message_dsp[128] = "Randomize to start";

static void usage()
{
  (void) fprintf(stderr, "usage: xmrubik\n");
  (void) fprintf(stderr,
    "\t[-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]]\n");
  (void) fprintf(stderr,
    "\t[-display [{host}]:[{vs}]][-fg {color}] [-bg {color}]\n");
  (void) fprintf(stderr,
    "\t[-size {int}] [-[no]orient] [-mono] [-face{0|1|2|3|4|5} {color}]\n");
  exit(1);
}

static XrmOptionDescRec options[] = {
  {"-fg",		"rubik.Foreground",	XrmoptionSepArg,	NULL},
  {"-bd",		"*rubik.Foreground",	XrmoptionSepArg,	NULL},
  {"-bg",		"*Background",		XrmoptionSepArg,	NULL},
  {"-foreground",	"rubik.Foreground",	XrmoptionSepArg,	NULL},
  {"-background",	"*Background",		XrmoptionSepArg,	NULL},
  {"-bordercolor",	"*rubik.Foreground",	XrmoptionSepArg,	NULL},
  {"-size",		"*rubik.size",		XrmoptionSepArg,	NULL},
  {"-orient",		"*rubik.orient",	XrmoptionNoArg,		"TRUE"},
  {"-noorient",		"*rubik.orient",	XrmoptionNoArg,		"FALSE"},
  {"-mono",		"*rubik.mono",		XrmoptionNoArg,		"TRUE"},
  {"-face0",		"*rubik.faceColor0",	XrmoptionSepArg,	NULL},
  {"-face1",		"*rubik.faceColor1",	XrmoptionSepArg,	NULL},
  {"-face2",		"*rubik.faceColor2",	XrmoptionSepArg,	NULL},
  {"-face3",		"*rubik.faceColor3",	XrmoptionSepArg,	NULL},
  {"-face4",		"*rubik.faceColor4",	XrmoptionSepArg,	NULL},
  {"-face5",		"*rubik.faceColor5",	XrmoptionSepArg,	NULL},
};

int main(argc, argv)
  int argc;
  char *argv[];
{
  Widget toplevel;
  Widget panel, panel2, rowcol, rowcol2;
  Pixmap mouse_left_cursor, mouse_middle_cursor, mouse_right_cursor;
  Pixel fg, bg;

  toplevel = XtInitialize(argv[0], "Rubik",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) rubik_bits, rubik_width, rubik_height));
  XtSetArg(arg[1], XmNkeyboardFocusPolicy, XmPOINTER); /* not XmEXPLICIT */  
  XtSetValues(toplevel, arg, 2);  
  panel = XtCreateManagedWidget("panel", xmPanedWindowWidgetClass, toplevel,
    NULL, 0);
  panel2 = XtVaCreateManagedWidget("panel2", xmPanedWindowWidgetClass, panel,
    XmNseparatorOn, False,
    XmNsashWidth, 1,
    XmNsashHeight, 1,
    NULL);

  rowcol = XtVaCreateManagedWidget("Rowcol", xmRowColumnWidgetClass, panel2,
    XmNnumColumns, 2,
    XmNorientation, XmHORIZONTAL,
    XmNpacking, XmPACK_COLUMN,
    NULL);
  XtVaGetValues(rowcol, XmNforeground, &fg, XmNbackground, &bg, NULL);
  mouse_left_cursor = XCreatePixmapFromBitmapData(XtDisplay(rowcol),
    RootWindowOfScreen(XtScreen(rowcol)), mouse_left_bits,
    mouse_left_width, mouse_left_height, fg, bg,
    DefaultDepthOfScreen(XtScreen(rowcol)));
  mouse_middle_cursor = XCreatePixmapFromBitmapData(XtDisplay(rowcol),
    RootWindowOfScreen(XtScreen(rowcol)), mouse_middle_bits,
    mouse_middle_width, mouse_middle_height, fg, bg,
    DefaultDepthOfScreen(XtScreen(rowcol)));
  mouse_right_cursor = XCreatePixmapFromBitmapData(XtDisplay(rowcol),
    RootWindowOfScreen(XtScreen(rowcol)), mouse_right_bits,
    mouse_right_width, mouse_right_height, fg, bg,
    DefaultDepthOfScreen(XtScreen(rowcol)));
  XtVaCreateManagedWidget("mouse_left_text", xmLabelGadgetClass, rowcol,
    XtVaTypedArg, XmNlabelString, XmRString, "CCW", 4, NULL);
  XtVaCreateManagedWidget("mouse_left", xmLabelGadgetClass, rowcol,
    XmNlabelType, XmPIXMAP, XmNlabelPixmap, mouse_left_cursor, NULL);
  XtVaCreateManagedWidget("mouse_middle_text", xmLabelGadgetClass, rowcol,
    XtVaTypedArg, XmNlabelString, XmRString, "Practice", 9, NULL);
  XtVaCreateManagedWidget("mouse_middle", xmLabelGadgetClass, rowcol,
    XmNlabelType, XmPIXMAP, XmNlabelPixmap, mouse_middle_cursor, NULL);
  XtVaCreateManagedWidget("mouse_right_text", xmLabelGadgetClass, rowcol,
    XtVaTypedArg, XmNlabelString, XmRString, "Randomize", 10, NULL);
  XtVaCreateManagedWidget("mouse_right", xmLabelGadgetClass, rowcol,
    XmNlabelType, XmPIXMAP, XmNlabelPixmap, mouse_right_cursor, NULL);
  XtVaCreateManagedWidget("moves_text", xmLabelGadgetClass, rowcol,
    XtVaTypedArg, XmNlabelString, XmRString, "Moves", 6, NULL);
  moves = XtVaCreateManagedWidget("0", xmLabelWidgetClass, rowcol, NULL);
  XtVaCreateManagedWidget("record_text", xmLabelGadgetClass, rowcol,
    XtVaTypedArg, XmNlabelString, XmRString, "Record", 7, NULL);
  record = XtVaCreateManagedWidget("0", xmLabelWidgetClass, rowcol, NULL);
  XtVaCreateManagedWidget("null", xmLabelGadgetClass, rowcol,
    XtVaTypedArg, XmNlabelString, XmRString, "", 1, NULL);

  rowcol2 = XtVaCreateManagedWidget("Rowcol2", xmRowColumnWidgetClass, panel2,
    NULL);
  XtVaGetValues(rowcol2, XmNforeground, &fg, XmNbackground, &bg, NULL);
  cubes = XtVaCreateManagedWidget("cubes", xmScaleWidgetClass, rowcol2,
    XtVaTypedArg, XmNtitleString, XmRString, "Cubes", 6,
    XmNminimum, MINCUBES,
    XmNmaximum, MAXCUBES,
    XmNvalue, MINCUBES,
    XmNshowValue, True,
    XmNorientation, XmHORIZONTAL,
    NULL);
  XtAddCallback(cubes, XmNvalueChangedCallback, cubes_slider, NULL);
  orient_switch = XtVaCreateManagedWidget ("Orient",
    xmToggleButtonWidgetClass, rowcol2,
    NULL);
  XtAddCallback(orient_switch, XmNvalueChangedCallback, orient_toggle, NULL);
  message = XtVaCreateManagedWidget("Play Rubik's Cube! (use mouse and keypad)",
    xmLabelWidgetClass, rowcol2,
    NULL);

  rubik2d = XtCreateManagedWidget("rubik", rubik2dWidgetClass, panel,
    NULL, 0);
  XtVaSetValues(rubik2d,
    XtNheight, 200,
    NULL);
  XtAddCallback(rubik2d, XtNselectCallback, moves_text, NULL);
  rubik3d = XtCreateManagedWidget("rubik", rubik3dWidgetClass, panel,
    NULL, 0);
  XtVaSetValues(rubik3d,
    XtNheight, 200,
    NULL);
  XtAddCallback(rubik3d, XtNselectCallback, moves_text, NULL);
  initialize();
  XtRealizeWidget(toplevel);
  XGrabButton(XtDisplay(rubik2d), AnyButton, AnyModifier, XtWindow(rubik2d),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(rubik2d),
    XCreateFontCursor(XtDisplay(rubik2d), XC_crosshair));
  XGrabButton(XtDisplay(rubik3d), AnyButton, AnyModifier, XtWindow(rubik3d),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(rubik3d),
    XCreateFontCursor(XtDisplay(rubik3d), XC_crosshair));
  XtMainLoop();

#ifdef VMS
  return 1;
#else
  return 0;
#endif
}

/* There's probably a better way to assure that they are the same but
   I don't know it off hand. */
static void make_equivalent(size, orient)
  int *size;
  Boolean *orient;
{
  Boolean mono;
  Pixel foreground, background;
  String facecolor[MAXFACES];

  XtVaGetValues(rubik2d,
    XtNsize, size,
    XtNorient, orient,
    XtNmono, &mono,
    XtNforeground, &foreground,
    XtNbackground, &background,
    XtNfaceColor0, &(facecolor[0]),
    XtNfaceColor1, &(facecolor[1]),
    XtNfaceColor2, &(facecolor[2]),
    XtNfaceColor3, &(facecolor[3]),
    XtNfaceColor4, &(facecolor[4]),
    XtNfaceColor5, &(facecolor[5]),
    NULL);
  XtVaSetValues(rubik2d,
    XtNface, RUBIK_IGNORE,
    XtNpractice, FALSE,
    XtNstart, FALSE,
    NULL);
  XtVaSetValues(rubik3d,
    XtNsize, *size,
    XtNorient, *orient,
    XtNmono, mono,
    XtNface, RUBIK_IGNORE,
    XtNpractice, FALSE,
    XtNstart, FALSE,
    XtNforeground, foreground,
    XtNbackground, background,
    XtNfaceColor0, facecolor[0],
    XtNfaceColor1, facecolor[1],
    XtNfaceColor2, facecolor[2],
    XtNfaceColor3, facecolor[3],
    XtNfaceColor4, facecolor[4],
    XtNfaceColor5, facecolor[5],
    NULL);
}

static void initialize()
{
  int size;
  Boolean orient;

  make_equivalent(&size, &orient);
  XmScaleSetValue(cubes, size);
  XmToggleButtonSetState(orient_switch, orient, True);
  read_records();
  print_record(orient, size);
}

static void moves_text(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  rubikCallbackStruct *call_data;
{
  int size;
  Boolean orient, practice;
  Widget otherw;

  if (w == rubik2d)
    otherw = rubik3d;
  else /* (w == rubik3d) */
    otherw = rubik2d;
  XtVaGetValues(w,
    XtNsize, &size,
    XtNorient, &orient,
    XtNpractice, &practice,
    NULL);
  (void) strcpy(message_dsp, "");
  switch (call_data->reason) {
    case RUBIK_RESTORE:
      XtSetArg(arg[0], XtNface, RUBIK_RESTORE);
      XtSetValues(otherw, arg, 1);
      XtSetValues(w, arg, 1);
      moves_dsp = 0;
      break;
    case RUBIK_RESET:
      moves_dsp = 0;
      break;
    case RUBIK_IGNORE:
      (void) strcpy(message_dsp, "Randomize to start");
      break;
    case RUBIK_MOVED:
      moves_dsp++;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetArg(arg[1], XtNface, call_data->face);
      XtSetArg(arg[2], XtNi, call_data->i);
      XtSetArg(arg[3], XtNj, call_data->j);
      XtSetArg(arg[4], XtNdirection, call_data->direction);
      XtSetValues(otherw, arg, 5);
      XtSetValues(w, arg, 1);
      break;
    case RUBIK_CONTROL:
      XtSetArg(arg[0], XtNface, call_data->face);
      XtSetArg(arg[1], XtNi, call_data->i);
      XtSetArg(arg[2], XtNj, call_data->j);
      XtSetArg(arg[3], XtNdirection, call_data->direction);
      XtSetValues(otherw, arg, 4);
      return;
    case RUBIK_SOLVED:
      if (practice)
        moves_dsp = 0;
      else { 
        if (handle_solved(moves_dsp, size, orient))
          (void) sprintf(message_dsp, "Congratulations %s!!", getlogin());
        else
          (void) strcpy(message_dsp, "Solved!");
      }
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case RUBIK_PRACTICE:
      moves_dsp = 0;
      practice = !practice;
      if (practice)
        wprintf(record, "practice");
      else {
        (void) strcpy(message_dsp, "Randomize to start");
        print_record(orient, size);
      }
      XtSetArg(arg[0], XtNpractice, practice);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      XtSetValues(otherw, arg, 2);
      break;
    case RUBIK_RANDOMIZE:
      moves_dsp = 0;
      XtSetArg(arg[0], XtNpractice, FALSE);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      XtSetValues(otherw, arg, 2);
      break; 
    case RUBIK_DEC:
      moves_dsp = 0;
      size--;
      print_record(orient, size);
      XtSetArg(arg[0], XtNsize, size);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      XmScaleSetValue(cubes, size);
      break;
    case RUBIK_ORIENT:
      moves_dsp = 0;
      orient = !orient;
      print_record(orient, size);
      XtSetArg(arg[0], XtNorient, orient);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      XmToggleButtonSetState(orient_switch, orient, True);
      break;
    case RUBIK_INC:
      moves_dsp = 0;
      size++;
      print_record(orient, size);
      XtSetArg(arg[0], XtNsize, size);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      XmScaleSetValue(cubes, size);
      break;
    case RUBIK_COMPUTED:
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case RUBIK_UNDO:
      moves_dsp--;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetArg(arg[1], XtNface, call_data->face);
      XtSetArg(arg[2], XtNi, call_data->i);
      XtSetArg(arg[3], XtNj, call_data->j);
      XtSetArg(arg[4], XtNdirection, call_data->direction);
      XtSetValues(otherw, arg, 5);
      XtSetValues(w, arg, 1);
      break;
  }
  wprintf(message, "%s", message_dsp);
  wprintf(moves, "%d", moves_dsp);
}

static void cubes_slider(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmScaleCallbackStruct *cbs;
{
  int size = cbs->value, old, orient;

  XtVaGetValues(rubik2d,
    XtNsize, &old,
    XtNorient, &orient,
    NULL);
  if (old != size) {
    XtVaSetValues(rubik2d,
      XtNsize, size,
      NULL);
    XtVaSetValues(rubik3d,
      XtNsize, size,
      NULL);
    moves_dsp = 0;
    wprintf(moves, "%d", moves_dsp);
    print_record(orient, size);
  }
}

static void orient_toggle(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmToggleButtonCallbackStruct *cbs;
{
  int size;
  Boolean orient = cbs->set;

  XtVaGetValues(rubik2d,
    XtNsize, &size,
    NULL);
  XtVaSetValues(rubik2d,
    XtNorient, orient,
    NULL);
  XtVaSetValues(rubik3d,
    XtNorient, orient,
    NULL);
  moves_dsp = 0;
  wprintf(moves, "%d", moves_dsp);
  print_record(orient, size);
}

static void print_record(orient, size)
  Boolean orient;
  int size;
{
  int i = (orient) ? 1 : 0;
  int j = size - MINCUBES;

  if (rubik_record[i][j] >= MAXRECORD)
    wprintf(record, "NEVER");
  else
    wprintf(record, "%d", rubik_record[i][j]);
}

static int handle_solved(counter, size, orient)
  int counter, size;
  Boolean orient;
{
  int i = (orient) ? 1 : 0;
  int j = size - MINCUBES;

  if (counter < rubik_record[i][j]) {
    rubik_record[i][j] = counter;
    if (size < 2 || (orient && rubik_record[i][j] < rubik_record[!i][j]))
      rubik_record[!i][j] = counter;
    write_records();
    print_record(orient, size);
    return TRUE;
  }
  return FALSE;
}

static void read_records()
{
  FILE *fp;
  int i, n, orient;

  for (orient = 0; orient < 2; orient++)
    for (i = 0; i < MAXCUBES - MINCUBES + 1; i++)
      rubik_record[orient][i] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    wprintf(message, "Can not open %s, taking defaults.", SCOREFILE);
  else {
    for (orient = 0; orient < 2; orient++)
      for (i = 0; i < MAXCUBES - MINCUBES + 1; i++) {
        (void) fscanf(fp, "%d", &n);
        rubik_record[orient][i] = n;
      }
    (void) fclose(fp);
  }
}

static void write_records()
{
  FILE *fp;
  int i, orient;

  if ((fp = fopen(SCOREFILE, "w")) == NULL)
    wprintf(message, "Can not write to %s.", SCOREFILE);
  else {
    for (orient = 0; orient < 2; orient++) {
      for (i = 0; i < MAXCUBES - MINCUBES + 1; i++)
        (void) fprintf(fp, "%d ", rubik_record[orient][i]);
      (void) fprintf(fp, "\n");
    }
    (void) fclose(fp);
  }
}

#include <varargs.h>
static void wprintf(va_alist)
  va_dcl
{
  Widget w;
  char *format;
  va_list args;
  char str[1000];
  Arg wargs[10];
  XmString xmstr;

  va_start(args);
  w = va_arg(args, Widget);
  if (!XtIsSubclass(w, xmLabelWidgetClass))
    XtError("wprintf() requires a Label Widget");
  format = va_arg(args, char *);
  (void) vsprintf(str, format, args);
  xmstr = XmStringLtoRCreate(str, XmSTRING_DEFAULT_CHARSET);
  XtSetArg(wargs[0], XmNlabelString, xmstr);
  XtSetValues(w, wargs, 1);
  va_end(args);
}
