/* $Id: ng.c,v 1.3 92/01/11 16:29:36 usenet Exp $
 *
 * $Log:	ng.c,v $
 * Revision 1.3  92/01/11  16:29:36  usenet
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
 * Revision 1.2  92/01/11  16:05:56  usenet
 * header twiddling, bug fixes
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
#include "rn.h"
#include "term.h"
#include "final.h"
#include "util.h"
#include "artsrch.h"
#include "cheat.h"
#include "help.h"
#include "kfile.h"
#include "rcstuff.h"
#include "head.h"
#include "bits.h"
#include "art.h"
#include "artio.h"
#include "ngstuff.h"
#include "intrp.h"
#include "respond.h"
#include "ngdata.h"
#include "backpage.h"
#include "rcln.h"
#include "last.h"
#include "search.h"
#ifdef SERVER
#include "server.h"
#endif
#ifdef USETHREADS
#include "threads.h"
#include "rthreads.h"
#endif
#include "decode.h"
#include "INTERN.h"
#include "ng.h"
#include "artstate.h"			/* somebody has to do it */

/* art_switch() return values */

#define AS_NORM 0
#define AS_INP 1
#define AS_ASK 2
#define AS_CLEAN 3

ART_NUM recent_art = -1;	/* previous article # for '-' command */
ART_NUM curr_art = -1;		/* current article # */
int exit_code = NG_NORM;

void
ng_init()
{

#ifdef KILLFILES
    open_kfile(KF_GLOBAL);
#endif
#ifdef CUSTOMLINES
    init_compex(&hide_compex);
    init_compex(&page_compex);
#endif
}

/* do newsgroup on line ng with name ngname */

/* assumes that we are chdir'ed to SPOOL, and assures that that is
 * still true upon return, but chdirs to SPOOL/ngname in between
 *
 * If you can understand this routine, you understand most of the program.
 * The basic structure is:
 *	for each desired article
 *		for each desired page
 *			for each line on page
 *				if we need another line from file
 *					get it
 *					if it's a header line
 *						do special things
 *				for each column on page
 *					put out a character
 *				end loop
 *			end loop
 *		end loop
 *	end loop
 *
 *	(Actually, the pager is in another routine.)
 *
 * The chief problem is deciding what is meant by "desired".  Most of
 * the messiness of this routine is due to the fact that people want
 * to do unstructured things all the time.  I have used a few judicious
 * goto's where I thought it improved readability.  The rest of the messiness
 * arises from trying to be both space and time efficient.  Have fun.
 */

int
do_newsgroup(start_command)
char *start_command;			/* command to fake up first */
{
#ifdef SERVER
    char artname[MAXFILENAME];
    char intrpwork[MAXFILENAME];
    static long our_pid=0;
#endif /* SERVER */
    char oldmode = mode;
    register long i;			/* scratch */
    int skipstate;			/* how many unavailable articles */
					/*   have we skipped already? */
    
    char *whatnext = "%sWhat next? [%s]";

#ifdef SERVER
    if (our_pid == 0)           /* Agreed, this is gross */
        our_pid = getpid();
#endif /* SERVER */

#ifdef ARTSEARCH
    srchahead = (scanon && ((ART_NUM)toread[ng]) >= scanon ? -1 : 0);
					/* did they say -S? */
#endif
    
    mode = 'a';
#ifdef USETHREADS
    recent_p_art = curr_p_art = Nullart;
#endif
    recent_art = curr_art = -1;
    exit_code = NG_NORM;

#ifdef SERVER
    sprintf(ser_line, "GROUP %s", ngname);
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
    if (*ser_line != CHAR_OK) {
	if (atoi(ser_line) != ERR_NOGROUP){
		fprintf(stderr, "\nrrn: server response to GROUP %s:\n%s\n",
			ngname, ser_line);
		finalize(1);
	}
	return (-1);
    }
    {
	long first, last;

	(void) sscanf(ser_line,"%*d%*d%ld%ld",&first,&last);
	ngmax[ng] = last;	/* ensure getngsize() knows the new maximum */
# ifdef CACHEFIRST
	abs1st[ng] = first;
# endif
    }
#else /* not SERVER */
    if (eaccess(ngdir,5)) {		/* directory read protected? */
	if (eaccess(ngdir,0)) {
#ifdef VERBOSE
	    IF(verbose)
		printf("\nNewsgroup %s does not have a spool directory!\n",
		    ngname) FLUSH;
	    ELSE
#endif
#ifdef TERSE
		printf("\nNo spool for %s!\n",ngname) FLUSH;
#endif
#ifdef CATCHUP
	    catch_up(ng);
#else
	    toread[ng] = TR_NONE;
#endif
	}
	else {
#ifdef VERBOSE
	    IF(verbose)
		printf("\nNewsgroup %s is not currently accessible.\n",
		    ngname) FLUSH;
	    ELSE
#endif
#ifdef TERSE
		printf("\n%s not readable.\n",ngname) FLUSH;
#endif
	    toread[ng] = TR_NONE;	/* make this newsgroup invisible */
					/* (temporarily) */
	}
	mode = oldmode;
	return -1;
    }

    /* chdir to newsgroup subdirectory */

    if (chdir(ngdir)) {
	printf(nocd,ngdir) FLUSH;
	mode = oldmode;
	return -1;
    }
#endif /* SERVER */

#ifdef CACHESUBJ
    subj_list = Null(char **);		/* no subject list till needed */
#endif
    
    /* initialize control bitmap */

    if (initctl()) {
	mode = oldmode;
	return -1;
    }

    /* FROM HERE ON, RETURN THRU CLEANUP OR WE ARE SCREWED */

    in_ng = TRUE;			/* tell the world we are here */
    forcelast = TRUE;			/* if 0 unread, do not bomb out */

#ifdef USETHREADS
    if (use_threads)		/* grab thread data */
	ThreadedGroup = use_data(ThreadedGroup);
    added_articles = -1;
#endif

    /* remember what newsgroup we were in for sake of posterity */

    writelast();

    /* see if there are any special searches to do */

#ifdef KILLFILES
    open_kfile(KF_LOCAL);
#ifdef VERBOSE
    IF(verbose)
	kill_unwanted(firstart,"Looking for articles to kill...\n\n",TRUE);
    ELSE
#endif
#ifdef TERSE
	kill_unwanted(firstart,"Killing...\n\n",TRUE);
#endif
#endif
#ifdef USETHREADS
    first_art();
#else
    art=firstart;
#endif
    
    /* do they want a special top line? */

    firstline = getval("FIRSTLINE",Nullch);

    /* custom line suppression, custom page ending */

#ifdef CUSTOMLINES
    if (hideline = getval("HIDELINE",Nullch))
	compile(&hide_compex,hideline,TRUE,TRUE);
    if (pagestop = getval("PAGESTOP",Nullch))
	compile(&page_compex,pagestop,TRUE,TRUE);
#endif

    /* now read each unread article */

    rc_changed = doing_ng = TRUE;	/* enter the twilight zone */
    skipstate = 0;			/* we have not skipped anything (yet) */
    checkcount = 0;			/* do not checkpoint for a while */
    do_fseek = FALSE;			/* start 1st article at top */
    if (art > lastart)
#ifdef USETHREADS
	first_art();
#else
	art=firstart;			/* init the for loop below */
#endif
    for (; art<=lastart+1; ) {		/* for each article */

	/* do we need to "grow" the newsgroup? */

#ifdef USETHREADS
	if (ThreadedGroup) {
	    ART_NUM oldlast = lastart;

	    if ((art > lastart || forcegrow) && getngsize(ng) > total.last) {
		unuse_data(1);		/* free data with selections saved */
		ThreadedGroup = use_data(TRUE);	/* grows ctl & sets lastart */
		if (art > oldlast)
		    if (!forcelast)
			first_art();
		    else
			art = lastart+1;
		find_article(art);
		curr_p_art = p_art;
		forcegrow = FALSE;
	    }
	    if (lastart > oldlast && added_articles >= 0)
		added_articles = lastart - oldlast;
	}
	else
#endif
	if (art > lastart || forcegrow)
	    grow_ctl(getngsize(ng));
	check_first(art);		/* make sure firstart is still 1st */
	if (start_command) {		/* fake up an initial command? */
	    prompt = whatnext;
	    strcpy(buf,start_command);
	    free(start_command);
	    start_command = Nullch;
#ifdef USETHREADS
	    p_art = Nullart;
#endif
	    art = lastart+1;
	    goto article_level;
	}
	if (art>lastart) {		/* are we off the end still? */
	    ART_NUM ucount = 0;		/* count of unread articles left */

	    for (i=firstart; i<=lastart; i++)
		if (!(ctl_read(i)))
		    ucount++;		/* count the unread articles */
#ifdef DEBUGGING
	    /*NOSTRICT*/
	    if (debug && ((ART_NUM)toread[ng]) != ucount)
		printf("(toread=%ld sb %ld)",(long)toread[ng],(long)ucount)
		  FLUSH;
#endif
	    /*NOSTRICT*/
	    toread[ng] = (ART_UNREAD)ucount;	/* this is perhaps pointless */
	    art = lastart + 1;		/* keep bitmap references sane */
#ifdef USETHREADS
	    if (ThreadedGroup)
		ucount -= unthreaded;
	    if (!forcelast && selected_root_cnt && !selected_count && ucount) {
		if (art != curr_art) {
		    art = curr_art;
		    p_art = curr_p_art;
		}
		strcpy(buf, "+");
		goto article_level;
	    }
#endif
	    if (art != curr_art) {
#ifdef USETHREADS
		recent_p_art = curr_p_art;
		find_article(art);
		curr_p_art = p_art;
#endif
		recent_art = curr_art;
					/* remember last article # (for '-') */
		curr_art = art;      /* remember this article # */
	    }
	    if (erase_screen)
		clear();			/* clear the screen */
	    else
		fputs("\n\n",stdout) FLUSH;
#ifdef VERBOSE
	    IF(verbose)
		printf("End of newsgroup %s.",ngname);
					/* print pseudo-article */
	    ELSE
#endif
#ifdef TERSE
		printf("End of %s",ngname);
#endif
	    if (ucount) {
#ifdef USETHREADS
		if (selected_root_cnt)
		    printf("  (%ld + %ld articles still unread)",
			(long)selected_count,(long)ucount-selected_count);
		else
#endif
		    printf("  (%ld article%s still unread)",
			(long)ucount,ucount==1?nullstr:"s");
	    }
	    else {
#if defined(USETHREADS) && !defined(USETMPTHREAD)
		if (tobethreaded)
		    printf("  (%d article%s not yet threaded)",
			tobethreaded, tobethreaded == 1 ? nullstr : "s") FLUSH;
#endif
		if (!forcelast)
		    goto cleanup;	/* actually exit newsgroup */
	    }
	    prompt = whatnext;
#ifdef ARTSEARCH
	    srchahead = 0;		/* no more subject search mode */
#endif
	    fputs("\n\n",stdout) FLUSH;
	    skipstate = 0;		/* back to none skipped */
	}
	else if (!reread && was_read(art)) {
					/* has this article been read? */
#ifdef USETHREADS
	    follow_thread('n');
#else
	    art++;			/* then skip it */
#endif
	    continue;
	}
	else if
	  (!reread && !was_read(art)
#ifdef SERVER
	    && nntpopen(art,GET_HEADER) == Nullfp) { 
#else
	    && artopen(art) == Nullfp) { /* never read it, & cannot find it? */
	    if (errno != ENOENT) {	/* has it not been deleted? */
#ifdef VERBOSE
		IF(verbose)
		    printf("\n(Article %ld exists but is unreadable.)\n",
			(long)art) FLUSH;
		ELSE
#endif /* VERBOSE */
#ifdef TERSE
		    printf("\n(%ld unreadable.)\n",(long)art) FLUSH;
#endif /* TERSE */
		skipstate = 0;
		sleep(2);
	    }
#endif /* SERVER */
	    switch(skipstate++) {
	    case 0:
		clear();
#ifdef VERBOSE
		IF(verbose)
		    fputs("Skipping unavailable article",stdout);
		ELSE
#endif /* VERBOSE */
#ifdef TERSE
		    fputs("Skipping",stdout);
#endif /* TERSE */
		pad(just_a_sec/3);
		sleep(1);
		break;
	    case 1:
		fputs("..",stdout);
		fflush(stdout);
		break;
	    default:
		putchar('.');
		fflush(stdout);
#ifndef SERVER
#define READDIR
#ifdef READDIR
		{			/* fast skip patch */
		    ART_NUM newart;
		    
		    if (! (newart=getngmin(".",art)))
			newart = lastart+1;
		    for (i=art; i<newart; i++)
			oneless(i);
#ifndef USETHREADS
		    art = newart - 1;
#endif
		}
#endif /* READDIR */
#else
		{
			char	ser_line[NNTP_STRLEN];
			ART_NUM	newart;

			if (isfirstart) {
				sprintf(ser_line, "STAT %d",absfirst);
				put_server(ser_line);
				if (nntp_get(ser_line, sizeof(ser_line)) < 0) {
					fprintf(stderr, 
					"\nrrn: Unexpected close of server socket.\n");
					finalize(1);
				}
				newart=absfirst;
				isfirstart=FALSE;
			} 
			else {
				put_server("NEXT");
				if  (nntp_get(ser_line, sizeof(ser_line)) < 0) {
					fprintf(stderr,
					"\nrrn: unexpected close of server socket.\n");
					finalize(1);
				}
				if (ser_line[0] != CHAR_OK) {
					newart = lastart + 1;
				}
				else
					newart = atoi(ser_line+4);
			}				
			for (i=art; i<newart; i++)
				oneless(i);
#ifndef USETHREADS
			art = newart - 1;
#endif
		}
#endif /* SERVER */
		break;
	    }
	    oneless(art);		/* mark deleted as read */
#ifdef USETHREADS
	    count_roots(FALSE);		/* Keep selected_count accurate */
	    find_article(art);
	    follow_thread('n');
#else
	    art++;			/* try next article */
#endif
	    continue;
	}
	else {				/* we have a real live article */
	    skipstate = 0;		/* back to none skipped */
	    if (art != curr_art) {
#ifdef USETHREADS
		recent_p_art = curr_p_art;
		find_article(art);
		curr_p_art = p_art;
#endif
		recent_art = curr_art;
					/* remember last article # (for '-') */
		curr_art = art;      /* remember this article # */
	    }
	    if (!do_fseek) {		/* starting at top of article? */
		artline = 0;		/* start at the beginning */
		topline = -1;		/* and remember top line of screen */
					/*  (line # within article file) */
	    }
	    clear();			/* clear screen */
	    if (!artopen(art)) {	/* make sure article is found & open */
#ifdef USETHREADS
		char tmpbuf[256];
		/* see if we have tree data for this article anyway */
		init_tree();
		sprintf(tmpbuf,"%s #%ld is not available.",ngname,(long)art);
		tree_puts(tmpbuf,0,0);
		vwtary((ART_LINE)0,(ART_POS)0);
		finish_tree(1);
		prompt = whatnext;
#else
		printf("Article %ld of %s is not available.\n\n",
		    (long)art,ngname) FLUSH;
		prompt = whatnext;
#endif
#ifdef ARTSEARCH
		srchahead = 0;
#endif
	    }
	    else {			/* found it, so print it */
		switch (do_article()) {
		case DA_CLEAN:		/* quit newsgroup */
		    goto cleanup;
		case DA_TOEND:		/* do not mark as read */
		    goto reask_article; 
		case DA_RAISE:		/* reparse command at end of art */
		    goto article_level;
		case DA_NORM:		/* normal end of article */
		    break;
		}
	    }
	    if (art >= absfirst)	/* don't mark non-existant articles */
		mark_as_read();		/* mark current article as read */
	    do_hiding = TRUE;
#ifdef ROTATION
	    rotate = FALSE;
#endif
	}

/* if these gotos bother you, think of this as a little state machine */

reask_article:
#ifdef MAILCALL
	setmail();
#endif
	setdfltcmd();
#ifdef CLEAREOL
	if (erase_screen && can_home_clear)
	    clear_rest();
#endif /* CLEAREOL */
	unflush_output();		/* disable any ^O in effect */
	standout();			/* enter standout mode */
	printf(prompt,mailcall,dfltcmd);/* print prompt, whatever it is */
	un_standout();			/* leave standout mode */
	putchar(' ');
	fflush(stdout);
reinp_article:
	reread = FALSE;
	forcelast = FALSE;
	eat_typeahead();
#ifdef PENDING
	look_ahead();			/* see what we can do in advance */
	if (!input_pending())
	    collect_subjects();		/* loads subject cache until */
					/* input is pending */
#endif
	getcmd(buf);
	if (errno || *buf == '\f') {
	    if (LINES < 100 && !int_count)
		*buf = '\f';		/* on CONT fake up refresh */
	    else {
		putchar('\n') FLUSH;		/* but only on a crt */
		goto reask_article;
	    }
	}
article_level:
#ifdef USETHREADS
	output_chase_phrase = TRUE;
#endif

	/* parse and process article level command */

	switch (art_switch()) {
	case AS_INP:			/* multichar command rubbed out */
	    goto reinp_article;
	case AS_ASK:			/* reprompt "End of article..." */
	    goto reask_article;
	case AS_CLEAN:			/* exit newsgroup */
	    goto cleanup;
	case AS_NORM:			/* display article art */
	    break;
	}
    }					/* end of article selection loop */
    
/* shut down newsgroup */

cleanup:
    decode_end();
#ifdef KILLFILES
    kill_unwanted(firstart,"\nCleaning up...\n\n",FALSE);
					/* do cleanup from KILL file, if any */
#endif
#ifdef USETHREADS
    if (ThreadedGroup)
	unuse_data(0);			/* free article thread data */
#endif
    in_ng = FALSE;			/* leave newsgroup state */
    if (artfp != Nullfp) {		/* article still open? */
	fclose(artfp);			/* close it */
	artfp = Nullfp;			/* and tell the world */
#ifdef SERVER
        interp(intrpwork,MAXFILENAME-1, "%P");
        sprintf(artname, "%s/rrn%ld.%ld", intrpwork,(long) openart, our_pid);
        UNLINK(artname);
#endif /* SERVER */
	openart = 0;
    }
    putchar('\n') FLUSH;
#ifdef DELAYMARK
    yankback();				/* do a Y command */
#endif
    restore_ng();			/* reconstitute .newsrc line */
    doing_ng = FALSE;			/* tell sig_catcher to cool it */
    free(ctlarea);			/* return the control area */
#ifdef CACHESUBJ
    if (subj_list) {
	for (i=OFFSET(lastart); i>=0; --i)
	    if (subj_list[i])
		free(subj_list[i]);
#ifndef lint
	free((char*)subj_list);
#endif /* lint */
    }
#endif
    write_rc();				/* and update .newsrc */
    rc_changed = FALSE;			/* tell sig_catcher it is ok */
    if (chdir(spool)) {
	printf(nocd,spool) FLUSH;
	sig_catcher(0);
    }
#ifdef KILLFILES
    if (localkfp) {
	fclose(localkfp);
	localkfp = Nullfp;
    }
#endif
    mode = oldmode;
    return exit_code;
}					/* Whew! */

/* decide what to do at the end of an article */

int
art_switch()
{
    register ART_NUM i;
      
    setdef(buf,dfltcmd);
#ifdef VERIFY
    printcmd();
#endif

    switch (*buf) {
#ifdef USETHREADS
    case '<':			/* goto previous thread */
	if (!ThreadedGroup) {
	    goto group_unthreaded;
	}
	prev_root();
	return AS_NORM;
    case '>':			/* goto next thread */
	if (!ThreadedGroup) {
	    goto group_unthreaded;
	}
	next_root();
	return AS_NORM;
    case 'U': {			/* unread some articles */
	char *u_prompt, *u_help_thread;

	if (!ThreadedGroup) {
	    dfltcmd = "a";
	    u_help_thread = nullstr;
#ifdef VERBOSE
	    IF(verbose)
		u_prompt = "\nSet unread: all articles? [an] ";
	    ELSE
#endif
#ifdef TERSE
		u_prompt = "\nUnread? [an] ";
#endif
	}
	else if (!p_art || art > lastart) {
	    dfltcmd = "+";
	    u_help_thread = nullstr;
#ifdef VERBOSE
	    IF(verbose)
		u_prompt = "\nSet unread: +select or all? [+an] ";
	    ELSE
#endif
#ifdef TERSE
		u_prompt = "\nUnread? [+an] ";
#endif
	}
	else {
	    dfltcmd = "+";
#ifdef VERBOSE
	    IF(verbose) {
		u_prompt = "\n\
Set unread: +select, thread, subthread, or all? [+tsan] ";
		u_help_thread = "\
Type t or SP to mark this thread's articles as unread.\n\
Type s to mark the current article and its descendants as unread.\n";
	    }
	    ELSE
#endif
#ifdef TERSE
	    {
		u_prompt = "\nUnread? [ts+an] ";
		u_help_thread = "\
t or SP to mark thread unread.\n\
s to mark subthread unread.\n";
	    }
#endif
	}
      reask_unread:
	in_char(u_prompt,'u');
	setdef(buf,dfltcmd);
#ifdef VERIFY
	printcmd();
#endif
	putchar('\n') FLUSH;
	if (*buf == 'h') {
#ifdef VERBOSE
	    IF(verbose)
	    {
		if (ThreadedGroup)
		    fputs("\
Type + to enter select thread mode using all the unread articles.\n\
(The selected threads will be marked as unread and displayed as usual.)\n\
",stdout) FLUSH;
		fputs(u_help_thread,stdout);
		fputs("\
Type a to mark all articles in this group as unread.\n\
Type n to change nothing.\n\
",stdout) FLUSH;
	    }
	    ELSE
#endif
#ifdef TERSE
	    {
		if (ThreadedGroup)
		    fputs("\
+ to select threads from the unread.\n\
",stdout) FLUSH;
		fputs(u_help_thread,stdout);
		fputs("\
a to mark all articles unread.\n\
n to change nothing.\n\
",stdout) FLUSH;
	    }
#endif
	    goto reask_unread;
	}
	else if (*buf == 'n' || *buf == 'q') {
	    return AS_ASK;
	}
	else if (*buf == 't' && u_help_thread != nullstr)
	    follow_thread('u');
	else if (*buf == 's' && u_help_thread != nullstr)
	    follow_thread('U');
	else if (*buf == 'a') {
	    check_first(absfirst);
	    for (i = absfirst; i <= lastart; i++) {
		onemore(i);		/* mark as unread */
	    }
	    scan_all_roots = FALSE;
	    count_roots(FALSE);
	    if (art > lastart) {
		first_art();
	    }
	}
	else if (ThreadedGroup && *buf == '+') {
	    *buf = 'U';
	    goto select_threads;
	}
	else {
	    fputs(hforhelp,stdout) FLUSH;
	    settle_down();
	    goto reask_unread;
	}
	return AS_NORM;
    }
    case '[':			/* goto parent article */
    case '{':			/* goto thread's root article */
	if (p_art) {
	    if (!p_art->parent) {
		if (p_art == p_articles + p_roots[p_art->root].articles) {
		    register char *cp = (*buf=='['?"parent":"root");
#ifdef VERBOSE
		    IF(verbose)
			fprintf(stdout,"\n\
There is no %s article prior to this one.\n",cp) FLUSH;
		    ELSE
#endif
#ifdef TERSE
			fprintf(stdout,"\nNo prior %s.\n",cp) FLUSH;
#endif
		    return AS_ASK;
		}
		*buf = '{';
		p_art--;
	    }
	    else
		p_art += p_art->parent;

	    if (*buf == '{')
		while (p_art->parent)
		    p_art += p_art->parent;

	    art = p_art->num;
	    reread = TRUE;
	    return AS_NORM;
	}
not_threaded:
	if (ThreadedGroup) {
#ifdef VERBOSE
	    IF(verbose)
		fputs("\nThis article is not threaded.\n",stdout) FLUSH;
	    ELSE
#endif
#ifdef TERSE
		fputs("\nUnthreaded article.\n",stdout) FLUSH;
#endif
	    return AS_ASK;
	}
group_unthreaded:
#ifdef VERBOSE
	IF(verbose)
	    fputs("\nThis group is not threaded.\n",stdout) FLUSH;
	ELSE
#endif
#ifdef TERSE
	    fputs("\nUnthreaded group.\n",stdout) FLUSH;
#endif
	return AS_ASK;
    case ']':			/* goto child article */
    case '}':			/* goto thread's leaf article */
	if (p_art) {
	    if (!(p_art++)->child_cnt) {
		PACKED_ARTICLE *root_limit = upper_limit(p_art-1,FALSE);

		if (p_art == root_limit) {
#ifdef VERBOSE
		    IF(verbose)
			fputs("\n\
This is the last leaf in this tree.\n",stdout) FLUSH;
		    ELSE
#endif
#ifdef TERSE
			fputs("\nLast leaf.\n",stdout) FLUSH;
#endif
		    p_art--;
		    return AS_ASK;
		}
		if (*buf == ']')
		    *buf = '}';
		else {
		    while (++p_art != root_limit && p_art->parent)
			;
		    p_art--;
		    *buf = ' ';
		}
	    }
	    if (*buf == '}')
		while (p_art->child_cnt)
		    p_art++;

	    art = p_art->num;
	    reread = TRUE;
	    return AS_NORM;
	}
	goto not_threaded;
    case 'T':
	if (p_art) {
	    sprintf(buf,"T%ld\t# %s",(long)p_roots[p_art->root].root_num,
		subject_ptrs[p_art->subject]);
	    fputs(buf,stdout);
	    kf_append(buf);
	    follow_thread('J');
	    return AS_NORM;
	}
	goto not_threaded;
    case 'K':
	if (p_art) {
	    /* first, write kill-subject command */
	    (void)art_search(buf, (sizeof buf), TRUE);
	    art = curr_art;
	    p_art = curr_p_art;
	    follow_thread('k');		/* then take care of any prior subjs */
	    return AS_NORM;
	}
	goto normal_search;
    case ',':		/* kill this node and all descendants */
	mark_as_read();
	*buf = 'K';
    case 'k':		/* kill current subject # (e.g. [1]) */
    case 'J':		/* Junk all nodes in this thread */
	if (ThreadedGroup) {
	    follow_thread(*buf);
	    return AS_NORM;
	}
	*buf = 'k';
	goto normal_search;
    case 't':
	carriage_return();
#ifndef CLEAREOL
	erase_eol();		/* erase the prompt */
#else
	if (erase_screen && can_home_clear)
	    clear_rest();
	else
	    erase_eol();	/* erase the prompt */
#endif /* CLEAREOL */
	fflush(stdout);
	page_line = 1;
	p_art = curr_p_art;
	entire_tree();
	return AS_ASK;
    case ':':			/* execute command on selected articles */
	if (!ThreadedGroup) {
	    goto group_unthreaded;
	}
	page_line = 1;
	if (!use_selected())
	    return AS_INP;
	putchar('\n');
	art = curr_art;
	p_art = curr_p_art;
	return AS_ASK;
#endif /* USETHREADS */
    case 'p':			/* find previous unread article */
#ifdef USETHREADS
	if (ThreadedGroup) {
	    goto backtrack_threads;
	}
#endif
	do {
	    if (art <= firstart)
		break;
	    art--;
#ifdef SERVER
	} while (was_read(art) || nntpopen(art,GET_HEADER) == Nullfp);
#else
	} while (was_read(art) || artopen(art) == Nullfp);
#endif
#ifdef ARTSEARCH
	srchahead = 0;
#endif
	return AS_NORM;
    case 'P':		/* goto previous article */
#ifdef USETHREADS
	if (ThreadedGroup) {
backtrack_threads:
	    backtrack_thread(*buf);
	    art++;		/* prepare for art-- below */
	}
#endif
	if (art > absfirst)
	    art--;
	else {
#ifdef VERBOSE
	    IF(verbose)
		fprintf(stdout,"\n\
There are no%s articles prior to this one.\n\
",*buf=='P'?nullstr:" unread") FLUSH;
	    ELSE
#endif
#ifdef TERSE
		fprintf(stdout,"\n\
No previous%s articles\n\
",*buf=='P'?nullstr:" unread") FLUSH;
#endif
	    art = curr_art;
#ifdef USETHREADS
	    p_art = curr_p_art;
#endif
	    return AS_ASK;
	}
	reread = TRUE;
#ifdef ARTSEARCH
	srchahead = 0;
#endif
	return AS_NORM;
    case '-':
	if (recent_art >= 0) {
#ifdef USETHREADS
	    p_art = recent_p_art;
#endif
	    art = recent_art;
	    reread = TRUE;
	    forcelast = TRUE;
#ifdef ARTSEARCH
	    srchahead = -(srchahead != 0);
#endif
	    return AS_NORM;
	}
	else {
	    exit_code = NG_MINUS;
	    return AS_CLEAN;
	}
    case 'n':		/* find next unread article? */
#ifdef USETHREADS
	if (ThreadedGroup) {
	    follow_thread(*buf);
	    return AS_NORM;
	}
#endif
	if (art > lastart) {
	    if (!toread[ng])
		return AS_CLEAN;
	    art = firstart;
	}
#ifdef ARTSEARCH
	else if (scanon && srchahead) {
	    *buf = Ctl('n');
	    goto normal_search;
	}
#endif
	else
	    art++;

#ifdef ARTSEARCH
	srchahead = 0;
#endif
	return AS_NORM;
    case 'N':			/* goto next article */
#ifdef USETHREADS
	if (ThreadedGroup) {
	    follow_thread(*buf);
	    return AS_NORM;
	}
#endif
	if (art > lastart)
	    art = absfirst;
	else
	    art++;
	if (art <= lastart)
	    reread = TRUE;
#ifdef ARTSEARCH
	srchahead = 0;
#endif
	return AS_NORM;
    case '$':
	art = lastart+1;
	forcelast = TRUE;
#ifdef USETHREADS
	p_art = Nullart;
#endif
#ifdef ARTSEARCH
	srchahead = 0;
#endif
	return AS_NORM;
    case '1': case '2': case '3':	/* goto specified article */
    case '4': case '5': case '6':	/* or do something with a range */
    case '7': case '8': case '9': case '.':
	forcelast = TRUE;
	switch (numnum()) {
	case NN_INP:
	    return AS_INP;
	case NN_ASK:
	    return AS_ASK;
	case NN_REREAD:
	    reread = TRUE;
#ifdef ARTSEARCH
	    if (srchahead)
		srchahead = -1;
#endif
	    break;
	case NN_NORM:
	    if (was_read(art)) {
#ifdef USETHREADS
		first_art();
#else
		art = firstart;
#endif
		pad(just_a_sec/3);
	    }
	    else {
		putchar('\n');
		return AS_ASK;
	    }
	    break;
	}
	return AS_NORM;
    case Ctl('k'):
	edit_kfile();
	return AS_ASK;
#ifndef USETHREADS
    case 'K':
    case 'k':
#endif
    case Ctl('n'):	/* search for next article with same subject */
#ifdef USETHREADS
	if (ThreadedGroup) {
	    follow_thread(*buf);
	    return AS_NORM;
	}
#endif
    case Ctl('p'):	/* search for previous article with same subject */
#ifdef USETHREADS
	if (ThreadedGroup) {
	    goto backtrack_threads;
	}
#endif
    case '/': case '?':
normal_search:
#ifdef ARTSEARCH
    {		/* search for article by pattern */
	char cmd = *buf;
	
	reread = TRUE;		/* assume this */
	page_line = 1;
	switch (art_search(buf, (sizeof buf), TRUE)) {
	case SRCH_ERROR:
	    art = curr_art;
	    return AS_ASK;
	case SRCH_ABORT:
	    art = curr_art;
	    return AS_INP;
	case SRCH_INTR:
#ifdef VERBOSE
	    IF(verbose)
		printf("\n(Interrupted at article %ld)\n",(long)art) FLUSH;
	    ELSE
#endif
#ifdef TERSE
		printf("\n(Intr at %ld)\n",(long)art) FLUSH;
#endif
	    art = curr_art;
			    /* restore to current article */
	    return AS_ASK;
	case SRCH_DONE:
	    fputs("done\n",stdout) FLUSH;
	    pad(just_a_sec/3);	/* 1/3 second */
	    if (!srchahead) {
		art = curr_art;
		return AS_ASK;
	    }
#ifdef USETHREADS
	    first_art();
#else
	    art = firstart;
#endif
	    reread = FALSE;
	    return AS_NORM;
	case SRCH_SUBJDONE:
#ifdef UNDEF
	    fputs("\n\n\n\nSubject not found.\n",stdout) FLUSH;
	    pad(just_a_sec/3);	/* 1/3 second */
#endif
#ifdef USETHREADS
	    first_art();
#else
	    art = firstart;
#endif
	    reread = FALSE;
	    return AS_NORM;
	case SRCH_NOTFOUND:
	    fputs("\n\n\n\nNot found.\n",stdout) FLUSH;
	    art = curr_art;  /* restore to current article */
	    return AS_ASK;
	case SRCH_FOUND:
	    if (cmd == Ctl('n') || cmd == Ctl('p'))
		oldsubject = TRUE;
	    break;
	}
	return AS_NORM;
    }
#else
    buf[1] = '\0';
    notincl(buf);
    return AS_ASK;
#endif
    case 'u':			/* unsubscribe from this newsgroup? */
	rcchar[ng] = NEGCHAR;
	return AS_CLEAN;
    case 'M':
#ifdef DELAYMARK
	if (art <= lastart) {
	    delay_unmark(art);
	    printf("\nArticle %ld will return.\n",(long)art) FLUSH;
	}
#else
	notincl("M");
#endif
	return AS_ASK;
    case 'm':
	if (art <= lastart) {
	    unmark_as_read();
	    printf("\nArticle %ld marked as still unread.\n",(long)art) FLUSH;
	}
	return AS_ASK;
    case 'c':			/* catch up */
      reask_catchup:
#ifdef VERBOSE
	IF(verbose)
	    in_char("\nDo you really want to mark everything as read? [yn] ",
		'C');
	ELSE
#endif
#ifdef TERSE
	    in_char("\nReally? [ynh] ", 'C');
#endif
	setdef(buf,"y");
#ifdef VERIFY
	printcmd();
#endif
	putchar('\n') FLUSH;
	if (*buf == 'h') {
#ifdef VERBOSE
	    IF(verbose)
		fputs("\
Type y or SP to mark all articles as read.\n\
Type n to leave articles marked as they are.\n\
Type u to mark everything read and unsubscribe.\n\
",stdout) FLUSH;
	    ELSE
#endif
#ifdef TERSE
		fputs("\
y or SP to mark all read.\n\
n to forget it.\n\
u to mark all and unsubscribe.\n\
",stdout) FLUSH;
#endif
	    goto reask_catchup;
	}
	else if (*buf == 'n' || *buf == 'q') {
	    return AS_ASK;
	}
	else if (*buf != 'y' && *buf != 'u') {
	    fputs(hforhelp,stdout) FLUSH;
	    settle_down();
	    goto reask_catchup;
	}
	for (i = firstart; i <= lastart; i++) {
	    oneless(i);		/* mark as read */
	}
#ifdef USETHREADS
	selected_count = selected_root_cnt = unthreaded = 0;
#endif
#ifdef DELAYMARK
	if (dmfp)
	    yankback();
#endif
	if (*buf == 'u') {
	    rcchar[ng] = NEGCHAR;
	    return AS_CLEAN;
	}
#ifdef USETHREADS
	p_art = Nullart;
#endif
	art = lastart+1;
	forcelast = FALSE;
	return AS_NORM;
    case 'Q':
	exit_code = NG_ASK;
	/* FALL THROUGH */
    case 'q':			/* go back up to newsgroup level? */
	return AS_CLEAN;
    case 'j':
	putchar('\n') FLUSH;
	if (art <= lastart)
	    mark_as_read();
	return AS_ASK;
    case 'h': {			/* help? */
	int cmd;

	if ((cmd = help_art()) > 0)
	    pushchar(cmd);
	return AS_ASK;
    }
    case '&':
	if (switcheroo()) /* get rest of command */
	    return AS_INP;	/* if rubbed out, try something else */
	return AS_ASK;
    case '#':
#ifdef VERBOSE
	IF(verbose)
	    printf("\nThe last article is %ld.\n",(long)lastart) FLUSH;
	ELSE
#endif
#ifdef TERSE
	    printf("\n%ld\n",(long)lastart) FLUSH;
#endif
	return AS_ASK;
#ifdef USETHREADS
    case '+':			/* enter thread selection mode */
	if (ThreadedGroup) {
select_threads:
	    *buf = select_thread(*buf);
	    switch (*buf) {
	    case '+':
	    case '\033':
		putchar('\n') FLUSH;
		return AS_ASK;
	    case 'Q':
		exit_code = NG_ASK;
		/* FALL THROUGH */
	    case 'q':
		break;
	    case 'N':
		exit_code = NG_SELNEXT;
		break;
	    case 'P':
		exit_code = NG_SELPRIOR;
		break;
	    default:
		if (toread[ng])
		    return AS_NORM;
		break;
	    }
	    return AS_CLEAN;
	}
	/* FALL THROUGH */
#endif
    case '=': {			/* list subjects */
	char tmpbuf[256];
	ART_NUM oldart = art;
	int cmd;
	char *subjline = getval("SUBJLINE",Nullch);
#ifndef CACHESUBJ
	char *s;
#endif

	page_init();
#ifdef CACHESUBJ
	if (!subj_list)
	    fetchsubj(art,TRUE,FALSE);
#endif
	for (i=firstart; i<=lastart && !int_count; i++) {
#ifdef CACHESUBJ
	    if (!was_read(i) &&
	      (subj_list[OFFSET(i)] != Nullch || fetchsubj(i,FALSE,FALSE)) &&
	      *subj_list[OFFSET(i)] ) {
		sprintf(tmpbuf,"%5ld ", i);
		if (subjline) {
		    art = i;
		    interp(tmpbuf + 6, (sizeof tmpbuf) - 6, subjline);
		}
		else
		    safecpy(tmpbuf + 6, subj_list[OFFSET(i)],
			(sizeof tmpbuf) - 6);
		if (cmd = print_lines(tmpbuf,NOMARKING)) {
		    if (cmd > 0)
			pushchar(cmd);
		    break;
		}
	    }
#else
	    if (!was_read(i) && (s = fetchsubj(i,FALSE,FALSE)) && *s) {
		sprintf(tmpbuf,"%5ld ", i);
		if (subjline) {	/* probably fetches it again! */
		    art = i;
		    interp(tmpbuf + 6, (sizeof tmpbuf) - 6, subjline);
		}
		else
		    safecpy(tmpbuf + 6, s, (sizeof tmpbuf) - 6);
		if (cmd = print_lines(tmpbuf,NOMARKING)) {
		    if (cmd > 0)
			pushchar(cmd);
		    break;
		}
	    }
#endif
	}
	int_count = 0;
	art = oldart;
	return AS_ASK;
    }
    case '^':
#ifdef USETHREADS
	first_art();
#else
	art = firstart;
#endif
#ifdef ARTSEARCH
	srchahead = 0;
#endif
	return AS_NORM;
#if defined(CACHESUBJ) && defined(DEBUGGING)
    case 'D':
	printf("\nFirst article: %ld\n",(long)firstart) FLUSH;
	if (!subj_list)
	    fetchsubj(art,TRUE,FALSE);
	if (subj_list != Null(char **)) {
	    for (i=1; i<=lastart && !int_count; i++) {
		if (subj_list[OFFSET(i)])
		    printf("%5ld %c %s\n",
			i, (was_read(i)?'y':'n'), subj_list[OFFSET(i)]) FLUSH;
	    }
	}
	int_count = 0;
	return AS_ASK;
#endif
    case 'v':
	if (art <= lastart) {
	    reread = TRUE;
	    do_hiding = FALSE;
	}
	return AS_NORM;
#ifdef ROTATION
    case Ctl('x'):
#endif
    case Ctl('r'):
#ifdef ROTATION
	rotate = (*buf==Ctl('x'));
#endif
	if (art <= lastart)
	    reread = TRUE;
	else
	    forcelast = TRUE;
	return AS_NORM;
#ifdef ROTATION
    case 'X':
	rotate = !rotate;
	/* FALL THROUGH */
#else
    case Ctl('x'):
    case 'x':
    case 'X':
	notincl("x");
	return AS_ASK;
#endif
    case 'l': case Ctl('l'):		/* refresh screen */
	if (art <= lastart) {
	    reread = TRUE;
	    clear();
	    do_fseek = TRUE;
	    artline = topline;
	    if (artline < 0)
		artline = 0;
	}
	return AS_NORM;
    case 'b': case Ctl('b'):		/* back up a page */
	if (art <= lastart) {
	    ART_LINE target;

	    reread = TRUE;
	    clear();
	    do_fseek = TRUE;
	    target = topline - (LINES - 2);
	    artline = topline;
	    if (artline >= 0) do {
		artline--;
	    } while(artline >= 0 && artline > target && vrdary(artline-1) >= 0);
	    topline = artline;
	    if (artline < 0)
		artline = 0;
	}
	return AS_NORM;
    case '!':			/* shell escape */
	if (escapade())
	    return AS_INP;
	return AS_ASK;
    case 'C': {
	cancel_article();
	return AS_ASK;
    }
    case 'z': {
	supersede_article();	/* supersedes */
	return AS_ASK;
    }
    case 'R':
    case 'r': {			/* reply? */
	reply();
	return AS_ASK;
    }
    case 'F':
    case 'f': {			/* followup command */
	followup();
	forcegrow = TRUE;		/* recalculate lastart */
	return AS_ASK;
    }
    case '|':
    case 'w': case 'W':
    case 's': case 'S':		/* save command */
    case 'e':			/* extract command */
	if (save_article() == SAVE_ABORT)
	    return AS_INP;
	int_count = 0;
	return AS_ASK;
    case 'E':
	if (decode_fp)
	    decode_end();
	else
	    putchar('\n') FLUSH;
	return AS_ASK;
#ifdef DELAYMARK
    case 'Y':				/* yank back M articles */
	yankback();
#ifdef USETHREADS
	first_art();
#else
	art = firstart;			/* from the beginning */
#endif
	return AS_NORM;			/* pretend nothing happened */
#endif
#ifdef STRICTCR
    case '\n':
	fputs(badcr,stdout) FLUSH;
	return AS_ASK;
#endif
    default:
	printf("\n%s",hforhelp) FLUSH;
	settle_down();
	return AS_ASK;
    }
}

#ifdef MAILCALL
/* see if there is any mail */

void
setmail()
{
    if (! (mailcount++)) {
	char *mailfile = filexp(getval("MAILFILE",MAILFILE));
	
	if (stat(mailfile,&filestat) < 0 || !filestat.st_size
	    || filestat.st_atime > filestat.st_mtime)
	    mailcall = nullstr;
	else
	    mailcall = getval("MAILCALL","(Mail) ");
    }
    mailcount %= 10;			/* check every 10 articles */
}
#endif

void
setdfltcmd()
{
#ifdef USETHREADS
    if (toread[ng] == unthreaded) {
#else
    if (!toread[ng]) {
#endif
	if (art > lastart)
	    dfltcmd = "qnp";
	else
	    dfltcmd = "npq";
    }
    else {
#ifdef ARTSEARCH
# ifdef USETHREADS
	if (!ThreadedGroup && srchahead)
# else
	if (srchahead)
# endif
	    dfltcmd = "^Nnpq";
	else
#endif
	    dfltcmd = "npq";
    }
}
