/* Copyright (C) 1992 Free Software Foundation, Inc.
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
#include <errno.h>
#include <sys/file.h>
#include <hurd.h>

/* Apply or remove an advisory lock, according to OPERATION,
   on the file FD refers to.  */
int
DEFUN(__flock, (fd, operation),
      int fd AND int operation)
{
  error_t err;

  if (err = _HURD_DPORT_USE (fd, __file_lock (port, operation)))
    return __hurd_dfail (fd, err);

  return 0;
}
