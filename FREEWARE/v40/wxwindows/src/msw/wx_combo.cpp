/*
 * File:	wx_combo.cc
 * Purpose:	Combobox item implementation
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
#endif

#if USE_COMBOBOX

#include "wx_combo.h"
#include "wx_clipb.h"
#include "wx_itemp.h"

BOOL wxComboBox::MSWCommand(UINT param, WORD WXUNUSED(id))
{
  if (param == CBN_SELCHANGE)
  {
    wxCommandEvent event(wxEVENT_TYPE_COMBOBOX_COMMAND);
    event.commandInt = GetSelection();
    event.eventObject = this;
    event.commandString = copystring(GetStringSelection());
    ProcessCommand(event);
    delete[] event.commandString;
    return TRUE;
  }
  else return FALSE;
}

wxComboBox::wxComboBox(void)
{
  no_strings = 0;
  wxWinType = wxTYPE_HWND;
  labelhWnd = NULL;
  windows_id = 0;
  ms_handle = 0;
}

wxComboBox::wxComboBox(wxPanel *panel, wxFunction func, Const char *Title, Constdata char *value,
                   int x, int y, int width, int height, int N, char **Choices,
                   long style, Constdata char *name):
  wxbComboBox(panel, func, Title, value, x, y, width, height, N, Choices, style, name)
{
  Create(panel, func, Title, value, x, y, width, height, N, Choices, style, name);
}

Bool wxComboBox::Create(wxPanel *panel, wxFunction func, Const char *Title, Constdata char *value,
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
  
  long msStyle = WS_CHILD | WS_HSCROLL | WS_VSCROLL
                     | WS_BORDER | WS_TABSTOP | WS_VISIBLE | CBS_NOINTEGRALHEIGHT;
  if (windowStyle & wxCB_READONLY)
    msStyle |= CBS_DROPDOWNLIST;
  else if (windowStyle & wxCB_SIMPLE) // A list (shown always) and edit control
    msStyle |= CBS_SIMPLE;
  else
    msStyle |= CBS_DROPDOWN;

  if (windowStyle & wxCB_SORT)
    msStyle |= CBS_SORT;

#if WIN95
//    msStyle |= WS_EX_CLIENTEDGE;
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
  if (value)
    SetWindowText(wx_combo, value);

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

wxComboBox::~wxComboBox(void)
{
  isBeingDeleted = TRUE;
  
  if (labelhWnd)
    DestroyWindow(labelhWnd);
  labelhWnd = NULL;
}

void wxComboBox::Append(char *Item)
{
  SendMessage((HWND)ms_handle, CB_ADDSTRING, 0, (LONG)Item);

  no_strings ++;
}

void wxComboBox::Delete(int n)
{
  no_strings = (int)SendMessage((HWND)ms_handle, CB_DELETESTRING, n, 0);
}

void wxComboBox::Clear(void)
{
  SendMessage((HWND)ms_handle, CB_RESETCONTENT, 0, 0);
  no_strings = 0;
}

int wxComboBox::GetSelection(void)
{
  return (int)SendMessage((HWND)ms_handle, CB_GETCURSEL, 0, 0);
}

void wxComboBox::SetSelection(int n)
{
  SendMessage((HWND)ms_handle, CB_SETCURSEL, n, 0);
}

int wxComboBox::FindString(char *s)
{
#if defined(__WATCOMC__) && defined(__WIN386)
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

char *wxComboBox::GetString(int n)
{
  int len = (int)SendMessage((HWND)ms_handle, CB_GETLBTEXT, n, (long)wxBuffer);
  wxBuffer[len] = 0;
  return wxBuffer;
}

void wxComboBox::SetSize(int x, int y, int width, int height, int sizeFlags)
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
      control_width = 100;
    else
    {
      float len, ht;
      float longest = 0.0;
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
    control_width = 100;

  MoveWindow((HWND)ms_handle, (int)control_x, (int)control_y,
                              (int)control_width, (int)control_height, TRUE);

  GetEventHandler()->OnSize(width, height);
}

void wxComboBox::GetSize(int *width, int *height)
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

void wxComboBox::GetPosition(int *x, int *y)
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

char *wxComboBox::GetLabel(void)
{
  if (labelhWnd)
  {
    GetWindowText(labelhWnd, wxBuffer, 300);
    return wxBuffer;
  }
  else return NULL;
}

void wxComboBox::SetLabel(char *label)
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

char *wxComboBox::GetValue(void)
{
  GetWindowText((HWND)ms_handle, wxBuffer, 500);
  return wxBuffer;
}

void wxComboBox::SetValue(char *value)
{
  if (!value)
    value = "";
    
  // If newlines are denoted by just 10, must stick 13 in front.
  int singletons = 0;
  int len = strlen(value);
  int i;
  for (i = 0; i < len; i ++)
  {
    if ((i > 0) && (value[i] == 10) && (value[i-1] != 13))
      singletons ++;
  }
  if (singletons > 0)
  {
    char *tmp = new char[len + singletons + 1];
    int j = 0;
    for (i = 0; i < len; i ++)
    {
      if ((i > 0) && (value[i] == 10) && (value[i-1] != 13))
      {
        tmp[j] = 13;
        j ++;
      }
      tmp[j] = value[i];
      j ++;
    }
    tmp[j] = 0;
    SetWindowText((HWND)ms_handle, tmp);
    delete[] tmp;
  }
  else
    SetWindowText((HWND)ms_handle, value);
}

// Clipboard operations
void wxComboBox::Copy(void)
{
  HWND hWnd = GetHWND();
  SendMessage(hWnd, WM_COPY, 0, 0L);
}

void wxComboBox::Cut(void)
{
  HWND hWnd = GetHWND();
  SendMessage(hWnd, WM_CUT, 0, 0L);
}

void wxComboBox::Paste(void)
{
  HWND hWnd = GetHWND();
  SendMessage(hWnd, WM_PASTE, 0, 0L);
}

void wxComboBox::SetEditable(Bool editable)
{
  // Can't implement in MSW?
//  HWND hWnd = GetHWND();
//  SendMessage(hWnd, EM_SETREADONLY, (WPARAM)!editable, (LPARAM)0L);
}

void wxComboBox::SetInsertionPoint(long pos)
{
/*
  HWND hWnd = GetHWND();
#ifdef WIN32
  SendMessage(hWnd, EM_SETSEL, pos, pos);
  SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
#else
  SendMessage(hWnd, EM_SETSEL, 0, MAKELPARAM(pos, pos));
#endif
  char *nothing = "";
  SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)nothing);
*/
}

void wxComboBox::SetInsertionPointEnd(void)
{
/*
  long pos = GetLastPosition();
  SetInsertionPoint(pos);
*/
}

long wxComboBox::GetInsertionPoint(void)
{
/*
  DWORD Pos=(DWORD)SendMessage(GetHWND(), EM_GETSEL, 0, 0L);
  return Pos&0xFFFF;
*/
  return 0;
}

long wxComboBox::GetLastPosition(void)
{
/*
    HWND hWnd = GetHWND();
    
    // Will always return a number > 0 (according to docs)
    int noLines = (int)SendMessage(hWnd, EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0L);

    // This gets the char index for the _beginning_ of the last line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)(noLines-1), (LPARAM)0L);
    
    // Get number of characters in the last line. We'll add this to the character
    // index for the last line, 1st position.
    int lineLength = (int)SendMessage(hWnd, EM_LINELENGTH, (WPARAM)charIndex, (LPARAM)0L);

    return (long)(charIndex + lineLength);
*/
  return 0;
}

void wxComboBox::Replace(long from, long to, char *value)
{
#if USE_CLIPBOARD
    HWND hWnd = GetHWND();
    long fromChar = from;
    long toChar = to;
    
    // Set selection and remove it
#ifdef WIN32
    SendMessage(hWnd, CB_SETEDITSEL, fromChar, toChar);
#else
    SendMessage(hWnd, CB_SETEDITSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);

    if (value)
    {
      // Now replace with 'value', by pasting.
      wxSetClipboardData(wxCF_TEXT, (wxObject *)value, 0, 0);

      // Paste into edit control
      SendMessage(hWnd, WM_PASTE, (WPARAM)0, (LPARAM)0L);
    }
#endif
}

void wxComboBox::Remove(long from, long to)
{
    HWND hWnd = GetHWND();
    long fromChar = from;
    long toChar = to;
    
    // Cut all selected text
#ifdef WIN32
    SendMessage(hWnd, CB_SETEDITSEL, fromChar, toChar);
#else
    SendMessage(hWnd, CB_SETEDITSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);
}

void wxComboBox::SetSelection(long from, long to)
{
    HWND hWnd = GetHWND();
    long fromChar = from;
    long toChar = to;
    // if from and to are both -1, it means
    // (in wxWindows) that all text should be selected.
    // This translates into Windows convention
    if ((from == -1) && (to == -1))
    {
      fromChar = 0;
      toChar = -1;
    }
    
#ifdef WIN32
    SendMessage(hWnd, CB_SETEDITSEL, (WPARAM)fromChar, (LPARAM)toChar);
//    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
#else
    // WPARAM is 0: selection is scrolled into view
    SendMessage(hWnd, CB_SETEDITSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
}

void wxComboBox::SetFocus(void)
{
  wxItem::SetFocus();
}

#endif
 // USE_COMBOBOX

