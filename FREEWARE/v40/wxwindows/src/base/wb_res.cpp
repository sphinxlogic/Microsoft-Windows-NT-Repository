/*
 * File:	wb_res.cc
 * Purpose:	Resource processor
 * Author:	Julian Smart
 * Created:	1994
 * Updated:	
 * Copyright:	(c) 1994, Julian Smart
 */

/* static const char sccsid[] = "%W% %G%"; */

#ifdef __GNUG__
#pragma implementation "wx_res.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_defs.h"
#include "wx_setup.h"
#include "wx_list.h"
#include "wx_hash.h"
#include "wx_gdi.h"
#include "wx_utils.h"
#include "wx_types.h"
#include "wx_menu.h"

#include "wx_panel.h"
#include "wx_messg.h"
#include "wx_buttn.h"
#include "wx_rbox.h"
#include "wx_lbox.h"
#include "wx_choic.h"
#include "wx_check.h"
#include "wx_slidr.h"
#include "wx_group.h"
#if USE_GAUGE
#include "wx_gauge.h"
#endif
#include "wx_txt.h"
#include "wx_mtxt.h"
#include "wx_cmdlg.h"
#endif

#if USE_SCROLLBAR
#include "wx_scrol.h"
#endif

#if USE_COMBOBOX
#include "wx_combo.h"
#endif

#if USE_ENHANCED_DIALOG
#include "wx_enhdg.h"
#endif

#if USE_WX_RESOURCES

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include "wx_res.h"
#include "../../utils/prologio/src/read.h"
#include "wxstring.h"

// Forward (private) declarations
Bool wxResourceInterpretResources(wxResourceTable& table, PrologDatabase& db);
wxItemResource *wxResourceInterpretDialog(wxResourceTable& table, PrologExpr *expr, Bool isPanel = FALSE);
wxItemResource *wxResourceInterpretControl(wxResourceTable& table, PrologExpr *expr);
wxItemResource *wxResourceInterpretMenu(wxResourceTable& table, PrologExpr *expr);
wxItemResource *wxResourceInterpretMenuBar(wxResourceTable& table, PrologExpr *expr);
wxItemResource *wxResourceInterpretString(wxResourceTable& table, PrologExpr *expr);
wxItemResource *wxResourceInterpretBitmap(wxResourceTable& table, PrologExpr *expr);
wxItemResource *wxResourceInterpretIcon(wxResourceTable& table, PrologExpr *expr);
// Interpret list expression
wxFont *wxResourceInterpretFontSpec(PrologExpr *expr);

Bool wxResourceReadOneResource(FILE *fd, PrologDatabase& db, Bool *eof, wxResourceTable *table = NULL);
Bool wxResourceParseIncludeFile(char *f, wxResourceTable *table = NULL);

wxResourceTable wxDefaultResourceTable;

static char *wxResourceBuffer = NULL;
static long wxResourceBufferSize = 0;
static long wxResourceBufferCount = 0;
static int wxResourceStringPtr = 0;

void wxDeleteResourceBuffer(void)
{
  if (!wxResourceBuffer)
  { 
    delete[] wxResourceBuffer; 
    wxResourceBuffer   = NULL; 
    wxResourceBufferSize = 0; 
  } 
} 

void wxWarning(char *msg)
{
  wxMessageBox(msg, "Warning", wxOK);
}

wxItemResource::wxItemResource(void)
{
  itemType = NULL;
  title = NULL;
  name = NULL;
  windowStyle = 0;
  x = y = width = height = 0;
  value1 = value2 = value3 = value5 = 0;
  value4 = NULL;
  stringValues = NULL;
  bitmap = NULL;
  backgroundColour = labelColour = buttonColour = NULL;
  buttonFont = labelFont = NULL;
}

wxItemResource::~wxItemResource(void)
{
  if (itemType) delete[] itemType;
  if (title) delete[] title;
  if (name) delete[] name;
  if (value4) delete[] value4;
  if (stringValues)
    delete stringValues;
  if (bitmap)
    delete bitmap;
  if (backgroundColour)
    delete backgroundColour;
  if (labelColour)
    delete labelColour;
  if (buttonColour)
    delete buttonColour;
  wxNode *node = children.First();
  while (node)
  {
    wxItemResource *item = (wxItemResource *)node->Data();
    delete item;
    delete node;
    node = children.First();
  }
}

void wxItemResource::SetTitle(char *t)
{
  if (t == title)
    return;
    
  if (title) delete[] title;
  if (t)
    title = copystring(t);
  else
    title = NULL;
}

void wxItemResource::SetType(char *t)
{
  if (itemType == t)
    return;
    
  if (itemType) delete[] itemType;
  if (t)
    itemType = copystring(t);
  else
    itemType = NULL;
}

void wxItemResource::SetName(char *n)
{
  if (n == name)
    return;
    
  if (name) delete[] name;
  if (n)
    name = copystring(n);
  else
    name = NULL;
}

void wxItemResource::SetStringValues(wxStringList *svalues)
{
  if (stringValues)
    delete stringValues;
  if (svalues)
    stringValues = svalues;
  else
    stringValues = NULL;
}

void wxItemResource::SetValue4(char *v)
{
  if (value4 == v)
    return;

  if (value4) delete[] value4;
  if (v)
    value4 = copystring(v);
  else
    value4 = NULL;
}

/*
 * Resource table
 */
 
wxResourceTable::wxResourceTable(void):wxHashTable(wxKEY_STRING), identifiers(wxKEY_STRING)
{
}

wxResourceTable::~wxResourceTable(void)
{
  ClearTable();
}
    
wxItemResource *wxResourceTable::FindResource(char *name)
{
  wxItemResource *item = (wxItemResource *)Get(name);
  return item;
}

void wxResourceTable::AddResource(wxItemResource *item)
{
  char *name = item->GetName();
  if (!name)
    name = item->GetTitle();
  if (!name)
    name = "no name";

  // Delete existing resource, if any.
  Delete(name);

  Put(name, item);
}

Bool wxResourceTable::DeleteResource(char *name)
{
  wxItemResource *item = (wxItemResource *)Delete(name);
  if (item)
  {
    // See if any resource has this as its child; if so, delete from
    // parent's child list.
    BeginFind();
    wxNode *node = NULL;
    while (node = Next())
    {
      wxItemResource *parent = (wxItemResource *)node->Data();
      if (parent->GetChildren().Member(item))
      {
        parent->GetChildren().DeleteObject(item);
        break;
      }
    }
    
    delete item;
    return TRUE;
  }
  else
    return FALSE;
}

Bool wxResourceTable::ParseResourceFile(char *filename)
{
  PrologDatabase db;

  FILE *fd = fopen(filename, "r");
  if (!fd)
    return FALSE;
  Bool eof = FALSE;
  while (wxResourceReadOneResource(fd, db, &eof, this) && !eof)
  {
    // Loop
  }
  fclose(fd);
  return wxResourceInterpretResources(*this, db);
}

Bool wxResourceTable::ParseResourceData(char *data)
{
  PrologDatabase db;
  if (!db.ReadPrologFromString(data))
  {
    wxWarning("Ill-formed resource file syntax.");
    return FALSE;
  }

  return wxResourceInterpretResources(*this, db);
}

Bool wxResourceTable::RegisterResourceBitmapData(char *name, char bits[], int width, int height)
{
  // Register pre-loaded bitmap data
  wxItemResource *item = new wxItemResource;
//  item->SetType(wxRESOURCE_TYPE_XBM_DATA);
  item->SetType("wxXBMData");
  item->SetName(name);
  item->SetValue1((long)bits);
  item->SetValue2((long)width);
  item->SetValue3((long)height);
  AddResource(item);
  return TRUE;
}

Bool wxResourceTable::RegisterResourceBitmapData(char *name, char **data)
{
  // Register pre-loaded bitmap data
  wxItemResource *item = new wxItemResource;
//  item->SetType(wxRESOURCE_TYPE_XPM_DATA);
  item->SetType("wxXPMData");
  item->SetName(name);
  item->SetValue1((long)data);
  AddResource(item);
  return TRUE;
}

Bool wxResourceTable::SaveResource(char *WXUNUSED(filename))
{
  return FALSE;
}

void wxResourceTable::ClearTable(void)
{
  BeginFind();
  wxNode *node = Next();
  while (node)
  {
    wxNode *next = Next();
    wxItemResource *item = (wxItemResource *)node->Data();
    delete item;
    delete node;
    node = next;
  }
}

wxItem *wxResourceTable::CreateItem(wxPanel *panel, wxItemResource *childResource)
{
  wxItem *control = NULL;
  wxString itemType(childResource->GetType());
  if (itemType == wxString("wxButton"))
      {
        if (childResource->GetValue4())
        {
          // Bitmap button
          wxBitmap *bitmap = childResource->GetBitmap();
          if (!bitmap)
          {
            bitmap = wxResourceCreateBitmap(childResource->GetValue4(), this);
            childResource->SetBitmap(bitmap);
          }
          if (bitmap)
           control = new wxButton(panel, (wxFunction)NULL, bitmap,
             childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
             childResource->GetStyle(), childResource->GetName());
        }
        else
          // Normal, text button
          control = new wxButton(panel, (wxFunction)NULL, childResource->GetTitle(),
           childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
           childResource->GetStyle(), childResource->GetName());
      }
   else if (itemType == wxString("wxMessage"))
      {
        if (childResource->GetValue4())
        {
          // Bitmap message
          wxBitmap *bitmap = childResource->GetBitmap();
          if (!bitmap)
          {
            bitmap = wxResourceCreateBitmap(childResource->GetValue4(), this);
            childResource->SetBitmap(bitmap);
          }
#if USE_BITMAP_MESSAGE
          if (bitmap)
           control = new wxMessage(panel, bitmap,
             childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
             childResource->GetStyle(), childResource->GetName());
#endif
        }
        else
        {
           control = new wxMessage(panel, childResource->GetTitle(),
             childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
             childResource->GetStyle(), childResource->GetName());
        }
      }
   else if (itemType == wxString("wxText"))
      {
        control = new wxText(panel, (wxFunction)NULL, childResource->GetTitle(), childResource->GetValue4(),
           childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
           childResource->GetStyle(), childResource->GetName());
      }
   else if (itemType == wxString("wxMultiText"))
      {
        control = new wxMultiText(panel, (wxFunction)NULL, childResource->GetTitle(), childResource->GetValue4(),
           childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
           childResource->GetStyle(), childResource->GetName());
      }
   else if (itemType == wxString("wxCheckBox"))
      {
        control = new wxCheckBox(panel, (wxFunction)NULL, childResource->GetTitle(),
           childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
           childResource->GetStyle(), childResource->GetName());
        ((wxCheckBox *)control)->SetValue((Bool)childResource->GetValue1());
      }
#if USE_GAUGE
   else if (itemType == wxString("wxGauge"))
      {
        control = new wxGauge(panel, childResource->GetTitle(), (int)childResource->GetValue2(),
           childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
           childResource->GetStyle(), childResource->GetName());
        ((wxGauge *)control)->SetValue((int)childResource->GetValue1());
      }
#endif
#if USE_RADIOBUTTON
   else if (itemType == wxString("wxRadioButton"))
      {
        control = new wxRadioButton(panel, (wxFunction)NULL, childResource->GetTitle(), (int)childResource->GetValue1(),
           childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
           childResource->GetStyle(), childResource->GetName());
      }
#endif
#if USE_SCROLLBAR
   else if (itemType == wxString("wxScrollBar"))
      {
        control = new wxScrollBar(panel, (wxFunction)NULL,
           childResource->GetX(), childResource->GetY(),
           childResource->GetWidth(), childResource->GetHeight(), 
           childResource->GetStyle(), childResource->GetName());
        ((wxScrollBar *)control)->SetValue((int)childResource->GetValue1());
        ((wxScrollBar *)control)->SetPageLength((int)childResource->GetValue2());
        ((wxScrollBar *)control)->SetObjectLength((int)childResource->GetValue3());
        ((wxScrollBar *)control)->SetViewLength((int)(long)childResource->GetValue5());
      }
#endif
   else if (itemType == wxString("wxSlider"))
      {
        control = new wxSlider(panel, (wxFunction)NULL, childResource->GetTitle(), (int)childResource->GetValue1(),
           (int)childResource->GetValue2(), (int)childResource->GetValue3(),
           childResource->GetWidth(), childResource->GetX(), childResource->GetY(),
           childResource->GetStyle(), childResource->GetName());
      }
   else if (itemType == wxString("wxGroupBox"))
      {
        control = new wxGroupBox(panel, childResource->GetTitle(),
           childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
           childResource->GetStyle(), childResource->GetName());
      }
   else if (itemType == wxString("wxListBox"))
      {
        wxStringList *stringList = childResource->GetStringValues();
        char **strings = NULL;
        int noStrings = 0;
        if (stringList && (stringList->Number() > 0))
        {
          noStrings = stringList->Number();
          strings = new char *[noStrings];
          wxNode *node = stringList->First();
          int i = 0;
          while (node)
          {
            strings[i] = (char *)node->Data();
            i ++;
            node = node->Next();
          }
        }
        control = new wxListBox(panel, (wxFunction)NULL, childResource->GetTitle(), (int)childResource->GetValue1(),
           childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
           noStrings, strings, childResource->GetStyle(), childResource->GetName());

        if (strings)
          delete[] strings;
      }
   else if (itemType == wxString("wxChoice"))
      {
        wxStringList *stringList = childResource->GetStringValues();
        char **strings = NULL;
        int noStrings = 0;
        if (stringList && (stringList->Number() > 0))
        {
          noStrings = stringList->Number();
          strings = new char *[noStrings];
          wxNode *node = stringList->First();
          int i = 0;
          while (node)
          {
            strings[i] = (char *)node->Data();
            i ++;
            node = node->Next();
          }
        }
        control = new wxChoice(panel, (wxFunction)NULL, childResource->GetTitle(),
           childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
           noStrings, strings, childResource->GetStyle(), childResource->GetName());

        if (strings)
          delete[] strings;
      }
#if USE_COMBOBOX
   else if (itemType == wxString("wxComboBox"))
      {
        wxStringList *stringList = childResource->GetStringValues();
        char **strings = NULL;
        int noStrings = 0;
        if (stringList && (stringList->Number() > 0))
        {
          noStrings = stringList->Number();
          strings = new char *[noStrings];
          wxNode *node = stringList->First();
          int i = 0;
          while (node)
          {
            strings[i] = (char *)node->Data();
            i ++;
            node = node->Next();
          }
        }
        control = new wxComboBox(panel, (wxFunction)NULL, childResource->GetTitle(), childResource->GetValue4(),
           childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
           noStrings, strings, childResource->GetStyle(), childResource->GetName());

        if (strings)
          delete[] strings;
      }
#endif
   else if (itemType == wxString("wxRadioBox"))
      {
        wxStringList *stringList = childResource->GetStringValues();
        char **strings = NULL;
        int noStrings = 0;
        if (stringList && (stringList->Number() > 0))
        {
          noStrings = stringList->Number();
          strings = new char *[noStrings];
          wxNode *node = stringList->First();
          int i = 0;
          while (node)
          {
            strings[i] = (char *)node->Data();
            i ++;
            node = node->Next();
          }
        }
        control = new wxRadioBox(panel, (wxFunction)NULL, childResource->GetTitle(),
           childResource->GetX(), childResource->GetY(), childResource->GetWidth(), childResource->GetHeight(),
           noStrings, strings, (int)childResource->GetValue1(), childResource->GetStyle(), childResource->GetName());

        if (strings)
          delete[] strings;
      }
  if (control && childResource->GetLabelFont())
    control->SetLabelFont(childResource->GetLabelFont());
  if (control && childResource->GetButtonFont())
    control->SetButtonFont(childResource->GetButtonFont());
  return control;
}

/*
 * Interpret database as a series of resources
 */

Bool wxResourceInterpretResources(wxResourceTable& table, PrologDatabase& db)
{
  wxNode *node = db.First();
  while (node)
  {
    PrologExpr *clause = (PrologExpr *)node->Data();
    char *functor = clause->Functor();

    wxItemResource *item = NULL;
    if (strcmp(functor, "dialog") == 0)
      item = wxResourceInterpretDialog(table, clause);
    else if (strcmp(functor, "panel") == 0)
      item = wxResourceInterpretDialog(table, clause, TRUE);
    else if (strcmp(functor, "menubar") == 0)
      item = wxResourceInterpretMenuBar(table, clause);
    else if (strcmp(functor, "menu") == 0)
      item = wxResourceInterpretMenu(table, clause);
    else if (strcmp(functor, "string") == 0)
      item = wxResourceInterpretString(table, clause);
    else if (strcmp(functor, "bitmap") == 0)
      item = wxResourceInterpretBitmap(table, clause);
    else if (strcmp(functor, "icon") == 0)
      item = wxResourceInterpretIcon(table, clause);

    if (item)
    {
      // Remove any existing resource of same name
      if (item->GetName())
        table.DeleteResource(item->GetName());
      table.AddResource(item);
    }
    node = node->Next();
  }
  return TRUE;
}

wxItemResource *wxResourceInterpretDialog(wxResourceTable& table, PrologExpr *expr, Bool isPanel)
{
  wxItemResource *dialogItem = new wxItemResource;
  if (isPanel)
    dialogItem->SetType("wxPanel");
  else
    dialogItem->SetType("wxDialogBox");
  char *style = NULL;
  char *title = NULL;
  char *name = NULL;
  char *backColourHex = NULL;
  char *labelColourHex = NULL;
  char *buttonColourHex = NULL;

  long windowStyle = wxDEFAULT_DIALOG_STYLE;
  if (isPanel)
    windowStyle = 0;
    
  int x = 0; int y = 0; int width = -1; int height = -1;
  int isModal = 0;
  PrologExpr *labelFontExpr = NULL;
  PrologExpr *buttonFontExpr = NULL;
  expr->AssignAttributeValue("style", &style);
  expr->AssignAttributeValue("name", &name);
  expr->AssignAttributeValue("title", &title);
  expr->AssignAttributeValue("x", &x);
  expr->AssignAttributeValue("y", &y);
  expr->AssignAttributeValue("width", &width);
  expr->AssignAttributeValue("height", &height);
  expr->AssignAttributeValue("modal", &isModal);
  expr->AssignAttributeValue("label_font", &labelFontExpr);
  expr->AssignAttributeValue("button_font", &buttonFontExpr);
  expr->AssignAttributeValue("background_colour", &backColourHex);
  expr->AssignAttributeValue("label_colour", &labelColourHex);
  expr->AssignAttributeValue("button_colour", &buttonColourHex);

  if (style)
  {
    windowStyle = wxParseWindowStyle(style);
  }
  dialogItem->SetStyle(windowStyle);
  dialogItem->SetValue1(isModal);
  if (name)
    dialogItem->SetName(name);
  if (title)
    dialogItem->SetTitle(title);
  dialogItem->SetSize(x, y, width, height);
  
  if (backColourHex)
  {
    int r = 0;
    int g = 0;
    int b = 0;
    r = wxHexToDec(backColourHex);
    g = wxHexToDec(backColourHex+2);
    b = wxHexToDec(backColourHex+4);
    dialogItem->SetBackgroundColour(new wxColour((unsigned char)r,(unsigned char)g,(unsigned char)b));
    delete[] backColourHex;
  }
  if (labelColourHex)
  {
    int r = 0;
    int g = 0;
    int b = 0;
    r = wxHexToDec(labelColourHex);
    g = wxHexToDec(labelColourHex+2);
    b = wxHexToDec(labelColourHex+4);
    dialogItem->SetLabelColour(new wxColour((unsigned char)r,(unsigned char)g,(unsigned char)b));
    delete[] labelColourHex;
  }
  if (buttonColourHex)
  {
    int r = 0;
    int g = 0;
    int b = 0;
    r = wxHexToDec(buttonColourHex);
    g = wxHexToDec(buttonColourHex+2);
    b = wxHexToDec(buttonColourHex+4);
    dialogItem->SetButtonColour(new wxColour((unsigned char)r,(unsigned char)g,(unsigned char)b));
    delete[] buttonColourHex;
  }

  if (name)
    delete[] name;
  if (title)
    delete[] title;
  if (style)
    delete[] style;

  if (buttonFontExpr)
    dialogItem->SetButtonFont(wxResourceInterpretFontSpec(buttonFontExpr));
  if (labelFontExpr)
    dialogItem->SetLabelFont(wxResourceInterpretFontSpec(labelFontExpr));

  // Now parse all controls
  PrologExpr *controlExpr = expr->GetFirst();
  while (controlExpr)
  {
    if (controlExpr->Number() == 3)
    {
      char *controlKeyword = controlExpr->Nth(1)->StringValue();
      if (controlKeyword && (strcmp(controlKeyword, "control") == 0))
      {
        // The value part: always a list.
        PrologExpr *listExpr = controlExpr->Nth(2);
        if (listExpr->Type() == PrologList)
        {
          wxItemResource *controlItem = wxResourceInterpretControl(table, listExpr);
          if (controlItem)
          {
            dialogItem->GetChildren().Append(controlItem);
          }
        }
      }
    }
    controlExpr = controlExpr->GetNext();
  }
  return dialogItem;
}

wxItemResource *wxResourceInterpretControl(wxResourceTable& WXUNUSED(table), PrologExpr *expr)
{
  char *controlType = expr->Nth(0)->StringValue();
  if (!controlType)
    return NULL;
  wxItemResource *controlItem = new wxItemResource;

  // First, find the standard features of a control definition:
  // title (1), style (2), name (3), x (4), y (5), width (6), height (7)
  PrologExpr *arg1 = expr->Nth(1);
  PrologExpr *arg2 = expr->Nth(2);
  PrologExpr *arg3 = expr->Nth(3);
  PrologExpr *arg4 = expr->Nth(4);
  PrologExpr *arg5 = expr->Nth(5);
  PrologExpr *arg6 = expr->Nth(6);
  PrologExpr *arg7 = expr->Nth(7);

  char *style = NULL;
  char *title = NULL;
  char *name = NULL;
  long windowStyle = 0;
  int x = 0; int y = 0; int width = -1; int height = -1;
  if (arg1)
    title = arg1->StringValue();
  if (arg2)
  {
    style = arg2->StringValue();
    if (style)
      windowStyle = wxParseWindowStyle(style);
  }
  if (arg3)
    name = arg3->StringValue();
  if (arg4)
    x = (int)arg4->IntegerValue();
  if (arg5)
    y = (int)arg5->IntegerValue();
  if (arg6)
    width = (int)arg6->IntegerValue();
  if (arg7)
    height = (int)arg7->IntegerValue();
  controlItem->SetStyle(windowStyle);
  controlItem->SetName(name);
  controlItem->SetTitle(title);
  controlItem->SetSize(x, y, width, height);

  
  controlItem->SetType(controlType);
  if (strcmp(controlType, "wxButton") == 0)
  {
    int count = 8;
//    controlItem->SetType(wxTYPE_BUTTON);
    
    // Check for bitmap resource name
    if (expr->Nth(8) && ((expr->Nth(8)->Type() == PrologString) || (expr->Nth(8)->Type() == PrologWord)))
    {
      controlItem->SetValue4(expr->Nth(8)->StringValue());
      count ++;
    }
    if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
      controlItem->SetButtonFont(wxResourceInterpretFontSpec(expr->Nth(count)));
  }
  else if (strcmp(controlType, "wxCheckBox") == 0)
  {
//    controlItem->SetType(wxTYPE_CHECK_BOX);
    // Check for default value
    if (expr->Nth(8) && (expr->Nth(8)->Type() == PrologInteger))
      controlItem->SetValue1(expr->Nth(8)->IntegerValue());
    if (expr->Nth(9) && expr->Nth(9)->Type() == PrologList)
      controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(9)));
  }
#if USE_RADIOBUTTON
  else if (strcmp(controlType, "wxRadioButton") == 0)
  {
    // Check for default value
    if (expr->Nth(8) && (expr->Nth(8)->Type() == PrologInteger))
      controlItem->SetValue1(expr->Nth(8)->IntegerValue());
    if (expr->Nth(9) && expr->Nth(9)->Type() == PrologList)
      controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(9)));
  }
#endif
  else if (strcmp(controlType, "wxText") == 0)
  {
//    controlItem->SetType(wxTYPE_TEXT);
    // Check for default value
    if (expr->Nth(8) && ((expr->Nth(8)->Type() == PrologString) || (expr->Nth(8)->Type() == PrologWord)))
      controlItem->SetValue4(expr->Nth(8)->StringValue());
    if (expr->Nth(9) && expr->Nth(9)->Type() == PrologList)
      controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(9)));
    if (expr->Nth(10) && expr->Nth(10)->Type() == PrologList)
      controlItem->SetButtonFont(wxResourceInterpretFontSpec(expr->Nth(10)));
  }
  else if (strcmp(controlType, "wxMultiText") == 0)
  {
//    controlItem->SetType(wxTYPE_MULTI_TEXT);
    // Check for default value
    if (expr->Nth(8) && ((expr->Nth(8)->Type() == PrologString) || (expr->Nth(8)->Type() == PrologWord)))
      controlItem->SetValue4(expr->Nth(8)->StringValue());
    if (expr->Nth(9) && expr->Nth(9)->Type() == PrologList)
      controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(9)));
    if (expr->Nth(10) && expr->Nth(10)->Type() == PrologList)
      controlItem->SetButtonFont(wxResourceInterpretFontSpec(expr->Nth(10)));
  }
  else if (strcmp(controlType, "wxMessage") == 0)
  {
    int count = 8;

//    controlItem->SetType(wxTYPE_MESSAGE);

    // Check for bitmap resource name
    if (expr->Nth(8) && ((expr->Nth(8)->Type() == PrologString) || (expr->Nth(8)->Type() == PrologWord)))
    {
      controlItem->SetValue4(expr->Nth(8)->StringValue());
      count ++;
    }
    if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
      controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(count)));
  }
  else if (strcmp(controlType, "wxGroupBox") == 0)
  {
//    controlItem->SetType(wxTYPE_GROUP_BOX);
    if (expr->Nth(8) && expr->Nth(8)->Type() == PrologList)
      controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(8)));
  }
  else if (strcmp(controlType, "wxGauge") == 0)
  {
//    controlItem->SetType(wxTYPE_GAUGE);
    // Check for default value
    if (expr->Nth(8) && (expr->Nth(8)->Type() == PrologInteger))
      controlItem->SetValue1(expr->Nth(8)->IntegerValue());
    // Check for range
    if (expr->Nth(9) && (expr->Nth(9)->Type() == PrologInteger))
      controlItem->SetValue2(expr->Nth(9)->IntegerValue());
    if (expr->Nth(10) && expr->Nth(10)->Type() == PrologList)
      controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(10)));
    if (expr->Nth(11) && expr->Nth(11)->Type() == PrologList)
      controlItem->SetButtonFont(wxResourceInterpretFontSpec(expr->Nth(11)));
  }
  else if (strcmp(controlType, "wxSlider") == 0)
  {
//    controlItem->SetType(wxTYPE_SLIDER);
    // Check for default value
    if (expr->Nth(8) && (expr->Nth(8)->Type() == PrologInteger))
      controlItem->SetValue1(expr->Nth(8)->IntegerValue());
    // Check for min
    if (expr->Nth(9) && (expr->Nth(9)->Type() == PrologInteger))
      controlItem->SetValue2(expr->Nth(9)->IntegerValue());
    // Check for max
    if (expr->Nth(10) && (expr->Nth(10)->Type() == PrologInteger))
      controlItem->SetValue3(expr->Nth(10)->IntegerValue());
    if (expr->Nth(11) && expr->Nth(11)->Type() == PrologList)
      controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(11)));
    if (expr->Nth(12) && expr->Nth(12)->Type() == PrologList)
      controlItem->SetButtonFont(wxResourceInterpretFontSpec(expr->Nth(12)));
  }
  else if (strcmp(controlType, "wxScrollBar") == 0)
  {
//    controlItem->SetType(wxTYPE_SCROLL_BAR);
    // DEFAULT VALUE
    if (expr->Nth(8) && (expr->Nth(8)->Type() == PrologInteger))
      controlItem->SetValue1(expr->Nth(8)->IntegerValue());
    // PAGE LENGTH
    if (expr->Nth(9) && (expr->Nth(9)->Type() == PrologInteger))
      controlItem->SetValue2(expr->Nth(9)->IntegerValue());
    // OBJECT LENGTH
    if (expr->Nth(10) && (expr->Nth(10)->Type() == PrologInteger))
      controlItem->SetValue3(expr->Nth(10)->IntegerValue());
    // VIEW LENGTH
    if (expr->Nth(11) && (expr->Nth(11)->Type() == PrologInteger))
      controlItem->SetValue5(expr->Nth(11)->IntegerValue());
  }
  else if (strcmp(controlType, "wxListBox") == 0)
  {
//    controlItem->SetType(wxTYPE_LIST_BOX);
    PrologExpr *valueList = NULL;

    if ((valueList = expr->Nth(8)) && (valueList->Type() == PrologList))
    {
      wxStringList *stringList = new wxStringList;
      PrologExpr *stringExpr = valueList->GetFirst();
      while (stringExpr)
      {
        stringList->Add(stringExpr->StringValue());
        stringExpr = stringExpr->GetNext();
      }
      controlItem->SetStringValues(stringList);
    }
    // Check for wxSINGLE/wxMULTIPLE
    PrologExpr *mult = NULL;
    controlItem->SetValue1(wxSINGLE);
    if ((mult = expr->Nth(9)) && ((mult->Type() == PrologString)||(mult->Type() == PrologWord)))
    {
      char *m = mult->StringValue();
      if (strcmp(m, "wxMULTIPLE") == 0)
        controlItem->SetValue1(wxMULTIPLE);
      else if (strcmp(m, "wxEXTENDED") == 0)
        controlItem->SetValue1(wxEXTENDED);
    }
    if (expr->Nth(10) && expr->Nth(10)->Type() == PrologList)
      controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(10)));
    if (expr->Nth(11) && expr->Nth(11)->Type() == PrologList)
      controlItem->SetButtonFont(wxResourceInterpretFontSpec(expr->Nth(11)));
  }
  else if (strcmp(controlType, "wxChoice") == 0)
  {
//    controlItem->SetType(wxTYPE_CHOICE);
    PrologExpr *valueList = NULL;
    // Check for default value list
    if ((valueList = expr->Nth(8)) && (valueList->Type() == PrologList))
    {
      wxStringList *stringList = new wxStringList;
      PrologExpr *stringExpr = valueList->GetFirst();
      while (stringExpr)
      {
        stringList->Add(stringExpr->StringValue());
        stringExpr = stringExpr->GetNext();
      }
      controlItem->SetStringValues(stringList);
    }
    if (expr->Nth(9) && expr->Nth(9)->Type() == PrologList)
      controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(9)));
    if (expr->Nth(10) && expr->Nth(10)->Type() == PrologList)
      controlItem->SetButtonFont(wxResourceInterpretFontSpec(expr->Nth(10)));
  }
#if USE_COMBOBOX
  else if (strcmp(controlType, "wxComboBox") == 0)
  {
    PrologExpr *textValue = expr->Nth(8);
    if (textValue && (textValue->Type() == PrologString || textValue->Type() == PrologWord))
      controlItem->SetValue4(textValue->StringValue());
      
    PrologExpr *valueList = NULL;
    // Check for default value list
    if ((valueList = expr->Nth(9)) && (valueList->Type() == PrologList))
    {
      wxStringList *stringList = new wxStringList;
      PrologExpr *stringExpr = valueList->GetFirst();
      while (stringExpr)
      {
        stringList->Add(stringExpr->StringValue());
        stringExpr = stringExpr->GetNext();
      }
      controlItem->SetStringValues(stringList);
    }
    if (expr->Nth(10) && expr->Nth(10)->Type() == PrologList)
      controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(10)));
    if (expr->Nth(11) && expr->Nth(11)->Type() == PrologList)
      controlItem->SetButtonFont(wxResourceInterpretFontSpec(expr->Nth(11)));
  }
#endif
  else if (strcmp(controlType, "wxRadioBox") == 0)
  {
//    controlItem->SetType(wxTYPE_RADIO_BOX);
    PrologExpr *valueList = NULL;
    // Check for default value list
    if ((valueList = expr->Nth(8)) && (valueList->Type() == PrologList))
    {
      wxStringList *stringList = new wxStringList;
      PrologExpr *stringExpr = valueList->GetFirst();
      while (stringExpr)
      {
        stringList->Add(stringExpr->StringValue());
        stringExpr = stringExpr->GetNext();
      }
      controlItem->SetStringValues(stringList);
    }
    // majorDim (number of rows or cols)
    if (expr->Nth(9) && (expr->Nth(9)->Type() == PrologInteger))
      controlItem->SetValue1(expr->Nth(9)->IntegerValue());
    else
      controlItem->SetValue1(0);

    if (expr->Nth(10) && expr->Nth(10)->Type() == PrologList)
      controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(10)));
    if (expr->Nth(11) && expr->Nth(11)->Type() == PrologList)
      controlItem->SetButtonFont(wxResourceInterpretFontSpec(expr->Nth(11)));
  }
  else
  {
    delete controlItem;
    return NULL;
  }
  return controlItem;
}

// Forward declaration 
wxItemResource *wxResourceInterpretMenu1(wxResourceTable& table, PrologExpr *expr);

/*
 * Interpet a menu item
 */

wxItemResource *wxResourceInterpretMenuItem(wxResourceTable& table, PrologExpr *expr)
{
  wxItemResource *item = new wxItemResource;
  
  PrologExpr *labelExpr = expr->Nth(0);
  PrologExpr *idExpr = expr->Nth(1);
  PrologExpr *helpExpr = expr->Nth(2);
  PrologExpr *checkableExpr = expr->Nth(3);

  // Further keywords/attributes to follow sometime...
  if (expr->Number() == 0)
  {
//    item->SetType(wxRESOURCE_TYPE_SEPARATOR);
    item->SetType("wxMenuSeparator");
    return item;
  }
  else
  {
//    item->SetType(wxTYPE_MENU); // Well, menu item, but doesn't matter.
    item->SetType("wxMenu"); // Well, menu item, but doesn't matter.
    if (labelExpr)
    {
      item->SetTitle(labelExpr->StringValue());
    }
    if (idExpr)
    {
      int id = 0;
      // If a string or word, must look up in identifier table.
      if ((idExpr->Type() == PrologString) || (idExpr->Type() == PrologWord))
      {
        id = wxResourceGetIdentifier(idExpr->StringValue(), &table);
        if (id == 0)
        {
          char buf[300];
	  sprintf(buf, "Could not resolve menu id '%s'. Use (non-zero) integer instead\n or provide #define (see manual for caveats)",
                 idExpr->StringValue());
          wxWarning(buf);
        }
      }
      else if (idExpr->Type() == PrologInteger)
        id = (int)idExpr->IntegerValue();
      item->SetValue1(id);
    }
    if (helpExpr)
    {
      item->SetValue4(helpExpr->StringValue());
    }
    if (checkableExpr)
      item->SetValue2(checkableExpr->IntegerValue());

    // Find the first expression that's a list, for submenu
    PrologExpr *subMenuExpr = expr->GetFirst();
    while (subMenuExpr && (subMenuExpr->Type() != PrologList))
      subMenuExpr = subMenuExpr->GetNext();
      
    while (subMenuExpr)
    {
      wxItemResource *child = wxResourceInterpretMenuItem(table, subMenuExpr);
      item->GetChildren().Append(child);
      subMenuExpr = subMenuExpr->GetNext();
    }
  }
  return item;
}

/*
 * Interpret a nested list as a menu
 */
/*
wxItemResource *wxResourceInterpretMenu1(wxResourceTable& table, PrologExpr *expr)
{
  wxItemResource *menu = new wxItemResource;
//  menu->SetType(wxTYPE_MENU);
  menu->SetType("wxMenu");
  PrologExpr *element = expr->GetFirst();
  while (element)
  {
    wxItemResource *item = wxResourceInterpretMenuItem(table, element);
    if (item)
      menu->GetChildren().Append(item);
    element = element->GetNext();
  }
  return menu;
}
*/

wxItemResource *wxResourceInterpretMenu(wxResourceTable& table, PrologExpr *expr)
{
  PrologExpr *listExpr = NULL;
  expr->AssignAttributeValue("menu", &listExpr);
  if (!listExpr)
    return NULL;
  
  wxItemResource *menuResource = wxResourceInterpretMenuItem(table, listExpr);

  if (!menuResource)
    return NULL;
    
  char *name = NULL;
  expr->AssignAttributeValue("name", &name);
  if (name)
  {
    menuResource->SetName(name);
    delete[] name;
  }
  
  return menuResource;
}

wxItemResource *wxResourceInterpretMenuBar(wxResourceTable& table, PrologExpr *expr)
{
  PrologExpr *listExpr = NULL;
  expr->AssignAttributeValue("menu", &listExpr);
  if (!listExpr)
    return NULL;

  wxItemResource *resource = new wxItemResource;
  resource->SetType("wxMenu");
//  resource->SetType(wxTYPE_MENU);
  
  PrologExpr *element = listExpr->GetFirst();
  while (element)
  {
    wxItemResource *menuResource = wxResourceInterpretMenuItem(table, listExpr);
    resource->GetChildren().Append(menuResource);
    element = element->GetNext();
  }

  char *name = NULL;
  expr->AssignAttributeValue("name", &name);
  if (name)
  {
    resource->SetName(name);
    delete[] name;
  }
  
  return resource;
}

wxItemResource *wxResourceInterpretString(wxResourceTable& WXUNUSED(table), PrologExpr *WXUNUSED(expr))
{
  return NULL;
}

wxItemResource *wxResourceInterpretBitmap(wxResourceTable& WXUNUSED(table), PrologExpr *expr)
{
  wxItemResource *bitmapItem = new wxItemResource;
//  bitmapItem->SetType(wxTYPE_BITMAP);
  bitmapItem->SetType("wxBitmap");
  char *name = NULL;
  expr->AssignAttributeValue("name", &name);
  if (name)
  {
    bitmapItem->SetName(name);
    delete[] name;
  }
  // Now parse all bitmap specifications
  PrologExpr *bitmapExpr = expr->GetFirst();
  while (bitmapExpr)
  {
    if (bitmapExpr->Number() == 3)
    {
      char *bitmapKeyword = bitmapExpr->Nth(1)->StringValue();
      if (bitmapKeyword && ((strcmp(bitmapKeyword, "bitmap") == 0)||(strcmp(bitmapKeyword, "icon") == 0)))
      {
        // The value part: always a list.
        PrologExpr *listExpr = bitmapExpr->Nth(2);
        if (listExpr->Type() == PrologList)
        {
          wxItemResource *bitmapSpec = new wxItemResource;
//          bitmapSpec->SetType(wxTYPE_BITMAP);
          bitmapSpec->SetType("wxBitmap");

          // List is of form: [filename, bitmaptype, platform, colours, xresolution, yresolution]
          // where everything after 'filename' is optional.
          PrologExpr *nameExpr = listExpr->Nth(0);
          PrologExpr *typeExpr = listExpr->Nth(1);
          PrologExpr *platformExpr = listExpr->Nth(2);
          PrologExpr *coloursExpr = listExpr->Nth(3);
          PrologExpr *xresExpr = listExpr->Nth(4);
          PrologExpr *yresExpr = listExpr->Nth(5);
          if (nameExpr && nameExpr->StringValue())
            bitmapSpec->SetName(nameExpr->StringValue());
          if (typeExpr && typeExpr->StringValue())
            bitmapSpec->SetValue1(wxParseWindowStyle(typeExpr->StringValue()));
          else
            bitmapSpec->SetValue1(0);
            
          if (platformExpr && platformExpr->StringValue())
          {
            char *plat = platformExpr->StringValue();
            if ((strcmp(plat, "windows") == 0) || (strcmp(plat, "WINDOWS") == 0))
              bitmapSpec->SetValue2(RESOURCE_PLATFORM_WINDOWS);
            else if ((strcmp(plat, "x") == 0) || (strcmp(plat, "X") == 0))
              bitmapSpec->SetValue2(RESOURCE_PLATFORM_X);
            else if ((strcmp(plat, "mac") == 0) || (strcmp(plat, "MAC") == 0))
              bitmapSpec->SetValue2(RESOURCE_PLATFORM_MAC);
            else
              bitmapSpec->SetValue2(RESOURCE_PLATFORM_ANY);
          }
          else
            bitmapSpec->SetValue2(RESOURCE_PLATFORM_ANY);

          if (coloursExpr)
            bitmapSpec->SetValue3(coloursExpr->IntegerValue());
          int xres = 0;
          int yres = 0;
          if (xresExpr)
            xres = (int)xresExpr->IntegerValue();
          if (yresExpr)
            yres = (int)yresExpr->IntegerValue();
          bitmapSpec->SetSize(0, 0, xres, yres);
            
          bitmapItem->GetChildren().Append(bitmapSpec);
        }
      }
    }
    bitmapExpr = bitmapExpr->GetNext();
  }
  
  return bitmapItem;
}

wxItemResource *wxResourceInterpretIcon(wxResourceTable& table, PrologExpr *expr)
{
  wxItemResource *item = wxResourceInterpretBitmap(table, expr);
  if (item)
  {
//    item->SetType(wxTYPE_ICON);
    item->SetType("wxIcon");
    return item;
  }
  else
    return NULL;
}

// Interpret list expression as a font
wxFont *wxResourceInterpretFontSpec(PrologExpr *expr)
{
  if (expr->Type() != PrologList)
    return NULL;

  int point = 10;
  int family = wxSWISS;
  int style = wxNORMAL;
  int weight = wxNORMAL;
  int underline = 0;
  char *faceName = NULL;
  
  PrologExpr *pointExpr = expr->Nth(0);
  PrologExpr *familyExpr = expr->Nth(1);
  PrologExpr *styleExpr = expr->Nth(2);
  PrologExpr *weightExpr = expr->Nth(3);
  PrologExpr *underlineExpr = expr->Nth(4);
  PrologExpr *faceNameExpr = expr->Nth(5);
  if (pointExpr)
    point = (int)pointExpr->IntegerValue();
  if (familyExpr)
    family = (int)wxParseWindowStyle(familyExpr->StringValue());
  if (styleExpr)
    style = (int)wxParseWindowStyle(styleExpr->StringValue());
  if (weightExpr)
    weight = (int)wxParseWindowStyle(weightExpr->StringValue());
  if (underlineExpr)
    underline = (int)underlineExpr->IntegerValue();
  if (faceNameExpr)
    faceName = faceNameExpr->StringValue();
  wxFont *font = wxTheFontList->FindOrCreateFont(point, family, style, weight, underline, faceName);
  return font;
}

/*
 * (Re)allocate buffer for reading in from resource file
 */

Bool wxReallocateResourceBuffer(void)
{
  if (!wxResourceBuffer)
  {
    wxResourceBufferSize = 1000;
    wxResourceBuffer = new char[wxResourceBufferSize];
    return TRUE;
  }
  if (wxResourceBuffer)
  {
    long newSize = wxResourceBufferSize + 1000;
    char *tmp = new char[(int)newSize];
    strncpy(tmp, wxResourceBuffer, (int)wxResourceBufferCount);
    delete[] wxResourceBuffer;
    wxResourceBuffer = tmp;
    wxResourceBufferSize = newSize;
  }
  return TRUE;
}

static Bool wxEatWhiteSpace(FILE *fd)
{
  int ch = getc(fd);
  if ((ch != ' ') && (ch != '/') && (ch != ' ') && (ch != 10) && (ch != 13) && (ch != 9))
  {
    ungetc(ch, fd);
    return TRUE;
  }

  // Eat whitespace
  while (ch == ' ' || ch == 10 || ch == 13 || ch == 9)
    ch = getc(fd);
  // Check for comment
  if (ch == '/')
  {
    ch = getc(fd);
    if (ch == '*')
    {
      Bool finished = FALSE;
      while (!finished)
      {
        ch = getc(fd);
        if (ch == EOF)
          return FALSE;
        if (ch == '*')
        {
          int newCh = getc(fd);
          if (newCh == '/')
            finished = TRUE;
          else
          {
            ungetc(newCh, fd);
          }
        }
      }
    }
    else // False alarm
      return FALSE;
  }
  else
    ungetc(ch, fd);
  return wxEatWhiteSpace(fd);
}

Bool wxGetResourceToken(FILE *fd)
{
  if (!wxResourceBuffer)
    wxReallocateResourceBuffer();
  wxResourceBuffer[0] = 0;
  wxEatWhiteSpace(fd);

  int ch = getc(fd);
  if (ch == '"')
  {
    // Get string
    wxResourceBufferCount = 0;
    ch = getc(fd);
    while (ch != '"')
    {
      int actualCh = ch;
      if (ch == EOF)
      {
        wxResourceBuffer[wxResourceBufferCount] = 0;
        return FALSE;
      }
      // Escaped characters
      else if (ch == '\\')
      {
        int newCh = getc(fd);
        if (newCh == '"')
          actualCh = '"';
        else if (newCh == 10)
          actualCh = 10;
        else
        {
          ungetc(newCh, fd);
        }
      }

      if (wxResourceBufferCount >= wxResourceBufferSize-1)
        wxReallocateResourceBuffer();
      wxResourceBuffer[wxResourceBufferCount] = (char)actualCh;
      wxResourceBufferCount ++;
      ch = getc(fd);
    }
    wxResourceBuffer[wxResourceBufferCount] = 0;
  }
  else
  {
    wxResourceBufferCount = 0;
    // Any other token
    while (ch != ' ' && ch != EOF && ch != ' ' && ch != 13 && ch != 9 && ch != 10)
    {
      if (wxResourceBufferCount >= wxResourceBufferSize-1)
        wxReallocateResourceBuffer();
      wxResourceBuffer[wxResourceBufferCount] = (char)ch;
      wxResourceBufferCount ++;
      
      ch = getc(fd);
    }
    wxResourceBuffer[wxResourceBufferCount] = 0;
    if (ch == EOF)
      return FALSE;
  }
  return TRUE;
}

/*
 * Files are in form:
  static char *name = "....";
  with possible comments.
 */
 
Bool wxResourceReadOneResource(FILE *fd, PrologDatabase& db, Bool *eof, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;
    
  // static or #define
  if (!wxGetResourceToken(fd))
  {
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "#define") == 0)
  {
    wxGetResourceToken(fd);
    char *name = copystring(wxResourceBuffer);
    wxGetResourceToken(fd);
    char *value = copystring(wxResourceBuffer);
    if (isalpha(value[0]))
    {
      int val = (int)atol(value);
      wxResourceAddIdentifier(name, val, table);
    }
    else
    {
      char buf[300];
      sprintf(buf, "#define %s must be an integer.", name);
      wxWarning(buf);
      delete[] name;
      delete[] value;
      return FALSE;
    }
    delete[] name;
    delete[] value;
 
    return TRUE;
  }
  else if (strcmp(wxResourceBuffer, "#include") == 0)
  {
    wxGetResourceToken(fd);
    char *name = copystring(wxResourceBuffer);
    char *actualName = name;
    if (name[0] == '"')
      actualName = name + 1;
    int len = strlen(name);
    if ((len > 0) && (name[len-1] == '"'))
      name[len-1] = 0;
    if (!wxResourceParseIncludeFile(actualName, table))
    {
      char buf[400];
      sprintf(buf, "Could not find resource include file %s.", actualName);
      wxWarning(buf);
    }
    delete[] name;
    return TRUE;
  }
  else if (strcmp(wxResourceBuffer, "static") != 0)
  {
    char buf[300];
    strcpy(buf, "Found ");
    strncat(buf, wxResourceBuffer, 30);
    strcat(buf, ", expected static, #include or #define\nwhilst parsing resource.");
    wxWarning(buf);
    return FALSE;
  }

  // char
  if (!wxGetResourceToken(fd))
  {
    wxWarning("Unexpected end of file whilst parsing resource.");
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "char") != 0)
  {
    wxWarning("Expected 'char' whilst parsing resource.");
    return FALSE;
  }
    
  // *name
  if (!wxGetResourceToken(fd))
  {
    wxWarning("Unexpected end of file whilst parsing resource.");
    *eof = TRUE;
    return FALSE;
  }

  if (wxResourceBuffer[0] != '*')
  {
    wxWarning("Expected '*' whilst parsing resource.");
    return FALSE;
  }
  char nameBuf[100];
  strncpy(nameBuf, wxResourceBuffer+1, 99);
    
  // =
  if (!wxGetResourceToken(fd))
  {
    wxWarning("Unexpected end of file whilst parsing resource.");
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "=") != 0)
  {
    wxWarning("Expected '=' whilst parsing resource.");
    return FALSE;
  }

  // String
  if (!wxGetResourceToken(fd))
  {
    wxWarning("Unexpected end of file whilst parsing resource.");
    *eof = TRUE;
    return FALSE;
  }
  else
  {
    if (!db.ReadPrologFromString(wxResourceBuffer))
    {
      char buf[300];
      sprintf(buf, "%s: ill-formed resource file syntax.", nameBuf);
      wxWarning(buf);
      return FALSE;
    }
  }
  // Semicolon
  if (!wxGetResourceToken(fd))
  {
    *eof = TRUE;
  }
  return TRUE;
}

/*
 * Parses string window style into integer window style
 */
 
/*
 * Style flag parsing, e.g.
 * "wxSYSTEM_MENU | wxBORDER" -> integer
 */

char *wxResourceParseWord(char *s, int *i)
{
  if (!s)
    return NULL;

  static char buf[150];
  int len = strlen(s);
  int j = 0;
  int ii = *i;
  while ((ii < len) && (isalpha(s[ii]) || (s[ii] == '_')))
  {
    buf[j] = s[ii];
    j ++;
    ii ++;
  }
  buf[j] = 0;

  // Eat whitespace and conjunction characters
  while ((ii < len) &&
         ((s[ii] == ' ') || (s[ii] == '|') || (s[ii] == ',')))
  {
    ii ++;
  }
  *i = ii;
  if (j == 0)
    return NULL;
  else
    return buf;
}

struct wxResourceBitListStruct
{
  char *word;
  long bits;
};

static wxResourceBitListStruct wxResourceBitListTable[] =
{
  /* wxListBox */
  { "wxSINGLE", wxSINGLE },
  { "wxMULTIPLE", wxMULTIPLE },
  { "wxEXTENDED", wxEXTENDED },
  { "wxLB_SINGLE", wxLB_SINGLE },
  { "wxLB_MULTIPLE", wxLB_MULTIPLE },
  { "wxLB_EXTENDED", wxLB_EXTENDED },
  { "wxNEEDED_SB", wxNEEDED_SB },
  { "wxALWAYS_SB", wxALWAYS_SB },
  { "wxLB_NEEDED_SB", wxLB_NEEDED_SB },
  { "wxLB_ALWAYS_SB", wxLB_ALWAYS_SB },
  { "wxLB_SORT", wxLB_SORT },
  
  /* wxComboxBox */
  { "wxCB_SIMPLE", wxCB_SIMPLE },
  { "wxCB_DROPDOWN", wxCB_DROPDOWN },
  { "wxCB_SORT", wxCB_SORT },
  
  /* wxGauge */
  { "wxGA_PROGRESSBAR", wxGA_PROGRESSBAR },

  /* wxText */
  { "wxPASSWORD", wxPASSWORD},
  { "wxPROCESS_ENTER", wxPROCESS_ENTER},
  { "wxTE_PASSWORD", wxTE_PASSWORD},
  { "wxTE_READONLY", wxTE_READONLY},
  { "wxTE_PROCESS_ENTER", wxTE_PROCESS_ENTER},

  /* wxRadioButton */
  { "wxRB_GROUP", wxRB_GROUP },

  /* wxItem */
  { "wxFIXED_LENGTH", wxFIXED_LENGTH},
  { "wxALIGN_LEFT", wxALIGN_LEFT},
  { "wxALIGN_CENTER", wxALIGN_CENTER},
  { "wxALIGN_CENTRE", wxALIGN_CENTRE},
  { "wxALIGN_RIGHT", wxALIGN_RIGHT},
  
  /* wxToolBar */
  { "wxTB_3DBUTTONS", wxTB_3DBUTTONS},

  /* Generic */
  { "wxVSCROLL", wxVSCROLL },
  { "wxHSCROLL", wxHSCROLL },
  { "wxCAPTION", wxCAPTION },
  { "wxSTAY_ON_TOP", wxSTAY_ON_TOP},
  { "wxICONIZE", wxICONIZE},
  { "wxMINIMIZE", wxICONIZE},
  { "wxMAXIMIZE", wxMAXIMIZE},
  { "wxSDI", wxSDI},
  { "wxMDI_PARENT", wxMDI_PARENT},
  { "wxMDI_CHILD", wxMDI_CHILD},
  { "wxTHICK_FRAME", wxTHICK_FRAME},
  { "wxRESIZE_BORDER", wxRESIZE_BORDER},
  { "wxSYSTEM_MENU", wxSYSTEM_MENU},
  { "wxMINIMIZE_BOX", wxMINIMIZE_BOX},
  { "wxMAXIMIZE_BOX", wxMAXIMIZE_BOX},
  { "wxRESIZE_BOX", wxRESIZE_BOX},
  { "wxDEFAULT_FRAME", wxDEFAULT_FRAME},
  { "wxDEFAULT_DIALOG_STYLE", wxDEFAULT_DIALOG_STYLE},
  { "wxBORDER", wxBORDER},
  { "wxRETAINED", wxRETAINED},
  { "wxEDITABLE", wxEDITABLE},
  { "wxREADONLY", wxREADONLY},
  { "wxNATIVE_IMPL", wxNATIVE_IMPL},
  { "wxEXTENDED_IMPL", wxEXTENDED_IMPL},
  { "wxBACKINGSTORE", wxBACKINGSTORE},
  { "wxFLAT", wxFLAT},
//  { "wxMOTIF_RESIZE", wxMOTIF_RESIZE},
  { "wxFIXED_LENGTH", wxFIXED_LENGTH},

#if USE_ENHANCED_DIALOG
  // Enhanced dialog
  { "wxBOTTOM_COMMANDS", wxBOTTOM_COMMANDS},
  { "wxRIGHT_COMMANDS", wxRIGHT_COMMANDS},
  { "wxSTATUS_FOOTER", wxSTATUS_FOOTER},
  { "wxNO_STATUS_FOOTER", wxNO_STATUS_FOOTER},
  { "wxNO_CANCEL_BUTTON", wxNO_CANCEL_BUTTON},
  { "wxCANCEL_BUTTON_FIRST", wxCANCEL_BUTTON_FIRST},
  { "wxCANCEL_BUTTON_SECOND", wxCANCEL_BUTTON_SECOND},
  { "wxCANCEL_BUTTON_LAST", wxCANCEL_BUTTON_LAST},
  { "wxENH_DEFAULT", wxENH_DEFAULT},
#endif

  { "wxCOLOURED", wxCOLOURED},
  { "wxTINY_CAPTION_HORIZ", wxTINY_CAPTION_HORIZ},
  { "wxTINY_CAPTION_VERT", wxTINY_CAPTION_VERT},

  // Text font families
  { "wxDEFAULT", wxDEFAULT},
  { "wxDECORATIVE", wxDECORATIVE},
  { "wxROMAN", wxROMAN},
  { "wxSCRIPT", wxSCRIPT},
  { "wxSWISS", wxSWISS},
  { "wxMODERN", wxMODERN},
  { "wxTELETYPE", wxTELETYPE},
  { "wxVARIABLE", wxVARIABLE},
  { "wxFIXED", wxFIXED},
  { "wxNORMAL", wxNORMAL},
  { "wxLIGHT", wxLIGHT},
  { "wxBOLD", wxBOLD},
  { "wxITALIC", wxITALIC},
  { "wxSLANT", wxSLANT},
  { "wxSOLID", wxSOLID},
  { "wxDOT", wxDOT},
  { "wxLONG_DASH", wxLONG_DASH},
  { "wxSHORT_DASH", wxSHORT_DASH},
  { "wxDOT_DASH", wxDOT_DASH},
  { "wxUSER_DASH", wxUSER_DASH},
  { "wxTRANSPARENT", wxTRANSPARENT},
  { "wxSTIPPLE", wxSTIPPLE},
  { "wxBDIAGONAL_HATCH", wxBDIAGONAL_HATCH},
  { "wxCROSSDIAG_HATCH", wxCROSSDIAG_HATCH},
  { "wxFDIAGONAL_HATCH", wxFDIAGONAL_HATCH},
  { "wxCROSS_HATCH", wxCROSS_HATCH},
  { "wxHORIZONTAL_HATCH", wxHORIZONTAL_HATCH},
  { "wxVERTICAL_HATCH", wxVERTICAL_HATCH},
  { "wxJOIN_BEVEL", wxJOIN_BEVEL},
  { "wxJOIN_MITER", wxJOIN_MITER},
  { "wxJOIN_ROUND", wxJOIN_ROUND},
  { "wxCAP_ROUND", wxCAP_ROUND},
  { "wxCAP_PROJECTING", wxCAP_PROJECTING},
  { "wxCAP_BUTT", wxCAP_BUTT},

  // Logical ops
  { "wxCLEAR", wxCLEAR},
  { "wxXOR", wxXOR},
  { "wxINVERT", wxINVERT},
  { "wxOR_REVERSE", wxOR_REVERSE},
  { "wxAND_REVERSE", wxAND_REVERSE},
  { "wxCOPY", wxCOPY},
  { "wxAND", wxAND},
  { "wxAND_INVERT", wxAND_INVERT},
  { "wxNO_OP", wxNO_OP},
  { "wxNOR", wxNOR},
  { "wxEQUIV", wxEQUIV},
  { "wxSRC_INVERT", wxSRC_INVERT},
  { "wxOR_INVERT", wxOR_INVERT},
  { "wxNAND", wxNAND},
  { "wxOR", wxOR},
  { "wxSET", wxSET},

  { "wxFLOOD_SURFACE", wxFLOOD_SURFACE},
  { "wxFLOOD_BORDER", wxFLOOD_BORDER},
  { "wxODDEVEN_RULE", wxODDEVEN_RULE},
  { "wxWINDING_RULE", wxWINDING_RULE},
  { "wxHORIZONTAL", wxHORIZONTAL},
  { "wxVERTICAL", wxVERTICAL},
  { "wxBOTH", wxBOTH},
  { "wxCENTER_FRAME", wxCENTER_FRAME},
  { "wxOK", wxOK},
  { "wxYES_NO", wxYES_NO},
  { "wxCANCEL", wxCANCEL},
  { "wxYES", wxYES},
  { "wxNO", wxNO},
  { "wxICON_EXCLAMATION", wxICON_EXCLAMATION},
  { "wxICON_HAND", wxICON_HAND},
  { "wxICON_QUESTION", wxICON_QUESTION},
  { "wxICON_INFORMATION", wxICON_INFORMATION},
  { "wxICON_STOP", wxICON_STOP},
  { "wxICON_ASTERISK", wxICON_ASTERISK},
  { "wxICON_MASK", wxICON_MASK},
  { "wxCENTRE", wxCENTRE},
  { "wxCENTER", wxCENTRE},
  { "wxUSER_COLOURS", wxUSER_COLOURS},
  { "wxVERTICAL_LABEL", wxVERTICAL_LABEL},
  { "wxHORIZONTAL_LABEL", wxHORIZONTAL_LABEL},

  // Bitmap types (not strictly styles)
  { "wxBITMAP_TYPE_XPM", wxBITMAP_TYPE_XPM},
  { "wxBITMAP_TYPE_XBM", wxBITMAP_TYPE_XBM},
  { "wxBITMAP_TYPE_BMP", wxBITMAP_TYPE_BMP},
  { "wxBITMAP_TYPE_RESOURCE", wxBITMAP_TYPE_BMP_RESOURCE},
  { "wxBITMAP_TYPE_BMP_RESOURCE", wxBITMAP_TYPE_BMP_RESOURCE},
  { "wxBITMAP_TYPE_GIF", wxBITMAP_TYPE_GIF},
  { "wxBITMAP_TYPE_TIF", wxBITMAP_TYPE_TIF},
  { "wxBITMAP_TYPE_ICO", wxBITMAP_TYPE_ICO},
  { "wxBITMAP_TYPE_ICO_RESOURCE", wxBITMAP_TYPE_ICO_RESOURCE},
  { "wxBITMAP_TYPE_CUR", wxBITMAP_TYPE_CUR},
  { "wxBITMAP_TYPE_CUR_RESOURCE", wxBITMAP_TYPE_CUR_RESOURCE},
  { "wxBITMAP_TYPE_XBM_DATA", wxBITMAP_TYPE_XBM_DATA},
  { "wxBITMAP_TYPE_XPM_DATA", wxBITMAP_TYPE_XPM_DATA},
  { "wxBITMAP_TYPE_ANY", wxBITMAP_TYPE_ANY}
};

static int wxResourceBitListCount = (sizeof(wxResourceBitListTable)/sizeof(wxResourceBitListStruct));

long wxParseWindowStyle(char *bitListString)
{
  int i = 0;
  char *word;
  long bitList = 0;
  while (word = wxResourceParseWord(bitListString, &i))
  {
    Bool found = FALSE;
    int j;
    for (j = 0; j < wxResourceBitListCount; j++)
      if (strcmp(wxResourceBitListTable[j].word, word) == 0)
      {
        bitList |= wxResourceBitListTable[j].bits;
        found = TRUE;
        break;
      }
    if (!found)
    {
      char buf[200];
      sprintf(buf, "Unrecognized style %s whilst parsing resource.", word);
      wxWarning(buf);
      return 0;
    }
  }
  return bitList;
}

/*
 * Load a bitmap from a wxWindows resource, choosing an optimum
 * depth and appropriate type.
 */
 
wxBitmap *wxResourceCreateBitmap(char *resource, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;
    
  wxItemResource *item = table->FindResource(resource);
  if (item)
  {
    if (!item->GetType() || strcmp(item->GetType(), "wxBitmap") != 0)
    {
      char buf[300];
      sprintf(buf, "%s not a bitmap resource specification.", resource);
      wxWarning(buf);
      return NULL;
    }
    int thisDepth = wxDisplayDepth();
    long thisNoColours = (long)pow(2.0, (double)thisDepth);

    wxItemResource *optResource = NULL;
    
    // Try to find optimum bitmap for this platform/colour depth
    wxNode *node = item->GetChildren().First();
    while (node)
    {
      wxItemResource *child = (wxItemResource *)node->Data();
      int platform = (int)child->GetValue2();
      int noColours = (int)child->GetValue3();
/*
      char *name = child->GetName();
      int bitmapType = (int)child->GetValue1();
      int xRes = child->GetWidth();
      int yRes = child->GetHeight();
*/

      switch (platform)
      {
        case RESOURCE_PLATFORM_ANY:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours.
            // If noColours is zero (unspecified), then assume this
            // is the right one.
            if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#ifdef wx_msw
        case RESOURCE_PLATFORM_WINDOWS:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours
            if ((noColours > 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#endif
#ifdef wx_x
        case RESOURCE_PLATFORM_X:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours
            if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#endif
#ifdef wx_max
        case RESOURCE_PLATFORM_MAC:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours
            if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#endif
        default:
          break;
      }
      node = node->Next();
    }
    // If no matching resource, fail.
    if (!optResource)
      return NULL;

    char *name = optResource->GetName();
    int bitmapType = (int)optResource->GetValue1();
    wxBitmap *bitmap = NULL;
    switch (bitmapType)
    {
      case wxBITMAP_TYPE_XBM_DATA:
      {
#ifdef wx_x
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          char buf[400];
          sprintf(buf, "Failed to find XBM resource %s.\nForgot to use wxResourceLoadBitmapData?", name);
          wxWarning(buf);
          return NULL;
        }
        bitmap = new wxBitmap((char *)item->GetValue1(), (int)item->GetValue2(), (int)item->GetValue3()); 
#else
        wxWarning("No XBM facility available!");
#endif
        break;
      }
      case wxBITMAP_TYPE_XPM_DATA:
      {
#if (defined(wx_x) && USE_XPM_IN_X) || (defined(wx_msw) && USE_XPM_IN_MSW)
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          char buf[400];
          sprintf(buf, "Failed to find XPM resource %s.\nForgot to use wxResourceLoadBitmapData?", name);
          wxWarning(buf);
          return NULL;
        }
        bitmap = new wxBitmap((char **)item->GetValue1());
#else
        wxWarning("No XPM facility available!");
#endif
        break;
      }
      default:
      {
        bitmap = new wxBitmap(name, bitmapType);
        break;
      }
    }
    if (!bitmap)
      return NULL;
      
    if (bitmap->Ok())
    {
      return bitmap;
    }
    else
    {
      delete bitmap;
      return NULL;
    }
  }
  else
  {
    char buf[300];
    sprintf(buf, "Bitmap resource specification %s not found.", resource);
    wxWarning(buf);
    return NULL;
  }
}

/*
 * Load an icon from a wxWindows resource, choosing an optimum
 * depth and appropriate type.
 */
 
wxIcon *wxResourceCreateIcon(char *resource, wxResourceTable *table)
{
  if (!table)
  table = &wxDefaultResourceTable;
  
  wxItemResource *item = table->FindResource(resource);
  if (item)
  {
    if (!item->GetType() || strcmp(item->GetType(), "wxIcon") != 0)
    {
      char buf[300];
      sprintf(buf, "%s not an icon resource specification.", resource);
      wxWarning(buf);
      return NULL;
    }
    int thisDepth = wxDisplayDepth();
    long thisNoColours = (long)pow(2.0, (double)thisDepth);

    wxItemResource *optResource = NULL;
    
    // Try to find optimum icon for this platform/colour depth
    wxNode *node = item->GetChildren().First();
    while (node)
    {
      wxItemResource *child = (wxItemResource *)node->Data();
      int platform = (int)child->GetValue2();
      int noColours = (int)child->GetValue3();
/*
      char *name = child->GetName();
      int bitmapType = (int)child->GetValue1();
      int xRes = child->GetWidth();
      int yRes = child->GetHeight();
*/

      switch (platform)
      {
        case RESOURCE_PLATFORM_ANY:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours.
            // If noColours is zero (unspecified), then assume this
            // is the right one.
            if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#ifdef wx_msw
        case RESOURCE_PLATFORM_WINDOWS:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours
            if ((noColours > 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#endif
#ifdef wx_x
        case RESOURCE_PLATFORM_X:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours
            if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#endif
#ifdef wx_max
        case RESOURCE_PLATFORM_MAC:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours
            if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#endif
        default:
          break;
      }
      node = node->Next();
    }
    // If no matching resource, fail.
    if (!optResource)
      return NULL;

    char *name = optResource->GetName();
    int bitmapType = (int)optResource->GetValue1();
    wxIcon *icon = NULL;
    switch (bitmapType)
    {
      case wxBITMAP_TYPE_XBM_DATA:
      {
#ifdef wx_x
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          char buf[400];
          sprintf(buf, "Failed to find XBM resource %s.\nForgot to use wxResourceLoadIconData?", name);
          wxWarning(buf);
          return NULL;
        }
        icon = new wxIcon((char *)item->GetValue1(), (int)item->GetValue2(), (int)item->GetValue3()); 
#else
        wxWarning("No XBM facility available!");
#endif
        break;
      }
      case wxBITMAP_TYPE_XPM_DATA:
      {
      // *** XPM ICON NOT YET IMPLEMENTED IN WXWINDOWS ***
/*
#if (defined(wx_x) && USE_XPM_IN_X) || (defined(wx_msw) && USE_XPM_IN_MSW)
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          char buf[400];
          sprintf(buf, "Failed to find XPM resource %s.\nForgot to use wxResourceLoadIconData?", name);
          wxWarning(buf);
          return NULL;
        }
        icon = new wxIcon((char **)item->GetValue1());
#else
        wxWarning("No XPM facility available!");
#endif
*/
        wxWarning("No XPM icon facility available!");
        break;
      }
      default:
      {
        icon = new wxIcon(name, bitmapType);
        break;
      }
    }
    if (!icon)
      return NULL;
      
    if (icon->Ok())
    {
      return icon;
    }
    else
    {
      delete icon;
      return NULL;
    }
  }
  else
  {
    char buf[300];
    sprintf(buf, "Icon resource specification %s not found.", resource);
    wxWarning(buf);
    return NULL;
  }
}

wxMenu *wxResourceCreateMenu(wxItemResource *item)
{
  wxMenu *menu = new wxMenu;
  wxNode *node = item->GetChildren().First();
  while (node)
  {
    wxItemResource *child = (wxItemResource *)node->Data();
    if (child->GetType() && strcmp(child->GetType(), "wxMenuSeparator") == 0)
      menu->AppendSeparator();
    else if (child->GetChildren().Number() > 0)
    {
      wxMenu *subMenu = wxResourceCreateMenu(child);
      if (subMenu)
        menu->Append((int)child->GetValue1(), child->GetTitle(), subMenu, child->GetValue4());
    }
    else
    {
      menu->Append((int)child->GetValue1(), child->GetTitle(), child->GetValue4(), child->GetValue2());
    }
    node = node->Next();
  }
  return menu;
}

wxMenuBar *wxResourceCreateMenuBar(char *resource, wxResourceTable *table, wxMenuBar *menuBar)
{
  if (!table)
    table = &wxDefaultResourceTable;
    
  wxItemResource *menuResource = table->FindResource(resource);
  if (menuResource && menuResource->GetType() && strcmp(menuResource->GetType(), "wxMenu") == 0)
  {
    if (!menuBar)
      menuBar = new wxMenuBar;
    wxNode *node = menuResource->GetChildren().First();
    while (node)
    {
      wxItemResource *child = (wxItemResource *)node->Data();
      wxMenu *menu = wxResourceCreateMenu(child);
      if (menu)
        menuBar->Append(menu, child->GetTitle());
      node = node->Next();
    }
    return menuBar;
  }
  return NULL;
}

wxMenu *wxResourceCreateMenu(char *resource, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;
    
  wxItemResource *menuResource = table->FindResource(resource);
  if (menuResource && menuResource->GetType() && strcmp(menuResource->GetType(), "wxMenu") == 0)
//  if (menuResource && (menuResource->GetType() == wxTYPE_MENU))
    return wxResourceCreateMenu(menuResource);
  return NULL;
}

// Global equivalents (so don't have to refer to default table explicitly)
Bool wxResourceParseData(char *resource, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;
    
  return table->ParseResourceData(resource);
}

Bool wxResourceParseFile(char *filename, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;
    
  return table->ParseResourceFile(filename);
}

// Register XBM/XPM data
Bool wxResourceRegisterBitmapData(char *name, char bits[], int width, int height, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;
    
  return table->RegisterResourceBitmapData(name, bits, width, height);
}

Bool wxResourceRegisterBitmapData(char *name, char **data, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;

  return table->RegisterResourceBitmapData(name, data);
}

void wxResourceClear(wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;

  table->ClearTable();
}

/*
 * Identifiers
 */

Bool wxResourceAddIdentifier(char *name, int value, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;
    
  table->identifiers.Put(name, (wxObject *)value);
  return TRUE;
}

int wxResourceGetIdentifier(char *name, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;
    
  return (int)table->identifiers.Get(name);
}

/*
 * Parse #include file for #defines (only)
 */

Bool wxResourceParseIncludeFile(char *f, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;
    
  FILE *fd = fopen(f, "r");
  if (!fd)
  {
    return FALSE;
  }
  while (wxGetResourceToken(fd))
  {
    if (strcmp(wxResourceBuffer, "#define") == 0)
    {
      wxGetResourceToken(fd);
      char *name = copystring(wxResourceBuffer);
      wxGetResourceToken(fd);
      char *value = copystring(wxResourceBuffer);
      if (isdigit(value[0]))
      {
        int val = (int)atol(value);
        wxResourceAddIdentifier(name, val, table);
      }
      delete[] name;
      delete[] value;
    }
  }
  fclose(fd);
  return TRUE;
}

/*
 * Reading strings as if they were .wxr files
 */

static int getc_string(char *s)
{
  int ch = s[wxResourceStringPtr];
  if (ch == 0)
    return EOF;
  else
  {
    wxResourceStringPtr ++;
    return ch;
  }
}

static int ungetc_string(void)
{
  wxResourceStringPtr --;
  return 0;
}

Bool wxEatWhiteSpaceString(char *s)
{
  int ch = getc_string(s);
  if (ch == EOF)
    return TRUE;
    
  if ((ch != ' ') && (ch != '/') && (ch != ' ') && (ch != 10) && (ch != 13) && (ch != 9))
  {
    ungetc_string();
    return TRUE;
  }

  // Eat whitespace
  while (ch == ' ' || ch == 10 || ch == 13 || ch == 9)
    ch = getc_string(s);
  // Check for comment
  if (ch == '/')
  {
    ch = getc_string(s);
    if (ch == '*')
    {
      Bool finished = FALSE;
      while (!finished)
      {
        ch = getc_string(s);
        if (ch == EOF)
          return FALSE;
        if (ch == '*')
        {
          int newCh = getc_string(s);
          if (newCh == '/')
            finished = TRUE;
          else
          {
            ungetc_string();
          }
        }
      }
    }
    else // False alarm
      return FALSE;
  }
  else if (ch != EOF)
    ungetc_string();
  return wxEatWhiteSpaceString(s);
}

Bool wxGetResourceTokenString(char *s)
{
  if (!wxResourceBuffer)
    wxReallocateResourceBuffer();
  wxResourceBuffer[0] = 0;
  wxEatWhiteSpaceString(s);

  int ch = getc_string(s);
  if (ch == '"')
  {
    // Get string
    wxResourceBufferCount = 0;
    ch = getc_string(s);
    while (ch != '"')
    {
      int actualCh = ch;
      if (ch == EOF)
      {
        wxResourceBuffer[wxResourceBufferCount] = 0;
        return FALSE;
      }
      // Escaped characters
      else if (ch == '\\')
      {
        int newCh = getc_string(s);
        if (newCh == '"')
          actualCh = '"';
        else if (newCh == 10)
          actualCh = 10;
        else
        {
          ungetc_string();
        }
      }

      if (wxResourceBufferCount >= wxResourceBufferSize-1)
        wxReallocateResourceBuffer();
      wxResourceBuffer[wxResourceBufferCount] = (char)actualCh;
      wxResourceBufferCount ++;
      ch = getc_string(s);
    }
    wxResourceBuffer[wxResourceBufferCount] = 0;
  }
  else
  {
    wxResourceBufferCount = 0;
    // Any other token
    while (ch != ' ' && ch != EOF && ch != ' ' && ch != 13 && ch != 9 && ch != 10)
    {
      if (wxResourceBufferCount >= wxResourceBufferSize-1)
        wxReallocateResourceBuffer();
      wxResourceBuffer[wxResourceBufferCount] = (char)ch;
      wxResourceBufferCount ++;
      
      ch = getc_string(s);
    }
    wxResourceBuffer[wxResourceBufferCount] = 0;
    if (ch == EOF)
      return FALSE;
  }
  return TRUE;
}

/*
 * Files are in form:
  static char *name = "....";
  with possible comments.
 */
 
Bool wxResourceReadOneResourceString(char *s, PrologDatabase& db, Bool *eof, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;
    
  // static or #define
  if (!wxGetResourceTokenString(s))
  {
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "#define") == 0)
  {
    wxGetResourceTokenString(s);
    char *name = copystring(wxResourceBuffer);
    wxGetResourceTokenString(s);
    char *value = copystring(wxResourceBuffer);
    if (isalpha(value[0]))
    {
      int val = (int)atol(value);
      wxResourceAddIdentifier(name, val, table);
    }
    else
    {
      char buf[300];
      sprintf(buf, "#define %s must be an integer.", name);
      wxWarning(buf);
      delete[] name;
      delete[] value;
      return FALSE;
    }
    delete[] name;
    delete[] value;
 
    return TRUE;
  }
/*
  else if (strcmp(wxResourceBuffer, "#include") == 0)
  {
    wxGetResourceTokenString(s);
    char *name = copystring(wxResourceBuffer);
    char *actualName = name;
    if (name[0] == '"')
      actualName = name + 1;
    int len = strlen(name);
    if ((len > 0) && (name[len-1] == '"'))
      name[len-1] = 0;
    if (!wxResourceParseIncludeFile(actualName, table))
    {
      char buf[400];
      sprintf(buf, "Could not find resource include file %s.", actualName);
      wxWarning(buf);
    }
    delete[] name;
    return TRUE;
  }
*/
  else if (strcmp(wxResourceBuffer, "static") != 0)
  {
    char buf[300];
    strcpy(buf, "Found ");
    strncat(buf, wxResourceBuffer, 30);
    strcat(buf, ", expected static, #include or #define\nwhilst parsing resource.");
    wxWarning(buf);
    return FALSE;
  }

  // char
  if (!wxGetResourceTokenString(s))
  {
    wxWarning("Unexpected end of file whilst parsing resource.");
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "char") != 0)
  {
    wxWarning("Expected 'char' whilst parsing resource.");
    return FALSE;
  }
    
  // *name
  if (!wxGetResourceTokenString(s))
  {
    wxWarning("Unexpected end of file whilst parsing resource.");
    *eof = TRUE;
    return FALSE;
  }

  if (wxResourceBuffer[0] != '*')
  {
    wxWarning("Expected '*' whilst parsing resource.");
    return FALSE;
  }
  char nameBuf[100];
  strncpy(nameBuf, wxResourceBuffer+1, 99);
    
  // =
  if (!wxGetResourceTokenString(s))
  {
    wxWarning("Unexpected end of file whilst parsing resource.");
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "=") != 0)
  {
    wxWarning("Expected '=' whilst parsing resource.");
    return FALSE;
  }

  // String
  if (!wxGetResourceTokenString(s))
  {
    wxWarning("Unexpected end of file whilst parsing resource.");
    *eof = TRUE;
    return FALSE;
  }
  else
  {
    if (!db.ReadPrologFromString(wxResourceBuffer))
    {
      char buf[300];
      sprintf(buf, "%s: ill-formed resource file syntax.", nameBuf);
      wxWarning(buf);
      return FALSE;
    }
  }
  // Semicolon
  if (!wxGetResourceTokenString(s))
  {
    *eof = TRUE;
  }
  return TRUE;
}

Bool wxResourceParseString(char *s, wxResourceTable *table)
{
  if (!table)
    table = &wxDefaultResourceTable;
    
  if (!s)
    return FALSE;
    
  // Turn backslashes into spaces 
  if (s)
  {
    int len = strlen(s);
    int i;
    for (i = 0; i < len; i++)
      if (s[i] == 92 && s[i+1] == 13)
      {
        s[i] = ' ';
        s[i+1] = ' ';
      }
  }

  PrologDatabase db;
  wxResourceStringPtr = 0;

  Bool eof = FALSE;
  while (wxResourceReadOneResourceString(s, db, &eof, table) && !eof)
  {
    // Loop
  }
  return wxResourceInterpretResources(*table, db);
}


#endif // USE_WX_RESOURCES
