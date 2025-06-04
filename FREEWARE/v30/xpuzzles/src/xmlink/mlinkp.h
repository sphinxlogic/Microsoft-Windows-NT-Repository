/*
# X-BASED MISSING LINK(tm)
#
#  MlinkP.h
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

/* Private header file for Mlink */

#ifndef _MlinkP_h
#define _MlinkP_h

#include "Mlink.h"

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
#define COORD 4

/* The following 4 are in xmlink.c also */
#define MINTILES 1
#define MAXTILES 8
#define MINFACES 1
#define MAXFACES 8

#define DEFAULTTILES 4
#define DEFAULTFACES 4
#define MAXTILEFACES (MAXTILES*MAXFACES)

#define ABS(a) (((a)<0)?(-a):(a))
#define SIGN(a) (((a)<0)?(-1):1)
#define MIN(a,b) (((int)(a)<(int)(b))?(int)(a):(int)(b))
#define MAX(a,b) (((int)(a)>(int)(b))?(int)(a):(int)(b))

typedef struct _MlinkPart
{
  Pixel foreground;
  Pixel tile_color;
  Pixel face_color[MAXFACES];
  int tile_of_position[MAXTILEFACES], space_position;
  Boolean started, orient, mono, middle;
  int depth;
  int base;
  int tiles, faces, tile_faces;
  Position wid, ht;
  Position tile_width, tile_height;
  Position face_width, face_height;
  Position puzzle_width, puzzle_height;
  XPoint delta, puzzle_offset, digit_offset;
  GC puzzle_GC;
  GC tile_GC;
  GC face_GC[MAXFACES];
  GC inverse_GC;
  String face_name[MAXFACES];
  XtCallbackList select;
} MlinkPart;

typedef struct _MlinkRec
{
  CorePart core;
  MlinkPart mlink;
} MlinkRec;

/* This gets around C's inability to do inheritance */
typedef struct _MlinkClassPart
{
  int ignore;
} MlinkClassPart;

typedef struct _MlinkClassRec
{
  CoreClassPart core_class;
  MlinkClassPart mlink_class;
} MlinkClassRec;

extern MlinkClassRec mlinkClassRec;

/* For future self-solver */
extern int move_mlink();
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

#endif /* _MlinkP_h */
