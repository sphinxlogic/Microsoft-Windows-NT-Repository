/* filter.c: run an averaging filter over a bitmap.
   This code is based on an initial implementation written by Richard Murphey.

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

#include "filter.h"


/* How many passes to make over the bitmap.  (-filter-passes)  */
unsigned filter_passes = 0;

/* How many rows and columns of neighbors to look at; i.e., a side of
   the filter cell is `filter_size' * 2 - 1.  Although only values > 0
   are meaningful, if we make it unsigned, comparisons to ints below
   fail.  (-filter-size)  */
int filter_size = 1;

/* The filter threshold at which we change the pixel.
   (-filter-threshold)  */
real filter_threshold = .5;


static void filter_once (bitmap_type);

/* Filter the bitmap B `filter_passes' times.  */

void
filter_bitmap (bitmap_type b)
{
  unsigned this_pass;
  
  for (this_pass = 0; this_pass < filter_passes; this_pass++)
    filter_once (b);
}


/* Filter the bitmap in B with an averaging filter.  See, for example,
   Digital Image Processing, by Rosenfeld and Kak.  The general idea is
   to average the intensity of the neighbors of each pixel.  If the
   difference between the pixel value and the intensity is more than
   `filter_threshold', flip the value of the pixel.  */

static void
filter_once (bitmap_type b)
{
  unsigned row, col;
  unsigned all_black = SQUARE (2 * filter_size + 1) - 1;
  unsigned t = filter_threshold * all_black; /* Rounded threshold.  */
  
  /* For each pixel in the bitmap...  */
  for (row = 0; row < BITMAP_HEIGHT (b); row++)
    {
      /* Don't look at pixels outside the bitmap.  */
      unsigned min_row = MAX ((int) row - filter_size, 0);
      unsigned max_row = MIN (row + filter_size, BITMAP_HEIGHT (b) - 1);

      for (col = 0; col < BITMAP_WIDTH (b); col++)
        {
          int cell_row, cell_col;
          unsigned sum = 0;

          unsigned min_col = MAX ((int) col - filter_size, 0);
          unsigned max_col = MIN (col + filter_size, BITMAP_WIDTH (b) - 1);

          /* For each pixel in the cell...  */
          for (cell_row = min_row; cell_row <= max_row; cell_row++)
            for (cell_col = min_col; cell_col <= max_col; cell_col++) 
              {
                sum += BITMAP_PIXEL (b, cell_row, cell_col);
              }

          /* Subtract the value for the pixel we're actually on.  I'm not
             sure why this is a good idea.  (It is much more efficient to
             do this than test for every pixel, though.)  */
          sum -= BITMAP_PIXEL (b, row, col);

          /* We've computed the sum of the neighbors.  Now change the
             pixel if the difference is greater than the threshold.  Doing
             integer arithmetic is not as precise as floating point (since
             the threshold is rounded), but it's much faster.  */
          if (abs (sum - (all_black * BITMAP_PIXEL (b, row, col))) > t)
            BITMAP_PIXEL (b, row, col) = ((double) sum) / all_black;
        }
    }
}
