/*
 * File:	bsubwin.cc
 * Purpose:	wxWindows GUI builder - subwindows
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

/*
 * Changes: (Panel or Dilogbox) OnItemLeftClick: 
 *          if Clicked with Cntrl
 *                only the Action-Popup will appear
 *          if Clicked with Shift
 *                all other items stay unchanged
 *          else
 *                all other items become unselected first
 *          if item was selected 
 *                item becomes unselected
 *          else
 *                item becomes selected
 *
 *  Author: Claus Anders 29.02.1996
 */          

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wx_form.h"
#include <ctype.h>
#include <stdlib.h>

#include "wxbuild.h"
#include "bapp.h"
#include "namegen.h"
#include "bframe.h"
#include "bsubwin.h"
#include "bitem.h"
#include "bgdi.h"
#include "bactions.h"

#ifdef wx_x
#include "wx_image.h"
#endif
#ifdef wx_msw
#include "dib.h"
#endif

#ifdef wx_x
#define DEFAULT_EDITOR "emacs"
#endif
#ifdef wx_msw
#define DEFAULT_EDITOR "notepad"
#endif
void SetAction(UserButton *but);
void DoUnselectAll(wxItem* item,wxNode *node);

BuildSubwindowData::BuildSubwindowData(BuildFrameData *theParent):
  BuildWindowData(theParent)
{
  percentOfFrame = 100;
  hasBorder = FALSE;
  resizeMode = RESIZE_PROPORTIONAL;
  resizeModeString = copystring("Proportional");
  labelFont = NULL;
  buttonFont = NULL;
}

BuildSubwindowData::~BuildSubwindowData(void)
{
  if (resizeModeString)
    delete[] resizeModeString;
  if (labelFont)
    delete[] labelFont;
  if (buttonFont)
    delete[] buttonFont;
}

Bool BuildSubwindowData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildWindowData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValue("percent_of_frame", &percentOfFrame);
  expr->AssignAttributeValue("border", &hasBorder);
  expr->AssignAttributeValue("resize_mode", &resizeMode);
  expr->AssignAttributeValue("label_font", &labelFont);
  expr->AssignAttributeValue("button_font", &buttonFont);
  return TRUE;
}

Bool BuildSubwindowData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildWindowData::WritePrologAttributes(expr, database);
  expr->AddAttributeValue("percent_of_frame", (long)percentOfFrame);
  expr->AddAttributeValue("border", (long)hasBorder);
  expr->AddAttributeValue("resize_mode", (long)resizeMode);
  expr->AddAttributeValueString("label_font", labelFont);
  expr->AddAttributeValueString("button_font", buttonFont);
  return TRUE;
}

void BuildSubwindowData::FindScreenPosition(int *sx, int *sy)
{
  if (!userWindow)
  {
    *sx = x; *sy = y; return;
  }

  if (buildParent)
  {
    userWindow->GetPosition(sx, sy);
    buildParent->userWindow->ClientToScreen(sx, sy);
  }
}

void BuildSubwindowData::FindClientPosition(int sx, int sy, int *cx, int *cy)
{
  if (buildParent)
  {
    *cx = sx;
    *cy = sy;
    buildParent->userWindow->ScreenToClient(cx, cy);
  }
  x = *cx; y = *cy;
}

// Generation
void BuildSubwindowData::WriteClassImplementation(ostream& stream)
{
}

void BuildSubwindowData::WriteClassDeclaration(ostream& stream)
{
}

void BuildSubwindowData::GenerateWindowStyleString(char *buf)
{
  buf[0] = 0;
  BuildWindowData::GenerateWindowStyleString(buf);
  GenerateStyle(buf, wxCAPTION, "wxCAPTION");
  GenerateStyle(buf, wxRETAINED, "wxRETAINED");
  GenerateStyle(buf, wxTHICK_FRAME, "wxTHICK_FRAME");
  GenerateStyle(buf, wxRESIZE_BORDER, "wxRESIZE_BORDER");
  GenerateStyle(buf, wxSYSTEM_MENU, "wxSYSTEM_MENU");
  GenerateStyle(buf, wxMINIMIZE_BOX, "wxMINIMIZE_BOX");
  GenerateStyle(buf, wxMAXIMIZE_BOX, "wxMAXIMIZE_BOX");

  if (strlen(buf) == 0)
    strcat(buf, "0");
}

// Add class-specific items to form
void BuildSubwindowData::AddFormItems(wxForm *form)
{
  form->Add(wxMakeFormString("Name", &name, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Description", &description, wxFORM_MULTITEXT, NULL, NULL, wxVERTICAL,
               300, 80));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Class name", &className, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               200));
  if (windowType == wxTYPE_DIALOG_BOX)
  {
    form->Add(wxMakeFormNewLine());
    form->Add(wxMakeFormString("Title", &title, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
    form->Add(wxMakeFormNewLine());
  }
  if (windowType != wxTYPE_DIALOG_BOX)
  {
    form->Add(wxMakeFormBool("Border", &hasBorder));
    form->Add(wxMakeFormNewLine());
    form->Add(wxMakeFormString("Resize strategy", &resizeModeString, wxFORM_RADIOBOX,
                new wxList(wxMakeConstraintStrings("Fixed", "Grow", "Proportional", NULL), NULL),
              NULL, wxVERTICAL));
    form->Add(wxMakeFormNewLine());
  }
  form->Add(wxMakeFormShort("Width", &width, wxFORM_DEFAULT, NULL, NULL, wxHORIZONTAL, 100));
  form->Add(wxMakeFormShort("Height", &height, wxFORM_DEFAULT, NULL, NULL, wxHORIZONTAL, 100));
  if (windowType != wxTYPE_DIALOG_BOX)
    form->Add(wxMakeFormShort("% frame", &percentOfFrame, wxFORM_DEFAULT, NULL, NULL, wxHORIZONTAL, 100));
}

// Add class-specific items to dialog, since forms can't
// cope with everything.
void BuildSubwindowData::AddDialogItems(wxDialogBox *dialog)
{
}


// Generate bitmap, icon etc. resource entries for RC file.
void BuildSubwindowData::GenerateResourceEntries(ostream &stream)
{
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->GenerateResourceEntries(stream);
    node = node->Next();
  }
}

/*
 * Panels
 *
 */
 
BuildPanelData::BuildPanelData(BuildFrameData *theParent):
  BuildSubwindowData(theParent)
{
  x = 30;
  y = 30;
  width = 300;
  height = 300;
  name = copystring(GetNewObjectName("panel"));
//  memberName = copystring(name);
  className = copystring(GetNewObjectName("PanelClass"));
  title = copystring("untitled");
  windowStyle = 0;
  windowType = wxTYPE_PANEL;

  fitContents = FALSE;
  relativeLayout = FALSE;
  horizLabelPosition = TRUE;
}

BuildPanelData::~BuildPanelData(void)
{
}

Bool BuildPanelData::EditAttributes(void)
{
  char nameBuf[200];
  sprintf(nameBuf, "Panel Properties for %s", name);
  wxDialogBox *dialog = new wxDialogBox(NULL, nameBuf, TRUE, 10, 10);
  dialog->SetLabelFont(SmallLabelFont);
  dialog->SetButtonFont(SmallButtonFont);

  BuildForm *form = new BuildForm("Creating a panel subwindow");

  AddFormItems(form);

  form->AssociatePanel(dialog);
  form->dialog = dialog;
  AddDialogItems(dialog);

  dialog->Fit();
  dialog->Centre(wxBOTH);

  form->RevertValues();

  dialog->Show(TRUE);
  if (strcmp(resizeModeString, "Proportional") == 0)
    resizeMode = RESIZE_PROPORTIONAL;
  else if (strcmp(resizeModeString, "Fixed") == 0)
    resizeMode = RESIZE_FIXED;
  else if (strcmp(resizeModeString, "Grow") == 0)
    resizeMode = RESIZE_GROW;

  BuildFrameData *frame = (BuildFrameData *)buildParent;
  frame->userWindow->OnSize(-1, -1);
  return TRUE;
}

// Add class-specific items to form
void BuildPanelData::AddFormItems(wxForm *form)
{
  BuildSubwindowData::AddFormItems(form);
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormBool("Fit contents", &fitContents));
//  form->Add(wxMakeFormBool("Relative layout", &relativeLayout));
  form->Add(wxMakeFormBool("Horizontal labels", &horizLabelPosition));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Label font", &labelFont, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               200));
  form->Add(wxMakeFormString("Button font", &buttonFont, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               200));
}

// Add class-specific items to dialog, since forms can't
// cope with everything.
void BuildPanelData::AddDialogItems(wxDialogBox *dialog)
{
}

Bool BuildPanelData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildSubwindowData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValue("fit_contents", &fitContents);
  expr->AssignAttributeValue("relative_layout", &relativeLayout);
  expr->AssignAttributeValue("label_position", &horizLabelPosition);
  return TRUE;
}

Bool BuildPanelData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildSubwindowData::WritePrologAttributes(expr, database);
  expr->AddAttributeValue("fit_contents", (long)fitContents);
  expr->AddAttributeValue("relative_layout", (long)relativeLayout);
  expr->AddAttributeValue("label_position", (long)horizLabelPosition);
  return TRUE;
}

Bool BuildPanelData::MakeRealWindow(void)
{
  if (!(buildParent && buildParent->userWindow)) return FALSE;

  if (hasBorder)
    windowStyle |= wxBORDER;
  else if (windowStyle & wxBORDER)
    windowStyle -= wxBORDER;

  UserPanel *panel = new UserPanel((wxFrame *)buildParent->userWindow, x, y,
                 width, height, windowStyle);
  userWindow = panel;
  panel->buildWindow = this;

  wxFont *bFont = NULL;
  wxFont *lFont = NULL;
  if (buttonFont && (bFont = FindFont(buttonFont)))
  {
    panel->SetButtonFont(bFont);
  }
  if (labelFont && (lFont = FindFont(labelFont)))
  {
    panel->SetLabelFont(lFont);
  }
  
  MakeRealWindowChildren();

  if (fitContents)
    panel->Fit();
  if (horizLabelPosition)
    panel->SetLabelPosition(wxHORIZONTAL);
  else
    panel->SetLabelPosition(wxVERTICAL);

  SetTestMode(buildApp.testMode);

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildPanelData::DestroyRealWindow(void)
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

void BuildPanelData::WriteClassDeclaration(ostream& stream)
{
  if (description && (strlen(description) > 0))
  {
    stream << "/*\n" << description << "\n*/\n";
  }

  stream << "class " << className << ": public wxPanel\n{\n";
  stream << " private:\n protected:\n public:\n";

  // Want to have as member variables all child subwindows
  if (!buildApp.useResourceMethod && (children.Number() > 0))
  {
    stream << "  // Panel items for reference within the program.\n";
    nameSpace.BeginBlock();
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();
//      if (child->memberName) delete[] child->memberName;
//      child->memberName = copystring(child->name);

      stream << "  " << child->className << " *" << child->name << ";\n";
      node = node->Next();
    }
    nameSpace.EndBlock();
    stream << "\n";
  }
  // Want to have as members, all bitmaps needed by buttons
  if (!buildApp.useResourceMethod && (children.Number() > 0))
  {
    stream << "  // Possible bitmaps for buttons.\n";
    nameSpace.BeginBlock();
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();
      if (child->windowType == wxTYPE_BUTTON)
      {
        BuildButtonData *button = (BuildButtonData *)child;
        if (button->bitmapData)
        {
          // Generate name for the bitmap
          char buf1[50];
          strcpy(buf1, button->bitmapData->GetBitmapName());
          strcat(buf1, "Bitmap");

          stream << "  wxBitmap *" << buf1 << ";\n";
        }
      }
      node = node->Next();
    }
    nameSpace.EndBlock();
    stream << "\n";
  }
  
  stream << "  // Constructor and destructor\n";
  stream << "  " << className ;
  if (buildApp.useResourceMethod)
    stream << "(void);\n";
  else
    stream << "(wxFrame *parent, int x, int y, int width, int height, long style, char *name);\n";
  stream << "  ~" << className << "(void);\n\n";
  stream << " void OnSize(int w, int h);\n";
  if (buildApp.useResourceMethod)
    stream << " void OnCommand(wxWindow& win, wxCommandEvent& event);\n";
  stream << "};\n\n";

  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();

    child->WriteClassDeclaration(stream);
    node = node->Next();
  }
}

// Generation
void BuildPanelData::WriteClassImplementation(ostream& stream)
{
  if (description && (strlen(description) > 0))
  {
    stream << "/*\n" << description << "\n*/\n";
  }

  stream << className << "::" << className ;
  if (buildApp.useResourceMethod)
  {
    stream << "(void)\n{\n";
  }
  else
  {
    stream << "(wxFrame *parent, int x, int y, int width, int height, long style, char *name):\n";
    stream << "  wxPanel(parent, x, y, width, height, style, name)\n{\n";
  }

  nameSpace.BeginBlock();

  // Set the default label orientation
  if (!horizLabelPosition)
  {
    stream << "  SetLabelPosition(wxVERTICAL);\n";
  }

  // Set button and label fonts if they exist
  wxFont *bFont = NULL;
  wxFont *lFont = NULL;
  if (buttonFont && (bFont = FindFont(buttonFont)))
  {
    stream << "  SetButtonFont(theApp." << buttonFont << ");\n";
  }
  if (labelFont && (lFont = FindFont(labelFont)))
  {
    stream << "  SetLabelFont(theApp." << labelFont << ");\n";
  }
  
  // Initialize any button bitmaps
  if (!buildApp.useResourceMethod && (children.Number() > 0))
  {
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();
      if (child->windowType == wxTYPE_BUTTON)
      {
        BuildButtonData *button = (BuildButtonData *)child;
        if (button->bitmapData)
        {
          // Generate name for the bitmap
          char buf1[100];
          char buf2[100];
          
          strcpy(buf1, button->bitmapData->GetBitmapName());
          strcat(buf1, "Bitmap");
          strcpy(buf2, button->bitmapData->GetBitmapName());
          strcat(buf2, ".xbm");

          stream << "#ifdef wx_x\n";
          stream << "#include \"" << buf2 << "\"\n";
          stream << "  " << buf1 << " = new wxBitmap(" << button->bitmapData->GetBitmapName() << ", ";
          stream << button->bitmapData->GetBitmapName() << "_width, ";
          stream << button->bitmapData->GetBitmapName() << "_height);\n";
          stream << "#endif\n";
          stream << "#ifdef wx_msw\n";
          stream << "  " << buf1 << " = new wxBitmap(\"" << button->bitmapData->GetBitmapName() << "\");\n";
          stream << "#endif\n";
        }
      }
      node = node->Next();
    }
  }

  // Create all child subwindows
  if (!buildApp.useResourceMethod && (children.Number() > 0))
  {
    stream << "  // Create panel items\n";
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();

      child->GenerateWindowInitialisationPre(stream, child->name, "  ");
      stream << "  " << child->name << " = ";
      child->GenerateConstructorCall(stream, "this");
      stream << ";\n";
      child->GenerateWindowInitialisationPost(stream, child->name, "  ");

      node = node->Next();
    }
  }

  if (fitContents)
  {
    stream << "  // Fit panel to contents\n";
    stream << "  Fit();\n\n";
  }

  nameSpace.EndBlock();
  stream << "}\n\n";

  // Destructor
  stream << className << "::~" << className << "(void)\n{\n";

  if (!buildApp.useResourceMethod)
  {
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();
      if (child->windowType == wxTYPE_BUTTON)
      {
        BuildButtonData *button = (BuildButtonData *)child;
        if (button->bitmapData)
        {
          // Generate name for the bitmap
          char buf1[50];
          strcpy(buf1, button->bitmapData->GetBitmapName());
          strcat(buf1, "Bitmap");
         stream << "  delete " << buf1 << ";\n";
        }
      }
      node = node->Next();
    }
  }

  stream << "\n}\n\n";

  stream << "void " << className << "::OnSize(int w, int h)\n{\n";
  stream << "  wxPanel::OnSize(w, h);\n";
  stream << "}\n\n";

  if (buildApp.useResourceMethod)
  {
    stream << "void " << className << "::OnCommand(wxWindow& win, wxCommandEvent& event)\n{\n";
    wxNode *node1 = children.First();
    while (node1)
    {
      BuildWindowData *child = (BuildWindowData *)node1->Data();
      if (node1 == children.First())
        stream << "  if (wxStringEq(win.GetName(), \"" << child->name << "\"))\n";
      else
        stream << "  else if (wxStringEq(win.GetName(), \"" << child->name << "\"))\n";
      stream << "  {\n";

      wxNode *node = child->buildActions.First();
      while (node)
      {
        BuildAction *action = (BuildAction *)node->Data();
        action->GenerateActionCPP(stream, "    ", child);
        node = node->Next();
      }
      node1 = node1->Next();
      stream << "  }\n";
    }

    stream << "}\n\n";
  }
  else
  {
    wxNode *node1 = children.First();
    while (node1)
    {
      BuildWindowData *child = (BuildWindowData *)node1->Data();

      child->WriteClassImplementation(stream);
      node1 = node1->Next();
    }
  }
}

void BuildPanelData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  if (buildApp.useResourceMethod)
  {
    stream << "new ";
    stream << className ;
  }
  else
  {
    stream << "new ";
    stream << className << "(" << parentName;
    stream << ", " << x << ", " << y << ", ";
    stream << width << ", " << height << ", ";

    char buf[400];
    GenerateWindowStyleString(buf);
    stream << buf << ", \"" << name << "\")";
  }
}

// Write .wxr resource information for this window
void BuildPanelData::GenerateWXResourceData(ostream& stream)
{
  stream << "static char *" << name << "_resource = \"dialog(name = '" << name << "',\\\n";
  char buf[300];
  GenerateWindowStyleString(buf);
  if (buf[0] == '0')
    buf[0] = 0;
  stream << "  style = '" << buf << "',\\\n";
  stream << "  title = " << SafeWord(title) << ",\\\n";
  if (windowType == wxTYPE_DIALOG_BOX)
  {
    stream << "  modal = " << ((BuildDialogBoxData *)this)->modal << ",\\\n";
  }
  if (buttonFont)
  {
    wxFont *theFont = FindFont(buttonFont);
    if (theFont)
    {
      int pointSize = theFont->GetPointSize();
      char *style = theFont->GetStyleString();
      char *weight = theFont->GetWeightString();
      char *family = theFont->GetFamilyString();
      Bool underlined = theFont->GetUnderlined();
      stream << "  button_font = [" << pointSize << ", '" << family << "', '" << style << "', '";
      stream << weight << "', " << (int)underlined << "],\\\n";
    }
  }
  if (labelFont)
  {
    wxFont *theFont = FindFont(labelFont);
    if (theFont)
    {
      int pointSize = theFont->GetPointSize();
      char *style = theFont->GetStyleString();
      char *weight = theFont->GetWeightString();
      char *family = theFont->GetFamilyString();
      Bool underlined = theFont->GetUnderlined();
      stream << "  label_font = [" << pointSize << ", '" << family << "', '" << style << "', '";
      stream << weight << "', " << (int)underlined << "],\\\n";
    }
  }

  stream << "  x = " << x << ", y = " << y << ", width = " << width << ", height = " << height;
  if (children.First())
    stream << ",\\\n";
  else
    stream << "\\\n";
    
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->GenerateWXResourceData(stream);
    if (node->Next())
      stream << ",\\\n";
    node = node->Next();
  }
  stream << ").\";\n\n";
}

// Write .wxr resource loading code
void BuildPanelData::GenerateResourceLoading(ostream& stream)
{
  stream << "  wxResourceParseData(" << name << "_resource);\n";
  BuildWindowData::GenerateResourceLoading(stream);
}

/*
 * Text window
 *
 */
 
BuildTextWindowData::BuildTextWindowData(BuildFrameData *theParent):
  BuildSubwindowData(theParent)
{
  x = 30;
  y = 30;
  width = 300;
  height = 300;
  name = copystring(GetNewObjectName("textwindow"));
//  memberName = copystring(name);
  className = copystring(GetNewObjectName("TextWindowClass"));
  title = copystring("untitled");
  windowStyle = wxBORDER | wxNATIVE_IMPL;
  windowType = wxTYPE_TEXT_WINDOW;

  defaultFile = NULL;
}

BuildTextWindowData::~BuildTextWindowData(void)
{
}

Bool BuildTextWindowData::EditAttributes(void)
{
  char nameBuf[200];
  sprintf(nameBuf, "Text Window Properties for %s", name);
  wxDialogBox *dialog = new wxDialogBox(NULL, nameBuf, TRUE, 10, 10);
  dialog->SetLabelFont(SmallLabelFont);
  dialog->SetButtonFont(SmallButtonFont);

  BuildForm *form = new BuildForm("Creating a text subwindow");
  AddFormItems(form);

  form->AssociatePanel(dialog);
  form->dialog = dialog;
  AddDialogItems(dialog);

  dialog->Fit();
  dialog->Centre(wxBOTH);

  form->RevertValues();

  dialog->Show(TRUE);
  if (strcmp(resizeModeString, "Proportional") == 0)
    resizeMode = RESIZE_PROPORTIONAL;
  else if (strcmp(resizeModeString, "Fixed") == 0)
    resizeMode = RESIZE_FIXED;
  else if (strcmp(resizeModeString, "Grow") == 0)
    resizeMode = RESIZE_GROW;

  BuildFrameData *frame = (BuildFrameData *)buildParent;
  frame->userWindow->OnSize(-1, -1);
  return TRUE;
}

// Add class-specific items to form
void BuildTextWindowData::AddFormItems(wxForm *form)
{
  BuildSubwindowData::AddFormItems(form);
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Text file", &defaultFile));
}

// Add class-specific items to dialog, since forms can't
// cope with everything.
void BuildTextWindowData::AddDialogItems(wxDialogBox *dialog)
{
}

Bool BuildTextWindowData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildSubwindowData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValue("file", &defaultFile);
  return TRUE;
}

Bool BuildTextWindowData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildSubwindowData::WritePrologAttributes(expr, database);
  expr->AddAttributeValueString("file", defaultFile);
  return TRUE;
}

Bool BuildTextWindowData::MakeRealWindow(void)
{
  if (!(buildParent && buildParent->userWindow)) return FALSE;

  windowStyle = 0;
  if (hasBorder)
    windowStyle |= wxBORDER;
  else if (windowStyle & wxBORDER)
    windowStyle -= wxBORDER;

  UserTextWindow *text = new UserTextWindow((wxFrame *)buildParent->userWindow, x, y,
                 width, height, windowStyle);
  userWindow = text;
  text->buildWindow = this;
  MakeRealWindowChildren();

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildTextWindowData::DestroyRealWindow(void)
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

void BuildTextWindowData::WriteClassDeclaration(ostream& stream)
{
  if (description && (strlen(description) > 0))
  {
    stream << "/*\n" << description << "\n*/\n";
  }

  stream << "class " << className << ": public wxTextWindow\n{\n";
  stream << " private:\n protected:\n public:\n";


  stream << "  // Constructor and destructor\n";
  stream << "  " << className ;
  stream << "(wxFrame *parent, int x, int y, int width, int height, long style, char *name);\n";
  stream << "  ~" << className << "(void);\n\n";
  stream << " void OnSize(int w, int h);\n";
  stream << "};\n\n";

  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();

    child->WriteClassDeclaration(stream);
    node = node->Next();
  }
}

// Generation
void BuildTextWindowData::WriteClassImplementation(ostream& stream)
{
  if (description && (strlen(description) > 0))
  {
    stream << "/*\n" << description << "\n*/\n";
  }

  stream << className << "::" << className ;
  stream << "(wxFrame *parent, int x, int y, int width, int height, long style, char *name):\n";
  stream << "  wxTextWindow(parent, x, y, width, height, style, name)\n{\n";

  if (defaultFile && strlen(defaultFile) > 0)
  {
    stream << "  // Load a default file\n";
    stream << "  LoadFile(" << "\"" << defaultFile << "\"" << ");\n\n";
  }

  stream << "}\n\n";

  stream << className << "::~" << className << "(void)\n{\n}\n\n";

  stream << "void " << className << "::OnSize(int w, int h)\n{\n";
  stream << "  wxTextWindow::OnSize(w, h);\n";
  stream << "}\n\n";

  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();

    child->WriteClassImplementation(stream);
    node = node->Next();
  }
}

void BuildTextWindowData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << className << "(" << parentName;
  stream << ", " << x << ", " << y << ", ";
  stream << width << ", " << height << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", \"" << name << "\")";
}

/*
 * Canvas
 *
 */
 
BuildCanvasData::BuildCanvasData(BuildFrameData *theParent):
  BuildSubwindowData(theParent)
{
  x = 30;
  y = 30;
  width = 300;
  height = 300;
  name = copystring(GetNewObjectName("canvas"));
  memberName = copystring(name);
  className = copystring(GetNewObjectName("CanvasClass"));
  title = copystring("untitled");
  windowStyle = wxBORDER;
  windowType = wxTYPE_CANVAS;
  bitmapData = NULL;

  // Scrollbar
  unitSizeX = 20;
  unitSizeY = 20;
  noUnitsX = 50;
  noUnitsY = 50;
  unitsPerPageX = 10;
  unitsPerPageY = 10;
}

BuildCanvasData::~BuildCanvasData(void)
{
/*
  if (simulationBitmap)
    delete simulationBitmap;
  if (simulationColourMap)
    delete simulationColourMap;
  if (simulationBitmapName)
    delete[] simulationBitmapName;
 */
 if (bitmapData)
   delete bitmapData;
}

Bool BuildCanvasData::EditAttributes(void)
{
  char nameBuf[200];
  sprintf(nameBuf, "Canvas Properties for %s", name);
  wxDialogBox *dialog = new wxDialogBox(NULL, nameBuf, TRUE, 10, 10);
  dialog->SetLabelFont(SmallLabelFont);
  dialog->SetButtonFont(SmallButtonFont);

  BuildForm *form = new BuildForm("Creating a canvas subwindow");
  AddFormItems(form);

  form->AssociatePanel(dialog);

  form->dialog = dialog;
  AddDialogItems(dialog);

  dialog->Fit();
  dialog->Centre(wxBOTH);

  form->RevertValues();

  dialog->Show(TRUE);
  if (strcmp(resizeModeString, "Proportional") == 0)
    resizeMode = RESIZE_PROPORTIONAL;
  else if (strcmp(resizeModeString, "Fixed") == 0)
    resizeMode = RESIZE_FIXED;
  else if (strcmp(resizeModeString, "Grow") == 0)
    resizeMode = RESIZE_GROW;

  BuildFrameData *frame = (BuildFrameData *)buildParent;
  frame->userWindow->OnSize(-1, -1);

  return TRUE;
}

// Add class-specific items to form
void BuildCanvasData::AddFormItems(wxForm *form)
{
  BuildSubwindowData::AddFormItems(form);
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormBool("Retained canvas", &isRetained));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormShort("Pixels/unit X", &unitSizeX, wxFORM_DEFAULT, NULL, NULL, NULL, 150));
  form->Add(wxMakeFormShort("Pixels/unit Y", &unitSizeY, wxFORM_DEFAULT, NULL, NULL, NULL, 150));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormShort("No units X", &noUnitsX, wxFORM_DEFAULT, NULL, NULL, NULL, 150));
  form->Add(wxMakeFormShort("No units Y", &noUnitsY, wxFORM_DEFAULT, NULL, NULL, NULL, 150));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormShort("Units/page X", &unitsPerPageX, wxFORM_DEFAULT, NULL, NULL, NULL, 150));
  form->Add(wxMakeFormShort("Units/page Y", &unitsPerPageY, wxFORM_DEFAULT, NULL, NULL, NULL, 150));
}

static wxButton *CanvasBitmapButton = NULL;

static void CanvasBitmapCheckBoxProc(wxCheckBox& check, wxCommandEvent& event)
{
  BuildCanvasData *data = (BuildCanvasData *)check.GetClientData();
  MakeModified();
  if (check.GetValue())
  {
    CanvasBitmapButton->Enable(TRUE);
    if (!data->bitmapData)
      data->bitmapData = new BuildBitmapData;
    data->bitmapData->Edit(check.GetParent());
    data->LoadCanvasBitmap();
  }
  else
  {
    data->ClearCanvasBitmap();
    CanvasBitmapButton->Enable(FALSE);
    if (data->bitmapData)
      delete data->bitmapData;
  }
}

static void CanvasBitmapButtonProc(wxButton& but, wxCommandEvent& event)
{
  BuildCanvasData *data = (BuildCanvasData *)but.GetClientData();
  MakeModified();
  wxBitmap *oldBitmap = data->bitmapData->GetBitmap();
  data->bitmapData->Edit(but.GetParent());
  data->bitmapData->SetBitmap(NULL);
  if (data->bitmapData->CreateBitmap())
  {
    data->LoadCanvasBitmap();
    delete oldBitmap;
  }
  else
  {
    data->bitmapData->SetBitmap(oldBitmap);
  }
}

// Add class-specific items to dialog, since forms can't
// cope with everything.
void BuildCanvasData::AddDialogItems(wxDialogBox *dialog)
{
  dialog->NewLine();
  wxCheckBox *checkBox = new wxCheckBox(dialog, (wxFunction)CanvasBitmapCheckBoxProc, "Use bitmap");
  checkBox->SetClientData((char *)this);
  CanvasBitmapButton = new wxButton(dialog, (wxFunction)CanvasBitmapButtonProc, "Edit bitmap properties");
  CanvasBitmapButton->SetClientData((char *)this);
  if (bitmapData)
  {
    checkBox->SetValue(TRUE);
  }
  else
  {
    checkBox->SetValue(FALSE);
    CanvasBitmapButton->Enable(FALSE);
  }
}

Bool BuildCanvasData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildSubwindowData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValue("unit_size_x", &unitSizeX);
  expr->AssignAttributeValue("unit_size_y", &unitSizeY);
  expr->AssignAttributeValue("no_units_x", &noUnitsX);
  expr->AssignAttributeValue("no_units_y", &noUnitsY);
  expr->AssignAttributeValue("unit_size_x", &unitSizeX);
  expr->AssignAttributeValue("units_page_x", &unitsPerPageX);
  expr->AssignAttributeValue("units_page_y", &unitsPerPageY);
//  expr->AssignAttributeValue("bitmap", &simulationBitmapName);
  PrologExpr *bitmapExpr = expr->AttributeValue("bitmap");
  
  if (bitmapExpr && (bitmapExpr->Type() == PrologList))
  {
    bitmapData = new BuildBitmapData;
    bitmapData->ReadPrologAttributes(expr);
  }
  else if (bitmapExpr)
  {
    // Backward compatibility: "bitmap" is a string, not a list.
    char *bitmapFilename = bitmapExpr->StringValue();
    if (bitmapFilename)
    {
      if (strlen(bitmapFilename) > 0)
      {
        bitmapData = new BuildBitmapData;
        char buf[400];
        strcpy(buf, FileNameFromPath(bitmapFilename));
        wxStripExtension(buf);
        bitmapData->SetBitmapName(buf);
      }
    }
  }
  return TRUE;
}

Bool BuildCanvasData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildSubwindowData::WritePrologAttributes(expr, database);
  expr->AddAttributeValue("unit_size_x", (long)unitSizeX);
  expr->AddAttributeValue("unit_size_y", (long)unitSizeY);
  expr->AddAttributeValue("no_units_x", (long)noUnitsX);
  expr->AddAttributeValue("no_units_y", (long)noUnitsY);
  expr->AddAttributeValue("unit_size_x", (long)unitSizeX);
  expr->AddAttributeValue("units_page_x", (long)unitsPerPageX);
  expr->AddAttributeValue("units_page_y", (long)unitsPerPageY);
//  expr->AddAttributeValueString("bitmap", simulationBitmapName);
  if (bitmapData)
    bitmapData->WritePrologAttributes(expr);

  return TRUE;
}

// Load bitmap into canvas
Bool BuildCanvasData::LoadCanvasBitmap(void)
{
  if (userWindow)
  {
    wxCanvas *canvas = (wxCanvas *)userWindow;
    wxDC *dc = canvas->GetDC();
    if (bitmapData && bitmapData->CreateBitmap())
    {
      userWindow->SetColourMap(bitmapData->GetBitmap()->GetColourMap());
        
      dc->Clear();
      canvas->OnPaint();
    }
    else
    {
      canvas->SetColourMap(NULL);
      dc->Clear();
    }
  }
  return TRUE;
}

// Clear colourmap
Bool BuildCanvasData::ClearCanvasBitmap(void)
{
  if (userWindow)
  {
    wxCanvas *canvas = (wxCanvas *)userWindow;
    wxDC *dc = canvas->GetDC();
    canvas->SetColourMap(NULL);
    dc->Clear();
  }
  return TRUE;
}

Bool BuildCanvasData::MakeRealWindow(void)
{
  if (!(buildParent && buildParent->userWindow)) return FALSE;

  windowStyle = 0;
  if (hasBorder)
    windowStyle |= wxBORDER;
  else if (windowStyle & wxBORDER)
    windowStyle -= wxBORDER;

  if (isRetained)
    windowStyle |= wxRETAINED;
  else if (windowStyle & wxRETAINED)
    windowStyle -= wxRETAINED;

  UserCanvas *canvas = new UserCanvas((wxFrame *)buildParent->userWindow, x, y,
                 width, height, windowStyle);
  userWindow = canvas;
  canvas->buildWindow = this;
  MakeRealWindowChildren();

  if ((unitSizeX > 0) && (unitSizeY > 0))
    canvas->SetScrollbars(unitSizeX, unitSizeY, noUnitsX, noUnitsY,
                          unitsPerPageX, unitsPerPageY);

  if (bitmapData)
    LoadCanvasBitmap();

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildCanvasData::DestroyRealWindow(void)
{
  if (userWindow)
  {
    if (bitmapData)
      ((wxCanvas *)userWindow)->SetColourMap(NULL);

    userWindow->GetPosition(&x, &y);
    userWindow->GetSize(&width, &height);
    delete userWindow;
    userWindow = NULL;
  }
  return TRUE;
}

void BuildCanvasData::WriteClassDeclaration(ostream& stream)
{
  if (description && (strlen(description) > 0))
  {
    stream << "/*\n" << description << "\n*/\n";
  }

  stream << "class " << className << ": public wxCanvas\n{\n";
  stream << " private:\n protected:\n public:\n";

  // Declare bitmap pointer if we're using a bitmap on the canvas
  if (bitmapData)
  {
    stream << "  wxBitmap *simulationBitmap;\n\n";
  }

  stream << "  // Constructor and destructor\n";
  stream << "  " << className ;
  stream << "(wxFrame *parent, int x, int y, int width, int height, long style, char *name);\n";
  stream << "  ~" << className << "(void);\n\n";
  stream << " void OnSize(int w, int h);\n";
  stream << " void OnPaint(void);\n";
  stream << " void OnEvent(wxMouseEvent& event);\n";
  stream << " void OnChar(wxKeyEvent& event);\n";
  stream << "};\n\n";

  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();

    child->WriteClassDeclaration(stream);
    node = node->Next();
  }
}

// Helper function
// Inserts the correct number of backslashes for DOS.
void WriteFileName(ostream& stream, char *filename)
{
  int len = strlen(filename);
  for (int i = 0; i < len; i++)
  {
    stream << filename[i];
#ifdef wx_msw
    if (filename[i] == '\\')
      stream << '\\';
#endif
  }
}

// Generation
void BuildCanvasData::WriteClassImplementation(ostream& stream)
{
  if (description && (strlen(description) > 0))
  {
    stream << "/*\n" << description << "\n*/\n";
  }

  stream << className << "::" << className ;
  stream << "(wxFrame *parent, int x, int y, int width, int height, long style, char *name):\n";
  stream << "  wxCanvas(parent, x, y, width, height, style, name)\n{\n";

  // Initialize bitmap to NULL
  if (bitmapData)
  {
    stream << "  simulationBitmap = NULL;\n\n";
  }

  if (unitSizeX > 0 && unitSizeY > 0 && noUnitsX > 0 && noUnitsY > 0)
  {
    stream << "  /* Set scrollbars to:\n";
    stream << "   *   " << unitSizeX << " pixels per horizontal units (1 unit = 1 scroll line)\n";
    stream << "   *   " << unitSizeY << " pixels per vertical units\n";
    stream << "   *   " << noUnitsX << " horizontal units\n";
    stream << "   *   " << noUnitsY << " vertical units\n";
    stream << "   *   " << unitsPerPageX << " units per horizontal page\n";
    stream << "   *   " << unitsPerPageY << " units per vertical page.\n";
    stream << "   * Virtual canvas is therefore " << unitSizeX*noUnitsX << " pixels by ";
    stream << unitSizeY*noUnitsY << " pixels.\n";
    stream << "   */\n";
    stream << "  SetScrollbars(" << unitSizeX << ", " << unitSizeY << ", ";
    stream << noUnitsX << ", " << noUnitsY << ", ";
    stream << unitsPerPageX << ", " << unitsPerPageY << ");\n";
  }

  // Load bitmap
  if (bitmapData)
  {
    if (buildApp.useResourceMethod)
    {
      stream << "  simulationBitmap = wxResourceCreateBitmap(\"" << bitmapData->GetBitmapName() << "_resource\");\n";
    }
    else
    {
      stream << "  simulationBitmap = new wxBitmap(\"";
      // Inserts the correct number of backslashes for DOS.
      WriteFileName(stream, bitmapData->GetBitmapName());
      stream << ".bmp\", wxBITMAP_TYPE_BMP);\n";
      stream << "  simulationBitmap->SetColourMap(simulationBitmap->GetColourMap());\n";
      stream << "  SetColourMap(simulationBitmap->GetColourMap());\n";
      stream << "  OnPaint();\n";
      stream << "\n";
    }
  }

  stream << "}\n\n";

  stream << className << "::~" << className << "(void)\n{\n";

  if (bitmapData)
  {
    stream << "  if (simulationBitmap)\n  {\n";
    stream << "    SetColourMap(NULL);\n";
    stream << "    delete simulationBitmap;\n  }\n";
  }

  stream << "}\n\n";

  stream << "// Called when canvas is resized.\n";
  stream << "void " << className << "::OnSize(int w, int h)\n{\n";
  stream << "  wxCanvas::OnSize(w, h);\n";
  stream << "}\n\n";

  stream << "// Called when canvas needs to be repainted.\n";
  stream << "void " << className << "::OnPaint(void)\n{\n";
  stream << "  // Speeds up drawing under Windows.\n";
  stream << "  GetDC()->BeginDrawing();\n\n";
  stream << "  // Insert your drawing code here.\n";
  if (bitmapData)
  {
    stream << "  if (simulationBitmap)\n  {\n";
    stream << "    // Draw the bitmap on the canvas\n";
    stream << "    wxCanvasDC *canvdc = GetDC();\n";
    stream << "    wxMemoryDC temp_dc(canvdc);\n";
    stream << "    temp_dc.SelectObject(simulationBitmap);\n";
    stream << "    canvdc->Blit(0.0,0.0,\n";
    stream << "      (float)simulationBitmap->GetWidth(),\n";
    stream << "      (float)simulationBitmap->GetHeight(),\n";
    stream << "      &temp_dc, 0.0, 0.0, wxCOPY);\n\n";
    stream << "  }\n";
  }
  stream << "\n";
  stream << "  GetDC()->EndDrawing();\n";
  stream << "}\n\n";

  stream << "// Called when the canvas receives a mouse event.\n";
  stream << "void " << className << "::OnEvent(wxMouseEvent& event)\n{\n";
  stream << "  wxCanvas::OnEvent(event);\n";
  stream << "}\n\n";

  stream << "// Called when the canvas receives a key event.\n";
  stream << "void " << className << "::OnChar(wxKeyEvent& event)\n{\n";
  stream << "  wxCanvas::OnChar(event);\n";
  stream << "}\n\n";

  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();

    child->WriteClassImplementation(stream);
    node = node->Next();
  }
}

void BuildCanvasData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << className << "(" << parentName;
  stream << ", " << x << ", " << y << ", ";
  stream << width << ", " << height << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", \"" << name << "\")";
}

// Generate bitmap, icon etc. resource entries for RC file.
void BuildCanvasData::GenerateResourceEntries(ostream &stream)
{
  if (bitmapData)
    bitmapData->GenerateResourceEntry(stream);
}

void BuildCanvasData::GenerateWXResourceBitmaps(ostream& stream)
{
  if (bitmapData)
    bitmapData->GenerateWXResourceBitmap(stream);
}

void BuildCanvasData::GenerateWXResourceBitmapRegistration(ostream& stream)
{
  if (bitmapData)
    bitmapData->GenerateWXResourceBitmapRegistration(stream);
}

void BuildCanvasData::GenerateResourceLoading(ostream &stream)
{
  if (bitmapData)
    bitmapData->GenerateResourceLoading(stream);
}

void BuildCanvasData::GenerateBitmapDataIncludes(ostream& stream)
{
  if (bitmapData)
    bitmapData->GenerateBitmapDataInclude(stream);
}

/*
 * USER WINDOWS
 *
 */

/*
 * User panel
 *
 */
 
void UserPanel::OnSize(int w, int h)
{
  wxPanel::OnSize(w, h);
}

void UserPanel::OnLeftClick(int x, int y, int keys)
{
      buildApp.UsePosition(TRUE, buildWindow, x, y);
      switch (EditorPaletteFrame->palette->currentlySelected)
      {
        case PALETTE_FRAME:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_FRAME);
          break;
        case PALETTE_DIALOG_BOX:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_DIALOG);
          break;
        case PALETTE_PANEL:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_PANEL);
          break;
        case PALETTE_CANVAS:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_CANVAS);
          break;
        case PALETTE_TEXT_WINDOW:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_TEXT_WINDOW);
          break;
        case PALETTE_BUTTON:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_BUTTON);
          break;
        case PALETTE_MESSAGE:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_MESSAGE);
          break;
        case PALETTE_TEXT:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_TEXT);
          break;
        case PALETTE_MULTITEXT:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_MULTITEXT);
          break;
        case PALETTE_CHOICE:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_CHOICE);
          break;
        case PALETTE_CHECKBOX:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_CHECKBOX);
          break;
        case PALETTE_RADIOBOX:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_RADIOBOX);
          break;
        case PALETTE_LISTBOX:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_LISTBOX);
          break;
        case PALETTE_SLIDER:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_SLIDER);
          break;
        case PALETTE_GAUGE:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_GAUGE);
          break;
        case PALETTE_GROUPBOX:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_GROUPBOX);
          break;
        default:
          break;
      }
      buildApp.UsePosition(FALSE);
}

void UserPanel::OnRightClick(int x, int y, int keys)
{
  buildApp.objectMenu->SetClientData((char *)buildWindow);
  PopupMenu(buildApp.objectMenu, (float)x, (float)y);
}

void UserPanel::OnItemLeftClick(wxItem *item, int x, int y, int keys)
{
  BuildWindowData *child = buildWindow->FindChildWindow(item);
  if (keys==2)
  {
		SetAction((UserButton *)item);
		return;
  }
  if(keys==0)
  {
    DoUnselectAll(item,(buildWindow->children).First());
    GetPanelDC()->Clear();
    Refresh();
  }
  if (item->IsSelected())
  {
    BuildSelections.DeleteObject(child);
    item->SelectItem(FALSE);
    GetPanelDC()->Clear();
    Refresh();
  }
  else 
  {
    BuildSelections.Append(child);
    item->SelectItem(TRUE);

    GetPanelDC()->BeginDrawing();
    item->DrawSelectionHandles(GetPanelDC());
    GetPanelDC()->EndDrawing();
  }
}

void UserPanel::OnItemRightClick(wxItem *item, int x, int y, int keys)
{
  BuildWindowData *child = buildWindow->FindChildWindow(item);
  if (child)
  {
    buildApp.objectMenu->SetClientData((char *)child);
    PopupMenu(buildApp.objectMenu, (float)x, (float)y);
  }
}

void UserPanel::OnItemMove(wxItem *item, int x, int y)
{
  BuildWindowData *child = buildWindow->FindChildWindow(item);
  if (child)
  {
    child->x = x;
    child->y = y;
  }
}

void UserPanel::OnItemSize(wxItem *item, int w, int h)
{
  BuildWindowData *child = buildWindow->FindChildWindow(item);
  if (child)
  {
    child->width = w;
    child->width = h;
  }
}

/*
 * User Text subwindow
 *
 */
 
void UserTextWindow::OnSize(int w, int h)
{
  wxTextWindow::OnSize(w, h);
}

/*
 * User canvas
 *
 */
 
void UserCanvas::OnSize(int w, int h)
{
  wxCanvas::OnSize(w, h);
}

void UserCanvas::OnChar(wxKeyEvent& event)
{
  wxCanvas::OnChar(event);
}

void UserCanvas::OnEvent(wxMouseEvent& event)
{
  wxCanvas::OnEvent(event);
}

void UserCanvas::OnPaint(void)
{
  if (buildWindow->bitmapData && buildWindow->bitmapData->GetBitmap())
  {
    wxBitmap *bitmap = buildWindow->bitmapData->GetBitmap();
    wxColourMap *colourmap = bitmap->GetColourMap();
    
    // Draw the bitmap on the canvas
    wxCanvasDC *canvdc = GetDC();
    wxMemoryDC temp_dc(canvdc);
    temp_dc.SelectObject(bitmap);
    temp_dc.SetColourMap(colourmap);
    canvdc->Blit(0.0,0.0,
      (float)bitmap->GetWidth(),
      (float)bitmap->GetHeight(),
      &temp_dc, 0.0, 0.0, wxCOPY);
  }
}


/*
 * Dialog boxes
 *
 */
 
BuildDialogBoxData::BuildDialogBoxData(BuildFrameData *theParent):
  BuildPanelData(theParent)
{
  x = 30;
  y = 30;
  width = 300;
  height = 300;
  name = copystring(GetNewObjectName("dialogbox"));
  memberName = copystring(name);
  className = copystring(GetNewObjectName("DialogBoxClass"));
  title = copystring("untitled");
  windowStyle = wxDEFAULT_DIALOG_STYLE;
  windowType = wxTYPE_DIALOG_BOX;

  modal = FALSE;
}

BuildDialogBoxData::~BuildDialogBoxData(void)
{
}

Bool BuildDialogBoxData::EditAttributes(void)
{
  char nameBuf[200];
  sprintf(nameBuf, "Dialog Box Properties for %s", name);
  wxDialogBox *dialog = new wxDialogBox(NULL, nameBuf, TRUE, 10, 10);
  dialog->SetLabelFont(SmallLabelFont);
  dialog->SetButtonFont(SmallButtonFont);

  BuildForm *form = new BuildForm("Creating a dialog box");
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

// Add class-specific items to form
void BuildDialogBoxData::AddFormItems(wxForm *form)
{
  BuildPanelData::AddFormItems(form);
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormBool("Modal", &modal));
}

// Add class-specific items to dialog, since forms can't
// cope with everything.
void BuildDialogBoxData::AddDialogItems(wxDialogBox *dialog)
{
}

Bool BuildDialogBoxData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildPanelData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValue("modal", &modal);
  return TRUE;
}

Bool BuildDialogBoxData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildPanelData::WritePrologAttributes(expr, database);
  expr->AddAttributeValue("modal", (long)modal);
  return TRUE;
}

Bool BuildDialogBoxData::MakeRealWindow(void)
{
  UserDialogBox *dialog = new UserDialogBox(NULL, title, FALSE, x, y,
                 width, height, windowStyle);
  userWindow = dialog;
  dialog->buildWindow = this;

  if (horizLabelPosition)
    dialog->SetLabelPosition(wxHORIZONTAL);
  else
    dialog->SetLabelPosition(wxVERTICAL);

  wxFont *bFont = NULL;
  wxFont *lFont = NULL;
  if (buttonFont && (bFont = FindFont(buttonFont)))
  {
    dialog->SetButtonFont(bFont);
  }
  if (labelFont && (lFont = FindFont(labelFont)))
  {
    dialog->SetLabelFont(lFont);
  }

  MakeRealWindowChildren();

  if (fitContents && (children.Number() > 0))
    dialog->Fit();

  SetTestMode(buildApp.testMode);

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildDialogBoxData::DestroyRealWindow(void)
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

void BuildDialogBoxData::WriteClassDeclaration(ostream& stream)
{
  if (description && (strlen(description) > 0))
  {
    stream << "/*\n" << description << "\n*/\n";
  }

  stream << "class " << className << ": public wxDialogBox\n{\n";
  stream << " private:\n protected:\n public:\n";

  // Want to have as member variables all child items
  if (!buildApp.useResourceMethod && (children.Number() > 0))
  {
    stream << "  // Panel items for reference within the program.\n";
    nameSpace.BeginBlock();
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();
//      if (child->name) delete[] child->name;
//      child->name = copystring(child->name);

      stream << "  " << child->className << " *" << child->name << ";\n";
      node = node->Next();
    }
    nameSpace.EndBlock();
    stream << "\n";
  }

  // Want to have as members, all bitmaps needed by buttons
  if (!buildApp.useResourceMethod && (children.Number() > 0))
  {
    stream << "  // Possible bitmaps for buttons.\n";
    nameSpace.BeginBlock();
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();
      if (child->windowType == wxTYPE_BUTTON)
      {
        BuildButtonData *button = (BuildButtonData *)child;
        if (button->bitmapData)
        {
          // Generate name for the bitmap
          char buf1[50];
          strcpy(buf1, button->bitmapData->GetBitmapName());
          strcat(buf1, "Bitmap");

          stream << "  wxBitmap *" << buf1 << ";\n";
        }
      }
      node = node->Next();
    }
    nameSpace.EndBlock();
    stream << "\n";
  }

  stream << "  // Constructor and destructor\n";
  stream << "  " << className ;
  if (buildApp.useResourceMethod)
    stream << "(void);\n";
  else
    stream << "(wxFrame *parent, char *title, Bool modal, int x, int y, int width, int height, long style, char *name);\n";
  stream << "  ~" << className << "(void);\n\n";
  stream << " void OnSize(int w, int h);\n";
  if (buildApp.useResourceMethod)
    stream << " void OnCommand(wxWindow& win, wxCommandEvent& event);\n";
  stream << "};\n\n";

  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();

    child->WriteClassDeclaration(stream);
    node = node->Next();
  }
}

// Generation
void BuildDialogBoxData::WriteClassImplementation(ostream& stream)
{
  if (description && (strlen(description) > 0))
  {
    stream << "/*\n" << description << "\n*/\n";
  }

  stream << className << "::" << className ;
  if (buildApp.useResourceMethod)
    stream << "(void)\n{\n";
  else
  {
    stream << "(wxFrame *parent, char *title, Bool modal, int x, int y, int width, int height, long style, char *name):\n";
    stream << "  wxDialogBox(parent, title, modal, x, y, width, height, style, name)\n{\n";
  }

  nameSpace.BeginBlock();

  // Set the default label orientation
  if (!horizLabelPosition)
  {
    stream << "  SetLabelPosition(wxVERTICAL);\n";
  }

  // Set button and label fonts if they exist
  wxFont *bFont = NULL;
  wxFont *lFont = NULL;
  if (buttonFont && (bFont = FindFont(buttonFont)))
  {
    stream << "  SetButtonFont(theApp." << buttonFont << ");\n";
  }
  if (labelFont && (lFont = FindFont(labelFont)))
  {
    stream << "  SetLabelFont(theApp." << labelFont << ");\n";
  }
  
  // Initialize any button bitmaps
  if (!buildApp.useResourceMethod && (children.Number() > 0))
  {
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();
      if (child->windowType == wxTYPE_BUTTON)
      {
        BuildButtonData *button = (BuildButtonData *)child;
        if (button->bitmapData)
        {
          // Generate name for the bitmap
          char buf1[100];
          char buf2[100];
          
          strcpy(buf1, button->bitmapData->GetBitmapName());
          strcat(buf1, "Bitmap");
          strcpy(buf2, button->bitmapData->GetBitmapName());
          strcat(buf2, ".xbm");

          stream << "#ifdef wx_x\n";
          stream << "#include \"" << buf2 << "\"\n";
          stream << "  " << buf1 << " = new wxBitmap(" << button->bitmapData->GetBitmapName() << ", " << button->bitmapData->GetBitmapName() << "_width, ";
          stream << button->bitmapData->GetBitmapName() << "_height);\n";
          stream << "#endif\n";
          stream << "#ifdef wx_msw\n";
          stream << "  " << buf1 << " = new wxBitmap(\"" << button->bitmapData->GetBitmapName() << "\");\n";
          stream << "#endif\n";
        }
      }
      node = node->Next();
    }
  }
  // Create all child subwindows
  if (!buildApp.useResourceMethod && (children.Number() > 0))
  {
    stream << "  // Create panel items\n";
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();

      child->GenerateWindowInitialisationPre(stream, child->name, "  ");

      stream << "  " << child->name << " = ";
      child->GenerateConstructorCall(stream, "this");
      stream << ";\n";

      child->GenerateWindowInitialisationPost(stream, child->name, "  ");

      node = node->Next();
    }
    stream << "\n";
  }

  if (fitContents)
  {
    stream << "  // Fit panel to contents\n";
    stream << "  Fit();\n\n";
  }

  nameSpace.EndBlock();
  stream << "\n}\n\n";

  // Destructor
  stream << className << "::~" << className << "(void)\n{\n";

  // Generate 'delete's for any bitmap buttons
  if (!buildApp.useResourceMethod)
  {
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();
      if (child->windowType == wxTYPE_BUTTON)
      {
        BuildButtonData *button = (BuildButtonData *)child;
        if (button->bitmapData)
        {
          // Generate name for the bitmap
          char buf1[50];
          strcpy(buf1, button->bitmapData->GetBitmapName());
          strcat(buf1, "Bitmap");
         stream << "  delete " << buf1 << ";\n";
        }
      }
      node = node->Next();
    }
  }

  stream << "\n}\n\n";

  stream << "void " << className << "::OnSize(int w, int h)\n{\n";
  stream << "  wxDialogBox::OnSize(w, h);\n";
  stream << "}\n\n";

  if (buildApp.useResourceMethod)
  {
    stream << "void " << className << "::OnCommand(wxWindow& win, wxCommandEvent& event)\n{\n";
    stream << "}\n\n";
  }

  wxNode *node1 = children.First();
  while (node1)
  {
    BuildWindowData *child = (BuildWindowData *)node1->Data();

    child->WriteClassImplementation(stream);
    node1 = node1->Next();
  }
}

void BuildDialogBoxData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  if (buildApp.useResourceMethod)
  {
    stream << "new ";
    stream << className ;
  }
  else
  {
    stream << "new ";
    stream << className << "(" << parentName << ", " << SafeString(title);
    stream << ", " << (modal ? "TRUE" : "FALSE") << ", " << x << ", " << y << ", ";
    stream << width << ", " << height << ", ";

    char buf[400];
    GenerateWindowStyleString(buf);
    stream << buf << ", \"" << name << "\")";
  }
}


void BuildDialogBoxData::FindScreenPosition(int *sx, int *sy)
{
  if (!userWindow)
  {
    *sx = x; *sy = y; return;
  }

  userWindow->GetPosition(&x, &y); // x and y are member variables  
  *sx = x;
  *sy = y;
}

void BuildDialogBoxData::FindClientPosition(int sx, int sy, int *cx, int *cy)
{
/*
  *cx = x; *cy = y;
  x = *cx; y = *cy;
*/
  *cx = sx;
  *cy = sy;
  x = sx; y = sy;
}

UserDialogBox::~UserDialogBox(void)
{
  if (!Iconized())
    GetPosition(&buildWindow->x, &buildWindow->y);
}

void UserDialogBox::OnSize(int w, int h)
{
  wxDialogBox::OnSize(w, h);
}

void UserDialogBox::OnLeftClick(int x, int y, int keys)
{
      buildApp.UsePosition(TRUE, buildWindow, x, y);
      switch (EditorPaletteFrame->palette->currentlySelected)
      {
        case PALETTE_FRAME:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_FRAME);
          break;
        case PALETTE_DIALOG_BOX:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_DIALOG);
          break;
        case PALETTE_PANEL:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_PANEL);
          break;
        case PALETTE_CANVAS:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_CANVAS);
          break;
        case PALETTE_TEXT_WINDOW:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_TEXT_WINDOW);
          break;
        case PALETTE_BUTTON:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_BUTTON);
          break;
        case PALETTE_MESSAGE:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_MESSAGE);
          break;
        case PALETTE_TEXT:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_TEXT);
          break;
        case PALETTE_MULTITEXT:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_MULTITEXT);
          break;
        case PALETTE_CHOICE:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_CHOICE);
          break;
        case PALETTE_CHECKBOX:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_CHECKBOX);
          break;
        case PALETTE_RADIOBOX:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_RADIOBOX);
          break;
        case PALETTE_LISTBOX:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_LISTBOX);
          break;
        case PALETTE_SLIDER:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_SLIDER);
          break;
        case PALETTE_GAUGE:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_GAUGE);
          break;
        case PALETTE_GROUPBOX:
          MainFrame->OnMenuCommand(OBJECT_EDITOR_NEW_GROUPBOX);
          break;
        default:
          break;
      }
      buildApp.UsePosition(FALSE);
}

void UserDialogBox::OnRightClick(int x, int y, int keys)
{
  buildApp.objectMenu->SetClientData((char *)buildWindow);
  PopupMenu(buildApp.objectMenu, (float)x, (float)y);
}

void UserDialogBox::OnItemLeftClick(wxItem *item, int x, int y, int keys)
{
  BuildWindowData *child = buildWindow->FindChildWindow(item);
  if (keys==2)
  {
		SetAction((UserButton *)item);
		return;
  }
  if(keys==0)
  {
    DoUnselectAll(item,(buildWindow->children).First());
    GetPanelDC()->Clear();
    Refresh();
  }
  if (item->IsSelected())
  {
    BuildSelections.DeleteObject(child);
    item->SelectItem(FALSE);
    GetPanelDC()->Clear();
    Refresh();
  }
  else
  {
    BuildSelections.Append(child);
    item->SelectItem(TRUE);

    GetPanelDC()->BeginDrawing();
    item->DrawSelectionHandles(GetPanelDC());
    GetPanelDC()->EndDrawing();
  }
}

void UserDialogBox::OnItemRightClick(wxItem *item, int x, int y, int keys)
{
  BuildWindowData *child = buildWindow->FindChildWindow(item);
  if (child)
  {
    buildApp.objectMenu->SetClientData((char *)child);
    PopupMenu(buildApp.objectMenu, (float)x, (float)y);
  }
}

void UserDialogBox::OnItemMove(wxItem *item, int x, int y)
{
  BuildWindowData *child = buildWindow->FindChildWindow(item);
  if (child)
  {
    child->x = x;
    child->y = y;
  }
}

void UserDialogBox::OnItemSize(wxItem *item, int w, int h)
{
  BuildWindowData *child = buildWindow->FindChildWindow(item);
  if (child)
  {
    child->width = w;
    child->height = h;
  }
}
void SetAction(UserButton *but)
{
  if (but->IsKindOf(CLASSINFO(wxButton)) && but->IsKindOf(CLASSINFO(wxCheckBox)))
		return;
  wxNode *node = but->buildWindow->buildActions.First();
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
        but->buildWindow->AddAction(action);
    }
    else
    {
      but->buildWindow->DeleteAction(action);
    }
  }
}
 
void DoUnselectAll(wxItem* item,wxNode *node)
{
			wxItem *anItem;
  		while (node)
 		  { 
    		BuildWindowData *child = (BuildWindowData *)node->Data();
    		anItem=(wxItem *)child->userWindow;
        if(anItem !=NULL)
    		if (anItem->IsSelected()&&anItem !=item)
    		{
						BuildSelections.DeleteObject(child);
    				anItem->SelectItem(FALSE);
				}
    	  node = node->Next();
  	}

}
