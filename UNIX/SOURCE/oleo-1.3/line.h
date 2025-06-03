#ifndef LINEH
#define LINEH
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
/*  t. lord	Fri Aug  7 22:23:59 1992	*/

#define LINE_MIN 28

struct line
{
  int alloc;
  char *buf;
};

#define init_line(L)  bzero((L), sizeof (struct line))


#ifdef __STDC__
extern void set_line (struct line *line, char *string);
extern void setn_line (struct line *line, char *string, int n);
extern void catn_line (struct line *line, char *string, int n);
extern void sprint_line (struct line *line, char * fmt, ...);
extern void splicen_line (struct line * line, char * str, int n, int pos);
extern void edit_line (struct line * line, int begin, int len);
extern void free_line (struct line * line);
extern int read_line (struct line * line, FILE * fp, int * linec);

#else
extern void set_line ();
extern void setn_line ();
extern void catn_line ();
extern void sprint_line ();
extern void splicen_line ();
extern void edit_line ();
extern void free_line ();
extern int read_line ();

#endif

#endif
