/* output-tfm.h: declarations for writing a TFM file.

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

#ifndef OUTPUT_TFM_H
#define OUTPUT_TFM_H

#include "types.h"
#include "font.h"


/* See tfm_output.c.  */
extern string tfm_header;
extern string fontdimens;


/* Called once at the beginning to initialize things.  */
extern void tfm_start_output (string input_name, string output_name,
                              real user_design_size, real default_design_size);

/* Called per each input font: the main one first and then each one
   specified with -concat.  */
extern void tfm_start_font (string font_name);

/* Called for every character in every font.  */
extern void tfm_output_char (char_info_type, real dpi);

/* Called at the very end.  */
extern void tfm_finish_output (void);

#endif /* not OUTPUT_TFM_H */
