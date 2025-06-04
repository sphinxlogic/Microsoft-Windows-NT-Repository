/*
 * File:	bsubwin.h
 * Purpose:	wxWindows GUI builder: subwindows
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */


#ifndef bsubwinh
#define bsubwinh

#include "wxbuild.h"
#include "bwin.h"
#include "bgdi.h"

#define RESIZE_FIXED 1
#define RESIZE_GROW  2
#define RESIZE_PROPORTIONAL 3

// Data for making a subwindow
class BuildSubwindowData: public BuildWindowData
{
 public:
  int resizeMode;
  char *resizeModeString;
  char *labelFont;
  char *buttonFont;

  int percentOfFrame; // Percentage of width or height the panel takes up
  Bool hasBorder;
 
  BuildSubwindowData(BuildFrameData *theParent);
  ~BuildSubwindowData(void);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  void FindScreenPosition(int *sx, int *sy);
  void FindClientPosition(int sx, int sy, int *cx, int *cy);

  // Add class-specific items to form
  void AddFormItems(wxForm *form);

  // Add class-specific items to dialog, since forms can't
  // cope with everything.
  void AddDialogItems(wxDialogBox *dialog);

  // Generation
  void WriteClassImplementation(ostream& stream);
  void WriteClassDeclaration(ostream& stream);
  void GenerateWindowStyleString(char *buf);
  void GenerateResourceEntries(ostream& stream);
};

// Data for making a panel
class BuildPanelData: public BuildSubwindowData
{
 public:
  Bool fitContents;
  Bool relativeLayout;
  Bool horizLabelPosition;

  BuildPanelData(BuildFrameData *theParent);
  ~BuildPanelData(void);

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

  // Write .wxr resource information for this window
  void GenerateWXResourceData(ostream& stream);
  // Generates code to load wxWindows resources from static data,
  // in OnInit.
  void GenerateResourceLoading(ostream& stream);
};

// Data for making a text window
class BuildTextWindowData: public BuildSubwindowData
{
 public:
  char *defaultFile;
  
  BuildTextWindowData(BuildFrameData *theParent);
  ~BuildTextWindowData(void);

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
};

// Data for making a canvas
class BuildCanvasData: public BuildSubwindowData
{
 public:
/*
  char *simulationBitmapName;
  wxBitmap *simulationBitmap;
  wxColourMap *simulationColourMap;
*/
  BuildBitmapData *bitmapData;
  Bool isRetained;
  int unitSizeX;
  int unitSizeY;
  int noUnitsX;
  int noUnitsY;
  int unitsPerPageX;
  int unitsPerPageY;

  BuildCanvasData(BuildFrameData *theParent);
  ~BuildCanvasData(void);

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

  // Load bitmap into canvas
  Bool LoadCanvasBitmap(void);
  // Reset colourmap
  Bool ClearCanvasBitmap(void);

  void GenerateResourceEntries(ostream &stream);
  void GenerateResourceLoading(ostream &stream);
  void GenerateBitmapDataIncludes(ostream& stream);

  void GenerateWXResourceBitmaps(ostream& stream);
  void GenerateWXResourceBitmapRegistration(ostream& stream);
};

// Data for making a dialog box
class BuildDialogBoxData: public BuildPanelData
{
 public:
  Bool modal;
  BuildDialogBoxData(BuildFrameData *theParent);
  ~BuildDialogBoxData(void);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  void FindScreenPosition(int *sx, int *sy);
  void FindClientPosition(int sx, int sy, int *cx, int *cy);

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
};


/*
 * Actual panel
 *
 */
 
class UserPanel: public wxPanel
{
 public:
  BuildPanelData *buildWindow; // Pointer to further user information
  UserPanel(wxFrame *parent, int x, int y, int w, int h, long style):
   wxPanel(parent, x, y, w, h, style)
  {
    buildWindow = NULL;
  }
  void OnSize(int w, int h);
  void OnLeftClick(int x, int y, int keys);
  void OnRightClick(int x, int y, int keys);
  void OnItemLeftClick(wxItem *item, int x, int y, int keys);
  void OnItemRightClick(wxItem *item, int x, int y, int keys);
  void OnItemSelect(wxItem *item);
  void OnItemMove(wxItem *item, int x, int y);
  void OnItemSize(wxItem *item, int w, int h);
};

/*
 * Actual dialog box
 *
 */
 
class UserDialogBox: public wxDialogBox
{
 public:
  BuildDialogBoxData *buildWindow; // Pointer to further user information
  UserDialogBox(wxFrame *parent, char *title, Bool modal, int x, int y, int w, int h, long style):
   wxDialogBox(parent, title, modal, x, y, w, h, style)
  {
    buildWindow = NULL;
  }
  ~UserDialogBox(void);
  
  void OnSize(int w, int h);

  void OnLeftClick(int x, int y, int keys);
  void OnRightClick(int x, int y, int keys);
  void OnItemLeftClick(wxItem *item, int x, int y, int keys);
  void OnItemRightClick(wxItem *item, int x, int y, int keys);
  void OnItemSelect(wxItem *item);
  void OnItemMove(wxItem *item, int x, int y);
  void OnItemSize(wxItem *item, int w, int h);
};

// Actual text window
class UserTextWindow: public wxTextWindow
{
 public:
  BuildTextWindowData *buildWindow; // Pointer to further user information
  UserTextWindow(wxFrame *parent, int x, int y, int w, int h, long style):
   wxTextWindow(parent, x, y, w, h, style)
  {
    buildWindow = NULL;
  }
  void OnSize(int w, int h);
};

// Actual canvas
class UserCanvas: public wxCanvas
{
 public:
  BuildCanvasData *buildWindow; // Pointer to further user information
  UserCanvas(wxFrame *parent, int x, int y, int w, int h, long style):
   wxCanvas(parent, x, y, w, h, style)
  {
    buildWindow = NULL;
  }
  void OnSize(int w, int h);
  void OnChar(wxKeyEvent& key);
  void OnEvent(wxMouseEvent& mouse);
  void OnPaint(void);
};


#endif // bsubwinh

