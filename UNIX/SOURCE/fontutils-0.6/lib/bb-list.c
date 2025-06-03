/* bb-list.c: operations on bounding box lists.

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

#include "bb-list.h"


/* This routine returns an initialized empty list.  */

bb_list_type
bb_list_init ()
{
  bb_list_type bb_list;
  
  BB_LIST_LENGTH (bb_list) = 0;
  BB_LIST_DATA (bb_list) = NULL;
  
  return bb_list;
}

/* Append BB to BB_LIST.  */

void
bb_list_append (bb_list_type *bb_list, bounding_box_type bb)
{
  BB_LIST_LENGTH (*bb_list)++;
  XRETALLOC (BB_LIST_DATA (*bb_list), BB_LIST_LENGTH (*bb_list),
             bounding_box_type);

  BB_LIST_ELT (*bb_list, BB_LIST_LENGTH (*bb_list) - 1) = bb;
}

/* Append the elements in the list B2 onto the end of B1.  */

void
bb_list_splice (bb_list_type *b1, bb_list_type b2)
{
  unsigned new_length;
  unsigned this_bb;

  if (BB_LIST_LENGTH (b2) == 0)
    return;
    
  assert (b1 != NULL);
  
  new_length = BB_LIST_LENGTH (*b1) + BB_LIST_LENGTH (b2);
  XRETALLOC (BB_LIST_DATA (*b1), new_length, bounding_box_type);
  
  for (this_bb = 0; this_bb < BB_LIST_LENGTH (b2); this_bb++)
    BB_LIST_ELT (*b1, BB_LIST_LENGTH (*b1)++) = BB_LIST_ELT (b2, this_bb);
}

/* Free the memory in a list.  */

void
bb_list_free (bb_list_type *bb_list)
{
  if (BB_LIST_DATA (*bb_list) != NULL)
    safe_free ((address *) &BB_LIST_DATA (*bb_list));
}
