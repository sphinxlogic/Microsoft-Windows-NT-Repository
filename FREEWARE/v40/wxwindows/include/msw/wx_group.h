/*
 * File:	wx_group.h
 * Purpose:	Group box panel item
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_group.h	1.2 5/9/94" */

#ifndef wx_grouph
#define wx_grouph

#include "wb_group.h"

#ifdef IN_CPROTO
typedef       void    *wxGroupBox;
#else

WXDLLEXPORT extern Constdata char *wxGroupBoxNameStr;

// Group box
class WXDLLEXPORT wxGroupBox: public wxbGroupBox
{
  DECLARE_DYNAMIC_CLASS(wxGroupBox)

 public:
  wxGroupBox(void);
  wxGroupBox(wxPanel *panel, Const char *label, int x = -1, int y = -1,
           int width = -1, int height = -1, long style = 0, Constdata char *name = wxGroupBoxNameStr);
  ~wxGroupBox(void);

  Bool Create(wxPanel *panel, Const char *label, int x=-1, int y=-1,
           int width=-1, int height=-1, long style=0, Constdata char *name=wxGroupBoxNameStr);
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void SetLabel(char *);
  char *GetLabel(void);
};

#endif // IN_CPROTO
#endif // wx_buttnh
