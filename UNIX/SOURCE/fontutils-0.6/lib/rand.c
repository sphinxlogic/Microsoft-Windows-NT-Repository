/* rand.c: a simple pseudo-random number generator.

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

#include "rand.h"


/* Current state of the random number generator.  ANSI C says that the
   default should be as if `srand' was called with the value 1.  At all
   times this will be a random number between 1 and RAND_MAX+1.  */
static long rand_state = 2;


/* Set the state.  It's ok if `seed' doesn't fit in a long, since we
   check for negative values before we return anything.  On the other
   hand, we cannot allow our state to become zero, since then we will
   produce zero forever.  */

void
seed_rand (unsigned seed)
{
  rand_state = seed + 1;
}


/* Return a pseudo-random number based on `x' in the range [0, RAND_MAX].
   We use the Ghostscript computation (from the function `zrand' in the
   file `zmath.c').  */

int
k_rand ()
{
  /* We use an algorithm from CACM 31(10), pp. 1192-1201, October 1988. 
     According to a posting by Ed Taft on comp.lang.postscript, Level 2
     (Adobe) PostScript interpreters use this algorithm too:

            x[n+1] = (16807 * x[n]) mod (2^31 - 1)
     
     The complication ensues from the fact that the multiplication may
     lead to a 46-bit number.  */

#define A 16807
#define M 0x7fffffff
#define Q 127773			/* M / A */
#define R 2836				/* M % A */
  rand_state = A * (rand_state % Q) - R * (rand_state / Q);
  while ( rand_state <= 0 ) rand_state += M;
  
  /* OK, `rand_state' is our new random number between 1 and 2^31 - 1.
     Now we have to subtract one, so that we return a number in the
     range zero to RAND_MAX (inclusive).  We obviously must define our
     RAND_MAX to be 2^31 - 2, which we do (in `rand.h').  */
  return rand_state - 1;
}
