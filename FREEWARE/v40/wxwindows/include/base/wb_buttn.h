/*
 * File:	wb_buttn.h
 * Purpose:	Buttons
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_buttn.h	1.2 5/9/94" */

#ifndef wxb_buttnh
#define wxb_buttnh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_item.h"

#ifdef IN_CPROTO
typedef       void    *wxbButton ;
#else

WXDLLEXPORT extern Constdata char *wxButtonNameStr;

// Pushbutton
class wxBitmap;
class WXDLLEXPORT wxbButton: public wxItem
{
 protected:
  Bool isBitmap;
 public:
  wxbButton(void);
  wxbButton(wxPanel *panel, wxFunction func, Const char *label, int x = -1, int y = -1,
           int width = -1, int height = -1, long style = 0, Constdata char *name = wxButtonNameStr);
  wxbButton(wxPanel *panel, wxFunction func, wxBitmap *bitmap, int x = -1, int y = -1,
           int width = -1, int height = -1, long style = 0, Constdata char *name = wxButtonNameStr);
  ~wxbButton(void);

  void Command(wxCommandEvent& event);
  void ProcessCommand(wxCommandEvent& event);

  virtual void SetDefault(void) = 0;
  // Avoids compiler warning
  inline void SetLabel(char *label) { wxItem::SetLabel(label) ; }
  virtual void SetLabel(wxBitmap *bitmap) = 0;

  inline Bool IsBitmap(void) { return isBitmap; }
  virtual wxBitmap *GetBitmap(void) = 0;
};

#endif // IN_CPROTO
#endif // wxb_buttnh
