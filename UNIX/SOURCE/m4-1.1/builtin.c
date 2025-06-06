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

/* Code for all builtin macros, initialisation of symbol table, and
   expansion of user defined macros.  */

#include "m4.h"
#ifndef size_t
#include <sys/types.h>
#endif
#include "regex.h"

#define ARG(i)	(argc > (i) ? TOKEN_DATA_TEXT (argv[i]) : "")

/* Initialisation of builtin and predefined macros.  The table
   "builtin_tab" is both used for initialisation, and by the "builtin"
   builtin.  */

#ifndef __STDC__
#define DECLARE(name) static void name ()
#else
#define DECLARE(name) static void name (struct obstack *, int, token_data **)
#endif

DECLARE (m4___file__);
DECLARE (m4___line__);
DECLARE (m4_builtin);
DECLARE (m4_changecom);
DECLARE (m4_changequote);
DECLARE (m4_debugmode);
DECLARE (m4_debugfile);
DECLARE (m4_decr);
DECLARE (m4_define);
DECLARE (m4_defn);
DECLARE (m4_divert);
DECLARE (m4_divnum);
DECLARE (m4_dnl);
DECLARE (m4_dumpdef);
DECLARE (m4_errprint);
DECLARE (m4_esyscmd);
DECLARE (m4_eval);
DECLARE (m4_format);
DECLARE (m4_ifdef);
DECLARE (m4_ifelse);
DECLARE (m4_include);
DECLARE (m4_incr);
DECLARE (m4_index);
DECLARE (m4_indir);
DECLARE (m4_len);
DECLARE (m4_m4exit);
DECLARE (m4_m4wrap);
DECLARE (m4_maketemp);
DECLARE (m4_patsubst);
DECLARE (m4_popdef);
DECLARE (m4_pushdef);
DECLARE (m4_regexp);
DECLARE (m4_shift);
DECLARE (m4_sinclude);
DECLARE (m4_substr);
DECLARE (m4_syscmd);
DECLARE (m4_sysval);
DECLARE (m4_traceoff);
DECLARE (m4_traceon);
DECLARE (m4_translit);
DECLARE (m4_undefine);
DECLARE (m4_undivert);

#undef DECLARE

static builtin
builtin_tab[] =
{

  /* name		gnu	args	blind	function */

  { "__file__",		TRUE,	FALSE,	FALSE,	m4___file__ },
  { "__line__",		TRUE,	FALSE,	FALSE,	m4___line__ },
  { "builtin",		TRUE,	FALSE,	TRUE,	m4_builtin },
  { "changecom",	FALSE,	FALSE,	FALSE,	m4_changecom },
  { "changequote",	FALSE,	FALSE,	FALSE,	m4_changequote },
  { "debugmode",	TRUE,	FALSE,	FALSE,	m4_debugmode },
  { "debugfile",	TRUE,	FALSE,	FALSE,	m4_debugfile },
  { "decr",		FALSE,	FALSE,	TRUE,	m4_decr },
  { "define",		FALSE,	TRUE,	TRUE,	m4_define },
  { "defn",		FALSE,	FALSE,	TRUE,	m4_defn },
  { "divert",		FALSE,	FALSE,	FALSE,	m4_divert },
  { "divnum",		FALSE,	FALSE,	FALSE,	m4_divnum },
  { "dnl",		FALSE,	FALSE,	FALSE,	m4_dnl },
  { "dumpdef",		FALSE,	FALSE,	FALSE,	m4_dumpdef },
  { "errprint",		FALSE,	FALSE,	FALSE,	m4_errprint },
  { "esyscmd",		TRUE,	FALSE,	TRUE,	m4_esyscmd },
  { "eval",		FALSE,	FALSE,	TRUE,	m4_eval },
  { "format",		TRUE,	FALSE,	FALSE,	m4_format },
  { "ifdef",		FALSE,	FALSE,	TRUE,	m4_ifdef },
  { "ifelse",		FALSE,	FALSE,	TRUE,	m4_ifelse },
  { "include",		FALSE,	FALSE,	TRUE,	m4_include },
  { "incr",		FALSE,	FALSE,	TRUE,	m4_incr },
  { "index",		FALSE,	FALSE,	TRUE,	m4_index },
  { "indir",		TRUE,	FALSE,	FALSE,	m4_indir },
  { "len",		FALSE,	FALSE,	TRUE,	m4_len },
  { "m4exit",		FALSE,	FALSE,	FALSE,	m4_m4exit },
  { "m4wrap",		FALSE,	FALSE,	FALSE,	m4_m4wrap },
  { "maketemp",		FALSE,	FALSE,	TRUE,	m4_maketemp },
  { "patsubst",		TRUE,	FALSE,	TRUE,	m4_patsubst },
  { "popdef",		FALSE,	FALSE,	TRUE,	m4_popdef },
  { "pushdef",		FALSE,	TRUE,	TRUE,	m4_pushdef },
  { "regexp",		TRUE,	FALSE,	TRUE,	m4_regexp },
  { "shift",		FALSE,	FALSE,	FALSE,	m4_shift },
  { "sinclude",		FALSE,	FALSE,	TRUE,	m4_sinclude },
  { "substr",		FALSE,	FALSE,	TRUE,	m4_substr },
  { "syscmd",		FALSE,	FALSE,	TRUE,	m4_syscmd },
  { "sysval",		FALSE,	FALSE,	FALSE,	m4_sysval },
  { "traceoff",		FALSE,	FALSE,	FALSE,	m4_traceoff },
  { "traceon",		FALSE,	FALSE,	FALSE,	m4_traceon },
  { "translit",		FALSE,	FALSE,	TRUE,	m4_translit },
  { "undefine",		FALSE,	FALSE,	TRUE,	m4_undefine },
  { "undivert",		FALSE,	FALSE,	FALSE,	m4_undivert },

  { 0,			FALSE,	FALSE,	FALSE,	0 },
};

static predefined const
predefined_tab[] =
{
  { "unix",	"__unix__",	"" },
  { NULL,	"__gnu__",	"" },

  { NULL,	NULL,		NULL },
};

/* The number of the currently active diversion.  */
static int current_diversion;


/*----------------------------------------.
| Prefix names of all builtins by "m4_".  |
`----------------------------------------*/

void
prefix_all_builtins (void)
{
  static int already_prefixed = 0;

  char *string;
  builtin *cursor;
  builtin *limit;

  if (already_prefixed)
    return;

  limit = builtin_tab + (sizeof (builtin_tab) / sizeof (builtin));
  for (cursor = builtin_tab; cursor < limit; cursor++)
    {
      string = xmalloc (strlen (cursor->name) + 4);
      strcpy (string, "m4_");
      strcat (string, cursor->name);
      cursor->name = string;
    }

  already_prefixed = 1;
}

/*----------------------------------------.
| Find the builtin, which lives on ADDR.  |
`----------------------------------------*/

const builtin *
find_builtin_by_addr (builtin_func *func)
{
  const builtin *bp;

  for (bp = &builtin_tab[0]; bp->name != NULL; bp++)
    if (bp->func == func)
      return bp;
  return NULL;
}

/*-----------------------------------.
| Find the builtin, which has NAME.  |
`-----------------------------------*/

const builtin *
find_builtin_by_name (char *name)
{
  const builtin *bp;

  for (bp = &builtin_tab[0]; bp->name != NULL; bp++)
    if (strcmp (bp->name, name) == 0)
      return bp;
  return NULL;
}


/*-------------------------------------------------------------------------.
| Install a builtin macro with name NAME, bound to the C function given in |
| BP.  MODE is SYMBOL_INSERT or SYMBOL_PUSHDEF.  TRACED defines wheather   |
| NAME is to be traced.							   |
`-------------------------------------------------------------------------*/

static void
define_builtin (const char *name, const builtin *bp, symbol_lookup mode,
		boolean traced)
{
  symbol *sym;

  sym = lookup_symbol (name, mode);
  SYMBOL_TYPE (sym) = TOKEN_FUNC;
  SYMBOL_MACRO_ARGS (sym) = bp->groks_macro_args;
  SYMBOL_BLIND_NO_ARGS (sym) = bp->blind_if_no_args;
  SYMBOL_FUNC (sym) = bp->func;
  SYMBOL_TRACED (sym) = traced;
}

/*-------------------------------------------------------------------------.
| Define a predefined or user-defined macro, with name NAME, and expansion |
| TEXT.  MODE destinguishes between the "define" and the "pushdef" case.   |
| It is also used from main ().						   |
`-------------------------------------------------------------------------*/

void
define_user_macro (const char *name, const char *text, symbol_lookup mode)
{
  symbol *s;

  s = lookup_symbol (name, mode);
  if (SYMBOL_TYPE (s) == TOKEN_TEXT)
    xfree (SYMBOL_TEXT (s));

  SYMBOL_TYPE (s) = TOKEN_TEXT;
  SYMBOL_TEXT (s) = xstrdup (text);
}

/*-----------------------------------------------.
| Initialise all builtin and predefined macros.	 |
`-----------------------------------------------*/

void
builtin_init (void)
{
  const builtin *bp;
  const predefined *pp;

  for (bp = &builtin_tab[0]; bp->name != NULL; bp++)
    {
      if (!(no_gnu_extensions && bp->gnu_extension))
	define_builtin (bp->name, bp, SYMBOL_INSERT, FALSE);
    }

  for (pp = &predefined_tab[0]; pp->func != NULL; pp++)
    {
      if (no_gnu_extensions)
	{
	  if (pp->unix_name != NULL)
	    define_user_macro (pp->unix_name, pp->func, SYMBOL_INSERT);
	}
      else
	{
	  if (pp->gnu_name != NULL)
	    define_user_macro (pp->gnu_name, pp->func, SYMBOL_INSERT);
	}
    }

  current_diversion = 0;
}

/*------------------------------------------------------------------------.
| Give friendly warnings if a builtin macro is passed an inappropriate	  |
| number of arguments.  NAME is macro name for messages, ARGC is actual	  |
| number of arguments, MIN is the minimum number of acceptable arguments, |
| negative if not applicable, MAX is the maximum number, negative if not  |
| applicable.								  |
`------------------------------------------------------------------------*/

static boolean
bad_argc (token_data *name, int argc, int min, int max)
{
  boolean isbad = FALSE;

  if (min > 0 && argc < min)
    {
      if (!suppress_warnings)
	warning ("too few arguments to built-in `%s'", TOKEN_DATA_TEXT (name));
      isbad = TRUE;
    }
  else if (max > 0 && argc > max && !suppress_warnings)
    warning ("excess arguments to built-in `%s' ignored",
	     TOKEN_DATA_TEXT (name));

  return isbad;
}

/*--------------------------------------------------------------------------.
| The function numeric_arg () converts ARG to an int pointed to by VALUEP.  |
| If the conversion fails, print error message for macro MACRO.  Return	    |
| TRUE iff conversion succeeds.						    |
`--------------------------------------------------------------------------*/

static boolean
numeric_arg (token_data *macro, char *arg, int *valuep)
{
  if (sscanf (arg, "%d", valuep) != 1)
    {
      m4error ("non-numeric argument to built-in `%s'", TOKEN_DATA_TEXT (macro));
      return FALSE;
    }
  return TRUE;
}

/*------------------------------------------------------------------------.
| The function ntoa () converts VALUE to a signed ascii representation in |
| radix RADIX.								  |
`------------------------------------------------------------------------*/

/* Digits for number to ascii conversions.  */
static char const digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

static char *
ntoa (register int value, int radix)
{
  boolean negative;
  unsigned int uvalue;
  static char str[256];
  register char *s = &str[sizeof str];

  *--s = '\0';

  if (value < 0)
    {
      negative = TRUE;
      uvalue = (unsigned int) -value;
    }
  else
    {
      negative = FALSE;
      uvalue = (unsigned int) value;
    }

  do
    {
      *--s = digits[uvalue % radix];
      uvalue /= radix;
    }
  while (uvalue > 0);

  if (negative)
    *--s = '-';
  return s;
}

/*----------------------------------------------------------------------.
| Format an int VAL, and stuff it into an obstack OBS.  Used for macros |
| expanding to numbers.						        |
`----------------------------------------------------------------------*/

static void
shipout_int (struct obstack *obs, int val)
{
  char *s;

  s = ntoa (val, 10);
  obstack_grow (obs, s, strlen (s));
}

/*----------------------------------------------------------------------.
| Print ARGC arguments from the table ARGV to obstack OBS, separated by |
| SEP, and quoted by the current quotes, if QUOTED is TRUE.	        |
`----------------------------------------------------------------------*/

static void
dump_args (struct obstack *obs, int argc, token_data **argv,
	   char *sep, boolean quoted)
{
  int i;
  int len = strlen (sep);

  for (i = 1; i < argc; i++)
    {
      if (i > 1)
	obstack_grow (obs, sep, len);
      if (quoted)
	obstack_grow (obs, lquote, len_lquote);
      obstack_grow (obs, TOKEN_DATA_TEXT (argv[i]),
		    strlen (TOKEN_DATA_TEXT (argv[i])));
      if (quoted)
	obstack_grow (obs, rquote, len_rquote);
    }

}

/* The rest of this file is code for builtins and expansion of user
   defined macros.  All the functions for builtins have a prototype as:
   
	void m4_MACRONAME (struct obstack *obs, int argc, char *argv[]);
   
   The function are expected to leave their expansion on the obstack OBS,
   as an unfinished object.  ARGV is a table of ARGC pointers to the
   individual arguments to the macro.  Please note that in general
   argv[argc] != NULL.  */

/* The first section are macros for definining, undefining, examining,
   changing, ... other macros.  */

/*-------------------------------------------------------------------------.
| The function define_macro is common for the builtins "define",	   |
| "undefine", "pushdef" and "popdef".  ARGC and ARGV is as for the caller, |
| and MODE argument determines how the macro name is entered into the	   |
| symbol table.								   |
`-------------------------------------------------------------------------*/

static void
define_macro (int argc, token_data **argv, symbol_lookup mode)
{
  const builtin *bp;

  if (bad_argc (argv[0], argc, 2, 3))
    return;

  if (TOKEN_DATA_TYPE (argv[1]) != TOKEN_TEXT)
    return;

  if (argc == 2)
    {
      define_user_macro (ARG (1), "", mode);
      return;
    }

  switch (TOKEN_DATA_TYPE (argv[2]))
    {
    case TOKEN_TEXT:
      define_user_macro (ARG (1), ARG (2), mode);
      break;
    case TOKEN_FUNC:
      bp = find_builtin_by_addr (TOKEN_DATA_FUNC (argv[2]));
      if (bp == NULL)
	return;
      else
	define_builtin (ARG (1), bp, mode, TOKEN_DATA_FUNC_TRACED (argv[2]));
      break;
    default:
      internal_error ("Bad token data type in define_macro ()");
      break;
    }
  return;
}

static void
m4_define (struct obstack *obs, int argc, token_data **argv)
{
  define_macro (argc, argv, SYMBOL_INSERT);
}

static void
m4_undefine (struct obstack *obs, int argc, token_data **argv)
{
  if (bad_argc (argv[0], argc, 2, 2))
    return;
  lookup_symbol (ARG (1), SYMBOL_DELETE);
}

static void
m4_pushdef (struct obstack *obs, int argc, token_data **argv)
{
  define_macro (argc, argv,  SYMBOL_PUSHDEF);
}

static void
m4_popdef (struct obstack *obs, int argc, token_data **argv)
{
  if (bad_argc (argv[0], argc, 2, 2))
    return;
  lookup_symbol (ARG (1), SYMBOL_POPDEF);
}


/*---------------------.
| Conditionals of m4.  |
`---------------------*/

static void
m4_ifdef (struct obstack *obs, int argc, token_data **argv)
{
  symbol *s;
  char *result;

  if (bad_argc (argv[0], argc, 3, 4))
    return;
  s = lookup_symbol (ARG (1), SYMBOL_LOOKUP);

  if (s != NULL)
    result = ARG (2);
  else if (argc == 4)
    result = ARG (3);
  else
    result = NULL;

  if (result != NULL)
    obstack_grow (obs, result, strlen (result));
}

static void
m4_ifelse (struct obstack *obs, int argc, token_data **argv)
{
  char *result;
  token_data *argv0;

  if (argc == 2)
    return;

  if (bad_argc (argv[0], argc, 4, -1))
    return;
  else
    /* Diagnose excess arguments if 5, 8, 11, etc., actual arguments.  */
    bad_argc (argv[0], (argc + 2) % 3, -1, 1);

  argv0 = argv[0];
  argv++;
  argc--;

  result = NULL;
  while (result == NULL)

    if (strcmp (ARG (0), ARG (1)) == 0)
      result = ARG (2);

    else
      switch (argc)
	{
	case 3:
	  return;

	case 4:
	case 5:
	  result = ARG (3);
	  break;

	default:
	  argc -= 3;
	  argv += 3;
	}

  obstack_grow (obs, result, strlen (result));
}


/*---------------------------------------------------------------------.
| The function dump_symbol () is for use by "dumpdef".  It builds up a |
| table of all defined, un-shadowed, symbols.			       |
`---------------------------------------------------------------------*/

/* The structure dump_symbol_data is used to pass the information needed
   from call to call to dump_symbol.  */

struct dump_symbol_data
{
  struct obstack *obs;		/* obstack for table */
  symbol **base;		/* base of table */
  int size;			/* size of table */
};

static void
dump_symbol (symbol *sym, struct dump_symbol_data *data)
{
  if (!SYMBOL_SHADOWED (sym) && SYMBOL_TYPE (sym) != TOKEN_VOID)
    {
      obstack_blank (data->obs, sizeof (symbol *));
      data->base = (symbol **) obstack_base (data->obs);
      data->base[data->size++] = sym;
    }
}

/*------------------------------------------------------------------------.
| qsort comparison routine, for sorting the table made in m4_dumpdef ().  |
`------------------------------------------------------------------------*/

static int
dumpdef_cmp (const void *s1, const void *s2)
{
  return strcmp (SYMBOL_NAME (* (const symbol **) s1),
		 SYMBOL_NAME (* (const symbol **) s2));
}

/*-------------------------------------------------------------------------.
| Implementation of "dumpdef" itself.  It builds up a table of pointers to |
| symbols, sorts it and prints the sorted table.			   |
`-------------------------------------------------------------------------*/

static void
m4_dumpdef (struct obstack *obs, int argc, token_data **argv)
{
  symbol *s;
  int i;
  struct dump_symbol_data data;
  const builtin *bp;

  data.obs = obs;
  data.base = (symbol **) obstack_base (obs);
  data.size = 0;

  if (argc == 1)
    {
      hack_all_symbols (dump_symbol, (char *) &data);
    }
  else
    {
      for (i = 1; i < argc; i++)
	{
	  s = lookup_symbol (TOKEN_DATA_TEXT (argv[i]), SYMBOL_LOOKUP);
	  if (s != NULL && SYMBOL_TYPE (s) != TOKEN_VOID)
	    dump_symbol (s, &data);
	  else
	    m4error ("Undefined name %s", TOKEN_DATA_TEXT (argv[i]));
	}
    }

  /* Make table of symbols invisible to expand_macro ().  */

  (void) obstack_finish (obs);

  qsort ((char *) data.base, data.size, sizeof (symbol *), dumpdef_cmp);

  for (; data.size > 0; --data.size, data.base++)
    {
      debug_print ("%s:\t", SYMBOL_NAME (data.base[0]));

      switch (SYMBOL_TYPE (data.base[0]))
	{
	case TOKEN_TEXT:
	  if (debug_level & DEBUG_TRACE_QUOTE)
	    debug_print ("%s%s%s\n", lquote, SYMBOL_TEXT (data.base[0]), rquote);
	  else
	    debug_print ("%s\n", SYMBOL_TEXT (data.base[0]));
	  break;
	case TOKEN_FUNC:
	  bp = find_builtin_by_addr (SYMBOL_FUNC (data.base[0]));
	  if (bp == NULL)
	    internal_error ("built-in not found in builtin table!");
	  debug_print ("<%s>\n", bp->name);
	  break;
	default:
	  internal_error ("Bad token data type in m4_dumpdef ()");
	  break;
	}
    }
}

/*---------------------------------------------------------------------.
| The builtin "builtin" allows calls to builtin macros, even if their  |
| definition has been overridden or shadowed.  It is thus possible to  |
| redefine builtins, and still access their original definition.  This |
| macro is not available in compatibility mode.			       |
`---------------------------------------------------------------------*/

static void
m4_builtin (struct obstack *obs, int argc, token_data **argv)
{
  const builtin *bp;
  char *name = ARG (1);

  if (bad_argc (argv[0], argc, 2, -1))
    return;

  bp = find_builtin_by_name (name);
  if (bp == NULL)
    m4error ("Undefined name %s", name);
  else
    (*bp->func) (obs, argc - 1, argv + 1);
}

/*------------------------------------------------------------------------.
| The builtin "indir" allows indirect calls to macros, even if their name |
| is not a proper macro name.  It is thus possible to define macros with  |
| ill-formed names for internal use in larger macro packages.  This macro |
| is not available in compatibility mode.				  |
`------------------------------------------------------------------------*/

static void
m4_indir (struct obstack *obs, int argc, token_data **argv)
{
  symbol *s;
  char *name = ARG (1);

  if (bad_argc (argv[0], argc, 1, -1))
    return;

  s = lookup_symbol (name, SYMBOL_LOOKUP);
  if (s == NULL)
    m4error ("Undefined macro `%s'", name);
  else
    call_macro (s, argc - 1, argv + 1, obs);
}

/*-------------------------------------------------------------------------.
| The macro "defn" returns the quoted definition of the macro named by the |
| first argument.  If the macro is builtin, it will push a special	   |
| macro-definition token on ht input stack.				   |
`-------------------------------------------------------------------------*/

static void
m4_defn (struct obstack *obs, int argc, token_data **argv)
{
  symbol *s;

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  s = lookup_symbol (ARG (1), SYMBOL_LOOKUP);
  if (s == NULL)
    return;

  switch (SYMBOL_TYPE (s))
    {
    case TOKEN_TEXT:
      obstack_grow (obs, lquote, len_lquote);
      obstack_grow (obs, SYMBOL_TEXT (s), strlen (SYMBOL_TEXT (s)));
      obstack_grow (obs, rquote, len_rquote);
      break;
    case TOKEN_FUNC:
      push_macro (SYMBOL_FUNC (s), SYMBOL_TRACED (s));
      break;
    case TOKEN_VOID:
      break;
    default:
      internal_error ("Bad symbol type in m4_defn ()");
      break;
    }
}


/*------------------------------------------------------------------------.
| This section contains macros to handle the builtins "syscmd", "esyscmd" |
| and "sysval".  "esyscmd" is GNU specific.				  |
`------------------------------------------------------------------------*/

/* Exit code from last "syscmd" command.  */
static int sysval;

static void
m4_syscmd (struct obstack *obs, int argc, token_data **argv)
{
  if (bad_argc (argv[0], argc, 2, 2))
    return;
  sysval = system (ARG (1));
}

static void
m4_esyscmd (struct obstack *obs, int argc, token_data **argv)
{
  FILE *pin;
  int ch;

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  pin = popen (ARG (1), "r");
  if (pin == NULL)
    {
      m4error ("Cannot open pipe to command \"%s\": %s", ARG (1), syserr ());
      sysval = 0xff << 8;
    }
  else
    {
      while ((ch = getc (pin)) != EOF)
	obstack_1grow (obs, (char) ch);
      sysval = pclose (pin);
    }
}

static void
m4_sysval (struct obstack *obs, int argc, token_data **argv)
{
  shipout_int (obs, (sysval >> 8) & 0xff);
}


/*-------------------------------------------------------------------------.
| This section contains the top level code for the "eval" builtin.  The	   |
| actual work is done in the function evaluate (), which lives in eval.c.  |
`-------------------------------------------------------------------------*/

static void
m4_eval (struct obstack *obs, int argc, token_data **argv)
{
  int value;
  int radix = 10;
  int min = 1;
  char *s;

  if (bad_argc (argv[0], argc, 2, 4))
    return;

  if (argc >= 3 && !numeric_arg (argv[0], ARG (2), &radix))
    return;

  if (radix <= 1 || radix > strlen (digits))
    {
      m4error ("radix in eval out of range (radix = %d)", radix);
      return;
    }

  if (argc >= 4 && !numeric_arg (argv[0], ARG (3), &min))
    return;
  if  (min <= 0)
    {
      m4error ("negative width to eval");
      return;
    }

  if (evaluate (ARG (1), &value))
    return;

  s = ntoa (value, radix);

  if (*s == '-')
    {
      obstack_1grow (obs, '-');
      min--;
      s++;
    }
  for (min -= strlen (s); --min >= 0;)
    obstack_1grow (obs, '0');

  obstack_grow (obs, s, strlen (s));
}

static void
m4_incr (struct obstack *obs, int argc, token_data **argv)
{
  int value;

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  if (!numeric_arg (argv[0], ARG (1), &value))
    return;

  shipout_int (obs, value + 1);
}

static void
m4_decr (struct obstack *obs, int argc, token_data **argv)
{
  int value;

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  if (!numeric_arg (argv[0], ARG (1), &value))
    return;

  shipout_int (obs, value - 1);
}

/* This section contains the macros "divert", "undivert" and "divnum" for
   handling diversion.  The utility functions used lives in output.c.  */

/*-----------------------------------------------------------------------.
| Divert further output to the diversion given by ARGV[1].  Out of range |
| means discard further output.						 |
`-----------------------------------------------------------------------*/

static void
m4_divert (struct obstack *obs, int argc, token_data **argv)
{
  int i = 0;

  if (bad_argc (argv[0], argc, 1, 2))
    return;

  if (argc == 2 && !numeric_arg (argv[0], ARG (1), &i))
    return;

  make_diversion (i);
  current_diversion = i;
}

/*-----------------------------------------------------.
| Expand to the current diversion number, -1 if none.  |
`-----------------------------------------------------*/

static void
m4_divnum (struct obstack *obs, int argc, token_data **argv)
{
  if (bad_argc (argv[0], argc, 1, 1))
    return;
  shipout_int (obs, current_diversion);
}

/*-----------------------------------------------------------------------.
| Bring back the diversion given by the argument list.  If none is	 |
| specified, bring back all diversions.  GNU specific is the option of	 |
| undiverting named files, by passing a non-numeric argument to undivert |
| ().									 |
`-----------------------------------------------------------------------*/

static void
m4_undivert (struct obstack *obs, int argc, token_data **argv)
{
  int i, div;
  FILE *fp;

  if (argc == 1)
    {
      undivert_all ();
    }
  else
    for (i = 1; i < argc; i++)
      {
	if (sscanf (ARG (i), "%d", &div) == 1)
	  insert_diversion (div);
	else if (no_gnu_extensions)
	  m4error ("non-numeric argument to %s", TOKEN_DATA_TEXT (argv[0]));
	else
	  {
	    fp = path_search (ARG (i));
	    if (fp != NULL)
	      {
		insert_file (fp);
		fclose (fp);
	      }
	    else
	      m4error ("can't undivert %s: %s", ARG (i), syserr ());
	  }
      }
}

/* This section contains various macros, which does not fall into any
   specific group.  These are "dnl", "shift", "changequote" and
   "changecom".  */

/*------------------------------------------------------------------------.
| Delete all subsequent whitespace from input.  The function skip_line () |
| lives in input.c.							  |
`------------------------------------------------------------------------*/

static void
m4_dnl (struct obstack *obs, int argc, token_data **argv)
{
  if (bad_argc (argv[0], argc, 1, 1))
    return;

  skip_line ();
}

/*-------------------------------------------------------------------------.
| Shift all argument one to the left, discarding the first argument.  Each |
| output argument is quoted with the current quotes.			   |
`-------------------------------------------------------------------------*/

static void
m4_shift (struct obstack *obs, int argc, token_data **argv)
{
  dump_args (obs, argc - 1, argv + 1, ",", TRUE);
}

/*--------------------------------------------------------------------------.
| Change the current quotes.  The function set_quotes () lives in input.c.  |
`--------------------------------------------------------------------------*/

static void
m4_changequote (struct obstack *obs, int argc, token_data **argv)
{
  if (bad_argc (argv[0], argc, 1, 3))
    return;

  set_quotes ((argc >= 2) ? TOKEN_DATA_TEXT (argv[1]) : NULL,
	     (argc >= 3) ? TOKEN_DATA_TEXT (argv[2]) : NULL);
}

/*--------------------------------------------------------------------.
| Change the current comment delimiters.  The function set_comment () |
| lives in input.c.						      |
`--------------------------------------------------------------------*/

static void
m4_changecom (struct obstack *obs, int argc, token_data **argv)
{
  if (bad_argc (argv[0], argc, 1, 3))
    return;

  if (argc == 1)
    set_comment ("", "");	/* disable comments */
  else
    set_comment (TOKEN_DATA_TEXT (argv[1]),
		(argc >= 3) ? TOKEN_DATA_TEXT (argv[2]) : NULL);
}

/* This section contains macros for inclusion of other files -- "include"
   and "sinclude".  This differs from bringing back diversions, in that
   the input is scanned before being copied to the output.  */

/*-------------------------------------------------------------------------.
| Generic include function.  Include the file given by the first argument, |
| if it exists.  Complain about inaccesible files iff SILENT is FALSE.	   |
`-------------------------------------------------------------------------*/

static void
include (int argc, token_data **argv, boolean silent)
{
  FILE *fp;

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  fp = path_search (ARG (1));
  if (fp == NULL)
    {
      if (!silent)
	m4error ("can't open %s: %s", ARG (1), syserr ());
      return;
    }

  push_file (fp, ARG (1));
}

/*------------------------------------------------.
| Include a file, complaining in case of errors.  |
`------------------------------------------------*/

static void
m4_include (struct obstack *obs, int argc, token_data **argv)
{
  include (argc, argv, FALSE);
}

/*----------------------------------.
| Include a file, ignoring errors.  |
`----------------------------------*/

static void
m4_sinclude (struct obstack *obs, int argc, token_data **argv)
{
  include (argc, argv, TRUE);
}

/* More miscellaneous builtins -- "maketemp", "errprint", "__file__" and
   "__line__".  The last two are GNU specific.  */

/*------------------------------------------------------------------.
| Use the first argument as at template for a temporary file name.  |
`------------------------------------------------------------------*/

static void
m4_maketemp (struct obstack *obs, int argc, token_data **argv)
{
  if (bad_argc (argv[0], argc, 2, 2))
    return;
  mktemp (ARG (1));
  obstack_grow (obs, ARG (1), strlen (ARG (1)));
}

/*----------------------------------------.
| Print all arguments on standard error.  |
`----------------------------------------*/

static void
m4_errprint (struct obstack *obs, int argc, token_data **argv)
{
  dump_args (obs, argc, argv, " ", FALSE);
  obstack_1grow (obs, '\0');
  fprintf (stderr, "%s", (char *) obstack_finish (obs));
  fflush (stderr);
}

static void
m4___file__ (struct obstack *obs, int argc, token_data **argv)
{
  if (bad_argc (argv[0], argc, 1, 1))
    return;
  obstack_grow (obs, lquote, len_lquote);
  obstack_grow (obs, current_file, strlen (current_file));
  obstack_grow (obs, rquote, len_rquote);
}

static void
m4___line__ (struct obstack *obs, int argc, token_data **argv)
{
  if (bad_argc (argv[0], argc, 1, 1))
    return;
  shipout_int (obs, current_line);
}

/* This section contains various macros for exiting, saving input until
   EOF is seen, and tracing macro calls.  That is: "m4exit", "m4wrap",
   "traceon" and "traceoff".  */

/*-------------------------------------------------------------------------.
| Exit immediately, with exitcode specified by the first argument, 0 if no |
| arguments are present.						   |
`-------------------------------------------------------------------------*/

static void
m4_m4exit (struct obstack *obs, int argc, token_data **argv)
{
  int exit_code = 0;

  if (bad_argc (argv[0], argc, 1, 2))
    return;
  if (argc == 2  && !numeric_arg (argv[0], ARG (1), &exit_code))
    exit_code = 0;

  exit (exit_code);
}

/*-------------------------------------------------------------------------.
| Save the argument text until EOF has been seen, allowing for user	   |
| specified cleanup action.  GNU version saves all arguments, the standard |
| version only the first.						   |
`-------------------------------------------------------------------------*/

static void
m4_m4wrap (struct obstack *obs, int argc, token_data **argv)
{
  if (no_gnu_extensions)
    obstack_grow (obs, ARG (1), strlen (ARG (1)));
  else
    dump_args (obs, argc, argv, " ", FALSE);
  obstack_1grow (obs, '\0');
  push_wrapup (obstack_finish (obs));
}

/* Enable tracing of all specified macros, or all, if none is specified.
   Tracing is disabled by default, when a macro is defined.  This can be
   overridden by the "t" debug flag.  */

/*-----------------------------------------------------------------------.
| Set_trace () is used by "traceon" and "traceoff" to enable and disable |
| tracing of a macro.  It disables tracing if DATA is NULL, otherwise it |
| enable tracing.							 |
`-----------------------------------------------------------------------*/

static void
set_trace (symbol *sym, char *data)
{
  SYMBOL_TRACED (sym) = (boolean) (data != NULL);
}

static void
m4_traceon (struct obstack *obs, int argc, token_data **argv)
{
  symbol *s;
  int i;

  if (argc == 1)
    hack_all_symbols (set_trace, (char *) obs);
  else
    for (i = 1; i < argc; i++)
      {
	s = lookup_symbol (TOKEN_DATA_TEXT (argv[i]), SYMBOL_LOOKUP);
	if (s != NULL)
	  set_trace (s, (char *) obs);
	else
	  m4error ("Undefined name %s", TOKEN_DATA_TEXT (argv[i]));
      }
}

/*------------------------------------------------------------------------.
| Disable tracing of all specified macros, or all, if none is specified.  |
`------------------------------------------------------------------------*/

static void
m4_traceoff (struct obstack *obs, int argc, token_data **argv)
{
  symbol *s;
  int i;

  if (argc == 1)
    hack_all_symbols (set_trace, NULL);
  else
    for (i = 1; i < argc; i++)
      {
	s = lookup_symbol (TOKEN_DATA_TEXT (argv[i]), SYMBOL_LOOKUP);
	if (s != NULL)
	  set_trace (s, NULL);
	else
	  m4error ("Undefined name %s", TOKEN_DATA_TEXT (argv[i]));
      }
}

/*----------------------------------------------------------------------.
| On-the-fly control of the format of the tracing output.  It takes one |
| argument, which is a character string like given to the -d option, or |
| none in which case the debug_level is zeroed.			        |
`----------------------------------------------------------------------*/

static void
m4_debugmode (struct obstack *obs, int argc, token_data **argv)
{
  int new_debug_level;
  int change_flag;

  if (bad_argc (argv[0], argc, 1, 2))
    return;

  if (argc == 1)
    debug_level = 0;
  else
    {
      if (ARG (1)[0] == '+' || ARG (1)[0] == '-')
	{
	  change_flag = ARG (1)[0];
	  new_debug_level = debug_decode (ARG (1) + 1);
	}
      else
	{
	  change_flag = 0;
	  new_debug_level = debug_decode (ARG (1));
	}

      if (new_debug_level < 0)
	m4error ("debugmode: bad debug flags: `%s'", ARG (1));
      else
	{
	  switch (change_flag)
	    {
	    case 0:
	      debug_level = new_debug_level;
	      break;
	    case '+':
	      debug_level |= new_debug_level;
	      break;
	    case '-':
	      debug_level &= ~new_debug_level;
	      break;
	    }
	}
    }
}

/*-------------------------------------------------------------------------.
| Specify the destination of the debugging output.  With one argument, the |
| argument is taken as a file name, with no arguments, revert to stderr.   |
`-------------------------------------------------------------------------*/

static void
m4_debugfile (struct obstack *obs, int argc, token_data **argv)
{
  if (bad_argc (argv[0], argc, 1, 2))
    return;

  if (argc == 1)
    debug_set_output (NULL);
  else if (!debug_set_output (ARG (1)))
    m4error ("cannot set error file: %s: %s", ARG (1), syserr ());
}

/* This section contains text processing macros: "len", "index",
   "substr", "translit", "format", "regexp" and "patsubst".  The last
   three are GNU specific.  */

/*---------------------------------------------.
| Expand to the length of the first argument.  |
`---------------------------------------------*/

static void
m4_len (struct obstack *obs, int argc, token_data **argv)
{
  if (bad_argc (argv[0], argc, 2, 2))
    return;
  shipout_int (obs, strlen (ARG (1)));
}

/*-------------------------------------------------------------------------.
| The macro expands to the first index of the second argument in the first |
| argument.								   |
`-------------------------------------------------------------------------*/

static void
m4_index (struct obstack *obs, int argc, token_data **argv)
{
  char *cp, *last;
  int l1, l2, retval;

  if (bad_argc (argv[0], argc, 3, 3))
    return;

  l1 = strlen (ARG (1));
  l2 = strlen (ARG (2));

  last = ARG (1) + l1 - l2;

  for (cp = ARG (1); cp <= last; cp++)
    {
      if (strncmp (cp, ARG (2), l2) == 0)
	break;
    }
  retval = (cp <= last) ? cp - ARG (1) : -1;

  shipout_int (obs, retval);
}

/*-------------------------------------------------------------------------.
| The macro "substr" extracts substrings from the first argument, starting |
| from the index given by the second argument, extending for a length	   |
| given by the third argument.  If the third argument is missing, the	   |
| substring extends to the end of the first argument.			   |
`-------------------------------------------------------------------------*/

static void
m4_substr (struct obstack *obs, int argc, token_data **argv)
{
  int start, length, avail;

  if (bad_argc (argv[0], argc, 3, 4))
    return;

  length = avail = strlen (ARG (1));
  if (!numeric_arg (argv[0], ARG (2), &start))
    return;

  if (argc == 4 && !numeric_arg (argv[0], ARG (3), &length))
    return;

  if (start < 0 || length <= 0 || start >= avail)
    return;

  if (start + length > avail)
    length = avail - start;
  obstack_grow (obs, ARG (1) + start, length);
}

/*------------------------------------------------------------------------.
| For "translit", ranges are allowed in the second and third argument.	  |
| They are expanded in the following function, and the expanded strings,  |
| without any ranges left, are used to translate the characters of the	  |
| first argument.  A single - (dash) can be included in the strings by	  |
| being the first or the last character in the string.  If the first	  |
| character in a range is after the first in the character set, the range |
| is made backwards, thus 9-0 is the string 9876543210.			  |
`------------------------------------------------------------------------*/

static char *
expand_ranges (char *s, struct obstack *obs)
{
  char from;
  char to;

  for (from = '\0'; *s != '\0'; from = *s++)
    {
      if (*s == '-' && from != '\0')
	{
	  to = *++s;
	  if (to == '\0')
	    obstack_1grow (obs, '-'); /* trailing dash */
	  else if (from <= to)
	    {
	      while (from++ < to)
		obstack_1grow (obs, from);
	    }
	  else
	    {
	      while (--from >= to)
		obstack_1grow (obs, from);
	    }
	}
      else
	obstack_1grow (obs, *s);
    }
  obstack_1grow (obs, '\0');
  return obstack_finish (obs);
}

/*----------------------------------------------------------------------.
| The macro "translit" translates all characters in the first argument, |
| which are present in the second argument, into the corresponding      |
| character from the third argument.  If the third argument is shorter  |
| than the second, the extra characters in the second argument, are     |
| deleted from the first (pueh).				        |
`----------------------------------------------------------------------*/

static void
m4_translit (struct obstack *obs, int argc, token_data **argv)
{
  register char *data, *tmp;
  char *from, *to;
  int tolen;

  if (bad_argc (argv[0], argc, 3, 4))
    return;

  from = ARG (2);
  if (index (from, '-') != NULL)
    {
      from = expand_ranges (from, obs);
      if (from == NULL)
	return;
    }

  if (argc == 4)
    {
      to = ARG (3);
      if (index (to, '-') != NULL)
	{
	  to = expand_ranges (to, obs);
	  if (to == NULL)
	    return;
	}
    }
  else
    to = "";

  tolen = strlen (to);

  for (data = ARG (1); *data; data++)
    {
      tmp = (char *) index (from, *data);
      if (tmp == NULL)
	{
	  obstack_1grow (obs, *data);
	}
      else
	{
	  if (tmp - from < tolen)
	    obstack_1grow (obs, *(to + (tmp - from)));
	}
    }
}

/*----------------------------------------------------------------------.
| Frontend for printf like formatting.  The function format () lives in |
| the file format.c.						        |
`----------------------------------------------------------------------*/

static void
m4_format (struct obstack *obs, int argc, token_data **argv)
{
  format (obs, argc - 1, argv + 1);
}

/*--------------------------------------------------------------------------.
| Function to perform substitution by regular expressions.  Used by the	    |
| builtins regexp and patsubst.  The changed text is place on the obstack.  |
| The substitution is REPL, with \& substituted by VICTIM (of length	    |
| LENGTH), and \N substituted by the text matched by the Nth parenthesized  |
| sub-expression, taken from REGS.					    |
`--------------------------------------------------------------------------*/

static void
substitute (struct obstack *obs, char *victim, int length,
	    char *repl, struct re_registers *regs)
{
  register unsigned int ch;

  for (;;)
    {
      while ((ch = *repl++) != '\\')
	{
	  if (ch == '\0')
	    return;
	  obstack_1grow (obs, ch);
	}

      switch ((ch = *repl++))
	{
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	  ch -= '0';
	  if (regs->end[ch] > 0)
	    obstack_grow (obs, victim + regs->start[ch],
			  regs->end[ch] - regs->start[ch]);
	  break;
	case '&':
	  obstack_grow (obs, victim, length);
	  break;
	default:
	  obstack_1grow (obs, ch);
	  break;
	}
    }
}

/*--------------------------------------------------------------------------.
| Regular expression version of index.  Given two arguments, expand to the  |
| index of the first match of the second argument (a regexp) in the first.  |
| Expand to -1 if here is no match.  Given a third argument, is changes	    |
| the expansion to this argument.					    |
`--------------------------------------------------------------------------*/

static void
m4_regexp (struct obstack *obs, int argc, token_data **argv)
{
  char *victim;			/* first argument */
  char *regexp;			/* regular expression */
  char *repl;			/* replacement string */

  struct re_pattern_buffer buf;	/* compiled regular expression */
  struct re_registers regs;	/* for subexpression matches */
  const char *msg;		/* error message from re_compile_pattern */
  int startpos;			/* start position of match */
  int length;			/* length of first argument */

  if (bad_argc (argv[0], argc, 3, 4))
    return;

  victim = TOKEN_DATA_TEXT (argv[1]);
  regexp = TOKEN_DATA_TEXT (argv[2]);
  if (argc == 4)
    repl = TOKEN_DATA_TEXT (argv[3]);

  buf.buffer = NULL;
  buf.allocated = 0;
  buf.fastmap = NULL;
  buf.translate = NULL;
  msg = re_compile_pattern (regexp, strlen (regexp), &buf);

  if (msg != NULL)
    {
      m4error ("bad regular expression: \"%s\": %s", regexp, msg);
      return;
    }

  length = strlen (victim);
  startpos = re_search (&buf, victim, length, 0, length, &regs);
  xfree (buf.buffer);

  if (startpos  == -2)
    {
      m4error ("error matching regular expression \"%s\"", regexp);
      return;
    }

  if (argc == 3)
    shipout_int (obs, startpos);
  else if (startpos >= 0)
    substitute (obs, victim, length, repl, &regs);

  return;
}

/*--------------------------------------------------------------------------.
| Substitute all matches of a regexp occuring in a string.  Each match of   |
| the second argument (a regexp) in the first argument is changed to the    |
| third argument, with \& substituted by the matched text, and \N	    |
| substituted by the text matched by the Nth parenthesized sub-expression.  |
`--------------------------------------------------------------------------*/

static void
m4_patsubst (struct obstack *obs, int argc, token_data **argv)
{
  char *victim;			/* first argument */
  char *regexp;			/* regular expression */

  struct re_pattern_buffer buf;	/* compiled regular expression */
  struct re_registers regs;	/* for subexpression matches */
  const char *msg;		/* error message from re_compile_pattern */
  int matchpos;			/* start position of match */
  int offset;			/* current match offset */
  int length;			/* length of first argument */

  if (bad_argc (argv[0], argc, 3, 4))
    return;

  regexp = TOKEN_DATA_TEXT (argv[2]);

  buf.buffer = NULL;
  buf.allocated = 0;
  buf.fastmap = NULL;
  buf.translate = NULL;
  msg = re_compile_pattern (regexp, strlen (regexp), &buf);

  if (msg != NULL)
    {
      m4error ("bad regular expression: \"%s\": %s", regexp, msg);
      if (buf.buffer != NULL)
	xfree (buf.buffer);
      return;
    }

  victim = TOKEN_DATA_TEXT (argv[1]);
  length = strlen (victim);

  offset = 0;
  matchpos = 0;
  while (offset < length)
    {
      matchpos = re_search (&buf, victim, length,
			    offset, length - offset, &regs);
      if (matchpos < 0)
	{
	  /*
	   * Match failed -- either error or there is no match in the
	   * rest of the string, in which case the rest of the string
	   * is copied verbatim.
	   */
	  if (matchpos == -2)
	    m4error ("error matching regular expression \"%s\"", regexp);
	  else if (offset < length)
	    obstack_grow (obs, victim + offset, length - offset);
	  break;
	}
      /*
       * Copy the part of the string that was skipped by re_search ().
       */
      if (matchpos > offset)
	obstack_grow (obs, victim + offset, matchpos - offset);

      /*
       * Handle the part of the string that was covered by the match.
       */
      substitute (obs, victim, length, ARG (3), &regs);

      /*
       * Update the offset to the end of the match.  If the regexp
       * matched a null string, advance offset one more, to avoid
       * infinite loops.
       */
      offset = regs.end[0];
      if (regs.start[0] == regs.end[0])
	obstack_1grow (obs, victim[offset++]);
    }
  obstack_1grow (obs, '\0');

  xfree (buf.buffer);
  return;
}


/*-------------------------------------------------------------------------.
| This function handles all expansion of user defined and predefined	   |
| macros.  It is called with an obstack OBS, where the macros expansion	   |
| will be placed, as an unfinished object.  SYM points to the macro	   |
| definition, giving the expansion text.  ARGC and ARGV are the arguments, |
| as usual.								   |
`-------------------------------------------------------------------------*/

void
expand_user_macro (struct obstack *obs, symbol *sym,
		   int argc, token_data **argv)
{
  register char *text;
  int i;

  for (text = SYMBOL_TEXT (sym); *text != '\0';)
    {
      if (*text != '$')
	{
	  obstack_1grow (obs, *text);
	  text++;
	  continue;
	}
      text++;
      switch (*text)
	{
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	  if (no_gnu_extensions)
	    {
	      i = *text++ - '0';
	    }
	  else
	    {
	      for (i = 0; isdigit (*text); text++)
		i = i*10 + (*text - '0');
	    }
	  if (i < argc)
	    obstack_grow (obs, TOKEN_DATA_TEXT (argv[i]),
			  strlen (TOKEN_DATA_TEXT (argv[i])));
	  break;

	case '#':		/* number of arguments */
	  shipout_int (obs, argc - 1);
	  text++;
	  break;

	case '*':		/* all arguments */
	case '@':		/* ... same, but quoted */
	  dump_args (obs, argc, argv, ",", *text == '@');
	  text++;
	  break;

	default:
	  obstack_1grow (obs, '$');
	  break;
	}
    }
}
