/* xgetcwd.c: a from-scratch version of getwd.  Based on the tcsh 5.20
   source, apparently uncopyrighted.

Copyright (C) 1992 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "config.h"
#include "c-pathmx.h"

#include "dirio.h"
#include "xstat.h"

/* Return the pathname of the current directory, or give a fatal error.  */

string
xgetcwd ()
{
  /* If the system provides getwd, use it.  But don't use getcwd;
     forking a process is even more expensive than all the stat calls we
     make figuring out the cwd.  */
#ifdef GETWD_MISSING /* whole function */
  struct stat root_stat, cwd_stat;
  string cwd_path = xmalloc (1);
  
  *cwd_path = 0;
  
  /* Find the inodes of the root and current directories.  */
  root_stat = xstat ("/");
  cwd_stat = xstat (".");

  /* Go up the directory hierarchy until we get to root, prepending each
     directory we pass through to `cwd_path'.  */
  while (!SAME_FILE_P (root_stat, cwd_stat))
    {
      struct dirent *e;
      DIR *parent_dir;
      boolean found = false;
      
      xchdir ("..");
      parent_dir = xopendir (".");

      /* Look through the parent directory for the entry with the same
         inode, so we can get its name.  */
      while ((e = readdir (parent_dir)) != NULL && !found)
        {
          struct stat test_stat = xlstat (e->d_name);
          
          if (SAME_FILE_P (test_stat, cwd_stat))
            {
              /* We've found it.  Prepend the pathname.  */
              string temp = cwd_path;
              cwd_path = concat3 ("/", e->d_name, cwd_path);
              free (temp);
              
              /* Set up to test the next parent.  */
              cwd_stat = xstat (".");
              
              /* Stop reading this directory.  */
              found = true;
            }
        }
      if (!found)
        FATAL2 ("No inode %d/device %d in parent directory",
                cwd_stat.st_ino, cwd_stat.st_dev);
      
      xclosedir (parent_dir);
    }
  
  /* If the current directory is the root, cwd_path will be the empty
     string, and we will have not gone through the loop.  */
  if (*cwd_path == 0)
    cwd_path = "/";
  else
    /* Go back to where we were.  */
    xchdir (cwd_path);

  return cwd_path;

#else /* not GETWD_MISSING */

  string path = xmalloc (PATH_MAX + 1);
  
  if (getwd (path) == 0)
    {
      fprintf (stderr, "getwd: %s", path);
      exit (1);
    }
  
  return path;
#endif /* not GETWD_MISSING */
}
