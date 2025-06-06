/* env - manipulate environment and execute a program in that environment
   Copyright (C) 1986 Free Software Foundation, Inc.

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

/* Mly 861126 */

/* If first argument is "-", then a new environment is constructed
   from scratch; otherwise the environment is inherited from the parent
   process, except as modified by other options.

   So, "env - foo" will invoke the "foo" program in a null environment,
   whereas "env foo" would invoke "foo" in the same environment as that
   passed to "env" itself.

   Subsequent arguments are interpreted as follows:

   * "variable=value" (i.e., an arg containing a "=" character)
     means to set the specified environment variable to that value.
     `value' may be of zero length ("variable=").  Note that setting
     a variable to a zero-length value is different from unsetting it.

   * "-u variable" or "-unset variable"
     means to unset that variable.
     If that variable isn't set, does nothing.

   * "-s variable value" or "-set variable value"
     same as "variable=value".

   * "-" or "--"
     are used to indicate that the following argument is the program
     to invoke.  This is only necessary when the program's name
     begins with "-" or contains a "=".

   * anything else
     The first remaining argument specifies a program to invoke
     (it is searched for according to the specification of the PATH
     environment variable) and any arguments following that are
     passed as arguments to that program.

     If no program-name is specified following the environment
     specifications, the resulting environment is printed.
     This is like specifying a program-name of "printenv".

   Examples:
     If the environment passed to "env" is
     { USER=rms EDITOR=emacs PATH=.:/gnubin:/hacks }

     * "env DISPLAY=gnu:0 nemacs"
        calls "nemacs" in the environment
	{ USER=rms EDITOR=emacs PATH=.:/gnubin:/hacks DISPLAY=gnu:0 }

     * "env - USER=foo /hacks/hack bar baz"
       calls the "hack" program on arguments "bar" and "baz"
       in an environment in which the only variable is "USER".
       Note that the "-" option clears out the PATH variable,
       so one should be careful to specify in which directory
       to find the program to call.

     * "env -u EDITOR USER=foo PATH=/energy -- e=mc2 bar baz"
       The program "/energy/e=mc2" is called with environment
       { USER=foo PATH=/energy }
*/

#ifdef EMACS
#define NO_SHORTNAMES
#define NOT_C_CODE /* for VMS */
#include "../src/config.h"
#endif /* EMACS */

#include <stdio.h>
#include <errno.h>

extern int execvp ();

char *xmalloc (), *xrealloc ();
char *concat ();

#ifndef VMS
extern char **environ;
#else
#include <unixlib.h>		/* To get the right defs */
#include <perror.h>		/* for errno and so on as well */
#endif

char **nenv;
int nenv_size;

char *progname;
void setenv ();
void fatal ();
char *myindex ();

main (argc, argv, envp)
     register int argc;
     register char **argv;
     char **envp;
{
  register char *tem;

  progname = argv[0];
  argc--;
  argv++;

  nenv_size = 100;
  nenv = (char **) xmalloc (nenv_size * sizeof (char *));
  *nenv = (char *) 0;

  /* "-" flag means to not inherit parent's environment */
  if (argc && !strcmp (*argv, "-"))
    {
      argc--;
      argv++;
    }
  else
    /* Else pass on existing env vars. */
    for (; *envp; envp++)
      {
	tem = myindex (*envp, '=');
	if (tem)
	  {
	    *tem = '\000';
	    setenv (*envp, tem + 1);
	  }
      }

  while (argc > 0)
    {
      tem = myindex (*argv, '=');
      if (tem)
	/* If arg contains a "=" it specifies to set a variable */
	{
	  *tem = '\000';
	  setenv (*argv, tem + 1);
	  argc--;
	  argv++;
	  continue;
	}

      if (**argv != '-')
	/* Remaining args are program name and args to pass it */
	break;

      if (argc < 2)
	fatal ("no argument for `%s' option", *argv);
      if (!strcmp (*argv, "-u")
	  || !strcmp (*argv, "-unset"))
	/* Unset a variable */
	{
	  argc--;
	  argv++;
	  setenv (*argv, (char *) 0);
	  argc--;
	  argv++;
	}
      else if (!strcmp (*argv, "-s") ||
	       !strcmp (*argv, "-set"))
	/* Set a variable */
	{
	  argc--;
	  argv++;
	  tem = *argv;
	  if (argc < 2)
	    fatal ("no value specified for variable \"%s\"", tem);
	  argc--;
	  argv++;
	  setenv (tem, *argv);
	  argc--;
	  argv++;
	}
      else if (!strcmp (*argv, "-") || !strcmp (*argv, "--"))
	{
	  argc--;
	  argv++;
	  break;
	}
      else
	{
	  fatal ("unrecognized option `%s'", *argv);
	}
    }

  /* If no program specified print the environment and exit */
  if (argc <= 0)
    {
      while (*nenv)
	printf ("%s\n", *nenv++);
      exit (0);
    }
  else
    {
#ifndef VMS			/* These are defined in perror.h,
				   earlier in this file.  */
      extern int errno;
      extern int sys_nerr;
      extern char *sys_errlist[];
#endif

      environ = nenv;
      (void) execvp (*argv, argv);

      fprintf (stderr, "%s: cannot execute `%s'", progname, *argv);
      if (errno < sys_nerr)
	fprintf (stderr, ": %s\n", sys_errlist[errno]);
      else
	putc ('\n', stderr);
      exit (errno != 0 ? errno : 1);
    }
}

void
setenv (var, val)
     register char *var, *val;
{
  register char **e;
  int len = strlen (var);

  {
    register char *tem = myindex (var, '=');
    if (tem)
      fatal ("environment variable names can not contain `=': %s", var);
    else if (*var == '\000')
      fatal ("zero-length environment variable name specified");
  }

  for (e = nenv; *e; e++)
    if (!strncmp (var, *e, len) && (*e)[len] == '=')
      {
	if (val)
	  goto set;
	else
	  do
	    {
	      *e = *(e + 1);
	  } while (*e++);
	return;
      }

  if (!val)
    return;			/* Nothing to unset */

  len = e - nenv;
  if (len + 1 >= nenv_size)
    {
      nenv_size += 100;
      nenv = (char **) xrealloc (nenv, nenv_size * sizeof (char *));
      e = nenv + len;
    }

set:
  val = concat (var, "=", val);
  if (*e)
    free (*e);
  else
    *(e + 1) = (char *) 0;
  *e = val;
  return;
}

void
fatal (msg, arg1, arg2)
     char *msg, *arg1, *arg2;
{
  fprintf (stderr, "%s: ", progname);
  fprintf (stderr, msg, arg1, arg2);
  putc ('\n', stderr);
  exit (1);
}


extern char *malloc (), *realloc ();

void
memory_fatal ()
{
  fatal ("virtual memory exhausted");
}

char *
xmalloc (size)
     int size;
{
  register char *value;
  value = (char *) malloc (size);
  if (!value)
    memory_fatal ();
  return (value);
}

char *
xrealloc (ptr, size)
     char *ptr;
     int size;
{
  register char *value;
  value = (char *) realloc (ptr, size);
  if (!value)
    memory_fatal ();
  return (value);
}

/* Return a newly-allocated string whose contents concatenate
   those of S1, S2, S3.  */

char *
concat (s1, s2, s3)
     char *s1, *s2, *s3;
{
  int len1 = strlen (s1), len2 = strlen (s2), len3 = strlen (s3);
  char *result = (char *) xmalloc (len1 + len2 + len3 + 1);

  strcpy (result, s1);
  strcpy (result + len1, s2);
  strcpy (result + len1 + len2, s3);
  result[len1 + len2 + len3] = 0;

  return result;
}

/* Return a pointer to the first occurrence in STR of C,
   or 0 if C does not occur.  */

char *
myindex (str, c)
     char *str;
     char c;
{
  char *s = str;

  while (*s)
    {
      if (*s == c)
	return s;
      s++;
    }
  return 0;
}
