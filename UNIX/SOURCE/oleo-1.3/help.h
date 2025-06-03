#ifndef HELPH
#define HELPH

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
/*  t. lord	Sun Feb 21 16:34:20 1993	*/


#include "info.h"


#ifdef __STDC__
extern void expand_help_msg (struct info_buffer * out, struct info_buffer * in);
extern void describe_function (char * name);
extern void brief_describe_key (struct key_sequence * keyseq);
extern void describe_key (struct key_sequence * keyseq);
extern void where_is (char * name);
extern void help_with_command (void);
extern void builtin_help (char * name);
extern void make_wallchart_info (void);
extern void write_info (char * info, FILE * fp);

#else
extern void expand_help_msg ();
extern void describe_function ();
extern void brief_describe_key ();
extern void describe_key ();
extern void where_is ();
extern void help_with_command ();
extern void builtin_help ();
extern void make_wallchart_info ();
extern void write_info ();

#endif

#endif  /* HELPH */


