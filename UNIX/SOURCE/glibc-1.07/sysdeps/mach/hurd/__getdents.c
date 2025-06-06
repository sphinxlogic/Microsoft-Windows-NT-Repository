/* Copyright (C) 1992, 1993 Free Software Foundation, Inc.
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
#include <errno.h>
#include <sys/types.h>
#include <hurd.h>

ssize_t
DEFUN(__getdirentries, (fd, buf, nbytes, basep),
      int fd AND PTR buf AND size_t nbytes AND off_t *basep)
{
  error_t err;
  mach_msg_type_number_t amount;

  /* Fault before taking any locks.  */
  *(volatile off_t *) basep = *basep;

  amount = nbytes;
  err = _HURD_DPORT_USE (fd,
			 __dir_readdir (port, buf, &amount, *basep,
					basep, nbytes));
  if (err)
    return __hurd_dfail (fd, err);

  return amount;
}
