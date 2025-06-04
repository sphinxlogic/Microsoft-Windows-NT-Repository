/*
 * File:	wx_gdi.cc
 * Purpose:	GDI (Graphics Device Interface) objects and functions
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_gdi.cc,v 1.1 1994/08/14 21:59:17 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   17 Feb 97   11:58 am
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx_setup.h"
#include "wx_list.h"
#include "wx_utils.h"
#include "wx_main.h"
#include "wx_gdi.h"
#endif

#include "assert.h"

#if USE_XPM_IN_MSW
#define FOR_MSW 1
#include "..\..\contrib\wxxpm\libxpm.34b\lib\xpm34.h"
#endif

#if USE_IMAGE_LOADING_IN_MSW
#include "..\..\utils\dib\dib.h"
#endif

#if USE_RESOURCE_LOADING_IN_MSW
#include "..\..\utils\rcparser\src\wxcurico.h"
#include "..\..\utils\rcparser\src\cric_prv.h"
#endif

/*
Bool wxMakeBitmapAndPalette(LPBITMAPINFOHEADER lpInfo,
			HPALETTE * phPal, HBITMAP * phBitmap);
*/

// #define DEBUG_CREATE
// #define TRACE_GDI

void wxGDIObject::IncrementResourceUsage(void)
{
//  wxDebugMsg("Object %ld about to be incremented: %d\n", (long)this, usageCount);
  usageCount ++;
};

void wxGDIObject::DecrementResourceUsage(void)
{
  usageCount --;
  if (wxTheApp)
    wxTheApp->SetPendingCleanup(TRUE);
//  wxDebugMsg("Object %ld decremented: %d\n", (long)this, usageCount);
  if (usageCount < 0)
  {
    char buf[80];
    sprintf(buf, "Object %ld usage count is %d\n", (long)this, usageCount);
    wxDebugMsg(buf);
  }
//  assert(usageCount >= 0);
};

wxFont::wxFont(void)
{
  point_size = 0;
  temporary = TRUE;
  cfont = NULL;
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheFontList->Append(this);
#endif
}

/* Constructor for a font. Note that the real construction is done
 * in wxDC::SetFont, when information is available about scaling etc.
 */
wxFont::wxFont(int PointSize, int Family, int Style, int Weight, Bool Underlined, const char *Face):
  wxbFont(PointSize, Family, Style, Weight, Underlined, Face)
{
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheFontList->Append(this);
#endif

  Create(PointSize, Family, Style, Weight, Underlined, Face);
}

Bool wxFont::Create(int PointSize, int Family, int Style, int Weight, Bool Underlined, const char *Face)
{
  family = Family;
  style = Style;
  weight = Weight;
  point_size = PointSize;
  underlined = Underlined;
  if (Face)
    faceName = copystring(Face);
  else
    faceName = NULL;

  temporary = FALSE;
  cfont = NULL;

  return TRUE;
}

wxFont::~wxFont()
{
  FreeResource();
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheFontList->DeleteObject(this);
#endif
}

Bool wxFont::RealizeResource(void)
{
  if (!cfont)
  {
    BYTE ff_italic;
    int ff_weight = 0;
    int ff_family = 0;
    char *ff_face = NULL;

    switch (family)
    {
      case wxSCRIPT:     ff_family = FF_SCRIPT ;
                         ff_face = "Script" ;
                         break ;
      case wxDECORATIVE: ff_family = FF_DECORATIVE;
                         break;
      case wxROMAN:      ff_family = FF_ROMAN;
                         ff_face = "Times New Roman" ;
                         break;
      case wxTELETYPE:
      case wxMODERN:     ff_family = FF_MODERN;
                         ff_face = "Courier New" ;
                         break;
      case wxSWISS:      ff_family = FF_SWISS;
                         ff_face = "Arial";
                         break;
      case wxDEFAULT:
      default:           ff_family = FF_SWISS;
                         ff_face = "Arial" ; 
    }

    if (style == wxITALIC || style == wxSLANT)
      ff_italic = 1;
    else
      ff_italic = 0;

    if (weight == wxNORMAL)
      ff_weight = FW_NORMAL;
    else if (weight == wxLIGHT)
      ff_weight = FW_LIGHT;
    else if (weight == wxBOLD)
      ff_weight = FW_BOLD;

#if defined(wx_x) || (defined(wx_msw) && USE_PORTABLE_FONTS_IN_MSW)
    ff_face = wxTheFontNameDirectory.GetScreenName(family, weight, style);
    if (!ff_face || !*ff_face)
      ff_face = NULL;
#else
    if (faceName)
      ff_face = faceName;
#endif

/* Always calculate fonts using the screen DC (is this the best strategy?)
 * There may be confusion if a font is selected into a printer
 * DC (say), because the height will be calculated very differently.
    // What sort of display is it?
    int technology = ::GetDeviceCaps(dc, TECHNOLOGY);

    int nHeight;
    
    if (technology != DT_RASDISPLAY && technology != DT_RASPRINTER)
    {
      // Have to get screen DC Caps, because a metafile will return 0.
      HDC dc2 = ::GetDC(NULL);
      nHeight = point_size*GetDeviceCaps(dc2, LOGPIXELSY)/72;
      ::ReleaseDC(NULL, dc2);
    }
    else
    {
      nHeight = point_size*GetDeviceCaps(dc, LOGPIXELSY)/72;
    }
*/
    // Have to get screen DC Caps, because a metafile will return 0.
    HDC dc2 = ::GetDC(NULL);
    int ppInch = ::GetDeviceCaps(dc2, LOGPIXELSY);
    ::ReleaseDC(NULL, dc2);

    // New behaviour: apparently ppInch varies according to
    // Large/Small Fonts setting in Windows. This messes
    // up fonts. So, set ppInch to a constant 96 dpi.
    ppInch = 96;
    
#if FONT_SIZE_COMPATIBILITY
    // Incorrect, but compatible with old wxWindows behaviour
    int nHeight = (point_size*ppInch/72);
#else
    // Correct for Windows compatibility
    int nHeight = - (point_size*ppInch/72);
#endif

    Bool ff_underline = underlined;
    
    cfont = CreateFont(nHeight, 0, 0, 0,ff_weight,ff_italic,(BYTE)ff_underline,
                0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                PROOF_QUALITY, DEFAULT_PITCH | ff_family, ff_face);
#ifdef DEBUG_CREATE
    if (cfont==NULL) wxError("Cannot create font","Internal Error") ;
#endif
    return (cfont != NULL);
  }
  return FALSE;
}

Bool wxFont::FreeResource(void)
{
  if (cfont && (GetResourceUsage() == 0))
  {
    DeleteObject(cfont);
    cfont = 0;
    return TRUE;
  }
  return FALSE;
}

Bool wxFont::UseResource(void)
{
  IncrementResourceUsage();
  return TRUE;
}

Bool wxFont::ReleaseResource(void)
{
  DecrementResourceUsage();
  return TRUE;
}
  
HANDLE wxFont::GetResourceHandle(void)
{
  return (HANDLE)cfont ;
}

Bool wxFont::IsFree(void)
{
  return (cfont == 0);
}

/*
 * Colour map
 *
 */

wxColourMap::wxColourMap(void)
{
  ms_palette = 0;
}

wxColourMap::wxColourMap(const int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue)
{
  Create(n, red, green, blue);
}

wxColourMap::~wxColourMap(void)
{
  if (ms_palette)
  {
    DeleteObject(ms_palette);
  }
}

Bool wxColourMap::Create(const int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue)
{
  if (ms_palette)
    return FALSE;
    
  NPLOGPALETTE npPal = (NPLOGPALETTE)LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) + 
                        (WORD)n * sizeof(PALETTEENTRY));
  if (!npPal)
    return(FALSE);

  npPal->palVersion = 0x300;
  npPal->palNumEntries = n;

  int i;
  for (i = 0; i < n; i ++)
  {
    npPal->palPalEntry[i].peRed = red[i];
    npPal->palPalEntry[i].peGreen = green[i];
    npPal->palPalEntry[i].peBlue = blue[i];
    npPal->palPalEntry[i].peFlags = 0;
  }
  ms_palette = CreatePalette((LPLOGPALETTE)npPal);
  LocalFree((HANDLE)npPal);
  return TRUE;
}

int wxColourMap::GetPixel(const unsigned char red, const unsigned char green, const unsigned char blue)
{
  return ::GetNearestPaletteIndex(ms_palette, PALETTERGB(red, green, blue));
}

Bool wxColourMap::GetRGB(const int index, unsigned char *red, unsigned char *green, unsigned char *blue)
{
  if (index < 0 || index > 255)
         return FALSE;

  PALETTEENTRY entry;
  if (::GetPaletteEntries(ms_palette, index, 1, &entry))
  {
         *red = entry.peRed;
         *green = entry.peGreen;
         *blue = entry.peBlue;
         return TRUE;
  } else
         return FALSE;
}

// Pens

wxPen::wxPen(void)
{
  stipple = NULL ;
  style = wxSOLID;
  width = 1;
  join = wxJOIN_ROUND ;
  cap = wxCAP_ROUND ;
  nb_dash = 0 ;
  dash = NULL ;
  cpen = NULL;
  my_old_cpen = NULL ;
  old_width = -1 ;
  old_style = -1 ;
  old_join  = -1 ;
  old_cap  = -1 ;
  old_nb_dash  = -1 ;
  old_dash  = NULL ;
  old_color  = 0 ;
  old_stipple = NULL ;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxThePenList->AddPen(this);
#endif
}

wxPen::~wxPen()
{
  FreeResource();
  
  wxThePenList->RemovePen(this);
}

wxPen::wxPen(wxColour& col, int Width, int Style)
{
  colour = col;
  stipple = NULL;
  width = Width;
  style = Style;
  join = wxJOIN_ROUND ;
  cap = wxCAP_ROUND ;
  nb_dash = 0 ;
  dash = NULL ;
  cpen = NULL ;
  my_old_cpen = NULL ;
  old_width = -1 ;
  old_style = -1 ;
  old_join  = -1 ;
  old_cap  = -1 ;
  old_nb_dash  = -1 ;
  old_dash  = NULL ;
  old_color  = 0 ;
  old_stipple = NULL ;

#ifndef WIN32
  // In Windows, only a pen of width = 1 can be dotted or dashed!
  if ((Style == wxDOT) || (Style == wxLONG_DASH) ||
      (Style == wxSHORT_DASH) || (Style == wxDOT_DASH) ||
      (Style == wxUSER_DASH))
    width = 1;
#else
/***
  DWORD vers = GetVersion() ;
  WORD  high = HIWORD(vers) ; // high bit=0 for NT, 1 for Win32s
  // Win32s doesn't support wide dashed pens

  if ((high&0x8000)!=0)
***/
  if (wxGetOsVersion()==wxWIN32S)
  {
    // In Windows, only a pen of width = 1 can be dotted or dashed!
    if ((Style == wxDOT) || (Style == wxLONG_DASH) ||
        (Style == wxSHORT_DASH) || (Style == wxDOT_DASH) ||
        (Style == wxUSER_DASH))
      width = 1;
  }
#endif 

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxThePenList->AddPen(this);
#endif
}

wxPen::wxPen(const char *col, int Width, int Style)
{
  colour = col;
  stipple = NULL ;
  width = Width;
  style = Style;
  join = wxJOIN_ROUND ;
  cap = wxCAP_ROUND ;
  nb_dash = 0 ;
  dash = NULL ;
  cpen = NULL ;
  my_old_cpen = NULL ;
  old_width = -1 ;
  old_style = -1 ;
  old_join  = -1 ;
  old_cap  = -1 ;
  old_nb_dash  = -1 ;
  old_dash  = NULL ;
  old_color  = 0 ;
  old_stipple = NULL ;

  // In Windows, only a pen of width = 1 can be dotted or dashed!
  if ((Style == wxDOT) || (Style == wxLONG_DASH) || (Style == wxSHORT_DASH) || (Style == wxDOT_DASH))
    width = 1;
    
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxThePenList->AddPen(this);
#endif
}

Bool wxPen::RealizeResource(void)
{
  if (cpen == 0)
  {
    if (style==wxTRANSPARENT)
    {
      cpen = ::GetStockObject(NULL_PEN);
      return TRUE;
    }

    COLORREF ms_colour = 0 ;
    ms_colour = colour.pixel ;

    // Join style, Cap style, Pen Stippling only on Win32.
    // Currently no time to find equivalent on Win3.1, sorry
    // [if such equiv exist!!]
#ifdef WIN32
    if (join==wxJOIN_ROUND        &&
        cap==wxCAP_ROUND          &&
        style!=wxUSER_DASH        &&
        style!=wxSTIPPLE
       )
      cpen = CreatePen(wx2msPenStyle(style), width, ms_colour);
    else
    {
      DWORD ms_style = PS_GEOMETRIC|wx2msPenStyle(style) ;

      LOGBRUSH logb ;

      switch(join)
      {
        case wxJOIN_BEVEL: ms_style |= PS_JOIN_BEVEL ; break ;
        case wxJOIN_MITER: ms_style |= PS_JOIN_MITER ; break ;
        default:
        case wxJOIN_ROUND: ms_style |= PS_JOIN_ROUND ; break ;
      }

      switch(cap)
      {
        case wxCAP_PROJECTING: ms_style |= PS_ENDCAP_SQUARE ; break ;
        case wxCAP_BUTT:       ms_style |= PS_ENDCAP_FLAT ;   break ;
        default:
        case wxCAP_ROUND:      ms_style |= PS_ENDCAP_ROUND ;  break ;
      }

      switch(style)
      {
        case wxSTIPPLE:
          logb.lbStyle = BS_PATTERN ;
          if (stipple)
            logb.lbHatch = (LONG)stipple->ms_bitmap ;
          else
            logb.lbHatch = (LONG)0 ;
        break ;
        case wxBDIAGONAL_HATCH:
          logb.lbStyle = BS_HATCHED ;
          logb.lbHatch = HS_BDIAGONAL ;
        break ;
        case wxCROSSDIAG_HATCH:
          logb.lbStyle = BS_HATCHED ;
          logb.lbHatch = HS_DIAGCROSS ;
        break ;
        case wxFDIAGONAL_HATCH:
          logb.lbStyle = BS_HATCHED ;
          logb.lbHatch = HS_FDIAGONAL ;
        break ;
        case wxCROSS_HATCH:
          logb.lbStyle = BS_HATCHED ;
          logb.lbHatch = HS_CROSS ;
        break ;
        case wxHORIZONTAL_HATCH:
          logb.lbStyle = BS_HATCHED ;
          logb.lbHatch = HS_HORIZONTAL ;
        break ;
        case wxVERTICAL_HATCH:
          logb.lbStyle = BS_HATCHED ;
          logb.lbHatch = HS_VERTICAL ;
        break ;
        default:
          logb.lbStyle = BS_SOLID ;
        break ;
      }
      logb.lbColor = ms_colour ;
      wxDash *real_dash ;
      if (style==wxUSER_DASH&&nb_dash&&dash)
      {
        real_dash = new wxDash[nb_dash] ;
        int i;
        for (i=0;i<nb_dash;i++)
          real_dash[i] = dash[i] * width ;
      }
      else
        real_dash = NULL ;

      // Win32s doesn't have ExtCreatePen function...
      if (wxGetOsVersion()==wxWINDOWS_NT)
        cpen = ExtCreatePen(ms_style,width,&logb,
                            style==wxUSER_DASH?nb_dash:0,real_dash);
      else
        cpen = CreatePen(wx2msPenStyle(style), width, ms_colour);

      if (real_dash)
        delete [] real_dash ;
    }
#else
    cpen = CreatePen(wx2msPenStyle(style), width, ms_colour);
#endif
#ifdef DEBUG_CREATE
    if (cpen==NULL)
      wxError("Cannot create pen","Internal error") ;
#endif
    return TRUE;
  }
  return FALSE;
}

HANDLE wxPen::GetResourceHandle(void)
{
  return cpen;
}

Bool wxPen::FreeResource(void)
{
  if ((GetResourceUsage() == 0) && (cpen != 0))
  {
    DeleteObject(cpen);
    cpen = 0;
    return TRUE;
  }
  else return FALSE;
}

Bool wxPen::UseResource(void)
{
  IncrementResourceUsage();
  return TRUE;
}

Bool wxPen::ReleaseResource(void)
{
  DecrementResourceUsage();
  return TRUE;
}

Bool wxPen::IsFree(void)
{
  return (cpen == 0);
}

void wxPen::SetColour(wxColour& col)
{
  wxbPen::SetColour(col);
  
  if (FreeResource())
    RealizeResource();
}

void wxPen::SetColour(const char *col)
{
  wxbPen::SetColour(col);
  
  if (FreeResource())
    RealizeResource();
}

void wxPen::SetColour(char r, char g, char b)
{
  wxbPen::SetColour(r, g, b);
  
  if (FreeResource())
    RealizeResource();
}

void wxPen::SetWidth(int width)
{
  wxbPen::SetWidth(width);
  
  if (FreeResource())
    RealizeResource();
}

void wxPen::SetStyle(int style)
{
  wxbPen::SetStyle(style);
  
  if (FreeResource())
    RealizeResource();
}

void wxPen::SetStipple(wxBitmap *stipple)
{
  wxbPen::SetStipple(stipple);
  
  if (FreeResource())
    RealizeResource();
}

void wxPen::SetDashes(int nb_dashes, wxDash *dash)
{
  wxbPen::SetDashes(nb_dashes, dash);
  
  if (FreeResource())
    RealizeResource();
}

void wxPen::SetJoin(int join)
{
  wxbPen::SetJoin(join);
  
  if (FreeResource())
    RealizeResource();
}

void wxPen::SetCap(int cap)
{
  wxbPen::SetCap(cap);
  
  if (FreeResource())
    RealizeResource();
}

int wx2msPenStyle(int wx_style)
{
  int cstyle;
/***
#ifdef WIN32
  DWORD vers = GetVersion() ;
  WORD  high = HIWORD(vers) ; // high bit=0 for NT, 1 for Win32s
#endif
***/
  switch (wx_style)
  {
    case wxDOT:
      cstyle = PS_DOT;
      break;
    case wxSHORT_DASH:
    case wxLONG_DASH:
      cstyle = PS_DASH;
      break;
    case wxTRANSPARENT:
      cstyle = PS_NULL;
      break;
    case wxUSER_DASH:
      // User dash style not supported on Win3.1, sorry...
#ifdef WIN32
      // Win32s doesn't have PS_USERSTYLE
/***
      if ((high&0x8000)==0)
***/
      if (wxGetOsVersion()==wxWINDOWS_NT)
        cstyle = PS_USERSTYLE ;
      else
        cstyle = PS_DOT ; // We must make a choice... This is mine!
#else
      cstyle = PS_DASH ;
#endif
      break ;
    case wxSOLID:
    default:
      cstyle = PS_SOLID;
      break;
  }
  return cstyle;
}


// Brushes
wxBrush::wxBrush(void)
{
  style = wxSOLID;
  stipple = NULL ;
  cbrush = NULL;
  my_old_cbrush = NULL ;
  old_color = 0 ;
  old_style = -1 ;
  old_stipple = NULL ;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBrushList->AddBrush(this);
#endif
}

wxBrush::~wxBrush()
{
  if (my_old_cbrush)
  {
    DeleteObject(my_old_cbrush);
  }
  my_old_cbrush = NULL ;

  if (cbrush)
  {
    DeleteObject(cbrush);
  }
  cbrush = NULL ;

  wxTheBrushList->RemoveBrush(this);
}

wxBrush::wxBrush(wxColour& col, int Style)
{
  colour = col;
  style = Style;
  stipple = NULL ;
  cbrush = NULL;
  my_old_cbrush = NULL;
  old_color = 0 ;
  old_style = -1 ;
  old_stipple = NULL ;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBrushList->AddBrush(this);
#endif
}

Bool wxBrush::RealizeResource(void) 
{
  if (cbrush == 0)
  {
    if (style==wxTRANSPARENT)
    {
      cbrush = ::GetStockObject(NULL_BRUSH);
      return TRUE;
    }
    COLORREF ms_colour = 0 ;

    ms_colour = colour.pixel ;

    switch (style)
    {
/****
    // Don't reset cbrush, wxTRANSPARENT is handled by wxBrush::SelectBrush()
    // this could save (many) time if frequently switching from
    // wxSOLID to wxTRANSPARENT, because Create... is not always called!!
    //
    // NB August 95: now create and select a Null brush instead.
    // This could be optimized as above.
    case wxTRANSPARENT:
      cbrush = NULL;  // Must always select a suitable background brush
                      // - could choose white always for a quick solution
      break;
***/
      case wxBDIAGONAL_HATCH:
        cbrush = CreateHatchBrush(HS_BDIAGONAL,ms_colour) ;
        break ;
      case wxCROSSDIAG_HATCH:
        cbrush = CreateHatchBrush(HS_DIAGCROSS,ms_colour) ;
        break ;
      case wxFDIAGONAL_HATCH:
        cbrush = CreateHatchBrush(HS_FDIAGONAL,ms_colour) ;
        break ;
      case wxCROSS_HATCH:
        cbrush = CreateHatchBrush(HS_CROSS,ms_colour) ;
        break ;
      case wxHORIZONTAL_HATCH:
        cbrush = CreateHatchBrush(HS_HORIZONTAL,ms_colour) ;
        break ;
      case wxVERTICAL_HATCH:
        cbrush = CreateHatchBrush(HS_VERTICAL,ms_colour) ;
        break ;
      case wxSTIPPLE:
        if (stipple)
          cbrush = CreatePatternBrush(stipple->ms_bitmap) ;
        else
          cbrush = CreateSolidBrush(ms_colour) ;
        break ;
      case wxSOLID:
      default:
        cbrush = CreateSolidBrush(ms_colour) ;
        break;
    }
#ifdef DEBUG_CREATE
    if (cbrush==NULL) wxError("Cannot create brush","Internal error") ;
#endif
    return TRUE;
  }
  else
    return FALSE;
}

HANDLE wxBrush::GetResourceHandle(void)
{
  return cbrush;
}

Bool wxBrush::FreeResource(void)
{
  if ((GetResourceUsage() == 0) && (cbrush != 0))
  {
    DeleteObject(cbrush);
    cbrush = 0;
    return TRUE;
  }
  else return FALSE;
}

Bool wxBrush::UseResource(void)
{
  IncrementResourceUsage();
  return TRUE;
}

Bool wxBrush::ReleaseResource(void)
{
  DecrementResourceUsage();
  return TRUE;
}

Bool wxBrush::IsFree(void)
{
  return (cbrush == 0);
}

void wxBrush::SetColour(wxColour& col)
{
  wxbBrush::SetColour(col);
  if (FreeResource())
    RealizeResource();
}

void wxBrush::SetColour(const char *col)
{
  wxbBrush::SetColour(col);
  if (FreeResource())
    RealizeResource();
}

void wxBrush::SetColour(char r, char g, char b)
{
  wxbBrush::SetColour(r, g, b);
  if (FreeResource())
    RealizeResource();
}

void wxBrush::SetStyle(int style)
{
  wxbBrush::SetStyle(style);
  if (FreeResource())
    RealizeResource();
}

void wxBrush::SetStipple(wxBitmap* stipple)
{
  wxbBrush::SetStipple(stipple);
  if (FreeResource())
    RealizeResource();
}

#if 0
HBRUSH wxBrush::SelectBrush(HDC dc)
{
  HBRUSH prev_brush ;

  if (cbrush && style!=wxTRANSPARENT)
  {
    prev_brush = ::SelectObject(dc, cbrush);
  }
  else
  {
    HBRUSH nullBrush = ::GetStockObject(NULL_BRUSH);
    prev_brush = ::SelectObject(dc, nullBrush);
  }
  if (my_old_cbrush)
  {
    DeleteObject(my_old_cbrush) ;
  }
  my_old_cbrush = NULL ;
  return(prev_brush) ;

}
#endif

wxBrush::wxBrush(const char *col, int Style)
{
  colour = col;
  style = Style;
  stipple = NULL ;
  cbrush = NULL;
  my_old_cbrush = NULL;
  old_color = 0 ;
  old_style = -1 ;
  old_stipple = NULL ;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBrushList->AddBrush(this);
#endif
}

// Icons
wxIcon::wxIcon(void)
{
  WXSET_TYPE(wxIcon, wxTYPE_ICON)
  
  width = 0;
  height = 0;
  ms_icon = NULL ;
  ok = FALSE;
//  wxTheIconList->Append(this);
}

wxIcon::wxIcon(char WXUNUSED(bits)[], int WXUNUSED(width), int WXUNUSED(height))
{
  WXSET_TYPE(wxIcon, wxTYPE_ICON)
  
  width = 0;
  height = 0;
  ms_icon = NULL ;
  ok = FALSE;
//  wxTheIconList->Append(this);
}

wxIcon::wxIcon(const char *icon_file, long flags)
{
  WXSET_TYPE(wxIcon, wxTYPE_ICON)
  
  width = 0 ;
  height = 0 ;
  LoadFile((char *)icon_file, flags);
//  wxTheIconList->Append(this);
}

wxIcon::~wxIcon(void)
{
  if (ms_icon)
    DestroyIcon(ms_icon);

//  wxTheIconList->DeleteObject(this);
}

Bool wxIcon::LoadFile(char *icon_file, long flags)
{
  WXSET_TYPE(wxIcon, wxTYPE_ICON)
  
  ok = FALSE;
  width = 0;
  height = 0;
  depth = 0;

  if (flags & wxBITMAP_TYPE_ICO_RESOURCE)
  {
    ms_icon = LoadIcon(wxhInstance, icon_file);
#ifdef WIN32
/***
    DWORD vers = GetVersion() ;
    WORD  high = HIWORD(vers) ; // high bit=0 for NT, 1 for Win32s
    // Win32s doesn't have GetIconInfo function...
    if (ms_icon && (high&0x8000)==0 )
***/
    if (ms_icon && wxGetOsVersion()==wxWINDOWS_NT)
    {
      ICONINFO info ;
      if (GetIconInfo(ms_icon,&info))
      {
        HBITMAP ms_bitmap = info.hbmMask ;
        if (ms_bitmap)
        {
          BITMAP bm;
          GetObject(ms_bitmap, sizeof(BITMAP), (LPSTR) &bm);
          width = bm.bmWidth;
          height = bm.bmHeight;
        }
        if (info.hbmMask)
          DeleteObject(info.hbmMask) ;
        if (info.hbmColor)
          DeleteObject(info.hbmColor) ;
      }
    }
#else
    width = 32;
    height = 32;
#endif
    ok = (ms_icon != 0);
    return ok;
  }
#if USE_RESOURCE_LOADING_IN_MSW
  else if (flags & wxBITMAP_TYPE_ICO)
  {
    ms_icon = ReadIconFile(icon_file, wxhInstance, &width, &height);
    ok = (ms_icon != 0);
    return ok;
  }
#endif
  else if (flags & wxBITMAP_TYPE_BMP)
  {
#if USE_RESOURCE_LOADING_IN_MSW
   /* This doesn't work: just gives us a grey square. Ideas, anyone?
    * (MakeIconFromBitmap doesn't work. I pinched it from MakeCursorFromBitmap
    * in curico.cc so it probably doesn't deal with colour or something.)
    */
    HBITMAP hBitmap = 0;
    HPALETTE hPalette = 0;
    Bool success = ReadDIB((char *)icon_file, &hBitmap, &hPalette);
    if (!success)
      return FALSE;
    if (hPalette)
      DeleteObject(hPalette);
    ms_icon = MakeIconFromBitmap(wxhInstance, hBitmap);
    DeleteObject(hBitmap);
    if (ms_icon)
    {
      ok = TRUE;
      return TRUE;
    }
#endif
  }
  else
  {
    wxError("Unimplemented icon type");
    return FALSE;
  }
  return FALSE;
}

// Cursors
wxCursor::wxCursor(void)
{
  WXSET_TYPE(wxCursor, wxTYPE_CURSOR)
  
  width = 32; height = 32;
  ms_cursor = NULL ;
  destroyCursor = FALSE;
//  wxTheCursorList->Append(this) ;
}

wxCursor::wxCursor(char WXUNUSED(bits)[], int WXUNUSED(width), int WXUNUSED(height))
{
  WXSET_TYPE(wxCursor, wxTYPE_CURSOR)
  
  ms_cursor = NULL ;
  destroyCursor = FALSE;
//  wxTheCursorList->Append(this) ;
}

wxCursor::wxCursor(const char *cursor_file, long flags, int hotSpotX, int hotSpotY)
{
  WXSET_TYPE(wxCursor, wxTYPE_CURSOR)
  
  destroyCursor = FALSE;
  ms_cursor = 0;
  ok = FALSE;
  if (flags & wxBITMAP_TYPE_CUR_RESOURCE)
  {
    ms_cursor = LoadCursor(wxhInstance, cursor_file);
    if (ms_cursor)
      ok = TRUE;
    else
      ok = FALSE;
  }
  else if (flags & wxBITMAP_TYPE_CUR)
  {
#if USE_RESOURCE_LOADING_IN_MSW
    ms_cursor = ReadCursorFile((char *)cursor_file, wxhInstance, &width, &height);
#endif
  }
  else if (flags & wxBITMAP_TYPE_ICO)
  {
#if USE_RESOURCE_LOADING_IN_MSW
    ms_cursor = IconToCursor((char *)cursor_file, wxhInstance, hotSpotX, hotSpotY, &width, &height);
#endif
  }
  else if (flags & wxBITMAP_TYPE_BMP)
  {
#if USE_RESOURCE_LOADING_IN_MSW
    HBITMAP hBitmap = 0;
    HPALETTE hPalette = 0;
    Bool success = ReadDIB((char *)cursor_file, &hBitmap, &hPalette);
    if (!success)
      return;
    if (hPalette)
      DeleteObject(hPalette);
    POINT pnt;
    pnt.x = hotSpotX;
    pnt.y = hotSpotY;
    ms_cursor = MakeCursorFromBitmap(wxhInstance, hBitmap, &pnt);
    DeleteObject(hBitmap);
    if (ms_cursor)
      ok = TRUE;
#endif
  }
//  wxTheCursorList->Append(this) ;
}

// Cursors by stock number
wxCursor::wxCursor(int cursor_type)
{
  WXSET_TYPE(wxCursor, wxTYPE_CURSOR)
  
  switch (cursor_type)
  {
    case wxCURSOR_WAIT:
      ms_cursor = LoadCursor(NULL, IDC_WAIT);
      break;
    case wxCURSOR_IBEAM:
      ms_cursor = LoadCursor(NULL, IDC_IBEAM);
      break;
    case wxCURSOR_CROSS:
      ms_cursor = LoadCursor(NULL, IDC_CROSS);
      break;
    case wxCURSOR_SIZENWSE:
      ms_cursor = LoadCursor(NULL, IDC_SIZENWSE);
      break;
    case wxCURSOR_SIZENESW:
      ms_cursor = LoadCursor(NULL, IDC_SIZENESW);
      break;
    case wxCURSOR_SIZEWE:
      ms_cursor = LoadCursor(NULL, IDC_SIZEWE);
      break;
    case wxCURSOR_SIZENS:
      ms_cursor = LoadCursor(NULL, IDC_SIZENS);
      break;
    case wxCURSOR_CHAR:
    {
      ms_cursor = LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_HAND:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_HAND");
      break;
    }
    case wxCURSOR_BULLSEYE:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_BULLSEYE");
      break;
    }
    case wxCURSOR_PENCIL:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_PENCIL");
      break;
    }
    case wxCURSOR_MAGNIFIER:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_MAGNIFIER");
      break;
    }
    case wxCURSOR_NO_ENTRY:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_NO_ENTRY");
      break;
    }
    case wxCURSOR_LEFT_BUTTON:
    {
      ms_cursor = LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_RIGHT_BUTTON:
    {
      ms_cursor = LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_MIDDLE_BUTTON:
    {
      ms_cursor = LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_SIZING:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_SIZING");
      break;
    }
    case wxCURSOR_WATCH:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_WATCH");
      break;
    }
    case wxCURSOR_SPRAYCAN:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_ROLLER");
      break;
    }
    case wxCURSOR_PAINT_BRUSH:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_PBRUSH");
      break;
    }
    case wxCURSOR_POINT_LEFT:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_PLEFT");
      break;
    }
    case wxCURSOR_POINT_RIGHT:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_PRIGHT");
      break;
    }
    case wxCURSOR_QUESTION_ARROW:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_QARROW");
      break;
    }
    case wxCURSOR_BLANK:
    {
      ms_cursor = LoadCursor(wxhInstance, "wxCURSOR_BLANK");
      break;
    }
    default:
    case wxCURSOR_ARROW:
      ms_cursor = LoadCursor(NULL, IDC_ARROW);
      break;
  }
//  wxTheCursorList->Append(this) ;
}

wxCursor::~wxCursor(void)
{
//  wxTheCursorList->DeleteObject(this) ;
}

// Global cursor setting
void wxSetCursor(wxCursor *cursor)
{
  if (cursor && cursor->ms_cursor)
    ::SetCursor(cursor->ms_cursor);

  // This will cause big reentrancy problems if wxFlushEvents is implemented.
//  wxFlushEvents();
}

// Misc. functions

// Return TRUE if we have a colour display
Bool wxColourDisplay(void)
{
  HDC dc = ::GetDC(NULL);
  Bool flag;
  int noCols = GetDeviceCaps(dc, NUMCOLORS);
  if ((noCols == -1) || (noCols > 2))
    flag = TRUE;
  else
    flag = FALSE;
  ReleaseDC(NULL, dc);
  return flag;
}

// Returns depth of screen
int wxDisplayDepth(void)
{
  HDC dc = ::GetDC(NULL);
  int planes = GetDeviceCaps(dc, PLANES);
  int bitsPerPixel = GetDeviceCaps(dc, BITSPIXEL);
  int depth = planes*bitsPerPixel;
  ReleaseDC(NULL, dc);
  return depth;
}

// Get size of display
void wxDisplaySize(int *width, int *height)
{
  HDC dc = ::GetDC(NULL);
  *width = GetDeviceCaps(dc, HORZRES); *height = GetDeviceCaps(dc, VERTRES);
  ReleaseDC(NULL, dc);
}

wxBitmap::wxBitmap(void)
{
  WXSET_TYPE(wxBitmap, wxTYPE_BITMAP)
  
  ok = FALSE;
  width = 0;
  height = 0;
  depth = 0;
  ms_bitmap = NULL ;
  selectedInto = NULL;
  numColors = 0;
  bitmapColourMap = NULL;
  bitmapMask = NULL;
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBitmapList->Append(this);
#endif
}

wxBitmap::wxBitmap(char bits[], int the_width, int the_height, int no_bits)
{
  WXSET_TYPE(wxBitmap, wxTYPE_BITMAP)
  
  width = the_width ;
  height = the_height ;
  depth = no_bits ;
  numColors = 0;
  bitmapColourMap = NULL;
  bitmapMask = NULL;

  ms_bitmap = CreateBitmap(the_width, the_height, no_bits, 1, bits);

  if (ms_bitmap)
    ok = TRUE;
  else
    ok = FALSE;

  selectedInto = NULL;
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBitmapList->Append(this);
#endif
}

wxBitmap::wxBitmap(int w, int h, int d)
{
  WXSET_TYPE(wxBitmap, wxTYPE_BITMAP)
  
  ok = FALSE;
  width = w;
  height = h;
  depth = d;
  numColors = 0;
  selectedInto = NULL;
  bitmapColourMap = NULL;
  bitmapMask = NULL;

  (void)Create(w, h, d);
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBitmapList->Append(this);
#endif
}

wxBitmap::wxBitmap(char *bitmap_file, long flags)
{
  WXSET_TYPE(wxBitmap, wxTYPE_BITMAP)
  
  ok = FALSE;
  width = 0;
  height = 0;
  depth = 0;
  selectedInto = NULL;
  numColors = 0;
  bitmapColourMap = NULL;
  bitmapMask = NULL;

  LoadFile(bitmap_file, (int)flags);
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBitmapList->Append(this);
#endif
}

#if USE_XPM_IN_MSW
// Create from data
wxBitmap::wxBitmap(char **data, wxItem *WXUNUSED(anItem))
{
  WXSET_TYPE(wxBitmap, wxTYPE_BITMAP)
  
  selectedInto = NULL;
  bitmapColourMap = NULL;
  bitmapMask = NULL;

  XImage *ximage = 0;
  XImage *xmask = 0;
  int     ErrorStatus;
  XpmAttributes xpmAttr;
  HDC     dc;

  ok = FALSE;
  numColors = 0;

  dc = CreateCompatibleDC(NULL);	/* memory DC */

  if (dc)
  {
    xpmAttr.valuemask = XpmReturnInfos;	/* get infos back */
    ErrorStatus = XpmCreateImageFromData(&dc, data,
         &ximage, &xmask, &xpmAttr);

    if (ErrorStatus == XpmSuccess)
    {
      /* ximage is malloced and contains bitmap and attributes */
      ms_bitmap = ximage->bitmap;
      if ( xmask && xmask->bitmap )
      {
        bitmapMask = new wxMask;
        bitmapMask->SetMaskBitmap(xmask->bitmap);
      }

      BITMAP  bm;
      GetObject(ms_bitmap, sizeof(bm), (LPSTR) & bm);

      width = (bm.bmWidth);
      height = (bm.bmHeight);
      depth = (bm.bmPlanes * bm.bmBitsPixel);
      numColors = xpmAttr.npixels;
      XpmFreeAttributes(&xpmAttr);

      XImageFree(ximage);	// releases the malloc, but does not detroy
	  XImageFree(xmask);	// the bitmap
      ok = TRUE;

    } else
    {
      ok = False;
//  XpmDebugError(ErrorStatus, NULL);
    }
    DeleteDC(dc);
  }
}
#endif

Bool wxBitmap::Create(int w, int h, int d)
{
  width = w;
  height = h;
  depth = d;

  if (d > 0)
  {
    ms_bitmap = CreateBitmap(w, h, 1, d, NULL);
  }
  else
  {
    HDC dc = GetDC(NULL);
    ms_bitmap = CreateCompatibleBitmap(dc, w, h);
    ReleaseDC(NULL, dc);
    depth = wxDisplayDepth();
  }
  if (ms_bitmap)
    ok = TRUE;
  else
    ok = FALSE;
  return ok;
}

Bool wxBitmap::LoadFile(char *bitmap_file, long flags)  
{
  ok = FALSE;
  width = 0;
  height = 0;
  depth = 0;
  selectedInto = NULL;

  if (flags & wxBITMAP_TYPE_BMP_RESOURCE)
  {
    ms_bitmap = LoadBitmap(wxhInstance, bitmap_file);
    if (ms_bitmap)
    {
      ok = TRUE;
      BITMAP bm;
      GetObject(ms_bitmap, sizeof(BITMAP), (LPSTR) &bm);
      width = bm.bmWidth;
      height = bm.bmHeight;
      depth = bm.bmPlanes;
      return TRUE;
    }
  }
#if USE_XPM_IN_MSW
  else if (flags & wxBITMAP_TYPE_XPM)
  {
    XImage *ximage = 0;
	XImage *xmask = 0;
    XpmAttributes xpmAttr;
    HDC     dc;

    ok = False;
    dc = CreateCompatibleDC(NULL);
    if (dc)
    {
      xpmAttr.valuemask = XpmReturnPixels;
      int errorStatus = XpmReadFileToImage(&dc, bitmap_file, &ximage, &xmask, &xpmAttr);
      DeleteDC(dc);
      if (errorStatus == XpmSuccess)
      {
	    ms_bitmap = ximage->bitmap;
        if ( xmask && xmask->bitmap )
        {
          bitmapMask = new wxMask;
          bitmapMask->SetMaskBitmap(xmask->bitmap);
        }

	    BITMAP  bm;
	    GetObject(ms_bitmap, sizeof(bm), (LPSTR) & bm);

	    width = (bm.bmWidth);
	    height = (bm.bmHeight);
	    depth = (bm.bmPlanes * bm.bmBitsPixel);
	    numColors = xpmAttr.npixels;
        XpmFreeAttributes(&xpmAttr);
	    XImageFree(ximage);
	    XImageFree(xmask);
	
	    ok = TRUE;
	    return TRUE;
      }
      else
      {
        ok = FALSE;
        return FALSE;
      }
    }
  }
#endif
#if USE_IMAGE_LOADING_IN_MSW
  else if ((flags & wxBITMAP_TYPE_BMP) || (flags & wxBITMAP_TYPE_ANY))
  {
    wxColourMap *cmap = NULL;
    Bool success = FALSE;
    if (flags & wxBITMAP_DISCARD_COLOURMAP)
      success = wxLoadIntoBitmap(bitmap_file, this);
    else
      success = wxLoadIntoBitmap(bitmap_file, this, &cmap);
    if (!success && cmap)
    {
      delete cmap;
      cmap = NULL;
    }
    if (cmap)
      bitmapColourMap = cmap;
    return success;
  }
#endif
  return FALSE;
}

wxBitmap::~wxBitmap(void)
{
  if (selectedInto)
  {
    char buf[200];
    sprintf(buf, "Bitmap %X was deleted without selecting out of wxMemoryDC %X.", this, selectedInto);
    wxFatalError(buf);
  }
  if (ms_bitmap)
  {
    DeleteObject(ms_bitmap);
  }
  ms_bitmap = NULL ;

  if (bitmapColourMap)
    delete bitmapColourMap;

  if ( bitmapMask )
    delete bitmapMask;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxTheBitmapList->DeleteObject(this);
#endif
}

Bool wxBitmap::SaveFile(char *filename, int typ, wxColourMap *cmap)
{
  switch (typ)
  {
#if USE_IMAGE_LOADING_IN_MSW
    case wxBITMAP_TYPE_BMP:
    {
      wxColourMap *actualCmap = cmap;
      if (!actualCmap)
        actualCmap = bitmapColourMap;
      return wxSaveBitmap(filename, this, actualCmap);
      break;
    }
#endif
#if USE_XPM_IN_MSW
    case wxBITMAP_TYPE_XPM:
    {
      HDC     dc = NULL;

      Visual *visual = NULL;
      XImage  ximage, xmask;

      dc = CreateCompatibleDC(NULL);
      if (dc)
      {
        if (SelectObject(dc, ms_bitmap))
        { /* for following SetPixel */
          /* fill the XImage struct 'by hand' */
	      ximage.width = width; ximage.height = height;
	      ximage.depth = depth; ximage.bitmap = ms_bitmap;
	      xmask.width = width; xmask.height = height;
	      xmask.depth = depth; xmask.bitmap = bitmapMask ? bitmapMask->GetMaskBitmap() : 0 ;
	      int errorStatus = XpmWriteFileFromImage(&dc, filename,
						  &ximage, (bitmapMask ? (&xmask) : (XImage *)NULL), (XpmAttributes *) NULL);

          if (dc)
	        DeleteDC(dc);

	  if (errorStatus == XpmSuccess)
	    return TRUE;		/* no error */
	  else
	    return FALSE;
        } else return FALSE;
      } else return FALSE;
      break;
    }
#endif
    default:
      break;
  }
  return FALSE;
}

// wxMask

wxMask::wxMask(void)
{
    m_maskBitmap = 0;
}

// Construct a mask from a bitmap and a colour indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
    // TODO
    m_maskBitmap = 0;
}

// Construct a mask from a bitmap and a palette index indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, const int paletteIndex)
{
    // TODO
    m_maskBitmap = 0;
}

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask(const wxBitmap& bitmap)
{
    // TODO
    m_maskBitmap = 0;
}

wxMask::~wxMask(void)
{
    if ( m_maskBitmap )
        ::DeleteObject(m_maskBitmap);
}

// Return the system colour for a particular GUI element
wxColour wxGetSysColour(int sysColour)
{
  // Windows colour ids the same as wxWindows ones, so no translation.
  COLORREF rgb = GetSysColor(sysColour);
  
  wxColour col(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
  return col;
}
