/* text.h: declarations for translating the BZR font to human-readable text.

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

#ifndef TEXT_H
#define TEXT_H

#include "bzr.h"
#include "input-ccc.h"


extern void text_start_output (string font_name, bzr_preamble_type);
extern void text_output_bzr_char (bzr_char_type);
extern void text_output_ccc_char (ccc_type);
extern void text_finish_output (bzr_postamble_type);

#endif /* not TEXT_H */
