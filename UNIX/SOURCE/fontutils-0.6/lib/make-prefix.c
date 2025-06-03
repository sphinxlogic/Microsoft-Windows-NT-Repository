/* make-prefix.c: construct a pathname prefix.

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
make_prefix (string prefix, string pathname)
{
  string slash_pos = strrchr (pathname, '/');
  
  if (slash_pos == NULL)
    return concat (prefix, pathname);
  else
    {
      pathname[slash_pos - pathname] = 0;
      return concat4 (pathname, "/", prefix, slash_pos + 1);
    }
}
