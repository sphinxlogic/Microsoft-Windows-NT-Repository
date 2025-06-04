/*
 * File:	bgdi.h
 * Purpose:	wxWindows GUI builder -- GDI stuff
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifndef bgdih
#define bgdih

#include "bwin.h"

class BuildFontData: public wxObject
{
 public:
  char *fontName;
  char *fontFamily;
  char *fontStyle;
  char *fontWeight;
  int fontSize;
  wxFont *font;
  
  BuildFontData(void);
  ~BuildFontData(void);

  wxFont *CreateFont(void);
  Bool Edit(wxWindow *parent = NULL);
};

class FontManagerDialog: public wxDialogBox
{
 public:
  wxListBox *listbox;
  FontManagerDialog(wxFrame *frame, char *title, int x, int y, int w, int h);
};

class FontManager: public wxList
{
 public:
  FontManagerDialog *dialog;
  FontManager(void);
  ~FontManager(void);

  void Show(Bool show);
  void ReadFonts(PrologDatabase *database);
  void WriteFonts(PrologDatabase *database);
};

wxFont *FindFont(char *fontName);

void FontManagerProc(wxListBox& lbox, wxCommandEvent& event);
void FontOkProc(wxButton& but, wxCommandEvent& event);
void FontHelpProc(wxButton& but, wxCommandEvent& event);
void AddFontProc(wxButton& but, wxCommandEvent& event);
void DeleteFontProc(wxButton& but, wxCommandEvent& event);

extern FontManager TheFontManager;

/*
 * Bitmap/Icon information
 */
 
class BuildBitmapData: public wxObject
{
 public:
  Bool bitmapIsIcon;
  char *bitmapName;
  wxBitmap *bitmap;
  wxIcon *icon;
  int bitmapTypeWindows;
  int bitmapTypeX;
  
  BuildBitmapData(Bool isIcon = FALSE);
  ~BuildBitmapData(void);

  wxBitmap *CreateBitmap(void);
  wxIcon *CreateIcon(void);
  Bool Edit(wxWindow *parent = NULL);
  inline wxBitmap *GetBitmap(void) { return bitmap; }
  inline wxIcon *GetIcon(void) { return icon; }
  inline char *GetBitmapName(void) { return bitmapName; }
  inline int GetTypeWindows(void) { return bitmapTypeWindows; }
  inline int GetTypeX(void) { return bitmapTypeX; }
  inline void SetBitmap(wxBitmap *bm) { bitmap = bm; }
  inline void SetIcon(wxIcon *ic) { icon = ic; }
  inline void SetTypeWindows(int typ) { bitmapTypeWindows = typ; }
  inline void SetTypeX(int typ) { bitmapTypeX = typ; }
  void SetBitmapName(char *name);

  // I/O
  // Write a list (named 'bitmap') representing the bitmap information
  PrologExpr *WriteList(void);
  Bool ReadList(PrologExpr *listExpr);
  Bool WritePrologAttributes(PrologExpr *expr);
  Bool ReadPrologAttributes(PrologExpr *expr);
  void GenerateWXResourceBitmap(ostream& stream);
  void GenerateWXResourceBitmapRegistration(ostream& stream);
  void GenerateResourceEntry(ostream &stream);
  void GenerateResourceLoading(ostream &stream);
  void GenerateBitmapDataInclude(ostream& stream);
};

#endif // bgdih

