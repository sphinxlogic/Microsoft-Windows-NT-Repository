/* i80386 __mpn_mul_1 -- Multiply a limb vector with a limb and store
   the result in a second limb vector.

   Copyright (C) 1992, 1994 Free Software Foundation, Inc.


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

/*
   INPUT PARAMETERS
   res_ptr	(sp + 4)
   s1_ptr	(sp + 8)
   size		(sp + 12)
   s2_limb	(sp + 16)
*/

#include "asm.h"

#define res_ptr edi
#define s1_ptr esi
#define size ecx
#define s2_limb ebp

ENTRY (__mpn_mul_1)

	INSN1(push,l	,R(edi))
	INSN1(push,l	,R(esi))
	INSN1(push,l	,R(ebx))
	INSN1(push,l	,R(ebp))

	INSN2(mov,l	,R(res_ptr),MEM_DISP(esp,20))
	INSN2(mov,l	,R(s1_ptr),MEM_DISP(esp,24))
	INSN2(mov,l	,R(size),MEM_DISP(esp,28))
	INSN2(mov,l	,R(s2_limb),MEM_DISP(esp,32))

	INSN2(lea,l	,R(res_ptr),MEM_INDEX(res_ptr,size,4))
	INSN2(lea,l	,R(s1_ptr),MEM_INDEX(s1_ptr,size,4))
	INSN1(neg,l	,R(size))
	INSN2(xor,l	,R(ebx),R(ebx))
Loop:
	INSN2(mov,l	,R(eax),MEM_INDEX(s1_ptr,size,4))
	INSN1(mul,l	,R(s2_limb))
	INSN2(add,l	,R(eax),R(ebx))
	INSN2(mov,l	,MEM_INDEX(res_ptr,size,4),R(eax))
	INSN2(adc,l	,R(edx),$0)
	INSN2(mov,l	,R(ebx),R(edx))

	INSN1(inc,l	,R(size))
	INSN1(jnz,	,Loop)
	INSN2(mov,l	,R(eax),R(ebx))

	INSN1(pop,l	,R(ebp))
	INSN1(pop,l	,R(ebx))
	INSN1(pop,l	,R(esi))
	INSN1(pop,l	,R(edi))
	ret
