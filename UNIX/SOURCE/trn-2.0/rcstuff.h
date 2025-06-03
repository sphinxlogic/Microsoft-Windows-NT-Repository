/* $Id: rcstuff.h,v 1.2 92/01/11 16:30:18 usenet Exp $
 *
 * $Log:	rcstuff.h,v $
 * Revision 1.2  92/01/11  16:30:18  usenet
 * Lots of bug fixes:
 * 
 *    o    More upgrade notes for trn 1.x users (see ** LOOK ** in NEW).
 *    o    Enhanced the article-reading code to not remember our side-trip
 *         to the end of the group between thread selections (to fix '-').
 *    o    Extended trrn's handling of new articles (we fetch the active file
 *         more consistently, and listen to what the GROUP command tells us).
 *    o    Enhanced the thread selector to mention when new articles have
 *         cropped up since the last visit to the selector.
 *    o    Changed strftime to use size_t and added a check for size_t in
 *         Configure to make sure it is defined.  Also made it a bit more
 *         portable by using gettimeofday() and timezone() in some instances.
 *    o    Fixed a problem with the "total" structure not getting zero'ed in
 *         mthreads (causing bogus 'E'rrors on null groups).
 *    o    Fixed a reference to tmpbuf in intrp.c that was bogus.
 *    o    Fixed a problem with using N and Y with the newgroup code and then
 *         trying to use the 'a' command.
 *    o    Fixed an instance where having trrn get ahead of the active file
 *         might declare a group as being reset when it wasn't.
 *    o    Reorganized the checks for Apollo's C library to make sure it doesn't
 *         find the Domain OS version.
 *    o    Added a check for Xenix 386's C library.
 *    o    Made the Configure PATH more portable.
 *    o    Fixed the arguments prototypes to int_catcher() and swinch_catcher().
 *    o    Fixed the insert-my-subject-before-my-sibling code to not do this
 *         when the sibling's subject is the same as the parent.
 *    o    Fixed a bug in the RELAY code (which I'm suprised is still being used).
 *    o    Twiddled the mthreads.8 manpage.
 *    o    mthreads.8 is now installed and the destination is prompted for in
 *         Configure, since it might be different from the .1 destination.
 *    o    Fixed a typo in newsnews.SH and the README.
 * 
 * Revision 4.4.2.1  1991/12/01  18:05:42  sob
 * Patchlevel 2 changes
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

EXT char **rcline INIT(NULL);/* pointers to lines of .newsrc */
EXT ART_UNREAD *toread INIT(NULL);
			/* number of articles to be read in newsgroup */
			/* <0 => invalid or unsubscribed newsgroup */
#define TR_ONE ((ART_UNREAD) 1)
#define TR_NONE ((ART_UNREAD) 0)
#define TR_UNSUB ((ART_UNREAD) -1)
			/* keep this one as -1, some tests use >= TR_UNSUB */
#define TR_BOGUS ((ART_UNREAD) -2)
#define TR_JUNK ((ART_UNREAD) -3)

#ifdef USETHREADS
#define RCCHAR(ch) ((ch) == '0' ? ':' : (ch))
#endif

EXT char *rcchar INIT(NULL); /* holds the character : or ! while spot is \0 */
EXT char *rcnums INIT(NULL); /* offset from rcline to numbers on line */
EXT ACT_POS *softptr INIT(NULL);
			/* likely ptr to active file entry for newsgroup */
EXT bool paranoid INIT(FALSE);	/* did we detect some inconsistency in .newsrc? */
EXT int maxrcline INIT(0);	/* current maximum # of lines in .newsrc */
EXT int addnewbydefault INIT(0);

bool	rcstuff_init ANSI((void));
bool	get_ng ANSI((char *,bool_int));	/* return TRUE if newsgroup can be found or added */
NG_NUM	add_newsgroup ANSI((char *,char_int));
#ifdef RELOCATE
    NG_NUM	relocate_newsgroup ANSI((NG_NUM,NG_NUM));/* move newsgroup around */
#endif
void	list_newsgroups ANSI((void));
NG_NUM	find_ng ANSI((char *));	/* return index of newsgroup */
void	cleanup_rc ANSI((void));
void	sethash ANSI((NG_NUM));
int	hash ANSI((char *));
void	newsrc_check ANSI((void));
void	write_rc ANSI((void));
void	get_old_rc ANSI((void));
