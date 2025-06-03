/* charspec.h: 

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

#ifndef CHARSPEC_H
#define CHARSPEC_H

#include "encoding.h"
#include "types.h"

/* If SPEC starts with a digit, return the result of `xparse_charcode
   (SPEC)'.  Otherwise, if ENC is NULL and SPEC is exactly one character
   long, return that character.  Otherwise (ENC is non-NULL), look up
   SPEC as a character name in ENC and return the corresponding character
   code.  If SPEC is NULL, the empty string, an unrecognized name, or
   otherwise invalid, give a fatal error.  */
extern charcode_type xparse_charspec (string spec, encoding_info_type *enc);

#endif /* not CHARSPEC_H */
