/* input-ccc.h: call Bison-generated code to parse a CCC file.  This
   file must be included before the Bison-generated `ccc.h'.

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

#ifndef INPUT_CCC_H
#define INPUT_CCC_H

#include "list.h"
#include "tfm.h"


/* The representation of a CCC dimension.  */
typedef struct
{
  real amount;
  boolean absolute;
  boolean valid;
} dimen_type;

/* The distance, in points.  */
#define DIMEN_AMOUNT(d) ((d).amount)

/* Says whether the dimension is font- or character-dependent.  */
#define DIMEN_ABSOLUTE(d) ((d).absolute)

/* If this is not set, the other members are garbage.  */
#define DIMEN_VALID(d) ((d).valid)

/* The representation of a CCC operation.  */
typedef struct
{
  int opcode;
  boolean valid;
  union
  {
    charcode_type charcode;
    dimen_type dimen;
  } u;
} ccc_cmd_type;

#define CCC_OPCODE(ccc) ((ccc).opcode)
#define CCC_VALID(ccc) ((ccc).valid)
#define CCC_CHARCODE(ccc) ((ccc).u.charcode)
#define CCC_DIMEN(ccc) ((ccc).u.dimen)
#define CCC_DIMEN_AMOUNT(ccc) DIMEN_AMOUNT ((ccc).u.dimen)
#define CCC_DIMEN_ABSOLUTE(ccc) DIMEN_ABSOLUTE ((ccc).u.dimen)

/* Use the generic operations in `list.h' to manipulate CCC command lists.  */
typedef list_type ccc_cmd_list_type;

/* A CCC file is a list of commands.  */
typedef ccc_cmd_list_type ccc_type;

/* This type is returned for character names.  It is only needed within
   the parser, but we have to define it here since it is part of the
   %union, and hence winds up in the Bison-generated include file.  */

typedef struct
{
  charcode_type code;
  string name;
} charname_type;

/* The font information the user gets to access in the CCC source.  All
   values are in points.  */

typedef struct
{
  int cap_height;
  int design_size;
  int em;
  int font_depth;
  int xheight;
} ccc_fontinfo_type;


/* For communication with the parser.  */
extern FILE *ccc_file;
extern string ccc_filename;
extern bzr_char_type **chars;
extern ccc_fontinfo_type ccc_fontinfo;
extern ccc_type *ccc_chars[];


/* Read the CCC file NAME, extended with `.ccc' if necessary.  Warn
   about the file not existing only if WARN_P is true.  Update CHARS,
   taking defaults from T.  */

extern ccc_type **parse_ccc_file
  (string name, bzr_char_type **chars, tfm_global_info_type *t,
   real design_size, boolean warn_p);

#endif /* not CCC_H */
