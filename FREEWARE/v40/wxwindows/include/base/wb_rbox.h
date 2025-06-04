/*
 * File:	wb_rbox.h
 * Purpose:	Radio box
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_rbox.h	1.2 5/9/94" */

#ifndef wxb_rboxh
#define wxb_rboxh

#ifdef __GNUG__
#pragma interface
#endif

#ifdef IN_CPROTO
typedef       void    *wxbRadioBox ;
#else

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_item.h"

WXDLLEXPORT extern Constdata char *wxRadioBoxNameStr;

// Radio box item
class wxBitmap ;
class WXDLLEXPORT wxbRadioBox: public wxItem
{
 public:
  int no_items;
  int noRowsOrCols;
  int selected;

  wxbRadioBox(void);
  wxbRadioBox(wxPanel *panel, wxFunction func, Const char *title,
             int x = -1, int y = -1, int width = -1, int height = -1,
             int n = 0, char **choices = NULL,
             int majorDim = 0,
             long style = wxHORIZONTAL, Constdata char *name = wxRadioBoxNameStr);

  wxbRadioBox(wxPanel *panel, wxFunction func, Const char *title,
             int x, int y, int width, int height,
             int n, wxBitmap **choices,
             int majorDim = 0,
             long style = wxHORIZONTAL, Constdata char *name = wxRadioBoxNameStr);

  ~wxbRadioBox(void);

  virtual int FindString(char *s) = 0;
  virtual void SetSelection(int N) = 0;
  virtual int GetSelection(void) = 0;
  virtual char *GetStringSelection(void);
  virtual Bool SetStringSelection(char *s);
  virtual int Number(void);
  virtual char *GetString(int N) = 0;
  // Avoids compiler warning
  inline void Enable(Bool enable) { wxWindow::Enable(enable) ; }
  virtual void Enable(int item, Bool enable) = 0; // Enable/disable specific item
  
  // Avoids compiler warning
  inline Bool Show(Bool show) { return wxItem::Show(show) ; }
  virtual void Show(int item, Bool show) = 0; // show/unshow specific item

  void Command(wxCommandEvent& event);
  void ProcessCommand(wxCommandEvent& event);
  
  virtual int GetNumberOfRowsOrCols(void) { return noRowsOrCols; }
  virtual void SetNumberOfRowsOrCols(int n) { noRowsOrCols = n; }
};

#if USE_RADIOBUTTON
extern Constdata char *wxRadioButtonNameStr;

class WXDLLEXPORT wxbRadioButton: public wxItem
{
 public:
  wxbRadioButton(void);
  wxbRadioButton(wxPanel *panel, wxFunction func, Const char *label, Bool value = FALSE,
             int x = -1, int y = -1, int width = -1, int height = -1,
             long style = 0, Constdata char *name = wxRadioButtonNameStr);

  wxbRadioButton(wxPanel *panel, wxFunction func, wxBitmap *label, Bool value = FALSE,
             int x = -1, int y = -1, int width = -1, int height = -1,
             long style = wxHORIZONTAL, Constdata char *name = wxRadioButtonNameStr);

  ~wxbRadioButton(void);

  virtual void SetValue(Bool val) = 0;
  virtual int GetValue(void) = 0;

  void Command(wxCommandEvent& event);
  void ProcessCommand(wxCommandEvent& event);
};
#endif

#endif // IN_CPROTO
#endif // wxb_rboxh
