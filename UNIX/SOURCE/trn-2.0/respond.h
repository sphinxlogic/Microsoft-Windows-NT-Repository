/* $Id: respond.h,v 1.1 92/01/11 15:52:29 usenet Stab $
 *
 * $Log:	respond.h,v $
 * Revision 1.1  92/01/11  15:52:29  usenet
 * USENET News Software
 * 
 * Revision 4.4  1991/09/09  20:27:37  sob
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

EXT char *savedest INIT(Nullch);	/* value of %b */
EXT char *extractdest INIT(Nullch);	/* value of %E */
EXT char *extractprog INIT(Nullch);	/* value of %e */
EXT ART_POS savefrom INIT(0);		/* value of %B */
EXT char *headname INIT(Nullch);

#define SAVE_ABORT 0
#define SAVE_DONE 1

void	respond_init ANSI((void));
int	save_article ANSI((void));
int	cancel_article ANSI((void));
int	supersede_article ANSI((void));
void	reply ANSI((void));
void	followup ANSI((void));
void	invoke ANSI((char *,char *));
