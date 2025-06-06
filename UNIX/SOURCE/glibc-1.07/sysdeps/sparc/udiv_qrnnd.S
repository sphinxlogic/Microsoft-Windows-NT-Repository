! SPARC  __udiv_qrnnd division support, used from longlong.h.

! Copyright (C) 1993 Free Software Foundation, Inc.

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
! rem_ptr	i0
! n1		i1
! n0		i2
! d		i3

	.data
	.align	8
LC0:	.double	0r4294967296
LC1:	.double	0r2147483648
#include "sysdep.h"

ENTRY (__udiv_qrnnd)
	!#PROLOGUE# 0
	save	%sp,-104,%sp
	!#PROLOGUE# 1
	st	%i1,[%fp-8]
	ld	[%fp-8],%f10
	sethi	%hi(LC0),%o7
	fitod	%f10,%f4
	ldd	[%o7+%lo(LC0)],%f8
	cmp	%i1,0
	bge	L248
	mov	%i0,%i5
	faddd	%f4,%f8,%f4
L248:
	st	%i2,[%fp-8]
	ld	[%fp-8],%f10
	fmuld	%f4,%f8,%f6
	cmp	%i2,0
	bge	L249
	fitod	%f10,%f2
	faddd	%f2,%f8,%f2
L249:
	st	%i3,[%fp-8]
	faddd	%f6,%f2,%f2
	ld	[%fp-8],%f10
	cmp	%i3,0
	bge	L250
	fitod	%f10,%f4
	faddd	%f4,%f8,%f4
L250:
	fdivd	%f2,%f4,%f2
	sethi	%hi(LC1),%o7
	ldd	[%o7+%lo(LC1)],%f4
	fcmped	%f2,%f4
	nop
	fbge,a	L251
	fsubd	%f2,%f4,%f2
	fdtoi	%f2,%f2
	st	%f2,[%fp-8]
	b	L252
	ld	[%fp-8],%i4
L251:
	fdtoi	%f2,%f2
	st	%f2,[%fp-8]
	ld	[%fp-8],%i4
	sethi	%hi(-2147483648),%g2
	xor	%i4,%g2,%i4
L252:
	wr	%g0,%i4,%y
	sra	%i3,31,%g2
	and	%i4,%g2,%g2
	andcc	%g0,0,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,%i3,%g1
	mulscc	%g1,0,%g1
	add	%g1,%g2,%i0
	rd	%y,%g3
	subcc	%i2,%g3,%o7
	subxcc	%i1,%i0,%g0
	be	L253
	cmp	%o7,%i3

	add	%i4,-1,%i0
	add	%o7,%i3,%o7
	st	%o7,[%i5]
	ret
	restore
L253:
	blu	L246
	mov	%i4,%i0
	add	%i4,1,%i0
	sub	%o7,%i3,%o7
L246:
	st	%o7,[%i5]
	ret
	restore
