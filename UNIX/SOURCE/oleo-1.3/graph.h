#ifndef GRAPHH
#define GRAPHH

/*	Copyright (C) 1993 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */
/*  t. lord	Mon Mar  1 17:37:20 1993	*/

#include "cell.h"

enum graph_axis
{
  graph_x = 0,
  graph_y = 1,
  graph_num_axis = 2
};
extern char * graph_axis_name [graph_num_axis];

/*
 * These are named according to the outermost iterator.
 * Thus graph_rows indicates that cells should be read in
 * the loop:
 *
 *	for (r = lr; r <= hr; ++r)
 *          for (c = lc; c <= hc; ++c)
 *		....
 */
enum graph_ordering
{
  graph_rows = 0,
  graph_cols = 1,
  graph_num_orders = 2
};
extern char * graph_order_name [graph_num_orders];

/* These describe supported ways to iterate over a range extracting
 * pairs of cells.
 *
 * There are two degrees of freedom:  the 2 cells in each
 * pair may be oriented horizontally or verically
 *			 ______________
 *	horizontal	 |   1  |  2  |
 *			 --------------
 *
 *
 *			 --------
 *	vertical	 |   1	|
 *			 --------
 *      		 |   2  |
 *			 --------
 *
 *      or the first member of each pair may be implicitly supplied.
 *	In that case, the values used will be 0, 1, 2 ...
 *
 * Pairs themselves can be read off either row-wise or column-wise as above.
 *
 */

enum graph_pair_orientation 
{
  graph_hz = 0,
  graph_vt = 1,
  graph_implicit = 2,
  graph_num_pair_orientations = 3
};

extern int graph_ornt_row_magic [graph_num_pair_orientations];
extern int graph_ornt_col_magic [graph_num_pair_orientations];

#define PAIR_ORDER(ORDER,ORNT)  \
  (((ORDER) * graph_num_pair_orientations) + ORNT)

#define ORDER_OF_PAIRS(GPO) ((GPO) / graph_num_pair_orientations)

enum graph_pair_ordering 
{
  graph_rows_hz = PAIR_ORDER(graph_rows, graph_hz),
  graph_rows_vt = PAIR_ORDER(graph_rows, graph_vt),
  graph_rows_implicit = PAIR_ORDER(graph_rows, graph_implicit),
  graph_cols_hz = PAIR_ORDER(graph_cols, graph_hz),
  graph_cols_vt = PAIR_ORDER(graph_cols, graph_vt),
  graph_cols_implicit = PAIR_ORDER(graph_cols, graph_implicit),
  graph_num_pair_orders = graph_cols_implicit + 1
};

extern char *pair_order_name [graph_num_pair_orders];


#ifdef __STDC__
typedef void (*fpi_thunk) (void * rock, CELL * cp, CELLREF, CELLREF);
#else
typedef void (*fpi_thunk) ();
#endif



#ifdef __STDC__
extern enum graph_axis chr_to_axis (int c);
extern enum graph_ordering chr_to_graph_ordering (int c);
extern enum graph_pair_ordering chrs_to_graph_pair_ordering (int pair, int dir);
extern char * graph_quoted_str (char *str);
extern void graph_x11_mono (void);
extern void graph_x11_color (void);
extern void graph_postscript (char * file, int kind, int spectrum, char * font, int pt_size);
extern void graph_set_axis_title (int axis_c, char * title);
extern void graph_set_logness (int axis_c, int explicit, int newval);
extern void graph_check_range (char * val);
extern void graph_set_axis_lo (int axis_c, char * val);
extern void graph_set_axis_hi (int axis_c, char * val);
extern void graph_set_axis_symbolic (int axis_c, struct rng * rng, int ordering_c) ;
extern void graph_set_axis_labels (int axis_c, struct rng * rng, int pair, int dir);
extern void graph_default_axis_labels (int axis_c);
extern int graph_check_style (char * name);
extern void graph_set_style (int data_set, char * style);
extern void graph_set_data_title (int data_set, char * title);
extern void graph_set_data (int data_set, struct rng * rng, int pair, int dir);
extern void graph_presets (void);
extern void graph_clear_datasets (void);
extern void init_graphing (void);
extern void graph_make_info (void);
extern void for_pairs_in (struct rng * rng, enum graph_pair_ordering order, fpi_thunk thunk, void * frame);
extern void graph_plot (void);

#else
extern enum graph_axis chr_to_axis ();
extern enum graph_ordering chr_to_graph_ordering ();
extern enum graph_pair_ordering chrs_to_graph_pair_ordering ();
extern char * graph_quoted_str ();
extern void graph_x11_mono ();
extern void graph_x11_color ();
extern void graph_postscript ();
extern void graph_set_axis_title ();
extern void graph_set_logness ();
extern void graph_check_range ();
extern void graph_set_axis_lo ();
extern void graph_set_axis_hi ();
extern void graph_set_axis_symbolic ();
extern void graph_set_axis_labels ();
extern void graph_default_axis_labels ();
extern int graph_check_style ();
extern void graph_set_style ();
extern void graph_set_data_title ();
extern void graph_set_data ();
extern void graph_presets ();
extern void graph_clear_datasets ();
extern void init_graphing ();
extern void graph_make_info ();
extern void for_pairs_in ();
extern void graph_plot ();

#endif

#endif  /* GRAPHH */


