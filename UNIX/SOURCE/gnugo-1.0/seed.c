/*
                GNU GO - the game of Go (Wei-Chi)
                Version 1.1   last revised 3-1-89
           Copyright (C) Free Software Foundation, Inc.
                      written by Man L. Li
                      modified by Wayne Iba
                    documented by Bob Webber
*/
/*
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation - version 1.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License in file COPYING for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

Please report any bug/fix, modification, suggestion to

mail address:   Man L. Li
                Dept. of Computer Science
                University of Houston
                4800 Calhoun Road
                Houston, TX 77004

e-mail address: manli@cs.uh.edu         (Internet)
                coscgbn@uhvax1.bitnet   (BITNET)
                70070,404               (CompuServe)
*/

/*
#define  IBM  8086
*/

#define  SUN  68000

#include <stdio.h>

#ifdef SUN

#include <sys/time.h>

#endif

#ifdef IBM

seed(i)
/* start seed of random number generator for PC */
/* Computer Innovation C86 compiler version */
int *i;
  {
   struct regval {int ax, bx, cx, dx, si, di, ds, es;};
   struct regval sreg, rreg;

   sreg.ax = 0x2c00;
   sysint21(&sreg, &rreg);
   *i = rreg.dx;
}  /* end seed */
#endif


#ifdef SUN

seed(i)
/* start seed of random number generator for Sun */
int *i;
  {
   struct timeval tp;
   struct timezone tzp;

   gettimeofday(&tp, &tzp);
   *i = tp.tv_usec;
}  /* end seed */

#endif

