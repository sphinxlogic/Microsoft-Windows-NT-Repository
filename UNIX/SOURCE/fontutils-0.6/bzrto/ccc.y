%{
/* ccc.y -- Bison and C code to parse a CCC file.

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
#include "encoding.h"
#include "filename.h"
#include "varstring.h"

#include "char.h"
#include "input-ccc.h"
#include "main.h"


/* Set the return value RET to either the dimension DIM, if NAME is a
   valid character name, or an invalid sentinel.  Used in the
   `unit_of_measure' rule.  */
#define DO_CHAR_DIMEN(ret, dimen, name)					\
  if (char_known_p (name))						\
    ret = dimen_init (dimen);						\
  else									\
    DIMEN_VALID (ret) = false


static ccc_cmd_list_type *append_cmd (ccc_cmd_list_type *, ccc_cmd_type);
static ccc_cmd_list_type *init_cmd_list (ccc_cmd_type);
static boolean char_known_p (charname_type);
static dimen_type dimen_init (real);
static int token_number (string);
static void yyerror (string);
static int yylex ();
%}

%union
{
  int int_val;
  real real_val;
  dimen_type dimen_val;
  charname_type charname_val;
  ccc_cmd_type cmd_val;
  ccc_cmd_list_type *cmd_list_val;
}
%token <int_val> TR_DEFINE TR_END TR_SETCHAR TR_SETCHARBB TR_HMOVE TR_VMOVE
%token <real_val> T_REAL
%token <dimen_val> TR_PT TR_XHEIGHT TR_CAPHEIGHT TR_FONTDEPTH
%token <dimen_val> TR_DESIGNSIZE TR_EM TR_HEIGHT TR_WIDTH TR_DEPTH
%token <dimen_val> TR_BBHEIGHT TR_BBWIDTH
%token <charname_val> T_CHARNAME
%type <cmd_list_val> statements
%type <cmd_val> statement
%type <int_val> set_cmd move_cmd
%type <dimen_val> dimension unit_of_measure

%%
chardefs:	  /* empty  */
		| chardefs chardef
		;

chardef:	TR_DEFINE T_CHARNAME '=' statements TR_END
                  { 
                    /* It doesn't hurt to define a character that won't
                       be used, so we don't call `char_known_p' here.  */
                    ccc_chars[$2.code] = $4;
                  }
		;

statements:	  statement
		    { $$ = init_cmd_list ($1); }
		| statements statement
		    { $$ = append_cmd ($1, $2); }
		;

statement:	  set_cmd '(' T_CHARNAME ')'	
		    {
                      CCC_VALID ($$) = char_known_p ($3);
                      if (CCC_VALID ($$))
                        {
                          CCC_OPCODE ($$) = $1;
  	  	          CCC_CHARCODE ($$) = $3.code;
                        }
                    }
		| move_cmd dimension
		    {
                      CCC_VALID ($$) = DIMEN_VALID ($2);
                      if (CCC_VALID ($$))
                        {
                          CCC_OPCODE ($$) = $1;
  		          CCC_DIMEN ($$) = $2;
                        }
                    }
		;

set_cmd:	TR_SETCHAR | TR_SETCHARBB ;
		
move_cmd:	TR_HMOVE | TR_VMOVE ;

dimension: 	  unit_of_measure
		    { $$ = $1; }
		| T_REAL unit_of_measure	
		    {
                      DIMEN_AMOUNT ($$) = $1 * DIMEN_AMOUNT ($2);
		      DIMEN_ABSOLUTE ($$) = DIMEN_ABSOLUTE ($2);
                      DIMEN_VALID ($$) = DIMEN_VALID ($2);
		    }
		;

unit_of_measure:  TR_PT				
		    {
                      $$ = dimen_init (1);
                      DIMEN_ABSOLUTE ($$) = true;
                    }
                | TR_CAPHEIGHT  { $$ = dimen_init (ccc_fontinfo.cap_height); }
                | TR_DESIGNSIZE { $$ = dimen_init (ccc_fontinfo.design_size); }
                | TR_EM         { $$ = dimen_init (ccc_fontinfo.em); }
                | TR_FONTDEPTH  { $$ = dimen_init (ccc_fontinfo.font_depth); }
                | TR_XHEIGHT    { $$ = dimen_init (ccc_fontinfo.xheight); }
                | TR_HEIGHT '(' T_CHARNAME ')'
                    {
                      DO_CHAR_DIMEN ($$, CHAR_HEIGHT (*chars[$3.code]), $3);
                    }
		| TR_WIDTH '(' T_CHARNAME ')'
                    {
                      DO_CHAR_DIMEN ($$, CHAR_SET_WIDTH (*chars[$3.code]), $3);
                    }
		| TR_DEPTH '(' T_CHARNAME ')'
                    {
                      DO_CHAR_DIMEN ($$, CHAR_DEPTH (*chars[$3.code]), $3);
                    }
                | TR_BBHEIGHT '(' T_CHARNAME ')'	
                    {
                      DO_CHAR_DIMEN
                        ($$, BB_HEIGHT (CHAR_BB (*chars[$3.code])), $3);
                    }
                | TR_BBWIDTH '(' T_CHARNAME ')'	
                    {
                      DO_CHAR_DIMEN
                        ($$, BB_WIDTH (CHAR_BB (*chars[$3.code])), $3);
                    }
		;
%%

/* Let's hope Bison will never use this value as a token code.  */
#define NOT_A_TOKEN (-1)

/* Keep track of line numbers for error messages.  */
static unsigned lineno = 1;

static int get_word_token (FILE *);


/* We don't want to use the libfile routines for CCC files, since
   there's no reason for them to be in the system data directories.  So
   we must have our own error routines.  */
#define CCC_WARN1(fmt, e1) \
  fprintf (stderr, "%s:%u: " fmt ".\n", ccc_filename, lineno, e1)
#define CCC_WARN2(fmt, e1, e2) \
  fprintf (stderr, "%s:%u: " fmt ".\n", ccc_filename, lineno, e1, e2)

static void
yyerror (string s)
{
  fprintf (stderr, "%s:%u: %s\n", ccc_filename, lineno, s);
}

/* Return the next token in `ccc_file'.  */

static int
yylex (void)
{
  int t = NOT_A_TOKEN;

  do
    {
      int c = getc (ccc_file);

      /* Skip whitespace.  */
      while (c != EOF && isspace (c))
        {
          if (c == '\n') lineno++;
          c = getc (ccc_file);
	}
        
      switch (c)
        {
        case EOF:
          t = 0;
          lineno = 1; /* For the next file (if we ever allow one).  */
          break;

        /* Comments begin with % and go to the end of the line.  */
        case '%':
          while ((c = getc (ccc_file)) != '\n' && c != EOF)
            ;
          if (c == '\n') lineno++;
          break;

        case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	case '.': case '-': case '+':
          ungetc (c, ccc_file);
          fscanf (ccc_file, "%lf", &yylval.real_val);
          t = T_REAL;
          break;

        default:
          if (isalpha (c))
            {
              ungetc (c, ccc_file);
              t = get_word_token (ccc_file);
            }
          else
            t = c;
          break;
        }
    }
  while (t == NOT_A_TOKEN);

  return t;
}

/* Return either the Bison-defined value for the token equal to the
   next nonalpha-delimited word in F, or if the word is
   the encoding name of a character, return T_CHARNAME.

   Sets the global `yylval.charname_val' to the encoding number of a
   character encoding name, if the word is an encoding name.

   Assumes the first character is valid (i.e., alpha).  */

static int
get_word_token (FILE *f)
{
  int token_value, encoding_value;
  int c;
  variable_string word = vs_init ();

  while ((c = getc (f)) != EOF && isalpha (c))
    vs_append_char (&word, c);

  if (c != EOF)
    ungetc (c, f);

  /* Add the terminating null.  */
  vs_append_char (&word, 0);

  token_value = token_number (VS_CHARS (word));
  encoding_value = encoding_number (encoding_info, VS_CHARS (word));

  /* If the identifier is neither a reserved word nor a known character
     name, return T_CHARNAME and call it character zero.  */
  if (token_value == NOT_A_TOKEN && encoding_value == -1)
    {
      CCC_WARN1 ("Unknown identifier `%s'", VS_CHARS (word));
      encoding_value = 0;
    }

  if (token_value != NOT_A_TOKEN && encoding_value != -1)
    CCC_WARN1 ("`%s' both a reserved word and a character name",
 	      VS_CHARS (word));

  if (token_value == NOT_A_TOKEN)
    {
      yylval.charname_val.code = encoding_value;
      yylval.charname_val.name = VS_CHARS (word);
      token_value = T_CHARNAME;
    }
  else
    {
      yylval.int_val = token_value;
      vs_free (&word);
    }
  
  return token_value;
}


/* If WORD is a reserved word, return the corresponding token code;
   otherwise, return `NOT_A_TOKEN'.  */

static int
token_number (string word)
{
  if (STREQ (word, "define"))		return TR_DEFINE;
  else if (STREQ (word, "bbheight"))	return TR_BBWIDTH;
  else if (STREQ (word, "bbwidth"))	return TR_BBWIDTH;
  else if (STREQ (word, "capheight"))	return TR_CAPHEIGHT;
  else if (STREQ (word, "depth"))	return TR_DEPTH;
  else if (STREQ (word, "designsize"))	return TR_DESIGNSIZE;
  else if (STREQ (word, "em"))		return TR_EM;
  else if (STREQ (word, "end"))		return TR_END;
  else if (STREQ (word, "fontdepth"))	return TR_FONTDEPTH;
  else if (STREQ (word, "height"))	return TR_HEIGHT;
  else if (STREQ (word, "hmove"))	return TR_HMOVE;
  else if (STREQ (word, "pt"))		return TR_PT;
  else if (STREQ (word, "setchar"))	return TR_SETCHAR;
  else if (STREQ (word, "setcharbb"))	return TR_SETCHARBB;
  else if (STREQ (word, "vmove"))	return TR_VMOVE;
  else if (STREQ (word, "width"))	return TR_WIDTH;
  else if (STREQ (word, "xheight"))	return TR_XHEIGHT;
  else
    return NOT_A_TOKEN;
}

/* Return a new list of CCC commands, initialized to contain CMD.  If
   CMD is invalid, return NULL.  */

static ccc_cmd_list_type *
init_cmd_list (ccc_cmd_type cmd)
{
  ccc_cmd_list_type *l;
  
  if (CCC_VALID (cmd))
    {
      l = XTALLOC1 (ccc_cmd_list_type);
      *l = list_init ();
      l = append_cmd (l, cmd);
    }
  else
    l = NULL;

  return l;
}


/* Add CMD to CMD_LIST and return CMD_LIST.  If CMD is invalid, don't
   add it.  If CMD_LIST is NULL, initialize it.  */

static ccc_cmd_list_type *
append_cmd (ccc_cmd_list_type *cmd_list, ccc_cmd_type cmd)
{
  if (CCC_VALID (cmd))
    {
      if (cmd_list == NULL)
        cmd_list = init_cmd_list (cmd);
      else
        {
          address new_element_addr = LIST_TAPPEND (cmd_list, ccc_cmd_type);
          *(ccc_cmd_type *) new_element_addr = cmd;
        }
    }

  return cmd_list;
}


/* Return an initialized dimension.  */

static dimen_type
dimen_init (real amount)
{
  dimen_type d;
  
  DIMEN_VALID (d) = true;
  DIMEN_ABSOLUTE (d) = false;
  DIMEN_AMOUNT (d) = amount;
  
  return d;
}

/* Return true if the character C is defined; otherwise, give a warning.  */

static boolean
char_known_p (charname_type c)
{
  boolean ok = false;
  
  if (chars[c.code] == NULL)
    CCC_WARN2 ("Character %d (%s) undefined", c.code, c.name);

  else
    ok = true;
  
  return ok;
}
