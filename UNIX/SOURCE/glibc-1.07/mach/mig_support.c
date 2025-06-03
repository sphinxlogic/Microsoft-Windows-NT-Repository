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

#include <mach.h>

/* These functions are called by MiG-generated code.  */

static mach_port_t reply_port;

/* Called at startup with NOP==0.  */
void
__mig_init (int nop)
{
  if (!nop)
    reply_port = MACH_PORT_NULL;
}

/* Called by MiG to get a reply port.  */
mach_port_t
__mig_get_reply_port (void)
{
  if (reply_port == MACH_PORT_NULL)
    reply_port = __mach_reply_port ();

  return reply_port;
}

/* Called by MiG to deallocate the reply port.  */
void
__mig_dealloc_reply_port (void)
{
  mach_port_t port = reply_port;
  reply_port = MACH_PORT_NULL;	/* So the mod_refs RPC won't use it.  */
  __mach_port_mod_refs (__mach_task_self (), port,
			MACH_PORT_RIGHT_RECEIVE, -1);
}

/* Called by MiG to allocate space.  */
void
__mig_allocate (vm_address_t *addr,
		vm_size_t size)
{
  if (__vm_allocate (__mach_task_self (), addr, size, 1) != KERN_SUCCESS)
    *addr = 0;
}

/* Called by MiG to deallocate space.  */
void
__mig_deallocate (vm_address_t addr,
		  vm_size_t size)
{
  (void) __vm_deallocate (__mach_task_self (), addr, size);
}
