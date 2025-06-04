/*
# X-BASED PYRAMINX(tm)
#
#  PyraminxP.h
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

/* Private header file for Pyraminx */

#ifndef _PyraminxP_h
#define _PyraminxP_h

#include "Pyraminx.h"

/*** random number generator ***/
/* insert your favorite */
extern void SetRNG();
extern long LongRNG();
#define SRAND(X) SetRNG((long) X)
#define LRAND() LongRNG()
 
#define NRAND(X) ((int)(LRAND()%(X)))

#define IGNORE (-1)
#define TOP 0
#define TR 1
#define RIGHT 2
#define BOTTOM 3
#define BL 4
#define LEFT 5
#define COORD 6
#define CW 7
#define CCW 11

/* The following 6 are in xpyraminx.c also */
#define MINTETRAS 1
#define MAXTETRAS 7
#define PERIOD2 2
#define PERIOD3 3
#define BOTH 4
#define MAXMODES 3

#define DEFAULTMODE PERIOD3
#define DEFAULTTETRAS 3
#define MAXFACES 4
#define DOWN 0
#define UP 1
#define MAXVIEWS 2
#define MAXSIDES (MAXFACES/MAXVIEWS) 
#define MAXORIENT (3*MAXSIDES)

#define ABS(a) (((a)<0)?(-a):(a))
#define SIGN(a) (((a)<0)?(-1):1)
#define MIN(a,b) (((int)(a)<(int)(b))?(int)(a):(int)(b))
#define MAX(a,b) (((int)(a)>(int)(b))?(int)(a):(int)(b))

typedef struct _PyraminxLoc
{
  int face, rotation;
} PyraminxLoc;

typedef struct _PyraminxPart
{
  Pixel foreground;
  Pixel face_color[MAXFACES];
  PyraminxLoc tetra_loc[MAXVIEWS][MAXTETRAS][MAXTETRAS][MAXSIDES];
  PyraminxLoc face_loc[MAXTETRAS][MAXTETRAS][MAXSIDES];
  PyraminxLoc row_loc_p2[2][MAXTETRAS][MAXSIDES];
  PyraminxLoc row_loc_p3[3][MAXTETRAS][MAXSIDES];
  Boolean started, practice, orient, vertical, mono, sticky;
  int mode, depth;
  int size;
  Position delta;
  Position tetra_length;
  Position face_length;
  Position view_length;
  Position puzzle_width, puzzle_height;
  Position side_offset, orient_line_length, orient_diag_length;
  XPoint puzzle_offset;
  GC puzzle_GC;
  GC face_GC[MAXFACES];
  GC inverse_GC;
  String face_name[MAXFACES];
  XtCallbackList select;
} PyraminxPart;

typedef struct _PyraminxRec
{
  CorePart core;
  PyraminxPart pyraminx;
} PyraminxRec;

/* This gets around C's inability to do inheritance */
typedef struct _PyraminxClassPart
{
  int ignore;
} PyraminxClassPart;

typedef struct _PyraminxClassRec
{
  CoreClassPart core_class;
  PyraminxClassPart pyraminx_class;
} PyraminxClassRec;

extern PyraminxClassRec pyraminxClassRec;

/* For future self-solver */
extern void move_pyraminx();
/*extern void solve_polyhedrons();*/
extern void draw_all_polyhedrons();
extern Boolean check_solved();
extern void init_moves();
extern void put_move();
extern void get_move();
extern int moves_made();
extern void flush_moves();
extern int num_moves();           
extern void scan_moves();          
extern void print_moves();
extern void scan_start_position();
extern void print_start_position();
extern void set_start_position();

#endif /* _PyraminxP_h */
