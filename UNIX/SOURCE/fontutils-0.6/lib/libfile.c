/* libfile.c: open and read a single auxiliary data file.

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
#include "paths.h"

#include <ctype.h>
#include "libfile.h"
#include "line.h"
#include "pathsrch.h"


/* Private variables to hold the auxiliary file we're reading from, and
   its name.  */ 
static FILE *lib_file = NULL;
static string lib_filename;
static unsigned lib_file_line_number;


/* Prepare to read the ``library file'' FILENAME.DEFAULT_SUFFIX (if
   FILENAME has no suffix), or just FILENAME (if it has).  Give a fatal
   error if it cannot be opened.  Return the resulting FILE * (although
   usually this will just be ignored by the caller).  */

FILE *
libfile_start (string filename, string default_suffix)
{
  string *lib_dirs = initialize_path_list (LIB_ENVVAR, DEFAULT_LIB_PATH);
  string name = extend_filename (filename, default_suffix);

  lib_filename = find_path_filename (name, lib_dirs);

  if (lib_filename == NULL)
    FATAL1 ("%s: Cannot find library file in path", name);

  lib_file = xfopen (lib_filename, "r");
  lib_file_line_number = 1;
  
  return lib_file;
}


/* Close our current open file.  If we don't have any file open, give a
   fatal error.  */

void
libfile_close ()
{
  assert (lib_file != NULL);
  
  fclose (lib_file);
  lib_file = NULL;
}

/* Return the name of the currently open file, or NULL if none.  */

string
libfilename ()
{
  return lib_file == NULL ? NULL : lib_filename;
}


/* Analogously, for the current line number.  */

unsigned
libfile_linenumber ()
{
  return lib_file == NULL ? 0 : lib_file_line_number;
}

/* Return the next nonblank non-comment line from `lib_file', or NULL if
   we are at EOF.  Also remove any trailing comment on the line.  */

string
libfile_line ()
{
  string s;
  string line;
  boolean skip = true;
  
  assert (lib_file != NULL);

  do
    {
      line = read_line (lib_file);
      lib_file_line_number++;
      
      /* If at EOF, quit the loop.  */
      if (line == NULL)
        break;

      s = line;
      
      /* Move ahead to the first nonblank character.  */
      while (*s != 0 && isspace (*s))
        s++;

     /* Keep going if the line was blank or a comment.  */
      if (*s == 0 || *s == '%')
        free (line);
      else
        skip = false;
    }
  while (skip);
  
  if (line != NULL)
    {
      s = strrchr (line, '%');
      if (s != NULL)
        *s = 0;
    }

  return line;
}
