/*
 * File:	wx_item.cc
 * Purpose:	Panel item implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_item.cc,v 1.1 1994/08/14 21:59:17 edz Exp $
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
#include "wx_txt.h"
#include "wx_mtxt.h"
#include "wx_choic.h"
#include "wx_combo.h"
#endif

#include "wx_privt.h"
#include "wx_itemp.h"

#ifdef GNUWIN32
#define CASTWNDPROC (long unsigned)
#else
#define CASTWNDPROC
#endif

// The MakeProcInstance version of the function
FARPROC wxGenericControlSubClassProc = 0;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
wxList *wxControlHandleList = NULL;
#else
wxNonlockingHashTable *wxControlHandleList = NULL;
#endif

// Item members
wxItem::wxItem(void)
{
  isFafa = FALSE ;
  mswLastXPos = 0;
  mswLastYPos = 0;
  mswLastEvent = 0;
  isBeingDeleted = FALSE;
  labelhWnd = 0;
  backgroundBrush = 0;
}

wxItem::~wxItem(void)
{
  if (backgroundBrush)
  {
    ::DeleteObject(backgroundBrush);
    backgroundBrush = 0;
  }
    
  isBeingDeleted = TRUE;

  // Restore old Window proc
  if (GetHWND())
  {
    FARPROC farProc = (FARPROC) GetWindowLong(GetHWND(), GWL_WNDPROC);
    if ((oldWndProc != 0) && (farProc != oldWndProc))
      SetWindowLong(GetHWND(), GWL_WNDPROC, (LONG) oldWndProc);
  }
  
  // If we delete an item, we should initialize the parent panel,
  // because it could now be invalid.
  wxPanel *panel = (wxPanel *)GetParent();
  if (panel)
  {
    panel->last_created = NULL;
    panel->cursor_x = PANEL_LEFT_MARGIN;
    panel->cursor_y = PANEL_TOP_MARGIN;
    panel->max_height = 0;
    panel->max_line_height = 0;
    panel->max_width = 0;
    panel->hSpacing = PANEL_HSPACING;
    panel->vSpacing = PANEL_VSPACING;
    panel->initial_hspacing = panel->hSpacing ;
    panel->initial_vspacing = panel->vSpacing ;
    panel->current_hspacing = panel->hSpacing ;
    panel->current_vspacing = panel->vSpacing ;

    panel->new_line = FALSE;
    panel->label_position = wxHORIZONTAL;
    panel->has_child = FALSE ;
    panel->last_created = 0 ;
  }

  if (labelFont)
    labelFont->ReleaseResource();
  if (buttonFont)
    buttonFont->ReleaseResource();
}

void wxItem::GetSize(int *width, int *height)
{
  HWND wnd = (HWND)ms_handle;
  RECT rect;
  rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

  wxFindMaxSize(wnd, &rect);

  *width = rect.right - rect.left;
  *height = rect.bottom - rect.top;
}

void wxItem::GetPosition(int *x, int *y)
{
  HWND wnd = (HWND)ms_handle;
  wxWindow *parent = GetParent();
  RECT rect;
  rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

  wxFindMaxSize(wnd, &rect);

  // Since we now have the absolute screen coords,
  // if there's a parent we must subtract its top left corner
  POINT point;
  point.x = rect.left;
  point.y = rect.top;
  if (parent)
  {
    wxWnd *cparent = (wxWnd *)(parent->handle);
    ::ScreenToClient(cparent->handle, &point);
  }

  *x = point.x;
  *y = point.y;
}

void wxItem::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  wxWindow::SetSize(x, y, width, height, sizeFlags);
}

void wxItem::SetClientSize(int width, int height)
{
  SetSize(-1, -1, width, height);
}

void wxItem::SetLabel(char *label)
{
  if (labelhWnd)
    SetWindowText(labelhWnd, label);
  else if (ms_handle)
    SetWindowText(ms_handle, label);
}

char *wxItem::GetLabel(void)
{
  if (labelhWnd)
    GetWindowText((HWND)labelhWnd, wxBuffer, 1000);
  else if (ms_handle)
    GetWindowText((HWND)ms_handle, wxBuffer, 1000);
  else
    return NULL;
  return wxBuffer;
}

void wxItem::SetFocus(void)
{
  wxWindow::SetFocus();
}

Bool wxItem::Show(Bool show)
{
  int cshow;
  if (show)
    cshow = SW_SHOW;
  else
    cshow = SW_HIDE;
  ShowWindow(GetHWND(), (BOOL)cshow);
  if (GetLabelHWND() && (GetLabelHWND() != GetHWND()))
    ShowWindow(GetLabelHWND(), (BOOL)cshow);
  if (show)
    BringWindowToTop(GetHWND());
  return TRUE;
}

float wxItem::GetCharHeight(void)
{
  TEXTMETRIC lpTextMetric;
  HWND wnd = (HWND)ms_handle;
  HDC dc = GetDC(wnd);

  GetTextMetrics(dc, &lpTextMetric);
  ReleaseDC(wnd, dc);

  return (float)lpTextMetric.tmHeight;
}

float wxItem::GetCharWidth(void)
{
  TEXTMETRIC lpTextMetric;
  HWND wnd = (HWND)ms_handle;
  HDC dc = GetDC(wnd);

  GetTextMetrics(dc, &lpTextMetric);
  ReleaseDC(wnd, dc);

  return (float)lpTextMetric.tmAveCharWidth;
}

void wxItem::SetLabelFont(wxFont *font)
{
  // Decrement the usage count of the old label font
  // (we may be able to free it up)
  if (labelFont)
    labelFont->ReleaseResource();
    
  // Increment usage count
  if (font)
   font->UseResource();

  labelFont = font;
  HWND hWnd = GetLabelHWND();
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

void wxItem::SetButtonFont(wxFont *font)
{
  // Decrement the usage count of the old label font
  // (we may be able to free it up)
  if (buttonFont)
    buttonFont->ReleaseResource();
    
  buttonFont = font;

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

void wxItem::SetBackgroundColour(wxColour& col)
{
  if (backgroundBrush)
  {
    ::DeleteObject(backgroundBrush);
    backgroundBrush = 0;
  }
  backgroundBrush = ::CreateSolidBrush(RGB(col.Red(), col.Green(), col.Blue()));
    
  backColour = col;
}

void wxItem::Enable(Bool enable)
{
  winEnabled = enable;
  HWND hWnd = GetHWND();
  if (hWnd)
    ::EnableWindow(hWnd, (BOOL)enable);

  if (GetLabelHWND())
    ::EnableWindow(GetLabelHWND(), (BOOL)enable);
}

void wxItem::SubclassControl(HWND hWnd)
{
  // Subclass again for purposes of dialog editing mode
  wxAddControlHandle(hWnd, this);
  oldWndProc = (FARPROC) GetWindowLong(hWnd, GWL_WNDPROC);
  if (!wxGenericControlSubClassProc)
    wxGenericControlSubClassProc = MakeProcInstance((FARPROC) wxSubclassedGenericControlProc, wxhInstance);
  SetWindowLong(hWnd, GWL_WNDPROC, (LONG) wxGenericControlSubClassProc);
}

// Call this repeatedly for several wnds to find the overall size
// of the widget.
// Call it initially with -1 for all values in rect.
// Keep calling for other widgets, and rect will be modified
// to calculate largest bounding rectangle.
void wxFindMaxSize(HWND wnd, RECT *rect)
{
  int left = rect->left;
  int right = rect->right;
  int top = rect->top;
  int bottom = rect->bottom;

  GetWindowRect(wnd, rect);

  if (left < 0)
    return;

  if (left < rect->left)
    rect->left = left;

  if (right > rect->right)
    rect->right = right;

  if (top < rect->top)
    rect->top = top;

  if (bottom > rect->bottom)
    rect->bottom = bottom;

}

/*
// Not currently used
void wxConvertDialogToPixels(wxWindow *control, int *x, int *y)
{
  if (control->window_parent && control->window_parent->is_dialog)
  {
    DWORD word = GetDialogBaseUnits();
    int xs = LOWORD(word);
    int ys = HIWORD(word);
    *x = (int)(*x * xs/4);
    *y = (int)(*y * ys/8);
  }
  else
  {
    *x = *x;
    *y = *y;
  }
}
*/

// We can't rely on Windows giving us events corresponding to the wxWindows Z-ordering.
// E.g. we can't push a wxGroupBox to the back for editing purposes.
// Convert the item event to parent coordinates, then search for
// an item that could receive this event.
wxItem *wxFakeItemEvent(wxPanel *parent, wxItem *item, wxMouseEvent& event)
{
  int x, y;
  item->GetPosition(&x, &y);
  event.x += x;
  event.y += y;
  
  wxNode *node = parent->GetChildren()->Last();
  while (node)
  {
    wxItem *newItem = (wxItem *)node->Data();
    if (newItem->IsSelected() && newItem->SelectionHandleHitTest(event.x, event.y))
    {
      // This event belongs to the panel.
      parent->GetEventHandler()->OnEvent(event);
      return NULL;
    }
    else if (newItem->HitTest(event.x, event.y))
    {
      int x1, y1;
      newItem->GetPosition(&x1, &y1);
      event.x -= x1;
      event.y -= y1;
      newItem->OnEvent(event);
      return newItem;
    }
    node = node->Previous();
  }
  // No takers, so do what we would have done anyway.
  event.x -= x;
  event.y -= y;
  item->OnEvent(event);
  return item;
}

// Sub-classed generic control proc
LONG APIENTRY _EXPORT
  wxSubclassedGenericControlProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  wxWindow *win = wxFindControlFromHandle(hWnd);

  if (!win)
  {
    wxDebugMsg("Panic! Cannot find wxItem for this HWND in wxSubclassedGenericControlProc.\n");
    return FALSE;
  }
  if (!win->IsKindOf(CLASSINFO(wxItem)))
    return FALSE;
    
  wxItem *item = (wxItem *)win;

  // If not in edit mode (or has been removed from parent), call the default proc.
  wxPanel *panel = (wxPanel *)item->GetParent();

  if (!panel || item->isBeingDeleted)
    return CallWindowProc(CASTWNDPROC item->oldWndProc, hWnd, message, wParam, lParam);
   
  // Special edit control processing
  if (!panel->GetUserEditMode() && (item->IsKindOf(CLASSINFO(wxText))
#if USE_COMBOBOX
    || item->IsKindOf(CLASSINFO(wxComboBox))
#endif
// You may want wxChoice to react to return events too
#if 0
    || item->IsKindOf(CLASSINFO(wxChoice))
#endif
     ))
  {
    switch (message)
    {
      case WM_GETDLGCODE:
      {
        if (item->GetWindowStyleFlag() & wxPROCESS_ENTER)
          return DLGC_WANTALLKEYS;
        break;
      }
      case WM_CHAR: // Always an ASCII character
      {
        if (item->IsKindOf(CLASSINFO(wxMultiText)))
          return CallWindowProc(CASTWNDPROC item->oldWndProc, hWnd, message, wParam, lParam);
          
        if (wParam == VK_RETURN)
        {
          wxCommandEvent event(wxEVENT_TYPE_TEXT_ENTER_COMMAND);
          event.commandString = ((wxText *)item)->GetValue();
          event.eventObject = item;
          item->ProcessCommand(event);
          return FALSE;
        }
        break;
      }
      case WM_KILLFOCUS:
      {
        item->GetEventHandler()->OnKillFocus();
        return CallWindowProc(CASTWNDPROC item->oldWndProc, hWnd, message, wParam, lParam);
        break;
      }
      case WM_SETFOCUS:
      {
        item->GetEventHandler()->OnSetFocus();
        return CallWindowProc(CASTWNDPROC item->oldWndProc, hWnd, message, wParam, lParam);
        break;
      }
    }
  }

  if (!panel || !panel->GetUserEditMode())
    return CallWindowProc(CASTWNDPROC item->oldWndProc, hWnd, message, wParam, lParam);

  int x = (int)LOWORD(lParam);
  int y = (int)HIWORD(lParam);
  unsigned int flags = wParam;

  // Ok, this is truly weird, but if a panel with a wxChoice loses the
  // focus, then you get a *fake* WM_LBUTTONUP message
  // with x = 65535 and y = 65535.
  // Filter out this nonsense.
  if (message == WM_LBUTTONUP && item->IsKindOf(CLASSINFO(wxChoice)) && x == 65535 && y == 65535)
    return CallWindowProc(CASTWNDPROC item->oldWndProc, hWnd, message, wParam, lParam);

  // If a mouse message, must convert X and Y to item coordinates
  // from HWND coordinates (the HWND may be part of the composite wxItem)
  if ((message == WM_RBUTTONDOWN) || (message == WM_LBUTTONDOWN) || (message == WM_MBUTTONDOWN) || 
      (message == WM_RBUTTONUP) || (message == WM_LBUTTONUP) || (message == WM_MBUTTONUP) || 
      (message == WM_RBUTTONUP) || (message == WM_LBUTTONUP) || (message == WM_MBUTTONUP) || 
      (message == WM_RBUTTONDBLCLK) || (message == WM_LBUTTONDBLCLK) || (message == WM_MBUTTONDBLCLK) || 
      (message == WM_RBUTTONDBLCLK) || (message == WM_LBUTTONDBLCLK) || (message == WM_MBUTTONDBLCLK) || 
      (message == WM_MOUSEMOVE))
  {
    int ix, iy;
    item->GetPosition(&ix, &iy);
    RECT rect;
    GetWindowRect(hWnd, &rect);

    // Since we now have the absolute screen coords,
    // convert to panel coordinates.
    POINT point;
    point.x = rect.left;
    point.y = rect.top;
    ::ScreenToClient(panel->GetHWND(), &point);

    x += (point.x - ix);
    y += (point.y - iy);
  }

  switch (message)
  {
    case WM_KILLFOCUS:
      item->GetEventHandler()->OnKillFocus();
      break;
    case WM_SETFOCUS:
      item->GetEventHandler()->OnSetFocus();
      break;
    case WM_RBUTTONDOWN:
    {
      wxMouseEvent event(wxEVENT_TYPE_RIGHT_DOWN);

      event.shiftDown = (flags & MK_SHIFT);
      event.controlDown = (flags & MK_CONTROL);
      event.leftDown = (flags & MK_LBUTTON);
      event.middleDown = (flags & MK_MBUTTON);
      event.rightDown = (flags & MK_RBUTTON);

      event.x = (float)x; event.y = (float)y;

      wxItem *actualItem = wxFakeItemEvent(panel, item, event);
      if (actualItem)
      {
        actualItem->mswLastXPos = (int)event.x; actualItem->mswLastYPos = (int)event.y;
        actualItem->mswLastEvent = wxEVENT_TYPE_RIGHT_DOWN;
      }
      return TRUE;
    }
    case WM_RBUTTONUP:
    {
      wxMouseEvent event(wxEVENT_TYPE_RIGHT_UP);

      event.shiftDown = (flags & MK_SHIFT);
      event.controlDown = (flags & MK_CONTROL);
      event.leftDown = (flags & MK_LBUTTON);
      event.middleDown = (flags & MK_MBUTTON);
      event.rightDown = (flags & MK_RBUTTON);

      event.x = (float)x; event.y = (float)y;

      wxItem *actualItem = wxFakeItemEvent(panel, item, event);
      if (actualItem)
      {
        actualItem->mswLastXPos = (int)event.x; actualItem->mswLastYPos = (int)event.y;
        actualItem->mswLastEvent = wxEVENT_TYPE_RIGHT_UP;
      }
      return TRUE;
    }
    case WM_RBUTTONDBLCLK:
    {
      wxMouseEvent event(wxEVENT_TYPE_RIGHT_DCLICK);

      event.x = (float)x; event.y = (float)y;

      event.shiftDown = (flags & MK_SHIFT);
      event.controlDown = (flags & MK_CONTROL);
      event.leftDown = (flags & MK_LBUTTON);
      event.middleDown = (flags & MK_MBUTTON);
      event.rightDown = (flags & MK_RBUTTON);

      wxItem *actualItem = wxFakeItemEvent(panel, item, event);
      if (actualItem)
      {
        actualItem->mswLastXPos = (int)event.x; actualItem->mswLastYPos = (int)event.y;
        actualItem->mswLastEvent = wxEVENT_TYPE_RIGHT_DCLICK;
      }
      return TRUE;
    }
    case WM_MBUTTONDOWN:
    {
      wxMouseEvent event(wxEVENT_TYPE_MIDDLE_DOWN);

      event.shiftDown = (flags & MK_SHIFT);
      event.controlDown = (flags & MK_CONTROL);
      event.leftDown = (flags & MK_LBUTTON);
      event.middleDown = (flags & MK_MBUTTON);
      event.rightDown = (flags & MK_RBUTTON);

      event.x = (float)x; event.y = (float)y;

      wxItem *actualItem = wxFakeItemEvent(panel, item, event);
      if (actualItem)
      {
        actualItem->mswLastXPos = (int)event.x; actualItem->mswLastYPos = (int)event.y;
        actualItem->mswLastEvent = wxEVENT_TYPE_MIDDLE_DOWN;
      }
      return TRUE;
    }
    case WM_MBUTTONUP:
    {
      wxMouseEvent event(wxEVENT_TYPE_MIDDLE_UP);

      event.shiftDown = (flags & MK_SHIFT);
      event.controlDown = (flags & MK_CONTROL);
      event.leftDown = (flags & MK_LBUTTON);
      event.middleDown = (flags & MK_MBUTTON);
      event.rightDown = (flags & MK_RBUTTON);

      event.x = (float)x; event.y = (float)y;

      wxItem *actualItem = wxFakeItemEvent(panel, item, event);
      if (actualItem)
      {
        actualItem->mswLastXPos = (int)event.x; actualItem->mswLastYPos = (int)event.y;
        actualItem->mswLastEvent = wxEVENT_TYPE_MIDDLE_UP;
      }
      return TRUE;
    }
    case WM_MBUTTONDBLCLK:
    {
      wxMouseEvent event(wxEVENT_TYPE_MIDDLE_DCLICK);

      event.shiftDown = (flags & MK_SHIFT);
      event.controlDown = (flags & MK_CONTROL);
      event.leftDown = (flags & MK_LBUTTON);
      event.middleDown = (flags & MK_MBUTTON);
      event.rightDown = (flags & MK_RBUTTON);

      event.x = (float)x; event.y = (float)y;

      wxItem *actualItem = wxFakeItemEvent(panel, item, event);
      if (actualItem)
      {
        actualItem->mswLastXPos = (int)event.x; actualItem->mswLastYPos = (int)event.y;
        actualItem->mswLastEvent = wxEVENT_TYPE_MIDDLE_DCLICK;
      }
      return TRUE;
    }
    case WM_LBUTTONDOWN:
    {
      wxMouseEvent event(wxEVENT_TYPE_LEFT_DOWN);

      event.shiftDown = (flags & MK_SHIFT);
      event.controlDown = (flags & MK_CONTROL);
      event.leftDown = (flags & MK_LBUTTON);
      event.middleDown = (flags & MK_MBUTTON);
      event.rightDown = (flags & MK_RBUTTON);

      event.x = (float)x; event.y = (float)y;

      wxItem *actualItem = wxFakeItemEvent(panel, item, event);
      if (actualItem)
      {
        actualItem->mswLastXPos = (int)event.x; actualItem->mswLastYPos = (int)event.y;
        actualItem->mswLastEvent = wxEVENT_TYPE_LEFT_DOWN;
      }
      return TRUE;
    }
    case WM_LBUTTONUP:
    {
      wxMouseEvent event(wxEVENT_TYPE_LEFT_UP);

      event.shiftDown = (flags & MK_SHIFT);
      event.controlDown = (flags & MK_CONTROL);
      event.leftDown = (flags & MK_LBUTTON);
      event.middleDown = (flags & MK_MBUTTON);
      event.rightDown = (flags & MK_RBUTTON);

      event.x = (float)x; event.y = (float)y;

      wxItem *actualItem = wxFakeItemEvent(panel, item, event);
      if (actualItem)
      {
        actualItem->mswLastXPos = (int)event.x; actualItem->mswLastYPos = (int)event.y;
        actualItem->mswLastEvent = wxEVENT_TYPE_LEFT_UP;
      }
      return TRUE;
    }
    case WM_LBUTTONDBLCLK:
    {
      wxMouseEvent event(wxEVENT_TYPE_LEFT_DCLICK);

      event.shiftDown = (flags & MK_SHIFT);
      event.controlDown = (flags & MK_CONTROL);
      event.leftDown = (flags & MK_LBUTTON);
      event.middleDown = (flags & MK_MBUTTON);
      event.rightDown = (flags & MK_RBUTTON);

      event.x = (float)x; event.y = (float)y;

      wxItem *actualItem = wxFakeItemEvent(panel, item, event);
      if (actualItem)
      {
        actualItem->mswLastXPos = (int)event.x; actualItem->mswLastYPos = (int)event.y;
        actualItem->mswLastEvent = wxEVENT_TYPE_LEFT_DCLICK;
      }
      return TRUE;
    }
    case WM_MOUSEMOVE:
    {
      wxMouseEvent event(wxEVENT_TYPE_MOTION);

      event.shiftDown = (flags & MK_SHIFT);
      event.controlDown = (flags & MK_CONTROL);
      event.leftDown = (flags & MK_LBUTTON);
      event.middleDown = (flags & MK_MBUTTON);
      event.rightDown = (flags & MK_RBUTTON);

      event.x = (float)x; event.y = (float)y;

      if ((item->mswLastEvent == wxEVENT_TYPE_RIGHT_DOWN || item->mswLastEvent == wxEVENT_TYPE_LEFT_DOWN ||
           item->mswLastEvent == wxEVENT_TYPE_MIDDLE_DOWN) &&
          (item->mswLastXPos == event.x && item->mswLastYPos == event.y))
      {
        item->mswLastXPos = (int)x; item->mswLastYPos = (int)y;
        item->mswLastEvent = wxEVENT_TYPE_MOTION;
        return TRUE;
      }
      
      wxItem *actualItem = wxFakeItemEvent(panel, item, event);
      if (actualItem)
      {
        actualItem->mswLastXPos = (int)event.x; actualItem->mswLastYPos = (int)event.y;
        actualItem->mswLastEvent = wxEVENT_TYPE_MOTION;
      }
      return TRUE;
    }
    // Ensure that static items get messages
    case WM_NCHITTEST:
    {
      return (long)HTCLIENT;
    }
    default:
    {
      return CallWindowProc(CASTWNDPROC item->oldWndProc, hWnd, message, wParam, lParam);
    }
  }
  return CallWindowProc(CASTWNDPROC item->oldWndProc, hWnd, message, wParam, lParam);
}

wxWindow *wxFindControlFromHandle(HWND hWnd)
{
  if (!wxControlHandleList)
    return NULL;
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxNode *node = wxControlHandleList->Find((long)hWnd);
  if (!node)
    return NULL;
  return (wxWindow *)node->Data();
#else
  return (wxWindow *)wxControlHandleList->Find((long)hWnd);
#endif
}

void wxAddControlHandle(HWND hWnd, wxWindow *item)
{
  if (!wxControlHandleList) {
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
    wxControlHandleList = new wxList(wxKEY_INTEGER);
#else
    wxControlHandleList = new wxNonlockingHashTable;
#endif
  }
  wxControlHandleList->Append((long)hWnd, item);
}


/*
 * Allocates control IDs within the appropriate range
 */


int NewControlId(void)
{
  static int controlId = 0;
  controlId ++;
  return controlId;
}
