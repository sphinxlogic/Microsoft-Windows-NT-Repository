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


#include <ctype.h>
#include "sysdef.h"
#include "global.h"
#include "basic.h"
#include "cmd.h"
#include "regions.h"
#include "window.h"
#include "io-term.h"
#include "io-generic.h"
#include "io-abstract.h"
#include "io-utils.h"
#include "io-curses.h"
#include "ref.h"
#include "format.h"
#include "lists.h"
#include "io-edit.h"
#include "eval.h"
#include "byte-compile.h"


/* Used by motion commands. */
const int colmagic[] = {0, 0, 1, -1, 1, -1, 1, -1, 0};
const int rowmagic[] = {-1, 1, 0, 0, -1, -1, 1, 1, 0};

char * motion_name[] =  
{
  "up",
  "down",
  "right",
  "left",
  "up right",
  "up left",
  "down right",
  "down left",
  "no motion"
};


/* This table ought to be a user parameter. */
enum motion_magic complementary_motion[] = 
{
  magic_right,
  magic_right,
  magic_down,
  magic_down,
  magic_right,
  magic_right,
  magic_right,
  magic_right,
  magic_no_motion,
};

enum motion_magic opposite_motion[] = 
{
  magic_down,
  magic_up,
  magic_left,
  magic_right,
  magic_down_left,
  magic_down_right,
  magic_up_left,
  magic_up_right,
  magic_no_motion,
};


/* Indexed by MAGIC + 1 */
const int boundrymagic[3] = { MIN_ROW, NON_ROW, MAX_ROW };




/* A very basic command. */

#ifdef __STDC__
void
noop (void)
#else
void
noop ()
#endif
{}



/* Commands that inser/delete rows/columns. */

#ifdef __STDC__
void
insert_row (int repeat)
#else
void
insert_row (repeat)
     int repeat;
#endif
{
  struct rng from;
  struct rng to;
  if ((repeat > (MAX_ROW - curow)) || (repeat < 0))
    {
      io_error_msg ("insert-row: prefix argument out of range.");
      return;
    }
  from.lc = MIN_COL;
  from.hc = MAX_COL;
  from.lr = curow;
  from.hr = MAX_ROW - repeat;
  to.lc = MIN_COL;
  to.hc = MIN_COL;
  to.lr = curow + repeat;
  to.hr = curow + repeat;
  move_region (&from, &to);
}

#ifdef __STDC__
void
insert_col (int repeat)
#else
void
insert_col (repeat)
     int repeat;
#endif
{
  struct rng from;
  struct rng to;
  if ((repeat > (MAX_COL - cucol)) || (repeat < 0))
    {
      io_error_msg ("insert-col: prefix argument out of range.");
      return;
    }
  from.lr = MIN_ROW;
  from.hr = MAX_ROW;
  from.lc = cucol;
  from.hc = MAX_COL - repeat;
  to.lr = MIN_ROW;
  to.hr = MIN_ROW;
  to.lc = cucol + repeat;
  to.hc = cucol + repeat;
  move_region (&from, &to);
}

#ifdef __STDC__
void
delete_row (int repeat)
#else
void
delete_row (repeat)
     int repeat;
#endif
{
  struct rng from;
  struct rng to;
  if ((repeat < 0) || (repeat > (MAX_ROW - curow + 1)))
    {
      io_error_msg ("delete-row: prefix argument out of range.");
      return;
    }
  from.lc = MIN_COL;
  from.hc = MAX_COL;
  from.lr = curow + repeat;
  from.hr = MAX_ROW;
  to.lc = MIN_COL;
  to.hc = MIN_COL;
  to.lr = curow;
  to.hr = curow;
  move_region (&from, &to);
}

#ifdef __STDC__
void
delete_col (int repeat)
#else
void
delete_col (repeat)
     int repeat;
#endif
{
  struct rng from;
  struct rng to;
  if ((repeat < 0) || (repeat > (MAX_COL - cucol + 1)))
    {
      io_error_msg ("delete-col: prefix argument out of range.");
      return;
    }
  from.lr = MIN_ROW;
  from.hr = MAX_ROW;
  from.lc = cucol + repeat;
  from.hc = MAX_COL;
  to.lr = MIN_ROW;
  to.hr = MIN_ROW;
  to.lc = cucol;
  to.hc = cucol;
  move_region (&from, &to);
}



/* Front end to the window functions. */

#ifdef __STDC__
void 
open_window (char *text)
#else
void 
open_window (text)
     char *text;
#endif
{
  int hv;
  int where;

  while (*text == ' ')
    text++;

  if (*text == 'h' || *text == 'H')
    hv = 0;
  else if (*text == 'v' || *text == 'V')
    hv = 1;
  else
    {
      io_error_msg ("Open 'h'orizontal or 'v'ertical window, not '%s'", text);
      return;
    }
  where = atoi (text + 1);
  while (isspace (*text))
    ++text;
  while (isalnum (*text))
    ++text;
  while (isspace (*text))
    ++text;
  if (*text == '%')
    {
      where *= (hv
		? (cwin->numr + (cwin->lh_wid ? label_rows : 0))
		: (cwin->numc + cwin->lh_wid));
      where /= 100;
    }
  io_win_open (hv, where);
}

#ifdef __STDC__
void
hsplit_window (void)
#else
void
hsplit_window ()
#endif
{
  open_window ("h50%");
}


#ifdef __STDC__
void
vsplit_window (void)
#else
void
vsplit_window ()
#endif
{
  open_window ("v50%");
}


#ifdef __STDC__
void 
close_window (char *text)
#else
void 
close_window (text)
     char *text;
#endif
{
  int num;

  num = atoi (text) - 1;

  if (num < 0 || num >= nwin)
    {
      io_error_msg ("Window %num?", text);
      return;
    }
  if (nwin == 1)
    {
      io_error_msg ("You can't close the last window!");
      return;
    }
  io_win_close (&wins[num]);
}

#ifdef __STDC__
void 
delete_window (void)
#else
void 
delete_window ()
#endif
{
  io_win_close (cwin);
}

#ifdef __STDC__
void
delete_other_windows (void)
#else
void
delete_other_windows ()
#endif
{
  if (nwin > 1)
    {
      CELLREF r = curow;
      CELLREF c = cucol;
      while (nwin > 1)
	io_win_close (cwin);
      io_move_cell_cursor (r, c);
    }
}

#ifdef __STDC__
void 
nicely_goto_window (int n)
#else
void 
nicely_goto_window (n)
     int n;
#endif
{
  if (input_active)
    {
      io_cellize_cursor ();
      window_after_input = n;
      input_active = 0;
      the_cmd_frame->top_keymap = map_id ("main");
      return;
    }
  else
    {
      if ((window_after_input >= 0)
	  && ((window_after_input % nwin) == n))
	{
	  io_inputize_cursor ();
	  window_after_input = -1;
	  input_active = 1;
	  the_cmd_frame->top_keymap =
	    map_id (the_cmd_frame->cmd
		    ? the_cmd_arg.style->keymap
		    : "main");
	}
      else
	io_set_cwin (&wins[n]);
    }
}

#ifdef __STDC__
void
goto_minibuffer (void)
#else
void
goto_minibuffer ()
#endif
{
  if (window_after_input < 0)
    {
      if (!input_active)
	io_error_msg ("Minibuffer not active.");
    }
  else
    nicely_goto_window ((window_after_input % nwin));
}


#ifdef __STDC__
void
goto_window (char *text)
#else
void
goto_window (text)
     char *text;
#endif
{
  int n;
  n = atoi (text) - 1;
  if (n < 0 || n > nwin)
    {
      io_error_msg ("Window %s doesn't exist.", text);
      return;
    }
  else
    nicely_goto_window (n);
}


#ifdef __STDC__
void 
other_window (void)
#else
void 
other_window ()
#endif
{
  int n = cwin - wins;
  if (!input_active)
    n = (n + 1) % nwin;
  nicely_goto_window (n);
}

#ifdef __STDC__
int
set_window_option (int set_opt, char *text)
#else
int
set_window_option (set_opt, text)
     int set_opt;
     char *text;
#endif
{
  int n;
  int stat;
  static struct opt
    {
      char *text;
      int bits;
    }
  opts[] =
  {
    {
      "reverse", WIN_EDGE_REV
    }
    ,
    {
      "standout", WIN_EDGE_REV
    }
    ,
    {
      "page", WIN_PAG_HZ | WIN_PAG_VT
    }
    ,
    {
      "pageh", WIN_PAG_HZ
    }
    ,
    {
      "pagev", WIN_PAG_VT
    }
    ,
    {
      "lockh", WIN_LCK_HZ
    }
    ,
    {
      "lockv", WIN_LCK_VT
    }
    ,
    {
      "edges", WIN_EDGES
    }
  };
  if ((stat = (!strincmp (text, "status", 6) && isspace (text[6])))
      || (!strincmp (text, "input", 5) && isspace (text[5])))
    {
      int n = set_opt ? atoi (text + 6 + stat) : 0;	/* A little pun. */
      int new_inp = stat ? user_input : n;
      int new_stat = stat ? n : user_status;
      io_set_input_status (new_inp, new_stat, 1);
    }
  else if (!strincmp (text, "link", 4))
    {
      if (set_opt)
	{
	  n = atoi (text + 4) - 1;
	  if (n < 0 || n > nwin)
	    io_error_msg ("Can't '%s': window # out of range", text);
	  else
	    cwin->link = n;
	}
      else
	cwin->link = -1;
    }
  else if (set_opt && !stricmp (text, "unlink"))
    cwin->link = -1;
  else if (set_opt && !strincmp (text, "row ", 4))
    {
      text += 4;
      curow = astol (&text);
    }
  else if (set_opt && !strincmp (text, "col ", 4))
    {
      text += 4;
      cucol = astol (&text);
    }
  else
    {
      for (n = 0; n < sizeof (opts) / sizeof (struct opt); n++)
	if (!stricmp (text, opts[n].text))
	  {
	    if (set_opt)
	      cwin->flags |= opts[n].bits;
	    else
	      cwin->flags &= ~opts[n].bits;
	    break;
	  }

      if (n == sizeof (opts) / sizeof (struct opt))
	  return 0;
    }
  return 1;
}

#ifdef __STDC__
void
show_window_options (void)
#else
void
show_window_options ()
#endif
{
  int n;

  cwin->win_curow = curow;
  cwin->win_cucol = cucol;
  if (user_status)
    io_text_line ("Status line at %d", user_status);
  else
    io_text_line ("Status line disabled.");
  io_text_line ("");
  for (n = 0; n < nwin; n++)
    {
      int flags = wins[n].flags;
      io_text_line ("Window #%d showing %s, with cursor at %s",
		    n + 1,
		    range_name (&wins[n].screen),
		    cell_name (wins[n].win_curow, wins[n].win_cucol));
      io_text_line ("   Options:  %sedges (%sreverse)%s%s%s%s",
		    flags & WIN_EDGES ? "" : "no",
		    flags & WIN_EDGE_REV ? "" : "no",
		    flags & WIN_PAG_HZ ? ", pageh" : "",
		    flags & WIN_PAG_VT ? ", pagev" : "",
		    flags & WIN_LCK_HZ ? ", lockh" : "",
		    flags & WIN_LCK_VT ? ", lockv" : "");
      if (wins[n].link != -1)
	io_text_line ("Linked to window %d", wins[n].link + 1);
    }
}

#ifdef __STDC__
void
recenter_window (void)
#else
void
recenter_window ()
#endif
{
  io_recenter_cur_win ();
}



/* Trivial front-end commands. */


#ifdef __STDC__
void
suspend_oleo (void)
#else
void
suspend_oleo ()
#endif
{
  if (using_curses)
    {
      stop_curses ();
      kill (getpid (), SIGSTOP);
    }
}

#ifdef __STDC__
void
recalculate (int all)
#else
void
recalculate (all)
     int all;
#endif
{
  current_cycle++;
  if (all)
    {
      CELLREF row;
      CELLREF col;
      struct rng all;
      all.lr = MIN_ROW;
      all.hr = MAX_ROW;
      all.lc = MIN_COL;
      all.hc = MAX_COL;
      find_cells_in_range (&all);
      while (next_row_col_in_range (&row, &col))
	push_cell (row, col);
    }
  while (eval_next_cell ())
    ;
}


#ifdef __STDC__
void
kill_oleo (void)
#else
void
kill_oleo ()
#endif
{
  io_close_display ();
  exit (0);
}


#ifdef __STDC__
void
kill_all_cmd (void)
#else
void
kill_all_cmd ()
#endif
{
  clear_spreadsheet ();
  io_repaint ();
}

#ifdef __STDC__
void
redraw_screen (void)
#else
void
redraw_screen ()
#endif
{
  io_repaint ();
}



/* Motion commands. */

#ifdef __STDC__
void
shift_cell_cursor (int dir, int repeat)
#else
void
shift_cell_cursor (dir, repeat)
     int dir;
     int repeat;
#endif
{
  io_shift_cell_cursor (dir, repeat);
}


#ifdef __STDC__
void
scroll_cell_cursor (int dir, int repeat)
#else
void
scroll_cell_cursor (dir, repeat)
     int dir;
     int repeat;
#endif
{
  io_scroll_cell_cursor (dir, repeat);
}



#ifdef __STDC__
void
goto_region (struct rng *r)
#else
void
goto_region (r)
     struct rng *r;
#endif
{
  (void) io_move_cell_cursor (r->lr, r->lc);

  if (r->hr != r->lr || r->hc != r->lc)
    {
      mkrow = r->hr;
      mkcol = r->hc;
    }
  else if (mkrow != NON_ROW)
    mkrow = NON_ROW;
  io_update_status ();
}

#ifdef __STDC__
void
goto_cell (struct rng * rng)
#else
void
goto_cell (rng)
     struct rng * rng;
#endif
{
  rng->hr = mkrow;
  rng->hc = mkcol;
  goto_region (rng);
}

#ifdef __STDC__
void
exchange_point_and_mark (int clrmk)
#else
void
exchange_point_and_mark (clrmk)
     int clrmk;
#endif
{
  struct rng rng;
  if (clrmk)
    {
      rng.lr = curow;
      rng.lc = cucol;
      rng.hr = NON_ROW;
      rng.hc = NON_COL;
      goto_region (&rng);
    }
  else if (mkrow != NON_ROW)
    {
      rng.lr = mkrow;
      rng.lc = mkcol;
      rng.hr = curow;
      rng.hc = cucol;
      goto_region (&rng);
    }
}

#ifdef __STDC__
static CELLREF
first_filled_col (CELLREF row)
#else
static CELLREF
first_filled_col (row)
     CELLREF row;
#endif
{
  struct rng rng;
  CELLREF r;
  CELLREF c;
  rng.lr = row;
  rng.hr = row;
  rng.lc = MIN_COL;
  rng.hc = MAX_COL;
  find_cells_in_range (&rng);
  while (1)
    {
      CELL * cp;
      cp = next_row_col_in_range (&r, &c);
      if (!cp)
	break;
      if (GET_TYP(cp))
	{
	  no_more_cells ();
	  return c;
	}
    }
  return NON_COL;
}

#ifdef __STDC__
static CELLREF
last_filled_col (CELLREF row)
#else
static CELLREF
last_filled_col (row)
     CELLREF row;
#endif
{
  struct rng rng;
  CELLREF r;
  CELLREF c;
  CELLREF bestc = MIN_COL;
  rng.lr = row;
  rng.hr = row;
  rng.lc = MIN_COL;
  rng.hc = MAX_COL;
  find_cells_in_range (&rng);
  while (1)
    {
      CELL * cp;
      cp = next_row_col_in_range (&r, &c);
      if (!cp)
	break;
      if (GET_TYP(cp))
	bestc = c;
    }
  return bestc;
}

#ifdef __STDC__
static CELLREF
first_filled_row (CELLREF col)
#else
static CELLREF
first_filled_row (col)
     CELLREF col;
#endif
{
  struct rng rng;
  CELLREF r;
  CELLREF c;
  CELL * cp;
  rng.lr = MIN_ROW;
  rng.hr = MAX_ROW;
  rng.lc = col;
  rng.hc = col;
  find_cells_in_range (&rng);
  while (1)
    {
      cp = next_row_col_in_range (&r, &c);
      if (!cp)
	break;
      if (GET_TYP(cp))
	{
	  no_more_cells ();
	  return r;
	}
    }
  return NON_ROW;
}

#ifdef __STDC__
static CELLREF
last_filled_row (CELLREF col)
#else
static CELLREF
last_filled_row (col)
     CELLREF col;
#endif
{
  struct rng rng;
  CELLREF r;
  CELLREF c;
  CELLREF bestr = MIN_ROW;
  CELL * cp;
  rng.lr = MIN_ROW;
  rng.hr = MAX_ROW;
  rng.lc = col;
  rng.hc = col;
  find_cells_in_range (&rng);
  while (1)
    {
      cp = next_row_col_in_range (&r, &c);
      if (!cp)
	break;
      if (GET_TYP(cp))
	bestr = r;
    }
  return bestr;
}

#ifdef __STDC__
static CELLREF
max_filled_row (void)
#else
static CELLREF
max_filled_row ()
#endif
{
  CELLREF max_r = highest_row ();
  while (max_r != MIN_ROW)
    {
      CELLREF c = first_filled_col (max_r);
      if (c != NON_COL)
	break;
      --max_r;
    }
  return max_r;
}


#ifdef __STDC__
static CELLREF
max_filled_col (void)
#else
static CELLREF
max_filled_col ()
#endif
{
  CELLREF max_c = highest_col ();
  while (max_c != MIN_COL)
    {
      CELLREF r = first_filled_row (max_c);
      if (r != NON_COL)
	break;
      --max_c;
    }
  return max_c;
}

#ifdef __STDC__
static void 
mk_for_extreme (struct rng * rng)
#else
static void 
mk_for_extreme (rng)
     struct rng * rng;
#endif
{
  if (mkrow != NON_ROW)
    {
      rng->hr = mkrow;
      rng->hc = mkcol;
    }
  else
    {
      rng->hc = cucol;
      rng->hr = curow;
    }
}

#ifdef __STDC__
void
upper_left (void)
#else
void
upper_left ()
#endif
{
  struct rng rng;
  rng.lr = MIN_ROW;
  rng.lc = MIN_COL;
  mk_for_extreme (&rng);
  goto_region (&rng);
}

#ifdef __STDC__
void
lower_left (void)
#else
void
lower_left ()
#endif
{
  struct rng rng;
  rng.lr = max_filled_row ();
  rng.lc = MIN_COL;
  mk_for_extreme (&rng);
  goto_region (&rng);
}

#ifdef __STDC__
void
upper_right (void)
#else
void
upper_right ()
#endif
{
  struct rng rng;
  rng.lr = MIN_ROW;
  rng.lc = max_filled_col ();
  mk_for_extreme (&rng);
  goto_region (&rng);
}

#ifdef __STDC__
void
lower_right (void)
#else
void
lower_right ()
#endif
{
  struct rng rng;
  rng.lr = max_filled_row ();
  rng.lc = max_filled_col ();
  mk_for_extreme (&rng);
  goto_region (&rng);
}

#ifdef __STDC__
void
mark_cell_cmd (int popmk)
#else
void
mark_cell_cmd (popmk)
     int popmk;
#endif
{
  if (popmk)
    {
      if (mkrow != NON_ROW)
	{
	  struct rng rng;
	  rng.lr = mkrow;
	  rng.lc = mkcol;
	  rng.hr = NON_ROW;
	  rng.hc = NON_COL;
	  goto_region (&rng);
	}
    }
  else
    {
      mkrow = curow;
      mkcol = cucol;
      io_update_status ();
    }
}

#ifdef __STDC__
void
unmark_cmd (void)
#else
void
unmark_cmd ()
#endif
{
  mkrow = NON_ROW;
  mkcol = NON_COL;
  io_update_status ();
}


/* This is a bit kludgey. Input line editting has its own event loop (grr!),
 * and all of its state is private.  These mouse commands can't entirely
 * handle it when the target is in the input line.  In that case, they
 * save the decoded mouse event where io_get_line can pick it up:
 */
struct mouse_event last_mouse_event;

#ifdef __STDC__
void
do_mouse_goto (void)
#else
void
do_mouse_goto ()
#endif
{
  if (!last_mouse_event.downp)
    return;
  if (last_mouse_event.location >= 0 && last_mouse_event.downp)
    {
      if (input_active)
	{
	  io_cellize_cursor ();
	  window_after_input = last_mouse_event.location;
	  input_active = 0;
	  the_cmd_frame->top_keymap = map_id ("main");
	}
      io_set_cwin (&wins[last_mouse_event.location]);
      io_move_cell_cursor (last_mouse_event.r, last_mouse_event.c);
    }
  else if (last_mouse_event.location == MOUSE_ON_INPUT)
    {
      goto_minibuffer ();
#ifdef HAVE_X11_X_H
      if (using_x)
	goto_char (io_col_to_input_pos (last_mouse_event.col));
#endif
    }
  else
    io_bell ();
}

#ifdef __STDC__
void
do_mouse_mark (void)
#else
void
do_mouse_mark ()
#endif
{
  if (last_mouse_event.location >= 0 && last_mouse_event.downp)
    {
      mkrow = last_mouse_event.r;
      mkcol = last_mouse_event.c;
    }
}


#ifdef __STDC__
void
do_mouse_mark_and_goto (void)
#else
void
do_mouse_mark_and_goto ()
#endif
{
  if (last_mouse_event.location >= 0 && last_mouse_event.downp)
    {
      mkrow = curow;
      mkcol = cucol;
    }
  do_mouse_goto ();
}

#ifdef __STDC__
void
do_mouse_cmd (void (*fn) ())
#else
void
do_mouse_cmd (fn)
     void (*fn) ();
#endif
{
  int seq = real_get_chr ();
  dequeue_mouse_event (&last_mouse_event, seq);
  fn ();
}

#ifdef __STDC__
void
mouse_mark_cmd (void)
#else
void
mouse_mark_cmd ()
#endif
{
  do_mouse_cmd (do_mouse_mark);
}


#ifdef __STDC__
void
mouse_goto_cmd (void)
#else
void
mouse_goto_cmd ()
#endif
{
  do_mouse_cmd (do_mouse_goto);
}

#ifdef __STDC__
void
mouse_mark_and_goto_cmd (void)
#else
void
mouse_mark_and_goto_cmd ()
#endif
{
  do_mouse_cmd (do_mouse_mark_and_goto);
}



/* Commands used to modify cell formulas. */

#ifdef __STDC__
void 
kill_cell_cmd (void)
#else
void 
kill_cell_cmd ()
#endif
{
  CELL *cp;

  cp = find_cell (curow, cucol);
  if (!cp)
    return;
  if ((GET_LCK (cp) == LCK_DEF && default_lock == LCK_LCK) || GET_LCK (cp) == LCK_LCK)
    {
      io_error_msg ("Cell %s is locked", cell_name (curow, cucol));
      return;
    }
  new_value (curow, cucol, "");
  cp->cell_flags = 0;
  cp->cell_font = 0;
  modified = 1;
}




/* A front end to sorting. */

#ifdef __STDC__
void
sort_region_cmd (char *ptr)
#else
void
sort_region_cmd (ptr)
     char *ptr;
#endif
{
  struct rng tmp_rng;

  if (get_abs_rng (&ptr, &sort_rng))
    {
      io_error_msg ("Can't find a range to sort in %s", ptr);
      return;
    }

  cur_row = sort_rng.lr;
  cur_col = sort_rng.lc;

  while (*ptr == ' ')
    ptr++;
  if (!*ptr)
    {
      sort_ele.lr = 0;
      sort_ele.lc = 0;
      sort_ele.hr = 0;
      sort_ele.hc = 0;
    }
  else if (!parse_cell_or_range (&ptr, &sort_ele))
    {
      io_error_msg ("Can't parse elements in %s", ptr);
      return;
    }
  else
    {
      sort_ele.lr -= sort_rng.lr;
      sort_ele.lc -= sort_rng.lc;
      sort_ele.hr -= sort_rng.lr;
      sort_ele.hc -= sort_rng.lc;
    }

  sort_keys_num = 0;
  while (*ptr == ' ')
    ptr++;
  for (; *ptr;)
    {
      if (sort_keys_num == sort_keys_alloc)
	{
	  sort_keys_alloc++;
	  if (sort_keys_alloc > 1)
	    sort_keys = ck_realloc (sort_keys, sort_keys_alloc * sizeof (struct cmp));
	  else
	    sort_keys = ck_malloc (sizeof (struct cmp));
	}
      sort_keys[sort_keys_num].mult = 1;
      if (*ptr == '+')
	ptr++;
      else if (*ptr == '-')
	{
	  sort_keys[sort_keys_num].mult = -1;
	  ptr++;
	}
      if (!*ptr)
	{
	  sort_keys[sort_keys_num].row = 0;
	  sort_keys[sort_keys_num].col = 0;
	  sort_keys_num++;
	  break;
	}
      if (!parse_cell_or_range (&ptr, &tmp_rng) || tmp_rng.lr != tmp_rng.hr || tmp_rng.lc != tmp_rng.hc)
	{
	  io_error_msg ("Can't parse key #%d in %s", sort_keys_num + 1, ptr);
	  sort_keys_num = -1;
	  return;
	}
      sort_keys[sort_keys_num].row = tmp_rng.lr - sort_rng.lr;
      sort_keys[sort_keys_num].col = tmp_rng.lc - sort_rng.lc;
      sort_keys_num++;

      while (*ptr == ' ')
	ptr++;
    }
  if (sort_keys_num == 0)
    {
      if (sort_keys_alloc == 0)
	{
	  sort_keys_alloc++;
	  sort_keys = ck_malloc (sizeof (struct cmp));
	}
      sort_keys[0].mult = 1;
      sort_keys[0].row = 0;
      sort_keys[0].col = 0;
      sort_keys_num++;
    }
  sort_region ();
  io_repaint ();
}





#ifdef __STDC__
void
imove (struct rng * rng, int ch)
#else
void
imove (rng, ch)
     struct rng * rng;
     int ch;
#endif
{
  if ((ch > 0) && (ch != 27))
    pushed_back_char = ch;

  goto_region (rng);
}




/* Incremental navigation
 *
 * This should be called in edit mode while gathering arguments 
 * for a complex command.  The expected the_cmd_arg.
 */

#define MIN(A,B)	((A) < (B) ? (A) : (B))

/* PAGE_RULE can be 0: page by  rows, 1: cols, 2 shorter of rows/cols,
 *    		   -1: don't page at all.
 */

#ifdef __STDC__
void
inc_direction (int count, int page_rule, int hack_magic)
#else
void
inc_direction (count, page_rule, hack_magic)
     int count;
     int page_rule;
     int hack_magic;
#endif
{
  if (check_editting_mode ())
    return;

  if (page_rule >= 0)
    {
      int page_size;

      switch (page_rule)
	{
	default:
	case 0:
	  page_size = (cwin->screen.hr - cwin->screen.lr);
	  break;
	case 1:
	  page_size = (cwin->screen.hc - cwin->screen.lc);
	  break;
	case 2:
	  page_size = MIN ((cwin->screen.hr - cwin->screen.lr),
			   (cwin->screen.hc - cwin->screen.lc));
	  break;
	}
      count *= page_size;
    }

  if (the_cmd_frame->cmd && the_cmd_arg.inc_cmd)
    the_cmd_arg.inc_cmd (hack_magic, count);
}




/* The commands that move to the extreme of a row[col] may also move
 * forward or backward some number of col[row], according to the prefix
 * arg.  This is the logic of that.  This function returns the new col[row]
 * and operates on the presumption that MIN_ROW == MIN_COL and
 * MAX_ROW == MAX_COL.
 */

#ifdef __STDC__
static CELLREF
extreme_cmd_orth_motion (int count, CELLREF current)
#else
static CELLREF
extreme_cmd_orth_motion (count, current)
     int count;
     CELLREF current;
#endif
{
  --count;
  if (count > (MAX_ROW - current))
    count =  (MAX_ROW - current);
  else if (-count > (current - MIN_ROW))
    count = (MIN_ROW - current);
  return current + count;
}


#ifdef __STDC__
void
beginning_of_row (int count)
#else
void
beginning_of_row (count)
     int count;
#endif
{
  struct rng rng;
  rng.lr = extreme_cmd_orth_motion (count, curow);
  rng.lc = MIN_COL;
  rng.hr = mkrow;
  rng.hc = mkcol;
  goto_region (&rng);
}

#ifdef __STDC__
void
end_of_row (int count)
#else
void
end_of_row (count)
     int count;
#endif
{
  struct rng rng;
  rng.lr = extreme_cmd_orth_motion (count, curow);
  rng.lc = last_filled_col (rng.lr);
  rng.hr = mkrow;
  rng.hc = mkcol;
  goto_region (&rng);
}

#ifdef __STDC__
void
beginning_of_col (int count)
#else
void
beginning_of_col (count)
     int count;
#endif
{
  struct rng rng;
  rng.lr = MIN_ROW;
  rng.lc = extreme_cmd_orth_motion (count, cucol);
  rng.hr = mkrow;
  rng.hc = mkcol;
  goto_region (&rng);
}

#ifdef __STDC__
void
end_of_col (int count)
#else
void
end_of_col (count)
     int count;
#endif
{
  struct rng rng;
  rng.lc = extreme_cmd_orth_motion (count, cucol);
  rng.lr = last_filled_row (rng.lc);
  rng.hr = mkrow;
  rng.hc = mkcol;
  goto_region (&rng);
}


#ifdef __STDC__
static void
skip_empties (CELLREF * rout, CELLREF * cout, int magic)
#else
static void
skip_empties (rout, cout, magic)
     CELLREF * rout;
     CELLREF * cout;
     int magic;
#endif
{
  CELLREF r = *rout;
  CELLREF c = *cout;
  CELL * cp = find_cell (r, c);

  while (!cp || !GET_TYP (cp))
    {
      if (r != boundrymagic [rowmagic [magic] + 1])
	r += rowmagic [magic];
      else if (rowmagic [magic])
	break;
      if (c != boundrymagic [colmagic [magic] + 1])
	c += colmagic [magic];
      else if (colmagic[magic])
	break;
      cp = find_cell (r, c);
    }

  *rout = r;
  *cout = c;
}

#ifdef __STDC__
void
scan_cell_cursor (int magic, int count)
#else
void
scan_cell_cursor (magic, count)
     int magic;
     int count;
#endif
{
  CELLREF r = curow;
  CELLREF c = cucol;
  CELLREF last_r = r;
  CELLREF last_c = c;

  skip_empties (&r, &c, magic);
  {
    CELL * cp = find_cell (r, c);
    if (!(cp && GET_TYP (cp)))
      return;
  }
  while (count)
    {
      CELL * cp = find_cell (r, c);
      while (!cp || !GET_TYP (cp))
	{
	  if (r != boundrymagic [rowmagic [magic] + 1])
	    r += rowmagic [magic];
	  else if (rowmagic [magic])
	    break;
	  if (c != boundrymagic [colmagic [magic] + 1])
	    c += colmagic [magic];
	  else if (colmagic[magic])
	    break;
	  cp = find_cell (r, c);
	}
      while (cp && GET_TYP (cp))
	{
	  if (r != boundrymagic [rowmagic [magic] + 1])
	    r += rowmagic [magic];
	  else if (rowmagic[magic])
	    break;
	  if (c != boundrymagic [colmagic [magic] + 1])
	    c += colmagic [magic];
	  else if (colmagic [magic])
	    break;
	  last_r = r;
	  last_c = c;
	  cp = find_cell (r, c);
	}
      --count;
    }
  {
    struct rng rng;
    rng.lr = last_r;
    rng.lc = last_c;
    rng.hr = mkrow;
    rng.hc = mkcol;
    goto_region (&rng);
  }
}


#ifdef __STDC__
void
edit_cell (char * new_formula)
#else
void
edit_cell (new_formula)
     char * new_formula;
#endif
{
  char * fail;
  fail = new_value (setrow, setcol, new_formula);
  if (fail)
    io_error_msg (fail);
  else
    modified = 1;
}


#ifdef __STDC__
void
set_region_formula (struct rng * rng, char * str)
#else
void
set_region_formula (rng, str)
     struct rng * rng;
     char * str;
#endif
{
  CELLREF row, col;

  for (row = rng->lr; row <= rng->hr; ++row)
    for (col = rng->lc; col <= rng->hc; ++col)
      {
	char * error = new_value (row, col, str);
	if (!error)
	  modified = 1;
	if (error)
	  {
	    io_error_msg (error);
	    return;
	  }
      }
}

#ifdef __STDC__
void
goto_edit_cell (int c)
#else
void
goto_edit_cell (c)
     int c;
#endif
{
  pushed_back_char = c;
  execute_command ("edit-cell", 1);
}


/* File i/o */

int sneaky_linec = 0;	/* for error reporting for now (see io-term.c) */

#ifdef __STDC__
void 
read_cmds_cmd (FILE *fp)
#else
void 
read_cmds_cmd (fp)
     FILE *fp;
#endif
{
  struct line line;
  char *ptr;
  init_line (&line);
  sneaky_linec = 0;
  while (read_line (&line, fp, &sneaky_linec))
    {
      for (ptr = line.buf; isspace (*ptr); ptr++);
      if (!*ptr || (*ptr == '#'))
	continue;
      execute_command (ptr, 1);
    }
}


static int run_load_hooks = 1;
static char load_hooks_string[] = "load_hooks";

#ifdef __STDC__
void
read_file_and_run_hooks (FILE * fp, int ismerge, char * name)
#else
void
read_file_and_run_hooks (fp, ismerge, name)
     FILE * fp;
     int ismerge;
     char * name;
#endif
{
  if (!ismerge)
    {
      if (current_filename)
	free (current_filename);
      current_filename = name ? ck_savestr (name) : 0;
    }
  (*read_file)(fp, ismerge);
  if (run_load_hooks)
    {
      struct var * v;
      v = find_var (load_hooks_string, sizeof (load_hooks_string) - 1);
      if (v && v->var_flags != VAR_UNDEF)
	execute_command (load_hooks_string, 1);
    }
}

/* If TURN_ON is 0, this toggles whether load hooks are run.
 * Otherwise, it turns load hooks on.
 */

#ifdef __STDC__
void
toggle_load_hooks (int turn_on)
#else
void
toggle_load_hooks (turn_on)
     int turn_on;
#endif
{
  if (!turn_on && run_load_hooks)
    {
      run_load_hooks = 0;
      io_info_msg ("load hooks turned off");
    }
  else
    {
      run_load_hooks = 1;
      io_info_msg ("load hooks turned on");
    }
}

#ifdef __STDC__
void
write_cmd (FILE *fp, char * name)
#else
void
write_cmd (fp, name)
     FILE *fp;
     char * name;
#endif
{
  if (current_filename)
    free (current_filename);
  current_filename = name ? ck_savestr (name) : 0;
  (*write_file) (fp, 0);
  modified = 0;
}

#ifdef __STDC__
void
read_cmd (FILE *fp, char * name)
#else
void
read_cmd (fp, name)
     FILE *fp;
     char * name;
#endif
{
  read_file_and_run_hooks (fp, 0, name);
}

#ifdef __STDC__
void
read_merge_cmd (FILE *fp)
#else
void
read_merge_cmd (fp)
     FILE *fp;
#endif
{
  (*read_file) (fp, 1);
}

#ifdef __STDC__
void
write_reg_cmd (FILE *fp, struct rng *rng)
#else
void
write_reg_cmd (fp, rng)
     FILE *fp;
     struct rng *rng;
#endif
{
  (*write_file) (fp, rng);
}




/* Cell attributes. */
#ifdef __STDC__
void
set_region_height (struct rng * rng, char * height)
#else
void
set_region_height (rng, height)
     struct rng * rng;
     char * height;
#endif
{
  int hgt;
  char * saved_height = height;

  while (isspace (*height))
    ++height;

  if (   !*height
      || words_imatch (&height, "d")
      || words_imatch (&height, "def")
      || words_imatch (&height, "default"))
    hgt = 0;
  else
    {
      hgt = astol (&height) + 1;
      if (hgt < 0)
        /* noreturn */
	io_error_msg ("Height (%d) can't be less than 0.", hgt);
    }      

  if (*height)
    {
      io_error_msg ("Unknown height '%s'", saved_height);
      /* Doesn't return */
    }
  {
    CELLREF cc;
    for (cc = rng->lr; ;cc++)
      {
	set_height (cc, hgt);
	if (cc == rng->hr)	/* This test goes here to prevent overflow. */
	  break;
      }
    io_recenter_all_win ();
  }
}

#ifdef __STDC__
void
set_region_width (struct rng * rng, char * width)
#else
void
set_region_width (rng, width)
     struct rng * rng;
     char * width;
#endif
{
  char * saved_width = width;
  int wid;

  while (isspace (*width))
    ++width;

  if (   !*width
      || words_imatch (&width, "d")
      || words_imatch (&width, "def")
      || words_imatch (&width, "default"))
    wid = 0;
  else
    {
      wid = astol (&width) + 1;
      if (wid < 0)
        /* noreturn */
	io_error_msg ("Width (%d) can't be less than 0.", wid);
    }      

  if (*width)
    {
      io_error_msg ("Unknown width '%s'", saved_width);
      /* No return. */
    }
  {
    CELLREF cc;
    for (cc = rng->lc; ;cc++)
      {
	set_width (cc, wid);
	if (cc == rng->hc)	/* This test goes here to prevent overflow. */
	  break;
      }
    io_recenter_all_win ();
  }
}


/* PROT may be `d', `p', or `u'. */

#ifdef __STDC__
void
set_region_protection (struct rng * rng, int prot)
#else
void
set_region_protection (rng, prot)
     struct rng * rng;
     int prot;
#endif
{
  prot = tolower (prot);
  switch (prot)
    {
    case 'd':
      lock_region (rng, LCK_DEF);
      break;
    case 'p':
      lock_region (rng, LCK_LCK);
      break;
    case 'u':
      lock_region (rng, LCK_UNL);
      break;
    default:
      io_error_msg ("Bad argument to protect-region %c.", prot);
      break;
    }
}

#ifdef __STDC__
void
set_region_alignment (struct rng * rng, int align)
#else
void
set_region_alignment (rng, align)
     struct rng * rng;
     int align;
#endif
{
  int fun = chr_to_jst (align);
  if (fun != -1)
    format_region (rng, -1, fun);
  else			/* if (main_map[align]!=BREAK_CMD) */
    io_error_msg ("Unknown Justify '%s'", char_to_string (align));
}

#ifdef __STDC__
void
set_region_format (struct rng * rng, int fmt)
#else
void
set_region_format (rng, fmt)
     struct rng * rng;
     int fmt;
#endif
{
  format_region (rng, fmt, -1); 
}




#ifdef __STDC__
void
set_def_height (char * height)
#else
void
set_def_height (height)
     char * height;
#endif
{
  char * saved_height = height;
  int hgt;

  while (isspace (*height))
    ++height;

  if (   !*height
      || words_imatch (&height, "d")
      || words_imatch (&height, "def")
      || words_imatch (&height, "default"))
    hgt = 0;
  else
    {
      hgt = astol (&height);
      if (hgt < 0)
        /* noreturn */
	io_error_msg ("Height (%d) can't be less than 0.", hgt);
    }      

  if (*height)
    {
      io_error_msg ("Unknown height '%s'", saved_height);
      /* No return. */
    }
  default_height = hgt;
  io_recenter_all_win ();
}

#ifdef __STDC__
void
set_def_width (char * width)
#else
void
set_def_width (width)
     char * width;
#endif
{
  char * saved_width = width;
  int wid;

  while (isspace (*width))
    ++width;

  if (   !*width
      || words_imatch (&width, "d")
      || words_imatch (&width, "def")
      || words_imatch (&width, "default"))
    wid = 8;
  else
    {
      wid = astol (&width);
      if (wid < 0)
        /* noreturn */
	io_error_msg ("Width (%d) can't be less than 0.", wid);
    }      

  if (*width)
    {
      io_error_msg ("Unknown width '%s'", saved_width);
      /* No return. */
    }
  default_width = wid;
  io_recenter_all_win ();
}

/* PROT may be `d', `p', or `u'. */

#ifdef __STDC__
void
set_def_protection (int prot)
#else
void
set_def_protection (prot)
     int prot;
#endif
{
  prot = tolower (prot);
  switch (prot)
    {
    case 'p':
      default_lock = LCK_LCK;
      break;
    case 'u':
      default_lock = LCK_UNL;
      break;
    default:
      io_error_msg ("Bad argument to set-default-protection %c.", prot);
      break;
    }
}

#ifdef __STDC__
void
set_def_alignment (int align)
#else
void
set_def_alignment (align)
     int align;
#endif
{
  int fun = chr_to_jst (align);
  if (fun == -1)
    io_error_msg ("Unknown justification.");

  default_jst = fun;
  io_repaint ();
}

#ifdef __STDC__
void
set_def_format (int fmt)
#else
void
set_def_format (fmt)
     int fmt;
#endif
{
  default_fmt = fmt;
  io_repaint ();
}



#ifdef __STDC__
void
define_usr_fmt (int fmt, char * pos_h, char * neg_h, char * pos_t,
		char * neg_t, char * zero, char * comma, char * decimal,
		char * precision, char * scale)
#else
void
define_usr_fmt (fmt, pos_h, neg_h, pos_t, neg_t, zero,
		comma, decimal, precision, scale)
     int fmt;
     char * pos_h;
     char * neg_h;
     char * pos_t;
     char * neg_t;
     char * zero;
     char * comma;
     char * decimal;
     char * precision;
     char * scale;
#endif
{
  char * usr_buf[9];
  if (fmt < 1 || fmt > 16)
    {
      io_error_msg ("Format number out of range %d (should be in [1..16].",
		    fmt);
      /* no return */
    }
  /* Vector to an older interface... */
  --fmt;
  usr_buf[0] = pos_h;
  usr_buf[1] = neg_h;
  usr_buf[2] = pos_t;
  usr_buf[3] = neg_t;
  usr_buf[4] = zero;
  usr_buf[5] = comma;
  usr_buf[6] = decimal;
  usr_buf[7] = precision;
  usr_buf[8] = scale;
  set_usr_stats (fmt, usr_buf);
}



/* Automatic motion while editting cell's: */

/* Hmm... where should this variable *really* go? */

static int auto_motion_direction = magic_down;

#ifdef __STDC__
void
set_auto_direction (enum motion_magic magic)
#else
void
set_auto_direction (magic)
     enum motion_magic magic;
#endif
{
  auto_motion_direction = magic;
  io_info_msg ("Auto-motion direction = %s.", motion_name[magic]);
}

#ifdef __STDC__
void
auto_move (void)
#else
void
auto_move ()
#endif
{
  shift_cell_cursor (auto_motion_direction, 1);
}

#ifdef __STDC__
void
auto_next_set (void)
#else
void
auto_next_set ()
#endif
{
  scan_cell_cursor (opposite_motion[auto_motion_direction], 1);
  shift_cell_cursor (complementary_motion[auto_motion_direction], 1);
}


/* This decompiles and then recompiles all of the formulas of cells.
 * This is never normally necessary unless you happen to have some
 * spreadsheets written when the byte-code compiler had bugs that
 * made your formulas produce parse errors.
 */

#ifdef __STDC__
void
recompile_spreadsheet (void)
#else
void
recompile_spreadsheet ()
#endif
{
  struct rng rng;
  CELL * cp;
  CELLREF r;
  CELLREF c;
  rng.lr = MIN_ROW;
  rng.lc = MIN_COL;
  rng.hr = MAX_ROW;
  rng.hc = MAX_COL;
  find_cells_in_range (&rng);
  for (cp = next_row_col_in_range (&r, &c);
       cp;
       cp = next_row_col_in_range(&r, &c))
    {
      char * form = decomp (r, c, cp);
      set_cell (r, c, form);
      if (my_cell)
	{
	  update_cell (my_cell);
	  if (is_constant (my_cell->cell_formula))
	    {
	      byte_free (my_cell->cell_formula);
	      my_cell->cell_formula = 0;
	    }
	  io_pr_cell (r, c, my_cell);
	  my_cell = 0;
	}
      decomp_free ();
    }
}

