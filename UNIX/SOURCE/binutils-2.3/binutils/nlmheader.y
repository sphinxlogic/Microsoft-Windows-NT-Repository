%{/* nlmheader.y - parse NLM header specification keywords.
     Copyright (C) 1993 Free Software Foundation, Inc.

This file is part of GNU Binutils.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Written by Ian Lance Taylor <ian@cygnus.com>.

   This bison file parses the commands recognized by the NetWare NLM
   linker, except for lists of object files.  It stores the
   information in global variables.

   This implementation is based on the description in the NetWare Tool
   Maker Specification manual, edition 1.0.  */

#include <ansidecl.h>
#include <stdio.h>
#include <ctype.h>
#include <bfd.h>
#include "sysdep.h"
#include "bucomm.h"
#include "nlm/common.h"
#include "nlm/internal.h"
#include "nlmconv.h"

/* Information is stored in the structures pointed to by these
   variables.  */

Nlm_Internal_Fixed_Header *fixed_hdr;
Nlm_Internal_Variable_Header *var_hdr;
Nlm_Internal_Version_Header *version_hdr;
Nlm_Internal_Copyright_Header *copyright_hdr;
Nlm_Internal_Extended_Header *extended_hdr;

/* Procedure named by CHECK.  */
char *check_procedure;
/* File named by CUSTOM.  */
char *custom_file;
/* Whether to generate debugging information (DEBUG).  */
boolean debug_info;
/* Procedure named by EXIT.  */
char *exit_procedure;
/* Exported symbols (EXPORT).  */
struct string_list *export_symbols;
/* Map file name (MAP, FULLMAP).  */
char *map_file;
/* Whether a full map has been requested (FULLMAP).  */
boolean full_map;
/* File named by HELP.  */
char *help_file;
/* Imported symbols (IMPORT).  */
struct string_list *import_symbols;
/* File named by MESSAGES.  */
char *message_file;
/* Autoload module list (MODULE).  */
struct string_list *modules;
/* File named by SHARELIB.  */
char *sharelib_file;
/* Start procedure name (START).  */
char *start_procedure;
/* VERBOSE.  */
boolean verbose;
/* RPC description file (XDCDATA).  */
char *rpc_file;

/* The number of serious errors that have occurred.  */
int parse_errors;

/* The current symbol prefix when reading a list of import or export
   symbols.  */
static char *symbol_prefix;

/* Parser error message handler.  */
#define yyerror(msg) nlmheader_error (msg);

/* Local functions.  */
static int yylex PARAMS ((void));
static void nlmlex_file_push PARAMS ((const char *));
static boolean nlmlex_file_open PARAMS ((const char *));
static int nlmlex_buf_init PARAMS ((void));
static char nlmlex_buf_add PARAMS ((int));
static long nlmlex_get_number PARAMS ((const char *));
static void nlmheader_warn PARAMS ((const char *, int));
static void nlmheader_error PARAMS ((const char *));
static struct string_list * string_list_cons PARAMS ((char *,
						      struct string_list *));
static struct string_list * string_list_append PARAMS ((struct string_list *,
							struct string_list *));
static struct string_list * string_list_append1 PARAMS ((struct string_list *,
							 char *));
static char *xstrdup PARAMS ((const char *));

%}

%union
{
  char *string;
  struct string_list *list;
};

/* The reserved words.  */

%token CHECK CODESTART COPYRIGHT CUSTOM DATE DEBUG DESCRIPTION EXIT
%token EXPORT FLAG_ON FLAG_OFF FULLMAP HELP IMPORT INPUT MAP MESSAGES
%token MODULE MULTIPLE OS_DOMAIN OUTPUT PSEUDOPREEMPTION REENTRANT
%token SCREENNAME SHARELIB STACK STACKSIZE START SYNCHRONIZE
%token THREADNAME TYPE VERBOSE VERSION XDCDATA

/* Arguments.  */

%token <string> STRING
%token <string> QUOTED_STRING

/* Typed non-terminals.  */
%type <list> symbol_list_opt symbol_list module_list
%type <string> symbol

%%

/* Keywords must start in the leftmost column of the file.  Arguments
   may appear anywhere else.  The lexer uses this to determine what
   token to return, so we don't have to worry about it here.  */

/* The entire file is just a list of commands.  */

file:	
	  commands
	;

/* A possibly empty list of commands.  */

commands:
	  /* May be empty.  */
	| command commands
	;

/* A single command.  There is where most of the work takes place.  */

command:
	  CHECK STRING
	  {
	    check_procedure = $2;
	  }
	| CODESTART STRING
	  {
	    nlmheader_warn ("CODESTART is not implemented; sorry", -1);
	    free ($2);
	  }
	| COPYRIGHT QUOTED_STRING
	  {
	    int len;

	    strncpy (copyright_hdr->stamp, "CoPyRiGhT=", 10);
	    len = strlen ($2);
	    if (len >= NLM_MAX_COPYRIGHT_MESSAGE_LENGTH)
	      {
		nlmheader_warn ("copyright string is too long",
				NLM_MAX_COPYRIGHT_MESSAGE_LENGTH - 1);
		len = NLM_MAX_COPYRIGHT_MESSAGE_LENGTH - 1;
	      }
	    copyright_hdr->copyrightMessageLength = len;
	    strncpy (copyright_hdr->copyrightMessage, $2, len);
	    copyright_hdr->copyrightMessage[len] = '\0';
	    free ($2);
	  }
	| CUSTOM STRING
	  {
	    custom_file = $2;
	  }
	| DATE STRING STRING STRING
	  {
	    /* We don't set the version stamp here, because we use the
	       version stamp to detect whether the required VERSION
	       keyword was given.  */
	    version_hdr->month = nlmlex_get_number ($2);
	    version_hdr->day = nlmlex_get_number ($3);
	    version_hdr->year = nlmlex_get_number ($4);
	    free ($2);
	    free ($3);
	    free ($4);
	  }
	| DEBUG
	  {
	    debug_info = true;
	  }
	| DESCRIPTION QUOTED_STRING
	  {
	    int len;

	    len = strlen ($2);
	    if (len > NLM_MAX_DESCRIPTION_LENGTH)
	      {
		nlmheader_warn ("description string is too long",
				NLM_MAX_DESCRIPTION_LENGTH);
		len = NLM_MAX_DESCRIPTION_LENGTH;
	      }
	    var_hdr->descriptionLength = len;
	    strncpy (var_hdr->descriptionText, $2, len);
	    var_hdr->descriptionText[len] = '\0';
	    free ($2);
	  }
	| EXIT STRING
	  {
	    exit_procedure = $2;
	  }
	| EXPORT
	  {
	    symbol_prefix = NULL;
	  }
	  symbol_list_opt
	  {
	    export_symbols = string_list_append (export_symbols, $3);
	  }
	| FLAG_ON STRING
	  {
	    fixed_hdr->flags |= nlmlex_get_number ($2);
	    free ($2);
	  }
	| FLAG_OFF STRING
	  {
	    fixed_hdr->flags &=~ nlmlex_get_number ($2);
	    free ($2);
	  }
	| FULLMAP STRING
	  {
	    map_file = $2;
	    full_map = true;
	  }
	| HELP STRING
	  {
	    help_file = $2;
	  }
	| IMPORT
	  {
	    symbol_prefix = NULL;
	  }
	  symbol_list_opt
	  {
	    import_symbols = string_list_append (import_symbols, $3);
	  }
	| INPUT STRING
	  {
	    nlmheader_warn ("INPUT not supported", -1);
	    free ($2);
	  }
	| MAP STRING
	  {
	    map_file = $2;
	  }
	| MESSAGES STRING
	  {
	    message_file = $2;
	  }
	| MODULE module_list
	  {
	    modules = string_list_append (modules, $2);
	  }
	| MULTIPLE
	  {
	    fixed_hdr->flags |= 0x2;
	  }
	| OS_DOMAIN
	  {
	    fixed_hdr->flags |= 0x10;
	  }
	| OUTPUT STRING
	  {
	    nlmheader_warn ("OUTPUT not supported", -1);
	    free ($2);
	  }
	| PSEUDOPREEMPTION
	  {
	    fixed_hdr->flags |= 0x8;
	  }
	| REENTRANT
	  {
	    fixed_hdr->flags |= 0x1;
	  }
	| SCREENNAME QUOTED_STRING
	  {
	    int len;

	    len = strlen ($2);
	    if (len >= NLM_MAX_SCREEN_NAME_LENGTH)
	      {
		nlmheader_warn ("screen name is too long",
				NLM_MAX_SCREEN_NAME_LENGTH);
		len = NLM_MAX_SCREEN_NAME_LENGTH;
	      }
	    var_hdr->screenNameLength = len;
	    strncpy (var_hdr->screenName, $2, len);
	    var_hdr->screenName[NLM_MAX_SCREEN_NAME_LENGTH] = '\0';
	    free ($2);
	  }
	| SHARELIB STRING
	  {
	    sharelib_file = $2;
	  }
	| STACK STRING
	  {
	    var_hdr->stackSize = nlmlex_get_number ($2);
	    free ($2);
	  }
	| STACKSIZE STRING
	  {
	    var_hdr->stackSize = nlmlex_get_number ($2);
	    free ($2);
	  }
	| START STRING
	  {
	    start_procedure = $2;
	  }
	| SYNCHRONIZE
	  {
	    fixed_hdr->flags |= 0x4;
	  }
	| THREADNAME QUOTED_STRING
	  {
	    int len;

	    len = strlen ($2);
	    if (len >= NLM_MAX_THREAD_NAME_LENGTH)
	      {
		nlmheader_warn ("thread name is too long",
				NLM_MAX_THREAD_NAME_LENGTH);
		len = NLM_MAX_THREAD_NAME_LENGTH;
	      }
	    var_hdr->threadNameLength = len;
	    strncpy (var_hdr->threadName, $2, len);
	    var_hdr->screenName[NLM_MAX_THREAD_NAME_LENGTH] = '\0';
	    free ($2);
	  }
	| TYPE STRING
	  {
	    fixed_hdr->moduleType = nlmlex_get_number ($2);
	    free ($2);
	  }
	| VERBOSE
	  {
	    verbose = true;
	  }
	| VERSION STRING STRING STRING
	  {
	    long val;

	    strncpy (version_hdr->stamp, "VeRsIoN#", 8);
	    version_hdr->majorVersion = nlmlex_get_number ($2);
	    val = nlmlex_get_number ($3);
	    if (val < 0 || val > 99)
	      nlmheader_warn ("illegal minor version number (must be between 0 and 99)",
			      -1);
	    else
	      version_hdr->minorVersion = val;
	    val = nlmlex_get_number ($4);
	    if (val < 1 || val > 26)
	      nlmheader_warn ("illegal revision number (must be between 1 and 26)",
			      -1);
	    else
	      version_hdr->revision = val;
	    free ($2);
	    free ($3);
	    free ($4);
	  }
	| VERSION STRING STRING
	  {
	    long val;

	    strncpy (version_hdr->stamp, "VeRsIoN#", 8);
	    version_hdr->majorVersion = nlmlex_get_number ($2);
	    val = nlmlex_get_number ($3);
	    if (val < 0 || val > 99)
	      nlmheader_warn ("illegal minor version number (must be between 0 and 99)",
			      -1);
	    else
	      version_hdr->minorVersion = val;
	    version_hdr->revision = 0;
	    free ($2);
	    free ($3);
	  }
	| XDCDATA STRING
	  {
	    rpc_file = $2;
	  }
	;

/* A possibly empty list of symbols.  */

symbol_list_opt:
	  /* Empty.  */
	  {
	    $$ = NULL;
	  }
	| symbol_list
	  {
	    $$ = $1;
	  }
	;

/* A list of symbols in an import or export list.  Prefixes may appear
   in parentheses.  We need to use left recursion here to avoid
   building up a large import list on the parser stack.  */

symbol_list:
	  symbol
	  {
	    $$ = string_list_cons ($1, NULL);
	  }
	| symbol_prefix
	  {
	    $$ = NULL;
	  }
	| symbol_list symbol
	  {
	    $$ = string_list_append1 ($1, $2);
	  }
	| symbol_list symbol_prefix
	  {
	    $$ = $1;
	  }
	;

/* A prefix for subsequent symbols.  */

symbol_prefix:
	  '(' STRING ')'
	  {
	    if (symbol_prefix != NULL)
	      free (symbol_prefix);
	    symbol_prefix = $2;
	  }
	;

/* A single symbol.  */

symbol:
	  STRING
	  {
	    if (symbol_prefix == NULL)
	      $$ = $1;
	    else
	      {
		$$ = xmalloc (strlen (symbol_prefix) + strlen ($1) + 2);
		sprintf ($$, "%s@%s", symbol_prefix, $1);
		free ($1);
	      }
	  }
	;

/* A list of modules.  */

module_list:
	  /* May be empty.  */
	  {
	    $$ = NULL;
	  }
	| STRING module_list
	  {
	    $$ = string_list_cons ($1, $2);
	  }
	;

%%

/* If strerror is just a macro, we want to use the one from libiberty
   since it will handle undefined values.  */
#undef strerror
extern char *strerror ();

/* The lexer is simple, too simple for flex.  Keywords are only
   recognized at the start of lines.  Everything else must be an
   argument.  A comma is treated as whitespace.  */

/* The states the lexer can be in.  */

enum lex_state
{
  /* At the beginning of a line.  */
  BEGINNING_OF_LINE,
  /* In the middle of a line.  */
  IN_LINE
};

/* We need to keep a stack of files to handle file inclusion.  */

struct input
{
  /* The file to read from.  */
  FILE *file;
  /* The name of the file.  */
  char *name;
  /* The current line number.  */
  int lineno;
  /* The current state.  */
  enum lex_state state;
  /* The next file on the stack.  */
  struct input *next;
};

/* The current input file.  */

static struct input current;

/* The character which introduces comments.  */
#define COMMENT_CHAR '#'

/* Start the lexer going on the main input file.  */

boolean
nlmlex_file (name)
     const char *name;
{
  current.next = NULL;
  return nlmlex_file_open (name);
}

/* Start the lexer going on a subsidiary input file.  */

static void
nlmlex_file_push (name)
     const char *name;
{
  struct input *push;

  push = (struct input *) xmalloc (sizeof (struct input));
  *push = current;
  if (nlmlex_file_open (name))
    current.next = push;
  else
    {
      current = *push;
      free (push);
    }
}

/* Start lexing from a file.  */

static boolean
nlmlex_file_open (name)
     const char *name;
{
  current.file = fopen (name, "r");
  if (current.file == NULL)
    {
      fprintf (stderr, "%s:%s: %s\n", program_name, name, strerror (errno));
      ++parse_errors;
      return false;
    }
  current.name = xstrdup (name);
  current.lineno = 1;
  current.state = BEGINNING_OF_LINE;
  return true;
}

/* Table used to turn keywords into tokens.  */

struct keyword_tokens_struct
{
  const char *keyword;
  int token;
};

struct keyword_tokens_struct keyword_tokens[] =
{
  { "CHECK", CHECK },
  { "CODESTART", CODESTART },
  { "COPYRIGHT", COPYRIGHT },
  { "CUSTOM", CUSTOM },
  { "DATE", DATE },
  { "DEBUG", DEBUG },
  { "DESCRIPTION", DESCRIPTION },
  { "EXIT", EXIT },
  { "EXPORT", EXPORT },
  { "FLAG_ON", FLAG_ON },
  { "FLAG_OFF", FLAG_OFF },
  { "FULLMAP", FULLMAP },
  { "HELP", HELP },
  { "IMPORT", IMPORT },
  { "INPUT", INPUT },
  { "MAP", MAP },
  { "MESSAGES", MESSAGES },
  { "MODULE", MODULE },
  { "MULTIPLE", MULTIPLE },
  { "OS_DOMAIN", OS_DOMAIN },
  { "OUTPUT", OUTPUT },
  { "PSEUDOPREEMPTION", PSEUDOPREEMPTION },
  { "REENTRANT", REENTRANT },
  { "SCREENNAME", SCREENNAME },
  { "SHARELIB", SHARELIB },
  { "STACK", STACK },
  { "STACKSIZE", STACKSIZE },
  { "START", START },
  { "SYNCHRONIZE", SYNCHRONIZE },
  { "THREADNAME", THREADNAME },
  { "TYPE", TYPE },
  { "VERBOSE", VERBOSE },
  { "VERSION", VERSION },
  { "XDCDATA", XDCDATA }
};

#define KEYWORD_COUNT (sizeof (keyword_tokens) / sizeof (keyword_tokens[0]))

/* The lexer accumulates strings in these variables.  */
static char *lex_buf;
static int lex_size;
static int lex_pos;

/* Start accumulating strings into the buffer.  */
#define BUF_INIT() \
  ((void) (lex_buf != NULL ? lex_pos = 0 : nlmlex_buf_init ()))

static int
nlmlex_buf_init ()
{
  lex_size = 10;
  lex_buf = xmalloc (lex_size + 1);
  lex_pos = 0;
  return 0;
}

/* Finish a string in the buffer.  */
#define BUF_FINISH() ((void) (lex_buf[lex_pos] = '\0'))

/* Accumulate a character into the buffer.  */
#define BUF_ADD(c) \
  ((void) (lex_pos < lex_size \
	   ? lex_buf[lex_pos++] = (c) \
	   : nlmlex_buf_add (c)))

static char
nlmlex_buf_add (c)
     int c;
{
  if (lex_pos >= lex_size)
    {
      lex_size *= 2;
      lex_buf = xrealloc (lex_buf, lex_size + 1);
    }

  return lex_buf[lex_pos++] = c;
}

/* The lexer proper.  This is called by the bison generated parsing
   code.  */

static int
yylex ()
{
  int c;

tail_recurse:

  c = getc (current.file);

  /* Commas are treated as whitespace characters.  */
  while (isspace ((unsigned char) c) || c == ',')
    {
      current.state = IN_LINE;
      if (c == '\n')
	{
	  ++current.lineno;
	  current.state = BEGINNING_OF_LINE;
	}
      c = getc (current.file);
    }

  /* At the end of the file we either pop to the previous file or
     finish up.  */
  if (c == EOF)
    {
      fclose (current.file);
      free (current.name);
      if (current.next == NULL)
	return 0;
      else
	{
	  struct input *next;

	  next = current.next;
	  current = *next;
	  free (next);
	  goto tail_recurse;
	}
    }

  /* A comment character always means to drop everything until the
     next newline.  */
  if (c == COMMENT_CHAR)
    {
      do
	{
	  c = getc (current.file);
	}
      while (c != '\n');
      ++current.lineno;
      current.state = BEGINNING_OF_LINE;
      goto tail_recurse;
    }

  /* An '@' introduces an include file.  */
  if (c == '@')
    {
      do
	{
	  c = getc (current.file);
	  if (c == '\n')
	    ++current.lineno;
	}
      while (isspace ((unsigned char) c));
      BUF_INIT ();
      while (! isspace ((unsigned char) c) && c != EOF)
	{
	  BUF_ADD (c);
	  c = getc (current.file);
	}
      BUF_FINISH ();

      ungetc (c, current.file);
      
      nlmlex_file_push (lex_buf);
      goto tail_recurse;
    }

  /* A non-space character at the start of a line must be the start of
     a keyword.  */
  if (current.state == BEGINNING_OF_LINE)
    {
      BUF_INIT ();
      while (isalnum ((unsigned char) c) || c == '_')
	{
	  if (islower ((unsigned char) c))
	    BUF_ADD (toupper ((unsigned char) c));
	  else
	    BUF_ADD (c);
	  c = getc (current.file);
	}
      BUF_FINISH ();

      if (c != EOF && ! isspace ((unsigned char) c) && c != ',')
	fprintf (stderr, "%s:%s:%d: illegal character in keyword: %c\n",
		 program_name, current.name, current.lineno, c);
      else
	{
	  int i;

	  for (i = 0; i < KEYWORD_COUNT; i++)
	    {
	      if (lex_buf[0] == keyword_tokens[i].keyword[0]
		  && strcmp (lex_buf, keyword_tokens[i].keyword) == 0)
		{
		  /* Pushing back the final whitespace avoids worrying
		     about \n here.  */
		  ungetc (c, current.file);
		  current.state = IN_LINE;
		  return keyword_tokens[i].token;
		}
	    }
	  
	  fprintf (stderr, "%s:%s:%d: unrecognized keyword: %s\n",
		   program_name, current.name, current.lineno, lex_buf);
	}

      ++parse_errors;
      /* Treat the rest of this line as a comment.  */
      ungetc (COMMENT_CHAR, current.file);
      goto tail_recurse;
    }

  /* Parentheses just represent themselves.  */
  if (c == '(' || c == ')')
    return c;

  /* Handle quoted strings.  */
  if (c == '"' || c == '\'')
    {
      int quote;
      int start_lineno;

      quote = c;
      start_lineno = current.lineno;

      c = getc (current.file);
      BUF_INIT ();
      while (c != quote && c != EOF)
	{
	  BUF_ADD (c);
	  if (c == '\n')
	    ++current.lineno;
	  c = getc (current.file);
	}
      BUF_FINISH ();

      if (c == EOF)
	{
	  fprintf (stderr, "%s:%s:%d: end of file in quoted string\n",
		   program_name, current.name, start_lineno);
	  ++parse_errors;
	}

      /* FIXME: Possible memory leak.  */
      yylval.string = xstrdup (lex_buf);
      return QUOTED_STRING;
    }

  /* Gather a generic argument.  */
  BUF_INIT ();
  while (! isspace (c)
	 && c != ','
	 && c != COMMENT_CHAR
	 && c != '('
	 && c != ')')
    {
      BUF_ADD (c);
      c = getc (current.file);
    }
  BUF_FINISH ();

  ungetc (c, current.file);

  /* FIXME: Possible memory leak.  */
  yylval.string = xstrdup (lex_buf);
  return STRING;
}

/* Get a number from a string.  */

static long
nlmlex_get_number (s)
     const char *s;
{
  long ret;
  char *send;

  ret = strtol (s, &send, 10);
  if (*send != '\0')
    nlmheader_warn ("bad number", -1);
  return ret;
}

/* Issue a warning.  */

static void
nlmheader_warn (s, imax)
     const char *s;
     int imax;
{
  fprintf (stderr, "%s:%s:%d: %s", program_name, current.name,
	   current.lineno, s);
  if (imax != -1)
    fprintf (stderr, " (max %d)", imax);
  fprintf (stderr, "\n");
}

/* Report an error.  */

static void
nlmheader_error (s)
     const char *s;
{
  nlmheader_warn (s, -1);
  ++parse_errors;
}

/* Add a string to a string list.  */

static struct string_list *
string_list_cons (s, l)
     char *s;
     struct string_list *l;
{
  struct string_list *ret;

  ret = (struct string_list *) xmalloc (sizeof (struct string_list));
  ret->next = l;
  ret->string = s;
  return ret;
}

/* Append a string list to another string list.  */

static struct string_list *
string_list_append (l1, l2)
     struct string_list *l1;
     struct string_list *l2;
{
  register struct string_list **pp;

  for (pp = &l1; *pp != NULL; pp = &(*pp)->next)
    ;
  *pp = l2;
  return l1;
}

/* Append a string to a string list.  */

static struct string_list *
string_list_append1 (l, s)
     struct string_list *l;
     char *s;
{
  struct string_list *n;
  register struct string_list **pp;

  n = (struct string_list *) xmalloc (sizeof (struct string_list));
  n->next = NULL;
  n->string = s;
  for (pp = &l; *pp != NULL; pp = &(*pp)->next)
    ;
  *pp = n;
  return l;
}

/* Duplicate a string in memory.  */

static char *
xstrdup (s)
     const char *s;
{
  unsigned long len;
  char *ret;

  len = strlen (s);
  ret = xmalloc (len + 1);
  strcpy (ret, s);
  return ret;
}
