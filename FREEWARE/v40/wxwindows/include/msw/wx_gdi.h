/*
 * File:	wx_gdi.h
 * Purpose:	Declaration of various graphics objects - fonts, pens, icons etc.
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_gdi.h	1.2 5/9/94" */


#ifndef wx_gdih
#define wx_gdih

#include "wb_gdi.h"

#ifdef IN_CPROTO
typedef       void    *wxFont ;
typedef       void    *wxColourMap;
typedef       void    *wxPen;
typedef       void    *wxBrush;
typedef       void    *wxIcon;
typedef       void    *wxCursor;
typedef       void    *wxBitmap;
#else

// Font
class WXDLLEXPORT wxFont: public wxbFont
{
  DECLARE_DYNAMIC_CLASS(wxFont)

 public:
  HFONT cfont;

  wxFont(void);
  wxFont(int PointSize, int Family, int Style, int Weight, Bool underlined = FALSE, const char *Face = NULL);
  ~wxFont(void);
  Bool Create(int PointSize, int Family, int Style, int Weight, Bool underlined = FALSE, const char *Face = NULL);

  // Internal
  Bool RealizeResource(void);
  HANDLE GetResourceHandle(void) ;
  Bool FreeResource(void);
  Bool UseResource(void);
  Bool ReleaseResource(void);
  Bool IsFree(void);
};

class WXDLLEXPORT wxColourMap: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxColourMap)

 public:
 HPALETTE ms_palette;
  wxColourMap(void);
  wxColourMap(const int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
  ~wxColourMap(void);
  Bool Create(const int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
  int GetPixel(const unsigned char red, const unsigned char green, const unsigned char blue);
  Bool GetRGB(const int pixel, unsigned char *red, unsigned char *green, unsigned char *blue);
};

#define wxColorMap wxColourMap

// Pen
class WXDLLEXPORT wxPen: public wxbPen
{
  DECLARE_DYNAMIC_CLASS(wxPen)

 public:
  int old_width;
  int old_style;
  int old_join ;
  int old_cap ;
  int old_nb_dash ;
  wxDash *old_dash ;
  wxBitmap *old_stipple ;
  COLORREF old_color ;

  HPEN cpen;
  HPEN my_old_cpen ;

  wxPen(void);
  wxPen(wxColour& col, int width, int style);
  wxPen(const char *col, int width, int style);
  ~wxPen(void);

  // Override in order to recreate the pen
  virtual void SetColour(wxColour& col) ;
  virtual void SetColour(const char *col)  ;
  virtual void SetColour(char r, char g, char b)  ;

  virtual void SetWidth(int width)  ;
  virtual void SetStyle(int style)  ;
  virtual void SetStipple(wxBitmap *stipple)  ;
  virtual void SetDashes(int nb_dashes, wxDash *dash)  ;
  virtual void SetJoin(int join)  ;
  virtual void SetCap(int cap)  ;

  // Internal
  Bool RealizeResource(void);
  HANDLE GetResourceHandle(void) ;
  Bool FreeResource(void);
  Bool UseResource(void);
  Bool ReleaseResource(void);
  Bool IsFree(void);
};

int wx2msPenStyle(int wx_style);

// Brush
class WXDLLEXPORT wxBrush: public wxbBrush
{
  DECLARE_DYNAMIC_CLASS(wxBrush)

 public:
  HBRUSH cbrush;
  HBRUSH my_old_cbrush ;
  int old_style;
  wxBitmap *old_stipple ;
  COLORREF old_color ;

  wxBrush(void);
  wxBrush(wxColour& col, int style);
  wxBrush(const char *col, int style);
  ~wxBrush(void);

  virtual void SetColour(wxColour& col)  ;
  virtual void SetColour(const char *col)  ;
  virtual void SetColour(char r, char g, char b)  ;
  virtual void SetStyle(int style)  ;
  virtual void SetStipple(wxBitmap* stipple=NULL)  ;

  // Internal
  Bool RealizeResource(void);
  HANDLE GetResourceHandle(void) ;
  Bool FreeResource(void);
  Bool UseResource(void);
  Bool ReleaseResource(void);
  Bool IsFree(void);
};

// Bitmap
class wxDC;
class wxItem;
class wxBitmap;

// New: a mask is a mono bitmap used for drawing bitmaps
// transparently.
class WXDLLEXPORT wxMask: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxMask)

public:
  wxMask(void);

  // Construct a mask from a bitmap and a colour indicating
  // the transparent area
  wxMask(const wxBitmap& bitmap, const wxColour& colour);

  // Construct a mask from a bitmap and a palette index indicating
  // the transparent area
  wxMask(const wxBitmap& bitmap, const int paletteIndex);

  // Construct a mask from a mono bitmap (copies the bitmap).
  wxMask(const wxBitmap& bitmap);

  ~wxMask(void);

  inline HBITMAP GetMaskBitmap(void) { return m_maskBitmap; }
  inline void SetMaskBitmap(HBITMAP bmp) { m_maskBitmap = bmp; }
protected:
  HBITMAP m_maskBitmap;
};

class WXDLLEXPORT wxBitmap: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxBitmap)

 protected:
  int width;
  int height;
  int depth;
  Bool ok;
  int numColors;
  wxColourMap *bitmapColourMap;
  wxMask *bitmapMask;
 public:
  HBITMAP ms_bitmap;
  wxDC *selectedInto; // So bitmap knows whether it's been selected into
                      // a device context (for error checking)

  wxBitmap(void); // Platform-specific

  // Initialize with raw data
  wxBitmap(char bits[], int width, int height, int depth = 1);

#if USE_XPM_IN_MSW
  // Initialize with XPM data
  wxBitmap(char **data, wxItem *anItem = NULL);
#endif

  // Load a file or resource
  wxBitmap(char *name, long flags = wxBITMAP_DISCARD_COLOURMAP | wxBITMAP_TYPE_BMP_RESOURCE);

  // If depth is omitted, will create a bitmap compatible with the display
  wxBitmap(int width, int height, int depth = -1);
  ~wxBitmap(void);

  virtual Bool Create(int width, int height, int depth = -1);
  virtual Bool LoadFile(char *name, long flags = wxBITMAP_DISCARD_COLOURMAP | wxBITMAP_TYPE_BMP_RESOURCE);
  virtual Bool SaveFile(char *name, int type, wxColourMap *cmap = NULL);

  inline Bool Ok(void) { return ok; }
  inline int GetWidth(void) { return width; }
  inline int GetHeight(void) { return height; }
  inline int GetDepth(void) { return depth; }
  inline void SetWidth(int w) { width = w; }
  inline void SetHeight(int h) { height = h; }
  inline void SetDepth(int d) { depth = d; }
  inline void SetOk(Bool isOk) { ok = isOk; }
  inline wxColourMap *GetColourMap(void) { return bitmapColourMap; }
  inline void SetColourMap(wxColourMap *cmap) { bitmapColourMap = cmap ; }
  inline wxMask *GetMask(void) { return bitmapMask; }
  inline void SetMask(wxMask *mask) { bitmapMask = mask ; }
};

// Icon
class WXDLLEXPORT wxIcon: public wxBitmap
{
  DECLARE_DYNAMIC_CLASS(wxIcon)

 public:
  HICON ms_icon;
  wxIcon(void);
  wxIcon(char bits[], int width, int height);
  wxIcon(const char *name, long flags = wxBITMAP_DISCARD_COLOURMAP | wxBITMAP_TYPE_ICO_RESOURCE);
  Bool LoadFile(char *name, long flags = wxBITMAP_DISCARD_COLOURMAP | wxBITMAP_TYPE_ICO_RESOURCE);
  ~wxIcon(void);
};

// Cursor
class WXDLLEXPORT wxCursor: public wxBitmap
{
  DECLARE_DYNAMIC_CLASS(wxCursor)

 public:
  HCURSOR ms_cursor;
  Bool destroyCursor;
  wxCursor(void);
  wxCursor(char bits[], int width, int height);
  wxCursor(const char *name, long flags = wxBITMAP_DISCARD_COLOURMAP | wxBITMAP_TYPE_CUR_RESOURCE,
   int hotSpotX = 0, int hotSpotY = 0);
  wxCursor(int cursor_type);
  ~wxCursor(void);
};


#endif // IN_CPROTO
#endif // wx_gdih
