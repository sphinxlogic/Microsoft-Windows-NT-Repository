/*
 * File:	wb_canvs.h
 * Purpose:	wxCanvas subwindow declarations
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_canvs.h	1.2 5/9/94" */

#ifndef wxb_canvsh
#define wxb_canvsh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_frame.h"
#include "wx_gdi.h"
#include "wx_dccan.h"
#include "wx_stdev.h"

#ifndef IN_CPROTO
#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif
#endif

// Convenience macros for accessing scroll event parameters
#define WXSCROLLPOS(event) event.commandInt
#define WXSCROLLORIENT(event) event.extraLong

#ifdef IN_CPROTO
typedef void *wxbCanvas ;
#else

WXDLLEXPORT extern Constdata char *wxCanvasNameStr;

// Canvas subwindow for drawing on
class WXDLLEXPORT wxbCanvas: public wxWindow
{
 public:
  wxCanvasDC *wx_dc;    // The canvas's device context

  Bool is_retained;
  int horiz_units;
  int vert_units;

  wxbCanvas(void);
  wxbCanvas(wxWindow *frame, int x=-1, int y=-1, int width=-1, int height=-1,
           long style = wxRETAINED, Constdata char *name = wxCanvasNameStr);
  virtual ~wxbCanvas(void);

  virtual void AllowDoubleClick(int value) ;

  void OnChar(wxKeyEvent& event);

  // Number of pixels per user unit (0 or -1 for no scrollbar)
  // Length of virtual canvas in user units
  // Length of page in user units
  virtual void SetScrollbars(int horizontal, int vertical,
                             int x_length, int y_length,
                             int x_page, int y_page,
                             int x_pos = 0, int y_pos = 0, Bool setVirtualSize = TRUE) = 0;

  // Scroll the canvas
  virtual void Scroll(int x_pos, int y_pos) = 0;
  virtual void ViewStart(int *x, int *y) = 0;
  virtual void GetScrollUnitsPerPage(int *x_page, int *y_page) = 0;
  virtual void GetScrollPixelsPerUnit(int *x_unit, int *y_unit) = 0;

  virtual void BeginDrawing(void) {} ;
  virtual void EndDrawing(void) {} ;

  virtual void FloodFill(float x1, float y1, wxColour *col, int style=wxFLOOD_SURFACE) ;
  virtual Bool GetPixel(float x1, float y1, wxColour *col) ;

  virtual void DrawLine(float x1, float y1, float x2, float y2);
  virtual void IntDrawLine(int x1, int y1, int x2, int y2);
  virtual void CrossHair(int x, int y) ;
  virtual void DrawArc(float x1,float y1,float x2,float y2,float xc,float yc);
  virtual void DrawPoint(float x, float y);
  virtual void DrawLines(int n, wxPoint points[], float xoffset = 0, float yoffset = 0);
  virtual void DrawLines(int n, wxIntPoint points[], int xoffset = 0, int yoffset = 0);
  virtual void DrawLines(wxList *list, float xoffset = 0, float yoffset = 0);
  virtual void DrawPolygon(int n, wxPoint points[], float xoffset = 0, float yoffset = 0, int fillStyle=wxODDEVEN_RULE);
  virtual void DrawPolygon(wxList *list, float xoffset = 0, float yoffset = 0, int fillStyle=wxODDEVEN_RULE);
  virtual void DrawRectangle(float x, float y, float width, float height);
  virtual void DrawRoundedRectangle(float x, float y, float width, float height, float radius = 20);
  virtual void DrawEllipse(float x, float y, float width, float height);

#if USE_SPLINES
  // Splines
  // 3-point spline
  virtual void DrawSpline(float x1, float y1, float x2, float y2, float x3, float y3);
  // Any number of control points - a list of pointers to wxPoints
  virtual void DrawSpline(wxList *points);
#endif

  virtual void Clear(void);
  virtual void SetFont(wxFont *font);
  virtual void SetPen(wxPen *pen);
  virtual void SetBrush(wxBrush *brush);
  virtual void SetLogicalFunction(int function);
  virtual void SetBackground(wxBrush *brush);
  virtual void SetTextForeground(wxColour *colour);
  virtual void SetTextBackground(wxColour *colour);
  virtual void SetClippingRegion(float x, float y, float width, float height);
  virtual void DestroyClippingRegion(void);
  virtual void DrawText(const char *text, float x, float y, Bool use16 = FALSE);

  // Actual size in pixels when scrolling is taken into account
  virtual void GetVirtualSize(int *x, int *y) = 0;
  virtual float GetCharHeight(void);
  virtual float GetCharWidth(void);
  virtual void GetTextExtent(const char *string, float *x, float *y,
                             float *descent = NULL,
                             float *externalLeading = NULL,
                             wxFont *theFont = NULL, Bool use16 = FALSE);

  // Gets 'context' member
  virtual wxCanvasDC *GetDC(void);

  virtual void SetColourMap(wxColourMap *cmap) = 0;

  // Enable/disable Windows 3.1 scrolling in either direction.
  // If TRUE, wxWindows scrolls the canvas and only a bit of
  // the canvas is invalidated; no Clear() is necessary.
  // If FALSE, the whole canvas is invalidated and a Clear() is
  // necessary. Disable for when the scroll increment is used
  // to actually scroll a non-constant distance
  virtual void EnableScrolling(Bool x_scrolling, Bool y_scrolling) = 0;

  Bool IsRetained(void) { return is_retained; }

  virtual void WarpPointer(int x_pos, int y_pos) = 0 ;
};

#endif // IN_CPROTO
#endif // wxb_canvsh
