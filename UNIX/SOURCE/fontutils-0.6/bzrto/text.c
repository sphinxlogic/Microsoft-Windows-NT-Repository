/* text.c: translate the binary BZR font to human-oriented text, but
   make the text sufficiently systematic that a computer can read it,
   too.  The output format resembles the property list (PL) format to which
   the TeX utility `tftopl' translates TFM files.

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
#include "text.h"
#include "bzr.h"

/* Because Emacs Lisp doesn't have floating-point constants, we resort
   to putting the numbers in strings.  */
#define OUT_NUM4(n1, n2, n3, n4)					\
  do {									\
    OUT_NUM2 (n1, n2);							\
    printf ("  ");							\
    OUT_NUM2 (n3, n4);							\
  } while (0)

#define OUT_NUM2(n1, n2) printf ("\"%.3f\" \"%.3f\"", n1, n2)


/* Print the preamble.  */

void
text_start_output (string font_name, bzr_preamble_type p)
{
  printf ("(fontfile \"%s\")\n", font_name);
  printf ("(fontcomment \"%s\")\n", BZR_COMMENT (p));
  printf ("(designsize %f)\n", BZR_DESIGN_SIZE (p));
}

/* Print one character in the file.  The iteration through the spline
   list isn't quite typical here, since we don't need to find the places
   where a new outline starts.  */

void
text_output_bzr_char (bzr_char_type c)
{
  unsigned this_list;
  spline_list_array_type shape = BZR_SHAPE (c);
  
  printf ("(char %d (comment hex 0x%x, octal 0%o",
          CHARCODE (c), CHARCODE (c), CHARCODE (c));
  if (isprint (CHARCODE (c)))
    printf (", ASCII `%c'", CHARCODE (c));
  puts (")");

  printf ("  (width \"%.3f\")\n", CHAR_SET_WIDTH (c));
  printf ("  (bb ");
  OUT_NUM4 (CHAR_MIN_COL (c), CHAR_MAX_COL (c),
            CHAR_MIN_ROW (c), CHAR_MAX_ROW (c));
  puts (")");

  for (this_list = 0; this_list < SPLINE_LIST_ARRAY_LENGTH (shape);
       this_list++)
    {
      unsigned this_spline;
      spline_list_type list = SPLINE_LIST_ARRAY_ELT (shape, this_list);
      
      if (SPLINE_LIST_LENGTH (list) > 0)
        {
          real_coordinate_type start = START_POINT (SPLINE_LIST_ELT (list, 0));
          printf ("  (outline ");
          OUT_NUM2 (start.x, start.y);
          puts ("");
      
          for (this_spline = 0; this_spline < SPLINE_LIST_LENGTH (list);
               this_spline++)
            {
              spline_type s = SPLINE_LIST_ELT (list, this_spline);

              if (SPLINE_DEGREE (s) == LINEAR)
                {
                  printf ("    (line ");
                  OUT_NUM2 (END_POINT (s).x, END_POINT (s).y);
                }

              else
                {
                  printf ("    (spline ");
                  OUT_NUM4 (CONTROL1 (s).x, CONTROL1 (s).y,
                            CONTROL2 (s).x, CONTROL2 (s).y);
                  printf ("  ");
                  OUT_NUM2 (END_POINT (s).x, END_POINT(s).y);
                }
              
              puts (")");
            }
          puts ("  )");
        }
    }

  puts (")");
}


/* Output the composite character.  */
void
text_output_ccc_char (ccc_type c)
{
}

/* Print the parts of the postamble (as the library returns it -- some
   more information is in the file, but not returned, because it is only
   interesting for programs that read the file).  */

void
text_finish_output (bzr_postamble_type p)
{
  printf ("(fontbb ");
  OUT_NUM4 (MIN_COL (BZR_FONT_BB (p)), MAX_COL (BZR_FONT_BB (p)),
            MIN_ROW (BZR_FONT_BB (p)), MAX_ROW (BZR_FONT_BB (p)));
  puts (")");
  printf ("(nchars %u)\n", BZR_NCHARS (p));
}
