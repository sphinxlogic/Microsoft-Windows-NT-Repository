/*
# X-BASED MISSING LINK(tm)
#
#  MlinkU.c
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
#include "MlinkP.h"

typedef struct _MoveRecord
{
  /*int direction, tile, control; */
  unsigned long int packed; /* This makes assumptions on the data */
} MoveRecord;

typedef struct _MoveStack
{
  MoveRecord move;
  struct _MoveStack *previous, *next;
} MoveStack;

static MoveStack *currmove, *lastmove, *firstmove;
static int count;
static int start_position[MAXTILEFACES];
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

static void write_move(move, direction, tile, control)
  MoveRecord *move;
  int direction, tile, control;
{
/*  move.direction = direction; move.tile = tile; move.control = control; */
  move->packed = ((control & 0xF) << 8) + ((tile & 0xF) << 4) +
    (direction & 0xF);
}

static void read_move(direction, tile, control, move)
  int *direction, *tile, *control;
  MoveRecord move;
{
  /* *direction = move.direction; *tile = move.tile; *control = move.control; */
  *direction = move.packed & 0xF;
  *tile = (move.packed >> 4) & 0xF;
  *control = (move.packed >> 8) & 0xF;
}

void put_move(direction, tile, control)
  int direction, tile, control;
{
  MoveRecord move;

  write_move(&move, direction, tile, control);
  push_stack(move);
}

void get_move(direction, tile, control)
  int *direction, *tile, *control;
{
  MoveRecord move;

  pop_stack(&move);
  read_move(direction, tile, control, move);}

int moves_made()
{
  return !empty_stack();
}

void flush_moves(w)
  MlinkWidget w;
{
  int i;

  flush_stack();
  start_space = w->mlink.space_position;
  start_position[w->mlink.tile_faces - 1] = 0;
  for (i = 0; i < w->mlink.tile_faces; i++)
    start_position[i] = w->mlink.tile_of_position[i];
}

int num_moves()
{
  return count;
}

void scan_moves(fp, w, moves)
  FILE *fp;
  MlinkWidget w;
  int moves;
{
  int direction, tile, shift, l;
  char c;

  for (l = 0; l < moves; l++) {
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d %d %d", &direction, &tile, &shift);
    if (!move_mlink(w, direction, tile, shift))
      (void) fprintf(fp,
        "%d move in direction %d, tile %d, shift %d, can not be made.",
        l, direction, tile, shift);
  }
}

void print_moves(fp)
  FILE *fp;
{
  int direction, tile, control, counter = 0;

  currmove = firstmove->next;
  (void) fprintf(fp, "moves\tdir\ttile\tshift\n"); 
  while (currmove != lastmove) {
    read_move(&direction, &tile, &control, currmove->move);
    (void) fprintf(fp, "%d:\t%d\t%d\t%d\n",
      ++counter, direction, tile, control); 
    currmove = currmove->next;
  }
}

void scan_start_position(fp, w)       
  FILE *fp;
  MlinkWidget w;
{
  int i, num;
  char c;

  while ((c = getc(fp)) != EOF && c != ':');
  for (i = 0; i < w->mlink.tile_faces; i++) {
    (void) fscanf(fp, "%d ", &num);
    start_position[i] = num;
    if (!num)
      start_space = i;
  }
}

void print_start_position(fp, w)       
  FILE *fp;
  MlinkWidget w;
{
  int i;

  (void) fprintf(fp, "\nstarting position:\n");
  for (i = 0; i < w->mlink.tile_faces; i++) {
    (void) fprintf(fp, "%3d ", start_position[i]);
    if (!((i + 1) % w->mlink.tiles))
      (void) fprintf(fp, "\n");
  }
  (void) fprintf(fp, "\n");
}

void set_start_position(w)       
  MlinkWidget w;
{
  int i;

  w->mlink.space_position = start_space;
  for (i = 0; i < w->mlink.tile_faces; i++)
    w->mlink.tile_of_position[i] = start_position[i];
  draw_all_tiles(w, w->mlink.tile_GC);
}
