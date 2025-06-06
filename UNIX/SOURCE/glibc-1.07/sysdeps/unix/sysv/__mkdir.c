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
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

/* Create a directory named PATH with protections MODE.  */
int
DEFUN(__mkdir, (path, mode), CONST char *path AND mode_t mode)
{
  char *cmd = __alloca (80 + strlen (path));
  char *p;
  int status;
  mode_t mask;
  int save;
  struct stat statbuf;

  if (path == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  /* Check for some errors.  */
  if (__stat (path, &statbuf) < 0)
    {
      if (errno != ENOENT)
	return -1;
      /* There is no file by that name.  Good.  */
    }
  else
    {
      errno = EEXIST;
      return -1;
    }

  /* Race condition, but how else to do it?  */
  mask = __umask (0777);
  (void) __umask (mask);

  p = cmd;
  *p++ = 'm';
  *p++ = 'k';
  *p++ = 'd';
  *p++ = 'i';
  *p++ = 'r';
  *p++ = ' ';

  mode &= ~mask;
  *p++ = '-';
  *p++ = 'm';
  *p++ = ' ';
  *p++ = ((mode & 07000) >> 9) + '0';
  *p++ = ((mode & 0700) >> 6) + '0';
  *p++ = ((mode & 070) >> 3) + '0';
  *p++ = ((mode & 07)) + '0';
  *p++ = ' ';

  strcpy (p, path);

  save = errno;
  /* If system doesn't set errno, but the mkdir fails, we really
     have no idea what went wrong.  EIO is the vaguest error I
     can think of, so I'll use that.  */
  errno = EIO;
  status = system (cmd);
  if (WIFEXITED (status) && WEXITSTATUS (status) == 0)
    {
      return 0;
      errno = save;
    }
  else
    return -1;
}
