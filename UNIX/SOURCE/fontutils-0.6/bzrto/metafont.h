/* metafont.h: declarations for the Metafont output.

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

#ifndef METAFONT_H
#define METAFONT_H

#include "bzr.h"
#include "char.h"
#include "input-ccc.h"
#include "tfm.h"


/* This is called first.  */
extern void metafont_start_output (string output_name, bzr_preamble_type, 
				   tfm_global_info_type *);

/* Then these are called for characters needed as subroutines.  */
extern void metafont_output_bzr_subr (bzr_char_type);
extern void metafont_output_ccc_subr (ccc_type, charcode_type, char_type *[]);

/* Then we call one or the other of these.  */
extern void metafont_output_bzr_char (bzr_char_type, char_type *[]);
extern void metafont_output_ccc_char (ccc_type, charcode_type, char_type *[]);

/* And finally this is called at the end.  */
extern void metafont_finish_output (list_type, char_type **);

#endif /* not METAFONT_H */
