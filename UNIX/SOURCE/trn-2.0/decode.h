/* $Id: decode.h,v 1.2 92/01/11 16:04:13 usenet Exp $
 *
 * $Log:	decode.h,v $
 * Revision 1.2  92/01/11  16:04:13  usenet
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
EXT FILE *decode_fp INIT(NULL);
EXT char decode_fname[MAXFILENAME];
EXT char decode_dest[MAXFILENAME];
EXT int decode_type;

void decode_init ANSI((void));
void decode_end ANSI((void));

void uud_start ANSI((void));
int uudecode ANSI((FILE *));

void unship_init ANSI((void));
int unship ANSI((FILE *));

#define UUDECODE 0
#define UNSHIP   1
