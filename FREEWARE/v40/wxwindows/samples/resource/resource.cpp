/*
 * File:	resource.cc
 * Purpose:	Demo for wxWindows resources
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

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
#include "wx_timer.h"
#include "wx_mf.h"
#include "resource.h"

// If we wanted to demonstrate total platform independence,
// then we'd use the dynamic file loading form for all platforms.
// But this shows how to embed the wxWindows resources
// in the program code/executable for UNIX and Windows
// platforms.

// If you have a Windows compiler that can cope with long strings,
// then you can always use the #include form for simplicity.

// NOTE: Borland's brc32.exe resource compiler doesn't recognize
// the TEXT resource, for some reason, so either run-time file loading
// or file inclusion should be used.

#if defined(wx_msw) && !defined(__BORLANDC__) && !defined(GNUWIN32)
// Under Windows, some compilers can't include
// a whole .wxr file. So we use a .rc user-defined resource
// instead. dialog1 will point to the whole .wxr 'file'.
static char *dialog1 = NULL;
static char *menu1 = NULL;
#else
// Other platforms should have sensible compilers that
// cope with long strings.
#include "dialog1.wxr"
#include "menu1.wxr"
#endif

// Declare two frames
MyFrame   *frame = NULL;

// This statement initialises the whole application
MyApp     myApp;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// Testing of ressources
MyApp::MyApp()
{
}

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
#if defined(wx_msw)  && !defined(__BORLANDC__)
  // Load the .wxr 'file' from a .rc resource, under Windows.
  dialog1 = wxLoadUserResource("dialog1");
  menu1 = wxLoadUserResource("menu1");
  // All resources in the file (only one in this case) get parsed
  // by this call.
  wxResourceParseString(dialog1);
  wxResourceParseString(menu1);
#else
  // Simply parse the data pointed to by the variable dialog1.
  // If there were several resources, there would be several
  // variables, and this would need to be called several times.
  wxResourceParseData(dialog1);
  wxResourceParseData(menu1);
#endif

  // Create the main frame window
  frame = new MyFrame(NULL, "wxWindows Resource Sample", 0, 0, 300, 250);

  // Give it a status line
  frame->CreateStatusLine(2);

/*
  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(RESOURCE_TEST1, "&Dialog box test",                "Test dialog box resource");
  file_menu->Append(RESOURCE_QUIT, "E&xit",                "Quit program");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
*/

  wxMenuBar *menu_bar = wxResourceCreateMenuBar("menu1");
  
  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  // Make a panel
  frame->panel = new wxPanel(frame, 0, 0, 400, 400, 0, "MyMainFrame");
  frame->Show(TRUE);

  // Return the main frame window
  return frame;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, Const char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
  panel = NULL;
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case RESOURCE_QUIT:
    {
      if (OnClose())
        delete this;
      break;
    }
    case RESOURCE_TEST1:
    {
      MyDialogBox *dialog = new MyDialogBox;
      if (dialog->LoadFromResource(this, "dialog1"))
      {
        wxMultiText *text = (wxMultiText *)wxFindWindowByName("multitext3", dialog);
        if (text)
          text->SetValue("wxWindows resource demo");
        dialog->SetModal(TRUE);
        dialog->Show(TRUE);
      }
      delete dialog;
      break;
    }
    default:
    {
      break;
    }
  }
}

Bool MyFrame::OnClose(void)
{
  Show(FALSE);

  return TRUE;
}

void MyDialogBox::OnCommand(wxWindow& win, wxCommandEvent& event)
{
  if (strcmp(win.GetName(), "button8") == 0 || strcmp(win.GetName(), "button7") == 0)
    Show(FALSE);
}
