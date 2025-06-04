/*
 * File:	bgdi.cc
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

#include "wx_help.h"
#include "wx_types.h"
#include <ctype.h>
#include <stdlib.h>

#include "wxbuild.h"
#include "namegen.h"
#include "bapp.h"
#include "bframe.h"
#include "bmenu.h"
#include "bsubwin.h"
#include "bitem.h"
#include "btoolbar.h"
#include "bgdi.h"

FontManager TheFontManager;

BuildFontData::BuildFontData(void)
{
  fontName = NULL;
  fontFamily = copystring("Swiss");
  fontStyle = copystring("Normal");
  fontWeight = copystring("Normal");
  fontSize = 10;
  font = NULL;
}

BuildFontData::~BuildFontData(void)
{
  if (fontName) delete[] fontName;
  delete[] fontFamily;
  delete[] fontStyle;
  delete[] fontWeight;
}

wxFont *BuildFontData::CreateFont(void)
{
  if (font)
    return font;
    
  int familyInt = wxDEFAULT;
  if (strcmp(fontFamily, "Roman") == 0)
    familyInt = wxROMAN;
  else if (strcmp(fontFamily, "Decorative") == 0)
    familyInt = wxDECORATIVE;
  else if (strcmp(fontFamily, "Modern") == 0)
    familyInt = wxMODERN;
  else if (strcmp(fontFamily, "Script") == 0)
    familyInt = wxSCRIPT;
  else if (strcmp(fontFamily, "Swiss") == 0)
    familyInt = wxSWISS;
  else if (strcmp(fontFamily, "Teletype") == 0)
    familyInt = wxTELETYPE;

  int styleInt = wxNORMAL;
  if (strcmp(fontStyle, "Italic") == 0)
    styleInt = wxITALIC;
  else if (strcmp(fontStyle, "Slant") == 0)
    styleInt = wxSLANT;
  else if (strcmp(fontStyle, "Normal") == 0)
    styleInt = wxNORMAL;

  int weightInt = wxNORMAL;
  if (strcmp(fontWeight, "Light") == 0)
    weightInt = wxLIGHT;
  else if (strcmp(fontWeight, "Bold") == 0)
    weightInt = wxBOLD;
  else if (strcmp(fontWeight, "Normal") == 0)
    weightInt = wxNORMAL;

  font = wxTheFontList->FindOrCreateFont(fontSize, familyInt, styleInt, weightInt);
  return font;
}

Bool BuildFontData::Edit(wxWindow *parent)
{
  wxDialogBox *dialog = new wxDialogBox(parent, "Editing Font", TRUE, 10, 10);
  dialog->SetLabelFont(SmallLabelFont);
  dialog->SetButtonFont(SmallButtonFont);

  BuildForm *form = new BuildForm;
  form->dialog = dialog;

  form->Add(wxMakeFormString("Font name", &fontName, wxFORM_DEFAULT, NULL, NULL,
             wxVERTICAL, 180));

  form->Add(wxMakeFormNewLine());
/*
  form->Add(wxMakeFormShort("Point size", &fontSize,
            wxFORM_DEFAULT, NULL, NULL, wxVERTICAL, 180));
*/
  form->Add(wxMakeFormString("Font family", &fontFamily, wxFORM_CHOICE,
            new wxList(wxMakeConstraintStrings(
  "Roman"            ,
  "Decorative"       ,
  "Modern"           ,
  "Swiss"            ,
  "Script"           ,
  "Teletype"         ,
  "Default"          ,
  NULL),
  NULL), NULL, wxVERTICAL, 180));

  form->Add(wxMakeFormNewLine());

  form->Add(wxMakeFormString("Font style", &fontStyle, wxFORM_CHOICE,
            new wxList(wxMakeConstraintStrings(
  "Italic"      ,
  "Slant"       ,
  "Normal"      ,
  NULL),
  NULL), NULL, wxVERTICAL, 180));

  form->Add(wxMakeFormString("Font weight", &fontWeight, wxFORM_CHOICE,
            new wxList(wxMakeConstraintStrings(
  "Light"      ,
  "Bold"       ,
  "Normal"     ,
  NULL),
  NULL), NULL, wxVERTICAL, 180));

  form->Add(wxMakeFormNewLine());

  // Allow choice of fonts between 6 and 32 Pts. (Slider control)
  form->Add(wxMakeFormShort("Pt. size", &fontSize, wxFORM_SLIDER,
	new wxList(wxMakeConstraintRange(6.0, 32.0), NULL),
   NULL, wxVERTICAL, 40));
  
  form->AssociatePanel(dialog);

  dialog->Fit();
  dialog->Centre(wxBOTH);

  dialog->Show(TRUE);
  font = NULL;
  if (formCancelled)
    return FALSE;
  else
    return TRUE;
}

/*
 * Font manager
 *
 */

FontManager::FontManager(void)
{
  dialog = NULL;
}

FontManager::~FontManager(void)
{
}

void FontManager::Show(Bool show)
{
  wxBeginBusyCursor();
  if (show)
  {
    if (dialog) dialog->Show(TRUE);
    else
    {
      dialog = new FontManagerDialog(NULL, "wxBuilder Font Manager", 100, 100, 400, 400);
      dialog->SetLabelFont(SmallLabelFont);
      dialog->SetButtonFont(SmallButtonFont);
      dialog->SetLabelPosition(wxVERTICAL);
      (void)new wxButton(dialog, (wxFunction)FontOkProc, "Ok");
      (void)new wxButton(dialog, (wxFunction)FontHelpProc, "Help");
      dialog->NewLine();
      dialog->listbox = new wxListBox(dialog, (wxFunction)FontManagerProc,
              "Fonts", wxSINGLE, -1, -1, 300, 200);
      dialog->NewLine();
      (void)new wxButton(dialog, (wxFunction)AddFontProc, "Add font");
      (void)new wxButton(dialog, (wxFunction)DeleteFontProc, "Delete font");

      wxNode *node = First();
      while (node)
      {
        BuildFontData *data = (BuildFontData *)node->Data();
        dialog->listbox->Append(data->fontName, (char *)data);
        node = node->Next();
      }
      
      dialog->Fit();
      dialog->Show(TRUE);
    }
  }
  else
  {
    if (dialog)
    {
      dialog->Show(FALSE);
      delete dialog;
    }
    dialog = NULL;
  }
  wxEndBusyCursor();
}

void FontManager::ReadFonts(PrologDatabase *database)
{
  wxNode *node = First();
  while (node)
  {
    BuildFontData *data = (BuildFontData *)node->Data();
    delete data;
    node = node->Next();
  }
  Clear();
  
  database->BeginFind();
  PrologExpr *clause = database->FindClauseByFunctor("fonts");
  if (!clause)
    return;
  int i = 1;
  
  char nameBuf[20];

  PrologExpr *fontExpr = NULL;
  sprintf(nameBuf, "font%d", i);

  while (fontExpr = clause->AttributeValue(nameBuf))
  {
    PrologExpr *nameExpr = fontExpr->Nth(0);
    PrologExpr *familyExpr = fontExpr->Nth(1);
    PrologExpr *styleExpr = fontExpr->Nth(2);
    PrologExpr *weightExpr = fontExpr->Nth(3);
    PrologExpr *sizeExpr = fontExpr->Nth(4);

    BuildFontData *data = new BuildFontData;
    data->fontName = copystring(nameExpr->StringValue());
    delete[] data->fontFamily;
    delete[] data->fontStyle;
    delete[] data->fontWeight;
    data->fontFamily = copystring(familyExpr->StringValue());
    data->fontWeight = copystring(weightExpr->StringValue());
    data->fontStyle = copystring(styleExpr->StringValue());
    data->fontSize = (int)sizeExpr->IntegerValue();
    Append(data->fontName, data);
    i ++;
    sprintf(nameBuf, "font%d", i);
  }
}

void FontManager::WriteFonts(PrologDatabase *database)
{
  PrologExpr *clause = new PrologExpr("fonts");
  wxNode *node = First();
  int i = 1;
  char buf[30];
  while (node)
  {
    BuildFontData *data = (BuildFontData *)node->Data();
    sprintf(buf, "font%d", i);
    PrologExpr *expr = new PrologExpr(PrologList);
    expr->Append(new PrologExpr(PrologString, data->fontName));
    expr->Append(new PrologExpr(PrologString, data->fontFamily));
    expr->Append(new PrologExpr(PrologString, data->fontStyle));
    expr->Append(new PrologExpr(PrologString, data->fontWeight));
    expr->Append(new PrologExpr((long)data->fontSize));
    clause->AddAttributeValue(buf, expr);
    node = node->Next();
    i ++;
  }
  database->Append(clause);
}

FontManagerDialog::FontManagerDialog(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxDialogBox(frame, title, FALSE, x, y, w, h)
{
  listbox = NULL;
}

void FontManagerProc(wxListBox& lbox, wxCommandEvent& event)
{
  int sel = lbox.GetSelection();
  if (sel > -1)
  {
    BuildFontData *data = (BuildFontData *)lbox.GetClientData(sel);
    data->Edit();
  }
}

void FontOkProc(wxButton& but, wxCommandEvent& event)
{
  FontManagerDialog *dialog = (FontManagerDialog *)but.GetParent();
  dialog->Show(FALSE);
  delete dialog;
  TheFontManager.dialog = NULL;
}

void FontHelpProc(wxButton& but, wxCommandEvent& event)
{
  wxBeginBusyCursor();
  HelpInstance->LoadFile();
  HelpInstance->KeywordSearch("Managing GDI objects");
  wxEndBusyCursor();
}

void AddFontProc(wxButton& but, wxCommandEvent& event)
{
  BuildFontData *data = new BuildFontData;
  if (data->Edit())
  {
    TheFontManager.Append(data->fontName, data);
    TheFontManager.dialog->listbox->Append(data->fontName, (char *)data);
  }
  else delete data;
}

void DeleteFontProc(wxButton& but, wxCommandEvent& event)
{
  int sel = TheFontManager.dialog->listbox->GetSelection();
  if (sel > -1)
  {
    BuildFontData *data = (BuildFontData *)TheFontManager.dialog->listbox->GetClientData(sel);
    TheFontManager.DeleteObject(data);
    TheFontManager.dialog->listbox->Delete(sel);
    delete data;
  }
}

wxFont *FindFont(char *fontName)
{
  wxNode *node = TheFontManager.Find(fontName);
  if (node)
  {
    BuildFontData *data = (BuildFontData *)node->Data();
    return data->CreateFont();
  }
  return NULL;
}

/*
 * Bitmap/Icon information
 */

BuildBitmapData::BuildBitmapData(Bool isIcon)
{
  bitmapIsIcon = isIcon;
  bitmap = NULL;
  icon = NULL;
  bitmapName = NULL;
  if (isIcon)
    bitmapTypeWindows = wxBITMAP_TYPE_ICO_RESOURCE;
  else
    bitmapTypeWindows = wxBITMAP_TYPE_BMP_RESOURCE;
  bitmapTypeX = wxBITMAP_TYPE_XBM_DATA;
}

BuildBitmapData::~BuildBitmapData(void)
{
  if (bitmapName)
    delete[] bitmapName;
  if (bitmap)
    delete bitmap;
}

wxBitmap *BuildBitmapData::CreateBitmap(void)
{
  if (bitmap)
    return bitmap;
  if (!bitmapName || (strlen(bitmapName) == 0))
    return NULL;
    
#ifdef wx_msw
  char buf[400];
  strcpy(buf, buildApp.projectDirDOS);
  strcat(buf, "\\");
  strcat(buf, bitmapName);    

  switch (bitmapTypeWindows)
  {
    case wxBITMAP_TYPE_BMP_RESOURCE:
    case wxBITMAP_TYPE_BMP:
    {
      strcat(buf, ".bmp");
      bitmap = new wxBitmap(buf, wxBITMAP_TYPE_BMP);
      if (!bitmap->Ok())
      {
        delete bitmap;
        bitmap = NULL;
        return NULL;
      }
      else
        return bitmap;
      break;
    }
    case wxBITMAP_TYPE_XPM:
    case wxBITMAP_TYPE_XPM_DATA:
    {
      strcat(buf, ".xpm");
      bitmap = new wxBitmap(buf, wxBITMAP_TYPE_XPM);
      if (!bitmap->Ok())
      {
        delete bitmap;
        bitmap = NULL;
        return NULL;
      }
      else
        return bitmap;
      break;
    }
    default:
      return NULL;
  }
#endif
#ifdef wx_x
  char buf[400];
  strcpy(buf, buildApp.projectDirUNIX);
  strcat(buf, "/");
  strcat(buf, bitmapName);    

  switch (bitmapTypeX)
  {
    case wxBITMAP_TYPE_BMP:
    {
      strcat(buf, ".bmp");
      bitmap = new wxBitmap(buf, wxBITMAP_TYPE_BMP);
      if (!bitmap->Ok())
      {
        delete bitmap;
        bitmap = NULL;
        return NULL;
      }
      else
        return bitmap;
      break;
    }
    case wxBITMAP_TYPE_GIF:
    {
      strcat(buf, ".gif");
      bitmap = new wxBitmap(buf, wxBITMAP_TYPE_GIF);
      if (!bitmap->Ok())
      {
        delete bitmap;
        bitmap = NULL;
        return NULL;
      }
      else
        return bitmap;
      break;
    }
    case wxBITMAP_TYPE_XPM:
    case wxBITMAP_TYPE_XPM_DATA:
    {
      strcat(buf, ".xpm");
      bitmap = new wxBitmap(buf, wxBITMAP_TYPE_XPM);
      if (!bitmap->Ok())
      {
        delete bitmap;
        bitmap = NULL;
        return NULL;
      }
      else
        return bitmap;
      break;
    }
    case wxBITMAP_TYPE_XBM:
    case wxBITMAP_TYPE_XBM_DATA:
    {
      strcat(buf, ".xbm");
      bitmap = new wxBitmap(buf, wxBITMAP_TYPE_XBM);
      if (!bitmap->Ok())
      {
        delete bitmap;
        bitmap = NULL;
        return NULL;
      }
      else
        return bitmap;
      break;
    }
    default:
      return NULL;
  }
#endif
  return NULL;
}

wxIcon *BuildBitmapData::CreateIcon(void)
{
  if (icon)
    return icon;
  if (!bitmapName || (strlen(bitmapName) == 0))
    return NULL;
    
#ifdef wx_msw
  char buf[400];
  strcpy(buf, buildApp.projectDirDOS);
  strcat(buf, "\\");
  strcat(buf, bitmapName);    

  switch (bitmapTypeWindows)
  {
    case wxBITMAP_TYPE_ICO_RESOURCE:
    case wxBITMAP_TYPE_ICO:
    {
      strcat(buf, ".ico");
      icon = new wxIcon(buf, wxBITMAP_TYPE_ICO);
      if (!icon->Ok())
      {
        delete icon;
        icon = NULL;
        return NULL;
      }
      else
        return icon;
      break;
    }
/*
    case wxBITMAP_TYPE_XPM:
    case wxBITMAP_TYPE_XPM_DATA:
    {
      strcat(buf, ".xpm");
      icon = new wxIcon(buf, wxBITMAP_TYPE_XPM);
      if (!icon->Ok())
      {
        delete icon;
        icon = NULL;
        return NULL;
      }
      else
        return icon;
      break;
    }
*/
    default:
      return NULL;
  }
#endif
#ifdef wx_x
  char buf[400];
  strcpy(buf, buildApp.projectDirUNIX);
  strcat(buf, "/");
  strcat(buf, bitmapName);    

  switch (bitmapTypeX)
  {
    case wxBITMAP_TYPE_BMP:
    {
      // Not actually implemented in wxWindows yet...
      strcat(buf, ".bmp");
      bitmap = new wxIcon(buf, wxBITMAP_TYPE_BMP);
      if (!icon->Ok())
      {
        delete icon;
        icon = NULL;
        return NULL;
      }
      else
        return icon;
      break;
    }
    case wxBITMAP_TYPE_GIF:
    {
      // Not actually implemented in wxWindows yet...
      strcat(buf, ".gif");
      icon = new wxIcon(buf, wxBITMAP_TYPE_GIF);
      if (!icon->Ok())
      {
        delete icon;
        icon = NULL;
        return NULL;
      }
      else
        return icon;
      break;
    }
    case wxBITMAP_TYPE_XPM:
    case wxBITMAP_TYPE_XPM_DATA:
    {
      // Not actually implemented in wxWindows yet...
      strcat(buf, ".xpm");
      icon = new wxIcon(buf, wxBITMAP_TYPE_XPM);
      if (!icon->Ok())
      {
        delete icon;
        icon = NULL;
        return NULL;
      }
      else
        return icon;
      break;
    }
    case wxBITMAP_TYPE_XBM:
    case wxBITMAP_TYPE_XBM_DATA:
    {
      strcat(buf, ".xbm");
      icon  = new wxIcon(buf, wxBITMAP_TYPE_XBM);
      if (!icon->Ok())
      {
        delete icon;
        icon = NULL;
        return NULL;
      }
      else
        return icon;
      break;
    }
    default:
      return NULL;
  }
#endif
  return NULL;
}

Bool BuildBitmapData::Edit(wxWindow *parent)
{
  wxDialogBox *dialog = new wxDialogBox(parent, (bitmapIsIcon ? "Choosing Icon" : "Choosing Bitmap"), TRUE, 10, 10);
  dialog->SetLabelFont(SmallLabelFont);
  dialog->SetButtonFont(SmallButtonFont);

  BuildForm *form = new BuildForm;
  form->dialog = dialog;

  char *windowsMethod = NULL;
  char *xMethod = NULL;
  char *oldName = bitmapName ? copystring(bitmapName) : copystring("");
  switch (bitmapTypeWindows)
  {
    case wxBITMAP_TYPE_BMP_RESOURCE:
      windowsMethod = copystring("Windows BMP resource");
      break;
    case wxBITMAP_TYPE_ICO_RESOURCE:
      windowsMethod = copystring("Windows ICO resource");
      break;
    case wxBITMAP_TYPE_BMP:
      windowsMethod = copystring("External BMP file");
      break;
    case wxBITMAP_TYPE_ICO:
      windowsMethod = copystring("External ICO file");
      break;
    case wxBITMAP_TYPE_XPM_DATA:
      windowsMethod = copystring("Included XPM");
      break;
    case wxBITMAP_TYPE_XPM:
      windowsMethod = copystring("External XPM file");
      break;
    default:
      windowsMethod = copystring("Unknown");
      break;
  }
  switch (bitmapTypeX)
  {
    case wxBITMAP_TYPE_XBM_DATA:
      xMethod = copystring("Included XBM");
      break;
    case wxBITMAP_TYPE_XBM:
      xMethod = copystring("External XBM file");
      break;
    case wxBITMAP_TYPE_XPM_DATA:
      xMethod = copystring("Included XPM");
      break;
    case wxBITMAP_TYPE_XPM:
      xMethod = copystring("External XPM file");
      break;
    case wxBITMAP_TYPE_GIF:
      xMethod = copystring("External GIF file");
      break;
    case wxBITMAP_TYPE_BMP:
      xMethod = copystring("External BMP file");
      break;
    default:
      xMethod = copystring("Unknown");
      break;
  }

  if (bitmapIsIcon)
    form->Add(wxMakeFormString("Icon name", &bitmapName, wxFORM_DEFAULT, NULL, NULL,
             wxVERTICAL, 300));
  else
    form->Add(wxMakeFormString("Bitmap name", &bitmapName, wxFORM_DEFAULT, NULL, NULL,
             wxVERTICAL, 300));

  form->Add(wxMakeFormNewLine());

  if (bitmapIsIcon)
    form->Add(wxMakeFormString("Windows method", &windowsMethod, wxFORM_CHOICE,
            new wxList(wxMakeConstraintStrings(
  "Windows ICO resource"            ,
  "Included XPM"           ,
  "External XPM file",
  NULL),
  NULL), NULL, wxVERTICAL, 200));
  else
    form->Add(wxMakeFormString("Windows method", &windowsMethod, wxFORM_CHOICE,
            new wxList(wxMakeConstraintStrings(
  "Windows BMP resource"            ,
  "External BMP file"       ,
  "Included XPM"           ,
  "External XPM file",
  NULL),
  NULL), NULL, wxVERTICAL, 200));

//  form->Add(wxMakeFormNewLine());

  if (bitmapIsIcon)
    form->Add(wxMakeFormString("X method", &xMethod, wxFORM_CHOICE,
            new wxList(wxMakeConstraintStrings(
  "Included XBM"       ,
  "External XBM file"      ,
  "Included XPM"      ,
  "External XPM file"      ,
  NULL),
  NULL), NULL, wxVERTICAL, 200));
  else
    form->Add(wxMakeFormString("X method", &xMethod, wxFORM_CHOICE,
            new wxList(wxMakeConstraintStrings(
  "Included XBM"       ,
  "External XBM file"      ,
  "Included XPM"      ,
  "External XPM file"      ,
  "External BMP file"      ,
  "External GIF file",
  NULL),
  NULL), NULL, wxVERTICAL, 200));

  form->AssociatePanel(dialog);

  dialog->Fit();
  dialog->Centre(wxBOTH);

  dialog->Show(TRUE);

  if (wxStringEq(windowsMethod, "Windows BMP resource"))
    bitmapTypeWindows = wxBITMAP_TYPE_BMP_RESOURCE;
  else if (wxStringEq(windowsMethod, "Windows ICO resource"))
    bitmapTypeWindows = wxBITMAP_TYPE_ICO_RESOURCE;
  else if (wxStringEq(windowsMethod, "External BMP file"))
    bitmapTypeWindows = wxBITMAP_TYPE_BMP;
  else if (wxStringEq(windowsMethod, "External ICO file"))
    bitmapTypeWindows = wxBITMAP_TYPE_ICO;
  else if (wxStringEq(windowsMethod, "Included XPM"))
    bitmapTypeWindows = wxBITMAP_TYPE_XPM_DATA;
  else if (wxStringEq(windowsMethod, "External XPM file"))
    bitmapTypeWindows = wxBITMAP_TYPE_XPM;
  else
    bitmapTypeWindows = wxBITMAP_TYPE_BMP_RESOURCE;
    
  if (wxStringEq(xMethod, "Included XBM"))
    bitmapTypeX = wxBITMAP_TYPE_XBM_DATA;
  else if (wxStringEq(xMethod, "External XBM file"))
    bitmapTypeX = wxBITMAP_TYPE_XBM;
  else if (wxStringEq(xMethod, "Included XPM"))
    bitmapTypeX = wxBITMAP_TYPE_XPM_DATA;
  else if (wxStringEq(xMethod, "External XPM file"))
    bitmapTypeX = wxBITMAP_TYPE_XPM;
  else if (wxStringEq(xMethod, "External GIF file"))
    bitmapTypeX = wxBITMAP_TYPE_GIF;
  else if (wxStringEq(xMethod, "External BMP file"))
    bitmapTypeX = wxBITMAP_TYPE_BMP;
  else
    bitmapTypeX = wxBITMAP_TYPE_XBM_DATA;

  delete[] xMethod;
  delete[] windowsMethod;
  delete[] oldName;
  
  return TRUE;
}

void BuildBitmapData::SetBitmapName(char *name)
{
  if (bitmapName)
    delete[] bitmapName;
  if (name)
    bitmapName = copystring(name);
  else
    bitmapName = NULL;
}

PrologExpr *BuildBitmapData::WriteList(void)
{
  PrologExpr *bitmapExpr = new PrologExpr(PrologList);
  bitmapExpr->Append(new PrologExpr(PrologString, bitmapName));
  bitmapExpr->Append(new PrologExpr((long)bitmapTypeWindows));
  bitmapExpr->Append(new PrologExpr((long)bitmapTypeX));
  return bitmapExpr;
}

Bool BuildBitmapData::ReadList(PrologExpr *bitmapExpr)
{
  PrologExpr *nameExpr = bitmapExpr->Nth(0);
  PrologExpr *windowsExpr = bitmapExpr->Nth(1);
  PrologExpr *xExpr = bitmapExpr->Nth(2);
  if (nameExpr)
    SetBitmapName(nameExpr->StringValue());
  if (windowsExpr)
    SetTypeWindows((int)windowsExpr->IntegerValue());
  if (xExpr)
    SetTypeX((int)xExpr->IntegerValue());
  return TRUE;
}

Bool BuildBitmapData::WritePrologAttributes(PrologExpr *expr)
{
  PrologExpr *bitmapExpr = new PrologExpr(PrologList);
  bitmapExpr->Append(new PrologExpr(PrologString, bitmapName));
  bitmapExpr->Append(new PrologExpr((long)bitmapTypeWindows));
  bitmapExpr->Append(new PrologExpr((long)bitmapTypeX));
  if (bitmapIsIcon)
    expr->AddAttributeValue("icon", WriteList());
  else
    expr->AddAttributeValue("bitmap", WriteList());
  return TRUE;
}

Bool BuildBitmapData::ReadPrologAttributes(PrologExpr *expr)
{
  PrologExpr *bitmapExpr = NULL;
  if (bitmapIsIcon)
    expr->AssignAttributeValue("icon", &bitmapExpr);
  else
    expr->AssignAttributeValue("bitmap", &bitmapExpr);
  if (!bitmapExpr)
    return FALSE;
  return ReadList(bitmapExpr);
}

// Generate bitmap, icon etc. resource entries for RC file.
void BuildBitmapData::GenerateResourceEntry(ostream &stream)
{
  char buf2[100];
  char buf3[300];
  if (bitmapName  && (strlen(bitmapName) > 0) && !bitmapIsIcon && (bitmapTypeWindows == wxBITMAP_TYPE_BMP_RESOURCE) && !buildApp.resourcesGenerated.Member(bitmapName))
  {
    strcpy(buf2, GetBitmapName());
    strcat(buf2, ".bmp");
    stream << GetBitmapName() << " BITMAP " << buf2 << "\n";

    // Check if it exists
#ifdef wx_msw
    strcpy(buf3, buildApp.projectDirDOS);
    strcat(buf3, "\\");
#endif
#ifdef wx_x
    strcpy(buf3, buildApp.projectDirUNIX);
    strcat(buf3, "/");
#endif
    strcat(buf3, GetBitmapName());
    strcat(buf3, ".bmp");
    if (!FileExists(buf3))
    {
      Report("Warning: bitmap file "); Report(buf3); Report(" not found.\n");
    }
  }
  else if (bitmapName  && (strlen(bitmapName) > 0) && bitmapIsIcon && (bitmapTypeWindows == wxBITMAP_TYPE_ICO_RESOURCE) && !buildApp.resourcesGenerated.Member(bitmapName))
  {
    strcpy(buf2, GetBitmapName());
    strcat(buf2, ".ico");
    stream << GetBitmapName() << " ICON " << buf2 << "\n";

    // Check if it exists
#ifdef wx_msw
    strcpy(buf3, buildApp.projectDirDOS);
    strcat(buf3, "\\");
#endif
#ifdef wx_x
    strcpy(buf3, buildApp.projectDirUNIX);
    strcat(buf3, "/");
#endif
    strcat(buf3, GetBitmapName());
    strcat(buf3, ".ico");
    if (!FileExists(buf3))
    {
      Report("Warning: icon file "); Report(buf3); Report(" not found.\n");
    }
  }
}

void BuildBitmapData::GenerateWXResourceBitmap(ostream& stream)
{
  if (bitmapName && (strlen(bitmapName) > 0) && !buildApp.resourcesGenerated.Member(bitmapName))
  {
    char *which = "bitmap";
    if (bitmapIsIcon)
      which = "icon";
      
    stream << "static char *" << GetBitmapName() << "_resource = \"" << which << "(name = '" << GetBitmapName() << "_resource',\\\n";
    switch (bitmapTypeWindows)
    {
       case wxBITMAP_TYPE_XPM_DATA:
        // XPM data
        stream << "  " << which << " = ['" << GetBitmapName() << "_data', wxBITMAP_TYPE_XPM_DATA, 'WINDOWS'],\\\n";
        break;
       case wxBITMAP_TYPE_XPM:
        // XPM file
        stream << "  " << which << " = ['" << GetBitmapName() << ".xpm', wxBITMAP_TYPE_XPM, 'WINDOWS'],\\\n";
        break;
       case wxBITMAP_TYPE_BMP:
        // BMP file
        stream << "  " << which << " = ['" << GetBitmapName() << ".bmp', wxBITMAP_TYPE_BMP, 'WINDOWS'],\\\n";
        break;
       case wxBITMAP_TYPE_ICO:
        // ICO file
        stream << "  " << which << " = ['" << GetBitmapName() << ".ico', wxBITMAP_TYPE_ICO, 'WINDOWS'],\\\n";
        break;
       case wxBITMAP_TYPE_ICO_RESOURCE:
        // ICO resource
        stream << "  " << which << " = ['" << GetBitmapName() << "', wxBITMAP_TYPE_ICO_RESOURCE, 'WINDOWS'],\\\n";
        break;
       case wxBITMAP_TYPE_BMP_RESOURCE:
       default:
        // A Windows bitmap alternative... a Windows resource identifier (put in .rc)
        stream << "  " << which << " = ['" << GetBitmapName() << "', wxBITMAP_TYPE_BMP_RESOURCE, 'WINDOWS'],\\\n";
        break;
    }
    switch (bitmapTypeX)
    {
      case wxBITMAP_TYPE_BMP:
        // A BMP file
        stream << "  " << which << " = ['" << GetBitmapName() << ".bmp', wxBITMAP_TYPE_BMP, 'X']).\";\n\n";
        break;
      case wxBITMAP_TYPE_GIF:
        // A GIF file
        stream << "  " << which << " = ['" << GetBitmapName() << ".gif', wxBITMAP_TYPE_GIF, 'X']).\";\n\n";
        break;
      case wxBITMAP_TYPE_XPM:
        // An XPM file
        stream << "  " << which << " = ['" << GetBitmapName() << ".xpm', wxBITMAP_TYPE_XPM, 'X']).\";\n\n";
        break;
      case wxBITMAP_TYPE_XPM_DATA:        
        // An XPM file included in the main program file
        stream << "  " << which << " = ['" << GetBitmapName() << "_data', wxBITMAP_TYPE_XPM_DATA, 'X']).\";\n\n";
        break;
      case wxBITMAP_TYPE_XBM:
        // An XBM file
        stream << "  " << which << " = ['" << GetBitmapName() << ".xpm', wxBITMAP_TYPE_XBM, 'X']).\";\n\n";
        break;
      case wxBITMAP_TYPE_XBM_DATA:
      default:
        // An X bitmap alternative... an XBM file included in the main program file
        stream << "  " << which << " = ['" << GetBitmapName() << "_data', wxBITMAP_TYPE_XBM_DATA, 'X']).\";\n\n";
        break;
    }
    buildApp.resourcesGenerated.Add(bitmapName);
  }
}

void BuildBitmapData::GenerateWXResourceBitmapRegistration(ostream& stream)
{
  if (bitmapName && (strlen(bitmapName) > 0))
  {
    // If we have XPM data for both cases, we don't have to conditionally compile:
    // looks neater.
    if ((bitmapTypeX == wxBITMAP_TYPE_XPM_DATA) && (bitmapTypeWindows == wxBITMAP_TYPE_XPM_DATA))
    {
      if (!buildApp.resourcesGenerated.Member(bitmapName))
      {
        stream << "  wxResourceRegisterBitmapData(\"" << GetBitmapName() << "_xpm\", " << GetBitmapName() << "_data);\n";
        buildApp.resourcesGenerated.Add(bitmapName);
      }
    }
    else
    {
      if (!buildApp.resourcesGenerated.Member(bitmapName))
      {
        switch (bitmapTypeX)
        {
          case wxBITMAP_TYPE_XBM_DATA:
            stream << "#ifdef wx_x\n";
            stream << "  wxResourceRegisterBitmapData(\"" << GetBitmapName() << "_data\", " << GetBitmapName() << "_bits, ";
            stream << GetBitmapName() << "_width, " << GetBitmapName() << "_height);\n";
            stream << "#endif\n";
            break;
          case wxBITMAP_TYPE_XPM_DATA:
            stream << "#ifdef wx_x\n";
            stream << "  wxResourceRegisterBitmapData(\"" << GetBitmapName() << "_data\", " << GetBitmapName() << "_xpm);\n";
            stream << "#endif\n";
            break;
          default:
            break;
        }
        switch (bitmapTypeWindows)
        {
          case wxBITMAP_TYPE_XPM_DATA:
            stream << "#ifdef wx_msw\n";
            stream << "  wxResourceRegisterBitmapData(\"" << GetBitmapName() << "_data\", " << GetBitmapName() << "_xpm);\n";
            stream << "#endif\n";
            break;
          default:
            break;
        }
        buildApp.resourcesGenerated.Add(bitmapName);
      }
    }
  }
}

// Write .wxr resource loading code
void BuildBitmapData::GenerateResourceLoading(ostream& stream)
{
  if (bitmapName && (strlen(bitmapName) > 0) && !buildApp.resourcesGenerated.Member(bitmapName))
  {
    stream << "  wxResourceParseData(" << bitmapName << "_resource);\n";
    buildApp.resourcesGenerated.Add(bitmapName);
  }
}

void BuildBitmapData::GenerateBitmapDataInclude(ostream& stream)
{
  if (bitmapName && (strlen(bitmapName) > 0))
  {
    // If we have XPM data for both cases, we don't have to conditionally compile:
    // looks neater.
    if ((bitmapTypeX == wxBITMAP_TYPE_XPM_DATA) && (bitmapTypeWindows == wxBITMAP_TYPE_XPM_DATA))
    {
      if (!buildApp.resourcesGenerated.Member(bitmapName))
      {
        stream << "#include \"" << GetBitmapName() << ".xpm\"\n\n";
        buildApp.resourcesGenerated.Add(bitmapName);
      }
    }
    else
    {
      if (!buildApp.resourcesGenerated.Member(bitmapName))
      {
        switch (bitmapTypeX)
        {
          case wxBITMAP_TYPE_XBM_DATA:
            stream << "#ifdef wx_x\n";
            stream << "#include \"" << GetBitmapName() << ".xbm\"\n";
            stream << "#endif\n\n";
            break;
          case wxBITMAP_TYPE_XPM_DATA:
            stream << "#ifdef wx_x\n";
            stream << "#include \"" << GetBitmapName() << ".xpm\"\n";
            stream << "#endif\n\n";
            break;
          default:
            break;
        }
        switch (bitmapTypeWindows)
        {
          case wxBITMAP_TYPE_XPM_DATA:
            stream << "#ifdef wx_msw\n";
            stream << "#include \"" << GetBitmapName() << ".xpm\"\n";
            stream << "#endif\n\n";
            break;
          default:
            break;
        }
        buildApp.resourcesGenerated.Add(bitmapName);
      }
    }
  }
}

