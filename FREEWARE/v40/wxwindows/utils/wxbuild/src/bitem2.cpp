/*
 * File:	bitem.cc
 * Purpose:	wxWindows GUI builder - panel items
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

#include "wx_form.h"

#ifdef wx_msw
#include "dib.h"
#else
#include "wx_image.h"
#endif

#include <ctype.h>
#include <stdlib.h>

#include "wxbuild.h"
#include "bapp.h"
#include "namegen.h"
#include "bframe.h"
#include "bsubwin.h"
#include "bitem.h"
#include "bactions.h"

/*
 * Text item
 *
 */
 
BuildTextData::BuildTextData(BuildPanelData *theParent):
  BuildItemData(theParent)
{
  x = -1;
  y = -1;
  width = -1;
  height = -1;
  name = copystring(GetNewObjectName("text"));
  memberName = copystring(name);
  className = copystring("wxText");
  title = NULL;
  windowStyle = 0;
  windowType = wxTYPE_TEXT;
  stringValue = NULL;
  functionName = copystring(GetNewObjectName("TextProc"));
}

BuildTextData::~BuildTextData(void)
{
}

Bool BuildTextData::EditAttributes(void)
{
  return BuildItemData::EditAttributes();
}

// Add class-specific items to form
void BuildTextData::AddFormItems(wxForm *form)
{
  BuildItemData::AddFormItems(form);
  form->Add(wxMakeFormBool("Auto size", &autoSize, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL));
  form->Add(wxMakeFormNewLine());

  form->Add(wxMakeFormString("Label position", &labelPositionString, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings("Horizontal", "Vertical", "Default", NULL), NULL),
      NULL, wxVERTICAL));

  SetHelpTopic("Creating a text item");
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Value", &stringValue, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
}

// Add class-specific items to dialog, since forms can't
// cope with everything.

void BuildTextData::AddDialogItems(wxDialogBox *dialog)
{
}

Bool BuildTextData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValue("value", &stringValue);
  return TRUE;
}

Bool BuildTextData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::WritePrologAttributes(expr, database);
  expr->AddAttributeValueString("value", stringValue);
  return TRUE;
}

Bool BuildTextData::MakeRealWindow(void)
{
  if (!(buildParent && buildParent->userWindow)) return FALSE;

  wxPanel *panel = (wxPanel *)buildParent->userWindow;
  if (labelPosition == wxDEFAULT_POSITION)
    panel->SetLabelPosition((((BuildPanelData *)buildParent)->horizLabelPosition  ? wxHORIZONTAL : wxVERTICAL));
  else
    panel->SetLabelPosition(labelPosition);

  if (autoSize)
  {
    width = -1;
    height = -1;
  }

  UserText *text = new UserText((wxPanel *)buildParent->userWindow, (wxFunction)NULL,
                 title, stringValue, x, y,
                 width, height, windowStyle);
  userWindow = text;
  text->buildWindow = this;

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildTextData::DestroyRealWindow(void)
{
  if (userWindow)
  {
    userWindow->GetPosition(&x, &y);
    userWindow->GetSize(&width, &height);
    delete userWindow;
    userWindow = NULL;
  }
  return TRUE;
}

void BuildTextData::WriteClassDeclaration(ostream& stream)
{
  if (!buildApp.useResourceMethod)
    stream << "void " << functionName << "(wxText& text, wxCommandEvent& event);\n\n";
}

// Generation
void BuildTextData::WriteClassImplementation(ostream& stream)
{
  if (!buildApp.useResourceMethod)
  {
    stream << "void " << functionName << "(wxText& text, wxCommandEvent& event)\n";
    stream << "{\n";
    stream << "}\n\n";
  }
}

void BuildTextData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << "wxText" << "(" << parentName;
  stream << ", (wxFunction)" << (functionName ? functionName : "NULL") << ", ";
  stream << SafeString(title) << ", ";
  stream << SafeString(stringValue) << ", ";
  stream << x << ", " << y << ", ";
  stream << width << ", " << height << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", \"" << name << "\")";
}

/*
 * MultiText item
 *
 */
 
BuildMultiTextData::BuildMultiTextData(BuildPanelData *theParent):
  BuildTextData(theParent)
{
  x = -1;
  y = -1;
  width = 300;
  height = 150;

  autoSize = FALSE;
  name = copystring(GetNewObjectName("multitext"));
  memberName = copystring(name);
  className = copystring("wxMultiText");
  title = NULL;
  windowStyle = 0;
  windowType = wxTYPE_MULTI_TEXT;
  stringValue = NULL;
  functionName = copystring(GetNewObjectName("MultiTextProc"));
}

BuildMultiTextData::~BuildMultiTextData(void)
{
}

void BuildMultiTextData::AddFormItems(wxForm *form)
{
  BuildTextData::AddFormItems(form);
  SetHelpTopic("Create a multi-line text item");
}

Bool BuildMultiTextData::MakeRealWindow(void)
{
  if (!(buildParent && buildParent->userWindow)) return FALSE;

  wxPanel *panel = (wxPanel *)buildParent->userWindow;
  if (labelPosition == wxDEFAULT_POSITION)
    panel->SetLabelPosition((((BuildPanelData *)buildParent)->horizLabelPosition  ? wxHORIZONTAL : wxVERTICAL));
  else
    panel->SetLabelPosition(labelPosition);

  if (autoSize)
  {
    width = -1;
    height = -1;
  }

  UserMultiText *text = new UserMultiText((wxPanel *)buildParent->userWindow, (wxFunction)NULL,
                 title, stringValue, x, y,
                 width, height, windowStyle);
  userWindow = text;
  text->buildWindow = this;

  userWindow->Show(TRUE);
  return TRUE;
}

void BuildMultiTextData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << "wxMultiText" << "(" << parentName;
  stream << ", (wxFunction)" << (functionName ? functionName : "NULL") << ", ";
  stream << SafeString(title) << ", ";
  stream << SafeString(stringValue) << ", ";
  stream << x << ", " << y << ", ";
  stream << width << ", " << height << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", \"" << name << "\")";
}

/*
 * List box
 *
 */
 
BuildListBoxData::BuildListBoxData(BuildPanelData *theParent):
  BuildItemData(theParent)
{
  x = -1;
  y = -1;
  width = 200;
  height = 100;

  autoSize = FALSE;
  name = copystring(GetNewObjectName("listbox"));
  memberName = copystring(name);
  className = copystring("wxListBox");
  title = NULL;
  windowStyle = 0;
  windowType = wxTYPE_LIST_BOX;

  multipleSel = FALSE;
  scrollAlways = FALSE;
  functionName = copystring(GetNewObjectName("ListBoxProc"));
  tmpStringArray = NULL;
}

BuildListBoxData::~BuildListBoxData(void)
{
}

Bool BuildListBoxData::EditAttributes(void)
{
  return BuildItemData::EditAttributes();
}

// Add class-specific items to form
void BuildListBoxData::AddFormItems(wxForm *form)
{
  BuildItemData::AddFormItems(form);
  form->Add(wxMakeFormString("Label position", &labelPositionString, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings("Horizontal", "Vertical", "Default", NULL), NULL),
      NULL, wxVERTICAL));
  form->Add(wxMakeFormBool("Auto size", &autoSize, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL));

  SetHelpTopic("Creating a listbox item");
}

// Add class-specific items to dialog, since forms can't
// cope with everything.
static void ListBoxStringsCallback(wxListBox& lbox, wxCommandEvent& event)
{
}

static void ListBoxAddCallback(wxButton& but, wxCommandEvent& event)
{
  BuildListBoxData *data = (BuildListBoxData *)but.GetClientData();
  char *s = (data->currentTextBox->GetValue());
  if (s && (strlen(s) > 0))
  {
    wxNode *node = data->strings.Add(s);
    data->currentStringsBox->Append(s, (char *)node);
    data->currentTextBox->SetValue("");
  }
}

static void ListBoxDeleteCallback(wxButton& but, wxCommandEvent& event)
{
  BuildListBoxData *data = (BuildListBoxData *)but.GetClientData();
  int sel = data->currentStringsBox->GetSelection();
  if (sel > -1)
  {
    wxNode *node = (wxNode *)data->currentStringsBox->GetClientData(sel);
    delete[] (char *)node->Data();
    delete node;
    data->currentStringsBox->Delete(sel);
  }
}

void BuildListBoxData::AddDialogItems(wxDialogBox *dialog)
{
  dialog->NewLine();
  dialog->SetLabelPosition(wxVERTICAL);
  currentStringsBox = new wxListBox(dialog, (wxFunction)ListBoxStringsCallback,
                           "Values", wxSINGLE,-1, -1, 300, 100);
  currentStringsBox->wxWindow::SetClientData((char *)this);
  wxNode *node = strings.First();
  while (node)
  {
    char *s = (char *)node->Data();
    currentStringsBox->Append(s, (char *)node);
    node = node->Next();
  }
  dialog->NewLine();
  dialog->SetLabelPosition(wxHORIZONTAL);
  currentTextBox = new wxText(dialog, (wxFunction)NULL, "Value", "", -1, -1, 200);
//  dialog->NewLine();
  wxButton *addBut = new wxButton(dialog, (wxFunction)ListBoxAddCallback, "Add");
  wxButton *deleteBut = new wxButton(dialog, (wxFunction)ListBoxDeleteCallback, "Delete");
  addBut->SetClientData((char *)this);
  deleteBut->SetClientData((char *)this);
}

Bool BuildListBoxData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValue("multiple_sel", &multipleSel);
  expr->AssignAttributeValue("scroll_always", &scrollAlways);
  expr->AssignAttributeValueStringList("values", &strings);
  return TRUE;
}

Bool BuildListBoxData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::WritePrologAttributes(expr, database);
  expr->AddAttributeValue("multiple_sel", (long)multipleSel);
  expr->AddAttributeValue("scroll_always", (long)scrollAlways);
  expr->AddAttributeValueStringList("values", &strings);
  return TRUE;
}

Bool BuildListBoxData::MakeRealWindow(void)
{
  if (!(buildParent && buildParent->userWindow)) return FALSE;

  wxPanel *panel = (wxPanel *)buildParent->userWindow;
  if (labelPosition == wxDEFAULT_POSITION)
    panel->SetLabelPosition((((BuildPanelData *)buildParent)->horizLabelPosition ? wxHORIZONTAL : wxVERTICAL));
  else
    panel->SetLabelPosition(labelPosition);

  if (autoSize)
  {
    width = -1;
    height = -1;
  }

  windowStyle = 0;
  if (scrollAlways)
    windowStyle |= wxALWAYS_SB;

  char **theStrings = new char*[strings.Number()];
  for (int i = 0; i < strings.Number(); i++)
    theStrings[i] = (char *)(strings.Nth(i))->Data();

  UserListBox *listbox = new UserListBox((wxPanel *)buildParent->userWindow,
                 (wxFunction)NULL, title, (multipleSel ? wxMULTIPLE : wxSINGLE), x, y,
                 width, height, strings.Number(), theStrings, windowStyle);

  delete[] theStrings;

  userWindow = listbox;
  listbox->buildWindow = this;

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildListBoxData::DestroyRealWindow(void)
{
  if (userWindow)
  {
    userWindow->GetPosition(&x, &y);
    userWindow->GetSize(&width, &height);
    delete userWindow;
    userWindow = NULL;
  }
  return TRUE;
}

void BuildListBoxData::WriteClassDeclaration(ostream& stream)
{
  if (!buildApp.useResourceMethod)
    stream << "void " << functionName << "(wxListBox& lbox, wxCommandEvent& event);\n\n";
}

// Generation
void BuildListBoxData::WriteClassImplementation(ostream& stream)
{
  if (!buildApp.useResourceMethod)
  {
    stream << "void " << functionName << "(wxListBox& lbox, wxCommandEvent& event)\n";
    stream << "{\n";
    stream << "}\n\n";
  }
}

void BuildListBoxData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << "wxListBox" << "(" << parentName;
  stream << ", " << "(wxFunction)" << (functionName ? functionName : "NULL") << ", " << SafeString(title) << ", ";
  stream << (multipleSel ? "wxMULTIPLE" : "wxSINGLE") << ", " << x << ", " << y << ", ";
  stream << width << ", " << height << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << strings.Number() << ", " << ((strings.Number() == 0) ? "NULL" : tmpStringArray)
         << ", " << buf << ", " << "\"" << name << "\")";
}

void BuildListBoxData::GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation)
{
  BuildItemData::GenerateWindowInitialisationPre(stream, instanceName, indentation);
  if (strings.Number() != 0)
  {
    if (tmpStringArray) delete[] tmpStringArray;
    tmpStringArray = GetNewObjectName("stringArray");
    stream << indentation << "char *" << tmpStringArray << "[] = { ";
    wxNode *node = strings.First();
    while (node)
    {
      char *s = (char *)node->Data();
      stream << "\"" << s << "\"";
      node = node->Next();
      if (node) stream << ", "; else stream << " ";
    }
    stream << "};\n";
  }
}

/*
 * Choice
 *
 */

BuildChoiceData::BuildChoiceData(BuildPanelData *theParent):
  BuildItemData(theParent)
{
  x = -1;
  y = -1;
  width = 200;
  height = 100;
  name = copystring(GetNewObjectName("choice"));
  memberName = copystring(name);
  className = copystring("wxChoice");
  title = NULL;
  windowStyle = 0;
  windowType = wxTYPE_CHOICE;
  functionName = copystring(GetNewObjectName("ChoiceProc"));
  tmpStringArray = NULL;
}

BuildChoiceData::~BuildChoiceData(void)
{
}

Bool BuildChoiceData::EditAttributes(void)
{
  return BuildItemData::EditAttributes();
}

// Add class-specific items to form
void BuildChoiceData::AddFormItems(wxForm *form)
{
  BuildItemData::AddFormItems(form);
  form->Add(wxMakeFormBool("Auto size", &autoSize, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL));
  form->Add(wxMakeFormNewLine());

  form->Add(wxMakeFormString("Label position", &labelPositionString, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings("Horizontal", "Vertical", "Default", NULL), NULL),
      NULL, wxVERTICAL));
  SetHelpTopic("Creating a choice item");
}

// Add class-specific items to dialog, since forms can't
// cope with everything.
static void ChoiceStringsCallback(wxListBox& lbox, wxCommandEvent& event)
{
}

static void ChoiceAddCallback(wxButton& but, wxCommandEvent& event)
{
  BuildChoiceData *data = (BuildChoiceData *)but.GetClientData();
  char *s = (data->currentTextBox->GetValue());
  if (s && (strlen(s) > 0))
  {
    wxNode *node = data->strings.Add(s);
    data->currentStringsBox->Append(s, (char *)node);
    data->currentTextBox->SetValue("");
  }
}

static void ChoiceDeleteCallback(wxButton& but, wxCommandEvent& event)
{
  BuildChoiceData *data = (BuildChoiceData *)but.GetClientData();
  int sel = data->currentStringsBox->GetSelection();
  if (sel > -1)
  {
    wxNode *node = (wxNode *)data->currentStringsBox->GetClientData(sel);
    delete[] (char *)node->Data();
    delete node;
    data->currentStringsBox->Delete(sel);
  }
}

void BuildChoiceData::AddDialogItems(wxDialogBox *dialog)
{
  dialog->NewLine();
  dialog->SetLabelPosition(wxVERTICAL);
  currentStringsBox = new wxListBox(dialog, (wxFunction)ChoiceStringsCallback,
                           "Values", wxSINGLE,-1, -1, 300, 100);
  wxNode *node = strings.First();
  while (node)
  {
    char *s = (char *)node->Data();
    currentStringsBox->Append(s, (char *)node);
    node = node->Next();
  }
  currentStringsBox->wxWindow::SetClientData((char *)this);
  dialog->NewLine();
  dialog->SetLabelPosition(wxHORIZONTAL);
  currentTextBox = new wxText(dialog, (wxFunction)NULL, "Value", "", -1, -1, 200);
//  dialog->NewLine();
  wxButton *addBut = new wxButton(dialog, (wxFunction)ChoiceAddCallback, "Add");
  wxButton *deleteBut = new wxButton(dialog, (wxFunction)ChoiceDeleteCallback, "Delete");
  addBut->SetClientData((char *)this);
  deleteBut->SetClientData((char *)this);
}

Bool BuildChoiceData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValueStringList("values", &strings);
  return TRUE;
}

Bool BuildChoiceData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::WritePrologAttributes(expr, database);
  expr->AddAttributeValueStringList("values", &strings);
  return TRUE;
}

Bool BuildChoiceData::MakeRealWindow(void)
{
  if (!(buildParent && buildParent->userWindow)) return FALSE;

  wxPanel *panel = (wxPanel *)buildParent->userWindow;
  if (labelPosition == wxDEFAULT_POSITION)
    panel->SetLabelPosition((((BuildPanelData *)buildParent)->horizLabelPosition ? wxHORIZONTAL : wxVERTICAL));
  else
    panel->SetLabelPosition(labelPosition);

  if (autoSize)
  {
    width = -1;
    height = -1;
  }

  windowStyle = 0;

  char **theStrings = new char*[strings.Number()];
  for (int i = 0; i < strings.Number(); i++)
    theStrings[i] = (char *)(strings.Nth(i))->Data();

  UserChoice *choice = new UserChoice((wxPanel *)buildParent->userWindow,
                 (wxFunction)NULL, title, x, y,
                 width, -1, strings.Number(), theStrings, windowStyle);

  delete[] theStrings;

  userWindow = choice;
  choice->buildWindow = this;

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildChoiceData::DestroyRealWindow(void)
{
  if (userWindow)
  {
    userWindow->GetPosition(&x, &y);
    userWindow->GetSize(&width, &height);
    delete userWindow;
    userWindow = NULL;
  }
  return TRUE;
}

void BuildChoiceData::WriteClassDeclaration(ostream& stream)
{
  if (!buildApp.useResourceMethod)
    stream << "void " << functionName << "(wxChoice& choice, wxCommandEvent& event);\n\n";
}

// Generation
void BuildChoiceData::WriteClassImplementation(ostream& stream)
{
  if (!buildApp.useResourceMethod)
  {
    stream << "void " << functionName << "(wxChoice& choice, wxCommandEvent& event)\n";
    stream << "{\n";
    stream << "}\n\n";
  }
}

void BuildChoiceData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << "wxChoice" << "(" << parentName;
  stream << ", " << "(wxFunction)" << (functionName ? functionName : "NULL") << ", " << SafeString(title) << ", " << x << ", " << y << ", ";
  stream << "-1" << ", " << "-1" << ", ";

  stream << strings.Number() << ", " << ((strings.Number() == 0) ? "NULL" : tmpStringArray) << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", \"" << name << "\")";
}

void BuildChoiceData::GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation)
{
  BuildItemData::GenerateWindowInitialisationPre(stream, instanceName, indentation);
  if (strings.Number() != 0)
  {
    if (tmpStringArray) delete[] tmpStringArray;
    tmpStringArray = GetNewObjectName("stringArray");
    stream << indentation << "char *" << tmpStringArray << "[] = { ";
    wxNode *node = strings.First();
    while (node)
    {
      char *s = (char *)node->Data();
      stream << "\"" << s << "\"";
      node = node->Next();
      if (node) stream << ", "; else stream << " ";
    }
    stream << "};\n";
  }
}

/*
 * RadioBox
 *
 */

BuildRadioBoxData::BuildRadioBoxData(BuildPanelData *theParent):
  BuildItemData(theParent)
{
  x = -1;
  y = -1;
  width = 200;
  height = 100;
  name = copystring(GetNewObjectName("radiobox"));
  memberName = copystring(name);
  className = copystring("wxRadioBox");
  title = copystring("untitled");
  windowStyle = 0;
  windowType = wxTYPE_RADIO_BOX;
  functionName = copystring(GetNewObjectName("RadioBoxProc"));
  noRowsCols = 2;
  tmpStringArray = NULL;
}

BuildRadioBoxData::~BuildRadioBoxData(void)
{
}

Bool BuildRadioBoxData::EditAttributes(void)
{
  return BuildItemData::EditAttributes();
}

// Add class-specific items to form
void BuildRadioBoxData::AddFormItems(wxForm *form)
{
  BuildItemData::AddFormItems(form);
  form->Add(wxMakeFormBool("Auto size", &autoSize, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL));
  form->Add(wxMakeFormNewLine());

  form->Add(wxMakeFormString("Label position", &labelPositionString, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings("Horizontal", "Vertical", "Default", NULL), NULL),
      NULL, wxVERTICAL));
  form->Add(wxMakeFormShort("Rows/cols", &noRowsCols, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               100));
  
  SetHelpTopic("Creating a radiobox item");
}

// Add class-specific items to dialog, since forms can't
// cope with everything.
static void RadioBoxStringsCallback(wxListBox& lbox, wxCommandEvent& event)
{
}

static void RadioBoxAddCallback(wxButton& but, wxCommandEvent& event)
{
  BuildRadioBoxData *data = (BuildRadioBoxData *)but.GetClientData();
  char *s = (data->currentTextBox->GetValue());
  if (s && (strlen(s) > 0))
  {
    wxNode *node = data->strings.Add(s);
    data->currentStringsBox->Append(s, (char *)node);
    data->currentTextBox->SetValue("");
  }
}

static void RadioBoxDeleteCallback(wxButton& but, wxCommandEvent& event)
{
  BuildRadioBoxData *data = (BuildRadioBoxData *)but.GetClientData();
  int sel = data->currentStringsBox->GetSelection();
  if (sel > -1)
  {
    wxNode *node = (wxNode *)data->currentStringsBox->GetClientData(sel);
    delete[] (char *)node->Data();
    delete node;
    data->currentStringsBox->Delete(sel);
  }
}

void BuildRadioBoxData::AddDialogItems(wxDialogBox *dialog)
{
  dialog->NewLine();
  dialog->SetLabelPosition(wxVERTICAL);
  currentStringsBox = new wxListBox(dialog, (wxFunction)RadioBoxStringsCallback,
                           "Values", wxSINGLE,-1, -1, 300, 100);
  currentStringsBox->wxWindow::SetClientData((char *)this);
  wxNode *node = strings.First();
  while (node)
  {
    char *s = (char *)node->Data();
    currentStringsBox->Append(s, (char *)node);
    node = node->Next();
  }
  dialog->NewLine();
  dialog->SetLabelPosition(wxHORIZONTAL);
  currentTextBox = new wxText(dialog, (wxFunction)NULL, "Value", "", -1, -1, 200);
//  dialog->NewLine();
  wxButton *addBut = new wxButton(dialog, (wxFunction)RadioBoxAddCallback, "Add");
  wxButton *deleteBut = new wxButton(dialog, (wxFunction)RadioBoxDeleteCallback, "Delete");
  addBut->SetClientData((char *)this);
  deleteBut->SetClientData((char *)this);
}

Bool BuildRadioBoxData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValueStringList("values", &strings);
  expr->AssignAttributeValue("rows_or_cols", &noRowsCols);
  return TRUE;
}

Bool BuildRadioBoxData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::WritePrologAttributes(expr, database);
  expr->AddAttributeValueStringList("values", &strings);
  expr->AddAttributeValue("rows_or_cols", (long)noRowsCols);
  return TRUE;
}

Bool BuildRadioBoxData::MakeRealWindow(void)
{
  if (!(buildParent && buildParent->userWindow)) return FALSE;

  wxPanel *panel = (wxPanel *)buildParent->userWindow;
  if (labelPosition == wxDEFAULT_POSITION)
    panel->SetLabelPosition((((BuildPanelData *)buildParent)->horizLabelPosition ? wxHORIZONTAL : wxVERTICAL));
  else
    panel->SetLabelPosition(labelPosition);

  if (autoSize)
  {
    width = -1;
    height = -1;
  }

  windowStyle = 0;

  char **theStrings = new char*[strings.Number()];
  for (int i = 0; i < strings.Number(); i++)
    theStrings[i] = (char *)(strings.Nth(i))->Data();

  UserRadioBox *radiobox = new UserRadioBox((wxPanel *)buildParent->userWindow,
                 (wxFunction)NULL, title, x, y,
                 -1, -1, strings.Number(), theStrings, noRowsCols, windowStyle);

  delete[] theStrings;
  
  userWindow = radiobox;
  radiobox->buildWindow = this;

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildRadioBoxData::DestroyRealWindow(void)
{
  if (userWindow)
  {
    userWindow->GetPosition(&x, &y);
    userWindow->GetSize(&width, &height);
    delete userWindow;
    userWindow = NULL;
  }
  return TRUE;
}

void BuildRadioBoxData::WriteClassDeclaration(ostream& stream)
{
  if (!buildApp.useResourceMethod)
    stream << "void " << functionName << "(wxRadioBox& radioBox, wxCommandEvent& event);\n\n";
}

// Generation
void BuildRadioBoxData::WriteClassImplementation(ostream& stream)
{
  if (!buildApp.useResourceMethod)
  {
    stream << "void " << functionName << "(wxRadioBox& radioBox, wxCommandEvent& event)\n";
    stream << "{\n";
    stream << "}\n\n";
  }
}

void BuildRadioBoxData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << "wxRadioBox" << "(" << parentName;
  stream << ", " << "(wxFunction)" << (functionName ? functionName : "NULL") << ", " << SafeString(title) << ", " << x << ", " << y << ", ";
  stream << "-1" << ", " << "-1" << ", ";
  stream << strings.Number() << ", " << ((strings.Number() == 0) ? "NULL" : tmpStringArray) << ", ";
  stream << noRowsCols << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", \"" << name << "\")";
}

void BuildRadioBoxData::GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation)
{
  BuildItemData::GenerateWindowInitialisationPre(stream, instanceName, indentation);
  if (strings.Number() != 0)
  {
    if (tmpStringArray) delete[] tmpStringArray;
    tmpStringArray = GetNewObjectName("stringArray");
    stream << indentation << "char *" << tmpStringArray << "[] = { ";
    wxNode *node = strings.First();
    while (node)
    {
      char *s = (char *)node->Data();
      stream << "\"" << s << "\"";
      node = node->Next();
      if (node) stream << ", "; else stream << " ";
    }
    stream << "};\n";
  }
}

/*
 * Slider
 *
 */

BuildSliderData::BuildSliderData(BuildPanelData *theParent):
  BuildItemData(theParent)
{
  x = -1;
  y = -1;
  width = 200;
  height = -1;
  minValue = 0;
  maxValue = 100;
  sliderValue = 0;
  
  name = copystring(GetNewObjectName("slider"));
  memberName = copystring(name);
  className = copystring("wxSlider");
  title = copystring("untitled");
  windowStyle = 0;
  windowType = wxTYPE_SLIDER;
  functionName = copystring(GetNewObjectName("SliderProc"));
}

BuildSliderData::~BuildSliderData(void)
{
}

Bool BuildSliderData::EditAttributes(void)
{
  return BuildItemData::EditAttributes();
}

// Add class-specific items to form
void BuildSliderData::AddFormItems(wxForm *form)
{
  BuildItemData::AddFormItems(form);
  form->Add(wxMakeFormBool("Auto size", &autoSize, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL));
  form->Add(wxMakeFormNewLine());

  form->Add(wxMakeFormString("Label position", &labelPositionString, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings("Horizontal", "Vertical", "Default", NULL), NULL),
      NULL, wxVERTICAL));
  form->Add(wxMakeFormShort("Min value", &minValue, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               100));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormShort("Max value", &maxValue, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               100));
  
  SetHelpTopic("Creating a slider item");
}

void BuildSliderData::AddDialogItems(wxDialogBox *dialog)
{
}

Bool BuildSliderData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValue("min_value", &minValue);
  expr->AssignAttributeValue("max_value", &maxValue);
  expr->AssignAttributeValue("slider_value", &sliderValue);
  return TRUE;
}

Bool BuildSliderData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  if (userWindow)
    sliderValue = ((wxSlider *)userWindow)->GetValue();

  BuildItemData::WritePrologAttributes(expr, database);
  expr->AddAttributeValue("min_value", (long)minValue);
  expr->AddAttributeValue("max_value", (long)maxValue);
  expr->AddAttributeValue("slider_value", (long)sliderValue);
  return TRUE;
}

Bool BuildSliderData::MakeRealWindow(void)
{
  if (!(buildParent && buildParent->userWindow)) return FALSE;

  wxPanel *panel = (wxPanel *)buildParent->userWindow;
  if (labelPosition == wxDEFAULT_POSITION)
    panel->SetLabelPosition((((BuildPanelData *)buildParent)->horizLabelPosition ? wxHORIZONTAL : wxVERTICAL));
  else
    panel->SetLabelPosition(labelPosition);

  if (autoSize)
  {
    width = -1;
    height = -1;
  }

  windowStyle = 0;

  UserSlider *slider = new UserSlider((wxPanel *)buildParent->userWindow,
                 (wxFunction)NULL, title, sliderValue, minValue, maxValue, width, x, y,
                 windowStyle);

  userWindow = slider;
  slider->buildWindow = this;

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildSliderData::DestroyRealWindow(void)
{
  if (userWindow)
  {
    sliderValue = ((wxSlider *)userWindow)->GetValue();
    userWindow->GetPosition(&x, &y);
    userWindow->GetSize(&width, &height);
    delete userWindow;
    userWindow = NULL;
  }
  return TRUE;
}

void BuildSliderData::WriteClassDeclaration(ostream& stream)
{
  if (!buildApp.useResourceMethod)
    stream << "void " << functionName << "(wxSlider& slider, wxCommandEvent& event);\n\n";
}

// Generation
void BuildSliderData::WriteClassImplementation(ostream& stream)
{
  if (!buildApp.useResourceMethod)
  {
    stream << "void " << functionName << "(wxSlider& slider, wxCommandEvent& event)\n";
    stream << "{\n";
    stream << "}\n\n";
  }
}

void BuildSliderData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  if (userWindow)
  {
    sliderValue = ((wxSlider *)userWindow)->GetValue();
  }
  stream << "new ";
  stream << "wxSlider" << "(" << parentName;
  stream << ", " << "(wxFunction)" << (functionName ? functionName : "NULL") << ", " << SafeString(title) << ", ";
  stream << sliderValue << ", " << minValue << ", " << maxValue << ", " << width << ", ";
  stream << x << ", " << y << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", \"" << name << "\")";
}

void BuildSliderData::GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation)
{
  BuildItemData::GenerateWindowInitialisationPre(stream, instanceName, indentation);
}

/*
 * Gauge
 *
 */

BuildGaugeData::BuildGaugeData(BuildPanelData *theParent):
  BuildItemData(theParent)
{
  x = -1;
  y = -1;
  width = 200;
  height = 30;
  maxValue = 100;
  gaugeValue = 0;
  orientation = copystring("Horizontal");
  autoSize = FALSE;
  
  name = copystring(GetNewObjectName("gauge"));
  memberName = copystring(name);
  className = copystring("wxGauge");
//  title = copystring("untitled");
  windowStyle = wxHORIZONTAL;
  windowType = wxTYPE_GAUGE;
}

BuildGaugeData::~BuildGaugeData(void)
{
  if (orientation)
    delete[] orientation;
}

Bool BuildGaugeData::EditAttributes(void)
{
  return BuildItemData::EditAttributes();
}

// Add class-specific items to form
void BuildGaugeData::AddFormItems(wxForm *form)
{
  BuildItemData::AddFormItems(form);
  form->Add(wxMakeFormBool("Auto size", &autoSize, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL));
  form->Add(wxMakeFormNewLine());

  form->Add(wxMakeFormString("Label position", &labelPositionString, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings("Horizontal", "Vertical", "Default", NULL), NULL),
      NULL, wxVERTICAL));
  form->Add(wxMakeFormString("Orientation", &orientation, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings("Horizontal", "Vertical", "Default", NULL), NULL),
      NULL, wxVERTICAL));
//  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormShort("Range", &maxValue, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               100));
  
  SetHelpTopic("Creating a gauge item");
}

void BuildGaugeData::AddDialogItems(wxDialogBox *dialog)
{
}

Bool BuildGaugeData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValue("max_value", &maxValue);
  expr->AssignAttributeValue("gauge_value", &gaugeValue);
  expr->AssignAttributeValue("orientation", &orientation);
  return TRUE;
}

Bool BuildGaugeData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
//  if (userWindow)
//    gaugeValue = ((wxGauge *)userWindow)->GetValue();

  BuildItemData::WritePrologAttributes(expr, database);
  expr->AddAttributeValue("max_value", (long)maxValue);
  expr->AddAttributeValue("gauge_value", (long)gaugeValue);
  expr->AddAttributeValueString("orientation", orientation);
  return TRUE;
}

Bool BuildGaugeData::MakeRealWindow(void)
{
  if (!(buildParent && buildParent->userWindow)) return FALSE;

  wxPanel *panel = (wxPanel *)buildParent->userWindow;
  if (labelPosition == wxDEFAULT_POSITION)
    panel->SetLabelPosition((((BuildPanelData *)buildParent)->horizLabelPosition ? wxHORIZONTAL : wxVERTICAL));
  else
    panel->SetLabelPosition(labelPosition);

  if (strcmp(orientation, "Horizontal") == 0)
    windowStyle = wxHORIZONTAL;
  else
    windowStyle = wxVERTICAL;

  if (autoSize)
  {
    width = -1;
    height = -1;
  }

  UserGauge *gauge = new UserGauge((wxPanel *)buildParent->userWindow,
                 title, maxValue, x, y, width, height, windowStyle);
  gauge->SetValue(gaugeValue);

  userWindow = gauge;
  gauge->buildWindow = this;

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildGaugeData::DestroyRealWindow(void)
{
  if (userWindow)
  {
    userWindow->GetPosition(&x, &y);
    userWindow->GetSize(&width, &height);
    delete userWindow;
    userWindow = NULL;
  }
  return TRUE;
}

void BuildGaugeData::WriteClassDeclaration(ostream& stream)
{
}

// Generation
void BuildGaugeData::WriteClassImplementation(ostream& stream)
{
}

void BuildGaugeData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << "wxGauge" << "(" << parentName;
  stream << ", " << SafeString(title) << ", ";
  stream << maxValue << ", " << x << ", " << y << ", " << width << ", " << height << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", \"" << name << "\")";
}

void BuildGaugeData::GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation)
{
  BuildItemData::GenerateWindowInitialisationPre(stream, instanceName, indentation);
}

/*
 * Group box
 *
 */

BuildGroupBoxData::BuildGroupBoxData(BuildPanelData *theParent):
  BuildItemData(theParent)
{
  x = -1;
  y = -1;
  width = 200;
  height = 200;

  autoSize = FALSE;  
  name = copystring(GetNewObjectName("groupbox"));
  memberName = copystring(name);
  className = copystring("wxGroupBox");
  title = copystring("untitled");
  windowStyle = 0;
  windowType = wxTYPE_GROUP_BOX;
}

BuildGroupBoxData::~BuildGroupBoxData(void)
{
}

Bool BuildGroupBoxData::EditAttributes(void)
{
  return BuildItemData::EditAttributes();
}

// Add class-specific items to form
void BuildGroupBoxData::AddFormItems(wxForm *form)
{
  BuildItemData::AddFormItems(form);
  SetHelpTopic("Creating a groupbox item");
}

void BuildGroupBoxData::AddDialogItems(wxDialogBox *dialog)
{
}

Bool BuildGroupBoxData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::ReadPrologAttributes(expr, database);
  return TRUE;
}

Bool BuildGroupBoxData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::WritePrologAttributes(expr, database);
  return TRUE;
}

Bool BuildGroupBoxData::MakeRealWindow(void)
{
  if (!(buildParent && buildParent->userWindow)) return FALSE;

  wxPanel *panel = (wxPanel *)buildParent->userWindow;
  if (labelPosition == wxDEFAULT_POSITION)
    panel->SetLabelPosition((((BuildPanelData *)buildParent)->horizLabelPosition ? wxHORIZONTAL : wxVERTICAL));
  else
    panel->SetLabelPosition(labelPosition);
/*
  if (autoSize)
  {
    width = -1;
    height = -1;
  }
*/
  windowStyle = 0;

  UserGroupBox *group = new UserGroupBox((wxPanel *)buildParent->userWindow,
                 title, x, y, width, height, windowStyle);

  userWindow = group;
  group->buildWindow = this;

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildGroupBoxData::DestroyRealWindow(void)
{
  if (userWindow)
  {
    userWindow->GetPosition(&x, &y);
    userWindow->GetSize(&width, &height);
    delete userWindow;
    userWindow = NULL;
  }
  return TRUE;
}

void BuildGroupBoxData::WriteClassDeclaration(ostream& stream)
{
}

// Generation
void BuildGroupBoxData::WriteClassImplementation(ostream& stream)
{
}

void BuildGroupBoxData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << "wxGroupBox" << "(" << parentName;
  stream << ", " << SafeString(title) << ", ";
  stream << x << ", " << y << ", " << width << ", " << height << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", \"" << name << "\")";
}

void BuildGroupBoxData::GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation)
{
  BuildItemData::GenerateWindowInitialisationPre(stream, instanceName, indentation);
}

/*
 * Callbacks
 *
 */
 
void UserButtonCallback(UserButton& but, wxCommandEvent& event)
{
  wxNode *node = but.buildWindow->buildActions.First();
  if (buildApp.TestMode())
  {
    if (node)
    {
      BuildAction *action = (BuildAction *)node->Data();
      action->DoAction();
    }
  }
  else
  {
    BuildAction *action = NULL;
    
    if (!node)
    {
      action = CreateNewAction();
      if (!action) return;
    }
    else
      action = (BuildAction *)node->Data();

    Bool ok = action->EditAction();
    if (ok)
    {
      if (!node)
        but.buildWindow->AddAction(action);
    }
    else
    {
      but.buildWindow->DeleteAction(action);
    }
  }
}
 
void BuildItemData::GenerateWindowStyleString(char *buf)
{
  BuildWindowData::GenerateWindowStyleString(buf);

  GenerateStyle(buf, wxHORIZONTAL, "wxHORIZONTAL");
  GenerateStyle(buf, wxVERTICAL, "wxVERTICAL");
}

void BuildRadioBoxData::GenerateWindowStyleString(char *buf)
{
  buf[0] = 0;
  BuildItemData::GenerateWindowStyleString(buf);

  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void BuildMessageData::GenerateWindowStyleString(char *buf)
{
  buf[0] = 0;
  BuildItemData::GenerateWindowStyleString(buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void BuildTextData::GenerateWindowStyleString(char *buf)
{
  buf[0] = 0;
  BuildItemData::GenerateWindowStyleString(buf);
  
  GenerateStyle(buf, wxPROCESS_ENTER, "wxPROCESS_ENTER");
  GenerateStyle(buf, wxREADONLY, "wxREADONLY");
  GenerateStyle(buf, wxPASSWORD, "wxPASSWORD");

  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void BuildMultiTextData::GenerateWindowStyleString(char *buf)
{
  BuildTextData::GenerateWindowStyleString(buf);
}

void BuildButtonData::GenerateWindowStyleString(char *buf)
{
  buf[0] = 0;
  BuildItemData::GenerateWindowStyleString(buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void BuildCheckBoxData::GenerateWindowStyleString(char *buf)
{
  buf[0] = 0;
  BuildItemData::GenerateWindowStyleString(buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void BuildListBoxData::GenerateWindowStyleString(char *buf)
{
  buf[0] = 0;
  BuildItemData::GenerateWindowStyleString(buf);
  
  GenerateStyle(buf, wxALWAYS_SB, "wxALWAYS_SB");
  GenerateStyle(buf, wxALWAYS_SB, "wxLB_SORT");

  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void BuildSliderData::GenerateWindowStyleString(char *buf)
{
  buf[0] = 0;
  BuildItemData::GenerateWindowStyleString(buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void BuildGroupBoxData::GenerateWindowStyleString(char *buf)
{
  buf[0] = 0;
  BuildItemData::GenerateWindowStyleString(buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void BuildGaugeData::GenerateWindowStyleString(char *buf)
{
  buf[0] = 0;
  BuildItemData::GenerateWindowStyleString(buf);
  
  GenerateStyle(buf, wxGA_PROGRESSBAR, "wxGA_PROGRESSBAR");

  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void BuildChoiceData::GenerateWindowStyleString(char *buf)
{
  buf[0] = 0;
  BuildItemData::GenerateWindowStyleString(buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

/*
void BuildScrollBarData::GenerateWindowStyleString(char *buf)
{
  buf[0] = 0;
  BuildItemData::GenerateWindowStyleString(buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}
*/

