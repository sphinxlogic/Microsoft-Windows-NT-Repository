/*
 * File:	ImaBase.h
 * Purpose:	Declaration of the Platform Independent Image Base Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 */
#if !defined(__ImaIter_h)
#define __ImaIter_h

class ImaIter
{
protected:
  int Itx, Ity;				// Counters
  int Stepx, Stepy;
  ImagePointerType IterImage;  		//  Image pointer
  ImaBase *ima;
public:
// Constructors
  ImaIter ( void );
  ImaIter ( ImaBase *imax );
  operator ImaBase* ();

// Iterators
  Bool ItOK ();
  void reset ();
  void upset ();
  void SetRow(byte *buf, int n);
  void GetRow(byte *buf, int n);
  byte GetByte( ) { return IterImage[Itx]; }
  void SetByte(byte b) { IterImage[Itx] = b; }
  ImagePointerType GetRow(void);
  Bool NextRow();
  Bool PrevRow();
  Bool NextByte();
  Bool PrevByte();

  void SetSteps(int x, int y=0) {  Stepx = x; Stepy = y; }
  void GetSteps(int *x, int *y) {  *x = Stepx; *y = Stepy; }
  Bool NextStep();
  Bool PrevStep();

////////////////////////// AD - for interlace ///////////////////////////////
  void SetY(int y);
/////////////////////////////////////////////////////////////////////////////
};


inline
ImaIter::ImaIter(void)
{
  ima = 0;
  IterImage = 0;
  Itx = Ity = 0;
  Stepx = Stepy = 0;
}

inline
ImaIter::ImaIter(ImaBase *imax): ima(imax)
{
  if (ima)
	 IterImage = ima->RawImage;
  Itx = Ity = 0;
  Stepx = Stepy = 0;
}

inline
ImaIter::operator ImaBase* ()
{
  return ima;
}

inline
Bool ImaIter::ItOK ()
{
  if (ima)
	 return ima->Inside(Itx, Ity);
  else
	 return FALSE;
}


inline void ImaIter::reset()
{
  IterImage = ima->RawImage;
  Itx = Ity = 0;
}

inline void ImaIter::upset()
{
  Itx = 0;
  Ity = ima->Height-1;
  IterImage = ima->RawImage + ima->EfeWidth*(ima->Height-1);
}

inline Bool ImaIter::NextRow()
{
  if (++Ity >= ima->Height) return 0;
  IterImage += ima->EfeWidth;
  return 1;
}

inline Bool ImaIter::PrevRow()
{
  if (--Ity < 0) return 0;
  IterImage -= ima->EfeWidth;
  return 1;
}

////////////////////////// AD - for interlace ///////////////////////////////
inline void ImaIter::SetY(int y)
{
  if ((y < 0) || (y > ima->Height)) return;
  Ity = y;
  IterImage = ima->RawImage + ima->EfeWidth*y;
}

/////////////////////////////////////////////////////////////////////////////

inline void ImaIter::SetRow(byte *buf, int n)
{
// Here should be bcopy or memcpy
  //_fmemcpy(IterImage, (void far *)buf, n);
  if (n<0)
	 n = ima->GetWidth();

  for (int i=0; i<n; i++) IterImage[i] = buf[i];
}

inline void ImaIter::GetRow(byte *buf, int n)
{
  for (int i=0; i<n; i++) buf[i] = IterImage[i];
}

inline ImagePointerType ImaIter::GetRow()
{
  return IterImage;
}

inline Bool ImaIter::NextByte()
{
  if (++Itx < ima->EfeWidth)
	 return 1;
  else
	 if (++Ity < ima->Height)
	 {
		IterImage += ima->EfeWidth;
		Itx = 0;
		return 1;
	 } else
		return 0;
}

inline Bool ImaIter::PrevByte()
{
  if (--Itx >= 0)
	 return 1;
  else
	 if (--Ity >= 0)
	 {
		IterImage -= ima->EfeWidth;
		Itx = 0;
		return 1;
	 } else
		return 0;
}

inline Bool ImaIter::NextStep()
{
  Itx += Stepx;
  if (Itx < ima->EfeWidth)
	 return 1;
  else {
	 Ity += Stepy;
	 if (Ity < ima->Height)
	 {
		IterImage += ima->EfeWidth;
		Itx = 0;
		return 1;
	 } else
		return 0;
  }
}

inline Bool ImaIter::PrevStep()
{
  Itx -= Stepx;
  if (Itx >= 0)
	 return 1;
  else {       
	 Ity -= Stepy;
	 if (Ity >= 0 && Ity < ima->Height)
	 {
		IterImage -= ima->EfeWidth;
		Itx = 0;
		return 1;
	 } else
		return 0;
  }
}

#endif
