/*
# MOTIF-BASED TRIANGLES
#
#  xmtriangles.c
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
  Version 2: 92/01/06 XView
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
#include "Triangles.h"
#include "triangles.xbm"
#include "mouse-l.xbm"
#include "mouse-r.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/triangles.scores"
#endif

/* The following are in TrianglesP.h also */
#define MINTRIANGLES 1
#define MAXTRIANGLES 16

#define SAVEDMAXTRIANGLES MAXTRIANGLES
#define MAXRECORD 32767

static void initialize();
static void moves_text();

static void print_record();
static int handle_solved();
static void read_records();
static void write_records();

static void wprintf();
static void tri_slider();

static Arg arg[2];
static Widget moves, record, message, triangles, tri;
static int triangles_record[MAXTRIANGLES - MINTRIANGLES + 1], moves_dsp = 0;
static char message_dsp[128] = "Randomize to start";

static void usage()
{
  (void) fprintf(stderr, "usage: xtriangles\n");
  (void) fprintf(stderr,
    "\t[-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]]\n");
  (void) fprintf(stderr,
    "\t[-display [{host}]:[{vs}]][-fg {color}] [-bg {color}]\n");
  (void) fprintf(stderr,
    "\t[-tile {color}] [-size {int}]\n");
  exit(1);
}

static XrmOptionDescRec options[] = {
  {"-fg",		"*triangles.Foreground",XrmoptionSepArg,	NULL},
  {"-bd",		"*triangles.Foreground",XrmoptionSepArg,	NULL},
  {"-bg",		"*Background",		XrmoptionSepArg,	NULL},
  {"-foreground",	"*triangles.Foreground",XrmoptionSepArg,	NULL},
  {"-background",	"*Background",		XrmoptionSepArg,	NULL},
  {"-bordercolor",	"*triangles.Foreground",XrmoptionSepArg,	NULL},
  {"-tile",		"*triangles.tileColor",	XrmoptionSepArg,	NULL},
  {"-size",		"*triangles.size",	XrmoptionSepArg,	NULL}
};

int main(argc, argv)
  int argc;
  char *argv[];
{
  Widget toplevel;
  Widget panel, panel2, rowcol, rowcol2;
  Pixmap mouse_left_cursor, mouse_right_cursor;
  Pixel fg, bg;

  toplevel = XtInitialize(argv[0], "Triangles",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) triangles_bits, triangles_width, triangles_height));
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
  mouse_right_cursor = XCreatePixmapFromBitmapData(XtDisplay(rowcol),
    RootWindowOfScreen(XtScreen(rowcol)), mouse_right_bits,
    mouse_right_width, mouse_right_height, fg, bg,
    DefaultDepthOfScreen(XtScreen(rowcol)));
  XtVaCreateManagedWidget("mouse_left_text", xmLabelGadgetClass, rowcol,
    XtVaTypedArg, XmNlabelString, XmRString, "Move tile", 10, NULL);
  XtVaCreateManagedWidget("mouse_left", xmLabelGadgetClass, rowcol,
    XmNlabelType, XmPIXMAP, XmNlabelPixmap, mouse_left_cursor, NULL);
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

  rowcol2 = XtVaCreateManagedWidget("Rowcol2", xmRowColumnWidgetClass, panel2,
    NULL);
  XtVaGetValues(rowcol2, XmNforeground, &fg, XmNbackground, &bg, NULL);
  tri = XtVaCreateManagedWidget("tris", xmScaleWidgetClass, rowcol2,
    XtVaTypedArg, XmNtitleString, XmRString, "Tris", 5,
    XmNminimum, MINTRIANGLES,
    XmNmaximum, MAXTRIANGLES,
    XmNvalue, MINTRIANGLES,
    XmNshowValue, True,
    XmNorientation, XmHORIZONTAL,
    NULL);
  XtAddCallback(tri, XmNvalueChangedCallback, tri_slider, NULL);
  message = XtVaCreateManagedWidget("Play Triangles! (use mouse or keypad)",
    xmLabelWidgetClass, rowcol2,
    NULL);

  triangles = XtCreateManagedWidget("triangles", trianglesWidgetClass, panel,
    NULL, 0);
  XtAddCallback(triangles, XtNselectCallback, moves_text, NULL);
  initialize(triangles);
  XtRealizeWidget(toplevel);
  XGrabButton(XtDisplay(triangles), AnyButton, AnyModifier, XtWindow(triangles),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(triangles),
    XCreateFontCursor(XtDisplay(triangles), XC_crosshair));
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

  XtVaSetValues(w,
    XtNstart, FALSE,
    NULL);
  XtVaGetValues(w,
    XtNsize, &size,
    NULL);
  XmScaleSetValue(tri, size);
  read_records();
  print_record(size);
}

static void moves_text(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  trianglesCallbackStruct *call_data;
{
  int size;

  XtVaGetValues(w,
    XtNsize, &size,
    NULL);
  (void) strcpy(message_dsp, "");
  switch (call_data->reason) {
    case TRIANGLES_RESTORE:
    case TRIANGLES_RESET:
      moves_dsp = 0;
      break;
    case TRIANGLES_BLOCKED:
      (void) strcpy(message_dsp, "Blocked");
      break;
    case TRIANGLES_SPACE:
      /*(void) strcpy(message_dsp, "Spaces can't move");*/  /* Too annoying */
      break;
    case TRIANGLES_IGNORE:
      (void) strcpy(message_dsp, "Randomize to start");
      break;
    case TRIANGLES_MOVED:
      moves_dsp++;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
    case TRIANGLES_SOLVED:
      if (handle_solved(moves_dsp, size))
        (void) sprintf(message_dsp, "Congratulations %s!!", getlogin());
      else
        (void) strcpy(message_dsp, "Solved!");
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case TRIANGLES_RANDOMIZE:
      moves_dsp = 0;
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case TRIANGLES_DEC:
      moves_dsp = 0;
      size--;
      print_record(size);
      XtSetArg(arg[0], XtNsize, size);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(tri, size);
      break;
    case TRIANGLES_INC:
      moves_dsp = 0;
      size++;
      print_record(size);
      XtSetArg(arg[0], XtNsize, size);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(tri, size);
      break;
    case TRIANGLES_COMPUTED:
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case TRIANGLES_UNDO:
      moves_dsp--;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
  }
  wprintf(message, "%s", message_dsp);
  wprintf(moves, "%d", moves_dsp);
}

static void tri_slider(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmScaleCallbackStruct *cbs;
{
  int size = cbs->value, old;

  XtVaGetValues(triangles,
    XtNsize, &old,
    NULL);  
  if (old != size) {
    XtVaSetValues(triangles,
      XtNsize, size,
      NULL);
    moves_dsp = 0;
    wprintf(moves, "%d", moves_dsp);
    print_record(size);
  }
}

static void print_record(size)
  int size;
{
  if (triangles_record[size - MINTRIANGLES] >= MAXRECORD)
    wprintf(record, "NEVER");
  else
    wprintf(record, "%d", triangles_record[size - MINTRIANGLES]);
}


static int handle_solved(counter, size)
  int counter, size;
{
  if (counter < triangles_record[size - MINTRIANGLES]) {
    triangles_record[size - MINTRIANGLES] = counter;
    write_records();
    print_record(size);
    return TRUE;
  }
  return FALSE;
}

static void read_records()
{
  FILE *fp;
  int i, n;

  for (i = 0; i < MAXTRIANGLES - MINTRIANGLES + 1; i++)
    triangles_record[i] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    wprintf(message, "Can not open %s, taking defaults.\n", SCOREFILE);
  else {
    for (i = 0; i < SAVEDMAXTRIANGLES - MINTRIANGLES + 1; i++) {
      (void) fscanf(fp, "%d", &n);
      triangles_record[i] = n;
    }
    (void) fclose(fp);
  }
}

static void write_records()
{
  FILE *fp;
  int i;

  if ((fp = fopen(SCOREFILE, "w")) == NULL)
    wprintf(message, "Can not write to %s.\n", SCOREFILE);
  else {
    for (i = 0; i < SAVEDMAXTRIANGLES - MINTRIANGLES + 1; i++)
      (void) fprintf(fp, "%d ", triangles_record[i]);
    (void) fprintf(fp, "\n");
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
