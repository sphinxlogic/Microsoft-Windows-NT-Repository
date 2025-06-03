/* psutil.c: utility routines for PostScript output.

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
#include "libfile.h"

#include "main.h"
#include "psutil.h"

extern string version_string;


/* Information for the PostScript FontInfo dictionary, etc.
   (-ps-font-info)  */
string ps_global_info = NULL;


/* This should be used for outputting a string S on a line by itself.  */
#define OUT_LINE(s) do { fprintf (f, "%s\n", s); } while (0)

/* These output their arguments, preceded by the indentation.  */
#define OUT1(s, e) do { fprintf (f, s, e); } while (0)
#define OUT2(s, e1, e2) do { fprintf (f, s, e1, e2); } while (0)

static string option_value (string, string);
static void out_notdef_loop (FILE *, charcode_type, charcode_type);

/* Return the information that goes in the FontInfo dictionary, etc.  We
   use the global variables `ps_global_info' and `tfm_global_info' to
   determine the information.  Either or both might be null.  */

ps_font_info_type
ps_set_font_info ()
{
  ps_font_info_type ret;

  /* The PostScript name of the font.  */
  ret.font_name = option_value (ps_global_info, "FontName") ? : "unknown";
  
  /* The typeface family which includes this font.  */
  ret.family_name = option_value (ps_global_info, "FamilyName");
  if (!ret.family_name)
    { /* Since they didn't specify a family name, try to guess from the
         font name.  The family name is `Times' for the font
         `Times-BoldItalic', but it's `Helvetica' for `Helvetica'.  (It
         should be `Lucida' for `LucidaBright-Italic', but we don't
         handle that case.)  */
      ret.family_name = strchr (ret.font_name, '-');
      if (!ret.family_name)
        ret.family_name = ret.font_name;  /* The `Helvetica' case.  */
      else
        ret.family_name
          = substring (ret.font_name, 0, ret.family_name - 1 - ret.font_name);
    }

  ret.weight = option_value (ps_global_info, "Weight");
  if (!ret.weight)
    { /* Again, since they didn't specify a weight, try to guess from
         the font name.  */
      unsigned w;
      string weights[]
        = { "Black", "Book", "Bold", "Demi", "ExtraBold", "Light",
            "Heavy", "Regular", "Semibold", "Ultra", NULL };
      
      for (w = 0; weights[w] != NULL && !strstr (ret.font_name, weights[w]);
           w++)
        ;
      ret.weight = weights[w] ? : "Medium";
    }

  /* We should be able to compute the italic angle by somehow looking at
     the characters.  bdftops.ps rotates the `I' and takes the angle
     that minimizes the width, for example.  */
  {
    string angle_str = option_value (ps_global_info, "ItalicAngle");
    ret.italic_angle = angle_str ? atof (angle_str) : 0;
  }
  
  /* The interword space must come from `tfm_global_info'.  Otherwise,
     we just let it be zero.  */
  ret.interword_space
    = tfm_global_info
      ? TFM_SAFE_FONTDIMEN (*tfm_global_info, TFM_SPACE_PARAMETER, 0.0)
      : 0.0;

  /* Monospaced fonts have no stretch or shrink in their interword
     space (or shouldn't), but they do have a nonzero interword space.
     (Except TeX math fonts have all their interword space parameters
     set to zero).  */
  {
    string monospace_str = option_value (ps_global_info, "isFixedPitch");
    if (monospace_str)
      ret.monospace_p = STREQ (monospace_str, "true"); 
    else if (tfm_global_info)
      ret.monospace_p
        =    TFM_SAFE_FONTDIMEN (*tfm_global_info, TFM_STRETCH_PARAMETER, -1)
             == 0.0
          && TFM_SAFE_FONTDIMEN (*tfm_global_info, TFM_SHRINK_PARAMETER, -1)
             == 0.0
          && TFM_SAFE_FONTDIMEN (*tfm_global_info, TFM_SPACE_PARAMETER, 0)
             > 0.0;
    else
      ret.monospace_p = false;
  }

  /* What might be a good way to compute this one?  */
  {
    string under_str = option_value (ps_global_info, "UnderlinePosition");
    ret.underline_position = under_str ? atof (under_str) : -100;
  }
  
  /* Here we use the rule thickness from the TFM file, if it's set.
     Otherwise, we should guess the dominant stem width in the font, but
     that's too much work for too little return.  */
  {
#define DEFAULT_THICKNESS 50
    string under_str = option_value (ps_global_info, "UnderlineThickness");
    if (under_str)
      ret.underline_thickness = atof (under_str);
    else if (tfm_global_info)
      ret.underline_thickness
        = TFM_SAFE_FONTDIMEN (*tfm_global_info,
                              TFM_DEFAULTRULETHICKNESS_PARAMETER,
                              DEFAULT_THICKNESS);
    else
      ret.underline_thickness = DEFAULT_THICKNESS;
  }

  /* What to do about the UniqueID's?  Actually, I'm not sure they
     should really be necessary.  Adobe wants people to register their
     fonts to get a UniqueID from them, which is semi-reasonable, but a
     lot of trouble.  We just omit them.  */
  {
    string unique_id_str = option_value (ps_global_info, "UniqueID");
    ret.unique_id = unique_id_str ? atou (unique_id_str) : 0;
    if (ret.unique_id >= 1 << 24)
      {
        WARNING1 ("%u: UniqueID >= 2**24; changing to zero", ret.unique_id);
        ret.unique_id = 0;
      }
  }
  
  /* If there is no version number in the TFM file, then just say it's
     version 0.  */
  {
    ret.version = option_value (ps_global_info, "version");
    if (!ret.version)
      ret.version
        = tfm_global_info
          && TFM_FONTDIMEN_COUNT (*tfm_global_info) >= TFM_VERSION_PARAMETER
          ? dtoa (TFM_FONTDIMEN (*tfm_global_info, TFM_VERSION_PARAMETER))
          : "0";
  }

  return ret;
}


/* Consider OPTION_STR as a list of <name>:<value> items separated by
   commas; return the <value> (as a string) associated with NAME, or
   NULL.  This could plausibly be put in `lib', but since we have one
   place (the routine just above) where we need to set each of a given
   set of names, as opposed to doing something with each specified
   item, we just define it here.  */

static string 
option_value (string option_str, string name)
{
  string item_end;
  string value = NULL;
  string item_start = option_str;

  if (option_str == NULL)
    return NULL;
    
  do
    {
      string item, colon;
      
      item_end = strchr (item_start, ',');
      item = item_end
             ? substring (item_start, 0, item_end - 1 - item_start)
             : item_start;
      colon = strchr (item, ':');
      
      if (colon)
        {
          string item_name = substring (item, 0, colon - 1 - item);
          
          if (STREQ (item_name, name))
            value = xstrdup (colon + 1);

          free (item_name);
        }
      else
        WARNING1 ("%s: Missing colon in option string", item);
      
      if (item != item_start)
        free (item);
      
      item_start = item_end + 1;
    }
  while (item_end);
  
  return value;
}

/* Output the common beginning (after the %! line) of a PostScript font
   to F, using the other args for information.  */

void
ps_start_font (FILE *f, ps_font_info_type ps_info, string comment)
{
  OUT1 ("%%%%Creator: %s\n", version_string);
  OUT1 ("%%%%CreationDate: %s\n", now ());
  OUT_LINE ("%%DocumentData: Clean7Bit");
  OUT_LINE ("%%EndComments");
  OUT1 ("%% %s\n", comment);
  OUT_LINE ("% This font is in the public domain.");

  OUT1 ("/%s 11 dict begin\n", ps_info.font_name);
  
  /* The FontInfo dictionary, which has additional
     (supposedly optional) information.  */
  OUT_LINE ("/FontInfo 8 dict begin");
  OUT1     ("  /version (%s) readonly def\n", ps_info.version);
  OUT1     ("  /FullName (%s) readonly def\n", ps_info.font_name);
  OUT1     ("  /FamilyName (%s) readonly def\n", ps_info.family_name);
  OUT1     ("  /Weight (%s) readonly def\n", ps_info.weight);
  OUT1     ("  /ItalicAngle %d def\n", ps_info.italic_angle);
  OUT1     ("  /isFixedPitch %s def\n",
                 ps_info.monospace_p ? "true" : "false");
  OUT1     ("  /underlinePosition %d def\n", ps_info.underline_position);
  OUT1     ("  /underlineThickness %d def\n", ps_info.underline_thickness);
  OUT_LINE ("currentdict end readonly def");
  
  /* Other constant top-level elements of the font dictionary.  Assume
     that the font name is the only thing on the stack at this point
     (which is true).  This saves us from having to write the name
     twice.  */
  OUT_LINE ("/FontName 1 index def");
  OUT_LINE ("/PaintType 0 def");  /* All our fonts are filled.  */

  if (ps_info.unique_id)
    OUT1 ("/UniqueID %d def\n", ps_info.unique_id);
}

/* Return the name corresponding to the number N in the current
   encoding.  If the number is not defined, give a warning and return
   ".notdef".  */

string
ps_encoding_name (charcode_type n)
{
  string ret = ENCODING_CHAR_NAME (encoding_info, n);
  if (ret == NULL)
    {
      WARNING2 ("%d: No such character number in encoding scheme `%s'",
                n, ENCODING_SCHEME_NAME (encoding_info));
      ret = ".notdef";
    }
  return ret;
}


/* Return the number corresponding to NAME in the current encoding.  If
   NAME is not defined, return -1.  */

int
ps_encoding_number (string name)
{
  int ret = encoding_number (encoding_info, name);
  return ret;
}


/* Output the encoding vector to the file F, based on CHAR_OUTPUT_P and
   the global `encoding'.  */

void
ps_output_encoding (FILE *f, boolean char_output_p[])
{
  unsigned c;
  int range_start = -1;

  fprintf (f, "/Encoding %d array %% %s\n", ENCODING_VECTOR_SIZE,
           ENCODING_SCHEME_NAME (encoding_info));

  for (c = 0; c < ENCODING_VECTOR_SIZE; c++)
    { /* Write sequences of .notdef's and characters we didn't output
         using a loop.  */
      if (!char_output_p[c]
          || ENCODING_CHAR_NAME (encoding_info, c) == NULL
	  || STREQ (ENCODING_CHAR_NAME (encoding_info, c), ".notdef"))
	{ /* Start or continue a loop.  */
	  if (range_start == -1)
            range_start = c;
	}
      else
	{ /* Finish a loop if one is started, then output the current
             character.  */
          if (range_start != -1)
            { /* Loop started, finish it.  */
              out_notdef_loop (f, range_start, c - 1);
              range_start = -1;
            }
          
          /* Output the current character.  */
          OUT2 ("  dup %d /%s put\n",c, ENCODING_CHAR_NAME (encoding_info, c));
	}
    }
  
  /* If the character 255 is not encoded, we'll be in the midst of a loop. */
  if (range_start != -1)
    out_notdef_loop (f, range_start, c - 1);
  
  /* Define it.  */
  OUT_LINE ("  readonly def");
}


/* Output a loop to put .notdef's into the encoding vector between START
   and END.  Assume the `Encoding' array is the top of the stack.  */

static void
out_notdef_loop (FILE *f, charcode_type start, charcode_type end)
{
  if (start == end)
    OUT1 ("  dup %d /.notdef put\n", start);
  else
    OUT2 ("  %d 1 %d { 1 index exch /.notdef put } bind for\n", start, end);
}
