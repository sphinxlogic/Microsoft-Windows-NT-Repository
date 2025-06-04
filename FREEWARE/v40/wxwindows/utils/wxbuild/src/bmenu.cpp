/*
 * File:	bmenu.cc
 * Purpose:	wxWindows GUI builder -- menu editor.
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wx_help.h"
#include "wx_form.h"
#include <ctype.h>
#include <stdlib.h>

#include "wxbuild.h"
#include "bapp.h"
#include "namegen.h"
#include "bframe.h"
#include "bmenu.h"
#include "bactions.h"

extern wxHelpInstance *HelpInstance;

wxStringList MenuStringList;

MenuBarEditor::MenuBarEditor(wxFrame *frame, char *title, Bool modal, int x, int y, int w, int h):
  wxDialogBox(frame, title, modal, x, y, w, h)
{
  SetLabelFont(SmallLabelFont);
  SetButtonFont(SmallButtonFont);
  buildMenuBar = NULL;
  SetLabelPosition(wxVERTICAL);
  (void)new wxButton(this, (wxFunction)MenuBarEditorOk, "Ok");
  (void)new wxButton(this, (wxFunction)MenuBarEditorCancel, "Cancel");
  (void)new wxButton(this, (wxFunction)MenuBarEditorHelp, "Help");
  NewLine();
  menus = new wxListBox(this, (wxFunction)MenuListProc, "Menus", wxSINGLE, -1, -1, 450, 200);
  NewLine();
  inputField = new wxText(this, (wxFunction)NULL, "Name", "", -1, -1, 200);
  idField = new wxText(this, (wxFunction)NULL, "Id", "", -1, -1, 200);
  NewLine();
  helpField = new wxText(this, (wxFunction)NULL, "Help string", "", -1, -1, 400);
  NewLine();
  (void)new wxButton(this, (wxFunction)NewItemProc, "New item", -1, -1, 110);
  (void)new wxButton(this, (wxFunction)NewSubmenuProc, "New child", -1, -1, 110);
  (void)new wxButton(this, (wxFunction)NewSeparatorProc, "New separator", -1, -1, 110);
  (void)new wxButton(this, (wxFunction)DeleteItemProc, "Delete item", -1, -1, 110);
  NewLine();
  (void)new wxButton(this, (wxFunction)SaveMenuProc, "Save item", -1, -1, 110);
  Fit();
  Centre(wxBOTH);
}

MenuBarEditor::~MenuBarEditor(void)
{
}

void MenuBarEditor::ShowMenuBar(BuildMenuItem *theBuildMenuBar)
{
  buildMenuBar = theBuildMenuBar;
  DisplayMenus();
  Show(TRUE);
}

void MenuBarEditor::DisplaySelectedMenu(void)
{
  BuildMenuItem *select = GetSelectedMenu();
  if (select)
  {
    // Display menu string and id
    if (select->menuString)
      inputField->SetValue(select->menuString);
    else
      inputField->SetValue("");
    if (select->menuIdName)
      idField->SetValue(select->menuIdName);
    else
      idField->SetValue("");
    if (select->helpString)
      helpField->SetValue(select->helpString);
    else
      helpField->SetValue("");
  }
  else
  {
    inputField->SetValue("");
    idField->SetValue("");
    helpField->SetValue("");
  }
}

void MenuBarEditor::DisplayMenus1(wxList& menuList, int level)
{
  char buffer[500];
  wxNode *node = menuList.First();
  while (node)
  {
    buffer[0] = 0;
    BuildMenuItem *item = (BuildMenuItem *)node->Data();
    for (int i = 0; i < level; i++)
      strcat(buffer, "    ");
    switch (item->menuType)
    {
      case BUILD_MENU_TYPE_MENU:
      case BUILD_MENU_TYPE_ITEM:
      {
        if (item->menuString)
        {
          strcat(buffer, item->menuString);
          menus->Append(buffer, (char *)item);
        }
        break;
      }
      case BUILD_MENU_TYPE_SEPARATOR:
      {
        strcat(buffer, "--------------");
        menus->Append(buffer, (char *)item);
        break;
      }
      default:
        break;
    }
    DisplayMenus1(item->menus, level+1);
    node = node->Next();
  }
}

void MenuBarEditor::DisplayMenus(BuildMenuItem *select)
{
  menus->Clear();
  DisplayMenus1(buildMenuBar->menus, 0);
  if (select)
  {
    for (int i = 0; i < 200; i++)
      if (menus->GetClientData(i) == (char *)select)
      {
        menus->SetSelection(i);
        break;
      }
  }
}

BuildMenuItem *MenuBarEditor::GetSelectedMenu(void)
{
  int sel = menus->GetSelection();
  if (sel > -1)
    return (BuildMenuItem *)menus->GetClientData(sel);
  else
    return NULL;
}

void MenuListProc(wxListBox& list, wxCommandEvent& event)
{
  MenuBarEditor *editor = (MenuBarEditor *)list.GetParent();
  editor->DisplaySelectedMenu();
}

void MenuBarEditor::NewItem(int menuAction)
{
  char *name = NULL;
  char *menuId = NULL;
  char *helpString = NULL;

  BuildMenuItem *selected = GetSelectedMenu();
  if (selected && (selected->menuType == BUILD_MENU_TYPE_SEPARATOR) &&
      (menuAction == BUILD_MENU_NEW_CHILD))
    return;

  name = copystring(inputField->GetValue());
  menuId = copystring(idField->GetValue());
  helpString = copystring(helpField->GetValue());

  int menuType;
  if (menuAction == BUILD_MENU_NEW_SEPARATOR)
  {
    if (!selected ||
        (selected && (selected->parent->menuType == BUILD_MENU_TYPE_MENU_BAR)))
      return;
    else menuType = BUILD_MENU_TYPE_SEPARATOR;
  }
  else if (menuAction == BUILD_MENU_NEW_CHILD)
  {
    menuType = BUILD_MENU_TYPE_ITEM;
  }
  else if (menuAction == BUILD_MENU_NEW_ITEM)
  {
    if (!selected ||
        (selected && (selected->parent->menuType == BUILD_MENU_TYPE_MENU_BAR)))
      menuType = BUILD_MENU_TYPE_MENU;
    else
      menuType = BUILD_MENU_TYPE_ITEM;
  }

  BuildMenuItem *item = new BuildMenuItem(menuType, name);
  item->helpString = helpString;
        
  switch (menuType)
  {
    case BUILD_MENU_TYPE_ITEM:
    case BUILD_MENU_TYPE_MENU:
    {
      if (!name || (strlen(name) == 0))
      {
        wxMessageBox("Please enter a name for the menu.", "Message");
        return;
      }
      if (!menuId || (strlen(menuId) == 0))
      {
        item->menuIdName = copystring(item->MakeIdName());
      }
      else
      {
        if (menuId)
          item->menuIdName = copystring(menuId);
        else
          item->menuIdName = NULL;
      }
      break;
    }
    case BUILD_MENU_TYPE_SEPARATOR:
    default:
      break;
  }

  if (name)
    delete[] name;
  if (menuId)
    delete[] menuId; 

  wxNode *insertBefore = NULL;
  if (selected)
  {
    wxList *list = NULL;

    if (selected->parent)
      list = &(selected->parent->menus);
    else list = &(buildMenuBar->menus);

    wxNode *node = list->First();
    while (node)
    {
      BuildMenuItem *it = (BuildMenuItem *)node->Data();
      if (it && (it == selected))
      {
        insertBefore = node->Next();
        node = NULL;
      }
      else
        node = node->Next();
    }

    // Want to insert in front of any selected item
    if (menuAction == BUILD_MENU_NEW_CHILD)
    {
      selected->menus.Append(item);
      item->parent = selected;
      selected->menuType = BUILD_MENU_TYPE_MENU;
    }
    else
    {
      if (insertBefore)
        list->Insert(insertBefore, item);
      else
        list->Append(item);
      item->parent = selected->parent;
    }
  }
  else
  {
    buildMenuBar->menus.Append(item);
    item->parent = buildMenuBar;
  }

  DisplayMenus(item);
  inputField->SetValue("");
  idField->SetValue("");
  helpField->SetValue("");
}

void NewSubmenuProc(wxButton& but, wxCommandEvent& event)
{
  MenuBarEditor *editor = (MenuBarEditor *)but.GetParent();
  editor->NewItem(BUILD_MENU_NEW_CHILD);
}

void NewItemProc(wxButton& but, wxCommandEvent& event)
{
  MenuBarEditor *editor = (MenuBarEditor *)but.GetParent();
  editor->NewItem(BUILD_MENU_NEW_ITEM);
}

void NewSeparatorProc(wxButton& but, wxCommandEvent& event)
{
  MenuBarEditor *editor = (MenuBarEditor *)but.GetParent();
  editor->NewItem(BUILD_MENU_NEW_SEPARATOR);
}

void SaveMenuProc(wxButton& but, wxCommandEvent& event)
{
  MenuBarEditor *editor = (MenuBarEditor *)but.GetParent();
  BuildMenuItem *item = editor->GetSelectedMenu();
  if (item)
  {
    char *s = editor->inputField->GetValue();
    if (s && (strlen(s) > 0))
    {
      if (item->menuString)
        delete[] item->menuString;
      item->menuString = copystring(s);
    }
    s = editor->idField->GetValue();
    if (s && (strlen(s) > 0))
    {
      if (item->menuIdName)
        delete[] item->menuIdName;
      item->menuIdName = copystring(s);
    }
    s = editor->helpField->GetValue();
    if (s && (strlen(s) > 0))
    {
      if (item->helpString)
        delete[] item->helpString;
      item->helpString = copystring(s);
    }
    editor->DisplayMenus(item);
  }
}

void DeleteItemProc(wxButton& but, wxCommandEvent& event)
{
  MenuBarEditor *editor = (MenuBarEditor *)but.GetParent();
  BuildMenuItem *item = editor->GetSelectedMenu();
  if (item)
  {
    if (item->parent)
    {
      item->parent->menus.DeleteObject(item);
      // If child count down to zero, we're no longer a menu,
      // but merely an item.
      if ((item->parent->menus.Number() == 0) &&
          (item->parent->menuType != BUILD_MENU_TYPE_MENU_BAR))
        item->parent->menuType = BUILD_MENU_TYPE_ITEM;
    }
    else
      editor->buildMenuBar->menus.DeleteObject(item);

    delete item;
    editor->DisplayMenus();
    editor->DisplaySelectedMenu();
  }
}

void MenuBarEditorOk(wxButton& but, wxCommandEvent& event)
{
  MenuBarEditor *parent = (MenuBarEditor *)but.GetParent();
  int menuId = 1;
  parent->buildMenuBar->GenerateIds(&menuId);
  parent->Show(FALSE);
}

void MenuBarEditorCancel(wxButton& but, wxCommandEvent& event)
{
  MenuBarEditor *parent = (MenuBarEditor *)but.GetParent();
  parent->Show(FALSE);
}

void MenuBarEditorHelp(wxButton& but, wxCommandEvent& event)
{
  wxBeginBusyCursor();
  HelpInstance->LoadFile();
  HelpInstance->KeywordSearch("Creating a menubar");
  wxEndBusyCursor();
}

static BuildFrameData *currentFrame = NULL;
void ShowMenuBarEditor(BuildFrameData *frameData, BuildMenuItem *theBuildMenuBar)
{
  currentFrame = frameData;
  MenuBarEditor *editor = new MenuBarEditor(NULL, "Menu Bar Editor", TRUE, 10, 10, 500, 500);
  editor->ShowMenuBar(theBuildMenuBar);
  delete editor;
  currentFrame = NULL;
}

BuildMenuItem::BuildMenuItem(int typ, char *s):BuildWindowData(NULL)
{
  menuType = typ;
  menuId = -1;
  if (s) menuString = copystring(s);
  else menuString = NULL;
  menuIdName = NULL;
  parent = NULL;
  windowType = wxTYPE_MENU_BAR;
}

BuildMenuItem::~BuildMenuItem(void)
{
  if (menuString) delete[] menuString;
  if (menuIdName) delete[] menuIdName;

  wxNode *node = menus.First();
  while (node)
  {
    BuildMenuItem *item = (BuildMenuItem *)node->Data();
    delete item;
    node = node->Next();
  }
}

Bool BuildMenuItem::EditAttributes(void)
{
  return TRUE;
}

Bool BuildMenuItem::WriteMenu(PrologDatabase *database)
{
  PrologExpr *expr = new PrologExpr("menu_item");
  WritePrologAttributes(expr, database);
  database->Append(expr);
  
  wxNode *node = menus.First();
  while (node)
  {
    BuildMenuItem *child = (BuildMenuItem *)node->Data();
    child->WriteMenu(database);
    node = node->Next();
  }
  return TRUE;
}

void BuildMenuItem::GenerateMenuCPP(ostream &stream)
{
  switch (menuType)
  {
    case BUILD_MENU_TYPE_ITEM:
    {
      break;
    }
    case BUILD_MENU_TYPE_SEPARATOR:
    {
      break;
    }
    case BUILD_MENU_TYPE_MENU:
    {
      if (!name)
        name = copystring(nameSpace.MakeVariable("menu"));

      stream << "  wxMenu *" << name << " = new wxMenu;\n";
      break;
    }
    case BUILD_MENU_TYPE_MENU_BAR:
    {
      if (!name)
        name = copystring(GetNewObjectName("menuBar"));
      if (buildApp.useResourceMethod)
      {
        stream << "  wxMenuBar *" << name << " = wxResourceCreateMenuBar(\"" << name << "\");\n";
        return;
      }
      else
      {
        stream << "  wxMenuBar *" << name << " = new wxMenuBar;\n";
      }
      break;
    }
    default:
    {
      break;
    }
  }
  wxNode *node = menus.First();
  while (node)
  {
    BuildMenuItem *item = (BuildMenuItem *)node->Data();
    item->GenerateMenuCPP(stream);
    node = node->Next();
  }
  switch (menuType)
  {
    case BUILD_MENU_TYPE_ITEM:
    {
      stream << "  " << parent->name << "->Append(" << menuIdName << ", ";
      stream << "\"" << menuString << "\"";
      if (helpString && (strlen(helpString) > 0))
        stream << ", " << "\"" << helpString << "\"" ;
      stream << ");\n";

      break;
    }
    case BUILD_MENU_TYPE_SEPARATOR:
    {
      stream << "  " << parent->name << "->AppendSeparator();\n";
      break;
    }
    case BUILD_MENU_TYPE_MENU:
    {
      if (parent->menuType == BUILD_MENU_TYPE_MENU)
      {
        stream << "  " << parent->name << "->Append(" << menuIdName << ", ";
        stream << "\"" << menuString << "\"";
        stream << ", " << name;
        if (helpString && (strlen(helpString) > 0))
          stream << ", " << "\"" << helpString << "\"" ;
        stream << ");\n";
      }
      else  // Menu bar is parent
      {
        stream << "  " << parent->name << "->Append(" << name << ", ";
        stream << "\"" << menuString << "\");\n";
      }
      break;
    }
    case BUILD_MENU_TYPE_MENU_BAR:
    {
      break;
    }
    default:
    {
    }
  }
}

void BuildMenuItem::GenerateDefines(ostream &stream)
{
  if (menuId == -1)
    menuId = (int)NewId();
  if (menuIdName)
  {
    if (MenuStringList.Member(menuIdName))
    {
      Report("Warning: menu item identifier ");
      Report(menuIdName);
      Report(" has been used more than once.\n");
    }
    else
     MenuStringList.Add(menuIdName);
  }

  if (menuIdName && (menuType != BUILD_MENU_TYPE_MENU_BAR))
  {
    stream << "#define " << menuIdName << " " << menuId << "\n";
  }
  wxNode *node = menus.First();
  while (node)
  {
    BuildMenuItem *item = (BuildMenuItem *)node->Data();
    item->GenerateDefines(stream);
    node = node->Next();
  }
}

void BuildMenuItem::GenerateIds(int *currentId)
{
  if (menuIdName && (menuType != BUILD_MENU_TYPE_SEPARATOR))
  {
    menuId = *currentId;
    *currentId = *currentId + 1;
  }
  wxNode *node = menus.First();
  while (node)
  {
    BuildMenuItem *item = (BuildMenuItem *)node->Data();
    item->GenerateIds(currentId);
    node = node->Next();
  }
}

void BuildMenuItem::GenerateSwitchStatementsCPP(BuildFrameData *frame, ostream &stream)
{
  if (menuIdName && parent && (parent->menuType != BUILD_MENU_TYPE_MENU_BAR))
  {
    if (menuString)
    {
      char buf[150];
      wxStripMenuCodes(menuString, buf);
      stream << "    // " << buf << "\n";
    }
    stream << "    case " << menuIdName << ":\n";
    stream << "    {\n";
    BuildAction *action = frame->FindMenuAction(menuId);
    if (action)
    {
      action->GenerateActionCPP(stream, "      ", this);
    }
    stream << "      break;\n    }\n";
  }
  wxNode *node = menus.First();
  while (node)
  {
    BuildMenuItem *item = (BuildMenuItem *)node->Data();
    item->GenerateSwitchStatementsCPP(frame, stream);
    node = node->Next();
  }
}

// Write .wxr resource information for this window
void BuildMenuItem::GenerateWXResourceDataMenu(ostream& stream, int indent)
{
  int i;
  switch (menuType)
  {
    case BUILD_MENU_TYPE_MENU:
    case BUILD_MENU_TYPE_MENU_BAR:
    {
      if (menuId == -1)
        menuId = (int)NewId();
      stream << "\\\n";
      for (i = 0; i < indent; i++)
        stream << " ";
      stream << "[";
      if (menuType != BUILD_MENU_TYPE_MENU_BAR)
      {
        stream << "'" << menuString << "', " << menuId << ", " << "'" << helpString << "', ";
      }
      wxNode *node1 = menus.First();
      while (node1)
      {
        BuildMenuItem *item1 = (BuildMenuItem *)node1->Data();
        item1->GenerateWXResourceDataMenu(stream, indent+2);
        if (node1->Next())
          stream << ",";
/*
        if (node1->Next())
          stream << ",\n\\";
        else
          stream << "\n\\";
*/
        node1 = node1->Next();
      }
      stream << "\\\n";
      for (int i = 0; i < indent; i++)
        stream << " ";
      stream << "]";
      break;
    }
    case BUILD_MENU_TYPE_ITEM:
    {
      if (menuId == -1)
        menuId = (int)NewId();
      stream << "\\\n";
      for (i = 0; i < indent; i++)
        stream << " ";
      stream << "['" << menuString << "', " << menuId << ", " << "'" << helpString << "']";
      break;
    }
    case BUILD_MENU_TYPE_SEPARATOR:
    {
      stream << "\\\n";
      for (i = 0; i < indent; i++)
        stream << " ";
      stream << "[]";
      break;
    }
    default:
      break;
  }
}

// Write .wxr resource information for this window
void BuildMenuItem::GenerateWXResourceData(ostream& stream)
{
  stream << "static char *" << name << "_resource = \"menu(name = '" << name << "',\\\n";
  stream << "  menu = ";
  GenerateWXResourceDataMenu(stream, 2);
  stream << ").\";\n\n";
}

// Write .wxr resource loading code
void BuildMenuItem::GenerateResourceLoading(ostream& stream)
{
  if (name)
    stream << "  wxResourceParseData(" << name << "_resource);\n";
/*
  wxNode *node = menus.First();
  while (node)
  {
    BuildMenuItem *child = (BuildMenuItem *)node->Data();
    child->GenerateResourceLoading(stream);
    node = node->Next();
  }
*/
}

Bool BuildMenuItem::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  expr->AddAttributeValue("id", id);
  expr->AddAttributeValue("menu_id", (long)menuId);
  if (parent)
    expr->AddAttributeValue("parent", parent->id);
  expr->AddAttributeValue("type", (long)menuType);
  if (menuString)
    expr->AddAttributeValueString("menu_string", menuString);
  if (helpString)
    expr->AddAttributeValueString("help_string", helpString);
  if (menuIdName)
    expr->AddAttributeValueString("menu_id_name", menuIdName);
  if (name)
    expr->AddAttributeValueString("name", name);

  return TRUE;
}

Bool BuildMenuItem::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  expr->AssignAttributeValue("type", &menuType);
  expr->AssignAttributeValue("menu_string", &menuString);
  expr->AssignAttributeValue("menu_id_name", &menuIdName);
  expr->AssignAttributeValue("menu_id", &menuId);
  expr->AssignAttributeValue("help_string", &helpString);
  expr->AssignAttributeValue("name", &name);
  return TRUE;
}

// Make the wxWindows object
Bool BuildMenuItem::MakeRealWindow(void)
{
  if (menus.Number() == 0)
  {
    userWindow = NULL;
    return FALSE;
  }
  wxMenuBar *menuBar = new wxMenuBar;
  userWindow = menuBar;
  wxNode *node = menus.First();
  while (node)
  {
    BuildMenuItem *item = (BuildMenuItem *)node->Data();
    wxMenu *menu = new wxMenu;
    wxNode *node1 = item->menus.First();
    while (node1)
    {
      BuildMenuItem *item1 = (BuildMenuItem *)node1->Data();
      item1->MakeRealMenuItem(menu);
      node1 = node1->Next();
    }
    menuBar->Append(menu, item->menuString);
    node = node->Next();
  }
  return TRUE;
}

Bool BuildMenuItem::MakeRealMenuItem(wxMenu *parentMenu)
{
  switch (menuType)
  {
    case BUILD_MENU_TYPE_MENU:
    {
      wxMenu *theMenu = new wxMenu;
      userWindow = theMenu;
      wxNode *node1 = menus.First();
      while (node1)
      {
        BuildMenuItem *item1 = (BuildMenuItem *)node1->Data();
        item1->MakeRealMenuItem(theMenu);
        node1 = node1->Next();
      }
      if (menuId == -1)
        menuId = (int)NewId();
      parentMenu->Append(menuId, menuString, theMenu, helpString);
      break;
    }
    case BUILD_MENU_TYPE_ITEM:
    {
      if (menuId == -1)
        menuId = (int)NewId();
      parentMenu->Append(menuId, menuString, helpString);
      break;
    }
    case BUILD_MENU_TYPE_SEPARATOR:
    {
      parentMenu->AppendSeparator();
      break;
    }
    default:
      break;
  }
  return TRUE;
}

// Destroy real wxWindows object
Bool BuildMenuItem::DestroyRealWindow(void)
{
  // Not necessary: always deleted with frame.
  return TRUE;
}

/*
 * Invent a suitable name for the menu item identifier.
 *
 */

char *BuildMenuItem::MakeIdName(void)
{
  static char buffer[200];
  char nameBuffer[100];
  int len = strlen(menuString);
  // First, make a plausible name out of the menu string.
  int j = 0;
  for (int i = 0; i < len; i++)
  {
    // Stop if we're starting some keystroke description
    if (menuString[i] == '\\' || menuString[i] == '\t')
      break;
    else if (menuString[i] == ' ')
    {
      nameBuffer[j] = '_';
      j ++;
    }
    else if (!isalpha(menuString[i]))
    {
    }
    else
    {
      nameBuffer[j] = toupper(menuString[i]);
      j ++;
    }
  }
  nameBuffer[j] = 0;
    
  if (parent && parent->menuIdName)
  {
    strcpy(buffer, parent->menuIdName);
    strcat(buffer, "_");
    strcat(buffer, nameBuffer);
  }
  else if (currentFrame)
  {
    char *nm = NULL;
    if (currentFrame->className)
      nm = currentFrame->className;
    else
      nm = currentFrame->name;

    strcpy(buffer, nm);
    for (int i = 0; i < (int)strlen(nm); i++)
      buffer[i] = (char)toupper(buffer[i]);
    strcat(buffer, "_");
    strcat(buffer, nameBuffer);
  }
  else
    strcpy(buffer, nameBuffer);
  return buffer;
}
