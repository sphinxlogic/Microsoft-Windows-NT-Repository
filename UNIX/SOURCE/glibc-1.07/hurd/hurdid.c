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

#include <hurd.h>
#include <gnu-stabs.h>

struct _hurd_id_data _hurd_id;

static void
init_id (void)
{
#ifdef noteven
  __mutex_init (&_hurd_id.lock);
#endif
  _hurd_id.valid = 0;
}

text_set_element (__libc_subinit, init_id);

/* Check that _hurd_id.{gen,aux} are valid and update them if not.
   Expects _hurd_id.lock to be held and does not release it.  */

error_t
_hurd_check_ids (void)
{
  if (! _hurd_id.valid)
    {
      error_t err;
      inline void dealloc (__typeof (_hurd_id.gen) *p)
	{
	  if (p->uids)
	    {
	      __vm_deallocate (__mach_task_self (),
			       (vm_address_t) p->uids,
			       p->nuids * sizeof (uid_t));
	      p->uids = NULL;
	    }
	  if (p->gids)
	    {
	      __vm_deallocate (__mach_task_self (),
			       (vm_address_t) p->gids,
			       p->ngids * sizeof (gid_t));
	      p->gids = NULL;
	    }
	}

      dealloc (&_hurd_id.gen);
      dealloc (&_hurd_id.aux);
      if (_hurd_id.rid_auth)
	{
	  __mach_port_deallocate (__mach_task_self (), _hurd_id.rid_auth);
	  _hurd_id.rid_auth = MACH_PORT_NULL;
	}

      if (err = __USEPORT (AUTH, __auth_getids
			   (port,
			    &_hurd_id.gen.uids, &_hurd_id.gen.nuids,
			    &_hurd_id.aux.uids, &_hurd_id.aux.nuids,
			    &_hurd_id.gen.gids, &_hurd_id.gen.ngids,
			    &_hurd_id.aux.gids, &_hurd_id.aux.ngids)))
	return err;

      _hurd_id.valid = 1;
    }

  return 0;
}
