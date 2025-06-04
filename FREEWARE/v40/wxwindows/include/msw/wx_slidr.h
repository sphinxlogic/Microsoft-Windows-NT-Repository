/*
 * File:	wx_slidr.h
 * Purpose:	Slider
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_slidr.h	1.2 5/9/94" */

#ifndef wx_slidrh
#define wx_slidrh

#include "wb_slidr.h"

#ifdef IN_CPROTO
typedef       void    *wxSlider ;
#else

WXDLLEXPORT extern Constdata char *wxSliderNameStr;

// Slider
class WXDLLEXPORT wxSlider: public wxbSlider
{
  DECLARE_DYNAMIC_CLASS(wxSlider)

 public:
  HWND static_min;
  HWND static_max;
  HWND edit_value;

  int s_min;
  int s_max;
  int page_size;
  int noTicks;

  wxSlider(void);
  wxSlider(wxPanel *panel, wxFunction func, Const char *label, int value,
           int min_value, int max_value, int width, int x = -1, int y = -1,
           long style = wxHORIZONTAL, Constdata char *name = wxSliderNameStr);
  ~wxSlider(void);

  Bool Create(wxPanel *panel, wxFunction func, Const char *label, int value,
           int min_value, int max_value, int width, int x = -1, int y = -1,
           long style = wxHORIZONTAL, Constdata char *name = wxSliderNameStr);
  virtual int GetValue(void);
  virtual char *GetLabel(void);
  virtual void SetValue(int);
  virtual void SetLabel(char *label);
  void GetSize(int *x, int *y);
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void GetPosition(int *x, int *y);

  void SetRange(int minValue, int maxValue);

  // Windows only for now
  void SetTicks(int n);
  int GetTicks(void);

  int GetMin(void);
  int GetMax(void);
};

#endif // IN_CPROTO
#endif // wx_slidrh
