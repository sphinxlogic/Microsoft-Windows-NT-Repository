/*
# X-BASED OCTAHEDRON
#
#  OctP.h
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

/* Private header file for Oct */

#ifndef _OctP_h
#define _OctP_h

#include "Oct.h"

/*** random number generator ***/
/* insert your favorite */
extern void SetRNG();
extern long LongRNG();
#define SRAND(X) SetRNG((long) X)
#define LRAND() LongRNG()
 
#define NRAND(X) ((int)(LRAND()%(X)))

#define IGNORE (-1)
#define TOP 0
#define TR 4
#define RIGHT 1
#define BR 5
#define BOTTOM 2
#define BL 6
#define LEFT 3
#define TL 7
#define COORD 8
#define CW 9
#define CCW 15

/* The following 6 are in xoct.c also */
#define MINOCTAS 1
#define MAXOCTAS 6
#define PERIOD3 3
#define PERIOD4 4
#define BOTH 5
#define MAXMODES 3

#define DEFAULTMODE PERIOD4
#define DEFAULTOCTAS 3
#define MAXOCTAS_SQ (MAXOCTAS*MAXOCTAS)
#define MAXFACES 8
#define SAME 0
#define OPPOSITE 1
#define DOWN 0
#define UP 1
#define MAXVIEWS 2
#define MAXSIDES (MAXFACES/MAXVIEWS) 
#define MAXORIENT (3*MAXSIDES)

#define ABS(a) (((a)<0)?(-a):(a))
#define SIGN(a) (((a)<0)?(-1):1)
#define MIN(a,b) (((int)(a)<(int)(b))?(int)(a):(int)(b))
#define MAX(a,b) (((int)(a)>(int)(b))?(int)(a):(int)(b))

typedef struct _OctLoc
{
  int face, rotation;
} OctLoc;
 
typedef struct _OctPart
{
  Pixel foreground;
  Pixel face_color[MAXFACES];
  OctLoc octa_loc[MAXFACES][MAXOCTAS_SQ];
  OctLoc face_loc[MAXOCTAS_SQ];
  OctLoc row_loc[MAXORIENT/2][2*MAXOCTAS-1];
  Boolean started, practice, orient, vertical, mono, sticky;
  int mode, depth;
  int size, size_size;
  Position delta;
  Position octa_length;
  Position face_length;
  Position view_length, view_middle;
  Position puzzle_width, puzzle_height;
  Position orient_line_length;
  XPoint puzzle_offset;
  GC puzzle_GC;
  GC face_GC[MAXFACES];
  GC inverse_GC;
  String face_name[MAXFACES];
  XtCallbackList select;
} OctPart;

typedef struct _OctRec
{
  CorePart core;
  OctPart oct;
} OctRec;

/* This gets around C's inability to do inheritance */
typedef struct _OctClassPart
{
  int ignore;
} OctClassPart;

typedef struct _OctClassRec
{
  CoreClassPart core_class;
  OctClassPart oct_class;
} OctClassRec;

extern OctClassRec octClassRec;

/* For future self-solver */
extern void move_oct();
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

#endif /* _OctP_h */
