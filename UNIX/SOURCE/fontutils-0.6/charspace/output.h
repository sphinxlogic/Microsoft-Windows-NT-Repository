/* output.h: declarations for outputting the newly spaced font.

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

#ifndef OUTPUT_H
#define OUTPUT_H

#include "font.h"
#include "char.h"


/* See output.c.  */
extern string fontdimens;
extern string output_name;
extern charcode_type xheight_char;

/* Output a TFM and (perhaps) GF file with the new spacings for the
   characters CHARS.  Use the font B for the character bitmaps.  The
   font is written to the current directory.  */
extern void output_font (bitmap_font_type b, char_type *chars[]);

#endif /* not OUTPUT_H */
