/*	Copyright (C) 1992, 1993 Free Software Foundation, Inc.

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


#include "proto.h"
#include "funcdef.h"
#include <stdio.h>
#include <curses.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#undef NULL
#include "sysdef.h"
#include "global.h"
#include "cell.h"
#include "cmd.h"
#include "line.h"
#include "io-generic.h"
#include "io-edit.h"
#include "io-term.h"
#include "io-abstract.h"
#include "io-utils.h"
#include "lists.h"
#include "regions.h"
#include "window.h"
#include "key.h"
#include "input.h"
#include "info.h"

#define MIN_WIN_HEIGHT	(cwin->flags&WIN_EDGES ? 2 : 1)
#define MIN_WIN_WIDTH	(cwin->flags&WIN_EDGES ? 6 : 1)

static int redrew = 0;
static int textout = 0;
static int term_cursor_claimed = 0;

#ifdef __STDC__
static void move_cursor_to (struct window *, CELLREF, CELLREF, int);
#else
static void move_cursor_to ();
#endif


#ifdef __STDC__
static int
curses_metric (char * str, int len)
#else
static int
curses_metric (str, len)
     char * str;
     int len;
#endif
{
  return len;
}

static struct input_view input_view
  = {0, curses_metric, curses_metric, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#ifdef __STDC__
static void
_io_redraw_input (void)
#else
static void
_io_redraw_input ()
#endif
{
  int pos;
  int row = (input_view.current_info ? 1 : input);

  if (input_view.info_redraw_needed)
    {
      input_view.info_redraw_needed = 0;
      io_repaint ();
      return;
    }

  if (input_view.redraw_needed == NO_REDRAW)
    return;

  if (input_view.redraw_needed == FULL_REDRAW)
    {
      /* Redraw	the prompt. */ 
      move (row, 0);
      if (input_view.expanded_keymap_prompt)
	{
	  addstr (input_view.expanded_keymap_prompt);
	  clrtoeol ();
	  input_view.redraw_needed = NO_REDRAW;
	  return;
	}
      if (input_view.prompt_wid)
	addstr (input_view.prompt);
      pos = input_view.visibility_begin;
    }
  else
    {
      pos = input_view.redraw_needed;
      move (row,
	    input_view.prompt_wid + pos - input_view.visibility_begin);
    }
    
  if (   input_view.input_area
      && (input_view.visibility_end >= input_view.visibility_begin)
      && (input_view.visibility_begin < strlen (input_view.input_area->buf)))
    {
      int x;
      for (x = pos; x <= input_view.visibility_end; ++x)
	addch (input_view.input_area->buf[x]);
    }
  clrtoeol ();
  input_view.redraw_needed = NO_REDRAW;
}


#undef MIN
#define MIN(A,B) (((A) < (B)) ? (A) : (B))

#ifdef __STDC__
void
redraw_info (void)
#else
void
redraw_info ()
#endif
{
  if (!input_view.current_info)
    return;
  {
    int ipos = input_view.info_pos;
    int stop = MIN (input_view.current_info->len, scr_lines - 1 + ipos);
    while (ipos < stop)
      {
	move (2 + ipos - input_view.info_pos, 0);
	addstr (input_view.current_info->text[ipos]);
	clrtoeol ();
	++ipos;
      }
  }
  _io_redraw_input ();
}


#ifdef __STDC__
static void
_io_fix_input (void)
#else
static void
_io_fix_input ()
#endif
{
  iv_fix_input (&input_view);
}

#ifdef __STDC__
static void
_io_move_cursor (void)
#else
static void
_io_move_cursor ()
#endif
{
  iv_move_cursor (&input_view);
}

#ifdef __STDC__
static void
_io_erase (int len)
#else
static void
_io_erase (len)
     int len;
#endif
{
  iv_erase (&input_view, len);
}

#ifdef __STDC__
static void
_io_insert (int len)
#else
static void
_io_insert (len)
     int len;
#endif
{
  iv_insert (&input_view, len);
}
	   
#ifdef __STDC__
static void
_io_over (char * str, int len)
#else
static void
_io_over (str, len)
     char * str;
     int len;
#endif
{
  iv_over (&input_view, len);
}





#ifdef __STDC__
static void 
_io_display_cell_cursor (void)
#else
static void 
_io_display_cell_cursor ()
#endif
{
  int cell_cursor_row;
  int cell_cursor_col;
  int cc;
  int rr;
  int cwid;
  int n;
  int x, y;

  if (input_view.current_info)
    return;

  if (   (curow < cwin->screen.lr)
      || (cucol < cwin->screen.lc)
      || (curow > cwin->screen.hr)
      || (cucol > cwin->screen.hc))
    return;

  getyx (stdscr, y, x);
  cell_cursor_col = cwin->win_over;
  for (cc = cwin->screen.lc; cc < cucol; cc++)
    cell_cursor_col += get_width (cc);
  cell_cursor_row = cwin->win_down;
  for (rr = cwin->screen.lr; rr < curow; rr++)
    cell_cursor_row += get_height (rr);
  cwid = get_width (cucol);
  if (cwid > cwin->numc)
    cwid = cwin->numc;
  move (cell_cursor_row, cell_cursor_col);
  standout ();
  for (n = cwid; n; n--)
#ifdef A_STANDOUT
    addch (inch () | A_STANDOUT);
#else
    addch (inch ());
#endif
  standend ();
  move (y, x);
}

#ifdef __STDC__
static void 
_io_hide_cell_cursor (void)
#else
static void 
_io_hide_cell_cursor ()
#endif
{
  int cc;
  int rr;
  int cell_cursor_row;
  int cell_cursor_col;
  int cwid;
  int n;
  int x, y;

  if (input_view.current_info)
    return;
  if (   (curow < cwin->screen.lr)
      || (cucol < cwin->screen.lc)
      || (curow > cwin->screen.hr)
      || (cucol > cwin->screen.hc))
    return;
  getyx (stdscr, y, x);
  cell_cursor_col = cwin->win_over;
  for (cc = cwin->screen.lc; cc < cucol; cc++)
    cell_cursor_col += get_width (cc);
  cell_cursor_row = cwin->win_down;
  for (rr = cwin->screen.lr; rr < curow; rr++)
    cell_cursor_row += get_height (rr);
  cwid = get_width (cucol);
  if (cwid > cwin->numc)
    cwid = cwin->numc;
  move (cell_cursor_row, cell_cursor_col);
  for (n = cwid; n; n--)
#ifdef A_STANDOUT
    addch (inch () & ~A_STANDOUT);
#else
    addch (inch ());
#endif
  move (y, x);
}



/* Functions, etc for dealing with cell contents being displayed
	on top of other cells. */

struct slops
{
  int s_alloc, s_used;
  struct s
    {
      CELLREF row, clo, chi;
    } s_b[1];
};

#ifdef __STDC__
static void 
flush_slops (VOIDSTAR where)
#else
static void 
flush_slops (where)
     VOIDSTAR where;
#endif
{
  struct slops *s;

  s = where;
  if (s)
    s->s_used = 0;
}

#ifdef __STDC__
static int 
find_slop (VOIDSTAR where, CELLREF r, CELLREF c, CELLREF *cclp, CELLREF *cchp)
#else
static int 
find_slop (where, r, c, cclp, cchp)
     VOIDSTAR where;
     CELLREF r;
     CELLREF c;
     CELLREF *cclp;
     CELLREF *cchp;
#endif
{
  int n;
  struct slops *s;

  s = where;
  if (!s)
    return 0;
  for (n = 0; n < s->s_used; n++)
    {
      if (s->s_b[n].row == r && s->s_b[n].clo <= c && s->s_b[n].chi >= c)
	{
	  *cclp = s->s_b[n].clo;
	  *cchp = s->s_b[n].chi;
	  return 1;
	}
    }
  return 0;
}

#ifdef __STDC__
static void 
kill_slop (VOIDSTAR where, CELLREF r, CELLREF clo, CELLREF chi)
#else
static void 
kill_slop (where, r, clo, chi)
     VOIDSTAR where;
     CELLREF r;
     CELLREF clo;
     CELLREF chi;
#endif
{
  int n;
  struct slops *s;

  s = where;
  for (n = 0; n < s->s_used; n++)
    {
      if (s->s_b[n].row == r && s->s_b[n].clo == clo && s->s_b[n].chi == chi)
	{
	  --(s->s_used);
	  s->s_b[n] = s->s_b[s->s_used];
	  return;
	}
    }
}

#ifdef __STDC__
static void 
set_slop (VOIDSTAR *wherep, CELLREF r, CELLREF clo, CELLREF chi)
#else
static void 
set_slop (wherep, r, clo, chi)
     VOIDSTAR *wherep;
     CELLREF r;
     CELLREF clo;
     CELLREF chi;
#endif
{
  int n;
  struct slops **sp;

  sp = (struct slops **) wherep;
  if (!*sp)
    {
      (*sp) = ck_malloc (sizeof (struct slops) + 2 * sizeof (struct s));
      (*sp)->s_alloc = 2;
      (*sp)->s_used = 1;
      n = 0;
    }
  else
    {
      n = (*sp)->s_used++;
      if ((*sp)->s_alloc == n)
	{
	  (*sp)->s_alloc = n * 2;
	  (*sp) = ck_realloc ((*sp), sizeof (struct slops) + n * 2 * sizeof (struct s));
	}
    }
  (*sp)->s_b[n].row = r;
  (*sp)->s_b[n].clo = clo;
  (*sp)->s_b[n].chi = chi;
}

#ifdef __STDC__
static void 
change_slop (VOIDSTAR where,
	     CELLREF r, CELLREF olo, CELLREF ohi, CELLREF lo, CELLREF hi)
#else
static void 
change_slop (where, r, olo, ohi, lo, hi)
     VOIDSTAR where;
     CELLREF r;
     CELLREF olo;
     CELLREF ohi;
     CELLREF lo;
     CELLREF hi;
#endif
{
  int n;
  struct slops *s;

  s = where;
  for (n = 0; n < s->s_used; n++)
    {
      if (s->s_b[n].row == r && s->s_b[n].clo == olo && s->s_b[n].chi == ohi)
	{
	  s->s_b[n].clo = lo;
	  s->s_b[n].chi = hi;
	  return;
	}
    }
}


#ifdef __STDC__
static void 
_io_open_display (void)
#else
static void 
_io_open_display ()
#endif
{
  initscr ();
  scrollok (stdscr, 0);
#ifdef HAVE_CBREAK
  cbreak ();
#else
  crmode ();
#endif
  raw ();
  noecho ();
  nonl ();
  /* Must be after initscr() */
  io_init_windows (LINES, COLS - 1, 1, 2, 1, 1, 1, 1);
  info_rows = 1;
  print_width = COLS;		/* Make ascii print width == terminal width. */
}

#ifdef __STDC__
void
cont_curses(void)
#else
void
cont_curses()
#endif
{
#ifdef HAVE_CBREAK
  cbreak ();
#else
  crmode ();
#endif
  raw ();
  noecho ();
  nonl ();
}


#ifdef __STDC__
void
stop_curses(void)
#else
void
stop_curses()
#endif
{
#ifdef HAVE_CBREAK
  nocbreak ();
#else
  nocrmode ();
#endif
  noraw ();
  echo ();
  nl ();
  io_redisp ();
}

#ifdef __STDC__
static void 
_io_cellize_cursor (void)
#else
static void 
_io_cellize_cursor ()
#endif
{
}

#ifdef __STDC__
static void 
_io_inputize_cursor (void)
#else
static void 
_io_inputize_cursor ()
#endif
{
}

#ifdef __STDC__
static void 
_io_redisp (void)
#else
static void 
_io_redisp ()
#endif
{
  if (!term_cursor_claimed)
    {
      _io_redraw_input ();
      if (!(input_view.current_info || input_active ||
	    input_view.expanded_keymap_prompt))
	move_cursor_to (cwin, curow, cucol, 0);
      else
	move ((input_view.current_info ? 1 : input), 
	      input_view.prompt_wid + input_view.input_cursor -
	      input_view.visibility_begin); 
    }
  {
    struct rng * rng = &cwin->screen;
    if (   (curow > rng->hr)
	|| (curow < rng->lr)
	|| (cucol > rng->hc)
	|| (cucol < rng->lc))
      io_recenter_cur_win ();
  }
  refresh ();
}

#ifdef __STDC__
static void 
_io_repaint_win (struct window *win)
#else
static void 
_io_repaint_win (win)
     struct window *win;
#endif
{
  io_repaint ();
}

#ifdef __STDC__
static void 
_io_repaint (void)
#else
static void 
_io_repaint ()
#endif
{
  CELLREF cc, rr;
  int n, n1;
  CELL *cp;
  struct window *win;

  clear ();
  io_fix_input ();
  redrew++;
  if (input_view.current_info)
    {
      redraw_info ();
      return;
    }

  for (win = wins; win < &wins[nwin]; win++)
    {
      if (win->lh_wid)
	{
	  move (win->win_down - 1, win->win_over - win->lh_wid);
	  printw ("#%*d ", win->lh_wid - 2, 1 + win - wins);
	  if (win->flags & WIN_EDGE_REV)
	    standout ();
	  cc = win->screen.lc;
	  do
	    {
	      n = get_width (cc);
	      if (n > win->numc)
		n = win->numc;
	      if (n > 1)
		{
		  char *ptr;
		  char buf[30];

		  if (a0)
		    ptr = col_to_str (cc);
		  else
		    {
		      sprintf (buf, "C%u", cc);
		      ptr = buf;
		    }
		  --n;
		  n1 = strlen (ptr);
		  if (n < n1)
		    printw ("%.*s ", n, "###############");
		  else
		    {
		      n1 = (n - n1) / 2;
		      printw ("%*s%-*s ", n1, "", n - n1, ptr);
		    }
		}
	      else if (n == 1)
		addstr ("#");
	    }
	  while (cc++ < win->screen.hc);

	  rr = win->screen.lr;
	  n = win->win_down;
	  do
	    {
	      n1 = get_height (rr);
	      if (n1)
		{
		  move (n, win->win_over - win->lh_wid);
		  if (a0)
		    printw ("%-*d ", win->lh_wid - 1, rr);
		  else
		    printw ("R%-*d", win->lh_wid - 1, rr);
		  n += n1;
		}
	    }
	  while (rr++ < win->screen.hr);

	  if (win->flags & WIN_EDGE_REV)
	    standend ();
	}
      flush_slops (win->win_slops);
      find_cells_in_range (&(win->screen));
      while (cp = next_row_col_in_range (&rr, &cc))
	if (GET_TYP (cp))
	  io_pr_cell_win (win, rr, cc, cp);
    }
  if (!(cp = find_cell (curow, cucol)) || !GET_TYP (cp))
    io_display_cell_cursor ();
  input_view.redraw_needed = FULL_REDRAW;
  _io_redraw_input ();
  io_update_status ();
}

#ifdef __STDC__
static void 
_io_close_display (void)
#else
static void 
_io_close_display ()
#endif
{
  clear ();
  refresh ();
  (void) endwin ();
}

/* This is extern because it was convenient to leave the signal */
/* handler that increments it in io_term.c */
int input_avail_val;

#ifdef __STDC__
static int 
_io_input_avail (void)
#else
static int 
_io_input_avail ()
#endif
{
  return (FD_ISSET (0, &read_pending_fd_set)
	  || FD_ISSET (0, &exception_pending_fd_set));
}

#ifdef __STDC__
static void 
_io_scan_for_input (int block)
#else
static void 
_io_scan_for_input (block)
     int block;
#endif
{
  /* This function only exists because X kbd events don't generate */
  /* SIGIO. Under curses, the SIGIO hander does the work of this */
  /* function. */
}

#ifdef __STDC__
static void 
_io_wait_for_input (void)
#else
static void 
_io_wait_for_input ()
#endif
{
  pause ();
}

#ifdef __STDC__
static int 
_io_read_kbd (VOLATILE char *buf, int size)
#else
static int 
_io_read_kbd (buf, size)
     VOLATILE char *buf;
     int size;
#endif
{
  int r = read (0, buf, size);
  FD_CLR (0, &read_pending_fd_set);
  FD_CLR (0, &exception_pending_fd_set);
  return r;
}


#if defined(SIGIO)


#ifdef __STDC__
static void 
_io_nodelay (int delayp)
#else
static void 
_io_nodelay (delayp)
     int delayp;
#endif
{
  panic ("Trying to curses nodelay on a system with SIGIO.");
}

#else

#ifdef __STDC__
static void 
_io_nodelay (int delayp)
#else
static void 
_io_nodelay (delayp)
     int delayp;
#endif
{
  nodelay (stdscr, delayp);
}

#endif

#ifdef __STDC__
static int 
_io_getch (void)
#else
static int 
_io_getch ()
#endif
{
  char ch;
  return ((io_read_kbd (&ch, 1) != 1)
	  ? EOF
	  : ch);
}


#ifdef __STDC__
static int 
_io_get_chr (char *prompt)
#else
static int 
_io_get_chr (prompt)
     char *prompt;
#endif
{
  int x;
  mvaddstr (input, 0, prompt);
  clrtoeol ();
  topclear = 2;
  refresh ();
  ++term_cursor_claimed;
  x = get_chr ();
  --term_cursor_claimed;
  return x;
}

#define BUFFER 10

#ifdef __STDC__
static void 
local_putchar (int ch)
#else
static void 
local_putchar (ch)
     int ch;
#endif
{
  (void) putc (ch, stdout);
  /* (void)putchar(ch); */
}

#ifdef __STDC__
static void
local_puts (char *s)
#else
static void
local_puts (s)
    char *s;
#endif
{
   (void) tputs (s, 1, (int (*)()) local_putchar);
}

#ifdef __STDC__
static void 
_io_bell (void)
#else
static void 
_io_bell ()
#endif
{
#ifndef HAVE_GETCAP
  putchar ('\007');
#else
  static char *vb;
  static int called = 0;

  if (!called)
    {
      called++;
      vb = getcap ("vb");
    }
  if (vb)
    {
      local_puts (vb);
    }
  else
    {
      local_putchar ('\007');
    }
#endif
}


#if __STDC__
static void
move_cursor_to (struct window *win, CELLREF r, CELLREF c, int dn)
#else
static void
move_cursor_to (win, r, c, dn)
     struct window *win;
     CELLREF r;
     CELLREF c;
     int dn;
#endif
{
  int cc;
  int cell_cursor_col;
  int rr;
  int cell_cursor_row;
  
  cell_cursor_col = win->win_over;
  for (cc = win->screen.lc; cc < c; cc++)
    cell_cursor_col += get_width (cc);
  cell_cursor_row = win->win_down + dn;
  for (rr = win->screen.lr; rr < r; rr++)
    cell_cursor_row += get_height (rr);
  move (cell_cursor_row, cell_cursor_col);
}

#ifdef __STDC__
static void
_io_update_status (void)
#else
static void
_io_update_status ()
#endif
{
  CELL *cp;
  char *dec;
  char *ptr;
  static char hmbuf[40];
  int wid;
  int plen;
  int dlen;
  int yy, xx;
  
  if (!user_status || input_view.current_info)
    return;
  getyx (stdscr, yy, xx);
  move (status, 0);
  wid = COLS - 2;
  
  if (mkrow != NON_ROW)
    {
      struct rng r;
      
      addch ('*');
      --wid;
      set_rng (&r, curow, cucol, mkrow, mkcol);
      ptr = range_name (&r);
    }
  else
    ptr = cell_name (curow, cucol);
  
  addstr (ptr);
  wid -= strlen (ptr);
  
  if (how_many != 1)
    {
      sprintf (hmbuf, " {%d}", how_many);
      addstr (hmbuf);
      wid -= strlen (hmbuf);
    }
  
  if ((cp = find_cell (curow, cucol)) && cp->cell_formula)
    {
      dec = decomp (curow, cucol, cp);
      dlen = strlen (dec);
    }
  else
    {
      dec = 0;
      dlen = 0;
    }
  
  ptr = cell_value_string (curow, cucol);
  plen = strlen (ptr);
  
  if (dec)
    {
      wid -= 4;
      if (dlen + plen > wid)
	{
	  if (plen + 3 > wid)
	    printw (" %.*s... [...]", wid - 6, ptr);
	  else
	    printw (" %s [%.*s...]", ptr, wid - plen - 3, dec);
	}
      else
	printw (" %s [%s]", ptr, dec);
      decomp_free ();
    }
  else if (plen)
    {
      --wid;
      if (plen > wid)
	printw (" %.*s...", wid - 3, ptr);
      else
	printw (" %s", ptr);
    }
  
  clrtoeol ();
  move (yy, xx);
}

extern int auto_recalc;

#ifdef __STDC__
static void
_io_clear_input_before (void)
#else
static void
_io_clear_input_before ()
#endif
{
  textout = 0;
  if (topclear == 2)
    {
      move (input, 0);
      clrtoeol ();
      topclear = 0;
    }
  move (0, 0);
}

#ifdef __STDC__
static void
_io_clear_input_after (void)
#else
static void
_io_clear_input_after ()
  #endif
{
  if (topclear)
    {
      move (input, 0);
      clrtoeol ();
      topclear = 0;
    }
}


#if __STDC__
static void
_io_pr_cell_win (struct window *win, CELLREF r, CELLREF c, CELL *cp)
#else
static void
_io_pr_cell_win (win, r, c, cp)
     struct window *win;
     CELLREF r;
     CELLREF c;
     CELL *cp;
#endif
{
  int glowing;
  int lenstr;
  int j;
  int wid, wwid;
  int hgt;
  char *ptr;
  int yy, xx;
  
  if (input_view.current_info)
    return;

  wid = get_width (c);
  if (!wid)
    return;
  if (wid > win->numc)
    wid = win->numc;
  hgt = get_height (r);
  if (!hgt)
    return;
  if (hgt > win->numr)
    hgt = win->numr;
  
  getyx (stdscr, yy, xx);
  glowing = (r == curow && c == cucol && win == cwin);
  ptr = print_cell (cp);
  move_cursor_to (win, r, c, 0);
  if (glowing)
    standout ();
  j = GET_JST (cp);
  if (j == JST_DEF)
    j = default_jst;
  lenstr = strlen (ptr);
  
  if (lenstr <= wid - 1)
    {
      CELLREF ccl, cch;
      
      if (j == JST_LFT)
	printw ("%-*.*s", wid, wid - 1, ptr);
      else if (j == JST_RGT)
	printw ("%*.*s ", wid - 1, wid - 1, ptr);
      else if (j == JST_CNT)
	{
	  wwid = (wid - 1) - lenstr;
	  printw ("%*s%*s ", (wwid + 1) / 2 + lenstr, ptr, wwid / 2, "");
	}
#ifdef TEST
      else
	panic ("Unknown justification");
#endif
      if (glowing)
	standend ();
      
      if (lenstr == 0 && c > win->screen.lc
	  && find_slop (win->win_slops, r, c - 1, &ccl, &cch))
	{
	  CELLREF ccdl, ccdh;
	  
	  if (find_slop (win->win_slops, r, c, &ccdl, &ccdh) && ccdl == c)
	    {
	      kill_slop (win->win_slops, r, ccdl, ccdh);
	      for (; ccdh != ccdl; --ccdh)
		if (ccdh != c && (wwid = get_width (ccdh)))
		  {
		    move_cursor_to (win, r, ccdh, 0);
		    printw ("%*s", wwid, "");
		  }
	    }
	  kill_slop (win->win_slops, r, ccl, cch);
	  io_pr_cell (r, ccl, find_cell (r, ccl));
	}
      else if (find_slop (win->win_slops, r, c, &ccl, &cch))
	{
	  kill_slop (win->win_slops, r, ccl, cch);
	  for (; cch != ccl; --cch)
	    if (cch != c && (wwid = get_width (cch)))
	      {
		move_cursor_to (win, r, cch, 0);
		printw ("%*s", wwid, "");
	      }
	  io_pr_cell (r, ccl, find_cell (r, ccl));
	}
    }
  else
    {
      CELLREF cc = c;
      CELL *ccp;
      CELLREF ccl, cch;
      
      for (wwid = wid; lenstr > wwid - 1; wwid += get_width (cc))
	{
	  if (++cc > win->screen.hc
	      || ((ccp = find_cell (r, cc))
		  && GET_TYP (ccp)
		  && (GET_FMT (ccp) != FMT_HID
		      || (GET_FMT (ccp) == FMT_DEF
			  && default_fmt != FMT_HID))))
	    {
	      --cc;
	      break;
	    }
	}
      
      if (lenstr > wwid - 1)
	if (GET_TYP (cp) == TYP_FLT)
	  ptr = adjust_prc (ptr, cp, wwid - 1, wid - 1, j);
	else if (GET_TYP (cp) == TYP_INT)
	  ptr = (char *) numb_oflo;
      
      if (wwid == 1)
	{
	  addch (' ');
	  if (glowing)
	    standend ();
	}
      else if (wwid == wid)
	{
	  printw ("%-*.*s ", wwid - 1, wwid - 1, ptr);
	  if (glowing)
	    standend ();
	}
      else if (glowing)
	{
	  printw ("%.*s", wid, ptr);
	  standend ();
	  printw ("%-*.*s ", wwid - wid - 1, wwid - wid - 1, ptr + wid);
	}
      else if (r == curow && (cucol > c && cucol <= cc))
	{
	  CELLREF ctmp;
	  int w_left;
	  int w_here;
	  
	  w_left = wid;
	  for (ctmp = c + 1; ctmp < cucol; ctmp++)
	    w_left += get_width (ctmp);
	  printw ("%.*s", w_left, ptr);
	  standout ();
	  w_here = get_width (cucol);
	  if (wwid > w_left + w_here)
	    {
	      printw ("%-*.*s", w_here, w_here, ptr + w_left);
	      standend ();
	      printw ("%-*.*s ",
		      wwid - (w_left + w_here) - 1, wwid - (w_left + w_here) - 1,
		      ptr + w_left + w_here);
	    }
	  else
	    {
	      printw ("%-*.*s", w_here, w_here - 1, ptr + w_left);
	      standend ();
	    }
	}
      else
	printw ("%-*.*s ", wwid - 1, wwid - 1, ptr);
      
      if (find_slop (win->win_slops, r, c, &ccl, &cch))
	{
	  change_slop (win->win_slops, r, ccl, cch, c, cc);
	  for (; cch > cc; --cch)
	    if (wwid = get_width (cch))
	      {
		move_cursor_to (win, r, cch, 0);
		printw ("%*s", wwid, "");
	      }
	  for (cch = c - 1; cch > ccl; --cch)
	    if (wwid = get_width (cch))
	      {
		move_cursor_to (win, r, cch, 0);
		printw ("%*s", wwid, "");
	      }
	  if (ccl != c)
	    io_pr_cell (r, ccl, find_cell (r, ccl));
	}
      else
	set_slop ((VOIDSTAR *) (&(win->win_slops)), r, c, cc);
    }
  if ((hgt > 1) && display_formula_mode)
    {
      move_cursor_to (win, r, c, 1);
      ptr = decomp (r, c, cp);
      printw ("%.*s ", wid - 1, ptr);
      decomp_free ();
    }
  if (glowing)
    io_update_status ();
  move (yy, xx);
}



#ifdef __STDC__
static void
_io_flush (void)
#else
static void
_io_flush ()
#endif
{
  refresh ();
}




#ifdef __STDC__
void
tty_graphics (void)
#else
void
tty_graphics ()
#endif
{

  FD_SET (0, &read_fd_set);
  FD_SET (0, &exception_fd_set);
  IO_SETUP;
}
