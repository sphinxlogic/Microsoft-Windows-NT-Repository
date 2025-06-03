#ifndef REGIONSH
#define REGIONSH

/*	Copyright (C) 1992, 1993 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


/*  t. lord	Sat Aug  8 15:48:59 1992	*/

#include "global.h"


struct cmp
{
  char mult;
  CELLREF row;
  CELLREF col;
};

extern struct rng sort_rng;
extern struct rng sort_ele;
extern struct cmp *sort_keys;
extern int sort_keys_num;
extern int sort_keys_alloc;
extern struct rng all_rng;
extern unsigned int print_width;




#ifdef __STDC__
extern void set_rng (struct rng *r, CELLREF r1, CELLREF c1, CELLREF r2, CELLREF c2);
extern void delete_region (struct rng *where);
extern void lock_region (struct rng *where, int locked);
extern void format_region (struct rng *where, int fmt, int just);
extern void print_region (FILE *fp, struct rng *print);
extern void move_region (struct rng *fm, struct rng *to);
extern void copy_region (struct rng *fm, struct rng *to);
extern void copy_values_region (struct rng *fm, struct rng *to);
extern void sort_region (void);
extern int cmp_cells (int n1, int n2);
extern void swp_cells (int n1, int n2);
extern void rot_cells (int n1, int n2);

#else
extern void set_rng ();
extern void delete_region ();
extern void lock_region ();
extern void format_region ();
extern void print_region ();
extern void move_region ();
extern void copy_region ();
extern void copy_values_region ();
extern void sort_region ();
extern int cmp_cells ();
extern void swp_cells ();
extern void rot_cells ();

#endif

#endif
