/* $Id: help.h,v 1.2 92/01/11 16:04:37 usenet Exp $
 *
 * $Log:	help.h,v $
 * Revision 1.2  92/01/11  16:04:37  usenet
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

void	help_init ANSI((void));
int	help_ng ANSI((void));
int	help_art ANSI((void));
int	help_page ANSI((void));
#ifdef ESCSUBS
    int	help_subs ANSI((void));
#endif
#ifdef USETHREADS
    int help_select ANSI((void));
#endif
