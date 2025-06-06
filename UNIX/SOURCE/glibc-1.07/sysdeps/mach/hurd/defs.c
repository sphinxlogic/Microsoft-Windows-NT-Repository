#include <sysdeps/stub/defs.c>
#if 0

/* Definitions of global stdio data structures.

Copyright (C) 1991, 1992, 1993 Free Software Foundation, Inc.
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

#include <ansidecl.h>
#include <stdio.h>
#include <hurd.h>
#include <gnu-stabs.h>
#include <unistd.h>

FILE *stdin, *stdout, *stderr;

/* Pointer to the first stream in the list.  */
FILE *__stdio_head = NULL;


static void
init_stdio (void)
{
  stdin = __fopenport (__getdport (STDIN_FILENO));
  stdout = __fopenport (__getdport (STDOUT_FILENO));
  stderr = __fopenport (__getdport (STDERR_FILENO));
}
text_set_element (__libc_subinit, init_stdio);

/* This function MUST be in this file!
   This is because we want _cleanup to go into the __libc_atexit set
   when any stdio code is used (and to use any stdio code, one must reference
   something defined in this file), and since only local symbols can be made
   set elements, having the set element stab entry here and _cleanup elsewhere
   loses; and having them both elsewhere loses because there is no reference
   to cause _cleanup to be linked in.  */

void
DEFUN_VOID(_cleanup)
{
  (void) fclose ((FILE *) NULL);
}
text_set_element (__libc_atexit, _cleanup);
#endif /* 0 */
