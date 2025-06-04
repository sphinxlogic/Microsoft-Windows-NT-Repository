/*
 * File:	test.cc
 * Purpose:	PROLOGIO Test
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

#include "wx.h"
#include "read.h"

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

Bool errorHandler(int errorType, char *msg);

PrologDatabase database(errorHandler);

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

// IDs
#define TEST_QUIT 1
#define TEST_LOAD 2
#define TEST_SAVE 3
#define TEST_CREATE 4

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initializes the whole application and calls OnInit
MyApp myApp;

// `Main program' equivalent, creating windows and returning main app frame
wxFrame *MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "PROLOGIO Test", 50, 50, 400, 300);

  // Give it an icon
  frame->SetIcon(new wxIcon("aiai_icn"));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(TEST_LOAD, "Load");
  file_menu->Append(TEST_SAVE, "Save");
  file_menu->Append(TEST_CREATE, "Create");
  file_menu->Append(TEST_QUIT, "Quit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "File");
  frame->SetMenuBar(menu_bar);

  // Make a panel with a message
  (void) new wxPanel(frame, 0, 0, 400, 300);

  // Show the frame
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
    case TEST_LOAD:
    {
      char *s = wxFileSelector("Load a Prolog file");
      if (s)
      {
        wxBeginBusyCursor();
        database.Clear();
        if (!database.ReadProlog(s))
        {
          wxEndBusyCursor();
          wxMessageBox("Could not read file.", "Error", wxOK);
        }
        else
          wxEndBusyCursor();
      }
      break;
    }
    case TEST_SAVE:
    {
      char *s = wxFileSelector("Save a Prolog file");
      if (s)
      {
#ifdef wx_msw
	  wxUnix2DosFilename(s);
#endif
        ofstream str(s);
        database.WriteProlog(str);
      }
      break;
    }
    case TEST_CREATE:
    {
      database.Clear();

      PrologExpr *clause1 = new PrologExpr("clause1");
      clause1->AddAttributeValue("id", (float)123000000.0);
      clause1->AddAttributeValueString("name", "fred");
      database.Append(clause1);

      PrologExpr *clause2 = new PrologExpr("clause2");
      clause2->AddAttributeValue("id", (long)654321);
      clause2->AddAttributeValueString("name", "jim");
      database.Append(clause2);

      break;
    }
    case TEST_QUIT:
      delete this;
    break;
  }
}

Bool errorHandler(int errorType, char *msg)
{
  char buf[500];
  sprintf(buf, "Error %d during loading: %s", errorType, msg);
  wxMessageBox(buf, "Error");
  return FALSE;
}
