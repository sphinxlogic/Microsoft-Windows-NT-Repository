/* realstrval.h: types common to `char.h' and `symtab.h'.

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

#ifndef REALSTRVAL_H
#define REALSTRVAL_H

#include "types.h"

/* This structure is used both as part of `char_type' and as part of
   `symval_type', so we must define it separately.

   We use `symval_tag_type' to remember which parts of the struct count,
   although `symval_char' is irrelevant to a `real_string_val'.  If both
   the real and string parts are set, the value is taken to be the
   product.  */

typedef enum
{
  symval_real, symval_string, symval_real_string, symval_char,
  symval_char_width
} symval_tag_type;


typedef struct
{
  real real_val;
  string string_val;
} real_string_val_type;

#endif /* not REALSTRVAL_H */
