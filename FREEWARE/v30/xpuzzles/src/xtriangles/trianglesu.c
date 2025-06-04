/*
# X-BASED TRIANGLES
#
#  TrianglesU.c
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
#include "TrianglesP.h"

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
static int start_position[MAXTRIANGLES_SQ];
static int start_space[MAXORIENT];
static int start_row[MAXORIENT][ROWTYPES];

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
  TrianglesWidget w;
{
  int i;

  flush_stack();
  start_space[UP] = w->triangles.space_position[UP];
  start_space[DOWN] = w->triangles.space_position[DOWN];
  start_row[UP][TRBL] = w->triangles.space_row[UP][TRBL];
  start_row[UP][TLBR] = w->triangles.space_row[UP][TLBR];
  start_row[UP][ROW] = w->triangles.space_row[UP][ROW];
  start_row[DOWN][TRBL] = w->triangles.space_row[DOWN][TRBL];
  start_row[DOWN][TLBR] = w->triangles.space_row[DOWN][TLBR];
  start_row[DOWN][ROW] = w->triangles.space_row[DOWN][ROW];
  for (i = 0; i < w->triangles.size_size; i++)
    start_position[i] = w->triangles.tile_of_position[i];
}

int num_moves()
{
  return count;
}

void scan_moves(fp, w, moves)
  FILE *fp;
  TrianglesWidget w;
  int moves;
{
  int direction, l;
  char c;

  for (l = 0; l < moves; l++) {
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &direction);
    if (!move_triangles_dir(w, direction))
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
  TrianglesWidget w;
{
  int i, num;
  char c;

  while ((c = getc(fp)) != EOF && c != ':');
  for (i = 0; i < w->triangles.size_size; i++) {
    (void) fscanf(fp, "%d ", &num);
    start_position[i] = num;
    if (!num)
      start_space[UP] = i;
    else if (num == -1)
      start_space[DOWN] = i;
  }
  for (i = DOWN; i <= UP; i++) {
    start_row[i][ROW] = row(start_space[i]);
    start_row[i][TRBL] = trbl(start_space[i], start_row[i][ROW]) / 2;
    start_row[i][TLBR] = tlbr(start_space[i], start_row[i][ROW]) / 2;
  }
}

void print_start_position(fp, w)       
  FILE *fp;
  TrianglesWidget w;
{
  int r = 0, p, space, temp;

  (void) fprintf(fp, "\nstarting position:\n");
  for (p = 0; p < w->triangles.size_size; p++) {
    if (p == r * r)
      for (space = 0; space < w->triangles.size - r - 1; space++)
        (void) fprintf(fp, "    ");
    (void) fprintf(fp, "%3d ", start_position[p]);
    temp = (r + 1) * (r + 1) - 1;
    if (p == temp) {
      (void) fprintf(fp, "\n");
      r++;
    }
  }
  (void) fprintf(fp, "\n");
}

void set_start_position(w)       
  TrianglesWidget w;
{
  int i;

  w->triangles.space_position[UP] = start_space[UP];
  w->triangles.space_position[DOWN] = start_space[DOWN];
  w->triangles.space_row[UP][TRBL] = start_row[UP][TRBL];
  w->triangles.space_row[UP][TLBR] = start_row[UP][TLBR];
  w->triangles.space_row[UP][ROW] = start_row[UP][ROW];
  w->triangles.space_row[DOWN][TRBL] = start_row[DOWN][TRBL];
  w->triangles.space_row[DOWN][TLBR] = start_row[DOWN][TLBR];
  w->triangles.space_row[DOWN][ROW] = start_row[DOWN][ROW];
  for (i = 0; i < w->triangles.size_size; i++)
    w->triangles.tile_of_position[i] = start_position[i];
  draw_all_tiles(w, w->triangles.tile_GC);
}
