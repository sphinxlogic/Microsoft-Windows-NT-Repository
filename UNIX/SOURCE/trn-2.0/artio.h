/* $Id: artio.h,v 1.2 92/01/11 16:03:44 usenet Exp $
 *
 * $Log:	artio.h,v $
 * Revision 1.2  92/01/11  16:03:44  usenet
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

EXT ART_POS artpos INIT(0);	/* byte position in article file */

EXT ART_LINE artline INIT(0);		/* current line number in article file */
EXT FILE *artfp INIT(Nullfp);		/* current article file pointer */
EXT ART_NUM openart INIT(0);		/* what is the currently open article number? */
#ifdef LINKART
    EXT char *linkartname INIT(nullstr);/* real name of article for Eunice */
#endif
#ifdef SERVER
EXT ART_PART openpart INIT(0);	/* how much of that article do we have? */
FILE	*nntpopen ANSI((ART_NUM,ART_PART));
				/* get an article unless already retrieved */
void	nntpclose ANSI((void));
/* MUST be in increasing order of completeness! */
#define GET_STATUS	1		/* test for existence only */
#define GET_HEADER	2		/* if only header items are desired */
#define GET_ARTICLE	3		/* retreive full article  */
#endif

void	artio_init ANSI((void));
FILE	*artopen ANSI((ART_NUM));  /* open an article unless already opened */
