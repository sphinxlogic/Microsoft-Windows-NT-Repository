#ifndef ARGSH
#define ARGSH

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
/*  t. lord	Wed Jan 27 15:46:24 1993	*/




/* Interacting with command arguments. */

/* See find_stub. */
enum command_arg_representation
{
  cmd_char  = 'c',
  cmd_float = 'd',
  cmd_file  = 'f',
  cmd_int   = 'i',
  cmd_key   = 'k',
  cmd_range = 'r',
  cmd_string = 's',
  cmd_none  = '0'
};

struct command_arg;

/* See comments in arg.c: */

#ifdef __STDC__
typedef char * (*arg_verifier) (char ** end_out, struct command_arg *);
typedef void (*arg_destroyer) (struct command_arg *);
#else
typedef char * (*arg_verifier) ();
typedef void (*arg_destroyer) ();
#endif

/* For every kind of prompt allowed in a FUNC_ARGS string, 
 * there exists a prompt_style that describes how that
 * kind of argument should be editted/passed:
 */

struct prompt_style
{
  arg_verifier verify;
  arg_destroyer destroy;
  enum command_arg_representation representation;
  char * keymap;
};



#ifdef __STDC__
extern char * char_verify (char ** end, struct command_arg * arg);
extern char * symbol_verify (char ** end, struct command_arg * arg);
extern char * word_verify (char ** end, struct command_arg * arg);
extern void symbol_destroy (struct command_arg * arg);
extern char * command_verify (char ** end, struct command_arg * arg);
extern char * read_file_verify (char ** end, struct command_arg * arg);
extern void read_file_destroy (struct command_arg * arg);
extern char * write_file_verify (char ** end, struct command_arg * arg);
extern void write_file_destroy (struct command_arg * arg);
extern char * keyseq_verify (char ** end, struct command_arg * arg);
extern char * keymap_verify (char ** end, struct command_arg * arg);
extern char * number_verify (char ** end, struct command_arg * arg);
extern char * double_verify (char ** end, struct command_arg * arg);
extern char * range_verify (char ** end, struct command_arg * arg);
extern char * string_verify (char ** end, struct command_arg * arg);
extern char * yes_verify (char ** end, struct command_arg * arg);
extern char * incremental_cmd_verify (char ** end, struct command_arg * arg);
extern char * menu_verify (char ** end, struct command_arg * arg);
extern char * format_verify (char ** end, struct command_arg * arg);
extern char * noop_verify (char ** end, struct command_arg * arg);

#else
extern char * char_verify ();
extern char * symbol_verify ();
extern char * word_verify ();
extern void symbol_destroy ();
extern char * command_verify ();
extern char * read_file_verify ();
extern void read_file_destroy ();
extern char * write_file_verify ();
extern void write_file_destroy ();
extern char * keyseq_verify ();
extern char * keymap_verify ();
extern char * number_verify ();
extern char * double_verify ();
extern char * range_verify ();
extern char * string_verify ();
extern char * yes_verify ();
extern char * incremental_cmd_verify ();
extern char * menu_verify ();
extern char * format_verify ();
extern char * noop_verify ();

#endif

#ifdef DEFINE_STYLES
#define DEFSTYLE(NAME,VER,DEST,REP,KEYMAP) \
struct prompt_style NAME = \
{ \
  VER, \
  DEST, \
  REP, \
  KEYMAP \
}
#else
#define DEFSTYLE(NAME,VER,DEST,REP,KEYMAP) extern struct prompt_style NAME
#endif

DEFSTYLE(char_style, char_verify, 0, cmd_int, "read-char");
DEFSTYLE(double_style, double_verify, 0, cmd_float, "read-float");
DEFSTYLE(menu_style, menu_verify, 0, cmd_int, "read-menu");
DEFSTYLE(format_style, format_verify, 0, cmd_int, "read-format");
DEFSTYLE(symbol_style,
	 symbol_verify, symbol_destroy, cmd_string, "read-symbol");
DEFSTYLE(word_style,
	 word_verify, symbol_destroy, cmd_string, "read-word");
DEFSTYLE(command_style,
	 command_verify, symbol_destroy, cmd_string, "read-symbol");
DEFSTYLE(read_file_style,
	 read_file_verify, read_file_destroy, cmd_file, "read-filename");
DEFSTYLE(write_file_style,
	 write_file_verify, write_file_destroy, cmd_file, "read-filename");
DEFSTYLE(keyseq_style, keyseq_verify, 0, cmd_key, "read-keyseq");
DEFSTYLE(keymap_style,
	 keymap_verify, symbol_destroy, cmd_string, "read-symbol"); 
DEFSTYLE(number_style, number_verify, 0, cmd_int, "read-integer");
DEFSTYLE(range_style, range_verify, 0, cmd_range, "read-range");
DEFSTYLE(formula_style, string_verify, 0, cmd_string, "read-formula");
DEFSTYLE(string_style, string_verify, 0, cmd_string, "read-string");
DEFSTYLE(file_name_style, string_verify, 0, cmd_string, "read-filename");
DEFSTYLE(yes_style, yes_verify, 0, cmd_none, "read-string");
DEFSTYLE(int_constant_style, 0, 0, cmd_int, 0);
DEFSTYLE(range_constant_style, 0, 0, cmd_range, 0);
DEFSTYLE(inc_cmd_style, incremental_cmd_verify, 0, cmd_none, "direction");
DEFSTYLE(mode_style, noop_verify, 0, cmd_none, 0);

#endif  /* ARGSH */


