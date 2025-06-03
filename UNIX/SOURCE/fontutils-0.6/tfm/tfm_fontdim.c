/* fontdimen.c: handle TFM fontdimens a.k.a. font parameters.

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

#include "tfm.h"


/* This structure maps fontdimen names to numbers (and vice versa).  */
typedef struct
{
  string name;
  unsigned number;
} fontdimen_type;

/* The list of fontdimen names and their corresponding numbers.  I wish
   I could figure out a good way to give this list only once (in tfm.h),
   instead of three times.  */

static fontdimen_type fontdimen[] = {
  { "slant",			TFM_SLANT_PARAMETER },
  { "space",			TFM_SPACE_PARAMETER },
  { "stretch",			TFM_STRETCH_PARAMETER },
  { "shrink",			TFM_SHRINK_PARAMETER },
  { "xheight",			TFM_XHEIGHT_PARAMETER },
  { "quad",			TFM_QUAD_PARAMETER },
  { "extraspace",		TFM_EXTRASPACE_PARAMETER },
  { "num1",			TFM_NUM1_PARAMETER },
  { "num2",			TFM_NUM2_PARAMETER },
  { "num3",			TFM_NUM3_PARAMETER },
  { "denom1",			TFM_DENOM1_PARAMETER },
  { "denom2",			TFM_DENOM2_PARAMETER },
  { "sup1",			TFM_SUP1_PARAMETER },
  { "sup2",			TFM_SUP2_PARAMETER },
  { "sup3",			TFM_SUP3_PARAMETER },
  { "sub1",			TFM_SUB1_PARAMETER },
  { "sub2",			TFM_SUB2_PARAMETER },
  { "supdrop",			TFM_SUPDROP_PARAMETER },
  { "subdrop",			TFM_SUBDROP_PARAMETER },
  { "delim1",			TFM_DELIM1_PARAMETER },
  { "delim2",			TFM_DELIM2_PARAMETER },
  { "axisheight",		TFM_AXISHEIGHT_PARAMETER },
  { "defaultrulethickness",	TFM_DEFAULTRULETHICKNESS_PARAMETER },
  { "bigopspacing1",		TFM_BIGOPSPACING1_PARAMETER },
  { "bigopspacing2",		TFM_BIGOPSPACING2_PARAMETER },
  { "bigopspacing3",		TFM_BIGOPSPACING3_PARAMETER },
  { "bigopspacing4",		TFM_BIGOPSPACING4_PARAMETER },
  { "bigopspacing5",		TFM_BIGOPSPACING5_PARAMETER },
  { "leadingheight",		TFM_LEADINGHEIGHT_PARAMETER },
  { "leadingdepth",		TFM_LEADINGDEPTH_PARAMETER },
  { "fontsize",			TFM_FONTSIZE_PARAMETER },
  { "version",			TFM_VERSION_PARAMETER },
};

/* How many entries we have in the `fontdimen' array.  */
static unsigned fontdimen_array_size
  = sizeof (fontdimen) / sizeof (fontdimen_type);

/* Set the font parameter entries in TFM_INFO according to the string S.
   If S is non-null and non-empty, it should look like
   <fontdimen>:<real>,<fontdimen>:<real>,..., where each <fontdimen> is
   either a standard name (in the list above) or a number between 1 and
   TFM_MAX_FONTDIMENS (which is currently 30, the default value in
   PLtoTF).  */
   
void
tfm_set_fontdimens (string s, tfm_global_info_type *tfm_info)
{
  string spec;
  
  /* If S is empty, we have nothing more to do.  */
  if (s == NULL || *s == 0)
    return;
  
  /* Parse the specification string.  */
  for (spec = strtok (s, ","); spec != NULL; spec = strtok (NULL, ","))
    {
      string fontdimen;
      real value;
      unsigned param_number = 0;
      string value_string = strchr (spec, ':');
      
      if (value_string == NULL || !float_ok (value_string))
        FATAL1 ("Fontdimens look like `<fontdimen>:<real>', not `%s'",
                spec);

      value = atof (value_string + 1);
      fontdimen = substring (spec, 0, value_string - spec - 1);
      
      /* If `fontdimen' is all numerals, we'll take it to be an integer.  */
      if (strspn (fontdimen, "0123456789") == strlen (fontdimen))
        {
          param_number = atou (fontdimen);

          if (param_number == 0 || param_number > TFM_MAX_FONTDIMENS)
            FATAL2 ("Font parameter %u is not between 1 and the maximum (%u)",
                    param_number, TFM_MAX_FONTDIMENS);
        }
      else
        { /* It wasn't a number; see if it's a valid name.  */
	  param_number = tfm_fontdimen_number (fontdimen);

	  if (param_number == 0)
            FATAL1 ("I don't know the font parameter name `%s'", fontdimen);
        }
      
      /* If we got here, `param_number' is the number of the font
         parameter we are supposed to assign to.  */
      tfm_set_fontdimen (tfm_info, param_number, value);
    }
}

/* Return zero if we do not recognize S as the name of a fontdimen, else
   its corresponding number.  We just do a linear search through the
   structure, since it's so small.  */

unsigned
tfm_fontdimen_number (string s)
{
  unsigned i; /* Index into the `fontdimen' array.  */
  unsigned param_number = 0;

  for (i = 0; i < fontdimen_array_size && param_number == 0; i++)
    if (STREQ (s, fontdimen[i].name))
      param_number = fontdimen[i].number;

  return param_number;
}


/* Return the name in `fontdimens' corresponding to the number N.  */

string
tfm_fontdimen_name (unsigned n)
{
  unsigned i;
  string name = NULL;
  
  for (i = 0; i < fontdimen_array_size && name == NULL; i++)
    {
      if (fontdimen[i].number == n)
        name = fontdimen[i].name;
    }
  
  return name;
}

/* Set the PARAMETER-th font parameter of TFM_INFO to VALUE.  If
   PARAMETER is beyond the current last parameter of TFM_INFO, set
   all the intervening parameters to zero.  */

void
tfm_set_fontdimen (tfm_global_info_type *tfm_info,
                   unsigned parameter, real value)
{
  if (TFM_FONTDIMEN_COUNT (*tfm_info) < parameter)
    {
      unsigned p;
      
      for (p = TFM_FONTDIMEN_COUNT (*tfm_info) + 1; p < parameter; p++)
        TFM_FONTDIMEN (*tfm_info, p) = 0.0;
      
      /* Now we have more parameters.  */
      TFM_FONTDIMEN_COUNT (*tfm_info) = parameter;
    }

  TFM_FONTDIMEN (*tfm_info, parameter) = value;
}


/* Set the `FONTSIZE' parameter of TFM_INFO, if the design size is set.  */

void
tfm_set_fontsize (tfm_global_info_type *tfm_info)
{
  if (TFM_DESIGN_SIZE (*tfm_info) != 0.0)
    tfm_set_fontdimen (tfm_info,
                       TFM_FONTSIZE_PARAMETER, TFM_DESIGN_SIZE (*tfm_info));
}
