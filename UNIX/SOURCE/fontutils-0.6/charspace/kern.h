/* kern.h: handle kerns in the CMI file.

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

#ifndef KERN_H
#define KERN_H

#include "symtab.h"
#include "types.h"

/* The kern list.  This differs from `tfm_kern_type' in that it
   uses character names instead of (integer) codes, and uses
   `symval_type' for the value.  Thus, the user can symbolically define
   kerns in the same way as sidebearings.  */

typedef struct
{
  string character;
  symval_type kern;
} char_kern_type;


/* Make the kern for the character named RIGHT in the list of
   `char_kern_type's KERN_LIST be K, replacing any kern already present.  */
extern void char_set_kern (list_type *kern_list, string right, symval_type k);

#endif /* not KERN_H */
