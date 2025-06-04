/*
 * File:	wx_group.cc
 * Purpose:	Group box implementation
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

#include "wx_group.h"
#include "wx_itemp.h"

/*
 * Group box
 */
 
wxGroupBox::wxGroupBox(void)
{
  wxWinType = wxTYPE_HWND;
  windows_id = 0;
  ms_handle = 0;
}

wxGroupBox::wxGroupBox(wxPanel *panel, Const char *label,
		   int x, int y, int width, int height,
                   long style, Constdata char *name):
  wxbGroupBox(panel, label, x, y, width, height, style, name)
{
  Create(panel, label, x, y, width, height, style, name);
}

Bool wxGroupBox::Create(wxPanel *panel, Const char *label,
		   int x, int y, int width, int height,
                   long style, Constdata char *name)
{
  SetName(name);
  if (panel) panel->AddChild(this);
  buttonFont = panel->buttonFont ;
//  labelFont = panel->labelFont ;

  if (buttonFont)
    buttonFont->UseResource();

  SetBackgroundColour(*panel->GetBackgroundColour()) ;
  labelColour = panel->labelColour ;
  buttonColour = panel->buttonColour ;

  wxWinType = wxTYPE_HWND;
  windowStyle = style;

  panel->GetValidPosition(&x, &y);

  windows_id = (int)NewControlId();

  long msStyle = GROUP_FLAGS;
#if WIN95
//  msStyle |= WS_EX_CLIENTEDGE;
#endif

  HWND wx_button =
    CreateWindowEx(0, GROUP_CLASS, label, msStyle,
                    0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                    wxhInstance, NULL);

#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(wx_button);
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

//  Callback(Function);

  panel->AdvanceCursor(this);
  return TRUE;
}

wxGroupBox::~wxGroupBox(void)
{
}

void wxGroupBox::SetLabel(char *label)
{
  SetWindowText((HWND)ms_handle, label);
}

char *wxGroupBox::GetLabel(void)
{
  GetWindowText((HWND)ms_handle, wxBuffer, 300);
  return wxBuffer;
}

void wxGroupBox::SetSize(int x, int y, int width, int height, int sizeFlags)
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

  float current_width;

  int cx;
  int cy;
  float cyf;

  HWND button = (HWND)ms_handle;
  wxGetCharSize(button, &cx, &cy,buttonFont);

  GetWindowText(button, buf, 300);
  GetTextExtent(buf, &current_width, &cyf,NULL,NULL,buttonFont);
  if (width < 0)
   width = (int)(current_width + 3*cx) ;
  if (height<0)
    height = (int)(cyf*EDIT_CONTROL_FACTOR) ;
  MoveWindow(button, x, y, width, height, TRUE);

  GetEventHandler()->OnSize(width, height);
}
