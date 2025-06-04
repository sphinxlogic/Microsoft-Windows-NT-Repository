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

BuildItemData::BuildItemData(BuildPanelData *theParent):
  BuildWindowData(theParent)
{
  labelPosition = TRUE;
  autoSize = TRUE;
  functionName = NULL;
}

BuildItemData::~BuildItemData(void)
{
  BuildSelections.DeleteObject(this);
}

Bool BuildItemData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildWindowData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValue("label_position", &labelPosition);
  expr->AssignAttributeValue("auto_size", &autoSize);
  return TRUE;
}

Bool BuildItemData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildWindowData::WritePrologAttributes(expr, database);
  expr->AddAttributeValue("label_position", (long)labelPosition);
  expr->AddAttributeValue("auto_size", (long)autoSize);
  return TRUE;
}

void BuildItemData::FindScreenPosition(int *sx, int *sy)
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

void BuildItemData::FindClientPosition(int sx, int sy, int *cx, int *cy)
{
  if (buildParent)
  {
    *cx = sx;
    *cy = sy;
    buildParent->userWindow->ScreenToClient(cx, cy);
    x = *cx; y = *cy;
  }
}

// Generation
void BuildItemData::WriteClassImplementation(ostream& stream)
{
}

void BuildItemData::WriteClassDeclaration(ostream& stream)
{
}

// Do any common pre-constructor stuff, such as setting the
// label orientation and fonts.
void BuildItemData::GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation)
{
  Bool defaultIsHorizontal = ((BuildPanelData *)buildParent)->horizLabelPosition;
  if (labelPosition == wxVERTICAL)
  {
    if (defaultIsHorizontal)
      stream << indentation << "SetLabelPosition(wxVERTICAL);\n";
  }
  else if (labelPosition == wxHORIZONTAL)
  {
    if (!defaultIsHorizontal)
      stream << indentation << "SetLabelPosition(wxHORIZONTAL);\n";
  }
}

void BuildItemData::GenerateWindowInitialisationPost(ostream& stream, char *instanceName, char *indentation)
{
  // Reset to default.
  Bool defaultIsHorizontal = ((BuildPanelData *)buildParent)->horizLabelPosition;
  if (labelPosition == wxVERTICAL)
  {
    if (defaultIsHorizontal)
      stream << indentation << "SetLabelPosition(wxHORIZONTAL);\n";
  }
  else if (labelPosition == wxHORIZONTAL)
  {
    if (!defaultIsHorizontal)
      stream << indentation << "SetLabelPosition(wxVERTICAL);\n";
  }
}

Bool BuildItemData::EditAttributes(void)
{
  char nameBuf[200];
  sprintf(nameBuf, "Properties for %s", name);
  wxDialogBox *dialog = new wxDialogBox(NULL, nameBuf, TRUE, 10, 10);
  dialog->SetLabelFont(SmallLabelFont);
  dialog->SetButtonFont(SmallButtonFont);

  BuildForm *form = new BuildForm;

  if (!((windowType == wxTYPE_BUTTON) ||
        (windowType == wxTYPE_MESSAGE)))
  {
    if (labelPosition == wxHORIZONTAL)
      labelPositionString = copystring("Horizontal");
    else if (labelPosition == wxVERTICAL)
      labelPositionString = copystring("Vertical");
    else
      labelPositionString = copystring("Default");
  }
  
  AddFormItems(form);

  form->AssociatePanel(dialog);
  form->dialog = dialog;
  AddDialogItems(dialog);

  dialog->Fit();
  dialog->Centre(wxBOTH);

//  form->RevertValues();

  dialog->Show(TRUE);

  if (!((windowType == wxTYPE_BUTTON) ||
        (windowType == wxTYPE_MESSAGE)))
  {
    if (strcmp(labelPositionString, "Horizontal") == 0)
      labelPosition = wxHORIZONTAL;
    else if (strcmp(labelPositionString, "Vertical") == 0)
      labelPosition = wxVERTICAL;
    else
      labelPosition = wxDEFAULT_POSITION;
  }

  // An empty title/label string should be NULLed
  if (title && (strcmp(title, "") == 0))
  {
    delete[] title;
    title = NULL;
  }

  return TRUE;
}

// Add class-specific items to form
void BuildItemData::AddFormItems(wxForm *form)
{
  SetHelpTopic("Procedures");
  form->Add(wxMakeFormString("Name", &name, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Description", &description, wxFORM_MULTITEXT, NULL, NULL, wxVERTICAL,
               300, 100));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Label", &title, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());
}

// Add class-specific items to dialog, since forms can't
// cope with everything.
void BuildItemData::AddDialogItems(wxDialogBox *dialog)
{
}

// Write .wxr resource information for this window
void BuildItemData::GenerateWXResourceData(ostream& stream)
{
  stream << "  control = [";
  char buf[300];
  if (labelPosition == wxVERTICAL)
  {
    if ((windowStyle & wxVERTICAL_LABEL) != wxVERTICAL_LABEL)
      windowStyle |= wxVERTICAL_LABEL;
  }
  GenerateWindowStyleString(buf);
  if (buf[0] == '0')
    buf[0] = 0;
    
  switch (windowType)
  {
    case wxTYPE_BUTTON:
    {
      BuildButtonData *buttonData  = (BuildButtonData *)this;
      stream << "wxButton, " << SafeWord(title) << ", '" << buf << "', ";
      stream << SafeWord(name) << ", ";
      stream << x << ", " << y << ", " << width << ", " << height;
      if (buttonData->bitmapData)
        stream << ", '" << buttonData->bitmapData->GetBitmapName() << "_resource'" ;
      break;
    }
    case wxTYPE_MESSAGE:
    {
      BuildMessageData *messageData  = (BuildMessageData *)this;
      stream << "wxMessage, " << SafeWord(title) << ", '" << buf << "', ";
      stream << SafeWord(name) << ", ";
      stream << x << ", " << y << ", " << width << ", " << height;
      if (messageData->bitmapData)
        stream << ", '" << messageData->bitmapData->GetBitmapName() << "_resource'" ;
      break;
    }
    case wxTYPE_CHECK_BOX:
    {
      BuildCheckBoxData *checkBoxData = (BuildCheckBoxData *)this;
      stream << "wxCheckBox, " << SafeWord(title) << ", '" << buf << "', ";
      stream << SafeWord(name) << ", ";
      stream << x << ", " << y << ", " << width << ", " << height;
      break;
    }
    case wxTYPE_GROUP_BOX:
    {
      BuildGroupBoxData *groupBoxData = (BuildGroupBoxData *)this;
      stream << "wxGroupBox, " << SafeWord(title) << ", '" << buf << "', ";
      stream << SafeWord(name) << ", ";
      stream << x << ", " << y << ", " << width << ", " << height;
      break;
    }
    case wxTYPE_TEXT:
    {
      BuildTextData *textData = (BuildTextData *)this;
      stream << "wxText, " << SafeWord(title) << ", '" << buf << "', ";
      stream << SafeWord(name) << ", ";
      stream << x << ", " << y << ", " << width << ", " << height;
      break;
    }
    case wxTYPE_MULTI_TEXT:
    {
      BuildMultiTextData *textData = (BuildMultiTextData *)this;
      stream << "wxMultiText, " << SafeWord(title) << ", '" << buf << "', ";
      stream << SafeWord(name) << ", ";
      stream << x << ", " << y << ", " << width << ", " << height;
      break;
    }
    case wxTYPE_GAUGE:
    {
      BuildGaugeData *gaugeData = (BuildGaugeData *)this;
      stream << "wxGauge, " << SafeWord(title) << ", '" << buf << "', ";
      stream << SafeWord(name) << ", ";
      stream << x << ", " << y << ", " << width << ", " << height;
      stream << ", " << gaugeData->gaugeValue << ", " << gaugeData->maxValue;
      break;
    }
    case wxTYPE_SLIDER:
    {
      BuildSliderData *sliderData = (BuildSliderData *)this;
      stream << "wxSlider, " << SafeWord(title) << ", '" << buf << "', ";
      stream << SafeWord(name) << ", ";
      stream << x << ", " << y << ", " << width << ", " << height;
      stream << ", " << sliderData->sliderValue << ", " << sliderData->minValue << ", " << sliderData->maxValue;
      break;
    }
    case wxTYPE_LIST_BOX:
    {
      BuildListBoxData *listBoxData = (BuildListBoxData *)this;
      stream << "wxListBox, " << SafeWord(title) << ", '" << buf << "', ";
      stream << SafeWord(name) << ", ";
      stream << x << ", " << y << ", " << width << ", " << height;
      // Default list of values
      stream << ", [";
      wxNode *node = listBoxData->strings.First();
      while (node)
      {
        char *s = (char *)node->Data();
        stream << SafeWord(s);
        if (node->Next())
          stream << ", ";
        node = node->Next();
      }
      stream << "], ";
      if (listBoxData->multipleSel)
        stream << "wxMULTIPLE";
      else
        stream << "wxSINGLE";

      break;
    }
    case wxTYPE_CHOICE:
    {
      BuildChoiceData *choiceData = (BuildChoiceData *)this;
      stream << "wxChoice, " << SafeWord(title) << ", '" << buf << "', ";
      stream << SafeWord(name) << ", ";
      stream << x << ", " << y << ", " << width << ", " << height;
      // Default list of values
      stream << ", [";
      wxNode *node = choiceData->strings.First();
      while (node)
      {
        char *s = (char *)node->Data();
        stream << SafeWord(s);
        if (node->Next())
          stream << ", ";
        node = node->Next();
      }
      stream << "]";

      break;
    }
    case wxTYPE_RADIO_BOX:
    {
      BuildRadioBoxData *radioBoxData = (BuildRadioBoxData *)this;
      stream << "wxRadioBox, " << SafeWord(title) << ", '" << buf << "', ";
      stream << SafeWord(name) << ", ";
      stream << x << ", " << y << ", " << width << ", " << height;
      // Default list of values
      stream << ", [";
      wxNode *node = radioBoxData->strings.First();
      while (node)
      {
        char *s = (char *)node->Data();
        stream << SafeWord(s);
        if (node->Next())
          stream << ", ";
        node = node->Next();
      }
      stream << "]";

      break;
    }
  }
  stream << "]";
}

/*
 * Button
 *
 */
  
BuildButtonData::BuildButtonData(BuildPanelData *theParent):
  BuildItemData(theParent)
{
  x = -1;
  y = -1;
  width = -1;
  height = -1;
  name = copystring(GetNewObjectName("button"));
  memberName = copystring(name);
  className = copystring("wxButton");
  title = copystring("untitled");
  windowStyle = 0;
  windowType = wxTYPE_BUTTON;
  functionName = copystring(GetNewObjectName("ButtonProc"));

//  bitmapFile = NULL;
//  buttonBitmap = NULL;
  bitmapData = NULL;
}

BuildButtonData::~BuildButtonData(void)
{
  if (bitmapData)
    delete bitmapData;    
}

Bool BuildButtonData::EditAttributes(void)
{
  return BuildItemData::EditAttributes();
}

// Add class-specific items to form
// Handles for us to associate the wxButton with client data
// AddFormItems
void BuildButtonData::AddFormItems(wxForm *form)
{
  BuildItemData::AddFormItems(form);
  form->Add(wxMakeFormBool("Auto size", &autoSize, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL));

  SetHelpTopic("Creating a button item");
/*
#if defined(wx_x) || FAFA_LIB
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Bitmap name", &bitmapFile, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL, 300));
#endif
*/
}

static wxButton *ButtonBitmapButton = NULL;

static void ButtonBitmapCheckBoxProc(wxCheckBox& check, wxCommandEvent& event)
{
  MakeModified();
  BuildButtonData *data = (BuildButtonData *)check.GetClientData();
  if (check.GetValue())
  {
    data->autoSize = TRUE;
    data->width = -1;
    data->height = -1;
    ButtonBitmapButton->Enable(TRUE);
    if (!data->bitmapData)
      data->bitmapData = new BuildBitmapData;
    data->bitmapData->Edit(check.GetParent());
    if (data->bitmapData->CreateBitmap())
      ((wxButton *)data->userWindow)->SetLabel(data->bitmapData->GetBitmap());
  }
  else
  {
    if (data->userWindow)
      ((wxButton *)data->userWindow)->SetLabel(data->title);

    ButtonBitmapButton->Enable(FALSE);
    if (data->bitmapData)
      delete data->bitmapData;
  }
}

static void ButtonBitmapButtonProc(wxButton& but, wxCommandEvent& event)
{
  MakeModified();
  BuildButtonData *data = (BuildButtonData *)but.GetClientData();
  wxBitmap *oldBitmap = data->bitmapData->GetBitmap();
  data->bitmapData->Edit(but.GetParent());
  data->bitmapData->SetBitmap(NULL);
  if (data->bitmapData->CreateBitmap())
  {
    ((wxButton *)data->userWindow)->SetLabel(data->bitmapData->GetBitmap());
    delete oldBitmap;
  }
  else
    data->bitmapData->SetBitmap(oldBitmap);
}

// Add class-specific items to dialog, since forms can't
// cope with everything.
void BuildButtonData::AddDialogItems(wxDialogBox *dialog)
{
  dialog->NewLine();
  wxCheckBox *checkBox = new wxCheckBox(dialog, (wxFunction)ButtonBitmapCheckBoxProc, "Use bitmap");
  checkBox->SetClientData((char *)this);
  ButtonBitmapButton = new wxButton(dialog, (wxFunction)ButtonBitmapButtonProc, "Edit bitmap properties");
  ButtonBitmapButton->SetClientData((char *)this);
  if (bitmapData)
  {
    checkBox->SetValue(TRUE);
  }
  else
  {
    checkBox->SetValue(FALSE);
    ButtonBitmapButton->Enable(FALSE);
  }
}

Bool BuildButtonData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::ReadPrologAttributes(expr, database);
//  expr->AssignAttributeValue("bitmap_file", &bitmapFile);
  if (expr->AttributeValue("bitmap"))
  {
    bitmapData = new BuildBitmapData;
    bitmapData->ReadPrologAttributes(expr);
  }
  else
  {
    // Backward compatibility
    char *bitmapFilename = NULL;
    expr->AssignAttributeValue("bitmap_file", &bitmapFilename);
    if (bitmapFilename)
    {
      if (strlen(bitmapFilename) > 0)
      {
        bitmapData = new BuildBitmapData;
        bitmapData->SetBitmapName(bitmapFilename);
      }
      delete[] bitmapFilename;
    }
  }
  return TRUE;
}

Bool BuildButtonData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::WritePrologAttributes(expr, database);
//  expr->AddAttributeValueString("bitmap_file", bitmapFile);
  if (bitmapData)
    bitmapData->WritePrologAttributes(expr);
  return TRUE;
}

Bool BuildButtonData::MakeRealWindow(void)
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
  
  UserButton *button = NULL;

#if defined(wx_x) || FAFA_LIB
  if (bitmapData && bitmapData->CreateBitmap())
  {
    button = new UserButton((wxPanel *)buildParent->userWindow,
                 (wxFunction)UserButtonCallback, bitmapData->GetBitmap(), x, y,
                 width, height, windowStyle);
  }
  else
#endif  
    button = new UserButton((wxPanel *)buildParent->userWindow,
                 (wxFunction)UserButtonCallback, title, x, y,
                 width, height, windowStyle);


  userWindow = button;
  button->buildWindow = this;

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildButtonData::DestroyRealWindow(void)
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

void BuildButtonData::WriteClassDeclaration(ostream& stream)
{
  if (!buildApp.useResourceMethod)
    stream << "void " << functionName << "(wxButton& but, wxCommandEvent& event);\n\n";
}

// Generation
void BuildButtonData::WriteClassImplementation(ostream& stream)
{
  if (!buildApp.useResourceMethod)
  {
    stream << "void " << functionName << "(wxButton& but, wxCommandEvent& event)\n";
    stream << "{\n";
    wxNode *node = buildActions.First();
    while (node)
    {
      BuildAction *action = (BuildAction *)node->Data();
      action->GenerateActionCPP(stream, "  ", this);
      node = node->Next();
    }
    stream << "}\n\n";
  }
}

void BuildButtonData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << "wxButton" << "(" << parentName;
  stream << ", (wxFunction)" << (functionName ? functionName : "NULL") << ", ";

  if (bitmapData)
  {
    // Generate name for the bitmap
    char buf1[50];
    strcpy(buf1, bitmapData->GetBitmapName());
    strcat(buf1, "Bitmap");
    stream << buf1 << ", ";
  }
  else
    stream << SafeString(title) << ", ";
  stream << x << ", " << y << ", ";
  stream << width << ", " << height << ", ";
  
  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", \"" << name << "\")";
}

// Generate bitmap, icon etc. resource entries for RC file.
void BuildButtonData::GenerateResourceEntries(ostream &stream)
{
  if (bitmapData)
    bitmapData->GenerateResourceEntry(stream);
}

void BuildButtonData::GenerateWXResourceBitmaps(ostream& stream)
{
  if (bitmapData)
    bitmapData->GenerateWXResourceBitmap(stream);
}

void BuildButtonData::GenerateWXResourceBitmapRegistration(ostream& stream)
{
  if (bitmapData)
    bitmapData->GenerateWXResourceBitmapRegistration(stream);
}

void BuildButtonData::GenerateResourceLoading(ostream &stream)
{
  if (bitmapData)
    bitmapData->GenerateResourceLoading(stream);
}

void BuildButtonData::GenerateBitmapDataIncludes(ostream& stream)
{
  if (bitmapData)
    bitmapData->GenerateBitmapDataInclude(stream);
}

/*
 * Checkbox
 *
 */
  
BuildCheckBoxData::BuildCheckBoxData(BuildPanelData *theParent):
  BuildItemData(theParent)
{
  x = -1;
  y = -1;
  width = -1;
  height = -1;
  name = copystring(GetNewObjectName("checkbox"));
  memberName = copystring(name);
  className = copystring("wxCheckBox");
  title = copystring("untitled");
  windowStyle = 0;
  windowType = wxTYPE_CHECK_BOX;
  functionName = copystring(GetNewObjectName("CheckBoxProc"));

  intValue = 0;
}

BuildCheckBoxData::~BuildCheckBoxData(void)
{
}

Bool BuildCheckBoxData::EditAttributes(void)
{
  return BuildItemData::EditAttributes();
}

// Add class-specific items to form
void BuildCheckBoxData::AddFormItems(wxForm *form)
{
  BuildItemData::AddFormItems(form);
  SetHelpTopic("Creating a checkbox item");
}

// Add class-specific items to dialog, since forms can't
// cope with everything.
void BuildCheckBoxData::AddDialogItems(wxDialogBox *dialog)
{
}

Bool BuildCheckBoxData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::ReadPrologAttributes(expr, database);
  expr->AssignAttributeValue("value", &intValue);
  return TRUE;
}

Bool BuildCheckBoxData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::WritePrologAttributes(expr, database);
  expr->AddAttributeValue("value", (long)intValue);
  return TRUE;
}

Bool BuildCheckBoxData::MakeRealWindow(void)
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
  
  UserCheckBox *button = new UserCheckBox((wxPanel *)buildParent->userWindow,
                 (wxFunction)NULL, title, x, y,
                 width, height, windowStyle);
  userWindow = button;
  button->buildWindow = this;

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildCheckBoxData::DestroyRealWindow(void)
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

void BuildCheckBoxData::WriteClassDeclaration(ostream& stream)
{
  if (!buildApp.useResourceMethod)
    stream << "void " << functionName << "(wxCheckBox& check, wxCommandEvent& event);\n\n";
}

// Generation
void BuildCheckBoxData::WriteClassImplementation(ostream& stream)
{
  if (!buildApp.useResourceMethod)
  {
    stream << "void " << functionName << "(wxCheckBox& but, wxCommandEvent& event)\n";
    stream << "{\n";
    stream << "}\n\n";
  }
}

void BuildCheckBoxData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << "wxCheckBox" << "(" << parentName;
  stream << ", (wxFunction)" << (functionName ? functionName : "NULL") << ", ";
  stream << SafeString(title) << ", " << x << ", " << y << ", ";
  stream << width << ", " << height << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", \"" << name << "\")";
}

/*
 * Message
 *
 */
  
BuildMessageData::BuildMessageData(BuildPanelData *theParent):
  BuildItemData(theParent)
{
  x = -1;
  y = -1;
  width = -1;
  height = -1;
  name = copystring(GetNewObjectName("message"));
  memberName = copystring(name);
  className = copystring("wxMessage");
  title = copystring("untitled");
  windowStyle = 0;
  windowType = wxTYPE_MESSAGE;
  bitmapData = NULL;
}

BuildMessageData::~BuildMessageData(void)
{
  if (bitmapData)
    delete bitmapData;
}

Bool BuildMessageData::EditAttributes(void)
{
  return BuildItemData::EditAttributes();
}

static wxButton *MessageBitmapButton = NULL;

static void MessageBitmapCheckBoxProc(wxCheckBox& check, wxCommandEvent& event)
{
  MakeModified();
  BuildMessageData *data = (BuildMessageData *)check.GetClientData();
  if (check.GetValue())
  {
    data->autoSize = TRUE;
    data->width = -1;
    data->height = -1;
    MessageBitmapButton->Enable(TRUE);
    if (!data->bitmapData)
      data->bitmapData = new BuildBitmapData;
    data->bitmapData->Edit(check.GetParent());
    if (data->bitmapData->CreateBitmap())
      ((wxMessage *)data->userWindow)->SetLabel(data->bitmapData->GetBitmap());
  }
  else
  {
    if (data->userWindow)
      ((wxMessage *)data->userWindow)->SetLabel(data->title);

    MessageBitmapButton->Enable(FALSE);
    if (data->bitmapData)
      delete data->bitmapData;
  }
}

static void MessageBitmapButtonProc(wxButton& but, wxCommandEvent& event)
{
  MakeModified();
  BuildMessageData *data = (BuildMessageData *)but.GetClientData();
  wxBitmap *oldBitmap = data->bitmapData->GetBitmap();
  data->bitmapData->Edit(but.GetParent());
  data->bitmapData->SetBitmap(NULL);
  if (data->bitmapData->CreateBitmap())
  {
    ((wxMessage *)data->userWindow)->SetLabel(data->bitmapData->GetBitmap());
    delete oldBitmap;
  }
  else
    data->bitmapData->SetBitmap(oldBitmap);
}

// Add class-specific items to dialog, since forms can't
// cope with everything.
void BuildMessageData::AddDialogItems(wxDialogBox *dialog)
{
  dialog->NewLine();
  wxCheckBox *checkBox = new wxCheckBox(dialog, (wxFunction)MessageBitmapCheckBoxProc, "Use bitmap");
  checkBox->SetClientData((char *)this);
  MessageBitmapButton = new wxButton(dialog, (wxFunction)MessageBitmapButtonProc, "Edit bitmap properties");
  MessageBitmapButton->SetClientData((char *)this);
  if (bitmapData)
  {
    checkBox->SetValue(TRUE);
  }
  else
  {
    checkBox->SetValue(FALSE);
    MessageBitmapButton->Enable(FALSE);
  }
}

// Add class-specific items to form
void BuildMessageData::AddFormItems(wxForm *form)
{
  BuildItemData::AddFormItems(form);
  SetHelpTopic("Creating a message item");
}

Bool BuildMessageData::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::ReadPrologAttributes(expr, database);
  if (expr->AttributeValue("bitmap"))
  {
    bitmapData = new BuildBitmapData;
    bitmapData->ReadPrologAttributes(expr);
  }
  return TRUE;
}

Bool BuildMessageData::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  BuildItemData::WritePrologAttributes(expr, database);
  if (bitmapData)
    bitmapData->WritePrologAttributes(expr);
  return TRUE;
}

Bool BuildMessageData::MakeRealWindow(void)
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

  UserMessage *message = NULL;
#if defined(wx_x) || FAFA_LIB
  if (bitmapData && bitmapData->CreateBitmap())
  {
    message = new UserMessage((wxPanel *)buildParent->userWindow,
                 bitmapData->GetBitmap(), x, y,
                 windowStyle);
  }
  else
#endif  
    message = new UserMessage((wxPanel *)buildParent->userWindow,
                 title, x, y,
                 windowStyle);
  userWindow = message;
  message->buildWindow = this;

  userWindow->Show(TRUE);
  return TRUE;
}

Bool BuildMessageData::DestroyRealWindow(void)
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

void BuildMessageData::WriteClassDeclaration(ostream& stream)
{
}

// Generation
void BuildMessageData::WriteClassImplementation(ostream& stream)
{
}

void BuildMessageData::GenerateConstructorCall(ostream& stream, char *parentName)
{
  stream << "new ";
  stream << "wxMessage" << "(" << parentName;
  stream << ", ";

  if (bitmapData)
  {
    // Generate name for the bitmap
    char buf1[50];
    strcpy(buf1, bitmapData->GetBitmapName());
    strcat(buf1, "Bitmap");
    stream << buf1 << ", ";
  }
  else
    stream << SafeString(title) << ", ";
  stream << x << ", " << y << ", ";

  char buf[400];
  GenerateWindowStyleString(buf);
  stream << buf << ", \"" << name << "\")";
}


// Generate bitmap, icon etc. resource entries for RC file.
void BuildMessageData::GenerateResourceEntries(ostream &stream)
{
  if (bitmapData)
    bitmapData->GenerateResourceEntry(stream);
}

void BuildMessageData::GenerateWXResourceBitmaps(ostream& stream)
{
  if (bitmapData)
    bitmapData->GenerateWXResourceBitmap(stream);
}

void BuildMessageData::GenerateWXResourceBitmapRegistration(ostream& stream)
{
  if (bitmapData)
    bitmapData->GenerateWXResourceBitmapRegistration(stream);
}

void BuildMessageData::GenerateResourceLoading(ostream &stream)
{
  if (bitmapData)
    bitmapData->GenerateResourceLoading(stream);
}

void BuildMessageData::GenerateBitmapDataIncludes(ostream& stream)
{
  if (bitmapData)
    bitmapData->GenerateBitmapDataInclude(stream);
}
