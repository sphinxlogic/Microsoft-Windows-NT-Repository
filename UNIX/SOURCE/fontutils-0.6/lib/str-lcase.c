/* str-casefold.c: make a string either all uppercase or all lowercase.

Copyright (C) 1992 Free Software Foundation, Inc.
This file was part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#include "config.h"
#include <ctype.h>

#include "str-lcase.h"


/* Return a malloced copy of S with all its uppercase letters replaced
   with their lowercase counterparts.  S must not be NULL.  */
   
string
str_to_lower (string s)
{
  unsigned c;
  string lc;
  unsigned length;
  
  assert (s != NULL);

  length = strlen (s);
  lc = xmalloc (length + 1);
  
  for (c = 0; c < length; c++)
    lc[c] = isupper (s[c]) ? tolower (s[c]) : s[c];
    
  lc[length] = 0;
  
  return lc;
  
}
