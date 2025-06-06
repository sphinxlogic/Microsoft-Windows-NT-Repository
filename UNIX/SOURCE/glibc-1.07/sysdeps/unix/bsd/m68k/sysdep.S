/* Copyright (C) 1991, 1992, 1993 Free Software Foundation, Inc.
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

#define _ERRNO_H
#include <errnos.h>

.globl syscall_error
syscall_error:
	/* We translate the system's EWOULDBLOCK error into EAGAIN.
	   The GNU C library always defines EWOULDBLOCK==EAGAIN.
	   EWOULDBLOCK_sys is the original number.  */
#ifdef __motorola__
#ifdef EWOULDBLOCK_sys
	cmp.l d0, #EWOULDBLOCK_sys
	bne store
	moveq.l #EAGAIN, d0
#endif
store:	move.l d0, _errno
	moveq.l #-1, d0
#else
#ifdef EWOULDBLOCK_sys
	cmpl d0, #EWOULDBLOCK_sys
	bne 0f
	moveq #EAGAIN, d0
#endif
0:	movel d0, _errno
	moveq #-1, d0
#endif
	rts
