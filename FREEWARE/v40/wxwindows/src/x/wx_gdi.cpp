/*
 * File:      wx_gdi.cc
 * Purpose:     GDI (Graphics Device Interface) objects and functions (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_gdi.cc,v 1.3 1994/08/14 21:28:43 edz Exp $
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "@(#)wx_gdi.cc	1.2 5/9/94";

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx_defs.h"
#include "wx_utils.h"
#include "wx_main.h"
#include "wx_gdi.h"

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#if USE_XPM_IN_X
#include "wx_item.h"
#ifdef VMS
#include "[--.contrib.wxxpm.libxpm_34b.lib]xpm34.h"
#else
extern "C" {
#include "../../contrib/wxxpm/libxpm.34b/lib/xpm34.h"
}
#endif
#endif
#if USE_IMAGE_LOADING_IN_X
#ifdef VMS
#include "[--.utils.image.src]wx_image.h"
#else
#include "../../utils/image/src/wx_image.h"
#endif
#endif

const DISPLAY_DPI = 100;

#ifdef wx_x
XFontPool *wxFontPool = NULL;
#include <X11/cursorfont.h>
#endif

#ifdef wx_motif
#include "wx_frame.h"
#endif

#ifdef wx_xview
#include <xview/screen.h>
#include <xview/cursor.h>
#include <xview/svrimage.h>
extern Xv_Server xview_server;

/* These cursors courtesy of xfig
 */

static const unsigned short bull_cursor_array[16] =
{
  0x0F00, 0x30C0, 0x4020, 0x4020, 0x8010, 0x8610, 0x8610, 0x8010,
  0x4020, 0x4020, 0x30C0, 0x0F00, 0x0000, 0x0000, 0x0000, 0x0000
};

static const unsigned short char_cursor_data[16] =
{
  0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00,
  0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00,
};

static const unsigned short crosshair_cursor_data[16] =
{
  0x1000, 0x1000, 0x1000, 0xFE00, 0x1000, 0x1000, 0x1000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

static const unsigned short magnifier_cursor_array[16] =
{
  0x0F80, 0x3060, 0x4010, 0x4010, 0x8008, 0x8008, 0x8008, 0x8008,
  0x8008, 0x4010, 0x4010, 0x3078, 0x0F9C, 0x000E, 0x0007, 0x0003,
};

static const unsigned short pencil_cursor_array[16] =
{
  0x0000, 0x0018, 0x0024, 0x0075, 0x009B, 0x0117, 0x022E, 0x045C,
  0x08B8, 0x1170, 0x22E0, 0x25C0, 0x7B80, 0x6700, 0x8600, 0x0800,
};

static const unsigned short vbar_cursor_array[16] =
{
  0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
  0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
};

static const unsigned short hand_cursor_array[16] =
{
  0x0C00, 0x1200, 0x1200, 0x1380, 0x1240, 0x7270, 0x9248, 0x924E,
  0x9249, 0x9249, 0x9009, 0x8001, 0x4002, 0x4002, 0x2004, 0x2004
};
#endif

#if !USE_IMAGE_PIXMAP_METHOD
#ifdef wx_motif
Pixmap
XCreateInsensitivePixmap( Display *display, Pixmap pixmap );
#endif
#endif

char *wxDecorativeFontName = copystring("helvetica");
char *wxRomanFontName = copystring("times");
char *wxModernFontName = copystring("courier");
char *wxSwissFontName = copystring("lucida");
char *wxScriptFontName = copystring("zapfchancery");
char *wxTeletypeFontName = copystring("lucidatypewriter");
char *wxDefaultFontName = copystring("*");

#ifdef wx_motif
/* MATTHEW: [4] Display-specific font */

wxXFont::~wxXFont()
{
  if (mFont)
    XmFontListFree (mFont);
  mFont = NULL;
}
#endif

wxFont::wxFont (void)
{
  point_size = 0;
  temporary = TRUE;
#ifdef wx_motif
/* MATTHEW: replaced with display-spicific list
  xFont = NULL;
  mFont = NULL;
*/
#endif
#ifdef wx_xview
  x_font = 0;
#endif
#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: Bad policy in general; worse in GC */
  wxTheFontList->Append (this);
#endif
}

/* Constructor for a font. Note that the real construction is done
 * in wxDC::SetFont, when information is available about scaling etc.
 */
wxFont::wxFont (int PointSize, int FontId, int Style, int Weight, Bool Underlined, const char *Face):
wxbFont (PointSize, FontId, Style, Weight, Underlined, Face)
{
  if (Face)
    faceName = copystring(Face);
  else
    faceName = NULL;
  if (Face)
  {
    fontid = wxTheFontNameDirectory.FindOrCreateFontId(Face, FontId);
    family = wxTheFontNameDirectory.GetFamily(fontid);
  }
  else
  {
    fontid = FontId;
    family = wxTheFontNameDirectory.GetFamily(FontId);
  }
  style = Style;
  weight = Weight;
  point_size = PointSize;
  underlined = Underlined;
#ifdef wx_motif
/*
  xFont = 0;
  mFont = NULL;
*/
#endif
#ifdef wx_xview
  temporary = TRUE;
  x_font = 0;
#endif

  wxTheFontList->Append (this);
}

wxFont::~wxFont ()
{
#ifdef wx_motif
  /* MATTHEW: [4] moved work to display-specific */
  xfonts.DeleteContents(TRUE);
/*
  if (mFont)
    XmFontListFree (mFont);
  mFont = NULL;
*/
#endif
  wxTheFontList->DeleteObject (this);
}

#ifdef wx_motif
/* MATTHEW: [4] Display argument, new implementation */
XmFontList wxFont::GetInternalFont (Display *display, XFontStruct **fs)
{
  wxNode *node;

  for (node = xfonts.First(); node; node = node->Next()) {
    wxXFont *xf = (wxXFont *)node->Data();
    if (xf->display == display) {
      if (fs)
	*fs = xf->xFont;
      return xf->mFont;
    }
  }

  int res_x = DISPLAY_DPI;
  int res_y = DISPLAY_DPI;
  int scaled_size = (int) (10 * point_size);

  wxXFont *xf;

  xf = new wxXFont;
  xf->display = display;
  
  xf->xFont = wxFontPool->FindNearestFont (fontid, style,
					   weight, scaled_size,
					   underlined, res_x, res_y,
					   display);
  
  xf->mFont = XmFontListCreate (xf->xFont, XmSTRING_DEFAULT_CHARSET);

  xfonts.Append(xf);

  if (fs)
    *fs = xf->xFont;

  return xf->mFont;
}
#endif

#ifdef wx_xview
/* MATTHEW: [4] Display argument and fs */
Xv_Font wxFont::GetInternalFont (Display *display, XFontStruct **fs)
{
  if (x_font == 0)
    {
      int res_x = DISPLAY_DPI;
      int res_y = DISPLAY_DPI;
      int scaled_size = (int) (10 * point_size);

      /* MATTHEW: [4] Use fontid, display */
      x_font = wxFontPool->FindNearestFont (fontid, style,
					    weight, scaled_size,
					    underlined, res_x, res_y,
					    display);
    }
  
  /* MATTHEW: [4] Fill in *fs */
  if (fs)
    *fs = (XFontStruct *)xv_get(x_font, FONT_INFO);

  return x_font;
}
#endif

#ifdef wx_x
// Pseudo-scaleable Font management under XView - a nightmare!
// - but it works o.k.
/* MATTHEW: [4] Use display */
XFontInfo::XFontInfo (int the_fontid, int the_style, int the_weight, int the_point_size,
	   Bool is_underlined, Display *a_display,
#ifdef wx_xview
	   Xv_Font the_font)
#else
	   XFontStruct * the_font)
#endif
{
  fontid = the_fontid;
  style = the_style;
  weight = the_weight;
  point_size = the_point_size;
  font = the_font;
  underlined = is_underlined;
  display = a_display; /* MATTHEW: [4] Remember display */
}

XFontPool::XFontPool (void)
{
  cache = NULL;
}


void XFontPool::
/* MATTHEW: [4] Display argument */
AddFont (int fontid, int style, int weight, int point_size,
	 Bool is_underlined, Display *display,
#ifdef wx_xview
	 Xv_Font font)
#else
	 XFontStruct * font)
#endif
{
  /* MATTHEW: [4] Use Display argument */
  XFontInfo *info = new XFontInfo (fontid, style, weight, point_size, is_underlined, display, font);
  Append (info);
}


#ifdef wx_xview
Xv_Font
#else
XFontStruct *
#endif
  XFontPool::
/* MATTHEW: [4] Display argument */
FindFont (int fontid, int style, int weight, int point_size, Bool is_underlined, Display *display)
{
  if (cache && cache->fontid == fontid && cache->style == style && cache->weight == weight
   && cache->point_size == point_size && cache->underlined == is_underlined
    && cache->display == display) /* MATTHEW: [4] Display test */
    return cache->font;

  wxNode *node = First ();
#ifdef wx_xview
  Xv_Font
#else
  XFontStruct *
#endif
    found = 0;
  while (node && !found)
    {
      XFontInfo *info = (XFontInfo *) node->Data ();
      if (info->fontid == fontid && info->style == style && info->weight == weight
      && info->point_size == point_size && info->underlined == is_underlined
	  && info->display == display) /* MATTHEW: [4] Display test */
	{
	  found = info->font;
	  cache = info;
	}
      node = node->Next ();
    }
  return found;
}

/* MATTHEW: [4] Thoroughly revied font finding code. My previous changes
   had actually messed up the serach algorithm. The search-store algorithm
   wasn't right before either, as far as I can tell. */

// This is really just CreateFont now...
#ifdef wx_motif
XFontStruct *
#else
Xv_Font
#endif
XFontPool::
FindOrCreateFont (int fontid, int style, int weight,
		  int point_size, Bool is_underlined, 
		  int point_size_to_store, int xres, int yres,
		  Display *display)
{
#ifdef wx_xview
  Xv_Font font;
#else
  XFontStruct *font;
#endif

  char *name;
  
  name = wxTheFontNameDirectory.GetScreenName(fontid, weight, style);
  if (!name)
    name = "-*-*-*-*-*-*-*-%d-*-*-*-*-*-*";

  sprintf (wxBuffer, name, point_size);
  
#ifdef wx_xview
  font = (Xv_Font) xv_find (XV_NULL, FONT,
			    FONT_NAME, wxBuffer,
			    NULL);
#else
  Display *dpy = display; /* MATTHEW: [4] Use display */
  font = XLoadQueryFont (dpy, wxBuffer);
#endif

  return font;
}

#ifdef wx_xview
Xv_Font
#else
XFontStruct *
#endif
  XFontPool::
FindNearestFont (int fontid, int style, int weight, int point_size,
		 Bool is_underlined, int xres, int yres,
		 Display *display)
{
#ifdef wx_xview
  Xv_Font font;
#else
  XFontStruct *font;
#endif

  font = FindFont(fontid, style, weight, point_size, is_underlined, display);

  if (font)
    return font;

  font = FindOrCreateFont (fontid, style, weight, point_size, is_underlined, 
			   point_size, xres, yres, display);

  if (!font) {
    
#define DELTA 10

    const int delta = DELTA;
    /* The weights below might need to be "tuned" @@@@ */
    const int max_size = point_size + 2 * delta * (1 + (point_size / (18 * delta)));
    const int min_size = point_size - 2 * delta * (1 + (point_size / (18 * delta)));
    int i;
    
    /* Search for another size (approx.) */
    /* Smaller */
    for (i = point_size - delta; !font && i >= delta && i >= min_size; i -= delta)
      font = FindOrCreateFont (fontid, style, weight, i, is_underlined, 
			       point_size, xres, yres, display);

    /* Larger */
    for (i = point_size + delta; !font && i <= max_size; i += delta)
      font = FindOrCreateFont (fontid, style, weight, i, is_underlined, point_size, 
			       xres, yres, display);
    
    /* Try Default fontid */
    if (!font && fontid != wxDEFAULT)
      font = FindOrCreateFont (wxDEFAULT, style, weight, point_size, is_underlined, 
			       point_size, xres, yres, display);

    /* Bogus font */
    if (!font)
      {
	int bog_standard = 120;
#ifdef wx_xview
	int real_bog_standard = 12;
#endif

	// For next time this happens, remember we just use a bog standard one
	
	font = FindOrCreateFont (wxDEFAULT, wxNORMAL, wxNORMAL, bog_standard,
				 is_underlined, point_size, DISPLAY_DPI, DISPLAY_DPI,
				 display);
	/* MATTHEW: [4] If above still found nothing, last ditch effort: */
	if (!font) {
#ifdef wx_xview
	  font = (Xv_Font) xv_find (XV_NULL, FONT,
				    FONT_FAMILY, FONT_FAMILY_DEFAULT,
				    FONT_SIZE, real_bog_standard,
				    NULL);
#else
	  if (!font) {
	    /* last-ditch efforst to find a font */
	    sprintf (wxBuffer, "-*-*-*-*-*-*-*-%d-*-*-*-*-*-*", 
		     bog_standard);
	    font = XLoadQueryFont (display, wxBuffer);
	  }
#endif
	}
      }
  }

  // Remember which font to use for next time
  if (font)
    AddFont(fontid, style, weight, point_size, is_underlined, display, font);

  return font;
}

#endif


/*
 * Colour map
 *
 * When constructed with the default constructor, we start from
 * the wxMainColormap, allocating additional read-only cells
 * in Create().  The cells are freed on the next call to Create()
 * or when the destructor is called.
 */
 
/* Wolfram Gloger <u7y22ab@sunmail.lrz-muenchen.de>
I have implemented basic colormap support for the X11 versions of
wxWindows, notably wxColourMap::Create().  The way I did it is to
allocate additional read-only color cells in the default colormap.  In
general you will get arbitrary pixel values assigned to these new
cells and therefore I added a method wxColourMap::TransferBitmap()
which maps the pixel values 0..n to the real ones obtained with
Create().  This is only implemented for the popular case of 8-bit
depth.

Allocating read-write color cells would involve installing a private
X11 colormap for a particular window, and AFAIK this is not
recommended; only the window manager should do this...  Also, it is
not the functionality that wxColourMap::Create() aims to provide.
 */

/* MATTHEW: [4] Display-specific colormap */
Colormap wxGetMainColormap(Display *display)
{
  static Colormap def_cmap;

  if (!display) /* Must be called first with non-NULL display */
    return def_cmap;

  Colormap c = 
    DefaultColormapOfScreen(XScreenOfDisplay(display, 
					     DefaultScreen(display)));

  if (!def_cmap)
    def_cmap = c;

  return c;
}

wxColourMap::wxColourMap (void)
{
  /* MATTHEW: [4] cmap, pix_array, destroyable only for XView */
#ifdef wx_xview
  cmap = wxGetMainColormap(NULL);
  pix_array = 0;
  destroyable = FALSE;
  pix_array_n = 0;
#endif
}

wxColourMap::~wxColourMap (void)
{
  /* MATTHEW: [4] Flush cmap by creating empty */
  Create(0, NULL, NULL, NULL);
}

/* MATTHEW: [4] Re-wrote to handle multiple displays */
Bool
wxColourMap::Create(const int n,
		    const unsigned char *red,
		    const unsigned char *green,
		    const unsigned char *blue)
{
  XColor xcol;
  Display *display;

#if wx_xview
  display = wxGetDisplay();
#endif
#ifdef wx_motif
  wxNode *node, *next;
  
  for (node = xcolormaps.First(); node; node = next) {
    wxXColormap *c = (wxXColormap *)node->Data();
    unsigned long *pix_array = c->pix_array;
    Colormap cmap = c->cmap;
    Bool destroyable = c->destroyable;
    int pix_array_n = c->pix_array_n;
    display = c->display;
#endif    
    if (pix_array_n > 0)
    {
//      XFreeColors(display, cmap, pix_array, pix_array_n, 0);
      // Be careful not to free '0' pixels...
      int i, j;
      for(i=j=0; i<pix_array_n; i=j) {
        while(j<pix_array_n && pix_array[j]!=0) j++;
        if(j > i) XFreeColors(display, cmap, &pix_array[i], j-i, 0);
        while(j<pix_array_n && pix_array[j]==0) j++;
      }
      delete [] pix_array;
    }

    if (destroyable)
      XFreeColormap(display, cmap);
#ifdef wx_motif
    next = node->Next();
    xcolormaps.DeleteNode(node);
    delete c;
  }
#endif
  
  if (!n) {
#ifdef wx_xview
    pix_array = NULL;
#endif
    return FALSE;
  }

#ifdef wx_motif
  display = wxGetDisplay();

  unsigned long *pix_array;
  Colormap cmap;
  int pix_array_n;
#endif

#ifdef wx_xview
  destroyable = FALSE;
#endif

  cmap = wxGetMainColormap(display);

  pix_array = new unsigned long[n];
  if (!pix_array)
    return FALSE;

  pix_array_n = n;
  xcol.flags = DoRed | DoGreen | DoBlue;
  for(int i = 0; i < n; i++) {
    xcol.red = (unsigned short)red[i] << 8;
    xcol.green = (unsigned short)green[i] << 8;
    xcol.blue = (unsigned short)blue[i] << 8;
    pix_array[i] = (XAllocColor(display, cmap, &xcol) == 0) ? 0 : xcol.pixel;
  }

#ifdef wx_motif
  wxXColormap *c = new wxXColormap;

  c->pix_array_n = pix_array_n;
  c->pix_array = pix_array;
  c->cmap = cmap;
  c->display = display;
  c->destroyable = FALSE;
  xcolormaps.Append(c);
#endif

  return TRUE;
}

Bool
wxColourMap::TransferBitmap(void *data, int depth, int size)
{
  switch(depth) {
  case 8:
    {
	unsigned char *uptr = (unsigned char *)data;
#ifdef wx_motif
	int pix_array_n;
	unsigned long *pix_array = GetXPixArray(wxGetDisplay(), &pix_array_n);
#endif	
	while(size-- > 0) {
	  if((int)*uptr < pix_array_n)
	    *uptr = (unsigned char)pix_array[*uptr];
	  uptr++;
	}

	return TRUE;
     }
  default:
	return FALSE;
}
}

Bool
wxColourMap::TransferBitmap8(unsigned char *data, unsigned long size,
			     void *dest, unsigned int bpp)
{
#ifdef wx_motif
  int pix_array_n;
  unsigned long *pix_array = GetXPixArray(wxGetDisplay(), &pix_array_n);
#endif
    switch(bpp) {
    case 8: {
	unsigned char *dptr = (unsigned char *)dest;
	while(size-- > 0) {
	    if((int)*data < pix_array_n)
	     *dptr = (unsigned char)pix_array[*data];
	    data++;
	    dptr++;
	}
	break;
    }
    case 16: {
	unsigned short *dptr = (unsigned short *)dest;
	while(size-- > 0) {
		if((int)*data < pix_array_n)
		 *dptr = (unsigned short)pix_array[*data];
		data++;
		dptr++;
	}
	break;
    }
    case 24: {
	struct rgb24 { unsigned char r, g, b; } *dptr = (struct rgb24 *)dest;
	while(size-- > 0) {
	    if((int)*data < pix_array_n) {
		dptr->r = pix_array[*data] & 0xFF;
		dptr->g = (pix_array[*data] >> 8) & 0xFF;
		dptr->b = (pix_array[*data] >> 16) & 0xFF;
	    }
	    data++;
	    dptr++;
	}
	break;
    }
    case 32: {
	unsigned long *dptr = (unsigned long *)dest;
	while(size-- > 0) {
		if((int)*data < pix_array_n)
		 *dptr = pix_array[*data];
		data++;
		dptr++;
	}
	break;
    }
    default:
	return FALSE;
    }
    return TRUE;
}

int wxColourMap::GetPixel(const unsigned char red, const unsigned char green, const unsigned char blue)
{
  return 0;
}

Bool wxColourMap::GetRGB(const int pixel, unsigned char *red, unsigned char *green, unsigned char *blue)
{
#ifdef wx_motif
  // Multiple colourmaps (1 per display) in Motif
  Colormap cmap = GetXColormap(wxGetDisplay());
#endif
  // In XView, there's already a cmap member.

  Display *dpy = wxGetDisplay();
  XColor xcol;

  if(pixel == 0) return FALSE;
  xcol.pixel = pixel;
  XQueryColor(dpy, cmap, &xcol);
  *red = xcol.red >> 8;
  *green = xcol.green >> 8;
  *blue = xcol.blue >> 8;
  return TRUE;
}

/* MATTHEW: [4] Get display-specific cmap */
Colormap wxColourMap::GetXColormap(Display *display)
{
#ifdef wx_xview
  return cmap;
#else
  if (!xcolormaps.Number())
    return wxGetMainColormap(display);

  wxNode *node;

  for (node = xcolormaps.First(); node; node = node->Next()) {
    wxXColormap *c = (wxXColormap *)node->Data();
    if (c->display == display)
      return c->cmap;
  }

  /* Make a new one: */
  wxXColormap *c = new wxXColormap;
  wxXColormap *first = (wxXColormap *)xcolormaps.First()->Data();
  XColor xcol;
  int pix_array_n = first->pix_array_n;
  
  c->pix_array_n = pix_array_n;
  c->pix_array = new unsigned long[pix_array_n];
  c->display = display;
  c->cmap = wxGetMainColormap(display);
  c->destroyable = FALSE;

  xcol.flags = DoRed | DoGreen | DoBlue;
  int i;
  for (i = 0; i < pix_array_n; i++) {
    xcol.pixel = first->pix_array[i];
    XQueryColor(first->display, first->cmap, &xcol);
    c->pix_array[i] = 
      (XAllocColor(display, c->cmap, &xcol) == 0) ? 0 : xcol.pixel;
  }

  xcolormaps.Append(c);

  return c->cmap;
#endif
}

/* MATTHEW: [4] Get display-specific pix_array */
unsigned long *wxColourMap::GetXPixArray(Display *display, int *n)
{
#ifdef wx_motif
  wxNode *node;

  for (node = xcolormaps.First(); node; node = node->Next()) {
    wxXColormap *c = (wxXColormap *)node->Data();
    if (c->display == display) {
      if (n)
	*n = c->pix_array_n;
      return c->pix_array;
    }
  }
  
  /* Not found; call GetXColormap, which will create it, then this again */
  (void)GetXColormap(display);
  return GetXPixArray(display, n);
#endif
#ifdef wx_xview
  if (n)
    *n = pix_array_n;
  return pix_array;
#endif
}

void wxColourMap::PutXColormap(Display *display, Colormap cm, Bool dp)
{
  Create(0, NULL, NULL, NULL);
#ifdef wx_xview
  cmap = cm;
  destroyable = dp;
  pix_array_n = 0;
#else
  wxXColormap *c = new wxXColormap;

  c->pix_array_n = 0;
  c->pix_array = NULL;
  c->display = display;
  c->cmap = cm;
  c->destroyable = dp;

  xcolormaps.Append(c);
#endif  
}

// Pens

wxPen::wxPen (void)
{
  stipple = NULL;
  style = wxSOLID;
  join = wxJOIN_ROUND;
  cap = wxCAP_ROUND;
  nb_dash = 0;
  dash = NULL;
  width = 1;

#if !WXGARBAGE_COLLECTION_ON
  wxThePenList->AddPen (this);
#endif
}

wxPen::~wxPen ()
{
//  if (colour)
//    delete colour;

#if !WXGARBAGE_COLLECTION_ON
  wxThePenList->RemovePen (this);
#endif
}

wxPen::wxPen (wxColour & col, int Width, int Style):
wxbPen (col, Width, Style)
{
  colour = col;

  stipple = NULL;
  width = Width;
  style = Style;
  join = wxJOIN_ROUND;
  cap = wxCAP_ROUND;
  nb_dash = 0;
  dash = NULL;
#if !WXGARBAGE_COLLECTION_ON
  wxThePenList->AddPen (this);
#endif
}

wxPen::wxPen (char *col, int Width, int Style):
wxbPen (col, Width, Style)
{
  colour = col;
  stipple = NULL;
  width = Width;
  style = Style;
  join = wxJOIN_ROUND;
  cap = wxCAP_ROUND;
  nb_dash = 0;
  dash = NULL;

#if !WXGARBAGE_COLLECTION_ON
  wxThePenList->AddPen (this);
#endif
}

// Brushes

wxBrush::wxBrush (void)
{
  style = wxSOLID;
  stipple = NULL;
#if !WXGARBAGE_COLLECTION_ON
  wxTheBrushList->AddBrush (this);
#endif
}

wxBrush::~wxBrush ()
{
//  if (colour)
//    delete colour;
#if !WXGARBAGE_COLLECTION_ON
  wxTheBrushList->RemoveBrush (this);
#endif
}

wxBrush::wxBrush (wxColour & col, int Style):
wxbBrush (col, Style)
{
  colour = col;
  style = Style;
  stipple = NULL;
#if !WXGARBAGE_COLLECTION_ON
  wxTheBrushList->AddBrush (this);
#endif
}

wxBrush::wxBrush (char *col, int Style):
wxbBrush (col, Style)
{
  colour = col;
  style = Style;
  stipple = NULL;
#if !WXGARBAGE_COLLECTION_ON
  wxTheBrushList->AddBrush (this);
#endif
}

// Icons
wxIcon::wxIcon (char bits[], int Width, int Height)
{
  WXSET_TYPE(wxIcon, wxTYPE_ICON)
  
  ok = FALSE;
  width = Width;
  height = Height;
  numColors = 0;
  x_pixmap = 0;
#ifdef wx_motif
  image = 0;
//  iconWidth = width;
//  iconHeight = height;
  Display *dpy = display = wxGetDisplay(); /* MATTHEW: [4] Use wxGetDisplay */
  x_pixmap = XCreateBitmapFromData (dpy, RootWindow (dpy, DefaultScreen (dpy)), bits, width, height);
  if (x_pixmap)
    ok = TRUE;
  else
    ok = FALSE;
#elif defined(wx_xview)
  x_image = (Server_image) xv_create (XV_NULL,
				      SERVER_IMAGE,
				      XV_WIDTH, width,
				      XV_HEIGHT, height,
				      SERVER_IMAGE_X_BITS, bits,
				      NULL);
  x_icon = (Icon) xv_create (XV_NULL,
			     ICON,
                             XV_WIDTH, width,
                             XV_HEIGHT, height,
			     ICON_IMAGE, x_image,
			     NULL);
  if (x_image && x_icon)
    ok = TRUE;
  else
    ok = FALSE;
#endif
//  wxTheIconList->Append (this);
}

wxIcon::wxIcon (void)
{
  WXSET_TYPE(wxIcon, wxTYPE_ICON)
  
  ok = FALSE;
  width = 0;
  height = 0;
  x_pixmap = 0;
#ifdef wx_motif
//  iconWidth = 0;
//  iconHeight = 0;
  image = 0;
#elif defined(wx_xview)
  x_image = 0;
  x_icon = 0;
#endif
//  wxTheIconList->Append (this);
}

wxIcon::wxIcon (const char *icon_file, long flags)
{
  WXSET_TYPE(wxIcon, wxTYPE_ICON)
  
  ok = FALSE;
  width = 0;
  height = 0;
  numColors = 0;
  x_pixmap = 0;
#ifdef wx_motif
  image = 0;
  int hotX, hotY;
  unsigned int w, h;
  Display *dpy = display = wxGetDisplay(); /* MATTHEW: [4] Use wxGetDisplay */
  int value = XReadBitmapFile (dpy, RootWindow (dpy, DefaultScreen (dpy)), icon_file, &w, &h, &x_pixmap, &hotX, &hotY);
  width = w;
  height = h;
  if ((value == BitmapFileInvalid) || (value == BitmapOpenFailed) || (value == BitmapNoMemory))
  {
    x_pixmap = 0;
    ok = FALSE;
  }
  else
    ok = TRUE;
#elif defined(wx_xview)
//  x_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
//				      SERVER_IMAGE_BITMAP_FILE, icon_file,
//				      NULL);
   x_icon = 0;
  (void)LoadFile((char *)icon_file, flags);

  if (x_image)
  {
    x_icon = (Icon) xv_create (XV_NULL, ICON,
  			     ICON_IMAGE, x_image,
			     NULL);
    width = (int) xv_get (x_icon, XV_WIDTH);
    height = (int) xv_get (x_icon, XV_HEIGHT);
  }
  if (x_image && x_icon)
    ok = TRUE;
  else
    ok = FALSE;
#endif
//  wxTheIconList->Append (this);
}

#if USE_XPM_IN_X
wxIcon::wxIcon(char **data):wxBitmap(data)
{
  WXSET_TYPE(wxIcon, wxTYPE_ICON)

#ifdef wx_motif
#elif defined(wx_xview)
  if (ok)
  {
    x_icon = (Icon) xv_create (XV_NULL, ICON,
                             XV_WIDTH, width,
                             XV_HEIGHT, height,
			     ICON_IMAGE, x_image,
			     NULL);
    if (x_icon)
      ok = TRUE;
    else
      ok = FALSE;
  }
#endif
}
#endif

wxIcon::~wxIcon (void)
{
#ifdef wx_motif
  Display *dpy = display; /* MATTHEW: [4] Use display */
  if (x_pixmap)
    XFreePixmap (dpy, x_pixmap);
#elif defined(wx_xview)
  xv_destroy_safe (x_icon);
#endif
//  wxTheIconList->DeleteObject (this);
}

// Cursors

/* MATTHEW: [4] Display-specific cursor for Motif */

wxCursor::wxCursor (void)
{
  WXSET_TYPE(wxCursor, wxTYPE_CURSOR)
  
  width = 0;
  height = 0;
  numColors = 0;
  x_pixmap = 0;
  ok = FALSE;
#ifdef wx_motif
  image = 0;
  cursor_id = 0; /* MATTHEW: [4]  Init cursor_id */
#endif
#ifdef wx_xview
  x_cursor = 0;  /* MATTHEW: [4] Only XView uses x_cursor now */
  x_image = 0;
  use_raw_x_cursor = FALSE;
#endif
//  wxTheCursorList->Append (this);
}

wxCursor::wxCursor (char bits[], int w, int h, int hotSpotX, int hotSpotY, char maskbits[])
{
  WXSET_TYPE(wxCursor, wxTYPE_CURSOR)
  
  width = 0;
  height = 0;
  numColors = 0;
  x_pixmap = 0; 
  
  if (hotSpotX == -1)
    hotSpotX = w/2;

  if (hotSpotY == -1)
    hotSpotY = h/2;

  /* MATTHEW: [4] Only XView uses x_cursor now */
#ifdef wx_motif
// Code inserted by D. Harrison to implement the dynamic cursor creation in
//  motif.

  Display *dpy = wxGetDisplay();
  int screen_num =  DefaultScreen (dpy);

  use_raw_x_cursor = TRUE;
  x_pixmap = XCreatePixmapFromBitmapData (dpy, 
                                          RootWindow (dpy, DefaultScreen(dpy)),
                                          bits, w, h , 
                                          1 , 0 , 1);

  Pixmap mask_pixmap = None;
  if (maskbits != NULL) {
	mask_pixmap = XCreatePixmapFromBitmapData (dpy, 
                                          RootWindow (dpy, DefaultScreen(dpy)),
                                          maskbits, w, h , 
                                          1 , 0 , 1);
  }

  XColor foreground_color;
  XColor background_color;
  foreground_color.pixel = BlackPixel(dpy, screen_num);
  background_color.pixel = WhitePixel(dpy, screen_num);
  Colormap cmap = wxGetMainColormap(dpy);
  XQueryColor(dpy, cmap, &foreground_color);
  XQueryColor(dpy, cmap, &background_color);

  Cursor cursor = XCreatePixmapCursor (dpy,
                                  x_pixmap,
                                  mask_pixmap,
                                  &foreground_color,
                                  &background_color,
                                  hotSpotX , 
                                  hotSpotY);

  XFreePixmap( dpy, x_pixmap );
  if (mask_pixmap != None) {
  	XFreePixmap( dpy, mask_pixmap );
  }

  if (cursor) {
    wxXCursor *c = new wxXCursor;

    c->x_cursor = cursor;
    c->display = dpy;
    xcursors.Append(c);
  }

  image = 0;
  if (cursor)
    ok = TRUE;
  else
    ok = FALSE;
  
  cursor_id = 0;
#elif defined(wx_xview)
  x_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
				    XV_WIDTH, w,
				    XV_HEIGHT, h,
				    SERVER_IMAGE_X_BITS, bits,
				    NULL);
  x_cursor = xv_create (XV_NULL, CURSOR, CURSOR_IMAGE, x_image, NULL);
  use_raw_x_cursor = FALSE;
  if (x_cursor && x_image)
    ok = TRUE;
  else
    ok = FALSE;
#endif
//  wxTheCursorList->Append (this);
}

wxCursor::wxCursor (const char *cursor_file, long flags, int hotSpotX, int hotSpotY)
{
  WXSET_TYPE(wxCursor, wxTYPE_CURSOR)
  
  width = 0;
  height = 0;
  numColors = 0;
  x_pixmap = 0;
#ifdef wx_motif
  image = 0;
#elif defined(wx_xview)
  x_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
				    SERVER_IMAGE_BITMAP_FILE, cursor_file,
				    NULL);
  x_cursor = xv_create (XV_NULL, CURSOR, CURSOR_IMAGE, x_image, NULL);
  use_raw_x_cursor = FALSE;
#endif
//  wxTheCursorList->Append (this);
}

/* MATTHEW: [4] Abstract cursor lookup: (Don't miss Motif I-beam change) */

static
#ifdef wx_xview
Xv_Cursor
#elif defined(wx_motif)
Cursor
#endif
 XMakeCursor(int cursor_type, Bool& use_raw_x_cursor, Display *dpy)
{
  
#ifdef wx_xview
  Xv_Cursor x_cursor = NULL;
#elif defined(wx_motif)
  Cursor x_cursor =
#ifdef VMS
    0;
#else
    NULL;
#endif
#endif
  
  switch (cursor_type)
    {
    case wxCURSOR_WAIT:
      {
#ifdef wx_xview
	x_cursor = xv_create (XV_NULL, CURSOR, CURSOR_SRC_CHAR, OLC_BUSY_PTR, NULL);
#elif defined(wx_motif)
	x_cursor = XCreateFontCursor (dpy, XC_watch);
#endif
	break;
      }
    case wxCURSOR_CROSS:
      {
#ifdef wx_xview
	Server_image svr_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
							   XV_WIDTH, 16, XV_HEIGHT, 16, SERVER_IMAGE_BITS, crosshair_cursor_data, NULL);
	x_cursor = xv_create (XV_NULL, CURSOR, CURSOR_IMAGE, svr_image,
			      CURSOR_XHOT, 3, CURSOR_YHOT, 3, NULL);
#elif defined(wx_motif)
	x_cursor = XCreateFontCursor (dpy, XC_crosshair);
#endif
	break;
      }
    case wxCURSOR_CHAR:
      {
#ifdef wx_xview
	Server_image svr_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
							   XV_WIDTH, 16, XV_HEIGHT, 16, SERVER_IMAGE_BITS, char_cursor_data, NULL);
	x_cursor = xv_create (XV_NULL, CURSOR, CURSOR_IMAGE, svr_image,
			      CURSOR_XHOT, 0, CURSOR_YHOT, 13, NULL);
#elif defined(wx_motif)
	// Nothing
#endif
	break;
      }
    case wxCURSOR_HAND:
      {
#ifdef wx_xview
	Server_image svr_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
							   XV_WIDTH, 16, XV_HEIGHT, 16, SERVER_IMAGE_BITS, hand_cursor_array, NULL);
	x_cursor = xv_create (XV_NULL, CURSOR, CURSOR_IMAGE, svr_image,
			      CURSOR_XHOT, 5, CURSOR_YHOT, 0, NULL);
#elif defined(wx_motif)
	x_cursor = XCreateFontCursor (dpy, XC_hand1);
#endif
	break;
      }
    case wxCURSOR_BULLSEYE:
      {
#ifdef wx_xview
	Server_image svr_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
							   XV_WIDTH, 16, XV_HEIGHT, 16, SERVER_IMAGE_BITS, bull_cursor_array, NULL);
	x_cursor = xv_create (XV_NULL, CURSOR, CURSOR_IMAGE, svr_image,
			      CURSOR_XHOT, 5, CURSOR_YHOT, 5, NULL);
#elif defined(wx_motif)
	x_cursor = XCreateFontCursor (dpy, XC_target);
#endif
	break;
      }
    case wxCURSOR_PENCIL:
      {
#ifdef wx_xview
	Server_image svr_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
	XV_WIDTH, 16, XV_HEIGHT, 16, SERVER_IMAGE_BITS, pencil_cursor_array,
							   NULL);
	x_cursor = xv_create (XV_NULL, CURSOR, CURSOR_IMAGE, svr_image,
			      CURSOR_XHOT, 0, CURSOR_YHOT, 14, NULL);
#elif defined(wx_motif)
	x_cursor = XCreateFontCursor (dpy, XC_pencil);
#endif
	break;
      }
    case wxCURSOR_MAGNIFIER:
      {
#ifdef wx_xview
	Server_image svr_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
							   XV_WIDTH, 16, XV_HEIGHT, 16, SERVER_IMAGE_BITS, magnifier_cursor_array, NULL);
	x_cursor = xv_create (XV_NULL, CURSOR, CURSOR_IMAGE, svr_image,
			      CURSOR_XHOT, 6, CURSOR_YHOT, 6, NULL);
#elif defined(wx_motif)
	x_cursor = XCreateFontCursor (dpy, XC_sizing);
#endif
	break;
      }
    case wxCURSOR_IBEAM:
      {
#ifdef wx_xview
	Server_image svr_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
							   XV_WIDTH, 16, XV_HEIGHT, 16, SERVER_IMAGE_BITS, vbar_cursor_array, NULL);
	x_cursor = xv_create (XV_NULL, CURSOR, CURSOR_IMAGE, svr_image,
			      CURSOR_XHOT, 0, CURSOR_YHOT, 13, NULL);
#elif defined(wx_motif)
	/* MATTHEW: [4] I-beam in Motif */
	x_cursor = XCreateFontCursor (dpy, XC_xterm);
#endif
	break;
      }
    case wxCURSOR_NO_ENTRY:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_pirate);
	break;
      }

    case wxCURSOR_LEFT_BUTTON:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_leftbutton);
	break;
      }
    case wxCURSOR_RIGHT_BUTTON:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_rightbutton);
	break;
      }
    case wxCURSOR_MIDDLE_BUTTON:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_middlebutton);
	break;
      }
    case wxCURSOR_QUESTION_ARROW:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_question_arrow);
	break;
      }
    case wxCURSOR_SIZING:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_sizing);
	break;
      }
    case wxCURSOR_WATCH:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_watch);
	break;
      }
    case wxCURSOR_SPRAYCAN:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_spraycan);
	break;
      }
    case wxCURSOR_PAINT_BRUSH:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_spraycan);
	break;
      }
    case wxCURSOR_SIZENWSE:
    case wxCURSOR_SIZENESW:
      {
        // Not available in X
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_crosshair);
	break;
      }
    case wxCURSOR_SIZEWE:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_sb_h_double_arrow);
	break;
      }
    case wxCURSOR_SIZENS:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_sb_v_double_arrow);
	break;
      }
    case wxCURSOR_POINT_LEFT:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_sb_left_arrow);
	break;
      }
    case wxCURSOR_POINT_RIGHT:
      {
	use_raw_x_cursor = TRUE;
	x_cursor = XCreateFontCursor (dpy, XC_sb_right_arrow);
	break;
      }
    // (JD Huggins) added more stock cursors for X
    // X-only cursors BEGIN
    case wxCURSOR_CROSS_REVERSE:
    {
      use_raw_x_cursor = TRUE;
      x_cursor = XCreateFontCursor(dpy, XC_cross_reverse);
      break;
    }
    case wxCURSOR_DOUBLE_ARROW:
    {
      use_raw_x_cursor = TRUE;
      x_cursor = XCreateFontCursor(dpy, XC_double_arrow);
      break;
    }
    case wxCURSOR_BASED_ARROW_UP:
    {
      use_raw_x_cursor = TRUE;
      x_cursor = XCreateFontCursor(dpy, XC_based_arrow_up);
      break;
    }
    case wxCURSOR_BASED_ARROW_DOWN:
    {
      use_raw_x_cursor = TRUE;
      x_cursor = XCreateFontCursor(dpy, XC_based_arrow_down);
      break;
    }
    // X-only cursors END
    default:
    case wxCURSOR_ARROW:
      {
#ifdef wx_xview
	x_cursor = xv_create (XV_NULL, CURSOR, CURSOR_SRC_CHAR, OLC_BASIC_PTR, NULL);
#elif defined(wx_motif)
	x_cursor = XCreateFontCursor (dpy, XC_top_left_arrow);
#endif
	break;
      }
    case wxCURSOR_BLANK:
      {
	GC gc;
	XGCValues gcv;
	Pixmap empty_pixmap;
	XColor blank_color;

	use_raw_x_cursor = TRUE;
	empty_pixmap = XCreatePixmap (dpy, RootWindow (dpy, DefaultScreen (dpy)),
				      16, 16, 1);
	gcv.function = GXxor;
	gc = XCreateGC (dpy,
			empty_pixmap,
			GCFunction,
			&gcv);
	XCopyArea (dpy,
		   empty_pixmap,
		   empty_pixmap,
		   gc,
		   0, 0,
		   16, 16,
		   0, 0);
	XFreeGC (dpy, gc);
	x_cursor = XCreatePixmapCursor (dpy,
					empty_pixmap,
					empty_pixmap,
					&blank_color,
					&blank_color,
					8, 8);

	break;
      }
    }

  return x_cursor;
}

/* MATTHEW: [4] This used XMakeCursor, now */
// Cursors by stock number
wxCursor::wxCursor (int cursor_type)
{
  WXSET_TYPE(wxCursor, wxTYPE_CURSOR)
  
  width = 0;
  height = 0;
  ok = TRUE;

  use_raw_x_cursor = FALSE;

#ifdef wx_motif
  image = 0;
  x_pixmap = 0;
#endif
#ifdef wx_xview
  x_image = 0;
#endif

#ifdef wx_motif
  cursor_id = cursor_type;
#endif
#ifdef wx_xview
  x_cursor = XMakeCursor(cursor_type, use_raw_x_cursor, wxGetDisplay());
#endif
}

/* MATTHEW: [4] Added method to get X cursor */
#ifdef wx_motif
Cursor
#else
Xv_Cursor
#endif
  wxCursor::GetXCursor(Display *disp)
{
#ifdef wx_motif
  wxNode *node;

  for (node = xcursors.First(); node; node = node->Next()) {
    wxXCursor *c = (wxXCursor *)node->Data();
    
    if (c->display == disp)
      return c->x_cursor;
  }

  Bool dummy;
  Cursor x_cursor = XMakeCursor(cursor_id, dummy, disp);
  if (x_cursor) {
    wxXCursor *c = new wxXCursor;

    c->x_cursor = x_cursor;
    c->display = disp;
    xcursors.Append(c);
  }

  return x_cursor;
#else
  return x_cursor;
#endif
}

wxCursor::~wxCursor (void)
{
#ifdef wx_motif
  xcursors.DeleteContents(TRUE);
#endif
#ifdef wx_xview
  if (!use_raw_x_cursor && x_cursor)
    xv_destroy_safe (x_cursor);
#endif
//  wxTheCursorList->DeleteObject (this);
}

// Global cursor setting
void 
wxSetCursor (wxCursor * cursor)
{
#ifdef wx_motif
#endif
#ifdef wx_xview
  Xv_Screen screen = xv_get (xview_server, SERVER_NTH_SCREEN, 0);
  Xv_Window root_window = xv_get (screen, XV_ROOT);
  if (cursor && cursor->x_cursor)
    {
      if (cursor->use_raw_x_cursor)
	{
	  Display *dpy = (Display *) xv_get (root_window, XV_DISPLAY);
	  Window win2 = xv_get (root_window, XV_XID);

	  XDefineCursor (dpy, win2, cursor->x_cursor);
	}
      else
	xv_set (root_window, WIN_CURSOR, cursor->x_cursor, NULL);
    }

    // Necessary? Could cause some awkward event ordering problems.
  wxFlushEvents ();
#endif
}

// Misc. functions

// Return TRUE if we have a colour display
Bool 
wxColourDisplay (void)
{
  Display *dpy = wxGetDisplay();

  if (DefaultDepth (dpy, DefaultScreen (dpy)) < 2)
    return FALSE;
  else
    return TRUE;
}

// Returns depth of screen
int 
wxDisplayDepth (void)
{
  Display *dpy = wxGetDisplay();
  return DefaultDepth (dpy, DefaultScreen (dpy));
}

// Get size of display
void 
wxDisplaySize (int *width, int *height)
{
  Display *dpy = wxGetDisplay();
  
#ifdef wx_motif
  if (wxTheApp->topLevel)
    {
      *width = DisplayWidth (dpy, DefaultScreen (dpy));
      *height = DisplayHeight (dpy, DefaultScreen (dpy));
    }
  else
    {
      // A good bet!
      *width = 1024;
      *height = 768;
    }
#endif
#ifdef wx_xview
  *width = DisplayWidth (dpy, DefaultScreen(dpy));
  *height = DisplayHeight (dpy, DefaultScreen(dpy));
#endif
}

wxBitmap::wxBitmap (void)
{
  WXSET_TYPE(wxBitmap, wxTYPE_BITMAP)
  
  ok = FALSE;
  width = 0;
  height = 0;
  depth = 0;
  numColors = 0;
  bitmapColourMap = NULL;
  bitmapMask = NULL;
  x_pixmap = 0;
  freePixmap = FALSE;

  free_colors = NULL;
  free_colors_num = 0;

#ifdef wx_motif
  insensPixmap = 0;
  labelPixmap = 0;
  armPixmap = 0;
  insensImage = 0;
  image = 0;
#endif
#ifdef wx_xview
  x_image = 0;
#endif

#if !WXGARBAGE_COLLECTION_ON
  wxTheBitmapList->Append (this);
#endif
}

wxBitmap::wxBitmap (char bits[], int the_width, int the_height, int no_bits)
{
  WXSET_TYPE(wxBitmap, wxTYPE_BITMAP)
  
  depth = no_bits;
  width = the_width;
  height = the_height;
  numColors = 0;
  bitmapColourMap = NULL;
  bitmapMask = NULL;
  free_colors = NULL;
  free_colors_num = 0;

#ifdef wx_motif
  insensPixmap = 0;
  labelPixmap = 0;
  armPixmap = 0;
  insensImage = 0;
  image = 0;

  freePixmap = TRUE;
  Display *dpy = display = wxGetDisplay(); /* MATTHEW: [4] Use wxGetDisplay */
  x_pixmap = XCreateBitmapFromData (dpy, RootWindow (dpy, DefaultScreen (dpy)), bits, width, height);
  if (x_pixmap)
    ok = TRUE;
  else
    ok = FALSE;

  // code for wxItem
  image = (XImage *) XtMalloc (sizeof (XImage));
  image->width = the_width;
  image->height = the_height;
  image->data = bits;
  image->depth = 1;
  image->xoffset = 0;
  image->format = XYBitmap;
  image->byte_order = LSBFirst;
  image->bitmap_unit = 8;
  image->bitmap_bit_order = LSBFirst;
  image->bitmap_pad = 8;
  image->bytes_per_line = (the_width + 7) >> 3;

  char tmp[128];
  sprintf (tmp, "Im%x", (unsigned int)image);
  XmInstallImage (image, tmp);

  // Build our manually stipped pixmap.

  int bpl = (the_width + 7) / 8;
  char *data = new char[the_height * bpl];
  int i;
  for (i = 0; i < the_height; i++)
    {
      int mask = i % 2 ? 0x55 : 0xaa;
      int j;
      for (j = 0; j < bpl; j++)
	data[i * bpl + j] = bits[i * bpl + j] & mask;
    }
  insensImage = (XImage *) XtMalloc (sizeof (XImage));
  insensImage->width = the_width;
  insensImage->height = the_height;
  insensImage->data = data;
  insensImage->depth = 1;
  insensImage->xoffset = 0;
  insensImage->format = XYBitmap;
  insensImage->byte_order = LSBFirst;
  insensImage->bitmap_unit = 8;
  insensImage->bitmap_bit_order = LSBFirst;
  insensImage->bitmap_pad = 8;
  insensImage->bytes_per_line = bpl;

  sprintf (tmp, "Not%x", (unsigned int)insensImage);
  XmInstallImage (insensImage, tmp);
#endif
#ifdef wx_xview
  freePixmap = FALSE;
  // Use the class variable, used by wxItem!
  x_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
				      XV_WIDTH, width,
				      XV_HEIGHT, height,
				      SERVER_IMAGE_X_BITS, bits,
				      NULL);

  if (x_image)
    {
      ok = TRUE;
      x_pixmap = (Pixmap) xv_get (x_image, SERVER_IMAGE_PIXMAP);
    }
  else
    ok = FALSE;
#endif
#if !WXGARBAGE_COLLECTION_ON
  wxTheBitmapList->Append (this);
#endif
}

wxBitmap::wxBitmap (char *bitmap_file, long flags)
{
  WXSET_TYPE(wxBitmap, wxTYPE_BITMAP)
  
  free_colors = NULL;
  free_colors_num = 0;
  numColors = 0;
  bitmapColourMap = NULL;
  bitmapMask = NULL;
  display = wxGetDisplay(); /* MATTHEW: [4] Display */
#ifdef wx_motif
  insensPixmap = 0;
  labelPixmap = 0;
  armPixmap = 0;
  insensImage = 0;
  image = 0;
  bitmapColourMap = NULL;
#endif
  width = 0;
  height = 0;
  depth = 0;
  freePixmap = FALSE;

  (void)LoadFile(bitmap_file, (int)flags);

#if !WXGARBAGE_COLLECTION_ON
  wxTheBitmapList->Append (this);
#endif
}

#if USE_XPM_IN_X
wxBitmap::wxBitmap(char **data, wxItem *anItem)
{
  WXSET_TYPE(wxBitmap, wxTYPE_BITMAP)
  
  free_colors = NULL;
  free_colors_num = 0;
  numColors = 0;
  bitmapColourMap = NULL;
  bitmapMask = NULL;
#ifdef wx_motif
  insensPixmap = 0;
  labelPixmap = 0;
  armPixmap = 0;
  insensImage = 0;
  image = 0;
#endif
  width = 0;
  height = 0;
  depth = 0;
  freePixmap = FALSE;

  XpmAttributes xpmAttr;

  ok = FALSE;

  xpmAttr.valuemask = XpmReturnInfos;	/* nothing yet, but get infos back */
  Display *dpy = wxGetDisplay();

  display = dpy; /* MATTHEW: [4] Remember the display */

#ifdef wx_motif
  XpmColorSymbol symbolicColors[4];
  if (anItem && anItem->handle) {
		symbolicColors[0].name = "foreground";
		symbolicColors[0].value = NULL;
		symbolicColors[1].name = "background";
		symbolicColors[1].value = NULL;
		XtVaGetValues((Widget)anItem->handle,
			XmNforeground,  &symbolicColors[0].pixel,
			XmNbackground,  &symbolicColors[1].pixel,NULL);
		xpmAttr.numsymbols = 2;
		xpmAttr.colorsymbols = symbolicColors;
		xpmAttr.valuemask |= XpmColorSymbols;	// add flag
	}
#endif

  Pixmap  pixmap;
  Pixmap mask = 0;
  int ErrorStatus = XpmCreatePixmapFromData(dpy, RootWindow(dpy, DefaultScreen(dpy)),
               data, &pixmap, &mask, &xpmAttr);
  if (ErrorStatus == XpmSuccess)
  {
    // Set attributes
    width=xpmAttr.width;
    height = xpmAttr.height;
    if ( xpmAttr.npixels > 2 )
    {
	depth = 8;	// next time not just a guess :-) ...
    } else
    {
	depth = 1;	// mono	
    }
    numColors = xpmAttr.npixels;
    XpmFreeAttributes(&xpmAttr);
    ok = TRUE;
    x_pixmap = pixmap;
    if ( mask )
    {
        bitmapMask = new wxMask;
        bitmapMask->SetMaskBitmap(mask);
    }
  }
  else
  {
//		XpmDebugError(ErrorStatus, NULL);
		ok = False;
  }
// ADDED JACS
#ifdef wx_xview
  freePixmap = TRUE;
  x_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
 				      SERVER_IMAGE_SAVE_PIXMAP, TRUE,
				      SERVER_IMAGE_PIXMAP, pixmap,
                                      SERVER_IMAGE_DEPTH, GetDepth(),
						   NULL);
  if (x_image)
    ok = TRUE;
  else
    ok = FALSE;

#endif
}
#endif

/*
 * Create a new bitmap of a given size and depth
 *
 */

wxBitmap::wxBitmap (int w, int h, int d)
{
  WXSET_TYPE(wxBitmap, wxTYPE_BITMAP)
  
  numColors = 0;
  bitmapColourMap = NULL;
  free_colors = NULL;
  free_colors_num = 0;
  bitmapColourMap = NULL;
  bitmapMask = NULL;
  (void)Create(w, h, d);
#if !WXGARBAGE_COLLECTION_ON
  wxTheBitmapList->Append (this);
#endif
}

wxBitmap::~wxBitmap (void)
{
  /* MATTHEW: [4] Use display */
#ifdef wx_motif
  if (labelPixmap)
    XmDestroyPixmap (DefaultScreenOfDisplay (display), labelPixmap);

  if (armPixmap)
    XmDestroyPixmap (DefaultScreenOfDisplay (display), armPixmap);

  if (insensPixmap)
    XmDestroyPixmap (DefaultScreenOfDisplay (display), insensPixmap);

  if (image)
    {
      XmUninstallImage (image);
      XtFree ((char *) image);
    }

  if (insensImage)
    {
      XmUninstallImage (insensImage);
      delete[]insensImage->data;
      XtFree ((char *) insensImage);
    }
#endif
#ifdef wx_xview
  // Should we be destroying the Server_image or not?
//  xv_destroy_safe(x_image);
#endif
  if (x_pixmap && freePixmap)
    XFreePixmap (display, x_pixmap);

  if (free_colors)
     {
       int screen = DefaultScreen(display);
       Colormap cmp = DefaultColormap(display,screen);
       long llp;
       for(llp = 0;llp < free_colors_num;llp++)
          XFreeColors(display, cmp, &free_colors[llp], 1, 0L);
       delete free_colors;
     };

  if (bitmapColourMap)
    delete bitmapColourMap;

  if ( bitmapMask )
    delete bitmapMask;

  free_colors = NULL;
  free_colors_num = 0;

#if !WXGARBAGE_COLLECTION_ON
  wxTheBitmapList->DeleteObject (this);
#endif
}

Bool wxBitmap::Create(int w, int h, int d)
{
  width = w;
  height = h;
  depth = d;
  if (d < 1)
    depth = wxDisplayDepth ();

  freePixmap = TRUE;

#ifdef wx_motif
  insensPixmap = 0;
  labelPixmap = 0;
  armPixmap = 0;
  insensImage = 0;
  image = 0;
#endif
  Display *dpy = wxGetDisplay();

  display = dpy; /* MATTHEW: [4] Remember the display */

  x_pixmap = XCreatePixmap (dpy, RootWindow (dpy, DefaultScreen (dpy)),
			    width, height, depth);

  if (x_pixmap)
    ok = TRUE;
  else
    ok = FALSE;
  return ok;
}

#ifdef wx_xview
void wxBitmap::CreateServerImage(Bool savePixmap)
{
  if (ok)
  {
    x_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
 				      SERVER_IMAGE_SAVE_PIXMAP, savePixmap,
				      SERVER_IMAGE_PIXMAP, x_pixmap,
                                      SERVER_IMAGE_DEPTH, GetDepth(),
						   NULL);
    freePixmap = savePixmap;

    if (x_image)
      ok = TRUE;
    else
      ok = FALSE;
  }
}
#endif

Bool wxBitmap::LoadFile(char *filename, long flags)
{
  if (flags & wxBITMAP_TYPE_XBM)
  {
#ifdef wx_motif
    insensPixmap = 0;
    labelPixmap = 0;
    armPixmap = 0;
    insensImage = 0;
    image = 0;
    width = 0;
    height = 0;
    
    freePixmap = TRUE;
    int hotX, hotY;
    unsigned int w, h;
    Display *dpy = display; /* MATTHEW: [4] Use display */
    int value = XReadBitmapFile (dpy, RootWindow (dpy, DefaultScreen (dpy)),
                        filename, &w, &h, &x_pixmap, &hotX, &hotY);
    width = w;
    height = h;
    depth = 1;
    if ((value == BitmapFileInvalid) ||
        (value == BitmapOpenFailed) ||
        (value == BitmapNoMemory))
      {
        ok = FALSE;
        x_pixmap = 0;
      }
    else
    {
      ok = TRUE;
#if 0
      // We don't have the 'bits' for making
      // XImages :-(
      // Maybe we could create the data from the pixmap???
      
      // code for wxItem
      image = (XImage *) XtMalloc (sizeof (XImage));
      image->width = the_width;
      image->height = the_height;
      image->data = bits;
      image->depth = 1;
      image->xoffset = 0;
      image->format = XYBitmap;
      image->byte_order = LSBFirst;
      image->bitmap_unit = 8;
      image->bitmap_bit_order = LSBFirst;
      image->bitmap_pad = 8;
      image->bytes_per_line = (the_width + 7) >> 3;

      char tmp[128];
      sprintf (tmp, "Im%x", (unsigned int)image);
      XmInstallImage (image, tmp);

      // Build our manually stipped pixmap.

      int bpl = (the_width + 7) / 8;
      char *data = new char[the_height * bpl];
      int i;
      for (i = 0; i < the_height; i++)
      {
        int mask = i % 2 ? 0x55 : 0xaa;
        int j;
        for (j = 0; j < bpl; j++)
  	  data[i * bpl + j] = bits[i * bpl + j] & mask;
      }
      insensImage = (XImage *) XtMalloc (sizeof (XImage));
      insensImage->width = the_width;
      insensImage->height = the_height;
      insensImage->data = data;
      insensImage->depth = 1;
      insensImage->xoffset = 0;
      insensImage->format = XYBitmap;
      insensImage->byte_order = LSBFirst;
      insensImage->bitmap_unit = 8;
      insensImage->bitmap_bit_order = LSBFirst;
      insensImage->bitmap_pad = 8;
      insensImage->bytes_per_line = bpl;
  
      sprintf (tmp, "Not%x", (unsigned int)insensImage);
      XmInstallImage (insensImage, tmp);
#endif
    }
#endif
#ifdef wx_xview
    freePixmap = FALSE;
    x_image = (Server_image) xv_create (XV_NULL, SERVER_IMAGE,
				      SERVER_IMAGE_BITMAP_FILE, filename,
						   NULL);
    if (x_image)
      {
        x_pixmap = (Pixmap) xv_get (x_image, SERVER_IMAGE_PIXMAP);
        depth = (int) xv_get (x_image, SERVER_IMAGE_DEPTH);
        width = (int) xv_get (x_image, XV_WIDTH);
        height = (int) xv_get (x_image, XV_HEIGHT);
        ok = TRUE;
        return TRUE;
      }
    else
    {
      ok = FALSE;
      return FALSE;
    }
#endif
  }
#if USE_XPM_IN_X
  else if (flags & wxBITMAP_TYPE_XPM)
  {
    Display *dpy = display; /* MATTHEW: [4] Use display */

    XpmAttributes xpmAttr;
    Pixmap  pixmap;
    Pixmap mask = 0;

    ok = FALSE;
    xpmAttr.valuemask = XpmReturnInfos | XpmCloseness;
    xpmAttr.closeness = 40000;
    int errorStatus = XpmReadFileToPixmap(dpy,
        RootWindow(dpy, DefaultScreen(dpy)), filename,
        &pixmap, &mask, &xpmAttr);
    if (errorStatus == XpmSuccess)
    {
        x_pixmap = pixmap;
        if ( mask )
        {
            bitmapMask = new wxMask;
            bitmapMask->SetMaskBitmap(mask);
        }

	width=xpmAttr.width;
	height = xpmAttr.height;
	if ( xpmAttr.npixels > 2 ) {
		depth = 8;	// next time not just a guess :-) ...
	} else {
		depth = 1;	// mono	
	}

	numColors = xpmAttr.npixels;

	XpmFreeAttributes(&xpmAttr);

        ok = TRUE;
    } else
    {
//      XpmDebugError(errorStatus, filename);
      ok = FALSE;
      return FALSE;
    }
// ADDED JACS
#ifdef wx_xview
    CreateServerImage(TRUE);
#endif
    return ok;
  }
#endif
#if USE_IMAGE_LOADING_IN_X
  else if ((flags & wxBITMAP_TYPE_ANY) || (flags & wxBITMAP_TYPE_BMP) ||
           (flags & wxBITMAP_TYPE_GIF))
  {
    // BUGBUG: no depth information returned!!!
    wxColourMap *cmap = NULL;
    Bool success = FALSE;
    if (flags & wxBITMAP_DISCARD_COLOURMAP)
      success = wxLoadIntoBitmap(filename, this);
    else
      success = wxLoadIntoBitmap(filename, this, &cmap);
    if (!success && cmap)
    {
      delete cmap;
      cmap = NULL;
    }
    if (cmap)
      bitmapColourMap = cmap;
#ifdef wx_xview
    CreateServerImage(TRUE);
#endif
    return success;
  }
#endif
  return FALSE;
}

Bool wxBitmap::SaveFile(char *filename, int typ, wxColourMap *cmap)
{
  switch (typ)
  {
#if USE_IMAGE_LOADING_IN_X
/* IMAGE SAVING NOT IN X YET
    case wxBITMAP_TYPE_BMP:
    {
      wxColourMap *actualCmap = cmap;
      if (!actualCmap)
        actualCmap = bitmapColourMap;
      return wxSaveBitmap(filename, this, actualCmap);
      break;
    }
 */
#endif
#if USE_XPM_IN_X && defined(wx_x) && !defined(VMS)
    case wxBITMAP_TYPE_XPM:
    {
      if (ok && x_pixmap)
      {
        Display *dpy =  display; /* MATTHEW: [4] Use display */
        int errorStatus = XpmWriteFileFromPixmap(dpy, filename,
          x_pixmap, (bitmapMask ? bitmapMask->GetMaskBitmap() : (Pixmap) 0), (XpmAttributes *) NULL);
        if (errorStatus == XpmSuccess)
          return TRUE;
        else
          return FALSE;
      }
      else
        return FALSE;
      break;
    }
#endif
    default:
      break;
  }
  return FALSE;
}

#ifdef wx_motif
Pixmap wxBitmap::GetLabelPixmap (Widget w)
{
  if (image == 0)
    return x_pixmap;

  Display *dpy = display; /* MATTHEW: [4] Use display */

#ifdef FOO
/*
   If we do:
   if (labelPixmap) return labelPixmap;
   things can be wrong, because colors can have been changed.

   If we do:
   if (labelPixmap)
   XmDestroyPixmap(DefaultScreenOfDisplay(dpy),labelPixmap) ;
   we got BadDrawable if the pixmap is referenced by multiples widgets

   this is a catch22!!

   So, before doing thing really clean, I just do nothing; if the pixmap is
   referenced by many widgets, Motif performs caching functions.
   And if pixmap is referenced with multiples colors, we just have some
   memory leaks... I hope we can deal with them...
 */
  // Must be destroyed, because colours can have been changed!
  if (labelPixmap)
    XmDestroyPixmap (DefaultScreenOfDisplay (dpy), labelPixmap);
#endif

  char tmp[128];
  sprintf (tmp, "Im%x", (unsigned int)image);

  Pixel fg, bg;

  while (XmIsGadget (w))
    w = XtParent (w);
  XtVaGetValues (w, XmNbackground, &bg, XmNforeground, &fg, NULL);

  labelPixmap = XmGetPixmap (DefaultScreenOfDisplay (dpy), tmp, fg, bg);

  return (labelPixmap);
}

Pixmap wxBitmap::GetArmPixmap (Widget w)
{
  if (image == 0)
    return x_pixmap;

  Display *dpy =display; /* MATTHEW: [4] Use display */
#ifdef FOO
  See GetLabelPixmap () comment
  // Must be destroyed, because colours can have been changed!
  if (armPixmap)
      XmDestroyPixmap (DefaultScreenOfDisplay (dpy), armPixmap);
#endif

  char tmp[128];
  sprintf (tmp, "Im%x", (unsigned int)image);

  Pixel fg, bg;

  XtVaGetValues (w, XmNarmColor, &bg, NULL);
  while (XmIsGadget (w))
    w = XtParent (w);
  XtVaGetValues (w, XmNforeground, &fg, NULL);

  armPixmap = XmGetPixmap (DefaultScreenOfDisplay (dpy), tmp, fg, bg);

  return (armPixmap);
}

Pixmap wxBitmap::GetInsensPixmap (Widget w)
{
  Display *dpy = display;  /* MATTHEW: [4] Use display */

  if (insensImage == 0)
    return x_pixmap;

#ifdef FOO
  See GetLabelPixmap () comment
  // Must be destroyed, because colours can have been changed!
  if (insensPixmap)
      XmDestroyPixmap (DefaultScreenOfDisplay (dpy), insensPixmap);
#endif

  char tmp[128];
  sprintf (tmp, "Not%x", (unsigned int)insensImage);

  Pixel fg, bg;

  while (XmIsGadget (w))
    w = XtParent (w);
  XtVaGetValues (w, XmNbackground, &bg, XmNforeground, &fg, NULL);

  insensPixmap = XmGetPixmap (DefaultScreenOfDisplay (dpy), tmp, fg, bg);

  return (insensPixmap);
}

#endif

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
    // TODO: this may be the wrong display
    if ( m_maskBitmap )
        XFreePixmap (wxGetDisplay(), m_maskBitmap);
}

// We may need this sometime...

#ifdef wx_motif
/****************************************************************************

NAME
	XCreateInsensitivePixmap - create a grayed-out copy of a pixmap

SYNOPSIS
	Pixmap XCreateInsensitivePixmap( Display *display, Pixmap pixmap )

DESCRIPTION
	This function creates a grayed-out copy of the argument pixmap, suitable
	for use as a XmLabel's XmNlabelInsensitivePixmap resource.

RETURN VALUES
	The return value is the new Pixmap id or zero on error.  Errors include
	a NULL display argument or an invalid Pixmap argument.

ERRORS
	If one of the XLib functions fail, it will produce a X error.  The
	default X error handler prints a diagnostic and calls exit().

SEE ALSO
	XCopyArea(3), XCreateBitmapFromData(3), XCreateGC(3), XCreatePixmap(3),
	XFillRectangle(3), exit(2)

AUTHOR
	John R Veregge - john@puente.jpl.nasa.gov
	Advanced Engineering and Prototyping Group (AEG)
	Information Systems Technology Section (395)
	Jet Propulsion Lab - Calif Institute of Technology

*****************************************************************************/

Pixmap
XCreateInsensitivePixmap( Display *display, Pixmap pixmap )

{
static
	char	stipple_data[] = 
		{
			0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA,
			0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA,
			0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA,
			0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA 
		};
    GC		gc;
    Pixmap	ipixmap, stipple;
    unsigned	width, height, depth;

    Window	window;	/* These return values */
    unsigned	border;	/* from XGetGeometry() */
    int		x, y;	/* are not needed.     */

    ipixmap = 0;

    if ( NULL == display || 0 == pixmap )
	return ipixmap;

    if ( 0 == XGetGeometry( display, pixmap, &window, &x, &y,
			    &width, &height, &border, &depth )
       )
	return ipixmap; /* BadDrawable: probably an invalid pixmap */

    /* Get the stipple pixmap to be used to 'gray-out' the argument pixmap.
    */
    stipple = XCreateBitmapFromData( display, pixmap, stipple_data, 16, 16 );
    if ( 0 != stipple )
    {
	gc = XCreateGC( display, pixmap, (XtGCMask)0, (XGCValues*)NULL );
	if ( NULL != gc )
	{
	    /* Create an identical copy of the argument pixmap.
	    */
	    ipixmap = XCreatePixmap( display, pixmap, width, height, depth );
	    if ( 0 != ipixmap )
	    {
		/* Copy the argument pixmap into the new pixmap.
		*/
		XCopyArea( display, pixmap, ipixmap,
			   gc, 0, 0, width, height, 0, 0 );

		/* Refill the new pixmap using the stipple algorithm/pixmap.
		*/
		XSetStipple( display, gc, stipple );
		XSetFillStyle( display, gc, FillStippled );
		XFillRectangle( display, ipixmap, gc, 0, 0, width, height );
	    }
	    XFreeGC( display, gc );
	}
	XFreePixmap( display, stipple );
    }
    return ipixmap;
}
#endif

// Return the system colour for a particular GUI element
wxColour wxGetSysColour(int sysColour)
{
  // Not implemented
  wxColour col(0, 0, 0);
  return col;
}
