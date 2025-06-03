/* $Id: backpage.h,v 1.2 92/01/11 16:03:57 usenet Exp $
 *
 * $Log:	backpage.h,v $
 * Revision 1.2  92/01/11  16:03:57  usenet
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

/* things for doing the 'back page' command */

EXT int varyfd INIT(0);			/* virtual array file for storing  */
					/* file offsets */
EXT ART_POS varybuf[VARYSIZE];		/* current window onto virtual array */

EXT long oldoffset INIT(-1);		/* offset to block currently in window */

void	backpage_init ANSI((void));
ART_POS	vrdary ANSI((ART_LINE));
void	vwtary ANSI((ART_LINE,ART_POS));
