/* env - run a program in a modified environment
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

/* Richard Mlynarik and David MacKenzie */

/* Options:
   -
   -i
   --ignore-environment
	Construct a new environment from scratch; normally the
	environment is inherited from the parent process, except as
	modified by other options.

   -u variable
   --unset=variable
	Unset variable VARIABLE (remove it from the environment).
	If VARIABLE was not set, does nothing.

   variable=value (an arg containing a "=" character)
	Set the environment variable VARIABLE to value VALUE.  VALUE
	may be of zero length ("variable=").  Setting a variable to a
	zero-length value is different from unsetting it.

   --
	Indicate that the following argument is the program
	to invoke.  This is necessary when the program's name
	begins with "-" or contains a "=".

   The first remaining argument specifies a program to invoke;
   it is searched for according to the specification of the PATH
   environment variable.  Any arguments following that are
   passed as arguments to that program.

   If no command name is specified following the environment
   specifications, the resulting environment is printed.
   This is like specifying a command name of "printenv".

   Examples:

   If the environment passed to "env" is
	{ LOGNAME=rms EDITOR=emacs PATH=.:/gnubin:/hacks }

   env - foo
	runs "foo" in a null environment.

   env foo
	runs "foo" in the environment
	{ LOGNAME=rms EDITOR=emacs PATH=.:/gnubin:/hacks }

   env DISPLAY=gnu:0 nemacs
	runs "nemacs" in the envionment
	{ LOGNAME=rms EDITOR=emacs PATH=.:/gnubin:/hacks DISPLAY=gnu:0 }

   env - LOGNAME=foo /hacks/hack bar baz
	runs the "hack" program on arguments "bar" and "baz" in an
	environment in which the only variable is "LOGNAME".  Note that
	the "-" option clears out the PATH variable, so one should be
	careful to specify in which directory to find the program to
	call.

   env -u EDITOR LOGNAME=foo PATH=/energy -- e=mc2 bar baz
	runs the program "/energy/e=mc2" with environment
	{ LOGNAME=foo PATH=/energy }
*/

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
#include <getopt.h>
#include <sys/types.h>
#include <getopt.h>

#include "version.h"
#include "system.h"

int putenv ();
void error ();

static void usage ();

extern char **environ;

/* The name by which this program was run. */
char *program_name;

/* If non-zero, display usage information and exit.  */
static int show_help;

/* If non-zero, print the version on standard output and exit.  */
static int show_version;

static struct option const longopts[] =
{
  {"help", no_argument, &show_help, 1},
  {"ignore-environment", no_argument, NULL, 'i'},
  {"unset", required_argument, NULL, 'u'},
  {"version", no_argument, &show_version, 1},
  {NULL, 0, NULL, 0}
};

void
main (argc, argv, envp)
     register int argc;
     register char **argv;
     char **envp;
{
  char *dummy_environ[1];
  int optc;
  int ignore_environment = 0;

  program_name = argv[0];

  while ((optc = getopt_long (argc, argv, "+iu:", longopts, (int *) 0)) != EOF)
    {
      switch (optc)
	{
	case 0:
	  break;
	case 'i':
	  ignore_environment = 1;
	  break;
	case 'u':
	  break;
	default:
	  usage (2);
	}
    }

  if (show_version)
    {
      printf ("%s\n", version_string);
      exit (0);
    }

  if (show_help)
    usage (0);

  if (optind != argc && !strcmp (argv[optind], "-"))
    ignore_environment = 1;
  
  environ = dummy_environ;
  environ[0] = NULL;

  if (!ignore_environment)
    for (; *envp; envp++)
      putenv (*envp);

  optind = 0;			/* Force GNU getopt to re-initialize. */
  while ((optc = getopt_long (argc, argv, "+iu:", longopts, (int *) 0)) != EOF)
    if (optc == 'u')
      putenv (optarg);		/* Requires GNU putenv. */

  if (optind != argc && !strcmp (argv[optind], "-"))
    ++optind;

  while (optind < argc && index (argv[optind], '='))
    putenv (argv[optind++]);

  /* If no program is specified, print the environment and exit. */
  if (optind == argc)
    {
      while (*environ)
	puts (*environ++);
      exit (0);
    }

  execvp (argv[optind], &argv[optind]);
  error (errno == ENOENT ? 127 : 126, errno, "%s", argv[optind]);
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
      printf ("Usage: %s [OPTION]... [-] [NAME=VALUE]... [COMMAND [ARG]...]\n",
	      program_name);
      printf ("\
\n\
  -u, --unset=NAME           remove variable from the environment\n\
  -i, --ignore-environment   start with an empty environment\n\
      --help                 display this help and exit\n\
      --version              output version information and exit\n\
\n\
A mere - implies -i.  If no COMMAND, print the resulting environment.\n\
");
    }
  exit (status);
}
