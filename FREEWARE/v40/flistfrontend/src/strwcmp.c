#ifndef NO_IDENT
static char *Id = "$Id: strwcmp.c,v 1.4 1995/06/06 13:41:18 tom Exp $";
#endif

/*
 * Title:	strwcmp.c
 * Author:	Thomas E. Dickey
 * Created:	24 May 1984
 * Last update:
 *		18 Feb 1995	prototyped
 *		14 Sep 1985	Don't pass ALL/ONE as arguments.  Also, test for
 *				pathname-wildcard "..." which matches 0 or more
 *				tokens delimited by '.'
 *		24 May 1984
 *
 * Function:	This procedure performs wild-card comparison on a string,
 *		a la VMS: the first argument may contain wild-card characters
 *		<ALL> and <ONE>, which respectively match any sequence and
 *		any single character in the second argument.  Characters
 *		other than <ALL> or <ONE> in the first argument must match
 *		exactly in the second.
 *
 * Parameters:	wild_	=> string containing wildcard characters
 *		tame_	=> string to be tested for match.
 *
 * Returns:	1 (TRUE) if no match, else 0 (FALSE, or EQUAL).
 */

#include <string.h>

#include "strutils.h"

#define	EQUAL		0
#define	ANY		'*'
#define	ONE		'%'
#define	DOT		'.'
#define	ELLIPSIS	"..."

int
strwcmp (char *wild_, char *tame_)
{
	while (*wild_ && *tame_)
	{
	    if (*wild_ == ANY)
	    {
		while (*wild_ == ANY)	wild_++;
		if (*wild_)
		{
		    while (*tame_)
		    {
			if (strwcmp (wild_, tame_))
			    tame_++;
			else
			    return (EQUAL);
		    }
		    return (! EQUAL);
		}
		else	/* Match remainder of string	*/
		    return (EQUAL);
	    }
	    else if (*wild_ == ONE)
		wild_++, tame_++;
	    else if (*wild_ == DOT)
	    {
		/*
		 * If we have an ellipsis, we may match any number of directory
		 * names, separated by '.', e.g., [...] matches
		 *	[name]
		 *	[.name]
		 *	[name.name]
		 */
		if (! strncmp (wild_, ELLIPSIS, sizeof(ELLIPSIS)-1))
		{
		    wild_ += sizeof(ELLIPSIS)-1;
		    if (*tame_ == DOT)
		    	tame_++;
		    /*
		     * The inputs to this routine *must* be syntactically
		     * correct.  VMS permits an ellipsis only within brackets.
		     * Therefore, we can safely (?) test the last character
		     * before the current scanning position:
		     */
		    else if (tame_[-1] != '.' && tame_[-1] != '[')
		    	return (! EQUAL);

		    /*
		     * Fall-thru to scan matches against an ellipsis:
		     */
		    while (*tame_ && (*tame_ != ']'))
		    {
			if (strwcmp (wild_, tame_))
			{
			    while ((*tame_ != DOT) && (*tame_ != ']'))
				tame_++;
			    if (*tame_ == DOT)	tame_++;
			}
			else
			    return (EQUAL);
		    }
		    return (strwcmp (wild_, tame_));
		}
		else if (*wild_++ != *tame_++)
		    return (! EQUAL);
	    }
	    else if (*wild_++ != *tame_++)
		return (! EQUAL);
	}

	if (*wild_ == ANY)
	    return (wild_[1] != '\0');
	return (*wild_ || *tame_); /* Equal if both end at the same point */
}
