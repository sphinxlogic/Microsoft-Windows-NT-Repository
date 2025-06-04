/*
   X-BASED VECTOR GLOBE PROJECTION

   rotation.c

   Copyright (c) 1994, 1995 Frederick (Rick) A Niles,
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

#include "vgp.h"
/******************************/
void
create3DSphere (image_data * data)
{
  int i;
  double rad, cosrad, sinrad;
  for (i = 0; i < INC; i++)
    {
      rad = i * 360. / ((INC - 2) * ONE_DEGREE);
      cosrad = cos (rad);
      sinrad = sin (rad);
      data->xyplane[i].x = cosrad;
      data->xyplane[i].y = sinrad;
      data->xyplane[i].z = 0.;
      data->yzplane[i].x = 0.;
      data->yzplane[i].y = cosrad;
      data->yzplane[i].z = sinrad;
      data->xzplane[i].x = cosrad;
      data->xzplane[i].y = 0.;
      data->xzplane[i].z = sinrad;
    }

}
/*******************************/
void
create_rot_matrix (image_data * data)
{
  double xt[3][3] = IDEN_MAT, yt[3][3] = IDEN_MAT, zt[3][3] = IDEN_MAT,
    tmp[3][3];
  int i, j;
  xt[1][1] = cos (data->rotation[0]);
  xt[1][2] = sin (data->rotation[0]);
  xt[2][1] = -sin (data->rotation[0]);
  xt[2][2] = cos (data->rotation[0]);

  yt[0][0] = cos (data->rotation[1]);
  yt[0][2] = -sin (data->rotation[1]);
  yt[2][0] = sin (data->rotation[1]);
  yt[2][2] = cos (data->rotation[1]);

  zt[0][0] = cos (data->rotation[2]);
  zt[0][1] = sin (data->rotation[2]);
  zt[1][0] = -sin (data->rotation[2]);
  zt[1][1] = cos (data->rotation[2]);

  mmult (&zt[0][0], &yt[0][0], &data->T[0][0], 3, 3, 3);
  mmult (&data->T[0][0], &xt[0][0], &tmp[0][0], 3, 3, 3);

  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      data->T[i][j] = tmp[i][j];
}

/*********************************/
void
screen_to_3D (int x, int y, double zoom, DPoint pan,
	      double T[3][3], double vec[3])
{
  int i;
  double len, Tt[3][3], tmp[3];
/*  vec[0] = x / zoom - pan.x;
   vec[1] = y / zoom - pan.y; */
  vec[0] = (x - pan.x) / zoom;
  vec[1] = (y - pan.y) / zoom;
  len = sqrt (vec[0] * vec[0] + vec[1] * vec[1]);
  if (len > 1.0)
    {
      vec[0] = vec[0] / len;
      vec[1] = vec[1] / len;
      vec[2] = 0.0;
    }
  else
    {
      vec[2] = -sqrt (1.0 - len * len);
    }
  /* printf("2d vec->%f %f %f \n",vec[0],vec[1],vec[2]); */
/**** vec is now in the 2D frame ****/
  mtrn (&T[0][0], &Tt[0][0], 3, 3);
  mmult (&Tt[0][0], &vec[0], &tmp[0], 3, 3, 1);
  for (i = 0; i < 3; i++)
    vec[i] = tmp[i];
  len = sqrt (vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
  vec[0] = vec[0] / len;
  vec[1] = vec[1] / len;
  vec[2] = vec[2] / len;

  /*  printf("3d vec->%f %f %f \n",vec[0],vec[1],vec[2]); */
}


/**************************************/
void
TD_to_screen (double T[3][3], double zoom, DPoint pan,
	      Vector * points, int *npoints,
	      XPoint vec[], int show)
{
  DPoint canidate;
  int i, count, draw_flag;
  double two_D_frame[3];
  count = 0;
  draw_flag = 0;
  for (i = 0; i < *npoints; i++)
    {
      mmult (&T[0][0], (double *) &(points[i]), two_D_frame, 3, 3, 1);
      if (two_D_frame[2] < 0 || show)
	{
	  canidate.x = (zoom * two_D_frame[0] + pan.x);
	  canidate.y = (zoom * two_D_frame[1] + pan.y);
	  if (fabs (canidate.x) < MAX_SCREEN
	      && fabs (canidate.y) < MAX_SCREEN)
	    {
	      (vec[count]).x = (int) canidate.x;
	      (vec[count]).y = (int) canidate.y;
	      count++;
	    }
	  draw_flag = 1;
	}
      else if (two_D_frame[2] == 0 || draw_flag)
	{
	  (vec[count]).x = 0;
	  (vec[count]).y = 0;
	  count++;
	}
    }
  *npoints = count;
}

/****************************/
void
move_sphere (image_data_ptr data)
{
  int i, j;
  double len, costheta;
  double start_vec[3], last_vec[3], cross_prod[3];
  double ET[3][3], tmp[3][3];
  screen_to_3D (data->start.x, data->start.y, data->zoom, data->pan,
		data->start_T, start_vec);
  screen_to_3D (data->last.x, data->last.y, data->zoom, data->pan,
		data->start_T, last_vec);
  vcross (start_vec, last_vec, cross_prod);
  costheta = vdot (start_vec, last_vec);
  len = sqrt (cross_prod[0] * cross_prod[0] +
	      cross_prod[1] * cross_prod[1] +
	      cross_prod[2] * cross_prod[2]);

  if (fabs (1 - len * len - costheta * costheta) > 0.000001)
    printf ("not normalized.\n");

  if (len > 0.01)
    {
/***** Normalize cross_prod ********/
      for (i = 0; i < 3; i++)
	cross_prod[i] = cross_prod[i] / len;

      create_euler_trans (cross_prod, costheta, len, ET);
      mmult (&data->start_T[0][0], &ET[0][0], &tmp[0][0], 3, 3, 3);
      for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++)
	  data->T[i][j] = tmp[i][j];
    }

}
