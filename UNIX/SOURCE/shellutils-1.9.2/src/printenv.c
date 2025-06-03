/* printenv -- print all or part of environment
   Copyright (C) 1989, 1991 Free Software Foundation.

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

/* Usage: printenv [variable...]

   If no arguments are given, print the entire environment.
   If one or more variable names are given, print the value of
   each one that is set, and nothing for ones that are not set.

   Exit status:
   0 if all variables specified were found
   1 if not

   David MacKenzie and Richard Mlynarik */

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

#include <stdio.h>
#include <sys/types.h>
#include <getopt.h>

#include "version.h"
#include "system.h"

void error ();

/* The name this program was run with. */
char *program_name;

/* If non-zero, display usage information and exit.  */
static int show_help;

/* If non-zero, print the version on standard output and exit.  */
static int show_version;

static struct option const long_options[] =
{
  {"help", no_argument, &show_help, 1},
  {"version", no_argument, &show_version, 1},
  {0, 0, 0, 0}
};

extern char **environ;

static void
usage (status)
     int status;
{
  if (status != 0)
    fprintf (stderr, "Try `%s --help' for more information.\n",
	     program_name);
  else
    {
      printf ("Usage: %s [OPTION]... [VARIABLE]...\n", program_name);
      printf ("\
\n\
  --help      display this help and exit\n\
  --version   output version information and exit\n\
\n\
If no VARIABLE, print them all.\n\
");
    }
  exit (status);
}

void
main (argc, argv)
     int argc;
     char **argv;
{
  char **env;
  char *ep, *ap;
  int i;
  int matches = 0;
  int c;
  int exit_status;

  program_name = argv[0];

  while ((c = getopt_long (argc, argv, "", long_options, (int *) 0)) != EOF)
    {
      switch (c)
	{
	case 0:
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

  if (optind == argc)
    {
      for (env = environ; *env != NULL; ++env)
	puts (*env);
      exit_status = 0;
    }
  else
    {
      for (i = optind; i < argc; ++i)
	{
	  for (env = environ; *env; ++env)
	    {
	      ep = *env;
	      ap = argv[i];
	      while (*ep != '\0' && *ap != '\0' && *ep++ == *ap++)
		{
		  if (*ep == '=' && *ap == '\0')
		    {
		      puts (ep + 1);
		      ++matches;
		      break;
		    }
		}
	    }
	}
      exit_status = (matches != argc - optind);
    }

  if (ferror (stdout) || fclose (stdout) == EOF)
    error (2, errno, "standard output");

  exit (exit_status);
}
