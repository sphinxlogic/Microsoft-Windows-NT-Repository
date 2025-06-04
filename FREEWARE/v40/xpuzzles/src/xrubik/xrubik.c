/*
# X-BASED RUBIK'S CUBE(tm)
#
#  xrubik.c
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
  Version 4: 94/04/07 Xt
  Version 3: 93/05/20 Motif
  Version 2: 92/01/16 XView
  Version 1: 91/01/16 SunView
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
#include "Rubik.h"
#include "Rubik2d.h"
#include "Rubik3d.h"
#include "rubik.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/rubik.scores"
#endif

/* The following 3 are in RubikP.h also */
#define MINCUBES 1
#define MAXCUBES 6
#define MAXFACES 6

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

static Widget rubik2d, rubik3d;
static Arg arg[5];
static int rubik_record[2][MAXCUBES - MINCUBES + 1], moves_dsp = 0;
static char prog_dsp[64] = "xrubik";
static char record_dsp[16] = "INF";
static char message_dsp[128] = "Randomize to start";
static char title_dsp[256] = "";

static void usage()
{
  (void) fprintf(stderr, "usage: xrubik\n");
  (void) fprintf(stderr,
    "\t[-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]]\n");
  (void) fprintf(stderr,
    "\t[-display [{host}]:[{vs}]][-fg {color}] [-bg {color}]\n");
  (void) fprintf(stderr,
    "\t[-size {int}] [-[no]orient] [-mono] [-face{0|1|2|3|4|5} {color}]\n");
  exit(1);
}

static XrmOptionDescRec options[] = {
  {"-fg",		"*rubik.Foreground",	XrmoptionSepArg,	NULL},
  {"-bd",		"*rubik.Foreground",	XrmoptionSepArg,	NULL},
  {"-bg",		"*Background",		XrmoptionSepArg,	NULL},
  {"-foreground",	"*rubik.Foreground",	XrmoptionSepArg,	NULL},
  {"-background",	"*Background",		XrmoptionSepArg,	NULL},
  {"-bordercolor",	"*rubik.Foreground",	XrmoptionSepArg,	NULL},
  {"-size",		"*rubik.size",		XrmoptionSepArg,	NULL},
  {"-orient",		"*rubik.orient",	XrmoptionNoArg,		"TRUE"},
  {"-noorient",		"*rubik.orient",	XrmoptionNoArg,		"FALSE"},
  {"-mono",		"*rubik.mono",		XrmoptionNoArg,		"TRUE"},
  {"-face0",		"*rubik.faceColor0",	XrmoptionSepArg,	NULL},
  {"-face1",		"*rubik.faceColor1",	XrmoptionSepArg,	NULL},
  {"-face2",		"*rubik.faceColor2",	XrmoptionSepArg,	NULL},
  {"-face3",		"*rubik.faceColor3",	XrmoptionSepArg,	NULL},
  {"-face4",		"*rubik.faceColor4",	XrmoptionSepArg,	NULL},
  {"-face5",		"*rubik.faceColor5",	XrmoptionSepArg,	NULL},
};

int main(argc, argv)
  int argc;
  char *argv[];
{
  Widget toplevel, shell;

  toplevel = XtInitialize(argv[0], "Rubik",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  shell = XtCreateApplicationShell(argv[0], topLevelShellWidgetClass, NULL,
    0);
  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) rubik_bits, rubik_width, rubik_height));
  XtSetValues(toplevel, arg, 1);
  XtSetArg(arg[0], XtNiconPixmap, 
    XCreateBitmapFromData(XtDisplay(shell),
      RootWindowOfScreen(XtScreen(shell)),
      (char *) rubik_bits, rubik_width, rubik_height));
  XtSetValues(shell, arg, 1);
  rubik2d = XtCreateManagedWidget("rubik", rubik2dWidgetClass, toplevel,
    NULL, 0);
  XtAddCallback(rubik2d, XtNselectCallback, moves_text, NULL);
  rubik3d = XtCreateManagedWidget("rubik", rubik3dWidgetClass, shell,
    NULL, 0);
  XtAddCallback(rubik3d, XtNselectCallback, moves_text, NULL);
  initialize();
  XtRealizeWidget(toplevel);
  XtRealizeWidget(shell);
  XGrabButton(XtDisplay(rubik2d), AnyButton, AnyModifier, XtWindow(rubik2d),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(rubik2d),
    XCreateFontCursor(XtDisplay(rubik2d), XC_crosshair));
  XGrabButton(XtDisplay(rubik3d), AnyButton, AnyModifier, XtWindow(rubik3d),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(rubik3d),
    XCreateFontCursor(XtDisplay(rubik3d), XC_crosshair));
  XtMainLoop();

#ifdef VMS
  return 1;
#else
  return 0;
#endif
}

/* There's probably a better way to assure that they are the same but
   I don't know it off hand. */
static void make_equivalent(size, orient)
  int *size;
  Boolean *orient;
{
  Boolean mono;
  Pixel foreground, background;
  String facecolor[MAXFACES];

  XtVaGetValues(rubik2d,
    XtNsize, size,
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
  XtVaSetValues(rubik2d,
    XtNface, RUBIK_IGNORE,
    XtNpractice, FALSE,
    XtNstart, FALSE,
    NULL);
  XtVaSetValues(rubik3d,
    XtNsize, *size,
    XtNorient, *orient,
    XtNmono, mono,
    XtNface, RUBIK_IGNORE,
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
  int size;
  Boolean orient;

  make_equivalent(&size, &orient);
  read_records();
  print_record(orient, size, record_dsp);
  print_state(XtParent(rubik2d), prog_dsp, 2, size, moves_dsp,
    record_dsp, message_dsp);
  print_state(XtParent(rubik3d), prog_dsp, 3, size, moves_dsp,
    record_dsp, message_dsp);
}

static void moves_text(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  rubikCallbackStruct *call_data;
{
  int size, dim, otherdim;
  Boolean orient, practice;
  Widget otherw;

  if (w == rubik2d) {
    dim = 2;
    otherw = rubik3d;
    otherdim = 3;
  } else { /* (w == rubik3d) */
    dim = 3;
    otherw = rubik2d;
    otherdim = 2;
  }
  XtVaGetValues(w,
    XtNsize, &size,
    XtNorient, &orient,
    XtNpractice, &practice,
    NULL);
  (void) strcpy(message_dsp, "");
  switch (call_data->reason) {
    case RUBIK_RESTORE:
      XtSetArg(arg[0], XtNface, RUBIK_RESTORE);
      XtSetValues(otherw, arg, 1);
      XtSetValues(w, arg, 1);
      moves_dsp = 0;
      break;
    case RUBIK_RESET:
      moves_dsp = 0;
      break;
    case RUBIK_IGNORE:
      (void) strcpy(message_dsp, "Randomize to start");
      break;
    case RUBIK_MOVED:
      moves_dsp++;
#ifdef DEBUG
      if (moves_dsp > 256)
	exit(1);
#endif
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetArg(arg[1], XtNface, call_data->face);
      XtSetArg(arg[2], XtNi, call_data->i);
      XtSetArg(arg[3], XtNj, call_data->j);
      XtSetArg(arg[4], XtNdirection, call_data->direction);
      XtSetValues(otherw, arg, 5);
      XtSetValues(w, arg, 1);
      break;
    case RUBIK_CONTROL:
      XtSetArg(arg[0], XtNface, call_data->face);
      XtSetArg(arg[1], XtNi, call_data->i);
      XtSetArg(arg[2], XtNj, call_data->j);
      XtSetArg(arg[3], XtNdirection, call_data->direction);
      XtSetValues(otherw, arg, 4);
      return;
    case RUBIK_SOLVED:
      if (practice)
        moves_dsp = 0;
      else { 
        if (handle_solved(moves_dsp, size, orient))
          (void) sprintf(message_dsp, "Congratulations %s!!", getenv("USER"));
        else
          (void) strcpy(message_dsp, "Solved!");
      }
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case RUBIK_PRACTICE:
      moves_dsp = 0;
      practice = !practice;
      if (practice)
        (void) strcpy(record_dsp, "practice");
      else {
        (void) strcpy(message_dsp, "Randomize to start");
        print_record(orient, size, record_dsp);
      }
      XtSetArg(arg[0], XtNpractice, practice);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      XtSetValues(otherw, arg, 2);
      break;
    case RUBIK_RANDOMIZE:
      moves_dsp = 0;
      XtSetArg(arg[0], XtNpractice, FALSE);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      XtSetValues(otherw, arg, 2);
      break; 
    case RUBIK_DEC:
      moves_dsp = 0;
      size--;
      print_record(orient, size, record_dsp);
      XtSetArg(arg[0], XtNsize, size);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case RUBIK_ORIENT:
      moves_dsp = 0;
      orient = !orient;
      print_record(orient, size, record_dsp);
      XtSetArg(arg[0], XtNorient, orient);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case RUBIK_INC:
      moves_dsp = 0;
      size++;
      print_record(orient, size, record_dsp);
      XtSetArg(arg[0], XtNsize, size);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case RUBIK_COMPUTED:
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case RUBIK_UNDO:
      moves_dsp--;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetArg(arg[1], XtNface, call_data->face);
      XtSetArg(arg[2], XtNi, call_data->i);
      XtSetArg(arg[3], XtNj, call_data->j);
      XtSetArg(arg[4], XtNdirection, call_data->direction);
      XtSetValues(otherw, arg, 5);
      XtSetValues(w, arg, 1);
      break;
  }
  print_state(XtParent(w), prog_dsp, dim, size, moves_dsp,
    record_dsp, message_dsp);
  print_state(XtParent(otherw), prog_dsp, otherdim, size, moves_dsp,
    record_dsp, message_dsp);
}

static void print_record(orient, size, record)
  Boolean orient;
  int size;
  char *record;
{
  int i = (orient) ? 1 : 0;
  int j = size - MINCUBES;

  if (rubik_record[i][j] >= MAXRECORD)
    (void) strcpy(record, "NEVER");
  else
    (void) sprintf(record, "%d", rubik_record[i][j]);
}

static int handle_solved(counter, size, orient)
  int counter, size;
  Boolean orient;
{
  int i = (orient) ? 1 : 0;
  int j = size - MINCUBES;

  if (counter < rubik_record[i][j]) {
    rubik_record[i][j] = counter;
    if (size < 2 || (orient && (rubik_record[i][j] < rubik_record[!i][j])))
      rubik_record[!i][j] = counter;
    write_records();
    (void) sprintf(record_dsp, "%d", counter);
    return TRUE;
  }
  return FALSE;
}

static void print_state(w, prog, dim, size, moves, record, message)
  Widget w;
  char *prog, *record, *message;
  int dim, size, moves;
{
  (void) sprintf(title_dsp, "%s%dd: %d@ (%d/%s) - %s", prog, dim, size, moves,
           record, message);
  XtSetArg(arg[0], XtNtitle, title_dsp);
  XtSetValues(w, arg, 1);
}

static void read_records()
{
  FILE *fp;
  int i, n, orient;

  for (orient = 0; orient < 2; orient++)
    for (i = 0; i < MAXCUBES - MINCUBES + 1; i++)
      rubik_record[orient][i] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    (void) sprintf(message_dsp,
             "Can not open %s, taking defaults.", SCOREFILE);
  else {
    for (orient = 0; orient < 2; orient++)
      for (i = 0; i < MAXCUBES - MINCUBES + 1; i++) {
        (void) fscanf(fp, "%d", &n);
        rubik_record[orient][i] = n;
      }
    (void) fclose(fp);
  }
}

static void write_records()
{
  FILE *fp;
  int i, orient;

  if ((fp = fopen(SCOREFILE, "w")) == NULL)
    (void) sprintf(message_dsp, "Can not write to %s.", SCOREFILE);
  else {
    for (orient = 0; orient < 2; orient++) {
      for (i = 0; i < MAXCUBES - MINCUBES + 1; i++)
        (void) fprintf(fp, "%d ", rubik_record[orient][i]);
      (void) fprintf(fp, "\n");
    }
    (void) fclose(fp);
  }
}
