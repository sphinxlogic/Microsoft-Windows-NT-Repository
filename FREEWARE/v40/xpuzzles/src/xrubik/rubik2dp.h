/*
# X-BASED RUBIK'S CUBE(tm)
#
#  Rubik2dP.h
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

/* Private header file for Rubik2d */

#ifndef _Rubik2dP_h
#define _Rubik2dP_h

#include "Rubik2d.h"

#define FRONTLOC 0
#define BACKLOC 11
#define COORD2D 8
#define MAXX 3
#define MAXY 4
#define MAXXY 4 /* (MAX(MAXX,MAXY)) */
#define MAXRECT (MAXX*MAXY)

typedef struct _Rubik2DPart
{
  Position cube_length;
  Position face_length;
  Position view_length;
} Rubik2DPart;

typedef struct _Rubik2DRec
{
  CorePart core;
  RubikPart rubik;
  Rubik2DPart rubik2d;
} Rubik2DRec;

/* This gets around C's inability to do inheritance */
typedef struct _Rubik2DClassPart
{
  int ignore;
} Rubik2DClassPart;

typedef struct _XsRubik2DClassRec
{
  CoreClassPart core_class;
  RubikClassPart rubik_class;
  Rubik2DClassPart rubik2d_class;
} Rubik2DClassRec;

extern Rubik2DClassRec rubik2dClassRec;

extern int position_polyhedrons2d();
extern void draw_square2d();

#endif /* _Rubik2dP_h */
