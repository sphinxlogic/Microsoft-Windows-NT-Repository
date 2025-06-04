/*
 * File:	wb_res.h
 * Purpose:	Resource processor
 * Author:	Julian Smart
 * Created:	1994
 * Updated:	
 * Copyright:	(c) 1994, Julian Smart
 */

/* sccsid[] = "%W% %G%" */

#ifndef wxb_resh
#define wxb_resh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_setup.h"

class wxPanel;

#if USE_WX_RESOURCES
#include <stdio.h>

// A few further types not in wx_types.h
#define wxRESOURCE_TYPE_SEPARATOR   1000
#define wxRESOURCE_TYPE_XBM_DATA    1001
#define wxRESOURCE_TYPE_XPM_DATA    1002

#define RESOURCE_PLATFORM_WINDOWS   1
#define RESOURCE_PLATFORM_X         2
#define RESOURCE_PLATFORM_MAC       3
#define RESOURCE_PLATFORM_ANY       4

/*
 * Internal format for control/panel item
 */
 
class WXDLLEXPORT wxItemResource: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxItemResource)

 protected:
  wxList children;
  char *itemType;
  int x, y, width, height;
  char *title;
  char *name;
  long windowStyle;
  long value1, value2, value3, value5;
  char *value4;
  wxStringList *stringValues; // Optional string values
  wxBitmap *bitmap;
  wxColour *backgroundColour;
  wxColour *labelColour;
  wxColour *buttonColour;
  wxFont *buttonFont;
  wxFont *labelFont;
 public:
 
  wxItemResource(void);
  ~wxItemResource(void);

  void SetType(char *typ);
  inline void SetStyle(long styl) { windowStyle = styl; }
  inline void SetBitmap(wxBitmap *bm) { bitmap = bm; }
  inline wxBitmap *GetBitmap(void) { return bitmap; }
  inline void SetButtonFont(wxFont *font) { buttonFont = font; }
  inline void SetLabelFont(wxFont *font) { labelFont = font; }
  inline wxFont *GetButtonFont(void) { return buttonFont; }
  inline wxFont *GetLabelFont(void) { return labelFont; }
  inline void SetSize(int xx, int yy, int ww, int hh)
  {  x = xx; y = yy; width = ww; height = hh; }
  void SetTitle(char *t);
  void SetName(char *n);
  inline void SetValue1(long v) { value1 = v; }
  inline void SetValue2(long v) { value2 = v; }
  inline void SetValue3(long v) { value3 = v; }
  inline void SetValue5(long v) { value5 = v; }
  void SetValue4(char *v);
  void SetStringValues(wxStringList *svalues);

  inline char *GetType(void) { return itemType; }
  inline int GetX(void) { return x; }
  inline int GetY(void) { return y; }
  inline int GetWidth(void) { return width; }
  inline int GetHeight(void) { return height; }

  inline char *GetTitle(void) { return title; }
  inline char *GetName(void) { return name; }
  inline long GetStyle(void) { return windowStyle; }

  inline long GetValue1(void) { return value1; }
  inline long GetValue2(void) { return value2; }
  inline long GetValue3(void) { return value3; }
  inline long GetValue5(void) { return value5; }
  inline char *GetValue4(void) { return value4; }
  inline wxList& GetChildren(void) { return children; }
  inline wxStringList *GetStringValues(void) { return stringValues; }
  
  inline void SetBackgroundColour(wxColour *col) { if (backgroundColour) delete backgroundColour; backgroundColour = col; }
  inline void SetLabelColour(wxColour *col) { if (labelColour) delete labelColour; labelColour = col; }
  inline void SetButtonColour(wxColour *col) { if (buttonColour) delete buttonColour; buttonColour = col; }
  
  inline wxColour *GetBackgroundColour(void) { return backgroundColour; }
  inline wxColour *GetLabelColour(void) { return labelColour; }
  inline wxColour *GetButtonColour(void) { return buttonColour; }
};

/*
 * Resource table (normally only one of these)
 */
 
class WXDLLEXPORT wxResourceTable: public wxHashTable
{
  DECLARE_DYNAMIC_CLASS(wxResourceTable)

  protected:
    
  public:
    wxHashTable identifiers;
    
    wxResourceTable(void);
    ~wxResourceTable(void);
    
    virtual wxItemResource *FindResource(char *name);
    virtual void AddResource(wxItemResource *item);
    virtual Bool DeleteResource(char *name);

    virtual Bool ParseResourceFile(char *filename);
    virtual Bool ParseResourceData(char *data);
    virtual Bool SaveResource(char *filename);

    // Register XBM/XPM data
    virtual Bool RegisterResourceBitmapData(char *name, char bits[], int width, int height);
    virtual Bool RegisterResourceBitmapData(char *name, char **data);

    virtual wxItem *CreateItem(wxPanel *panel, wxItemResource *childResource);

    virtual void ClearTable(void);
};

extern wxResourceTable wxDefaultResourceTable;
WXDLLEXPORT extern long wxParseWindowStyle(char *style);

class wxMenuBar;
class wxMenu;
class wxBitmap;
class wxIcon;
WXDLLEXPORT extern wxBitmap *wxResourceCreateBitmap(char *resource, wxResourceTable *table = NULL);
WXDLLEXPORT extern wxIcon *wxResourceCreateIcon(char *resource, wxResourceTable *table = NULL);
WXDLLEXPORT extern wxMenuBar *wxResourceCreateMenuBar(char *resource, wxResourceTable *table = NULL, wxMenuBar *menuBar = NULL);
WXDLLEXPORT extern wxMenu *wxResourceCreateMenu(char *resource, wxResourceTable *table = NULL);
WXDLLEXPORT extern Bool wxResourceParseData(char *resource, wxResourceTable *table = NULL);
WXDLLEXPORT extern Bool wxResourceParseFile(char *filename, wxResourceTable *table = NULL);
WXDLLEXPORT extern Bool wxResourceParseString(char *s, wxResourceTable *table = NULL);
WXDLLEXPORT extern void wxResourceClear(wxResourceTable *table = NULL);
// Register XBM/XPM data
WXDLLEXPORT extern Bool wxResourceRegisterBitmapData(char *name, char bits[], int width, int height, wxResourceTable *table = NULL);
WXDLLEXPORT extern Bool wxResourceRegisterBitmapData(char *name, char **data, wxResourceTable *table = NULL);
#define wxResourceRegisterIconData wxResourceRegisterBitmapData

/*
 * Resource identifer code: #define storage
 */

WXDLLEXPORT extern Bool wxResourceAddIdentifier(char *name, int value, wxResourceTable *table = NULL);
WXDLLEXPORT extern int wxResourceGetIdentifier(char *name, wxResourceTable *table = NULL);

#endif
#endif
