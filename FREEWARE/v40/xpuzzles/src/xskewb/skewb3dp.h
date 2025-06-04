/*
# X-BASED SKEWB
#
#  Skewb3dP.h
#
###
#
#  Copyright (c) 1994 - 95	David Albert Bagley, bagleyd@source.asset.com
#
#                   All Rights Reserved
#
#  Permission to use, copy, modify, and distribute this software and
#  its documentation for any purpose and without fee is hereby granted,
#  provided that the above copyright notice appear in all copies and
#  that both that copyright notice and this permission notice appear in
#  supporting documentation, and that the name of the author not be
#  used in advertising or publicity pertaining to distribution of the
#  software without specific, written prior permission.
#
#  This program is distributed in the hope that it will be "playable",
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
*/

/* Private header file for Skewb3d */

#ifndef _Skewb3dP_h
#define _Skewb3dP_h

#include "Skewb3d.h"

#define SAME 0
#define OPPOSITE 1
#define DOWN 0
#define UP 1
#define COORD3D 12
#define MAXVIEWS 2
#define MAXORIENT 4

#define SQRT_3 1.732050808
#define MULTIPLY(a) ((int)((double)a*SQRT_3/2.0))
#define DIVIDE(a) ((int)((double)a*2.0/SQRT_3))

typedef struct _Skewb3DPart
{
  Position cube_width, cube_height, cube_diagonal, cube_diag;
  Position diamond_width, diamond_height, diamond_diagonal;
  Position face_width, face_height, face_diagonal;
  Position view_width, view_height;
  XPoint view_middle;
} Skewb3DPart;

typedef struct _Skewb3DRec
{
  CorePart core;
  SkewbPart skewb;
  Skewb3DPart skewb3d;
} Skewb3DRec;

/* This gets around C's inability to do inheritance */
typedef struct _Skewb3DClassPart
{
  int ignore;
} Skewb3DClassPart;

typedef struct _Skewb3DClassRec
{
  CoreClassPart core_class;
  SkewbClassPart skewb_class;
  Skewb3DClassPart skewb3d_class;
} Skewb3DClassRec;

extern Skewb3DClassRec skewb3dClassRec;

extern int position_polyhedrons3d();
extern void draw_diamond3d();
extern void draw_triangle3d();

#endif /* _Skewb3dP_h */
