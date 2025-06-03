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


#include "global.h"
#include "input.h"
#include "cmd.h"
#include "window.h"
#include "io-abstract.h"


/* In the functions below, we only ever deal with one input_view at a 
 * time.  By convention, call a pointer to it THIS_IV and use these
 * macros: 
 */
#define redraw_needed 		(this_iv->redraw_needed)
#define prompt_metric 		(this_iv->prompt_metric)
#define input_metric 		(this_iv->input_metric)
#define keymap_prompt 		(this_iv->keymap_prompt)
#define expanded_keymap_prompt 	(this_iv->expanded_keymap_prompt)
#define prompt_wid 		(this_iv->prompt_wid)
#define must_fix_input 		(this_iv->must_fix_input)
#define input_area 		(this_iv->input_area)
#define prompt 			(this_iv->prompt)
#define visibility_begin 	(this_iv->visibility_begin)
#define visibility_end 		(this_iv->visibility_end)
#define current_info		(this_iv->current_info)
#define info_pos		(this_iv->info_pos)
#define info_redraw_needed	(this_iv->info_redraw_needed)
#define vis_wid			(this_iv->vis_wid)
#define input_cursor 		(this_iv->input_cursor)



#define Max(A,B)  ((A) < (B) ? (B) : (A))
#define Min(A,B)  ((A) > (B) ? (B) : (A))


/* Decide if a keymap prompt should be displayed.  If so, 
 * return the prompt, else 0.
 */

#ifdef __STDC__
static char * 
desired_keymap_prompt (struct input_view * this_iv)
#else
static char * 
desired_keymap_prompt (this_iv)
     struct input_view * this_iv;
#endif
{
  int map = cur_keymap;
  if (the_cmd_frame->cmd && (the_cmd_arg.style == &keyseq_style))
    map = the_cmd_arg.val.key.cmd.code;
  return ((map >= 0) ? map_prompts[map] : 0);
}




/* In WID columns, find the first char in STR that can
 * be displayed while still leaving POS visible.  METRIC
 * converts str->cols.
 */

#ifdef __STDC__
static int
find_vis_begin (int * wid_used, int wid,
		char * str, int pos, text_measure metric)
#else
static int
find_vis_begin (wid_used, wid, str, pos, metric)
     int * wid_used;
     int wid;
     char * str;
     int pos;
     text_measure metric;
#endif
{
  int used;			/* How many cols allocated? */
  if (str[pos])
    used = metric (&str[pos], 1);
  else
    /* POS could be just after the end of STR, in which case pretend
     * that the character there is a SPC. 
     */
    used = metric (" ", 1);

  while (pos && (used < wid))
    {
      int next;
      next = metric (&str[pos - 1], 1);
      if (used + next > wid)
	break;
      else
	{
	  --pos;
	  used += next;
	}
    }
  if (wid_used)
    *wid_used = used;
  return pos;
}



/* Find the last visible character...  -1 if none are vis. */

#ifdef __STDC__
static int
find_vis_end (int * wid_used, int wid, char * str, int start, text_measure
	      metric) 
#else
static int
find_vis_end (wid_used, wid, str, start, metric)
     int * wid_used;
     int wid;
     char * str;
     int start;
     text_measure metric;
#endif
{
  int used = metric (&str[start], 1);	/* How many cols allocated? */
  int pos = start;
  int max = strlen (str) - 1;

  while ((pos < max) && (used < wid))
    {
      int next;
      next = metric (&str[pos + 1], 1);
      if (used + next > wid)
	break;
      else
	{
	  ++pos;
	  used += next;
	}
    }
  if (wid_used)
    *wid_used = used;
  return pos;
}

#ifdef __STDC__
static void
set_vis_wid (struct input_view * this_iv)
#else
static void
set_vis_wid (this_iv)
     struct input_view * this_iv;
#endif
{
  vis_wid = (input_metric (input_area->buf + visibility_begin,
			     visibility_end - visibility_begin)
	       + ((input_cursor > visibility_end)
		  ? input_metric (" ", 1)
		  : 0));
}


/* This recomputes the input area parameters of an input_view, attempting to 
 * center the cursor. 
 */

#ifdef __STDC__
static void
iv_reset_input (struct input_view * this_iv)
#else
void
static iv_reset_input (this_iv)
     struct input_view * this_iv;
#endif
{
  char * km = desired_keymap_prompt (this_iv);
  if (km && (km == keymap_prompt))
    return;
  redraw_needed = FULL_REDRAW;
  keymap_prompt = km;
  if (km)
    {
      /* A keymap prompt should be displayed. */
      expanded_keymap_prompt = expand_prompt (keymap_prompt);
      prompt_wid = prompt_metric (expanded_keymap_prompt,
				  strlen(expanded_keymap_prompt));
      must_fix_input = 1;
      visibility_begin = visibility_end = 0;
      input_area = 0;
      vis_wid = input_metric (" ", 1);
      prompt = 0;
      input_cursor = 0;
    }
  else
    {
      if (expanded_keymap_prompt)
	{
	  ck_free (expanded_keymap_prompt);
	  expanded_keymap_prompt = 0;
	}
      if (!the_cmd_frame->cmd || the_cmd_arg.is_set || !the_cmd_arg.do_prompt)
	{
	  prompt_wid = 0;
	  must_fix_input = 0;
	  input_area = 0;
	  vis_wid = 0;
	  prompt = 0;
	  visibility_begin = visibility_end = 0;
	  input_cursor = 0;
	}
      else
	{
	  prompt_wid = prompt_metric (the_cmd_arg.expanded_prompt,
				       strlen (the_cmd_arg.expanded_prompt));
	  must_fix_input = 0;
	  input_area = &the_cmd_arg.text;
	  prompt = the_cmd_arg.expanded_prompt;

	  if ((scr_cols - prompt_wid) < input_metric ("M", 1))
	    prompt += find_vis_begin (&prompt_wid,
				      scr_cols - input_metric("M", 1),
				      prompt, strlen(prompt) - 1,
				      prompt_metric);
	  
	  {
	    int wid_avail = scr_cols - prompt_wid;
	    visibility_begin =
	      find_vis_begin (0, wid_avail / 2, input_area->buf,
			      the_cmd_arg.cursor, input_metric);
	    visibility_end =
	      find_vis_end (0, wid_avail,
			    input_area->buf, visibility_begin,
			    input_metric); 
	  }

	  input_cursor = the_cmd_arg.cursor;
	  set_vis_wid (this_iv);
	}
    }
}





/* This is called strategicly from the command loop and whenever
 * the input area is changed by beginning a complex command.
 * It may do nothing, though if the input area appeas to have changed
 * it will recompute its appearence.
 *
 * now it also updates the info_fields of the input view.
 */

#ifdef __STDC__
void
iv_fix_input (struct input_view * this_iv)
#else
void
iv_fix_input (this_iv)
     struct input_view * this_iv;
#endif
{
  char * km_prompt = desired_keymap_prompt (this_iv);

  if (keymap_prompt && (keymap_prompt == km_prompt))

    must_fix_input = 1;		/* Do nothing, keymap prompt has precedence */

  else if (must_fix_input
	   || (keymap_prompt != km_prompt)
	   || ((the_cmd_frame->cmd
		&& (the_cmd_arg.do_prompt && !the_cmd_arg.is_set))
	       ? ((input_area != &the_cmd_arg.text)
		  || (prompt != the_cmd_arg.expanded_prompt)
		  || (input_cursor != the_cmd_arg.cursor))
	       : (input_area || prompt_wid)))
    iv_reset_input (this_iv);

  if (the_cmd_frame->cmd
      && ((the_cmd_arg.prompt_info != current_info)
	  || (the_cmd_arg.info_line != info_pos)))
    {
      current_info = the_cmd_arg.prompt_info;
      info_pos = the_cmd_arg.info_line;
      info_redraw_needed = 1;
    }
  else if (current_info
	   && (!the_cmd_frame->cmd || !the_cmd_arg.prompt_info))
    {
      current_info = 0;
      io_repaint ();
    }
	   
}


/* Incremental updates:
 * For simple edits it is not necessary to redraw the entire line.
 * These schedule incremental updating.
 */


#ifdef __STDC__
void
iv_move_cursor (struct input_view * this_iv)
#else
void
iv_move_cursor (this_iv)
     struct input_view * this_iv;
#endif
{
  if (   must_fix_input
      || (visibility_begin > the_cmd_arg.cursor)
      || (   ((visibility_end + 1) < the_cmd_arg.cursor)
	  || (((visibility_end + 1) == the_cmd_arg.cursor)
	      && ((vis_wid + prompt_wid) < scr_cols))))

    must_fix_input = 1;
  else if (redraw_needed != FULL_REDRAW)
    {
      if (   (redraw_needed == NO_REDRAW)
	  || ((redraw_needed > input_cursor)
	      && (redraw_needed > the_cmd_arg.cursor)))
	{
	  input_cursor = the_cmd_arg.cursor;
	  set_vis_wid (this_iv);
	}
      else
	{
	  redraw_needed = Min (input_cursor, the_cmd_arg.cursor);
	  set_vis_wid (this_iv);
	}
    }
}

#ifdef __STDC__
void
iv_erase (struct input_view * this_iv, int len)
#else
void
iv_erase (this_iv, len)
     struct input_view * this_iv;
     int len;
#endif
{
  if (must_fix_input
      || (the_cmd_arg.cursor <= visibility_begin))
    {
      must_fix_input = 1;
      return;
    }
  else if (redraw_needed != FULL_REDRAW)
    {
      if ((redraw_needed == NO_REDRAW) || (redraw_needed > the_cmd_arg.cursor))
	{
	  redraw_needed = the_cmd_arg.cursor;
	  visibility_end = find_vis_end (0, scr_cols - prompt_wid,
					 input_area->buf, visibility_begin,
					 input_metric);
	  input_cursor = the_cmd_arg.cursor;
	}
      else
	redraw_needed = the_cmd_arg.cursor;
      set_vis_wid (this_iv);
    }
}


#ifdef __STDC__
void
iv_insert (struct input_view * this_iv, int len)
#else
void
iv_insert (this_iv, len)
     struct input_view * this_iv;
     int len;
#endif
{
  if (!must_fix_input)
    {
      int new_end;
      int cursor_past_end = (input_cursor + len == strlen (input_area->buf));
      
      new_end = find_vis_end (0, scr_cols - prompt_wid,
			      input_area->buf, visibility_begin,
			      input_metric);
      
      if ((new_end + cursor_past_end) < (input_cursor + len))
	must_fix_input = 1;
      else
	{
	  if (   (redraw_needed != FULL_REDRAW)
	      && ((redraw_needed == NO_REDRAW) || (redraw_needed > input_cursor)))
	    redraw_needed = input_cursor;
	  input_cursor += len;
	  visibility_end = new_end;
	  set_vis_wid (this_iv);
	}
    }
}
	   
#ifdef __STDC__
void
iv_over (struct input_view * this_iv, int len)
#else
void
iv_over (this_iv, len)
     struct input_view * this_iv;
     int len;
#endif
{
  iv_insert (this_iv, len);
}


