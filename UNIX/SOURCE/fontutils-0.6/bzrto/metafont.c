/* metafont.c: output the spline descriptions as a Metafont program.

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

#include "bzr.h"
#include "encoding.h"
#include "filename.h"
#include "font.h"
#include "spline.h"
#include "tfm.h"
#include "vector.h"

#include "char.h"
#include "input-ccc.h"
#include "main.h"
#include "metafont.h"

#include "ccc.h"


/* Where the output goes.  */
static FILE *mf_file;
static string mf_filename;


/* Macros for output to `mf_file'.  */
#define OUT1(str, a)		  fprintf (mf_file, str, a)
#define OUT2(str, a1, a2)	  fprintf (mf_file, str, a1, a2)
#define OUT3(str, a1, a2, a3)	  fprintf (mf_file, str, a1, a2, a3)
#define OUT4(str, a1, a2, a3, a4) fprintf (mf_file, str, a1, a2, a3, a4)

#define OUT_LINE(str)      OUT1 ("%s\n", str)
#define OUT_STRING(str)    OUT1 ("%s", str)
#define OUT_STATEMENT(str) OUT1 ("%s;\n", str)

#define OUT_ZASSIGNMENT(lhs, c, l, s, pt)				\
  do {									\
    OUT3 (lhs " = ", c, l, s);						\
    OUT_POINT (pt);							\
    OUT_STRING (";\n");							\
  } while (0)

#define OUT_POINT(p) OUT2 ("(%.3fu,%.3fu)", p.x, p.y)

/* How to indent.  */
#define INDENT "  "


static string charcode_to_subr_name (charcode_type);
static void out_bb_char (real_bounding_box_type *,
                         real_coordinate_type *, bzr_char_type);
static void out_move (real_coordinate_type *, real, real, boolean);
static void output_bzr_body (bzr_char_type);
static real_bounding_box_type output_ccc_body (ccc_type, char_type *[]);
static void output_char_title (charcode_type);
static void output_ligtable (list_type, char_type **);
static void output_subr_start (charcode_type code);
static tfm_char_type **read_tfm_files (list_type);
static void read_tfm_file (tfm_char_type *[], string);

/* Start things off.  We want to write to a Metafont file, e.g.,
   `cmss.mf', given a font name, e.g., `cmss10'.  */

void
metafont_start_output (string output_name, bzr_preamble_type pre, 
		       tfm_global_info_type *info)
{
  unsigned this_param;
  string current_time = now ();

  mf_filename = extend_filename (output_name, "mf");
  mf_file = xfopen (mf_filename, "w");

  /* Output some identification.  */
  OUT1 ("%% This file defines the Metafont font %s.\n", mf_filename);
  OUT1 ("%% {%s}\n", BZR_COMMENT (pre));
  OUT1 ("%% Generated %s.\n", current_time);
  OUT_LINE ("% This font is in the public domain.");
  
  /* Output the size that our original data is based on.  The font can
     be generated at sizes different than this.  */
  OUT1 ("true_design_size# := %.3fpt#;\n", BZR_DESIGN_SIZE (pre));
  
  /* The real work is done in an auxiliary file.  */
  OUT_STATEMENT ("input bzrsetup");

  /* Output the fontwide information given in the global `encoding_info'.  */
  OUT1 ("font_coding_scheme := \"%s\";\n",
        ENCODING_SCHEME_NAME (encoding_info));
  OUT1 ("font_identifier := \"%s\";\n", output_name);

  if (info)
    for (this_param = 1; this_param <= TFM_FONTDIMEN_COUNT (*info);
         this_param++)
      if (TFM_FONTDIMEN (*info, this_param) != 0.0)
        OUT2 ("fontdimen %u: %.3fpt# * u#;\n", this_param,
              TFM_FONTDIMEN (*info, this_param)); 
}


/* Used to output the character names as ``titles'', which Metafont
   displays and puts on proofsheets in some circumstances.  */

static void
output_char_title (charcode_type code)
{
  string title = ENCODING_CHAR_NAME (encoding_info, code);
  
  if (title == NULL)
    title = concat ("character #", utoa (code));
    
  OUT1 (INDENT "\"%s\";\n", title);
}

/* Output of subroutines.  The main part of the subroutine is done in
   the same way as a character, it's just the adminstrative junk that's
   different.  We have to remember some information in case this
   subroutine later gets output as a character.  I'm not entirely sure
   that we need it in all cases, but it seems cleaner to just always
   save it.  */

void
metafont_output_bzr_subr (bzr_char_type c)
{
  output_subr_start (CHARCODE (c));
  output_bzr_body (c);
  OUT_STATEMENT ("enddef");
}


void
metafont_output_ccc_subr (ccc_type c, charcode_type code, char_type *subrs[])
{
  output_subr_start (code);
  output_ccc_body (c, subrs);
  OUT_STATEMENT ("enddef");
}


/* Output the common beginning of a subroutine.  The character name is a
   Metafont ``title'' which gets output on proofsheets and sometimes on
   the terminal.  */

static void
output_subr_start (charcode_type code)
{
  OUT1 ("def %s = \n", charcode_to_subr_name (code));
  output_char_title (code);
}


/* When we do need to output a subroutine, however, we have to choose a
   name for it.  Since Metafont doesn't allow numerals in identifiers,
   so we can't just use the decimal character code to distinguish the
   subroutines.  Instead, we use the character name from the global
   `encoding_info', or, if the encoding name is not strictly alphabetic
   (e.g., `.notdef') a base-16 representation where zero is `a', ...,
   fifteen is `o'.  */

#define OUT_CHAR_SUBR(c) OUT1 (INDENT "%s;\n", charcode_to_subr_name (c))
#define SUBR_NAME_PREFIX "bzr_subr_"

static string
charcode_to_subr_name (charcode_type code)
{
  string subr_name;
  string name = ENCODING_CHAR_NAME (encoding_info, code);

  if (strspn (name, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
      != strlen (name))
    {
      unsigned high = code >> 4;
      unsigned low = code - (high << 4);
      char alpha[3] = { high + 'a', low + 'a', 0 };

      subr_name = concat (SUBR_NAME_PREFIX, alpha);
    }
  else
    subr_name = concat (SUBR_NAME_PREFIX, name);
  
  return subr_name;
}

/* Output of characters.  Eventually we should do something about
   italic corrections.  Also, it might be better to use a symbolic name
   for the character code instead of hardwiring an integer constant.
   Then different encodings could be selected at Metafont-time.  If the
   character is already as a subroutine, just output a call.  */

void
metafont_output_bzr_char (bzr_char_type c, char_type *subrs[])
{
  unsigned code = CHARCODE (c);
  
  OUT4 ("\nbeginchar (%d, %.3fu#, %.3fu#, %.3fu#);\n", 
        code, CHAR_SET_WIDTH (c), CHAR_MAX_ROW (c), CHAR_DEPTH (c));
  
  if (subrs[code])
    OUT_CHAR_SUBR (code);
  else
    {
      output_char_title (code);
      output_bzr_body (c);
    }
  
  OUT_STATEMENT ("endchar");
}


void
metafont_output_ccc_char (ccc_type c, charcode_type code, char_type *subrs[])
{
  real_bounding_box_type bb;
  
  if (subrs[code])
    WARNING ("Sorry, CCC characters can't be subroutines yet");
  else
    {
      OUT1 ("\nbegin_no_dimen_char (%d);\n", code);

      output_char_title (code);
      bb = output_ccc_body (c, subrs);

      /* Set this character's dimensions: width, height, depth.  */
      OUT3 (INDENT "set_char_dimens (%.3fu#, %.3fu#, %.3fu#);\n",
            BB_WIDTH (bb), MAX_ROW (bb), -MIN_ROW (bb));
      OUT_STATEMENT ("end_no_dimen_char");
    }
}

/* Output the shape in the BZR character C.  */

static void
output_bzr_body (bzr_char_type c)
{
  unsigned this_list;
  charcode_type code = CHARCODE (c);
  spline_list_array_type shape = BZR_SHAPE (c);
  boolean was_output = false;

  /* Go through the list of splines once, assigning the control points
     to Metafont variables.  This allows us to produce more information
     on proofsheets.  */

  for (this_list = 0; this_list < SPLINE_LIST_ARRAY_LENGTH (shape);
       this_list++)
    {
      unsigned this_spline;
      spline_list_type list = SPLINE_LIST_ARRAY_ELT (shape, this_list);

      if (SPLINE_LIST_LENGTH (list) == 0)
        continue;
        
      was_output = true;
      
      OUT_ZASSIGNMENT (INDENT "z%u\\%u\\%us", code, this_list, 0,
                       START_POINT (SPLINE_LIST_ELT (list, 0))); 

      for (this_spline = 0; this_spline < SPLINE_LIST_LENGTH (list);
           this_spline++) 
        {
          spline_type s = SPLINE_LIST_ELT (list, this_spline);

          if (SPLINE_DEGREE (s) == CUBIC)
            {
              OUT_ZASSIGNMENT (INDENT "z%u\\%u\\%uc1", code, this_list,
			       this_spline, CONTROL1 (s)); 
              OUT_ZASSIGNMENT (INDENT "z%u\\%u\\%uc2", code, this_list,
			       this_spline, CONTROL2 (s)); 
            }

          /* The last point in the list is also the first point, and we
             don't want to output two variables for the same point.  */
          if (this_spline < SPLINE_LIST_LENGTH (list) - 1)
            OUT_ZASSIGNMENT (INDENT "z%u\\%u\\%u", code, this_list,
		             this_spline, END_POINT (s));
        }
    }


  /* Go through the list of splines again, outputting the
     path-construction commands.  */

  for (this_list = 0; this_list < SPLINE_LIST_ARRAY_LENGTH (shape);
       this_list++)
    {
      unsigned this_spline;
      spline_list_type list = SPLINE_LIST_ARRAY_ELT (shape, this_list);

      if (SPLINE_LIST_LENGTH (list) == 0)
        continue;
        
      OUT3 (INDENT "fill_or_unfill z%u\\%u\\%us\n", code, this_list, 0);

      for (this_spline = 0; this_spline < SPLINE_LIST_LENGTH (list);
           this_spline++)
        {
          spline_type s = SPLINE_LIST_ELT (list, this_spline);

          if (SPLINE_DEGREE (s) == LINEAR)
            OUT_STRING (INDENT INDENT "--");
          else
            {
              OUT_STRING (INDENT INDENT "..controls ");
              OUT3 ("z%u\\%u\\%uc1 and ", code, this_list, this_spline);
              OUT3 ("z%u\\%u\\%uc2..", code, this_list, this_spline);
            }
	  
          if (this_spline < SPLINE_LIST_LENGTH (list) - 1)
            OUT3 ("z%u\\%u\\%u\n", code, this_list, this_spline);
        }
        
      OUT_STRING ("cycle;\n");
    }
  
  /* The plain Metafont `labels' command makes it possible to produce
     proofsheets with all the points labeled.  We always want labels for
     the starting and ending points, and possibly labels for the control
     points on each spline, so we have our own command `proof_labels',
     defined in `bzrsetup.mf'.  */
     
  if (was_output)
    OUT_LINE (INDENT "proof_labels (");
  
  for (this_list = 0; this_list < SPLINE_LIST_ARRAY_LENGTH (shape);
       this_list++)
    {
      unsigned this_spline;
      spline_list_type list = SPLINE_LIST_ARRAY_ELT (shape, this_list);

      for (this_spline = 0; this_spline < SPLINE_LIST_LENGTH (list);
           this_spline++)
        OUT3 (INDENT INDENT "%u\\%u\\%u,\n", code, this_list, this_spline);
    }

  if (was_output)
    OUT_STRING (");\n");
}


/* Output the definition of CCC character CCC_CHAR.  All the subroutines
   must be defined in SUBRS.  */

#define OUT_HMOVE(h, a) out_move (&current, h, 0.0, a)
#define OUT_VMOVE(v, a) out_move (&current, 0.0, v, a)

static real_bounding_box_type
output_ccc_body (ccc_type ccc_char, char_type *subrs[])
{
  unsigned this_cmd;
  real_coordinate_type current = (real_coordinate_type) { 0.0, 0.0 };
  real_bounding_box_type bb
    = (real_bounding_box_type) { FLT_MAX, FLT_MIN, FLT_MAX, FLT_MIN };

  for (this_cmd = 0; this_cmd < LIST_SIZE (ccc_char); this_cmd++)
    { 
      ccc_cmd_type cmd = *(ccc_cmd_type *) LIST_ELT (ccc_char, this_cmd);
      
      switch (CCC_OPCODE (cmd))
        {
        case TR_SETCHAR:
          {
            char_type subr = *subrs[CCC_CHARCODE (cmd)];

            if (CHAR_CLASS (subr) == bzr_char_class)
              {
                bzr_char_type bzr_char = *CHAR_BZR (subr);
                
                update_real_bounding_box (&bb, current);
                OUT_HMOVE (CHAR_LSB (bzr_char), false);

                out_bb_char (&bb, &current, bzr_char);

                OUT_HMOVE (CHAR_RSB (bzr_char), false);
                update_real_bounding_box (&bb, current);
              }
            else
              /* We haven't implemented this yet since it doesn't seem
                 all that necessary at the moment, and it would require
                 figuring out a bounding box and possibly side bearings
                 for CCC character -- certainly not impossible, but not
                 trivial, either.  */
              WARNING ("Sorry, I can't `setchar' a CCC char yet");
          }
          break;

        case TR_SETCHARBB:
          {
            char_type subr = *subrs[CCC_CHARCODE (cmd)];

            if (CHAR_CLASS (subr) == bzr_char_class)
              out_bb_char (&bb, &current, *CHAR_BZR (subr));
            else
              WARNING ("Sorry, I can't `setcharbb' a CCC char yet");
          }
          break;

        case TR_HMOVE:
          OUT_HMOVE (CCC_DIMEN_AMOUNT (cmd), CCC_DIMEN_ABSOLUTE (cmd));
          break;

        case TR_VMOVE:
          OUT_VMOVE (CCC_DIMEN_AMOUNT (cmd), CCC_DIMEN_ABSOLUTE (cmd));
          break;

        default:
          FATAL1 ("Bad command %d in CCC character", CCC_OPCODE (cmd));
	}
    }
  
  return bb;
}


/* Move by H horizontally and V vertically.  If ABSOLUTE is true, use
   `pt' for units, otherwise `u'.  Update CURRENT.  */

static void
out_move (real_coordinate_type *current, real h, real v, boolean absolute)
{
  string units = absolute ? "pt" : "u";
  
  OUT4 (INDENT "move (%.3f%s, %.3f%s);\n", (real) h, units, (real) v, units);
  current->x += h;
  current->y += v;
}


/* Output the character shape in BZR_CHAR, sans side bearings.  Update
   CURRENT by the width of the shape, and update BB by the points before
   and after the character.  We take into account the height of the
   character when updating BB, even though we don't translate the
   current point up there.  */

#define VOFFSET_POINT(voffset) \
  ((real_coordinate_type) { current->x, current->y + (voffset) })

static void
out_bb_char (real_bounding_box_type *bb, real_coordinate_type *current,
             bzr_char_type bzr_char)
{
  /* Update BB with the current point.  */
  update_real_bounding_box (bb, VOFFSET_POINT (CHAR_MIN_ROW (bzr_char)));

  /* Output the call to the character.  */
  OUT_CHAR_SUBR (CHARCODE (bzr_char));
  
  /* Move by the width of the shape.  */
  out_move (current, BB_WIDTH (CHAR_BB (bzr_char)), 0, false);

  /* Update the BB again with the new current point.  */
  update_real_bounding_box (bb, VOFFSET_POINT (CHAR_MAX_ROW (bzr_char)));
}

/* We use a macro `bye' to finish off the Metafont output, instead of the
   primitive `end', for maximal flexibility.  (Although in practice it
   makes no difference, since `end' can be redefined just like `bye'.)  */

void
metafont_finish_output (list_type bzr_names, char_type **chars)
{
  output_ligtable (bzr_names, chars);
  
  OUT_LINE ("\nbye.");
  xfclose (mf_file, mf_filename);
}


/* True if the character CODE exists in the array `tfm_chars'.  */

#define HAVE_CHAR_P(code) (tfm_chars[code] != NULL)

/* Start or continue the ligtable for the character CODE.  */
#define LIGTABLE_ENTRY(code)						\
  if (output_something)							\
    OUT_LINE (",");							\
  else									\
    {									\
      output_something = true;						\
      OUT1 ("ligtable hex\"%x\":\n", code);				\
    }


/* Output the kerning and ligature information for this font.  Omit it
   for any character which didn't get output.  */

static void
output_ligtable (list_type bzr_names, char_type **chars)
{
  unsigned code;
  tfm_char_type **tfm_chars = read_tfm_files (bzr_names);
  
  for (code = starting_char; code <= ending_char; code++)
    {
      list_type kern_list, ligature_list;
      unsigned this_kern, this_lig;
      tfm_char_type *c;
      boolean output_something = false;
      
      /* If we don't have TFM information for CODE, don't do anything.  */
      if (!HAVE_CHAR_P (code))
        continue;

      c = tfm_chars[code];

      /* Output the kerns.  */
      kern_list = c->kern;
      for (this_kern = 0; this_kern < LIST_SIZE (kern_list); this_kern++)
        {
          tfm_kern_type *kern = LIST_ELT (kern_list, this_kern);

          if (HAVE_CHAR_P (kern->character))
            {
              LIGTABLE_ENTRY (code);
              OUT2 ("hex\"%x\" kern %.3fpt#", kern->character, kern->kern);
            }
        }
      
      /* Output the ligatures.  */
      ligature_list = c->ligature;
      for (this_lig = 0; this_lig < LIST_SIZE (ligature_list); this_lig++)
        {
          tfm_ligature_type *lig = LIST_ELT (ligature_list, this_lig);

          if (HAVE_CHAR_P (lig->character) && HAVE_CHAR_P (lig->ligature))
          {
  	    LIGTABLE_ENTRY (code);
            OUT2 ("hex\"%x\" =: hex\"%x\"", lig->character, lig->ligature);
          }
        }

      if (output_something)
        OUT_LINE (";");
    }
}


/* We want to retain kerning and ligature information from the original
   TFM files, if such exist.  Our goal here is to read the TFM file
   corresponding to each name in BZR_NAMES and return the union of all
   the characters found as an array of `tfm_char_type's.  We do not
   complain about nonexistent TFM files.  */

static tfm_char_type **
read_tfm_files (list_type bzr_names)
{
  unsigned c, name;
  tfm_char_type **tfm_chars = XTALLOC (MAX_CHARCODE + 1, tfm_char_type *);
  
  /* Assume we'll find nothing.  */
  for (c = 0; c <= MAX_CHARCODE; c++)
    tfm_chars[c] = NULL;

  /* Read all the files, updating the array as we go.  */
  for (name = 0; name < LIST_SIZE (bzr_names); name++)
    {
      string *bzr_name = LIST_ELT (bzr_names, name);
      
      read_tfm_file (tfm_chars, *bzr_name);
    }

  return tfm_chars;
}


/* If the TFM file corresponding to BZR_NAME exists, read it and update
   TFM_CHARS.  We do not overwrite existing TFM characters, since
   earlier files should override later files.  (The first file is the
   main input file.)  Arguably multiple characters should be a warning,
   but I'm not sure.  */

static void
read_tfm_file (tfm_char_type *tfm_chars[], string bzr_name)
{
  string tfm_root = make_suffix (bzr_name, "tfm");
  string tfm_name = find_tfm_filename (tfm_root);

  if (tfm_name != NULL && tfm_open_input_file (tfm_name))
    {
      unsigned code;
      tfm_char_type *file_chars = tfm_get_chars ();
      
      for (code = 0; code <= MAX_CHARCODE; code++)
        {
          if (tfm_chars[code] == NULL && TFM_CHAR_EXISTS (file_chars[code]))
            {
              tfm_chars[code] = XTALLOC1 (tfm_char_type);
              *tfm_chars[code] = file_chars[code];
            }
        }

      free (file_chars);
      tfm_close_input_file ();
    }
}
