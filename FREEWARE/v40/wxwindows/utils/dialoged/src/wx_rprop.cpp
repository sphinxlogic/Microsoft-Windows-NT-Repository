/*
 * File:	wx_rprop.cc
 * Purpose:	Resource editor property implementations
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"

#include "wx_scrol.h"
#include "wx_check.h"
#include "wx_buttn.h"
#include "wx_choic.h"
#include "wx_lbox.h"
#include "wx_rbox.h"
#include "wx_group.h"
#include "wx_gauge.h"
#include "wx_slidr.h"
#include "wx_txt.h"
#include "wx_mtxt.h"
#endif

#if defined(__BORLANDC__) && !USE_GNU_WXSTRING
#error "Please use the GNU version of wxString because of no 'const char *' operator in wxWin wxString."
#endif

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if defined(wx_msw) && !defined(GNUWIN32)
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include "wx_resed.h"
#include "wx_repal.h"
#include "wx_rprop.h"

#include "wx_scrol.h"

#ifdef wx_msw
#if FAFA_LIB
#include "../../contrib/fafa/fafa.h"
extern HBRUSH SetupBackground(HWND wnd) ; // in wx_main.cpp
#endif
#endif

// Causes immediate feedback.
void wxResourcePropertyListView::OnPropertyChanged(wxProperty *property)
{
  // Sets the value of the property back into the actual object,
  // IF the property value was modified.
  if (property->GetValue().GetModified())
  {
    propertyInfo->SetProperty(property->GetName(), property);
    property->GetValue().SetModified(FALSE);
  }
}

Bool wxResourcePropertyListView::OnClose(void)
{
  GetManagedWindow()->GetSize(&(wxResourceManager::currentResourceManager->propertyWindowSize.width),
          &(wxResourceManager::currentResourceManager->propertyWindowSize.height));
  GetManagedWindow()->GetPosition(&(wxResourceManager::currentResourceManager->propertyWindowSize.x),
              &(wxResourceManager::currentResourceManager->propertyWindowSize.y));
  return wxPropertyListView::OnClose();
}

/*
 * wxPropertyInfo
 */

// Edit the information represented by this object, whatever that
// might be.
Bool wxPropertyInfo::Edit(wxWindow *parent, char *title)
{
  int width = wxResourceManager::currentResourceManager->propertyWindowSize.width;
  int height = wxResourceManager::currentResourceManager->propertyWindowSize.height;
  int x = wxResourceManager::currentResourceManager->propertyWindowSize.x;
  int y = wxResourceManager::currentResourceManager->propertyWindowSize.y;
  
  wxPropertySheet *propSheet = new wxPropertySheet;

  wxStringList propNames;
  GetPropertyNames(propNames);

  wxNode *node = propNames.First();
  while (node)
  {
    wxString name = (char *)node->Data();
    wxProperty *prop = GetProperty(name);
    if (prop)
    {
      propSheet->AddProperty(prop);
    }
    node = node->Next();
  }
  
  // Reset 'modified' flags for all property values
  propSheet->SetAllModified(FALSE);

  if (!title)
    title = "Properties";
  wxResourcePropertyListView *view = new wxResourcePropertyListView(this, NULL,
#ifdef wx_xview
     wxPROP_BUTTON_OK | wxPROP_BUTTON_CANCEL |
#endif
     wxPROP_BUTTON_CHECK_CROSS|wxPROP_DYNAMIC_VALUE_FIELD|wxPROP_PULLDOWN|wxPROP_SHOWVALUES);

  wxPropertyListDialog *propDialog = new wxPropertyListDialog(view, NULL, title, TRUE, x, y, width, height);
  
  wxPropertyValidatorRegistry theValidatorRegistry;
  theValidatorRegistry.RegisterValidator((wxString)"real", new wxRealListValidator);
  theValidatorRegistry.RegisterValidator((wxString)"string", new wxStringListValidator);
  theValidatorRegistry.RegisterValidator((wxString)"integer", new wxIntegerListValidator);
  theValidatorRegistry.RegisterValidator((wxString)"bool", new wxBoolListValidator);
  theValidatorRegistry.RegisterValidator((wxString)"filename", new wxFilenameListValidator);
  theValidatorRegistry.RegisterValidator((wxString)"stringlist", new wxListOfStringsListValidator);

//  view->propertyWindow = propDialog;
  view->AddRegistry(&theValidatorRegistry);
  view->ShowView(propSheet, propDialog);

  if (!propDialog->Show(TRUE) || wxPropertyListView::dialogCancelled)
  {
    delete propSheet;
    return FALSE;
  }

  // Unnecessary if we're giving immediate feedback.
/*
  // Now put the properties back into the object associated with wxPropertyInfo.
  node = propNames.First();
  while (node)
  {
    wxString name = (char *)node->Data();
    wxProperty *prop = propSheet->GetProperty(name);

    // Sets the value of the property back into the actual object,
    // IF the property value was modified.
    if (prop && prop->GetValue().GetModified())
      SetProperty(name, prop);
    
    node = node->Next();
  }
*/

  delete propSheet;
  return TRUE;
}

/*
 * wxWindowPropertyInfo
 */

wxWindowPropertyInfo::wxWindowPropertyInfo(wxWindow *win, wxItemResource *res)
{
  propertyWindow = win;
  propertyResource = res;
}

wxWindowPropertyInfo::~wxWindowPropertyInfo(void)
{
}

wxProperty *wxWindowPropertyInfo::GetFontProperty(wxString& name, wxFont *font)
{
  if (!font)
    return NULL;
    
  if (name.Contains("Points"))
    return new wxProperty(name, (long)font->GetPointSize(), "integer", new wxIntegerListValidator(1, 100));
  else if (name.Contains("Family"))
    return new wxProperty(name, font->GetFamilyString(), "string",
       new wxStringListValidator(new wxStringList("wxDECORATIVE", "wxROMAN", "wxSCRIPT", "wxSWISS", "wxMODERN",
          NULL)));
  else if (name.Contains("Style"))
    return new wxProperty(name, font->GetStyleString(), "string",
       new wxStringListValidator(new wxStringList("wxNORMAL", "wxITALIC", "wxSLANT", NULL)));
  else if (name.Contains("Weight"))
    return new wxProperty(name, font->GetWeightString(), "string",
       new wxStringListValidator(new wxStringList("wxNORMAL", "wxBOLD", "wxLIGHT", NULL)));
  else if (name.Contains("Underlined"))
    return new wxProperty(name, (Bool)font->GetUnderlined(), "bool");
  else
    return NULL;
}

wxFont *wxWindowPropertyInfo::SetFontProperty(wxString& name, wxProperty *property, wxFont *font)
{
  int pointSize = 12;
  int fontFamily = wxMODERN;
  int fontStyle = wxNORMAL;
  int fontWeight = wxNORMAL;
  Bool fontUnderlined = FALSE;

  if (name.Contains("Points"))
  {
    pointSize = (int)property->GetValue().IntegerValue();
    if (font && (pointSize == font->GetPointSize()))
      return NULL; // No change
  }
  else if (font) pointSize = font->GetPointSize();

  if (name.Contains("Family"))
  {
    wxString val = property->GetValue().StringValue();
    fontFamily = wxStringToFontFamily(val);
    
    if (font && (fontFamily == font->GetFamily()))
      return NULL; // No change
  }
  else if (font) fontFamily = font->GetFamily();

  if (name.Contains("Style"))
  {
    wxString val = property->GetValue().StringValue();
    fontStyle = wxStringToFontStyle(val);
    
    if (font && (fontStyle == font->GetStyle()))
      return NULL; // No change
  }
  else if (font) fontStyle = font->GetStyle();
  if (name.Contains("Weight"))
  {
    wxString val = property->GetValue().StringValue();
    fontWeight = wxStringToFontWeight(val);
      
    if (font && (fontWeight == font->GetWeight()))
      return NULL; // No change
  }
  else if (font) fontWeight = font->GetWeight();
      
  if (name.Contains("Underlined"))
  {
    fontUnderlined = property->GetValue().BoolValue();
      
    if (font && (fontUnderlined == font->GetUnderlined()))
      return NULL; // No change
  }
  else if (font) fontUnderlined = font->GetUnderlined();

  wxFont *newFont = wxTheFontList->FindOrCreateFont(pointSize, fontFamily, fontStyle, fontWeight, fontUnderlined);
  if (newFont)
  {
    return newFont;
  }
  else
    return NULL;
}

wxProperty *wxWindowPropertyInfo::GetProperty(wxString& name)
{
  wxFont *font = propertyWindow->GetFont();
  if (name == "fontPoints" || name == "fontFamily" || name == "fontStyle" || name == "fontWeight" ||
      name == "fontUnderlined")
    return GetFontProperty(name, font);
  else if (name == "name")
    return new wxProperty("name", propertyWindow->GetName(), "string");
  else if (name == "title")
    return new wxProperty("title", propertyWindow->GetTitle(), "string");
  else if (name == "x")
  {
    int x, y;
    propertyWindow->GetPosition(&x, &y);
    return new wxProperty("x", (long)x, "integer");
  }
  else if (name == "y")
  {
    int x, y;
    propertyWindow->GetPosition(&x, &y);
    return new wxProperty("y", (long)y, "integer");
  }
  else if (name == "width")
  {
    int width, height;
    propertyWindow->GetSize(&width, &height);
    return new wxProperty("width", (long)width, "integer");
  }
  else if (name == "height")
  {
    int width, height;
    propertyWindow->GetSize(&width, &height);
    return new wxProperty("height", (long)height, "integer");
  }
  else
    return NULL;
}

Bool wxWindowPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxFont *font = propertyWindow->GetFont();
  if (font && (name == "fontPoints" || name == "fontFamily" || name == "fontStyle" || name == "fontWeight" || name == "fontUnderlined" ))
  {
    wxFont *newFont = SetFontProperty(name, property, font);
    if (newFont)
      propertyWindow->SetFont(newFont);
    return TRUE;
  }
  else if (name == wxString("name"))
  {
    // Remove old name from resource table, if it's there.
    wxItemResource *oldResource = (wxItemResource *)wxResourceManager::currentResourceManager->GetResourceTable().Delete(propertyWindow->GetName());
    if (oldResource)
    {
      // It's a top-level resource
      propertyWindow->SetName(property->GetValue().StringValue());
      oldResource->SetName(property->GetValue().StringValue());
      wxResourceManager::currentResourceManager->GetResourceTable().Put(propertyWindow->GetName(), oldResource);
    }
    else
    {
      // It's a child of something; just set the name of the resource and the window.
      propertyWindow->SetName(property->GetValue().StringValue());
      propertyResource->SetName(property->GetValue().StringValue());
    }
    // Refresh the resource manager list, because the name changed.
    wxResourceManager::currentResourceManager->UpdateResourceList();
    return TRUE;
  }
  else if (name == wxString("title"))
  {
    propertyWindow->SetTitle(property->GetValue().StringValue());
    return TRUE;
  }
  else if (name == wxString("x"))
  {
    int x, y;
    propertyWindow->GetPosition(&x, &y);
    int newX = (int)property->GetValue().IntegerValue();
    if (x != newX)
      propertyWindow->Move(newX, y);
    return TRUE;
  }
  else if (name == wxString("y"))
  {
    int x, y;
    propertyWindow->GetPosition(&x, &y);
    int newY = (int)property->GetValue().IntegerValue();
    if (y != newY)
      propertyWindow->Move(x, newY);
    return TRUE;
  }
  else if (name == wxString("width"))
  {
    int width, height;
    propertyWindow->GetSize(&width, &height);
    int newWidth = (int)property->GetValue().IntegerValue();
    if (width != newWidth)
    {
      propertyWindow->SetSize(newWidth, height);
      if (propertyWindow->IsKindOf(CLASSINFO(wxPanel)) && !propertyWindow->IsKindOf(CLASSINFO(wxDialogBox)))
      {
        propertyWindow->GetParent()->SetClientSize(newWidth, height);
      }
    }
    return TRUE;
  }
  else if (name == wxString("height"))
  {
    int width, height;
    propertyWindow->GetSize(&width, &height);
    int newHeight = (int)property->GetValue().IntegerValue();
    if (height != newHeight)
    {
      propertyWindow->SetSize(width, newHeight);
      if (propertyWindow->IsKindOf(CLASSINFO(wxPanel)) && !propertyWindow->IsKindOf(CLASSINFO(wxDialogBox)))
      {
        propertyWindow->GetParent()->SetClientSize(width, newHeight);
      }
    }
    return TRUE;
  }
  else
    return FALSE;
}

void wxWindowPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("name");
  names.Add("x");
  names.Add("y");
  names.Add("width");
  names.Add("height");
  if (!propertyWindow->IsKindOf(CLASSINFO(wxItem)))
  {
    names.Add("fontPoints");
    names.Add("fontFamily");
    names.Add("fontStyle");
    names.Add("fontWeight");
    names.Add("fontUnderlined");
  }
}

// Fill in the wxItemResource members to mirror the current window settings
Bool wxWindowPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  resource->SetType(propertyWindow->GetClassInfo()->GetClassName());
  
//  resource->SetStyle(propertyWindow->GetWindowStyleFlag());
  if (propertyWindow->GetName())
    resource->SetName(propertyWindow->GetName());
  int x, y, w, h;
  propertyWindow->GetSize(&w, &h);

#ifdef wx_msw
#if FAFA_LIB
	if (!strcmp(resource->GetType(),"wxButton") && resource->GetValue4() )
	  	{
		  w -= FB_MARGIN;
		  h -= FB_MARGIN;
		}
#endif
#endif

  propertyWindow->GetPosition(&x, &y);
  resource->SetSize(x, y, w, h);
  return TRUE;
}


/*
 * Panel items
 */

wxProperty *wxItemPropertyInfo::GetProperty(wxString& name)
{
  wxItem *itemWindow = (wxItem *)propertyWindow; 
  wxFont *labelFont = itemWindow->GetLabelFont();
  wxFont *buttonFont = itemWindow->GetButtonFont();
  
  if (name == "labelFontPoints" || name == "labelFontFamily" || name == "labelFontStyle" || name == "labelFontWeight" ||
      name == wxString("labelFontUnderlined"))
    return GetFontProperty(name, labelFont);
  else if (name == "buttonFontPoints" || name == "buttonFontFamily" || name == "buttonFontStyle" || name == "buttonFontWeight" ||
      name == "buttonFontUnderlined")
    return GetFontProperty(name, buttonFont);
  else if (name == "label" && itemWindow->GetLabel())
    return new wxProperty("label", propertyWindow->GetLabel(), "string");
  else if (name == "labelOrientation")
  {
    char *pos = NULL;
    if (itemWindow->GetLabelPosition() == wxHORIZONTAL)
      pos = "wxHORIZONTAL";
    else
      pos = "wxVERTICAL";

    return new wxProperty("labelOrientation", pos, "string",
       new wxStringListValidator(new wxStringList("wxHORIZONTAL", "wxVERTICAL",
          NULL)));
  }
  else
    return wxWindowPropertyInfo::GetProperty(name);
}

Bool wxItemPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxItem *itemWindow = (wxItem *)propertyWindow; 
  wxFont *labelFont = itemWindow->GetLabelFont();
  wxFont *buttonFont = itemWindow->GetButtonFont();
  
  if (labelFont && (name == "labelFontPoints" || name == "labelFontFamily" || name == "labelFontStyle" || name == "labelFontWeight" || name == "labelFontUnderlined" ))
  {
    wxFont *newFont = SetFontProperty(name, property, labelFont);
    if (newFont)
      itemWindow->SetLabelFont(newFont);
    return TRUE;
  }
  else if (buttonFont && (name == "buttonFontPoints" || name == "buttonFontFamily" || name == wxString("buttonFontStyle") || name == "buttonFontWeight" || name == "buttonFontUnderlined" ))
  {
    wxFont *newFont = SetFontProperty(name, property, buttonFont);
    if (newFont)
      itemWindow->SetButtonFont(newFont);
    return TRUE;
  }
  else if (name == "label")
  {
    itemWindow->SetLabel(property->GetValue().StringValue());
    return TRUE;
  }
  else if (name == "labelOrientation")
  {
//    wxPanel *parent = (wxPanel *)itemWindow->GetParent();
//    int panelPosition = parent->GetLabelPosition();
    
    int pos = 0;
    if (wxString(property->GetValue().StringValue()) == "wxHORIZONTAL")
      pos = wxHORIZONTAL;
    else if (wxString(property->GetValue().StringValue()) == "wxVERTICAL")
      pos = wxVERTICAL;
/*
    // Default
    else
      pos = panelPosition;
*/
    if (pos != itemWindow->GetLabelPosition())
    {
      // Anything that changes the window style must change the
      // resource explicitly.
      wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(itemWindow);
      itemWindow->SetLabelPosition( pos );
      resource->SetStyle(itemWindow->GetWindowStyleFlag());

      // Under Windows, this can be done to force redrawing
      // of the label/value in the new configuration.
      // Unfortunately, under other systems,
      // you have to recreate the whole item.
#ifdef wx_msw
      int x, y, w, h;
      itemWindow->GetPosition(&x, &y);
      itemWindow->GetSize(&w, &h);
      itemWindow->SetSize(x, y, w, h);
#else
      // Must write code to recreate the item from the resource :-(
#endif
      itemWindow->Refresh();
    }
    return TRUE;
  }
  else
    return wxWindowPropertyInfo::SetProperty(name, property);
}

void wxItemPropertyInfo::GetPropertyNames(wxStringList& names)
{
  wxWindowPropertyInfo::GetPropertyNames(names);
  
  names.Add("labelFontPoints");
  names.Add("labelFontFamily");
  names.Add("labelFontStyle");
  names.Add("labelFontWeight");
  names.Add("labelFontUnderlined");
  names.Add("buttonFontPoints");
  names.Add("buttonFontFamily");
  names.Add("buttonFontStyle");
  names.Add("buttonFontWeight");
  names.Add("buttonFontUnderlined");
}

Bool wxItemPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxWindowPropertyInfo::InstantiateResource(resource);
  
  wxItem *item = (wxItem *)propertyWindow;
  if (item->GetLabel())
    resource->SetTitle(item->GetLabel());
  if (item->GetLabelFont())
    resource->SetLabelFont(item->GetLabelFont());
  if (item->GetButtonFont())
    resource->SetButtonFont(item->GetButtonFont());
  return TRUE;
}

/*
 * Button
 */

wxProperty *wxButtonPropertyInfo::GetProperty(wxString& name)
{
  wxButton *button = (wxButton *)propertyWindow;
  if (name == "label" && isBitmapButton)
  {
    wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(button);
    wxString str("none.bmp");
   
    if (resource)
    {
      char *filename = wxResourceManager::currentResourceManager->FindBitmapFilenameForResource(resource);
      if (filename)
        str = filename;
    }
    return new wxProperty("label", (char *)(const char *)str, "string", new wxFilenameListValidator("Select a bitmap file", "*.bmp"));
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

Bool wxButtonPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxButton *button = (wxButton *)propertyWindow;
  if (name == "label" && isBitmapButton)
  {
    char *s = property->GetValue().StringValue();
    if (s && wxFileExists(s))
    {
      s = copystring(s);
      wxBitmap *bitmap = new wxBitmap(s, wxBITMAP_TYPE_BMP);
      if (!bitmap->Ok())
      {
        delete bitmap;
        delete[] s;
        return FALSE;
      }
      else
      {
        wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(button);
        if (resource)
        {
          wxString oldResource(resource->GetValue4());
          char *resName = wxResourceManager::currentResourceManager->AddBitmapResource(s);
          resource->SetValue4(resName);
          
          if (!oldResource.IsNull())
            wxResourceManager::currentResourceManager->PossiblyDeleteBitmapResource((char *)(const char *)oldResource);
        }

        button->SetLabel(bitmap);
        delete[] s;
        return TRUE;
      }
    }
    return FALSE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxButtonPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  wxItemPropertyInfo::GetPropertyNames(names);
}

Bool wxButtonPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Message
 */

wxProperty *wxMessagePropertyInfo::GetProperty(wxString& name)
{
  wxMessage *message = (wxMessage *)propertyWindow;
  
  if (name == "label" && isBitmapMessage)
  {
    wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(message);
    wxString str("none.bmp");
   
    if (resource)
    {
      char *filename = wxResourceManager::currentResourceManager->FindBitmapFilenameForResource(resource);
      if (filename)
        str = filename;
    }
    return new wxProperty("label", (char *)(const char *)str, "string", new wxFilenameListValidator("Select a bitmap file", "*.bmp"));
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

Bool wxMessagePropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxMessage *message = (wxMessage *)propertyWindow;
  if (name == "label" && isBitmapMessage)
  {
    char *s = property->GetValue().StringValue();
    if (s && wxFileExists(s))
    {
      s = copystring(s);
      
      wxBitmap *bitmap = new wxBitmap(s, wxBITMAP_TYPE_BMP);
      if (!bitmap->Ok())
      {
        delete bitmap;
        delete[] s;
        return FALSE;
      }
      else
      {
        wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(message);
        if (resource)
        {
          wxString oldResource(resource->GetValue4());
          char *resName = wxResourceManager::currentResourceManager->AddBitmapResource(s);
          resource->SetValue4(resName);
          
          if (!oldResource.IsNull())
            wxResourceManager::currentResourceManager->PossiblyDeleteBitmapResource((char *)(const char *)oldResource);
        }

        message->SetLabel(bitmap);
        delete[] s;
        return TRUE;
      }
    }
    return FALSE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxMessagePropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  wxItemPropertyInfo::GetPropertyNames(names);
}

Bool wxMessagePropertyInfo::InstantiateResource(wxItemResource *resource)
{
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Text item
 */

wxProperty *wxTextPropertyInfo::GetProperty(wxString& name)
{
  wxText *text = (wxText *)propertyWindow;
  if (name == "value")
    return new wxProperty("value", text->GetValue(), "string");
  else if (name == "password")
  {
    Bool isPassword = ((text->GetWindowStyleFlag() & wxTE_PASSWORD) == wxTE_PASSWORD);
    return new wxProperty("password", isPassword, "bool");
  }
  else if (name == "readonly")
  {
    Bool isReadOnly = ((text->GetWindowStyleFlag() & wxTE_READONLY) == wxTE_READONLY);
    return new wxProperty("readonly", isReadOnly, "bool");
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

Bool wxTextPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxText *text = (wxText *)propertyWindow;
  if (name == "value")
  {
    text->SetValue(property->GetValue().StringValue());
    return TRUE;
  }
  else if (name == "password")
  {
    long flag = text->GetWindowStyleFlag();
    if (property->GetValue().BoolValue())
    {
      if ((flag & wxTE_PASSWORD) != wxTE_PASSWORD)
        flag |= wxTE_PASSWORD;
    }
    else
    {
      if ((flag & wxTE_PASSWORD) == wxTE_PASSWORD)
        flag -= wxTE_PASSWORD;
    }
    wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(text);
    resource->SetStyle(flag);

    wxResourceManager::currentResourceManager->RecreateWindowFromResource(text, this);
    return TRUE;
  }
  else if (name == "readonly")
  {
    long flag = text->GetWindowStyleFlag();
    if (property->GetValue().BoolValue())
    {
      if ((flag & wxTE_READONLY) != wxTE_READONLY)
        flag |= wxTE_READONLY;
    }
    else
    {
      if ((flag & wxTE_READONLY) == wxTE_READONLY)
        flag -= wxTE_READONLY;
    }
    wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(text);
    resource->SetStyle(flag);

    wxResourceManager::currentResourceManager->RecreateWindowFromResource(text, this);
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxTextPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  names.Add("value");
  names.Add("labelOrientation");
  names.Add("readonly");
  names.Add("password");
  wxItemPropertyInfo::GetPropertyNames(names);  
}

Bool wxTextPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxText *text = (wxText *)propertyWindow;
  if (text->GetValue())
    resource->SetValue4(text->GetValue());
    
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Listbox item
 */

wxProperty *wxListBoxPropertyInfo::GetProperty(wxString& name)
{
  wxListBox *listBox = (wxListBox *)propertyWindow;
  if (name == "values")
  {
    wxStringList *stringList = new wxStringList;
    int i;
    for (i = 0; i < listBox->Number(); i++)
      stringList->Add(listBox->GetString(i));

    return new wxProperty(name, stringList, "stringlist");
  }
  else if (name == "multiple")
  {
    char *pos = NULL;
    wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(listBox);
    if (!resource)
      return NULL;
      
    char *mult = "wxSINGLE";
      
    switch (resource->GetValue1())
    {
      case wxMULTIPLE:
        mult = "wxMULTIPLE";
        break;
      case wxEXTENDED:
        mult = "wxEXTENDED";
        break;
      default:
      case wxSINGLE:
        mult = "wxSINGLE";
        break;
    }
    return new wxProperty("multiple", mult, "string",
       new wxStringListValidator(new wxStringList("wxSINGLE", "wxMULTIPLE", "wxEXTENDED",
          NULL)));
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

Bool wxListBoxPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxListBox *listBox = (wxListBox *)propertyWindow;
  if (name == "values")
  {
    listBox->Clear();
    wxPropertyValue *expr = property->GetValue().GetFirst();
    while (expr)
    {
      char *s = expr->StringValue();
      if (s)
        listBox->Append(s);
      expr = expr->GetNext();
    }
    return TRUE;
  }
  else if (name == "multiple")
  {
    int mult = wxSINGLE;
    wxString str(property->GetValue().StringValue());
    if (str == "wxMULTIPLE")
      mult = wxMULTIPLE;
    else if (str == "wxEXTENDED")
      mult = wxEXTENDED;
    else
      mult = wxSINGLE;
    wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(listBox);
    if (resource)
      resource->SetValue1(mult);
    wxResourceManager::currentResourceManager->RecreateWindowFromResource(listBox, this);
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxListBoxPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  names.Add("values");
  names.Add("labelOrientation");
  names.Add("multiple");
  wxItemPropertyInfo::GetPropertyNames(names);  
}

Bool wxListBoxPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxListBox *lbox = (wxListBox *)propertyWindow;
  // This will be set for the wxItemResource on reading or in SetProperty
//  resource->SetValue1(lbox->GetSelectionMode());
  int i;
  if (lbox->Number() == 0)
    resource->SetStringValues(NULL);
  else
  {
    wxStringList *slist = new wxStringList;
    
    for (i = 0; i < lbox->Number(); i++)
      slist->Add(lbox->GetString(i));
      
    resource->SetStringValues(slist);
  }
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Choice item
 */

wxProperty *wxChoicePropertyInfo::GetProperty(wxString& name)
{
  wxChoice *choice = (wxChoice *)propertyWindow;
  if (name == "values")
  {
    wxStringList *stringList = new wxStringList;
    int i;
    for (i = 0; i < choice->Number(); i++)
      stringList->Add(choice->GetString(i));

    return new wxProperty(name, stringList, "stringlist");
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

Bool wxChoicePropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxChoice *choice = (wxChoice *)propertyWindow;
  if (name == "values")
  {
    choice->Clear();
    wxPropertyValue *expr = property->GetValue().GetFirst();
    while (expr)
    {
      char *s = expr->StringValue();
      if (s)
        choice->Append(s);
      expr = expr->GetNext();
    }
    if (choice->Number() > 0)
      choice->SetSelection(0);
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxChoicePropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  names.Add("values");
  names.Add("labelOrientation");
  wxItemPropertyInfo::GetPropertyNames(names);  
}

Bool wxChoicePropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxChoice *choice = (wxChoice *)propertyWindow;
  int i;
  if (choice->Number() == 0)
    resource->SetStringValues(NULL);
  else
  {
    wxStringList *slist = new wxStringList;
    
    for (i = 0; i < choice->Number(); i++)
      slist->Add(choice->GetString(i));
      
    resource->SetStringValues(slist);
  }
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Radiobox item
 */

wxProperty *wxRadioBoxPropertyInfo::GetProperty(wxString& name)
{
  wxRadioBox *radioBox = (wxRadioBox *)propertyWindow;
  if (name == "numberRowsOrCols")
  {
    return new wxProperty("numberRowsOrCols", (long)radioBox->GetNumberOfRowsOrCols(), "integer");
  }
  if (name == "orientation")
  {
    char *pos = NULL;
    if (propertyWindow->GetWindowStyleFlag() & wxHORIZONTAL)
      pos = "wxHORIZONTAL";
    else
      pos = "wxVERTICAL";

    return new wxProperty("orientation", pos, "string",
       new wxStringListValidator(new wxStringList("wxHORIZONTAL", "wxVERTICAL",
          NULL)));
  }
  else if (name == "values")
  {
    wxStringList *stringList = new wxStringList;
    int i;
    for (i = 0; i < radioBox->Number(); i++)
      stringList->Add(radioBox->GetString(i));

    return new wxProperty(name, stringList, "stringlist");
  }
  return wxItemPropertyInfo::GetProperty(name);
}

Bool wxRadioBoxPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxRadioBox *radioBox = (wxRadioBox *)propertyWindow;
  if (name == "numberRowsOrCols")
  {
    radioBox->SetNumberOfRowsOrCols((int)property->GetValue().IntegerValue());
    wxResourceManager::currentResourceManager->RecreateWindowFromResource(radioBox, this);
    return TRUE;
  }
  else if (name == "orientation")
  {
    long windowStyle = radioBox->GetWindowStyleFlag();
    wxString val(property->GetValue().StringValue());
    if (val == "wxHORIZONTAL")
    {
      if (windowStyle & wxVERTICAL)
        windowStyle -= wxVERTICAL;
      windowStyle |= wxHORIZONTAL;
    }
    else
    {
      if (windowStyle & wxHORIZONTAL)
        windowStyle -= wxHORIZONTAL;
      windowStyle |= wxVERTICAL;
    }
    radioBox->SetWindowStyleFlag(windowStyle);
    
    wxResourceManager::currentResourceManager->RecreateWindowFromResource(radioBox, this);
    return TRUE;
  }
  else if (name == "values")
  {
    // Set property into *resource*, not wxRadioBox, and then recreate
    // the wxRadioBox. This is because we can't dynamically set the strings
    // of a wxRadioBox.
    wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(propertyWindow);
    if (!resource)
      return FALSE;
      
    wxStringList *stringList = resource->GetStringValues();
    if (!stringList)
    {
      stringList = new wxStringList;
      resource->SetStringValues(stringList);
    }
    stringList->Clear();
      
    wxPropertyValue *expr = property->GetValue().GetFirst();
    while (expr)
    {
      char *s = expr->StringValue();
      if (s)
        stringList->Add(s);
      expr = expr->GetNext();
    }
    wxResourceManager::currentResourceManager->RecreateWindowFromResource(radioBox, this);
    return TRUE;
  }
  return wxItemPropertyInfo::SetProperty(name, property);
}

void wxRadioBoxPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  names.Add("values");
  names.Add("orientation");
  names.Add("numberRowsOrCols");
  wxItemPropertyInfo::GetPropertyNames(names);
}

Bool wxRadioBoxPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxRadioBox *rbox = (wxRadioBox *)propertyWindow;
  // Take strings from resource instead
/*
  int i;
  if (rbox->Number() == 0)
    resource->SetStringValues(NULL);
  else
  {
    wxStringList *slist = new wxStringList;
    
    for (i = 0; i < rbox->Number(); i++)
      slist->Add(rbox->GetString(i));
      
    resource->SetStringValues(slist);
  }
*/
  resource->SetValue1(rbox->GetNumberOfRowsOrCols());
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Groupbox item
 */

wxProperty *wxGroupBoxPropertyInfo::GetProperty(wxString& name)
{
  wxGroupBox *groupBox = (wxGroupBox *)propertyWindow;
  return wxItemPropertyInfo::GetProperty(name);
}

Bool wxGroupBoxPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  return wxItemPropertyInfo::SetProperty(name, property);
}

void wxGroupBoxPropertyInfo::GetPropertyNames(wxStringList& names)
{  
  names.Add("label");
  wxItemPropertyInfo::GetPropertyNames(names);
}

Bool wxGroupBoxPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxGroupBox *gbox = (wxGroupBox *)propertyWindow;
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Checkbox item
 */

wxProperty *wxCheckBoxPropertyInfo::GetProperty(wxString& name)
{
  wxCheckBox *checkBox = (wxCheckBox *)propertyWindow;
  if (name == "value")
    return new wxProperty("value", checkBox->GetValue(), "bool");
  else
    return wxItemPropertyInfo::GetProperty(name);
}

Bool wxCheckBoxPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxCheckBox *checkBox = (wxCheckBox *)propertyWindow;
  if (name == "value")
  {
    checkBox->SetValue((Bool)property->GetValue().BoolValue());
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxCheckBoxPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  names.Add("value");
  wxItemPropertyInfo::GetPropertyNames(names);
}

Bool wxCheckBoxPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxCheckBox *cbox = (wxCheckBox *)propertyWindow;
  resource->SetValue1(cbox->GetValue());
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Slider item
 */

wxProperty *wxSliderPropertyInfo::GetProperty(wxString& name)
{
  wxSlider *slider = (wxSlider *)propertyWindow;
  if (name == "value")
    return new wxProperty("value", (long)slider->GetValue(), "integer");
  else if (name == "orientation")
  {
    char *pos = NULL;
    if (propertyWindow->GetWindowStyleFlag() & wxHORIZONTAL)
      pos = "wxHORIZONTAL";
    else
      pos = "wxVERTICAL";

    return new wxProperty("orientation", pos, "string",
       new wxStringListValidator(new wxStringList("wxHORIZONTAL", "wxVERTICAL",
          NULL)));
  }
  else if (name == "min_value")
    return new wxProperty("min_value", (long)slider->GetMin(), "integer");
  else if (name == "max_value")
    return new wxProperty("max_value", (long)slider->GetMax(), "integer");
  else
    return wxItemPropertyInfo::GetProperty(name);
}

Bool wxSliderPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxSlider *slider = (wxSlider *)propertyWindow;
  if (name == "value")
  {
    slider->SetValue((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else if (name == "orientation")
  {
    long windowStyle = slider->GetWindowStyleFlag();
    long oldWindowStyle = windowStyle;
    wxString val(property->GetValue().StringValue());
    if (val == "wxHORIZONTAL")
    {
      if (windowStyle & wxVERTICAL)
        windowStyle -= wxVERTICAL;
      windowStyle |= wxHORIZONTAL;
    }
    else
    {
      if (windowStyle & wxHORIZONTAL)
        windowStyle -= wxHORIZONTAL;
      windowStyle |= wxVERTICAL;
    }
    
    if (oldWindowStyle == windowStyle)
      return TRUE;
      
    slider->SetWindowStyleFlag(windowStyle);
    
    // If the window style has changed, we swap the width and height parameters.
    int w, h;
    slider->GetSize(&w, &h);
    
    slider = (wxSlider *)wxResourceManager::currentResourceManager->RecreateWindowFromResource(slider, this);
    slider->SetSize(-1, -1, h, w);
    
    return TRUE;
  }
  else if (name == "min_value")
  {
    slider->SetRange((int)property->GetValue().IntegerValue(), slider->GetMax());
    return TRUE;
  }
  else if (name == "max_value")
  {
    slider->SetRange(slider->GetMin(), (int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxSliderPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  names.Add("labelOrientation");
  names.Add("value");
  names.Add("orientation");
  names.Add("min_value");
  names.Add("max_value");
  wxItemPropertyInfo::GetPropertyNames(names);
}

Bool wxSliderPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxSlider *slider = (wxSlider *)propertyWindow;
  resource->SetValue1(slider->GetValue());
  resource->SetValue2(slider->GetMin());
  resource->SetValue3(slider->GetMax());
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Gauge item
 */

wxProperty *wxGaugePropertyInfo::GetProperty(wxString& name)
{
  wxGauge *gauge = (wxGauge *)propertyWindow;
  if (name == "value")
    return new wxProperty("value", (long)gauge->GetValue(), "integer");
  else if (name == "max_value")
    return new wxProperty("max_value", (long)gauge->GetRange(), "integer");
  else
    return wxItemPropertyInfo::GetProperty(name);
}

Bool wxGaugePropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxGauge *gauge = (wxGauge *)propertyWindow;
  if (name == "value")
  {
    gauge->SetValue((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else if (name == "max_value")
  {
    gauge->SetRange((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxGaugePropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  names.Add("value");
  names.Add("max_value");
  names.Add("labelOrientation");
  wxItemPropertyInfo::GetPropertyNames(names);
}

Bool wxGaugePropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxGauge *gauge = (wxGauge *)propertyWindow;
  resource->SetValue1(gauge->GetValue());
  resource->SetValue2(gauge->GetRange());
  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Scrollbar item
 */

wxProperty *wxScrollBarPropertyInfo::GetProperty(wxString& name)
{
  wxScrollBar *scrollBar = (wxScrollBar *)propertyWindow;
  if (name == "value")
    return new wxProperty("value", (long)scrollBar->GetValue(), "integer");
  else if (name == "orientation")
  {
    char *pos = NULL;
    if (propertyWindow->GetWindowStyleFlag() & wxHORIZONTAL)
      pos = "wxHORIZONTAL";
    else
      pos = "wxVERTICAL";

    return new wxProperty("orientation", pos, "string",
       new wxStringListValidator(new wxStringList("wxHORIZONTAL", "wxVERTICAL",
          NULL)));
  }
  else if (name == "pageLength")
  {
    int viewStart, pageLength, objectLength, viewLength;
    scrollBar->GetValues(&viewStart, &viewLength, &objectLength, &pageLength);
    
    return new wxProperty("pageLength", (long)pageLength, "integer");
  }
  else if (name == "viewLength")
  {
    int viewStart, pageLength, objectLength, viewLength;
    scrollBar->GetValues(&viewStart, &viewLength, &objectLength, &pageLength);
    
    return new wxProperty("viewLength", (long)viewLength, "integer");
  }
  else if (name == "objectLength")
  {
    int viewStart, pageLength, objectLength, viewLength;
    scrollBar->GetValues(&viewStart, &viewLength, &objectLength, &pageLength);
    
    return new wxProperty("objectLength", (long)objectLength, "integer");
  }
  else
    return wxItemPropertyInfo::GetProperty(name);
}

Bool wxScrollBarPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxScrollBar *scrollBar = (wxScrollBar *)propertyWindow;
  if (name == "value")
  {
    scrollBar->SetValue((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else if (name == "orientation")
  {
    long windowStyle = scrollBar->GetWindowStyleFlag();
    long oldWindowStyle = windowStyle;
    wxString val(property->GetValue().StringValue());
    if (val == "wxHORIZONTAL")
    {
      if (windowStyle & wxVERTICAL)
        windowStyle -= wxVERTICAL;
      windowStyle |= wxHORIZONTAL;
    }
    else
    {
      if (windowStyle & wxHORIZONTAL)
        windowStyle -= wxHORIZONTAL;
      windowStyle |= wxVERTICAL;
    }
    
    if (oldWindowStyle == windowStyle)
      return TRUE;
      
    scrollBar->SetWindowStyleFlag(windowStyle);
    
    // If the window style has changed, we swap the width and height parameters.
    int w, h;
    scrollBar->GetSize(&w, &h);
    
    scrollBar = (wxScrollBar *)wxResourceManager::currentResourceManager->RecreateWindowFromResource(scrollBar, this);
    scrollBar->SetSize(-1, -1, h, w);
    
    return TRUE;
  }
  else if (name == "pageLength")
  {
    scrollBar->SetPageLength((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else if (name == "viewLength")
  {
    scrollBar->SetViewLength((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else if (name == "objectLength")
  {
    scrollBar->SetObjectLength((int)property->GetValue().IntegerValue());
    return TRUE;
  }
  else
    return wxItemPropertyInfo::SetProperty(name, property);
}

void wxScrollBarPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("label");
  names.Add("orientation");
  names.Add("value");
  names.Add("pageLength");
  names.Add("viewLength");
  names.Add("objectLength");
  wxItemPropertyInfo::GetPropertyNames(names);

  // Remove some properties we don't inherit
  names.Delete("labelOrientation");
  names.Delete("labelFontPoints");
  names.Delete("labelFontFamily");
  names.Delete("labelFontStyle");
  names.Delete("labelFontWeight");
  names.Delete("labelFontUnderlined");
  names.Delete("buttonFontPoints");
  names.Delete("buttonFontFamily");
  names.Delete("buttonFontStyle");
  names.Delete("buttonFontWeight");
  names.Delete("buttonFontUnderlined");
}

Bool wxScrollBarPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxScrollBar *sbar = (wxScrollBar *)propertyWindow;
  
  resource->SetValue1(sbar->GetValue());

  int viewStart, pageLength, objectLength, viewLength;
  sbar->GetValues(&viewStart, &viewLength, &objectLength, &pageLength);
  
  resource->SetValue2(pageLength);
  resource->SetValue3(objectLength);
  resource->SetValue5(viewLength);

  return wxItemPropertyInfo::InstantiateResource(resource);
}

/*
 * Panels
 */

wxProperty *wxPanelPropertyInfo::GetProperty(wxString& name)
{
  wxPanel *panelWindow = (wxPanel *)propertyWindow; 
  wxFont *labelFont = panelWindow->GetLabelFont();
  wxFont *buttonFont = panelWindow->GetButtonFont();
  
  if (name == "labelFontPoints" || name == "labelFontFamily" || name == "labelFontStyle" || name == "labelFontWeight" ||
      name == "labelFontUnderlined")
    return GetFontProperty(name, labelFont);
  else if (name == "buttonFontPoints" || name == "buttonFontFamily" || name == "buttonFontStyle" || name == "buttonFontWeight" ||
      name == "buttonFontUnderlined")
    return GetFontProperty(name, buttonFont);
  else if (name == "userColours")
  {
    Bool userColours;
    if (panelWindow->GetWindowStyleFlag() & wxUSER_COLOURS)
      userColours = TRUE;
    else
      userColours = FALSE;
      
    return new wxProperty(name, (Bool)userColours, "bool");
  }
  else if (name == "backgroundColour")
  {
    wxBrush *brush = panelWindow->GetDC()->GetBackground();
    if (!brush)
      brush = wxLIGHT_GREY_BRUSH;

    wxColour col = brush->GetColour();
    char buf[7];
    wxDecToHex(col.Red(), buf);
    wxDecToHex(col.Green(), buf+2);
    wxDecToHex(col.Blue(), buf+4);

    return new wxProperty(name, buf, "string", new wxColourListValidator);
  }
/*
  else if (name == "itemBackgroundColour")
  {
    wxColour col = *dialogWindow->GetBackgroundColour();
    char buf[7];
    wxDecToHex(col.Red(), buf);
    wxDecToHex(col.Green(), buf+2);
    wxDecToHex(col.Blue(), buf+4);

    return new wxProperty(name, buf, "string", new wxColourListValidator);
  }
*/
  else if (name == "labelColour")
  {
    wxColour col = *panelWindow->GetLabelColour();
    char buf[7];
    wxDecToHex(col.Red(), buf);
    wxDecToHex(col.Green(), buf+2);
    wxDecToHex(col.Blue(), buf+4);

    return new wxProperty(name, buf, "string", new wxColourListValidator);
  }
  else if (name == "buttonColour")
  {
    wxColour col = *panelWindow->GetButtonColour();
    char buf[7];
    wxDecToHex(col.Red(), buf);
    wxDecToHex(col.Green(), buf+2);
    wxDecToHex(col.Blue(), buf+4);

    return new wxProperty(name, buf, "string", new wxColourListValidator);
  }
  else if (name == "labelOrientation")
  {
    char *pos = NULL;
    if (panelWindow->GetLabelPosition() == wxHORIZONTAL)
      pos = "wxHORIZONTAL";
    else
      pos = "wxVERTICAL";

    return new wxProperty("labelOrientation", pos, "string",
       new wxStringListValidator(new wxStringList("wxHORIZONTAL", "wxVERTICAL",
          NULL)));
  }
  else
    return wxWindowPropertyInfo::GetProperty(name);
}

Bool wxPanelPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxPanel *panelWindow = (wxPanel *)propertyWindow; 
  wxFont *labelFont = panelWindow->GetLabelFont();
  wxFont *buttonFont = panelWindow->GetButtonFont();
  
  if (labelFont && (name == "labelFontPoints" || name == "labelFontFamily" || name == "labelFontStyle" || name == "labelFontWeight" || name == "labelFontUnderlined" ))
  {
    wxFont *newFont = SetFontProperty(name, property, labelFont);
    if (newFont)
      panelWindow->SetLabelFont(newFont);
    return TRUE;
  }
  else if (buttonFont && (name == "buttonFontPoints" || name == "buttonFontFamily" || name == "buttonFontStyle" || name == "buttonFontWeight" || name == "buttonFontUnderlined" ))
  {
    wxFont *newFont = SetFontProperty(name, property, buttonFont);
    if (newFont)
      panelWindow->SetButtonFont(newFont);
    return TRUE;
  }
  else if (name == "userColours")
  {
    Bool userColours = property->GetValue().BoolValue();
    long flag = panelWindow->GetWindowStyleFlag();
    
    if (userColours)
    {
      if ((panelWindow->GetWindowStyleFlag() & wxUSER_COLOURS) != wxUSER_COLOURS)
        panelWindow->SetWindowStyleFlag(panelWindow->GetWindowStyleFlag() | wxUSER_COLOURS);
    }
    else
    {
      if ((panelWindow->GetWindowStyleFlag() & wxUSER_COLOURS) == wxUSER_COLOURS)
        panelWindow->SetWindowStyleFlag(panelWindow->GetWindowStyleFlag() - wxUSER_COLOURS);
    }
    wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(panelWindow);
    resource->SetStyle(panelWindow->GetWindowStyleFlag());

    panelWindow = (wxPanel *)wxResourceManager::currentResourceManager->RecreateWindowFromResource(panelWindow, this);
    panelWindow->SetUserEditMode(wxResourceManager::currentResourceManager->GetEditMode());
#ifdef wx_msw
    // This window should not be enabled by being recreated: we're in a modal dialog (property editor)
    ::EnableWindow(panelWindow->GetHWND(), FALSE);
#endif
    return TRUE;
  }
  else if (name == "backgroundColour")
  {
    char *hex = property->GetValue().StringValue();
    int r = wxHexToDec(hex);
    int g = wxHexToDec(hex+2);
    int b = wxHexToDec(hex+4);
    
    wxColour col(r,g,b);
    wxBrush *brush = wxTheBrushList->FindOrCreateBrush(&col, wxSOLID);
    if (brush)
    {
      panelWindow->GetDC()->SetBackground(brush);
      panelWindow->SetBackgroundColour(col);
      panelWindow = (wxPanel *)wxResourceManager::currentResourceManager->RecreateWindowFromResource(panelWindow, this);
      panelWindow->SetUserEditMode(wxResourceManager::currentResourceManager->GetEditMode());
#ifdef wx_msw
    // This window should not be enabled by being recreated: we're in a modal dialog (property editor)
    ::EnableWindow(panelWindow->GetHWND(), FALSE);
#endif
      return TRUE;
    }
    else return FALSE;
  }
  else if (name == "labelColour")
  {
    char *hex = property->GetValue().StringValue();
    int r = wxHexToDec(hex);
    int g = wxHexToDec(hex+2);
    int b = wxHexToDec(hex+4);
    
    wxColour col(r,g,b);
    panelWindow->SetLabelColour(col);
    panelWindow = (wxPanel *)wxResourceManager::currentResourceManager->RecreateWindowFromResource(panelWindow, this);
    panelWindow->SetUserEditMode(wxResourceManager::currentResourceManager->GetEditMode());
#ifdef wx_msw
    // This window should not be enabled by being recreated: we're in a modal dialog (property editor)
    ::EnableWindow(panelWindow->GetHWND(), FALSE);
#endif
    return TRUE;
  }
  else if (name == "buttonColour")
  {
    char *hex = property->GetValue().StringValue();
    int r = wxHexToDec(hex);
    int g = wxHexToDec(hex+2);
    int b = wxHexToDec(hex+4);
    
    wxColour col(r,g,b);
    panelWindow->SetButtonColour(col);
    panelWindow = (wxPanel *)wxResourceManager::currentResourceManager->RecreateWindowFromResource(panelWindow, this);
    panelWindow->SetUserEditMode(wxResourceManager::currentResourceManager->GetEditMode());
#ifdef wx_msw
    // This window should not be enabled by being recreated: we're in a modal dialog (property editor)
    ::EnableWindow(panelWindow->GetHWND(), FALSE);
#endif
    return TRUE;
  }
  else if (name == "labelOrientation")
  {
    int pos = 0;
    if (wxString(property->GetValue().StringValue()) == "wxHORIZONTAL")
      pos = wxHORIZONTAL;
    else
      pos = wxVERTICAL;

    long flag = panelWindow->GetWindowStyleFlag();
    
    if (pos == wxHORIZONTAL)
    {
      if (panelWindow->GetWindowStyleFlag() & wxVERTICAL_LABEL)
        flag -= wxVERTICAL_LABEL;
      if (!(panelWindow->GetWindowStyleFlag() & wxHORIZONTAL_LABEL))
        flag |= wxHORIZONTAL_LABEL;
    }
    else
    {
      if (panelWindow->GetWindowStyleFlag() & wxHORIZONTAL_LABEL)
        flag -= wxHORIZONTAL_LABEL;
      if (!(panelWindow->GetWindowStyleFlag() & wxVERTICAL_LABEL))
        flag |= wxVERTICAL_LABEL;
    }
    wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(panelWindow);
    panelWindow->SetWindowStyleFlag(flag);
    resource->SetStyle(flag);

//      panelWindow->SetLabelPosition( pos );
    panelWindow = (wxPanel *)wxResourceManager::currentResourceManager->RecreateWindowFromResource(panelWindow, this);
    panelWindow->SetUserEditMode(wxResourceManager::currentResourceManager->GetEditMode());
#ifdef wx_msw
    // This window should not be enabled by being recreated: we're in a modal dialog (property editor)
    ::EnableWindow(panelWindow->GetHWND(), FALSE);
#endif
    return TRUE;
  }
  else
    return wxWindowPropertyInfo::SetProperty(name, property);
}

void wxPanelPropertyInfo::GetPropertyNames(wxStringList& names)
{
  wxWindowPropertyInfo::GetPropertyNames(names);
  
//  names.Add("orientation");
  names.Add("labelFontPoints");
  names.Add("labelFontFamily");
  names.Add("labelFontStyle");
  names.Add("labelFontWeight");
  names.Add("labelFontUnderlined");
  names.Add("buttonFontPoints");
  names.Add("buttonFontFamily");
  names.Add("buttonFontStyle");
  names.Add("buttonFontWeight");
  names.Add("buttonFontUnderlined");
  names.Add("userColours");
  names.Add("backgroundColour");
//  names.Add("itemBackgroundColour");
  names.Add("labelColour");
  names.Add("buttonColour");
  names.Add("labelOrientation");
}

Bool wxPanelPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxPanel *panel = (wxPanel *)propertyWindow;
  if (panel->GetLabelFont())
    resource->SetLabelFont(panel->GetLabelFont());
  if (panel->GetButtonFont())
    resource->SetButtonFont(panel->GetButtonFont());
  if (panel->GetBackgroundColour())
    resource->SetBackgroundColour(new wxColour(*panel->GetBackgroundColour()));
  if (panel->GetButtonColour())
    resource->SetButtonColour(new wxColour(*panel->GetButtonColour()));
  if (panel->GetLabelColour())
    resource->SetLabelColour(new wxColour(*panel->GetLabelColour()));

  return wxWindowPropertyInfo::InstantiateResource(resource);
}

/*
 * Dialog boxes
 */

wxProperty *wxDialogPropertyInfo::GetProperty(wxString& name)
{
  wxDialogBox *dialogWindow = (wxDialogBox *)propertyWindow; 
  if (name == "modal")
  {
    wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(dialogWindow);
    if (!resource)
      return NULL;

    Bool modal = resource->GetValue1();
    return new wxProperty(name, (Bool)modal, "bool");
  }
  else  
    return wxPanelPropertyInfo::GetProperty(name);
}

Bool wxDialogPropertyInfo::SetProperty(wxString& name, wxProperty *property)
{
  wxDialogBox *dialogWindow = (wxDialogBox *)propertyWindow;
  
  if (name == "modal")
  {
    wxItemResource *resource = wxResourceManager::currentResourceManager->FindResourceForWindow(dialogWindow);
    if (!resource)
      return FALSE;

    resource->SetValue1(property->GetValue().BoolValue());
    return TRUE;
  }
  else
    return wxPanelPropertyInfo::SetProperty(name, property);
}

void wxDialogPropertyInfo::GetPropertyNames(wxStringList& names)
{
  names.Add("title");
  names.Add("modal");

  wxPanelPropertyInfo::GetPropertyNames(names);
}

Bool wxDialogPropertyInfo::InstantiateResource(wxItemResource *resource)
{
  wxDialogBox *dialog = (wxDialogBox *)propertyWindow;
  if (dialog->GetTitle())
    resource->SetTitle(dialog->GetTitle());
    
  return wxPanelPropertyInfo::InstantiateResource(resource);
}

/*
 * Utilities
 */
 
int wxStringToFontWeight(wxString& val)
{
  if (val == "wxBOLD") return wxBOLD;
  else if (val == "wxLIGHT") return wxLIGHT;
  else return wxNORMAL;
}

int wxStringToFontStyle(wxString& val)
{
  if (val == "wxITALIC") return wxITALIC;
  else if (val == "wxSLANT") return wxSLANT;
  else return wxNORMAL;
}

int wxStringToFontFamily(wxString& val)
{
  if (val == "wxDECORATIVE") return wxDECORATIVE;
  else if (val == "wxROMAN") return wxROMAN;
  else if (val == "wxSCRIPT") return wxSCRIPT;
  else if (val == "wxMODERN") return wxMODERN;
  else if (val == "wxTELETYPE") return wxTELETYPE;
  else return wxSWISS;
}
