/*
 * File:	wx_frame.cc
 * Purpose:	wxFrame implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_frame.cc,v 1.1 1994/08/14 21:59:17 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   26 May 97    3:26 pm
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_setup.h"
#include "wx_frame.h"
#include "wx_menu.h"
#include "wx_gdi.h"
#include "wx_main.h"
#include "wx_utils.h"
#include "wx_dialg.h"
#include "wx_text.h"
#endif

#include "wx_privt.h"
#include "wx_itemp.h"

#if FAFA_LIB
#include "fafa.h"
#endif

#if USE_ITSY_BITSY
#include "..\..\contrib\itsybits\itsybits.h"
#endif

extern wxList wxModelessWindows;

wxPen *wxStatusGreyPen = NULL;
wxPen *wxStatusWhitePen = NULL;

#define IDM_WINDOWTILE  4001
#define IDM_WINDOWCASCADE 4002
#define IDM_WINDOWICONS 4003
#define IDM_WINDOWNEXT 4004
// This range gives a maximum of 500
// MDI children. Should be enough :-)
#define wxFIRST_MDI_CHILD 4100
#define wxLAST_MDI_CHILD 4600

// Status border dimensions
#define         wxTHICK_LINE_BORDER 3
#define         wxTHICK_LINE_WIDTH  1

extern char wxFrameClassName[];
extern char wxMDIFrameClassName[];
extern char wxMDIChildFrameClassName[];
extern char wxPanelClassName[];

wxFrame::wxFrame(void)
{
  wx_menu_bar = NULL;
  status_line_exists = FALSE;
  status_widths_exists = FALSE;
  status_widths = NULL;

  icon = NULL;
  modal_showing = FALSE;
  window_parent = NULL;
  int i;
  for (i = 0; i < wxMAX_STATUS; i++)
    status_window[i] = NULL;
  wx_iconized = FALSE;
  wxWinType = 0;
  handle = NULL;
}

wxFrame::wxFrame(wxFrame *Parent, Const char *title, int x, int y,
                 int width, int height, long style, Constdata char *name):
  wxbFrame(Parent, title, x, y, width, height, style, name)
{
  Create(Parent, title, x, y, width, height, style, name);
}

Bool wxFrame::Create(wxFrame *Parent, Const char *title, int x, int y,
                 int width, int height, long style, Constdata char *name)
{
  wxbFrame::Create(Parent, title, x, y, width, height, style, name);
  
  SetName(name);
  windowStyle = style;
  wx_menu_bar = NULL;
  status_line_exists = FALSE;
  status_widths_exists = FALSE;
  status_widths = NULL;
  icon = NULL;
  modal_showing = FALSE;
  handle = NULL;
  
  if (Parent) Parent->AddChild(this);
  window_parent = Parent;

  int i;
  for (i = 0; i < wxMAX_STATUS; i++)
    status_window[i] = NULL;

  wx_iconized = FALSE;
  wxWnd *cparent = NULL;
  if (Parent)
    cparent = (wxWnd *)Parent->handle;

  long frame_type =  windowStyle & (wxSDI | wxMDI_PARENT | wxMDI_CHILD);

  switch (frame_type)
  {
    case wxMDI_PARENT:
      wxWinType = wxTYPE_XWND;
      handle = (char *)new wxMDIFrame(NULL, this, (char *)title, x, y, width, height, style);
      break;
    case wxMDI_CHILD:
      wxWinType = wxTYPE_MDICHILD;
      handle = (char *)new wxMDIChild((wxMDIFrame *)cparent, this, (char *)title, x, y, width, height, style);
      break;
    default:
    case wxSDI:
      wxWinType = wxTYPE_XWND;
      handle = (char *)new wxFrameWnd(cparent, wxFrameClassName, this, (char *)title,
                   x, y, width, height, style);
      break;
  }

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxModelessWindows.Append(this);
#endif
  return TRUE;
}

wxFrame::~wxFrame(void)
{
  if (status_widths_exists)
    delete [] status_widths;

  if (wx_menu_bar)
    delete wx_menu_bar;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  if (icon)
    delete icon;
#endif

  int i;
  for (i = 0; i < wxMAX_STATUS; i++)
    if (status_window[i])
    {
      status_window[i]->DestroyWindow();
      delete status_window[i];
    }

  if (wxTheApp && (this == wxTheApp->wx_frame))
  {
    wxTheApp->wx_frame = NULL;

    if (wxTheApp->GetExitOnFrameDelete())
    {
       PostQuitMessage(0);
    }
  }

  wxModelessWindows.DeleteObject(this);

  // For some reason, wxWindows can activate another task altogether
  // when a frame is destroyed after a modal dialog has been invoked.
  // Try to bring the parent to the top.
  HWND hWnd = 0;
  if (GetParent() && GetParent()->GetHWND())
    ::BringWindowToTop(GetParent()->GetHWND());
}

HMENU wxFrame::GetWinMenu(void)
{
  if (handle)
    return ((wxWnd *)handle)->hMenu;
  else return 0;
}

// Get size *available for subwindows* i.e. excluding menu bar etc.
// For XView, this is the same as GetSize
void wxFrame::GetClientSize(int *x, int *y)
{
  RECT rect;
  GetClientRect(GetHWND(), &rect);

  long frame_type =  windowStyle & (wxSDI | wxMDI_PARENT | wxMDI_CHILD);
  switch (frame_type)
  {
    case wxMDI_PARENT:
    {
      int cwidth = rect.right;
      int cheight = rect.bottom;
      int ctop = 0;

      if (frameToolBar)
      {
        int tw, th;
        frameToolBar->GetSize(&tw, &th);
        ctop = th;
        cheight -= th;
      }

      if (status_window[0])
        cheight -= status_window[0]->height;
      *x = cwidth;
      *y = cheight;
      break;
    }
    default:
    case wxMDI_CHILD:
    case wxSDI:
    {
      if (status_window[0])
        rect.bottom -= status_window[0]->height;

      *x = rect.right;
      *y = rect.bottom;
      break;
    }
  }
}

// Set the client size (i.e. leave the calculation of borders etc.
// to wxWindows)
void wxFrame::SetClientSize(int width, int height)
{
  wxFrame *parent = (wxFrame *)GetParent();
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

  if (status_window[0])
    actual_height += status_window[0]->height;

  POINT point;
  point.x = rect2.left;
  point.y = rect2.top;

  // If there's an MDI parent, must subtract the parent's top left corner
  // since MoveWindow moves relative to the parent
  if (parent && (wxWinType == wxTYPE_MDICHILD))
  {
    wxMDIFrame *mdiParent = (wxMDIFrame *)parent->handle;
    ::ScreenToClient(mdiParent->client_hwnd, &point);

//    ::ScreenToClient(hParentWnd, &point);
  }

  MoveWindow(hWnd, point.x, point.y, actual_width, actual_height, (BOOL)TRUE);
  GetEventHandler()->OnSize(actual_width, actual_height);
}

void wxFrame::GetSize(int *width, int *height)
{
  RECT rect;
  GetWindowRect(GetHWND(), &rect);
  *width = rect.right - rect.left;
  *height = rect.bottom - rect.top;
}

void wxFrame::GetPosition(int *x, int *y)
{
  wxWindow *parent = GetParent();

  RECT rect;
  GetWindowRect(GetHWND(), &rect);
  POINT point;
  point.x = rect.left;
  point.y = rect.top;

  // Since we now have the absolute screen coords,
  // if there's a parent we must subtract its top left corner
  if (parent)
  {
    wxWnd *cparent = (wxWnd *)(parent->handle);
    if (wxWinType == wxTYPE_MDICHILD)
    {
      wxMDIFrame *mdiParent = (wxMDIFrame *)cparent;
      ::ScreenToClient(mdiParent->client_hwnd, &point);
    }
  }
  *x = point.x;
  *y = point.y;
}

void wxFrame::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  int ww,hh ;
  GetSize(&ww,&hh) ;
  if (width == -1) width = ww ;
  if (height==-1) height = hh ;

  if (handle)
  {
    MoveWindow(GetHWND(), x, y, width, height, (BOOL)TRUE);
    GetEventHandler()->OnSize(width, height);
  }
}

Bool wxFrame::Show(Bool show)
{
  int cshow;
  if (show)
    cshow = SW_SHOW;
  else
    cshow = SW_HIDE;

#if WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  if (show)  {
    if (!wxModelessWindows.Member(this))
      wxModelessWindows.Append(this);
    if (!wxTopLevelWindows.Member(this))
      wxTopLevelWindows.Append(this);
  } else {
    wxModelessWindows.DeleteObject(this);
    wxTopLevelWindows.DeleteObject(this);
  }
#endif  

  if (!show)
  {
    // Try to highlight the correct window (the parent)
    HWND hWndParent = 0;
    if (GetParent())
    {
      hWndParent = GetParent()->GetHWND();
      if (hWndParent)
        ::BringWindowToTop(hWndParent);
    }
  }

  ShowWindow(GetHWND(), (BOOL)cshow);
  if (show)
  {
    BringWindowToTop(GetHWND());
    OnActivate(TRUE);
  }
  return TRUE;
}

void wxFrame::Iconize(Bool iconize)
{
  if (!iconize)
    Show(TRUE);

  int cshow;
  if (iconize)
    cshow = SW_MINIMIZE;
  else
    cshow = SW_RESTORE;
  ShowWindow(GetHWND(), (BOOL)cshow);
  wx_iconized = iconize;
}

// Equivalent to maximize/restore in Windows
void wxFrame::Maximize(Bool maximize)
{
  Show(TRUE);
  int cshow;
  if (maximize)
    cshow = SW_MAXIMIZE;
  else
    cshow = SW_RESTORE;
  ShowWindow(GetHWND(), cshow);
  wx_iconized = FALSE;
}

Bool wxFrame::Iconized(void)
{
  wx_iconized = (Bool)::IsIconic(GetHWND());
  return wx_iconized;
}

void wxFrame::SetTitle(char *title)
{
  SetWindowText(GetHWND(), title);
}

char *wxFrame::GetTitle(void)
{
  GetWindowText(GetHWND(), wxBuffer, 1000);
  return wxBuffer;
}

void wxFrame::SetIcon(wxIcon *wx_icon)
{
  icon = wx_icon;
    
  wxFrameWnd *wnd = (wxFrameWnd *)handle;
  if (!wx_icon)
    wnd->icon = 0;
  else
  {
    wnd->icon = wx_icon->ms_icon;
#if WIN95
    SendMessage(GetHWND(), WM_SETICON,
                (WPARAM)TRUE, (LPARAM)wnd->icon);
#endif
  }
}

void wxFrame::CreateStatusLine(int number, Constdata char *WXUNUSED(name))
{
  if (status_line_exists)
    return;

  status_line_exists = TRUE;

  nb_status = number;

  wxFrameWnd *cframe = (wxFrameWnd *)handle;

  TEXTMETRIC tm;
  HDC dc = GetDC(cframe->handle);
  SelectObject(dc, wxSTATUS_LINE_FONT);
  GetTextMetrics(dc, &tm);
  ReleaseDC(cframe->handle, dc);
  int char_height = tm.tmHeight + tm.tmExternalLeading;
  int status_window_height =
     (int)((char_height * 4.0/3.0) + 2*wxTHICK_LINE_BORDER);

  if (!wxStatusGreyPen)
  {
    wxStatusGreyPen = new wxPen("DIM GREY", wxTHICK_LINE_WIDTH, wxSOLID);
    wxStatusWhitePen = new wxPen("WHITE", wxTHICK_LINE_WIDTH, wxSOLID);
  }

  int i;
  for (i = 0; i < number; i++)
    status_window[i] = new wxStatusWnd(cframe, status_window_height);
  PositionStatusWindow();
}

void wxFrame::SetStatusText(char *text, int number)
{
  if (!status_line_exists)
    return;

  if ((number < 0) || (number >= nb_status))
    return;

#if FAFA_LIB
  // Microsoft standard: use button colors for status line
  status_window[number]->light_grey_brush = brushFace ;
#endif

  if (status_window[number]->status_text)
    delete[] status_window[number]->status_text;

  if (text)
    status_window[number]->status_text = copystring(text);
  else status_window[number]->status_text = NULL;

  HDC dc = GetDC(status_window[number]->handle);
  SelectObject(dc, wxSTATUS_LINE_FONT);

  RECT rect;
  GetClientRect(status_window[number]->handle, &rect );

  int width = rect.right;
  int height = rect.bottom;

  SetBkMode(dc, TRANSPARENT);

  ::SetTextColor(dc, ::GetSysColor( COLOR_BTNTEXT ) );

  TEXTMETRIC tm;
  GetTextMetrics(dc, &tm);
  int cy = tm.tmHeight + tm.tmExternalLeading;
  int y = (int)((rect.bottom - cy)/2);

  rect.left += wxTHICK_LINE_BORDER + 1;
  rect.top += wxTHICK_LINE_BORDER + 1;
  rect.right -= (wxTHICK_LINE_BORDER + 1);
  rect.bottom -= (wxTHICK_LINE_BORDER + 1);
  FillRect(dc, &rect, status_window[number]->light_grey_brush);

  IntersectClipRect(dc, wxTHICK_LINE_BORDER + 3, y-1,
                            width - wxTHICK_LINE_BORDER - 1, height);

  if (status_window[number]->status_text)
    TextOut(dc, wxTHICK_LINE_BORDER + 4, y,
                status_window[number]->status_text, strlen(status_window[number]->status_text));

  SelectClipRgn(dc, NULL);
  ReleaseDC(status_window[number]->handle, dc);
}

void wxFrame::SetStatusWidths(int n, int *widths_field)
{
  // only set status widths, when n == number of statuswindows
  if (n == nb_status)
  {
    // only set status widths,
    // when one window (minimum) is variable (width <= 0)
    Bool is_variable = FALSE;
    int i;
    for (i = 0; i < nb_status; i++)
    {
      if (widths_field[i] <= 0) is_variable = TRUE;
    }

    // if there are old widths, delete them
    if (status_widths_exists)
    {
      delete [] status_widths;
      status_widths_exists = FALSE;
    }
    // set widths
    status_widths = new int[n];
    for (i = 0; i < nb_status; i++)
    {
      status_widths[i] = widths_field[i];
    }
    status_widths_exists = TRUE;
    PositionStatusWindow();
  }
}

void wxFrame::PositionStatusWindow(void)
{
  RECT rect;
  GetClientRect(GetHWND(), &rect);
  int cwidth = rect.right;
  int cheight = rect.bottom;
  int i;
  int sum_of_nonvar = 0;
  int num_of_var = 0;
  Bool do_same_width = FALSE;
  if (status_widths_exists)
  {
    // if sum(not variable Windows) > c_width - (20 points per variable_window)
    // then do_same_width = TRUE;
    for (i = 0; i < nb_status; i++)
    {
       if (status_widths[i] > 0) sum_of_nonvar += status_widths[i];
        else num_of_var++;
     }
     if (sum_of_nonvar > (cwidth - 20*num_of_var)) do_same_width = TRUE;
  }
  else do_same_width = TRUE;
  if (do_same_width)
  {
    for (i = 0; i < nb_status; i++)
    {
      int real_width = (int)(cwidth/nb_status);
      MoveWindow(status_window[i]->handle, i*real_width, cheight - status_window[0]->height,
                            real_width, status_window[0]->height, TRUE);
    }
  }
  else // no_same_width
  {
    int *tempwidth = new int[nb_status];
    int temppos = 0;
    for (i = 0; i < nb_status; i++)
    {
      if (status_widths[i] > 0) tempwidth[i] = status_widths[i];
      else tempwidth[i] = (cwidth - sum_of_nonvar) / num_of_var;
    }
    for (i = 0; i < nb_status; i++)
    {
      MoveWindow(status_window[i]->handle, temppos,
         cheight - status_window[0]->height, tempwidth[i], status_window[0]->height, TRUE);
      temppos += tempwidth[i];
    }
    delete [] tempwidth;
  }
}

void wxFrame::LoadAccelerators(char *table)
{
  wxFrameWnd *cframe = (wxFrameWnd *)handle;
  cframe->accelerator_table = ::LoadAccelerators(wxhInstance, table);
}

void wxFrame::Fit(void)
{
  // Work out max. size
  wxNode *node = children->First();
  int max_width = 0;
  int max_height = 0;
  while (node)
  {
    // Find a child that's a subwindow, but not a dialog box.
    wxWindow *win = (wxWindow *)node->Data();

    if ((win->IsKindOf(CLASSINFO(wxPanel)) &&
         !win->IsKindOf(CLASSINFO(wxDialogBox))) ||
        win->IsKindOf(CLASSINFO(wxTextWindow)) ||
        win->IsKindOf(CLASSINFO(wxCanvas)))
    {
      int width, height;
      int x, y;
      win->GetSize(&width, &height);
      win->GetPosition(&x, &y);

      if ((x + width) > max_width)
        max_width = x + width;
      if ((y + height) > max_height)
        max_height = y + height;
    }
    node = node->Next();
  }
  SetClientSize(max_width, max_height);
}

// Returns the active MDI child window
wxFrame *wxFrame::GetActiveChild(void)
{
  if (windowStyle & wxMDI_PARENT != wxMDI_PARENT)
    return NULL;
    
  wxMDIFrame *wnd = (wxMDIFrame *)handle;
  if (!wnd)
    return NULL;
    
  HWND hWnd = (HWND)LOWORD(SendMessage(wnd->client_hwnd, WM_MDIGETACTIVE, 0, 0L));
  if (hWnd == 0)
    return NULL;
  else
  {
    wxWnd *wnd = wxFindWinFromHandle(hWnd);
    if (wnd)
      return (wxFrame *)wnd->wx_window;
  }
  return NULL;
}

/*
 * Windows 3 specific windows
 *
 */

wxStatusWnd::wxStatusWnd(wxFrameWnd *parent, int the_height)
{
  status_text = NULL;
  height = the_height;
#if FAFA_LIB
  // Microsoft standard: use button colors for status line
  light_grey_brush = brushFace ;
#else
  light_grey_brush = GetStockObject(LTGRAY_BRUSH);
#endif

  Create(parent, wxPanelClassName, NULL, NULL, 0, 0, 100, 100, WS_CHILD);
  ShowWindow(handle, SW_SHOW);
}

wxStatusWnd::~wxStatusWnd(void)
{
  if (status_text)
    delete[] status_text;
}

BOOL wxStatusWnd::OnPaint()
{
#if DEBUG > 1
  wxDebugMsg("wxStatusWnd::OnPaint %d\n", handle);
#endif
  RECT rect;
  if (GetUpdateRect(handle, &rect, FALSE))
  {
#if FAFA_LIB
  // Microsoft standard: use button colors for status line
    light_grey_brush = brushFace ;
#endif
    PAINTSTRUCT ps;
    // Hold a pointer to the dc so long as the OnPaint() message
    // is being processed
    HDC cdc = BeginPaint(handle, &ps);
    SelectObject(cdc, wxSTATUS_LINE_FONT);

    ::GetClientRect(handle, &rect);

    int width = rect.right;
    int height = rect.bottom;

    ::SetBkMode(cdc, TRANSPARENT);
    ::FillRect(cdc, &rect, light_grey_brush);

#if !FAFA_LIB
    wxGREY_BRUSH->RealizeResource() ;
    HBRUSH old_brush = ::SelectObject(cdc, wxGREY_BRUSH->GetResourceHandle());

    // Draw border
    // Have grey background, plus 3-d border -
    // One black rectangle.
    // Inside this, left and top sides - dark grey. Bottom and right -
    // white.

    // Right and bottom white lines
    wxStatusWhitePen->RealizeResource() ;
    HPEN old_pen = ::SelectObject(cdc, wxStatusWhitePen->GetResourceHandle());
    MoveToEx(cdc, width-wxTHICK_LINE_BORDER,
                  wxTHICK_LINE_BORDER, NULL);
    LineTo(cdc, width-wxTHICK_LINE_BORDER,
                height-wxTHICK_LINE_BORDER);
    LineTo(cdc, wxTHICK_LINE_BORDER,
              height-wxTHICK_LINE_BORDER);

    // Left and top grey lines
    wxStatusGreyPen->RealizeResource() ;
    old_pen = ::SelectObject(cdc, wxStatusGreyPen->GetResourceHandle());
    LineTo(cdc, wxTHICK_LINE_BORDER, wxTHICK_LINE_BORDER);
    LineTo(cdc, width-wxTHICK_LINE_BORDER, wxTHICK_LINE_BORDER);
#else
    HBRUSH old_brush = ::SelectObject(cdc,brushFace) ;

    // Draw border
    // Have grey background, plus 3-d border -
    // One black rectangle.
    // Inside this, left and top sides - dark grey. Bottom and right -
    // white.

    // Right and bottom white lines
    HPEN old_pen = ::SelectObject(cdc,penLight) ;
    MoveToEx(cdc, width-wxTHICK_LINE_BORDER,
                  wxTHICK_LINE_BORDER, NULL);
    LineTo(cdc, width-wxTHICK_LINE_BORDER,
                height-wxTHICK_LINE_BORDER);
    LineTo(cdc, wxTHICK_LINE_BORDER,
              height-wxTHICK_LINE_BORDER);

    // Left and top grey lines
    ::SelectObject(cdc,penShadow) ;
    LineTo(cdc, wxTHICK_LINE_BORDER, wxTHICK_LINE_BORDER);
    LineTo(cdc, width-wxTHICK_LINE_BORDER, wxTHICK_LINE_BORDER);
#endif

    SetTextColor(cdc, ::GetSysColor( COLOR_BTNTEXT ) );

    TEXTMETRIC tm;
    ::GetTextMetrics(cdc, &tm);
    int cy = tm.tmHeight + tm.tmExternalLeading;
    int y = (int)((rect.bottom - cy)/2);

    ::IntersectClipRect(cdc, wxTHICK_LINE_BORDER + 3, y-1,
                            rect.right - wxTHICK_LINE_BORDER - 1, rect.bottom);

    if (status_text)
      ::TextOut(cdc, wxTHICK_LINE_BORDER + 4, y,
                  status_text, strlen(status_text));

    ::SelectClipRgn(cdc, NULL);
    if (old_pen)
      SelectObject(cdc, old_pen);
    old_pen = NULL ;
    if (old_brush)
      SelectObject(cdc, old_brush);
    old_brush = NULL ;

    EndPaint(handle, &ps);
//    cdc = NULL;
    return 0;
  }
  return 1;
}


/*
 * Frame window
 *
 */

wxFrameWnd::wxFrameWnd(void)
{
}
		   
wxFrameWnd::wxFrameWnd(wxWnd *parent, char *WXUNUSED(wclass), wxWindow *wx_win, char *title,
                   int x, int y, int width, int height, long style)
{
  defaultIcon = (wxSTD_FRAME_ICON ? wxSTD_FRAME_ICON : wxDEFAULT_FRAME_ICON);

//  DWORD msflags = WS_OVERLAPPED;
  DWORD msflags = WS_OVERLAPPED | WS_CLIPCHILDREN ; // WS_POPUP | WS_CLIPCHILDREN ;

  DWORD extendedStyle = 0;
  if (style & wxMINIMIZE_BOX)
    msflags |= WS_MINIMIZEBOX;
  if (style & wxMAXIMIZE_BOX)
    msflags |= WS_MAXIMIZEBOX;
  if (style & wxTHICK_FRAME)
    msflags |= WS_THICKFRAME;
  if (style & wxSYSTEM_MENU)
    msflags |= WS_SYSMENU;
  if ((style & wxMINIMIZE) || (style & wxICONIZE))
    msflags |= WS_MINIMIZE;
  if (style & wxMAXIMIZE)
    msflags |= WS_MAXIMIZE;
  if (style & wxCAPTION)
    msflags |= WS_CAPTION;

#if USE_ITSY_BITSY
  if (style & wxTINY_CAPTION_VERT)
    msflags |= IBS_VERTCAPTION;
  if (style & wxTINY_CAPTION_HORIZ)
    msflags |= IBS_HORZCAPTION;
#else
  if (style & wxTINY_CAPTION_VERT)
    msflags |= WS_CAPTION;
  if (style & wxTINY_CAPTION_HORIZ)
    msflags |= WS_CAPTION;
#endif
  if ((style & wxTHICK_FRAME) == 0)
    msflags |= WS_BORDER;

  if (style & wxSTAY_ON_TOP)
    extendedStyle |= WS_EX_TOPMOST;

  icon = NULL;
  iconized = FALSE;
  Create(parent, wxFrameClassName, wx_win, title, x, y, width, height,
         msflags, NULL, extendedStyle);
  // Seems to be necessary if we use WS_POPUP
  // style instead of WS_OVERLAPPED
  if (width > -1 && height > -1)
    ::PostMessage(handle, WM_SIZE, SIZE_RESTORED, MAKELPARAM(width, height));
#if DEBUG > 1
  wxDebugMsg("wxFrameWnd::wxFrameWnd %d\n", handle);
#endif
}

wxFrameWnd::~wxFrameWnd(void)
{
}

BOOL wxFrameWnd::OnPaint(void)
{
#if DEBUG > 1
  wxDebugMsg("wxFrameWnd::OnPaint %d\n", handle);
#endif
  RECT rect;
  if (GetUpdateRect(handle, &rect, FALSE))
  {
    PAINTSTRUCT ps;
    // Hold a pointer to the dc so long as the OnPaint() message
    // is being processed
    HDC cdc = BeginPaint(handle, &ps);
      
    if (iconized)
    {
      HICON the_icon = icon;
      if (the_icon == 0)
        the_icon = defaultIcon;

      // Erase background before painting or we get white background
      this->DefWindowProc(WM_ICONERASEBKGND,(WORD)ps.hdc,0L);
      
      if (the_icon)
      {
        RECT rect;
        GetClientRect(handle, &rect);
        int icon_width = 32;
        int icon_height = 32;
        int icon_x = (int)((rect.right - icon_width)/2);
        int icon_y = (int)((rect.bottom - icon_height)/2);
        DrawIcon(cdc, icon_x, icon_y, the_icon);
      }
    }

    if (!iconized && wx_window)
    {
      wx_window->paintHDC = cdc;
      wx_window->GetEventHandler()->OnPaint();
      wx_window->paintHDC = NULL;
    }

    EndPaint(handle, &ps);
    return 0;
  }
  return 1;
}

HICON wxFrameWnd::OnQueryDragIcon(void)
{
  if (icon != 0)
    return icon;
  else
    return defaultIcon;
}

void wxFrameWnd::OnSize(int x, int y, UINT id)
{
#if DEBUG > 1
  wxDebugMsg("wxFrameWnd::OnSize %d\n", handle);
#endif
  switch (id)
  {
    case SIZEFULLSCREEN:
    case SIZENORMAL:
      iconized = FALSE;
    break;
    case SIZEICONIC:
      iconized = TRUE;
    break;
  }

 if (!iconized)
 {
  wxFrame *frame = (wxFrame *)wx_window;
  if (frame && frame->status_window[0])
    frame->PositionStatusWindow();

  if (wx_window && wx_window->handle)
    wx_window->GetEventHandler()->OnSize(x, y);
 }
}

BOOL wxFrameWnd::OnClose(void)
{
#if DEBUG > 1
  wxDebugMsg("wxFrameWnd::OnClose %d\n", handle);
#endif
  if (wx_window)
  {
    if (wx_window->GetEventHandler()->OnClose())
    {
      delete wx_window;
      return TRUE;
    } else return FALSE;
  }
  return FALSE;
}

BOOL wxFrameWnd::OnCommand(WORD id, WORD cmd, HWND control)
{
#if DEBUG > 1
  wxDebugMsg("wxFrameWnd::OnCommand %d\n", handle);
#endif
  if (cmd == 0 || cmd == 1 ) // Can be either a menu command or an accelerator.
  {
    // In case it's e.g. a toolbar.
    wxWindow *win = wxFindControlFromHandle(control);
    if (win)
      return win->MSWCommand(cmd, id);

    wxFrame *frame = (wxFrame *)wx_window;
    if (frame->GetMenuBar() && frame->GetMenuBar()->FindItemForId(id))
    {
      ((wxFrame *)wx_window)->Command(id);
      return TRUE;
    }
    else
      return FALSE;
  }
  else
    return FALSE;
}

void wxFrameWnd::OnMenuSelect(WORD nItem, WORD nFlags, HMENU hSysMenu)
{
  wxFrame *frame = (wxFrame *)wx_window;
  if (nFlags == 0xFFFF && hSysMenu == NULL)
    frame->GetEventHandler()->OnMenuSelect(-1);
  else if (nFlags != MF_SEPARATOR)
    frame->GetEventHandler()->OnMenuSelect(nItem);
}

BOOL wxFrameWnd::ProcessMessage(MSG* pMsg)
{
  if (accelerator_table != NULL &&
          ::TranslateAccelerator(handle, accelerator_table, pMsg))
    return TRUE;
	
  return FALSE;
}

/*
 * Windows MDI stuff
 */

wxMDIFrame::wxMDIFrame(wxWnd *parent, wxWindow *wx_win, char *title,
                   int x, int y, int width, int height, long style)
{
  defaultIcon = (wxSTD_MDIPARENTFRAME_ICON ? wxSTD_MDIPARENTFRAME_ICON : wxDEFAULT_MDIPARENTFRAME_ICON);
  icon = NULL;
  iconized = FALSE;
  parent_frame_active = TRUE;
  current_child = NULL;
  client_hwnd = NULL;

  window_menu = ::LoadMenu(wxhInstance, "wxWindowMenu");
#if DEBUG > 1
  wxDebugMsg("Loaded window_menu %d\n", window_menu);
#endif
  
  DWORD msflags = WS_OVERLAPPED;
  if (style & wxMINIMIZE_BOX)
    msflags |= WS_MINIMIZEBOX;
  if (style & wxMAXIMIZE_BOX)
    msflags |= WS_MAXIMIZEBOX;
  if (style & wxTHICK_FRAME)
    msflags |= WS_THICKFRAME;
  if (style & wxSYSTEM_MENU)
    msflags |= WS_SYSMENU;
  if ((style & wxMINIMIZE) || (style & wxICONIZE))
    msflags |= WS_MINIMIZE;
  if (style & wxMAXIMIZE)
    msflags |= WS_MAXIMIZE;
  if (style & wxCAPTION)
    msflags |= WS_CAPTION;

  Create(parent, wxMDIFrameClassName, wx_win, title, x, y, width, height,
         msflags);
#if DEBUG > 1
  wxDebugMsg("End of wxMDIFrame::wxMDIFrame %d\n", handle);
#endif
}

wxMDIFrame::~wxMDIFrame(void)
{
#if DEBUG > 1
  wxDebugMsg("wxMDIFrame::~wxMDIFrame %d\n", handle);
#endif
  DestroyMenu(window_menu); // Destroy dummy "Window" menu
}

BOOL wxMDIFrame::OnDestroy(void)
{
#if DEBUG > 1
  wxDebugMsg("wxMDIFrame::OnDestroy %d\n", handle);
#endif
  return FALSE;
}

void wxMDIFrame::OnCreate(LPCREATESTRUCT WXUNUSED(cs))
{
#if DEBUG > 1
  wxDebugMsg("wxMDIFrame::OnCreate %d\n", handle);
#endif
  CLIENTCREATESTRUCT ccs;
	
  ccs.hWindowMenu = window_menu;
  ccs.idFirstChild = wxFIRST_MDI_CHILD;

  client_hwnd = ::CreateWindowEx(0, "mdiclient", NULL,
                WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0, 0, 0, 0, handle, NULL,
                wxhInstance, (LPSTR)(LPCLIENTCREATESTRUCT)&ccs);
}

void wxMDIFrame::OnSize(int x, int y, UINT id)
{
#if DEBUG > 1
  wxDebugMsg("wxMDIFrame::OnSize %d\n", handle);
#endif
  switch (id)
  {
    case SIZEFULLSCREEN:
    case SIZENORMAL:
      iconized = FALSE;
    break;
    case SIZEICONIC:
      iconized = TRUE;
    break;
  }

 if (!iconized)
 {
  wxFrame *frame = (wxFrame *)wx_window;

  if (frame && (frame->status_window[0] || frame->frameToolBar))
  {
    RECT rect;
    GetClientRect(handle, &rect);
    int cwidth = rect.right;
    int cheight = rect.bottom;
    int ctop = 0;
    int tw, th;

    if (frame->frameToolBar)
    {
      frame->frameToolBar->GetSize(&tw, &th);
      ctop = th;
      cheight -= th;
    }

    if (frame->status_window[0])
      cheight -= frame->status_window[0]->height;

    MoveWindow(client_hwnd, 0, ctop, cwidth, cheight, TRUE);

    if (frame->frameToolBar)
      frame->frameToolBar->SetSize(0, 0, cwidth, th);
    if (frame->status_window[0])
      frame->PositionStatusWindow();
  }
  else (void)DefWindowProc(last_msg, last_wparam, last_lparam);

  if (wx_window && wx_window->handle)
    wx_window->GetEventHandler()->OnSize(x, y);
  }
}

BOOL wxMDIFrame::OnCommand(WORD id, WORD cmd, HWND control)
{
#if DEBUG > 1
  wxDebugMsg("wxMDIFrame::OnCommand %d, id = %d\n", handle, id);
#endif
  if (cmd == 0)
  {
    switch (id)
    {
      case IDM_WINDOWCASCADE:
        SendMessage(client_hwnd, WM_MDICASCADE, MDITILE_SKIPDISABLED, 0);
        return TRUE;
      case IDM_WINDOWTILE:
        SendMessage(client_hwnd, WM_MDITILE, MDITILE_HORIZONTAL, 0);
        return TRUE;
      case IDM_WINDOWICONS:
        SendMessage(client_hwnd, WM_MDIICONARRANGE, 0, 0);
        return TRUE;
      case IDM_WINDOWNEXT:
//        SendMessage(client_hwnd, WM_MDINEXT, current_child->handle, 0);
        SendMessage(client_hwnd, WM_MDINEXT, 0, 0);
        return TRUE;
      default:
        break;
     }
    if (id >= 0xF000)
    {
#if DEBUG > 1
      wxDebugMsg("wxMDIFrame::OnCommand %d: system command: calling default window proc\n", handle);
#endif
      return FALSE; // Get WndProc to call default proc
    }
    
    if (parent_frame_active && (id < wxFIRST_MDI_CHILD || id > wxLAST_MDI_CHILD))
    {
      ((wxFrame *)wx_window)->Command(id);
      return TRUE;
    }
    else if (current_child && (id < wxFIRST_MDI_CHILD || id > wxLAST_MDI_CHILD))
    {
/*
      ((wxFrame *)(current_child->wx_window))->Command(id);
      return TRUE;
*/
#if DEBUG > 1
      wxDebugMsg("wxMDIFrame::OnCommand %d: calling child OnCommand\n", handle);
#endif
      return current_child->OnCommand(id, cmd, control);
    }
  }
  return FALSE;
}

void wxMDIFrame::OnMenuSelect(WORD nItem, WORD nFlags, HMENU hSysMenu)
{
  if (parent_frame_active)
  {
    wxFrame *frame = (wxFrame *)wx_window;
    if (nFlags == 0xFFFF && hSysMenu == NULL)
      frame->GetEventHandler()->OnMenuSelect(-1);
    else if (nFlags != MF_SEPARATOR)
      frame->GetEventHandler()->OnMenuSelect(nItem);
  }
  else if (current_child)
  {
    current_child->OnMenuSelect(nItem, nFlags, hSysMenu);
  }
}

long wxMDIFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
#if DEBUG > 1
  wxDebugMsg("wxMDIFrame::DefWindowProc %d, message = %d\n", handle, message);
#endif
  return DefFrameProc(handle, client_hwnd, message, wParam, lParam);
}

BOOL wxMDIFrame::ProcessMessage(MSG* pMsg)
{
#if DEBUG > 1
  wxDebugMsg("wxMDIFrame::ProcessMessage %d\n", handle);
#endif
  if ((current_child != NULL) && (current_child->handle != NULL) && current_child->ProcessMessage(pMsg))
     return TRUE;
	
  if (accelerator_table != NULL &&
          ::TranslateAccelerator(handle, accelerator_table, pMsg))
    return TRUE;
	
  if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
  {
    if (::TranslateMDISysAccel(client_hwnd, pMsg))
      return TRUE;
  }

  return FALSE;
}

BOOL wxMDIFrame::OnEraseBkgnd(HDC WXUNUSED(pDC))
{
  return TRUE;
}

extern wxWnd *wxWndHook;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
extern wxList *wxWinHandleList;
#else
extern wxNonlockingHashTable *wxWinHandleList;
#endif

wxMDIChild::wxMDIChild(wxMDIFrame *parent, wxWindow *wx_win, char *title,
                   int x, int y, int width, int height, long style)
{
  defaultIcon = (wxSTD_MDICHILDFRAME_ICON ? wxSTD_MDICHILDFRAME_ICON : wxDEFAULT_MDICHILDFRAME_ICON);
  icon = NULL;
  iconized = FALSE;
  wx_window = wx_win;
  active = FALSE;
  is_dialog = FALSE;

  wxWndHook = this;

  MDICREATESTRUCT mcs;
	
  mcs.szClass = wxMDIChildFrameClassName;
  mcs.szTitle = title;
  mcs.hOwner = wxhInstance;
  if (x > -1) mcs.x = x;
  else mcs.x = CW_USEDEFAULT;

  if (y > -1) mcs.y = y;
  else mcs.y = CW_USEDEFAULT;

  if (width > -1) mcs.cx = width;
  else mcs.cx = CW_USEDEFAULT;

  if (height > -1) mcs.cy = height;
  else mcs.cy = CW_USEDEFAULT;

  DWORD msflags = WS_OVERLAPPED | WS_CLIPCHILDREN ;
  if (style & wxMINIMIZE_BOX)
    msflags |= WS_MINIMIZEBOX;
  if (style & wxMAXIMIZE_BOX)
    msflags |= WS_MAXIMIZEBOX;
  if (style & wxTHICK_FRAME)
    msflags |= WS_THICKFRAME;
  if (style & wxSYSTEM_MENU)
    msflags |= WS_SYSMENU;
  if ((style & wxMINIMIZE) || (style & wxICONIZE))
    msflags |= WS_MINIMIZE;
  if (style & wxMAXIMIZE)
    msflags |= WS_MAXIMIZE;
  if (style & wxCAPTION)
    msflags |= WS_CAPTION;

  mcs.style = msflags;

  mcs.lParam = 0;

  DWORD Return = SendMessage(parent->client_hwnd,
		WM_MDICREATE, 0, (LONG)(LPSTR)&mcs);

  //handle = (HWND)LOWORD(Return);
  // Must be the DWORRD for WIN32. And in 16 bits, HIWORD=0 (says Microsoft)
  handle = (HWND)Return;

  wxWndHook = NULL;
  wxWinHandleList->Append((long)handle, this);

  SetWindowLong(handle, 0, (long)this);
#if DEBUG > 1
  wxDebugMsg("End of wxMDIChild::wxMDIChild %d\n", handle);
#endif
}

static HWND invalidHandle = 0;
void wxMDIChild::OnSize(int x, int y, UINT id)
{
#if DEBUG > 1
  wxDebugMsg("wxMDIChild::OnSize %d\n", handle);
#endif
  if (!handle) return;

  if (invalidHandle == handle)
  {
#if DEBUG > 1
  wxDebugMsg("wxMDIChild::OnSize %d: invalid, so returning.\n", handle);
#endif
    return;
  }
  
#if DEBUG > 1
  wxDebugMsg("wxMDIChild::OnSize %d: calling DefWindowProc\n", handle);
#endif
  (void)DefWindowProc(last_msg, last_wparam, last_lparam);
#if DEBUG > 1
  wxDebugMsg("wxMDIChild::OnSize %d: called DefWindowProc\n", handle);
#endif
  
  switch (id)
  {
    case SIZEFULLSCREEN:
    case SIZENORMAL:
      iconized = FALSE;
    break;
    case SIZEICONIC:
      iconized = TRUE;
    break;
  }

 if (!iconized)
 {
  wxFrame *frame = (wxFrame *)wx_window;
  if (frame && frame->status_window[0])
    frame->PositionStatusWindow();

  if (wx_window && wx_window->handle)
    wx_window->GetEventHandler()->OnSize(x, y);
 }
}

BOOL wxMDIChild::OnClose(void)
{
#if DEBUG > 1
  wxDebugMsg("wxMDIChild::OnClose %d\n", handle);
#endif
  if (wx_window && handle)
  {
    if (wx_window->GetEventHandler()->OnClose())
    {
      delete wx_window;
      return TRUE;
    } else return FALSE;
  }
  return FALSE;
}

BOOL wxMDIChild::OnCommand(WORD id, WORD cmd, HWND control)
{
#if DEBUG > 1
  wxDebugMsg("wxMDIChild::OnCommand %d\n", handle);
#endif
//  (void)DefWindowProc(last_msg, last_wparam, last_lparam);
  if ((cmd == 0) && handle)
  {
    // In case it's e.g. a toolbar.
    wxWindow *win = wxFindControlFromHandle(control);
    if (win)
      return win->MSWCommand(cmd, id);

    ((wxFrame *)wx_window)->Command(id);
    return TRUE;
  }
  else
    return FALSE;
}

long wxMDIChild::DefWindowProc(UINT message, UINT wParam, LPARAM lParam)
{
  if (handle)
    return DefMDIChildProc(handle, message, wParam, lParam);
  else return 0;
}

BOOL wxMDIChild::ProcessMessage(MSG *msg)
{
  if (accelerator_table && handle)
  {
    wxFrame *parent = (wxFrame *)wx_window->GetParent();
    HWND parent_hwnd = parent->GetHWND();
    return ::TranslateAccelerator(parent_hwnd, accelerator_table, msg);
  }
  return FALSE;
}

BOOL wxMDIChild::OnMDIActivate(BOOL bActivate, HWND WXUNUSED(one), HWND WXUNUSED(two))
{
  wxFrame *parent = (wxFrame *)wx_window->GetParent();
  wxFrame *child = (wxFrame *)wx_window;
  HMENU parent_menu = parent->GetWinMenu();
#if DEBUG > 1
  wxDebugMsg("Parent menu is %d\n", parent_menu);
#endif
  HMENU child_menu = child->GetWinMenu();
#if DEBUG > 1
  wxDebugMsg("Child menu is %d\n", child_menu);
#endif

  wxMDIFrame *cparent = (wxMDIFrame *)parent->handle;
  if (bActivate)
  {
    active = TRUE;
    cparent->current_child = this;
    if (child_menu)
    {
      cparent->parent_frame_active = FALSE;
      HMENU subMenu = GetSubMenu(cparent->window_menu, 0);
#if DEBUG > 1
      wxDebugMsg("Window submenu is %d\n", subMenu);
#endif
//      HMENU subMenu = 0;
#ifdef WIN32
      ::SendMessage(cparent->client_hwnd, WM_MDISETMENU,
                    (WPARAM)child_menu,
                    (LPARAM)subMenu);
#else
      ::SendMessage(cparent->client_hwnd, WM_MDISETMENU, 0,
                  MAKELONG(child_menu, subMenu));
#endif

      ::DrawMenuBar(cparent->handle);
    }
    if (child)
      child->GetEventHandler()->OnActivate(TRUE);
  }
  else
  {
    if (cparent->current_child == this)
      cparent->current_child = NULL;
    if (child)
      child->GetEventHandler()->OnActivate(FALSE);

    active = FALSE;
    if (parent_menu)
    {
      cparent->parent_frame_active = TRUE;
      HMENU subMenu = GetSubMenu(cparent->window_menu, 0);
#if DEBUG > 1
      wxDebugMsg("Window submenu is %d\n", subMenu);
#endif
//      HMENU subMenu = 0;
#ifdef WIN32
      ::SendMessage(cparent->client_hwnd, WM_MDISETMENU,
                    (WPARAM)parent_menu,
                    (LPARAM)subMenu);
#else
      ::SendMessage(cparent->client_hwnd, WM_MDISETMENU, 0,
                  MAKELONG(parent_menu, subMenu));
#endif

      ::DrawMenuBar(cparent->handle);
    }
  }
  wx_window->GetEventHandler()->OnActivate(bActivate);
#if DEBUG > 1
  wxDebugMsg("Finished (de)activating\n");
#endif
  return 0;
}

wxMDIChild::~wxMDIChild(void)
{
}

void wxMDIChild::DestroyWindow(void)
{
#if DEBUG > 1
  wxDebugMsg("Start of wxMDIChild::DestroyWindow %d\n", handle);
#endif
  DetachWindowMenu();
  invalidHandle = handle;

  wxFrame *parent = (wxFrame *)wx_window->GetParent();
  wxMDIFrame *cparent = (wxMDIFrame *)parent->handle;

  // Must make sure this handle is invalidated (set to NULL)
  // since all sorts of things could happen after the
  // child client is destroyed, but before the wxFrame is
  // destroyed.

  HWND oldHandle = (HWND)handle;
#if DEBUG > 1
  wxDebugMsg("*** About to DestroyWindow MDI child %d\n", oldHandle);
#endif
#ifdef WIN32
  SendMessage(cparent->client_hwnd, WM_MDIDESTROY, (WPARAM)oldHandle, (LPARAM)0);
#else
  SendMessage(cparent->client_hwnd, WM_MDIDESTROY, (HWND)oldHandle, 0);
#endif
#if DEBUG > 1
  wxDebugMsg("*** Finished DestroyWindow MDI child %d\n", oldHandle);
#endif
  invalidHandle = 0;

  if (hMenu)
  {
    ::DestroyMenu(hMenu);
    hMenu = 0;
  }
}

/*
BOOL wxFrameWnd::OnEraseBkgnd (HDC pDC)
{
  if (background_brush)
    {
    RECT rect;

    ::GetClipBox(pDC, &rect);
    int mode = SetMapMode(pDC, MM_TEXT);
    ::FillRect(pDC, &rect, background_brush);
    ::SetMapMode(pDC, mode);

      return TRUE;
    }
  else
    return FALSE;
}
*/
