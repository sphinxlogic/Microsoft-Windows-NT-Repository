/* $Id: sw.h,v 1.2 92/01/11 16:08:45 usenet Exp $
 *
 * $Log:	sw.h,v $
 * Revision 1.2  92/01/11  16:08:45  usenet
 * header twiddling, bug fixes
 * 
 * Revision 4.4  1991/09/09  20:27:37  sob
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

#ifdef INNERSEARCH
EXT int gline INIT(0);
#endif

void    sw_init ANSI((int,char **,char **));
void    sw_file ANSI((char **,bool_int));
void    sw_list ANSI((char *));
void	decode_switch ANSI((char *));
void	pr_switches ANSI((void));
void	cwd_check ANSI((void));
