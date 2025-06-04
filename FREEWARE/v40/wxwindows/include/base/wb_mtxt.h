/*
 * File:	wb_mtxt.h
 * Purpose:	Multitext item
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_mtxt.h	1.2 5/9/94" */

#ifndef wxb_mtxth
#define wxb_mtxth

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_txt.h"

#ifdef IN_CPROTO
typedef       void    *wxbMultiText ;
#else

WXDLLEXPORT extern Constdata char *wxMultiTextNameStr;
WXDLLEXPORT extern Constdata char *wxEmptyString;

// Multi-line text item
class WXDLLEXPORT wxbMultiText: public wxText
{
 public:
  wxbMultiText(void);
  wxbMultiText(wxPanel *panel, wxFunction func, Const char *label, Constdata char *value = wxEmptyString,
         int x = -1, int y = -1, int width = -1, int height = 50,
         long style = 0, Constdata char *name = wxMultiTextNameStr);

  void Command(wxCommandEvent& event);
  void ProcessCommand(wxCommandEvent& event);
};

#endif // IN_CPROTO
#endif // wxb_mtxth
