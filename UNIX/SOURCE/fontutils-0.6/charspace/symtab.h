/* symtab.h: declarations for our symbol table manipulations.

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

#ifndef SYMTAB_H
#define SYMTAB_H

#include "char.h"
#include "types.h"
#include "realstrval.h"


/* What a single symbol looks like.  */

typedef struct
{
  symval_tag_type tag;
  
  union
  {
    real_string_val_type real_string_val;
    char_type char_val;
  } value;
} symval_type;

/* Accessor macros.  */
#define SYMVAL_TAG(s)	((s).tag)
#define SYMVAL_CHAR(s)	((s).value.char_val)
#define SYMVAL_REAL(s)	((s).value.real_string_val.real_val)
#define SYMVAL_REAL_STRING(s) ((s).value.real_string_val)
#define SYMVAL_STRING(s)((s).value.real_string_val.string_val)


/* Create new value nodes of the various types.  */
extern symval_type symtab_char_node (symval_type lsb, symval_type rsb);
extern symval_type symtab_real_node (real);
extern symval_type symtab_real_string_node (real, string);
extern symval_type symtab_string_node (string);

/* Define the identifier KEY to be the value V.  Overwrite any
   previous definition of KEY.  */
extern void symtab_define (string key, symval_type v);

/* Define a kern K between the characters named LEFT and RIGHT.  */
extern void symtab_define_kern (string left, string right, symval_type k);


/* Return the value of KEY, or NULL.  */
extern symval_type *symtab_lookup (string key);

/* Resolve KEY to a real if possible and return the result.  If it
   cannot be resolved, give a fatal error.  */
extern real symtab_lookup_real (string key);


/* Resolve the definition of SV to a real, if possible.  Return success.  */
extern boolean symval_resolve (symval_type *sv);


/* Return a string describing SV (e.g., for error messages).  The string
   is allocated with malloc.  */
extern string symval_as_string (symval_type sv);

#endif /* not SYMTAB_H */
