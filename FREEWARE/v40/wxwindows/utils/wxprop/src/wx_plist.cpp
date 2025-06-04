/*
 * File:	wx_plist.cc
 * Purpose:	Property list classes implementation
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
#include "wx_plist.h"

/*
 * Property text edit control
 */
 
IMPLEMENT_CLASS(wxPropertyTextEdit, wxText)

wxPropertyTextEdit::wxPropertyTextEdit(wxPropertyListView *v, wxPanel *parent, wxFunction func, char *label, char *value, int x, int y,
    int width, int height, long style, char *name):
 wxText(parent, func, label, value, x, y, width, height, style, name)
{
  view = v;
}

void wxPropertyTextEdit::OnSetFocus(void)
{
}

void wxPropertyTextEdit::OnKillFocus(void)
{
}

/*
 * Property list view
 */

IMPLEMENT_DYNAMIC_CLASS(wxPropertyListView, wxPropertyView)

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
  view->OnOk();
}

static void wxPropertyViewHelpProc(wxButton& but, wxCommandEvent& ev)
{
  wxPanel *panel = (wxPanel *)but.GetParent();
  wxPropertyView *view = (wxPropertyView *)panel->GetClientData();
  view->OnHelp();
}

Bool wxPropertyListView::dialogCancelled = FALSE;
wxBitmap *wxPropertyListView::tickBitmap = NULL;
wxBitmap *wxPropertyListView::crossBitmap = NULL;

wxPropertyListView::wxPropertyListView(wxPanel *propPanel, long flags):wxPropertyView(flags)
{
  propertyScrollingList = NULL;
  valueList = NULL;
  valueText = NULL;
  editButton = NULL;
  confirmButton = NULL;
  cancelButton = NULL;
  propertyWindow = propPanel;
  managedWindow = NULL;

  windowCloseButton = NULL;
  windowCancelButton = NULL;
  windowHelpButton = NULL;

  detailedEditing = FALSE;
}

wxPropertyListView::~wxPropertyListView(void)
{
/*
  if (tickBitmap)
    delete tickBitmap;
  if (crossBitmap)
    delete crossBitmap;
*/
}

void wxPropertyListView::ShowView(wxPropertySheet *ps, wxPanel *panel)
{
  propertySheet = ps;
  
  AssociatePanel(panel);
  CreateControls();

  UpdatePropertyList();
}

// Update this view of the viewed object, called e.g. by
// the object itself.
Bool wxPropertyListView::OnUpdateView(void)
{
  return TRUE;
}

Bool wxPropertyListView::UpdatePropertyList(Bool clearEditArea)
{
  if (!propertyScrollingList || !propertySheet)
    return FALSE;

  propertyScrollingList->Clear();
  if (clearEditArea)
  {
    valueList->Clear();
    valueText->SetValue("");
  }
  wxNode *node = propertySheet->GetProperties().First();

  // Should sort them... later...
  while (node)
  {
    wxProperty *property = (wxProperty *)node->Data();
    wxString stringValueRepr(property->GetValue().GetStringRepresentation());
    wxString paddedString(MakeNameValueString(property->GetName(), stringValueRepr));

    propertyScrollingList->Append((char *)(const char *)paddedString, (char *)(const char *)property);
    node = node->Next();
  }
  return TRUE;
}

Bool wxPropertyListView::UpdatePropertyDisplayInList(wxProperty *property)
{
  if (!propertyScrollingList || !propertySheet)
    return FALSE;

  int currentlySelected = propertyScrollingList->GetSelection();
// #ifdef wx_msw
  wxString stringValueRepr(property->GetValue().GetStringRepresentation());
  wxString paddedString(MakeNameValueString(property->GetName(), stringValueRepr));
  int sel = FindListIndexForProperty(property);

  if (sel > -1)
  {
    // Don't update the listbox unnecessarily because it can cause
    // ugly flashing.
    
    if (paddedString != propertyScrollingList->GetString(sel))
      propertyScrollingList->SetString(sel, (char *)(const char *)paddedString);
  }
//#else
//  UpdatePropertyList(FALSE);
//#endif

  if (currentlySelected > -1)
    propertyScrollingList->SetSelection(currentlySelected);

  return TRUE;
}

// Find the wxListBox index corresponding to this property
int wxPropertyListView::FindListIndexForProperty(wxProperty *property)
{
  int n = propertyScrollingList->Number();
  for (int i = 0; i < n; i++)
  {
    if (property == (wxProperty *)propertyScrollingList->wxListBox::GetClientData(i))
      return i;
  }
  return -1;
}

wxString wxPropertyListView::MakeNameValueString(wxString name, wxString value)
{
  wxString theString(name);

  int nameWidth = 25;
  int padWith = nameWidth - theString.Length();
  if (padWith < 0)
    padWith = 0;

  if (GetFlags() & wxPROP_SHOWVALUES)
  {
    // Want to pad with spaces
    theString.Append(' ', padWith);
    theString += value;
  }

  return theString;
}

// Select and show string representation in validator the given
// property. NULL resets to show no property.
Bool wxPropertyListView::ShowProperty(wxProperty *property, Bool select)
{
  if (currentProperty)
  {
    EndShowingProperty(currentProperty);
    currentProperty = NULL;
  }

  valueList->Clear();
  valueText->SetValue("");

  if (property)
  {
    currentProperty = property;
    BeginShowingProperty(property);
  }
  if (select)
  {
    int sel = FindListIndexForProperty(property);
    if (sel > -1)
      propertyScrollingList->SetSelection(sel);
  }
  return TRUE;
}

// Find appropriate validator and load property into value controls
Bool wxPropertyListView::BeginShowingProperty(wxProperty *property)
{
  currentValidator = FindPropertyValidator(property);
  if (!currentValidator)
    return FALSE;

  if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return FALSE;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

  listValidator->OnPrepareControls(property, this, propertyWindow);
  DisplayProperty(property);
  return TRUE;
}

// Find appropriate validator and unload property from value controls
Bool wxPropertyListView::EndShowingProperty(wxProperty *property)
{
  if (!currentValidator)
    return FALSE;

  RetrieveProperty(property);

  if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return FALSE;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

  listValidator->OnClearControls(property, this, propertyWindow);
  if (detailedEditing)
  {
    listValidator->OnClearDetailControls(property, this, propertyWindow);
    detailedEditing = FALSE;
  }
  return TRUE;
}

void wxPropertyListView::BeginDetailedEditing(void)
{
  if (!currentValidator)
    return;
  if (!currentProperty)
    return;
  if (detailedEditing)
    return;
  if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return;
  if (!currentProperty->IsEnabled())
    return;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

  if (listValidator->OnPrepareDetailControls(currentProperty, this, propertyWindow))
    detailedEditing = TRUE;
}

void wxPropertyListView::EndDetailedEditing(void)
{
  if (!currentValidator)
    return;
  if (!currentProperty)
    return;

  RetrieveProperty(currentProperty);

  if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

  if (detailedEditing)
  {
    listValidator->OnClearDetailControls(currentProperty, this, propertyWindow);
    detailedEditing = FALSE;
  }
}

Bool wxPropertyListView::DisplayProperty(wxProperty *property)
{
  if (!currentValidator)
    return FALSE;

  if (((currentValidator->GetFlags() & wxPROP_ALLOW_TEXT_EDITING) == 0) || !property->IsEnabled())
    valueText->SetEditable(FALSE);
  else
    valueText->SetEditable(TRUE);

  if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return FALSE;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

  listValidator->OnDisplayValue(property, this, propertyWindow);
  return TRUE;
}

Bool wxPropertyListView::RetrieveProperty(wxProperty *property)
{
  if (!currentValidator)
    return FALSE;
  if (!property->IsEnabled())
    return FALSE;

  if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return FALSE;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

  if (listValidator->OnCheckValue(property, this, propertyWindow))
  {
    if (listValidator->OnRetrieveValue(property, this, propertyWindow))
    {
      UpdatePropertyDisplayInList(property);
      OnPropertyChanged(property);
    }
  }
  else
  {
    // Revert to old value
    listValidator->OnDisplayValue(property, this, propertyWindow);
  }
  return TRUE;
}


Bool wxPropertyListView::EditProperty(wxProperty *property)
{
  return TRUE;
}

// Called by the listbox callback
Bool wxPropertyListView::OnPropertySelect(void)
{
  int sel = propertyScrollingList->GetSelection();
  if (sel > -1)
  {
    wxProperty *newSel = (wxProperty *)propertyScrollingList->wxListBox::GetClientData(sel);
    if (newSel && newSel != currentProperty)
    {
      ShowProperty(newSel, FALSE);
    }
  }
  return TRUE;
}

static void wxPropertyEditProc(wxButton& but, wxCommandEvent& ev)
{
  wxPanel *panel = (wxPanel *)but.GetParent();
  wxPropertyListView *view = (wxPropertyListView *)panel->GetClientData();
  view->OnEdit();
}

static void wxPropertyCheckProc(wxButton& but, wxCommandEvent& ev)
{
  wxPanel *panel = (wxPanel *)but.GetParent();
  wxPropertyListView *view = (wxPropertyListView *)panel->GetClientData();
  view->OnCheck();
}

static void wxPropertyCrossProc(wxButton& but, wxCommandEvent& ev)
{
  wxPanel *panel = (wxPanel *)but.GetParent();
  wxPropertyListView *view = (wxPropertyListView *)panel->GetClientData();
  view->OnCross();
}

static void wxPropertyValueTextProc(wxText& text, wxCommandEvent& ev)
{
  wxPanel *panel = (wxPanel *)text.GetParent();
  wxPropertyListView *view = (wxPropertyListView *)panel->GetClientData();
  if (ev.GetEventType() == wxEVENT_TYPE_TEXT_ENTER_COMMAND)
  {
    view->OnCheck();
  }
}

// List of properties
static void wxPropertyScrollListProc(wxListBox& lbox, wxCommandEvent& ev)
{
  wxPanel *panel = (wxPanel *)lbox.GetParent();
  wxPropertyListView *view = (wxPropertyListView *)panel->GetClientData();
  view->OnPropertySelect();
}

// List of possible values
static void wxPropertyValueListProc(wxListBox& lbox, wxCommandEvent& ev)
{
  wxPanel *panel = (wxPanel *)lbox.GetParent();
  wxPropertyListView *view = (wxPropertyListView *)panel->GetClientData();
  view->OnValueListSelect();
}

Bool wxPropertyListView::CreateControls(void)
{
  wxPanel *panel = (wxPanel *)propertyWindow;
  
  int largeButtonWidth = 50;
  int largeButtonHeight = 25;

  int smallButtonWidth = 25;
  int smallButtonHeight = 20;

  // XView must be allowed to choose its own sized buttons
#ifdef wx_xview
  largeButtonWidth = -1;
  largeButtonHeight = -1;

  smallButtonWidth = -1;
  smallButtonHeight = -1;
#endif
  
  if (valueText)
    return TRUE;
    
  if (!panel)
    return FALSE;

  wxWindow *leftMostWindow = panel;
  wxWindow *topMostWindow = panel;
  wxWindow *rightMostWindow = panel;

  wxFont *boringFont = wxTheFontList->FindOrCreateFont(11, wxMODERN, wxNORMAL, wxNORMAL);
  wxFont *niceFont = wxTheFontList->FindOrCreateFont(11, wxSWISS, wxNORMAL, wxBOLD);
  panel->SetButtonFont(niceFont);

  // May need to be changed in future to eliminate clashes with app.
  panel->SetClientData((char *)this);
  
  if (buttonFlags & wxPROP_BUTTON_OK)
  {
    windowCloseButton = new wxButton(panel, (wxFunction)wxPropertyViewOkProc, "OK",
     -1, -1, largeButtonWidth, largeButtonHeight);
    windowCloseButton->SetDefault();
    windowCloseButton->SetFocus();
  }
  if (buttonFlags & wxPROP_BUTTON_CLOSE)
  {
    windowCloseButton = new wxButton(panel, (wxFunction)wxPropertyViewOkProc, "Close",
     -1, -1, largeButtonWidth, largeButtonHeight);
  }
  if (buttonFlags & wxPROP_BUTTON_CANCEL)
  {
    windowCancelButton = new wxButton(panel, (wxFunction)wxPropertyViewCancelProc, "Cancel",
     -1, -1, largeButtonWidth, largeButtonHeight);
  }
  if (buttonFlags & wxPROP_BUTTON_HELP)
  {
    windowHelpButton = new wxButton(panel, (wxFunction)wxPropertyViewHelpProc, "Help",
     -1, -1, largeButtonWidth, largeButtonHeight);
  }

  if (windowCloseButton)
  {
    wxLayoutConstraints *c1 = new wxLayoutConstraints;
    c1->left.SameAs       (panel, wxLeft, 2);
    c1->top.SameAs        (panel, wxTop, 2);
    c1->width.AsIs();
    c1->height.AsIs();
    windowCloseButton->SetConstraints(c1);
    leftMostWindow = windowCloseButton;
  }
  if (windowCancelButton)
  {
    wxLayoutConstraints *c2 = new wxLayoutConstraints;
    if (leftMostWindow == panel)
      c2->left.SameAs       (panel, wxLeft, 2);
    else
      c2->left.RightOf      (leftMostWindow, 2);
      
    c2->top.SameAs          (panel, wxTop, 2);
    c2->width.AsIs();
    c2->height.AsIs();
    windowCancelButton->SetConstraints(c2);
    leftMostWindow = windowCancelButton;
  }
  if (windowHelpButton)
  {
    wxLayoutConstraints *c2 = new wxLayoutConstraints;
    if (leftMostWindow == panel)
      c2->left.SameAs       (panel, wxLeft, 2);
    else
      c2->left.RightOf      (leftMostWindow, 2);
      
    c2->top.SameAs          (panel, wxTop, 2);
    c2->width.AsIs();
    c2->height.AsIs();
    windowHelpButton->SetConstraints(c2);
    leftMostWindow = windowHelpButton;
  }
  
//  panel->NewLine();

  if (buttonFlags & wxPROP_BUTTON_CHECK_CROSS)
  {
/*
    if (!tickBitmap)
    {
#ifdef wx_msw
      tickBitmap = new wxBitmap("tick_bmp", wxBITMAP_TYPE_RESOURCE);
      crossBitmap =  new wxBitmap("cross_bmp", wxBITMAP_TYPE_RESOURCE);
      if (!tickBitmap || !crossBitmap || !tickBitmap->Ok() || !crossBitmap->Ok())
      {
        if (tickBitmap)
          delete tickBitmap;
        if (crossBitmap)
          delete crossBitmap;
        tickBitmap = NULL;
        crossBitmap = NULL;
      }
#endif
    }
*/
/*
    if (tickBitmap && crossBitmap)
    {
      confirmButton = new wxButton(panel, (wxFunction)wxPropertyCheckProc, tickBitmap,
       -1, -1, smallButtonWidth-5, smallButtonHeight-5);
      cancelButton = new wxButton(panel, (wxFunction)wxPropertyCrossProc, crossBitmap,
       -1, -1, smallButtonWidth-5, smallButtonHeight-5);
    }
    else
*/
    {
      confirmButton = new wxButton(panel, (wxFunction)wxPropertyCheckProc, ":-)",
       -1, -1, smallButtonWidth, smallButtonHeight);
      cancelButton = new wxButton(panel, (wxFunction)wxPropertyCrossProc, "X",
       -1, -1, smallButtonWidth, smallButtonHeight);
    }

    wxLayoutConstraints *c = new wxLayoutConstraints;
    c->left.SameAs         (panel, wxLeft, 2);
    if (windowCloseButton)
      c->top.Below         (windowCloseButton, 2);
    else
      c->top.SameAs        (panel, wxTop, 2);

    c->width.AsIs();
    c->height.AsIs();

    cancelButton->SetConstraints(c);

    c = new wxLayoutConstraints;
    c->left.RightOf        (cancelButton, 2);
    c->top.SameAs          (cancelButton, wxTop, 0);
    c->width.AsIs();
    c->height.AsIs();

    confirmButton->SetConstraints(c);

    cancelButton->Enable(FALSE);
    confirmButton->Enable(FALSE);
  }

  if (buttonFlags & wxPROP_PULLDOWN)
  {
    editButton = new wxButton(panel, (wxFunction)wxPropertyEditProc, "...",
     -1, -1, smallButtonWidth, smallButtonHeight);
    editButton->Enable(FALSE);
    wxLayoutConstraints *c = new wxLayoutConstraints;

    if (windowCloseButton)
      c->top.Below           (windowCloseButton, 2);
    else
      c->top.SameAs          (panel, wxTop, 2);

    c->right.SameAs          (panel, wxRight, 2);
    c->width.AsIs();
    c->height.AsIs();
    editButton->SetConstraints(c);
  }

  valueText = new wxPropertyTextEdit(this, panel, (wxFunction)wxPropertyValueTextProc, NULL,
     "", -1, -1, -1, -1, wxPROCESS_ENTER);
  valueText->Enable(FALSE);
  
  wxLayoutConstraints *c = new wxLayoutConstraints;

  if (cancelButton)
    c->left.RightOf        (confirmButton, 2);
  else
    c->left.SameAs         (panel, wxLeft, 2);

  if (windowCloseButton)
    c->top.Below           (windowCloseButton, 2);
  else
    c->top.SameAs          (panel, wxTop, 2);

  if (editButton)
    c->right.LeftOf        (editButton, 2);
  else
    c->right.SameAs        (panel, wxRight, 2);
  c->height.AsIs();

  valueText->SetConstraints(c);

  valueList = new wxListBox(panel, (wxFunction)wxPropertyValueListProc, NULL, wxSINGLE, -1, -1, -1, 60);
  valueList->Show(FALSE);

  c = new wxLayoutConstraints;

  c->left.SameAs         (panel, wxLeft, 2);
  c->top.Below           (valueText, 2);
  c->right.SameAs        (panel, wxRight, 2);
  c->height.Absolute(60);

  valueList->SetConstraints(c);

  panel->SetButtonFont(boringFont);
  propertyScrollingList = new wxListBox(panel, (wxFunction)wxPropertyScrollListProc, NULL, wxSINGLE,
    -1, -1, 300, 300);

  c = new wxLayoutConstraints;

  c->left.SameAs         (panel, wxLeft, 2);

  if (buttonFlags & wxPROP_DYNAMIC_VALUE_FIELD)
    c->top.Below         (valueText, 2);
  else
    c->top.Below         (valueList, 2);

  c->right.SameAs        (panel, wxRight, 2);
  c->bottom.SameAs       (panel, wxBottom, 2);

  propertyScrollingList->SetConstraints(c);

  panel->Layout();

  return TRUE;
}

void wxPropertyListView::ShowTextControl(Bool show)
{
  if (valueText)
    valueText->Show(show);
}

void wxPropertyListView::ShowListBoxControl(Bool show)
{
  if (valueList)
  {
    valueList->Show(show);
    if (buttonFlags & wxPROP_DYNAMIC_VALUE_FIELD)
    {
      wxLayoutConstraints *constraints = propertyScrollingList->GetConstraints();
      if (constraints)
      {
        if (show)
          constraints->top.Below(valueList, 2);
        else
          constraints->top.Below(valueText, 2);
        propertyWindow->Layout();
      }
    }
  }
}

void wxPropertyListView::EnableCheck(Bool show)
{
  if (confirmButton)
    confirmButton->Enable(show);
}

void wxPropertyListView::EnableCross(Bool show)
{
  if (cancelButton)
    cancelButton->Enable(show);
}

Bool wxPropertyListView::OnClose(void)
{
  // Retrieve the value if any
  OnCheck();
  
  delete this;
  return TRUE;
}

Bool wxPropertyListView::OnValueListSelect(void)
{
  if (currentProperty && currentValidator)
  {
    if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
      return FALSE;

    wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

    listValidator->OnValueListSelect(currentProperty, this, propertyWindow);
  }
  return TRUE;
}

void wxPropertyListView::OnOk(void)
{
  // Retrieve the value if any
  OnCheck();
  
  managedWindow->Close(TRUE);
}

void wxPropertyListView::OnCancel(void)
{
  managedWindow->Close(TRUE);
  dialogCancelled = TRUE;
}

void wxPropertyListView::OnHelp(void)
{
}

void wxPropertyListView::OnCheck(void)
{
  if (currentProperty)
  {
    RetrieveProperty(currentProperty);
  }
}

void wxPropertyListView::OnCross(void)
{
  if (currentProperty && currentValidator)
  {
    if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
      return;

    wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

    // Revert to old value
    listValidator->OnDisplayValue(currentProperty, this, propertyWindow);
  }
}

void wxPropertyListView::OnDoubleClick(void)
{
  if (currentProperty && currentValidator)
  {
    if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
      return;

    wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

    // Revert to old value
    listValidator->OnDoubleClick(currentProperty, this, propertyWindow);
  }
}

void wxPropertyListView::OnEdit(void)
{
  if (currentProperty && currentValidator)
  {
    if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
      return;

    wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

    listValidator->OnEdit(currentProperty, this, propertyWindow);
  }
}

/*
 * Property dialog box
 */
 
IMPLEMENT_CLASS(wxPropertyListDialog, wxDialogBox)

wxPropertyListDialog::wxPropertyListDialog(wxPropertyListView *v, wxWindow *parent, char *title, Bool modal, int x, int y,
    int w, int h, long style, char *name):
     wxDialogBox(parent, title, modal, x, y, w, h, style, name)
{
  view = v;
  view->AssociatePanel(this);
  view->SetManagedWindow(this);
  SetAutoLayout(TRUE);
}

Bool wxPropertyListDialog::OnClose(void)
{
  if (view)
    return view->OnClose();
  else
    return FALSE;
}

Bool wxPropertyListDialog::OnCharHook(wxKeyEvent& event)
{
  if (event.keyCode == WXK_ESCAPE)
  {
    this->Close();
//    if (OnClose())
//      delete this;
  }
  return FALSE;
}

void wxPropertyListDialog::OnDefaultAction(wxItem *item)
{
  if (item == view->GetPropertyScrollingList())
    view->OnDoubleClick();
}

/*
 * Property panel
 */
 
IMPLEMENT_CLASS(wxPropertyListPanel, wxPanel)

void wxPropertyListPanel::OnDefaultAction(wxItem *item)
{
  if (item == view->GetPropertyScrollingList())
    view->OnDoubleClick();
}

/*
 * Property frame
 */
 
IMPLEMENT_CLASS(wxPropertyListFrame, wxFrame)

Bool wxPropertyListFrame::OnClose(void)
{
  if (view)
    return view->OnClose();
  else
    return FALSE;
}

wxPanel *wxPropertyListFrame::OnCreatePanel(wxFrame *parent, wxPropertyListView *v)
{
  return new wxPropertyListPanel(v, parent);
}

Bool wxPropertyListFrame::Initialize(void)
{
  propertyPanel = OnCreatePanel(this, view);
  if (propertyPanel)
  {
    view->AssociatePanel(propertyPanel);
    view->SetManagedWindow(this);
    propertyPanel->SetAutoLayout(TRUE);
    return TRUE;
  }
  else
    return FALSE;
}

 /*
  * Property list specific validator
  */
  
IMPLEMENT_ABSTRACT_CLASS(wxPropertyListValidator, wxPropertyValidator)

Bool wxPropertyListValidator::OnSelect(Bool select, wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
//  view->GetValueText()->Show(TRUE);
  if (select)
    OnDisplayValue(property, view, parentWindow);

  return TRUE;
}

Bool wxPropertyListValidator::OnValueListSelect(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  char *s = view->GetValueList()->GetStringSelection();
  if (s)
  {
    s = copystring(s);
    view->GetValueText()->SetValue(s);
    view->RetrieveProperty(property);

    delete[] s;
  }
  return TRUE;
}

Bool wxPropertyListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
//  view->GetValueText()->Show(TRUE);
  wxString str(property->GetValue().GetStringRepresentation());

  view->GetValueText()->SetValue((char *)(const char *)str);
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
Bool wxPropertyListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  return FALSE;
}

void wxPropertyListValidator::OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetDetailedEditing())
    view->EndDetailedEditing();
  else
    view->BeginDetailedEditing();
}

Bool wxPropertyListValidator::OnClearControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(FALSE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(FALSE);
  if (view->GetEditButton())
    view->GetEditButton()->Enable(FALSE);
  return TRUE;
}

/*
 * Default validators
 */

IMPLEMENT_DYNAMIC_CLASS(wxRealListValidator, wxPropertyListValidator)

///
/// Real number validator
/// 
Bool wxRealListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (realMin == 0.0 && realMax == 0.0)
    return TRUE;
    
  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());

  float val = 0.0;
  if (!StringToFloat((char *)(const char *)value, &val))
  {
    char buf[200];
    sprintf(buf, "Value %s is not a valid real number!", value.GetData());
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

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
Bool wxRealListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;

  if (strlen(view->GetValueText()->GetValue()) == 0)
    return FALSE;
    
  wxString value(view->GetValueText()->GetValue());
  float f = (float)atof((char *)(const char *)value);
  property->GetValue() = f;
  return TRUE;
}

Bool wxRealListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(TRUE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(TRUE);
  if (view->GetEditButton())
    view->GetEditButton()->Enable(FALSE);
  if (view->GetValueText())
    view->GetValueText()->Enable(TRUE);
  return TRUE;
}

///
/// Integer validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxIntegerListValidator, wxPropertyListValidator)

Bool wxIntegerListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (integerMin == 0 && integerMax == 0)
    return TRUE;
    
  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());

  long val = 0;
  if (!StringToLong((char *)(const char *)value, &val))
  {
    char buf[200];
    sprintf(buf, "Value %s is not a valid integer!", (char *)(const char *)value);
    wxMessageBox(buf, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  if (val < integerMin || val > integerMax)
  {
    char buf[200];
    sprintf(buf, "Value must be an integer between %ld and %ld!", integerMin, integerMax);
    wxMessageBox(buf, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
Bool wxIntegerListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;

  if (strlen(view->GetValueText()->GetValue()) == 0)
    return FALSE;
    
  wxString value(view->GetValueText()->GetValue());
  long val = (long)atoi((char *)(const char *)value);
  property->GetValue() = (long)val;
  return TRUE;
}

Bool wxIntegerListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(TRUE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(TRUE);
  if (view->GetEditButton())
    view->GetEditButton()->Enable(FALSE);
  if (view->GetValueText())
    view->GetValueText()->Enable(TRUE);
  return TRUE;
}

///
/// Boolean validatir
/// 
IMPLEMENT_DYNAMIC_CLASS(wxBoolListValidator, wxPropertyListValidator)

Bool wxBoolListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());
  if (value != "True" && value != "False")
  {
    wxMessageBox("Value must be True or False!", "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
Bool wxBoolListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;

  if (strlen(view->GetValueText()->GetValue()) == 0)
    return FALSE;
    
  wxString value(view->GetValueText()->GetValue());
  Bool boolValue = FALSE;
  if (value == "True")
    boolValue = TRUE;
  else
    boolValue = FALSE;
  property->GetValue() = (Bool)boolValue;
  return TRUE;
}

Bool wxBoolListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString str(property->GetValue().GetStringRepresentation());

  view->GetValueText()->SetValue((char *)(const char *)str);
  view->GetValueList()->SetStringSelection((char *)(const char *)str);
  return TRUE;
}

Bool wxBoolListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(FALSE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(FALSE);
  if (view->GetEditButton())
    view->GetEditButton()->Enable(TRUE);
  if (view->GetValueText())
    view->GetValueText()->Enable(FALSE);
  return TRUE;
}

Bool wxBoolListValidator::OnPrepareDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetValueList())
  {
    view->ShowListBoxControl(TRUE);
    view->GetValueList()->Enable(TRUE);
    
    view->GetValueList()->Append("True");
    view->GetValueList()->Append("False");
    char *currentString = copystring(view->GetValueText()->GetValue());
    view->GetValueList()->SetStringSelection(currentString);
    delete[] currentString;
  }
  return TRUE;
}

Bool wxBoolListValidator::OnClearDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetValueList())
  {
    view->GetValueList()->Clear();
    view->ShowListBoxControl(FALSE);
    view->GetValueList()->Enable(FALSE);
  }
  return TRUE;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
Bool wxBoolListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  if (property->GetValue().BoolValue())
    property->GetValue() = (Bool)FALSE;
  else
    property->GetValue() = (Bool)TRUE;
  view->DisplayProperty(property);
  view->UpdatePropertyDisplayInList(property);
  view->OnPropertyChanged(property);
  return TRUE;
}

///
/// String validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxStringListValidator, wxPropertyListValidator)

wxStringListValidator::wxStringListValidator(wxStringList *list, long flags):
  wxPropertyListValidator(flags)
{
  strings = list;
  // If no constraint, we just allow the string to be edited.
  if (!strings && ((validatorFlags & wxPROP_ALLOW_TEXT_EDITING) == 0))
    validatorFlags |= wxPROP_ALLOW_TEXT_EDITING;
}

Bool wxStringListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!strings)
    return TRUE;

  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());

  if (!strings->Member((char *)(const char *)value))
  {
    wxString s("Value ");
    s += value;
    s += " is not valid.";
    wxMessageBox((char *)(const char *)s, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
Bool wxStringListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());
  property->GetValue() = value;
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
Bool wxStringListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString str(property->GetValue().GetStringRepresentation());
  view->GetValueText()->SetValue((char *)(const char *)str);
  if (strings)
  {
    view->GetValueList()->SetStringSelection((char *)(const char *)str);
  }
  return TRUE;
}

Bool wxStringListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  // Unconstrained
  if (!strings)
  {
    if (view->GetEditButton())
      view->GetEditButton()->Enable(FALSE);
    if (view->GetConfirmButton())
      view->GetConfirmButton()->Enable(TRUE);
    if (view->GetCancelButton())
      view->GetCancelButton()->Enable(TRUE);
    if (view->GetValueText())
      view->GetValueText()->Enable(TRUE);
    return TRUE;
  }
  
  // Constrained
  if (view->GetValueText())
    view->GetValueText()->Enable(FALSE);

  if (view->GetEditButton())
    view->GetEditButton()->Enable(TRUE);

  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(FALSE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(FALSE);
  return TRUE;
}

Bool wxStringListValidator::OnPrepareDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetValueList())
  {
    view->ShowListBoxControl(TRUE);
    view->GetValueList()->Enable(TRUE);
    wxNode *node = strings->First();
    while (node)
    {
      char *s = (char *)node->Data();
      view->GetValueList()->Append(s);
      node = node->Next();
    }
    char *currentString = property->GetValue().StringValue();
    view->GetValueList()->SetStringSelection(currentString);
  }
  return TRUE;
}

Bool wxStringListValidator::OnClearDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!strings)
  {
    return TRUE;
  }

  if (view->GetValueList())
  {
    view->GetValueList()->Clear();
    view->ShowListBoxControl(FALSE);
    view->GetValueList()->Enable(FALSE);
  }
  return TRUE;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
Bool wxStringListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  if (!strings)
    return FALSE;

  wxNode *node = strings->First();
  char *currentString = property->GetValue().StringValue();
  while (node)
  {
    char *s = (char *)node->Data();
    if (strcmp(s, currentString) == 0)
    {
      char *nextString = NULL;
      if (node->Next())
        nextString = (char *)node->Next()->Data();
      else
        nextString = (char *)strings->First()->Data();
      property->GetValue() = nextString;
      view->DisplayProperty(property);
      view->UpdatePropertyDisplayInList(property);
      view->OnPropertyChanged(property);
      return TRUE;
    }
    else node = node->Next();
  }
  return TRUE;
}

///
/// Filename validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxFilenameListValidator, wxPropertyListValidator)

wxFilenameListValidator::wxFilenameListValidator(wxString message , wxString wildcard, long flags):
  wxPropertyListValidator(flags), filenameWildCard(wildcard), filenameMessage(message)
{
}

wxFilenameListValidator::~wxFilenameListValidator(void)
{
}

Bool wxFilenameListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
Bool wxFilenameListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());
  property->GetValue() = (char *)(const char *)value;
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
Bool wxFilenameListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString str(property->GetValue().GetStringRepresentation());
  view->GetValueText()->SetValue((char *)(const char *)str);
  return TRUE;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
Bool wxFilenameListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  OnEdit(property, view, parentWindow);
  return TRUE;
}

Bool wxFilenameListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(TRUE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(TRUE);
  if (view->GetEditButton())
    view->GetEditButton()->Enable(TRUE);
  if (view->GetValueText())
    view->GetValueText()->Enable((GetFlags() & wxPROP_ALLOW_TEXT_EDITING) == wxPROP_ALLOW_TEXT_EDITING);
  return TRUE;
}

void wxFilenameListValidator::OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return;

  char *s = wxFileSelector(
     (char *)(const char *)filenameMessage,
     wxPathOnly(property->GetValue().StringValue()),
     wxFileNameFromPath(property->GetValue().StringValue()),
     NULL,
     (char *)(const char *)filenameWildCard,
     0,
     parentWindow);
  if (s)
  {
    property->GetValue() = s;
    view->DisplayProperty(property);
    view->UpdatePropertyDisplayInList(property);
    view->OnPropertyChanged(property);
  }
}

///
/// Colour validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxColourListValidator, wxPropertyListValidator)

wxColourListValidator::wxColourListValidator(long flags):
  wxPropertyListValidator(flags)
{
}

wxColourListValidator::~wxColourListValidator(void)
{
}

Bool wxColourListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
Bool wxColourListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());
  
  
  property->GetValue() = value;
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
Bool wxColourListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString str(property->GetValue().GetStringRepresentation());
  view->GetValueText()->SetValue((char *)(const char *)str);
  return TRUE;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
Bool wxColourListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  OnEdit(property, view, parentWindow);
  return TRUE;
}

Bool wxColourListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(TRUE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(TRUE);
  if (view->GetEditButton())
    view->GetEditButton()->Enable(TRUE);
  if (view->GetValueText())
    view->GetValueText()->Enable((GetFlags() & wxPROP_ALLOW_TEXT_EDITING) == wxPROP_ALLOW_TEXT_EDITING);
  return TRUE;
}

void wxColourListValidator::OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return;
    
  char *s = property->GetValue().StringValue();
  int r = 0;
  int g = 0;
  int b = 0;
  if (s)
  {
    r = wxHexToDec(s);
    g = wxHexToDec(s+2);
    b = wxHexToDec(s+4);
  }
  
  wxColour col(r,g,b);
  
  wxColourData data;
  data.SetChooseFull(TRUE);
  data.SetColour(col);
  
  for (int i = 0; i < 16; i++)
  {
    wxColour colour(i*16, i*16, i*16);
    data.SetCustomColour(i, colour);
  }
      
  wxColourDialog dialog(parentWindow, &data);
  if (dialog.Show(TRUE))
  {
    wxColourData retData = dialog.GetColourData();
    col = retData.GetColour();
    
    char buf[7];
    wxDecToHex(col.Red(), buf);
    wxDecToHex(col.Green(), buf+2);
    wxDecToHex(col.Blue(), buf+4);

    property->GetValue() = buf;
    view->DisplayProperty(property);
    view->UpdatePropertyDisplayInList(property);
    view->OnPropertyChanged(property);
  }
}

///
/// List of strings validator. For this we need more user interface than
/// we get with a property list; so create a new dialog for editing the list.
///
IMPLEMENT_DYNAMIC_CLASS(wxListOfStringsListValidator, wxPropertyListValidator)

wxListOfStringsListValidator::wxListOfStringsListValidator(long flags):
  wxPropertyListValidator(flags)
{
}

Bool wxListOfStringsListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  // No constraints for an arbitrary, user-editable list of strings.
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself.
// In this case, the user cannot directly edit the string list.
Bool wxListOfStringsListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  return TRUE;
}

Bool wxListOfStringsListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString str(property->GetValue().GetStringRepresentation());
  view->GetValueText()->SetValue((char *)(const char *)str);
  return TRUE;
}

Bool wxListOfStringsListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetEditButton())
    view->GetEditButton()->Enable(TRUE);
  if (view->GetValueText())
    view->GetValueText()->Enable(FALSE);

  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(FALSE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(FALSE);
  return TRUE;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
Bool wxListOfStringsListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  OnEdit(property, view, parentWindow);
  return TRUE;
}

void wxListOfStringsListValidator::OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  // Convert property value to a list of strings for editing
  wxStringList *stringList = new wxStringList;
  
  wxPropertyValue *expr = property->GetValue().GetFirst();
  while (expr)
  {
    char *s = expr->StringValue();
    if (s)
      stringList->Add(s);
    expr = expr->GetNext();
  }
  
  wxString title("Editing ");
  title += property->GetName();
  
  if (EditStringList(parentWindow, stringList, (char *)(const char *)title))
  {
    wxPropertyValue& oldValue = property->GetValue();
    oldValue.ClearList();
    wxNode *node = stringList->First();
    while (node)
    {
      char *s = (char *)node->Data();
      oldValue.Append(new wxPropertyValue(s));
      
      node = node->Next();
    }
    
    view->DisplayProperty(property);
    view->UpdatePropertyDisplayInList(property);
    view->OnPropertyChanged(property);
  }
  delete stringList;
}

static void StringListEditorStringsProc(wxListBox& lbox, wxCommandEvent& event);
static void StringListEditorDeleteProc(wxButton& but, wxCommandEvent& event);
static void StringListEditorAddProc(wxButton& but, wxCommandEvent& event);
static void StringListEditorOkProc(wxButton& but, wxCommandEvent& event);
static void StringListEditorCancelProc(wxButton& but, wxCommandEvent& event);
static void StringListEditorHelpProc(wxButton& but, wxCommandEvent& event);
static void StringListEditorTextProc(wxText& text, wxCommandEvent& event);

class wxPropertyStringListEditorDialog: public wxDialogBox
{
  public:
    wxStringList *stringList;
    wxListBox *listBox;
    wxText *stringText;
    static Bool dialogCancelled;
    int currentSelection;
    wxPropertyStringListEditorDialog(wxWindow *parent, char *title, Bool modal, int x = -1, int y = -1,
      int width = -1, int height = -1, long windowStyle = wxDEFAULT_DIALOG_STYLE, char *name = "stringEditorDialogBox"):
       wxDialogBox(parent, title, modal, x, y, width, height, windowStyle, name)
    {
      stringList = NULL;
      stringText = NULL;
      listBox = NULL;
      dialogCancelled = FALSE;
      currentSelection = -1;
    }
    ~wxPropertyStringListEditorDialog(void) {}
    Bool OnClose(void);
    void SaveCurrentSelection(void);
    void ShowCurrentSelection(void);
};

class wxPropertyStringListEditorText: public wxText
{
 public:
  wxPropertyStringListEditorText(wxPanel *parent, wxFunction func, char *label, char *val, int x = -1, int y = -1,
    int width = -1, int height = -1, long windowStyle = 0, char *name = "text"):
     wxText(parent, func, label, val, x, y, width, height, windowStyle, name)
  {
  }
  void OnKillFocus(void)
  {
    wxPropertyStringListEditorDialog *dialog = (wxPropertyStringListEditorDialog *)GetParent();
    dialog->SaveCurrentSelection();
  }
};

Bool wxPropertyStringListEditorDialog::dialogCancelled = FALSE;

// Edit the string list.
Bool wxListOfStringsListValidator::EditStringList(wxWindow *parent, wxStringList *stringList, const char *title)
{
  wxBeginBusyCursor();
  wxPropertyStringListEditorDialog *dialog = new wxPropertyStringListEditorDialog(parent, (char *)title, TRUE, 10, 10, 400, 400);
  
  dialog->stringList = stringList;
  
//  dialog->SetLabelFont(gySmallLabelFont);
//  dialog->SetButtonFont(gySmallButtonFont);
  dialog->SetLabelPosition(wxVERTICAL);

  wxButton *okButton = new wxButton(dialog, (wxFunction)StringListEditorOkProc, "OK");
  wxButton *cancelButton = new wxButton(dialog, (wxFunction)StringListEditorCancelProc, "Cancel");

//  wxButton *helpButton = new wxButton(dialog, (wxFunction)StringListEditorHelpProc, "Help");
//  helpButton->SetClientData((char *)this);
  dialog->NewLine();

  dialog->listBox = new wxListBox(dialog, (wxFunction)StringListEditorStringsProc,
    "Strings", wxSINGLE, -1, -1, 300, 200);

  dialog->NewLine();
  dialog->stringText = new wxPropertyStringListEditorText(dialog, (wxFunction)StringListEditorTextProc, NULL, "", -1, -1,
       300, -1, wxPROCESS_ENTER);
  dialog->stringText->Enable(FALSE);

  dialog->NewLine();
  wxButton *addButton = new wxButton(dialog, (wxFunction)StringListEditorAddProc, "Add");
  wxButton *deleteButton = new wxButton(dialog, (wxFunction)StringListEditorDeleteProc, "Delete");

  wxNode *node = stringList->First();
  while (node)
  {
    char *str = (char *)node->Data();
    // Save node as client data for each listbox item
    dialog->listBox->Append(str, (char *)node);
    node = node->Next();
  }

  dialog->Fit();
  dialog->Centre(wxBOTH);
  wxEndBusyCursor();
  dialog->Show(TRUE);
  
  return (!wxPropertyStringListEditorDialog::dialogCancelled);
}

/*
 * String list editor callbacks
 *
 */

static void StringListEditorStringsProc(wxListBox& lbox, wxCommandEvent& event)
{
  wxPropertyStringListEditorDialog *dialog = (wxPropertyStringListEditorDialog *)lbox.GetParent();

  int sel = lbox.GetSelection();
  if (sel > -1)
  {
//    dialog->SaveCurrentSelection();
    
    dialog->currentSelection = sel;

    dialog->ShowCurrentSelection();
  }
}

static void StringListEditorDeleteProc(wxButton& but, wxCommandEvent& event)
{
  wxPropertyStringListEditorDialog *dialog = (wxPropertyStringListEditorDialog *)but.GetParent();
  
  int sel = dialog->listBox->GetSelection();
  if (sel == -1)
    return;
    
  wxNode *node = (wxNode *)dialog->listBox->wxListBox::GetClientData(sel);
  if (!node)
    return;
    
  dialog->listBox->Delete(sel);
  delete[] (char *)node->Data();
  delete node;
  dialog->currentSelection = -1;
  dialog->stringText->SetValue("");
}

static void StringListEditorAddProc(wxButton& but, wxCommandEvent& event)
{
  wxPropertyStringListEditorDialog *dialog = (wxPropertyStringListEditorDialog *)but.GetParent();
  dialog->SaveCurrentSelection();
  
  char *initialText = "";
  wxNode *node = dialog->stringList->Add(initialText);
  dialog->listBox->Append(initialText, (char *)node);
  dialog->currentSelection = dialog->stringList->Number() - 1;
  dialog->listBox->SetSelection(dialog->currentSelection);
  dialog->ShowCurrentSelection();
  dialog->stringText->SetFocus();
}

static void StringListEditorOkProc(wxButton& but, wxCommandEvent& event)
{
  wxPropertyStringListEditorDialog *dialog = (wxPropertyStringListEditorDialog *)but.GetParent();
  dialog->SaveCurrentSelection();
  dialog->Show(FALSE);
  delete dialog;
}

static void StringListEditorCancelProc(wxButton& but, wxCommandEvent& event)
{
  wxPropertyStringListEditorDialog *dialog = (wxPropertyStringListEditorDialog *)but.GetParent();
  dialog->dialogCancelled = TRUE;
  dialog->Show(FALSE);
  delete dialog;
}

static void StringListEditorHelpProc(wxButton& but, wxCommandEvent& event)
{
  wxPropertyStringListEditorDialog *dialog = (wxPropertyStringListEditorDialog *)but.GetParent();
}

static void StringListEditorTextProc(wxText& text, wxCommandEvent& event)
{
  wxPropertyStringListEditorDialog *dialog = (wxPropertyStringListEditorDialog *)text.GetParent();
  if (event.GetEventType() == wxEVENT_TYPE_TEXT_ENTER_COMMAND)
  {
    dialog->SaveCurrentSelection();
  }
}

Bool wxPropertyStringListEditorDialog::OnClose(void)
{
  SaveCurrentSelection();
  return TRUE;
}

void wxPropertyStringListEditorDialog::SaveCurrentSelection(void)
{
  if (currentSelection == -1)
    return;
    
  wxNode *node = (wxNode *)listBox->wxListBox::GetClientData(currentSelection);
  if (!node)
    return;
    
  char *txt = stringText->GetValue();
  if (node->Data())
    delete[] (char *)node->Data();
  node->SetData((wxObject *)copystring(txt));
  
  listBox->SetString(currentSelection, (char *)node->Data());
}

void wxPropertyStringListEditorDialog::ShowCurrentSelection(void)
{
  if (currentSelection == -1)
  {
    stringText->SetValue("");
    return;
  }
  wxNode *node = (wxNode *)listBox->wxListBox::GetClientData(currentSelection);
  char *txt = (char *)node->Data();
  stringText->SetValue(txt);
  stringText->Enable(TRUE);
}
