/*
 * File:	wx_canvs.h
 * Purpose:	wxCanvas subwindow declarations
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   26 May 97    9:43 am
 */

/* sccsid[] = "@(#)wx_canvs.h	1.2 5/9/94" */

#ifndef wx_canvsh
#define wx_canvsh

#include "wb_canvs.h"

#ifdef IN_CPROTO
typedef       void    *wxCanvas ;
#else

WXDLLEXPORT extern Constdata char *wxCanvasNameStr;

// Canvas subwindow for drawing on
class WXDLLEXPORT wxCanvas: public wxbCanvas
{
  DECLARE_DYNAMIC_CLASS(wxCanvas)

 public:
  Bool clipping;
  int clip_x1;
  int clip_y1;
  int clip_x2;
  int clip_y2;

  wxCanvas(void);
  wxCanvas(wxWindow *parent, int x=-1, int y=-1, int width=-1, int height=-1,
           long style = wxRETAINED, Constdata char *name = wxCanvasNameStr);
  ~wxCanvas(void);

  Bool Create(wxWindow *parent, int x=-1, int y=-1, int width=-1, int height=-1,
           long style = wxRETAINED, Constdata char *name = wxCanvasNameStr);
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);

  // Number of pixels per user unit (0 or -1 for no scrollbar)
  // Length of virtual canvas in user units
  // Length of page in user units
  void SetScrollbars(int horizontal, int vertical,
                             int x_length, int y_length,
                             int x_page, int y_page,
                             int x_pos = 0, int y_pos = 0, Bool setVirtualSize = TRUE);

  // Scroll the canvas
  void Scroll(int x_pos, int y_pos);
  void GetScrollUnitsPerPage(int *x_page, int *y_page);
  void GetScrollPixelsPerUnit(int *x_unit, int *y_unit);

  void OnSize(int w, int h);
  void AdjustScrollbars(void);

  void ViewStart(int *x, int *y);

  // Actual size in pixels when scrolling is taken into account
  void GetVirtualSize(int *x, int *y);

  void SetColourMap(wxColourMap *cmap);

  // Enable/disable Windows 3.1 scrolling in either direction.
  // If TRUE, wxWindows scrolls the canvas and only a bit of
  // the canvas is invalidated; no Clear() is necessary.
  // If FALSE, the whole canvas is invalidated and a Clear() is
  // necessary. Disable for when the scroll increment is used
  // to actually scroll a non-constant distance
  void EnableScrolling(Bool x_scrolling, Bool y_scrolling);

  virtual void WarpPointer(int x_pos, int y_pos) ;
};

#endif // IN_CPROTO
#endif // wx_canvsh
