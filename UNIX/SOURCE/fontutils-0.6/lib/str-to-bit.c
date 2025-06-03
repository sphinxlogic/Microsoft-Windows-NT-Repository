/* str-to-bit.c: typeset a text string in some font, producing a bitmap.

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

#include <ctype.h>
#include "font.h"


/* Turn the C string TEXT into a bitmap, taking the characters from the
   font FONT_NAME at resolution DPI.  Any space characters in TEXT turn
   into the natural interword space for this font.  No escapes are
   recognized in TEXT.  We don't close FONT_NAME when we're done, since
   the most common case is to be called multiple times on a single font.  */

bitmap_type
string_to_bitmap (string text, string font_name, unsigned dpi)
{
  bitmap_type b;
  unsigned this_char;
  char_info_type *chars[strlen (text)];
  font_info_type font = get_font (font_name, dpi);
  int width = 0, height = 0, depth = 0;
  int x = 0, y = 0;  /* Our current position.  */
  real tfm_space
    = TFM_SAFE_FONTDIMEN (FONT_TFM_FONT (font), TFM_SPACE_PARAMETER, 0);
  int font_space = POINTS_TO_PIXELS (tfm_space, dpi);

  for (this_char = 0; this_char < strlen (text); this_char++)
    {
      /* Turn any kind of space character into a normal interword space.  */
      if (isspace (text[this_char]))
        width += font_space;
      else
        {
	  chars[this_char] = get_char (font_name, text[this_char]);
          
          /* If the character doesn't exist in this font, just keep
             going.  */
          if (chars[this_char] == NULL)
            continue;
            
          /* The set width should be equal to the left side bearing plus
             the bitmap width plus the right side bearing.  */
	  width += CHAR_SET_WIDTH (*chars[this_char]);
	  height = MAX (height, CHAR_HEIGHT (*chars[this_char]));
	  depth = MAX (depth, CHAR_DEPTH (*chars[this_char]));
        }
    }

  /* Unless the image is nonnegative both horizontally and vertically,
     it is invisible.  */
  if (width <= 0 || (height + depth <= 0))
    {
      BITMAP_WIDTH (b) = 0;
      BITMAP_HEIGHT (b) = 0;
      BITMAP_BITS (b) = NULL;
      return b;
    }

  /* The image is going to be visible.  */
  b = new_bitmap ((dimensions_type) { height + depth, width });
  
  for (this_char = 0; this_char < strlen (text); this_char++)
    {
      if (isspace (text[this_char]))
        x += font_space;
      else
        {
          int char_x, char_y;
          char_info_type c;
          
          if (chars[this_char] == NULL)
            continue;
          
          c = *chars[this_char];

          x += CHAR_LSB (c);
          x = MAX (x, 0);  /* In case the lsb was negative.  */

          /* Copy the character image to the bitmap we are building, one
             column at a time, from top to bottom.  */
          for (char_x = 0; char_x < CHAR_BITMAP_WIDTH (c); char_x++, x++)
            for (char_y = 0, y = height - CHAR_HEIGHT (c);
                 char_y < CHAR_BITMAP_HEIGHT (c);
                 char_y++, y++)
              BITMAP_PIXEL (b, y, x)
                = BITMAP_PIXEL (CHAR_BITMAP (c), char_y, char_x);
          
          x += CHAR_RSB (c);
        }
    }

  return b;
}
