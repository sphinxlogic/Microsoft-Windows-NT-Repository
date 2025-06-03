/* input-cmi.h: declarations for top-level CMI reading.

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

#ifndef INPUT_CMI_H
#define INPUT_CMI_H

#include <stdio.h>
#include "types.h"


/* See `input-cmi.c'.  */
extern FILE *cmi_file;
extern string *cmi_names;


/* Read the CMI file NAME.  If NAME has no extension, supply `.DPIcmi'.  */
extern void read_cmi_file (string name, string dpi);

/* Call `read_cmi_file' on each name in NAME_LIST.  */
extern void read_cmi_file_list (string *name_list, string dpi);

#endif /* not INPUT_CMI_H */
