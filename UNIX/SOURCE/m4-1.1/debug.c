/* GNU m4 -- A simple macro processor
   Copyright (C) 1991, 1992, 1993 Free Software Foundation, Inc.
  
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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* #if HAVE_FSTAT && HAVE_FILENO */
#include <sys/types.h>
#include <sys/stat.h>
/* #endif */

#include "m4.h"

/* File for debugging output.  */
static FILE *debug = NULL;

/* Obstack for trace messages.  */
static struct obstack trace;

extern int expansion_level;

static void debug_set_file ();

/*----------------------------------.
| Initialise the debugging module.  |
`----------------------------------*/

void
debug_init (void)
{
  debug_set_file (stderr);
  obstack_init (&trace);
}


/*-----------------------------------------------------------------------.
| Function to decode the debugging flags OPTS.  Used by main (-dXXX) and |
| the builtin debugmode ().						 |
`-----------------------------------------------------------------------*/

int
debug_decode (char *opts)
{
  int level;

  if (opts == NULL || *opts == '\0')
    level = DEBUG_TRACE_DEFAULT;
  else
    {
      for (level = 0; *opts; opts++)
	{
	  switch (*opts)
	    {
	    case 'a':
	      level |= DEBUG_TRACE_ARGS;
	      break;
	    case 'e':
	      level |= DEBUG_TRACE_EXPANSION;
	      break;
	    case 'q':
	      level |= DEBUG_TRACE_QUOTE;
	      break;
	    case 't':
	      level |= DEBUG_TRACE_ALL;
	      break;
	    case 'l':
	      level |= DEBUG_TRACE_LINE;
	      break;
	    case 'f':
	      level |= DEBUG_TRACE_FILE;
	      break;
	    case 'p':
	      level |= DEBUG_TRACE_PATH;
	      break;
	    case 'c':
	      level |= DEBUG_TRACE_CALL;
	      break;
	    case 'i':
	      level |= DEBUG_TRACE_INPUT;
	      break;
	    case 'x':
	      level |= DEBUG_TRACE_CALLID;
	      break;
	    case 'V':
	      level |= DEBUG_TRACE_VERBOSE;
	      break;
	    default:
	      return -1;
	    }
	}
    }

  /* This is to avoid screwing up the trace output due to changes in the
     debug_level.  */

  obstack_free (&trace, obstack_finish (&trace));

  return level;
}

/*------------------------------------------------------------------------.
| Change the debug output stream to FP.  If the underlying file is the	  |
| same as stdout, use stdout instead so that debug messages appear in the |
| correct relative position.						  |
`------------------------------------------------------------------------*/

static void
debug_set_file (FILE *fp)
{
/* #if HAVE_FSTAT && HAVE_FILENO */
  struct stat stdout_stat, debug_stat;
/* #endif */

  if (debug != NULL && debug != stderr && debug != stdout)
    fclose (debug);
  debug = fp;

/* #if HAVE_FSTAT && HAVE_FILENO */

  if (debug != NULL && debug != stdout)
    {
      if (fstat (fileno (stdout), &stdout_stat) < 0)
	return;
      if (fstat (fileno (debug), &debug_stat) < 0)
	return;

      if (stdout_stat.st_ino == debug_stat.st_ino
	  && stdout_stat.st_dev == debug_stat.st_dev)
	{
	  if (debug != stderr)
	    fclose (debug);
	  debug = stdout;
	}
    }

/* #endif HAVE_FSTAT && HAVE_FILENO */
}

/*-------------------------------------------------------------------------.
| Change the debug output to file NAME.  If NAME is NULL, debug output is  |
| reverted to stderr, and if empty debug output is discarded.  Return TRUE |
| iff the output stream was changed.					   |
`-------------------------------------------------------------------------*/

boolean
debug_set_output (char *name)
{
  FILE *fp;

  if (name == NULL)
    debug_set_file (stderr);
  else if (*name == '\0')
    debug_set_file (NULL);
  else
    {
      fp = fopen (name, "a");
      if (fp == NULL)
	return FALSE;

      debug_set_file (fp);
    }
  return TRUE;
}

/*---------------------------------------------------------------------.
| Generic function to do formatted output to the debug output stream.  |
| Usage as for printf ().					       |
`---------------------------------------------------------------------*/

void
debug_print (va_alist)
    va_dcl
{
  va_list args;
  char *fmt;

  if (debug != NULL)
    {
      va_start (args);
      fmt = va_arg (args, char *);
      vfprintf (debug, fmt, args);
      va_end (args);
    }
}

/*--------------------------------------------------------------------.
| Print a one-line debug message, headed by "m4 debug".  Usage as for |
| printf ().							      |
`--------------------------------------------------------------------*/

void
debug_message (va_alist)
    va_dcl
{
  va_list args;
  char *fmt;

  if (debug != NULL)
    {
      fprintf (debug, "m4 debug: ");
      if (debug_level & DEBUG_TRACE_FILE)
	fprintf (debug, "%s: ", current_file);
      if (debug_level & DEBUG_TRACE_LINE)
	fprintf (debug, "%d: ", current_line);

      va_start (args);
      fmt = va_arg (args, char *);
      vfprintf (debug, fmt, args);
      va_end (args);

      putc ('\n', debug);
    }
}

/* The rest of this file contains the functions for macro tracing output.
   All tracing output for a macro call is collected on an obstack TRACE,
   and printed whenever the line is complete.  This prevents tracing
   output from interfering with other debug messages generated by the
   various builtins.  */

/*----------------------------------------------------------------------.
| Tracing output is formatted here, by a simplified printf-to-obstack   |
| function trace_format ().  Understands only %s, %d, %l (optional left |
| quote) and %r (optional right quote).				        |
`----------------------------------------------------------------------*/

static void
trace_format (va_alist)
    va_dcl
{
  va_list args;
  char *fmt;
  char ch;

  int d;
  char nbuf[32];
  char *s;
  int slen;
  int maxlen;

  va_start (args);
  fmt = va_arg (args, char *);

  while (TRUE)
    {
      while ((ch = *fmt++) != '\0' && ch != '%')
	obstack_1grow (&trace, ch);

      if (ch == '\0')
	break;

      maxlen = 0;
      switch (*fmt++)
	{
	case 'S':
	  maxlen = max_debug_argument_length;
	  /* fall through */
	case 's':
	  s = va_arg (args, char *);
	  break;
	case 'l':
	  s = (debug_level & DEBUG_TRACE_QUOTE) ? lquote : "";
	  break;
	case 'r':
	  s = (debug_level & DEBUG_TRACE_QUOTE) ? rquote : "";
	  break;
	case 'd':
	  d = va_arg (args, int);
	  sprintf (nbuf, "%d", d);
	  s = nbuf;
	  break;
	default:
	  s = "";
	  break;
	}

      slen = strlen (s);
      if (maxlen == 0 || maxlen > slen)
	obstack_grow (&trace, s, slen);
      else
	{
	  obstack_grow (&trace, s, maxlen);
	  obstack_grow (&trace, "...", 3);
	}
    }

  va_end (args);
}

/*------------------------------------------------------------------.
| Format the standard header attached to all tracing output lines.  |
`------------------------------------------------------------------*/

static void
trace_header (int id)
{
  trace_format ("m4trace:");
  if (debug_level & DEBUG_TRACE_FILE)
    trace_format ("%s:", current_file);
  if (debug_level & DEBUG_TRACE_LINE)
    trace_format ("%d:", current_line);
  trace_format (" -%d- ", expansion_level);
  if (debug_level & DEBUG_TRACE_CALLID)
    trace_format ("id %d: ", id);
}

/*----------------------------------------------------.
| Print current tracing line, and clear the obstack.  |
`----------------------------------------------------*/

static void
trace_flush (void)
{
  char *line;

  obstack_1grow (&trace, '\0');
  line = obstack_finish (&trace);
  debug_print ("%s\n", line);
  obstack_free (&trace, line);
}

/*-------------------------------------------------------------.
| Do pre-argument-collction tracing for macro NAME.  Used from |
| expand_macro ().					       |
`-------------------------------------------------------------*/

void
trace_prepre (char *name, int id)
{
  trace_header (id);
  trace_format ("%s ...", name);
  trace_flush ();
}

/*-----------------------------------------------------------------------.
| Format the parts of a trace line, that can be made before the macro is |
| actually expanded.  Used from expand_macro ().			 |
`-----------------------------------------------------------------------*/

void
trace_pre (char *name, int id, int argc, token_data **argv)
{
  int i;
  const builtin *bp;

  trace_header (id);
  trace_format ("%s", name);

  if (argc > 1 && (debug_level & DEBUG_TRACE_ARGS))
    {
      trace_format ("(");

      for (i = 1; i < argc; i++)
	{
	  if (i != 1)
	    trace_format (", ");

	  switch (TOKEN_DATA_TYPE (argv[i]))
	    {
	    case TOKEN_TEXT:
	      trace_format ("%l%S%r", TOKEN_DATA_TEXT (argv[i]));
	      break;
	    case TOKEN_FUNC:
	      bp = find_builtin_by_addr (TOKEN_DATA_FUNC (argv[i]));
	      if (bp == NULL)
		internal_error ("builtin not found in builtin table! (trace_pre ())");
	      trace_format ("<%s>", bp->name);
	      break;
	    default:
	      internal_error ("Bad token data type (trace_pre ())");
	      break;
	    }

	}
      trace_format (")");
    }

  if (debug_level & DEBUG_TRACE_CALL)
    {
      trace_format (" -> ???");
      trace_flush ();
    }
}

/*-------------------------------------------------------------------.
| Format the final part of a trace line and print it all.  Used from |
| expand_macro ().						     |
`-------------------------------------------------------------------*/

void
trace_post (char *name, int id, int argc, token_data **argv, char *expanded)
{
  if (debug_level & DEBUG_TRACE_CALL)
    {
      trace_header (id);
      trace_format ("%s%s", name, (argc > 1) ? "(...)" : "");
    }

  if (expanded && (debug_level & DEBUG_TRACE_EXPANSION))
    trace_format (" -> %l%S%r", expanded);
  trace_flush ();
}
