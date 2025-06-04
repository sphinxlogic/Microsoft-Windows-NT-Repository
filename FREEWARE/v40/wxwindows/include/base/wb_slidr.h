/*
 * File:	wb_slidr.h
 * Purpose:	Slider
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_slidr.h	1.2 5/9/94" */

#ifndef wxb_slidrh
#define wxb_slidrh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_item.h"

#ifdef IN_CPROTO
typedef       void    *wxbSlider ;
#else

WXDLLEXPORT extern Constdata char *wxSliderNameStr;

// Slider
class WXDLLEXPORT wxbSlider: public wxItem
{
 public:
  wxbSlider(void);
  wxbSlider(wxPanel *panel, wxFunction func, Const char *label, int value,
           int min_value, int max_value, int width, int x = -1, int y = -1,
           long style = wxHORIZONTAL, Constdata char *name = wxSliderNameStr);
  ~wxbSlider(void);

  virtual int GetValue(void) = 0;
  virtual void SetValue(int) = 0;
  virtual void SetRange(int minValue, int maxValue) = 0;

  virtual int GetMin(void) = 0;
  virtual int GetMax(void) = 0;
  
  virtual void SetTicks(int) {}
  virtual int GetTicks(void) { return 0; }

  void Command(wxCommandEvent& event);
  void ProcessCommand(wxCommandEvent& event);
};

#endif // IN_CPROTO
#endif // wxb_slidrh
