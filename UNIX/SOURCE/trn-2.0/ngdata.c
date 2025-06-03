/* $Id: ngdata.c,v 1.2 92/01/11 16:29:42 usenet Exp $
 *
 * $Log:	ngdata.c,v $
 * Revision 1.2  92/01/11  16:29:42  usenet
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
 * Revision 4.4.1.1  1991/09/25  19:38:08  sob
 * Some adaptions for CNEWS
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

#include "EXTERN.h"
#include "common.h"
#include "ndir.h"
#include "rcstuff.h"
#include "rn.h"
#include "ng.h"
#include "intrp.h"
#include "final.h"
#include "rcln.h"
#include "util.h"
#ifdef SERVER
#include "server.h"
#endif
#include "INTERN.h"
#include "ngdata.h"

void
ngdata_init()
{
    char *cp;

    /* open the active file */

#ifdef SERVER
#ifdef DEBUGGING
    if (debug & DEB_NNTP)
	printf(">LIST\n") FLUSH;
#endif
    put_server("LIST");		/* tell server we want the active file */
    nntp_get(ser_line, sizeof(ser_line));
#ifdef DEBUGGING
    if (debug & DEB_NNTP)
	printf("<%s\n", ser_line) FLUSH;
#endif
    if (*ser_line != CHAR_OK) {		/* and then see if that's ok */
	fprintf(stdout, "Can't get active file from server: \n%s\n", ser_line);
	finalize(1);
    }
    time(&lastactfetch);

    cp = filexp("%P/rrnact.%$");	/* make a temporary name */
    strcpy(active_name, cp);
    actfp = fopen(active_name, "w+");	/* and get ready */
    if (actfp == Nullfp) {
	printf(cantopen,active_name) FLUSH;
	finalize(1);
    }

    activeitems = 0;
    while (1) {
	if (nntp_get(ser_line, sizeof(ser_line)) < 0) {
	    printf("Can't get active file from server:\ntransfer failed after %d entries\n", activeitems);
	    finalize(1);
	}
	if (ser_line[0] == '.')		/* while there's another line */
		break;			/* get it and write it to */
	activeitems++;
	fputs(ser_line, actfp);
	putc('\n', actfp);
    }

    if (ferror(actfp)) {
	printf("Error writing to active file %s.\n", active_name) FLUSH;
	finalize(1);
    }
#else /* not SERVER */

    cp = filexp(ACTIVE);
    actfp = fopen(cp,"r");
    if (actfp == Nullfp) {
	printf(cantopen,cp) FLUSH;
	finalize(1);
    }
    activeitems = 0;
    /* count entries */
    while(fgets(buf,LBUFLEN,actfp) != NULL)
	activeitems++;
#endif
    if (fseek(actfp,0L,0) == -1) {	/* just get to the beginning */
	printf("Error seeking in active file.\n") FLUSH;
	finalize(1);
    }

    return;
}

/* find the maximum article number of a newsgroup */

ART_NUM
getngsize(num)
register NG_NUM num;
{
    register int len;
    register char *nam;
    char tmpbuf[80];
    ART_POS oldsoft;
    long last;

    nam = rcline[num];
    len = rcnums[num] - 1;
    softtries++;
#ifdef DEBUGGING
    if (debug & DEB_SOFT_POINTERS)
	printf("Softptr = %ld\n",(long)softptr[num]) FLUSH;
#endif
    oldsoft = softptr[num];
    if ((softptr[num] = findact(tmpbuf, nam, len, (long)oldsoft)) >= 0) {
	if (softptr[num] != oldsoft) {
	    softmisses++;
	    writesoft = TRUE;
	}
    }
    else {
	softptr[num] = 0;
#ifdef USETHREADS
	if (RCCHAR(rcchar[num]) == ':')
#else
	if (rcchar[num] == ':')		/* unsubscribe quietly */
#endif
	    rcchar[num] = NEGCHAR;
	return TR_BOGUS;		/* well, not so quietly, actually */
    }
	
#ifdef DEBUGGING
    if (debug & DEB_SOFT_POINTERS) {
	printf("Should be %ld\n",(long)softptr[num]) FLUSH;
    }
#endif
    {
	register char *s;
	ART_NUM tmp;

	for (s=tmpbuf+len+1; isdigit(*s); s++) ;
	if (tmp = atol(s))
#ifdef MININACT
#ifdef CACHEFIRST
	    abs1st[num] = tmp;
#else
	    abs1st = tmp;
#endif
#endif
	if (!in_ng) {
	    for (s++; isdigit(*s); s++) ;
	    while (isspace(*s)) s++;
	    switch (*s) {
	    case 'n': moderated = getval("NOPOSTRING"," (no posting)"); break;
	    case 'm': moderated = getval("MODSTRING", " (moderated)"); break;
	    /* This shouldn't even occur.  What are we doing in a non-existent
	       group?  Disallow it. */
	    case 'x': return TR_BOGUS;
	    /* what should be done about refiled groups?  rn shouldn't even
	       be in them (ie, if sci.aquaria is refiled to rec.aquaria, then
	       get the news there) */
	    case '=': return TR_BOGUS;
	    default: moderated = nullstr;
	    }
	}
    }
    last = atol(tmpbuf+len+1);
    if (last < ngmax[num]) {
#ifdef SERVER
	if (time(Null(time_t*)) - lastactfetch > MINFETCHTIME) {
	    fclose(actfp);
	    ngdata_init();	/* re-grab the active file */
	}
#endif
	return ngmax[num];
    }
    return (ART_NUM)last;
}

ACT_POS
findact(outbuf,nam,len,suggestion)
char *outbuf;
char *nam;
int len;
long suggestion;
{
    ACT_POS retval;

    fseek(actfp,100000L,1);	/* hopefully this forces a reread */
    if (suggestion == 0L || fseek(actfp,suggestion,0) < 0 ||
      fgets(outbuf,80,actfp) == Nullch ||
      outbuf[len] != ' ' ||
      strnNE(outbuf,nam,len)) {
#ifdef DEBUGGING
	if (debug & DEB_SOFT_POINTERS)
	    printf("Missed, looking for %s in %sLen = %d\n",nam,outbuf,len)
	      FLUSH;
#endif
	fseek(actfp,0L,0);
#ifndef lint
	retval = (ACT_POS)ftell(actfp);
#else
	retval = Null(ACT_POS);
#endif /* lint */
	while (fgets(outbuf,80,actfp) != Nullch) {
	    if (outbuf[len] == ' ' && strnEQ(outbuf,nam,len))
		return retval;
#ifndef lint
	    retval = (ACT_POS) ftell(actfp);
#endif /* lint */
	    if (ferror(actfp)) {
		perror("error on active file"); /* something is wrong */
		sig_catcher(0);
	    }
	}
	if(ferror(actfp)) {
	    perror("error on active file");
	    sig_catcher(0);
	}
	return (ACT_POS) -1;
    }
    else
#ifndef lint
	return (ACT_POS) suggestion;
#else
	return retval;
#endif /* lint */
    /*NOTREACHED*/
}

/* determine the absolutely first existing article number */
#ifdef SERVER
ART_NUM
getabsfirst(ngnum,ngsize)
register NG_NUM ngnum;
ART_NUM ngsize;
{
    long a1st, last;

#ifdef CACHEFIRST
    if (a1st = abs1st[ngnum])
	return (ART_NUM)a1st;
#endif
#ifdef MININACT
    getngsize(ngnum);
# ifdef CACHEFIRST
    return abs1st[ngnum];
# else
    return abs1st;
# endif
#else
    getngsize(ngnum); /* set moderated as side effect */
    sprintf(ser_line,"GROUP %s",rcline[ngnum]);
#ifdef DEBUGGING
    if (debug & DEB_NNTP)
	printf(">%s\n", ser_line) FLUSH;
#endif
    put_server(ser_line);
    if (nntp_get(ser_line, sizeof(ser_line)) < 0) {
	fprintf(stderr, "\nrrn: Unexpected close of server socket.\n");
	finalize(1);
    }
#ifdef DEBUGGING
    if (debug & DEB_NNTP)
	printf("<%s\n", ser_line) FLUSH;
#endif
    if (*ser_line == CHAR_FATAL){
	fprintf(stderr,"\nrrn: %s\n",ser_line);
	finalize(1);
    }
    if (*ser_line != CHAR_OK)		/* and then see if that's ok */
	a1st = ngsize+1;		/* nothing there */
    else {
	(void) sscanf(ser_line,"%*d%*d%ld%ld",&a1st,&last);
	ngmax[ngnum] = last;
    }
# ifdef CACHEFIRST
    abs1st[ngnum] = (ART_NUM)a1st;
# endif
    return (ART_NUM)a1st;
#endif
}
/* we already know the lowest article number with NNTP */
ART_NUM
getngmin(dirname,floor)
char *dirname;
ART_NUM floor;
{
    return(floor);		/* dirname not used */
}

#else /*SERVER*/

ART_NUM
getabsfirst(ngnum,ngsize)
register NG_NUM ngnum;
ART_NUM ngsize;
{
    register ART_NUM a1st;
#ifndef MININACT
    char dirname[MAXFILENAME];
#endif

#ifdef CACHEFIRST
    if (a1st = abs1st[ngnum])
	return a1st;
#endif
#ifdef MININACT
    getngsize(ngnum);
# ifdef CACHEFIRST
    return abs1st[ngnum];
# else
    return abs1st;
# endif
#else /* not MININACT */
    getngsize(ngnum); /* set moderate as side effect */
    sprintf(dirname,"%s/%s",spool,getngdir(rcline[ngnum]));
    a1st = getngmin(dirname,0L);
    if (!a1st)				/* nothing there at all? */
	a1st = ngsize+1;		/* aim them at end of newsgroup */
# ifdef CACHEFIRST
    abs1st[ngnum] = a1st;
# endif
    return a1st;
#endif /* MININACT */
}

/* scan a directory for minimum article number greater than floor */

ART_NUM
getngmin(dirname,floor)
char *dirname;
ART_NUM floor;
{
    register DIR *dirp;
    register struct DIRTYPE *dp;
    register ART_NUM min = 1000000;
    register ART_NUM maybe;
    register char *p;
#ifdef notdef
    char tmpbuf[128];
#endif
    
    dirp = opendir(dirname);
    if (!dirp)
	return 0;
    while ((dp = readdir(dirp)) != Null(struct DIRTYPE *)) {
	if ((maybe = atol(dp->d_name)) < min && maybe > floor) {
	    for (p = dp->d_name; *p; p++)
		if (!isdigit(*p))
		    goto nope;
#ifdef notdef
	   /* 
	    * If newsgroup names ever go entirely numeric, then
	    * this code will have to be reinserted.
	    * For the time being, we assume that if a numeric name is
	    * found, it must be an article (and not a directory).
	    * This will avoid two stat(2) calls for those running
	    * rn.
	    */
	    if (*dirname == '.' && !dirname[1])
		stat(dp->d_name, &filestat);
	    else {
		sprintf(tmpbuf,"%s/%s",dirname,dp->d_name);
		stat(tmpbuf, &filestat);
	    }
	    if (! (filestat.st_mode & S_IFDIR))
#endif
		min = maybe;
	}
      nope:
	;
    }
    closedir(dirp);
    return min==1000000 ? 0 : min;
}
#endif
