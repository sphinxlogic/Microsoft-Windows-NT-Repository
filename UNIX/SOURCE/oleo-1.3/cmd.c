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
#include "errno.h"
#define obstack_chunk_alloc ck_malloc
#define obstack_chunk_free free
#include "obstack.h"
#include "global.h"
#include "cmd.h"
#include "io-term.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "io-utils.h"
#include "io-edit.h"
#include "stub.h"
#include "ref.h"

#undef MIN
#undef MAX
#define MIN(A,B) ((A) < (B) ? (A) : (B))
#define MAX(A,B) ((A) > (B) ? (A) : (B))

/* Bogus mapping from KEY->CODE to ranges.  This is how bound
 * macros are represented.
 * This is bogus because the ranges will not be adjusted in 
 * the ways they should.  Variables should be used instead. 
 */
int n_bound_macros;
struct rng *bound_macros;
int bound_macro_vec;



/* Flow of control centers around a select loop.  These are the 
 * fd's selected on.
 */
SELECT_TYPE read_fd_set;
SELECT_TYPE exception_fd_set;
SELECT_TYPE write_fd_set;

/* These are fd's returned by the last call to select.
 */
SELECT_TYPE read_pending_fd_set;
SELECT_TYPE exception_pending_fd_set;
SELECT_TYPE write_pending_fd_set;

/* Hooks for asynchronos i/o
 */

struct select_hook file_read_hooks[SELECT_SET_SIZE] = {0};
struct select_hook file_exception_hooks[SELECT_SET_SIZE] = {0};
struct select_hook file_write_hooks[SELECT_SET_SIZE] = {0};


/* The current stream from which commands are being read. */
struct input_stream * the_input_stream = 0;




#ifdef __STDC__
static struct input_stream *
default_input_stream (void)
#else
static struct input_stream *
default_input_stream ()
#endif
{
  if (!the_input_stream)
    {
      the_input_stream =
	(struct input_stream *)ck_malloc (sizeof (struct input_stream));
      the_input_stream->_rmac = 0;
      the_input_stream->_func_arg = 0;
      obstack_init (&the_input_stream->_macro_stack);
      the_input_stream->_macro = 0;
      the_input_stream->_macro_start = 0;
      the_input_stream->_macro_size = 0;
      the_input_stream->prev_stream = 0;
      the_input_stream->_last_macro = 0;
      the_input_stream->_pushed_back_char = -1;
    }
  return the_input_stream;
}


/* This constructs an input stream that reads from a macro but never
 * from a keyboard.  EXECUTE_CMD uses this.
 */


#ifdef __STDC__
static struct input_stream *
macro_only_input_stream (struct rng * rng, char * first_line, int len,
			 struct command_frame * frame)
#else
static struct input_stream *
macro_only_input_stream (rng, first_line, len, frame)
     struct rng * rng;
     char * first_line;
     int len;
     struct command_frame * frame;
#endif
{
  struct input_stream * ret;
  ret = (struct input_stream *)ck_malloc (sizeof (struct input_stream));
  ret->_func_arg = 0;
  obstack_init (&ret->_macro_stack);
  ret->_rmac =
    (struct macro *) obstack_alloc (&ret->_macro_stack, sizeof (struct macro));
  ret->_rmac->mac_prev = 0;
  ret->_rmac->mac_rng = *rng;
  ret->_rmac->mac_row = rng->lr;
  ret->_rmac->mac_col = rng->lc;
  (void) obstack_grow (&ret->_macro_stack, first_line, len);
  (void) obstack_grow (&ret->_macro_stack, "", 1);
  ret->_rmac->mac_start = ret->_rmac->mac_exe
    = (unsigned char *) obstack_finish (&ret->_macro_stack);
  ret->_macro = 0;
  ret->_macro_start = 0;
  ret->_macro_size = 0;
  ret->_last_macro = 0;
  ret->prev_stream = frame->input;
  {
    struct input_stream * key = frame->input;
    while (frame->input == key)
      {
	frame->input = ret;
	frame = frame->prev;
      }
  }
  ret->_pushed_back_char = -1;
  return ret;
}

#ifdef __STDC__
void
free_input_stream (struct input_stream * stream)
#else
void
free_input_stream (stream)
     struct input_stream * stream;
#endif
{
  if (stream->_macro_start)
    free (stream->_macro_start);
  if (stream->_last_macro)
    free (stream->_last_macro);
  obstack_free (&stream->_macro_stack, 0);
  free (stream);
}

/* This gets rid of an input stream created by macro_only_input_stream.
 * It fixes the INPUT fields of pending command frames.
 */

#ifdef __STDC__
void
pop_input_stream (void)
#else
void
pop_input_stream ()
#endif
{
  if (the_cmd_frame->input->prev_stream)
    {
      struct command_frame * fr = the_cmd_frame;
      struct input_stream * key = the_cmd_frame->input;
      while (fr->input == key)
	{
	  fr->input = key->prev_stream;
	  fr = fr->prev;
	}
      free_input_stream (key);
      return;
    }
}


/* Macros 
 * These are the commands the user has to interact with macros.
 */

#ifdef __STDC__
void
start_entering_macro (void)
#else
void
start_entering_macro ()
#endif
{
  if (making_macro)
    {
      io_error_msg ("Can't define two macros at once");
      return;
    }
  making_macro_size = 20;
  making_macro = making_macro_start = ck_malloc (5 + making_macro_size);
}

#ifdef __STDC__
void
bound_macro (int num)
#else
void
bound_macro (num)
     int num;
#endif
{
  struct macro *old;
  CELL *cp;

  cp = find_cell (bound_macros[num].lr, bound_macros[num].lc);
  if (!cp || GET_TYP (cp) != TYP_STR || cp->cell_str[0] == '\0')
    return;
  old = rmac;
  rmac =
    (struct macro *) obstack_alloc (&macro_stack, sizeof (struct macro));
  rmac->mac_prev = old;
  rmac->mac_rng = bound_macros[num];
  rmac->mac_row = bound_macros[num].lr;
  rmac->mac_col = bound_macros[num].lc;
  obstack_grow (&macro_stack, cp->cell_str, 1 + strlen (cp->cell_str));
  rmac->mac_start = rmac->mac_exe =
    (unsigned char *) obstack_finish (&macro_stack);
}

#ifdef __STDC__
void
run_string_as_macro (char * macro, int count)
#else
void
run_string_as_macro (macro, count)
     char * macro;
     int count;
#endif
{
  struct rng rng;
  while (count--)
    {
      rng.lr = rng.hr = rng.lc = rng.hc = MIN_ROW;
      macro_only_input_stream (&rng, macro, strlen (macro), the_cmd_frame);
      command_loop (1);
    }
}

#ifdef __STDC__
void
call_last_kbd_macro (int count)
#else
void
call_last_kbd_macro (count)
     int count;
#endif
{
  if (!last_macro)
    io_error_msg ("No keyboard macro entered.");
  run_string_as_macro ((char *)last_macro, count);
}

/* This command is automaticly inserted into the command stream
 * when the end of a macro is reached.  
 */
#ifdef __STDC__
void
end_macro (void)
#else
void
end_macro ()
#endif
{
  CELL *cp;
  struct macro *old;

  if (!rmac)
    {
      io_error_msg ("Not executing a macro!");
      return;
    }
  if ((rmac->mac_row == rmac->mac_rng.hr)
      && (rmac->mac_col == rmac->mac_rng.hc))
    {
      old = rmac->mac_prev;
      obstack_free (&macro_stack, rmac);
      rmac = old;
    }
  else
    {
      if (rmac->mac_row == rmac->mac_rng.hr)
	{
	  rmac->mac_row = rmac->mac_rng.lr; 
	  rmac->mac_col++;
	}
      else
	rmac->mac_row++;
      
      cp = find_cell (rmac->mac_row, rmac->mac_col);
      
      if (!cp || GET_TYP (cp) != TYP_STR || cp->cell_str[0] == '\0') 
	{
	  old = rmac->mac_prev;
	  obstack_free (&macro_stack, rmac); 
	  rmac = old;
	}
      else
	{
	  obstack_grow (&macro_stack, cp->cell_str, 1 + strlen (cp->cell_str));
	  rmac->mac_exe
	    = (unsigned char *) obstack_finish (&macro_stack);
	}
    }
}


/* This command is executed by the user to stop entering a macro.
 */
#ifdef __STDC__
void
stop_entering_macro (void)
#else
void
stop_entering_macro ()
#endif
{
  if (!making_macro)
    {
      if (rmac)
	return;
      io_error_msg ("Not defining a macro!");
      return;
    }

  making_macro[0] = '\0';
  making_macro = 0;
  if (last_macro)
    ck_free (last_macro);
  last_macro = making_macro_start;
  making_macro_start = 0;
  free (making_macro_start);
}

#ifdef __STDC__
void
store_last_macro (struct rng * rng)
#else
void
store_last_macro (rng)
     struct rng * rng;
#endif
{
  union vals z;
  z.c_s = (char *)last_macro;
  set_new_value (rng->lr, rng->lc, TYP_STR, &z);
}



/* Scheduling 
 *
 * Scheduling is centered around the function real_get_chr
 * which is allowed to block until an input event has occured.
 * Before blocking, real_get_chr may evaluate cells and/or update 
 * the display.
 */


/* Error messages are delivered to the user by invoking a command 
 * that prompts with the error message, and waits for the user's next
 * keypress.  This command shouldn't wait indefinitely.  After a short time,
 * the error message should disappear.  This is accomplished by counting down
 * a timer, then destorying the error message command frame and throwing an
 * error.  The error is thrown directly rather than with io_error_msg in order
 * to avoid circularity.
 */

static void
error_alarm ()
{
  if (the_cmd_frame->cmd && the_cmd_arg.timeout_seconds)
    {
      --the_cmd_arg.timeout_seconds;
      if (!the_cmd_arg.timeout_seconds)
	{
	  pop_unfinished_command ();
	  alarm_table[2].freq = 0;
	  longjmp (error_exception, 1);
	}
    }
  else
    alarm_table[2].freq = 0;
}

struct alarm_entry alarm_table [3] = 
{
  {cell_alarm, 1, 0},
  {error_alarm, 0, 0},
  {0, 0}
};

/* Function that get called whenever blocking times out. */

#ifdef __STDC__
static void
alarm_hooks (void)
#else
static void
alarm_hooks ()
#endif
{
  int x;
  time_t now = time(0);
  for (x = 0; alarm_table[x].fn; ++x)
    if (alarm_table[x].freq
	&& ((now - alarm_table[x].last_time) >= alarm_table[x].freq))
      {
	alarm_table[x].last_time = now;
	alarm_table[x].fn ();
      }
}


#ifdef __STDC__
static void
select_hooks (void)
#else
static void
select_hooks ()
#endif
{
  int x;
  for (x = 0; x < SELECT_SET_SIZE; ++x)
    {
      if (file_read_hooks[x].hook_fn && FD_ISSET (x, &read_pending_fd_set))
	file_read_hooks[x].hook_fn (x);
      FD_CLR (x, &read_pending_fd_set);
      if (file_write_hooks[x].hook_fn && FD_ISSET (x, &write_pending_fd_set))
	file_write_hooks[x].hook_fn (x);
      FD_CLR (x, &write_pending_fd_set);
      if (file_exception_hooks[x].hook_fn
	  && FD_ISSET (x, &exception_pending_fd_set))
	file_exception_hooks[x].hook_fn (x);
      FD_CLR (x, &exception_pending_fd_set);
    }
}

/* Block until we get a signal (unless system calls restart), 
 * can do i/o or, until we timeout (timeout is specified in seconds,
 * 0 means block indefinately).  (Front end to select)
 */
#ifdef __STDC__
static void
block_until_excitment (int timeout_seconds)
#else
static void
block_until_excitment (timeout_seconds)
     int timeout_seconds;
#endif
{
  int ret;
  struct timeval timeout;
  struct timeval * time_p = 0;

  if (timeout_seconds)
    {
      timeout.tv_sec = timeout_seconds;
      timeout.tv_usec = 0;
      time_p = &timeout;
    }
  bcopy ((char *)&read_fd_set, (char *)&read_pending_fd_set,
	 sizeof (SELECT_TYPE)) ;
  bcopy ((char *)&exception_fd_set,
	 (char *)&exception_pending_fd_set, sizeof (SELECT_TYPE));
  bcopy ((char *)&write_fd_set,
	 (char *)&write_pending_fd_set, sizeof (SELECT_TYPE));
  ret = select (SELECT_SET_SIZE,
		&read_pending_fd_set, &write_pending_fd_set,
		&exception_pending_fd_set, time_p);
  if (ret < 0)
    {
      FD_ZERO (&read_pending_fd_set);
      FD_ZERO (&write_pending_fd_set);
      FD_ZERO (&exception_pending_fd_set);
    }
}

/* This is the main interact loop.  As quickly as possible
 * it returns a character from the keyboard.  While waiting,
 * it updates cells and the display.  If a macro is being defined,
 * this function save characters in the macro.
 */

#ifdef __STDC__
int 
real_get_chr (void)
#else
int 
real_get_chr ()
#endif
{
  int ch;			/* The char that will be returned. */

  /* Characters with the meta bit set are returned as
   * two characters: ESC and a non-meta character.
   * This buffers the non-meta character between calls.
   * The value is 0 if no character is buffered, C+1 if
   * C is buffered.
   */
  static int saved_char;

  /* A buffer of characters read in one burst from the kbd. */
  static char ibuf[256];
  static int i_in;		/* chars buffered */
  static int i_cnt;		/* buffer position */

  alarm_hooks ();
  if (saved_char)
    {
      ch = saved_char - 1;
      saved_char = 0;
      goto fini;
    }

  if (i_cnt)
    {
      ch = ibuf[i_cnt++];
      if (i_cnt == i_in)
	i_cnt = i_in = 0;
      goto fini;
    }

  /* This loop until a character can be read. */
  while (!io_input_avail ())
    {
      alarm_hooks ();
      select_hooks ();
      io_scan_for_input (0);
      if (!io_input_avail ())
	{
	  ++current_cycle;
	  if (auto_recalc && eval_next_cell ())
	    {
	      if (bkgrnd_recalc)
		while (!io_input_avail () && eval_next_cell ())
		  io_scan_for_input (0);
	      else
		while (eval_next_cell ())
		  ;
	      io_scan_for_input (0);
	      if (!io_input_avail ())
		io_redisp ();
	      io_flush ();
	      io_scan_for_input (0);
	    }
	  else
	    {
	      int timeout = (alarm_active
			     ? (alarm_seconds == 1
				? 1
				: (alarm_seconds / 2))
			     : 0);
			     
	      --current_cycle;
	      io_redisp ();
	      io_flush ();
	      io_scan_for_input (0);
	      if (!io_input_avail ())
		block_until_excitment (timeout);
	    }
	}
    }

  {
    int ret;
    ret = io_read_kbd (ibuf, sizeof (ibuf));
    if (ret == 1)
      {
	ch = ibuf[0];
	goto fini;
      }
    if (ret > 1)
      {
	i_cnt = 1;
	i_in = ret;
	ch = ibuf[0];
	goto fini;
      }
    if (ret == 0 || errno != EINTR)
      return EOF;
  }

fini:

  if (ch & 0x80)
    {
      saved_char = 1 + (ch & 0x7f);
      ch = CTRL ('[');
    }

  if (making_macro)
    {
      /* This is stoopid and should be fixed.
       * Macros (and other cell strings) should be 
       * `struct line' and not c-strings.   -tl
       */
      if (ch == 0)
	*making_macro++ = 0x80 | 'a';
      else if (ch == '{')
	*making_macro++ = 0x80 | 'b';
      else
	*making_macro++ = ch;
      if (making_macro >= (making_macro_start + making_macro_size))
	{
	  making_macro_start = ck_realloc (making_macro_start, 5
					   + making_macro_size * 2);
	  making_macro = (making_macro_start + making_macro_size);
	  making_macro_size *= 2;
	}
    }
  return ch;
}




/*****************************************************************
 * 
 * Command loops
 *
 * The basic cycle is that the user or a macro selects a function
 * (while oleo updates the display and evaluates cells).
 * A new command_frame is allocated in which to evaluate the selected 
 * function.   Arguments to the function will be stored in this frame.
 * The command loop interprets the FUNC_ARGS string of the selected function
 * and builds an argument list.  If the FUNC_ARGS string specifies that
 * the user must be prompted for an argument, an editting mode is entered
 * and the command loop restarts.  The queue of command_frames form
 * a stack of recursively invoked editting modes.
 *
 * When all of the arguments are ready, the command loop executes
 * the function and discards its frame.
 *
 * In principle, any number of command_frames can be created and they
 * could be evaluated in any order.  It is assumed in the code though that
 * the frame the_cmd_frame->prev is the frame the user was in when
 * the_cmd_frame was created (call it the `parent' frame).  Some arguments, 
 * for example the prefix argument and the current row/col, are taken from the
 * parent frame.  This is because those values may have changed in
 * the_cmd_frame as the user editted arguments to the function being called.
 */

/* The active command frame. This is the head of a queue which is used as a
 * stack. 
 */ 
struct command_frame * the_cmd_frame = 0;

/* This is a list (next field) of frames that are currently running (their 
 * commands are active on the c stack below the error_exception jump buffer).
 */
struct command_frame * running_frames = 0;


/* This is called when the current frame has keymapped
 * down to some function (stored in the_cmd_frame->_cur_cmd.
 * This pushes a new frame in which the arguments to that
 * command will be stored.
 *
 * This can also be called when the_cmd_frame is 0.  In that case,
 * it will create a top-level frame.
 *
 */

#ifdef __STDC__
void
push_command_frame (struct rng * rng, char * first_line, int len)
#else
void
push_command_frame (rng, first_line, len)
     struct rng * rng;
     struct line * first_line;
     int len;
#endif
{
  struct command_frame * new_cf =
    (struct command_frame *)ck_malloc (sizeof (*new_cf));

  new_cf->next = new_cf;
  new_cf->prev = new_cf;

  new_cf->input = (rng
		   ? macro_only_input_stream (rng, first_line, len, new_cf)
		   : default_input_stream ());

  new_cf->_setrow = NON_ROW;
  new_cf->_setcol = NON_COL;
  new_cf->_curow = MIN_ROW;
  new_cf->_cucol = MIN_COL;
  new_cf->_mkrow = NON_ROW;
  new_cf->_mkcol = NON_COL;
  new_cf->_input_active = 0;
  new_cf->_window_after_input = -1;

  /* These may be reset later. */
  new_cf->top_keymap = map_id ("main");
  if (new_cf->top_keymap < 0)
    new_cf->top_keymap = map_id ("universal");
  new_cf->saved_cur_keymap = -1;
  new_cf->_cur_keymap = map_id ("main");
  new_cf->_how_many = 1;
  new_cf->_cur_cmd = 0;
  new_cf->_cur_vector = 0;
  new_cf->_cur_chr = the_cmd_frame ? cur_chr : 0;
 
  init_line (&new_cf->_raw_prefix);
  new_cf->_cmd_argc = 0;
  new_cf->complex_to_user = 0;

  if (!the_cmd_frame)
    {
      /* This is a new top-level frame. */
      the_cmd_frame = new_cf;
      new_cf->cmd = 0;
      new_cf->top_keymap =  map_id ("main");
      if (new_cf->top_keymap < 0)
	new_cf->top_keymap = map_id ("universal");
    }
  else if (cur_cmd)
    {
      new_cf->_cur_arg = 0;
      new_cf->cmd = cur_cmd;
      {
	int argc = 0;
	char ** prompt = new_cf->cmd->func_args;
	while (prompt && *prompt)
	  {
	    new_cf->argv[argc].do_prompt = 0;
	    new_cf->argv[argc].is_set = 0;
	    new_cf->argv[argc].style = 0;
	    new_cf->argv[argc].arg_desc = *prompt;
	    new_cf->argv[argc].prompt = 0;
	    new_cf->argv[argc].expanded_prompt = 0;
	    new_cf->argv[argc].prompt_info = 0;
	    new_cf->argv[argc].info_line = 0;
	    init_line (&new_cf->argv[argc].text);
	    set_line (&new_cf->argv[argc].text, "");
	    new_cf->argv[argc].cursor = 0;
	    new_cf->argv[argc].overwrite = 0;
	    new_cf->argv[argc].inc_cmd = 0;
	    new_cf->argv[argc].timeout_seconds = 0;
	    bzero (&new_cf->argv[argc].val, sizeof (union command_arg_val));
	    ++argc;
	    ++prompt;
	  }
	if (argc && new_cf->argv[0].arg_desc[0] == '+')
	  ++new_cf->argv[0].arg_desc;
	new_cf->_cmd_argc = argc;
	new_cf->_curow = curow;
	new_cf->_cucol = cucol;
	new_cf->_mkrow = mkrow;
	new_cf->_mkcol = mkcol;
	new_cf->_setrow = setrow;
	new_cf->_setcol = setcol;
	if (!rng)
	  new_cf->input = the_cmd_frame->input;
      }
    }

  new_cf->prev = the_cmd_frame;
  new_cf->next = the_cmd_frame->next;
  new_cf->prev->next = new_cf;
  new_cf->next->prev = new_cf;
  the_cmd_frame = new_cf;
}

/* Remove a frame from the queue/stack. */
#ifdef __STDC__
void
remove_cmd_frame (struct command_frame * frame)
#else
void
remove_cmd_frame (frame)
     struct command_frame * frame;
#endif
{
  frame->next->prev = frame->prev;
  frame->prev->next = frame->next;
  if (the_cmd_frame == frame)
    the_cmd_frame = frame->prev;
  if (the_cmd_frame == frame)
    {
      the_cmd_frame = 0;
      push_command_frame (0, 0, 0);
    }
  frame->next = frame->prev = 0;
}


/* This frees all of the memory allocated to FRAME (including
 * the frame itself. 
 */
#ifdef __STDC__
void
free_cmd_frame (struct command_frame * frame)
#else
void
free_cmd_frame (frame)
     struct command_frame * frame;
#endif
{
  if (frame->next)
    remove_cmd_frame (frame);

  free_line (&frame->_raw_prefix);
  if (frame->cmd)
    {
      int argc;
      for (argc = 0; argc < frame->_cmd_argc; ++argc)
	{
	  if (frame->argv[argc].is_set && frame->argv[argc].style->destroy)
	    frame->argv[argc].style->destroy (&frame->argv[argc]);
	  free_line (&frame->argv[argc].text);
	  if (frame->argv[argc].expanded_prompt &&
	      (frame->argv[argc].expanded_prompt != frame->argv[argc].prompt))
	    free (frame->argv[argc].expanded_prompt);
	}
    }
  ck_free (frame);
}


/* Discard the current frame if it contains an unexecuted commnand. 
 * This is used, for example, to handle break.
 */
#ifdef __STDC__
void
pop_unfinished_command (void)
#else
void
pop_unfinished_command ()
#endif
{
  if (the_cmd_frame->cmd)
    {
      int move_cursor = 0;
      struct command_frame * frame = the_cmd_frame;
      if (   frame->_curow != frame->prev->_curow
	  || frame->_cucol != frame->prev->_cucol)
	{
	  io_hide_cell_cursor ();
	  move_cursor = 1;
	}
      remove_cmd_frame (frame);
      if (move_cursor)
	io_display_cell_cursor ();
      free_cmd_frame (frame);
    }
}

/* This is called if an error has been signaled with io_error_msg.
 * It discards any frames that the user has never interacted with
 * and cancels all pending macros.  This is properly followed by 
 * generating an error message for the user and longjmp to error_exception.
 */

#ifdef __STDC__
void
recover_from_error (void)
#else
void
recover_from_error ()
#endif
{
  /* pop input streams until the bottom is reached. */
  while (the_cmd_frame->input->prev_stream)
    pop_input_stream ();

  /* cancel the current macros */
  {
    struct input_stream * stream = the_cmd_frame->input;
    if (stream->_macro_start)
      free (stream->_macro_start);
    if (stream->_last_macro)
      free (stream->_last_macro);
    obstack_free (&stream->_macro_stack, 0);
    obstack_init (&stream->_macro_stack);
    stream->_rmac = 0;
    stream->_func_arg = 0;
    stream->_macro = stream->_macro_start = 0;
    stream->_macro_size = 0;
    stream->_pushed_back_char = -1;
  }

  /* pop command frames until an interactive one is reached. */
  while (the_cmd_frame->prev != the_cmd_frame
	 && !the_cmd_frame->complex_to_user)
    {
      struct command_frame * fr = the_cmd_frame;
      the_cmd_frame = the_cmd_frame->prev;
      free_cmd_frame (fr);
    }

  /* Discard any frames that were executing */
  while (running_frames)
    {
      struct command_frame * f = running_frames;
      running_frames = running_frames->next;
      f->next = 0;
      free_cmd_frame (f);
    }
}



/* When we begin editting a new argument, this function sets up the
 * appropriate keymap, and then resets the state of the editting commands.
 *
 * The return value is 1 if the user must be prompted, 0 otherwise.
 */
#ifdef __STDC__
static int
get_argument (char * prompt, struct prompt_style * style)
#else
static int
get_argument (prompt, style)
     char * prompt;
     struct prompt_style * style;
#endif
{
  the_cmd_arg.style = style;
  the_cmd_arg.prompt = prompt;
  if (!the_cmd_arg.expanded_prompt)
    the_cmd_arg.expanded_prompt = expand_prompt (prompt);
  the_cmd_frame->top_keymap = map_id (the_cmd_arg.style->keymap);
  the_cmd_arg.is_set = 0;
  the_cmd_arg.do_prompt = 1;
  if (the_cmd_frame->top_keymap < 0)
    the_cmd_frame->top_keymap = map_id ("universal");
  if (macro_func_arg)
    {
      set_line (&the_cmd_arg.text, macro_func_arg);
      {
	char * arg_ptr;
	char * error;
	arg_ptr = the_cmd_arg.text.buf;
	error = the_cmd_arg.style->verify (&arg_ptr, &the_cmd_arg);
	if (error)
	  {
	    macro_func_arg = 0;
	    io_error_msg ("%s", error);
	  }
	else
	  {
	    the_cmd_arg.is_set = 1;
	    if (arg_ptr)
	      while (isspace (*arg_ptr))
		++arg_ptr;
	    if (arg_ptr && *arg_ptr)
	      macro_func_arg = arg_ptr;
	    else
	      macro_func_arg = 0;
	    return 0;
	  }
      }
    }
  input_active = 1;
  begin_edit ();

  /* Functions can come with macros that initialize arguments for the user. 
   * As for the call to expand_prompt -- hehehehehe
   */
  if (the_cmd_frame->cmd->init_code && the_cmd_frame->cmd->init_code[cur_arg])
    {
      char * init_code = expand_prompt(the_cmd_frame->cmd->init_code[cur_arg]);
      struct rng rng;
      rng.lr = rng.hr = rng.lc = rng.hc = 1;
      macro_only_input_stream (&rng, init_code, strlen (init_code),
			       the_cmd_frame);
      command_loop (1);
    }

  return 1;
}

#ifdef __STDC__
void
exit_minibuffer (void)
#else
void
exit_minibuffer ()
#endif
{
  if (check_editting_mode ())
    return;
  else
    {
      char * extra = the_cmd_arg.text.buf;
      char * error = the_cmd_arg.style->verify (&extra, &the_cmd_arg);
      if (error)
	{
	  if (*error)
	    io_error_msg ("%s", error);
	}
      else
	{
	  if (extra)
	    {
	      while (isspace (*extra))
		++extra;
	      if (*extra)
		io_error_msg ("%s: extra characters in argument (%s)",
			      the_cmd_frame->cmd->func_name, extra);
	    }
	  the_cmd_arg.is_set = 1;
	  input_active = 0;
	  window_after_input = -1;
	  topclear = 2;
	}
    }
}




#ifdef __STDC__
void
setn_arg_text (struct command_arg * arg, char * text, int len)
#else
void
setn_arg_text (arg, text, len)
     struct command_arg * arg;
     char * text;
     int len;
#endif
{
  setn_line (&arg->text, text, len);
  arg->cursor = len;
}

#ifdef __STDC__
void
init_arg_text (struct command_arg * arg, char * text)
#else
void
init_arg_text (arg, text)
     struct command_arg * arg;
     char * text;
#endif
{
  setn_arg_text (arg, text, strlen (text));
}

/* This apparently useless alias is here because
 * sometime in the future i want to handle defaults
 * differently.  
 */

#ifdef __STDC__
void
set_default_arg (struct command_arg * arg, char * text, int len)
#else
void
set_default_arg (arg, text, len)
     struct command_arg * arg;
     char * text;
     int len;
#endif
{
  setn_arg_text (arg, text, len);
}

/* This is the main loop of oleo.   It reads commands and their arguments, and
 * evaluates them.  It (via real_get_chr) udpates the display and performs
 * background recomputation.
 *
 * This function can also be used to evaluate a function without doing any
 * interaction.  This is done by pushing a macro_only command frame
 *  (see execute_command).
 */

#ifdef __STDC__
void
command_loop (int prefix)
#else
void
command_loop (prefix)
     int prefix;
#endif
{

  /* We might be re-entering after a longjmp caused by an error.
   * In that case, we use an alternate entry point:
   */
  if (the_cmd_frame->cmd)
    goto resume_getting_arguments;

  /* Commands (notably execute_command) just tweek the command_frame
   * state for some other command.  To accomplish this, there is an 
   * entry point that avoid reinitializing the command_frame.\
   */
  if (prefix)
    {
      prefix = 0;
      goto prefix_cmd_continuation;
    }
  
  while (1)
    {
      int ch;			/* The next character to be keymapped. */

    new_cycle:

      if (!the_cmd_frame)
	push_command_frame (0, 0, 0);

      /* Reset the prefix argument. */

      how_many = 1;
      set_line (&raw_prefix, "");
      io_update_status ();

      /* Reset the keystate. */
      cur_keymap = the_cmd_frame->top_keymap;

      /* If the input area holds a prompt from the last command,
       * erase it. 
       */
      io_clear_input_before ();


      /* Some commands are prefix commands: they effect the 
       * user's state without beginnging a new command cyle.
       * Those commands return here:
       */

    prefix_cmd_continuation:

      /* In this loop, we look for the next command to
       * execute.  This may involve reading from a macro, 
       * or the keyboard.  If there is time to kill, updates
       * and evalutations are done.
       *
       * This loop is exited by `goto got_command'.
       */

      while (1)
	{
	  /* Get the next character.
	   * However, if we are in a macro, and the next character
	   * is '{', then the macro contains a function name
	   * and keymapping is circumvented. 
	   */

	get_next_char:

	  if (pushed_back_char >= 0)
	    {
	      ch = pushed_back_char;
	      pushed_back_char = -1;
	    }
	  else if (!rmac)
	    {
	      io_fix_input ();
	      ch = real_get_chr ();
	    }
	  else
	    {
	      int len;
	      unsigned char *ptr;
	      
	    tryagain:
	      alarm_hooks ();
	      ch = *(rmac->mac_exe++);
	      switch (ch)
		{
		case '\0':
		  cur_vector = 0;
		  cur_cmd = end_macro_cmd;
		  cur_chr = 0;
		  goto got_command;
		  
		case 0x80 | 'a':
		  ch = '\0';
		  break;
		  
		case 0x80 | 'b':
		  ch = '{';
		  break;
		case 0x80 | 'c':
		  ch = '}';
		  break;
		  
		case '{':
		  for (ptr = rmac->mac_exe;
		       *ptr && *ptr != ' ' && *ptr != '}';
		       ptr++);
		  len = ptr - rmac->mac_exe;
		  for (cur_vector = 0;
		       cur_vector < num_funcs;
		       cur_vector++)
		    for (cur_cmd =
			 &the_funcs[cur_vector][0];
			 cur_cmd->func_name;
			 cur_cmd++)
		      if (!strincmp ((char *) (rmac->mac_exe),
				     cur_cmd->func_name, len)
			  && cur_cmd->func_name[len] == '\0')
			{
			  cur_chr = '\0';
			  goto out;
			}
		  io_error_msg ("Ignoring unknown function '%.*s' in macro",
				len, rmac->mac_exe);
		  while (*ptr != '\0' && *ptr != '}')
		    ptr++;
		  if (*ptr == '}')
		    ptr++;
		  rmac->mac_exe = ptr;
		  goto tryagain;
		  
		out:
		  if (*ptr == ' ')
		    {
		      /* ... add argument support here ... */
		      if (!cur_cmd->func_args)
			{
			  io_error_msg ("Ignoring extra operand to %s",
					cur_cmd->func_name);
			  while (*ptr && *ptr != '}')
			    ptr++;
			  if (*ptr == '}')
			    ptr++;
			}
		      else if (cur_cmd->func_args[0][0] == '+')
			{
			  unsigned char * start = ptr;
			  how_many = astol ((char **) (&ptr));
			  setn_line (&raw_prefix, (char *)start,
				     ptr - start);
			  if (*ptr == '}')
			    ptr++;
			}
		      else
			{
			  while (isspace(*ptr))
			    ++ptr;
			  macro_func_arg = (char *) ptr;
			  while (*ptr && *ptr != '}')
			    {
			      switch (*ptr)
				{
				case 0x80 | 'b':
				  *ptr = '{';
				  break;
				case 0x80 | 'c':
				  *ptr = '}';
				  break;
				}
			      ptr++;
			    }
			  if (*ptr == '}')
			    *ptr++ = '\0';
			}
		      rmac->mac_exe = ptr;
		    }
		  else
		    rmac->mac_exe += len + 1;
		  goto got_command;
		}
	    }

	  /* When control comes here, adjust the keystate according 
	   * to the cur_keymap and `ch';
	   */
	have_character:

	  /* This is how keymaps are searched for a binding. */ 
	  while (1)
	    {
	      struct key * key;
	      key = &(the_maps[cur_keymap]->keys[ch]);
	      if (key->vector < 0)
		{
		  if (key->code >= 0)
		    {
		      cur_keymap = key->code;
		      goto get_next_char;
		    }
		  else if (the_maps[cur_keymap]->map_next)
		    cur_keymap =
		      the_maps[cur_keymap]->map_next->id;
		  else
		    {
		      cur_vector = 0;
		      cur_cmd = 0;
		      cur_chr = ch;
		      goto got_command;
		    }
		}
	      else
		{
		  cur_vector = key->vector;
		  cur_cmd =
		    &(the_funcs[key->vector][key->code]);
		  cur_chr = ch;
		  goto got_command;
		}
	    }
	}


      /* Now the next command to begin has been read from a macro
       * or the keyboard.
       */
    got_command:

      /* If there is anything left in the input area (e.g. old error message)
       * get rid of it (but only if we're not executing a macro).
       */
      if (!rmac)
	io_clear_input_after ();

      /* There are some commands that are implemented right here. */
      if (cur_cmd == break_cmd)
	{
	  io_bell ();
	  set_info (0);
	  if (input_active)
	    pop_unfinished_command ();	  /* Abort a complex command.*/
	  goto new_cycle;
	}

      /* The binding of all keys associated with the prefix arg. */
      if (cur_cmd == universal_arg_cmd)
  	{
	  char ch = cur_chr;
	  int prefix_map = map_id ("prefix");
	  /* Make sure the prefix-arg keymap is in place. */
	  if (cur_keymap != prefix_map)
	    {
	      the_cmd_frame->saved_cur_keymap = the_cmd_frame->top_keymap;
	      cur_keymap = prefix_map;
	    }
	  /* Store the last character typed in the raw-prefix.*/
	  catn_line (&raw_prefix, &ch, 1);
	  /* Recompute the numeric value of the prefix. */
	  {
	    int x = 0;
	    int presumed_digits = 0;
	    int sign = 1;

	    how_many = 1;
	    while (raw_prefix.buf[x])
	      {
		if (isdigit (raw_prefix.buf[x]))
		  {
		    if (presumed_digits)
		      how_many = how_many * 10 + (raw_prefix.buf[x] - '0');
		    else
		      {
			presumed_digits = 1;
			how_many = raw_prefix.buf[x] - '0';
		      }
		  }
		else if (raw_prefix.buf[x] == '-')
		  sign *= -1;
		else
		  {
		    if (presumed_digits)
		      {
			presumed_digits = 0;
			how_many = 1;
		      }
		    how_many *= 4;
		  }
		++x;
	      }
	    how_many *= sign;
	    io_update_status ();
	    goto prefix_cmd_continuation; 
	  }
	}

      /* Make sure we really mapped to a command. */
      if (!cur_cmd || !cur_cmd->func_func)
	{
	  /* If a character is unmapped in the prefix map,
	   * retry mapping in the last-used normal keymap.
	   */
	  if (the_cmd_frame->saved_cur_keymap >= 0)
	    {
	      cur_keymap = the_cmd_frame->saved_cur_keymap;
	      the_cmd_frame->saved_cur_keymap = -1;
	      goto have_character;
	    }
	  /* Otherwise, signal an error and start from the top keymap. */
	  io_bell ();
	  goto new_cycle;
	}
      


      /* The next step is to gather the arguments with which to call
       * the function interactively.
       */
      /* Whever a new command is encountered, we begin by creating a 
       * frame in which to store it's arguments.
       * This initializes the new frame on the basis of cur_cmd in 
       * the_cmd_frame.  
       */
      push_command_frame (0, 0, 0);

      /* After some other command finishes from underneath a complex command,
       * flow returns here. 
       */
      
    resume_getting_arguments:

      while (cur_arg < cmd_argc)
	{
	  if (the_cmd_arg.is_set)
	    goto next_arg;
	  else if (the_cmd_arg.prompt)
	    {
	      begin_edit ();
	      goto new_cycle;
	    }
	  else
	    {
	      /* If we're just starting on this argument, then parse the
	       * FUNC_ARGS string. To continue this loop, use `goto next_arg;'.
	       *
	       * If user interaction is required, the appropriate keymap,
	       * editting area, etc. is set up, and the command loop resumes
	       * (`goto new_cycle').
	       */
	      char * prompt = the_cmd_arg.arg_desc;
	      
	      switch (*prompt)
		{
		case 'c':
		  {
		    ++prompt;
		    if (*prompt == '#')
		      {
			++prompt;
			the_cmd_arg.val.integer = *prompt;
			the_cmd_arg.is_set = 1;
			the_cmd_arg.do_prompt = 0;
			the_cmd_arg.style = &int_constant_style;
			{
			  char c[2];
			  c[0] = cur_chr;
			  c[1] = '\0';
			  init_arg_text (&the_cmd_arg, c);
			}
			goto next_arg;
		      }
		    else if (*prompt == '\'')
		      {
			the_cmd_arg.timeout_seconds = 30;
			alarm_table[1].freq = 1;
			++prompt;
		      }
		    if (get_argument (prompt, &char_style))
		      goto new_cycle;
		    goto next_arg;
		  }
		case 'C':
		  {
		    ++prompt;
		    if (get_argument (prompt, &command_style))
		      goto new_cycle;
		    goto next_arg;
		  }
		case 'd':
		  {
		    ++prompt;
		    if (get_argument (prompt, &double_style))
		      goto new_cycle;
		    goto next_arg;
		  }
		case 'f':
		  {
		    char type;
		    struct prompt_style * style;
		    ++prompt;
		    type = *prompt;
		    ++prompt;
		    switch (type)
		      {
		      case 'r':
			style = &read_file_style;
			break;
		      case 'w':
			style = &write_file_style;
			break;
		      case 'n':
			style = &file_name_style;
			break;
		      default:
			style = 0; /* shutup gcc -ansi -pendantic -Wall! */
			io_error_msg ("func_args bug for %s",
				      the_cmd_frame->cmd->func_name);
		      }
		    if (get_argument (prompt, style))
		      goto new_cycle;
		    goto next_arg;
		  }
		case 'F':
		  {
		    ++prompt;
		    if (get_argument (prompt, &format_style))
		      goto new_cycle;
		    goto next_arg;
		  }
		case 'k':
		  {
		    ++prompt;
		    the_cmd_arg.val.key.cmd.vector = -1;
		    the_cmd_arg.val.key.cmd.code
		      = the_cmd_frame->prev->top_keymap;
		    the_cmd_arg.val.key.keys = &the_cmd_arg.text;
		    if (get_argument (prompt, &keyseq_style))
		      goto new_cycle;
		    goto next_arg;
		  }
		case 'K':
		  {
		    ++prompt;
		    if (get_argument (prompt, &keymap_style))
		      goto new_cycle;
		    goto next_arg;
		  }
		case 'l':
		  {
		    the_cmd_arg.val.integer = cur_chr;
		    the_cmd_arg.is_set = 1;
		    the_cmd_arg.do_prompt = 0;
		    the_cmd_arg.style = &int_constant_style;
		    {
		      char c[2];
		      c[0] = cur_chr;
		      c[1] = '\0';
		      init_arg_text (&the_cmd_arg, c);
		    }
		    goto next_arg;
		  }
		case 'm':
		  {
		    int want_keyseq = 0;
		    ++prompt;
		    want_keyseq = (*prompt == '\'');
		    if (want_keyseq)
		      {
			char * map;
			++prompt;
			map = expand_prompt (prompt);
			the_cmd_arg.val.key.cmd.vector = -1;
			the_cmd_arg.val.key.cmd.code = map_id (map);
			the_cmd_arg.val.key.keys = &the_cmd_arg.text;
		      }
		    else
		      {
			if (mode_style.keymap)
			  ck_free (mode_style.keymap);
			mode_style.keymap = expand_prompt (prompt);
		      }
		    if (get_argument (prompt, (want_keyseq
					       ? &keyseq_style
					       : &mode_style)))
		      goto new_cycle;
		    goto next_arg;
		  }
		case 'M':
		  if (modified)
		    {
		      ++prompt;
		      if (get_argument (prompt, &yes_style))
			goto new_cycle;
		      goto next_arg;
		    }
		  else
		    {
		      the_cmd_arg.is_set = 1;
		      the_cmd_arg.do_prompt = 1;
		      the_cmd_arg.style = &yes_style;
		      init_arg_text (&the_cmd_arg, "yes");
		      goto next_arg;
		    }
		case 'p':
		  {
		    ++prompt;
		    switch (*prompt)
		      {
		      default:
			the_cmd_arg.val.integer
			  = the_cmd_frame->prev->_how_many;
			the_cmd_arg.is_set = 1;
			the_cmd_arg.do_prompt = 0;
			the_cmd_arg.style = &int_constant_style;
			init_arg_text
			  (&the_cmd_arg,
			   long_to_str ((long)the_cmd_arg.val.integer)); 
			break;

		      case '?':	/* Command wants to know if prefix provided */
			the_cmd_arg.val.integer =
			  (the_cmd_frame->prev->_raw_prefix.alloc
			   && the_cmd_frame->prev->_raw_prefix.buf[0]);
			the_cmd_arg.is_set = 1;
			the_cmd_arg.do_prompt = 0;
			the_cmd_arg.style = &int_constant_style;
			init_arg_text (&the_cmd_arg,
				       the_cmd_arg.val.integer ? "1" : "0");
			break;
		      }
		    goto next_arg;
		  }
		case 'N':
		case 'n':
		  {
		    long low = 0;
		    long high = -1;
		    char type = *prompt;
		    ++prompt;
		    if (*prompt == '[')
		      {
			++prompt;
			low = astol (&prompt);
			while (isspace (*prompt))  ++prompt;
			if (*prompt == ',') ++prompt;
			high = astol (&prompt);
			while (isspace (*prompt))  ++prompt;
			if (*prompt == ']') ++prompt;
		      }
		    if (   (type == 'N')
			&& the_cmd_frame->prev->_raw_prefix.alloc
			&& the_cmd_frame->prev->_raw_prefix.buf[0])
		      {
			the_cmd_arg.val.integer
			  = the_cmd_frame->prev->_how_many;
			the_cmd_arg.is_set = 1;
			the_cmd_arg.do_prompt = 1;
			the_cmd_arg.style = &number_style;
			if (   (low >= high)
			    && (   (low > the_cmd_arg.val.integer)
				|| (high < the_cmd_arg.val.integer)))
			  io_error_msg
			    ("Out of range %d (should be in [%d-%d]).");
			else
			  init_arg_text
			    (&the_cmd_arg,
			     long_to_str ((long)the_cmd_arg.val.integer));
		      }
		    else
		      {
			if (get_argument (prompt, &number_style))
			  goto new_cycle;
		      }
		    goto next_arg;
		  }
		case 'r':
		case 'R':
		  {
		    if (*prompt != 'R' && mkrow != NON_ROW)
		      {
			the_cmd_arg.val.range.lr = MIN(mkrow, curow);
			the_cmd_arg.val.range.hr = MAX(mkrow, curow);
			the_cmd_arg.val.range.lc = MIN(mkcol, cucol);
			the_cmd_arg.val.range.hc = MAX(mkcol, cucol);
			the_cmd_arg.is_set = 1;
			the_cmd_arg.do_prompt = 1;
			the_cmd_arg.style = &range_style;
			mkrow = NON_ROW;
			mkcol = NON_COL;
			init_arg_text (&the_cmd_arg,
				       range_name (&the_cmd_arg.val.range)); 
			goto next_arg;
		      }
		    else
		      {
			++prompt;
			if (get_argument (prompt, &range_style))
			  goto new_cycle;
			goto next_arg;
		      }
		  }
		case 's':
		  {
		    {
		      ++prompt;
		      if (get_argument (prompt, &string_style))
			goto new_cycle;
		      goto next_arg;
		    }
		  }
		case 'S':
		  {
		    {
		      ++prompt;
		      if (*prompt == '\'')
			++prompt;
		      if (get_argument (prompt, &symbol_style))
			goto new_cycle;
		      goto next_arg;
		    }
		  }
		case 'V':
		  {
		    ++prompt;
		    the_cmd_arg.inc_cmd = io_shift_cell_cursor;
		    if (get_argument (prompt, &inc_cmd_style))
		      goto new_cycle;
		    goto next_arg;
		  }
		case 'w':
		  {
		    {
		      ++prompt;
		      if (*prompt == '\'')
			++prompt;
		      if (get_argument (prompt, &word_style))
			goto new_cycle;
		      goto next_arg;
		    }
		  }
		case '#':
		  {
		    ++prompt;
		    
		    init_arg_text (&the_cmd_arg, prompt);
		    the_cmd_arg.val.integer = astol(&prompt);
		    the_cmd_arg.is_set = 1;
		    the_cmd_arg.do_prompt = 0;
		    the_cmd_arg.style = &int_constant_style;
		    goto next_arg;
		  }
		case '=':
		  {
		    ++prompt;
		    the_cmd_arg.expanded_prompt = expand_prompt(prompt);
		    init_arg_text (&the_cmd_arg, the_cmd_arg.expanded_prompt);
		    the_cmd_arg.val.string = the_cmd_arg.expanded_prompt;
		    the_cmd_arg.is_set = 1;
		    the_cmd_arg.do_prompt = 0;
		    the_cmd_arg.style = &string_style;
		    goto next_arg;
		  }
		case '.':
		  {
		    ++prompt;
		    the_cmd_arg.val.range.lr = curow;
		    the_cmd_arg.val.range.lc = cucol;
		    if (*prompt == '\'')
		      {
			the_cmd_arg.val.range.hr = curow;
			the_cmd_arg.val.range.hc = cucol;
		      }
		    else
		      {
			the_cmd_arg.val.range.hr = mkrow;
			the_cmd_arg.val.range.hc = mkcol;
		      }
		    the_cmd_arg.is_set = 1;
		    the_cmd_arg.do_prompt = 0;
		    init_arg_text (&the_cmd_arg,
				   range_name (&the_cmd_arg.val.range));
		    the_cmd_arg.style = &range_constant_style;
		    goto next_arg;
		  }
		case '[':
		  {
		    ++prompt;
		    while (*prompt && (*prompt != ']'))
		      if (*prompt != '\\')
			++prompt;
		      else
			{
			  ++prompt;
			  if (*prompt)
			    ++prompt;
			}
		    if (*prompt == ']')
		      ++prompt;
		    
		    if (get_argument (prompt, &menu_style))
		      goto new_cycle;
		    goto next_arg;
		  }
		case '$':
		  {
		    /* Edit a cell's formula. */
		    CELL * cp = find_cell (curow, cucol);
		    ++prompt;
		    if (((!cp || GET_LCK (cp) == LCK_DEF)
			 && (default_lock == LCK_LCK))
			|| (cp && GET_LCK (cp) == LCK_LCK))
		      {
			io_error_msg ("Cell %s is locked",
				      cell_name (curow, cucol));
			pop_unfinished_command ();
			goto new_cycle;
		      }
		    the_cmd_frame->prev->_setrow = curow;
		    the_cmd_frame->prev->_setcol = cucol;
		    if (get_argument (prompt, &formula_style))
		      {
			init_arg_text (&the_cmd_arg,
				       decomp (curow, cucol, cp));
			decomp_free ();
			goto new_cycle;
		      }
		    goto next_arg;
		  }
		  
		default:
		  {
		    io_error_msg ("Interaction-string error!!!");
		    pop_unfinished_command ();
		    goto new_cycle;
		  }
		}
	    }
	next_arg:
	  ++cur_arg;
	}

      /* Make sure that all the args are really there. */
      for (cur_arg = 0; cur_arg < cmd_argc; ++cur_arg)
	if (the_cmd_arg.do_prompt && !the_cmd_arg.is_set)
	  goto resume_getting_arguments;

      /* If this point is reached, call the interactive function,
       * destroy it's frame, and restart the cycle.
       */
      {
	int move_cursor = 0;
	struct command_frame * frame = the_cmd_frame;
	cmd_invoker stub = find_stub ();
	if (   frame->_curow != frame->prev->_curow
	    || frame->_cucol != frame->prev->_cucol)
	  {
	    move_cursor = 1;
	    io_hide_cell_cursor ();
	  }
	remove_cmd_frame (frame);
	/* Add frame to the list of frames to be freed on error. */
	frame->next = running_frames;
	running_frames = frame;
	if (move_cursor)
	  io_display_cell_cursor ();
	if (!stub)
	  io_error_msg ("Don't know how to invoke %s!!!",
			frame->cmd->func_name);
	else
	  stub (frame);
	running_frames = running_frames->next;
	frame->next = 0;
	free_cmd_frame (frame);

	/* If command_loop was called by execute_command, it should
	 * return as soon as there is no more macro to evaluate.
 	 */
	if (!rmac && the_cmd_frame->input->prev_stream)
	  {
	    pop_input_stream ();
	    return;
	  }
	if (the_cmd_frame->cmd)
	  goto resume_getting_arguments;
      }
    }
}

/* Exectute the command called in `string'.
 * If the string begins with a proper command name,
 * it is executed as if it were embedded in "{}" in 
 * a macro.  Otherwise, if the string can be interpreted 
 * as a range address, the macro at that address is executed.
 */

static struct line exec_cmd_line = {0, 0};

#ifdef __STDC__
void
execute_as_macro (char * str)
#else
void
execute_as_macro (str)
     char * str;
#endif
{
  struct rng fake_rng;
  fake_rng.lr = fake_rng.lc = fake_rng.hr = fake_rng.hc = 1;
  macro_only_input_stream (&fake_rng, str, strlen (str), the_cmd_frame);
  command_loop (1);
}


/* execute_command buils a macro expression of the from `{command args}'.
 * This function quotes the braces in ARGS so that the macro reader knows
 * they are literal rather than macro syntax.
 */
static void
quote_macro_args (args)
     char * args;
{
  while (*args)
    {
      switch (*args)
	{
	case '{':
	  *args = 0x80 | 'b';
	  break;
	case '}':
	  *args = 0x80 | 'c';
	  break;
	}
      ++args;
    }
}

#ifdef __STDC__
void 
execute_command (char *str, int count)
#else
void 
execute_command (str, count)
     char *str;
     int count;
#endif
{
  char *ptr = str;
  char * run;			/* The first string to execute. */
  /* The address of the macro to execute.  If the user typed a 
   * command name and not a range name, then this range will be
   * set to a one cell region.
   */
  struct rng rng;

  /* Chop off the first word. */
  while (isspace (*str))
    ++str;
  if (!*str || *str == '#')
    return;
  for (ptr = str; *ptr && *ptr != ' '; ptr++);
  
  if (*ptr)
    *ptr++ = '\0';
  else
    ptr = 0;
  

  /* First, look for a command name. */
  
  {
    int vector;
    struct cmd_func * cmd;

    if (!find_function (&vector, &cmd, str, strlen(str)))
      {
	if (ptr)
	  {
	    quote_macro_args (ptr);
	    sprint_line (&exec_cmd_line, "{%s %s}", str, ptr);
	  }
	else
	  sprint_line (&exec_cmd_line, "{%s}", str);
	run = exec_cmd_line.buf;
	rng.lr = rng.hr = 1;
	rng.lc = rng.hc = 1;
	goto found_command;
      }
  }
  
  {
    /* Try for a range address. */
    CELL *cp;
    if (get_abs_rng (&str, &rng))
      {
	io_error_msg ("Unknown command %s", str);
	return;
      }
    if (ptr)
      {
	io_error_msg ("Macros can't take arguments");
	return;
      }

    cp = find_cell (rng.lr, rng.lc);
    if (!cp
	|| GET_TYP (cp) != TYP_STR
	|| cp->cell_str[0] == '\0')
      {
	io_error_msg ("No macro found at %s.", range_name (&rng));
	return;
      }

    run = cp->cell_str;
    count = how_many;
    how_many = 1;
    set_line (&raw_prefix, "");
  }

 found_command:
  while (count --)
    {
      macro_only_input_stream (&rng, run, strlen (run), the_cmd_frame);
      command_loop (1);
    }
}




/* Read a character.  If we're in a macro, read from the macro. . . */
#ifdef __STDC__
int
get_chr (void)
#else
int
get_chr ()
#endif
{
  int ch;

  if (rmac)
    {
      ch = *(rmac->mac_exe++);
      switch (ch)
	{
	case '{':		/* What else can we do? */
	case '\0':
	  --rmac->mac_exe;
	  break;

	case (0x80 | 'a'):
	  ch = 0;
	  break;

	case (0x80 | 'b'):
	  ch = '{';
	  break;
	default:
	  break;
	}
    }
  else
    ch = real_get_chr ();
  return ch;
}


/* This is an entirely magical function.  All of it's work is done 
 * by the argument prompting system.  All that remains to be done
 * when this is called is to push back a character the user may have
 * typed to cause the error message to go away.
 */

#ifdef __STDC__
void
display_error_msg (char * msg, int c)
#else
void
display_error_msg (msg, c)
     char * msg;
     int c;
#endif
{
  if (c > 0)
    pushed_back_char = c;
}

#ifdef __STDC__
void
io_error_msg (char *str,...)
#else
void
io_error_msg (str, va_alist)
     char *str;
     va_dcl
#endif
{
  va_list foo;
  char buf[1000];
  char buf2[1000];

  /* This is made robust against errors that occur before
   * the io hooks have been initialized.
   */
  if (display_opened)
    io_bell ();
  
  var_start (foo, str);
  vsprintf (buf, str, foo);
  sprintf (buf2, "display-error-msg %s", buf);
  recover_from_error ();
  if (display_opened)
    execute_command (buf2, 1);
  else
    fprintf (stderr, "oleo: %s\n", buf);
  longjmp (error_exception, 1);
}


#ifdef __STDC__
void
io_info_msg (char *str,...)
#else
void
io_info_msg (str, va_alist)
     char *str;
     va_dcl
#endif
{
  va_list foo;
  char buf[1000];
  char buf2[1000];

  var_start (foo, str);
  vsprintf (buf, str, foo);
  sprintf (buf2, "display-error-msg %s", buf);
  execute_command (buf2, 1);
}




/* Expands a string that will be used to prompt for an argument.
 *    %n expands to the text of argument n (if defined -- ??? otherwise).
 *    %% expands to %
 *    %c expands to the name of the_cmd_frame->prev->_set{row,col}
 * If no expansion is needed, the argument is returned.  Otherwise,
 * malloced memory is returned.
 */

#ifdef __STDC__
char *
expand_prompt (char * str)
#else
char *
expand_prompt (str)
     char * str;
#endif
{
  struct line expanded;
  init_line (&expanded);
  if (!str || !index (str, '%'))
    return ck_savestr (str);
  {
    char * last_pos = str;
    char * src_pos;

    for (src_pos = index (str, '%'); src_pos; src_pos = index (src_pos, '%'))
      {
	catn_line (&expanded, last_pos, src_pos - last_pos);
	++src_pos;
	switch (*src_pos)
	  {
	  case '%':
	    catn_line (&expanded, src_pos, 1);
	    ++src_pos;
	    break;
	  case 'c':
	    {
	      struct rng rng;
	      char * str;
	      rng.lr = rng.hr = the_cmd_frame->prev->_setrow;
	      rng.lc = rng.hc = the_cmd_frame->prev->_setcol;
	      str = range_name (&rng);
	      catn_line (&expanded, str, strlen(str));
	      ++src_pos;
	      break;
	    }
	  case '.':
	    {
	      struct rng rng;
	      char * str;
	      rng.lr = rng.hr = the_cmd_frame->prev->_curow;
	      rng.lc = rng.hc = the_cmd_frame->prev->_cucol;
	      str = range_name (&rng);
	      catn_line (&expanded, str, strlen(str));
	      ++src_pos;
	      break;
	    }
	  case '0': case '1': case '2': case '3': case '4':
	  case '5': case '6': case '7': case '8': case '9':
	    {
	      int argn = *src_pos - '0';
	      if (   (cmd_argc > argn)
		  && the_cmd_frame->argv[argn].is_set
		  && the_cmd_frame->argv[argn].text.buf)
		catn_line (&expanded, the_cmd_frame->argv[argn].text.buf,
			   strlen (the_cmd_frame->argv[argn].text.buf));
	      else
		catn_line (&expanded, "????", 4);
	      ++src_pos;
	      break;
	    }
	  default:
	    catn_line (&expanded, "%", 1);
	    break;
	  }
	last_pos = src_pos;
      }
    catn_line (&expanded, last_pos, strlen(last_pos));
  }
  return expanded.buf;
}




/* Info commands */

#ifdef __STDC__
void
set_info (char * name)
#else
void
set_info (name)
     char * name;
#endif
{
  struct info_buffer * ib = (name ? find_or_make_info (name) : 0);
  
  if (the_cmd_frame->cmd && (the_cmd_arg.prompt_info != ib))
    {
      the_cmd_arg.info_line = 0;
      the_cmd_arg.prompt_info = ib;
    }
  if (!ib && name && *name)
    io_error_msg ("No information about %s.", name);
}


#ifdef __STDC__
void
page_info_backwards (int rep)
#else
void
page_info_backwards (rep)
     int rep;
#endif
{
  if (rep < 0)
    page_info (-rep);
  else if (the_cmd_frame->cmd && the_cmd_arg.prompt_info)
    {
      int vis_lines = (scr_lines - input_rows) / info_rows;
      int next = the_cmd_arg.info_line - vis_lines * rep;
      the_cmd_arg.info_line = ((next >= 0) ? next : 0);
    }
  else
    io_error_msg ("No info to page.");
}

#undef MAX
#define MAX(A,B) (((A) >= (B)) ? (A) : (B))

#ifdef __STDC__
void
page_info (int rep)
#else
void
page_info (rep)
     int rep;
#endif
{
  if (rep < 0)
    page_info_backwards (-rep);
  else if (the_cmd_frame->cmd && the_cmd_arg.prompt_info)
    {
      int vis_lines = (scr_lines - input_rows) / info_rows;
      int next = the_cmd_arg.info_line + vis_lines * rep;
      the_cmd_arg.info_line =
	((next >= the_cmd_arg.prompt_info->len)
	 ? MAX(0, (the_cmd_arg.prompt_info->len - vis_lines))
	 : next);
    }
  else
    io_error_msg ("No info to page.");
}

#ifdef __STDC__
void
view_info (char * name, int ignore)
#else
void
view_info (name, ignore)
     char * name;
     int ignore;
#endif
{}



/* The C part of this function is uninteresting.  The interesting part
 * is in defun.h.
 */

#ifdef __STDC__
void
with_keymap (char * mapname)
#else
void
with_keymap (mapname)
     char * mapname;
#endif
{}

#ifdef __STDC__
void
one_cmd_with_keymap (char * mapname, struct key_sequence * keyseq)
#else
void
one_cmd_with_keymap (mapname, keyseq)
     char * mapname;
     struct key_sequence * keyseq;
#endif
{
  if (keyseq->cmd.vector < 0 && keyseq->cmd.code < 0)
    io_bell ();
  else if (keyseq->cmd.vector < 0)
    io_error_msg
      ("one-command-with-keymap: %s maps to a keymap (%s), not a command.",
       keyseq->keys->buf, map_names[keyseq->cmd.code]);
  else
    execute_command
      (the_funcs[keyseq->cmd.vector][keyseq->cmd.code].func_name, 1);
}
