/* unexpand - convert spaces to tabs
   Copyright (C) 1989, 1991 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* By default, convert only maximal strings of initial blanks and tabs
   into tabs.
   Preserves backspace characters in the output; they decrement the
   column count for tab calculations.
   The default action is equivalent to -8.

   Options:
   --tabs=tab1[,tab2[,...]]
   -t tab1[,tab2[,...]]
   -tab1[,tab2[,...]]	If only one tab stop is given, set the tabs tab1
			spaces apart instead of the default 8.  Otherwise,
			set the tabs at columns tab1, tab2, etc. (numbered from
			0); replace any tabs beyond the tabstops given with
			single spaces.
   --all
   -a			Use tabs wherever they would replace 2 or more spaces,
			not just at the beginnings of lines.

   David MacKenzie <djm@gnu.ai.mit.edu> */

#ifdef HAVE_CONFIG_H
#if defined (CONFIG_BROKETS)
/* We use <config.h> instead of "config.h" so that a compilation
   using -I. -I$srcdir will use ./config.h rather than $srcdir/config.h
   (which it would do because it found this file in $srcdir).  */
#include <config.h>
#else
#include "config.h"
#endif
#endif

/* Get isblank from GNU libc.  */
#define _GNU_SOURCE

#include <stdio.h>
#include <getopt.h>
#include <sys/types.h>
#include "system.h"
#include "version.h"

/* The number of bytes added at a time to the amount of memory
   allocated for the output line. */
#define OUTPUT_BLOCK 256

/* The number of bytes added at a time to the amount of memory
   allocated for the list of tabstops. */
#define TABLIST_BLOCK 256

char *xmalloc ();
char *xrealloc ();
void error ();

static FILE *next_file ();
static void add_tabstop ();
static void parse_tabstops ();
static void unexpand ();
static void usage ();
static void validate_tabstops ();

/* The name this program was run with. */
char *program_name;

/* If nonzero, convert blanks even after nonblank characters have been
   read on the line. */
static int convert_entire_line;

/* If nonzero, the size of all tab stops.  If zero, use `tab_list' instead. */
static int tab_size;

/* Array of the explicit column numbers of the tab stops;
   after `tab_list' is exhausted, the rest of the line is printed
   unchanged.  The first column is column 0. */
static int *tab_list;

/* The index of the first invalid element of `tab_list',
   where the next element can be added. */
static int first_free_tab;

/* Null-terminated array of input filenames. */
static char **file_list;

/* Default for `file_list' if no files are given on the command line. */
static char *stdin_argv[] =
{
  "-", NULL
};

/* Nonzero if we have ever read standard input. */
static int have_read_stdin;

/* Status to return to the system. */
static int exit_status;

/* If non-zero, display usage information and exit.  */
static int show_help;

/* If non-zero, print the version on standard output then exit.  */
static int show_version;

static struct option const longopts[] =
{
  {"tabs", required_argument, NULL, 't'},
  {"all", no_argument, NULL, 'a'},
  {"help", no_argument, &show_help, 1},
  {"version", no_argument, &show_version, 1},
  {NULL, 0, NULL, 0}
};

void
main (argc, argv)
     int argc;
     char **argv;
{
  int tabval = -1;		/* Value of tabstop being read, or -1. */
  int c;			/* Option character. */

  have_read_stdin = 0;
  exit_status = 0;
  convert_entire_line = 0;
  tab_list = NULL;
  first_free_tab = 0;
  program_name = argv[0];

  while ((c = getopt_long (argc, argv, "at:,0123456789", longopts, (int *) 0))
	 != EOF)
    {
      switch (c)
	{
	case 0:
	  break;

	case '?':
	  usage (1);
	case 'a':
	  convert_entire_line = 1;
	  break;
	case 't':
	  convert_entire_line = 1;
	  parse_tabstops (optarg);
	  break;
	case ',':
	  add_tabstop (tabval);
	  tabval = -1;
	  break;
	default:
	  if (tabval == -1)
	    tabval = 0;
	  tabval = tabval * 10 + c - '0';
	  break;
	}
    }

  if (show_version)
    {
      printf ("%s\n", version_string);
      exit (0);
    }

  if (show_help)
    usage (0);

  add_tabstop (tabval);

  validate_tabstops (tab_list, first_free_tab);

  if (first_free_tab == 0)
    tab_size = 8;
  else if (first_free_tab == 1)
    tab_size = tab_list[0];
  else
    tab_size = 0;

  if (optind == argc)
    file_list = stdin_argv;
  else
    file_list = &argv[optind];

  unexpand ();

  if (have_read_stdin && fclose (stdin) == EOF)
    error (1, errno, "-");
  if (fclose (stdout) == EOF)
    error (1, errno, "write error");
  exit (exit_status);
}

/* Add the comma or blank separated list of tabstops STOPS
   to the list of tabstops. */

static void
parse_tabstops (stops)
     char *stops;
{
  int tabval = -1;

  for (; *stops; stops++)
    {
      if (*stops == ',' || ISBLANK (*stops))
	{
	  add_tabstop (tabval);
	  tabval = -1;
	}
      else if (ISDIGIT (*stops))
	{
	  if (tabval == -1)
	    tabval = 0;
	  tabval = tabval * 10 + *stops - '0';
	}
      else
	error (1, 0, "tab size contains an invalid character");
    }

  add_tabstop (tabval);
}

/* Add tab stop TABVAL to the end of `tab_list', except
   if TABVAL is -1, do nothing. */

static void
add_tabstop (tabval)
     int tabval;
{
  if (tabval == -1)
    return;
  if (first_free_tab % TABLIST_BLOCK == 0)
    tab_list = (int *) xrealloc (tab_list, first_free_tab + TABLIST_BLOCK);
  tab_list[first_free_tab++] = tabval;
}

/* Check that the list of tabstops TABS, with ENTRIES entries,
   contains only nonzero, ascending values. */

static void
validate_tabstops (tabs, entries)
     int *tabs;
     int entries;
{
  int prev_tab = 0;
  int i;

  for (i = 0; i < entries; i++)
    {
      if (tabs[i] == 0)
	error (1, 0, "tab size cannot be 0");
      if (tabs[i] <= prev_tab)
	error (1, 0, "tab sizes must be ascending");
      prev_tab = tabs[i];
    }
}

/* Change spaces to tabs, writing to stdout.
   Read each file in `file_list', in order. */

static void
unexpand ()
{
  FILE *fp;			/* Input stream. */
  int c;			/* Each input character. */
  /* Index in `tab_list' of next tabstop: */
  int tab_index = 0;		/* For calculating width of pending tabs. */
  int print_tab_index = 0;	/* For printing as many tabs as possible. */
  int column = 0;		/* Column on screen of next char. */
  int next_tab_column; 		/* Column the next tab stop is on. */
  int convert = 1;		/* If nonzero, perform translations. */
  int pending = 0;		/* Pending columns of blanks. */

  fp = next_file ((FILE *) NULL);
  for (;;)
    {
      c = getc (fp);
      if (c == EOF)
	{
	  fp = next_file (fp);
	  if (fp == NULL)
	    break;		/* No more files. */
	  else
	    continue;
	}

      if (c == ' ' && convert)
	{
	  ++pending;
	  ++column;
	}
      else if (c == '\t' && convert)
	{
	  if (tab_size == 0)
	    {
	      /* Do not let tab_index == first_free_tab;
		 stop when it is 1 less. */
	      while (tab_index < first_free_tab - 1
		     && column >= tab_list[tab_index])
		tab_index++;
	      next_tab_column = tab_list[tab_index];
	      if (tab_index < first_free_tab - 1)
		tab_index++;
	      if (column >= next_tab_column)
		{
		  convert = 0;	/* Ran out of tab stops. */
		  goto flush_pend;
		}
	    }
	  else
	    {
	      next_tab_column = column + tab_size - column % tab_size;
	    }
	  pending += next_tab_column - column;
	  column = next_tab_column;
	}
      else
	{
	flush_pend:
	  /* Flush pending spaces.  Print as many tabs as possible,
	     then print the rest as spaces. */
	  if (pending == 1)
	    {
	      putchar (' ');
	      pending = 0;
	    }
	  column -= pending;
	  while (pending != 0)
	    {
	      if (tab_size == 0)
		{
		  /* Do not let tab_index == first_free_tab;
		     stop when it is 1 less. */
		  while (tab_index < first_free_tab - 1
			 && column >= tab_list[tab_index])
		    print_tab_index++;
		  next_tab_column = tab_list[print_tab_index];
		  if (print_tab_index < first_free_tab - 1)
		    print_tab_index++;
		}
	      else
		{
		  next_tab_column = column + tab_size - column % tab_size;
		}
	      if (next_tab_column - column <= pending)
		{
		  putchar ('\t');
		  pending -= next_tab_column - column;
		  column = next_tab_column;
		}
	      else
		{
		  --print_tab_index;
		  column += pending;
		  while (pending != 0)
		    {
		      putchar (' ');
		      pending--;
		    }
		}
	    }

	  if (convert)
	    {
	      if (c == '\b')
		{
		  if (column > 0)
		    --column;
		}
	      else
		{
		  ++column;
		  if (convert_entire_line == 0)
		    convert = 0;
		}
	    }

	  putchar (c);

	  if (c == '\n')
	    {
	      tab_index = print_tab_index = 0;
	      column = pending = 0;
	      convert = 1;
	    }
	}
    }
}

/* Close the old stream pointer FP if it is non-NULL,
   and return a new one opened to read the next input file.
   Open a filename of `-' as the standard input.
   Return NULL if there are no more input files.  */

static FILE *
next_file (fp)
     FILE *fp;
{
  static char *prev_file;
  char *file;

  if (fp)
    {
      if (ferror (fp))
	{
	  error (0, errno, "%s", prev_file);
	  exit_status = 1;
	}
      if (fp == stdin)
	clearerr (fp);		/* Also clear EOF. */
      else if (fclose (fp) == EOF)
	{
	  error (0, errno, "%s", prev_file);
	  exit_status = 1;
	}
    }

  while ((file = *file_list++) != NULL)
    {
      if (file[0] == '-' && file[1] == '\0')
	{
	  have_read_stdin = 1;
	  prev_file = file;
	  return stdin;
	}
      fp = fopen (file, "r");
      if (fp)
	{
	  prev_file = file;
	  return fp;
	}
      error (0, errno, "%s", file);
      exit_status = 1;
    }
  return NULL;
}

static void
usage (status)
     int status;
{
  if (status != 0)
    fprintf (stderr, "Try `%s --help' for more information.\n",
	     program_name);
  else
    {
      printf ("\
Usage: %s [OPTION]... [FILE]...\n\
",
	      program_name);
      printf ("\
\n\
  -a, --all           convert all whitespace, instead of initial whitespace\n\
  -t, --tabs=NUMBER   have tabs NUMBER characters apart instead of 8\n\
  -t, --tabs=LIST     use comma separated list of explicit tab positions\n\
      --help          display this help and exit\n\
      --version       output version information and exit\n\
\n\
Instead of -t NUMBER or -t LIST, -NUMBER or -LIST may be used.  With\n\
no FILE, or when FILE is -, read standard input.\n\
");
    }
  exit (status);
}
