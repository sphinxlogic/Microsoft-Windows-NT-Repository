/* Change the ANSI prototypes in C programs to old non prototype format.
   Copyright (C) 1991 Free Software Foundation, Inc.
   Francois Pinard <pinard@iro.umontreal.ca>, February 1991.
*/


/* This program is a quick hack, which lets me write ANSI prototypes in C
   programs without bothering too much about compilers which do not
   accept such prototypes, because this program can later remove the
   prototypes from the code.  For example:
   
	 static void remove (char *name, int flag) { ...  }
   
   will become:
   
	 static void remove (name, flag) char *name; int flag; { ...  }
   
   The correct operation of this program relies on the fact the
   programmer complies with GNU writing code standards, and does not
   write ugly things :-).  This program also removes `const' and
   `volatile' keywords.	 Most other ANSI dependencies should still be
   adjusted the `#ifdef __STDC__' way.	*/

%x			COMMENT
%s			COPYLINE
%s			ARGUMENT
%s			QUOTED

white			(" "|"\t"|"\n"|"\f")
ident			([_A-Za-z][_A-Za-z0-9]*)


%{

/* Some standard declarations.  */

#include <ctype.h>

#ifdef __STDC__

#ifndef FLEX_SCANNER
void *malloc (int);
#endif
void *realloc (void *, int);

#else /* not __STDC__ */

#ifndef FLEX_SCANNER
void *malloc ();
#endif
void *realloc ();

#endif /* not __STDC__ */


/* Global variables.  */

int quote_level;		/* parenthesis quote level for arguments */

char **argument;		/* array of string arguments */
int *length;			/* array of argument lengths */
int *allocated;			/* array of allocated lengths for arguments */
int arg_index_max;		/* length of preceeding arrays */
int arg_index;			/* index for current argument */

/*--------------------------------------------------------------.
| Add a character to the prototype argument under construction, |
| reallocating the space provided for it whenever necessary.    |
`--------------------------------------------------------------*/

#ifdef __STDC__
void
argument_character (char character)
#else
void
argument_character (character)
     char character;
#endif
{
  if (length[arg_index] > 0 || !isspace(character))
    {
      if (length[arg_index] == allocated[arg_index])
	{
	  allocated[arg_index] += 50;
	  if (argument[arg_index] == NULL)
	    argument[arg_index] = malloc (allocated[arg_index]);
	  else
	    argument[arg_index]
	      = (char *) realloc (argument[arg_index], allocated[arg_index]);
	}
      argument[arg_index][length[arg_index]] = character;
      length[arg_index]++;
    }
}

/*---------------------------------------------------------------------.
| Begin the accumulation of a new prototype argument, reallocating the |
| argument list whenever necessary to accomodate it.                   |
`---------------------------------------------------------------------*/

#ifdef __STDC__
void
begin_argument (void)
#else
void
begin_argument ()
#endif
{
  int i;

  if (arg_index == arg_index_max)
    {
      arg_index_max += 10;
      if (argument == NULL)
	{
	  argument = (char **) malloc (arg_index_max * sizeof (char *));
	  length = (int *) malloc (arg_index_max * sizeof (int));
	  allocated = (int *) malloc (arg_index_max * sizeof (int));
	}
      else
	{
	  argument
	    = (char **) realloc (argument, arg_index_max * sizeof (char *));
	  length
	    = (int *) realloc (length, arg_index_max * sizeof (int));
	  allocated
	    = (int *) realloc (allocated, arg_index_max * sizeof (int));
	}
      for (i = arg_index; i < arg_index_max; i++)
	{
	  argument[i] = NULL;
	  allocated[i] = 0;
	}
    }
  length[arg_index] = 0;
}

/*--------------------------------------------------------------------------.
| Complete the accumulation of the current prototype argument.  If it       |
| looks like a function prototype, that is, if the last character is a      |
| closing parenthesis, insure the argument list of this function prototype  |
| is empty by moving the closing parenthesis next to its open parenthesis.  |
`--------------------------------------------------------------------------*/

#ifdef __STDC__
void
end_argument (void)
#else
void
end_argument ()
#endif
{
  int j;

  j = length[arg_index];
  if (j > 0)
    {
      if (argument[arg_index][j-1] == ')')
	{
	  while (j > 0 && argument[arg_index][j-1] != '(')
	    j--;
	  argument[arg_index][j] = ')';
	  length[arg_index] = j + 1;
	}
      arg_index++;
    }
}

/*--------------------------------------------------------------------.
| Start or reset accumulation of prototype arguments for a function.  |
`--------------------------------------------------------------------*/

#ifdef __STDC__
void
begin_argument_list (void)
#else
void
begin_argument_list ()
#endif
{
  arg_index = 0;
  begin_argument ();
}

/*-------------------------------------------------------------------------.
| Complete processing of prototype arguments for a function prototype      |
| which has a body.  This routine first produces a comma separated list of |
| argument names, followed by a close parenthesis.  Then, it produces a    |
| list or argument specifications, one per line, indented five spaces.     |
`-------------------------------------------------------------------------*/

#ifdef __STDC__
void
end_argument_list (void)
#else
void
end_argument_list ()
#endif
{
  int c, i, j, k;

  end_argument ();

  for (i = 0; i < arg_index; i++)
    {
      if (i > 0)
	printf (", ");
      for (k = length[i]; k > 0; k--)
	{
	  c = argument[i][k-1];
	  if (isalpha (c) || isdigit (c) || c == '_')
	    break;
	}
      for (j = k; j > 0; j--)
	{
	  c = argument[i][j-1];
	  if (!isalpha (c) && !isdigit (c) && c != '_')
	    break;
	}
      for (; j < k; j++)
	putchar (argument[i][j]);
    }
  printf (")");
  for (i = 0; i < arg_index; i++)
    {
      printf ("\n     ");
      for (j = 0; j < length[i] && isspace(argument[i][j]); j++)
	;
      for (; j < length[i]; j++)
	putchar (argument[i][j]);
      printf (";");
    }
}

%}

%%

		/* Remove some ANSI keywords.  Because we cannot check left
		   context for a separator and we need to recognize some
		   keywords at other places, we also ought to copy all other
		   identifiers verbatim, elsewhere.  */

"const "		;
"volatile "		;

		/* For now, a prototype line starts with a letter and,
		   before an open parenthesis, may contain identifiers,
		   spaces, stars, brackets, and nothing else.  We copy all
		   this unchanged.  If we meet anything different, stop
		   looking for a prototype.  */

<INITIAL>"\n"		ECHO;
<INITIAL>^[^A-Za-z_]	ECHO; BEGIN COPYLINE;
<INITIAL>{ident}	ECHO;
<INITIAL>{white}	ECHO;
<INITIAL>[*\[\]]	ECHO;
<INITIAL>"("		ECHO; BEGIN ARGUMENT; begin_argument_list ();
<INITIAL>.		ECHO; BEGIN COPYLINE;

		/* Process the prototype.  If this is an declaration without
		   body, which is indicated by a semi-colon after the
		   closing parenthesis, produce a simple declaration with no
		   parameters.  Protect possible commas inside nested
		   parentheses, for functionnal arguments.  */

<ARGUMENT>"("		{
			  argument_character ('(');
			  quote_level = 1; BEGIN QUOTED;
			}
<QUOTED>"("		argument_character ('('); quote_level++;
<QUOTED>")"		{
			  argument_character (')'); quote_level--;
			  if (quote_level == 0) BEGIN ARGUMENT;
			}
<QUOTED>(.|"\n")	argument_character (yytext[0]);

<ARGUMENT>","		end_argument (); begin_argument ();
<ARGUMENT>");"		ECHO; BEGIN COPYLINE;
<ARGUMENT>")"		end_argument_list (); BEGIN COPYLINE;
<ARGUMENT>(.|"\n")	argument_character (yytext[0]);

		/* Recognize (void) as a special case.  */

<ARGUMENT>"void);"	putchar (')'); putchar (';'); BEGIN COPYLINE;
<ARGUMENT>"void)"	end_argument_list (); BEGIN COPYLINE;

		/* Copy all comments verbatim, using an exclusive state if
		   they span several lines.  The following will not work if
		   there are more than one comment in one line.  */

"/*".*"*/"		ECHO;

"/*"			ECHO; BEGIN COMMENT;
<COMMENT>"*/"		ECHO; BEGIN COPYLINE;
<COMMENT>(.|"\n")	ECHO;

		/* The COPYLINE state copies the remainder of the line.  */

<COPYLINE>{ident}	ECHO;
<COPYLINE>.		ECHO;
<COPYLINE>"\n"		ECHO; BEGIN INITIAL;

%%

#include <stdio.h>

/*-----------------------------------------------------------------.
| Merely convert stdin to stdout.  This looks sufficient for now!  |
`-----------------------------------------------------------------*/

void
#ifdef __STDC__
main (int argc, char **argv)
#else
main (argc, argv)
     int argc;
     char **argv;
#endif
{

  /* Check for proper usage.  */

  if (argc > 1)
    {
      fprintf (stderr, "Usage: %s < INPUT > OUTPUT\n", argv[0]);
      exit (1);
    }

  /* Initialize global variables.  */

  argument = NULL;
  arg_index_max = 0;

  /* Process the input file.  */

  yylex ();

  /* All done.  */

  exit (0);
}
