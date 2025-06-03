/* $Id: search.h,v 1.2 92/01/11 16:08:37 usenet Exp $
 *
 * $Log:	search.h,v $
 * Revision 1.2  92/01/11  16:08:37  usenet
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

#ifndef NBRA
#define	NBRA	10		/* the maximum number of meta-brackets in an
				   RE -- \( \) */
#define NALTS	10		/* the maximum number of \|'s */
 
typedef struct {	
    char *expbuf;		/* The compiled search string */
    int eblen;			/* Length of above buffer */
    char *alternatives[NALTS];	/* The list of \| seperated alternatives */
    char *braslist[NBRA];	/* RE meta-bracket start list */
    char *braelist[NBRA];	/* RE meta-bracket end list */
    char *brastr;		/* saved match string after execute() */
    char nbra;			/* The number of meta-brackets int the most
				   recenlty compiled RE */
    bool do_folding;		/* fold upper and lower case? */
} COMPEX;

void	search_init ANSI((void));
void	init_compex ANSI((COMPEX *));
void	free_compex ANSI((COMPEX *));
char	*getbracket ANSI((COMPEX *,int));
void	case_fold ANSI((int));
char	*compile ANSI((COMPEX *,char *,int,int)); 
void	grow_eb ANSI((COMPEX *));
char	*execute ANSI((COMPEX *,char *)); 
bool	advance ANSI((COMPEX *,char *,char *));
bool	backref ANSI((COMPEX *,int,char *)); 
bool	cclass ANSI((char *,int,int));
#endif
