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
#include <sys/socket.h>
#include <hurd.h>

/* Open a connection on socket FD to peer at ADDR (which LEN bytes long).
   For connectionless socket types, just set the default address to send to
   and the only address from which to accept transmissions.
   Return 0 on success, -1 for errors.  */
int
DEFUN(connect, (fd, addr, len),
      int fd AND struct sockaddr *addr AND size_t len)
{
  return _HURD_DPORT_USE
    (fd,
     ({
       error_t err;
       addr_port_t aport;
       err = __socket_create_address (port, addr, len, &aport, 0);
       if (!err)
	 {
	   err = __socket_connect (port, aport);
	   __mach_port_deallocate (__mach_task_self (), aport);
	 }
       err ? __hurd_dfail (fd, err) : 0;
     }));
}
