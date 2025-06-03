/* input-ccc.c: control the CCC parsing by Bison.

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

#include "char.h"
#include "main.h"

#include "input-ccc.h"


/* The main input file and its name.  */
FILE *ccc_file;
string ccc_filename;

/* The information we've collected about the characters.  */
bzr_char_type **chars;

/* The information for resolving various CCC units of measure.  */
ccc_fontinfo_type ccc_fontinfo;

/* Set as we parse.  */
ccc_type *ccc_chars[MAX_CHARCODE + 1];

/* Go through all of CHARS, setting the various components in the return
   struct.  Some defaults are taken from TFM_INFO (if it's non-null),
   and the global `encoding_info'.  */

static ccc_fontinfo_type
get_ccc_fontinfo (real design_size, bzr_char_type *chars[],
                  tfm_global_info_type *tfm_info)
{
  ccc_fontinfo_type ccc_info; /* What we'll return.  */
  unsigned this_char;
  int font_height;
  boolean cap_height_found = false;
  boolean xheight_found = false;

  /* The designsize is always present.  */
  ccc_info.design_size = design_size;
  
  /* If the `quad' parameter is not present in TFM_INFO to define the
     font's em space, use the design size.  */
  ccc_info.em
    = tfm_info
      ? TFM_SAFE_FONTDIMEN (*tfm_info, TFM_QUAD_PARAMETER, design_size)
      : design_size;

  /* If the `xheight' parameter is present in TFM_INFO, use it.  */
  if (tfm_info && TFM_XHEIGHT_PARAMETER <= TFM_FONTDIMEN_COUNT (*tfm_info))
    {
      ccc_info.xheight = TFM_FONTDIMEN (*tfm_info, TFM_XHEIGHT_PARAMETER);
      xheight_found = true;
    }

  /* For the cap_height and font depth (and maybe for the xheight) we
     must go through all the characters, so initialize.  */
  ccc_info.cap_height = ccc_info.font_depth = font_height = INT_MIN;

  for (this_char = 0; this_char < MAX_CHARCODE; this_char++)
    {
      if (chars[this_char])
        {
          bzr_char_type c = *chars[this_char];
          int char_height = CHAR_HEIGHT (c);
          int char_depth = CHAR_DEPTH (c);
          string char_name = ENCODING_CHAR_NAME (encoding_info, this_char);

          if (char_height > font_height) 
            font_height = char_height;

          if (char_depth > ccc_info.font_depth) 
            ccc_info.font_depth = char_depth;

          /* If it's a letter from which we can find the font's
             cap_height, use it.  Don't use `F' et al. because they
             might have serifs that go above the height of the stem.  */
          if (!cap_height_found && char_name[1] == 0
              && (*char_name == 'B'
                  || *char_name == 'D'
                  || *char_name == 'H'
                  || *char_name == 'P'
                  || *char_name == 'R'
                  || *char_name == 'X'))
            {
              ccc_info.cap_height = char_height;
              cap_height_found = true;
            }

          /* If we haven't found the x-height yet, and `this_char' is a
             letter from which we can determined it, use it.  */
          if (!xheight_found && char_name[1] == 0
              && (*char_name == 'u'
                  || *char_name == 'v'
                  || *char_name == 'w'
                  || *char_name == 'x'
                  || *char_name == 'y'
                  || *char_name == 'z'))
            {
              ccc_info.xheight = char_height;
              xheight_found = true;
            }
        }
    }
  
  /* Having gone through all the characters, if we haven't been able to
     determine the cap height, use the maximum font height.  */
  if (!cap_height_found)
    ccc_info.cap_height = font_height;
  
  /* Similarly for the x-height.  */
  if (!xheight_found)
    ccc_info.xheight = .6 * font_height;

  return ccc_info;
}

/* If the CCC file `IN_FILENAME' (extended with `.ccc' if necessary)
   exists, read it, updating CHARS_ARG.  Default information for some
   CCC dimensions comes from TFM_INFO.
   
   If `IN_FILENAME' isn't readable, complain unless WARNING_P is false.  */

ccc_type **
parse_ccc_file (string in_filename, bzr_char_type *chars_arg[],
		tfm_global_info_type *tfm_info, real design_size,
                boolean warning_p)
{
  string ccc_name = extend_filename (in_filename, "ccc");

  ccc_file = fopen (ccc_name, "r");

  if (ccc_file == NULL)
    {
      if (warning_p)
        perror (ccc_name);
    }
  else
    {
      extern int yyparse ();
      
      /* We have to use these globals, since we can't alter `yyparse's
         signature.  */
      ccc_fontinfo = get_ccc_fontinfo (design_size, chars_arg, tfm_info);
      ccc_filename = ccc_name;
      chars = chars_arg;
      
      /* Parse the CCC source file.  */
      yyparse ();

      xfclose (ccc_file, ccc_name);
    }

  return ccc_chars;
}
