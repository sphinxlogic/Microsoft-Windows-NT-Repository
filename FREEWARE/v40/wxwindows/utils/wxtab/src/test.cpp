/*
 * File:     test.cc
 * Purpose:  wxWindows wxTab demo
 *
	Last change:  JS   25 May 97    6:01 pm
 */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wxtab.h"
#include "test.h"

// Declare two frames
MyFrame   *frame = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp     myApp;

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  // Create the main frame window
  frame = new MyFrame(NULL, "Tab Test", 0, 0, 350, 350);

  // Give it a status line
  frame->CreateStatusLine(2);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(TEST_TABBED_DIALOG, "&Test tabbed dialog", "Test tabbed dialog");
  file_menu->Append(TEST_QUIT, "E&xit",                        "Quit program");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(TEST_ABOUT, "&About",                      "About Tab Test");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  frame->Show(TRUE);

  // Return the main frame window
  return frame;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case TEST_QUIT:
    {
      OnClose();
      delete this;
      break;
    }
    case TEST_TABBED_DIALOG:
    {
      TestTabbedDialog();
      break;
    }
    case TEST_ABOUT:
    {
      (void)wxMessageBox("wxWindows Tab Demo\nAuthor: Julian Smart J.Smart@ed.ac.uk\nAIAI (c) 1996", "About Tab Test");
      break;
    }
  }
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
Bool MyFrame::OnClose(void)
{
  return TRUE;
}

void MyFrame::TestTabbedDialog(void)
{
  int dialogWidth = 365;
  int dialogHeight = 390;
  
  wxTabbedDialogBox *dialog = new wxTabbedDialogBox(this, "Tabbed Dialog Box", TRUE, -1, -1, 365, 390);

  wxButton *okButton = new wxButton(dialog, (wxFunction)GenericOk, "Close", 100, 330, 80, 25);
  wxButton *cancelButton = new wxButton(dialog, (wxFunction)GenericOk, "Cancel", 185, 330, 80, 25);
  wxButton *HelpButton = new wxButton(dialog, NULL, "Help", 270, 330, 80, 25);
  okButton->SetDefault();


  // Note, omit the wxTAB_STYLE_COLOUR_INTERIOR, so we will guarantee a match
  // with the panel background, and save a bit of time.
  wxPanelTabView *view = new wxPanelTabView(dialog, wxTAB_STYLE_DRAW_BOX);
  
  wxRectangle rect;
  rect.x = 5;
  rect.y = 70;
  // Could calculate the view width from the tab width and spacing,
  // as below, but let's assume we have a fixed view width.
//  rect.width = view->GetTabWidth()*4 + 3*view->GetHorizontalTabSpacing();
  rect.width = 326;
  rect.height = 250;
  
  view->SetViewRect(rect);

  // Calculate the tab width for 4 tabs, based on a view width of 326 and
  // the current horizontal spacing. Adjust the view width to exactly fit
  // the tabs.
  view->CalculateTabWidth(4, TRUE);

  if (!view->AddTab(TEST_TAB_CAT,        wxString("Cat")))
    return;
    
  if (!view->AddTab(TEST_TAB_DOG,        wxString("Dog")))
    return;
  if (!view->AddTab(TEST_TAB_GUINEAPIG,  wxString("Guinea Pig")))
    return;
  if (!view->AddTab(TEST_TAB_GOAT,       wxString("Goat")))
    return;
  if (!view->AddTab(TEST_TAB_ANTEATER,   wxString("Ant-eater")))
    return;
  if (!view->AddTab(TEST_TAB_SHEEP,      wxString("Sheep")))
    return;
  if (!view->AddTab(TEST_TAB_COW,        wxString("Cow")))
    return;
  if (!view->AddTab(TEST_TAB_HORSE,      wxString("Horse")))
    return;
  if (!view->AddTab(TEST_TAB_PIG,        wxString("Pig")))
    return;
  if (!view->AddTab(TEST_TAB_OSTRICH,    wxString("Ostrich")))
    return;
  if (!view->AddTab(TEST_TAB_AARDVARK,   wxString("Aardvark")))
    return;
  if (!view->AddTab(TEST_TAB_HUMMINGBIRD,wxString("Hummingbird")))
    return;
    
  // Add some panels
  wxPanel *panel1 = new wxPanel(dialog, rect.x + 20, rect.y + 10, 200, 200);
  (void)new wxButton(panel1, NULL, "Press me");
  panel1->NewLine();
  (void)new wxText(panel1, NULL, "Input:", "1234", -1, -1, 120);
  
  view->AddTabWindow(TEST_TAB_CAT, panel1);

  wxPanel *panel2 = new wxPanel(dialog, rect.x + 20, rect.y + 10, 200, 200);
  panel2->SetLabelPosition(wxVERTICAL);
  
  char *animals[] = { "Fox", "Hare", "Rabbit", "Sabre-toothed tiger", "T Rex" };
  (void)new wxListBox(panel2, NULL, "List of animals", wxSINGLE, 5, 5, 170, 80, 5, animals);

  (void)new wxMultiText(panel2, NULL, "Notes", "Some notes about the animals in this house", 5, 100, 170, 100);
  
  view->AddTabWindow(TEST_TAB_DOG, panel2);
  
  // Don't know why this is necessary under Motif...
#ifdef wx_motif
  dialog->SetSize(dialogWidth, dialogHeight-20);
#endif

  view->SetTabSelection(TEST_TAB_CAT);
  
  dialog->Centre(wxBOTH);
  
  dialog->Show(TRUE);  
}

void GenericOk(wxButton& but, wxCommandEvent& event)
{
  wxDialogBox *dialog = (wxDialogBox *)but.GetParent();

  dialog->Show(FALSE);
  dialog->Close();
}
