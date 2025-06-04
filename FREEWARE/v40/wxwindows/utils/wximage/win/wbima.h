/*
 * File:	wxIma.h
 * Purpose:	Declaration of the Platform Independent Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Description: This class adds drawing routines and color map management.
 *
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
	Last change:  JS   29 May 97   12:06 pm
 */
#if !defined(__wbIma_h)
#define __wbIma_h

#include <wx_gdi.h>
#include <windowsx.h>

#define WXIMA_COLORS DIB_PAL_COLORS

#define WXIMA_SUPPORT_BMP 1
#define WXIMA_SUPPORT_GIF 1
#define WXIMA_SUPPORT_JPEG 0
#define WXIMA_SUPPORT_PNG  1
#define WXIMA_SUPPORT_XPM  1

#ifndef byte
typedef unsigned char byte;
#endif

#ifdef WIN32
typedef byte* ImagePointerType;
#else
typedef byte huge* ImagePointerType;
#endif

#include "imabase.h"

typedef struct
{
	byte red;
	byte green;
   byte blue;
} rgb_color_struct;

class wbIma: public ImaBase
{
protected:
#ifdef wx_msw
  LPBITMAPINFOHEADER lpbi;
#endif
  int bgindex;
  wxColourMap* ColourMap;
public:
// Constructors
  wbIma();
  wbIma(int width, int height, int depth, int colortype=-1);
  wbIma( const wbIma *ima );
  wbIma( const wxBitmap*ima );   // Constructs from a wxBitmap object.

// Destructor
  ~wbIma();

  void Create(int width, int height, int deep, int colortype=-1);

// Drawing routines
  Bool Draw(wxDC *dc, int x=0, int y=0, int dx=-1, int dy=-1, int xs=0, int ys=0);
  Bool Stretch(wxDC *dc, int xd=0, int yd=0, int dxd=-1, int dyd=-1,
	 int xs=0, int ys=0, int dxs=-1, int dys=-1);

//  Image Information  (See ImaBase.h)
//  int  GetWidth( void ) { return Width; };
//  int  GetHeight( void ) { return Height; };
//  int  GetDepth( void ) { return Depth; };        
//  Bool Inside(int x, int y);
  int  GetIndex(int x, int y);
  Bool GetRGB(int x, int y, byte* r, byte* g, byte* b);

  Bool SetIndex(int x, int y, int index);
  Bool SetRGB(int x, int y, byte r, byte g, byte b);

// ColorMap settings
  BOOL SetColourMap(wxColourMap* colourmap);
  BOOL SetColourMap(int n, rgb_color_struct *rgb_struct);
  BOOL SetColourMap(int n, byte *r, byte *g=0, byte *b=0);
  wxColourMap* GetColourMap() const { return ColourMap; }
  
  void NullData();
  inline int GetBGIndex(void) { return bgindex; }

  virtual wxBitmap *GetBitmap(void);
};


inline
wbIma::wbIma(): /*ImaBase(),*/ ColourMap(0), lpbi(0)
{
  bgindex = -1;
}

inline
wbIma::wbIma(int width, int height, int depth, int colortype): ColourMap(0), lpbi(0)
{
  bgindex = -1;
  RawImage = 0;
  Create(width, height, depth, colortype);
}


inline
wbIma::wbIma( const wbIma *ima ): /*ImaBase(),*/ ColourMap(0), lpbi(0)
{
  bgindex = -1;

  if (ima) {
	 Create(ima->GetWidth(), ima->GetHeight(), ima->GetDepth(), ima->GetColorType());
	 SetColourMap(ima->GetColourMap());
  }
}

#endif
