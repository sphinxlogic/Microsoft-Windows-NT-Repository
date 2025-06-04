/*
 * File:	wb_group.h
 * Purpose:	Group box
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_group.h	1.2 5/9/94" */

#ifndef wb_grouph
#define wb_grouph

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_item.h"

#ifdef IN_CPROTO
typedef       void    *wxbGroupBox;
#else

WXDLLEXPORT extern Constdata char *wxGroupBoxNameStr;

// Group box
class WXDLLEXPORT wxbGroupBox: public wxItem
{
 public:
  wxbGroupBox(void);
  wxbGroupBox(wxPanel *panel, Const char *label, int x = -1, int y = -1,
           int width = -1, int height = -1, long style = 0, Constdata char *name = wxGroupBoxNameStr);
  ~wxbGroupBox(void);
};

#endif // IN_CPROTO
#endif // wb_grouph
