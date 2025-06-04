#ifndef SERVER_H
#define SERVER_H

/*
 * $Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/server.h,v 1.7 1993/02/04 18:22:30 ricks Exp $
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
 * server.h: routines for communicating with the NNTP remote news server
 *
 */

#include "codes.h"

/*
 * function definitions for the nntp server (in nntp/clientlib.c)
 */
  
extern void close_server();
extern int get_server _ARGUMENTS((char *, int));
extern char *getserverbyfile _ARGUMENTS((char *));
extern int put_server _ARGUMENTS((char *, Boolean));
extern int server_init _ARGUMENTS((char *));
extern void start_server();

/* get the list of active news groups from the news server */
extern void getactive();

/* see if the subscribed to groups have 0 or 1 article */
extern void fixLowArticleNumbers();

/* get a single article in the current group from the news server */
#ifdef XLATE
extern char *getarticle _ARGUMENTS((art_num, int *, int, int, int));
#else
extern char *getarticle _ARGUMENTS((art_num, int *, int, int));
#endif
#define FULL_HEADER   1
#define NORMAL_HEADER 2
#define NOT_ROTATED   1
#define ROTATED       2
#define NOT_XLATED    1
#define XLATED        2

/*
 * tell the server that the next set of article requests will be for this group
 *  returns NO_GROUP on failure
 */
extern int getgroup _ARGUMENTS((struct newsgroup *, art_num *, art_num *, int *number));

/* get a list of subject lines for a range of articles in the current group from the server */
extern void getsubjectlist _ARGUMENTS((struct newsgroup *,art_num,art_num,int));
extern void getauthorlist _ARGUMENTS((struct newsgroup *,art_num,art_num,int));
extern void getlineslist _ARGUMENTS((struct newsgroup *,art_num,art_num,int));
extern void getmsgidlist _ARGUMENTS((struct newsgroup *,art_num,art_num,int));
extern void getrefslist _ARGUMENTS((struct newsgroup *,art_num,art_num,int));
extern Boolean getoverview _ARGUMENTS((struct newsgroup *,art_num,art_num,int));

/* xhdr commands */
extern void xhdr _ARGUMENTS((art_num, char *, char **));

/* post article */
extern int postArticle _ARGUMENTS((char *, int));

/* wrap article text buffer */
extern char** wrapText _ARGUMENTS((char *, int *));
extern char*  wrapString _ARGUMENTS((char *));

/* in clientlib.c */
extern int handle_server_response _ARGUMENTS((int, char *));

#define XRN_MAIL	 0
#define XRN_NEWS	 1

#define POST_FAILED      0
#define POST_OKAY        1
#define POST_NOTALLOWED  2
#define POST_TRYAGAIN	 3

#endif /* SERVER_H */
