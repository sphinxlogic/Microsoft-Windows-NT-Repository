static char rcsid[] = "@(#)$Id: gcos_name.c,v 5.1 1992/10/03 22:41:36 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.1 $   $State: Exp $
 *
 *			Copyright (c) 1988-1992 USENET Community Trust
 *			Copyright (c) 1986,1987 Dave Taylor
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: gcos_name.c,v $
 * Revision 5.1  1992/10/03  22:41:36  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** 

**/

#include "headers.h"
#include <ctype.h>

#ifdef BSD 
#undef tolower
#undef toupper
#endif

char *strtok(), *strcpy(), *strcat(), *strncpy(), *index(), *rindex();

char *
gcos_name(gcos_field, logname)
char *logname, *gcos_field;
{
    /** Return the full name found in a passwd file gcos field **/

#ifdef BERKNAMES

    static char fullname[SLEN];
    register char *fncp, *gcoscp, *lncp, *end;


    /* full name is all chars up to first ',' (or whole gcos, if no ',') */
    /* replace any & with logname in upper case */

    for(fncp = fullname, gcoscp= gcos_field, end = fullname + SLEN - 1;
        (*gcoscp != ',' && *gcoscp != '\0' && fncp != end);
	gcoscp++) {

	if(*gcoscp == '&') {
	    for(lncp = logname; *lncp; fncp++, lncp++)
		*fncp = toupper(*lncp);
	} else {
	    *fncp++ = *gcoscp;
	}
    }
    
    *fncp = '\0';
    return(fullname);
#else
#ifdef USGNAMES

    char *firstcp, *lastcp;

    /* The last character of the full name is the one preceding the first
     * '('. If there is no '(', then the full name ends at the end of the
     * gcos field.
     */
    if(lastcp = index(gcos_field, '('))
	*lastcp = '\0';

    /* The first character of the full name is the one following the 
     * last '-' before that ending character. NOTE: that's why we
     * establish the ending character first!
     * If there is no '-' before the ending character, then the fullname
     * begins at the beginning of the gcos field.
     */
    if(firstcp = rindex(gcos_field, '-'))
	firstcp++;
    else
	firstcp = gcos_field;

    return(firstcp);

#else
    /* use full gcos field */
    return(gcos_field);
#endif
#endif
}
