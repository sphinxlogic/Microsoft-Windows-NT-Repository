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
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <hurd.h>

/* Make the process sleep for SECONDS seconds, or until a signal arrives
   and is not ignored.  The function returns the number of seconds less
   than SECONDS which it actually slept (zero if it slept the full time).
   There is no return value to indicate error, but if `sleep' returns
   SECONDS, it probably didn't work.  */
unsigned int
DEFUN(sleep, (seconds), unsigned int seconds)
{
  time_t before, after;
  mach_port_t recv;

  recv = __mach_reply_port ();

  before = time ((time_t *) NULL);
  (void) __mach_msg (NULL, MACH_RCV_TIMEOUT|MACH_RCV_INTERRUPT,
		     0, 0, recv, seconds * 1000000, MACH_PORT_NULL);
  after = time ((time_t *) NULL);
  __mach_port_destroy (__mach_task_self (), recv);

  return (after - before);
}
