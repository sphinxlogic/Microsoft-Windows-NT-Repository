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
#include <unistd.h>
#include <hurd.h>

int
__setreuid (uid_t ruid, uid_t euid)
{
  auth_t newauth;
  error_t err;

  __mutex_lock (&_hurd_idlock);
  err = _hurd_check_ids ();

  if (!err)
    {
      /* Make a new auth handle which has RUID as the real uid,
	 and EUID as the first element in the list of effective uids.  */

      size_t ngen = _hurd_id.gen.nuids < 1 ? 1 : _hurd_id.gen.nuids;
      size_t naux = _hurd_id.aux.nuids < 1 ? 1 : _hurd_id.aux.nuids;
      uid_t newaux[naux], newgen[ngen];

      newgen[0] = euid;
      memcpy (&newgen[1], _hurd_id.gen.uids, (ngen - 1) * sizeof (uid_t));
      newaux[0] = ruid;
      memcpy (&newaux[1], _hurd_id.aux.uids, (naux - 1) * sizeof (uid_t));

      err = __USEPORT (AUTH, __auth_makeauth
		       (port, NULL, 0, MACH_MSG_TYPE_COPY_SEND,
			newgen, ngen,
			newaux, naux,
			_hurd_id.gen.gids, _hurd_id.gen.ngids,
			_hurd_id.aux.gids, _hurd_id.aux.ngids,
			&newauth));
    }
  __mutex_unlock (&_hurd_id.lock);

  if (err)
    return __hurd_fail (err);

  /* Install the new handle and reauthenticate everything.  */
  err = __setauth (newauth);
  __mach_port_deallocate (__mach_task_self (), newauth);
  return err;
}
