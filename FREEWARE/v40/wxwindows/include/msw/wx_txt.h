/*
 * File:	wx_txt.h
 * Purpose:	Text panel item
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifndef wx_txth
#define wx_txth

#include "wb_txt.h"

#ifdef IN_CPROTO
typedef       void    *wxText ;
#else

WXDLLEXPORT extern Constdata char *wxTextNameStr;
WXDLLEXPORT extern Constdata char *wxEmptyString;

// Single-line text item
class WXDLLEXPORT wxText: public wxbText
{
  DECLARE_DYNAMIC_CLASS(wxText)

 public:
  wxText(void);
  wxText(wxPanel *panel, wxFunction func, Const char *label, Constdata char *value = wxEmptyString,
         int x = -1, int y = -1, int width = -1, int height = -1,
         long style = 0, Constdata char *name = wxTextNameStr);
  ~wxText(void);

  Bool Create(wxPanel *panel, wxFunction func, Const char *label, Constdata char *value = wxEmptyString,
         int x = -1, int y = -1, int width = -1, int height = -1,
         long style = 0, Constdata char *name = wxTextNameStr);
  char *GetValue(void);
  char *GetLabel(void);
  void SetValue(char *value);
  void SetLabel(char *label);
  void GetSize(int *x, int *y);
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void GetPosition(int *x, int *y);
  void SetFocus(void);

  // Clipboard operations
  virtual void Copy(void);
  virtual void Cut(void);
  virtual void Paste(void);

  virtual void SetInsertionPoint(long pos);
  virtual void SetInsertionPointEnd(void);
  virtual long GetInsertionPoint(void);
  virtual long GetLastPosition(void);
  virtual void Replace(long from, long to, char *value);
  virtual void Remove(long from, long to);
  virtual void SetSelection(long from, long to);

  void SetEditable(Bool editable);

  BOOL MSWCommand(UINT param, WORD id);
};

#endif // IN_CPROTO
#endif // wx_txth
