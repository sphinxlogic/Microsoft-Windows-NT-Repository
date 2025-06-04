/*
 * File:	test.cc
 * Purpose:	Test wxWindows app to demonstrate use of wxHelp API
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
    Bool OnClose(void);
};

// ID for the menu quit command
#define TEST_QUIT 1
#define TEST_HELP_ON_HELP 2
// #define TEST_HELP_ON_EDITING 3
#define TEST_KEYWORD 4

// This statement initializes the whole application and calls OnInit
MyApp myApp;

// Help instance
wxHelpInstance *HelpInstance = NULL;

// `Main program' equivalent, creating windows and returning main app frame
wxFrame *MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "Test wxWindows App", 0, 0, 400, 300);

  // Give it an icon
  frame->SetIcon(new wxIcon("aiai_icn"));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(TEST_QUIT, "Quit");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(TEST_HELP_ON_HELP, "wxWindows Manual");
//  help_menu->Append(TEST_HELP_ON_EDITING, "Help on  wxHelp Editing");
  help_menu->Append(TEST_KEYWORD, "Keyword search");

  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "File");
  menu_bar->Append(help_menu, "Help");
  frame->SetMenuBar(menu_bar);

  // Make a panel with a message
  wxPanel *panel = new wxPanel(frame, 0, 0, 400, 300);
  (void)new wxMessage(panel, "Hello, this is a test wxWindows program!");

  HelpInstance = new wxHelpInstance(TRUE);
  HelpInstance->Initialize("manual");

  frame->Show(TRUE);

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
    case TEST_QUIT:
      OnClose();
      if (HelpInstance)
        delete HelpInstance ;
      delete this;
      break;
    case TEST_HELP_ON_HELP:
    {
      HelpInstance->LoadFile();
      HelpInstance->DisplayContents();
      break;
    }
/*
    case TEST_HELP_ON_EDITING:
    {
      HelpInstance->LoadFile();
      HelpInstance->DisplaySection(9);
      break;
    }
*/
    case TEST_KEYWORD:
    {
      char *s = wxGetTextFromUser("Enter keyword");
      if (s)
      {
        HelpInstance->LoadFile();
        HelpInstance->KeywordSearch(s);
        delete [] s ;
      }
      break;
    }
  }
}

Bool MyFrame::OnClose(void)
{
  HelpInstance->Quit();
  return TRUE;
}

