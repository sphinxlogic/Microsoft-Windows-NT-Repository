/*
# X-BASED RUBIK'S CUBE(tm)
#
#  RubikS.c
#
###
#
#  Taken from code originally written by 
#  Michael B. Martin <martinm@sps1.phys.vt.edu>
#  From cubist10.c-- for IBM PC.
#  Used by permission.
#  Taken from the algorithm in the Ideal Solution book.
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
 
/* Solver file for Rubik */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include "RubikP.h"
#include "Rubik2dP.h"
#include "Rubik3dP.h"

/* Mappings of Ideal notation to my General nxnxn cube notation */
#define rotate_left(w)	move_rubik(w,2,0,0,LEFT,TRUE)
#define rotate_right(w)	move_rubik(w,2,0,0,RIGHT,TRUE)
#define rotate_up(w)	move_rubik(w,2,0,0,TOP,TRUE)
#define rotate_down(w)	move_rubik(w,2,0,0,BOTTOM,TRUE)
#define rotate_clockwise(w)	move_rubik(w,0,0,0,RIGHT,TRUE)
#define rotate_counterclockwise(w)	move_rubik(w,0,0,0,LEFT,TRUE)

#define rotate_top_left(w)	move_rubik(w,2,0,0,LEFT,FALSE)
#define rotate_center_left(w)	move_rubik(w,2,0,1,LEFT,FALSE)
#define rotate_bottom_left(w)	move_rubik(w,2,0,w->rubik.size-1,LEFT,FALSE)
#define rotate_top_right(w)	move_rubik(w,2,0,0,RIGHT,FALSE)
#define rotate_center_right(w)	move_rubik(w,2,0,1,RIGHT,FALSE)
#define rotate_bottom_right(w)	move_rubik(w,2,0,w->rubik.size-1,RIGHT,FALSE)
#define rotate_left_up(w)	move_rubik(w,2,0,0,TOP,FALSE)
#define rotate_center_up(w)	move_rubik(w,2,1,0,TOP,FALSE)
#define rotate_right_up(w)	move_rubik(w,2,w->rubik.size-1,0,TOP,FALSE)
#define rotate_left_down(w)	move_rubik(w,2,0,0,BOTTOM,FALSE)
#define rotate_center_down(w)	move_rubik(w,2,1,0,BOTTOM,FALSE)
#define rotate_right_down(w)	move_rubik(w,2,w->rubik.size-1,0,BOTTOM,FALSE)
#define rotate_front_clockwise(w)\
	move_rubik(w,0,0,w->rubik.size-1,RIGHT,FALSE)
#define rotate_front_counterclockwise(w)\
	move_rubik(w,0,0,w->rubik.size-1,LEFT,FALSE)

#define BLUE 0
#define WHITE 1
#define RED 2
#define YELLOW 3
#define GREEN 4
#define ORANGE 5

#ifdef DEBUG
static int mapGeneraltoIdeal[MAXFACES]={5, 4, 1, 2, 6, 3};
#endif
static int mapIdealtoGeneral[MAXFACES]={2, 3, 5, 1, 0, 4}; /* Remember to subtract 1 */

static int side();
static int find_corner();
static int find_edge();
static void place_edge();
static void solve_top();
static void align_corners();
static void swap_bottom_corners();
static void solve_bottom();
static void solve_2_2();
static void solve_bottom_edges();
static void align_last_edge();
static void color_align_front();
static void align_edges();
static void shuffle_edges();
static void recolor_top_edges();

static int side(w, m, c)
  RubikWidget w;
  int m, c;
{
  int d, i, j;

  d = mapIdealtoGeneral[m - 1];
  i = c % 3;
  j = c / 3;
  if (i == 2)
    i = w->rubik.size - 1;
  if (j == 2)
    j = w->rubik.size - 1;
  return w->rubik.cube_loc[d][i][j].face;
}

/* This procedure finds the location of a specified corner.  An */
/* improperly-specified color combination will result in a return */
/* value of 9. */
static int find_corner(w, color1, color2, color3)
  RubikWidget w;
  int color1, color2, color3;
{
  int corner = 0, temp = 1;

  do {
    if (side(w, 5, 6) == color1) {
      if (side(w, 1, 0) == color2)
        if (side(w, 4, 2) == color3)
          corner = temp;
    } else if (side(w, 5, 6) == color2) {
      if (side(w, 1, 0) == color3)
        if (side(w, 4, 2) == color1)
          corner = temp;
    } else if (side(w, 5, 6) == color3) {
      if (side(w, 1, 0) == color1)
        if (side(w, 4, 2) == color2)
          corner = temp;
    }

    if (corner == 0) {
      if (temp < 4)
        rotate_left(w);
      else if (temp == 4) {
        rotate_clockwise(w);
        rotate_clockwise(w);
      } else if (temp < 8)
        rotate_right(w);
      else if (temp == 8)
        corner = 9;
      temp++;
    }
  } while (corner == 0);

  /* put the cube back to the way it was */
  if (corner == 2)
     rotate_right(w);
  else if (corner == 3) {
    rotate_right(w);
    rotate_right(w);
  } else if (corner == 4)
    rotate_left(w);
  else if (corner == 5) {
    rotate_clockwise(w);
    rotate_clockwise(w);
    rotate_left(w);
  } else if (corner == 6) {
    rotate_clockwise(w);
    rotate_clockwise(w);
  } else if (corner == 7) {
    rotate_clockwise(w);
    rotate_clockwise(w);
    rotate_right(w);
  } else if (corner == 8) {
    rotate_clockwise(w);
    rotate_clockwise(w);
    rotate_right(w);
    rotate_right(w);
  }
  return(corner);
}

/* This procedure finds the location of a specified edge.  An */
/* improperly-specified color combination will result in a return */
/* value of 13. */
static int find_edge(w, color1, color2)
  RubikWidget w;
  int color1, color2;
{
  int edge = 0, temp = 1;

  do {
    if (side(w, 5, 7) == color1) {
      if (side(w, 1, 1) == color2)
        edge = temp;
    } else if (side(w, 5, 7) == color2) {
      if (side(w, 1, 1) == color1)
        edge = temp;
    }

    if (edge == 0) {
      if (temp < 4)
        rotate_left(w);
      else if (temp == 4) {
        rotate_left(w);
        rotate_clockwise(w);
      } else if (temp < 8)
        rotate_up(w);
      else if (temp == 8) {
        rotate_up(w);
        rotate_clockwise(w);
      } else if (temp < 12)
        rotate_right(w);
      else if (temp == 12)
        edge = 13;  /* end condition, just in case */
      temp++;
    }
  } while (edge == 0);

  /* put the cube back to the way it was */
  if (edge == 2) {
    rotate_right(w);
  } else if (edge == 3) {
    rotate_right(w);
    rotate_right(w);
  } else if (edge == 4) {
    rotate_left(w);
  } else if (edge == 5) {
    rotate_counterclockwise(w);
  } else if (edge == 6) {
    rotate_counterclockwise(w);
    rotate_right(w);
  } else if (edge == 7) {
    rotate_counterclockwise(w);
    rotate_right(w);
    rotate_right(w);
  } else if (edge == 8) {
    rotate_counterclockwise(w);
    rotate_left(w);
  } else if (edge == 9) {
    rotate_counterclockwise(w);
    rotate_counterclockwise(w);
  } else if (edge == 10) {
    rotate_counterclockwise(w);
    rotate_counterclockwise(w);
    rotate_right(w);
  } else if (edge == 11) {
    rotate_up(w);
    rotate_up(w);
  } else if (edge == 12) {
    rotate_up(w);
    rotate_up(w);
    rotate_right(w);
  }
  return(edge);
}

/* This procedure places the specified edge piece in edge position */
/* #1 (front top). */
static void place_edge(w, edge, top_color)
  RubikWidget w;
  int edge, top_color;
{
  /* first put the edge piece in position #8 (center row, left rear) */
  if (edge == 1) {
    if (side(w, 5, 7) == top_color)
      return;  /* already in place */
    else {
      rotate_front_counterclockwise(w);
      rotate_center_left(w);
      rotate_front_clockwise(w);
    }
  } else if (edge == 2) {
    rotate_top_left(w);
    rotate_front_counterclockwise(w);
    rotate_center_left(w);
    rotate_front_clockwise(w);
    rotate_top_right(w);
  } else if (edge == 3) {
    rotate_top_left(w);
    rotate_top_left(w);
    rotate_front_counterclockwise(w);
    rotate_center_left(w);
    rotate_front_clockwise(w);
    rotate_top_right(w);
    rotate_top_right(w);
  } else if (edge == 4) {
    rotate_top_right(w);
    rotate_front_counterclockwise(w);
    rotate_center_left(w);
    rotate_front_clockwise(w);
    rotate_top_left(w);
  } else if (edge == 5) {
    rotate_center_left(w);
  } else if (edge == 6) {
    rotate_center_left(w);
    rotate_center_left(w);
  } else if (edge == 7) {
    rotate_center_right(w);
  } else if (edge == 9) {
    rotate_front_clockwise(w);
    rotate_center_left(w);
    rotate_front_counterclockwise(w);
  } else if (edge == 10) {
    rotate_bottom_left(w);
    rotate_front_clockwise(w);
    rotate_center_left(w);
    rotate_front_counterclockwise(w);
    rotate_bottom_right(w);
  } else if (edge == 11) {
    rotate_bottom_left(w);
    rotate_bottom_left(w);
    rotate_front_clockwise(w);
    rotate_center_left(w);
    rotate_front_counterclockwise(w);
    rotate_bottom_right(w);
    rotate_bottom_right(w);
  } else if (edge == 12) {
    rotate_bottom_right(w);
    rotate_front_clockwise(w);
    rotate_center_left(w);
    rotate_front_counterclockwise(w);
    rotate_bottom_left(w);
  }

  /* put the piece in place */
  if (side(w, 4, 3) == top_color) {
    rotate_front_clockwise(w);
    rotate_center_right(w);
    rotate_center_right(w);
    rotate_front_counterclockwise(w);
  } else {
    rotate_front_counterclockwise(w);
    rotate_center_right(w);
    rotate_front_clockwise(w);
  }
}

/* This procedure solves the top (BLUE) side, except for one edge. */
static void solve_top(w)
  RubikWidget w;
{
  int corner, edge;

  /* put the blue face on the top */
  if (side(w, 1, 4) == BLUE)
    rotate_up(w);
  else if (side(w, 2, 4) == BLUE)
    rotate_counterclockwise(w);
  else if (side(w, 3, 4) == BLUE)
    rotate_down(w);
  else if (side(w, 4, 4) == BLUE)
    rotate_clockwise(w);
  else if (side(w, 6, 4) == BLUE) {
    rotate_up(w);
    rotate_up(w);
  }

  /* first find the blue-red-white corner and place it */
     corner = find_corner(w,BLUE,RED,WHITE);
  if (corner == 1) {
    if (side(w, 5, 6) == RED) {
      rotate_front_clockwise(w);
      rotate_top_left(w);
    } else if (side(w, 5, 6) == WHITE) {
      rotate_left_up(w);
      rotate_top_right(w);
    }
  } else if (corner == 2) {
  if (side(w, 5, 8) == BLUE)
    rotate_top_left(w);
  else if (side(w, 5, 8) == RED) {
    rotate_right_up(w);
    rotate_top_left(w);
    rotate_top_left(w);
  } else if (side(w, 5, 8) == WHITE)
    rotate_front_counterclockwise(w);
  } else if (corner == 3) {
    if (side(w, 5, 2) == BLUE) {
      rotate_top_left(w);
      rotate_top_left(w);
    } else if (side(w, 5, 2) == RED) {
      rotate_top_right(w);
      rotate_left_down(w);
    } else if (side(w, 5, 2) == WHITE) {
      rotate_right_down(w);
      rotate_top_left(w);
    }
  } else if (corner == 4) {
    if (side(w, 5, 0) == BLUE)
      rotate_top_right(w);
    else if (side(w, 5, 0) == RED)
      rotate_left_down(w);
    else if (side(w, 5, 0) == WHITE) {
      rotate_top_right(w);
      rotate_left_up(w);
      rotate_top_right(w);
    }
  } else if (corner == 5) {
    if (side(w, 6, 0) == BLUE) {
      rotate_bottom_left(w);
      rotate_left_up(w);
      rotate_left_up(w);
    } else if (side(w, 6, 0) == RED)
      rotate_left_up(w);
    else if (side(w, 6, 0) == WHITE)
      rotate_front_clockwise(w);
  } else if (corner == 6) {
    if (side(w, 6, 2) == BLUE) {
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
    } else if (side(w, 6, 2) == RED) {
      rotate_bottom_left(w);
      rotate_left_up(w);
    } else if (side(w, 6, 2) == WHITE) {
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
    }
  } else if (corner == 7) {
    if (side(w, 6, 8) == BLUE) {
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
    } else if (side(w, 6, 8) == RED) {
      rotate_bottom_left(w);
      rotate_bottom_left(w);
      rotate_left_up(w);
    } else if (side(w, 6, 8) == WHITE) {
      rotate_bottom_left(w);
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
    }
  } else if (corner == 8) {
    if (side(w, 6, 6) == BLUE) {
      rotate_left_up(w);
      rotate_left_up(w);
    } else if (side(w, 6, 6) == RED) {
      rotate_bottom_right(w);
      rotate_left_up(w);
    } else if (side(w, 6, 6) == WHITE) {
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
    }
  }

  /* now find the blue-yellow-red corner and place it */
  rotate_left(w);
  corner = find_corner(w,BLUE,YELLOW,RED);
  if (corner == 1) {
    if (side(w, 5, 6) == YELLOW) {
      rotate_front_counterclockwise(w);
      rotate_bottom_right(w);
      rotate_front_counterclockwise(w);
      rotate_front_counterclockwise(w);
    } else if (side(w, 5, 6) == RED) {
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
    }
  } else if (corner == 2) {
    if (side(w, 5, 8) == BLUE) {
      rotate_right_down(w);
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
    } else if (side(w, 5, 8) == YELLOW) {
      rotate_right_down(w);
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
    } else if (side(w, 5, 8) == RED)
      rotate_front_counterclockwise(w);
  } else if (corner == 3) {
    if (side(w, 5, 2) == BLUE) {
      rotate_right_down(w);
      rotate_right_down(w);
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
    } else if (side(w, 5, 2) == YELLOW) {
      rotate_right_down(w);
      rotate_front_counterclockwise(w);
    } else if (side(w, 5, 2) == RED) {
      rotate_right_down(w);
      rotate_right_down(w);
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
    }
  } else if (corner == 5) {
    if (side(w, 6, 0) == BLUE) {
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
    } else if (side(w, 6, 0) == YELLOW) {
      rotate_bottom_right(w);
      rotate_right_up(w);
      rotate_front_counterclockwise(w);
    } else if (side(w, 6, 0) == RED)
      rotate_front_clockwise(w);
  } else if (corner == 6) {
    if (side(w, 6, 2) == BLUE) {
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
    } else if (side(w, 6, 2) == YELLOW) {
      rotate_right_up(w);
      rotate_front_counterclockwise(w);
    } else if (side(w, 6, 2) == RED) {
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
    }
  } else if (corner == 7) {
    if (side(w, 6, 8) == BLUE) {
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
    } else if (side(w, 6, 8) == YELLOW) {
      rotate_bottom_left(w);
      rotate_right_up(w);
      rotate_front_counterclockwise(w);
    } else if (side(w, 6, 8) == RED) {
      rotate_bottom_left(w);
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
    }
  } else if (corner == 8) {
    if (side(w, 6, 6) == BLUE) {
      rotate_bottom_right(w);
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
    } else if (side(w, 6, 6) == YELLOW) {
      rotate_bottom_right(w);
      rotate_bottom_right(w);
      rotate_right_up(w);
      rotate_front_counterclockwise(w);
    } else if (side(w, 6, 6) == RED) {
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
    }
  }

  /* now find the blue-orange-yellow corner and place it */
  rotate_left(w);
  corner = find_corner(w,BLUE,ORANGE,YELLOW);
  if (corner == 1) {
    if (side(w, 5, 6) == ORANGE) {
      rotate_front_counterclockwise(w);
      rotate_bottom_right(w);
      rotate_front_counterclockwise(w);
      rotate_front_counterclockwise(w);
    } else if (side(w, 5, 6) == YELLOW) {
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
    }
  } else if (corner == 2) {
    if (side(w, 5, 8) == BLUE) {
      rotate_right_down(w);
      rotate_bottom_left(w);
      rotate_right_up(w);
      rotate_front_clockwise(w);
    } else if (side(w, 5, 8) == ORANGE) {
      rotate_front_clockwise(w);
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
    } else if (side(w, 5, 8) == YELLOW)
      rotate_front_counterclockwise(w);
  } else if (corner == 5) {
    if (side(w, 6, 0) == BLUE) {
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
    } else if (side(w, 6, 0) == ORANGE) {
      rotate_right_down(w);
      rotate_bottom_right(w);
      rotate_right_up(w);
      rotate_front_counterclockwise(w);
    } else if (side(w, 6, 0) == YELLOW)
      rotate_front_clockwise(w);
  } else if (corner == 6) {
    if (side(w, 6, 2) == BLUE) {
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
    } else if (side(w, 6, 2) == ORANGE) {
      rotate_bottom_left(w);
      rotate_right_down(w);
      rotate_bottom_right(w);
      rotate_right_up(w);
      rotate_front_counterclockwise(w);
    } else if (side(w, 6, 2) == YELLOW) {
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
    }
  } else if (corner == 7) {
    if (side(w, 6, 8) == BLUE) {
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
    } else if (side(w, 6, 8) == ORANGE) {
      rotate_bottom_left(w);
      rotate_bottom_left(w);
      rotate_right_down(w);
      rotate_bottom_right(w);
      rotate_right_up(w);
      rotate_front_counterclockwise(w);
    } else if (side(w, 6, 8) == YELLOW) {
      rotate_bottom_left(w);
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
    }
  } else if (corner == 8) {
    if (side(w, 6, 6) == BLUE) {
      rotate_bottom_right(w);
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
      rotate_front_clockwise(w);
    } else if (side(w, 6, 6) == ORANGE) {
      rotate_right_down(w);
      rotate_bottom_right(w);
      rotate_bottom_right(w);
      rotate_right_up(w);
      rotate_front_counterclockwise(w);
    } else if (side(w, 6, 6) == YELLOW) {
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
    }
  }

  /* and now find the blue-white-orange corner and place it */
  rotate_left(w);
  corner = find_corner(w,BLUE,WHITE,ORANGE);
  if (corner == 1) {
    if (side(w, 5, 6) == WHITE) {
      rotate_left_down(w);
      rotate_bottom_right(w);
      rotate_left_up(w);
      rotate_bottom_left(w);
      rotate_bottom_left(w);
      rotate_front_counterclockwise(w);
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
    } else if (side(w, 5, 6) == ORANGE) {
      rotate_front_counterclockwise(w);
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
      rotate_bottom_right(w);
      rotate_bottom_right(w);
      rotate_left_down(w);
      rotate_bottom_left(w);
      rotate_left_up(w);
    }
  } else if (corner == 5) {
    if (side(w, 6, 0) == BLUE) {
      rotate_bottom_right(w);
      rotate_left_down(w);
      rotate_bottom_left(w);
      rotate_bottom_left(w);
      rotate_left_up(w);
      rotate_bottom_right(w);
      rotate_left_down(w);
      rotate_bottom_left(w);
      rotate_left_up(w);
    } else if (side(w, 6, 0) == WHITE) {
      rotate_bottom_right(w);
      rotate_left_down(w);
      rotate_bottom_left(w);
      rotate_left_up(w);
    } else if (side(w, 6, 0) == ORANGE) {
      rotate_bottom_left(w);
      rotate_front_counterclockwise(w);
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
    }
  } else if (corner == 6) {
    if (side(w, 6, 2) == BLUE) {
      rotate_bottom_right(w);
      rotate_front_counterclockwise(w);
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
      rotate_bottom_left(w);
      rotate_front_counterclockwise(w);
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
    } else if (side(w, 6, 2) == WHITE) {
      rotate_left_down(w);
      rotate_bottom_left(w);
      rotate_left_up(w);
    } else if (side(w, 6, 2) == ORANGE) {
      rotate_bottom_left(w);
      rotate_bottom_left(w);
      rotate_front_counterclockwise(w);
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
    }
  } else if (corner == 7) {
    if (side(w, 6, 8) == BLUE) {
      rotate_left_down(w);
      rotate_bottom_right(w);
      rotate_left_up(w);
      rotate_bottom_right(w);
      rotate_left_down(w);
      rotate_bottom_left(w);
      rotate_left_up(w);
    } else if (side(w, 6, 8) == WHITE) {
      rotate_left_down(w);
      rotate_bottom_left(w);
      rotate_bottom_left(w);
      rotate_left_up(w);
    } else if (side(w, 6, 8) == ORANGE) {
      rotate_front_counterclockwise(w);
      rotate_bottom_left(w);
      rotate_bottom_left(w);
      rotate_front_clockwise(w);
    }
  } else if (corner == 8) {
    if (side(w, 6, 6) == BLUE) {
      rotate_front_counterclockwise(w);
      rotate_bottom_right(w);
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
      rotate_bottom_left(w);
      rotate_front_counterclockwise(w);
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
    } else if (side(w, 6, 6) == WHITE) {
      rotate_bottom_right(w);
      rotate_bottom_right(w);
      rotate_left_down(w);
      rotate_bottom_left(w);
      rotate_left_up(w);
    } else if (side(w, 6, 6) == ORANGE) {
      rotate_front_counterclockwise(w);
      rotate_bottom_right(w);
      rotate_front_clockwise(w);
    }
  }
  rotate_left(w);

  /* find the blue-red edge and place it */
  edge = find_edge(w,BLUE,RED);
  place_edge(w,edge,BLUE);
  rotate_left(w);

  /* find the blue-yellow edge and place it */
  edge = find_edge(w,BLUE,YELLOW);
  place_edge(w,edge,BLUE);
  rotate_left(w);

  /* find the blue-orange edge and place it */
  edge = find_edge(w,BLUE,ORANGE);
  place_edge(w,edge,BLUE);
  rotate_left(w);

  rotate_up(w);     /* put the blue side to the back */
}

/* This procedure places the front (GREEN) four corners into */
/* their correct positions. */
static void align_corners(w)
  RubikWidget w;
{
  int corner;

  /* find and place the green-orange-white corner (upper left) */
  corner = find_corner(w,GREEN,ORANGE,WHITE);
  if (corner == 2)
    rotate_front_counterclockwise(w);
  else if (corner == 5)
    rotate_front_clockwise(w);
  else if (corner == 6) {
    rotate_front_clockwise(w);
    rotate_front_clockwise(w);
  }

  /* find and place the green-yellow-orange corner (lower left) */
  corner = find_corner(w,GREEN,YELLOW,ORANGE);
  if (corner == 2) {
    rotate_clockwise(w);
    swap_bottom_corners(w);
    rotate_counterclockwise(w);
    swap_bottom_corners(w);
  } else if (corner == 6)
    swap_bottom_corners(w);

  /* find and place the green-red-yellow corner (lower right) */
  corner = find_corner(w,GREEN,RED,YELLOW);
  if (corner == 2) {
    rotate_clockwise(w);
    swap_bottom_corners(w);
    rotate_counterclockwise(w);
  }
}

/* This procedure swaps the the bottom front corners. */
static void swap_bottom_corners(w)
  RubikWidget w;
{
  rotate_top_right(w);
  rotate_front_clockwise(w);
  rotate_top_left(w);
  rotate_left_up(w);
  rotate_top_left(w);
  rotate_left_down(w);
  rotate_top_right(w);
  rotate_front_clockwise(w);
  rotate_front_clockwise(w);
}

/* This procedure completes the GREEN side by color-aligning the GREEN */
/* corners and putting the GREEN edges in place. */
static void solve_bottom(w)
  RubikWidget w;
{
  int aligned;

  /* the GREEN corners (currently in the front) should now be in their */
  /* proper locations; next, we color align them, and then move the */
  /* bottom edges into place */
  do {
    aligned = 0;
    if (side(w, 1, 0) == GREEN)
      aligned++;
    if (side(w, 1, 2) == GREEN)
      aligned++;
    if (side(w, 1, 6) == GREEN)
      aligned++;
    if (side(w, 1, 8) == GREEN)
      aligned++;

    if (aligned == 0) {
      color_align_front(w);
    } else if (aligned == 1) {
      /* place aligned corner in upper right */
      if (side(w, 1, 0) == GREEN)
      rotate_front_clockwise(w);
      if (side(w, 1, 6) == GREEN) {
        rotate_front_clockwise(w);
        rotate_front_clockwise(w);
      }
      if (side(w, 1, 8) == GREEN)
        rotate_front_counterclockwise(w);
      color_align_front(w);
    } else if (aligned == 2) {
      if (side(w, 1, 0) != GREEN)
        rotate_front_clockwise(w);
      else if (side(w, 1, 2) == GREEN)
        rotate_front_clockwise(w);
      if (side(w, 1, 0) != GREEN)
        rotate_front_clockwise(w);
      color_align_front(w);
    } else if (aligned == 3) /* not sure if this is possible */
      color_align_front(w);
  } while (aligned < 4);
}

static void solve_2_2(w)
  RubikWidget w;
{
  int i;

  for (i = 0; i < 3; i++) /* This is not always efficient */
    if (!check_solved(w))
      rotate_front_clockwise(w);
}

static void solve_bottom_edges(w)
  RubikWidget w;
{
  int edge, color;

  /* next we move the bottom edges into place */
  rotate_down(w);        /* put the green face on top */
  rotate_clockwise(w);
  rotate_clockwise(w);

  color = side(w, 1, 0);     /* get upper front corner color */
  edge = find_edge(w,GREEN,color);
  place_edge(w,edge,GREEN);
  rotate_top_right(w);

  color = side(w, 1, 0);     /* get upper front corner color */
  edge = find_edge(w,GREEN,color);
  place_edge(w,edge,GREEN);
  rotate_top_right(w);

  color = side(w, 1, 0);     /* get upper front corner color */
  edge = find_edge(w,GREEN,color);
  place_edge(w,edge,GREEN);
  rotate_top_right(w);

  color = side(w, 1, 0);     /* get upper front corner color */
  edge = find_edge(w,GREEN,color);
  place_edge(w,edge,GREEN);
  rotate_top_right(w);

  /* now, align the fourth blue edge piece (if necessary) */
  rotate_clockwise(w);
  rotate_clockwise(w);
  edge = find_edge(w,BLUE,WHITE);
  if (edge == 1) {
    if (side(w, 1, 1) == BLUE) {
      rotate_front_counterclockwise(w);
      rotate_center_left(w);
      rotate_front_clockwise(w);
      rotate_center_left(w);
      rotate_front_clockwise(w);
      rotate_center_left(w);
      rotate_front_counterclockwise(w);
    }
  } else {
    if (edge == 5)
      rotate_center_right(w);
    else if (edge == 7)
      rotate_center_left(w);
    else if (edge == 8) {
      rotate_center_right(w);
      rotate_center_right(w);
    }
    align_last_edge(w);
  }
}

/* This procedure moves the remaining BLUE edge piece into position */
/* from edge position #6. */
static void align_last_edge(w)
  RubikWidget w;
{
  /* edge piece should be in edge position #6 */
  /* check its orientation and decide which sequence to perform */
  if (side(w, 1, 5) == BLUE) {
    rotate_center_left(w);
    rotate_front_clockwise(w);
    rotate_center_right(w);
    rotate_front_counterclockwise(w);
    rotate_center_left(w);
    rotate_front_counterclockwise(w);
    rotate_center_right(w);
    rotate_front_clockwise(w);
  } else {
    rotate_front_counterclockwise(w);
    rotate_center_left(w);
    rotate_front_clockwise(w);
    rotate_center_right(w);
    rotate_front_clockwise(w);
    rotate_center_left(w);
    rotate_front_counterclockwise(w);
  }
}

/* This procedure aligns the bottom corner colors (may need to be repeated). */
static void color_align_front(w)
  RubikWidget w;
{
  rotate_top_right(w);
  rotate_front_clockwise(w);
  rotate_front_clockwise(w);
  rotate_top_left(w);
  rotate_front_clockwise(w);
  rotate_top_right(w);
  rotate_front_clockwise(w);
  rotate_top_left(w);
  rotate_front_clockwise(w);
  rotate_front_clockwise(w);
}

/* This procedure completes the solution process by placing the */
/* remaining edges in place and alignment. */
static void align_edges(w)
  RubikWidget w;
{
  int aligned, color, edge;

  /* move the red side to the front */
  if (side(w, 1, 4) == YELLOW)
    rotate_right(w);
  else if (side(w, 1, 4) == ORANGE) {
    rotate_right(w);
    rotate_right(w);
  } else if (side(w, 1, 4) == WHITE)
    rotate_left(w);

  /* rotate the top until its aligned with the center colors */
  edge = find_edge(w,BLUE,RED);
  if (edge == 2)
    rotate_top_left(w);
  else if (edge == 3) {
    rotate_top_left(w);
    rotate_top_left(w);
  } else if (edge == 4)
    rotate_top_right(w);

  /* rotate the bottom until its aligned with the center colors */
  edge = find_edge(w,GREEN,RED);
  if (edge == 10)
    rotate_bottom_left(w);
  else if (edge == 11) {
    rotate_bottom_left(w);
    rotate_bottom_left(w);
  } else if (edge == 12)
    rotate_bottom_right(w);

  if (check_solved(w))
    return;

  rotate_counterclockwise(w);  /* place unaligned edges vertically */

  /* see if any edges are in correct position */
  aligned = 0;
  edge = find_edge(w,RED,YELLOW);
  if (edge == 1)
    aligned++;
  edge = find_edge(w,YELLOW,ORANGE);
  if (edge == 3)
    aligned++;
  edge = find_edge(w,ORANGE,WHITE);
  if (edge == 11)
    aligned++;
  edge = find_edge(w,WHITE,RED);
  if (edge == 9)
    aligned++;

  if (aligned == 0) {
    shuffle_edges(w);  /* put one edge into position */
    aligned++;
  }

  if (aligned == 1) {
    /* find the correct piece and move it to the back bottom edge */
    edge = find_edge(w,RED,YELLOW);
    if (edge == 1) {
      rotate_down(w);
      rotate_down(w);
    } else {
      edge = find_edge(w,YELLOW,ORANGE);
      if (edge == 3)
        rotate_up(w);
      else {
        edge = find_edge(w,WHITE,RED);
        if (edge == 9)
          rotate_down(w);
      }
    }

    /* shuffle */
    color = side(w, 1, 4);
    if (side(w, 1, 7) == color) {
      rotate_right(w);
      rotate_right(w);
      rotate_down(w);
      shuffle_edges(w);
    } else if (side(w, 6, 1) == color) {
      rotate_right(w);
      rotate_right(w);
      rotate_down(w);
      shuffle_edges(w);
    } else
      shuffle_edges(w);
  }

  /* pieces should be in place; complete color alignment */
  /* find number of unaligned edge pieces and fix them */
  aligned = 0;
  if (side(w, 1, 1) == side(w, 1, 4))
    aligned++;
  if (side(w, 1, 7) == side(w, 1, 4))
    aligned++;
  if (side(w, 3, 1) == side(w, 3, 4))
    aligned++;
  if (side(w, 3, 7) == side(w, 3, 4))
    aligned++;
  if (aligned == 0) {
    recolor_top_edges(w);
    rotate_down(w);
    rotate_down(w);
    recolor_top_edges(w);
  } else if (aligned == 2) {
    if (side(w, 1, 1) == side(w, 1, 4))
      do {
        rotate_down(w);
      } while (side(w, 1, 1) == side(w, 1, 4));
    if (side(w, 1, 7) != side(w, 1, 4))
      rotate_up(w);
    recolor_top_edges(w);
    if (!check_solved(w)) {
      rotate_down(w);
      recolor_top_edges(w);
    }
  }
}

/* This procedure "shuffles" the three center edges on the front and */
/* top faces. */
static void shuffle_edges(w)
  RubikWidget w;
{
  rotate_center_up(w);
  rotate_top_right(w);
  rotate_top_right(w);
  rotate_center_down(w);
  rotate_top_right(w);
  rotate_top_right(w);
}

/* This procedure should be used when the two opposing top front and back */
/* edge pieces are in position but not color aligned (this sequence is */
/* known as Rubik's Maneuver). */
static void recolor_top_edges(w)
  RubikWidget w;
{
  rotate_center_up(w);
  rotate_top_right(w);
  rotate_center_up(w);
  rotate_top_right(w);
  rotate_center_up(w);
  rotate_top_right(w);
  rotate_top_right(w);
  rotate_center_down(w);
  rotate_top_right(w);
  rotate_center_down(w);
  rotate_top_right(w);
  rotate_center_down(w);
  rotate_top_right(w);
  rotate_top_right(w);
}

/* This procedure coordinates the solution process. */
void solve_polyhedrons(w)
  RubikWidget w;
{
  rubikCallbackStruct cb;

  cb.reason = RUBIK_RESET;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
  if (!check_solved(w)) {
    solve_top(w);
    align_corners(w);
    solve_bottom(w);
    if (w->rubik.size > 2) {
      solve_bottom_edges(w);
      align_edges(w);
    } else if (w->rubik.size == 2)
      solve_2_2(w);
  }
  cb.reason = RUBIK_COMPUTED;
  XtCallCallbacks((Widget) w, XtNselectCallback, &cb);
}
