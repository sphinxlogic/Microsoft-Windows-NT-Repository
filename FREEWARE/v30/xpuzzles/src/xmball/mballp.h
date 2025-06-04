/*
# X-BASED MASTERBALL(tm)
#
#  MballP.h
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

/* Private header file for Mball */

#ifndef _MballP_h
#define _MballP_h

#include "Mball.h"

/*** random number generator ***/
/* insert your favorite */
extern void SetRNG();
extern long LongRNG();
#define SRAND(X) SetRNG((long) X)
#define LRAND() LongRNG()
 
#define NRAND(X) ((int)(LRAND()%(X)))

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
#define CUTS (COORD / 2)

/* The following 4 are in xmball.c also */
#define MINWEDGES 2
#define MAXWEDGES 8
#define MINRINGS 1
#define MAXRINGS 6

#define DEFAULTWEDGES 8
#define DEFAULTRINGS 4
#define SAME 0
#define OPPOSITE 1
#define DOWN 0
#define UP 1
#define MAXVIEWS 2

#define NUM_DEGREES 360
#define ST_ANG  (NUM_DEGREES/2)
#define RT_ANG  (NUM_DEGREES/4)
#define DEGREES(x) ((x)/M_PI*(double)ST_ANG)
#define RADIANS(x) (M_PI*(x)/(double)ST_ANG)
#define MULT 64
#define CIRCLE (NUM_DEGREES*MULT)
#define CIRCLE_2  (CIRCLE/2)
#define CIRCLE_4  (CIRCLE/4)

#define ABS(a) (((a)<0)?(-a):(a))
#define SIGN(a) (((a)<0)?(-1):1)
#define MIN(a,b) (((int)(a)<(int)(b))?(int)(a):(int)(b))
#define MAX(a,b) (((int)(a)>(int)(b))?(int)(a):(int)(b))

typedef struct _MballLoc
{
  int wedge, direction;
} MballLoc;
 
typedef struct _MballPart
{
  Pixel foreground;
  Pixel wedge_color[MAXWEDGES];
  MballLoc mball_loc[MAXWEDGES][MAXRINGS];
  Boolean started, practice, orient, vertical, mono;
  int depth;
  int wedges, rings;
  Position delta, dr;
  Position mball_length;
  Position wedge_length;
  Position view_length, view_middle;
  Position puzzle_width, puzzle_height;
  XPoint puzzle_offset, letter_offset;
  GC puzzle_GC;
  GC wedge_GC[MAXWEDGES];
  GC inverse_GC;
  String wedge_name[MAXWEDGES];
  XtCallbackList select;
} MballPart;

typedef struct _MballRec
{
  CorePart core;
  MballPart mball;
} MballRec;

/* This gets around C's inability to do inheritance */
typedef struct _MballClassPart
{
  int ignore;
} MballClassPart;

typedef struct _MballClassRec
{
  CoreClassPart core_class;
  MballClassPart mball_class;
} MballClassRec;

extern MballClassRec mballClassRec;

/* For future self-solver */
extern void move_mball();
/*extern void solve_wedges();*/
extern void draw_all_wedges();
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

#endif /* _MballP_h */
