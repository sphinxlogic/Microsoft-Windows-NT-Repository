/* list.c: simple list (represented as arrays) manipulation.

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


list_type
list_init ()
{
  list_type answer;

  LIST_DATA (answer) = NULL;
  LIST_SIZE (answer) = 0;

  return answer;
}


/* Free the memory for both the elements and the list itself.  */

void
list_free (list_type *list)
{
  if (list != NULL && LIST_DATA (*list) != NULL)
    {
      unsigned e;
      
      for (e = 0; e < LIST_SIZE (*list); e++)
        free (LIST_ELT (*list, e));
      
      free (LIST_DATA (*list));
    }
}

/* The list consists entirely of pointers to objects.  We allocate the
   space for the objects pointed to here, though, and return a pointer to
   the newly-created final element in the list.  */

address
list_append (list_type *list, unsigned element_size)
{
  LIST_SIZE (*list)++;
  XRETALLOC (LIST_DATA (*list), LIST_SIZE (*list), address);
  LIST_LAST_ELT (*list) = xmalloc (element_size);

  return LIST_LAST_ELT (*list);
}
