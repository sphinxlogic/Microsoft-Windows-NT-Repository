
/*  @(#)parsefrom.c 1.6 91/01/07
 *
 *  Returns pointers to the user and host components of an address
 *  described in either a Unix "From " line or an RFC 822 "From:" line.
 *
 *  Copyright (c) Chris Maltby - Softway Ptd Ltd - June 1990.
 *
 *  Permission is given to distribute these sources, as long as the
 *  copyright messages are not removed, and no monies are exchanged. 
 *
 *  No responsibility is taken for any errors inherent either
 *  to the comments or the code of this program, but if reported
 *  to me then an attempt will be made to fix them.
 */

#include <stdio.h>
#if SYSV32 || hpux
#include <string.h>
#else
#include <strings.h>
#endif /* SYSV32 || hpux */
#include "faces.h"
#include "extern.h"


int
parsefrom(line, userp, hostp)
char *line;
char **userp;
char **hostp;
{
	register char	*p;
	register char	*user;
	register char	*host;

	if (strncmp((user = line), "From ", 5) != 0) {
		if (strncmp(user, "From:", 5) == 0) {
			user += 5;

			while (*user == ' ' || *user == '\t')
				++user;

			/* RFC822 - two formats
			 * From: comment <address>
			 * From: address (comment)
			 *
                         * Will also handle:
                         * From: address
                         */
			if ((p = index(user, '<')) != NULL) {
				user = ++p;
				if ((p = index(user, '>')) != NULL)
					*p = '\0';
				else
					return 0;
			}
			else if ((p = rindex(user, '(' )) != NULL ||
				 (p = rindex(user, '\n')) != NULL) {
				do
					*p = '\0';
				while (*--p == ' ' || *p == '\t');
			}
		}
		else
			return 0;
	} else {
		user += 5;
		while (*user == ' ' || *user == '\t')
			++user;
		if ((p = index(user, ' ')) != NULL)
			*p = '\0';	/* discard date */
	}

	host = NULL;

	/* look for route addresses: @host,addr */
	while (*user == '@') {
		host = ++user;
		if ((p = index(user, ',')) != NULL) {
			*p++ = '\0';
			user = p;
		}
	}
	/* look for '@' */
	while ((p = rindex(user, '@')) != NULL) {
		*p++ = '\0';
		host = p;
	}
	/* now for '%' */
	while ((p = rindex(user, '%')) != NULL) {
		*p++ = '\0';
		host = p;
	}
	/* now for '!' */
	while ((p = index(user, '!')) != NULL) {
		*p++ = '\0';
		host = user;
		user = p;
	}
	if (host == NULL)
		host = hostname;
#ifdef	NODOMAINS
	else if ((p = index(host, '.')) != NULL)
		*p = '\0';
#endif	/* NODOMAINS */
	if ((p = index(user, '\n')) != NULL)
		*p = '\0';
	if ((p = index(host, '\n')) != NULL)
		*p = '\0';
	*hostp = host;
	*userp = user;
	return 1;
}
