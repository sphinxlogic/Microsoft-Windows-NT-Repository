/* output-tfm.c: write a TFM file.

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

#include "filename.h"
#include "random.h"
#include "report.h"
#include "tfm.h"

#include "filter.h"
#include "main.h"
#include "output-tfm.h"


/* User assignments to the TFM character header values.  */
string tfm_header = NULL;

/* User assignments to the TFM fontdimen values.  */
string fontdimens = NULL;


/* `false' if we're doing any font other than the main one (which is
    done first).  */
static boolean main_input_font_p = true;

/* `true' if we have an input TFM file from which to get defaults.  */
static boolean have_file_info = false;

/* This accumulates the information about each character.  */
static tfm_char_type *tfm_chars;

/* This holds the information about each character old TFM character.  */
static tfm_char_type *old_tfm_chars;

/* The name of the TFM file we finally write.  */
static string output_tfm_name;


static void tfm_do_chars_defaults (void);

/* At the beginning, we can set up the output file and write the fontwide
   information.  This includes the fontdimen parameters (in the global
   `fontdimens'), and the header bytes (in `tfm_header'), if the user
   specified them.  */
   
void
tfm_start_output (string input_name, string output_name,
		  real user_design_size, real default_design_size)
{
  tfm_global_info_type tfm_info;
  string input_tfm_name = find_tfm_filename (input_name);
  
  /* We remove the suffix before making the PL name because if the user
     said `-output-file=foo.bar', it's the TFM file which should be
     named `foo.bar'; the PL file is still named `foo.pl'.  */
  string output_root = remove_suffix (output_name);
  string pl_name = extend_filename (output_root, "pl");
  output_tfm_name = extend_filename (output_name, "tfm");

  /* If we'd overwrite the input, change the name.  */
  if (input_tfm_name != NULL && same_file_p (input_tfm_name, output_tfm_name))
    {
      pl_name = make_prefix ("x", pl_name);
      output_tfm_name = make_prefix ("x", output_tfm_name);
    }
    
  if (!tfm_open_pl_output_file (pl_name))
    FATAL_PERROR (pl_name);
  
  /* Initialize `tfm_info' with the data from an existing file, if we
     have it.
     
     For the design size in the output TFM file, use the following, in
     order of preference:
     1) `designsize' parameter given by the user in the -tfm-header option;
     2) `-design-size' option (which is `user_design_size');
     3) the designsize from an existing TFM file;
     4) the `default_design_size' passed in (which is the designsize in
        the input bitmap font).  */

  if (input_tfm_name != NULL && tfm_open_input_file (input_tfm_name))
    {
      tfm_info = tfm_get_global_info ();
      have_file_info = true;

      REPORT1 ("Reading %s.\n", input_tfm_name);
      
      /* Never carry along the checksum.  */
      TFM_CHECKSUM (tfm_info) = 0;
      
      /* A user-supplied design size (#2) overrides the design size from
         the file (#3).  (The -tfm-header (#1) override is below.)  */
      if (user_design_size != 0.0)
        TFM_DESIGN_SIZE (tfm_info) = user_design_size;
    }
  else
    { /* No existing TFM file.  */
      tfm_info = tfm_init_global_info ();
      
      /* Use the bitmap font's design size (#4).  */
      TFM_DESIGN_SIZE (tfm_info) = default_design_size;
    }
  
  /* Be sure the design size in the header bytes and the fontdimen
     parameter agree.  */
  tfm_set_fontsize (&tfm_info);

  /* Update `tfm_info' with any user-supplied strings.  */
  if (tfm_header != NULL)
    tfm_set_header (tfm_header, &tfm_info);

  if (fontdimens != NULL)
    tfm_set_fontdimens (fontdimens, &tfm_info);
  
  /* Output this to start us off.  */
  tfm_put_global_info (tfm_info);

  /* Initialize the array where we will store the character information.  */
  tfm_chars = tfm_new_chars ();
}

/* We could do something more complicated here, if we wanted to get the
   TFM information for each input font: close the first font (after
   first getting everything we need), open the next one, then close it
   next time we're called, etc.  (Or make the TFM library able to handle
   more than one open font.)  Then we could combine all the lig/kern
   information, besides using the character dimensions.  But none of
   this seems worth it (i.e., we haven't needed to do it yet).  
   
   So instead we just remember whether we're doing the first font --
   which is the main one -- so we can skip looking in the TFM file for
   font #1 when we're doing fonts #2, #3, ...  */

void
tfm_start_font (string font_name)
{
  if (main_input_font_p)
    main_input_font_p = false;
}

/* Store the relevant information about the character C into `tfm_chars'.  */

void
tfm_output_char (char_info_type c, real dpi_real)
{
  tfm_char_type *new_tfm_char = &tfm_chars[CHARCODE (c)];
  
  TFM_CHAR_EXISTS (*new_tfm_char) = true;
  TFM_CHARCODE (*new_tfm_char) = CHARCODE (c);
  
  /* If we're doing radical things to the characters, don't use the
     default information from an existing file even if we have one.  */
  if (baseline_adjust[CHARCODE (c)] != 0
      || column_split[CHARCODE (c)] != NULL
      || filter_passes > 0
      || random_max > 0.0
      || !have_file_info
      || !main_input_font_p)
    {
      TFM_WIDTH (*new_tfm_char)
        = PIXELS_TO_POINTS (CHAR_SET_WIDTH (c), dpi_real);
  
      TFM_HEIGHT (*new_tfm_char) 
        = PIXELS_TO_POINTS (CHAR_HEIGHT (c), dpi_real);
  
      TFM_DEPTH (*new_tfm_char) = PIXELS_TO_POINTS (CHAR_DEPTH (c), dpi_real);
      TFM_ITALIC_CORRECTION (*new_tfm_char) = 0.0;
    }
   else
    {
      tfm_char_type *old_tfm_char = tfm_get_char (CHARCODE (c));

      TFM_WIDTH (*new_tfm_char) = TFM_WIDTH (*old_tfm_char);
      TFM_HEIGHT (*new_tfm_char) = TFM_HEIGHT (*old_tfm_char);
      TFM_DEPTH (*new_tfm_char) = TFM_DEPTH (*old_tfm_char);
      TFM_ITALIC_CORRECTION (*new_tfm_char) 
	= TFM_ITALIC_CORRECTION (*old_tfm_char);
    }

  /* We do the ligatures and kerns when we have gone through all the
     characters. */
}


/* If we have an existing TFM file, use it to get the lig/kern info.  */

static void
tfm_do_chars_defaults ()
{
  unsigned code;

  if (!have_file_info)
    return;

  old_tfm_chars = tfm_get_chars ();
  
  for (code = 0; code < TFM_SIZE; code++)
    {
      tfm_char_type *new_tfm_char = &tfm_chars[code];
      
      if (TFM_CHAR_EXISTS (*new_tfm_char))
        {
          unsigned this_one;
	  tfm_char_type *old_tfm_char = &old_tfm_chars[code];
	  list_type old_kern_table = TFM_KERN (*old_tfm_char);
	  list_type old_lig_table = TFM_LIGATURE (*old_tfm_char);
          
          for (this_one = 0; this_one < LIST_SIZE (old_kern_table); this_one++)
            {
              tfm_kern_type *old_kern = LIST_ELT (old_kern_table, this_one);
	      tfm_char_type *old_kern_char
                = &tfm_chars[translate[old_kern->character]];
	      
              if (TFM_CHAR_EXISTS (*old_kern_char))
	        tfm_set_kern (&TFM_KERN (*new_tfm_char),
  			      CHARCODE (*old_kern_char), old_kern->kern);
            }

          for (this_one = 0; this_one < LIST_SIZE (old_lig_table); this_one++)
	    {
              tfm_ligature_type *old_lig = LIST_ELT (old_lig_table, this_one);
              tfm_char_type *old_lig_char
                = &tfm_chars[translate[old_lig->character]];
              tfm_char_type *old_lig_ligature
                = &tfm_chars[translate[old_lig->ligature]];
	      
              if (TFM_CHAR_EXISTS (*old_lig_char) 
                  && TFM_CHAR_EXISTS (*old_lig_ligature))
	        tfm_set_ligature (&TFM_LIGATURE (*new_tfm_char),
                                  CHARCODE (*old_lig_char),
				  CHARCODE (*old_lig_ligature));
            }
        } /* new TFM char exists */
    }
}

/* Output `tfm_chars' and close up.  */

void
tfm_finish_output ()
{
  tfm_do_chars_defaults ();

  tfm_put_chars (tfm_chars);
  tfm_convert_pl (output_tfm_name, verbose);
  tfm_close_pl_output_file ();

  if (have_file_info)
    tfm_close_input_file ();
}
