/* bitmap.c: operations on bitmaps.

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

#include "bitmap.h"
#include "bounding-box.h"

static void bb_ensure_bounds (bounding_box_type *, bitmap_type, string);

/* Make sure the bitmap is entirely white to begin with.  */

bitmap_type
new_bitmap (dimensions_type d)
{
  bitmap_type answer;
  unsigned size = DIMENSIONS_WIDTH (d) * DIMENSIONS_HEIGHT (d);

  BITMAP_DIMENSIONS (answer) = d;
  BITMAP_BITS (answer) = size > 0 ? xcalloc (size, 1) : NULL;

  return answer;
}


/* Free the storage that is allocated for a bitmap.  On the other hand,
   the bitmap might not have any storage allocated for it if it is zero
   in either dimension; in that case, don't free it.  */

void
free_bitmap (bitmap_type *b)
{
  if (BITMAP_BITS (*b) != NULL)
    safe_free ((address *) &BITMAP_BITS (*b));
}


/* Copy an existing bitmap into new memory.  We don't want to use
   `new_bitmap' here, since that routine clears the bitmap's bits, and
   we are going to set all the bits ourselves, anyway.  */

bitmap_type
copy_bitmap (bitmap_type bitmap)
{
  bitmap_type answer;
  unsigned size = BITMAP_WIDTH (bitmap) * BITMAP_HEIGHT (bitmap);

  BITMAP_DIMENSIONS (answer) = BITMAP_DIMENSIONS (bitmap);
  BITMAP_BITS (answer) = size > 0 ? xmalloc (size) : NULL;
  
  if (size > 0)
    memcpy (BITMAP_BITS (answer), BITMAP_BITS (bitmap), size);
    
  return answer;
}


/* Return the part of the bitmap SOURCE enclosed by the bounding box BB.
   BB is interpreted in bitmap coordinates, with y increasing downwards;
   for example, if MIN_ROW (BB) == 10, the subimage will start in the
   tenth row from the top of SOURCE.  */

bitmap_type
extract_subbitmap (bitmap_type source, bounding_box_type bb)
{
  unsigned this_row;
  bitmap_type sub = new_bitmap (bb_to_dimensions (bb));
  
  /* Move to the bit at which we want to start copying.  */
  BITMAP_BITS (source) += MIN_ROW (bb) * BITMAP_WIDTH (source) + MIN_COL (bb);
  
  bb_ensure_bounds (&bb, source, "extract_subbitmap");

  for (this_row = MIN_ROW (bb); this_row <= MAX_ROW (bb); this_row++)
    {
      one_byte *target = BITMAP_ROW (sub, this_row - MIN_ROW (bb));

      memcpy (target, BITMAP_BITS (source), BITMAP_WIDTH (sub));
      BITMAP_BITS (source) += BITMAP_WIDTH (source);
    }

  return sub;
}


/* If any of the elements of BB (taken to be in bitmap coordinates) are
   outside the bounds of the bitmap SOURCE, give a warning (using NAME)
   and update them.  */

static void
bb_ensure_bounds (bounding_box_type *bb, bitmap_type source, string name)
{
  if (MIN_COL (*bb) < 0)
    {
      WARNING2 ("%s: min col=%d outside source image", name, MIN_COL (*bb));
      MIN_COL (*bb) = 0;
    }

  if (MIN_ROW (*bb) < 0)
    {
      WARNING2 ("%s: min row=%d outside source image", name, MIN_ROW (*bb));
      MIN_COL (*bb) = 0;
    }

  /* See comments at `get_character_bitmap' in gf_input.c for why the
     width and height are treated asymetrically.  */
  if (MAX_COL (*bb) > BITMAP_WIDTH (source))
    {
      WARNING2 ("%s: max col=%d outside source image", name, MAX_COL (*bb));
      MAX_COL (*bb) = BITMAP_WIDTH (source) - 1;
    }

  if (MAX_ROW (*bb) >= BITMAP_HEIGHT (source))
    {
      WARNING2 ("%s: max row=%d outside source image", name, MAX_ROW (*bb));
      MAX_COL (*bb) = BITMAP_HEIGHT (source) - 1;
    }
}

/* The bounding boxes that we make in this routine are unlike the
   bounding boxes used elsewhere.  These are in bitmap coordinates, not
   Cartesian, and they refer to pixels, not edges.  So we have to adjust
   the maximum column by one.  */

const bounding_box_type
bitmap_to_bb (const bitmap_type b)
{
  bounding_box_type bb = dimensions_to_bb (BITMAP_DIMENSIONS (b));
  if (MAX_COL (bb) != 0) MAX_COL (bb)--;
  
  return bb;
}

/* Return the (zero-based) column numbers in which ROW changes from
   black to white or white to black.  The first element marks a
   white-to-black transition, and the last element marks a
   black-to-white transition, imagining that ROW is surrounded by white.
   (In other words, there is always an even number of transitions.)  We
   mark the end of the vector with an element WIDTH + 1.  */

/* We use `length - 2' because we need to save one element at the end
   for our sentinel.  */
#define INSERT_TRANSITION(e)						\
  do {									\
    XRETALLOC (vector, ++length, unsigned);				\
    vector[length - 2] = e;						\
  } while (0)

unsigned *
bitmap_find_transitions (const one_byte *row, unsigned width)
{
  unsigned col;
  unsigned start;
  one_byte color = BLACK;
  unsigned length = 1;
  
  /* We want to make sure we start at a column with some black.  */
  char *start_ptr = memchr (row, BLACK, width);
  
  /* Save the last element for the sentinel.  */
  unsigned *vector =  XTALLOC1 (unsigned);

  if (start_ptr == NULL)
    start = width; /* Don't go through the loop.  */
  else
    {
      INSERT_TRANSITION (start_ptr - (char *) row);
      start = vector[0] + 1; /* Don't look at this pixel again.  */
    }

  for (col = start; col < width - 1; col++)
    {
      if (row[col] != color)
        {
          INSERT_TRANSITION (col);
          color = row[col];
        }
    }
  
  /* We have to treat the last pixel in ROW specially.  There are
     several cases:
       1) if it's white, and the previous pixel is white, or the row is
          one pixel long, do nothing;
       2) if it's white, and the previous pixel is black,
          insert one element in `vector' (marking the end of a run);
       3) if it's black, and either the previous pixel is black or this was the
          first black pixel in the row, insert one element (marking the
          end of a run because we're at the end of the row);
       4) if it's black, and the previous pixel is white and there was a
          previous black pixel, or the row was one pixel long, insert
          two elements (marking the start and end of this one-pixel run).
  */
  if (row[width - 1] == WHITE)
    {
      if (width > 1 && row[width - 2] == BLACK)
        INSERT_TRANSITION (width - 1);
    }
  else
    { /* Last pixel is black.  If previous pixel was also black, just
         finish off the run.  */
      if (width > 1 && row[width - 2] == BLACK)
        INSERT_TRANSITION (width);
      else
        { /* Last pixel is black, previous pixel was white.  If this was
             the only black pixel in the row, we've already inserted the
             start of the run via `start' above, so just insert the end of
             the run.  Otherwise, insert both the start and end of this
             one-pixel run.  */
          if (start != width)
            INSERT_TRANSITION (width - 1);
          INSERT_TRANSITION (width);
        }
    }
  
  vector[length - 1] = width + 1;  /* Sentinel for the end of the vector.  */
  return vector;
}

/* Print a part of the bitmap in human-readable form.  */

void
print_bounded_bitmap (FILE *f, bitmap_type bitmap, bounding_box_type bb)
{
  unsigned this_row, this_col;

  fprintf (f, "Printing bitmap between (%u,%u) and (%u,%u):\n",
	   MIN_COL (bb), MIN_ROW (bb),
	   MAX_COL (bb), MAX_ROW (bb));
  for (this_row = MIN_ROW (bb); this_row <= MAX_ROW (bb); this_row++)
    {
      for (this_col = MIN_COL (bb); this_col <= MAX_COL (bb); this_col++)
        putc (BITMAP_PIXEL (bitmap, this_row, this_col) ? '*' : ' ', f);
        
      fprintf (f, "%u\n", this_row);
    }
}


void
print_bitmap (FILE *f, bitmap_type b)
{
  print_bounded_bitmap (f, b, bitmap_to_bb (b));
}
