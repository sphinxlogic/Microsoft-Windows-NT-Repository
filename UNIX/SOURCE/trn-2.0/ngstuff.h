/* $Id: ngstuff.h,v 1.2 92/01/11 16:06:17 usenet Exp $
 *
 * $Log:	ngstuff.h,v $
 * Revision 1.2  92/01/11  16:06:17  usenet
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

#define NN_NORM 0
#define NN_INP 1
#define NN_REREAD 2
#define NN_ASK 3

EXT bool one_command INIT(FALSE);	/* no ':' processing in perform() */

void	ngstuff_init ANSI((void));
int	escapade ANSI((void));
int	switcheroo ANSI((void));
int	numnum ANSI((void));
int	perform ANSI((char *,int));
#ifdef USETHREADS
int	use_selected ANSI((void));
#endif
