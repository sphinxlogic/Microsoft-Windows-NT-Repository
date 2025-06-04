/*
 * File:	rcload.cc
 * Purpose:	wxWindows GUI builder -- RC file loader
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wx_help.h"
#include "wx_types.h"
#include <ctype.h>
#include <stdlib.h>

#include "wxbuild.h"
#include "bapp.h"
#include "bmenu.h"
#include "bsubwin.h"
#include "bitem.h"
#include "rcload.h"

rcFile *TheRCFile = NULL;
RCLoaderDialog *TheLoaderDialog = NULL;

void ConvertRCMenu(BuildMenuItem *buildItem, rcMenuItem *rcItem);
void ConvertRCDialogItem(BuildDialogBoxData *dialogData, rcItem *item, rcDialog *rcd);
void ConvertDialogToPixels(int *x, int *y);

RCLoaderDialog::RCLoaderDialog(wxFrame *parent, char *title, Bool modal,
   int x, int y, int w, int h):
 wxDialogBox(parent, title, modal, x, y, w, h)
{
  wxBeginBusyCursor();

  SetLabelFont(SmallLabelFont);
  SetButtonFont(SmallButtonFont);
  SetLabelPosition(wxVERTICAL);
  (void)new wxButton(this, (wxFunction)RCLoaderQuit, "OK");
  (void)new wxButton(this, (wxFunction)RCLoaderLoad, "Load RC file");
  (void)new wxButton(this, (wxFunction)RCLoaderAdd, "Add RC file");
  (void)new wxButton(this, (wxFunction)RCLoaderHelp, "Help");
  NewLine();
  includeItem = new wxText(this, (wxFunction)NULL, "Include directories",
         buildApp.windowsInclude, -1, -1, 200);
                           
  NewLine();
  dialogList = new wxListBox(this, (wxFunction)RCLoaderDialogList, "Dialogs",
      wxSINGLE, -1, -1, 200, 150);
  NewLine();
  menuList = new wxListBox(this, (wxFunction)RCLoaderMenuList, "Menus",
      wxSINGLE, -1, -1, 200, 150);
  Fit();
  Centre();
  UpdateLists();

  wxEndBusyCursor();

  Show(TRUE);
}

Bool RCLoaderDialog::OnClose(void)
{
  if (buildApp.windowsInclude) delete[] buildApp.windowsInclude;
  buildApp.windowsInclude = copystring(includeItem->GetValue());
  return FALSE;
}
 
void RCLoaderDialog::UpdateLists(void)
{
  if (!TheRCFile) return;
  int nDialogs = TheRCFile->nDialogs();
  int nMenus = TheRCFile->nMenus();
  dialogList->Clear();
  menuList->Clear();
  rcDialog *rcd = NULL;
  rcMenuPopup *rcm = NULL;
  int i;
  for( i = 0 ; i < nDialogs ; ++i)
  {
    if( (rcd = TheRCFile->GetIthDialog( i)) != 0)
      dialogList->Append( (char *)rcd->dlgName());
  }
  for( i = 0 ; i < nMenus ; ++i)
  {
    if( (rcm = TheRCFile->GetIthMenu( i)) != 0)
      menuList->Append( (char *)rcm->GetName());
  }
}

void RCLoaderQuit(wxButton& but, wxCommandEvent& event)
{
  TheLoaderDialog->Show(FALSE);
  TheLoaderDialog->OnClose();
  delete TheLoaderDialog;
  TheLoaderDialog = NULL;
}

void RCLoaderHelp(wxButton& but, wxCommandEvent& event)
{
  wxBeginBusyCursor();
  HelpInstance->LoadFile();
  HelpInstance->KeywordSearch("Converting Windows resource files");
  wxEndBusyCursor();
}

void RCLoaderLoad(wxButton& but, wxCommandEvent& event)
{
  char *s = wxFileSelector( "Select RC file to load", 0, 0, "rc",
                                        "*.rc", wxOPEN, NULL);
  if (!s)
    return;

  wxBeginBusyCursor();

  if (TheRCFile)
    delete TheRCFile;
  TheRCFile = new rcFile;

  TheRCFile->Read(s, TheLoaderDialog->includeItem->GetValue());
  TheLoaderDialog->UpdateLists();

  wxEndBusyCursor();
}

void RCLoaderAdd(wxButton& but, wxCommandEvent& event)
{
  char *s = wxFileSelector( "Select RC file to add", 0, 0, "rc",
                                        "*.rc", wxOPEN, NULL);
  if (!s)
    return;
  wxBeginBusyCursor();

  if (!TheRCFile)
    TheRCFile = new rcFile;
  TheRCFile->Read(s, TheLoaderDialog->includeItem->GetValue());
  TheLoaderDialog->UpdateLists();

  wxEndBusyCursor();
}

void RCLoaderDialogList(wxListBox& lbox, wxCommandEvent& event)
{
}

void RCLoaderMenuList(wxListBox& lbox, wxCommandEvent& event)
{
}

void ShowRCLoader(void)
{
  if (TheLoaderDialog)
  {
    TheLoaderDialog->Iconize(FALSE);
    TheLoaderDialog->Show(TRUE);
    return;
  }
  TheLoaderDialog = new RCLoaderDialog(NULL, "Windows Resource Converter", FALSE, 0, 0, 500, 500);
}

void CloseRCLoader(void)
{
  if (TheLoaderDialog)
  {
    wxBeginBusyCursor();
    TheLoaderDialog->Show(FALSE);
    TheLoaderDialog->OnClose();
    delete TheLoaderDialog;
    TheLoaderDialog = NULL;
    wxEndBusyCursor();
  }
}

Bool ConvertMenuFromRC(BuildFrameData *frameData)
{
  if (!TheRCFile)
  {
    wxMessageBox("Please load your Windows resources first.", "Error");
    return FALSE;
  }
  int nMenus = TheRCFile->nMenus();

  if (nMenus == 0)
  {
    wxMessageBox("No menu resources are loaded yet.", "Error");
    return FALSE;
  }

  char **choices = new char *[nMenus];
  char **clientData = new char *[nMenus];

  rcMenuPopup *rcm = NULL;
  int i;
  for( i = 0 ; i < nMenus ; ++i)
  {
    if( (rcm = TheRCFile->GetIthMenu( i)) != 0)
    {
      choices[i] = (char *)rcm->GetName();
      clientData[i] = (char *)rcm;
    }
  }
  rcMenuPopup *ans = (rcMenuPopup *)wxGetSingleChoiceData("Pick a menu to convert",
      "Choose please", nMenus, choices, clientData);
  delete[] choices;
  delete[] clientData;

  if (!ans)
    return FALSE;

  if (frameData->buildMenuBar && (frameData->buildMenuBar->menus.Number() > 0))
  {
    if (wxYES == wxMessageBox("Override existing menu bar?", "Confirm", wxYES_NO))
    {
      delete frameData->buildMenuBar;
      frameData->buildMenuBar = new BuildMenuItem;
      frameData->buildMenuBar->menuType = BUILD_MENU_TYPE_MENU_BAR;
    }
    else return FALSE;
  }
  ConvertRCMenu(frameData->buildMenuBar, ans);
  int menuId = 1;
  frameData->buildMenuBar->GenerateIds(&menuId);
  return TRUE;
}

void ConvertRCMenu(BuildMenuItem *buildItem, rcMenuItem *rcItem)
{
  switch (buildItem->menuType)
  {
    case BUILD_MENU_TYPE_ITEM:
    {
      buildItem->menuId = rcItem->GetID();
      buildItem->menuString = copystring(rcItem->GetName());
      if (rcItem->HasNamedID())
        buildItem->menuIdName = copystring((char *)rcItem->szIdName());
      else
        buildItem->menuIdName = copystring(buildItem->MakeIdName());
      break;
    }
    case BUILD_MENU_TYPE_MENU:
    {
      buildItem->menuId = rcItem->GetID();
      buildItem->menuString = copystring(rcItem->GetName());
      if (rcItem->HasNamedID())
        buildItem->menuIdName = copystring((char *)rcItem->szIdName());
      else
        buildItem->menuIdName = copystring(buildItem->MakeIdName());
      break;
    }
    case BUILD_MENU_TYPE_SEPARATOR:
    {
      break;
    }
    case BUILD_MENU_TYPE_MENU_BAR:
    {
      break;
    }
    default:
      break;
  }
  if (((buildItem->menuType == BUILD_MENU_TYPE_MENU) ||
       (buildItem->menuType == BUILD_MENU_TYPE_MENU_BAR))
       && rcItem->IsPopup())
  {
    rcMenuPopup *rcPopup = (rcMenuPopup *)rcItem;
    int i;
    int nChildren = rcPopup->NChilds();
    for (i = 0; i < nChildren; i++)
    {
      rcMenuItem *rcChild = rcItem->rcGetIthChild(i);
      if (!rcChild->IsMenuBarBreak())
      {
        BuildMenuItem *buildChild = new BuildMenuItem;

        if (rcChild->IsPopup())
        {
          buildChild->menuType = BUILD_MENU_TYPE_MENU;
        }
        else if (rcChild->IsSeparator() || rcChild->IsMenuBreak())
        {
          buildChild->menuType = BUILD_MENU_TYPE_SEPARATOR;
        }
        else /* Must be normal menu item */
        {
          buildChild->menuType = BUILD_MENU_TYPE_ITEM;
        }
        buildItem->menus.Append(buildChild);
        buildChild->parent = buildItem;
        ConvertRCMenu(buildChild, rcChild);
      }
    }
  }
}

BuildDialogBoxData *ConvertDialogFromRC(void)
{
  if (!TheRCFile)
  {
    wxMessageBox("Please load your Windows resources first.", "Error");
    return NULL;
  }
  int nDialogs = TheRCFile->nDialogs();

  if (nDialogs == 0)
  {
    wxMessageBox("No dialog resources are loaded yet.", "Error");
    return NULL;
  }

  char **choices = new char *[nDialogs];
  char **clientData = new char *[nDialogs];

  rcDialog *rcd = NULL;
  int i;
  for( i = 0 ; i < nDialogs ; ++i)
  {
    if( (rcd = TheRCFile->GetIthDialog( i)) != 0)
    {
      choices[i] = (char *)rcd->dlgName();
      clientData[i] = (char *)rcd;
    }
  }
  rcDialog *ans = (rcDialog *)wxGetSingleChoiceData("Pick a dialog to convert",
      "Choose please", nDialogs, choices, clientData);
  delete[] choices;
  delete[] clientData;

  if (!ans)
    return NULL;

  wxBeginBusyCursor();

  BuildDialogBoxData *dialogData = new BuildDialogBoxData(NULL);
  dialogData->title = copystring(ans->dlgName());

  int x = ans->left();
  int y = ans->top();
  int width = ans->width();
  int height = ans->height();
  ConvertDialogToPixels(&x, &y);
  ConvertDialogToPixels(&width, &height);
  dialogData->x = x; dialogData->y = y;
  dialogData->width = width; dialogData->height = height;

  int nItems = ans->nItems();
  for (i = 0; i < nItems; i++)
  {
    rcItem *item = (rcItem *)ans->getIthItem(i);
    ConvertRCDialogItem(dialogData, item, ans);
  }

  dialogData->MakeRealWindow();
  // Width and height from resouce file is actually
  // _client_ width/height
  dialogData->userWindow->SetClientSize(width, height);
  dialogData->userWindow->GetSize(&dialogData->width, &dialogData->height);

  buildApp.topLevelWindows.Append(dialogData);
  buildApp.UpdateWindowList();

  buildApp.ShowObjectEditor(dialogData);

  MakeModified();

  wxEndBusyCursor();

  return dialogData;
}

void ConvertRCDialogItem(BuildDialogBoxData *dialogData, rcItem *item, rcDialog *rcd)
{
  int x = item->left();
  int y = item->top();
  int width = item->width();
  int height = item->height();
  ConvertDialogToPixels(&x, &y);
  ConvertDialogToPixels(&width, &height);
  char *label = (char *)item->label();
  
  switch (item->ItemType())
  {
    case rcItem::rcCHECKBOX:
    {
      BuildCheckBoxData *buildItem = new BuildCheckBoxData(dialogData);
      buildItem->x = x; buildItem->y = y;
      buildItem->width = width; buildItem->height = height;
      buildItem->autoSize = FALSE;
      if (buildItem->title) delete[] buildItem->title;

      if (label) buildItem->title = copystring(label);
      else buildItem->title = NULL;
      break;
    }
    case rcItem::rcCOMBOBOX:
    {
      BuildChoiceData *buildItem = new BuildChoiceData(dialogData);
      buildItem->x = x; buildItem->y = y;
      buildItem->width = width; buildItem->height = height;
      buildItem->autoSize = FALSE;
      if (buildItem->title) delete[] buildItem->title;
      if (label) buildItem->title = copystring(label);
      else buildItem->title = NULL;
      break;
    }
    case rcItem::rcEDITTEXT:
    {
      BuildItemData *buildItem = NULL;
      if (height > 30) // a bit arbitrary...
        buildItem = new BuildMultiTextData(dialogData);
      else
        buildItem = new BuildTextData(dialogData);
      buildItem->x = x; buildItem->y = y;
      buildItem->width = width; buildItem->height = height;
      buildItem->autoSize = FALSE;
      if (buildItem->title) delete[] buildItem->title;
      if (label) buildItem->title = copystring(label);
      else buildItem->title = NULL;
      break;
    }
    case rcItem::rcGROUPBOX:
    {
      // Try to construct a radiobox within this group.
      int nItems = rcd->nItems();
      wxList radioButtons;
      for (int i = 0; i < nItems; i++)
      {
        rcItem *rItem = (rcItem *)rcd->getIthItem(i);
        if (rItem->ItemType() == rcItem::rcRADIOBUTTON)
        {
          int rx = rItem->left();
          int ry = rItem->top();
          int rWidth = rItem->width();
          int rHeight = rItem->height();
          ConvertDialogToPixels(&rx, &ry);
          ConvertDialogToPixels(&rWidth, &rHeight);
          if ((rx >= x) && (ry >=y) && (rx <= (x + width)) && (ry <= (y + height)))
          {
            radioButtons.Append(rItem);
          }
        }
      }
      if (radioButtons.Number() > 0)
      {
        BuildRadioBoxData *buildItem = new BuildRadioBoxData(dialogData);
        buildItem->x = x; buildItem->y = y;
        buildItem->width = -1; buildItem->height = -1;
        buildItem->autoSize = TRUE;
        buildItem->noRowsCols = radioButtons.Number();
        if (buildItem->title) delete[] buildItem->title;

        if (label) buildItem->title = copystring(label);
        else buildItem->title = NULL;
        wxNode *node = radioButtons.First();
        while (node)
        {
          rcItem *it = (rcItem *)node->Data();
          if (it->label())
            buildItem->strings.Add((char *)it->label());
          node = node->Next();
        }
      }
      else // Must be a normal group box
      {
        BuildGroupBoxData *buildItem = new BuildGroupBoxData(dialogData);
        buildItem->x = x; buildItem->y = y;
        buildItem->width = width; buildItem->height = height;
        buildItem->autoSize = FALSE;
        if (buildItem->title) delete[] buildItem->title;
        if (label) buildItem->title = copystring(label);
        else buildItem->title = NULL;
      }
      
      break;
    }
    case rcItem::rcICON:
    {
      break;
    }
    case rcItem::rcLISTBOX:
    {
      BuildListBoxData *buildItem = new BuildListBoxData(dialogData);
      buildItem->x = x; buildItem->y = y;
      buildItem->width = width; buildItem->height = height;
      buildItem->autoSize = FALSE;
      if (buildItem->title) delete[] buildItem->title;

      if (label) buildItem->title = copystring(label);
      else buildItem->title = NULL;
      break;
    }
    case rcItem::rcDEFPUSHBUTTON:
    case rcItem::rcPUSHBUTTON:
    {
      BuildButtonData *buildItem = new BuildButtonData(dialogData);
      buildItem->x = x; buildItem->y = y;
      buildItem->width = width; buildItem->height = height;
      buildItem->autoSize = FALSE;
      if (buildItem->title) delete[] buildItem->title;

      if (label) buildItem->title = copystring(label);
      else buildItem->title = NULL;
      break;
    }
    case rcItem::rcRADIOBUTTON:
    {
      break;
    }
    case rcItem::rcSCROLLBAR:
    {
      break;
    }
    case rcItem::rcRTEXT:
    case rcItem::rcCTEXT:
    case rcItem::rcLTEXT:
    case rcItem::rcSTATIC:
    {
      BuildMessageData *buildItem = new BuildMessageData(dialogData);
      buildItem->x = x; buildItem->y = y;
      buildItem->width = width; buildItem->height = height;
      buildItem->autoSize = FALSE;
      if (buildItem->title) delete[] buildItem->title;
      if (label) buildItem->title = copystring(label);
      else buildItem->title = NULL;
      break;
    }
    default:
      break;
  }
}

void ConvertDialogToPixels(int *x, int *y)
{
#ifdef wx_msw
  DWORD word = GetDialogBaseUnits();
  int xs = LOWORD(word);
  int ys = HIWORD(word);
  *x = (int)((*x * xs)/4);
  *y = (int)((*y * ys)/8);
#endif
#ifdef wx_x
#endif
}

