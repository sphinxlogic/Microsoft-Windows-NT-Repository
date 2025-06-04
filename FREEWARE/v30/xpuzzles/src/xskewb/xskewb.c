/*
# X-BASED SKEWB
#
#  xskewb.c
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
  Version 4: 94/05/30 Xt
  Version 3: 93/10/14 Motif
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
#include "Skewb.h"
#include "Skewb2d.h"
#include "Skewb3d.h"
#include "skewb.xbm"

#ifndef SCOREFILE
#define SCOREFILE "/usr/games/lib/skewb.scores"
#endif

/* The following is in SkewbP.h also */
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

static Widget skewb2d, skewb3d;
static Arg arg[4];
static int skewb_record[2], moves_dsp = 0;
static char prog_dsp[64] = "xskewb";
static char record_dsp[16] = "INF";
static char message_dsp[128] = "Randomize to start";
static char title_dsp[256] = "";

static void usage()
{
  (void) fprintf(stderr, "usage: xskewb\n");
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
  Widget toplevel, shell;

  toplevel = XtInitialize(argv[0], "Skewb",
    options, XtNumber(options), &argc, argv);
  if (argc != 1)
    usage();

  shell = XtCreateApplicationShell(argv[0], topLevelShellWidgetClass, NULL,
    0);
  XtSetArg(arg[0], XtNiconPixmap,
    XCreateBitmapFromData(XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *) skewb_bits, skewb_width, skewb_height));
  XtSetValues(toplevel, arg, 1);
  XtSetArg(arg[0], XtNiconPixmap, 
    XCreateBitmapFromData(XtDisplay(shell),
      RootWindowOfScreen(XtScreen(shell)),
      (char *) skewb_bits, skewb_width, skewb_height));
  XtSetValues(shell, arg, 1);
  skewb2d = XtCreateManagedWidget("skewb", skewb2dWidgetClass, toplevel,
    NULL, 0);
  XtAddCallback(skewb2d, XtNselectCallback, moves_text, NULL);
  skewb3d = XtCreateManagedWidget("skewb", skewb3dWidgetClass, shell,
    NULL, 0);
  XtAddCallback(skewb3d, XtNselectCallback, moves_text, NULL);
  initialize();
  XtRealizeWidget(toplevel);
  XtRealizeWidget(shell);
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
  read_records();
  print_record(orient, record_dsp);
  print_state(XtParent(skewb2d), prog_dsp, 2, moves_dsp,
    record_dsp, message_dsp);
  print_state(XtParent(skewb3d), prog_dsp, 3, moves_dsp,
    record_dsp, message_dsp);
}

static void moves_text(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  skewbCallbackStruct *call_data;
{
  int dim, otherdim;
  Boolean orient, practice;
  Widget otherw;

  if (w == skewb2d) {
    dim = 2;
    otherw = skewb3d;
    otherdim = 3;
  } else { /* (w == skewb3d) */
    dim = 3;
    otherw = skewb2d;
    otherdim = 2;
  }
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
#ifdef DEBUG
      if (moves_dsp > 256)
        exit(1);
#endif
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
          (void) sprintf(message_dsp, "Congratulations %s!!", getenv("USER"));
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
        (void) strcpy(record_dsp, "practice");
      else {
        (void) strcpy(message_dsp, "Randomize to start");
        print_record(orient, record_dsp);
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
      print_record(orient, record_dsp);
      XtSetArg(arg[0], XtNorient, orient);
      XtSetValues(w, arg, 1);
      XtSetValues(otherw, arg, 1);
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
  print_state(XtParent(w), prog_dsp, dim, moves_dsp,
    record_dsp, message_dsp);
  print_state(XtParent(otherw), prog_dsp, otherdim, moves_dsp,
    record_dsp, message_dsp);
}

static void print_record(orient, record)
  Boolean orient;
  char *record;
{
  int i = (orient) ? 1 : 0;

  if (skewb_record[i] >= MAXRECORD)
    (void) strcpy(record, "NEVER");
  else
    (void) sprintf(record, "%d", skewb_record[i]);
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
    (void) sprintf(record_dsp, "%d", counter);
    return TRUE;
  }
  return FALSE;
}

static void print_state(w, prog, dim, moves, record, message)
  Widget w;
  char *prog, *record, *message;
  int dim, moves;
{
  (void) sprintf(title_dsp, "%s%dd: (%d/%s) - %s", prog, dim, moves,
    record, message);
  XtSetArg(arg[0], XtNtitle, title_dsp);
  XtSetValues(w, arg, 1);
}

static void read_records()
{
  FILE *fp;
  int n, orient;

  for (orient = 0; orient < 2; orient++)
    skewb_record[orient] = MAXRECORD;
  if ((fp = fopen(SCOREFILE, "r")) == NULL)
    (void) sprintf(message_dsp, "Can not open %s, taking defaults.", SCOREFILE);
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
    (void) sprintf(message_dsp, "Can not write to %s.", SCOREFILE);
  else {
    for (orient = 0; orient < 2; orient++)
      (void) fprintf(fp, "%d ", skewb_record[orient]);
    (void) fprintf(fp, "\n");
    (void) fclose(fp);
  }
}
