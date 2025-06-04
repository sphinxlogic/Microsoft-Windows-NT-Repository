/*
# X-BASED HEXAGONS
#
#  HexagonsU.c
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
#include "HexagonsP.h"

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
static int start_position[MAXHEXAGONS_SIZE];
static int start_space[SPACES];
static int start_row[ROWTYPES];

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
  HexagonsWidget w;
{
  int i;

  flush_stack();
  start_space[LOW] = w->hexagons.space_position[LOW];
  start_space[HIGH] = w->hexagons.space_position[HIGH];
  start_row[TRBL] = w->hexagons.space_row[TRBL];
  start_row[TLBR] = w->hexagons.space_row[TLBR];
  start_row[ROW] = w->hexagons.space_row[ROW];
  for (i = 0; i < w->hexagons.size_size; i++)
    start_position[i] = w->hexagons.tile_of_position[i];
}

int num_moves()
{
  return count;
}

void scan_moves(fp, w, moves)
  FILE *fp;
  HexagonsWidget w;
  int moves;
{
  int direction, l;
  char c;

  for (l = 0; l < moves; l++) {
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d", &direction);
    if (!move_hexagons_dir(w, direction))
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
  HexagonsWidget w;
{
  int i, num;
  char c;

  while ((c = getc(fp)) != EOF && c != ':');
  for (i = 0; i < w->hexagons.size_size; i++) {
    (void) fscanf(fp, "%d ", &num);
    start_position[i] = num;
    if (!num)
      start_space[HIGH] = i;
    else if (num == -1)
      start_space[LOW] = i;
  }
  if (w->hexagons.corners) {
    start_row[HIGH] = row(w, start_space[HIGH]);
    if (w->hexagons.size > 1)
      start_row[LOW] = row(w, start_space[LOW]);
  } else {
    start_row[ROW] = row(w, start_space[HIGH]);
    start_row[TRBL] = trbl(w, start_space[HIGH], start_row[ROW]);
    start_row[TLBR] = tlbr(w, start_space[HIGH], start_row[ROW]);
  }
}

void print_start_position(fp, w)       
  FILE *fp;
  HexagonsWidget w;
{
  int rowpos = 0, r = 0, rp, p, length = 0;

  (void) fprintf(fp, "\nstarting position:\n");
  for (p = 0; p < w->hexagons.size_size; p++) {
    if (rowpos == 0) {
      if (p < w->hexagons.size_size / 2) /* CENTER */ {
        length = w->hexagons.size + r;
        for (rp = 0; rp < w->hexagons.size - 1 - r; rp++)
          (void) fprintf(fp, "  ");
      } else {
        length = 3 * w->hexagons.size - r - 2;
        for (rp = 0; rp < r - w->hexagons.size + 1; rp++)
          (void) fprintf(fp, "  ");
      }
    }
    (void) fprintf(fp, "%3d ", start_position[p]);
    if (rowpos == length - 1) {
      r++;
      rowpos = 0;
      (void) fprintf(fp, "\n");
    } else
      rowpos++;
  }
  (void) fprintf(fp, "\n");
}

void set_start_position(w)       
  HexagonsWidget w;
{
  int i;

  w->hexagons.space_position[HIGH] = start_space[HIGH];
  w->hexagons.space_position[LOW] = start_space[LOW];
  w->hexagons.space_row[TRBL] = start_row[TRBL];
  w->hexagons.space_row[TLBR] = start_row[TLBR];
  w->hexagons.space_row[ROW] = start_row[ROW];
  for (i = 0; i < w->hexagons.size_size; i++)
    w->hexagons.tile_of_position[i] = start_position[i];
  draw_all_tiles(w, w->hexagons.tile_GC);
}
