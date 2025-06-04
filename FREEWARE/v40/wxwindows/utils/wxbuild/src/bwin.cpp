/*
 * File:	bwin.cc
 * Purpose:	wxWindows GUI builder -- base window stuff
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
#include <ctype.h>
#include <stdlib.h>

#include "wxbuild.h"
#include "namegen.h"
#include "bapp.h"
#include "bwin.h"
#include "bframe.h"
#include "btoolbar.h"
#include "bactions.h"

BuildWindowData *NewObject = NULL;

/*
 * BuildWindow class for storing window information (size, position, colour etc.).
 * This is for all derivatives from wxWindow, and probably more besides.
 *
 */

BuildWindowData::BuildWindowData(BuildWindowData *theParent)
{
  userWindow = NULL;
  buildParent = theParent;
  name = NULL;
  description = NULL;
  className = NULL;
  memberName = NULL;
  title = NULL;
  helpString = NULL;
  intValue = 0;
  stringValue = NULL;
  windowStyle = 0;
  x = 0;
  y = 0;
  width = 0;
  height = 0;
  windowFont = NULL;
  windowColour = NULL;
  windowType = 0;
  dontResize = FALSE;
  id = NewId();

  if (theParent)
    theParent->children.Append(this);
}

BuildWindowData::~BuildWindowData(void)
{
  if (name) delete[] name;
  if (description) delete[] description;
  if (className) delete[] className;
  if (memberName) delete[] memberName; 
  if (title) delete[] title;
  if (helpString) delete[] helpString;

  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    delete child;
    node = children.First();
  }
  if (buildParent)
    buildParent->children.DeleteObject(this);

  node = buildActions.First();
  while (node)
  {
    BuildAction *action = (BuildAction *)node->Data();
    wxNode *next = node->Next();
    DeleteAction(action);
    node = next;
  }

  // Remove dangling pointers -- windows that rely on this window
  // for an action.
  node = actionWindows.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    // Have to delete the actions that refer to this window.
    wxNode *node1 = child->buildActions.First();
    while (node1)
    {
      BuildAction *action = (BuildAction *)node1->Data();
      wxNode *next1 = node1->Next();
      if (action->actionWindow == this)
        child->DeleteAction(action);
      node1 = next1;
    }
      
    node = node->Next();
  }
}

void BuildWindowData::MakeRealWindowChildren(void)
{
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->MakeRealWindow();
    node = node->Next();
  }
}

 // Set userWindow to NULL in all children, since
// we're deleting this real window.
void BuildWindowData::NullUserWindows(void)
{
  userWindow = NULL;
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->NullUserWindows();
    node = node->Next();
  }
}

void BuildWindowData::AddAction(BuildAction *action)
{
  buildActions.Append(action);
  if (action->actionWindow)
    action->actionWindow->actionWindows.Append(this);
}

void BuildWindowData::DeleteAction(BuildAction *action)
{
  buildActions.DeleteObject(action);
  if (action->actionWindow)
    action->actionWindow->actionWindows.DeleteObject(this);
  delete action;
}

// Find an action for this window, for a specific event.
// There may be zero or more actions for each type of event relevant
// to this window. This function is only relevant when there
// is only one action per event (e.g. a button, as opposed to a frame
// where there may be many command actions for the menu command event).
BuildAction *BuildWindowData::FindAction(WXTYPE eventType)
{
  wxNode *node = buildActions.First();
  while (node)
  {
    BuildAction *action = (BuildAction *)node->Data();
    if (action->eventType == eventType)
      return action;
    node = node->Next();
  }
  return NULL;
}

/*
Bool BuildWindowData::EditAttributes(void)
{
  return TRUE;
}
*/

Bool BuildWindowData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  expr->AssignAttributeValue("id", &id);
  RegisterId(id);
  
  expr->AssignAttributeValue("name", &name);
  expr->AssignAttributeValue("description", &description);
  expr->AssignAttributeValue("class", &className);
  if (title)
    delete[] title;
  title = NULL;
  expr->AssignAttributeValue("title", &title);
  expr->AssignAttributeValue("help_string", &helpString);

  expr->AssignAttributeValue("window_style", &windowStyle);
  expr->AssignAttributeValue("x", &x);
  expr->AssignAttributeValue("y", &y);
  expr->AssignAttributeValue("width", &width);
  expr->AssignAttributeValue("height", &height);
  return TRUE;
}

Bool BuildWindowData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  // Get the window position in case it's changed without being saved
  if (userWindow)
    userWindow->GetPosition(&x, &y);
    
  expr->AddAttributeValue("id", id);
  expr->AddAttributeValue("type", (long)windowType);
  if (buildParent)
    expr->AddAttributeValue("parent", buildParent->id);
  if (name)
    expr->AddAttributeValueString("name", name);
  if (description)
    expr->AddAttributeValueString("description", description);
  if (className)
    expr->AddAttributeValueString("class", className);
  if (title)
    expr->AddAttributeValueString("title", title);
  if (helpString)
    expr->AddAttributeValueString("help_string", helpString);

  expr->AddAttributeValue("window_style", (long)windowStyle);
  expr->AddAttributeValue("x", (long)x);
  expr->AddAttributeValue("y", (long)y);
  expr->AddAttributeValue("width", (long)width);
  expr->AddAttributeValue("height", (long)height);

  return TRUE;
}

Bool BuildWindowData::WriteRecursively(PrologDatabase *database)
{
  PrologExpr *expr = new PrologExpr("window");
  WritePrologAttributes(expr, database);
  WriteActions(expr, database);
  database->Append(expr);
  
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->WriteRecursively(database);
    node = node->Next();
  }
  return TRUE;
}

void BuildWindowData::WriteClassImplementation(ostream& stream)
{
}

void BuildWindowData::WriteClassDeclaration(ostream& stream)
{
}

/*
 * Default action reading/writing
 *
 */

Bool BuildWindowData::WriteActions(PrologExpr *expr, PrologDatabase *database)
{
  char buf[100];
  int i = 1;
  wxNode *node = buildActions.First();
  while (node)
  {
    BuildAction *action = (BuildAction *)node->Data();
    sprintf(buf, "action%d", i);
    PrologExpr *listExpr = new PrologExpr(PrologList);
    action->WritePrologAttributes(listExpr, database);
    expr->AddAttributeValue(buf, listExpr);
    i ++;
    node = node->Next();
  }
  return TRUE;
}

Bool BuildWindowData::ReadActions(PrologExpr *expr, PrologDatabase *database)
{
  char buf[100];
  int i = 1;
  sprintf(buf, "action%d", i);
  PrologExpr *actionExpr = NULL;
  while (actionExpr = expr->AttributeValue(buf))
  {
    BuildAction *action = new BuildAction;
    action->ReadPrologAttributes(actionExpr, database);
    buildActions.Append(action);
    i ++;
    sprintf(buf, "action%d", i);
  }
  return TRUE;
}

void BuildWindowData::GenerateWXResourceBitmaps(ostream& stream)
{
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->GenerateWXResourceBitmaps(stream);
    node = node->Next();
  }
}

void BuildWindowData::GenerateWXResourceBitmapRegistration(ostream& stream)
{
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->GenerateWXResourceBitmapRegistration(stream);
    node = node->Next();
  }
}

void BuildWindowData::GenerateWXResourceData(ostream& stream)
{
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->GenerateWXResourceData(stream);
    node = node->Next();
  }
}

void BuildWindowData::GenerateDefines(ostream& stream)
{
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->GenerateDefines(stream);
    node = node->Next();
  }
}

void BuildWindowData::GenerateResourceLoading(ostream& stream)
{
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->GenerateResourceLoading(stream);
    node = node->Next();
  }
}

void BuildWindowData::GenerateBitmapDataIncludes(ostream& stream)
{
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->GenerateBitmapDataIncludes(stream);
    node = node->Next();
  }
}

// Set test mode for actual windows, recursively
void BuildWindowData::SetTestMode(Bool testMode)
{
  if (userWindow)
  {
    userWindow->SetUserEditMode(!testMode);
    if (userWindow->IsKindOf(CLASSINFO(wxPanel)))
    {
/*
      if (testMode)
        userWindow->SetCursor(wxSTANDARD_CURSOR);
      else
        userWindow->SetCursor(crossCursor);
*/
    }
  }
    
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->SetTestMode(testMode);
    node = node->Next();
  }
}

// Find a BuildWindowData child given the real window.
BuildWindowData *BuildWindowData::FindChildWindow(wxWindow *win)
{
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    if (child->userWindow == win)
      return child;
      
    node = node->Next();
  }
  return NULL;
}

void BuildWindowData::GenerateWindowStyleString(char *buf)
{
  GenerateStyle(buf, wxUSER_COLOURS, "wxUSER_COLOURS");
  GenerateStyle(buf, wxVERTICAL_LABEL, "wxVERTICAL_LABEL");
  GenerateStyle(buf, wxHORIZONTAL_LABEL, "wxHORIZONTAL_LABEL");
  GenerateStyle(buf, wxVSCROLL, "wxVSCROLL");
  GenerateStyle(buf, wxHSCROLL, "wxHSCROLL");
  GenerateStyle(buf, wxBORDER, "wxBORDER");
  GenerateStyle(buf, wxNATIVE_IMPL, "wxNATIVE_IMPL");
}

Bool BuildWindowData::GenerateStyle(char *buf, long flag, char *strStyle)
{
  if ((windowStyle & flag) == flag)
  {
    if (strlen(buf) > 0)
      strcat(buf, " | ");
    strcat(buf, strStyle);
    return TRUE;
  }
  else
    return FALSE;
}

