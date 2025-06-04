/*
 * File:	wb_choic.h
 * Purpose:	Choice items
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_choic.h	1.2 5/9/94" */

#ifndef wxb_choich
#define wxb_choich

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_item.h"

#ifdef IN_CPROTO
typedef       void    *wxbChoice ;
#else

WXDLLEXPORT extern Constdata char *wxChoiceNameStr;

// Choice item
class WXDLLEXPORT wxbChoice: public wxItem
{
 public:
  int no_strings;

  wxbChoice(void);
  wxbChoice(wxPanel *panel, wxFunction func, Const char *title,
           int x = -1, int y = -1, int width = -1, int height = -1,
           int N = 0, char **choices = NULL,
           long style = 0, Constdata char *name = wxChoiceNameStr);
  ~wxbChoice(void);

  virtual void Append(char *Item) = 0;
  virtual void Clear(void) = 0;
  virtual int GetSelection(void) = 0;
  virtual void SetSelection(int n) = 0;
  virtual int FindString(char *s) = 0;
  virtual char *GetStringSelection(void);
  virtual Bool SetStringSelection(char *s);
  virtual char *GetString(int n) = 0;
  inline int Number(void) { return no_strings; }
  void Command(wxCommandEvent& event);
  void ProcessCommand(wxCommandEvent& event);
};

#endif // IN_CPROTO
#endif // wxb_choich
