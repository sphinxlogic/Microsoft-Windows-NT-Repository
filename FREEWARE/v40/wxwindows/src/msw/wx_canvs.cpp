/*
 * File:        wx_canvs.cc
 * Purpose:     wxCanvas implementation (MSW)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_canvs.cc,v 1.1 1994/08/14 21:59:17 edz Exp $
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
	Last change:  JS   26 May 97    3:35 pm
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_frame.h"
#include "wx_dc.h"
#include "wx_canvs.h"
#include "wx_panel.h"
#include "wx_stdev.h"
#include "wx_utils.h"
#endif

#include "wx_privt.h"
#include <math.h>

extern char wxCanvasClassName[];

wxCanvas::wxCanvas (void)
{
  is_retained = 0;
  wxWinType = wxTYPE_XWND;
  is_retained = FALSE;
  clipping = FALSE;
  handle = NULL;
  window_parent = NULL;
  horiz_units = 0;
  vert_units = 0;
  wx_dc = NULL;
}

wxCanvas::wxCanvas (wxWindow *parent, int x, int y, int width, int height, long style,
	  Constdata char *name):
wxbCanvas (parent, x, y, width, height, style, name)
{
  Create (parent, x, y, width, height, style, name);
}

Bool wxCanvas::
Create (wxWindow * parent, int x, int y, int width, int height, long style,
	Constdata char *name)
{
  SetName(name);

  is_retained = ((style & wxRETAINED) == wxRETAINED);

  wxWinType = wxTYPE_XWND;
  windowStyle = style;
  is_retained = FALSE;
  clipping = FALSE;
  wxWnd *cparent = NULL;
  if (parent)
    cparent = (wxWnd *) parent->handle;

  if (parent->IsKindOf(CLASSINFO(wxPanel)))
    ((wxPanel *)parent)->GetValidPosition(&x, &y);

  DWORD msflags = 0;
  if (style & wxBORDER)
    msflags |= WS_BORDER;
  if (style & wxTHICK_FRAME)
    msflags |= WS_THICKFRAME;
  msflags |= WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;;

  wxCanvasWnd *wnd = new wxCanvasWnd (cparent, wxCanvasClassName, this, x, y, width, height, msflags);
  wnd->SetBackgroundBrush(GetStockObject(WHITE_BRUSH), FALSE);
  wnd->background_colour = RGB(255, 255, 255);
#if DEBUG > 1
  wxDebugMsg ("wxCanvas::Create: stock white background_brush is %X\n", wnd->background_brush);
#endif
  handle = (char *) wnd;

  if (parent)
    parent->AddChild (this);
  window_parent = parent;

  horiz_units = 0;
  vert_units = 0;

  wx_dc = new wxCanvasDC (this);

  if (parent->IsKindOf(CLASSINFO(wxPanel)))
    ((wxPanel *)parent)->AdvanceCursor(this);

  return TRUE;
}

wxCanvas::~wxCanvas (void)
{
#if DEBUG > 1
  wxDebugMsg("About to select old objects for canvas dc %X\n", wx_dc);
#endif
  // NB THIS CODE RELIES on destructor ordering:
  // this must be called BEFORE the window is destroyed.
  // This may not be true for some compilers (Borland?)
  // in which case we need to do something messy in ~wxWindow
  // to call this code BEFORE we destroy the window (or how
  // do we get hold of the DC?)
    
  if (wx_dc)
  {
    HWND hWnd = GetHWND();
    HDC dc = ::GetDC(hWnd);
    wx_dc->SelectOldObjects (dc);
    ReleaseDC(hWnd, dc);
    delete wx_dc;
  }
}

void wxCanvas::SetColourMap (wxColourMap * cmap)
{
  if (wx_dc)
    wx_dc->SetColourMap(cmap);
}

void wxCanvas::SetSize (int x, int y, int w, int h, int sizeFlags)
{
  int currentX, currentY;
  GetPosition (&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  int ww, hh;
  GetSize (&ww, &hh);
  if (w == -1)
    w = ww;
  if (h == -1)
    h = hh;

  if (GetHWND())
    {
      MoveWindow (GetHWND(), x, y, w, h, TRUE);
//      GetEventHandler()->OnSize (w, h);
    }
}

/*
 * horizontal/vertical: number of pixels per unit (e.g. pixels per text line)
 * x/y_length:        : no. units per scrollbar
 * x/y_page:          : no. units per page scrolled
 */
void wxCanvas::
SetScrollbars (int horizontal, int vertical,
	       int x_length, int y_length,
	       int x_page, int y_page,
	       int x_pos, int y_pos, Bool setVirtualSize)
{
  if (wxWinType != wxTYPE_XWND)
    return;

  horiz_units = horizontal;
  vert_units = vertical;

  wxWnd *wnd = (wxWnd *) handle;
  if (wnd)
    {
      wnd->xscroll_pixels_per_line = horizontal;
      wnd->yscroll_pixels_per_line = vertical;
      wnd->xscroll_lines = x_length;
      wnd->yscroll_lines = y_length;
      wnd->xscroll_lines_per_page = x_page;
      wnd->yscroll_lines_per_page = y_page;
      wnd->calcScrolledOffset = setVirtualSize;
      wnd->xscroll_position = x_pos;
      wnd->yscroll_position = y_pos;

      int w, h;
      RECT rect;
      GetWindowRect (wnd->handle, &rect);
      w = rect.right - rect.left;
      h = rect.bottom - rect.top;

      AdjustScrollbars();
      AdjustScrollbars(); // Bug in Windows?

#if 0
      // Recalculate scroll bar range and position
      if (wnd->xscroll_lines > 0)
	{
	  int nMaxWidth = wnd->xscroll_lines * wnd->xscroll_pixels_per_line;
	  int nHscrollMax = wxMax (0, (int) (2 + (nMaxWidth - w) / wnd->xscroll_pixels_per_line));
	  wnd->xscroll_position = x_pos;
	  wnd->xscroll_position = wxMin (nHscrollMax, wnd->xscroll_position);

	  ::SetScrollRange (wnd->handle, SB_HORZ, 0, nHscrollMax, FALSE);
	  ::SetScrollPos (wnd->handle, SB_HORZ, wnd->xscroll_position, TRUE);
	}
        else
        {
	  ::SetScrollRange (wnd->handle, SB_HORZ, 0, 0, FALSE);
          wnd->xscroll_position = 0;
        }

        if (wnd->yscroll_lines > 0)
	{
	  int nMaxHeight = wnd->yscroll_lines * wnd->yscroll_pixels_per_line;
	  int nVscrollMax = wxMax (0, (int) (2 + (nMaxHeight - h) / wnd->yscroll_pixels_per_line));
	  wnd->yscroll_position = y_pos;
	  wnd->yscroll_position = wxMin (nVscrollMax, wnd->yscroll_position);

	  ::SetScrollRange (wnd->handle, SB_VERT, 0, nVscrollMax, FALSE);
	  ::SetScrollPos (wnd->handle, SB_VERT, wnd->yscroll_position, TRUE);
	}
        else
        {
	  ::SetScrollRange (wnd->handle, SB_VERT, 0, 0, FALSE);
          wnd->yscroll_position = 0;
        }
#endif

      InvalidateRect (wnd->handle, NULL, TRUE);
      UpdateWindow (wnd->handle);
    }
}

void wxCanvas::GetScrollUnitsPerPage (int *x_page, int *y_page)
{
  if (wxWinType != wxTYPE_XWND)
    return;

  wxWnd *wnd = (wxWnd *) handle;
  if (wnd)
    {
      *x_page = wnd->xscroll_lines_per_page;
      *y_page = wnd->yscroll_lines_per_page;
    }
}

void wxCanvas::GetScrollPixelsPerUnit (int *x_unit, int *y_unit)
{
  if (wxWinType != wxTYPE_XWND)
    return;

  wxWnd *wnd = (wxWnd *) handle;
  if (wnd)
    {
      *x_unit = wnd->xscroll_pixels_per_line;
      *y_unit = wnd->yscroll_pixels_per_line;
    }
}

/*
 * Scroll to given position (scroll position, not pixel position)
 */
void wxCanvas::Scroll (int x_pos, int y_pos)
{
  if (wxWinType != wxTYPE_XWND)
    return;

  int old_x, old_y;
  ViewStart (&old_x, &old_y);
  if (((x_pos == -1) || (x_pos == old_x)) && ((y_pos == -1) || (y_pos == old_y)))
    return;

  wxWnd *wnd = (wxWnd *) handle;

  if (x_pos > -1)
    {
      wnd->xscroll_position = x_pos;
      ::SetScrollPos (wnd->handle, SB_HORZ, x_pos, TRUE);
    }
  if (y_pos > -1)
    {
      wnd->yscroll_position = y_pos;
      ::SetScrollPos (wnd->handle, SB_VERT, y_pos, TRUE);
    }
  InvalidateRect (wnd->handle, NULL, TRUE);
  UpdateWindow (wnd->handle);
}

void wxCanvas::EnableScrolling (Bool x_scroll, Bool y_scroll)
{
  if (wxWinType != wxTYPE_XWND)
    return;

  wxWnd *wnd = (wxWnd *) handle;
  wnd->x_scrolling_enabled = x_scroll;
  wnd->y_scrolling_enabled = y_scroll;
}

void wxCanvas::GetVirtualSize (int *x, int *y)
{
  if (wxWinType != wxTYPE_XWND)
    return;

  wxWnd *wnd = (wxWnd *) handle;
  if (wnd)
    {
      *x = wnd->xscroll_pixels_per_line * wnd->xscroll_lines;
      *y = wnd->yscroll_pixels_per_line * wnd->yscroll_lines;
    }
}

void wxCanvas::WarpPointer (int x_pos, int y_pos)
{
  if (wxWinType != wxTYPE_XWND)
    return;

  // Move the pointer to (x_pos,y_pos) coordinates. They are expressed in
  // pixel coordinates, relatives to the canvas -- So, we first need to
  // substract origin of the window, then convert to screen position

  wxWnd *wnd = (wxWnd *) handle;

  if (wnd)
    {
      x_pos -= wnd->xscroll_position * wnd->xscroll_pixels_per_line;
      y_pos -= wnd->yscroll_position * wnd->yscroll_pixels_per_line;

      RECT rect;
      GetWindowRect (wnd->handle, &rect);

      x_pos += rect.left;
      y_pos += rect.top;

      SetCursorPos (x_pos, y_pos);
    }
}

// Where the current view starts from
void wxCanvas::ViewStart (int *x, int *y)
{
  if (wxWinType != wxTYPE_XWND)
    return;

  wxWnd *wnd = (wxWnd *) handle;
  *x = wnd->xscroll_position;
  *y = wnd->yscroll_position;
}

void wxWnd::DeviceToLogical (float *x, float *y)
{
  if (is_canvas)
    {
      wxCanvas *canvas = (wxCanvas *) wx_window;
      if (canvas->wx_dc)
      {
        *x = canvas->wx_dc->DeviceToLogicalX ((int) *x);
        *y = canvas->wx_dc->DeviceToLogicalY ((int) *y);
      }
    }
}

wxCanvasWnd::wxCanvasWnd (void)
{
  is_canvas = TRUE;
}

wxCanvasWnd::wxCanvasWnd (wxWnd * parent, char *winClass, wxWindow * wx_win,
	     int x, int y, int width, int height, DWORD style):
wxSubWnd (parent, winClass, wx_win, x, y, width, height, style)
{
  is_canvas = TRUE;
//  ShowScrollBar(handle, SB_BOTH, TRUE);
}


BOOL wxCanvasWnd::OnEraseBkgnd (HDC pDC)
{
  if (background_brush)
    {
/*
    RECT rect;

    ::GetClipBox(pDC, &rect);
    int mode = SetMapMode(pDC, MM_TEXT);
    ::FillRect(pDC, &rect, background_brush);
    ::SetMapMode(pDC, mode);
*/

//    wxDebugMsg("Clip box: left = %d, top = %d, right = %d, bottom = %d\n", rect.left,
//        rect.top, rect.right, rect.bottom);
      RECT rect;
      GetClientRect (handle, &rect);
      int mode = SetMapMode (pDC, MM_TEXT);
      FillRect (pDC, &rect, background_brush);
      SetMapMode (pDC, mode);

      wxCanvas *canvas = (wxCanvas *) wx_window;
      SetViewportExtEx (pDC, VIEWPORT_EXTENT, VIEWPORT_EXTENT, NULL);
      SetWindowExtEx (pDC, canvas->wx_dc->window_ext_x, canvas->wx_dc->window_ext_y, NULL);

      return TRUE;
    }
  else
  {
    return FALSE;
  }
}

// OnSize resets scrollbars
void wxCanvas::OnSize(int w, int h)
{
  if (inOnSize)
    return;
    
#if USE_CONSTRAINTS
  if (GetAutoLayout())
    Layout();
#endif

  inOnSize = TRUE;

  AdjustScrollbars();
  AdjustScrollbars(); // Bug in Windows?

  inOnSize = FALSE;
}

void wxCanvas::AdjustScrollbars(void)
{
  int w, h;
  GetClientSize(&w, &h);

  wxWnd *wnd = (wxWnd *)handle;

  HWND hWnd = GetHWND();

  // Recalculate scroll bar range and position
  if (wnd->xscroll_lines > 0)
  {
    int nMaxWidth = wnd->xscroll_lines*wnd->xscroll_pixels_per_line;
    int noPositions = (int) ( ((nMaxWidth - w)/(float)wnd->xscroll_pixels_per_line) + 0.5 );
    if (noPositions < 0)
      noPositions = 0;

    wnd->xscroll_position = wxMin(noPositions, wnd->xscroll_position);

    // Calculate page size i.e. number of scroll units you get on the
    // current client window
    int noPagePositions = (int) ( (w/(float)wnd->xscroll_pixels_per_line) + 0.5 );
    if (noPagePositions < 1)
      noPagePositions = 1;

    SetScrollPage(wxHORIZONTAL, noPagePositions);
    SetScrollRange(wxHORIZONTAL, noPositions);
    SetScrollPos(wxHORIZONTAL, wnd->xscroll_position);
/*
    int nMaxWidth = wnd->xscroll_lines*wnd->xscroll_pixels_per_line;
    float noPositions = (float)((nMaxWidth - w)/(float)wnd->xscroll_pixels_per_line);
    if (noPositions > 0 && noPositions < 1)
      noPositions = (float)1;
    else if (noPositions < 0)
      noPositions = (float)0;
      
    int nHscrollMax = wxMax(0, (int)(0 + noPositions));
    wnd->xscroll_position = wxMin(nHscrollMax, wnd->xscroll_position);

    ::SetScrollRange(hWnd, SB_HORZ, 0, nHscrollMax, FALSE);
    ::SetScrollPos(hWnd, SB_HORZ, wnd->xscroll_position, TRUE );
*/
  }
  if (wnd->yscroll_lines > 0)
  {
    int nMaxHeight = wnd->yscroll_lines*wnd->yscroll_pixels_per_line;
    int noPositions = (int)( ((nMaxHeight - h)/(float)wnd->yscroll_pixels_per_line) + 0.5);
    if (noPositions < 0)
      noPositions = 0;

    wnd->yscroll_position = wxMin(noPositions, wnd->yscroll_position);

    // Calculate page size i.e. number of scroll units you get on the
    // current client window
    int noPagePositions = (int) ( (h/(float)wnd->yscroll_pixels_per_line) + 0.5 );
    if (noPagePositions < 1)
      noPagePositions = 1;

    SetScrollPage(wxVERTICAL, noPagePositions);
    SetScrollRange(wxVERTICAL, noPositions);
    SetScrollPos(wxVERTICAL, wnd->yscroll_position);
/*
    int nMaxHeight = wnd->yscroll_lines*wnd->yscroll_pixels_per_line;
    float noPositions = (float)((nMaxHeight - h)/(float)wnd->yscroll_pixels_per_line);
    if (noPositions > 0 && noPositions < 1)
      noPositions = (float)1;
    else if (noPositions < 0)
      noPositions = (float)0;

    int nVscrollMax = wxMax(0, (int)(0 + noPositions));
    wnd->yscroll_position = wxMin(nVscrollMax, wnd->yscroll_position);

    ::SetScrollRange(hWnd, SB_VERT, 0, nVscrollMax, FALSE);
    ::SetScrollPos(hWnd, SB_VERT, wnd->yscroll_position, TRUE );
*/
  }
}
