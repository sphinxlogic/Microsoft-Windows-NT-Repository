/* bb-list.h: bounding box lists.

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

#ifndef BB_LIST_H
#define BB_LIST_H

#include "bounding-box.h"


typedef struct
{
  bounding_box_type *data;
  unsigned length;
} bb_list_type;

/* The length of the list.  */
#define BB_LIST_LENGTH(bb_l) ((bb_l).length)

/* The array of elements as a whole.  */
#define BB_LIST_DATA(bb_l) ((bb_l).data)

/* The Nth element in the list.  */
#define BB_LIST_ELT(bb_l, n) BB_LIST_DATA (bb_l)[n]


/* Initialize a list.  */
extern bb_list_type bb_list_init (void);

/* Append BB to L.  */
extern void bb_list_append (bb_list_type *l, bounding_box_type bb);

/* Splice the elements in list B2 onto B1, changing B1.  */
extern void bb_list_splice (bb_list_type *B1, bb_list_type B2);

/* Free the memory in a list.  */
extern void bb_list_free (bb_list_type *);

#endif /* not BB_LIST_H */
