/* Support routines for GNU DIFF.
   Copyright 1988, 89, 92, 93, 94, 95, 1998 Free Software Foundation, Inc.

   This file is part of GNU DIFF.

   GNU DIFF is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GNU DIFF is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.
   If not, write to the Free Software Foundation, 
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "diff.h"
#include "quotesys.h"

char const pr_program[] = PR_PROGRAM;

/* Queue up one-line messages to be printed at the end,
   when -l is specified.  Each message is recorded with a `struct msg'.  */

struct msg
{
  struct msg *next;
  char args[1]; /* Format + 4 args, each '\0' terminated, concatenated.  */
};

/* Head of the chain of queues messages.  */

static struct msg *msg_chain;

/* Tail of the chain of queues messages.  */

static struct msg **msg_chain_end = &msg_chain;

/* Use when a system call returns non-zero status.
   TEXT should normally be the file name.  */

void
perror_with_name (text)
     char const *text;
{
  error (0, errno, "%s", text);
}

/* Use when a system call returns non-zero status and that is fatal.  */

void
pfatal_with_name (text)
     char const *text;
{
  int e = errno;
  print_message_queue ();
  error (2, e, "%s", text);
  abort ();
}

/* Print an error message containing MSGID, then exit.  */

void
fatal (msgid)
     char const *msgid;
{
  print_message_queue ();
  error (2, 0, "%s", _(msgid));
  abort ();
}

/* Like printf, except if -l in effect then save the message and print later.
   This is used for things like "Only in ...".  */

void
message (format_msgid, arg1, arg2)
     char const *format_msgid, *arg1, *arg2;
{
  message5 (format_msgid, arg1, arg2, 0, 0);
}

void
message5 (format_msgid, arg1, arg2, arg3, arg4)
     char const *format_msgid, *arg1, *arg2, *arg3, *arg4;
{
  if (paginate_flag)
    {
      char *p;
      char const *arg[5];
      int i;
      size_t size[5];
      size_t total_size = sizeof (struct msg);
      struct msg *new;

      arg[0] = format_msgid;
      arg[1] = arg1;
      arg[2] = arg2;
      arg[3] = arg3 ? arg3 : "";
      arg[4] = arg4 ? arg4 : "";

      for (i = 0;  i < 5;  i++)
	total_size += size[i] = strlen (arg[i]) + 1;

      new = (struct msg *) xmalloc (total_size);

      for (i = 0, p = new->args;  i < 5;  p += size[i++])
	memcpy (p, arg[i], size[i]);

      *msg_chain_end = new;
      new->next = 0;
      msg_chain_end = &new->next;
    }
  else
    {
      if (sdiff_help_sdiff)
	putchar (' ');
      printf (_(format_msgid), arg1, arg2, arg3, arg4);
    }
}

/* Output all the messages that were saved up by calls to `message'.  */

void
print_message_queue ()
{
  char const *arg[5];
  int i;
  struct msg *m = msg_chain;

  while (m)
    {
      struct msg *next = m->next;
      arg[0] = m->args;
      for (i = 0;  i < 4;  i++)
	arg[i + 1] = arg[i] + strlen (arg[i]) + 1;
      printf (_(arg[0]), arg[1], arg[2], arg[3], arg[4]);
      free (m);
      m = next;
    }
}

/* Call before outputting the results of comparing files NAME0 and NAME1
   to set up OUTFILE, the stdio stream for the output to go to.

   Usually, OUTFILE is just stdout.  But when -l was specified
   we fork off a `pr' and make OUTFILE a pipe to it.
   `pr' then outputs to our stdout.  */

static char const *current_name0;
static char const *current_name1;
static int currently_recursive;

void
setup_output (name0, name1, recursive)
     char const *name0, *name1;
     int recursive;
{
  current_name0 = name0;
  current_name1 = name1;
  currently_recursive = recursive;
  outfile = 0;
}

#if HAVE_FORK
static pid_t pr_pid;
#endif

void
begin_output ()
{
  char *name;

  if (outfile != 0)
    return;

  /* Construct the header of this piece of diff.  */
  name = xmalloc (strlen (current_name0) + strlen (current_name1)
		  + strlen (switch_string) + 7);
  /* Posix.2 section 4.17.6.1.1 specifies this format.  But there are some
     bugs in the first printing (IEEE Std 1003.2-1992 p 251 l 3304):
     it says that we must print only the last component of the pathnames,
     and it requires two spaces after "diff" if there are no options.
     These requirements are silly and do not match historical practice.  */
  sprintf (name, "diff%s %s %s", switch_string, current_name0, current_name1);

  if (paginate_flag)
    {
      if (fflush (stdout) != 0)
	pfatal_with_name (_("write failed"));

      /* Make OUTFILE a pipe to a subsidiary `pr'.  */
      {
#if HAVE_FORK
	int pipes[2];
	char const *not_found = _(": not found\n");

	if (pipe (pipes) != 0)
	  pfatal_with_name ("pipe");

	pr_pid = vfork ();
	if (pr_pid < 0)
	  pfatal_with_name ("fork");

	if (pr_pid == 0)
	  {
	    close (pipes[1]);
	    if (pipes[0] != STDIN_FILENO)
	      {
		if (dup2 (pipes[0], STDIN_FILENO) < 0)
		  pfatal_with_name ("dup2");
		close (pipes[0]);
	      }

	    execl (pr_program, pr_program, "-f", "-h", name, 0);
	    /* Avoid stdio, because the parent process's buffers are inherited.
	       Also, avoid gettext since it may modify the parent buffers.  */
	    write (STDERR_FILENO, pr_program, strlen (pr_program));
	    write (STDERR_FILENO, not_found, strlen (not_found));
	    _exit (1);
	  }
	else
	  {
	    close (pipes[0]);
	    outfile = fdopen (pipes[1], "w");
	    if (!outfile)
	      pfatal_with_name ("fdopen");
	  }
#else /* ! HAVE_FORK */
	char *command = xmalloc (sizeof (pr_program) - 1 + 7
				 + quote_system_arg ((char *) 0, name) + 1);
	char *p;
	sprintf (command, "%s -f -h ", pr_program);
	p = command + sizeof (pr_program) - 1 + 7;
	p += quote_system_arg (p, name);
	*p = 0;
	outfile = popen (command, "w");
	if (!outfile)
	  pfatal_with_name (command);
	free (command);
#endif /* ! HAVE_FORK */
      }
    }
  else
    {

      /* If -l was not specified, output the diff straight to `stdout'.  */

      outfile = stdout;

      /* If handling multiple files (because scanning a directory),
	 print which files the following output is about.  */
      if (currently_recursive)
	printf ("%s\n", name);
    }

  free (name);

  /* A special header is needed at the beginning of context output.  */
  switch (output_style)
    {
    case OUTPUT_CONTEXT:
      print_context_header (files, 0);
      break;

    case OUTPUT_UNIFIED:
      print_context_header (files, 1);
      break;

    default:
      break;
    }
}

/* Call after the end of output of diffs for one file.
   Close OUTFILE and get rid of the `pr' subfork.  */

void
finish_output ()
{
  if (outfile != 0 && outfile != stdout)
    {
      int wstatus;
      if (ferror (outfile))
	fatal ("write failed");
#if ! HAVE_FORK
      wstatus = pclose (outfile);
#else /* HAVE_FORK */
      if (fclose (outfile) != 0)
	pfatal_with_name (_("write failed"));
      if (waitpid (pr_pid, &wstatus, 0) < 0)
	pfatal_with_name ("waitpid");
#endif /* HAVE_FORK */
      if (wstatus != 0)
	fatal ("subsidiary program failed");
    }

  outfile = 0;
}

/* Compare two lines (typically one from each input file)
   according to the command line options.
   For efficiency, this is invoked only when the lines do not match exactly
   but an option like -i might cause us to ignore the difference.
   Return nonzero if the lines differ.  */

int
line_cmp (s1, s2)
     char const *s1, *s2;
{
  register unsigned char const *t1 = (unsigned char const *) s1;
  register unsigned char const *t2 = (unsigned char const *) s2;

  while (1)
    {
      register unsigned char c1 = *t1++;
      register unsigned char c2 = *t2++;

      /* Test for exact char equality first, since it's a common case.  */
      if (c1 != c2)
	{
	  /* Ignore horizontal white space if -b or -w is specified.  */

	  if (ignore_all_space_flag)
	    {
	      /* For -w, just skip past any white space.  */
	      while (ISSPACE (c1) && c1 != '\n') c1 = *t1++;
	      while (ISSPACE (c2) && c2 != '\n') c2 = *t2++;
	    }
	  else if (ignore_space_change_flag)
	    {
	      /* For -b, advance past any sequence of white space in line 1
		 and consider it just one Space, or nothing at all
		 if it is at the end of the line.  */
	      if (ISSPACE (c1))
		{
		  while (c1 != '\n')
		    {
		      c1 = *t1++;
		      if (! ISSPACE (c1))
			{
			  --t1;
			  c1 = ' ';
			  break;
			}
		    }
		}

	      /* Likewise for line 2.  */
	      if (ISSPACE (c2))
		{
		  while (c2 != '\n')
		    {
		      c2 = *t2++;
		      if (! ISSPACE (c2))
			{
			  --t2;
			  c2 = ' ';
			  break;
			}
		    }
		}

	      if (c1 != c2)
		{
		  /* If we went too far when doing the simple test
		     for equality, go back to the first non-white-space
		     character in both sides and try again.  */
		  if (c2 == ' ' && c1 != '\n'
		      && (unsigned char const *) s1 + 1 < t1
		      && ISSPACE (t1[-2]))
		    {
		      --t1;
		      continue;
		    }
		  if (c1 == ' ' && c2 != '\n'
		      && (unsigned char const *) s2 + 1 < t2
		      && ISSPACE (t2[-2]))
		    {
		      --t2;
		      continue;
		    }
		}
	    }

	  /* Lowercase all letters if -i is specified.  */

	  if (ignore_case_flag)
	    {
	      if (ISUPPER (c1))
		c1 = _tolower (c1);
	      if (ISUPPER (c2))
		c2 = _tolower (c2);
	    }

	  if (c1 != c2)
	    break;
	}
      if (c1 == '\n')
	return 0;
    }

  return 1;
}

/* Find the consecutive changes at the start of the script START.
   Return the last link before the first gap.  */

struct change *
find_change (start)
     struct change *start;
{
  return start;
}

struct change *
find_reverse_change (start)
     struct change *start;
{
  return start;
}

/* Divide SCRIPT into pieces by calling HUNKFUN and
   print each piece with PRINTFUN.
   Both functions take one arg, an edit script.

   HUNKFUN is called with the tail of the script
   and returns the last link that belongs together with the start
   of the tail.

   PRINTFUN takes a subscript which belongs together (with a null
   link at the end) and prints it.  */

void
print_script (script, hunkfun, printfun)
     struct change *script;
     struct change * (*hunkfun) PARAMS((struct change *));
     void (*printfun) PARAMS((struct change *));
{
  struct change *next = script;

  while (next)
    {
      struct change *this, *end;

      /* Find a set of changes that belong together.  */
      this = next;
      end = (*hunkfun) (next);

      /* Disconnect them from the rest of the changes,
	 making them a hunk, and remember the rest for next iteration.  */
      next = end->link;
      end->link = 0;
#ifdef DEBUG
      debug_script (this);
#endif

      /* Print this hunk.  */
      (*printfun) (this);

      /* Reconnect the script so it will all be freed properly.  */
      end->link = next;
    }
}

/* Print the text of a single line LINE,
   flagging it with the characters in LINE_FLAG (which say whether
   the line is inserted, deleted, changed, etc.).  */

void
print_1_line (line_flag, line)
     char const *line_flag;
     char const * const *line;
{
  char const *text = line[0], *limit = line[1]; /* Help the compiler.  */
  FILE *out = outfile; /* Help the compiler some more.  */
  char const *flag_format = 0;

  /* If -T was specified, use a Tab between the line-flag and the text.
     Otherwise use a Space (as Unix diff does).
     Print neither space nor tab if line-flags are empty.  */

  if (line_flag && *line_flag)
    {
      flag_format = tab_align_flag ? "%s\t" : "%s ";
      fprintf (out, flag_format, line_flag);
    }

  output_1_line (text, limit, flag_format, line_flag);

  if ((!line_flag || line_flag[0]) && limit[-1] != '\n')
    fprintf (out, "\n\\ %s\n", _("No newline at end of file"));
}

#ifndef __VMS /* replacement in vms/vmsputline.c */

/* Output a line from TEXT up to LIMIT.  Without -t, output verbatim.
   With -t, expand white space characters to spaces, and if FLAG_FORMAT
   is nonzero, output it with argument LINE_FLAG after every
   internal carriage return, so that tab stops continue to line up.  */

void
output_1_line (text, limit, flag_format, line_flag)
     char const *text, *limit, *flag_format, *line_flag;
{
  if (!tab_expand_flag)
    fwrite (text, sizeof (char), limit - text, outfile);
  else
    {
      register FILE *out = outfile;
      register unsigned char c;
      register char const *t = text;
      register unsigned column = 0;

      while (t < limit)
	switch ((c = *t++))
	  {
	  case '\t':
	    {
	      unsigned spaces = TAB_WIDTH - column % TAB_WIDTH;
	      column += spaces;
	      do
		putc (' ', out);
	      while (--spaces);
	    }
	    break;

	  case '\r':
	    putc (c, out);
	    if (flag_format && t < limit && *t != '\n')
	      fprintf (out, flag_format, line_flag);
	    column = 0;
	    break;

	  case '\b':
	    if (column == 0)
	      continue;
	    column--;
	    putc (c, out);
	    break;

	  default:
	    if (ISPRINT (c))
	      column++;
	    putc (c, out);
	    break;
	  }
    }
}

#endif /* __VMS */

int
change_letter (inserts, deletes)
     int inserts, deletes;
{
  if (!inserts)
    return 'd';
  else if (!deletes)
    return 'a';
  else
    return 'c';
}

/* Translate an internal line number (an index into diff's table of lines)
   into an actual line number in the input file.
   The internal line number is LNUM.  FILE points to the data on the file.

   Internal line numbers count from 0 starting after the prefix.
   Actual line numbers count from 1 within the entire file.  */

int
translate_line_number (file, lnum)
     struct file_data const *file;
     int lnum;
{
  return lnum + file->prefix_lines + 1;
}

void
translate_range (file, a, b, aptr, bptr)
     struct file_data const *file;
     int a, b;
     int *aptr, *bptr;
{
  *aptr = translate_line_number (file, a - 1) + 1;
  *bptr = translate_line_number (file, b + 1) - 1;
}

/* Print a pair of line numbers with SEPCHAR, translated for file FILE.
   If the two numbers are identical, print just one number.

   Args A and B are internal line numbers.
   We print the translated (real) line numbers.  */

void
print_number_range (sepchar, file, a, b)
     int sepchar;
     struct file_data *file;
     int a, b;
{
  int trans_a, trans_b;
  translate_range (file, a, b, &trans_a, &trans_b);

  /* Note: we can have B < A in the case of a range of no lines.
     In this case, we should print the line number before the range,
     which is B.  */
  if (trans_b > trans_a)
    fprintf (outfile, "%d%c%d", trans_a, sepchar, trans_b);
  else
    fprintf (outfile, "%d", trans_b);
}

/* Look at a hunk of edit script and report the range of lines in each file
   that it applies to.  HUNK is the start of the hunk, which is a chain
   of `struct change'.  The first and last line numbers of file 0 are stored in
   *FIRST0 and *LAST0, and likewise for file 1 in *FIRST1 and *LAST1.
   Note that these are internal line numbers that count from 0.

   If no lines from file 0 are deleted, then FIRST0 is LAST0+1.

   Also set *DELETES nonzero if any lines of file 0 are deleted
   and set *INSERTS nonzero if any lines of file 1 are inserted.
   If only ignorable lines are inserted or deleted, both are
   set to 0.  */

void
analyze_hunk (hunk, first0, last0, first1, last1, deletes, inserts)
     struct change *hunk;
     int *first0, *last0, *first1, *last1;
     int *deletes, *inserts;
{
  struct change *next;
  int l0, l1, show_from, show_to;
  int i;
  int trivial = ignore_blank_lines_flag || ignore_regexp.fastmap;
  int trivial_length = ignore_blank_lines_flag - 1;
    /* If 0, ignore zero-length lines;
       if -1, do not ignore any lines just because of their length.  */

  char const * const *linbuf0 = files[0].linbuf;  /* Help the compiler.  */
  char const * const *linbuf1 = files[1].linbuf;

  show_from = show_to = 0;

  *first0 = hunk->line0;
  *first1 = hunk->line1;

  next = hunk;
  do
    {
      l0 = next->line0 + next->deleted - 1;
      l1 = next->line1 + next->inserted - 1;
      show_from += next->deleted;
      show_to += next->inserted;

      for (i = next->line0; i <= l0 && trivial; i++)
	{
	  char const *line = linbuf0[i];
	  int len = linbuf0[i + 1] - line - 1;
	  if (len != trivial_length
	      && (! ignore_regexp.fastmap
		  || re_search (&ignore_regexp, line, len, 0, len, 0) < 0))
	    trivial = 0;
	}

      for (i = next->line1; i <= l1 && trivial; i++)
	{
	  char const *line = linbuf1[i];
	  int len = linbuf1[i + 1] - line - 1;
	  if (len != trivial_length
	      && (! ignore_regexp.fastmap
		  || re_search (&ignore_regexp, line, len, 0, len, 0) < 0))
	    trivial = 0;
	}
    }
  while ((next = next->link) != 0);

  *last0 = l0;
  *last1 = l1;

  /* If all inserted or deleted lines are ignorable,
     tell the caller to ignore this hunk.  */

  if (trivial)
    show_from = show_to = 0;

  *deletes = show_from;
  *inserts = show_to;
}

/* Concatenate three strings, returning a newly malloc'd string.  */

char *
concat (s1, s2, s3)
     char const *s1, *s2, *s3;
{
  char *new = xmalloc (strlen (s1) + strlen (s2) + strlen (s3) + 1);
  sprintf (new, "%s%s%s", s1, s2, s3);
  return new;
}

/* Yield the newly malloc'd pathname
   of the file in DIR whose filename is FILE.  */

char *
dir_file_pathname (dir, file)
     char const *dir, *file;
{
  char const *p = filename_lastdirchar (dir);
  return concat (dir, "/" + (p && !p[1]), file);
}

void
debug_script (sp)
     struct change *sp;
{
  fflush (stdout);
  for (; sp; sp = sp->link)
    fprintf (stderr, "%3d %3d delete %d insert %d\n",
	     sp->line0, sp->line1, sp->deleted, sp->inserted);
  fflush (stderr);
}
