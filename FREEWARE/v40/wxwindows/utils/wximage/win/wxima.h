/*
 * File:	image.h
 * Purpose:	Declaration of the Platform Independent Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 * This software is based in part on the work of the Independent JPEG Group.
 *
	Last change:  JS   29 May 97   12:06 pm
 */
#if !defined(__Image_h)
#define __Image_h

#include "wbima.h"

enum {
IMAGE_FORMAT_BMP,
IMAGE_FORMAT_JPEG,
IMAGE_FORMAT_GIF,
IMAGE_FORMAT_XPM,
IMAGE_FORMAT_PNG,
MAX_IMAGE_FORMATS
};

#define wxIma wxImage

class wxImage: public wbIma
{
protected:
  int filetype;
  char filename[255];
public:
  wxImage(void): wbIma() { }
  wxImage(wxBitmap* bitmap): wbIma(bitmap) { }
  wxImage (char* ImageFileName, int ImageType = -1);     // Read an image file
  ~wxImage () {};

  BOOL readfile( char* ImageFileName=0, int ImageType = -1);
  BOOL savefile( char* ImageFileName=0, int ImageType = -1);

  int GetFileType(void) { return filetype; }
  char *GetFilename() { return &filename[0]; }
};

#endif
