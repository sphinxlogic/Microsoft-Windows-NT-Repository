/*
# MOTIF-BASED MISSING LINK(tm)
#
#  xmmlink.c
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
  Version 5: 95/10/01 Xt/Motif
  Version 1: 94/08/30 Xt
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
#include "Mlink.h"
#include "mlink.xbm"
#include "mouse-l.xbm"
#include "mouse-m.xbm"
#include "mouse-r.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/mlink.scores"
#endif
 
/* The following are in MlinksP.h also */
#define MINTILES 1
#define MAXTILES 8
#define MINFACES 1
#define MAXFACES 8

#define SAVEDMAXTILES MAXTILES
#define SAVEDMAXFACES MAXFACES
#define MAXRECORD 32767

static void initialize();
static void moves_text();

static void print_record();
static int handle_solved();
static void read_records();
static void write_records();

static void wprintf();
static void tile_slider();
static void face_slider();
static void orient_toggle();
static void middle_toggle();
 
static Arg arg[2];
static Widget moves, record, message, mlink, orient_switch, middle_switch,
  tile, face;
static int mlink_record[2][2][MAXFACES - MINFACES + 1][MAXTILES - MINTILES + 1],
  moves_dsp = 0;
static char message_dsp[128] = "Randomize to start";

static void usage()
{
  (void) fprintf(stderr, "usage: xmmlink\n");
  (void) fprintf(stderr,
    "\t[-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]]\n");
  (void) fprintf(stderr,
    "\t[-display [{host}]:[{vs}]][-fg {color}] [-bg {color}]\n");
  (void) fprintf(stderr,
    "\t[-tiles {int}] [-faces {int}] [-[no]orient] [-[no]middle]\n");
  (void) fprintf(stderr,
    "\t[-mono] [-tile {color}] [-face{0|1|2|3|4|5|6|7} {color}]\n");
  exit(1);
}

static XrmOptionDescRec options[] = {
  {"-fg",		"*mlink.Foreground",	XrmoptionSepArg,	NULL},
  {"-bd",		"*mlink.Foreground",	XrmoptionSepArg,	NULL},
  {"-bg",		"*Background",		XrmoptionSepArg,	NULL},
  {"-foreground",	"*mlink.Foreground",	XrmoptionSepArg,	NULL},
  {"-background",	"*Background",		XrmoptionSepArg,	NULL},
  {"-bordercolor",	"*mlink.Foreground",	XrmoptionSepArg,	NULL},
  {"-tile",		"*mlink.tileColor",	XrmoptionSepArg,	NULL},
  {"-tiles",		"*mlink.tiles",		XrmoptionSepArg,	NULL},
  {"-faces",		"*mlink.faces",		XrmoptionSepArg,	NULL},
  {"-orient",		"*mlink.orient",	XrmoptionNoArg,		"TRUE"},
  {"-noorient",		"*mlink.orient",	XrmoptionNoArg,		"FALSE"},
  {"-middle",		"*mlink.middle",	XrmoptionNoArg,		"TRUE"},
  {"-nomiddle",		"*mlink.middle",	XrmoptionNoArg,		"FALSE"},
  {"-mono",		"*mlink.mono",		XrmoptionNoArg,		"TRUE"},
  {"-face0",		"*mlink.faceColor0",	XrmoptionSepArg,	NULL},
  {"-face1",		"*mlink.faceColor1",	XrmoptionSepArg,	NULL},
  {"-face2",		"*mlink.faceColor2",	XrmoptionSepArg,	NULL},
  {"-face3",		"*mlink.faceColor3",	XrmoptionSepArg,	NULL},
  {"-face4",		"*mlink.faceColor4",	XrmoptionSepArg,	NULL},
  {"-face5",		"*mlink.faceColor5",	XrmoptionSepArg,	NULL},
  {"-face6",		"*mlink.faceColor6",	XrmoptionSepArg,	NULL},
  {"-face7",		"*mlink.faceColor7",	XrmoptionSepArg,	NULL}
};

int main(argc, argv)
  int argc;
  char *argv[];
{
  Widget toplevel; 
  Widget panel, panel2, rowcol, rowcol2, rowcol3;
  Pixmap mouse_left_cursor, mouse_middle_cursor, mouse_right_cursor;
  Pixel fg, bg;

  toplevel = XtInitialize(argv[0], "Mlink",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) mlink_bits, mlink_width, mlink_height));
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
    XtVaTypedArg, XmNlabelString, XmRString, "Move tile HIGH", 10, NULL);
  XtVaCreateManagedWidget("mouse_left", xmLabelGadgetClass, rowcol,
    XmNlabelType, XmPIXMAP, XmNlabelPixmap, mouse_left_cursor, NULL);
  XtVaCreateManagedWidget("mouse_middle_text", xmLabelGadgetClass, rowcol,
    XtVaTypedArg, XmNlabelString, XmRString, "Move tile LOW", 15, NULL);
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
    NULL);
  XtVaGetValues(rowcol2, XmNforeground, &fg, XmNbackground, &bg, NULL);
  tile = XtVaCreateManagedWidget("tile", xmScaleWidgetClass, rowcol2,
    XtVaTypedArg, XmNtitleString, XmRString, "Tiles", 6,
    XmNminimum, MINTILES,
    XmNmaximum, MAXTILES,
    XmNvalue, MINTILES,
    XmNshowValue, True,
    XmNorientation, XmHORIZONTAL,
    NULL);
  XtAddCallback(tile, XmNvalueChangedCallback, tile_slider, NULL);
  face = XtVaCreateManagedWidget("face", xmScaleWidgetClass, rowcol2,
    XtVaTypedArg, XmNtitleString, XmRString, "Faces", 6,
    XmNminimum, MINFACES,
    XmNmaximum, MAXFACES,
    XmNvalue, MINFACES,
    XmNshowValue, True,
    XmNorientation, XmHORIZONTAL,
    NULL);
  XtAddCallback(face, XmNvalueChangedCallback, face_slider, NULL);
  rowcol3 = XtVaCreateManagedWidget("Rowcol3", xmRowColumnWidgetClass, rowcol2,
    XmNnumColumns, 1,
    XmNorientation, XmHORIZONTAL,
    XmNpacking, XmPACK_COLUMN,
    NULL);
  orient_switch = XtVaCreateManagedWidget ("Orient",
    xmToggleButtonWidgetClass, rowcol3,
    NULL);
  XtAddCallback(orient_switch, XmNvalueChangedCallback, orient_toggle, NULL);
  middle_switch = XtVaCreateManagedWidget ("Middle",
    xmToggleButtonWidgetClass, rowcol3,
    NULL);
  XtAddCallback(middle_switch, XmNvalueChangedCallback, middle_toggle, NULL);
  message = XtVaCreateManagedWidget("Play Missing Link! (use mouse and keypad)",
    xmLabelWidgetClass, rowcol2,
    NULL);

  mlink = XtCreateManagedWidget("mlink", mlinkWidgetClass, panel,
    NULL, 0);
  XtAddCallback(mlink, XtNselectCallback, moves_text, NULL);
  initialize(mlink);
  XtRealizeWidget(toplevel);
  XGrabButton(XtDisplay(mlink), AnyButton, AnyModifier, XtWindow(mlink),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(mlink),
    XCreateFontCursor(XtDisplay(mlink), XC_crosshair));
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
  int tiles, faces;
  Boolean orient, middle;

  XtVaSetValues(w,
    XtNstart, FALSE,
    NULL);
  XtVaGetValues(w,
    XtNtiles, &tiles,
    XtNfaces, &faces,
    XtNorient, &orient,
    XtNmiddle, &middle,
    NULL);
  XmScaleSetValue(tile, tiles);
  XmScaleSetValue(face, faces);
  XmToggleButtonSetState(orient_switch, orient, True);
  XmToggleButtonSetState(middle_switch, middle, True);
  read_records();
  print_record(tiles, faces, orient, middle);
}

static void moves_text(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  mlinkCallbackStruct *call_data;
{
  int tiles, faces;
  Boolean orient, middle;

  XtVaGetValues(w,
    XtNtiles, &tiles,
    XtNfaces, &faces,
    XtNorient, &orient,
    XtNmiddle, &middle,
    NULL);
  (void) strcpy(message_dsp, "");
  switch (call_data->reason) {
    case MLINK_RESTORE:
    case MLINK_RESET:
      moves_dsp = 0;
      break;
    case MLINK_BLOCKED:
      (void) strcpy(message_dsp, "Blocked");
      break;
    case MLINK_SPACE:
      /*(void) strcpy(message_dsp, "Spaces can't move");*/  /* Too annoying */
      break;
    case MLINK_IGNORE:
      (void) strcpy(message_dsp, "Randomize to start");
      break;
    case MLINK_MOVED:
      moves_dsp++;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
    case MLINK_CONTROL:
      return;
    case MLINK_SOLVED:
      if (handle_solved(moves_dsp, tiles, faces, orient, middle))
        (void) sprintf(message_dsp, "Congratulations %s!!", getenv("USER"));
      else
        (void) strcpy(message_dsp, "Solved!");
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case MLINK_RANDOMIZE:
      moves_dsp = 0;
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case MLINK_ORIENT:
      moves_dsp = 0;
      orient = !orient;
      print_record(tiles, faces, orient, middle);
      XtSetArg(arg[0], XtNorient, orient);
      XtSetValues(w, arg, 1);
      XmToggleButtonSetState(orient_switch, orient, True);
      break;
    case MLINK_MIDDLE:
      moves_dsp = 0;
      middle = !middle;
      print_record(tiles, faces, orient, middle);
      XtSetArg(arg[0], XtNmiddle, middle);
      XtSetValues(w, arg, 1);
      XmToggleButtonSetState(middle_switch, middle, True);
      break;
    case MLINK_DEC_X:
      moves_dsp = 0;
      tiles--;
      print_record(tiles, faces, orient, middle);
      XtSetArg(arg[0], XtNtiles, tiles);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(tile, tiles);
      break;
    case MLINK_INC_X:
      moves_dsp = 0;
      tiles++;
      print_record(tiles, faces, orient, middle);
      XtSetArg(arg[0], XtNtiles, tiles);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(tile, tiles);
      break;
    case MLINK_DEC_Y:
      moves_dsp = 0;
      faces--;
      print_record(tiles, faces, orient, middle);
      XtSetArg(arg[0], XtNfaces, faces);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(face, faces);
      break;
    case MLINK_INC_Y:
      moves_dsp = 0;
      faces++;
      print_record(tiles, faces, orient, middle);
      XtSetArg(arg[0], XtNfaces, faces);
      XtSetValues(w, arg, 1);
      XmScaleSetValue(face, faces);
      break;
    case MLINK_COMPUTED:
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case MLINK_UNDO:
      moves_dsp--;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
  }
  wprintf(message, "%s", message_dsp);
  wprintf(moves, "%d", moves_dsp);
}

static void tile_slider(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmScaleCallbackStruct *cbs;
{
  int tiles = cbs->value, faces, old;
 
  XtVaGetValues(mlink,
    XtNtiles, &old,
    XtNfaces, &faces,
    NULL);
  if (old != tiles) {
    XtVaSetValues(mlink,
      XtNtiles, tiles,
      NULL);
    moves_dsp = 0;
    wprintf(moves, "%d", moves_dsp);
    print_record(tiles, faces, XmToggleButtonGetState(orient_switch),
      XmToggleButtonGetState(middle_switch));
  }
}

static void face_slider(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmScaleCallbackStruct *cbs;
{
  int tiles, faces = cbs->value, old;
 
  XtVaGetValues(mlink,
    XtNtiles, &tiles,
    XtNfaces, &old,
    NULL);
  if (old != faces) {
    XtVaSetValues(mlink,
      XtNfaces, faces,
      NULL);
    moves_dsp = 0;
    wprintf(moves, "%d", moves_dsp);
    print_record(tiles, faces, XmToggleButtonGetState(orient_switch),
      XmToggleButtonGetState(middle_switch));
  }
}
 
static void orient_toggle(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmToggleButtonCallbackStruct *cbs;
{
  int tiles, faces;
  Boolean orient = cbs->set;
 
  XtVaSetValues(mlink,
    XtNorient, orient,
    NULL);
  XtVaGetValues(mlink,
    XtNtiles, &tiles,
    XtNfaces, &faces,
    NULL);
  moves_dsp = 0;
  wprintf(moves, "%d", moves_dsp);
  print_record(tiles, faces, orient, XmToggleButtonGetState(middle_switch));
}
 
static void middle_toggle(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmToggleButtonCallbackStruct *cbs;
{
  int tiles, faces;
  Boolean middle = cbs->set;
 
  XtVaSetValues(mlink,
    XtNmiddle, middle,
    NULL);
  XtVaGetValues(mlink,
    XtNtiles, &tiles,
    XtNfaces, &faces,
    NULL);
  moves_dsp = 0;
  wprintf(moves, "%d", moves_dsp);
  print_record(tiles, faces, XmToggleButtonGetState(orient_switch), middle);
}
 
static void print_record(tiles, faces, orient, middle)
  int tiles, faces;
  Boolean orient, middle;
{
  int i = tiles - MINTILES, j = faces - MINFACES;
  int k = (orient) ? 1 : 0, l = (middle) ? 1 : 0;

  if (mlink_record[l][k][j][i] >= MAXRECORD)
    wprintf(record, "NEVER");
  else
    wprintf(record, "%d", mlink_record[l][k][j][i]);
}

static int handle_solved(counter, tiles, faces, orient, middle)
  int counter, tiles, faces;
  Boolean orient, middle;
{
  int i = tiles - MINTILES, j = faces - MINFACES;
  int k = (orient) ? 1 : 0, l = (middle) ? 1 : 0;

  if (counter < mlink_record[l][k][j][i]) {
    mlink_record[l][k][j][i] = counter;
    if (tiles < 4 || faces < 2)
      mlink_record[!l][k][j][i] = counter;
    write_records();
    print_record(tiles, faces, orient, middle);
    return TRUE;
  }
  return FALSE;
}

static void read_records()
{
  FILE *fp;
  int i, j, k, l, n;

  for (l = 0; l < 2; l++)
    for (k = 0; k < 2; k++)
      for (j = 0; j < MAXFACES - MINFACES + 1; j++)
        for (i = 0; i < MAXTILES - MINTILES + 1; i++)
          mlink_record[l][k][j][i] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    wprintf(message_dsp, "Can not open %s, taking defaults.", SCOREFILE);
  else {
    for (l = 0; l < 2; l++)
      for (k = 0; k < 2; k++)
        for (j = 0; j < SAVEDMAXFACES - MINFACES + 1; j++)
          for (i = 0; i < SAVEDMAXTILES - MINTILES + 1; i++) {
            (void) fscanf(fp, "%d", &n);
            mlink_record[l][k][j][i] = n;
      }
    (void) fclose(fp);
  }
}

static void write_records()
{
  FILE *fp;
  int i, j, k, l;

  if ((fp = fopen(SCOREFILE, "w")) == NULL)
    wprintf(message_dsp, "Can not write to %s.", SCOREFILE);
  else {
    for (l = 0; l < 2; l++) {
      for (k = 0; k < 2; k++) {
        for (j = 0; j < SAVEDMAXFACES - MINFACES + 1; j++) {
          for (i = 0; i < SAVEDMAXTILES - MINTILES + 1; i++)
            (void) fprintf(fp, "%d ", mlink_record[l][k][j][i]);
          (void) fprintf(fp, "\n");
        }
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
