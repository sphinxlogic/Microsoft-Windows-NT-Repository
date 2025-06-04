/*
 * File:	wb_canvs.cc
 * Purpose:	wxbCanvas implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wb_canvs.cc,v 1.3 1994/08/14 21:34:01 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "@(#)wb_canvs.cc	1.2 5/9/94"; */

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_defs.h"
#include "wx_setup.h"
#include "wx_dc.h"
#include "wx_canvs.h"
#endif

class wxFrame;

wxbCanvas::wxbCanvas(void)
{
  WXSET_TYPE(wxCanvas, wxTYPE_CANVAS)
}

wxbCanvas::wxbCanvas(wxWindow *WXUNUSED(window), int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height),
   long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxCanvas, wxTYPE_CANVAS)
  
  windowStyle = style;
}

wxbCanvas::~wxbCanvas(void)
{
}

void wxbCanvas::AllowDoubleClick(int value)
{
  doubleClickAllowed = value ;
}

void wxbCanvas::SetClippingRegion(float cx, float cy, float cw, float ch)
{
  if (wx_dc)
    wx_dc->SetClippingRegion(cx, cy, cw, ch);
}

void wxbCanvas::DestroyClippingRegion(void)
{
  if (wx_dc)
    wx_dc->DestroyClippingRegion();
}

wxCanvasDC *wxbCanvas::GetDC(void)
{
  return wx_dc;
}

void wxbCanvas::Clear(void)
{
  if (wx_dc)
    wx_dc->Clear();
}

// Default input behaviour for a scrolling canvas should be to scroll
// according to the cursor keys pressed
void wxbCanvas::OnChar(wxKeyEvent& event)
{
  int x_page = 0;
  int y_page = 0;
  int start_x = 0;
  int start_y = 0;
  // Bugfix Begin
  int v_width = 0;
  int v_height = 0;
  int y_pages = 0;
  // Bugfix End

  GetScrollUnitsPerPage(&x_page, &y_page);
  // Bugfix Begin
  GetVirtualSize(&v_width,&v_height);
  // Bugfix End
  ViewStart(&start_x, &start_y);
  // Bugfix begin
  if (vert_units)
	 y_pages = (int)(v_height/vert_units) - y_page;

#ifdef wx_msw
  int y = 0;
#else
  int y = y_page-1;
#endif
 // Bugfix End
  switch (event.keyCode)
  {
	 case WXK_PRIOR:
	 {
	 // BugFix Begin
		if (y_page > 0)
		  {
		  if (start_y - y_page > 0)
			 Scroll(start_x, start_y - y_page);
		  else
			 Scroll(start_x, 0);
		  }
		  // Bugfix End
		break;
	 }
	 case WXK_NEXT:
	 {
	 // Bugfix Begin
		if ((y_page > 0)  && (start_y <= y_pages-y-1))
						 {
			if (y_pages + y < start_y + y_page)
			  Scroll(start_x, y_pages + y);
			else
			  Scroll(start_x, start_y + y_page);
			}
	 // Bugfix End
		break;
	 }
	 case WXK_UP:
	 {
		if ((y_page > 0) && (start_y >= 1))
		  Scroll(start_x, start_y - 1);
		break;
	 }
	 case WXK_DOWN:
	 {
	 // Bugfix Begin
//		if ((y_page > 0) && (start_y <= y_pages-y-1))
		if ((y_page > 0) && (start_y <= y_pages))
	 // Bugfix End
		{
		  Scroll(start_x, start_y + 1);
		}
		break;
	 }
	 case WXK_LEFT:
	 {
		if ((x_page > 0) && (start_x >= 1))
		  Scroll(start_x - 1, start_y);
		break;
	 }
	 case WXK_RIGHT:
	 {
		if (x_page > 0)
		  Scroll(start_x + 1, start_y);
		break;
	 }
	 case WXK_HOME:
	 {
		Scroll(0, 0);
		break;
	 }
	 // This is new
	 case WXK_END:
	 {
		Scroll(start_x, y_pages+y);
		break;
	 }
	 // end
  }
}

void wxbCanvas::FloodFill(float x, float y, wxColour *col, int style)
{
  if (wx_dc)
    wx_dc->FloodFill(x,y,col,style) ;
}

Bool wxbCanvas::GetPixel(float x, float y, wxColour *col)
{
  if (wx_dc)
    return wx_dc->GetPixel(x,y,col) ;
  else
    return FALSE;
}

void wxbCanvas::DrawLine(float x1, float y1, float x2, float y2)
{
  if (wx_dc)
    wx_dc->DrawLine(x1, y1, x2, y2);
}

void wxbCanvas::IntDrawLine(int x1, int y1, int x2, int y2)
{
  if (wx_dc)
    wx_dc->IntDrawLine(x1, y1, x2, y2);
}

void wxbCanvas::CrossHair(int x, int y)
{
  if (wx_dc)
    wx_dc->CrossHair(x, y);
}

void wxbCanvas::DrawArc(float x1,float y1,float x2,float y2,float xc,float yc)
{
  if (wx_dc)
    wx_dc->DrawArc(x1, y1, x2, y2,xc,yc);
}

void wxbCanvas::DrawPoint(float x, float y)
{
  if (wx_dc)
    wx_dc->DrawPoint(x, y);
}

void wxbCanvas::DrawPolygon(int n, wxPoint points[], float xoffset, float yoffset,int fillStyle)
{
  if (wx_dc)
    wx_dc->DrawPolygon(n, points, xoffset, yoffset,fillStyle);
}

void wxbCanvas::DrawPolygon(wxList *list, float xoffset, float yoffset,int fillStyle)
{
  if (wx_dc)
    ((wxDC *)wx_dc)->DrawPolygon(list, xoffset, yoffset,fillStyle);
}

void wxbCanvas::DrawLines(int n, wxPoint points[], float xoffset, float yoffset)
{
  if (wx_dc)
    wx_dc->DrawLines(n, points, xoffset, yoffset);
}

void wxbCanvas::DrawLines(int n, wxIntPoint points[], int xoffset, int yoffset)
{
  if (wx_dc)
    wx_dc->DrawLines(n, points, xoffset, yoffset);
}

void wxbCanvas::DrawLines(wxList *list, float xoffset, float yoffset)
{
  if (wx_dc)
    ((wxDC *)wx_dc)->DrawLines(list, xoffset, yoffset);
}

void wxbCanvas::DrawRectangle(float x, float y, float width, float height)
{
  if (wx_dc)
    wx_dc->DrawRectangle(x, y, width, height);
}

void wxbCanvas::DrawRoundedRectangle(float x, float y, float width, float height, float radius)
{
  if (wx_dc)
    wx_dc->DrawRoundedRectangle(x, y, width, height, radius);
}

void wxbCanvas::DrawEllipse(float x, float y, float width, float height)
{
  if (wx_dc)
    wx_dc->DrawEllipse(x, y, width, height);
}


void wxbCanvas::SetFont(wxFont *the_font)
{
  if (wx_dc)
    wx_dc->SetFont(the_font);
}

void wxbCanvas::SetPen(wxPen *pen)
{
  if (wx_dc)
    wx_dc->SetPen(pen);
}

void wxbCanvas::SetTextForeground(wxColour *colour)
{
  if (wx_dc)
    wx_dc->SetTextForeground(colour);
}

void wxbCanvas::SetTextBackground(wxColour *colour)
{
  if (wx_dc)
    wx_dc->SetTextBackground(colour);
}

void wxbCanvas::SetBrush(wxBrush *brush)
{
  if (wx_dc)
    wx_dc->SetBrush(brush);
}

void wxbCanvas::DrawText(const char *text, float x, float y, Bool use16bit)
{
  if (wx_dc)
    wx_dc->DrawText(text, x, y, use16bit);
}

void wxbCanvas::SetBackground(wxBrush *brush)
{
  if (wx_dc)
    wx_dc->SetBackground(brush);
}

void wxbCanvas::SetLogicalFunction(int function)
{
  if (wx_dc)
    wx_dc->SetLogicalFunction(function);
}

#if USE_SPLINES
// Make a 3-point spline
void wxbCanvas::DrawSpline(float x1, float y1, float x2, float y2, float x3, float y3)
{
  if (wx_dc)
    wx_dc->DrawSpline(x1, y1, x2, y2, x3, y3);
}

void wxbCanvas::DrawSpline(wxList *list)
{
  if (wx_dc)
    wx_dc->DrawSpline(list);
}
#endif

float wxbCanvas::GetCharHeight(void)
{
  if (wx_dc)
    return wx_dc->GetCharHeight();
  else
    return (float)0.0;
}

float wxbCanvas::GetCharWidth(void)
{
  if (wx_dc)
    return wx_dc->GetCharWidth();
  else
    return (float)0.0;
}

void wxbCanvas::GetTextExtent(const char *string, float *x, float *y,
                              float *descent, float *externalLeading, wxFont *theFont, Bool use16bit)
{
  if (wx_dc)
    wx_dc->GetTextExtent(string, x, y, descent, externalLeading, theFont, use16bit);
  else
    wxWindow::GetTextExtent(string, x, y, descent, externalLeading, theFont, use16bit);
}
