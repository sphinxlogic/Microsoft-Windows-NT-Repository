%{
/* cmi.y: parse a character metric information file, entering what we
   find into the symbol table.

Copyright (C) 1992 Free Software Foundation, Inc.

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

#include "config.h"

#include <ctype.h>
#include "libfile.h"
#include "varstring.h"

#include "input-cmi.h"
#include "main.h"
#include "symtab.h"


static void yyerror (string);
static int yylex (void);
%}

%union
{
  real realval;
  symval_type symval;
  string strval;
}

%token TR_CHAR TR_CHAR_WIDTH TR_CODINGSCHEME TR_DEFINE TR_FONTDIMEN TR_KERN
%token <strval> T_IDENTIFIER T_STRING
%token <realval> T_REAL

/* A <symval> is not a symbol, it's the value of a symbol.  */
%type <symval> expr

%%
statements:
	  /* empty */
	| statements statement
	;

statement:
	  char
        | char_width
	| codingscheme
        | define
	| fontdimen
        | kern
        ;

/* The two expr's are the left and right side bearings.  We have to
   preserve any existing kerns.  */
char:
	  TR_CHAR T_IDENTIFIER expr ',' expr
	    { symval_type *old_sv = symtab_lookup ($2);
              symval_type new_sv = symtab_char_node ($3, $5);

	      if (old_sv != NULL && SYMVAL_TAG (*old_sv) == symval_char)
                CHAR_KERNS (SYMVAL_CHAR (new_sv))
	          = CHAR_KERNS (SYMVAL_CHAR (*old_sv));

	      symtab_define ($2, new_sv);
	    }
	;

/* The two expr's here are the set width and the percentage of
   (set width - bitmap width) to make the left side bearing.  We cheat
   and use the rsb to store the width expression in the interest of not
   complicating the `char_type' structure even more.  We use the symbol
   tag to distinguish the different ways of defining characters.  */
char_width:
	  TR_CHAR_WIDTH T_IDENTIFIER expr ',' expr
            { symval_type *old_sv = symtab_lookup ($2);
              symval_type new_sv = symtab_char_node ($5, $3);
              SYMVAL_TAG (new_sv) = symval_char_width;

	      if (old_sv != NULL && SYMVAL_TAG (*old_sv) == symval_char)
                CHAR_KERNS (SYMVAL_CHAR (new_sv))
	          = CHAR_KERNS (SYMVAL_CHAR (*old_sv));

	      symtab_define ($2, new_sv);
	    }
        ;

codingscheme:
	  TR_CODINGSCHEME T_STRING
            { symtab_define ("codingscheme", symtab_string_node ($2)); }
	;

define:
	  TR_DEFINE T_IDENTIFIER expr
            { symtab_define ($2, $3); }
	;

fontdimen:
	  TR_FONTDIMEN T_IDENTIFIER expr
            { if (tfm_fontdimen_number ($2) == 0)
                WARNING1 ("%s: Not the name of a fontdimen", $2);
               
              symtab_define ($2, $3);
            }
	;

kern:
	  TR_KERN T_IDENTIFIER T_IDENTIFIER expr
            { symtab_define_kern ($2, $3, $4); }
	;

expr:
	  T_REAL
            { $$ = symtab_real_node ($1); }
	| T_IDENTIFIER
	    { $$ = symtab_string_node ($1); }
        | T_REAL T_IDENTIFIER
            { $$ = symtab_real_string_node ($1, $2); }
	;
%%

/* Let's hope Bison will never use this value as a token code.  */
#define NOT_A_TOKEN (-1)

/* Keep track of line numbers for error messages.  */
static unsigned lineno = 1;

static int get_numeric_token (FILE *);
static int get_word_token (FILE *);
static int get_string_token (FILE *);
static int token_number (string);


/* Return the next token from `cmi_file'.  */

static int
yylex (void)
{
  int t = NOT_A_TOKEN;
  
  do
    {
      int c = getc (cmi_file);

      /* Skip whitespace.  */
      while (c != EOF && isspace (c))
        {
          if (c == '\n') lineno++;
          c = getc (cmi_file);
	}
        
      switch (c)
        {
        case EOF: 
          t = 0;
          lineno = 1; /* For the next file.  */
          break;

        /* Comments begin with % and go to the end of the line.  */
        case '%': 
          while ((c = getc (cmi_file)) != '\n' && c != EOF)
            ;
          if (c == '\n') lineno++;
          break;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '.': case '-': case '+':
          ungetc (c, cmi_file);
          t = get_numeric_token (cmi_file);
          break;

        case '"':
          t = get_string_token (cmi_file);
          break;

        case ',':
          t = c;
          break;

        default:
          ungetc (c, cmi_file);
          t = get_word_token (cmi_file);
          break;
        }
    }
  while (t == NOT_A_TOKEN);

  return t;
}


/* Assume we've opened the file with `libfile_start'.  We've been
   keeping track of the line numbers ourselves, though.  */

static void
yyerror (string s)
{
  fprintf (stderr, "%s:%u: %s\n", libfilename (), lineno, s);
}

/* Read a numeric decimal constant from the file F.  Always return T_REAL
   and set yylval.realval.  */

static int
get_numeric_token (FILE *f)
{
  variable_string vs = vs_init ();
  int c = getc (f);
  
  /* Look for a sign.  */
  if (c == '+' || c == '-')
    {
      vs_append_char (&vs, c);
      c = getc (f);
    }

  /* Collect leading digits.  */
  while (isdigit (c))
    {
      vs_append_char (&vs, c);
      c = getc (f);
    }
  
  /* If we're at a decimal point, keep going.  */
  if (c == '.')
    {
      /* Collect more digits.  Append first, so we grab the `.'.  */
      do
        {
          vs_append_char (&vs, c);
          c = getc (f);
        }
      while (isdigit (c));
      
      /* Don't bother to allow `e<exponent>' at the end.  */
    }
  
  /* Add the terminating null and convert the string to a number.  */
  vs_append_char (&vs, 0);
  
  yylval.realval = atof (VS_CHARS (vs));
  vs_free (&vs);
  
  return T_REAL;
}

/* Read a string constant from the file F.  We make no provision for
   quoting " characters; the string just consists of everything from one
   " to the next.  We return T_STRING and set yylval.strval.  */

static int
get_string_token (FILE *f)
{
  int c;
  variable_string vs = vs_init ();

  while ((c = getc (f)) != EOF && c != '"')
    vs_append_char (&vs, c);

  if (c == EOF)
    FATAL ("CMI file ended in the midst of a string");

  /* Add the terminating null.  */
  vs_append_char (&vs, 0);

  yylval.strval = VS_CHARS (vs);
  
  return T_STRING;
}

/* Return the Bison-defined value for the token equal to the
   next (space-delimited) identifier in the file F.  That is, if it's a
   reserved word return the corresponding value TR_..., else return
   T_IDENTIFIER.  */
   
static int
get_word_token (FILE *f)
{
  int c, t;
  variable_string word = vs_init ();

  while ((c = getc (f)) != EOF && !isspace (c))
    vs_append_char (&word, c);
  
  if (c != EOF)
    ungetc (c, f);

  /* Add the terminating null.  */
  vs_append_char (&word, 0);

  t = token_number (VS_CHARS (word));
  
  /* We don't need the characters in the identifier again.  */
  vs_free (&word);
  
  return t;
}


/* Return the token code corresponding to the word STR.  If STR is not
   one of the reserved words, return T_IDENTIFIER.  */

static int
token_number (string word)
{
  int t;
  
  if (STREQ (word, "char"))
    t = TR_CHAR;
  else if (STREQ (word, "char-width"))
    t = TR_CHAR_WIDTH;
  else if (STREQ (word, "codingscheme"))
    t = TR_CODINGSCHEME;
  else if (STREQ (word, "define"))
    t = TR_DEFINE;
  else if (STREQ (word, "fontdimen"))
    t = TR_FONTDIMEN;
  else if (STREQ (word, "kern"))
    t = TR_KERN;
  else
    {
      t = T_IDENTIFIER;
      yylval.strval = xstrdup (word);
    }

  return t;
}
