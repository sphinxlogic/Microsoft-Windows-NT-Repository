#ifndef IO_GENERICH
#define IO_GENERICH
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
along with Oleo; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */
/*  t. lord	Fri Aug  7 23:20:52 1992	*/

#include "global.h"
#include "obstack.h"


/*
 * User settable options.
 */
extern int bkgrnd_recalc;
extern int auto_recalc;
extern int a0;
/* This is how frequently the alarm should go off. */
extern unsigned int alarm_seconds;
extern unsigned int alarm_active;	/* Whether or not the alarm matters. */

/*
 * 1 -- clear the input area before reading the next char.
 * 2 -- clear the input area after reading the next char.
 * 0 -- don't clear the input area.
 */
extern int topclear;

extern char * current_filename;

/* Has io_open_display been called? */
extern int display_opened;

#ifdef __STDC__
extern void (*read_file) (FILE *, int);
extern void (*write_file) (FILE *, struct rng *);
extern int (*set_file_opts) (int, char *);
extern void (*show_file_opts) (void);

extern int get_chr (void);		/* read from kbd or macro */
extern int set_window_option (int set_opt, char *text);
extern void show_window_options (void);
#else
extern void (*read_file) ();
extern void (*write_file) ();
extern int (*set_file_opts) ();
extern void (*show_file_opts) ();

extern int get_chr ();		/* read from kbd or macro */
extern int set_window_option ();
extern void show_window_options ();
#endif


extern const int rowmagic[], colmagic[];

#endif
