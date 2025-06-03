/* $Id: cheat.c,v 1.1 92/01/11 15:50:05 usenet Stab $
 *
 * $Log:	cheat.c,v $
 * Revision 1.1  92/01/11  15:50:05  usenet
 * USENET News Software
 * 
 * Revision 4.4  1991/09/09  20:18:23  sob
 * release 4.4
 *
 *
 * 
 */
/* This software is Copyright 1991 by Stan Barber. 
 *
 * Permission is hereby granted to copy, reproduce, redistribute or otherwise
 * use this software as long as: there is no monetary profit gained
 * specifically from the use or reproduction of this software, it is not
 * sold, rented, traded or otherwise marketed, and this copyright notice is
 * included prominently in any copy made. 
 *
 * The author make no claims as to the fitness or correctness of this software
 * for any use whatsoever, and it is provided as is. Any use of this software
 * is at the user's own risk. 
 */

#include "EXTERN.h"
#include "common.h"
#include "intrp.h"
#include "search.h"
#include "ng.h"
#include "bits.h"
#include "artio.h"
#include "term.h"
#include "artsrch.h"
#include "head.h"
#include "INTERN.h"
#include "cheat.h"

/* see what we can do while they are reading */

#ifdef PENDING
#   ifdef ARTSEARCH
	COMPEX srchcompex;		/* compiled regex for searchahead */
#   endif
#endif

void
cheat_init()
{
    ;
}

#ifdef PENDING
void
look_ahead()
{
#ifdef ARTSEARCH
    register char *h, *s;

#ifdef DEBUGGING
    if (debug && srchahead) {
	printf("(%ld)",(long)srchahead);
	fflush(stdout);
    }
#endif
    if (srchahead && srchahead < art) {	/* in ^N mode? */
	char *pattern;

	pattern = buf+1;
	strcpy(pattern,": *");
	h = pattern + strlen(pattern);
	interp(h,(sizeof buf) - (h-buf),"%\\s");
	{			/* compensate for notesfiles */
	    register int i;
	    for (i = 24; *h && i--; h++)
		if (*h == '\\')
		    h++;
	    *h = '\0';
	}
#ifdef DEBUGGING
	if (debug & DEB_SEARCH_AHEAD) {
	    fputs("(hit CR)",stdout);
	    fflush(stdout);
	    gets(buf+128);
	    printf("\npattern = %s\n",pattern);
	}
#endif
	if ((s = compile(&srchcompex,pattern,TRUE,TRUE)) != Nullch) {
				    /* compile regular expression */
	    printf("\n%s\n",s);
	    srchahead = 0;
	}
	if (srchahead) {
	    srchahead = art;
	    for (;;) {
		srchahead++;	/* go forward one article */
		if (srchahead > lastart) { /* out of articles? */
#ifdef DEBUGGING
		    if (debug)
			fputs("(not found)",stdout);
#endif
		    break;
		}
		if (!was_read(srchahead) &&
		    wanted(&srchcompex,srchahead,0)) {
				    /* does the shoe fit? */
#ifdef DEBUGGING
		    if (debug)
			printf("(%ld)",(long)srchahead);
#endif
#ifdef SERVER
		    nntpopen(srchahead,GET_HEADER);
#else
		    artopen(srchahead);
#endif
		    break;
		}
		if (input_pending())
		    break;
	    }
	    fflush(stdout);
	}
    }
    else
#endif
    {
	if (art+1 <= lastart)/* how about a pre-fetch? */
#ifdef SERVER
	    nntpopen(art+1,GET_HEADER);	/* look for the next article */
#else
	    artopen(art+1);	/* look for the next article */
#endif
    }
}
#endif

/* see what else we can do while they are reading */

void
collect_subjects()
{
#ifdef PENDING
# ifdef CACHESUBJ
    ART_NUM oldart = openart;
    ART_POS oldartpos;

    if (!in_ng || !srchahead)
	return;
    if (oldart)			/* remember where we were in art */
	oldartpos = ftell(artfp);
    if (srchahead >= subj_to_get)
	subj_to_get = srchahead+1;
    while (!input_pending() && subj_to_get <= lastart)
	fetchsubj(subj_to_get++,FALSE,FALSE);
    if (oldart) {
	artopen(oldart);
	fseek(artfp,oldartpos,0);	/* do not screw the pager */
    }
# endif
#endif
}

