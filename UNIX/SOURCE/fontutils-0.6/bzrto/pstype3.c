/* pstype3.c: translate the BZR font to a Type 3 PostScript font. 
   See section 5.3 of the PostScript Language Reference Manual for
   details on how Type 3 fonts work.

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

#include "bounding-box.h"
#include "bzr.h"
#include "encoding.h"
#include "spline.h"

#include "pstype3.h"
#include "psutil.h"


/* Where we'll output.  */
static FILE *ps_file;
static string ps_filename;

/* The interword space, in points.  */
static real interword_space;

/* Whether or not a particular character has been written.  We depend on
   this being initialized to false by virtue of its storage class.  */
static boolean char_output_p[ENCODING_VECTOR_SIZE];


static void out_splines (spline_list_array_type);

/* Output macros.  */

/* This should be used for outputting a string S on a line by itself.  */
#define OUT_LINE(s)							\
  do { fprintf (ps_file, "%s\n", s); } while (0)			\

/* These output their arguments, preceded by the indentation.  */
#define OUT1(s, e)							\
  do { fprintf (ps_file, s, e); } while (0)

#define OUT2(s, e1, e2)							\
  do { fprintf (ps_file, s, e1, e2); } while (0)

#define OUT4(s, e1, e2, e3, e4)						\
  do { fprintf (ps_file, s, e1, e2, e3, e4); } while (0)

#define OUT5(s, e1, e2, e3, e4, e5)					\
  do { fprintf (ps_file, s, e1, e2, e3, e4, e5); } while (0)

/* These macros just output their arguments.  */
#define OUT_STRING(s)	fprintf (ps_file, "%s", s)
#define OUT_REAL(r)	fprintf (ps_file,				\
                                 r == ROUND (r) ? "%.0f " : "%.3f ", r)

/* For a PostScript command with two real arguments, e.g., lineto.  OP
   should be a constant string.  */
#define OUT_COMMAND2(first, second, op)					\
  do									\
    {									\
      OUT_STRING ("  ");						\
      OUT_REAL (first);							\
      OUT_REAL (second);						\
      OUT_STRING (op "\n");						\
    }									\
  while (0)

/* For a PostScript command with six real arguments, e.g., curveto. 
   Again, OP should be a constant string.  */
#define OUT_COMMAND6(first, second, third, fourth, fifth, sixth, op)	\
  do									\
    {									\
      OUT_STRING ("  ");						\
      OUT_REAL (first);							\
      OUT_REAL (second);						\
      OUT_STRING (" ");							\
      OUT_REAL (third);							\
      OUT_REAL (fourth);						\
      OUT_STRING (" ");							\
      OUT_REAL (fifth);							\
      OUT_REAL (sixth);							\
      OUT_STRING (" " op " \n");					\
    }									\
   while (0)

/* This should be called before the others in this file.  It opens the
   output file `OUTPUT_NAME.pf3', and writes some preliminary boilerplate,
   including the font matrix and bounding box, then sets up to write the
   characters.  We generate `(BUILD_CHAR_FILENAME) run' in the
   PostScript output; the file should define a BuildChar routine
   named `BZRBuildChar'.  */

#ifndef BUILD_CHAR_FILENAME
#define BUILD_CHAR_FILENAME "bzrbuildch.PS"
#endif

void
pstype3_start_output (string output_name, 
		      bzr_preamble_type pre, bzr_postamble_type post)
{
  ps_font_info_type ps_info = ps_set_font_info ();

  /* I don't know of any convention about what extension to give Type 3
     fonts, so I use `pf3'.  */
  ps_filename = extend_filename (output_name, "pf3");
  ps_file = xfopen (ps_filename, "w");
  
  /* Remember the natural interword space, so we can output it later, if
     necessary.  */
  interword_space = ps_info.interword_space;

  /* Output some identification.  */
  OUT_LINE ("%!");

  ps_start_font (ps_file, ps_info, BZR_COMMENT (pre));
  OUT_LINE ("/FontType 3 def");
  
  /* We should probably physically insert the BuildChar routine, as
     (run) doesn't work when the font is downloaded.  */
  OUT_LINE ("(" BUILD_CHAR_FILENAME ") run");
  OUT_LINE ("/BuildChar { BZRBuildChar } def");

  OUT2 ("/FontMatrix [ %.3f 0 0 %.3f 0 0 ] def\n",
        1.0 / BZR_DESIGN_SIZE (pre), 1.0 / BZR_DESIGN_SIZE (pre));
  OUT4 ("/FontBBox [ %.3f %.3f %.3f %.3f ] def\n",
        MIN_COL (BZR_FONT_BB (post)), MIN_ROW (BZR_FONT_BB (post)),
        MAX_COL (BZR_FONT_BB (post)), MAX_ROW (BZR_FONT_BB (post)));

  /* One extra for `.notdef' and one, perhaps, for `space'.   */
  OUT1 ("/CharDescriptions %d dict begin\n", BZR_NCHARS (post) + 2);

  OUT_LINE ("/.notdef [ 0 0 0 0 0 0 {} ] def");
}

/* This is called on each character to be output.  We set the global
   `char_output_p' for the given character code.  It then outputs a
   character description in the following format (which depends on the
   BuildChar routine):

	 /<PostScript encoding name>
	 [ <character width>  0  <character bounding box> {
	       <series of moveto, lineto, and curveto commands>
	       fill } def ]
   */

void
pstype3_output_char (bzr_char_type c)
{
  string charname = ps_encoding_name (CHARCODE (c));
  
  if (!STREQ (charname, ".notdef"))
    {
      char_output_p[CHARCODE (c)] = true;

      OUT1 ("/%s ", charname);
      OUT5 ("[ %.3f 0 %.3f %.3f %.3f %.3f {\n", CHAR_SET_WIDTH (c),
                  CHAR_MIN_COL (c), CHAR_MIN_ROW (c),
                  CHAR_MAX_COL (c), CHAR_MAX_ROW (c));

      out_splines (BZR_SHAPE (c));

      OUT_LINE ("  fill } ] def");
    }
}


/* This outputs the PostScript code which produces the shape in
   SHAPE.  */

static void
out_splines (spline_list_array_type shape)
{
  unsigned this_list;
                            
  for (this_list = 0; this_list < SPLINE_LIST_ARRAY_LENGTH (shape);
       this_list++)
    {
      unsigned this_spline;
      spline_list_type list = SPLINE_LIST_ARRAY_ELT (shape, this_list);
      spline_type first = SPLINE_LIST_ELT (list, 0);

      OUT_COMMAND2 (START_POINT (first).x, START_POINT (first).y, "moveto");

      for (this_spline = 0; this_spline < SPLINE_LIST_LENGTH (list);
           this_spline++)
        {
          spline_type s = SPLINE_LIST_ELT (list, this_spline);

          if (SPLINE_DEGREE (s) == LINEAR)
            OUT_COMMAND2 (END_POINT (s).x, END_POINT (s).y, "lineto");
          else
            OUT_COMMAND6 (CONTROL1 (s).x, CONTROL1 (s).y,
                          CONTROL2 (s).x, CONTROL2 (s).y,
                          END_POINT (s).x, END_POINT (s).y,
                          "curveto");
        }
      OUT_LINE ("  closepath");
    }
}

/* This finishes up the PostScript for the character descriptions and for
  the rest of the font description.  It also closes the output file.  */

void
pstype3_finish_output ()
{
  int space_encoding = ps_encoding_number ("space");
  
  /* If the space character hasn't been output, do so (even if it won't
     be encoded).  */
  if (space_encoding == -1 || !char_output_p[space_encoding])
    {
      OUT1 ("/space [ %d 0 0 0 0 0 {} ] def\n", ROUND (interword_space));
      if (space_encoding != -1)
        char_output_p[space_encoding] = true;
    }

  /* Finish up the characters' descriptions.  */
  OUT_LINE ("currentdict end readonly def\n");

  /* Now that we've seen all the characters, we can output the encoding.  */
  ps_output_encoding (ps_file, char_output_p);

  /* Define and discard the font.  */
  OUT_LINE ("currentdict end definefont pop");

  xfclose (ps_file, ps_filename);
}
