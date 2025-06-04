/*
   X-BASED VECTOR GLOBE PROJECTION

   algebra.h

   Copyright (c) 1994, 1995 Frederick (Rick) A Niles
   niles@axp745.gsfc.nasa.gov

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */


typedef struct
  {
    double x;
    double y;
    double z;
  }
Vector;

/******** algebra.c *******************/
void norm_vec (Vector *);
double vdot (double[], double[]);
void vcross (double[], double[], double[]);
void mtrn (double *, double *, int, int);
void mmult (double *, double *, double *,
	    short, short, short);
void create_euler_trans (double[], double, double,
			 double[3][3]);
