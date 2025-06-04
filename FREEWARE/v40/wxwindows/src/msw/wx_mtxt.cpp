/*
 * File:	wx_mtxt.cc
 * Purpose:	Multi-line text item implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	April 1995
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
	Last change:  JS   26 May 97    3:41 pm
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_panel.h"
#include "wx_mtxt.h"
#endif

#include "wx_itemp.h"
#include "wx_clipb.h"

// Multi-line Text item
wxMultiText::wxMultiText(void)
{
}

wxMultiText::wxMultiText(wxPanel *panel, wxFunction Function, Const char *label, Constdata char *value,
               int x, int y, int width, int height, long style, Constdata char *name):
  wxbMultiText(panel, Function, label, value, x, y, width, height, style, name)
{
  Create(panel, Function, label, value, x, y, width, height, style, name);
}

Bool wxMultiText::Create(wxPanel *panel, wxFunction Function, Const char *label, Constdata char *value,
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
  if (height == -1) height = 50;

  window_parent = panel;
  SetAppropriateLabelPosition();
  
  wxWinType = wxTYPE_HWND;

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

#if defined(WIN32) || WIN95
  HGLOBAL globalHandle=0;
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

  long msStyle = ES_MULTILINE | ES_LEFT | ES_WANTRETURN |
               WS_BORDER | WS_VISIBLE | WS_CHILD | WS_TABSTOP |
               WS_VSCROLL;

  if (windowStyle & wxREADONLY)
    msStyle |= ES_READONLY;

  if (windowStyle & wxHSCROLL)
    msStyle |= (WS_HSCROLL | ES_AUTOHSCROLL) ;

  if (windowStyle & wxPASSWORD) // hidden input
    msStyle |= ES_PASSWORD;

#if WIN95 && !defined(GNUWIN32)
    HWND edit = CreateWindowEx(0, "RichEdit", label,
#else
  HWND edit = CreateWindowEx(0, "EDIT", label,
#endif
               msStyle,
               0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
               globalHandle ? globalHandle : wxhInstance, NULL);
#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(edit);
#endif

  ms_handle = (HANDLE)edit;

  // Subclass again for purposes of dialog editing mode
  SubclassControl(edit);

  if (buttonFont)
  {
    buttonFont->RealizeResource();
    if (buttonFont->GetResourceHandle())
      SendMessage((HWND)ms_handle,WM_SETFONT,
                (WPARAM)buttonFont->GetResourceHandle(),0L);
  }

  SetSize(x, y, width, height);

  if (value)
    SetWindowText(edit, value);

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

char *wxMultiText::GetValue(void)
{
  static char *multiTextBuffer = NULL;
  
  int TL = GetWindowTextLength((HWND)ms_handle);

  if (multiTextBuffer)
    delete[] multiTextBuffer;
  multiTextBuffer = new char[TL+2];
/*
  if (TL > 999)
    TL = 999;
*/
  // Calling SendMessage this way is equivalent with a GetWindowText call.
  // Note that (some of) the parameters have to be casted appropriately...
  SendMessage((HWND)ms_handle, WM_GETTEXT, TL+1, (LPARAM)((LPSTR) multiTextBuffer));
  
  multiTextBuffer[TL] = 0;
  
  return multiTextBuffer;

/*
  if(TL == 999)
    wxBuffer[999] = '\0';

  return wxBuffer;
*/

/* Not sure if this is still needed.

// Julian! I can't see why this job below should have to be done -
// the edit control has the es_wantreturn attribute byte set,
// and will thus have the cr/lf characters inserted appropriately... /Roffe

  int buf_len = 1000;
  int no_chars = 0;
  int no_lines = (int)SendMessage((HWND)ms_handle, EM_GETLINECOUNT, 0, 0L);
  int i;
  for (i = 0; i < no_lines; i++)
  {
    *(LPINT)(wxBuffer+no_chars) = buf_len;
    int n = (int)SendMessage((HWND)ms_handle, EM_GETLINE, i,
                         (LONG)(wxBuffer+no_chars));
    no_chars += n;
    buf_len -= (n + 2);
    if (i < (no_lines - 1))
    {
      wxBuffer[no_chars] = 13;
      no_chars ++;
      wxBuffer[no_chars] = 10;
      no_chars ++;
    }
  }
  wxBuffer[no_chars] = 0;
  return wxBuffer;
*/
}

void wxMultiText::GetValue(char *buffer, int maxSize)
{
  // Fix by /Roffe for Borland 4.0.
  int TL = GetWindowTextLength((HWND)ms_handle);

  // We still want to keep within the limits for an edit control, won't we?
  if (maxSize > 32766)
    maxSize = 32766;

  if (TL > maxSize)
    TL = maxSize;
    
  // Here we use GetWindowText instead.
  GetWindowText((HWND)ms_handle, (LPSTR) buffer, TL);

  if(TL == maxSize)
    buffer[maxSize] = '\0';

/*
  buffer[0] = 0;
  int no_chars = 0;
  int no_lines = (int)SendMessage((HWND)ms_handle, EM_GETLINECOUNT, 0, 0L);
  int i;
  for (i = 0; i < no_lines; i++)
  {
    *(LPINT)(buffer+no_chars) = maxSize;
    int n = (int)SendMessage((HWND)ms_handle, EM_GETLINE, i,
                         (LONG)(buffer+no_chars));
    no_chars += n;
    maxSize -= (n + 2);
    if (i < (no_lines - 1))
    {
      buffer[no_chars] = 13;
      no_chars ++;
      buffer[no_chars] = 10;
      no_chars ++;
    }
  }
  buffer[no_chars] = 0;
*/
}

void wxMultiText::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  char buf[300];

  int cx;
  int cy;
  int clx;
  int cly;

  // If we're prepared to use the existing size, then...
  if (width == -1 && height == -1 && ((sizeFlags & wxSIZE_AUTO) != wxSIZE_AUTO))
  {
    GetSize(&width, &height);
  }

  if (height <= 0)
    height = DEFAULT_ITEM_HEIGHT;

  wxGetCharSize((HWND)ms_handle, &cx, &cy,buttonFont);

  float label_width, label_height, label_x, label_y;
  float control_width, control_height, control_x, control_y;

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
      label_x = (float)x;
      label_y = (float)y;
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
    control_height = (float)DEFAULT_ITEM_HEIGHT;

  if (control_width <= 0)
    control_width = (float)DEFAULT_ITEM_WIDTH;

//  wxDebugMsg("About to set the multitext height to %d", (int)control_height);

  MoveWindow((HWND)ms_handle, (int)control_x, (int)control_y,
                              (int)control_width, (int)control_height, TRUE);
  GetEventHandler()->OnSize(width, height);
}

/*
 * ADDED BY JACS Nov 1995
 */
 
/*
 * Some of the following functions are yet to be implemented
 *
 */
 
int wxMultiText::GetNumberOfLines(void)
{
  return (int)SendMessage(GetHWND(), EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
}

long wxMultiText::XYToPosition(long x, long y)
{
    HWND hWnd = GetHWND();
    
    // This gets the char index for the _beginning_ of this line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)y, (LPARAM)0);
    return (long)(x + charIndex);
}

void wxMultiText::PositionToXY(long pos, long *x, long *y)
{
    HWND hWnd = GetHWND();

    // This gets the line number containing the character
    int lineNo = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)pos, (LPARAM)0);
    // This gets the char index for the _beginning_ of this line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)lineNo, (LPARAM)0);
    // The X position must therefore be the different between pos and charIndex
    *x = (long)(pos - charIndex);
    *y = (long)lineNo;
}

void wxMultiText::ShowPosition(long pos)
{
    HWND hWnd = GetHWND();
    
    // To scroll to a position, we pass the number of lines and characters
    // to scroll *by*. This means that we need to:
    // (1) Find the line position of the current line.
    // (2) Find the line position of pos.
    // (3) Scroll by (pos - current).
    // For now, ignore the horizontal scrolling.

    // Is this where scrolling is relative to - the line containing the caret?
    // Or is the first visible line??? Try first visible line.
    int currentLineLineNo1 = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)-1, (LPARAM)0L);

    int currentLineLineNo = (int)SendMessage(hWnd, EM_GETFIRSTVISIBLELINE, (WPARAM)0, (LPARAM)0L);

    int specifiedLineLineNo = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)pos, (LPARAM)0L);
    
    int linesToScroll = specifiedLineLineNo - currentLineLineNo;

/*
    wxDebugMsg("Caret line: %d; Current visible line: %d; Specified line: %d; lines to scroll: %d\n",
      currentLineLineNo1, currentLineLineNo, specifiedLineLineNo, linesToScroll);
*/

    if (linesToScroll != 0)
      (void)SendMessage(hWnd, EM_LINESCROLL, (WPARAM)0, (LPARAM)MAKELPARAM(linesToScroll, 0));
}

int wxMultiText::GetLineLength(long lineNo)
{
    long charIndex = XYToPosition(0, lineNo);
    HWND hWnd = GetHWND();
    int len = (int)SendMessage(hWnd, EM_LINELENGTH, (WPARAM)charIndex, (LPARAM)0);
    return len;
}

int wxMultiText::GetLineText(long lineNo, char *buf)
{
    HWND hWnd = GetHWND();
    *(WORD *)buf = 128;
    int noChars = (int)SendMessage(hWnd, EM_GETLINE, (WPARAM)lineNo, (LPARAM)buf);
    buf[noChars] = 0;
    return noChars;
}

// Copy selection to clipboard
void wxMultiText::Copy(void)
{
    HWND hWnd = GetHWND();
    SendMessage(hWnd, WM_COPY, 0, 0L);
}

// Paste clipboard into text window
void wxMultiText::Paste(void)
{
    HWND hWnd = GetHWND();
    SendMessage(hWnd, WM_PASTE, 0, 0L);
}

// Copy selection to clipboard, then remove selection.
void wxMultiText::Cut(void)
{
    HWND hWnd = GetHWND();
    SendMessage(hWnd, WM_CUT, 0, 0L);
}

void wxMultiText::WriteText(char *text)
{
    // Covert \n to \r\n
    int len = strlen(text);
    char *newtext = new char[(len*2)+1];
    int i = 0;
    int j = 0;
    while (i < len)
    {
      if (text[i] == '\n')
      {
        newtext[j] = '\r';
        j ++;
      }
      newtext[j] = text[i];
      i ++;
      j ++;
    }
    newtext[j] = 0;
    SendMessage(GetHWND(), EM_REPLACESEL, 0, (LPARAM)newtext);
    delete[] newtext;
}