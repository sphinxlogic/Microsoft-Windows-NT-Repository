/*
# X-BASED RUBIK'S CUBE(tm)
#
#  RubikP.h
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

/* Private header file for Rubik */

#ifndef _RubikP_h
#define _RubikP_h

#include "Rubik.h"

/*** random number generator ***/
/* insert your favorite */
extern void SetRNG();
extern long LongRNG();
#define SRAND(X) SetRNG((long) X)
#define LRAND() LongRNG()

#define NRAND(X) ((int)(LRAND()%(X)))

#define IGNORE (-1)
#define TOP 0
#define RIGHT 1
#define BOTTOM 2
#define LEFT 3
#define STRT 4
#define CW 5
#define HALF 6
#define CCW 7
#define MAXORIENT 4

/* The following 3 are in xrubik.c also */
#define MAXFACES 6
#define MINCUBES 1
#define MAXCUBES 6

#define DEFAULTCUBES 3
#define MAXCUBES_SQ (MAXCUBES*MAXCUBES)

#define MIN(a,b) (((int)(a)<(int)(b))?(int)(a):(int)(b))
#define MAX(a,b) (((int)(a)>(int)(b))?(int)(a):(int)(b))

typedef struct _RubikLoc
{
  int face, rotation;
} RubikLoc;

typedef struct _RubikPart
{
  Pixel foreground;
  Pixel face_color[MAXFACES];
  RubikLoc cube_loc[MAXFACES][MAXCUBES][MAXCUBES];
  RubikLoc face_loc[MAXCUBES][MAXCUBES];
  RubikLoc row_loc[MAXORIENT][MAXCUBES];
  Boolean started, practice, orient, vertical, mono;
  int dim; /* This allows us to reuse code between 2d and 3d */
  int depth;
  int size, size_size;
  int face, i, j, direction;
  Position delta;
  Position orient_line_length;
  Position puzzle_width, puzzle_height;
  XPoint puzzle_offset, letter_offset;
  GC puzzle_GC;
  GC face_GC[MAXFACES];
  GC inverse_GC;
  String face_name[MAXFACES];
  XtCallbackList select;
} RubikPart;

typedef struct _RubikRec
{
  CorePart core;
  RubikPart rubik;
} RubikRec;

/* This gets around C's inability to do inheritance */
typedef struct _RubikClassPart
{
  int ignore;
} RubikClassPart;

typedef struct _RubikClassRec
{
  CoreClassPart core_class;
  RubikClassPart rubik_class;
} RubikClassRec;

typedef struct _RowNext
{
  int face, direction, side_face;
} RowNext;

extern RubikClassRec rubikClassRec;

extern void quit_rubik();
extern void practice_rubik();
extern void maybe_practice_rubik();
extern void randomize_rubik();
extern void maybe_randomize_rubik();
extern void enter_rubik();
extern void write_rubik();
extern void undo_rubik();
extern void solve_rubik();
extern void increment_rubik();
extern void decrement_rubik();
extern void orientize_rubik();
extern void move_rubik_cw();
extern void move_rubik_ccw();
extern void move_rubik_input();
extern void move_rubik();
extern void reset_polyhedrons();
extern void solve_polyhedrons();
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
extern void print_face();
extern void print_row();
#endif

#endif /* _RubikP_h */
