/*
 * File:	bitem.h
 * Purpose:	wxWindows GUI builder: subwindows
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */


#ifndef bitemh
#define bitemh

#include "wxbuild.h"
#include "bwin.h"
#include "bgdi.h"

#define wxDEFAULT_POSITION 4

// Data for making a subwindow
class BuildItemData: public BuildWindowData
{
 public:
  int labelPosition;
  char *labelPositionString;
  Bool autoSize;
  
  BuildItemData(BuildPanelData *theParent);
  ~BuildItemData(void);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  void FindScreenPosition(int *sx, int *sy);
  void FindClientPosition(int sx, int sy, int *cx, int *cy);

  // Add class-specific items to form
  void AddFormItems(wxForm *form);

  Bool EditAttributes(void);

  // Add class-specific items to dialog, since forms can't
  // cope with everything.
  void AddDialogItems(wxDialogBox *dialog);

  // Generation
  void WriteClassImplementation(ostream& stream);
  void WriteClassDeclaration(ostream& stream);
  void GenerateWindowStyleString(char *buf);
  void GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation);
  void GenerateWindowInitialisationPost(ostream& stream, char *instanceName, char *indentation);

  // Write .wxr resource information for this window
  void GenerateWXResourceData(ostream& stream);
};

// Data for making a button
class BuildButtonData: public BuildItemData
{
 public:
//  char *bitmapFile;
//  wxBitmap *buttonBitmap;
  BuildBitmapData *bitmapData;
  
  BuildButtonData(BuildPanelData *theParent);
  ~BuildButtonData(void);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  // Make the wxWindows object
  Bool MakeRealWindow(void);

  // Destroy
  Bool DestroyRealWindow(void);

  Bool EditAttributes(void);

  // Add class-specific items to form
  void AddFormItems(wxForm *form);

  // Add class-specific items to dialog, since forms can't
  // cope with everything.
  void AddDialogItems(wxDialogBox *dialog);

  void WriteClassImplementation(ostream& stream);
  void WriteClassDeclaration(ostream& stream);
  void GenerateConstructorCall(ostream& stream, char *parentName);

  void GenerateResourceEntries(ostream &stream);
  void GenerateResourceLoading(ostream &stream);
  void GenerateBitmapDataIncludes(ostream &stream);

  void GenerateWXResourceBitmaps(ostream& stream);
  void GenerateWXResourceBitmapRegistration(ostream& stream);
  void GenerateWindowStyleString(char *buf);
};

// Checkbox
class BuildCheckBoxData: public BuildItemData
{
 public:
  BuildCheckBoxData(BuildPanelData *theParent);
  ~BuildCheckBoxData(void);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  // Make the wxWindows object
  Bool MakeRealWindow(void);

  // Destroy
  Bool DestroyRealWindow(void);

  Bool EditAttributes(void);

  // Add class-specific items to form
  void AddFormItems(wxForm *form);

  // Add class-specific items to dialog, since forms can't
  // cope with everything.
  void AddDialogItems(wxDialogBox *dialog);

  void WriteClassImplementation(ostream& stream);
  void WriteClassDeclaration(ostream& stream);
  void GenerateConstructorCall(ostream& stream, char *parentName);
  void GenerateWindowStyleString(char *buf);
};

// Data for making a message
class BuildMessageData: public BuildItemData
{
 public:
  BuildBitmapData *bitmapData;
  BuildMessageData(BuildPanelData *theParent);
  ~BuildMessageData(void);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  // Make the wxWindows object
  Bool MakeRealWindow(void);

  // Destroy
  Bool DestroyRealWindow(void);

  Bool EditAttributes(void);

  // Add class-specific items to form
  void AddFormItems(wxForm *form);

  // Add class-specific items to dialog, since forms can't
  // cope with everything.
  void AddDialogItems(wxDialogBox *dialog);

  void WriteClassImplementation(ostream& stream);
  void WriteClassDeclaration(ostream& stream);
  void GenerateConstructorCall(ostream& stream, char *parentName);

  void GenerateResourceEntries(ostream &stream);
  void GenerateResourceLoading(ostream &stream);
  void GenerateBitmapDataIncludes(ostream &stream);

  void GenerateWXResourceBitmaps(ostream& stream);
  void GenerateWXResourceBitmapRegistration(ostream& stream);
  void GenerateWindowStyleString(char *buf);
};

// Data for making a text item
class BuildTextData: public BuildItemData
{
 public:
  BuildTextData(BuildPanelData *theParent);
  ~BuildTextData(void);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  // Make the wxWindows object
  Bool MakeRealWindow(void);

  // Destroy
  Bool DestroyRealWindow(void);

  Bool EditAttributes(void);

  // Add class-specific items to form
  void AddFormItems(wxForm *form);

  // Add class-specific items to dialog, since forms can't
  // cope with everything.
  void AddDialogItems(wxDialogBox *dialog);

  void WriteClassImplementation(ostream& stream);
  void WriteClassDeclaration(ostream& stream);
  void GenerateConstructorCall(ostream& stream, char *parentName);
  void GenerateWindowStyleString(char *buf);
};

// Data for making a multitext item
class BuildMultiTextData: public BuildTextData
{
 public:
  BuildMultiTextData(BuildPanelData *theParent);
  ~BuildMultiTextData(void);

  void AddFormItems(wxForm *form);
  // Make the wxWindows object
  Bool MakeRealWindow(void);
  void GenerateConstructorCall(ostream& stream, char *parentName);
  void GenerateWindowStyleString(char *buf);
};

// Data for making a listbox
class BuildListBoxData: public BuildItemData
{
 public:
  Bool multipleSel;
  Bool scrollAlways;
  wxStringList strings;
  wxListBox *currentStringsBox;
  wxText *currentTextBox;
  char *tmpStringArray; // Name of string array used before/during initialisation

  BuildListBoxData(BuildPanelData *theParent);
  ~BuildListBoxData(void);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  // Make the wxWindows object
  Bool MakeRealWindow(void);

  // Destroy
  Bool DestroyRealWindow(void);

  Bool EditAttributes(void);

  // Add class-specific items to form
  void AddFormItems(wxForm *form);

  // Add class-specific items to dialog, since forms can't
  // cope with everything.
  void AddDialogItems(wxDialogBox *dialog);

  void WriteClassImplementation(ostream& stream);
  void WriteClassDeclaration(ostream& stream);
  void GenerateConstructorCall(ostream& stream, char *parentName);
  void GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation);
  void GenerateWindowStyleString(char *buf);
};

// Data for making a choice item
class BuildChoiceData: public BuildItemData
{
 public:
  wxStringList strings;
  wxListBox *currentStringsBox;
  wxText *currentTextBox;
  char *tmpStringArray; // Name of string array used before/during initialisation

  BuildChoiceData(BuildPanelData *theParent);
  ~BuildChoiceData(void);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  // Make the wxWindows object
  Bool MakeRealWindow(void);

  // Destroy
  Bool DestroyRealWindow(void);

  Bool EditAttributes(void);

  // Add class-specific items to form
  void AddFormItems(wxForm *form);

  // Add class-specific items to dialog, since forms can't
  // cope with everything.
  void AddDialogItems(wxDialogBox *dialog);

  void WriteClassImplementation(ostream& stream);
  void WriteClassDeclaration(ostream& stream);
  void GenerateConstructorCall(ostream& stream, char *parentName);
  void GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation);

  Bool CanSizeHeight(void) { return FALSE; }
  void GenerateWindowStyleString(char *buf);
};

// Data for making a radiobox item
class BuildRadioBoxData: public BuildItemData
{
 public:
  int noRowsCols;
  wxStringList strings;
  wxListBox *currentStringsBox;
  wxText *currentTextBox;
  char *tmpStringArray; // Name of string array used before/during initialisation

  BuildRadioBoxData(BuildPanelData *theParent);
  ~BuildRadioBoxData(void);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  // Make the wxWindows object
  Bool MakeRealWindow(void);

  // Destroy
  Bool DestroyRealWindow(void);

  Bool EditAttributes(void);

  // Add class-specific items to form
  void AddFormItems(wxForm *form);

  // Add class-specific items to dialog, since forms can't
  // cope with everything.
  void AddDialogItems(wxDialogBox *dialog);

  void WriteClassImplementation(ostream& stream);
  void WriteClassDeclaration(ostream& stream);
  void GenerateConstructorCall(ostream& stream, char *parentName);
  void GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation);

  Bool CanSizeWidth(void) { return FALSE; }
  Bool CanSizeHeight(void) { return FALSE; }
  void GenerateWindowStyleString(char *buf);
};

// Data for making a slider item
class BuildSliderData: public BuildItemData
{
 public:
  int minValue;
  int maxValue;
  int sliderValue;
  BuildSliderData(BuildPanelData *theParent);
  ~BuildSliderData(void);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  // Make the wxWindows object
  Bool MakeRealWindow(void);

  // Destroy
  Bool DestroyRealWindow(void);

  Bool EditAttributes(void);

  // Add class-specific items to form
  void AddFormItems(wxForm *form);

  // Add class-specific items to dialog, since forms can't
  // cope with everything.
  void AddDialogItems(wxDialogBox *dialog);

  void WriteClassImplementation(ostream& stream);
  void WriteClassDeclaration(ostream& stream);
  void GenerateConstructorCall(ostream& stream, char *parentName);
  void GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation);

  Bool CanSizeWidth(void) { return TRUE; }
  Bool CanSizeHeight(void) { return FALSE; }
  void GenerateWindowStyleString(char *buf);
};

// Data for making a gauge item
class BuildGaugeData: public BuildItemData
{
 public:
  int maxValue;
  int gaugeValue;
  char *orientation;
  BuildGaugeData(BuildPanelData *theParent);
  ~BuildGaugeData(void);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  // Make the wxWindows object
  Bool MakeRealWindow(void);

  // Destroy
  Bool DestroyRealWindow(void);

  Bool EditAttributes(void);

  // Add class-specific items to form
  void AddFormItems(wxForm *form);

  // Add class-specific items to dialog, since forms can't
  // cope with everything.
  void AddDialogItems(wxDialogBox *dialog);

  void WriteClassImplementation(ostream& stream);
  void WriteClassDeclaration(ostream& stream);
  void GenerateConstructorCall(ostream& stream, char *parentName);
  void GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation);

  Bool CanSizeWidth(void) { return TRUE; }
  Bool CanSizeHeight(void) { return TRUE; }
  void GenerateWindowStyleString(char *buf);
};

// Data for making a groubox item
class BuildGroupBoxData: public BuildItemData
{
 public:
  BuildGroupBoxData(BuildPanelData *theParent);
  ~BuildGroupBoxData(void);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  // Make the wxWindows object
  Bool MakeRealWindow(void);

  // Destroy
  Bool DestroyRealWindow(void);

  Bool EditAttributes(void);

  // Add class-specific items to form
  void AddFormItems(wxForm *form);

  // Add class-specific items to dialog, since forms can't
  // cope with everything.
  void AddDialogItems(wxDialogBox *dialog);

  void WriteClassImplementation(ostream& stream);
  void WriteClassDeclaration(ostream& stream);
  void GenerateConstructorCall(ostream& stream, char *parentName);
  void GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation);

  Bool CanSizeWidth(void) { return TRUE; }
  Bool CanSizeHeight(void) { return TRUE; }
  void GenerateWindowStyleString(char *buf);
};

/*
 * Actual button
 *
 */

class UserButton: public wxButton
{
 public:
  BuildButtonData *buildWindow; // Pointer to further user information
  UserButton(wxPanel *parent, wxFunction func, char *label, int x, int y, int w, int h, long style):
   wxButton(parent, func, label, x, y, w, h, style)
  {
    buildWindow = NULL;
  }
  UserButton(wxPanel *parent, wxFunction func, wxBitmap *bitmap, int x, int y, int w, int h, long style):
   wxButton(parent, func, bitmap, x, y, w, h, style)
  {
    buildWindow = NULL;
  }
};


void UserButtonCallback(UserButton& but, wxCommandEvent& event);
 
/*
 * Actual checkbox
 *
 */
 
class UserCheckBox: public wxCheckBox
{
 public:
  BuildCheckBoxData *buildWindow; // Pointer to further user information
  UserCheckBox(wxPanel *parent, wxFunction func, char *label, int x, int y, int w, int h, long style):
   wxCheckBox(parent, func, label, x, y, w, h, style)
  {
    buildWindow = NULL;
  }
};

/*
 * Actual message
 *
 */
 
class UserMessage: public wxMessage
{
 public:
  BuildMessageData *buildWindow; // Pointer to further user information
  UserMessage(wxPanel *parent, char *label, int x, int y, long style):
   wxMessage(parent, label, x, y, style)
  {
    buildWindow = NULL;
  }
  UserMessage(wxPanel *parent, wxBitmap *bitmap, int x, int y, long style):
   wxMessage(parent, bitmap, x, y, style)
  {
    buildWindow = NULL;
  }
};

// Actual text widget
class UserText: public wxText
{
 public:
  BuildTextData *buildWindow; // Pointer to further user information
  UserText(wxPanel *parent, wxFunction func, char *label, char *value,
           int x, int y, int w, int h, long style):
   wxText(parent, func, label, value, x, y, w, h, style)
  {
    buildWindow = NULL;
  }
};

// Actual multitext widget
class UserMultiText: public wxMultiText
{
 public:
  BuildMultiTextData *buildWindow; // Pointer to further user information
  UserMultiText(wxPanel *parent, wxFunction func, char *label, char *value,
           int x, int y, int w, int h, long style):
   wxMultiText(parent, func, label, value, x, y, w, h, style)
  {
    buildWindow = NULL;
  }
};

// Actual listbox
class UserListBox: public wxListBox
{
 public:
  BuildListBoxData *buildWindow; // Pointer to further user information
  UserListBox(wxPanel *parent, wxFunction func, char *label, int mode,
              int x, int y, int w, int h, int n, char *strings[], long style):
   wxListBox(parent, func, label, mode, x, y, w, h, n, strings, style)
  {
    buildWindow = NULL;
  }
};

// Actual choice
class UserChoice: public wxChoice
{
 public:
  BuildChoiceData *buildWindow; // Pointer to further user information
  UserChoice(wxPanel *parent, wxFunction func, char *label,
              int x, int y, int w, int h, int n, char *strings[], long style):
   wxChoice(parent, func, label, x, y, w, h, n, strings, style)
  {
    buildWindow = NULL;
  }
};

// Actual radiobox
class UserRadioBox: public wxRadioBox
{
 public:
  BuildRadioBoxData *buildWindow; // Pointer to further user information
  UserRadioBox(wxPanel *parent, wxFunction func, char *label,
              int x, int y, int w, int h, int n, char *strings[], int dim, long style):
   wxRadioBox(parent, func, label, x, y, w, h, n, strings, dim, style)
  {
    buildWindow = NULL;
  }
};

// Actual slider
class UserSlider: public wxSlider
{
 public:
  BuildSliderData *buildWindow; // Pointer to further user information
  UserSlider(wxPanel *parent, wxFunction func, char *label,
              int value, int min_value, int max_value, int width,
              int x, int y, long style):
   wxSlider(parent, func, label, value, min_value, max_value, width, x, y, style)
  {
    buildWindow = NULL;
  }
};

// Actual gauge
class UserGauge: public wxGauge
{
 public:
  BuildGaugeData *buildWindow; // Pointer to further user information
  UserGauge(wxPanel *parent, char *label,
              int range, int x, int y, int width, int height, long style):
   wxGauge(parent, label, range, x, y, width, height, style)
  {
    buildWindow = NULL;
  }
};

// Actual gauge
class UserGroupBox: public wxGroupBox
{
 public:
  BuildGroupBoxData *buildWindow; // Pointer to further user information
  UserGroupBox(wxPanel *parent, char *label,
              int x, int y, int width, int height, long style):
   wxGroupBox(parent, label, x, y, width, height, style)
  {
    buildWindow = NULL;
  }
};


#endif // bitemh


