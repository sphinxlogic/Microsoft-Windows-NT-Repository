/*
# MOTIF-BASED DINO
#
#  xmdino.c
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

/*
  Version 5: 95/10/06 Xt/Motif
  Version 4: 94/05/30 Xt
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
#include "Dino.h"
#include "Dino2d.h"
#include "Dino3d.h"
#include "dino.xbm"
#include "mouse-l.xbm"
#include "mouse-m.xbm"
#include "mouse-r.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/dino.scores"
#endif

/* The following are in DinoP.h also */
#define PERIOD2 2
#define PERIOD3 3
#define BOTH 4
#define MAXMODES 3
#define MAXFACES 6

#define MAXRECORD 32767

static void initialize();
static void moves_text();

static void print_record();
static int handle_solved();
static void read_records();
static void write_records();

static void mode_toggle();
static void orient_toggle();
static void wprintf();

static Arg arg[5];
static Widget moves, record, message, dino2d, dino3d,
  modes[MAXMODES], orient_switch;
static int dino_record[MAXMODES][2], moves_dsp = 0;
static char message_dsp[128] = "Randomize to start";

static char *mode_string[] = {
  "Period 2", "Period 3", "Both"
};

static void usage()
{
  (void) fprintf(stderr, "usage: xmdino\n");
  (void) fprintf(stderr, 
    "\t[-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]]\n");
  (void) fprintf(stderr, 
    "\t[-display [{host}]:[{vs}]][-fg {color}] [-bg {color}]\n");
  (void) fprintf(stderr,
    "\t[-[no]orient] [-mono] [-{mode {int} | both}]\n");
  (void) fprintf(stderr,
    "\t[-face{0|1|2|3|4|5} {color}]\n");
  exit(1);
}

static XrmOptionDescRec options[] = {
  {"-fg",		"*dino.Foreground",	XrmoptionSepArg,	NULL},
  {"-bd",		"*dino.Foreground",	XrmoptionSepArg,	NULL},
  {"-bg",		"*Background",		XrmoptionSepArg,	NULL},
  {"-foreground",	"*dino.Foreground",	XrmoptionSepArg,	NULL},
  {"-background",	"*Background",		XrmoptionSepArg,	NULL},
  {"-bordercolor",	"*dino.Foreground",	XrmoptionSepArg,	NULL},
  {"-orient",		"*dino.orient",		XrmoptionNoArg,		"TRUE"},
  {"-noorient",		"*dino.orient",		XrmoptionNoArg,		"FALSE"},
  {"-mode",		"*dino.mode",		XrmoptionSepArg,	NULL},
  {"-both",		"*dino.mode",		XrmoptionNoArg,		"4"},
  {"-mono",		"*dino.mono",		XrmoptionNoArg,		"TRUE"},
  {"-face0",		"*dino.faceColor0",	XrmoptionSepArg,	NULL},
  {"-face1",		"*dino.faceColor1",	XrmoptionSepArg,	NULL},
  {"-face2",		"*dino.faceColor2",	XrmoptionSepArg,	NULL},
  {"-face3",		"*dino.faceColor3",	XrmoptionSepArg,	NULL},
  {"-face4",		"*dino.faceColor4",	XrmoptionSepArg,	NULL},
  {"-face5",		"*dino.faceColor5",	XrmoptionSepArg,	NULL},
};

int main(argc, argv)
  int argc;
  char *argv[];
{
  Widget toplevel;
  Widget panel, panel2, rowcol, rowcol2, rowcol3;
  Pixmap mouse_left_cursor, mouse_middle_cursor, mouse_right_cursor;
  Pixel fg, bg;
  int i;

  toplevel = XtInitialize(argv[0], "Dino",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) dino_bits, dino_width, dino_height));
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
    XmNnumColumns, 1,
    XmNorientation, XmHORIZONTAL,
    XmNpacking, XmPACK_COLUMN,
    XmNradioBehavior, True,
    NULL);
  for (i = 0; i < XtNumber(mode_string); i++) {
    modes[i] = XtVaCreateManagedWidget(mode_string[i],
      xmToggleButtonGadgetClass, rowcol2,
      XmNradioBehavior, True,
      NULL);
    XtAddCallback(modes[i], XmNvalueChangedCallback, mode_toggle,
      (XtPointer) i);
  }

  rowcol3 = XtVaCreateManagedWidget("Rowcol3", xmRowColumnWidgetClass, panel2,
    NULL);
  XtVaGetValues(rowcol2, XmNforeground, &fg, XmNbackground, &bg, NULL);
  orient_switch = XtVaCreateManagedWidget ("Orient",
    xmToggleButtonWidgetClass, rowcol3,
    NULL);
  XtAddCallback(orient_switch, XmNvalueChangedCallback, orient_toggle, NULL);
  message = XtVaCreateManagedWidget("Play Dino! (use mouse and keypad)",
    xmLabelWidgetClass, rowcol3,
    NULL);

  dino2d = XtCreateManagedWidget("dino", dino2dWidgetClass, panel,
    NULL, 0);
  XtVaSetValues(dino2d,
    XtNheight, 200,
    NULL);
  XtAddCallback(dino2d, XtNselectCallback, moves_text, NULL);
  dino3d = XtCreateManagedWidget("dino", dino3dWidgetClass, panel,
    NULL, 0);
  XtVaSetValues(dino3d,
    XtNheight, 200,
    NULL);
  XtAddCallback(dino3d, XtNselectCallback, moves_text, NULL);
  initialize();
  XtRealizeWidget(toplevel);
  XGrabButton(XtDisplay(dino2d), AnyButton, AnyModifier, XtWindow(dino2d),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(dino2d),
    XCreateFontCursor(XtDisplay(dino2d), XC_crosshair));
  XGrabButton(XtDisplay(dino3d), AnyButton, AnyModifier, XtWindow(dino3d),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(dino3d),
    XCreateFontCursor(XtDisplay(dino3d), XC_crosshair));
  XtMainLoop();

#ifdef VMS
  return 1;
#else
  return 0;
#endif
}

/* There's probably a better way to assure that they are the same but
   I don't know it off hand. */
static void make_equivalent(mode, orient)
  int *mode;
  Boolean *orient;
{
  Boolean mono;
  Pixel foreground, background;
  String facecolor[MAXFACES];

  XtVaGetValues(dino2d,
    XtNmode, mode,
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
  XtVaSetValues(dino2d,
    XtNface, DINO_IGNORE,
    XtNpractice, FALSE,
    XtNstart, FALSE,
    NULL);
  XtVaSetValues(dino3d,
    XtNmode, *mode,
    XtNorient, *orient,
    XtNmono, mono,
    XtNface, DINO_IGNORE,
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
  int mode;
  Boolean orient;
 
  make_equivalent(&mode, &orient);
  read_records();
  XmToggleButtonSetState(modes[mode - PERIOD2], True, False);
  XmToggleButtonSetState(orient_switch, orient, True);
  print_record(mode, orient);
}

static void moves_text(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  dinoCallbackStruct *call_data;
{
  int mode;
  Boolean orient, practice;
  Widget otherw;

  if (w == dino2d)
    otherw = dino3d;
  else /* (w == dino3d) */
    otherw = dino2d;
  XtVaGetValues(w,
    XtNorient, &orient,
    XtNmode, &mode,
    XtNpractice, &practice,
    NULL);
  (void) strcpy(message_dsp, "");
  switch (call_data->reason) {
    case DINO_RESTORE:
      XtSetArg(arg[0], XtNface, DINO_RESTORE);
      XtSetValues(otherw, arg, 1);
      XtSetValues(w, arg, 1);
      moves_dsp = 0;
      break;
    case DINO_RESET:
      moves_dsp = 0;
      break;
    case DINO_IGNORE:
      (void) strcpy(message_dsp, "Randomize to start");
      break;
    case DINO_MOVED:
      moves_dsp++;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetArg(arg[1], XtNface, call_data->face);
      XtSetArg(arg[2], XtNside, call_data->side);
      XtSetArg(arg[3], XtNdirection, call_data->direction);
      XtSetArg(arg[4], XtNstyle, call_data->style);
      XtSetValues(otherw, arg, 5);
      XtSetValues(w, arg, 1);
      break;
    case DINO_CONTROL:
      XtSetArg(arg[0], XtNface, call_data->face);
      XtSetArg(arg[1], XtNside, call_data->side);
      XtSetArg(arg[2], XtNdirection, call_data->direction);
      XtSetArg(arg[3], XtNstyle, call_data->style);
      XtSetValues(otherw, arg, 4);
      return;
    case DINO_SOLVED:
      if (practice)
        moves_dsp = 0;
      else { 
        if (handle_solved(moves_dsp, mode, orient))
          (void) sprintf(message_dsp, "Congratulations %s!!", getlogin());
        else
          (void) strcpy(message_dsp, "Solved!");
      }
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case DINO_PRACTICE:
      moves_dsp = 0;
      practice = !practice;
      if (practice)
        wprintf(record, "practice");
      else {
        (void) strcpy(message_dsp, "Randomize to start");
        print_record(mode, orient);
      }
      XtSetArg(arg[0], XtNpractice, practice);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      XtSetValues(otherw, arg, 2);
      break;
    case DINO_RANDOMIZE:
      moves_dsp = 0;
      XtSetArg(arg[0], XtNpractice, FALSE);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      XtSetValues(otherw, arg, 2);
      break; 
    case DINO_ORIENT:
      moves_dsp = 0;
      orient = !orient;
      print_record(mode, orient);
      XtSetArg(arg[0], XtNorient, orient);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      XmToggleButtonSetState(orient_switch, orient, True);
      break;
    case DINO_PERIOD2:
    case DINO_PERIOD3:
    case DINO_BOTH:
      moves_dsp = 0;
      mode = call_data->reason - DINO_PERIOD2 + PERIOD2;
      print_record(mode, orient);
      XtSetArg(arg[0], XtNmode, mode);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      XmToggleButtonSetState(modes[mode - PERIOD2], True, True);
      break;
    case DINO_COMPUTED:
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case DINO_UNDO:
      moves_dsp--;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetArg(arg[1], XtNface, call_data->face);
      XtSetArg(arg[2], XtNside, call_data->side);
      XtSetArg(arg[3], XtNdirection, call_data->direction);
      XtSetArg(arg[4], XtNstyle, call_data->style);
      XtSetValues(otherw, arg, 5);
      XtSetValues(w, arg, 1);
      break;
  }
  wprintf(message, "%s", message_dsp);
  wprintf(moves, "%d", moves_dsp);
}

static void mode_toggle(w, mode, cbs)
  Widget w;
  int mode;
  XmToggleButtonCallbackStruct *cbs;
{
  if (cbs->set) {
    XtVaSetValues(dino2d,
      XtNmode, mode + PERIOD2,
      NULL);
    XtVaSetValues(dino3d,
      XtNmode, mode + PERIOD2,
      NULL);
    moves_dsp = 0;
    wprintf(moves, "%d", moves_dsp);
    print_record(mode + PERIOD2, XmToggleButtonGetState(orient_switch));
  }
}

static void orient_toggle(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmToggleButtonCallbackStruct *cbs;
{
  int mode;
  Boolean orient = cbs->set;

  XtVaGetValues(dino2d,
    XtNmode, &mode,
    NULL);
  XtVaSetValues(dino2d,
    XtNorient, orient,
    NULL);
  XtVaSetValues(dino3d,
    XtNorient, orient,
    NULL);
  moves_dsp = 0;
  wprintf(moves, "%d", moves_dsp);
  print_record(mode, orient);
}

static void print_record(mode, orient)
  int mode;
  Boolean orient;
{
  int i = mode - PERIOD2;
  int j = (orient) ? 1 : 0;

  if (dino_record[i][j] >= MAXRECORD)
    wprintf(record, "NEVER");
  else
    wprintf(record, "%d", dino_record[i][j]);
}

static int handle_solved(counter, mode, orient)
  int counter, mode;
  Boolean orient;
{
  int i = mode - PERIOD2;
  int j = (orient) ? 1 : 0;

  if (counter < dino_record[i][j]) {
    dino_record[i][j] = counter;
    if (orient && (dino_record[i][j] < dino_record[!i][j]))
      dino_record[!i][j] = counter;
    write_records();
    print_record(mode, orient);
    return TRUE;
  }
  return FALSE;
}

static void read_records()
{
  FILE *fp;
  int n, mode, orient;

  for (mode = 0; mode < MAXMODES; mode++)
    for (orient = 0; orient < 2; orient++)
      dino_record[mode][orient] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    wprintf(message, "Can not open %s, taking defaults.", SCOREFILE);
  else {
    for (mode = 0; mode < MAXMODES; mode++)
      for (orient = 0; orient < 2; orient++) {
        (void) fscanf(fp, "%d", &n);
        dino_record[mode][orient] = n;
      }
    (void) fclose(fp);
  }
}

static void write_records()
{
  FILE *fp;
  int mode, orient;

  if ((fp = fopen(SCOREFILE, "w")) == NULL)
    wprintf(message, "Can not write to %s.", SCOREFILE);
  else {
    for (mode = 0; mode < MAXMODES; mode++) {
      for (orient = 0; orient < 2; orient++)
        (void) fprintf(fp, "%d ", dino_record[mode][orient]);
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
