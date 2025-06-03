/* kern.c: kerns in the CMI file.

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

#include "kern.h"


/* This is taken from `tfm_set_kern', except RIGHT is a character name,
   instead of a character code, and the value of the kern K is a
   `symval_type' (since it is useful, as well as more consistent, to
   define kerns like side bearings) instead of a real.  */

void
char_set_kern (list_type *kern_list, string right, symval_type k)
{
  unsigned this_right;
  char_kern_type *new_kern;
  
  assert (kern_list != NULL);
  
  for (this_right = 0; this_right < LIST_SIZE (*kern_list); this_right++)
    {
      char_kern_type *kern = LIST_ELT (*kern_list, this_right);

      if (STREQ (kern->character, right))
	{ /* Already there, just replace the value.  */
	  kern->kern = k;
	  return;
	}
    }

  /* RIGHT wasn't in the existing list.  Add it to the end.  */
  new_kern = LIST_TAPPEND (kern_list, char_kern_type);
  new_kern->character = right;
  new_kern->kern = k;
}
