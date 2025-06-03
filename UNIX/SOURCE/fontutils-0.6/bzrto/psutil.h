/* psutil.h: utility routines for PostScript output (used for both Type 1 and
   Type 3 format).

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

#ifndef PSUTIL_H
#define PSUTIL_H

#include <stdio.h>
#include "tfm.h"
#include "types.h"


/* Global information about a PostScript font.  */
typedef struct
{
  string family_name;
  string font_name;
  real interword_space;
  int italic_angle; /* In degrees.  */
  boolean monospace_p;
  int underline_position, underline_thickness; /* In 1000 units/em.  */
  int unique_id;
  string version;
  string weight;
} ps_font_info_type;


/* Return the information that goes in the FontInfo dictionary, as well
   as a few other miscellanous things.  */
extern ps_font_info_type ps_set_font_info (void);

/* Output the starting boilerplate that is the same for Type 1 and Type
   3 fonts to the file F, using the other args for information.  */
extern void ps_start_font (FILE *f, ps_font_info_type ps_info,
                           string comment);

/* Output an encoding vector, using CHAR_OUTPUT_P to tell which
   characters exist in the font.  */
extern void ps_output_encoding (FILE *, boolean char_output_p[]);

/* Return the name corresponding to character code N in `encoding_info'.  */
extern string ps_encoding_name (charcode_type n);

/* Given the name NAME of a character, find its number in the current
   encoding vector.  If NAME is not defined, return -1.  */
extern int ps_encoding_number (string name);

/* User option; see psutil.c.  */
extern string ps_global_info;

#endif /* not PSUTIL_H */
