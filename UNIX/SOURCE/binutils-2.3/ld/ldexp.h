/* ldexp.h -
   Copyright 1991, 1992 Free Software Foundation, Inc.

   This file is part of GLD, the Gnu Linker.

   GLD is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GLD is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GLD; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* The result of an expression tree */
typedef struct 
{
  bfd_vma value;
  struct lang_output_section_statement_struct *section;
  boolean valid;
} etree_value_type;



typedef struct 
{
  int node_code;
  enum { etree_binary,
	   etree_trinary,
	   etree_unary,
	   etree_name,
	   etree_assign,
	   etree_undef,
	   etree_unspec,
	   etree_value } node_class;
} node_type;



typedef union etree_union 
{
  node_type type;
  struct {
    node_type type;
    union etree_union *lhs;
    union etree_union *rhs;
  } binary;
  struct {
    node_type type;
    union etree_union *cond;
    union etree_union *lhs;
    union etree_union *rhs;
  } trinary;
  struct {
    node_type type;
    CONST char *dst;
    union etree_union *src;
  } assign;

  struct {
    node_type type;
    union   etree_union *child;
  } unary;
  struct {
    node_type type;
   CONST char *name;
  } name;
  struct {
    node_type type;
    bfd_vma value;
  } value;

} etree_type;


etree_type *exp_intop PARAMS ((bfd_vma));
etree_value_type invalid PARAMS ((void));
etree_value_type exp_fold_tree PARAMS ((etree_type *, struct
					lang_output_section_statement_struct *,
					lang_phase_type,
					bfd_vma, bfd_vma *));
etree_type *exp_binop PARAMS ((int, etree_type *, etree_type *));
etree_type *exp_trinop PARAMS ((int,etree_type *, etree_type *, etree_type *));
etree_type *exp_unop PARAMS ((int, etree_type *));
etree_type *exp_nameop PARAMS ((int, CONST char *));
etree_type *exp_assop PARAMS ((int, CONST char *, etree_type *));
void exp_print_tree PARAMS ((etree_type *));
bfd_vma exp_get_vma PARAMS ((etree_type *, bfd_vma, char *, lang_phase_type));
int exp_get_value_int PARAMS ((etree_type *, int, char *,lang_phase_type));
