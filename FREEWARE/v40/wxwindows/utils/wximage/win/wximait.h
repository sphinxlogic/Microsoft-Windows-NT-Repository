#include "wbima.h"
#include "imaiter.h"

#if !defined(__wxImaIter_h)
#define __wxImaIter_h

class wxIterPixel: public ImaIter
{
public:
  wxIterPixel(wbIma *imax);
   
  int  GetIndex(void);
  Bool GetRGB(int* r, int* g, int* b);
  Bool SetIndex(int index);
  Bool SetRGB(int r, int g, int b);
}

inline
wxIterPixel::wxIterPixel(wbIma *imax): ImaIter(imax)
{
  if (ima)
	 SetSteps(ima->GetDepth(), 1);
}

#endif
