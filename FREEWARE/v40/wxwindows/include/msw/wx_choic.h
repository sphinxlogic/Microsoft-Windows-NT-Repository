/*
 * File:	wx_choic.h
 * Purpose:	Choice panel item
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_choic.h	1.2 5/9/94" */

#ifndef wx_choich
#define wx_choich

#include "wb_choic.h"

#ifdef IN_CPROTO
typedef       void    *wxChoice ;
#else

WXDLLEXPORT extern Constdata char *wxChoiceNameStr;

// Choice item
class WXDLLEXPORT wxChoice: public wxbChoice
{
  DECLARE_DYNAMIC_CLASS(wxChoice)

 public:
  wxChoice(void);
  wxChoice(wxPanel *panel, wxFunction func, Const char *title,
           int x = -1, int y = -1, int width = -1, int height = -1,
           int n = 0, char **choices = NULL,
           long style = 0, Constdata char *name = wxChoiceNameStr);
  ~wxChoice(void);

  Bool Create(wxPanel *panel, wxFunction func, Const char *title,
           int x = -1, int y = -1, int width = -1, int height = -1,
           int n = 0, char **choices = NULL,
           long style = 0, Constdata char *name = wxChoiceNameStr);
  void Append(char *Item);
  void Clear(void);
  int GetSelection(void);
  void SetSelection(int n);
  int FindString(char *s);
  char *GetString(int n);
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void GetSize(int *x, int *y);
  void GetPosition(int *x, int *y);
  char *GetLabel(void);
  void SetLabel(char *label);

  BOOL MSWCommand(UINT param, WORD id);

  inline void SetColumns(int WXUNUSED(n) = 1 ) { /* No effect */ } ;
  inline int GetColumns(void) { return 1 ; };
};

#endif // IN_CPROTO
#endif // wx_choich
