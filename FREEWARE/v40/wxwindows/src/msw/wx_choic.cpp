/*
 * File:	wx_choic.cc
 * Purpose:	Choice item implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	April 1995
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_panel.h"
#include "wx_choic.h"
#endif

#include "wx_itemp.h"

BOOL wxChoice::MSWCommand(UINT param, WORD WXUNUSED(id))
{
  if (param == CBN_SELCHANGE)
  {
    wxCommandEvent event(wxEVENT_TYPE_CHOICE_COMMAND);
    event.commandInt = GetSelection();
    event.eventObject = this;
    event.commandString = copystring(GetStringSelection());
    ProcessCommand(event);
    delete[] event.commandString;
    return TRUE;
  }
  else return FALSE;
}

wxChoice::wxChoice(void)
{
  no_strings = 0;
  wxWinType = wxTYPE_HWND;
  labelhWnd = NULL;
  windows_id = 0;
  ms_handle = 0;
}

wxChoice::wxChoice(wxPanel *panel, wxFunction func, Const char *Title,
                   int x, int y, int width, int height, int N, char **Choices,
                   long style, Constdata char *name):
  wxbChoice(panel, func, Title, x, y, width, height, N, Choices, style, name)
{
  Create(panel, func, Title, x, y, width, height, N, Choices, style, name);
}

Bool wxChoice::Create(wxPanel *panel, wxFunction func, Const char *Title,
                   int x, int y, int width, int height, int N, char **Choices,
                   long style, Constdata char *name)
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
  no_strings = N;

  wxWinType = wxTYPE_HWND;
  windowStyle = style;

  SetAppropriateLabelPosition();
  panel->GetValidPosition(&x, &y);

  char *the_label = NULL ;

  if (Title)
  {
    the_label = new char[strlen(Title)+1] ;
    if (style&wxFIXED_LENGTH)
    {
      int i;
      for (i=0;i<(int)strlen(Title);i++)
        the_label[i]=MEANING_CHARACTER ;
    }
    else
      strcpy(the_label,Title) ;
    the_label[strlen(Title)] = '\0' ;
  }

  if (Title)
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

  windows_id = (int)NewControlId();
  
  long msStyle = WS_CHILD | CBS_DROPDOWNLIST | WS_HSCROLL | WS_VSCROLL
                   | WS_BORDER | WS_TABSTOP | WS_VISIBLE;
  if (windowStyle & wxCB_SORT)
    msStyle |= CBS_SORT;
#if WIN95
//  msStyle |= WS_EX_CLIENTEDGE;
#endif

  HWND wx_combo = CreateWindowEx(0, "COMBOBOX", NULL,
                   msStyle,
                   0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                   wxhInstance, NULL);
#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(wx_combo);
#endif

  ms_handle = (HANDLE)wx_combo;

  // Subclass again for purposes of dialog editing mode
  SubclassControl(wx_combo);

  if (buttonFont)
  {
    buttonFont->RealizeResource();
    if (buttonFont->GetResourceHandle())
      SendMessage((HWND)ms_handle,WM_SETFONT,
                  (WPARAM)panel->buttonFont->GetResourceHandle(),0L);
  }

  int i;
  for (i = 0; i < N; i++)
    SendMessage(wx_combo, CB_INSERTSTRING, i, (LONG)Choices[i]);
  SendMessage(wx_combo, CB_SETCURSEL, i, 0);

  SetSize(x, y, width, height);

  panel->AdvanceCursor(this);
  Callback(func);

  if (Title)
  {
    if (style&wxFIXED_LENGTH)
      SetLabel((char *)Title) ;
    if (the_label)
      delete [] the_label ;
  }

  return TRUE;
}

wxChoice::~wxChoice(void)
{
  isBeingDeleted = TRUE;
  
  if (labelhWnd)
    DestroyWindow(labelhWnd);
  labelhWnd = NULL;
}

void wxChoice::Append(char *Item)
{
  SendMessage((HWND)ms_handle, CB_ADDSTRING, 0, (LONG)Item);

  no_strings ++;
}

// Unfortunately, under XView it doesn't redisplay until user resizes
// window. Any suggestions folks?
void wxChoice::Clear(void)
{
  SendMessage((HWND)ms_handle, CB_RESETCONTENT, 0, 0);

  no_strings = 0;
}


int wxChoice::GetSelection(void)
{
  return (int)SendMessage((HWND)ms_handle, CB_GETCURSEL, 0, 0);
}

void wxChoice::SetSelection(int n)
{
  SendMessage((HWND)ms_handle, CB_SETCURSEL, n, 0);
}

int wxChoice::FindString(char *s)
{
#if defined(__WATCOMC__) && defined(__WIN386__)
  // For some reason, Watcom in WIN386 mode crashes in the CB_FINDSTRINGEXACT message.
  // Do it the long way instead.
  char buf[512];
  for (int i = 0; i < Number(); i++)
  {
    int len = (int)SendMessage((HWND)ms_handle, CB_GETLBTEXT, i, (LPARAM)(LPSTR)buf);
    buf[len] = 0;
    if (strcmp(buf, s) == 0)
      return i;
  }
  return -1;
#else
 int pos = (int)SendMessage((HWND)ms_handle, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)(LPSTR)s);
 if (pos == LB_ERR)
   return -1;
 else
   return pos;
#endif
}

char *wxChoice::GetString(int n)
{
  int len = (int)SendMessage((HWND)ms_handle, CB_GETLBTEXT, n, (long)wxBuffer);
  wxBuffer[len] = 0;
  return wxBuffer;
}

void wxChoice::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  // If we're prepared to use the existing size, then...
  if (width == -1 && height == -1 && ((sizeFlags & wxSIZE_AUTO) != wxSIZE_AUTO))
  {
    GetSize(&width, &height);
  }

  char buf[300];

  int cx; // button font dimensions
  int cy;
  int clx; // label font dimensions
  int cly;
  wxGetCharSize((HWND)ms_handle, &cx, &cy, buttonFont);

  float label_width, label_height, label_x, label_y;
  float control_width, control_height, control_x, control_y;

  // Ignore height parameter because height doesn't
  // mean 'initially displayed' height, it refers to the
  // drop-down menu as well. The wxWindows interpretation
  // is different; also, getting the size returns the
  // _displayed_ size (NOT the drop down menu size)
  // so setting-getting-setting size would not work.
  height = -1;

  // Deal with default size (using -1 values)
  if (width <= 0)
  {
    // Find the longest string
    if (no_strings == 0)
      control_width = (float)100.0;
    else
    {
      float len, ht;
      float longest = (float)0.0;
      int i;
      for (i = 0; i < no_strings; i++)
      {
        char *s = GetString(i);
        GetTextExtent(s, &len, &ht, NULL, NULL,buttonFont);
        if ( len > longest) longest = len;
      }

      control_width = (float)(int)(longest + cx*5);
    }
  }

  // Choice drop-down list depends on number of items (limited to 10)
  if (height <= 0)
  {
    if (no_strings == 0)
      height = (int)(EDIT_CONTROL_FACTOR*cy*10.0);
    else height = (int)(EDIT_CONTROL_FACTOR*cy*(wxMin(10, no_strings) + 1));
  }

  if (labelhWnd)
  {
    // Find size of label
    wxGetCharSize((HWND)ms_handle, &clx, &cly,labelFont);
    GetWindowText(labelhWnd, buf, 300);
    GetTextExtent(buf, &label_width, &label_height, NULL, NULL,labelFont);

    // Given size is total label + edit size, so find individual
    // control sizes on that basis.
    if (GetLabelPosition() == wxHORIZONTAL)
    {
      if (height<=0)
        height = (int)((wxMax(cy,cly))*EDIT_CONTROL_FACTOR) ;

      label_x = (float)x;
      label_y = (float)y; // + (height - (int)label_height)/2; // Centre vertically
      label_width += (float)clx;

      control_x = label_x + label_width + clx;
      control_y = (float)y;
      if (width >= 0)
        control_width = width - (control_x - label_x);
      control_height = (float)height;
    }
    else // wxVERTICAL
    {
      label_x = (float)x;
      label_y = (float)y;

      control_x = (float)x;
      control_y = label_y + label_height + 3; // Allow for 3D border

      if (width >= 0)
        control_width = (float)width;

      if (height<=0)
        control_height = (float)(int)(cy*EDIT_CONTROL_FACTOR) ;
      else
        control_height = height - label_height - 3;
    }
    MoveWindow(labelhWnd, (int)label_x, (int)label_y,
               (int)label_width, (int)label_height, TRUE);
  }
  else
  {
    control_x = (float)x;
    control_y = (float)y;
    if (width >= 0)
      control_width = (float)width;
    control_height = (float)height;
  }

  // Calculations may have made text size too small
  if (control_height <= 0)
    control_height = (float)(int)(cy*EDIT_CONTROL_FACTOR) ;

  if (control_width <= 0)
    control_width = (float)100.0;

  MoveWindow((HWND)ms_handle, (int)control_x, (int)control_y,
                              (int)control_width, (int)control_height, TRUE);

  GetEventHandler()->OnSize(width, height);
}

void wxChoice::GetSize(int *width, int *height)
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

void wxChoice::GetPosition(int *x, int *y)
{
  HWND wnd = (HWND)ms_handle;
  wxWindow *parent = GetParent();
  RECT rect;
  rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

  wxFindMaxSize(wnd, &rect);
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

char *wxChoice::GetLabel(void)
{
  if (labelhWnd)
  {
    GetWindowText(labelhWnd, wxBuffer, 300);
    return wxBuffer;
  }
  else return NULL;
}

void wxChoice::SetLabel(char *label)
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
