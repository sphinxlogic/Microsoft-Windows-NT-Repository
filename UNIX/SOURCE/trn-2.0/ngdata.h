/* $Id: ngdata.h,v 1.3 92/01/11 16:29:45 usenet Exp $
 *
 * $Log:	ngdata.h,v $
 * Revision 1.3  92/01/11  16:29:45  usenet
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
 * Revision 1.2  92/01/11  16:06:09  usenet
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

EXT FILE *actfp INIT(Nullfp);	/* the active file */
EXT bool writesoft INIT(FALSE);	/* rewrite the soft pointer file? */
EXT int softtries INIT(0), softmisses INIT(0);

#ifdef SERVER
    EXT char active_name[MAXFILENAME];
    EXT time_t lastactfetch INIT(0);
#define MINFETCHTIME (5 * 60)
#endif

#ifdef CACHEFIRST
    EXT ART_NUM *abs1st INIT(NULL);	/* 1st real article in newsgroup */
#else
# ifdef MININACT
    EXT ART_NUM abs1st INIT(0);
# endif
#endif

EXT char *moderated;
#ifdef USETHREADS
EXT char *tmpthread_group INIT(Nullch);
EXT int tmpthread_glen INIT(0);
EXT char *tmpthread_file INIT(Nullch);
EXT bool ThreadedGroup;
#endif
EXT long activeitems;			/* number of enties in active file */
void	ngdata_init ANSI((void));
ART_NUM	getngsize ANSI((NG_NUM));
ACT_POS findact ANSI((char *,char *,int,long));
ART_NUM	getabsfirst ANSI((NG_NUM,ART_NUM));
ART_NUM	getngmin ANSI((char *,ART_NUM));
