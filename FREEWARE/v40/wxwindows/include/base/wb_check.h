/*
 * File:	wb_check.h
 * Purpose:	Check boxes
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_check.h	1.2 5/9/94" */

#ifndef wxb_checkh
#define wxb_checkh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_item.h"

#ifdef IN_CPROTO
typedef void  *wxbCheckBox ;
#else

WXDLLEXPORT extern Constdata char *wxCheckBoxNameStr;

// Checkbox item (single checkbox)
class wxBitmap ;
class WXDLLEXPORT wxbCheckBox: public wxItem
{
 public:
  wxbCheckBox(void);
  wxbCheckBox(wxPanel *panel, wxFunction func, Const char *Title,
             int x = -1, int y = -1, int width = -1, int height = -1,
             long style = 0, Constdata char *name = wxCheckBoxNameStr);
  wxbCheckBox(wxPanel *panel, wxFunction func, wxBitmap *bitmap,
             int x = -1, int y = -1, int width = -1, int height = -1,
             long style = 0, Constdata char *name = wxCheckBoxNameStr);
  ~wxbCheckBox(void);

  void Command(wxCommandEvent& event);
  void ProcessCommand(wxCommandEvent& event);

  virtual void SetValue(Bool) = 0;
  virtual Bool GetValue(void) = 0;
  // Avoids compiler warning
  inline void SetLabel(char *label) { wxItem::SetLabel(label) ; }
  virtual void SetLabel(wxBitmap *bitmap) = 0;
};

#endif // IN_CPROTO
#endif // wxb_checkh
