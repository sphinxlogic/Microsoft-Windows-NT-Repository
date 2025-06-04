/*
# X-BASED RUBIK'S CUBE(tm)
#
#  RubikU.c
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

/* Undo algorithm */

#include <stdio.h>
#include <stdlib.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include "RubikP.h"
#include "Rubik2dP.h"
#include "Rubik3dP.h"

typedef struct _MoveRecord
{
/*  int face, i, j, direction, control; */
  unsigned long int packed; /* This makes assumptions on the data */
} MoveRecord;

typedef struct _MoveStack
{
  MoveRecord move;
  struct _MoveStack *previous, *next;
} MoveStack;

static MoveStack *currmove, *lastmove, *firstmove;
static int count;
static RubikLoc start_loc[MAXFACES][MAXCUBES][MAXCUBES];

static void init_stack();
static void push_stack();
static void pop_stack();
static int empty_stack();
static void flush_stack();

static void init_stack()
{
  lastmove = (MoveStack *) malloc (sizeof (MoveStack));
  firstmove = (MoveStack *) malloc (sizeof (MoveStack));
  firstmove->previous = lastmove->next = NULL;
  firstmove->next = lastmove;
  lastmove->previous = firstmove;
  count = 0;
}

static void push_stack(move)
  MoveRecord move;
{
  currmove = (MoveStack *) malloc (sizeof (MoveStack));
  lastmove->previous->next = currmove;
  currmove->previous = lastmove->previous;
  currmove->next = lastmove;
  lastmove->previous = currmove;
  currmove->move = move;
  count++;
}

static void pop_stack(move)
  MoveRecord *move;
{
  *move = lastmove->previous->move;
  currmove = lastmove->previous;
  lastmove->previous->previous->next = lastmove;
  lastmove->previous = lastmove->previous->previous;
  (void) free((void *) currmove);
  count--;
}

static int empty_stack()
{
  return (lastmove->previous == firstmove);
}

static void flush_stack()
{
  while (lastmove->previous != firstmove) {
    currmove = lastmove->previous;
    lastmove->previous->previous->next = lastmove;
    lastmove->previous = lastmove->previous->previous;
    (void) free((void *) currmove);
  }
  count = 0;
}

/**********************************/

void init_moves()
{
  init_stack();
}

static void write_move(move, face, i, j, direction, control)
  MoveRecord *move;
  int face, i, j, direction, control;
{
/*  move.face = face; move.i = i; move.j = j;
  move.direction = direction; move.control = control; */
  move->packed = ((control & 0xF) << 16) + ((direction & 0xF) << 12) +
    ((j & 0xF) << 8) + ((i & 0xF) << 4) + (face & 0xF);
}

static void read_move(face, i, j, direction, control, move)
  int *face, *i, *j, *direction, *control;
  MoveRecord move;
{
  /* *face = move.face; *i = move.i; *j = move.j;
  *direction = move.direction; *control = move.control; */
  *face = move.packed & 0xF;
  *i = (move.packed >> 4) & 0xF;
  *j = (move.packed >> 8) & 0xF;
  *direction = (move.packed >> 12) & 0xF;
  *control = (move.packed >> 16) & 0xF;
}

void put_move(face, i, j, direction, control)
  int face, i, j, direction, control;
{
  MoveRecord move;

  write_move(&move, face, i, j, direction, control);
  push_stack(move);
}

void get_move(face, i, j, direction, control)
  int *face, *i, *j, *direction, *control;
{
  MoveRecord move;

  pop_stack(&move);
  read_move(face, i, j, direction, control, move);
}

int moves_made()
{
  return !empty_stack();
}

void flush_moves(w)
  RubikWidget w;
{
  int face, i, j;

  flush_stack();
  for (face = 0; face < MAXFACES; face++)
    for (i = 0; i < w->rubik.size; i++)
      for (j = 0; j < w->rubik.size; j++) {
        start_loc[face][i][j].face =
          w->rubik.cube_loc[face][i][j].face;
        start_loc[face][i][j].rotation =
          w->rubik.cube_loc[face][i][j].rotation;
      }
}

int num_moves()
{
  return count;
}

void scan_moves(fp, w, moves)
  FILE *fp;
  RubikWidget w;
  int moves;
{
  int face, i, j, direction, control, k;
  char c;

  for (k = 0; k < moves; k++) {
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d %d %d %d %d", &face, &i, &j, &direction, &control);
    move_rubik(w, face, i, j, direction, control);
  }
}

void print_moves(fp)
  FILE *fp;
{
  int face, i, j, direction, control, counter = 0;

  currmove = firstmove->next;
  (void) fprintf(fp, "moves\tface\ti\tj\tdir\tcon\n"); 
  while (currmove != lastmove) {
    read_move(&face, &i, &j, &direction, &control, currmove->move);
    (void) fprintf(fp, "%d:\t%d\t%d\t%d\t%d\t%d\n",
      ++counter, face, i, j, direction, control); 
    currmove = currmove->next;
  }
}

void scan_start_position(fp, w)       
  FILE *fp;
  RubikWidget w;
{
  int face, i, j, num;
  char c;

  while ((c = getc(fp)) != EOF && c != ':');
  for (face = 0; face < MAXFACES; face++)
    for (j = 0; j < w->rubik.size; j++)
      for (i = 0; i < w->rubik.size; i++) {
        (void) fscanf(fp, "%d ", &num);
        start_loc[face][i][j].face = num;
        if (w->rubik.orient) {
          (void) fscanf(fp, "%d ", &num);
          start_loc[face][i][j].rotation = num;
        }
      }
}

void print_start_position(fp, w)       
  FILE *fp;
  RubikWidget w;
{
  int face, i, j;

  (void) fprintf(fp, "\nstarting position:\n");
  for (face = 0; face < MAXFACES; face++) {
    for (j = 0; j < w->rubik.size; j++) {
      for (i = 0; i < w->rubik.size; i++) {
        (void) fprintf(fp, "%d ", start_loc[face][i][j].face);
        if (w->rubik.orient)
          (void) fprintf(fp, "%d  ", start_loc[face][i][j].rotation);
      }
      (void) fprintf(fp, "\n");
    }
    (void) fprintf(fp, "\n");
  }
}

void set_start_position(w)       
  RubikWidget w;
{
  int face, i, j;

  for (face = 0; face < MAXFACES; face++)
    for (j = 0; j < w->rubik.size; j++)
      for (i = 0; i < w->rubik.size; i++) {
        w->rubik.cube_loc[face][i][j].face = start_loc[face][i][j].face;
        if (w->rubik.orient)
           w->rubik.cube_loc[face][i][j].rotation =
             start_loc[face][i][j].rotation;
      }
  draw_all_polyhedrons(w);
}
