/*
# X-BASED DINO
#
#  DinoP.h
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

/* Private header file for Dino */

#ifndef _DinoP_h
#define _DinoP_h

#include "Dino.h"

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
#define CORNER 0
#define MIDDLE 1
#define EDGE 2
#define FACE 3

/* The following 5 are in xdino.c also */
#define PERIOD2 2
#define PERIOD3 3
#define BOTH 4
#define MAXMODES 3
#define MAXFACES 6

#define DEFAULTMODE PERIOD3

#define MIN(a,b) (((int)(a)<(int)(b))?(int)(a):(int)(b))
#define MAX(a,b) (((int)(a)>(int)(b))?(int)(a):(int)(b))

typedef struct _DinoLoc
{
  int face, side, dir;
} DinoLoc;

typedef struct _DinoCornerLoc
{
  int face, rotation;
} DinoCornerLoc;

typedef struct _DinoPart
{
  Pixel foreground;
  Pixel face_color[MAXFACES];
  DinoCornerLoc cube_loc[MAXFACES][MAXORIENT];
  DinoCornerLoc face_loc[MAXORIENT];
  DinoCornerLoc row_loc[MAXORIENT][MAXORIENT];
  DinoCornerLoc spindle_loc[MAXROTATE][2];
  Boolean started, practice, orient, vertical, mono;
  int dim; /* This allows us to reuse code between 2d and 3d */
  int mode, depth;
  int face, side, direction, style;
  Position delta;
  Position orient_line_length;
  Position puzzle_width, puzzle_height;
  XPoint puzzle_offset, letter_offset;
  GC puzzle_GC;
  GC face_GC[MAXFACES];
  GC inverse_GC;
  String face_name[MAXFACES];
  XtCallbackList select;
  int dino_loc[MAXFACES][MAXORIENT];
} DinoPart;

typedef struct _DinoRec
{
  CorePart core;
  DinoPart dino;
} DinoRec;

/* This gets around C's inability to do inheritance */
typedef struct _DinoClassPart
{
  int ignore;
} DinoClassPart;

typedef struct _DinoClassRec
{
  CoreClassPart core_class;
  DinoClassPart dino_class;
} DinoClassRec;

typedef struct _RowNext
{
  int face, direction, side_face;
} RowNext;

extern DinoClassRec dinoClassRec;

extern void quit_dino();
extern void practice_dino();
extern void maybe_practice_dino();
extern void randomize_dino();
extern void maybe_randomize_dino();
extern void enter_dino();
extern void write_dino();
extern void undo_dino();
extern void solve_dino();
extern void orientize_dino();
extern void period2_mode_dino();
extern void period3_mode_dino();
extern void both_mode_dino();
extern void move_dino_input();
extern void move_dino();
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

#endif /* _DinoP_h */
