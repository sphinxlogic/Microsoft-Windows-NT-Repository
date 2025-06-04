/*
 * File:	wx_win.cc
 * Purpose:	wxWindow class implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_win.cc,v 1.1 1994/08/14 21:59:17 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx_setup.h"
#include "wx_panel.h"
#include "wx_rbox.h"
#include "wx_txt.h"
#include "wx_messg.h"
#include "wx_check.h"
#include "wx_group.h"
#include "wx_text.h"
#include "wx_slidr.h"
#include "wx_menu.h"
#include "wx_dcpan.h"
#include "wx_utils.h"
#include "wx_main.h"
#endif

#if USE_SCROLLBAR
#include "wx_scrol.h"
#endif

#include "wx_privt.h"
#include "wx_itemp.h"

#ifndef GNUWIN32
#include <shellapi.h>
#endif

/*
#if HAVE_SOCKET
#include "winsock.h"
#include "dde_ipc.h"
#endif
*/

#ifdef WIN32
#include <windowsx.h>
#endif

#if CTL3D
#include <ctl3d.h>
#endif

#if FAFA_LIB
#include "fafa.h"
extern HBRUSH SetupBackground(HWND wnd) ; // in wx_main.cc
#endif

#if USE_ITSY_BITSY
#include "..\..\contrib\itsybits\itsybits.h"
#endif

#if !defined(APIENTRY)	// NT defines APIENTRY, 3.x not
#define APIENTRY FAR PASCAL
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

#if USE_SCROLLBAR
extern void wxScrollBarEvent(HWND hbar, WORD wParam, WORD pos);
#endif

#define WINDOW_MARGIN 3	// This defines sensitivity of Leave events

extern long last_msg_time; /* MATTHEW: timeStamp implementation */

wxMenu *wxCurrentPopupMenu = NULL;

// Find an item given the MS Windows id
wxWindow *wxWindow::FindItem(int id)
{
  if (!children)
    return NULL;
  wxNode *current = children->First();
  while (current)
  {
    wxObject *obj = (wxObject *)current->Data() ;
    if (obj->IsKindOf(CLASSINFO(wxPanel)))
    {
      // Do a recursive search.
      wxPanel *panel = (wxPanel*)obj ;
      wxWindow *wnd = panel->FindItem(id) ;
      if (wnd)
        return wnd ;
    }
    else
    {
      wxWindow *childWin = (wxWindow *)current->Data();
      if (childWin->IsKindOf(CLASSINFO(wxItem)))
      {
        wxItem *item = (wxItem *)childWin;
        if (item->windows_id == id)
          return item;
        else
        {
          // In case it's a 'virtual' control (e.g. radiobox)
          if (item->subControls.Member((wxObject *)id))
            return item;
        }
      }
    }
    current = current->Next();
  }
  return NULL;
}

// Find an item given the MS Windows handle
wxWindow *wxWindow::FindItemByHWND(HWND hWnd)
{
  if (!children)
    return NULL;
  wxNode *current = children->First();
  while (current)
  {
    wxObject *obj = (wxObject *)current->Data() ;
    if (obj->IsKindOf(CLASSINFO(wxPanel)))
    {
      // Do a recursive search.
      wxPanel *panel = (wxPanel*)obj ;
      wxWindow *wnd = panel->FindItemByHWND(hWnd) ;
      if (wnd)
        return wnd ;
    }
    else if (obj->IsKindOf(CLASSINFO(wxItem)))
    {
      wxItem *item = (wxItem *)current->Data();
      if ((HWND)(item->ms_handle) == hWnd)
        return item;
      else
      {
        // In case it's a 'virtual' control (e.g. radiobox)
        if (item->IsKindOf(CLASSINFO(wxRadioBox)))
        {
          wxRadioBox *rbox = (wxRadioBox *)item;
          int i;
          for (i = 0; i < rbox->no_items; i++)
            if (rbox->radioButtons[i] == hWnd)
              return item;
        }
      }
    }
    current = current->Next();
  }
  return NULL;
}

// Find an item given the static label
wxWindow *wxWindow::FindItemByStatic(HWND hWnd)
{
  if (!children)
    return NULL;
  wxNode *current = children->First();
  while (current)
  {
    wxObject *obj = (wxObject *)current->Data() ;
    if (obj->IsKindOf(CLASSINFO(wxPanel)))
    {
      // Do a recursive search.
      wxPanel *panel = (wxPanel*)obj ;
      wxWindow *wnd = panel->FindItemByStatic(hWnd) ;
      if (wnd)
        return wnd ;
    }
    else if (obj->IsKindOf(CLASSINFO(wxItem)))
    {
      wxItem *item = (wxItem *)current->Data();
      if ((HWND)(item->GetLabelHWND()) == hWnd)
        return item;
      else
      {
        if (item->IsKindOf(CLASSINFO(wxSlider)))
        {
          wxSlider *slider = (wxSlider *)item;
          if (hWnd == slider->static_min || hWnd == slider->static_max || hWnd == slider->edit_value)
            return slider;
        }
      }
    }
    current = current->Next();
  }
  return NULL;
}

// Default command handler
BOOL wxWindow::MSWCommand(UINT WXUNUSED(param), WORD WXUNUSED(id))
{
  return FALSE;
}

BOOL wxWindow::MSWNotify(WPARAM WXUNUSED(wParam), LPARAM WXUNUSED(lParam))
{
  return FALSE;
}

void wxWindow::PreDelete(HDC WXUNUSED(dc))
{
  mouseInWindow = FALSE ;
}

HWND wxWindow::GetHWND(void)
{
  HWND hWnd = 0;

  switch (wxWinType)
  {
    case wxTYPE_XWND:
    case wxTYPE_MDICHILD:
    {
      wxWnd *wnd = (wxWnd *)handle;
      hWnd = (HWND)wnd->handle;
      break;
    }
    default:
    {
      hWnd = (HWND)ms_handle;
      break;
    }
  }
  return hWnd;
}

// Constructor
wxWindow::wxWindow(void)
{
  ms_handle = 0;
  handle = NULL;
  mouseInWindow = FALSE ;
  winEnabled = TRUE;
  cxChar = 0; cyChar = 0;
  windows_id = 0;
  mouseInWindow = FALSE;
  winEnabled = TRUE;
  caretWidth = 0; caretHeight = 0;
  caretEnabled = FALSE;
  caretShown = FALSE;
  inOnSize = FALSE;
  minSizeX = -1;
  minSizeY = -1;
  maxSizeX = -1;
  maxSizeY = -1;
  paintHDC = 0;
  tempHDC = 0;
  countHDC = 0;
  oldWndProc = 0;
#ifndef WIN32
  globalHandle = 0;
#endif
}

// Destructor
wxWindow::~wxWindow(void)
{
  if (window_parent)
    window_parent->RemoveChild(this);

  wxbWindow::DestroyChildren();
  switch (wxWinType)
  {
    case wxTYPE_XWND:
    {
      if (handle)
      {
        wxWnd *wnd = (wxWnd *)handle;
        HDC dc = GetDC(wnd->handle);
        PreDelete(dc);
        ReleaseDC(wnd->handle, dc);

        wnd->DestroyWindow();
        delete wnd;
        handle = NULL;
      }
      break;
    }
    case wxTYPE_MDICHILD:
    {
      wxMDIChild *child = (wxMDIChild *)handle;
      child->DestroyWindow();
      delete child;
      handle = NULL;
      break;
    }
    case wxTYPE_HWND:
    {
      if (ms_handle)
        DestroyWindow((HWND)ms_handle);
      handle = NULL;
#ifndef WIN32
      if (globalHandle)
      {
        GlobalFree(globalHandle);
        globalHandle = 0;
      }
#endif
      if (wxControlHandleList)
        wxControlHandleList->DeleteObject(this);

      break;
    }
    default:
      break;
    }

  delete children;
  children = NULL;
}

void wxWindow::SetFocus(void)
{
  HWND hWnd = GetHWND();
  if (hWnd)
    ::SetFocus(hWnd);
}

void wxWindow::Enable(Bool enable)
{
  winEnabled = enable;
  HWND hWnd = GetHWND();
  if (hWnd)
    ::EnableWindow(hWnd, (BOOL)enable);
}

void wxWindow::CaptureMouse(void)
{
  HWND hWnd = GetHWND();
  if (hWnd && !winCaptured)
  {
    SetCapture(hWnd);
    winCaptured = TRUE;
  }
}

void wxWindow::ReleaseMouse(void)
{
  if (winCaptured)
  {
    ReleaseCapture();
    winCaptured = FALSE;
  }
}

void wxWindow::DragAcceptFiles(Bool accept)
{
#ifndef GNUWIN32
  HWND hWnd = GetHWND();
  if (hWnd)
    ::DragAcceptFiles(hWnd, (BOOL)accept);
#endif
}

// Get total size
void wxWindow::GetSize(int *x, int *y)
{
  HWND hWnd = GetHWND();
  RECT rect;
  GetWindowRect(hWnd, &rect);
  *x = rect.right - rect.left;
  *y = rect.bottom - rect.top;
}

void wxWindow::GetPosition(int *x, int *y)
{
  HWND hWnd = GetHWND();
  HWND hParentWnd = 0;
  if (GetParent())
    hParentWnd = GetParent()->GetHWND();
  
  RECT rect;
  GetWindowRect(hWnd, &rect);

  // Since we now have the absolute screen coords,
  // if there's a parent we must subtract its top left corner
  POINT point;
  point.x = rect.left;
  point.y = rect.top;
  if (hParentWnd)
  {
    ::ScreenToClient(hParentWnd, &point);
  }
  *x = point.x;
  *y = point.y;
}

void wxWindow::ScreenToClient(int *x, int *y)
{
  HWND hWnd = GetHWND();
  POINT pt;
  pt.x = *x;
  pt.y = *y;
  ::ScreenToClient(hWnd, &pt);

  *x = pt.x;
  *y = pt.y;
}

void wxWindow::ClientToScreen(int *x, int *y)
{
  HWND hWnd = GetHWND();
  POINT pt;
  pt.x = *x;
  pt.y = *y;
  ::ClientToScreen(hWnd, &pt);

  *x = pt.x;
  *y = pt.y;
}

wxCursor *wxWindow::SetCursor(wxCursor *cursor)
{
  wxCursor *old_cursor = wx_cursor;
  wx_cursor = cursor;
  if (wx_cursor)
  {
    HWND hWnd = GetHWND();

    // Change the cursor NOW if we're within the correct window
    POINT point;
    ::GetCursorPos(&point);

    RECT rect;
    ::GetWindowRect(hWnd, &rect);

    if (::PtInRect(&rect, point) && !wxIsBusy())
      ::SetCursor(wx_cursor->ms_cursor);
  }

  // This will cause big reentrancy problems if wxFlushEvents is implemented.
//  wxFlushEvents();
  return old_cursor;
}

void wxWindow::SetColourMap(wxColourMap *WXUNUSED(cmap))
{
}

// Get size *available for subwindows* i.e. excluding menu bar etc.
// For XView, this is the same as GetSize
void wxWindow::GetClientSize(int *x, int *y)
{
  HWND hWnd = GetHWND();
  RECT rect;
  GetClientRect(hWnd, &rect);
  *x = rect.right - rect.left;
  *y = rect.bottom - rect.top;
}

void wxWindow::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  int actualWidth = width;
  int actualHeight = height;
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  int currentW,currentH;
  GetSize(&currentW, &currentH);
  if (width == -1)
    actualWidth = currentW ;
  if (height == -1)
    actualHeight = currentH ;

  HWND hWnd = GetHWND();
  if (hWnd)
    MoveWindow(hWnd, x, y, actualWidth, actualHeight, (BOOL)TRUE);

  if (!(width == -1) && (height == -1))
    GetEventHandler()->OnSize(width, height);
}

void wxWindow::SetClientSize(int width, int height)
{
  wxWindow *parent = GetParent();
  HWND hWnd = GetHWND();
  HWND hParentWnd = parent->GetHWND();

  RECT rect;
  GetClientRect(hWnd, &rect);

  RECT rect2;
  GetWindowRect(hWnd, &rect2);

  // Find the difference between the entire window (title bar and all)
  // and the client area; add this to the new client size to move the
  // window
  int actual_width = rect2.right - rect2.left - rect.right + width;
  int actual_height = rect2.bottom - rect2.top - rect.bottom + height;

  // If there's a parent, must subtract the parent's top left corner
  // since MoveWindow moves relative to the parent

  POINT point;
  point.x = rect2.left;
  point.y = rect2.top;
  if (parent)
  {
    ::ScreenToClient(hParentWnd, &point);
  }

  MoveWindow(hWnd, point.x, point.y, actual_width, actual_height, (BOOL)TRUE);
  GetEventHandler()->OnSize(actual_width, actual_height);
}

Bool wxWindow::Show(Bool show)
{
  HWND hWnd = GetHWND();
  int cshow;
  if (show)
    cshow = SW_SHOW;
  else
    cshow = SW_HIDE;
  ShowWindow(hWnd, (BOOL)cshow);
  if (show)
  {
    BringWindowToTop(hWnd);
    // Next line causes a crash on NT, apparently.
//    UpdateWindow(hWnd); // Should this be here or will it cause inefficiency?
  }
  return TRUE;
}

Bool wxWindow::IsShown(void)
{
  return (Bool)::IsWindowVisible(GetHWND());
}

float wxWindow::GetCharHeight(void)
{
  TEXTMETRIC lpTextMetric;
  HWND hWnd = GetHWND();
  HDC dc = GetDC(hWnd);

  GetTextMetrics(dc, &lpTextMetric);
  ReleaseDC(hWnd, dc);

  return (float)lpTextMetric.tmHeight;
}

float wxWindow::GetCharWidth(void)
{
  TEXTMETRIC lpTextMetric;
  HWND hWnd = GetHWND();
  HDC dc = GetDC(hWnd);

  GetTextMetrics(dc, &lpTextMetric);
  ReleaseDC(hWnd, dc);

  return (float)lpTextMetric.tmAveCharWidth;
}

void wxWindow::GetTextExtent(const char *string, float *x, float *y,
                           float *descent, float *externalLeading, wxFont *theFont, Bool)
{
  wxFont *fontToUse = theFont;
  if (!fontToUse)
    fontToUse = font;

  HWND hWnd = GetHWND();
  HDC dc = GetDC(hWnd);

  HFONT fnt = 0; 
  HFONT was = 0;
  if (fontToUse)
  {
    fontToUse->UseResource();
    
    fontToUse->RealizeResource();
    if ((fnt=fontToUse->GetResourceHandle()))
      was = SelectObject(dc,fnt) ;
  }

  SIZE sizeRect;
  TEXTMETRIC tm;
  GetTextExtentPoint(dc, string, (int)strlen(string), &sizeRect);
  GetTextMetrics(dc, &tm);

  if (fontToUse && fnt && was) 
    SelectObject(dc,was) ; 

  ReleaseDC(hWnd, dc);

  *x = (float)sizeRect.cx;
  *y = (float)sizeRect.cy;
  if (descent) *descent = (float)tm.tmDescent;
  if (externalLeading) *externalLeading = (float)tm.tmExternalLeading;

  if (fontToUse)
    fontToUse->ReleaseResource();
}

void wxWindow::Refresh(Bool eraseBack, wxRectangle *rect)
{
  HWND hWnd = GetHWND();
  if (hWnd)
  {
    if (rect)
    {
      RECT mswRect;
      mswRect.left = rect->x;
      mswRect.top = rect->y;
      mswRect.right = rect->x + rect->width;
      mswRect.bottom = rect->y + rect->height;
      
      ::InvalidateRect(hWnd, &mswRect, eraseBack);
    }
    else
      ::InvalidateRect(hWnd, NULL, eraseBack);
  }
}

HDC wxWindow::GetHDC(void)
{
  if (paintHDC)
    return(paintHDC) ;
  if (countHDC==0)
    tempHDC = ::GetDC(GetHWND()) ;
  countHDC ++ ;
  return(tempHDC) ;
}

void wxWindow::ReleaseHDC(void)
{
  // We're within an OnPaint: it'll be released.
  if (paintHDC)
    return ;

  // It was only temporary.
  countHDC -- ;
  if (countHDC==0)
    ::ReleaseDC(GetHWND(),tempHDC) ;
  if (countHDC < 0)
    countHDC = 0;
}

// Hook for new window just as it's being created,
// when the window isn't yet associated with the handle
wxWnd *wxWndHook = NULL;

/*
#if HAVE_SOCKET
// DDE Interface Handler
extern	"C" {
  long	ddeWindowProc(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam);
  void __ddeUnblock(HWND hWnd, WPARAM wParam);
};
#endif
*/

// Main Windows 3 window proc
LRESULT APIENTRY _EXPORT wxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  wxWnd *wnd = (wxWnd *)GetWindowLong(hWnd, 0);
  if (!wnd) {
    if (wxWndHook) {
      wnd = wxWndHook;
      wnd->handle = hWnd;
    } else wnd = wxFindWinFromHandle(hWnd);
  }
#if (DEBUG > 1)
  wxDebugMsg("hWnd = %d, wnd->handle = %d, msg = %d\n", hWnd, wnd ? wnd->handle : 0, message);
#endif
  // Stop right here if we don't have a valid handle
  // in our wxWnd object.
  if (wnd && !wnd->handle) {
//    wxDebugMsg("Warning: could not find a valid handle, wx_win.cc/wxWndProc.\n");
    wnd->handle = hWnd;
    LONG res = wnd->DefWindowProc(message, wParam, lParam );
    wnd->handle = NULL;
    return res;
  }

  if (wnd) {
    wnd->last_msg = message;
    wnd->last_wparam = wParam;
    wnd->last_lparam = lParam;
/* Don't know why this was here
    if (message == WM_SETFONT)
      return 0;
    else if (message == WM_INITDIALOG)
      return TRUE;
*/
  }
  if (wnd)
    return wnd->WindowProc(message, wParam, lParam);
  else
    return DefWindowProc( hWnd, message, wParam, lParam );
}

// Should probably have a test for 'genuine' NT
#if defined(WIN32)
#define DIMENSION_TYPE short
#else
#define DIMENSION_TYPE int
#endif

// Main Windows 3 window proc
LONG wxWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  HWND hWnd = (HWND)handle;
  
  switch (message)
  {
        case WM_SETFONT:
        {
          return 0;
        }
        case WM_INITDIALOG:
        {
          return TRUE;
        }
        case WM_ACTIVATE:
        {
#ifdef WIN32
            WORD state = LOWORD(wParam);
            WORD minimized = HIWORD(wParam);
            HWND hwnd = (HWND)lParam;
#else
            WORD state = (WORD)wParam;
            WORD minimized = LOWORD(lParam);
            HWND hwnd = (HWND)HIWORD(lParam);
#endif
            OnActivate(state, minimized, hwnd);
            return 0;
//            if (!OnActivate(state, minimized, hwnd))
//              return DefWindowProc(message, wParam, lParam );
            break;
        }
        case WM_SETFOCUS:
        {
            HWND hwnd = (HWND)wParam;
//            return OnSetFocus(hwnd);

            if (OnSetFocus(hwnd))
              return 0;
            else return DefWindowProc(message, wParam, lParam );
            break;
        }
        case WM_KILLFOCUS:
        {
            HWND hwnd = (HWND)lParam;
//            return OnKillFocus(hwnd);
            if (OnKillFocus(hwnd))
              return 0;
            else
              return DefWindowProc(message, wParam, lParam );
            break;
        }
	case WM_CREATE:
	{
          OnCreate((LPCREATESTRUCT)lParam);
          return 0;
          break;
	}
	case WM_PAINT:
	{
          if (OnPaint())
            return 0;
          else return DefWindowProc(message, wParam, lParam );
          break;
        }
	case WM_QUERYDRAGICON:
	{
	  HICON hIcon = 0;
          if ((hIcon = OnQueryDragIcon()))
            return (LONG)hIcon;
          else return DefWindowProc(message, wParam, lParam );
          break;
        }

        case WM_SIZE:
        {
          int width = LOWORD(lParam);
          int height = HIWORD(lParam);
          OnSize(width, height, wParam);
          break;
        }

        case WM_RBUTTONDOWN:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            OnRButtonDown(x, y, wParam);
            break;
        }
        case WM_RBUTTONUP:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            OnRButtonUp(x, y, wParam);
            break;
        }
        case WM_RBUTTONDBLCLK:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            OnRButtonDClick(x, y, wParam);
            break;
        }
        case WM_MBUTTONDOWN:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            OnMButtonDown(x, y, wParam);
            break;
        }
        case WM_MBUTTONUP:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            OnMButtonUp(x, y, wParam);
            break;
        }
        case WM_MBUTTONDBLCLK:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            OnMButtonDClick(x, y, wParam);
            break;
        }
        case WM_LBUTTONDOWN:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            OnLButtonDown(x, y, wParam);
            break;
        }
        case WM_LBUTTONUP:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            OnLButtonUp(x, y, wParam);
            break;
        }
        case WM_LBUTTONDBLCLK:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            OnLButtonDClick(x, y, wParam);
            break;
        }
        case WM_MOUSEMOVE:
        {
            int x = (DIMENSION_TYPE)LOWORD(lParam);
            int y = (DIMENSION_TYPE)HIWORD(lParam);
            OnMouseMove(x, y, wParam);
            break;
        }
        case WM_DESTROY:
        {
          if (OnDestroy())
            return 0;
          else return DefWindowProc(message, wParam, lParam );
          break;
        }
/*
        case WM_SYSCOMMAND:
            break;
*/
        case WM_COMMAND:
	{
#ifdef WIN32
            WORD id = LOWORD(wParam);
            HWND hwnd = (HWND)lParam;
            WORD cmd = HIWORD(wParam);
#else
            WORD id = (WORD)wParam;
            HWND hwnd = (HWND)LOWORD(lParam) ;
            WORD cmd = HIWORD(lParam);
#endif
            if (!OnCommand(id, cmd, hwnd))
              return DefWindowProc(message, wParam, lParam );
            break;
	 }
#if WIN95
        case WM_NOTIFY:
	{
          if (!OnNotify(wParam, lParam))
            return DefWindowProc(message, wParam, lParam );
          break;
        }
#endif
        case WM_MENUSELECT:
        {
#ifdef WIN32
            WORD flags = HIWORD(wParam);
            HMENU sysmenu = (HMENU)lParam;
#else
            WORD flags = LOWORD(lParam);
            HMENU sysmenu = (HMENU)HIWORD(lParam);
#endif
            OnMenuSelect((WORD)wParam, flags, sysmenu);
            break;
        }
        case WM_INITMENUPOPUP:
        {
            OnInitMenuPopup((HMENU)wParam, (int)LOWORD(lParam), (Bool)HIWORD(lParam));
            break;
        }
        case WM_DRAWITEM:
        {
          return OnDrawItem((int)wParam, (DRAWITEMSTRUCT *)lParam);
          break;
        }
        case WM_MEASUREITEM:
        {
          return OnMeasureItem((int)wParam, (MEASUREITEMSTRUCT *)lParam);
          break;
        }
        case WM_KEYDOWN:
        {
            if (wParam == VK_SHIFT)
              return DefWindowProc(message, wParam, lParam );
//              wxShiftDown = TRUE;
            else if (wParam == VK_CONTROL)
              return DefWindowProc(message, wParam, lParam );
//              wxControlDown = TRUE;
            // Avoid duplicate messages to OnChar
            else if ((wParam != VK_ESCAPE) && (wParam != VK_SPACE) && (wParam != VK_RETURN) && (wParam != VK_BACK) && (wParam != VK_TAB))
	    	{
              OnChar((WORD)wParam, lParam);
	    	}
			else return DefWindowProc(message, wParam, lParam );

            break;
        }
        case WM_KEYUP:
        {
/*
            if (wParam == VK_SHIFT)
              wxShiftDown = FALSE;
            else if (wParam == VK_CONTROL)
              wxControlDown = FALSE;
*/
            break;
        }
        case WM_CHAR: // Always an ASCII character
        {
          OnChar((WORD)wParam, lParam, TRUE);
          break;
        }
        case WM_HSCROLL:
        {
#ifdef WIN32
            WORD code = LOWORD(wParam);
            WORD pos = HIWORD(wParam);
            HWND control = (HWND)lParam;
#else
            WORD code = (WORD)wParam;
            WORD pos = LOWORD(lParam);
            HWND control = (HWND)HIWORD(lParam);
#endif
            OnHScroll(code, pos, control);
            break;
        }
        case WM_VSCROLL:
        {
#ifdef WIN32
            WORD code = LOWORD(wParam);
            WORD pos = HIWORD(wParam);
            HWND control = (HWND)lParam;
#else
            WORD code = (WORD)wParam;
            WORD pos = LOWORD(lParam);
            HWND control = (HWND)HIWORD(lParam);
#endif
            OnVScroll(code, pos, control);
            break;
        }
#ifdef WIN32
        case WM_CTLCOLORBTN:
	{
          int nCtlColor = CTLCOLOR_BTN;
          HWND control = (HWND)lParam;
          HDC pDC = (HDC)wParam;
          return (DWORD)OnCtlColor(pDC, control, nCtlColor,
                                        message, wParam, lParam);
          break;
	}
        case WM_CTLCOLORDLG:
	{
          int nCtlColor = CTLCOLOR_DLG;
          HWND control = (HWND)lParam;
          HDC pDC = (HDC)wParam;
          return (DWORD)OnCtlColor(pDC, control, nCtlColor,
                                        message, wParam, lParam);\
          break;
	}
        case WM_CTLCOLORLISTBOX:
	{
          int nCtlColor = CTLCOLOR_LISTBOX;
          HWND control = (HWND)lParam;
          HDC pDC = (HDC)wParam;
          return (DWORD)OnCtlColor(pDC, control, nCtlColor,
                                        message, wParam, lParam);
          break;
	}
        case WM_CTLCOLORMSGBOX:
	{
          int nCtlColor = CTLCOLOR_MSGBOX;
          HWND control = (HWND)lParam;
          HDC pDC = (HDC)wParam;
          return (DWORD)OnCtlColor(pDC, control, nCtlColor,
                                        message, wParam, lParam);
          break;
	}
        case WM_CTLCOLORSCROLLBAR:
	{
          int nCtlColor = CTLCOLOR_SCROLLBAR;
          HWND control = (HWND)lParam;
          HDC pDC = (HDC)wParam;
          return (DWORD)OnCtlColor(pDC, control, nCtlColor,
                                        message, wParam, lParam);
          break;
	}
        case WM_CTLCOLORSTATIC:
	{
          int nCtlColor = CTLCOLOR_STATIC;
          HWND control = (HWND)lParam;
          HDC pDC = (HDC)wParam;
          return (DWORD)OnCtlColor(pDC, control, nCtlColor,
                                        message, wParam, lParam);
          break;
	}
        case WM_CTLCOLOREDIT:
	{
          int nCtlColor = CTLCOLOR_EDIT;
          HWND control = (HWND)lParam;
          HDC pDC = (HDC)wParam;
          return (DWORD)OnCtlColor(pDC, control, nCtlColor,
                                        message, wParam, lParam);
          break;
	}
#else
        case WM_CTLCOLOR:
        {
          HWND control = (HWND)LOWORD(lParam);
          int nCtlColor = (int)HIWORD(lParam);
          HDC pDC = (HDC)wParam;
          return (DWORD)OnCtlColor(pDC, control, nCtlColor,
                                        message, wParam, lParam);
          break;
        }
#endif
        case WM_SYSCOLORCHANGE:
        {
          // Return value of 0 means, we processed it.
          if (OnColorChange(hWnd, message, wParam, lParam) == 0)
            return 0;
          else
            return ::DefWindowProc( hWnd, message, wParam, lParam );
          break;
        }
        case WM_ERASEBKGND:
        {
          // Prevents flicker when dragging
          if (IsIconic(hWnd)) return 1;

          if (!OnEraseBkgnd((HDC)wParam))
            return DefWindowProc(message, wParam, lParam );
          else return 1;
          break;
        }
        case WM_MDIACTIVATE:
        {
#ifdef WIN32
            HWND hWndActivate = GET_WM_MDIACTIVATE_HWNDACTIVATE(wParam,lParam);
            HWND hWndDeactivate = GET_WM_MDIACTIVATE_HWNDDEACT(wParam,lParam);
            BOOL activate = GET_WM_MDIACTIVATE_FACTIVATE(hWnd,wParam,lParam);
            return OnMDIActivate(activate, hWndActivate, hWndDeactivate);
#else
            return OnMDIActivate((BOOL)wParam, (HWND)LOWORD(lParam),
                                               (HWND)HIWORD(lParam));
#endif
        }
        case WM_DROPFILES:
        {
            OnDropFiles(wParam);
            break;
	}
        case WM_QUERYENDSESSION:
        {
	    // Same as WM_CLOSE, but inverted results. Thx Microsoft :-)
            return OnClose();
            break;
        }
        case WM_CLOSE:
        {
            if (OnClose())
              return 0L;
            else
              return 1L;
            break;
        }

        case WM_GETMINMAXINFO:
        {
          MINMAXINFO *info = (MINMAXINFO *)lParam;
          if (wx_window->minSizeX != -1)
           info->ptMinTrackSize.x = (int)wx_window->minSizeX;
          if (wx_window->minSizeY != -1)
           info->ptMinTrackSize.y = (int)wx_window->minSizeY;
          if (wx_window->maxSizeX != -1)
           info->ptMaxTrackSize.x = (int)wx_window->maxSizeX;
          if (wx_window->maxSizeY != -1)
           info->ptMaxTrackSize.y = (int)wx_window->maxSizeY;
          return DefWindowProc(message, wParam, lParam );
          break;
        }

/*
#if HAVE_SOCKET
        case WM_TIMER:
	{
          __ddeUnblock(hWnd, wParam);
          break;
	}

 	case ASYNC_SELECT_MESSAGE:
 	    return ddeWindowProc(hWnd,message,wParam,lParam);
#endif
*/

        default:
            return DefWindowProc(message, wParam, lParam );
    }
    return 0; // Success: we processed this command.
}

// Dialog window proc
LONG APIENTRY _EXPORT
  wxDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  wxWnd *wnd = wxFindWinFromHandle(hWnd);

  if (!wnd && wxWndHook)
  {
    wnd = wxWndHook;
    wnd->handle = hWnd;
  }

  if (wnd)
  {
    wnd->last_msg = message;
    wnd->last_wparam = wParam;
    wnd->last_lparam = lParam;
  }

  if (message == WM_SETFONT)
    return 0;
  else if (message == WM_INITDIALOG)
    return 0;

  // Don't know why this is necessary: seems to have a problem
  // finding the virtual wxDialogWnd::WindowProc function without being forced,
  // otherwise uses wxWnd::WindowProc.
  wxDialogWnd *dialogWnd = (wxDialogWnd *)wnd;
  return dialogWnd->wxDialogWnd::WindowProc(message, wParam, lParam);
}

LONG wxDialogWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  wxWnd *wnd = this;
  HWND hWnd = (HWND)wnd->handle;

  switch (message)
  {
        // See note for the 'default' case for why we need all these.
        case WM_ACTIVATE:
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        case WM_CREATE:
        case WM_SIZE:
        case WM_COMMAND:
	case WM_PAINT:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_MOUSEMOVE:
        case WM_HSCROLL:
        case WM_VSCROLL:
#ifdef WIN32
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT:
#else
        case WM_CTLCOLOR:
#endif
        case WM_SYSCOLORCHANGE:
        case WM_ERASEBKGND:
        case WM_QUERYENDSESSION:
        case WM_CLOSE:
        case WM_DROPFILES:
        case WM_DRAWITEM:
        case WM_MEASUREITEM:
        case WM_SETFONT:
        {
          return this->wxWnd::WindowProc(message, wParam, lParam);
        }

        case WM_INITDIALOG:
        {
          return 0;
        }
#if CTL3D
//        case WM_SETTEXT: // Causes garbled frame titles in Windows NT!
        case WM_NCPAINT:
        case WM_NCACTIVATE:
        {
          // Panels/dialogs with user colours don't use CTL3D
          if (!userColours)
          {
            SetWindowLong(hWnd, DWL_MSGRESULT,
                          Ctl3dDlgFramePaint(hWnd, message, wParam, lParam));
            return TRUE;
          }
          else return FALSE;
          break;
        }
#endif
        // Don't intercept keystrokes from dialogs, or this would cause problems, so
        // override the wxWnd behaviour.
        case WM_KEYDOWN:
        {
          return ::DefWindowProc(hWnd, message, wParam, lParam);
          break;
        }
        case WM_KEYUP:
        {
          return ::DefWindowProc(hWnd, message, wParam, lParam);
          break;
        }
        default:
        {
          // I would like to be able to call wxSubWnd::WindowProc here
          // but it interferes with dialog painting and message handling,
          // so until we ascertain why and intercept the offending message(s),
          // we have to explicitly call wxSubWnd::WindowProc (above) only for the
          // messages we know work for wxDialogWnd too.
          return FALSE;
//          return wxSubWnd::WindowProc(message, wParam, lParam);
	}
    }
    return FALSE;
}

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
wxList *wxWinHandleList = NULL;
#else
wxNonlockingHashTable *wxWinHandleList = NULL;
#endif
wxWnd *wxFindWinFromHandle(HWND hWnd)
{
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxNode *node = wxWinHandleList->Find((long)hWnd);
  if (!node)
    return NULL;
  return (wxWnd *)node->Data();
#else
  return (wxWnd *)wxWinHandleList->Find((long)hWnd);
#endif
}

void wxAssociateWinWithHandle(HWND hWnd, wxWnd *win)
{
  wxWinHandleList->Append((long)hWnd, win);
}

/* wxWnd class used to implement all Windows 3 windows
 */
wxWnd::wxWnd(void)

{
  x_scrolling_enabled = TRUE;
  y_scrolling_enabled = TRUE;
  calcScrolledOffset = TRUE;
  last_msg = 0;
  last_wparam = 0;
  last_lparam = 0;
  accelerator_table = NULL;
  hMenu = 0;

  xscroll_pixels_per_line = 0;
  yscroll_pixels_per_line = 0;
  xscroll_lines = 0;
  yscroll_lines = 0;
  xscroll_lines_per_page = 0;
  yscroll_lines_per_page = 0;
  xscroll_position = 0;
  yscroll_position = 0;
  canDeleteBackgroundBrush = FALSE;
  userColours = FALSE;
  background_colour = GetSysColor(COLOR_BTNFACE);
  background_transparent = FALSE;
  
#if (FAFA_LIB && !USE_GREY_BACKGROUND)
  SetBackgroundBrush(brushFace, FALSE) ;
#else
  //background_brush = GetStockObject( LTGRAY_BRUSH );
  // No no no... After investigations, I found that Ctl3d use BTNFACE color
  // (which is ALWAYS grey :-))
  // So, respect the behavior!
  SetBackgroundBrush(CreateSolidBrush(GetSysColor(COLOR_BTNFACE)), TRUE) ;
#endif
  last_x_pos = (float)-1.0;
  last_y_pos = (float)-1.0;
  last_event = -1;
  is_canvas = FALSE;
/*
  cdc = NULL;
  ldc = NULL ;
  dc_count = 0 ;
*/
}

wxWnd::~wxWnd(void)
{
  wxWinHandleList->DeleteObject(this);
#if !(FAFA_LIB && !USE_GREY_BACKGROUND)
  if (background_brush && canDeleteBackgroundBrush)
    ::DeleteObject(background_brush) ;
#endif
}

// Default destroyer - override if you destroy it in some other way
// (e.g. with MDI child windows)
void wxWnd::DestroyWindow(void)
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::DestroyWindow %d\n", handle);
#endif
  DetachWindowMenu();
  SetWindowLong(handle, 0, (long)0);
  HWND oldHandle = handle;
  handle = NULL;

#if 0
  // For some reason, wxWindows can activate another task altogether
  // when a frame is destroyed after a modal dialog has been invoked.
  // Try to bring the parent or main frame to the top.
  if (wx_window && (wx_window->IsKindOf(CLASSINFO(wxFrame)) || wx_window->IsKindOf(CLASSINFO(wxDialogBox))))
  {
    HWND hWnd = 0;
    if (wx_window->GetParent())
      hWnd = wx_window->GetParent()->GetHWND();
//    else if (wxTheApp->wx_frame && (wxTheApp->wx_frame != wx_window))
//      hWnd = wxTheApp->wx_frame->GetHWND();
    if (hWnd)
      ::BringWindowToTop(hWnd);
  }
#endif

  ::DestroyWindow(oldHandle);

  // Menu is destroyed explicitly by wxMDIChild::DestroyWindow,
  // or when Windows HWND is deleted if MDI parent or
  // SDI frame.
/*
  if (hMenu)
  {
    ::DestroyMenu(hMenu);
    hMenu = 0;
  }
 */
}

void wxWnd::Create(wxWnd *parent, char *wclass, wxWindow *wx_win, char *title,
                    int x, int y, int width, int height,
                    DWORD style, char *dialog_template, DWORD extendedStyle)
{
  wx_window = wx_win;
  if (wx_window)
    wx_window->handle = (char *)this;
    
  is_dialog = (dialog_template != NULL);
  int x1 = CW_USEDEFAULT;
  int y1 = 0;
  int width1 = CW_USEDEFAULT;
  int height1 = 100;
  if (wx_window && wx_window->GetWindowStyleFlag() & wxUSER_COLOURS)
    userColours = TRUE;
  else
    userColours = FALSE;

  // Find parent's size, if it exists, to set up a possible default
  // panel size the size of the parent window
  RECT parent_rect;
  if (parent)
  {
    // Was GetWindowRect: JACS 5/5/95
    GetClientRect(parent->handle, &parent_rect);

    width1 = parent_rect.right - parent_rect.left;
    height1 = parent_rect.bottom - parent_rect.top;
  }

  if (x > -1) x1 = x;
  if (y > -1) y1 = y;
  if (width > -1) width1 = width;
  if (height > -1) height1 = height;

  HWND hParent = NULL;
  if (parent)
    hParent = parent->handle;

  wxWndHook = this;

  if (is_dialog)
  {
    // MakeProcInstance doesn't seem to be needed in C7. Is it needed for
    // other compilers???
//    DLGPROC dlgproc = (DLGPROC)MakeProcInstance(wxWndProc, wxhInstance);

    handle = ::CreateDialog(wxhInstance, dialog_template, hParent,
                            (DLGPROC)wxDlgProc);
/*
    DLGPROC dlgproc = (DLGPROC)MakeProcInstance((DLGPROC)wxWndProc, wxhInstance);

    handle = ::CreateDialog(wxhInstance, dialog_template, hParent,
                            (DLGPROC)dlgproc);
*/
    if (handle == 0)
       MessageBox(NULL, "Can't find dummy dialog template!\nCheck resource include path for finding wx.rc.",
                  "wxWindows Error", MB_ICONEXCLAMATION | MB_OK);
    else MoveWindow(handle, x1, y1, width1, height1, FALSE);
  }
  else
  {
    int controlId = 0;
    if (style & WS_CHILD)
      controlId = NewControlId();
    if (!title)
      title = "";
      
    handle = CreateWindowEx(extendedStyle, wclass,
                title,
                style,
                x1, y1,
                width1, height1,
//                hParent, NULL, wxhInstance,
                hParent, (HMENU)controlId, wxhInstance,
                NULL);

    if (handle == 0)
    {
       char buf[300];
       sprintf(buf, "Can't create window of class %s! Weird.\nPossible Windows 3.x compatibility problem?",
         wclass);
       wxFatalError(buf,
                  "Fatal wxWindows Error");
    }
  }
  wxWndHook = NULL;
  wxWinHandleList->Append((long)handle, this);

#if DEBUG > 1
  wxDebugMsg("wxWnd::Create %d\n", handle);
#endif

  // Can't do this for dialogs!!!!
  if (!is_dialog) SetWindowLong(handle, 0, (long)this);
}

void wxWnd::OnCreate(LPCREATESTRUCT WXUNUSED(cs))
{
}

BOOL wxWnd::OnPaint(void)
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnPaint %d\n", handle);
#endif
  return 1;
}

BOOL wxWnd::OnClose(void)
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnClose %d\n", handle);
#endif
  return FALSE;
}

BOOL wxWnd::OnDestroy(void)
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnDestroy %d\n", handle);
#endif
  return TRUE;
}

void wxWnd::OnSize(int WXUNUSED(x), int WXUNUSED(y), UINT WXUNUSED(flag))
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnSize %d\n", handle);
#endif
}

// Deal with child commands from buttons etc.

BOOL wxWnd::OnCommand(WORD WXUNUSED(id), WORD WXUNUSED(cmd), HWND WXUNUSED(control))
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnCommand %d\n", handle);
#endif
  return FALSE;
}

BOOL wxWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
#if WIN95
  if (wx_window)
  {
    // Find a child window to send the notification to, e.g. a toolbar.
    // There's a problem here. NMHDR::hwndFrom doesn't give us the
    // handle of the toolbar; it's probably the handle of the tooltip
    // window (anyway, it's parent is also the toolbar's parent).
    // So, since we don't know which hWnd or wxWindow originated the
    // WM_NOTIFY, we'll need to go through all the children of this window
    // trying out MSWNotify.
    wxNode *node = wx_window->GetChildren()->First();
    while (node)
    {
      wxWindow *child = (wxWindow *)node->Data();
      if (child->MSWNotify(wParam, lParam))
        return TRUE;
      node = node->Next();
    }
    return FALSE;

/* This doesn't work
    NMHDR *hdr = (NMHDR *)lParam;
    HWND hWnd = (HWND)hdr->hwndFrom;
    wxWindow *win = wxFindControlFromHandle(hWnd);
*/
  }
  else
#endif
    return FALSE;
}

void wxWnd::OnMenuSelect(WORD WXUNUSED(item), WORD WXUNUSED(flags), HMENU WXUNUSED(sysmenu))
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnMenuSelect %d\n", handle);
#endif
}

void wxWnd::OnInitMenuPopup(HMENU menu, int pos, Bool isSystem)
{
  if (wx_window && !isSystem)
    wx_window->OnInitMenuPopup(pos);
}

BOOL wxWnd::OnActivate(BOOL state, BOOL WXUNUSED(minimized), HWND WXUNUSED(activate))
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnActivate %d\n", handle);
#endif
  if (wx_window)
  {
    wx_window->GetEventHandler()->OnActivate(((state == WA_ACTIVE) || (state == WA_CLICKACTIVE)));

    // If this window is an MDI parent, we must also send an OnActivate message
    // to the current child.
    if (wx_window->IsKindOf(CLASSINFO(wxFrame)))
    {
      wxFrame *frame = (wxFrame *)wx_window;
      if (frame->GetWindowStyleFlag() & wxMDI_PARENT)
      {
        wxMDIFrame *mdiFrame = (wxMDIFrame *)this;
        if ((mdiFrame->current_child) && ((state == WA_ACTIVE) || (state == WA_CLICKACTIVE)))
          mdiFrame->current_child->wx_window->GetEventHandler()->OnActivate(TRUE);
      }
    }
    return 0;
  }
  else return TRUE;
}

BOOL wxWnd::OnSetFocus(HWND WXUNUSED(hwnd))
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnSetFocus %d\n", handle);
#endif
  if (wx_window)
  {
    // Deal with caret
    if (wx_window->caretEnabled && (wx_window->caretWidth > 0) && (wx_window->caretHeight > 0))
    {
      ::CreateCaret(wx_window->GetHWND(), NULL, wx_window->caretWidth, wx_window->caretHeight);
      if (wx_window->caretShown)
        ::ShowCaret(wx_window->GetHWND());
    }
    
    wx_window->GetEventHandler()->OnSetFocus();
//    return 0;
    return TRUE;
  }
  else return FALSE;
}

BOOL wxWnd::OnKillFocus(HWND WXUNUSED(hwnd))
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnKillFocus %d\n", handle);
#endif
  if (wx_window)
  {
    // Deal with caret
    if (wx_window->caretEnabled)
    {
      ::DestroyCaret();
    }

    wx_window->GetEventHandler()->OnKillFocus();
    return TRUE;
  }
  else return FALSE;
}

void wxWnd::OnDropFiles(WPARAM wParam)
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnDropFiles %d\n", handle);
#endif

#ifndef GNUWIN32
  HANDLE hFilesInfo = (HANDLE)wParam;
  POINT dropPoint;
  DragQueryPoint(hFilesInfo, (LPPOINT) &dropPoint);

  // Get the total number of files dropped
  WORD gwFilesDropped = (WORD)DragQueryFile ((HDROP)hFilesInfo,
				   (UINT)-1,
                                   (LPSTR)0,
                                   (UINT)0);

  char **files = new char *[gwFilesDropped];
  int wIndex;
  for (wIndex=0; wIndex < (int)gwFilesDropped; wIndex++)
  {
    DragQueryFile (hFilesInfo, wIndex, (LPSTR) wxBuffer, 1000);
    files[wIndex] = copystring(wxBuffer);
  }
  DragFinish (hFilesInfo);
  if (wx_window)
    wx_window->GetEventHandler()->OnDropFiles(gwFilesDropped, files, dropPoint.x, dropPoint.y);

  int i;
  for (i = 0; i < (int)gwFilesDropped; i++)
    delete[] files[i];
  delete[] files;
#endif
}

BOOL wxWnd::OnDrawItem(int id, DRAWITEMSTRUCT *itemStruct)
{
  wxWindow *item = wx_window->FindItem(id);
#if USE_DYNAMIC_CLASSES
  if (item && item->IsKindOf(CLASSINFO(wxItem)))
  {
    return ((wxItem *)item)->MSWOnDraw(itemStruct);
  }
  else
#endif
    return FALSE;
}

BOOL wxWnd::OnMeasureItem(int id, MEASUREITEMSTRUCT *itemStruct)
{
  wxWindow *item = wx_window->FindItem(id);
#if USE_DYNAMIC_CLASSES
  if (item && item->IsKindOf(CLASSINFO(wxItem)))
  {
    return ((wxItem *)item)->MSWOnMeasure(itemStruct);
  }
  else
#endif
    return FALSE;
}

void wxWnd::OnVScroll(WORD WXUNUSED(code), WORD WXUNUSED(pos), HWND WXUNUSED(control))
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnVScroll %d\n", handle);
#endif
}

void wxWnd::OnHScroll(WORD WXUNUSED(code), WORD WXUNUSED(pos), HWND WXUNUSED(control))
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnHScroll %d\n", handle);
#endif
}

void wxWnd::CalcScrolledPosition(int x, int y, int *xx, int *yy)
{
  *xx = (calcScrolledOffset ? (x - xscroll_position * xscroll_pixels_per_line) : x);
  *yy = (calcScrolledOffset ? (y - yscroll_position * yscroll_pixels_per_line) : y);
}

void wxWnd::CalcUnscrolledPosition(int x, int y, float *xx, float *yy)
{
  *xx = (float)(calcScrolledOffset ? (x + xscroll_position * xscroll_pixels_per_line) : x);
  *yy = (float)(calcScrolledOffset ? (y + yscroll_position * yscroll_pixels_per_line) : y);
}

HBRUSH wxWnd::OnCtlColor(HDC pDC, HWND pWnd, UINT nCtlColor,
                         UINT message, WPARAM wParam, LPARAM lParam)
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnCtlColour %d\n", handle);
#endif
  // Ignores CTL3D and FAFA settings of background colour,
  // uses current background colour for background,
  // and COLOR_BTNFACE for foreground.
  if (userColours && wx_window)
  {
    // Is this OK for WIN32???
    wxItem *item = (wxItem *)wx_window->FindItemByHWND(pWnd);

    // Change this if it's a static item
    SetBkMode(pDC, OPAQUE);

    if (!item)
    {
      // If it's not an item, it's probably a static associated with
      // an item. Find that item and get the label text/background colour.
      
      wxItem *itemForStatic = (wxItem *)wx_window->FindItemByStatic(pWnd);
      if (itemForStatic)
      {
        if (nCtlColor == CTLCOLOR_EDIT)
        {
          // Probably a wxSlider edit control
          if (itemForStatic->GetButtonColour())
          {
            COLORREF foreground = PALETTERGB(itemForStatic->GetButtonColour()->Red(), itemForStatic->GetButtonColour()->Green(), itemForStatic->GetButtonColour()->Blue());
            ::SetTextColor(pDC, foreground);
          }
          else
          {
            ::SetTextColor(pDC, GetSysColor(COLOR_BTNTEXT));
          }

          if (itemForStatic->GetBackgroundColour() && itemForStatic->GetBackgroundHBRUSH())
          {
             ::SetBkColor(pDC, PALETTERGB(itemForStatic->GetBackgroundColour()->Red(), itemForStatic->GetBackgroundColour()->Green(),
                                   itemForStatic->GetBackgroundColour()->Blue()));
             return itemForStatic->GetBackgroundHBRUSH();
          }
          else
          {
            ::SetBkColor(pDC, PALETTERGB(255,255,255));
            return GetStockObject( WHITE_BRUSH );
          }
        }
        else
        {
          if (background_transparent)
            SetBkMode(pDC, TRANSPARENT);
          else
            SetBkMode(pDC, OPAQUE);
      
          if (itemForStatic->GetLabelColour())
          {
            COLORREF foreground = PALETTERGB(itemForStatic->GetLabelColour()->Red(), itemForStatic->GetLabelColour()->Green(), itemForStatic->GetLabelColour()->Blue());
            ::SetTextColor(pDC, foreground);
            ::SetBkColor(pDC, background_colour);
            return background_brush;
          }
          else
          {
            ::SetTextColor(pDC, GetSysColor(COLOR_BTNTEXT));
            ::SetBkColor(pDC, background_colour);
            return background_brush;
          }
        }
      }
    }
      
    if (!item)
      return 0;

    if (item->GetButtonColour())
    {
      COLORREF foreground = PALETTERGB(item->GetButtonColour()->Red(), item->GetButtonColour()->Green(), item->GetButtonColour()->Blue());
      ::SetTextColor(pDC, foreground);
    }
    else
      ::SetTextColor(pDC, GetSysColor(COLOR_BTNTEXT));

    // For some controls we return the PANEL background brush, e.g. statics.
    // For others, we return the ITEM background brush, e.g. text control.

    if (nCtlColor == CTLCOLOR_STATIC && item->IsKindOf(CLASSINFO(wxMessage)))
    {
      if (background_transparent)
        SetBkMode(pDC, TRANSPARENT);
      else
        SetBkMode(pDC, OPAQUE);

      ::SetBkColor(pDC, background_colour);
      return background_brush;
    }

    if (nCtlColor == CTLCOLOR_BTN &&
        (item->IsKindOf(CLASSINFO(wxCheckBox)) || item->IsKindOf(CLASSINFO(wxGroupBox)) || 
          (item->IsKindOf(CLASSINFO(wxRadioBox)) && item->GetHWND() == (HWND)LOWORD(lParam))))
    {
      if (background_transparent)
        SetBkMode(pDC, TRANSPARENT);
      else
        SetBkMode(pDC, OPAQUE);

      ::SetBkColor(pDC, background_colour);
      if (item->GetLabelColour())
      {
        COLORREF foreground = PALETTERGB(item->GetLabelColour()->Red(), item->GetLabelColour()->Green(), item->GetLabelColour()->Blue());
        ::SetTextColor(pDC, foreground);
      }
      return background_brush;
    }

    // For radiobox labels, use the button font to distinguish from the label.
    if (nCtlColor == CTLCOLOR_BTN && item->IsKindOf(CLASSINFO(wxRadioBox)))
    {
      if (background_transparent)
        SetBkMode(pDC, TRANSPARENT);
      else
        SetBkMode(pDC, OPAQUE);

      ::SetBkColor(pDC, background_colour);
      if (item->GetButtonColour())
      {
        COLORREF foreground = PALETTERGB(item->GetButtonColour()->Red(), item->GetButtonColour()->Green(), item->GetButtonColour()->Blue());
        ::SetTextColor(pDC, foreground);
      }
      return background_brush;
    }

    /* WHY does an ordinary button (at least running a 16-bit app in Windows 95)
     * NOT get coloured with our own colours? All other controls seem to work OK.
     */
     
    if ((nCtlColor == CTLCOLOR_BTN &&
          !item->IsKindOf(CLASSINFO(wxCheckBox)) &&
          !item->IsKindOf(CLASSINFO(wxGroupBox)) &&
          !item->IsKindOf(CLASSINFO(wxRadioBox)))
         || nCtlColor == CTLCOLOR_LISTBOX ||
        nCtlColor == CTLCOLOR_SCROLLBAR || nCtlColor == CTLCOLOR_EDIT)
    {
      if (item->GetBackgroundColour())
      {
        COLORREF background = PALETTERGB(item->GetBackgroundColour()->Red(), item->GetBackgroundColour()->Green(), item->GetBackgroundColour()->Blue());
        ::SetBkColor(pDC, background);
      }
      else
        ::SetBkColor(pDC, background_colour);

      if (item->GetBackgroundHBRUSH())
        return item->GetBackgroundHBRUSH();
      else
      {
        // Use a default background brush
        switch (nCtlColor)
        {
          case CTLCOLOR_BTN:
          {
#if FAFA_LIB
            return brushFace;
#else
            return background_brush;
#endif
          }
          default:
          {
            // Make it white
            ::SetBkColor(pDC, PALETTERGB(255,255,255));
            return GetStockObject( WHITE_BRUSH );
          }
        }
      }
    }
    else
    {
      // These are statics
      ::SetTextColor(pDC, GetSysColor(COLOR_BTNTEXT));
      ::SetBkColor(pDC, background_colour);
      return background_brush;
    }
  }

  if (background_transparent)
    SetBkMode(pDC, TRANSPARENT);
  else
    SetBkMode(pDC, OPAQUE);

#if CTL3D
  HBRUSH hbrush = Ctl3dCtlColorEx(message, wParam, lParam);
      
  if (hbrush != (HBRUSH) 0)
    return hbrush;
  else
    return (HBRUSH)::DefWindowProc(pWnd, message, wParam, lParam);
#elif (!FAFA_LIB || USE_GREY_BACKGROUND)
  if ((nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_BTN) && background_brush)
  {
    // After investigations, I found that Ctl3d use BTNFACE color
    // (which is ALWAYS grey :-))
    // So, respect the behavior!
    SetBkColor(pDC, GetSysColor(COLOR_BTNFACE)) ;
    return background_brush;
  }
  else return NULL;
#else
  if ((nCtlColor==CTLCOLOR_BTN || nCtlColor==CTLCOLOR_SCROLLBAR) && brushFace)
  {
    SetBkColor(pDC, GetSysColor(COLOR_BTNFACE)) ;
    return brushFace;
  }
  else if (brushBack)
  {
    SetBkColor(pDC, GetSysColor(COLOR_WINDOW)) ;
    return brushBack;
  }
  else return NULL;
#endif
}

// Set background brush, possibly deleting old one and
// noting whether we can delete the current one.
void wxWnd::SetBackgroundBrush(HBRUSH br, Bool canDelete)
{
  if (background_brush && canDeleteBackgroundBrush)
    ::DeleteObject(background_brush);
  canDeleteBackgroundBrush = canDelete;
  background_brush = br;
}

BOOL wxWnd::OnColorChange(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  // Check if app handles this.
  if (wx_window && wx_window->GetEventHandler()->OnSysColourChange())
    return 0;

  // No FAFA/CTL3D processing: invoke default proc.
  if (userColours)
    return 1;

#if FAFA_LIB
#if !USE_GREY_BACKGROUND
  HBRUSH br = SetupBackground(hWnd) ;
  if (br)
  {
//    wnd->background_brush = br ;
    SetBackgroundBrush(br, FALSE);
  }
  return 0 ;
#endif
#endif
#if CTL3D
  Ctl3dColorChange();
#endif
#if !FAFA_LIB && !CTL3D
  // We didn't process it
  return 1;
#else
  // We processed this message.
  return 0;
#endif
}

BOOL wxWnd::OnEraseBkgnd(HDC WXUNUSED(pDC))
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnEraseBkgnd %d\n", handle);
#endif
  return FALSE;
}

void wxWnd::OnLButtonDown(int WXUNUSED(x), int WXUNUSED(y), UINT WXUNUSED(flags))
{
}

void wxWnd::OnLButtonUp(int WXUNUSED(x), int WXUNUSED(y), UINT WXUNUSED(flags))
{
}

void wxWnd::OnLButtonDClick(int WXUNUSED(x), int WXUNUSED(y), UINT WXUNUSED(flags))
{
}

void wxWnd::OnMButtonDown(int WXUNUSED(x), int WXUNUSED(y), UINT WXUNUSED(flags))
{
}

void wxWnd::OnMButtonUp(int WXUNUSED(x), int WXUNUSED(y), UINT WXUNUSED(flags))
{
}

void wxWnd::OnMButtonDClick(int WXUNUSED(x), int WXUNUSED(y), UINT WXUNUSED(flags))
{
}

void wxWnd::OnRButtonDown(int WXUNUSED(x), int WXUNUSED(y), UINT WXUNUSED(flags))
{
}

void wxWnd::OnRButtonUp(int WXUNUSED(x), int WXUNUSED(y), UINT WXUNUSED(flags))
{
}

void wxWnd::OnRButtonDClick(int WXUNUSED(x), int WXUNUSED(y), UINT WXUNUSED(flags))
{
}

void wxWnd::OnMouseMove(int WXUNUSED(x), int WXUNUSED(y), UINT WXUNUSED(flags))
{
}

void wxWnd::OnMouseEnter(int WXUNUSED(x), int WXUNUSED(y), UINT WXUNUSED(flags))
{
}

void wxWnd::OnMouseLeave(int WXUNUSED(x), int WXUNUSED(y), UINT WXUNUSED(flags))
{
}

void wxWnd::OnChar(WORD WXUNUSED(wParam), LPARAM WXUNUSED(lParam), Bool WXUNUSED(isASCII))
{
}

LONG wxWnd::DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
#if USE_ITSY_BITSY
  if (wx_window && ((wx_window->GetWindowStyleFlag() & wxTINY_CAPTION_HORIZ) ||
                    (wx_window->GetWindowStyleFlag() & wxTINY_CAPTION_VERT)))
    return ::ibDefWindowProc(handle, nMsg, wParam, lParam);
  else                  
#endif
  return ::DefWindowProc(handle, nMsg, wParam, lParam);
}

BOOL wxWnd::ProcessMessage(MSG* pMsg)
{
  if (!wx_window || !handle)
    return FALSE;
  else
  {
    // Suggestion by Andrew Davison to allow
    // a panel to accept character input in user edit mode
    if (!wx_window->GetUserEditMode() && wx_window->IsKindOf(CLASSINFO(wxPanel)))
      return ::IsDialogMessage(handle, pMsg);
    else
      return FALSE;
  }
}

BOOL wxWnd::OnMDIActivate(BOOL WXUNUSED(flag), HWND WXUNUSED(activate), HWND WXUNUSED(deactivate))
{
#if DEBUG > 1
  wxDebugMsg("wxWnd::OnMDIActivate %d\n", handle);
#endif
  return 1;
}

void wxWnd::DetachWindowMenu(void)
{
  if (hMenu)
  {
    int N = GetMenuItemCount(hMenu);
    int i;
    for (i = 0; i < N; i++)
    {
      char buf[100];
      int chars = GetMenuString(hMenu, i, buf, 100, MF_BYPOSITION);
      if ((chars > 0) && (strcmp(buf, "&Window") == 0))
      {
        RemoveMenu(hMenu, i, MF_BYPOSITION);
        break;
      }
    }
  }
}

/*
 * Subwindow - used for panels and canvases
 *
 */

wxSubWnd::wxSubWnd(void)
{
  mouse_in_window = FALSE ;
}

wxSubWnd::wxSubWnd(wxWnd *parent, char *wclass, wxWindow *wx_win,
                           int x, int y, int width, int height,
                           DWORD style, char *dialog_template)

{
  Create(parent, wclass, wx_win, NULL, x, y, width, height, style, dialog_template);
  mouse_in_window = FALSE ;
}

wxSubWnd::~wxSubWnd(void)
{
}


BOOL wxSubWnd::OnPaint(void)
{
#if DEBUG > 1
  wxDebugMsg("wxSubWnd::OnPaint %d\n", handle);
#endif
#ifdef WIN32
  HRGN	tRgn=CreateRectRgn(0,0,0,0);	//Dummy call to get a handle!
  if (GetUpdateRgn(handle, tRgn, FALSE))
#else
  RECT tRect;
  if (GetUpdateRect(handle, &tRect, FALSE))
#endif
  {
    PAINTSTRUCT ps;
    // Hold a pointer to the dc so long as the OnPaint() message
    // is being processed
    HDC dc = BeginPaint(handle, &ps);
    Bool isPanel = (wx_window && wx_window->IsKindOf(CLASSINFO(wxPanel)));
    if (wx_window)
    {
      wx_window->paintHDC = dc;
      wx_window->updateRect = ps.rcPaint;
      
      if (isPanel)
        ((wxPanel *)wx_window)->tempPS = &ps;
      wx_window->GetEventHandler()->OnPaint();
      if (isPanel)
        ((wxPanel *)wx_window)->tempPS = 0;
      wx_window->paintHDC = 0;
    }
    EndPaint(handle, &ps);
#ifdef WIN32
    DeleteObject(tRgn);
#endif

    if (isPanel)
      // Do default processing
      return FALSE;
    else
      return TRUE;
  }
#ifdef WIN32
  DeleteObject(tRgn);
#endif
  return FALSE;
}

void wxSubWnd::OnSize(int w, int h, UINT WXUNUSED(flag))
{
#if DEBUG > 1
  wxDebugMsg("wxSubWnd::OnSize %d\n", handle);
#endif
  if (!handle)
    return;

  if (wx_window)
    wx_window->GetEventHandler()->OnSize(w, h);
}

// Deal with child commands from buttons etc.
BOOL wxSubWnd::OnCommand(WORD id, WORD cmd, HWND WXUNUSED(control))
{
#if DEBUG > 1
  wxDebugMsg("wxSubWnd::OnCommand\n");
#endif
  if (wxCurrentPopupMenu)
  {
    wxMenu *popupMenu = wxCurrentPopupMenu;
    wxCurrentPopupMenu = NULL;
    BOOL succ = popupMenu->MSWCommand(cmd, id);
    return succ;
  }
#if DEBUG > 1
  char buf[80];
  sprintf(buf, "Looking for item %d...\n", id);
  wxDebugMsg(buf);
#endif

  wxWindow *item = wx_window->FindItem(id);
  if (item)
  {
    BOOL value = item->MSWCommand(cmd, id);
#if DEBUG > 1
    if (value)
      wxDebugMsg("MSWCommand succeeded\n");
    else
      wxDebugMsg("MSWCommand failed\n");
#endif
    return value;
  }
  else
  {
#if DEBUG > 1
    wxDebugMsg("Could not find item!\n");
    char buf[100];
    wxDebugMsg("Item ids for this panel:\n");

    wxNode *current = wx_window->GetChildren()->First();
    while (current)
    {
      wxObject *obj = (wxObject *)current->Data() ;
      if (obj->IsKindOf(CLASSINFO(wxItem)))
      {
        wxItem *item = (wxItem *)current->Data();
        sprintf(buf, "  %d\n", (int)item->windows_id);
        wxDebugMsg(buf);
      }
      current = current->Next();  
    }
    wxYield();
#endif
    return FALSE;
  }
}

void wxSubWnd::OnLButtonDown(int x, int y, UINT flags)
{
#if 0 // defined(WIN32) && !WIN95
  // DClick not clean supported on Win3.1, except if someone know
  // how to emulate Sleep()...
  // This means that your app will receive Down-Up-Dclick sequences
  // rather than Dclick
  if (wx_window && wx_window->doubleClickAllowed)
  {
    UINT time = GetDoubleClickTime() ;
    Sleep(time) ;
    MSG dummy ;
    if (PeekMessage(&dummy,handle,
                    WM_LBUTTONDBLCLK,WM_LBUTTONDBLCLK,
                    PM_NOREMOVE)
       )
    {
      PeekMessage(&dummy,handle,WM_LBUTTONUP,WM_LBUTTONUP,PM_REMOVE);
      return; 
    }
  }
#endif
//wxDebugMsg("LButtonDown\n") ;
  wxMouseEvent event(wxEVENT_TYPE_LEFT_DOWN);

  float px = (float)x;
  float py = (float)y;

  DeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.x, &event.y);

  event.shiftDown = (flags & MK_SHIFT);
  event.controlDown = (flags & MK_CONTROL);
  event.leftDown = (flags & MK_LBUTTON);
  event.middleDown = (flags & MK_MBUTTON);
  event.rightDown = (flags & MK_RBUTTON);
  event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */
  event.eventObject = wx_window;

  last_x_pos = event.x; last_y_pos = event.y; last_event = wxEVENT_TYPE_LEFT_DOWN;
  if (wx_window) wx_window->GetEventHandler()->OnEvent(event);
}

void wxSubWnd::OnLButtonUp(int x, int y, UINT flags)
{
//wxDebugMsg("LButtonUp\n") ;
  wxMouseEvent event(wxEVENT_TYPE_LEFT_UP);
  float px = (float)x;
  float py = (float)y;

  DeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.x, &event.y);

  event.shiftDown = (flags & MK_SHIFT);
  event.controlDown = (flags & MK_CONTROL);
  event.leftDown = (flags & MK_LBUTTON);
  event.middleDown = (flags & MK_MBUTTON);
  event.rightDown = (flags & MK_RBUTTON);
  event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */
  event.eventObject = wx_window;

  last_x_pos = event.x; last_y_pos = event.y; last_event = wxEVENT_TYPE_LEFT_UP;

  if (wx_window) wx_window->GetEventHandler()->OnEvent(event);
}

void wxSubWnd::OnLButtonDClick(int x, int y, UINT flags)
{
//wxDebugMsg("LButtonDClick\n") ;
  /* MATTHEW: If dclick not allowed, generate another single-click */
  wxMouseEvent event((wx_window && wx_window->doubleClickAllowed) ?
		     wxEVENT_TYPE_LEFT_DCLICK : wxEVENT_TYPE_LEFT_DOWN);
  float px = (float)x;
  float py = (float)y;

  DeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.x, &event.y);

  event.shiftDown = (flags & MK_SHIFT);
  event.controlDown = (flags & MK_CONTROL);
  event.leftDown = (flags & MK_LBUTTON);
  event.middleDown = (flags & MK_MBUTTON);
  event.rightDown = (flags & MK_RBUTTON);
  event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */
  event.eventObject = wx_window;

  last_x_pos = event.x; last_y_pos = event.y; last_event = wxEVENT_TYPE_LEFT_DCLICK;

  if (wx_window && wx_window->doubleClickAllowed) wx_window->GetEventHandler()->OnEvent(event);
}

void wxSubWnd::OnMButtonDown(int x, int y, UINT flags)
{
#if 0 // defined(WIN32) && !WIN95
  // DClick not clean supported on Win3.1, except if someone know
  // how to emulate Sleep()...
  // This means that your app will receive Down-Up-Dclick sequences
  // rather than Dclick
  if (wx_window && wx_window->doubleClickAllowed)
  {
    UINT time = GetDoubleClickTime() ;
    Sleep(time) ;
    MSG dummy ;
    if (PeekMessage(&dummy,handle,
                    WM_MBUTTONDBLCLK,WM_MBUTTONDBLCLK,
                    PM_NOREMOVE)
       )
    {
      PeekMessage(&dummy,handle,WM_MBUTTONUP,WM_MBUTTONUP,PM_REMOVE);
      return; 
    }
  }
#endif

//wxDebugMsg("MButtonDown\n") ;
  wxMouseEvent event(wxEVENT_TYPE_MIDDLE_DOWN);

  float px = (float)x;
  float py = (float)y;

  DeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.x, &event.y);

  event.shiftDown = (flags & MK_SHIFT);
  event.controlDown = (flags & MK_CONTROL);
  event.leftDown = (flags & MK_LBUTTON);
  event.middleDown = (flags & MK_MBUTTON);
  event.rightDown = (flags & MK_RBUTTON);
  event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */
  event.eventObject = wx_window;

  last_x_pos = event.x; last_y_pos = event.y; last_event = wxEVENT_TYPE_LEFT_DOWN;
  if (wx_window) wx_window->GetEventHandler()->OnEvent(event);
}

void wxSubWnd::OnMButtonUp(int x, int y, UINT flags)
{
//wxDebugMsg("MButtonUp\n") ;
  wxMouseEvent event(wxEVENT_TYPE_MIDDLE_UP);
  float px = (float)x;
  float py = (float)y;

  DeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.x, &event.y);

  event.shiftDown = (flags & MK_SHIFT);
  event.controlDown = (flags & MK_CONTROL);
  event.leftDown = (flags & MK_LBUTTON);
  event.middleDown = (flags & MK_MBUTTON);
  event.rightDown = (flags & MK_RBUTTON);
  event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */
  event.eventObject = wx_window;

  last_x_pos = event.x; last_y_pos = event.y; last_event = wxEVENT_TYPE_LEFT_UP;
  if (wx_window) wx_window->GetEventHandler()->OnEvent(event);
}

void wxSubWnd::OnMButtonDClick(int x, int y, UINT flags)
{
//wxDebugMsg("MButtonDClick\n") ;
  /* MATTHEW: If dclick not allowed, generate another single-click */
  wxMouseEvent event((wx_window && wx_window->doubleClickAllowed) ?
		     wxEVENT_TYPE_MIDDLE_DCLICK : wxEVENT_TYPE_MIDDLE_DOWN);

  float px = (float)x;
  float py = (float)y;

  DeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.x, &event.y);

  event.shiftDown = (flags & MK_SHIFT);
  event.controlDown = (flags & MK_CONTROL);
  event.leftDown = (flags & MK_LBUTTON);
  event.middleDown = (flags & MK_MBUTTON);
  event.rightDown = (flags & MK_RBUTTON);
  event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */
  event.eventObject = wx_window;

  last_x_pos = event.x; last_y_pos = event.y; last_event = wxEVENT_TYPE_LEFT_DCLICK;
  if (wx_window && wx_window->doubleClickAllowed) wx_window->GetEventHandler()->OnEvent(event);
}

void wxSubWnd::OnRButtonDown(int x, int y, UINT flags)
{
#if 0 // defined(WIN32) && !WIN95
  // DClick not clean supported on Win3.1, except if someone know
  // how to emulate Sleep()...
  // This means that your app will receive Down-Up-Dclick sequences
  // rather than Dclick
  if (wx_window && wx_window->doubleClickAllowed)
  {
    UINT time = GetDoubleClickTime() ;
    Sleep(time) ;
    MSG dummy ;
    if (PeekMessage(&dummy,handle,
                    WM_RBUTTONDBLCLK,WM_RBUTTONDBLCLK,
                    PM_NOREMOVE)
       )
    {
      PeekMessage(&dummy,handle,WM_RBUTTONUP,WM_RBUTTONUP,PM_REMOVE);
      return; 
    }
  }
#endif

//wxDebugMsg("RButtonDown\n") ;
  wxMouseEvent event(wxEVENT_TYPE_RIGHT_DOWN);
  float px = (float)x;
  float py = (float)y;

  DeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.x, &event.y);

  event.shiftDown = (flags & MK_SHIFT);
  event.controlDown = (flags & MK_CONTROL);
  event.leftDown = (flags & MK_LBUTTON);
  event.middleDown = (flags & MK_MBUTTON);
  event.rightDown = (flags & MK_RBUTTON);
  event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */
  event.eventObject = wx_window;

  last_x_pos = event.x; last_y_pos = event.y; last_event = wxEVENT_TYPE_RIGHT_DOWN;
  if (wx_window) wx_window->GetEventHandler()->OnEvent(event);
}

void wxSubWnd::OnRButtonUp(int x, int y, UINT flags)
{
//wxDebugMsg("RButtonUp\n") ;
  wxMouseEvent event(wxEVENT_TYPE_RIGHT_UP);
  float px = (float)x;
  float py = (float)y;

  DeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.x, &event.y);

  event.shiftDown = (flags & MK_SHIFT);
  event.controlDown = (flags & MK_CONTROL);
  event.leftDown = (flags & MK_LBUTTON);
  event.middleDown = (flags & MK_MBUTTON);
  event.rightDown = (flags & MK_RBUTTON);
  event.eventObject = wx_window;
  event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */

  last_x_pos = event.x; last_y_pos = event.y; last_event = wxEVENT_TYPE_RIGHT_UP;
  if (wx_window) wx_window->GetEventHandler()->OnEvent(event);
}

void wxSubWnd::OnRButtonDClick(int x, int y, UINT flags)
{
//wxDebugMsg("RButtonDClick\n") ;
  /* MATTHEW: If dclick not allowed, generate another single-click */
  wxMouseEvent event((wx_window && wx_window->doubleClickAllowed) ?
		     wxEVENT_TYPE_RIGHT_DCLICK : wxEVENT_TYPE_RIGHT_DOWN);

  float px = (float)x;
  float py = (float)y;

  DeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.x, &event.y);

  event.shiftDown = (flags & MK_SHIFT);
  event.controlDown = (flags & MK_CONTROL);
  event.leftDown = (flags & MK_LBUTTON);
  event.middleDown = (flags & MK_MBUTTON);
  event.rightDown = (flags & MK_RBUTTON);
  event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */
  event.eventObject = wx_window;

  last_x_pos = event.x; last_y_pos = event.y; last_event = wxEVENT_TYPE_RIGHT_DCLICK;
  if (wx_window && wx_window->doubleClickAllowed) wx_window->GetEventHandler()->OnEvent(event);
}

void wxSubWnd::OnMouseMove(int x, int y, UINT flags)
{
//wxDebugMsg("Client 0x%08x Move Msg %d,%d\n",this,x,y) ;

// #if MOUSE_EXIT_FIX //Should work now!!

  // Don't do the Leave/Enter fix if we've captured the window,
  // or SetCapture won't work properly.
  if (wx_window && !wx_window->winCaptured)
  {
    HWND hunder ;
    POINT pt ;
    // See if we Leave/Enter the window.
    GetCursorPos(&pt) ;
    hunder = WindowFromPoint(pt) ;
    if (hunder==handle)
    {
      // I'm in the Window, but perhaps in NC area.
      RECT wind ;
      RECT nc ;
      GetClientRect(handle,&wind) ;
      GetWindowRect(handle,&nc) ;
      pt.x -= nc.left ;
      pt.y -= nc.top ;
      wind.left    += WINDOW_MARGIN ; // to be able to 'see' leave
      wind.top     += WINDOW_MARGIN ; // to be able to 'see' leave
      wind.right   -= WINDOW_MARGIN ; // to be able to 'see' leave
      wind.bottom  -= WINDOW_MARGIN ; // to be able to 'see' leave

      if (!PtInRect(&wind,pt))
        hunder = NULL ; // So, I can simulate a Leave event...
    }

    if (hunder!=handle)
    {
      if (mouse_in_window)
      {
        mouse_in_window = FALSE ;
        // Capture/Release is no more needed...
        //ReleaseCapture() ;
        OnMouseLeave(x,y,flags) ;
        return ;
      }
      // We never want to see Enter or Motion in this part of the Window...
      return ;
    }
    else
    {
      // Event was triggered while I'm really into my client area.
      // Do an Enter if not done.
      if (!mouse_in_window)
      {
        mouse_in_window = TRUE ;
        // Capture/Release is no more needed...
        //SetCapture(handle) ;
        // Set cursor, but only if we're not in 'busy' mode
        if (wx_window->wx_cursor && !wxIsBusy())
          ::SetCursor(wx_window->wx_cursor->ms_cursor);
        OnMouseEnter(x,y,flags) ;
        return ;
      }
    }
  }
// #endif //MOUSE_EXIT_FIX
    
  // 'normal' move event...
  // Set cursor, but only if we're not in 'busy' mode
  if (wx_window->wx_cursor && !wxIsBusy())
    ::SetCursor(wx_window->wx_cursor->ms_cursor);

  wxMouseEvent event(wxEVENT_TYPE_MOTION);
  float px = (float)x;
  float py = (float)y;

  DeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.x, &event.y);

  event.shiftDown = (flags & MK_SHIFT);
  event.controlDown = (flags & MK_CONTROL);
  event.leftDown = (flags & MK_LBUTTON);
  event.middleDown = (flags & MK_MBUTTON);
  event.rightDown = (flags & MK_RBUTTON);
  event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */
  event.eventObject = wx_window;

  // Window gets a click down message followed by a mouse move
  // message even if position isn't changed!  We want to discard
  // the trailing move event if x and y are the same.
  if ((last_event == wxEVENT_TYPE_RIGHT_DOWN || last_event == wxEVENT_TYPE_LEFT_DOWN ||
       last_event == wxEVENT_TYPE_MIDDLE_DOWN) &&
      (last_x_pos == event.x && last_y_pos == event.y))
  {
    last_x_pos = event.x; last_y_pos = event.y;
    last_event = wxEVENT_TYPE_MOTION;
    return;
  }

  last_event = wxEVENT_TYPE_MOTION;
  last_x_pos = event.x; last_y_pos = event.y;
  if (wx_window) wx_window->GetEventHandler()->OnEvent(event);
}

void wxSubWnd::OnMouseEnter(int x, int y, UINT flags)
{
//wxDebugMsg("Client 0x%08x Enter %d,%d\n",this,x,y) ;

  // Set cursor, but only if we're not in 'busy' mode
  if (wx_window->wx_cursor && !wxIsBusy())
    ::SetCursor(wx_window->wx_cursor->ms_cursor);

  wxMouseEvent event(wxEVENT_TYPE_ENTER_WINDOW);
  float px = (float)x;
  float py = (float)y;

  DeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.x, &event.y);

  event.shiftDown = (flags & MK_SHIFT);
  event.controlDown = (flags & MK_CONTROL);
  event.leftDown = (flags & MK_LBUTTON);
  event.middleDown = (flags & MK_MBUTTON);
  event.rightDown = (flags & MK_RBUTTON);
  event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */
  event.eventObject = wx_window;

  last_event = wxEVENT_TYPE_ENTER_WINDOW;
  last_x_pos = event.x; last_y_pos = event.y;
  if (wx_window) wx_window->GetEventHandler()->OnEvent(event);
}

void wxSubWnd::OnMouseLeave(int x, int y, UINT flags)
{
//wxDebugMsg("Client 0x%08x Leave %d,%d\n",this,x,y) ;

  // Set cursor, but only if we're not in 'busy' mode
  if (wx_window->wx_cursor && !wxIsBusy())
    ::SetCursor(wx_window->wx_cursor->ms_cursor);

  wxMouseEvent event(wxEVENT_TYPE_LEAVE_WINDOW);
  float px = (float)x;
  float py = (float)y;

  DeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.x, &event.y);

  event.shiftDown = (flags & MK_SHIFT);
  event.controlDown = (flags & MK_CONTROL);
  event.leftDown = (flags & MK_LBUTTON);
  event.middleDown = (flags & MK_MBUTTON);
  event.rightDown = (flags & MK_RBUTTON);
  event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */
  event.eventObject = wx_window;

  last_event = wxEVENT_TYPE_LEAVE_WINDOW;
  last_x_pos = event.x; last_y_pos = event.y;
  if (wx_window) wx_window->GetEventHandler()->OnEvent(event);
}

void wxSubWnd::OnChar(WORD wParam, LPARAM lParam, Bool isASCII)
{
  int id;
  Bool tempControlDown = FALSE;
  if (isASCII)
  {
    // If 1 -> 26, translate to CTRL plus a letter.
    id = wParam;
    if ((id > 0) && (id < 27))
    {
      switch (id)
      {
        case 13:
        {
          id = WXK_RETURN;
          break;
        }
        case 8:
        {
          id = WXK_BACK;
          break;
        }
        case 9:
        {
          id = WXK_TAB;
          break;
        }
        default:
        {
          tempControlDown = TRUE;
          id = id + 96;
        }
      }
    }
  }
  else
    if ((id = wxCharCodeMSWToWX(wParam)) == 0)
      id = -1;

  if ((id > -1) && wx_window)
  {
    wxKeyEvent event(wxEVENT_TYPE_CHAR);
//    if (wxShiftDown)
    event.shiftDown = (::GetKeyState(VK_SHIFT)&0x100?TRUE:FALSE);
//    if (wxControlDown || tempControlDown)
    event.controlDown = (::GetKeyState(VK_CONTROL)&0x100?TRUE:FALSE);
    if ((HIWORD(lParam) & KF_ALTDOWN) == KF_ALTDOWN)
      event.altDown = TRUE;

    event.eventObject = wx_window;
    event.keyCode = id;
    event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */

    POINT pt ;
    GetCursorPos(&pt) ;
    RECT rect ;
    GetWindowRect(handle,&rect) ;
    pt.x -= rect.left ;
    pt.y -= rect.top ;
    float fx,fy ;
    fx = (float)pt.x ;
    fy = (float)pt.y ;
    DeviceToLogical(&fx,&fy) ;
    CalcUnscrolledPosition((int)fx,(int)fy,&event.x,&event.y) ;

    wx_window->GetEventHandler()->OnChar(event);
  }
}

void wxSubWnd::OnVScroll(WORD wParam, WORD pos, HWND control)
{
  if (control)
  {
    wxNode *node = (wxNode *)wxScrollBarList.Find((long)control);
    if (!node)
      return;
    wxWindow * win = (wxWindow *)node->Data();
    if (win->IsKindOf(CLASSINFO(wxSlider)))
    {
    	wxSliderEvent(control, wParam, pos);
	return;
    }
#if USE_SCROLLBAR
    else if (win->IsKindOf(CLASSINFO(wxScrollBar)))
    {
    	wxScrollBarEvent(control, wParam, pos);
	return;
    }
#endif
    return;
  }

        wxCommandEvent event;
        event.commandInt = pos;
        event.extraLong = wxVERTICAL;
	switch ( wParam )
	{
		case SB_TOP:
			event.eventType = wxEVENT_TYPE_SCROLL_TOP;
			break;

		case SB_BOTTOM:
			event.eventType = wxEVENT_TYPE_SCROLL_BOTTOM;
			break;

		case SB_LINEUP:
			event.eventType = wxEVENT_TYPE_SCROLL_LINEUP;
			break;

		case SB_LINEDOWN:
		        event.eventType = wxEVENT_TYPE_SCROLL_LINEDOWN;
			break;

		case SB_PAGEUP:
                        event.eventType = wxEVENT_TYPE_SCROLL_PAGEUP;
			break;

		case SB_PAGEDOWN:
                        event.eventType = wxEVENT_TYPE_SCROLL_PAGEDOWN;
			break;

                case SB_THUMBTRACK:
                        event.eventType = wxEVENT_TYPE_SCROLL_THUMBTRACK;
			break;

		default:
                        return;
                        break;
	}
	if (wx_window)
          wx_window->DoScroll(event);
}

void wxSubWnd::OnHScroll( WORD wParam, WORD pos, HWND control)
{
  if (control)
  {
    wxNode *node = (wxNode *)wxScrollBarList.Find((long)control);
    if (!node)
      return;
    wxWindow * win = (wxWindow *)node->Data();
    if (win->IsKindOf(CLASSINFO(wxSlider)))
    {
      wxSliderEvent(control, wParam, pos);
      return;
    }
#if USE_SCROLLBAR
    else if (win->IsKindOf(CLASSINFO(wxScrollBar)))
    {
      wxScrollBarEvent(control, wParam, pos);
      return;
    }
#endif
    return;
  }
  else
  {
        wxCommandEvent event;
        event.commandInt = pos;
        event.extraLong = wxHORIZONTAL;
	switch ( wParam )
	{
		case SB_TOP:
			event.eventType = wxEVENT_TYPE_SCROLL_TOP;
			break;

		case SB_BOTTOM:
			event.eventType = wxEVENT_TYPE_SCROLL_BOTTOM;
			break;

		case SB_LINEUP:
			event.eventType = wxEVENT_TYPE_SCROLL_LINEUP;
			break;

		case SB_LINEDOWN:
		        event.eventType = wxEVENT_TYPE_SCROLL_LINEDOWN;
			break;

		case SB_PAGEUP:
                        event.eventType = wxEVENT_TYPE_SCROLL_PAGEUP;
			break;

		case SB_PAGEDOWN:
                        event.eventType = wxEVENT_TYPE_SCROLL_PAGEDOWN;
			break;

                case SB_THUMBTRACK:
                        event.eventType = wxEVENT_TYPE_SCROLL_THUMBTRACK;
			break;

		default:
                        return;
                        break;
	}
	if (wx_window)
          wx_window->DoScroll(event);
  }
}

void wxGetCharSize(HWND wnd, int *x, int *y,wxFont *the_font)
{
  TEXTMETRIC tm;
  HDC dc = GetDC(wnd);
  HFONT fnt =0;
  HFONT was = 0;
  if (the_font)
  {
#if DEBUG > 1
    wxDebugMsg("wxGetCharSize: Selecting HFONT %X\n", fnt);
#endif
    the_font->UseResource();
    the_font->RealizeResource();
    if ((fnt=the_font->GetResourceHandle()))
      was = SelectObject(dc,fnt) ;
  }
  GetTextMetrics(dc, &tm);
  if (the_font && fnt && was)
  {
#if DEBUG > 1
    wxDebugMsg("wxGetCharSize: Selecting old HFONT %X\n", was);
#endif
    SelectObject(dc,was) ;
  }
  ReleaseDC(wnd, dc);
  *x = tm.tmAveCharWidth;
  *y = tm.tmHeight + tm.tmExternalLeading;

  if (the_font)
    the_font->ReleaseResource();
}

// Returns 0 if was a normal ASCII value, not a special key. This indicates that
// the key should be ignored by WM_KEYDOWN and processed by WM_CHAR instead.
int wxCharCodeMSWToWX(int keySym)
{
  int id = 0;
  switch (keySym)
  {
    case VK_CANCEL:             id = WXK_CANCEL; break;
    case VK_BACK:               id = WXK_BACK; break;
    case VK_TAB:	        id = WXK_TAB; break;
    case VK_CLEAR:		id = WXK_CLEAR; break;
    case VK_RETURN:		id = WXK_RETURN; break;
    case VK_SHIFT:		id = WXK_SHIFT; break;
    case VK_CONTROL:		id = WXK_CONTROL; break;
    case VK_MENU :		id = WXK_MENU; break;
    case VK_PAUSE:		id = WXK_PAUSE; break;
    case VK_SPACE:		id = WXK_SPACE; break;
    case VK_ESCAPE:		id = WXK_ESCAPE; break;
    case VK_PRIOR:		id = WXK_PRIOR; break;
    case VK_NEXT :		id = WXK_NEXT; break;
    case VK_END:		id = WXK_END; break;
    case VK_HOME :		id = WXK_HOME; break;
    case VK_LEFT :		id = WXK_LEFT; break;
    case VK_UP:		        id = WXK_UP; break;
    case VK_RIGHT:		id = WXK_RIGHT; break;
    case VK_DOWN :		id = WXK_DOWN; break;
    case VK_SELECT:		id = WXK_SELECT; break;
    case VK_PRINT:		id = WXK_PRINT; break;
    case VK_EXECUTE:		id = WXK_EXECUTE; break;
    case VK_INSERT:		id = WXK_INSERT; break;
    case VK_DELETE:		id = WXK_DELETE; break;
    case VK_HELP :		id = WXK_HELP; break;
    case VK_NUMPAD0:		id = WXK_NUMPAD0; break;
    case VK_NUMPAD1:		id = WXK_NUMPAD1; break;
    case VK_NUMPAD2:		id = WXK_NUMPAD2; break;
    case VK_NUMPAD3:		id = WXK_NUMPAD3; break;
    case VK_NUMPAD4:		id = WXK_NUMPAD4; break;
    case VK_NUMPAD5:		id = WXK_NUMPAD5; break;
    case VK_NUMPAD6:		id = WXK_NUMPAD6; break;
    case VK_NUMPAD7:		id = WXK_NUMPAD7; break;
    case VK_NUMPAD8:		id = WXK_NUMPAD8; break;
    case VK_NUMPAD9:		id = WXK_NUMPAD9; break;
    case VK_MULTIPLY:		id = WXK_MULTIPLY; break;
    case VK_ADD:		id = WXK_ADD; break;
    case VK_SUBTRACT:		id = WXK_SUBTRACT; break;
    case VK_DECIMAL:		id = WXK_DECIMAL; break;
    case VK_DIVIDE:		id = WXK_DIVIDE; break;
    case VK_F1:		id = WXK_F1; break;
    case VK_F2:		id = WXK_F2; break;
    case VK_F3:		id = WXK_F3; break;
    case VK_F4:		id = WXK_F4; break;
    case VK_F5:		id = WXK_F5; break;
    case VK_F6:		id = WXK_F6; break;
    case VK_F7:		id = WXK_F7; break;
    case VK_F8:		id = WXK_F8; break;
    case VK_F9:		id = WXK_F9; break;
    case VK_F10:		id = WXK_F10; break;
    case VK_F11:		id = WXK_F11; break;
    case VK_F12:		id = WXK_F12; break;
    case VK_F13:		id = WXK_F13; break;
    case VK_F14:		id = WXK_F14; break;
    case VK_F15:		id = WXK_F15; break;
    case VK_F16:		id = WXK_F16; break;
    case VK_F17:		id = WXK_F17; break;
    case VK_F18:		id = WXK_F18; break;
    case VK_F19:		id = WXK_F19; break;
    case VK_F20:		id = WXK_F20; break;
    case VK_F21:		id = WXK_F21; break;
    case VK_F22:		id = WXK_F22; break;
    case VK_F23:		id = WXK_F23; break;
    case VK_F24:		id = WXK_F24; break;
    case VK_NUMLOCK:		id = WXK_NUMLOCK; break;
    case VK_SCROLL:		id = WXK_SCROLL; break;
    default:
    {
      return 0;
    }
  }
  return id;
}

int wxCharCodeWXToMSW(int id, Bool *isVirtual)
{
  *isVirtual = TRUE;
  int keySym = 0;
  switch (id)
  {
    case WXK_CANCEL:            keySym = VK_CANCEL; break;
    case WXK_CLEAR:		keySym = VK_CLEAR; break;
    case WXK_SHIFT:		keySym = VK_SHIFT; break;
    case WXK_CONTROL:		keySym = VK_CONTROL; break;
    case WXK_MENU :		keySym = VK_MENU; break;
    case WXK_PAUSE:		keySym = VK_PAUSE; break;
    case WXK_PRIOR:		keySym = VK_PRIOR; break;
    case WXK_NEXT :		keySym = VK_NEXT; break;
    case WXK_END:		keySym = VK_END; break;
    case WXK_HOME :		keySym = VK_HOME; break;
    case WXK_LEFT :		keySym = VK_LEFT; break;
    case WXK_UP:		keySym = VK_UP; break;
    case WXK_RIGHT:		keySym = VK_RIGHT; break;
    case WXK_DOWN :		keySym = VK_DOWN; break;
    case WXK_SELECT:		keySym = VK_SELECT; break;
    case WXK_PRINT:		keySym = VK_PRINT; break;
    case WXK_EXECUTE:		keySym = VK_EXECUTE; break;
    case WXK_INSERT:		keySym = VK_INSERT; break;
    case WXK_DELETE:		keySym = VK_DELETE; break;
    case WXK_HELP :		keySym = VK_HELP; break;
    case WXK_NUMPAD0:		keySym = VK_NUMPAD0; break;
    case WXK_NUMPAD1:		keySym = VK_NUMPAD1; break;
    case WXK_NUMPAD2:		keySym = VK_NUMPAD2; break;
    case WXK_NUMPAD3:		keySym = VK_NUMPAD3; break;
    case WXK_NUMPAD4:		keySym = VK_NUMPAD4; break;
    case WXK_NUMPAD5:		keySym = VK_NUMPAD5; break;
    case WXK_NUMPAD6:		keySym = VK_NUMPAD6; break;
    case WXK_NUMPAD7:		keySym = VK_NUMPAD7; break;
    case WXK_NUMPAD8:		keySym = VK_NUMPAD8; break;
    case WXK_NUMPAD9:		keySym = VK_NUMPAD9; break;
    case WXK_MULTIPLY:		keySym = VK_MULTIPLY; break;
    case WXK_ADD:		keySym = VK_ADD; break;
    case WXK_SUBTRACT:		keySym = VK_SUBTRACT; break;
    case WXK_DECIMAL:		keySym = VK_DECIMAL; break;
    case WXK_DIVIDE:		keySym = VK_DIVIDE; break;
    case WXK_F1:		keySym = VK_F1; break;
    case WXK_F2:		keySym = VK_F2; break;
    case WXK_F3:		keySym = VK_F3; break;
    case WXK_F4:		keySym = VK_F4; break;
    case WXK_F5:		keySym = VK_F5; break;
    case WXK_F6:		keySym = VK_F6; break;
    case WXK_F7:		keySym = VK_F7; break;
    case WXK_F8:		keySym = VK_F8; break;
    case WXK_F9:		keySym = VK_F9; break;
    case WXK_F10:		keySym = VK_F10; break;
    case WXK_F11:		keySym = VK_F11; break;
    case WXK_F12:		keySym = VK_F12; break;
    case WXK_F13:		keySym = VK_F13; break;
    case WXK_F14:		keySym = VK_F14; break;
    case WXK_F15:		keySym = VK_F15; break;
    case WXK_F16:		keySym = VK_F16; break;
    case WXK_F17:		keySym = VK_F17; break;
    case WXK_F18:		keySym = VK_F18; break;
    case WXK_F19:		keySym = VK_F19; break;
    case WXK_F20:		keySym = VK_F20; break;
    case WXK_F21:		keySym = VK_F21; break;
    case WXK_F22:		keySym = VK_F22; break;
    case WXK_F23:		keySym = VK_F23; break;
    case WXK_F24:		keySym = VK_F24; break;
    case WXK_NUMLOCK:		keySym = VK_NUMLOCK; break;
    case WXK_SCROLL:		keySym = VK_SCROLL; break;
    default:
    {
      *isVirtual = FALSE;
      keySym = id;
      break;
    }
  }
  return keySym;
}

void wxWindow::DoScroll(wxCommandEvent& event)
{
  long orient = event.extraLong;

  int nScrollInc = CalcScrollInc(event);
  if (nScrollInc == 0)
    return;

  wxWnd *wnd = (wxWnd *)handle;
  HWND hWnd = GetHWND();

  if (orient == wxHORIZONTAL)
  {
    int newPos = wnd->xscroll_position + nScrollInc;
    ::SetScrollPos(hWnd, SB_HORZ, newPos, TRUE );
  }
  else
  {
    int newPos = wnd->yscroll_position + nScrollInc;
    ::SetScrollPos(hWnd, SB_VERT, newPos, TRUE );
  }

  OnScroll(event);

  if (orient == wxHORIZONTAL)
  {
    wnd->xscroll_position += nScrollInc;
  }
  else
  {
    wnd->yscroll_position += nScrollInc;
  }
}

int wxWindow::CalcScrollInc(wxCommandEvent& event)
{
  int pos = event.commandInt;
  long orient = event.extraLong;

  int nScrollInc = 0;
  wxWnd *wnd = (wxWnd *)handle;

  switch (event.GetEventType())
  {
    case wxEVENT_TYPE_SCROLL_TOP:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = - wnd->xscroll_position;
      else
        nScrollInc = - wnd->yscroll_position;
      break;
    }
    case wxEVENT_TYPE_SCROLL_BOTTOM:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = wnd->xscroll_lines - wnd->xscroll_position;
      else
        nScrollInc = wnd->yscroll_lines - wnd->yscroll_position;
      break;
    }
    case wxEVENT_TYPE_SCROLL_LINEUP:
    {
      nScrollInc = -1;
      break;
    }
    case wxEVENT_TYPE_SCROLL_LINEDOWN:
    {
      nScrollInc = 1;
      break;
    }
    case wxEVENT_TYPE_SCROLL_PAGEUP:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = -wnd->xscroll_lines_per_page;
      else
        nScrollInc = -wnd->yscroll_lines_per_page;
      break;
    }
    case wxEVENT_TYPE_SCROLL_PAGEDOWN:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = wnd->xscroll_lines_per_page;
      else
        nScrollInc = wnd->yscroll_lines_per_page;
      break;
    }
    case wxEVENT_TYPE_SCROLL_THUMBTRACK:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = pos - wnd->xscroll_position;
      else
        nScrollInc = pos - wnd->yscroll_position;
      break;
    }
    default:
    {
      break;
    }
  }
  if (orient == wxHORIZONTAL)
  {
    // We're scrolling automatically
    if (wnd->calcScrolledOffset)
    {
        int w, h;
		GetClientSize(&w, &h);

    	int nMaxWidth = wnd->xscroll_lines*wnd->xscroll_pixels_per_line;
    	int noPositions = (int) ( ((nMaxWidth - w)/(float)wnd->xscroll_pixels_per_line) + 0.5 );
    	if (noPositions < 0)
      		noPositions = 0;

		if ( (wnd->xscroll_position + nScrollInc) < 0 )
			nScrollInc = -wnd->xscroll_position; // As -ve as we can go
		else if ( (wnd->xscroll_position + nScrollInc) > noPositions )
			nScrollInc = noPositions - wnd->xscroll_position; // As +ve as we can go

        return nScrollInc;
    }
    else
    {
      // We're not scrolling automatically so we don't care about pixel-per-line
      /* MATTHEW: Fix scrolling calculation */
      int newPosition = wnd->xscroll_position + nScrollInc;
      if (newPosition < 0)
	return -wnd->xscroll_position;
      else if (newPosition > wnd->xscroll_lines)
	return wnd->xscroll_lines - wnd->xscroll_position;
      else
        return nScrollInc;
    }
  }
  else
  {
    // We're scrolling automatically
    if (wnd->calcScrolledOffset)
    {
        int w, h;
		GetClientSize(&w, &h);

    	int nMaxHeight = wnd->yscroll_lines*wnd->yscroll_pixels_per_line;
    	int noPositions = (int) ( ((nMaxHeight - h)/(float)wnd->yscroll_pixels_per_line) + 0.5 );
    	if (noPositions < 0)
      		noPositions = 0;

		if ( (wnd->yscroll_position + nScrollInc) < 0 )
			nScrollInc = -wnd->yscroll_position; // As -ve as we can go
		else if ( (wnd->yscroll_position + nScrollInc) > noPositions )
			nScrollInc = noPositions - wnd->yscroll_position; // As +ve as we can go

        return nScrollInc;

/*
        RECT rect;
        GetClientRect(hWnd, &rect);
        int h = rect.bottom - rect.top;

        int nMaxHeight = wnd->yscroll_lines*wnd->yscroll_pixels_per_line;

        float noPositions = (float)((nMaxHeight - h)/(float)wnd->yscroll_pixels_per_line);

        // Deal with situation where we need to scroll a fractional
        // amount: scroll by 1 instead (ok, so we step slightly
        // outside the scrollable area but tough.)
        if (noPositions > 0 && noPositions < 1)
          noPositions = (float)1;
        else if (noPositions < 0)
          noPositions = (float)0;

        int nVscrollMax = wxMax(0, (int)(0 + noPositions));

        nScrollInc = wxMax( -wnd->yscroll_position,
                        wxMin( nScrollInc, nVscrollMax - wnd->yscroll_position ) );
        return nScrollInc;
*/
    }
    else
    {
      // We're not scrolling automatically so we don't care about pixel-per-line
      /* MATTHEW: Fix scrolling calculation */
      int newPosition = wnd->yscroll_position + nScrollInc;
      if (newPosition < 0)
	return -wnd->yscroll_position;
      else if (newPosition > wnd->yscroll_lines)
	return wnd->yscroll_lines - wnd->yscroll_position;
      else
	return nScrollInc;
    }
  }
}

void wxWindow::OnScroll(wxCommandEvent& event)
{
  long orient = event.extraLong;

  int nScrollInc = CalcScrollInc(event);
  if (nScrollInc == 0)
    return;

  wxWnd *wnd = (wxWnd *)handle;
  HWND hWnd = GetHWND();
    
  if (orient == wxHORIZONTAL)
  {
    if (wnd->x_scrolling_enabled)
      ::ScrollWindow(hWnd, -wnd->xscroll_pixels_per_line * nScrollInc, 0, NULL, NULL );
    else
      InvalidateRect(hWnd, NULL, FALSE);
  }
  else
  {
    if (wnd->y_scrolling_enabled)
      ::ScrollWindow(hWnd, 0, -wnd->yscroll_pixels_per_line * nScrollInc, NULL, NULL );
    else
      InvalidateRect(hWnd, NULL, FALSE);
  }
}

void wxWindow::SetScrollPos(int orient, int pos)
{
  int wOrient ;
  if (orient == wxHORIZONTAL)
    wOrient = SB_HORZ;
  else
    wOrient = SB_VERT;
    
  HWND hWnd = GetHWND();
  if (hWnd)
    ::SetScrollPos(hWnd, wOrient, pos, TRUE);

  wxWnd *wnd = (wxWnd *)handle;
  if (orient == wxHORIZONTAL)
    wnd->xscroll_position = pos;
  else
    wnd->yscroll_position = pos;
}

void wxWindow::SetScrollRange(int orient, int range)
{
#if WIN95

  int range1 = range;

  // Try to adjust the range to cope with page size > 1
  // - a Windows API quirk
  int pageSize = GetScrollPage(orient);
  if ( pageSize > 1 && range != 0)
  {
	range1 += (pageSize - 1);
  }

  SCROLLINFO info;
  int dir;

  if (orient == wxHORIZONTAL) {
	dir = SB_HORZ;
  } else {
	dir = SB_VERT;
  }

  info.cbSize = sizeof(SCROLLINFO);
  info.nPage = pageSize;
  info.nMin = 0;
  info.nMax = range1;
  info.nPos = 0;
  info.fMask = SIF_RANGE | SIF_PAGE;

  HWND hWnd = (HWND) GetHWND();
  if (hWnd)
    ::SetScrollInfo(hWnd, dir, &info, TRUE);
#else
  int wOrient ;
  if (orient == wxHORIZONTAL)
    wOrient = SB_HORZ;
  else
    wOrient = SB_VERT;
    
  HWND hWnd = (HWND) GetHWND();
  if (hWnd)
    ::SetScrollRange(hWnd, wOrient, 0, range, TRUE);
#endif
}

void wxWindow::SetScrollPage(int orient, int page)
{
#if WIN95 // defined(WIN32) && !defined(__WATCOMC__)
  SCROLLINFO info;
  int dir; // , range;
  wxWnd *wnd = (wxWnd *)handle;

  if (orient == wxHORIZONTAL) {
	dir = SB_HORZ;
    wnd->xscroll_lines_per_page = page;
//	range = wnd->xscroll_lines;
  } else {
	dir = SB_VERT;
    wnd->yscroll_lines_per_page = page;
//	range = wnd->yscroll_lines;
  }

  info.cbSize = sizeof(SCROLLINFO);
  info.nPage = page;
//  info.nMin = 0;
//  info.nMax = range + page - 1;
  info.fMask = SIF_PAGE ; // | SIF_RANGE;

  HWND hWnd = GetHWND();
  if (hWnd)
    ::SetScrollInfo(hWnd, dir, &info, TRUE);
#else
  wxWnd *wnd = (wxWnd *)handle;
  if (orient == wxHORIZONTAL)
    wnd->xscroll_lines_per_page = page;
  else
    wnd->yscroll_lines_per_page = page;
#endif
}

int wxWindow::GetScrollPos(int orient)
{
  int wOrient ;
  if (orient == wxHORIZONTAL)
    wOrient = SB_HORZ;
  else
    wOrient = SB_VERT;
  HWND hWnd = GetHWND();
  if (hWnd)
  {
    return ::GetScrollPos(hWnd, wOrient);
  }
  else
    return 0;
}

int wxWindow::GetScrollRange(int orient)
{
  int wOrient ;
  if (orient == wxHORIZONTAL)
    wOrient = SB_HORZ;
  else
    wOrient = SB_VERT;

#if __WATCOMC__ && defined(__WINDOWS_386__)
  short minPos, maxPos;
#else
  int minPos, maxPos;
#endif
  HWND hWnd = GetHWND();
  if (hWnd)
  {
    ::GetScrollRange(hWnd, wOrient, &minPos, &maxPos);
    return maxPos;
  }
  else
    return 0;
}

int wxWindow::GetScrollPage(int orient)
{
  wxWnd *wnd = (wxWnd *)handle;
  if (orient == wxHORIZONTAL)
    return wnd->xscroll_lines_per_page;
  else
    return wnd->yscroll_lines_per_page;
}

// Default OnSize resets scrollbars, if any
void wxWindow::OnSize(int w, int h)
{
#if USE_CONSTRAINTS
  if (GetAutoLayout())
    Layout();
#endif
}

// Caret manipulation
void wxWindow::CreateCaret(int w, int h)
{
  caretWidth = w;
  caretHeight = h;
  caretEnabled = TRUE;
}

void wxWindow::CreateCaret(wxBitmap *WXUNUSED(bitmap))
{
  // Not implemented
}

void wxWindow::ShowCaret(Bool show)
{
  if (caretEnabled)
  {
    if (show)
      ::ShowCaret(GetHWND());
    else
      ::HideCaret(GetHWND());
    caretShown = show;
  }
}

void wxWindow::DestroyCaret(void)
{
  caretEnabled = FALSE;
}

void wxWindow::SetCaretPos(int x, int y)
{
  ::SetCaretPos(x, y);
}

void wxWindow::GetCaretPos(int *x, int *y)
{
  POINT point;
  ::GetCaretPos(&point);
  *x = point.x;
  *y = point.y;
}

/*
 * Update iterator. Use from within OnPaint.
 */
 
wxUpdateIterator::wxUpdateIterator(wxWindow* wnd)
{
  current = 0;					//start somewhere...
#ifdef WIN32
#ifndef __win32s__
  rlist = NULL;					//make sure I don't free randomly
  int len = GetRegionData(wnd->updateRgn,0,NULL);	//Get buffer size
  if (len)
  {
    rlist = (RGNDATA *)new char[len];
    GetRegionData(wnd->updateRgn,len,rlist);
    rp = (RECT*)rlist->Buffer;
    rects = rlist->rdh.nCount;
  }
  else
#endif
  {
    rects = 1;
    rp = &wnd->updateRect;			//Only one available in Win16,32s
  }
#else
  rects = 1;
  rp = &wnd->updateRect;			//Only one available in Win16,32s
#endif
}

wxUpdateIterator::~wxUpdateIterator(void)
{
#ifdef WIN32
#ifndef __win32s__
  if (rlist) delete rlist;
#endif
#endif
}

// Salford C++ doesn't like this.
#ifndef __SALFORD__
wxUpdateIterator::operator int (void)
{
  return (current < rects) ;
}
#endif

Bool wxUpdateIterator::HasRects(void)
{
  return (current < rects) ;
}

wxUpdateIterator* wxUpdateIterator::operator ++(int)
{
  current++;
  return this;
}

RECT* wxUpdateIterator::GetMSWRect(void)
{
  return rp+current;	//ought to error check this...
}

void wxUpdateIterator::GetRect(wxRectangle *rect)
{
  RECT *mswRect = rp+current;	//ought to error check this...
  rect->x = mswRect->left;
  rect->y = mswRect->top;
  rect->width = mswRect->right - mswRect->left;
  rect->height = mswRect->bottom - mswRect->top;
}

int wxUpdateIterator::GetX()
{
  return rp[current].left;
}

int wxUpdateIterator::GetY()
{
  return rp[current].top;
}

int wxUpdateIterator::GetW()
{
 return rp[current].right-GetX();
}

int wxUpdateIterator::GetH()
{
  return rp[current].bottom-GetY();
}

wxWindow *wxGetActiveWindow(void)
{
  HWND hWnd = GetActiveWindow();
  if (hWnd != 0)
  {
    wxWnd *wnd = wxFindWinFromHandle(hWnd);
    if (wnd && wnd->wx_window)
    {
      return wnd->wx_window;
    }
  }
  return NULL;
}

// Windows keyboard hook. Allows interception of e.g. F1, ESCAPE
// in active frames and dialogs, regardless of where the focus is.
static HHOOK wxTheKeyboardHook = 0;
static FARPROC wxTheKeyboardHookProc = 0;
int APIENTRY _EXPORT
  wxKeyboardHook(int nCode, WORD wParam, DWORD lParam);

void wxSetKeyboardHook(Bool doIt)
{
  if (doIt)
  {
    wxTheKeyboardHookProc = MakeProcInstance((FARPROC) wxKeyboardHook, wxhInstance);
    wxTheKeyboardHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC) wxTheKeyboardHookProc, wxhInstance,
#ifdef WIN32
      GetCurrentThreadId());
//      (DWORD)GetCurrentProcess()); // This is another possibility. Which is right?
#else
      GetCurrentTask());
#endif
  }
  else
  {
    UnhookWindowsHookEx(wxTheKeyboardHook);
    // This should probably test for WIN32
#ifndef GNUWIN32
    FreeProcInstance(wxTheKeyboardHookProc);
#endif
  }
}

int APIENTRY _EXPORT
  wxKeyboardHook(int nCode, WORD wParam, DWORD lParam)
{
  DWORD hiWord = HIWORD(lParam);
  if (nCode != HC_NOREMOVE && ((hiWord & KF_UP) == 0))
  {
    int id;
    if ((id = wxCharCodeMSWToWX(wParam)) != 0)
    {
      wxKeyEvent event(wxEVENT_TYPE_CHAR);
      if ((HIWORD(lParam) & KF_ALTDOWN) == KF_ALTDOWN)
        event.altDown = TRUE;
          
      event.eventObject = NULL;
      event.keyCode = id;
/* begin Albert's fix for control and shift key 26.5 */
      event.shiftDown = (::GetKeyState(VK_SHIFT)&0x100?TRUE:FALSE);
      event.controlDown = (::GetKeyState(VK_CONTROL)&0x100?TRUE:FALSE);
/* end Albert's fix for control and shift key 26.5 */
      event.SetTimestamp(last_msg_time); /* MATTHEW: timeStamp */

      if (wxTheApp && wxTheApp->OnCharHook(event))
        return 1;
    }
  }
  return (int)CallNextHookEx(wxTheKeyboardHook, nCode, wParam, lParam);
}

void wxWindow::SetSizeHints(int minW, int minH, int maxW, int maxH, int WXUNUSED(incW), int WXUNUSED(incH))
{
  minSizeX = minW;
  minSizeY = minH;
  maxSizeX = maxW;
  maxSizeY = maxH;
}

