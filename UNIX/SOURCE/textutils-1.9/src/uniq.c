/* uniq -- remove duplicate lines from a sorted file
   Copyright (C) 1986, 1991 Free Software Foundation, Inc.

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

/* Written by Richard Stallman and David MacKenzie. */

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
#include "linebuffer.h"
#include "version.h"

#define min(x, y) ((x) < (y) ? (x) : (y))

void error ();

static char *find_field ();
static int different ();
static void check_file ();
static void usage ();
static void writeline ();

/* The name this program was run with. */
char *program_name;

/* Number of fields to skip on each line when doing comparisons. */
static int skip_fields;

/* Number of chars to skip after skipping any fields. */
static int skip_chars;

/* Number of chars to compare; if 0, compare the whole lines. */
static int check_chars;

enum countmode
{
  count_occurrences,		/* -c Print count before output lines. */
  count_none			/* Default.  Do not print counts. */
};

/* Whether and how to precede the output lines with a count of the number of
   times they occurred in the input. */
static enum countmode countmode;

enum output_mode
{
  output_repeated,		/* -d Only lines that are repeated. */
  output_unique,		/* -u Only lines that are not repeated. */
  output_all			/* Default.  Print first copy of each line. */
};

/* Which lines to output. */
static enum output_mode mode;

/* If non-zero, display usage information and exit.  */
static int show_help;

/* If non-zero, print the version on standard output then exit.  */
static int show_version;

static struct option const longopts[] =
{
  {"count", no_argument, NULL, 'c'},
  {"repeated", no_argument, NULL, 'd'},
  {"unique", no_argument, NULL, 'u'},
  {"skip-fields", required_argument, NULL, 'f'},
  {"skip-chars", required_argument, NULL, 's'},
  {"check-chars", required_argument, NULL, 'w'},
  {"help", no_argument, &show_help, 1},
  {"version", no_argument, &show_version, 1},
  {NULL, 0, NULL, 0}
};

void
main (argc, argv)
     int argc;
     char *argv[];
{
  int optc;
  char *infile = "-", *outfile = "-";

  program_name = argv[0];
  skip_chars = 0;
  skip_fields = 0;
  check_chars = 0;
  mode = output_all;
  countmode = count_none;

  while ((optc = getopt_long (argc, argv, "0123456789cdf:s:uw:", longopts,
			      (int *) 0)) != EOF)
    {
      switch (optc)
	{
	case 0:
	  break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	  skip_fields = skip_fields * 10 + optc - '0';
	  break;

	case 'c':
	  countmode = count_occurrences;
	  break;

	case 'd':
	  mode = output_repeated;
	  break;

	case 'f':		/* Like '-#'. */
	  skip_fields = atoi (optarg);
	  break;

	case 's':		/* Like '+#'. */
	  skip_chars = atoi (optarg);
	  break;

	case 'u':
	  mode = output_unique;
	  break;

	case 'w':
	  check_chars = atoi (optarg);
	  break;

	default:
	  usage (1);
	}
    }

  if (show_version)
    {
      printf ("%s\n", version_string);
      exit (0);
    }

  if (show_help)
    usage (0);

  if (optind >= 2 && strcmp (argv[optind - 1], "--") != 0)
    {
      /* Interpret non-option arguments with leading `+' only
	 if we haven't seen `--'.  */
      while (optind < argc && argv[optind][0] == '+')
	skip_chars = atoi (argv[optind++]);
    }

  if (optind < argc)
    infile = argv[optind++];

  if (optind < argc)
    outfile = argv[optind++];

  if (optind < argc)
    usage (1);			/* Extra arguments. */

  check_file (infile, outfile);

  exit (0);
}

/* Process input file INFILE with output to OUTFILE.
   If either is "-", use the standard I/O stream for it instead. */

static void
check_file (infile, outfile)
     char *infile, *outfile;
{
  FILE *istream;
  FILE *ostream;
  struct linebuffer lb1, lb2;
  struct linebuffer *thisline, *prevline, *exch;
  char *prevfield, *thisfield;
  int prevlen, thislen;
  int match_count = 0;

  if (!strcmp (infile, "-"))
    istream = stdin;
  else
    istream = fopen (infile, "r");
  if (istream == NULL)
    error (1, errno, "%s", infile);

  if (!strcmp (outfile, "-"))
    ostream = stdout;
  else
    ostream = fopen (outfile, "w");
  if (ostream == NULL)
    error (1, errno, "%s", outfile);

  thisline = &lb1;
  prevline = &lb2;

  initbuffer (thisline);
  initbuffer (prevline);

  if (readline (prevline, istream) == 0)
    goto closefiles;
  prevfield = find_field (prevline);
  prevlen = prevline->length - (prevfield - prevline->buffer);

  while (!feof (istream))
    {
      if (readline (thisline, istream) == 0)
	break;
      thisfield = find_field (thisline);
      thislen = thisline->length - (thisfield - thisline->buffer);
      if (!different (thisfield, prevfield, thislen, prevlen))
	match_count++;
      else
	{
	  writeline (prevline, ostream, match_count);
	  match_count = 0;

	  exch = prevline;
	  prevline = thisline;
	  thisline = exch;
	  prevfield = thisfield;
	  prevlen = thislen;
	}
    }

  writeline (prevline, ostream, match_count);

 closefiles:
  if (ferror (istream) || fclose (istream) == EOF)
    error (1, errno, "error reading %s", infile);

  if (ferror (ostream) || fclose (ostream) == EOF)
    error (1, errno, "error writing %s", outfile);

  free (lb1.buffer);
  free (lb2.buffer);
}

/* Given a linebuffer LINE,
   return a pointer to the beginning of the line's field to be compared. */

static char *
find_field (line)
     struct linebuffer *line;
{
  register int count;
  register char *lp = line->buffer;
  register int size = line->length;
  register int i = 0;

  for (count = 0; count < skip_fields && i < size; count++)
    {
      while (i < size && ISBLANK (lp[i]))
	i++;
      while (i < size && !ISBLANK (lp[i]))
	i++;
    }

  for (count = 0; count < skip_chars && i < size; count++)
    i++;

  return lp + i;
}

/* Return zero if two strings OLD and NEW match, nonzero if not.
   OLD and NEW point not to the beginnings of the lines
   but rather to the beginnings of the fields to compare.
   OLDLEN and NEWLEN are their lengths. */

static int
different (old, new, oldlen, newlen)
     char *old;
     char *new;
     int oldlen;
     int newlen;
{
  register int order;

  if (check_chars)
    {
      if (oldlen > check_chars)
	oldlen = check_chars;
      if (newlen > check_chars)
	newlen = check_chars;
    }
  order = memcmp (old, new, min (oldlen, newlen));
  if (order == 0)
    return oldlen - newlen;
  return order;
}

/* Output the line in linebuffer LINE to stream STREAM
   provided that the switches say it should be output.
   If requested, print the number of times it occurred, as well;
   LINECOUNT + 1 is the number of times that the line occurred. */

static void
writeline (line, stream, linecount)
     struct linebuffer *line;
     FILE *stream;
     int linecount;
{
  if ((mode == output_unique && linecount != 0)
      || (mode == output_repeated && linecount == 0))
    return;

  if (countmode == count_occurrences)
    fprintf (stream, "%7d\t", linecount + 1);

  fwrite (line->buffer, sizeof (char), line->length, stream);
  putc ('\n', stream);
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
Usage: %s [OPTION]... [INPUT [OUTPUT]]\n\
",
	      program_name);
      printf ("\
\n\
  -c, --count           prefix lines by the number of occurrences\n\
  -d, --repeated        only print duplicate lines\n\
  -f, --skip-fields=N   avoid comparing the N first fields\n\
  -s, --skip-chars=N    avoid comparing the N first characters\n\
  -u, --unique          only print unique lines\n\
  -w, --check-chars=N   compare no more then N characters in lines\n\
  -N                    same as -f N\n\
  +N                    same as -s N\n\
      --help            display this help and exit\n\
      --version         output version information and exit\n\
\n\
A field is a run of whitespace, than non whitespace characters.\n\
Fields are skipped before chars.  If OUTPUT not specified, writes to\n\
standard output.  If INPUT not specified, reads from standard input.\n\
");
    }
  exit (status);
}
