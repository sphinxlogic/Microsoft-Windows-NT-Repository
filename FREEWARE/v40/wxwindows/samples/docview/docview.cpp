/*
 * File:	docview.cc
 * Purpose:	
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
#pragma implementation
#endif

/*
 * Purpose:  Document/view architecture demo for wxWindows class library
 *           Run with no arguments for SDI style, with -mdi argument
 *           for MDI style windows.
 */


// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#if !USE_DOC_VIEW_ARCHITECTURE
#error You must set USE_DOC_VIEW_ARCHITECTURE to 1 in wx_setup.h!
#endif

#include "wx_doc.h"

#include "docview.h"
#include "doc.h"
#include "view.h"

MyFrame *frame = NULL;

Bool isMDI = FALSE;

// In single window mode, don't have any child windows; use
// main window.
Bool singleWindowMode = FALSE;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp myApp;

long top_frame_type = wxSDI | wxDEFAULT_FRAME;
long child_frame_type = wxSDI | wxDEFAULT_FRAME;

MyApp::MyApp(void)
{
}

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  //// Find out if we're:
  ////  SDI : multiple windows and documents but not MDI
  ////  MDI : multiple windows and documents with containing frame - MSW only)
  ///   single window : (one document at a time, only one frame, as in Windows Write)
  if (argc > 1)
  {
    if (strcmp(argv[1], "-mdi") == 0)
    {
      isMDI = TRUE;
      top_frame_type = wxMDI_PARENT | wxDEFAULT_FRAME;
      child_frame_type = wxMDI_CHILD | wxDEFAULT_FRAME;
      singleWindowMode = FALSE;
    }
    else if (strcmp(argv[1], "-sdi") == 0)
    {
      isMDI = FALSE;
      top_frame_type = wxSDI | wxDEFAULT_FRAME;
      child_frame_type = wxSDI | wxDEFAULT_FRAME;
      singleWindowMode = FALSE;
    }
    else if (strcmp(argv[1], "-single") == 0)
    {
      isMDI = FALSE;
      top_frame_type = wxSDI | wxDEFAULT_FRAME;
      child_frame_type = wxSDI | wxDEFAULT_FRAME;
      singleWindowMode = TRUE;
    }
  }

  //// Create a document manager
  wxDocManager *myDocManager = new wxDocManager;

  //// Create a template relating drawing documents to their views
  (void) new wxDocTemplate(myDocManager, "Drawing", "*.drw", NULL, "drw", "Drawing Doc", "Drawing View",
          CLASSINFO(DrawingDocument), CLASSINFO(DrawingView));

  if (singleWindowMode)
  {
    // If we've only got one window, we only get to edit
    // one document at a time. Therefore no text editing, just
    // doodling.
    myDocManager->SetMaxDocsOpen(1);
  }
  else
    //// Create a template relating text documents to their views
    (void) new wxDocTemplate(myDocManager, "Text", "*.txt", NULL, "txt", "Text Doc", "Text View",
          CLASSINFO(TextEditDocument), CLASSINFO(TextEditView));

  //// Create the main frame window
  frame = new MyFrame(myDocManager, NULL, "DocView Demo", 0, 0, 500, 400, top_frame_type);

  //// Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef wx_msw
  wxIcon *icon = new wxIcon("doc_icn");
#endif
#ifdef wx_x
  wxIcon *icon = new wxIcon("aiai.xbm");
#endif
  frame->SetIcon(icon);

  //// Make a menubar
  wxMenu *file_menu = new wxMenu;
  wxMenu *edit_menu = NULL;

  file_menu->Append(wxID_NEW, "&New...");
  file_menu->Append(wxID_OPEN, "&Open...");

  if (singleWindowMode)
  {
    file_menu->Append(wxID_CLOSE, "&Close");
    file_menu->Append(wxID_SAVE, "&Save");
    file_menu->Append(wxID_SAVEAS, "Save &As...");
    file_menu->AppendSeparator();
    file_menu->Append(wxID_PRINT, "&Print...");
    file_menu->Append(wxID_PRINT_SETUP, "Print &Setup...");
    file_menu->Append(wxID_PREVIEW, "Print Pre&view");

    edit_menu = new wxMenu;
    edit_menu->Append(wxID_UNDO, "&Undo");
    edit_menu->Append(wxID_REDO, "&Redo");
    edit_menu->AppendSeparator();
    edit_menu->Append(DOCVIEW_CUT, "&Cut last segment");

    frame->editMenu = edit_menu;
  }
  
  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, "E&xit");

  // A nice touch: a history of files visited. Use this menu.
  myDocManager->FileHistoryUseMenu(file_menu);

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(DOCVIEW_ABOUT, "&About");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  if (edit_menu)
    menu_bar->Append(edit_menu, "&Edit");
  menu_bar->Append(help_menu, "&Help");

  if (singleWindowMode)
    frame->canvas = frame->CreateCanvas(NULL, frame);

  //// Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  frame->Centre(wxBOTH);
  frame->Show(TRUE);

  return frame;
}

/*
 * Centralised code for creating a document frame.
 * Called from view.cc, when a view is created, but not used at all
 * in 'single window' mode.
 */
 
wxFrame *MyApp::CreateChildFrame(wxDocument *doc, wxView *view, Bool isCanvas)
{
  //// Make a child frame
  wxDocChildFrame *subframe = new wxDocChildFrame(doc, view, GetMainFrame(), "Child Frame", 10, 10, 300, 300,
                             child_frame_type);

  //// Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef wx_msw
  wxIcon *icon = new wxIcon((isCanvas ? "chrt_icn" : "notepad_icn"));
#endif
#ifdef wx_x
  wxIcon *icon = new wxIcon("aiai.xbm");
#endif
  subframe->SetIcon(icon);

  //// Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(wxID_NEW, "&New...");
  file_menu->Append(wxID_OPEN, "&Open...");
  file_menu->Append(wxID_CLOSE, "&Close");
  file_menu->Append(wxID_SAVE, "&Save");
  file_menu->Append(wxID_SAVEAS, "Save &As...");

  if (isCanvas)
  {
    file_menu->AppendSeparator();
    file_menu->Append(wxID_PRINT, "&Print...");
    file_menu->Append(wxID_PRINT_SETUP, "Print &Setup...");
    file_menu->Append(wxID_PREVIEW, "Print Pre&view");
  }

  if (isMDI)
  {
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, "E&xit");
  }

  wxMenu *edit_menu = NULL;

  if (isCanvas)
  {
    edit_menu = new wxMenu;
    edit_menu->Append(wxID_UNDO, "&Undo");
    edit_menu->Append(wxID_REDO, "&Redo");
    edit_menu->AppendSeparator();
    edit_menu->Append(DOCVIEW_CUT, "&Cut last segment");

    doc->GetCommandProcessor()->SetEditMenu(edit_menu);
  }

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(DOCVIEW_ABOUT, "&About");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  if (isCanvas)
    menu_bar->Append(edit_menu, "&Edit");
  menu_bar->Append(help_menu, "&Help");

  //// Associate the menu bar with the frame
  subframe->SetMenuBar(menu_bar);

  if (!isMDI)
    subframe->Centre(wxBOTH);

  return subframe;
}

/*
 * This is the top-level window of the application.
 */
 
IMPLEMENT_CLASS(MyFrame, wxDocParentFrame)

MyFrame::MyFrame(wxDocManager *manager, wxFrame *frame, char *title, int x, int y, int w, int h, long type):
  wxDocParentFrame(manager, frame, title, x, y, w, h, type)
{
  // This pointer only needed if in single window mode
  canvas = NULL;
  editMenu = NULL;
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case DOCVIEW_ABOUT:
    {
      (void)wxMessageBox("DocView Demo\nAuthor: Julian Smart J.Smart@ed.ac.uk\nAIAI (c) 1995\nUsage: docview.exe [-mdi | -sdi | -single]", "About DocView");
      break;
    }
    // If you're wondering where some behaviour gets implemented...
    // it's probably done in wx_doc.cc, routed through here!
    default:
    {
      wxDocParentFrame::OnMenuCommand(id);
    }
  }
}

// Creates a canvas. Called either from view.cc when a new drawing
// view is created, or in OnInit as a child of the main window,
// if in 'single window' mode.
MyCanvas *MyFrame::CreateCanvas(wxView *view, wxFrame *parent)
{
  int width, height;
  parent->GetClientSize(&width, &height);

  // Non-retained canvas
  MyCanvas *canvas = new MyCanvas(view, parent, 0, 0, width, height, 0);
  wxCursor *cursor = new wxCursor(wxCURSOR_PENCIL);
  canvas->SetCursor(cursor);

  // Give it scrollbars
  canvas->SetPen(wxRED_PEN);
  canvas->SetBackground(wxWHITE_BRUSH);
  canvas->SetScrollbars(20, 20, 50, 50, 4, 4);

  return canvas;
}

MyFrame *GetMainFrame(void)
{
  return frame;
}

