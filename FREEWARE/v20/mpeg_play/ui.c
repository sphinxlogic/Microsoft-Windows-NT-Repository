/* Copyright (c) 1994 The Geometry Center; University of Minnesota
   1300 South Second Street;  Minneapolis, MN  55454, USA;

This file is free software; you can redistribute it and/or modify it only
under the the terms of the GNU GENERAL PUBLIC LICENSE which should be
included along with this file.  This software may be obtained via anonymous
ftp from geom.umn.edu; email: software@geom.umn.edu. */

/* Author: Daeron Meyer */

#include "mibload.h"
#include "interface/rewind.xbm"
#include "interface/play.xbm"
#include "interface/step.xbm"
#include "interface/stop.xbm"
#include "interface/loop.xbm"
#include "interface/MainWindow.mib"
#include "interface/Info.mib"
#include "video.h"

/* Declarataion of global display pointer. */
extern Display *display;

extern int loopFlag;
extern int argc_share;
extern char **argv_share;
extern char infomessage[];
extern Colormap cmap;
extern int totNumFrames;


/*****************************************************************************

/* Private Variables and Methods */

/* Intrinsics Application Context */
static  XtAppContext		App;
static  Window			monitorwindow = NULL;
static  mib_Widget		*mainload, *infoload;
static  Widget			shell = NULL, infoshell = NULL;
static  Widget			frametext = NULL;
static  int			frameshown = 0;
static  VidStream *theStream    = NULL;
static  XtWorkProcId dovid      = NULL;
static  int rewound		= 0;
static  int at_end		= 0;
static  Window displaywin       = NULL;
static  int minx		= 400;
static  int miny		= 300;
static  int winx		= 0;
static  int winy		= 0;
static  int sizelock		= 0;
static  int framelock		= 0;
static  int privcolormap	= 0;
static  int currentframe	= 0;

static void exit_callback(Widget, XtPointer, XmAnyCallbackStruct *);
static void stop_callback(Widget, XtPointer, XmAnyCallbackStruct *);
static void step_callback(Widget, XtPointer, XmAnyCallbackStruct *);
static void play_callback(Widget, XtPointer, XmAnyCallbackStruct *);
static void loop_callback(Widget, XtPointer, XmAnyCallbackStruct *);
static void rewind_callback(Widget, XtPointer, XmAnyCallbackStruct *);
static void info_callback(Widget, XtPointer, XmAnyCallbackStruct *);
static void close_callback(Widget, XtPointer, XmAnyCallbackStruct *);
static void monitor_expose(Widget, XtPointer, XmDrawingAreaCallbackStruct *);
static Boolean do_video(XtPointer);

/*****************************************************************************

  UICreate: Create/load in a motif interface.

 *****************************************************************************/

void UICreate()
{
  Widget		toplevel,
			mainwindow,
			temp;

  Pixel			fg, bg;
  Pixmap		button_pix;

  Arg			args[20];
  int			n, depth;

  static String		fallbacks[] = {
  "*Foreground:  gray20",
  "*BorderWidth: 0",
  "*Background:  gray70",
  "*XmToggleButton.selectColor:         yellow",
  "*XmToggleButton.indicatorSize:       16",
  "*XmToggleButtonGadget.selectColor:   yellow",
  "*XmToggleButtonGadget.indicatorSize: 16",
  "*fontList: -adobe-helvetica-medium-r-normal--14-*-*-*-p-76-iso8859-1",
  "*XmText*fontList: -adobe-courier-medium-r-normal--12-*-*-*-m-70-iso8859-1",
  NULL
  };

/*****************************************************************************/

  n = 0;

  toplevel = XtAppInitialize(&App, "mpeg_play", NULL, 0, &argc_share,
				argv_share, fallbacks, args, n);

  display = XtDisplay(toplevel);

/*****************************************************************************/

  n = 0;
  XtSetArg(args[n], XmNtitle, "mpeg player"); n++;
  XtSetArg(args[n], XmNdeleteResponse, XmDO_NOTHING);n++;
  XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
  XtSetArg(args[n], XmNwidth, minx); n++;
  XtSetArg(args[n], XmNminWidth, minx); n++;
  XtSetArg(args[n], XmNheight, miny); n++;
  XtSetArg(args[n], XmNminHeight, miny); n++;

  shell = XtAppCreateShell("mpeg player", "mpeg_play",
				topLevelShellWidgetClass, display, args, n);

/*****************************************************************************/

  n = 0;
  XtSetArg(args[n], XmNtitle, "mpeg player"); n++;

  mainwindow = XmCreateForm(shell, "mpeg player", args, n);
  mainload = mib_load_interface(mainwindow, MainMib,
				MI_FROMSTRING);

  XtManageChild(mainwindow);
  XtSetMappedWhenManaged(shell, False);
  XtRealizeWidget(shell);

  frametext = mib_find_name(mainload, "FrameText")->me;
  XtVaSetValues(frametext, XmNcursorPositionVisible, False,
			   XmNeditable, False, NULL);

  temp = mib_find_name(mainload, "Monitor")->me;
  XtAddCallback(temp, XmNexposeCallback,
			(XtCallbackProc) monitor_expose, (XtPointer) NULL);
  XtVaSetValues(temp,   XmNforeground,
			BlackPixel(display, DefaultScreen(display)),
			XmNbackground,
			BlackPixel(display, DefaultScreen(display)),
                        NULL);
  monitorwindow = XtWindow(temp);

  temp = mib_find_name(mainload, "Stop")->me;
  XtVaGetValues(temp, XmNforeground, &fg,
                        XmNbackground, &bg, XmNdepth, &depth, NULL);
  button_pix = XCreatePixmapFromBitmapData(display, DefaultRootWindow(display),
                stop_bits, stop_width, stop_height, fg, bg, depth);
  XtVaSetValues(temp, XmNlabelType, XmPIXMAP,
                        XmNlabelPixmap, button_pix, NULL);
  XtAddCallback(temp, XmNactivateCallback, (XtCallbackProc) stop_callback,
			(XtPointer) NULL);

  temp = mib_find_name(mainload, "Step")->me;
  XtVaGetValues(temp, XmNforeground, &fg,
                        XmNbackground, &bg, XmNdepth, &depth, NULL);
  button_pix = XCreatePixmapFromBitmapData(display, DefaultRootWindow(display),
                step_bits, step_width, step_height, fg, bg, depth);
  XtVaSetValues(temp, XmNlabelType, XmPIXMAP,
                        XmNlabelPixmap, button_pix, NULL);
  XtAddCallback(temp, XmNactivateCallback, (XtCallbackProc) step_callback,
			(XtPointer) NULL);

  temp = mib_find_name(mainload, "Play")->me;
  XtVaGetValues(temp, XmNforeground, &fg,
                        XmNbackground, &bg, XmNdepth, &depth, NULL);
  button_pix = XCreatePixmapFromBitmapData(display, DefaultRootWindow(display),
                play_bits, play_width, play_height, fg, bg, depth);
  XtVaSetValues(temp, XmNlabelType, XmPIXMAP,
                        XmNlabelPixmap, button_pix, NULL);
  XtAddCallback(temp, XmNactivateCallback, (XtCallbackProc) play_callback,
			(XtPointer) NULL);


  temp = mib_find_name(mainload, "Loop")->me;
  XtVaGetValues(temp, XmNforeground, &fg,
                        XmNbackground, &bg, XmNdepth, &depth, NULL);
  button_pix = XCreatePixmapFromBitmapData(display, DefaultRootWindow(display),
                loop_bits, loop_width, loop_height, fg, bg, depth);
  XtVaSetValues(temp, XmNlabelType, XmPIXMAP,
                        XmNlabelPixmap, button_pix, NULL);
  XtAddCallback(temp, XmNactivateCallback, (XtCallbackProc) loop_callback,
			(XtPointer) NULL);


  temp = mib_find_name(mainload, "Info")->me;
  XtAddCallback(temp, XmNactivateCallback, (XtCallbackProc) info_callback,
			(XtPointer) NULL);

  temp = mib_find_name(mainload, "Rewind")->me;
  XtVaGetValues(temp, XmNforeground, &fg,
                        XmNbackground, &bg, XmNdepth, &depth, NULL);
  button_pix = XCreatePixmapFromBitmapData(display, DefaultRootWindow(display),
                rewind_bits, rewind_width, rewind_height, fg, bg, depth);
  XtVaSetValues(temp, XmNlabelType, XmPIXMAP,
                        XmNlabelPixmap, button_pix, NULL);
  XtAddCallback(temp, XmNactivateCallback, (XtCallbackProc) rewind_callback,
			(XtPointer) NULL);


  temp = mib_find_name(mainload, "Exit")->me;
  XtAddCallback(temp, XmNactivateCallback, (XtCallbackProc) exit_callback,
			(XtPointer) NULL);

/*****************************************************************************/

  n = 0;
  XtSetArg(args[n], XmNtitle, "Info"); n++;
  XtSetArg(args[n], XmNdeleteResponse, XmDO_NOTHING);n++;
  XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
  XtSetArg(args[n], XmNwidth, minx+200); n++;
  XtSetArg(args[n], XmNminWidth, minx); n++;
  XtSetArg(args[n], XmNheight, miny+100); n++;
  XtSetArg(args[n], XmNminHeight, miny); n++;

  infoshell = XtAppCreateShell("Info", "mpeg_play",
				topLevelShellWidgetClass, display, args, n);

/*****************************************************************************/

  n = 0;
  XtSetArg(args[n], XmNtitle, "Info"); n++;

  temp  = XmCreateForm(infoshell, "Info", args, n);
  infoload = mib_load_interface(temp, InfoMib,
                                MI_FROMSTRING);
  XtManageChild(temp);

  temp = mib_find_name(infoload, "Close")->me;
  XtAddCallback(temp, XmNactivateCallback, (XtCallbackProc) close_callback,
			(XtPointer) NULL);

  temp = mib_find_name(infoload, "TextBig")->me;
  XtVaSetValues(temp, XmNcursorPositionVisible, False,
			   XmNeditable, False, NULL);
  XmTextSetString(temp, infomessage);

}

/*****************************************************************************/

void UIMainLoop(VidStream *mystream)
{
  int frame;
  theStream = mystream;

  if (loopFlag && !rewound)
    dovid = XtAppAddWorkProc(App, do_video, NULL);

  at_end = 0;

  while ((totNumFrames == 0) && (!at_end))
  {
    mpegVidRsrc(0, theStream);
  }

  frameshown = 1;
  rewound = 0;
  at_end = 0;

  XtAppMainLoop(App);
}

/*****************************************************************************/

Window MonitorWindow()
{
  return monitorwindow;
}

/*****************************************************************************/

void UISetColormap()
{
  privcolormap = 1;
}

/*****************************************************************************/

void UISetwin(Window win)
{
  displaywin = win;
}

/*****************************************************************************/

void UISetFrame()
{
  char framestr[100];

  if (at_end)
  {
    XmTextFieldSetString(frametext, "end");
    return;
  }

  if (!framelock)
  {
    totNumFrames++;

    currentframe = totNumFrames;

    sprintf(framestr, "%d", totNumFrames);
    XmTextFieldSetString(frametext, framestr);
  }
}

/*****************************************************************************/

void UIMinsize(int x, int y)
{
  winx = x;
  winy = y;

  if (sizelock)
    return;

  x += 34;
  y += 67;

  if (x < minx)
    x = minx;
  if (y < miny)
    y = miny;

  minx = x; miny = y;

  if (privcolormap)
  {
    XtVaSetValues(shell, XmNcolormap, cmap, NULL);
  }
  if (shell)
  {
    XtResizeWidget(shell, (Dimension)minx, (Dimension)miny, (Dimension)0);
    XtVaSetValues(shell, XmNminWidth, 400,
			 XmNmaxWidth, minx,
			 XmNminHeight, 300,
			 XmNmaxHeight, miny, NULL);

    XtMapWidget(shell); /* Map the shell now to avoid disappearing buttons! */
    sizelock = 1;
    if (displaywin)
    {
      x = winx; y = winy;
      winx = minx - 34; winy = miny - 67;
      XMoveWindow(display, displaywin, (winx - x)/2, (winy - y)/2);
    }
  }
}

/*****************************************************************************/

void UIEnd()
{

  if (dovid)
    XtRemoveWorkProc(dovid);

  XmTextFieldSetString(frametext, "end");
  dovid = NULL;
  at_end = 1;

  if (loopFlag)
  {
    at_end = 0;
    rewound = 0;
    longjmp(env, 1);
  }
}

/*****************************************************************************

  From here on we have private methods:

 *****************************************************************************/

static void exit_callback(Widget w, XtPointer data, XmAnyCallbackStruct *cbs)
{
  exit(0);
}

/*****************************************************************************/

static void stop_callback(Widget w, XtPointer data, XmAnyCallbackStruct *cbs)
{
  if (at_end)
    return;

  if (dovid)
    XtRemoveWorkProc(dovid);
  dovid = NULL;

  loopFlag = 0;
}

/*****************************************************************************/

static void play_callback(Widget w, XtPointer data, XmAnyCallbackStruct *cbs)
{
  if (at_end)
  {
    rewound = 1;
    loopFlag = 0;
    longjmp(env, 1);
  }

  if (!dovid)
    dovid = XtAppAddWorkProc(App, do_video, NULL);

  loopFlag = 0;
}

/*****************************************************************************/

static void step_callback(Widget w, XtPointer data, XmAnyCallbackStruct *cbs)
{
  int aframe;

  loopFlag = 0;

  if (at_end)
  {
    rewound = 1;
    longjmp(env, 1);
  }

  if (dovid)
    XtRemoveWorkProc(dovid);

  dovid = NULL;

  aframe = currentframe;

  while ((aframe == currentframe) && (!at_end))
    mpegVidRsrc(0, theStream);

}

/*****************************************************************************/

static void loop_callback(Widget w, XtPointer data, XmAnyCallbackStruct *cbs)
{
  if (at_end)
  {
    loopFlag = 1;
    rewound = 0;
    longjmp(env, 1);
  }

  if (!dovid)
    dovid = XtAppAddWorkProc(App, do_video, NULL);

  loopFlag = 1;
}

/*****************************************************************************/

static void info_callback(Widget w, XtPointer data, XmAnyCallbackStruct *cbs)
{
  XtRealizeWidget(infoshell);
}

static void close_callback(Widget w, XtPointer data, XmAnyCallbackStruct *cbs)
{
  XtUnrealizeWidget(infoshell);
}

/*****************************************************************************/

static void rewind_callback(Widget w, XtPointer data, XmAnyCallbackStruct *cbs)
{
  if (at_end)
  {
    rewound = 1;
    longjmp(env, 1);
  }
  UIEnd();
  at_end = 0;
  rewound = 0;
  longjmp(env, 1);

}

/*****************************************************************************/

static Boolean do_video( XtPointer data)
{
  mpegVidRsrc(0, theStream);
  if (at_end)
    return True;
  else
    return False;
}

/*****************************************************************************/

static void monitor_expose(Widget w, XtPointer data,
				XmDrawingAreaCallbackStruct *cbs)
{
  Window		t_root;
  int			toss;
  unsigned int		toss2, rx, ry;
  int			nx, ny;

  framelock = 1;
  if (theStream && !dovid && frameshown)
  {
    ExecuteDisplay(theStream);
  }
  framelock = 0;
}

/*****************************************************************************/
