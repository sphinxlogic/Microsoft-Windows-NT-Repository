/*  trn -- threaded readnews program
 *
 *  Author/Maintainer of trn: davison@borland.com (Wayne Davison)
 *  Organization: Borland International
 *  Author/Maintainer of rn: sob@bcm.tmc.edu (Stan Barber)
 *  Organization: Baylor College of Medicine, Houston,Tx
 *  Original Author: lwall@sdcrdcf.UUCP (Larry Wall)
 *  Organization: System Development Corporation, Santa Monica
 *
 *  begun:   01/14/83
 *	1.0: 04/08/83
 *      2.0: 09/01/83
 *      RRN/RN: 11/01/89
 *      4.4  07/04/91
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

static char rnid[] = "@(#)$Id: rn.c,v 1.3 92/01/11 16:30:26 usenet Exp $";
static char patchlevel[] = "Trn 2.2 based on rn 4.4 pl 2";

/* $Log:	rn.c,v $
 * Revision 1.3  92/01/11  16:30:26  usenet
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
 * Revision 1.2  92/01/11  16:06:50  usenet
 * header twiddling, bug fixes
 * 
 * Revision 4.4.2.1  1991/12/01  18:05:42  sob
 * Patchlevel 2 changes
 *
 * Revision 4.4.1.1  1991/09/25  19:38:08  sob
 * Updated patchlevel message
 *
 * Revision 4.4  1991/09/09  20:27:37  sob
 * release 4.4
 *
 *
 *
 * 
 */

#include "INTERN.h"
#include "common.h"
#include "rn.h"
#ifdef SERVER
#include "server.h"
#endif
#include "EXTERN.h"
#include "rcstuff.h"
#include "term.h"
#include "final.h"
#include "search.h"
#include "ngdata.h"
#include "util.h"
#include "only.h"
#include "ngsrch.h"
#include "help.h"
#include "last.h"
#include "init.h"
#include "intrp.h"
#include "rcln.h"
#include "sw.h"
#include "addng.h"
#include "ng.h"

void
rn_init()
{
    ;
}

void
main(argc,argv)
int argc;
char *argv[];
{
    bool foundany;
    register char *s;
    bool oh_for_the_good_old_days = FALSE;
    int direction = 1;

#if defined(USETHREADS) && !THREAD_INIT
    /* Default to threaded operation if our name starts with a 't' */
    if ((s = rindex(argv[0],'/')) == Nullch)
	s = argv[0];
    else
	s++;
    if (*s == 't')
	use_threads = TRUE;
#endif
    foundany = initialize(argc,argv);

    if (maxngtodo)
	starthere = 0;
    else if (!foundany) {		/* nothing to do? */
#ifdef VERBOSE
	if (verbose)
	    fputs("\
No unread news in subscribed-to newsgroups.  To subscribe to a new\n\
newsgroup use the g<newsgroup> command.\n\
",stdout) FLUSH;
#endif
	starthere = nextrcline;
    }

    /* loop through all unread news */

    {
	bool special = FALSE;		/* temporarily allow newsgroup */
					/*   with no unread news? */
	bool retry;			/* cycle back to top of list? */
	NG_NUM recent_ng = 0;
	
	current_ng = 0;
	do {
	    retry = FALSE;
	    if (findlast) {
		findlast = FALSE;
		starthere = 0;
		if (*lastngname) {
		    if ((ng = find_ng(lastngname)) == nextrcline)
			ng = 0;
		    else {
			set_ngname(lastngname);
		    	set_toread(ng);
			if (toread[ng] <= TR_NONE)
			    ng = 0;
		    }
		}
	    }
	    else {
		ng = starthere;
		starthere = 0;
	    }
	    while (ng <= nextrcline) {	/* for each newsgroup */
		mode = 'n';
		if (ng >= nextrcline) {	/* after the last newsgroup? */
		    ng = nextrcline;	/* force it to 1 after */
#ifdef ONLY
		    if (maxngtodo) {
			if (retry)
#ifdef VERBOSE
			    IF(verbose)
				printf("\nRestriction %s%s still in effect.\n",
				    ngtodo[0],
				    maxngtodo > 1 ? ", etc." : nullstr) FLUSH;
			    ELSE
#endif
#ifdef TERSE
				fputs("\n(\"Only\" mode.)\n",stdout) FLUSH;
#endif
			else {
#ifdef VERBOSE
			    IF(verbose)
				fputs("\nNo articles under restriction.",
				  stdout) FLUSH;
			    ELSE
#endif
#ifdef TERSE
				fputs("\nNo \"only\" articles.",stdout) FLUSH;
#endif
			    end_only();	/* release the restriction */
			    retry = TRUE;
			}
		    }
#endif
		}
		else {
		    bool shoe_fits;	/* newsgroup matches restriction? */

		    if (toread[ng] >= TR_NONE) {	/* recalc toread? */
			set_ngname(rcline[ng]);
			shoe_fits = inlist(ngname);
			if (shoe_fits)
			    set_toread(ng);
			if (paranoid) {
			    recent_ng = current_ng;
			    current_ng = ng;
			    cleanup_rc();
					/* this may move newsgroups around */
			    ng = current_ng;
			    set_ngname(rcline[ng]);
			}
		    }
		    if (toread[ng] < (maxngtodo||special ? TR_NONE : TR_ONE)
		     || !shoe_fits) {		/* unwanted newsgroup? */
			ng += direction;	/* then skip it */
			if (ng < 0) {
			   ng = 1;
			   direction = 1;
			}
			continue;
		    }
		}
		special = FALSE;	/* go back to normal mode */
		if (ng != current_ng) {
		    recent_ng = current_ng;
					/* remember previous newsgroup */
		    current_ng = ng;	/* remember current newsgroup */
		}
    reask_newsgroup:
		unflush_output();	/* disable any ^O in effect */
		if (ng >= nextrcline) {
#ifdef SERVER
		    if (time(Null(time_t*)) - lastactfetch > MINFETCHTIME) {
			fclose(actfp);
			ngdata_init();	/* re-grab the active file */
		    }
#endif
		    dfltcmd = (retry ? "npq" : "qnp");
#ifdef VERBOSE
		    IF(verbose)
			printf("\n******** End of newsgroups--what next? [%s] ",
			    dfltcmd);
		    ELSE
#endif
#ifdef TERSE
			printf("\n**** End--next? [%s] ", dfltcmd);
#endif
		} else {
#ifdef USETHREADS
		    ThreadedGroup = (use_threads
			&& (thread_always || rcchar[ng] == '0'));
		    dfltcmd = (use_threads && select_on
			&& (ART_NUM)toread[ng] >= select_on ? "+ynq" : "ynq");
#else
		    dfltcmd = "ynq";
#endif
#ifdef VERBOSE
		    IF(verbose)
			printf("\n******** %3ld unread article%s in %s--read now? [%s] ",
			    (long)toread[ng], (toread[ng]==TR_ONE ? nullstr : "s"),
			    ngname, dfltcmd);
		    ELSE
#endif
#ifdef TERSE
			printf("\n**** %3ld in %s--read? [%s] ",
			    (long)toread[ng],
			    ngname,dfltcmd);
#endif
		}
		fflush(stdout);
    reinp_newsgroup:
		eat_typeahead();
		getcmd(buf);
		if (errno || *buf == '\f') {
		    putchar('\n') FLUSH; /* if return from stop signal */
		    goto reask_newsgroup;	/* give them a prompt again */
		}
		setdef(buf,dfltcmd);
#ifdef VERIFY
		printcmd();
#endif
    do_command:
		direction = 1;		/* default to forward motion */
		switch (*buf) {
		case 'P':		/* goto previous newsgroup */
		    special = TRUE;	/* don't skip it if toread==0 */
		    /* FALL THROUGH */
		case 'p':		/* find previous unread newsgroup */
		    if (ng > 0)
			ng--;
		    direction = -1;	/* go backward in the newsrc */
		    break;
		case '-':
		    ng = recent_ng;	/* recall previous newsgroup */
		    if (ng < nextrcline)
			if (!get_ng(rcline[ng],FALSE))
			    ng = current_ng;
		    special = TRUE;	/* don't skip it if toread==0 */
		    break;
		case 'q': case 'Q': case 'x':	/* quit? */
		    oh_for_the_good_old_days = (*buf == 'x');
		    putchar('\n') FLUSH;
		    ng = nextrcline+1;	/* satisfy */
		    retry = FALSE;	/*   loop conditions */
		    break;
		case '^':
		    putchar('\n') FLUSH;
		    ng = 0;
		    break;
		case 'n':		/* find next unread newsgroup */
		    if (ng == nextrcline) {
			putchar('\n') FLUSH;
			retry = TRUE;
		    }
		    else if (toread[ng] > TR_NONE)
			retry = TRUE;
		    ng++;
		    break;
		case 'N':		/* goto next newsgroup */
		    special = TRUE;	/* and don't skip it if toread==0 */
		    ng++;
		    break;
		case '1':		/* goto 1st newsgroup */
		    ng = 0;
		    special = TRUE;	/* and don't skip it if toread==0 */
		    break;
		case '$':
		    ng = nextrcline;	/* goto last newsgroup */
		    retry = TRUE;
		    break;
		case 'L':
		    list_newsgroups();
		    goto reask_newsgroup;
		case '/': case '?':	/* scan for newsgroup pattern */
#ifdef NGSEARCH
		    switch (ng_search(buf,TRUE)) {
		    case NGS_ERROR:
			goto reask_newsgroup;
		    case NGS_ABORT:
			goto reinp_newsgroup;
		    case NGS_INTR:
#ifdef VERBOSE
			IF(verbose)
			    fputs("\n(Interrupted)\n",stdout) FLUSH;
			ELSE
#endif
#ifdef TERSE
			    fputs("\n(Intr)\n",stdout) FLUSH;
#endif
			ng = current_ng;
			goto reask_newsgroup;
		    case NGS_FOUND:
			special = TRUE;	/* don't skip it if toread==0 */
			break;
		    case NGS_NOTFOUND:
#ifdef VERBOSE
			IF(verbose)
			    fputs("\n\nNot found--use a or g to add newsgroups\n",
				stdout) FLUSH;
			ELSE
#endif
#ifdef TERSE
			    fputs("\n\nNot found\n",stdout) FLUSH;
#endif
			goto reask_newsgroup;
		    }
#else
		    notincl("/");
#endif
		    break;
		case 'm':
#ifndef RELOCATE
		    notincl("m");
		    break;
#endif		    
		case 'g':	/* goto named newsgroup */
		    if (!finish_command(FALSE))
					/* if they didn't finish command */
			goto reinp_newsgroup;	/* go try something else */
		    for (s = buf+1; *s == ' '; s++);
					/* skip leading spaces */
#ifdef RELOCATE
		    if (!*s && *buf == 'm' && ngname && ng < nextrcline)
			strcpy(s,ngname);
#endif
		    if (isalpha(*s)) 
			set_ngname(s);
		    else {
			if (isdigit(*s)) {
			    int rcnum;
			    rcnum = atoi(s);
			    if (rcnum < nextrcline)
				set_ngname(rcline[rcnum]);
			    else {
				printf("\nOnly %d groups. Try again.\n",
					nextrcline) FLUSH;
				goto reask_newsgroup;
			    }
			}
			else {
			    printf("\nPlease specify a newsgroup.\n") FLUSH;
			    goto reask_newsgroup;
			}
		    }
#ifdef RELOCATE
		    if (!get_ng(ngname,*buf=='m'))
						/* try to find newsgroup */
#else
		    if (!get_ng(ngname,FALSE))	/* try to find newsgroup */
#endif
			ng = current_ng;/* if not found, go nowhere */
		    special = TRUE;	/* don't skip it if toread==0 */
		    break;
#ifdef DEBUGGING
		case 'D':
		    printf("\nTries: %d Hits: %d\n",
			softtries,softtries-softmisses) FLUSH;
		    goto reask_newsgroup;
#endif
		case '!':		/* shell escape */
		    if (escapade())	 /* do command */
			goto reinp_newsgroup;
					/* if rubbed out, re input */
		    goto reask_newsgroup;
		case Ctl('k'):		/* edit global KILL file */
		    edit_kfile();
		    goto reask_newsgroup;
		case 'c':		/* catch up */
#ifdef CATCHUP
reask_catchup:
#ifdef VERBOSE
		    IF(verbose)
			in_char("\nDo you really want to mark everything as read? [yn] ", 'C');
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
			printf("Type y or SP to mark all articles as read.\n");
			printf("Type n to leave articles marked as they are.\n");
#else
			printf("y or SP to mark all read.\n");
			printf("n to forget it.\n");
#endif
			goto reask_catchup;
		    }
		    else if (*buf != 'y' && *buf != 'n' && *buf != 'q') {
			printf(hforhelp);
			settle_down();
			goto reask_catchup;
		    } else if (*buf == 'y' && ng<nextrcline)
			catch_up(ng);
		    else
			retry = TRUE;
		    ng++;
#else
		    notincl("c");
#endif
		    break;
#ifdef USETHREADS
		case 't':
		    if (!use_threads)
			printf("\n\nNot running in thread mode.\n");
		    else if (ng < nextrcline && toread[ng] >= TR_NONE) {
			bool force_threaded = (rcchar[ng] == ':');
			rcchar[ng] = (force_threaded ? '0' : ':');
			printf("\n\n%s will %s.\n", rcline[ng],
				force_threaded ? "always be read threaded"
				: "be read unthreaded if no thread file exists"
				) FLUSH;
			set_toread(ng);
		    }
		    special = TRUE;	/* don't skip it if toread==0 */
		    break;
#endif
		case 'u':		/* unsubscribe */
		    if (ng < nextrcline && toread[ng] >= TR_NONE) {
					/* unsubscribable? */
			printf(unsubto,rcline[ng]) FLUSH;
			rcchar[ng] = NEGCHAR;
					/* unsubscribe to (from?) it */
			toread[ng] = TR_UNSUB;
					/* and make line invisible */
			ng++;		/* do an automatic 'n' */
		    }
		    break;
		case 'h': {		/* help */
		    int cmd;

		    if ((cmd = help_ng()) > 0)
			pushchar(cmd);
		    goto reask_newsgroup;
		}
		case 'A':
		    if (ng >= nextrcline)
			break;
reask_abandon:
#ifdef VERBOSE
		    IF(verbose)
			in_char("\nAbandon changes to current newsgroup? [yn] ", 'B');
		    ELSE
#endif
#ifdef TERSE
			in_char("\nAbandon? [ynh] ", 'B');
#endif
		    setdef(buf,"y");
#ifdef VERIFY
		    printcmd();
#endif
		    putchar('\n') FLUSH;
		    if (*buf == 'h') {
#ifdef VERBOSE
			printf("Type y or SP to abandon the changes to this group since you started trn.\n");
			printf("Type n to leave the group as it is.\n");
#else
			printf("y or SP to abandon changes to this group.\n");
			printf("n to forget it.\n");
#endif
			goto reask_abandon;
		    }
		    else if (*buf != 'y' && *buf != 'n' && *buf != 'q') {
			printf(hforhelp);
			settle_down();
			goto reask_abandon;
		    } else if (*buf == 'y')
			abandon_ng(ng);
		    special = TRUE;	/* don't skip it if toread==0 */
		    break;
		case 'a':
#ifndef FINDNEWNG
		    notincl("a");
		    goto reask_newsgroup;
#else
		    addnewbydefault = 0;
		    /* FALL THROUGH */
#endif
		case 'o':
#ifdef ONLY
		{
#ifdef FINDNEWNG
		    bool doscan = (*buf == 'a');
#endif

		    if (!finish_command(TRUE)) /* get rest of command */
			goto reinp_newsgroup;	/* if rubbed out, try something else */
		    end_only();
		    if (buf[1]) {
			bool minusd = instr(buf+1,"-d", TRUE) != Nullch;

			sw_list(buf+1);
			if (minusd)
			    cwd_check();
			putchar('\n') FLUSH;
#ifdef FINDNEWNG
			if (doscan && maxngtodo)
			    scanactive();
#endif
		    }
		    ng = 0;		/* simulate ^ */
		    retry = FALSE;
		    break;
		}
#else
		    notincl("o");
		    goto reask_newsgroup;
#endif
		case '&':
		    if (switcheroo()) /* get rest of command */
			goto reinp_newsgroup;	/* if rubbed out, try something else */
		    goto reask_newsgroup;
		case 'l': {		/* list other newsgroups */
		    if (!finish_command(TRUE)) /* get rest of command */
			goto reinp_newsgroup;	/* if rubbed out, try something else */
		    for (s = buf+1; *s == ' '; s++);
		    			/* skip leading spaces */
		    sprintf(cmd_buf,"%s '%s'",filexp(NEWSGROUPS),s);
		    resetty();
		    if (doshell(sh,cmd_buf))
#ifdef VERBOSE
			IF(verbose)
			    fputs("    (Error from newsgroups program)\n",
				stdout) FLUSH;
			ELSE
#endif
#ifdef TERSE
			    fputs("(Error)\n",stdout) FLUSH;
#endif
		    noecho();
		    crmode();
		    goto reask_newsgroup;
		}
#ifdef USETHREADS
		case 'U': case '+':
#endif
		case '.': case '=':
		case 'y': case 'Y': case '\t': /* do normal thing */
		    if (ng >= nextrcline) {
			fputs("\nNot on a newsgroup.",stdout) FLUSH;
			goto reask_newsgroup;
		    }
#ifdef USETHREADS
		    else if (*buf == '+' || *buf == 'U' || *buf == '=') {
			buf[1] = '\0';
			s = savestr(buf);
		    }
#else
		    if (*buf == '=')
			s = savestr("=");
#endif
		    else if (*buf == '.') {	/* start command? */
			if (!finish_command(FALSE)) /* get rest of command */
			    goto reinp_newsgroup;
			s = savestr(buf+1);
					/* do_newsgroup will free it */
		    }
		    else
			s = Nullch;
		    if (toread[ng])
			retry = TRUE;
		    switch (do_newsgroup(s)) {
		    case NG_ERROR:
		    case NG_NORM:
			ng++;
			break;
		    case NG_ASK:
			goto reask_newsgroup;
		    case NG_SELPRIOR:
			*buf = 'p';
			goto do_command;
		    case NG_SELNEXT:
			*buf = 'n';
			goto do_command;
		    case NG_MINUS:
			ng = recent_ng;	/* recall previous newsgroup */
			special = TRUE;	/* don't skip it if toread==0 */
			break;
		    }
		    break;
#ifdef STRICTCR
		case '\n':
		    fputs(badcr,stdout) FLUSH;
		    goto reask_newsgroup;
#endif
		case 'v':
		    printf("\n%s",rnid);
		    printf("\n%s",patchlevel);
		    printf("\nSend bugs to davison@borland.com\n") FLUSH;
		    goto reask_newsgroup;
		default:
		    printf("\n%s",hforhelp) FLUSH;
		    settle_down();
		    goto reask_newsgroup;
		}
	    }
	} while (retry);
    }

    /* now write .newsrc back out */

    write_rc();

    if (oh_for_the_good_old_days)
	get_old_rc();

    finalize(0);			/* and exit */
}

/* set current newsgroup */

void
set_ngname(what)
char *what;
{
    int len = strlen(what)+1;

    growstr(&ngname,&ngnlen,len);
    strcpy(ngname,what);
    growstr(&ngdir,&ngdlen,len);
    strcpy(ngdir,getngdir(ngname));
}

static char *myngdir;
static int ngdirlen = 0;

char *
getngdir(ngnam)
char *ngnam;
{
    register char *s;

    growstr(&myngdir,&ngdirlen,strlen(ngnam)+1);
    strcpy(myngdir,ngnam);
    for (s = myngdir; *s; s++)
	if (*s == '.')
	    *s = '/';
    return myngdir;
}
