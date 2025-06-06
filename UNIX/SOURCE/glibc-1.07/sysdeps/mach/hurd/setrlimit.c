/* Copyright (C) 1991, 1992, 1993 Free Software Foundation, Inc.
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
#include <sys/resource.h>
#include <hurd.h>
#include <errno.h>


/* Set the soft and hard limits for RESOURCE to *RLIMITS.
   Only the super-user can increase hard limits.
   Return 0 if successful, -1 if not (and sets errno).  */
int
DEFUN(setrlimit, (resource, rlimits),
      enum __rlimit_resource resource AND struct rlimit *rlimits)
{
  struct rlimit lim;

  if (rlimits == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  lim = *rlimits;		/* Fault now if ever.  */

  if (lim.rlim_max != RLIM_INFINITY)
    {
      /* We have no enforceable resource limits.  */
      errno = ENOSYS;
      return -1;
    }

  switch (resource)
    {
    default:
      errno = EINVAL;
      return -1;

    case RLIMIT_CORE:
      _hurd_core_limit = lim.rlim_cur;
      return 0;

    case RLIMIT_DATA:
      return _hurd_set_data_limit (&lim);

    case RLIMIT_CPU:
    case RLIMIT_FSIZE:
    case RLIMIT_STACK:
    case RLIMIT_RSS:
    case RLIMIT_NPROC:
    case RLIMIT_MEMLOCK:
      errno = ENOSYS;
      return -1;

    case RLIMIT_OFILE:
      __mutex_lock (&_hurd_dtable_lock);
      _hurd_dtable_rlimit = lim.rlim_cur; /* XXX open descriptors? */
      __mutex_unlock (&_hurd_dtable_lock);
      return 0;
    }
}
