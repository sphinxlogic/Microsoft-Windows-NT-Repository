/*
 * File:      wb_ps.cc
 * Purpose:     Device context implementation (PostScript)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wb_ps.cc,v 1.5 1994/08/14 21:34:01 edz Exp $
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "@(#)wb_ps.cc	1.2 5/9/94"; */

#ifdef __GNUG__
#pragma implementation "wx_dcps.h"
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx_defs.h"

#if USE_POSTSCRIPT

#ifndef WX_PRECOMP
#include "wx_frame.h"
#include "wx_dcps.h"
#include "wx_utils.h"
#include "wx_dialg.h"
#include "wx_cmdlg.h"
#include "wx_main.h"
#include "wx_lbox.h"
#include "wx_rbox.h"
#include "wx_buttn.h"
#include "wx_choic.h"
#include "wx_check.h"
#include "wx_messg.h"
#include "wx_txt.h"
#include "wx_mtxt.h"
#endif

class wxCanvas;

#include "wx_dcmem.h"
#include "wx_privt.h"

// VC++ 4.2 and above use a different iostream scheme
#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#include <fstream.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
// #include <time.h>
#include <limits.h>
#include <assert.h>

// Declarations local to this file
#define YSCALE(y) (yorigin / scale_factor - (y))

Bool XPrinterDialog (wxWindow *parent);

// Determine the Default Postscript Previewer
// available on the platform
#if defined(sun) && defined(wx_xview)
// OpenWindow/NeWS's Postscript Previewer
# define PS_VIEWER_PROG "pageview"
#elif defined(VMS)
#define PS_VIEWER_PROG "view/format=ps/select=x_display"
#elif defined(__sgi)
// SGI's Display Postscript Previewer
//# define PS_VIEWER_PROG "dps"
# define PS_VIEWER_PROG "xpsview"
#elif defined(wx_x)
// Front-end to ghostscript 
# define PS_VIEWER_PROG "ghostview"
#else
// Windows ghostscript/ghostview
# define PS_VIEWER_PROG NULL
#endif

wxPrintSetupData *wxThePrintSetupData = NULL;

// these should move into wxPostscriptDC:
float UnderlinePosition = 0.0F;
float UnderlineThickness = 0.0F;

/*
static char *wx_preview_command = copystring(PS_VIEWER_PROG);
static Bool wx_printer_orientation = PS_PORTRAIT;
static int wx_printer_mode = PS_PREVIEW;

#ifdef VMS
static char *wx_printer_command = copystring("print");
static char *wx_printer_flags = copystring("/nonotify/queue=psqueue");
static char *wx_afm_path = copystring("sys$ps_font_metrics:");
#endif
#ifdef wx_msw
static char *wx_printer_command = copystring("print");
static char *wx_afm_path = copystring("c:\\windows\\system\\");
static char *wx_printer_flags = NULL;
#endif
#if !defined(VMS) && !defined(wx_msw)
static char *wx_printer_command = copystring("lpr");
static char *wx_printer_flags = NULL;
static char *wx_afm_path = NULL;
#endif
*/

#define _MAXPATHLEN 500

/* See "wxspline.cc" and "xfspline.cc" */
#if USE_XFIG_SPLINE_CODE
static const char *wxPostScriptHeaderSpline = " \
/DrawSplineSection {\n\
	/y3 exch def\n\
	/x3 exch def\n\
	/y2 exch def\n\
	/x2 exch def\n\
	/y1 exch def\n\
	/x1 exch def\n\
	/xa x1 x2 x1 sub 0.666667 mul add def\n\
	/ya y1 y2 y1 sub 0.666667 mul add def\n\
	/xb x3 x2 x3 sub 0.666667 mul add def\n\
	/yb y3 y2 y3 sub 0.666667 mul add def\n\
	x1 y1 lineto\n\
	xa ya xb yb x3 y3 curveto\n\
	} def\n\
";
#else
// No extra PS header for this spline implementation.
static const char *wxPostScriptHeaderSpline = NULL;

#endif /* USE_XFIG_SPLINE_CODE */

// steve, 05.09.94
// VMS has a bug in the ofstream class.
// the buffering doesn't work correctly. therefore
// we will allocate (temporarily) a very big buffer (1MB), so
// that a buffer overflow will not occur.
#ifdef VMS
#define VMS_BUFSIZ (1024L*1024L)
static char *fileBuffer = NULL;
#endif

wxPostScriptDC::wxPostScriptDC (void)
{
  WXSET_TYPE(wxPostScriptDC, wxTYPE_DC_POSTSCRIPT)
  
  wx_interactive = FALSE;
  font = NULL;
  device = wxDEVICE_EPS;
  clipping = FALSE;

  logical_origin_x = (float)0;
  logical_origin_y = (float)0;

  device_origin_x = (float)0;
  device_origin_y = (float)0;

  logical_scale_x = (float)1.0;
  logical_scale_y = (float)1.0;

  user_scale_x = (float)1.0;
  user_scale_y = (float)1.0;

  mapping_mode = MM_TEXT;

//  yorigin = 792;		            // For EPS output
  yorigin = 842;		            // For A4 output

  min_x = (float)1000.0;
  min_y = (float)1000.0;
  max_x = (float)-1000.0;
  max_y = (float)-1000.0;
  title = NULL;
  filename = NULL;

  pstream = NULL;

#ifdef wx_msw
  // Can only send to file in Windows
  wxThePrintSetupData->SetPrinterMode(PS_FILE);
#endif
  ok = FALSE;

  currentRed = 255;
  currentGreen = 255;
  currentBlue = 0;

  current_logical_function = -1;
/*
  current_pen = NULL;
  current_brush = NULL;
  current_background_brush = wxWHITE_BRUSH;
  current_text_foreground = *wxBLACK;
  Colour = wxColourDisplay ();
*/
}

wxPostScriptDC::wxPostScriptDC (char *file, Bool interactive, wxWindow *parent)
{
  Create(file, interactive, parent);
}

Bool wxPostScriptDC::Create(char *file, Bool interactive, wxWindow *parent)
{
  WXSET_TYPE(wxPostScriptDC, wxTYPE_DC_POSTSCRIPT)
  
  wx_interactive = interactive;
  font = NULL;
  device = wxDEVICE_EPS;
  clipping = FALSE;

  logical_origin_x = (float)0;
  logical_origin_y = (float)0;

  device_origin_x = (float)0;
  device_origin_y = (float)0;

  logical_scale_x = (float)1.0;
  logical_scale_y = (float)1.0;

  user_scale_x = (float)1.0;
  user_scale_y = (float)1.0;

  mapping_mode = MM_TEXT;

  yorigin = 792;		            // For EPS output
//  yorigin = 842;		            // For A4 output

  min_x = (float)1000.0;
  min_y = (float)1000.0;
  max_x = (float)-1000.0;
  max_y = (float)-1000.0;
  title = NULL;
  if (file)
    filename = copystring (file);
  else
    filename = NULL;

  pstream = NULL;

#ifdef wx_msw
  // Can only send to file in Windows
  wxThePrintSetupData->SetPrinterMode(PS_FILE);
#endif

  if (interactive)
  {
    if ((ok = PrinterDialog (parent) ) == FALSE) return FALSE;
  }
  else
    ok = TRUE;

  currentRed = 255;
  currentGreen = 255;
  currentBlue = 0;

  current_logical_function = -1;
  current_pen = NULL;
  current_brush = NULL;
  current_background_brush = wxWHITE_BRUSH;
  current_text_foreground = *wxBLACK;
  Colour = wxColourDisplay ();

  scale_factor = 1.0;
  
  return ok;
}

wxPostScriptDC::~wxPostScriptDC (void)
{
  if (pstream)
    delete pstream;
  if (filename)
    delete[]filename;
}

Bool wxPostScriptDC::PrinterDialog(wxWindow *parent)
{
  ok = XPrinterDialog (parent);
  if (!ok)
    return FALSE;

  if (!filename && (wxThePrintSetupData->GetPrinterMode() == PS_PREVIEW  || wxThePrintSetupData->GetPrinterMode() == PS_PRINTER))
    {
// steve, 05.09.94
#ifdef VMS
      wxThePrintSetupData->SetPrinterFile("preview");
#else
      // For PS_PRINTER action this depends on a Unix-style print spooler
      // since the wx_printer_file can be destroyed during a session
      // @@@ TODO: a Windows-style answer for non-Unix
      char userId[256];
      wxGetUserId (userId, sizeof (userId) / sizeof (char));
      char tmp[256];
      strcpy (tmp, "/tmp/preview_");
      strcat (tmp, userId);
      wxThePrintSetupData->SetPrinterFile(tmp);
#endif
      char tmp2[256];
      strcpy(tmp2, wxThePrintSetupData->GetPrinterFile());
      strcat (tmp2, ".ps");
      wxThePrintSetupData->SetPrinterFile(tmp2);
      filename = copystring(tmp2);
    }
    else if (!filename && (wxThePrintSetupData->GetPrinterMode() == PS_FILE))
    {
      char *file = wxSaveFileSelector ("PostScript", "ps");
      if (!file)
      {
        ok = FALSE;
        return FALSE;
      }
      wxThePrintSetupData->SetPrinterFile(file);
      filename = copystring(file);
      ok = TRUE;
    }

  return ok;
}

void wxPostScriptDC::SetClippingRegion (float cx, float cy, float cw, float ch)
{
  if (clipping)
    return;
  if (!pstream)
    return;

  clipping = TRUE;
  *pstream << "gsave\n";
  *pstream << "newpath\n";
  *pstream << cx << " " << YSCALE (cy) << " moveto\n";
  *pstream << cx + cw << " " << YSCALE (cy) << " lineto\n";
  *pstream << cx + cw << " " << YSCALE (cy + ch) << " lineto\n";
  *pstream << cx << " " << YSCALE (cy + ch) << " lineto\n";
  *pstream << "closepath clip newpath\n";
}

void wxPostScriptDC::DestroyClippingRegion (void)
{
  if (!pstream)
    return;
  if (clipping)
    {
      clipping = FALSE;
      *pstream << "grestore\n";
    }
}

void wxPostScriptDC::Clear (void)
{
}

void wxPostScriptDC::FloodFill (float WXUNUSED(x), float WXUNUSED(y), wxColour * WXUNUSED(col), int WXUNUSED(style))
{
}

Bool wxPostScriptDC::GetPixel (float WXUNUSED(x), float WXUNUSED(y), wxColour * WXUNUSED(col))
{
  return FALSE;
}

void wxPostScriptDC::IntDrawLine (int x1, int y1, int x2, int y2)
{
  DrawLine ((float) x1, (float) y1, (float) x2, (float) y2);
}

void wxPostScriptDC::CrossHair (int WXUNUSED(x), int WXUNUSED(y))
{
}

void wxPostScriptDC::DrawLine (float x1, float y1, float x2, float y2)
{
  if (!pstream)
    return;
  if (current_pen)
    SetPen (current_pen);
  *pstream << "newpath\n";
  *pstream << x1 << " " << YSCALE (y1) << " moveto\n";
  *pstream << x2 << " " << YSCALE (y2) << " lineto\n";
  *pstream << "stroke\n";
  CalcBoundingBox (x1, YSCALE (y1));
  CalcBoundingBox (x2, YSCALE (y2));
}

#define RAD2DEG 57.29577951308

void wxPostScriptDC::DrawArc (float x1, float y1, float x2, float y2, float xc, float yc)
{

    if (!pstream)
	return;

    double dx = x1 - xc;
    double dy = y1 - yc;
    double radius = sqrt(dx*dx+dy*dy);
    double alpha1, alpha2;

    if (x1 == x2 && y1 == y2) {
	alpha1 = 0.0;
	alpha2 = 360.0;
    } else if (radius == 0.0) {
	alpha1 = alpha2 = 0.0;
    } else {
	alpha1 = (x1 - xc == 0) ?
	    (y1 - yc < 0) ? 90.0 : -90.0 :
	    -atan2(double(y1-yc), double(x1-xc)) * RAD2DEG;
	alpha2 = (x2 - xc == 0) ?
	    (y2 - yc < 0) ? 90.0 : -90.0 :
	    -atan2(double(y2-yc), double(x2-xc)) * RAD2DEG;
    }
    while (alpha1 <= 0)   alpha1 += 360;
    while (alpha2 <= 0)   alpha2 += 360; // adjust angles to be between
    while (alpha1 > 360)  alpha1 -= 360; // 0 and 360 degree
    while (alpha2 > 360)  alpha2 -= 360;

    if (current_brush && current_brush->GetStyle() != wxTRANSPARENT) {
	SetBrush(current_brush);
	*pstream << "newpath\n"
	         << xc      << " " << YSCALE(yc) << " "
	         << radius  << " " << radius     << " "
		 << alpha1  << " " << alpha2     << " ellipse\n"
		 << xc      << " " << YSCALE(yc) << " lineto\n"
		 << "closepath\n"
	         << "fill\n";
    }
    if (current_pen && current_pen->GetStyle() != wxTRANSPARENT) {
	SetPen(current_pen);
	*pstream << "newpath\n"
	         << xc      << " " << YSCALE(yc) << " "
	         << radius  << " " << radius     << " "
		 << alpha1  << " " << alpha2     << " ellipse\n"
	         << "stroke\n";
    }
    CalcBoundingBox(x1, YSCALE(y1));
    CalcBoundingBox(x2, YSCALE(y2));
}

void wxPostScriptDC::DrawEllipticArc(float x,float y,float w,float h,float sa,float ea)
{
  if (!pstream)
    return;

  if (sa>=360 || sa<=-360) sa=sa-int(sa/360)*360;
  if (ea>=360 || ea<=-360) ea=ea-int(ea/360)*360;
  if (sa<0) sa+=360;
  if (ea<0) ea+=360;
  if (sa==ea)
  {
    DrawEllipse(x,y,w,h);
    return;
  }

  if (current_brush && current_brush->GetStyle () != wxTRANSPARENT)
    {
      SetBrush (current_brush);

      *pstream << 
         "newpath\n" <<
         x+w/2 << " " << YSCALE (y+h/2) << " " <<
         w/2 << " " << h/2 << " " <<
         int(sa) <<" "<< int(ea)<<" true ellipticarc\n";

      CalcBoundingBox (x , YSCALE (y ));
      CalcBoundingBox (x+w,YSCALE(y+h));
    }
  if (current_pen && current_pen->GetStyle () != wxTRANSPARENT)
    {
      SetPen (current_pen);

      *pstream << 
         "newpath\n" <<
         x+w/2 << " " << YSCALE (y+h/2) << " " <<
         w/2 << " " << h/2 << " " <<
         int(sa) <<" "<< int(ea)<<" false ellipticarc\n";

      CalcBoundingBox (x , YSCALE (y ));
      CalcBoundingBox (x+w,YSCALE(y+h));
    }
}

void wxPostScriptDC::DrawPoint (float x, float y)
{
  if (!pstream)
    return;
  if (current_pen)
    SetPen (current_pen);
  *pstream << "newpath\n";
  *pstream << x << " " << YSCALE (y) << " moveto\n";
  *pstream << (x+1) << " " << YSCALE (y) << " lineto\n";
  *pstream << "stroke\n";
  CalcBoundingBox (x, YSCALE (y));
}

void wxPostScriptDC::DrawPolygon (int n, wxPoint points[], float xoffset, float yoffset, int WXUNUSED(fillStyle))
{
  if (!pstream)
    return;
  if (n > 0)
    {
      if (current_brush && current_brush->GetStyle () != wxTRANSPARENT)
	{
	  SetBrush (current_brush);
	  *pstream << "newpath\n";

	  float xx = points[0].x + xoffset;
	  float yy = YSCALE (points[0].y + yoffset);
	  *pstream << xx << " " << yy << " moveto\n";
	  CalcBoundingBox (xx, yy);

	  int i;
	  for (i = 1; i < n; i++)
	    {
	      xx = points[i].x + xoffset;
	      yy = YSCALE (points[i].y + yoffset);
	      *pstream << xx << " " << yy << " lineto\n";
	      CalcBoundingBox (xx, yy);
	    }
	  *pstream << "fill\n";
	}

      if (current_pen && current_pen->GetStyle () != wxTRANSPARENT)
	{
	  SetPen (current_pen);
	  *pstream << "newpath\n";

	  float xx = points[0].x + xoffset;
	  float yy = YSCALE (points[0].y + yoffset);
	  *pstream << xx << " " << yy << " moveto\n";
	  CalcBoundingBox (xx, yy);

	  int i;
	  for (i = 1; i < n; i++)
	    {
	      xx = points[i].x + xoffset;
	      yy = YSCALE (points[i].y + yoffset);
	      *pstream << xx << " " << yy << " lineto\n";
	      CalcBoundingBox (xx, yy);
	    }

	  // Close the polygon
	  xx = points[0].x + xoffset;
	  yy = YSCALE (points[0].y + yoffset);
	  *pstream << xx << " " << yy << " lineto\n";

	  // Output the line
	  *pstream << "stroke\n";
	}
    }
}

void wxPostScriptDC::DrawLines (int n, wxIntPoint points[], int xoffset, int yoffset)
{
  if (!pstream)
    return;
  if (n > 0)
    {
      if (current_pen)
	SetPen (current_pen);

      *pstream << "newpath\n";

      float xx = (float) (points[0].x + xoffset);
      float yy = (float) (YSCALE (points[0].y + yoffset));
      *pstream << xx << " " << yy << " moveto\n";
      CalcBoundingBox (xx, yy);

      int i;
      for (i = 1; i < n; i++)
	{
	  xx = (float) (points[i].x + xoffset);
	  yy = (float) (YSCALE (points[i].y + yoffset));
	  *pstream << xx << " " << yy << " lineto\n";
	  CalcBoundingBox (xx, yy);
	}
      *pstream << "stroke\n";
    }
}

void wxPostScriptDC::DrawLines (int n, wxPoint points[], float xoffset, float yoffset)
{
  if (!pstream)
    return;
  if (n > 0)
    {
      if (current_pen)
	SetPen (current_pen);

      *pstream << "newpath\n";

      float xx = points[0].x + xoffset;
      float yy = YSCALE (points[0].y + yoffset);
      *pstream << xx << " " << yy << " moveto\n";
      CalcBoundingBox (xx, yy);

      int i;
      for (i = 1; i < n; i++)
	{
	  xx = points[i].x + xoffset;
	  yy = YSCALE (points[i].y + yoffset);
	  *pstream << xx << " " << yy << " lineto\n";
	  CalcBoundingBox (xx, yy);
	}
      *pstream << "stroke\n";
    }
}

void wxPostScriptDC::DrawRectangle (float x, float y, float width, float height)
{
  if (!pstream)
    return;
  if (current_brush && current_brush->GetStyle () != wxTRANSPARENT)
    {
      SetBrush (current_brush);

      *pstream << "newpath\n";
      *pstream << x << " " << YSCALE (y) << " moveto\n";
      *pstream << x + width << " " << YSCALE (y) << " lineto\n";
      *pstream << x + width << " " << YSCALE (y + height) << " lineto\n";
      *pstream << x << " " << YSCALE (y + height) << " lineto\n";
      *pstream << "closepath\n";
      *pstream << "fill\n";

      CalcBoundingBox (x, YSCALE (y));
      CalcBoundingBox (x + width, YSCALE (y + height));
    }
  if (current_pen && current_pen->GetStyle () != wxTRANSPARENT)
    {
      SetPen (current_pen);

      *pstream << "newpath\n";
      *pstream << x << " " << YSCALE (y) << " moveto\n";
      *pstream << x + width << " " << YSCALE (y) << " lineto\n";
      *pstream << x + width << " " << YSCALE (y + height) << " lineto\n";
      *pstream << x << " " << YSCALE (y + height) << " lineto\n";
      *pstream << "closepath\n";
      *pstream << "stroke\n";

      CalcBoundingBox (x, YSCALE (y));
      CalcBoundingBox (x + width, YSCALE (y + height));
    }
}

void wxPostScriptDC::DrawRoundedRectangle (float x, float y, float width, float height, float radius)
{
  if (!pstream)
    return;

  if (radius < 0.0)
    {
      // Now, a negative radius is interpreted to mean
      // 'the proportion of the smallest X or Y dimension'
      float smallest = (float)0.0;
      if (width < height)
	smallest = width;
      else
	smallest = height;
      radius = (float) (-radius * smallest);
    }

  if (current_brush && current_brush->GetStyle () != wxTRANSPARENT)
    {
      SetBrush (current_brush);
      // Draw rectangle anticlockwise
      *pstream << "newpath\n";
      *pstream << x + radius << " " << YSCALE (y + radius) << " " << radius << " 90 180 arc\n";

      *pstream << x << " " << YSCALE (y + radius) << " moveto\n";

      *pstream << x + radius << " " << YSCALE (y + height - radius) << " " << radius << " 180 270 arc\n";
      *pstream << x + width - radius << " " << YSCALE (y + height) << " lineto\n";

      *pstream << x + width - radius << " " << YSCALE (y + height - radius) << " " << radius << " 270 0 arc\n";
      *pstream << x + width << " " << YSCALE (y + radius) << " lineto\n";

      *pstream << x + width - radius << " " << YSCALE (y + radius) << " " << radius << " 0 90 arc\n";

      *pstream << x + radius << " " << YSCALE (y) << " lineto\n";

      *pstream << "closepath\n";

      *pstream << "fill\n";

      CalcBoundingBox (x, YSCALE (y));
      CalcBoundingBox (x + width, YSCALE (y + height));
    }
  if (current_pen && current_pen->GetStyle () != wxTRANSPARENT)
    {
      SetPen (current_pen);
      // Draw rectangle anticlockwise
      *pstream << "newpath\n";
      *pstream << x + radius << " " << YSCALE (y + radius) << " " << radius << " 90 180 arc\n";

      *pstream << x << " " << YSCALE (y + height - radius) << " lineto\n";

      *pstream << x + radius << " " << YSCALE (y + height - radius) << " " << radius << " 180 270 arc\n";
      *pstream << x + width - radius << " " << YSCALE (y + height) << " lineto\n";

      *pstream << x + width - radius << " " << YSCALE (y + height - radius) << " " << radius << " 270 0 arc\n";
      *pstream << x + width << " " << YSCALE (y + radius) << " lineto\n";

      *pstream << x + width - radius << " " << YSCALE (y + radius) << " " << radius << " 0 90 arc\n";

      *pstream << x + radius << " " << YSCALE (y) << " lineto\n";

      *pstream << "closepath\n";

      *pstream << "stroke\n";

      CalcBoundingBox (x, YSCALE (y));
      CalcBoundingBox (x + width, YSCALE (y + height));
    }
}

void wxPostScriptDC::DrawEllipse (float x, float y, float width, float height)
{
  if (!pstream)
    return;
  if (current_brush && current_brush->GetStyle () != wxTRANSPARENT)
    {
      SetBrush (current_brush);

      *pstream << "newpath\n";
      *pstream << x + width / 2 << " " << YSCALE (y + height / 2) << " ";
      *pstream << width / 2 << " " << height / 2 << " 0 360 ellipse\n";
      *pstream << "fill\n";

      CalcBoundingBox (x - width, YSCALE (y - height));
      CalcBoundingBox (x + width, YSCALE (y + height));
    }
  if (current_pen && current_pen->GetStyle () != wxTRANSPARENT)
    {
      SetPen (current_pen);

      *pstream << "newpath\n";
      *pstream << x + width / 2 << " " << YSCALE (y + height / 2) << " ";
      *pstream << width / 2 << " " << height / 2 << " 0 360 ellipse\n";
      *pstream << "stroke\n";

      CalcBoundingBox (x - width, YSCALE (y - height));
      CalcBoundingBox (x + width, YSCALE (y + height));
    }
}

void wxPostScriptDC::DrawIcon (wxIcon * icon, float x, float y)
{
#ifdef wx_x
  wxMemoryDC memDC;
  memDC.SelectObject(icon);
  Blit(x, y, float(icon->GetWidth()), float(icon->GetHeight()), &memDC, 0.0, 0.0);
#endif
}

void wxPostScriptDC::SetFont (wxFont * the_font)
{

    if (!pstream)
    return;
  if (font == the_font)
    return;

  font = the_font;

#ifdef wx_x
  /* MATTHEW: Use wxTheFontNameDirectory */
  char *name;
  int Family = font->GetFamily ();
  int Style = font->GetStyle ();
  int Weight = font->GetWeight ();

  name = wxTheFontNameDirectory.GetPostScriptName(Family, Weight, Style);
  if (!name)
      name = "Times-Roman";

  *pstream << "/" << name << " reencodeISO def\n";
  *pstream << "/" << name << " findfont\n";
  *pstream << font->GetPointSize () << " scalefont setfont\n";
#else
  char buf[100];
  char *name;
  char *style = "";
  int Style = font->GetStyle ();
  int Weight = font->GetWeight ();

  switch (font->GetFamily ())
    {
    case wxTELETYPE:
    case wxMODERN:
      name = "/Courier";
      break;
    case wxSWISS:
      name = "/Helvetica";
      break;
    case wxROMAN:
//      name = "/Times-Roman";
      name = "/Times"; // Altered by EDZ
      break;
    case wxSCRIPT:
      name = "/Zapf-Chancery-MediumItalic";
      Style  = wxNORMAL;
      Weight = wxNORMAL;
      break;
    default:
    case wxDEFAULT: // Sans Serif Font
      name = "/LucidaSans";
    }

  if (Style == wxNORMAL && (Weight == wxNORMAL || Weight == wxLIGHT))
    {
      if (font->GetFamily () == wxROMAN)
	style = "-Roman";
      else
	style = "";
    }
  else if (Style == wxNORMAL && Weight == wxBOLD)
    style = "-Bold";

  else if (Style == wxITALIC && (Weight == wxNORMAL || Weight == wxLIGHT))
    {
      if (font->GetFamily () == wxROMAN)
	style = "-Italic";
      else
	style = "-Oblique";
    }
  else if (Style == wxITALIC && Weight == wxBOLD)
    {
      if (font->GetFamily () == wxROMAN)
	style = "-BoldItalic";
      else
	style = "-BoldOblique";
    }
  else if (Style == wxSLANT && (Weight == wxNORMAL || Weight == wxLIGHT))
    {
      if (font->GetFamily () == wxROMAN)
	style = "-Italic";
      else
	style = "-Oblique";
    }
  else if (Style == wxSLANT && Weight == wxBOLD)
    {
      if (font->GetFamily () == wxROMAN)
	style = "-BoldItalic";
      else
	style = "-BoldOblique";
    }
  else
    style = "";

  strcpy (buf, name);
  strcat (buf, style);
  *pstream << buf << " findfont\n";
  *pstream << font->GetPointSize() * scale_factor << " scalefont setfont\n";
#endif
}

void wxPostScriptDC::SetPen (wxPen * pen)
{
  if (!pstream)
    return;
  wxPen *oldPen = current_pen;

  if ((current_pen = pen) == NULL)
    return;			/* NIL */

  // Line width
  *pstream << pen->GetWidth () << " setlinewidth\n";

  // Line style - WRONG: 2nd arg is OFFSET
  /*
     Here, I'm afraid you do not conceive meaning of parameters of 'setdash'
     operator correctly. You should look-up this in the Red Book: the 2nd parame-
     ter is not number of values in the array of the first one, but an offset
     into this description of the pattern. I mean a real *offset* not index
     into array. I.e. If the command is [3 4] 1 setdash   is used, then there
     will be first black line *2* units long, then space 4 units, then the
     pattern of *3* units black, 4 units space will be repeated.
   */
  static char *dotted = "[2 5] 2";
  static char *short_dashed = "[4 4] 2";
  static char *long_dashed = "[4 8] 2";
  static char *dotted_dashed = "[6 6 2 6] 4";

  char *psdash = NULL;
  switch (pen->GetStyle ())
    {
    case wxDOT:
      psdash = dotted;
      break;
    case wxSHORT_DASH:
      psdash = short_dashed;
      break;
    case wxLONG_DASH:
      psdash = long_dashed;
      break;
    case wxDOT_DASH:
      psdash = dotted_dashed;
      break;
    case wxSOLID:
    case wxTRANSPARENT:
    default:
      psdash = "[] 0";
      break;
    }
  if (oldPen != pen)
    *pstream << psdash << " setdash\n";

  // Line colour
  unsigned char red = pen->GetColour ().Red ();
  unsigned char blue = pen->GetColour ().Blue ();
  unsigned char green = pen->GetColour ().Green ();

  if (!Colour)
    {
      // Anything not white is black
      if (!(red == (unsigned char) 255 && blue == (unsigned char) 255
	    && green == (unsigned char) 255))
	{
	  red = (unsigned char) 0;
	  green = (unsigned char) 0;
	  blue = (unsigned char) 0;
	}
    }

  if (!(red == currentRed && green == currentGreen && blue == currentBlue))
  {
    float redPS = (float) (((int) red) / 255.0);
    float bluePS = (float) (((int) blue) / 255.0);
    float greenPS = (float) (((int) green) / 255.0);

    *pstream << redPS << " " << greenPS << " " << bluePS << " setrgbcolor\n";
    
    currentRed = red;
    currentBlue = blue;
    currentGreen = green;
  }
}

void wxPostScriptDC::SetBrush (wxBrush * brush)
{
  if (!pstream)
    return;
//  wxBrush *oldBrush = current_brush;

  if ((current_brush = brush) == NULL)
    return;			/* NIL */

  // Brush colour
  unsigned char red = brush->GetColour ().Red ();
  unsigned char blue = brush->GetColour ().Blue ();
  unsigned char green = brush->GetColour ().Green ();

  if (!Colour)
    {
      // Anything not black is white
      if (!(red == (unsigned char) 0 && blue == (unsigned char) 0
	    && green == (unsigned char) 0))
	{
	  red = (unsigned char) 255;
	  green = (unsigned char) 255;
	  blue = (unsigned char) 255;
	}
    }

  if (!(red == currentRed && green == currentGreen && blue == currentBlue))
  {
    float redPS = (float) (((int) red) / 255.0);
    float bluePS = (float) (((int) blue) / 255.0);
    float greenPS = (float) (((int) green) / 255.0);
    *pstream << redPS << " " << greenPS << " " << bluePS << " setrgbcolor\n";
    currentRed = red;
    currentBlue = blue;
    currentGreen = green;
  }
}

void wxPostScriptDC::DrawText (const char *text, float x, float y, Bool WXUNUSED(use16bit))
{
  if (!pstream)
    return;
  if (font)
    SetFont (font);

  if (current_text_foreground.Ok ())
    {
      unsigned char red = current_text_foreground.Red ();
      unsigned char blue = current_text_foreground.Blue ();
      unsigned char green = current_text_foreground.Green ();

      if (!Colour)
	{
	  // Anything not white is black
	  if (!(red == (unsigned char) 255 && blue == (unsigned char) 255
		&& green == (unsigned char) 255))
	    {
	      red = (unsigned char) 0;
	      green = (unsigned char) 0;
	      blue = (unsigned char) 0;
	    }
	}
      if (!(red == currentRed && green == currentGreen && blue == currentBlue))
      {
        float redPS = (float) (((int) red) / 255.0);
        float bluePS = (float) (((int) blue) / 255.0);
        float greenPS = (float) (((int) green) / 255.0);
        *pstream << redPS << " " << greenPS << " " << bluePS << " setrgbcolor\n";

        currentRed = red;
        currentBlue = blue;
        currentGreen = green;
      }
    }

  int size = 10;
  if (font)
    size = font->GetPointSize ();

  *pstream << x << " " << YSCALE (y + size) << " moveto\n";

//  *pstream << "(" << text << ")" << " show\n";
  *pstream << "(";
  int len = strlen (text);
  int i;
  for (i = 0; i < len; i++)
    {
/*
      char ch = text[i];
      if (ch == ')' || ch == '(' || ch == '\\')
	*pstream << "\\";
      *pstream << ch;
*/
      int c = (unsigned char) text[i];
      if ( c == ')' || c == '(' || c == '\\')
      {
        *pstream << "\\" << (char) c;
      }
      else if ( c >= 128 )
      {
        // Cope with character codes > 127
        char tmp[5];
        sprintf(tmp, "\\%o", c);
        *pstream << tmp;
      }
      else
        *pstream << (char) c;
    }

  *pstream << ")" << " show\n";

  if (font->GetUnderlined())
  {
      float w, h;
      GetTextExtent(text, &w, &h);
      *pstream << "gsave " << x << " " << YSCALE (y + size - UnderlinePosition)
               << " moveto\n"
               << UnderlineThickness << " setlinewidth "
               << x + w << " " << YSCALE (y + size - UnderlinePosition)
               << " lineto stroke grestore\n";
  }
  
  CalcBoundingBox (x, YSCALE (y + size));
  CalcBoundingBox (x + size * strlen (text), YSCALE (y));
}


void wxPostScriptDC::SetBackground (wxBrush * brush)
{
  current_background_brush = brush;
}

void wxPostScriptDC::SetLogicalFunction (int WXUNUSED(function))
{
}

static const char *wxPostScriptHeaderEllipse = "\
/ellipsedict 8 dict def\n\
ellipsedict /mtrx matrix put\n\
/ellipse\n\
{ ellipsedict begin\n\
  /endangle exch def\n\
  /startangle exch def\n\
  /yrad exch def\n\
  /xrad exch def\n\
  /y exch def\n\
  /x exch def\n\
  /savematrix mtrx currentmatrix def\n\
  x y translate\n\
  xrad yrad scale\n\
  0 0 1 startangle endangle arc\n\
  savematrix setmatrix\n\
  end\n\
  } def\n\
";

static const char *wxPostScriptHeaderEllipticArc= "\
/ellipticarcdict 8 dict def\n\
ellipticarcdict /mtrx matrix put\n\
/ellipticarc\n\
{ ellipticarcdict begin\n\
  /do_fill exch def\n\
  /endangle exch def\n\
  /startangle exch def\n\
  /yrad exch def\n\
  /xrad exch def \n\
  /y exch def\n\
  /x exch def\n\
  /savematrix mtrx currentmatrix def\n\
  x y translate\n\
  xrad yrad scale\n\
  do_fill { 0 0 moveto } if\n\
  0 0 1 startangle endangle arc\n\
  savematrix setmatrix\n\
  do_fill { fill }{ stroke } ifelse\n\
  end\n\
} def\n";

Bool wxPostScriptDC::StartDoc (char *message)
{
  if (device == wxDEVICE_EPS)
    {
      if (!filename)
	{
/*
	  char *file = wxSaveFileSelector ("PostScript", "ps");
	  if (!file)
	    {
	      ok = FALSE;
	      return FALSE;
	    }
*/
#ifdef VMS
          filename = copystring("wxtmp.ps");
#else
          filename = copystring(wxGetTempFileName("ps"));
#endif
	  wxThePrintSetupData->SetPrinterFile(filename);
	  ok = TRUE;
	}
      else
	wxThePrintSetupData->SetPrinterFile(filename);

#ifdef VMS
      // steve, 05.09.94
      // VMS is sh*t!
      pstream = new ofstream;
      if(fileBuffer) delete[] fileBuffer;
      fileBuffer = new char[VMS_BUFSIZ]; 
      pstream->setbuf(fileBuffer,VMS_BUFSIZ);
      pstream->open(wxThePrintSetupData->GetPrinterFile());
#else
      pstream = new ofstream (wxThePrintSetupData->GetPrinterFile());
#endif
      if (!pstream || !pstream->good())
	{
	  wxMessageBox (wxSTR_ERROR, "Cannot open file!", wxOK);
	  ok = FALSE;
	  return FALSE;
	}
      ok = TRUE;
    }

  SetBrush (wxBLACK_BRUSH);
  SetPen (wxBLACK_PEN);

  wxPageNumber = 1;
  if (message)
    title = copystring (message);
  return TRUE;
}


void wxPostScriptDC::EndDoc (void)
{
    static char wxPostScriptHeaderReencodeISO1[] =
    "\n/reencodeISO {\n"
"dup dup findfont dup length dict begin\n"
"{ 1 index /FID ne { def }{ pop pop } ifelse } forall\n"
"/Encoding ISOLatin1Encoding def\n"
"currentdict end definefont\n"
"} def\n"
"/ISOLatin1Encoding [\n"
"/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
"/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
"/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
"/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
"/space/exclam/quotedbl/numbersign/dollar/percent/ampersand/quoteright\n"
"/parenleft/parenright/asterisk/plus/comma/minus/period/slash\n"
"/zero/one/two/three/four/five/six/seven/eight/nine/colon/semicolon\n"
"/less/equal/greater/question/at/A/B/C/D/E/F/G/H/I/J/K/L/M/N\n"
"/O/P/Q/R/S/T/U/V/W/X/Y/Z/bracketleft/backslash/bracketright\n"
"/asciicircum/underscore/quoteleft/a/b/c/d/e/f/g/h/i/j/k/l/m\n"
"/n/o/p/q/r/s/t/u/v/w/x/y/z/braceleft/bar/braceright/asciitilde\n"
"/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
"/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n";

static char wxPostScriptHeaderReencodeISO2[] =
"/.notdef/dotlessi/grave/acute/circumflex/tilde/macron/breve\n"
"/dotaccent/dieresis/.notdef/ring/cedilla/.notdef/hungarumlaut\n"
"/ogonek/caron/space/exclamdown/cent/sterling/currency/yen/brokenbar\n"
"/section/dieresis/copyright/ordfeminine/guillemotleft/logicalnot\n"
"/hyphen/registered/macron/degree/plusminus/twosuperior/threesuperior\n"
"/acute/mu/paragraph/periodcentered/cedilla/onesuperior/ordmasculine\n"
"/guillemotright/onequarter/onehalf/threequarters/questiondown\n"
"/Agrave/Aacute/Acircumflex/Atilde/Adieresis/Aring/AE/Ccedilla\n"
"/Egrave/Eacute/Ecircumflex/Edieresis/Igrave/Iacute/Icircumflex\n"
"/Idieresis/Eth/Ntilde/Ograve/Oacute/Ocircumflex/Otilde/Odieresis\n"
"/multiply/Oslash/Ugrave/Uacute/Ucircumflex/Udieresis/Yacute\n"
"/Thorn/germandbls/agrave/aacute/acircumflex/atilde/adieresis\n"
"/aring/ae/ccedilla/egrave/eacute/ecircumflex/edieresis/igrave\n"
"/iacute/icircumflex/idieresis/eth/ntilde/ograve/oacute/ocircumflex\n"
"/otilde/odieresis/divide/oslash/ugrave/uacute/ucircumflex/udieresis\n"
"/yacute/thorn/ydieresis\n"
        "] def\n\n";

    if (!pstream)
    return;
  if (clipping)
    {
      clipping = FALSE;
      *pstream << "grestore\n";
    }

  // Will reuse pstream for header
#ifdef VMS
  // see the definition of fileBuffer for explanation
  pstream->close(); // steve, 05.09.94
  if(fileBuffer) delete[] fileBuffer;
#endif
  if (pstream)
    {
      delete pstream;
      pstream = NULL;
    }

  // Write header now
// steve, 05.09.94
#ifdef VMS
  char *header_file = "header.ps";
#else
  char *header_file = wxGetTempFileName("ps");
#endif
  pstream = new ofstream (header_file);

  *pstream << "%!PS-Adobe-2.0\n";	/* PostScript magic strings */
  if (title)
    *pstream << "%%Title: " << title << "\n";
  *pstream << "%%Creator: " << wxTheApp->argv[0] << "\n";
//  time_t when; time (&when);
//  *pstream << "%%CreationDate: " << ctime (&when);
  *pstream << "%%CreationDate: " << wxNow() << "\n";

  // User Id information
  char userID[256];
  if ( wxGetEmailAddress(userID, sizeof(userID)) )
  {
    *pstream << "%%For: " << (char *)userID;
    char userName[245];
    if (wxGetUserName(userName, sizeof(userName)))
      *pstream << " (" << (char *)userName << ")";
    *pstream << "\n";
  }
  else if ( wxGetUserName(userID, sizeof(userID)) )
  {
    *pstream << "%%For: " << (char *)userID << "\n";
  }

  // THE FOLLOWING HAS BEEN CONTRIBUTED BY Andy Fyfe <andy@hyperparallel.com>

  float wx_printer_translate_x, wx_printer_translate_y;
  float wx_printer_scale_x, wx_printer_scale_y;
  wxThePrintSetupData->GetPrinterTranslation(&wx_printer_translate_x, &wx_printer_translate_y);
  wxThePrintSetupData->GetPrinterScaling(&wx_printer_scale_x, &wx_printer_scale_y);

  // Compute the bounding box.  Note that it is in the default user
  // coordinate system, thus we have to convert the values.
  float llx = (min_x+wx_printer_translate_x)*wx_printer_scale_x;
  float lly = (min_y+wx_printer_translate_y)*wx_printer_scale_y;
  float urx = (max_x+wx_printer_translate_x)*wx_printer_scale_x;
  float ury = (max_y+wx_printer_translate_y)*wx_printer_scale_y;

  // If we're landscape, our sense of "x" and "y" is reversed.
  if (wxThePrintSetupData->GetPrinterOrientation() == PS_LANDSCAPE)
    {
      float tmp;
      tmp = llx; llx = lly; lly = tmp;
      tmp = urx; urx = ury; ury = tmp;

      // We need either the two lines that follow, or we need to subtract
      // min_x from real_translate_y, which is commented out below.
      llx = llx - min_x*wx_printer_scale_y;
      urx = urx - min_x*wx_printer_scale_y;
    }

  // The Adobe specifications call for integers; we round as to make
  // the bounding larger.
  *pstream << "%%BoundingBox: "
      << floor(llx) << " " << floor(lly) << " "
      << ceil(urx)  << " " << ceil(ury)  << "\n";
  *pstream << "%%Pages: " << wxPageNumber - 1 << "\n";
  *pstream << "%%EndComments\n\n";

  // To check the correctness of the bounding box, postscript commands
  // to draw a box corresponding to the bounding box are generated below.
  // But since we typically don't want to print such a box, the postscript
  // commands are generated within comments.  These lines appear before any
  // adjustment of scale, rotation, or translation, and hence are in the
  // default user coordinates.
  *pstream << "% newpath\n";
  *pstream << "% " << llx << " " << lly << " moveto\n";
  *pstream << "% " << urx << " " << lly << " lineto\n";
  *pstream << "% " << urx << " " << ury << " lineto\n";
  *pstream << "% " << llx << " " << ury << " lineto closepath stroke\n";

  // Output scaling
  float real_translate_y = wx_printer_translate_y;
  if (wxThePrintSetupData->GetPrinterOrientation() == PS_LANDSCAPE)
    {
      real_translate_y -= max_y;
      // The following line can be used instead of the adjustment to 
      // llx and urx above.
      // real_translate_y -= min_x;
      *pstream << "90 rotate\n";
    }

/* Probably don't want this now we have it in EndPage, below.
 * We should rationalise the scaling code to one place. JACS, October 1995
 * Do we take the next 2 lines out or not?
 */
 
  *pstream << wx_printer_scale_x << " " << wx_printer_scale_y << " scale\n";
  *pstream << wx_printer_translate_x << " " << real_translate_y << " translate\n";

  *pstream << "%%BeginProlog\n";
  *pstream << wxPostScriptHeaderEllipse;
  *pstream << wxPostScriptHeaderEllipticArc;
  *pstream << wxPostScriptHeaderReencodeISO1;
  *pstream << wxPostScriptHeaderReencodeISO2;
  
  if (wxPostScriptHeaderSpline)
    *pstream << wxPostScriptHeaderSpline;
  *pstream << "%%EndProlog\n";

  delete pstream;
  pstream = NULL;

#ifdef VMS
  char *tmp_file = "tmp.ps";
#else
  char *tmp_file = wxGetTempFileName("ps");
#endif

  // Paste header Before wx_printer_file
  wxConcatFiles (header_file, wxThePrintSetupData->GetPrinterFile(), tmp_file);
  wxRemoveFile (header_file);
  wxRemoveFile (wxThePrintSetupData->GetPrinterFile());
  wxRenameFile(tmp_file, wxThePrintSetupData->GetPrinterFile());

#ifdef wx_x
  if (ok) // && wx_interactive)
    {
      switch (wxThePrintSetupData->GetPrinterMode()) {
	case PS_PREVIEW:
	{
          char *argv[3];
          argv[0] = wxThePrintSetupData->GetPrintPreviewCommand();
          argv[1] = wxThePrintSetupData->GetPrinterFile();
          argv[2] = NULL;
	  wxExecute (argv, TRUE);
          wxRemoveFile(wxThePrintSetupData->GetPrinterFile());
	}
	break;

	case PS_PRINTER:
	{
          char *argv[4];
          int argc = 0;
          argv[argc++] = wxThePrintSetupData->GetPrinterCommand();

          // !SM! If we simply assign to argv[1] here, if printer options
          // are blank, we get an annoying and confusing message from lpr.
          char * opts = wxThePrintSetupData->GetPrinterOptions();
          if (opts && *opts)
              argv[argc++] = opts;
              
          argv[argc++] = wxThePrintSetupData->GetPrinterFile();
          argv[argc++] = NULL;
	  wxExecute (argv, TRUE);
          wxRemoveFile(wxThePrintSetupData->GetPrinterFile());
	}
	break;

	case PS_FILE:
	  break;
	}
    }
#endif
}

void wxPostScriptDC::StartPage (void)
{
  if (!pstream)
    return;
  *pstream << "%%Page: " << wxPageNumber++ << "\n";
  *pstream << "matrix currentmatrix\n";
}

void wxPostScriptDC::EndPage (void)
{
  if (!pstream)
    return;
  *pstream << "showpage\n";

  *pstream << "setmatrix\n";
  
  // THE FOLLOWING HAS BEEN CONTRIBUTED BY Andy Fyfe <andy@hyperparallel.com>

  float wx_printer_translate_x, wx_printer_translate_y;
  float wx_printer_scale_x, wx_printer_scale_y;
  wxThePrintSetupData->GetPrinterTranslation(&wx_printer_translate_x, &wx_printer_translate_y);
  wxThePrintSetupData->GetPrinterScaling(&wx_printer_scale_x, &wx_printer_scale_y);

  // Compute the bounding box.  Note that it is in the default user
  // coordinate system, thus we have to convert the values.
  float llx = (min_x+wx_printer_translate_x)*wx_printer_scale_x;
  float lly = (min_y+wx_printer_translate_y)*wx_printer_scale_y;
  float urx = (max_x+wx_printer_translate_x)*wx_printer_scale_x;
  float ury = (max_y+wx_printer_translate_y)*wx_printer_scale_y;

  // If we're landscape, our sense of "x" and "y" is reversed.
  if (wxThePrintSetupData->GetPrinterOrientation() == PS_LANDSCAPE)
    {
      float tmp;
      tmp = llx; llx = lly; lly = tmp;
      tmp = urx; urx = ury; ury = tmp;

      // We need either the two lines that follow, or we need to subtract
      // min_x from real_translate_y, which is commented out below.
      llx = llx - min_x*wx_printer_scale_y;
      urx = urx - min_x*wx_printer_scale_y;
    }

  // Output scaling
  float real_translate_y = wx_printer_translate_y;
  if (wxThePrintSetupData->GetPrinterOrientation() == PS_LANDSCAPE)
    {
      real_translate_y -= max_y;
      // The following line can be used instead of the adjustment to 
      // llx and urx above.
      // real_translate_y -= min_x;
      *pstream << "90 rotate\n";
    }

  *pstream << wx_printer_scale_x << " " << wx_printer_scale_y << " scale\n";
  *pstream << wx_printer_translate_x << " " << real_translate_y << " translate\n";
}

/* MATTHEW: Implement Blit: */
/* MATTHEW: [4] Re-wrote to use colormap */
Bool wxPostScriptDC::
Blit (float xdest, float ydest, float fwidth, float fheight,
      wxCanvasDC * source, float xsrc, float ysrc, int WXUNUSED(rop), Bool WXUNUSED(useMask))
{
  long width, height, x, y;

#ifndef wx_x
  return FALSE;
#endif

  width = (long)floor(fwidth);
  height = (long)floor(fheight);
  x = (long)floor(xsrc);
  y = (long)floor(ysrc);

  /* PostScript setup: */
  *pstream << "gsave\n";
  *pstream << xdest << " " << YSCALE(ydest + fheight) << " translate\n";
  *pstream << fwidth << " " << fheight << " scale\n";
  *pstream << "/DataString " << width << " string def\n";
  *pstream << width << " " << height << " 8 [ ";
  *pstream << width << " 0 0 " << (-height) << " 0 " << height;
  *pstream << " ]\n{\n";
  *pstream << "  currentfile DataString readhexstring pop\n";
  *pstream << "} bind image\n";
  
#ifdef wx_x
  /* Output data as hex digits: */
  Display *d;
  Colormap cm;
  XImage *image;
  long j, i;
  char s[3];

#ifdef wx_motif
  d = source->display;
#else
  d = wxGetDisplay();
#endif

  cm = wxGetMainColormap(d);

  image = XGetImage(d, source->pixmap, x, y, width, height, AllPlanes, ZPixmap);

  s[2] = 0;

#define CM_CACHE_SIZE 256
  unsigned long cachesrc[CM_CACHE_SIZE];
  int cachedest[CM_CACHE_SIZE], cache_pos = 0, all_cache = FALSE;

  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      XColor xcol;
      unsigned long spixel;
      int pixel, k;
      const unsigned short MAX_COLOR = 0xFFFF;

      spixel = XGetPixel(image, i, j);

      for (k = cache_pos; k--; )
	if (cachesrc[k] == spixel) {
	  pixel = cachedest[k];
	  goto install;
	}
      if (all_cache)
	for (k = CM_CACHE_SIZE; k-- > cache_pos; )
	  if (cachesrc[k] == spixel) {
	    pixel = cachedest[k];
	    goto install;
	  }
      
      cachesrc[cache_pos] = xcol.pixel = spixel;
      XQueryColor(d, cm, &xcol);
      
      float r, g, b;

      r = ((float)(xcol.red) / MAX_COLOR);
      g = ((float)(xcol.green) / MAX_COLOR);
      b = ((float)(xcol.blue) / MAX_COLOR);

      pixel = (int)(255 * sqrt(((r * r) + (g * g) + (b * b)) / 3));

      cachedest[cache_pos] = pixel;
      
      if (++cache_pos >= CM_CACHE_SIZE) {
	cache_pos = 0;
	all_cache = TRUE;
      }

    install:      
      int h, l;

      h = (pixel >> 4) & 0xF;
      l = pixel & 0xF;

      if (h <= 9)
	s[0] = '0' + h;
      else
	s[0] = 'a' + (h - 10);
      if (l <= 9)
	s[1] = '0' + l;
      else
	s[1] = 'a' + (l - 10);
      
      *pstream << s;
    }
    *pstream << "\n";
  }

  XDestroyImage(image);
#endif

  *pstream << "grestore\n";
  
  CalcBoundingBox(xdest, YSCALE(ydest));
  CalcBoundingBox(xdest + fwidth, YSCALE(ydest + fheight));

  return TRUE;
}

float wxPostScriptDC::GetCharHeight (void)
{
  if (font)
    return (float) font->GetPointSize ();
  else
    return (float)12.0;
}

void wxPostScriptDC::GetTextExtent (const char *string, float *x, float *y,
	       float *descent, float *externalLeading, wxFont *theFont,
				    Bool WXUNUSED(use16))
{
  wxFont *fontToUse = theFont;
  if (!fontToUse)
    fontToUse = font;
    
  if (!pstream)
    return;
#if !USE_AFM_FOR_POSTSCRIPT
  // Provide a VERY rough estimate (avoid using it)
  int width = 12;
  int height = 12;

  if (fontToUse)
    {
      height = fontToUse->GetPointSize ();
      width = height;
    }
  *x = (float) strlen (string) * width;
  *y = (float) height;
  if (descent)
    *descent = (float)0.0;
  if (externalLeading)
    *externalLeading = (float)0.0;
#else
  // +++++ start of contributed code +++++
  
  // ************************************************************
  // method for calculating string widths in postscript:
  // read in the AFM (adobe font metrics) file for the
  // actual font, parse it and extract the character widths
  // and also the descender. this may be improved, but for now
  // it works well. the AFM file is only read in if the
  // font is changed. this may be chached in the future.
  // calls to GetTextExtent with the font unchanged are rather
  // efficient!!!
  //
  // for each font and style used there is an AFM file necessary.
  // currently i have only files for the roman font family.
  // i try to get files for the other ones!
  //
  // CAVE: the size of the string is currently always calculated
  //       in 'points' (1/72 of an inch). this should later on be
  //       changed to depend on the mapping mode.
  // CAVE: the path to the AFM files must be set before calling this
  //       function. this is usually done by a call like the following:
  //       wxSetAFMPath("d:\\wxw161\\afm\\");
  //
  // example:
  //
  //    wxPostScriptDC dc(NULL, TRUE);
  //    if (dc.Ok()){
  //      wxSetAFMPath("d:\\wxw161\\afm\\");
  //      dc.StartDoc("Test");
  //      dc.StartPage();
  //      float w,h;
  //      dc.SetFont(new wxFont(10, wxROMAN, wxNORMAL, wxNORMAL));
  //      dc.GetTextExtent("Hallo",&w,&h);
  //      dc.EndPage();
  //      dc.EndDoc();
  //    }
  //
  // by steve (stefan.hammes@urz.uni-heidelberg.de)
  // created: 10.09.94
  // updated: 14.05.95

  assert(fontToUse && "void wxPostScriptDC::GetTextExtent: no font defined");
  assert(x && "void wxPostScriptDC::GetTextExtent: x == NULL");
  assert(y && "void wxPostScriptDC::GetTextExtent: y == NULL");

  // these static vars are for storing the state between calls
  static int lastFamily= INT_MIN;
  static int lastSize= INT_MIN;
  static int lastStyle= INT_MIN;
  static int lastWeight= INT_MIN;
  static int lastDescender = INT_MIN;
  static int lastWidths[256]; // widths of the characters

  // get actual parameters
  const int Family = fontToUse->GetFamily();
  const int Size =   fontToUse->GetPointSize();
  const int Style =  fontToUse->GetStyle();
  const int Weight = fontToUse->GetWeight();

  // if we have another font, read the font-metrics
  if(Family!=lastFamily||Size!=lastSize||Style!=lastStyle||Weight!=lastWeight){
    // store actual values
    lastFamily = Family;
    lastSize =   Size;
    lastStyle =  Style;
    lastWeight = Weight;

    // read in new font metrics **************************************

    // 1. construct filename ******************************************
    /* MATTHEW: [2] Use wxTheFontNameDirectory */
    char *name;

    name = wxTheFontNameDirectory.GetAFMName(Family, Weight, Style);
    if (!name)
      name = "unknown";

    // get the directory of the AFM files
    char afmName[256];
    afmName[0] = 0;
    if (wxGetAFMPath())
      strcpy(afmName,wxGetAFMPath());

    // 2. open and process the file **********************************

    // a short explanation of the AFM format:
    // we have for each character a line, which gives its size
    // e.g.:
    //
    //   C 63 ; WX 444 ; N question ; B 49 -14 395 676 ;
    //
    // that means, we have a character with ascii code 63, and width 
    // (444/1000 * fontSize) points.
    // the other data is ignored for now!
    //
    // when the font has changed, we read in the right AFM file and store the
    // character widths in an array, which is processed below (see point 3.).
    
        // new elements JC Sun Aug 25 23:21:44 MET DST 1996

    
    strcat(afmName,name);
    strcat(afmName,".afm");
    FILE *afmFile = fopen(afmName,"r");
    if(afmFile==NULL){
      wxDebugMsg("GetTextExtent: can't open AFM file '%s'\n",afmName);
      wxDebugMsg("               using approximate values\n");
      int i;
      for (i=0; i<256; i++) lastWidths[i] = 500; // an approximate value
      lastDescender = -150; // dito.
    }else{
      int i;
      // init the widths array
      for(i=0; i<256; i++) lastWidths[i]= INT_MIN;
      // some variables for holding parts of a line
      char cString[10],semiString[10],WXString[10],descString[20];
      char upString[30], utString[30], encString[50];
      char line[256];
      int ascii,cWidth;
      // read in the file and parse it
      while(fgets(line,sizeof(line),afmFile)!=NULL){
        // A.) check for descender definition
        if(strncmp(line,"Descender",9)==0){
          if((sscanf(line,"%s%d",descString,&lastDescender)!=2)
	     || (strcmp(descString,"Descender")!=0)) {
	    wxDebugMsg("AFM-file '%s': line '%s' has error (bad descender)\n",
		       afmName,line);
          }
        }
            // JC 1.) check for UnderlinePosition
        else if(strncmp(line,"UnderlinePosition",17)==0){
          if((sscanf(line,"%s%f",upString,&UnderlinePosition)!=2)
	     || (strcmp(upString,"UnderlinePosition")!=0)) {
	    wxDebugMsg("AFM-file '%s': line '%s' has error (bad UnderlinePosition)\n",
		       afmName,line);
          }
        }
	// JC 2.) check for UnderlineThickness
        else if(strncmp(line,"UnderlineThickness",18)==0){
          if((sscanf(line,"%s%f",utString,&UnderlineThickness)!=2)
	     || (strcmp(utString,"UnderlineThickness")!=0)) {
	    wxDebugMsg("AFM-file '%s': line '%s' has error (bad UnderlineThickness)\n",
		       afmName,line);
          }
        }
	// JC 3.) check for EncodingScheme
        else if(strncmp(line,"EncodingScheme",14)==0){
          if((sscanf(line,"%s%s",utString,encString)!=2)
	     || (strcmp(utString,"EncodingScheme")!=0)) {
	    wxDebugMsg("AFM-file '%s': line '%s' has error (bad EncodingScheme)\n",
		       afmName,line);
          }
          else if (strncmp(encString, "AdobeStandardEncoding", 21))
          {
	    wxDebugMsg("AFM-file '%s': line '%s' has error (unsupported EncodingScheme %s)\n",
		       afmName,line, encString);
          }
        }
            // B.) check for char-width
        else if(strncmp(line,"C ",2)==0){
          if(sscanf(line,"%s%d%s%s%d",
              cString,&ascii,semiString,WXString,&cWidth)!=5){
             wxDebugMsg("AFM-file '%s': line '%s' has an error (bad character width)\n",afmName,line);
          }
          if(strcmp(cString,"C")!=0 || strcmp(semiString,";")!=0 ||
             strcmp(WXString,"WX")!=0){
             wxDebugMsg("AFM-file '%s': line '%s' has a format error\n",afmName,line);
          }
          //printf("            char '%c'=%d has width '%d'\n",ascii,ascii,cWidth);
          if(ascii>=0 && ascii<256){
            lastWidths[ascii] = cWidth; // store width
          }else{
	    /* MATTHEW: this happens a lot; don't print an error */
            // wxDebugMsg("AFM-file '%s': ASCII value %d out of range\n",afmName,ascii);
          }
        }
        // C.) ignore other entries.
      }
      fclose(afmFile);
    }
        // hack to compute correct values for german 'Umlaute'
        // the correct way would be to map the character names
        // like 'adieresis' to corresp. positions of ISOEnc and read
        // these values from AFM files, too. Maybe later ...
    lastWidths[196] = lastWidths['A'];  // Ä
    lastWidths[228] = lastWidths['a'];  // ä
    lastWidths[214] = lastWidths['O'];  // Ö
    lastWidths[246] = lastWidths['o'];  // ö
    lastWidths[220] = lastWidths['U'];  // Ü
    lastWidths[252] = lastWidths['u'];  // ü
    lastWidths[223] = lastWidths[251];  // ß
  }

      // JC: calculate UnderlineThickness/UnderlinePosition
  UnderlinePosition = UnderlinePosition * font->GetPointSize() / 1000.0f;
  UnderlineThickness = UnderlineThickness * font->GetPointSize() / 1000.0f * scale_factor;
  
  // 3. now the font metrics are read in, calc size *******************
  // this is done by adding the widths of the characters in the
  // string. they are given in 1/1000 of the size!

  float widthSum=0.0;
  float height=(float)Size; // by default
  unsigned char *p;
  for(p=(unsigned char *)string; *p; p++){
    if(lastWidths[*p]== INT_MIN){
      wxDebugMsg("GetTextExtent: undefined width for character '%c' (%d)\n",
                 *p,*p);
      widthSum += lastWidths[' ']/1000.0F * Size; // assume space
    }else{
      widthSum += (lastWidths[*p]/1000.0F)*Size;
    }
  }
  // add descender to height (it is usually a negative value)
  if(lastDescender!=INT_MIN){
    height += ((-lastDescender)/1000.0F) * Size; /* MATTHEW: forgot scale */
  }
  
  // return size values
  *x = widthSum;
  *y = height;

  // return other parameters
  if (descent){
    if(lastDescender!=INT_MIN){
      *descent = ((-lastDescender)/1000.0F) * Size; /* MATTHEW: forgot scale */
    }else{
      *descent = 0.0;
    }
  }

  // currently no idea how to calculate this!
  // if (externalLeading) *externalLeading = 0.0;
  if (externalLeading)
    *externalLeading = 0.0;

  // ----- end of contributed code -----
#endif
}

float wxPostScriptDC::GetCharWidth (void)
{
  return (float)0;
}


void wxPostScriptDC::SetMapMode (int mode)
{
  mapping_mode = mode;
  SetupCTM();
  return;
}

void wxPostScriptDC::SetLogicalOrigin(float x, float y)
{
  logical_origin_x = x;
  logical_origin_y = y;
  SetupCTM();
}

void
wxPostScriptDC::SetupCTM()
{
  switch (mapping_mode)
  {
  case MM_METRIC:
      scale_factor = 72 / 25.4;
      break;
  default:
      break;
  }
  *pstream << "setmatrix\nmatrix currentmatrix\n"
           << -logical_origin_x * scale_factor << " "
           << logical_origin_y * scale_factor << " translate\n"
           << scale_factor << " " << scale_factor << " scale\n";
}

void wxPostScriptDC::SetUserScale (float x, float y)
{
  user_scale_x = x;
  user_scale_y = y;

  scale_factor = user_scale_x;
  SetupCTM();
  
}

float wxPostScriptDC::DeviceToLogicalX (int x)
{
  return (float) x;
}

float wxPostScriptDC::DeviceToLogicalXRel (int x)
{
  return (float) x;
}

float wxPostScriptDC::DeviceToLogicalY (int y)
{
  return (float) y;
}

float wxPostScriptDC::DeviceToLogicalYRel (int y)
{
  return (float) y;
}

int wxPostScriptDC::LogicalToDeviceX (float x)
{
  return (int) x;
}

int wxPostScriptDC::LogicalToDeviceXRel (float x)
{
  return (int) x;
}

int wxPostScriptDC::LogicalToDeviceY (float y)
{
  return (int) y;
}

int wxPostScriptDC::LogicalToDeviceYRel (float y)
{
  return (int) y;
}

void wxPostScriptDC::GetSize(float *width, float *height)
{
  char *paperType = wxThePrintSetupData->GetPaperName();
  if (!paperType)
    paperType = "A4 210 x 297 mm";

  wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(paperType);
  if (!paper)
    paper = wxThePrintPaperDatabase->FindPaperType("A4 210 x 297 mm");
  if (paper)
  {
    *width = (float)paper->widthPixels;
    *height = (float)paper->heightPixels;
  }
  else
  {
    *width = (float)1000;
    *height = (float)1000;
  }
}

void wxPostScriptDC::GetSizeMM(float *width, float *height)
{
  char *paperType = wxThePrintSetupData->GetPaperName();
  if (!paperType)
    paperType = "A4 210 x 297 mm";

  wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(paperType);
  if (!paper)
    paper = wxThePrintPaperDatabase->FindPaperType("A4 210 x 297 mm");
  if (paper)
  {
    *width = (float)paper->widthMM;
    *height = (float)paper->heightMM;
  }
  else
  {
    *width = (float)1000;
    *height = (float)1000;
  }
}

void wxPostScriptDC::CalcBoundingBox(float x, float y)
{
    float device_x = x - logical_origin_x * scale_factor;
    float device_y = y + logical_origin_y * scale_factor;
    
    if (device_x < min_x) min_x = device_x;
    if (device_y < min_y) min_y = device_y;
    if (device_x > max_x) max_x = device_x;
    if (device_y > max_y) max_y = device_y;
}

class wxPrinterDialogBox:public wxDialogBox
{
  public:
  wxPrinterDialogBox (wxWindow *parent, char *title, Bool modal = FALSE,
		      int x = -1, int y = -1, int
		      width = -1, int height = -1);
};

wxPrinterDialogBox::wxPrinterDialogBox (wxWindow *parent, char *title, Bool isModal,
		    int x, int y, int width, int height):
wxDialogBox (parent, title, isModal, x, y, width, height)
{
}

Bool wxPrinterDialogAnswer = TRUE;

void 
wxPrinterDialogOk (wxButton & button, wxEvent & WXUNUSED(event))
{
  wxPrinterDialogAnswer = TRUE;
  wxPrinterDialogBox *dialog = (wxPrinterDialogBox *) button.GetParent ();
  dialog->Show (FALSE);
}

void 
wxPrinterDialogCancel (wxButton & button, wxEvent & WXUNUSED(event))
{
  wxPrinterDialogAnswer = FALSE;
  wxPrinterDialogBox *dialog = (wxPrinterDialogBox *) button.GetParent ();
  dialog->Show (FALSE);
}

Bool 
XPrinterDialog (wxWindow *parent)
{
  wxBeginBusyCursor();
  char buf[100];
  wxPrinterDialogBox dialog (parent, "Printer Settings", TRUE, 150, 150, 400, 400);
  dialog.SetLabelPosition(wxVERTICAL);

  wxButton *okBut = new wxButton ((wxPrinterDialogBox *)&dialog, (wxFunction) wxPrinterDialogOk, wxSTR_BUTTON_OK);
  (void) new wxButton ((wxPrinterDialogBox *)&dialog, (wxFunction) wxPrinterDialogCancel, wxSTR_BUTTON_CANCEL);
  dialog.NewLine ();
  dialog.NewLine ();
  okBut->SetDefault();

#ifdef wx_x
  wxText text_prt (&dialog, (wxFunction) NULL, "Printer Command: ", wxThePrintSetupData->GetPrinterCommand(), -1, -1, 100, -1);

  wxText text0 (&dialog, (wxFunction) NULL, "Printer Options: ", wxThePrintSetupData->GetPrinterOptions(), -1, -1, 150, -1);
  dialog.NewLine ();
  dialog.NewLine ();
#endif

//  char *orientation[] = {"Portrait", "Landscape"};  // HP compiler complains
  char *orientation[2];
  orientation[0] = "Portrait";
  orientation[1] = "Landscape";
  wxRadioBox radio0 ((wxPrinterDialogBox *)&dialog, (wxFunction)NULL, "Orientation: ",-1,-1,-1,-1,2,orientation,2,wxFLAT);
  radio0.SetSelection((int)wxThePrintSetupData->GetPrinterOrientation());

  // @@@ Configuration hook
  if (wxThePrintSetupData->GetPrintPreviewCommand() == NULL)
    wxThePrintSetupData->SetPrintPreviewCommand(PS_VIEWER_PROG);

#if USE_RESOURCES
  wxGetResource ("wxWindows", "PSView", &wxThePrintSetupData->previewCommand);
#endif
//  char *print_modes[] = {"Send to Printer", "Print to File", "Preview Only"}; 
  char *print_modes[3];
  print_modes[0] = "Send to Printer";
  print_modes[1] = "Print to File";
  print_modes[2] = "Preview Only";
  int features = (wxThePrintSetupData->GetPrintPreviewCommand() && *wxThePrintSetupData->GetPrintPreviewCommand()) ? 3 : 2;
  wxRadioBox radio1 (&dialog, (wxFunction)NULL, "PostScript:"
    ,-1,-1,-1,-1, features, print_modes, features, wxFLAT);
#ifdef wx_msw
  radio1.Enable(0, FALSE);
  if (wxThePrintSetupData->GetPrintPreviewCommand() && *wxThePrintSetupData->GetPrintPreviewCommand())
    radio1.Enable(2, FALSE);
#endif
  radio1.SetSelection((int)wxThePrintSetupData->GetPrinterMode());
  
  float wx_printer_translate_x, wx_printer_translate_y;
  float wx_printer_scale_x, wx_printer_scale_y;
  wxThePrintSetupData->GetPrinterTranslation(&wx_printer_translate_x, &wx_printer_translate_y);
  wxThePrintSetupData->GetPrinterScaling(&wx_printer_scale_x, &wx_printer_scale_y);

  sprintf (buf, "%.2f", wx_printer_scale_x);
  dialog.NewLine ();
  dialog.NewLine ();

  wxText text1 ((wxPrinterDialogBox *)&dialog, (wxFunction) NULL, "X Scaling: ", buf, -1, -1, 100, -1);

  sprintf (buf, "%.2f", wx_printer_scale_y);
  wxText text2 ((wxPrinterDialogBox *)&dialog, (wxFunction) NULL, "Y Scaling: ", buf, -1, -1, 100, -1);

  dialog.NewLine ();

  sprintf (buf, "%.2f", wx_printer_translate_x);
  wxText text3 ((wxPrinterDialogBox *)&dialog, (wxFunction) NULL, "X Translation: ", buf, -1, -1, 100, -1);

  sprintf (buf, "%.2f", wx_printer_translate_y);
  wxText text4 ((wxPrinterDialogBox *)&dialog, (wxFunction) NULL, "Y Translation: ", buf, -1, -1, 100, -1);

  dialog.NewLine ();
  dialog.NewLine ();

  dialog.Fit ();

  wxEndBusyCursor();
  dialog.Show (TRUE);

  if (wxPrinterDialogAnswer)
    {
      StringToFloat (text1.GetValue (), &wxThePrintSetupData->printerScaleX);
      StringToFloat (text2.GetValue (), &wxThePrintSetupData->printerScaleY);
      StringToFloat (text3.GetValue (), &wxThePrintSetupData->printerTranslateX);
      StringToFloat (text4.GetValue (), &wxThePrintSetupData->printerTranslateY);

#ifdef wx_x
      wxThePrintSetupData->SetPrinterOptions(text0.GetValue ());
      wxThePrintSetupData->SetPrinterCommand(text_prt.GetValue ());
#endif

      wxThePrintSetupData->SetPrinterOrientation((radio0.GetSelection() == PS_LANDSCAPE ? PS_LANDSCAPE : PS_PORTRAIT));

      // C++ wants this
      switch ( radio1.GetSelection() ) {
	case PS_PREVIEW: wxThePrintSetupData->SetPrinterMode(PS_PREVIEW); break;
	case PS_FILE:    wxThePrintSetupData->SetPrinterMode(PS_FILE);    break;
	case PS_PRINTER: wxThePrintSetupData->SetPrinterMode(PS_PRINTER); break;
     }

    }

  return wxPrinterDialogAnswer;
}

// PostScript printer settings
// RETAINED FOR BACKWARD COMPATIBILITY
void wxSetPrinterCommand(char *cmd)
{
  wxThePrintSetupData->SetPrinterCommand(cmd);
}

void wxSetPrintPreviewCommand(char *cmd)
{
  wxThePrintSetupData->SetPrintPreviewCommand(cmd);
}

void wxSetPrinterOptions(char *flags)
{
  wxThePrintSetupData->SetPrinterOptions(flags);
}

void wxSetPrinterFile(char *f)
{
  wxThePrintSetupData->SetPrinterFile(f);
}

void wxSetPrinterOrientation(int orient)
{
  wxThePrintSetupData->SetPrinterOrientation(orient);
}

void wxSetPrinterScaling(float x, float y)
{
  wxThePrintSetupData->SetPrinterScaling(x, y);
}

void wxSetPrinterTranslation(float x, float y)
{
  wxThePrintSetupData->SetPrinterTranslation(x, y);
}

// 1 = Preview, 2 = print to file, 3 = send to printer
void wxSetPrinterMode(int mode)
{
  wxThePrintSetupData->SetPrinterMode(mode);
}

void wxSetAFMPath(char *f)
{
  wxThePrintSetupData->SetAFMPath(f);
}

// Get current values
char *wxGetPrinterCommand(void)
{
  return wxThePrintSetupData->GetPrinterCommand();
}

char *wxGetPrintPreviewCommand(void)
{
  return wxThePrintSetupData->GetPrintPreviewCommand();
}

char *wxGetPrinterOptions(void)
{
  return wxThePrintSetupData->GetPrinterOptions();
}

char *wxGetPrinterFile(void)
{
  return wxThePrintSetupData->GetPrinterFile();
}

int wxGetPrinterOrientation(void)
{
  return wxThePrintSetupData->GetPrinterOrientation();
}

void wxGetPrinterScaling(float *x, float *y)
{
  wxThePrintSetupData->GetPrinterScaling(x, y);
}

void wxGetPrinterTranslation(float *x, float *y)
{
  wxThePrintSetupData->GetPrinterTranslation(x, y);
}

int wxGetPrinterMode(void)
{
  return wxThePrintSetupData->GetPrinterMode();
}

char *wxGetAFMPath(void)
{
  return wxThePrintSetupData->GetAFMPath();
}

/*
 * Print setup data
 */

wxPrintSetupData::wxPrintSetupData(void)
{
  printerCommand = NULL;
  previewCommand = NULL;
  printerFlags = NULL;
  printerOrient = PS_PORTRAIT;
  printerScaleX = (float)1.0;
  printerScaleY = (float)1.0;
  printerTranslateX = (float)0.0;
  printerTranslateY = (float)0.0;
  // 1 = Preview, 2 = print to file, 3 = send to printer
  printerMode = 3;
  afmPath = NULL;
  paperName = NULL;
  printColour = TRUE;
  printerFile = NULL;
}

wxPrintSetupData::~wxPrintSetupData(void)
{
  if (printerCommand)
    delete[] printerCommand;
  if (previewCommand)
    delete[] previewCommand;
  if (printerFlags)
    delete[] printerFlags;
  if (afmPath)
    delete[] afmPath;
  if (paperName)
    delete[] paperName;
  if (printerFile)
    delete[] printerFile;
}

void wxPrintSetupData::SetPrinterCommand(char *cmd)
{
  if (cmd == printerCommand)
    return;

  if (printerCommand)
    delete[] printerCommand;
  if (cmd)
    printerCommand = copystring(cmd);
  else
    printerCommand = NULL;
}

void wxPrintSetupData::SetPrintPreviewCommand(char *cmd)
{
  if (cmd == previewCommand)
    return;
    
  if (previewCommand)
    delete[] previewCommand;
  if (cmd)
    previewCommand = copystring(cmd);
  else
    previewCommand = NULL;
}

void wxPrintSetupData::SetPaperName(char *name)
{
  if (name == paperName)
    return;

  if (paperName)
    delete[] paperName;
  if (name)
    paperName = copystring(name);
  else
    paperName = NULL;
}

void wxPrintSetupData::SetPrinterOptions(char *flags)
{
  if (printerFlags == flags)
    return;
   
  if (printerFlags)
    delete[] printerFlags;
  if (flags)
    printerFlags = copystring(flags);
  else
    printerFlags = NULL;
}

void wxPrintSetupData::SetPrinterFile(char *f)
{
  if (f == printerFile)
    return;
    
  if (printerFile)
    delete[] printerFile;
  if (f)
    printerFile = copystring(f);
  else
    printerFile = NULL;
}

void wxPrintSetupData::SetPrinterOrientation(int orient)
{
  printerOrient = orient;
}

void wxPrintSetupData::SetPrinterScaling(float x, float y)
{
  printerScaleX = x;
  printerScaleY = y;
}

void wxPrintSetupData::SetPrinterTranslation(float x, float y)
{
  printerTranslateX = x;
  printerTranslateY = y;
}

// 1 = Preview, 2 = print to file, 3 = send to printer
void wxPrintSetupData::SetPrinterMode(int mode)
{
  printerMode = mode;
}

void wxPrintSetupData::SetAFMPath(char *f)
{
  if (f == afmPath)
    return;
    
  if (afmPath)
    delete[] afmPath;
  if (f)
    afmPath = copystring(f);
  else
    afmPath = NULL;
}

void wxPrintSetupData::SetColour(Bool col)
{
  printColour = col;
}

// Get current values
char *wxPrintSetupData::GetPrinterCommand(void)
{
  return printerCommand;
}

char *wxPrintSetupData::GetPrintPreviewCommand(void)
{
  return previewCommand;
}

char *wxPrintSetupData::GetPrinterOptions(void)
{
  return printerFlags;
}

char *wxPrintSetupData::GetPrinterFile(void)
{
  return printerFile;
}

char *wxPrintSetupData::GetPaperName(void)
{
  return paperName;
}

int wxPrintSetupData::GetPrinterOrientation(void)
{
  return printerOrient;
}

void wxPrintSetupData::GetPrinterScaling(float *x, float *y)
{
  *x = printerScaleX;
  *y = printerScaleY;
}

void wxPrintSetupData::GetPrinterTranslation(float *x, float *y)
{
  *x = printerTranslateX;
  *y = printerTranslateY;
}

int wxPrintSetupData::GetPrinterMode(void)
{
  return printerMode;
}

char *wxPrintSetupData::GetAFMPath(void)
{
  return afmPath;
}

Bool wxPrintSetupData::GetColour(void)
{
  return printColour;
}

void wxPrintSetupData::operator=(wxPrintSetupData& data)
{
  SetPrinterCommand(data.GetPrinterCommand());
  SetPrintPreviewCommand(data.GetPrintPreviewCommand());
  SetPrinterOptions(data.GetPrinterOptions());
  float x, y;
  data.GetPrinterTranslation(&x, &y);
  SetPrinterTranslation(x, y);

  data.GetPrinterScaling(&x, &y);
  SetPrinterScaling(x, y);

  SetPrinterOrientation(data.GetPrinterOrientation());
  SetPrinterMode(data.GetPrinterMode());
  SetAFMPath(data.GetAFMPath());
  SetPaperName(data.GetPaperName());
  SetColour(data.GetColour());
}

void wxInitializePrintSetupData(Bool init)
{
  if (init)
  {
    wxThePrintSetupData = new wxPrintSetupData;

    wxThePrintSetupData->SetPrintPreviewCommand(PS_VIEWER_PROG);
    wxThePrintSetupData->SetPrinterOrientation(PS_PORTRAIT);
    wxThePrintSetupData->SetPrinterMode(PS_PREVIEW);
    wxThePrintSetupData->SetPaperName("A4 210 x 297 mm");

    // Could have a .ini file to read in some defaults
    // - and/or use environment variables, e.g. WXWIN
#ifdef VMS
    wxThePrintSetupData->SetPrinterCommand("print");
    wxThePrintSetupData->SetPrinterOptions("/nonotify/queue=psqueue");
    wxThePrintSetupData->SetAFMPath("sys$ps_font_metrics:");
#endif
#ifdef wx_msw
    wxThePrintSetupData->SetPrinterCommand("print");
    wxThePrintSetupData->SetAFMPath("c:\\windows\\system\\");
    wxThePrintSetupData->SetPrinterOptions(NULL);
#endif
#if !defined(VMS) && !defined(wx_msw)
    wxThePrintSetupData->SetPrinterCommand("lpr");
    wxThePrintSetupData->SetPrinterOptions(NULL);
    wxThePrintSetupData->SetAFMPath(NULL);
#endif
  }
  else
  {
    if (wxThePrintSetupData)
      delete wxThePrintSetupData;
     wxThePrintSetupData = NULL;
  }
}

/*
 * Paper size database for PostScript
 */

wxPrintPaperType::wxPrintPaperType(char *name, int wmm, int hmm, int wp, int hp)
{
  widthMM = wmm;
  heightMM = hmm;
  widthPixels = wp;
  heightPixels = hp;
  pageName = copystring(name);
}

wxPrintPaperType::~wxPrintPaperType(void)
{
  delete[] pageName;
}

wxPrintPaperDatabase::wxPrintPaperDatabase(void):wxList(wxKEY_STRING)
{
  DeleteContents(TRUE);
}

wxPrintPaperDatabase::~wxPrintPaperDatabase(void)
{
}

void wxPrintPaperDatabase::CreateDatabase(void)
{
  // Need correct values for page size in pixels.
  // Each unit is one 'point' = 1/72 of an inch.
  // NOTE: WE NEED ALSO TO MAKE ADJUSTMENTS WHEN TRANSLATING
  // in wxPostScriptDC code, so we can start from top left.
  // So access this database and translate by appropriate number
  // of points for this paper size. OR IS IT OK ALREADY?
  // Can't remember where the PostScript origin is by default.
  // Heck, someone will know how to make it hunky-dory...
  // JACS 25/5/95
  
  AddPaperType("A4 210 x 297 mm", 210, 297,         595, 842);
  AddPaperType("A3 297 x 420 mm", 297, 420,         842, 1191);
  AddPaperType("Letter 8 1/2 x 11 in", 216, 279,    612, 791);
  AddPaperType("Legal 8 1/2 x 14 in", 216, 356,     612, 1009);
}

void wxPrintPaperDatabase::ClearDatabase(void)
{
  Clear();
}

void wxPrintPaperDatabase::AddPaperType(char *name, int wmm, int hmm, int wp, int hp)
{
  Append(name, new wxPrintPaperType(name, wmm, hmm, wp, hp));
}

wxPrintPaperType *wxPrintPaperDatabase::FindPaperType(char *name)
{
  wxNode *node = Find(name);
  if (node)
    return (wxPrintPaperType *)node->Data();
  else
    return NULL;
}

#endif
