/* out-strips.c: cut the entire image into strips.

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

#include "gf.h"
#include "report.h"

#include "main.h"
#include "out-strips.h"

/* Output a ``font'' in which each ``character'' is a constant number of
   scanlines from the image.  This might use less than resources than
   the giant PostScript file that -epsf outputs.
   
   We only output the GF characters here; we assume the postamble and
   preamble are written by the caller.  */

void
write_chars_as_strips (image_header_type image_header, real design_size)
{
  dimensions_type char_dimens;
  gf_char_type gf_char;
  unsigned gf_row;
  unsigned lines_per_char;
  one_byte *scanline;
  real width_in_points;
  boolean first_char = true;
  unsigned scanline_count = 0;

  /* Set up for the first character.  We divide the image into 256 parts,
     each of which will turn into one ``character''.  */
  lines_per_char = image_header.height / 256 + 1;
  GF_CHARCODE (gf_char) = 0;
  DIMENSIONS_WIDTH (char_dimens) = image_header.width;
  DIMENSIONS_HEIGHT (char_dimens) = lines_per_char;
  GF_BITMAP (gf_char) = new_bitmap (char_dimens);
  GF_CHAR_MIN_COL (gf_char) = GF_CHAR_MIN_ROW (gf_char) = 0;
  GF_CHAR_MAX_COL (gf_char) = DIMENSIONS_WIDTH (char_dimens);
  GF_CHAR_MAX_ROW (gf_char) = DIMENSIONS_HEIGHT (char_dimens) - 1;
  
  /* We aren't going to have any side bearings.  */
  GF_H_ESCAPEMENT (gf_char) = DIMENSIONS_WIDTH (char_dimens);
  width_in_points = DIMENSIONS_WIDTH (char_dimens) * POINTS_PER_INCH
                    / (real) image_header.hres;
  GF_TFM_WIDTH (gf_char) = real_to_fix (width_in_points / design_size);
  
  
  /* Read the image.  */
  while (true)
    {
      if (scanline_count % lines_per_char == 0)
        {
          /* We get here when scanline_count == 0, and we haven't read
             anything, so we can't write anything.  */
          if (!first_char)
            {
              gf_put_char (gf_char);
              REPORT2 ("[%u]%c", GF_CHARCODE (gf_char),
                                (GF_CHARCODE (gf_char) + 1) % 13 ? ' ' : '\n');
              GF_CHARCODE (gf_char)++;
            }
          else
            first_char = false;
          gf_row = 0;
        }

      scanline = BITMAP_BITS (GF_BITMAP (gf_char))
                 + gf_row * DIMENSIONS_WIDTH (char_dimens); 
      if (!(*image_get_scanline) (scanline)) break;

      scanline_count++;
      gf_row++;
    }
  
  if (scanline_count != image_header.height)
    WARNING2 ("Expected %u scanlines, read %u", image_header.height,
              scanline_count); 
}
