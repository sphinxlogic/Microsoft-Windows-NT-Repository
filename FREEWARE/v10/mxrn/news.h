#ifndef NEWS_H
#define NEWS_H

/*
 * $Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/news.h,v 1.4 1993/01/11 02:15:07 ricks Exp $
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


#ifndef AVL_H
#include "avl.h"
#endif

/*
 * news.h: internal data structures
 *
 */


typedef int art_num;	/* easy way to pick out variables refering to articles */
typedef short ng_num;	/* easy way to pick out newsgroup variables            */

extern avl_tree *NewsGroupTable;
extern int ActiveGroupsCount;
extern avl_tree *StringPool;

/* this is indexed via 'current - first' */
struct artStat {
    unsigned int status;
        /* ART_* */
};


/* this is indexed via 'current - first' */
struct article {
    int position;	/* header/body separation point (bytes)	*/
    char *subject;      /* subject line				*/
    char *author;       /* author name				*/
    char *lines;        /* number of lines in the article	*/
    char *text;		/* text of the article			*/
    char *msgid;	/* message ID				*/
    char *refs[3];	/* references				*/
    art_num parent;	/* article number of parent		*/
    art_num child;	/* first child of a chain		*/
};


struct newsgroup {
    char *name;        /* name of the group                                 */
    ng_num newsrc;     /* index of this group into Newsrc                   */
    unsigned short status;
        /* NG_* */
    art_num first;     /* first available article number                    */
    art_num last;      /* last article number                               */
    art_num current;   /* current article number                            */
    art_num initial;   /* art-num when first loaded			    */
    struct artStat *artStatus;/* List of per-article status info	    */
    struct list *nglist;  /* newsgroup entry for unsubscribed groups        */
};

/* handle first and/or last article number changes */
extern void articleArrayResync _ARGUMENTS((struct newsgroup *, art_num, art_num, int));
extern void updateArticleArray _ARGUMENTS((struct newsgroup *, int));

/*
 * NOTE: if first > last, that means that there are no articles available for
 * the group and no articles array will exist
 */


/* article has been read / is unread */
#define ART_READ      0x0001
#define ART_UNREAD    0x0002
/* printed */
#define ART_PRINTED   0x0004
#define ART_UNPRINTED 0x0008
/* article has been fetched / is unfetched */
#define ART_FETCHED   0x0010
#define ART_UNFETCHED 0x0020
/* article is not available */
#define ART_UNAVAIL   0x0040

/* headers */
#define ART_ALL_HEADERS      0x0100
#define ART_STRIPPED_HEADERS 0x0200

/* rotation */
#define ART_ROTATED   0x0400
#define ART_UNROTATED 0x0800

/* saved */
#define ART_SAVED   0x4000
#define ART_UNSAVED 0x8000

/* marked */
#define ART_MARKED   0x1000
#define ART_UNMARKED 0x2000

/* killed */
#define ART_KILLED   0x10000
#define ART_UNKILLED 0x20000

#define ART_ACTIVE   0x40000
#define ART_INACTIVE 0x80000

/* Translated */
#define ART_XLATED	0x100000
#define ART_UNXLATED	0x200000

#define ART_CLEAR       (ART_UNREAD | ART_UNFETCHED | ART_STRIPPED_HEADERS | ART_UNROTATED | ART_UNMARKED | ART_UNKILLED | ART_INACTIVE | ART_UNXLATED)
#define ART_CLEAR_READ  (ART_READ | ART_UNFETCHED | ART_STRIPPED_HEADERS | ART_UNROTATED | ART_UNMARKED | ART_UNKILLED | ART_INACTIVE | ART_UNXLATED)

/* helper macros */
#define IS_READ(art)       (((art).status & ART_READ) == ART_READ)
#define IS_UNREAD(art)     (((art).status & ART_UNREAD) == ART_UNREAD)
#define IS_PRINTED(art)    (((art).status & ART_PRINTED) == ART_PRINTED)
#define IS_UNPRINTED(art)  (((art).status & ART_UNPRINTED) == ART_UNPRINTED)
#define IS_FETCHED(art)    (((art).status & ART_FETCHED) == ART_FETCHED)
#define IS_UNFETCHED(art)  (((art).status & ART_UNFETCHED) == ART_UNFETCHED)
#define IS_UNAVAIL(art)    (((art).status & ART_UNAVAIL) == ART_UNAVAIL)
#define IS_ALL_HEADERS(art) (((art).status & ART_ALL_HEADERS) == ART_ALL_HEADERS)
#define IS_ROTATED(art)    (((art).status & ART_ROTATED) == ART_ROTATED)
#define IS_SAVED(art)      (((art).status & ART_SAVED) == ART_SAVED)
#define IS_MARKED(art)     (((art).status & ART_MARKED) == ART_MARKED)
#define IS_UNMARKED(art)   (((art).status & ART_UNMARKED) == ART_UNMARKED)
#define IS_KILLED(art)     (((art).status & ART_KILLED) == ART_KILLED)
#define IS_UNKILLED(art)   (((art).status & ART_UNKILLED) == ART_UNKILLED)
#define IS_ACTIVE(art)     (((art).status & ART_ACTIVE) == ART_ACTIVE)
#define IS_INACTIVE(art)   (((art).status & ART_INACTIVE) == ART_INACTIVE)
#define IS_XLATED(art)     (((art).status & ART_XLATED) == ART_XLATED)
  
  
#define SET_READ(art)	   ((art).status &= ~ART_UNREAD, (art).status |= ART_READ)
#define SET_UNREAD(art)    ((art).status &= ~ART_READ, (art).status |= ART_UNREAD)
#define SET_PRINTED(art) ((art).status &= ~ART_PRINTED, (art).status |= ART_PRINTED)
#define SET_UNPRINTED(art) ((art).status &= ~ART_UNPRINTED, (art).status |= ART_UNPRINTED)
#define SET_FETCHED(art)   ((art).status &= ~ART_UNFETCHED, (art).status |= ART_FETCHED)
#define SET_UNFETCHED(art) ((art).status &= ~ART_FETCHED, (art).status |= ART_UNFETCHED)
#define SET_STRIPPED_HEADERS(art) ((art).status &= ~ART_ALL_HEADERS, (art).status |= ART_STRIPPED_HEADERS)
#define SET_ALL_HEADERS(art) ((art).status &= ~ART_STRIPPED_HEADERS, (art).status |= ART_ALL_HEADERS)
#define SET_UNROTATED(art) ((art).status &= ~ART_ROTATED, (art).status |= ART_UNROTATED)
#define SET_ROTATED(art)   ((art).status &= ~ART_UNROTATED, (art).status |= ART_ROTATED)
#define SET_UNAVAIL(art)   ((art).status |= ART_UNAVAIL)
#define SET_AVAIL(art)     ((art).status &= ~ART_UNAVAIL)
#define SET_SAVED(art)     ((art).status &= ~ART_UNSAVED, (art).status |= ART_SAVED)
#define SET_MARKED(art)    ((art).status &= ~ART_UNMARKED, (art).status |= ART_MARKED)
#define SET_UNMARKED(art)  ((art).status &= ~ART_MARKED, (art).status |= ART_UNMARKED)
#define SET_KILLED(art)    ((art).status &= ~ART_UNKILLED, (art).status |= ART_KILLED)
#define SET_UNKILLED(art)  ((art).status &= ~ART_KILLED, (art).status |= ART_UNKILLED)
#define SET_ACTIVE(art)    ((art).status &= ~ART_INACTIVE, (art).status |= ART_ACTIVE)
#define SET_INACTIVE(art)  ((art).status &= ~ART_ACTIVE, (art).status |= ART_INACTIVE)
#define SET_UNXLATED(art) ((art).status &= ~ART_XLATED, (art).status |= ART_UNXLATED)
#define SET_XLATED(art) ((art).status &= ~ART_UNXLATED, (art).status |= ART_XLATED)

#define CLEAR_TEXT(art) \
  if ((art).text != NIL(char)) { \
      FREE((art).text); \
      (art).text = NIL(char); \
  }	

#define CLEAR_SUBJECT(art) (art).subject = NIL(char) 

#define CLEAR_AUTHOR(art) (art).author = NIL(char)

#define CLEAR_LINES(art) (art).lines = NIL(char)
 
#define CLEAR_MSGID(art) (art).msgid = NIL(char)

#define CLEAR_REFS(art) \
	(art).refs[0] = NIL(char); \
	(art).refs[1] = NIL(char); \
	(art).refs[2] = NIL(char);

/* newsgroup is subscribed/unsubscribed to */
#define NG_SUB      0x0001
#define NG_UNSUB    0x0002
/* no entry in the .newsrc for this group */
#define NG_NOENTRY  0x0004
/* newsgroup can be posted to / can not be posted to / is moderated */
#define NG_POSTABLE 0x0008
#define NG_UNPOSTABLE 0x0010
#define NG_MODERATED 0x0020
/* newsgroup is an alias */
#define NG_ALIAS    0x0040
#define NG_ALIASSEEN 0x0080

#define IS_SUBSCRIBED(ng) (((ng)->status & NG_SUB) == NG_SUB)
#define IS_NOENTRY(ng)    (((ng)->status & NG_NOENTRY) == NG_NOENTRY)
#define IS_ALIAS(ng)	  (((ng)->status & NG_ALIAS) == NG_ALIAS)
#define IS_NOTALIAS(ng)	  (((ng)->status & NG_ALIAS) == 0)
  
#define SET_SUB(ng)    ((ng)->status &= ~NG_UNSUB, (ng)->status |= NG_SUB)
#define SET_UNSUB(ng)  ((ng)->status &= ~NG_SUB, (ng)->status |= NG_UNSUB)
#define CLEAR_NOENTRY(ng) ((ng)->status &= ~NG_NOENTRY)

#define EMPTY_GROUP(ng) ((ng)->last < (ng)->first || (ng)->last == 0)
  
#define CLEAR_ARTICLES(ng) currentArticles = NIL(struct article)

#define CLEAR_ARTSTATUS(ng) \
  if ((ng)->artStatus != NIL(struct artStat)) { \
      FREE((ng)->artStatus); \
      (ng)->artStatus = NIL(struct artStat); \
  }	

#define INDEX(artnum)  (artnum - newsgroup->first)
#define CURRENT        INDEX(newsgroup->current)
#define LAST           INDEX(newsgroup->last)

extern struct newsgroup *CurrentGroup;	/* Current group */

extern ng_num MaxGroupNumber;       /* size of the newsrc array                  */

extern struct newsgroup **Newsrc;   /* sequence list for .newsrc file            */

#define NOT_IN_NEWSRC -1            /* must be less than 0 */

/* not a valid group (must be less than 0) */
#define NO_GROUP -1


#define GROUP_NAME_SIZE 128

#endif /* NEWS_H */
