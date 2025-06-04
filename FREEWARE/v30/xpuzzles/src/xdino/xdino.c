/*
# X-BASED DINO
#
#  xdino.c
#
###
#
#  Copyright (c) 1995	David Albert Bagley, bagleyd@source.asset.com
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
  Version 4: 94/05/30 Xt
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
#include "Dino.h"
#include "Dino2d.h"
#include "Dino3d.h"
#include "dino.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/dino.scores"
#endif

/* The following 5 are in DinoP.h also */
#define PERIOD2 2
#define PERIOD3 3
#define BOTH 4
#define MAXMODES 3
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

static Widget dino2d, dino3d;
static Arg arg[5];
static int dino_record[MAXMODES][2], moves_dsp = 0;
static char prog_dsp[64] = "xdino";
static char record_dsp[16] = "INF";
static char message_dsp[128] = "Randomize to start";
static char title_dsp[256] = "";

static void usage()
{
  (void) fprintf(stderr, "usage: xdino\n");
  (void) fprintf(stderr, 
    "\t[-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]]\n");
  (void) fprintf(stderr, 
    "\t[-display [{host}]:[{vs}]][-fg {color}] [-bg {color}]\n");
  (void) fprintf(stderr,
    "\t[-[no]orient] [-mono] [-{mode {int} | both}]\n");
  (void) fprintf(stderr,
    "\t[-face{0|1|2|3|4|5} {color}]\n");
  exit(1);
}

static XrmOptionDescRec options[] = {
  {"-fg",		"*dino.Foreground",	XrmoptionSepArg,	NULL},
  {"-bd",		"*dino.Foreground",	XrmoptionSepArg,	NULL},
  {"-bg",		"*Background",		XrmoptionSepArg,	NULL},
  {"-foreground",	"*dino.Foreground",	XrmoptionSepArg,	NULL},
  {"-background",	"*Background",		XrmoptionSepArg,	NULL},
  {"-bordercolor",	"*dino.Foreground",	XrmoptionSepArg,	NULL},
  {"-orient",		"*dino.orient",		XrmoptionNoArg,		"TRUE"},
  {"-noorient",		"*dino.orient",		XrmoptionNoArg,		"FALSE"},
  {"-mode",		"*dino.mode",		XrmoptionSepArg,	NULL},
  {"-both",		"*dino.mode",		XrmoptionNoArg,		"4"},
  {"-mono",		"*dino.mono",		XrmoptionNoArg,		"TRUE"},
  {"-face0",		"*dino.faceColor0",	XrmoptionSepArg,	NULL},
  {"-face1",		"*dino.faceColor1",	XrmoptionSepArg,	NULL},
  {"-face2",		"*dino.faceColor2",	XrmoptionSepArg,	NULL},
  {"-face3",		"*dino.faceColor3",	XrmoptionSepArg,	NULL},
  {"-face4",		"*dino.faceColor4",	XrmoptionSepArg,	NULL},
  {"-face5",		"*dino.faceColor5",	XrmoptionSepArg,	NULL},
};

int main(argc, argv)
  int argc;
  char *argv[];
{
  Widget toplevel, shell;

  toplevel = XtInitialize(argv[0], "Dino",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  shell = XtCreateApplicationShell(argv[0], topLevelShellWidgetClass, NULL,
    0);
  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) dino_bits, dino_width, dino_height));
  XtSetValues(toplevel, arg, 1);
  XtSetArg(arg[0], XtNiconPixmap, 
    XCreateBitmapFromData(XtDisplay(shell),
      RootWindowOfScreen(XtScreen(shell)),
      (char *) dino_bits, dino_width, dino_height));
  XtSetValues(shell, arg, 1);
  dino2d = XtCreateManagedWidget("dino", dino2dWidgetClass, toplevel,
    NULL, 0);
  XtAddCallback(dino2d, XtNselectCallback, moves_text, NULL);
  dino3d = XtCreateManagedWidget("dino", dino3dWidgetClass, shell,
    NULL, 0);
  XtAddCallback(dino3d, XtNselectCallback, moves_text, NULL);
  initialize();
  XtRealizeWidget(toplevel);
  XtRealizeWidget(shell);
  XGrabButton(XtDisplay(dino2d), AnyButton, AnyModifier, XtWindow(dino2d),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(dino2d),
    XCreateFontCursor(XtDisplay(dino2d), XC_crosshair));
  XGrabButton(XtDisplay(dino3d), AnyButton, AnyModifier, XtWindow(dino3d),
    TRUE, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(dino3d),
    XCreateFontCursor(XtDisplay(dino3d), XC_crosshair));
  XtMainLoop();

#ifdef VMS
  return 1;
#else
  return 0;
#endif
}

/* There's probably a better way to assure that they are the same but
   I don't know it off hand. */
static void make_equivalent(mode, orient)
  int *mode;
  Boolean *orient;
{
  Boolean mono;
  Pixel foreground, background;
  String facecolor[MAXFACES];

  XtVaGetValues(dino2d,
    XtNmode, mode,
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
  XtVaSetValues(dino2d,
    XtNface, DINO_IGNORE,
    XtNpractice, FALSE,
    XtNstart, FALSE,
    NULL);
  XtVaSetValues(dino3d,
    XtNmode, *mode,
    XtNorient, *orient,
    XtNmono, mono,
    XtNface, DINO_IGNORE,
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
  int mode;
  Boolean orient;
 
  make_equivalent(&mode, &orient);
  read_records();
  print_record(mode, orient, record_dsp);
  print_state(XtParent(dino2d), prog_dsp, 2, mode, moves_dsp,
    record_dsp, message_dsp);
  print_state(XtParent(dino3d), prog_dsp, 3, mode, moves_dsp,
    record_dsp, message_dsp);
}

static void moves_text(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  dinoCallbackStruct *call_data;
{
  int mode, dim, otherdim;
  Boolean orient, practice;
  Widget otherw;

  if (w == dino2d) {
    dim = 2;
    otherw = dino3d;
    otherdim = 3;
  } else { /* (w == dino3d) */
    dim = 3;
    otherw = dino2d;
    otherdim = 2;
  }
  XtVaGetValues(w,
    XtNorient, &orient,
    XtNmode, &mode,
    XtNpractice, &practice,
    NULL);
  (void) strcpy(message_dsp, "");
  switch (call_data->reason) {
    case DINO_RESTORE:
      XtSetArg(arg[0], XtNface, DINO_RESTORE);
      XtSetValues(otherw, arg, 1);
      XtSetValues(w, arg, 1);
      moves_dsp = 0;
      break;
    case DINO_RESET:
      moves_dsp = 0;
      break;
    case DINO_IGNORE:
      (void) strcpy(message_dsp, "Randomize to start");
      break;
    case DINO_MOVED:
      moves_dsp++;
#ifdef DEBUG
      if (moves_dsp > 256)
        exit(1);
#endif
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetArg(arg[1], XtNface, call_data->face);
      XtSetArg(arg[2], XtNside, call_data->side);
      XtSetArg(arg[3], XtNdirection, call_data->direction);
      XtSetArg(arg[4], XtNstyle, call_data->style);
      XtSetValues(otherw, arg, 5);
      XtSetValues(w, arg, 1);
      break;
    case DINO_CONTROL:
      XtSetArg(arg[0], XtNface, call_data->face);
      XtSetArg(arg[1], XtNside, call_data->side);
      XtSetArg(arg[2], XtNdirection, call_data->direction);
      XtSetArg(arg[3], XtNstyle, call_data->style);
      XtSetValues(otherw, arg, 4);
      return;
    case DINO_SOLVED:
      if (practice)
        moves_dsp = 0;
      else { 
        if (handle_solved(moves_dsp, mode, orient))
          (void) sprintf(message_dsp, "Congratulations %s!!", getenv("USER"));
        else
          (void) strcpy(message_dsp, "Solved!");
      }
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case DINO_PRACTICE:
      moves_dsp = 0;
      practice = !practice;
      if (practice)
        (void) strcpy(record_dsp, "practice");
      else {
        (void) strcpy(message_dsp, "Randomize to start");
        print_record(mode, orient, record_dsp);
      }
      XtSetArg(arg[0], XtNpractice, practice);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      XtSetValues(otherw, arg, 2);
      break;
    case DINO_RANDOMIZE:
      moves_dsp = 0;
      XtSetArg(arg[0], XtNpractice, FALSE);
      XtSetArg(arg[1], XtNstart, FALSE);
      XtSetValues(w, arg, 2);
      XtSetValues(otherw, arg, 2);
      break; 
    case DINO_ORIENT:
      moves_dsp = 0;
      orient = !orient;
      print_record(mode, orient, record_dsp);
      XtSetArg(arg[0], XtNorient, orient);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case DINO_PERIOD2:
    case DINO_PERIOD3:
    case DINO_BOTH:
      moves_dsp = 0;
      mode = call_data->reason - DINO_PERIOD2 + PERIOD2;
      print_record(mode, orient, record_dsp);
      XtSetArg(arg[0], XtNmode, mode);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case DINO_COMPUTED:
      XtSetArg(arg[0], XtNstart, FALSE);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
      break;
    case DINO_UNDO:
      moves_dsp--;
      XtSetArg(arg[0], XtNstart, TRUE);
      XtSetArg(arg[1], XtNface, call_data->face);
      XtSetArg(arg[2], XtNside, call_data->side);
      XtSetArg(arg[3], XtNdirection, call_data->direction);
      XtSetArg(arg[4], XtNstyle, call_data->style);
      XtSetValues(otherw, arg, 5);
      XtSetValues(w, arg, 1);
      break;
  }
  print_state(XtParent(w), prog_dsp, dim, mode, moves_dsp,
    record_dsp, message_dsp);
  print_state(XtParent(otherw), prog_dsp, otherdim, mode, moves_dsp,
    record_dsp, message_dsp);
}

static void print_record(mode, orient, record)
  int mode;
  Boolean orient;
  char *record;
{
  int i = mode - PERIOD2;
  int j = (orient) ? 1 : 0;

  if (dino_record[i][j] >= MAXRECORD)
    (void) strcpy(record, "NEVER");
  else
    (void) sprintf(record, "%d", dino_record[i][j]);
}

static int handle_solved(counter, mode, orient)
  int counter, mode;
  Boolean orient;
{
  int i = mode - PERIOD2;
  int j = (orient) ? 1 : 0;

  if (counter < dino_record[i][j]) {
    dino_record[i][j] = counter;
    if (orient && (dino_record[i][j] < dino_record[!i][j]))
      dino_record[!i][j] = counter;
    write_records();
    (void) sprintf(record_dsp, "%d", counter);
    return TRUE;
  }
  return FALSE;
}

static void print_state(w, prog, dim, mode, moves, record, message)
  Widget w;
  char *prog, *record, *message;
  int mode, dim, moves;
{
  char mb[10];

  if (mode == BOTH)
    (void) strcpy(mb, "both");
  else
    (void) sprintf(mb, "%d", mode);
  (void) sprintf(title_dsp, "%s%dd.%s: (%d/%s) - %s", prog, dim, mb, moves,
    record, message);
  XtSetArg(arg[0], XtNtitle, title_dsp);
  XtSetValues(w, arg, 1);
}

static void read_records()
{
  FILE *fp;
  int n, mode, orient;

  for (mode = 0; mode < MAXMODES; mode++)
    for (orient = 0; orient < 2; orient++)
      dino_record[mode][orient] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    (void) sprintf(message_dsp, "Can not open %s, taking defaults.", SCOREFILE);
  else {
    for (mode = 0; mode < MAXMODES; mode++)
      for (orient = 0; orient < 2; orient++) {
        (void) fscanf(fp, "%d", &n);
        dino_record[mode][orient] = n;
      }
    (void) fclose(fp);
  }
}

static void write_records()
{
  FILE *fp;
  int mode, orient;

  if ((fp = fopen(SCOREFILE, "w")) == NULL)
    (void) sprintf(message_dsp, "Can not write to %s.", SCOREFILE);
  else {
    for (mode = 0; mode < MAXMODES; mode++) {
      for (orient = 0; orient < 2; orient++)
        (void) fprintf(fp, "%d ", dino_record[mode][orient]);
      (void) fprintf(fp, "\n");
    }
    (void) fclose(fp);
  }
}
