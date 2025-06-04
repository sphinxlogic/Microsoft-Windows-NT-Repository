/*
 * File:	wx_rprop.h
 * Purpose:	Resource editor property classes
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

#ifndef wx_rproph
#define wx_rproph

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_resed.h"

class wxPropertyInfo;

// A kind of property list view that intercepts OnPropertyChanged
// feedback.
class wxResourcePropertyListView: public wxPropertyListView
{
 public:
   wxPropertyInfo *propertyInfo;
   
   wxResourcePropertyListView(wxPropertyInfo *info, wxPanel *propPanel = NULL, long flags = wxPROP_BUTTON_DEFAULT):
     wxPropertyListView(propPanel, flags)
   {
     propertyInfo = info;
   }
   void OnPropertyChanged(wxProperty *property);
   Bool OnClose(void);
};

// Generic class for relating an object to a collection of properties.
// Instead of defining new functions like wxButton::GetProperty, wxButton::SetProperty,
// we take these functions out into of the wxWindows library and associate
// them with separate classes.
class wxPropertyInfo: public wxObject
{
 protected:
  wxPropertyInfo(void)
  {
  }
  ~wxPropertyInfo(void)
  {
  }
 public:
  virtual wxProperty *GetProperty(wxString& propName) = 0;
  virtual Bool SetProperty(wxString& propName, wxProperty *property) = 0;
  virtual void GetPropertyNames(wxStringList& names) = 0;
  virtual Bool Edit(wxWindow *parent, char *title);
};

// For all windows
class wxWindowPropertyInfo: public wxPropertyInfo
{
 protected:
  wxWindow *propertyWindow;
  wxItemResource *propertyResource;
 public:
  wxWindowPropertyInfo(wxWindow *win, wxItemResource *res = NULL);
  ~wxWindowPropertyInfo(void);
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  
  inline void SetPropertyWindow(wxWindow *win) { propertyWindow = win; }
  
  inline void SetResource(wxItemResource *res) { propertyResource = res; }
  
  // Helper functions for font properties
  
  wxProperty *GetFontProperty(wxString& name, wxFont *font);
  wxFont *SetFontProperty(wxString& name, wxProperty *property, wxFont *oldFont);
  
  // Fill in the wxItemResource members to mirror the current window settings
  virtual Bool InstantiateResource(wxItemResource *resource);
};

// For panel items
class wxItemPropertyInfo: public wxWindowPropertyInfo
{
 protected:
 public:
  wxItemPropertyInfo(wxWindow *win, wxItemResource *res = NULL):
    wxWindowPropertyInfo(win, res) {}
  ~wxItemPropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);
};

// For buttons
class wxButtonPropertyInfo: public wxItemPropertyInfo
{
 protected:
 public:
  Bool isBitmapButton;
  wxButtonPropertyInfo(wxWindow *win, wxItemResource *res = NULL, Bool bmButton = FALSE):
    wxItemPropertyInfo(win, res) { isBitmapButton = bmButton; }
  ~wxButtonPropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);

  inline Bool IsBitmapButton(void) { return isBitmapButton; }
};

// For messages
class wxMessagePropertyInfo: public wxItemPropertyInfo
{
 protected:
 public:
  Bool isBitmapMessage;
  wxMessagePropertyInfo(wxWindow *win, wxItemResource *res = NULL, Bool bmMessage = FALSE):
    wxItemPropertyInfo(win, res) { isBitmapMessage = bmMessage; }
  ~wxMessagePropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);
  
  inline Bool IsBitmapMessage(void) { return isBitmapMessage; }
};

// For text/multitext items
class wxTextPropertyInfo: public wxItemPropertyInfo
{
 protected:
 public:
  wxTextPropertyInfo(wxWindow *win, wxItemResource *res = NULL):
    wxItemPropertyInfo(win, res) {}
  ~wxTextPropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);
};

// For list boxes
class wxListBoxPropertyInfo: public wxItemPropertyInfo
{
 protected:
 public:
  wxListBoxPropertyInfo(wxWindow *win, wxItemResource *res = NULL):
    wxItemPropertyInfo(win, res) {}
  ~wxListBoxPropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);
};

// For choice items
class wxChoicePropertyInfo: public wxItemPropertyInfo
{
 protected:
 public:
  wxChoicePropertyInfo(wxWindow *win, wxItemResource *res = NULL):
    wxItemPropertyInfo(win, res) {}
  ~wxChoicePropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);
};

// For radiobox items
class wxRadioBoxPropertyInfo: public wxItemPropertyInfo
{
 protected:
 public:
  wxRadioBoxPropertyInfo(wxWindow *win, wxItemResource *res = NULL):
    wxItemPropertyInfo(win, res) {}
  ~wxRadioBoxPropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);
};

// For groupbox items
class wxGroupBoxPropertyInfo: public wxItemPropertyInfo
{
 protected:
 public:
  wxGroupBoxPropertyInfo(wxWindow *win, wxItemResource *res = NULL):
    wxItemPropertyInfo(win, res) {}
  ~wxGroupBoxPropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);
};

// For checkbox items
class wxCheckBoxPropertyInfo: public wxItemPropertyInfo
{
 protected:
 public:
  wxCheckBoxPropertyInfo(wxWindow *win, wxItemResource *res = NULL):
    wxItemPropertyInfo(win, res) {}
  ~wxCheckBoxPropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);
};

// For gauge items
class wxGaugePropertyInfo: public wxItemPropertyInfo
{
 protected:
 public:
  wxGaugePropertyInfo(wxWindow *win, wxItemResource *res = NULL):
    wxItemPropertyInfo(win, res) {}
  ~wxGaugePropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);
};

// For scrollbar items
class wxScrollBarPropertyInfo: public wxItemPropertyInfo
{
 protected:
 public:
  wxScrollBarPropertyInfo(wxWindow *win, wxItemResource *res = NULL):
    wxItemPropertyInfo(win, res) {}
  ~wxScrollBarPropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);
};

// For slider items
class wxSliderPropertyInfo: public wxItemPropertyInfo
{
 protected:
 public:
  wxSliderPropertyInfo(wxWindow *win, wxItemResource *res = NULL):
    wxItemPropertyInfo(win, res) {}
  ~wxSliderPropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);
};

// For panels
class wxPanelPropertyInfo: public wxWindowPropertyInfo
{
 protected:
 public:
  wxPanelPropertyInfo(wxWindow *win, wxItemResource *res = NULL):
    wxWindowPropertyInfo(win, res) {}
  ~wxPanelPropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);
};

// For dialogs
class wxDialogPropertyInfo: public wxPanelPropertyInfo
{
 protected:
 public:
  wxDialogPropertyInfo(wxWindow *win, wxItemResource *res = NULL):
    wxPanelPropertyInfo(win, res) {}
  ~wxDialogPropertyInfo(void) {}
  wxProperty *GetProperty(wxString& name);
  Bool SetProperty(wxString& name, wxProperty *property);
  void GetPropertyNames(wxStringList& names);
  Bool InstantiateResource(wxItemResource *resource);
};

int wxStringToFontWeight(wxString& val);
int wxStringToFontStyle(wxString& val);
int wxStringToFontFamily(wxString& val);

#endif
