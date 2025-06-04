/*
# X-BASED PYRAMINX(tm)
#
#  xpyraminx.c
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
  Version 4: 94/05/31 Xt
  Version 3: 93/04/01 Motif
  Version 2: 92/01/29 XView
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
#include "Pyraminx.h"
#include "pyraminx.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/pyraminx.scores"
#endif
 
/* The following 6 are in PyraminxP.h also */
#define MINTETRAS 1
#define MAXTETRAS 7
#define PERIOD2 2
#define PERIOD3 3
#define BOTH 4
#define MAXMODES 3

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
 
static Arg arg[5];
static int pyraminx_record[MAXMODES][2][MAXTETRAS - MINTETRAS + 2];
static int moves_dsp = 0;
static char prog_dsp[64] = "xpyraminx";
static char record_dsp[16] = "INF";
static char message_dsp[128] = "Randomize to start";
static char title_dsp[256] = "";
static int oldsize;

static void usage()
{
  (void) fprintf(stderr, "usage: xpyraminx\n");
  (void) fprintf(stderr,
    "\t[-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]]\n");
  (void) fprintf(stderr,
    "\t[-display [{host}]:[{vs}]][-fg {color}] [-bg {color}]\n");
  (void) fprintf(stderr,
    "\t[-{size {int} | sticky}] [-[no]orient] [-mono]\n");
  (void) fprintf(stderr,
    "\t[-{mode {int} | both}] [-face{0|1|2|3} {color}]\n");
  exit(1);
}

static XrmOptionDescRec options[] = {
  {"-fg",		"*pyraminx.Foreground",	XrmoptionSepArg,	NULL},
  {"-bd",		"*pyraminx.Foreground",	XrmoptionSepArg,	NULL},
  {"-bg",		"*Background",		XrmoptionSepArg,	NULL},
  {"-foreground",	"*pyraminx.Foreground",	XrmoptionSepArg,	NULL},
  {"-background",	"*Background",		XrmoptionSepArg,	NULL},
  {"-bordercolor",	"*pyraminx.Foreground",	XrmoptionSepArg,	NULL},
  {"-size",		"*pyraminx.size",	XrmoptionSepArg,	NULL},
  {"-sticky",		"*pyraminx.sticky",	XrmoptionNoArg,		"FALSE"},
  {"-orient",		"*pyraminx.orient",	XrmoptionNoArg,		"TRUE"},
  {"-noorient",		"*pyraminx.orient",	XrmoptionNoArg,		"FALSE"},
  {"-mode",		"*pyraminx.mode",	XrmoptionSepArg,	NULL},
  {"-both",		"*pyraminx.mode",	XrmoptionNoArg,		"4"},
  {"-mono",		"*pyraminx.mono",	XrmoptionNoArg,		"TRUE"},
  {"-face0",		"*pyraminx.faceColor0",	XrmoptionSepArg,	NULL},
  {"-face1",		"*pyraminx.faceColor1",	XrmoptionSepArg,	NULL},
  {"-face2",		"*pyraminx.faceColor2",	XrmoptionSepArg,	NULL},
  {"-face3",		"*pyraminx.faceColor3",	XrmoptionSepArg,	NULL}
};

int main(argc, argv)
  int argc;
  char *argv[];
{
  Widget toplevel, pyraminx; 

  toplevel = XtInitialize(argv[0], "Pyraminx",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) pyraminx_bits, pyraminx_width, pyraminx_height));
  XtSetValues(toplevel, arg, 1);
  pyraminx = XtCreateManagedWidget("pyraminx", pyraminxWidgetClass, toplevel,
    NULL, 0);
  XtAddCallback(pyraminx, XtNselectCallback, moves_text, NULL);
  initialize(pyraminx);
  XtRealizeWidget(toplevel);
  XGrabButton(XtDisplay(pyraminx), AnyButton, AnyModifier, XtWindow(pyraminx),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(pyraminx),
    XCreateFontCursor(XtDisplay(pyraminx), XC_crosshair));
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
  read_records();
  print_record(size, mode, orient, sticky, record_dsp);
  oldsize = size;
  print_state(XtParent(w), prog_dsp, mode, size, sticky, moves_dsp,
    record_dsp, message_dsp);
}

static void moves_text(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  pyraminxCallbackStruct *call_data;
{
  int size, mode;
  Boolean orient, practice, sticky;

  XtVaGetValues(w,
    XtNsize, &size,
    XtNorient, &orient,
    XtNmode, &mode,
    XtNorient, &orient,
    XtNsticky, &sticky,
    XtNpractice, &practice,
    NULL);
  (void) strcpy(message_dsp, "");
  switch (call_data->reason) {
    case PYRAMINX_RESTORE:
      if (practice)
        (void) strcpy(record_dsp, "practice");
      moves_dsp = 0;
      break;
    case PYRAMINX_RESET:
      moves_dsp = 0;
      break;
    case PYRAMINX_IGNORE:
      (void) strcpy(message_dsp, "Randomize to start");
      break;
    case PYRAMINX_MOVED:
      moves_dsp++;
#ifdef DEBUG
      if (moves_dsp > 256)
        exit(1);
#endif
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
    case PYRAMINX_CONTROL:
      return;
    case PYRAMINX_SOLVED:
      if (practice)
        moves_dsp = 0;
      else { 
        if (handle_solved(moves_dsp, size, mode, orient, sticky))
          (void) sprintf(message_dsp, "Congratulations %s!!", getenv("USER"));
        else
          (void) strcpy(message_dsp, "Solved!");
      }
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case PYRAMINX_PRACTICE:
      moves_dsp = 0;
      practice = !practice;
      if (practice)
        (void) strcpy(record_dsp, "practice");
      else {
        (void) strcpy(message_dsp, "Randomize to start");
        print_record(size, mode, orient, sticky, record_dsp);
      }
      XtSetArg(arg[0], XtNpractice, practice);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      break;
    case PYRAMINX_RANDOMIZE:
      moves_dsp = 0;
      XtSetArg(arg[0], XtNpractice, FALSE);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      break; 
    case PYRAMINX_DEC:
      if (!sticky) {
        moves_dsp = 0;
        size--;
        oldsize = size;
        print_record(size, mode, orient, sticky, record_dsp);
        XtSetArg(arg[0], XtNsize, size);
        XtSetValues(w, arg, 1);
      }
      break;
    case PYRAMINX_ORIENT:
      moves_dsp = 0;
      orient = !orient;
      print_record(size, mode, orient, sticky, record_dsp);
      XtSetArg(arg[0], XtNorient, orient);
      XtSetValues(w, arg, 1);
      break;
    case PYRAMINX_INC:
      if (!sticky) {
        moves_dsp = 0;
        size++;
        oldsize = size;
        print_record(size, mode, orient, sticky, record_dsp);
        XtSetArg(arg[0], XtNsize, size);
        XtSetValues(w, arg, 1);
      }
      break;
    case PYRAMINX_PERIOD2:
    case PYRAMINX_PERIOD3:
    case PYRAMINX_BOTH:
      moves_dsp = 0;
      mode = call_data->reason - PYRAMINX_PERIOD2 + PERIOD2;
      print_record(size, mode, orient, sticky, record_dsp);
      XtSetArg(arg[0], XtNmode, mode);
      XtSetValues(w, arg, 1);
      break;
    case PYRAMINX_STICKY:
      moves_dsp = 0;
      sticky = !sticky;
      if (sticky)
        size = 4;
      else
        size = oldsize;
      print_record(size, mode, orient, sticky, record_dsp);
      XtSetArg(arg[0], XtNsticky, sticky);
      XtSetArg(arg[1], XtNsize, size);
      XtSetValues(w, arg, 2);
      break;
    case PYRAMINX_COMPUTED:
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      break;
    case PYRAMINX_UNDO:
      moves_dsp--;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetValues(w, arg, 1);
      break;
  }
  print_state(XtParent(w), prog_dsp, mode, size, sticky, moves_dsp,
    record_dsp, message_dsp);
}

static void print_record(size, mode, orient, sticky, record)
  int size, mode;
  Boolean orient, sticky;
  char *record;
{
  int i = mode - PERIOD2;
  int j = (orient) ? 1 : 0;  
  int k = (sticky) ? MAXTETRAS - MINTETRAS + 1 : size - MINTETRAS;
 
  if (pyraminx_record[i][j][k] >= MAXRECORD)
    (void) strcpy(record, "NEVER");
  else
    (void) sprintf(record, "%d", pyraminx_record[i][j][k]);
}

static int handle_solved(counter, size, mode, orient, sticky)
  int counter, size, mode;
  Boolean orient, sticky;
{
  int i = mode - PERIOD2;
  int j = (orient) ? 1 : 0;
  int k = (sticky) ? MAXTETRAS - MINTETRAS + 1 : size - MINTETRAS;

  if (counter < pyraminx_record[i][j][k]) {
    pyraminx_record[i][j][k] = counter;
    if (size < 4 || mode == PERIOD2 || (orient &&
        (pyraminx_record[i][j][k] < pyraminx_record[i][!j][k])))
      pyraminx_record[i][!j][k] = counter;
    write_records();
    (void) sprintf(record_dsp, "%d", counter);
    return TRUE;
  }
  return FALSE;
}

static void print_state(w, prog, mode, size, sticky, moves, record, message)
  Widget w;
  char *prog, *record, *message;
  int mode, size, moves;
  Boolean sticky;
{
  char ss[10], mb[10];

  if (sticky)
    (void) strcpy(ss, "sticky");
  else
    (void) sprintf(ss, "%d", size);
  if (mode == BOTH)
    (void) strcpy(mb, "both");
  else
    (void) sprintf(mb, "%d", mode);
  (void) sprintf(title_dsp, "%s.%s: %s@ (%d/%s) - %s", prog, mb, ss, moves,
           record, message);
  XtSetArg(arg[0], XtNtitle, title_dsp);
  XtSetValues(w, arg, 1);
}

static void read_records()
{
  FILE *fp;
  int i, n, mode, orient;

  for (mode = 0; mode < MAXMODES; mode++)
    for (orient = 0; orient < 2; orient++)
      for (i = 0; i <= MAXTETRAS - MINTETRAS + 1; i++)
        pyraminx_record[mode][orient][i] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    (void) sprintf(message_dsp, "Can not open %s, taking defaults.", SCOREFILE);
  else {
    for (mode = 0; mode < MAXMODES; mode++)
      for (orient = 0; orient < 2; orient++)
        for (i = 0; i <= MAXTETRAS - MINTETRAS + 1; i++) {
          (void) fscanf(fp, "%d", &n);
          pyraminx_record[mode][orient][i] = n;
        }
    (void) fclose(fp);
  }
}

static void write_records()
{
  FILE *fp;
  int i, orient, mode;

  if ((fp = fopen(SCOREFILE, "w")) == NULL)
    (void) sprintf(message_dsp, "Can not write to %s.", SCOREFILE);
  else {
    for (mode = 0; mode < MAXMODES; mode++) {
      for (orient = 0; orient < 2; orient++) {
        for (i = 0; i <= MAXTETRAS - MINTETRAS + 1; i++)
          (void) fprintf(fp, "%d ", pyraminx_record[mode][orient][i]);
        (void) fprintf(fp, "\n");
      }
      (void) fprintf(fp, "\n");
    }
    (void) fclose(fp);
  }
}
