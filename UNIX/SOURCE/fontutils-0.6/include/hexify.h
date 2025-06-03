/* hexify.h: change strings to ASCII hex characters.

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

#ifndef HEXIFY_H
#define HEXIFY_H

#include "types.h"

/* Convert the LENGTH bytes starting at DATA to a string in ASCII
   hexadecimal, i.e., a string consisting entirely of the characters 0-9
   and a-f, and terminated with a null.  The result is allocated with
   malloc.  */
extern string hexify (one_byte *data, unsigned length);

#endif /* not HEXIFY_H */

