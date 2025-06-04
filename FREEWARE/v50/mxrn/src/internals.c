
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/internals.c,v 1.9 1993/02/04 18:22:19 ricks Exp $";
#endif

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
 * internals.c: routines for handling the internal data structures
 *    calls server routines, is calls by the user interface code
 *
 */

#include "copyright.h"
#ifdef VMS
#include <jpidef.h>
#include <ssdef.h>
#ifdef MOTIF
#include <X11/Xos.h>
#else
#include <decw$include/Xos.h>
#endif
#endif
#include <stdio.h>
#include <errno.h>
#include "config.h"
#include "utils.h"
#include <assert.h>
#ifndef VMS
#include <sys/param.h>
#include <sys/file.h>
#include <sys/stat.h>
#else
#define index strchr
#include <file.h>
#include <ctype.h>
#include <starlet.h>
#endif /* VMS */
#ifdef __STDC__
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#endif /* __STDC__ */
#include "avl.h"
#include "news.h"
#include "newsrcfile.h"
#include "resources.h"
#include "server.h"
#include "mesg.h"
#include "error_hnds.h"
#include "save.h"
#include "internals.h"
#include "xrn.h"

#ifdef SYSV_REGEX
    extern char *regcmp();
    extern char *regex();
#else
    extern char *re_comp();
#endif

#if !defined(hpux) && !defined(linux) && !defined(VMS) && defined(__STDC__)
extern int gethostname _ARGUMENTS((char *, int));
#endif

#define BUFFER_SIZE 1024
#define FATAL 0
#define OKAY  1
#define MAX_GROUP_SIZE 100

extern void stop_server();
extern Boolean watchingGroup();
struct article *currentArticles = NULL;	/* List of article data pointers */
struct article *savedArt = NULL;
int artCount = 0;		/* Count of entries available */
static char subjectline[BUFFER_SIZE];

#define SETARTICLES(newsgroup) if (currentArticles == 0) (void) getarticles(newsgroup)
#define SETSTATUS(newsgroup) if (newsgroup->artStatus == 0) (void) getartstatus(newsgroup)
static char *strip();

/*
 * length of lines in the top text window
 */
#define LINE_LENGTH 512

/*
 * next article to get when faulting in articles that are of the top
 * of the subject screen.
 */
static art_num NextPreviousArticle;
  
/* storage for all newsgroups, key is the name of the group */
avl_tree *NewsGroupTable = NIL(avl_tree);

/* storage for newsgroup message ids */
static avl_tree *msgids = NIL(avl_tree);

/* number of groups in the active file */
int ActiveGroupsCount = 0;

/* number of the group currently being processed */
struct newsgroup * CurrentGroup = 0;

/* number of groups in the .newsrc file */
ng_num MaxGroupNumber = 0;

/* index number of last article read */
static int PrevArtIndx[5] = {-1,-1,-1,-1,-1};

/* Storage for shared string pool */
avl_tree *StringPool = NIL(avl_tree);

/* sprintf format string for article subjects */
static char formatString[LINE_LENGTH];

/*
 * see if another xrn is running
 */
void
checkLock()
{
    char *buffer = utTildeExpand(app_resources.lockFile);
#ifdef VMS
    char *ptr;
    struct _itemList {
	short item_size;
	short item_code;
	void *item_ptr;
	int   item_rtn_size;
	int   end_list;
    } itemList;
#endif /* VMS */
    char host[64];
    char myhost[64];
    int pid;
    FILE *fp;

    if (!buffer) {
	/* silently ignore this condition */
	return;
    }

#ifndef VMS
    if (gethostname(myhost, sizeof(myhost)) == -1) {
	(void) strcpy(myhost, "bogusHost");
    }
#else
    ptr = getenv("SYS$NODE");
    if (ptr != NIL(char)) {
	(void) strcpy(myhost, ptr);
	ptr = index(myhost, ':');
	if (ptr != NIL(char)) {
	    *ptr = '\0';
	}
    }

    if (*myhost == '\0') {
	(void) strcpy(myhost, "bogusHost");
    }
#endif /* VMS */

    if ((fp = fopen(buffer, "r")) == NULL) {
	if ((fp = fopen(buffer, "w")) == NULL) {
	    /* silently ignore this condition */
	    return;
	}
	(void) fprintf(fp, "%s %d\n", myhost, getpid());
	(void) fclose(fp);
	return;
    }

    (void) fscanf(fp, "%s %d", host, &pid);

#ifndef VMS
    /* see if I'm on the same host */
    if (strcmp(host, myhost) == 0) {
	if ((kill(pid, 0) == -1) && (errno == ESRCH)) {
	    /* hey, it's not running.... */
	    /* why do it right when you can just do this... */
	    removeLock();
	    checkLock();
	    return;
	}
    }
#else
    /* see if I'm on the same host */
    if (strcmp(host, myhost) == 0) {
	int status, pidTmp;
	/* whoa!  I am, see if the process is mine */
	if (pid == getpid()) {
	    /* It's mine.. so delete it */
	    /* why do it right when you can just do this... */
	    (void) fclose(fp);
	    removeLock();
	    checkLock();
	    return;
	}
	/* whoa!  I am, see if the process is running */
	itemList.item_size = 4;
	itemList.item_code = JPI$_PID;
	itemList.item_ptr = &pidTmp;
	itemList.item_rtn_size = 0;
	itemList.end_list = 0;

	pidTmp = 0;
	status = sys$getjpiw(0, &pid, 0, &itemList, 0, 0, 0);
	if (status == SS$_NONEXPR) {
	    /* hey, it's not running.... */
	    /* why do it right when you can just do this... */
	    removeLock();
	    checkLock();
	    return;
	}
    }
#endif

    (void) fprintf(stderr, "An XRN of yours is running on %s as process %d.\n",
		   host, pid);
    (void) fprintf(stderr, "If it is no longer running, remove %s\n",
		   buffer);
    exit(-1);

    return;
}


void
removeLock()
{
    char *buffer = utTildeExpand(app_resources.lockFile);

    if (buffer) {
	(void) unlink(buffer);
    }
    return;
}

int groupCompare(str1, str2)
#if defined(__STDC__) && __STDC__
const char *str1;
const char *str2;
#else
char *str1;
char *str2;
#endif
{
    if (str1 == NULL) return 1;
    if (str2 == NULL) return -1;
    return(strncmp(str1, str2, MAX_GROUP_SIZE));
}

/*
 * initialize the program and the news system
 *   read newsrc, etc
 *
 *   returns: void
 *
 */
void
initializeNews()
{
    start_server(app_resources.nntpServer);

    do {
	 NewsGroupTable = avl_init_table(groupCompare);
	 if (! NewsGroupTable) {
	     ehErrorExitXRN("out of memory");
	 }
    
	 getactive();
	 
	 if (readnewsrc(app_resources.newsrcFile,
			app_resources.saveNewsrcFile))
	      break;
	 
	 ehErrorRetryXRN("Can not read the .newsrc file", True);

	 avl_free_table(NewsGroupTable, XtFree, XtFree);
    } while (1);
	
#if !defined(FIXED_C_NEWS_ACTIVE_FILE) && !defined(FIXED_ACTIVE_FILE)
    fixLowArticleNumbers();
#endif
    return;
}

/*
 * get the active file again and grow the Newsrc array if necessary
 */
void
rescanServer()
{
    int old = ActiveGroupsCount;

    /* update the newsrc file */
    while (!updatenewsrc())
	ehErrorRetryXRN("Can not update the newsrc file", True);

#if !defined(NNTP_REREADS_ACTIVE_FILE)
    stop_server();
    start_server(NIL(char));
#endif
    
    getactive();

    if (ActiveGroupsCount > old) {
	/* new newsgroups were found, allocate a bigger array */
	Newsrc = (struct newsgroup **) XtRealloc((char *) Newsrc,
		(unsigned) (sizeof(struct newsgroup *) * ActiveGroupsCount));
    }

#if !defined(FIXED_C_NEWS_ACTIVE_FILE) && !defined(FIXED_ACTIVE_FILE)
    fixLowArticleNumbers();
#endif
    return;
}

#define lsDestroy(lst) \
    if (lst != NIL(struct list)) { \
	struct list *_next; \
	do { \
	    _next = lst->next; \
	    FREE(lst); \
	    lst = _next; \
	} while (lst != NIL(struct list)); \
    }

/*
 * accurately count the number of unread articles in a group
 *
 *   returns: the number of unread articles in the group
 */
int
unreadArticleCount(newsgroup)
struct newsgroup *newsgroup;
{
    art_num i;
    struct artStat *artstat = GETSTATUS(newsgroup);
    int count = 0;
    register struct artStat *ap;
    register art_num	last	= newsgroup->last;
    register art_num	artn;


    if (EMPTY_GROUP(newsgroup)) {
	return 0;
    }
    
    if ((newsgroup->first == 0) && (newsgroup->last == 0)) {
	return 0;
    }
    
    ap = &artstat[INDEX(newsgroup->first)];

    for (artn = newsgroup->first; artn <= last; artn++) {
	if (IS_UNREAD(*ap) && !IS_UNAVAIL(*ap)) {
	    count++;
	}
	ap++;
    }
    return count;
}

/*
 * accurately count the number of articles in a group
 *
 *   returns: the total number of articles in the group
 */
int
totalArticleCount(newsgroup)
struct newsgroup *newsgroup;
{
    art_num i;
    struct artStat *artstat = GETSTATUS(newsgroup);
    int count = 0;
    struct artStat *ap;
    art_num last = newsgroup->last;
  
    if (EMPTY_GROUP(newsgroup)) {
	return 0;
    }
    
    if ((newsgroup->first == 0) && (newsgroup->last == 0)) {
	return 0;
    }
    
    ap = &artstat[INDEX(newsgroup->first)];

    for (i = newsgroup->first; i <= last; i++) {
	if (!IS_UNAVAIL(*ap)) {
	    count++;
	}
	ap++;
    }
    return count;
}

/*
 * find the first (possibly unread) article in a group and set 'current' to it
 *
 * returns: void
 *
 */
static void
setCurrentArticle(newsgroup, mode)
struct newsgroup *newsgroup;
int mode;
{
    struct artStat *artstat = GETSTATUS(newsgroup);
    art_num i;
    int start;

    newsgroup->current = newsgroup->last + 1;
    
    /* if the resource 'onlyShow' is > 0, then mark all but the last
     * 'onlyShow' articles as read */

    start = newsgroup->first;
    if (app_resources.onlyShow > 0) {
	start = MAX(start, newsgroup->last - app_resources.onlyShow);
    }
    
    if (mode == UNREAD) {
	for (i = newsgroup->first; i<= newsgroup->last; i++) {
	    long indx = INDEX(i);
	    if (IS_UNREAD(artstat[indx]) && !IS_UNAVAIL(artstat[indx])) {
		newsgroup->initial = newsgroup->current = i;
		return;
	    }
	} 
    } 
    if (mode == UNKILLED) {
	if (newsgroup->initial == 0)
	    newsgroup->initial = newsgroup->first;
	for (i = newsgroup->initial; i<= newsgroup->last; i++) {
	    long indx = INDEX(i);
	    if (IS_UNKILLED(artstat[indx]) && IS_ACTIVE(artstat[indx]) &&
		!IS_UNAVAIL(artstat[indx])) {
		newsgroup->current = i;
		return;
	    }
	} 
    }

    if (mode == ACTIVE) {
	if (newsgroup->initial == 0)
	    newsgroup->initial = newsgroup->first;
	for (i = newsgroup->initial; i<= newsgroup->last; i++) {
	    long indx = INDEX(i);
	    if (IS_ACTIVE(artstat[indx]) && !IS_UNAVAIL(artstat[indx])) {
		newsgroup->current = i;
		return;
	    }
	} 
    }

    if (mode == ALL) {
	newsgroup->initial = newsgroup->first;
	for (i = newsgroup->initial; i<= newsgroup->last; i++) {
	    long indx = INDEX(i);
	    if (!IS_UNAVAIL(artstat[indx])) {
		newsgroup->initial = newsgroup->current = i;
		return;
	    }
	} 
    }

    return;
}

int
unreadNews()
{
    int i, count = 0;
    struct newsgroup *newsgroup;

    for (i = 0; i < MaxGroupNumber; i++) {
	newsgroup = Newsrc[i];
#ifdef WATCH
	if (IS_SUBSCRIBED(newsgroup) && watchingGroup(newsgroup->name)) {
#else
	if (IS_SUBSCRIBED(newsgroup)) {
#endif
	    count += unreadArticleCount(newsgroup);
	}
    }
    return count;
}

/*
 * build and return an array of information about groups that need to be read
 */
ng_num *
unreadGroups(mode, flag)
int mode;
Boolean *flag;
{
    struct newsgroup *newsgroup;
    ng_num i;
    int unread;
    int subscribedGroups = 0;
    ng_num *ar;

    ar = ARRAYALLOC(ng_num, MaxGroupNumber+1);

    *flag = False;

    for (i = 0; i < MaxGroupNumber; i++) {
	newsgroup = Newsrc[i];

	if (IS_SUBSCRIBED(newsgroup) &&
	   (((unread = unreadArticleCount(newsgroup)) > 0) || mode)) {
	    ar[subscribedGroups++] = i;
	    if (unread > 0) *flag = True;
	}
    }

    ar[subscribedGroups++] = -1;
    return ar;
}

/*
 * build and return a string describing the status of a group
 */
char *
groupStatus(which)
int which;
{
    static char dummy[LINE_LENGTH];
    struct newsgroup *newsgroup;
    int unread, total;

    dummy[0] = '\0';

    if (which < 0 || which > MaxGroupNumber)
	return dummy;

    newsgroup = Newsrc[which];
    total = totalArticleCount(newsgroup);
    unread = unreadArticleCount(newsgroup);

    (void) sprintf(dummy, "%6s %7s %-40s%5d article%s +%5d old",
			(unread > 0 ? "Unread" : ""),
			(total > 0 ? "news in" : ""),
			newsgroup->name, unread,
			((unread != 1) ? "s" : " "),
			total-unread);
    return dummy;
}
/*
 * return the name of a newsgroup
 */
char *
groupName(which)
int which;
{
    struct newsgroup *newsgroup;
    static char *dummy = ".";

    if (which < 0 || which > MaxGroupNumber)
	return dummy;

    newsgroup = Newsrc[which];
    return newsgroup->name;
}
/*
 * determine the newsgroups that are not in the .newsrc file
 *
 *   returns an array containing the newsgroup structures
 */
struct newsgroup **
newGroups()
{
    int count = 0;
    int i;
    avl_generator *gen;
    char *key, *value;
    struct newsgroup **ar;
    struct newsgroup *newsgroup;

    ar = ARRAYALLOC(struct newsgroup *, ActiveGroupsCount + 1);

    gen = avl_init_gen(NewsGroupTable, AVL_FORWARD);
    if (! gen) {
	ehErrorExitXRN("out of memory");
    }

    while (avl_gen(gen, &key, &value)) {
	newsgroup = (struct newsgroup *) value;
	if (IS_NOENTRY(newsgroup) && IS_NOTALIAS(newsgroup)) {
	    ar[count++] = newsgroup;
	}
    }

    avl_free_gen(gen);

    /* no new groups return */
    if (count == 0) {
	FREE(ar);	
	return NULL;
    }

    if (app_resources.unsubNewGroups) {
	for (i = 0; i < count; i++) {
	    newsgroup = ar[i];
	    addToNewsrcEnd(newsgroup->name, UNSUBSCRIBE);
	}
	FREE(ar);	
	return NULL;
    }

    ar[count] = NULL;
    return ar;
}

/*
 *   release some resources
 *
 *   returns: void
 */
void
releaseNewsgroupResources()
{
    int i;

    if (StringPool != NULL) {
	avl_free_table(StringPool, XtFree, NULL);
    }
    StringPool = avl_init_table((void *)strcmp);

    for (i = 0; i < 5; i++) 
	PrevArtIndx[i] = -1;
    /*
     * CurrentGroup can be invalid when called from the
     * signal catcher
     */
    if (CurrentGroup != NIL(struct newsgroup) ) {
	struct newsgroup *newsgroup = CurrentGroup;
	struct article *articles = currentArticles;
	struct artStat *artstat = newsgroup->artStatus;
	art_num art;

	newsgroup->initial = 0;
	if ((newsgroup->last == 0) || EMPTY_GROUP(newsgroup)) {
	    return;
	}

	for (art = newsgroup->first; art <= newsgroup->last; art++) {
	    long indx = INDEX(art);
	    
	    if (articles) {
		CLEAR_SUBJECT(articles[indx]);
		CLEAR_AUTHOR(articles[indx]);
		CLEAR_LINES(articles[indx]);
		CLEAR_TEXT(articles[indx]);
		CLEAR_MSGID(articles[indx]);
		CLEAR_REFS(articles[indx]);
	    }
	    if (artstat) {
		SET_UNFETCHED(artstat[indx]);
		SET_UNMARKED(artstat[indx]);
	    }
	}
	/* free the articles array every time */
	CLEAR_ARTICLES(newsgroup);
    }

    if (app_resources.updateNewsrc == TRUE) {
	if (!updatenewsrc()) {
	    ehErrorExitXRN("Can not update the newsrc file");
	}
    }
    CurrentGroup = NULL;
    currentArticles = NULL;

    return;
}

void clearArtCache()
{
    int i;
    for (i = 0; i < 5; i++)
	PrevArtIndx[i] = -1;
    return;
}

/*
 * update an article array if the first and/or last article numbers change
 *
 *  returns: void
 */
void
articleArrayResync(newsgroup, first, last, number)
struct newsgroup *newsgroup;
art_num first, last;
int number;
{
    struct article *articles = currentArticles;
    struct article *newarticles;
    struct artStat *artstatus = newsgroup->artStatus;
    struct artStat *newartstatus;
    int i;
    int newsize;

    /*
     * if there are actually no articles in the group, free up the
     * article array and set the first/last values
     */
       
    if (number == 0) {
	CLEAR_ARTICLES(newsgroup);
	CLEAR_ARTSTATUS(newsgroup);
	newsgroup->first = newsgroup->last + 1;
	return;
    }
    
    /* refuse to resync if last == 0 */
    if (last == 0) {
	return;
    }

    if (first > last) {
	/* all articles have been removed */
	CLEAR_ARTICLES(newsgroup);
	CLEAR_ARTSTATUS(newsgroup);
	newsgroup->first = newsgroup->last + 1;
	return;
    }

    /* don't allow last to go backwards!!! */
    if (last < newsgroup->last) {
	last = newsgroup->last;
    }

    /* don't allow first to go backwards!!! */
    if (first < newsgroup->first) {
	first = newsgroup->first;
    }
	
    if ((first != newsgroup->first) || (last != newsgroup->last)) {

	/* only do this if the articles array has been allocated */
	if ((newsgroup == CurrentGroup) && currentArticles) {

	    /* the first/last values have changed, resync */

	    articles = currentArticles;

	    newsize = (last - first) + 10;
	    if (newsize > artCount) {
		newsize = MAX(newsize, 1000);
		savedArt = newarticles = ARRAYALLOC(struct article, newsize);
		artCount = newsize;
	    } else {
		newarticles = savedArt;
	    }

	    /*
	     * initialize the new article structures
	     */
	
	    for (i = first; i <= last; i++) {
		newarticles[i - first].subject = NIL(char);
		newarticles[i - first].author = NIL(char);
		newarticles[i - first].text = NIL(char);
		newarticles[i - first].lines = NIL(char);
		newarticles[i - first].msgid = NIL(char);
		newarticles[i - first].refs[0] = NIL(char);
		newarticles[i - first].refs[1] = NIL(char);
		newarticles[i - first].refs[2] = NIL(char);
		newarticles[i - first].parent = 0;
		newarticles[i - first].child = 0;
	    }

	    if ((!EMPTY_GROUP(newsgroup)) &&
		(newsgroup->first != 0) && (newsgroup->last != 0) &&
		(articles != NIL(struct article))) {
		for (i = first; i <= last; i++) {
		    if ((i >= newsgroup->first) && (i <= newsgroup->last)) {
			newarticles[i - first] = articles[INDEX(i)];
		    }
		}

		/* free up the old resources - before the new first */
		for (i = newsgroup->first; i < first; i++) {
		    long indx = INDEX(i);
		    CLEAR_SUBJECT(articles[indx]);
		    CLEAR_AUTHOR(articles[indx]);
		    CLEAR_LINES(articles[indx]);
		    CLEAR_MSGID(articles[indx]);
		    CLEAR_REFS(articles[indx]);
		    CLEAR_TEXT(articles[indx]);
		}
		/* after the old last to the new last */
		for (i = newsgroup->last + 1; i <= last; i++) {
		    newarticles[i - first].subject = NIL(char);
		    newarticles[i - first].author = NIL(char);
		    newarticles[i - first].text = NIL(char);
		    newarticles[i - first].lines = NIL(char);
		    newarticles[i - first].msgid = NIL(char);
		    newarticles[i - first].refs[0] = NIL(char);
		    newarticles[i - first].refs[1] = NIL(char);
		    newarticles[i - first].refs[2] = NIL(char);
		    newarticles[i - first].parent = 0;
		    newarticles[i - first].child = 0;
		}
	    }

	    if (articles != NIL(struct article) &&
		articles != savedArt) {
		FREE(articles);
	    }
	    currentArticles = newarticles;
	}

	if (newsgroup->artStatus) {

	    /* the first/last values have changed, resync */

	    artstatus = newsgroup->artStatus;
	    newartstatus = ARRAYALLOC(struct artStat, (last - first) + 10);

	    /*
	     * initialize the new article status
	     */
	
	    for (i = first; i <= last; i++) {
		if (i < newsgroup->first) {
		    /* handle first decreasing... mark them as read */
		    newartstatus[i - first].status = ART_CLEAR_READ;
		} else {
		    newartstatus[i - first].status = ART_CLEAR;
		}
	    }

	    if ((!EMPTY_GROUP(newsgroup)) &&
		(newsgroup->first != 0) && (newsgroup->last != 0) &&
		(newsgroup->artStatus != NIL(struct artStat))) {
		for (i = first; i <= last; i++) {
		    if ((i >= newsgroup->first) && (i <= newsgroup->last)) {
			newartstatus[i - first] = artstatus[INDEX(i)];
		    }
		}
	    }
	    if (artstatus != NIL(struct artStat)) {
		FREE(artstatus);
	    }
	    newsgroup->artStatus = newartstatus;
	}
	newsgroup->first = first;
	newsgroup->last = last;
    }
    return;
}


char *
localKillFile(newsgroup, mode)
struct newsgroup *newsgroup;
int mode;
{
    static char buffer[BUFFER_SIZE];
    char *ptr;
    int i;

    if (!createNewsDir()) {
	return NIL(char);
    }

    (void) strcpy(buffer, app_resources.expandedSaveDir);
    i = strlen(buffer);

#ifndef VMS
    buffer[i++] = '/';

    for (ptr = newsgroup->name; *ptr != 0; ptr++) {
	if (*ptr == '.') {
	    if (mode) {
		buffer[i] = '\0';
		(void) mkdir(buffer, 0777);
	    }
	    buffer[i] = '/';
	    i++;
	} else {
	    buffer[i++] = *ptr;
	}
    }
    buffer[i] = '\0';
    if (mode) {
	(void) mkdir(buffer, 0777);
    }
    (void) strcpy(&buffer[i], "/KILL");
#else
    i = i + utGroupToVmsFilename(&buffer[i], newsgroup->name);
    (void) strcpy(&buffer[i], ".KILL");
#endif

    return buffer;
}


char *
globalKillFile()
{
    static char buffer[BUFFER_SIZE];

    if (!createNewsDir()) {
	return NIL(char);
    }
    (void) strcpy(buffer, app_resources.expandedSaveDir);
#ifndef VMS
    (void) strcat(buffer, "/KILL");
#else
    (void) strcat(buffer, "NEWS$GLOBAL.KILL");
#endif
    return buffer;
}

/*
 * mark a thread as killed
 */
void
killThread(artNum)
art_num artNum;
{
    struct newsgroup *newsgroup = CurrentGroup;
    struct article *articles = GETARTICLES(newsgroup);
    long indx = INDEX(artNum);

    if (articles[indx].child) {
	artNum = articles[indx].child;
	while (artNum) {
	    if (!articleMarkedUnread(artNum))
		markArticleAsKilled(artNum);
	    indx = INDEX(artNum);
	    artNum = articles[indx].child;
	}
    }
}

/*
 * add a kill subject/author entry to a kill file
 */
void
killItem(item, type)
char *item;
int type;
{
    char input[BUFFER_SIZE], *iptr;
    char output[BUFFER_SIZE], *optr;
    FILE *fp;
    struct newsgroup *newsgroup = CurrentGroup;
    char *file;

    if (type == KILL_LOCAL) {
	file = localKillFile(newsgroup, 1);
    } else {
	file = globalKillFile();
    }

    if (file == NULL || ((fp = fopen(file, "a")) == NULL)) {
	mesgPane(XRN_SERIOUS, "Can not open the %s kill file for %s", 
		       (type == KILL_LOCAL) ? "local" : "global",
		       newsgroup->name);
	return;
    }
    /* get rid of all magic characters */
    (void) strncpy(input, item, sizeof(input));
    iptr = input;
    optr = output;
    while (*iptr) {
	if ((*iptr == '/')  ||
	    (*iptr == '\\') ||
	    (*iptr == '(')  ||
	    (*iptr == '[')  ||
	    (*iptr == '+')  ||
	    (*iptr == '-')  ||
	    (*iptr == ':')  ||
	    (*iptr == '.')  ||
	    (*iptr == '^')  ||
	    (*iptr == '*')  ||
	    (*iptr == '$')  ||
	    (*iptr == '\'') ||
	    (*iptr == '\"')) {
	    *iptr = '.';
	}
	*optr++ = *iptr++;
    }
    *optr = '\0';

    fprintf(fp, "/%.24s/:j\n", output);
    (void) fclose(fp);
    return;
}


/*
 * kill all subjects in the newsgroup that match the kill
 * orders in fp.
 */

static void
killArticles(newsgroup, fp)
struct newsgroup *newsgroup;
FILE *fp;
{
    char string[BUFFER_SIZE], pattern[BUFFER_SIZE], commands[BUFFER_SIZE];
    char dummy[BUFFER_SIZE];
    art_num i, start = newsgroup->current;
    char *subject, *author, *subj, *ptr, *pptr;
    int scount = 0, kcount = 0, ucount = 0, mcount = 0;
    int append;
    char *reRet;
    char type;

    mesgDisableRedisplay();

    info("processing KILL file");

    /* XXX don't reprocess global each time, keep in persistent hash table */

    string[0] = '\0';
    while (fgets(string, sizeof(string), fp) != NULL) {
	append = 0;

	/*
	 * see if there is a 'THRU artnum' line, if so,
	 * only compare subjects from that article on
	 * XXX need to update THRU
	 */
	if (STREQN(string, "THRU", 4)) {
	    i = atol(&string[5]);
	    /* if THRU is less than current, ignore it */
	    if (i + 1 > newsgroup->current) {
		start = i + 1;
	    } else {
		start = newsgroup->current;
	    }
	    continue;
	}

	if (*string == '&') {
	    /* 'rn' switch setting, ignore */
	    continue;
	}


	/*
	 * process kill file request should be in the form
	 */
	ptr = string + 1;
	pptr = pattern;

	while (*ptr && (*ptr != '/' || ((*ptr == '/') && *(ptr - 1) == '\\'))) {
	   *pptr++ = *ptr;
	   ptr++;
	}
	*pptr = '\0';

	if (!*ptr) {
	    /* empty line - ignore it. */
	    continue;
	}

	/* rn puts ': *' in front of patterns, xrn doesn't */
	if (strncmp(pattern, ": *", 3) == 0) {
	    /* deal with overlapping strings */
	    (void) strcpy(dummy, pattern + 3);
	    (void) strcpy(pattern, dummy);
	}

	/* XXX kludge to deal with :.*checks */
	if (*pattern == ':') {
	    /* deal with overlapping strings */
	    (void) strcpy(dummy, pattern + 1);
	    (void) strcpy(pattern, dummy);
	}

#ifdef SYSV_REGEX
	if ((reRet = regcmp(pattern, NULL)) == NULL) {
	    mesgPane(XRN_SERIOUS, "Bad KILL file pattern: %s", string);
#else
	if ((reRet = re_comp(pattern)) != NIL(char)) {
	    mesgPane(XRN_SERIOUS, "Bad KILL file pattern: %s\n%s", string, reRet);
#endif
	    continue;
	}

	ptr++;	/* skip past the slash */
	(void) strcpy(commands, ptr);
	if ((ptr = index(commands, ':')) == NIL(char)) {
	    mesgPane(XRN_SERIOUS, "Malformed KILL file entry (command): %s (%s)",
		     string, commands);
	    continue;
	}
	ptr++;	/* skip past the colon */
	type = *ptr;

	switch (type) {
	    case 'j':
	    case 'm':
	    case 's':
	        break;
	    default:
	        mesgPane(XRN_INFO,
			 "unknown KILL file option: %s", string);
	        break;
	}

	for (i = start; i <= newsgroup->last; i++) {

	    /* short cut */
	    if (IS_UNAVAIL(newsgroup->artStatus[INDEX(i)]) ||
		((type == 'j') && IS_READ(newsgroup->artStatus[INDEX(i)])) ||
		((type == 'm') && IS_UNREAD(newsgroup->artStatus[INDEX(i)]))) {
		continue;
	    }

	    if (currentArticles[INDEX(i)].subject ||
	        currentArticles[INDEX(i)].author) {

		subject = currentArticles[INDEX(i)].subject;
		author = currentArticles[INDEX(i)].author;

		if (subject) {
		    subj = strip(subject, FALSE);
		}

#ifdef SYSV_REGEX
		if ((subject && (regex(reRet, subj) != NULL)) ||
		    (author  && (regex(reRet, author) != NULL))) {
#else
		if ((subject && re_exec(subj)) ||
		    (author  && re_exec(author))) {
#endif
		    switch (type) {
			case 'j':
			    SET_READ(newsgroup->artStatus[INDEX(i)]);
			    mesgPane(XRN_INFO | append, "killed - %s",
				     subject);
			    append = XRN_APPEND;
			    kcount++;
			    break;

			case 'm':
			    SET_UNREAD(newsgroup->artStatus[INDEX(i)]);
			    mesgPane(XRN_INFO | append, "marked unread - %s",
				     subject);
			    append = XRN_APPEND;
			    mcount++;
			    break;

			case 's':
			    (void) saveArticle(NIL(char), newsgroup, i);
			    mesgPane(XRN_INFO | append, "saved - %s",
				     subject);
			    append = XRN_APPEND;
			    scount++;
			    break;

			default:
			    ucount++;
			    break;
		    }
		}
	    }
	}
    }

    append = 0;

#define printcount(c,m) \
    if( c != 0 ) \
    {	\
	mesgPane(XRN_INFO | append, m, c, \
		 ((c==1) ? "" : "s"), \
		 newsgroup->name); \
	append = XRN_APPEND; \
    }

    if (app_resources.verboseKill == True) {
	printcount(kcount, "killed %d article%s in %s");
	printcount(mcount, "marked %d article%s unread in %s");
	printcount(scount, "saved %d article%s in %s");
	printcount(ucount, "matched %d article%s with unknown option in %s");
    }

#undef printcount

    mesgEnableRedisplay();

    return;
}

/*
 * mark articles as read if in the kill files
 */
static void
checkKillFiles(newsgroup)
struct newsgroup *newsgroup;
{
    FILE *fp;
    char *file;
    
    file = globalKillFile();
    if (file && ((fp = fopen(file, "r")) != NULL)) {
	killArticles(newsgroup, fp);
	(void) fclose(fp);
    }

    file = localKillFile(newsgroup, 0);
    if (file && ((fp = fopen(file, "r")) != NULL)) {
	killArticles(newsgroup, fp);
	(void) fclose(fp);
    }

    return;
}

/*
 * Find the sort order for this newsgroup and set it up
 */

static void
setSortOrder(name)
char *name;
{
    char rname[200];
    char *type;
    XrmValue value;
    XrmValue converted;

#ifdef MOTIF
    sprintf(rname, "mxrn.%s.sortedSubjects", name);
#else
    sprintf(rname, "dxrn.%s.sortedSubjects", name);
#endif
    if (XrmGetResource(XtDatabase(XtDisplay(TopLevel)),
			rname, "SortedSubjects", &type, &value)) {
	converted.addr = NULL;
	converted.size = 0;
	XtConvert(TopLevel, type, &value, "SortedSubjects", &converted);
	if (converted.addr) {
	    app_resources.sortedSubjects = (int) *((int *)(converted.addr));
	} else {
	    app_resources.sortedSubjects = app_resources.defSortedSubjects;
	}
    } else {
	app_resources.sortedSubjects = app_resources.defSortedSubjects;
    }
}    

static void
setUpGroup(newsgroup, killfilesp)
struct newsgroup *newsgroup;
int killfilesp;		/* check kill files? no for jumpToNewsgroup */
{
    art_num first, last;
    int number;
    
    /* get the latest group information */
    if (getgroup(newsgroup, &first, &last, &number)) {
	return;
    }
    if (!EMPTY_GROUP(newsgroup)) {
        CurrentGroup = newsgroup;
	articleArrayResync(newsgroup, first, last, number);
	currentArticles = GETARTICLES(newsgroup);
	newsgroup->artStatus = GETSTATUS(newsgroup);
	newsgroup->initial = 0;
	setCurrentArticle(newsgroup, UNREAD);
	NextPreviousArticle = newsgroup->current;
	if (!getoverview(newsgroup, newsgroup->current,
			 newsgroup->last, False)) {
	    getsubjectlist(newsgroup, newsgroup->current,
			newsgroup->last, False);
	    getauthorlist(newsgroup, newsgroup->current,
			newsgroup->last, False);
	    getlineslist(newsgroup, newsgroup->current,
			newsgroup->last, False);
	    getmsgidlist(newsgroup, newsgroup->current,
			newsgroup->last, False);
	    getrefslist(newsgroup, newsgroup->current,
			newsgroup->last, False);
	}
	if (killfilesp && (app_resources.killFiles == TRUE)) {
	    checkKillFiles(newsgroup);
	}
    }
    setSortOrder(newsgroup->name);
    return;
}

/*
 * goto a particular newsgroup
 */
int
gotoNewsgroup(name)
char *name;
{
    char *ptr;
    struct newsgroup *newsgroup;

    if (!avl_lookup(NewsGroupTable, name, &ptr)) {
	mesgPane(XRN_SERIOUS, "Newsgroup `%s' does not exist", name);
	return BAD_GROUP;
    }

    newsgroup = (struct newsgroup *) ptr;
    CurrentGroup = newsgroup;
    setSortOrder(newsgroup->name);
    return GOOD_GROUP;
}
    
/*
 * goto a particular newsgroup
 *   if all of the articles have been read, unmark the last one
 */
int
gotoNewsgroupForRead(name)
char *name;
{
    char *ptr;
    struct newsgroup *newsgroup;
    art_num i;

    if (!avl_lookup(NewsGroupTable, name, &ptr)) {
	mesgPane(XRN_SERIOUS, "Newsgroup `%s' does not exist", name);
	return BAD_GROUP;
    }

    newsgroup = (struct newsgroup *) ptr;
    CurrentGroup = newsgroup;
    
    if (EMPTY_GROUP(newsgroup)) {
	mesgPane(XRN_SERIOUS,
	    "Cannot go to the newsgroup, no articles available");
	CurrentGroup = NULL;
	return BAD_GROUP;
    }
    /*
     * if there are no unread articles, find the last available one
     * and mark it as unread
     *
     * XXX bug if the last article becomes unavailable
     */
    if (unreadArticleCount(newsgroup) == 0) {
	if (newsgroup->artStatus == NIL(struct artStat)) {
	    mesgPane(XRN_SERIOUS,
		"Cannot go to the newsgroup, no articles available");
	    CurrentGroup = NULL;
	    return BAD_GROUP;
	}	    
	assert(newsgroup->artStatus != NIL(struct artStat));
	for (i = newsgroup->last; i >= newsgroup->first; i--) {
	    if (!IS_UNAVAIL(newsgroup->artStatus[INDEX(i)])) {
		SET_UNREAD(newsgroup->artStatus[INDEX(i)]);
		return GOOD_GROUP_MARKED;
	    }
	}
    }
    return GOOD_GROUP;
}

/*
 * set the current state to indicate that we're in a particular newsgroup
 */
int
setNewsgroup(name)
char *name;
{
    char *ptr;
    struct newsgroup *newsgroup;

    if (!avl_lookup(NewsGroupTable, name, &ptr)) {
	mesgPane(XRN_SERIOUS, "Newsgroup `%s' does not exist", name);
	return BAD_GROUP;
    }

    newsgroup = (struct newsgroup *) ptr;
    CurrentGroup = newsgroup;
    setSortOrder(newsgroup->name);
    return GOOD_GROUP;
}

static struct newsgroup *
findNewsGroupMatch(name)
char *name;
{
    static char *reRet;
    int i;

    /* no single character queries, to comfusing */
    if (strlen(name) == 1) {
	return NIL(struct newsgroup);
    }

#ifdef SYSV_REGEX
    if ((reRet = regcmp(name, NULL)) == NULL) {
	return NIL(struct newsgroup);
    }
#else
    if ((reRet = re_comp(name)) != NULL) {
	return NIL(struct newsgroup);
    }
#endif


    for (i = 0; i < MaxGroupNumber; i++) {
	struct newsgroup *newsgroup = Newsrc[i];
#ifdef SYSV_REGEX
	if (regex(reRet, newsgroup->name) != NULL) {
	    return newsgroup;
	}
#else
	if (re_exec(newsgroup->name)) {
	    return newsgroup;
	}
#endif
	    
    }

    return NIL(struct newsgroup);
}

/*
 * goto a particular newsgroup
 *   if all of the articles have been read, unmark the last one
 */
int
jumpToNewsgroup(name)
char *name;
{
    char *ptr;
    struct newsgroup *newsgroup;
    art_num i;
    
    if (!avl_lookup(NewsGroupTable, name, &ptr)) {
	/* is not the full name, try regexp matching */
	if ((newsgroup = findNewsGroupMatch(name)) == NIL(struct newsgroup)) {
	    mesgPane(XRN_SERIOUS, "Newsgroup `%s' does not exist", name);
	    return BAD_GROUP;
	}
    } else {
	newsgroup = (struct newsgroup *) ptr;
    }

    CurrentGroup = newsgroup;

    if (EMPTY_GROUP(newsgroup)) {
	mesgPane(XRN_SERIOUS, "Cannot go to the newsgroup, no articles available");
	return BAD_GROUP;
    }

    SETARTICLES(newsgroup);

    if (!IS_SUBSCRIBED(newsgroup)) {
	updateArticleArray(newsgroup, True);
    }

    /*
     * if there are no unread articles, find the last available one
     * and mark it as unread
     *
     * XXX bug if the last article becomes unavailable
     */
    if (unreadArticleCount(newsgroup) == 0) {
	if (newsgroup->artStatus == NIL(struct artStat)) {
	    mesgPane(XRN_SERIOUS, "Cannot go to the newsgroup, no articles available");
	    return BAD_GROUP;
	}
	assert(newsgroup->artStatus != NIL(struct artStat));
	for (i = newsgroup->last; i >= newsgroup->first; i--) {
	    if (!IS_UNAVAIL(newsgroup->artStatus[INDEX(i)])) {
		SET_UNREAD(newsgroup->artStatus[INDEX(i)]);
		break;
	    }
	}
    }
    setUpGroup(newsgroup, 0);
    return GOOD_GROUP;
}
    
/*
 * get the newsgroup
 *
 * returns:  XRN_NOMORE - bad group
 *           XRN_OKAY   - okay
 */
int
getNewsgroup()
{
    struct newsgroup *newsgroup = CurrentGroup;

    if (IS_SUBSCRIBED(newsgroup) && (unreadArticleCount(newsgroup) > 0)) {
	setUpGroup(newsgroup, 1);
	return XRN_OKAY;
    }
    
    return XRN_NOMORE;
}

/*
 * mark all articles in a newsgroup as read
 *
 *   other functions will take care of releasing resources
 *
 *   returns: void
 *
 */
void
catchUp()
{
    struct newsgroup *newsgroup = CurrentGroup;
    art_num art;

    if (!newsgroup) return;
    SETSTATUS(newsgroup);
    if (!newsgroup->artStatus)
	return;
    for (art = newsgroup->first; art <= newsgroup->last; art++) {
	if (IS_UNMARKED(newsgroup->artStatus[INDEX(art)])) {
	    SET_READ(newsgroup->artStatus[INDEX(art)]);
	} else {
	    SET_UNREAD(newsgroup->artStatus[INDEX(art)]);
	    SET_UNMARKED(newsgroup->artStatus[INDEX(art)]);
	}
    }
}


int
issubscribed()
{
    struct newsgroup *newsgroup = CurrentGroup;

    if (IS_SUBSCRIBED(newsgroup)) {
	return 1;
    } else {
	return 0;
    }
}

/*
 * subscribe to a newsgroup
 *
 *   returns: void
 *
 */
void
subscribe()
{
    struct newsgroup *newsgroup = CurrentGroup;
    
    if (!IS_SUBSCRIBED(newsgroup)) {
	SET_SUB(newsgroup);
	updateArticleArray(newsgroup, True);
    }
    return;
}

/*
 * unsubscribe to a newsgroup
 *
 *   returns: void
 *
 */
void
unsubscribe()
{
    struct newsgroup *newsgroup = CurrentGroup;
    
    SET_UNSUB(newsgroup);
    return;
}

/*
 * build a string that is used as the question for what
 * to do at the end of an article
 *
 *   returns: the question in a static area
 *
 */
static char *
buildQuestion(newsgroup)
struct newsgroup *newsgroup;
{
    static char dummy[LABEL_SIZE];
    art_num i;
    struct artStat *artstat = GETSTATUS(newsgroup);
    long unread = 0;
    ng_num number;
    struct newsgroup *nextnewsgroup;
    int found;
    
    for (i = newsgroup->first; i <= newsgroup->last; i++) {
	long indx = INDEX(i);
	if (IS_UNREAD(artstat[indx]) && !IS_UNAVAIL(artstat[indx])) {
	    unread++;
	}
    }
	    
    found = 0;
    for (number = CurrentGroup->newsrc + 1; number < MaxGroupNumber;
	 number++) {
	nextnewsgroup = Newsrc[number];
	/* find a group that is subscribed to and has unread articles */
	if (IS_SUBSCRIBED(nextnewsgroup) && (unreadArticleCount(nextnewsgroup) > 0)) {
	    found = 1;
	    break;
	}
    }
	    
    if (found) {
	if (unread <= 0) {
	    (void) sprintf(dummy, "Article %ld in %s\nNext group: %s, with %d articles",
			   newsgroup->current, newsgroup->name,
			   nextnewsgroup->name, unreadArticleCount(nextnewsgroup));
	} else {
	    (void) sprintf(dummy, "Article %ld in %s (%ld remaining)\nNext group: %s, with %d articles",
			   newsgroup->current, newsgroup->name, unread,
			   nextnewsgroup->name, unreadArticleCount(nextnewsgroup));
	}
    } else {
	if (unread <= 0) {
	    (void) sprintf(dummy, "Article %ld in %s",
			   newsgroup->current, newsgroup->name);
	} else {
	    (void) sprintf(dummy, "Article %ld in %s (%ld remaining)",
			   newsgroup->current, newsgroup->name, unread);
	}
    }
    return dummy;
}

char *currentQuestion()
{
    struct newsgroup *newsgroup = CurrentGroup;
    return buildQuestion(newsgroup);
}

/*
 * build a string that is used as the question for what
 * to do at the end of an article
 *
 *   returns: the question in a static area
 *
 */
char *
openQuestion()
{
    struct newsgroup *newsgroup = CurrentGroup;
    static char dummy[LABEL_SIZE];
    art_num i;
    struct artStat *artstat = GETSTATUS(newsgroup);
    long unread = 0;
    ng_num number;
    struct newsgroup *nextnewsgroup;
    int found;

    for (i = newsgroup->first; i <= newsgroup->last; i++) {
	long indx = INDEX(i);
	if (IS_UNREAD(artstat[indx]) && !IS_UNAVAIL(artstat[indx])) {
	    unread++;
	}
    }
	    
    found = 0;
    for (number = CurrentGroup->newsrc + 1; number < MaxGroupNumber;
	 number++) {
	nextnewsgroup = Newsrc[number];
	/* find a group that is subscribed to and has unread articles */
	if (IS_SUBSCRIBED(nextnewsgroup) && (unreadArticleCount(nextnewsgroup) > 0)) {
	    found = 1;
	    break;
	}
    }
	    
    if (found) {
	if (unread <= 0) {
	    (void) sprintf(dummy, "Group %s\nNext group: %s, with %d articles",
			   newsgroup->name,
			   nextnewsgroup->name, unreadArticleCount(nextnewsgroup));
	} else {
	    (void) sprintf(dummy, "Group %s (%ld remaining)\nNext group: %s, with %d articles",
			   newsgroup->name, unread,
			   nextnewsgroup->name, unreadArticleCount(nextnewsgroup));
	}
    } else {
	if (unread <= 0) {
	    (void) sprintf(dummy, "Group %s", newsgroup->name);
	} else {
	    (void) sprintf(dummy, "Group %s (%ld remaining)",
			   newsgroup->name, unread);
	}
    }
    return dummy;
}

static void
handleXref(article)
art_num article;
{
    char *string, *ptr, *token, group[GROUP_NAME_SIZE], *gptr;
    int count, number;
    struct newsgroup *newsgroup;

    
    xhdr(article, "xref", &string);

    if (string == NIL(char)) {
	/* no xrefs */
	return;
    }

    /*
     * an xrefs line is of the form:
     *
     *   host group:number group:number .... group:number
     */

    if ((ptr = index(string, ' ')) == NIL(char)) {
	FREE(string);
	return;
    }

    while ((token = strtok(ptr, " ")) != NIL(char)) {
	ptr = NIL(char);
	
	count = sscanf(token, "%[^: ]:%d", group, &number);
	if (count != 2) {
	    /* bogus entry */
	    continue;
	}

	if (!avl_lookup(NewsGroupTable, group, &gptr)) {
	    /* bogus group */
	    continue;
	}

	/* only Xref groups that are subscribed to */
	
	newsgroup = (struct newsgroup *) gptr;

	if (IS_SUBSCRIBED(newsgroup) &&
	    (number >= newsgroup->first) && (number <= newsgroup->last)) {
	    SETSTATUS(newsgroup);	/* defer until we have to do this */

	    SET_READ(newsgroup->artStatus[INDEX(number)]);
	}
    }
    FREE(string);
    return;
}

/*
 * get the next article
 *
 * returns: XRN_ERROR - article has been canceled
 *          XRN_OKAY  - article returned
 */
int
getArticleText(text, question)
char **text;
char **question;
{
    struct newsgroup *newsgroup = CurrentGroup;
    struct article *articles = GETARTICLES(newsgroup);
    struct artStat *artstat = GETSTATUS(newsgroup);
    long indx = CURRENT;
    int header, rotation;
#ifdef XLATE
    int	xlation;
#endif /* XLATE */
    int i;
    static int artIndex = -1;

/*
 *  Point to next cell in the index array, wrapping around.
 */
    if (artIndex < 4) {
	artIndex++;
    } else {
	artIndex = 0;
    }
/*
 *  Is this article one of the last five read? If so, re-use the
 *  stored article array
 */
    for (i = 0; i < 5; i++) {
	if (PrevArtIndx[i] == indx) {
	    artIndex = i;
	    break;
	}
    }
/*
 *  If No match found, we free the article text for the current
 *  cell in the storage array. Else, it's left in the cache.
 */
    if ((PrevArtIndx[artIndex] != -1) && (PrevArtIndx[artIndex] != indx)) {
	if (PrevArtIndx[artIndex] < 0 ||
	    PrevArtIndx[artIndex] > newsgroup->last - newsgroup->first) {
		PrevArtIndx[artIndex] = -1;
	} else {
	    CLEAR_TEXT(articles[PrevArtIndx[artIndex]]);
	    SET_UNFETCHED(artstat[PrevArtIndx[artIndex]]);
	    PrevArtIndx[artIndex] = -1;
	}
    }

    if (IS_UNFETCHED(artstat[indx])) {
	
	/* get the article and handle unavailable ones.... */
	header = (IS_ALL_HEADERS(artstat[indx]) ? FULL_HEADER : NORMAL_HEADER);
	rotation = (IS_ROTATED(artstat[indx]) ? ROTATED : NOT_ROTATED);
#ifdef XLATE
	xlation = (IS_XLATED(artstat[indx]) ? XLATED : NOT_XLATED);
	if ((articles[indx].text = getarticle(newsgroup->current, 
				         &articles[indx].position,
					 header, rotation,xlation)) == NIL(char)) {
#else /* XLATE */
	if ((articles[indx].text = getarticle(newsgroup->current, 
				         &articles[indx].position,
					 header, rotation)) == NIL(char)) {
#endif /* XLATE */
	    SET_UNAVAIL(artstat[indx]);
	    mesgPane(XRN_SERIOUS, "Can not get the next article, it was canceled by the author or a file system filled");
	    return XRN_ERROR;
	}
	SET_FETCHED(artstat[indx]);
    } else {
	if (articles[indx].text == NIL(char)) {
	    /* refetch the text */
	    SET_UNFETCHED(artstat[indx]);
	    return getArticleText(text, question);
	}	    
    }

    *text = articles[indx].text;
    SET_READ(artstat[indx]);
    *question = buildQuestion(newsgroup);
    handleXref(newsgroup->current);
/*
 *  Save the index of this article in the
 *  cache array
 */
    PrevArtIndx[artIndex] = indx;
    return XRN_OKAY;
}


int
toggleHeaders(text, question)
char **text;
char **question;
{
    struct newsgroup *newsgroup = CurrentGroup;
    struct article *articles = GETARTICLES(newsgroup);
    struct artStat *artstat = GETSTATUS(newsgroup);
    long indx = CURRENT;
    int i;
/*
 *  Find the article in the storage array and "forget" it.
 */
    for (i = 0; i < 5; i++) {
	if (PrevArtIndx[i] == indx) {
	    CLEAR_TEXT(articles[CURRENT]);
	    SET_UNFETCHED(artstat[CURRENT]);
	    PrevArtIndx[i] = -1;
	    break;
	}
    }

    if (IS_ALL_HEADERS(artstat[CURRENT])) {
	SET_STRIPPED_HEADERS(artstat[CURRENT]);
    } else {
	SET_ALL_HEADERS(artstat[CURRENT]);
    }	
    CLEAR_TEXT(articles[CURRENT]);
    SET_UNFETCHED(artstat[CURRENT]);
    return getArticleText(text, question);
}


int
toggleRotation(text, question)
char **text;
char **question;
{
    struct newsgroup *newsgroup = CurrentGroup;
    struct article *articles = GETARTICLES(newsgroup);
    struct artStat *artstat = GETSTATUS(newsgroup);

    if (IS_ROTATED(artstat[CURRENT])) {
	SET_UNROTATED(artstat[CURRENT]);
    } else {
	SET_ROTATED(artstat[CURRENT]);
    }	
    CLEAR_TEXT(articles[CURRENT]);
    SET_UNFETCHED(artstat[CURRENT]);
    return getArticleText(text, question);
}

#ifdef XLATE
int
toggleXlation(text, question)
char **text;
char **question;
{
    struct newsgroup *newsgroup = CurrentGroup;
    struct article *articles = GETARTICLES(newsgroup);
    struct artStat *artstat = GETSTATUS(newsgroup);

    if (IS_XLATED(artstat[CURRENT])) {
	SET_UNXLATED(artstat[CURRENT]);
    } else {
	SET_XLATED(artstat[CURRENT]);
    }	
    CLEAR_TEXT(articles[CURRENT]);
    SET_UNFETCHED(artstat[CURRENT]);
    return getArticleText(text, question);
}
#endif /* XLATE */

/*
 * mark the articles in a group that have been read
 *
 *   returns: void
 *
 */
#if defined(STDC) && !defined(_NO_PROTO)
void
updateArticleArray(struct newsgroup *newsgroup, int force)
#else
void
updateArticleArray(newsgroup, force)
struct newsgroup *newsgroup;    /* newsgroup to update article array for   */
int force;			/* True to force update */
#endif
{
    struct list *item;
    art_num art;
    extern void XtFree();
#ifndef FIXED_C_NEWS_ACTIVE_FILE
    art_num first, last;
    int number;
#endif

    if (newsgroup->last == 0) {
	return;
    }

    if (EMPTY_GROUP(newsgroup)) {
	currentArticles = NIL(struct article);
	newsgroup->artStatus = NIL(struct artStat);
	return;
    }

    if (newsgroup->nglist == NIL(struct list)) {
	return;
    }

    if (!IS_SUBSCRIBED(newsgroup) && !force) {
	currentArticles = NIL(struct article);
	newsgroup->artStatus = NIL(struct artStat);
	return;
    }

#ifndef FIXED_C_NEWS_ACTIVE_FILE
    /* get the group range to fix c-news low number problem */
    if ((XRNState & XRN_NEWS_UP) == XRN_NEWS_UP) {
	(void) getgroup(newsgroup, &first, &last, &number);
	articleArrayResync(newsgroup, first, last, number);
	if ((newsgroup->first == 0 && newsgroup->last == 0) || number == 0) {
	    lsDestroy(newsgroup->nglist);
	    newsgroup->nglist = NIL(struct list);
	    return;
	}
    }
#endif

    SETSTATUS(newsgroup);

    /* process the .newsrc line */

    for (item = newsgroup->nglist; item != NIL(struct list); item = item->next) {
	switch (item->type) {
	    case SINGLE:
	    if (item->contents.single > newsgroup->last) {
		/* something really bad has happened, reset */
		mesgPane(XRN_SERIOUS, "Article numbering problem, marking all articles in %s as unread",
			       newsgroup->name);
		for (art = newsgroup->first; art <= newsgroup->last; art++) {
		    if (newsgroup->artStatus != NIL(struct artStat))
			newsgroup->artStatus[INDEX(art)].status = ART_CLEAR;
		}
		lsDestroy(newsgroup->nglist);
		newsgroup->nglist = NIL(struct list);
		return;
	    }
	    if (item->contents.single >= newsgroup->first) {
		newsgroup->artStatus[INDEX(item->contents.single)].status = ART_CLEAR_READ;
	    }
	    break;

	    case RANGE:
	    if ((item->contents.range.start > newsgroup->last) ||
		(item->contents.range.end > newsgroup->last)) {
		/* something really bad has happened, reset */
		if (newsgroup->first != 0 && newsgroup->last !=0)
		    mesgPane(XRN_SERIOUS,
	"Article numbering problem, marking all articles in %s as unread\n",
			       newsgroup->name);
		for (art = newsgroup->first; art <= newsgroup->last; art++) {
		    if (newsgroup->artStatus != NIL(struct artStat))
			newsgroup->artStatus[INDEX(art)].status = ART_CLEAR;
		}
		lsDestroy(newsgroup->nglist);
		newsgroup->nglist = NIL(struct list);
		return;
	    }
	    if (item->contents.range.start < newsgroup->first) {
		item->contents.range.start = newsgroup->first;
	    }
	    
	    if (item->contents.range.end < newsgroup->first) {
		break;
	    }
	    for (art = item->contents.range.start; art <= item->contents.range.end; art++) {
		newsgroup->artStatus[INDEX(art)].status = ART_CLEAR_READ;
	    }
	}
    }

    lsDestroy(newsgroup->nglist);
    newsgroup->nglist = NIL(struct list);
    
    return;
}

/*
 * mark an article as read
 */
void
markArticleAsRead(article)
art_num article;
{
    struct newsgroup *newsgroup = CurrentGroup;

    SETSTATUS(newsgroup);
    SET_READ(newsgroup->artStatus[INDEX((art_num) article)]);
    SET_UNMARKED(newsgroup->artStatus[INDEX((art_num) article)]);
    if (app_resources.xrefMarkRead) {
	handleXref(article);
    }
    return;
}

/*
 * mark an article as unread
 */
void
markArticleAsUnread(article)
art_num article;
{
    struct newsgroup *newsgroup = CurrentGroup;

    SETSTATUS(newsgroup);
    SET_UNREAD(newsgroup->artStatus[INDEX((art_num) article)]);
    SET_MARKED(newsgroup->artStatus[INDEX((art_num) article)]);
    return;
}

/*
 * mark an article as killed
 */
void
markArticleAsKilled(article)
art_num article;
{
    struct newsgroup *newsgroup = CurrentGroup;

    SETSTATUS(newsgroup);
    SET_READ(newsgroup->artStatus[INDEX((art_num) article)]);
    SET_KILLED(newsgroup->artStatus[INDEX((art_num) article)]);
    SET_UNMARKED(newsgroup->artStatus[INDEX((art_num) article)]);
    return;
}

/*
 * mark an article as unkilled
 */
void
markArticleAsUnkilled(article)
art_num article;
{
    struct newsgroup *newsgroup = CurrentGroup;

    SETSTATUS(newsgroup);
    SET_UNREAD(newsgroup->artStatus[INDEX((art_num) article)]);
    SET_UNKILLED(newsgroup->artStatus[INDEX((art_num) article)]);
    SET_MARKED(newsgroup->artStatus[INDEX((art_num) article)]);
    return;
}

/*
 * Return TRUE if article is read
 */
Boolean
articleRead(article)
art_num article;
{
    struct newsgroup *newsgroup = CurrentGroup;

    SETSTATUS(newsgroup);
    return(IS_READ(newsgroup->artStatus[INDEX((art_num) article)])  ||
    	   IS_MARKED(newsgroup->artStatus[INDEX((art_num) article)]));
}

/*
 * Return TRUE if article is marked unread
 */
Boolean
articleMarkedUnread(article)
art_num article;
{
    struct newsgroup *newsgroup = CurrentGroup;
    struct artStat *artstat = GETSTATUS(newsgroup);
    long indx = INDEX((art_num) article);

    return (IS_UNREAD(artstat[indx]) && IS_MARKED(artstat[indx]));
}


/*
 * handle adding items to the newsrc
 *
 *   3 cases
 *
 *   1. add to the beginning
 *        move 0 to MaxGroupNumber-1 down 1, update newsrc fields
 *        update 0
 *        inc MaxGroupNumber
 *   2. add to the end
 *        inc MaxGroupNumber
 *        update MaxGroupNumber-1
 *   3. add after a group (newloc is the current location of the group)
 *      move newloc+1 to end down 1, update newsrc fields
 *      update newloc
 *      in MaxGroupNumber
 *
 *   And the case of 'subscribe' (assumes it moves)
 *
 *   1. add to the beginning
 *        move 0 to oldloc-1 down 1, update newsrc fields
 *        update 0
 *   2. add to the end
 *        move oldloc+1 to MaxGroupNumber-1 update 1, update newsrc fields
 *        upadte MaxGroupNumber-1
 *   3. add after a group
 *
 */

int
addToNewsrcBeginning(newGroup, status)
char *newGroup;
int status;
{
    char *ptr;
    struct newsgroup *newsgroup;
    ng_num i;
    
    if (!avl_lookup(NewsGroupTable, newGroup, &ptr)) {
	mesgPane(XRN_SERIOUS, "Newsgroup `%s' does not exist", newGroup);
	return BAD_GROUP;
    }
    
    newsgroup = (struct newsgroup *) ptr;

    CLEAR_NOENTRY(newsgroup);
    if (status == SUBSCRIBE) {
	SET_SUB(newsgroup);
    } else {
	SET_UNSUB(newsgroup);
    }
    if (newsgroup->newsrc == NOT_IN_NEWSRC) {
	for (i = MaxGroupNumber - 1; i >= 0; i--) {
	    Newsrc[i + 1] = Newsrc[i];
	    Newsrc[i + 1]->newsrc = i + 1;
	}
    
	MaxGroupNumber++;
	
    } else {
	for (i = newsgroup->newsrc - 1; i >= 0; i--) {
	    Newsrc[i + 1] = Newsrc[i];
	    Newsrc[i + 1]->newsrc = i + 1;
	}
    }
    
    newsgroup->newsrc = 0;
    Newsrc[0] = newsgroup;
    
    return GOOD_GROUP;
}


int
addToNewsrcEnd(newGroup, status)
char *newGroup;
int status;
{
    char *ptr;
    struct newsgroup *newsgroup;
    ng_num i;
    
    if (!avl_lookup(NewsGroupTable, newGroup, &ptr)) {
	mesgPane(XRN_SERIOUS, "Newsgroup `%s' does not exist", newGroup);
	return BAD_GROUP;
    }
    
    newsgroup = (struct newsgroup *) ptr;

    CLEAR_NOENTRY(newsgroup);
    if (status == SUBSCRIBE) {
	SET_SUB(newsgroup);
    } else {
	SET_UNSUB(newsgroup);
    }
    if (newsgroup->newsrc == NOT_IN_NEWSRC) {
	MaxGroupNumber++;
    } else {
	for (i = newsgroup->newsrc + 1; i < MaxGroupNumber; i++) {
	    Newsrc[i - 1] = Newsrc[i];
	    Newsrc[i - 1]->newsrc = i - 1;
	}
    }
    
    newsgroup->newsrc = MaxGroupNumber - 1;
    Newsrc[MaxGroupNumber - 1] = newsgroup;
    
    return GOOD_GROUP;
}


int
addToNewsrcAfterGroup(newGroup, afterGroup, status)
char *newGroup;
char *afterGroup;
int status;
{
    char *ptr;
    struct newsgroup *newsgroup, *ng;
    ng_num newloc, i;
    
    if (!avl_lookup(NewsGroupTable, newGroup, &ptr)) {
	mesgPane(XRN_SERIOUS, "Newsgroup `%s' does not exist", newGroup);
	return BAD_GROUP;
    }
    
    newsgroup = (struct newsgroup *) ptr;

    CLEAR_NOENTRY(newsgroup);
    if (status == SUBSCRIBE) {
	SET_SUB(newsgroup);
    } else {
	SET_UNSUB(newsgroup);
    }
    if (!avl_lookup(NewsGroupTable, afterGroup, &ptr)) {
	mesgPane(XRN_SERIOUS, "Newsgroup `%s' does not exist", afterGroup);
	return BAD_GROUP;
    }
    
    ng = (struct newsgroup *) ptr;
    newloc = ng->newsrc;

    if (newloc == NOT_IN_NEWSRC) {
	mesgPane(XRN_SERIOUS, "Newsgroup `%s' is not in your .newsrc file", afterGroup);
	return BAD_GROUP;
    }

    if (newsgroup->newsrc == NOT_IN_NEWSRC) {
	for (i = MaxGroupNumber - 1; i >= newloc + 1; i--) {
	    Newsrc[i + 1] = Newsrc[i];
	    Newsrc[i + 1]->newsrc = i + 1;
	}

	MaxGroupNumber++;
	newsgroup->newsrc = newloc + 1;
	Newsrc[newloc + 1] = newsgroup;

    } else {
	
	if (newloc + 1 < newsgroup->newsrc) {
	    for (i = newsgroup->newsrc - 1; i >= newloc + 1; i--) {
		Newsrc[i + 1] = Newsrc[i];
		Newsrc[i + 1]->newsrc = i + 1;
	    }
	    newsgroup->newsrc = newloc + 1;
	    Newsrc[newloc + 1] = newsgroup;
	    
	} else if (newsgroup->newsrc < newloc + 1) {
	    for (i = newsgroup->newsrc + 1; i < newloc + 1; i++) {
		Newsrc[i - 1] = Newsrc[i];
		Newsrc[i - 1]->newsrc = i - 1;
	    }
	    newsgroup->newsrc = newloc;
	    Newsrc[newloc] = newsgroup;
	}
	/* if its in the correct location already, don't touch it */
    }
    
    return GOOD_GROUP;
}

	
/*
 * build and return an array that shows the subscription status
 * of all newsgroups; assumes all groups have been subscribed or
 * unsubscribed to by this time.
 *
 *   if sorted is non-zero, the list is sorted alphabetically, if
 *    zero, the list is returned as it exists in the newsrc file
 */
ng_num *
getStatusList(sorted)
int sorted;
{
    int i, count = 0;
    avl_generator *gen;
    char *key, *value;
    ng_num *ar;

    ar = ARRAYALLOC(ng_num, ActiveGroupsCount + 1);

    if (sorted) {
	gen = avl_init_gen(NewsGroupTable, AVL_FORWARD);

	while (avl_gen(gen, &key, &value)) {
	    struct newsgroup *newsgroup = (struct newsgroup *) value;
	    
	    if (IS_NOTALIAS(newsgroup)) {
		ar[count++] = newsgroup->newsrc;
	    }
	}
	avl_free_gen(gen);
	
    } else {
	for (i = 0; i < MaxGroupNumber; i++) {
	    struct newsgroup *newsgroup = (struct newsgroup *) Newsrc[i];
	    
	    if (IS_NOTALIAS(newsgroup)) {
		ar[count++] = i;
	    }
	}
    }	    
    
    ar[count++] = -1;
    return ar;
}

/*
 * build and return a string that shows the subscription status
 * of a newsgroup.
 */
char *
getStatusString(which)
int which;
{
    struct newsgroup *newsgroup;
    static char dummy[LINE_LENGTH];

    dummy[0] = '\0';
    if (which < 0 || which > MaxGroupNumber)
	return dummy;

    newsgroup = Newsrc[which];
	    
    if (IS_NOTALIAS(newsgroup)) {
	(void) sprintf(dummy, "%-60s%s",
		newsgroup->name,
		IS_SUBSCRIBED(newsgroup) ? "subscribed  " : "unsubscribed");
    }
    return dummy;
}
/*
 * build and return the subjects string
 */
art_num *
getUnSortedArticles(mode, count)
int mode;
int *count;
{
    struct newsgroup *newsgroup = CurrentGroup;
    struct article *articles = GETARTICLES(newsgroup);
    struct artStat *artstat = GETSTATUS(newsgroup);
    art_num i;
    int	    j;
    char *start, *end;
    art_num *ar;

    *count = 0;
    if (EMPTY_GROUP(newsgroup)) {
	return NIL(art_num);
    }

    if (mode == ALL) {
	newsgroup->initial = newsgroup->current = NextPreviousArticle;
    } else {
	setCurrentArticle(newsgroup, mode);
    }

    if (newsgroup->current > newsgroup->last) {
	return NIL(art_num);
    }

    NextPreviousArticle = newsgroup->current;

    if ((newsgroup->last - newsgroup->current + 1) < 0) {
	(void) sprintf(error_buffer, "Active File Error: last - current + 1 < 0 (%s)\n",
			       newsgroup->name);
	ehErrorExitXRN(error_buffer);
    }
	
/*
 * Allocate an array to store the article numbers.
 */
    ar = ARRAYALLOC(art_num, (newsgroup->last - newsgroup->current + 3));
    for (i = 0; i <= (newsgroup->last - newsgroup->current);i++) {
	ar[i] = 0;
    }

    j = 0;
    for (i = newsgroup->current; i <= newsgroup->last; i++) {
	long indx = INDEX(i);
	
	/* canceled and empty articles will not have a subject entry */
	if (articles[indx].subject != NIL(char)) {
	    
	    /* don't put articles in the string if already read ... */
	    if ((mode == ALL) ||
	        (mode == UNKILLED && IS_UNKILLED(artstat[indx]) &&
				     IS_ACTIVE(artstat[indx])) ||
		(mode == ACTIVE && IS_ACTIVE(artstat[indx])) ||
	        (mode == UNREAD && IS_UNREAD(artstat[indx]))) {
		if (mode != UNKILLED)
		    SET_ACTIVE(artstat[indx]);
		ar[j] = i;
		j++;
	    }
	} else {
	    /* if you can't get the subject, mark the article as unavailable */
	    SET_UNAVAIL(artstat[indx]);
	}
    }

    *count = j;
    return ar;
}


struct entry {
    int startingArticle;
    art_num *articleList;
    int used;
    int left;
    int size;
};

static void
valfree(ptr)
char *ptr;
{
    struct entry *val = (struct entry *) ptr;
    XtFree((char *) val->articleList);
    XtFree((char *) val);
    return;
}


#if defined(STDC) && !defined(_NO_PROTO)
static int
pteCompare(const void *a, const void *b)
#else
static int
pteCompare(a, b)
void *a, *b;
#endif
{
    struct entry **pa = (struct entry **) a;
    struct entry **pb = (struct entry **) b;

    return (*pa)->startingArticle - (*pb)->startingArticle;
}


/*
 * XXX AVL TREE is the wrong data structure here, hash table would be
 * better.... no need for ordering based on subject string
 */
/*
 * build and return the subjects string
 */
static art_num *
getSortedArticles(mode, count)
int mode;
int *count;
{
    struct newsgroup *newsgroup = CurrentGroup;
    struct article *articles = GETARTICLES(newsgroup);
    struct artStat *artstat = GETSTATUS(newsgroup);
    struct article *art;
    art_num i;
    int j, k;
    art_num *ar;

    struct entry *pte;
    struct entry **pteArray;

    avl_generator *gen;
    char *key, *ptr, *p;
    avl_tree *tree;
    char curSub[81];
    int treeSize, sz;
    char buffer[LINE_LENGTH];

    *count = 0;
    if (EMPTY_GROUP(newsgroup)) {
	return NIL(art_num);
    }

    tree = avl_init_table(utSubjectCompare);

    if (mode == ALL) {
	newsgroup->initial = newsgroup->current = NextPreviousArticle;
    } else {
	setCurrentArticle(newsgroup, mode);
    }

    if (newsgroup->current > newsgroup->last) {
	return NIL(art_num);
    }

    NextPreviousArticle = newsgroup->current;

    if ((newsgroup->last - newsgroup->current + 1) < 0) {
	(void) sprintf(error_buffer, "Active File Error: last - current + 1 < 0 (%s)\n",
			       newsgroup->name);
	ehErrorExitXRN(error_buffer);
    }
	
    /* 
     * build the subject groups 
     */
    for (i = newsgroup->current; i <= newsgroup->last; i++) {
	long indx = INDEX(i);

	/* canceled and empty articles will not have a subject entry */
	if (articles[indx].subject != NIL(char)) {

	    /* don't put articles in the string if already read ... */
	    if ((mode == ALL) ||
	        (mode == UNKILLED && IS_UNKILLED(artstat[indx]) &&
				     IS_ACTIVE(artstat[indx])) ||
		(mode == ACTIVE && IS_ACTIVE(artstat[indx])) ||
	        (mode == UNREAD && IS_UNREAD(artstat[indx]))) {
		if (mode != UNKILLED)
		    SET_ACTIVE(artstat[indx]);
		(void) strncpy(curSub, getSubject(i), sizeof(curSub));
		curSub[sizeof(curSub)-1] = '\0';

		if (avl_lookup(tree, curSub, &ptr)) {
		    /* add to the end */
		    pte = (struct entry *) ptr;
		    if (pte->left <= 0) {
			pte->left = pte->size;
			pte->size = pte->size * 2;
			pte->articleList =
				(art_num *) XtRealloc((char *)pte->articleList,
					     pte->size * sizeof(art_num));
		    }
		    pte->articleList[pte->used] = i;
		    pte->used++;
		    pte->left--;
		} else {
		    /* create new */
		    pte = ALLOC(struct entry);
		    pte->startingArticle = i;
		    pte->articleList = ARRAYALLOC(art_num, 32);
		    pte->used = 1;
		    pte->size = 32;
		    pte->left = pte->size - pte->used;
		    pte->articleList[0] = i;
		    (void) avl_insert(tree, XtNewString(curSub), (char *) pte);
		}
	    }
	} else {
	    /* if you can't get the subject, mark the article as unavailable */
	    SET_UNAVAIL(artstat[indx]);
	}
    }

    i = 0;
    treeSize = avl_count(tree);
    if (treeSize == 0) {		/* No articles */
	avl_free_table(tree, XtFree, valfree);
	return NIL(art_num);
    }

    pteArray = ARRAYALLOC(struct entry *, treeSize);
    gen = avl_init_gen(tree, AVL_FORWARD);
    while (avl_gen(gen, &key, &ptr)) {
	pteArray[i++] = (struct entry *) ptr;
    }
    avl_free_gen(gen);

    /* sort by article number */
    if (app_resources.sortedSubjects == ARTICLE_SORTED)
	qsort((void *) pteArray, treeSize, sizeof(struct pte *), pteCompare);

/*
 * Allocate a pointer array - with an end null string followed by
 * an end pointer of NULL
 */

    ar = ARRAYALLOC(art_num, (newsgroup->last - newsgroup->current + 3));
    for (i = 0; i <= (newsgroup->last - newsgroup->current);i++) {
	ar[i] = 0;
    }

    j = 0;
    for (i = 0; i < treeSize; i++) {
	for (k = 0; k < pteArray[i]->used; k++) {
	    ar[j++] = pteArray[i]->articleList[k];
	}
    }

    avl_free_table(tree, XtFree, valfree);
    FREE(pteArray);
    ar[j] = 0;
    *count = j;
    return ar;
}

/*
 * Build a list of article numbers in threaded order
 */
static art_num *
getThreadedArticles(mode, count)
int mode;
int *count;
{
    struct newsgroup *newsgroup = CurrentGroup;
    struct article *articles = GETARTICLES(newsgroup);
    struct artStat *artstat = GETSTATUS(newsgroup);
    struct article *art, *art1;
    art_num i, num;
    int j, k;
    art_num *ar;
    int num_arts = 0;
    struct entry *pte;
    struct entry **pteArray;
    avl_generator *gen;
    char *key, *ptr, *p;
    avl_tree *tree;
    char curSub[81];
    int treeSize, sz;
    char buffer[LINE_LENGTH];

    *count = 0;
    if (EMPTY_GROUP(newsgroup)) {
	return NIL(art_num);
    }

    if (mode == ALL) {
	newsgroup->initial = newsgroup->current = NextPreviousArticle;
    } else {
	setCurrentArticle(newsgroup, mode);
    }

    if (newsgroup->current > newsgroup->last) {
	return NIL(art_num);
    }

    NextPreviousArticle = newsgroup->current;

    if ((newsgroup->last - newsgroup->current + 1) < 0) {
	(void) sprintf(error_buffer, "Active File Error: last - current + 1 < 0 (%s)\n",
			       newsgroup->name);
	ehErrorExitXRN(error_buffer);
    }
	
/*
 * Allocate an array to store the article numbers.
 */
    ar = ARRAYALLOC(art_num, (newsgroup->last - newsgroup->current + 3));
    for (i = 0; i <= (newsgroup->last - newsgroup->current);i++) {
	ar[i] = 0;
    }

    msgids = avl_init_table((void *)strcmp);
    if (msgids == NULL) {
	(void) sprintf(error_buffer, "Can't build message ID hash table for %s\n",
			       newsgroup->name);
	ehErrorExitXRN(error_buffer);
    }

    /* 
     * Step 1: Initialize and build the message ID lookup table
     */
    num_arts = 0;
    for (i = newsgroup->current; i <= newsgroup->last; i++) {
	long indx = INDEX(i);

	/* canceled and empty articles will not have a subject entry */
	if (articles[indx].subject != NIL(char)) {

	    /* don't put articles in the string if already read ... */
	    if ((mode == ALL) ||
	        (mode == UNKILLED && IS_UNKILLED(artstat[indx]) &&
				     IS_ACTIVE(artstat[indx])) ||
		(mode == ACTIVE && IS_ACTIVE(artstat[indx])) ||
	        (mode == UNREAD && IS_UNREAD(artstat[indx]))) {
		if (mode != UNKILLED)
		    SET_ACTIVE(artstat[indx]);
		avl_insert(msgids, articles[indx].msgid, (char *)&articles[indx]);
		articles[indx].parent = 0;
		articles[indx].child = 0;
		ar[num_arts++] = i;
	    }
	} else {
	    /* if you can't get the subject, mark the article as unavailable */
	    SET_UNAVAIL(artstat[indx]);
	}
    }

    /* 
     * Step 2: find parents and link articles
     */
    for (i = 0; i < num_arts; i++) {
	long indx = INDEX(ar[i]);

	if (avl_lookup(msgids, articles[indx].refs[0], &ptr) ||
	    avl_lookup(msgids, articles[indx].refs[1], &ptr) ||
	    avl_lookup(msgids, articles[indx].refs[2], &ptr)) {
	    art1 = art = (struct article *) ptr;
	    articles[indx].parent = ar[i];
	    /*
	     * Find end of child chain
	     */
	    while (art1->child) {
		art1 = &articles[INDEX(art1->child)];
	    }
	    art1->child = ar[i];
	} 
    }

    /* 
     * Step 3: subject sort toplevel articles (no parents)
     */
    tree = avl_init_table(utSubjectCompare);

    for (i = 0; i < num_arts; i++) {
	long indx = INDEX(ar[i]);

	if (!articles[indx].parent) {
	    (void) strncpy(curSub, getSubject(ar[i]), sizeof(curSub));
	    curSub[sizeof(curSub)-1] = '\0';

	    if (avl_lookup(tree, curSub, &ptr)) {
		/* add to the end */
		pte = (struct entry *) ptr;
		if (pte->left <= 0) {
		    pte->left = pte->size;
		    pte->size = pte->size * 2;
		    pte->articleList =
			(art_num *) XtRealloc((char *)pte->articleList,
					     pte->size * sizeof(art_num));
		}
		pte->articleList[pte->used] = ar[i];
		pte->used++;
		pte->left--;
	    } else {
		/* create new */
		pte = ALLOC(struct entry);
		pte->startingArticle = ar[i];
		pte->articleList = ARRAYALLOC(art_num, 32);
		pte->used = 1;
		pte->size = 32;
		pte->left = pte->size - pte->used;
		pte->articleList[0] = ar[i];
		(void) avl_insert(tree, XtNewString(curSub), (char *) pte);
	    }
	}
    }
    i = 0;

    treeSize = avl_count(tree);
    if (treeSize == 0) {		/* No articles */
	avl_free_table(tree, XtFree, valfree);
	return NIL(art_num);
    }

    pteArray = ARRAYALLOC(struct entry *, treeSize);
    gen = avl_init_gen(tree, AVL_FORWARD);
    while (avl_gen(gen, &key, &ptr)) {
	pteArray[i++] = (struct entry *) ptr;
    }
    avl_free_gen(gen);

    /* sort by article number */
    qsort((void *) pteArray, treeSize, sizeof(struct pte *), pteCompare);

    /* 
     * Step 4: generate output in thread order
     */
    for (i = 0; i <= (newsgroup->last - newsgroup->current);i++) {
	ar[i] = 0;
    }

    j = 0;
    for (i = 0; i < treeSize; i++) {
	for (k = 0; k < pteArray[i]->used; k++) {
	    ar[j++] = num = pteArray[i]->articleList[k];
	    art1 = &articles[INDEX(num)];
	    while (art1->child) {
		ar[j++] = art1->child;
		art1 = &articles[INDEX(art1->child)];
	    }
	}
    }

    avl_free_table(tree, XtFree, valfree);
    avl_free_table(msgids, NULL, NULL);
    FREE(pteArray);
    ar[j] = 0;
    *count = j;
    return ar;
}


art_num *
getArticleNumbers(mode, count)
int mode;
int *count;
{
    if (app_resources.displayLineCount) {
	sprintf(formatString,"  %%5ld:%%-%d\056%ds %%6.6s %%-%d.%ds",
		app_resources.subjectLength, app_resources.subjectLength,
		app_resources.authorLength, app_resources.authorLength);
    } else {
	sprintf(formatString,"  %%5ld:%%-%d\056%ds  %%-%d.%ds",
		app_resources.subjectLength, app_resources.subjectLength,
		app_resources.authorLength, app_resources.authorLength);
    }
    if (app_resources.sortedSubjects == THREAD_SORTED) {
	return getThreadedArticles(mode, count);
    } 
    if (app_resources.sortedSubjects) {
	return getSortedArticles(mode, count);
    } else {
	return getUnSortedArticles(mode, count);
    }
}

/*
 * set the internal pointers to a particular article
 */
void
gotoArticle(article)
art_num article;
{
    struct newsgroup *newsgroup = CurrentGroup;

    newsgroup->current = (art_num) article;
    NextPreviousArticle = (art_num) article;
    return;
}

/*
 * set the internal pointers to the first article
 */
void
gotoFirstArticle()
{
    struct newsgroup *newsgroup = CurrentGroup;
    art_num article = newsgroup->initial;

    newsgroup->current = (art_num) article;
    NextPreviousArticle = (art_num) article;
    return;
}

int
checkArticle(art)
art_num art;
{
    struct newsgroup *newsgroup = CurrentGroup;

    /* Check if requested article is not available */
    if (art < newsgroup->first || art > newsgroup->last) {
        return XRN_ERROR;
    }
    return XRN_OKAY;
}    

/*
 * first and last are the same and there are no articles
 *
 * representation bug in the news system active file
 *
 */
void
bogusNewsgroup()
{
    struct newsgroup *newsgroup = CurrentGroup;

    if (newsgroup->artStatus != NIL(struct artStat)) {
	SET_READ(newsgroup->artStatus[LAST]);
	SET_UNAVAIL(newsgroup->artStatus[LAST]);
    }

    currentArticles = NULL;
    mesgPane(XRN_INFO, "No articles in `%s', probably killed or canceled", newsgroup->name);
    
    return;
}


#define STRIPLEADINGSPACES   for (; *start == ' '; start++);
#define STRIPENDINGSPACES  for ( ; *end == ' '; *end = '\0', end--);

static char *
strip(str, striprefs)
char *str;
Boolean striprefs;
{
    register char *start, *end, *ptr;
    static char work[BUFFER_SIZE];

    if (str == NIL(char)) {
	return NIL(char);
    }
    (void) strncpy(work, str, BUFFER_SIZE);
    start = work;
    work[BUFFER_SIZE - 1] = '\0';
    end = index(start, '\0') - 1;

    STRIPLEADINGSPACES;

    /*
     * strip leading '[rR][eE]: ' and 'Re^N: ' -
     * only if striprefs is TRUE (want to be able to kill follow-ups)
     */
    if (striprefs) {
	while (STREQN(start, "Re: ", 4) ||
	       STREQN(start, "RE: ", 4) ||
	       STREQN(start, "re: ", 4) ||
	       STREQN(start, "Re: ", 4)) {
	    start += 4;
	
	    /* strip leading spaces after '[rR]e: ' */
	    STRIPLEADINGSPACES;
	}

	while (STREQN(start, "Re^", 3)) {
	    start += 3;
	    ptr = index(start, ':');
	    if (ptr != NIL(char)) {
		start = ptr + 1;
	    }
	    STRIPLEADINGSPACES;
	}

	for (end = start; (end = index(end, '(')) != NULL;)
	  if (STREQN(end,"(was:",5)
	      ||  STREQN(end,"(Was:",5)
	      ||  STREQN(end,"(WAS:",5)) {
	      *end = '\0';
	      break;
	  }
	  else ++end;
    }

    end = index(start, '\0') - 1;
    STRIPENDINGSPACES;

    return start;
}

/*
 * return the subject of an article with trailing/leading spaces stripped,
 * leading '[rR]e: ' stripped, and trailing ' ([wW]as: ' stripped
 */
char *
getSubject(article)
art_num article;
{
    struct newsgroup *newsgroup = CurrentGroup;
    art_num artnum = (art_num) article;

    SETARTICLES(newsgroup);

    return strip(currentArticles[INDEX(artnum)].subject, True);
}

/*
 * return the author of an article
 */
char *
getAuthor(article)
art_num article;
{
    struct newsgroup *newsgroup = CurrentGroup;
    struct article *articles = GETARTICLES(newsgroup);

    return articles[INDEX((art_num) article)].author;
}

/*
 * return the text of an article
 */
char *
getText(article)
art_num article;
{
    struct newsgroup *newsgroup = CurrentGroup;
    struct article *articles = GETARTICLES(newsgroup);

    return articles[INDEX((art_num) article)].text;
}

/*
 * get the previous subject (article number is NextPreviousArticle).
 * only called when going off the top of the subject string
 *
 *   returns a point to a static area
 *
 *  NextPreviousArticle is set to current on building the subject string.
 *  NextPreviousArticle is decremented by this routine.
 */
char *
getPrevSubject()
{
    struct newsgroup *newsgroup = CurrentGroup;
    struct article *articles = GETARTICLES(newsgroup);

    /* search for the next available article in the reverse direction */
    for (NextPreviousArticle--;
	 NextPreviousArticle >= newsgroup->first; NextPreviousArticle--) {
	long indx = INDEX(NextPreviousArticle);
	
	if (NextPreviousArticle < newsgroup->initial)
	    newsgroup->initial = NextPreviousArticle;

	/* get the subject (and author) if it does not already exist */
	if (articles[indx].subject == NIL(char)) {

	    /* get the subject and a few more */
	    getsubjectlist(newsgroup,
			   MAX(newsgroup->first, NextPreviousArticle - SUBJECTS),
			   NextPreviousArticle, False);
	}

	if (articles[indx].author == NIL(char)) {
	    getauthorlist(newsgroup,
			  MAX(newsgroup->first, NextPreviousArticle - SUBJECTS),
			   NextPreviousArticle, False);

	}

	if (articles[indx].lines == NIL(char)) {
	    getlineslist(newsgroup,
			 MAX(newsgroup->first, NextPreviousArticle - SUBJECTS),
			 NextPreviousArticle, False);
	}

	if (articles[indx].msgid == NIL(char)) {
	    getmsgidlist(newsgroup,
			 MAX(newsgroup->first, NextPreviousArticle - SUBJECTS),
			 NextPreviousArticle, False);
	}

	if (articles[indx].refs[0] == NIL(char)) {
	    getrefslist(newsgroup,
			 MAX(newsgroup->first, NextPreviousArticle - SUBJECTS),
			 NextPreviousArticle, False);
	}

	if (articles[indx].subject) {
	    (void) getSubjectLine(NextPreviousArticle);
	    return subjectline;
	}
	/* continue on */
    }

    return NIL(char);
}


static art_num justInCase;   /* old NextPreviousArticle, just in case the search fails */

void
startSearch()
/* the front-end is about to do an article search, save the starting point */
{
    justInCase = NextPreviousArticle;
    return;
}


void
failedSearch()
/* the article search failed, restore the original point */
{
    NextPreviousArticle = justInCase;
    return;
}


void
fillUpArray(art)
art_num art;
{
    struct newsgroup *newsgroup = CurrentGroup;

    if (art < newsgroup->current) {
	if (!getoverview(newsgroup, art, newsgroup->current - 1, False)) {
	    getsubjectlist(newsgroup, art, newsgroup->current - 1, False);
	    getauthorlist(newsgroup, art, newsgroup->current - 1, False);
	    getlineslist(newsgroup, art, newsgroup->current - 1, False);
	    getmsgidlist(newsgroup, art, newsgroup->current - 1, False);
	    getrefslist(newsgroup, art, newsgroup->current - 1, False);
	}
    }
    return;
}


/*
 * getinfofromfile	Get a string from a named file
 *			Handle white space and comments.
 *
 *	Parameters:	"file" is the name of the file to read.
 *
 *	Returns:	Pointer to static data area containing the
 *			first non-ws/comment line in the file.
 *			NULL on error (or lack of entry in file).
 *
 *	Side effects:	None.
 */

char *
getinfofromfile(file)
char	*file;
{
	register FILE	*fp;
	register char	*cp;
	static char	buf[256];
	char		*getenv();

	if (file == NULL)
		return (NULL);

	fp = fopen(file, "r");
	if (fp == NULL)
		return (NULL);

	while (fgets(buf, sizeof (buf), fp) != NULL) {
		if (*buf == '\n' || *buf == '#')
			continue;
		cp = index(buf, '\n');
		if (cp)
			*cp = '\0';
		(void) fclose(fp);
		return (buf);
	}

	(void) fclose(fp);
	return (NULL);			 /* No entry */
}

char *
getSubjectLine(art)
art_num art;
{
    struct newsgroup *newsgroup = CurrentGroup;
    struct article *articles = GETARTICLES(newsgroup);
    struct artStat *artstat = GETSTATUS(newsgroup);
    long indx = INDEX(art);

    subjectline[0] = '\0';
    if (art < newsgroup->first || art > newsgroup->last)
	return subjectline;
    if (app_resources.displayLineCount) {
	(void) sprintf(subjectline, formatString, 
		       art,
		       (articles[indx].subject != NIL(char) ?
		       articles[indx].subject : " "),
		       (articles[indx].lines != NIL(char) ?
		       articles[indx].lines : " "),
		       (articles[indx].author == NIL(char) ?
		       "(none)" : articles[indx].author));
    } else {
	(void) sprintf(subjectline, formatString,
		       art,
		       (articles[indx].subject != NIL(char) ?
		       articles[indx].subject : " "),
		       (articles[indx].author == NIL(char) ?
		       "(none)" : articles[indx].author));
    }
    /* mark articles if they have already been read */
    if (IS_READ(artstat[indx])) {
	subjectline[0] = '+';
    } else {
	subjectline[0] = ' ';
    }
    if (IS_KILLED(artstat[indx])) {
	subjectline[0] = 'k';
    }
    if (IS_UNREAD(artstat[indx]) && IS_MARKED(artstat[indx])) {
	subjectline[0] = 'u';
    }
    if (IS_SAVED(artstat[indx])) {
	subjectline[1] = 'S';
    } else if(IS_PRINTED(artstat[indx])) {
	subjectline[1] = 'P';
    } else {
	subjectline[1] = ' ';
    }
    return subjectline;
}
