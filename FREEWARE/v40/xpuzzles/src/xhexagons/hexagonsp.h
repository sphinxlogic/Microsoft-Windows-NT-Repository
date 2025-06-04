/*
# X-BASED HEXAGONS
#
#  HexagonP.h
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

/* Private header file for Hexagons */

#ifndef _HexagonsP_h
#define _HexagonsP_h

#include "Hexagons.h"

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
#define LOW 0
#define HIGH 1
#define SPACES 2

/* The following 5 are in xhexagons.c also */
#define MINHEXAGONS 1
#define MAXHEXAGONS 12 
#define NOCORN 0
#define CORNERS 1
#define MAXORIENT 2

#define DEFAULTCORNERS CORNERS
#define DEFAULTHEXAGONS 3
#define MAXHEXAGONS_SIZE (3*MAXHEXAGONS*(MAXHEXAGONS-1)+1)

#define ABS(a) (((a)<0)?(-a):(a))
#define SIGN(a) (((a)<0)?(-1):1)
#define MIN(a,b) (((int)(a)<(int)(b))?(int)(a):(int)(b))
#define MAX(a,b) (((int)(a)>(int)(b))?(int)(a):(int)(b))

typedef struct _HexagonsPart
{
  Pixel foreground;
  Pixel tile_color;
  int tile_of_position[MAXHEXAGONS_SIZE], space_position[SPACES];
  int space_row[ROWTYPES];
  int current_position, current_row[ROWTYPES], current_position_orient;
  Boolean started, vertical, corners;
  int base;
  int size, size_size, size_center;
  Position wid, ht;
  Position tile_width, tile_height;
  Position puzzle_width, puzzle_height;
  XPoint delta, puzzle_offset, digit_offset;
  GC puzzle_GC;
  GC tile_GC;
  GC inverse_GC;
  XtCallbackList select;
} HexagonsPart;

typedef struct _HexagonsRec
{
  CorePart core;
  HexagonsPart hexagons;
} HexagonsRec;

/* This gets around C's inability to do inheritance */
typedef struct _HexagonsClassPart
{
  int ignore;
} HexagonsClassPart;

typedef struct _HexagonsClassRec
{
  CoreClassPart core_class;
  HexagonsClassPart hexagons_class;
} HexagonsClassRec;

extern HexagonsClassRec hexagonsClassRec;

/* For future self-solver */
extern int move_hexagons_dir();
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

#endif /* _HexagonsP_h */
