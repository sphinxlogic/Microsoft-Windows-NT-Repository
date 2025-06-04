/*
# X-BASED TRIANGLES
#
#  TriangleP.h
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

/* Private header file for Triangles */

#ifndef _TrianglesP_h
#define _TrianglesP_h

#include "Triangles.h"

/*** random number generator ***/
/* insert your favorite */
extern void SetRNG();
extern long LongRNG();
#define SRAND(X) SetRNG((long) X)
#define LRAND() LongRNG()
 
#define NRAND(X) ((int)(LRAND()%(X)))

#define TR 0
#define RIGHT 1
#define BR 2
#define BL 3
#define LEFT 4
#define TL 5
#define COORD 6
#define TRBL 0
#define TLBR 1
#define ROW 2
#define ROWTYPES 3
#define DOWN 0
#define UP 1
#define MAXORIENT 2

/* The following 2 are in xtriangles.c also */
#define MINTRIANGLES 1
#define MAXTRIANGLES 16 

#define DEFAULTTRIANGLES 4
#define MAXTRIANGLES_SQ (MAXTRIANGLES*MAXTRIANGLES)

#define ABS(a) (((a)<0)?(-a):(a))
#define SIGN(a) (((a)<0)?(-1):1)
#define MIN(a,b) (((int)(a)<(int)(b))?(int)(a):(int)(b))
#define MAX(a,b) (((int)(a)>(int)(b))?(int)(a):(int)(b))

typedef struct _TrianglesPart
{
  Pixel foreground;
  Pixel tile_color;
  int tile_of_position[MAXTRIANGLES_SQ], space_position[MAXORIENT];
  int space_row[MAXORIENT][ROWTYPES];
  int current_position, current_row[ROWTYPES], current_position_orient;
  Boolean started, vertical;
  int base;
  int size, size_size;
  Position wid, ht;
  Position tile_width, tile_height;
  Position puzzle_width, puzzle_height;
  XPoint delta, puzzle_offset, digit_offset;
  GC puzzle_GC;
  GC tile_GC;
  GC inverse_GC;
  XtCallbackList select;
} TrianglesPart;

typedef struct _TrianglesRec
{
  CorePart core;
  TrianglesPart triangles;
} TrianglesRec;

/* This gets around C's inability to do inheritance */
typedef struct _TrianglesClassPart
{
  int ignore;
} TrianglesClassPart;

typedef struct _TrianglesClassRec
{
  CoreClassPart core_class;
  TrianglesClassPart triangles_class;
} TrianglesClassRec;

extern TrianglesClassRec trianglesClassRec;

/* For future self-solver */
extern int move_triangles_dir();
/*extern void solve_tiles();*/
extern void draw_all_tiles();
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
extern int row();
extern int trbl();
extern int tlbr();

#endif /* _TrianglesP_h */
