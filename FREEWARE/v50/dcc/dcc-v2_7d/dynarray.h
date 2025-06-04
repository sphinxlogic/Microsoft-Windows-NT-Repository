/* DYNARRAY.H */

/* Only manages (for the time being) 1- and 2-dimensions arrays */
#ifndef _DYNARRAY_
#define _DYNARRAY_

#include <stdlib.h>

#define _DynArrayMagicNb_ 	0xA0F9

#define DynArray1(type)		type *

#define _DynArray1EltSiz_(array)	sizeof(*(array))
#define _DynArray1HdrNbSlots_	2
#define _DynArray1HdrSiz_(array) _DynArray1HdrSizSlot_(array) *		       \
							 _DynArray1HdrNbSlots_
#define _DynArray1HdrSizSlot_(array)					       \
  /* Beware, int size may not be a multiple of array element size */	       \
  ((_DynArray1EltSiz_(array) >= sizeof(int))				       \
    ? 1									       \
    : ((sizeof(int) + _DynArray1EltSiz_(array) - 1)/_DynArray1EltSiz_(array)))

#define AllocDynArray1(array, lim, pOk) do {				       \
  unsigned int __l__ = (lim);						       \
									       \
  FreeDynArray1(array);							       \
  if ((array)!=NULL							       \
      || ((__l__ + _DynArray1HdrSiz_(array))*_DynArray1EltSiz_(array))/	       \
		   _DynArray1EltSiz_(array) - _DynArray1HdrSiz_(array)!= __l__ \
      || ((array) = malloc((size_t)(__l__ + _DynArray1HdrSiz_(array))*	       \
			      _DynArray1EltSiz_(array)))==NULL) *(pOk) = 0!=0; \
  else {								       \
    *(unsigned int * /*~OddCast*/)(array) = _DynArrayMagicNb_;		       \
    *(unsigned int * /*~OddCast*/)((array) + _DynArray1HdrSizSlot_(array)) =   \
									__l__; \
    (array) += _DynArray1HdrSiz_(array);				       \
    *(pOk) = 0==0;}							       \
} while (0 != 0)

#define FreeDynArray1(array) do {					       \
  if ((array)!=NULL && *(unsigned int * /*~OddCast*/)((array) -		       \
			       _DynArray1HdrSiz_(array))==_DynArrayMagicNb_) { \
    free((array) - _DynArray1HdrSiz_(array));				       \
    (array) = NULL;}} while (0 != 0)
     
#define LimDynArray1(array, pi) do {					       \
  *(pi) = ((array)!=NULL && *(unsigned int * /*~OddCast */)((array) -	       \
				 _DynArray1HdrSiz_(array))==_DynArrayMagicNb_) \
           ? *(unsigned int * /*~OddCast*/)((array) -			       \
						 _DynArray1HdrSizSlot_(array)) \
           : 0;} while (0 != 0)


#define _DynArray2SizHdr_	3
#define DynArray2(type)		type *const *

#define AllocDynArray2(array, lim1, lim2, pOk) do {			       \
  unsigned int __l1__ = (lim1), __l2__ = (lim2);			       \
  register const void ** __ptrArray__;					       \
									       \
  FreeDynArray2(array);							       \
  if ((array)!=NULL							       \
      || ((__l1__ + _DynArray2SizHdr_)*sizeof(void *))/sizeof(void *)-	       \
						   _DynArray2SizHdr_!= __l1__  \
      || (__l2__*sizeof(**(array)))/sizeof(**(array))!= __l2__		       \
      || (__ptrArray__ = malloc((size_t)(__l1__ + _DynArray2SizHdr_)*sizeof(   \
					       void *)))==NULL) *(pOk) = 0!=0; \
  else {								       \
    register const void *const * __endArray__;				       \
									       \
    *(__ptrArray__)++ = (void * /*~OddCast*/)_DynArrayMagicNb_;		       \
    *(__ptrArray__)++ = (void * /*~OddCast*/)__l1__;			       \
    *(__ptrArray__)++ = (void * /*~OddCast*/)__l2__;			       \
    (array) = /*~VoidToOther*/ (void * /*~OddCast*/)__ptrArray__;	       \
    *(pOk) = 0==0;							       \
    __endArray__ = __ptrArray__ + __l1__;				       \
    while (__ptrArray__ != __endArray__) {				       \
      if ((*__ptrArray__++ = malloc((size_t)__l2__ * sizeof(**(array)))) ==    \
								      NULL) {  \
      __FreeDA2__(array)  *(pOk) = 0!=0; break;}}}			       \
} while (0 != 0)

#define FreeDynArray2(array) do {					       \
  if ((array)!=NULL && *(unsigned int * /*~OddCast*/)((array) -		       \
				      _DynArray2SizHdr_)==_DynArrayMagicNb_) { \
    register const void **__ptrArray__ = ((const void ** /*~OddCast*/)array) + \
	     *(unsigned int */*~OddCast*/)((array) - (_DynArray2SizHdr_ - 1)); \
    __FreeDA2__(array)}} while (0 != 0)

#define __FreeDA2__(array) {						       \
  while (__ptrArray__ != ((const void **/*~OddCast*/)array)) {		       \
    free((void */*~OddCast*/)*--__ptrArray__);}				       \
    free((void * /*~OddCast*/)((array) - _DynArray2SizHdr_));		       \
    (array) = NULL;}

#define LimDynArray2(array, pi, pj) do {				       \
  if ((array)!=NULL && *(unsigned int * /*~OddCast*/)((array) -		       \
				      _DynArray2SizHdr_)==_DynArrayMagicNb_) { \
    *(pi) = *(unsigned int * /*~OddCast*/)((array) - (_DynArray2SizHdr_ - 1)); \
    *(pj) = *(unsigned int * /*~OddCast*/)((array) - 1);}		       \
  else *(pi) = *(pj) = 0;} while (0 != 0)

#endif

/* End DYNARRAY.H */
