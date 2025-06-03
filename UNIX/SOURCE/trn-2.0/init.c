/* $Id: init.c,v 1.2 92/01/11 16:04:39 usenet Exp $
 *
 * $Log:	init.c,v $
 * Revision 1.2  92/01/11  16:04:39  usenet
 * header twiddling, bug fixes
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
#include "util.h"
#include "final.h"
#include "term.h"
#include "last.h"
#include "rn.h"
#include "rcstuff.h"
#include "ngdata.h"
#include "only.h"
#include "intrp.h"
#include "addng.h"
#include "sw.h"
#include "art.h"
#include "artsrch.h"
#include "artio.h"
#include "backpage.h"
#include "bits.h"
#include "cheat.h"
#include "head.h"
#include "help.h"
#include "kfile.h"
#include "ngsrch.h"
#include "ngstuff.h"
#include "rcln.h"
#include "respond.h"
#ifdef SERVER
#include "server.h"
#endif
#ifdef USETHREADS
#include "threads.h"
#include "rthreads.h"
#endif
#include "ng.h"
#include "decode.h"
#include "INTERN.h"
#include "init.h"

bool
initialize(argc,argv)
int argc;
char *argv[];
{
    char *tcbuf;
    register bool foundany = FALSE;
    long time();
#ifdef SERVER
    char *server;
    int response;
#endif
#ifdef NOLINEBUF
    static char std_out_buf[BUFSIZ];	/* must be static or malloced */

    setbuf(stdout, std_out_buf);
#endif

    tcbuf = safemalloc(1024);		/* make temp buffer for termcap and */
					/* other initialization stuff */
    
    /* init terminal */
    
    term_init();			/* must precede sw_init() so that */
					/* ospeed is set for baud-rate */
					/* switches.  Actually terminal */
					/* mode setting is in term_set() */

    /* we have to know rnlib to look up global switches in %X/INIT */

    lib = savestr(filexp(LIB));
    rnlib = savestr(filexp(RNLIB));

    /* decode switches */

    sw_init(argc,argv,&tcbuf);          /* must not do % interps! */
					/* (but may mung environment) */

    /* init signals, status flags */

    final_init();
    
    /* start up file expansion and the % interpreter */

    intrp_init(tcbuf);
    
    /* now make sure we have a current working directory */

    if (!checkflag)
	cwd_check();
    
    /* now that we know where to save things, cd to news directory */

    if (chdir(spool)) {
	printf(nocd,spool) FLUSH;
	finalize(1);
    }

    /* if we aren't just checking, turn off echo */

    if (!checkflag)
	term_set(tcbuf);

    /* get info on last rn run, if any */

    if (!checkflag)
	last_init(tcbuf);

    free(tcbuf);			/* recover 1024 bytes */

    /* make sure we are the sole possessors of .newsrc */

    if (!checkflag)
	lock_check();

    /* check for news news */

    if (!checkflag)
	newsnews_check();

#ifdef SERVER

    /* open connection to server if appropriate */

    if ((server = get_server_name(1)) == NULL)
	finalize(1);

    response = server_init(server);
    if (response < 0) {
	fprintf(stderr,
	    "Couldn't connect to %s news server, try again later.\n",
		server);
	finalize(1);
    }

    if (handle_server_response(response, server) < 0)
	finalize(1);

#endif

    /* open active file, etc. */

    ngdata_init();

    /* now read in the .newsrc file */

    foundany = rcstuff_init();

    /* it looks like we will actually read something, so init everything */

    addng_init();
    art_init();
    artio_init();
    artsrch_init();
    backpage_init();
    bits_init();
    cheat_init();
    head_init();
    help_init();
    kfile_init();
    ng_init();
    ngsrch_init();
    ngstuff_init();
    only_init();
    rcln_init();
    respond_init();
    rn_init();
    search_init();
    decode_init();
#ifdef USETHREADS
    thread_init();
#endif
    util_init();

#ifdef FINDNEWNG
	/*
	 * Skip this check if the -q flag was given.
	 */
    if (!quickstart) {
	if (find_new_groups()) {	/* did we add any new groups? */
	    foundany = TRUE;		/* let main() know */
	    starthere = 0;		/* start ng scan from the top */
	}
    }
#endif
    time(&lasttime);			/* remember when we inited-- */
					/* ends up back in .rnlast */
    writelast();			/* in fact, put it there now */

#ifdef FINDNEWNG
# ifdef ONLY
    if (maxngtodo)			/* patterns on command line? */
	foundany |= scanactive();
# endif
#endif

    return foundany;
}

/* make sure there is no rn out there already */

void
lock_check()
{
    lockname = savestr(filexp(LOCKNAME));
    if (!checkflag) {
	tmpfp = fopen(lockname,"r");
	if (tmpfp != Nullfp) {
	    int processnum;
    
	    fgets(buf,LBUFLEN,tmpfp);
	    fclose(tmpfp);
	    processnum = atoi(buf);
#ifdef VERBOSE
	    IF(verbose)
		printf("You seem to have left a trn running, process %d.\n",
		    processnum) FLUSH;
	    ELSE
#endif
#ifdef TERSE
		printf("Trn left running, #%d.\n", processnum) FLUSH;
#endif
	    if (kill(processnum, SIGEMT)) {
				    /* does process not exist? */
				    /* (rn ignores SIGEMT) */
		sleep(2);
#ifdef VERBOSE
		IF(verbose)
		    fputs("\n\
That process does not seem to exist anymore.  The count of read articles\n\
may be incorrect in the last newsgroup accessed by that other (defunct)\n\
process.\n\n",stdout) FLUSH;
		ELSE
#endif
#ifdef TERSE
		    fputs("\nProcess crashed.\n",stdout) FLUSH;
#endif
		if (*lastngname) {
#ifdef VERBOSE
		    IF(verbose)
			printf("(The last newsgroup accessed was %s.)\n\n",
			lastngname) FLUSH;
		    ELSE
#endif
#ifdef TERSE
			printf("(In %s.)\n\n",lastngname) FLUSH;
#endif
		}
		get_anything();
		putchar('\n') FLUSH;
	    }
	    else {
#ifdef VERBOSE
		IF(verbose)
		    fputs("\n\
You may not have two copies of [t]rn running simultaneously.  Goodbye.\n\
",stdout) FLUSH;
		ELSE
#endif
#ifdef TERSE
		    fputs("\nCan't start another.\n",stdout) FLUSH;
#endif
               if (bizarre)
                 resetty();
		exit(0);
	    }
	}
	tmpfp = fopen(lockname,"w");
	if (tmpfp == Nullfp) {
	    printf(cantcreate,lockname) FLUSH;
	    sig_catcher(0);
	}
	fprintf(tmpfp,"%d\n",getpid());
	fclose(tmpfp);
    }
}

void
newsnews_check()
{
    char *newsnewsname = filexp(NEWSNEWSNAME);

    if ((tmpfp = fopen(newsnewsname,"r")) != Nullfp) {
	fstat(fileno(tmpfp),&filestat);
	if (filestat.st_mtime > lasttime) {
	    while (fgets(buf,sizeof(buf),tmpfp) != Nullch)
		fputs(buf,stdout) FLUSH;
	    get_anything();
	    putchar('\n') FLUSH;
	}
	fclose(tmpfp);
    }
}
