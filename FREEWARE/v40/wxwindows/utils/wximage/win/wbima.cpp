/*
 * File:	wbimage.cc
 * Purpose:	Platform Independent Image Base Class (Windows version)
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995 Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 */
//#include <stdio.h>
#include <wx_gdi.h>
#include <wx_utils.h>
#include <windowsx.h>
#include "wbima.h"
#include "dibutils.h"
#include "wx_canvs.h"
#include "wx_privt.h"

void
wbIma::Create(int width, int height, int depth, int colortype)
{
  Width = width; Height = height; Depth = depth;
  ColorType = (colortype>=0) ? colortype: ((Depth>8) ? COLORTYPE_COLOR: 0);

  if (lpbi)  {
	 GlobalFreePtr(lpbi);
//	 delete ColourMap;
  }
  RawImage = 0;
  ColourMap = 0;
  if (lpbi = DibCreate(Depth, Width, Height))  {
		RawImage = (ImagePointerType)DibPtr(lpbi);
		EfeWidth = (long)(((long)Width*Depth + 31) / 32) * 4;
  }
}


wbIma::wbIma( const wxBitmap* wxbmp)
{
  bgindex = -1;
  lpbi = 0;
  RawImage = 0;
  ColourMap = 0;
  if (wxbmp) {
         Create(((wxBitmap*)wxbmp)->GetWidth(), ((wxBitmap*)wxbmp)->GetHeight(), ((wxBitmap*)wxbmp)->GetDepth());
         GetDIBits(GetDC(NULL), ((wxBitmap*)wxbmp)->ms_bitmap, 0, GetHeight(),
		RawImage, (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);
         SetColourMap(((wxBitmap*)wxbmp)->GetColourMap());
  }
}

wbIma::~wbIma ( )
{
  if (lpbi)  {
	 GlobalFreePtr(lpbi);
	 delete ColourMap;
  }
}


int wbIma::GetIndex(int x, int y)
{
  if (!Inside(x, y) || (Depth>8)) return -1;

  ImagePointerType ImagePointer = RawImage + EfeWidth*y + (x*Depth >> 3);
  int index = (int)(*ImagePointer);
  return index;
}

Bool wbIma::GetRGB(int x, int y, byte* r, byte* g, byte* b)
{
  if (!Inside(x, y)) return FALSE;

  if (ColourMap) {
	 return ColourMap->GetRGB(GetIndex(x, y), r, g, b);
/*	 PALETTEENTRY entry;
	 ::GetPaletteEntries(ColourMap->ms_palette, GetIndex(x, y), 1, &entry);
	 *r = entry.peRed;
	 *g = entry.peGreen;
	 *b = entry.peBlue;  */
  } else {
	 ImagePointerType ImagePointer = RawImage + EfeWidth*y + (x*Depth >> 3);
	 *b = ImagePointer[0];
	 *g = ImagePointer[1];
	 *r = ImagePointer[2];
  }
  return TRUE;
}


Bool wbIma::SetIndex(int x, int y, int index)
{
  if (!Inside(x, y) || (Depth>8)) return FALSE;

  ImagePointerType ImagePointer = RawImage + EfeWidth*y + (x*Depth >> 3);
  *ImagePointer = index;

  return TRUE;
}

Bool wbIma::SetRGB(int x, int y, byte r, byte g, byte b)
{
  if (!Inside(x, y)) return FALSE;

  if (ColorType & COLORTYPE_PALETTE)
  {
	 if (!ColourMap) return FALSE;
	 SetIndex(x, y, ColourMap->GetPixel(r, g, b));

  } else {
	 ImagePointerType ImagePointer = RawImage + EfeWidth*y + (x*Depth >> 3);
	 ImagePointer[0] = b;
	 ImagePointer[1] = g;
	 ImagePointer[2] = r;
  }

  return TRUE;
}

Bool wbIma::SetColourMap(wxColourMap* colourmap)
{
  if (!colourmap)
	 return FALSE;
  ColorType |= (COLORTYPE_PALETTE | COLORTYPE_COLOR);
  ColourMap = colourmap;
  return DibSetUsage(lpbi, ColourMap->ms_palette, WXIMA_COLORS );
}

Bool
wbIma::SetColourMap(int n, byte *r, byte *g, byte *b)
{
  ColourMap = new wxColourMap();
  if (!ColourMap)
	 return FALSE;

  if (!g) g = r;
  if (!b) b = g;
  ColourMap->Create(n, r, g, b);
  ColorType |= (COLORTYPE_PALETTE | COLORTYPE_COLOR);
  return DibSetUsage(lpbi, ColourMap->ms_palette, WXIMA_COLORS );
}

Bool
wbIma::SetColourMap(int n, rgb_color_struct *rgb_struct)
{
  ColourMap = new wxColourMap();
  if (!ColourMap)
	 return FALSE;

  byte r[256], g[256], b[256];

  for(int i=0; i<n; i++)
  {
	 r[i] = rgb_struct[i].red;
	 g[i] = rgb_struct[i].green;
	 b[i] = rgb_struct[i].blue;
  }
  ColourMap->Create(n, r, g, b);
  ColorType |= (COLORTYPE_PALETTE | COLORTYPE_COLOR);
  return DibSetUsage(lpbi, ColourMap->ms_palette, WXIMA_COLORS );
}

Bool wbIma::Draw(wxDC *wxdc, int x, int y, int dx, int dy, int xs, int ys)
{
  if (lpbi)
  {
	 HDC dc = 0;
	 wxWnd *wnd = NULL;

	 if (wxdc->cdc) {
		dc = wxdc->cdc;
		wnd = (wxWnd *)wxdc->canvas->handle;
		wnd->CalcScrolledPosition(x, y, &x, &y);
	 }
	 else if (wxdc->canvas) {
		wnd = (wxWnd *)wxdc->canvas->handle;
		wnd->CalcScrolledPosition(x, y, &x, &y);
#if wxVERSION_NUMBER > 1605
		dc =  wxdc->canvas->GetHDC();
#else
		dc =  wnd->GetHDC();
#endif
	 }


	 if (dc)   {
		::RealizePalette(dc);
		if (dx==-1)  dx = GetWidth();
		if (dy==-1)  dy = GetHeight();

		SetDIBitsToDevice(dc, x, y, dx, dy, xs, 0, ys, dy,
			RawImage, (BITMAPINFO *)lpbi, WXIMA_COLORS );
	 }
#if wxVERSION_NUMBER > 1605
	wxdc->canvas->ReleaseHDC();
#else
	((wxWnd *)wxdc->canvas->handle)->ReleaseHDC();
#endif
	 return TRUE;
  }
  else
	 return FALSE;

}

Bool wbIma::Stretch(wxDC *wxdc, int xd, int yd, int dxd, int dyd,
	 int xs, int ys, int dxs, int dys)
{
  if (lpbi)
  {
	 HDC dc = 0;
	 wxWnd *wnd = NULL;
	 if (wxdc->cdc)
		dc = wxdc->cdc;
	 else if (wxdc->canvas) {
		wnd = (wxWnd *)wxdc->canvas->handle;
		wnd->CalcScrolledPosition(xd, yd, &xd, &yd);
#if wxVERSION_NUMBER > 1605
		dc =  wxdc->canvas->GetHDC();
#else
		dc =  wnd->GetHDC();
#endif
	 }

	 if (dc)   {
		if (dxd==-1) dxd = GetWidth();
		if (dyd==-1) dyd = GetHeight();
		if (dxs==-1) dxs = Width - xs;
		if (dys==-1) dys = Height - ys;
		::RealizePalette(dc);

#ifdef WIN32       // Paul Shirley's patch
		SetStretchBltMode(dc, COLORONCOLOR);
#else
		SetStretchBltMode(dc, STRETCH_DELETESCANS);
#endif

	  if (bgindex == -1)
	  {
			StretchDIBits(dc, xd, yd, dxd, dyd, xs, ys, dxs, dys,
				RawImage, (BITMAPINFO *)lpbi, WXIMA_COLORS, SRCCOPY);
		}
		else
		{
			//HDC tmpdc = CreateCompatibleDC(dc);
			//HBITMAP tmpbmp = CreateCompatibleBitmap(dc, dxd, dyd);
			//SelectObject(tmpdc, tmpbmp);
			//BitBlt(tmpdc, 0, 0, dxd, dyd, dc, xd, yd, SRCCOPY);

			unsigned char* save_index = new unsigned char[Height*Width];
			int black = ColourMap->GetPixel(0, 0, 0);
			int white = ColourMap->GetPixel(255, 255, 255);

			// Make background white and foreground black to use as mask...

			int x, y;

			for (y = 0; y < Height; y++)
				for (x = 0; x < Width; x++)
				{
					long i = ((long)y*Width)+x;
					save_index[i] = (unsigned char)GetIndex(x, y);

					if (GetIndex(x, y) == bgindex)
						SetIndex(x, y, white);
					else
						SetIndex(x, y, black);
				}

			// AND into the canvas, this will preserve the background but
			// turn the foreground white...

			//StretchDIBits(tmpdc, 0, 0, dxd, dyd, xs, ys, dxs, dys,
			StretchDIBits(dc, xd, yd, dxd, dyd, xs, ys, dxs, dys,
				RawImage, (BITMAPINFO *)lpbi, WXIMA_COLORS, SRCAND);

			// Restore foreground & make background black...

			for (y = 0; y < Height; y++)
				for (x = 0; x < Width; x++)
				{
					long i = ((long)y*Width)+x;
					SetIndex(x, y, save_index[i]);

					if (GetIndex(x, y) == bgindex)
						SetIndex(x, y, black);
				}

			// OR into the canvas, this will preserve the background but
			// copy the foreground...

			//StretchDIBits(tmpdc, 0, 0, dxd, dyd, xs, ys, dxs, dys,
			StretchDIBits(dc, xd, yd, dxd, dyd, xs, ys, dxs, dys,
				RawImage, (BITMAPINFO *)lpbi, WXIMA_COLORS, SRCPAINT);

			for (y = 0; y < Height; y++)
				for (x = 0; x < Width; x++)
				{
					long i = ((long)y*Width)+x;
					SetIndex(x, y, save_index[i]);
				}

			delete [] save_index;

			//BitBlt(dc, xd, yd, dxd, dyd, tmpdc, 0, 0, SRCCOPY);
			//DeleteDC(tmpdc);
			//DeleteObject(tmpbmp);
		}
#if wxVERSION_NUMBER > 1605
		wxdc->canvas->ReleaseHDC();
#else
		((wxWnd *)wxdc->canvas->handle)->ReleaseHDC();
#endif
	 }
	 return TRUE;
  }
  else
	 return FALSE;
}

void wbIma::NullData()
{
  lpbi = NULL;
  ColourMap = NULL;
}
