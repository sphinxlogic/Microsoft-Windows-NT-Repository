/*
 * File:	wx_resed.cc
 * Purpose:	Resource editor
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

#include "wx_check.h"
#include "wx_buttn.h"
#include "wx_choic.h"
#include "wx_lbox.h"
#include "wx_rbox.h"
#include "wx_group.h"
#include "wx_gauge.h"
#include "wx_slidr.h"
#include "wx_txt.h"
#include "wx_mtxt.h"
#endif

#include "wx_scrol.h"

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if defined(wx_msw) && !defined(GNUWIN32)
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include "wx_help.h"

#include "wx_resed.h"
#include "wx_rprop.h"
#include "wx_repal.h"

static void ObjectMenuProc(wxMenu& menu, wxCommandEvent& event);
void wxResourceEditWindow(wxWindow *win);
wxWindowPropertyInfo *wxCreatePropertyInfoForWindow(wxWindow *win);
wxResourceManager *wxResourceManager::currentResourceManager = NULL;

// Bitmaps for toolbar
wxBitmap *ToolbarLoadBitmap = NULL;
wxBitmap *ToolbarSaveBitmap = NULL;
wxBitmap *ToolbarNewBitmap = NULL;
wxBitmap *ToolbarVertBitmap = NULL;
wxBitmap *ToolbarAlignTBitmap = NULL;
wxBitmap *ToolbarAlignBBitmap = NULL;
wxBitmap *ToolbarHorizBitmap = NULL;
wxBitmap *ToolbarAlignLBitmap = NULL;
wxBitmap *ToolbarAlignRBitmap = NULL;
wxBitmap *ToolbarCopySizeBitmap = NULL;
wxBitmap *ToolbarToFrontBitmap = NULL;
wxBitmap *ToolbarToBackBitmap = NULL;
wxBitmap *ToolbarHelpBitmap = NULL;

wxBitmap *wxWinBitmap = NULL;

#ifdef wx_x
#include "bitmaps/load.xbm"
#include "bitmaps/save.xbm"
#include "bitmaps/new.xbm"
#include "bitmaps/vert.xbm"
#include "bitmaps/alignt.xbm"
#include "bitmaps/alignb.xbm"
#include "bitmaps/horiz.xbm"
#include "bitmaps/alignl.xbm"
#include "bitmaps/alignr.xbm"
#include "bitmaps/copysize.xbm"
#include "bitmaps/tofront.xbm"
#include "bitmaps/toback.xbm"
#include "bitmaps/help.xbm"
#include "bitmaps/wxwin.xbm"
#endif

/*
 * Resource manager
 */


wxResourceManager::wxResourceManager(void)
{
  currentResourceManager = this;
  editorFrame = NULL;
  editorPanel = NULL;
  popupMenu = NULL;
  editorResourceList = NULL;
  editorPalette = NULL;
  nameCounter = 1;
  modified = FALSE;
  currentFilename = "";
  editMode = TRUE;
  editorToolBar = NULL;
  
  // Default window positions
  resourceEditorWindowSize.width = 470;
  resourceEditorWindowSize.height = 300;

  resourceEditorWindowSize.x = 0;
  resourceEditorWindowSize.y = 0;
  
  propertyWindowSize.width = 300;
  propertyWindowSize.height = 300;

  helpInstance = NULL;
}

wxResourceManager::~wxResourceManager(void)
{
  currentResourceManager = NULL;
  SaveOptions();

  helpInstance->Quit();
  delete helpInstance;
  helpInstance = NULL;
}

Bool wxResourceManager::Initialize(void)
{
  // Set up the resource filename for each platform.
#ifdef wx_msw
  // dialoged.ini in the Windows directory
  char buf[256];
  GetWindowsDirectory(buf, 256);
  strcat(buf, "\\dialoged.ini");
  optionsResourceFilename = buf;
#elif defined(wx_x)
  char buf[500];
  (void)wxGetHomeDir(buf);
  strcat(buf, "/.hardyrc");
  optionsResourceFilename = buf;
#else
#error "Unsupported platform."
#endif

  LoadOptions();

  helpInstance = new wxHelpInstance(TRUE);
  helpInstance->Initialize("dialoged");

  InitializeTools();
  popupMenu = new wxMenu(NULL, (wxFunction)ObjectMenuProc);
  popupMenu->Append(OBJECT_MENU_EDIT, "Edit properties");
  popupMenu->Append(OBJECT_MENU_DELETE, "Delete object");
  
  if (!wxWinBitmap)
  {
#ifdef wx_msw
    wxWinBitmap = new wxBitmap("WXWINBMP", wxBITMAP_TYPE_BMP_RESOURCE);
#endif
#ifdef wx_x
    wxWinBitmap = new wxBitmap(wxwin_bits, wxwin_width, wxwin_height);
#endif
  }
  return TRUE;
}

Bool wxResourceManager::LoadOptions(void)
{
  wxGetResource("DialogEd", "editorWindowX", &resourceEditorWindowSize.x, (char *)(const char *)optionsResourceFilename);
  wxGetResource("DialogEd", "editorWindowY", &resourceEditorWindowSize.y, (char *)(const char *)optionsResourceFilename);
  wxGetResource("DialogEd", "editorWindowWidth", &resourceEditorWindowSize.width, (char *)(const char *)optionsResourceFilename);
  wxGetResource("DialogEd", "editorWindowHeight", &resourceEditorWindowSize.height, (char *)(const char *)optionsResourceFilename);
  wxGetResource("DialogEd", "propertyWindowX", &propertyWindowSize.x, (char *)(const char *)optionsResourceFilename);
  wxGetResource("DialogEd", "propertyWindowY", &propertyWindowSize.y, (char *)(const char *)optionsResourceFilename);
  wxGetResource("DialogEd", "propertyWindowWidth", &propertyWindowSize.width, (char *)(const char *)optionsResourceFilename);
  wxGetResource("DialogEd", "propertyWindowHeight", &propertyWindowSize.height, (char *)(const char *)optionsResourceFilename);
  return TRUE;
}

Bool wxResourceManager::SaveOptions(void)
{
  wxWriteResource("DialogEd", "editorWindowX", resourceEditorWindowSize.x, (char *)(const char *)optionsResourceFilename);
  wxWriteResource("DialogEd", "editorWindowY", resourceEditorWindowSize.y, (char *)(const char *)optionsResourceFilename);
  wxWriteResource("DialogEd", "editorWindowWidth", resourceEditorWindowSize.width, (char *)(const char *)optionsResourceFilename);
  wxWriteResource("DialogEd", "editorWindowHeight", resourceEditorWindowSize.height, (char *)(const char *)optionsResourceFilename);

  wxWriteResource("DialogEd", "propertyWindowX", propertyWindowSize.x, (char *)(const char *)optionsResourceFilename);
  wxWriteResource("DialogEd", "propertyWindowY", propertyWindowSize.y, (char *)(const char *)optionsResourceFilename);
  wxWriteResource("DialogEd", "propertyWindowWidth", propertyWindowSize.width, (char *)(const char *)optionsResourceFilename);
  wxWriteResource("DialogEd", "propertyWindowHeight", propertyWindowSize.height, (char *)(const char *)optionsResourceFilename);
  
  return TRUE;
}

// Show or hide the resource editor frame, which displays a list
// of resources with ability to edit them.
Bool wxResourceManager::ShowResourceEditor(Bool show, wxWindow *parent, const char *title)
{
  if (show)
  {
    if (editorFrame)
    {
      editorFrame->Iconize(FALSE);
      editorFrame->Show(TRUE);
      return TRUE;
    }
    editorFrame = OnCreateEditorFrame(title);
    SetFrameTitle("");
    wxMenuBar *menuBar = OnCreateEditorMenuBar(editorFrame);
    editorFrame->SetMenuBar(menuBar);
    editorPanel = OnCreateEditorPanel(editorFrame);
    editorToolBar = (EditorToolBar *)OnCreateToolBar(editorFrame);
    editorPalette = OnCreatePalette(editorFrame);

    // Constraints for toolbar
    wxLayoutConstraints *c = new wxLayoutConstraints;
    c->left.SameAs       (editorFrame, wxLeft, 0);
    c->top.SameAs        (editorFrame, wxTop, 0);
    c->right.SameAs      (editorFrame, wxRight, 0);
    c->bottom.Unconstrained();    
    c->width.Unconstrained();
    c->height.Absolute(28);
    editorToolBar->SetConstraints(c);

    // Constraints for palette
    c = new wxLayoutConstraints;
    c->left.SameAs       (editorFrame, wxLeft, 0);
    c->top.SameAs        (editorToolBar, wxBottom, 0);
    c->right.SameAs      (editorFrame, wxRight, 0);
    c->bottom.Unconstrained();    
    c->width.Unconstrained();
    c->height.Absolute(34);
    editorPalette->SetConstraints(c);

    // Constraints for panel
    c = new wxLayoutConstraints;
    c->left.SameAs       (editorFrame, wxLeft, 0);
    c->top.SameAs        (editorPalette, wxBottom, 0);
    c->right.SameAs      (editorFrame, wxRight, 0);
    c->bottom.SameAs     (editorFrame, wxBottom, 0);    
    c->width.Unconstrained();
    c->height.Unconstrained();
    editorPanel->SetConstraints(c);

    editorFrame->SetAutoLayout(TRUE);

    UpdateResourceList();
    editorFrame->Show(TRUE);
    return TRUE;
  }
  else
  {
    wxFrame *fr = editorFrame;
    if (editorFrame->OnClose())
    {
      fr->Show(FALSE);
      delete fr;
      editorFrame = NULL;
      editorPanel = NULL;
    }
  }
  return TRUE;
}

void wxResourceManager::SetFrameTitle(const wxString& filename)
{
  if (editorFrame)
  {
    if (filename == wxString(""))
      editorFrame->SetTitle("wxWindows Dialog Editor - untitled");
    else
    {
      wxString str("wxWindows Dialog Editor - ");
      char *data = (char *)(const char *)filename;
      wxString str2(wxFileNameFromPath(data));
      str += str2;
      editorFrame->SetTitle((char *)(const char *)str);
    }
  }
}

Bool wxResourceManager::Save(void)
{
  if (currentFilename == wxString(""))
    return SaveAs();
  else
    return Save(currentFilename);
}

Bool wxResourceManager::Save(const wxString& filename)
{
  // Ensure all visible windows are saved to their resources
  currentFilename = filename;
  SetFrameTitle(currentFilename);
  InstantiateAllResourcesFromWindows();
  if (resourceTable.Save(filename))
  {
    Modify(FALSE);
    return TRUE;
  }
  else
    return FALSE;
}

Bool wxResourceManager::SaveAs(void)
{
  char *s = wxFileSelector("Save resource file", wxPathOnly((char *)(const char *)currentFilename), wxFileNameFromPath((char *)(const char *)currentFilename),
    "wxr", "*.wxr", wxSAVE | wxOVERWRITE_PROMPT);
    
  if (!s)
    return FALSE;
    
  currentFilename = s;
  Save(currentFilename);
  return TRUE;
}

Bool wxResourceManager::SaveIfModified(void)
{
  if (Modified())
    return Save();
  else return TRUE;
}

Bool wxResourceManager::Load(const wxString& filename)
{
  return New(TRUE, filename);
}

Bool wxResourceManager::New(Bool loadFromFile, const wxString& filename)
{
  if (!Clear(TRUE, FALSE))
    return FALSE;
    
  if (loadFromFile)
  {
    wxString str = filename;
    if (str == wxString(""))
    {
      char *f = wxFileSelector("Open resource file", NULL, NULL, "wxr", "*.wxr", 0, NULL);
      if (f)
        str = f;
      else
        return FALSE;
    }
    
    if (!resourceTable.ParseResourceFile((char *)(const char *)str))
    {
      wxMessageBox("Could not read file.", "Resource file load error", wxOK | wxICON_EXCLAMATION);
      return FALSE;
    }
    currentFilename = str;
    
    SetFrameTitle(currentFilename);
    
    UpdateResourceList();
  }
  else
  {
    SetFrameTitle("");
    currentFilename = "";
  }
  Modify(FALSE);
  
  return TRUE;
}

Bool wxResourceManager::Clear(Bool deleteWindows, Bool force)
{
  if (!force && Modified())
  {
    int ans = wxMessageBox("Save modified resource file?", "Dialog Editor", wxYES_NO | wxCANCEL);
    if (ans == wxCANCEL)
      return FALSE;
    if (ans == wxYES)
      SaveIfModified();
  }
  
  DisassociateWindows(deleteWindows);

  resourceTable.ClearTable();
  UpdateResourceList();

  return TRUE;
}

Bool wxResourceManager::DisassociateWindows(Bool deleteWindows)
{
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    DisassociateResource(res, deleteWindows);
  }
  
  return TRUE;
}

void wxResourceManager::AssociateResource(wxItemResource *resource, wxWindow *win)
{
  if (!resourceAssociations.Get((long)resource))
    resourceAssociations.Put((long)resource, win);
    
  wxNode *node = resource->GetChildren().First();
  while (node)
  {
    wxItemResource *child = (wxItemResource *)node->Data();
    wxWindow *childWindow = (wxWindow *)resourceAssociations.Get((long)child);
/*
    if (!childWindow)
      childWindow = wxFindWindowByName(child->GetName(), win);
*/
    if (childWindow)
      AssociateResource(child, childWindow);
    else
    {
      char buf[200];
      sprintf(buf, "AssociateResource: cannot find child window %s", child->GetName() ? child->GetName() : "(unnamed)");
      wxMessageBox(buf, "Dialog Editor problem", wxOK);
    }

    node = node->Next();
  }
}

Bool wxResourceManager::DisassociateResource(wxItemResource *resource, Bool deleteWindow)
{
  wxWindow *win = FindWindowForResource(resource);
  if (!win)
    return FALSE;

  // Disassociate children of window without deleting windows
  // since they'll be deleted by parent.
  if (win->GetChildren())
  {
    wxNode *node = win->GetChildren()->First();
    while (node)
    {
      wxWindow *child = (wxWindow *)node->Data();
      DisassociateResource(child, FALSE);
      node = node->Next();
    }
  }
  
  if (deleteWindow)
  {
    if (win->IsKindOf(CLASSINFO(wxPanel)) && !win->IsKindOf(CLASSINFO(wxDialogBox)))
      delete win->GetParent(); // Delete frame
    else
      delete win;
  }
  RemoveSelection(win);
  resourceAssociations.Delete((long)resource);
  return TRUE;
}

Bool wxResourceManager::DisassociateResource(wxWindow *win, Bool deleteWindow)
{
  wxItemResource *res = FindResourceForWindow(win);
  if (res)
    return DisassociateResource(res, deleteWindow);
  return FALSE;
}

wxItemResource *wxResourceManager::FindResourceForWindow(wxWindow *win)
{
  resourceAssociations.BeginFind();
  wxNode *node;
  while (node = resourceAssociations.Next())
  {
    wxWindow *w = (wxWindow *)node->Data();
    if (w == win)
    {
      return (wxItemResource *)node->key.integer;
    }
  }
  return NULL;
}

wxWindow *wxResourceManager::FindWindowForResource(wxItemResource *resource)
{
  return (wxWindow *)resourceAssociations.Get((long)resource);
}


void wxResourceManager::MakeUniqueName(char *prefix, char *buf)
{
  while (TRUE)
  {
    sprintf(buf, "%s%d", prefix, nameCounter);
    nameCounter ++;
    
    if (!resourceTable.FindResource(buf))
      return;
  }
}

wxFrame *wxResourceManager::OnCreateEditorFrame(const char *title)
{
  int frameWidth = 420;
  int frameHeight = 300;
  
  wxResourceEditorFrame *frame = new wxResourceEditorFrame(this, NULL, (char *)title,

    resourceEditorWindowSize.x, resourceEditorWindowSize.y,
    resourceEditorWindowSize.width, resourceEditorWindowSize.height,

    wxSDI | wxDEFAULT_FRAME);

  frame->CreateStatusLine(2);
  frame->SetStatusText(editMode ? "Edit mode" : "Test mode", 1);
  frame->SetAutoLayout(TRUE);
#ifdef wx_msw
  wxIcon *icon = new wxIcon("DIALOGEDICON");
  frame->SetIcon(icon);
#endif
  return frame;
}

wxMenuBar *wxResourceManager::OnCreateEditorMenuBar(wxFrame *parent)
{
  wxMenuBar *menuBar = new wxMenuBar;

  wxMenu *fileMenu = new wxMenu;
  fileMenu->Append(RESED_NEW_DIALOG, "New &dialog", "Create a new dialog");
  fileMenu->Append(RESED_NEW_PANEL, "New &panel", "Create a new panel");
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_NEW, "&New project",           "Clear the current project");
  fileMenu->Append(wxID_OPEN, "&Open...",         "Load a resource file");
  fileMenu->Append(wxID_SAVE, "&Save",            "Save a resource file");
  fileMenu->Append(wxID_SAVEAS, "Save &As...",   "Save a resource file as...");
  fileMenu->Append(RESED_CLEAR, "&Clear",   "Clear current resources");
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_EXIT, "E&xit",            "Exit resource editor");

  wxMenu *editMenu = new wxMenu;
  editMenu->Append(RESED_RECREATE, "&Recreate",  "Recreate the selected resource(s)");
  editMenu->Append(RESED_DELETE, "&Delete",  "Delete the selected resource(s)");
  editMenu->AppendSeparator();
  editMenu->Append(RESED_TOGGLE_TEST_MODE, "&Toggle edit/test mode",  "Toggle edit/test mode");

  wxMenu *helpMenu = new wxMenu;
  helpMenu->Append(RESED_CONTENTS, "&Help topics",          "Invokes the on-line help");
  helpMenu->AppendSeparator();
  helpMenu->Append(wxID_ABOUT, "&About",          "About wxWindows Dialog Editor");

  menuBar->Append(fileMenu, "&File");
  menuBar->Append(editMenu, "&Edit");
  menuBar->Append(helpMenu, "&Help");

  return menuBar;
}

wxPanel *wxResourceManager::OnCreateEditorPanel(wxFrame *parent)
{
  wxResourceEditorPanel *panel = new wxResourceEditorPanel(parent);

  panel->SetLabelPosition(wxVERTICAL);

  editorResourceList = new wxListBox(panel, (wxFunction)NULL, "Resources", wxSINGLE,
    -1, -1, -1, -1);

  wxLayoutConstraints *c = new wxLayoutConstraints;
  c->left.SameAs       (panel, wxLeft, 5);
  c->top.SameAs        (panel, wxTop, 5);
  c->right.SameAs      (panel, wxRight, 5);
  c->bottom.SameAs     (panel, wxBottom, 5);
  c->width.Unconstrained();
  c->height.Unconstrained();
  editorResourceList->SetConstraints(c);

  return panel;
}

wxToolBar *wxResourceManager::OnCreateToolBar(wxFrame *parent)
{
  // Load palette bitmaps
#ifdef wx_msw
  ToolbarLoadBitmap = new wxBitmap("LOADTOOL");
  ToolbarSaveBitmap = new wxBitmap("SAVETOOL");
  ToolbarNewBitmap = new wxBitmap("NEWTOOL");
  ToolbarVertBitmap = new wxBitmap("VERTTOOL");
  ToolbarAlignTBitmap = new wxBitmap("ALIGNTTOOL");
  ToolbarAlignBBitmap = new wxBitmap("ALIGNBTOOL");
  ToolbarHorizBitmap = new wxBitmap("HORIZTOOL");
  ToolbarAlignLBitmap = new wxBitmap("ALIGNLTOOL");
  ToolbarAlignRBitmap = new wxBitmap("ALIGNRTOOL");
  ToolbarCopySizeBitmap = new wxBitmap("COPYSIZETOOL");
  ToolbarToBackBitmap = new wxBitmap("TOBACKTOOL");
  ToolbarToFrontBitmap = new wxBitmap("TOFRONTTOOL");
  ToolbarHelpBitmap = new wxBitmap("HELPTOOL");
#endif
#ifdef wx_x
  ToolbarLoadBitmap = new wxBitmap(load_bits, load_width, load_height);
  ToolbarSaveBitmap = new wxBitmap(save_bits, save_width, save_height);
  ToolbarNewBitmap = new wxBitmap(new_bits, save_width, save_height);
  ToolbarVertBitmap = new wxBitmap(vert_bits, vert_width, vert_height);
  ToolbarAlignTBitmap = new wxBitmap(alignt_bits, alignt_width, alignt_height);
  ToolbarAlignBBitmap = new wxBitmap(alignb_bits, alignb_width, alignb_height);
  ToolbarHorizBitmap = new wxBitmap(horiz_bits, horiz_width, horiz_height);
  ToolbarAlignLBitmap = new wxBitmap(alignl_bits, alignl_width, alignl_height);
  ToolbarAlignRBitmap = new wxBitmap(alignr_bits, alignr_width, alignr_height);
  ToolbarCopySizeBitmap = new wxBitmap(copysize_bits, copysize_width, copysize_height);
  ToolbarToBackBitmap = new wxBitmap(toback_bits, toback_width, toback_height);
  ToolbarToFrontBitmap = new wxBitmap(tofront_bits, tofront_width, tofront_height);
//  ToolbarCPPBitmap = new wxBitmap(cpp_bits, cpp_width, cpp_height);
//  ToolbarTreeBitmap = new wxBitmap(tree_bits, tree_width, tree_height);
  ToolbarHelpBitmap = new wxBitmap(help_bits, help_width, help_height);
#endif

  // Create the toolbar
  EditorToolBar *toolbar = new EditorToolBar(parent, 0, 0, -1, -1, 0,
                                        wxVERTICAL, 1);
  toolbar->SetMargins(2, 2);
  toolbar->GetDC()->SetBackground(wxLIGHT_GREY_BRUSH);

#ifdef wx_msw
  int width = 24;
  int dx = 2;
  int gap = 6;
#else
  int width = ToolbarLoadBitmap->GetWidth();
  int dx = 2;
  int gap = 6;
#endif
  int currentX = gap;
  toolbar->AddSeparator();
  toolbar->AddTool(TOOLBAR_NEW, ToolbarNewBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "New dialog");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_LOAD_FILE, ToolbarLoadBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Load");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_SAVE_FILE, ToolbarSaveBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Save");
  currentX += width + dx + gap;
  toolbar->AddSeparator();
  toolbar->AddTool(TOOLBAR_FORMAT_HORIZ, ToolbarVertBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Horizontal align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_VERT_TOP_ALIGN, ToolbarAlignTBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Top align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_VERT_BOT_ALIGN, ToolbarAlignBBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Bottom align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_VERT, ToolbarHorizBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Vertical align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN, ToolbarAlignLBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Left align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN, ToolbarAlignRBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Right align");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_COPY_SIZE, ToolbarCopySizeBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Copy size");
  currentX += width + dx + gap;
  toolbar->AddSeparator();
  toolbar->AddTool(TOOLBAR_TO_FRONT, ToolbarToFrontBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "To front");
  currentX += width + dx;
  toolbar->AddTool(TOOLBAR_TO_BACK, ToolbarToBackBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "To back");
  currentX += width + dx + gap;
/*
  toolbar->AddTool(TOOLBAR_GEN_CPP, ToolbarCPPBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL);
  currentX += width + dx;

  toolbar->AddTool(TOOLBAR_TREE, ToolbarTreeBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL);
  currentX += width + dx;
*/
  toolbar->AddSeparator();
  toolbar->AddTool(TOOLBAR_HELP, ToolbarHelpBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Help");
  currentX += width + dx;
  
  toolbar->CreateTools();
  
  return toolbar;

//  parent->OnSize(-1, -1);
}

void wxResourceManager::UpdateResourceList(void)
{
  editorResourceList->Clear();
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    wxString resType(res->GetType());
//    if (res->GetType() == wxTYPE_DIALOG_BOX || res->GetType() == wxTYPE_FRAME || res->GetType() == wxTYPE_BITMAP)
    if (resType == "wxDialogBox" || resType == "wxPanel" || resType == "wxBitmap")
    {
      AddItemsRecursively(0, res);
    }
  }
}

void wxResourceManager::AddItemsRecursively(int level, wxItemResource *resource)
{
  int padWidth = level*4;
  
  wxString theString("");
  theString.Append(' ', padWidth);
  theString += resource->GetName();
  
  editorResourceList->Append((char *)(const char *)theString, (char *)resource);

  if (strcmp(resource->GetType(), "wxBitmap") != 0)
  {
    wxNode *node = resource->GetChildren().First();
    while (node)
    {
      wxItemResource *res = (wxItemResource *)node->Data();
      AddItemsRecursively(level+1, res);
      node = node->Next();
    }
  }
}

Bool wxResourceManager::EditSelectedResource(void)
{
  int sel = editorResourceList->GetSelection();
  if (sel > -1)
  {
    wxItemResource *res = (wxItemResource *)editorResourceList->wxListBox::GetClientData(sel);
    return Edit(res);
  }
  return FALSE;
}

Bool wxResourceManager::Edit(wxItemResource *res)
{
  wxString resType(res->GetType());
  if (resType == "wxDialogBox")
    {
      wxDialogBox *dialog = (wxDialogBox *)FindWindowForResource(res);
      if (dialog)
        dialog->Show(TRUE);
      else
      {
        dialog = new wxDialogBox;
        wxResourceEditorDialogHandler *handler = new wxResourceEditorDialogHandler(dialog, res, dialog->GetEventHandler(),
           this);
        dialog->SetEventHandler(handler);
        dialog->SetUserEditMode(TRUE);
          
        dialog->LoadFromResource(GetEditorFrame(), res->GetName(), &resourceTable);
        dialog->SetModal(FALSE);
        AssociateResource(res, dialog);
        dialog->Show(TRUE);
      }
    }
  else if (resType == "wxPanel")
    {
      wxPanel *panel = (wxPanel *)FindWindowForResource(res);
      if (panel)
        panel->GetParent()->Show(TRUE);
      else
      {
        DialogEditorPanelFrame *frame = new DialogEditorPanelFrame(GetEditorFrame(), res->GetName(), 10, 10, 400, 300, wxDEFAULT_FRAME_STYLE, res->GetName());
        panel = new wxPanel;
        wxResourceEditorDialogHandler *handler = new wxResourceEditorDialogHandler(panel, res, panel->GetEventHandler(),
           this);
        panel->SetEventHandler(handler);
        panel->SetUserEditMode(TRUE);
          
        panel->LoadFromResource(frame, res->GetName(), &resourceTable);
        AssociateResource(res, panel);
        frame->SetClientSize(res->GetWidth(), res->GetHeight());
        frame->Show(TRUE);
      }
    }
  return FALSE;
}

Bool wxResourceManager::CreateNewDialog(void)
{
  char buf[256];
  MakeUniqueName("dialog", buf);
  
  wxItemResource *resource = new wxItemResource;
//  resource->SetType(wxTYPE_DIALOG_BOX);
  resource->SetType("wxDialogBox");
  resource->SetName(buf);
  resource->SetTitle(buf);
  resourceTable.AddResource(resource);

#ifdef wx_motif
  wxDialogBox *dialog = new wxDialogBox(GetEditorFrame(), buf, FALSE, 10, 10, 400, 300, wxDEFAULT_DIALOG_STYLE|wxUSER_COLOURS|wxVERTICAL_LABEL, buf);
  dialog->SetBackgroundColour(*wxLIGHT_GREY);
  dialog->GetDC()->SetBackground(wxLIGHT_GREY_BRUSH);
  dialog->SetButtonColour(*wxBLACK);
  dialog->SetLabelColour(*wxBLACK);
#else
  wxDialogBox *dialog = new wxDialogBox(GetEditorFrame(), buf, FALSE, 10, 10, 400, 300, wxDEFAULT_DIALOG_STYLE|wxVERTICAL_LABEL, buf);
#endif
  
  dialog->SetLabelPosition(wxVERTICAL);
  resource->SetValue1(FALSE); // Modeless to start with
  resource->SetStyle(dialog->GetWindowStyleFlag());

  // For editing in situ we will need to use the hash table to ensure
  // we don't dereference invalid pointers.
//  resourceWindowTable.Put((long)resource, dialog);

  wxResourceEditorDialogHandler *handler = new wxResourceEditorDialogHandler(dialog, resource, dialog->GetEventHandler(),
   this);
  dialog->SetEventHandler(handler);
  
  dialog->Centre(wxBOTH);
  dialog->SetUserEditMode(TRUE);
  dialog->Show(TRUE);

  SetEditMode(TRUE, FALSE);

  AssociateResource(resource, dialog);
//  SetCurrentResource(resource);
//  SetCurrentResourceWindow(dialog);
  UpdateResourceList();
  
  Modify(TRUE);
  
  return TRUE;
}

Bool wxResourceManager::CreateNewPanel(void)
{
  char buf[256];
  MakeUniqueName("panel", buf);
  
  wxItemResource *resource = new wxItemResource;
//  resource->SetType(wxTYPE_PANEL);
  resource->SetType("wxPanel");
  resource->SetName(buf);
  resource->SetTitle(buf);
  resourceTable.AddResource(resource);
  
  DialogEditorPanelFrame *frame = new DialogEditorPanelFrame(GetEditorFrame(), buf, 10, 10, 400, 300, wxDEFAULT_FRAME_STYLE, buf);

#ifdef wx_motif
  wxPanel *panel = new wxPanel(frame, 0, 0, 400, 300, wxVERTICAL_LABEL|wxUSER_COLOURS, buf);
  panel->SetBackgroundColour(*wxLIGHT_GREY);
  panel->GetDC()->SetBackground(wxLIGHT_GREY_BRUSH);
  panel->SetButtonColour(*wxBLACK);
  panel->SetLabelColour(*wxBLACK);
#else
  wxPanel *panel = new wxPanel(frame, 0, 0, 400, 300, wxVERTICAL_LABEL, buf);
#endif
  
  panel->SetLabelPosition(wxVERTICAL);
//  resource->SetValue1(FALSE); // Modeless to start with
  resource->SetStyle(panel->GetWindowStyleFlag());

  // For editing in situ we will need to use the hash table to ensure
  // we don't dereference invalid pointers.
//  resourceWindowTable.Put((long)resource, panel);

  wxResourceEditorDialogHandler *handler = new wxResourceEditorDialogHandler(panel, resource, panel->GetEventHandler(),
   this);
  panel->SetEventHandler(handler);
  
  panel->Centre(wxBOTH);
  panel->SetUserEditMode(TRUE);
  frame->Show(TRUE);

  SetEditMode(TRUE, FALSE);

  AssociateResource(resource, panel);
//  SetCurrentResource(resource);
//  SetCurrentResourceWindow(panel);
  UpdateResourceList();
  
  Modify(TRUE);
  
  return TRUE;
}

Bool wxResourceManager::CreatePanelItem(wxItemResource *panelResource, wxPanel *panel, char *iType, int x, int y, Bool isBitmap)
{
  char buf[256];
  if (!panel->IsKindOf(CLASSINFO(wxPanel)))
    return FALSE;

  Modify(TRUE);
  
  wxItemResource *res = new wxItemResource;
  wxItem *newItem = NULL;
  res->SetSize(x, y, -1, -1);
  res->SetType(iType);
  
  wxString itemType(iType);

  if (itemType == "wxButton")
    {
      MakeUniqueName("button", buf);
      res->SetName(buf);
      if (isBitmap)
        newItem = new wxButton(panel, (wxFunction)NULL, wxWinBitmap, x, y, -1, -1, 0, buf);
      else
        newItem = new wxButton(panel, (wxFunction)NULL, "Button", x, y, -1, -1, 0, buf);
    }
  else if (itemType == "wxMessage")
    {
      MakeUniqueName("message", buf);
      res->SetName(buf);
      if (isBitmap)
        newItem = new wxMessage(panel, wxWinBitmap, x, y, 0, buf);
      else
        newItem = new wxMessage(panel, "Message", x, y, 0, buf);
    }
  else if (itemType == "wxCheckBox")
    {
      MakeUniqueName("checkbox", buf);
      res->SetName(buf);
      newItem = new wxCheckBox(panel, (wxFunction)NULL, "Checkbox", x, y, -1, -1, 0, buf);
    }
  else if (itemType == "wxListBox")
    {
      MakeUniqueName("listbox", buf);
      res->SetName(buf);
      newItem = new wxListBox(panel, (wxFunction)NULL, "Listbox", wxSINGLE, x, y, -1, -1, 0, NULL, 0, buf);
    }
  else if (itemType == "wxRadioBox")
    {
      MakeUniqueName("radiobox", buf);
      res->SetName(buf);
      char *names[] = { "One", "Two" };
      newItem = new wxRadioBox(panel, (wxFunction)NULL, "Radiobox", x, y, -1, -1, 2, names, 2, wxHORIZONTAL, buf);
      res->SetStringValues(new wxStringList("One", "Two", NULL));
    }
  else if (itemType == "wxChoice")
    {
      MakeUniqueName("choice", buf);
      res->SetName(buf);
      newItem = new wxChoice(panel, (wxFunction)NULL, "Choice", x, y, -1, -1, 0, NULL, 0, buf);
    }
  else if (itemType == "wxGroupBox")
    {
      MakeUniqueName("group", buf);
      res->SetName(buf);
      newItem = new wxGroupBox(panel, "Groupbox", x, y, 200, 200, 0, buf);
    }
  else if (itemType == "wxGauge")
    {
      MakeUniqueName("gauge", buf);
      res->SetName(buf);
      newItem = new wxGauge(panel, "Gauge", 10, x, y, 80, 30, wxHORIZONTAL, buf);
    }
  else if (itemType == "wxSlider")
    {
      MakeUniqueName("slider", buf);
      res->SetName(buf);
      newItem = new wxSlider(panel, (wxFunction)NULL, "Slider", 1, 1, 10, 120, x, y, wxHORIZONTAL, buf);
    }
  else if (itemType == "wxText")
    {
      MakeUniqueName("text", buf);
      res->SetName(buf);
      newItem = new wxText(panel, (wxFunction)NULL, "Text", "", x, y, 120, -1, 0, buf);
    }
  else if (itemType == "wxMultiText")
    {
      MakeUniqueName("multitext", buf);
      res->SetName(buf);
      newItem = new wxMultiText(panel, (wxFunction)NULL, "Multitext", "", x, y, -1, -1, 0, buf);
    }
  else if (itemType == "wxScrollBar")
    {
      MakeUniqueName("scrollbar", buf);
      res->SetName(buf);
      newItem = new wxScrollBar(panel, (wxFunction)NULL, x, y, 140, -1, wxHORIZONTAL, buf);
    }
  if (!newItem)
    return FALSE;

  res->SetStyle(newItem->GetWindowStyleFlag());
  AssociateResource(res, newItem);
  panelResource->GetChildren().Append(res);

  UpdateResourceList();
  
  return TRUE;
}

// Find the first dialog or panel for which
// there is a selected panel item.
wxWindow *wxResourceManager::FindParentOfSelection(void)
{
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    wxWindow *win = FindWindowForResource(res);
    if (win)
    {
      wxNode *node1 = win->GetChildren()->First();
      while (node1)
      {
        wxItem *item = (wxItem *)node1->Data();
        if (item->IsSelected())
          return win;
          
        node1 = node1->Next();
      }
    }
  }
  return NULL;
}

// Format the panel items according to 'flag'
void wxResourceManager::AlignItems(int flag)
{
  wxWindow *win = FindParentOfSelection();
  if (!win)
    return;

  wxNode *node = GetSelections().First();
  if (!node)
    return;
    
  wxItem *firstSelection = (wxItem *)node->Data();
  if (firstSelection->GetParent() != win)
    return;
    
  int firstX, firstY;
  int firstW, firstH;
  firstSelection->GetPosition(&firstX, &firstY);
  firstSelection->GetSize(&firstW, &firstH);
  int centreX = (int)(firstX + (firstW / 2));
  int centreY = (int)(firstY + (firstH / 2));

  while (node = node->Next())
  {
    wxItem *item = (wxItem *)node->Data();
    if (item->GetParent() == win)
    {
      int x, y, w, h;
      item->GetPosition(&x, &y);
      item->GetSize(&w, &h);

      int newX, newY;

      switch (flag)
      {
        case TOOLBAR_FORMAT_HORIZ:
        {
          newX = x;
          newY = (int)(centreY - (h/2.0));
          break;
        }
        case TOOLBAR_FORMAT_VERT:
        {
          newX = (int)(centreX - (w/2.0));
          newY = y;
          break;
        }
        case TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN:
        {
          newX = firstX;
          newY = y;
          break;
        }
        case TOOLBAR_FORMAT_VERT_TOP_ALIGN:
        {
          newX = x;
          newY = firstY;
          break;
        }
        case TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN:
        {
          newX = firstX + firstW - w;
          newY = y;
          break;
        }
        case TOOLBAR_FORMAT_VERT_BOT_ALIGN:
        {
          newX = x;
          newY = firstY + firstH - h;
          break;
        }
        default:
          newX = x; newY = y;
          break;
      }

      item->SetSize(newX, newY, w, h);
    }
  }
  win->Refresh();
}

// Copy the first image's size to subsequent images
void wxResourceManager::CopySize(void)
{
  wxWindow *win = FindParentOfSelection();
  if (!win)
    return;

  wxNode *node = GetSelections().First();
  if (!node)
    return;
    
  wxItem *firstSelection = (wxItem *)node->Data();
  if (firstSelection->GetParent() != win)
    return;
    
  int firstX, firstY;
  int firstW, firstH;
  firstSelection->GetPosition(&firstX, &firstY);
  firstSelection->GetSize(&firstW, &firstH);
  int centreX = (int)(firstX + (firstW / 2));
  int centreY = (int)(firstY + (firstH / 2));

  while (node = node->Next())
  {
    wxItem *item = (wxItem *)node->Data();
    if (item->GetParent() == win)
      item->SetSize(-1, -1, firstW, firstH);
  }
  win->Refresh();
}

void wxResourceManager::ToBackOrFront(Bool toBack)
{
  wxWindow *win = FindParentOfSelection();
  if (!win)
    return;
  wxItemResource *winResource = FindResourceForWindow(win);

  wxNode *node = GetSelections().First();
  while (node)
  {
    wxItem *item = (wxItem *)node->Data();
    wxItemResource *itemResource = FindResourceForWindow(item);
    if (item->GetParent() == win)
    {
      win->GetChildren()->DeleteObject(item);
      if (winResource)
        winResource->GetChildren().DeleteObject(itemResource);
      if (toBack)
      {
        win->GetChildren()->Insert(item);
        if (winResource)
          winResource->GetChildren().Insert(itemResource);
      }
      else
      {
        win->GetChildren()->Append(item);
        if (winResource)
          winResource->GetChildren().Append(itemResource);
      }
    }
    node = node->Next();
  }
//  win->Refresh();
}

void wxResourceManager::AddSelection(wxWindow *win)
{
  if (!selections.Member(win))
    selections.Append(win);
}

void wxResourceManager::RemoveSelection(wxWindow *win)
{
  selections.DeleteObject(win);
}

// Need to search through resource table removing this from
// any resource which has this as a parent.
Bool wxResourceManager::RemoveResourceFromParent(wxItemResource *res)
{
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *thisRes = (wxItemResource *)node->Data();
    if (thisRes->GetChildren().Member(res))
    {
      thisRes->GetChildren().DeleteObject(res);
      return TRUE;
    }
  }
  return FALSE;
}

Bool wxResourceManager::DeleteResource(wxItemResource *res)
{
  if (!res)
    return FALSE;

  RemoveResourceFromParent(res);

  wxNode *node = res->GetChildren().First();
  while (node)
  {
    wxNode *next = node->Next();
    wxItemResource *child = (wxItemResource *)node->Data();
    DeleteResource(child);
    node = next;
  }
  
  // If this is a button or message resource, delete the
  // associate bitmap resource if not being used.
  wxString resType(res->GetType());
  
  if ((resType == "wxMessage" || resType == "wxButton") && res->GetValue4())
  {
    PossiblyDeleteBitmapResource(res->GetValue4());
  }

  resourceTable.Delete(res->GetName());
  delete res;
  return TRUE;
}

Bool wxResourceManager::DeleteResource(wxWindow *win, Bool deleteWindow)
{
  if (win->IsKindOf(CLASSINFO(wxItem)))
  {
    // Deselect and refresh window in case we leave selection
    // handles behind
    wxItem *item = (wxItem *)win;
    if (item->IsSelected())
    {
      RemoveSelection(item);
      item->SelectItem(FALSE);
      item->GetParent()->Refresh();
    }
  }
  
  wxItemResource *res = FindResourceForWindow(win);
  
  DisassociateResource(res, deleteWindow);
  DeleteResource(res);
  UpdateResourceList();

  // What about associated event handler? Must clean up! BUGBUG
//  if (win && deleteWindow)
//    delete win;
  return TRUE;
}

// Will eventually have bitmap type information, for different
// kinds of bitmap.
char *wxResourceManager::AddBitmapResource(char *filename)
{
  wxItemResource *resource = FindBitmapResourceByFilename(filename);
  if (!resource)
  {
    char buf[256];
    MakeUniqueName("bitmap", buf);
    resource = new wxItemResource;
    resource->SetType("wxBitmap");
    resource->SetName(buf);
    
    // A bitmap resource has one or more children, specifying
    // alternative bitmaps.
    wxItemResource *child = new wxItemResource;
    child->SetType("wxBitmap");
    child->SetName(filename);
    child->SetValue1(wxBITMAP_TYPE_BMP);
    child->SetValue2(RESOURCE_PLATFORM_ANY);
    child->SetValue3(0); // Depth
    child->SetSize(0,0,0,0);
    resource->GetChildren().Append(child);

    resourceTable.AddResource(resource);

    UpdateResourceList();
  }
  if (resource)
    return resource->GetName();
  else
    return NULL;
}

 // Delete the bitmap resource if it isn't being used by another resource.
void wxResourceManager::PossiblyDeleteBitmapResource(char *resourceName)
{
  if (!IsBitmapResourceUsed(resourceName))
  {
    wxItemResource *res = resourceTable.FindResource(resourceName);
    DeleteResource(res);
    UpdateResourceList();
  }
}

Bool wxResourceManager::IsBitmapResourceUsed(char *resourceName)
{
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    wxString resType(res->GetType());
    if (resType == "wxDialogBox")
    {
      wxNode *node1 = res->GetChildren().First();
      while (node1)
      {
        wxItemResource *child = (wxItemResource *)node1->Data();
        wxString childResType(child->GetType());
        
        if ((childResType == "wxMessage" || childResType == "wxButton") &&
             child->GetValue4() &&
            (strcmp(child->GetValue4(), resourceName) == 0))
          return TRUE;
        node1 = node1->Next();
      }
    }
  }
  return FALSE;
}

// Given a wxButton or wxMessage, find the corresponding bitmap filename.
char *wxResourceManager::FindBitmapFilenameForResource(wxItemResource *resource)
{
  if (!resource || !resource->GetValue4())
    return NULL;
  wxItemResource *bitmapResource = resourceTable.FindResource(resource->GetValue4());
  if (!bitmapResource)
    return NULL;

  wxNode *node = bitmapResource->GetChildren().First();
  while (node)
  {
    // Eventually augment this to return a bitmap of the right kind or something...
    // Maybe the root of the filename remains the same, so it doesn't matter which we
    // pick up. Otherwise how do we specify multiple filenames... too boring...
    wxItemResource *child = (wxItemResource *)node->Data();
    return child->GetName();
    
    node = node->Next();
  }
  return NULL;
}

wxItemResource *wxResourceManager::FindBitmapResourceByFilename(char *filename)
{
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    wxString resType(res->GetType());
    if (resType == "wxBitmap")
    {
      wxNode *node1 = res->GetChildren().First();
      while (node1)
      {
        wxItemResource *child = (wxItemResource *)node1->Data();
        if (child->GetName() && (strcmp(child->GetName(), filename) == 0))
          return res;
        node1 = node1->Next();
      }
    }
  }
  return NULL;
}

 // Deletes 'win' and creates a new window from the resource that
 // was associated with it. E.g. if you can't change properties on the
 // fly, you'll need to delete the window and create it again.
wxWindow *wxResourceManager::RecreateWindowFromResource(wxWindow *win, wxWindowPropertyInfo *info)
{
  wxItemResource *resource = FindResourceForWindow(win);

  // Put the current window properties into the wxItemResource object
  
  wxWindowPropertyInfo *newInfo = NULL;
  if (!info)
  {
    newInfo = wxCreatePropertyInfoForWindow(win);
    info = newInfo;
  }

  info->InstantiateResource(resource);

  wxWindow *newWin = NULL;
  wxWindow *parent = win->GetParent();
  
  if (win->IsKindOf(CLASSINFO(wxPanel)))
  {
    Bool isDialog = win->IsKindOf(CLASSINFO(wxDialogBox));
    wxWindow *parent = win->GetParent();
    
    win->GetEventHandler()->OnClose();
    
    if (!isDialog && parent)
    {
      // Delete frame parent of panel if this is not a dialog box
      parent->Close(TRUE);
    }

    Edit(resource);
    newWin = FindWindowForResource(resource);
  }
  else
  {
    DisassociateResource(resource, FALSE);
    delete win;
    newWin = resourceTable.CreateItem((wxPanel *)parent, resource);
    AssociateResource(resource, newWin);
    UpdateResourceList();
  }

  if (info)
    info->SetPropertyWindow(newWin);
    
  if (newInfo)
    delete newInfo;
  
  return newWin;
}

// Delete resource highlighted in the listbox
Bool wxResourceManager::DeleteSelection(Bool deleteWindow)
{
  int sel = editorResourceList->GetSelection();
  if (sel > -1)
  {
    wxItemResource *res = (wxItemResource *)editorResourceList->wxListBox::GetClientData(sel);
    wxWindow *win = FindWindowForResource(res);
/*
    if (res == currentResource)
    {
      currentResource = NULL;
      currentResourceWindow = NULL;
    }
*/

    DisassociateResource(res, deleteWindow);
    DeleteResource(res);
    UpdateResourceList();

/*
    // What about associated event handler? Must clean up! BUGBUG
    if (win && deleteWindow)
      delete win;
*/

    Modify(TRUE);
  }

  return FALSE;
}

// Delete resource highlighted in the listbox
Bool wxResourceManager::RecreateSelection(void)
{
  wxNode *node = GetSelections().First();
  while (node)
  {
    wxItem *item = (wxItem *)node->Data();
    wxNode *next = node->Next();
    
    item->SelectItem(FALSE);
    RemoveSelection(item);
    
    RecreateWindowFromResource(item);
    
    node = next;
  }
  return TRUE;
}

Bool wxResourceManager::EditDialog(wxDialogBox *dialog, wxWindow *parent)
{
  return FALSE;
}

void wxResourceManager::SetEditMode(Bool flag, Bool changeCurrentResource)
{
  editMode = flag;
  if (editorFrame)
    editorFrame->SetStatusText(editMode ? "Edit mode" : "Test mode", 1);

  // Switch mode for each dialog in the resource list
  resourceTable.BeginFind();
  wxNode *node = resourceTable.Next();
  while (node)
  {
    wxItemResource *resource = (wxItemResource *)node->Data();
    wxWindow *currentResourceWindow = FindWindowForResource(resource);
  
    if (changeCurrentResource && currentResourceWindow && (currentResourceWindow->IsKindOf(CLASSINFO(wxPanel))))
    {
      wxPanel *panel = (wxPanel *)currentResourceWindow;
      if (editMode)
      {
        // If we have already installed our own handler, don't bother editing.
        // This test will need to be changed eventually because for in-situ editing,
        // the user might have installed a different one anyway.
        if (panel->GetEventHandler() == panel)
        {
          wxResourceEditorDialogHandler *handler = new wxResourceEditorDialogHandler(panel, resource, panel->GetEventHandler(),
     this);
          panel->SetEventHandler(handler);
          panel->SetUserEditMode(TRUE);
        }
      }
      else
      {
        if (panel->GetEventHandler() != panel)
        {
          wxEvtHandler *oldHandler = panel->GetEventHandler();
          panel->SetEventHandler(panel);
          panel->SetUserEditMode(FALSE);
          delete oldHandler;
        
          // Deselect all items on the dialog and refresh.
          wxNode *node = panel->GetChildren()->First();
          while (node)
          {
            wxItem *item = (wxItem *)node->Data();
            item->SelectItem(FALSE);
            node = node->Next();
          }
          panel->Refresh();
        }
      }
    }
    node = resourceTable.Next();
  }
}

// Ensures that all currently shown windows are saved to resources,
// e.g. just before writing to a .wxr file.
Bool wxResourceManager::InstantiateAllResourcesFromWindows(void)
{
  resourceTable.BeginFind();
  wxNode *node;
  while (node = resourceTable.Next())
  {
    wxItemResource *res = (wxItemResource *)node->Data();
    wxString resType(res->GetType());
    
    if (resType == "wxDialogBox")
    {
      wxWindow *win = (wxWindow *)FindWindowForResource(res);
      if (win)
        InstantiateResourceFromWindow(res, win, TRUE);
    }
    else if (resType == "wxPanel")
    {
      wxWindow *win = (wxWindow *)FindWindowForResource(res);
      if (win)
        InstantiateResourceFromWindow(res, win, TRUE);
    }
  }
  return TRUE;  
}

Bool wxResourceManager::InstantiateResourceFromWindow(wxItemResource *resource, wxWindow *window, Bool recurse)
{
  wxWindowPropertyInfo *info = wxCreatePropertyInfoForWindow(window);
  info->SetResource(resource);
  info->InstantiateResource(resource);
  delete info;
  
  if (recurse)
  {
    wxNode *node = resource->GetChildren().First();
    while (node)
    {
      wxItemResource *child = (wxItemResource *)node->Data();
      wxWindow *childWindow = FindWindowForResource(child);
      
      if (!childWindow)
      {
        char buf[200];
        sprintf(buf, "Could not find window %s", child->GetName());
        wxMessageBox(buf, "Dialog Editor problem", wxOK);
      }
      else
        InstantiateResourceFromWindow(child, childWindow, recurse);
      node = node->Next();
    }
  }
  
  return TRUE;
}


/*
 * Resource editor frame
 */
 
wxResourceEditorFrame::wxResourceEditorFrame(wxResourceManager *resMan, wxFrame *parent, char *title,
    int x, int y, int width, int height, long style, char *name):
  wxFrame(parent, title, x, y, width, height, style, name)
{
  manager = resMan;
}

wxResourceEditorFrame::~wxResourceEditorFrame(void)
{
}

void wxResourceEditorFrame::OnMenuCommand(int cmd)
{
  switch (cmd)
  {
    case wxID_NEW:
    {
      manager->New(FALSE);
      break;
    }
    case RESED_NEW_DIALOG:
    {
      manager->CreateNewDialog();
      break;
    }
    case RESED_NEW_PANEL:
    {
      manager->CreateNewPanel();
      break;
    }
    case wxID_OPEN:
    {
      manager->New(TRUE);
      break;
    }
    case RESED_CLEAR:
    {
      manager->Clear(TRUE, FALSE);
      break;
    }
    case wxID_SAVE:
    {
      manager->Save();
      break;
    }
    case wxID_SAVEAS:
    {
      manager->SaveAs();
      break;
    }
    case wxID_EXIT:
    {
	  manager->Clear(TRUE, FALSE) ;
      this->Close();
      break;
    }
    case wxID_ABOUT:
    {
      char buf[300];
      sprintf(buf, "wxWindows Dialog Editor %.1f\nAuthor: Julian Smart J.Smart@ed.ac.uk\nJulian Smart (c) 1996", wxDIALOG_EDITOR_VERSION);
      (void)wxMessageBox(buf, "About Dialog Editor", wxOK|wxCENTRE);
      break;
    }
    case RESED_CONTENTS:
    {
      wxBeginBusyCursor();
      manager->GetHelpInstance()->LoadFile();
      manager->GetHelpInstance()->DisplayContents();
      wxEndBusyCursor();
      break;
    }
    case RESED_DELETE:
    {
      manager->DeleteSelection();
      break;
    }
    case RESED_RECREATE:
    {
      manager->RecreateSelection();
      break;
    }
    case RESED_TOGGLE_TEST_MODE:
    {
      manager->SetEditMode(!manager->GetEditMode());
      break;
    }
    default:
      break;
  }
}

Bool wxResourceEditorFrame::OnClose(void)
{
  if (manager->Modified())
  {
    int ans = wxMessageBox("Save modified resource file?", "Dialog Editor", wxYES_NO | wxCANCEL);
    if (ans == wxCANCEL)
      return FALSE;
    if (ans == wxYES)
      if (!manager->SaveIfModified())
        return FALSE;
  }
    
  if (!Iconized())
  {
    GetSize(&(manager->resourceEditorWindowSize.width),
            &(manager->resourceEditorWindowSize.height));
    GetPosition(&(manager->resourceEditorWindowSize.x),
                &(manager->resourceEditorWindowSize.y));
  }
  manager->SetEditorFrame(NULL);
  manager->SetEditorToolBar(NULL);
  manager->SetEditorPalette(NULL);

  return TRUE;
}

/*
 * Resource editor panel
 */
 
wxResourceEditorPanel::wxResourceEditorPanel(wxWindow *parent, int x, int y, int width, int height,
    long style, char *name):
  wxPanel(parent, x, y, width, height, style, name)
{
}

wxResourceEditorPanel::~wxResourceEditorPanel(void)
{
}

void wxResourceEditorPanel::OnDefaultAction(wxItem *item)
{
  wxResourceEditorFrame *frame = (wxResourceEditorFrame *)GetParent();
  wxResourceManager *manager = frame->manager;

  if (item == manager->GetEditorResourceList())
  {
    manager->EditSelectedResource();
  }
}

// Popup menu callback
void ObjectMenuProc(wxMenu& menu, wxCommandEvent& event)
{
  wxWindow *data = (wxWindow *)menu.GetClientData();
  if (!data)
    return;

  switch (event.commandInt)
  {
    case OBJECT_MENU_EDIT:
    {
      wxResourceEditWindow(data);
      break;
    }
    case OBJECT_MENU_DELETE:
    {
      wxResourceManager::currentResourceManager->DeleteResource(data);
      break;
    }
    default:
      break;
  }
}

wxWindowPropertyInfo *wxCreatePropertyInfoForWindow(wxWindow *win)
{
  wxWindowPropertyInfo *info = NULL;
  if (win->IsKindOf(CLASSINFO(wxScrollBar)))
        {
          info = new wxScrollBarPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxGroupBox)))
        {
          info = new wxGroupBoxPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxCheckBox)))
        {
          info = new wxCheckBoxPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxSlider)))
        {
          info = new wxSliderPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxGauge)))
        {
          info = new wxGaugePropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxListBox)))
        {
          info = new wxListBoxPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxRadioBox)))
        {
          info = new wxRadioBoxPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxChoice)))
        {
          info = new wxChoicePropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxButton)))
        {
          info = new wxButtonPropertyInfo(win);
          if (((wxButton *)win)->IsBitmap())
            ((wxButtonPropertyInfo *)info)->isBitmapButton = TRUE;
        }
  else if (win->IsKindOf(CLASSINFO(wxMessage)))
        {
          info = new wxMessagePropertyInfo(win);
          if (((wxMessage *)win)->IsBitmap())
            ((wxMessagePropertyInfo *)info)->isBitmapMessage = TRUE;
        }
  else if (win->IsKindOf(CLASSINFO(wxText)))
        {
          info = new wxTextPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxDialogBox)))
        {
          info = new wxDialogPropertyInfo(win);
        }
  else if (win->IsKindOf(CLASSINFO(wxPanel)))
        {
          info = new wxPanelPropertyInfo(win);
        }
  else
        {
          info = new wxWindowPropertyInfo(win);
        }
  return info;
}

// Popup menu callback
void wxResourceEditWindow(wxWindow *win)
{
  wxWindowPropertyInfo *info = wxCreatePropertyInfoForWindow(win);
  if (info)
  {
    info->SetResource(wxResourceManager::currentResourceManager->FindResourceForWindow(win));
    wxString str("Editing ");
    str += win->GetClassInfo()->GetClassName();
    str += ": ";
    str += (win->GetName() ? win->GetName() : "properties");
    info->Edit(NULL, (char *)(const char *)str);
    delete info;
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
  wxResourceManager *manager = wxResourceManager::currentResourceManager;
  
  switch (toolIndex)
  {
    case TOOLBAR_LOAD_FILE:
    {
      manager->New(TRUE);
      break;
    }
    case TOOLBAR_NEW:
    {
      manager->New(FALSE);
      break;
    }
    case TOOLBAR_SAVE_FILE:
    {
      manager->Save();
      break;
    }
    case TOOLBAR_HELP:
    {
      wxBeginBusyCursor();
      manager->GetHelpInstance()->LoadFile();
      manager->GetHelpInstance()->DisplayContents();
      wxEndBusyCursor();
      break;
    }
    case TOOLBAR_FORMAT_HORIZ:
    {
      manager->AlignItems(TOOLBAR_FORMAT_HORIZ);
      break;
    }
    case TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN:
    {
      manager->AlignItems(TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN);
      break;
    }
    case TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN:
    {
      manager->AlignItems(TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN);
      break;
    }
    case TOOLBAR_FORMAT_VERT:
    {
      manager->AlignItems(TOOLBAR_FORMAT_VERT);
      break;
    }
    case TOOLBAR_FORMAT_VERT_TOP_ALIGN:
    {
      manager->AlignItems(TOOLBAR_FORMAT_VERT_TOP_ALIGN);
      break;
    }
    case TOOLBAR_FORMAT_VERT_BOT_ALIGN:
    {
      manager->AlignItems(TOOLBAR_FORMAT_VERT_BOT_ALIGN);
      break;
    }
    case TOOLBAR_COPY_SIZE:
    {
      manager->CopySize();
      break;
    }
    case TOOLBAR_TO_BACK:
    {
      manager->ToBackOrFront(TRUE);
      break;
    }
    case TOOLBAR_TO_FRONT:
    {
      manager->ToBackOrFront(FALSE);
      break;
    }
    default:
      break;
  }
  return TRUE;
}

void EditorToolBar::OnMouseEnter(int toolIndex)
{
  wxFrame *frame = (wxFrame *)GetParent();
  
  if (!frame) return;
  
  if (toolIndex > -1)
  {
      switch (toolIndex)
      {
        case TOOLBAR_LOAD_FILE:
          frame->SetStatusText("Load project file");
          break;
        case TOOLBAR_SAVE_FILE:
          frame->SetStatusText("Save project file");
          break;
        case TOOLBAR_NEW:
          frame->SetStatusText("Create a new resource");
          break;
        case TOOLBAR_FORMAT_HORIZ:
          frame->SetStatusText("Align items horizontally");
          break;
        case TOOLBAR_FORMAT_VERT:
          frame->SetStatusText("Align items vertically");
          break;
        case TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN:
          frame->SetStatusText("Left-align items");
          break;
        case TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN:
          frame->SetStatusText("Right-align items");
          break;
        case TOOLBAR_FORMAT_VERT_TOP_ALIGN:
          frame->SetStatusText("Top-align items");
          break;
        case TOOLBAR_FORMAT_VERT_BOT_ALIGN:
          frame->SetStatusText("Bottom-align items");
          break;
        case TOOLBAR_COPY_SIZE:
          frame->SetStatusText("Copy size from first selection");
          break;
        case TOOLBAR_TO_FRONT:
          frame->SetStatusText("Put image to front");
          break;
        case TOOLBAR_TO_BACK:
          frame->SetStatusText("Put image to back");
          break;
        case TOOLBAR_HELP:
          frame->SetStatusText("Display help contents");
          break;
        default:
          break;
      }
  }
  else frame->SetStatusText("");
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
