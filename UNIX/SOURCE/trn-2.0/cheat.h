/* $Id: cheat.h,v 1.2 92/01/11 16:04:05 usenet Exp $
 *
 * $Log:	cheat.h,v $
 * Revision 1.2  92/01/11  16:04:05  usenet
 * header twiddling, bug fixes
 * 
 * Revision 4.4  1991/09/09  20:18:23  sob
 * release 4.4
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

#ifdef ARTSEARCH
EXT ART_NUM srchahead INIT(0); 	/* are we in subject scan mode? */
				/* (if so, contains art # found or -1) */
#endif

#ifdef PENDING
#   ifdef CACHESUBJ
	EXT ART_NUM subj_to_get;
#   endif
#endif

void	cheat_init ANSI((void));
void	look_ahead ANSI((void));
void	collect_subjects ANSI((void));
