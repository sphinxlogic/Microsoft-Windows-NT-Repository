/*
# X-BASED MISSING LINK(tm)
#
#  xmlink.c
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
#include "Mlink.h"
#include "mlink.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/mlink.scores"
#endif
 
/* The following 4 are in MlinksP.h also */
#define MINTILES 1
#define MAXTILES 8
#define MINFACES 1
#define MAXFACES 8

#define SAVEDMAXTILES MAXTILES
#define SAVEDMAXFACES MAXFACES
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
static int mlink_record[2][2][MAXFACES - MINFACES + 1][MAXTILES - MINTILES + 1];
static int moves_dsp = 0;
static char prog_dsp[64] = "xmlink";
static char record_dsp[16] = "INF";
static char message_dsp[128] = "Randomize to start";
static char title_dsp[256] = "";

static void usage()
{
  (void) fprintf(stderr, "usage: xmlink\n");
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
  Widget toplevel, mlink; 

  toplevel = XtInitialize(argv[0], "Mlink",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) mlink_bits, mlink_width, mlink_height));
  XtSetValues(toplevel, arg, 1);
  mlink = XtCreateManagedWidget("mlink", mlinkWidgetClass, toplevel,
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
  read_records();
  print_record(tiles, faces, orient, middle, record_dsp);
  print_state(XtParent(w), prog_dsp, tiles, faces, middle, moves_dsp,
    record_dsp, message_dsp);
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
      print_record(tiles, faces, orient, middle, record_dsp);
      XtSetArg(arg[0], XtNorient, orient);
      XtSetValues(w, arg, 1);
      break;
    case MLINK_MIDDLE:
      moves_dsp = 0;
      middle = !middle;
      print_record(tiles, faces, orient, middle, record_dsp);
      XtSetArg(arg[0], XtNmiddle, middle);
      XtSetValues(w, arg, 1);
      break;
    case MLINK_DEC_X:
      moves_dsp = 0;
      tiles--;
      print_record(tiles, faces, orient, middle, record_dsp);
      XtSetArg(arg[0], XtNtiles, tiles);
      XtSetValues(w, arg, 1);
      break;
    case MLINK_INC_X:
      moves_dsp = 0;
      tiles++;
      print_record(tiles, faces, orient, middle, record_dsp);
      XtSetArg(arg[0], XtNtiles, tiles);
      XtSetValues(w, arg, 1);
      break;
    case MLINK_DEC_Y:
      moves_dsp = 0;
      faces--;
      print_record(tiles, faces, orient, middle, record_dsp);
      XtSetArg(arg[0], XtNfaces, faces);
      XtSetValues(w, arg, 1);
      break;
    case MLINK_INC_Y:
      moves_dsp = 0;
      faces++;
      print_record(tiles, faces, orient, middle, record_dsp);
      XtSetArg(arg[0], XtNfaces, faces);
      XtSetValues(w, arg, 1);
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
  print_state(XtParent(w), prog_dsp, tiles, faces, middle, moves_dsp,
    record_dsp, message_dsp);
}

static void print_record(tiles, faces, orient, middle, record)
  int tiles, faces;
  Boolean orient, middle;
  char *record;
{
  int i = tiles - MINTILES, j = faces - MINFACES;
  int k = (orient) ? 1 : 0, l = (middle) ? 1 : 0;

  if (mlink_record[l][k][j][i] >= MAXRECORD)
    (void) strcpy(record, "NEVER");
  else
    (void) sprintf(record, "%d", mlink_record[l][k][j][i]);
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
    (void) sprintf(record_dsp, "%d", counter);
    return TRUE;
  }
  return FALSE;
}

static void print_state(w, prog, tiles, faces, middle, moves, record, message)
  Widget w;
  char *prog, *record, *message;
  int tiles, faces, moves;
  Boolean middle;
{
  if (middle)
    (void) sprintf(title_dsp, "%s: %dx%d norm@ (%d/%s) - %s",
             prog, tiles, faces, moves, record, message);
  else
    (void) sprintf(title_dsp, "%s: %dx%d ind@ (%d/%s) - %s",
             prog, tiles, faces, moves, record, message);
  XtSetArg(arg[0], XtNtitle, title_dsp);
  XtSetValues(w, arg, 1);
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
    (void) sprintf(message_dsp, "Can not open %s, taking defaults.", SCOREFILE);
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
    (void) sprintf(message_dsp, "Can not write to %s.", SCOREFILE);
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
