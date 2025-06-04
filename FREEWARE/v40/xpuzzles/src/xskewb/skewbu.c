/*
# X-BASED SKEWB
#
#  SkewbU.c
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
#include "SkewbP.h"
#include "Skewb2dP.h"
#include "Skewb3dP.h"

typedef struct _MoveRecord
{
/*  int face, corner, direction, control; */
  unsigned long int packed; /* This makes assumptions on the data */
} MoveRecord;

typedef struct _MoveStack
{
  MoveRecord move;
  struct _MoveStack *previous, *next;
} MoveStack;

static MoveStack *currmove, *lastmove, *firstmove;
static int count;
static SkewbLoc start_loc[MAXFACES][MAXCUBES];

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

static void write_move(move, face, corner, direction, control)
  MoveRecord *move;
  int face, corner, direction, control;
{
/*  move.face = face; move.corner = corner;
  move.direction = direction; move.control = control; */
  move->packed = ((control & 0xF) << 12) + ((direction & 0xF) << 8) +
    ((corner & 0xF) << 4) + (face & 0xF);
}

static void read_move(face, corner, direction, control, move)
  int *face, *corner, *direction, *control;
  MoveRecord move;
{
  /* *face = move.face; *corner = move.corner;
  *direction = move.direction; *control = move.control; */
  *face = move.packed & 0xF;
  *corner = (move.packed >> 4) & 0xF;
  *direction = (move.packed >> 8) & 0xF;
  *control = (move.packed >> 12) & 0xF;
}

void put_move(face, corner, direction, control)
  int face, corner, direction, control;
{
  MoveRecord move;

  write_move(&move, face, corner, direction, control);
  push_stack(move);
}

void get_move(face, corner, direction, control)
  int *face, *corner, *direction, *control;
{
  MoveRecord move;

  pop_stack(&move);
  read_move(face, corner, direction, control, move);
}

int moves_made()
{
  return !empty_stack();
}

void flush_moves(w)
  SkewbWidget w;
{
  int face, corner;

  flush_stack();
  for (face = 0; face < MAXFACES; face++)
    for (corner = 0; corner < MAXCUBES; corner++) {
      start_loc[face][corner].face = w->skewb.cube_loc[face][corner].face;
      start_loc[face][corner].rotation =
        w->skewb.cube_loc[face][corner].rotation;
    }
}

int num_moves()
{
  return count;
}

void scan_moves(fp, w, moves)
  FILE *fp;
  SkewbWidget w;
  int moves;
{
  int face, corner, direction, control, k;
  char c;

  for (k = 0; k < moves; k++) {
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d %d %d %d", &face, &corner, &direction, &control);
    move_skewb(w, face, corner, direction, control);
  }
}

void print_moves(fp)
  FILE *fp;
{
  int face, corner, direction, control, counter = 0;

  currmove = firstmove->next;
  (void) fprintf(fp, "moves\tface\tcorner\tdir\tcon\n"); 
  while (currmove != lastmove) {
    read_move(&face, &corner, &direction, &control, currmove->move);
    (void) fprintf(fp, "%d:\t%d\t%d\t%d\t%d\n",
      ++counter, face, corner, direction, control); 
    currmove = currmove->next;
  }
}

void scan_start_position(fp, w)       
  FILE *fp;
  SkewbWidget w;
{
  int face, corner, num;
  char c;

  while ((c = getc(fp)) != EOF && c != ':');
  for (face = 0; face < MAXFACES; face++)
    for (corner = 0; corner < MAXCUBES; corner++) {
      (void) fscanf(fp, "%d ", &num);
      start_loc[face][corner].face = num;
      if (w->skewb.orient) {
        (void) fscanf(fp, "%d ", &num);
        start_loc[face][corner].rotation = num;
      }
    }
}

void print_start_position(fp, w)       
  FILE *fp;
  SkewbWidget w;
{
  int face, corner;

  (void) fprintf(fp, "\nstarting position:\n");
  for (face = 0; face < MAXFACES; face++) {
    for (corner = 0; corner < MAXCUBES; corner++) {
      (void) fprintf(fp, "%d ", start_loc[face][corner].face);
      if (w->skewb.orient)
        (void) fprintf(fp, "%d  ", start_loc[face][corner].rotation);
    }
    (void) fprintf(fp, "\n");
  }
}

void set_start_position(w)       
  SkewbWidget w;
{
  int face, corner;

  for (face = 0; face < MAXFACES; face++)
    for (corner = 0; corner < MAXCUBES; corner++) {
      w->skewb.cube_loc[face][corner].face = start_loc[face][corner].face;
      if (w->skewb.orient)
         w->skewb.cube_loc[face][corner].rotation =
           start_loc[face][corner].rotation;
    }
  draw_all_polyhedrons(w);
}
