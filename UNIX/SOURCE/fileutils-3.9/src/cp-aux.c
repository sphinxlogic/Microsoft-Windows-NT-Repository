/* cp-aux.c  -- file copying (auxiliary routines)
   Copyright (C) 1989, 1990, 1991 Free Software Foundation.

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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Written by Torbjorn Granlund, Sweden (tege@sics.se). */

#ifdef HAVE_CONFIG_H
#if defined (CONFIG_BROKETS)
/* We use <config.h> instead of "config.h" so that a compilation
   using -I. -I$srcdir will use ./config.h rather than $srcdir/config.h
   (which it would do because it found this file in $srcdir).  */
#include <config.h>
#else
#include "config.h"
#endif
#endif

#include <stdio.h>

#include "cp.h"

extern char *program_name;

void
usage (status, reason)
     int status;
     char *reason;
{
  if (reason != NULL)
    fprintf (status == 0 ? stdout : stderr, "%s: %s\n",
	     program_name, reason);

  if (status != 0)
    fprintf (stderr, "Try `%s --help' for more information.\n",
	     program_name);
  else
    {
      printf ("\
Usage: %s [OPTION]... SOURCE DEST\n\
  or:  %s [OPTION]... SOURCE... DIRECTORY\n\
",
	      program_name, program_name);
      printf ("\
\n\
  -a, --archive                same as -dpr\n\
  -b, --backup                 make backup before removal\n\
  -d, --no-dereference         preserve links\n\
  -f, --force                  remove existing destinations, never prompt\n\
  -i, --interactive            prompt before overwrite\n\
  -l, --link                   link files instead of copying\n\
  -p, --preserve               preserve file attributes if possible\n\
  -r                           copy recursively, non-directories as files\n\
  -s, --symbolic-link          make symbolic links instead of copying\n\
  -u, --update                 copy only older or brand new files\n\
  -v, --verbose                explain what is being done\n\
  -x, --one-file-system        stay on this file system\n\
  -P, --parents                append source path to DIRECTORY\n\
  -R, --recursive              copy directories recursively\n\
  -S, --suffix SUFFIX          override the usual backup suffix\n\
  -V, --version-control WORD   override the usual version control\n\
      --help                   display this help and exit\n\
      --version                output version information and exit\n\
\n\
The backup suffix is ~, unless set with SIMPLE_BACKUP_SUFFIX.  The\n\
version control may be set with VERSION_CONTROL, values are:\n\
\n\
  t, numbered     make numbered backups\n\
  nil, existing   numbered if numbered backups exist, simple otherwise\n\
  never, simple   always make simple backups  \n");
    }
  exit (status);
}

int
is_ancestor (sb, ancestors)
     struct stat *sb;
     struct dir_list *ancestors;
{
  while (ancestors != 0)
    {
      if (ancestors->ino == sb->st_ino && ancestors->dev == sb->st_dev)
	return 1;
      ancestors = ancestors->parent;
    }
  return 0;
}
