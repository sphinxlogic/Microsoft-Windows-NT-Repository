/*
 * File:	wx_messg.cc
 * Purpose:	Message item implementation
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
#include "wx_messg.h"
#endif

#include <stdio.h>
#include "wx_itemp.h"

// Message
wxMessage::wxMessage(void)
{
  wxWinType = wxTYPE_HWND;
  ms_handle = 0;
  messageBitmap = NULL;
}

wxMessage::wxMessage(wxPanel *panel, Const char *label, int x, int y, long style, Constdata char *name):
  wxbMessage(panel, label, x, y, style, name)
{
  Create(panel, label, x, y, -1, -1, style, name);
}

wxMessage::wxMessage(wxPanel *panel, Const char *label, int x, int y, int width, int height, long style, Constdata char *name):
  wxbMessage(panel, label, x, y, width, height, style, name)
{
  Create(panel, label, x, y, width, height, style, name);
}

#if USE_BITMAP_MESSAGE
wxMessage::wxMessage(wxPanel *panel, wxBitmap *image, int x, int y, long style, Constdata char *name):
  wxbMessage(panel, image, x, y, style, name)
{
  Create(panel, image, x, y, -1, -1, style, name);
}

wxMessage::wxMessage(wxPanel *panel, wxBitmap *image, int x, int y, int width, int height, long style, Constdata char *name):
  wxbMessage(panel, image, x, y, width, height, style, name)
{
  Create(panel, image, x, y, width, height, style, name);
}
#endif
  
Bool wxMessage::Create(wxPanel *panel, Const char *label, int x, int y, int width, int height, long style, Constdata char *name)
{
  messageBitmap = NULL;
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
  
  long msStyle = WS_CHILD|WS_VISIBLE;
  if (windowStyle & wxALIGN_CENTRE)
    msStyle |= SS_CENTER;
  else if (windowStyle & wxALIGN_RIGHT)
    msStyle |= SS_RIGHT;
  else
    msStyle |= SS_LEFT;

  HWND static_item = CreateWindowEx(0, "STATIC", label,
                         msStyle,
                         0, 0, 0, 0, panel->GetHWND(), (HMENU)NewControlId(),
                         wxhInstance, NULL);

#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(static_item);
#endif

  ms_handle = (HANDLE)static_item;

  SubclassControl(static_item);

  if (labelFont)
  {
    labelFont->RealizeResource();
    if (labelFont->GetResourceHandle())
      SendMessage((HWND)ms_handle,WM_SETFONT,
                (WPARAM)labelFont->GetResourceHandle(),0L);
  }

  panel->GetValidPosition(&x, &y);
  
  SetSize(x, y, width, height);
  panel->AdvanceCursor(this);
  return TRUE;
}

#if USE_BITMAP_MESSAGE
Bool wxMessage::Create(wxPanel *panel, wxBitmap *image, int x, int y, int width, int height, long style, Constdata char *name)
{
  messageBitmap = image;
  SetName(name);
  if (panel) panel->AddChild(this);
//  buttonFont = panel->buttonFont ;
  labelFont = panel->labelFont ;
  if (labelFont)
    labelFont->UseResource();
  backColour = panel->backColour ;
  labelColour = panel->labelColour ;
  buttonColour = panel->buttonColour ;
  wxWinType = wxTYPE_HWND;
  windowStyle = style;

  HWND static_item = CreateWindowEx(0, FafaStat, NULL,
//                         FS_BITMAP | FS_Y4 | FS_X4 | WS_CHILD | WS_VISIBLE | WS_GROUP,
                         FS_BITMAP | FS_X2 | FS_Y2 | WS_CHILD | WS_VISIBLE | WS_GROUP,
                         0, 0, 0, 0, panel->GetHWND(), (HMENU)NewControlId(),
                         wxhInstance, NULL);
  if (image)
    SendMessage((HWND)static_item,WM_CHANGEBITMAP,
                  (WPARAM)((image->GetHeight()<<8)+image->GetWidth()),
                  (LPARAM)image->ms_bitmap);
/*
#if CTL3D
  Ctl3dSubclassCtl(static_item);
#endif
*/
  ms_handle = (HANDLE)static_item;

  // Subclass again for purposes of dialog editing mode
  SubclassControl(static_item);

  panel->GetValidPosition(&x, &y);

  SetSize(x, y, image ? image->GetWidth() : 0, image ? image->GetHeight() : 0);
  panel->AdvanceCursor(this);
  return TRUE;
}
#endif

wxMessage::~wxMessage(void)
{
}

void wxMessage::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  int actualWidth = width;
  int actualHeight = height;

  char buf[300];
  float current_width;
  float cyf;
  
  // This returns zero!!! and buf is empty. Why doesn't it work?
  int retValue = GetWindowText((HWND)ms_handle, buf, 300);

/*
  if (retValue == 0)
  {
    char txtBuf[100];
    sprintf(txtBuf, "GetWindowText returned extended error %ld", (long)GetLastError());
    MessageBox(NULL, txtBuf, "Caption", MB_OK);
  }
*/

  GetTextExtent(buf, &current_width, &cyf, NULL, NULL,labelFont);

  int ww, hh;
  GetSize(&ww, &hh);

  // If we're prepared to use the existing width, then...
  if (width == -1 && ((sizeFlags & wxSIZE_AUTO_WIDTH) != wxSIZE_AUTO_WIDTH))
    actualWidth = ww;
  else if (width == -1)
  {
    int cx;
    int cy;
    wxGetCharSize((HWND)ms_handle, &cx, &cy,labelFont);
    actualWidth = (int)(current_width + cx) ;
  }

  // If we're prepared to use the existing height, then...
  if (height == -1 && ((sizeFlags & wxSIZE_AUTO_HEIGHT) != wxSIZE_AUTO_HEIGHT))
    actualHeight = hh;
  else if (height == -1)
  {
    actualHeight = (int)(cyf) ;
  }

  MoveWindow((HWND)ms_handle, x, y, actualWidth, actualHeight, TRUE);

  if (!((width == -1) && (height == -1)))
    GetEventHandler()->OnSize(actualWidth, actualHeight);
}

void wxMessage::SetLabel(char *label)
{
  messageBitmap = NULL;
  isBitmap = FALSE;
  float w, h;
  RECT rect;

  wxWindow *parent = GetParent();
  GetWindowRect((HWND)ms_handle, &rect);

  // Since we now have the absolute screen coords,
  // if there's a parent we must subtract its top left corner
  POINT point;
  point.x = rect.left;
  point.y = rect.top;
  if (parent)
  {
    ::ScreenToClient(parent->GetHWND(), &point);
  }

  GetTextExtent(label, &w, &h, NULL, NULL, labelFont);
  MoveWindow((HWND)ms_handle, point.x, point.y, (int)(w + 10), (int)h,
             TRUE);
  SetWindowText((HWND)ms_handle, label);
}

#if USE_BITMAP_MESSAGE
void wxMessage::SetLabel(wxBitmap *bitmap)
{
#if FAFA_LIB
  messageBitmap = bitmap;
  isBitmap = TRUE;

  int x, y;
  int w, h;
  GetPosition(&x, &y);
  GetSize(&w, &h);
  RECT rect;
  rect.left = x; rect.top = y; rect.right = x + w; rect.bottom = y + h;
  
  MoveWindow((HWND)ms_handle, x, y, bitmap->GetWidth(), bitmap->GetHeight(),
             FALSE);
  
  SendMessage((HWND)ms_handle,WM_CHANGEBITMAP,
                (WPARAM)((bitmap->GetHeight()<<8)+bitmap->GetWidth()),
                (LPARAM)bitmap->ms_bitmap);
  
  InvalidateRect(GetParent()->GetHWND(), &rect, TRUE);
#endif
}
#endif

void wxMessage::SetLabelFont(wxFont *font)
{
  // Decrement the usage count of the old label font
  // (we may be able to free it up)
  if (labelFont)
    labelFont->ReleaseResource();
    
  labelFont = font;
  HWND hWnd = GetHWND();
  if (hWnd != 0)
  {
    if (font)
    {
      // Increment usage count
      font->UseResource();
      font->RealizeResource();
      
      if (font->GetResourceHandle())
        SendMessage(hWnd, WM_SETFONT,
                  (WPARAM)font->GetResourceHandle(),TRUE);
    }
  }
}
