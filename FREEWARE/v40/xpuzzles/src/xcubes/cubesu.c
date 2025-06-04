/*
# X-BASED CUBES
#
#  CubesU.c
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
#include "CubesP.h"

typedef struct _MoveRecord
{
  int direction;
} MoveRecord;

typedef struct _MoveStack
{
  MoveRecord move;
  struct _MoveStack *previous, *next;
} MoveStack;

static MoveStack *currmove, *lastmove, *firstmove;
static int count;
static int start_position[MAXCUBES_CB];
static int start_space;

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

void put_move(direction)
  int direction;
{
  MoveRecord move;

  move.direction = direction;
  push_stack(move);
}

void get_move(direction)
  int *direction;
{
  MoveRecord move;

  pop_stack(&move);
  *direction = move.direction;
}

int moves_made()
{
  return !empty_stack();
}

void flush_moves(w)
  CubesWidget w;
{
  int i;

  flush_stack();
  start_space = w->cubes.space_position;
  start_position[w->cubes.size_size - 1] = 0;
  for (i = 0; i < w->cubes.size_size; i++)
    start_position[i] = w->cubes.brick_of_position[i];
}

int num_moves()
{
  return count;
}

void scan_moves(fp, w, moves)
  FILE *fp;
  CubesWidget w;
  int moves;
{
  int direction, l;
  char c;

  for (l = 0; l < moves; l++) {
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &direction);
    if (!move_cubes_dir(w, direction))
      (void) fprintf(fp, "%d move in direction %d, can not be made.",
        l, direction);
  }
}

void print_moves(fp)
  FILE *fp;
{
  int direction, counter = 0;

  currmove = firstmove->next;
  (void) fprintf(fp, "moves\tdir\n"); 
  while (currmove != lastmove) {
    direction = currmove->move.direction;
    (void) fprintf(fp, "%d:\t%d\n", ++counter, direction); 
    currmove = currmove->next;
  }
}

void scan_start_position(fp, w)       
  FILE *fp;
  CubesWidget w;
{
  int i, num;
  char c;

  while ((c = getc(fp)) != EOF && c != ':');
  for (i = 0; i < w->cubes.size_size; i++) {
    (void) fscanf(fp, "%d ", &num);
    start_position[i] = num;
    if (!num)
      start_space = i;
  }
}

void print_start_position(fp, w)       
  FILE *fp;
  CubesWidget w;
{
  int i;

  (void) fprintf(fp, "\nstarting position:\n");
  for (i = 0; i < w->cubes.size_size; i++) {
    (void) fprintf(fp, "%3d ", start_position[i]);
    if (!((i + 1) % w->cubes.size_x))
      (void) fprintf(fp, "\n");
    if (!((i + 1) % w->cubes.size_rect))
      (void) fprintf(fp, "\n");
  }
  (void) fprintf(fp, "\n");
}

void set_start_position(w)       
  CubesWidget w;
{
  int i;

  w->cubes.space_position = start_space;
  for (i = 0; i < w->cubes.size_size; i++)
    w->cubes.brick_of_position[i] = start_position[i];
  draw_all_bricks(w, w->cubes.brick_GC);
}
