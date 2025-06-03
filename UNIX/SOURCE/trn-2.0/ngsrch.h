/* $Id: ngsrch.h,v 1.2 92/01/11 16:06:13 usenet Exp $
 *
 * $Log:	ngsrch.h,v $
 * Revision 1.2  92/01/11  16:06:13  usenet
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

#ifdef NGSEARCH
#define NGS_ABORT 0
#define NGS_FOUND 1
#define NGS_INTR 2
#define NGS_NOTFOUND 3
#define NGS_ERROR 4

EXT bool ng_doread INIT(FALSE);		/* search read newsgroups? */
#endif

void	ngsrch_init ANSI((void));
#ifdef NGSEARCH 
    int		ng_search ANSI((char *,int));
    bool	ng_wanted ANSI((void));
#endif
#ifdef NGSORONLY
    char	*ng_comp ANSI((COMPEX *,char *,bool_int,bool_int));
#endif
