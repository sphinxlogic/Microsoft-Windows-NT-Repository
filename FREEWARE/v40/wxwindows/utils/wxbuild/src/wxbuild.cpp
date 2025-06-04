/*
 * File:	wxbuild.cc
 * Purpose:	wxWindows GUI builder
 * Author:	Julian Smart
 * Created:	1994
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
#include <ctype.h>
#include <stdlib.h>
#if !defined(USE_IOSTREAMH) || USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#include "wxbuild.h"
#include "namegen.h"
#include "bwin.h"
#include "bapp.h"
#include "bframe.h"
#include "bmenu.h"
#include "bsubwin.h"
#include "btoolbar.h"
#include "bitem.h"
#include "btree.h"
#include "bgdi.h"
#include "rcload.h"

float buildVersion = 3.0;

#ifdef wx_x
#define DEFAULT_EDITOR "emacs"
#endif
#ifdef wx_msw
#define DEFAULT_EDITOR "notepad"
#endif

#ifdef wx_x
#include "bitmaps/wxbuild.xbm"
#endif

BuildFrame   *MainFrame = NULL;
ReportFrame *TheReportFrame = NULL;
EditorToolPaletteFrame *EditorPaletteFrame = NULL;
wxCursor *handCursor = NULL;
wxCursor *crossCursor = NULL;
wxFont *SmallButtonFont = NULL;
wxFont *SmallLabelFont = NULL;
wxFont *TextWindowFont = NULL;

// File menu: we need the handle for the file history.
wxMenu *fileMenu = NULL;

extern BuildWindowData *NewObject; // Object just created

wxHelpInstance *HelpInstance = NULL;
wxPathList BuildPathList;

wxList BuildSelections;  // Currently selected windows
Bool isModified = FALSE;

// Define my frame constructor
BuildFrame::BuildFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
  buildWindowsItem = NULL;
  panel = NULL;
  canvas = NULL;
  toolbar = NULL;
  currentWindow = NULL;
  
  CreateStatusLine(2);
  if (buildApp.TestMode())
    SetStatusText("Test mode", 1);
  else
    SetStatusText("Edit mode", 1);

  // Give it an icon
#ifdef wx_msw
  wxIcon *icon = new wxIcon("wxbuild");
#endif
#ifdef wx_x
  wxIcon *icon = new wxIcon(wxbuild_bits, wxbuild_width, wxbuild_height);
#endif

  SetIcon(icon);

  // Make a menubar
  fileMenu = new wxMenu;

  fileMenu->Append(BUILD_NEW_PROJECT, "&New project");
  fileMenu->AppendSeparator();
  fileMenu->Append(BUILD_OPEN, "&Open project");
  fileMenu->Append(BUILD_SAVE, "Save &project");
  fileMenu->Append(BUILD_SAVE_AS, "&Save as...");
  fileMenu->AppendSeparator();

//  fileMenu->Append(BUILD_DELETE_WINDOW, "&Delete window");
  fileMenu->Append(BUILD_APPLICATION_SETTINGS, "Edit application &properties");
  fileMenu->Append(BUILD_SHOW_TOPLEVEL_FRAME, "Show &top level frame");
  fileMenu->AppendSeparator();

  fileMenu->Append(BUILD_EXIT, "&Exit");

  if (buildApp.fileHistoryN > 0)
  {
    fileMenu->AppendSeparator();
    for (int i = 0; i < buildApp.fileHistoryN; i++)
    {
      char buf[400];
      sprintf(buf, "&%d %s", i+1, buildApp.fileHistory[i]);
      fileMenu->Append(BUILD_FILE1+i, buf);
    }
  }

  wxMenu *edit_menu = new wxMenu;

  edit_menu->Append(OBJECT_EDITOR_NEW_FRAME, "New &frame");
  edit_menu->Append(OBJECT_EDITOR_NEW_DIALOG, "New &dialog");
  edit_menu->AppendSeparator();
  wxMenu *subWindowMenu = new wxMenu;
  subWindowMenu->Append(OBJECT_EDITOR_NEW_PANEL, "New &panel");
  subWindowMenu->Append(OBJECT_EDITOR_NEW_CANVAS, "New &canvas");
  subWindowMenu->Append(OBJECT_EDITOR_NEW_TEXT_WINDOW, "New text &window");
  edit_menu->Append(OBJECT_EDITOR_NEW_SUBWINDOW, "New &subwindow...", subWindowMenu);
  wxMenu *itemMenu = new wxMenu;
  itemMenu->Append(OBJECT_EDITOR_NEW_BUTTON, "New &button");
  itemMenu->Append(OBJECT_EDITOR_NEW_CHECKBOX, "New checkbo&x");
  itemMenu->Append(OBJECT_EDITOR_NEW_MESSAGE, "New &message");
  itemMenu->Append(OBJECT_EDITOR_NEW_CHOICE, "New cho&ice");
  itemMenu->Append(OBJECT_EDITOR_NEW_GAUGE, "New ga&uge");
  itemMenu->Append(OBJECT_EDITOR_NEW_GROUPBOX, "New &groupbox");
  itemMenu->Append(OBJECT_EDITOR_NEW_LISTBOX, "New &listbox");
  itemMenu->Append(OBJECT_EDITOR_NEW_RADIOBOX, "New &radiobox");
  itemMenu->Append(OBJECT_EDITOR_NEW_SLIDER, "New &slider");
  itemMenu->Append(OBJECT_EDITOR_NEW_TEXT, "New &text");
  itemMenu->Append(OBJECT_EDITOR_NEW_MULTITEXT, "New &multitext");
  edit_menu->Append(OBJECT_EDITOR_NEW_ITEM, "New &item...", itemMenu);
  edit_menu->AppendSeparator();

  edit_menu->Append(OBJECT_EDITOR_EDIT_MENU, "Edit men&u bar");
  edit_menu->Append(OBJECT_EDITOR_EDIT_TOOLBAR, "Edit t&ool bar");
  edit_menu->Append(OBJECT_EDITOR_EDIT_ATTRIBUTES, "Edit &properties for selection");
  edit_menu->AppendSeparator();
#ifdef wx_msw
  wxMenu *convertMenu = new wxMenu;
  convertMenu->Append(BUILD_SHOW_RCLOADER, "Load Windows &resources");
  convertMenu->AppendSeparator();
  convertMenu->Append(OBJECT_EDITOR_RC_CONVERT_MENU, "Convert &menu bar");
  convertMenu->Append(OBJECT_EDITOR_RC_CONVERT_DIALOG, "Convert &dialog");
  edit_menu->Append(OBJECT_EDITOR_RC_CONVERT, "&Windows resource conversion",
     convertMenu);
  edit_menu->AppendSeparator();
#endif
//  edit_menu->Append(OBJECT_EDITOR_DELETE_OBJECT, "&Delete object");
  edit_menu->Append(OBJECT_EDITOR_CLOSE_OBJECT, "&Close object");
  edit_menu->AppendSeparator();
  edit_menu->Append(OBJECT_EDITOR_GRID, "&Grid settings...");
  edit_menu->AppendSeparator();
  edit_menu->Append(OBJECT_EDITOR_TOGGLE_TEST_MODE, "&Toggle test/edit mode");

  wxMenu *gdiMenu = new wxMenu;
  gdiMenu->Append(BUILD_GDI_FONTS, "&Font manager");
/*
  wxMenu *zoom_menu = new wxMenu;
  zoom_menu->Append(ZOOM_30, "&30%");
  zoom_menu->Append(ZOOM_40, "&40%");
  zoom_menu->Append(ZOOM_50, "&50%");
  zoom_menu->Append(ZOOM_60, "&60%");
  zoom_menu->Append(ZOOM_70, "&70%");
  zoom_menu->Append(ZOOM_80, "&80%");
  zoom_menu->Append(ZOOM_90, "&90%");
  zoom_menu->Append(ZOOM_100, "&100%");

  zoom_menu->Check(ZOOM_70, TRUE);
*/
  currentZoom = ZOOM_70;

  wxMenu *generate_menu = new wxMenu;
  generate_menu->Append(BUILD_PROJECT_SETTINGS, "Edit &project settings");
  generate_menu->Append(BUILD_GLOBAL_SETTINGS, "Edit &global settings");
  generate_menu->AppendSeparator();
  generate_menu->Append(BUILD_GENERATE_CPP, "&Generate C++");
//  generate_menu->Append(BUILD_GENERATE_CLIPS, "Generate &CLIPS");
//  generate_menu->Append(BUILD_GENERATE_RC, "Generate &RC");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(BUILD_HELP_CONTENTS, "&Contents");
  help_menu->AppendSeparator();
  help_menu->Append(BUILD_ABOUT, "&About wxBuilder...");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(fileMenu, "&File");
  menu_bar->Append(edit_menu, "&Edit");
  menu_bar->Append(gdiMenu, "GD&I");
//  menu_bar->Append(zoom_menu, "&Zoom");
  menu_bar->Append(generate_menu, "&Project");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  SetMenuBar(menu_bar);

  canvas = new ObjectEditorCanvas(this);
//  canvas->SetSnapToGrid(TRUE);
  canvas->SetScrollbars(20, 20, 50, 50, 4, 4);
//  canvas->GetDC()->SetUserScale(0.5, 0.5);

  panel = new wxPanel(this, 0, 0, 400, 400, 0);
  panel->SetLabelPosition(wxVERTICAL);
  panel->SetLabelFont(SmallLabelFont);
  panel->SetButtonFont(SmallButtonFont);

  buildWindowsItem = new wxListBox(panel, (wxFunction)TopLevelWindowsProc, "Top Level Windows", wxSINGLE,
                         -1, -1, 370, 100);
//  Centre();
  Show(TRUE);
  OnSize(w, h);
}

void BuildFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case BUILD_NEW_PROJECT:
    {
      if (buildApp.Modified())
      {
        if (wxMessageBox("Discard modified project?", "Respond", wxYES_NO) == wxNO)
          return;
      }
      buildApp.ClearProject();
      break;
    }
    case BUILD_OPEN:
    {
      buildApp.LoadProject(buildApp.projectFilename);
      break;
    }
    case BUILD_SAVE:
    {
      buildApp.SaveProject(buildApp.projectFilename);
      break;
    }
    case BUILD_SAVE_AS:
    {
      buildApp.SaveProject(NULL);
      break;
    }
    case BUILD_FILE1:
    case BUILD_FILE2:
    case BUILD_FILE3:
    case BUILD_FILE4:
    case BUILD_FILE5:
    {
      buildApp.LoadProject(buildApp.fileHistory[id-BUILD_FILE1], FALSE);
      break;
    }
    case BUILD_EXIT:
    {
      this->Close();
      break;
    }
#ifdef wx_msw
    case BUILD_SHOW_RCLOADER:
    {
      ShowRCLoader();
      break;
    }
#endif
    case BUILD_GDI_FONTS:
    {
      TheFontManager.Show(TRUE);
      break;
    }
    case BUILD_HELP_CONTENTS:
    {
      wxBeginBusyCursor();
      HelpInstance->LoadFile();
      HelpInstance->DisplayContents();
      wxEndBusyCursor();
      break;
    }

    case BUILD_ABOUT:
    {
      char buf[400];
      sprintf(buf, "wxBuilder version %.2f\n", buildVersion);
      strcat(buf, "by Julian Smart, AIAI (c) 1994\n");
      strcat(buf, "J.Smart@ed.ac.uk");
      wxMessageBox(buf, "About wxBuilder");
      break;
    }
    case BUILD_APPLICATION_SETTINGS:
    {
      ShowAppEditor();
      break;
    }
    case BUILD_PROJECT_SETTINGS:
    {
      buildApp.EditProjectSettings();
      break;
    }
    case BUILD_GLOBAL_SETTINGS:
    {
      buildApp.EditGlobalSettings();
      break;
    }
    case OBJECT_EDITOR_GRID:
    {
      wxBeginBusyCursor();
      wxDialogBox *dialog = new wxDialogBox(this, "Grid settings", TRUE, 10, 10, 500, 500);
      dialog->SetLabelFont(SmallLabelFont);
      dialog->SetButtonFont(SmallButtonFont);

      BuildForm *form = new BuildForm("Grid settings");
//      form->Add(wxMakeFormFloat("Grid spacing", &canvas->grid_spacing, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
//                 200));
      form->AssociatePanel(dialog);
      form->dialog = dialog;

      dialog->Fit();
      dialog->Centre(wxBOTH);


      wxEndBusyCursor();
      dialog->Show(TRUE);
      canvas->Redraw();
      break;
    }
/*
    case BUILD_NEW_TOPLEVEL_FRAME:
    {
      Bool ok = TRUE;
      if (buildApp.topLevelFrame)
      {
        if (wxYES == wxMessageBox("Override current top level frame?",
                                    "Warning", wxYES_NO))
          ok = TRUE;
        else ok = FALSE;
      }
      if (ok)
      {
        BuildFrameData *win = new BuildFrameData(NULL);
        win->MakeRealWindow();

        buildApp.topLevelFrame = win;
        buildApp.topLevelWindows.Append(win);
        buildApp.UpdateWindowList(win);
        buildApp.ShowObjectEditor(win);
      }
      break;
    }
*/
    case BUILD_SHOW_TOPLEVEL_FRAME:
    {
      if (!buildApp.topLevelFrame)
      {
        (void)wxMessageBox("No top level frame!", "Warning", wxOK);
      }
      else
      {
        wxBeginBusyCursor();
        buildApp.ShowObjectEditor(buildApp.topLevelFrame);
        buildApp.topLevelFrame->userWindow->Show(TRUE);
        wxEndBusyCursor();
      }
      break;
    }
/*
    case BUILD_DELETE_WINDOW:
    {
      BuildWindowData *win = buildApp.FindCurrentWindow();
      if (!win)
        win = MainFrame->currentWindow;
      if (win)
        buildApp.DeleteWindow(win);
      break;
    }
*/
    case BUILD_GENERATE_CPP:
    {
      buildApp.GenerateCPP();
      break;
    }
    case BUILD_GENERATE_CLIPS:
    {
      break;
    }
    case BUILD_GENERATE_RC:
    {
      break;
    }
    case OBJECT_EDITOR_CLOSE_OBJECT:
    {
      if (currentWindow)
      {
        wxBeginBusyCursor();
        BuildWindowData *curr = currentWindow;
        buildApp.DisassociateObjectWithEditor(currentWindow);
        curr->DestroyRealWindow();
        curr->NullUserWindows();
        wxEndBusyCursor();
      }
      break;
    }
    case OBJECT_EDITOR_DELETE_OBJECT:
    {
      if (!currentWindow)
        return;
      wxBeginBusyCursor();
      int noSelections = 0;
      if (currentWindow)
        DisplayTree(TRUE, currentWindow);
      wxEndBusyCursor();
      break;
    }
    case OBJECT_EDITOR_NEW_FRAME:
    {
      wxBeginBusyCursor();
      BuildFrameData *win = new BuildFrameData(NULL);
      win->MakeRealWindow();

      if (!buildApp.topLevelFrame)
        buildApp.topLevelFrame = win;
      buildApp.topLevelWindows.Append(win);
      buildApp.UpdateWindowList();

      buildApp.ShowObjectEditor(win);
      NewObject = win;

      MakeModified();
      DisplayTree(TRUE, win);
      wxEndBusyCursor();

      break;
    }
    case OBJECT_EDITOR_NEW_DIALOG:
    {
      wxBeginBusyCursor();
      BuildDialogBoxData *win = new BuildDialogBoxData(NULL);
      win->MakeRealWindow();

      buildApp.topLevelWindows.Append(win);
      buildApp.UpdateWindowList();

      buildApp.ShowObjectEditor(win);
      NewObject = win;

      MakeModified();
      DisplayTree(TRUE, win);
      wxEndBusyCursor();

      break;
    }
    case OBJECT_EDITOR_NEW_PANEL:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();

      // Not really necessary since there's only one frame in
      // the editor.
      BuildWindowData *data = canvas->FindSelectionOfType(wxTYPE_FRAME);

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_FRAME)
          data = currentWindow;

      if (data)
      {
        BuildFrameData *frameData = (BuildFrameData *)data;
        BuildPanelData *win = new BuildPanelData(frameData);
        win->MakeRealWindow();
        int w, h;
        frameData->userWindow->GetClientSize(&w, &h);
        frameData->userWindow->OnSize(w, h);
        canvas->Redraw();
        NewObject = win;

        MakeModified();
        DisplayTree(TRUE, currentWindow);
        wxEndBusyCursor();
      }
      else
      {
        wxEndBusyCursor();
        wxMessageBox("Cannot add a panel to this object.", "Error");
        return;
      }
      break;
    }
    case OBJECT_EDITOR_NEW_CANVAS:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();

      // Not really necessary since there's only one frame in
      // the editor.
      BuildWindowData *data = canvas->FindSelectionOfType(wxTYPE_FRAME);

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_FRAME)
          data = currentWindow;

      if (data)
      {
        BuildFrameData *frameData = (BuildFrameData *)data;
        BuildCanvasData *win = new BuildCanvasData(frameData);
        win->MakeRealWindow();
        int w, h;
        frameData->userWindow->GetClientSize(&w, &h);
        frameData->userWindow->OnSize(w, h);
        canvas->Redraw();
        NewObject = win;
        MakeModified();
        DisplayTree(TRUE, currentWindow);
        wxEndBusyCursor();
      }
      else
      {
        wxEndBusyCursor();
        wxMessageBox("Cannot add a canvas to this object.", "Error");
        return;
      }
      break;
    }
    case OBJECT_EDITOR_NEW_TEXT_WINDOW:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();

      // Not really necessary since there's only one frame in
      // the editor.
      BuildWindowData *data = canvas->FindSelectionOfType(wxTYPE_FRAME);

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_FRAME)
          data = currentWindow;

      if (data)
      {
        BuildFrameData *frameData = (BuildFrameData *)data;
        BuildTextWindowData *win = new BuildTextWindowData(frameData);
        win->MakeRealWindow();
        int w, h;
        frameData->userWindow->GetClientSize(&w, &h);
        frameData->userWindow->OnSize(w, h);
        canvas->Redraw();
        NewObject = win;
        MakeModified();
        DisplayTree(TRUE, currentWindow);
        wxEndBusyCursor();
      }
      else
      {
        wxEndBusyCursor();
        wxMessageBox("Cannot add a text window to this object.", "Error");
        return;
      }
      break;
    }
    case OBJECT_EDITOR_NEW_BUTTON:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();

      BuildWindowData *data = canvas->FindSuitableItemParent();

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_DIALOG_BOX)
          data = currentWindow;

      if (data)
      {
        BuildPanelData *panelData = (BuildPanelData *)data;
        BuildButtonData *win = new BuildButtonData(panelData);
        if (buildApp.GetUsePosition())
        {
          win->x = buildApp.GetObjectX(); win->y = buildApp.GetObjectY();
        }
        win->MakeRealWindow();
        canvas->Redraw();
        NewObject = win;
        MakeModified();
        DisplayTree(TRUE, currentWindow);
#ifdef wx_x
        panelData->userWindow->Refresh();
#endif
        wxEndBusyCursor();
      }
      else
      {
        wxMessageBox("Cannot add a button to this object.", "Error");
        wxEndBusyCursor();
        return;
      }
      break;
    }
    case OBJECT_EDITOR_NEW_CHECKBOX:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();
      BuildWindowData *data = canvas->FindSuitableItemParent();

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_DIALOG_BOX)
          data = currentWindow;

      if (data)
      {
        BuildPanelData *panelData = (BuildPanelData *)data;
        BuildCheckBoxData *win = new BuildCheckBoxData(panelData);
        if (buildApp.GetUsePosition())
        {
          win->x = buildApp.GetObjectX(); win->y = buildApp.GetObjectY();
        }
        win->MakeRealWindow();
        canvas->Redraw();
        NewObject = win;
        MakeModified();
        DisplayTree(TRUE, currentWindow);
#ifdef wx_x
        panelData->userWindow->Refresh();
#endif
        wxEndBusyCursor();
      }
      else
      {
        wxEndBusyCursor();
        wxMessageBox("Cannot add a checkbox to this object.", "Error");
        return;
      }
      break;
    }
    case OBJECT_EDITOR_NEW_MESSAGE:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();
      BuildWindowData *data = canvas->FindSuitableItemParent();

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_DIALOG_BOX)
          data = currentWindow;

      if (data)
      {
        BuildPanelData *panelData = (BuildPanelData *)data;
        BuildMessageData *win = new BuildMessageData(panelData);
        if (buildApp.GetUsePosition())
        {
          win->x = buildApp.GetObjectX(); win->y = buildApp.GetObjectY();
        }
        win->MakeRealWindow();
        canvas->Redraw();
        NewObject = win;
        DisplayTree(TRUE, currentWindow);
        MakeModified();
#ifdef wx_x
        panelData->userWindow->Refresh();
#endif
        wxEndBusyCursor();
      }
      else
      {
        wxEndBusyCursor();
        wxMessageBox("Cannot add a message to this object.", "Error");
        return;
      }
      break;
    }
    case OBJECT_EDITOR_NEW_CHOICE:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();
      BuildWindowData *data = canvas->FindSuitableItemParent();

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_DIALOG_BOX)
          data = currentWindow;

      if (data)
      {
        BuildPanelData *panelData = (BuildPanelData *)data;
        BuildChoiceData *win = new BuildChoiceData(panelData);
        if (buildApp.GetUsePosition())
        {
          win->x = buildApp.GetObjectX(); win->y = buildApp.GetObjectY();
        }
        win->MakeRealWindow();
        canvas->Redraw();
        NewObject = win;
        DisplayTree(TRUE, currentWindow);
        MakeModified();
#ifdef wx_x
        panelData->userWindow->Refresh();
#endif
        wxEndBusyCursor();
      }
      else
      {
        wxEndBusyCursor();
        wxMessageBox("Cannot add a choice to this object.", "Error");
        return;
      }
      break;
    }
    case OBJECT_EDITOR_NEW_LISTBOX:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();
      BuildWindowData *data = canvas->FindSuitableItemParent();

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_DIALOG_BOX)
          data = currentWindow;

      if (data)
      {
        BuildPanelData *panelData = (BuildPanelData *)data;
        BuildListBoxData *win = new BuildListBoxData(panelData);
        if (buildApp.GetUsePosition())
        {
          win->x = buildApp.GetObjectX(); win->y = buildApp.GetObjectY();
        }
        win->MakeRealWindow();
        canvas->Redraw();
        NewObject = win;
        DisplayTree(TRUE, currentWindow);
        MakeModified();
#ifdef wx_x
        panelData->userWindow->Refresh();
#endif
        wxEndBusyCursor();
      }
      else
      {
        wxEndBusyCursor();
        wxMessageBox("Cannot add a listbox to this object.", "Error");
        return;
      }
      break;
    }
    case OBJECT_EDITOR_NEW_RADIOBOX:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();
      BuildWindowData *data = canvas->FindSuitableItemParent();

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_DIALOG_BOX)
          data = currentWindow;

      if (data)
      {
        BuildPanelData *panelData = (BuildPanelData *)data;
        BuildRadioBoxData *win = new BuildRadioBoxData(panelData);
        if (buildApp.GetUsePosition())
        {
          win->x = buildApp.GetObjectX(); win->y = buildApp.GetObjectY();
        }
        win->strings.Add("One");
        win->strings.Add("Two");
        win->MakeRealWindow();
        canvas->Redraw();
        NewObject = win;
        DisplayTree(TRUE, currentWindow);
        MakeModified();
#ifdef wx_x
        panelData->userWindow->Refresh();
#endif
        wxEndBusyCursor();
      }
      else
      {
        wxEndBusyCursor();
        wxMessageBox("Cannot add a radiobox to this object.", "Error");
        return;
      }
      break;
    }
    case OBJECT_EDITOR_NEW_SLIDER:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();
      BuildWindowData *data = canvas->FindSuitableItemParent();

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_DIALOG_BOX)
          data = currentWindow;

      if (data)
      {
        BuildPanelData *panelData = (BuildPanelData *)data;
        BuildSliderData *win = new BuildSliderData(panelData);
        if (buildApp.GetUsePosition())
        {
          win->x = buildApp.GetObjectX(); win->y = buildApp.GetObjectY();
        }
        win->MakeRealWindow();
        canvas->Redraw();
        NewObject = win;
        DisplayTree(TRUE, currentWindow);
        MakeModified();
#ifdef wx_x
        panelData->userWindow->Refresh();
#endif
        wxEndBusyCursor();
      }
      else
      {
        wxEndBusyCursor();
        wxMessageBox("Cannot add a slider to this object.", "Error");
        return;
      }

      break;
    }
    case OBJECT_EDITOR_NEW_GAUGE:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();
      BuildWindowData *data = canvas->FindSuitableItemParent();

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_DIALOG_BOX)
          data = currentWindow;

      if (data)
      {
        BuildPanelData *panelData = (BuildPanelData *)data;
        BuildGaugeData *win = new BuildGaugeData(panelData);
        if (buildApp.GetUsePosition())
        {
          win->x = buildApp.GetObjectX(); win->y = buildApp.GetObjectY();
        }
        win->MakeRealWindow();
        canvas->Redraw();
        NewObject = win;
        DisplayTree(TRUE, currentWindow);
        MakeModified();
#ifdef wx_x
        panelData->userWindow->Refresh();
#endif
        wxEndBusyCursor();
      }
      else
      {
        wxEndBusyCursor();
        wxMessageBox("Cannot add a gauge to this object.", "Error");
        return;
      }

      break;
    }
    case OBJECT_EDITOR_NEW_GROUPBOX:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();
      BuildWindowData *data = canvas->FindSuitableItemParent();

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_DIALOG_BOX)
          data = currentWindow;

      if (data)
      {
        BuildPanelData *panelData = (BuildPanelData *)data;
        BuildGroupBoxData *win = new BuildGroupBoxData(panelData);
        if (buildApp.GetUsePosition())
        {
          win->x = buildApp.GetObjectX(); win->y = buildApp.GetObjectY();
        }
        win->MakeRealWindow();
        canvas->Redraw();
        NewObject = win;
        DisplayTree(TRUE, currentWindow);
        MakeModified();
#ifdef wx_x
        panelData->userWindow->Refresh();
#endif
        wxEndBusyCursor();
      }
      else
      {
        wxEndBusyCursor();
        wxMessageBox("Cannot add a groupbox to this object.", "Error");
        return;
      }

      break;
    }
    case OBJECT_EDITOR_NEW_TEXT:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();
      BuildWindowData *data = canvas->FindSuitableItemParent();

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_DIALOG_BOX)
          data = currentWindow;

      if (data)
      {
        BuildPanelData *panelData = (BuildPanelData *)data;
        BuildTextData *win = new BuildTextData(panelData);
        if (buildApp.GetUsePosition())
        {
          win->x = buildApp.GetObjectX(); win->y = buildApp.GetObjectY();
        }
        win->MakeRealWindow();
        canvas->Redraw();
        NewObject = win;
        DisplayTree(TRUE, currentWindow);
        MakeModified();
#ifdef wx_x
        panelData->userWindow->Refresh();
#endif
        wxEndBusyCursor();
      }
      else
      {
        wxEndBusyCursor();
        wxMessageBox("Cannot add a text item to this object.", "Error");
        return;
      }
      break;
    }
    case OBJECT_EDITOR_NEW_MULTITEXT:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();
      BuildWindowData *data = canvas->FindSuitableItemParent();

      // However, we might be editing a dialog...
      if (!data)
        if (currentWindow->windowType == wxTYPE_DIALOG_BOX)
          data = currentWindow;

      if (data)
      {
        BuildPanelData *panelData = (BuildPanelData *)data;
        BuildMultiTextData *win = new BuildMultiTextData(panelData);
        if (buildApp.GetUsePosition())
        {
          win->x = buildApp.GetObjectX(); win->y = buildApp.GetObjectY();
        }
        win->MakeRealWindow();
        canvas->Redraw();
        NewObject = win;
        DisplayTree(TRUE, currentWindow);
        MakeModified();
#ifdef wx_x
        panelData->userWindow->Refresh();
#endif
        wxEndBusyCursor();
      }
      else
      {
        wxEndBusyCursor();
        wxMessageBox("Cannot add a multitext item to this object.", "Error");
        return;
      }
      break;
    }
    case OBJECT_EDITOR_EDIT_MENU:
    {
      if (!currentWindow)
        return;

      if (currentWindow->windowType == wxTYPE_FRAME)
      {
        BuildFrameData *frameData = (BuildFrameData *)currentWindow;
        if (frameData->buildMenuBar)
        {
          wxBeginBusyCursor();
          ShowMenuBarEditor(frameData, frameData->buildMenuBar);
          frameData->DestroyRealWindow();
          frameData->MakeRealWindow();
          buildApp.AssociateObjectWithEditor(frameData);
          MakeModified();
          wxEndBusyCursor();
        }
      }
      break;
    }
    case OBJECT_EDITOR_EDIT_TOOLBAR:
    {
      if (!currentWindow)
        return;

      if (currentWindow->windowType == wxTYPE_FRAME)
      {
        BuildFrameData *frameData = (BuildFrameData *)currentWindow;
        wxBeginBusyCursor();
        if (!frameData->toolbar)
        {
          frameData->toolbar = new BuildToolbarData(frameData, FALSE);
          frameData->toolbar->MakeRealWindow();
          ((wxFrame *)frameData->userWindow)->OnSize(-1, -1);
        }
          
        if (frameData->toolbar)
        {
          frameData->toolbar->EditAttributes();
          frameData->toolbar->DestroyRealWindow();
          frameData->toolbar->MakeRealWindow();
          ((wxFrame *)frameData->userWindow)->OnSize(-1, -1);
          MakeModified();
        }
        wxEndBusyCursor();
      }
      break;
    }
    case OBJECT_EDITOR_EDIT_ATTRIBUTES:
    {
      if (!currentWindow)
        return;

      wxBeginBusyCursor();
/*
      BuildWindowData *data = canvas->FindSelectionOfType(wxTYPE_WINDOW);

      if (!data)
        data = currentWindow;

      if (data)
      {
        data->EditAttributes();
        data->DestroyRealWindow();
        data->MakeRealWindow();
#ifdef wx_motif
        wxFlushEvents();
#endif
        if (wxSubType(data->windowType, wxTYPE_DIALOG_BOX) ||
            wxSubType(data->windowType, wxTYPE_FRAME))
          buildApp.AssociateObjectWithEditor(data);
      }
      canvas->Redraw();
*/
      wxEndBusyCursor();
      break;
    }
    case OBJECT_EDITOR_TOGGLE_TEST_MODE:
    {
      buildApp.testMode = ! buildApp.testMode;
      if (currentWindow)
        currentWindow->SetTestMode(buildApp.testMode);
        
      if (buildApp.testMode)
      {
        SetStatusText("Test mode", 1);
      }
      else
      {
        SetStatusText("Edit mode", 1);
      }
      break;
    }
#ifdef wx_msw
    case OBJECT_EDITOR_RC_CONVERT_MENU:
    {
      if (!currentWindow || (currentWindow->windowType != wxTYPE_FRAME))
      {
        wxMessageBox("Please load a frame into the object editor first.", "Error");
        return;
      }
      if (ConvertMenuFromRC((BuildFrameData *)currentWindow))
      {
        wxBeginBusyCursor();

        BuildFrameData *frameData = (BuildFrameData *)currentWindow;
        ShowMenuBarEditor(frameData, frameData->buildMenuBar);
        frameData->DestroyRealWindow();
        frameData->MakeRealWindow();
        buildApp.AssociateObjectWithEditor(frameData);
        MakeModified();

        wxEndBusyCursor();
      }
      break;
    }
    case OBJECT_EDITOR_RC_CONVERT_DIALOG:
    {
      BuildDialogBoxData *win = ConvertDialogFromRC();
      break;
    }
#endif
    case ZOOM_30:
    {
      canvas->GetDC()->SetUserScale(0.3, 0.3);
      GetMenuBar()->Check(currentZoom, FALSE);
      GetMenuBar()->Check(ZOOM_30, TRUE);
      currentZoom = ZOOM_30;
      canvas->GetDC()->Clear();
      canvas->Redraw();
      break;
    }
    case ZOOM_40:
    {
      canvas->GetDC()->SetUserScale(0.4, 0.4);
      GetMenuBar()->Check(currentZoom, FALSE);
      GetMenuBar()->Check(ZOOM_40, TRUE);
      currentZoom = ZOOM_40;
      canvas->GetDC()->Clear();
      canvas->Redraw();
      break;
    }
    case ZOOM_50:
    {
      canvas->GetDC()->SetUserScale(0.5, 0.5);
      GetMenuBar()->Check(currentZoom, FALSE);
      GetMenuBar()->Check(ZOOM_50, TRUE);
      currentZoom = ZOOM_50;
      canvas->GetDC()->Clear();
      canvas->Redraw();
      break;
    }
    case ZOOM_60:
    {
      canvas->GetDC()->SetUserScale(0.6, 0.6);
      GetMenuBar()->Check(currentZoom, FALSE);
      GetMenuBar()->Check(ZOOM_60, TRUE);
      currentZoom = ZOOM_60;
      canvas->GetDC()->Clear();
      canvas->Redraw();
      break;
    }
    case ZOOM_70:
    {
      canvas->GetDC()->SetUserScale(0.7, 0.7);
      GetMenuBar()->Check(currentZoom, FALSE);
      GetMenuBar()->Check(ZOOM_70, TRUE);
      currentZoom = ZOOM_70;
      canvas->GetDC()->Clear();
      canvas->Redraw();
      break;
    }
    case ZOOM_80:
    {
      canvas->GetDC()->SetUserScale(0.8, 0.8);
      GetMenuBar()->Check(currentZoom, FALSE);
      GetMenuBar()->Check(ZOOM_80, TRUE);
      currentZoom = ZOOM_80;
      canvas->GetDC()->Clear();
      canvas->Redraw();
      break;
    }
    case ZOOM_90:
    {
      canvas->GetDC()->SetUserScale(0.9, 0.9);
      GetMenuBar()->Check(currentZoom, FALSE);
      GetMenuBar()->Check(ZOOM_90, TRUE);
      currentZoom = ZOOM_90;
      canvas->GetDC()->Clear();
      canvas->Redraw();
      break;
    }
    case ZOOM_100:
    {
      canvas->GetDC()->SetUserScale(1.0, 1.0);
      GetMenuBar()->Check(currentZoom, FALSE);
      GetMenuBar()->Check(ZOOM_100, TRUE);
      currentZoom = ZOOM_100;
      canvas->GetDC()->Clear();
      canvas->Redraw();
      break;
    }
    default:
      break;
  }
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
Bool BuildFrame::OnClose(void)
{
  if (IsModified())
  {
    int ans = wxMessageBox("Save modified project?", "Warning", wxYES_NO | wxCANCEL);
    if (ans == wxYES)
    {
      buildApp.SaveProject(NULL);
    }
    else if (ans == wxCANCEL)
      return FALSE;
  }
  buildApp.ClearProject();
  ClearEditor();
  CloseTreeFrame();
  TheFontManager.Show(FALSE);
  if (EditorPaletteFrame)
  {
    wxFrame *fr = EditorPaletteFrame;
    EditorPaletteFrame->OnClose();
    delete fr;
    EditorPaletteFrame = NULL;
  }
  HelpInstance->Quit();
  if (TheReportFrame)
  {
    wxFrame *fr = TheReportFrame;
    TheReportFrame->OnClose();
    delete fr;
  }
#ifdef wx_msw
  CloseRCLoader();
#endif
  if (!Iconized())
  {
    GetPosition(&buildApp.mainX, &buildApp.mainY);
    GetSize(&buildApp.mainWidth, &buildApp.mainHeight);
  }
  buildApp.WriteDefaults();
  return TRUE;
}

void BuildFrame::OnSize(int w, int h)
{
  if (!buildWindowsItem || !toolbar)
    return;

  int toolbarHeight = 28;
    
  int frameW, frameH;
  GetClientSize(&frameW, &frameH);

/*
  buildWindowsItem->GetSize(&listW, &listH);
  buildWindowsItem->GetPosition(&listX, &listY);
  int panelW = listW + 10;
  int panelH = listH + listY + 10;
*/
  int listW, listH, listX, listY;
  buildWindowsItem->GetPosition(&listX, &listY);
  listH = 100;
  listW = frameW - 20;
  int panelW = frameW;
  int panelH = listH + listY + 10;

  int canvasH = frameH - panelH - toolbarHeight;

  toolbar->SetSize(0, 0, frameW, toolbarHeight);
  canvas->SetSize(0, toolbarHeight, frameW, canvasH);
#ifdef wx_xview
  panel->SetSize(0, toolbarHeight+canvasH, panelW, panelH);
  buildWindowsItem->SetSize(listX, listY, listW, listH);
#else
//  buildWindowsItem->SetSize(listX, listY, frameW - listX - 10, listH);
  buildWindowsItem->SetSize(listX, listY, listW, listH);
  panel->SetSize(0, toolbarHeight+canvasH, frameW, panelH);
#endif
}

void BuildFrame::ClearEditor(void)
{
  DisplayTree(FALSE, NULL);
}

/*
 * Object editor canvas behaviour
 *
 */
 
/*
 * Utilities
 *
 */
 
char *GetNewObjectName(char *root)
{
  static long objectId = 1;
  static char buf[100];
  sprintf(buf, "%s%ld", root, objectId);
  objectId ++;
  return buf;
}

void TopLevelWindowsProc(wxListBox& list, wxCommandEvent& event)
{
  BuildWindowData *win = buildApp.FindCurrentWindow();
//  if (win && ((!MainFrame) ||
//               (MainFrame && (MainFrame->currentWindow != win))))
  if (win)
  {
    wxBeginBusyCursor();
    buildApp.ShowObjectEditor(win);
    wxEndBusyCursor();
  }
}

void ShowAppEditor(void)
{
  wxBeginBusyCursor();
  char nameBuf[200];
  sprintf(nameBuf, "Application Properties");
  wxDialogBox *dialog = new wxDialogBox(NULL, nameBuf, FALSE, 10, 10);
  dialog->SetLabelFont(SmallLabelFont);
  dialog->SetButtonFont(SmallButtonFont);

  BuildForm *form = new BuildForm("Changing project and global settings");
  form->Add(wxMakeFormString("Application class name", &buildApp.appClass, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               200));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Description", &buildApp.buildDescription, wxFORM_MULTITEXT, NULL, NULL, wxVERTICAL,
//               400));
               400, 100));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Authors", &buildApp.buildAuthors, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               400));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Date", &buildApp.buildDate, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               400));

  form->AssociatePanel(dialog);
  form->dialog = dialog;

  dialog->Fit();
  dialog->Centre(wxBOTH);

//  form->RevertValues();

  wxEndBusyCursor();
  dialog->Show(TRUE);
}


/*
 * Form for all property dialogs
 *
 */

static char *BuildHelpTopic = NULL;
Bool formCancelled = FALSE;

BuildForm::BuildForm(char *helpTopic):wxForm(wxFORM_BUTTON_ALL | wxFORM_BUTTON_HELP)
{
  if (helpTopic) SetHelpTopic(helpTopic);
    
  dialog = NULL;
  formCancelled = FALSE;
}

BuildForm::~BuildForm(void)
{
  SetHelpTopic(NULL);
}

void BuildForm::OnOk(void)
{
  dialog->Show(FALSE);
  delete dialog;
  delete this;
  MakeModified();
}

void BuildForm::OnCancel(void)
{
  dialog->Show(FALSE);
  formCancelled = TRUE;
  delete dialog;
  delete this;
}

void BuildForm::OnHelp(void)
{
  if (BuildHelpTopic)
  {
    wxBeginBusyCursor();
    if (HelpInstance->LoadFile())
      HelpInstance->KeywordSearch(BuildHelpTopic);
    wxEndBusyCursor();
  }
}

void SetHelpTopic(char *s)
{
  if (BuildHelpTopic) delete[] BuildHelpTopic;
  if (s)
    BuildHelpTopic = copystring(s);
  else
    BuildHelpTopic = NULL;
}

/*
 * Object editor tool palette
 *
 */

EditorToolPalette::EditorToolPalette(wxFrame *frame, int x, int y, int w, int h,
            long style, int direction, int RowsOrColumns):
  wxToolBar(frame, x, y, w, h, style, direction, RowsOrColumns)
{
  currentlySelected = -1;
}

Bool EditorToolPalette::OnLeftClick(int toolIndex, Bool toggled)
{
  // BEGIN mutual exclusivity code
  if (toggled && (currentlySelected != -1) && (toolIndex != currentlySelected))
    ToggleTool(currentlySelected, FALSE);

  if (toggled)
    currentlySelected = toolIndex;
  else if (currentlySelected == toolIndex)
    currentlySelected = -1;
  //  END mutual exclusivity code

  if (MainFrame)
  {
    if (toggled && (toolIndex != PALETTE_ARROW))
      MainFrame->canvas->SetCursor(crossCursor);
    else
      MainFrame->canvas->SetCursor(handCursor);
  }

  return TRUE;
}

void EditorToolPalette::OnMouseEnter(int toolIndex)
{
  if (!MainFrame) return;
  
  if (toolIndex > -1)
  {
      switch (toolIndex)
      {
        case PALETTE_FRAME:
          MainFrame->SetStatusText("wxFrame");
          break;
        case PALETTE_DIALOG_BOX:
          MainFrame->SetStatusText("wxDialogBox");
          break;
        case PALETTE_PANEL:
          MainFrame->SetStatusText("wxPanel");
          break;
        case PALETTE_CANVAS:
          MainFrame->SetStatusText("wxCanvas");
          break;
        case PALETTE_TEXT_WINDOW:
          MainFrame->SetStatusText("wxTextWindow");
          break;
        case PALETTE_BUTTON:
          MainFrame->SetStatusText("wxButton");
          break;
        case PALETTE_MESSAGE:
          MainFrame->SetStatusText("wxMessage");
          break;
        case PALETTE_TEXT:
          MainFrame->SetStatusText("wxText");
          break;
        case PALETTE_MULTITEXT:
          MainFrame->SetStatusText("wxMultiText");
          break;
        case PALETTE_CHOICE:
          MainFrame->SetStatusText("wxChoice");
          break;
        case PALETTE_CHECKBOX:
          MainFrame->SetStatusText("wxCheckBox");
          break;
        case PALETTE_RADIOBOX:
          MainFrame->SetStatusText("wxRadioBox");
          break;
        case PALETTE_LISTBOX:
          MainFrame->SetStatusText("wxListBox");
          break;
        case PALETTE_SLIDER:
          MainFrame->SetStatusText("wxSlider");
          break;
        case PALETTE_GROUPBOX:
          MainFrame->SetStatusText("wxGroupBox");
          break;
        case PALETTE_GAUGE:
          MainFrame->SetStatusText("wxGauge");
          break;
        case PALETTE_ARROW:
          MainFrame->SetStatusText("Pointer");
          break;
      }
  }
  else MainFrame->SetStatusText("");
}

Bool EditorToolPaletteFrame::OnClose(void)
{
  GetPosition(&buildApp.paletteX, &buildApp.paletteY);
  return FALSE;
}
/*
 * Strip off any extension (dot something) from end of file,
 * IF one exists. Inserts zero into buffer.
 *
 */
 
void StripExtension(char *buffer)
{
  int len = strlen(buffer);
  int i = len-1;
  while (i > 0)
  {
    if (buffer[i] == '.')
    {
      buffer[i] = 0;
      break;
    }
    i --;
  }
}

/*
 * Main toolbar
 *
 */

EditorToolBar::EditorToolBar(wxFrame *frame, int x, int y, int w, int h,
            long style, int direction, int RowsOrColumns):
#ifdef wx_msw
  wxButtonBar(frame, x, y, w, h, style, direction, RowsOrColumns)
#else
  wxToolBar(frame, x, y, w, h, style, direction, RowsOrColumns)
#endif
{
}

Bool EditorToolBar::OnLeftClick(int toolIndex, Bool toggled)
{
  switch (toolIndex)
  {
    case TOOLBAR_LOAD_FILE:
    {
      buildApp.LoadProject(buildApp.projectFilename);
      break;
    }
    case TOOLBAR_SAVE_FILE:
    {
      buildApp.SaveProject(NULL);
      break;
    }
    case TOOLBAR_GEN_CPP:
    {
      buildApp.GenerateCPP();
      break;
    }
/*
    case TOOLBAR_TREE:
    {
      DisplayTree();
      break;
    }
*/
    case TOOLBAR_HELP:
    {
      wxBeginBusyCursor();
      HelpInstance->LoadFile();
      HelpInstance->DisplayContents();
      wxEndBusyCursor();
      break;
    }
    case TOOLBAR_FORMAT_HORIZ:
    {
      buildApp.FormatItems(TOOLBAR_FORMAT_HORIZ);
      break;
    }
    case TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN:
    {
      buildApp.FormatItems(TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN);
      break;
    }
    case TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN:
    {
      buildApp.FormatItems(TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN);
      break;
    }
    case TOOLBAR_FORMAT_VERT:
    {
      buildApp.FormatItems(TOOLBAR_FORMAT_VERT);
      break;
    }
    case TOOLBAR_FORMAT_VERT_TOP_ALIGN:
    {
      buildApp.FormatItems(TOOLBAR_FORMAT_VERT_TOP_ALIGN);
      break;
    }
    case TOOLBAR_FORMAT_VERT_BOT_ALIGN:
    {
      buildApp.FormatItems(TOOLBAR_FORMAT_VERT_BOT_ALIGN);
      break;
    }
    default:
      break;
  }
  return TRUE;
}

void EditorToolBar::OnMouseEnter(int toolIndex)
{
  if (!MainFrame) return;
  
  if (toolIndex > -1)
  {
      switch (toolIndex)
      {
        case TOOLBAR_LOAD_FILE:
          MainFrame->SetStatusText("Load project file");
          break;
        case TOOLBAR_SAVE_FILE:
          MainFrame->SetStatusText("Save project file");
          break;
        case TOOLBAR_FORMAT_HORIZ:
          MainFrame->SetStatusText("Align items horizontally");
          break;
        case TOOLBAR_FORMAT_VERT:
          MainFrame->SetStatusText("Align items vertically");
          break;
        case TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN:
          MainFrame->SetStatusText("Left-align items");
          break;
        case TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN:
          MainFrame->SetStatusText("Right-align items");
          break;
        case TOOLBAR_FORMAT_VERT_TOP_ALIGN:
          MainFrame->SetStatusText("Top-align items");
          break;
        case TOOLBAR_FORMAT_VERT_BOT_ALIGN:
          MainFrame->SetStatusText("Bottom-align items");
          break;
        case TOOLBAR_GEN_CPP:
          MainFrame->SetStatusText("Generate C++");
          break;
        case TOOLBAR_GEN_CLIPS:
          MainFrame->SetStatusText("Generate CLIPS");
          break;
        case TOOLBAR_TREE:
          MainFrame->SetStatusText("View object hierarchy");
          break;
        case TOOLBAR_HELP:
          MainFrame->SetStatusText("Display help contents");
          break;
        default:
          break;
      }
  }
  else MainFrame->SetStatusText("");
}

void EditorToolBar::OnPaint(void)
{
#ifdef wx_msw
  wxButtonBar::OnPaint();
#else
  wxToolBar::OnPaint();
#endif

  int w, h;
  GetSize(&w, &h);
  wxDC *dc = GetDC();
  dc->SetPen(wxBLACK_PEN);
  dc->SetBrush(wxTRANSPARENT_BRUSH);
  DrawLine(0, h-1, w, h-1);
}

/*
 * Selections
 *
 */
 
void SelectWindow(BuildWindowData *win)
{
  if (!BuildSelections.Member(win))
  {
    BuildSelections.Append(win);
  }

//  if (win->windowObject && !win->windowObject->Selected())
//    win->windowObject->Select(TRUE);
}

void DeselectWindow(BuildWindowData *win)
{
  if (BuildSelections.Member(win))
    BuildSelections.DeleteObject(win);

//  if (win->windowObject && win->windowObject->Selected())
//    win->windowObject->Select(FALSE);
}

void DeselectAll(void)
{
  wxNode *node = BuildSelections.First();
  while (node)
  {
    BuildWindowData *data = (BuildWindowData *)node->Data();
    wxNode *next = node->Next();
    
//    if (data->windowObject && data->windowObject->Selected())
//      data->windowObject->Select(FALSE);
    // May have been deleted by previous statement, so check.
    if (BuildSelections.Member(data))
      delete node;
    node = next;
  }
}

BuildWindowData *GetFirstSelection(void)
{
  wxNode *node = BuildSelections.First();
  if (node)
    return (BuildWindowData *)node->Data();
  else return NULL;
}

/*
 * Report frame
 *
 */
 
ReportFrame::ReportFrame(wxFrame *frame, char *title, int x, int y, int w, int h, long style):
  wxFrame(frame, title, x, y, w, h, style)
{
#ifdef wx_msw
  wxIcon *icon = new wxIcon("wxbuild");
#endif
#ifdef wx_x
  wxIcon *icon = new wxIcon(wxbuild_bits, wxbuild_width, wxbuild_height);
#endif

  SetIcon(icon);

  TheReportFrame = this;
  textWindow = new wxTextWindow(this);
#ifdef wx_x
  // Needs some persuasion to size correctly!
  OnSize(w, h);
#endif
  Show(TRUE);
}
  
Bool ReportFrame::OnClose(void)
{
  if (!Iconized())
  {
    GetPosition(&buildApp.reportX, &buildApp.reportY);
    GetSize(&buildApp.reportWidth, &buildApp.reportHeight);
  }
  TheReportFrame = NULL;
  return TRUE;
}

// void ReportFrame::OnMenuCommand(int id);

void ShowReportWindow(void)
{
  if (TheReportFrame)
  {
    TheReportFrame->Show(TRUE);
    return;
  }
    
  TheReportFrame = new ReportFrame(NULL, "wxBuilder Report",
     buildApp.reportX, buildApp.reportY, buildApp.reportWidth, buildApp.reportHeight,
     wxSDI | wxDEFAULT_FRAME);
}

void ClearReportWindow(void)
{
  if (TheReportFrame)
  {
    TheReportFrame->textWindow->Clear();
  }
}

void Report(char *s)
{
  if (!TheReportFrame)
    ShowReportWindow();
  TheReportFrame->textWindow->WriteText(s);
}

void MakeModified(Bool mod)
{
  isModified = mod;
//  if (mod)
//    DisplayTree(FALSE);
}

Bool IsModified(void)
{
  return isModified;
}

// Returns quoted string or "NULL"
char *SafeString(char *s)
{
  if (!s)
    return "NULL";
  else
  {
    strcpy(wxBuffer, "\"");
    strcat(wxBuffer, s);
    strcat(wxBuffer, "\"");
    return wxBuffer;
  }
}

// Returns quoted string or ''
char *SafeWord(char *s)
{
  if (!s)
    return "''";
  else
  {
    strcpy(wxBuffer, "'");
    strcat(wxBuffer, s);
    strcat(wxBuffer, "'");
    return wxBuffer;
  }
}

ObjectEditorCanvas::ObjectEditorCanvas(wxFrame *frame, int x, int y, int width, int height, long style):
  wxCanvas(frame, x, y, width, height, style)
{
    SetFont(SmallButtonFont);
}

ObjectEditorCanvas::~ObjectEditorCanvas(void)
{
}

BuildWindowData *ObjectEditorCanvas::FindSelectionOfType(WXTYPE type)
{
  return NULL;
}

BuildWindowData *ObjectEditorCanvas::FindSuitableItemParent(void)
{
  if (buildApp.GetUsePosition())
    return buildApp.GetObjectParent();
  return NULL;
}

void ObjectEditorCanvas::Redraw(void)
{
}

void ObjectEditorCanvas::OnPaint(void)
{
  if (TheBuilderTree)
    TheBuilderTree->Draw();
}

void ObjectEditorCanvas::OnEvent(wxMouseEvent& event)
{
  if (!TheBuilderTree)
    return;

  if (event.LeftDown())
  {
/*
    long id = TheBuilderTree->NodeHitTest(event.x, event.y);
    if (id > -1)
    {
      if (id != 1)
      {
        BuildWindowData *win = (BuildWindowData *)id;
        if (wxSubType(win->windowType, wxTYPE_DIALOG_BOX) ||
            wxSubType(win->windowType, wxTYPE_FRAME))
        {
          wxBeginBusyCursor();
          buildApp.ShowObjectEditor(win);
          wxEndBusyCursor();
        }
      }
    }
*/
  }
  else if (event.RightDown())
  {
    long id = TheBuilderTree->NodeHitTest(event.x, event.y);
    if (id > -1)
    {
      BuildWindowData *win = (BuildWindowData *)id;
      buildApp.objectMenu->SetClientData((char *)win);
      PopupMenu(buildApp.objectMenu, (float)event.x, (float)event.y);
    }
  }
}

