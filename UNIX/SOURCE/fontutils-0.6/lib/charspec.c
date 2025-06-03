/* charspec.c: parse a character code or name.

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
#include "charspec.h"
#include "encoding.h"


charcode_type
xparse_charspec (string spec, encoding_info_type *enc)
{
  int code;
  
  /* It's an error to call this with SPEC == NULL or the empty string.  */
  assert (spec != NULL && *spec);

  if (isdigit (*spec))
    code = xparse_charcode (spec);

  else if (enc == NULL)
    {
      /* If ENC is null, and SPEC is longer than a single character, we
         don't know what to do.  */
      if (spec[1] != 0)
	FATAL1 ("%s: Unparseable character specification", spec);
      else
        code = *spec;
    }

  else
    {
      int code = encoding_number (*enc, spec);

      /* If SPEC is not in the encoding, and it's one character long,
         just use its value as a C integer.  */
      if (code == -1)
        {
          if (spec[1] == 0)
            code = spec[0];
          else
            FATAL1 ("%s: Undefined character name", spec);
	}
    }
  
  return (charcode_type) code;
}
