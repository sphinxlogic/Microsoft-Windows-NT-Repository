/*
 * File:	wx_panel.cc
 * Purpose:	wxPanel class implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_panel.cc,v 1.1 1994/08/14 21:59:17 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   26 May 97    3:21 pm
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_setup.h"
#include "wx_panel.h"
#include "wx_frame.h"
#include "wx_utils.h"
#include "wx_dcpan.h"
#endif

#include "wx_privt.h"

#if USE_EXTENDED_STATICS
#include "wx_stat.h"
#endif

extern char wxPanelClassName[];
extern char wxCanvasClassName[];

#if 0
class wxPanelWnd : public wxSubWnd
{
public:
  wxPanelWnd(wxWnd *parent, char *winClass, wxWindow *wx_win,
              int x, int y, int width, int height, DWORD flags);

  // Handlers
  LONG DefWindowProc(UINT nMsg, UINT wParam, LONG lParam);
  BOOL ProcessMessage(MSG* pMsg);
  BOOL OnEraseBkgnd(HDC pDC);
};

wxPanelWnd::wxPanelWnd(wxWnd *parent, char *winClass, wxWindow *wx_win,
              int x, int y, int width, int height, DWORD flags):
  wxSubWnd(parent, winClass, wx_win, x, y, width, height, flags)
{
}

LONG wxPanelWnd::DefWindowProc(UINT nMsg, UINT wParam, LPARAM lParam)
{
  return ::DefWindowProc(handle, nMsg, wParam, lParam);
}

BOOL wxPanelWnd::ProcessMessage(MSG* pMsg)
{
  return ::IsDialogMessage(handle, pMsg);
}

BOOL wxPanelWnd::OnEraseBkgnd(HDC pDC)
{
  if (background_brush)
  {
    RECT rect;
    GetClientRect(handle, &rect);
    int mode = SetMapMode(pDC, MM_TEXT);
    FillRect(pDC, &rect, background_brush);
    SetMapMode(pDC, mode);
    return TRUE;
  }
  else return FALSE;
}
#endif

wxPanel::wxPanel(void)
{
  tempPS = 0;
  window_parent = NULL;
  cursor_x = PANEL_LEFT_MARGIN;
  cursor_y = PANEL_TOP_MARGIN;
  max_height = 0;
  max_line_height = 0;
  max_width = 0;
  hSpacing = PANEL_HSPACING;
  vSpacing = PANEL_VSPACING;
  initial_hspacing = hSpacing ;
  initial_vspacing = vSpacing ;
  current_hspacing = hSpacing ;
  current_vspacing = vSpacing ;

  new_line = FALSE;
  label_position = wxHORIZONTAL;
  wxWinType = wxTYPE_XWND;
  handle = NULL;
  has_child = FALSE ;
  last_created = 0 ;
  labelFont = NULL ;
  buttonFont = NULL ;
  backColour = *wxWHITE ;
  labelColour = *wxBLACK ;
  buttonColour = *wxBLACK ;
}

// Constructor
wxPanel::wxPanel(wxWindow *parent, int x, int y, int width, int height,
                 long style, Constdata char *name):
  wxbPanel(parent, x, y, width, height, style, name)
{
  wx_dc = NULL;

  Create(parent, x, y, width, height, style, name);
}

// Constructor
Bool wxPanel::Create(wxWindow *parent, int x, int y, int width, int height, long style,
                     Constdata char *name)
{
  if (!parent)
    return FALSE;

  SetName(name);

  is_retained = ((style & wxRETAINED) == wxRETAINED);

  if (parent->IsKindOf(CLASSINFO(wxPanel)))
  {
    wxPanel *parentPanel = (wxPanel *)parent;
/*
    parentPanel->GetValidPosition(&x,&y) ;
*/
    labelFont = parentPanel->labelFont ;
    buttonFont = parentPanel->buttonFont ;
    backColour = parentPanel->backColour ;
    labelColour = parentPanel->labelColour ;
    buttonColour = parentPanel->buttonColour ;
  }

  cursor_x = PANEL_LEFT_MARGIN;
  cursor_y = PANEL_TOP_MARGIN;
  max_height = 0;
  max_line_height = 0;
  max_width = 0;
  hSpacing = PANEL_HSPACING;
  vSpacing = PANEL_VSPACING;
  initial_hspacing = hSpacing ;
  initial_vspacing = vSpacing ;
  current_hspacing = hSpacing ;
  current_vspacing = vSpacing ;
  new_line = FALSE;
  label_position = wxHORIZONTAL;
  wxWinType = wxTYPE_XWND;

  windowStyle = style;

  if (windowStyle & wxVERTICAL_LABEL)
    SetLabelPosition(wxVERTICAL);
  else if (windowStyle & wxHORIZONTAL_LABEL)
    SetLabelPosition(wxHORIZONTAL);
  else
    SetLabelPosition(wxHORIZONTAL);

  has_child = FALSE ;
  last_created = 0 ;
  tempPS = 0;

  labelFont = wxTheFontList->FindOrCreateFont(11, wxSWISS, wxNORMAL, wxNORMAL);
  buttonFont = wxTheFontList->FindOrCreateFont(11, wxSWISS, wxNORMAL, wxNORMAL);
  backColour = *wxWHITE ;
  labelColour = *wxBLACK ;
  buttonColour = *wxBLACK ;

  window_parent = parent;

  wxWnd *cparent = NULL;
  if (parent)
    cparent = (wxWnd *)parent->handle;

  DWORD msflags = 0;
  if (style & wxBORDER)
    msflags |= WS_BORDER;
  if (style & wxTHICK_FRAME)
    msflags |= WS_THICKFRAME;
  // Note: with WS_CLIPCHILDREN we get less flicker, but some CTL3D controls
  // don't draw right, and groupboxes and radioboxes don't draw their contents.
  msflags |= WS_CHILD | WS_VISIBLE ; // | WS_CLIPCHILDREN;

  wxCanvasWnd *wnd = new wxCanvasWnd(cparent, wxCanvasClassName, this, x, y, width, height, msflags);

  handle = (char *)wnd;
  if (parent) parent->AddChild(this);
  if (parent->IsKindOf(CLASSINFO(wxPanel)))
  {
	 wxPanel *parentPanel = (wxPanel *)parent;
	 parentPanel->GetValidPosition(&x,&y) ;
  }

  wx_dc = new wxPanelDC (this);

  if (parent->IsKindOf(CLASSINFO(wxPanel)))
    ((wxPanel *)parent)->AdvanceCursor(this) ;

  return TRUE;
}

wxPanel::~wxPanel(void)
{
}

void wxPanel::SetLabelPosition(int pos)  // wxHORIZONTAL or wxVERTICAL
{
  label_position = pos;
}

void wxPanel::SetSize(int x, int y, int w, int h, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  int currentW,currentH;
  GetSize(&currentW, &currentH);
  if (w == -1)
    w = currentW ;
  if (h == -1)
    h = currentH ;

  if (GetHWND())
    MoveWindow(GetHWND(), x, y, w, h, TRUE);

  GetEventHandler()->OnSize(w, h);
}

/*****************************************************************
 * ITEM PLACEMENT FUNCTIONS
 *****************************************************************/

 // Start a new line
void wxPanel::RealNewLine(void)
{
  //cursor_x = PANEL_LEFT_MARGIN;
  cursor_x = initial_hspacing;
  if (max_line_height == 0)
  {
    cursor_y += current_vspacing;
  }
  else
    cursor_y += current_vspacing + max_line_height;
  max_line_height = 0;
  new_line = FALSE ;
}
  
void wxPanel::NewLine(int pixels)
{
  if (new_line)
    current_vspacing += pixels ;
  else
    current_vspacing = pixels ;
  new_line = TRUE ;
}

void wxPanel::NewLine(void)
{
  if (new_line)
    current_vspacing += vSpacing ;
  else
    current_vspacing = vSpacing ;
  new_line = TRUE ;
}

void wxPanel::Tab(void)
{
  cursor_x += hSpacing;
  if (cursor_x > max_width)
    max_width = cursor_x;
}

void wxPanel::Tab(int pixels)
{
  cursor_x += pixels;
  if (cursor_x > max_width)
    max_width = cursor_x;
}

void wxPanel::GetCursor(int *x, int *y)
{
  *x = cursor_x;
  *y = cursor_y;
}

// Set/get horizontal spacing
void wxPanel::SetHorizontalSpacing(int sp)
{
  hSpacing = sp;
  current_hspacing = sp ;
}

int wxPanel::GetHorizontalSpacing(void)
{
  return hSpacing;
}

// Set/get vertical spacing
void wxPanel::SetVerticalSpacing(int sp)
{
  vSpacing = sp;
  current_vspacing = sp ;
}

int wxPanel::GetVerticalSpacing(void)
{
  return vSpacing;
}

// Fits the panel around the items
void wxPanel::Fit(void)
{
  RealAdvanceCursor() ;
  SetClientSize(max_width + initial_hspacing,
                max_height + initial_vspacing);
/*
  SetClientSize(max_width + PANEL_HSPACING,
                max_height + PANEL_VSPACING);
*/
}

// Update next cursor position
void wxPanel::RealAdvanceCursor(void)
{
  wxWindow *item = last_created ;
  if (item)
  {
    int width, height;
    int x, y;
    item->GetSize(&width, &height);
    item->GetPosition(&x, &y);

    if ((x + width) > max_width)
      max_width = x + width;
    if ((y + height) > max_height)
      max_height = y + height;
    if (height > max_line_height)
      max_line_height = height;

    cursor_x = x + width + current_hspacing;
    cursor_y = y;
    last_created = NULL ;
  }
  if (new_line)
    RealNewLine() ;
}


// Update next cursor position
void wxPanel::AdvanceCursor(wxWindow *item)
{
/*
  int width, height;
  int x, y;
  item->GetSize(&width, &height);
  item->GetPosition(&x, &y);

  if ((x + width) > max_width)
    max_width = x + width;
  if ((y + height) > max_height)
    max_height = y + height;
  if (height > max_line_height)
    max_line_height = height;

  cursor_x = x + width + hspacing;
  cursor_y = y;
*/
   last_created = item ;
}

// If x or y are not specified (i.e. < 0), supply
// values based on left to right, top to bottom layout.
// Internal use only.
void wxPanel::GetValidPosition(int *x, int *y)
{
  if (*x < 0)
    *x = cursor_x;

  if (*y < 0)
    *y = cursor_y;
}

void wxPanel::Centre(int direction)
{
  int x, y, width, height, panel_width, panel_height, new_x, new_y;

  wxPanel *father = (wxPanel *)GetParent();
  if (!father)
    return;

  father->GetClientSize(&panel_width, &panel_height);
  GetSize(&width, &height);
  GetPosition(&x, &y);

  new_x = -1;
  new_y = -1;

  if (direction & wxHORIZONTAL)
    new_x = (int)((panel_width - width)/2);

  if (direction & wxVERTICAL)
    new_y = (int)((panel_height - height)/2);

  SetSize(new_x, new_y, -1, -1);

}

void wxPanel::Enable(Bool Flag)
{
  EnableWindow(GetHWND(),(BOOL)Flag) ;
}

void wxPanel::AddChild(wxObject *child)
{
  if (!has_child)
  {
    initial_hspacing = hSpacing ;
    initial_vspacing = vSpacing ;
  }
  has_child = TRUE ;

  cursor_x = hSpacing ;
  cursor_y = vSpacing ;
  RealAdvanceCursor() ;
  current_hspacing = hSpacing ;
  current_vspacing = vSpacing ;

  children->Append(child) ;
}

void wxPanel::SetBackgroundColour(wxColour& col)
{
  backColour = col;
}

void wxPanel::SetLabelColour(wxColour& col)
{
  labelColour = col;
}

void wxPanel::SetButtonColour(wxColour& col)
{
  buttonColour = col;
}

#if USE_EXTENDED_STATICS

#define LOH_MAX(a,b) ((a > b) ? a : b)
#define LOH_MIN(a,b) ((a < b) ? a : b)

Bool IsBoxCovered(RECT r1,RECT r2)
   {
     int x_min, x_max, y_min, y_max, w_max, h_max;

     x_min = LOH_MIN(r1.left  ,r2.left);
     x_max = LOH_MAX(r1.right ,r2.right);
     y_min = LOH_MIN(r1.top   ,r2.top);
     y_max = LOH_MAX(r1.bottom,r2.bottom);

     w_max = r1.right - r1.left + r2.right - r2.left;
     h_max = r1.bottom - r1.top + r2.bottom - r2.top;

     if (((x_max - x_min) <= (w_max)) && ((y_max - y_min) <= (h_max)))
          return TRUE;
     else return FALSE;
   }

void wxPanel::DrawAllStaticItems(PAINTSTRUCT *ps)
{
  int x,y,w,h;
  RECT r1;
  if (staticItems.Number() > 0)
  {
    wxDC *dc = GetDC();
    dc->BeginDrawing();
    wxNode *node = staticItems.First();
    wxStaticItem *item;
    while(node)
    {
       item = (wxStaticItem *)node -> Data();
       item -> GetDrawingSize(&x,&y,&w,&h);
       r1.left   = x;
       r1.top    = y;
       r1.right  = x + w;
       r1.bottom = y + h;

       if (!ps || (IsBoxCovered(ps -> rcPaint,r1) && item -> IsShow()))
       {
         item -> Draw(ps);
       }
       node = node -> Next();
    }
    dc->EndDrawing();
  }
}
#endif

void wxPanel::OnPaint(void)
{
#if USE_EXTENDED_STATICS
  DrawAllStaticItems(tempPS);
#endif
  PaintSelectionHandles();
}
