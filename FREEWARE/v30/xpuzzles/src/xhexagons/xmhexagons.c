/*
# MOTIF-BASED HEXAGONS
#
#  xmhexagons.c
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
  Version 5: 95/10/01 Xt/Motif
  Version 4: 94/06/07 Xt
  Version 3: 93/04/01 Motif
  Version 2: 92/01/07 XView
  Version 1: 91/09/05 SunView
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
#include "Hexagons.h"
#include "hexagons.xbm"
#include "mouse-l.xbm"
#include "mouse-m.xbm"
#include "mouse-r.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/hexagons.scores"
#endif
 
/* The following are in HexagonsP.h also */
#define MINHEXAGONS 1
#define MAXHEXAGONS 12
#define MAXORIENT 2
#define NOCORN 0
#define CORNERS 1

#define SAVEDMAXHEXAGONS MAXHEXAGONS
#define MAXRECORD 32767

static void initialize();
static void moves_text();
 
static void print_record();
static int handle_solved();
static void read_records();
static void write_records();
 
static void wprintf();
static void hex_slider();
static void corners_toggle();

static Arg arg[2];
static Widget moves, record, message, hexagons, corners_switch, hex;
static int hexagons_record[MAXORIENT][MAXHEXAGONS - MINHEXAGONS + 1],
  moves_dsp = 0;
static char message_dsp[128] = "Randomize to start";

static void usage()
{
  (void) fprintf(stderr, "usage: xmhexagons\n");
  (void) fprintf(stderr,
    "\t[-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]]\n");
  (void) fprintf(stderr,
    "\t[-display [{host}]:[{vs}]][-fg {color}] [-bg {color}]\n");
  (void) fprintf(stderr,
    "\t[-tile {color}] [-size {int}] [-[no]corners]\n");
  exit(1);
}

static XrmOptionDescRec options[] = {
  {"-fg",		"*hexagons.Foreground",	XrmoptionSepArg,	NULL},
  {"-bd",		"*hexagons.Foreground",	XrmoptionSepArg,	NULL},
  {"-bg",		"*Background",		XrmoptionSepArg,	NULL},
  {"-foreground",	"*hexagons.Foreground",	XrmoptionSepArg,	NULL},
  {"-background",	"*Background",		XrmoptionSepArg,	NULL},
  {"-bordercolor",	"*hexagons.Foreground",	XrmoptionSepArg,	NULL},
  {"-tile",		"*hexagons.tileColor",	XrmoptionSepArg,	NULL},
  {"-size",		"*hexagons.size",	XrmoptionSepArg,	NULL},
  {"-corners",		"*hexagons.corners",	XrmoptionNoArg,		"TRUE"},
  {"-nocorners",	"*hexagons.corners",	XrmoptionNoArg,		"FALSE"}
};

int main(argc, argv)
  int argc;
  char *argv[];
{
  Widget toplevel; 
  Widget panel, panel2, rowcol, rowcol2;
  Pixmap mouse_left_cursor, mouse_middle_cursor, mouse_right_cursor;
  Pixel fg, bg;

  toplevel = XtInitialize(argv[0], "Hexagons",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) hexagons_bits, hexagons_width, hexagons_height));
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
    XtVaTypedArg, XmNlabelString, XmRString, "Move tile HIGH", 14, NULL);
  XtVaCreateManagedWidget("mouse_left", xmLabelGadgetClass, rowcol,
    XmNlabelType, XmPIXMAP, XmNlabelPixmap, mouse_left_cursor, NULL);
  XtVaCreateManagedWidget("mouse_middle_text", xmLabelGadgetClass, rowcol,
    XtVaTypedArg, XmNlabelString, XmRString, "Move tile LOW", 15, NULL);
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
  hex = XtVaCreateManagedWidget("hexs", xmScaleWidgetClass, rowcol2,
    XtVaTypedArg, XmNtitleString, XmRString, "Hexs", 5,
    XmNminimum, MINHEXAGONS,
    XmNmaximum, MAXHEXAGONS,
    XmNvalue, MINHEXAGONS,
    XmNshowValue, True,
    XmNorientation, XmHORIZONTAL,
    NULL);
  XtAddCallback(hex, XmNvalueChangedCallback, hex_slider, NULL);
  corners_switch = XtVaCreateManagedWidget ("Corners",
    xmToggleButtonWidgetClass, rowcol2,
    NULL);
  XtAddCallback(corners_switch, XmNvalueChangedCallback, corners_toggle, NULL);
  message = XtVaCreateManagedWidget("Play Hexagons! (use mouse or keypad)",
    xmLabelWidgetClass, rowcol2,
    NULL);

  hexagons = XtCreateManagedWidget("hexagons", hexagonsWidgetClass, panel,
    NULL, 0);
  XtAddCallback(hexagons, XtNselectCallback, moves_text, NULL);
  initialize(hexagons);
  XtRealizeWidget(toplevel);
  XGrabButton(XtDisplay(hexagons), AnyButton, AnyModifier, XtWindow(hexagons),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(hexagons),
    XCreateFontCursor(XtDisplay(hexagons), XC_crosshair));
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
  int size;
  Boolean corners;

  XtVaSetValues(w,
    XtNstart, FALSE,
    NULL);
  XtVaGetValues(w,
    XtNsize, &size,
    XtNcorners, &corners,
    NULL);
  XmScaleSetValue(hex, size);
  XmToggleButtonSetState(corners_switch, corners, True);
  read_records();
  print_record(size, corners);
}

static void moves_text(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  hexagonsCallbackStruct *call_data;
{
  int size;
  Boolean corners;

  XtVaGetValues(w,
    XtNsize, &size,
    XtNcorners, &corners,
    NULL);
  (void) strcpy(message_dsp, "");
  switch (call_data->reason) {
    case HEXAGONS_RESTORE:
    case HEXAGONS_RESET:
      moves_dsp = 0;
      break;
    case HEXAGONS_BLOCKED:
      (void) strcpy(message_dsp, "Blocked");
      break;
    case HEXAGONS_SPACE:
      /*(void) strcpy(message_dsp, "Spaces can't move");*/  /* Too annoying */
      break;
    case HEXAGONS_IGNORE:
      (void) strcpy(message_dsp, "Randomize to start");
      break;
    case HEXAGONS_MOVED:
      moves_dsp++;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
    case HEXAGONS_SOLVED:
      if (handle_solved(moves_dsp, size, corners))
        (void) sprintf(message_dsp, "Congratulations %s!!", getlogin());
      else
        (void) strcpy(message_dsp, "Solved!");
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case HEXAGONS_RANDOMIZE:
      moves_dsp = 0;
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case HEXAGONS_DEC:
      moves_dsp = 0;
      size--;
      print_record(size, corners);
      XtSetArg(arg[0], XtNsize, size);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(hex, size);
      break;
    case HEXAGONS_INC:
      moves_dsp = 0;
      size++;
      print_record(size, corners);
      XtSetArg(arg[0], XtNsize, size);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(hex, size);
      break;
    case HEXAGONS_CORNERS:
      moves_dsp = 0;
      corners = !corners;
      print_record(size, corners);
      XtSetArg(arg[0], XtNcorners, corners);
      XtSetValues(w, arg, 1);
      XmToggleButtonSetState(corners_switch, corners, True);
      break;
    case HEXAGONS_COMPUTED:
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case HEXAGONS_UNDO:
      moves_dsp--;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
  }
  wprintf(message, "%s", message_dsp);
  wprintf(moves, "%d", moves_dsp);
}

static void hex_slider(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmScaleCallbackStruct *cbs;
{
  int size = cbs->value, old;

  XtVaGetValues(hexagons,
    XtNsize, &old,
    NULL);  
  if (old != size) {
    XtVaSetValues(hexagons,
      XtNsize, size,
      NULL);  
    moves_dsp = 0;
    wprintf(moves, "%d", moves_dsp);
    print_record(size, XmToggleButtonGetState(corners_switch));
  }
}

static void corners_toggle(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmToggleButtonCallbackStruct *cbs;
{
  int size;
  Boolean corners = cbs->set;

  XtVaSetValues(hexagons,
    XtNcorners, corners,
    NULL);
  XtVaGetValues(hexagons,
    XtNsize, &size,
    NULL);
  moves_dsp = 0;
  wprintf(moves, "%d", moves_dsp);
  print_record(size, corners);
}

static void print_record(size, corners)
  int size;
  Boolean corners;
{
  int i = (corners) ? 1 : 0, j = size - MINHEXAGONS;
 
  if (hexagons_record[i][j] >= MAXRECORD)
    wprintf(record, "NEVER");
  else
    wprintf(record, "%d", hexagons_record[i][j]);
}
 
 
static int handle_solved(counter, size, corners)
  int counter, size, corners;
{
  int i = (corners) ? 1 : 0, j = size - MINHEXAGONS;
 
  if (counter < hexagons_record[i][j]) {
    hexagons_record[i][j] = counter;
    write_records();
    print_record(size, corners);
    return TRUE;
  }
  return FALSE;
}

static void read_records()
{
  FILE *fp;
  int i, j, n;
 
  for (i = 0; i < MAXORIENT; i++)
    for (j = 0; j < MAXHEXAGONS - MINHEXAGONS + 1; j++)
      hexagons_record[i][j] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    wprintf(message, "Can not open %s, taking defaults.\n", SCOREFILE);
  else {
    for (i = 0; i < MAXORIENT; i++)
      for (j = 0; j < SAVEDMAXHEXAGONS - MINHEXAGONS + 1; j++) {
        (void) fscanf(fp, "%d", &n);
        hexagons_record[i][j] = n;
    }
    (void) fclose(fp);
  }
}

static void write_records()
{
  FILE *fp;
  int i, j;
 
  if ((fp = fopen(SCOREFILE, "w")) == NULL)
    wprintf(message, "Can not write to %s.", SCOREFILE);
  else {
    for (i = 0; i < MAXORIENT; i++) {
      for (j = 0; j < SAVEDMAXHEXAGONS - MINHEXAGONS + 1; j++)
        (void) fprintf(fp, "%d ", hexagons_record[i][j]);
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
    XtError("xs_wprintf() requires a Label Widget");
  format = va_arg(args, char *);
  (void) vsprintf(str, format, args);
  xmstr = XmStringLtoRCreate(str, XmSTRING_DEFAULT_CHARSET);
  XtSetArg(wargs[0], XmNlabelString, xmstr);
  XtSetValues(w, wargs, 1);
  va_end(args);
}

