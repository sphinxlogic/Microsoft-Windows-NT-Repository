/*
# X-BASED MASTERBALL(tm)
#
#  MballU.c
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
#include "MballP.h"

typedef struct _MoveRecord
{
/*  int wedge, ring, direction, control; */
  unsigned long int packed; /* This makes assumptions on the data */
} MoveRecord;

typedef struct _MoveStack
{
  MoveRecord move;
  struct _MoveStack *previous, *next;
} MoveStack;

static MoveStack *currmove, *lastmove, *firstmove;
static int count;
static MballLoc start_loc[MAXWEDGES][MAXRINGS];

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

static void write_move(move, wedge, ring, direction, control)
  MoveRecord *move;
  int wedge, ring, direction, control;
{
  /* move.wedge = wedge; move.ring = ring; move.direction = direction;
     move.control = control; */
  move->packed = ((control & 0xF) << 12) + ((direction & 0xF) << 8) +
    ((ring & 0xF) << 4) + (wedge & 0xF);
}

static void read_move(wedge, ring, direction, control, move)
  int *wedge, *ring, *direction, *control;
  MoveRecord move;
{
  /* *wedge = move.wedge; *ring = move.ring; *direction = move.direction;
  *control = move.control; */
  *wedge = move.packed & 0xF;
  *ring = (move.packed >> 4) & 0xF;
  *direction = (move.packed >> 8) & 0xF;
  *control = (move.packed >> 12) & 0xF;
}

void put_move(wedge, ring, direction, control)
  int wedge, ring, direction, control;
{
  MoveRecord move;

  write_move(&move, wedge, ring, direction, control);
  push_stack(move);
}

void get_move(wedge, ring, direction, control)
  int *wedge, *ring, *direction, *control;
{
  MoveRecord move;

  pop_stack(&move);
  read_move(wedge, ring, direction, control, move);
}

int moves_made()
{
  return !empty_stack();
}

void flush_moves(w)
  MballWidget w;
{
  int wedge, ring;

  flush_stack();
  for (wedge = 0; wedge < w->mball.wedges; wedge++)
    for (ring = 0; ring < w->mball.rings; ring++) {
      start_loc[wedge][ring].wedge =
        w->mball.mball_loc[wedge][ring].wedge;
      start_loc[wedge][ring].direction =
        w->mball.mball_loc[wedge][ring].direction;
    }
}

int num_moves()
{
  return count;
}

void scan_moves(fp, w, moves)
  FILE *fp;
  MballWidget w;
  int moves;
{
  int wedge, ring, direction, control, l;
  char c;

  for (l = 0; l < moves; l++) {
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d %d %d %d", &wedge, &ring, &direction, &control);
    move_mball(w, wedge, ring, direction, control);
  }
}

void print_moves(fp)
  FILE *fp;
{
  int wedge, ring, direction, control, counter = 0;

  currmove = firstmove->next;
  (void) fprintf(fp, "moves\twedge\tring\tdir\tcon\n"); 
  while (currmove != lastmove) {
    read_move(&wedge, &ring, &direction, &control, currmove->move);
    (void) fprintf(fp, "%d:\t%d\t%d\t%d\t%d\n",
      ++counter, wedge, ring, direction, control); 
    currmove = currmove->next;
  }
}

void scan_start_position(fp, w)       
  FILE *fp;
  MballWidget w;
{
  int wedge, ring, num;
  char c;

  while ((c = getc(fp)) != EOF && c != ':');
  for (wedge = 0; wedge < w->mball.wedges; wedge++)
    for (ring = 0; ring < w->mball.rings; ring++) {
      (void) fscanf(fp, "%d ", &num);
      start_loc[wedge][ring].wedge = num;
      if (w->mball.orient) {
        (void) fscanf(fp, "%d ", &num);
        start_loc[wedge][ring].direction = num;
      }
    }
}

void print_start_position(fp, w)       
  FILE *fp;
  MballWidget w;
{
  int wedge, ring;

  (void) fprintf(fp, "\nstarting position:\n");
  for (wedge = 0; wedge < w->mball.wedges; wedge++) {
    for (ring = 0; ring < w->mball.rings; ring++) {
      (void) fprintf(fp, "%d ", start_loc[wedge][ring].wedge);
      if (w->mball.orient)
        (void) fprintf(fp, "%d  ", start_loc[wedge][ring].direction);
    }
    (void) fprintf(fp, "\n");
  }
}

void set_start_position(w)       
  MballWidget w;
{
  int wedge, ring;

  for (wedge = 0; wedge < w->mball.wedges; wedge++)
    for (ring = 0; ring < w->mball.rings; ring++) {
      w->mball.mball_loc[wedge][ring].wedge =
        start_loc[wedge][ring].wedge;
      if (w->mball.orient)
        w->mball.mball_loc[wedge][ring].direction =
          start_loc[wedge][ring].direction;
    }
  draw_all_wedges(w);
}
