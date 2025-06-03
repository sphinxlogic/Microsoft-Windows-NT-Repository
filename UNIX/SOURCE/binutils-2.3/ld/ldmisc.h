/* ldmisc.h -
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

/* VARARGS*/
void einfo ();
/* VARARGS*/
void minfo ();
/* VARARGS*/
void info_msg ();
void info_assert PARAMS ((char *, unsigned int));
void multiple_warn PARAMS ((char *message1, asymbol *sym, char *message2, asymbol *sy));
void yyerror PARAMS ((char *));
char *concat PARAMS ((CONST char *, CONST char *, CONST char *));
PTR ldmalloc PARAMS ((size_t));
PTR ldrealloc PARAMS ((PTR, size_t));
char *buystring PARAMS ((CONST char *CONST));

#define ASSERT(x) \
{ if (!(x)) info_assert(__FILE__,__LINE__); }

#define FAIL() \
{ info_assert(__FILE__,__LINE__); }

void print_space PARAMS ((void));
void print_nl PARAMS ((void));
void print_address PARAMS ((bfd_vma value));
