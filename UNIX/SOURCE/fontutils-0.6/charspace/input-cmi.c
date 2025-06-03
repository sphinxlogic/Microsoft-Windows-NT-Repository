/* input-cmi.c: routines to control the CMI parsing.

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

#include "libfile.h"

#include "input-cmi.h"


/* Font-specific sidebearing and other information.  (-cmi-files)
   `cmi_file' is used as the input file in `cmi.y'.  */
FILE *cmi_file;
string *cmi_names = NULL;

/* Call `read_cmi_file' on each CMI file in CMI_NAMES.  DPI is
   simply passed along.  */

void
read_cmi_file_list (string *cmi_names, string dpi)
{
  if (cmi_names == NULL)
    return;

  for ( ; *cmi_names != NULL; cmi_names++)
    {
      read_cmi_file (*cmi_names, dpi);
    }
}


/* Read the CMI file `NAME.DPIcmi', unless NAME has a suffix, in which
   case don't append the `.DPIcmi'.  This uses the global `cmi_file' to
   communicate with the Bison parser.  */

void
read_cmi_file (string name, string dpi)
{
  extern int yyparse ();

  cmi_file = libfile_start (name, concat (dpi, "cmi"));
  yyparse ();

  libfile_close ();
}
