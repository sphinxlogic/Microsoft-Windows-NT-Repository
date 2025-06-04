/*
 * File:	wb_panel.cc
 * Purpose:	wxPanel class implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wb_panel.cc,v 1.3 1994/08/14 21:34:01 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "@(#)wb_panel.cc	1.2 5/9/94"; */

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_buttn.h"
#include "wx_lbox.h"
#include "wx_stdev.h"
#include "wx_utils.h"
#endif

#include <stdlib.h>
#include <math.h>

#if USE_EXTENDED_STATICS
#include "wx_stat.h"
#endif

class wxFrame;
class wxPanel;

// Constructors

wxbPanel::wxbPanel(void)
{
  WXSET_TYPE(wxPanel, wxTYPE_PANEL)
  
  defaultItem = NULL;
  new_line = FALSE;
  label_position = wxHORIZONTAL;
  window_parent = NULL;

  dragSlow = TRUE;
  dragMode = wxDRAG_MODE_NONE;
  dragType = wxDRAG_TYPE_NONE;
  dragItem = NULL;
  firstDragX = 0;
  firstDragY = 0;
  oldDragX = 0;
  oldDragY = 0;
  dragTolerance = 3;
  checkTolerance = TRUE;
}

wxbPanel::wxbPanel(wxWindow *parent, int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height),
   long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxPanel, wxTYPE_PANEL)
  
  windowStyle = style;
  defaultItem = NULL;
  new_line = FALSE;
  label_position = wxHORIZONTAL;

  window_parent = parent;

  dragSlow = TRUE;
  dragMode = wxDRAG_MODE_NONE;
  dragType = wxDRAG_TYPE_NONE;
  dragItem = NULL;
  firstDragX = 0;
  firstDragY = 0;
  oldDragX = 0;
  oldDragY = 0;
  dragTolerance = 3;
  checkTolerance = TRUE;
}

wxbPanel::~wxbPanel(void)
{
#if USE_EXTENDED_STATICS
  wxStaticItem *s_item;
  wxNode *node = staticItems.First();
  while (node)
  {
    s_item = (wxStaticItem *) node -> Data();
    wxNode *next = node->Next();
    delete s_item;
    node = next ;
  }
#endif
}

void wxbPanel::OnChangeFocus(wxItem *, wxItem *)
{
}

Bool wxbPanel::OnFunctionKey(wxKeyEvent &)
{
	return FALSE;
}

wxObject* wxbPanel::GetChild(int number)
{
  // Return a pointer to the Nth object in the Panel
  if (!children)
    return(NULL) ;
  wxNode *node = GetChildren()->First();
  while (node && number--)
    node = node->Next() ;
  if (node)
  {
    wxObject *obj = (wxObject *)node->Data();
    return(obj) ;
  }
  else
    return NULL ;
}

void wxbPanel::SetLabelPosition(int pos)  // wxHORIZONTAL or wxVERTICAL
{
  label_position = pos;
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

void wxbPanel::OnDefaultAction(wxItem *initiatingItem)
{
  if (initiatingItem->IsKindOf(CLASSINFO(wxListBox)) && initiatingItem->callback)
  {
    wxListBox *lbox = (wxListBox *)initiatingItem;
    wxCommandEvent event(wxEVENT_TYPE_LISTBOX_DCLICK_COMMAND);
    event.commandInt = -1;
    if ((lbox->GetWindowStyleFlag() & wxLB_SINGLE) || (lbox->GetSelectionMode() == wxSINGLE))
    {
      event.commandString = copystring(lbox->GetStringSelection());
      event.commandInt = lbox->GetSelection();
      event.clientData = lbox->wxListBox::GetClientData(event.commandInt);
    }
    event.eventObject = lbox;

    lbox->ProcessCommand(event);

    if (event.commandString)
      delete[] event.commandString;
    return;
  }
  
  wxButton *but = GetDefaultItem();
  if (but)
  {
    wxCommandEvent event(wxEVENT_TYPE_BUTTON_COMMAND);
    but->Command(event);
  }
}

void wxbPanel::SetLabelFont(wxFont *fnt)
{
  labelFont = fnt ;
  /* MATTHEW: Label font resolution does not belong here. */
/*
  this->font=labelFont; 
#ifdef wx_motif
  int scaled_size = (int) (10 * ((int) (this->font->GetPointSize () + 0.5))); 
  int res_x = 100; 
  int res_y = 100; 
  XFontStruct *fontStruct = wxFontPool->FindNearestFont (this->font->GetFamily (
),  
                                this->font->GetStyle (), 
                                this->font->GetWeight (), scaled_size, 
                                this->font->GetUnderlined (), res_x, res_y); 
  this->font->xFont=fontStruct; 
#endif 
*/
}

void wxbPanel::SetButtonFont(wxFont *theFont)
{
  buttonFont = theFont ;
}

void wxbPanel::SetBackgroundColour(wxColour& col)
{
  backColour = col ;
}

void wxbPanel::SetLabelColour(wxColour& col)
{
  labelColour = col ;
}

void wxbPanel::SetButtonColour(wxColour& col)
{
  buttonColour = col ;
}

/*
void wxbPanel::SetBackgroundColour(wxColour *col)
{
  if (col)
    SetBackgroundColour(*col) ;
}

void wxbPanel::SetLabelColour(wxColour *col)
{
  if (col)
    SetLabelColour(*col) ;
}

void wxbPanel::SetButtonColour(wxColour *col)
{
  if (col)
    SetButtonColour(*col);
}
*/

/*
 * Called if in editing mode
 */

// An event outside any items: may be a drag event.
void wxbPanel::OnEvent(wxMouseEvent& event)
{
  if (editUIMode)
  {
    // If we're dragging an item or selection handle,
    // continue dragging.
    if (dragMode != wxDRAG_MODE_NONE)
    {
      ProcessItemEvent(dragItem, event, dragType);
      return;
    }
  
    float xx, yy;
    event.Position(&xx, &yy);
    int x = (int)xx;
    int y = (int)yy;
    
    // Find which selection handle we're on, if any
    wxNode *node = GetChildren()->First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (win->IsKindOf(CLASSINFO(wxItem)))
      {
        wxItem *item = (wxItem *)win;
        int selHandle = item->SelectionHandleHitTest(x, y);
        if (selHandle > 0)
        {
          ProcessItemEvent(item, event, selHandle);
          return;
        }
      }
      node = node->Next();
    }

    // We're not on an item or selection handle.
    // so... check for a left or right click event
    // to send to the application.
    int keys = 0;
    if (event.ShiftDown()) keys = keys | wxKEY_SHIFT;
    if (event.ControlDown()) keys = keys | wxKEY_CTRL;

    if (event.LeftUp())
      GetEventHandler()->OnLeftClick(x, y, keys);
    else if (event.RightUp())
      GetEventHandler()->OnRightClick(x, y, keys);
  }
}

void wxbPanel::OnItemEvent(wxItem *item, wxMouseEvent& event)
{
  if (!editUIMode)
    return;

  // Not a selection handle event: just a normal item event.
  // Transform to panel coordinates.
  int x, y;
  item->GetPosition(&x, &y);

  event.x = (float)(event.x + x);
  event.y = (float)(event.y + y);
  ProcessItemEvent(item, event, dragType);
}

void wxbPanel::ProcessItemEvent(wxItem *item, wxMouseEvent& event, int selectionHandle)
{
  float x, y;
  event.Position(&x, &y);
  int keys = 0;
  if (event.ShiftDown()) keys = keys | wxKEY_SHIFT;
  if (event.ControlDown()) keys = keys | wxKEY_CTRL;
  Bool dragging = event.Dragging();
  if (dragging)
  {
    int dx = (int)abs((int)(x - firstDragX));
    int dy = (int)abs((int)(y - firstDragY));
    if (checkTolerance && (dx <= dragTolerance) && (dy <= dragTolerance))
    {
      return;
    }
    else
      // If we've ignored the tolerance once, then ALWAYS ignore
      // tolerance in this drag, even if we come back within
      // the tolerance range.
      {
        checkTolerance = FALSE;
      }
  }
  if (dragging && dragItem && dragMode == wxDRAG_MODE_START_LEFT)
  {
    dragMode = wxDRAG_MODE_CONTINUE_LEFT;
    dragItem->OnDragBegin((int)x, (int)y, keys, GetPanelDC(), selectionHandle);
    oldDragX = (int)x; oldDragY = (int)y;
  }
  else if (dragging && dragItem && dragMode == wxDRAG_MODE_CONTINUE_LEFT)
  { 
    dragItem->OnDragContinue(FALSE, oldDragX, oldDragY, keys, GetPanelDC(), selectionHandle);
    dragItem->OnDragContinue(TRUE, (int)x, (int)y, keys, GetPanelDC(), selectionHandle);
    oldDragX = (int)x; oldDragY = (int)y;
  }
  else if (event.LeftUp() && dragItem && dragMode == wxDRAG_MODE_CONTINUE_LEFT)
  {
    dragMode = wxDRAG_MODE_NONE;
    checkTolerance = TRUE;
    dragItem->OnDragContinue(FALSE, oldDragX, oldDragY, keys, GetPanelDC(), selectionHandle);
    dragItem->OnDragEnd((int)x, (int)y, keys, GetPanelDC(), selectionHandle);
    dragItem = NULL;
    dragType = wxDRAG_TYPE_NONE;
  }
  else if (dragging && dragItem && dragMode == wxDRAG_MODE_START_RIGHT)
  {
    dragMode = wxDRAG_MODE_CONTINUE_RIGHT;
    dragItem->OnDragBegin((int)x, (int)y, keys, GetPanelDC(), selectionHandle);
    oldDragX = (int)x; oldDragY = (int)y;
  }
  else if (dragging && dragItem && dragMode == wxDRAG_MODE_CONTINUE_RIGHT)
  { 
//    dragItem->OnDragContinue(FALSE, oldDragX, oldDragY, keys, GetPanelDC(), selectionHandle);
//    dragItem->OnDragContinue(TRUE, (int)x, (int)y, keys, GetPanelDC(), selectionHandle);
    oldDragX = (int)x; oldDragY = (int)y;
  }
  else if (event.RightUp() && dragItem && dragMode == wxDRAG_MODE_CONTINUE_RIGHT)
  {
    dragMode = wxDRAG_MODE_NONE;
    checkTolerance = TRUE;
//    dragItem->OnDragContinue(FALSE, oldDragX, oldDragY, keys, GetPanelDC(), selectionHandle);
//    dragItem->OnDragEnd((int)x, (int)y, keys, GetPanelDC(), selectionHandle);
    dragItem = NULL;
    dragType = wxDRAG_TYPE_NONE;
  }
  else if (event.IsButton())
  {
    checkTolerance = TRUE;

    if (event.LeftDown())
    {
      dragItem = item;
      dragMode = wxDRAG_MODE_START_LEFT;
      firstDragX = (int)x;
      firstDragY = (int)y;
      dragType = selectionHandle;
    }
    else if (event.RightDown())
    {
      dragItem = item;
      dragMode = wxDRAG_MODE_START_RIGHT;
      firstDragX = (int)x;
      firstDragY = (int)y;
      dragType = selectionHandle;
    }
    else if (event.LeftUp())
    {
      if (dragItem)
        dragItem->GetEventHandler()->OnLeftClick((int)x, (int)y, keys);
      else
        GetEventHandler()->OnLeftClick((int)x, (int)y, keys);
        
      dragItem = NULL; dragMode = wxDRAG_MODE_NONE; dragType = wxDRAG_TYPE_NONE;
    }
    else if (event.RightUp())
    {
      if (dragItem)
        dragItem->GetEventHandler()->OnRightClick((int)x, (int)y, keys);
      else
        GetEventHandler()->OnRightClick((int)x, (int)y, keys);
        
      dragItem = NULL; dragMode = wxDRAG_MODE_NONE; dragType = wxDRAG_TYPE_NONE;
    }
  }
}

// Calls DrawSelectionHandles for all items if
// edit mode is on.
void wxbPanel::PaintSelectionHandles(void)
{
  if (!editUIMode)
    return;

  wxPanelDC *theDC = GetPanelDC();
  theDC->BeginDrawing();

  wxNode *node = GetChildren()->First();
  while (node)
  {
    wxWindow *win = (wxWindow *)node->Data();
    if (win->IsKindOf(CLASSINFO(wxItem)))
    {
      wxItem *item = (wxItem *)win;
      // Don't draw handles for an item that's being moved: it'll
      // smear.
      if (item->IsSelected() && (item != dragItem))
        item->DrawSelectionHandles(theDC);
    }
    node = node->Next();
  }
  theDC->EndDrawing();
}


#if USE_WX_RESOURCES
#include "wx_res.h"
#include "wx_buttn.h"
#include "wx_check.h"
#include "wx_choic.h"
#include "wx_group.h"
#include "wx_messg.h"
#include "wx_txt.h"
#include "wx_mtxt.h"
#include "wx_rbox.h"
#include "wx_lbox.h"
#if USE_GAUGE
#include "wx_gauge.h"
#endif
#include "wx_slidr.h"
#include "wx_dialg.h"

/*
 * Optional resource loading facility
 */

Bool wxbPanel::LoadFromResource(wxWindow *parent, char *resourceName, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;
    
  wxItemResource *resource = table->FindResource(resourceName);
//  if (!resource || (resource->GetType() != wxTYPE_DIALOG_BOX))
  if (!resource || !resource->GetType() ||
    ! ((strcmp(resource->GetType(), "wxDialogBox") == 0) || (strcmp(resource->GetType(), "wxPanel") == 0)))
    return FALSE;

  char *title = resource->GetTitle();
  long theWindowStyle = resource->GetStyle();
  Bool isModal = (Bool)resource->GetValue1();
  int x = resource->GetX();
  int y = resource->GetY();
  int width = resource->GetWidth();
  int height = resource->GetHeight();
  char *name = resource->GetName();

  wxFont *theButtonFont = resource->GetButtonFont();
  wxFont *theLabelFont = resource->GetLabelFont();

  if (IsKindOf(CLASSINFO(wxDialogBox)))
  {
    wxDialogBox *dialogBox = (wxDialogBox *)this;
    if (!dialogBox->Create(parent, title, isModal, x, y, width, height, theWindowStyle, name))
      return FALSE;
  }
  else
  {
    if (!((wxPanel *)this)->Create(parent, x, y, width, height, theWindowStyle, name))
      return FALSE;
  }

  if (theButtonFont)
    SetButtonFont(theButtonFont);
  if (theLabelFont)
    SetLabelFont(theLabelFont);
    
  if (resource->GetBackgroundColour())
  {
    SetBackgroundColour(*resource->GetBackgroundColour());
    wxBrush *brush = wxTheBrushList->FindOrCreateBrush(resource->GetBackgroundColour(), wxSOLID);
    GetDC()->SetBackground(brush);
  }
  if (resource->GetLabelColour())
    SetLabelColour(*resource->GetLabelColour());
  if (resource->GetButtonColour())
    SetButtonColour(*resource->GetButtonColour());
    
  // Now create children
  wxNode *node = resource->GetChildren().First();
  while (node)
  {
    wxItemResource *childResource = (wxItemResource *)node->Data();
    
    (void) CreateItem(childResource, table);

    node = node->Next();
  }
  return TRUE;
}

wxItem *wxbPanel::CreateItem(wxItemResource *resource, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;
  return table->CreateItem((wxPanel *)this, resource);
}

#endif

#if USE_EXTENDED_STATICS
void wxbPanel::AddStaticItem(wxStaticItem *item)
{
   staticItems.Append((wxObject *) item);
}

void wxbPanel::RemoveStaticItem(wxStaticItem *item)
{
  wxNode *node = staticItems.Member((wxObject *)item);
  wxStaticItem *nitem;
  if (node) 
  {
     nitem = (wxStaticItem *) node -> Data();
     nitem -> Show(FALSE);
     delete node;
  }
}

void wxbPanel::DestroyStaticItem(wxStaticItem *item)
{
  wxNode *node = staticItems.Member((wxObject *)item);
  wxStaticItem *s_item;
  if (node)
  {
    s_item = (wxStaticItem *) node -> Data();
    item -> Show(FALSE);
    delete s_item;
    delete node;
  }
}
#endif
