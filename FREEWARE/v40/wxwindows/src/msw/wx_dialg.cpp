/*
 * File:	wx_dialg.cc
 * Purpose:	wxDialogBox and miscellaneous dialog functions
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_dialg.cc,v 1.1 1994/08/14 21:59:17 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_dialg.h"
#include "wx_utils.h"
#include "wx_frame.h"
#include "wx_main.h"
#endif

#include "wx_privt.h"

#if USE_COMMON_DIALOGS
#include <commdlg.h>
#endif

#if CTL3D
#include <ctl3d.h>
#endif

#if FAFA_LIB
#include "fafa.h"
#endif

#if !defined(APIENTRY)	// NT defines APIENTRY, 3.x not
#define APIENTRY far pascal
#endif
 
#ifdef WIN32
#define _EXPORT /**/
#else
#define _EXPORT _export
typedef signed short int SHORT ;
#endif
 
#if !defined(WIN32)	// 3.x uses FARPROC for dialogs
#define DLGPROC FARPROC
#endif

#define wxDIALOG_DEFAULT_X 300
#define wxDIALOG_DEFAULT_Y 300

// Lists to keep track of windows, so we can disable/enable them
// for modal dialogs
wxList wxModalDialogs;
wxList wxModelessWindows;  // Frames and modeless dialogs


wxDialogWnd::wxDialogWnd(wxWnd *parent, wxWindow *wx_win,
              int x, int y, int width, int height,
              char *dialog_template):
  wxSubWnd(parent, NULL, wx_win, x, y, width, height, 0, dialog_template)
{
}

LONG wxDialogWnd::DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
  return ::DefWindowProc(handle, nMsg, wParam, lParam);
}

BOOL wxDialogWnd::ProcessMessage(MSG* pMsg)
{
  return ::IsDialogMessage(handle, pMsg);
}

BOOL wxDialogWnd::OnClose(void)
{
  if (wx_window)
  {
    if (wx_window->GetEventHandler()->OnClose())
    {
      ((wxDialogBox *)wx_window)->Show(FALSE);
      delete wx_window;
      return TRUE;
    } else return FALSE;
  }
  return FALSE;
}

BOOL wxDialogWnd::OnEraseBkgnd(HDC pDC)
{
  if (background_brush)
  {
    RECT rect;
    GetClientRect(handle, &rect);
    int mode = SetMapMode(pDC, MM_TEXT);
    FillRect(pDC, &rect, background_brush);
    SetMapMode(pDC, mode);
    return TRUE;
  }
  else return FALSE;
}

wxDialogBox::wxDialogBox(void)
{
  window_parent = NULL;
  handle = NULL;
  modal = FALSE;
  isShown = FALSE;
  modal_showing = FALSE;
  has_child = FALSE ;

  hSpacing = PANEL_HSPACING;
  vSpacing = PANEL_VSPACING;
  
  initial_hspacing = hSpacing ;
  initial_vspacing = vSpacing ;

  current_hspacing = hSpacing ;
  current_vspacing = vSpacing ;
}

// Dialog box - like panel but doesn't need a frame, and is modal or
// non-modal
wxDialogBox::wxDialogBox(wxWindow *Parent, Const char *Title, Bool Modal, 
               int x, int y, int width, int height, long style, Constdata char *name):
  wxbDialogBox(Parent, Title, Modal, x, y, width, height, style, name)
{
  Create(Parent, Title, Modal, x, y, width, height, style, name);
}
  
Bool wxDialogBox::Create(wxWindow *Parent, Const char *Title, Bool Modal, 
                         int x, int y, int width, int height, long style, Constdata char *name)
{
  SetName(name);

  // Do anything that needs to be done in the generic base class
  wxbDialogBox::Create(Parent, Title, Modal, x, y, width, height, style, name);

  has_child = FALSE ;

  hSpacing = PANEL_HSPACING;
  vSpacing = PANEL_VSPACING;
  
  initial_hspacing = hSpacing ;
  initial_vspacing = vSpacing ;

  current_hspacing = hSpacing ;
  current_vspacing = vSpacing ;
  
  labelFont = wxTheFontList->FindOrCreateFont(11, wxSWISS, wxNORMAL, wxNORMAL);
  buttonFont = wxTheFontList->FindOrCreateFont(11, wxSWISS, wxNORMAL, wxNORMAL);

  if (Parent) Parent->AddChild(this);
  window_parent = Parent;

  if (x < 0) x = wxDIALOG_DEFAULT_X;
  if (y < 0) y = wxDIALOG_DEFAULT_Y;

  wxWinType = wxTYPE_XWND;
  windowStyle = style;

  if (windowStyle & wxVERTICAL_LABEL)
    SetLabelPosition(wxVERTICAL);
  else if (windowStyle & wxHORIZONTAL_LABEL)
    SetLabelPosition(wxHORIZONTAL);
  else
    SetLabelPosition(wxHORIZONTAL);

  wxWnd *cparent = NULL;
  if (Parent)
    cparent = (wxWnd *)Parent->handle;

  isShown = FALSE;
  modal_showing = FALSE;

  if (width < 0)
    width = 500;
  if (height < 0)
    height = 500;

  // Allows creation of dialogs with & without captions under MSWindows
  wxDialogWnd *wnd;
  if(style & wxCAPTION){
    wnd = new wxDialogWnd(cparent, this, x, y, width, height,
                          "wxCaptionDialog");
  }
  else{
    wnd = new wxDialogWnd(cparent, this, x, y, width, height,
                          "wxNoCaptionDialog");
  }

  handle = (char *)wnd;
  SetWindowText(wnd->handle, Title);

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  if (!Modal)
    wxModelessWindows.Append(this);
#endif

  wx_dc = new wxPanelDC (this);

  modal = Modal;
  return TRUE;
}

void wxDialogBox::SetModal(Bool flag)
{
  modal = flag;
  
  wxModelessWindows.DeleteObject(this);
  if (!flag)
    wxModelessWindows.Append(this);
}

wxDialogBox::~wxDialogBox()
{
  wxWnd *wnd = (wxWnd *)handle;
  if (wnd && modal_showing)
    Show(FALSE);

  modal_showing = FALSE;
  if (wnd)
  {
    ShowWindow(wnd->handle, SW_HIDE);
  }
  if (!modal)
    wxModelessWindows.DeleteObject(this);

  // For some reason, wxWindows can activate another task altogether
  // when a frame is destroyed after a modal dialog has been invoked.
  // Try to bring the parent to the top.
  HWND hWnd = 0;
  if (GetParent() && GetParent()->GetHWND())
    ::BringWindowToTop(GetParent()->GetHWND());
}

// By default, pressing escape quits the dialog
Bool wxDialogBox::OnCharHook(wxKeyEvent& event)
{
  if (handle)
  {
    if (event.keyCode == WXK_ESCAPE)
    {
      if (Close())
        return TRUE;
      else
        return FALSE;
/*
      if (OnClose())
      {
        Show(FALSE);
        delete this;
      }
      return TRUE;
*/
    }
  }
  return FALSE;
}

void wxDialogBox::Fit(void)
{
  wxPanel::Fit();
}

void wxDialogBox::Iconize(Bool WXUNUSED(iconize))
{
  // Windows dialog boxes can't be iconized
}

Bool wxDialogBox::Iconized(void)
{
  return FALSE;
}

void wxDialogBox::SetSize(int x, int y, int width, int height, int WXUNUSED(sizeFlags))
{
  wxWindow::SetSize(x, y, width, height);
}

void wxDialogBox::SetClientSize(int width, int height)
{
  HWND hWnd = GetHWND();
  RECT rect;
  GetClientRect(hWnd, &rect);

  RECT rect2;
  GetWindowRect(hWnd, &rect2);

  // Find the difference between the entire window (title bar and all)
  // and the client area; add this to the new client size to move the
  // window
  int actual_width = rect2.right - rect2.left - rect.right + width;
  int actual_height = rect2.bottom - rect2.top - rect.bottom + height;

  MoveWindow(hWnd, rect2.left, rect2.top, actual_width, actual_height, TRUE);
  GetEventHandler()->OnSize(actual_width, actual_height);
}

void wxDialogBox::GetPosition(int *x, int *y)
{
  HWND hWnd = GetHWND();
  RECT rect;
  GetWindowRect(hWnd, &rect);

  *x = rect.left;
  *y = rect.top;
}

Bool wxDialogBox::IsShown(void)
{
  return isShown;
}

Bool wxDialogBox::Show(Bool show)
{
  isShown = show;

#if WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  if (!modal) {
    if (show) {
      if (!wxModelessWindows.Member(this))
	wxModelessWindows.Append(this);
    } else
      wxModelessWindows.DeleteObject(this);
  }
  if (show) {
    if (!wxTopLevelWindows.Member(this))
      wxTopLevelWindows.Append(this);
  } else
    wxTopLevelWindows.DeleteObject(this);
#endif

  if (modal)
  {
    if (show)
    {
      wxList DisabledWindows;
      if (modal_showing)
      {
        BringWindowToTop(GetHWND());
        return TRUE;
      }
      
      modal_showing = TRUE;
      wxNode *node = wxModalDialogs.First();
      while (node)
      {
        wxDialogBox *box = (wxDialogBox *)node->Data();
        if (box != this)
          ::EnableWindow(box->GetHWND(), FALSE);
        node = node->Next();
      }
      node = wxModelessWindows.First();
      while (node)
      {
        wxWindow *win = (wxWindow *)node->Data();
        ::EnableWindow(win->GetHWND(), FALSE);
        DisabledWindows.Append((wxObject *)win->GetHWND());
        node = node->Next();
      }

      ShowWindow(GetHWND(), SW_SHOW);
      EnableWindow(GetHWND(), TRUE);
      BringWindowToTop(GetHWND());

      if (!wxModalDialogs.Member(this))
        wxModalDialogs.Append(this);

      MSG msg;
      // Must test whether this dialog still exists: we may not process
      // a message before the deletion.
      while (wxModalDialogs.Member(this) && modal_showing && GetMessage(&msg, NULL, 0, 0))
      {
        if (!IsDialogMessage(GetHWND(), &msg))
        {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
      }
      // dfgg: now must specifically re-enable all other app windows that we disabled earlier
      node=DisabledWindows.First();
      while(node) {
        HWND hWnd = (HWND)node->Data();
        if (::IsWindow(hWnd)) // Check handle is still valid
          ::EnableWindow(hWnd,TRUE);
        node=node->Next();
      }
    }
    else
    {
      wxModalDialogs.DeleteObject(this);

      wxNode *last = wxModalDialogs.Last();

      // If there's still a modal dialog active, we
      // enable it, else we enable all modeless windows
      if (last)
      {
        wxDialogBox *box = (wxDialogBox *)last->Data();
        HWND hwnd = box->GetHWND();
        if (box->winEnabled)
          EnableWindow(hwnd, TRUE);
        BringWindowToTop(GetHWND());
      }
      else
      {
        wxNode *node = wxModelessWindows.First();
        while (node)
        {
          wxWindow *win = (wxWindow *)node->Data();
          HWND hwnd = win->GetHWND();
          // Only enable again if not user-disabled.
          if (win->winEnabled)
            EnableWindow(hwnd, TRUE);
          node = node->Next();
        }
      }
      // Try to highlight the correct window (the parent)
      HWND hWndParent = 0;
      if (GetParent())
      {
        hWndParent = GetParent()->GetHWND();
        if (hWndParent)
          ::BringWindowToTop(hWndParent);
      }
      ShowWindow(GetHWND(), SW_HIDE);
      modal_showing = FALSE;
    }
  }
  else
  {
    if (show)
    {
      ShowWindow(GetHWND(), SW_SHOW);
      BringWindowToTop(GetHWND());
    }
    else
    {
      // Try to highlight the correct window (the parent)
      HWND hWndParent = 0;
      if (GetParent())
      {
        hWndParent = GetParent()->GetHWND();
        if (hWndParent)
          ::BringWindowToTop(hWndParent);
      }
      ShowWindow(GetHWND(), SW_HIDE);
    }
  }
  return TRUE;
}

void wxDialogBox::SetTitle(char *title)
{
  SetWindowText(GetHWND(), title);
}

char *wxDialogBox::GetTitle(void)
{
  GetWindowText(GetHWND(), wxBuffer, 1000);
  return wxBuffer;
}
