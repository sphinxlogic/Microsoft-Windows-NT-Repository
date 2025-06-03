#ifndef INTERNALS_H
#define INTERNALS_H

/*
 * $Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/internals.h,v 1.4 1993/01/11 02:15:00 ricks Exp $
 */

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988-1993, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/*
 * internals.h: news system interface
 */

/*
 * Get datatype definitions we need for prototyping
 */
#include "news.h"

#define XRN_ERROR 0
#define XRN_NOMORE 0
#define XRN_OKAY   1

/* 
 * kill file stuff
 */
#define KILL_GLOBAL 0
#define KILL_LOCAL 1
extern void killItem _ARGUMENTS((char *, int));
extern char *localKillFile _ARGUMENTS((struct newsgroup*, int));
extern char *globalKillFile ();
extern void killThread _ARGUMENTS((int));

/*
 * routines for adding newsgroups to the newsrc file
 */
 
/* 'status' values for the 'add' functions */
#define SUBSCRIBE 1
#define UNSUBSCRIBE 0
                                                          
extern int addToNewsrcAfterGroup _ARGUMENTS((char *, char *, int));
extern int addToNewsrcEnd _ARGUMENTS((char *, int));
extern int addToNewsrcBeginning _ARGUMENTS((char *, int));


/*
 * routines for doing newsgroup management
 */

/* jump/goto/add newsgroup codes */
#define BAD_GROUP -1
#define GOOD_GROUP 1
#define GOOD_GROUP_MARKED 2

/* set the internal group to 'name', with prefetch invalidation */
extern int gotoNewsgroup _ARGUMENTS((char *));
extern int gotoNewsgroupForRead _ARGUMENTS((char *));
/* same thing, but without prefetch invalidation */
extern int setNewsgroup _ARGUMENTS((char *));

/* set the internal group to 'name', and if all of the articles are read, unmark the last one */
extern int jumpToNewsgroup _ARGUMENTS((char *));

/* getNewsgroup returns 0 if a bad group, 1 if okay */
extern int getNewsgroup();

/* subscribe to the current newsgroup */
extern void subscribe();
/* unsubscribe to the current newsgroup */
extern void unsubscribe();

/* check subscription status */
extern int issubscribed();

/* updates the .newsrc file so that the current newsgroup is marked as all read */
extern void catchUpAll();
extern void catchUp();


/*
 * routines for doing article management
 */

extern void gotoArticle _ARGUMENTS((art_num));
extern void gotoFirstArticle();

extern int getArticleText _ARGUMENTS((char **, char **));
extern int toggleHeaders _ARGUMENTS((char **, char **));
extern int toggleRotation _ARGUMENTS((char **, char **));
extern char *openQuestion();
extern char *currentQuestion();

/* mark articles */
extern void markArticleAsRead _ARGUMENTS((art_num));
extern void markArticleAsUnread _ARGUMENTS((art_num));
extern void markArticleAsKilled _ARGUMENTS((art_num));
extern void markArticleAsUnkilled _ARGUMENTS((art_num));

/* get a single subject line, stripped of leading/trailing spaces, Re: */
extern char *getSubject _ARGUMENTS((art_num));
extern char *getAuthor _ARGUMENTS((art_num));
extern char *getText _ARGUMENTS((art_num));
extern char *getSubjectLine _ARGUMENTS((art_num));

extern Boolean articleRead _ARGUMENTS((art_num));
extern Boolean articleMarkedUnread _ARGUMENTS((art_num));

/* get the subject line for the previous subject (and get the article too) */
/* only called when going off the top of the subject string */
extern char *getPrevSubject();

extern void startSearch();
extern void failedSearch();

/*
 * information gathering routines
 */

extern void checkLock();
extern void removeLock();
  
/* read the .newsrc file, find out what to read */
extern void initializeNews();

/* query the server for new information */
extern void rescanServer();

/* return the new newsgroups string */
extern struct newsgroup **newGroups();

/* return the unread newsgroups list */
extern ng_num *unreadGroups _ARGUMENTS((int, Boolean *));

/* return the group status string */
extern char *groupStatus _ARGUMENTS((int));
extern char *groupName _ARGUMENTS((int));

/* return the subject string */
#define ALL 0
#define UNREAD 1
#define UNKILLED 2
#define ACTIVE 3

extern art_num *getArticleNumbers _ARGUMENTS((int, int *));
/* Types of subject sorting */
#define UNSORTED 0
#define ARTICLE_SORTED 1
#define STRICT_SORTED 2
#define THREAD_SORTED 3

/* build and return the status list */
extern ng_num *getStatusList _ARGUMENTS((int));
extern char *getStatusString _ARGUMENTS((int));

extern void bogusNewsgroup();

extern void releaseNewsgroupResources();
extern void clearArtCache();

extern void fillUpArray _ARGUMENTS((art_num));

#define GETARTICLES(newsgroup) (currentArticles) ? \
	(currentArticles) : getarticles(newsgroup)
struct article *getarticles _ARGUMENTS((struct newsgroup *));

#define GETSTATUS(newsgroup) (newsgroup->artStatus) ? \
	(newsgroup->artStatus) : getartstatus(newsgroup)

struct artStat *getartstatus _ARGUMENTS((struct newsgroup *));

extern int checkArticle _ARGUMENTS((art_num));
#ifdef XLATE
extern int toggleXlation _ARGUMENTS((char **, char **));
#endif

#endif /* INTERNALS_H */
