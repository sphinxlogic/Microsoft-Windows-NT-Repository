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

int
__getgroups (int n, gid_t *gidset)
{
  error_t err;
  int ngids;

  __mutex_lock (&_hurd_id.lock);

  if (err = _hurd_check_ids ())
    {
      __mutex_unlock (&_hurd_id.lock);
      return __hurd_fail (err);
    }

  ngids = _hurd_id.gen.ngids;

  if (n != 0)
    {
      /* Copy the gids onto stack storage and then release the idlock.  */
      gid_t gids[ngids];
      memcpy (gids, _hurd_id.gen.gids, sizeof (gids));
      __mutex_unlock (&_hurd_id.lock);

      /* Now that the lock is released, we can safely copy the
	 group set into the user's array, which might fault.  */
      if (ngids > n)
	ngids = n;
      memcpy (gidset, gids, ngids * sizeof (gid_t));
    }
  else
    __mutex_unlock (&_hurd_idlock);

  return ngids;
}
