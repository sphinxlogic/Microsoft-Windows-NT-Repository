/* char.c: functions to muck with `char_type's.

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

#include "list.h"
#include "report.h"

#include "char.h"
#include "kern.h"
#include "main.h"
#include "symtab.h"


static boolean resolve_sidebearing (sidebearing_type *);
static void update_kerns (char_type *, tfm_char_type *);
static void update_metrics (real, real, char_info_type *, tfm_char_type *);
static void update_sidebearings (char_type *, char_info_type *,
                                 tfm_char_type *);
static char_type *update_via_name (string, char_info_type *, tfm_char_type *);
static void update_width (char_type *, char_info_type *, tfm_char_type *);

/* Initialize the parts of the character structure.  */

char_type
init_char ()
{
  char_type c;
  
  CHAR_BITMAP_INFO (c) = NULL;
  CHAR_TFM_INFO (c) = NULL;
  CHAR_LSB (c) = NULL;
  CHAR_RSB (c) = NULL;
  CHAR_KERNS (c) = list_init ();
  
  return c;
}

/* Our goal here is to extract the information about the character CODE
   from the symbol table, and fill in the information from the bitmap
   and TFM fonts.  We also read ligature information from the encoding
   vector.  C_PTR_ADDR is the address of a pointer to the `char_type' we
   fill in (or will be).
   
   Thus far, we have information only in the symbol table, and thus
   can be retrieved only by character name.  But when it comes
   time to output, we obviously have to do it by number.  If we can find
   `encoding_info's name for CODE in the symbol table, great -- we can
   make *C_PTR_ADDR point at that character and fill it in.  If not,
   then we allocate some new space -- just because the character wasn't
   mentioned in the CMI files shouldn't mean it doesn't get output.
   (Only if we can't find bitmap information for a character do we not
   output it.)
   
   We return true if CODE will be output, false if not.  */

void
do_char (charcode_type code, char_type **c_ptr_addr)
{
  static unsigned char_count = 0;
  string char_name;
  tfm_char_type *tfm_char = NULL;
  
  /* Check that the character CODE exists in the input font.  */
  char_info_type *bitmap_char = get_char (input_name, code);

  /* If no bitmap information, give up.  Everything else we can fake.  */
  if (bitmap_char == NULL)
    return;
  
  REPORT1 ("[%d", CHARCODE (*bitmap_char));
  
  /* Look for existing TFM information.  */
  if (have_tfm)
    tfm_char = tfm_get_char (code);
  
  /* If no prior TFM file, or if the character CODE wasn't there,
     initialize `tfm_char' from the bitmap information.  */
  if (tfm_char == NULL)
    {
      tfm_char = XTALLOC1 (tfm_char_type);
      *tfm_char = tfm_new_char ();
      
      TFM_CHAR_EXISTS (*tfm_char) = true;
      TFM_CHARCODE (*tfm_char) = code;
      TFM_HEIGHT (*tfm_char)
        = PIXELS_TO_POINTS (CHAR_HEIGHT (*bitmap_char), dpi_real);
      TFM_DEPTH (*tfm_char)
        = PIXELS_TO_POINTS (CHAR_DEPTH (*bitmap_char), dpi_real);
      /* Leave the italic correction at zero, since we don't know how to
         compute a reasonable value.  And don't bother to compute the
         width, since we do so below.  */
    }
  
  /* We've initialized the TFM and bitmap information from previously
     existing files.  Next, get the character name from the global
     `encoding_info', so we can get at info in the symbol table.  */
  char_name = ENCODING_CHAR_NAME (encoding_info, code);

  if (char_name == NULL)
    WARNING1 ("No character name for character %d", code);
  else
    *c_ptr_addr = update_via_name (char_name, bitmap_char, tfm_char);
  
  /* If we haven't allocated a `char_type' yet, do so.  The fields which
     get used for the CMI stuff -- the side bearings and kern list --
     don't need to be initialized.  */
  if (*c_ptr_addr == NULL)
    *c_ptr_addr = XTALLOC1 (char_type);

  /* Set the pointers in *C_PTR_ADDR to the new bitmap and TFM info.  */
  CHAR_BITMAP_INFO (**c_ptr_addr) = bitmap_char;
  CHAR_TFM_INFO (**c_ptr_addr) = tfm_char;
  
  REPORT1 ("]%c", ++char_count % 13 == 0 ? '\n' : ' ');

  return;
}

/* Assume that CHAR_NAME exists in the encoding vector.  If looking up
   CHAR_NAME in the symbol table yields a character, return a pointer to
   the `char_type' in the symbol table with updated side bearing
   information.  Otherwise we return NULL.  In any case, update TFM_CHAR
   with the ligatures from the global `encoding_info'.  */

static char_type *
update_via_name (string char_name, char_info_type *bitmap_char,
                 tfm_char_type *tfm_char)
{
  list_type enc_ligs;
  unsigned l;
  char_type *c_ptr = NULL;

  /* We have a character name, so look for side bearing info.  */
  symval_type *sv = symtab_lookup (char_name);

  if (sv == NULL)
    WARNING1 ("%s: Undefined in CMI files", char_name);

  else if (SYMVAL_TAG (*sv) == symval_char)
    { 
      /* We'll return `c_ptr', a pointer to the struct in the symbol table.  */
      c_ptr = &SYMVAL_CHAR (*sv);

      update_sidebearings (c_ptr, bitmap_char, tfm_char);
      update_kerns (c_ptr, tfm_char);
    }

  else if (SYMVAL_TAG (*sv) == symval_char_width)
    { 
      /* We'll return `c_ptr', a pointer to the struct in the symbol table.  */
      c_ptr = &SYMVAL_CHAR (*sv);

      update_width (c_ptr, bitmap_char, tfm_char);
      update_kerns (c_ptr, tfm_char);
    }

  else
    WARNING1 ("%s: Not defined as a character", char_name);

  /* Regardless of whether CHAR_NAME was in the symbol table, we can
     also update the ligature list from `encoding_info'.  */
  enc_ligs = ENCODING_CHAR_LIG (encoding_info, CHARCODE (*bitmap_char));
  for (l = 0; l < LIST_SIZE (enc_ligs); l++)
    {
      tfm_ligature_type *lig = LIST_ELT (enc_ligs, l);
      tfm_set_ligature (&TFM_LIGATURE (*tfm_char), lig->character,
                        lig->ligature);
    }

  return c_ptr;
}

/* Update the set width and sidebearings for a character defined with
   `char-width'.  We take the RSB info in C_PTR to be the desired set
   width and the LSB to be the percentage of whitespace to make the left
   side bearing.  We update BITMAP_CHAR and TFM_CHAR correspondingly.  */

static void
update_width (char_type *c_ptr, char_info_type *bitmap_char,
              tfm_char_type *tfm_char)
{
  real lsb_percent = 0.0;
  charcode_type code = CHARCODE (*bitmap_char);
  /* We might not have info for the character, though, so set up the
     defaults as the existing metrics.  */
  boolean changed_metrics = false;
  real lsb = CHAR_MIN_COL (*bitmap_char);
  real real_width = CHAR_SET_WIDTH (*bitmap_char);

  /* Find the lsb percent.  */
  if (CHAR_LSB (*c_ptr) == NULL)
    WARNING1 ("char %d: No left side bearing information", code);
  else if (resolve_sidebearing (CHAR_LSB (*c_ptr)))
    {
      lsb_percent = SB_REAL (*CHAR_LSB (*c_ptr));
      if (lsb_percent <= 0.0)
        WARNING2 ("char %d: lsb percent (%f) must be nonnegative",
                  code, lsb_percent);
    }

  if (CHAR_RSB (*c_ptr) == NULL)
    WARNING1 ("char %d: No set width information", CHARCODE (*bitmap_char));
  else if (resolve_sidebearing (CHAR_RSB (*c_ptr)))
    {
      real_width = SB_REAL (*CHAR_RSB (*c_ptr));
      changed_metrics = true;
    }

  /* If we have a positive percent, figure out the new lsb.  */
  if (lsb_percent > 0.0)
    {
      real whitespace = real_width - CHAR_BITMAP_WIDTH (*bitmap_char);
      lsb = whitespace * lsb_percent;
      changed_metrics = true;
    }

  /* If necessary, update the widths.  */
  if (changed_metrics)
    update_metrics (real_width, lsb, bitmap_char, tfm_char);
}


/* Call `resolve_sidebearing' on the sidebearing info in C_PTR.  If that
   succeeds, update the various widths and side bearing info in
   BITMAP_CHAR and TFM_CHAR.  This is called for characters defined with
   the `char' command.  */

static void
update_sidebearings (char_type *c_ptr, char_info_type *bitmap_char,
                     tfm_char_type *tfm_char)
{
  /* We might not have side bearing info for it, so set up the defaults
     as the existing side bearings.  */
  boolean changed_sb = false;
  real lsb = CHAR_MIN_COL (*bitmap_char);
  real rsb = CHAR_SET_WIDTH (*bitmap_char) - CHAR_MAX_COL (*bitmap_char);

  /* Compute the side bearings.  */
  if (CHAR_LSB (*c_ptr) == NULL)
    WARNING1 ("char %d: No left side bearing information",
              CHARCODE (*bitmap_char));
  else if (resolve_sidebearing (CHAR_LSB (*c_ptr)))
    {
      lsb = SB_REAL (*CHAR_LSB (*c_ptr));
      changed_sb = true;
    }

  if (CHAR_RSB (*c_ptr) == NULL)
    WARNING1 ("char %d: No right side bearing information",
              CHARCODE (*bitmap_char));
  else if (resolve_sidebearing (CHAR_RSB (*c_ptr)))
    {
      rsb = SB_REAL (*CHAR_RSB (*c_ptr));
      changed_sb = true;
    }

  /* If the side bearings changed, then update the widths.  */
  if (changed_sb)
    {
      real real_width = lsb + CHAR_BITMAP_WIDTH (*bitmap_char) + rsb;
      update_metrics (real_width, lsb, bitmap_char, tfm_char);
    }
}

/* Following routines are called by both `update_sidebearings' and
   `update_width'.  */

/* Try to make the definition of SB into a number, by looking up names
   in the symbol table.  */

static boolean
resolve_sidebearing (sidebearing_type *sb)
{
  boolean ok;
  symval_type sv;
  
  /* This should have already been checked.  */
  assert (sb != NULL);
  
  /* Make SB into a symval so we can resolve it.  */
  SYMVAL_TAG (sv) = SB_TAG (*sb);
  SYMVAL_REAL_STRING (sv) = SB_VALUE (*sb);
  
  ok = symval_resolve (&sv);
  
  if (ok)
    {
      SB_TAG (*sb) = symval_real;
      SB_REAL (*sb) = SYMVAL_REAL (sv);
    }
  else
    {
      string desc = symval_as_string (sv);
      WARNING1 ("Unresolvable sidebearing definition `%s'", desc);
      free (desc);
    }

  return ok;
}


/* Update the widths and side bearings in BITMAP_CHAR and TFM_CHAR
   according to REAL_WIDTH and LSB.  */

static void
update_metrics (real real_width, real lsb, char_info_type *bitmap_char,
                tfm_char_type *tfm_char)
{
  int pixel_width = ROUND (real_width);
  real point_width = PIXELS_TO_POINTS (real_width, dpi_real);
  real ds = get_designsize_in_points ();

  TFM_WIDTH (*tfm_char) = point_width;
  CHAR_TFM_WIDTH (*bitmap_char) = real_to_fix (point_width / ds);
  CHAR_SET_WIDTH (*bitmap_char) = pixel_width;
  CHAR_MIN_COL (*bitmap_char) = lsb;
  CHAR_MAX_COL (*bitmap_char) = lsb + CHAR_BITMAP_WIDTH (*bitmap_char);
}

/* Update the kern list in TFM_CHAR from any kerns in *C_PTR.  */

static void
update_kerns (char_type *c_ptr, tfm_char_type *tfm_char)
{
  unsigned k;
  list_type char_kerns = CHAR_KERNS (*c_ptr);
  
  for (k = 0; k < LIST_SIZE (char_kerns); k++)
    {
      char_kern_type *kern_elt = LIST_ELT (char_kerns, k);

      /* If the name of the kern character isn't in the global
         `encoding_info', skip it.  */ 
      int code = encoding_number (encoding_info, kern_elt->character);

      if (code != -1)
        {
          if (symval_resolve (&kern_elt->kern))
            {
              real pixel_kern = SYMVAL_REAL (kern_elt->kern);
              real points_kern = PIXELS_TO_POINTS (pixel_kern, dpi_real);
              
              tfm_set_kern (&TFM_KERN (*tfm_char), code, points_kern);
	    }
          else
            {
              string desc = symval_as_string (kern_elt->kern);
              WARNING2 ("%s: Unresolvable kern value `%s'",
 	                kern_elt->character, desc);
              free (desc);
	    }
	}
    }
}

/* We need to do this several times, so...  Have to use the global
   `dpi-real'.  */

real
get_designsize_in_points ()
{
  real ds_pixels = symtab_lookup_real ("designsize");
  real ds_points = PIXELS_TO_POINTS (ds_pixels, dpi_real);
  
  return ds_points;
}
