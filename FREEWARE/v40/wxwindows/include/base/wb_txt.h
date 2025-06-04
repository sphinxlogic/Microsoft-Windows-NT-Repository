/*
 * File:	wb_txt.h
 * Purpose:	Text panel item
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_txt.h	1.2 5/9/94" */

#ifndef wxb_txth
#define wxb_txth

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_item.h"

#ifdef IN_CPROTO
typedef       void    *wxbText ;
#else

WXDLLEXPORT extern Constdata char *wxTextNameStr;
WXDLLEXPORT extern Constdata char *wxEmptyString;

// Single-line text item
class WXDLLEXPORT wxbText: public wxItem
{
 public:
  wxbText(void);
  wxbText(wxPanel *panel, wxFunction func, Const char *label, Constdata char *value = wxEmptyString,
         int x = -1, int y = -1, int width = -1, int height = -1,
         long style = 0, Constdata char *name = wxTextNameStr);
  ~wxbText(void);

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
#endif // wxb_txth
