/* main.h: global variable declarations.

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

#include <stdio.h>
#include "encoding.h"
#include "types.h"


/* See main.c.  */
extern FILE *cmi_file;
extern string dpi;
extern real dpi_real;
extern encoding_info_type encoding_info;
extern boolean have_tfm;
extern string input_name;
extern boolean no_gf;
extern charcode_type starting_char, ending_char;

/* This is defined in version.c.  */
extern string version_string;

#endif /* not MAIN_H */
