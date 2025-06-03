/* bzr_opcodes.h: symbolic names for the BZR commands.  We have the
   luxury here of choosing any values for the opcodes, so we may as well
   be a little mnemonic about it.
   
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

#ifndef BZR_OPCODES_H
#define BZR_OPCODES_H

#define BOC		'b'
#define BOC_ABBREV	'B'
#define BZR_ID		'K'
#define CHAR_LOC	'c'
#define CHAR_LOC_ABBREV	'C'
#define EOC		'e'
#define LINE		'l'
#define LINE_ABBREV	'L'
#define NO_OP		'x'
#define POST		'P'
#define POST_POST	'Q'
#define SPLINE		's'
#define SPLINE_ABBREV	'S'
#define START_PATH	'p'

#endif /* not BZR_OPCODES_H */
