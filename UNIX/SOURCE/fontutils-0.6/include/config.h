/* config.h: master configuration file, included first by all compilable
   source files (not headers).

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

#ifndef CONFIG_H
#define CONFIG_H

/* System dependencies that are figured out by `configure'.  */
#include "c-auto.h"

/* Because we use the GNU m/c/realloc from libg++, we always have the ANSI C
   definition for the return type.  */
#define ALLOC_RETURN_TYPE void

/* ``Standard'' headers.  */
#include "c-std.h"

/* strchr vs. index, memcpy vs. bcopy, etc.  */
#include "c-memstr.h"

/* Error numbers and errno declaration.  */
#include "c-errno.h"

/* Standard in ANSI C.  */
#include <assert.h>

/* Numeric minima and maxima.  */
#include "c-minmax.h"

/* The arguments for fseek.  */
#include "c-seek.h"

/* Usually declared in <math.h>, but not always.  */
#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif


/* Macros to discard or keep prototypes.  */
#include "c-proto.h"

/* Some definitions of our own.  */
#include "global.h"

#endif /* not CONFIG_H */
