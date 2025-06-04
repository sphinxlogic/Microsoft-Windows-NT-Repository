/*
# MOTIF-BASED MASTERBALL(tm)
#
#  xmmball.c
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

/*
  Version 5: 95/10/02 Xt/Motif
  Version 1: 94/09/15 Xt
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
#include "Mball.h"
#include "mball.xbm"
#include "mouse-l.xbm"
#include "mouse-m.xbm"
#include "mouse-r.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/mball.scores"
#endif
 
/* The following are in MballP.h also */
#define MINWEDGES 2
#define MAXWEDGES 8
#define MINRINGS 1
#define MAXRINGS 6

#define MAXRECORD 32767

static void initialize();
static void moves_text();

static void print_record();
static int handle_solved();
static void read_records();
static void write_records();

static void wedge_toggle();
static void ring_slider();
static void orient_toggle();
static void wprintf();
 
static Arg arg[5];
static Widget moves, record, message, mball, orient_switch,
   wedge[(MAXWEDGES - MINWEDGES) / 2 + 1], ring;
static int mball_record[2][(MAXWEDGES - MINWEDGES) / 2 + 1]
  [MAXRINGS - MINRINGS + 1];
static int moves_dsp = 0;
static char message_dsp[128] = "Randomize to start";

static char *wedge_string[] = {
  "Two", "Four", "Six", "Eight"
};

static void usage()
{
  (void) fprintf(stderr, "usage: xmmball\n");
  (void) fprintf(stderr,
    "\t[-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]]\n");
  (void) fprintf(stderr,
    "\t[-display [{host}]:[{vs}]][-fg {color}] [-bg {color}]\n");
  (void) fprintf(stderr,
    "\t[-{wedges {int}}] [-{rings {int}}] [-[no]orient] [-mono]\n");
  (void) fprintf(stderr,
    "\t[-face{0|1|2|3|4|5|6|7} {color}]\n");
  exit(1);
}

static XrmOptionDescRec options[] = {
  {"-fg",		"*mball.Foreground",	XrmoptionSepArg,	NULL},
  {"-bd",		"*mball.Foreground",	XrmoptionSepArg,	NULL},
  {"-bg",		"*Background",		XrmoptionSepArg,	NULL},
  {"-foreground",	"*mball.Foreground",	XrmoptionSepArg,	NULL},
  {"-background",	"*Background",		XrmoptionSepArg,	NULL},
  {"-bordercolor",	"*mball.Foreground",	XrmoptionSepArg,	NULL},
  {"-wedges",		"*mball.wedges",	XrmoptionSepArg,	NULL},
  {"-rings",		"*mball.rings",		XrmoptionSepArg,	NULL},
  {"-orient",		"*mball.orient",	XrmoptionNoArg,		"TRUE"},
  {"-noorient",		"*mball.orient",	XrmoptionNoArg,		"FALSE"},
  {"-mono",		"*mball.mono",		XrmoptionNoArg,		"TRUE"},
  {"-face0",		"*mball.faceColor0",	XrmoptionSepArg,	NULL},
  {"-face1",		"*mball.faceColor1",	XrmoptionSepArg,	NULL},
  {"-face2",		"*mball.faceColor2",	XrmoptionSepArg,	NULL},
  {"-face3",		"*mball.faceColor3",	XrmoptionSepArg,	NULL},
  {"-face4",		"*mball.faceColor4",	XrmoptionSepArg,	NULL},
  {"-face5",		"*mball.faceColor5",	XrmoptionSepArg,	NULL},
  {"-face6",		"*mball.faceColor6",	XrmoptionSepArg,	NULL},
  {"-face7",		"*mball.faceColor7",	XrmoptionSepArg,	NULL},
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

  toplevel = XtInitialize(argv[0], "Mball",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) mball_bits, mball_width, mball_height));
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
    XtVaTypedArg, XmNlabelString, XmRString, "Move", 10, NULL);
  XtVaCreateManagedWidget("mouse_left", xmLabelGadgetClass, rowcol,
    XmNlabelType, XmPIXMAP, XmNlabelPixmap, mouse_left_cursor, NULL);
  XtVaCreateManagedWidget("mouse_middle_text", xmLabelGadgetClass, rowcol,
    XtVaTypedArg, XmNlabelString, XmRString, "Practice", 15, NULL);
  XtVaCreateManagedWidget("mouse_middle", xmLabelGadgetClass, rowcol,
    XmNlabelType, XmPIXMAP, XmNlabelPixmap, mouse_middle_cursor, NULL);
  XtVaCreateManagedWidget("mouse_right_text", xmLabelGadgetClass, rowcol,
    XtVaTypedArg, XmNlabelString, XmRString, "Randomize", 20, NULL);
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
  for (i = 0; i < XtNumber(wedge_string); i++) {
    wedge[i] = XtVaCreateManagedWidget(wedge_string[i],
      xmToggleButtonGadgetClass, rowcol2,
      XmNradioBehavior, True,
      NULL);
    XtAddCallback(wedge[i], XmNvalueChangedCallback, wedge_toggle,
      (XtPointer) i);
  }

  rowcol3 = XtVaCreateManagedWidget("Rowcol3", xmRowColumnWidgetClass, panel2,
    NULL);
  XtVaGetValues(rowcol3, XmNforeground, &fg, XmNbackground, &bg, NULL);
  ring = XtVaCreateManagedWidget("ring", xmScaleWidgetClass, rowcol3,
    XtVaTypedArg, XmNtitleString, XmRString, "Rings", 6,
    XmNminimum, MINRINGS,
    XmNmaximum, MAXRINGS,
    XmNvalue, MINRINGS,
    XmNshowValue, True,
    XmNorientation, XmHORIZONTAL,
    NULL);
  XtAddCallback(ring, XmNvalueChangedCallback, ring_slider, NULL);
  orient_switch = XtVaCreateManagedWidget ("Orient",
    xmToggleButtonWidgetClass, rowcol3,
    NULL);
  XtAddCallback(orient_switch, XmNvalueChangedCallback, orient_toggle, NULL);
  message = XtVaCreateManagedWidget("Play Masterball! (use mouse and keypad)",
    xmLabelWidgetClass, rowcol3,
    NULL);
 
  mball = XtCreateManagedWidget("mball", mballWidgetClass, panel,
    NULL, 0);
  XtAddCallback(mball, XtNselectCallback, moves_text, NULL);
  initialize(mball);
  XtRealizeWidget(toplevel);
  XGrabButton(XtDisplay(mball), AnyButton, AnyModifier, XtWindow(mball),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(mball),
    XCreateFontCursor(XtDisplay(mball), XC_crosshair));
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
  int wedges, rings;
  Boolean orient;

  XtVaSetValues(w,
    XtNpractice, FALSE,
    XtNstart, FALSE,
    NULL);
  XtVaGetValues(w,
    XtNwedges, &wedges,
    XtNrings, &rings,
    XtNorient, &orient,
    NULL);
  XmToggleButtonSetState(wedge[(wedges - MINWEDGES) / 2], True, False);
  XmScaleSetValue(ring, rings);
  XmToggleButtonSetState(orient_switch, orient, True);
  read_records();
  print_record(orient, wedges, rings);
}

static void moves_text(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  mballCallbackStruct *call_data;
{
  int wedges, rings;
  Boolean orient, practice;

  XtVaGetValues(w,
    XtNwedges, &wedges,
    XtNrings, &rings,
    XtNorient, &orient,
    XtNpractice, &practice,
    NULL);
  (void) strcpy(message_dsp, "");
  switch (call_data->reason) {
    case MBALL_RESTORE:
      if (practice)
        wprintf(record, "practice");
      moves_dsp = 0;
      break;
    case MBALL_RESET:
      moves_dsp = 0;
      break;
    case MBALL_IGNORE:
      (void) strcpy(message_dsp, "Randomize to start");
      break;
    case MBALL_MOVED:
      moves_dsp++;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
    case MBALL_CONTROL:
      return;
    case MBALL_SOLVED:
      if (practice)
        moves_dsp = 0;
      else { 
        if (handle_solved(moves_dsp, wedges, rings, orient))
          (void) sprintf(message_dsp, "Congratulations %s!!", getenv("USER"));
        else
          (void) strcpy(message_dsp, "Solved!");
      }
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case MBALL_PRACTICE:
      moves_dsp = 0;
      practice = !practice;
      if (practice)
        wprintf(record, "practice");
      else {
        (void) strcpy(message_dsp, "Randomize to start");
        print_record(orient, wedges, rings);
      }
      XtSetArg(arg[0], XtNpractice, practice);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      break;
    case MBALL_RANDOMIZE:
      moves_dsp = 0;
      XtSetArg(arg[0], XtNpractice, FALSE);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      break; 
    case MBALL_DEC:
      moves_dsp = 0;
      rings--;
      print_record(orient, wedges, rings);
      XtSetArg(arg[0], XtNrings, rings);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(ring, rings);
      break;
   case MBALL_ORIENT:
      moves_dsp = 0;
      orient = !orient;
      print_record(orient, wedges, rings);
      XtSetArg(arg[0], XtNorient, orient);
      XtSetValues(w, arg, 1);
      XmToggleButtonSetState(orient_switch, orient, True);
      break;
    case MBALL_INC:
      moves_dsp = 0;
      rings++;
      print_record(orient, wedges, rings);
      XtSetArg(arg[0], XtNrings, rings);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(ring, rings);
      break;
    case MBALL_WEDGE2:
    case MBALL_WEDGE4:
    case MBALL_WEDGE6:
    case MBALL_WEDGE8:
      moves_dsp = 0;
      wedges = 2 * (call_data->reason - MBALL_WEDGE2) + MINWEDGES;
      print_record(orient, wedges, rings);
      XtSetArg(arg[0], XtNwedges, wedges);
      XtSetValues(w, arg, 1);
      XmToggleButtonSetState(wedge[(wedges - MINWEDGES) / 2], True, True);
      break;
    case MBALL_COMPUTED:
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case MBALL_UNDO:
      moves_dsp--;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
  }
  wprintf(message, "%s", message_dsp);
  wprintf(moves, "%d", moves_dsp);
}
 
static void wedge_toggle(w, bit, cbs)
  Widget w;
  int bit;
  XmToggleButtonCallbackStruct *cbs;
{
  int wedges, rings;

  if (cbs->set) {
    XtVaGetValues(mball,
      XtNrings, &rings,
      NULL);
    wedges = bit * 2 + MINWEDGES;
    XtVaSetValues(mball,
      XtNwedges, wedges,
      NULL);
    moves_dsp = 0;
    wprintf(moves, "%d", moves_dsp);
    print_record(XmToggleButtonGetState(orient_switch), wedges, rings);
  }
}

static void ring_slider(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmScaleCallbackStruct *cbs;
{
  int wedges, rings = cbs->value, old;
 
  XtVaGetValues(mball,
    XtNwedges, &wedges,
    XtNrings, &old,
    NULL);
  if (old != rings) {
    XtVaSetValues(mball,
      XtNrings, rings,
      NULL);
    moves_dsp = 0;
    wprintf(moves, "%d", moves_dsp);
    print_record(XmToggleButtonGetState(orient_switch), wedges, rings);
  }
}
 
static void orient_toggle(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmToggleButtonCallbackStruct *cbs;
{
  int wedges, rings;
  Boolean orient = cbs->set;
 
  XtVaSetValues(mball,
    XtNorient, orient,
    NULL);
  XtVaGetValues(mball,
    XtNwedges, &wedges,
    XtNrings, &rings,
    NULL);
  moves_dsp = 0;
  wprintf(moves, "%d", moves_dsp);
  print_record(orient, wedges, rings);
}

static void print_record(orient, wedges, rings)
  Boolean orient;
  int wedges, rings;
{
  int i = (orient) ? 1 : 0;

  if (mball_record[i][(wedges - MINWEDGES) / 2][rings - MINRINGS] >= MAXRECORD)
    wprintf(record, "NEVER");
  else
    wprintf(record, "%d",
       mball_record[i][(wedges - MINWEDGES) / 2][rings - MINRINGS]);
}

static int handle_solved(counter, wedges, rings, orient)
  int counter, wedges, rings;
  Boolean orient;
{
  int i = (orient) ? 1 : 0;
  int w = (wedges - MINWEDGES) / 2;
  int r = rings - MINRINGS;

  if (counter < mball_record[i][w][r]) {
    mball_record[i][w][r] = counter;
    if (orient && (mball_record[i][w][r] < mball_record[!i][w][r]))
      mball_record[!i][w][r] = counter;
    write_records();
    print_record(orient, wedges, rings);
    return TRUE;
  }
  return FALSE;
}

static void read_records()
{
  FILE *fp;
  int i, j, n, orient;

  for (orient = 0; orient < 2; orient++)
    for (i = 0; i < (MAXWEDGES - MINWEDGES) / 2 + 1; i++)
      for (j = 0; j < MAXRINGS - MINRINGS + 1; j++)
        mball_record[orient][i][j] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    wprintf(message, "Can not open %s, taking defaults.", SCOREFILE);
  else {
    for (orient = 0; orient < 2; orient++)
      for (i = 0; i < (MAXWEDGES - MINWEDGES) / 2 + 1; i++)
        for (j = 0; j < MAXRINGS - MINRINGS + 1; j++) {
          (void) fscanf(fp, "%d", &n);
          mball_record[orient][i][j] = n;
        }
    (void) fclose(fp);
  }
}

static void write_records()
{
  FILE *fp;
  int i, j, orient;

  if ((fp = fopen(SCOREFILE, "w")) == NULL)
    wprintf(message, "Can not write to %s.", SCOREFILE);
  else {
    for (orient = 0; orient < 2; orient++) {
      for (i = 0; i < (MAXWEDGES - MINWEDGES) / 2 + 1; i++) {
        for (j = 0; j < MAXRINGS - MINRINGS + 1; j++)
          (void) fprintf(fp, "%d ", mball_record[orient][i][j]);
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
