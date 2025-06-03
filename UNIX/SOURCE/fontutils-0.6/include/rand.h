/* rand.h: declarations for pseudo-random number generator.

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

#ifndef RAND_H
#define RAND_H

/* Forgot the system's idea of `RAND_MAX'.  (This must therefore be
   included after <stdlib.h>.)  */
#ifdef RAND_MAX
#undef RAND_MAX
#endif
#define RAND_MAX ((1 << 31) - 2)

/* Set the state of the random number generator.  */
extern void seed_rand (unsigned seed);

/* Return a pseudo-random number between 0 and RAND_MAX.  */
extern int k_rand (void);

#endif /* not RAND_H */
