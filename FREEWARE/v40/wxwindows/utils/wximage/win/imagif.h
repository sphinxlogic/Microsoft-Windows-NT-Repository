/*
 * File:	imagif.h
 * Purpose:	Declaration of the Platform Independent GIF Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 */
#if !defined(__ImaGIF_h)
#define __ImaGIF_h

#include "imafile.h"

class ImaGIF: public ImaFile
{
public:
  ImaGIF(const wbIma* ima): ImaFile(ima)
  {
  }
  ImaGIF(char* ImageFileName): ImaFile(ImageFileName)
  {
  }
  ~ImaGIF() {};

  virtual BOOL readfile( char* ImageFileName=0);
  virtual BOOL savefile( char* ImageFileName=0) { return FALSE; };
};

#endif

