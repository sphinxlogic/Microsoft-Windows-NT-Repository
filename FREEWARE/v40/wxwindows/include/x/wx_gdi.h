/*
 * File:	wx_gdi.h
 * Purpose:	Declaration of various graphics objects - fonts, pens, icons etc.
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   28 May 97   12:06 pm
 */

/* sccsid[] = "@(#)wx_gdi.h	1.2 5/9/94" */


#ifndef wx_gdih
#define wx_gdih

#ifdef __GNUG__
#pragma interface
#endif

#include "wb_gdi.h"

#ifdef wx_motif
#include <Xm/Xm.h>
#endif

#ifdef wx_xview
#include <xview/font.h>
#include <xview/icon.h>
#include <xview/cursor.h>
#endif

#ifdef IN_CPROTO
typedef       void    *wxFont ;
typedef       void    *wxColourMap;
typedef       void    *wxPen;
typedef       void    *wxBrush;
typedef       void    *wxIcon;
typedef       void    *wxCursor;
typedef       void    *wxBitmap;
typedef       void    *XFontInfo;
typedef       void    *XFontPool;
#else

#ifdef wx_motif
/* MATTHEW: [4] Font for one display */
class wxXFont : public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxXFont)

 public:
  ~wxXFont();

  Display *display;
  XFontStruct *xFont;
  XmFontList  mFont;
};
#endif

// Font
class wxFont: public wxbFont
{
  DECLARE_DYNAMIC_CLASS(wxFont)

 public:
#ifdef wx_motif
  wxList xfonts; /* MATTHEW: [4] to handle multiple displays */
/*
  XFontStruct *xFont;
  XmFontList  mFont ;
*/
#endif
#ifdef wx_xview
  Xv_Font x_font;
#endif

  wxFont(void);
  wxFont(int PointSize, int Family, int Style, int Weight, Bool underlined = FALSE, const char *Face = NULL);
  ~wxFont(void);
  /* Display & optional XFontStruct pointer */
#ifdef wx_motif
  XmFontList GetInternalFont(Display *, XFontStruct **s = NULL); 
#endif
#ifdef wx_xview
  Xv_Font GetInternalFont(Display *, XFontStruct **s = NULL);
#endif
};

#ifdef wx_motif
/* MATTHEW: [4] Colormap for one display */
class wxXColormap : public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxXColormap)

 public:
  Display *display;
  int pix_array_n;
  unsigned long *pix_array;
  Colormap cmap;
  Bool destroyable;
};
#endif

class wxColourMap: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxColourMap)

 public:
#ifdef wx_xview
  Bool destroyable;
  Colormap cmap;
  unsigned long *pix_array;
  int pix_array_n;
#endif
#ifdef wx_motif
  wxList xcolormaps; /* display-specific colormaps */
#endif

  wxColourMap(void);
  ~wxColourMap(void);
  Bool Create(const int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
  Bool TransferBitmap(void *data, int depth, int size);
  Bool TransferBitmap8(unsigned char *data, unsigned long size, void *dest, unsigned int bpp);

  int GetPixel(const unsigned char red, const unsigned char green, const unsigned char blue);
  Bool GetRGB(const int pixel, unsigned char *red, unsigned char *green, unsigned char *blue);

  /* Get a real colormap */
  Colormap GetXColormap(Display *display);
  unsigned long *GetXPixArray(Display *display, int *pix_array_n);

  /* Put a real colormap */
  void PutXColormap(Display *display, Colormap cmap, Bool destroyable);
};

#define wxColorMap wxColourMap

/* main colormap depends on the display */
extern Colormap wxGetMainColormap(Display *display);

// Pen
class wxPen: public wxbPen
{
  DECLARE_DYNAMIC_CLASS(wxPen)

 public:
  wxPen(void);
  wxPen(wxColour& col, int width, int style);
  wxPen(char *col, int width, int style);
  ~wxPen(void);

};

// Brush
class wxBrush: public wxbBrush
{
  DECLARE_DYNAMIC_CLASS(wxBrush)

 public:
  wxBrush(void);
  wxBrush(wxColour& col, int style);
  wxBrush(char *col, int style);
  ~wxBrush(void);

};

// Bitmap
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

  inline Pixmap GetMaskBitmap(void) { return m_maskBitmap; }
  inline void SetMaskBitmap(Pixmap bmp) { m_maskBitmap = bmp; }
protected:
  Pixmap m_maskBitmap;
};

class wxItem;
class wxBitmap: public wxObject
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
  Pixmap x_pixmap;
  Bool freePixmap;
  Display *display; /* We can use multiple displays now */

  unsigned long *free_colors;
  long free_colors_num;

#ifdef wx_motif
  // This 5 variables are for wxItem
  Pixmap insensPixmap ;
  Pixmap labelPixmap ;
  Pixmap armPixmap ;
  XImage *image ;
  XImage *insensImage ;
#endif

#ifdef wx_xview
  Server_image x_image;
#endif

  wxBitmap(void) ;
  wxBitmap(char bits[], int width, int height, int depth = 1);

  // If depth is omitted, will create a bitmap compatible with the display
  wxBitmap(int width, int height, int depth = -1);

#ifdef USE_XPM_IN_MSW
  // Initialize with XPM data
  wxBitmap(char **data, wxItem *anItem = NULL);
#endif

  // Load a file or resource
  wxBitmap(char *name, long flags = wxBITMAP_DISCARD_COLOURMAP | wxBITMAP_TYPE_XBM);

  ~wxBitmap(void);

  // Create pixmap
  virtual Bool Create(int width, int height, int depth = -1);
  virtual Bool LoadFile(char *name, long flags = wxBITMAP_DISCARD_COLOURMAP | wxBITMAP_TYPE_XBM);
  virtual Bool SaveFile(char *name, int type, wxColourMap *cmap = NULL);

#ifdef wx_xview
  virtual void CreateServerImage(Bool savePixmap = TRUE);
#endif

#ifdef wx_motif
  // For wxItem
  virtual Pixmap GetLabelPixmap(Widget w) ;
  virtual Pixmap GetArmPixmap(Widget w) ;
  virtual Pixmap GetInsensPixmap(Widget w) ;
#endif
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
class wxIcon: public wxBitmap
{
  DECLARE_DYNAMIC_CLASS(wxIcon)

 public:
#ifdef wx_motif
//  int iconWidth;
//  int iconHeight;
//  Pixmap x_pixmap;
#endif
#ifdef wx_xview
  Icon x_icon;
#endif
  wxIcon(void);
  wxIcon(char bits[], int width, int height);
  wxIcon(const char *name, long flags = wxBITMAP_DISCARD_COLOURMAP | wxBITMAP_TYPE_XBM);

#ifdef USE_XPM_IN_MSW
  // Initialize with XPM data
  wxIcon(char **data);
#endif
//  Bool LoadFile(char *name, long flags = wxBITMAP_DISCARD_COLOURMAP | wxBITMAP_TYPE_XBM);

  ~wxIcon(void);
};

#ifdef wx_motif
/* Cursor for one display */
class wxXCursor : public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxXCursor)

 public:
  Display *display;
  Cursor x_cursor;
};
#endif

// Cursor
class wxCursor: public wxBitmap
{
  DECLARE_DYNAMIC_CLASS(wxCursor)

  public:
  Bool use_raw_x_cursor;
#ifdef wx_motif
  wxList xcursors; /* List of cursors and an id */
  int cursor_id;
/*
  Cursor x_cursor;
*/
#endif
#ifdef wx_xview
  Xv_Cursor x_cursor;
#endif
  /* We can use multiple displays now */
#ifdef wx_motif
  Cursor
#else
  Xv_Cursor  
#endif
     GetXCursor(Display *d);

  wxCursor(void);
  wxCursor(char bits[], int width, int height, int hotSpotX = -1, int hotSpotY = -1, char maskbits[] = NULL);
  wxCursor(const char *name, long flags = wxBITMAP_DISCARD_COLOURMAP | wxBITMAP_TYPE_XBM,
   int hotSpotX = 0, int hotSpotY = 0);
  wxCursor(int cursor_type);
  ~wxCursor(void);
};


#ifdef wx_x
// X-specific font matching mechanism
class XFontInfo: public wxObject
{
 public:
//  int family;
  int fontid;
  int style;
  int weight;
  int point_size; // Points in XView, 10ths of a point in Motif
  Bool underlined;
  Display *display; /* Use display */
#ifdef wx_xview
  Xv_Font font;
#endif
#ifdef wx_motif
  XFontStruct *font;
#endif
  XFontInfo(int the_fontid, int the_style, int the_weight, int the_point_size,
             Bool underlined, Display *display,
#ifdef wx_xview
            Xv_Font the_font);
#endif
#ifdef wx_motif
            XFontStruct *the_font);
#endif
};

class XFontPool: public wxList
{
  XFontInfo *cache;
 public:
  XFontPool(void);
  void AddFont(int fontid, int style, int weight, int point_size,
               Bool underlined, Display *display,
#ifdef wx_xview
               Xv_Font font);
#endif
#ifdef wx_motif
               XFontStruct *font);
#endif
#ifdef wx_xview
  Xv_Font
#endif
#ifdef wx_motif
  XFontStruct *
#endif
    FindFont(int fontid, int style, int weight, int point_size, Bool underlined, Display *display);

#ifdef wx_xview
  Xv_Font
#endif
#ifdef wx_motif
  XFontStruct *
#endif
    FindOrCreateFont(int fontid, int style, int weight, int point_size, Bool underlined,
        int point_size_to_store, int xres, int yres, Display *display);

#ifdef wx_xview
  Xv_Font FindNearestFont(int fontid, int style, int weight, int point_size, Bool underlined, int xres, int yres, Display *display);
#endif
#ifdef wx_motif
  XFontStruct *FindNearestFont(int fontid, int style, int weight, int point_size, Bool underlined, int xres, int yres, Display *display);
#endif
};

extern XFontPool *wxFontPool;
#endif

#endif // IN_CPROTO
#endif // wx_gdih
