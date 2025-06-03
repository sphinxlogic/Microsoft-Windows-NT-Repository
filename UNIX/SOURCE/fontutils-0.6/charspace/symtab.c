/* symtab.c: create and update a symbol table.  We use a simple linear
   representation, since there will be a few hundred entries at
   the very most.
   
   The numbers which get stored in the symbol table should all be in
   pixels.  That's what the rest of the program expects.  (Unfortunately
   there's no way to check this at the time of definition.)

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
#include "main.h"
#include "symtab.h"


/* Every identifier in the CMI files -- character names and fontdimen
   names as well as `define'd quantities -- gets put into our symbol
   table.  We do not resolve any definitions until they are actually
   needed, so that later definitions will replace earlier ones.  */
struct symbol
{
  string key;
  symval_type value;
  struct symbol *next;
};

/* These all work on pointers instead of structures, since that's what
   we need to pass around in this file.  */
#define SYM_KEY(s)	((s)->key)
#define SYM_NEXT(s)	((s)->next)
#define SYM_VAL(s)	((s)->value)

typedef struct symbol symbol_type;


/* We have only one symbol table.  Here it is.  */
static symbol_type *symbol_table = NULL;


static symbol_type *symtab_find (string);
static boolean resolve_string (symval_type *, real);

/* Routines to create symval nodes.  */

symval_type
symtab_char_node (symval_type lsb, symval_type rsb)
{
  symval_type sv;
  
  SYMVAL_TAG (sv) = symval_char;
  SYMVAL_CHAR (sv) = init_char ();

  CHAR_LSB (SYMVAL_CHAR (sv)) = XTALLOC1 (sidebearing_type);
  CHAR_LSB_TAG (SYMVAL_CHAR (sv)) = SYMVAL_TAG (lsb);
  CHAR_LSB_VALUE (SYMVAL_CHAR (sv)) = SYMVAL_REAL_STRING (lsb);

  CHAR_RSB (SYMVAL_CHAR (sv)) = XTALLOC1 (sidebearing_type);
  CHAR_RSB_TAG (SYMVAL_CHAR (sv)) = SYMVAL_TAG (rsb);
  CHAR_RSB_VALUE (SYMVAL_CHAR (sv)) = SYMVAL_REAL_STRING (rsb);
  
  return sv;
}


symval_type
symtab_real_node (real r)
{
  symval_type sv;
  
  SYMVAL_TAG (sv) = symval_real;
  SYMVAL_REAL (sv) = r;

  return sv;
}


symval_type
symtab_real_string_node (real r, string s)
{
  symval_type sv;
  
  SYMVAL_TAG (sv) = symval_real_string;
  SYMVAL_REAL (sv) = r;
  SYMVAL_STRING (sv) = s;

  return sv;
}


symval_type
symtab_string_node (string s)
{
  symval_type sv;
  
  SYMVAL_TAG (sv) = symval_string;
  SYMVAL_STRING (sv) = s;

  return sv;
}

/* Define the identifier KEY to be the value V.  We do no checking on V,
   we just plop it in.  Overwrite any previous definition of KEY.  If
   KEY was not previously defined, we malloc a new symbol.  */

void 
symtab_define (string key, symval_type v)
{
  symbol_type *s = symtab_find (key);

  if (s == NULL)
    {
      s = XTALLOC1 (symbol_type);
      
      /* Make a private copy of KEY so outside assignments or
         deallocations don't affect the symbol table.  */
      SYM_KEY (s) = xstrdup (key);
      SYM_NEXT (s) = symbol_table;
      symbol_table = s;
    }

  /* Whether we just created it or not, put in the new value.  */
  SYM_VAL (s) = v;
}

/* Define a kern of K pixels between the characters named LEFT and
   RIGHT.  If no kern for that pair previously existed, we create it.
   If the character LEFT was not previously in the symbol table, we add
   it.  But if LEFT was in the symbol table, we have to preserve any
   kerns or side bearing information that have already been stored.  */

void
symtab_define_kern (string left, string right, symval_type k)
{
  symval_type sv;
  symbol_type *s = symtab_find (left);
  
  if (s == NULL || SYMVAL_TAG (SYM_VAL (s)) != symval_char)
    {
      list_type *kern_list_ptr;
      char_kern_type *new_kern;
      
      SYMVAL_TAG (sv) = symval_char;
      SYMVAL_CHAR (sv) = init_char ();
      
      kern_list_ptr = &CHAR_KERNS (SYMVAL_CHAR (sv));
      new_kern = LIST_TAPPEND (kern_list_ptr, char_kern_type);
      new_kern->character = right;
      new_kern->kern = k;
    }
  else
    {
      sv = SYM_VAL (s);
      char_set_kern (&CHAR_KERNS (SYMVAL_CHAR (sv)), right, k);
    }
  
  symtab_define (left, sv);
}

/* Look up KEY in `symbol_table'.  If found, return a pointer to the
   corresponding symval, else NULL.  */

symval_type *
symtab_lookup (string key)
{
  symbol_type *s = symtab_find (key);
  symval_type *sv = s ? &SYM_VAL (s) : NULL;
  
  return sv;
}


/* Look up KEY in `symbol_table', resolve its definition to a real, and
   return the result.  If the value cannot be resolved, or if KEY isn't
   defined, give a fatal error.  */

real
symtab_lookup_real (string key)
{
  real r;
  symval_type *sv = symtab_lookup (key);
  
  if (sv == NULL)
    FATAL1 ("%s: Undefined symbol", key);

  if (symval_resolve (sv))
    r = SYMVAL_REAL (*sv);
  else
    FATAL1 ("%s: Cannot be resolved to a real", key);

  return r;
}


/* Look up KEY in `symbol_table'.  If found, return the containing
   symbol, otherwise NULL.  */

static symbol_type *
symtab_find (string key)
{
  symbol_type *s;

  assert (key != NULL);

  for (s = symbol_table; s != NULL && !STREQ (SYM_KEY (s), key);
       s = SYM_NEXT (s))
    ;

  return s;
}

/* Resolve (to a real) the symbol value SV.  Change SV if we succeed.
   Return success.  */

boolean
symval_resolve (symval_type *sv)
{
  boolean ok;
  
  if (sv == NULL)
    return false;
    
  switch (SYMVAL_TAG (*sv))
    {
    case symval_char:
      ok = false;
      break;
    
    case symval_real:
      ok = true;
      break;
      
    case symval_real_string:
      ok = resolve_string (sv, SYMVAL_REAL (*sv));
      break;
    
    case symval_string:
      ok = resolve_string (sv, 1.0);
      break;
    
    default:
      abort (); /* We have listed all the cases.  */
    }
  
  return ok;
}


/* Assume SV is non-null, and that its SYMVAL_STRING field is
   meaningful.  Look up that string, resolve the result.  If that
   succeeds, multiply by FACTOR, change SV, and return success.  */

static boolean
resolve_string (symval_type *sv, real factor)
{
  boolean ok = false;
  
  symval_type *sv_aux = symtab_lookup (SYMVAL_STRING (*sv));

  if (symval_resolve (sv_aux))
    {
      SYMVAL_TAG (*sv) = symval_real;
      SYMVAL_REAL (*sv) = SYMVAL_REAL (*sv_aux) * factor;
      ok = true;
    }
  
  return ok;
}

/* Return a description of SV as a string.  */

string
symval_as_string (symval_type sv)
{
  string desc;
  
  switch (SYMVAL_TAG (sv))
    {
    case symval_char:
      {
        string charcode = CHAR_BITMAP_INFO (SYMVAL_CHAR (sv)) != NULL
                        ? concat (" ", utoa (CHAR_CHARCODE (SYMVAL_CHAR (sv))))
                        : "";
        desc = concat ("character", charcode);
        if (*charcode != 0)
          free (charcode);
      }
      break;
    
    case symval_real:
      desc = dtoa (SYMVAL_REAL (sv));
      break;
      
    case symval_real_string:
      desc = concat (dtoa (SYMVAL_REAL (sv)), SYMVAL_STRING (sv));
      break;
    
    case symval_string:
      desc = xstrdup (SYMVAL_STRING (sv));
      break;
    
    default:
      abort (); /* We have listed all the cases.  */
    }
  
  return desc;
}
