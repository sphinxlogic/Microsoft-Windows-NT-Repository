/* filename.h: declarations for manipulating filenames.

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

#ifndef FILENAME_H
#define FILENAME_H

#include "types.h"

/* This searches specifically for the PK font FONT_NAME at resolution
   DPI.  If FONT_NAME is absolute or explicitly relative, we simply
   return whether `FONT_NAME.DPIpk' is readable.  Otherwise, we use the
   environment variables PKFONTS, then TEXPKS, then TEXFONTS for the
   paths to search.  We return NULL if the font cannot be found.  */
extern string find_pk_filename (string font_name, unsigned dpi);

/* Like `find_pk_filename', except search for a GF font named
   `FONT_NAME.DPIgf'.  The environment variables used are GFFONTS then
   TEXFONTS.  We return NULL if the font cannot be found.  */
extern string find_gf_filename (string font_name, unsigned dpi);

/* Like `find_pk_filename', except search for a TFM file named
   `FONT_NAME.tfm'.  The environment variable used is TEXFONTS.  We
   return NULL if the font cannot be found.  */
extern string find_tfm_filename (string font_name);

#endif /* not FILENAME_H */
