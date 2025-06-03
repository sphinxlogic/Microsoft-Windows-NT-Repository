
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/server.c,v 1.10 1993/02/04 18:22:28 ricks Exp $";
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
 * server.c: routines for communicating with the NNTP remote news server
 *
 */

#include "copyright.h"
#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#ifndef VMS
#include <sys/param.h>
#include <sys/wait.h>
#else
#define MAXPATHLEN 512
#define index strchr
#endif /* VMS */
#include <errno.h>
#include "config.h"
#include "utils.h"
#include "xrn.h"
#include "avl.h"
#include "news.h"
#include "mesg.h"
#include "error_hnds.h"
#include "resources.h"
#include "server.h"
#include "dialogs.h"
#include "xthelper.h"
#include "xmisc.h"

#if defined(sun) && defined(sparc) && !defined(SOLARIS)
#include <vfork.h>
#endif

#ifdef INEWS
#include <sys/stat.h>
#endif

#define BUFFER_SIZE 1024
#define MESSAGE_SIZE 1024
#define MAXFIELDS 9
#define DEFREFS 20

#ifdef __STDC__
#include <stdlib.h>
#include <time.h>
#ifndef VMS
#include <unistd.h>
#else
extern void sleep();
extern int mailArticle();
#endif
#ifdef VFORK_SUPPORTED
#ifndef SVR4_0
extern int vfork();
#else
extern pid_t vfork(void);
#endif
#endif
#else
#if !defined(VMS) && !defined(SVR4) && !defined(ultrix)
#if defined(__convexc__)
extern unsigned sleep();
#else
#if !defined(__osf__)
extern void sleep();
#endif
#endif
#endif
#endif

extern Boolean allowErrors;
extern struct article *currentArticles;
extern struct article *savedArt;
extern int artCount;

int ServerDown = 0;
static char mybuf[MESSAGE_SIZE+100] = 
	"The news server is not responding correctly, aborting\n";

static struct newsgroup *currentNewsgroup = 0;
#define SETNEWSGROUP(n) (((n) == currentNewsgroup) ? 0 : getgroup((n), 0, 0, 0))

/*
 * get data from the server (active file, article)
 *
 *  on error, sets 'ServerDown'
 *
 *   returns: void
 */
static void
get_data_from_server(str, size)
char *str;     /* string for message to be copied into */
int size;      /* size of string                       */
{
    xthHandleAllPendingEvents();
    str[0] = '\0';
    if (ServerDown) return;
    if (get_server(str, size) < 0) {
	ServerDown = 1;
    } else {
	ServerDown = 0;
    }
    return;
}


static void
check_time_out(command, response, size, pause)
char *command;  /* command to resend           */
char *response; /* response from the command   */
int size;       /* size of the response buffer */
Boolean pause;	/* True if pause should be checked */
{
    /*
     * try to recover from a timeout
     *
     *   this assumes that the timeout message stays the same
     *   since the error number (503) is used for more than just
     *   timeout
     *
     *   Message is:
     *     503 Timeout ...
     */

    int old = ActiveGroupsCount;
    struct newsgroup *wanted = currentNewsgroup;
    char message[512];
    int status;

    if (ServerDown || STREQN(response, "503 Timeout", 11)) {

        if (XtIsRealized(TopLevel)) {
	    xmSetIconAndName(LostIcon);
	    XFlush(XtDisplay(TopLevel));
	}
	mesgPane(XRN_SERIOUS, "Lost connection to the NNTP server, attempting to reconnect");
	start_server(NIL(char));

	mesgPane(XRN_INFO, "Reconnected to the NNTP server");
        if (XtIsRealized(TopLevel)) {
	    xmSetIconAndName(UnreadIcon);
	    XFlush(XtDisplay(TopLevel));
	}

	/*
	 * if it was an ARTICLE or XHDR or HEAD command, then you must get the
	 * server into the right state (GROUP mode), so resend the last
	 * group command
	 */
	if (STREQN(command, "ARTICLE", 7) || STREQN(command,"XHDR", 4) ||
	    STREQN(command, "HEAD", 4) || STREQN(command,"XOVER", 5)) {
	    if (wanted) {
		currentNewsgroup = 0;
		if (getgroup(wanted, NIL(art_num), NIL(art_num), NIL(int))
			== NO_GROUP) {
		    return;
		}		
		currentNewsgroup = wanted;
		/* XXX should do some processing of changed first/last numbers */
	    }
	}

	
	if (put_server(command, True) == -1)
	    ServerDown = True;
	get_data_from_server(response, size);
    }
    if (pause && STREQN(response, "400 ", 4)) {
        if (XtIsRealized(TopLevel)) {
	    xmSetIconAndName(LostIcon);
	    XFlush(XtDisplay(TopLevel));
	}
	sprintf(message, "Error from news server: %s - retry?", response);
	status = ConfirmationBox(TopLevel, message);
	if (!status)
	    ehCleanExitXRN();
	
	if (put_server(command, True) == -1)
	    ServerDown = True;
	get_data_from_server(response, size);
    }
    
    return;
}

/*
 * retrieve article number 'artnumber' in the current group, update structure
 *
 *   returns:  article text or NIL(char) if the article is not avaiable
 *
 */
char *
#ifdef XLATE
getarticle(artnumber, position, header, rotation,xlation)
#else
getarticle(artnumber, position, header, rotation)
#endif
art_num artnumber; /* number of article in the current group to retrieve */
int *position;	   /* byte position of header/body seperation            */
int header, rotation;
#ifdef XLATE
int xlation;
#endif
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE], *msg;
#ifdef REALLY_USE_LOCALTIME
    char temp[MESSAGE_SIZE];
#endif
    static char	*artstring = NULL;
    char *newartstring = NULL;
    static int	artstringsize = 0;
    char *ptr;
    char *endPtr;
    int  segSize;
    char field[BUFFER_SIZE];
    int byteCount = 0, lineCount = 0;
    int last_stripped = 0;

    if (artstring == NULL) {
	artstring = XtMalloc(100000);
        artstringsize = 100000;
    }
    *position = 0;
    *artstring = '\0';
    
    /* send ARTICLE */
    (void) sprintf(command, "ARTICLE %ld", artnumber);
    if (put_server(command, True) == -1)
	ServerDown = True;
    get_data_from_server(message, sizeof(message));

    check_time_out(command, message, sizeof(message), True);

    if (*message != CHAR_OK) {
	/* can't get article */
	return(NIL(char));
    }

    endPtr = artstring;
    for (;;) {
	if (byteCount + MESSAGE_SIZE > artstringsize) {
	    newartstring = XtMalloc(artstringsize + 100000);
	    artstringsize += 100000;
	    strcpy (newartstring, artstring);
	    XtFree(artstring);
	    artstring = newartstring;
	    endPtr = artstring + byteCount;
	}
	get_data_from_server(message, sizeof(message));
	if (ServerDown) {
	    check_time_out(command, message, sizeof(message), False);
	    if (*message != CHAR_OK) {
		/* can't get article */
		return(NIL(char));
	    }
	    byteCount = 0;
	    endPtr = artstring;
	    continue;
	}	    

	/* the article is ended by a '.' on a line by itself */
	if ((message[0] == '.') && (message[1] == '\0')) {
	    /* check for a bogus message */
	    if (byteCount == 0) {
		return(NIL(char));
	    }
	    break;
	}

	msg = &message[0];

	/* find header/body seperation */
	if (*position == 0) {
	    if (*msg == '\0') {
		*position = byteCount;
	    }
	}
	      
	if (*msg == '.') {
	    msg++;
	}

	if (*msg != '\0') {
	    /* strip leading ^H */
	    while (*msg == '\b') {
		msg++;
	    }
	    /* strip '<character>^H' */
	    for (ptr = index(msg + 1, '\b'); ptr != NIL(char); ptr = index(ptr, '\b')) {
		if (ptr - 1 < msg) {
		    /* too many backspaces, kill all leading back spaces */
		    while (*ptr == '\b') {
		        (void) strcpy(ptr, ptr + 1);
			ptr++;
		    }
		    break;
		}
		(void) strcpy(ptr - 1, ptr + 1);
		ptr--;
	    }

#ifdef REALLY_USE_LOCALTIME
  	    if (app_resources.displayLocalTime && !strncmp(msg, "Date: ", 6)) {
		  tconvert(temp, msg+6);
		  (void) strcpy(msg+6, temp);
	    }
#endif
	    /* strip the headers */
	    if ((*position == 0) && (header == NORMAL_HEADER)) {
		if ((*msg == ' ') || (*msg == '\t')) { /* continuation line */
		    if (last_stripped)
			continue;
		} else {
		    if ((ptr = index(msg, ':')) == NIL(char)) {
			continue;    /* weird header line, skip */
		    }
		    if (*(ptr+1) == '\0') {
			continue;    /* empty field, skip */
		    }
		    (void) strncpy(field, msg, (int) (ptr - msg));
		    field[(int) (ptr - msg)] = '\0';
		    utDowncase(field);
		    if (avl_lookup(app_resources.headerTree, field, &ptr)) {
			if (app_resources.headerMode == STRIP_HEADERS) {
			    last_stripped = 1;
			    continue;
			} else {
			    last_stripped = 0;
			}
		    } else {
			if (app_resources.headerMode == LEAVE_HEADERS) {
			    last_stripped = 1;
			    continue;
			} else {
			    last_stripped = 0;
			}
		    }
		}
	    }

	    /* handle rotation of the article body */
	    if ((rotation == ROTATED) && (*position != 0)) {
		for (ptr = msg; *ptr != '\0'; ptr++) {
		    if (isalpha(*ptr)) {
			if ((*ptr & 31) <= 13) {
			    *ptr = *ptr + 13;
			} else {
			    *ptr = *ptr - 13;
			}
		    }
		}
	    }

#ifdef XLATE
	    /* handle translation of the article body */
	    if ((xlation == XLATED) && (*position != 0))
		utXlate(msg);
#endif /* XLATE */

	    /* handle ^L (poorly?) */
	    if (*msg == '\014') {
		int i, lines;
		lines = articleLines();
		lines -= lineCount % lines;
		for (i = 0; i < lines; i++) {
		    (void) strcpy(endPtr, "\n");
		    endPtr++;
		}
		byteCount += lines;
		lineCount += lines;
		msg++;
	    }
	    (void) strcpy(endPtr, msg);
	}
	segSize = utStrlen(msg);
	byteCount += segSize + 1;
	endPtr += segSize;
	*endPtr++ = '\n';
	*endPtr = '\0';
	lineCount++;
    }

    return( (char *) XtNewString(artstring));
}

/*
 * enter a new group and get its statistics (and update the structure)
 *   allocate an array for the articles and process the .newsrc article
 *   info for this group
 *
 *   returns: NO_GROUP on failure, 0 on success
 *
 */
int
getgroup(newsgroup, first, last, number)
struct newsgroup *newsgroup;    /* group                      */
art_num *first; 		/* first article in the group */
art_num *last;  		/* last article in the group  */
int *number;    		/* number of articles in the group,
				 if 0, first and last are bogus */
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE];
    char group[GROUP_NAME_SIZE];
    static long code, num, count, frst, lst;

    if (!newsgroup) {
	newsgroup = currentNewsgroup;
    }

    if (!newsgroup) {
	/* this shouldn't ever happen, but let's be cautious */
	ehErrorExitXRN("getgroup(NIL) called with no currentNewsgroup");
    }

    (void) sprintf(command, "GROUP %s", newsgroup->name);
    if (put_server(command, True) == -1)
	ServerDown = True;

    get_data_from_server(message, sizeof(message));

    check_time_out(command, message, sizeof(message), True);
    
    if (*message != CHAR_OK) {
	if (atoi(message) != ERR_NOGROUP) {

	    (void) strcat(mybuf, "	Request was: ");
	    (void) strcat(mybuf, command);
	    (void) strcat(mybuf, "\n");
	    (void) strcat(mybuf, "	Failing response was: ");
	    (void) strcat(mybuf, message);
	    ehErrorExitXRN(mybuf);
	}
	mesgPane(XRN_SERIOUS,
	"Can't get the group \"%s\", looks like it was deleted out from under us",
			newsgroup->name);
	
	/* remove the group from active use ??? */
	
	return(NO_GROUP);
    }
    currentNewsgroup = newsgroup;

    /* break up the message */
#if GROUP_NAME_SIZE <= 127
	"GROUP_NAME_SIZE is too small" /* this will produce a compilation */
	     /* error */
#endif
    count = sscanf(message, "%ld %ld %ld %ld %127s",
		   &code, &num, &frst, &lst, group);
    assert(count == 5);

    if (number != NIL(int)) {
	*number = num;
    }
    if (first != NIL(art_num)) {
	*first = frst;
    }
    if (last != NIL(art_num)) {
	*last = lst;
    }

    return(0);
}


static Boolean error_retry = False;
/*
 * get a list of all active newsgroups and create a structure for each one
 *
 *   returns: void
 */
void
getactive()
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE], group[GROUP_NAME_SIZE];
    char type[MESSAGE_SIZE];
    struct newsgroup *newsgroup;
    long first, last;
    char *ptr;
    int error_count = 0;
#ifdef SYSV_REGEX
    extern char *regcmp();
    extern char *regex();
    static char **re_cache = 0;
#else
    extern char *re_comp();
#endif
    char **re_list = app_resources.ignoreNewsgroupsList, **re_ptr;

#ifdef SYSV_REGEX
    if (!re_cache && re_list) {
	int i = 0, re_num;
	while (re_list[i]) {
	    i++;
	}
	re_cache = (char **) XtMalloc(sizeof(char *) * (i + 1));
	if (!re_cache) {
	    ehErrorExitXRN("out of memory");
	}
	for (re_num = 0; re_num < i; re_num++) {
	    if (re_cache[re_num] = regcmp(re_list[re_num], 0)) {
		re_num++;
	    }
	}
	re_cache[re_num] = 0;
    }
    re_list = re_cache;
#endif

    /*
     * It *is* necessary to reset currentNewsgroup to 0 when getactive
     * is called, even though the NNTP server does not forget its idea
     * of the current newsgroup when a LIST command is issued.  To
     * understand why this is so, imagine the following sequence of
     * events:
     *
     * 1) User reads newsgroup foo.bar.
     * 2) User exits from newsgroup, but currentNewsgroup remains
     *    foo.bar.
     * 3) User rescans.
     * 4) The active file that gets sent as a result of the LIST
     *    command says that there are new articles in foo.bar.
     * 5) HOWEVER, even though the active file says that, the NNTP
     *    server has not yet realized that, because it doesn't update
     *    its idea of what's in the current newsgroup unless a GROUP
     *    command is issued, i.e., it ignores the LIST data it sends
     *    over the wire.
     * 6) User tries to reenter newsgroup.
     * 7) Since currentNewsgroup is set to foo.bar, xrn doesn't send
     *    GROUP command.  Therefore, NNTP server does not update its
     *    idea of low and high articles for the newsgroup.
     * 8) Therefore, the XHDR commands to get header information from
     *    the new articles fail, because the server doesn't know that
     *    those articles exist.
     *
     * We solve this problem by always requiring a GROUP command when
     * first accessing a group after a LIST command, by setting
     * currentNewsgroup to 0.
     */

    currentNewsgroup = 0;
    (void) strcpy(command, "LIST");
    if (put_server(command, True) == -1)
	ServerDown = True;
    get_data_from_server(message, sizeof(message));

    check_time_out(command, message, sizeof(message), True);
    
    if (*message != CHAR_OK) {

	(void) strcat(mybuf, "\tRequest was: ");
	(void) strcat(mybuf, command);
	(void) strcat(mybuf, "\n");
	(void) strcat(mybuf, "\tFailing response was: ");
	(void) strcat(mybuf, message);
	ehErrorExitXRN(mybuf);
    }

    for (;;) {
	get_data_from_server(message, sizeof(message));
	if (ServerDown) {
	    check_time_out(command, message, sizeof(message));
	    continue;
	}
	
	/* the list is ended by a '.' at the beginning of a line */
	if ((message[0] == '.') && (message[1] == '\0')) {
	    break;
	}

	if (*message == '\0') {
	    continue;
	}
	/* server returns: group last first y/m/x/=otherGroup */
#if GROUP_NAME_SIZE <= 127
	"GROUP_NAME_SIZE is too small" /* this will produce a compilation */
	     			       /* error */
#endif
	if (sscanf(message, "%127s %ld %ld %s", group, &last, &first, type) != 4) {
	    mesgPane(XRN_SERIOUS, "Bogus active file entry, skipping\n%s", message);
	    error_count++;
	    if (error_count++ > 20) {	/* More than 20 bad entries in a row */
		if (!error_retry) {
		    mesgPane(XRN_SERIOUS, "Too many errors.. attempting restart");
		    error_retry = True;
		    start_server(NIL(char));
		    getactive();		
		    return;
		} else {
		    ehErrorExitXRN("Repeated errors from getactive(); exiting");
		}
	    }
	    continue;
	}

	error_count = 0;
	if (type[0] == 'x') {
	    /* bogus newsgroup, pay no attention to it */
	    continue;
	}

	if (type[0] == '=') {
	    /* This newsgroup doesn't exist, it's just an alias */
	    continue;
	}

#ifndef NO_BOGUS_GROUP_HACK
	/* determine if the group name is screwed up - check for jerks who
	 * create group names like: alt.music.enya.puke.puke.pukeSender: 
	 * - note that there is a ':' in the name of the group... */

	if (strpbrk(group, ":! \n\t")) {
	    /* bogus newsgroup - can't have a separator */
	    continue;
	}
#endif /* NO_BOGUS_GROUP_HACK */

	for (re_ptr = re_list; re_ptr && *re_ptr; re_ptr++) {
#ifdef SYSV_REGEX
	    if (regex(*re_ptr, group))
#else
	    if ((! re_comp(*re_ptr)) && re_exec(group))
#endif
	    {
#ifdef DEBUG
		fprintf(stderr, "Ignoring %s.\n", group);
#endif
		break;
	    }
	}
	if (re_ptr && *re_ptr) {
	    continue;
	}

 	if (first == 0) {
	    first = 1;
	}

	if (!avl_lookup(NewsGroupTable, group, &ptr)) {

	    /* no entry, create a new group */
	    newsgroup = ALLOC(struct newsgroup);
	    newsgroup->name = (char *) XtNewString(group);
	    newsgroup->newsrc = NOT_IN_NEWSRC;
	    newsgroup->status = NG_NOENTRY;
	    newsgroup->first = first;
	    newsgroup->last = last;
	    newsgroup->initial = first;
	    newsgroup->nglist = 0;
	    newsgroup->artStatus = NULL;
	    
	    switch (type[0]) {
		case 'y':
		newsgroup->status |= NG_POSTABLE;
		break;

		case 'm':
		newsgroup->status |= NG_MODERATED;
		break;

		case 'n':
		newsgroup->status |= NG_UNPOSTABLE;
		break;

#ifndef INN
		case '=':
		newsgroup->status |= NG_ALIAS;
#endif
		default:

		/*
		fprintf(stderr, "unexpected type (%s) for newsgroup %s\n",
			type, newsgroup->name);
		*/
		break;

	    }
	
	    if (avl_insert(NewsGroupTable, newsgroup->name,
			   (char *) newsgroup) < 0) {
		 ehErrorExitXRN("out of memory");
	    }

	    ActiveGroupsCount++;
	    
	} else {
	    
	    /*
	     * entry exists, use it; must be a rescanning call
	     *
	     * just update the first and last values and adjust the
	     * articles array
	     */
	    
	    newsgroup = (struct newsgroup *) ptr;

	    /*
	     * only allow last to increase or stay the same
	     * - don't allow bogus last values to trash a group
	     */
	    if (IS_SUBSCRIBED(newsgroup) && last >= newsgroup->last) {
		/* XXX really should save up the resync and use the GROUP info also */
		articleArrayResync(newsgroup, first, last, 1);
	    }
	}
    }

    error_retry = False;
    return;
}

#if !defined(FIXED_C_NEWS_ACTIVE_FILE) && !defined (FIXED_ACTIVE_FILE)
/*
 * check the case where the first article number information is not
 * being updated.
 *
 * also check the case where the first and last article numbers are equal
 * - unfortunately, this means two different things:
 *   1) there are no articles in the group
 *   2) there is one article in the group
 *
 * - so, to get rid of the ambiguity, we make a GROUP call
 *   and look at the 'number' of articles field to determine
 *   whether there are 0 or 1 articles
 */
void
fixLowArticleNumbers()
{
    avl_generator *gen;
    char *key, *value;
    art_num first, last;
    int number;

    /* check out first == last groups */
    gen = avl_init_gen(NewsGroupTable, AVL_FORWARD);
    if (! gen) {
	 ehErrorExitXRN("out of memory");
    }

    while (avl_gen(gen, &key, &value)) {
	struct newsgroup *newsgroup = (struct newsgroup *) value;

	if (IS_SUBSCRIBED(newsgroup)) {
	    if (getgroup(newsgroup, &first, &last, &number) != 0) continue;
	    if (newsgroup->artStatus) {
		articleArrayResync(newsgroup, first, last, number);
	    } else {
		newsgroup->first = first;
		newsgroup->last = last;
	    }
	    if (newsgroup->initial < newsgroup->first)
		newsgroup->initial = newsgroup->first;
	}
    }
    avl_free_gen(gen);

    return;
}
#endif
static int popupCount = 0;
extern Boolean CBretVal;		/* Confirmation box value;set from timer */
static XtIntervalId tryTimer = 0;

static void popDown()
{
    CBretVal = True;
    tryTimer = 0;
}
static Boolean AnotherTry(errorType)
int errorType;
{
    int i;
    Boolean status;

    popupCount++;
    if (popupCount < app_resources.retryPopupCount) {
	for (i = 0; i < app_resources.retryPause; i++) {
	    xthHandleAllPendingEvents();
	    sleep(1);
	}
	return True;
    }
    allowErrors = True;
    if (app_resources.retryPopupTimeout > 0) {
	tryTimer = XtAddTimeOut(app_resources.retryPopupTimeout * 1000,
				(XtTimerCallbackProc) popDown, 0);
    }
    if (errorType == 0) {
	status = ConfirmationBox (TopLevel,
	"Failed to reconnect to the NNTP server (server_init), retry?");
    } else {
	status = ConfirmationBox (TopLevel,
	"Invalid response from the NNTP server (server_init), retry?");
    }
    if (tryTimer != 0) {
	XtRemoveTimeOut(tryTimer);
	tryTimer = 0;
    }
    popupCount = 0;
    allowErrors = False;
    return status;
}

/*
 * initiate a connection to the news server
 *
 * nntpserver is the name of an alternate server (use the default if NULL)
 *
 * the server eventually used is remembered, so if this function is called
 * again (for restarting after a timeout), it will use it.
 *
 *   returns: void
 *
 */
void
start_server(nntpserver)
char *nntpserver;
{
    static char *server = NIL(char);   /* for restarting */
    int response, retryLimit;

    if (server == NIL(char)) {
	
	if (nntpserver != NIL(char)) {
	    server = nntpserver;
	
	} else {
	    
	    /* Note: INN's getserverbyfile will ignore the filename argument */
	    if ((server = getserverbyfile(SERVER_FILE)) == NULL) {
		mesgPane(XRN_SERIOUS, "Can't get the name of the news server from `%s'.%s",
			 SERVER_FILE,
#ifndef VMS
		"Either fix this file, or put NNTPSERVER in your environment."
#else
		"Either fix this file, or define logical NNTPSERVER"
#endif
		);
		ehErrorExitXRN("");
	    }
	}
    }

    for (retryLimit = 0;
	 retryLimit < app_resources.retryLimit * app_resources.retryPopupCount;
	 retryLimit++) {
	if ((response = server_init(server)) < 0) {
	    if (XtIsRealized(TopLevel) && retryLimit == 0) {
		xmSetIconAndName(LostIcon);
		XFlush(XtDisplay(TopLevel));
	    }
	    close_server();
	    if (!AnotherTry(0)) {
		ehCleanExitXRN(1);
	    }
	    continue;
	}
	if (handle_server_response(response, server) < 0) {
	    close_server();
	    if (!AnotherTry(1)) {
		ehCleanExitXRN();
	    }
	    continue;
	} else {
	    ServerDown = False;
	    popupCount = 0;
	    return;
	}
    }
    
    ehCleanExitXRN();
    ServerDown = True;
    popupCount = 0;
    return;
}

/*
 * close an outstanding connection to the NNTP server
 */
void stop_server()
{
    currentNewsgroup = 0;
    close_server();
}

/*
 * get a list of subject lines for the current group in the range
 *  'first' to 'last'
 *
 *   returns: void
 *
 * Note that XHDR is not part of the rfc977 standard, but is implemented
 * by the Berkeley NNTP server
 *
 */
void
getsubjectlist(newsgroup, artfirst, artlast, unreadonly)
struct newsgroup *newsgroup;
art_num artfirst;
art_num artlast;
int unreadonly;
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE];
    char *subjectline;
    art_num number;
    art_num first, last;

    if (SETNEWSGROUP(newsgroup)) {
	return;
    }

    first = artfirst;
    while (first <= artlast) {
	if (currentArticles[INDEX(first)].subject != NIL(char) ||
	    (unreadonly && IS_READ(newsgroup->artStatus[INDEX(first)]))) {
	     first++;
	     continue;
	}
	for (last = first + 1; last <= artlast; last++) {
	    if (currentArticles[INDEX(last)].subject != NIL(char) ||
		(unreadonly && IS_READ(newsgroup->artStatus[INDEX(last)]))) {
		break;
	    }
	}
	last--;

	(void) sprintf(command, "XHDR subject %ld-%ld", first, last);
	if (put_server(command, True) == -1)
	    ServerDown = True;
	get_data_from_server(message, sizeof(message));

	check_time_out(command, message, sizeof(message), True);

	/* check for errors */
	if (*message != CHAR_OK) {
	    mesgPane(XRN_SERIOUS,
"XRN: serious error, your NNTP server does not have XHDR support.\n\
Either you are running a pre-1.5 NNTP server or XHDR has\n\
not been defined in 'nntp/common/conf.h'\n\
XRN requires XHDR support to run.");
	    return;
	}

	for(;;) {

	    get_data_from_server(message, sizeof(message));

	    if (ServerDown) {
		check_time_out(command, message, sizeof(message), False);
		continue;
	    }
	
	    if ((message[0] == '.') && (message[1] == '\0')) {
		break;
	    }

	    /*
	     * message is of the form:
	     *
	     *    Number SubjectLine
	     *
	     *    203 Re: Gnumacs Bindings
	     *
	     * must get the number since not all subjects will be returned
	     */

	    number = atol(message);
	    subjectline = index(message, ' ');

	    if (number == 0 || subjectline == NULL) {
		mesgPane(XRN_SERIOUS,
		    "Bogus subject returned from server (%s), continuing..",
		     message);
		continue;
	    }
	    currentArticles[INDEX(number)].subject = string_pool(++subjectline);
	    SET_AVAIL(newsgroup->artStatus[INDEX(number)]);
	}
	first = last + 1;
    }
    return;
}

/*
 * get a list of author lines for the current group in the range
 *  'first' to 'last'
 *
 *   returns: void
 *
 * Note that XHDR is not part of the rfc977 standard, but is implemented
 * by the Berkeley NNTP server
 *
 */
void
getauthorlist(newsgroup, artfirst, artlast, unreadonly)
struct newsgroup *newsgroup;
art_num artfirst;
art_num artlast;
int unreadonly;
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE];
    char *author, *end, *brackbeg, *brackend;
    art_num number;
    art_num first, last;

    if (SETNEWSGROUP(newsgroup)) {
	return;
    }
    first = artfirst;
    while (first <= artlast) {
	if (currentArticles[INDEX(first)].author != NIL(char) ||
	    (unreadonly && IS_READ(newsgroup->artStatus[INDEX(first)]))) {
	    first++;
	    continue;
	}

	for (last = first + 1; last <= artlast; last++) {
	    if (currentArticles[INDEX(last)].author != NIL(char) ||
		(unreadonly && IS_READ(newsgroup->artStatus[INDEX(last)]))) {
		break;
	    }
	}
	last--;

	(void) sprintf(command, "XHDR from %ld-%ld", first, last);
	if (put_server(command, True) == -1)
	    ServerDown = True;
	get_data_from_server(message, sizeof(message));

	check_time_out(command, message, sizeof(message), True);

	/* check for errors */
	if (*message != CHAR_OK) {
	    mesgPane(XRN_SERIOUS,
"XRN: serious error, your NNTP server does not have XHDR support.\n\
Either you are running a pre-1.5 NNTP server or XHDR has\n\
not been defined in 'nntp/common/conf.h'\n\
XRN requires XHDR support to run.");
	    return;
	}
    
	for(;;) {

	    get_data_from_server(message, sizeof(message));
	    if (ServerDown) {
		check_time_out(command, message, sizeof(message), False);
		continue;
	    }
	
	    if ((message[0] == '.') && (message[1] == '\0')) {
		break;
	    }

	    /*
	     * message is of the form:
	     *
	     *    Number Author
	     *
	     *    201 ricks@shambhala (Rick L. Spickelmier)
	     *    202 Jens Thommasen <jens@ifi.uio.no>
	     *    203 <oea@ifi.uio.no>
	     *    302 "Rein Tollevik" <rein@ifi.uio.no>
	     *
	     * must get the number since not all authors will be returned
	     */

	    number = atol(message);
	    if (app_resources.authorFullName) {
		/* Can be made fancyer at the expence of extra cpu time */
		author = index(message, ' ');
		if (author == NIL(char)) {
		    strcat(message, " (None) ");
		    author = index(message, ' ');
		}
		assert(author != NIL(char));
		author++;

		/* First check for case 1, user@domain ("name") -> name */

		brackbeg = index(message, '(');
		brackend = index(message, '\0') - sizeof(char);
		/* brackend now points at the last ')' if this is case 1 */
		if (brackbeg != NIL(char) && (brackend > brackbeg) &&
		    (*brackend == ')')) {
		    author = brackbeg + sizeof(char);

		    /* Remove surrounding quotes ? */
		    if ((*author == '"') && (*(brackend - sizeof(char)) == '"')) {
			author++;
			brackend--;
		    }

		    /* Rather strip trailing spaces here */

		    *brackend = '\0';
		} else {
		    /* Check for case 2, "name" <user@domain> -> name */
		    brackbeg = index(message, '<');
		    if (brackbeg != NIL(char) && (index(brackbeg, '>') != NIL(char))
			&& (brackbeg > message)) {
			while (*--brackbeg == ' ')
			    ;

			/* Remove surrounding quotes ? */
			if ((*brackbeg == '"') && (*author ==  '"')) {
			    *brackbeg = '\0';
			    author++;

			    /* Rather strip trailing spaces here */

			} else {
			    *++brackbeg = '\0';
			}
		    } else {

			/* 
			 * Check for case 3, <user@domain> -> usr@domain
			 *
			 * Don't need to do this again:
			 * brackbeg = index(message, '<');
			 */

			brackend = index(message, '>');
			if ((author == brackbeg) && (brackend != NIL(char))) {
			    author++;
			    *brackend = '\0';
			} else {
			    if ((end = index(author, ' ')) != NIL(char)) {
				*end = '\0';
			    }
			}
		    }
		}
	    } else {
		if ((author = index(message, '<')) == NIL(char)) {
		    /* first form */
		    author = index(message, ' ');
		    if (author == NIL(char)) {
			strcat(message, " (None) ");
			author = index(message, ' ');
		    }
		    assert(author != NIL(char));
		    author++;
		    if ((end = index(author, ' ')) != NIL(char)) {
			*end = '\0';
		    }
		} else {
		    /* second form */
		    author++;
		    if ((end = index(author, '>')) != NIL(char)) {
			*end = '\0';
		    }
		}
	    }
	    /*
	     * do a final trimming - just in case the authors name ends
	     * in spaces or tabs - it does happen
	     */
	    end = author + strlen(author) - 1;
	    while ((end > author) && ((*end == ' ') || (*end == '\t'))) {
		*end = '\0';
		end--;
	    }
	    currentArticles[INDEX(number)].author = string_pool(author);
	}
	first = last + 1;
    }
    return;
}

/*
 * get a list of number of lines per message for the current group in the
 *  range 'first' to 'last'
 *
 *   returns: void
 *
 * Note that XHDR is not part of the rfc977 standard, but is implemented
 * by the Berkeley NNTP server
 *
 */
void
getlineslist(newsgroup, artfirst, artlast, unreadonly)
struct newsgroup *newsgroup;
art_num artfirst;
art_num artlast;
int unreadonly;
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE];
    char *numoflines, *end;
    art_num number;
    int lcv;
    art_num first, last;
 
    if (SETNEWSGROUP(newsgroup)) {
	return;
    }

    if (!app_resources.displayLineCount) {
	return;
    }

    first = artfirst;
    while (first <= artlast) {
	if (currentArticles[INDEX(first)].lines != NIL(char) ||
	    (unreadonly && IS_READ(newsgroup->artStatus[INDEX(first)]))) {
	    first++;
	    continue;
	}

	for (last = first + 1; last <= artlast; last++) {
	    if (currentArticles[INDEX(last)].lines != NIL(char) ||
		(unreadonly && IS_READ(newsgroup->artStatus[INDEX(last)]))) {
		break;
	    }
	}
	last--;

	(void) sprintf(command, "XHDR lines %ld-%ld", first, last);
	if (put_server(command, True) == -1)
	    ServerDown = True;
	get_data_from_server(message, sizeof(message));

	check_time_out(command, message, sizeof(message), True);

	/* check for errors */
	if (*message != CHAR_OK) {
	    mesgPane(XRN_SERIOUS,
"XRN: serious error, your NNTP server does not have XHDR support.\n\
Either you are running a pre-1.5 NNTP server or XHDR has\n\
not been defined in 'nntp/common/conf.h'\n\
XRN requires XHDR support to run.");
	    return;
	}

	for(;;) {

	    get_data_from_server(message, sizeof(message));
	    if (ServerDown) {
		check_time_out(command, message, sizeof(message), False);
		continue;
	    }

	    if ((message[0] == '.') && (message[1] == '\0')) {
		break;
	    }

	    /*
	     * message is of the form:
	     *
	     *    Number NumberOfLines
	     *
	     *    203 ##
	     *
	     * must get the number since not all subjects will be returned
	     */

	    number = atol(message);
	    numoflines = index(message, ' ');
	    assert(numoflines != NIL(char));
	    numoflines++;
	    if ((end = index(numoflines, ' ')) != NIL(char)) {
		*end = '\0';
	    }
	    if (numoflines[0] != '(') {
		numoflines[utStrlen(numoflines)+1] = '\0';
		numoflines[utStrlen(numoflines)] = ']';
		for (lcv = utStrlen(numoflines); lcv >= 0; lcv--) {
		    numoflines[lcv+1] = numoflines[lcv];
		}
		numoflines[0] = '[';
	    } else {
		numoflines[0] = '[';
		numoflines[utStrlen(numoflines)-1] = ']';
	    }
	    if (strcmp(numoflines, "[none]") == 0) {
		(void) strcpy(numoflines, "[?]");
	    }
	    currentArticles[INDEX(number)].lines = string_pool(numoflines);
	}
	first = last + 1;
    }
    return;
}

/*
 * get a list of message IDs for the current group in the range
 *  'first' to 'last'
 *
 *   returns: void
 *
 * Note that XHDR is not part of the rfc977 standard, but is implemented
 * by the Berkeley NNTP server
 *
 */
void
getmsgidlist(newsgroup, artfirst, artlast, unreadonly)
struct newsgroup *newsgroup;
art_num artfirst;
art_num artlast;
int unreadonly;
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE];
    char *messageid;
    art_num number;
    art_num first, last;

    if (SETNEWSGROUP(newsgroup)) {
	return;
    }

    first = artfirst;
    while (first <= artlast) {
	if (currentArticles[INDEX(first)].msgid != NIL(char) ||
	    (unreadonly && IS_READ(newsgroup->artStatus[INDEX(first)]))) {
	     first++;
	     continue;
	}
	for (last = first + 1; last <= artlast; last++) {
	    if (currentArticles[INDEX(last)].msgid != NIL(char) ||
		(unreadonly && IS_READ(newsgroup->artStatus[INDEX(last)]))) {
		break;
	    }
	}
	last--;

	(void) sprintf(command, "XHDR Message-ID %ld-%ld", first, last);
	if (put_server(command, True) == -1)
	    ServerDown = True;
	get_data_from_server(message, sizeof(message));

	check_time_out(command, message, sizeof(message), True);

	/* check for errors */
	if (*message != CHAR_OK) {
	    mesgPane(XRN_SERIOUS,
"XRN: serious error, your NNTP server does not have XHDR support.\n\
Either you are running a pre-1.5 NNTP server or XHDR has\n\
not been defined in 'nntp/common/conf.h'\n\
XRN requires XHDR support to run.");
	    return;
	}

	for(;;) {

	    get_data_from_server(message, sizeof(message));

	    if (ServerDown) {
		check_time_out(command, message, sizeof(message), False);
		continue;
	    }
	
	    if ((message[0] == '.') && (message[1] == '\0')) {
		break;
	    }

	    /*
	     * message is of the form:
	     *
	     *    Number MessageID
	     *
	     *    203 <12345@foo.bar.baz>
	     *
	     */

	    number = atol(message);
	    messageid = index(message, ' ');

	    if (number == 0 || messageid == NULL) {
		mesgPane(XRN_SERIOUS,
		    "Bogus message ID returned from server (%s), continuing..",
		     message);
		continue;
	    }
	    currentArticles[INDEX(number)].msgid = string_pool(++messageid);
	}
	first = last + 1;
    }
    return;
}

/*
 * split - divide a string into fields, like awk split()
 * Copied from Geoffrey Collyer's `nov' package.
 */
static int			/* number of fields, including overflow */
split(string, fields, nfields, sep)
char *string;
char *fields[];			/* list is not NULL-terminated */
int nfields;			/* number of entries available in fields[] */
char *sep;			/* "" white, "c" single char, "ab" [ab]+ */
{
	register char *p = string;
	register char c;			/* latest character */
	register char sepc = sep[0];
	register char sepc2;
	register int fn;
	register char **fp = fields;
	register char *sepp;
	register int trimtrail;

	/* white space */
	if (sepc == '\0') {
		while ((c = *p++) == ' ' || c == '\t')
			continue;
		p--;
		trimtrail = 1;
		sep = " \t";	/* note, code below knows this is 2 long */
		sepc = ' ';
	} else
		trimtrail = 0;
	sepc2 = sep[1];		/* now we can safely pick this up */

	/* catch empties */
	if (*p == '\0')
		return(0);

	/* single separator */
	if (sepc2 == '\0') {
		fn = nfields;
		for (;;) {
			*fp++ = p;
			fn--;
			if (fn == 0)
				break;
			while ((c = *p++) != sepc)
				if (c == '\0')
					return(nfields - fn);
			*(p-1) = '\0';
		}
		/* we have overflowed the fields vector -- just count them */
		fn = nfields;
		for (;;) {
			while ((c = *p++) != sepc)
				if (c == '\0')
					return(fn);
			fn++;
		}
		/* not reached */
	}

	/* two separators */
	if (sep[2] == '\0') {
		fn = nfields;
		for (;;) {
			*fp++ = p;
			fn--;
			while ((c = *p++) != sepc && c != sepc2)
				if (c == '\0') {
					if (trimtrail && **(fp-1) == '\0')
						fn++;
					return(nfields - fn);
				}
			if (fn == 0)
				break;
			*(p-1) = '\0';
			while ((c = *p++) == sepc || c == sepc2)
				continue;
			p--;
		}
		/* we have overflowed the fields vector -- just count them */
		fn = nfields;
		while (c != '\0') {
			while ((c = *p++) == sepc || c == sepc2)
				continue;
			p--;
			fn++;
			while ((c = *p++) != '\0' && c != sepc && c != sepc2)
				continue;
		}
		/* might have to trim trailing white space */
		if (trimtrail) {
			p--;
			while ((c = *--p) == sepc || c == sepc2)
				continue;
			p++;
			if (*p != '\0') {
				if (fn == nfields+1)
					*p = '\0';
				fn--;
			}
		}
		return(fn);
	}

	/* n separators */
	fn = 0;
	for (;;) {
		if (fn < nfields)
			*fp++ = p;
		fn++;
		for (;;) {
			c = *p++;
			if (c == '\0')
				return(fn);
			sepp = sep;
			while ((sepc = *sepp++) != '\0' && sepc != c)
				continue;
			if (sepc != '\0')	/* it was a separator */
				break;
		}
		if (fn < nfields)
			*(p-1) = '\0';
		for (;;) {
			c = *p++;
			sepp = sep;
			while ((sepc = *sepp++) != '\0' && sepc != c)
				continue;
			if (sepc == '\0')	/* it wasn't a separator */
				break;
		}
		p--;
	}

	/* not reached */
}

/*
 * get a list of reference lines for the current group in the range
 *  'first' to 'last'
 *
 *   returns: void
 *
 * Note that XHDR is not part of the rfc977 standard, but is implemented
 * by the Berkeley NNTP server
 *
 */
void
getrefslist(newsgroup, artfirst, artlast, unreadonly)
struct newsgroup *newsgroup;
art_num artfirst;
art_num artlast;
int unreadonly;
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE];
    char *refstr;
    char *refs[DEFREFS];
    int nrefs;
    int i, j;
    art_num number;
    art_num first, last;

    if (SETNEWSGROUP(newsgroup)) {
	return;
    }

    first = artfirst;
    while (first <= artlast) {
	if (currentArticles[INDEX(first)].refs[0] != NIL(char) ||
	    (unreadonly && IS_READ(newsgroup->artStatus[INDEX(first)]))) {
	     first++;
	     continue;
	}
	for (last = first + 1; last <= artlast; last++) {
	    if (currentArticles[INDEX(last)].refs[0] != NIL(char) ||
		(unreadonly && IS_READ(newsgroup->artStatus[INDEX(last)]))) {
		break;
	    }
	}
	last--;

	(void) sprintf(command, "XHDR references %ld-%ld", first, last);
	if (put_server(command, True) == -1)
	    ServerDown = True;
	get_data_from_server(message, sizeof(message));

	check_time_out(command, message, sizeof(message), True);

	/* check for errors */
	if (*message != CHAR_OK) {
	    mesgPane(XRN_SERIOUS,
"XRN: serious error, your NNTP server does not have XHDR support.\n\
Either you are running a pre-1.5 NNTP server or XHDR has\n\
not been defined in 'nntp/common/conf.h'\n\
XRN requires XHDR support to run.");
	    return;
	}

	for(;;) {

	    get_data_from_server(message, sizeof(message));

	    if (ServerDown) {
		check_time_out(command, message, sizeof(message), False);
		continue;
	    }
	
	    if ((message[0] == '.') && (message[1] == '\0')) {
		break;
	    }

	    /*
	     * message is of the form:
	     *
	     *    Number refs
	     *
	     *    203 <12345@foo.bar.baz>
	     *
	     */

	    number = atol(message);
	    refstr = index(message, ' ');

	    if (number == 0 || refstr == NULL) {
		mesgPane(XRN_SERIOUS,
		    "Bogus message ID returned from server (%s), continuing..",
		     message);
		continue;
	    }

	    currentArticles[INDEX(number)].refs[0] = 
	    currentArticles[INDEX(number)].refs[1] = 
	    currentArticles[INDEX(number)].refs[2] = string_pool("");
	    nrefs = split(++refstr, refs, DEFREFS, " ,");
	    if (nrefs > DEFREFS) nrefs = DEFREFS;
	    for (i = 0, j = nrefs - 1; i <= 2 && j >= 0; j--, i++) {
		currentArticles[INDEX(number)].refs[i] = string_pool(refs[j]);
	    }
	}
	first = last + 1;
    }
    return;
}


/*
 * get the overview data for a list of articles.
 *
 *   returns: status
 *
 * This function requires a NNTP server that supports XOVER and an overview
 * database. 
 *
 */
Boolean
getoverview(newsgroup, artfirst, artlast, unreadonly)
struct newsgroup *newsgroup;
art_num artfirst;
art_num artlast;
int unreadonly;
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE];
    char *subjectline;
    char *fields[MAXFIELDS], *refs[DEFREFS];
    char authorline[MESSAGE_SIZE];
    char *author, *end, *brackbeg, *brackend;
    char *numoflines;
    int nf, nrefs, len;
    int i, j;
    int lcv;
    art_num number;
    art_num first, last;

    if (SETNEWSGROUP(newsgroup)) {
	return False;
    }

    first = artfirst;
    while (first <= artlast) {
	if (currentArticles[INDEX(first)].subject != NIL(char) ||
	    (unreadonly && IS_READ(newsgroup->artStatus[INDEX(first)]))) {
	     first++;
	     continue;
	}
	for (last = first + 1; last <= artlast; last++) {
	    if (currentArticles[INDEX(last)].subject != NIL(char) ||
		(unreadonly && IS_READ(newsgroup->artStatus[INDEX(last)]))) {
		break;
	    }
	}
	last--;

	for (i = first; i < last; i++) {
	    SET_UNAVAIL(newsgroup->artStatus[INDEX(i)]);
	}
	(void) sprintf(command, "XOVER %ld-%ld", first, last);
	if (put_server(command, True) == -1)
	    ServerDown = True;
	get_data_from_server(message, sizeof(message));

	check_time_out(command, message, sizeof(message), True);

	/* check for errors */
	if (*message != CHAR_OK) {
	    return False;
	}

	for(;;) {
	    get_data_from_server(message, sizeof(message));

	    if (ServerDown) {
		check_time_out(command, message, sizeof(message), False);
		continue;
	    }
	
	    if ((message[0] == '.') && (message[1] == '\0')) {
		break;
	    }

	    /*
	     * message is of the form:
	     *
	     *    Number Subj(tab)Author(tab)date(tab)refs(tab)bytes(tab)lines
	     *
	     * must get the number since not all subjects will be returned
	     */

	    len = strlen(message);
	    if (len > 0 && message[len-1] == '\n')
		message[len-1] = '\0';	/* make field count straightforward */
	    nf = split(message, fields, MAXFIELDS, "\t");
	    if (nf < MAXFIELDS - 1)	/* only "others" fields are optional */
		continue;		/* skip this line */
	    while (nf < MAXFIELDS)
		fields[nf++] = "";	/* fake missing fields */

	    number = atol(fields[0]);
	    subjectline = fields[1];

	    if (number == 0 || *subjectline == '\0') {
		mesgPane(XRN_SERIOUS,
		    "Bogus overview data returned from server (%s), continuing",
		     message);
		continue;
	    }

	    currentArticles[INDEX(number)].subject = string_pool(subjectline);
	    SET_AVAIL(newsgroup->artStatus[INDEX(number)]);

	    strcpy(authorline, fields[2]);
	    author = authorline;
	    if (app_resources.authorFullName) {
		/* Can be made fancyer at the expence of extra cpu time */
		if (author[0] == '\0') {
		    strcpy(authorline, "(None)");
		}

		/* First check for case 1, user@domain ("name") -> name */

		brackbeg = index(authorline, '(');
		brackend = index(authorline, '\0') - sizeof(char);
		/* brackend now points at the last ')' if this is case 1 */
		if (brackbeg != NIL(char) && (brackend > brackbeg) &&
		    (*brackend == ')')) {
		    author = brackbeg + sizeof(char);

		    /* Remove surrounding quotes ? */
		    if ((*author == '"') && (*(brackend - sizeof(char)) == '"')) {
			author++;
			brackend--;
		    }

		    /* Rather strip trailing spaces here */

		    *brackend = '\0';
		} else {
		    /* Check for case 2, "name" <user@domain> -> name */
		    brackbeg = index(authorline, '<');
		    if (brackbeg != NIL(char) && (index(brackbeg, '>') != NIL(char))
			&& (brackbeg > authorline)) {
			while (*--brackbeg == ' ')
			    ;

			/* Remove surrounding quotes ? */
			if ((*brackbeg == '"') && (*author ==  '"')) {
			    *brackbeg = '\0';
			    author++;

			    /* Rather strip trailing spaces here */

			} else {
			    *++brackbeg = '\0';
			}
		    } else {

			/* 
			 * Check for case 3, <user@domain> -> usr@domain
			 *
			 * Don't need to do this again:
			 * brackbeg = index(authorline, '<');
			 */

			brackend = index(authorline, '>');
			if ((author == brackbeg) && (brackend != NIL(char))) {
			    author++;
			    *brackend = '\0';
			} else {
			    if ((end = index(author, ' ')) != NIL(char)) {
				*end = '\0';
			    }
			}
		    }
		}
	    } else {
		if ((author = index(authorline, '<')) == NIL(char)) {
		    /* first form */
		    author = authorline;
		    if (*author == '\0') {
			strcpy(authorline, "(None)");
			author = authorline;
		    }
		    if ((end = index(author, ' ')) != NIL(char)) {
			*end = '\0';
		    }
		} else {
		    /* second form */
		    if ((end = index(author, '>')) != NIL(char)) {
			*end = '\0';
		    }
		}
	    }
	    /*
	     * do a final trimming - just in case the authors name ends
	     * in spaces or tabs - it does happen
	     */
	    end = author + strlen(author) - 1;
	    while ((end > author) && ((*end == ' ') || (*end == '\t'))) {
		*end = '\0';
		end--;
	    }
	    currentArticles[INDEX(number)].author = string_pool(author);

	    numoflines = fields[7];
	    if ((end = index(numoflines, '\t')) != NIL(char) ||
		(end = index(numoflines,  ' ')) != NIL(char)) {
		*end = '\0';
	    }
	    if (numoflines[0] != '(') {
		numoflines[utStrlen(numoflines)+1] = '\0';
		numoflines[utStrlen(numoflines)] = ']';
		for (lcv = utStrlen(numoflines); lcv >= 0; lcv--) {
		    numoflines[lcv+1] = numoflines[lcv];
		}
		numoflines[0] = '[';
	    } else {
		numoflines[0] = '[';
		numoflines[utStrlen(numoflines)-1] = ']';
	    }
	    if (strcmp(numoflines, "[none]") == 0) {
		(void) strcpy(numoflines, "[?]");
	    }
	    currentArticles[INDEX(number)].lines = string_pool(numoflines);
	    currentArticles[INDEX(number)].msgid = string_pool(fields[4]);
	    currentArticles[INDEX(number)].refs[0] = 
	    currentArticles[INDEX(number)].refs[1] = 
	    currentArticles[INDEX(number)].refs[2] = string_pool("");
	    nrefs = split(fields[5], refs, DEFREFS, " ,");
	    if (nrefs > DEFREFS) nrefs = DEFREFS;
	    for (i = 0, j = nrefs - 1; i <= 2 && j >= 0; j--, i++) {
		currentArticles[INDEX(number)].refs[i] = string_pool(refs[j]);
	    }
	}
	first = last + 1;
    }
    return;
}

#ifndef INEWS
static void
sendLine(str)
char *str;
{
    if (*str == '.') {
	char *str2 = XtMalloc(utStrlen(str) + 2); /* one for the extra period,
						   * and one for the null at
						   * the end */
	str2[0] = '.';
	strcpy(str2 + 1, str);
	if (put_server(str, False) == -1)
	    ServerDown = True;
	XtFree(str2);
    } else {
	if (put_server(str, False) == -1)
	    ServerDown = True;
    }
    return;
}
#endif

static char *
getLine(ptr)
char **ptr;
{
    static char line[512];
    char *end = index(*ptr, '\n');

    if (end) {
	(void) strncpy(line, *ptr, end - *ptr);
	line[end - *ptr] = '\0';
	*ptr = end + 1;
    } else {
	(void) strcpy(line, *ptr);
	*ptr = 0;
    }
    return line;
}


/*
 * Takes a block of text, wraps the text based on lineLength and
 * breakLength resources, and returns a NULL-terminated allocated
 * array of allocated strings representing the wrapped lines.  The
 * procedure which calls wrapText should use the wrapped Text and then
 * free each string and free the array.
 */
char **
wrapText(ptr, lineCount)
char *ptr;
int *lineCount;
{
    long c = 0;			/* current line length */
    char **lines, *this_line;
    long num_lines = 0;
    long breakAt = app_resources.breakLength;

    lines = (char **) XtMalloc((Cardinal) 0);

    if (app_resources.breakLength && app_resources.lineLength) {
	/*
	 * Do text wrapping.
	 */
	this_line = XtMalloc((Cardinal) (app_resources.breakLength + 1));

	while (*ptr != '\0') {
	    if (c >= breakAt) {
		/*
		 * Everything after the first line in a paragraph
		 * should be wrapped at lineLength, not breakLength.
		 * This prevents the last line of a paragraph from
		 * ending up a little bit longer than all the other
		 * lines (and extending into the margin), but not quite
		 * breakLength characters lines long.
		 */
		breakAt = app_resources.lineLength;
		/* backoff to app_resources.lineLength */
		ptr -= c - breakAt;
		c = app_resources.lineLength;
		for (; c > 0 && *ptr != ' ' && *ptr != '\t'; ptr--) {
		     c--;
		}

		if (c == 0) {
		    /* pathological, cut to app_resources.lineLength */
		    c = app_resources.lineLength;
		    ptr += app_resources.lineLength - 1;
		}

		/* output */
		this_line[c] = '\0';
		lines = (char **) XtRealloc((char *) lines, (Cardinal)
				     (sizeof(char *) * ++num_lines));
		lines[num_lines-1] = this_line;
		this_line = XtMalloc((Cardinal)
				      (app_resources.breakLength + 1));
		c = 0;
		if (strncmp(lines[num_lines-1],
			    app_resources.includePrefix,
			    utStrlen(app_resources.includePrefix)) == 0) {
		    strcpy(this_line, app_resources.includePrefix);
		    c += utStrlen(app_resources.includePrefix);
		}

		/*
		 * Delete any extra spaces, tabs or carriage returns at 
		 * the beginning of the next line.  This is necessary
		 * because we may break a line in the middle of a
		 * multi-space word break (e.g. the end of a sentence),
		 * or right before the paragraph-ending carriage
		 * return, which we've already printed as part of the
		 * line above.
		 */
		while ((*ptr == ' ') || (*ptr == '\t') || (*ptr == '\n')) {
		    ptr++;
		    if (*(ptr-1) == '\n')
			/* We only one to get rid of one carriage return */
			break;
		}
	       
		continue;
	    }

	    if (*ptr == '\n') {
		this_line[c] = '\0';
		lines = (char **) XtRealloc((char *) lines, (Cardinal)
					    (sizeof(char *) * ++num_lines));
		lines[num_lines-1] = this_line;
		this_line = XtMalloc((Cardinal)
				      (app_resources.breakLength + 1));
		if (c == 0)
		     breakAt = app_resources.breakLength;
		c = 0, ptr++;
		continue;
	    }

#ifdef notdef
	    if (*ptr == '\t') {
		c += c % 8;
		continue;
	    }
#endif
	  
	    this_line[c++] = *ptr++;
	}

	if (c != 0) {
	    this_line[c] = '\0';
	    lines = (char **) XtRealloc((char *) lines, (Cardinal)
					 (sizeof(char *) * ++num_lines));
	    lines[num_lines-1] = this_line;
	}
    } else {
	/*
	 * Don't do text wrapping, just break the text at linefeeds.
	 */
	while (*ptr) {
	    c = 0;
	    for (; *ptr && (*ptr != '\n'); ptr++, c++) ;
	    if (c || *ptr) {
		this_line = XtMalloc((Cardinal) (c + 1));
		lines = (char **) XtRealloc((char *) lines,
					     (Cardinal) (sizeof(char *) *
							 ++num_lines));
		strncpy(this_line, (char *) ptr-c, c);
		this_line[c] = '\0';
		lines[num_lines-1] = this_line;
		if (*ptr)
		    ptr++;
	    }
	}
    }
     
    lines = (char **) XtRealloc((char *) lines,
				 (Cardinal) (sizeof(char *) * ++num_lines));
    lines[num_lines-1] = NULL;

    *lineCount = num_lines;
    return(lines);
}

#ifdef MOTIF
char* wrapString(ptr)
char *ptr;
/*
 * Wrap a string and return the wrapped version.
 * only used on Motif to eliminate need for horizontal
 * scroll bar (enabling wrap in the widget is too slow)
 */
{
    unsigned int c = 0;		/* current line length */
    char *this_line;
    char *wrappedText, *w;
    int breakLength;
    int i, sizeNeeded, thisSize, sizeLeft;

    sizeNeeded = 0;

    breakLength = xthTextWidth(ArticleText) + 1;

    sizeNeeded = strlen(ptr) + 1000;
    wrappedText = w = XtMalloc(sizeNeeded);
    sizeLeft = sizeNeeded;
    *w = '\0';
    this_line = XtMalloc((Cardinal) (breakLength + 1));

    /*
     * Do text wrapping.
     */

    while (*ptr != '\0') {
	if (c >= breakLength) {
	    for (; c > 0 && *ptr != ' ' && *ptr != '\t'; ptr--) {
		c--;
	    }

	    if (c == 0) {
		/* pathological, cut to breakLength */
		c = breakLength;
		ptr += breakLength - 1;
	    }

	    /* output */
	    this_line[c] = '\0';
	    thisSize = strlen(this_line);
	    if (w != wrappedText) {
		*w++ = '\n';
		sizeLeft--;
	    }
	    if (sizeLeft < breakLength * 2) {
		sizeNeeded += 1000;
		sizeLeft += 1000;
		wrappedText = XtRealloc(wrappedText, sizeNeeded);
		w = wrappedText + strlen(wrappedText);
	    }
	    strcpy(w, this_line);
	    w += thisSize;
	    sizeLeft -= thisSize;
	    this_line[0] = '\0';
	    c = 0;

	    /*
	     * Delete any extra spaces, tabs or carriage returns at 
	     * the beginning of the next line.  This is necessary
	     * because we may break a line in the middle of a
	     * multi-space word break (e.g. the end of a sentence),
	     * or right before the paragraph-ending carriage
	     * return, which we've already printed as part of the
	     * line above.
	     */
	    while ((*ptr == ' ') || (*ptr == '\t') || (*ptr == '\n')) {
		ptr++;
		if (*(ptr-1) == '\n')
		    /* We only one to get rid of one carriage return */
		    break;
	    }
	       
	    continue;
	}

	if (*ptr == '\n') {
	    this_line[c] = '\0';
	    thisSize = strlen(this_line);
	    if (w != wrappedText) {
		*w++ = '\n';
		sizeLeft--;
	    }
	    if (sizeLeft < breakLength * 2) {
		sizeNeeded += 1000;
		sizeLeft += 1000;
		wrappedText = XtRealloc(wrappedText, sizeNeeded);
		w = wrappedText + strlen(wrappedText);
	    }
	    strcpy(w, this_line);
	    w += thisSize;
	    sizeLeft -= thisSize;
	    this_line[0] = '\0';

	    c = 0, ptr++;
	    continue;
	}

	this_line[c++] = *ptr++;
    }

    if (c != 0) {
	this_line[c] = '\0';
	thisSize = strlen(this_line);
	if (w != wrappedText) {
	    *w++ = '\n';
	    sizeLeft--;
	}
	if (sizeLeft < breakLength * 2) {
	    sizeNeeded += 1000;
	    sizeLeft += 1000;
	    wrappedText = XtRealloc(wrappedText, sizeNeeded);
	    w = wrappedText + strlen(wrappedText);
	}
	strcpy(w, this_line);
	w += thisSize;
	sizeLeft -= thisSize;
	this_line[0] = '\0';
    }
    *w++ = '\n';
    *w++ = '\0';
    FREE(this_line);
    return(wrappedText);
}
#endif /* MOTIF */
#ifndef VMS

int
mailArticle(article)
char *article;
{
#ifdef sequent
     extern FILE *popen _ARGUMENTS((const char *, const char *)); /* sequent */
#endif

     FILE *fp;
     char **lines_ptr, **lines;
     char *ptr;
     int lineCount;
     
     if ((fp = (FILE *) popen(app_resources.mailer, "w")) == NULL)
	  return POST_FAILED;

     /* First, send everything up to the first blank line without any */
     /* wrapping. 						      */
     while (1) {
	  ptr = index(article, '\n');
	  if ((ptr == article) || (ptr == NULL))
	       /* line has nothing but newline or end of article */
	       break;
	  (void) fwrite(article, sizeof(char), (unsigned) (ptr - article + 1), fp);
	  article = ptr + 1;
     }
     
     lines_ptr = lines = wrapText(article, &lineCount);
     while (*lines) {
	  (void) fwrite(*lines, sizeof(char), utStrlen(*lines), fp);
	  (void) fwrite("\n", sizeof(char), 1, fp); /* wrapText deletes newlines */
	  FREE(*lines);
	  lines++;
     }
     FREE(lines_ptr);

     return pclose(fp) ? POST_FAILED : POST_OKAY;
}
#endif


int
postArticle(article, mode)
char *article;
int mode;   /* XRN_NEWS or XRN_MAIL */
/*
 * post an article
 *
 *   returns 1 for success, 0 for failure
 */
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE];
    char *ptr, *saveptr;
    char **lines, **lines_ptr;
    int lineCount;

#ifdef INEWS
    char *tempfile;
    int exitstatus;
    char buffer[1024];
    FILE *inews;
    extern char * utTempnam();
#endif
    
    if (mode == XRN_MAIL) {
	return mailArticle(article);
    }

#ifdef INEWS
    tempfile = XtNewString(utTempnam(app_resources.tmpDir, "xrn"));
    (void) sprintf(buffer, "%s -h > %s 2>&1",INEWS, tempfile);
    if ((inews = (FILE *) popen(buffer, "w")) == NULL) {
        mesgPane(XRN_SERIOUS, "Failed to start inews\n");
	(void) unlink(tempfile);
	FREE(tempfile);
        return(POST_FAILED);
    }
#else

    (void) strcpy(command, "POST");
    if (put_server(command, True) == -1)
	ServerDown = True;
    get_data_from_server(message, sizeof(message));

    check_time_out(command, message, sizeof(message), True);

    if (*message != CHAR_CONT) {
	mesgPane(XRN_SERIOUS,
	    "Your message could not be posted due to the following error:\r%s",
		message);
	mesgPane(XRN_SERIOUS,
	    "Please contact your news system adminstrator for assistance.");
	if (atoi(message) == ERR_NOPOST) {
	    return(POST_NOTALLOWED);
	} else {
	    return(POST_FAILED);
	}
    }
#endif

    ptr = article;

    while (1) {
	char *line;

	saveptr = ptr;

	line = getLine(&ptr);
	if (index(line, ':') || (*line == ' ') || (*line == '\t')) {
#ifdef INEWS
	    fputs(line, inews);
	    fputc('\n', inews);
#else
	    sendLine(line);
#endif
	    continue;
	}
	break;
    }

    if (*saveptr != '\n') {
	 /* if the skipped line was not blank, point back to it */
	 ptr = saveptr;
    }

    lines_ptr = lines = wrapText(ptr, &lineCount);
#ifdef INEWS
    sprintf(command,"Lines: %d\n",lineCount);
    fputs(command, inews);
    fputs("\n\n", inews);
#else
    sprintf(command,"Lines: %d\n",lineCount);
    sendLine(command);
    sendLine("");		/* send a blank line */
#endif

    while (*lines) {
#ifdef INEWS
         fputs(*lines, inews);
	 fputc('\n', inews);
#else
	 sendLine(*lines);
#endif
	 XtFree(*lines);
	 lines++;
    }
    FREE(lines_ptr);
    
#ifdef INEWS
    if (exitstatus = pclose(inews)) {
	FILE *filefp;
	char *p;
	struct stat buf;
	char temp[1024];
	(void) sprintf(temp, "\n\ninews exit value: %d\n", exitstatus);
	if ((filefp = fopen(tempfile, "r")) != NULL) {
	    if (fstat(fileno(filefp), &buf) != -1) {
		p = XtMalloc(buf.st_size + utStrlen(temp) + 10);
		(void) fread(p, sizeof(char), buf.st_size, filefp);
		p[buf.st_size] = '\0';
		(void) strcat(p, temp);
		(void) fclose(filefp);
		mesgPane(XRN_SERIOUS, "INEWS Serious Error: %s", p);
	    }
	}
	(void) unlink(tempfile);
	FREE(tempfile);
	return(POST_FAILED);
    }
#else
    if (put_server(".", True) == -1)
	ServerDown = True;

#ifdef SERIALHACK
    get_data_from_server(message, sizeof(message));
#endif

    get_data_from_server(message, sizeof(message));
    if (ServerDown) {
	mesgPane(XRN_SERIOUS, "NNTP Server connection lost");
	check_time_out(".", message, sizeof(message), False);
	return(POST_FAILED);
    }	
    if (*message != CHAR_OK) {
	mesgPane(XRN_SERIOUS, "NNTP Serious Error: %s", message);
	return(POST_FAILED);
    }
#endif

    return(POST_OKAY);
}


#ifdef DONT_USE_XHDR_FOR_A_SINGLE_ITEM

void
xhdr(article, field, string)
art_num article;
char *field;
char **string;
/*
 * get header information about 'article'
 *
 *   the results are stored in 'string'
 */
{
    char command[BUFFER_SIZE], message[MESSAGE_SIZE];
    char buffer[BUFFER_SIZE];
    char *ptr, *cmp;
    Boolean found = 0;
    Boolean current = 0;
 
    *string = 0;
    if (currentNewsgroup == NULL) {
	return;
    }

    buffer[0] = '\0';
    /*
     * In some implementations of NNTP, the XHDR request on a
     * single article can be *very* slow, so we do a HEAD request
     * instead and just search for the appropriate field.
     */

    (void) sprintf(command, "HEAD %ld", article);
    if (put_server(command, True) == -1)
	ServerDown = True;
    get_data_from_server(message, sizeof(message));
    
    check_time_out(buffer, message, sizeof(message), True);
    
    /* check for errors */
    if (*message != CHAR_OK) {
	/* can't get header */
	*string = NIL(char);
	return;
    }
    
    for (;;) {
	get_data_from_server(message, sizeof(message));
	if (ServerDown || STREQN(message, "503 Timeout", 11)) {
	    check_time_out(buffer, message, sizeof(message), False);
	    continue;
	}

	/* the header information is ended by a '.' on a line by itself */

	if ((message[0] == '.') && (message[1] == '\0')) {
	    break;
	}

	if (!found) {
	    for (ptr = message, cmp = field; *ptr; ptr++, cmp++) {
		/* used to be 'mklower' */
		if (tolower(*cmp) != tolower(*ptr))
		    break;
	    }
	    if (*cmp == 0 && *ptr == ':') {
		while (*++ptr == ' ')
		    ;
		strcat(buffer, ptr);
		found = True;
		current = True;
	    }
	}
	if (current) {
	    if (message[0] != '\t') {
		current = False;
	    } else {
		ptr = message;
		while (*++ptr == '\t')
		    ;
		strcat(buffer, " ");
		strcat(buffer, ptr);
	    }
	}
		
    }

    if (found)
	*string = XtNewString(buffer);
    else
	*string = NIL(char);

    return;
}

#else

void
xhdr(article, field, string)
art_num article;
char *field;
char **string;
/*
 * get header information about 'article'
 *
 *   the results are stored in 'string'
 */
{
    char buffer[BUFFER_SIZE], message[MESSAGE_SIZE], *ptr;
    
    (void) sprintf(buffer, "XHDR %s %ld", field, article);
    if (put_server(buffer, True) == -1)
	ServerDown = True;
    get_data_from_server(message, sizeof(message));
    
    check_time_out(buffer, message, sizeof(message), True);
    
    /* check for errors */
    if (*message != CHAR_OK) {
	fprintf(stderr, "NNTP error: %s\n", message);
	*string = NIL(char);
	mesgPane(XRN_SERIOUS,
"XRN: serious error, your NNTP server does not have XHDR support.\n\
Either you are running a pre-1.5 NNTP server or XHDR has\n\
not been defined in 'nntp/common/conf.h'\n\
XRN requires XHDR support to run.");
	return;
    }
    
    get_data_from_server(message, sizeof(message));
    check_time_out(buffer, message, sizeof(message), False);

    /* no information */
    if (*message == '.') {
	*string = NIL(char);
	return;
    }

    ptr = index(message, ' ');

    /* malformed entry */
    if (ptr == NIL(char)) {
	mesgPane(XRN_SERIOUS,
"XRN debugging message: malformed XHDR return\n\
command: %s, return: %s",
		       buffer, message);
	get_data_from_server(message, sizeof(message));
	return;
    }

    ptr++;

    /* no information */
    if (STREQ(ptr, "(none)")) {
	*string = NIL(char);
	/* ending '.' */
	do {
	    get_data_from_server(message, sizeof(message));
	    if (ServerDown) {
		check_time_out(buffer, message, sizeof(message), False);
		continue;
	    }
	} while (*message != '.');
	return;
    }

    *string = (char *) XtNewString(ptr);

    /* ending '.' */
    do {
	get_data_from_server(message, sizeof(message));
	if (ServerDown) {
	    check_time_out(buffer, message, sizeof(message), False);
	    continue;
	}
    } while (*message != '.');

    return;
}
#endif

struct article *
getarticles(newsgroup)
struct newsgroup *newsgroup;
{
    register art_num first = newsgroup->first, last = newsgroup->last, art;
    int newsize;

    if (last >= first && last != 0) {
	register struct article	*ap;

	newsize = last - first + 1;
	if (newsize > artCount) {
	    if (savedArt) FREE(savedArt);
	    newsize = artCount = MAX(newsize, 1000);
	    currentArticles = savedArt = ARRAYALLOC(struct article, newsize+10);
	} else {
	    currentArticles = savedArt;
	}

	ap = &currentArticles[INDEX(first)];
    
	for (art = first; art <= last; art++) {
	    ap->subject = NIL(char);
	    ap->author = NIL(char);
	    ap->lines = NIL(char);
	    ap->msgid = NIL(char);
	    ap->refs[0] = NIL(char);
	    ap->refs[1] = NIL(char);
	    ap->refs[2] = NIL(char);
	    ap->parent = 0;
	    ap->child = 0;
	    ap->text = NIL(char);
	    ap++;
	}
    }
    return(currentArticles);
}

struct artStat *
getartstatus(newsgroup)
struct newsgroup *newsgroup;
{
    register art_num first = newsgroup->first, last = newsgroup->last, art;

    if (last >= first && last != 0) {
	register struct artStat *ap;
	newsgroup->artStatus = ARRAYALLOC(struct artStat, last - first + 1);

	ap = &newsgroup->artStatus[INDEX(first)];
    
	for (art = first; art <= last; art++) {
	    ap->status = ART_CLEAR;
	    ap++;
	}
    }
    return(newsgroup->artStatus);
}

#ifdef VFORK_SUPPORTED
#ifndef POPEN_USES_INEXPENSIVE_FORK

static int popen_pid = 0;

#if defined(STDC) && !defined(_NO_PROTO)
FILE  *	popen(const char *__command, const char *__type )
#else
FILE *
popen(__command, __type)
char *__command;
char *__type;
#endif
{
    int pipes[2];
    int itype = (strcmp(__type, "w") == 0 ? 1 : 0);

    if (pipe(pipes) == -1)
	return NULL;

    switch (popen_pid = vfork()) {
    case -1:
	(void)close(pipes[0]);
	(void)close(pipes[1]);
	return NULL;

    case 0:
	if (itype) {
	    dup2(pipes[0], fileno(stdin));
	    close(pipes[1]);
	} else {
	    dup2(pipes[1], fileno(stdout));
	    close(pipes[0]);
	}
	execl("/bin/sh", "/bin/sh", "-c", __command, 0);
	fprintf(stderr, "XRN Error: failed the execlp");
	_exit(-1);
	/* NOTREACHED */

    default:
	    if (itype) {
		close(pipes[0]);
		return fdopen(pipes[1], "w");
	    } else {
		close(pipes[1]);
		return fdopen(pipes[0], "w");
	    }
    }
}

#if defined(STDC) && !defined(_NO_PROTO)
int
pclose( FILE *__stream )
#else
int
pclose(__stream)
FILE *__stream;
#endif
{
    int pd = 0;
#if defined (ultrix) && defined (STDC)
    union wait status;
#else
    int	status;
#endif
    int	err;

    err = fclose(__stream);

    do {
	if ((pd = wait(&status)) == -1)	{
	    err = EOF;
	    break;
	}
    } while (pd != popen_pid);

    if (err == EOF)
	return  -1;
	
#if defined(ultrix) && defined(STDC)
    return WEXITSTATUS(status);
#else
    if (status)
	status >>= 8;	/* exit status in high byte */

    return status;
#endif
}
#endif
#endif
