/*
# MOTIF-BASED OCTAHEDRON
#
#  xmoct.c
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
  Version 5: 95/10/04 Xt/Motif
  Version 4: 94/05/31 Xt
  Version 3: 93/04/01 Motif
  Version 2: 92/02/01 XView
  Version 1: 91/06/10 SunView
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
#include <Xm/ToggleBG.h>
#include "Oct.h"
#include "oct.xbm"
#include "mouse-l.xbm"
#include "mouse-m.xbm"
#include "mouse-r.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/oct.scores"
#endif
 
/* The following are in OctP.h also */
#define MINOCTAS 1
#define MAXOCTAS 6
#define PERIOD3 3
#define PERIOD4 4
#define BOTH 5
#define MAXMODES 3

#define MAXRECORD 32767

static void initialize();
static void moves_text();

static void print_record();
static int handle_solved();
static void read_records();
static void write_records();
 
static void octa_slider();
static void mode_toggle();
static void orient_toggle();
static void sticky_toggle();
static void wprintf();

static Arg arg[5];
static Widget moves, record, message, oct, orient_switch, sticky_switch,
  modes[MAXMODES], octas;
static int oct_record[MAXMODES][2][MAXOCTAS - MINOCTAS + 2], moves_dsp = 0;
static int oldsize;
static char message_dsp[128] = "Randomize to start";

static char *mode_string[] = {
  "Period 3", "Period 4", "Both"
};
 
static void usage()
{
  (void) fprintf(stderr, "usage: xmoct\n");
  (void) fprintf(stderr,
    "\t[-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]]\n");
  (void) fprintf(stderr,
    "\t[-display [{host}]:[{vs}]][-fg {color}] [-bg {color}]\n");
  (void) fprintf(stderr,
    "\t[-{size {int} | sticky}] [-[no]orient] [-mono]\n");
  (void) fprintf(stderr,
    "\t[-{mode {int} | both}] [-face{0|1|2|3|4|5|6|7} {color}]\n");
  exit(1);
}

static XrmOptionDescRec options[] = {
  {"-fg",		"*oct.Foreground",	XrmoptionSepArg,	NULL},
  {"-bd",		"*oct.Foreground",	XrmoptionSepArg,	NULL},
  {"-bg",		"*Background",		XrmoptionSepArg,	NULL},
  {"-foreground",	"*oct.Foreground",	XrmoptionSepArg,	NULL},
  {"-background",	"*Background",		XrmoptionSepArg,	NULL},
  {"-bordercolor",	"*oct.Foreground",	XrmoptionSepArg,	NULL},
  {"-size",		"*oct.size",		XrmoptionSepArg,	NULL},
  {"-sticky",		"*oct.sticky",		XrmoptionNoArg,		"FALSE"},
  {"-orient",		"*oct.orient",		XrmoptionNoArg,		"TRUE"},
  {"-noorient",		"*oct.orient",		XrmoptionNoArg,		"FALSE"},
  {"-mode",		"*oct.mode",		XrmoptionSepArg,	NULL},
  {"-both",		"*oct.mode",		XrmoptionNoArg,		"4"},
  {"-mono",		"*oct.mono",		XrmoptionNoArg,		"TRUE"},
  {"-face0",		"*oct.faceColor0",	XrmoptionSepArg,	NULL},
  {"-face1",		"*oct.faceColor1",	XrmoptionSepArg,	NULL},
  {"-face2",		"*oct.faceColor2",	XrmoptionSepArg,	NULL},
  {"-face3",		"*oct.faceColor3",	XrmoptionSepArg,	NULL},
  {"-face4",		"*oct.faceColor4",	XrmoptionSepArg,	NULL},
  {"-face5",		"*oct.faceColor5",	XrmoptionSepArg,	NULL},
  {"-face6",		"*oct.faceColor6",	XrmoptionSepArg,	NULL},
  {"-face7",		"*oct.faceColor7",	XrmoptionSepArg,	NULL}
};

int main(argc, argv)
  int argc;
  char *argv[];
{
  Widget toplevel; 
  Widget panel, panel2, rowcol, rowcol2, rowcol3, rowcol4;
  Pixmap mouse_left_cursor, mouse_middle_cursor, mouse_right_cursor;
  Pixel fg, bg;
  int i;

  toplevel = XtInitialize(argv[0], "Oct",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) oct_bits, oct_width, oct_height));
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
  octas = XtVaCreateManagedWidget("octas", xmScaleWidgetClass, rowcol2,
    XtVaTypedArg, XmNtitleString, XmRString, "Octas", 6,
    XmNminimum, MINOCTAS,
    XmNmaximum, MAXOCTAS,
    XmNvalue, MINOCTAS,
    XmNshowValue, True,
    XmNorientation, XmHORIZONTAL,
    NULL);
  XtAddCallback(octas, XmNvalueChangedCallback, octa_slider, NULL);
  rowcol3 = XtVaCreateManagedWidget("Rowcol", xmRowColumnWidgetClass, rowcol2,
    XmNnumColumns, 1,
    XmNorientation, XmHORIZONTAL,
    XmNpacking, XmPACK_COLUMN,
    XmNradioBehavior, True,
    NULL);
  orient_switch = XtVaCreateManagedWidget ("Orient",
    xmToggleButtonWidgetClass, rowcol3,
    NULL);
  XtAddCallback(orient_switch, XmNvalueChangedCallback, orient_toggle, NULL);
  sticky_switch = XtVaCreateManagedWidget ("Sticky",
    xmToggleButtonWidgetClass, rowcol3,
    NULL);
  XtAddCallback(sticky_switch, XmNvalueChangedCallback, sticky_toggle, NULL);
  rowcol4 = XtVaCreateManagedWidget("Rowcol", xmRowColumnWidgetClass, rowcol2,
    XmNnumColumns, 1,
    XmNorientation, XmHORIZONTAL,
    XmNpacking, XmPACK_COLUMN,
    XmNradioBehavior, True,
    NULL);
  for (i = 0; i < XtNumber(mode_string); i++) {
    modes[i] = XtVaCreateManagedWidget(mode_string[i],
      xmToggleButtonGadgetClass, rowcol4,
      XmNradioBehavior, True,
      NULL);
    XtAddCallback(modes[i], XmNvalueChangedCallback, mode_toggle,
      (XtPointer) i);
  }
  message = XtVaCreateManagedWidget("Play Oct! (use mouse and keypad)",
    xmLabelWidgetClass, rowcol2,
    NULL);

  oct = XtCreateManagedWidget("oct", octWidgetClass, panel,
    NULL, 0);
  XtAddCallback(oct, XtNselectCallback, moves_text, NULL);
  initialize(oct);
  XtRealizeWidget(toplevel);
  XGrabButton(XtDisplay(oct), AnyButton, AnyModifier, XtWindow(oct),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(oct),
    XCreateFontCursor(XtDisplay(oct), XC_crosshair));
  XtMainLoop();

#ifdef VMS
  return 1;
#else
  return 0;
#endif
}

static void initialize(w)
  Widget w;
{
  int size, mode;
  Boolean orient, sticky;
 
  XtVaSetValues(w,
    XtNpractice, FALSE,
    XtNstart, FALSE,
    NULL);
  XtVaGetValues(w,
    XtNsize, &size,
    XtNmode, &mode,
    XtNorient, &orient,
    XtNsticky, &sticky,
    NULL);
  XmScaleSetValue(octas, size);
  XmToggleButtonSetState(modes[mode - PERIOD3], True, False);
  XmToggleButtonSetState(orient_switch, orient, True);
  XmToggleButtonSetState(sticky_switch, sticky, True);
  read_records();
  print_record(size, mode, orient, sticky);
  oldsize = size;
}

static void moves_text(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  octCallbackStruct *call_data;
{
  int size, mode;
  Boolean orient, practice, sticky;

  XtVaGetValues(w,
    XtNsize, &size,
    XtNmode, &mode,
    XtNorient, &orient,
    XtNpractice, &practice,
    XtNsticky, &sticky,
    NULL);
  (void) strcpy(message_dsp, "");
  switch (call_data->reason) {
    case OCT_RESTORE:
      if (practice)
        wprintf(record, "practice");
      moves_dsp = 0;
      break;
    case OCT_RESET:
      moves_dsp = 0;
      break;
    case OCT_IGNORE:
      (void) strcpy(message_dsp, "Randomize to start");
      break;
    case OCT_MOVED:
      moves_dsp++;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
    case OCT_CONTROL:
      return;
    case OCT_SOLVED:
      if (practice)
        moves_dsp = 0;
      else { 
        if (handle_solved(moves_dsp, size, mode, orient, sticky))
          (void) sprintf(message_dsp, "Congratulations %s!!", getlogin());
        else
          (void) strcpy(message_dsp, "Solved!");
      }
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case OCT_PRACTICE:
      moves_dsp = 0;
      practice = !practice;
      if (practice)
        wprintf(record, "practice");
      else {
        (void) strcpy(message_dsp, "Randomize to start");
        print_record(size, mode, orient, sticky);
      }
      XtSetArg(arg[0], XtNpractice, practice);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      break;
    case OCT_RANDOMIZE:
      moves_dsp = 0;
      XtSetArg(arg[0], XtNpractice, FALSE);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      break; 
    case OCT_DEC:
      if (!sticky) {
        moves_dsp = 0;
        size--;
        oldsize = size;
        print_record(size, mode, orient, sticky);
        XtSetArg(arg[0], XtNsize, size);
        XtSetValues(w, arg, 1);
        XmScaleSetValue(octas, size);
      }
      break;
    case OCT_ORIENT:
      moves_dsp = 0;
      orient = !orient;
      print_record(size, mode, orient, sticky);
      XtSetArg(arg[0], XtNorient, orient);
      XtSetValues(w, arg, 1);
      XmToggleButtonSetState(orient_switch, orient, True);
      break;
    case OCT_INC:
      if (!sticky) {
        moves_dsp = 0;
        size++;
        oldsize = size;
        print_record(size, mode, orient, sticky);
        XtSetArg(arg[0], XtNsize, size);
        XtSetValues(w, arg, 1);
        XmScaleSetValue(octas, size);
      }
      break;
    case OCT_PERIOD3:
    case OCT_PERIOD4:
    case OCT_BOTH:
      moves_dsp = 0;
      mode = call_data->reason - OCT_PERIOD3 + PERIOD3;
      print_record(size, mode, orient, sticky);
      XtSetArg(arg[0], XtNmode, mode);
      XtSetValues(w, arg, 1);
      XmToggleButtonSetState(modes[mode - PERIOD3], True, True);
      break;
    case OCT_STICKY:
      moves_dsp = 0;
      sticky = !sticky;
      if (sticky)
        size = 4;
      else
        size = oldsize;
      print_record(size, mode, orient, sticky);
      XtSetArg(arg[0], XtNsticky, sticky);
      XtSetArg(arg[1], XtNsize, size);
      XtSetValues(w, arg, 2);
      XmToggleButtonSetState(sticky_switch, sticky, True);
      break;
    case OCT_COMPUTED:
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case OCT_UNDO:
      moves_dsp--;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
  }
  wprintf(message, "%s", message_dsp);
  wprintf(moves, "%d", moves_dsp);
}

static void octa_slider(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmScaleCallbackStruct *cbs;
{
  int size = cbs->value, old, mode;
  Boolean orient, sticky;

  XtVaGetValues(oct,
    XtNsize, &old,
    XtNmode, &mode,
    XtNorient, &orient,
    XtNsticky, &sticky,
    NULL);
  if (sticky)
    XmScaleSetValue(w, old);
  else if (old != size) {
    XtVaSetValues(oct,
      XtNsize, size,
      NULL);
    moves_dsp = 0;
    wprintf(moves, "%d", moves_dsp);
    print_record(size, mode, orient, sticky);
  }
}

static void mode_toggle(w, mode, cbs)
  Widget w;
  int mode;
  XmToggleButtonCallbackStruct *cbs;
{
  int size;
  Boolean orient, sticky;

  if (cbs->set) {
    XtVaGetValues(oct,
      XtNsize, &size,
      XtNorient, &orient,
      XtNsticky, &sticky,
      NULL);
    XtVaSetValues(oct,
      XtNmode, mode + PERIOD3,
      NULL);
    moves_dsp = 0;
    wprintf(moves, "%d", moves_dsp);
    print_record(size, mode + PERIOD3, orient, sticky);
  }
}

static void orient_toggle(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmToggleButtonCallbackStruct *cbs;
{
  int size, mode;
  Boolean orient = cbs->set, sticky;

  XtVaGetValues(oct,
    XtNsize, &size,
    XtNmode, &mode,
    XtNsticky, &sticky,
    NULL);
  XtVaSetValues(oct,
    XtNorient, orient,
    NULL);
  moves_dsp = 0;
  wprintf(moves, "%d", moves_dsp);
  print_record(size, mode, orient, sticky);
}

static void sticky_toggle(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmToggleButtonCallbackStruct *cbs;
{
  int size, mode;
  Boolean orient, sticky = cbs->set;

  XtVaSetValues(oct,
    XtNsticky, sticky,
    NULL);
  XtVaGetValues(oct,
    XtNsize, &size,
    XtNmode, &mode,
    XtNorient, &orient,
    NULL);
  if (sticky && size != 4) {
    size = 4;
    XmScaleSetValue(octas, size);
    XtVaSetValues(oct,
      XtNsize, size,
      NULL);
  }
  moves_dsp = 0;
  wprintf(moves, "%d", moves_dsp);
  print_record(size, mode, orient, sticky);
}

static void print_record(size, mode, orient, sticky)
  int size, mode;
  Boolean sticky;
{
  int i = mode - PERIOD3;
  int j = (orient) ? 1 : 0;
  int k = (sticky) ? MAXOCTAS - MINOCTAS + 1 : size - MINOCTAS;
 
  if (oct_record[i][j][k] >= MAXRECORD)
    wprintf(record, "NEVER");
  else
    wprintf(record, "%d", oct_record[i][j][k]);
}

static int handle_solved(counter, size, mode, orient, sticky)
  int counter, size, mode;
  Boolean orient, sticky;
{
  int i = mode - PERIOD3;
  int j = (orient) ? 1 : 0;
  int k = (sticky) ? MAXOCTAS - MINOCTAS + 1 : size - MINOCTAS;

  if (counter < oct_record[i][j][k]) {
    oct_record[i][j][k] = counter;
    if ((size < 2 && mode != PERIOD4) || (size < 4  && mode == PERIOD4) || 
        (orient && (oct_record[i][j][k] < oct_record[i][!j][k])))
      oct_record[i][!j][k] = counter;
    write_records();
    print_record(size, mode, orient, sticky);
    return TRUE;
  }
  return FALSE;
}

static void read_records()
{
  FILE *fp;
  int i, n, orient, mode;

  for (mode = 0; mode < MAXMODES; mode++)
    for (orient = 0; orient < 2; orient++)
      for (i = 0; i <= MAXOCTAS - MINOCTAS + 1; i++)
        oct_record[mode][orient][i] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    wprintf(message, "Can not open %s, taking defaults.", SCOREFILE);
  else {
    for (mode = 0; mode < MAXMODES; mode++)
      for (orient = 0; orient < 2; orient++)
        for (i = 0; i <= MAXOCTAS - MINOCTAS + 1; i++) {
          (void) fscanf(fp, "%d", &n);
          oct_record[mode][orient][i] = n;
        }
    (void) fclose(fp);
  }
}

static void write_records()
{
  FILE *fp;
  int i, orient, mode;

  if ((fp = fopen(SCOREFILE, "w")) == NULL)
    wprintf(message, "Can not write to %s.", SCOREFILE);
  else {
    for (mode = 0; mode < MAXMODES; mode++) {
      for (orient = 0; orient < 2; orient++) {
        for (i = 0; i <= MAXOCTAS - MINOCTAS + 1; i++)
          (void) fprintf(fp, "%d ", oct_record[mode][orient][i]);
        (void) fprintf(fp, "\n");
      }
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
