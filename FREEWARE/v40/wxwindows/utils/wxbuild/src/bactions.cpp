/*
 * File:	bactions.cc
 * Purpose:	wxWindows GUI builder -- actions.
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
#include "bactions.h"
#include "bwin.h"

ActionTypeRecord ActionTypeArray[] = {
    { WXBUILD_ACTION_LOAD_FILE, "Load file" },
//    { WXBUILD_ACTION_SAVE_FILE, "Save file" },
    { WXBUILD_ACTION_LOAD_TEXT_FILE, "Load text file" },
    { WXBUILD_ACTION_SAVE_TEXT_FILE, "Save text file" },
//    { WXBUILD_ACTION_LOAD_BITMAP, "Load bitmap file" },
//    { WXBUILD_ACTION_ABOUT_BOX, "'About' box" },
//    { WXBUILD_ACTION_HELP, "Invoke help" },
//    { WXBUILD_ACTION_MESSAGE_BOX, "Message box" },
//    { WXBUILD_ACTION_CHOICE_BOX, "Choice box" },
    { WXBUILD_ACTION_SHOW_WINDOW, "Show window" },
    { WXBUILD_ACTION_CLOSE_WINDOW, "Close window" },
//    { WXBUILD_ACTION_SET_LABEL, "Set window label" },
//    { WXBUILD_ACTION_ADD_STRING, "Add list string" },
//    { WXBUILD_ACTION_COPY_METAFILE_TO_CLIPBOARD, "Copy metafile" },
//   { WXBUILD_ACTION_PRINT, "Print" }
  };

int noActionTypes = sizeof(ActionTypeArray)/sizeof(ActionTypeRecord);

BuildAction::BuildAction(int typ, WXTYPE evType, WXTYPE winType1, WXTYPE winType2)
{
  actionType = typ;
  eventType = evType;
  windowType1 = winType1;
  windowType2 = winType2;

  filename = NULL;
  stringArg1 = NULL;
  stringArg2 = NULL;
  integerArg1 = 0;
  integerArg2 = 0;

  actionWindow = NULL;
  actionWindowId = -1;
  commandId = -1;
}

BuildAction::~BuildAction(void)
{
  if (filename) delete[] filename;
  if (stringArg1) delete[] stringArg1;
  if (stringArg2) delete[] stringArg2;
}

void BuildAction::ReadPrologAttributes(PrologExpr *listExpr, PrologDatabase *database)
{
  actionType = (int)listExpr->Nth(0)->IntegerValue();
  eventType = (WXTYPE)listExpr->Nth(1)->IntegerValue();
  commandId = (int)listExpr->Nth(2)->IntegerValue();
  actionWindowId = listExpr->Nth(3)->IntegerValue();

  char *s = listExpr->Nth(4)->StringValue();
  if (s)
    filename = copystring(s);
  s = listExpr->Nth(5)->StringValue();
  if (s)
    stringArg1 = copystring(s);
  s = listExpr->Nth(6)->StringValue();
  if (s)
    stringArg2 = copystring(s);
  integerArg1 = listExpr->Nth(7)->IntegerValue();
  integerArg2 = listExpr->Nth(8)->IntegerValue();
}

// Write the build action into the PrologExpr list
void BuildAction::WritePrologAttributes(PrologExpr *listExpr, PrologDatabase *database)
{
  listExpr->Append(new PrologExpr((long)actionType));
  listExpr->Append(new PrologExpr((long)eventType));
  listExpr->Append(new PrologExpr((long)commandId));
  if (actionWindow)
    listExpr->Append(new PrologExpr(actionWindow->id));
  else
    listExpr->Append(new PrologExpr((long)-1));
  listExpr->Append(new PrologExpr(PrologString, (filename ? filename : "")));
  listExpr->Append(new PrologExpr(PrologString, (stringArg1? stringArg1: "")));
  listExpr->Append(new PrologExpr(PrologString, (stringArg2? stringArg2: "")));
  listExpr->Append(new PrologExpr(integerArg1));
  listExpr->Append(new PrologExpr(integerArg2));
}

void BuildAction::MakeWindowList(wxList& list)
{
  wxNode *node = buildApp.topLevelWindows.First();
  while (node)
  {
    BuildWindowData *win = (BuildWindowData *)node->Data();
    FindWindows(win, list);
    node = node->Next();
  }
}

void BuildAction::FindWindows(BuildWindowData *win, wxList& list)
{
  if (!list.Member(win))
  {
    Bool ok = FALSE;
    if ((windowType1 != -1) && (windowType2 != -1))
    {
      if ((win->windowType == windowType1) ||
          (win->windowType == windowType2))
        ok = TRUE;
    }
    else
    {
      if (win->windowType == windowType1)
        ok = TRUE;
    }
    if (ok)
      list.Append(win);
  }
  wxNode *node = win->children.First();
  while (node)
  {
    BuildWindowData *child = (BuildWindowData *)node->Data();
    FindWindows(child, list);
    node = node->Next();
  }
}

BuildWindowData *BuildAction::FindWindowForName(char *name, wxList& list)
{
  wxNode *node = list.First();
  while (node)
  {
    BuildWindowData *win = (BuildWindowData *)node->Data();
    if (win->name && (strcmp(name, win->name) == 0))
      return win;
    node = node->Next();
  }
  return NULL;
}

static Bool BuildActionDelete = FALSE;
void BuildActionDeleteProc(wxButton& but, wxCommandEvent& event)
{
  BuildForm *form = (BuildForm *)but.GetClientData();
  form->dialog->Show(FALSE);
  delete form;
  delete form->dialog;
  BuildActionDelete = TRUE;
}

Bool BuildAction::EditAction(void)
{
  BuildActionDelete = FALSE;
  MakeModified();

  switch (actionType)
  {
    case WXBUILD_ACTION_SHOW_WINDOW:
    {
      windowType1 = wxTYPE_FRAME;
      windowType2 = wxTYPE_DIALOG_BOX;
      wxList windowList;
      MakeWindowList(windowList);
      wxList *stringList = new wxList;
      wxNode *node = windowList.First();
      while (node)
      {
        BuildWindowData *win = (BuildWindowData *)node->Data();
        char *s = copystring(win->name);
        stringList->Append((wxObject *)s);
        node = node->Next();
      }

      char *windowName = NULL;
      Bool deleteAction = FALSE;
      if (actionWindow)
        windowName = copystring(actionWindow->name);
      
      BuildForm *form = new BuildForm("Associating actions with events");
      form->Add(wxMakeFormString("Window to show", &windowName, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings(stringList), NULL), NULL, wxVERTICAL));
      wxDialogBox *dialog = new wxDialogBox(NULL, "Show window", TRUE);
      dialog->SetLabelFont(SmallLabelFont);
      dialog->SetButtonFont(SmallButtonFont);
      form->AssociatePanel(dialog);
      dialog->NewLine();
      wxButton *but = new wxButton(dialog, (wxFunction)BuildActionDeleteProc, "Delete action");
      but->SetClientData((char *)form);
      form->dialog = dialog;
      dialog->Fit();
      dialog->Centre(wxBOTH);
      dialog->Show(TRUE);
      if (windowName)
        actionWindow = FindWindowForName(windowName, windowList);
      if (windowName) delete[] windowName;

      if (BuildActionDelete)
        return FALSE;
      
      break;
    }
    case WXBUILD_ACTION_CLOSE_WINDOW:
    {
      windowType1 = wxTYPE_FRAME;
      windowType2 = wxTYPE_DIALOG_BOX;
      wxList windowList;
      MakeWindowList(windowList);
      wxList *stringList = new wxList;
      wxNode *node = windowList.First();
      while (node)
      {
        BuildWindowData *win = (BuildWindowData *)node->Data();
        char *s = copystring(win->name);
        stringList->Append((wxObject *)s);
        node = node->Next();
      }

      char *windowName = NULL;
      Bool deleteAction = FALSE;
      if (actionWindow)
        windowName = copystring(actionWindow->name);
      
      BuildForm *form = new BuildForm("Associating actions with events");
      form->Add(wxMakeFormString("Window to close", &windowName, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings(stringList), NULL), NULL, wxVERTICAL));
      wxDialogBox *dialog = new wxDialogBox(NULL, "Close window", TRUE);
      dialog->SetLabelFont(SmallLabelFont);
      dialog->SetButtonFont(SmallButtonFont);
      form->AssociatePanel(dialog);
      dialog->NewLine();
      wxButton *but = new wxButton(dialog, (wxFunction)BuildActionDeleteProc, "Delete action");
      but->SetClientData((char *)form);
      form->dialog = dialog;
      dialog->Fit();
      dialog->Centre(wxBOTH);
      dialog->Show(TRUE);
      if (windowName)
        actionWindow = FindWindowForName(windowName, windowList);
      if (windowName) delete[] windowName;

      if (BuildActionDelete)
        return FALSE;
      
      break;
    }
    case WXBUILD_ACTION_LOAD_FILE:
    {
      Bool deleteAction = FALSE;
      
      BuildForm *form = new BuildForm("Associating actions with events");
      form->Add(wxMakeFormString("File selector prompt", &stringArg1, wxFORM_DEFAULT,
              NULL, NULL, wxVERTICAL, 200));
      form->Add(wxMakeFormNewLine());
      form->Add(wxMakeFormString("Default filename", &filename, wxFORM_DEFAULT,
              NULL, NULL, wxVERTICAL, 200));
      form->Add(wxMakeFormNewLine());
      form->Add(wxMakeFormString("Extension", &stringArg2, wxFORM_DEFAULT,
              NULL, NULL, wxVERTICAL, 200));
      wxDialogBox *dialog = new wxDialogBox(NULL, "Load file", TRUE);
      dialog->SetLabelFont(SmallLabelFont);
      dialog->SetButtonFont(SmallButtonFont);
      form->AssociatePanel(dialog);
      dialog->NewLine();
      wxButton *but = new wxButton(dialog, (wxFunction)BuildActionDeleteProc, "Delete action");
      but->SetClientData((char *)form);
      form->dialog = dialog;
      dialog->Fit();
      dialog->Centre(wxBOTH);
      dialog->Show(TRUE);

      if (BuildActionDelete)
        return FALSE;
      
      break;
    }
    case WXBUILD_ACTION_LOAD_TEXT_FILE:
    {
      windowType1 = wxTYPE_TEXT_WINDOW;
      wxList windowList;
      MakeWindowList(windowList);
      wxList *stringList = new wxList;
      wxNode *node = windowList.First();
      while (node)
      {
        BuildWindowData *win = (BuildWindowData *)node->Data();
        char *s = copystring(win->name);
        stringList->Append((wxObject *)s);
        node = node->Next();
      }

      char *windowName = NULL;
      Bool deleteAction = FALSE;
      if (actionWindow)
        windowName = copystring(actionWindow->name);
      
      BuildForm *form = new BuildForm("Associating actions with events");
      form->Add(wxMakeFormString("Text window to load into", &windowName, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings(stringList), NULL), NULL, wxVERTICAL));
      form->Add(wxMakeFormNewLine());
      form->Add(wxMakeFormString("File selector prompt", &stringArg1, wxFORM_DEFAULT,
              NULL, NULL, wxVERTICAL, 200));
      form->Add(wxMakeFormNewLine());
      form->Add(wxMakeFormString("Default filename", &filename, wxFORM_DEFAULT,
              NULL, NULL, wxVERTICAL, 200));
      form->Add(wxMakeFormNewLine());
      form->Add(wxMakeFormString("Extension", &stringArg2, wxFORM_DEFAULT,
              NULL, NULL, wxVERTICAL, 200));
      wxDialogBox *dialog = new wxDialogBox(NULL, "Load text file", TRUE);
      dialog->SetLabelFont(SmallLabelFont);
      dialog->SetButtonFont(SmallButtonFont);
      form->AssociatePanel(dialog);
      dialog->NewLine();
      wxButton *but = new wxButton(dialog, (wxFunction)BuildActionDeleteProc, "Delete action");
      but->SetClientData((char *)form);
      form->dialog = dialog;
      dialog->Fit();
      dialog->Centre(wxBOTH);
      dialog->Show(TRUE);
      if (windowName)
        actionWindow = FindWindowForName(windowName, windowList);
      if (windowName) delete[] windowName;

      if (BuildActionDelete)
        return FALSE;
      
      break;
    }
    case WXBUILD_ACTION_SAVE_TEXT_FILE:
    {
      windowType1 = wxTYPE_TEXT_WINDOW;
      wxList windowList;
      MakeWindowList(windowList);
      wxList *stringList = new wxList;
      wxNode *node = windowList.First();
      while (node)
      {
        BuildWindowData *win = (BuildWindowData *)node->Data();
        char *s = copystring(win->name);
        stringList->Append((wxObject *)s);
        node = node->Next();
      }

      char *windowName = NULL;
      Bool deleteAction = FALSE;
      if (actionWindow)
        windowName = copystring(actionWindow->name);
      
      BuildForm *form = new BuildForm("Associating actions with events");
      form->Add(wxMakeFormString("Text window to save from", &windowName, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings(stringList), NULL), NULL, wxVERTICAL));
      form->Add(wxMakeFormNewLine());
      form->Add(wxMakeFormString("File selector prompt", &stringArg1, wxFORM_DEFAULT,
              NULL, NULL, wxVERTICAL, 200));
      form->Add(wxMakeFormNewLine());
      form->Add(wxMakeFormString("Default filename", &filename, wxFORM_DEFAULT,
              NULL, NULL, wxVERTICAL, 200));
      form->Add(wxMakeFormNewLine());
      form->Add(wxMakeFormString("Extension", &stringArg2, wxFORM_DEFAULT,
              NULL, NULL, wxVERTICAL, 200));
      wxDialogBox *dialog = new wxDialogBox(NULL, "Save text file", TRUE);
      dialog->SetLabelFont(SmallLabelFont);
      dialog->SetButtonFont(SmallButtonFont);
      form->AssociatePanel(dialog);
      dialog->NewLine();
      wxButton *but = new wxButton(dialog, (wxFunction)BuildActionDeleteProc, "Delete action");
      but->SetClientData((char *)form);
      form->dialog = dialog;
      dialog->Fit();
      dialog->Centre(wxBOTH);
      dialog->Show(TRUE);
      if (windowName)
        actionWindow = FindWindowForName(windowName, windowList);
      if (windowName) delete[] windowName;

      if (BuildActionDelete)
        return FALSE;
      
      break;
    }
    default:
      return FALSE;
      break;
  }
  return TRUE;
}

Bool BuildAction::DoAction(void)
{
  switch (actionType)
  {
    case WXBUILD_ACTION_SHOW_WINDOW:
    {
      if (actionWindow)
      {
        wxBeginBusyCursor();

        // If we already have a real window...
        if (actionWindow->userWindow)
        {
          // If it's a frame, show it (ignore dialog box pro tem)
          if (actionWindow->windowType == wxTYPE_FRAME)
          {
            ((wxFrame *)actionWindow->userWindow)->Iconize(FALSE);
            actionWindow->userWindow->Show(TRUE);
          }
        }
        else
        {
          // No real window - make it
          actionWindow->MakeRealWindow();

          // If it's a frame, show it (ignore dialog box pro tem)
          if (actionWindow->windowType == wxTYPE_FRAME)
            actionWindow->userWindow->Show(TRUE);
        }
        wxEndBusyCursor();
      }
      break;
    }
    case WXBUILD_ACTION_CLOSE_WINDOW:
    {
      if (actionWindow)
      {
        // If we already have a real window...
        if (actionWindow->userWindow)
        {
          wxBeginBusyCursor();

          actionWindow->userWindow->Show(FALSE);
          if (MainFrame->currentWindow && (MainFrame->currentWindow == actionWindow))
            buildApp.DisassociateObjectWithEditor(actionWindow);
          actionWindow->DestroyRealWindow();

          wxEndBusyCursor();
	}
      }
      break;
    }
    case WXBUILD_ACTION_LOAD_FILE:
    {
      char *ext = "txt";
      char wild[20];
      char *prompt = "Load a file";
      if (stringArg1 && strlen(stringArg1))
        prompt = stringArg1;

      if (stringArg2 && strlen(stringArg2))
      {
        ext = stringArg2;
        sprintf(wild, "*.%s", ext);
      }
      else
        sprintf(wild, "*.txt");

      char *s = wxFileSelector(prompt, NULL, filename, ext, wild);
      break;
    }
    case WXBUILD_ACTION_LOAD_TEXT_FILE:
    {
      if (!actionWindow || !actionWindow->userWindow)
        return FALSE;
      
      char *ext = "txt";
      char wild[20];
      char *prompt = "Load a text file";
      if (stringArg1 && strlen(stringArg1))
        prompt = stringArg1;

      if (stringArg2 && strlen(stringArg2))
      {
        ext = stringArg2;
        sprintf(wild, "*.%s", ext);
      }
      else
        sprintf(wild, "*.txt");

      char *f = wxFileSelector(prompt, NULL, filename, ext, wild);
      if (f)
      {
        wxTextWindow *win = (wxTextWindow *)actionWindow->userWindow;
        win->LoadFile(f);
      }
      break;
    }
    case WXBUILD_ACTION_SAVE_TEXT_FILE:
    {
      if (!actionWindow || !actionWindow->userWindow)
        return FALSE;
      
      char *ext = "txt";
      char wild[20];
      char *prompt = "Save a text file";
      if (stringArg1 && strlen(stringArg1))
        prompt = stringArg1;

      if (stringArg2 && strlen(stringArg2))
      {
        ext = stringArg2;
        sprintf(wild, "*.%s", ext);
      }
      else
        sprintf(wild, "*.txt");

      char *f = wxFileSelector(prompt, NULL, filename, ext, wild);
      if (f)
      {
        wxTextWindow *win = (wxTextWindow *)actionWindow->userWindow;
        win->SaveFile(f);
      }
      break;
    }
    default:
      break;
  }
  return FALSE;
}

// win is the window from which the action originates.
void BuildAction::GenerateActionCPP(ostream& stream, char *indentation, BuildWindowData *win)
{
  switch (actionType)
  {
    case WXBUILD_ACTION_SHOW_WINDOW:
    {
      stream << indentation << "// Create window, or show it if already created.\n";
      stream << indentation << "if (!theApp." << actionWindow->name << ")\n";
      stream << indentation << "{\n";
      actionWindow->GenerateWindowInitialisationPre(stream, actionWindow->name, indentation);
      stream << indentation << "  theApp." << actionWindow->name << " = \n";
      stream << indentation;
      actionWindow->GenerateConstructorCall(stream,
         (actionWindow->buildParent ? actionWindow->buildParent->name : "NULL"));
      stream << ";\n";
      if (buildApp.useResourceMethod && ((actionWindow->windowType == wxTYPE_PANEL) || actionWindow->windowType == wxTYPE_DIALOG_BOX))
      {
        stream << indentation << "theApp." << actionWindow->name << "->LoadFromResource(" << "this" << ", \"" << actionWindow->name << "\");\n";
      }
      actionWindow->GenerateWindowInitialisationPost(stream, actionWindow->name, indentation);
      stream << "\n";
      stream << indentation << "}\n";

      // If a frame, make sure isn't iconized.
      if (actionWindow->windowType == wxTYPE_FRAME)
        stream << indentation << "theApp." << actionWindow->name << "->Iconize(FALSE);\n";
      stream << indentation << "theApp." << actionWindow->name << "->Show(TRUE);\n";
      break;
    }
    case WXBUILD_ACTION_CLOSE_WINDOW:
    {
      stream << indentation << "// Close a window.\n";
      stream << indentation << "if (theApp." << actionWindow->name << ")\n";
      stream << indentation << "{\n";
      if (actionWindow->windowType == wxTYPE_DIALOG_BOX)
      {
        stream << indentation << "  theApp." << actionWindow->name << "->Show(FALSE);\n";
        stream << indentation << "  delete theApp." << actionWindow->name << ";\n";
      }
      else if (actionWindow->windowType == wxTYPE_FRAME)
      {
        stream << indentation << "  wxFrame *fr = theApp." << actionWindow->name << ";\n";
        stream << indentation << "  fr->Show(FALSE);\n";
        stream << indentation << "  fr->OnClose();\n";
        stream << indentation << "  delete fr;\n";
      }
      stream << indentation << "  theApp." << actionWindow->name << " = NULL;\n";
      stream << indentation << "}\n";
      break;
    }
    case WXBUILD_ACTION_LOAD_FILE:
    {
      stream << indentation << "// Show file selector.\n";
      stream << indentation << "char *f = wxFileSelector(";
      if (stringArg1 && strlen(stringArg1) > 0)
        stream << "\"" << stringArg1 << "\"";
      else stream << "\"File to load\"";

      stream << ", NULL, ";
      if (filename && strlen(filename) > 0)
        stream << "\"" << filename << "\"";
      else stream << "NULL";
      stream << ", ";
      if (stringArg2 && strlen(stringArg2) > 0)
        stream << "\"" << stringArg2 << "\"";
      else stream << "NULL";
      stream << ", \"*.";
      if (stringArg2 && strlen(stringArg2) > 0)
        stream << stringArg2;
      else stream << "*";
      stream << "\");\n";
      stream << indentation << "if (!f)\n";
      stream << indentation << "  return;\n";
      break;
    }
    case WXBUILD_ACTION_LOAD_TEXT_FILE:
    {
      if (!actionWindow)
        return;
      if (!actionWindow->buildParent)
        return;

      char *textMember = actionWindow->name;
      char *frameMember = actionWindow->buildParent->name;
    
      stream << indentation << "// Show file selector.\n";
      stream << indentation << "char *f = wxFileSelector(";
      if (stringArg1 && strlen(stringArg1) > 0)
        stream << "\"" << stringArg1 << "\"";
      else stream << "\"File to load\"";

      stream << ", NULL, ";
      if (filename && strlen(filename) > 0)
        stream << "\"" << filename << "\"";
      else stream << "NULL";
      stream << ", ";
      if (stringArg2 && strlen(stringArg2) > 0)
        stream << "\"" << stringArg2 << "\"";
      else stream << "NULL";
      stream << ", \"*.";
      if (stringArg2 && strlen(stringArg2) > 0)
        stream << stringArg2;
      else stream << "*";
      stream << "\");\n";
      stream << indentation << "if (f)\n";
      stream << indentation << "{\n";
      stream << indentation << "  if (theApp." << frameMember << ")\n";
      stream << indentation << "    theApp." << frameMember << "->" << textMember << "->LoadFile(f);\n";
      stream << indentation << "}\n";

      break;
    }
    case WXBUILD_ACTION_SAVE_TEXT_FILE:
    {
      if (!actionWindow)
        return;
      if (!actionWindow->buildParent)
        return;

      char *textMember = actionWindow->name;
      char *frameMember = actionWindow->buildParent->name;
    
      stream << indentation << "// Show file selector.\n";
      stream << indentation << "char *f = wxFileSelector(";
      if (stringArg1 && strlen(stringArg1) > 0)
        stream << "\"" << stringArg1 << "\"";
      else stream << "\"File to save\"";

      stream << ", NULL, ";
      if (filename && strlen(filename) > 0)
        stream << "\"" << filename << "\"";
      else stream << "NULL";
      stream << ", ";
      if (stringArg2 && strlen(stringArg2) > 0)
        stream << "\"" << stringArg2 << "\"";
      else stream << "NULL";
      stream << ", \"*.";
      if (stringArg2 && strlen(stringArg2) > 0)
        stream << stringArg2;
      else stream << "*";
      stream << "\");\n";
      stream << indentation << "if (f)\n";
      stream << indentation << "{\n";
      stream << indentation << "  if (theApp." << frameMember << ")\n";
      stream << indentation << "    theApp." << frameMember << "->" << textMember << "->SaveFile(f);\n";
      stream << indentation << "}\n";

      break;
    }
    default:
      break;
  }
}

BuildAction *CreateNewAction(void)
{
  char **strings = new char *[noActionTypes];

  for (int i = 0; i < noActionTypes; i++)
  {
    strings[i] = ActionTypeArray[i].actionString;
  }
  int choice = wxGetSingleChoiceIndex("Choose an action", "Choice",
     noActionTypes, strings);
  delete[] strings;
  if (choice > -1)
  {
    BuildAction *action = new BuildAction(ActionTypeArray[choice].actionId);
    return action;
  }
  else return NULL;
}

