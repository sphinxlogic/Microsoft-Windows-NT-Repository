/*
 * File:	wx_check.cc
 * Purpose:	Check box implementation
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
#include "wx_check.h"
#endif

#include "wx_itemp.h"

BOOL wxCheckBox::MSWCommand(UINT WXUNUSED(param), WORD WXUNUSED(id))
{
  wxCommandEvent event(wxEVENT_TYPE_CHECKBOX_COMMAND);
  event.commandInt = GetValue();
  event.eventObject = this;
  ProcessCommand(event);
  return TRUE;
}

// Single check box item
wxCheckBox::wxCheckBox(void)
{
  wxWinType = wxTYPE_HWND;
  windows_id = 0;
  ms_handle = 0;
  isFafa = CHECK_IS_FAFA ;
}

// Single check box item
wxCheckBox::wxCheckBox(wxPanel *panel, wxFunction func, Const char *Title,
                       int x, int y, int width, int height, long style, Constdata char *name):
  wxbCheckBox(panel, func, Title, x, y, width, height, style, name)
{
  Create(panel, func, Title, x, y, width, height, style, name);
}

wxCheckBox::wxCheckBox(wxPanel *panel, wxFunction func, wxBitmap *bitmap,
                       int x, int y, int width, int height, long style, Constdata char *name):
  wxbCheckBox(panel, func, bitmap, x, y, width, height, style, name)
{
  Create(panel, func, bitmap, x, y, width, height, style, name);
}

// Single check box item
Bool wxCheckBox::Create(wxPanel *panel, wxFunction func, Const char *Title,
                       int x, int y, int width, int height, long style, Constdata char *name)
{
  SetName(name);
  if (panel) panel->AddChild(this);
//  buttonFont = panel->buttonFont ;

  labelFont = panel->labelFont ;
  if (labelFont)
    labelFont->UseResource();

  SetBackgroundColour(*panel->GetBackgroundColour()) ;
  labelColour = panel->labelColour ;
  buttonColour = panel->buttonColour ;

  wxWinType = wxTYPE_HWND;
  windowStyle = style;

  if (!Title)
    Title = " "; // Apparently needed or checkbox won't show

  panel->GetValidPosition(&x, &y);

  windows_id = NewControlId();

  if (GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS)
    isFafa = FALSE;
  else
    isFafa = CHECK_IS_FAFA ;
  
  checkWidth = -1 ;
  checkHeight = -1 ;

  long msStyle = (isFafa ? CHECK_FLAGS : (BS_AUTOCHECKBOX|WS_TABSTOP|WS_CHILD));
#if WIN95
  msStyle |= WS_EX_CLIENTEDGE;
#endif

  HWND wx_button = CreateWindowEx(0, isFafa ? CHECK_CLASS : "BUTTON", Title,
                    msStyle,
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
  if (labelFont)
  {
    labelFont->RealizeResource();
  
    if (labelFont->GetResourceHandle())
      SendMessage((HWND)ms_handle,WM_SETFONT,
                (WPARAM)labelFont->GetResourceHandle(),0L);
  }
  
//  ReleaseDC((HWND)ms_handle,the_dc) ;

  SetSize(x, y, width, height);

  ShowWindow(wx_button, SW_SHOW);
  panel->AdvanceCursor(this);
  Callback(func);
  return TRUE;
}

Bool wxCheckBox::Create(wxPanel *panel, wxFunction func, wxBitmap *bitmap,
                       int x, int y, int width, int height, long style, Constdata char *name)
{
  SetName(name);
  if (panel) panel->AddChild(this);
//  buttonFont = panel->buttonFont ;
  labelFont = panel->labelFont ;

  if (labelFont)
    labelFont->UseResource();

  SetBackgroundColour(*panel->GetBackgroundColour()) ;
  labelColour = panel->labelColour ;
  buttonColour = panel->buttonColour ;
  wxWinType = wxTYPE_HWND;
  windowStyle = style;

  panel->GetValidPosition(&x, &y);

  windows_id = NewControlId();

#if FAFA_LIB // && !CTL3D
  if (width<0)
    width = bitmap->GetWidth() ;
  if (height<0)
    height = bitmap->GetHeight() ;
  checkWidth = width ;
  checkHeight = height ;
  width += FB_MARGIN ;
  height += FB_MARGIN ;

  long msStyle = BITCHECK_FLAGS;
#if WIN95
//  msStyle |= WS_EX_CLIENTEDGE;
#endif
  HWND wx_button = CreateWindowEx(0, FafaChck, "toggle",
                    msStyle,
                    0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                    wxhInstance, NULL);
      SendMessage((HWND)wx_button,WM_CHANGEBITMAP,
                  (WPARAM)((bitmap->GetHeight()<<8)+bitmap->GetWidth()),
                  (LPARAM)bitmap->ms_bitmap);
  isFafa = TRUE;
#else
  isFafa = CHECK_IS_FAFA;
  checkWidth = -1 ;
  checkHeight = -1 ;
  long msStyle = CHECK_FLAGS;
#if WIN95
//  msStyle |= WS_EX_CLIENTEDGE;
#endif
  HWND wx_button = CreateWindowEx(0, CHECK_CLASS, "toggle",
                    msStyle,
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

//  HDC the_dc = GetWindowDC((HWND)ms_handle) ;

  if (labelFont)
  {
    labelFont->RealizeResource();
    
    if (labelFont->GetResourceHandle())
      SendMessage((HWND)ms_handle,WM_SETFONT,
                (WPARAM)labelFont->GetResourceHandle(),0L);
  }
//  ReleaseDC((HWND)ms_handle,the_dc) ;

  SetSize(x, y, width, height);

  ShowWindow(wx_button, SW_SHOW);
  panel->AdvanceCursor(this);
  Callback(func);
  return TRUE;
}

wxCheckBox::~wxCheckBox(void)
{
}

void wxCheckBox::SetLabel(char *label)
{
#if FAFA_LIB && !CTL3D
    checkWidth = checkHeight = -1 ;
    // This message will switch from FB_BITMAP style to FB_TEXT, if needed.
    SendMessage((HWND)ms_handle,WM_CHANGEBITMAP,
                (WPARAM)0,
                (LPARAM)NULL);
#endif
  SetWindowText((HWND)ms_handle, label);
}

char *wxCheckBox::GetLabel()
{
  char buf[300];
  GetWindowText((HWND)ms_handle, buf, 300);
  return copystring(buf);
}

void wxCheckBox::SetLabel(wxBitmap *bitmap)
{
#if FAFA_LIB // && !CTL3D
    checkWidth = bitmap->GetWidth() ;
    checkHeight = bitmap->GetHeight() ;
    SendMessage((HWND)ms_handle,WM_CHANGEBITMAP,
                (WPARAM)((bitmap->GetHeight()<<8)+bitmap->GetWidth()),
                (LPARAM)bitmap->ms_bitmap);
#endif
}

void wxCheckBox::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  char buf[300];

  float current_width;

  int cx;
  int cy;
  float cyf;

  HWND button = (HWND)ms_handle;
#if FAFA_LIB && !CTL3D
  if (checkWidth<0)
#endif
  {
    GetWindowText(button, buf, 300);
    if (buf[0])
    {
      GetTextExtent(buf, &current_width, &cyf, NULL, NULL, labelFont);
      if (width < 0)
        width = (int)(current_width + RADIO_SIZE);
      if (height < 0)
      {
        height = (int)(cyf);
        if (height < RADIO_SIZE)
          height = RADIO_SIZE;
      }
    }
    else
    {
      if (width < 0)
        width = RADIO_SIZE;
      if (height < 0)
        height = RADIO_SIZE;
    }
/*
    wxGetCharSize(button, &cx, &cy, labelFont);

    GetWindowText(button, buf, 300);
    GetTextExtent(buf, &current_width, &cyf,NULL,NULL,labelFont);
    if (width < 0)
      width = (int)(current_width + RADIO_SIZE) ;
    if (height<0)
      height = (int)(cyf) ;
*/
  }
#if FAFA_LIB && !CTL3D
  else
  {
    if (width<0)
      width = checkWidth + FB_MARGIN ;
    if (height<0)
      height = checkHeight + FB_MARGIN ;
  }
#endif
  MoveWindow(button, x, y, width, height, TRUE);

  GetEventHandler()->OnSize(width, height);
}


void wxCheckBox::SetValue(Bool val)
{
/*
// Following necessary for Win32s, because Win32s translate BM_SETCHECK
#if FAFA_LIB && !CTL3D
  SendMessage((HWND)ms_handle, FAFA_SETCHECK, val, 0);
#else
  SendMessage((HWND)ms_handle, BM_SETCHECK, val, 0);
#endif
*/
#if FAFA_LIB
  SendMessage((HWND)ms_handle, isFafa?FAFA_SETCHECK:BM_SETCHECK, val, 0);
#else
  SendMessage((HWND)ms_handle, BM_SETCHECK, val, 0);
#endif
}

Bool wxCheckBox::GetValue(void)
{
/*
// Following necessary for Win32s, because Win32s translate BM_SETCHECK
#if FAFA_LIB && !CTL3D
  return (Bool)(0x003 & SendMessage((HWND)ms_handle, FAFA_GETCHECK, 0, 0));
#else
  return (Bool)(0x003 & SendMessage((HWND)ms_handle, BM_GETCHECK, 0, 0));
#endif
*/
#if FAFA_LIB
  return (Bool)(0x003 & SendMessage((HWND)ms_handle,
                isFafa?FAFA_GETCHECK:BM_GETCHECK, 0, 0));
#else
  return (Bool)(0x003 & SendMessage((HWND)ms_handle, BM_GETCHECK, 0, 0));
#endif
}

void wxCheckBox::SetLabelFont(wxFont *font)
{
  // Decrement the usage count of the old label font
  // (we may be able to free it up)
  if (labelFont)
    labelFont->ReleaseResource();
    
  labelFont = font;
  
  // Increment usage count
  if (font)
    font->UseResource();
    
  HWND hWnd = GetHWND();
  if (hWnd != 0)
  {
    if (font)
    {
      font->RealizeResource();
      
      if (font->GetResourceHandle())
        SendMessage(hWnd, WM_SETFONT,
                  (WPARAM)font->GetResourceHandle(),TRUE);
    }
  }
}
