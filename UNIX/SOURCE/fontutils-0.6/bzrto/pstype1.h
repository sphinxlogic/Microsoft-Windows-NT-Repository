/* pstype1.h: declarations for translating the BZR file to a Type 1
   PostScript font.

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

#ifndef PSTYPE1_H
#define PSTYPE1_H

#include "bzr.h"
#include "char.h"
#include "input-ccc.h"
#include "tfm.h"


/* This is called first.  */
extern void pstype1_start_output (string output_name,
				  bzr_preamble_type, bzr_postamble_type);

/* Then these are called for characters needed as subroutines.  */
extern void pstype1_output_bzr_subr (bzr_char_type);
extern void pstype1_output_ccc_subr (ccc_type, charcode_type, char_type *[]);

/* This starts the subroutines.  */
extern void pstype1_start_subrs (unsigned subr_count);

/* And this finishes the subroutines and starts the characters.  */
extern void pstype1_start_chars (unsigned char_count);

/* Then we call one or the other of these.  */
extern void pstype1_output_bzr_char (bzr_char_type, char_type *[]);
extern void pstype1_output_ccc_char (ccc_type, charcode_type, char_type *[]);

/* And finally this is called at the end.  */
extern void pstype1_finish_output (void);

#endif /* not PSTYPE1_H */
