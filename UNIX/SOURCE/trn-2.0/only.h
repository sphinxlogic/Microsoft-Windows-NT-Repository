/* $Id: only.h,v 1.2 92/01/11 16:06:22 usenet Exp $
 *
 * $Log:	only.h,v $
 * Revision 1.2  92/01/11  16:06:22  usenet
 * header twiddling, bug fixes
 * 
 * Revision 4.4  1991/09/09  20:23:31  sob
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

#ifdef ONLY
    EXT char *ngtodo[NGMAX];		/* restrictions in effect */
#   ifdef SPEEDOVERMEM
	EXT COMPEX *compextodo[NGMAX];	/* restrictions in compiled form */
#   endif
#endif

EXT int maxngtodo INIT(0);			/*  0 => no restrictions */
					/* >0 => # of entries in ngtodo */

void	only_init ANSI((void));
bool	inlist ANSI((char *));	/* return TRUE if ngname is in command line list */
			/* or if there was no list */
void	setngtodo ANSI((char *));
#ifdef ONLY
    void	end_only ANSI((void));
#endif
