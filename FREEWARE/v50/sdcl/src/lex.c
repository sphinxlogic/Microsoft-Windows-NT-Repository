/*
**  Copyright Dick Munroe, 1985-2000.
**  Rights to distribution and modification are granted as specified in the GNU
**  Public License.
**
**  	Function lex returns the token string and tokencode from standard input.
**  	The lex routine is table driven. The two tables "nextstate" and "output"
**  	were developed by hand.  These tables are external to this file.  They
**  	reside in "tcodes.h" are included in this file.
**
** 0.001 Dick Munroe 03-Sep-90
**	Make $ equivalent to #.
**
** 0.002 Dick Munroe 15-Sep-90
**	Teach lex about variable substitution within quoted strings.
**
**  0.003 Dick Munroe 01-Jun-95
**	Teach lex about ON blocks.
**
**  0.004 dick Munroe 03-Nov-85
**	For right now, for all characters > 127 in value, they are just
**	letters.
**/

#include <ctype.h>
#include <stdio.h>
#include "tcodes.h"
#include "defs.h"

/* Variables that need to retain their values across calls to lex. */

static int state = 0;
static int nextchar;
/*
 *
 *	External FILE variable infile is initialized by the sdcl.c
 *	module.  It points either to stdin or to a user-specified
 *	input file.
 */

extern FILE *infile;

int lex(token)
    char *token;
    {
	/* 
	 *	The findclass array is used for quick lookups of 
	 *	character class of a character.  The ordinal
	 *	value of a character is used as an index into
	 *	this array.  The value found at the location
	 *	indexed by this ordinal value will be the character
	 *	class for this input index character.  Since it
	 *	will be used continously throughout program
	 *	execution and it is only used by function lex()
	 *	the array is static.  The comments used in the
	 *	initialization section for the array correspond
	 *	to the decimal ascii values the rows represent.
	 *
	 */

	static int findclass[] =
	{	 
/*  0 */	 WHITESPACE,

/*  1 - 10 */	 WHITESPACE, WHITESPACE, WHITESPACE, WHITESPACE, WHITESPACE,
		 WHITESPACE, WHITESPACE, WHITESPACE, WHITESPACE, EOL,

/* 11 - 20 */	 WHITESPACE, WHITESPACE, WHITESPACE, WHITESPACE, WHITESPACE,
		 WHITESPACE, WHITESPACE, WHITESPACE, WHITESPACE, WHITESPACE,

/* 21 - 30 */	 WHITESPACE, WHITESPACE, WHITESPACE, WHITESPACE, WHITESPACE,
		 WHITESPACE, WHITESPACE, WHITESPACE, WHITESPACE, WHITESPACE,

/* 31 - 40 */	 WHITESPACE, WHITESPACE, ONECHAR, DQUOTE, ONECHAR,
		 LETTER, ONECHAR, ONECHAR, SUBSTITUTE, ONECHAR,

/* 41 - 50 */ 	 ONECHAR, STAR, ONECHAR, ONECHAR, ONECHAR,
		 ONECHAR, SLASH, DIGIT, DIGIT, DIGIT, 

/* 51 - 60 */	 DIGIT, DIGIT, DIGIT, DIGIT, DIGIT, 
		 DIGIT, DIGIT, ONECHAR, ONECHAR, ONECHAR,

/* 61 - 70 */	 ONECHAR, ONECHAR, ONECHAR, ONECHAR, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 71 - 80 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 81 - 90 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 91 - 100 */	 ONECHAR, ONECHAR, ONECHAR, ONECHAR, LETTER,
		 ONECHAR, LETTER, LETTER, LETTER, LETTER,

/* 101 - 110 */  LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 111 - 120 */	 LETTER, LETTER, LETTER, LETTER, LETTER,  
		 LETTER, LETTER, LETTER, LETTER, LETTER,  

/* 121 - 130 */	 LETTER, LETTER, ONECHAR, ONECHAR, ONECHAR,
		 ONECHAR, LETTER, LETTER, LETTER, LETTER,

/* 131 - 140 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 141 - 150 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 151 - 160 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 161 - 170 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 171 - 180 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 181 - 190 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 191 - 200 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 201 - 210 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 211 - 220 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 221 - 230 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 231 - 240 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 241 - 250 */	 LETTER, LETTER, LETTER, LETTER, LETTER,
		 LETTER, LETTER, LETTER, LETTER, LETTER,

/* 251 - 255 */	 LETTER, LETTER, LETTER, LETTER, LETTER

	};

	int i = 0, out = 0, class;

	/*
	 *  While the output state is non-zero, indicating a complete 
	 *  token has not been found...
	 */
        while (!out){
		if (state)
			/*  and if the maximum token length has not been
			 * exceeded...
			 */
			if (i < MAXTOKENLEN)
			    token[i++] = nextchar;

		nextchar = getc(infile);

		/*
		 *  Find out the character class of this next input
		 *  character.  Use this value to determine what
		 *  the output and nextstate states should be for
		 *  the DFA scanner.
		 */

		/* If the next character is EOF... */

		class = (nextchar != EOF)? findclass[nextchar] : ENDFILE;

		out = output[state][class];
		state = nextstate[state][class];
	}
	token[i] = '\0';
	/* 
	 * Output contains the tokencode. If it is ID, check for keywords
	 * by searching table of keywords.
 	 */
	if (out == ID)
	    out = iskeyword(token);
	else  if (out == SCHAR)
	    out = isspecialonechar(token[0]);

	/* here's the token code */	
	return(out);
    } 

int iskeyword(token)
    char *token;
/*
**  This function compares the input parameter string with a struct array
**  of keywords to determine if the input token is a keyword.  The
**  way the array is arranged function iskeyword returns either 
**  the token code of the keyword if it is one, or the generic 
**  token code ID if the inputted token is not a keyword.
**
**  Note that since the "$" parses as an identifier this routine is the
**  place where it gets converted into a DOLLAR token rather than an
**  IDENTIFIER token.
**/
    {
	int i = 0;
	char buf[MAXTOKENLEN] ;
	static struct kwdtype
	{
		char *kw;
		int tokevalue;
	}
	kwdtable [] =
	{
		"if", IF,
		"else", ELSE,
		"while", WHILE,
		"for", FOR,
		"break", BREAK,
		"next", NEXT,
		"do", DO,
		"$", DOLLAR,
		"on", ON,
		"then", THEN,
		"warning", ON_WARNING,
		"error", ON_ERROR,
		"severe_error", ON_SEVERE_ERROR,
		"control_y", ON_CONTROL_Y,
		NULL, ID
	};

	/*
	** Make the comparison for the keywords be caseless.
	*/

	for (i = 0 ;
	     (buf[i] = _tolower(*(token+i))) != 0 ;
	     i++) {} ;

	for (i = 0 ;
	     kwdtable[i].kw != (char *) NULL ;
	     i++ )
	{
		if (strcmp(kwdtable[i].kw, buf) == 0)
		    return(kwdtable[i].tokevalue);
	}
	return(kwdtable[i].tokevalue);
    }

int isspecialonechar(token)
    char token;
/*
 *	The inputted token is compared with each of the characters
 *	found in actual_char which is an array of structures.  If 
 *	a match is found then the corresponding value to return for
 *	this function is contained in the tvalue of that field of
 *	actual_char.  If no match is found SCHAR is returned.
 */
    {
	static struct {
		char specchar;
		int  tvalue;
	} actual_char[] = {
		'"', DQUOTE,
		'#', POUND,
		'$', DOLLAR,
		'(', OPAREN,
		')', CPAREN,
		';', SEMICOLON,
		'\\', BACKSLASH,
		'{', OBRACE,
		'}', CBRACE,
		'0', SCHAR
	};

	int i;
	for (i = 0; actual_char[i].specchar != '0'; i++)
		if (actual_char[i].specchar == token)
		    return(actual_char[i].tvalue);
	return(actual_char[i].tvalue);
    }
