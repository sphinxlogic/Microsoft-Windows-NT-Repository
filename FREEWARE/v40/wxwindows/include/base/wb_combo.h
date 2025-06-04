/*
 * File:	wb_combo.h
 * Purpose:	Combobox items
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_combo.h	1.2 5/9/94" */

#ifndef wxb_comboh
#define wxb_comboh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"

#if USE_COMBOBOX

#include "wx_panel.h"
#include "wx_item.h"

#ifdef IN_CPROTO
typedef       void    *wxbComboBox ;
#else

WXDLLEXPORT extern Constdata char *wxComboBoxNameStr;
WXDLLEXPORT extern Constdata char *wxEmptyString;

// Choice item
class WXDLLEXPORT wxbComboBox: public wxItem
{
 public:
  int no_strings;

  wxbComboBox(void);
  wxbComboBox(wxPanel *panel, wxFunction func, Const char *title, Constdata char *value = wxEmptyString,
           int x = -1, int y = -1, int width = -1, int height = -1,
           int N = 0, char **choices = NULL,
           long style = 0, Constdata char *name = wxComboBoxNameStr);
  ~wxbComboBox(void);

  // List functions
  virtual void Append(char *item) = 0;
  virtual void Delete(int n) = 0;
  virtual void Clear(void) = 0;
  virtual int GetSelection(void) = 0;
  virtual void SetSelection(int n) = 0;
  virtual int FindString(char *s) = 0;
  virtual char *GetStringSelection(void);
  virtual Bool SetStringSelection(char *s);
  virtual char *GetString(int n) = 0;
  inline int Number(void) { return no_strings; }

  // Text field functions
  virtual char *GetValue(void) = 0;
  virtual void SetValue(char *value) = 0;

  // Clipboard operations
  virtual void Copy(void) = 0;
  virtual void Cut(void) = 0;
  virtual void Paste(void) = 0;

  void Command(wxCommandEvent& event);
  void ProcessCommand(wxCommandEvent& event);
};

#endif // IN_CPROTO

#endif // USE_COMBOBOX

#endif // wxb_comboh
