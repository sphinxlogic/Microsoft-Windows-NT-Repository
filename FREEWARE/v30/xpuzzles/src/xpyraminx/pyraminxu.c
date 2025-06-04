/*
# X-BASED PYRAMINX(tm)
#
#  PyraminxU.c
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
#include "PyraminxP.h"

typedef struct _MoveRecord
{
/*  int view, i, j, side, direction, style, control; */
  unsigned long int packed; /* This makes assumptions on the data */
} MoveRecord;

typedef struct _MoveStack
{
  MoveRecord move;
  struct _MoveStack *previous, *next;
} MoveStack;

static MoveStack *currmove, *lastmove, *firstmove;
static int count;
static PyraminxLoc start_loc[MAXVIEWS][MAXTETRAS][MAXTETRAS][MAXSIDES];

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

static void write_move(move, view, i, j, side, direction, style, control)
  MoveRecord *move;
  int view, i, j, side, direction, style, control;
{
/*  move.view = view; move.i = i; move.j = j; move.side = side;
  move.direction = direction; move.control = control; */
  move->packed = ((control & 0xF) << 24) + ((style & 0xF) << 20) +
    ((direction & 0xF) << 16) + ((side & 0xF) << 12) +
    ((j & 0xF) << 8) + ((i & 0xF) << 4) + (view & 0xF);  
}

static void read_move(view, i, j, side, direction, style, control, move)
  int *view, *i, *j, *side, *direction, *style, *control;
  MoveRecord move;
{
  /* *view = move.view; *i = move.i; *j = move.j;
  *side = move.side; *direction = move.direction;
  *style = move.style; *control = move.control; */
  *view = move.packed & 0xF;
  *i = (move.packed >> 4) & 0xF;
  *j = (move.packed >> 8) & 0xF;
  *side = (move.packed >> 12) & 0xF;
  *direction = (move.packed >> 16) & 0xF;
  *style = (move.packed >> 20) & 0xF;
  *control = (move.packed >> 24) & 0xF;
}

void put_move(view, i, j, side, direction, style, control)
  int view, i, j, side, direction, style, control;
{
  MoveRecord move;

  write_move(&move, view, i, j, side, direction, style, control);
  push_stack(move);
}

void get_move(view, i, j, side, direction, style, control)
  int *view, *i, *j, *side, *direction, *style, *control;
{
  MoveRecord move;

  pop_stack(&move);
  read_move(view, i, j, side, direction, style, control, move);
}

int moves_made()
{
  return !empty_stack();
}

void flush_moves(w)
  PyraminxWidget w;
{
  int view, i, j, side;

  flush_stack();
  for (view = 0; view < MAXVIEWS; view++)
    for (i = 0; i < w->pyraminx.size; i++)
      for (j = 0; j < w->pyraminx.size; j++)
        for (side = 0; side < MAXSIDES; side++) {
          start_loc[view][i][j][side].face =
            w->pyraminx.tetra_loc[view][i][j][side].face;
          start_loc[view][i][j][side].rotation =
            w->pyraminx.tetra_loc[view][i][j][side].rotation;
        }
}

int num_moves()
{
  return count;
}

void scan_moves(fp, w, moves)
  FILE *fp;
  PyraminxWidget w;
  int moves;
{
  int view, i, j, side, direction, style, control, k;
  char c;

  for (k = 0; k < moves; k++) {
    while ((c = getc(fp)) != EOF && c != ':');
    (void) fscanf(fp, "%d %d %d %d %d %d %d",
      &view, &i, &j, &side, &direction, &style, &control);
    move_pyraminx(w, view, i, j, side, direction, style, control);
  }
}

void print_moves(fp)
  FILE *fp;
{
  int view, i, j, side, direction, style, control, counter = 0;

  currmove = firstmove->next;
  (void) fprintf(fp, "moves\tview\ti\tj\tside\tdir\tstyle\tcon\n"); 
  while (currmove != lastmove) {
    read_move(&view, &i, &j, &side, &direction, &style, &control,
      currmove->move);
    (void) fprintf(fp, "%d:\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
      ++counter, view, i, j, side, direction, style, control); 
    currmove = currmove->next;
  }
}

void scan_start_position(fp, w)       
  FILE *fp;
  PyraminxWidget w;
{
  int view, i, j, side, num;
  char c;

  while ((c = getc(fp)) != EOF && c != ':');
  for (view = 0; view < MAXVIEWS; view++)
    for (j = 0; j < w->pyraminx.size; j++)
      for (i = 0; i < w->pyraminx.size; i++)
        for (side = 0; side < MAXSIDES; side++) {
          (void) fscanf(fp, "%d ", &num);
          start_loc[view][i][j][side].face = num;
          if (w->pyraminx.orient) {
            (void) fscanf(fp, "%d ", &num);
            start_loc[view][i][j][side].rotation = num;
          }
        }
}

void print_start_position(fp, w)       
  FILE *fp;
  PyraminxWidget w;
{
  int view, i, j, side;

  (void) fprintf(fp, "\nstarting position:\n");
  for (view = 0; view < MAXVIEWS; view++) {
    for (j = 0; j < w->pyraminx.size; j++) {
      for (i = 0; i < w->pyraminx.size; i++)
        for (side = 0; side < MAXSIDES; side++) {
          (void) fprintf(fp, "%d ", start_loc[view][i][j][side].face);
          if (w->pyraminx.orient)
            (void) fprintf(fp, "%d  ", start_loc[view][i][j][side].rotation);
        }
      (void) fprintf(fp, "\n");
    }
    (void) fprintf(fp, "\n");
  }
}

void set_start_position(w)       
  PyraminxWidget w;
{
  int view, i, j, side;

  for (view = 0; view < MAXVIEWS; view++)
    for (j = 0; j < w->pyraminx.size; j++)
      for (i = 0; i < w->pyraminx.size; i++)
        for (side = 0; side < MAXSIDES; side++) {
          w->pyraminx.tetra_loc[view][i][j][side].face =
            start_loc[view][i][j][side].face;
          if (w->pyraminx.orient)
            w->pyraminx.tetra_loc[view][i][j][side].rotation =
              start_loc[view][i][j][side].rotation;
        }
  draw_all_polyhedrons(w);
}
