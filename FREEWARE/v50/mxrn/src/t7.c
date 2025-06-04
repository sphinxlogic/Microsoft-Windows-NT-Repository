#
/*
 *				T 7 . C
 *
 *	char *
 *	re_exec(string, pattern)
 *	char		*string;	-- Where to search for it
 *	char		*pattern;	-- What to search for
 *
 * Apply the regular expression (pattern[]) to the argument string[].
 * Return 0 if it fails.  If successful, return a pointer to the string.
 * that matched.  Regular expressions are defined in rxcomp().
 *
 * Globals unique to this routine are preceeded by "re_".
 */

#include	<stdio.h>
#ifdef	vms
#include	<ctype.h>
#define	FALSE	0
#define	TRUE	1
#endif
#include	"t.h"

/*
 * The following must be identical in rxcomp().
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
#define	CDIGIT	14		/* Not used in this version		*/
#define RANGE	15
#define	ENDPAT	16

static char	*re_string;		/* Remember start loc.		*/
static char	*re_pmatch();
extern void	abort();

char *
re_exec(string)
char		*string;		/* The string to look for	*/
/*
 * Match the string, return TRUE if it does.
 */
{
	char *pattern = rx_pattern;
	register char	*l;		/* Line pointer			*/

	re_string = string;
	for (l = string; *l; l++) {
		if (re_pmatch(l, string, pattern))
			return(l);
	}
	return(NULL);
}

static char *
re_pmatch(line, start, pattern)
char		*line;		/* (partial) line to match		*/
char		*start;		/* Invariant line start			*/
char		*pattern;	/* (partial) pattern to match		*/
/*
 * Do the actual work of matching (recursively).  Line is the current
 * piece of information to match.  Start is the entire line -- as passed
 * to t_rxgrep().  This is needed for the :c scan.
 */
{
	register char	*l;		/* Current line pointer		*/
	register char	*p;		/* Current pattern pointer	*/
	register char	c;		/* Current character		*/
	char		*e;		/* End for STAR and PLUS match	*/
	int		op;		/* Pattern operation		*/
	int		n;		/* Class counter		*/
	char		*are;		/* Start of STAR match		*/
	char		*commapos;	/* Comma position for :c match	*/

	l = line;
	p = pattern;
	commapos = 0; 
	while ((op = *p++) != ENDPAT) {
		switch(op) {

		case CHAR:
			if (tolower(*l++) != *p++)
				return(0);
			break;

		case BOL:
			if (l != re_string)
				return(0);
			break;

		case EOL:
			if (*l != '\0')
				return(0);
			break;

		case ANY:
			if (*l++ == '\0')
				return(0);
			break;

		case DIGIT:
			if ((c = *l++) < '0' || (c > '9'))
				return(0);
			break;

		case ALPHA:
			c = tolower(*l++);
			if (c < 'a' || c > 'z')
				return(0);
			break;

		case NALPHA:
			c = tolower(*l++);
			if (c >= 'a' && c <= 'z')
				break;
			else if (c < '0' || c > '9')
				return(0);
			break;

		case PUNCT:
			c = *l++;
			if (c == 0 || c > ' ')
				return(0);
			break;

		case CLASS:
		case NCLASS:
			c = tolower(*l++);
			n = *p++ & 0377;
			do {
				if (*p == RANGE) {
					p += 3;
					n -= 2;
					if (c >= p[-2] && c <= p[-1])
						break;
				}
				else if (c == *p++)
					break;
			} while (--n > 1);
			if ((op == CLASS) == (n <= 1))
				return(0);
			if (op == CLASS)
				p += n - 2;
			break;

		case MINUS:
			e = re_pmatch(l, start, p);
			while (*p++ != ENDPAT);	/* Skip over pattern	*/
			if (e)			/* Got a match?		*/
				l = e;		/* Yes, update string	*/
			break;			/* Always succeeds	*/

		case PLUS:			/* One or more ...	*/
			if ((l = re_pmatch(l, start, p)) == 0)
				return(0);	/* Gotta have a match	*/
		case STAR:			/* Zero or more ...	*/
			are = l;		/* Remember line start	*/
			while (*l && (e = re_pmatch(l, start, p)))
				l = e;		/* Get longest match	*/
			while (*p++ != ENDPAT);	/* Skip over pattern	*/
			while (l >= are) {	/* Try to match rest	*/
				if (e = re_pmatch(l, start, p))
					return(e);
				--l;		/* Nope, try earlier	*/
			}
			return(0);		/* Nothing else worked	*/

		default:
			abort();		/* Illegal op code	*/
		}
	}
	return(l);
}
