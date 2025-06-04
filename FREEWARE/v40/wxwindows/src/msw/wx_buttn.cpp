/*
 * File:	wx_buttn.cc
 * Purpose:	Button implementation
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
#include "wx_panel.h"
#include "wx_buttn.h"
#endif

#include "wx_itemp.h"

#define BUTTON_HEIGHT_FACTOR (EDIT_CONTROL_FACTOR * 1.1)

// Buttons

BOOL wxButton::MSWCommand(UINT param, WORD WXUNUSED(id))
{
  if (param == BN_CLICKED)
  {
    wxCommandEvent event(wxEVENT_TYPE_BUTTON_COMMAND);
    event.eventObject = this;
    ProcessCommand(event);
    return TRUE;
  }
  else return FALSE;
}

wxButton::wxButton(void)
{
  wxWinType = wxTYPE_HWND;
  windows_id = 0;
  ms_handle = 0;
  buttonBitmap = NULL;
}

wxButton::wxButton(wxPanel *panel, wxFunction Function, Const char *label,
		   int x, int y, int width, int height,
                   long style, Constdata char *name):
  wxbButton(panel, Function, label, x, y, width, height, style, name)
{
  Create(panel, Function, label, x, y, width, height, style, name);
}

wxButton::wxButton(wxPanel *panel, wxFunction Function, wxBitmap *bitmap,
		   int x, int y, int width, int height,
                   long style, Constdata char *name):
  wxbButton(panel, Function, bitmap, x, y, width, height, style, name)
{
  Create(panel, Function, bitmap, x, y, width, height, style, name);
}

Bool wxButton::Create(wxPanel *panel, wxFunction Function, Const char *label,
		   int x, int y, int width, int height,
                   long style, Constdata char *name)
{
  buttonBitmap = NULL;
  isBitmap = FALSE;
  SetName(name);
  
  if (panel) panel->AddChild(this);
  buttonFont = panel->buttonFont ;

  if (buttonFont)
    buttonFont->UseResource();

  backColour = panel->backColour ;
  labelColour = panel->labelColour ;
  buttonColour = panel->buttonColour ;

  wxWinType = wxTYPE_HWND;
  windowStyle = style;

  panel->GetValidPosition(&x, &y);

//  windows_id = (int)NewId();
  windows_id = NewControlId();

/* Sorry, but Fafa seems to mess up default button setting,
 * so we're reverting to normal Windows buttons this time.
 * JACS 29/3/94
#if FAFA_LIB
  HWND wx_button =
      CreateWindowEx(0, FafaButt, label, FB_TEXT | WS_TABSTOP | WS_CHILD,
                     0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                     wxhInstance, NULL);
#else
*/
/*
  char buf[100];
  sprintf(buf, "Creating wxButton with id %d\n", windows_id);
  wxDebugMsg(buf);
  wxYield();
*/

  HWND wx_button =
    CreateWindowEx(0, "BUTTON", label, BS_PUSHBUTTON | WS_TABSTOP | WS_CHILD,
                    0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                    wxhInstance, NULL);

#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(wx_button);
#endif

  ms_handle = (HANDLE)wx_button;

  // Subclass again for purposes of dialog editing mode
  SubclassControl(wx_button);

//  HDC the_dc = GetWindowDC((HWND)ms_handle) ;
  if (buttonFont)
  {
    buttonFont->RealizeResource();

    // This resource won't be deleted during the lifetime of this button,
    // because wxFont::ReleaseResource isn't called until the destructor, and
    // meanwhile, the resource is locked from being cleaned up.
    if (buttonFont->GetResourceHandle())
      SendMessage((HWND)ms_handle,WM_SETFONT,
                  (WPARAM)buttonFont->GetResourceHandle(),0L);
  }
  
//  ReleaseDC((HWND)ms_handle,the_dc) ;

  SetSize(x, y, width, height);
  ShowWindow(wx_button, SW_SHOW);

  Callback(Function);

  panel->AdvanceCursor(this);
  return TRUE;
}

Bool wxButton::Create(wxPanel *panel, wxFunction Function, wxBitmap *bitmap,
		   int x, int y, int width, int height,
                   long style, Constdata char *name)
{
  buttonBitmap = bitmap;
  isBitmap = TRUE;
  SetName(name);

  if (panel) panel->AddChild(this);
  buttonFont = panel->buttonFont ;

  if (buttonFont)
    buttonFont->UseResource();
    
  backColour = panel->backColour ;
  labelColour = panel->labelColour ;
  buttonColour = panel->buttonColour ;
  wxWinType = wxTYPE_HWND;
  windowStyle = style;

  panel->GetValidPosition(&x, &y);

//  windows_id = (int)NewId();
  windows_id = NewControlId();

#if FAFA_LIB
  if (width<0)
    width = bitmap->GetWidth() ;
  if (height<0)
    height = bitmap->GetHeight() ;
  width += FB_MARGIN ;
  height+= FB_MARGIN ;

  HWND wx_button =
      CreateWindowEx(0, FafaButt, "?", FB_BITMAP | WS_TABSTOP | WS_CHILD,
                     0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                     wxhInstance, NULL);
  if (bitmap)
    SendMessage((HWND)wx_button,WM_CHANGEBITMAP,
                  (WPARAM)((bitmap->GetHeight()<<8)+bitmap->GetWidth()),
                  (LPARAM)bitmap->ms_bitmap);
#else
  HWND wx_button =
    CreateWindowEx(0, "BUTTON", "not implemented", BS_PUSHBUTTON | WS_TABSTOP | WS_CHILD,
                    0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                    wxhInstance, NULL);
#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(wx_button);
#endif
#endif

  ms_handle = (HANDLE)wx_button;

  // Subclass again for purposes of dialog editing mode
  SubclassControl(wx_button);

  if (buttonFont)
  {
    buttonFont->RealizeResource();
  
    if (buttonFont->GetResourceHandle())
      SendMessage((HWND)ms_handle,WM_SETFONT,
                (WPARAM)buttonFont->GetResourceHandle(),0L);
  }
  
  SetSize(x, y, width, height);
  ShowWindow(wx_button, SW_SHOW);

  Callback(Function);

  panel->AdvanceCursor(this);
  return TRUE;
}

wxButton::~wxButton(void)
{
}

void wxButton::SetLabel(char *label)
{
  buttonBitmap = NULL;
  isBitmap = FALSE;
#if FAFA_LIB
    // This message will switch from FB_BITMAP style to FB_TEXT, if needed.
    SendMessage((HWND)ms_handle,WM_CHANGEBITMAP,
                (WPARAM)0,
                (LPARAM)NULL);
#endif
  SetWindowText((HWND)ms_handle, label);
}

void wxButton::SetLabel(wxBitmap *bitmap)
{
  buttonBitmap = bitmap;
  isBitmap = TRUE;
#if FAFA_LIB
    SendMessage((HWND)ms_handle,WM_CHANGEBITMAP,
                (WPARAM)((bitmap->GetHeight()<<8)+bitmap->GetWidth()),
                (LPARAM)bitmap->ms_bitmap);
#endif
}

char *wxButton::GetLabel(void)
{
  GetWindowText((HWND)ms_handle, wxBuffer, 300);
  return wxBuffer;
}

void wxButton::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  HWND button = (HWND)ms_handle;

  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  int actualWidth = width;
  int actualHeight = height;
  int ww, hh;
  GetSize(&ww, &hh);

  float current_width;
  float cyf;
  char buf[300];
  GetWindowText(button, buf, 300);
  GetTextExtent(buf, &current_width, &cyf,NULL,NULL,buttonFont);

  // If we're prepared to use the existing width, then...
  if (width == -1 && ((sizeFlags & wxSIZE_AUTO_WIDTH) != wxSIZE_AUTO_WIDTH))
    actualWidth = ww;
  else if (width == -1)
  {
    int cx;
    int cy;
    wxGetCharSize(button, &cx, &cy,buttonFont);
    actualWidth = (int)(current_width + 3*cx) ;
  }
  
  // If we're prepared to use the existing height, then...
  if (height == -1 && ((sizeFlags & wxSIZE_AUTO_HEIGHT) != wxSIZE_AUTO_HEIGHT))
    actualHeight = hh;
  else if (height == -1)
  {
    actualHeight = (int)(cyf*BUTTON_HEIGHT_FACTOR) ;
  }

  MoveWindow(button, x, y, actualWidth, actualHeight, TRUE);

  if (!((width == -1) && (height == -1)))
    GetEventHandler()->OnSize(width, height);
}

void wxButton::SetFocus(void)
{
/*
  wxPanel *panel = (wxPanel *)GetParent();
  if (panel)
  {
    SendMessage(panel->GetHWND(), DM_SETDEFID, windows_id, 0L);
  }
*/
  wxItem::SetFocus();
}

void wxButton::SetDefault(void)
{
  wxPanel *panel = (wxPanel *)GetParent();
  if (panel)
    panel->defaultItem = this;

  if (panel)
  {
    SendMessage(panel->GetHWND(), DM_SETDEFID, windows_id, 0L);
  }
}
