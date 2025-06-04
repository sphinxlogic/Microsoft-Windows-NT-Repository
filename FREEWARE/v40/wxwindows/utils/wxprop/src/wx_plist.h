/*
 * File:	wx_plist.h
 * Purpose:	Property list classes
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

 /*

 TO DO:

 (1) Optional popup-help for each item, and an optional Help button
   for dialog.

 (2) Align Ok, Cancel, Help buttons properly.

 (3) Consider retrieving the rectangle on the panel that can be
 drawn into (where the value listbox is) and giving an example
 of editing graphically. May be too fancy.

 (4) Deriveable types for wxPropertyValue => may need to reorganise
 wxPropertyValue to use inheritance rather than present all-types-in-one
 scheme.

 (5) Optional popup panel for value list, perhaps.

 (6) Floating point checking routine still crashes with Floating
 point error for zany input.

 (7) Property sheet with choice (or listbox) to select alternative
 sheets... multiple views per panel, only one active. For this
 we really need a wxChoice that can be dynamically set: XView
 may be a problem; Motif?

 (8) More example validators, e.g. colour selector.
  */

#ifndef wx_plisth
#define wx_plisth

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_prop.h"

#define wxPROP_BUTTON_CLOSE       1
#define wxPROP_BUTTON_OK          2
#define wxPROP_BUTTON_CANCEL      4
#define wxPROP_BUTTON_CHECK_CROSS 8
#define wxPROP_BUTTON_HELP        16
#define wxPROP_DYNAMIC_VALUE_FIELD 32
#define wxPROP_PULLDOWN           64
#define wxPROP_SHOWVALUES         128

#ifdef wx_xview
#define wxPROP_BUTTON_DEFAULT wxPROP_BUTTON_OK | wxPROP_BUTTON_CANCEL | wxPROP_BUTTON_CHECK_CROSS | wxPROP_PULLDOWN
#else
#define wxPROP_BUTTON_DEFAULT wxPROP_BUTTON_CHECK_CROSS | wxPROP_PULLDOWN | wxPROP_SHOWVALUES
#endif

// Mediates between a physical panel and the property sheet
class wxPropertyListView: public wxPropertyView
{
 DECLARE_DYNAMIC_CLASS(wxPropertyListView)
 protected:
  wxListBox *propertyScrollingList;
  wxListBox *valueList;     // Should really be a combobox, but we don't have one.
  wxText *valueText;
  wxButton *confirmButton;  // A tick, as in VB
  wxButton *cancelButton;   // A cross, as in VB
  wxButton *editButton;     // Invokes the custom validator, if any

  Bool detailedEditing;     // E.g. using listbox for choices

  static wxBitmap *tickBitmap;
  static wxBitmap *crossBitmap;
  
  wxPanel *propertyWindow; // Panel that the controls will appear on
  wxWindow *managedWindow; // Frame or dialog
  
  wxButton *windowCloseButton; // Or OK
  wxButton *windowCancelButton;
  wxButton *windowHelpButton;
 public:
  static Bool dialogCancelled;

  wxPropertyListView(wxPanel *propPanel = NULL, long flags = wxPROP_BUTTON_DEFAULT);
  ~wxPropertyListView(void);

  // Associates and shows the view
  virtual void ShowView(wxPropertySheet *propertySheet, wxPanel *panel);

  // Update this view of the viewed object, called e.g. by
  // the object itself.
  virtual Bool OnUpdateView(void);

  wxString MakeNameValueString(wxString name, wxString value);

  // Update a single line in the list of properties
  virtual Bool UpdatePropertyDisplayInList(wxProperty *property);

  // Update the whole list
  virtual Bool UpdatePropertyList(Bool clearEditArea = TRUE);

  // Find the wxListBox index corresponding to this property
  virtual int FindListIndexForProperty(wxProperty *property);

  // Select and show string representation in editor the given
  // property. NULL resets to show no property.
  virtual Bool ShowProperty(wxProperty *property, Bool select = TRUE);
  virtual Bool EditProperty(wxProperty *property);

  // Update the display from the property
  virtual Bool DisplayProperty(wxProperty *property);
  // Update the property from the display
  virtual Bool RetrieveProperty(wxProperty *property);

  // Find appropriate validator and load property into value controls
  virtual Bool BeginShowingProperty(wxProperty *property);
  // Find appropriate validator and unload property from value controls
  virtual Bool EndShowingProperty(wxProperty *property);

  // Begin detailed editing (e.g. using value listbox)
  virtual void BeginDetailedEditing(void);

  // End detailed editing (e.g. using value listbox)
  virtual void EndDetailedEditing(void);

  // Called by the property listbox callback
  virtual Bool OnPropertySelect(void);

  // Called by the value listbox callback
  virtual Bool OnValueListSelect(void);

  virtual Bool CreateControls(void);
  virtual void ShowTextControl(Bool show);
  virtual void ShowListBoxControl(Bool show);
  virtual void EnableCheck(Bool show);
  virtual void EnableCross(Bool show);

  virtual void OnOk(void);
  virtual void OnCancel(void);
  virtual void OnHelp(void);
  virtual void OnDoubleClick(void);

  virtual void OnCheck(void);
  virtual void OnCross(void);
  virtual void OnEdit(void);

  inline virtual wxListBox *GetPropertyScrollingList() { return propertyScrollingList; }
  inline virtual wxListBox *GetValueList() { return valueList; }
  inline virtual wxText *GetValueText() { return valueText; }
  inline virtual wxButton *GetConfirmButton() { return confirmButton; }
  inline virtual wxButton *GetCancelButton() { return cancelButton; }
  inline virtual wxButton *GetEditButton() { return editButton; }
  inline virtual Bool GetDetailedEditing(void) { return detailedEditing; }

  inline virtual void AssociatePanel(wxPanel *win) { propertyWindow = win; }
  inline virtual wxPanel *GetPanel(void) { return propertyWindow; }

  inline virtual void SetManagedWindow(wxWindow *win) { managedWindow = win; }
  inline virtual wxWindow *GetManagedWindow(void) { return managedWindow; }

  inline virtual wxButton *GetWindowCloseButton() { return windowCloseButton; }
  inline virtual wxButton *GetWindowCancelButton() { return windowCancelButton; }
  inline virtual wxButton *GetHelpButton() { return windowHelpButton; }
  
  Bool OnClose(void);
};

class wxPropertyTextEdit: public wxText
{
 DECLARE_CLASS(wxPropertyTextEdit)
 public:
  wxPropertyListView *view;
  wxPropertyTextEdit(wxPropertyListView *v, wxPanel *parent, wxFunction func, char *label, char *value, int x = -1, int y = -1,
    int width = -1, int height = -1, long style = 0, char *name = "text");
  void OnSetFocus(void);
  void OnKillFocus(void);
};

#define wxPROP_ALLOW_TEXT_EDITING           1

/*
 * The type of validator used for property lists (Visual Basic style)
 */
 
class wxPropertyListValidator: public wxPropertyValidator
{
  DECLARE_DYNAMIC_CLASS(wxPropertyListValidator)
 protected:
 public:
   wxPropertyListValidator(long flags = wxPROP_ALLOW_TEXT_EDITING): wxPropertyValidator(flags) { }
   ~wxPropertyListValidator(void) {}

   // Called when the property is selected or deselected: typically displays the value
   // in the edit control (having chosen a suitable control to display: (non)editable text or listbox)
   virtual Bool OnSelect(Bool select, wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the property is double clicked. Extra functionality can be provided, such as
   // cycling through possible values.
   inline virtual Bool OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
   { return TRUE; }

   // Called when the value listbox is selected. Default behaviour is to copy
   // string to text control, and retrieve the value into the property.
   virtual Bool OnValueListSelect(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the property value is edited using standard text control
   virtual Bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
   {
     return TRUE;
   }

   virtual Bool OnClearControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

      // Called when the property is edited in detail
   virtual Bool OnPrepareDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
   {
     return TRUE;
   }

   // Called if focus lost, IF we're in a modeless property editing situation.
   virtual Bool OnClearDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
   {
     return TRUE;
   }

   // Called when the edit (...) button is pressed. The default implementation
   // calls view->BeginDetailedEditing; the filename validator (for example) overrides
   // this function to show the file selector.
   virtual void OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   virtual Bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
   { return TRUE; }

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transferance from the property editing area to the property itself
   virtual Bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   virtual Bool OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
};

/*
 * A default dialog box class to use.
 */
 
class wxPropertyListDialog: public wxDialogBox
{
  DECLARE_CLASS(wxPropertyListDialog)
 private:
  wxPropertyListView *view;
 public:
  wxPropertyListDialog(wxPropertyListView *v, wxWindow *parent, char *title, Bool modal = FALSE, int x = -1, int y = -1,
    int w = -1, int h = -1, long style = wxDEFAULT_DIALOG_STYLE, char *name = "dialogBox");
  Bool OnClose(void);
  Bool OnCharHook(wxKeyEvent& event);
  void OnDefaultAction(wxItem *item);
};

/*
 * A default panel class to use.
 */
 
class wxPropertyListPanel: public wxPanel
{
  DECLARE_CLASS(wxPropertyListPanel)
 private:
  wxPropertyListView *view;
 public:
  wxPropertyListPanel(wxPropertyListView *v, wxWindow *parent, int x = -1, int y = -1,
    int w = -1, int h = -1, long style = 0, char *name = "panel"):
     wxPanel(parent, x, y, w, h, style, name)
  {
    view = v;
  }
  void OnDefaultAction(wxItem *item);
};

/*
 * A default frame class to use.
 */
 
class wxPropertyListFrame: public wxFrame
{
  DECLARE_CLASS(wxPropertyListFrame)
 private:
  wxPropertyListView *view;
  wxPanel *propertyPanel;
 public:
  wxPropertyListFrame(wxPropertyListView *v, wxFrame *parent, char *title, int x = -1, int y = -1,
    int w = -1, int h = -1, long style = wxSDI | wxDEFAULT_FRAME, char *name = "frame"):
     wxFrame(parent, title, x, y, w, h, style, name)
  {
    view = v;
    propertyPanel = NULL;
  }
  Bool OnClose(void);

  // Must call this to create panel and associate view
  virtual Bool Initialize(void);
  virtual wxPanel *OnCreatePanel(wxFrame *parent, wxPropertyListView *v);
  inline virtual wxPanel *GetPropertyPanel(void) { return propertyPanel; }
};

/*
 * Some default validators
 */
 
class wxRealListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxRealListValidator)
 protected:
  float realMin;
  float realMax;
 public:
   // 0.0, 0.0 means no range
   wxRealListValidator(float min = 0.0, float max = 0.0, long flags = wxPROP_ALLOW_TEXT_EDITING):wxPropertyListValidator(flags)
   {
     realMin = min; realMax = max;
   }
   ~wxRealListValidator(void) {}

   Bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   Bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transfer from the property editing area to the property itself
   Bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
};

class wxIntegerListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxIntegerListValidator)
 protected:
  long integerMin;
  long integerMax;
 public:
   // 0, 0 means no range
   wxIntegerListValidator(long min = 0, long max = 0, long flags = wxPROP_ALLOW_TEXT_EDITING):wxPropertyListValidator(flags)
   {
     integerMin = min; integerMax = max;
   }
   ~wxIntegerListValidator(void) {}

   Bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   Bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transfer from the property editing area to the property itself
   Bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
};

class wxBoolListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxBoolListValidator)
 protected:
 public:
   wxBoolListValidator(long flags = 0):wxPropertyListValidator(flags)
   {
   }
   ~wxBoolListValidator(void) {}

   Bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   Bool OnPrepareDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   Bool OnClearDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   Bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transfer from the property editing area to the property itself
   Bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   Bool OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the property is double clicked. Extra functionality can be provided,
   // cycling through possible values.
   virtual Bool OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
};

class wxStringListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxStringListValidator)
 protected:
  wxStringList *strings;
 public:
   wxStringListValidator(wxStringList *list = NULL, long flags = 0);

   ~wxStringListValidator(void)
   {
     if (strings)
       delete strings;
   }

   Bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   Bool OnPrepareDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   Bool OnClearDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   Bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transfer from the property editing area to the property itself
   Bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   Bool OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the property is double clicked. Extra functionality can be provided,
   // cycling through possible values.
   Bool OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
};
 
class wxFilenameListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxFilenameListValidator)
 protected:
  wxString filenameWildCard;
  wxString filenameMessage;
  
 public:
   wxFilenameListValidator(wxString message = "Select a file", wxString wildcard = "*.*", long flags = 0);

   ~wxFilenameListValidator(void);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   Bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transferance from the property editing area to the property itself
   Bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   Bool OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   Bool OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   Bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the edit (...) button is pressed.
   void OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
};

class wxColourListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxColourListValidator)
 protected:
 public:
   wxColourListValidator(long flags = 0);

   ~wxColourListValidator(void);

   Bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   Bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   Bool OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   Bool OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   Bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the edit (...) button is pressed.
   void OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
};

class wxListOfStringsListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxListOfStringsListValidator)
 protected:
 public:
   wxListOfStringsListValidator(long flags = 0);

   ~wxListOfStringsListValidator(void)
   {
   }

   Bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   Bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transfer from the property editing area to the property itself
   Bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   Bool OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the property is double clicked.
   Bool OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   
   Bool EditStringList(wxWindow *parent, wxStringList *stringList, const char *title = "String List Editor");

   // Called when the edit (...) button is pressed.
   void OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
};

#endif
