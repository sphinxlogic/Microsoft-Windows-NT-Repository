/* list.h: simple list (represented as arrays) manipulation.

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

#ifndef LIST_H
#define LIST_H

#include "types.h"


typedef struct
{
  address *list;
  unsigned size;
} list_type;

/* The size of the list L.  */
#define LIST_SIZE(l) ((l).size)

/* The address of the array of data.  */
#define LIST_DATA(l) ((l).list)

/* Get the contents of the element at position INDEX in L.  */
#define LIST_ELT(l, index) LIST_DATA (l)[index]

/* Get the last list element.  */
#define LIST_LAST_ELT(l) LIST_ELT (l, LIST_SIZE (l) - 1)


/* Constructor/destructor.  */
extern list_type list_init (void);
extern void list_free (list_type *);

/* Returns a pointer to ELEMENT_SIZE bytes of memory allocated for the
   new element.  */
extern address list_append (list_type *, unsigned element_size);

/* An abbreviation for the usual case.  */
#define LIST_TAPPEND(l_ptr, type) list_append (l_ptr, sizeof (type))

#endif /* not LIST_H */
