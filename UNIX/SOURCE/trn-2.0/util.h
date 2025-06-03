/* $Id: util.h,v 1.3 92/01/11 16:31:43 usenet Exp $
 *
 * $Log:	util.h,v $
 * Revision 1.3  92/01/11  16:31:43  usenet
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
 * Revision 1.2  92/01/11  16:10:41  usenet
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

EXT bool waiting INIT(FALSE);	
			/* are we waiting for subprocess (in doshell)? */
EXT int len_last_line_got INIT(0);
			/* strlen of some_buf after */
			/*  some_buf = get_a_line(bufptr,buffersize,fp) */

/* is the string for makedir a directory name or a filename? */

#define MD_DIR 	0
#define MD_FILE 1

void	util_init ANSI((void));
int	doshell ANSI((char *,char *));
char	*safemalloc ANSI((MEM_SIZE));
char	*saferealloc ANSI((char *,MEM_SIZE));
char	*safecpy ANSI((char *,char *,int));
char	*safecat ANSI((char *,char *,int));
char	*cpytill ANSI((char *,char *,int));
char	*instr ANSI((char *,char *, int));
#ifdef SETUIDGID
    int		eaccess ANSI((char *,int));
#endif
char	*getwd ANSI((char *));
char	*get_a_line ANSI((char *,int,FILE *));
char	*savestr ANSI((char *));
int	makedir ANSI((char *,int));
void	setenv ANSI((char *,char *));
int	envix ANSI((char *));
void	notincl ANSI((char *));
char	*getval ANSI((char *,char *));
void	growstr ANSI((char **,int *,int));
void	setdef ANSI((char *,char *));
#ifdef SERVER
int	nntp_get ANSI((char *, int));
#endif
#ifndef FOUNDSIZET
#define size_t int
#endif
#ifndef STRFTIME
size_t	strftime ANSI((char *, size_t, char *, struct tm *));
#endif
