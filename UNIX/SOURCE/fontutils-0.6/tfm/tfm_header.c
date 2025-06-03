/* tfm_header.c: deal with the TFM header bytes.

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


/* Set the header in TFM_INFO according to the string S.  */
   
void
tfm_set_header (string s, tfm_global_info_type *tfm_info)
{
  string spec;

  /* If S is empty, we have nothing more to do.  */
  if (s == NULL || *s == 0)
    return;
  
  /* Parse the specification string.  */
  for (spec = strtok (s, ","); spec != NULL; spec = strtok (NULL, ","))
    {
      string header_item;
      string value_string = strchr (spec, ':');
      
      if (value_string == NULL)
        FATAL1 ("TFM headers look like `<header-item>:<value>', not `%s'",
	         spec);

      header_item = substring (spec, 0, value_string - spec - 1);

      /* Advance past the `:'.  */
      value_string++;
      
      if (STREQ (header_item, "checksum"))
        {
          if (!integer_ok (value_string))
            FATAL1 ("%s: Invalid integer constant (for checksum)",
                    value_string);
  	  TFM_CHECKSUM (*tfm_info) = atoi (value_string);
	}

      else if (STREQ (header_item, "designsize"))
	{
          if (!float_ok (value_string))
            FATAL1 ("%s: Invalid floating-point constant (for designsize)",
		    value_string);
          tfm_set_design_size (atof (value_string), tfm_info);
        }

      else if (STREQ (header_item, "codingscheme"))
	{
          unsigned length = strlen (value_string);
	  
          if (strchr (value_string, '(') != NULL
	      || strchr (value_string, ')') != NULL)
            FATAL1 ("Your coding scheme `%s' may not contain parentheses",
            	    value_string);
        
          if (length > TFM_MAX_CODINGSCHEME_LENGTH)
            WARNING3 ("Your coding scheme `%s' of length %d will be \
truncated to %d", value_string, length, TFM_MAX_CODINGSCHEME_LENGTH);
	  
	  TFM_CODING_SCHEME (*tfm_info) = xstrdup (value_string);
        }
    }
}

/* Set the design and font size of TFM_INFO to DESIGN_SIZE.  */

void
tfm_set_design_size (real design_size, tfm_global_info_type *tfm_info)
{
  TFM_CHECK_DESIGN_SIZE (design_size);
  TFM_DESIGN_SIZE (*tfm_info) = design_size;
  tfm_set_fontsize (tfm_info);
}
