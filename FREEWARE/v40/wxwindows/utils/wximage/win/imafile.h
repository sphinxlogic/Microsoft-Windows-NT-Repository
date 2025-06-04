/*
 * File:	imafile.h
 * Purpose:	Declaration of the Platform Independent File Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 *
 */
#if !defined(__ImaFile_h)
#define __ImaFile_h

#include "wbima.h"

class ImaFile: public wbIma
{
protected:
  char filename[255];
public:
  ImaFile(): wbIma() {}
  ImaFile(const wbIma* ima): wbIma(ima) {}
  ImaFile(char* ImageFileName);
  virtual BOOL readfile( char* ImageFileName=0)=0;
  virtual BOOL savefile( char* ImageFileName=0)=0;
};

inline
ImaFile::ImaFile(char* ImageFileName)
{
  strcpy(filename, ImageFileName);
}

#endif
