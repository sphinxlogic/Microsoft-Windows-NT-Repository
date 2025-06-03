/* out-chars.h: extract the characters from the image.

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

#ifndef OUT_CHARS_H
#define OUT_CHARS_H

#include "image-header.h"


/* See out-chars.c.  */
extern int *baseline_list;
extern boolean print_clean_info, print_guidelines;
extern unsigned nchars_wanted;
extern int starting_char, ending_char;


extern void write_image_chars (image_header_type, real design_size);

#endif /* not OUT_CHARS_H */
