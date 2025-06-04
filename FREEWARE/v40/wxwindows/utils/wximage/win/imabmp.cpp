/*
 * File:	wbimage.cc
 * Purpose:	Platform Independent Image Base Class (Windows version)
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995 Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 */
#include "imabmp.h"

#if WXIMA_SUPPORT_BMP
#include "dibutils.h"

//BOOL WriteDIB(LPSTR szFile, HANDLE hdib);

BOOL ImaBMP::readfile(char* ImageFileName)
{
  if (ImageFileName)
	 strcpy(filename, ImageFileName);

  if (lpbi = DibOpenFile(filename))  {
		Width =  DibWidth(lpbi);
		Height = DibHeight(lpbi);
		Depth = DibBitCount(lpbi);
		RawImage = (ImagePointerType)DibPtr(lpbi);
		EfeWidth = (long)(((long)Width*Depth + 31) / 32) * 4;
		HPALETTE palette;
		if (palette = MakePalette((const BITMAPINFO FAR*)lpbi, 0))
		{
		  ColourMap = new wxColourMap;
		  ColourMap->ms_palette = palette;
		  DibSetUsage(lpbi, ColourMap->ms_palette, WXIMA_COLORS);
		  ColorType = (COLORTYPE_PALETTE | COLORTYPE_COLOR);
		} 

		return TRUE;
  }
  return FALSE;
}


BOOL ImaBMP::savefile(char* ImageFileName)
{                         
  if (ImageFileName)
	 strcpy(filename, ImageFileName);

//  return WriteDIB(ImageFileName, HandleFromDib(lpbi));
  DibSetUsage(lpbi, ColourMap->ms_palette, DIB_RGB_COLORS);
  if (!DibWriteFile(filename, lpbi))
	return FALSE;
	else return TRUE;
}

#endif 	// WXIMA_SUPPORT_BMP
