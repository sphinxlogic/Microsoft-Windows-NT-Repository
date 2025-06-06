/* i80386 __mpn_rshift -- 

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


/* INPUT PARAMETERS
   res_ptr	(sp + 4)
   s_ptr	(sp + 8)
   size		(sp + 12)
   cnt		(sp + 16) */

#include "sysdep.h"

ENTRY (__mpn_rshift)
	pushl	%edi
	pushl	%esi
	pushl	%ebx

	movl	16(%esp),%edi	/* res_ptr */
	movl	20(%esp),%esi	/* s_ptr */
	movl	24(%esp),%edx	/* size */
	movl	28(%esp),%ecx	/* cnt */

	leal	-4(%edi,%edx,4),%edi
	leal	(%esi,%edx,4),%esi
	negl	%edx

	movl	(%esi,%edx,4),%ebx	/* read least significant limb */
	xorl	%eax,%eax
	shrdl	%cl,%ebx,%eax		/* compute carry limb */
	incl	%edx
	jz	Lend
	pushl	%eax			/* push carry limb onto stack */
	testb	$1,%edx
	jnz	L1			/* enter loop in the middle */
	movl	%ebx,%eax

	.align	2,0x90
Loop:	movl	(%esi,%edx,4),%ebx	/* load next higher limb */
	shrdl	%cl,%ebx,%eax		/* compute result limb */
	movl	%eax,(%edi,%edx,4)	/* store it */
	incl	%edx
L1:	movl	(%esi,%edx,4),%eax
	shrdl	%cl,%eax,%ebx
	movl	%ebx,(%edi,%edx,4)
	incl	%edx
	jnz	Loop

	shrl	%cl,%eax		/* compute most significant limb */
	movl	%eax,(%edi)		/* store it */

	popl	%eax			/* pop carry limb */

	popl	%ebx
	popl	%esi
	popl	%edi
	ret

Lend:	shrl	%cl,%ebx		/* compute most significant limb */
	movl	%ebx,(%edi)		/* store it */

	popl	%ebx
	popl	%esi
	popl	%edi
	ret
