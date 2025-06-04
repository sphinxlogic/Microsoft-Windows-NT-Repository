/*
# X-BASED DINO
#
#  Dino2dP.h
#
###
#
#  Copyright (c) 1995	David Albert Bagley, bagleyd@source.asset.com
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

/* Private header file for Dino2d */

#ifndef _Dino2dP_h
#define _Dino2dP_h

#include "Dino2d.h"

#define FRONTLOC 0
#define BACKLOC 11
#define COORD2D 8
#define MAXX 3
#define MAXY 4
#define MAXXY 4 /* (MAX(MAXX,MAXY)) */
#define MAXRECT (MAXX*MAXY)

typedef struct _Dino2DPart
{
  Position diamond_length, triangle_length, triangle_width;
  Position face_length;
  Position view_length;
} Dino2DPart;

typedef struct _Dino2DRec
{
  CorePart core;
  DinoPart dino;
  Dino2DPart dino2d;
} Dino2DRec;

/* This gets around C's inability to do inheritance */
typedef struct _Dino2DClassPart
{
  int ignore;
} Dino2DClassPart;

typedef struct _XsDino2DClassRec
{
  CoreClassPart core_class;
  DinoClassPart dino_class;
  Dino2DClassPart dino2d_class;
} Dino2DClassRec;

extern Dino2DClassRec dino2dClassRec;

extern int position_polyhedrons2d();
extern void draw_diamond2d();
extern void draw_triangle2d();

#endif /* _Dino2dP_h */
