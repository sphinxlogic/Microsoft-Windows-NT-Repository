/*
 * File:	bframe.h
 * Purpose:	wxWindows GUI builder: frames
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */


#ifndef bframeh
#define bframeh

#include "wxbuild.h"
#include "bwin.h"
#include "bgdi.h"

class BuildMenuItem;
class BuildToolbarData;

// Data for making a frame
class BuildFrameData: public BuildWindowData
{
 public:
  BuildMenuItem *buildMenuBar; // Menu bar
  long menuBarId;
  int noStatusLineFields;
  int tilingMode;
//  char *iconName;
  BuildBitmapData *iconData;
  
  BuildToolbarData *toolbar;

  Bool thickFrame;
  Bool minBox;
  Bool maxBox;
  Bool hasCaption;
  Bool systemMenu;
  
  BuildFrameData(BuildFrameData *theParent);
  ~BuildFrameData(void);

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

  // Recalculate subwindow sizes if percentages don't
  // add up.
  void RecomputeSubwindowSizes(void);

  // Find action associated with this menu command id, if any.
  BuildAction *FindMenuAction(int id);

  // Do the tiling
  void DoTiling(void);

  void WriteClassImplementation(ostream& stream);
  void WriteClassDeclaration(ostream& stream);
  void GenerateWindowStyleString(char *buf);
  void GenerateConstructorCall(ostream& stream, char *parentName);

  // Generate bitmap, icon etc. resource entries for RC file.
  void GenerateResourceEntries(ostream &stream);

  // Write .wxr resource information for this window
  void GenerateWXResourceData(ostream& stream);
  // Generates code to load wxWindows resources from static data,
  // in OnInit.
  void GenerateWXResourceBitmaps(ostream& stream);
  void GenerateWXResourceBitmapRegistration(ostream& stream);
  void GenerateBitmapDataIncludes(ostream& stream);
  void GenerateResourceLoading(ostream& stream);
  void GenerateDefines(ostream& stream);
};

/*
 * USER OBJECTS
 * We must define a new class for every GUI object of interest.
 * These will be manipulated via the editor, etc.
 * So they must have as much behaviour as we allow the user
 * to muck about with.
 *
 */

class UserFrame: public wxFrame
{
 public:
  BuildFrameData *buildWindow; // Pointer to further user information
  UserFrame(wxFrame *parent, char *title, int x, int y, int w, int h, long style):
   wxFrame(parent, title, x, y, w, h, style)
  {
    buildWindow = NULL;
  }
  ~UserFrame(void);
  
  Bool OnClose(void);
  void OnSize(int w, int h);
  void OnMenuCommand(int command);
  void OnMenuSelect(int command);
};

#endif // bframeh

