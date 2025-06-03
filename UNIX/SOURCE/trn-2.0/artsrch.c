/* $Id: artsrch.c,v 1.1 92/01/11 15:49:46 usenet Stab $
 *
 * $Log:	artsrch.c,v $
 * Revision 1.1  92/01/11  15:49:46  usenet
 * USENET News Software
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
#include "search.h"
#include "term.h"
#include "util.h"
#include "intrp.h"
#include "bits.h"
#include "kfile.h"
#include "head.h"
#include "final.h"
#include "cheat.h"
#ifdef SERVER
#include "server.h"
#endif
#include "ng.h"
#include "artio.h"
#ifdef USETHREADS
#include "threads.h"
#include "rthreads.h"
#include "ngdata.h"
#endif
#include "INTERN.h"
#include "artsrch.h"

void
artsrch_init()
{
#ifdef ARTSEARCH
#ifdef ZEROGLOB
    init_compex(&sub_compex);
    init_compex(&art_compex);
#endif
#endif
}

/* search for an article containing some pattern */

#ifdef ARTSEARCH
int
art_search(patbuf,patbufsiz,get_cmd)
char *patbuf;				/* if patbuf != buf, get_cmd must */
int patbufsiz;
int get_cmd;				/*   be set to FALSE!!! */
{
    char *pattern;			/* unparsed pattern */
    register char cmdchr = *patbuf;	/* what kind of search? */
    register char *s;
    bool backward = cmdchr == '?' || cmdchr == Ctl('p');
					/* direction of search */
    COMPEX *compex;			/* which compiled expression */
    char *cmdlst = Nullch;		/* list of commands to do */
    int normal_return = SRCH_NOTFOUND;	/* assume no commands */
    bool saltaway = FALSE;		/* store in KILL file? */
    char howmuch;			/* search just the subjects */
    bool doread;			/* search read articles? */
    bool foldcase = TRUE;		/* fold upper and lower case? */

    int_count = 0;
    if (cmdchr == '/' || cmdchr == '?') {	/* normal search? */
	if (get_cmd && buf == patbuf)
	    if (!finish_command(FALSE))	/* get rest of command */
		return SRCH_ABORT;
	compex = &art_compex;
	if (patbuf[1]) {
	    howmuch = 0;
	    doread = FALSE;
	}
	else {
	    howmuch = art_howmuch;
	    doread = art_doread;
	}
	s = cpytill(buf,patbuf+1,cmdchr);/* ok to cpy buf+1 to buf */
	pattern = buf;
	if (*pattern) {
	    if (*lastpat)
		free(lastpat);
	    lastpat = savestr(pattern);
	}
	if (*s) {			/* modifiers or commands? */
	    for (s++; *s && index("Kharc",*s); s++) {
		if (*s == 'h')		/* scan header */
		    howmuch = 1;
		else if (*s == 'a')	/* scan article */
		    howmuch = 2;
		else if (*s == 'r')	/* scan read articles */
		    doread = TRUE;
		else if (*s == 'K')	/* put into KILL file */
		    saltaway = TRUE;
		else if (*s == 'c')	/* make search case sensitive */
		    foldcase = FALSE;
	    }
	}
	while (isspace(*s) || *s == ':')
	    s++;
	if (*s) {
	    if (*s == 'm' || *s == 'M')
		doread = TRUE;
	    if (*s == 'k')		/* grandfather clause */
		*s = 'j';
	    cmdlst = savestr(s);
	    normal_return = SRCH_DONE;
	}
	art_howmuch = howmuch;
	art_doread = doread;
	if (srchahead)
	    srchahead = -1;
    }
    else {
	register char *h;

	howmuch = 0;			/* just search subjects */
	doread = (cmdchr == Ctl('p'));
	if (cmdchr == Ctl('n'))
	    normal_return = SRCH_SUBJDONE;
	compex = &sub_compex;
	pattern = patbuf+1;
	strcpy(pattern,": *");
	h = pattern + strlen(pattern);
	interp(h,patbufsiz - (h-patbuf),"%\\s");  /* fetch current subject */
	if (cmdchr == 'K') {
	    saltaway = TRUE;
	    cmdchr = 'k';
	}
	if (cmdchr == 'k') {
	    normal_return = SRCH_DONE;
	    cmdlst = savestr("j");
	    mark_as_read();		/* this article has this subject */
	    if (!*h) {
#ifdef VERBOSE
		IF(verbose)
		    fputs("\nCannot delete null subject.\n",stdout) FLUSH;
		ELSE
#endif
#ifdef TERSE
		    fputs("\nNull subject.\n",stdout) FLUSH;
#endif
		return SRCH_ABORT;
	    }
#ifdef VERBOSE
	    else if (verbose)
		printf("\nMarking subject \"%s\" as read.\n",h) FLUSH;
#endif
	}
	else if (!srchahead)
	    srchahead = -1;
	{			/* compensate for notesfiles */
	    register int i;
	    for (i = 24; *h && i--; h++)
		if (*h == '\\')
		    h++;
	    *h = '\0';
	}
#ifdef DEBUGGING
	if (debug) {
	    printf("\npattern = %s\n",pattern) FLUSH;
	}
#endif
    }
    if ((s = compile(compex,pattern,TRUE,foldcase)) != Nullch) {
					/* compile regular expression */
	printf("\n%s\n",s) FLUSH;
	return SRCH_ABORT;
    }
#ifdef KILLFILES
    if (saltaway) {
	char saltbuf[LBUFLEN];

	s = saltbuf;
	sprintf(s,"/%s/",pattern);
	s += strlen(s);
	if (doread)
	    *s++ = 'r';
	if (howmuch==1)
	    *s++ = 'h';
	else if (howmuch==2)
	    *s++ = 'a';
	*s++ = ':';
	if (!cmdlst)
	    cmdlst = savestr("j");
	safecpy(s,cmdlst,LBUFLEN-(s-saltbuf));
	kf_append(saltbuf);
    }
#endif
    if (cmdlst && index(cmdlst,'='))
	normal_return = SRCH_ERROR;	/* listing subjects is an error? */
    if (get_cmd) {
	fputs("\nSearching...\n",stdout) FLUSH;
					/* give them something to read */
    }
#ifdef USETHREADS
    if (mode == 't') {
	if (!cmdlst)
	    cmdlst = savestr("+");	/* thread selector's default command */
	if (unread_selector)
	    doread = TRUE;
	normal_return = SRCH_DONE;
    }
#endif
    if (backward) {
	if (cmdlst && art <= lastart)
	    art++;			/* include current article */
	if (doread)
	    check_first(absfirst);
    }
    else {
	if (art > lastart) {
	    art = (doread ? absfirst : firstart);
	    check_first(art--);
	}
	else if (cmdlst && art >= absfirst)
	    art--;			/* include current article */
    }
    if (srchahead > 0) {
	if (!backward)
	    art = srchahead - 1;
	srchahead = -1;
    }
    assert(!cmdlst || *cmdlst);
    perform_cnt = 0;
    for (;;) {
	if (backward ?
		(--art < absfirst || (!doread && art < firstart)) :
		(++art > lastart)
	  ) {			/* out of articles? */
	    if (cmdlst)
		free(cmdlst);
	    return normal_return;
	}
	if (int_count) {
	    int_count = 0;
	    if (cmdlst)
		free(cmdlst);
	    return SRCH_INTR;
	}
	/*NOSTRICT*/
	if (doread || !was_read(art)) {
	    if (wanted(compex,art,howmuch)) {
				    /* does the shoe fit? */
		if (cmdlst) {
		    if (perform(cmdlst,TRUE)) {
			if (cmdlst)
			    free(cmdlst);
			return SRCH_INTR;
		    }
		}
		else {
		    if (cmdlst)
			free(cmdlst);
		    return SRCH_FOUND;
		}
	    }
	    else if (!cmdlst && ! (art%50)) {
		printf("...%ld",(long)art);
		fflush(stdout);
	    }
	}
    }
}

/* determine if article fits pattern */
/* returns TRUE if it exists and fits pattern, FALSE otherwise */

bool
wanted(compex, artnum, scope)
COMPEX *compex;
ART_NUM artnum;
char_int scope;
{
    if (!scope) {
	char subj_buf[266];
	
#ifdef USETHREADS
	if (ThreadedGroup)
	    find_article(art);
	if (p_art) {
	    if (mode != 't')
		strcpy(subj_buf, "Subject: ");
	    else
		*subj_buf = '\0';
	    if (p_art->subject != -1)
		strcat(subj_buf,subject_ptrs[p_art->subject]);
	}
	else
#endif
	{
	    strcpy(subj_buf, "Subject: ");
	    strncpy(subj_buf+9,fetchsubj(artnum,FALSE,FALSE),256);
	}
#ifdef DEBUGGING
	if (debug & DEB_SEARCH_AHEAD)
	    printf("%s\n",subj_buf) FLUSH;
#endif
	return execute(compex,subj_buf) != Nullch;
    }
#ifdef CACHESUBJ
    else
	fetchsubj(artnum,FALSE,FALSE);/* might as well get subject handy */
#endif
    
#ifdef SERVER
    if (scope == 1){
	if (nntpopen(artnum,GET_HEADER) == Nullfp) /* we only need the header */
	    return FALSE;
    }
    else
#endif
    if (artopen(artnum) == Nullfp)	/* ensure that article is open */

	return FALSE;			/* if not, return NO MATCH */
    scope--;
    while (fgets(buf,LBUFLEN,artfp) != Nullch) {
					/* for each line of article */
	if (!scope && index(buf,':') == Nullch && *buf != ' ' && *buf != '\t')
					/* if headers only and out of header */
	    return FALSE;		/* say no go */
	if (execute(compex,buf) != Nullch) {
					/* does pattern matcher match? */
	    return TRUE;		/* say Eureka */
	}
    }
    return FALSE;			/* out of article, so no match */
}
#endif

