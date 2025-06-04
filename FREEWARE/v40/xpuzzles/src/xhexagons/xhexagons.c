/*
# X-BASED HEXAGONS
#
#  xhexagons.c
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
#include "Hexagons.h"
#include "hexagons.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/hexagons.scores"
#endif
 
/* The following 5 are in HexagonsP.h also */
#define MINHEXAGONS 1
#define MAXHEXAGONS 12
#define NOCORN 0
#define CORNERS 1
#define MAXORIENT 2

#define SAVEDMAXHEXAGONS MAXHEXAGONS
#define MAXRECORD 32767
#define MAXPROGNAME 80
#define MAXNAME 256

static void initialize();
static void moves_text();

static void print_record();
static int handle_solved();
static void print_state();
static void read_records();
static void write_records();
 
static Arg arg[1];
static int hexagons_record[MAXORIENT][MAXHEXAGONS - MINHEXAGONS + 1],
  moves_dsp = 0;
static char prog_dsp[64] = "xhexagons";
static char record_dsp[16] = "INF";
static char message_dsp[128] = "Randomize to start";
static char title_dsp[256] = "";

static void usage()
{
  (void) fprintf(stderr, "usage: xhexagons\n");
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
  Widget toplevel, hexagons; 

  toplevel = XtInitialize(argv[0], "Hexagons",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) hexagons_bits, hexagons_width, hexagons_height));
  XtSetValues(toplevel, arg, 1);
  hexagons = XtCreateManagedWidget("hexagons", hexagonsWidgetClass, toplevel,
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
  read_records();
  print_record(size, corners, record_dsp);
  print_state(XtParent(w), prog_dsp, size, moves_dsp,
    record_dsp, message_dsp);
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
        (void) sprintf(message_dsp, "Congratulations %s!!", getenv("USER"));
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
      print_record(size, corners, record_dsp);
      XtSetArg(arg[0], XtNsize, size);
      XtSetValues(w, arg, 1);
      break;
    case HEXAGONS_INC:
      moves_dsp = 0;
      size++;
      print_record(size, corners, record_dsp);
      XtSetArg(arg[0], XtNsize, size);
      XtSetValues(w, arg, 1);
      break;
    case HEXAGONS_CORNERS:
      moves_dsp = 0;
      corners = !corners;
      print_record(size, corners, record_dsp);
      XtSetArg(arg[0], XtNcorners, corners);
      XtSetValues(w, arg, 1);
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
  print_state(XtParent(w), prog_dsp, size, moves_dsp,
    record_dsp, message_dsp);
}

static void print_record(size, corners, record)
  int size;
  Boolean corners;
  char *record;
{
  int i = (corners) ? 1 : 0, j = size - MINHEXAGONS;

  if (hexagons_record[i][j] >= MAXRECORD)
    (void) strcpy(record, "NEVER");
  else
    (void) sprintf(record, "%d", hexagons_record[i][j]);
}

static int handle_solved(counter, size, corners)
  int counter, size;
  Boolean corners;
{
  int i = (corners) ? 1 : 0, j = size - MINHEXAGONS;

  if (counter < hexagons_record[i][j]) {
    hexagons_record[i][j] = counter;
    write_records();
    (void) sprintf(record_dsp, "%d", counter);
    return TRUE;
  }
  return FALSE;
}

static void print_state(w, prog, size, moves, record, message)
  Widget w;
  char *prog, *record, *message;
  int size, moves;
{
  (void) sprintf(title_dsp, "%s: %d@ (%d/%s) - %s",
           prog, size, moves, record, message);
  XtSetArg(arg[0], XtNtitle, title_dsp);
  XtSetValues(w, arg, 1);
}

static void read_records()
{
  FILE *fp;
  int i, j, n;

  for (i = 0; i < MAXORIENT; i++)
    for (j = 0; j < MAXHEXAGONS - MINHEXAGONS + 1; j++)
      hexagons_record[i][j] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    (void) sprintf(message_dsp, "Can not open %s, taking defaults.", SCOREFILE);
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
    (void) sprintf(message_dsp, "Can not write to %s.", SCOREFILE);
  else {
    for (i = 0; i < MAXORIENT; i++) {
      for (j = 0; j < SAVEDMAXHEXAGONS - MINHEXAGONS + 1; j++)
        (void) fprintf(fp, "%d ", hexagons_record[i][j]);
      (void) fprintf(fp, "\n");
    }
    (void) fclose(fp);
  }
}
