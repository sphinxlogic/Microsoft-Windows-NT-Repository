/* tfm_util.c: routines independent of reading or writing a particular font.

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

#include "config.h"

#include "tfm.h"


/* A constructor for the TFM character type.  */

tfm_char_type
tfm_new_char ()
{
  tfm_char_type ch;

  ch.code = 0;
  ch.width = ch.height = ch.depth = ch.italic_correction = 0.0;
  ch.fix_width = ch.fix_height = ch.fix_depth = ch.fix_italic_correction = 0;

  ch.ligature = list_init ();
  ch.kern = list_init ();

  ch.exists = false;

  return ch;
}


/* Return an initialized array of `tfm_char_type's.  */

tfm_char_type *
tfm_new_chars ()
{
  unsigned i;
  tfm_char_type *chars = XTALLOC (TFM_SIZE, tfm_char_type);
  
  for (i = 0; i < TFM_SIZE; i++)
    chars[i] = tfm_new_char ();
  
  return chars;
}
