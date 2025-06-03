/* GNU m4 -- A simple macro processor
   Copyright (C) 1989, 90, 91, 92, 93 Free Software Foundation, Inc.
  
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

#include "m4.h"
#include "getopt.h"
#include <sys/types.h>
#include <sys/signal.h>

static void usage ();

/* Current version numbers.  */
static const char version[] = "1.1";

/* Operate interactively (-e).  */
static int interactive = 0;

/* Enable sync output for /lib/cpp (-s).  */
int sync_output = 0;

/* Debug (-d[flags]).  */
int debug_level = 0;

/* Hash table size (should be a prime) (-Hsize).  */
int hash_table_size = HASHMAX;

/* Number of diversions allocated.  */
int ndiversion = NDIVERSIONS;

/* Disable GNU extensions (-G).  */
int no_gnu_extensions = 0;

/* Max length of arguments in trace output (-lsize).  */
int max_debug_argument_length = 0;

/* Suppress warnings about missing arguments.  */
int suppress_warnings = 0;

/* The name this program was run with. */
const char *program_name;

/* If non-zero, display usage information and exit.  */
static int show_help;

/* If non-zero, print the version on standard output and exit.  */
static int show_version;

struct macro_definition
{
  struct macro_definition *next;
  int code;			/* D, U or t */
  char *macro;
};
typedef struct macro_definition macro_definition;

/* Error handling functions.  */

/*----------------------------------------------.
| Basic varargs function for all error output.  |
`----------------------------------------------*/

static void
vmesg (char *level, va_list args)
{
  char *fmt;

  fflush (stdout);
  fmt = va_arg (args, char *);
  fprintf (stderr, "%s:%s:%d: ", program_name, current_file, current_line);
  if (level != NULL)
    fprintf (stderr, "%s: ", level);
  vfprintf (stderr, fmt, args);
  putc ('\n', stderr);
}

/*-----------------------------------------.
| Internal errors -- print and dump core.  |
`-----------------------------------------*/

volatile void
internal_error (va_alist)
    va_dcl
{
  va_list args;
  va_start (args);
  vmesg ("INTERNAL ERROR", args);
  va_end (args);

  abort ();
}

/*--------------------------------.
| Fatal error -- print and exit.  |
`--------------------------------*/

volatile void
fatal (va_alist)
    va_dcl
{
  va_list args;
  va_start (args);
  vmesg ("ERROR", args);
  va_end (args);

  exit (1);
}

/*----------------------------------.
| "Normal" error -- just complain.  |
`----------------------------------*/

void
m4error (va_alist)
    va_dcl
{
  va_list args;
  va_start (args);
  vmesg ((char *) NULL, args);
  va_end (args);
}

/*------------------------------------.
| Warning --- for potential trouble.  |
`------------------------------------*/

void
warning (va_alist)
    va_dcl
{
  va_list args;
  va_start (args);
  vmesg ("Warning", args);
  va_end (args);
}

/* Memory allocation.  */

/*------------------------.
| Failsafe free routine.  |
`------------------------*/

void
xfree (void *p)
{
  if (p != NULL)
    free (p);
}


/*---------------------------------------------.
| Print a usage message and exit with STATUS.  |
`---------------------------------------------*/

static void
usage (int status)
{
  if (status != 0)
    fprintf (stderr, "Try `%s --help' for more information.\n", program_name);
  else
    {
      printf ("Usage: %s [OPTION]... [FILE]...\n", program_name);
      printf ("\
\n\
  -D, --define=NAME[=VALUE]   enter NAME has having VALUE, or empty\n\
  -G, --traditional           suppress all GNU extensions\n\
  -H, --hashsize=PRIME        set symbol lookup hash table size\n\
  -I, --include=DIRECTORY     search this directory second for includes\n\
  -N, --diversions=NUMBER     select number of simultaneous diversions\n\
  -P, --prefix-builtins       force a `m4_' prefix to all builtins\n\
  -Q, --quiet, --silent       suppress some warnings for builtins\n\
  -U, --undefine=NAME         delete builtin NAME\n\
  -d, --debug=[FLAGS]         set debug level\n\
  -e, --interactive           unbuffer output, ignore interrupts\n\
  -l, --arglength=NUM         restrict macro tracing size\n\
  -o, --error-output=FILE     redirect debug and trace output\n\
  -s, --synclines             generate `#line NO \"FILE\"' lines\n\
  -t, --trace=NAME            trace NAME when it will be defined\n\
      --help                  display this help and exit\n\
      --version               output version information and exit\n\
\n\
If no FILE or if FILE is `-', standard input is read.\n");
    }
  exit (status);
}

/*--------------------------------------.
| Decode options and launch execution.  |
`--------------------------------------*/

static const struct option long_options[] =
{
  {"arglength", required_argument, NULL, 'l'},
  {"debug", optional_argument, NULL, 'd'},
  {"diversions", required_argument, NULL, 'N'},
  {"erroroutput", required_argument, NULL, 'o'},
  {"hashsize", required_argument, NULL, 'H'},
  {"include", required_argument, NULL, 'I'},
  {"interactive", no_argument, NULL, 'e'},
  {"prefix-builtins", no_argument, NULL, 'P'},
  {"quiet", no_argument, NULL, 'Q'},
  {"silent", no_argument, NULL, 'Q'},
  {"synclines", no_argument, NULL, 's'},
  {"traditional", no_argument, NULL, 'G'},

  {"help", no_argument, &show_help, 1},
  {"version", no_argument, &show_version, 1},

  /* These are somewhat troublesome.  */
  { "define", required_argument, NULL, 'D' },
  { "undefine", required_argument, NULL, 'U' },
  { "trace", required_argument, NULL, 't' },

  { 0, 0, 0, 0 },
};

#define OPTSTRING "B:D:GH:I:N:PQS:T:U:d::el:o:st:"

int
main (int argc, char *const *argv)
{
  macro_definition *head;	/* head of deferred argument list */
  macro_definition *tail;
  macro_definition *new;
  int optchar;			/* option character */

  macro_definition *defines;
  FILE *fp;

  program_name = rindex (argv[0], '/');
  if (program_name == NULL)
    program_name = argv[0];
  else
    program_name++;

  include_init ();
  debug_init ();

  /* First, we decode the arguments, to size up tables and stuff.  */

  head = tail = NULL;

  while (optchar = getopt_long (argc, argv, OPTSTRING, long_options, NULL),
	 optchar != EOF)
    switch (optchar)
      {
      default:
	usage (1);

      case 0:
	break;

      case 'e':
	interactive = 1;
	break;

      case 's':
	sync_output = 1;
	break;

      case 'G':
	no_gnu_extensions = 1;
	break;

      case 'Q':
	suppress_warnings = 1;
	break;

      case 'd':
	debug_level = debug_decode (optarg);
	if (debug_level < 0)
	  {
	    error (0, 0, "bad debug flags: `%s'", optarg);
	    debug_level = 0;
	  }
	break;

      case 'l':
	max_debug_argument_length = atoi (optarg);
	if (max_debug_argument_length <= 0)
	  max_debug_argument_length = 0;
	break;

      case 'o':
	if (!debug_set_output (optarg))
	  error (0, errno, optarg);
	break;

      case 'H':
	hash_table_size = atoi (optarg);
	if (hash_table_size <= 0)
	  hash_table_size = HASHMAX;
	break;

      case 'N':
	ndiversion = atoi (optarg);
	if (ndiversion <= 0)
	  ndiversion = NDIVERSIONS;
	break;

      case 'I':
	add_include_directory (optarg);
	break;

      case 'B':			/* compatibility junk */
      case 'S':
      case 'T':
	break;

      case 'D':
      case 'U':
      case 't':

	/* Arguments that cannot be handled until later are accumulated.  */

	new = (macro_definition *) xmalloc (sizeof (macro_definition));
	new->code = optchar;
	new->macro = optarg;
	new->next = NULL;

	if (head == NULL)
	  head = new;
	else
	  tail->next = new;
	tail = new;

	break;

      case 'P':
	prefix_all_builtins ();
	break;
      }

  if (show_version)
    {
      printf ("GNU m4 %s\n", version);
      exit (0);
    }

  if (show_help)
    usage (0);

  defines = head;

  /* Do the basic initialisations.  */

  input_init ();
  output_init ();
  symtab_init ();
  builtin_init ();
  include_env_init ();

  /* Handle deferred command line macro definitions.  Must come after
     initialisation of the symbol table.  */

  while (defines != NULL)
    {
      macro_definition *next;
      char *macro_value;
      symbol *sym;

      switch (defines->code)
	{
	case 'D':
	  macro_value = index (defines->macro, '=');
	  if (macro_value == NULL)
	    macro_value = "";
	  else
	    *macro_value++ = '\0';
	  define_user_macro (defines->macro, macro_value, SYMBOL_INSERT);
	  break;

	case 'U':
	  lookup_symbol (defines->macro, SYMBOL_DELETE);
	  break;

	case 't':
	  sym = lookup_symbol (defines->macro, SYMBOL_INSERT);
	  SYMBOL_TRACED (sym) = TRUE;
	  break;

	default:
	  internal_error ("bad code in deferred arguments.");
	}

      next = defines->next;
      xfree (defines);
      defines = next;
    }

  /* Interactive mode means unbuffered output, and interrupts ignored.  */

  if (interactive)
    {
      signal (SIGINT, SIG_IGN);
      setbuf (stdout, (char *) NULL);
    }

  /* Handle the various input files.  Each file is pushed on the input,
     and the input read.  Wrapup text is handled separately later.  */

  if (optind == argc)
    {
      push_file (stdin, "stdin");
      expand_input ();
    }
  else
    for (; optind < argc; optind++)
      {
	if (strcmp (argv[optind], "-") == 0)
	  {
	    push_file (stdin, "stdin");
	  }
	else
	  {
	    fp = path_search (argv[optind]);
	    if (fp == NULL)
	      {
		error (0, errno, argv[optind]);
		continue;
	      }
	    else
	      push_file (fp, argv[optind]);
	  }
	expand_input ();
      }
#undef NEXTARG

  /* Now handle wrapup text.  */

  while (pop_wrapup ())
    expand_input ();

  make_diversion (0);
  undivert_all ();

  exit (0);
}
