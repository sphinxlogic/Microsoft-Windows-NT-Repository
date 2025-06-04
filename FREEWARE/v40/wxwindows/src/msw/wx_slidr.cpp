/*
 * File:	wx_slidr.cc
 * Purpose:	Slider implementation
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
#include <stdio.h>
#include "wx_panel.h"
#include "wx_slidr.h"
#endif

#include "wx_itemp.h"

// Can opt to not use track bar under Win95 if you prefer it - set to 0
#define USE_TRACK_BAR 0

#if WIN95
#ifndef GNUWIN32
#include <commctrl.h>
#endif
#endif

wxList wxScrollBarList;

// Slider
wxSlider::wxSlider(void)
{
  wxWinType = wxTYPE_HWND;
  labelhWnd = NULL;
  edit_value = NULL;
  static_min = NULL;
  windows_id = 0;
  page_size = 0;
  s_max = 0;
  s_min = 0;
  ms_handle = 0;
  static_max = 0;
  noTicks = 0;
}

wxSlider::wxSlider(wxPanel *panel, wxFunction func, Const char *label, int value,
           int min_value, int max_value, int width, int x, int y,
           long style, Constdata char *name):
  wxbSlider(panel, func, label, value, min_value, max_value, width, x, y,
            style, name)
{
  Create(panel, func, label, value, min_value, max_value, width, x, y,
         style, name);
}

Bool wxSlider::Create(wxPanel *panel, wxFunction func, Const char *label, int value,
           int min_value, int max_value, int width, int x, int y, long style, Constdata char *name)
{
  SetName(name);
  if (panel) panel->AddChild(this);
  buttonFont = panel->buttonFont ;
  labelFont = panel->labelFont ;
  if (buttonFont)
    buttonFont->UseResource();
  if (labelFont)
    labelFont->UseResource();
  SetBackgroundColour(*panel->GetBackgroundColour()) ;
  labelColour = panel->labelColour ;
  buttonColour = panel->buttonColour ;
  wxWinType = wxTYPE_HWND;
  windowStyle = style;
  noTicks = 0;

  labelPosition = panel->label_position;
  panel->GetValidPosition(&x, &y);

  int cx;
  int cy;
  wxGetCharSize(panel->GetHWND(), &cx, &cy,buttonFont);

  char *the_label = NULL ;

  if (label)
  {
    the_label = new char[strlen(label)+1] ;
    if (style&wxFIXED_LENGTH)
    {
      int i;
      for (i=0;i<(int)strlen(label);i++)
        the_label[i]=MEANING_CHARACTER ;
    }
    else
      strcpy(the_label,label) ;
    the_label[strlen(label)] = '\0' ;
  }

#if WIN95 && USE_TRACK_BAR
  // If label exists, create a static control for it.
  if (label)
  {
    labelhWnd = CreateWindowEx(0, STATIC_CLASS, the_label,
                         STATIC_FLAGS,
                         0, 0, 0, 0, panel->GetHWND(), (HMENU)NewControlId(),
                         wxhInstance, NULL);
#if CTL3D
    if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
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

  // Now create trackbar
  windows_id = (int)NewControlId();

  long msStyle = 0;
  if (windowStyle & wxVERTICAL)
    msStyle = TBS_VERT | TBS_NOTICKS | WS_CHILD | WS_VISIBLE;
  else
    msStyle = TBS_HORZ | TBS_NOTICKS | WS_CHILD | WS_VISIBLE;

  HWND scroll_bar = CreateWindowEx(0, TRACKBAR_CLASS, wxBuffer,
                         msStyle,
                         0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                         wxhInstance, NULL);
/*
#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(scroll_bar);
#endif
*/

  wxScrollBarList.Append((long)scroll_bar, this);

  s_max = max_value;
  s_min = min_value;

  int page_size = (int)((max_value-min_value)/10);

  ::SendMessage(scroll_bar, TBM_SETRANGE, TRUE, MAKELONG(min_value, max_value));
  ::SendMessage(scroll_bar, TBM_SETPOS, TRUE, (LPARAM)value);
  ::SendMessage(scroll_bar, TBM_SETPAGESIZE, 0, (LPARAM)page_size);
//  ShowWindow(scroll_bar, SW_SHOW);

  ms_handle = (HANDLE)scroll_bar;

  // Subclass for purposes of dialog editing mode
  SubclassControl(scroll_bar);

#else
  // non-Win95 implementation
  
  // If label exists, create a static control for it.
  if (label)
  {
    labelhWnd = CreateWindowEx(0, STATIC_CLASS, the_label,
                         STATIC_FLAGS,
                         0, 0, 0, 0, panel->GetHWND(), (HMENU)NewControlId(),
                         wxhInstance, NULL);
#if CTL3D
    if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
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

  long msStyle = ES_AUTOHSCROLL | ES_LEFT | WS_VISIBLE | WS_CHILD |
                           WS_TABSTOP | ES_READONLY | WS_BORDER;

  edit_value = CreateWindowEx(0, "EDIT", NULL,
                           msStyle,
                           0, 0, 0, 0, panel->GetHWND(), (HMENU)NewControlId(),
                           wxhInstance, NULL);
#if CTL3D
    if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
      Ctl3dSubclassCtl(edit_value);
#endif

  // Now create min static control
  sprintf(wxBuffer, "%d", min_value);
  static_min = CreateWindowEx(0, STATIC_CLASS, wxBuffer,
                         STATIC_FLAGS,
                         0, 0, 0, 0, panel->GetHWND(), (HMENU)NewControlId(),
                         wxhInstance, NULL);
#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(static_min);
#endif

  // Now create slider
  windows_id = (int)NewControlId();

  msStyle = 0;
  if (windowStyle & wxVERTICAL)
    msStyle = SBS_VERT | WS_CHILD | WS_VISIBLE;
  else
    msStyle = SBS_HORZ | WS_CHILD | WS_VISIBLE;

#if WIN95
//  msStyle |= WS_EX_CLIENTEDGE;
#endif

  HWND scroll_bar = CreateWindowEx(0, "SCROLLBAR", wxBuffer,
                         msStyle,
                         0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                         wxhInstance, NULL);
#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(scroll_bar);
#endif

  wxScrollBarList.Append((long)scroll_bar, this);

  page_size = (int)((max_value-min_value)/10);
  s_max = max_value;
  s_min = min_value;

  ::SetScrollRange(scroll_bar, SB_CTL, min_value, max_value, FALSE);
  ::SetScrollPos(scroll_bar, SB_CTL, value, FALSE);
  ShowWindow(scroll_bar, SW_SHOW);

  ms_handle = (HANDLE)scroll_bar;

  // Subclass again for purposes of dialog editing mode
  SubclassControl(scroll_bar);

  // Finally, create max value static item
  sprintf(wxBuffer, "%d", max_value);
  static_max = CreateWindowEx(0, STATIC_CLASS, wxBuffer,
                         STATIC_FLAGS,
                         0, 0, 0, 0, panel->GetHWND(), (HMENU)NewControlId(),
                         wxhInstance, NULL);
#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(static_max);
#endif

  if (buttonFont)
  {
    buttonFont->RealizeResource();
    if (buttonFont->GetResourceHandle())
    {
      SendMessage((HWND)static_min,WM_SETFONT,
                  (WPARAM)buttonFont->GetResourceHandle(),0L);
      SendMessage((HWND)static_max,WM_SETFONT,
                  (WPARAM)buttonFont->GetResourceHandle(),0L);
      if (edit_value)
        SendMessage((HWND)edit_value,WM_SETFONT,
                    (WPARAM)buttonFont->GetResourceHandle(),0L);
    }
  }
#endif
  // WIN95
  

  SetSize(x, y, width, -1);
  SetValue(value);

  panel->AdvanceCursor(this);
  Callback(func);

  if (label)
  {
    if (style&wxFIXED_LENGTH)
      SetLabel((char *)label) ;
    if (the_label)
      delete [] the_label ;
  }

  return TRUE;
}

// Called from wx_win.cc: wxWnd::OnHScroll, wxWnd::OnVScroll
void wxSliderEvent(HWND bar, WORD wParam, WORD pos)
{
    wxNode *node = (wxNode *)wxScrollBarList.Find((long)bar);
    if (!node)
      return;

    wxSlider *slider = (wxSlider *)node->Data();
#if WIN95 && USE_TRACK_BAR
    int newPos = (int)::SendMessage(bar, TBM_GETPOS, 0, 0);
/*
    int minValue = GetMin();
    int maxValue = GetMax();
*/

//    if (newPos <= maxValue && newPos >= minValue)
    {
      wxCommandEvent event(wxEVENT_TYPE_SLIDER_COMMAND);
      event.commandInt = newPos;
      event.eventObject = slider;
      slider->ProcessCommand(event);
    }
#else
    int position = GetScrollPos(bar, SB_CTL);

    int nScrollInc;
    switch ( wParam )
    {
            case SB_LINEUP:
                    nScrollInc = -1;
                    break;

            case SB_LINEDOWN:
                    nScrollInc = 1;
                    break;

            case SB_PAGEUP:
                    nScrollInc = -slider->page_size;
                    break;

            case SB_PAGEDOWN:
                    nScrollInc = slider->page_size;;
                    break;

            case SB_THUMBTRACK:
#ifdef WIN32
                    nScrollInc = (signed short)pos - position;
#else
                    nScrollInc = pos - position;
#endif
                    break;

            default:
                    nScrollInc = 0;
    }

    if (nScrollInc != 0)
    {
      int new_pos = position + nScrollInc;
      if (!(new_pos < slider->s_min || new_pos > slider->s_max))
      {
        slider->SetValue(new_pos);
        wxCommandEvent event(wxEVENT_TYPE_SLIDER_COMMAND);
        event.commandInt = new_pos;
        event.eventObject = slider;
        slider->ProcessCommand(event);
      }
    }
#endif
}

wxSlider::~wxSlider(void)
{
  isBeingDeleted = TRUE;
  
    if (static_min)
    {
      DestroyWindow(static_min);
    }
    if (static_max)
    {
      DestroyWindow(static_max);
    }
    if (edit_value)
    {
      DestroyWindow(edit_value);
    }
    if (labelhWnd)
    {
      DestroyWindow(labelhWnd);
    }
    wxScrollBarList.DeleteObject(this);
}

int wxSlider::GetValue(void)
{
#if WIN95 && USE_TRACK_BAR
  return ::SendMessage((HWND)ms_handle, TBM_GETPOS, 0, 0);
#else
  return ::GetScrollPos((HWND)ms_handle, SB_CTL);
#endif
}

char *wxSlider::GetLabel(void)
{
  if (labelhWnd)
  {
    GetWindowText(labelhWnd, wxBuffer, 300);
    return wxBuffer;
  }
  else return NULL;
}

void wxSlider::SetValue(int value)
{
#if WIN95 && USE_TRACK_BAR
  ::SendMessage((HWND)ms_handle, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)value);
#else
  ::SetScrollPos((HWND)ms_handle, SB_CTL, value, TRUE);

  if (edit_value)
  {
    sprintf(wxBuffer, "%d", value);
    SetWindowText(edit_value, wxBuffer);
  }
#endif
}

void wxSlider::SetLabel(char *label)
{
  if (labelhWnd)
  {
    float w, h;
    RECT rect;

    wxWindow *parent = GetParent();
    GetWindowRect(labelhWnd, &rect);

    // Since we now have the absolute screen coords,
    // if there's a parent we must subtract its top left corner
    POINT point;
    point.x = rect.left;
    point.y = rect.top;
    if (parent)
    {
      ::ScreenToClient(parent->GetHWND(), &point);
    }

    GetTextExtent(label, &w, &h, NULL, NULL,labelFont);
    MoveWindow(labelhWnd, point.x, point.y, (int)(w + 10), (int)h,
               TRUE);
    SetWindowText(labelhWnd, label);
  }
}

void wxSlider::GetSize(int *width, int *height)
{
  RECT rect;
  rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

  wxFindMaxSize((HWND)ms_handle, &rect);

  if (labelhWnd)
  {
    wxFindMaxSize(labelhWnd, &rect);
  }
  if (static_min)
  {
    wxFindMaxSize(static_min, &rect);
  }
  if (static_max)
  {
    wxFindMaxSize(static_max, &rect);
  }
  if (edit_value)
  {
    wxFindMaxSize(edit_value, &rect);
  }

  *width = rect.right - rect.left;
  *height = rect.bottom - rect.top;
}

void wxSlider::GetPosition(int *x, int *y)
{
  wxWindow *parent = GetParent();
  RECT rect;
  rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

  wxFindMaxSize((HWND)ms_handle, &rect);

  if (labelhWnd)
  {
    wxFindMaxSize(labelhWnd, &rect);
  }
  if (static_min)
  {
    wxFindMaxSize(static_min, &rect);
  }
  if (static_max)
  {
    wxFindMaxSize(static_max, &rect);
  }
  if (edit_value)
  {
    wxFindMaxSize(edit_value, &rect);
  }

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

void wxSlider::SetSize(int x, int y, int width, int height, int sizeFlags)
{
#if WIN95 && USE_TRACK_BAR
  // If we're prepared to use the existing size, then...
  if (width == -1 && height == -1 && ((sizeFlags & wxSIZE_AUTO) != wxSIZE_AUTO))
  {
    GetSize(&width, &height);
  }

  int widthOrHeight = width;
  if (windowStyle & wxHORIZONTAL)
  {
    width = widthOrHeight;
    height = 25;
    if (width == -1)
      width = 100;
  }
  else
  {
    height = widthOrHeight;
    width = 25;
    if (height == -1)
      height = 100;
  }
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
#else
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  char buf[300];

  int x_offset = x;
  int y_offset = y;
  float current_width;

  int cx;     // slider,min,max sizes
  int cy;
  float cyf;
  int cxs;    // label sizes
  int cys;
  int dy,dys ;  // Adjustment values (vertical) if font sizes differs.

  wxGetCharSize((HWND)ms_handle, &cx, &cy,buttonFont);

  if ((windowStyle & wxVERTICAL) != wxVERTICAL)
  {
    // Horizontal
    if (labelhWnd)
    {
      wxGetCharSize((HWND)ms_handle, &cxs, &cys,labelFont);
      GetWindowText(labelhWnd, buf, 300);
      GetTextExtent(buf, &current_width, &cyf,NULL,NULL, labelFont);
      if (cys>cy)
      {
        dys = 0 ;
        dy = (cys-cy)/2 ; // center slider
      }
      else
      {
        dys = (cy-cys)/2; // center label
        dy = 0 ;
      }
      MoveWindow(labelhWnd, x_offset, y+dys, (int)(current_width + 2*cxs), (int)cyf,
                 TRUE);
      if (labelPosition==wxHORIZONTAL)
      {
        x_offset += (int)(current_width + 2*cxs + cxs);
        y_offset += dy ;
      }
      else
        y_offset += cys ;
    }

    float min_len;
    GetWindowText(static_min, buf, 300);
    GetTextExtent(buf, &min_len, &cyf,NULL,NULL, buttonFont);

    float max_len;
    GetWindowText(static_max, buf, 300);
    GetTextExtent(buf, &max_len, &cyf,NULL,NULL, buttonFont);

    if (edit_value)
    {
      int new_width = (int)(wxMax(min_len, max_len));
#ifdef WIN32
      // For some reason, under Win95, the text edit control has
      // a lot of space before the first character
      new_width += 3*cx;
#endif
      MoveWindow(edit_value, x_offset, y_offset, new_width, (int)cyf, TRUE);
      x_offset += new_width + cx;
    }

    MoveWindow(static_min, x_offset, y_offset, (int)min_len, cy, TRUE);
    x_offset += (int)(min_len + cx);

    int slider_length = (int)(width - x_offset - max_len - cx);

    // Slider must have a minimum/default length
    if (slider_length < 100)
      slider_length = 100;

    MoveWindow((HWND)ms_handle, x_offset, y_offset, slider_length, cy, TRUE);
    x_offset += slider_length + cx;

    MoveWindow(static_max, x_offset, y_offset, (int)max_len, cy, TRUE);
  }
  else
  {
    if (labelhWnd)
    {
      wxGetCharSize((HWND)ms_handle, &cxs, &cys,labelFont);
      GetWindowText(labelhWnd, buf, 300);
      GetTextExtent(buf, &current_width, &cyf,NULL,NULL, labelFont);
      MoveWindow(labelhWnd, x_offset, y, (int)(current_width + 2*cxs), (int)cyf, TRUE);
      y_offset += cys ;
    }

    float min_len;
    GetWindowText(static_min, buf, 300);
    GetTextExtent(buf, &min_len, &cyf,NULL,NULL, buttonFont);

    float max_len;
    GetWindowText(static_max, buf, 300);
    GetTextExtent(buf, &max_len, &cyf,NULL,NULL, buttonFont);

    if (edit_value)
    {
      int new_width = (int)(wxMax(min_len, max_len));
/*** Suggested change by George Tasker - remove this block...
#ifdef WIN32
      // For some reason, under Win95, the text edit control has
      // a lot of space before the first character
      new_width += 3*cx;
#endif
 ... and replace with following line: */
      new_width += cx;
 
      MoveWindow(edit_value, x_offset, y_offset, new_width, (int)cyf, TRUE);
      y_offset += (int)cyf;
    }

    MoveWindow(static_min, x_offset, y_offset, (int)min_len, cy, TRUE);
    y_offset += cy;

    int slider_length = (int)(height - y_offset - cy - cy);

    // Slider must have a minimum/default length
    if (slider_length < 100)
      slider_length = 100;

    // Use character height as an estimate of slider width (yes, width)
    MoveWindow((HWND)ms_handle, x_offset, y_offset, cy, slider_length, TRUE);
    y_offset += slider_length;

    MoveWindow(static_max, x_offset, y_offset, (int)max_len, cy, TRUE);
  }
#endif
  GetEventHandler()->OnSize(width, height);
}

void wxSlider::SetRange(int minValue, int maxValue)
{
  s_min = minValue;
  s_max = maxValue;

#if WIN95 && USE_TRACK_BAR
  ::SendMessage((HWND)ms_handle, TBM_SETRANGE, TRUE, MAKELONG(minValue, maxValue));
#else
  char buf[40];
  sprintf(buf, "%d", s_min);
  SetWindowText(static_min, buf);

  sprintf(buf, "%d", s_max);
  SetWindowText(static_max, buf);

  ::SetScrollRange((HWND)ms_handle, SB_CTL, s_min, s_max, TRUE);
#endif
}

int wxSlider::GetMin(void)
{
  return s_min;
}

int wxSlider::GetMax(void)
{
  return s_max;
}

void wxSlider::SetTicks(int n)
{
  noTicks = n;
}

int wxSlider::GetTicks(void)
{
  return noTicks;
}