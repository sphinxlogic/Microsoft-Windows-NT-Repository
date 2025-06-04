/*
 * File:	btoolbar.cc
 * Purpose:	wxWindows GUI builder: toolbars
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

#include <ctype.h>
#include <stdlib.h>

#include "wxbuild.h"
#include "bapp.h"
#include "namegen.h"
#include "bframe.h"
#include "bsubwin.h"
#include "btoolbar.h"
#include "bactions.h"
#ifdef wx_msw
#include "dib.h"
#endif

#define wxEVENT_TYPE_TOOL_COMMAND (EVENT_TYPES_FIRST + 200)

BuildToolbarData::BuildToolbarData(BuildFrameData *theParent, Bool floating):
  BuildCanvasData(theParent)
{
  x = 0;
  y = 0;
  width = 400;
  height = 60;
  name = copystring(GetNewObjectName("toolbar"));
//  memberName = copystring(name);
  className = copystring(GetNewObjectName("ToolBarClass"));
  title = copystring("untitled");
  windowStyle = 0;
  windowType = wxTYPE_TOOLBAR;
  resizeMode = RESIZE_FIXED;
  resizeModeString = copystring("Fixed");

  orientation = wxHORIZONTAL;
  rowsOrCols = 40;
  toolListBox = NULL;
  isMutuallyExclusive = TRUE;

  // Remove from parent
  if (theParent)
    theParent->children.DeleteObject(this);
}

BuildToolbarData::~BuildToolbarData(void)
{
  wxNode *node = tools.First();
  while (node)
  {
    BuildTool *tool = (BuildTool *)node->Data();
    delete tool;
    node = node->Next();
  }
  if (buildParent)
    ((BuildFrameData *)buildParent)->toolbar = NULL;
}

Bool BuildToolbarData::EditAttributes(void)
{
  char nameBuf[200];
  sprintf(nameBuf, "Toolbar Properties for %s", name);
  wxDialogBox *dialog = new wxDialogBox(NULL, nameBuf, TRUE, 10, 10);
  dialog->SetLabelFont(SmallLabelFont);
  dialog->SetButtonFont(SmallButtonFont);

  BuildForm *form = new BuildForm("Creating a tool bar");
  AddFormItems(form);
  
  form->AssociatePanel(dialog);
  form->dialog = dialog;
  AddDialogItems(dialog);

  dialog->Fit();
  dialog->Centre(wxBOTH);

  form->RevertValues();

  dialog->Show(TRUE);
  return TRUE;
}

Bool BuildToolbarData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildCanvasData::ReadPrologAttributes(expr, database);

  expr->AssignAttributeValue("floating", &isFloating);
  expr->AssignAttributeValue("rows_or_cols", &rowsOrCols);
  expr->AssignAttributeValue("orientation", &orientation);
  expr->AssignAttributeValue("mutually_exclusive", &isMutuallyExclusive);

  // Read all tools
  int id = 1;
  char buf[50];
  Bool keepGoing = TRUE;
  while (keepGoing)
  {
    sprintf(buf, "tool%d", id);
    PrologExpr *listExpr = NULL;
    expr->AssignAttributeValue(buf, &listExpr);
    if (!listExpr)
    {
      keepGoing = FALSE;
    }
    else
    {
      BuildTool *tool = new BuildTool;

      if (listExpr->Nth(0)->Type() != PrologList)
      {
        // Backward compatibility
        PrologExpr *idExpr = listExpr->Nth(0);
        PrologExpr *nameExpr = listExpr->Nth(1);
        PrologExpr *bitmapExpr = listExpr->Nth(2);
        PrologExpr *helpExpr = listExpr->Nth(3);
        PrologExpr *toggleExpr = listExpr->Nth(4);

        tool->toolId = (int)idExpr->IntegerValue();
        tool->toolIdName = copystring(nameExpr->StringValue());
        tool->bitmapData->SetBitmapName(bitmapExpr->StringValue());
        tool->helpString = copystring(helpExpr->StringValue());
        tool->isToggle = (Bool)toggleExpr->IntegerValue();
      }
      else
      {
        // New format: a bitmap list [name windowsType xType] followed
        // by tool-specific values: integer id, id name, help string, toggle 0/1
        PrologExpr *bitmapListExpr = listExpr->Nth(0);
        PrologExpr *idExpr = listExpr->Nth(1);
        PrologExpr *idNameExpr = listExpr->Nth(2);
        PrologExpr *helpExpr = listExpr->Nth(3);
        PrologExpr *toggleExpr = listExpr->Nth(4);

        tool->bitmapData->ReadList(bitmapListExpr);

        tool->toolId = (int)idExpr->IntegerValue();
        tool->toolIdName = copystring(idNameExpr->StringValue());
        tool->helpString = copystring(helpExpr->StringValue());
        tool->isToggle = (Bool)toggleExpr->IntegerValue();
      }
      tools.Append(tool);

      id ++;
    }
  }

  return TRUE;
}

Bool BuildToolbarData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildCanvasData::WritePrologAttributes(expr, database);

  expr->AddAttributeValue("floating", (long)isFloating);
  expr->AddAttributeValue("rows_or_cols", (long)rowsOrCols);
  expr->AddAttributeValue("orientation", (long)orientation);
  expr->AddAttributeValue("mutually_exclusive", (long)isMutuallyExclusive);

  // Add all tools
  int id = 1;
  char buf[50];
  wxNode *node = tools.First();
  while (node)
  {
    BuildTool *tool = (BuildTool *)node->Data();
    sprintf(buf, "tool%d", id);
    PrologExpr *listExpr = new PrologExpr(PrologList);
    listExpr->Append(tool->bitmapData->WriteList());
    listExpr->Append(new PrologExpr((long)tool->toolId));
    listExpr->Append(new PrologExpr(PrologString, tool->toolIdName));
    listExpr->Append(new PrologExpr(PrologString, tool->helpString));
    listExpr->Append(new PrologExpr((long)tool->isToggle));

    expr->AddAttributeValue(buf, listExpr);
    id ++;
    node = node->Next();
  }

  return TRUE;
}

Bool BuildToolbarData::MakeRealWindow(void)
{
  if (!(buildParent && buildParent->userWindow)) return FALSE;

  windowStyle = 0;
  if (hasBorder)
    windowStyle |= wxBORDER;
  if (isRetained)
    windowStyle |= wxRETAINED;

  UserToolbar *toolbar = new UserToolbar((wxFrame *)buildParent->userWindow, x, y,
                 width, height, windowStyle, orientation, rowsOrCols);
  toolbar->SetMargins(2, 2);
  toolbar->GetDC()->SetBackground(wxGREY_BRUSH);
  userWindow = toolbar;
  toolbar->buildWindow = this;

  int currentX = 2;
  wxNode *node = tools.First();
  while (node)
  {
    BuildTool *tool = (BuildTool *)node->Data();
    if (tool->toolId == -1)
      tool->toolId = (int)NewId();
      
    if (tool->bitmapData->CreateBitmap())
    {
      toolbar->AddTool(tool->toolId, tool->bitmapData->GetBitmap(),
                       NULL, tool->isToggle, (float)currentX, -1, NULL);
      currentX += tool->bitmapData->GetBitmap()->GetWidth() + 2;
    }
    node = node->Next();
  }

/*
  if ((unitSizeX > 0) && (unitSizeY > 0))
    canvas->SetScrollbars(unitSizeX, unitSizeY, noUnitsX, noUnitsY,
                          unitsPerPageX, unitsPerPageY);
*/
  return TRUE;
}

Bool BuildToolbarData::DestroyRealWindow(void)
{
  BuildFrameData *frame = (BuildFrameData *)buildParent;
  if (userWindow)
  {
    userWindow->GetPosition(&x, &y);
    userWindow->GetSize(&width, &height);
    delete userWindow;
    userWindow = NULL;
  }
  return TRUE;
}

// Add class-specific items to form
void BuildToolbarData::AddFormItems(wxForm *form)
{
  form->Add(wxMakeFormString("Name", &name, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Description", &description, wxFORM_MULTITEXT, NULL, NULL, wxVERTICAL,
               300, 80));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Class name", &className, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               150));
  form->Add(wxMakeFormShort("Rows/cols", &rowsOrCols, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL, 150));
}

static void ToolBarListProc(wxListBox& lbox, wxCommandEvent& event)
{
  BuildToolbarData *toolbar = (BuildToolbarData *)lbox.wxWindow::GetClientData();
  int sel = lbox.GetSelection();
  if (sel > -1)
  {
    BuildTool *tool = (BuildTool *)lbox.GetClientData(sel);
    toolbar->toolIdItem->SetValue(tool->toolIdName ? tool->toolIdName : "");
    toolbar->toolHelpItem->SetValue(tool->helpString ? tool->helpString : "");
    toolbar->toolBitmapItem->SetValue(tool->bitmapData->GetBitmapName() ? tool->bitmapData->GetBitmapName() : "");
    toolbar->toolToggleItem->SetValue(tool->isToggle);
  }
}

static void AddToolProc(wxButton& but, wxCommandEvent& event)
{
  BuildToolbarData *toolbar = (BuildToolbarData *)but.GetClientData();
  BuildTool *tool = new BuildTool;
  tool->toolIdName = copystring(toolbar->toolIdItem->GetValue());
  tool->bitmapData->SetBitmapName(toolbar->toolBitmapItem->GetValue());
  tool->helpString = copystring(toolbar->toolHelpItem->GetValue());
  tool->isToggle = toolbar->toolToggleItem->GetValue();
  toolbar->tools.Append(tool);
  toolbar->DisplayTools(toolbar->toolListBox, (int)(toolbar->tools.Number() - 1));
}

static void EditToolProc(wxButton& but, wxCommandEvent& event)
{
  BuildToolbarData *toolbar = (BuildToolbarData *)but.GetClientData();
  int sel = toolbar->toolListBox->GetSelection();
  if (sel > -1)
  {
    BuildTool *tool = (BuildTool *)toolbar->toolListBox->GetClientData(sel);
    tool->bitmapData->Edit(but.GetParent());
    toolbar->toolBitmapItem->SetValue(tool->bitmapData->GetBitmapName());
  }
}

static void DemoteToolProc(wxButton& but, wxCommandEvent& event)
{
  BuildToolbarData *toolbar = (BuildToolbarData *)but.GetClientData();
  int sel = toolbar->toolListBox->GetSelection();
  if (sel > -1)
  {
    BuildTool *tool = (BuildTool *)toolbar->toolListBox->GetClientData(sel);
    toolbar->tools.DeleteObject(tool);
    toolbar->tools.Append(tool);
    toolbar->DisplayTools(toolbar->toolListBox);
  }
}

static void DeleteToolProc(wxButton& but, wxCommandEvent& event)
{
  BuildToolbarData *toolbar = (BuildToolbarData *)but.GetClientData();
  int sel = toolbar->toolListBox->GetSelection();
  if (sel > -1)
  {
    BuildTool *tool = (BuildTool *)toolbar->toolListBox->GetClientData(sel);
    toolbar->tools.DeleteObject(tool);
    delete tool;
    toolbar->DisplayTools(toolbar->toolListBox);
  }
}

static void SaveToolProc(wxButton& but, wxCommandEvent& event)
{
  BuildToolbarData *toolbar = (BuildToolbarData *)but.GetClientData();
  int sel = toolbar->toolListBox->GetSelection();
  if (sel > -1)
  {
    BuildTool *tool = (BuildTool *)toolbar->toolListBox->GetClientData(sel);
    char *idName = copystring(toolbar->toolIdItem->GetValue());
    char *toolHelp = copystring(toolbar->toolHelpItem->GetValue());
    char *toolBitmap = copystring(toolbar->toolBitmapItem->GetValue());
    Bool toolToggle = toolbar->toolToggleItem->GetValue();
    if (tool->toolIdName) delete[] tool->toolIdName;
    if (tool->helpString) delete[] tool->helpString;

    tool->toolIdName = idName;
    tool->helpString = toolHelp;
    tool->bitmapData->SetBitmapName(toolBitmap);
    delete[] toolBitmap;
    tool->isToggle = toolToggle;

    toolbar->DisplayTools(toolbar->toolListBox, sel);
  }
}

void BuildToolbarData::AddDialogItems(wxDialogBox *dialog)
{
  dialog->NewLine();
  dialog->SetLabelPosition(wxVERTICAL);
  toolListBox = new wxListBox(dialog, (wxFunction)ToolBarListProc, "Tools",
                              wxSINGLE, -1, -1, 300, 80);
  toolListBox->wxWindow::SetClientData((char *)this);
  dialog->NewLine();

//  dialog->SetLabelPosition(wxHORIZONTAL);
  toolIdItem = new wxText(dialog, (wxFunction)NULL, "Id", "", -1, -1, 150);
  toolHelpItem = new wxText(dialog, (wxFunction)NULL, "Help string", "", -1, -1, 150);
  dialog->NewLine();
  toolBitmapItem = new wxText(dialog, (wxFunction)NULL, "Bitmap", "", -1, -1, 150);
  toolToggleItem = new wxCheckBox(dialog, (wxFunction)NULL, "Toggle");
  dialog->NewLine();
  wxButton *addButton = new wxButton(dialog, (wxFunction)AddToolProc, "Add");
  wxButton *saveButton = new wxButton(dialog, (wxFunction)SaveToolProc, "Save");
  wxButton *deleteButton = new wxButton(dialog, (wxFunction)DeleteToolProc, "Delete");
  wxButton *demoteButton = new wxButton(dialog, (wxFunction)DemoteToolProc, "Demote");
//  dialog->NewLine();
  wxButton *editButton = new wxButton(dialog, (wxFunction)EditToolProc, "Edit bitmap properties");
  addButton->SetClientData((char *)this);
  deleteButton->SetClientData((char *)this);
  saveButton->SetClientData((char *)this);
  demoteButton->SetClientData((char *)this);
  editButton->SetClientData((char *)this);

  DisplayTools(toolListBox);
}

void BuildToolbarData::DisplayTools(wxListBox *listbox, int sel)
{
  // Display the tool identifiers
  listbox->Clear();
  wxNode *node = tools.First();
  while (node)
  {
    BuildTool *tool = (BuildTool *)node->Data();
    if (tool->toolIdName)
      listbox->Append(tool->toolIdName, (char *)tool);
    node = node->Next();
  }
  toolIdItem->SetValue("");
  toolBitmapItem->SetValue("");
  toolHelpItem->SetValue("");
  toolToggleItem->SetValue(FALSE);
  if (sel > -1)
    listbox->SetSelection(sel);
}

// Find action associated with this toolbar command id, if any.
BuildAction *BuildToolbarData::FindToolAction(int toolId)
{
  BuildAction *action = NULL;

  // Try to find an action corresponding to this command ID
  wxNode *node = buildActions.First();
  while (node)
  {
    BuildAction *act = (BuildAction *)node->Data();
    if ((act->eventType == wxEVENT_TYPE_TOOL_COMMAND) &&
        (act->commandId == toolId))
    {
      action = act;
      node = NULL;
    }
    else node = node->Next();
  }
  return action;
}

void BuildToolbarData::WriteClassDeclaration(ostream& stream)
{
  if (description && (strlen(description) > 0))
  {
    stream << "/*\n" << description << "\n*/\n";
  }

  stream << "class " << className << ": public wxToolBar\n{\n";
  stream << " private:\n protected:\n public:\n";
//  stream << "  wxBitmap *bitmaps[" << tools.Number() << "]\n";

  stream << "  // Constructor and destructor\n";
  stream << "  " << className ;
  stream << "(wxFrame *parent, int x, int y, int width, int height, long style, int orient,\n";
  stream << "      int rowsOrCols);\n";
  stream << "  ~" << className << "(void);\n\n";
//  stream << " void OnSize(int w, int h);\n";
  stream << " void OnPaint(void);\n";
  stream << " Bool OnLeftClick(int toolIndex, Bool toggleDown);\n";
  stream << " void OnMouseEnter(int toolIndex);\n";
  stream << "};\n\n";
}


// Generation
void BuildToolbarData::WriteClassImplementation(ostream& stream)
{
  if (description && (strlen(description) > 0))
  {
    stream << "/*\n" << description << "\n*/\n";
  }

  stream << className << "::" << className ;
  stream << "(wxFrame *parent, int x, int y, int width, int height, long style, int orient,\n";
  stream << "     int rowsOrCols):\n";
  stream << "    wxToolBar(parent, x, y, width, height, style, orient, rowsOrCols)\n{\n";

  stream << "  static wxBitmap *bitmaps[" << tools.Number() << "];\n\n";

  // Load the bitmaps
  wxNode *node = tools.First();
  int i = 0;
  while (node)
  {
    BuildTool *tool = (BuildTool *)node->Data();

    if (buildApp.useResourceMethod)
    {
      stream << "  bitmaps[" << i << "] = wxResourceCreateBitmap(\"" << tool->bitmapData->GetBitmapName() << "_resource\");\n";
    }
    else
    {
      char buf1[200];
      char buf2[200];
      strcpy(buf1, tool->bitmapData->GetBitmapName());
      StripExtension(buf1);
      strcpy(buf2, buf1);

      strcat(buf2, ".xbm");

      stream << "#ifdef wx_x\n";
      stream << "#include \"" << buf2 << "\"\n";
      stream << "  bitmaps[" << i << "] = new wxBitmap(" << buf1 << ", " << buf1 << "_width, ";
      stream << buf1 << "_height);\n";
      stream << "#endif\n";
      stream << "#ifdef wx_msw\n";
      stream << "  bitmaps[" << i << "] = new wxBitmap(\"" << buf1 << "\");\n";
      stream << "#endif\n";
    }
    node = node->Next();
    i ++;
  }
  
  stream << "  SetMargins(2, 2);\n";
  stream << "  GetDC()->SetBackground(wxGREY_BRUSH);\n";
  stream << "  int toolX = 2;\n";
    
  // Add the tools
  node = tools.First();
  i = 0;
  while (node)
  {
    BuildTool *tool = (BuildTool *)node->Data();

    stream << "  AddTool(" << tool->toolIdName << ", bitmaps[" << i << "], NULL, ";
    if (tool->isToggle)
      stream << "TRUE, ";
    else
      stream << "FALSE, ";

    stream << "(float)toolX, -1, NULL);\n";
    stream << "  toolX += bitmaps[" << i << "]->GetWidth() + 2;\n";
    node = node->Next();
    i ++;
  }

  stream << "}\n\n";

  stream << className << "::~" << className << "(void)\n{\n}\n\n";

/*
  stream << "// Called when canvas is resized.\n";
  stream << "void " << className << "::OnSize(int w, int h)\n{\n";
  stream << "  wxCanvas::OnSize(w, h);\n";
  stream << "}\n\n";
*/

  stream << "// Called when toolbar needs to be repainted.\n";
  stream << "void " << className << "::OnPaint(void)\n{\n";
  stream << "  // Speeds up drawing under Windows.\n";
  stream << "  GetDC()->BeginDrawing();\n\n";
  stream << "  // Insert your drawing code here.\n";
  stream << "  wxToolBar::OnPaint();\n";
  stream << "\n";
  stream << "  GetDC()->EndDrawing();\n";
  stream << "}\n\n";

  stream << "// Called when the toolbar receives a click event.\n";
  stream << "Bool " << className << "::OnLeftClick(int toolIndex, Bool toggleDown)\n{\n";
  stream << "  switch (toolIndex)\n";
  stream << "  {\n";
  node = tools.First();
  while (node)
  {
    BuildTool *tool = (BuildTool *)node->Data();
    
    BuildAction *action = FindToolAction(tool->toolId);
    if (action)
    {
      if (tool->helpString)
        stream << "    // " << tool->helpString << "\n";
      stream << "    case " << tool->toolIdName << ":\n";
      stream << "    {\n";

      action->GenerateActionCPP(stream, "      ", this);
      stream << "      break;\n    }\n";
    }
    node = node->Next();
  }
  stream << "    default:\n";
  stream << "      break;\n";
  stream << "  }\n";
  
  stream << "  return TRUE;\n";
  stream << "}\n\n";

  stream << "// Called when the mouse enters or exits a tool.\n";
  stream << "void " << className << "::OnMouseEnter(int toolIndex)\n{\n";
  stream << "}\n\n";
}

void BuildToolbarData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << className << "(" << parentName;
  stream << ", " << x << ", " << y << ", ";
  stream << width << ", " << height << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", ";
  if (orientation == wxHORIZONTAL) stream << "wxHORIZONTAL, ";
  else stream << "wxVERTICAL, ";
  stream << rowsOrCols << ");\n";
}

Bool BuildToolbarData::LoadBitmapFromFile(BuildTool *tool)
{
  if (tool->bitmapData && tool->bitmapData->GetBitmapName())
  {
    return (tool->bitmapData->CreateBitmap() != NULL);
  }
  return FALSE;
}

void BuildToolbarData::GenerateToolbarIdsCPP(ostream &stream)
{
  wxStringList stringList;
  wxNode *node = tools.First();
  while (node)
  {
    BuildTool *tool = (BuildTool *)node->Data();
    if (tool->toolIdName && strlen(tool->toolIdName) > 0)
    {
      if (tool->helpString && strlen(tool->helpString))
        stream << "// " << tool->helpString << "\n";
      stream << "#define " << tool->toolIdName << " " << buildApp.currentMenuItemId << "\n";
      buildApp.currentMenuItemId ++;
      if (stringList.Member(tool->toolIdName))
      {
        Report("Warning: toolbar identifier ");
        Report(tool->toolIdName);
        Report(" has been used more than once.\n");
      }
      else
        stringList.Add(tool->toolIdName);
    }
    else
    {
      Report("Tool in toolbar "); Report(name); Report("has no name.\n");
    }
    node = node->Next();
  }
}

// Generate bitmap, icon etc. resource entries for RC file.
void BuildToolbarData::GenerateResourceEntries(ostream &stream)
{
  wxNode *node = tools.First();
  while (node)
  {
    BuildTool *tool = (BuildTool *)node->Data();
    if (tool->bitmapData)
      tool->bitmapData->GenerateResourceEntry(stream);
    node = node->Next();
  }
}

void BuildToolbarData::GenerateWXResourceBitmaps(ostream& stream)
{
  wxNode *node = tools.First();
  while (node)
  {
    BuildTool *tool = (BuildTool *)node->Data();
    if (tool->bitmapData)
      tool->bitmapData->GenerateWXResourceBitmap(stream);
    node = node->Next();
  }
}

void BuildToolbarData::GenerateWXResourceBitmapRegistration(ostream& stream)
{
  wxNode *node = tools.First();
  while (node)
  {
    BuildTool *tool = (BuildTool *)node->Data();
    if (tool->bitmapData)
      tool->bitmapData->GenerateWXResourceBitmapRegistration(stream);
    node = node->Next();
  }
}

void BuildToolbarData::GenerateResourceLoading(ostream &stream)
{
  wxNode *node = tools.First();
  while (node)
  {
    BuildTool *tool = (BuildTool *)node->Data();
    if (tool->bitmapData)
      tool->bitmapData->GenerateResourceLoading(stream);
    node = node->Next();
  }
}

void BuildToolbarData::GenerateBitmapDataIncludes(ostream& stream)
{
  wxNode *node = tools.First();
  while (node)
  {
    BuildTool *tool = (BuildTool *)node->Data();
    if (tool->bitmapData)
      tool->bitmapData->GenerateBitmapDataInclude(stream);
    node = node->Next();
  }
}

/*
 * USER WINDOWS
 *
 */

/*
 * User toolbar
 *
 */
 
void UserToolbar::OnSize(int w, int h)
{
}

void UserToolbar::OnPaint(void)
{
  wxToolBar::OnPaint();
  int w, h;
  GetClientSize(&w, &h);
  GetDC()->SetPen(wxBLACK_PEN);
  GetDC()->DrawLine(0, h, w, h);
}

Bool UserToolbar::OnLeftClick(int toolIndex, Bool toggleDown)
{
  BuildAction *action = NULL;
  Bool newAction = TRUE;

  // Try to find an action corresponding to this command ID
  wxNode *node = buildWindow->buildActions.First();
  while (node)
  {
    BuildAction *act = (BuildAction *)node->Data();
    if ((act->eventType == wxEVENT_TYPE_TOOL_COMMAND) &&
        (act->commandId == toolIndex))
    {
      action = act;
      newAction = FALSE;
      node = NULL;
    }
    else node = node->Next();
  }
  
  if (buildApp.TestMode())
  {
    if (action)
      action->DoAction();
  }
  else
  {
    if (!action)
    {
      action = CreateNewAction();
      if (!action) return TRUE;

      action->commandId = toolIndex;
      action->eventType = wxEVENT_TYPE_TOOL_COMMAND;
    }

    Bool ok = action->EditAction();
    if (ok)
    {
      if (newAction)
        buildWindow->AddAction(action);
    }
    else
    {
      buildWindow->DeleteAction(action);
    }
  }
  return TRUE;
}

void UserToolbar::OnMouseEnter(int toolIndex)
{
}

BuildTool::BuildTool(void)
{
  isToggle = FALSE;
  toolIdName = copystring("");
  bitmapData = new BuildBitmapData;
  bitmapData->SetBitmapName("");
  helpString = copystring("");
  toolId = -1;
}

BuildTool::~BuildTool(void)
{
  if (helpString) delete[] helpString;
  if (toolIdName) delete[] toolIdName;
//  if (bitmapFileName) delete[] bitmapFileName;
//  if (bitmap) delete bitmap;
  if (bitmapData)
    delete bitmapData;
}

