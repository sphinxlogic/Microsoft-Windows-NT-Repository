#include <wx.h>
#include <stdlib.h>
#include <string.h>
#include "wxima.h"

#if WXIMA_SUPPORT_JPEG
#include "imajpg.h"
#endif

#if WXIMA_SUPPORT_GIF
#include "imagif.h"
#endif

#if WXIMA_SUPPORT_PNG
#include "imapng.h"
#endif

#if WXIMA_SUPPORT_BMP
#include "imabmp.h"
#endif

wxColourMap *wxCopyPalette(const wxColourMap *cmap);

struct FORMAT_LIST {
  char *ext;
  int Type;
} Image_Format_List[] = {
	"BMP", IMAGE_FORMAT_BMP,
	"GIF", IMAGE_FORMAT_GIF,
	"JPG", IMAGE_FORMAT_JPEG,
	"PNG", IMAGE_FORMAT_PNG,
	"XPM", IMAGE_FORMAT_XPM
};

char *GetExtension( char* FileName )
{
  char* p = &FileName[strlen(FileName)-1];

  if (*p=='.') return 0;

  for (int i=strlen(FileName); i > 0 && *(p-1)!='.'; i--) p--;

  return p;
}


int search_formats(char *format)
{
	int l = 0, r = MAX_IMAGE_FORMATS, m, k;
//	printf("{%s}", format);
	while (l<r) {
	  m = (l+r)/2;
	  k = strcmpi(Image_Format_List[m].ext, format);
	  if (k==0)
		 return Image_Format_List[m].Type;
	  else
		 if (k<0) l = m+1; else r = m;
	}

	return -1;
}

ImaFile *GetImaFile( char* ImageFileName, int& ImageType)
{
	ImaFile* imaf;

//	filetype = ImageType;
	switch (ImageType) {
	
#if WXIMA_SUPPORT_PNG
	case IMAGE_FORMAT_PNG:
	{
	  imaf = new ImaPNG(ImageFileName);
	  break;
	}
#endif

#if WXIMA_SUPPORT_JPEG
	case IMAGE_FORMAT_JPEG:
	{
	  imaf = new ImaJPG(ImageFileName);
	  break;
	}
#endif

#if WXIMA_SUPPORT_BMP
	case IMAGE_FORMAT_BMP:
	{
	  imaf =  new ImaBMP(ImageFileName);
//	  printf("BMP");
	  break;
	}
#endif

#if WXIMA_SUPPORT_GIF
	case IMAGE_FORMAT_GIF:
	{
	  imaf = new ImaGIF(ImageFileName);
	  break;
	}
#endif

	default:
	  imaf = 0;  
//	  filetype = 0;

//	  wxMessageBox("Unrecognized wxImage format", "Warning");
	}

	return imaf;
}

wxImage::wxImage ( char* ImageFileName, int ImageType)
{
	readfile (ImageFileName, ImageType);
}

BOOL
wxImage::readfile( char* ImageFileName, int ImageType)
{
	if (ImageFileName)
	  strcpy(filename, ImageFileName);
	if (ImageType==-1) {
	  ImageType = search_formats(GetExtension(filename));
	}

#if USE_XPM_IN_MSW && WXIMA_SUPPORT_XPM
	if (ImageType == IMAGE_FORMAT_XPM) {
	  wxBitmap *bitmap=new wxBitmap(ImageFileName, wxBITMAP_TYPE_XPM);
	  if (bitmap) {
		 *this = *(new wxImage(bitmap));
		 delete bitmap;
		 return TRUE;
	  } else
			return FALSE;
	}
#endif
	ImaFile *imaf = GetImaFile(ImageFileName, ImageType);
	if (imaf) {
	  if (imaf->readfile())
	    *this = *((wxImage*)imaf);
          imaf->NullData();
          delete imaf;
	  return TRUE;
	} else
			return FALSE;
}

BOOL
wxImage::savefile( char* ImageFileName, int ImageType)
{
	if (ImageFileName)
	  strcpy(filename, ImageFileName);

	if (ImageType==-1) {
	  ImageType = search_formats(GetExtension(filename));
	}

	ImaFile *imaf = GetImaFile(ImageFileName, ImageType);
	if (imaf) {
	  *((wbIma*)imaf) = *((wbIma*)this);
	  imaf->savefile();
          imaf->NullData();
          delete imaf;
	  return TRUE;
	} else
			return FALSE;

}


wxBitmap* wbIma::GetBitmap(void)
{
	HDC dc = ::CreateCompatibleDC(NULL);

	if (dc)
	{
        // tmpBitmap is a dummy, to satisfy ::CreateCompatibleDC (it
        // is a memory dc that must have a bitmap selected into it)
        HDC dc2 = GetDC(NULL);
        HBITMAP tmpBitmap = ::CreateCompatibleBitmap(dc2, GetWidth(), GetHeight());
        ReleaseDC(NULL, dc2);
        HBITMAP oldBitmap = ::SelectObject(dc, tmpBitmap);

        if ( ColourMap )
        {
            HPALETTE oldPal = ::SelectPalette(dc, ColourMap->ms_palette, FALSE);
		    ::RealizePalette(dc);
        }

		HBITMAP hBitmap = ::CreateDIBitmap(dc, lpbi,
		    CBM_INIT, RawImage, (LPBITMAPINFO) lpbi, DIB_PAL_COLORS);

        ::SelectPalette(dc, NULL, TRUE);
        ::SelectObject(dc, oldBitmap);
        ::DeleteObject(tmpBitmap);
        ::DeleteDC(dc);

        if ( hBitmap )
        {
		  wxBitmap *bitmap = new wxBitmap;
          bitmap->ms_bitmap = hBitmap;
          bitmap->SetWidth(GetWidth());
          bitmap->SetHeight(GetHeight());
          bitmap->SetDepth(GetDepth());
          if ( GetDepth() > 1 && ColourMap )
            bitmap->SetColourMap(wxCopyPalette(ColourMap));
          bitmap->SetOk(TRUE);

		  return bitmap;
        }
        else
        {
          return NULL;
        }
	}
	else
    {
		return NULL;
    }
}

wxColourMap *wxCopyPalette(const wxColourMap *cmap)
{
  // To get number of entries...
  WORD count = 0;
  ::GetObject(cmap->ms_palette, sizeof(WORD), &count);
  
  LOGPALETTE* logPal = (LOGPALETTE*)
		 new BYTE[sizeof(LOGPALETTE) + count*sizeof(PALETTEENTRY)];
  logPal->palVersion = 0x300;
  logPal->palNumEntries = count;
  ::GetPaletteEntries(cmap->ms_palette, 0, count, logPal->palPalEntry);
  
  HPALETTE hPalette = ::CreatePalette(logPal);
  delete[] logPal;

  wxColourMap *newCmap = new wxColourMap;
  newCmap->ms_palette = hPalette;
  return newCmap;
}


