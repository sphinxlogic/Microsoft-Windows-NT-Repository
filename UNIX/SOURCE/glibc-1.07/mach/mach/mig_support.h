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

/* Useful declarations and support functions for MiG-generated stubs.  */

#ifndef	_MACH_MIG_SUPPORT_H

#define	_MACH_MIG_SUPPORT_H	1

#include <mach/std_types.h>
#include <mach/message.h>
#include <sys/types.h>
#include <string.h>

/* MiG initialization.  */
extern void __mig_init (void *__first);
extern void mig_init (void *__first);

/* Shorthand functions for vm_allocate and vm_deallocate on
   mach_task_self () (and with ANYWHERE=1).  */
extern void __mig_allocate (vm_address_t *__addr_p, vm_size_t __size);
extern void mig_allocate (vm_address_t *__addr_p, vm_size_t __size);
extern void __mig_deallocate (vm_address_t __addr, vm_size_t __size);
extern void mig_deallocate (vm_address_t __addr, vm_size_t __size);

/* Reply-port management support functions.  */
extern void __mig_dealloc_reply_port (void);
extern void mig_dealloc_reply_port (void);
extern mach_port_t __mig_get_reply_port (void);
extern mach_port_t mig_get_reply_port (void);
extern void __mig_reply_setup (const mach_msg_header_t *__request,
			       mach_msg_header_t *__reply);
extern void mig_reply_setup (const mach_msg_header_t *__request,
			     mach_msg_header_t *__reply);

/* Idiocy support function.  */
extern __inline vm_size_t
__mig_strncpy (char *__dst, const char *__src, vm_size_t __len)
{
  return __stpncpy (__dst, __src, __len) - __dst;
}
extern __inline vm_size_t 
mig_strncpy (char *__dst, const char *__src, vm_size_t __len)
{
  return __mig_strncpy (__dst, __src, __len);
}



#endif	/* mach/mig_support.h */
