/* find-suffix.c: return a suffix.

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
find_suffix (string name)
{
  string dot_pos = strrchr (name, '.');
  string slash_pos = strrchr (name, '/');
  
  /* If the name is `foo' or `/foo.bar/baz', we have no extension.  */
  return
    dot_pos == NULL || dot_pos < slash_pos
    ? NULL
    : dot_pos + 1;
}
