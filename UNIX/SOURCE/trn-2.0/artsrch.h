/* $Id: artsrch.h,v 1.2 92/01/11 16:03:48 usenet Exp $
 *
 * $Log:	artsrch.h,v $
 * Revision 1.2  92/01/11  16:03:48  usenet
 * header twiddling, bug fixes
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

#ifndef NBRA
#include "search.h"
#endif

#ifdef ARTSEARCH

#define SRCH_ABORT 0
#define SRCH_INTR 1
#define SRCH_FOUND 2
#define SRCH_NOTFOUND 3
#define SRCH_DONE 4
#define SRCH_SUBJDONE 5
#define SRCH_ERROR 6
#endif

EXT char *lastpat INIT(nullstr);	/* last search pattern */
#ifdef ARTSEARCH
    EXT COMPEX sub_compex;		/* last compiled subject search */
    EXT COMPEX art_compex;		/* last compiled normal search */
#   ifdef CONDSUB
	EXT COMPEX *bra_compex INIT(&(art_compex));
					/* current compex with brackets */
#   endif
    EXT char art_howmuch;		/* search just the subjects */
    EXT bool art_doread;		/* search read articles? */
#endif

void	artsrch_init ANSI((void));
#ifdef ARTSEARCH
    int		art_search ANSI((char *, int, int));
    bool	wanted ANSI((COMPEX *, ART_NUM, char_int));
			/* return TRUE if current article matches pattern */
#endif
