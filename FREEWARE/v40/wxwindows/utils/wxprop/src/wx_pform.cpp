/*
 * File:	wx_pform.cc
 * Purpose:	wxPropertyFormView and associated classes
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
#endif

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if defined(wx_msw) && !defined(GNUWIN32)
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include "wx_win.h"
#include "wx_utils.h"
#include "wx_list.h"
#include "wx_pform.h"

/*
 * Property view
 */

IMPLEMENT_DYNAMIC_CLASS(wxPropertyFormView, wxPropertyView)

static void wxPropertyViewOkProc(wxButton& but, wxCommandEvent& ev)
{
  wxPanel *panel = (wxPanel *)but.GetParent();
  wxPropertyView *view = (wxPropertyView *)panel->GetClientData();
  view->OnOk();
}

static void wxPropertyViewCancelProc(wxButton& but, wxCommandEvent& ev)
{
  wxPanel *panel = (wxPanel *)but.GetParent();
  wxPropertyView *view = (wxPropertyView *)panel->GetClientData();
  view->OnCancel();
}

static void wxPropertyViewHelpProc(wxButton& but, wxCommandEvent& ev)
{
  wxPanel *panel = (wxPanel *)but.GetParent();
  wxPropertyView *view = (wxPropertyView *)panel->GetClientData();
  view->OnHelp();
}

Bool wxPropertyFormView::dialogCancelled = FALSE;

wxPropertyFormView::wxPropertyFormView(wxPanel *propPanel, long flags):wxPropertyView(flags)
{
  propertyWindow = propPanel;
  managedWindow = NULL;

  windowCloseButton = NULL;
  windowCancelButton = NULL;
  windowHelpButton = NULL;

  detailedEditing = FALSE;
}

wxPropertyFormView::~wxPropertyFormView(void)
{
}

void wxPropertyFormView::ShowView(wxPropertySheet *ps, wxPanel *panel)
{
  propertySheet = ps;
  
  AssociatePanel(panel);
//  CreateControls();
//  UpdatePropertyList();
}

// Update this view of the viewed object, called e.g. by
// the object itself.
Bool wxPropertyFormView::OnUpdateView(void)
{
  return TRUE;
}

Bool wxPropertyFormView::Check(void)
{
  if (!propertySheet)
    return FALSE;
    
  wxNode *node = propertySheet->GetProperties().First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    wxPropertyValidator *validator = FindPropertyValidator(prop);
    if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
    {
      wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
      if (!formValidator->OnCheckValue(prop, this, propertyWindow))
        return FALSE;
    }
    node = node->Next();
  }
  return TRUE;
}

Bool wxPropertyFormView::TransferToPropertySheet(void)
{
  if (!propertySheet)
    return FALSE;
    
  wxNode *node = propertySheet->GetProperties().First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    wxPropertyValidator *validator = FindPropertyValidator(prop);
    if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
    {
      wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
      formValidator->OnRetrieveValue(prop, this, propertyWindow);
    }
    node = node->Next();
  }
  return TRUE;
}

Bool wxPropertyFormView::TransferToDialog(void)
{
  if (!propertySheet)
    return FALSE;
    
  wxNode *node = propertySheet->GetProperties().First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    wxPropertyValidator *validator = FindPropertyValidator(prop);
    if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
    {
      wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
      formValidator->OnDisplayValue(prop, this, propertyWindow);
    }
    node = node->Next();
  }
  return TRUE;
}

Bool wxPropertyFormView::AssociateNames(void)
{
  if (!propertySheet || !propertyWindow)
    return FALSE;

  wxNode *node = propertyWindow->GetChildren()->First();
  while (node)
  {
    wxWindow *win = (wxWindow *)node->Data();
    if (win->GetName())
    {
      wxString str(win->GetName());
      wxProperty *prop = propertySheet->GetProperty(str);
      if (prop)
        prop->SetWindow(win);
    }
    node = node->Next();
  }
  return TRUE;
}


Bool wxPropertyFormView::OnClose(void)
{
  delete this;
  return TRUE;
}

void wxPropertyFormView::OnOk(void)
{
  // Retrieve the value if any
  if (!Check())
    return;
  
  dialogCancelled = FALSE;

  managedWindow->Close(TRUE);
}

void wxPropertyFormView::OnCancel(void)
{
  dialogCancelled = TRUE;
  
  managedWindow->Close(TRUE);
}

void wxPropertyFormView::OnHelp(void)
{
}

void wxPropertyFormView::OnUpdate(void)
{
  TransferToPropertySheet();
}

void wxPropertyFormView::OnRevert(void)
{
  TransferToDialog();
}

void wxPropertyFormView::OnCommand(wxWindow& win, wxCommandEvent& event)
{
  if (!propertySheet)
    return;
    
  if (!win.GetName())
    return;
    
  if (strcmp(win.GetName(), "ok") == 0)
    OnOk();
  else if (strcmp(win.GetName(), "cancel") == 0)
    OnCancel();
  else if (strcmp(win.GetName(), "help") == 0)
    OnHelp();
  else if (strcmp(win.GetName(), "update") == 0)
    OnUpdate();
  else if (strcmp(win.GetName(), "revert") == 0)
    OnRevert();
  else
  {
    // Find a validator to route the command to.
    wxNode *node = propertySheet->GetProperties().First();
    while (node)
    {
      wxProperty *prop = (wxProperty *)node->Data();
      if (prop->GetWindow() && (prop->GetWindow() == &win))
      {
        wxPropertyValidator *validator = FindPropertyValidator(prop);
        if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
        {
          wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
          formValidator->OnCommand(prop, this, propertyWindow, event);
          return;
        }
      }
      node = node->Next();
    }
  }
}

void wxPropertyFormView::OnDoubleClick(wxItem *item)
{
  if (!propertySheet)
    return;
    
  // Find a validator to route the command to.
  wxNode *node = propertySheet->GetProperties().First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    if (prop->GetWindow() && ((wxItem *)prop->GetWindow() == item))
    {
      wxPropertyValidator *validator = FindPropertyValidator(prop);
      if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
      {
        wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
        formValidator->OnDoubleClick(prop, this, propertyWindow);
        return;
      }
    }
    node = node->Next();
  }
}

/*
 * Property form dialog box
 */
 
IMPLEMENT_CLASS(wxPropertyFormDialog, wxDialogBox)

wxPropertyFormDialog::wxPropertyFormDialog(wxPropertyFormView *v, wxWindow *parent, char *title, Bool modal, int x, int y,
    int w, int h, long style, char *name):
     wxDialogBox(parent, title, modal, x, y, w, h, style, name)
{
  view = v;
  view->AssociatePanel(this);
  view->SetManagedWindow(this);
//  SetAutoLayout(TRUE);
}

Bool wxPropertyFormDialog::OnClose(void)
{
  if (view)
    return view->OnClose();
  else
    return FALSE;
}

void wxPropertyFormDialog::OnDefaultAction(wxItem *item)
{
  view->OnDoubleClick(item);
}

void wxPropertyFormDialog::OnCommand(wxWindow& win, wxCommandEvent& event)
{
  view->OnCommand(win, event);
}

/*
 * Property form panel
 */
 
IMPLEMENT_CLASS(wxPropertyFormPanel, wxPanel)

void wxPropertyFormPanel::OnDefaultAction(wxItem *item)
{
  view->OnDoubleClick(item);
}

void wxPropertyFormPanel::OnCommand(wxWindow& win, wxCommandEvent& event)
{
  view->OnCommand(win, event);
}

/*
 * Property frame
 */
 
IMPLEMENT_CLASS(wxPropertyFormFrame, wxFrame)

Bool wxPropertyFormFrame::OnClose(void)
{
  if (view)
    return view->OnClose();
  else
    return FALSE;
}

wxPanel *wxPropertyFormFrame::OnCreatePanel(wxFrame *parent, wxPropertyFormView *v)
{
  return new wxPropertyFormPanel(v, parent);
}

Bool wxPropertyFormFrame::Initialize(void)
{
  propertyPanel = OnCreatePanel(this, view);
  if (propertyPanel)
  {
    view->AssociatePanel(propertyPanel);
    view->SetManagedWindow(this);
//    propertyPanel->SetAutoLayout(TRUE);
    return TRUE;
  }
  else
    return FALSE;
}

 /*
  * Property form specific validator
  */
  
IMPLEMENT_ABSTRACT_CLASS(wxPropertyFormValidator, wxPropertyValidator)


/*
 * Default validators
 */

IMPLEMENT_DYNAMIC_CLASS(wxRealFormValidator, wxPropertyFormValidator)

///
/// Real number form validator
/// 
Bool wxRealFormValidator::OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  if (realMin == 0.0 && realMax == 0.0)
    return TRUE;
    
  // The item used for viewing the real number: should be a text item.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow || !propertyWindow->IsKindOf(CLASSINFO(wxText)))
    return FALSE;

  wxString value(((wxText *)propertyWindow)->GetValue());

  float val = 0.0;
  if (!StringToFloat((char *)(const char *)value, &val))
  {
    char buf[200];
    sprintf(buf, "Value %s is not a valid real number!", (char *)(const char *)value);
    wxMessageBox(buf, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  
  if (val < realMin || val > realMax)
  {
    char buf[200];
    sprintf(buf, "Value must be a real number between %.2f and %.2f!", realMin, realMax);
    wxMessageBox(buf, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  return TRUE;
}

Bool wxRealFormValidator::OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the real number: should be a text item.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow || !propertyWindow->IsKindOf(CLASSINFO(wxText)))
    return FALSE;

  wxString value(((wxText *)propertyWindow)->GetValue());

  if (value.Length() == 0)
    return FALSE;
    
  float f = (float)atof((char *)(const char *)value);
  property->GetValue() = f;
  return TRUE;
}

Bool wxRealFormValidator::OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the real number: should be a text item.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow || !propertyWindow->IsKindOf(CLASSINFO(wxText)))
    return FALSE;

  wxText *textItem = (wxText *)propertyWindow;
  textItem->SetValue(FloatToString(property->GetValue().RealValue()));
  return TRUE;
}

///
/// Integer validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxIntegerFormValidator, wxPropertyFormValidator)

Bool wxIntegerFormValidator::OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  if (integerMin == 0.0 && integerMax == 0.0)
    return TRUE;
    
  // The item used for viewing the real number: should be a text item or a slider
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow)
    return FALSE;

  long val = 0;

  if (propertyWindow->IsKindOf(CLASSINFO(wxText)))
  {
    wxString value(((wxText *)propertyWindow)->GetValue());

    if (!StringToLong((char *)(const char *)value, &val))
    {
      char buf[200];
      sprintf(buf, "Value %s is not a valid integer!", (char *)(const char *)value);
      wxMessageBox(buf, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
      return FALSE;
    }
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxSlider)))
  {
    val = (long)((wxSlider *)propertyWindow)->GetValue();
  }
  else
    return FALSE;
    
  if (val < integerMin || val > integerMax)
  {
    char buf[200];
    sprintf(buf, "Value must be an integer between %ld and %ld!", integerMin, integerMax);
    wxMessageBox(buf, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  return TRUE;
}

Bool wxIntegerFormValidator::OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the real number: should be a text item or a slider
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow)
    return FALSE;

  if (propertyWindow->IsKindOf(CLASSINFO(wxText)))
  {
    wxString value(((wxText *)propertyWindow)->GetValue());

    if (value.Length() == 0)
      return FALSE;
    
    long i = atol((char *)(const char *)value);
    property->GetValue() = i;
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxSlider)))
  {
    property->GetValue() = (long)((wxSlider *)propertyWindow)->GetValue();
  }
  else
    return FALSE;
    
  return TRUE;
}

Bool wxIntegerFormValidator::OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the real number: should be a text item or a slider
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow)
    return FALSE;

  if (propertyWindow->IsKindOf(CLASSINFO(wxText)))
  {
    wxText *textItem = (wxText *)propertyWindow;
    textItem->SetValue(LongToString(property->GetValue().IntegerValue()));
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxSlider)))
  {
    ((wxSlider *)propertyWindow)->SetValue((int)property->GetValue().IntegerValue());
  }
  else
    return FALSE;
  return TRUE;
}

///
/// Boolean validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxBoolFormValidator, wxPropertyFormValidator)

Bool wxBoolFormValidator::OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the boolean: should be a checkbox
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow || !propertyWindow->IsKindOf(CLASSINFO(wxCheckBox)))
    return FALSE;

  return TRUE;
}

Bool wxBoolFormValidator::OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the boolean: should be a checkbox.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow || !propertyWindow->IsKindOf(CLASSINFO(wxCheckBox)))
    return FALSE;

  wxCheckBox *checkBox = (wxCheckBox *)propertyWindow;
  
  property->GetValue() = (Bool)checkBox->GetValue();
  return TRUE;
}

Bool wxBoolFormValidator::OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the boolean: should be a checkbox.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow || !propertyWindow->IsKindOf(CLASSINFO(wxCheckBox)))
    return FALSE;

  wxCheckBox *checkBox = (wxCheckBox *)propertyWindow;
  checkBox->SetValue((Bool)property->GetValue().BoolValue());
  return TRUE;
}

///
/// String validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxStringFormValidator, wxPropertyFormValidator)

wxStringFormValidator::wxStringFormValidator(wxStringList *list, long flags):
  wxPropertyFormValidator(flags)
{
  strings = list;
}

Bool wxStringFormValidator::OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  if (!strings)
    return TRUE;

  // The item used for viewing the string: should be a text item, choice item or listbox.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow)
    return FALSE;
  if (propertyWindow->IsKindOf(CLASSINFO(wxText)))
  {
    wxText *text = (wxText *)propertyWindow;
    if (!strings->Member(text->GetValue()))
    {
      wxString s("Value ");
      s += text->GetValue();
      s += " is not valid.";
      wxMessageBox((char *)(const char *)s, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
      return FALSE;
    }
  }
  else
  {
    // Any other item constrains the string value,
    // so we don't have to check it.
  }
  return TRUE;
}

Bool wxStringFormValidator::OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the string: should be a text item, choice item or listbox.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow)
    return FALSE;
  if (propertyWindow->IsKindOf(CLASSINFO(wxText)))
  {
    wxText *text = (wxText *)propertyWindow;
    property->GetValue() = text->GetValue();
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxListBox)))
  {
    wxListBox *lbox = (wxListBox *)propertyWindow;
    if (lbox->GetSelection() > -1)
      property->GetValue() = lbox->GetStringSelection();
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxRadioBox)))
  {
    wxRadioBox *rbox = (wxRadioBox *)propertyWindow;
    int n = 0;
    if ((n = rbox->GetSelection()) > -1)
      property->GetValue() = rbox->GetString(n);
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxChoice)))
  {
    wxChoice *choice = (wxChoice *)propertyWindow;
    if (choice->GetSelection() > -1)
      property->GetValue() = choice->GetStringSelection();
  }
  else
    return FALSE;
  return TRUE;
}

Bool wxStringFormValidator::OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the string: should be a text item, choice item or listbox.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow)
    return FALSE;
  if (propertyWindow->IsKindOf(CLASSINFO(wxText)))
  {
    wxText *text = (wxText *)propertyWindow;
    text->SetValue(property->GetValue().StringValue());
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxListBox)))
  {
    wxListBox *lbox = (wxListBox *)propertyWindow;
    if (lbox->Number() == 0 && strings)
    {
      // Try to initialize the listbox from 'strings'
      wxNode *node = strings->First();
      while (node)
      {
        char *s = (char *)node->Data();
        lbox->Append(s);
        node = node->Next();
      }
    }
    lbox->SetStringSelection(property->GetValue().StringValue());
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxRadioBox)))
  {
    wxRadioBox *rbox = (wxRadioBox *)propertyWindow;
    rbox->SetStringSelection(property->GetValue().StringValue());
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxChoice)))
  {
    wxChoice *choice = (wxChoice *)propertyWindow;
#ifndef wx_xview
    if (choice->Number() == 0 && strings)
    {
      // Try to initialize the choice item from 'strings'
      // XView doesn't allow this kind of thing.
      wxNode *node = strings->First();
      while (node)
      {
        char *s = (char *)node->Data();
        choice->Append(s);
        node = node->Next();
      }
    }
#endif
    choice->SetStringSelection(property->GetValue().StringValue());
  }
  else
    return FALSE;
  return TRUE;
}

