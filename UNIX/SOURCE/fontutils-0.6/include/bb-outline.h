/* bb-outline.h: find a list of bounding boxes enclosing outlines.

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

#ifndef BB_OUTLINE_H
#define BB_OUTLINE_H

#include "bb-list.h"
#include "bitmap.h"
#include "types.h"


/* Find the bounding boxes around the outlines in the bitmap B.  If ALL is
   true, we find the bounding boxes around all the outlines, including
   counterforms.  If ALL is false, we find only ``outside'' outlines. 
   The character `a', for example, would be represented by one bounding
   box.  We don't look in any of the columns from LEFT to RIGHT,
   left-inclusive.  */
extern bb_list_type
  find_outline_bbs (bitmap_type b, boolean all, int left, int right);

#endif /* not BB_OUTLINE_H */
