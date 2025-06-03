/* tfm_kern.c: deal with TFM kern lists.

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

#include "list.h"
#include "tfm.h"


/* The character RIGHT may or may not be in the list of kerns already.  */

void
tfm_set_kern (list_type *kern_table, charcode_type right, real k)
{
  unsigned this_right;
  tfm_kern_type *new_kern;
  
  assert (kern_table != NULL);
  
  for (this_right = 0; this_right < LIST_SIZE (*kern_table); this_right++)
    {
      tfm_kern_type *kern = LIST_ELT (*kern_table, this_right);

      if (kern->character == right)
	{ /* Already there, just replace the value.  */
	  kern->kern = k;
	  return;
	}
    }

  /* RIGHT wasn't in the existing list.  Add it to the end.  */
  new_kern = LIST_TAPPEND (kern_table, tfm_kern_type);
  new_kern->character = right;
  new_kern->kern = k;
}

/* Find the kern between the characters LEFT and RIGHT.  (Return zero if
   none such.)  */ 

real
tfm_get_kern (tfm_char_type left, charcode_type right)
{
  list_type kern_table;
  unsigned r;
  
  if (!TFM_CHAR_EXISTS (left))
    return 0.0;
  
  kern_table = left.kern;

  for (r = 0; r < LIST_SIZE (kern_table); r++)
    {
      tfm_kern_type *kern = LIST_ELT (kern_table, r);

      if (kern->character == right)
	return kern->kern;
    }

  return 0.0;
}
