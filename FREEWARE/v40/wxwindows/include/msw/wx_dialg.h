/*
 * File:	wx_dialg.h
 * Purpose:	wxDialogBox and common dialog declarations
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifndef wx_dialgh
#define wx_dialgh

#include "wx_defs.h"
#include "wx_item.h"
#include "wx_check.h"
#include "wx_messg.h"
#include "wb_dialg.h"

#ifdef IN_CPROTO
typedef void    *wxDialogBox ;
#else

WXDLLEXPORT extern Constdata char *wxDialogNameStr;

// Dialog boxes
class WXDLLEXPORT wxDialogBox: public wxbDialogBox
{
  DECLARE_DYNAMIC_CLASS(wxDialogBox)

 public:
  Bool modal_showing;

  wxDialogBox(void);
  wxDialogBox(wxWindow *parent, Const char *title, Bool modal = FALSE,
              int x = -1, int y = -1,
              int width = -1, int height = -1, long style = wxDEFAULT_DIALOG_STYLE,
              Constdata char *name = wxDialogNameStr);
  ~wxDialogBox(void);

  Bool Create(wxWindow *parent, Const char *title, Bool modal=FALSE,
              int x=-1, int y=-1,
              int width=-1, int height=-1, long style=wxDEFAULT_DIALOG_STYLE,
              Constdata char *name = wxDialogNameStr);
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void SetClientSize(int width, int height);
  void GetPosition(int *x, int *y);
  Bool Show(Bool show);
  Bool IsShown(void);
  void Iconize(Bool iconize);
  Bool Iconized(void);
  void Fit(void);

  void SetTitle(char *title);
  char *GetTitle(void);

  Bool OnCharHook(wxKeyEvent& event);
  
  void SetModal(Bool flag);
};

#endif // IN_CPROTO
#endif // wx_dialgh
