/* char.h: declarations for handling TFM and BZR characters simultaneously.

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

#ifndef CHAR_H
#define CHAR_H

#include "bzr.h"
#include "input-ccc.h"

typedef enum { bzr_char_class, ccc_char_class } char_class_type;

typedef struct
{
  char_class_type tag;
  union
  {
    bzr_char_type *bzr_char;
    ccc_type *ccc_char;
  } u;
} char_type;

#define CHAR_CLASS(c) ((c).tag)
#define CHAR_BZR(c) ((c).u.bzr_char)
#define CHAR_CCC(c) ((c).u.ccc_char)


/* See char.c.  */
extern real oblique_angle;

/* Slant all splines in the list by `oblique_angle'.  */
extern spline_list_array_type oblique_splines (spline_list_array_type);


/* Return an array of which characters are needed by CCC_CHARS (between
   START and END) as subroutines, and how many we found in SUBR_COUNT.  */
extern char_type **subr_chars (bzr_char_type *[], ccc_type *ccc_chars[],
                               charcode_type start, charcode_type end,
                               unsigned *subr_count);

/* Scale a character by SCALE_FACTOR.  */
extern void scale_char (bzr_char_type *, real scale_factor);

#endif /* not CHAR_H */
