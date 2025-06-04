/*
 * File:	wx_mtxt.h
 * Purpose:	Multi-line text panel item
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_mtxt.h	1.2 5/9/94" */

#ifndef wx_mtxth
#define wx_mtxth

#include "wb_mtxt.h"

#ifdef IN_CPROTO
typedef       void    *wxMultiText ;
#else

WXDLLEXPORT extern Constdata char *wxMultiTextnameStr;
WXDLLEXPORT extern Constdata char *wxEmptyString;

// Multi-line text item
class WXDLLEXPORT wxMultiText: public wxbMultiText
{
  DECLARE_DYNAMIC_CLASS(wxMultiText)

 public:
  wxMultiText(void);
  wxMultiText(wxPanel *panel, wxFunction func, Const char *label, Constdata char *value = wxEmptyString,
         int x = -1, int y = -1, int width = -1, int height = -1,
         long style = 0, Constdata char *name = wxMultiTextNameStr);

  Bool Create(wxPanel *panel, wxFunction func, Const char *label, Constdata char *value=wxEmptyString,
         int x=-1, int y=-1, int width=-1, int height=-1,
         long style=0, Constdata char *name = wxMultiTextNameStr);
  char *GetValue(void);
  virtual void GetValue(char *buffer, int maxLen);
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);

  virtual long XYToPosition(long x, long y);
  virtual void PositionToXY(long pos, long *x, long *y);
  virtual void ShowPosition(long pos);
  virtual int GetLineLength(long lineNo);
  virtual int GetLineText(long lineNo, char *buf);
  virtual int GetNumberOfLines(void);

  virtual void Copy(void); // Copy selection to clipboard
  virtual void Paste(void); // Paste clipboard into text window
  virtual void Cut(void); // Copy selection to clipboard, then remove selection.
  virtual void WriteText(char *text); // Append to end of multitext
};

#endif // IN_CPROTO
#endif // wx_mtxth
