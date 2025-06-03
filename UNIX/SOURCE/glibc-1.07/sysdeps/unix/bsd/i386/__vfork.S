/* Copyright (C) 1993 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#include <sysdep.h>

/* Since vfork shares the address space between the parent and child,
   we must take care to avoid the child unwinding the stack frame and
   clobbering it for the parent.  */

.text
ENTRY (__vfork)
	popl %edx		/* Save the return PC in a register.  */
	DO_CALL (vfork, 0)	/* Do the system call.  */
	pushl %edx		/* Put the return PC back on the stack.  */
	jb syscall_error
	orl r1, r1
	je parent	/* Branch if parent.  */
	/* We are the child.  Return zero.  */
	xorl r0, r0
parent:
	ret
