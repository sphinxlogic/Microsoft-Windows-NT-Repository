! SPARC v8 __mpn_addmul_1 -- Multiply a limb vector with a limb and
! add the result to a second limb vector.

! Copyright (C) 1992, 1993, 1994 Free Software Foundation, Inc.

! This file is part of the GNU MP Library.

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


! INPUT PARAMETERS
! res_ptr	o0
! s1_ptr	o1
! size		o2
! s2_limb	o3

#include "sysdep.h"

ENTRY (__mpn_addmul_1)
	sub	%g0,%o2,%o2		! negate ...
	sll	%o2,2,%o2		! ... and scale size
	sub	%o1,%o2,%o1		! o1 is offset s1_ptr
	sub	%o0,%o2,%g1		! g1 is offset res_ptr

	mov	0,%o0			! clear cy_limb

Loop:	ld	[%o1+%o2],%o4
	ld	[%g1+%o2],%g2
	umul	%o4,%o3,%o5
	rd	%y,%g3
	addcc	%o5,%o0,%o5
	addx	%g3,0,%o0
	addcc	%o5,%g2,%g2
	addx	%o0,0,%o0
	st	%g2,[%g1+%o2]

	addcc	%o2,4,%o2
	bne	Loop
	 nop

	retl
	 nop
