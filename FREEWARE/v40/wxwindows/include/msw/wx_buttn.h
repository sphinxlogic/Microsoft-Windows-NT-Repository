/*
 * File:	wx_buttn.h
 * Purpose:	Button panel item
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_buttn.h	1.2 5/9/94" */

#ifndef wx_buttnh
#define wx_buttnh

#include "wb_buttn.h"

#ifdef IN_CPROTO
typedef       void    *wxButton ;
#else

WXDLLEXPORT extern Constdata char *wxButtonNameStr;

// Pushbutton
class wxBitmap;
class WXDLLEXPORT wxButton: public wxbButton
{
  DECLARE_DYNAMIC_CLASS(wxButton)
 protected:
  wxBitmap *buttonBitmap;
 public:
  wxButton(void);
  wxButton(wxPanel *panel, wxFunction func, Const char *label, int x = -1, int y = -1,
           int width = -1, int height = -1, long style = 0, Constdata char *name = wxButtonNameStr);
  wxButton(wxPanel *panel, wxFunction func, wxBitmap *bitmap, int x = -1, int y = -1,
           int width = -1, int height = -1, long style = 0, Constdata char *name = wxButtonNameStr);
  ~wxButton(void);

  Bool Create(wxPanel *panel, wxFunction func, Const char *label, int x=-1, int y=-1,
           int width=-1, int height=-1, long style=0, Constdata char *name=wxButtonNameStr);
  Bool Create(wxPanel *panel, wxFunction func, wxBitmap *bitmap, int x=-1, int y=-1,
           int width=-1, int height=-1, long style=0, Constdata char *name=wxButtonNameStr);
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void SetFocus(void);
  void SetDefault(void);
  void SetLabel(char *);
  void SetLabel(wxBitmap *bitmap);
  char *GetLabel(void);
  wxBitmap *GetBitmap(void) { return buttonBitmap; }

  BOOL MSWCommand(UINT param, WORD id);
};

#endif // IN_CPROTO
#endif // wx_buttnh
