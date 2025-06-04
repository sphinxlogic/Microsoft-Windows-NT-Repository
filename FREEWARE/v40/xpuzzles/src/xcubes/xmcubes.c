/*
# MOTIF-BASED CUBES
#
#  xmcubes.c
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
  Version 2: 92/12/19 XView
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
#include "Cubes.h"
#include "cubes.xbm"
#include "mouse-l.xbm"
#include "mouse-r.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/cubes.scores"
#endif

/* The following are in CubesP.h also */
#define MINCUBES 1 
#define MAXCUBES 8

#define SAVEDMAXCUBES MAXCUBES
#define MAXRECORD 32767

static void initialize();
static void moves_text();

static void print_record();
static int handle_solved();
static void read_records();
static void write_records();

static void wprintf();
static void box_x_slider();
static void box_y_slider();
static void box_z_slider();

static Arg arg[2];
static Widget moves, record, message, cubes, box_x, box_y, box_z;
static int cubes_record[MAXCUBES - MINCUBES + 1][MAXCUBES - MINCUBES + 1]
  [MAXCUBES - MINCUBES + 1], moves_dsp = 0;
static char message_dsp[128] = "Randomize to start";

static void usage()
{
  (void) fprintf(stderr, "usage: xmcubes\n");
  (void) fprintf(stderr,
    "\t[-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]]\n");
  (void) fprintf(stderr,
    "\t[-display [{host}]:[{vs}]][-fg {color}] [-bg {color}]\n");
  (void) fprintf(stderr,
    "\t[-brick {color}] [-size{x|y|z} {int}]\n");
  exit(1);
}

static XrmOptionDescRec options[] = {
  {"-fg",		"*cubes.Foreground",	XrmoptionSepArg,	NULL},
  {"-bd",		"*cubes.Foreground",	XrmoptionSepArg,	NULL},
  {"-bg",		"*Background",		XrmoptionSepArg,	NULL},
  {"-foreground",	"*cubes.Foreground",	XrmoptionSepArg,	NULL},
  {"-background",	"*Background",		XrmoptionSepArg,	NULL},
  {"-bordercolor",	"*cubes.Foreground",	XrmoptionSepArg,	NULL},
  {"-brick",		"*cubes.brickColor",	XrmoptionSepArg,	NULL},
  {"-sizex",		"*cubes.sizeX",		XrmoptionSepArg,	NULL},
  {"-sizey",		"*cubes.sizeY",		XrmoptionSepArg,	NULL},
  {"-sizez",		"*cubes.sizeZ",		XrmoptionSepArg,	NULL},
};

int main(argc, argv)
  int argc;
  char *argv[];
{
  Widget toplevel;
  Widget panel, panel2, rowcol, rowcol2;
  Pixmap mouse_left_cursor, mouse_right_cursor;
  Pixel fg, bg;

  toplevel = XtInitialize(argv[0], "Cubes",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();
 
  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) cubes_bits, cubes_width, cubes_height));
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
    XtVaTypedArg, XmNlabelString, XmRString, "Move brick", 10, NULL);
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
  box_x = XtVaCreateManagedWidget("boxs_x", xmScaleWidgetClass, rowcol2,
    XtVaTypedArg, XmNtitleString, XmRString, "Rects X", 8,
    XmNminimum, MINCUBES,
    XmNmaximum, MAXCUBES,
    XmNvalue, MINCUBES,
    XmNshowValue, True,
    XmNorientation, XmHORIZONTAL,
    NULL);
  XtAddCallback(box_x, XmNvalueChangedCallback, box_x_slider, NULL);
  box_y = XtVaCreateManagedWidget("boxs_y", xmScaleWidgetClass, rowcol2,
    XtVaTypedArg, XmNtitleString, XmRString, "Rects Y", 8,
    XmNminimum, MINCUBES,
    XmNmaximum, MAXCUBES,
    XmNvalue, MINCUBES,
    XmNshowValue, True,
    XmNorientation, XmHORIZONTAL,
    NULL);
  XtAddCallback(box_y, XmNvalueChangedCallback, box_y_slider, NULL);
  box_z = XtVaCreateManagedWidget("boxs_z", xmScaleWidgetClass, rowcol2,
    XtVaTypedArg, XmNtitleString, XmRString, "Rects Z", 8,
    XmNminimum, MINCUBES,
    XmNmaximum, MAXCUBES,
    XmNvalue, MINCUBES,
    XmNshowValue, True,
    XmNorientation, XmHORIZONTAL,
    NULL);
  XtAddCallback(box_z, XmNvalueChangedCallback, box_z_slider, NULL);
  message = XtVaCreateManagedWidget("Play Cubes! (use mouse or keypad)",
    xmLabelWidgetClass, rowcol2,
    NULL);

  cubes = XtCreateManagedWidget("cubes", cubesWidgetClass, panel, NULL,
    0);
  XtAddCallback(cubes, XtNselectCallback, moves_text, NULL);
  initialize(cubes);
  XtRealizeWidget(toplevel);
  XGrabButton(XtDisplay(cubes), AnyButton, AnyModifier, XtWindow(cubes),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(cubes),
    XCreateFontCursor(XtDisplay(cubes), XC_crosshair));
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
  int size_x, size_y, size_z;
 
  XtVaSetValues(w,
    XtNstart, FALSE,
    NULL);
  XtVaGetValues(w,
    XtNsizeX, &size_x,
    XtNsizeY, &size_y,
    XtNsizeZ, &size_z,
    NULL);
  XmScaleSetValue(box_x, size_x);
  XmScaleSetValue(box_y, size_y);
  XmScaleSetValue(box_z, size_z);
  read_records();
  print_record(size_x, size_y, size_z);
}

static void moves_text(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  cubesCallbackStruct *call_data;
{
  int size_x, size_y, size_z;
 
  XtVaGetValues(w,
    XtNsizeX, &size_x,
    XtNsizeY, &size_y,
    XtNsizeZ, &size_z,
    NULL);
  (void) strcpy(message_dsp, "");
  switch (call_data->reason) {
    case CUBES_RESTORE:
    case CUBES_RESET:
      moves_dsp = 0;
      break;
    case CUBES_BLOCKED:
      (void) strcpy(message_dsp, "Blocked");
      break;
    case CUBES_SPACE:
      /*(void) strcpy(message_dsp, "Spaces can't move");*/  /* Too annoying */
      break;
    case CUBES_IGNORE:
      (void) strcpy(message_dsp, "Randomize to start");
      break;
    case CUBES_MOVED:
      moves_dsp++;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
    case CUBES_SOLVED:
      if (handle_solved(moves_dsp, size_x, size_y, size_z))
        (void) sprintf(message_dsp, "Congratulations %s!!", getlogin());
      else
        (void) strcpy(message_dsp, "Solved!");
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case CUBES_RANDOMIZE:
      moves_dsp = 0;
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case CUBES_DEC_X:
      moves_dsp = 0;
      size_x--;
      print_record(size_x, size_y, size_z);
      XtSetArg(arg[0], XtNsizeX, size_x);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(box_x, size_x);
      break;
    case CUBES_INC_X:
      moves_dsp = 0;
      size_x++;
      print_record(size_x, size_y, size_z);
      XtSetArg(arg[0], XtNsizeX, size_x);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(box_x, size_x);
      break;
    case CUBES_DEC_Y:
      moves_dsp = 0;
      size_y--;
      print_record(size_x, size_y, size_z);
      XtSetArg(arg[0], XtNsizeY, size_y);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(box_y, size_y);
      break;
    case CUBES_INC_Y:
      moves_dsp = 0;
      size_y++;
      print_record(size_x, size_y, size_z);
      XtSetArg(arg[0], XtNsizeY, size_y);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(box_y, size_y);
      break;
    case CUBES_DEC_Z:
      moves_dsp = 0;
      size_z--;
      print_record(size_x, size_y, size_z);
      XtSetArg(arg[0], XtNsizeZ, size_z);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(box_z, size_z);
      break;
    case CUBES_INC_Z:
      moves_dsp = 0;
      size_z++;
      print_record(size_x, size_y, size_z);
      XtSetArg(arg[0], XtNsizeZ, size_z);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(box_z, size_z);
      break;
    case CUBES_COMPUTED:
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case CUBES_UNDO:
      moves_dsp--;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
  }
  wprintf(message, "%s", message_dsp);
  wprintf(moves, "%d", moves_dsp);
}

static void box_x_slider(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmScaleCallbackStruct *cbs;
{
  int size_x = cbs->value, size_y, size_z, old_x;

  XtVaGetValues(cubes,
    XtNsizeX, &old_x,
    XtNsizeY, &size_y,
    XtNsizeZ, &size_z,
    NULL);  
  if (old_x != size_x) {
    XtVaSetValues(cubes,
      XtNsizeX, size_x,
      NULL);
    moves_dsp = 0;
    wprintf(moves, "%d", moves_dsp);
    print_record(size_x, size_y, size_z);
  }
}

static void box_y_slider(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmScaleCallbackStruct *cbs;
{
  int size_x, size_y = cbs->value, size_z, old_y;

  XtVaGetValues(cubes,
    XtNsizeX, &size_x,
    XtNsizeY, &old_y,
    XtNsizeZ, &size_z,
    NULL);  
  if (old_y != size_y) {
    XtVaSetValues(cubes,
      XtNsizeY, size_y,
      NULL);
    moves_dsp = 0;
    wprintf(moves, "%d", moves_dsp);
    print_record(size_x, size_y, size_z);
  }
}

static void box_z_slider(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmScaleCallbackStruct *cbs;
{
  int size_x, size_y, size_z = cbs->value, old_z;

  XtVaGetValues(cubes,
    XtNsizeX, &size_x,
    XtNsizeY, &size_y,
    XtNsizeZ, &old_z,
    NULL);  
  if (old_z != size_z) {
    XtVaSetValues(cubes,
      XtNsizeZ, size_z,
      NULL);
    moves_dsp = 0;
    wprintf(moves, "%d", moves_dsp);
    print_record(size_x, size_y, size_z);
  }
}

static void print_record(size_x, size_y, size_z)
  int size_x, size_y, size_z;
{
  int i = size_x - MINCUBES, j = size_y - MINCUBES, k = size_z - MINCUBES;
 
  if (cubes_record[i][j][k] >= MAXRECORD)
    wprintf(record, "NEVER");
  else
    wprintf(record, "%d", cubes_record[i][j][k]);
}

static int handle_solved(counter, size_x, size_y, size_z)
  int counter, size_x, size_y, size_z;
{
  int i = size_x - MINCUBES, j = size_y - MINCUBES, k = size_z - MINCUBES;
 
  if (counter < cubes_record[i][j][k]) {
    cubes_record[i][j][k] = cubes_record[i][k][j] = cubes_record[j][i][k] =
      cubes_record[j][k][i] = cubes_record[k][i][j] = cubes_record[k][j][i] =
      counter;
    write_records();
    print_record(size_x, size_y, size_z);
    return TRUE;
  }
  return FALSE;
}

static void read_records()
{
  FILE *fp;
  int i, j, k, n;
 
  for (i = 0; i < MAXCUBES - MINCUBES + 1; i++)
    for (j = i; j < MAXCUBES - MINCUBES + 1; j++)
      for (k = j; k < MAXCUBES - MINCUBES + 1; k++)
        cubes_record[k][j][i] = cubes_record[k][i][j] =
          cubes_record[j][k][i] = cubes_record[j][i][k] =
          cubes_record[i][k][j] = cubes_record[i][j][k] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    wprintf(message, "Can not open %s, taking defaults.\n", SCOREFILE);
  else {
    for (i = 0; i < SAVEDMAXCUBES - MINCUBES + 1; i++)
      for (j = i; j < SAVEDMAXCUBES - MINCUBES + 1; j++)
        for (k = j; k < SAVEDMAXCUBES - MINCUBES + 1; k++) {
          (void) fscanf(fp, "%d", &n);
          cubes_record[k][j][i] = cubes_record[k][i][j] =
            cubes_record[j][k][i] = cubes_record[j][i][k] =
            cubes_record[i][k][j] = cubes_record[i][j][k] = n;
        }
    (void) fclose(fp);
  }
}

static void write_records()
{
  FILE *fp;
  int i, j, k;
 
  if ((fp = fopen(SCOREFILE, "w")) == NULL)
    wprintf(message, "Can not write to %s.\n", SCOREFILE);
  else {
    for (i = 0; i < SAVEDMAXCUBES - MINCUBES + 1; i++) {
      for (j = i; j < SAVEDMAXCUBES - MINCUBES + 1; j++) {
        for (k = j; k < SAVEDMAXCUBES - MINCUBES + 1; k++)
          (void) fprintf(fp, "%d ", cubes_record[i][j][k]);
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

