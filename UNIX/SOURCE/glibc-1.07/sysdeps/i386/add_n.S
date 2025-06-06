/* i80386 __mpn_add_n -- Add two limb vectors of the same length > 0 and store
sum in a third limb vector.

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
   s1_ptr	(sp + 8)
   s2_ptr	(sp + 12)
   size		(sp + 16) */

#include "sysdep.h"

ENTRY (__mpn_add_n)
	pushl %edi
	pushl %esi

	movl 12(%esp),%edi	/* res_ptr */
	movl 16(%esp),%esi	/* s1_ptr */
	movl 20(%esp),%edx	/* s2_ptr */
	movl 24(%esp),%ecx	/* size */

	movl	%ecx,%eax
	shrl	$3,%ecx			/* compute loop count for unrolled loop */
	negl	%eax
	andl	$7,%eax			/* get index where to start loop */
	jz	Loop			/* necessary special case for 0 */
	incl	%ecx			/* adjust loop count for `loop' insn */
	shll	$2,%eax			/* adjustment for pointers... */
	subl	%eax,%edi		/* ... since they are offset ... */
	subl	%eax,%esi		/* ... by a constant when we enter ... */
	subl	%eax,%edx		/* ... the loop */
	shrl	$2,%eax			/* restore previous value */
	leal	(Loop - 3)(%eax,%eax,8),%eax	/* calc start addr in loop */
	jmp	*%eax			/* jump into loop */
	.align	2
Loop:	movl	(%esi),%eax
	adcl	(%edx),%eax
	movl	%eax,(%edi)
	movl	4(%esi),%eax
	adcl	4(%edx),%eax
	movl	%eax,4(%edi)
	movl	8(%esi),%eax
	adcl	8(%edx),%eax
	movl	%eax,8(%edi)
	movl	12(%esi),%eax
	adcl	12(%edx),%eax
	movl	%eax,12(%edi)
	movl	16(%esi),%eax
	adcl	16(%edx),%eax
	movl	%eax,16(%edi)
	movl	20(%esi),%eax
	adcl	20(%edx),%eax
	movl	%eax,20(%edi)
	movl	24(%esi),%eax
	adcl	24(%edx),%eax
	movl	%eax,24(%edi)
	movl	28(%esi),%eax
	adcl	28(%edx),%eax
	movl	%eax,28(%edi)
	leal	32(%edi),%edi
	leal	32(%esi),%esi
	leal	32(%edx),%edx
#ifdef GAS_SYNTAX
/* It seems that gas 2.2 can't handle loop instructions *at all*. Fudge 
 * like this: (Greg Lehey, LEMIS, 30 January 1994). */
	.byte	0xe2, Loop - next
next:
#else					/* AT&T syntax seems to handle this somehow */
	loop	Loop
#endif
	sbbl	%eax,%eax
	negl	%eax

	popl %esi
	popl %edi
	ret
