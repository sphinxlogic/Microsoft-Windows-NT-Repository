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
#include "cmd.h"
#include "key.h"
#include "info.h"
#include "forminfo.h"
#include "help.h"
#include "key.h"
#include "io-utils.h"

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* __GNUC__ not defined.  */
#if HAVE_ALLOCA_H
#include <alloca.h>
#else /* not HAVE_ALLOCA_H */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
#pragma alloca
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not HAVE_ALLOCA_H */
#endif /* __GNUC__ not defined.  */
#endif /* alloca not defined.  */



/* This reads one info buffer, writing another.  On each line,  
 * occurences of [COMMAND] are replaced with keysequence names,
 * if COMMAND is bound.
 */

#ifdef __STDC__
void
expand_help_msg (struct info_buffer * out, struct info_buffer * in)
#else
void
expand_help_msg (out, in)
     struct info_buffer * out;
     struct info_buffer * in;
#endif
{
  int x;
  struct line line;
  struct line seq_buf;
  int out_offset;
  int rel_map = cur_keymap;

  if (the_cmd_frame->cmd && (the_cmd_arg.style == &keyseq_style))
    rel_map = the_cmd_arg.val.key.cmd.code;

  print_info (out, "");
  out_offset = out->len;
  out->len += in->len;
  out->text = (char **)ck_remalloc (out->text, out->len * sizeof (char *));
  init_line (&line);
  init_line (&seq_buf);
  for (x = 0; x < in->len; ++x)
    {
      char * next_burst = in->text[x];
      char * end_burst = index (next_burst, '[');
      set_line (&line, "");
      set_line (&seq_buf, "");
      while (end_burst)
	{
	  char * fn_start = end_burst + 1;
	  char * fn_end = index (fn_start, ']');
	  int vec;
	  struct cmd_func * cmd;
	  if (fn_end && (*fn_start == '[') && fn_end[1] == ']')
	    {
	      int map = map_idn (fn_start + 1, fn_end - fn_start - 1);
	      if (map < 0)
		map = map_id ("universal");
	      rel_map = map;
	      catn_line (&line, next_burst, end_burst - next_burst);
	      next_burst = fn_end + 2;
	      end_burst = index (next_burst, '[');
	    }
	  else if (   fn_end
		   && !find_function (&vec, &cmd, fn_start, fn_end - fn_start))
	    {
	      if (search_map_for_cmd (&seq_buf, rel_map, vec,
				      cmd - the_funcs[vec])) 
		{
		  catn_line (&line, next_burst, end_burst - next_burst);
		  catn_line (&line, seq_buf.buf, strlen (seq_buf.buf));
		  set_line (&seq_buf, "");
		  next_burst = fn_end + 1;
		  end_burst = index (next_burst, '[');
		}
	      else
		{
		  catn_line (&line, "M-x ", 4);
		  catn_line (&line, fn_start, fn_end - fn_start);
		  next_burst = fn_end + 1;
		  end_burst = index (next_burst, '[');
		}
	    }
	  else if (fn_end)
	    end_burst = index (fn_end + 1, '[');
	}
      catn_line (&line, next_burst, strlen(next_burst));
      out->text[x + out_offset] = line.buf;
      init_line (&line);
    }
  free_line (&seq_buf);
  free_line (&line);
}




#ifdef __STDC__
void
describe_function (char * name)
#else
void
describe_function (name)
     char * name;
#endif
{
  int vector;
  struct cmd_func * cmd;
  if (!(   !find_function (&vector, &cmd, name, strlen(name))
	&& cmd->func_doc))
    io_error_msg ("%s is not a function.", name); /* no return */
  
  {
    struct info_buffer * ib_disp = find_or_make_info ("help-buffer");
    clear_info (ib_disp);
    print_info (ib_disp, "");
    print_info (ib_disp, "%s", name);
    {
      struct info_buffer ib;
      for (ib.len = 0; cmd->func_doc[ib.len]; ++ib.len) ;
      ib.text = cmd->func_doc;
      ib.name = name;
      expand_help_msg (ib_disp, &ib);
    }
  }

  {
    static char buf[] = "{view-info help-buffer}";
    execute_as_macro (buf);
  }
}



#ifdef __STDC__
void
brief_describe_key (struct key_sequence * keyseq)
#else
void
brief_describe_key (keyseq)
     struct key_sequence * keyseq;
#endif
{
  if (keyseq->cmd.vector < 0 && keyseq->cmd.code < 0)
    io_info_msg ("%s is unbound", keyseq->keys->buf);
  else
    io_info_msg ("%s --> %s", keyseq->keys->buf,
		 the_funcs[keyseq->cmd.vector][keyseq->cmd.code].func_name);
}

#ifdef __STDC__
void
describe_key (struct key_sequence * keyseq)
#else
void
describe_key (keyseq)
     struct key_sequence * keyseq;
#endif
{
  if (keyseq->cmd.vector < 0 && keyseq->cmd.code < 0)
    io_info_msg ("%s is unbound", keyseq->keys->buf);
  else if (keyseq->cmd.vector < 0)
    io_info_msg ("%s is a prefix leading to the keymap `%s'.",
		 keyseq->keys->buf, map_names[keyseq->cmd.code]);
  else
    describe_function
      (the_funcs[keyseq->cmd.vector][keyseq->cmd.code].func_name); 
}


#ifdef __STDC__
void
where_is (char * name)
#else
void
where_is (name)
char * name;
#endif
{
  struct line seqname;
  int vec;
  int code;
  struct cmd_func * cmd;
  
  if (!find_function (&vec, &cmd, name, strlen(name)))
    code = cmd - the_funcs[vec];
  else if (map_id (name) >= 0)
    {
      code = map_id (name);
      vec = -1;
    }
  else
    io_error_msg ("%s is not a function.", name); /* no return */
  code = cmd - the_funcs[vec];
  init_line (&seqname);
  set_line (&seqname, "");
  
  if (!search_map_for_cmd (&seqname, the_cmd_frame->top_keymap, vec, code))
    io_info_msg ("%s is not on any keys.", name);
  else
    io_info_msg ("%s is bound to %s.", name, seqname.buf);
  
  free_line (&seqname);
}


/* Help for the context of the current complex command. */

#ifdef __STDC__
void
help_with_command (void)
#else
void
help_with_command ()
#endif
{
  if (!the_cmd_frame->cmd)
    io_info_msg ("help-with-command: No command is executing now.");
  describe_function (the_cmd_frame->cmd->func_name);
}





/* Access to the help messages defined in forminfo.c
 * These include documentation for formula functions and
 * other miscelaneous doc strings.
 *
 * The info message NAME is permanently stored in an info-buffer
 * called _info_NAME
 */

#ifdef __STDC__
void
builtin_help (char * name)
#else
void
builtin_help (name)
     char * name;
#endif
{
  char info_name[100];
  struct info_buffer * ib;

  if (strlen (name) < sizeof (info_name - 20))
    io_error_msg ("No built in help for %s.", name);

  sprintf (info_name, "_info_%s", name);
  ib = find_info (info_name);

  if (!ib)
    {
      char ** msg = forminfo_text (name);
      if (!msg)
	{
	  msg = (char **)ck_malloc (sizeof (char *) * 2);
	  msg[0] = mk_sprintf ("No built in help for %s.", name);
	  msg[1] = 0;
	}
      ib = find_or_make_info (info_name);
      ib->len = parray_len (msg);
      ib->text = msg;
    }

  {
    char exp_name[200];
    char command[250];
    struct info_buffer * expanded;
    sprintf (exp_name, "_expanded_%s", info_name);
    expanded = find_or_make_info (exp_name);
    clear_info (expanded);
    expand_help_msg (expanded, ib);
    sprintf (command, "{set-info %s}", exp_name);
    execute_as_macro (command);
  }
}

#ifdef __STDC__
void
make_wallchart_info (void)
#else
void
make_wallchart_info ()
#endif
{
  struct info_buffer * wc = find_or_make_info ("unexpanded_wallchart");
  struct info_buffer * wc_expanded = find_or_make_info ("wallchart");
  if (wc->len == 0)
    {
      char ** txt = forminfo_text ("keybindings-wallchart");
      if (!txt)
	io_error_msg ("<bug> Wallchart is missing!");
      wc->len = parray_len (txt);
      wc->text = txt;
    }
  clear_info (wc_expanded);
  expand_help_msg (wc_expanded, wc);
}


#ifdef __STDC__
void
write_info (char * info, FILE * fp)
#else
void
write_info (info, fp)
     char * info;
     FILE * fp;
#endif
{
  struct info_buffer * ib = find_or_make_info (info);
  int x;
  for (x = 0; x < ib->len; ++x)
    fprintf (fp, "%s\n", ib->text[x]);
}

