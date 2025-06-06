/* __mpn_cmp -- Compare two low-level natural-number integers.

Copyright (C) 1991, 1993 Free Software Foundation, Inc.


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

/* Compare OP1_PTR/OP1_SIZE with OP2_PTR/OP2_SIZE.
   There are no restrictions on the relative sizes of
   the two arguments.
   Return 1 if OP1 > OP2, 0 if they are equal, and -1 if OP1 < OP2.  */

int
#if __STDC__
__mpn_cmp (mp_srcptr op1_ptr, mp_srcptr op2_ptr, mp_size_t size)
#else
__mpn_cmp (op1_ptr, op2_ptr, size)
     mp_srcptr op1_ptr;
     mp_srcptr op2_ptr;
     mp_size_t size;
#endif
{
  mp_size_t i;
  mp_limb op1_word, op2_word;

  for (i = size - 1; i >= 0; i--)
    {
      op1_word = op1_ptr[i];
      op2_word = op2_ptr[i];
      if (op1_word != op2_word)
	goto diff;
    }
  return 0;
 diff:
  /* This can *not* be simplified to
	op2_word - op2_word
     since that expression might give signed overflow.  */
  return (op1_word > op2_word) ? 1 : -1;
}
