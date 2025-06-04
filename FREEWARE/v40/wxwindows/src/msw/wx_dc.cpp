/*
 * File:	wx_dc.cc
 * Purpose:	Device context implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_dc.cc,v 1.1 1994/08/14 21:59:17 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_dcpan.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_dcpan.h"
#include "wx_frame.h"
#include "wx_dc.h"
#include "wx_dccan.h"
#include "wx_dcmem.h"
#include "wx_dcps.h"
#include "wx_utils.h"
#include "wx_canvs.h"
#include "wx_dialg.h"
#include "wx_main.h"
#endif

#include "wx_privt.h"
#include "math.h"

#if USE_COMMON_DIALOGS
#include <commdlg.h>
#endif

#ifndef WIN32
#include <print.h>
#endif

// Declarations local to this file

#define YSCALE(y) (yorigin - (y))

// #define     wx_round(a)    (int)((a)+.5)

// Default constructor
wxDC::wxDC(void)
{
  WXSET_TYPE(wxDC, wxTYPE_DC)
  
  filename = NULL;
  selected_bitmap = NULL;
  canvas = NULL;
  cur_dc = NULL ;
  cur_bk = 0 ;
  cur_cpen = NULL ;
  cur_cbrush = NULL ;
  old_bitmap = 0;
  old_pen = 0;
  old_brush = 0;
  old_font = 0;
  old_palette = 0;
  min_x = 0; min_y = 0; max_x = 0; max_y = 0;
  font = NULL;
  logical_origin_x = 0;
  logical_origin_y = 0;
  device_origin_x = 0;
  device_origin_y = 0;
  logical_scale_x = 1.0;
  logical_scale_y = 1.0;
  user_scale_x = 1.0;
  user_scale_y = 1.0;
  system_scale_x = 1.0;
  system_scale_y = 1.0;
  mapping_mode = MM_TEXT;
  title = NULL;
  dont_delete = FALSE;
  cdc = NULL;
  clipping = FALSE;
  ok = TRUE;
  window_ext_x = VIEWPORT_EXTENT;
  window_ext_y = VIEWPORT_EXTENT;
  current_logical_function = -1;
  current_pen = NULL;
  current_brush = NULL;

  current_background_brush = wxWHITE_BRUSH;
  current_background_brush->UseResource();
  
  current_text_foreground = *wxBLACK;
  current_text_background = *wxWHITE;
  Colour = wxColourDisplay();
}


wxDC::~wxDC(void)
{
  if (filename)
    delete[] filename;

  if (cdc)
  {
    SelectOldObjects(cdc);
    DeleteDC(cdc);
  }
}

// This will select current objects out of the DC,
// which is what you have to do before deleting the
// DC.
void wxDC::SelectOldObjects(HDC dc)
{
#if DEBUG > 1
  wxDebugMsg("wxDC::SelectOldObjects %X\n", this);
#endif
  if (dc)
  {
    if (old_bitmap)
    {
#if DEBUG > 1
      wxDebugMsg("wxDC::SelectOldObjects: Selecting old HBITMAP %X\n", old_bitmap);
#endif
      ::SelectObject(dc, old_bitmap);
      if (selected_bitmap)
      {
        selected_bitmap->selectedInto = NULL;
        selected_bitmap = NULL;
      }
    }
    old_bitmap = NULL ;
    if (old_pen)
    {
#if DEBUG > 1
      wxDebugMsg("wxDC::SelectOldObjects: Selecting old HPEN %X\n", old_pen);
#endif
      ::SelectObject(dc, old_pen);
    }
    old_pen = NULL ;
    if (old_brush)
    {
#if DEBUG > 1
      wxDebugMsg("wxDC::SelectOldObjects: Selecting old HBRUSH %X\n", old_brush);
#endif
      ::SelectObject(dc, old_brush);
    }
    old_brush = NULL ;
    if (old_font)
    {
#if DEBUG > 1
      wxDebugMsg("wxDC::SelectOldObjects: Selecting old HFONT %X\n", old_font);
#endif
      ::SelectObject(dc, old_font);
    }
    old_font = NULL ;
    if (old_palette)
    {
#if DEBUG > 1
      wxDebugMsg("wxDC::SelectOldObjects: Selecting old HPALETTE %X\n", old_palette);
#endif
      ::SelectPalette(dc, old_palette, TRUE);
    }
#if DEBUG > 1
    wxDebugMsg("wxDC::SelectOldObjects: Done.\n");
#endif
    old_palette = NULL ;
  }
  if (font)
    font->ReleaseResource();
  if (current_pen)
    current_pen->ReleaseResource();
  if (current_brush)
    current_brush->ReleaseResource();
  if (current_background_brush)
    current_background_brush->ReleaseResource();
}

void wxDC::SetClippingRegion(float cx, float cy, float cw, float ch)
{
  clipping = TRUE;
  clip_x1 = (int)cx;
  clip_y1 = (int)cy;
  clip_x2 = (int)(cx + cw);
  clip_y2 = (int)(cy + ch);

  HDC dc = NULL;

  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  DoClipping(dc);

  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

void wxDC::DoClipping(HDC dc)
{
  if (clipping && dc)
  {
    int x_off = 0;
    int y_off = 0;
    if (canvas && (canvas->wxWinType == wxTYPE_XWND))
    {
      wxWnd *wnd = (wxWnd *)canvas->handle;
      wnd->CalcScrolledPosition(0, 0, &x_off, &y_off);
    }
//    HRGN rgn = CreateRectRgn(XLOG2DEV(clip_x1 + x_off), YLOG2DEV(clip_y1 + y_off),
//                          XLOG2DEV(clip_x2 + x_off), YLOG2DEV(clip_y2 + y_off));

//    SelectClipRgn(dc, rgn);
//    DeleteObject(rgn);
    IntersectClipRect(dc, XLOG2DEV(clip_x1 + x_off), YLOG2DEV(clip_y1 + y_off),
                          XLOG2DEV(clip_x2 + x_off), YLOG2DEV(clip_y2 + y_off));
  }
}

void wxDC::DestroyClippingRegion(void)
{
  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  if (clipping && dc)
  {
//        SelectClipRgn(dc, NULL);
    HRGN rgn = CreateRectRgn(0, 0, 32000, 32000);
#if DEBUG > 1
    wxDebugMsg("wxDC::DestroyClippingRegion: Selecting HRGN %X\n", rgn);
#endif
    SelectClipRgn(dc, rgn);
#if DEBUG > 1
    wxDebugMsg("wxDC::DestroyClippingRegion: Deleting HRGN %X\n", rgn);
#endif
    DeleteObject(rgn);
   }
   clipping = FALSE;

  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

Bool wxDC::CanDrawBitmap(void)
{
  return TRUE;
}

Bool wxDC::CanGetTextExtent(void)
{
  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();
  
  // What sort of display is it?
  int technology = ::GetDeviceCaps(dc, TECHNOLOGY);

  Bool ok;
  
  if (technology != DT_RASDISPLAY && technology != DT_RASPRINTER)
    ok = FALSE;
  else ok = TRUE;

  if (!cdc && canvas)
    canvas->ReleaseHDC();

  return ok;
}

void wxDC::SetColourMap(wxColourMap *cmap)
{
  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  if (!cmap)
  {
    // Setting a NULL colourmap is a way of restoring
    // the original colourmap
    if (old_palette)
    {
      ::SelectPalette(dc, old_palette, TRUE);
#if DEBUG > 1
      wxDebugMsg("wxDC::SetColourMap: set old palette %X\n", old_palette);
#endif
      old_palette = 0;
    }
  }
    
  if (cmap && cmap->ms_palette)
  {
    HPALETTE oldPal = ::SelectPalette(dc, cmap->ms_palette, TRUE);
    if (!old_palette)
      old_palette = oldPal;
      
#if DEBUG > 1
    wxDebugMsg("wxDC::SetColourMap %X: selected palette %X\n", this, cmap->ms_palette);
    if (oldPal)
      wxDebugMsg("wxDC::SetColourMap: oldPal was palette %X\n", oldPal);
    if (old_palette)
      wxDebugMsg("wxDC::SetColourMap: old_palette is palette %X\n", old_palette);
#endif
    ::RealizePalette(dc);
  }

  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

void wxDC::Clear(void)
{
  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  RECT rect;
  if (canvas)
    GetClientRect(canvas->GetHWND(), &rect);
  else if (selected_bitmap)
  {
    rect.left = 0; rect.top = 0;
    rect.right = selected_bitmap->GetWidth();
    rect.bottom = selected_bitmap->GetHeight();
  }
  (void) ::SetMapMode(dc, MM_TEXT);

  DWORD colour = GetBkColor(dc);
  HBRUSH brush = CreateSolidBrush(colour);
  FillRect(dc, &rect, brush);
  DeleteObject(brush);

  ::SetMapMode(dc, MM_ANISOTROPIC);
  ::SetViewportExtEx(dc, VIEWPORT_EXTENT, VIEWPORT_EXTENT, NULL);
  ::SetWindowExtEx(dc, window_ext_x, window_ext_y, NULL);
  ::SetViewportOrgEx(dc, (int)device_origin_x, (int)device_origin_y, NULL);
  ::SetWindowOrgEx(dc, (int)logical_origin_x, (int)logical_origin_y, NULL);

  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

void wxDC::BeginDrawing(void)
{
  if (canvas)
    cdc = canvas->GetHDC() ;
}

void wxDC::EndDrawing(void)
{
  if (canvas)
  {
    canvas->ReleaseHDC() ;
    if (canvas->countHDC == 0)
      cdc = 0;
  }
}

void wxDC::FloodFill(float x, float y, wxColour *col, int style)
{
  int xx = (int)x;
  int yy = (int)y;

  if (current_brush && autoSetting)
    SetBrush(current_brush);

  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition((int)x, (int)y, &xx, &yy);
  }
  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC() ;

  (void)ExtFloodFill(dc, XLOG2DEV(xx), YLOG2DEV(yy),
                        col->pixel,
                        style==wxFLOOD_SURFACE?
                          FLOODFILLSURFACE:FLOODFILLBORDER
                        );

  if (!cdc && canvas)
    canvas->ReleaseHDC() ;

  CalcBoundingBox((float)x, (float)y);
}

Bool wxDC::GetPixel(float x, float y, wxColour *col)
{
  // added by steve 29.12.94 (copied from DrawPoint)
  // returns TRUE for pixels in the color of the current pen
  // and FALSE for all other pixels colors
  // if col is non-NULL return the color of the pixel
  int xx1 = (int)x;
  int yy1 = (int)y;
  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition((int)x, (int)y, &xx1, &yy1);
  }

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  // get the color of the pixel
  COLORREF pixelcolor = ::GetPixel(dc, XLOG2DEV(xx1), YLOG2DEV(yy1));
  // get the color of the pen
  COLORREF pencolor = 0x00ffffff;
  if (current_pen)
  {
    current_pen->RealizeResource();
    pencolor = current_pen->GetColour().pixel ;
  }
  
  if (!cdc && canvas)
    canvas->ReleaseHDC();

  // return the color of the pixel
  if(col)
    col->Set(GetRValue(pixelcolor),GetGValue(pixelcolor),GetBValue(pixelcolor));
  
  // check, if color of the pixels is the same as the color
  // of the current pen
  return(pixelcolor==pencolor);
}

void wxDC::IntDrawLine(int x1, int y1, int x2, int y2)
{
  int xx1 = (int)x1;
  int yy1 = (int)y1;
  int xx2 = (int)x2;
  int yy2 = (int)y2;

  if (current_pen && autoSetting)
    SetPen(current_pen);
 
  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition((int)x1, (int)y1, &xx1, &yy1);
    wnd->CalcScrolledPosition((int)x2, (int)y2, &xx2, &yy2);
  }
  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC() ;
 
   (void)MoveToEx(dc, XLOG2DEV(xx1), YLOG2DEV(yy1), NULL);
   (void)LineTo(dc, XLOG2DEV(xx2), YLOG2DEV(yy2));

 /* MATTHEW: [6] New normalization; add final point */
#if WX_STANDARD_GRAPHICS
  (void)LineTo(dc, XLOG2DEV(xx2) + 1, YLOG2DEV(yy2));
#endif
 
  if (!cdc && canvas)
    canvas->ReleaseHDC() ;
 
  CalcBoundingBox((float)x1, (float)y1);
  CalcBoundingBox((float)x2, (float)y2);
}

void wxDC::CrossHair(int x, int y)
{
      // We suppose that our screen is 2000x2000 max.
      int x1 = x-2000;
      int y1 = y-2000;
      int x2 = x+2000;
      int y2 = y+2000;

      int xx1 = x1 ;
      int yy1 = y1 ;
      int xx2 = x2 ;
      int yy2 = y2 ;
      int xx  = x ;
      int yy  = y ;

      if (current_pen && autoSetting)
        SetPen(current_pen);

      if (canvas && (canvas->wxWinType == wxTYPE_XWND))
      {
        wxWnd *wnd = (wxWnd *)canvas->handle;
        wnd->CalcScrolledPosition(x1, y1, &xx1, &yy1);
        wnd->CalcScrolledPosition(x2, y2, &xx2, &yy2);
        wnd->CalcScrolledPosition(x, y, &xx, &yy);
      }
      HDC dc = NULL;
      if (cdc)
        dc = cdc;
      else if (canvas)
        dc = canvas->GetHDC() ;

      (void)MoveToEx(dc, XLOG2DEV(xx1), YLOG2DEV(yy), NULL);
      (void)LineTo(dc, XLOG2DEV(xx2), YLOG2DEV(yy));

      (void)MoveToEx(dc, XLOG2DEV(xx), YLOG2DEV(yy1), NULL);
      (void)LineTo(dc, XLOG2DEV(xx), YLOG2DEV(yy2));

      if (!cdc && canvas)
        canvas->ReleaseHDC() ;

      CalcBoundingBox((float)x1, (float)y1);
      CalcBoundingBox((float)x2, (float)y2);
}

void wxDC::DrawLine(float x1, float y1, float x2, float y2)
{
  int xx1 = (int)x1;
  int yy1 = (int)y1;
  int xx2 = (int)x2;
  int yy2 = (int)y2;

// BUGBUG - is this necessary? YES YES YES YEs Yes yes ye....
  if (current_pen && autoSetting)
    SetPen(current_pen);

  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition((int)x1, (int)y1, &xx1, &yy1);
    wnd->CalcScrolledPosition((int)x2, (int)y2, &xx2, &yy2);
  }
  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  (void)MoveToEx(dc, XLOG2DEV(xx1), YLOG2DEV(yy1), NULL);
  (void)LineTo(dc, XLOG2DEV(xx2), YLOG2DEV(yy2));
  
  /* MATTHEW: [6] New normalization */
#if WX_STANDARD_GRAPHICS
  (void)LineTo(dc, XLOG2DEV(xx2) + 1, YLOG2DEV(yy2));
#endif

  if (!cdc && canvas)
    canvas->ReleaseHDC();

  CalcBoundingBox(xx1, yy1);
  CalcBoundingBox(xx2, yy2);
}

void wxDC::DrawArc(float x1,float y1,float x2,float y2,float xc,float yc)
{
  int xx1 = (int)x1;
  int yy1 = (int)y1;
  int xx2 = (int)x2;
  int yy2 = (int)y2;
  int xxc = (int)xc ;
  int yyc = (int)yc;

  double dx = xc-x1 ;
  double dy = yc-y1 ;
  double radius = (double)sqrt(dx*dx+dy*dy) ;;
  if (xx1==xx2 && xx2==yy2)
  {
    DrawEllipse(xc,yc,(float)(radius*2.0),(float)(radius*2)) ;
    return ;
  }

// BUGBUG - is this necessary?
  if (current_pen && autoSetting)
    SetPen(current_pen) ;

  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition((int)x1, (int)y1, &xx1, &yy1);
    wnd->CalcScrolledPosition((int)x2, (int)y2, &xx2, &yy2);
    wnd->CalcScrolledPosition((int)xc, (int)yc, &xxc, &yyc);
  }
  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC() ;

  xx1 = XLOG2DEV(xx1) ;
  yy1 = YLOG2DEV(yy1) ;
  xx2 = XLOG2DEV(xx2) ;
  yy2 = YLOG2DEV(yy2) ;
  xxc = XLOG2DEV(xxc) ;
  yyc = YLOG2DEV(yyc) ;
  double ray = sqrt(double((xxc-xx1)*(xxc-xx1)+(yyc-yy1)*(yyc-yy1))) ;
 
  (void)MoveToEx(dc, xx1, yy1, NULL);
  int xxx1 = (int)(xxc-ray);
  int yyy1 = (int)(yyc-ray);
  int xxx2 = (int)(xxc+ray);
  int yyy2 = (int)(yyc+ray);
  if (current_brush && current_brush->GetStyle() !=wxTRANSPARENT)
  {
// BUGBUG - is this necessary?
    if (current_brush&&current_brush->GetStyle()!=wxTRANSPARENT&&autoSetting)
      SetBrush(current_brush) ;
    Pie(dc,xxx1,yyy1,xxx2,yyy2,
        xx1,yy1,xx2,yy2) ;
  }
  else
    Arc(dc,xxx1,yyy1,xxx2,yyy2,
        xx1,yy1,xx2,yy2) ;

  if (!cdc && canvas)
    canvas->ReleaseHDC() ;

  CalcBoundingBox((float)(xc-radius), (float)(yc-radius));
  CalcBoundingBox((float)(xc+radius), (float)(yc+radius));
}

void wxDC::DrawEllipticArc(float WXUNUSED(x),float WXUNUSED(y),float WXUNUSED(w),float WXUNUSED(h),float WXUNUSED(sa),float WXUNUSED(ea))
{
  // Not implemented
}

void wxDC::DrawPoint(float x, float y)
{
// BUGBUG - is this necessary?
  if (current_pen && autoSetting)
    SetPen(current_pen) ;

  int xx1 = (int)x;
  int yy1 = (int)y;
  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition((int)x, (int)y, &xx1, &yy1);
  }

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();
    
  COLORREF color = 0x00ffffff;
  if (current_pen)
  {
    current_pen->RealizeResource();
    color = current_pen->GetColour().pixel ;
  }

/*
    color = RGB(current_pen->GetColour().Red(),
                current_pen->GetColour().Green(),
                current_pen->GetColour().Blue());
*/

  SetPixel(dc, XLOG2DEV(xx1), YLOG2DEV(yy1), color);

  if (!cdc && canvas)
    canvas->ReleaseHDC();

  CalcBoundingBox(x, y);
}

void wxDC::DrawPolygon(int n, wxPoint points[], float xoffset, float yoffset,int fillStyle)
{
  int xoffset1 = 0;
  int yoffset1 = 0;

// BUGBUG - is this necessary?
  if (current_pen && autoSetting)
    SetPen(current_pen) ;

  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition(0, 0, &xoffset1, &yoffset1);
  }

  xoffset1 += (int)xoffset; yoffset1 += (int)yoffset;

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  POINT *cpoints = new POINT[n];
  int i;
  for (i = 0; i < n; i++)
  {
    cpoints[i].x = (int)(XLOG2DEV(points[i].x + xoffset1));
    cpoints[i].y = (int)(YLOG2DEV(points[i].y + yoffset1));

    CalcBoundingBox(points[i].x + xoffset1, points[i].y + yoffset1);
  }

  int prev = SetPolyFillMode(dc,fillStyle==wxODDEVEN_RULE?ALTERNATE:WINDING) ;
  (void)Polygon(dc, cpoints, n);
  SetPolyFillMode(dc,prev) ;

  if (!cdc && canvas)
    canvas->ReleaseHDC();

  delete[] cpoints;
}

void wxDC::DrawLines(int n, wxIntPoint points[], int xoffset, int yoffset)
{
  int xoffset1 = 0;
  int yoffset1 = 0;
  if (current_pen && autoSetting)
    SetPen(current_pen) ;
 
  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition(0, 0, &xoffset1, &yoffset1);
  }
  xoffset1 += (int)xoffset; yoffset1 += (int)yoffset;
 
  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC() ;
 
  POINT *cpoints = new POINT[n];
  int i;
  for (i = 0; i < n; i++)
  {
    cpoints[i].x = (int)(XLOG2DEV(points[i].x + xoffset1));
    cpoints[i].y = (int)(YLOG2DEV(points[i].y + yoffset1));

    CalcBoundingBox((float)points[i].x + xoffset1, (float)points[i].y + yoffset1);
  }
 
  (void)Polyline(dc, cpoints, n);

  if (!cdc && canvas)
    canvas->ReleaseHDC() ;

  delete[] cpoints;
}

void wxDC::DrawLines(int n, wxPoint points[], float xoffset, float yoffset)
{
  int xoffset1 = 0;
  int yoffset1 = 0;

// BUGBUG - is this necessary?
  if (current_pen && autoSetting)
    SetPen(current_pen) ;

  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition(0, 0, &xoffset1, &yoffset1);
  }
  xoffset1 += (int)xoffset; yoffset1 += (int)yoffset;

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  POINT *cpoints = new POINT[n];
  int i;
  for (i = 0; i < n; i++)
  {
    cpoints[i].x = (int)(XLOG2DEV(points[i].x + xoffset1));
    cpoints[i].y = (int)(YLOG2DEV(points[i].y + yoffset1));

    CalcBoundingBox(points[i].x + xoffset1, points[i].y + yoffset1);
  }

 (void)Polyline(dc, cpoints, n);

  if (!cdc && canvas)
    canvas->ReleaseHDC();

  delete[] cpoints;
}

void wxDC::DrawRectangle(float x, float y, float width, float height)
{
  int x1 = (int)x;
  int y1 = (int)y;
  int x2 = (int)(x+width);
  int y2 = (int)(y+height);

// BUGBUG - is this necessary?
  if (current_pen && autoSetting)
    SetPen(current_pen) ;

  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition((int)x, (int)y, &x1, &y1);
    wnd->CalcScrolledPosition((int)(x+width), (int)(y+height), &x2, &y2);
  }

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

/* MATTHEW: [6] new normalization */
#if WX_STANDARD_GRAPHICS
  Bool do_brush, do_pen;

  do_brush = current_brush && current_brush->GetStyle() != wxTRANSPARENT;
  do_pen = current_pen && current_pen->GetStyle() != wxTRANSPARENT;

  if (do_brush) {
	 HPEN orig_pen = NULL;

	 if (do_pen || !current_pen)
		orig_pen = ::SelectObject(dc, ::GetStockObject(NULL_PEN));

	 (void)Rectangle(dc, XLOG2DEV(x1), YLOG2DEV(y1),
		  XLOG2DEV(x2) + 1, YLOG2DEV(y2) + 1);

	 if (do_pen || !current_pen)
		::SelectObject(dc , orig_pen);
  }
  if (do_pen) {
	 HBRUSH orig_brush = NULL;

	 if (do_brush || !current_brush)
		orig_brush = ::SelectObject(dc, ::GetStockObject(NULL_BRUSH));

	 (void)Rectangle(dc, XLOG2DEV(x1), YLOG2DEV(y1),
		  XLOG2DEV(x2), YLOG2DEV(y2));

	 if (do_brush || !current_brush)
		::SelectObject(dc, orig_brush);
  }
#else
  (void)Rectangle(dc, XLOG2DEV(x1), YLOG2DEV(y1), XLOG2DEV(x2), YLOG2DEV(y2));
#endif

  CalcBoundingBox((float)x1, (float)y1);
  CalcBoundingBox((float)x2, (float)y2);

  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

void wxDC::DrawRoundedRectangle(float x, float y, float width, float height, float radius)
{
  // Now, a negative radius value is interpreted to mean
  // 'the proportion of the smallest X or Y dimension'

  if (radius < 0.0)
  {
    float smallest = 0.0;
    if (width < height)
      smallest = width;
    else
      smallest = height;
    radius = (float)(- radius * smallest);
  }

  int x1 = (int)x;
  int y1 = (int)y;
  int x2 = (int)(x+width);
  int y2 = (int)(y+height);

// BUGBUG - is this necessary?
  if (current_pen && autoSetting)
    SetPen(current_pen) ;

  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition((int)x, (int)y, &x1, &y1);
    wnd->CalcScrolledPosition((int)(x+width), (int)(y+height), &x2, &y2);
  }

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  (void)RoundRect(dc, XLOG2DEV(x1), YLOG2DEV(y1), XLOG2DEV(x2),
                      YLOG2DEV(y2), 2*XLOG2DEV(radius), 2*YLOG2DEV(radius));

  CalcBoundingBox((float)x1, (float)y1);
  CalcBoundingBox((float)x2, (float)y2);

  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

void wxDC::DrawEllipse(float x, float y, float width, float height)
{
  int x1 = (int)x;
  int y1 = (int)y;
  int x2 = (int)(x+width);
  int y2 = (int)(y+height);

// BUGBUG - is this necessary?
  if (current_pen && autoSetting)
    SetPen(current_pen) ;

  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition((int)x, (int)y, &x1, &y1);
    wnd->CalcScrolledPosition((int)(x+width), (int)(y+height), &x2, &y2);
  }

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  (void)Ellipse(dc, XLOG2DEV(x1), YLOG2DEV(y1), XLOG2DEV(x2), YLOG2DEV(y2));

  if (!cdc && canvas)
    canvas->ReleaseHDC();

  CalcBoundingBox((float)x1, (float)y1);
  CalcBoundingBox((float)x2, (float)y2);
}

void wxDC::DrawIcon(wxIcon *icon, float x, float y)
{
  int x1 = (int)x;
  int y1 = (int)y;

  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition((int)x, (int)y, &x1, &y1);
  }

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  ::DrawIcon(dc, XLOG2DEV(x1), YLOG2DEV(y1), icon->ms_icon);
  CalcBoundingBox((float)x1, (float)y1);

  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

void wxDC::SetFont(wxFont *the_font)
{
  // Release the current font from servitude (decrements the usage count)
  if (font)
    font->ReleaseResource();
  if (the_font)
    the_font->UseResource();

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  font = the_font;

  if (!the_font)
  {
    if (old_font)
      ::SelectObject(dc, old_font);
    old_font = NULL ;
  }
  if (font)
    font->RealizeResource() ;

  if (font && font->cfont)
  {
#if DEBUG > 1
    wxDebugMsg("wxDC::SetFont: Selecting HFONT %X\n", font->cfont);
#endif
    HFONT f = ::SelectObject(dc, font->cfont);
    if (!old_font)
      old_font = f;
  }
  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

void wxDC::SetPen(wxPen *pen)
{
  HDC dc = NULL;

  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  if (current_pen)
    current_pen->ReleaseResource();
  if (pen)
    pen->UseResource();

  current_pen = pen;

  if (!pen)
  {
    if (old_pen)
      ::SelectObject(dc, old_pen);
    old_pen = NULL ;
  }

  if (pen)
  {
    pen->RealizeResource();
    if (pen->GetResourceHandle())
    {
      HPEN p = ::SelectObject(dc, (HPEN)pen->GetResourceHandle()) ;
      if (!old_pen)
        old_pen = p;
    }
  }

  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

void wxDC::SetBrush(wxBrush *brush)
{
  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  if (current_brush)
    current_brush->ReleaseResource();
  if (brush)
    brush->UseResource();

  current_brush = brush;

  if (!brush)
  {
    if (old_brush)
      ::SelectObject(dc, old_brush);
    old_brush = NULL ;
  }

  if (brush)
  {
    brush->RealizeResource();

    if (brush->GetResourceHandle())
    {
      HBRUSH b = 0;
      b = ::SelectObject(dc, (HBRUSH)brush->GetResourceHandle()) ;
      if (!old_brush)
        old_brush = b;
    }
  }
  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

void wxDC::DrawText(const char *text, float x, float y, Bool use16bit)
{
  int xx1 = (int)x;
  int yy1 = (int)y;

  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition((int)x, (int)y, &xx1, &yy1);
  }

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  if (font && font->cfont)
  {
#if DEBUG > 1
    wxDebugMsg("wxDC::DrawText: Selecting HFONT %X\n", font->cfont);
#endif
    HFONT f = ::SelectObject(dc, font->cfont);
    if (!old_font)
      old_font = f;
  }

  if (current_text_foreground.Ok())
    SetTextColor(dc, current_text_foreground.pixel) ;

  DWORD old_background;
  if (current_text_background.Ok())
  {
    old_background = SetBkColor(dc, current_text_background.pixel) ;
  }

  if (current_bk_mode == wxTRANSPARENT)
    SetBkMode(dc, TRANSPARENT);
  else
    SetBkMode(dc, OPAQUE);

  (void)TextOut(dc, XLOG2DEV(xx1), YLOG2DEV(yy1), text, strlen(text));

  if (current_text_background.Ok())
    (void)SetBkColor(dc, old_background);

  if (!cdc && canvas)
    canvas->ReleaseHDC();

  CalcBoundingBox((float)xx1, (float)yy1);

  float w, h;
  GetTextExtent(text, &w, &h);
  CalcBoundingBox((float)(xx1 + w), (float)(yy1 + h));
}

void wxDC::SetBackground(wxBrush *brush)
{
  if (current_background_brush)
    current_background_brush->ReleaseResource();
  if (brush)
    brush->UseResource();

  current_background_brush = brush;

  if (!brush)
    return;

  brush->RealizeResource() ;

  if (canvas)
  {
    Bool customColours = TRUE;
    // If we haven't specified wxUSER_COLOURS, don't allow the panel/dialog box to
    // change background colours from the control-panel specified colours.
    if (canvas->IsKindOf(CLASSINFO(wxPanel)) && ((canvas->GetWindowStyleFlag() & wxUSER_COLOURS) != wxUSER_COLOURS))
      customColours = FALSE;
      
    if (customColours)
    {
      wxCanvasWnd *wnd = (wxCanvasWnd *)canvas->handle;
      //wnd->background_brush = brush->cbrush;
      HBRUSH br = (brush->GetStyle()==wxTRANSPARENT) ?
                              GetStockObject(NULL_BRUSH) : brush->cbrush;
      // Remember we don't want to delete this brush when we change the
      // background again or delete the window.
      wnd->SetBackgroundBrush(br, FALSE);
      if (brush->GetStyle()==wxTRANSPARENT)
      {
        wnd->background_transparent = TRUE;
      }
      else
      {
        wnd->background_colour = PALETTERGB(brush->GetColour().Red(), brush->GetColour().Green(), brush->GetColour().Blue());
        wnd->background_transparent = FALSE;
      }
    }
  }
  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC() ;

  if (current_background_brush)
  {
  COLORREF new_color = current_background_brush->colour.pixel ;
    if (new_color!=cur_bk || dc!=cur_dc)
    {
      (void)SetBkColor(dc, new_color);
      cur_bk = new_color ;
      cur_dc = dc ;
    }
  }

  if (!cdc && canvas)
    canvas->ReleaseHDC() ;
}

void wxDC::SetBackgroundMode(int mode)
{
  current_bk_mode = mode;

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  if (current_bk_mode == wxTRANSPARENT)
    ::SetBkMode(dc, TRANSPARENT);
  else
    ::SetBkMode(dc, OPAQUE);

  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

void wxDC::SetLogicalFunction(int function)
{
  current_logical_function = function;

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  SetRop(dc);

  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

void wxDC::SetRop(HDC dc)
{
  if (!dc || current_logical_function < 0)
    return;

  int c_rop;
  // These may be wrong
  switch (current_logical_function)
  {
//    case wxXOR: c_rop = R2_XORPEN; break;
    case wxXOR: c_rop = R2_NOTXORPEN; break;
    case wxINVERT: c_rop = R2_NOT; break;
    case wxOR_REVERSE: c_rop = R2_MERGEPENNOT; break;
    case wxAND_REVERSE: c_rop = R2_MASKPENNOT; break;
    case wxCLEAR: c_rop = R2_WHITE; break;
    case wxSET: c_rop = R2_BLACK; break;
    case wxSRC_INVERT: c_rop = R2_NOTCOPYPEN; break;
    case wxOR_INVERT: c_rop = R2_MERGENOTPEN; break;
    case wxAND: c_rop = R2_MASKPEN; break;
    case wxOR: c_rop = R2_MERGEPEN; break;
    case wxAND_INVERT: c_rop = R2_MASKNOTPEN; break;
    case wxEQUIV:
    case wxNAND:
    case wxCOPY:
    default:
      c_rop = R2_COPYPEN; break;
  }
  SetROP2(dc, c_rop);
}

Bool wxDC::StartDoc(char *message)
{
  if (!this->IsKindOf(CLASSINFO(wxPrinterDC)))
    return TRUE;
    
  Bool flag = FALSE;

  DOCINFO docinfo;
  docinfo.cbSize = sizeof(DOCINFO);
  docinfo.lpszDocName = message;
  docinfo.lpszOutput = filename;
#if WIN95
  docinfo.lpszDatatype = NULL;
  docinfo.fwType = 0;
#endif

  if (cdc) flag = (SP_ERROR != ::StartDoc(cdc, &docinfo));
  else flag = FALSE;

  return flag;
}

void wxDC::EndDoc(void)
{
  if (!this->IsKindOf(CLASSINFO(wxPrinterDC)))
    return;
  if (cdc) ::EndDoc(cdc);
}

void wxDC::StartPage(void)
{
  if (!this->IsKindOf(CLASSINFO(wxPrinterDC)))
    return;
  if (cdc)
    ::StartPage(cdc);
}

void wxDC::EndPage(void)
{
  if (!this->IsKindOf(CLASSINFO(wxPrinterDC)))
    return;
  if (cdc)
    ::EndPage(cdc);
}

float wxDC::GetCharHeight(void)
{
  TEXTMETRIC lpTextMetric;
  HDC dc = NULL;

  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  GetTextMetrics(dc, &lpTextMetric);

  if (!cdc && canvas)
   canvas->ReleaseHDC();

  return (float)YDEV2LOGREL(lpTextMetric.tmHeight);
}

float wxDC::GetCharWidth(void)
{
  TEXTMETRIC lpTextMetric;
  HDC dc = NULL;

  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  GetTextMetrics(dc, &lpTextMetric);

  if (!cdc && canvas)
    canvas->ReleaseHDC();

  return (float)XDEV2LOGREL(lpTextMetric.tmAveCharWidth);
}

void wxDC::GetTextExtent(const char *string, float *x, float *y,
                         float *descent, float *externalLeading, wxFont *theFont, Bool use16bit)
{
  wxFont *fontToUse = theFont;
  if (!fontToUse)
    fontToUse = font;

  HDC dc = NULL;

  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  SIZE sizeRect;
  TEXTMETRIC tm;

  GetTextExtentPoint(dc, string, strlen(string), &sizeRect);
  GetTextMetrics(dc, &tm);

  if (!cdc && canvas)
    canvas->ReleaseHDC();

  *x = (float)XDEV2LOGREL(sizeRect.cx);
  *y = (float)YDEV2LOGREL(sizeRect.cy);
  if (descent) *descent = (float)tm.tmDescent;
  if (externalLeading) *externalLeading = (float)tm.tmExternalLeading;
}

void wxDC::SetMapMode(int mode)
{
  mapping_mode = mode;

  int pixel_width = 0;
  int pixel_height = 0;
  int mm_width = 0;
  int mm_height = 0;

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC() ;

  pixel_width = GetDeviceCaps(dc, HORZRES);
  pixel_height = GetDeviceCaps(dc, VERTRES);
  mm_width = GetDeviceCaps(dc, HORZSIZE);
  mm_height = GetDeviceCaps(dc, VERTSIZE);

  if ((pixel_width == 0) || (pixel_height == 0) || (mm_width == 0) || (mm_height == 0))
  {
    if (!cdc && canvas)
      canvas->ReleaseHDC() ;
    return;
  }

  float mm2pixelsX = (float)pixel_width/mm_width;
  float mm2pixelsY = (float)pixel_height/mm_height;

  switch (mode)
  {
    case MM_TWIPS:
    {
      logical_scale_x = (float)(twips2mm * mm2pixelsX);
      logical_scale_y = (float)(twips2mm * mm2pixelsY);
      break;
    }
    case MM_POINTS:
    {
      logical_scale_x = (float)(pt2mm * mm2pixelsX);
      logical_scale_y = (float)(pt2mm * mm2pixelsY);
      break;
    }
    case MM_METRIC:
    {
      logical_scale_x = mm2pixelsX;
      logical_scale_y = mm2pixelsY;
      break;
    }
    case MM_LOMETRIC:
    {
      logical_scale_x = (float)(mm2pixelsX/10.0);
      logical_scale_y = (float)(mm2pixelsY/10.0);
      break;
    }
    default:
    case MM_TEXT:
    {
      logical_scale_x = 1.0;
      logical_scale_y = 1.0;
      break;
    }
  }

  if (::GetMapMode(dc) != MM_ANISOTROPIC)
    ::SetMapMode(dc, MM_ANISOTROPIC);
    
  SetViewportExtEx(dc, VIEWPORT_EXTENT, VIEWPORT_EXTENT, NULL);
  window_ext_x = (int)MS_XDEV2LOG(VIEWPORT_EXTENT);
  window_ext_y = (int)MS_YDEV2LOG(VIEWPORT_EXTENT);
  ::SetWindowExtEx(dc, window_ext_x, window_ext_y, NULL);
  ::SetViewportOrgEx(dc, (int)device_origin_x, (int)device_origin_y, NULL);
  ::SetWindowOrgEx(dc, (int)logical_origin_x, (int)logical_origin_y, NULL);

  if (!cdc && canvas)
    canvas->ReleaseHDC() ;
}

void wxDC::SetUserScale(float x, float y)
{
  user_scale_x = x;
  user_scale_y = y;

  SetMapMode(mapping_mode);
}

void wxDC::SetSystemScale(float x, float y)
{
  system_scale_x = x;
  system_scale_y = y;

  SetMapMode(mapping_mode);
}

void wxDC::SetLogicalOrigin(float x, float y)
{
  logical_origin_x = x;
  logical_origin_y = y;

  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC() ;

  ::SetWindowOrgEx(dc, (int)logical_origin_x, (int)logical_origin_y, NULL);

  if (!cdc && canvas)
    canvas->ReleaseHDC() ;
}

void wxDC::SetDeviceOrigin(float x, float y)
{
  device_origin_x = x;
  device_origin_y = y;
  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC() ;

  ::SetViewportOrgEx(dc, (int)device_origin_x, (int)device_origin_y, NULL);

  if (!cdc && canvas)
    canvas->ReleaseHDC() ;
}

float wxDC::DeviceToLogicalX(int x)
{
  return (float)MS_XDEV2LOG(x);
}

float wxDC::DeviceToLogicalXRel(int x)
{
  return (float)MS_XDEV2LOGREL(x);
}

float wxDC::DeviceToLogicalY(int y)
{
  return (float)MS_YDEV2LOG(y);
}

float wxDC::DeviceToLogicalYRel(int y)
{
  return (float)MS_YDEV2LOGREL(y);
}

int wxDC::LogicalToDeviceX(float x)
{
  return MS_XLOG2DEV(x);
}

int wxDC::LogicalToDeviceXRel(float x)
{
  return MS_XLOG2DEVREL(x);
}

int wxDC::LogicalToDeviceY(float y)
{
  return MS_YLOG2DEV(y);
}

int wxDC::LogicalToDeviceYRel(float y)
{
  return MS_YLOG2DEVREL(y);
}

Bool wxDC::Blit(float xdest, float ydest, float width, float height,
                wxCanvasDC *source, float xsrc, float ysrc, int rop, Bool useMask)
{
  HDC dc = NULL;
  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  HDC dc_src = NULL;
  if (source->cdc)
    dc_src = source->cdc;
  else if (source->canvas)
    dc_src = ::GetDC(source->canvas->GetHWND());

  // Chris Breeze 3/7/97: use Pen colour as foreground colour
  // when blitting from 1-bit bitmaps
  if (current_pen && autoSetting)
  {
    SetPen(current_pen);
    SetTextColor(dc, current_pen->GetColour().pixel) ;
  }
  int xdest1 = (int)xdest;
  int ydest1 = (int)ydest;
  int xsrc1 = (int)xsrc;
  int ysrc1 = (int)ysrc;

  if (canvas && (canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd = (wxWnd *)canvas->handle;
    wnd->CalcScrolledPosition((int)xdest, (int)ydest, &xdest1, &ydest1);
  }
  if (source->canvas  && (source->canvas->wxWinType == wxTYPE_XWND))
  {
    wxWnd *wnd_src = (wxWnd *)source->canvas->handle;
    wnd_src->CalcScrolledPosition((int)xsrc, (int)ysrc, &xsrc1, &ysrc1);
  }

  Bool success;
  if (useMask && source->selected_bitmap && source->selected_bitmap->GetMask())
  {

#if 0
	// Not implemented under Win95 (or maybe a specific device?)
    if (MaskBlt(dc, xdest1, ydest1, (int)width, (int)height,
            dc_src, xsrc1, ysrc1, source->selected_bitmap->GetMask()->GetMaskBitmap(),
            0, 0, 0xAACC0020))
    {
        // Success
    }
    else
#endif
    {
	    HDC dc_mask = CreateCompatibleDC(dc_src);
	    ::SelectObject(dc_mask, source->selected_bitmap->GetMask()->GetMaskBitmap());
        success = BitBlt(dc, xdest1, ydest1, (int)width, (int)height,
					 dc_mask, xsrc1, ysrc1, 0x00220326 /* NOTSRCAND */);
        success = BitBlt(dc, xdest1, ydest1, (int)width, (int)height,
					 dc_src, xsrc1, ysrc1, SRCPAINT);
	    ::SelectObject(dc_mask, 0);
	    ::DeleteDC(dc_mask);
    }
  }
  else
  {
    success = BitBlt(dc, xdest1, ydest1, (int)width, (int)height, dc_src,
                        xsrc1, ysrc1,
                rop == wxCOPY ? SRCCOPY :
                rop == wxCLEAR ? WHITENESS :
                rop == wxSET ? BLACKNESS :
                rop == wxINVERT ? DSTINVERT :
                rop == wxAND ? MERGECOPY :
                rop == wxOR ? MERGEPAINT :
                rop == wxSRC_INVERT ? NOTSRCCOPY :
                rop == wxXOR ? SRCINVERT :
                rop == wxOR_REVERSE ? MERGEPAINT :
                rop == wxAND_REVERSE ? SRCERASE :
                rop == wxSRC_OR ? SRCPAINT :
                rop == wxSRC_AND ? SRCAND :
                SRCCOPY);
  }

  if (!cdc && canvas)
    canvas->ReleaseHDC();

  if (!source->cdc && source->canvas)
    ::ReleaseDC(source->canvas->GetHWND(), dc_src);
  return success;
}

void wxDC::GetSize(float *width, float *height)
{
  HDC dc = NULL;

  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  int w=::GetDeviceCaps(dc,HORZRES);
  int h=::GetDeviceCaps(dc,VERTRES);
  *width = (float)w;
  *height = (float)h;

  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

void wxDC::GetSizeMM(float *width, float *height)
{
  HDC dc = NULL;

  if (cdc)
    dc = cdc;
  else if (canvas)
    dc = canvas->GetHDC();

  int w=::GetDeviceCaps(dc,HORZSIZE);
  int h=::GetDeviceCaps(dc,VERTSIZE);
  *width = (float)w;
  *height = (float)h;

  if (!cdc && canvas)
    canvas->ReleaseHDC();
}

wxCanvasDC::wxCanvasDC(void)
{
  WXSET_TYPE(wxCanvasDC, wxTYPE_DC_CANVAS)
  
  device = wxDEVICE_WINDOWS;
}

wxCanvasDC::wxCanvasDC(wxCanvas *the_canvas):wxbCanvasDC(the_canvas)
{
  WXSET_TYPE(wxCanvasDC, wxTYPE_DC_CANVAS)
  
  canvas = the_canvas;
  device = wxDEVICE_WINDOWS;
  SetBrush(wxWHITE_BRUSH);
  SetPen(wxBLACK_PEN);
}

wxCanvasDC::~wxCanvasDC(void)
{
}

void wxCanvasDC::GetClippingBox(float *x,float *y,float *w,float *h)
{
  if (clipping)
  {
    *x = (float)clip_x1 ;
    *y = (float)clip_y1 ;
    *w = (float)(clip_x2 - clip_x1) ;
    *h = (float)(clip_y2 - clip_y1) ;
  }
  else
   *x = *y = *w = *h = 0 ;
}

wxPrinterDC::wxPrinterDC(char *driver_name, char *device_name, char *file, Bool interactive)
{
  WXSET_TYPE(wxPrinterDC, wxTYPE_DC_PRINTER)
  
  wx_interactive = interactive;
  device = wxDEVICE_WINDOWS;

  if (file)
    filename = copystring(file);
  else filename = NULL;

#if USE_COMMON_DIALOGS
   if (interactive)
   {
     PRINTDLG pd;
	
     pd.lStructSize = sizeof( PRINTDLG );
     pd.hwndOwner=NULL;
     pd.hDevMode=(HANDLE)NULL;
     pd.hDevNames=(HANDLE)NULL;
     pd.Flags=PD_RETURNDC | PD_NOSELECTION | PD_NOPAGENUMS;
     pd.nFromPage=0;
     pd.nToPage=0;
     pd.nMinPage=0;
     pd.nMaxPage=0;
     pd.nCopies=1;
     pd.hInstance=(HINSTANCE)NULL;

     if ( PrintDlg( &pd ) != 0 )
     {
       cdc = pd.hDC;
       ok = TRUE;
     }
     else
     {
       ok = FALSE;
       return;
     }

     dont_delete = TRUE; // ??? WHY???
   }
   else
#endif
   if (driver_name && device_name && file)
   {
     cdc = CreateDC(driver_name, device_name, file, NULL);
     ok = cdc ? TRUE: FALSE;
   }
   else
   {
     cdc = wxGetPrinterDC();
     ok = cdc ? TRUE: FALSE;
   }
   
   if (cdc)
   {
//     int width = GetDeviceCaps(cdc, VERTRES);
//     int height = GetDeviceCaps(cdc, HORZRES);
     SetMapMode(MM_TEXT);
   }
  SetBrush(wxBLACK_BRUSH);
  SetPen(wxBLACK_PEN);
}

wxPrinterDC::wxPrinterDC(HDC theDC)
{
  WXSET_TYPE(wxPrinterDC, wxTYPE_DC_PRINTER)
  
  wx_interactive = FALSE;
  device = wxDEVICE_WINDOWS;

  filename = NULL;

  cdc = theDC;
  ok = TRUE;
  if (cdc)
 {
//     int width = GetDeviceCaps(cdc, VERTRES);
//     int height = GetDeviceCaps(cdc, HORZRES);
    SetMapMode(MM_TEXT);
  }
  SetBrush(wxBLACK_BRUSH);
  SetPen(wxBLACK_PEN);
}

wxPrinterDC::~wxPrinterDC(void)
{
}

HDC wxGetPrinterDC(void)
{
#if USE_COMMON_DIALOGS
    HDC         hDC;
    LPDEVMODE   lpDevMode = NULL;
    LPDEVNAMES  lpDevNames;
    LPSTR       lpszDriverName;
    LPSTR       lpszDeviceName;
    LPSTR       lpszPortName;

    PRINTDLG    pd;
    pd.lStructSize    = sizeof(PRINTDLG);
    pd.hwndOwner      = (HWND)NULL;
    pd.hDevMode       = NULL;
    pd.hDevNames      = NULL;
    pd.Flags          = PD_RETURNDEFAULT;
    pd.nCopies        = 1;

    if (!PrintDlg((LPPRINTDLG)&pd))
        return(NULL);

    if (!pd.hDevNames)
        return(NULL);

#ifdef __WINDOWS_386__
    lpDevNames = (LPDEVNAMES)MK_FP32(GlobalLock(pd.hDevNames));
#else
    lpDevNames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
#endif
    lpszDriverName = (LPSTR)lpDevNames + lpDevNames->wDriverOffset;
    lpszDeviceName = (LPSTR)lpDevNames + lpDevNames->wDeviceOffset;
    lpszPortName   = (LPSTR)lpDevNames + lpDevNames->wOutputOffset;
    GlobalUnlock(pd.hDevNames);

    if (pd.hDevMode)
#ifdef __WINDOWS_386__
        lpDevMode = (LPDEVMODE)MK_FP32(GlobalLock(pd.hDevMode));
#else
        lpDevMode = (LPDEVMODE)GlobalLock(pd.hDevMode);
#endif

#ifdef WIN32
    hDC = CreateDC(lpszDriverName, lpszDeviceName, lpszPortName, (DEVMODE *)lpDevMode);
#else
    hDC = CreateDC(lpszDriverName, lpszDeviceName, lpszPortName, (LPSTR)lpDevMode);
#endif

    if (pd.hDevMode && lpDevMode)
        GlobalUnlock(pd.hDevMode);

    if (pd.hDevNames)
    {
	GlobalFree(pd.hDevNames);
	pd.hDevNames=NULL;
    }
    if (pd.hDevMode)
    {
       GlobalFree(pd.hDevMode);
       pd.hDevMode=NULL;
    }
    return(hDC);
#else
  return 0;
#endif
}

/*
 * Memory DC
 *
 */

wxMemoryDC::wxMemoryDC(void)
{
  WXSET_TYPE(wxMemoryDC, wxTYPE_DC_MEMORY)
  
  device = wxDEVICE_WINDOWS;

  cdc = ::CreateCompatibleDC(NULL);
  ok = (cdc != NULL);

  SetBrush(wxWHITE_BRUSH);
  SetPen(wxBLACK_PEN);
}

wxMemoryDC::wxMemoryDC(wxCanvasDC *old_dc):wxbMemoryDC(old_dc)
{
  WXSET_TYPE(wxMemoryDC, wxTYPE_DC_MEMORY)
  
  device = wxDEVICE_WINDOWS;

  HDC dc = NULL;
  if (old_dc->cdc)
    dc = old_dc->cdc;
  else if (old_dc->canvas)
    dc = old_dc->canvas->GetHDC() ;

  cdc = ::CreateCompatibleDC(dc);
  ok = (cdc != NULL);

  if (!old_dc->cdc && old_dc->canvas)
    old_dc->canvas->ReleaseHDC() ;

  SetBrush(wxWHITE_BRUSH);
  SetPen(wxBLACK_PEN);
}

wxMemoryDC::~wxMemoryDC(void)
{
}

void wxMemoryDC::SelectObject(wxBitmap *bitmap)
{
  // Select old bitmap out of the device context
  if (old_bitmap)
  {
    ::SelectObject(cdc, old_bitmap);
    if (selected_bitmap)
    {
      selected_bitmap->selectedInto = NULL;
      selected_bitmap = NULL;
    }
  }

  if (!bitmap)
    return;

  // Do own check for whether the bitmap is already selected into
  // a device context
  if (bitmap->selectedInto && (bitmap->selectedInto != this))
  {
    wxFatalError("Error in wxMemoryDC::SelectObject\nBitmap is selected in another wxMemoryDC.\nDelete the first wxMemoryDC or use SelectObject(NULL)");
    return;
  }
  
  selected_bitmap = bitmap;
  bitmap->selectedInto = this;
#if DEBUG > 1
  wxDebugMsg("wxMemoryDC::SelectObject: Selecting HBITMAP %X\n", bitmap->ms_bitmap);
#endif
  HBITMAP bm = ::SelectObject(cdc, bitmap->ms_bitmap);

  if (bm == ERROR)
  {
    wxFatalError("Error in wxMemoryDC::SelectObject\nBitmap may not be loaded, or may be selected in another wxMemoryDC.\nDelete the first wxMemoryDC to deselect bitmap.");
  }
  else if (!old_bitmap)
    old_bitmap = bm;
}

void wxMemoryDC::GetSize(float *width, float *height)
{
  if (!selected_bitmap)
  {
    *width = 0.0; *height = 0.0;
    return;
  }
  *width = selected_bitmap->GetWidth();
  *height = selected_bitmap->GetHeight();
}

/*
 * Panel device context
 */
 
wxPanelDC::wxPanelDC(void)
{
  WXSET_TYPE(wxPanelDC, wxTYPE_DC_PANEL)
  
  panel = NULL;
}

wxPanelDC::wxPanelDC(wxPanel *the_panel):wxbPanelDC(the_panel)
   {
     panel = the_panel;
   }

wxPanelDC::~wxPanelDC(void)
   {
   }


// Create a DC representing the whole screen
wxScreenDC::wxScreenDC(void)
{
  WXSET_TYPE(wxScreenDC, wxTYPE_DC_CANVAS)
  
  cdc = ::GetDC(NULL);
}

wxScreenDC::~wxScreenDC(void)
{
  ::ReleaseDC(NULL, cdc);
}

