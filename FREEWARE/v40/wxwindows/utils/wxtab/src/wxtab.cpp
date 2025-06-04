/*
 * File:     wxtab.cc
 * Purpose:  Tab library (tabbed panels/dialogs, etc.)
 * Author:   Julian Smart
	Last change:  JS   25 May 97    2:55 pm
 * Chris Breeze, 3 June 97: restructured wxTabControl::OnDraw
 * to make Windows and Unix versions more similar and to draw
 * rounded tabs.
 */

#ifdef __GNUG__
#pragma implementation "wxtab.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "wxtab.h"

IMPLEMENT_DYNAMIC_CLASS(wxTabControl, wxObject)

IMPLEMENT_DYNAMIC_CLASS(wxTabLayer, wxList)

wxTabControl::wxTabControl(wxTabView *v)
{
  view = v;
  isSelected = FALSE;
  labelFont = NULL;
  offsetX = 0;
  offsetY = 0;
  width = 0;
  height = 0;
  id = 0;
  rowPosition = 0;
  colPosition = 0;
}

wxTabControl::~wxTabControl(void)
{
}
    
void wxTabControl::OnDraw(wxDC *dc, Bool lastInRow)
{
	if (!(view && dc)) return;


	int tabInc   = 0;
	if (isSelected)
	{
		tabInc = view->GetTabSelectionHeight() - view->GetTabHeight();
	}
	int tabLeft  = GetX() + view->GetViewRect().x;
	int tabTop   = GetY() + view->GetViewRect().y - tabInc;
	int tabRight = tabLeft + view->GetTabWidth();
	int left     = view->GetViewRect().x;
	int top      = tabTop + view->GetTabHeight() + tabInc;
	int right    = left + view->GetViewRect().width;
	int bottom   = top + view->GetViewRect().height;

	if (isSelected)
	{
		// TAB is selected - draw TAB and the View's full outline

		dc->SetPen(view->GetHighlightPen());
		wxPoint pnts[10];
		int n = 0;
		pnts[n].x = (float)left;			pnts[n++].y = (float)bottom;
		pnts[n].x = (float)left;	 		pnts[n++].y = (float)top;
		pnts[n].x = (float)tabLeft; 		pnts[n++].y = (float)top;
		pnts[n].x = (float)tabLeft;			pnts[n++].y = (float)tabTop + 2;
		pnts[n].x = (float)tabLeft + 2;		pnts[n++].y = (float)tabTop;
		pnts[n].x = (float)tabRight - 1;	pnts[n++].y = (float)tabTop;
		dc->DrawLines(n, pnts);
		if (!lastInRow)
		{
			dc->DrawLine(
					(float)(tabRight + 2),
					(float)top,
					(float)right,
					(float)top
					);
		}

		dc->SetPen(view->GetShadowPen());
		dc->DrawLine(
				(float)tabRight,
				(float)tabTop + 2,
				(float)tabRight,
				(float)top
				);
		dc->DrawLine(
				(float)right,
				(float)top,
				(float)right,
				(float)bottom
				);
		dc->DrawLine(
				(float)right,
				(float)bottom,
				(float)left,
				(float)bottom
				);

		dc->SetPen(wxBLACK_PEN);
		dc->DrawPoint(
				(float)tabRight,
				(float)tabTop + 1
				);
		dc->DrawPoint(
				(float)tabRight + 1,
				(float)tabTop + 2
				);
		if (lastInRow)
		{
			dc->DrawLine(
				(float)tabRight + 1,
				(float)bottom,
				(float)tabRight + 1,
				(float)tabTop + 1
				);
		}
		else
		{
			dc->DrawLine(
				(float)tabRight + 1,
				(float)tabTop + 2,
				(float)tabRight + 1,
				(float)top
				);
			dc->DrawLine(
				(float)right + 1,
				(float)top,
				(float)right + 1,
				(float)bottom + 1
				);
		}
		dc->DrawLine(
				(float)right + 1,
				(float)bottom + 1,
				(float)left + 1,
				(float)bottom + 1
				);
	}
	else
	{
		// TAB is not selected - just draw TAB outline and RH edge
		// if the TAB is the last in the row

		int maxPositions = ((wxTabLayer*)view->GetLayers().Nth(0)->Data())->Number();
		wxTabControl* tabBelow = 0;
		wxTabControl* tabBelowRight = 0;
		if (GetColPosition() > 0)
		{
			tabBelow = view->FindTabControlForPosition(
						GetColPosition() - 1,
						GetRowPosition()
						);
		}
		if (!lastInRow && GetColPosition() > 0)
		{
			tabBelowRight = view->FindTabControlForPosition(
						GetColPosition() - 1,
						GetRowPosition() + 1
						);
		}

		float raisedTop = (float)top - view->GetTabSelectionHeight() +
							view->GetTabHeight();

		dc->SetPen(view->GetHighlightPen());
		wxPoint pnts[10];
		int n = 0;

		pnts[n].x = (float)tabLeft;

		if (tabBelow && tabBelow->IsSelected())
		{
			pnts[n++].y = raisedTop;
		}
		else
		{
			pnts[n++].y = (float)top;
		}
		pnts[n].x = (float)tabLeft;			pnts[n++].y = (float)tabTop + 2;
		pnts[n].x = (float)tabLeft + 2;		pnts[n++].y = (float)tabTop;
		pnts[n].x = (float)tabRight - 1;	pnts[n++].y = (float)tabTop;
		dc->DrawLines(n, pnts);

		dc->SetPen(view->GetShadowPen());
		if (GetRowPosition() >= maxPositions - 1)
		{
			dc->DrawLine(
					(float)tabRight,
					(float)(tabTop + 2),
					(float)tabRight,
					(float)bottom
					);
			dc->DrawLine(
					(float)tabRight,
					(float)bottom,
					(float)(tabRight - view->GetHorizontalTabOffset()),
					(float)bottom
					);
		}
		else
		{
			if (tabBelowRight && tabBelowRight->IsSelected())
			{
				dc->DrawLine(
						(float)tabRight,
						(float)raisedTop,
						(float)tabRight,
						(float)tabTop + 1
						);
			}
			else
			{
				dc->DrawLine(
						(float)tabRight,
						(float)top - 1,
						(float)tabRight,
						(float)tabTop + 1
						);
			}
		}

		dc->SetPen(wxBLACK_PEN);
		dc->DrawPoint(
				(float)tabRight,
				(float)tabTop + 1
				);
		dc->DrawPoint(
				(float)tabRight + 1,
				(float)tabTop + 2
				);
		if (GetRowPosition() >= maxPositions - 1)
		{
			// draw right hand edge to bottom of view
			dc->DrawLine(
					(float)tabRight + 1,
					(float)bottom + 1,
					(float)tabRight + 1,
					(float)tabTop + 2
					);
			dc->DrawLine(
					(float)tabRight + 1,
					(float)bottom + 1,
					(float)(tabRight - view->GetHorizontalTabOffset()),
					(float)bottom + 1
					);
		}
		else
		{
			// draw right hand edge of TAB
			if (tabBelowRight && tabBelowRight->IsSelected())
			{
				dc->DrawLine(
						(float)tabRight + 1,
						(float)raisedTop - 1,
						(float)tabRight + 1,
						(float)tabTop + 2
						);
			}
			else
			{
				dc->DrawLine(
						(float)tabRight + 1,
						(float)top - 1,
						(float)tabRight + 1,
						(float)tabTop + 2
						);
			}
		}
	}

	// Draw centered text
	dc->SetPen(wxBLACK_PEN);
	if (isSelected)
	{
		dc->SetFont(view->GetSelectedTabFont());
	}
	else
	{
		dc->SetFont(GetFont());
	}

	wxColour col(view->GetTextColour());
	dc->SetTextForeground(&col);
	dc->SetBackgroundMode(wxTRANSPARENT);
	float textWidth, textHeight;
	dc->GetTextExtent(GetLabel(), &textWidth, &textHeight);
  
	float textX = (float)(tabLeft + tabRight - textWidth) / 2;
	float textY = (float)(tabInc + tabTop +
						  view->GetVerticalTabTextSpacing());
	dc->DrawText(GetLabel(), textX, textY);
}

Bool wxTabControl::HitTest(int x, int y)
{
  // Top-left of tab control
  int tabX1 = GetX() + view->GetViewRect().x;
  int tabY1 = GetY() + view->GetViewRect().y;
  
  // Bottom-right
  int tabX2 = tabX1 + GetWidth();
  int tabY2 = tabY1 + GetHeight();
  
  if (x >= tabX1 && y >= tabY1 && x <= tabX2 && y <= tabY2)
    return TRUE;
  else
    return FALSE;
}

IMPLEMENT_DYNAMIC_CLASS(wxTabView, wxObject)

wxTabView::wxTabView(long style)
{
  noTabs = 0;
  tabStyle = style;
  tabSelection = -1;
  tabHeight = 20;
  tabSelectionHeight = tabHeight + 2;
  tabWidth = 80;
  tabHorizontalOffset = 10;
  tabHorizontalSpacing = 2;
  tabVerticalTextSpacing = 3;
  topMargin = 5;
  tabViewRect.x = 20;
  tabViewRect.y = 20;
  tabViewRect.width = 300;
  tabViewRect.x = 300;
  highlightColour = *wxWHITE;
  shadowColour = wxColour(128, 128, 128);
  backgroundColour = *wxLIGHT_GREY;
  textColour = *wxBLACK;
  highlightPen = wxWHITE_PEN;
  shadowPen = wxGREY_PEN;
  backgroundPen = wxLIGHT_GREY_PEN;
  backgroundBrush = wxLIGHT_GREY_BRUSH;
  tabFont = wxTheFontList->FindOrCreateFont(11, wxSWISS, wxNORMAL, wxNORMAL);
  tabSelectedFont = wxTheFontList->FindOrCreateFont(11, wxSWISS, wxNORMAL, wxBOLD);
  tabDC = NULL;
}

wxTabView::~wxTabView()
{
}
  
// Automatically positions tabs
wxTabControl *wxTabView::AddTab(int id, const wxString& label, wxTabControl *existingTab)
{
  // First, find which layer we should be adding to.
  wxNode *node = layers.Last();
  if (!node)
  {
    wxTabLayer *newLayer = new wxTabLayer;
    node = layers.Append(newLayer);
  }
  // Check if adding another tab control would go off the
  // right-hand edge of the layer.
  wxTabLayer *tabLayer = (wxTabLayer *)node->Data();
  wxNode *lastTabNode = tabLayer->Last();
  if (lastTabNode)
  {
    wxTabControl *lastTab = (wxTabControl *)lastTabNode->Data();
    // Start another layer (row).
    // Tricky choice: can't just check if will be overlapping the edge, because
    // this happens anyway for 2nd and subsequent rows.
    // Should check this for 1st row, and then subsequent rows should not exceed 1st
    // in length.
    if (((tabLayer == layers.First()->Data()) && ((lastTab->GetX() + 2*lastTab->GetWidth() + GetHorizontalTabSpacing())
              > GetViewRect().width)) ||
        ((tabLayer != layers.First()->Data()) && (tabLayer->Number() == ((wxTabLayer *)layers.First()->Data())->Number())))
    {
      tabLayer = new wxTabLayer;
      layers.Append(tabLayer);
      lastTabNode = NULL;
    }
  }
  int layer = layers.Number() - 1;
  
  wxTabControl *tabControl = existingTab;
  if (!existingTab)
    tabControl = OnCreateTabControl();
  tabControl->SetRowPosition(tabLayer->Number());
  tabControl->SetColPosition(layer);
  
  wxTabControl *lastTab = NULL;
  if (lastTabNode)
    lastTab = (wxTabControl *)lastTabNode->Data();
  
  // Top of new tab
  int verticalOffset = (- GetTopMargin()) - ((layer+1)*GetTabHeight());
  // Offset from view top-left
  int horizontalOffset = 0;
  if (!lastTab)
    horizontalOffset = layer*GetHorizontalTabOffset();
  else
    horizontalOffset = lastTab->GetX() + GetTabWidth() + GetHorizontalTabSpacing();
  
  tabControl->SetPosition(horizontalOffset, verticalOffset);
  tabControl->SetSize(GetTabWidth(), GetTabHeight());
  tabControl->SetId(id);
  tabControl->SetLabel(label);
  tabControl->SetFont(GetTabFont());
  
  tabLayer->Append(tabControl);
  noTabs ++;
  
  return tabControl;
}
  
void wxTabView::ClearTabs(Bool deleteTabs)
{
  wxNode *layerNode = layers.First();
  while (layerNode)
  {
    wxTabLayer *layer = (wxTabLayer *)layerNode->Data();
    wxNode *tabNode = layer->First();
    while (tabNode)
    {
      wxTabControl *tab = (wxTabControl *)tabNode->Data();
      if (deleteTabs)
        delete tab;
      wxNode *next = tabNode->Next();
      delete tabNode;
      tabNode = next;
    }
    wxNode *nextLayerNode = layerNode->Next();
    delete layer;
    delete layerNode;
    layerNode = nextLayerNode;
  }
}
  
// Layout tabs (optional, e.g. if resizing window)
void wxTabView::Layout(void)
{
  // Make a list of the tab controls, deleting the wxTabLayers.
  wxList controls;

  wxNode *layerNode = layers.First();
  while (layerNode)
  {
    wxTabLayer *layer = (wxTabLayer *)layerNode->Data();
    wxNode *tabNode = layer->First();
    while (tabNode)
    {
      wxTabControl *tab = (wxTabControl *)tabNode->Data();
      controls.Append(tab);
      wxNode *next = tabNode->Next();
      delete tabNode;
      tabNode = next;
    }
    wxNode *nextLayerNode = layerNode->Next();
    delete layer;
    delete layerNode;
    layerNode = nextLayerNode;
  }
  
  wxTabControl *lastTab = NULL;
  
  wxTabLayer *currentLayer = new wxTabLayer;
  layers.Append(currentLayer);
  
  wxNode *node = controls.First();
  while (node)
  {
    wxTabControl *tabControl = (wxTabControl *)node->Data();
    if (lastTab)
    {
      // Start another layer (row).
      // Tricky choice: can't just check if will be overlapping the edge, because
      // this happens anyway for 2nd and subsequent rows.
      // Should check this for 1st row, and then subsequent rows should not exceed 1st
      // in length.
      if (((currentLayer == layers.First()->Data()) && ((lastTab->GetX() + 2*lastTab->GetWidth() + GetHorizontalTabSpacing())
                > GetViewRect().width)) ||
          ((currentLayer != layers.First()->Data()) && (currentLayer->Number() == ((wxTabLayer *)layers.First()->Data())->Number())))
     {
       currentLayer = new wxTabLayer;
       layers.Append(currentLayer);
       lastTab = NULL;
     }
    }
    
    int layer = layers.Number() - 1;

    tabControl->SetRowPosition(currentLayer->Number());
    tabControl->SetColPosition(layer);
  
    // Top of new tab
    int verticalOffset = (- GetTopMargin()) - ((layer+1)*GetTabHeight());
    // Offset from view top-left
    int horizontalOffset = 0;
    if (!lastTab)
      horizontalOffset = layer*GetHorizontalTabOffset();
    else
      horizontalOffset = lastTab->GetX() + GetTabWidth() + GetHorizontalTabSpacing();
  
    tabControl->SetPosition(horizontalOffset, verticalOffset);
    tabControl->SetSize(GetTabWidth(), GetTabHeight());

    currentLayer->Append(tabControl);
    lastTab = tabControl;

    node = node->Next();
  }

  // Move the selected tab to the bottom
  wxTabControl *control = FindTabControlForId(tabSelection);
  if (control)
    MoveSelectionTab(control);

}

// Draw all tabs
void wxTabView::Draw(void)
{
	if (!GetDC()) return;
  
	GetDC()->BeginDrawing();
  
	// Draw top margin area (beneath tabs and above view area)
	if (GetTabStyle() & wxTAB_STYLE_COLOUR_INTERIOR)
	{
		GetDC()->SetPen(wxTRANSPARENT_PEN);
		GetDC()->SetBrush(GetBackgroundBrush());

		// Add 1 because the pen is transparent. Under Motif, may be different.
		GetDC()->DrawRectangle(
				(float)tabViewRect.x,
				(float)(tabViewRect.y - topMargin),
				(float)(tabViewRect.width + 1),
				(float)(topMargin + 1)
				);
	}

	// Draw layers in reverse order
	wxNode *node = layers.Last();
	while (node)
	{
		wxTabLayer *layer = (wxTabLayer *)node->Data();
		wxNode *node2 = layer->First();
		while (node2)
		{
			wxTabControl *control = (wxTabControl *)node2->Data();
			control->OnDraw(GetDC(), (node2->Next() == NULL));
			node2 = node2->Next();
		}

		node = node->Previous();
	}


#if 0
	if (GetTabStyle() & wxTAB_STYLE_DRAW_BOX)
	{
		GetDC()->SetPen(GetShadowPen());

		// Draw bottom line
		GetDC()->DrawLine(
				(float)(GetViewRect().x + 1),
				(float)(GetViewRect().y + GetViewRect().height),
				(float)(GetViewRect().x + GetViewRect().width),
				(float)(GetViewRect().y + GetViewRect().height)
				);

		// Draw right line
		GetDC()->DrawLine(
				(float)(GetViewRect().x + GetViewRect().width),
				(float)(GetViewRect().y - GetTopMargin() + 1),
				(float)(GetViewRect().x + GetViewRect().width),
				(float)(GetViewRect().y + GetViewRect().height)
				);

		GetDC()->SetPen(wxBLACK_PEN);

		// Draw bottom line
		GetDC()->DrawLine(
				(float)(GetViewRect().x),
				(float)(GetViewRect().y + GetViewRect().height + 1),
				(float)(GetViewRect().x + GetViewRect().width),
				(float)(GetViewRect().y + GetViewRect().height + 1)
				);

		// Draw right line
		GetDC()->DrawLine(
				(float)(GetViewRect().x + GetViewRect().width + 1),
				(float)(GetViewRect().y - GetTopMargin()),
				(float)(GetViewRect().x + GetViewRect().width + 1),
				(float)(GetViewRect().y + GetViewRect().height + 1)
				);
	}
#endif
	GetDC()->EndDrawing();
}
  
// Process mouse event, return FALSE if we didn't process it
Bool wxTabView::OnEvent(wxMouseEvent& event)
{
  if (!event.LeftDown())
    return FALSE;
    
  float x, y;
  event.Position(&x, &y);
  
  wxTabControl *hitControl = NULL;
  
  wxNode *node = layers.First();
  while (node)
  {
    wxTabLayer *layer = (wxTabLayer *)node->Data();
    wxNode *node2 = layer->First();
    while (node2)
    {
      wxTabControl *control = (wxTabControl *)node2->Data();
      if (control->HitTest((int)x, (int)y))
      {
        hitControl = control;
        node = NULL;
        node2 = NULL;
      }
      else
        node2 = node2->Next();
    }
  
    if (node)
      node = node->Next();
  }
  
  if (!hitControl)
    return FALSE;
    
  wxTabControl *currentTab = FindTabControlForId(tabSelection);
    
  if (hitControl == currentTab)
    return FALSE;
    
  ChangeTab(hitControl);
  
  return TRUE;
}

Bool wxTabView::ChangeTab(wxTabControl *control)
{
  wxTabControl *currentTab = FindTabControlForId(tabSelection);
  int oldTab = -1;
  if (currentTab)
    oldTab = currentTab->GetId();
  
  if (control == currentTab)
    return TRUE;
    
  if (layers.Number() == 0)
    return FALSE;
    
  if (!OnTabPreActivate(control->GetId(), oldTab))
    return FALSE;

  // Move the tab to the bottom
  MoveSelectionTab(control);

  if (currentTab)
    currentTab->SetSelected(FALSE);
    
  control->SetSelected(TRUE);
  tabSelection = control->GetId();

  OnTabActivate(control->GetId(), oldTab);
  
  // Leave window refresh for the implementing window

  return TRUE;
}

// Move the selected tab to the bottom layer, if necessary,
// without calling app activation code
Bool wxTabView::MoveSelectionTab(wxTabControl *control)
{
  if (layers.Number() == 0)
    return FALSE;
    
  wxTabLayer *firstLayer = (wxTabLayer *)layers.First()->Data();
    
  // Find what column this tab is at, so we can swap with the one at the bottom.
  // If we're on the bottom layer, then no need to swap.
  if (!firstLayer->Member(control))
  {
    // Do a swap
    int col = 0;
    wxNode *thisNode = FindTabNodeAndColumn(control, &col);
    if (!thisNode)
      return FALSE;
    wxNode *otherNode = firstLayer->Nth(col);
    if (!otherNode)
      return FALSE;
      
    // If this is already in the bottom layer, return now
    if (otherNode == thisNode)
      return TRUE;
      
    wxTabControl *otherTab = (wxTabControl *)otherNode->Data();
     
    // We now have pointers to the tab to be changed to,
    // and the tab on the first layer. Swap tab structures and
    // position details.
     
    int thisX = control->GetX();
    int thisY = control->GetY();
    int thisColPos = control->GetColPosition();
    int otherX = otherTab->GetX();
    int otherY = otherTab->GetY();
    int otherColPos = otherTab->GetColPosition();
     
    control->SetPosition(otherX, otherY);
    control->SetColPosition(otherColPos);
    otherTab->SetPosition(thisX, thisY);
    otherTab->SetColPosition(thisColPos);
     
    // Swap the data for the nodes
    thisNode->SetData(otherTab);
    otherNode->SetData(control);
  }
  return TRUE;
}

// Called when a tab is activated
void wxTabView::OnTabActivate(int /*activateId*/, int /*deactivateId*/)
{
}
  
void wxTabView::SetHighlightColour(const wxColour& col)
{
  highlightColour = col;
  highlightPen = wxThePenList->FindOrCreatePen((wxColour *)&col, 1, wxSOLID);
}

void wxTabView::SetShadowColour(const wxColour& col)
{
  shadowColour = col;
  shadowPen = wxThePenList->FindOrCreatePen((wxColour *)&col, 1, wxSOLID);
}

void wxTabView::SetBackgroundColour(const wxColour& col)
{
  backgroundColour = col;
  backgroundPen = wxThePenList->FindOrCreatePen((wxColour *)&col, 1, wxSOLID);
  backgroundBrush = wxTheBrushList->FindOrCreateBrush((wxColour *)&col, wxSOLID);
}

void wxTabView::SetTabSelection(int sel, Bool activateTool)
{
  int oldSel = tabSelection;
  wxTabControl *control = FindTabControlForId(sel);

  if (!OnTabPreActivate(sel, oldSel))
    return;
    
  if (control)
    control->SetSelected(sel);
  else
  {
    wxMessageBox("Could not find tab for id", "Error", wxOK);
    return;
  }
    
  tabSelection = sel;
  MoveSelectionTab(control);
  
  if (activateTool)
    OnTabActivate(sel, oldSel);
}

// Find tab control for id
wxTabControl *wxTabView::FindTabControlForId(int id)
{
  wxNode *node1 = layers.First();
  while (node1)
  {
    wxTabLayer *layer = (wxTabLayer *)node1->Data();
    wxNode *node2 = layer->First();
    while (node2)
    {
      wxTabControl *control = (wxTabControl *)node2->Data();
      if (control->GetId() == id)
        return control;
      node2 = node2->Next();
    }
    node1 = node1->Next();
  }
  return NULL;
}

// Find tab control for layer, position (starting from zero)
wxTabControl *wxTabView::FindTabControlForPosition(int layer, int position)
{
  wxNode *node1 = layers.Nth(layer);
  if (!node1)
    return NULL;
  wxTabLayer *tabLayer = (wxTabLayer *)node1->Data();
  wxNode *node2 = tabLayer->Nth(position);
  if (!node2)
    return NULL;
  return (wxTabControl *)node2->Data();
}

// Find the node and the column at which this control is positioned.
wxNode *wxTabView::FindTabNodeAndColumn(wxTabControl *control, int *col)
{
  wxNode *node1 = layers.First();
  while (node1)
  {
    wxTabLayer *layer = (wxTabLayer *)node1->Data();
    int c = 0;
    wxNode *node2 = layer->First();
    while (node2)
    {
      wxTabControl *cnt = (wxTabControl *)node2->Data();
      if (cnt == control)
      {
        *col = c;
        return node2;
      }
      node2 = node2->Next();
      c ++;
    }
    node1 = node1->Next();
  }
  return NULL;
}

int wxTabView::CalculateTabWidth(int noTabs, Bool adjustView)
{
  tabWidth = (int)((tabViewRect.width - ((noTabs - 1)*GetHorizontalTabSpacing()))/noTabs);
  if (adjustView)
  {
    tabViewRect.width = noTabs*tabWidth + ((noTabs-1)*GetHorizontalTabSpacing());
  }
  return tabWidth;
}

/*
 * wxTabbedDialogBox
 */
 
IMPLEMENT_CLASS(wxTabbedDialogBox, wxDialogBox)

wxTabbedDialogBox::wxTabbedDialogBox(wxWindow *parent, Const char *title, Bool modal, int x, int y,
     int width, int height, long windowStyle, Constdata char *name):
   wxDialogBox(parent, title, modal, x, y, width, height, windowStyle, name)
{
  tabView = NULL;
}

wxTabbedDialogBox::~wxTabbedDialogBox(void)
{
  if (tabView)
    delete tabView;
}
 
Bool wxTabbedDialogBox::OnClose(void)
{
  return TRUE;
}

void wxTabbedDialogBox::OnEvent(wxMouseEvent& event)
{
  if (tabView)
    tabView->OnEvent(event);
}

void wxTabbedDialogBox::OnPaint(void)
{
  if (tabView)
    tabView->Draw();
}

/*
 * wxTabbedPanel
 */
 
IMPLEMENT_CLASS(wxTabbedPanel, wxPanel)

wxTabbedPanel::wxTabbedPanel(wxWindow *parent, int x, int y,
     int width, int height, long windowStyle, Constdata char *name):
   wxPanel(parent, x, y, width, height, windowStyle, name)
{
  tabView = NULL;
}

wxTabbedPanel::~wxTabbedPanel(void)
{
  delete tabView;
}
 
void wxTabbedPanel::OnEvent(wxMouseEvent& event)
{
  if (tabView)
    tabView->OnEvent(event);
}

void wxTabbedPanel::OnPaint(void)
{
  if (tabView)
    tabView->Draw();
}

/*
 * wxDialogTabView
 */
 
IMPLEMENT_CLASS(wxPanelTabView, wxTabView)
 
wxPanelTabView::wxPanelTabView(wxPanel *pan, long style): wxTabView(style), tabWindows(wxKEY_INTEGER)
{
  panel = pan;
  currentWindow = NULL;

  if (panel->IsKindOf(CLASSINFO(wxTabbedDialogBox)))
    ((wxTabbedDialogBox *)panel)->SetTabView(this);
  else if (panel->IsKindOf(CLASSINFO(wxTabbedPanel)))
    ((wxTabbedPanel *)panel)->SetTabView(this);

  SetDC(panel->GetDC());
}

wxPanelTabView::~wxPanelTabView(void)
{
  ClearWindows(TRUE);
}

// Called when a tab is activated
void wxPanelTabView::OnTabActivate(int activateId, int deactivateId)
{
  if (!panel)
    return;
    
  wxWindow *oldWindow = ((deactivateId == -1) ? 0 : GetTabPanel(deactivateId));
  wxWindow *newWindow = GetTabWindow(activateId);

  if (oldWindow)
    oldWindow->Show(FALSE);
  if (newWindow)
    newWindow->Show(TRUE);
    
  panel->Refresh();
}

   
void wxPanelTabView::AddTabWindow(int id, wxWindow *window)
{
  tabWindows.Append((long)id, window);
  window->Show(FALSE);
}

wxWindow *wxPanelTabView::GetTabWindow(int id)
{
  wxNode *node = tabWindows.Find((long)id);
  if (!node)
    return NULL;
  return (wxWindow *)node->Data();    
}

void wxPanelTabView::ClearWindows(Bool deleteWindows)
{
  if (deleteWindows)
    tabWindows.DeleteContents(TRUE);
  tabWindows.Clear();
  tabWindows.DeleteContents(FALSE);
}

void wxPanelTabView::ShowWindowForTab(int id)
{
  wxWindow *newWindow = GetTabWindow(id);
  if (newWindow == currentWindow)
    return;
  if (currentWindow)
    currentWindow->Show(FALSE);
  newWindow->Show(TRUE);
  newWindow->Refresh();
}

