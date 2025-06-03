/* char.h: the definition of `char_type'.

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

#ifndef CHAR_H
#define CHAR_H

#include "font.h"
#include "list.h"
#include "realstrval.h"

/* We want these to mean the values in our own structure.  */
#undef CHAR_LSB
#undef CHAR_RSB


/* A side bearing can be defined almost like a symbol (no `char_type').  */
typedef struct
{
  symval_tag_type tag;
  real_string_val_type value;
} sidebearing_type;

#define SB_TAG(sb) ((sb).tag)
#define SB_VALUE(sb) ((sb).value)
#define SB_REAL(sb) ((sb).value.real_val)
#define SB_STRING(sb) ((sb).value.string_val)

/* The information about each character in which we're interested.  */
typedef struct
{
  char_info_type *char_info;
  tfm_char_type *tfm_info;
  sidebearing_type *lsb, *rsb;
  list_type kerns; /* We use `char_kern_type' for the elements.  */
} char_type;


/* The information from the bitmap font.  */
#define CHAR_BITMAP_INFO(c) ((c).char_info)
#define CHAR_CHARCODE(c) CHARCODE (*CHAR_BITMAP_INFO (c))

/* The information from the TFM file.  */
#define CHAR_TFM_INFO(c) ((c).tfm_info)

/* Information we've collected for the sidebearings.  */
#define CHAR_LSB(c) ((c).lsb)
#define CHAR_LSB_TAG(c) (CHAR_LSB (c)->tag)
#define CHAR_LSB_VALUE(c) (CHAR_LSB (c)->value)

#define CHAR_RSB(c) ((c).rsb)
#define CHAR_RSB_TAG(c) (CHAR_RSB (c)->tag)
#define CHAR_RSB_VALUE(c) (CHAR_RSB (c)->value)

#define CHAR_KERNS(c) ((c).kerns)

/* Initialize the various fields.  */
extern char_type init_char (void);

/* Get the font information for CODE, and update *C_PTR_ADDR from the
   symbol table and encoding vector.  */
extern void do_char (charcode_type code, char_type **c_ptr_addr);

/* Looks up `designsize' in the symbol table and converts the result
   back to printer's points.  */
extern real get_designsize_in_points (void);

#endif /* not CHAR_H */
