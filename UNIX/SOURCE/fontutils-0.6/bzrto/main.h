/* main.h: global declarations.

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

#ifndef MAIN_H
#define MAIN_H

#include "encoding.h"
#include "tfm.h"

/* The encoding vector information for the input font.  */
extern encoding_info_type encoding_info;

/* Which characters to process.  */
extern charcode_type starting_char;
extern charcode_type ending_char;

/* The global information from the main TFM font, or NULL if no such
   font could be read.  */
extern tfm_global_info_type *tfm_global_info;

#endif /* not MAIN_H */
