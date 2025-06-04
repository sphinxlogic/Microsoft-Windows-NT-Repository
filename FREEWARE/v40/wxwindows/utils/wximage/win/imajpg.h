/*
 * File:	imajpg.h
 * Purpose:	Declaration of the Platform Independent JPEG Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 * This software is based in part on the work of the Independent JPEG Group.
 *
 */
#if !defined(__ImaJPEG_h)
#define __ImaJPEG_h

#include "imafile.h"

#if WXIMA_SUPPORT_JPEG
class ImaJPG: public ImaFile
{
protected:
  int quality;
  void CreateGrayColourMap(int n);
public:
  ImaJPG(const wbIma* ima): ImaFile(ima) { quality = 30; }
  ImaJPG( char* ImageFileName ): ImaFile(ImageFileName) { quality = 30; }
  ~ImaJPG() {};

  BOOL readfile( char* ImageFileName=0);
  BOOL savefile( char* ImageFileName=0);
};

#endif

#endif

