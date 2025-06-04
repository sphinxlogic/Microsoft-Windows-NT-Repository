/*
 * File:        wx_canvs.cc
 * Purpose:     wxCanvas implementation
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_canvs.cc,v 1.3 1994/08/14 21:28:43 edz Exp $
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

// Chris Breeze 27/07/97: wxUpdateIterator coords
// The Windows version and versions before wx1.66 set client coordinates
// in wxUpdateIterator. Since only the X version of wx1.66 used logical
// coordinates it was decided to revert to client coordinates for wx1.67
// for consistency across platforms. If this is a problem uncomment the
// #define below to use wx1.66-style logical coordinates:
//#define UPDATE_ITERATOR_LOGICAL_COORDS

static const char sccsid[] = "@(#)wx_canvs.cc	1.2 5/9/94";

#ifdef __GNUG__
#pragma implementation
#endif

#include <math.h>
#include <stdlib.h>
#include "wx_defs.h"
#include "wx_frame.h"
#include "wx_dccan.h"
#include "wx_dcpan.h"
#include "wx_canvs.h"
#include "wx_panel.h"
#include "wx_stdev.h"
#include "wx_utils.h"
#include "wx_privt.h"

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#include <X11/Xutil.h>
#include <X11/keysym.h>

#ifdef wx_motif
#include "wx_main.h"

#include <Xm/DrawingA.h>
#include <Xm/ScrolledW.h>
#include <Xm/ScrollBar.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>

#define SCROLL_MARGIN 4
void wxCanvasRepaintProc (Widget, XtPointer, XmDrawingAreaCallbackStruct * cbs);
void wxCanvasInputEvent (Widget drawingArea, XtPointer data, XmDrawingAreaCallbackStruct * cbs);
void wxCanvasMotionEvent (Widget, XButtonEvent * event);
void wxCanvasEnterLeave (Widget drawingArea, XtPointer clientData, XCrossingEvent * event);

#define event_left_is_down(x) ((x)->xbutton.state & Button1Mask)
#define event_middle_is_down(x) ((x)->xbutton.state & Button2Mask)
#define event_right_is_down(x) ((x)->xbutton.state & Button3Mask)

#endif

#ifdef wx_xview
#include <xview/screen.h>
#include <xview/frame.h>
#include <xview/dragdrop.h>
#include <xview/cms.h>
extern Xv_Server xview_server;
void wxCanvasRepaintProc (Canvas canvas, Xv_Window paint_window,
			  Display * dpy, Window xwin, Xv_xrectlist * xrects);
void wxCanvasResizeProc (Canvas canvas, int width, int height);
void wxCanvasEventProc (Xv_Window window, Event * event, Notify_arg arg);
Notify_value wxCanvasScrollProc(Notify_client client, Event *event, Scrollbar sbar, Notify_event_type typ);
#endif

//#define NSTATIC_RECTS	20
//static wxRectangle wxRectArray[NSTATIC_RECTS];

extern Colormap wxMainColormap;

wxCanvas::wxCanvas (void)
{
  is_retained = FALSE;
  horiz_units = 0;
  vert_units = 0;
  wx_dc = NULL;
  units_per_page_x = 0;
  units_per_page_y = 0;
  range_x = 0;
  range_y = 0;
  scrolls_set_size = TRUE;
  updateRects.DeleteContents(TRUE);
#ifdef wx_motif
  requiresBackingStore = FALSE;
  hScroll = FALSE;
  vScroll = FALSE;
  hScrollBar = NULL;
  vScrollBar = NULL;
  allowRepainting = TRUE;
  hScrollingEnabled = TRUE;
  vScrollingEnabled = TRUE;
  backingPixmap = 0;
  pixmapWidth = 0;
  pixmapHeight = 0;
  hExtent = 0;
  vExtent = 0;
  pixmapOffsetX = 0;
  pixmapOffsetY = 0;
  scrolledWindow = 0;
  borderWidget = 0;
  handle = NULL;
  display = NULL;
  xwindow = 0;
  lastTS = 0;
  lastButton = 0;
  dclickTime = XtGetMultiClickTime(wxGetDisplay()) ;
#endif
#ifdef wx_xview
  DRAG_MAX = 0;
  handle = NULL;
  horiz_scroll = 0;
  vert_scroll = 0;
  xwindow = (Window) 0;
  display = NULL;
  drag_count = DRAG_MAX;
//  xrects = NULL;
  selectionRequestor = 0;
  xviewClient = 0;
  xviewEvent = NULL;
  xviewEventType = (Notify_event_type)0;
#endif
}

wxCanvas::wxCanvas (wxWindow * parent, int x, int y, int width, int height,
	  long style, Constdata char *name):
wxbCanvas (parent, x, y, width, height, style, name)
{
  Create (parent, x, y, width, height, style, name);
}

Bool wxCanvas::
Create (wxWindow * parent, int x, int y, int width, int height,
	long style, Constdata char *name)
{
  requiresRetention = ((style & wxRETAINED) == wxRETAINED);
  windowStyle = style;
  is_retained = FALSE;		// Can only be retained after scrollbars have been set
  scrolls_set_size = TRUE;

  units_per_page_x = 0;
  units_per_page_y = 0;
  range_x = 0;
  range_y = 0;
  SetName(name);
  updateRects.DeleteContents(TRUE);
#ifdef wx_motif

  lastTS = 0;
  lastButton = 0;
  dclickTime = XtGetMultiClickTime(wxGetDisplay()) ;
  requiresBackingStore = ((style & wxBACKINGSTORE) == wxBACKINGSTORE);
  hScroll = FALSE;
  vScroll = FALSE;
  hScrollBar = NULL;
  vScrollBar = NULL;
  allowRepainting = TRUE;
  borderWidget = 0;
  hScrollingEnabled = TRUE;
  vScrollingEnabled = TRUE;
  backingPixmap = 0;
  pixmapWidth = 0;
  pixmapHeight = 0;
  hExtent = 0;
  vExtent = 0;
  button1Pressed = FALSE;
  button2Pressed = FALSE;
  button3Pressed = FALSE;
  pixmapOffsetX = 0;
  pixmapOffsetY = 0;

  // New translations for getting mouse motion feedback
  String translations =
  "<Btn1Motion>: wxCanvasMotionEvent() DrawingAreaInput() ManagerGadgetButtonMotion()\n\
     <Btn2Motion>: wxCanvasMotionEvent() DrawingAreaInput() ManagerGadgetButtonMotion()\n\
     <Btn3Motion>: wxCanvasMotionEvent() DrawingAreaInput() ManagerGadgetButtonMotion()\n\
     <BtnMotion>: wxCanvasMotionEvent() DrawingAreaInput() ManagerGadgetButtonMotion()\n\
     <Btn1Down>: DrawingAreaInput() ManagerGadgetArm()\n\
     <Btn2Down>: DrawingAreaInput() ManagerGadgetArm()\n\
     <Btn3Down>: DrawingAreaInput() ManagerGadgetArm()\n\
     <Btn1Up>: DrawingAreaInput() ManagerGadgetActivate()\n\
     <Btn2Up>: DrawingAreaInput() ManagerGadgetActivate()\n\
     <Btn3Up>: DrawingAreaInput() ManagerGadgetActivate()\n\
     <Motion>: wxCanvasMotionEvent() DrawingAreaInput()\n\
     <EnterWindow>: wxCanvasMotionEvent() DrawingAreaInput()\n\
     <LeaveWindow>: wxCanvasMotionEvent() DrawingAreaInput()\n\
     <Key>: DrawingAreaInput()";

  XtActionsRec actions[1];
  actions[0].string = "wxCanvasMotionEvent";
  actions[0].proc = (XtActionProc) wxCanvasMotionEvent;
  XtAppAddActions (wxTheApp->appContext, actions, 1);

  Widget parentWidget = 0;
  if (parent->IsKindOf(CLASSINFO(wxFrame)))
    parentWidget = ((wxFrame *)parent)->clientArea;
  else
    parentWidget = (Widget)parent->handle;
/*
  else if (parent->IsKindOf(CLASSINFO(wxPanel)))
    parentWidget = (Widget)parent->handle;
  else
  {
    wxError("Canvas subwindow must be a child of either a frame or panel!");
    return FALSE;
  }
*/

  if (style & wxBORDER)
    borderWidget = XtVaCreateManagedWidget ("canvasBorder",
				      xmFrameWidgetClass, parentWidget,
					    XmNshadowType, XmSHADOW_IN,
					    NULL);

  scrolledWindow = XtVaCreateManagedWidget ("scrolledWindow",
					    xmScrolledWindowWidgetClass, borderWidget ? borderWidget : parentWidget,
                                  XmNspacing, 0,
				  XmNscrollingPolicy, XmAPPLICATION_DEFINED,
					    NULL);

  XtTranslations ptr;
  Widget drawingArea = XtVaCreateWidget (windowName,
				   xmDrawingAreaWidgetClass, scrolledWindow,
					 XmNunitType, XmPIXELS,
//					 XmNresizePolicy, XmRESIZE_ANY,
					 XmNresizePolicy, XmRESIZE_NONE,
                                         XmNmarginHeight, 0,
                                         XmNmarginWidth, 0,
	      XmNtranslations, ptr = XtParseTranslationTable (translations),
					 NULL);

  if (windowStyle & wxOVERRIDE_KEY_TRANSLATIONS)
  {
    XtFree ((char *) ptr);
    ptr = XtParseTranslationTable ("<Key>: DrawingAreaInput()");
    XtOverrideTranslations (drawingArea, ptr);
    XtFree ((char *) ptr);
  }

  wxAddWindowToTable(drawingArea, this);
  wxAddWindowToTable(scrolledWindow, this);

  /*
   * This order is very important in Motif 1.2.1
   *
   */

  XtRealizeWidget (scrolledWindow);
  XtRealizeWidget (drawingArea);
  XtManageChild (drawingArea);

  XtOverrideTranslations (drawingArea,
		   ptr = XtParseTranslationTable ("<Configure>: resize()"));
  XtFree ((char *) ptr);

  XtAddCallback (drawingArea, XmNexposeCallback, (XtCallbackProc) wxCanvasRepaintProc, (XtPointer) this);
  XtAddCallback (drawingArea, XmNinputCallback, (XtCallbackProc) wxCanvasInputEvent, (XtPointer) this);

  handle = (char *) drawingArea;

  display = XtDisplay (scrolledWindow);
  xwindow = XtWindow (drawingArea);

  XtAddEventHandler (drawingArea, PointerMotionHintMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask,
    False, (XtEventHandler) wxCanvasEnterLeave, (XtPointer) this);
#endif
#ifdef wx_xview
  if (!parent->IsKindOf(CLASSINFO(wxFrame)))
  {
    wxError("A canvas subwindow must be a child of a frame in XView!");
    return FALSE;
  }
  wxFrame *frame = (wxFrame *)parent;

  // Since I have found that it's difficult to keep up with
  // all XView drag events, this counts down to zero before sending
  // an OnEvent message. If you want more sensitivity, reduce this value
  // and recompile wxWindows.
  DRAG_MAX = 0;
  selectionRequestor = 0;

  int real_y = frame->y_offset;
  if (y > -1)
    real_y = y + frame->y_offset;	// Allow for possible menu bar

  Frame x_frame = (Frame) frame->handle;
  Canvas x_canvas = (Canvas) xv_create (x_frame, CANVAS,
					CANVAS_X_PAINT_WINDOW, TRUE,
					WIN_CLIENT_DATA, (char *) this,
//                            CANVAS_RETAINED, is_retained,
					CANVAS_RETAINED, FALSE,
					XV_SHOW, FALSE,
					NULL);

  xv_set (canvas_paint_window (x_canvas), WIN_EVENT_PROC, wxCanvasEventProc,
	  WIN_CONSUME_EVENTS,
	  WIN_ASCII_EVENTS, KBD_USE, KBD_DONE,
	  LOC_DRAG, LOC_MOVE, LOC_WINENTER, LOC_WINEXIT, WIN_MOUSE_BUTTONS,
	  NULL,
	  WIN_CLIENT_DATA, (char *) this, NULL);
  handle = (char *) x_canvas;


  xv_set (x_canvas, XV_SHOW, TRUE, NULL);

  horiz_scroll = 0;
  vert_scroll = 0;
/*
  Xv_Screen screen = xv_get (xview_server, SERVER_NTH_SCREEN, 0);
  Xv_Window root_window = xv_get (screen, XV_ROOT);
  xwindow = (Window) xv_get (canvas_paint_window (x_canvas), XV_XID);

  display = (Display *) xv_get (root_window, XV_DISPLAY);
*/
  display = (Display *) wxGetDisplay();

  drag_count = DRAG_MAX;
//  xrects = NULL;
#endif
  if (parent)
    parent->AddChild (this);
  window_parent = parent;

  horiz_units = 0;
  vert_units = 0;

  if (this->IsKindOf(CLASSINFO(wxPanel)))
    wx_dc = new wxPanelDC ((wxPanel *)this);
  else
    wx_dc = new wxCanvasDC (this);

#ifdef wx_motif
  if (parent->IsKindOf(CLASSINFO(wxPanel)))
    ((wxPanel *)parent)->AttachWidget(this, 0, x, y, width, height);
  else
    SetSize (x, y, width, height);
#endif

#ifdef wx_xview
  xv_set (x_canvas,
	  CANVAS_REPAINT_PROC, wxCanvasRepaintProc,
//          CANVAS_RESIZE_PROC, wxCanvasResizeProc,
	  NULL);
  if (x > -1)
    xv_set (x_canvas, XV_X, x, NULL);
  if (y > -1)
    xv_set (x_canvas, XV_Y, real_y, NULL);
  if (width > -1)
    xv_set (x_canvas, XV_WIDTH, width, NULL);
  if (height > -1)
    xv_set (x_canvas, XV_HEIGHT, height, NULL);

#endif
  return TRUE;
}

wxCanvas::~wxCanvas (void)
{
#ifdef wx_motif
  if (backingPixmap)
    XFreePixmap (XtDisplay ((Widget) handle), backingPixmap);

  // This should be the right ordering now.
  // The (potential) children of a panel-canvas are deleted
  // in ~wxPanel. Now delete the canvas widget.
  
  Widget w = (Widget)handle;
  wxDeleteWindowFromTable(w);

  if (w)
    XtDestroyWidget(w);
  handle = NULL;

  // Only if we're _really_ a canvas (not a dialog box/panel)
  if (scrolledWindow)
  {
    wxDeleteWindowFromTable(scrolledWindow);
  }

  if (hScrollBar)
    {
      XtUnmanageChild (hScrollBar);
      XtDestroyWidget (hScrollBar);
    }
  if (vScrollBar)
    {
      XtUnmanageChild (vScrollBar);
      XtDestroyWidget (vScrollBar);
    }
  if (scrolledWindow)
  {
    XtUnmanageChild (scrolledWindow);
    XtDestroyWidget (scrolledWindow);
    handle = NULL;
  }

  if (borderWidget)
  {
    XtDestroyWidget (borderWidget);
    borderWidget = 0;
  }

#endif
#ifdef wx_xview
  Canvas x_canvas = (Canvas) handle;
  if (x_canvas)
  {
    Xv_window pw = canvas_paint_window (x_canvas);
    (void) xv_set (pw, WIN_CLIENT_DATA, NULL, NULL);
    xv_set (x_canvas,
  	  CANVAS_X_PAINT_WINDOW, FALSE, NULL);

    if (horiz_scroll)
      xv_destroy_safe ((Xv_opaque) horiz_scroll);
    if (vert_scroll)
      xv_destroy_safe ((Xv_opaque) vert_scroll);
  }
#endif
  if (wx_dc)
  {
    delete wx_dc;
    wx_dc = NULL;
  }
}

#ifdef wx_motif
Bool wxCanvas:: PreResize (void)
{
//  cout << "Canvas PreResize\n";
  //  OnPaint();
  return TRUE;
}
#endif

Bool wxCanvas::Show(Bool show)
{
#ifdef wx_motif
	if (!show)
		XtUnmapWidget(borderWidget ? borderWidget : scrolledWindow);
	else
		XtMapWidget(borderWidget ? borderWidget : scrolledWindow);
#else
	wxWindow::Show(show);
#endif
	return TRUE;
}

void wxCanvas:: SetColourMap (wxColourMap * cmap)
{
  if (wx_dc)
    wx_dc->SetColourMap(cmap);
}


void wxCanvas:: SetClientSize (int w, int h)
{
#ifdef wx_motif
//  SetSize(-1, -1, w, h);
  /* ALTERNATIVE CODE SUPPLIED BY ALS, NOT TESTED
   * IS THIS BETTER AND IF SO WHY!
   */
  Widget drawingArea = (Widget) handle;

  XtVaSetValues(drawingArea, XmNresizePolicy, XmRESIZE_ANY, NULL);

  if (w > -1)
    XtVaSetValues (drawingArea, XmNwidth, w, NULL);
  if (h > -1)
    XtVaSetValues (drawingArea, XmNheight, h, NULL);
  allowRepainting = FALSE;

  XSync (XtDisplay (drawingArea), FALSE);
  XEvent event;
  while (XtAppPending (wxTheApp->appContext))
    {
      XFlush (XtDisplay (drawingArea));
      XtAppNextEvent (wxTheApp->appContext, &event);
      XtDispatchEvent (&event);
    }
  XtVaSetValues(drawingArea, XmNresizePolicy, XmRESIZE_NONE, NULL);

  allowRepainting = TRUE;
  DoRefresh ();
  GetEventHandler()->OnSize (w, h);
#else
  wxWindow::SetClientSize (w, h);
#endif
}

void wxCanvas:: GetClientSize (int *w, int *h)
{
#ifdef wx_motif
//  XtVaGetValues((Widget)handle, XmNwidth, &xx, XmNheight, &yy, NULL);
  // Must return the same thing that was set via SetClientSize
  Dimension xx, yy;
  XtVaGetValues ((Widget) handle, XmNwidth, &xx, XmNheight, &yy, NULL);
  *w = xx;
  *h = yy;
#else
  wxWindow::GetClientSize (w, h);
#endif
}

void wxCanvas:: SetSize (int x, int y, int w, int h, int sizeFlags)
{
#ifdef wx_motif
  Widget drawingArea = (Widget) handle;
  Bool managed = XtIsManaged(borderWidget ? borderWidget : scrolledWindow);

  if (managed)
    XtUnmanageChild (borderWidget ? borderWidget : scrolledWindow);
  XtVaSetValues(drawingArea, XmNresizePolicy, XmRESIZE_ANY, NULL);

  if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    {
      XtVaSetValues (borderWidget ? borderWidget : scrolledWindow,
//       XmNleftAttachment, XmATTACH_SELF,
		     XmNx, x, NULL);
    }

  if (y > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    {
      XtVaSetValues (borderWidget ? borderWidget : scrolledWindow,
//       XmNtopAttachment, XmATTACH_SELF,
		     XmNy, y, NULL);
    }

  if (w > -1)
    {
//    XtVaSetValues(scrolledWindow, XmNwidth, w, NULL);
      if (borderWidget)
	{
	  XtVaSetValues (borderWidget, XmNwidth, w, NULL);
	  short thick, margin;
	  XtVaGetValues (borderWidget,
			 XmNshadowThickness, &thick,
			 XmNmarginWidth, &margin,
			 NULL);
	  w -= 2 * (thick + margin);
	}

      XtVaSetValues (scrolledWindow, XmNwidth, w, NULL);

      Dimension spacing;
      Widget sbar;
      XtVaGetValues (scrolledWindow,
		     XmNspacing, &spacing,
		     XmNverticalScrollBar, &sbar,
		     NULL);
      Dimension wsbar;
      if (sbar)
	XtVaGetValues (sbar, XmNwidth, &wsbar, NULL);
      else
	wsbar = 0;

      w -= (spacing + wsbar);

      XtVaSetValues (drawingArea, XmNwidth, w, NULL);

//    if (!hScroll)
      //      XtVaSetValues(drawingArea, XmNwidth, w - SCROLL_MARGIN, NULL);
    }
  if (h > -1)
    {
//    XtVaSetValues(scrolledWindow, XmNheight, h, NULL);
      if (borderWidget)
	{
	  XtVaSetValues (borderWidget, XmNheight, h, NULL);
	  short thick, margin;
	  XtVaGetValues (borderWidget,
			 XmNshadowThickness, &thick,
			 XmNmarginHeight, &margin,
			 NULL);
	  h -= 2 * (thick + margin);
	}

      XtVaSetValues (scrolledWindow, XmNheight, h, NULL);

      Dimension spacing;
      Widget sbar;
      XtVaGetValues (scrolledWindow,
		     XmNspacing, &spacing,
		     XmNhorizontalScrollBar, &sbar,
		     NULL);
      Dimension wsbar;
      if (sbar)
	XtVaGetValues (sbar, XmNheight, &wsbar, NULL);
      else
	wsbar = 0;

      h -= (spacing + wsbar);

      XtVaSetValues (drawingArea, XmNheight, h, NULL);

//    if (!vScroll)
      //      XtVaSetValues(drawingArea, XmNheight, h - SCROLL_MARGIN, NULL);
    }
  if (managed)
    XtManageChild (borderWidget ? borderWidget : scrolledWindow);
  XtVaSetValues(drawingArea, XmNresizePolicy, XmRESIZE_NONE, NULL);

/*
   allowRepainting = FALSE;

   XSync(XtDisplay(drawingArea), FALSE);
   XEvent event;
   while (XtAppPending(wxTheApp->appContext))
   {
   XFlush(XtDisplay(drawingArea));
   XtAppNextEvent(wxTheApp->appContext, &event);
   XtDispatchEvent(&event);
   }
   allowRepainting = TRUE;
   DoRefresh();
 */
#endif
#ifdef wx_xview
  if ((x == -1 || y == -1) && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    {
      int xx, yy;
      GetPosition (&xx, &yy);
      if (x == -1)
	x = xx;
      if (y == -1)
	y = yy;
    }

  if (w == -1 || h == -1)
    {
      int ww, hh;
      GetSize (&ww, &hh);
      if (w == -1)
	w = ww;
      if (h == -1)
	h = hh;
    }

  int real_y = y;

  if (window_parent)
    real_y += ((wxFrame *) window_parent)->y_offset;	// Allow for possible menu bar

  Xv_opaque x_win = (Xv_opaque) handle;

  (void) xv_set (x_win, XV_X, x, XV_Y, real_y, XV_WIDTH, w, XV_HEIGHT, h, NULL);

#endif
  int ww, hh;
  GetClientSize (&ww, &hh);
  GetEventHandler()->OnSize (ww, hh);
}

void wxCanvas:: GetSize (int *w, int *h)
{
#ifdef wx_motif
  Dimension xx, yy;
//  XtVaGetValues(scrolledWindow, XmNwidth, &xx, XmNheight, &yy, NULL);
  if (borderWidget)
    XtVaGetValues (borderWidget, XmNwidth, &xx, XmNheight, &yy, NULL);
  else if (scrolledWindow)
    XtVaGetValues (scrolledWindow, XmNwidth, &xx, XmNheight, &yy, NULL);
  else
    XtVaGetValues ((Widget)handle, XmNwidth, &xx, XmNheight, &yy, NULL);

  *w = xx;
  *h = yy;
#else
  wxWindow::GetSize (w, h);
#endif
}

void wxCanvas:: GetPosition (int *x, int *y)
{
#ifdef wx_motif
  Position xx, yy;
  XtVaGetValues (borderWidget ? borderWidget : scrolledWindow, XmNx, &xx, XmNy, &yy, NULL);
  *x = xx;
  *y = yy;
#else
  wxWindow::GetPosition (x, y);
#endif
}

#ifdef wx_motif
void wxCanvas::PhysicalScroll(int x, int y, int w, int h,
		int deltax, int deltay)
{
//  cerr << "Scrolling. delta = " << deltax << ", " << deltay << endl;
  Widget drawingArea = (Widget) handle;

  int x1 = deltax >= 0 ? x : x - deltax ;
  int y1 = deltay >= 0 ? y : y - deltay;
  int w1 = w - abs(deltax);
  int h1 = h - abs(deltay);
  int x2 = deltax >= 0 ? x + deltax : x;
  int y2 = deltay >= 0 ? y + deltay : y;
/*
  cerr << "Copying " << x1 << ", " << y1 << ", " << "width = " << w1;
  cerr << ", height = " << h1 << ", to " << x2 << ", " << y2 << endl;
*/
  int orig = GetDC()->GetLogicalFunction();
  GetDC()->SetLogicalFunction (wxCOPY);

//  XSetFunction(XtDisplay(drawingArea), GetDC()->gc, GXcopy);

  XCopyArea(XtDisplay(drawingArea), XtWindow(drawingArea),
    XtWindow(drawingArea), GetDC()->gc,
    x1, y1,
    w1, h1,
    x2, y2);

  GetDC()->autoSetting = TRUE;
  SetBrush(GetDC()->current_background_brush);

  // We'll add rectangles to the list of update rectangles
  // according to which bits we've exposed.
  updateRects.Clear();
	
  if (deltax > 0)
  {
    wxRectangle *rect = new wxRectangle;
    rect->x = x;
    rect->y = y;
    rect->width = deltax;
    rect->height = h;

//    cerr << "Filling rectangle " << rect->x << ", " << rect->y << ", ";
//    cerr << rect->width << ", " << rect->height << endl;

    XFillRectangle(XtDisplay(drawingArea), XtWindow(drawingArea),
    GetDC()->gc, rect->x, rect->y, rect->width, rect->height);

// Chris Breeze 27/7/97: see comment start of file for explanation
#ifdef UPDATE_ITERATOR_LOGICAL_COORDS
    rect->x = (int)GetDC() -> DeviceToLogicalX(rect->x);
    rect->y = (int)GetDC() -> DeviceToLogicalY(rect->y);
    rect->width = (int)GetDC() -> DeviceToLogicalXRel(rect->width);
    rect->height = (int)GetDC() -> DeviceToLogicalYRel(rect->height);
#else
    rect->x = rect->x;
    rect->y = rect->y;
    rect->width = rect->width;
    rect->height = rect->height;
#endif

    updateRects.Append(rect);
  }
  else if (deltax < 0)
  {
    wxRectangle *rect = new wxRectangle;

    rect->x = x + w + deltax;
    rect->y = y;
    rect->width = -deltax;
    rect->height = h;

//    cerr << "Filling rectangle " << rect->x << ", " << rect->y << ", ";
//    cerr << rect->width << ", " << rect->height << endl;

    XFillRectangle(XtDisplay(drawingArea), XtWindow(drawingArea),
      GetDC()->gc, rect->x, rect->y, rect->width,
      rect->height);

// Chris Breeze 27/7/97: see comment start of file for explanation
#ifdef UPDATE_ITERATOR_LOGICAL_COORDS
    rect->x = (int)GetDC() -> DeviceToLogicalX(rect->x);
    rect->y = (int)GetDC() -> DeviceToLogicalY(rect->y);
    rect->width = (int)GetDC() -> DeviceToLogicalXRel(rect->width);
    rect->height = (int)GetDC() -> DeviceToLogicalYRel(rect->height);
#else
    rect->x = rect->x;
    rect->y = rect->y;
    rect->width = rect->width;
    rect->height = rect->height;
#endif

    updateRects.Append(rect);
  }
  if (deltay > 0)
  {
    wxRectangle *rect = new wxRectangle;

    rect->x = x;
    rect->y = y;
    rect->width = w;
    rect->height = deltay;
//    cerr << "Filling rectangle " << rect->x << ", " << rect->y << ", ";
//    cerr << rect->width << ", " << rect->height << endl;

    XFillRectangle(XtDisplay(drawingArea), XtWindow(drawingArea),
      GetDC()->gc, rect->x, rect->y, rect->width, rect->height);

// Chris Breeze 27/7/97: see comment start of file for explanation
#ifdef UPDATE_ITERATOR_LOGICAL_COORDS
    rect->x = (int)GetDC() -> DeviceToLogicalX(rect->x);
    rect->y = (int)GetDC() -> DeviceToLogicalY(rect->y);
    rect->width = (int)GetDC() -> DeviceToLogicalXRel(rect->width);
    rect->height = (int)GetDC() -> DeviceToLogicalYRel(rect->height);
#else
    rect->x = rect->x;
    rect->y = rect->y;
    rect->width = rect->width;
    rect->height = rect->height;
#endif

    updateRects.Append(rect);
  }
  else if (deltay < 0)
  {
    wxRectangle *rect = new wxRectangle;

    rect->x = x;
    rect->y = y + h + deltay;
    rect->width = w;
    rect->height = -deltay;
//    cerr << "Filling rectangle " << rect->x << ", " << rect->y << ", ";
//    cerr << rect->width << ", " << rect->height << endl;

    XFillRectangle(XtDisplay(drawingArea), XtWindow(drawingArea),
      GetDC()->gc, rect->x, rect->y, rect->width, rect->height);

// Chris Breeze 27/7/97: see comment start of file for explanation
#ifdef UPDATE_ITERATOR_LOGICAL_COORDS
    rect->x = (int)GetDC() -> DeviceToLogicalX(rect->x);
    rect->y = (int)GetDC() -> DeviceToLogicalY(rect->y);
    rect->width = (int)GetDC() -> DeviceToLogicalXRel(rect->width);
    rect->height = (int)GetDC() -> DeviceToLogicalYRel(rect->height);
#else
    rect->x = rect->x;
    rect->y = rect->y;
    rect->width = rect->width;
    rect->height = rect->height;
#endif

    updateRects.Append(rect);
  }
  GetDC()->SetLogicalFunction (orig);

//  cerr << "About to paint" << endl;
  GetEventHandler()->OnPaint();
  updateRects.Clear();
}
#endif

#ifdef wx_motif
void wxCanvas::DoPaint(XRectangle *xrect, int n)
{
  DoRefresh();
}
#endif

#ifdef wx_motif
/* Calls OnPaint or uses retained pixmap,
 * as necessary
 */
void wxCanvas:: DoRefresh (void)
{
  int canvasWidth1;
  int canvasHeight1;
  GetClientSize (&canvasWidth1, &canvasHeight1);

  // Following test assure that callback is not called repeatedly.
  if (hScroll && scrolls_set_size)
    {
      int old_size, old_max, old_val;
      XtVaGetValues (hScrollBar,
       XmNsliderSize, &old_size,
       XmNmaximum, &old_max,
       XmNvalue, &old_val,
        NULL);
      int new_size =
        (int) (wxMax (wxMin (canvasWidth1 / horiz_units, hExtent / horiz_units), 1));
      int new_max = wxMax(new_size, old_max); //// KB: introduce and 
                                              ////     set new_max
      int new_val = old_val;
      if( old_val > new_max - new_size )      //// KB: if the old position is
      {                                       //// bigger than the maximal
	new_val = new_max - new_size;         //// possible position -> set to 
      }                                       //// it

      if (old_size != new_size)
	XtVaSetValues (hScrollBar,
         XmNmaximum, new_max,
         XmNsliderSize, new_size,
         XmNvalue, new_val,
         NULL);
    }
  if (vScroll && scrolls_set_size)
    {
      int old_size, old_max, old_val;
      XtVaGetValues (vScrollBar, XmNsliderSize, &old_size,
       XmNmaximum, &old_max,
       XmNvalue, &old_val,
       NULL);
      int new_size =
        (int) (wxMax (wxMin (canvasHeight1 / vert_units, vExtent / vert_units), 1));
      int new_max = wxMax(new_size, old_max); //// KB: introduce and 
                                              ////     set new_max
      int new_val = old_val;
      if( old_val > new_max - new_size )      //// KB: if the old position is
      {                                       //// bigger than the maximal
	new_val = new_max - new_size;         //// possible position -> set to 
      }                                       //// it

      if (old_size != new_size)
	XtVaSetValues (vScrollBar,
          XmNmaximum, new_max,
          XmNsliderSize, new_size,
          XmNvalue, new_val,
          NULL);
    }
  int x, y;
  ViewStart (&x, &y);
  if (is_retained && backingPixmap)
    {
      Widget drawingArea = (Widget) handle;
      int orig = GetDC()->GetLogicalFunction();
      GetDC()->SetLogicalFunction (wxCOPY);

//      XSetFunction(XtDisplay(drawingArea), GetDC()->gc, GXcopy);
      XCopyArea (XtDisplay (drawingArea), backingPixmap, XtWindow (drawingArea), GetDC ()->gc,
		 pixmapOffsetX, pixmapOffsetY,
		 pixmapWidth, pixmapHeight,
		 0, 0);

      GetDC()->SetLogicalFunction (orig);
    }
  else
    {
      GetEventHandler()->OnPaint ();
    }
}

void 
wxScrollCallback (Widget scrollbar, int orientation, XmScrollBarCallbackStruct * cbs)
{
  Widget scrolledWindow = XtParent (scrollbar);
  wxCanvas *canvas = (wxCanvas *) wxWidgetHashTable->Get ((long) scrolledWindow);
  if (canvas)
  {
    wxCommandEvent event;
    WXSCROLLPOS(event) = cbs->value;
    if (orientation == XmHORIZONTAL)
      WXSCROLLORIENT(event) = wxHORIZONTAL;
    else
      WXSCROLLORIENT(event) = wxVERTICAL;

    switch (cbs->reason)
    {
      case XmCR_INCREMENT:
      {
        event.eventType = wxEVENT_TYPE_SCROLL_LINEDOWN;
        break;
      }
      case XmCR_DECREMENT:
      {
        event.eventType = wxEVENT_TYPE_SCROLL_LINEUP;
        break;
      }
      case XmCR_PAGE_INCREMENT:
      {
        event.eventType = wxEVENT_TYPE_SCROLL_PAGEDOWN;
        break;
      }
      case XmCR_PAGE_DECREMENT:
      {
        event.eventType = wxEVENT_TYPE_SCROLL_PAGEUP;
        break;
      }
      case XmCR_TO_TOP:
      {
        event.eventType = wxEVENT_TYPE_SCROLL_TOP;
        break;
      }
      case XmCR_TO_BOTTOM:
      {
        event.eventType = wxEVENT_TYPE_SCROLL_BOTTOM;
        break;
      }
      case XmCR_DRAG:
      {
        event.eventType = wxEVENT_TYPE_SCROLL_THUMBTRACK;
        break;
      }
      case XmCR_VALUE_CHANGED:
      {
        event.eventType = wxEVENT_TYPE_SCROLL_THUMBTRACK;
        break;
      }
      default:
      {
        return;
      }
    }
    canvas->GetEventHandler()->OnScroll(event);
  }
}

#endif

/*
 * horizontal/vertical: number of pixels per unit (e.g. pixels per text line)
 * x/y_length:        : no. units per scrollbar
 * x/y_page:          : no. units per page scrolled
 */

void wxCanvas::SetScrollbars (int horizontal, int vertical,
	       int x_length, int y_length,
	       int x_page, int y_page,
	       int x_pos, int y_pos, Bool setVirtualSize)
{
  int xp, yp;
  xp = -1;
  yp = -1;
#ifdef wx_motif
  if (hScrollBar)
    XtVaGetValues (hScrollBar, XmNvalue, &xp, NULL);
  if (vScrollBar)
    XtVaGetValues (vScrollBar, XmNvalue, &yp, NULL);
#endif
#ifdef wx_xview
  if (horiz_scroll)
    xp = xv_get (horiz_scroll, SCROLLBAR_VIEW_START);

  if (vert_scroll)
    yp = xv_get (vert_scroll, SCROLLBAR_VIEW_START);
#endif

  Bool needRepaint = TRUE;
  if (horizontal == horiz_units &&
      vertical == vert_units &&
      x_pos == xp &&
      y_pos == yp &&
      x_page == units_per_page_x &&
      y_page == units_per_page_y
    ) {
	  if (x_length == units_x &&
	      y_length == units_y) {
	//DebugMsg("No change\n") ;
	      return;			/* Nothing changed */
	    }
		/*
			This flag is to avoid repainting (which causes
			flickering) when all we are doing
			is changing the virtual size of the canvas.
			Hernan Otero (hernan@isoft.com.ar)
		*/
    	needRepaint = FALSE;
	}

  horiz_units = horizontal;
  vert_units = vertical;
  units_per_page_x = x_page;
  units_per_page_y = y_page;
  units_x = x_length;
  units_y = y_length;
#ifdef wx_motif
  int w, h, x, y;
  GetSize (&w, &h);
  GetPosition (&x, &y);

  Bool scrollingInitialized = (hScrollBar || vScrollBar);
  int canvasWidth1;
  int canvasHeight1;
  GetClientSize (&canvasWidth1, &canvasHeight1);
  scrolls_set_size = setVirtualSize;

  Widget drawingArea = (Widget) handle;
  if (horizontal > 0)
    {
      if (setVirtualSize)
       hExtent = horizontal * x_length;
      else hExtent = 0;

      if (!hScroll)
	{
	  hScrollBar = XtVaCreateManagedWidget ("hsb",
				     xmScrollBarWidgetClass, scrolledWindow,
						XmNorientation, XmHORIZONTAL,
						NULL);
	  XtAddCallback (hScrollBar, XmNvalueChangedCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmHORIZONTAL);
	  XtAddCallback (hScrollBar, XmNdragCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmHORIZONTAL);
	  XtAddCallback (hScrollBar, XmNincrementCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmHORIZONTAL);
	  XtAddCallback (hScrollBar, XmNdecrementCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmHORIZONTAL);
	  XtAddCallback (hScrollBar, XmNpageIncrementCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmHORIZONTAL);
	  XtAddCallback (hScrollBar, XmNpageDecrementCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmHORIZONTAL);
	  XtAddCallback (hScrollBar, XmNtoTopCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmHORIZONTAL);
	  XtAddCallback (hScrollBar, XmNtoBottomCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmHORIZONTAL);
	}

      XtVaSetValues (hScrollBar,
		     XmNincrement, 1,
		     XmNvalue, x_pos,
		     NULL);
/*
      XtVaSetValues (hScrollBar,
		     XmNincrement, 1,
		     XmNpageIncrement, x_page,
		     XmNmaximum, x_length,
		     XmNvalue, x_pos,
		     XmNsliderSize, (int) (wxMax (wxMin (canvasWidth1 / horizontal, x_length), 1)),
		     NULL);
*/
	  hStart = x_pos;
      if (GetDC ())
	GetDC ()->device_origin_x = -(x_pos * horiz_units);
      if (requiresRetention)
	pixmapOffsetX = (x_pos * horiz_units);

      hScroll = TRUE;
    }
  else
    {
      hStart = 0;
      if (scrolls_set_size)
        hExtent = 0;
      hScroll = FALSE;
    }
  if (vertical > 0)
    {
      if (setVirtualSize)
        vExtent = vertical * y_length;
      else
        vExtent = 0;

      if (!vScroll)
	{
	  vScrollBar = XtVaCreateManagedWidget ("vsb",
				     xmScrollBarWidgetClass, scrolledWindow,
						XmNorientation, XmVERTICAL,
						NULL);
	  XtAddCallback (vScrollBar, XmNvalueChangedCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmVERTICAL);
	  XtAddCallback (vScrollBar, XmNdragCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmVERTICAL);
	  XtAddCallback (vScrollBar, XmNincrementCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmVERTICAL);
	  XtAddCallback (vScrollBar, XmNdecrementCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmVERTICAL);
	  XtAddCallback (vScrollBar, XmNpageIncrementCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmVERTICAL);
	  XtAddCallback (vScrollBar, XmNpageDecrementCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmVERTICAL);
	  XtAddCallback (vScrollBar, XmNtoTopCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmVERTICAL);
	  XtAddCallback (vScrollBar, XmNtoBottomCallback, (XtCallbackProc) wxScrollCallback, (XtPointer) XmVERTICAL);
	}

      XtVaSetValues (vScrollBar,
		     XmNincrement, 1,
		     XmNvalue, y_pos,
		     NULL);
/*
      XtVaSetValues (vScrollBar,
		     XmNincrement, 1,
		     XmNpageIncrement, y_page,
		     XmNmaximum, y_length,
		     XmNvalue, y_pos,
		     XmNsliderSize, (int) (wxMax (wxMin (canvasHeight1 / vertical, y_length), 1)),
		     NULL);
*/

	  vStart = y_pos;
      if (GetDC ())
	GetDC ()->device_origin_y = -(y_pos * vert_units);
      if (requiresRetention)
	pixmapOffsetY = (y_pos * vert_units);

      vScroll = TRUE;
    }
  else
    {
      vStart = 0;
      if (scrolls_set_size)
        vExtent = 0;
      vScroll = FALSE;
    }

  AdjustScrollbars();

  if (!scrollingInitialized)
    {
      XmScrolledWindowSetAreas (scrolledWindow, hScrollBar, vScrollBar, drawingArea);
      if (hScrollBar)
	XtRealizeWidget (hScrollBar);
      if (vScrollBar)
	XtRealizeWidget (vScrollBar);
    }

  /*
   * Retained pixmap stuff
   *
   */

  if (requiresRetention && (hExtent > 0) && (vExtent > 0))
    {
      if ((hExtent != pixmapWidth) || (vExtent != pixmapHeight))
	{
	  pixmapWidth = hExtent;
	  pixmapHeight = vExtent;

	  if (backingPixmap)
	    XFreePixmap (XtDisplay (drawingArea), backingPixmap);

	  backingPixmap = XCreatePixmap (XtDisplay (drawingArea),
	      RootWindowOfScreen (XtScreen (drawingArea)), hExtent, vExtent,
			     DefaultDepthOfScreen (XtScreen (drawingArea)));

	  if (backingPixmap)
	    is_retained = TRUE;
	  else
	    is_retained = FALSE;

		if (needRepaint) {
			Clear ();
			GetEventHandler()->OnPaint ();
		}
	}
    }
	if (needRepaint) {
  		// This necessary to make scrollbars appear, for some reason!
  		SetSize (x, y, w, h);
	}
#endif
#ifdef wx_xview
  Canvas canvas = (Canvas) handle;
  if (!horiz_scroll && horizontal > 0)
  {
    horiz_scroll = xv_create (canvas, SCROLLBAR,
                           WIN_CLIENT_DATA, (char *)this,
			   SCROLLBAR_DIRECTION, SCROLLBAR_HORIZONTAL, NULL);
    notify_interpose_event_func(xv_get(horiz_scroll, SCROLLBAR_NOTIFY_CLIENT),
     (Notify_func)wxCanvasScrollProc, NOTIFY_IMMEDIATE); //  NOTIFY_SAFE);
  }
  if (horizontal > 0)
    {
      int canvas_width = horizontal * x_length + 1;
      (void) xv_set (horiz_scroll,
		     SCROLLBAR_PIXELS_PER_UNIT, horizontal,
		     SCROLLBAR_OBJECT_LENGTH, x_length,
		     SCROLLBAR_PAGE_LENGTH, x_page,
		     SCROLLBAR_VIEW_LENGTH, x_page,
		     SCROLLBAR_VIEW_START, x_pos,
		     NULL);
      (void) xv_set (canvas, CANVAS_WIDTH, canvas_width,
		     CANVAS_AUTO_EXPAND, FALSE,
		     CANVAS_AUTO_SHRINK, FALSE,
		     NULL);
    }
  if (vertical > 0 && !vert_scroll)
  {
    vert_scroll = xv_create (canvas, SCROLLBAR,
                             WIN_CLIENT_DATA, (char *)this,
			     SCROLLBAR_DIRECTION, SCROLLBAR_VERTICAL, NULL);
    notify_interpose_event_func(xv_get(vert_scroll, SCROLLBAR_NOTIFY_CLIENT),
     (Notify_func)wxCanvasScrollProc, NOTIFY_IMMEDIATE); // NOTIFY_SAFE);
  }
  if (vertical > 0)
    {
      int canvas_height = vertical * y_length + 1;
      (void) xv_set (vert_scroll,
		     SCROLLBAR_PIXELS_PER_UNIT, vertical,
		     SCROLLBAR_OBJECT_LENGTH, y_length,
		     SCROLLBAR_PAGE_LENGTH, y_page,
		     SCROLLBAR_VIEW_LENGTH, y_page,
		     SCROLLBAR_VIEW_START, y_pos,
		     NULL);
      (void) xv_set (canvas, CANVAS_HEIGHT, canvas_height,
		     CANVAS_AUTO_EXPAND, FALSE,
		     CANVAS_AUTO_SHRINK, FALSE,
		     NULL);

    }
  if (requiresRetention)
    {
      (void) xv_set (canvas, CANVAS_RETAINED, TRUE,
		     NULL);
      is_retained = (Bool) xv_get (canvas, CANVAS_RETAINED, NULL);
    }
#endif
}

void wxCanvas::GetScrollUnitsPerPage (int *x_page, int *y_page)
{
  *x_page = units_per_page_x;
  *y_page = units_per_page_y;
}

void wxCanvas::GetScrollPixelsPerUnit (int *x_unit, int *y_unit)
{
  *x_unit = horiz_units;
  *y_unit = vert_units;
}

/*
 * Scroll to given position (scroll position, not pixel position)
 */

void wxCanvas:: Scroll (int x_pos, int y_pos)
{
  int old_x, old_y;
  ViewStart (&old_x, &old_y);
  if (((x_pos == -1) || (x_pos == old_x)) && ((y_pos == -1) || (y_pos == old_y)))
    return;

#ifdef wx_motif
  Bool clearCanvas = FALSE;
  if (hScroll && x_pos != -1)
    {
      int sliderSize, maxValue;
      XtVaGetValues (hScrollBar,
             XmNmaximum, &maxValue,
             XmNsliderSize, &sliderSize,
             NULL);
      x_pos = wxMin(x_pos, maxValue - sliderSize);
      XtVaSetValues (hScrollBar, XmNvalue, x_pos, NULL);

      hStart = x_pos;
      if (hScrollingEnabled && !is_retained)
		clearCanvas = TRUE;

      if (GetDC ())
	GetDC ()->device_origin_x = -(x_pos * horiz_units);
      if (is_retained)
	pixmapOffsetX = (x_pos * horiz_units);
    }
  if (vScroll && y_pos != -1)
    {
      int sliderSize, maxValue;
      XtVaGetValues (vScrollBar,
             XmNmaximum, &maxValue,
             XmNsliderSize, &sliderSize,
             NULL);
      y_pos = wxMin(y_pos, maxValue - sliderSize);
      XtVaSetValues (vScrollBar, XmNvalue, y_pos, NULL);

      vStart = y_pos;

      if (vScrollingEnabled && !is_retained)
		clearCanvas = TRUE;

      if (GetDC ())
	GetDC ()->device_origin_y = -(y_pos * vert_units);
      if (is_retained)
	pixmapOffsetY = (y_pos * vert_units);
    }

	if (clearCanvas) {
		int new_x, new_y;
		int width, height;
		ViewStart(&new_x, &new_y);
		GetClientSize(&width, &height);
		PhysicalScroll(0, 0, width, height,
				(old_x - new_x) * horiz_units,
				(old_y - new_y) * vert_units);
	} else
	  DoRefresh ();
#endif
#ifdef wx_xview
  if (x_pos > -1 && horiz_scroll)
    (void) xv_set (horiz_scroll, SCROLLBAR_VIEW_START, x_pos, NULL);

  if (y_pos > -1 && vert_scroll)
    (void) xv_set (vert_scroll, SCROLLBAR_VIEW_START, y_pos, NULL);
#endif
}

void wxCanvas:: EnableScrolling (Bool x_scroll, Bool y_scroll)
{
#ifdef wx_motif
  hScrollingEnabled = x_scroll;
  vScrollingEnabled = y_scroll;
#endif
}

void wxCanvas:: GetVirtualSize (int *x, int *y)
{
#ifdef wx_motif
  int x1, y1;
  GetClientSize (&x1, &y1);
  if (hExtent == 0)
    *x = x1;
  else
    *x = hExtent;

  if (vExtent == 0)
    *y = y1;
  else
    *y = vExtent;
#endif
#ifdef wx_xview
  Canvas canvas = (Canvas) handle;
  *x = (int) xv_get (canvas, CANVAS_WIDTH);
  *y = (int) xv_get (canvas, CANVAS_HEIGHT);
#endif
}

#ifdef wx_xview

void 
wxCanvasRepaintProc (Canvas x_canvas, Xv_Window paint_win,
		     Display * display, Window xwin, Xv_xrectlist * xrects)
{
  wxCanvas *canvas = (wxCanvas *) xv_get (x_canvas, WIN_CLIENT_DATA);

  if (canvas && display && xwin)
    {
      if (xrects && canvas->is_retained)
	{
//	  canvas->xrects = xrects;
	  XSetClipRectangles (canvas->display, canvas->GetDC ()->gc, 0, 0, xrects->rect_array, xrects->count, Unsorted);
	}

      canvas->paint_window = paint_win;
      canvas->display = display;
      canvas->xwindow = xwin;
      canvas->updateRects.Clear();

      if (xrects != NULL && xrects->count > 0)
      {
        int i;
        for (i = 0; i < xrects->count; i++)
        {
          wxRectangle *rect = new wxRectangle;
// Chris Breeze 27/7/97: see comment start of file for explanation
#ifdef UPDATE_ITERATOR_LOGICAL_COORDS
          rect->x = (int)canvas -> GetDC() -> DeviceToLogicalX(xrects->rect_array[i].x);
          rect->y = (int)canvas -> GetDC() -> DeviceToLogicalY(xrects->rect_array[i].y);
          rect->width = (int)canvas -> GetDC() -> DeviceToLogicalXRel(xrects->rect_array[i].width);
          rect->height = (int)canvas -> GetDC() -> DeviceToLogicalYRel(xrects->rect_array[i].height);
#else
          rect->x = rects->rect_array[i].x;
          rect->y = rects->rect_array[i].y;
          rect->width = rects->rect_array[i].width;
          rect->height = rects->rect_array[i].height;
#endif
          canvas->updateRects.Append(rect);
        }
	canvas->GetEventHandler()->OnPaint();
	canvas->updateRects.Clear();
/*
			wxRectangle *wxrects = wxRectArray;
			int n = xrects->count;
			if (n > NSTATIC_RECTS)
				wxrects = new wxRectangle[n];
			wxRectangle *wxrect = wxrects;
			XRectangle *xrect = xrects->rect_array;
			while (n--) {
				wxrect->x = xrect->x;
				wxrect->y = xrect->y;
				wxrect->width = xrect->width;
				wxrect->height = xrect->height;
				++wxrect;
				++xrect;
			}
      		canvas->GetEventHandler()->OnPaint(xrects->count, wxrects);
      		if (xrects->count > NSTATIC_RECTS)
      			delete [] wxrects;
		}
*/
//      canvas->xrects = NULL;

        XGCValues gc_val;
        gc_val.clip_mask = None;
        XChangeGC (canvas->display, canvas->GetDC ()->gc, GCClipMask, &gc_val);
      }
    }
}

void 
wxCanvasResizeProc (Canvas x_canvas, int width, int height)
{
  wxCanvas *canvas = (wxCanvas *) xv_get (x_canvas, WIN_CLIENT_DATA);
  if (canvas)
    {
      Xv_Window pw = canvas_paint_window (x_canvas);
      canvas->paint_window = pw;
      canvas->xwindow = (Window) xv_get (pw, XV_XID);
      canvas->GetEventHandler()->OnSize (width, height);
    }
}

void 
wxCanvasEventProc (Xv_Window window, Event * x_event, Notify_arg arg)
{
  wxCanvas *canvas = (wxCanvas *) xv_get (window, WIN_CLIENT_DATA);
  if (canvas)
    {
      canvas->paint_window = window;
      canvas->xwindow = (Window) xv_get (window, XV_XID);

/* Couldn't get drag and drop to work I'm afraid
   if ((event_action(x_event) == ACTION_DRAG_COPY) || (event_action(x_event) == ACTION_DRAG_MOVE))
   {
   Xv_drop_site ds;
   if (canvas->selectionRequestor &&
   (ds = dnd_decode_drop(canvas->selectionRequestor, x_event)))
   {
   int format;
   unsigned long length;
   char *data = (char *)xv_get(canvas->selectionRequestor, SEL_DATA, &length, &format);
   cout << "Got selection " << data << "\n";
   dnd_done(canvas->selectionRequestor);
   }
   }

   else 
 */

      if (event_id (x_event) == KBD_USE)
	canvas->GetEventHandler()->OnSetFocus ();
      else if (event_id (x_event) == KBD_DONE)
	canvas->GetEventHandler()->OnKillFocus ();
      else if (x_event->ie_xevent->xany.type == KeyPress)
      {
        KeySym keySym;
//        XComposeStatus compose;
//        (void) XLookupString ((XKeyEvent *) x_event->ie_xevent, wxBuffer, 20, &keySym, &compose);
        (void) XLookupString ((XKeyEvent *) x_event->ie_xevent, wxBuffer, 20, &keySym, NULL);
        int id = CharCodeXToWX (keySym);

        wxKeyEvent event (wxEVENT_TYPE_CHAR);
        if (event_shift_is_down (x_event))
	  event.shiftDown = TRUE;
	if (event_ctrl_is_down (x_event))
	  event.controlDown = TRUE;
	if (event_meta_is_down (x_event))
	  event.metaDown = TRUE;
	if (event_alt_is_down (x_event))
	  event.altDown = TRUE;
	event.keyCode = id;
	event.eventObject = canvas;
        event.SetTimestamp(1000 * event_time(x_event).tv_sec 
                          + event_time(x_event).tv_usec / 1000);

        event.x = event_x (x_event);
	event.y = event_y (x_event);

	if (canvas->GetDC ())
	{
	  event.x = canvas->GetDC ()->DeviceToLogicalX (event_x (x_event));
	  event.y = canvas->GetDC ()->DeviceToLogicalY (event_y (x_event));
	}

	if ((id > -1) && (id != WXK_SHIFT) && (id != WXK_CONTROL))
	  canvas->GetEventHandler()->OnChar (event);
      }
      else
	{
	  WXTYPE eventType = 0;
          if (event_id (x_event) == LOC_WINENTER)
          {
            eventType = wxEVENT_TYPE_ENTER_WINDOW;
          }
          else if (event_id (x_event) == LOC_WINEXIT)
          {
            eventType = wxEVENT_TYPE_LEAVE_WINDOW;
          }
	  else if ((event_id (x_event) == LOC_DRAG) || (event_id (x_event) == LOC_MOVE))
	    eventType = wxEVENT_TYPE_MOTION;
	  else if (event_is_button (x_event))
	    {
	      if (event_is_down (x_event))
		{
		  if (event_id (x_event) == BUT (1))
		    eventType = wxEVENT_TYPE_LEFT_DOWN;
		  else if (event_id (x_event) == BUT (2))
		    eventType = wxEVENT_TYPE_MIDDLE_DOWN;
		  else if (event_id (x_event) == BUT (3))
		    eventType = wxEVENT_TYPE_RIGHT_DOWN;
		}
	      else if (event_is_up (x_event))
		{
		  if (event_id (x_event) == BUT (1))
		    eventType = wxEVENT_TYPE_LEFT_UP;
		  else if (event_id (x_event) == BUT (2))
		    eventType = wxEVENT_TYPE_MIDDLE_UP;
		  else if (event_id (x_event) == BUT (3))
		    eventType = wxEVENT_TYPE_RIGHT_UP;
		}
	    }

	  if (eventType == 0)
	    return;

	  wxMouseEvent event (eventType);

          event.eventObject = canvas;
          event.controlDown = event_ctrl_is_down (x_event);
          event.shiftDown = event_shift_is_down (x_event);
          event.altDown = event_alt_is_down (x_event);
          event.metaDown = event_meta_is_down (x_event);
          event.leftDown = (eventType == wxEVENT_TYPE_LEFT_DOWN)
            || (event_left_is_down (x_event) 
                && (eventType != wxEVENT_TYPE_LEFT_UP));
          event.middleDown = (eventType == wxEVENT_TYPE_MIDDLE_DOWN)
            || (event_middle_is_down (x_event) 
                && (eventType != wxEVENT_TYPE_MIDDLE_UP));
          event.rightDown = (eventType == wxEVENT_TYPE_RIGHT_DOWN)
            || (event_right_is_down (x_event) 
                && (eventType != wxEVENT_TYPE_RIGHT_UP));
          event.eventHandle = (char *) x_event;
          event.SetTimestamp(1000 * event_time(x_event).tv_sec 
                          + event_time(x_event).tv_usec / 1000);

	  if (canvas->GetDC ())
	    {
	      event.x = canvas->GetDC ()->DeviceToLogicalX (event_x (x_event));
	      event.y = canvas->GetDC ()->DeviceToLogicalY (event_y (x_event));
	    }

	  if (event.Dragging ())
	    {
	      // Don't respond to ALL drag events since we can't necessarily
	      // keep up with them (dependent on application, really - define
	      // DRAG_MAX differently if necessary)
	      if (canvas->drag_count > 0)
		{
		  canvas->drag_count--;
		}
	      else
		{
		  canvas->drag_count = canvas->DRAG_MAX;
		  canvas->GetEventHandler()->OnEvent (event);
		}
	    }
	  else
	    canvas->GetEventHandler()->OnEvent (event);
	}
    }
}
#endif

#ifdef wx_motif

/* There follows a fix to a X event sequencing problem, contributed
 * by Scott Maxwell (maxwell@natasha.jpl.nasa.giv).
 */

// Maps wxCanvases to lists of XRectangles that need updating.
// wxHash is unfortunately not usable for the purpose since the
// wxCanvas * -> long conversion could lead to bogus matches.
// JACS, how about a third key type for wxHash: wxObject *?
typedef wxList inherited;
class wxRectMap: public wxList
{
    // Prevent copying.
    wxRectMap(const wxRectMap &);
    wxRectMap & operator = (const wxRectMap &);

    // Our map consists of Pair *s.
    class Pair
    {
	// Prevent copying.
	Pair(const Pair &);
	Pair & operator = (const Pair &);

    public:
	const wxCanvas & canv;
	wxList rectQ;

	Pair(const wxCanvas & canv_): canv(canv_)  {}
	~Pair(void)  {  rectQ.Clear();  }  // Needed?
    };

    wxNode * Append(wxObject * obj)  {  return inherited::Append(obj);  }
/*
    // Give linker error if we mistakenly try to use these.
    wxNode * Append(long key, wxObject * obj);
    wxNode * Append(char * key, wxObject * obj);
*/
    // Get the list node that maps from the specified canvas to its rectangle
    // list, or return 0 if canvas not found.
    wxNode * getPair(const wxCanvas & canvas)
    {
	for (wxNode * node = First(); node; node = node->Next())
	{
	    Pair * p = (Pair *) (node->Data());
	    
	    if (&(p->canv) == &canvas)
		return node;
	}

	return 0;
    }

public:
    wxRectMap(void)  {};
    ~wxRectMap(void);

    // Appends the given rect to the list of rects waiting to be updated
    // by canvas.  If necessary, create a new canvas/rectQ pair.
    // We take over ownership of rect but not of canvas.
    void Append(const wxCanvas & canvas, XRectangle & rect)
    {
	wxNode * node = getPair(canvas);

	if (!node)
	    node = Append((wxObject *) new Pair(canvas));

	Pair * pair = (Pair *) (node->Data());
	pair->rectQ.Append((wxObject *) &rect);
    }

    // Convert the list of rectangles stored for canvas into an array;
    // return a pointer to this array, and set n to its size.  Deletes
    // the corresponding Pair, so that further Appends for the same canvas
    // will start afresh.  Caller owns the returned array.
    XRectangle * GiveArray(const wxCanvas & canvas, int & N)
    {
	wxNode * node = getPair(canvas);

	if (!node)
	{
	    N = 0;
	    return 0;
	}

	Pair * p = (Pair *) (node->Data());
	wxList & rectQueue = p->rectQ;
	N = rectQueue.Number();

	XRectangle * xrects = new XRectangle[N];
	XRectangle * rectp = xrects;

	// Copy XRectangles into the array, freeing the old copies as we go.
	for (wxNode * iter = rectQueue.First(); iter; iter = iter->Next())
	{
	    XRectangle * xrect = (XRectangle *) (iter->Data());

	    *rectp++ = *xrect;
	    delete xrect;  xrect = 0;
	}

	delete p;  p = 0;
	DeleteNode(node);

	return xrects;
    }
};

wxRectMap::~wxRectMap(void)
    {
	// Drain the list if necessary -- should always be empty, though.
	while (Number())
	{
	    int bogus;

	    wxNode * node = First();

	    // As a side effect, this deletes the underlying Pair.
	    delete [] GiveArray(((Pair *) (node->Data()))->canv, bogus);
	}
    }

/*
void indent(int nLevels)  // Debugging only.
{
    if (nLevels < 0)
    {
	cerr << "!!! nLevels == " << nLevels << endl;
	return;
    }

    while (nLevels--)
	cerr << '\t';
    cerr << flush;
}
*/


void 
wxCanvasRepaintProc (Widget drawingArea, XtPointer clientData, XmDrawingAreaCallbackStruct * cbs)
// void wxCanvasRepaintProc(Widget w, XtPointer c_data, XEvent *event, char *)
{
    if (!wxWidgetHashTable->Get ((long) drawingArea))
	return;

    static wxRectMap rectMap;
//    static int depth = -1;  // Debugging only.

    XEvent * event = cbs->event;
    wxCanvas * canvas = (wxCanvas *) clientData;
    Display * display = (Display *) canvas->GetXDisplay();
    GC gc = (GC) canvas->GetDC()->gc;

    switch (event->type)
    {
      case Expose:
      {
	XRectangle * xrect = new XRectangle;

	xrect->x = event->xexpose.x;
	xrect->y = event->xexpose.y;
	xrect->width = event->xexpose.width;
	xrect->height = event->xexpose.height;

	/*
	++depth;
	indent(depth);
	
	cerr << "Appending to " << hex << canvas << dec
	     << " (" << event->xexpose.count << " to go)." << endl;
	*/
	
	// Append this rectangle to the queue of rectangles to be updated
	// by this canvas.  After this operation, rectMap owns the memory
	// pointed to by xrect, so do not delete it.
	rectMap.Append(*canvas, *xrect);

	if (!event->xexpose.count)
	{
	    int n;
	    XRectangle * xrects = rectMap.GiveArray(*canvas, n);

	    /*
	    indent(depth);
	    cerr << "Servicing " << hex << canvas << dec
		 << " (" << n << " queued) ... " << endl;
	    */
	    
	    XSetClipRectangles(display, gc, 0, 0, xrects, n, Unsorted);

            canvas->updateRects.Clear();
            int llp;
            for (llp = 0;llp < n;llp++)
            {
              wxRectangle *rect = new wxRectangle;
// Chris Breeze 27/7/97: see comment start of file for explanation
#ifdef UPDATE_ITERATOR_LOGICAL_COORDS
              rect->x = (int)canvas -> GetDC() -> DeviceToLogicalX(xrects[llp].x);
              rect->y = (int)canvas -> GetDC() -> DeviceToLogicalY(xrects[llp].y);
              rect->width = (int)canvas -> GetDC() -> DeviceToLogicalXRel(xrects[llp].width);
              rect->height = (int)canvas -> GetDC() -> DeviceToLogicalYRel(xrects[llp].height);
#else
              rect->x = xrects[llp].x;
              rect->y = xrects[llp].y;
              rect->width = xrects[llp].width;
              rect->height = xrects[llp].height;
#endif
              canvas->updateRects.Append(rect);
            }

            canvas->DoPaint(xrects, n);

            canvas->updateRects.Clear();

	    // We own the array pointed to by xrects, so delete it.
	    delete [] xrects;  xrects = 0;

	    XGCValues gc_val;
	    gc_val.clip_mask = None;
	    XChangeGC(display, gc, GCClipMask, &gc_val);
	}
//	--depth;
	break;
    }
    default:
    {
	cout << "\n\nNew Event ! is = " << event -> type << "\n";
	break;
    }
  }
}

// Code with X event sequencing problems
#if 0
void 
wxCanvasRepaintProc (Widget drawingArea, XtPointer clientData, XmDrawingAreaCallbackStruct * cbs)
// void wxCanvasRepaintProc(Widget w, XtPointer c_data, XEvent *event, char *)
   {
  if (!wxWidgetHashTable->Get ((long) drawingArea))
    return;

  wxCanvas *canvas = (wxCanvas *) clientData;

//     wxCanvas *canvas;
     Window window;
     static XRectangle *xrect;
     Display *display;
     GC gc;
     int llp = 0;
//     int ppl;
     static int last_count = 0;
     static int draw_count = 0;
     XEvent *event = cbs->event;

//     canvas = (wxCanvas *) c_data;

     switch(event -> type)
        {
          case Expose :
               window = (Window) canvas -> GetXWindow();
               display = (Display *) canvas -> GetXDisplay();
               gc = (GC) canvas -> GetDC() -> gc;
               
               llp = event -> xexpose.count;
               
               if ((last_count == 0) && (llp == 0))
                  {
                    xrect = new XRectangle[1];
                    xrect[0].x = event -> xexpose.x;
                    xrect[0].y = event -> xexpose.y;
                    xrect[0].width = event -> xexpose.width;
                    xrect[0].height = event -> xexpose.height;

                    XSetClipRectangles(display,gc,0,0,xrect,1,Unsorted);
                    canvas -> DoPaint(xrect,1);
                    delete[] xrect;

                    // This line is an attempt to restore canvas to no clipping: JACS
//                    XSetClipMask (display, gc, None);

                    // Didn't work; try this instead. JACS.
                    XGCValues gc_val;
                    gc_val.clip_mask = None;
                    XChangeGC (display, gc, GCClipMask, &gc_val);

                  }

               if ((last_count == 0) && (llp != 0))
                  {
                    xrect = new XRectangle[llp + 1];
                    draw_count = llp + 1;
                    
                    xrect[draw_count - llp - 1].x = event -> xexpose.x;
                    xrect[draw_count - llp - 1].y = event -> xexpose.y;
                    xrect[draw_count - llp - 1].width = event -> xexpose.width;
                    xrect[draw_count - llp - 1].height = event -> xexpose.height;
                  }

               if ((last_count != 0) && (llp != 0))
                  {
                    xrect[draw_count - llp - 1].x = event -> xexpose.x;
                    xrect[draw_count - llp - 1].y = event -> xexpose.y;
                    xrect[draw_count - llp - 1].width = event -> xexpose.width;
                    xrect[draw_count - llp - 1].height = event -> xexpose.height;
                  }
               
               if ((last_count != 0) && (llp == 0))
                  {
                    xrect[draw_count - llp - 1].x = event -> xexpose.x;
                    xrect[draw_count - llp - 1].y = event -> xexpose.y;
                    xrect[draw_count - llp - 1].width = event -> xexpose.width;
                    xrect[draw_count - llp - 1].height = event -> xexpose.height;

                    XSetClipRectangles(display,gc,0,0,xrect,draw_count,Unsorted);
                    canvas -> DoPaint(xrect,draw_count);
                    delete[] xrect;

                    // This line is an attempt to restore canvas to no clipping: JACS
//                    XSetClipMask (display, gc, None);
                    // Didn't work; try this instead
                    XGCValues gc_val;
                    gc_val.clip_mask = None;
                    XChangeGC (display, gc, GCClipMask, &gc_val);
                  }
               last_count = event -> xexpose.count;
               break;
          default :
               cout << "\n\nNew Event ! is = " << event -> type << "\n";
               break;
        }
   }
#endif

// Unable to deal with Enter/Leave without a separate EventHandler (Motif 1.1.4)
void 
wxCanvasEnterLeave (Widget drawingArea, XtPointer clientData, XCrossingEvent * event)
{
  XmDrawingAreaCallbackStruct cbs;
  XEvent ev;

  //if (event->mode!=NotifyNormal)
  //  return ;

//  ev = *((XEvent *) event); // Causes Purify error (copying too many bytes)
  ((XCrossingEvent &) ev) = *event;

  cbs.reason = XmCR_INPUT;
  cbs.event = &ev;

  wxCanvasInputEvent (drawingArea, (XtPointer) NULL, &cbs);
}

// Fix to make it work under Motif 1.0 (!)
void 
wxCanvasMotionEvent (Widget drawingArea, XButtonEvent * event)
{
#if   XmVersion<=1000

  XmDrawingAreaCallbackStruct cbs;
  XEvent ev;

  //ev.xbutton = *event;
  ev = *((XEvent *) event);
  cbs.reason = XmCR_INPUT;
  cbs.event = &ev;

  wxCanvasInputEvent (drawingArea, (XtPointer) NULL, &cbs);
#endif
}

void
wxCanvasInputEvent (Widget drawingArea, XtPointer data, XmDrawingAreaCallbackStruct * cbs)
{
  wxCanvas *canvas = (wxCanvas *) wxWidgetHashTable->Get ((long) drawingArea);
  XEvent local_event;

  if (canvas==NULL)
    return ;

  if (cbs->reason != XmCR_INPUT)
    return;

  local_event = *(cbs->event);	// We must keep a copy!

  switch (local_event.xany.type)
    {
    case EnterNotify:
    case LeaveNotify:
    case ButtonPress:
    case ButtonRelease:
    case MotionNotify:
      {
	WXTYPE eventType = 0;

	if (local_event.xany.type == EnterNotify)
	  {
	    //if (local_event.xcrossing.mode!=NotifyNormal)
	    //  return ; // Ignore grab events
	    eventType = wxEVENT_TYPE_ENTER_WINDOW;
//            canvas->GetEventHandler()->OnSetFocus();
	  }
	else if (local_event.xany.type == LeaveNotify)
	  {
	    //if (local_event.xcrossing.mode!=NotifyNormal)
	    //  return ; // Ignore grab events
	    eventType = wxEVENT_TYPE_LEAVE_WINDOW;
//            canvas->GetEventHandler()->OnKillFocus();
	  }
	else if (local_event.xany.type == MotionNotify)
	  {
	    eventType = wxEVENT_TYPE_MOTION;
	    if (local_event.xmotion.is_hint == NotifyHint)
	      {
		Window root, child;
		Display *dpy = XtDisplay (drawingArea);

		XQueryPointer (dpy, XtWindow (drawingArea),
			       &root, &child,
			       &local_event.xmotion.x_root,
			       &local_event.xmotion.y_root,
			       &local_event.xmotion.x,
			       &local_event.xmotion.y,
			       &local_event.xmotion.state);
//fprintf(stderr,"*") ; fflush(stderr) ;
	      }
	    else
	      {
//fprintf(stderr,".") ; fflush(stderr) ;
	      }
	  }

	else if (local_event.xany.type == ButtonPress)
	  {
	    if (local_event.xbutton.button == Button1)
	      {
		eventType = wxEVENT_TYPE_LEFT_DOWN;
		canvas->button1Pressed = TRUE;
	      }
	    else if (local_event.xbutton.button == Button2)
	      {
		eventType = wxEVENT_TYPE_MIDDLE_DOWN;
		canvas->button2Pressed = TRUE;
	      }
	    else if (local_event.xbutton.button == Button3)
	      {
		eventType = wxEVENT_TYPE_RIGHT_DOWN;
		canvas->button3Pressed = TRUE;
	      }
	  }
	else if (local_event.xany.type == ButtonRelease)
	  {
	    if (local_event.xbutton.button == Button1)
	      {
		eventType = wxEVENT_TYPE_LEFT_UP;
		canvas->button1Pressed = FALSE;
	      }
	    else if (local_event.xbutton.button == Button2)
	      {
		eventType = wxEVENT_TYPE_MIDDLE_UP;
		canvas->button2Pressed = FALSE;
	      }
	    else if (local_event.xbutton.button == Button3)
	      {
		eventType = wxEVENT_TYPE_RIGHT_UP;
		canvas->button3Pressed = FALSE;
	      }
	  }

	wxMouseEvent wxevent (eventType);
	wxevent.eventHandle = (char *) &local_event;

	if (canvas->GetDC ())
	  {
	    wxevent.x = canvas->GetDC ()->DeviceToLogicalX (local_event.xbutton.x);
	    wxevent.y = canvas->GetDC ()->DeviceToLogicalY (local_event.xbutton.y);
	  }

/*
	wxevent.leftDown = canvas->button1Pressed;
	wxevent.middleDown = canvas->button2Pressed;
	wxevent.rightDown = canvas->button3Pressed;
*/
	wxevent.leftDown = ((eventType == wxEVENT_TYPE_LEFT_DOWN)
			    || (event_left_is_down (&local_event) 
				&& (eventType != wxEVENT_TYPE_LEFT_UP)));
	wxevent.middleDown = ((eventType == wxEVENT_TYPE_MIDDLE_DOWN)
			      || (event_middle_is_down (&local_event) 
				  && (eventType != wxEVENT_TYPE_MIDDLE_UP)));
	wxevent.rightDown = ((eventType == wxEVENT_TYPE_RIGHT_DOWN)
			     || (event_right_is_down (&local_event) 
				 && (eventType != wxEVENT_TYPE_RIGHT_UP)));

	wxevent.shiftDown = local_event.xbutton.state & ShiftMask;
	wxevent.controlDown = local_event.xbutton.state & ControlMask;
        wxevent.altDown = local_event.xbutton.state & Mod3Mask;
        wxevent.metaDown = local_event.xbutton.state & Mod1Mask;
	wxevent.eventObject = canvas;
        wxevent.SetTimestamp(local_event.xbutton.time);

    // Now check if we need to translate this event into a double click
    if (canvas->doubleClickAllowed)
    {
       if (wxevent.ButtonDown())
       {
	     // get button and time-stamp
	     int button = 0;
	     if      (wxevent.LeftDown())   button = 1;
	     else if (wxevent.MiddleDown()) button = 2;
	     else if (wxevent.RightDown())  button = 3;
	     long ts = wxevent.GetTimestamp();
	     // check, if single or double click
	     if (canvas->lastButton && canvas->lastButton==button && (ts - canvas->lastTS) < canvas->dclickTime)
	       {
	        // I have a dclick
		 canvas->lastButton = 0;
		 switch ( eventType )
		   {
		   case wxEVENT_TYPE_LEFT_DOWN:
		     wxevent.eventType = wxEVENT_TYPE_LEFT_DCLICK;
		     break;
		   case wxEVENT_TYPE_MIDDLE_DOWN:
		     wxevent.eventType = wxEVENT_TYPE_MIDDLE_DCLICK;
		     break;
		   case wxEVENT_TYPE_RIGHT_DOWN:
		     wxevent.eventType = wxEVENT_TYPE_RIGHT_DCLICK;
		     break;

		   default :
		     break;
		   }

	       }
	     else
	       {
		 // not fast enough or different button
		 canvas->lastTS     = ts;
		 canvas->lastButton = button;
	       }
       }
    }

	canvas->GetEventHandler()->OnEvent (wxevent);
	/*
	if (eventType == wxEVENT_TYPE_ENTER_WINDOW ||
	    eventType == wxEVENT_TYPE_LEAVE_WINDOW ||
	    eventType == wxEVENT_TYPE_MOTION
	  )
	  return;
	  */
	break;
      }
    case KeyPress:
      {
	KeySym keySym;
//	XComposeStatus compose;
//	(void) XLookupString ((XKeyEvent *) & local_event, wxBuffer, 20, &keySym, &compose);
	(void) XLookupString ((XKeyEvent *) & local_event, wxBuffer, 20, &keySym, NULL);
	int id = CharCodeXToWX (keySym);

	wxKeyEvent event (wxEVENT_TYPE_CHAR);

	if (local_event.xkey.state & ShiftMask)
	  event.shiftDown = TRUE;
	if (local_event.xkey.state & ControlMask)
	  event.controlDown = TRUE;
	if (local_event.xkey.state & Mod3Mask)
	  event.altDown = TRUE;
	if (local_event.xkey.state & Mod1Mask)
	  event.metaDown = TRUE;
	event.eventObject = canvas;
	event.keyCode = id;
        event.SetTimestamp(local_event.xkey.time);

	if (canvas->GetDC ())
	  {
	    event.x = canvas->GetDC ()->DeviceToLogicalX (local_event.xbutton.x);
	    event.y = canvas->GetDC ()->DeviceToLogicalY (local_event.xbutton.y);
	  }

	if (id > -1)
        {
          // Implement wxFrame::OnCharHook by checking ancestor.
          wxWindow *parent = canvas->GetParent();
          while (parent && !parent->IsKindOf(CLASSINFO(wxFrame)))
            parent = parent->GetParent();
            
          if (parent)
          {
            if (parent->GetEventHandler()->OnCharHook(event))
              return;
          }

	  canvas->GetEventHandler()->OnChar (event);
        }
	break;
      }
    case FocusIn:
      {
        if (local_event.xfocus.detail != NotifyPointer)
          canvas->GetEventHandler()->OnSetFocus ();
	break;
      }
    case FocusOut:
      {
        if (local_event.xfocus.detail != NotifyPointer)
	  canvas->GetEventHandler()->OnKillFocus ();
        break;
      }
    default:
      break;
    }
}

#endif

// Where the current view starts from
void wxCanvas:: ViewStart (int *x, int *y)
{
#ifdef wx_motif
  int xx, yy;
  if (hScroll)
    XtVaGetValues (hScrollBar, XmNvalue, &xx, NULL);
  else
    xx = 0;
  if (vScroll)
    XtVaGetValues (vScrollBar, XmNvalue, &yy, NULL);
  else
    yy = 0;
  *x = xx;
  *y = yy;
#endif
#ifdef wx_xview
  if (horiz_scroll)
    *x = (int) xv_get (horiz_scroll, SCROLLBAR_VIEW_START);
  else
    *x = 0;

  if (vert_scroll)
    *y = (int) xv_get (vert_scroll, SCROLLBAR_VIEW_START);
  else
    *y = 0;

#endif
}

#ifdef wx_xview
/*
 * Doesn't work yet -- I don't know how to get selections
 */
void wxCanvas:: DragAcceptFiles (Bool accept)
{
/*
   if (accept)
   {
   if (dropSite)
   xv_destroy(dropSite);

   dropSite = xv_create(canvas_paint_window((Canvas)handle), DROP_SITE_ITEM,
   DROP_SITE_ID, NewId(),
   DROP_SITE_REGION, xv_get(canvas_paint_window((Canvas)handle), WIN_RECT),
   DROP_SITE_EVENT_MASK, DND_ENTERLEAVE,
   NULL);
   if (!selectionRequestor)
   selectionRequestor = xv_create((Canvas)handle, SELECTION_REQUESTOR, NULL);
   }
   else if (dropSite) xv_destroy(dropSite);
 */
}
#endif

void wxCanvas:: WarpPointer (int x_pos, int y_pos)
{
  // Move the pointer to (x_pos,y_pos) coordinates. They are expressed in
  // pixel coordinates, relatives to the canvas -- So, we only need to
  // substract origin of the window.

  if (GetDC ())
    {
      x_pos += (int) (GetDC ()->device_origin_x);
      y_pos += (int) (GetDC ()->device_origin_y);
    }
#ifdef wx_motif
#endif
#ifdef wx_xview
#endif
  XWarpPointer (display, None, xwindow, 0, 0, 0, 0, x_pos, y_pos);

}

wxCursor *wxCanvas:: SetCursor (wxCursor * cursor)
{
  return wxWindow::SetCursor (cursor);
}


// Chris Breeze 22/07/97: updated to use the same
// calculations as DoRefresh()
void wxCanvas::AdjustScrollbars(void)
{
	int w, h;
	GetClientSize(&w, &h);

	if (hScroll && horiz_units)
	{
		int origPos = GetScrollPos(wxHORIZONTAL);
		int sliderSize = (int)wxMax(wxMin(w / horiz_units, hExtent / horiz_units), 1);
		int noPositions = units_x - sliderSize;

		SetScrollPage(wxHORIZONTAL, sliderSize);
		SetScrollRange(wxHORIZONTAL, noPositions);

		// scroll canvas if slider pos has changed
		int value = GetScrollPos(wxHORIZONTAL);
		if (origPos != value)
		{
			wxCommandEvent event;
			WXSCROLLPOS(event) = value;
			WXSCROLLORIENT(event) = wxHORIZONTAL;
			event.eventType = wxEVENT_TYPE_SCROLL_THUMBTRACK;
			GetEventHandler()->OnScroll(event);
		}
	}

	if (vScroll && vert_units)
	{
		int origPos = GetScrollPos(wxVERTICAL);
		int sliderSize = (int)wxMax(wxMin(h / vert_units, vExtent / vert_units), 1);
		int noPositions = units_y - sliderSize;

		SetScrollPage(wxVERTICAL, sliderSize);
		SetScrollRange(wxVERTICAL, noPositions);

		// scroll canvas if slider pos has changed
		int value = GetScrollPos(wxVERTICAL);
		if (origPos != value)
		{
			wxCommandEvent event;
			WXSCROLLPOS(event) = value;
			WXSCROLLORIENT(event) = wxVERTICAL;
			event.eventType = wxEVENT_TYPE_SCROLL_THUMBTRACK;
			GetEventHandler()->OnScroll(event);
		}
	}
}

// Update the page sizes
void wxCanvas::OnSize(int x, int y)
{
#ifdef wx_motif
  AdjustScrollbars();
#endif
  wxWindow::OnSize(x, y);
}

// Default scrolling behaviour
void wxCanvas::OnScroll(wxCommandEvent& event)
{
#ifdef wx_motif
  int newx, newy;
  ViewStart(&newx, &newy);
  int oldScrollX = hStart;
  int oldScrollY = vStart;

  Bool doScroll = FALSE;
  int value = WXSCROLLPOS(event);

  if (WXSCROLLORIENT(event) == wxHORIZONTAL)
  {
    if (hScrollingEnabled && !is_retained)
      doScroll = TRUE;

    hStart = value;
    if (GetDC ())
      GetDC ()->device_origin_x = -(value * horiz_units);
    if (is_retained)
      pixmapOffsetX = (value * horiz_units);
  }
  else
  {
    if (vScrollingEnabled && !is_retained)
      doScroll = TRUE;

     vStart = value;
     if (GetDC ())
       GetDC ()->device_origin_y = -(value * vert_units);
     if (is_retained)
       pixmapOffsetY = (value * vert_units);
  }
  if (doScroll)
  {
    int width, height;
    GetClientSize(&width, &height);
    PhysicalScroll(0, 0, width, height,
		(oldScrollX - newx) * horiz_units

    // This is an OBOB correction.... I don't know who's fault
    // it is, but if this is not here... the horizontal
    // scrolling is offset by 1.
    // Hernan Otero (hernan@isoft.com.ar)
/* Taken out by JACS 21/5/95: Alexey Iskhakov's new wx_dc.h macros
 * should cure the problem.
        + (oldx == 0 && newx != 0 ? 1 :
	  (newx == 0 && oldx != 0 ? -1 : 0))
*/
      ,
      
      (oldScrollY - newy) * vert_units);
  }
  else
  {
    DoRefresh ();
  }
#endif
#ifdef wx_xview
  Scrollbar sbar = 0;
  if ((WXSCROLLORIENT(event) == wxHORIZONTAL) && horiz_scroll)
  {
    sbar = horiz_scroll;
  }
  else if ((WXSCROLLORIENT(event) == wxVERTICAL) && horiz_scroll)
  {
    sbar = vert_scroll;
  }
  else
    return;

  if (xviewClient)
  {
    xviewReturnValue = notify_next_event_func(xviewClient,
        (Notify_event)xviewEvent, sbar, xviewEventType);
  }
#endif
}

void wxCanvas::SetScrollPos(int orient, int pos)
{
#ifdef wx_motif
	Widget bar = ((orient == wxHORIZONTAL) ? hScrollBar : vScrollBar);
	if (bar)
	{
		XtVaSetValues (bar,
				XmNvalue, pos,
				NULL);
	}
#endif
#ifdef wx_xview
  Scrollbar bar = ((orient == wxHORIZONTAL) ? horiz_scroll : vert_scroll);
  if (bar)
    xv_set (bar, SCROLLBAR_VIEW_START, pos, NULL);
#endif
}

void wxCanvas::SetScrollRange(int orient, int range)
{
#ifdef wx_motif
	// Chris Breeze 22/07/97: restructure so that only the range
	// is set directly. Now calls SetScrollPos() if the pos needs
	// to be changed

	int pageSize;
	if (orient == wxHORIZONTAL)
	{
		range_x = range;
		pageSize = units_per_page_x;
	}
	else
	{
		range_y = range;
		pageSize = units_per_page_y;
	}

	Widget bar = ((orient == wxHORIZONTAL) ? hScrollBar : vScrollBar);
	if (bar)
	{
		// avoid pos being greater than range
		if (GetScrollPos(orient) > range) SetScrollPos(orient, range);

		XtVaSetValues (bar,
				XmNmaximum, range + pageSize,
				NULL);
	}
#endif
#ifdef wx_xview
  Scrollbar bar = ((orient == wxHORIZONTAL) ? horiz_scroll : vert_scroll);
  // This probably isn't right.
  // The range is the number of steps available, which _depends_ on the view
  // and object length.
  if (bar)
      xv_set (bar,
	      SCROLLBAR_OBJECT_LENGTH, range, 
	      SCROLLBAR_VIEW_LENGTH, range, 
	      NULL);
#endif
}

int wxCanvas::GetScrollPos(int orient)
{
#ifdef wx_motif
  Widget bar = ((orient == wxHORIZONTAL) ? hScrollBar : vScrollBar);
  int d;
  if (bar) {
    XtVaGetValues (bar,
		   XmNvalue, &d,
		   NULL);
    return (int)d;
  }
#endif
#ifdef wx_xview
  Scrollbar bar = ((orient == wxHORIZONTAL) ? horiz_scroll : vert_scroll);
  if (bar)
    return (int)xv_get(bar, SCROLLBAR_VIEW_START);
#endif
  return 0;
}

int wxCanvas::GetScrollRange(int orient)
{
#ifdef wx_motif
  return ((orient == wxHORIZONTAL) ? range_x : range_y);

/*
  Widget bar = ((orient == wxHORIZONTAL) ? hScrollBar : vScrollBar);
  int d;
  if (bar) {
    XtVaGetValues (bar,
		   XmNmaximum, &d,
		   NULL);
    return d - ((orient == wxHORIZONTAL) ? units_per_page_x:units_per_page_y);
  }
*/
#endif
#ifdef wx_xview
  Scrollbar bar = ((orient == wxHORIZONTAL) ? horiz_scroll : vert_scroll);
  if (bar)
    return (int) xv_get (bar, SCROLLBAR_OBJECT_LENGTH);
#endif  
  return 0;
}

void wxCanvas::SetScrollPage(int orient, int page)
{
  if (orient == wxHORIZONTAL)
    units_per_page_x = page;
  else
    units_per_page_y = page;

#ifdef wx_motif
	Widget bar = ((orient == wxHORIZONTAL) ? hScrollBar : vScrollBar);
	int range = (orient == wxHORIZONTAL) ? range_x : range_y;

	if (bar)
	{
		XtVaSetValues (bar,
			XmNsliderSize,		page,
			XmNmaximum,			range + page,
			XmNpageIncrement,	page,
			NULL);
	}
#endif
#ifdef wx_xview
  Scrollbar bar = ((orient == wxHORIZONTAL) ? horiz_scroll : vert_scroll);
  if (bar)
    xv_set (bar, SCROLLBAR_PAGE_LENGTH, page, NULL);
#endif
}

int wxCanvas::GetScrollPage(int orient)
{
#ifdef wx_motif
  Widget bar = ((orient == wxHORIZONTAL) ? hScrollBar : vScrollBar);
  int d;
  if (bar) {
    XtVaGetValues (bar, 
		   XmNsliderSize, &d,
		   NULL);
    return d;
  }
#endif
#ifdef wx_xview
  Scrollbar bar = ((orient == wxHORIZONTAL) ? horiz_scroll : vert_scroll);
  if (bar)
    return (int) xv_get (bar, SCROLLBAR_PAGE_LENGTH);
#endif  
  return 0;
}

void wxCanvas::CreatePixmap(int w, int h)
{
#ifdef wx_motif
  Widget drawingArea = (Widget) handle;
  backingPixmap =XCreatePixmap (XtDisplay (drawingArea),
	         RootWindowOfScreen (XtScreen (drawingArea)),w ,h ,
	         DefaultDepthOfScreen (XtScreen (drawingArea)));
#endif
}

void wxCanvas::FreePixmap(void)
{
#ifdef wx_motif
  XFreePixmap(XtDisplay((Widget) handle), backingPixmap);
  backingPixmap = 0;
#endif
}

#ifdef wx_xview
Notify_value wxCanvasScrollProc(Notify_client client, Event *event, Scrollbar sbar, Notify_event_type typ)
{
  if (event_id(event) == SCROLLBAR_REQUEST)
  {
    wxCanvas *canvas = (wxCanvas *)xv_get(sbar, WIN_CLIENT_DATA);
    if (!canvas)
      return NOTIFY_DONE;

    canvas->xviewClient = client;
    canvas->xviewEvent = event;
    canvas->xviewEventType = typ;
    canvas->xviewReturnValue = NOTIFY_DONE;

    // Is this the right value for the position of the scrollbar???
    int pos = (int)xv_get(sbar, SCROLLBAR_VIEW_START);

    // Call the virtual OnScroll member, which might call notify_next_event_func
    // if the default OnScroll member.
    wxCommandEvent event(wxEVENT_TYPE_SCROLL_THUMBTRACK);
    WXSCROLLPOS(event) = pos;
    WXSCROLLORIENT(event) = ((canvas->horiz_scroll == sbar) ? wxHORIZONTAL : wxVERTICAL);

    canvas->GetEventHandler()->OnScroll(event);
    
    canvas->xviewClient = 0;
    canvas->xviewEvent = NULL;
    canvas->xviewEventType = (Notify_event_type)0;

    return canvas->xviewReturnValue;
  }
  return NOTIFY_DONE;
}
#endif

/*
 * Update iterator. Use from within OnPaint.
 */
 
wxUpdateIterator::wxUpdateIterator(wxWindow* wnd)
{
  current = 0;					//start somewhere...
  win = wnd;
}

wxUpdateIterator::~wxUpdateIterator(void)
{
}

wxUpdateIterator::operator int (void)
{
  wxCanvas *can = (wxCanvas *)win;
  return (current < can->updateRects.Number()) ;
}

Bool wxUpdateIterator::HasRects(void)
{
  wxCanvas *can = (wxCanvas *)win;
  return (current < can->updateRects.Number()) ;
}

wxUpdateIterator* wxUpdateIterator::operator ++(int)
{
  current++;
  return this;
}

void wxUpdateIterator::GetRect(wxRectangle *rect)
{
  wxCanvas *can = (wxCanvas *)win;
  if (current < can->updateRects.Number())
  {
    wxRectangle *canRect = (wxRectangle *)can->updateRects.Nth(current)->Data();
    rect->x = canRect->x;
    rect->y = canRect->y;
    rect->width = canRect->width;
    rect->height = canRect->height;
  }
}

int wxUpdateIterator::GetX()
{
  wxCanvas *can = (wxCanvas *)win;
  if (current < can->updateRects.Number())
  {
    wxRectangle *canRect = (wxRectangle *)can->updateRects.Nth(current)->Data();
    return canRect->x;
  }
  else return 0;
}

int wxUpdateIterator::GetY()
{
  wxCanvas *can = (wxCanvas *)win;
  if (current < can->updateRects.Number())
  {
    wxRectangle *canRect = (wxRectangle *)can->updateRects.Nth(current)->Data();
    return canRect->y;
  }
  else return 0;
}

int wxUpdateIterator::GetW()
{
  wxCanvas *can = (wxCanvas *)win;
  if (current < can->updateRects.Number())
  {
    wxRectangle *canRect = (wxRectangle *)can->updateRects.Nth(current)->Data();
    return canRect->width;
  }
  else return 0;
}

int wxUpdateIterator::GetH()
{
  wxCanvas *can = (wxCanvas *)win;
  if (current < can->updateRects.Number())
  {
    wxRectangle *canRect = (wxRectangle *)can->updateRects.Nth(current)->Data();
    return canRect->height;
  }
  else return 0;
}
