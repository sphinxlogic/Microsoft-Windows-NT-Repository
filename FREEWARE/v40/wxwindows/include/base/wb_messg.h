/*
 * File:	wb_messg.h
 * Purpose:	Declares panel items (controls/widgets)
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_messg.h	1.2 5/9/94" */

#ifndef wxb_messgh
#define wxb_messgh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_item.h"

#ifdef IN_CPROTO
typedef       void    *wxbMessage ;
#else

WXDLLEXPORT extern Constdata char *wxMessageNameStr;

// Message item
class WXDLLEXPORT wxbMessage: public wxItem
{
 protected:
  Bool isBitmap;
 public:
  wxbMessage(void);

  wxbMessage(wxPanel *panel, Const char *message, int x=-1, int y=-1, long style=0,
             Constdata char *name = wxMessageNameStr);
  wxbMessage(wxPanel *panel, Const char *message, int x, int y, int width, int height, long style,
             Constdata char *name);
#if USE_BITMAP_MESSAGE
  wxbMessage(wxPanel *panel, wxBitmap *image, int x=-1, int y=-1, long style=0, Constdata char *name=wxMessageNameStr);
  wxbMessage(wxPanel *panel, wxBitmap *image, int x, int y, int width, int height, long style,
             Constdata char *name);
#endif
  ~wxbMessage(void);

#if USE_BITMAP_MESSAGE  
  virtual Bool IsBitmap(void) { return isBitmap; }
  virtual wxBitmap *GetBitmap(void) = 0;
#endif
};

#endif // IN_CPROTO
#endif // wxb_messgh
