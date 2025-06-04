/*
 * File:	wb_gauge.h
 * Purpose:	Gauge box (experimental)
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_group.h	1.2 5/9/94" */

#ifndef wb_gaugeh
#define wb_gaugeh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_item.h"

#ifdef IN_CPROTO
typedef       void    *wxbGauge;
#else

WXDLLEXPORT extern Constdata char *wxGaugeNameStr;

// Group box
class WXDLLEXPORT wxbGauge: public wxItem
{
 public:
  wxbGauge(void);
  wxbGauge(wxPanel *panel, Const char *label, int range, int x = -1, int y = -1,
           int width = -1, int height = -1, long style = 0, Constdata char *name = wxGaugeNameStr);
  ~wxbGauge(void);

  virtual void SetShadowWidth(int w) = 0;
  virtual void SetBezelFace(int w) = 0;
  virtual void SetRange(int r) = 0;
  virtual void SetValue(int pos) = 0;

  virtual int GetShadowWidth(void) = 0;
  virtual int GetBezelFace(void) = 0;
  virtual int GetRange(void) = 0;
  virtual int GetValue(void) = 0;
};

#endif // IN_CPROTO
#endif // wb_gaugeh
