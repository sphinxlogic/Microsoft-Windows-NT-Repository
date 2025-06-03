/* $Id: art.h,v 1.2 92/01/11 16:03:40 usenet Exp $
 *
 * $Log:	art.h,v $
 * Revision 1.2  92/01/11  16:03:40  usenet
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

/* do_article() return values */

#define DA_NORM 0
#define DA_RAISE 1
#define DA_CLEAN 2
#define DA_TOEND 3

void	art_init ANSI((void));
int	do_article ANSI((void));
int	page_switch ANSI((void));
bool	innermore ANSI((void));
