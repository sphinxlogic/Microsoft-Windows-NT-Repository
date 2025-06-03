
#ifndef IO_EDITH
#define IO_EDITH

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

#include "cmd.h"

#ifdef __STDC__
extern int check_editting_mode (void);
extern void begin_edit (void);
extern void setn_edit_line (char * str, int len);
extern void toggle_overwrite (int set, int setting);
extern void beginning_of_line (void);
extern void end_of_line (void);
extern void backward_char (int n);
extern void backward_word (int n);
extern void forward_char (int n);
extern void goto_char (int n);
extern void forward_word (int n);
extern void backward_delete_char (int n);
extern void backward_delete_word (int n);
extern void delete_to_start(void);
extern void delete_char (int n);
extern void delete_word (int n);
extern void kill_line(void);
extern void insert_string (char * str, int len);
extern void over_string (char * str, int len);
extern void put_string (char * str, int len);
extern void insert_cell_expression (void);
extern void insert_cell_value(void);
extern void insert_rel_ref(void);
extern void insert_abs_ref(void);
extern void insert_cell_attr (struct rng * rng, char * attr);
extern void insert_usr_fmt_part (int fmt, int stat);
extern void self_insert_command (int ch, int count);
extern void self_map_command (int c);
extern void insert_current_filename (void);
extern void exit_self_inserting (int c);
extern void insert_context_word (void);

#else
extern int check_editting_mode ();
extern void begin_edit ();
extern void setn_edit_line ();
extern void toggle_overwrite ();
extern void beginning_of_line ();
extern void end_of_line ();
extern void backward_char ();
extern void backward_word ();
extern void forward_char ();
extern void goto_char ();
extern void forward_word ();
extern void backward_delete_char ();
extern void backward_delete_word ();
extern void delete_to_start();
extern void delete_char ();
extern void delete_word ();
extern void kill_line();
extern void insert_string ();
extern void over_string ();
extern void put_string ();
extern void insert_cell_expression ();
extern void insert_cell_value();
extern void insert_rel_ref();
extern void insert_abs_ref();
extern void insert_cell_attr ();
extern void insert_usr_fmt_part ();
extern void self_insert_command ();
extern void self_map_command ();
extern void insert_current_filename ();
extern void exit_self_inserting ();
extern void insert_context_word ();

#endif

#endif
