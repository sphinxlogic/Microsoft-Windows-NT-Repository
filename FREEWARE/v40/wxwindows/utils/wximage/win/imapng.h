/*
 * File:	ImaPNG.h
 * Purpose:	Declaration of the PNG Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 *
 */
#if !defined(__ImaPNG_h)
#define __ImaPNG_h

#include "imafile.h"

#if WXIMA_SUPPORT_PNG
class ImaPNG: public ImaFile
{
protected:
  wbIma *imabg;

public:
  ImaPNG(const wbIma* ima): ImaFile(ima) { imabg=0; }
  ImaPNG( char* ImageFileName ): ImaFile(ImageFileName) { imabg=0; }
  ~ImaPNG() {};

  BOOL readfile( char* ImageFileName=0);
  BOOL savefile( char* ImageFileName=0);

//  SetBackground(int r, int g, int b);
  void SetBackground(wbIma* bg) { imabg = bg; }
};

#endif

#endif

