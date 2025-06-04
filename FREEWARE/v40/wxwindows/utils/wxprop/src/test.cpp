/*
 * File:	test.cc
 * Purpose:	Test wxPropertySheet classes
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

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

#include "test.h"

MyFrame *frame = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp myApp;

wxPropertyValidatorRegistry myListValidatorRegistry;
wxPropertyValidatorRegistry myFormValidatorRegistry;

MyApp::MyApp(void)
{
}

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  RegisterValidators();

  // Create the main frame window
  frame = new MyFrame(NULL, "wxPropertySheet Demo", 0, 0, 300, 400, wxSDI | wxDEFAULT_FRAME);

  // Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef wx_msw
  wxIcon *icon = new wxIcon("aiai_icn");
#endif
#ifdef wx_x
  wxIcon *icon = new wxIcon("aiai.xbm");
#endif
  frame->SetIcon(icon);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(PROPERTY_TEST_DIALOG_LIST, "Test property list &dialog...");
  file_menu->Append(PROPERTY_TEST_FRAME_LIST, "Test property list &frame...");
  file_menu->AppendSeparator();
  file_menu->Append(PROPERTY_TEST_DIALOG_FORM, "Test property form d&ialog...");
  file_menu->Append(PROPERTY_TEST_FRAME_FORM, "Test property form f&rame...");
  file_menu->AppendSeparator();
  file_menu->Append(PROPERTY_QUIT, "&Exit");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(PROPERTY_ABOUT, "&About");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  frame->Centre(wxBOTH);
  frame->Show(TRUE);
  
//  char buf[300];
//  sprintf(buf, "%.2f", 1.23);

  // Essential - return the main frame window
  return frame;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h, long type):
  wxFrame(frame, title, x, y, w, h, type)
{
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case PROPERTY_QUIT:
    {
      if (OnClose())
        delete this;
      break;
    }
    case PROPERTY_TEST_DIALOG_LIST:
    {
      PropertyListTest(TRUE);
      break;
    }
    case PROPERTY_TEST_FRAME_LIST:
    {
      PropertyListTest(FALSE);
      break;
    }
    case PROPERTY_TEST_DIALOG_FORM:
    {
      PropertyFormTest(TRUE);
      break;
    }
    case PROPERTY_TEST_FRAME_FORM:
    {
      PropertyFormTest(FALSE);
      break;
    }
    case PROPERTY_ABOUT:
    {
      (void)wxMessageBox("wxPropertySheet Demo\nAuthor: Julian Smart J.Smart@ed.ac.uk\nAIAI (c) 1995", "About wxPropertySheet Test");
      break;
    }
    default:
    {
    }
  }
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
Bool MyFrame::OnClose(void)
{
  return TRUE;
}


wxFrame *GetMainFrame(void)
{
  return frame;
}


void RegisterValidators(void)
{
  myListValidatorRegistry.RegisterValidator((wxString)"real", new wxRealListValidator);
  myListValidatorRegistry.RegisterValidator((wxString)"string", new wxStringListValidator);
  myListValidatorRegistry.RegisterValidator((wxString)"integer", new wxIntegerListValidator);
  myListValidatorRegistry.RegisterValidator((wxString)"bool", new wxBoolListValidator);
  myListValidatorRegistry.RegisterValidator((wxString)"stringlist", new wxListOfStringsListValidator);

  myFormValidatorRegistry.RegisterValidator((wxString)"real", new wxRealFormValidator);
  myFormValidatorRegistry.RegisterValidator((wxString)"string", new wxStringFormValidator);
  myFormValidatorRegistry.RegisterValidator((wxString)"integer", new wxIntegerFormValidator);
  myFormValidatorRegistry.RegisterValidator((wxString)"bool", new wxBoolFormValidator);
}

void PropertyListTest(Bool useDialog)
{
  wxPropertySheet *sheet = new wxPropertySheet;

  sheet->AddProperty(new wxProperty("fred", 1.0, "real"));
  sheet->AddProperty(new wxProperty("tough choice", (Bool)TRUE, "bool"));
  sheet->AddProperty(new wxProperty("ian", (long)45, "integer", new wxIntegerListValidator(-50, 50)));
  sheet->AddProperty(new wxProperty("bill", 25.0, "real", new wxRealListValidator(0.0, 100.0)));
  sheet->AddProperty(new wxProperty("julian", "one", "string"));
  sheet->AddProperty(new wxProperty("bitmap", "none", "string", new wxFilenameListValidator("Select a bitmap file", "*.bmp")));
  wxStringList *strings = new wxStringList("one", "two", "three", NULL);
  sheet->AddProperty(new wxProperty("constrained", "one", "string", new wxStringListValidator(strings)));

  wxStringList *strings2 = new wxStringList("earth", "fire", "wind", "water", NULL);
  sheet->AddProperty(new wxProperty("string list", strings2, "stringlist"));

  wxPropertyListView *view =
    new wxPropertyListView(NULL,
     wxPROP_BUTTON_CHECK_CROSS|wxPROP_DYNAMIC_VALUE_FIELD|wxPROP_PULLDOWN|wxPROP_SHOWVALUES);

  wxDialogBox *propDialog = NULL;
  wxPropertyListFrame *propFrame = NULL;
  if (useDialog)
  {
    propDialog = new wxPropertyListDialog(view, NULL, "Property Sheet Test", TRUE, -1, -1, 400, 500);
  }
  else
  {
    propFrame = new wxPropertyListFrame(view, NULL, "Property Sheet Test", -1, -1, 400, 500);
  }
  
  view->AddRegistry(&myListValidatorRegistry);

  if (useDialog)
  {
    view->ShowView(sheet, propDialog);
    propDialog->Centre(wxBOTH);
    propDialog->Show(TRUE);
  }
  else
  {
    propFrame->Initialize();
    view->ShowView(sheet, propFrame->GetPropertyPanel());
    propFrame->Centre(wxBOTH);
    propFrame->Show(TRUE);
  }
}

void PropertyFormTest(Bool useDialog)
{
  wxPropertySheet *sheet = new wxPropertySheet;

  sheet->AddProperty(new wxProperty("fred", 25.0, "real", new wxRealFormValidator(0.0, 100.0)));
  sheet->AddProperty(new wxProperty("tough choice", (Bool)TRUE, "bool"));
  sheet->AddProperty(new wxProperty("ian", (long)45, "integer", new wxIntegerFormValidator(-50, 50)));
  sheet->AddProperty(new wxProperty("julian", "one", "string"));
  wxStringList *strings = new wxStringList("one", "two", "three", NULL);
  sheet->AddProperty(new wxProperty("constrained", "one", "string", new wxStringFormValidator(strings)));

  wxPropertyFormView *view = new wxPropertyFormView(NULL);

  wxDialogBox *propDialog = NULL;
  wxPropertyFormFrame *propFrame = NULL;
  if (useDialog)
  {
    propDialog = new wxPropertyFormDialog(view, NULL, "Property Form Test", TRUE, -1, -1, 380, 250);
  }
  else
  {
    propFrame = new wxPropertyFormFrame(view, NULL, "Property Form Test", -1, -1, 280, 250);
    propFrame->Initialize();
  }
  
  wxPanel *panel = propDialog ? propDialog : propFrame->GetPropertyPanel();
  panel->SetLabelPosition(wxVERTICAL);
  
  // Add items to the panel
  
  (void) new wxButton(panel, (wxFunction)NULL, "OK", -1, -1, -1, -1, 0, "ok");
  (void) new wxButton(panel, (wxFunction)NULL, "Cancel", -1, -1, 80, -1, 0, "cancel");
  (void) new wxButton(panel, (wxFunction)NULL, "Update", -1, -1, 80, -1, 0, "update");
  (void) new wxButton(panel, (wxFunction)NULL, "Revert", -1, -1, -1, -1, 0, "revert");
  panel->NewLine();
  
  // The name of this text item matches the "fred" property
  (void) new wxText(panel, (wxFunction)NULL, "Fred", "", -1, -1, 90, -1, 0, "fred");
  (void) new wxCheckBox(panel, (wxFunction)NULL, "Yes or no", -1, -1, -1, -1, 0, "tough choice");
  (void) new wxSlider(panel, (wxFunction)NULL, "Scale", 0, -50, 50, 150, -1, -1, wxHORIZONTAL, "ian");
  panel->NewLine();
  (void) new wxListBox(panel, (wxFunction)NULL, "Constrained", wxSINGLE, -1, -1, 100, 90, 0, NULL, 0, "constrained");

  view->AddRegistry(&myFormValidatorRegistry);

  if (useDialog)
  {
    view->ShowView(sheet, propDialog);
    view->AssociateNames();
    view->TransferToDialog();
    propDialog->Centre(wxBOTH);
    propDialog->Show(TRUE);
  }
  else
  {
    view->ShowView(sheet, propFrame->GetPropertyPanel());
    view->AssociateNames();
    view->TransferToDialog();
    propFrame->Centre(wxBOTH);
    propFrame->Show(TRUE);
  }
}
