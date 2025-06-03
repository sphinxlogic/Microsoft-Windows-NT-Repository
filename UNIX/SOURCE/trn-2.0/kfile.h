/* $Id: kfile.h,v 1.2 92/01/11 16:04:49 usenet Exp $
 *
 * $Log:	kfile.h,v $
 * Revision 1.2  92/01/11  16:04:49  usenet
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

#define KF_GLOBAL 0
#define KF_LOCAL 1

#ifdef KILLFILES
EXT FILE *globkfp INIT(Nullfp);		/* global article killer file */
EXT FILE *localkfp INIT(Nullfp);	/* local (for this newsgroup) */
					/*  article killer file */
#endif

void	kfile_init ANSI((void));
int	do_kfile ANSI((FILE *,int));
void	kill_unwanted ANSI((ART_NUM,char *,int));
int	edit_kfile ANSI((void));
void	open_kfile ANSI((int));
void    kf_append ANSI((char *));
void	setthru ANSI((ART_NUM));
