/*
 * File:	wx_rdlg.cc
 * Purpose:	Resource editor: dialog handler implementation
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if defined(wx_msw) && !defined(GNUWIN32)
#include <strstrea.h>
#else
#include <strstream.h>
#endif

// #include "wx_scrol.h"
// #include "wx_help.h"

#include "wx_resed.h"
#include "wx_rprop.h"
#include "wx_repal.h"

extern void wxResourceEditWindow(wxWindow *win);

/*
 * Dialog box event handler
 */

wxResourceEditorDialogHandler::wxResourceEditorDialogHandler(wxPanel *dialog, wxItemResource *resource,
   wxEvtHandler *oldHandler, wxResourceManager *manager)
{
  handlerDialog = dialog;
  handlerResource = resource;
  handlerOldHandler = oldHandler;
  resourceManager = manager;
}

void wxResourceEditorDialogHandler::OnSize(int w, int h)
{
}

void wxResourceEditorDialogHandler::OnItemSelect(wxItem *item, Bool select)
{
  if (select)
    resourceManager->AddSelection(item);
  else
    resourceManager->RemoveSelection(item);
}


Bool wxResourceEditorDialogHandler::OnClose(void)
{
  handlerDialog->SetEventHandler(handlerOldHandler);
  
  // Save the information before deleting the dialog.
  resourceManager->InstantiateResourceFromWindow(handlerResource, handlerDialog, TRUE);
  
  resourceManager->DisassociateResource(handlerDialog, FALSE);

  handlerDialog->Show(FALSE);
  delete this;
  return TRUE;
}

// There's default behaviour in wxbPanel, so forward the event
// to that code: it'll eventually call OnLeftClick, etc.
void wxResourceEditorDialogHandler::OnEvent(wxMouseEvent& event)
{
  handlerOldHandler->OnEvent(event);
}

void wxResourceEditorDialogHandler::OnPaint(void)
{
  handlerOldHandler->OnPaint();
}

void wxResourceEditorDialogHandler::OnItemEvent(wxItem *item, wxMouseEvent& event)
{
  handlerOldHandler->OnItemEvent(item, event);
}

void wxResourceEditorDialogHandler::OnLeftClick(int x, int y, int keys)
{
  if (keys & wxKEY_CTRL)
  {
    wxResourceEditWindow(handlerDialog);
    return;
  }

  // Deselect all items if click on panel
  if (resourceManager->GetEditorPalette()->currentlySelected == PALETTE_ARROW)
  {
    Bool needsRefresh = 0;
    wxNode *node = handlerDialog->GetChildren()->First();
    while (node)
    {
      wxItem *item = (wxItem *)node->Data();
      if (item->IsSelected())
      {
        needsRefresh ++;
        OnItemSelect(item, FALSE);
        item->SelectItem(FALSE);
      }
      node = node->Next();
    }
    if (needsRefresh > 0)
    {
      handlerDialog->GetPanelDC()->Clear();
      handlerDialog->Refresh();
    }
    return;
  }

  switch (resourceManager->GetEditorPalette()->currentlySelected)
  {
        case PALETTE_FRAME:
          break;
        case PALETTE_DIALOG_BOX:
          break;
        case PALETTE_PANEL:
          break;
        case PALETTE_CANVAS:
          break;
        case PALETTE_TEXT_WINDOW:
          break;
        case PALETTE_BUTTON:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxButton", x, y);
          break;
        case PALETTE_BITMAP_BUTTON:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxButton", x, y, TRUE);
          break;
        case PALETTE_MESSAGE:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxMessage", x, y);
          break;
        case PALETTE_BITMAP_MESSAGE:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxMessage", x, y, TRUE);
          break;
        case PALETTE_TEXT:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxText", x, y);
          break;
        case PALETTE_MULTITEXT:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxMultiText", x, y);
          break;
        case PALETTE_CHOICE:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxChoice", x, y);
          break;
        case PALETTE_CHECKBOX:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxCheckBox", x, y);
          break;
        case PALETTE_RADIOBOX:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxRadioBox", x, y);
          break;
        case PALETTE_LISTBOX:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxListBox", x, y);
          break;
        case PALETTE_SLIDER:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxSlider", x, y);
          break;
        case PALETTE_GAUGE:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxGauge", x, y);
          break;
        case PALETTE_GROUPBOX:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxGroupBox", x, y);
          break;
        case PALETTE_SCROLLBAR:
          resourceManager->CreatePanelItem(handlerResource, handlerDialog, "wxScrollBar", x, y);
          break;
        default:
          break;
  }

  // Now switch pointer on.
  if (resourceManager->GetEditorPalette()->currentlySelected != PALETTE_ARROW)
  {
    resourceManager->GetEditorPalette()->ToggleTool(resourceManager->GetEditorPalette()->currentlySelected, FALSE);
    resourceManager->GetEditorPalette()->ToggleTool(PALETTE_ARROW, TRUE);
    resourceManager->GetEditorPalette()->currentlySelected = PALETTE_ARROW;
  }
}

void wxResourceEditorDialogHandler::OnRightClick(int x, int y, int keys)
{
  wxMenu *menu = resourceManager->GetPopupMenu();
  menu->SetClientData((char *)handlerDialog);
#if 0 // wx_motif
  handlerDialog->FakePopupMenu(menu, (float)x, (float)y);
#else
  handlerDialog->PopupMenu(menu, (float)x, (float)y);
#endif
}

void wxResourceEditorDialogHandler::OnItemLeftClick(wxItem *item, int x, int y, int keys)
{
  if (keys & wxKEY_CTRL)
  {
    wxResourceEditWindow(item);
    return;
  }
  
  // If this is a wxGroupBox and the pointer isn't an arrow, chances
  // are that we really meant to place an item on the panel.
  // Fake this event.
  if ((item->GetClassInfo() == CLASSINFO(wxGroupBox)) && resourceManager->GetEditorPalette()->currentlySelected != PALETTE_ARROW)
  {
    OnLeftClick(x,  y, keys);
    return;
  }
  
//  wxDebugMsg("Item %s, selected = %d\n", item->GetName(), item->IsSelected());

  if (item->IsSelected())
  {
    item->SelectItem(FALSE);
    item->GetParent()->GetEventHandler()->OnItemSelect(item, FALSE);
    handlerDialog->GetPanelDC()->Clear();
    handlerDialog->Refresh();
  }
  else
  {
    item->SelectItem(TRUE);
    item->GetParent()->GetEventHandler()->OnItemSelect(item, TRUE);
    
    // Deselect other items if shift is not pressed
    int needsRefresh = 0;
    if (!(keys & wxKEY_SHIFT))
    {
      wxNode *node = item->GetParent()->GetChildren()->First();
      while (node)
      {
        wxItem *child = (wxItem *)node->Data();
        if (child->IsSelected() && child != item)
        {
          child->SelectItem(FALSE);
          child->GetParent()->GetEventHandler()->OnItemSelect(child, FALSE);
          needsRefresh ++;
        }
        node = node->Next();
      }
    }

    handlerDialog->GetPanelDC()->BeginDrawing();
    item->DrawSelectionHandles(handlerDialog->GetPanelDC());
    handlerDialog->GetPanelDC()->EndDrawing();

    if (needsRefresh > 0)
    {
      handlerDialog->GetPanelDC()->Clear();
      handlerDialog->Refresh();
    }
  }
}

void wxResourceEditorDialogHandler::OnItemRightClick(wxItem *item, int x, int y, int keys)
{
/*
  if (keys & wxKEY_CTRL)
  {
    wxDebugMsg("Item %s, selected = %d\n", item->GetName(), item->IsSelected());
    return;
  }
*/

  wxMenu *menu = resourceManager->GetPopupMenu();
  menu->SetClientData((char *)item);
#ifdef 0 // wx_motif
  handlerDialog->FakePopupMenu(menu, (float)x, (float)y);
#else
  handlerDialog->PopupMenu(menu, (float)x, (float)y);
#endif
}

void wxResourceEditorDialogHandler::OnItemMove(wxItem *item, int x, int y)
{
/*
  BuildWindowData *child = buildWindow->FindChildWindow(item);
  if (child)
  {
    child->x = x;
    child->y = y;
  }
*/
}

void wxResourceEditorDialogHandler::OnItemSize(wxItem *item, int w, int h)
{
/*
  BuildWindowData *child = buildWindow->FindChildWindow(item);
  if (child)
  {
    child->width = w;
    child->height = h;
  }
*/
}

/*
 * Frame for editing a panel in
 */
 
Bool DialogEditorPanelFrame::OnClose(void)
{
  wxWindow *child = (wxWindow *)GetChildren()->First()->Data();
  wxEvtHandler *handler = child->GetEventHandler();
  return handler->OnClose();
}
