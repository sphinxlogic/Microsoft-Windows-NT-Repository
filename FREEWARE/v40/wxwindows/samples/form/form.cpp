/*
 * File:	form.cc
 * Purpose:	Demo for wxWindows `forms' class
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

#ifdef __GNUG__
#pragma implementation
#endif

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wx_form.h"
#include "form.h"

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

MyFrame *frame = NULL;
MyForm *my_form = NULL;
wxDialogBox *form_dialog = NULL;
MyObject object_to_edit;

// This statement initialises the whole application
MyApp myApp;

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  // Create the main frame window
  frame = new MyFrame(NULL, "Form Demo", 0, 0, 400, 300);

  // Give it an icon
#ifdef wx_msw
  wxIcon *icon = new wxIcon("aiai_icn");
#endif
#ifdef wx_x
  wxIcon *icon = new wxIcon("aiai.xbm");
#endif
  frame->SetIcon(icon);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(FORM_EDIT, "Edit a form");
  file_menu->Append(FORM_QUIT, "Quit");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "File");

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
    case FORM_QUIT:
    {
//      OnClose();
//      delete this;
      Close(TRUE);
      break;
    }
    case FORM_EDIT:
    {
      if (!form_dialog)
      {
        form_dialog = new wxDialogBox(frame, "Form", FALSE, 100, 100, 600, 400);
        my_form = new MyForm;
        my_form->EditForm(&object_to_edit, form_dialog);
        form_dialog->Show(TRUE);
      }
      else form_dialog->Show(TRUE);
      break;
    }
  }
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
Bool MyFrame::OnClose(void)
{
  if (form_dialog)
//    delete form_dialog;
    form_dialog->Close(TRUE);
  return TRUE;
}

// Test button function
void MyButtonProc(wxButton& but, wxCommandEvent& event)
{
  wxMessageBox("Pressed a button", "Notification", wxOK);
}

// A user-defined constraint
Bool MyConstraint(int type, char *value, char *label, char *msg_buffer)
{
  if (value && (strlen(value) > 7))
  {
    sprintf(msg_buffer, "Value for %s should be 7 characters or less",
            label);
    return FALSE;
  }
  else return TRUE;
}


void MyForm::EditForm(MyObject *object, wxPanel *panel)
{
  Add(wxMakeFormString("string 1", &(object->string1), wxFORM_DEFAULT,
                       new wxList(wxMakeConstraintFunction(MyConstraint), 0), NULL, wxVERTICAL));
  Add(wxMakeFormNewLine());

  Add(wxMakeFormString("string 2", &(object->string2), wxFORM_DEFAULT,
                       new wxList(wxMakeConstraintStrings("One", "Two", "Three", 0), 0), NULL, wxVERTICAL));
  Add(wxMakeFormString("string 3", &(object->string3), wxFORM_CHOICE,
                       new wxList(wxMakeConstraintStrings("Pig", "Cow",
                                  "Aardvark", "Gorilla", 0), 0), NULL, wxVERTICAL));
  Add(wxMakeFormNewLine());
  Add(wxMakeFormShort("int 1", &(object->int1), wxFORM_DEFAULT,
                       new wxList(wxMakeConstraintRange(0.0, 50.0), 0), NULL, wxVERTICAL));
  Add(wxMakeFormNewLine());

  Add(wxMakeFormFloat("float 1", &(object->float1), wxFORM_DEFAULT,
                       new wxList(wxMakeConstraintRange(-100.0, 100.0), 0), NULL, wxVERTICAL));
  Add(wxMakeFormBool("bool 1", &(object->bool1)));
//  Add(wxMakeFormNewLine());

  Add(wxMakeFormButton("Test button", (wxFunction)MyButtonProc));

  AssociatePanel(panel);
  panel->Fit();
}

void MyForm::OnOk(void)
{
  form_dialog->Show(FALSE);

  delete my_form;
//  delete form_dialog;
  form_dialog->Close(TRUE);

  my_form = NULL;
  form_dialog = NULL;
}

void MyForm::OnCancel(void)
{
  form_dialog->Show(FALSE);

  delete my_form;
//  delete form_dialog;
  form_dialog->Close(TRUE);

  my_form = NULL;
  form_dialog = NULL;
}

MyObject::MyObject(void)
{
  string1 = NULL;
  string2 = NULL;
  string3 = NULL;
  int1 = 20;
  bool1 = 1;
  float1 = 0.0;
}

