/*
 * File:	wx_gauge.cc
 * Purpose:	Gauge implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	April 1995
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_defs.h"
#endif

#if USE_GAUGE 

#include "wx_panel.h"
#include "wx_gauge.h"
#include "wx_itemp.h"

#include "..\..\contrib\gauge\zyzgauge.h"

// Progress bar is not a good replacement for the gauge
// implementation because:
// (1) Can't see how to specify the number of blocks drawn
//     Gives 7.5 at present!! (see panel sample).
// (2) You don't see a precise value if there are a small
//     number of blocks drawn.
// (3) Doesn't show the value on the gauge.
// (4) Doesn't draw vertically.

#define USE_PROGRESS_BAR 1

#if WIN95
#ifndef GNUWIN32
#include <commctrl.h>
#endif
#endif

wxGauge::wxGauge(void)
{
  wxWinType = wxTYPE_HWND;
  windows_id = 0;
  ms_handle = 0;
  labelhWnd = 0;
  gaugeRange = 100;
  gaugePos = 0;
}

wxGauge::wxGauge(wxPanel *panel, Const char *label,
		   int range, int x, int y, int width, int height,
                   long style, Constdata char *name):
  wxbGauge(panel, label, range, x, y, width, height, style, name)
{
  Create(panel, label, range, x, y, width, height, style, name);
}

Bool wxGauge::Create(wxPanel *panel, Const char *label,
		   int range, int x, int y, int width, int height,
                   long style, Constdata char *name)
{
  SetName(name);
  if (panel) panel->AddChild(this);
  buttonFont = panel->buttonFont ;
  labelFont = panel->labelFont ;
  gaugeRange = range;

  if (buttonFont)
    buttonFont->UseResource();
  if (labelFont)
    labelFont->UseResource();

  labelhWnd = 0;
  wxWinType = wxTYPE_HWND;
  windowStyle = style;

  SetAppropriateLabelPosition();
  panel->GetValidPosition(&x, &y);

  // If label exists, create a static control for it.
  if (label)
  {
    labelhWnd = CreateWindowEx(0, STATIC_CLASS, label,
                         STATIC_FLAGS,
                         0, 0, 0, 0, panel->GetHWND(), (HMENU)NewControlId(),
                         wxhInstance, NULL);
#if CTL3D
    Ctl3dSubclassCtl(labelhWnd);
#endif
    if (labelFont)
    {
      labelFont->RealizeResource();
      if (labelFont->GetResourceHandle())
        SendMessage(labelhWnd,WM_SETFONT,
                  (WPARAM)labelFont->GetResourceHandle(),0L);
    }
  }
  else
    labelhWnd = NULL;

  windows_id = (int)NewControlId();

  // Use the Win95 progress bar if possible, but not if
  // we request a vertical gauge.
#if WIN95 && USE_PROGRESS_BAR
  Bool useWin95Control = FALSE;
  if ((windowStyle & wxGA_PROGRESSBAR) && ((windowStyle & wxHORIZONTAL) == wxHORIZONTAL));
#else
  Bool useWin95Control =   FALSE;
#endif

  if (useWin95Control)
  {
#if WIN95 && USE_PROGRESS_BAR
    long msFlags = WS_CHILD | WS_VISIBLE | WS_TABSTOP;

    HWND wx_button =
      CreateWindowEx(0, PROGRESS_CLASS, label, msFlags,
                    0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                    wxhInstance, NULL);

    ms_handle = (HANDLE)wx_button;

    // Subclass again for purposes of dialog editing mode
    SubclassControl(wx_button);

    SendMessage((HWND)ms_handle, PBM_SETRANGE, 0, MAKELPARAM(0, range));
#endif
  }
  else
  {
    long msFlags = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
/*  if (windowStyle & wxTHREED) */
    msFlags |= ZYZGS_3D;

    HWND wx_button =
      CreateWindowEx(0, "zYzGauge", label, msFlags,
                    0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                    wxhInstance, NULL);

    ms_handle = (HANDLE)wx_button;

    // Subclass again for purposes of dialog editing mode
    SubclassControl(wx_button);

    int wOrient = 0;

    if (windowStyle & wxHORIZONTAL)
      wOrient = ZYZG_ORIENT_LEFTTORIGHT;
    else
      wOrient = ZYZG_ORIENT_BOTTOMTOTOP;
  
    SendMessage(wx_button, ZYZG_SETORIENTATION, wOrient, 0);
    SendMessage(wx_button, ZYZG_SETRANGE, range, 0);

    SendMessage((HWND)ms_handle, ZYZG_SETFGCOLOR, 0, RGB(GetButtonColour()->Red(), GetButtonColour()->Green(), GetButtonColour()->Blue()));
    SendMessage((HWND)ms_handle, ZYZG_SETBKCOLOR, 0, RGB(GetBackgroundColour()->Red(), GetBackgroundColour()->Green(), GetBackgroundColour()->Blue()));
  }

  if (buttonFont)
  {
    buttonFont->RealizeResource();
    
    if (buttonFont->GetResourceHandle())
      SendMessage((HWND)ms_handle,WM_SETFONT,
                (WPARAM)buttonFont->GetResourceHandle(),0L);
  }

  SetBackgroundColour(*panel->GetBackgroundColour()) ;
  labelColour = panel->labelColour ;
  buttonColour = panel->buttonColour ;

  if (width == -1)
    width = 50;
  if (height == -1)
    height = 50;
  SetSize(x, y, width, height);

  ShowWindow((HWND)ms_handle, SW_SHOW);

//  Callback(Function);

  panel->AdvanceCursor(this);
  return TRUE;
}

wxGauge::~wxGauge(void)
{
  isBeingDeleted = TRUE;
  
  if (labelhWnd)
    DestroyWindow(labelhWnd);
  labelhWnd = NULL;
}

void wxGauge::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  int clx; // label font dimensions
  int cly;

  float label_width, label_height, label_x, label_y;
  float control_width, control_height, control_x, control_y;

  // If we're prepared to use the existing size, then...
  if (width == -1 && height == -1 && ((sizeFlags & wxSIZE_AUTO) != wxSIZE_AUTO))
  {
    GetSize(&width, &height);
  }

  // Deal with default size (using -1 values)
  if (width<=0)
    width = DEFAULT_ITEM_WIDTH;

  if (height<=0)
    height = DEFAULT_ITEM_HEIGHT;

  if (labelhWnd)
  {
    // Find size of label
    char buf[300];
    wxGetCharSize((HWND)ms_handle, &clx, &cly,labelFont);
    GetWindowText(labelhWnd, buf, 300);
    GetTextExtent(buf, &label_width, &label_height, NULL, NULL,labelFont);

    // Given size is total label + edit size, find individual
    // control sizes on that basis.
    if (GetLabelPosition() == wxHORIZONTAL)
    {
      label_x = (float)x;
      label_y = (float)y;
      label_width += (float)clx;

      control_x = label_x + label_width + clx;
      control_y = (float)y;
      control_width = width - (control_x - label_x);
      control_height = (float)height;
    }
    else // wxVERTICAL
    {
      label_x = (float)x;
      label_y = (float)y;

      control_x = (float)x;
      control_y = label_y + label_height;
      control_width = (float)width;
      control_height = height - label_height; // - 3;
    }

    MoveWindow(labelhWnd, (int)label_x, (int)label_y,
               (int)label_width, (int)label_height, TRUE);
  }
  else
  {
    control_x = (float)x;
    control_y = (float)y;
    control_width = (float)width;
    control_height = (float)height;
  }

  MoveWindow((HWND)ms_handle, (int)control_x, (int)control_y, (int)control_width, (int)control_height, TRUE);

  GetEventHandler()->OnSize(width, height);
}

void wxGauge::GetSize(int *width, int *height)
{
  RECT rect;
  rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

  wxFindMaxSize((HWND)ms_handle, &rect);

  if (labelhWnd)
  {
    wxFindMaxSize(labelhWnd, &rect);
  }

  *width = rect.right - rect.left;
  *height = rect.bottom - rect.top;
}

void wxGauge::GetPosition(int *x, int *y)
{
  wxWindow *parent = GetParent();
  RECT rect;
  rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

  wxFindMaxSize((HWND)ms_handle, &rect);
  if (labelhWnd)
    wxFindMaxSize(labelhWnd, &rect);

  // Since we now have the absolute screen coords,
  // if there's a parent we must subtract its top left corner
  POINT point;
  point.x = rect.left;
  point.y = rect.top;
  if (parent)
  {
    ::ScreenToClient(parent->GetHWND(), &point);
  }

  *x = point.x;
  *y = point.y;
}

void wxGauge::SetShadowWidth(int w)
{
#if WIN95 && USE_PROGRESS_BAR
  Bool useWin95Control = FALSE;
  if ((windowStyle & wxGA_PROGRESSBAR) && ((windowStyle & wxHORIZONTAL) == wxHORIZONTAL));
#else
  Bool useWin95Control =   FALSE;
#endif
  if (useWin95Control)
  {
  }
  else
    SendMessage((HWND)ms_handle, ZYZG_SETWIDTH3D, w, 0);
}

void wxGauge::SetBezelFace(int w)
{
#if WIN95 && USE_PROGRESS_BAR
  Bool useWin95Control = FALSE;
  if ((windowStyle & wxGA_PROGRESSBAR) && ((windowStyle & wxHORIZONTAL) == wxHORIZONTAL));
#else
  Bool useWin95Control =   FALSE;
#endif
  if (useWin95Control)
  {
  }
  else
    SendMessage((HWND)ms_handle, ZYZG_SETBEZELFACE, w, 0);
}

void wxGauge::SetRange(int r)
{
  gaugeRange = r;
#if WIN95 && USE_PROGRESS_BAR
  Bool useWin95Control = FALSE;
  if ((windowStyle & wxGA_PROGRESSBAR) && ((windowStyle & wxHORIZONTAL) == wxHORIZONTAL));
#else
  Bool useWin95Control =   FALSE;
#endif

#if WIN95 && USE_PROGRESS_BAR
  if (useWin95Control)
    SendMessage((HWND)ms_handle, PBM_SETRANGE, 0, MAKELPARAM(0, r));
  else
#endif
    SendMessage((HWND)ms_handle, ZYZG_SETRANGE, r, 0);
}

void wxGauge::SetValue(int pos)
{
  gaugePos = pos;
  
#if WIN95 && USE_PROGRESS_BAR
  Bool useWin95Control = FALSE;
  if ((windowStyle & wxGA_PROGRESSBAR) && ((windowStyle & wxHORIZONTAL) == wxHORIZONTAL));
#else
  Bool useWin95Control =   FALSE;
#endif

#if WIN95 && USE_PROGRESS_BAR
  if (useWin95Control)
    SendMessage((HWND)ms_handle, PBM_SETPOS, pos, 0);
  else
#endif
    SendMessage((HWND)ms_handle, ZYZG_SETPOSITION, pos, 0);
}

int wxGauge::GetShadowWidth(void)
{
#if WIN95 && USE_PROGRESS_BAR
  Bool useWin95Control = FALSE;
  if ((windowStyle & wxGA_PROGRESSBAR) && ((windowStyle & wxHORIZONTAL) == wxHORIZONTAL));
#else
  Bool useWin95Control =   FALSE;
#endif

  if (useWin95Control)
    return 0;
  else
    return (int) SendMessage((HWND)ms_handle, ZYZG_GETWIDTH3D, 0, 0);
}

int wxGauge::GetBezelFace(void)
{
#if WIN95 && USE_PROGRESS_BAR
  Bool useWin95Control = FALSE;
  if ((windowStyle & wxGA_PROGRESSBAR) && ((windowStyle & wxHORIZONTAL) == wxHORIZONTAL));
#else
  Bool useWin95Control =   FALSE;
#endif

  if (useWin95Control)
    return 0;
  else
    return (int) SendMessage((HWND)ms_handle, ZYZG_GETBEZELFACE, 0, 0);
}

int wxGauge::GetRange(void)
{
#if WIN95 && USE_PROGRESS_BAR
  Bool useWin95Control = FALSE;
  if ((windowStyle & wxGA_PROGRESSBAR) && ((windowStyle & wxHORIZONTAL) == wxHORIZONTAL));
#else
  Bool useWin95Control =   FALSE;
#endif

  if (useWin95Control)
    return gaugeRange;
  else
    return (int) SendMessage((HWND)ms_handle, ZYZG_GETRANGE, 0, 0);
}

int wxGauge::GetValue(void)
{
#if WIN95 && USE_PROGRESS_BAR
  Bool useWin95Control = FALSE;
  if ((windowStyle & wxGA_PROGRESSBAR) && ((windowStyle & wxHORIZONTAL) == wxHORIZONTAL));
#else
  Bool useWin95Control =   FALSE;
#endif

  if (useWin95Control)
    return gaugePos;
  else
    return (int) SendMessage((HWND)ms_handle, ZYZG_GETPOSITION, 0, 0);
}

void wxGauge::SetButtonColour(wxColour& col)
{
#if WIN95 && USE_PROGRESS_BAR
  Bool useWin95Control = FALSE;
  if ((windowStyle & wxGA_PROGRESSBAR) && ((windowStyle & wxHORIZONTAL) == wxHORIZONTAL));
#else
  Bool useWin95Control =   FALSE;
#endif
  if (useWin95Control)
  {
  }
  else
    SendMessage((HWND)ms_handle, ZYZG_SETFGCOLOR, 0, RGB(col.Red(), col.Green(), col.Blue()));
}

void wxGauge::SetBackgroundColour(wxColour& col)
{
#if WIN95 && USE_PROGRESS_BAR
  Bool useWin95Control = FALSE;
  if ((windowStyle & wxGA_PROGRESSBAR) && ((windowStyle & wxHORIZONTAL) == wxHORIZONTAL));
#else
  Bool useWin95Control =   FALSE;
#endif
  if (useWin95Control)
  {
  }
  else
    SendMessage((HWND)ms_handle, ZYZG_SETBKCOLOR, 0, RGB(col.Red(), col.Green(), col.Blue()));
}

#endif
