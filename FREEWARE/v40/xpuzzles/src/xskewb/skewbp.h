/*
# X-BASED SKEWB
#
#  SkewbP.h
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

/* Private header file for Skewb */

#ifndef _SkewbP_h
#define _SkewbP_h

#include "Skewb.h"

/*** random number generator ***/
/* insert your favorite */
extern void SetRNG();
extern long LongRNG();
#define SRAND(X) SetRNG((long) X)
#define LRAND() LongRNG()
 
#define NRAND(X) ((int)(LRAND()%(X)))

#define IGNORE (-1)
#define TR 0
#define BR 1
#define BL 2
#define TL 3
#define STRT 4
#define CW 5
#define HALF 6
#define CCW 7
#define TOP 8
#define RIGHT 9
#define BOTTOM 10
#define LEFT 11
#define MAXORIENT 4
#define MAXROTATE 3
#define MAXCUBES (MAXORIENT+1)
#define MINOR 0
#define MAJOR 1

/* The following is in xskewb.c also */
#define MAXFACES 6

#define MIN(a,b) (((int)(a)<(int)(b))?(int)(a):(int)(b))
#define MAX(a,b) (((int)(a)>(int)(b))?(int)(a):(int)(b))

typedef struct _SkewbLoc
{
  int face, rotation;
} SkewbLoc;

typedef struct _SkewbPart
{
  Pixel foreground;
  Pixel face_color[MAXFACES];
  SkewbLoc cube_loc[MAXFACES][MAXCUBES];
  SkewbLoc face_loc[MAXCUBES];
  SkewbLoc row_loc[MAXORIENT][MAXCUBES];
  SkewbLoc minor_loc[MAXORIENT], major_loc[MAXORIENT][MAXORIENT];
  Boolean started, practice, orient, vertical, mono;
  int dim; /* This allows us to reuse code between 2d and 3d */
  int depth;
  int face, cube, direction;
  Position delta;
  Position orient_line_length;
  Position puzzle_width, puzzle_height;
  XPoint puzzle_offset, letter_offset;
  GC puzzle_GC;
  GC face_GC[MAXFACES];
  GC inverse_GC;
  String face_name[MAXFACES];
  XtCallbackList select;
  SkewbLoc skewb_loc[MAXFACES][MAXCUBES];
} SkewbPart;

typedef struct _SkewbRec
{
  CorePart core;
  SkewbPart skewb;
} SkewbRec;

/* This gets around C's inability to do inheritance */
typedef struct _SkewbClassPart
{
  int ignore;
} SkewbClassPart;

typedef struct _SkewbClassRec
{
  CoreClassPart core_class;
  SkewbClassPart skewb_class;
} SkewbClassRec;

typedef struct _RowNext
{
  int face, direction, side_face;
} RowNext;

extern SkewbClassRec skewbClassRec;

extern void quit_skewb();
extern void practice_skewb();
extern void maybe_practice_skewb();
extern void randomize_skewb();
extern void maybe_randomize_skewb();
extern void enter_skewb();
extern void write_skewb();
extern void undo_skewb();
extern void solve_skewb();
extern void orientize_skewb();
extern void move_skewb_cw();
extern void move_skewb_ccw();
extern void move_skewb_input();
extern void move_skewb();
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

#ifdef DEBUG
extern void print_cube();
#endif

#endif /* _SkewbP_h */
