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
#if !defined(__ImaBMP_h)
#define __ImaBMP_h

#include "imafile.h"

class ImaBMP: public ImaFile
{
public:
  ImaBMP(const wbIma* ima): ImaFile(ima) {}
  ImaBMP( char* ImageFileName ): ImaFile(ImageFileName) {}
  ~ImaBMP() {};

  BOOL readfile( char* ImageFileName=0);
  BOOL savefile( char* ImageFileName=0);
};

#endif

