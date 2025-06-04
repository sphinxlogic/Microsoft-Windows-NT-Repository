/*
# X-BASED MASTERBALL(tm)
#
#  xmball.c
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
#include "Mball.h"
#include "mball.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/mball.scores"
#endif
 
/* The following 4 are in MballP.h also */
#define MINWEDGES 2
#define MAXWEDGES 8
#define MINRINGS 1
#define MAXRINGS 6

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
static int mball_record[2][(MAXWEDGES - MINWEDGES) / 2 + 1]
  [MAXRINGS - MINRINGS + 1];
static int moves_dsp = 0;
static char prog_dsp[64] = "xmball";
static char record_dsp[16] = "INF";
static char message_dsp[128] = "Randomize to start";
static char title_dsp[256] = "";

static void usage()
{
  (void) fprintf(stderr, "usage: xmball\n");
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
  Widget toplevel, mball; 

  toplevel = XtInitialize(argv[0], "Mball",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) mball_bits, mball_width, mball_height));
  XtSetValues(toplevel, arg, 1);
  mball = XtCreateManagedWidget("mball", mballWidgetClass, toplevel,
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
  read_records();
  print_record(orient, wedges, rings, record_dsp);
  print_state(XtParent(w), prog_dsp, wedges, rings, moves_dsp,
    record_dsp, message_dsp);
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
        (void) strcpy(record_dsp, "practice");
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
        (void) strcpy(record_dsp, "practice");
      else {
        (void) strcpy(message_dsp, "Randomize to start");
        print_record(orient, wedges, rings, record_dsp);
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
      print_record(orient, wedges, rings, record_dsp);
      XtSetArg(arg[0], XtNrings, rings);
      XtSetValues(w, arg, 1);
      break;
    case MBALL_ORIENT:
      moves_dsp = 0;
      orient = !orient;
      print_record(orient, wedges, rings, record_dsp);
      XtSetArg(arg[0], XtNorient, orient);
      XtSetValues(w, arg, 1);
      break;
    case MBALL_INC:
      moves_dsp = 0;
      rings++;
      print_record(orient, wedges, rings, record_dsp);
      XtSetArg(arg[0], XtNrings, rings);
      XtSetValues(w, arg, 1);
      break;
    case MBALL_WEDGE2:
    case MBALL_WEDGE4:
    case MBALL_WEDGE6:
    case MBALL_WEDGE8:
      moves_dsp = 0;
      wedges = 2 * (call_data->reason - MBALL_WEDGE2) + MINWEDGES;
      print_record(orient, wedges, rings, record_dsp);
      XtSetArg(arg[0], XtNwedges, wedges);
      XtSetValues(w, arg, 1);
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
  print_state(XtParent(w), prog_dsp, wedges, rings, moves_dsp,
    record_dsp, message_dsp);
}

static void print_record(orient, wedges, rings, record)
  Boolean orient;
  int wedges, rings;
  char *record;
{
  int i = (orient) ? 1 : 0;

  if (mball_record[i][(wedges - MINWEDGES) / 2][rings - MINRINGS] >= MAXRECORD)
    (void) strcpy(record, "NEVER");
  else
    (void) sprintf(record, "%d",
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
    if (orient && mball_record[i][w][r] < mball_record[!i][w][r])
      mball_record[!i][w][r] = counter;
    write_records();
    (void) sprintf(record_dsp, "%d", counter);
    return TRUE;
  }
  return FALSE;
}

static void print_state(w, prog, wedges, rings, moves, record, message)
  Widget w;
  char *prog, *record, *message;
  int wedges, rings, moves;
{
  (void) sprintf(title_dsp, "%s.%d: %d@ (%d/%s) - %s", prog, wedges, rings,
           moves, record, message);
  XtSetArg(arg[0], XtNtitle, title_dsp);
  XtSetValues(w, arg, 1);
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
    (void) sprintf(message_dsp, "Can not open %s, taking defaults.", SCOREFILE);
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
    (void) sprintf(message_dsp, "Can not write to %s.", SCOREFILE);
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
