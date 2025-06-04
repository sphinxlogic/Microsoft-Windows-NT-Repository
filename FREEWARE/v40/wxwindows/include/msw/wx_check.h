/*
 * File:	wx_check.h
 * Purpose:	Check box
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_check.h	1.2 5/9/94" */

#ifndef wx_checkh
#define wx_checkh

#include "wb_check.h"

#ifdef IN_CPROTO
typedef       void    *wxCheckBox ;
#else

WXDLLEXPORT extern Constdata char *wxCheckBoxNameStr;

// Checkbox item (single checkbox)
class wxBitmap;
class WXDLLEXPORT wxCheckBox: public wxbCheckBox
{
  DECLARE_DYNAMIC_CLASS(wxCheckBox)

 public:
  int checkWidth ;
  int checkHeight ;
  wxCheckBox(void);
  wxCheckBox(wxPanel *panel, wxFunction func, Const char *title,
             int x = -1, int y = -1, int width = -1, int height = -1,
             long style = 0, Constdata char *name = wxCheckBoxNameStr);
  wxCheckBox(wxPanel *panel, wxFunction func, wxBitmap *bitmap,
             int x = -1, int y = -1, int width = -1, int height = -1,
             long style = 0, Constdata char *name = wxCheckBoxNameStr);
  ~wxCheckBox(void);

  Bool Create(wxPanel *panel, wxFunction func, Const char *title,
             int x = -1, int y = -1, int width = -1, int height = -1,
             long style = 0, Constdata char *name = wxCheckBoxNameStr);
  Bool Create(wxPanel *panel, wxFunction func, wxBitmap *bitmap,
             int x = -1, int y = -1, int width = -1, int height = -1,
             long style = 0, Constdata char *name = wxCheckBoxNameStr);
  void SetValue(Bool);
  Bool GetValue(void);
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  virtual BOOL MSWCommand(UINT param, WORD id);
  void SetLabel(char *);
  void SetLabel(wxBitmap *bitmap);
  char *GetLabel(void) ;

  void SetLabelFont(wxFont *font);
};

#endif // IN_CPROTO
#endif // wx_checkh
