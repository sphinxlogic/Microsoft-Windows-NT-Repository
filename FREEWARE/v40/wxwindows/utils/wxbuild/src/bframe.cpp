/*
 * File:	bframe.cc
 * Purpose:	wxWindows GUI builder
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
#include "bmenu.h"

#ifdef wx_msw
// Windows icon/cursor reading code
#include "wxcurico.h"
#endif

#ifdef wx_x
#define DEFAULT_EDITOR "emacs"
#endif
#ifdef wx_msw
#define DEFAULT_EDITOR "notepad"
#endif

extern wxStringList MenuStringList;

Bool IsSubwindow(int typ)
{
/*
  return ((wxSubType(typ, wxTYPE_PANEL) ||
           wxSubType(typ, wxTYPE_TEXT_WINDOW) ||
           wxSubType(typ, wxTYPE_CANVAS)) &&
          !wxSubType(typ, wxTYPE_TOOLBAR));
*/
  return (((typ == wxTYPE_PANEL) || (typ == wxTYPE_TEXT_WINDOW) || (typ == wxTYPE_CANVAS)) && (typ != wxTYPE_TOOLBAR));
}

BuildFrameData::BuildFrameData(BuildFrameData *theParent):
  BuildWindowData(theParent)
{
  x = 30;
  y = 30;
  width = 300;
  height = 300;
  name = copystring(GetNewObjectName("frame"));
//  memberName = copystring(name);
  className = copystring(GetNewObjectName("FrameClass"));
  title = copystring("untitled");
  windowStyle = wxSDI | wxDEFAULT_FRAME;
  windowType = wxTYPE_FRAME;
  buildMenuBar = new BuildMenuItem;
  buildMenuBar->menuType = BUILD_MENU_TYPE_MENU_BAR;
  buildMenuBar->name = copystring(GetNewObjectName("menuBar"));

  noStatusLineFields = 0;
  tilingMode = TRUE; // TRUE means vertical, FALSE is horizontal
//  iconName = NULL;
  toolbar = NULL;
  iconData = new BuildBitmapData(TRUE);
  iconData->SetBitmapName("");

  thickFrame = TRUE;
  minBox = TRUE;
  maxBox = TRUE;
  hasCaption = TRUE;
  systemMenu = TRUE;
}

BuildFrameData::~BuildFrameData(void)
{
  if (buildMenuBar) delete buildMenuBar;
  if (toolbar) delete toolbar;
  if (iconData)
    delete iconData;
}

Bool BuildFrameData::EditAttributes(void)
{
  char nameBuf[200];
  sprintf(nameBuf, "Frame Properties for %s", name);
  wxDialogBox *dialog = new wxDialogBox(NULL, nameBuf, TRUE, 10, 10);
  dialog->SetLabelFont(SmallLabelFont);
  dialog->SetButtonFont(SmallButtonFont);

  BuildForm *form = new BuildForm("Creating a frame");
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

Bool BuildFrameData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildWindowData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValue("menu_bar", &menuBarId);

  expr->AssignAttributeValue("no_status_fields", &noStatusLineFields);
  expr->AssignAttributeValue("tiling_mode", &tilingMode);
  
  if (expr->AttributeValue("icon"))
  {
    if (!iconData) iconData = new BuildBitmapData(TRUE);
    iconData->ReadPrologAttributes(expr);
  }
  else
  {
    // Backward compatibility
    char *iconName = NULL;
    expr->AssignAttributeValue("icon_name", &iconName);
    if (iconName)
    {
      if (!iconData) iconData = new BuildBitmapData(TRUE);
      iconData->SetBitmapName(iconName);
      delete[] iconName;
    }
  }

  thickFrame = ((windowStyle & wxTHICK_FRAME) == wxTHICK_FRAME);
  hasCaption = ((windowStyle & wxCAPTION) == wxCAPTION);
  minBox = ((windowStyle & wxMINIMIZE_BOX) == wxMINIMIZE_BOX);
  maxBox = ((windowStyle & wxMAXIMIZE_BOX) == wxMAXIMIZE_BOX);
  systemMenu = ((windowStyle & wxSYSTEM_MENU) == wxSYSTEM_MENU);
  
  return TRUE;
}

Bool BuildFrameData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  if (buildMenuBar)
    buildMenuBar->WriteMenu(database);

  if (toolbar)
  {
    PrologExpr *toolbarClause = new PrologExpr("window");
    toolbar->WritePrologAttributes(toolbarClause, database);
    database->Append(toolbarClause);
  }
    
  BuildWindowData::WritePrologAttributes(expr, database);

  if (buildMenuBar)
    expr->AddAttributeValue("menu_bar", buildMenuBar->id);

  if (toolbar)
    expr->AddAttributeValue("tool_bar", toolbar->id);
    
  expr->AddAttributeValue("no_status_fields", (long)noStatusLineFields);
  expr->AddAttributeValue("tiling_mode", (long)tilingMode);

  if (iconData)
    iconData->WritePrologAttributes(expr);

  return TRUE;
}

Bool BuildFrameData::MakeRealWindow(void)
{
  long windowStyle = wxSDI;
  if (thickFrame)
    windowStyle |= (wxTHICK_FRAME | wxRESIZE_BORDER);
  if (systemMenu)
    windowStyle |= wxSYSTEM_MENU;
  if (minBox)
    windowStyle |= wxMINIMIZE_BOX;
  if (maxBox)
    windowStyle |= wxMAXIMIZE_BOX;
  if (hasCaption)
    windowStyle |= wxCAPTION;
  
  UserFrame *frame = new UserFrame(NULL, title, x, y,
                 width, height, windowStyle);
  userWindow = frame;
  frame->buildWindow = this;

  MakeRealWindowChildren();
  
  if (noStatusLineFields > 0)
    frame->CreateStatusLine(noStatusLineFields);

  if (toolbar)
    toolbar->MakeRealWindow();

  buildMenuBar->MakeRealWindow();

  // This can resize the frame, so set last.
  if (buildMenuBar->userWindow)
    frame->SetMenuBar((wxMenuBar *)buildMenuBar->userWindow);

  frame->SetTitle(title);

  // Make icon, if possible.
  if (iconData->CreateIcon())
  {
    ((wxFrame *)userWindow)->SetIcon(iconData->GetIcon());
    // The frame now 'owns' the icon, so set the data to NULL
    // so we don't access it after the frame has been deleted.
    iconData->SetIcon(NULL);
  }

  SetTestMode(buildApp.testMode);

  userWindow->Show(TRUE);
#ifdef wx_motif
  userWindow->SetSize(x, y, -1, -1);
#endif
  return TRUE;
}

Bool BuildFrameData::DestroyRealWindow(void)
{
  if (userWindow)
  {
    userWindow->GetPosition(&x, &y);
    userWindow->GetSize(&width, &height);
    wxWindow *fr = userWindow;
    userWindow->OnClose();
    delete fr;
    userWindow = NULL;
  }

  if (toolbar)
  {
    // Toolbar deleted implicitly when the frame is deleted,
    // so must NULL the toolbar's userWindow as it's now invalid.
    toolbar->userWindow = NULL;
  }
  
  return TRUE;
}

void BuildFrameData::FindScreenPosition(int *sx, int *sy)
{
  if (!userWindow)
  {
    *sx = x; *sy = y; return;
  }
#ifdef wx_msw
  if (buildParent)
  {
    *sx = 0;
    *sy = 0;
    buildParent->userWindow->ClientToScreen(sx, sy);
  }
  else
  {
    userWindow->GetPosition(sx, sy);
  }
#endif
#ifdef wx_x
  // Assume for the moment that the position is already the
  // screen position - not true for MDI.
  userWindow->GetPosition(sx, sy);
#endif
}

void BuildFrameData::FindClientPosition(int sx, int sy, int *cx, int *cy)
{
  // Assume for the moment that the position is already the
  // screen position - not true for MDI.
  *cx = sx; *cy = sy;
  x = sx; y = sy;
/*
#ifdef wx_msw
  *cx = sx;
  *cy = sy;

  if (buildParent)
    buildParent->userWindow->ScreenToClient(cx, cy);
  else
  {
    *cx = sx; *cy = sy;
  }
  x = *cx; y = *cy;
#endif
#ifdef wx_x
  // Assume for the moment that the position is already the
  // screen position - not true for MDI.
  *cx = sx; *cy = sy;
  x = sx; y = sy;
#endif
*/
}

// Recalculate subwindow sizes if percentages don't
// add up.
void BuildFrameData::RecomputeSubwindowSizes(void)
{
  if (children.Number() < 1)
    return;

  int totalPercent = 0;
  int noSubwindows = 0;
  
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    if ((child->windowType == wxTYPE_PANEL) ||
        (child->windowType == wxTYPE_TEXT_WINDOW) ||
        (child->windowType == wxTYPE_CANVAS))
    {
      BuildSubwindowData *subwin = (BuildSubwindowData *)child;
      totalPercent += subwin->percentOfFrame;
      noSubwindows ++;
    }
    node = node->Next();
  }
  if ((noSubwindows > 0) && (totalPercent != 100))
  {
    int currentPercent = 0;
    int eachPercent = (int)(100.0/noSubwindows);
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();
      if ((child->windowType == wxTYPE_PANEL) ||
          (child->windowType == wxTYPE_TEXT_WINDOW) ||
          (child->windowType == wxTYPE_CANVAS))
      {
        BuildSubwindowData *subwin = (BuildSubwindowData *)child;
        if (node->Next())
        {
          subwin->percentOfFrame = eachPercent;
          currentPercent += eachPercent;
        }
        else
          // Allow for inexact division
          subwin->percentOfFrame = 100 - currentPercent;
      }
      node = node->Next();
    }
  }
}

/*
 * Do the tiling
 *
 * Algorithm:
 *
 * The goal is to exploit what constraints we have to calculate the
 * subwindow tiling. E.g. we may have 1 fixed-size subwindow, 1 variable-size
 * subwindow, then 1 fixed-size subwindow. This is doable if we place the
 * 2 fixed size windows, then allocate the remaining space to the middle one
 *
 * For vertical tiling,
 *
 * 1) set topY = frame top (0)
 *    set bottomY = frame height
 * 2) Loop from top towards bottom of frame:
 *      while we have fixed proportional subwindows, topY = calculated/fixed position.
 *      End loop if we can't calculate (i.e. we have an arbitrarily growing subwindow)
 * 3) Loop from bottom toward top of frame:
 *      while we have fixed proportional subwindows, bottomY = calculated/fixed position.
 *      End loop if we can't calculate (i.e. we have an arbitrarily growing subwindow)
 * 4) If > 1 variable subwindow left, error, else calculate remaining subwindow
 *    if any.
 *
 * For horizontal tiling, similar, but with leftX and rightX
 *
 */
 
void BuildFrameData::DoTiling(void)
{
  if (!userWindow) return;

  int subWindowWidth;
  int subWindowHeight;
  int subWindowX;
  int subWindowY;

  int frameWidth, frameHeight;
  userWindow->GetClientSize(&frameWidth, &frameHeight);

  /*
   * Allow for toolbar
   *
   */
  int toolHeight = 0;
  if (toolbar)
  {
    wxNode *node = toolbar->tools.First();
    if (node)
    {
      BuildTool *tool = (BuildTool *)node->Data();
      if (tool->bitmapData->GetBitmap())
      {
        toolHeight = tool->bitmapData->GetBitmap()->GetHeight() + 5;
      }
    }
    if (toolHeight == 0)
      toolHeight = 24 + 5;
    if (toolbar->userWindow)
      toolbar->userWindow->SetSize(0, 0, frameWidth, toolHeight);
  }
  if (children.Number() == 0)
    return;

  int currentSubwindowPosition = 0;
  if (toolbar && tilingMode)
    currentSubwindowPosition = toolHeight;

  int actualWidth = frameWidth;
  int actualHeight = frameHeight;

  actualHeight -= toolHeight;

  int bottom = tilingMode ? actualHeight : actualWidth;
  Bool canCalculate = TRUE;
  wxNode *node = children.First();
  wxNode *lastTopVisited = NULL;
  wxNode *lastBottomVisited = NULL;
  Bool someLeft = FALSE;

  // Go from top towards bottom...    
  while (node && canCalculate)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    if (IsSubwindow(child->windowType))
    {
      BuildSubwindowData *subwin = (BuildSubwindowData *)child;

      switch (subwin->resizeMode)
      {
        case RESIZE_GROW:
        {
          canCalculate = FALSE;
          someLeft = TRUE;
          break;
        }
        case RESIZE_PROPORTIONAL:
        {
          if (tilingMode)
          {
            subWindowWidth = frameWidth;
            subWindowHeight = (int)(((float)actualHeight*subwin->percentOfFrame)/100.0);
            subWindowX = 0;
            subWindowY = currentSubwindowPosition;
          }
          else
          {
            subWindowWidth = (int)(((float)frameWidth*subwin->percentOfFrame)/100.0);
            subWindowHeight = actualHeight;
            subWindowX = currentSubwindowPosition;
            subWindowY = toolHeight;
          }
          break;
        }
        case RESIZE_FIXED:
        {
          if (tilingMode)
          {
            subWindowWidth = frameWidth;
            subWindowHeight = subwin->height;
            subWindowX = 0;
            subWindowY = currentSubwindowPosition;
          }
          else
          {
            subWindowWidth = subwin->width;
            subWindowHeight = actualHeight;
            subWindowX = currentSubwindowPosition;
            subWindowY = toolHeight;
          }
          break;
        }
        default:
          break;
      }
      if (canCalculate)
      {
        if (subwin->userWindow)
          subwin->userWindow->SetSize(subWindowX, subWindowY, subWindowWidth, subWindowHeight);
        if (tilingMode)
          currentSubwindowPosition += subWindowHeight;
        else
          currentSubwindowPosition += subWindowWidth;
        lastTopVisited = node;
        node = node->Next();
      }
    }
    else
      node = node->Next();
  }
  int top = currentSubwindowPosition;

  // Start from the bottom of the frame.
  if (tilingMode)
    currentSubwindowPosition = actualHeight;
  else
    currentSubwindowPosition = frameWidth;

  canCalculate = TRUE;
  node = children.Last();
  while (canCalculate && node && (node != lastTopVisited))
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    if (IsSubwindow(child->windowType))
    {
      BuildSubwindowData *subwin = (BuildSubwindowData *)child;
      switch (subwin->resizeMode)
      {
        case RESIZE_GROW:
        {
          canCalculate = FALSE;
          someLeft = TRUE;
          break;
        }
        case RESIZE_PROPORTIONAL:
        {
          if (tilingMode)
          {
            subWindowWidth = frameWidth;
            subWindowHeight = (int)(((float)actualHeight*subwin->percentOfFrame)/100.0);
            subWindowX = 0;
            subWindowY = currentSubwindowPosition - subWindowHeight;
          }
          else
          {
            subWindowWidth = (int)(((float)frameWidth*subwin->percentOfFrame)/100.0);
            subWindowHeight = actualHeight;
            subWindowX = currentSubwindowPosition - subWindowWidth;
            subWindowY = toolHeight;
          }
          break;
        }
        case RESIZE_FIXED:
        {
          if (tilingMode)
          {
            subWindowWidth = frameWidth;
            subWindowHeight = subwin->height;
            subWindowX = 0;
            subWindowY = currentSubwindowPosition - subWindowHeight;
          }
          else
          {
            subWindowWidth = subwin->width;
            subWindowHeight = actualHeight;
            subWindowX = currentSubwindowPosition - subWindowWidth;
            subWindowY = toolHeight;
          }
          break;
        }
        default:
          break;
      }
      if (canCalculate)
      {
        if (subwin->userWindow)
          subwin->userWindow->SetSize(subWindowX, subWindowY, subWindowWidth, subWindowHeight);
        if (tilingMode)
          currentSubwindowPosition -= subWindowHeight;
        else
         currentSubwindowPosition -= subWindowWidth;
        lastBottomVisited = node;
        node = node->Previous();
      }
    }
    else
      node = node->Previous();
  }
  bottom = currentSubwindowPosition;
    
  // At least one left...
  if (someLeft)
  {
    wxNode *node;
    if (!lastTopVisited)
      node = children.First();
    else if (!lastBottomVisited)
      node = children.Last();
    else node = lastTopVisited->Next();
     
    BuildSubwindowData *child = (BuildSubwindowData *)node->Data();

    if (IsSubwindow(child->windowType))
    {
      BuildSubwindowData *subwin = (BuildSubwindowData *)child;
      // Resize it to the remaining space.
      if (tilingMode)
      {
        subWindowWidth = frameWidth;
        subWindowHeight = bottom - top;
        subWindowX = 0;
        subWindowY = top;
      }
      else
      {
        subWindowWidth = bottom - top;
        subWindowHeight = actualHeight;
        subWindowX = top;
        subWindowY = toolHeight;
      }
      if (child->userWindow)
        subwin->userWindow->SetSize(subWindowX, subWindowY, subWindowWidth, subWindowHeight);
    }
  }
}

// Add class-specific items to form
void BuildFrameData::AddFormItems(wxForm *form)
{
  form->Add(wxMakeFormString("Name", &name, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Description", &description, wxFORM_MULTITEXT, NULL, NULL, wxVERTICAL,
                 300, 100));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Class name", &className, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());
/*
  form->Add(wxMakeFormString("Icon name", &iconName, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());
*/
  form->Add(wxMakeFormString("Title", &title, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormShort("No. status line fields", &noStatusLineFields, wxFORM_DEFAULT, NULL, NULL,
            wxVERTICAL));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormBool("Vertical subwindow tiling", &tilingMode));
  form->Add(wxMakeFormBool("Thick frame", &thickFrame));
  form->Add(wxMakeFormBool("Caption", &hasCaption));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormBool("System menu", &systemMenu));
  form->Add(wxMakeFormBool("Minimize button", &minBox));
  form->Add(wxMakeFormBool("Maximize button", &maxBox));
}


// Move the first subwindow to the end, to rearrange them.
void FrameDoTheShuffle(wxButton&but, wxCommandEvent& event)
{
  BuildFrameData *frameData = (BuildFrameData *)but.GetClientData();
  if (frameData->children.Number() > 1)
  {
    wxNode *node = frameData->children.First();
    BuildWindowData *child = (BuildWindowData *)node->Data();
    frameData->children.DeleteObject(child);
    frameData->children.Append(child);
  }
}

static void FrameIconProc(wxButton& but, wxCommandEvent& event)
{
  MakeModified();
  BuildFrameData *data = (BuildFrameData *)but.GetClientData();
//  wxBitmap *oldBitmap = data->bitmapData->GetBitmap();
  data->iconData->Edit(but.GetParent());
//  data->bitmapData->SetBitmap(NULL);
  if (data->iconData->CreateIcon())
  {
    ((wxFrame *)data->userWindow)->SetIcon(data->iconData->GetIcon());
    data->iconData->SetIcon(NULL);
//    delete oldBitmap;
  }
}

// Add class-specific items to dialog, since forms can't
// cope with everything.
void BuildFrameData::AddDialogItems(wxDialogBox *dialog)
{
  dialog->NewLine();
  wxButton *but = new wxButton(dialog, (wxFunction)FrameDoTheShuffle, "Shuffle subwindows");
  but->SetClientData((char *)this);
//  dialog->NewLine();
  but = new wxButton(dialog, (wxFunction)FrameIconProc, "Edit icon properties");
  but->SetClientData((char *)this);
}

void BuildFrameData::WriteClassDeclaration(ostream& stream)
{
  // First write forward declarations for all the subwindow classes
  if (toolbar)
    stream << "class " << toolbar->className << ";\n";

  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    stream << "class " << child->className << ";\n";
    node = node->Next();

    if (!node)
      stream << "\n";
  }
  
  if (description && (strlen(description) > 0))
  {
    stream << "/*\n" << description << "\n*/\n";
  }

  stream << "class " << className << ": public wxFrame\n{\n";
  stream << " private:\n protected:\n public:\n";

  // Want to have as member variables all child subwindows
  // First, the toolbar, if any.
  if (toolbar)
  {
    stream << "  " << toolbar->className << " *" << toolbar->name << ";\n";
  }
  if (children.Number() > 0)
  {
    stream << "  // Subwindows for reference within the program.\n";
    nameSpace.BeginBlock();
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();
      if (child != toolbar)
      {
        stream << "  " << child->className << " *" << child->name << ";\n";
      }
      node = node->Next();
    }
    nameSpace.EndBlock();
    stream << "\n";
  }

  stream << "  // Constructor and destructor\n";
  stream << "  " << className ;
  stream << "(wxFrame *parent, char *title, int x, int y, int width, int height, long style, char *name);\n";
  stream << "  ~" << className << "(void);\n\n";
  stream << " Bool OnClose(void);\n";
  stream << " void OnSize(int w, int h);\n";
  stream << " void OnMenuCommand(int commandId);\n";
  stream << "};\n\n";

  // Write menu identifier declarations.
  MenuStringList.Clear();

  if (!buildApp.useResourceMethod)
  {
    stream << "/* Menu identifiers\n */\n";
    buildMenuBar->GenerateDefines(stream);
    stream << "\n";
  }

  // Write toolbar identifier declarations.
  stream << "/* Toolbar identifiers\n */\n";
  if (toolbar)
    toolbar->GenerateToolbarIdsCPP(stream);
  stream << "\n";

  node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();

    child->WriteClassDeclaration(stream);
    node = node->Next();
  }

  // Write toolbar declaration, if any
  if (toolbar)
  {
    toolbar->WriteClassDeclaration(stream);
  }
}

// Generation
void BuildFrameData::WriteClassImplementation(ostream& stream)
{
  if (description && (strlen(description) > 0))
  {
    stream << "/*\n" << description << "\n*/\n";
  }
  if (!buildApp.useResourceMethod && iconData->GetBitmapName() && (strlen(iconData->GetBitmapName()) > 0))
  {
    char iconBuf[150];

    strcpy(iconBuf, iconData->GetBitmapName());
    StripExtension(iconBuf);
    strcat(iconBuf, ".xbm");
    stream << "#ifdef wx_x\n";
    stream << "#include \"" << iconData->GetBitmapName() << ".xbm\"\n";
    stream << "#endif\n\n";
  }

  stream << className << "::" << className ;
  stream << "(wxFrame *parent, char *title, int x, int y, int width, int height, long style, char *name):\n";
  stream << "  wxFrame(parent, title, x, y, width, height, style, name)\n{\n";

  nameSpace.BeginBlock();

  // Initialize the subwindow data members
  if (children.Number() > 0)
  {
    stream << "  // Initialize child subwindow members.\n";
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();
      stream << "  " << child->name << " = NULL;\n";
      node = node->Next();
    }
    stream << "\n";
  }

  // Set the icon
  if (iconData->GetBitmapName() && (strlen(iconData->GetBitmapName()) > 0))
  {
    if (buildApp.useResourceMethod)
    {
      stream << "  " << "wxIcon *frameIcon = wxResourceCreateIcon(\"" << iconData->GetBitmapName() << "_resource\");\n";
    }
    else
    {
      char iconBuf[150];
      strcpy(iconBuf, iconData->GetBitmapName());
      StripExtension(iconBuf);
      stream << "#ifdef wx_x\n";
      stream << "  " << "wxIcon *frameIcon = new wxIcon(" << iconBuf << "_bits, " << iconBuf << "_width, ";
      stream << iconBuf << "_height);\n";
      stream << "#endif\n";
      stream << "#ifdef wx_msw\n";
      stream << "  " << "wxIcon *frameIcon = new wxIcon(\"" << iconBuf << "\");\n";
      stream << "#endif\n";
    }
    stream << "  " << "SetIcon(frameIcon);\n";
  }

  // Create the status line
  if (noStatusLineFields > 0)
  {
    stream << "  CreateStatusLine(" << noStatusLineFields << ");\n";
  }

  // Create the menu bar
  if (buildMenuBar && buildMenuBar->menus.Number() > 0)
  {
    stream << "  // Create a menu bar for the frame\n";
    buildMenuBar->GenerateMenuCPP(stream);
    stream << "  SetMenuBar(" << buildMenuBar->name << ");\n\n";
  }

  // Create all child subwindows
  if (children.Number() > 0)
  {
    stream << "  // Create child subwindows.\n";
    if (toolbar)
    {
      stream << "  " << toolbar->name << " = ";
      toolbar->GenerateConstructorCall(stream, "this");
      stream << ";\n";
    }
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();

      stream << "  " << child->name << " = ";
      child->GenerateConstructorCall(stream, "this");
      stream << ";\n";
      if (buildApp.useResourceMethod && ((child->windowType == wxTYPE_PANEL) || (child->windowType == wxTYPE_DIALOG_BOX)))
      {
        stream << "  " << child->name << "->LoadFromResource(" << "this" << ", \"" << child->name << "\");\n";
      }

      node = node->Next();
    }
    stream << "\n";
  }

  stream << "  // Ensure the subwindows get resized o.k.\n";
  stream << "  OnSize(width, height);\n\n";

  stream << "  // Centre frame on the screen.\n";
  stream << "  Centre(wxBOTH);\n\n";

  stream << "  // Show the frame.\n";
  stream << "  Show(TRUE);\n";

  nameSpace.EndBlock();
  stream << "}\n\n";

  stream << className << "::~" << className << "(void)\n{\n}\n\n";

  stream << "Bool " << className << "::OnClose(void)\n{\n";
  stream << "  // Clean up any associated data here.\n";
  stream << "  theApp." << name << " = NULL;\n";
  stream << "  return TRUE;\n";
  stream << "}\n\n";
  
  stream << "void " << className << "::OnSize(int w, int h)\n{\n";

  int noChildren = 0;
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    if (IsSubwindow(child->windowType))
      noChildren ++;
    node = node->Next();
  }

  if ((noChildren < 2) && !toolbar)
  {
    stream << "  // Let default member handle sizing for only one subwindow.\n"; 
    stream << "  wxFrame::OnSize(w, h);\n";
  }
  else
  {
    wxNode *node = children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();
      if (IsSubwindow(child->windowType))
      {
        BuildSubwindowData *subwin = (BuildSubwindowData *)child;
        stream << "  // Return if subwindows not yet initialized.\n";
        stream << "  if (!" << subwin->name << ") return;\n\n";
        node = NULL;
      }
      if (node)
        node = node->Next();
    }

    stream << "  int clientW, clientH;\n";
    stream << "  GetClientSize(&clientW, &clientH);\n";

    if (toolbar)
    {
      stream << "  int toolbarHeight, toolbarWidth;\n";
      // Find toolbar height from first tool
      stream << "  // Find toolbar height from first tool\n";
      stream << "  if (" << toolbar->name << "->GetTools().Number() > 0)\n";
      stream << "    toolbarHeight = ((wxToolBarTool *)" << toolbar->name << "->GetTools().First()->Data())->bitmap1->GetHeight() + 4;\n";
      stream << "  else toolbarHeight = 26;\n";
      stream << "  toolbarWidth = clientW;\n";
      stream << "  " << toolbar->name << "->SetSize(0, 0, toolbarWidth, toolbarHeight);\n";
    }
    else
    {
      stream << "  int toolbarHeight = 0;\n";
      stream << "  int toolbarWidth = 0;\n";
    }
    if (tilingMode)
    {
      stream << "  int currentY = toolbarHeight;\n\n";
      stream << "  int top = toolbarHeight;\n";
      stream << "  int bottom = clientH;\n";
    }
    else
    {
      stream << "  int currentX = 0;\n\n";
      stream << "  int top = 0;\n";
      stream << "  int bottom = clientW;\n";
    }

  Bool canCalculate = TRUE;
  node = children.First();
  wxNode *lastTopVisited = NULL;
  wxNode *lastBottomVisited = NULL;
  Bool someLeft = FALSE;

  // Go from top towards bottom...    
  while (node && canCalculate)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    if (IsSubwindow(child->windowType))
    {
      BuildSubwindowData *subwin = (BuildSubwindowData *)child;

      switch (subwin->resizeMode)
      {
        case RESIZE_GROW:
        {
          canCalculate = FALSE;
          someLeft = TRUE;
          break;
        }
        case RESIZE_PROPORTIONAL:
        {
          if (tilingMode)
          {
            stream << "  " << child->name;
            stream << "->SetSize(0, currentY, clientW, (int)((float)(clientH - toolbarHeight)*" << subwin->percentOfFrame;
            stream << "/100.0));\n";
            stream << "  currentY += (int)((float)(clientH - toolbarHeight)*" << subwin->percentOfFrame;
            stream << "/100.0);\n";
            stream << "  top = currentY;\n";
          }
          else
          {
            stream << "  " << child->name;
            stream << "->SetSize(currentX, toolbarHeight, (int)((float)clientW*" << subwin->percentOfFrame;
            stream << "/100.0), (clientH - toolbarHeight));\n";
            stream << "  currentX += (int)((float)clientW*" << subwin->percentOfFrame;
            stream << "/100.0);\n";
            stream << "  top = currentX;\n";
          }
          break;
        }
        case RESIZE_FIXED:
        {
          if (tilingMode)
          {
            stream << "  " << child->name;
            stream << "->SetSize(0, currentY, clientW, " << subwin->height << ");\n";
            stream << "  currentY += " << subwin->height << ";\n";
            stream << "  top = currentY;\n";
          }
          else
          {
            stream << "  " << child->name;
            stream << "->SetSize(0, toolbarHeight, " << subwin->width << ", (clientH-toolbarHeight));\n";
            stream << "  currentX += " << subwin->width << ";\n";
            stream << "  top = currentX;\n";
          }
          break;
        }
        default:
          break;
      }
      if (canCalculate)
      {
        lastTopVisited = node;
        node = node->Next();
      }
    }
    else
      node = node->Next();
  }

  // Start from the bottom of the frame.
  if (tilingMode)
  {
    stream << "  currentY = clientH;\n";
  }
  else
  {
    stream << "  currentX = clientW;\n";
  }

  canCalculate = TRUE;
  node = children.Last();
  while (canCalculate && node && (node != lastTopVisited))
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    if (IsSubwindow(child->windowType))
    {
      BuildSubwindowData *subwin = (BuildSubwindowData *)child;
      switch (subwin->resizeMode)
      {
        case RESIZE_GROW:
        {
          canCalculate = FALSE;
          someLeft = TRUE;
          break;
        }
        case RESIZE_PROPORTIONAL:
        {
          if (tilingMode)
          {
            stream << "  currentY -= (int)((float)(clientH-toolbarHeight)*" << subwin->percentOfFrame;
            stream << "/100.0);\n";
            stream << "  bottom = currentY;\n";
            stream << "  " << child->name;
            stream << "->SetSize(0, currentY, clientW, (int)((float)(clientH-toolbarHeight)*" << subwin->percentOfFrame;
            stream << "/100.0));\n";
          }
          else
          {
            stream << "  currentX -= (int)((float)clientW*" << subwin->percentOfFrame;
            stream << "/100.0);\n";
            stream << "  bottom = currentX;\n";
            stream << "  " << child->name;
            stream << "->SetSize(currentX, toolbarHeight, (int)((float)clientW*" << subwin->percentOfFrame;
            stream << "/100.0), (clientH-toolbarHeight));\n";
          }
          break;
        }
        case RESIZE_FIXED:
        {
          if (tilingMode)
          {
            stream << "  currentY -= " << subwin->height << ";\n";
            stream << "  bottom = currentY;\n";
            stream << "  " << child->name;
            stream << "->SetSize(0, currentY, clientW, " << subwin->height << ");\n";
          }
          else
          {
            stream << "  currentX -= " << subwin->width << ";\n";
            stream << "  bottom = currentX;\n";
            stream << "  " << child->name;
            stream << "->SetSize(0, toolbarHeight, " << subwin->width << ", (clientH-toolbarHeight);\n";
          }
          break;
        }
        default:
          break;
      }
      if (canCalculate)
      {
        lastBottomVisited = node;
        node = node->Previous();
      }
    }
    else
      node = node->Previous();
  }

  // At least one left...
  if (someLeft)
  {
    wxNode *node;
    if (!lastTopVisited)
      node = children.First();
    else if (!lastBottomVisited)
      node = children.Last();
    else node = lastTopVisited->Next();
     
    BuildSubwindowData *child = (BuildSubwindowData *)node->Data();

    if (IsSubwindow(child->windowType))
    {
      BuildSubwindowData *subwin = (BuildSubwindowData *)child;

      // Resize it to the remaining space.
      if (tilingMode)
      {
        stream << "  " << child->name;
        stream << "->SetSize(0, top, clientW, bottom - top);\n";
      }
      else
      {
        stream << "  " << child->name;
        stream << "->SetSize(top, toolbarHeight, bottom - top, (clientH-toolbarHeight));\n";
      }
    }
  }
  }

  stream << "}\n\n";
  
  stream << "void " << className << "::OnMenuCommand(int commandId)\n{\n";
  stream << "  switch (commandId)\n";
  stream << "  {\n";
  if (buildMenuBar)
  {
    buildMenuBar->GenerateSwitchStatementsCPP(this, stream);
  }
  stream << "  default:\n";
  stream << "    break;\n";
  stream << "  }\n";

  stream << "}\n\n";  // End of OnMenuCommand

  node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();

    child->WriteClassImplementation(stream);
    node = node->Next();
  }

  // Write toolbar implementation, if any
  if (toolbar)
  {
    toolbar->WriteClassImplementation(stream);
  }
}

void BuildFrameData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << className << "(" << parentName << ", " << SafeString(title) ;
  stream << ", " << x << ", " << y << ", ";
  stream << width << ", " << height << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", " << "\"" << name << "\")";
}

void BuildFrameData::GenerateWindowStyleString(char *buf)
{
  buf[0] = 0;
  if (windowStyle & wxSDI)
  {
    if (strlen(buf) > 0)
      strcat(buf, " | ");
    strcat(buf, "wxSDI");
  }
  if (windowStyle & wxMDI_PARENT)
  {
    if (strlen(buf) > 0)
      strcat(buf, " | ");
    strcat(buf, "wxMDI_PARENT");
  }
  if (windowStyle & wxMDI_CHILD)
  {
    if (strlen(buf) > 0)
      strcat(buf, " | ");
    strcat(buf, "wxCHILD");
  }
  if (windowStyle & wxMAXIMIZE)
  {
    if (strlen(buf) > 0)
      strcat(buf, " | ");
    strcat(buf, "wxMAXIMZE");
  }
  if (windowStyle & wxICONIZE)
  {
    if (strlen(buf) > 0)
      strcat(buf, " | ");
    strcat(buf, "wxICONIZE");
  }
  if ((windowStyle & wxDEFAULT_FRAME) == wxDEFAULT_FRAME)
  {
    if (strlen(buf) > 0)
      strcat(buf, " | ");
    strcat(buf, "wxDEFAULT_FRAME");
  }
  else
  {
    if (windowStyle & wxCAPTION)
    {
      if (strlen(buf) > 0)
        strcat(buf, " | ");
      strcat(buf, "wxCAPTION");
    }
    if (windowStyle & wxTHICK_FRAME)
    {
      if (strlen(buf) > 0)
        strcat(buf, " | ");
      strcat(buf, "wxTHICK_FRAME");
    }
    if (windowStyle & wxRESIZE_BORDER)
    {
      if (strlen(buf) > 0)
        strcat(buf, " | ");
      strcat(buf, "wxRESIZE_BORDER");
    }
    if (windowStyle & wxSYSTEM_MENU)
    {
      if (strlen(buf) > 0)
        strcat(buf, " | ");
      strcat(buf, "wxSYSTEM_MENU");
    }
    if (windowStyle & wxMINIMIZE_BOX)
    {
      if (strlen(buf) > 0)
        strcat(buf, " | ");
      strcat(buf, "wxMINIMIZE_BOX");
    }
    if (windowStyle & wxMAXIMIZE_BOX)
    {
      if (strlen(buf) > 0)
        strcat(buf, " | ");
      strcat(buf, "wxMAXIMIZE_BOX");
    }
  }
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

// Generate bitmap, icon etc. resource entries for RC file.
void BuildFrameData::GenerateResourceEntries(ostream &stream)
{
  if (iconData)
  {
    if (iconData->GetBitmapName() && (strlen(iconData->GetBitmapName()) > 0))
    {
      iconData->GenerateResourceEntry(stream);
    }
    else
    {
      Report("Warning: frame "); Report(name); Report( " has no icon.\n");
    }
  }
  
  // Toolbar bitmaps
  if (toolbar)
    toolbar->GenerateResourceEntries(stream);

  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->GenerateResourceEntries(stream);
    node = node->Next();
  }
}

void BuildFrameData::GenerateWXResourceBitmaps(ostream& stream)
{
  if (toolbar)
    toolbar->GenerateWXResourceBitmaps(stream);
  if (iconData)
    iconData->GenerateWXResourceBitmap(stream);
    
  BuildWindowData::GenerateWXResourceBitmaps(stream);
}

void BuildFrameData::GenerateWXResourceBitmapRegistration(ostream& stream)
{
  if (toolbar)
    toolbar->GenerateWXResourceBitmapRegistration(stream);
  if (iconData)
    iconData->GenerateWXResourceBitmapRegistration(stream);
}

// Write .wxr resource information for this window
void BuildFrameData::GenerateWXResourceData(ostream& stream)
{
  if (buildMenuBar)
  {
    buildMenuBar->GenerateWXResourceData(stream);
    stream << "\n";
  }
  if (toolbar)
  {
    toolbar->GenerateWXResourceData(stream);
    stream << "\n";
  }
  
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->GenerateWXResourceData(stream);
    node = node->Next();
  }
}

void BuildFrameData::GenerateBitmapDataIncludes(ostream& stream)
{
  if (toolbar)
    toolbar->GenerateBitmapDataIncludes(stream);
  if (iconData)
    iconData->GenerateBitmapDataInclude(stream);
}

// Write #defines
void BuildFrameData::GenerateDefines(ostream& stream)
{
  if (buildMenuBar)
  {
    buildMenuBar->GenerateDefines(stream);
    stream << "\n";
  }
  
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->GenerateDefines(stream);
    node = node->Next();
  }
}

// Write code to load resources from data
void BuildFrameData::GenerateResourceLoading(ostream& stream)
{
  if (buildMenuBar)
  {
    buildMenuBar->GenerateResourceLoading(stream);
  }
  if (toolbar)
  {
    toolbar->GenerateResourceLoading(stream);
  }
  if (iconData)
    iconData->GenerateResourceLoading(stream);

  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    child->GenerateResourceLoading(stream);
    node = node->Next();
  }
}

// Find action associated with this menu command id, if any.
BuildAction *BuildFrameData::FindMenuAction(int command)
{
  BuildAction *action = NULL;

  // Try to find an action corresponding to this command ID
  wxNode *node = buildActions.First();
  while (node)
  {
    BuildAction *act = (BuildAction *)node->Data();
    if ((act->eventType == wxEVENT_TYPE_MENU_COMMAND) &&
        (act->commandId == command))
    {
      action = act;
      node = NULL;
    }
    else node = node->Next();
  }
  return action;
}

/*
 * USER WINDOWS
 *
 */

/*
 * User frame
 *
 */

UserFrame::~UserFrame(void)
{
}

Bool UserFrame::OnClose(void)
{
  if (!Iconized())
  {
    GetPosition(&buildWindow->x, &buildWindow->y);
    GetSize(&buildWindow->width, &buildWindow->height);
  }
  buildApp.DisassociateObjectWithEditor(buildWindow);

  buildWindow->NullUserWindows();
  if (buildWindow->toolbar)
    buildWindow->toolbar->userWindow = NULL;
  buildWindow = NULL;
  return TRUE;
}

void UserFrame::OnSize(int w, int h)
{
  if (buildWindow)
  {
    buildWindow->RecomputeSubwindowSizes();
    buildWindow->DoTiling();
  }
}

void UserFrame::OnMenuCommand(int command)
{
  BuildAction *action = NULL;
  Bool newAction = TRUE;

  // Try to find an action corresponding to this command ID
  wxNode *node = buildWindow->buildActions.First();
  while (node)
  {
    BuildAction *act = (BuildAction *)node->Data();
    if ((act->eventType == wxEVENT_TYPE_MENU_COMMAND) &&
        (act->commandId == command))
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
      if (!action) return;

      action->commandId = command;
      action->eventType = wxEVENT_TYPE_MENU_COMMAND;
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
}

void UserFrame::OnMenuSelect(int command)
{
  wxFrame::OnMenuSelect(command);
}

void ShowFrameEditor(BuildFrameData *bframe)
{
}


