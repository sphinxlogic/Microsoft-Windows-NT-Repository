/* output.c: use the `char_type's' information to output a new font.

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

#include "encoding.h"               
#include "filename.h"
#include "gf.h"
#include "libfile.h"
#include "report.h"
#include "tfm.h"

#include "char.h"
#include "main.h"
#include "output.h"
#include "symtab.h"


/* The assignments to the TFM fontdimen values.  (-fontdimens)  */
string fontdimens = NULL;

/* The name of the output file specified by the user.  (-output-file)  */
string output_name = NULL;

/* The character code to use for the x-height.  (-xheight-char)  */
charcode_type xheight_char = 120; /* `x' in ASCII.  */


static void finalize_kern_list (tfm_char_type *, char_type *[]);
static void finalize_lig_list (tfm_char_type *, char_type *[]);
static tfm_global_info_type find_tfm_info (char_type *[]);
static gf_char_type make_gf_char (char_info_type);
static void update_tfm_info_from_symtab (tfm_global_info_type *);
static void write_gf (bitmap_font_type, char_type *[]);
static void write_tfm (char_type *[]);
static boolean write_tfm_char (charcode_type, char_type *[]);

/* Output a TFM and (optionally) GF file with the characters in CHARS. 
   Use the characters from BITMAP_FONT for the shapes.  We use an
   existing TFM file, the encoding file, and command line arguments to
   determine the metric information.  */

void
output_font (bitmap_font_type bitmap_font, char_type *chars[])
{
  write_tfm (chars);
  
  if (!no_gf)
    write_gf (bitmap_font, chars);
}

/* Write a TFM file with CHARS.  */

static void
write_tfm (char_type *chars[])
{
  unsigned this_char;
  unsigned char_count = 0;
  string pl_output_name = extend_filename (output_name, "pl");
  tfm_global_info_type tfm_info = find_tfm_info (chars);

  /* If `have_tfm', we've already opened the input file.  */
  string input_tfm_name = have_tfm ? tfm_input_filename () : NULL;
  
  /* We remove the suffix before making the PL name because if the user
     said `-output-file=foo.bar', it's the TFM file which should be
     named `foo.bar'; the PL file is still named `foo.pl'.  */
  string output_root = remove_suffix (output_name);
  string pl_name = extend_filename (output_root, "pl");
  string output_tfm_name = extend_filename (output_name, "tfm");

  /* If we'd overwrite the input, change the name.  */
  if (input_tfm_name != NULL && same_file_p (input_tfm_name, output_tfm_name))
    {
      pl_name = make_prefix ("x", pl_name);
      output_tfm_name = make_prefix ("x", output_tfm_name);
    }
    
  /* Let's write the TFM file first, but to a PL file, which is easier.  */
  if (!tfm_open_pl_output_file (pl_output_name))
    FATAL_PERROR (pl_output_name);

  REPORT1 ("\nWriting PL file `%s':\n", pl_output_name);

  /* Write the global information we just figured out.  */
  tfm_put_global_info (tfm_info);

  /* Only look between `starting_char' and `ending_char', i.e., the
     user's -range.  */
  for (this_char = starting_char; this_char <= ending_char; this_char++)
    {
      if (write_tfm_char (this_char, chars))
        REPORT2 ("[%d]%c", this_char, ++char_count % 13 == 0 ? '\n' : ' ');
    }

  if (char_count % 13 != 0)
    REPORT ("\n");

  tfm_convert_pl (output_tfm_name, verbose);
  tfm_close_pl_output_file ();
}


/* Return the global TFM information structure: we read from an existing
   TFM file, if the global `have_tfm' is true, then from the encoding
   information in the global `encoding_info', then from the symbol
   table, and finally from the command line (i.e., the global
   `fontdimens).  */

static tfm_global_info_type
find_tfm_info (char_type *chars[])
{
  tfm_global_info_type tfm_info;
  
  if (have_tfm)
    {
      tfm_info = tfm_get_global_info ();
      TFM_CHECKSUM (tfm_info) = 0; /* Can't use the old checksum.  */
    }
  else
    { /* No TFM file to be found, so initialize the structure
         with the defaults.  */
      tfm_info = tfm_init_global_info ();
      TFM_DESIGN_SIZE (tfm_info) = get_designsize_in_points ();
    }

  /* `encoding_info' has been set at this point, one way or another; use
     whatever codingscheme it says.  */
  TFM_CODING_SCHEME (tfm_info) = ENCODING_SCHEME_NAME (encoding_info);

  /* The `fontsize' fontdimen is typically the same as the designsize.
     (I can't imagine when it would be anything else, but it seems
     prudent to let the user-specified values in the symbol table or
     commandline override this.)  */
  tfm_set_fontsize (&tfm_info);
  
  /* Use the bitmap height of `xheight_char', if it exists, for the
     default x-height.  If not, guess .5 of the designsize. */
  TFM_FONTDIMEN (tfm_info, TFM_XHEIGHT_PARAMETER)
    = chars[xheight_char] != NULL
       ? PIXELS_TO_POINTS (
              CHAR_HEIGHT (*CHAR_BITMAP_INFO (*chars[xheight_char])), dpi_real)
       : .5 * TFM_DESIGN_SIZE (tfm_info);
  
  /* If any fontdimens were set in the CMI files, use them.  */
  update_tfm_info_from_symtab (&tfm_info);
  
  /* Finally, update the fontdimens from the command line.  */
  tfm_set_fontdimens (fontdimens, &tfm_info);

  return tfm_info;
}


/* It is not an error for the user to use a fontdimen name for something
   else, so we do not complain if we cannot resolve such a name.  */

static void
update_tfm_info_from_symtab (tfm_global_info_type *tfm_info)
{
  unsigned p;
  
  for (p = 1; p <= TFM_MAX_FONTDIMENS; p++)
    {
      string name = tfm_fontdimen_name (p);
      symval_type *sv = name ? symtab_lookup (name) : NULL;
      if (sv != NULL)
        {
          if (symval_resolve (sv))
            tfm_set_fontdimen (tfm_info, p,
	                       PIXELS_TO_POINTS (SYMVAL_REAL (*sv), dpi_real));
	}
    }
}


/* Return true if we have TFM info for CODE in CHARS.  */

static boolean
have_tfm_char (charcode_type code, char_type *chars[])
{
  boolean ok;
  
  /* If we have no `char_type' for CODE, have to give up.  */
  if (chars[code] == NULL)
    ok = false;
  
  /* If we have no TFM info in the `char_type', have to give up.  */
  else if (CHAR_TFM_INFO (*chars[code]) == NULL)
    ok = false;
  
/* xx what about _EXISTS? */
  
  else
    ok = true;
  
  return ok;
}


/* Write the single character CODE from CHARS to the TFM file (assume
   the latter is open), if we have all the information.  Return true if
   we wrote it, else false.  */

static boolean
write_tfm_char (charcode_type code, char_type *chars[])
{
  boolean ok;
  
  if (have_tfm_char (code, chars))
    {
      tfm_char_type c = *CHAR_TFM_INFO (*chars[code]);

      /* The remaining thing to check is that the kerns and ligatures
         refer only to (other) characters which will be output.  */
      finalize_kern_list (&c, chars);
      finalize_lig_list (&c, chars);

      /* Write the character.  */
      tfm_put_char (c);
      
      ok = true;
    }
  else
    ok = false;

  return ok;
}


/* Look through the kern list in C for kerns which refer to characters
   in CHARS we won't be outputting.  Remove such.  */

static void
finalize_kern_list (tfm_char_type *c, char_type *chars[])
{
  unsigned e;
  list_type good_list = list_init ();

  for (e = 0; e < LIST_SIZE (TFM_KERN (*c)); e++)
    {
      tfm_kern_type *k = LIST_ELT (TFM_KERN (*c), e);

      if (have_tfm_char (k->character, chars))
        {
          tfm_kern_type *new_kern = LIST_TAPPEND (&good_list, tfm_kern_type);
          *new_kern = *k;
	}
    }
  
  /* Throw away the old list.  */
  list_free (&TFM_KERN (*c));
  
  /* Insert the good list.  */
  TFM_KERN (*c) = good_list;
}


/* Routine for ligatures analogous to `finalize_kern_list'.  */

static void
finalize_lig_list (tfm_char_type *c, char_type *chars[])
{
  unsigned e;
  list_type good_list = list_init ();

  for (e = 0; e < LIST_SIZE (TFM_LIGATURE (*c)); e++)
    {
      tfm_ligature_type *lig = LIST_ELT (TFM_LIGATURE (*c), e);

      if (have_tfm_char (lig->character, chars)
          && have_tfm_char (lig->ligature, chars))
        {
          tfm_ligature_type *new_lig
  	    = LIST_TAPPEND (&good_list, tfm_ligature_type);
          *new_lig = *lig;
	}
    }
  
  /* Throw away the old list.  */
  list_free (&TFM_LIGATURE (*c));
  
  /* Insert the good list.  */
  TFM_LIGATURE (*c) = good_list;
}

/* Write a GF file using BITMAP_FONT and CHARS.  Don't overwrite an
   existing file named `BITMAP_FONT_FILENAME (FONT)'.  */

static void
write_gf (bitmap_font_type font, char_type *chars[])
{
  unsigned this_char;
  unsigned char_count = 0;
  /* We depend upon `output_name' being set when we are called.  */
  string gf_output_name = extend_filename (output_name, concat (dpi, "gf"));

  if (same_file_p (BITMAP_FONT_FILENAME (font), gf_output_name))
    gf_output_name = make_prefix ("x", gf_output_name);

  if (!gf_open_output_file (gf_output_name))
    FATAL_PERROR (gf_output_name);

  REPORT1 ("\nWriting GF file `%s':\n", gf_output_name);

  /* Write the first part of the file.  */
  gf_put_preamble (concat4 ("charspace output ", now () + 4, " from ",
                            BITMAP_FONT_COMMENT (font)));

  for (this_char = starting_char; this_char <= ending_char; this_char++)
    {
      if (chars[this_char] != NULL
          && CHAR_BITMAP_INFO (*chars[this_char]) != NULL)
        {
          gf_put_char (make_gf_char (*CHAR_BITMAP_INFO (*chars[this_char])));
          REPORT2 ("[%d]%c", this_char, ++char_count % 13 == 0 ? '\n' : ' ');
        }
    }

  if (char_count % 13 != 0)
    REPORT ("\n");

  /* Write the last part of the file.  */
  gf_put_postamble (real_to_fix (get_designsize_in_points ()),
                    dpi_real, dpi_real);

  gf_close_output_file ();
}


/* Turn BITMAP_CHAR (the generic structure) into a `gf_char_type'.  We
   assume there are no real changes to be made in BITMAP_CHAR, i.e.,
   that we need only copy the relevant fields.  */

static gf_char_type
make_gf_char (char_info_type bitmap_char)
{
  gf_char_type gf_char;
  
  GF_CHARCODE (gf_char) = CHARCODE (bitmap_char);
  GF_BITMAP (gf_char) = CHAR_BITMAP (bitmap_char);
  GF_CHAR_BB (gf_char) = CHAR_BB (bitmap_char);
  GF_H_ESCAPEMENT (gf_char) = CHAR_SET_WIDTH (bitmap_char);
  GF_TFM_WIDTH (gf_char) = CHAR_TFM_WIDTH (bitmap_char);
  
  return gf_char;
}
