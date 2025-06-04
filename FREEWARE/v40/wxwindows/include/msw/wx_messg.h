/*
 * File:	wx_messg.h
 * Purpose:	Message panel item
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_messg.h	1.2 5/9/94" */

#ifndef wx_messgh
#define wx_messgh

#include "wb_messg.h"

#ifdef IN_CPROTO
typedef       void    *wxMessage ;
#else

WXDLLEXPORT extern Constdata char *wxMessageNameStr;

// Message item
class WXDLLEXPORT wxMessage: public wxbMessage
{
  DECLARE_DYNAMIC_CLASS(wxMessage)
 protected:
  wxBitmap *messageBitmap;
 public:
  wxMessage(void);
  wxMessage(wxPanel *panel, Const char *message, int x = -1, int y = -1,
            long style = 0, Constdata char *name = wxMessageNameStr);
  wxMessage(wxPanel *panel, Const char *message, int x, int y, int width, int height,
            long style, Constdata char *name);
#if USE_BITMAP_MESSAGE
  wxMessage(wxPanel *panel, wxBitmap *image, int x = -1, int y = -1,
            long style = 0, Constdata char *name = wxMessageNameStr);
  wxMessage(wxPanel *panel, wxBitmap *image, int x, int y, int width, int height,
            long style = 0, Constdata char *name = wxMessageNameStr);
#endif
  ~wxMessage(void);

  Bool Create(wxPanel *panel, Const char *message, int x = -1, int y = -1, int width = -1, int height = -1,
            long style = 0, Constdata char *name = wxMessageNameStr);
#if USE_BITMAP_MESSAGE
  Bool Create(wxPanel *panel, wxBitmap *image, int x = -1, int y = -1, int width = -1, int height = -1,
            long style = 0, Constdata char *name = wxMessageNameStr);
  void SetLabel(wxBitmap *bitmap);
#endif
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void SetLabel(char *label);
  void SetLabelFont(wxFont *font);
  wxBitmap *GetBitmap(void) { return messageBitmap; }
};

#endif // IN_CPROTO
#endif // wx_messgh
