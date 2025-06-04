/*
 * File:	wx_txt.cc
 * Purpose:	Single-line text item implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	April 1995
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
	Last change:  JS   26 May 97    3:47 pm
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_panel.h"
#include "wx_txt.h"
#include "wx_mtxt.h"
#endif

#if USE_CLIPBOARD
#include "wx_clipb.h"
#endif

#if WIN95 && !defined(GNUWIN32)
#include <richedit.h>
#endif

#include "wx_itemp.h"

BOOL wxText::MSWCommand(UINT param, WORD WXUNUSED(id))
{
/*
  // Debugging
  wxDebugMsg("Edit control %d: ", (int)id);
  switch (param)
  {
    case EN_SETFOCUS:
      wxDebugMsg("EN_SETFOCUS\n");
      break;
    case EN_KILLFOCUS:
      wxDebugMsg("EN_KILLFOCUS\n");
      break;
    case EN_CHANGE:
      wxDebugMsg("EN_CHANGE\n");
      break;
    case EN_UPDATE:
      wxDebugMsg("EN_UPDATE\n");
      break;
    case EN_ERRSPACE:
      wxDebugMsg("EN_ERRSPACE\n");
      break;
    case EN_MAXTEXT:
      wxDebugMsg("EN_MAXTEXT\n");
      break;
    case EN_HSCROLL:
      wxDebugMsg("EN_HSCROLL\n");
      break;
    case EN_VSCROLL:
      wxDebugMsg("EN_VSCROLL\n");
      break;
    default:
      wxDebugMsg("Unknown EDIT notification\n");
      break;
  }
*/
  WXTYPE eventTyp = 0;
  switch (param)
  {
    case EN_SETFOCUS:
      eventTyp = wxEVENT_TYPE_SET_FOCUS;
      break;
    case EN_KILLFOCUS:
      eventTyp = wxEVENT_TYPE_KILL_FOCUS;
      break;
    case EN_CHANGE:
      break;
    case EN_UPDATE:
      eventTyp = wxEVENT_TYPE_TEXT_COMMAND;
      break;
    case EN_ERRSPACE:
      break;
    case EN_MAXTEXT:
      break;
    case EN_HSCROLL:
       break;
    case EN_VSCROLL:
      break;
    default:
      break;
  }
  if (eventTyp != 0)
  {
    wxCommandEvent event(eventTyp);
    event.commandString = GetValue();
    event.eventObject = this;
    ProcessCommand(event);
    return TRUE;
  }
  else
    return FALSE;
}

// Text item
wxText::wxText(void)
{
  wxWinType = wxTYPE_HWND;
  labelhWnd = NULL;
  windows_id = 0;
  ms_handle = 0;
}

wxText::wxText(wxPanel *panel, wxFunction Function, Const char *label, Constdata char *value,
               int x, int y, int width, int height, long style, Constdata char *name):
  wxbText(panel, Function, label, value, x, y, width, height, style, name)
{
  Create(panel, Function, label, value, x, y, width, height, style, name);
}
  
Bool wxText::Create(wxPanel *panel, wxFunction Function, Const char *label, Constdata char *value,
               int x, int y, int width, int height, long style, Constdata char *name)
{
  SetName(name);
  if (panel) panel->AddChild(this);
  buttonFont = panel->buttonFont ;
  labelFont = panel->labelFont ;
  if (buttonFont)
    buttonFont->UseResource();
  if (labelFont)
    labelFont->UseResource();
  windowStyle = style;

  SetBackgroundColour(*panel->GetBackgroundColour()) ;
  labelColour = panel->labelColour ;
  buttonColour = panel->buttonColour ;
  wxWinType = wxTYPE_HWND;

  SetAppropriateLabelPosition();
  panel->GetValidPosition(&x, &y);

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

  windows_id = (int)NewControlId();

#ifdef WIN32
  HGLOBAL globalHandle = 0;
#else
  // Obscure method from the MS Developer's Network Disk for
  // using global memory instead of the local heap, which
  // runs out far too soon. Solves the problem with
  // failing to appear.
  globalHandle=0;
  if ((wxGetOsVersion() != wxWINDOWS_NT) && (wxGetOsVersion() != wxWIN95))
    globalHandle = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
                         256L);
#endif

  long msStyle = ES_AUTOHSCROLL | ES_LEFT | WS_BORDER |
                      WS_VISIBLE | WS_CHILD | WS_TABSTOP;

  if (windowStyle & wxREADONLY)
    msStyle |= ES_READONLY;

  if (windowStyle & wxPASSWORD) // hidden input
    msStyle |= ES_PASSWORD;

//    HWND edit = CreateWindowEx(0, "RichEdit", NULL,
  HWND edit = CreateWindowEx(0, "EDIT", NULL,
                        msStyle,
                        0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                        globalHandle ? globalHandle : wxhInstance, NULL);

#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(edit);
#endif
/* The following code subclasses the EDIT control (again! -- CTL3D may
 * already have done it) to intercept the ENTER key, which only
 * works if the style wxPROCESS_ENTER has been used.
 */
  ms_handle = (HANDLE)edit;

  SubclassControl(edit);

  if (buttonFont)
  {
    buttonFont->RealizeResource();
    if (buttonFont->GetResourceHandle())
      SendMessage((HWND)ms_handle,WM_SETFONT,
                (WPARAM)buttonFont->GetResourceHandle(), MAKELPARAM(0L, 0L));
  }

  if ((width < 0) && value && strlen(value))
  {
  char *the_value ;
    the_value = new char[strlen(value)+1] ;
    if (style&wxFIXED_LENGTH)
    {
      int i;
      for (i=0;i<(int)strlen(value);i++)
        the_value[i]=MEANING_CHARACTER ;
    }
    else
      strcpy(the_value,value) ;
    the_value[strlen(value)] = '\0' ;

    int cx,cy ;
    wxGetCharSize((HWND)ms_handle,&cx,&cy,buttonFont) ;
    float current_width,cyf ;
    GetTextExtent(the_value,&current_width,&cyf, NULL, NULL,buttonFont) ;
    width = (int)current_width + 2*cx ;
    if (label && strlen(label) && GetLabelPosition()==wxHORIZONTAL)
    {
      GetTextExtent(the_label,&current_width,&cyf, NULL, NULL,labelFont) ;
      width += (int)current_width + cx ;
    }
    delete [] the_value ;
  }

  SetSize(x, y, width, height);

  // Causes a crash for Symantec C++ and WIN32 for some reason
#if !(defined(__SC__) && defined(WIN32))
  if (value)
    SetWindowText(edit, value);
#endif

  panel->AdvanceCursor(this);
  Callback(Function);

  if (label)
  {
    if (style&wxFIXED_LENGTH)
      SetLabel((char *)label) ;
    if (the_label)
      delete [] the_label ;
  }

  return TRUE;
}

wxText::~wxText(void)
{
  isBeingDeleted = TRUE;
  
  if (labelhWnd)
    DestroyWindow(labelhWnd);
}

char *wxText::GetLabel(void)
{
  if (labelhWnd)
  {
    GetWindowText(labelhWnd, wxBuffer, 300);
    return wxBuffer;
  }
  else return NULL;
}

void wxText::SetLabel(char *label)
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

char *wxText::GetValue(void)
{
  GetWindowText((HWND)ms_handle, wxBuffer, 999);
  return wxBuffer;
}

void wxText::SetValue(char *value)
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

void wxText::GetSize(int *width, int *height)
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

void wxText::GetPosition(int *x, int *y)
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

void wxText::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  char buf[300];
  int cx; // button font dimensions
  int cy;
  int clx; // label font dimensions
  int cly;

  wxGetCharSize((HWND)ms_handle, &cx, &cy,buttonFont);

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
        height = (int)((wxMax(cly, cy))*EDIT_CONTROL_FACTOR) ;

      label_x = (float)x;
      label_y = (float)(y+(height - (int)label_height)/2); // Centre vertically
      label_width += (float)clx;

      control_x = label_x + label_width + cx;
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
    control_width = (float)width;
    control_height = (float)height;
  }

  // Calculations may have made text size too small
  if (control_height <= 0)
    control_height = (float)(int)(cy*EDIT_CONTROL_FACTOR) ;

  if (control_width <= 0)
    control_width = (float)DEFAULT_ITEM_WIDTH;

  MoveWindow((HWND)ms_handle, (int)control_x, (int)control_y,
                              (int)control_width, (int)control_height, TRUE);
  GetEventHandler()->OnSize(width, height);
}

void wxText::SetFocus(void)
{
  wxItem::SetFocus();
}

// Clipboard operations
void wxText::Copy(void)
{
  HWND hWnd = GetHWND();
  SendMessage(hWnd, WM_COPY, 0, 0L);
}

void wxText::Cut(void)
{
  HWND hWnd = GetHWND();
  SendMessage(hWnd, WM_CUT, 0, 0L);
}

void wxText::Paste(void)
{
  HWND hWnd = GetHWND();
  SendMessage(hWnd, WM_PASTE, 0, 0L);
}

void wxText::SetEditable(Bool editable)
{
  HWND hWnd = GetHWND();
  SendMessage(hWnd, EM_SETREADONLY, (WPARAM)!editable, (LPARAM)0L);
}

void wxText::SetInsertionPoint(long pos)
{
  HWND hWnd = GetHWND();
#if WIN95 && !defined(GNUWIN32)
  if (this->IsKindOf(CLASSINFO(wxMultiText)))
  {
    CHARRANGE range;
    range.cpMin = pos;
    range.cpMax = pos;
    SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM) &range);
    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
  }
  else
  {
    SendMessage(hWnd, EM_SETSEL, pos, pos);
    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
  }
#else
#ifdef WIN32
  SendMessage(hWnd, EM_SETSEL, pos, pos);
  SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
#else
  SendMessage(hWnd, EM_SETSEL, 0, MAKELPARAM(pos, pos));
#endif
#endif
  char *nothing = "";
  SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)nothing);
}

void wxText::SetInsertionPointEnd(void)
{
  long pos = GetLastPosition();
  SetInsertionPoint(pos);
}

long wxText::GetInsertionPoint(void)
{
#if WIN95 && !defined(GNUWIN32)
  if (this->IsKindOf(CLASSINFO(wxMultiText)))
  {
    CHARRANGE range;
    range.cpMin = 0;
    range.cpMax = 0;
    SendMessage(GetHWND(), EM_EXGETSEL, 0, (LPARAM) &range);
    return range.cpMin;
  }
  else
  {
    DWORD Pos=(DWORD)SendMessage(GetHWND(), EM_GETSEL, 0, 0L);
    return Pos&0xFFFF;
  }
#else
  DWORD Pos=(DWORD)SendMessage(GetHWND(), EM_GETSEL, 0, 0L);
  return Pos&0xFFFF;
#endif
}

long wxText::GetLastPosition(void)
{
    HWND hWnd = GetHWND();
    
    // Will always return a number > 0 (according to docs)
    int noLines = (int)SendMessage(hWnd, EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0L);

    // This gets the char index for the _beginning_ of the last line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)(noLines-1), (LPARAM)0L);
    
    // Get number of characters in the last line. We'll add this to the character
    // index for the last line, 1st position.
    int lineLength = (int)SendMessage(hWnd, EM_LINELENGTH, (WPARAM)charIndex, (LPARAM)0L);

    return (long)(charIndex + lineLength);
}

void wxText::Replace(long from, long to, char *value)
{
#if USE_CLIPBOARD
    HWND hWnd = GetHWND();
    long fromChar = from;
    long toChar = to;
    
    // Set selection and remove it
#ifdef WIN32
    SendMessage(hWnd, EM_SETSEL, fromChar, toChar);
#else
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
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

void wxText::Remove(long from, long to)
{
    HWND hWnd = GetHWND();
    long fromChar = from;
    long toChar = to;
    
    // Cut all selected text
#ifdef WIN32
    SendMessage(hWnd, EM_SETSEL, fromChar, toChar);
#else
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);
}

void wxText::SetSelection(long from, long to)
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
    SendMessage(hWnd, EM_SETSEL, (WPARAM)fromChar, (LPARAM)toChar);
    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
#else
    // WPARAM is 0: selection is scrolled into view
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
}

