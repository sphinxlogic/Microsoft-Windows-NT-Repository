/*
 * File:        tool3d.cc
 * Purpose:     class implementation for projecting 3D objects
 * Author:      Markus Franz Meisinger
 * e-mail:      Markus.Meisinger@risc.uni-linz.ac.at
 * Created:     June 1994
 * Updated:     
 * Copyright:   
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifndef VMS
#include <malloc.h>
#endif

#include "tool3d.h"

object3D::object3D(void)
{
  int i;

  for (i = 0; i < 360; i++) {
    sinTable[i] = (int)(sin(i/57.29577951) * 16384);
  }

  alpha = beta = gamma = 0;

  faces.no = 0;
  faces.points = 0;

}

object3D::~object3D(void)
{
  int i;
  struct faceStruct *face;

  for (i = 0; i < faces.no; i++) {   // for all faces
    face = (faces.points+i);         //   free each dynamic allocated data
    free(face->origPoint);
    free(face->point);
    free(face->projected);
  }
  free(faces.points);
}


// returns the sin of <grad> times 16384
long object3D::Sinus(int grad)
{
  return (sinTable[grad]);
}

// returns the cos of <grad> times 16384
long object3D::Cosinus(int grad)
{
  return (sinTable[((grad + 90) % 360)]);
}

void object3D::Project(int xOfs, int yOfs, int xEye, int yEye, int zEye)
{
  int    i, j, noPts;
  long   tmp, d;
  struct faceStruct *face;
  struct point3D *pt;

  for (i = 0; i < faces.no; i++) {                 // for all faces
    face = (faces.points+i);
    noPts = face->noPts;
    face->visible = ((long)(xEye * face->normal.x) +       //   if visible
		     (long)(yEye * face->normal.y) +
		     (long)(zEye * face->normal.z)) > 0;
    if (face->visible) {
      for (j = 0; j < noPts; j++) {                //     project each point
	pt  = ((face->point)+j);
        d   = (long)zEye;
	tmp = (long)pt->z - d;

	(*(face->projected+j)).x = xOfs - (int)((long)(pt->x * d) / tmp);
	(*(face->projected+j)).y = yOfs + (int)((long)(pt->y * d) / tmp);
      }
    }
  }
}

void object3D::RotateX(void)
{
  int i, j, noPts;
  long sinAlpha, cosAlpha;
  long y, z, yTmp, zTmp;
  struct faceStruct *face;
  struct point3D *pt;

  sinAlpha = Sinus(alpha);
  cosAlpha = Cosinus(alpha);

  for (i = 0; i < faces.no; i++) {                  // for all faces

    face = (faces.points+i);
    noPts = face->noPts;

    // restore original vector
    face->normal = face->origNormal;

    // fist, rotate normal vector
    y = (long)(face->normal.y);
    z = (long)(face->normal.z);
    yTmp = (y * cosAlpha + z * sinAlpha) >> 14;
    zTmp = (-y * sinAlpha + z * cosAlpha) >> 14;
    face->normal.y = (int)yTmp;
    face->normal.z = (int)zTmp;

    for (j = 0; j < noPts; j++) {                   //   rotate each point
      pt  = (face->point+j);

      // restore original point
      *pt = *(face->origPoint+j);

      // rotate point
      y = (long)(pt->y);
      z = (long)(pt->z);
      yTmp = (y * cosAlpha + z * sinAlpha) >> 14;
      zTmp = (-y * sinAlpha + z * cosAlpha) >> 14;
      pt->y = (int)yTmp;
      pt->z = (int)zTmp;
    }
  }
}

void object3D::RotateY(void)
{
  int i, j, noPts;
  long sinBeta, cosBeta;
  long x, z, xTmp, zTmp;
  struct faceStruct *face;
  struct point3D *pt;

  sinBeta = Sinus(beta);
  cosBeta = Cosinus(beta);

  for (i = 0; i < faces.no; i++) {                  // for all faces

    face = (faces.points+i);
    noPts = face->noPts;

    // fist, rotate normal vector
    x = (long)(face->normal.x);
    z = (long)(face->normal.z);
    xTmp = (x * cosBeta + z * sinBeta) >> 14;
    zTmp = (-x * sinBeta + z * cosBeta) >> 14;
    face->normal.x = (int)xTmp;
    face->normal.z = (int)zTmp;

    for (j = 0; j < noPts; j++) {                   // rotate each point
      pt  = (face->point+j);
      x = (long)(pt->x);
      z = (long)(pt->z);
      xTmp = (x * cosBeta + z * sinBeta) >> 14;
      zTmp = (-x * sinBeta + z * cosBeta) >> 14;
      pt->x = (int)xTmp;
      pt->z = (int)zTmp;
    }
  }
}

void object3D::RotateZ(void)
{
  int i, j, noPts;
  long sinGamma, cosGamma;
  long x, y, xTmp, yTmp;
  struct faceStruct *face;
  struct point3D *pt;

  sinGamma = Sinus(gamma);
  cosGamma = Cosinus(gamma);

  for (i = 0; i < faces.no; i++) {                  // for all faces

    face = (faces.points+i);
    noPts = face->noPts;

    // fist, rotate normal vector
    x = (long)(face->normal.x);
    y = (long)(face->normal.y);
    xTmp = (x * cosGamma + y * sinGamma) >> 14;
    yTmp = (-x * sinGamma + y * cosGamma) >> 14;
    face->normal.x = (int)xTmp;
    face->normal.y = (int)yTmp;

    for (j = 0; j < noPts; j++) {                   // rotate each point
      pt  = (face->point+j);
      x = (long)(pt->x);
      y = (long)(pt->y);
      xTmp = (x * cosGamma + y * sinGamma) >> 14;
      yTmp = (-x * sinGamma + y * cosGamma) >> 14;
      pt->x = (int)xTmp;
      pt->y = (int)yTmp;
    }
  }
}

void object3D::Rotate(int xRot, int yRot, int zRot)
{
  alpha = xRot;
  beta  = yRot;
  gamma = zRot;

  RotateX();
  RotateY();
  RotateZ();
}

void object3D::Translate(int x, int y, int z)
{
  int i, j, noPts;
  struct faceStruct *face;
  struct point3D *pt;

  for (i = 0; i < faces.no; i++) {                  // for all faces

    face = (faces.points+i);
    noPts = face->noPts;

    for (j = 0; j < noPts; j++) {                   //   translate each point
      pt  = (face->point+j);
      pt->x += x;
      pt->y += y;
      pt->z += z;
    }
  }
}
