/* tfm_ligature.c: deal with TFM ligature lists.

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


/* The character RIGHT may or may not be in LIG_LIST already.
   Update it if so, otherwise add it.  */

void
tfm_set_ligature (list_type *lig_list, charcode_type right, charcode_type l)
{
  unsigned this_right;
  tfm_ligature_type *new_ligature;
  
  for (this_right = 0; this_right < LIST_SIZE (*lig_list); this_right++)
    {
      tfm_ligature_type *ligature = LIST_ELT (*lig_list, this_right);

      if (ligature->character == right)
	{
	  ligature->ligature = l;
          return;
        }
    }

  /* RIGHT wasn't in the existing list.  Add it to the end.  */
  new_ligature = LIST_TAPPEND (lig_list, tfm_ligature_type);
  new_ligature->character = right;
  new_ligature->ligature = l;
}
