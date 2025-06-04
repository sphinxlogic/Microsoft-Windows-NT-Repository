/*
 *	This file contains the character class and token code
 *	definitions that are used globally throuhout all files
 *	used in the sdcl program.  Hence all C source files
 *	include this file.
 *
** 0.001 Dick Munroe 03-Sep-90
**	Add DOLLAR as a special identifier.
**
** 0.002 Dick Munroe 15-Sep-90
**	Add parsing of identifier substitution in quoted strings.
**
**  0.003 Dick Munroe 30-May-95
**	Add parsing of identifier substitution.
**
**  0.004 Dick Munroe 01-Jun-95
**	Add ON blocks.
**/

#define  MAXTOKENLEN	133
#define  MAXCONDLEN     255
#define  TRUE		  1
#define  FALSE		  0

/* First, defines to avoid using numbers for charclasses. */

#define  LETTER     0  /* [A-Za-z_$] (notice _ and $ are
                                       considered to be characters) */
#define  DIGIT      1  /* [0-9] */
#define  ONECHAR    2  /* (){}[].;:#%? */
#define  SLASH      3  /* /  */
#define  STAR       4  /* * */
#define  WHITESPACE 5  /* blank,tab,non-printing chars */

#define  EOL        6  /* /n */
#define  DQUOTE     7  /* "  */
#define  ENDFILE    8 
#define	 ERR	    9
#define  SUBSTITUTE 10 /* '  */

/* Use defines to set up token codes.  */

#define  IF      0  /* keywords first, if */
#define  ELSE    1
#define  WHILE   2
#define  FOR     3
#define  BREAK   4
#define  DO      50   /* this was added later */
#define	 ON	 51
#define  THEN	 52
#define	 ON_WARNING 53
#define	 ON_ERROR 54
#define	 ON_SEVERE_ERROR 55
#define	 ON_CONTROL_Y 56

#define  NEXT    5
#define  ID      6  

/* 
 *  If an identifier is not a keyword it might be one 
 *  of the following special single characters... 
 *
 */

#define  OPAREN  7
#define  CPAREN  8
#define  OBRACE  9
#define  CBRACE  10
#define	 SEMICOLON  19
#define  BANG 20
#define  POUND 21
#define  PERCENT 22
#define  AND 23
#define  QUOTE 24
#define  PLUS 25
#define  COMMA 26
#define  MINUS 27
#define  DOT 28
#define  COLON 29
#define  LT 30
#define  EQUAL 31
#define  GT 32 
#define  QUESTION 33
#define  AT 34
#define  OBRACKET 35
#define  BACKSLASH 36
#define  CBRACKET 37
#define  CIRCUMFLEX 38
#define  GRAVE 39
#define  VERTICAL 40
#define  TILDE 41
#define  DOLLAR 42

/* otherwise it is a single character. */

#define  SCHAR   11  

/* Blanks,tab,non-printing characters */

#define  COMMENT 12
#define  WSPACE  13 
#define  NEWLINE 14
#define  FILEEND 15
#define  STRING  16  /* "...." */
#define  ERROR   17
#define  INTEGER 18
