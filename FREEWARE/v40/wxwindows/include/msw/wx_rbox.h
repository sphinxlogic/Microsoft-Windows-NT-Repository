/*
 * File:	wx_rbox.h
 * Purpose:	Radio box panel item
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_rbox.h	1.2 5/9/94" */

#ifndef wx_rboxh
#define wx_rboxh

#include "wb_rbox.h"

#ifdef IN_CPROTO
typedef       void    *wxRadioBox ;
#if USE_RADIOBUTTON
typedef       void    *wxRadioButton ;
#endif
#else

WXDLLEXPORT extern Constdata char *wxRadioBoxNameStr;

// List box item
class wxBitmap ;
class WXDLLEXPORT wxRadioBox: public wxbRadioBox
{
  DECLARE_DYNAMIC_CLASS(wxRadioBox)
 private:
 public:
  Bool selected;
  HWND *radioButtons;
  int majorDim ;
  long style ;
  int *radioWidth ;  // for bitmaps
  int *radioHeight ;

  wxRadioBox(void);
  wxRadioBox(wxPanel *panel, wxFunction func, Const char *title,
             int x = -1, int y = -1, int width = -1, int height = -1,
             int n = 0, char **choices = NULL,
             int majorDim = 0, long style = wxHORIZONTAL, Constdata char *name = wxRadioBoxNameStr);

  wxRadioBox(wxPanel *panel, wxFunction func, Const char *title,
             int x, int y, int width, int height,
             int n, wxBitmap **choices,
             int majorDim = 0, long style = wxHORIZONTAL, Constdata char *name = wxRadioBoxNameStr);

  ~wxRadioBox(void);

  Bool Create(wxPanel *panel, wxFunction func, Const char *title,
             int x = -1, int y = -1, int width =-1, int height = -1,
             int n = 0, char **choices = NULL,
             int majorDim = 0, long style = wxHORIZONTAL, Constdata char *name = wxRadioBoxNameStr);

  Bool Create(wxPanel *panel, wxFunction func, Const char *title,
             int x, int y, int width, int height,
             int n, wxBitmap **choices,
             int majorDim = 0, long style = wxHORIZONTAL, Constdata char *name = wxRadioBoxNameStr);
             
  BOOL MSWCommand(UINT param, WORD id);

  int FindString(char *s);
  void SetSelection(int N);
  int GetSelection(void);
  char *GetString(int N);
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void GetSize(int *x, int *y);
  void GetPosition(int *x, int *y);
  char *GetLabel(void);
  void SetLabel(char *label);
  void SetLabel(int item, char *label) ;
  void SetLabel(int item, wxBitmap *bitmap) ;
  char *GetLabel(int item) ;
  Bool Show(Bool show);
  void SetFocus(void);
  void Enable(Bool enable);
  void Enable(int item, Bool enable);
  void Show(int item, Bool show) ;
  void SetLabelFont(wxFont *font);
  void SetButtonFont(wxFont *font);
};

#if USE_RADIOBUTTON
extern Constdata char *wxRadioButtonNameStr;

class wxRadioButton: public wxbRadioButton
{
  DECLARE_DYNAMIC_CLASS(wxRadioButton)
 public:
  wxBitmap *theButtonBitmap;
  wxRadioButton(void);
  wxRadioButton(wxPanel *panel, wxFunction func, Const char *label, Bool value = FALSE,
             int x = -1, int y = -1, int width = -1, int height = -1,
             long style = 0, Constdata char *name = wxRadioButtonNameStr);

  wxRadioButton(wxPanel *panel, wxFunction func, wxBitmap *label, Bool value = FALSE,
             int x = -1, int y = -1, int width = -1, int height = -1,
             long style = wxHORIZONTAL, Constdata char *name = wxRadioButtonNameStr);

  Bool Create(wxPanel *panel, wxFunction func, Const char *label, Bool value = FALSE,
             int x = -1, int y = -1, int width = -1, int height = -1,
             long style = 0, Constdata char *name = wxRadioButtonNameStr);

  Bool Create(wxPanel *panel, wxFunction func, wxBitmap *label, Bool value = FALSE,
             int x = -1, int y = -1, int width = -1, int height = -1,
             long style = 0, Constdata char *name = wxRadioButtonNameStr);

  ~wxRadioButton(void);

  virtual void SetLabel(char *label);
  virtual void SetLabel(wxBitmap *label);
  virtual char *GetLabel(void);
  virtual void SetValue(Bool val);
  virtual int GetValue(void);
};
#endif

#endif // IN_CPROTO
#endif // wx_rboxh
