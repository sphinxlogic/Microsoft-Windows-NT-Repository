/*
# MOTIF-BASED SKEWB
#
#  xmskewb.c
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
  Version 4: 94/05/30 Xt
  Version 3: 93/10/03 Motif
  Version 2: 92/01/22 XView
  Version 1: 91/03/19 SunView
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
#include <Xm/ToggleB.h>
#include "Skewb.h"
#include "Skewb2d.h"
#include "Skewb3d.h"
#include "skewb.xbm"
#include "mouse-l.xbm"
#include "mouse-m.xbm"
#include "mouse-r.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/mball.scores"
#endif

/* The following is in SkewbP.h also */
#define MAXFACES 6

#define MAXRECORD 32767

static void initialize();
static void moves_text();

static void print_record();
static int handle_solved();
static void read_records();
static void write_records();

static void orient_toggle();
static void wprintf();

static Arg arg[4];
static Widget moves, record, message, skewb2d, skewb3d, orient_switch;
static int skewb_record[2], moves_dsp = 0;
static char message_dsp[128] = "Randomize to start";

static void usage()
{
  (void) fprintf(stderr, "usage: xmskewb\n");
  (void) fprintf(stderr,
    "\t[-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]]\n");
  (void) fprintf(stderr,
    "\t[-display [{host}]:[{vs}]][-fg {color}] [-bg {color}]\n");
  (void) fprintf(stderr,
    "\t[-[no]orient] [-mono] [-face{0|1|2|3|4|5} {color}]\n");
  exit(1);
}

static XrmOptionDescRec options[] = {
  {"-fg",		"*skewb.Foreground",	XrmoptionSepArg,	NULL},
  {"-bd",		"*skewb.Foreground",	XrmoptionSepArg,	NULL},
  {"-bg",		"*Background",		XrmoptionSepArg,	NULL},
  {"-foreground",	"*skewb.Foreground",	XrmoptionSepArg,	NULL},
  {"-background",	"*Background",		XrmoptionSepArg,	NULL},
  {"-bordercolor",	"*skewb.Foreground",	XrmoptionSepArg,	NULL},
  {"-orient",		"*skewb.orient",	XrmoptionNoArg,		"TRUE"},
  {"-noorient",		"*skewb.orient",	XrmoptionNoArg,		"FALSE"},
  {"-mono",		"*skewb.mono",		XrmoptionNoArg,		"TRUE"},
  {"-face0",		"*skewb.faceColor0",	XrmoptionSepArg,	NULL},
  {"-face1",		"*skewb.faceColor1",	XrmoptionSepArg,	NULL},
  {"-face2",		"*skewb.faceColor2",	XrmoptionSepArg,	NULL},
  {"-face3",		"*skewb.faceColor3",	XrmoptionSepArg,	NULL},
  {"-face4",		"*skewb.faceColor4",	XrmoptionSepArg,	NULL},
  {"-face5",		"*skewb.faceColor5",	XrmoptionSepArg,	NULL},
};

int main(argc, argv)
  int argc;
  char *argv[];
{
  Widget toplevel;
  Widget panel, panel2, rowcol, rowcol2;
  Pixmap mouse_left_cursor, mouse_middle_cursor, mouse_right_cursor;
  Pixel fg, bg;

  toplevel = XtInitialize(argv[0], "Skewb",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) skewb_bits, skewb_width, skewb_height));
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
  orient_switch = XtVaCreateManagedWidget ("Orient",
    xmToggleButtonWidgetClass, rowcol2,
    NULL);
  XtAddCallback(orient_switch, XmNvalueChangedCallback, orient_toggle, NULL);
  message = XtVaCreateManagedWidget("Play Skewb! (use mouse and keypad)",
    xmLabelWidgetClass, rowcol2,
    NULL);

  skewb2d = XtCreateManagedWidget("skewb", skewb2dWidgetClass, panel,
    NULL, 0);
  XtVaSetValues(skewb2d,
    XtNheight, 200,
    NULL);
  XtAddCallback(skewb2d, XtNselectCallback, moves_text, NULL);
  skewb3d = XtCreateManagedWidget("skewb", skewb3dWidgetClass, panel,
    NULL, 0);
  XtVaSetValues(skewb3d,
    XtNheight, 200,
    NULL);
  XtAddCallback(skewb3d, XtNselectCallback, moves_text, NULL);
  initialize();
  XtRealizeWidget(toplevel);
  XGrabButton(XtDisplay(skewb2d), AnyButton, AnyModifier, XtWindow(skewb2d),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(skewb2d),
    XCreateFontCursor(XtDisplay(skewb2d), XC_crosshair));
  XGrabButton(XtDisplay(skewb3d), AnyButton, AnyModifier, XtWindow(skewb3d),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(skewb3d),
    XCreateFontCursor(XtDisplay(skewb3d), XC_crosshair));
  XtMainLoop();

#ifdef VMS
  return 1;
#else
  return 0;
#endif
}

/* There's probably a better way to assure that they are the same but
   I don't know it off hand. */
static void make_equivalent(orient)
  Boolean *orient;
{
  Boolean mono;
  Pixel foreground, background;
  String facecolor[MAXFACES];

  XtVaGetValues(skewb2d,
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
  XtVaSetValues(skewb2d,
    XtNface, SKEWB_IGNORE,
    XtNpractice, FALSE,
    XtNstart, FALSE,
    NULL);
  XtVaSetValues(skewb3d,
    XtNorient, *orient,
    XtNmono, mono,
    XtNface, SKEWB_IGNORE,
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
  Boolean orient;

  make_equivalent(&orient);
  XmToggleButtonSetState(orient_switch, orient, True);
  read_records();
  print_record(orient);
}

static void moves_text(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  skewbCallbackStruct *call_data;
{
  Boolean orient, practice;
  Widget otherw;

  if (w == skewb2d)
    otherw = skewb3d;
  else /* (w == skewb3d) */
    otherw = skewb2d;
  XtVaGetValues(w,
    XtNorient, &orient,
    XtNpractice, &practice,
    NULL);
  (void) strcpy(message_dsp, "");
  switch (call_data->reason) {
    case SKEWB_RESTORE:
      XtSetArg(arg[0], XtNface, SKEWB_RESTORE);
      XtSetValues(otherw, arg, 1);
      XtSetValues(w, arg, 1);
      moves_dsp = 0;
      break;
    case SKEWB_RESET:
      moves_dsp = 0;
      break;
    case SKEWB_IGNORE:
      (void) strcpy(message_dsp, "Randomize to start");
      break;
    case SKEWB_MOVED:
      moves_dsp++;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetArg(arg[1], XtNface, call_data->face);
      XtSetArg(arg[2], XtNcube, call_data->corner);
      XtSetArg(arg[3], XtNdirection, call_data->direction);
      XtSetValues(otherw, arg, 4);
      XtSetValues(w, arg, 1);
      break;
    case SKEWB_CONTROL:
      XtSetArg(arg[0], XtNface, call_data->face);
      XtSetArg(arg[1], XtNcube, call_data->corner);
      XtSetArg(arg[2], XtNdirection, call_data->direction);
      XtSetValues(otherw, arg, 3);
      return;
    case SKEWB_SOLVED:
      if (practice)
        moves_dsp = 0;
      else { 
        if (handle_solved(moves_dsp, orient))
          (void) sprintf(message_dsp, "Congratulations %s!!", getlogin());
        else
          (void) strcpy(message_dsp, "Solved!");
      }
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case SKEWB_PRACTICE:
      moves_dsp = 0;
      practice = !practice;
      if (practice)
        wprintf(record, "practice");
      else {
        (void) strcpy(message_dsp, "Randomize to start");
        print_record(orient);
      }
      XtSetArg(arg[0], XtNpractice, practice);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      XtSetValues(otherw, arg, 2);
      break;
    case SKEWB_RANDOMIZE:
      moves_dsp = 0;
      XtSetArg(arg[0], XtNpractice, FALSE);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      XtSetValues(otherw, arg, 2);
      break; 
    case SKEWB_ORIENT:
      moves_dsp = 0;
      orient = !orient;
      print_record(orient);
      XtSetArg(arg[0], XtNorient, orient);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      XmToggleButtonSetState(orient_switch, orient, True);
      break;
    case SKEWB_COMPUTED:
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case SKEWB_UNDO:
      moves_dsp--;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetArg(arg[1], XtNface, call_data->face);
      XtSetArg(arg[2], XtNcube, call_data->corner);
      XtSetArg(arg[3], XtNdirection, call_data->direction);
      XtSetValues(otherw, arg, 4);
      XtSetValues(w, arg, 1);
      break;
  }
  wprintf(message, "%s", message_dsp);
  wprintf(moves, "%d", moves_dsp);
}

static void orient_toggle(w, client_data, cbs)
  Widget w;
  XtPointer client_data;
  XmToggleButtonCallbackStruct *cbs;
{
  Boolean orient = cbs->set;

  XtVaSetValues(skewb2d,
    XtNorient, orient,
    NULL);
  XtVaSetValues(skewb3d,
    XtNorient, orient,
    NULL);
  moves_dsp = 0;
  wprintf(moves, "%d", moves_dsp);
  print_record(orient);
}

static void print_record(orient)
  Boolean orient;
{
  int i = (orient) ? 1 : 0;

  if (skewb_record[i] >= MAXRECORD)
    wprintf(record, "NEVER");
  else
    wprintf(record, "%d", skewb_record[i]);
}

static int handle_solved(counter, orient)
  int counter;
  Boolean orient;
{
  int i = (orient) ? 1 : 0;

  if (counter < skewb_record[i]) {
    skewb_record[i] = counter;
    if (orient && (skewb_record[i] < skewb_record[!i]))
      skewb_record[!i] = counter;
    write_records();
    print_record(orient);
    return TRUE;
  }
  return FALSE;
}

static void read_records()
{
  FILE *fp;
  int n, orient;

  for (orient = 0; orient < 2; orient++)
    skewb_record[orient] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    wprintf(message, "Can not open %s, taking defaults.", SCOREFILE);
  else {
    for (orient = 0; orient < 2; orient++) {
      (void) fscanf(fp, "%d", &n);
      skewb_record[orient] = n;
    }
    (void) fclose(fp);
  }
}

static void write_records()
{
  FILE *fp;
  int orient;

  if ((fp = fopen(SCOREFILE, "w")) == NULL)
    wprintf(message, "Can not write to %s.", SCOREFILE);
  else {
    for (orient = 0; orient < 2; orient++)
      (void) fprintf(fp, "%d ", skewb_record[orient]);
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
