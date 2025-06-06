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

#include <ansidecl.h>
#include <errno.h>
#include <unistd.h>
#include <hurd.h>

/* Get the real user ID of the calling process.  */
uid_t
DEFUN_VOID(__getuid)
{
  error_t err;
  uid_t uid;

  __mutex_lock (&_hurd_id.lock);

  if (err = _hurd_check_ids ())
    {
      errno = err;
      uid = -1;
    }
  else if (_hurd_id.aux.nuids >= 1)
    uid = _hurd_id.aux.uids[0];
  else
    {
      /* We do not even have a real uid.  */
      errno = EGRATUITOUS;
      uid = -1;
    }

  __mutex_unlock (&_hurd_id.lock);
  return uid;
}
