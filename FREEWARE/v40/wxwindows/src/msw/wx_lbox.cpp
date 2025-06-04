/*
 * File:	wx_lbox.cc
 * Purpose:	List box implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	April 1995
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   25 May 97   11:22 am
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_panel.h"
#include "wx_lbox.h"
#endif

#include "wx_itemp.h"

BOOL wxListBox::MSWCommand(UINT param, WORD WXUNUSED(id))
{
  wxCommandEvent event(wxEVENT_TYPE_LISTBOX_COMMAND);
/*
  if (param == LBN_SELCANCEL)
  {
    event.extraLong = FALSE;
  }
*/
  if (param == LBN_SELCHANGE)
  {
    int *liste = NULL;
    int count = GetSelections(&liste) ;
    if (count && liste)
    {
      event.commandInt = liste[0] ;
      event.clientData = GetClientData(event.commandInt);
      char *s = GetString(event.commandInt);
      if (s)
        event.commandString = copystring(s);
    }
    else
    {
      event.commandInt = -1 ;
      event.commandString = copystring("") ;
    }

    event.eventObject = this;
    ProcessCommand(event);
    if (event.commandString)
      delete[] event.commandString ;
    return TRUE;
  }
  else if (param == LBN_DBLCLK)
  {
    wxPanel *parent = (wxPanel *)GetParent();
    if (parent)
      parent->GetEventHandler()->OnDefaultAction(this);
    return TRUE;
  }
  return FALSE;
}

// Listbox item
wxListBox::wxListBox(void)
{
  wxWinType = wxTYPE_HWND;
  labelhWnd = NULL;
  windows_id = 0;
  no_items = 0;
  ms_handle = 0;
}

wxListBox::wxListBox(wxPanel *panel, wxFunction func,
                       Const char *Title, Bool Multiple,
                       int x, int y, int width, int height,
                       int N, char **Choices, long style, Constdata char *name):
  wxbListBox(panel, func, Title, Multiple, x, y, width, height, N, Choices,
             style, name)
{
  Create(panel, func, Title, Multiple, x, y, width, height, N, Choices,
         style, name);
}

Bool wxListBox::Create(wxPanel *panel, wxFunction func,
                       Const char *Title, Bool Multiple,
                       int x, int y, int width, int height,
                       int N, char **Choices, long style, Constdata char *name)
{
  SetName(name);
  window_parent = panel;
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
  multiple = Multiple & wxMULTIPLE_MASK;
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

  // If label exists, create a static control for it.
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

  DWORD wstyle;
  if ((multiple == wxMULTIPLE) || (windowStyle & wxLB_MULTIPLE))
    wstyle = WS_VSCROLL | WS_BORDER | LBS_MULTIPLESEL | LBS_NOTIFY | WS_TABSTOP;
  else if ((multiple == wxEXTENDED) || (windowStyle & wxLB_EXTENDED))
    wstyle = WS_VSCROLL | WS_BORDER | LBS_EXTENDEDSEL | LBS_NOTIFY | WS_TABSTOP ;
  else
    wstyle = WS_VSCROLL | WS_BORDER | LBS_NOTIFY | WS_TABSTOP;
  if ((Multiple&wxALWAYS_SB) || (windowStyle & wxALWAYS_SB))
    wstyle |= LBS_DISABLENOSCROLL ;
  if (windowStyle & wxHSCROLL)
    wstyle |= WS_HSCROLL;
  if (windowStyle & wxLB_SORT)
    wstyle |= LBS_SORT;

  // Change from previous versions of wxWin: JACS Nov. 1995
  // Not sure whether to have integral, or no integral
  // style. With the latter we may get partial items showing.
  wstyle |= LBS_NOINTEGRALHEIGHT;

  windows_id = (int)NewControlId();

#if WIN95
  // Causes bizarre behaviour: the list items wrap,
  // giving 2 columns.
//  wstyle |= WS_EX_CLIENTEDGE;
#endif

  HWND wx_list = CreateWindowEx(0, "LISTBOX", NULL,
                         wstyle | WS_CHILD,
                         0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                         wxhInstance, NULL);
#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(wx_list);
#endif

  int i;
  for (i = 0; i < N; i++)
    SendMessage(wx_list, LB_ADDSTRING, 0, (LONG)Choices[i]);
  if (!Multiple)
    SendMessage(wx_list, LB_SETCURSEL, 0, 0);

  ShowWindow(wx_list, SW_SHOW);
  no_items = N;

  ms_handle = (HANDLE)wx_list;

  // Subclass again for purposes of dialog editing mode
  SubclassControl(wx_list);

  if (buttonFont)
  {
    buttonFont->RealizeResource();
    
    if (buttonFont->GetResourceHandle())
      SendMessage((HWND)ms_handle,WM_SETFONT,
                (WPARAM)buttonFont->GetResourceHandle(),0L);
  }

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

wxListBox::~wxListBox(void)
{
  isBeingDeleted = TRUE;
  
  if (selections)
    delete[] selections;
  if (labelhWnd)
    DestroyWindow(labelhWnd);
  labelhWnd = NULL;
}

void wxListBox::SetFirstItem(int N)
{
  SendMessage((HWND)ms_handle,LB_SETTOPINDEX,(WPARAM)N,(LPARAM)0) ;
}

void wxListBox::SetFirstItem(char *s)
{
  int N = FindString(s) ;

  if (N>=0)
    SetFirstItem(N) ;
}

void wxListBox::Delete(int N)
{
  SendMessage((HWND)ms_handle, LB_DELETESTRING, N, 0);
  no_items --;
  SetHorizontalExtent(NULL);
}

void wxListBox::Append(char *Item)
{
  SendMessage((HWND)ms_handle, LB_ADDSTRING, 0, (LONG)Item);
  no_items ++;
  SetHorizontalExtent(Item);
}

void wxListBox::Append(char *Item, char *Client_data)
{
  int index = (int)SendMessage((HWND)ms_handle, LB_ADDSTRING, 0, (LONG)Item);
  SendMessage((HWND)ms_handle, LB_SETITEMDATA, index, (LONG)Client_data);
  no_items ++;
  SetHorizontalExtent(Item);
}

void wxListBox::Set(int n, char *choices[], char *clientData[])
{
  ShowWindow((HWND)ms_handle, SW_HIDE);
  SendMessage((HWND)ms_handle, LB_RESETCONTENT, 0, 0);
  int i;
  for (i = 0; i < n; i++)
  {
    SendMessage((HWND)ms_handle, LB_ADDSTRING, 0, (LONG)choices[i]);
    if ( clientData )
      SendMessage((HWND)ms_handle, LB_SETITEMDATA, i, (LONG)clientData[i]);
  }
  no_items = n;
  SetHorizontalExtent(NULL);
  ShowWindow((HWND)ms_handle, SW_SHOW);
}

int wxListBox::FindString(char *s)
{
 int pos = (int)SendMessage((HWND)ms_handle, LB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)s);
 if (pos == LB_ERR)
   return -1;
 else
   return pos;
}

void wxListBox::Clear(void)
{
  SendMessage((HWND)ms_handle, LB_RESETCONTENT, 0, 0);

  no_items = 0;
  SendMessage((HWND)ms_handle, LB_SETHORIZONTALEXTENT, LOWORD(0), 0L);
}

void wxListBox::SetSelection(int N, Bool select)
{
  if ((multiple != wxSINGLE) || (windowStyle & wxLB_MULTIPLE) || (windowStyle & wxLB_EXTENDED))
    SendMessage((HWND)ms_handle, LB_SETSEL, select, N);
  else
  {
    if (!select) N = -N;
    SendMessage((HWND)ms_handle, LB_SETCURSEL, N, 0);
  }
}

Bool wxListBox::Selected(int N)
{
  return (Bool)SendMessage((HWND)ms_handle, LB_GETSEL, N, 0);
}

void wxListBox::Deselect(int N)
{
  if ((multiple != wxSINGLE) || (windowStyle & wxLB_MULTIPLE) || (windowStyle & wxLB_EXTENDED))
    SendMessage((HWND)ms_handle, LB_SETSEL, FALSE, N);
//  else
//    SendMessage((HWND)ms_handle, LB_SETCURSEL, -N, 0);
}

char *wxListBox::GetClientData(int N)
{
  return (char *)SendMessage((HWND)ms_handle, LB_GETITEMDATA, N, 0);
}

void wxListBox::SetClientData(int N, char *Client_data)
{
  (void)SendMessage((HWND)ms_handle, LB_SETITEMDATA, N, (LONG)Client_data);
/*
  if (result == LB_ERR)
      return -1;
  else
      return 0;
 */
}

// Return number of selections and an array of selected integers
// Use selections field to store data, which will be cleaned up
// by destructor if necessary.
int wxListBox::GetSelections(int **list_selections)
{
  HWND listbox = (HWND)ms_handle;
  if (selections)
    { delete[] selections; selections = NULL; };
  if ((multiple != wxSINGLE) || (windowStyle & wxLB_MULTIPLE) || (windowStyle & wxLB_EXTENDED))
  {
    int no_sel = (int)SendMessage(listbox, LB_GETSELCOUNT, 0, 0);
    if (no_sel == 0)
      return 0;
    selections = new int[no_sel];
    SendMessage(listbox, LB_GETSELITEMS, no_sel, (LONG)selections);
    *list_selections = selections;
    return no_sel;
  }
  else
  {
    int sel = (int)SendMessage(listbox, LB_GETCURSEL, 0, 0);
    if (sel == LB_ERR)
      return 0;
    selections = new int[1];
    selections[0] = sel;
    *list_selections = selections;
    return 1;
  }
}

// Get single selection, for single choice list items
int wxListBox::GetSelection(void)
{
  HWND listbox = (HWND)ms_handle;
  if (selections)
    { delete[] selections; selections = NULL; };
  if ((multiple == wxMULTIPLE) || (windowStyle & wxLB_MULTIPLE) || (windowStyle & wxLB_EXTENDED))
    return -1;
  else
  {
    int sel = (int)SendMessage(listbox, LB_GETCURSEL, 0, 0);
    if (sel == LB_ERR)
      return -1;
    else
    {
      return sel;
    }
  }
}

// Find string for position
char *wxListBox::GetString(int N)
{
  if (N < 0 || N > no_items)
    return NULL;
    
  int len = (int)SendMessage((HWND)ms_handle, LB_GETTEXT, N, (LONG)wxBuffer);
  wxBuffer[len] = 0;
  return wxBuffer;
}

void wxListBox::SetSize(int x, int y, int width, int height, int sizeFlags)
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

  wxGetCharSize((HWND)ms_handle, &cx, &cy,buttonFont);

  float label_width, label_height, label_x, label_y;
  float control_width, control_height, control_x, control_y;

  // Deal with default size (using -1 values)
  if (width<=0)
    width = DEFAULT_ITEM_WIDTH;

  if (height<=0)
    height = DEFAULT_ITEM_HEIGHT;

  if (labelhWnd)
  {
    // Find size of label
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
      control_y = label_y + label_height + 3; // Allow for 3D border
      control_width = (float)width;
      control_height = height - label_height - 3;
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

  // Calculations may have made size too small
  if (control_height <= 0)
    control_height = (float)DEFAULT_ITEM_HEIGHT;

  if (control_width <= 0)
    control_width = (float)DEFAULT_ITEM_WIDTH;

//  wxDebugMsg("About to set the listbox height to %d", (int)control_height);
  MoveWindow((HWND)ms_handle, (int)control_x, (int)control_y,
                              (int)control_width, (int)control_height, TRUE);

  GetEventHandler()->OnSize(width, height);
}

void wxListBox::GetSize(int *width, int *height)
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

void wxListBox::GetPosition(int *x, int *y)
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

char *wxListBox::GetLabel(void)
{
  if (labelhWnd)
  {
    GetWindowText(labelhWnd, wxBuffer, 300);
    return wxBuffer;
  }
  else return NULL;
}

void wxListBox::SetLabel(char *label)
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

    GetTextExtent((LPSTR)label, &w, &h, NULL, NULL,labelFont);
    MoveWindow(labelhWnd, point.x, point.y, (int)(w + 10), (int)h,
               TRUE);
    SetWindowText(labelhWnd, label);
  }
}

// Windows-specific code to set the horizontal extent of
// the listbox, if necessary. If s is non-NULL, it's
// used to calculate the horizontal extent.
// Otherwise, all strings are used.
void wxListBox::SetHorizontalExtent(char *s)
{
  // Only necessary if we want a horizontal scrollbar
  if (!(windowStyle & wxHSCROLL))
    return;
  TEXTMETRIC lpTextMetric;

  HWND hWnd = GetHWND();

  if (s)
  {
    int existingExtent = (int)SendMessage(hWnd, LB_GETHORIZONTALEXTENT, 0, 0L);
    HDC dc = GetWindowDC(hWnd);
    HFONT oldFont = 0;
    if (buttonFont && buttonFont->GetResourceHandle())
      oldFont = ::SelectObject(dc, buttonFont->GetResourceHandle());

    GetTextMetrics(dc, &lpTextMetric);
    SIZE extentXY;
    ::GetTextExtentPoint(dc, (LPSTR)s, strlen(s), &extentXY);
    int extentX = (int)(extentXY.cx + lpTextMetric.tmAveCharWidth);
    
    if (oldFont)
      ::SelectObject(dc, oldFont);

    ReleaseDC(hWnd, dc);
    if (extentX > existingExtent)
      SendMessage(hWnd, LB_SETHORIZONTALEXTENT, LOWORD(extentX), 0L);
    return;
  }
  else
  {
    int largestExtent = 0;
    HDC dc = GetWindowDC(hWnd);
    HFONT oldFont = 0;
    if (buttonFont && buttonFont->GetResourceHandle())
      oldFont = ::SelectObject(dc, buttonFont->GetResourceHandle());

    GetTextMetrics(dc, &lpTextMetric);
    int i;
    for (i = 0; i < no_items; i++)
    {
      int len = (int)SendMessage(hWnd, LB_GETTEXT, i, (LONG)wxBuffer);
      wxBuffer[len] = 0;
      SIZE extentXY;
      ::GetTextExtentPoint(dc, (LPSTR)wxBuffer, len, &extentXY);
      int extentX = (int)(extentXY.cx + lpTextMetric.tmAveCharWidth);
      if (extentX > largestExtent)
        largestExtent = extentX;
    }
    if (oldFont)
      ::SelectObject(dc, oldFont);

    ReleaseDC(hWnd, dc);
    SendMessage(hWnd, LB_SETHORIZONTALEXTENT, LOWORD(largestExtent), 0L);
  }
}

void
wxListBox::InsertItems(int nItems, char **Items, int pos)
{
  int i;
  for (i = 0; i < nItems; i++)
    SendMessage((HWND)ms_handle, LB_INSERTSTRING, i + pos, (LPARAM)Items[i]);
  no_items += nItems;
  SetHorizontalExtent(NULL);
}

void wxListBox::SetString(int N, char *s)
{
  int sel = GetSelection();
  
  char *oldData = (char *)wxListBox::GetClientData(N);
  
  SendMessage((HWND)ms_handle, LB_DELETESTRING, N, 0);

  int newN = N;
  if (N == (no_items - 1))
    newN = -1;
    
  SendMessage((HWND)ms_handle, LB_INSERTSTRING, newN, (LPARAM)s);
  if (oldData)
    wxListBox::SetClientData(N, oldData);

  // Selection may have changed
  if (sel >= 0)
    SetSelection(sel);
}

