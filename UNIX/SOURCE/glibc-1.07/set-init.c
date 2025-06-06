/* Copyright (C) 1991, 1992 Free Software Foundation, Inc.
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
#include <stddef.h>

struct
  {
    size_t n;
    void EXFUN((*fn[0]), (int argc, char **argv, char **envp));
  } __libc_subinit;

void
DEFUN(__libc_init, (argc, argv, envp),
      int argc AND char **argv AND char **envp)
{
  
  void EXFUN((**fn), (int argc, char **argv, char **envp));

  for (fn = __libc_subinit.fn; *fn != NULL; ++fn)
    (**fn) (argc, argv, envp);
}
