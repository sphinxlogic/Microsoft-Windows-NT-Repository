
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/cursor.c,v 1.5 1993/01/11 02:14:44 ricks Exp $";
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
 * cursor.c: routines for manipulating the cursor and/or text in a text
 *           window
 */

#include "copyright.h"
#include "config.h"
#include <stdio.h>
#ifdef __STDC__
#include <stdlib.h>
#endif
#include "utils.h"
#include "mesg.h"
#include "news.h"
#include "internals.h"
#include "modes.h"
#include "xrn.h"
#include "cursor.h"
#include "error_hnds.h"

/*
 * Return the name of the group on the current line.
 * Assume there is a group there.
 */
void
currentNewsgroup(mode, tstring, groupName)
int mode;			/* xrn Mode */
char *tstring;			/* text string */
char *groupName;		/* string to return group name in */
{
    if ((mode != ALL_MODE) && (mode != ADD_MODE)) {
	if (sscanf(&tstring[0], NEWS_GROUP_LINE, groupName) == 1) {
	    return;
	} else {
	    *groupName = '\0';
	    return;
	}
    } else {
	(void) sscanf(&tstring[0], "%s", groupName);
    }
    return;
}

/*
 * Return the status of the group on the current line.
 */
void
currentMode(tstring, groupName, mode)
char *tstring;
char *groupName;
int *mode;
{
    char status[100];

    (void) sscanf(&tstring[0], "%s %s", groupName, status);
    if (strcmp(status,"unsubscribed") == 0) {
	*mode = UNSUBSCRIBE;
    } else {
	*mode = SUBSCRIBE;
    }
    return;
}

/*
 * Mark the article at the current ArticlePosition as read,if it is
 * not already marked. Return the article number of the article
 * that was marked.  This subroutine only marks the text string;
 * the article is marked internally as read by calling 
 * markArticleAsRead().
 */
int
markStringRead(tstring)
char *tstring;			/* text string */
{
    if (tstring[0] == ' ') {
	tstring[0] = '+';
    }
    return atoi(&tstring[2]);
}

/*
 * Mark an article as read or unread.
 */
void
markArticle(artNum, marker)
art_num artNum;
int marker;			/* either '+' or ' ' */
{
    if (marker == '+' || marker == 'S') {
	markArticleAsRead(artNum);
    } else {
	if (marker != 'P') {
	    markArticleAsUnread(artNum);
	}
    }
    return;
}

int
subjectSearch(dir, artnums, position, expr, text, ques, artNum)
int dir;			/* direction, either FORWARD or BACK */
art_num *artnums;		/* text string (may be changed here) */
int  *position;			/* cursor position */
char *expr;			/* regular expression to search for */
char **text, **ques;		/* text and status line for new article */
art_num *artNum;		/* number of new article */
{
#ifdef SYSV_REGEX
    extern char *regcmp();
    extern char *regex();
#else
    char *re_comp();		/* regular expression routine */
#endif
    static char *reRet;		/* returned by re_comp/regcmp */
    char *newsubject;		/* subject of current line */
    char lowersubject[1024];	/* lowercase subject of current line */
    char *newLine;
    extern void abortClear();
    extern int abortP();
    static Boolean caseSensitive = False;
    int num;			/* Current article number being searched */
    char statBuf[80];		/* message buffer */

    abortClear();

    if (expr != NIL(char)) {
	utLowerCase(lowersubject, expr, sizeof(lowersubject) - 1);
	if (strcmp(expr, lowersubject) != 0) {
	    caseSensitive = True;
	} else {
	    caseSensitive = False;
	}

#ifdef SYSV_REGEX
	if ((reRet = regcmp(expr, NULL)) == NULL) {
#else
	if ((reRet = re_comp(expr)) != NULL) {
#endif
	    /* bad regular expression */
#ifdef SYSV_REGEX
	    mesgPane(XRN_SERIOUS, "error in regular expression");
#else
	    mesgPane(XRN_SERIOUS, reRet);
#endif
	    failedSearch();
	    return ERROR;
	}
    }

    if (dir == FORWARD) {
	for (;;) {
	    if (abortP()) {
		return ABORT;
	    }
	    if (artnums[*position] == 0) {
		if (*position == 0) {

		    /* the string is null, no more articles are left */

		    return EXIT;
		}
		return NOMATCH;
	    }
	    *position = *position + 1;
	    if (artnums[*position] == 0) {

		/* reached end of string */

		return NOMATCH;
	    }
	    *artNum = artnums[*position];
	    newsubject = getSubject(*artNum);
	    if (caseSensitive) {
		strncpy(lowersubject, newsubject, sizeof(lowersubject) - 1);
	    } else {
		utLowerCase(lowersubject, newsubject, sizeof(lowersubject) - 1);
	    }

#ifdef SYSV_REGEX
	    if (regex(reRet, lowersubject) != NULL) {
#else
	    if (re_exec(lowersubject)) {
#endif
		/* found a match to the regular expression */

		gotoArticle(*artNum);
		if (getArticleText(text, ques) != XRN_OKAY) {

		    /* the matching article was invalid */

		    continue;
		}
		return MATCH;
	    }
	}
    } else {
	startSearch();
	for (;;) {
	    if (abortP()) {

		/* reset pointers back to where we began, since the */
		/* search was aborted */

		failedSearch();
		return ABORT;
	    }
	    if ((*position == 0) &&
		(artnums[*position] == 0) ) {

		/* no more articles remain, return to Newgroup mode */

		return EXIT;
	    }
	    if (*position != 0) {

		/* we are still within the subject list */

		*position = *position - 1;
		*artNum = artnums[*position];
		newsubject = getSubject(*artNum);
		if (caseSensitive) {
		    utLowerCase(lowersubject, newsubject, sizeof(lowersubject) - 1);
		} else {
		    strncpy(lowersubject, newsubject, sizeof(lowersubject) - 1);
		}

#ifdef SYSV_REGEX
		if (regex(reRet, lowersubject) != NULL) {
#else
		if (re_exec(lowersubject)) {
#endif
		    /* an article matching the regular expression was found */

		    gotoArticle(*artNum);
		    if (getArticleText(text, ques) != XRN_OKAY) {

			/* article is invalid, remove it from the text string*/

			continue;
		    }
		    return MATCH;
		}
	    } else {

		/* must query the news server for articles not shown */
		/* on the current subject screen */

		if ((newLine = getPrevSubject()) == NIL(char)) {
		    
		    /* all articles have been exhausted, reset variables */
		    /* to what they were before the search was started */

		    failedSearch();
		    return NOMATCH;
		}
		newLine[0] = '+';
		num = atol(&newLine[2]);
		*artNum = num;
		if ((num % 100) == 0) {
		    sprintf(statBuf, "Now searching at article %d", num);
		    infoNow(statBuf);
		}
		newsubject = getSubject(*artNum);
		if (caseSensitive) {
		    utLowerCase(lowersubject, newsubject, sizeof(lowersubject) - 1);
		} else {
		    strncpy(lowersubject, newsubject, sizeof(lowersubject) - 1);
		}
#ifdef SYSV_REGEX
		if (regex(reRet, lowersubject) != NULL) {
#else
		if (re_exec(lowersubject)) {
#endif
		    /* the new article (subjectline) obtained here */
		    /* does not need to be checked for validity, since */
		    /* getPrevSubject() only returns valid articles */

		    gotoArticle(*artNum);
		    (void) getArticleText(text, ques);
		    return WINDOWCHANGE;
		}
		continue;
	    }
	}
    }
}

int
authorSearch(dir, artnums, position, expr, text, ques, artNum)
int dir;			/* direction, either FORWARD or BACK */
art_num *artnums;		/* text string (may be changed here) */
int  *position;			/* cursor position */
char *expr;			/* regular expression to search for */
char **text, **ques;		/* text and status line for new article */
art_num *artNum;		/* number of new article */
{
#ifdef SYSV_REGEX
    extern char *regcmp();
    extern char *regex();
#else
    char *re_comp();		/* regular expression routine */
#endif
    static char *reRet;		/* returned by re_comp/regcmp */
    char *newauthor;		/* author of current line */
    char lowerauthor[128];	/* lowercase author */
    char *newLine;
    extern void abortClear();
    extern int abortP();
    int num;			/* Current article number being searched */
    char statBuf[80];		/* message buffer */

    abortClear();

    if (expr != NIL(char)) {
#ifdef SYSV_REGEX
	if ((reRet = regcmp(expr, NULL)) == NULL) {
#else
	if ((reRet = re_comp(expr)) != NULL) {
#endif
	    /* bad regular expression */
#ifdef SYSV_REGEX
	    mesgPane(XRN_SERIOUS, "error in regular expression");
#else
	    mesgPane(XRN_SERIOUS, reRet);
#endif
	    failedSearch();
	    return ERROR;
	}
    }

    if (dir == FORWARD) {
	for (;;) {
	    if (abortP()) {
		return ABORT;
	    }
	    if (artnums[*position] == 0) {
		if (*position == 0) {

		    /* the string is null, no more articles are left */

		    return EXIT;
		}
		return NOMATCH;
	    }
	    *position = *position + 1;
	    if (artnums[*position] == 0) {

		/* reached end of string */

		return NOMATCH;
	    }
	    *artNum = artnums[*position];
	    newauthor = getAuthor(*artNum);
	    utLowerCase(lowerauthor, newauthor, sizeof(lowerauthor) - 1);

#ifdef SYSV_REGEX
	    if (regex(reRet, lowerauthor) != NULL) {
#else
	    if (re_exec(lowerauthor)) {
#endif
		/* found a match to the regular expression */

		gotoArticle(*artNum);
		if (getArticleText(text, ques) != XRN_OKAY) {

		    /* the matching article was invalid */

		    continue;
		}
		return MATCH;
	    }
	}
    } else {
	startSearch();
	for (;;) {
	    if (abortP()) {

		/* reset pointers back to where we began, since the */
		/* search was aborted */

		failedSearch();
		return ABORT;
	    }
	    if ((*position == 0) &&
		(artnums[*position] == 0) ) {

		/* no more articles remain, return to Newgroup mode */

		return EXIT;
	    }
	    if (*position != 0) {

		/* we are still within the article list */

		*position = *position - 1;
		num = artnums[*position];
		*artNum = num;
		if ((num % 100) == 0) {
		    sprintf(statBuf, "Now searching at article %d", num);
		    infoNow(statBuf);
		}
		newauthor = getAuthor(*artNum);
		utLowerCase(lowerauthor, newauthor, sizeof(lowerauthor) - 1);

#ifdef SYSV_REGEX
		if (regex(reRet, lowerauthor) != NULL) {
#else
		if (re_exec(lowerauthor)) {
#endif
		    /* an article matching the regular expression was found */

		    gotoArticle(*artNum);
		    if (getArticleText(text, ques) != XRN_OKAY) {

			/* article is invalid, remove it from the text string*/

			continue;
		    }
		    return MATCH;
		}
	    } else {

		failedSearch();
		return NOMATCH;
	    }
	}
    }
}


int
moveToArticle(artNum, file, ques)
art_num artNum;			/* number of new article */
char **file, **ques;		/* text and status line for new article */
{
    fillUpArray(artNum);

    if (checkArticle(artNum) != XRN_OKAY) {
	return NOMATCH;
    }

    gotoArticle(artNum);
    if (getArticleText(file, ques) != XRN_OKAY) {
	return ERROR;
    }

    return MATCH;
}
