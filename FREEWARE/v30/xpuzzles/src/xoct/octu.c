/*
# X-BASED OCTAHEDRON
#
#  OctU.c
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
#include "OctP.h"

typedef struct _MoveRecord
{
/*  int i, j, k, direction, style, control; */
  unsigned long int packed; /* This makes assumptions on the data */
} MoveRecord;

typedef struct _MoveStack
{
  MoveRecord move;
  struct _MoveStack *previous, *next;
} MoveStack;

static MoveStack *currmove, *lastmove, *firstmove;
static int count;
static OctLoc start_loc[MAXFACES][MAXOCTAS_SQ];

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

static void write_move(move, face, i, j, k, direction, style, control)
  MoveRecord *move;
  int face, i, j, k, direction, style, control;
{
/*  move.face = face; move.i = i; move.j = j; move.k = k;
  move.direction = direction; move.control = control; */
  move->packed = ((control & 0xF) << 24) + ((style & 0xF) << 20) +
    ((direction & 0xF) << 16) + ((k & 0xF) << 12) +
    ((j & 0xF) << 8) + ((i & 0xF) << 4) + (face & 0xF);
}

static void read_move(face, i, j, k, direction, style, control, move)
  int *face, *i, *j, *k, *direction, *style, *control;
  MoveRecord move;
{
  /* *face = move.face; *i = move.i; *j = move.j;
  *k = move.k; *direction = move.direction;
  *style = move.style; *control = move.control; */
  *face = move.packed & 0xF;
  *i = (move.packed >> 4) & 0xF;
  *j = (move.packed >> 8) & 0xF;
  *k = (move.packed >> 12) & 0xF;
  *direction = (move.packed >> 16) & 0xF;
  *style = (move.packed >> 20) & 0xF;
  *control = (move.packed >> 24) & 0xF;
}

void put_move(face, i, j, k, direction, style, control)
  int face, i, j, k, direction, style, control;
{
  MoveRecord move;

  write_move(&move, face, i, j, k, direction, style, control);
  push_stack(move);
}

void get_move(face, i, j, k, direction, style, control)
  int *face, *i, *j, *k, *direction, *style, *control;
{
  MoveRecord move;

  pop_stack(&move);
  read_move(face, i, j, k, direction, style, control, move);
}

int moves_made()
{
  return !empty_stack();
}

void flush_moves(w)
  OctWidget w;
{
  int face, i;

  flush_stack();
  for (face = 0; face < MAXFACES; face++)
    for (i = 0; i < w->oct.size_size; i++) {
      start_loc[face][i].face =
        w->oct.octa_loc[face][i].face;
      start_loc[face][i].rotation =
        w->oct.octa_loc[face][i].rotation;
    }
}

int num_moves()
{
  return count;
}

void scan_moves(fp, w, moves)
  FILE *fp;
  OctWidget w;
  int moves;
{
  int face, i, j, k, direction, style, control, l;
  char c;

  for (l = 0; l < moves; l++) {
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d %d %d %d %d %d %d",
      &face, &i, &j, &k, &direction, &style, &control);
    move_oct(w, face / MAXSIDES, face % MAXSIDES, i, j, k,
      direction, style, control);
  }
}

void print_moves(fp)
  FILE *fp;
{
  int face, i, j, k, direction, style, control, counter = 0;

  currmove = firstmove->next;
  (void) fprintf(fp, "moves\tface\ti\tj\tk\tdir\tstyle\tcon\n"); 
  while (currmove != lastmove) {
    read_move(&face, &i, &j, &k, &direction, &style, &control,
      currmove->move);
    (void) fprintf(fp, "%d:\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
      ++counter, face, i, j, k, direction, style, control); 
    currmove = currmove->next;
  }
}

void scan_start_position(fp, w)       
  FILE *fp;
  OctWidget w;
{
  int face, i, num;
  char c;

  while ((c = getc(fp)) != EOF && c != ':');
  for (face = 0; face < MAXFACES; face++)
    for (i = 0; i < w->oct.size_size; i++) {
      (void) fscanf(fp, "%d ", &num);
      start_loc[face][i].face = num;
      if (w->oct.orient) {
        (void) fscanf(fp, "%d ", &num);
        start_loc[face][i].rotation = num;
      }
    }
}

void print_start_position(fp, w)       
  FILE *fp;
  OctWidget w;
{
  int face, i;

  (void) fprintf(fp, "\nstarting position:\n");
  for (face = 0; face < MAXFACES; face++) {
    for (i = 0; i < w->oct.size_size; i++) {
      (void) fprintf(fp, "%d ", start_loc[face][i].face);
      if (w->oct.orient)
        (void) fprintf(fp, "%d  ", start_loc[face][i].rotation);
    }
    (void) fprintf(fp, "\n");
  }
}

void set_start_position(w)       
  OctWidget w;
{
  int face, i;

  for (face = 0; face < MAXFACES; face++)
    for (i = 0; i < w->oct.size_size; i++) {
      w->oct.octa_loc[face][i].face =
        start_loc[face][i].face;
      if (w->oct.orient)
        w->oct.octa_loc[face][i].rotation =
          start_loc[face][i].rotation;
    }
  draw_all_polyhedrons(w);
}
