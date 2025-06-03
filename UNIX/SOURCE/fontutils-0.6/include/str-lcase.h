/* str-lcase.h: convert a string to lowercase.

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

#ifndef STR_LCASE_H
#define STR_LCASE_H

#include "types.h"


/* Return a malloced copy of S with all its uppercase letters replaced
   with their lowercase counterparts.  */
extern string str_to_lower (string s);

#endif /* not CASE_H  */
