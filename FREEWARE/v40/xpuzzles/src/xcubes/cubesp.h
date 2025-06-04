/*
# X-BASED CUBES
#
#  CubesP.h
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

/* Private header file for Cubes */

#ifndef _CubesP_h
#define _CubesP_h

#include "Cubes.h"

/*** random number generator ***/
/* insert your favorite */
extern void SetRNG();
extern long LongRNG();
#define SRAND(X) SetRNG((long) X)
#define LRAND() LongRNG()
 
#define NRAND(X) ((int)(LRAND()%(X)))

#define TOP 0
#define RIGHT 1
#define BOTTOM 2
#define LEFT 3
#define IN 4
#define OUT 5
#define COORD 6

/* The following 2 are in xcubes.c also */
#define MINCUBES 1
#define MAXCUBES 8

#define DEFAULTCUBES 3
#define MAXCUBES_CB (MAXCUBES*MAXCUBES*MAXCUBES)

#define ABS(a) (((a)<0)?(-a):(a))
#define SIGN(a) (((a)<0)?(-1):1)
#define MIN(a,b) (((int)(a)<(int)(b))?(int)(a):(int)(b))
#define MAX(a,b) (((int)(a)>(int)(b))?(int)(a):(int)(b))

typedef struct _CubesPart
{
  Pixel foreground;
  Pixel brick_color;
  int brick_of_position[MAXCUBES_CB], space_position;
  Boolean started, vertical;
  int base;
  int size_x, size_y, size_z, size_rect, size_size;
  Position wid, ht;
  Position brick_width, brick_height;
  Position face_width, face_height;
  Position puzzle_width, puzzle_height;
  XPoint delta, puzzle_offset, digit_offset;
  GC puzzle_GC;
  GC brick_GC;
  GC inverse_GC;
  XtCallbackList select;
} CubesPart;

typedef struct _CubesRec
{
  CorePart core;
  CubesPart cubes;
} CubesRec;

/* This gets around C's inability to do inheritance */
typedef struct _CubesClassPart
{
  int ignore;
} CubesClassPart;

typedef struct _CubesClassRec
{
  CoreClassPart core_class;
  CubesClassPart cubes_class;
} CubesClassRec;

extern CubesClassRec cubesClassRec;

/* For future self-solver */
extern int move_cubes_dir();
/*extern void solve_bricks();*/
extern void draw_all_bricks();
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

#endif /* _CubesP_h */
