/*
# X-BASED TRIANGLES
#
#  xtriangles.c
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
#include "Triangles.h"
#include "triangles.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/triangles.scores"
#endif
 
/* The following 2 are in TrianglesP.h also */
#define MINTRIANGLES 1
#define MAXTRIANGLES 16

#define SAVEDMAXTRIANGLES MAXTRIANGLES
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
static int triangles_record[MAXTRIANGLES - MINTRIANGLES + 1], moves_dsp = 0;
static char prog_dsp[64] = "xtriangles";
static char record_dsp[16] = "INF";
static char message_dsp[128] = "Randomize to start";
static char title_dsp[256] = "";

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
  Widget toplevel, triangles; 

  toplevel = XtInitialize(argv[0], "Triangles",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) triangles_bits, triangles_width, triangles_height));
  XtSetValues(toplevel, arg, 1);
  triangles = XtCreateManagedWidget("triangles", trianglesWidgetClass, toplevel,
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
  read_records();
  print_record(size, record_dsp);
  print_state(XtParent(w), prog_dsp, size, moves_dsp,
    record_dsp, message_dsp);
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
        (void) sprintf(message_dsp, "Congratulations %s!!", getenv("USER"));
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
      print_record(size, record_dsp);
      XtSetArg(arg[0], XtNsize, size);
      XtSetValues(w, arg, 1);
      break;
    case TRIANGLES_INC:
      moves_dsp = 0;
      size++;
      print_record(size, record_dsp);
      XtSetArg(arg[0], XtNsize, size);
      XtSetValues(w, arg, 1);
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
  print_state(XtParent(w), prog_dsp, size, moves_dsp,
    record_dsp, message_dsp);
}

static void print_record(size, record)
  int size;
  char *record;
{
  if (triangles_record[size - MINTRIANGLES] >= MAXRECORD)
    (void) strcpy(record, "NEVER");
  else
    (void) sprintf(record, "%d", triangles_record[size - MINTRIANGLES]);
}

static int handle_solved(counter, size)
  int counter, size;
{
  if (counter < triangles_record[size - MINTRIANGLES]) {
    triangles_record[size - MINTRIANGLES] = counter;
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
  int i, n;

  for (i = 0; i < MAXTRIANGLES - MINTRIANGLES + 1; i++)
    triangles_record[i] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    (void) sprintf(message_dsp, "Can not open %s, taking defaults.", SCOREFILE);
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
    (void) sprintf(message_dsp, "Can not write to %s.", SCOREFILE);
  else {
    for (i = 0; i < SAVEDMAXTRIANGLES - MINTRIANGLES + 1; i++)
      (void) fprintf(fp, "%d ", triangles_record[i]);
    (void) fprintf(fp, "\n");
    (void) fclose(fp);
  }
}
