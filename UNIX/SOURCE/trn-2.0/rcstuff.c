/* $Id: rcstuff.c,v 1.3 92/01/11 16:30:15 usenet Exp $
 *
 * $Log:	rcstuff.c,v $
 * Revision 1.3  92/01/11  16:30:15  usenet
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
 * Revision 1.2  92/01/11  16:06:40  usenet
 * header twiddling, bug fixes
 * 
 * Revision 4.4.2.1  1991/12/01  18:05:42  sob
 * Patchlevel 2 changes
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

#include "EXTERN.h"
#include "common.h"
#include "util.h"
#include "ngdata.h"
#include "term.h"
#include "final.h"
#include "rn.h"
#include "intrp.h"
#include "only.h"
#include "rcln.h"
#ifdef SERVER
#include "server.h"
#endif
#include "autosub.h"
#include "INTERN.h"
#include "rcstuff.h"

char *rcname INIT(Nullch);		/* path name of .newsrc file */
char *rctname INIT(Nullch);		/* path name of temp .newsrc file */
char *rcbname INIT(Nullch);		/* path name of backup .newsrc file */
char *softname INIT(Nullch);		/* path name of .rnsoft file */
FILE *rcfp INIT(Nullfp);		/* .newsrc file pointer */

static void grow_rc_arrays ANSI((int));
static void parse_rcline ANSI((NG_NUM));

#ifdef HASHNG
    static int hashsiz;
    static short *hashtbl = NULL;
#endif

bool
rcstuff_init()
{
    register NG_NUM newng;
    register int i;
    register bool foundany = FALSE;
    char *some_buf;
    long length;
#ifdef SERVER
    char *cp;
#endif /* SERVER */
    bool found = FALSE;

    /* make filenames */

#ifdef SERVER

    if (cp = getenv("NEWSRC"))
	rcname = savestr(filexp(cp));
    else
	rcname = savestr(filexp(RCNAME));

#else /* not SERVER */

    rcname = savestr(filexp(RCNAME));

#endif /* SERVER */

    rctname = savestr(filexp(RCTNAME));
    rcbname = savestr(filexp(RCBNAME));
    softname = savestr(filexp(SOFTNAME));
    
    /* make sure the .newsrc file exists */

    newsrc_check();

    /* open .rnsoft file containing soft ptrs to active file */

    tmpfp = fopen(softname,"r");
    if (tmpfp == Nullfp)
	writesoft = TRUE;

    /* allocate memory for rc file globals */
    grow_rc_arrays(1500);

    /* read in the .newsrc file */

    for (nextrcline = 0;
	(some_buf = get_a_line(buf,LBUFLEN,rcfp)) != Nullch;
	nextrcline++) {
					/* for each line in .newsrc */
	char tmpbuf[10];

	newng = nextrcline;		/* get it into a register */
	length = len_last_line_got;	/* side effect of get_a_line */
	if (length <= 1) {		/* only a newline??? */
	    nextrcline--;		/* compensate for loop increment */
	    continue;
	}
	if (newng >= maxrcline)		/* check for overflow */
	    grow_rc_arrays(maxrcline + 500);
	if (tmpfp != Nullfp && fgets(tmpbuf,10,tmpfp) != Nullch)
	    softptr[newng] = atol(tmpbuf);
	else
	    softptr[newng] = 0;
	some_buf[--length] = '\0';	/* wipe out newline */
	if (checkflag)			/* no extra mallocs for -c */
	    rcline[newng] = some_buf;
	else if (some_buf == buf) {
	    rcline[newng] = savestr(some_buf);
					/* make a semipermanent copy */
	}
	else {
	    /*NOSTRICT*/
#ifndef lint
	    some_buf = saferealloc(some_buf,(MEM_SIZE)(length+1));
#endif /* lint */
	    rcline[newng] = some_buf;
	}
#ifdef NOTDEF
	if (strnEQ(some_buf,"to.",3)) {	/* is this a non-newsgroup? */
	    nextrcline--;		/* destroy this line */
	    continue;
	}
#endif
	if (*some_buf == ' ' ||
	  *some_buf == '\t' ||
	  strnEQ(some_buf,"options",7)) {		/* non-useful line? */
	    toread[newng] = TR_JUNK;
	    rcchar[newng] = ' ';
	    rcnums[newng] = 0;
	    continue;
	}
	parse_rcline(newng);
	if (rcchar[newng] == NEGCHAR) {
	    toread[newng] = TR_UNSUB;
	    continue;
	}

	/* now find out how much there is to read */

	if (!inlist(buf) || (suppress_cn && foundany && !paranoid))
	    toread[newng] = TR_NONE;	/* no need to calculate now */
	else
	    set_toread(newng);
#ifdef VERBOSE
	if (!checkflag && softmisses == 1) {
	    softmisses++;		/* lie a little */
	    fputs("(Revising soft pointers--be patient.)\n",stdout) FLUSH;
	}
#endif
	if (toread[newng] > TR_NONE) {	/* anything unread? */
	    if (!foundany) {
		starthere = newng;
		foundany = TRUE;	/* remember that fact*/
	    }
	    if (suppress_cn) {		/* if no listing desired */
		if (checkflag) {	/* if that is all they wanted */
		    finalize(1);	/* then bomb out */
		}
	    }
	    else {
#ifdef VERBOSE
		IF(verbose)
		    printf("Unread news in %-40s %5ld article%s\n",
			rcline[newng],(long)toread[newng],
			toread[newng]==TR_ONE ? nullstr : "s") FLUSH;
		ELSE
#endif
#ifdef TERSE
		    printf("%s: %ld article%s\n",
			rcline[newng],(long)toread[newng],
			toread[newng]==TR_ONE ? nullstr : "s") FLUSH;
#endif
		if (int_count) {
		    countdown = 1;
		    int_count = 0;
		}
		if (countdown) {
		    if (! --countdown) {
			fputs("etc.\n",stdout) FLUSH;
			if (checkflag)
			    finalize(1);
			suppress_cn = TRUE;
		    }
		}
	    }
	}
    }
    fclose(rcfp);			/* close .newsrc */
    if (tmpfp != Nullfp)
	fclose(tmpfp);			/* close .rnsoft */
    if (checkflag) {			/* were we just checking? */
	finalize(foundany);		/* tell them what we found */
    }
    if (paranoid)
	cleanup_rc();

#ifdef HASHNG

    /* find a good hash size given num of newsgroups */

    hashsiz = maxrcline * 1.15;
    if ((hashsiz & 1) == 0)	/* must be odd */
	hashsiz++;

    if (hashsiz <= activeitems)
	hashsiz = activeitems * 1.15;
    /* find one that's prime */
    while (! found) {
	for (i=3; ; i+=2) {
	    if ( (float)(hashsiz)/i == hashsiz/i )
		break;
	    if ( i > hashsiz/i ) {
		found = TRUE;
		break;
	    }
	}
	if (! found)
	    hashsiz += 2;
    }

    hashtbl = (short *) safemalloc( hashsiz * sizeof(short) );

    for (i=0; i<hashsiz; i++)
	hashtbl[i] = -1;

    if (!checkflag)
	for (i=0; i<nextrcline; i++)
	    sethash(i);
#endif

#ifdef DEBUGGING
    if (debug & DEB_HASH) {
	page_init();
	for (i=0; i<hashsiz; i++) {
	    sprintf(buf,"%d	%d",i,hashtbl[i]);
	    print_lines(buf,NOMARKING);
	}
    }
#endif

    return foundany;
}

static void
parse_rcline(ngnum)
NG_NUM ngnum;
{
#ifdef M_XENIX
    char *s;			/* bypass a compiler bug (ugh!) */
#else
    register char *s;
#endif

    for (s = rcline[ngnum]; *s && *s != ':' && *s != NEGCHAR; s++) ;
    if (!*s && !checkflag) {
#ifndef lint
	rcline[ngnum] = saferealloc(rcline[ngnum],
				(MEM_SIZE)(s - rcline[ngnum]) + 3);
#endif /* lint */
	strcpy(s, ": ");
    }
#ifdef USETHREADS
    if (*s == ':' && s[1] && s[2] == '0') {
	rcchar[ngnum] = '0';
	s[2] = '1';
    } else
#endif
	rcchar[ngnum] = *s;	/* salt away the : or ! */
    rcnums[ngnum] = (char)(s - rcline[ngnum]) + 1;
				/* remember where the numbers are */
    *s = '\0';			/* null terminate newsgroup name */
}

void
abandon_ng(ngnum)
NG_NUM ngnum;
{
    char *some_buf = Nullch;

    /* open .oldnewsrc and try to find the prior value for the group. */
    if ((rcfp = fopen(rcbname, "r")) != Nullfp) {
	int length = rcnums[ngnum] - 1;

	while ((some_buf = get_a_line(buf,LBUFLEN,rcfp)) != Nullch) {
	    if (len_last_line_got <= 0)
		continue;
	    some_buf[len_last_line_got-1] = '\0';	/* wipe out newline */
	    if ((some_buf[length] == ':' || some_buf[length] == NEGCHAR)
	     && strnEQ(rcline[ngnum], some_buf, length)) {
		break;
	    }
	    if (some_buf != buf)
		free(some_buf);
	}
	fclose(rcfp);
    } else if (errno != ENOENT) {
	printf("Unable to open %s.\n", rcbname) FLUSH;
	return;
    }
    if (some_buf == Nullch) {
	some_buf = rcline[ngnum] + rcnums[ngnum];
	if (*some_buf == ' ')
	    some_buf++;
	*some_buf = '\0';
#ifdef CACHEFIRST
	abs1st[ngnum] = 0;	/* force group to be re-calculated */
#endif
    }
    else {
	free(rcline[ngnum]);
	if (some_buf == buf) {
	    rcline[ngnum] = savestr(some_buf);
	}
	else {
	    /*NOSTRICT*/
#ifndef lint
	    some_buf = saferealloc(some_buf, (MEM_SIZE)(len_last_line_got));
#endif /* lint */
	    rcline[ngnum] = some_buf;
	}
    }
    parse_rcline(ngnum);
    if (rcchar[ngnum] == NEGCHAR)
	rcchar[ngnum] = ':';
    set_toread(ngnum);
}

/* try to find or add an explicitly specified newsgroup */
/* returns TRUE if found or added, FALSE if not. */
/* assumes that we are chdir'ed to SPOOL */

#define ADDNEW_SUB ':'
#define ADDNEW_UNSUB '!'

bool
get_ng(what,do_reloc)
char *what;
bool_int do_reloc;
{
    char *ntoforget;
    char promptbuf[128];
    int autosub;

#ifdef VERBOSE
    IF(verbose)
	ntoforget = "Type n to forget about this newsgroup.\n";
    ELSE
#endif
#ifdef TERSE
	ntoforget = "n to forget it.\n";
#endif
    if (index(what,'/')) {
	dingaling();
	printf("\nBad newsgroup name.\n") FLUSH;
	return FALSE;
    }
    set_ngname(what);
    ng = find_ng(ngname);
    if (ng == nextrcline) {		/* not in .newsrc? */

#ifdef SERVER
	sprintf(ser_line, "GROUP %s", ngname);
	put_server(ser_line);
	if (nntp_get(ser_line, sizeof(ser_line)) < 0) {
	    fprintf(stderr, "\nrrn: Unexpected close of server socket.\n");
	    finalize(1);
	}
	if (*ser_line != CHAR_OK) {
	    if (atoi(ser_line) != ERR_NOGROUP) {
		fprintf(stderr, "\nServer response to GROUP %s:\n%s\n",
		    ngname, ser_line);
		finalize(1);
	    }
#else /* not SERVER */

	if (ng >= maxrcline)		/* check for overflow */
	    grow_rc_arrays(maxrcline + 25);

	if ((softptr[ng] = findact(buf,ngname,strlen(ngname),0L)) < 0 ) {

#endif /* SERVER */

	    dingaling();
#ifdef VERBOSE
	    IF(verbose)
		printf("\nNewsgroup %s does not exist!\n",ngname) FLUSH;
	    ELSE
#endif
#ifdef TERSE
		printf("\nNo %s!\n",ngname) FLUSH;
#endif
	    sleep(2);
	    return FALSE;
	}
	autosub = auto_subscribe(ngname);
	if (!autosub) autosub = addnewbydefault;
	if (autosub) {
		printf("(Adding %s to end of your .newsrc %ssubscribed)\n",
		       ngname, (autosub == ADDNEW_SUB) ? "" : "un");
		ng = add_newsgroup(ngname, autosub);
	        do_reloc = FALSE;
	} else {
#ifdef VERBOSE
	IF(verbose)
	    sprintf(promptbuf,"\nNewsgroup %s not in .newsrc--subscribe? [ynYN] ",ngname);
	ELSE
#endif
#ifdef TERSE
	    sprintf(promptbuf,"\nSubscribe %s? [ynYN] ",ngname);
#endif
reask_add:
	in_char(promptbuf,'A');
	setdef(buf,"y");
#ifdef VERIFY
	printcmd();
#endif
	putchar('\n') FLUSH;
	if (*buf == 'h') {
#ifdef VERBOSE
	    IF(verbose)
		printf("Type y or SP to add %s to your .newsrc.\nType Y to add all new groups to the end of your .newsrc.\nType N to add all new groups to the end of your .newsrc unsubscribed.\n", ngname)
		  FLUSH;
	    ELSE
#endif
#ifdef TERSE
		fputs("y or SP to add, Y to add all new groups, N to add all new groups unsubscribed\n",stdout) FLUSH;
#endif
	    fputs(ntoforget,stdout) FLUSH;
	    goto reask_add;
	}
	else if (*buf == 'n' || *buf == 'q') {
	    ng = add_newsgroup(ngname, '!');
	    return FALSE;
	}
	else if (*buf == 'y') {
	    ng = add_newsgroup(ngname, ':');
	    do_reloc = TRUE;
	}
	else if (*buf == 'Y') {
	    fputs(
    "(I'll add all new newsgroups (subscribed) to the end of your .newsrc.)\n",
		  stdout);
	    addnewbydefault = ADDNEW_SUB;
	    printf("(Adding %s to end of your .newsrc subscribed)\n", ngname);
	    ng = add_newsgroup(ngname, ':');
	    do_reloc = FALSE;
	}
	else if (*buf == 'N') {
	    fputs(
  "(I'll add all new newsgroups (unsubscribed) to the end of your .newsrc.)\n",
		  stdout);
	    addnewbydefault = ADDNEW_UNSUB;
	    printf("(Adding %s to end of your .newsrc unsubscribed)\n", ngname);
	    ng = add_newsgroup(ngname, '!');
	    do_reloc = FALSE;
	}
	else {
	    fputs(hforhelp,stdout) FLUSH;
	    settle_down();
	    goto reask_add;
	}
      }
    }
    else if (mode == 'i')		/* adding new groups during init? */
	return FALSE;
    else if (rcchar[ng] == NEGCHAR) {	/* unsubscribed? */
#ifdef VERBOSE
	IF(verbose)
	    sprintf(promptbuf,
"\nNewsgroup %s is currently unsubscribed to--resubscribe? [yn] ",ngname)
  FLUSH;
	ELSE
#endif
#ifdef TERSE
	    sprintf(promptbuf,"\n%s unsubscribed--resubscribe? [yn] ",ngname)
	      FLUSH;
#endif
reask_unsub:
	in_char(promptbuf,'R');
	setdef(buf,"y");
#ifdef VERIFY
	printcmd();
#endif
	putchar('\n') FLUSH;
	if (*buf == 'h') {
#ifdef VERBOSE
	    IF(verbose)
		printf("Type y or SP to resubscribe to %s.\n", ngname) FLUSH;
	    ELSE
#endif
#ifdef TERSE
		fputs("y or SP to resubscribe.\n",stdout) FLUSH;
#endif
	    fputs(ntoforget,stdout) FLUSH;
	    goto reask_unsub;
	}
	else if (*buf == 'n' || *buf == 'q') {
	    return FALSE;
	}
	else if (*buf == 'y') {
#ifdef USETHREADS
	    register char *cp = rcline[ng] + rcnums[ng];
	    rcchar[ng] = (*cp && cp[1] == '0' ? '0' : ':');
#else
	    rcchar[ng] = ':';
#endif
	    do_reloc = FALSE;
	}
	else {
	    fputs(hforhelp,stdout) FLUSH;
	    settle_down();
	    goto reask_unsub;
	}
    }

    /* now calculate how many unread articles in newsgroup */

    set_toread(ng);
#ifdef RELOCATE
    if (do_reloc)
	ng = relocate_newsgroup(ng,-1);
#endif
    return toread[ng] >= TR_NONE;
}

/* add a newsgroup to the .newsrc file (eventually) */

NG_NUM
add_newsgroup(ngn, c)
char *ngn;
char_int c;
{
    register NG_NUM newng = nextrcline++;
					/* increment max rcline index */

    if (newng >= maxrcline)		/* check for overflow */
	grow_rc_arrays(maxrcline + 25);

    rcnums[newng] = strlen(ngn) + 1;
    rcline[newng] = safemalloc((MEM_SIZE)(rcnums[newng] + 2));
    strcpy(rcline[newng],ngn);		/* and copy over the name */
    strcpy(rcline[newng]+rcnums[newng], " ");
    rcchar[newng] = c;			/* subscribe or unsubscribe */
    toread[newng] = TR_NONE;	/* just for prettiness */
#ifdef HASHNG
    sethash(newng);			/* so we can find it again */
#endif
    return newng;
}

#ifdef RELOCATE
NG_NUM
relocate_newsgroup(ngx,newng)
NG_NUM ngx;
NG_NUM newng;
{
    char *dflt = (ngx!=current_ng ? "$^.L" : "$^L");
    char *tmprcline;
    ART_UNREAD tmptoread;
    char tmprcchar;
    char tmprcnums;
    ACT_POS tmpsoftptr;
    register NG_NUM i;
    ART_NUM tmpngmax;
#ifdef CACHEFIRST
    ART_NUM tmpabs1st;
#endif
    
    starthere = 0;                      /* Disable this optimization */
    writesoft = TRUE;			/* Update soft pointer file */
    if (ngx < nextrcline-1) {
#ifdef HASHNG
	for (i=0; i<hashsiz; i++) {
	    if (hashtbl[i] > ngx)
		--hashtbl[i];
	    else if (hashtbl[i] == ngx)
		hashtbl[i] = nextrcline-1;
	}
#endif
	tmprcline = rcline[ngx];
	tmptoread = toread[ngx];
	tmprcchar = rcchar[ngx];
	tmprcnums = rcnums[ngx];
	tmpsoftptr = softptr[ngx];
	tmpngmax = ngmax[ngx];
#ifdef CACHEFIRST
	tmpabs1st = abs1st[ngx];
#endif
	for (i=ngx+1; i<nextrcline; i++) {
	    rcline[i-1] = rcline[i];
	    toread[i-1] = toread[i];
	    rcchar[i-1] = rcchar[i];
	    rcnums[i-1] = rcnums[i];
	    softptr[i-1] = softptr[i];
	    ngmax[i-1] = ngmax[i];
#ifdef CACHEFIRST
	    abs1st[i-1] = abs1st[i];
#endif
	}
	rcline[nextrcline-1] = tmprcline;
	toread[nextrcline-1] = tmptoread;
	rcchar[nextrcline-1] = tmprcchar;
	rcnums[nextrcline-1] = tmprcnums;
	softptr[nextrcline-1] = tmpsoftptr;
	ngmax[nextrcline-1] = tmpngmax;
#ifdef CACHEFIRST
	abs1st[nextrcline-1] = tmpabs1st;
#endif
    }
    if (current_ng > ngx)
	current_ng--;
    if (newng < 0) {
      reask_reloc:
	unflush_output();		/* disable any ^O in effect */
#ifdef VERBOSE
	IF(verbose)
	    printf("\nPut newsgroup where? [%s] ", dflt);
	ELSE
#endif
#ifdef TERSE
	    printf("\nPut where? [%s] ", dflt);
#endif
	fflush(stdout);
      reinp_reloc:
	eat_typeahead();
	getcmd(buf);
	if (errno || *buf == '\f') {
			    /* if return from stop signal */
	    goto reask_reloc;	/* give them a prompt again */
	}
	setdef(buf,dflt);
#ifdef VERIFY
	printcmd();
#endif
	if (*buf == 'h') {
#ifdef VERBOSE
	    IF(verbose) {
		printf("\n\n\
Type ^ to put the newsgroup first (position 0).\n\
Type $ to put the newsgroup last (position %d).\n", nextrcline-1);
		printf("\
Type . to put it before the current newsgroup (position %d).\n", current_ng);
		printf("\
Type -newsgroup name to put it before that newsgroup.\n\
Type +newsgroup name to put it after that newsgroup.\n\
Type a number between 0 and %d to put it at that position.\n", nextrcline-1);
		printf("\
Type L for a listing of newsgroups and their positions.\n") FLUSH;
	    }
	    ELSE
#endif
#ifdef TERSE
	    {
		printf("\n\n\
^ to put newsgroup first (pos 0).\n\
$ to put last (pos %d).\n", nextrcline-1);
		printf("\
. to put before current newsgroup (pos %d).\n", current_ng);
		printf("\
-newsgroup to put before newsgroup.\n\
+newsgroup to put after.\n\
number in 0-%d to put at that pos.\n", nextrcline-1);
		printf("\
L for list of .newsrc.\n") FLUSH;
	    }
#endif
	    goto reask_reloc;
	}
	else if (*buf == 'L') {
	    putchar('\n') FLUSH;
	    list_newsgroups();
	    goto reask_reloc;
	}
	else if (isdigit(*buf)) {
	    if (!finish_command(TRUE))	/* get rest of command */
		goto reinp_reloc;
	    newng = atol(buf);
	    if (newng < 0)
		newng = 0;
	    if (newng >= nextrcline)
		return nextrcline-1;
	}
	else if (*buf == '^') {
	    putchar('\n') FLUSH;
	    newng = 0;
	}
	else if (*buf == '$') {
	    newng = nextrcline-1;
	}
	else if (*buf == '.') {
	    putchar('\n') FLUSH;
	    newng = current_ng;
	}
	else if (*buf == '-' || *buf == '+') {
	    if (!finish_command(TRUE))	/* get rest of command */
		goto reinp_reloc;
	    newng = find_ng(buf+1);
	    if (newng == nextrcline) {
		fputs("Not found.",stdout) FLUSH;
		goto reask_reloc;
	    }
	    if (*buf == '+')
		newng++;
	}
	else {
	    printf("\n%s",hforhelp) FLUSH;
	    settle_down();
	    goto reask_reloc;
	}
    }
    if (newng < nextrcline-1) {
#ifdef HASHNG
	for (i=0; i<hashsiz; i++) {
	    if (hashtbl[i] == nextrcline-1)
		hashtbl[i] = newng;
	    else if (hashtbl[i] >= newng)
		++hashtbl[i];
	}
#endif
	tmprcline = rcline[nextrcline-1];
	tmptoread = toread[nextrcline-1];
	tmprcchar = rcchar[nextrcline-1];
	tmprcnums = rcnums[nextrcline-1];
	tmpsoftptr = softptr[nextrcline-1];
	tmpngmax = ngmax[nextrcline-1];
#ifdef CACHEFIRST
	tmpabs1st = abs1st[nextrcline-1];
#endif
	for (i=nextrcline-2; i>=newng; i--) {
	    rcline[i+1] = rcline[i];
	    toread[i+1] = toread[i];
	    rcchar[i+1] = rcchar[i];
	    rcnums[i+1] = rcnums[i];
	    softptr[i+1] = softptr[i];
	    ngmax[i+1] = ngmax[i];
#ifdef CACHEFIRST
	    abs1st[i+1] = abs1st[i];
#endif
	}
	rcline[newng] = tmprcline;
	toread[newng] = tmptoread;
	rcchar[newng] = tmprcchar;
	rcnums[newng] = tmprcnums;
	softptr[newng] = tmpsoftptr;
	ngmax[newng] = tmpngmax;
#ifdef CACHEFIRST
	abs1st[newng] = tmpabs1st;
#endif
    }
    if (current_ng >= newng)
	current_ng++;
    return newng;
}
#endif

/* List out the newsrc with annotations */

void
list_newsgroups()
{
    register NG_NUM i;
    char tmpbuf[2048];
    static char *status[] = {"(READ)","(UNSUB)","(BOGUS)","(JUNK)"};
    int cmd;

    page_init();
    print_lines("\
  #  Status  Newsgroup\n\
",STANDOUT);
    for (i=0; i<nextrcline && !int_count; i++) {
	if (toread[i] >= 0)
	    set_toread(i);
#ifdef USETHREADS
	*(rcline[i] + rcnums[i] - 1) = RCCHAR(rcchar[i]);
#else
	*(rcline[i] + rcnums[i] - 1) = rcchar[i];
#endif
	if (toread[i] > 0)
	    sprintf(tmpbuf,"%3d %6ld   ",i,(long)toread[i]);
	else
	    sprintf(tmpbuf,"%3d %7s  ",i,status[-toread[i]]);
	safecpy(tmpbuf+13,rcline[i],2034);
	*(rcline[i] + rcnums[i] - 1) = '\0';
	if (cmd = print_lines(tmpbuf,NOMARKING)) {
	    if (cmd > 0)
		pushchar(cmd);
	    break;
	}
    }
    int_count = 0;
}

/* find a newsgroup in .newsrc */

NG_NUM
find_ng(ngnam)
char *ngnam;
{
    register NG_NUM ngnum;
#ifdef HASHNG
    register int hashix = hash(ngnam);
    register int incr = 1;

    while ((ngnum = hashtbl[hashix]) >= 0) {
	if (strEQ(rcline[ngnum], ngnam) && toread[ngnum] >= TR_UNSUB)
	    return ngnum;
	hashix = (hashix + incr) % hashsiz;
	incr += 2;			/* offsets from original are in n*2 */
    }
    return nextrcline;			/* = notfound */

#else /* just do linear search */

    for (ngnum = 0; ngnum < nextrcline; ngnum++) {
	if (strEQ(rcline[ngnum],ngnam))
	    break;
    }
    return ngnum;
#endif
}

void
cleanup_rc()
{
    register NG_NUM ngx;
    register NG_NUM bogosity = 0;

#ifdef VERBOSE
    IF(verbose)
	fputs("Checking out your .newsrc--hang on a second...\n",stdout)
	  FLUSH;
    ELSE
#endif
#ifdef TERSE
	fputs("Checking .newsrc--hang on...\n",stdout) FLUSH;
#endif
    for (ngx = 0; ngx < nextrcline; ngx++) {
	if (toread[ngx] >= TR_UNSUB) {
	    set_toread(ngx);		/* this may reset newsgroup */
					/* or declare it bogus */
	}
	if (toread[ngx] == TR_BOGUS)
	    bogosity++;
    }
    for (ngx = nextrcline-1; ngx >= 0 && toread[ngx] == TR_BOGUS; ngx--)
	bogosity--;			/* discount already moved ones */
    if (nextrcline > 5 && bogosity > nextrcline / 2) {
	fputs(
"It looks like the active file is messed up.  Contact your news administrator,\n\
",stdout);
	fputs(
"leave the \"bogus\" groups alone, and they may come back to normal.  Maybe.\n\
",stdout) FLUSH;
    }
#ifdef RELOCATE
    else if (bogosity) {
#ifdef VERBOSE
	IF(verbose)
	    fputs("Moving bogus newsgroups to the end of your .newsrc.\n",
		stdout) FLUSH;
	ELSE
#endif
#ifdef TERSE
	    fputs("Moving boguses to the end.\n",stdout) FLUSH;
#endif
	for (; ngx >= 0; ngx--) {
	    if (toread[ngx] == TR_BOGUS)
		relocate_newsgroup(ngx,nextrcline-1);
	}
#ifdef DELBOGUS
reask_bogus:
	in_char("Delete bogus newsgroups? [ny] ", 'D');
	setdef(buf,"n");
#ifdef VERIFY
	printcmd();
#endif
	putchar('\n') FLUSH;
	if (*buf == 'h') {
#ifdef VERBOSE
	    IF(verbose)
		fputs("\
Type y to delete bogus newsgroups.\n\
Type n or SP to leave them at the end in case they return.\n\
",stdout) FLUSH;
	    ELSE
#endif
#ifdef TERSE
		fputs("y to delete, n to keep\n",stdout) FLUSH;
#endif
	    goto reask_bogus;
	}
	else if (*buf == 'n' || *buf == 'q')
	    ;
	else if (*buf == 'y') {
	    while (toread[nextrcline-1] == TR_BOGUS && nextrcline > 0)
		--nextrcline;		/* real tough, huh? */
	}
	else {
	    fputs(hforhelp,stdout) FLUSH;
	    settle_down();
	    goto reask_bogus;
	}
#endif
    }
#else
#ifdef VERBOSE
    IF(verbose)
	fputs("You should edit bogus newsgroups out of your .newsrc.\n",
	    stdout) FLUSH;
    ELSE
#endif
#ifdef TERSE
	fputs("Edit boguses from .newsrc.\n",stdout) FLUSH;
#endif
#endif
    paranoid = FALSE;
}

#ifdef HASHNG
/* make an entry in the hash table for the current newsgroup */

void
sethash(thisng)
NG_NUM thisng;
{
    register int hashix = hash(rcline[thisng]);
    register int incr = 1;
#ifdef DEBUGGING
    static int hashhits = 0, hashtries = 0;
#endif

#ifdef DEBUGGING
    hashtries++;
#endif
    while (hashtbl[hashix] >= 0) {
#ifdef DEBUGGING
	hashhits++;
	if (debug & DEB_HASH) {
	    printf("  Hash hits: %d / %d\n",hashhits, hashtries) FLUSH;
	}
	hashtries++;
#endif
	hashix = (hashix + incr) % hashsiz;
	incr += 2;			/* offsets from original are in n*2 */
    }
    hashtbl[hashix] = thisng;
}

short prime[] = {1,2,-3,-5,7,11,-13,-17,19,23,-29,-31,37,41,-43,-47,53,57,-59,
    -61,67,71,-73,-79,83,89,-97,-101,1,1,1,1,1,1,1,1,1,1,1,1};

int
hash(ngnam)
register char *ngnam;
{
    register int i = 0;
    register int ch;
    register int sum = 0;
#ifdef DEBUGGING
    char *ngn = ngnam;
#endif

    while (ch = *ngnam++) {
	sum += (ch + i) * prime[i];   /* gives ~ 10% hits at 25% full */
	i++;
    }
#ifdef DEBUGGING
    if (debug & DEB_HASH)
	printf("hash(%s) => %d => %d\n",ngn, sum, (sum<0?-sum:sum)%hashsiz)
	  FLUSH;
#endif
    if (sum < 0)
	sum = -sum;
    return (sum % hashsiz);
}

#endif

void
newsrc_check()
{
    rcfp = fopen(rcname,"r");		/* open it */
    if (rcfp == Nullfp) {			/* not there? */
#ifdef VERBOSE
	IF(verbose)
	    fputs("\nTrying to set up a .newsrc file--running newsetup...\n\n\
",stdout) FLUSH;
	ELSE
#endif
#ifdef TERSE
	    fputs("Setting up .newsrc...\n",stdout) FLUSH;
#endif
	if (doshell(sh,filexp(NEWSETUP)) ||
	    (rcfp = fopen(rcname,"r")) == Nullfp) {
#ifdef VERBOSE
	    IF(verbose)
		fputs("\nCan't create a .newsrc--you must do it yourself.\n\
",stdout) FLUSH;
	    ELSE
#endif
#ifdef TERSE
		fputs("(Fatal)\n",stdout) FLUSH;
#endif
	    finalize(1);
	}
    }
    else {
	UNLINK(rcbname);		/* unlink backup file name */
	link(rcname,rcbname);		/* and backup current name */
    }
}

/* write out the (presumably) revised .newsrc */

void
write_rc()
{
    register NG_NUM tmpng;
    register char *delim;

    rcfp = fopen(rctname, "w");		/* open .newnewsrc */
    if (rcfp == Nullfp) {
	printf(cantrecreate,".newsrc") FLUSH;
	finalize(1);
    }
    if (stat(rcname,&filestat)>=0) {	/* preserve permissions */
	chmod(rctname,filestat.st_mode&0666);
	chown(rctname,filestat.st_uid,filestat.st_gid);	/* if possible */
    }

    /* write out each line*/

    for (tmpng = 0; tmpng < nextrcline; tmpng++) {
	if (rcnums[tmpng]) {
	    delim = rcline[tmpng] + rcnums[tmpng] - 1;
#ifdef USETHREADS
	    *delim = RCCHAR(rcchar[tmpng]);
	    if (rcchar[tmpng] == '0' && delim[2] == '1')
		delim[2] = '0';
#else
	    *delim = rcchar[tmpng];
#endif
	}
	else
	    delim = Nullch;
#ifdef DEBUGGING
	if (debug & DEB_NEWSRC_LINE)
	    printf("%s\n",rcline[tmpng]) FLUSH;
#endif
	if (fprintf(rcfp,"%s\n",rcline[tmpng]) < 0) {
	write_error:
	    printf(cantrecreate,".newsrc") FLUSH;
	    fclose(rcfp);		/* close .newnewsrc */
	    UNLINK(rctname);
	    finalize(1);
	}
	if (delim) {
	    *delim = '\0';		/* might still need this line */
#ifdef USETHREADS
	    if (rcchar[tmpng] == '0' && delim[2] == '0')
		delim[2] = '1';
#endif
	}
    }
    fflush(rcfp);
    if (ferror(rcfp))
	goto write_error;

    fclose(rcfp);			/* close .newnewsrc */
    UNLINK(rcname);
#ifdef RENAME
    rename(rctname,rcname);
#else
    link(rctname,rcname);
    UNLINK(rctname);
#endif

    if (writesoft) {
	tmpfp = fopen(filexp(softname), "w");	/* open .rnsoft */
	if (tmpfp == Nullfp) {
	    printf(cantcreate,filexp(softname)) FLUSH;
	    return;
	}
	for (tmpng = 0; tmpng < nextrcline; tmpng++) {
	    fprintf(tmpfp,"%ld\n",(long)softptr[tmpng]);
	}
	fclose(tmpfp);
    }
}

void
get_old_rc()
{
    UNLINK(rctname);
#ifdef RENAME
    rename(rcname,rctname);
    rename(rcbname,rcname);
#else
    link(rcname,rctname);
    UNLINK(rcname);
    link(rcbname,rcname);
    UNLINK(rcbname);
#endif
}

static char *
grow(ptr, elem, size)
char *ptr;
int elem;
int size;
{
    if (ptr != NULL)
	return saferealloc(ptr, (MEM_SIZE)elem * size);
    else
	return safemalloc((MEM_SIZE)elem * size);
}

static void
grow_rc_arrays(newsize)
int newsize;
{
    register int i;

#ifdef CACHEFIRST
    abs1st = (ART_NUM *) grow((char*)abs1st, newsize, sizeof(ART_NUM));
#endif
    ngmax = (ART_NUM *) grow((char*)ngmax, newsize, sizeof(ART_NUM));
    rcline = (char **) grow((char*)rcline, newsize, sizeof(char*));
    toread = (ART_UNREAD *) grow((char*)toread, newsize, sizeof(ART_UNREAD));
    rcchar = (char *) grow(rcchar, newsize, sizeof(char));
    rcnums = (char *) grow(rcnums, newsize, sizeof(char));
    softptr = (ACT_POS *) grow((char*)softptr, newsize, sizeof(ACT_POS));

    for (i=maxrcline; i < newsize; i++) {
#ifdef CACHEFIRST
	abs1st[i] = 0;
#endif
	ngmax[i] = 0;
    }
    maxrcline = newsize;
    return;
}
