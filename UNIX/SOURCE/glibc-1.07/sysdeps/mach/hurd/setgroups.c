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
#include <sys/types.h>
#include <grp.h>
#include <hurd.h>

/* Set the group set for the current user to GROUPS (N of them).  */
int
DEFUN(setgroups, (n, groups), size_t n AND CONST gid_t *groups)
{
  error_t err;
  auth_t newauth;
  size_t i;
  gid_t new[n];

  /* Fault before taking locks.  */
  for (i = 0; i < n; ++i)
    new[i] = groups[i];

  __mutex_lock (&_hurd_id.lock);
  err = _hurd_check_ids ();
  if (! err)
    {
      /* Get a new auth port using those IDs.  */
      err = __USEPORT (AUTH,
		       __auth_makeauth (port, NULL, 0, 0,
					_hurd_id.gen.uids, _hurd_id.gen.nuids,
					_hurd_id.aux.uids, _hurd_id.aux.nuids,
					new, n,
					_hurd_id.aux.gids, _hurd_id.aux.ngids,
					&newauth));
    }
  __mutex_unlock (&_hurd_id.lock);

  if (err)
    return __hurd_fail (err);

  /* Install the new auth port and reauthenticate everything.  */
  err = __setauth (newauth);
  __mach_port_deallocate (__mach_task_self (), newauth);
  return err;
}
