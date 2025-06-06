/* mc68020 __mpn_mul_1 -- Multiply a limb vector with a limb and store
   the result in a second limb vector.

   Copyright (C) 1992 Free Software Foundation, Inc.


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

	TEXT
	ALIGN
	GLOBL	___mpn_mul_1

LAB(___mpn_mul_1)

#define res_ptr a0
#define s1_ptr a1
#define size d2
#define s2_limb d4

/* Save used registers on the stack.  */
	INSN2(movem,l	,MEM_PREDEC(sp),d2-d4)
#if 0
	INSN2(move,l	,MEM_PREDEC(sp),d2)
	INSN2(move,l	,MEM_PREDEC(sp),d3)
	INSN2(move,l	,MEM_PREDEC(sp),d4)
#endif

/* Copy the arguments to registers.  Better use movem?  */
	INSN2(move,l	,res_ptr,MEM_DISP(sp,16))
	INSN2(move,l	,s1_ptr,MEM_DISP(sp,20))
	INSN2(move,l	,size,MEM_DISP(sp,24))
	INSN2(move,l	,s2_limb,MEM_DISP(sp,28))

	INSN2(eor,w	,size,#1)
	INSN1(clr,l	,d1)
	INSN2(lsr,l	,size,#1)
	bcc	L1
	INSN2(subq,l	,size,#1)
	INSN2(sub,l	,d0,d0)		/* (d0,cy) <= (0,0) */

LAB(Loop)
	INSN2(move,l	,d3,MEM_POSTINC(s1_ptr))
	INSN2(mulu,l	,d1:d3,s2_limb)
	INSN2(addx,l	,d3,d0)
	INSN2(move,l	,MEM_POSTINC(res_ptr),d3)
LAB(L1)	INSN2(move,l	,d3,MEM_POSTINC(s1_ptr))
	INSN2(mulu,l	,d0:d3,s2_limb)
	INSN2(addx,l	,d3,d1)
	INSN2(move,l	,MEM_POSTINC(res_ptr),d3)

	dbf	size,Loop
	INSN1(clr,l	,d3)
	INSN2(addx,l	,d0,d3)
	INSN2(sub,l	,size,#0x10000)
	bcc	Loop

/* Restore used registers from stack frame.  */
	INSN2(movem,l	,d2-d4,MEM_POSTINC(sp))
#if 0
	INSN2(move,l	,d4,MEM_POSTINC(sp))
	INSN2(move,l	,d3,MEM_POSTINC(sp))
	INSN2(move,l	,d2,MEM_POSTINC(sp))
#endif
	rts
