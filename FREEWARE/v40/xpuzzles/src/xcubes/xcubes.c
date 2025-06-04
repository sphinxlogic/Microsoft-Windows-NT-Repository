/*
# X-BASED CUBES
#
#  xcubes.c
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
#include "Cubes.h"
#include "cubes.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/cubes.scores"
#endif
 
/* The following are in CubesP.h also */
#define MINCUBES 1
#define MAXCUBES 8

#define SAVEDMAXCUBES MAXCUBES
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
static int cubes_record[MAXCUBES - MINCUBES + 1][MAXCUBES - MINCUBES + 1]
  [MAXCUBES - MINCUBES + 1], moves_dsp = 0;
static char prog_dsp[64] = "xcubes";
static char record_dsp[16] = "INF";
static char message_dsp[128] = "Randomize to start";
static char title_dsp[256] = "";

static void usage()
{
  (void) fprintf(stderr, "usage: xcubes\n");
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
  Widget toplevel, cubes; 

  toplevel = XtInitialize(argv[0], "Cubes",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) cubes_bits, cubes_width, cubes_height));
  XtSetValues(toplevel, arg, 1);
  cubes = XtCreateManagedWidget("cubes", cubesWidgetClass, toplevel,
    NULL, 0);
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
  read_records();
  print_record(size_x, size_y, size_z, record_dsp);
  print_state(XtParent(w), prog_dsp, size_x, size_y, size_z, moves_dsp,
    record_dsp, message_dsp);
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
        (void) sprintf(message_dsp, "Congratulations %s!!", getenv("USER"));
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
      print_record(size_x, size_y, size_z, record_dsp);
      XtSetArg(arg[0], XtNsizeX, size_x);
      XtSetValues(w, arg, 1);
      break;
    case CUBES_INC_X:
      moves_dsp = 0;
      size_x++;
      print_record(size_x, size_y, size_z, record_dsp);
      XtSetArg(arg[0], XtNsizeX, size_x);
      XtSetValues(w, arg, 1);
      break;
    case CUBES_DEC_Y:
      moves_dsp = 0;
      size_y--;
      print_record(size_x, size_y, size_z, record_dsp);
      XtSetArg(arg[0], XtNsizeY, size_y);
      XtSetValues(w, arg, 1);
      break;
    case CUBES_INC_Y:
      moves_dsp = 0;
      size_y++;
      print_record(size_x, size_y, size_z, record_dsp);
      XtSetArg(arg[0], XtNsizeY, size_y);
      XtSetValues(w, arg, 1);
      break;
    case CUBES_DEC_Z:
      moves_dsp = 0;
      size_z--;
      print_record(size_x, size_y, size_z, record_dsp);
      XtSetArg(arg[0], XtNsizeZ, size_z);
      XtSetValues(w, arg, 1);
      break;
    case CUBES_INC_Z:
      moves_dsp = 0;
      size_z++;
      print_record(size_x, size_y, size_z, record_dsp);
      XtSetArg(arg[0], XtNsizeZ, size_z);
      XtSetValues(w, arg, 1);
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
  print_state(XtParent(w), prog_dsp, size_x, size_y, size_z, moves_dsp,
    record_dsp, message_dsp);
}

static void print_record(size_x, size_y, size_z, record)
  int size_x, size_y, size_z;
  char *record;
{
  int i = size_x - MINCUBES, j = size_y - MINCUBES, k = size_z - MINCUBES;

  if (cubes_record[i][j][k] >= MAXRECORD)
    (void) strcpy(record, "NEVER");
  else
    (void) sprintf(record, "%d", cubes_record[i][j][k]);
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
    (void) sprintf(record_dsp, "%d", counter);
    return TRUE;
  }
  return FALSE;
}

static void print_state(w, prog, size_x, size_y, size_z, moves, record, message)
  Widget w;
  char *prog, *record, *message;
  int size_x, size_y, size_z, moves;
{
  (void) sprintf(title_dsp, "%s: %dx%dx%d@ (%d/%s) - %s",
           prog, size_x, size_y, size_z, moves, record, message);
  XtSetArg(arg[0], XtNtitle, title_dsp);
  XtSetValues(w, arg, 1);
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
    (void) sprintf(message_dsp, "Can not open %s, taking defaults.", SCOREFILE);
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
    (void) sprintf(message_dsp, "Can not write to %s.", SCOREFILE);
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
