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
#include "global.h"
#define DEFINE_STYLES	1
#include "args.h"
#include "cmd.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "io-edit.h"
#include "io-utils.h"
#include "format.h"



/* These commands define the syntax and editting modes of command arguments.
 * Each _verify function parses some kind of argument and stores its value
 * in a command_arg structure.  An error message or NULL is returned.
 * If the match succeeds, a string pointer is advanced to the end of what was
 * parsed.
 *
 * Several arguments may be listed on one line separated only by
 * whitespace.  A _verify function should stop after its argument and ignore
 * everything after that. 
 */

#ifdef __STDC__
char *
char_verify (char ** end, struct command_arg * arg)
#else
char *
char_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  if (!**end)
    return "No character specified";
  else
    {
      int ch = string_to_char (end);
      if (ch < 0)
	{
	  setn_edit_line ("", 0);
	  return "Illegal character constant.";
	}
      else
	{
	  arg->val.integer = ch;
	  return 0;
	}
    }
}

#ifdef __STDC__
char *
symbol_verify (char ** end, struct command_arg * arg)
#else
char *
symbol_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  char * e = *end;
  char * start = *end;
  if (isalpha (*e) || (*e == '-') || (*e == '_') || (*e == (a0 ? '$' : ':')))
    {
      while (isalpha(*e) || isdigit(*e) || (*e == '-') || (*e == '_')
	     || (*e == (a0 ? '$' : ':')))
	++e;
      if (!isspace(*e) && *e)
	goto bad_symbol;
      *end = e;
      arg->val.string = (char *)ck_malloc (e - start + 1);
      bcopy (start, arg->val.string, e - start);
      arg->val.string[e - start] = '\0';
      return 0;
    }
 bad_symbol:
  if (arg->arg_desc[1] == '\'')
    {
      arg->val.string = 0;
      return 0;
    }
  else
    return "Invalid symbol name.";
}

char *
word_verify (end, arg)
     char ** end;
     struct command_arg * arg;
{
  char * e = *end;
  char * start = *end;
  if (!isspace (*e))
    {
      while (!isspace(*e))
	++e;
      *end = e;
      arg->val.string = (char *)ck_malloc (e - start + 1);
      bcopy (start, arg->val.string, e - start);
      arg->val.string[e - start] = '\0';
      return 0;
    }
  else if (arg->arg_desc[1] == '\'')
    {
      arg->val.string = 0;
      return 0;
    }
  else
    return "Invalid symbol name.";
}

#ifdef __STDC__
void
symbol_destroy (struct command_arg * arg)
#else
void
symbol_destroy (arg)
     struct command_arg * arg;
#endif
{
  if (arg->val.string)
    ck_free (arg->val.string);
}

#ifdef __STDC__
char *
command_verify (char ** end, struct command_arg * arg)
#else
char *
command_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  char * error = symbol_verify (end, arg);
  char * str;
  if (error)
    return error;
  str = arg->val.string;
  if (!(find_function (0, 0, arg->val.string, strlen(arg->val.string))
        && get_abs_rng (&str, 0)))
    return 0;
  else
    return "Not a command or macro address.";
}

#ifdef __STDC__
char * 
read_file_verify (char ** end, struct command_arg * arg)
#else
char * 
read_file_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  FILE * fp = xopen_with_backup (arg->text.buf, "r");
  *end = 0;
  if (!fp)
    {
      io_error_msg ("Can't open file '%s':%s", arg->text.buf, err_msg ());
      return "";
    }
  else
    {
      arg->val.fp = fp;
      return 0;
    }
}

#ifdef __STDC__
void
read_file_destroy (struct command_arg * arg)
#else
void
read_file_destroy (arg)
     struct command_arg * arg;
#endif
{
  int num;
  num = xclose (arg->val.fp);
  if (num)
    io_error_msg ("Can't close '%s': Error code %d: %s",
		  arg->text.buf, num, err_msg ());
}


#ifdef __STDC__
char * 
write_file_verify (char ** end, struct command_arg * arg)
#else
char * 
write_file_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  FILE * fp = xopen_with_backup (arg->text.buf, "w");
  *end = 0;
  if (!fp)
    {
      io_error_msg ("Can't open file '%s':%s", arg->text.buf, err_msg ());
      return "";
    }
  else
    {
      arg->val.fp = fp;
      return 0;
    }
}

#ifdef __STDC__
void
write_file_destroy (struct command_arg * arg)
#else
void
write_file_destroy (arg)
     struct command_arg * arg;
#endif
{
  int num;

  num = xclose (arg->val.fp);
  if (num)
    io_error_msg ("Can't close '%s': Error code %d: %s",
		  arg->text.buf, num, err_msg ());
}

/* As a special case, cmd_loop makes sure that keyseq arguments are only read
 * interactively.
 */

#ifdef __STDC__
char *
keyseq_verify (char ** end, struct command_arg * arg)
#else
char *
keyseq_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  *end = 0;
  return 0;
}


#ifdef __STDC__
char *
keymap_verify (char ** end, struct command_arg * arg)
#else
char *
keymap_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  char * start = *end;
  char * error = symbol_verify (end, arg);
  int id;
  if (error)
    return error;
  id = map_idn (start, *end - start);
  return (id >= 0
	  ? (char *) 0
	  : "No such keymap.");
}


#ifdef __STDC__
char *
number_verify (char ** end, struct command_arg * arg)
#else
char *
number_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  char * e = *end;

  while (isspace (*e))
    ++e;
  if (isdigit(*e) || (*e == '-'))
    {
      arg->val.integer = astol (end);
      if (arg->arg_desc[1] == '[')
	{
	  char * prompt = arg->arg_desc + 2;
	  {
	    long low = 0;
	    long high = -1;
	    low = astol (&prompt);
	    while (isspace (*prompt))  ++prompt;
	    if (*prompt == ',') ++prompt;
	    high = astol (&prompt);
	    while (isspace (*prompt))  ++prompt;
	    if (*prompt == ']') ++prompt;
	    if (   (low > arg->val.integer)
		|| (high < arg->val.integer))
	      io_error_msg ("Out of range %d (should be in [%d - %d]).",
			    arg->val.integer, low, high); /* no return */
	  }
	}
      return 0;
    }
  else
    return "Not a number.";
}


#ifdef __STDC__
char *
double_verify (char ** end, struct command_arg * arg)
#else
char *
double_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  char * e = *end;

  while (isspace (*e))
    ++e;
  if (isdigit(*e) || ((*e == '-') && isdigit (*(e + 1))))
    {
      arg->val.floating = astof (end);
      return 0;
    }
  else
    return "Not a number.";
}


#ifdef __STDC__
char * 
range_verify (char ** end, struct command_arg * arg)
#else
char * 
range_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  union command_arg_val * val = &arg->val;
  *end = arg->text.buf;
  if (get_abs_rng (end, &val->range))
    return "Not a range.";
  else
    return 0;
}

#ifdef __STDC__
char * 
string_verify (char ** end, struct command_arg * arg)
#else
char * 
string_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  arg->val.string = arg->text.buf;
  *end = 0;
  return 0;
}

/* Unlike most verify functions, this
 * one may destroy the command frame that it is 
 * operating on.  It's purpose is to allow user's
 * to abort commands. 
 */
#ifdef __STDC__
char * 
yes_verify (char ** end, struct command_arg * arg)
#else
char * 
yes_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  if (words_imatch (end, "no"))
    {
      pop_unfinished_command ();
      return "Aborted.";
    }
  else if (words_imatch (end, "yes"))
    return 0;
  else
    {
      setn_edit_line ("", 0);
      return "Please answer yes or no.";
    }
}

#ifdef __STDC__
char *
incremental_cmd_verify (char ** end, struct command_arg * arg)
#else
char *
incremental_cmd_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  return 0;
}


#ifdef __STDC__
char *
menu_verify (char ** end, struct command_arg * arg)
#else
char *
menu_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  char * error = char_verify (end, arg);
  if (error)
    return error;

  {
    int pick = arg->val.integer;
    char * key = arg->arg_desc + 1;
    while (*key && (*key != ']'))
      {
	if (*key == '\\')
	  {
	    ++key;
	    if (!*key)
	      break;
	  }
	if (pick == *key)
	  return 0;
	else
	  ++key;
      }
    setn_edit_line ("", 0);
    return "No such menu option.";
  }
}


#ifdef __STDC__
char *
format_verify (char ** end, struct command_arg * arg)
#else
char *
format_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  arg->val.integer = str_to_fmt (*end);
  if (arg->val.integer < 0)
    return "Unknown format.";
  *end = 0;
  return 0;
}


#ifdef __STDC__
char *
noop_verify (char ** end, struct command_arg * arg)
#else
char *
noop_verify (end, arg)
     char ** end;
     struct command_arg * arg;
#endif
{
  return 0;
}
