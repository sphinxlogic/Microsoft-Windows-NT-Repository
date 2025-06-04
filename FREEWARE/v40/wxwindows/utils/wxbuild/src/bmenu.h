/*
 * File:	bmenu.h
 * Purpose:	wxWindows GUI builder - menu editor
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#include "read.h"

#ifndef menuh
#define menuh

#define BUILD_MENU_TYPE_ITEM 0
#define BUILD_MENU_TYPE_MENU 1
#define BUILD_MENU_TYPE_SEPARATOR 2
#define BUILD_MENU_TYPE_MENU_BAR 3

#define BUILD_MENU_NEW_ITEM 0
#define BUILD_MENU_NEW_SEPARATOR 1
#define BUILD_MENU_NEW_CHILD 2

class BuildFrameData;

class BuildMenuItem: public BuildWindowData
{
 public:
  int menuType;
  int menuId;
  char *menuIdName;
  char *menuString;
  wxList menus; // Other menus (if any)
  BuildMenuItem *parent;
  
  BuildMenuItem(int typ = BUILD_MENU_TYPE_MENU, char *s = NULL);
  ~BuildMenuItem(void);
  Bool EditAttributes(void);

  Bool WriteMenu(PrologDatabase *database);
  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  // Make the wxWindows object
  Bool MakeRealWindow(void);
  Bool MakeRealMenuItem(wxMenu *parentMenu);
  char *MakeIdName(void);

  // Destroy real wxWindows object
  Bool DestroyRealWindow(void);

  void GenerateIds(int *id);

  void GenerateMenuCPP(ostream &stream);
  void GenerateSwitchStatementsCPP(BuildFrameData *frame, ostream &stream);

  // Write .wxr resource information for this window
  void GenerateWXResourceData(ostream& stream);
  void GenerateWXResourceDataMenu(ostream& stream, int indent);
  void GenerateDefines(ostream& stream);
  void GenerateResourceLoading(ostream& stream);
};

class MenuBarEditor: public wxDialogBox
{
 public:
  wxListBox *menus;
  wxText *inputField;
  wxText *idField;
  wxText *helpField;
  BuildMenuItem *buildMenuBar;

  MenuBarEditor(wxFrame *frame, char *title, Bool modal, int x, int y, int w, int h);
  ~MenuBarEditor(void);

  // Display menu bar and loop
  void ShowMenuBar(BuildMenuItem *menuBar);

  void DisplaySelectedMenu(void);

  void DisplayMenus(BuildMenuItem *select = NULL);
  void DisplayMenus1(wxList& menuList, int level);

  BuildMenuItem *GetSelectedMenu(void);

  void NewItem(int menuType);
};

void MenuListProc(wxListBox& list, wxCommandEvent& event);

void NewSubmenuProc(wxButton& but, wxCommandEvent& event);
void NewItemProc(wxButton& but, wxCommandEvent& event);
void NewSeparatorProc(wxButton& but, wxCommandEvent& event);
void SaveMenuProc(wxButton& but, wxCommandEvent& event);
void DeleteItemProc(wxButton& but, wxCommandEvent& event);

void MenuBarEditorOk(wxButton& but, wxCommandEvent& event);
void MenuBarEditorCancel(wxButton& but, wxCommandEvent& event);
void MenuBarEditorHelp(wxButton& but, wxCommandEvent& event);

// Function to invoke dialog box
void ShowMenuBarEditor(BuildFrameData *frameData, BuildMenuItem *buildMenuBar);

#endif // menuh

