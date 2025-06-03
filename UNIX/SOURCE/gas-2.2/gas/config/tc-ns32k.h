/* tc-ns32k.h -- Opcode table for National Semi 32k processor
   Copyright (C) 1987, 1992 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

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

#include "bit_fix.h"

#define tc_aout_pre_write_hook(x)	{;}	/* not used */
#define tc_crawl_symbol_chain(a)	{;}	/* not used */
#define tc_headers_hook(a)		{;}	/* not used */

#ifdef SEQUENT_COMPATABILITY
#define DEF_MODEC 20
#define DEF_MODEL 21
#endif

#ifndef DEF_MODEC
#define DEF_MODEC 20
#endif

#ifndef DEF_MODEL
#define DEF_MODEL 20
#endif

#define MAX_ARGS 4
#define ARG_LEN 50

#define TC_CONS_FIX_NEW cons_fix_new_ns32k

extern void fix_new_ns32k PARAMS ((fragS *frag,
				   int where,
				   int size,
				   struct symbol *add_symbol,
				   long offset,
				   int pcrel,
				   int pcrel_adjust,
				   int im_disp,
				   bit_fixS *bit_fixP,	/* really bit_fixS */
				   int bsr);

extern void cons_fix_new_ns32k PARAMS ((fragS *frag,
					int where,
					int size,
					expressionS *exp));

/* end of tc-ns32k.h */
