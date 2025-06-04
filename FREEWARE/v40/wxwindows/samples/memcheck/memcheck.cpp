/*
 * File:	minimal.cc
 * Purpose:	Memory-checking sample
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#if !DEBUG
#error You must set DEBUG to 1 on the 'make' command line or make.env.
#endif

#include "wxstring.h"

// #define new WXDEBUG_NEW

// Define a new application type
class MyApp: public wxApp
{ public:
    wxFrame *OnInit(void);
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    void OnMenuCommand(int id);
};

// ID for the menu quit command
#define MEMCHECK_QUIT 1

// This statement initializes the whole application and calls OnInit
MyApp myApp;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// `Main program' equivalent, creating windows and returning main app frame
wxFrame *MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "Memcheck wxWindows sample", 50, 50, 400, 300);

  // Give it an icon
#ifdef wx_msw
  frame->SetIcon(new wxIcon("aiai_icn"));
#endif
#ifdef wx_x
  frame->SetIcon(new wxIcon("aiai.xbm"));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(MEMCHECK_QUIT, "Quit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "File");
  frame->SetMenuBar(menu_bar);

  // Make a panel with a message
  wxPanel *panel = new wxPanel(frame, 0, 0, 400, 400);

  panel->SetLabelPosition(wxHORIZONTAL) ;

  (void)new wxMessage(panel, "Hello, this is a minimal debugging wxWindows program!", 0, 0);
  // Show the frame
  frame->Show(TRUE);

//  wxDebugContext::SetCheckpoint();
  wxDebugContext::SetFile("debug.log");

  wxString *thing = WXDEBUG_NEW wxString;

  // Proves that defining 'new' to be 'WXDEBUG_NEW' doesn't mess up
  // non-object allocation
  char *ordinaryNonObject = new char[1000];

  char *data = thing->GetData();

  wxDebugContext::PrintClasses();
  wxDebugContext::Dump();
  wxDebugContext::PrintStatistics();

  WXTRACE("Testing memory checking and statistics from memcheck.\n");
  
  // Return the main frame window
  return frame;
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id) {
    case MEMCHECK_QUIT:
      delete this;
    break;
  }
}

