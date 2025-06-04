/*
 * File:	btoolbar.h
 * Purpose:	wxWindows GUI builder: user-defined toolbars
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */


#ifndef btoolbarh
#define btoolbarh

#include "wx_tbar.h"
#include "wxbuild.h"
#include "bframe.h"
#include "bsubwin.h"
#include "bgdi.h"

class BuildAction;

class BuildTool: public wxObject
{
 public:
  Bool isToggle;
  int toolId;
  char *toolIdName;
//  char *bitmapFileName;
  char *helpString;
//  wxBitmap *bitmap;
  BuildBitmapData *bitmapData;
  BuildTool(void);
  ~BuildTool(void);
};

// Data for making a toolbar
class BuildToolbarData: public BuildCanvasData
{
 public:
  Bool isMutuallyExclusive;
  Bool isFloating;
  int orientation;
  int  rowsOrCols;
  BuildFrameData *floatingFrame;
  
  wxList tools;
  wxListBox *toolListBox;
  wxText *toolIdItem;
  wxText *toolHelpItem;
  wxText *toolBitmapItem;
  wxCheckBox *toolToggleItem;
  
  BuildToolbarData(BuildFrameData *frame, Bool floating);
  ~BuildToolbarData(void);

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

  Bool LoadBitmapFromFile(BuildTool *tool);
  // Tool to select is optional.
  void DisplayTools(wxListBox *listbox, int sel = -1);

  void GenerateToolbarIdsCPP(ostream& stream);

  // Find action associated with this toolbar command id, if any.
  BuildAction *FindToolAction(int id);

  void GenerateResourceEntries(ostream &stream);
  void GenerateResourceLoading(ostream &stream);

  void GenerateWXResourceBitmaps(ostream& stream);
  void GenerateWXResourceBitmapRegistration(ostream& stream);
  void GenerateBitmapDataIncludes(ostream& stream);
};

/*
 * Real Toolbar
 *
 */

class UserToolbar: public wxToolBar
{
 public:
  BuildToolbarData *buildWindow; // Pointer to further user information
  UserToolbar(wxFrame *parent, int x, int y, int w, int h, long style, int orient,
              int rowsOrCols):
   wxToolBar(parent, x, y, w, h, style, orient, rowsOrCols)
  {
    buildWindow = NULL;
  }
  void OnPaint(void);
  void OnSize(int w, int h);

  // Only allow toggle if returns TRUE
  Bool OnLeftClick(int toolIndex, Bool toggleDown);

  // Called when the mouse cursor enters a tool bitmap (no button pressed).
  // Argument is -1 if mouse is exiting the toolbar.
  void OnMouseEnter(int toolIndex);
};

#endif // btoolbarh


