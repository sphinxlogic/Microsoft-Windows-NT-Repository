/*
 * File:	wx_pform.h
 * Purpose:	Property form classes
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

#ifndef wx_pformh
#define wx_pformh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_prop.h"

////
//// Property form classes: for using an existing dialog or panel
////

// Mediates between a physical panel and the property sheet
class wxPropertyFormView: public wxPropertyView
{
 DECLARE_DYNAMIC_CLASS(wxPropertyFormView)
 protected:
  Bool detailedEditing;     // E.g. using listbox for choices

  wxPanel *propertyWindow; // Panel that the controls will appear on
  wxWindow *managedWindow; // Frame or dialog
  
  wxButton *windowCloseButton; // Or OK
  wxButton *windowCancelButton;
  wxButton *windowHelpButton;
 public:
  static Bool dialogCancelled;

  wxPropertyFormView(wxPanel *propPanel = NULL, long flags = 0);
  ~wxPropertyFormView(void);

  // Associates and shows the view
  virtual void ShowView(wxPropertySheet *propertySheet, wxPanel *panel);

  // Update this view of the viewed object, called e.g. by
  // the object itself.
  virtual Bool OnUpdateView(void);

  // Transfer values from property sheet to dialog
  virtual Bool TransferToDialog(void);

  // Transfer values from dialog to property sheet
  virtual Bool TransferToPropertySheet(void);

  // Check that all the values are valid
  virtual Bool Check(void);

  // Give each property in the sheet a panel item, by matching
  // the name of the property to the name of the panel item.
  // The user doesn't always want to call this; sometimes, it
  // will have been done explicitly (e.g., no matching names).
  virtual Bool AssociateNames(void);

  virtual void OnOk(void);
  virtual void OnCancel(void);
  virtual void OnHelp(void);
  virtual void OnUpdate(void);
  virtual void OnRevert(void);
  virtual Bool OnClose(void);
  virtual void OnDoubleClick(wxItem *item);
  virtual void OnCommand(wxWindow& win, wxCommandEvent& event);

  inline virtual void AssociatePanel(wxPanel *win) { propertyWindow = win; }
  inline virtual wxPanel *GetPanel(void) { return propertyWindow; }

  inline virtual void SetManagedWindow(wxWindow *win) { managedWindow = win; }
  inline virtual wxWindow *GetManagedWindow(void) { return managedWindow; }

  inline virtual wxButton *GetWindowCloseButton() { return windowCloseButton; }
  inline virtual wxButton *GetWindowCancelButton() { return windowCancelButton; }
  inline virtual wxButton *GetHelpButton() { return windowHelpButton; }
};
 
/*
 * The type of validator used for forms (wxForm style but using an existing panel
 * or dialog box).
 * Classes derived from this know how to map from whatever widget they
 * find themselves paired with, to the wxProperty and vice versa.
 * Should the widget pointer be stored with the validator, or
 * the wxProperty? If with the property, we don't have to supply
 * a validator for every property. Otherwise, there ALWAYS needs
 * to be a validator. On the other hand, not storing a wxWindow pointer
 * in the wxProperty is more elegant. Perhaps.
 * I think on balance, should put wxWindow pointer into wxProperty.
 * After all, wxProperty will often be used to represent the data
 * assocated with a window. It's that kinda thing.
 */

class wxPropertyFormValidator: public wxPropertyValidator
{
  DECLARE_DYNAMIC_CLASS(wxPropertyFormValidator)
 protected:
 public:
  wxPropertyFormValidator(long flags = 0): wxPropertyValidator(flags) { }
  ~wxPropertyFormValidator(void) {}
  
   // Called to check value is OK (e.g. when OK is pressed)
   // Return FALSE if value didn't check out; signal to restore old value.
   virtual Bool OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
   { return TRUE; }

   // Does the transferance from the property editing area to the property itself.
   // Called by the view, e.g. when closing the window.
   virtual Bool OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow) = 0;

   // Called by the view to transfer the property to the window.
   virtual Bool OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow) = 0;

  virtual void OnDoubleClick(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow) {};
  virtual void OnSetFocus(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow) {};
  virtual void OnKillFocus(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow) {};
  virtual void OnCommand(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow, wxCommandEvent& event) {};

};

/*
 * Some default validators
 */
 
class wxRealFormValidator: public wxPropertyFormValidator
{
  DECLARE_DYNAMIC_CLASS(wxRealFormValidator)
 protected:
  float realMin;
  float realMax;
 public:
   // 0.0, 0.0 means no range
   wxRealFormValidator(float min = 0.0, float max = 0.0, long flags = 0):wxPropertyFormValidator(flags)
   {
     realMin = min; realMax = max;
   }
   ~wxRealFormValidator(void) {}

   Bool OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   Bool OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   // Called by the view to transfer the property to the window.
   Bool OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
};

class wxIntegerFormValidator: public wxPropertyFormValidator
{
  DECLARE_DYNAMIC_CLASS(wxIntegerFormValidator)
 protected:
  long integerMin;
  long integerMax;
 public:
   // 0, 0 means no range
   wxIntegerFormValidator(long min = 0, long max = 0, long flags = 0):wxPropertyFormValidator(flags)
   {
     integerMin = min; integerMax = max;
   }
   ~wxIntegerFormValidator(void) {}

   Bool OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   Bool OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   Bool OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
};

class wxBoolFormValidator: public wxPropertyFormValidator
{
  DECLARE_DYNAMIC_CLASS(wxBoolFormValidator)
 protected:
 public:
   wxBoolFormValidator(long flags = 0):wxPropertyFormValidator(flags)
   {
   }
   ~wxBoolFormValidator(void) {}

   Bool OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   Bool OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   Bool OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
};

class wxStringFormValidator: public wxPropertyFormValidator
{
  DECLARE_DYNAMIC_CLASS(wxStringFormValidator)
 protected:
  wxStringList *strings;
 public:
   wxStringFormValidator(wxStringList *list = NULL, long flags = 0);

   ~wxStringFormValidator(void)
   {
     if (strings)
       delete strings;
   }

   Bool OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   Bool OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   Bool OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
};

/*
 * A default dialog box class to use.
 */
 
class wxPropertyFormDialog: public wxDialogBox
{
  DECLARE_CLASS(wxPropertyFormDialog)
 private:
  wxPropertyFormView *view;
 public:
  wxPropertyFormDialog(wxPropertyFormView *v, wxWindow *parent, char *title, Bool modal = FALSE, int x = -1, int y = -1,
    int w = -1, int h = -1, long style = wxDEFAULT_DIALOG_STYLE, char *name = "dialogBox");
  Bool OnClose(void);
  void OnDefaultAction(wxItem *item);
  void OnCommand(wxWindow& win, wxCommandEvent& event);
};

/*
 * A default panel class to use.
 */
 
class wxPropertyFormPanel: public wxPanel
{
  DECLARE_CLASS(wxPropertyFormPanel)
 private:
  wxPropertyFormView *view;
 public:
  wxPropertyFormPanel(wxPropertyFormView *v, wxWindow *parent, int x = -1, int y = -1,
    int w = -1, int h = -1, long style = 0, char *name = "panel"):
     wxPanel(parent, x, y, w, h, style, name)
  {
    view = v;
  }
  void OnDefaultAction(wxItem *item);
  void OnCommand(wxWindow& win, wxCommandEvent& event);
};

/*
 * A default frame class to use.
 */
 
class wxPropertyFormFrame: public wxFrame
{
  DECLARE_CLASS(wxPropertyFormFrame)
 private:
  wxPropertyFormView *view;
  wxPanel *propertyPanel;
 public:
  wxPropertyFormFrame(wxPropertyFormView *v, wxFrame *parent, char *title, int x = -1, int y = -1,
    int w = -1, int h = -1, long style = wxSDI | wxDEFAULT_FRAME, char *name = "frame"):
     wxFrame(parent, title, x, y, w, h, style, name)
  {
    view = v;
    propertyPanel = NULL;
  }
  Bool OnClose(void);

  // Must call this to create panel and associate view
  virtual Bool Initialize(void);
  virtual wxPanel *OnCreatePanel(wxFrame *parent, wxPropertyFormView *v);
  inline virtual wxPanel *GetPropertyPanel(void) { return propertyPanel; }
};

#endif
