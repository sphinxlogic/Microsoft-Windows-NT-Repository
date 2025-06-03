/* ECOFF object file format header file.
   Copyright (C) 1993 Free Software Foundation, Inc.
   Contributed by Cygnus Support.
   Written by Ian Lance Taylor <ian@cygnus.com>.

   This file is part of GAS.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define OBJ_ECOFF 1

#include "targ-cpu.h"

/* For each gas symbol we keep track of which file it came from, of
   whether we have generated an ECOFF symbol for it, and whether the
   symbols is undefined (this last is needed to distinguish a .extern
   symbols from a .comm symbol).  */

#define TARGET_SYMBOL_FIELDS \
  struct efdr *ecoff_file; \
  char ecoff_symbol; \
  char ecoff_undefined;

/* This is used to write the symbolic data in the format that BFD
   expects it.  */
extern void ecoff_frob_file PARAMS ((void));
#define obj_frob_file() ecoff_frob_file ()

/* At the moment we don't want to do any stabs processing in read.c.  */
extern void ecoff_stab PARAMS ((int what, const char *string,
				int type, int other, int desc));
#define OBJ_PROCESS_STAB(what, string, type, other, desc) \
  ecoff_stab ((what), (string), (type), (other), (desc))
