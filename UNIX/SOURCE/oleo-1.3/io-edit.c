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




#include "funcdef.h"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#undef NULL

#include "sysdef.h"
#include "global.h"
#include "cell.h"
#include "io-utils.h"
#include "io-edit.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "cmd.h"
#include "format.h"
#include "lists.h"
#include "regions.h"


/* Shorthand */

#define cursor		the_cmd_arg.cursor
#define text 		the_cmd_arg.text
#define do_prompt	the_cmd_arg.do_prompt
#define is_set		the_cmd_arg.is_set
#define overwrite	the_cmd_arg.overwrite



/* Editting primitives
 * 
 * Commands that edit arguments to other commands should work by changing
 * the string stored in the_cmd_arg.text.   These functions edit that string
 * and correctly update the display.
 */

#ifdef __STDC__
int
check_editting_mode (void)
#else
int
check_editting_mode ()
#endif
{
  if (!the_cmd_frame->cmd || cur_arg >= cmd_argc || !do_prompt || is_set)
    {
      io_error_msg ("Command '%s' is not appropriate now.",
		    cur_cmd->func_name);
      /* not reached */
    }
  return 0;
}

/* Set the currently-being-editted line. 
 *
 * When this function is called, it indicates that some argument
 * is being read interactively from the user.  That fact is recorded
 * in the command frame because it relevant to error handling.
 * (See cmd_error in cmd.c)
 *
 */
#ifdef __STDC__
void
begin_edit (void)
#else
void
begin_edit ()
#endif
{
  topclear = 0;
  the_cmd_frame->complex_to_user = 1;
  io_fix_input ();
}

#ifdef __STDC__
void
setn_edit_line (char * str, int len)
#else
void
setn_edit_line (str, len)
     char * str;
     int len;
#endif
{
  setn_line (&text, str, len);
  cursor = len;
}

#ifdef __STDC__
void
toggle_overwrite (int set, int setting)
#else
void
toggle_overwrite (set, setting)
     int set;
     int setting;
#endif
{
  if (!set)
    overwrite = !overwrite;
  else
    overwrite = (setting > 0);
}

#ifdef __STDC__
void
beginning_of_line (void)
#else
void
beginning_of_line ()
#endif
{
  if (check_editting_mode ())
    return;
  cursor = 0;
  io_move_cursor ();
}


#ifdef __STDC__
void
end_of_line (void)
#else
void
end_of_line ()
#endif
{
  if (check_editting_mode ())
    return;
  cursor = strlen (text.buf);
  io_move_cursor ();
}

#ifdef __STDC__
void
backward_char (int n)
#else
void
backward_char (n)
     int n;
#endif
{
  if (check_editting_mode ())
    return;
  if (n < 0)
    forward_char (-n);
  else
    {
      char * error = 0;
      if (cursor < n)
	{
	  error = "Beginning of buffer.";
	  cursor = 0;
	}
      else
	cursor -= n;
      io_move_cursor ();
      if (error)
	io_error_msg (error);	/* Doesn't return. */
    }
}

#ifdef __STDC__
void
backward_word (int n)
#else
void
backward_word (n)
#endif
{
  if (check_editting_mode ())
    return;
  if (n < 0)
    forward_word (-n);
  else
    {
      if (cursor == strlen (text.buf))
	--cursor;
      while (n)
	{
	  while (cursor
		 && !isalnum (text.buf[cursor]))
	    --cursor;
	  while (cursor
		 && isalnum (text.buf[cursor]))
	    --cursor;
	  --n;
	}
      io_move_cursor ();
    }
}


#ifdef __STDC__
void
forward_char (int n)
#else
void
forward_char (n)
     int n;
#endif
{
  if (check_editting_mode ())
    return;
  if (n < 0)
    backward_char (-n);
  else
    {
      char * error = 0;
      int len = strlen(text.buf);
      if ((cursor + n) >= len)
	{
	  error = "End of buffer.";
	  cursor = len;
	}
      else
	cursor += n;
      io_move_cursor ();
      if (error)
	io_error_msg (error);	/* Doesn't return. */
    }
}


#ifdef __STDC__
void
goto_char (int n)
#else
void
goto_char (n)
     int n;
#endif
{
  if ((n < 0) || (n > (strlen (text.buf) + 1)))
    io_error_msg ("Char out of range (%d)", n);
  cursor = n;
  io_move_cursor ();
}

#ifdef __STDC__
void
forward_word (int n)
#else
void
forward_word (n)
     int n;
#endif
{
  if (check_editting_mode ())
    return;
  if (n < 0)
    backward_word (-n);
  else
    {
      int len = strlen (text.buf);
      while (n)
	{
	  while ((cursor < len)
		 && !isalnum (text.buf[cursor]))
	    ++cursor;
	  while ((cursor < len)
		 && isalnum (text.buf[cursor]))
	    ++cursor;
	  --n;
	}
      io_move_cursor ();
    }
}


#ifdef __STDC__
static void
erase (int len)
#else
static void
erase (len)
     int len;
#endif
{
  if (check_editting_mode ())
    return;
  else
    {
      strcpy (&text.buf[cursor],
	      &text.buf[cursor + len]);
      io_erase (len);
    }
}


#ifdef __STDC__
void
backward_delete_char (int n)
#else
void
backward_delete_char (n)
#endif
{
  if (check_editting_mode ())
    return;
  if (n < 0)
    delete_char (-n);
  else
    {
      char * error = 0;
      if (cursor < n)
	{
	  error = "Beginning of buffer.";
	  n = cursor;
	}
      cursor -= n;
      erase (n);
      if (error)
	io_error_msg (error);	/* Doesn't return. */
    }
}


#ifdef __STDC__
void 
backward_delete_word (int n)
#else
void 
backward_delete_word (n)
#endif
{
  if (check_editting_mode ())
    return;
  else
    {
      int at = cursor;
      while (n)
	{
	  while (cursor
		 && !isalnum (text.buf[cursor]))
	    --cursor;

	  while (cursor
		 && isalnum (text.buf[cursor - 1]))
	    --cursor;
	  --n;
	}
      erase (at - cursor);
    }
}


#ifdef __STDC__
void
delete_to_start(void)
#else
void
delete_to_start()
#endif
{
  if (check_editting_mode ())
    return;
  else
    {
      int at = cursor;
      cursor = 0;
      erase (at);
    }
}


#ifdef __STDC__
void
delete_char (int n)
#else
void
delete_char (n)
     int n;
#endif
{
  if (check_editting_mode ())
    return;
  if (n < 0)
    backward_delete_char (-n);
  else
    {
      char * error = 0;
      int len = strlen (text.buf);
      if (cursor + n > len)
	{
	  error = "End of buffer.";
	  n = len - cursor;
	}
      erase (n);
      if (error)
	io_error_msg (error);	/* Doesn't return. */
    }
}

#ifdef __STDC__
void
delete_word (int n)
#else
void
delete_word (n)
     int n;
#endif
{
  if (check_editting_mode ())
    return;
  if (n < 0)
    backward_delete_word (-n);
  else
    {
      int len = strlen (text.buf);
      int erase_len = 0;
      while (n)
	{
	  while (((cursor + erase_len) < len)
		 && !isalnum (text.buf[(cursor + erase_len)]))
	    ++erase_len;
	  while (((cursor + erase_len) < len)
		 && isalnum (text.buf[(cursor + erase_len)]))
	    ++erase_len;
	  --n;
	}      
      erase (erase_len);
    }
}


#ifdef __STDC__
void
kill_line(void)
#else
void
kill_line()
#endif
{
  if (check_editting_mode ())
    return;
  else
    {
      int len = strlen (text.buf);
      erase (len - cursor);
    }
}

#ifdef __STDC__
void
insert_string (char * str, int len)
#else
void
insert_string (str, len)
     char * str;
     int len;
#endif
{
  if (check_editting_mode ())
    return;
  splicen_line (&text, str, len, cursor);
  io_insert (len);
  cursor += len;
}

#ifdef __STDC__
void
over_string (char * str, int len)
#else
void
over_string (str, len)
     char * str;
     int len;
#endif
{
  if (check_editting_mode ())
    return;
  if (cursor + len > strlen (text.buf))
    {
      catn_line (&text, str + text.alloc - cursor,
		 len - (text.alloc - cursor));
      len = text.alloc - cursor;
    }
  bcopy (str, text.buf + cursor, len);
  io_over (str, len);
  cursor += len;
}

#ifdef __STDC__
void
put_string (char * str, int len)
#else
void
put_string (str, len)
     char * str;
     int len;
#endif
{
  if (check_editting_mode ())
    return;
  (overwrite ? over_string : insert_string) (str, len);
}



/* Higher Level editting commands. */

#ifdef __STDC__
void
insert_cell_expression (void)
#else
void
insert_cell_expression ()
#endif
{
  if (check_editting_mode ())
    return;
  else
    {
      CELL *cp;
      char * in_str;
      if (!(cp = find_cell (curow, cucol)))
	return;
      in_str = decomp (curow, cucol, cp);
      put_string (in_str, strlen(in_str));
      decomp_free ();
    }
}


#ifdef __STDC__
void
insert_cell_value(void)
#else
void
insert_cell_value()
#endif
{
  if (check_editting_mode ())
    return;
  else
    {
      char * in_str;
      in_str = cell_value_string (curow, cucol);
      put_string (in_str, strlen(in_str));
    }
}

#ifdef __STDC__
void
insert_rel_ref(void)
#else
void
insert_rel_ref()
#endif
{
  if (check_editting_mode ())
    return;
  else
    {
      char vbuf[50];
      char * in_str;
      if (a0)
	{
	  if (mkrow != NON_ROW)
	    {
	      struct rng r;
	      set_rng (&r, curow, cucol, mkrow, mkcol);
	      in_str = range_name (&r);
	    }
	  else
	    in_str = cell_name (curow, cucol);
	}
      else
	{
	  if (mkrow != NON_ROW)
	    {
	      switch (((curow == setrow) << 3)
		      + ((mkrow == setrow) << 2)
		      + ((cucol == setcol) << 1)
		      + (mkcol == setcol))
		{
		case 0:
		case 1:
		case 2:
		case 4:
		case 5:
		case 6:
		case 8:
		case 9:
		case 10:
		  sprintf (vbuf, "r[%+d:%+d]c[%+d:%+d]",
			   (curow < mkrow ? curow : mkrow) - setrow,
			   (curow < mkrow ? mkrow : curow) - setrow,
			   (cucol < mkcol ? cucol : mkcol) - setcol,
			   (cucol < mkcol ? mkcol : cucol) - setcol);
		  break;
		  
		case 3:
		case 7:
		case 11:
		  sprintf (vbuf, "r[%+d:%+d]c",
			   (curow < mkrow ? curow : mkrow) - setrow,
			   (curow < mkrow ? mkrow : curow) - setrow);
		  break;
		  
		case 12:
		case 14:
		case 13:
		  sprintf (vbuf, "rc[%+d:%+d]",
			   (cucol < mkcol ? cucol : mkcol) - setcol,
			   (cucol < mkcol ? mkcol : cucol) - setcol);
		  break;
		  
		case 15:
		  strcpy (vbuf, "rc");
		  break;
		}
	    }
	  
	  else
	    {
	      switch (((curow == setrow) << 1) + (cucol == setcol))
		{
		case 0:
		  sprintf (vbuf, "r[%+d]c[%+d]", curow - setrow, cucol - setcol);
		  break;
		case 1:
		  sprintf (vbuf, "r[%+d]c", curow - setrow);
		  break;
		case 2:
		  sprintf (vbuf, "rc[%+d]", cucol - setcol);
		  break;
		case 3:
		  strcpy (vbuf, "rc");
		  break;
#ifdef TEST
		default:
		  panic ("huh what");
#endif
		}
	    }
	  in_str = vbuf;
	}
      put_string (in_str, strlen (in_str));
    }
}


#ifdef __STDC__
void
insert_abs_ref(void)
#else
void
insert_abs_ref()
#endif
{
  if (check_editting_mode ())
    return;
  else
    {
      char vbuf[50];
      char * in_str;
      /* Insert current cell/range name as an absolute reference */
      if (a0)
	{
	  if (mkrow != NON_ROW)
	    sprintf (vbuf, "$%s$%u:$%s:$%u",
		     col_to_str (cucol), curow, col_to_str (mkcol), mkrow) ;
	  else
	    sprintf (vbuf, "$%s$%u", col_to_str (cucol), curow);
	  in_str = vbuf;
	}
      else
	{
	  if (mkrow != NON_ROW)
	    {
	      struct rng r;
	      
	      set_rng (&r, curow, cucol, mkrow, mkcol);
	      in_str = range_name (&r);
	    }
	  else
	    in_str = cell_name (curow, cucol);
	}
      put_string (in_str, strlen (in_str));  
    }
}

#ifdef __STDC__
void
insert_cell_attr (struct rng * rng, char * attr)
#else
void
insert_cell_attr (rng, attr)
     struct rng * rng;
     char * attr;
#endif
{
  struct line line;
  init_line (&line);
  if (!stricmp (attr, "width"))
    {
      int wid = get_nodef_width (rng->lc);
      if (wid == 0)
	set_line (&line, "def");
      else
	sprint_line (&line, "%d", wid - 1);
    }
  else if (!stricmp (attr, "height"))
    {
      int hgt = get_nodef_height (rng->lr);
      if (hgt == 0)
	set_line (&line, "def");
      else
	sprint_line (&line, "%d", hgt - 1);
    }
  else if (!stricmp (attr, "format"))
    {
      CELL * cp = find_cell (rng->lr, rng->lc);
      if (!cp)
	set_line (&line, "def");
      else
	{
	  int fmt = GET_FMT (cp);
	  set_line (&line, fmt_to_str (fmt));
	}
    }
  else if (!stricmp (attr, "font"))
    {
      CELL * cp = find_cell (rng->lr, rng->lc);
      if (!(cp && cp->cell_font))
	set_line (&line, "def");
      else
	set_line (&line, cp->cell_font->names->oleo_name);
    }
  else if (!stricmp (attr, "font-scale"))
    {
      CELL * cp = find_cell (rng->lr, rng->lc);
      if (!(cp && cp->cell_font))
	set_line (&line, "1.0");
      else
	sprint_line (&line, "%lf", cp->cell_font->scale);
    }
  put_string (line.buf, strlen (line.buf));
}

#ifdef __STDC__
void
insert_usr_fmt_part (int fmt, int stat)
#else
void
insert_usr_fmt_part (fmt, stat)
     int fmt;
     int stat;
#endif
{
  char * usr_stats[9];
  if ((fmt < 1) || (fmt > 16))
    io_error_msg
      ("insert-user-format-part arg 1 out of range (%d); should be in [1-16].",
       fmt);
  --fmt;
  if ((stat < 1) || (stat > 16))
    io_error_msg
      ("insert-user-format-part arg 2 out of range (%d); should be in [1-9].",
       stat);
  --stat;
  get_usr_stats (fmt, usr_stats);
  put_string (usr_stats[stat], strlen (usr_stats[stat]));
}

#ifdef __STDC__
void
self_insert_command (int ch, int count)
#else
void
self_insert_command (ch, count)
     int ch;
     int count;
#endif
{
  if (check_editting_mode ())
    return;
  else if (count == 1)
    {
      char chr = ch;
      put_string (&chr, 1);
    }
  else if (count > 0)
    {
      char * buf = (char *)ck_malloc (count); /* sleazy, huh? */
      int x;
      for (x = 0; x < count; ++x)
	buf[x] = ch;
      put_string (buf, count);
    }
}


/* Keysequences are read using the `keyseq' keymap.
 * Every key in that map should be bound to this function.
 */
#ifdef __STDC__
void
self_map_command (int c)
#else
void
self_map_command (c)
     int c;
#endif
{
  struct keymap * map = the_maps[the_cmd_arg.val.key.cmd.code];
  char space = ' ';
  char * str = char_to_string (c);

  insert_string (str, strlen (str));
  insert_string (&space, 1);

  while (map)
    {
      the_cmd_arg.val.key.cmd = map->keys[c];
      if (the_cmd_arg.val.key.cmd.vector < 0)
	{
	  if (the_cmd_arg.val.key.cmd.code < 0)
	    map = map->map_next;
	  else
	    return;
	}
      else
	break;
    }
  exit_minibuffer ();
  return;
}

#ifdef __STDC__
void
insert_current_filename (void)
#else
void
insert_current_filename ()
#endif
{
  if (current_filename)
    put_string (current_filename, strlen (current_filename));
}


/* Reading a single character is done with the read-char
 * map.  Every key in that map should be bound to this function.
 */
#ifdef __STDC__
void
exit_self_inserting (int c)
#else
void
exit_self_inserting (c)
     int c;
#endif
{
  char * str = char_to_string (c);

  insert_string (str, strlen (str));
  exit_minibuffer ();
}


#ifdef __STDC__
static int
issymb (int c)
#else
static int
issymb (c)
     int c;
#endif
{
  return isalpha (c) || isdigit (c) || (c == '_');
}

#undef text
#undef cursor
#undef do_prompt
#ifdef __STDC__
void
insert_context_word (void)
#else
void
insert_context_word ()
#endif
{
  struct command_frame * cf = the_cmd_frame->prev;
  if (   (cf == the_cmd_frame)
      || !cf->cmd
      || !cf->argv [cf->_cur_arg].do_prompt)
    return;
  {
    struct command_arg * ca = &cf->argv [cf->_cur_arg];
    char * beg_text = ca->text.buf;
    char * last = beg_text + ca->cursor;
    char * start;

    while ((last > beg_text) && !issymb(*last))
      --last;
    while (*last && issymb (*last))
      ++last;
    --last;
    start = last;
    while ((start > beg_text) && issymb(*start))
      --start;
    if (!issymb (*start) && (start < last))
      ++start;

    if ((start <= last) && issymb (*start))
      {
	insert_string (start, last - start + 1);
	the_cmd_arg.cursor = 0;
      }
  }
}
#define text the_cmd_arg.text
#define do_prompt the_cmd_arg.do_prompt
#define cursor the_cmd_arg.cursor
