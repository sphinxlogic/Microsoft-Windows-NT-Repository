/* __mpn_addmul_1 -- multiply the S1_SIZE long limb vector pointed to by S1_PTR
   by S2_LIMB, add the S1_SIZE least significant limbs of the product to the
   limb vector pointed to by RES_PTR.  Return the most significant limb of
   the product, adjusted for carry-out from the addition.

Copyright (C) 1992, 1993 Free Software Foundation, Inc.


! This file is part of the GNU C Library.  Its master source is NOT part of
! the C library, however.  This file is in fact copied from the GNU MP
! Library and its source lives there.

! The GNU C Library is free software; you can redistribute it and/or
! modify it under the terms of the GNU Library General Public License as
! published by the Free Software Foundation; either version 2 of the
! License, or (at your option) any later version.

! The GNU C Library is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
! Library General Public License for more details.

! You should have received a copy of the GNU Library General Public
! License along with the GNU C Library; see the file COPYING.LIB.  If
! not, write to the Free Software Foundation, Inc., 675 Mass Ave,
! Cambridge, MA 02139, USA.  */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

mp_limb
__mpn_addmul_1 (res_ptr, s1_ptr, s1_size, s2_limb)
     register mp_ptr res_ptr;
     register mp_srcptr s1_ptr;
     mp_size_t s1_size;
     register mp_limb s2_limb;
{
  register mp_limb cy_limb;
  register mp_size_t j;
  register mp_limb prod_high, prod_low;
  register mp_limb x;

  /* The loop counter and index J goes from -SIZE to -1.  This way
     the loop becomes faster.  */
  j = -s1_size;

  /* Offset the base pointers to compensate for the negative indices.  */
  res_ptr -= j;
  s1_ptr -= j;

  cy_limb = 0;
  do
    {
      umul_ppmm (prod_high, prod_low, s1_ptr[j], s2_limb);

      prod_low += cy_limb;
      cy_limb = (prod_low < cy_limb) + prod_high;

      x = res_ptr[j];
      prod_low = x + prod_low;
      cy_limb += (prod_low < x);
      res_ptr[j] = prod_low;
    }
  while (++j != 0);

  return cy_limb;
}
