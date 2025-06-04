/*
 * File:	wx_dccan.h
 * Purpose:	Canvas device context declaration
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_dccan.h	1.2 5/9/94" */


#ifndef wx_dccanh
#define wx_dccanh

#include "wx_gdi.h"
#include "wb_dccan.h"

#ifdef IN_CPROTO
typedef       void    *wxCanvasDC ;
#else

class WXDLLEXPORT wxCanvasDC: public wxbCanvasDC
{
  DECLARE_DYNAMIC_CLASS(wxCanvasDC)

 public:
  wxCanvasDC(void);

  // Create a DC corresponding to a canvas
  wxCanvasDC(wxCanvas *canvas);
  void GetClippingBox(float *x,float *y,float *w,float *h) ;

  ~wxCanvasDC(void);
};

class WXDLLEXPORT wxScreenDC: public wxCanvasDC
{
  DECLARE_DYNAMIC_CLASS(wxScreenDC)

 public:
  // Create a DC representing the whole screen
  wxScreenDC(void);
  ~wxScreenDC(void);

  // Compatibility with X's requirements for
  // drawing on top of all windows
  static Bool StartDrawingOnTop(wxWindow *window) { return TRUE; }
  static Bool StartDrawingOnTop(wxRectangle *rect = NULL) { return TRUE; }
  static Bool EndDrawingOnTop(void) { return TRUE; }
};

#endif // IN_CPROTO
#endif // wx_dccan

