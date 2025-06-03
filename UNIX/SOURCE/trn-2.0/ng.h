/* $Id: ng.h,v 1.2 92/01/11 16:06:03 usenet Exp $
 *
 * $Log:	ng.h,v $
 * Revision 1.2  92/01/11  16:06:03  usenet
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

EXT ART_NUM art INIT(0);	/* current or prospective article # */

EXT int checkcount INIT(0);	/* how many articles have we read */
			/*   in the current newsgroup since */
			/*   the last checkpoint? */
EXT int docheckwhen INIT(20);	/* how often to do checkpoint */

#ifdef MAILCALL
EXT int mailcount INIT(0);		/* check for mail when 0 mod 10 */
#endif
EXT char *mailcall INIT(nullstr);
#ifdef SERVER
EXT bool isfirstart INIT(TRUE);
#endif
EXT bool forcelast INIT(FALSE);		/* ought we show "End of newsgroup"? */
EXT bool forcegrow INIT(FALSE);		/* do we want to recalculate size */
				    /* of newsgroup, e.g. after posting? */

#define NG_ERROR -1
#define NG_NORM 0
#define NG_ASK 1
#define NG_MINUS 2
#define NG_SELPRIOR 3
#define NG_SELNEXT 4

void    ng_init ANSI((void));
int	do_newsgroup ANSI((char *));
int	art_switch ANSI((void));
#ifdef MAILCALL
    void	setmail ANSI((void));
#endif
void	setdfltcmd ANSI((void));
