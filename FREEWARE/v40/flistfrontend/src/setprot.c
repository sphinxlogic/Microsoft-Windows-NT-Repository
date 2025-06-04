#ifndef NO_IDENT
static char *Id = "$Id: setprot.c,v 1.7 1995/03/18 23:00:53 tom Exp $";
#endif

/*
 * Title:	setprot.c
 * Author:	Thomas E. Dickey
 * Created:	19 Nov 1984
 * Last update:
 *		18 Mar 1995, prototypes
 *		15 Jun 1985, typed 'strchr'
 *		21 Dec 1984, make proper error-returns!
 *
 * Function:	Given a partially-parsed "SET PROTECTION" command, perform
 *		it by calling 'chprot'.
 *
 * Arguments:	filespec - file specification (a null-ended string)
 *		prot_code - string in the form "PROT=xxx".
 */

#include	<ctype.h>
#include	<string.h>

#include	"bool.h"
#include	"dclarg.h"

#include	"getprot.h"
#include	"chprot.h"

#include	"strutils.h"

#define	SKIP(p)	while(isspace(*p))	p++

int
setprot (char *filespec, char *prot_code)
{
	int	level,
		infield,
		found	= FALSE,	/* flag if any access specified	*/
		code	= 0xffff,	/* Deny any access		*/
		mask	= 0xffff,	/* Use all old-fields		*/
		brace	= FALSE;
	register
	char	*code_	= prot_code;
	char	*s_;

	static	char	*key[] = {"SYSTEM", "OWNER", "GROUP", "WORLD"},
			key2[] = "aRWED";

	/*
	 * Skip over "PROT=":
	 */
	while (*code_ && !isopt2(*code_))		code_++;
	if (! (*code_++))				goto no_code;
	if (! (*code_))					goto no_code;

	SKIP(code_);
	if (*code_ == '(')	/* If we have a '('	*/
	{
		code_++;	/* ...skip it		*/
		brace++;	/* ...but remember it.	*/
	}

	while (*code_)
	{
		SKIP(code_);
		if (isalpha(*code_))	/* Find a keyword	*/
		{
			s_ = code_;
			while (isalpha(*code_))	code_++;
			for (level = 0; level < 4; level++)
			{
				if (strabbr(s_, key[level], code_-s_, 1))
					goto ok_abbr;
			}
			goto no_code;
ok_abbr:		SKIP(code_);
			found = (0xf << (level << 2));
			code |= found;
			mask &= ~found;
			if (isopt2(*code_))
			{
				code_++;	/* skip over ':'	*/
				SKIP(code_);
				level = (level << 2) - 1;
				while ((s_ = strchr(key2, _toupper(*code_)))
				&& 	*code_)
				{
					infield = s_ - key2;
					code &= ~(1 << (infield + level));
					code_++;
				}
			}
		}
		SKIP(code_);
		if (!*code_)
			break;
		else if (*code_ == ',')
			code_++;
		else if (*code_++ == ')')
		{
			SKIP(code_);
			if (brace && !*code_ && found)
			{
				brace = 0;
				break;
			}
			goto no_code;
		}
	}
	return (chprot (filespec, code, mask));

no_code:
	return (-1);	/* Syntax error	*/
}
