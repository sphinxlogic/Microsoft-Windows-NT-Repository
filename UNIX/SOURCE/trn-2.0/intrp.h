/* $Id: intrp.h,v 1.1 92/01/11 15:50:49 usenet Stab $
 *
 * $Log:	intrp.h,v $
 * Revision 1.1  92/01/11  15:50:49  usenet
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

EXT char *lib INIT(Nullch);		/* news library */
EXT char *rnlib INIT(Nullch);		/* private news program library */
EXT char *origdir INIT(Nullch);		/* cwd when rn invoked */
EXT char *homedir INIT(Nullch);		/* login directory */
EXT char *dotdir INIT(Nullch);		/* where . files go */
EXT char *logname INIT(Nullch);		/* login id */
EXT char *sitename INIT(Nullch);	/* host name */
EXT int perform_cnt;

#ifdef NEWSADMIN
    EXT char newsadmin[] INIT(NEWSADMIN);/* news administrator */
    EXT int newsuid INIT(0);
#endif

void    intrp_init ANSI((char *));
char	*filexp ANSI((char *));
char	*dointerp ANSI((char *,int,char *,char *));
void	interp ANSI((char *,int,char *));
void	refscpy ANSI((char *,int,char *));
char	*getrealname ANSI((long));
