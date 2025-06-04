/*
   X-BASED VECTOR GLOBE PROJECTION

   algebra.c

   Copyright (c) 1994, 1995  Frederick (Rick) A Niles
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

#include <math.h>
#include "algebra.h"

/*********************************/
/********** Vector Section *******/
/*********************************/

/*************************************/
void
norm_vec (Vector * v)
{
  double norm;

  norm = sqrt (v->x * v->x + v->y * v->y + v->z * v->z);

  if (norm != 0)
    {
      v->x = v->x / norm;
      v->y = v->y / norm;
      v->z = v->z / norm;
    }
}

/*****************************/
double
vdot (double u[3], double v[3])
{
  return (u[0] * v[0] + u[1] * v[1] + u[2] * v[2]);
}

/******************************/
void
vcross (double u[3], double v[3], double w[3])
{
  w[0] = u[1] * v[2] - u[2] * v[1];
  w[1] = u[2] * v[0] - u[0] * v[2];
  w[2] = u[0] * v[1] - u[1] * v[0];
}

/*********************************/
/********** Matrix Section * *****/
/*********************************/

/********* mtrn ***************/
void
mtrn (double *a, double *b, int n, int m)
/****** b( m x n ) = trans[ a ( n x m ) ] ****/
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      b[j + i * m] = a[i + n * j];
}

/****************** mmult ********/
/*void mmult(a,b,c,l,m,n) */
/*********** Matrix multiplication ***/
/********a(l,m) * b(m,n) = c(l,n) ****/
/*double *a, *b, *c;
   int l,m,n;
   {
   double d;
   int i,j,k;
   for(i = 0;  i < l; i++)
   for(k = 0; k < n; k++)
   {
   d = 0.0;
   for(j = 0; j < m; j++)
   d = d + a[i*m+j] * b[j*n+k];
   c[i*n+k] = d;
   }
   }
 */

/*
   **  Matrix Multiply
   **
   **      C[r1,c2] = Sum{c1} of A[r1,c1] * B[r2,c2], where r2 = c1
   **
 */

void
mmult (double *A, double *B, double *C, short r1, short c1, short c2)
{
  double *temp;
  double Sum;

  short i, j, k;
  short i1;
  short k2;

  i = i1 = 0;

  do
    {
      j = 0;
      do
	{
	  Sum = 0.0;
	  k = k2 = 0;
	  temp = &A[i1];
	  do
	    {
	      Sum += (*temp++ * B[k2 + j]);
	      k2 += c2;
	      k++;
	    }
	  while (k < c1);
	  *C++ = Sum;
	  j++;
	}
      while (j < c2);
      i1 += c1;
      i++;
    }
  while (i < r1);
}


/******************************/
void
create_euler_trans (double e[3],
		    double costheta,
		    double sintheta,
		    double A[3][3])
{
  double c, s, omc;
  c = costheta;
  s = -sintheta;
  omc = 1 - c;
  A[0][0] = c + e[0] * e[0] * omc;
  A[0][1] = e[0] * e[1] * omc + e[2] * s;
  A[0][2] = e[0] * e[2] * omc - e[1] * s;
  A[1][0] = e[0] * e[1] * omc - e[2] * s;
  A[1][1] = c + e[1] * e[1] * omc;
  A[1][2] = e[1] * e[2] * omc + e[0] * s;
  A[2][0] = e[0] * e[2] * omc + e[1] * s;
  A[2][1] = e[1] * e[2] * omc - e[0] * s;
  A[2][2] = c + e[2] * e[2] * omc;
}
