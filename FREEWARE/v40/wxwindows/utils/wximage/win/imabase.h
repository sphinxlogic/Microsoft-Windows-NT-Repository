/*
 * File:	ImaBase.h
 * Purpose:	Declaration of the Platform Independent Image Base Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 *  NOTE: This is an abstract class. You can't access it directly.
 *
 *         ImaPointerType must be defided by a derived class before reading
 *			  this file, i.e.  typedef byte* ImagePointerType;
 *
	Last change:  JS   29 May 97   10:09 am
 */
#if !defined(__ImaBase_h)
#define __ImaBase_h

#ifndef byte
typedef unsigned char byte;
#endif

#define COLORTYPE_PALETTE	1
#define COLORTYPE_COLOR		2
#define COLORTYPE_ALPHA		4


class ImaBase
{
protected:
  ImagePointerType RawImage;  		//  Image data

  int Width, Height;   		//  Dimensions
  int Depth;	 				// (bits x pixel)
  int ColorType;				// Bit 1 = Palette used
									// Bit 2 = Color used
									// Bit 3 = Alpha used

  long EfeWidth;	 			// Efective Width

friend class ImaIter;
public:
// Constructors
  ImaBase( void );
//  ImaBase(int width, int height, int depth, int colortype=-1);
//  ImaBase(const ImaBase *ima)=0;

  virtual void Create(int width, int height, int deep=8, int colortype=-1)=0;

//  Image Information
  int  GetWidth( void ) const { return Width; };
  int  GetHeight( void ) const { return Height; };
  int  GetDepth( void ) const { return Depth; };
  int  GetColorType( void ) const { return ColorType; };

  virtual int  GetIndex(int x, int y)=0; 
  Bool GetRGB(int x, int y, byte* r, byte* g, byte* b);

  virtual Bool SetIndex(int x, int y, int index)=0;
  virtual Bool SetRGB(int x, int y, byte r, byte g, byte b)=0;

  Bool Inside(int x, int y);
};


inline
ImaBase::ImaBase( void )
{
  RawImage = 0; 
  Width = Height = 0;
  Depth = 0;
  ColorType = 0;
}

inline
Bool ImaBase::Inside(int x, int y)
{
  return (0<=y && y<Height && 0<=x && x<Width);
}

#endif

