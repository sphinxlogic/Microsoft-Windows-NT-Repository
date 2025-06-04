/*
 *	re_comp(pattern)
 *	char *pattern;
 *
 * Compile the regular expression pattern.  Return TRUE if success.
 * The compiled pattern is stored in a global character vector 'rx_pattern'
 *
 * If an error occurred, re_comp() returns FALSE and rx_pattern contains
 * an error message.
 *
 *
 * Syntax errors print a message and return 0.  Size errors abort.
 *
 *
 * External routines:
 *	warn			Print warning/error messages
 *	fatal			Fatal error exit
 *
 * Regular expressions are defined as follows:
 *
 * x	An ordinary character (not mentioned below) matches that character.
 * '\'	The backslash quotes any character.  "\$" matches a dollar-sign.
 * '^'	A circumflex at the beginning of an expression matches the
 * 	  beginning of a line.
 * '$'	A dollar-sign at the end of an expression matches the end of
 * 	  a line.
 * '.'	A period matches any character except "new-line".
 * ':a'	A colon matches a class of characters described by the following
 * ':d'	  character.  ":a" matches any alphabetic, ":d" matches digits,
 * ':n'	  ":n" matches alphanumerics, ": " matches spaces, tabs, and
 * ': '	  other control characters, such as new-line.
 * '*'	An expression followed by an asterisk matches zero or more
 * 	  occurrances of that expression: "fo*" matches "f", "fo"
 * 	  "foo", etc.
 * '+'	An expression followed by a plus sign matches one or more
 * 	  occurrances of that expression: "fo+" matches "fo", etc.
 * '-'	An expression followed by a minus sign optionally matches
 * 	  the expression.
 * '[]'	A string enclosed in square brackets matches any character in
 * 	  that string, but no others.  If the first character in the
 * 	  string is a circumflex, the expression matches any character
 * 	  except "new-line" and the characters in the string.  For
 * 	  example, "[xyz]" matches "xx" and "zyx", while "[^xyz]"
 * 	  matches "abc" but not "axb".  A range of characters may be
 * 	  specified by two characters seperated by "-".  Note that,
 * 	  [a-z] matches alphabetics, while [z-a] never matches.
 * 
 * The concatenation of regular expressions is a regular expression.
 */

#include	<stdio.h>
#ifdef vms
#include	<ctype.h>
#include	<string.h>
#define	FALSE	0
#define	TRUE	1
#endif
#include	"t.h"

#define PMAX	256			/* The longest pattern		*/

/*
 * Note: the following must be identical in re_grep() and re_comp().
 */

#define CHAR	1
#define BOL	2
#define EOL	3
#define	ANY	4
#define CLASS	5
#define	NCLASS	6
#define STAR	7
#define	PLUS	8
#define	MINUS	9
#define	ALPHA	10
#define	DIGIT	11
#define	NALPHA	12
#define	PUNCT	13
#define	CDIGIT	14		/* Not used				*/
#define RANGE	15
#define	ENDPAT	16

static char	*rx_pp;			/* Work area pointer		*/
char		rx_pattern[PMAX];	/* Pattern output here		*/
static char	*rx_bad();
static		rx_store();
static char	*rx_cclass();		/* Compile class routine	*/

char *
re_comp(pattern)
char		*pattern;	/* Pattern to compile			*/
/*
 * Compile the pattern.
 */
{
	register char	*s;		/* Source string pointer	*/
	register char	*lp;		/* Last pattern pointer		*/
	register int	c;		/* Current character		*/
	int		o;		/* Temp				*/
	char		*spp;		/* Save beginning of pattern	*/

	s = pattern;
	rx_pp = rx_pattern;
	while (c = *s++) {
		/*
		 * STAR, PLUS and MINUS are special.
		 */
		if (c == '*' || c == '+' || c == '-') {
			if (rx_pp == rx_pattern || (o=rx_pp[-1]) == BOL ||
					o == EOL || o == STAR ||
					o == PLUS || o == MINUS)
				return(rx_bad("Illegal occurrance op.", s));
			rx_store(ENDPAT);
			rx_store(ENDPAT);
			/*
			 * Save the pattern's end and move it down one byte.
			 */
			spp = rx_pp;
			while (--rx_pp > lp)
				*rx_pp = rx_pp[-1];
			*rx_pp = (c == '*') ? STAR :
				(c == '-') ? MINUS : PLUS;
			rx_pp = spp;		/* Restore pattern end	*/
			continue;
		}
		/*
		 * All the rest.
		 */
		lp = rx_pp;			/* Remember start	*/
		switch(c) {

		case '^':
			rx_store(BOL);
			break;

		case '$':
			rx_store(EOL);
			break;

		case '.':
			rx_store(ANY);
			break;

		case '[':
			if ((s = rx_cclass(s)) == 0)
				return("glump");
			break;

		case ':':
			if (*s) {
				c = *s++;
				switch(tolower(c)) {

				case 'a':
					rx_store(ALPHA);
					break;

				case 'd':
					rx_store(DIGIT);
					break;

				case 'n':
					rx_store(NALPHA);
					break;

				case ' ':
					rx_store(PUNCT);
					break;

				default:
					return(rx_bad("Unknown : type", s));

				}
				break;
			}
			else	return(rx_bad("No : type", s));

		case '\\':
			if (*s)
				c = *s++;

		default:
			rx_store(CHAR);
			rx_store(tolower(c));
		}
	}
	rx_store(ENDPAT);
	rx_store(EOS);				/* Terminate string	*/
	return (NULL);
}

static char *
rx_cclass(src)
char		*src;		/* Class start				*/
/*
 * Compile a class (within [])
 */
{
	register char	*s;		/* Pattern pointer		*/
	register char	*cp;		/* Pattern start		*/
	register int	c;		/* Current character		*/
	int		o;		/* Temp				*/

	s = src;
	o = CLASS;
	if (*s == '^') {
		++s;
		o = NCLASS;
	}
	rx_store(o);
	cp = rx_pp;
	rx_store(0);				/* Byte count		*/
	while ((c = *s++) && c!=']') {
		if (c == '\\') {		/* Store quoted char	*/
			if ((c = *s++) == '\0')	/* Gotta get something	*/
				return(rx_bad("Class terminates badly", s));
			else	rx_store(tolower(c));
		}
		else if (c == '-' &&
				(rx_pp - cp) > 1 && *s != ']' && *s != '\0') {
			c = rx_pp[-1];		/* Range start		*/
			rx_pp[-1] = RANGE;	/* Range signal		*/
			rx_store(c);		/* Re-store start	*/
			c = *s++;		/* Get end char and	*/
			rx_store(tolower(c));	/* Store it		*/
		}
		else {
			rx_store(tolower(c));	/* Store normal char	*/
		}
	}
	if (c != ']')
		return(rx_bad("Unterminated class", s));
	if ((c = (rx_pp - cp)) >= 256)
		return(rx_bad("Class too large", s));
	if (c == 0)
		return(rx_bad("Empty class", s));
	*cp = c;
	return(s);
}

static
rx_store(op)
{
	if (rx_pp >= &rx_pattern[PMAX])
		perror("?RXCOMP-F-Pattern too complex");
	*rx_pp++ = op;
}

static char *
rx_bad(message, stopbyte)
char		*message;	/* Error message			*/
char		*stopbyte;	/* Pattern end				*/
{
	strcpy(rx_pattern, "Regular expression error \"");
	strcat(rx_pattern, message);
	strcat(rx_pattern, "\", stopped at \"");
	strcat(rx_pattern, &stopbyte[-1]);
	strcat(rx_pattern, "\"");
	return (rx_pattern);
}

