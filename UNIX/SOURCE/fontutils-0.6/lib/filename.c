/* filename.c: routines to manipulate filenames.

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

#include "filename.h"
#include "pathsrch.h"


/* Try to find the PK font FONT_NAME at resolution DPI, using various
   paths (see `filename.h').  The filename must have the resolution as
   part of the extension, e.g., `cmr10.300pk', for it to be found.  */
   
string
find_pk_filename (string font_name, unsigned dpi)
{
  static string *dirs = NULL;
  string pk_var, pk_name, name;
  char suffix[MAX_INT_LENGTH + sizeof ".pk"];

  sprintf (suffix, ".%dpk", dpi);

  pk_var = getenv ("PKFONTS") ? "PKFONTS"
           : getenv ("TEXPKS") ? "TEXPKS" : "TEXFONTS";

  if (dirs == NULL)
    dirs = initialize_path_list (pk_var, DEFAULT_PK_PATH);

  name = concat (font_name, suffix);
  pk_name = find_path_filename (name, dirs);

  if (name != pk_name)
    free (name);
  
  return pk_name;
}


/* Like `find_pk_filename', but search for a font in GF format.  */

string
find_gf_filename (string font_name, unsigned dpi)
{
  static string *dirs = NULL;
  string gf_var, gf_name, name;
  char suffix[MAX_INT_LENGTH + sizeof ".pk"];

  sprintf (suffix, ".%dgf", dpi);

  gf_var = getenv ("GFFONTS") ? "GFFONTS" : "TEXFONTS";
  
  if (dirs == NULL)
    dirs = initialize_path_list (gf_var, DEFAULT_GF_PATH);

  name = concat (font_name, suffix);
  gf_name = find_path_filename (name, dirs);

  if (name != gf_name)
    free (name);
    
  return gf_name;
}


/* Like `find_pk_filename', except search for a TFM file.  */

string
find_tfm_filename (string font_name)
{
  static string *dirs = NULL;
  string tfm_name, name;

  if (dirs == NULL)
    dirs = initialize_path_list ("TEXFONTS", DEFAULT_TFM_PATH);

  name = concat (font_name, ".tfm");
  tfm_name = find_path_filename (name, dirs);

  if (name != tfm_name)
    free (name);
    
  return tfm_name; 
}
