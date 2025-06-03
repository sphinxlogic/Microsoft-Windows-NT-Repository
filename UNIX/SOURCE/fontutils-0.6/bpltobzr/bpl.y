%{
/* bpl.y: parse a Bezier property list file and output it in the binary
   BZR format.  We assume `bzr_open_output_file' has been called.

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
#include "bzr.h"
#include "spline.h"
#include "varstring.h"

#include "main.h"


/* We need to keep track of a current point for building the characters.  */
static real_coordinate_type cur;

static void yyerror (string);
static int yylex (void);
%}

%union
{
  real_bounding_box_type *bbval;
  int intval;
  spline_list_type *outlineval;		/* A single cyclic path.  */
  real realval;
  spline_list_array_type *shapeval;	/* A sequence of outlines.  */
  spline_type splineval;		/* A single line or cubic.  */
  string strval;
}
%token TR_BB TR_CHAR TR_DESIGNSIZE TR_FONTBB TR_FONTCOMMENT TR_FONTFILE
%token TR_LINE TR_NCHARS TR_OUTLINE TR_SPLINE TR_WIDTH
%token <intval> T_INTEGER
%token <realval> T_REAL T_REALSTR
%token <strval> T_STRING

%type <bbval> charbb
%type <outlineval> outline splines
%type <realval> designsize setwidth
%type <shapeval> outline_list shape
%type <splineval> spline
%type <strval> fontcomment

%%
bpl_file:
	preamble chardefs postamble ;

preamble:
	  fontfile fontcomment designsize
            { bzr_preamble_type pre;
              BZR_COMMENT (pre) = $2;
              BZR_DESIGN_SIZE (pre) = $3;
              bzr_put_preamble (pre);
            }
        ;

fontfile:
	TR_FONTFILE T_STRING ;

fontcomment:
          TR_FONTCOMMENT T_STRING
            { $$ = $2; }
        ;

designsize:
	  TR_DESIGNSIZE T_REAL
            { $$ = $2; }
        ;

chardefs:
	  chardef
        | chardefs chardef
        ;

chardef:
          TR_CHAR T_INTEGER setwidth charbb shape
            {
              bzr_char_type ch;

              if ($2 > MAX_CHARCODE)
                {
                  WARNING1 ("%d: Character code too large", $2);
                  $2 %= 256;
                }
              CHARCODE (ch) = $2;
              CHAR_SET_WIDTH (ch) = $3;
              CHAR_BB (ch) = *$4;
              BZR_SHAPE (ch) = *$5;
              bzr_put_char (ch);
            }
	;

setwidth:
  	  TR_WIDTH T_REALSTR
	    { $$ = $2; }
        ;

charbb:
	  TR_BB T_REALSTR T_REALSTR T_REALSTR T_REALSTR 
            { $$ = XTALLOC1 (real_bounding_box_type); 
              MIN_COL (*$$) = $2;
              MAX_COL (*$$) = $3;
              MIN_ROW (*$$) = $4;
              MAX_ROW (*$$) = $5;
            }
        ;

shape:
	  /* empty */
            { $$ = XTALLOC1 (spline_list_array_type);
              SPLINE_LIST_ARRAY_LENGTH (*$$) = 0;
              SPLINE_LIST_ARRAY_DATA (*$$) = NULL;
            }
        | outline_list
            { $$ = $1; }

outline_list:
	  outline
            { $$ = XTALLOC1 (spline_list_array_type);
              SPLINE_LIST_ARRAY_LENGTH (*$$) = 1;
              SPLINE_LIST_ARRAY_DATA (*$$) = XTALLOC (1, spline_list_type);
              SPLINE_LIST_ARRAY_ELT (*$$, 0) = *$1;
            }
	| outline_list outline
            { append_spline_list ($1, *$2);
              $$ = $1;
            }
        ;

outline:
          TR_OUTLINE T_REALSTR T_REALSTR
	    { cur = (real_coordinate_type) { $2, $3 }; }
          splines
            { $$ = $5; }
        ;

splines:
	  spline
            { $$ = XTALLOC1 (spline_list_type);
              SPLINE_LIST_LENGTH (*$$) = 1;
              SPLINE_LIST_DATA (*$$) = XTALLOC (1, spline_type);
              SPLINE_LIST_ELT (*$$, 0) = $1;
            }
        | splines spline
            { append_spline ($1, $2);
              $$ = $1;
            }
        ;

spline:
	  TR_LINE T_REALSTR T_REALSTR
	    { $$ = new_spline ();
              START_POINT ($$) = cur; 
              cur = (real_coordinate_type) { $2, $3 };
              END_POINT ($$) = cur;
              SPLINE_DEGREE ($$) = LINEAR;
            }
	| TR_SPLINE T_REALSTR T_REALSTR T_REALSTR T_REALSTR T_REALSTR T_REALSTR
            { $$ = new_spline ();
              START_POINT ($$) = cur;
              CONTROL1 ($$) = (real_coordinate_type) { $2, $3 };
              CONTROL2 ($$) = (real_coordinate_type) { $4, $5 };
              cur = (real_coordinate_type) { $6, $7 };
              END_POINT ($$) = cur;
              SPLINE_DEGREE ($$) = CUBIC;
            }
        ;

postamble:
	  fontbb nchars
	    { bzr_put_postamble (); }
        ;

fontbb:
	TR_FONTBB T_REALSTR T_REALSTR T_REALSTR T_REALSTR ;

nchars:
	TR_NCHARS T_INTEGER ;
%%

/* Let's hope Bison will never use this value as a token code.  */
#define NOT_A_TOKEN (-1)

static int get_numeric_token (FILE *);
static int get_word_token (FILE *);
static int get_string_token (FILE *);
static int token_number (string);


/* Return the next token from `bpl_file'.  */

static int
yylex (void)
{
  int t = NOT_A_TOKEN;
  
  do
    {
      int c = getc (bpl_file);

      /* Although the BPL file has parentheses in it to make it easy for
         Lisp to execute, there's no need to clutter up the grammar with
         parentheses everywhere.  The keywords do well enough to
         separate the various components.  */
      while (c != EOF && (isspace (c) || c == '(' || c == ')'))
        c = getc (bpl_file);
        
      switch (c)
        {
        case EOF: 
          t = 0;
          break;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '.':
          ungetc (c, bpl_file);
          t = get_numeric_token (bpl_file);
          break;

        case '"':
          t = get_string_token (bpl_file);
          break;

        default:
          if (isalpha (c))
            {
              ungetc (c, bpl_file);
              t = get_word_token (bpl_file);
            }
          else 
            t = c;
          break;
        }
    }
  while (t == NOT_A_TOKEN);

  return t;
}


/* Don't do anything special for errors.  */

static void
yyerror (string s)
{
  fprintf (stderr, "%s\n", s);
}

/* Read a numeric decimal constant from the file F.  If we see just
   <digits>, we return T_INTEGER and set yylval.intval.  If we see
   <digits>.<digits>, we return T_REAL and set yylval.realval.  This
   allows only unsigned numbers.  */

static int
get_numeric_token (FILE *f)
{
  int c;
  int t = T_INTEGER;
  variable_string vs = vs_init ();
  
  /* First collect leading digits.  */
  for (;;)
    {
      c = getc (f);
      if (!isdigit (c))
        break;
      vs_append_char (&vs, c);
    }
  
  /* If we're at a decimal point, this is a real constant.  */
  if (c == '.')
    {
      t = T_REAL;
      
      /* Collect more digits.  Append first, so we grab the `.'.  */
      do
        {
          vs_append_char (&vs, c);
          c = getc (f);
        }
      while (isdigit (c));
      
      /* Don't bother to allow `e<exponent>' at the end, since we never
         need to output in that format.  */
    }
  
  /* Add the terminating null and convert the string to a number.  */
  vs_append_char (&vs, 0);
  
  if (t == T_INTEGER)
    yylval.intval = atoi (VS_CHARS (vs));
  else
    yylval.realval = atof (VS_CHARS (vs));
  
  vs_free (&vs);
  
  return t;
}

/* Read a string constant from the file F. We make no provision for
   quoting " characters; the string just consists of everything from one
   " to the next.  Because Emacs 18 does not support floating-point
   numeric constants, bzrto writes such as a string.  Therefore, when we
   find a string whose contents are entirely such a constant, we return
   T_REALSTR and set yylval.realval.  Otherwise, we return T_STRING and
   set yylval.strval.  */

static int
get_string_token (FILE *f)
{
  int c, t;
  variable_string vs = vs_init ();

  while ((c = getc (f)) != EOF && c != '"')
    vs_append_char (&vs, c);

  if (c == EOF)
    FATAL ("BPL file ended in the midst of a string");

  /* Add the terminating null.  */
  vs_append_char (&vs, 0);

  /* Cheat and use `scanf' to see if this string is really a real.  */
  if (float_ok (VS_CHARS (vs)))
    {
      t = T_REALSTR;
      yylval.realval = atof (VS_CHARS (vs));
    }
  else
    {
      t = T_STRING;
      yylval.strval = VS_CHARS (vs);
    }
  
  return t;
}

/* Return the Bison-defined value for the token equal to the
   next nonalpha-delimited word in the file F, unless the word is
   ``comment''; in that case, ignore everything through the next `)' and
   return NOT_A_TOKEN.  */
   
static int
get_word_token (FILE *f)
{
  int c, t;
  variable_string word = vs_init ();

  while ((c = getc (f)) != EOF && isalpha (c))
    vs_append_char (&word, c);
  
  if (c != EOF)
    ungetc (c, f);

  /* Add the terminating null.  */
  vs_append_char (&word, 0);

  if (STREQ (VS_CHARS (word), "comment"))
    {
      while ((c = getc (f)) != EOF && c != ')')
        ;
      if (c == EOF)
        FATAL ("BPL file ended in the midst of a comment");
      t = NOT_A_TOKEN;
    }
  else
    t = token_number (VS_CHARS (word));
  
  /* We don't need the characters in the identifier again.  */
  vs_free (&word);
  
  return t;
}


/* Return the token code corresponding to the word STR.  If STR is not
   one of the reserved words, give an error message and return NOT_A_TOKEN:
   this is a language which doesn't have arbitrary user identifiers.  */

static int
token_number (string word)
{
  int t = NOT_A_TOKEN;
  
  if (STREQ (word, "bb"))
    return TR_BB;
  else if (STREQ (word, "char"))
    return TR_CHAR;
  else if (STREQ (word, "designsize"))
    return TR_DESIGNSIZE;
  else if (STREQ (word, "fontbb"))
    return TR_FONTBB;
  else if (STREQ (word, "fontcomment"))
    return TR_FONTCOMMENT;
  else if (STREQ (word, "fontfile"))
    return TR_FONTFILE;
  else if (STREQ (word, "line"))
    return TR_LINE;
  else if (STREQ (word, "nchars"))
    return TR_NCHARS;
  else if (STREQ (word, "outline"))
    return TR_OUTLINE;
  else if (STREQ (word, "spline"))
    return TR_SPLINE;
  else if (STREQ (word, "width"))
    return TR_WIDTH;

  return t;
}
