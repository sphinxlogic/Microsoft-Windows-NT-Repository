/* bb-outline.c: find the bounding boxes enclosing outlines.

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

#include "bb-outline.h"
#include "edge.h"


static bounding_box_type find_one_bb
  (bitmap_type, edge_type, unsigned, unsigned, bitmap_type *);

/* A character is made up of a list of one or more outlines.  Here, we
   go through a character's bitmap marking edges on pixels, top to
   bottom, left to right, looking for the next pixel with an unmarked
   edge also on the character's outline.  Each one of these we find is
   the starting place for one outline.  We get the bounding boxes of the
   outlines, and put them in a list to return.  We don't look at pixels
   that are at or after the column LEFT_MARK or before RIGHT_MARK.  If
   FIND_INNER is false, we return only outside outlines; otherwise, we
   return both inside and outside.  */

bb_list_type
find_outline_bbs (bitmap_type b, boolean find_inner,
                  int left_mark, int right_mark)
{
  unsigned row, col;
  bb_list_type bb_list = bb_list_init ();
  bitmap_type marked = new_bitmap (BITMAP_DIMENSIONS (b));

  /* By looking through the bitmap in column-major order, we preserve
     the original ordering of the characters in the image.  Otherwise,
     `b' would be found before `a' (for example), because of the
     ascender.  */
  for (col = 0; col < BITMAP_WIDTH (b); col++)
    if (col < left_mark || col >= right_mark)
      for (row = 0; row < BITMAP_HEIGHT (b); row++)
        {
          edge_type edge;

          if (BITMAP_PIXEL (b, row, col) == WHITE
              || BITMAP_INTERIOR_PIXEL (b, row, col))
            continue;

          edge = next_unmarked_outline_edge (row, col, START_EDGE, b, marked);
          
          if (edge != no_edge)
	    {
	      /* Want to mark edges in inner boxes, too.  */
	      bounding_box_type bb = find_one_bb (b, edge, row, col, &marked);

	      /* If edge != START_EDGE, it's on an inner bounding box.  */
	      if (find_inner || edge == START_EDGE)
                {
                  /* The numbers in BB as it comes in refer to pixels;
                     we have to change the maximum row so that it refers
                     to the edge beyond the last pixel.  */
                  MAX_COL (bb)++;

                  bb_list_append (&bb_list, bb);
                }
	    }
        }

  free_bitmap (&marked);
  return bb_list;
}

/* Here we find one of a character's outlines.  We're passed the
   position (ORIGINAL_ROW and ORIGINAL_COL) of a starting pixel and one
   of its unmarked edges, ORIGINAL_EDGE.  We traverse the adjacent edges
   of the outline pixels but keep the bounding box in pixel coordinates.
   We keep track of the marked edges in MARKED, which should be
   initialized to zeros before we get it.  */

static bounding_box_type
find_one_bb (bitmap_type b, edge_type original_edge,
             unsigned original_row, unsigned original_col,
             bitmap_type *marked)
{
  bounding_box_type bb = { INT_MAX, INT_MIN, INT_MAX, INT_MIN };
  unsigned row = original_row, col = original_col;
  edge_type edge = original_edge;

  do
    {
      coordinate_type p = { col, row };
      update_bounding_box (&bb, p);

      mark_edge (edge, row, col, marked);
      next_outline_edge (b, &edge, &row, &col);
    }
  while (!(row == original_row && col == original_col
           && edge == original_edge));

  return bb;
}
