/*
 * File:	wb_dialg.h
 * Purpose:	wxDialogBox and common dialog declarations
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_dialg.h	1.2 5/9/94" */

#ifndef wxb_dialgh
#define wxb_dialgh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_panel.h"

#ifdef IN_CPROTO
typedef       void    *wxbDialogBox ;
#else

WXDLLEXPORT extern Constdata char *wxDialogNameStr;

// Dialog boxes
class WXDLLEXPORT wxbDialogBox: public wxPanel
{
 protected:
  Bool modal;
 public:

  wxbDialogBox(void);
  wxbDialogBox(wxWindow *parent, Const char *title, Bool modal = FALSE,
              int x = -1, int y = -1,
              int width = -1, int height = -1, long style = wxDEFAULT_DIALOG_STYLE, Constdata char *name = wxDialogNameStr);
  ~wxbDialogBox();

  Bool Create(wxWindow *window, Const char *title, Bool modal = FALSE,
              int x = -1, int y = -1,
              int width = -1, int height = -1, long style = wxDEFAULT_DIALOG_STYLE, Constdata char *name = wxDialogNameStr);

  virtual void Iconize(Bool iconize) = 0;
  virtual Bool Iconized(void) = 0;

  void Centre(int direction = wxBOTH);
  virtual Bool IsModal(void) { return modal; }
  virtual void SetModal(Bool mod) { modal = mod; }
};

#endif // IN_CPROTO
#endif // wxb_dialgh
