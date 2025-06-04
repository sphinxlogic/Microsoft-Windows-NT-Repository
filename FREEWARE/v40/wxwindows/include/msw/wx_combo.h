/*
 * File:	wx_combo.h
 * Purpose:	Combobox panel item
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

#ifndef wx_comboh
#define wx_comboh

#include "wb_combo.h"

#if USE_COMBOBOX

#ifdef IN_CPROTO
typedef       void    *wxComboBox ;
#else

WXDLLEXPORT extern Constdata char *wxComboBoxNameStr;
WXDLLEXPORT extern Constdata char *wxEmptyString;

// Combobox item
class WXDLLEXPORT wxComboBox: public wxbComboBox
{
  DECLARE_DYNAMIC_CLASS(wxComboBox)

 public:
  wxComboBox(void);
  wxComboBox(wxPanel *panel, wxFunction func, Const char *title, Constdata char *value = wxEmptyString,
           int x = -1, int y = -1, int width = -1, int height = -1,
           int n = 0, char **choices = NULL,
           long style = 0, Constdata char *name = wxComboBoxNameStr);
  ~wxComboBox(void);

  Bool Create(wxPanel *panel, wxFunction func, Const char *title, Constdata char *value = wxEmptyString,
           int x = -1, int y = -1, int width = -1, int height = -1,
           int n = 0, char **choices = NULL,
           long style = 0, Constdata char *name = wxComboBoxNameStr);

  // List functions
  void Append(char *item);
  void Delete(int n);
  void Clear(void);
  int GetSelection(void);
  void SetSelection(int n);
  int FindString(char *s);
  char *GetString(int n);
  
  // Text field functions
  char *GetValue(void);
  void SetValue(char *value);
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
  virtual void SetEditable(Bool editable);

  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void GetSize(int *x, int *y);
  void GetPosition(int *x, int *y);
  char *GetLabel(void);
  void SetLabel(char *label);

  BOOL MSWCommand(UINT param, WORD id);

  inline void SetColumns(int WXUNUSED(n) = 1 ) { /* No effect */ } ;
  inline int GetColumns(void) { return 1 ; };
};

#endif // IN_CPROTO
#endif // USE_COMBOBOX
#endif // wx_comboh
