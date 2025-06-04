/*
 * File:        wb_item.cc
 * Purpose:     Panel items implementation: base (platform-independent) code
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	March 1995
 * RCS_ID:      $Id: wb_item.cc,v 1.3 1994/08/14 21:34:01 edz Exp $
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

#ifdef __GNUG__
#pragma implementation

#pragma implementation "wb_item.h"
#pragma implementation "wb_lbox.h"
#pragma implementation "wb_rbox.h"
#pragma implementation "wb_buttn.h"
#pragma implementation "wb_choic.h"
#pragma implementation "wb_check.h"
#pragma implementation "wb_messg.h"
#pragma implementation "wb_slidr.h"
#pragma implementation "wb_menu.h"
#pragma implementation "wb_mnuit.h"
#pragma implementation "wb_txt.h"
#pragma implementation "wb_mtxt.h"
#pragma implementation "wb_menu.h"
#pragma implementation "wb_group.h"
#pragma implementation "wb_gauge.h"
#pragma implementation "wb_combo.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_defs.h"
#include "wx_item.h"
#include "wx_lbox.h"
#include "wx_rbox.h"
#include "wx_buttn.h"
#include "wx_choic.h"
#include "wx_check.h"
#include "wx_messg.h"
#include "wx_slidr.h"
#include "wx_group.h"
#include "wx_menu.h"
#include "wx_txt.h"
#include "wx_mtxt.h"
#include "wx_utils.h"
#endif

#if USE_GAUGE
#include "wx_gauge.h"
#endif
#if USE_COMBOBOX
#include "wx_combo.h"
#endif

#include "wx_stdev.h"
#include "math.h"
#include "stdlib.h"

/* When implementing a new item, be sure to:

 * - add the item to the parent panel
 * - set window_parent to the parent
 * - NULL any extra child window pointers not created for this item
 *   (e.g. label control that wasn't needed)
 * - delete any extra child windows in the destructor (e.g. label control)
 * - implement GetSize and SetSize
 * - to find panel position if coordinates are (-1, -1), use GetPosition
 * - call AdvanceCursor after creation, for panel layout mechanism.
 *
 */

/*
   Motif notes

   A panel is a form.
   Each item is created on a RowColumn or Form of its own, to allow a label to
   be positioned. wxListBox and wxMultiText have forms, all the others have RowColumns.
   This is to allow labels to be positioned to the top left (can't do it with a
   RowColumn as far as I know).
   AttachWidget positions widgets relative to one another (left->right, top->bottom)
   unless the x, y coordinates are given (more than -1).
 */

int wxbItem::dragOffsetX = 0;
int wxbItem::dragOffsetY = 0;

// Item members
wxbItem::wxbItem (void)
{
  WXSET_TYPE(wxItem, wxTYPE_ITEM)

  handleSize = 6;
  handleMargin = 1;
  isSelected = FALSE;
  dragOffsetX = 0;
  dragOffsetY = 0;
  backColour = *wxWHITE;
  labelColour = *wxBLACK;
  buttonColour = *wxBLACK;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbItem::~wxbItem (void)
{
  wxPanel *parent = (wxPanel *) GetParent ();
  if (parent)
  {
    // parent is not always a wxPanel: can be a wxMenu...
    if (parent->IsKindOf(CLASSINFO(wxPanel)))
    {
      if (parent->defaultItem == this)
        parent->defaultItem = NULL;
    }
  }
}

void wxbItem::SetClientSize (int width, int height)
{
  SetSize (-1, -1, width, height);
}

int wxbItem::GetLabelPosition (void)
{
  return labelPosition;
}

void wxbItem::SetLabelPosition (int pos)
{
  labelPosition = pos;
  if (pos == wxHORIZONTAL)
  {
    if (windowStyle & wxVERTICAL_LABEL)
      windowStyle -= wxVERTICAL_LABEL;
    windowStyle |= wxHORIZONTAL_LABEL;
  }
  else if (pos == wxVERTICAL)
  {
    if (windowStyle & wxHORIZONTAL_LABEL)
      windowStyle -= wxHORIZONTAL_LABEL;
    windowStyle |= wxVERTICAL_LABEL;
  }
}

// Helper function that sets the
// appropriate label position depending on windowStyle flags
// and parent label position setting
void wxbItem::SetAppropriateLabelPosition(void)
{
  wxPanel *panel = (wxPanel *)GetParent();
  if (!panel)
    return;
    
  if (windowStyle & wxVERTICAL_LABEL)
    SetLabelPosition(wxVERTICAL);
  else if (windowStyle & wxHORIZONTAL_LABEL)
    SetLabelPosition(wxHORIZONTAL);
  else
    SetLabelPosition(panel->GetLabelPosition());
}

void wxbItem::Centre (int direction)
{
  int x, y, width, height, panel_width, panel_height, new_x, new_y;

  wxPanel *panel = (wxPanel *) GetParent ();
  if (!panel)
    return;

  panel->GetClientSize (&panel_width, &panel_height);
  GetSize (&width, &height);
  GetPosition (&x, &y);

  new_x = x;
  new_y = y;

  if (direction & wxHORIZONTAL)
    new_x = (int) ((panel_width - width) / 2);

  if (direction & wxVERTICAL)
    new_y = (int) ((panel_height - height) / 2);

  SetSize (new_x, new_y, width, height);
  int temp_x, temp_y;
  GetPosition (&temp_x, &temp_y);
  GetPosition (&temp_x, &temp_y);
}

/*
 * Manipulation and drawing of items in Edit Mode
 */
 
void wxbItem::SelectItem(Bool select)
{
  isSelected = select;
}

// Returns TRUE or FALSE
Bool wxbItem::HitTest(int x, int y)
{
  int xpos, ypos, width, height;
  GetPosition(&xpos, &ypos);
  GetSize(&width, &height);

  return ((x >= xpos) && (x <= (xpos + width)) && (y >= ypos) && (y <= (ypos + height)));
}

// Calculate position of the 8 handles
void wxbItem::CalcSelectionHandles(int *hx, int *hy)
{
  int xpos, ypos, width, height;
  GetPosition(&xpos, &ypos);
  GetSize(&width, &height);
  int middleX = (int)(xpos + (width/2));
  int middleY = (int)(ypos + (height/2));

  // Start from top middle, clockwise.
/*
  7      0      1

  6             2

  5      4      3
*/

  hx[0] = (int)(middleX - (handleSize/2));
  hy[0] = ypos - handleSize - handleMargin;

  hx[1] = xpos + width + handleMargin;
  hy[1] = ypos - handleSize - handleMargin;

  hx[2] = xpos + width + handleMargin;
  hy[2] = (int)(middleY - (handleSize/2));

  hx[3] = xpos + width + handleMargin;
  hy[3] = ypos + height + handleMargin;

  hx[4] = (int)(middleX - (handleSize/2));
  hy[4] = ypos + height + handleMargin;

  hx[5] = xpos - handleSize - handleMargin;
  hy[5] = ypos + height + handleMargin;

  hx[6] = xpos - handleSize - handleMargin;
  hy[6] = (int)(middleY - (handleSize/2));

  hx[7] = xpos - handleSize - handleMargin;
  hy[7] = ypos - handleSize - handleMargin;
}

// Returns 0 (no hit), 1 - 8 for which selection handle
// (clockwise from top middle)
int wxbItem::SelectionHandleHitTest(int x, int y)
{
  // Handle positions
  int hx[8];
  int hy[8];
  CalcSelectionHandles(hx, hy);

  int i;
  for (i = 0; i < 8; i++)
  {
    if ((x >= hx[i]) && (x <= (hx[i] + handleSize)) && (y >= hy[i]) && (y <= (hy[i] + handleSize)))
      return (i + 1);
  }
  return 0;
}

void wxbItem::DrawSelectionHandles(wxPanelDC *dc, Bool WXUNUSED(erase))
{
//  wxPanel *panel = (wxPanel *)GetParent();
  
  dc->SetOptimization(FALSE);

  dc->SetLogicalFunction(wxCOPY);
  dc->SetPen(wxBLACK_PEN);
  dc->SetBrush(wxBLACK_BRUSH);

  dc->SetOptimization(TRUE);

  // Handle positions
  int hx[8];
  int hy[8];
  CalcSelectionHandles(hx, hy);

  int i;
  for (i = 0; i < 8; i++)
  {
    dc->DrawRectangle((float)hx[i], (float)hy[i], (float)handleSize, (float)handleSize);
  }
}

void wxbItem::DrawBoundingBox(wxPanelDC *dc, int x, int y, int w, int h)
{
  dc->DrawRectangle((float)x, (float)y, (float)w, (float)h);
}

// If selectionHandle is zero, not dragging the selection handle.
void wxbItem::OnDragBegin(int x, int y, int WXUNUSED(keys), wxPanelDC *dc, int selectionHandle)
{
  int xpos, ypos, width, height;
  GetPosition(&xpos, &ypos);
  GetSize(&width, &height);

  dc->BeginDrawing();

//  dc->DestroyClippingRegion();

  wxPanel *panel = (wxPanel *)GetParent();

  // Erase selection handles
//  DrawSelectionHandles(dc, TRUE);

  if (panel->GetSlowDrag())
  {
    dc->SetOptimization(FALSE);

    dc->SetLogicalFunction(wxXOR);

    dc->SetPen(wxBLACK_DASHED_PEN);
    dc->SetBrush(wxTRANSPARENT_BRUSH);

    dc->SetOptimization(TRUE);
  }

  if (selectionHandle > 0)
  {
    GetParent()->Refresh();

    if (panel->GetSlowDrag())
    {
      DrawBoundingBox(dc, xpos, ypos, width, height);
    }
  }
  else
  {
    GetParent()->Refresh();

    dragOffsetX = (x - xpos);
    dragOffsetY = (y - ypos);

    if (panel->GetSlowDrag())
    {
      DrawBoundingBox(dc, xpos, ypos, width, height);

      // Also draw bounding boxes for other selected items
      wxNode *node = panel->GetChildren()->First();
      while (node)
      {
        wxWindow *win = (wxWindow *)node->Data();
        if (win->IsKindOf(CLASSINFO(wxItem)))
        {
          wxItem *item = (wxItem *)win;
          if ((item != this) && item->IsSelected())
          {
            int x1, y1, w1, h1;
            item->GetPosition(&x1, &y1);
            item->GetSize(&w1, &h1);
            item->DrawBoundingBox(dc, x1, y1, w1, h1);
          }
        }
        node = node->Next();
      }
    }
  }
  dc->EndDrawing();
}

void wxbItem::OnDragContinue(Bool paintIt, int x, int y, int WXUNUSED(keys), wxPanelDC *dc, int selectionHandle)
{
  wxPanel *panel = (wxPanel *)GetParent();

  if (!panel->GetSlowDrag() && !paintIt)
    return;
    
  int xpos, ypos, width, height;
  GetPosition(&xpos, &ypos);
  GetSize(&width, &height);
  
  if (selectionHandle > 0)
  {
/*
  8      1      2

  7             3

  6      5      4
*/

    int x1, y1, width1, height1;
  
    switch (selectionHandle)
    {
      case 1:
        x1 = xpos;
        y1 = y;
        width1 = width;
        height1 = (ypos + height) - y;
        break;
      case 5:
        x1 = xpos;
        y1 = ypos;
        width1 = width;
        height1 = (y - ypos);
        break;
      case 3:
        x1 = xpos;
        y1 = ypos;
        width1 = (x - xpos);
        height1 = height;
        break;
      case 7:
        x1 = x;
        y1 = ypos;
        width1 = (xpos + width) - x;
        height1 = height;
        break;
      case 2:
        x1 = xpos;
        y1 = y;
        width1 = (x - xpos);
        height1 = (ypos + height) - y;
        break;
      case 4:
        x1 = xpos;
        y1 = ypos;
        width1 = (x - xpos);
        height1 = (y - ypos);
        break;
      case 6:
        x1 = x;
        y1 = ypos;
        width1 = (xpos + width) - x;
        height1 = y - ypos;
        break;
      case 8:
        x1 = x;
        y1 = y;
        width1 = (xpos + width) - x;
        height1 = (ypos + height) - y;
        break;
    }
    if (panel->GetSlowDrag())
    {
      dc->BeginDrawing();
      
      dc->SetLogicalFunction(wxXOR);
      dc->SetPen(wxBLACK_DASHED_PEN);
      dc->SetBrush(wxTRANSPARENT_BRUSH);

      DrawBoundingBox(dc, x1, y1, width1, height1);

      dc->EndDrawing();
    }
    else
      SetSize(x1, y1, width1, height1);
  }
  else
  {
//    SetSize((int)(x - dragOffsetX), (int)(y - dragOffsetY), width, height);
    if (panel->GetSlowDrag())
    {
      dc->BeginDrawing();

      dc->SetLogicalFunction(wxXOR);
      dc->SetPen(wxBLACK_DASHED_PEN);
      dc->SetBrush(wxTRANSPARENT_BRUSH);

      DrawBoundingBox(dc, (int)(x - dragOffsetX), (int)(y - dragOffsetY), width, height);

      // Also draw bounding boxes for other selected items
      wxNode *node = panel->GetChildren()->First();
      while (node)
      {
        wxWindow *win = (wxWindow *)node->Data();
        if (win->IsKindOf(CLASSINFO(wxItem)))
        {
          wxItem *item = (wxItem *)win;
          if ((item != this) && item->IsSelected())
          {
            int x1, y1, w1, h1;
            item->GetPosition(&x1, &y1);
            item->GetSize(&w1, &h1);
            int x2 = (int)(x1 + (x - dragOffsetX) - xpos);
            int y2 = (int)(y1 + (y - dragOffsetY) - ypos);
            item->DrawBoundingBox(dc, x2, y2, w1, h1);
          }
        }
        node = node->Next();
      }

      dc->EndDrawing();
    }
    else
      Move((int)(x - dragOffsetX), (int)(y - dragOffsetY));
  }
}

void wxbItem::OnDragEnd(int x, int y, int WXUNUSED(keys), wxPanelDC *dc, int selectionHandle)
{
  wxPanel *panel = (wxPanel *)GetParent();
  dc->BeginDrawing();

  int xpos, ypos, width, height;
  GetPosition(&xpos, &ypos);
  GetSize(&width, &height);

  if (selectionHandle > 0)
  {
    if (panel->GetSlowDrag())
    {

    int x1, y1, width1, height1;
  
    switch (selectionHandle)
    {
      case 1:
        x1 = xpos;
        y1 = y;
        width1 = width;
        height1 = (ypos + height) - y;
        break;
      case 5:
        x1 = xpos;
        y1 = ypos;
        width1 = width;
        height1 = (y - ypos);
        break;
      case 3:
        x1 = xpos;
        y1 = ypos;
        width1 = (x - xpos);
        height1 = height;
        break;
      case 7:
        x1 = x;
        y1 = ypos;
        width1 = (xpos + width) - x;
        height1 = height;
        break;
      case 2:
        x1 = xpos;
        y1 = y;
        width1 = (x - xpos);
        height1 = (ypos + height) - y;
        break;
      case 4:
        x1 = xpos;
        y1 = ypos;
        width1 = (x - xpos);
        height1 = (y - ypos);
        break;
      case 6:
        x1 = x;
        y1 = ypos;
        width1 = (xpos + width) - x;
        height1 = y - ypos;
        break;
      case 8:
        x1 = x;
        y1 = y;
        width1 = (xpos + width) - x;
        height1 = (ypos + height) - y;
        break;
    }
      SetSize(x1, y1, width1, height1);
    }
  }
  else
  {
    Move((int)(x - dragOffsetX), (int)(y - dragOffsetY));
    GetEventHandler()->OnMove((int)(x - dragOffsetX), (int)(y - dragOffsetY));

    // Also move other selected items
    wxNode *node = panel->GetChildren()->First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (win->IsKindOf(CLASSINFO(wxItem)))
      {
        wxItem *item = (wxItem *)win;
        if ((item != this) && item->IsSelected())
        {
          int x1, y1;
          item->GetPosition(&x1, &y1);
          int x2 = (int)(x1 + (x - dragOffsetX) - xpos);
          int y2 = (int)(y1 + (y - dragOffsetY) - ypos);
          item->Move(x2, y2);
          item->OnMove(x2, y2);
          item->DrawSelectionHandles(dc);
        }
      }
      node = node->Next();
    }
  }

  dc->SetOptimization(FALSE);

  dc->SetLogicalFunction(wxCOPY);
  dc->SetPen(wxBLACK_PEN);
  dc->SetBrush(wxBLACK_BRUSH);

  dc->SetOptimization(TRUE);

  // Force it to repaint the selection handles (if any)
  // since the panel thinks we're still within a drag and
  // won't paint the handles.
  if (IsSelected())
    DrawSelectionHandles(dc);

  dc->EndDrawing();

  GetParent()->Refresh();
}

// These functions call OnItemEvent, OnItemMove and OnItemSize
// by default.
void wxbItem::OnEvent(wxMouseEvent& event)
{
  if ((event.eventType == wxEVENT_TYPE_LEFT_DCLICK) ||
      (event.eventType == wxEVENT_TYPE_RIGHT_DCLICK))
    return;
    
  wxWindow *theParent = this->GetParent();
  ((wxPanel *)theParent)->GetEventHandler()->OnItemEvent((wxItem *)this, event);
}

void wxbItem::OnMove(int x, int y)
{
  wxWindow *theParent = GetParent();
  ((wxPanel *)theParent)->GetEventHandler()->OnItemMove((wxItem *)this, x, y);
}

void wxbItem::OnSize(int w, int h)
{
  wxWindow *theParent = GetParent();
  ((wxPanel *)theParent)->GetEventHandler()->OnItemSize((wxItem *)this, w, h);
}

void wxbItem::OnSelect(Bool select)
{
  wxWindow *theParent = GetParent();
  ((wxPanel *)theParent)->GetEventHandler()->OnItemSelect((wxItem *)this, select);
}

void wxbItem::OnLeftClick(int x, int y, int keys)
{
  wxWindow *theParent = GetParent();
  ((wxPanel *)theParent)->GetEventHandler()->OnItemLeftClick((wxItem *)this, x, y, keys);
}

void wxbItem::OnRightClick(int x, int y, int keys)
{
  wxWindow *theParent = GetParent();
  ((wxPanel *)theParent)->GetEventHandler()->OnItemRightClick((wxItem *)this, x, y, keys);
}


/*
 * Button
 */
 
wxbButton::wxbButton (void)
{
  WXSET_TYPE(wxButton, wxTYPE_BUTTON)
  
  window_parent = NULL;
  labelPosition = wxHORIZONTAL;
  buttonFont = NULL;
  labelFont = NULL;
  isBitmap = FALSE;
}

wxbButton::wxbButton (wxPanel * panel, wxFunction WXUNUSED(Function), Const char *WXUNUSED(label),
	   int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height), long style,
           Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxButton, wxTYPE_BUTTON)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = wxHORIZONTAL;
  buttonFont = NULL;
  labelFont = NULL;
  isBitmap = FALSE;
}

wxbButton::wxbButton (wxPanel * panel, wxFunction WXUNUSED(Function), wxBitmap * WXUNUSED(bitmap),
	   int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height), long style,
           Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxButton, wxTYPE_BUTTON)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = wxHORIZONTAL;
  buttonFont = NULL;
  labelFont = NULL;
  isBitmap = FALSE;
}

wxbButton::~wxbButton (void)
{
}

void wxbButton::Command (wxCommandEvent & event)
{
  ProcessCommand (event);
}

void wxbButton::ProcessCommand (wxCommandEvent & event)
{
  if (wxNotifyEvent (event, TRUE))
  {
    return;
  }

  wxFunction fun = callback;
  if (fun)
    {
      (void) (*(fun)) (*this, event);
    }
    else
    {
      GetEventHandler()->OnCommand(*this, event);
    }

  wxNotifyEvent (event, FALSE);
}

/*
 * Menu
 */

// Construct a menu with optional title (then use append)
wxbMenu::wxbMenu (char *Title, wxFunction WXUNUSED(func))
{
  WXSET_TYPE(wxMenu, wxTYPE_MENU)
  
  no_items = 0;
  menu_bar = NULL;
  WXGC_IGNORE(menu_bar);
  WXGC_IGNORE(top_level_menu);
  if (Title)
    title = copystring (Title);
  else
    title = NULL;
}

// The wxWindow destructor will take care of deleting the submenus.
wxbMenu::~wxbMenu (void)
{
  if (title)
    delete[]title;
}

// Finds the item id matching the given string, -1 if not found.
int wxbMenu::FindItem (char *itemString)
{
  char buf1[200];
  char buf2[200];
  wxStripMenuCodes (itemString, buf1);

  for (wxNode * node = menuItems.First (); node; node = node->Next ())
    {
      wxMenuItem *item = (wxMenuItem *) node->Data ();
      if (item->subMenu)
	{
	  int ans = item->subMenu->FindItem (itemString);
	  if (ans > -1)
	    return ans;
	}
      if ((item->itemId > -1) && item->itemName)
	{
	  wxStripMenuCodes (item->itemName, buf2);
	  if (strcmp (buf1, buf2) == 0)
	    return item->itemId;
	}
    }

  return -1;
}

wxMenuItem *wxbMenu::FindItemForId (int itemId, wxMenu ** itemMenu)
{
  if (itemMenu)
    *itemMenu = NULL;
  for (wxNode * node = menuItems.First (); node; node = node->Next ())
    {
      wxMenuItem *item = (wxMenuItem *) node->Data ();

      if (item->itemId == itemId)
	{
	  if (itemMenu)
	    *itemMenu = (wxMenu *) this;
	  return item;
	}

      if (item->subMenu)
	{
	  wxMenuItem *ans = item->subMenu->FindItemForId (itemId, itemMenu);
	  if (ans)
	    return ans;
	}
    }

  if (itemMenu)
    *itemMenu = NULL;
  return NULL;
}

void wxbMenu::SetHelpString (int itemId, char *helpString)
{
  wxMenuItem *item = FindItemForId (itemId);
  if (item)
    {
      if (item->helpString)
	delete[]item->helpString;
      item->helpString = copystring (helpString);
    }
}

char *wxbMenu::GetHelpString (int itemId)
{
  wxMenuItem *item = FindItemForId (itemId);
  if (item)
    return item->helpString;
  else
    return NULL;
}

void wxbMenu::ProcessCommand (wxCommandEvent & event)
{
  if (wxNotifyEvent (event, TRUE))
    return;

  wxFunction fun = callback;
  if (fun)
    {
      (void) (*(fun)) (*this, event);
    }
    else
    {
      GetEventHandler()->OnCommand(*this, event);
    }

  wxNotifyEvent (event, FALSE);
}

/*
 * Menu Bar
 */

wxbMenuBar::wxbMenuBar (void)
{
  WXSET_TYPE(wxMenuBar, wxTYPE_MENU_BAR)
  
  n = 0;
  menus = NULL;
  titles = NULL;
  menu_bar_frame = NULL;
  WXGC_IGNORE(menu_bar_frame);
}

wxbMenuBar::wxbMenuBar (int N, wxMenu * Menus[], char *Titles[])
{
  WXSET_TYPE(wxMenuBar, wxTYPE_MENU_BAR)
  
  n = N;
  menus = Menus;
  titles = Titles;
  menu_bar_frame = NULL;
  int i;
  for (i = 0; i < N; i++)
    menus[i]->menu_bar = (wxMenuBar *) this;
  WXGC_IGNORE(menu_bar_frame);
}

wxbMenuBar::~wxbMenuBar (void)
{
}

void wxbMenuBar::Append (wxMenu * menu, char *title)
{
  /* MATTHEW: [6] */
  if (!OnAppend(menu, title))
	 return;

  n++;
  wxMenu **new_menus = new wxMenu *[n];
  char **new_titles = new char *[n];
  int i;

  for (i = 0; i < n - 1; i++)
	 {
		new_menus[i] = menus[i];
		menus[i] = NULL;
		new_titles[i] = titles[i];
		titles[i] = NULL;
	 }
  if (menus)
	 {
		delete[]menus;
		delete[]titles;
	 }
  menus = new_menus;
  titles = new_titles;

  menus[n - 1] = menu;
  titles[n - 1] = copystring (title);

  menu->menu_bar = (wxMenuBar *) this;
  menu->SetParent(this);
}

/* MATTHEW: [6] */
void wxbMenuBar::Delete(wxMenu * menu, int i)
{
  int j;

  if (menu) {
	 for (i = 0; i < n; i++) {
		if (menus[i] == menu)
	break;
	 }
	 if (i >= n)
		return;
  } else {
	 if (i < 0 || i >= n)
		return;
	 menu = menus[i];
  }

  if (!OnDelete(menu, i))
	 return;

  menu->SetParent(NULL);

  --n;
  for (j = i; j < n; j++) {
	 menus[j] = menus[j + 1];
	 titles[j] = titles[j + 1];
  }
}

// Find the menu menuString, item itemString, and return the item id.
// Returns -1 if none found.
int wxbMenuBar::FindMenuItem (char *menuString, char *itemString)
{
  char buf1[200];
  char buf2[200];
  wxStripMenuCodes (menuString, buf1);
  int i;
  for (i = 0; i < n; i++)
    {
      wxStripMenuCodes (titles[i], buf2);
      if (strcmp (buf1, buf2) == 0)
	return menus[i]->FindItem (itemString);
    }
  return -1;
}

wxMenuItem *wxbMenuBar::FindItemForId (int Id, wxMenu ** itemMenu)
{
  if (itemMenu)
    *itemMenu = NULL;

  wxMenuItem *item = NULL;
  int i;
  for (i = 0; i < n; i++)
    if ((item = menus[i]->FindItemForId (Id, itemMenu)))
      return item;
  return NULL;
}

void wxbMenuBar::SetHelpString (int Id, char *helpString)
{
  int i;
  for (i = 0; i < n; i++)
    {
      if (menus[i]->FindItemForId (Id))
	{
	  menus[i]->SetHelpString (Id, helpString);
	  return;
	}
    }
}

char *wxbMenuBar::GetHelpString (int Id)
{
  int i;
  for (i = 0; i < n; i++)
    {
      if (menus[i]->FindItemForId (Id))
	return menus[i]->GetHelpString (Id);
    }
  return NULL;
}

/*
 * Single check box item
 */
 
wxbCheckBox::wxbCheckBox (void)
{
  WXSET_TYPE(wxCheckBox, wxTYPE_CHECK_BOX)
  
  buttonFont = NULL;
  labelFont = NULL;
}

wxbCheckBox::wxbCheckBox (wxPanel * panel, wxFunction WXUNUSED(func), Const char *WXUNUSED(Title),
	     int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height), long style,
             Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxCheckBox, wxTYPE_CHECK_BOX)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbCheckBox::wxbCheckBox (wxPanel * panel, wxFunction WXUNUSED(func), wxBitmap * WXUNUSED(bitmap),
	     int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height), long style,
             Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxCheckBox, wxTYPE_CHECK_BOX)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbCheckBox::~wxbCheckBox (void)
{
}

void wxbCheckBox::Command (wxCommandEvent & event)
{
  SetValue (event.commandInt);
  ProcessCommand (event);
}

void wxbCheckBox::ProcessCommand (wxCommandEvent & event)
{
  if (wxNotifyEvent (event, TRUE))
    return;

  wxFunction fun = callback;
  if (fun)
    {
      (void) (*(fun)) (*this, event);
    }
    else
    {
      GetEventHandler()->OnCommand(*this, event);
    }

  wxNotifyEvent (event, FALSE);
}

/*
 * Choice
 */
 
wxbChoice::wxbChoice (void)
{
  WXSET_TYPE(wxChoice, wxTYPE_CHOICE)
  
  buttonFont = NULL;
  labelFont = NULL;
}

wxbChoice::wxbChoice (wxPanel * panel, wxFunction WXUNUSED(func), Const char *WXUNUSED(Title),
	   int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height), int N,
           char **WXUNUSED(Choices), long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxChoice, wxTYPE_CHOICE)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = panel->label_position;
  no_strings = N;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbChoice::~wxbChoice (void)
{
}

char *wxbChoice::GetStringSelection (void)
{
  int sel = GetSelection ();
  if (sel > -1)
    return this->GetString (sel);
  else
    return NULL;
}

Bool wxbChoice::SetStringSelection (char *s)
{
  int sel = FindString (s);
  if (sel > -1)
    {
      SetSelection (sel);
      return TRUE;
    }
  else
    return FALSE;
}

void wxbChoice::Command (wxCommandEvent & event)
{
  SetSelection (event.commandInt);
  ProcessCommand (event);
}

void wxbChoice::ProcessCommand (wxCommandEvent & event)
{
  if (wxNotifyEvent (event, TRUE))
    return;

  wxFunction fun = callback;
  if (fun)
    {
      (void) (*(fun)) (*this, event);
    }
    else
    {
      GetEventHandler()->OnCommand(*this, event);
    }

  wxNotifyEvent (event, FALSE);
}

/*
 * Combobox
 */

#if USE_COMBOBOX

wxbComboBox::wxbComboBox (void)
{
  WXSET_TYPE(wxComboBox, wxTYPE_COMBO_BOX)
  
  buttonFont = NULL;
  labelFont = NULL;
}

wxbComboBox::wxbComboBox (wxPanel * panel, wxFunction WXUNUSED(func), Const char *WXUNUSED(Title), Constdata char *WXUNUSED(value),
	   int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height), int N,
           char **WXUNUSED(Choices), long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxChoice, wxTYPE_CHOICE)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = panel->label_position;
  no_strings = N;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbComboBox::~wxbComboBox (void)
{
}

char *wxbComboBox::GetStringSelection (void)
{
  int sel = GetSelection ();
  if (sel > -1)
    return this->GetString (sel);
  else
    return NULL;
}

Bool wxbComboBox::SetStringSelection (char *s)
{
  int sel = FindString (s);
  if (sel > -1)
    {
      SetSelection (sel);
      return TRUE;
    }
  else
    return FALSE;
}

void wxbComboBox::Command (wxCommandEvent & event)
{
  SetSelection (event.commandInt);
  ProcessCommand (event);
}

void wxbComboBox::ProcessCommand (wxCommandEvent & event)
{
  if (wxNotifyEvent (event, TRUE))
    return;

  wxFunction fun = callback;
  if (fun)
    {
      (void) (*(fun)) (*this, event);
    }
    else
    {
      GetEventHandler()->OnCommand(*this, event);
    }

  wxNotifyEvent (event, FALSE);
}

#endif

/*
 * Listbox
 */
 
wxbListBox::wxbListBox (void)
{
  WXSET_TYPE(wxListBox, wxTYPE_LIST_BOX)
  
  selected = -1;
  selections = 0;
  no_items = 0;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbListBox::wxbListBox (wxPanel * panel, wxFunction WXUNUSED(func),
	    Const char *WXUNUSED(Title), Bool Multiple,
	    int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height),
	    int WXUNUSED(N), char **WXUNUSED(Choices), long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxListBox, wxTYPE_LIST_BOX)
  
  windowStyle = style;
  selected = -1;
  selections = 0;
  multiple = Multiple;
  window_parent = panel;
  no_items = 0;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbListBox::~wxbListBox (void)
{
}

int wxbListBox::Number (void)
{
  return no_items;
}

// For single selection items only
char *wxbListBox::GetStringSelection (void)
{
  int sel = GetSelection ();
  if (sel > -1)
    return this->GetString (sel);
  else
    return NULL;
}

Bool wxbListBox::SetStringSelection (char *s)
{
  int sel = FindString (s);
  if (sel > -1)
    {
      SetSelection (sel);
      return TRUE;
    }
  else
    return FALSE;
}

// Is this the right thing? Won't setselection generate a command
// event too? No! It'll just generate a setselection event.
// But we still can't have this being called whenever a real command
// is generated, because it sets the selection, which will already
// have been done! (Unless we have an optional argument for calling
// by the actual window system, or a separate function, ProcessCommand)
void wxbListBox::Command (wxCommandEvent & event)
{
  if (event.extraLong)
    SetSelection (event.commandInt);
  else
    {
      Deselect (event.commandInt);
      return;
    }
  ProcessCommand (event);
}

void wxbListBox::ProcessCommand (wxCommandEvent & event)
{
  if (wxNotifyEvent (event, TRUE))
    return;

  wxFunction fun = callback;

  if (fun)
    {
      (void) (*(fun)) (*this, event);
    }
    else
    {
      GetEventHandler()->OnCommand(*this, event);
    }


  wxNotifyEvent (event, FALSE);
}

/*
 * Radiobox item
 */
 
wxbRadioBox::wxbRadioBox (void)
{
  WXSET_TYPE(wxRadioBox, wxTYPE_RADIO_BOX)
  
  selected = -1;
  no_items = 0;
  noRowsOrCols = 0;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbRadioBox::wxbRadioBox (wxPanel * panel, wxFunction WXUNUSED(func),
	     Const char *WXUNUSED(Title),
	     int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height),
	     int WXUNUSED(N), char **WXUNUSED(Choices),
	     int WXUNUSED(majorDim), long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxRadioBox, wxTYPE_RADIO_BOX)
  
  windowStyle = style;
  selected = -1;
  window_parent = panel;
  no_items = 0;
  noRowsOrCols = 0;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbRadioBox::wxbRadioBox (wxPanel * panel, wxFunction WXUNUSED(func),
	     Const char *WXUNUSED(Title),
	     int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height),
	     int WXUNUSED(N), wxBitmap ** WXUNUSED(Choices),
	     int WXUNUSED(majorDim), long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxRadioBox, wxTYPE_RADIO_BOX)
  
  windowStyle = style;
  selected = -1;
  window_parent = panel;
  no_items = 0;
  noRowsOrCols = 0;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbRadioBox::~wxbRadioBox (void)
{
}

int wxbRadioBox::Number (void)
{
  return no_items;
}

// For single selection items only
char *wxbRadioBox::GetStringSelection (void)
{
  int sel = GetSelection ();
  if (sel > -1)
    return this->GetString (sel);
  else
    return NULL;
}

Bool wxbRadioBox::SetStringSelection (char *s)
{
  int sel = FindString (s);
  if (sel > -1)
    {
      SetSelection (sel);
      return TRUE;
    }
  else
    return FALSE;
}

void wxbRadioBox::Command (wxCommandEvent & event)
{
  SetSelection (event.commandInt);
  ProcessCommand (event);
}

void wxbRadioBox::ProcessCommand (wxCommandEvent & event)
{
  if (wxNotifyEvent (event, TRUE))
    return;

  wxFunction fun = callback;

  if (fun)
    {
      (void) (*(fun)) (*this, event);
    }
    else
    {
      GetEventHandler()->OnCommand(*this, event);
    }

  wxNotifyEvent (event, FALSE);
}

#if USE_RADIOBUTTON

/*
 * Radio button item
 */
 
wxbRadioButton::wxbRadioButton (void)
{
  WXSET_TYPE(wxRadioButton, wxTYPE_RADIO_BUTTON)
  buttonFont = NULL;
  labelFont = NULL;
}

wxbRadioButton::wxbRadioButton (wxPanel * panel, wxFunction WXUNUSED(func),
	     Const char *WXUNUSED(label), Bool WXUNUSED(value),
	     int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height),
	     long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxRadioButton, wxTYPE_RADIO_BUTTON)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbRadioButton::wxbRadioButton (wxPanel * panel, wxFunction WXUNUSED(func),
	     wxBitmap *WXUNUSED(label), Bool WXUNUSED(value),
	     int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height),
	     long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxRadioButton, wxTYPE_RADIO_BUTTON)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbRadioButton::~wxbRadioButton (void)
{
}

void wxbRadioButton::Command (wxCommandEvent & event)
{
  SetValue (event.commandInt);
  ProcessCommand (event);
}

void wxbRadioButton::ProcessCommand (wxCommandEvent & event)
{
  if (wxNotifyEvent (event, TRUE))
    return;

  wxFunction fun = callback;

  if (fun)
    {
      (void) (*(fun)) (*this, event);
    }
    else
    {
      GetEventHandler()->OnCommand(*this, event);
    }

  wxNotifyEvent (event, FALSE);
}

#endif

/*
 * Message
 */
 
wxbMessage::wxbMessage (void)
{
  WXSET_TYPE(wxMessage, wxTYPE_MESSAGE)
  
  buttonFont = NULL;
  labelFont = NULL;
  isBitmap = FALSE;
}

wxbMessage::wxbMessage (wxPanel * panel, Const char *WXUNUSED(label), int WXUNUSED(x), int WXUNUSED(y),
   long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxMessage, wxTYPE_MESSAGE)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
  isBitmap = FALSE;
}

wxbMessage::wxbMessage (wxPanel * panel, Const char *WXUNUSED(label), int WXUNUSED(x), int WXUNUSED(y),
   int WXUNUSED(width), int WXUNUSED(height), long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxMessage, wxTYPE_MESSAGE)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
  isBitmap = FALSE;
}

#if USE_BITMAP_MESSAGE
wxbMessage::wxbMessage (wxPanel * panel, wxBitmap *WXUNUSED(image), int WXUNUSED(x), int WXUNUSED(y),
 long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxMessage, wxTYPE_MESSAGE)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
  isBitmap = TRUE;
}

wxbMessage::wxbMessage (wxPanel * panel, wxBitmap *WXUNUSED(image), int WXUNUSED(x), int WXUNUSED(y),
 int WXUNUSED(width), int WXUNUSED(height), long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxMessage, wxTYPE_MESSAGE)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
  isBitmap = TRUE;
}
#endif

wxbMessage::~wxbMessage (void)
{
}

/*
 * Text item
 */
 
wxbText::wxbText (void)
{
  WXSET_TYPE(wxText, wxTYPE_TEXT)
  
  buttonFont = NULL;
  labelFont = NULL;
}

wxbText::wxbText (wxPanel * panel, wxFunction WXUNUSED(Function), Const char *WXUNUSED(label), Constdata char *WXUNUSED(value),
	 int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height), long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxText, wxTYPE_TEXT)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbText::~wxbText (void)
{
}


void wxbText::Command (wxCommandEvent & event)
{
  SetValue (event.commandString);
  ProcessCommand (event);
}

void wxbText::ProcessCommand (wxCommandEvent & event)
{
  if (wxNotifyEvent (event, TRUE))
    return;

  wxFunction fun = callback;
  if (fun)
    {
      (void) (*(fun)) (*this, event);
    }
    else
    {
      GetEventHandler()->OnCommand(*this, event);
    }

  wxNotifyEvent (event, FALSE);
}

/*
 * Multi-line Text item
 */
 
wxbMultiText::wxbMultiText (void)
{
  WXSET_TYPE(wxMultiText, wxTYPE_MULTI_TEXT)
  
  buttonFont = NULL;
  labelFont = NULL;
}

wxbMultiText::wxbMultiText (wxPanel * panel, wxFunction WXUNUSED(Function),
              Const char *WXUNUSED(label), Constdata char *WXUNUSED(value),
	      int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height),
              long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxMultiText, wxTYPE_MULTI_TEXT)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
}

void wxbMultiText::Command (wxCommandEvent & event)
{
  SetValue (event.commandString);
  ProcessCommand (event);
}

void wxbMultiText::ProcessCommand (wxCommandEvent & event)
{
  if (wxNotifyEvent (event, TRUE))
    return;

  wxFunction fun = callback;
  if (fun)
    {
      (void) (*(fun)) (*this, event);
    }
    else
    {
      GetEventHandler()->OnCommand(*this, event);
    }
  wxNotifyEvent (event, FALSE);
}

/*
 * Slider
 */
 
wxbSlider::wxbSlider (void)
{
  WXSET_TYPE(wxSlider, wxTYPE_SLIDER)
  
  buttonFont = NULL;
  labelFont = NULL;
}

wxbSlider::wxbSlider (wxPanel * panel, wxFunction WXUNUSED(func), Const char *WXUNUSED(label), int WXUNUSED(value),
	   int WXUNUSED(min_value), int WXUNUSED(max_value), int WXUNUSED(width),
           int WXUNUSED(x), int WXUNUSED(y), long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxSlider, wxTYPE_SLIDER)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = panel->label_position;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbSlider::~wxbSlider (void)
{
}

void wxbSlider::Command (wxCommandEvent & event)
{
  SetValue (event.commandInt);
  ProcessCommand (event);
}

void wxbSlider::ProcessCommand (wxCommandEvent & event)
{
  if (wxNotifyEvent (event, TRUE))
    return;

  wxFunction fun = callback;
  if (fun)
    {
      (void) (*(fun)) (*this, event);
    }
    else
    {
      GetEventHandler()->OnCommand(*this, event);
    }
  wxNotifyEvent (event, FALSE);
}

/*
 * Group box
 */
 
wxbGroupBox::wxbGroupBox (void)
{
  WXSET_TYPE(wxGroupBox, wxTYPE_GROUP_BOX)
  
  window_parent = NULL;
  labelPosition = wxHORIZONTAL;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbGroupBox::wxbGroupBox (wxPanel * panel, Const char *WXUNUSED(label),
	   int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height),
           long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxGroupBox, wxTYPE_GROUP_BOX)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = wxHORIZONTAL;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbGroupBox::~wxbGroupBox (void)
{
}

/*
 * Gauge
 */
 
#if USE_GAUGE
wxbGauge::wxbGauge (void)
{
  WXSET_TYPE(wxGauge, wxTYPE_GAUGE)
  
  window_parent = NULL;
  labelPosition = wxHORIZONTAL;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbGauge::wxbGauge (wxPanel * panel, Const char *WXUNUSED(label),
	   int WXUNUSED(range), int WXUNUSED(x), int WXUNUSED(y),
           int WXUNUSED(width), int WXUNUSED(height), long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxGauge, wxTYPE_GAUGE)
  
  windowStyle = style;
  window_parent = panel;
  labelPosition = wxHORIZONTAL;
  buttonFont = NULL;
  labelFont = NULL;
}

wxbGauge::~wxbGauge (void)
{
}
#endif
