/* te-irix.h  Irix target-specific emulation file.
   Copyright (C) 1993 Free Software Foundation, Inc.

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

#include "te-generic.h"

/* gcc for the SGI generates a bunch of local labels named LM%d.  I
   don't know why they don't start with '$'. We must check specially
   for these.  */
#undef LOCAL_LABEL
#define LOCAL_LABEL(name) \
  ((name)[0] == '$' || ((name)[0] == 'L' && (name)[1] == 'M'))
