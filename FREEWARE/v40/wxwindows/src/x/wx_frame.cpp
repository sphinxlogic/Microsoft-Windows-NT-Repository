/*
 * File:	wx_frame.cc
 * Purpose:	wxFrame implementation (X version)
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_frame.cc,v 1.3 1994/08/14 21:28:43 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "@(#)wx_frame.cc	1.2 5/9/94";

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx_defs.h"
#include "wx_frame.h"
#include "wx_dialg.h"
#include "wx_text.h"
#include "wx_main.h"
#include "wx_utils.h"
#include "wx_privt.h"
#include "wx_menu.h"

#include <stdlib.h>

#ifdef wx_motif
#if defined(__ultrix) || defined(__sgi)
#include <Xm/Frame.h>
#endif
#include <Xm/Xm.h>
#include <X11/Shell.h>
#if XmVersion >= 1002
#include <Xm/XmAll.h>
#else
#include <Xm/Frame.h>
#endif
#include <Xm/MwmUtil.h>
#include <Xm/BulletinB.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/AtomMgr.h>
#include <Xm/LabelG.h>
#include <Xm/Frame.h>
#if   XmVersion > 1000
#include <Xm/Protocols.h>
#endif
void wxCloseFrameCallback(Widget, XtPointer, XmAnyCallbackStruct *cbs);
#endif

#ifdef wx_xview
#include <xview/frame.h>
#include <xview/panel.h>
Notify_value wxFrameInterposer(Frame x_frame, Event *x_event, Notify_arg arg,
                               Notify_event_type type);
Notify_value wxDummyFrameInterposer(Frame x_frame, Event *x_event, Notify_arg arg,
                               Notify_event_type type);
Notify_value wxFrameCloseInterposer(Notify_client client, Destroy_status status);
#endif

#ifdef wx_motif
static Bool wxTopLevelUsed = FALSE;
static void wxFrameFocusProc(Widget workArea, XtPointer clientData, 
                      XmAnyCallbackStruct *cbs)
{
  wxFrame *frame = (wxFrame *)clientData;

  // wxDebugMsg("focus proc from frame %ld\n",(long)frame);
  frame->GetEventHandler()->OnSetFocus();
}

/* MATTEW: Used to insure that hide-&-show within an event cycle works */
static void wxFrameMapProc(Widget frameShell, XtPointer clientData, 
			   XCrossingEvent * event)
{
  wxFrame *frame = (wxFrame *)wxWidgetHashTable->Get((long)clientData);

  if (frame) {
    XEvent *e = (XEvent *)event;

    if (e->xany.type == MapNotify)
    {
      // Iconize fix
      XtVaSetValues(frameShell, XmNiconic, (Boolean)False, NULL);
      if (!frame->visibleStatus)
      {
	/* We really wanted this to be hidden! */
	XtUnmapWidget(frame->frameShell);
      }
    }
    else if (e->xany.type == UnmapNotify)
      // Iconize fix
      XtVaSetValues(frameShell, XmNiconic, (Boolean)True, NULL);
  }
}

#endif

#ifdef wx_xview
/* MATTEW: Used to insure that hide-&-show within an event cycle works */
void wxFrameEventProc (Frame x_frame, Event * event, Notify_arg arg)
{
  wxFrame *frame = (wxFrame *)xv_get(x_frame, WIN_CLIENT_DATA);
  if (frame && event_id(event) == WIN_MAP_NOTIFY)
    {
      if (!frame->visibleStatus) {
	/* We really wanted this to be hidden! */
	Xv_opaque window = (Xv_opaque)frame->handle;
	/* Re-show to set the XView internal "shown" flag: */
	xv_set(window, XV_SHOW, TRUE, NULL);
	/* Do the real hide */
	xv_set(window, XV_SHOW, FALSE, NULL);
      }
    }
}
#endif

wxFrame::wxFrame(void)
{
  wx_menu_bar = NULL;
  status_line_exists = FALSE;
  icon = NULL;
  modal_showing = FALSE;
  window_parent = NULL;
  frameTitle = NULL;
  lastWidth = -1 ;
  lastHeight = -1 ;
  visibleStatus = FALSE;
#ifdef wx_motif
  int i;
  for (i = 0; i < wxMAX_STATUS; i++)
    statusTextWidget[i] = 0;
  statusLineForm = 0;
  frameShell = NULL;
  menuBarWidget = NULL;
  statusLineWidget = NULL;
  frameWidget = NULL;
  workArea = NULL;
  clientArea = NULL;
  handle = NULL;
  isShown = FALSE;
#endif
#ifdef wx_xview
  handle = NULL;
  menu_bar_panel = NULL;
  isPinned = FALSE;
  y_offset = 0;
  int i;
  for (i = 0; i < wxMAX_STATUS; i++)
    statusText[i] = 0;
#endif
}

wxFrame::wxFrame(wxFrame *Parent, char *title, int x, int y,
                 int width, int height, long style, char *name):
  wxbFrame(Parent, title, x, y, width, height, style, name)
{
  Create(Parent, title, x, y, width, height, style, name);
}

Bool wxFrame::Create(wxFrame *Parent, char *title, int x, int y,
                 int width, int height, long style, char *name)
{
  frameTitle = NULL;

  wxbFrame::Create(Parent, title, x, y, width, height, style, name);

  SetName(name);
  if (title)
    frameTitle = copystring(title);

  windowStyle = style;
  wx_menu_bar = NULL;
  status_line_exists = FALSE;
  icon = NULL;
  modal_showing = FALSE;
  if (Parent) Parent->AddChild(this);
  window_parent = Parent;
  lastWidth = -1 ;
  lastHeight = -1 ;
#ifdef wx_motif
  isShown = FALSE;
  statusLineForm = 0;
  statusLineWidget = 0;
  int i;
  for (i = 0; i < wxMAX_STATUS; i++)
    statusTextWidget[i] = 0;

  if (wxTopLevelUsed)
    // Change suggested by Matthew Flatt
    frameShell = XtAppCreateShell(windowName, wxTheApp->wx_class, topLevelShellWidgetClass, wxGetDisplay(), NULL, 0);
  else
  {
    frameShell = wxTheApp->topLevel;
    wxTopLevelUsed = TRUE;
  }

  XtVaSetValues(frameShell, 
                 // Allows menu to resize
                 XmNallowShellResize, True,
                 XmNdeleteResponse, XmDO_NOTHING,
                 XmNmappedWhenManaged, False,
                 XmNiconic, (style & wxICONIZE) ? TRUE : FALSE,
                 NULL);

  if (title)
    XtVaSetValues(frameShell, 
                  XmNtitle, title,
                  NULL);

  menuBarWidget = NULL;
  statusLineWidget = NULL;

  frameWidget = XtVaCreateManagedWidget("main_window",
                    xmMainWindowWidgetClass, frameShell,
                    XmNresizePolicy, XmRESIZE_NONE,
                    NULL);

  workArea = XtVaCreateWidget("form",
                    xmFormWidgetClass, frameWidget,
                    XmNresizePolicy, XmRESIZE_NONE,
                    NULL);

  clientArea = XtVaCreateWidget("client",
                    xmBulletinBoardWidgetClass, workArea,
                    XmNmarginWidth, 0,
                    XmNmarginHeight, 0,
                    XmNrightAttachment, XmATTACH_FORM,
                    XmNleftAttachment, XmATTACH_FORM,
                    XmNtopAttachment, XmATTACH_FORM,
                    XmNbottomAttachment, XmATTACH_FORM,
//                    XmNresizePolicy, XmRESIZE_ANY,
                    NULL);

  XtVaSetValues(frameWidget,
    XmNworkWindow, workArea,
    NULL);


  XtManageChild(clientArea);
//  XtManageChild(statusLineWidget);
  XtManageChild(workArea);

  if (wxWidgetHashTable->Get((long)workArea))
  {
    wxError("Widget table clash in wx_frame.cc");
  }
  wxAddWindowToTable(workArea, this);

  XtTranslations ptr ;

  XtOverrideTranslations(workArea,
              ptr = XtParseTranslationTable("<Configure>: resize()"));

  XtFree((char *)ptr);

  XtAddCallback(workArea, XmNfocusCallback, 
                (XtCallbackProc)wxFrameFocusProc, (XtPointer)this);

  /* MATTHEW: part of show-&-hide fix */
  XtAddEventHandler(frameShell, StructureNotifyMask,
		    False, (XtEventHandler)wxFrameMapProc,
		    (XtPointer)workArea);

  if (x > -1)
    XtVaSetValues(frameShell, XmNx, x, NULL);
  if (y > -1)
    XtVaSetValues(frameShell, XmNy, y, NULL);
  if (width > -1)
    XtVaSetValues(frameShell, XmNwidth, width, NULL);
  if (height > -1)
    XtVaSetValues(frameShell, XmNheight, height, NULL);

  handle = (char *)frameWidget;

  // This patch comes from Torsten Liermann lier@lier1.muc.de
  if (XmIsMotifWMRunning(wxTheApp->topLevel))
  {
    int decor = 0 ;
    if (style & wxRESIZE_BORDER)
      decor |= MWM_DECOR_RESIZEH ;
    if (style & wxSYSTEM_MENU)
      decor |= MWM_DECOR_MENU;
    if ((style & wxCAPTION) ||
        (style & wxTINY_CAPTION_HORIZ) ||
        (style & wxTINY_CAPTION_VERT))
      decor |= MWM_DECOR_TITLE;
    if (style & wxTHICK_FRAME)
      decor |= MWM_DECOR_BORDER;
    if (style & wxTHICK_FRAME)
      decor |= MWM_DECOR_BORDER;
    if (style & wxMINIMIZE_BOX)
      decor |= MWM_DECOR_MINIMIZE;
    if (style & wxMAXIMIZE_BOX)
      decor |= MWM_DECOR_MAXIMIZE;
    XtVaSetValues(frameShell,XmNmwmDecorations,decor,NULL) ;
  }
  // This allows non-Motif window managers to support at least the
  // no-decorations case.
  else
  {
    if (style == 0)
      XtVaSetValues(frameShell,XmNoverrideRedirect,TRUE,NULL);
  }
  XtRealizeWidget(frameShell);

  // Intercept CLOSE messages from the window manager
  Atom WM_DELETE_WINDOW = XmInternAtom(XtDisplay(frameShell), "WM_DELETE_WINDOW", False);
#if (XmREVISION > 1 || XmVERSION > 1)
  XmAddWMProtocolCallback(frameShell, WM_DELETE_WINDOW, (XtCallbackProc) wxCloseFrameCallback, (XtPointer)this);
#else
#if XmREVISION == 1
  XmAddWMProtocolCallback(frameShell, WM_DELETE_WINDOW, (XtCallbackProc) wxCloseFrameCallback, (caddr_t)this);
#else
  XmAddWMProtocolCallback(frameShell, WM_DELETE_WINDOW, (void (*)())wxCloseFrameCallback, (caddr_t)this);
#endif
#endif

  PreResize();
  GetEventHandler()->OnSize(width, height);
#endif
#ifdef wx_xview
  isPinned = FALSE;
  int i;
  for (i=0;i<wxMAX_STATUS;i++)
    statusText[i] = NULL ;

  Frame parent = (Parent == NULL) ? (Frame)NULL : (Frame)(Parent->GetHandle());

  Xv_pkg *frameStyle;
  if (style & wxPUSH_PIN)
    frameStyle = FRAME_CMD;
  else
    frameStyle = FRAME;

//  Frame frame = (Frame) xv_create(parent,
  // Using a NULL parent has a positive effect
  // on wxFrameCloseInterposer calling! but doesn't seem to
  // have any bad side effect.
  Frame frame = (Frame) xv_create(NULL,
                              frameStyle,
                              FRAME_LABEL, title,
                              WIN_CLIENT_DATA, (char *)this,
			      /* MATTHEW: hide-&-show sequence */
			      WIN_EVENT_PROC, wxFrameEventProc,
                              FRAME_SHOW_LABEL,
                                 (style & (wxCAPTION|wxTINY_CAPTION_HORIZ|wxTINY_CAPTION_VERT)) ? TRUE : FALSE,
                              XV_SHOW, FALSE,
                              FRAME_CLOSED, (style & wxICONIZE) ? TRUE : FALSE,
                              NULL);

  if (frameStyle == FRAME_CMD)
     xv_set(frame, FRAME_SHOW_RESIZE_CORNER, (style & wxRESIZE_BOX) ? TRUE : FALSE, NULL);

  if (x > -1) xv_set(frame, XV_X, x, NULL);

  if (y > -1) xv_set(frame, XV_Y, y, NULL);

  if (width > -1) xv_set(frame, XV_WIDTH, width, NULL);

  if (height > -1) xv_set(frame, XV_HEIGHT, height, NULL);

  handle = (char *)frame;

  // edz: @@@@ Set default Icon
//  SetIcon(new wxIcon (wxwin_xbm_bits, wxwin_xbm_width, wxwin_xbm_height));

  menu_bar_panel = NULL;
  y_offset = 0;

  // Have to do this interposition to receive frame resize events
  (void)notify_interpose_event_func(frame, (Notify_func)wxFrameInterposer, NOTIFY_SAFE);
  (void)notify_interpose_destroy_func(frame, (Notify_func)wxFrameCloseInterposer);
#endif
  return TRUE;
}

#ifdef wx_motif
Bool wxFrame::PreResize(void)
{
  // Set status line, if any
  if (status_line_exists)
  {
    Dimension clientW, clientH;
    XtVaGetValues(clientArea, XmNwidth, &clientW, XmNheight, &clientH, NULL);
    Dimension xx, yy;
    XtVaGetValues(statusLineWidget, XmNwidth, &xx, XmNheight, &yy, NULL);

    XtUnmanageChild(statusLineWidget);
    XtVaSetValues(statusLineWidget, XmNx, 0, XmNy, clientH - yy, XmNwidth, clientW, NULL);

    if (statusLineForm)
      XtVaSetValues(statusLineForm,  XmNwidth, clientW, NULL);

    XtManageChild(statusLineWidget);
  }

  int width, height;
  GetSize(&width, &height);

  if (width == lastWidth && height == lastHeight)
    return FALSE;
  else
  {
    return TRUE;
  }
}
#endif

// Get size *available for subwindows* i.e. excluding menu bar etc.
// For XView, this is the same as GetSize
void wxFrame::GetClientSize(int *x, int *y)
{
#ifdef wx_motif
  Dimension xx, yy;
  XtVaGetValues(workArea, XmNwidth, &xx, XmNheight, &yy, NULL);

  if (status_line_exists)
  {
    Dimension ys;
    XtVaGetValues(statusLineWidget, XmNheight, &ys, NULL);
    yy -= ys;
  }

  if (wx_menu_bar != NULL)
  {
    // it seems that if a frame holds a panel, the menu bar size
    // gets automatically taken care of --- grano@cs.helsinki.fi 4.4.95
    Bool hasSubPanel = FALSE;
    for(wxNode* node = GetChildren()->First(); node; node = node->Next())
    {
     wxWindow *win = (wxWindow *)node->Data();
     hasSubPanel = ((win->IsKindOf(CLASSINFO(wxPanel)) && !win->IsKindOf(CLASSINFO(wxDialogBox))) ||
                     win->IsKindOf(CLASSINFO(wxCanvas)) ||
                     win->IsKindOf(CLASSINFO(wxTextWindow)));

      if (hasSubPanel)
          break;
    }
    if (! hasSubPanel) {
      Dimension ys;
      XtVaGetValues(GetMenuBarWidget(), XmNheight, &ys, NULL);
      yy -= ys;
    }
  }

  *x = xx; *y = yy;
#endif
#ifdef wx_xview
  Frame x_frame = (Frame)handle;

  *x = (int)xv_get(x_frame, XV_WIDTH);
  *y = (int)xv_get(x_frame, XV_HEIGHT) - y_offset;
#endif
}

// Set the client size (i.e. leave the calculation of borders etc.
// to wxWindows)
void wxFrame::SetClientSize(int width, int height)
{
#ifdef wx_motif
  // Calculate how large the new main window should be
  // by finding the difference between the client area and the
  // main window area, and adding on to the new client area
/*
  Dimension current_frame_width, current_frame_height;
  Dimension current_form_width, current_form_height;
  XtVaGetValues(frameShell, XmNwidth, &current_frame_width, XmNheight, &current_frame_height, NULL);
  XtVaGetValues(workArea, XmNwidth, &current_form_width, XmNheight, &current_form_height, NULL);
  int diffX = current_frame_width - current_form_width;
  int diffY = current_frame_height - current_form_height;

  if (width > -1)
    XtVaSetValues(frameShell, XmNwidth, width + diffX, NULL);
  if (height > -1)
  {
    int real_height = height + diffY;
    if (status_line_exists)
    {
      Dimension ys;
      XtVaGetValues(statusLineWidget, XmNheight, &ys, NULL);
      real_height += ys;
    }
    XtVaSetValues(frameShell, XmNheight, real_height, NULL);
  }
*/
  if (width > -1)
    XtVaSetValues(workArea, XmNwidth, width, NULL);

  if (height > -1)
  {
    if (status_line_exists)
    {
      Dimension ys;
      XtVaGetValues(statusLineWidget, XmNheight, &ys, NULL);
      height += ys;
    }
    XtVaSetValues(workArea, XmNheight, height, NULL);
  }
  PreResize();
#endif
#ifdef wx_xview
  Frame x_frame = (Frame)handle;

  if (width > -1 && height > -1)
    (void)xv_set(x_frame, XV_WIDTH, width, XV_HEIGHT, height + y_offset, NULL);

#endif
  GetSize(&lastWidth, &lastHeight);
  GetEventHandler()->OnSize(lastWidth, lastHeight);
}

void wxFrame::GetSize(int *width, int *height)
{
#ifdef wx_motif
  Dimension xx, yy;
  XtVaGetValues(frameShell, XmNwidth, &xx, XmNheight, &yy, NULL);
  *width = xx; *height = yy;
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque)handle;

  *width = (int)xv_get(x_win, XV_WIDTH);
  *height = (int)xv_get(x_win, XV_HEIGHT);
#endif
}

void wxFrame::GetPosition(int *x, int *y)
{
#ifdef wx_motif
    Window parent_window = XtWindow(frameShell),
	   next_parent   = XtWindow(frameShell),
	   root          = RootWindowOfScreen(XtScreen(frameShell));
    // search for the parent that is child of ROOT, because the WM may
    // reparent twice and notify only the next parent (like FVWM)
    while (next_parent != root) {
	Window *theChildren; unsigned int n;
	parent_window = next_parent;
	XQueryTree(XtDisplay(frameShell), parent_window, &root,
		   &next_parent, &theChildren, &n);
	XFree(theChildren); // not needed
    }
    int xx, yy; unsigned int dummy;
    XGetGeometry(XtDisplay(frameShell), parent_window, &root,
		 &xx, &yy, &dummy, &dummy, &dummy, &dummy);
    if (x) *x = xx;
    if (y) *y = yy;
/*
  Position xx, yy;
  XtVaGetValues(frameShell, XmNx, &xx, XmNy, &yy, NULL);
  *x = xx; *y = yy;
*/
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque)handle;

  *x = (int)xv_get(x_win, XV_X);
  *y = (int)xv_get(x_win, XV_Y);
#endif
}

void wxFrame::SetSize(int x, int y, int width, int height, int sizeFlags)
{
#ifdef wx_motif
  if (x > -1)
    XtVaSetValues(frameShell, XmNx, x, NULL);
  if (y > -1)
    XtVaSetValues(frameShell, XmNy, y, NULL);
  if (width > -1)
    XtVaSetValues(frameWidget, XmNwidth, width, NULL);
  if (height > -1)
    XtVaSetValues(frameWidget, XmNheight, height, NULL);

  if (!(height == -1 && width == -1))
  {
    PreResize();
    GetEventHandler()->OnSize(width, height);
  }
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque)handle;

  if (x == -1 || y == -1)
  {
    int xx,yy ;
    GetPosition(&xx, &yy);
    if (x == -1) x = xx ;
    if (y == -1) y = yy ;
  }

  if (width == -1 && height == -1)
  {
    (void)xv_set(x_win, XV_X, x, XV_Y, y, NULL);
  }
  else
  {
    int ww,hh ;
    GetSize(&ww, &hh);
    if (width == -1) width = ww ;
    if (height == -1) height = hh ;

    (void)xv_set(x_win, XV_X, x, XV_Y, y, XV_WIDTH, width, XV_HEIGHT, height, NULL);
    GetEventHandler()->OnSize(width, height);
  }
#endif
  GetSize(&lastWidth, &lastHeight);
}

Bool wxFrame::Show(Bool show)
{
  visibleStatus = show; /* MATTHEW: show-&-hide fix */
#if WXGARBAGE_COLLECTION_ON
  if (!window_parent) {
    if (show) {
      if (!wxTopLevelWindows.Member(this))
	wxTopLevelWindows.Append(this);
    } else 
      wxTopLevelWindows.DeleteObject(this);
  }
#endif
#ifdef wx_motif
  isShown = show;
  if (show) {
    XtMapWidget(frameShell);
    XRaiseWindow(XtDisplay(frameShell), XtWindow(frameShell));
  } else {
    XtUnmapWidget(frameShell);
//    XmUpdateDisplay(wxTheApp->topLevel); // Experimental: may be responsible for crashes
  }
#endif
#ifdef wx_xview
  Xv_opaque window = (Xv_opaque)handle;
  xv_set(window, XV_SHOW, show, NULL);
#endif
  return TRUE;
}

#ifdef wx_xview
Notify_value wxDummyFrameInterposer(Frame x_frame, Event *x_event, Notify_arg arg,
                               Notify_event_type type)
{
  return (Notify_value)1;
}

Notify_value wxFrameInterposer(Frame x_frame, Event *x_event, Notify_arg arg,
                               Notify_event_type type)
{
  wxFrame *frame = (wxFrame *)xv_get(x_frame, WIN_CLIENT_DATA);

//  cout << event_action(x_event) << "\n";

  if (frame && event_action(x_event) == WIN_RESIZE)
    {
//      cout << "Frame resized\n";
        int width, height;
        frame->GetSize(&width, &height);
        if (!(width == frame->lastWidth && height == frame->lastHeight))
	{
          frame->lastWidth = width; frame->lastHeight = height;
          frame->GetEventHandler()->OnSize(width, height);
	}

       return notify_next_event_func(x_frame, (long unsigned int)x_event, arg, type);
    }
  else if (frame && event_action(x_event) == ACTION_PINOUT)
    {
//    cout << "Pin out - frame " << frame << endl;
      if (frame->isPinned) {
        if (frame->OnClose() == FALSE) {
          (void)xv_set(x_frame, FRAME_CMD_PUSHPIN_IN, TRUE, NULL) ;
        } else {
          frame->isPinned = FALSE;
          frame->Show(FALSE);
        }
      }
    }
  else if (frame && event_action(x_event) == ACTION_PININ)
    {
//    cout << "Pin in - frame " << frame << endl;
      if (!frame->isPinned) {
        frame->isPinned = TRUE;
      }
    }

  return notify_next_event_func(x_frame, (long unsigned int)x_event, arg, type);
//  else return notify_next_event_func(x_frame, (long unsigned int)x_event, arg, type);
}

Notify_value
wxFrameCloseInterposer(Notify_client client, Destroy_status status)
{
  wxFrame *frame = (wxFrame *)xv_get(client, WIN_CLIENT_DATA);

  if (status == DESTROY_CHECKING) {
    if (frame && frame->GetEventHandler()->OnClose() == FALSE)
        notify_veto_destroy(client);
  } else if (status == DESTROY_CLEANUP) {
    // Should we destroy the wxFrame or wot?
    // Try to delete the wxFrame without allowing the Frame
    // to be deleted, since this will be done by XView

    if (frame) {
      frame->Show(FALSE);
      (void)notify_interpose_event_func((Frame)frame->handle, (Notify_func)wxDummyFrameInterposer, NOTIFY_SAFE);
      xv_set((Frame)frame->handle, WIN_CLIENT_DATA, NULL, NULL);

      frame->handle = NULL;
      frame->DestroyChildren();

      if (frame == wxTheApp->wx_frame)
        wxTheApp->wx_frame = NULL;

      delete frame;
    }
    return notify_next_destroy_func(client, status);
  } else if (status == DESTROY_SAVE_YOURSELF) {
    // Do nothing - this is an Open Look specific feature
  } else  if (status == DESTROY_PROCESS_DEATH) { 
    if (!wxTheApp->death_processed) {
      wxTheApp->OnExit();
      wxTheApp->death_processed = TRUE;
    }
  };
  return NOTIFY_DONE;
}

#endif

#ifdef wx_motif
void wxCloseFrameCallback(Widget widget, XtPointer client_data, XmAnyCallbackStruct *cbs)
{
  wxFrame *frame = (wxFrame *)client_data;
 /* MATTHEW: [8] GC */
  if (frame->GetEventHandler()->OnClose())
  {
#if !WXGARBAGE_COLLECTION_ON
    delete frame;
#else
    frame->Show(FALSE);
#endif
  }
}
#endif

void wxFrame::Iconize(Bool iconize)
{
  if (!iconize)
    Show(TRUE);
#ifdef wx_motif
  XtVaSetValues(frameShell, XmNiconic, (Boolean)iconize, NULL);
#endif
#ifdef wx_xview
  Xv_opaque window = (Xv_opaque)handle;
  xv_set(window, FRAME_CLOSED, iconize, NULL);
#endif
}

// Equivalent to maximize/restore in Windows
void wxFrame::Maximize(Bool maximize)
{
  Show(TRUE);
#ifdef wx_motif
  if (maximize)
    XtVaSetValues(frameShell, XmNiconic, FALSE, NULL);
#endif
#ifdef wx_xview
  Xv_opaque window = (Xv_opaque)handle;
  if (maximize)
    xv_set(window, FRAME_CLOSED, FALSE, NULL);
#endif
}

Bool wxFrame::Iconized(void)
{
#ifdef wx_motif
  Boolean iconic;
  XtVaGetValues(frameShell, XmNiconic, &iconic, NULL);
  return iconic;
#endif
#ifdef wx_xview
  Xv_opaque window = (Xv_opaque)handle;
  return xv_get(window, FRAME_CLOSED);
#endif
}


void wxFrame::SetTitle(char *title)
{
  if (title == frameTitle)
    return;
    
  if (frameTitle)
    delete[] frameTitle;

  frameTitle = copystring(title);

#ifdef wx_motif
  if (title)
    XtVaSetValues(frameShell, 
                  XmNtitle, title,
                  XmNiconName, title,
                  NULL);
#endif
#ifdef wx_xview
  Frame xframe = (Frame)handle;
  xv_set(xframe, FRAME_LABEL, title, NULL);
  if (icon)
  {
    xv_set(icon->x_icon, XV_LABEL, title, NULL);
    xv_set(xframe, FRAME_ICON, icon->x_icon, NULL);
  }
#endif
}

char *wxFrame::GetTitle(void)
{
  return frameTitle;
}


void wxFrame::SetIcon(wxIcon *wx_icon)
{
/*
  if (icon)
    delete icon;
*/
  icon = wx_icon;
#ifdef wx_motif
  if (!wx_icon->x_pixmap)
    return;

  XtVaSetValues(frameShell, XtNiconPixmap, wx_icon->x_pixmap, NULL);

  // The following isn't necessary and doesn't even work (P.587 of Heller)
/*
  Display *dpy = XtDisplay(wxTheApp->topLevel);
  Window window, root;
  XtVaGetValues(frameShell, XtNiconWindow, &window, NULL);
  if (!window)
  {
    int x, y;
    unsigned int width, height, border_width, depth;
    if (!XGetGeometry(dpy, wx_icon->x_pixmap, &root, &x, &y, &width, &height, &border_width,
                      &depth) ||
        !(window = XCreateSimpleWindow(dpy, root, 0, 0, width, height, (unsigned)0, 
          CopyFromParent, CopyFromParent)))
    {
      XtVaSetValues(frameShell, XmNiconPixmap, wx_icon->x_pixmap, NULL);
      return;
    }
    XSetWindowBackgroundPixmap(dpy, window, wx_icon->x_pixmap);
    XClearWindow(dpy, window);
  }
*/
#endif
#ifdef wx_xview
  Frame xframe = (Frame)handle;
  char *label = (char *)xv_get(xframe, FRAME_LABEL);
  if (wx_icon->x_icon)
  {
    xv_set(wx_icon->x_icon, XV_LABEL, label, NULL);
    xv_set(xframe, FRAME_ICON, wx_icon->x_icon, NULL);
  }
#endif
}


void wxFrame::CreateStatusLine(int number, Constdata char *name)
{
  if (status_line_exists)
    return;

  nb_status = number;
  status_line_exists = TRUE;

#ifdef wx_motif
  int i;
  statusLineWidget = XtVaCreateManagedWidget(name,
      xmFrameWidgetClass,      clientArea,
      XmNshadowType,           XmSHADOW_IN,
      NULL);

  if (number == 1)
  {
    sprintf(wxBuffer,"status_text%d",0) ;
    statusTextWidget[0] = XtVaCreateManagedWidget(wxBuffer,
#if USE_GADGETS
        xmLabelGadgetClass,      statusLineWidget,
#else
        xmLabelWidgetClass,      statusLineWidget,
#endif
        XmNalignment,            XmALIGNMENT_BEGINNING,
        NULL);
  }
  else
  {
    statusLineForm = XtVaCreateManagedWidget("form",
                xmFormWidgetClass, statusLineWidget,NULL) ;
    for (i=0;i<number;i++)
    {
      sprintf(wxBuffer,"status_text%d",i) ;
      statusTextWidget[i] = XtVaCreateManagedWidget(wxBuffer,
#if USE_GADGETS
          xmLabelGadgetClass,      statusLineForm,
#else
          xmLabelWidgetClass,      statusLineForm,
#endif
          XmNalignment,            XmALIGNMENT_BEGINNING,
          NULL);
    }
    Widget attach_me = NULL  ;
    for (i=0;i<number/2;i++)
    {
      XtVaSetValues(statusTextWidget[i],
          XmNleftAttachment ,  i==0? XmATTACH_FORM:XmATTACH_WIDGET,
          XmNleftWidget ,      attach_me ,
          XmNrightAttachment , XmATTACH_NONE ,
          NULL) ;
      attach_me = statusTextWidget[i] ;
    }
    for (i=number-1;i>=number/2;i--)
    {
      XtVaSetValues(statusTextWidget[i],
          XmNrightAttachment ,i==number-1? XmATTACH_FORM : XmATTACH_WIDGET,
          XmNrightWidget ,    attach_me ,
          XmNleftAttachment , XmATTACH_NONE ,
          NULL) ;
      attach_me = statusTextWidget[i] ;
    }
  }

  XtRealizeWidget(statusLineWidget);
//  XtRealizeWidget(statusTextWidget);
  if (statusLineForm)
    XtRealizeWidget(statusLineForm);
  for(i=0;i<number;i++)
    XtRealizeWidget(statusTextWidget[i]);

  for (i=0;i<number;i++)
    SetStatusText(" ",i);

/*
  // Initialize it to set it to the desired height
  XmString str = XmStringCreateSimple(" ");
  XtVaSetValues(statusTextWidget,
                  XmNrecomputeSize, True,
                  XmNlabelString, str,
                  NULL);
  XmStringFree(str);
*/

  PreResize();
#endif
#ifdef wx_xview
  Frame xframe = (Frame)handle;
  xv_set(xframe, FRAME_SHOW_FOOTER, TRUE, NULL);
#endif
}

void wxFrame::SetStatusText(char *text, int number)
{
  if (!status_line_exists)
    return;
#ifdef wx_motif
  if (!text) text = " ";
  sprintf(wxBuffer, " %s", text);

  // First check if the string is the same as what's there.
  XmString textString;
  char *s = NULL;
  XtVaGetValues(statusTextWidget[number],
                  XmNlabelString, &textString,
                  NULL);

  if (XmStringGetLtoR(textString, XmSTRING_DEFAULT_CHARSET, &s))
  {
    Bool theSame = (strcmp(s, wxBuffer) == 0);
    XtFree(s);
    XmStringFree(textString) ;
    if (theSame)
      return;
  }
  else
  {
    if (s) XtFree(s);
    XmStringFree(textString) ;
  }

  // Not the same, so carry on

  XmString str = XmStringCreateSimple(wxBuffer);
  XtVaSetValues(statusTextWidget[number],
//                  XmNrecomputeSize, False,
                  XmNlabelString, str,
                  NULL);
  XmStringFree(str);
  PreResize();  // Stretch it back to full width!
#endif
#ifdef wx_xview
  Frame xframe = (Frame)handle;
//  xv_set(xframe, FRAME_LEFT_FOOTER, text, NULL);
  if (statusText[number])
    delete statusText[number] ;
  statusText[number] = copystring(text) ;

  char tmp[256] ;
  tmp[0]='\0' ;
  if ((float)number < ((float)nb_status/2.0))
  {
    int i;
    for (i=0;((float)i) < ((float)(nb_status/2.0));i++)
    {
      if (statusText[i])
        strcat(tmp,statusText[i]) ;
      strcat(tmp," ") ;
    }
    xv_set(xframe, FRAME_LEFT_FOOTER, tmp , NULL);
  }
  else
  {
    int i;
    for (i=nb_status/2;i<nb_status;i++)
    {
      if (statusText[i])
        strcat(tmp,statusText[i]) ;
      strcat(tmp," ") ;
    }
    xv_set(xframe, FRAME_RIGHT_FOOTER, tmp, NULL);
  }
#endif
}

void wxFrame::LoadAccelerators(char *table)
{
}

void wxFrame::Fit(void)
{
  int maxX = 0;
  int maxY = 0;
  for(wxNode *node = GetChildren()->First(); node; node = node->Next())
  {
    wxWindow *win = (wxWindow *)node->Data();
    if ((win->IsKindOf(CLASSINFO(wxPanel)) &&
         !win->IsKindOf(CLASSINFO(wxDialogBox))) ||
        win->IsKindOf(CLASSINFO(wxTextWindow)) ||
        win->IsKindOf(CLASSINFO(wxCanvas)))
    {
      int x, y, w, h;
      win->GetPosition(&x, &y);
      win->GetSize(&w, &h);
      if ((x + w) > maxX)
        maxX = x + w;
      if ((y + h) > maxY)
        maxY = y + h;
    }
  }
  SetClientSize(maxX, maxY);
}

#ifdef wx_motif
Widget wxFrame::GetMenuBarWidget (void)
{
  return (Widget) wx_menu_bar->handle;
}
#endif

void wxFrame::SetMenuBar (wxMenuBar * menu_bar)
{
  /* MATTHEW: [6] Protect against resetting the menu bar or using 
     the same bar twice */
  if (wx_menu_bar || menu_bar->menu_bar_frame)
    return;

#ifdef wx_motif
  Widget MenuBar = XmCreateMenuBar (frameWidget, "MenuBar", NULL, 0);
  menu_bar->handle = (char *) MenuBar;

  int i;
  for (i = 0; i < menu_bar->n; i++)
    {
      wxMenu *menu = menu_bar->menus[i];
      menu->buttonWidget = menu->CreateMenu (menu_bar, MenuBar, menu, menu_bar->titles[i], TRUE);

      /*
       * COMMENT THIS OUT IF YOU DON'T LIKE A RIGHT-JUSTIFIED HELP MENU
       */
      wxStripMenuCodes (menu_bar->titles[i], wxBuffer);

      if (strcmp (wxBuffer, wxSTR_MENU_HELP) == 0)
	XtVaSetValues (MenuBar, XmNmenuHelpWidget, menu->buttonWidget, NULL);
    }

  XtRealizeWidget (MenuBar);
  XtManageChild (MenuBar);
#endif
#ifdef wx_xview
  y_offset = 0;
  menu_bar_panel = new wxPanel (this, 0, 0, -1, MENU_BAR_PANEL_HEIGHT - 1);
  children->DeleteObject (menu_bar_panel);
  y_offset = MENU_BAR_PANEL_HEIGHT;

  int i;
  for (i = 0; i < menu_bar->n; i++)
    {
      wxStripMenuCodes (menu_bar->titles[i], wxBuffer);
      Menu x_menu = (Menu) (menu_bar->menus[i]->handle);
//    (void) xv_create((Panel)(menu_bar_panel->handle), PANEL_BUTTON,
      menu_bar->menus[i]->panelItem =
	(void *) xv_create ((Panel) (menu_bar_panel->handle), PANEL_BUTTON,
			    PANEL_LABEL_STRING, wxBuffer,
			    PANEL_ITEM_MENU, x_menu,
			    NULL);
    }
#endif
  wx_menu_bar = menu_bar;
  menu_bar->menu_bar_frame = this;
}

wxFrame::~wxFrame (void)
{
  if (handle)
    Show (FALSE);

  if (wx_menu_bar)
  {
// Hack to stop core dump on Ultrix, OSF, for some strange reason.
#if MOTIF_MENUBAR_DELETE_FIX
    wx_menu_bar->handle = 0;
#endif
    delete wx_menu_bar;
  }
#if !WXGARBAGE_COLLECTION_ON
  if (icon)
    delete icon;
#endif

  if (frameTitle)
    delete[]frameTitle;

#ifdef wx_motif
  DestroyChildren();

  int i;
  for (i = 0; i < wxMAX_STATUS; i++)
    if (statusTextWidget[i])
      XtDestroyWidget (statusTextWidget[i]);

  if (statusLineForm)
    XtDestroyWidget (statusLineForm);

  if (statusLineWidget)
    XtDestroyWidget (statusLineWidget);

  wxDeleteWindowFromTable(workArea);

  XtDestroyWidget (workArea);
  XtDestroyWidget (frameWidget);

  wxDeleteWindowFromTable(frameWidget);

  XtDestroyWidget (frameShell);

  handle = NULL;

  if (this == wxTheApp->wx_frame)
    {
      int retValue = wxTheApp->OnExit ();
      wxCleanUp ();
      exit (retValue);
    }
#endif
#ifdef wx_xview
  int i;
  for (i = 0; i < wxMAX_STATUS; i++)
    if (statusText[i])
      delete statusText[i];

  if (handle)
    {
      (void) notify_interpose_event_func ((Frame) handle, (Notify_func) wxDummyFrameInterposer, NOTIFY_SAFE);
    }
#endif
}

void wxFrame::CaptureMouse(void)
{
  if (winCaptured)
    return;
    
#ifdef wx_motif
  if (handle)
    XtAddGrab(frameShell, TRUE, FALSE);
#endif
#ifdef wx_xview
  if (handle)
  {
    Xv_opaque win = (Xv_opaque)handle;
    xv_set(win, WIN_GRAB_ALL_INPUT, TRUE, NULL);
  }
#endif
  winCaptured = TRUE;
}

void wxFrame::ReleaseMouse(void)
{
  if (!winCaptured)
    return;
    
#ifdef wx_motif
  if (handle)
    XtRemoveGrab(frameShell);
#endif
#ifdef wx_xview
  if (handle)
  {
    Xv_opaque win = (Xv_opaque)handle;
    xv_set(win, WIN_GRAB_ALL_INPUT, FALSE, NULL);
  }
#endif
  winCaptured = FALSE;
}

void wxFrame::SetStatusWidths(int WXUNUSED(n), int *WXUNUSED(widths_field))
{
}