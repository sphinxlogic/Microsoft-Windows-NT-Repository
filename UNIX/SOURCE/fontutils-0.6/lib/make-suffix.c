/* make-suffix.c: unconditionally change the suffix on a string.

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

string
make_suffix (string s, string new_suffix)
{
  string new_s;
  string old_suffix = find_suffix (s);

  if (old_suffix == NULL)
    new_s = concat3 (s, ".", new_suffix);
  else
    {
      unsigned length_through_dot = old_suffix - s;
      
      new_s = xmalloc (length_through_dot + strlen (new_suffix) + 1);
      strncpy (new_s, s, length_through_dot);
      strcpy (new_s + length_through_dot, new_suffix);
    }

  return new_s;
}
