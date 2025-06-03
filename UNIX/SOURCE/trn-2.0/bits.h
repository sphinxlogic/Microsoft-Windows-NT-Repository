/* $Id: bits.h,v 1.2 92/01/11 16:04:01 usenet Exp $
 *
 * $Log:	bits.h,v $
 * Revision 1.2  92/01/11  16:04:01  usenet
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

EXT char *ctlarea INIT(Nullch);	/* one bit for each article in current newsgroup */
			/* with the following interpretation: */
			/*	0 => unread  */
			/*	1 => read    */

/* if subscripting is faster than shifting on your machine, define this */
#undef USESUBSCRIPT
#ifdef USESUBSCRIPT
EXT char powerof2[] INIT({1,2,4,8,16,32,64,128});
#define pow2(x) powerof2[x]
#else
#define pow2(x) (1 << (x))
#endif

#ifdef lint
EXT bool nonesuch INIT(FALSE);
#define ctl_set(a)
#define ctl_clear(a)
#define ctl_read(a) nonesuch
#define was_read(a) nonesuch
#else
#define ctl_set(a) (ctlarea[(OFFSET(a)) / BITSPERBYTE] |= pow2((OFFSET(a)) % BITSPERBYTE))
#define ctl_clear(a) (ctlarea[(OFFSET(a)) / BITSPERBYTE] &= ~pow2((OFFSET(a)) % BITSPERBYTE))
#define ctl_read(a) ((ctlarea[(OFFSET(a)) / BITSPERBYTE] & pow2((OFFSET(a)) % BITSPERBYTE)) != 0)

#define was_read(a) ((a)<firstbit || ctl_read(a))
#endif /* lint */

EXT ART_NUM absfirst INIT(0);	/* 1st real article in current newsgroup */
EXT ART_NUM firstart INIT(0);	/* minimum unread article number in newsgroup */
EXT ART_NUM firstbit INIT(0);	/* minimum valid bit, usually == firstart */
EXT ART_NUM lastart INIT(0);	/* maximum article number in newsgroup */

#ifdef DELAYMARK
EXT FILE *dmfp INIT(Nullfp);
EXT char *dmname INIT(Nullch);
EXT int dmcount INIT(0);
#endif

void	bits_init ANSI((void));
void	checkpoint_rc ANSI((void));
void	restore_ng ANSI((void));
void	onemore ANSI((ART_NUM));
void	oneless ANSI((ART_NUM));
void	unmark_as_read ANSI((void));
#ifdef USETHREADS
void	set_read ANSI((ART_NUM,int,bool_int));
void	set_unread ANSI((ART_NUM,int));
#endif
void	delay_unmark ANSI((ART_NUM));
void	mark_as_read ANSI((void));
void	check_first ANSI((ART_NUM));
#ifdef DELAYMARK
    void	yankback ANSI((void));
#endif
int	chase_xrefs ANSI((ART_NUM,int));
int	initctl ANSI((void));
void	grow_ctl ANSI((ART_NUM));
