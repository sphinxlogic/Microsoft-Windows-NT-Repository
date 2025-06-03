/* $Id: addng.c,v 1.2 92/01/11 16:27:18 usenet Exp $
 *
 * $Log:	addng.c,v $
 * Revision 1.2  92/01/11  16:27:18  usenet
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

#include "EXTERN.h"
#include "common.h"
#include "rn.h"
#include "ngdata.h"
#include "last.h"
#include "util.h"
#include "intrp.h"
#include "only.h"
#include "rcstuff.h"
#ifdef SERVER
#include "server.h"
#endif
#include "final.h"
#include "INTERN.h"
#include "addng.h"

#ifdef TZSET
#include <time.h>
#else
#include <sys/time.h>
#include <sys/timeb.h>
#endif

void
addng_init()
{
    ;
}

#ifdef FINDNEWNG
/* generate a list of new newsgroups from active file */

bool
newlist(munged,checkinlist)
bool_int munged;			/* are we scanning the whole file? */
bool_int checkinlist;
{
    char *tmpname;
    register char *s, *status;
    register NG_NUM ngnum;
#ifndef ACTIVE_TIMES
    long birthof();
#endif

    tmpname = filexp(RNEWNAME);
    tmpfp = fopen(tmpname,"w+");
    if (tmpfp == Nullfp) {
	printf(cantcreate,tmpname) FLUSH;
	return FALSE;
    }
    UNLINK(tmpname);			/* be nice to the world */

    while (fgets(buf,LBUFLEN,actfp) != Nullch) {
	/* Check if they want to break out of the new newsgroups search */
	if (int_count) {
	    int_count = 0;
	    fclose(tmpfp);
	    return FALSE;
	}
	if (s = index(buf,' ')) {
	    status=s;
	    while (isdigit(*status) || isspace(*status)) status++;
	    *s++ = '\0';
	    if (strnEQ(buf,"to.",3) || *status == 'x' || *status == '=')
	        /* since = groups are refiling to another group, just
		   ignore their existence */
		continue;
#ifdef ACTIVE_TIMES
	    if (inlist(buf) && ((ngnum = find_ng(buf)) == nextrcline
				|| toread[ngnum] == TR_UNSUB)
#else
	    if (checkinlist ?
		(inlist(buf) && ((ngnum = find_ng(buf)) == nextrcline
				 || toread[ngnum] == TR_UNSUB))
	      : (find_ng(buf) == nextrcline
		 && birthof(buf,(ART_NUM)atol(s)) > lasttime)
#endif
	    ) {
					/* if not in .newsrc and younger */
					/* than the last time we checked */
		fprintf(tmpfp,"%s\n",buf);
					/* then remember said newsgroup */
	    }
#ifdef FASTNEW
	    else {			/* not really a new group */
		if (!munged) {		/* did we assume not munged? */
		    fclose(tmpfp);	/* then go back, knowing that */
		    return TRUE;	/* active file was indeed munged */
		}
	    }
#endif
	}
#ifdef DEBUGGING
	else
	    printf("Bad active record: %s\n",buf) FLUSH;
#endif
    }

    /* we have successfully generated the list */

    fseek(tmpfp,0L,0);			/* rewind back to the beginning */
    while (fgets(buf,LBUFLEN,tmpfp) != Nullch) {
	buf[strlen(buf)-1] = '\0';
	get_ng(buf,TRUE);		/* add newsgroup, maybe */
    }
    fclose(tmpfp);			/* be nice to ourselves */
    return FALSE;			/* do not call us again */
}

#ifdef ACTIVE_TIMES
#ifdef SERVER

bool
find_new_groups()
{
    char *tmpname;
    register char *s;
    struct tm *ts;
    long now;
    NG_NUM oldnext = nextrcline;	/* remember # lines in newsrc */

    tmpname = filexp(RNEWNAME);
    tmpfp = fopen(tmpname,"w+");
    if (tmpfp == Nullfp) {
	printf(cantcreate,tmpname) FLUSH;
	return FALSE;
    }
    UNLINK(tmpname);			/* be nice to the world */

    time(&now);
    ts = gmtime(&lastnewtime);
    sprintf(ser_line, "NEWGROUPS %02d%02d%02d %02d%02d%02d GMT",
	ts->tm_year % 100, ts->tm_mon+1, ts->tm_mday,
	ts->tm_hour, ts->tm_min, ts->tm_sec);
#ifdef DEBUGGING
    if (debug & DEB_NNTP)
	printf(">%s\n", ser_line) FLUSH;
#endif
    put_server(ser_line);
    nntp_get(ser_line, sizeof(ser_line));
#ifdef DEBUGGING
    if (debug & DEB_NNTP)
	printf("<%s\n", ser_line) FLUSH;
#endif
    if (*ser_line != CHAR_OK) {		/* and then see if that's ok */
  error_exit:
	fclose(tmpfp);
	printf("Can't get new groups from server:\n%s\n", ser_line);
	return FALSE;
    }

    while (1) {
	if (nntp_get(ser_line, sizeof(ser_line)) < 0)
	    goto error_exit;
#ifdef DEBUGGING
	if (debug & DEB_NNTP)
	    printf("<%s\n", ser_line) FLUSH;
#endif
	if (ser_line[0] == '.')
	    break;
	if ((s = index(ser_line, ' ')) != Nullch)
	    *s = '\0';
	fprintf(tmpfp,"%s\n",ser_line);
    }

    /* we have successfully generated the list */

    if (ftell(tmpfp)) {
	fputs("\nFinding new newsgroups:\n",stdout) FLUSH;

	fseek(tmpfp,0L,0);		/* rewind back to the beginning */
	while (fgets(buf,LBUFLEN,tmpfp) != Nullch) {
	    buf[strlen(buf)-1] = '\0';
	    get_ng(buf,FALSE);		/* add newsgroup, maybe */
	}
	lastnewtime = now;		/* remember when we found new groups */
    }					/* (ends up back in .rnlast) */
    fclose(tmpfp);			/* be nice to ourselves */

    return oldnext != nextrcline;
}
#else /* not SERVER */

bool
find_new_groups()
{
    register char *s;
    long lastone;
    NG_NUM oldnext = nextrcline;	/* remember # lines in newsrc */

    fstat(fileno(actfp),&filestat);	/* find active file size */
    lastactsiz = filestat.st_size;	/* just to save it in .rnlast */

    stat(ACTIVE_TIMES,&filestat);	/* did active.times file grow? */
    if (filestat.st_size == lastnewsize)
	return FALSE;
    lastnewsize = filestat.st_size;

    fputs("\nChecking for new newsgroups...\n",stdout) FLUSH;

    s = filexp(ACTIVE_TIMES);
    tmpfp = fopen(s,"r");
    if (tmpfp == Nullfp) {
	printf(cantopen,s) FLUSH;
	return FALSE;
    }
    lastone = time(Null(time_t*)) - 24L * 60 * 60 - 1;
    while (fgets(buf,LBUFLEN,tmpfp) != Nullch) {
	if ((s = index(buf, ' ')) != Nullch)
	    if ((lastone = atol(s+1)) >= lastnewtime) {
		char tmpbuf[80];
		*s = '\0';
		if (findact(tmpbuf, buf, s - buf, 0L) >= 0)
		    get_ng(buf,FALSE);	/* add newsgroup, maybe */
	    }
    }
    fclose(tmpfp);
    lastnewtime = lastone+1;		/* remember time of last new group */
					/* (ends up back in .rnlast) */
    return oldnext != nextrcline;
}
#endif /* SERVER */
#else /* not ACTIVE_TIMES */

bool
find_new_groups()
{
    long oldactsiz = lastactsiz;
    NG_NUM oldnext = nextrcline;	/* remember # lines in newsrc */

    fstat(fileno(actfp),&filestat);	/* did active file grow? */

    if (filestat.st_size == lastactsiz)
	return FALSE;
    lastactsiz = filestat.st_size;	/* remember new size */

#ifdef VERBOSE
    IF(verbose)
	fputs("\nChecking active file for new newsgroups...\n",stdout) FLUSH;
    ELSE
#endif
#ifdef TERSE
	fputs("\nNew newsgroups:\n",stdout) FLUSH;
#endif

#ifdef FASTNEW				/* bad soft ptrs -> edited active */
    if (!writesoft && oldactsiz) {	/* maybe just do tail of file? */
	fseek(actfp,oldactsiz-NL_SIZE,0);
	fgets(buf,LBUFLEN,actfp);
	if (*buf == '\n' && !newlist(FALSE,FALSE))
	    goto bugout;
    }
#endif
    fseek(actfp,0L,0);		/* rewind active file */
    newlist(TRUE,FALSE);		/* sure hope they use hashing... */
bugout:
    return oldnext != nextrcline;
}

/* return creation time of newsgroup */

long
birthof(ngnam,ngsize)
char *ngnam;
ART_NUM ngsize;
{
    char tst[128];
    long time();
 
#ifdef SERVER		/* ngsize not used */
    long tot;
    sprintf(tst,"GROUP %s",ngnam);
#ifdef DEBUGGING
    if (debug & DEB_NNTP)
	printf(">%s\n", tst) FLUSH;
#endif
    put_server(tst);
    (void) nntp_get(tst, sizeof(tst));
#ifdef DEBUGGING
    if (debug & DEB_NNTP)
	printf("<%s\n", tst) FLUSH;
#endif
    if (*tst != CHAR_OK) return(0); /* not a real group */
    (void) sscanf(tst,"%*d%ld",&tot);
    if (tot > 0) return(time(Null(long *)));
    else return(0);
#else /* not SERVER */

    sprintf(tst, ngsize ? "%s/%s/1" : "%s/%s" ,spool,getngdir(ngnam));
    if (stat(tst,&filestat) < 0)
	return (ngsize ? 0L : time(Null(long *)));
	/* not there, assume something good */
    else
	return filestat.st_mtime;

#endif
}
#endif /* ACTIVE_TIMES */

bool
scanactive()
{
    NG_NUM oldnext = nextrcline;	/* remember # lines in newsrc */

    fseek(actfp,0L,0);
    newlist(TRUE,TRUE);
    if (nextrcline != oldnext) {	/* did we add any new groups? */
	return TRUE;
    }
    return FALSE;
}

#endif

