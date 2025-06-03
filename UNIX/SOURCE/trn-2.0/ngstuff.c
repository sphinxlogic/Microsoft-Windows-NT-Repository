/* $Id: ngstuff.c,v 1.2 92/01/11 16:06:15 usenet Exp $
 *
 * $Log:	ngstuff.c,v $
 * Revision 1.2  92/01/11  16:06:15  usenet
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
#include "term.h"
#include "util.h"
#include "ng.h"
#include "bits.h"
#include "intrp.h"
#include "cheat.h"
#include "head.h"
#include "final.h"
#include "sw.h"
#ifdef USETHREADS
#include "threads.h"
#include "rthreads.h"
#include "rn.h"
#include "rcstuff.h"
#endif
#include "decode.h"
#include "INTERN.h"
#include "ngstuff.h"

void
ngstuff_init()
{
    ;
}

/* do a shell escape */

int
escapade()
{
    register char *s;
    bool interactive = (buf[1] == FINISHCMD);
    bool docd;
    char whereiam[512];

    if (!finish_command(interactive))	/* get remainder of command */
	return -1;
    s = buf+1;
    docd = *s != '!';
    if (!docd) {
	s++;
    }
    else {
	getwd(whereiam);
	if (chdir(cwd)) {
	    printf(nocd,cwd) FLUSH;
	    sig_catcher(0);
	}
    }
    while (*s == ' ') s++;
					/* skip leading spaces */
    interp(cmd_buf, (sizeof cmd_buf), s);/* interpret any % escapes */
    resetty();				/* make sure tty is friendly */
    doshell(Nullch,cmd_buf);	/* invoke the shell */
    noecho();				/* and make terminal */
    crmode();				/*   unfriendly again */
    if (docd) {
	if (chdir(whereiam)) {
	    printf(nocd,whereiam) FLUSH;
	    sig_catcher(0);
	}
    }
#ifdef MAILCALL
    mailcount = 0;			/* force recheck */
#endif
    return 0;
}

/* process & command */

int
switcheroo()
{
    if (!finish_command(TRUE)) /* get rest of command */
	return -1;	/* if rubbed out, try something else */
    if (!buf[1])
	pr_switches();
#ifdef PUSHBACK
    else if (buf[1] == '&') {
	if (!buf[2]) {
	    page_init();
	    show_macros();
	}
	else {
	    char tmpbuf[LBUFLEN];
	    register char *s;

	    for (s=buf+2; isspace(*s); s++);
	    mac_line(s,tmpbuf,(sizeof tmpbuf));
	}
    }
#endif
    else {
	bool docd = (instr(buf,"-d", TRUE) != Nullch);
 	char whereami[512];
 
	if (docd)
	    getwd(whereami);
	sw_list(buf+1);
	if (docd) {
	    cwd_check();
	    if (chdir(whereami)) {		/* -d does chdirs */
		printf(nocd,whereami) FLUSH;
		sig_catcher(0);
	    }
	}
    }
    return 0;
}

/* process range commands */

int
numnum()
{
    ART_NUM min, max;
    char *cmdlst = Nullch;
    register char *s, *c;
    ART_NUM oldart = art;
    char tmpbuf[LBUFLEN];
    bool justone = TRUE;		/* assume only one article */

    perform_cnt = 0;
    if (!finish_command(TRUE))	/* get rest of command */
	return NN_INP;
	if (lastart < 1) {
	    fputs("\nNo articles\n",stdout) FLUSH;
	    return NN_ASK;
	}
#ifdef ARTSRCH
    if (srchahead)
	srchahead = -1;
#endif
    for (s=buf; *s && (isdigit(*s) || index(" ,-.$",*s)); s++)
	if (!isdigit(*s))
	    justone = FALSE;
    if (*s) {
	cmdlst = savestr(s);
	justone = FALSE;
    }
    else if (!justone)
	cmdlst = savestr("m");
    *s++ = ',';
    *s = '\0';
    safecpy(tmpbuf,buf,LBUFLEN);
    for (s = tmpbuf; c = index(s,','); s = ++c) {
	*c = '\0';
	if (*s == '.')
	    min = oldart;
	else
	    min = atol(s);
#ifdef USETHREADS
	if (min<absfirst && justone) {
	    int r;

	    /* Check if this is a root number */
	    for (r = 0; r < total.root; r++) {
		if (p_roots[r].root_num == min) {
		    p_art = p_articles + p_roots[r].articles;
		    art = p_art->num;
		    if (p_art->subject == -1) {
			follow_thread('N');
		    }
		    return NN_REREAD;
		}
	    }
	}
#endif
	if (min<absfirst) {		/* make sure it is reasonable */
	    min = absfirst;
	    printf("(First article is %ld)\n",(long)absfirst) FLUSH;
	    pad(just_a_sec/3);
	}
	if ((s=index(s,'-')) != Nullch) {
	    s++;
	    if (*s == '$')
		max = lastart;
	    else if (*s == '.')
		max = oldart;
	    else
		max = atol(s);
	}
	else
	    max = min;
	if (max>lastart) {
	    max = lastart;
	    if (min > max)
		min = max;
	    printf("(Last article is %ld)\n",(long)lastart) FLUSH;
	    pad(just_a_sec/3);
	}
	if (max < min) {
	    fputs("\nBad range\n",stdout) FLUSH;
	    if (cmdlst)
		free(cmdlst);
	    return NN_ASK;
	}
	if (justone) {
	    art = min;
	    return NN_REREAD;
	}
	check_first(min);
	for (art=min; art<=max; art++) {
	    if (perform(cmdlst,TRUE)) {
#ifdef VERBOSE
		IF(verbose)
		    printf("\n(Interrupted at article %ld)\n",(long)art)
		      FLUSH;
		ELSE
#endif
#ifdef TERSE
		    printf("\n(Intr at %ld)\n",(long)art) FLUSH;
#endif
		if (cmdlst)
		    free(cmdlst);
		return NN_ASK;
	    }
	}
    }
    art = oldart;
    if (cmdlst)
	free(cmdlst);
    return NN_NORM;
}

#ifdef USETHREADS
int
use_selected()
{
    PACKED_ARTICLE *root_limit;
    register char ch;
    register int r;
    char *cmdstr;
    int ret = 1, orig_root_cnt = selected_root_cnt;

    if (!finish_command(TRUE))	/* get rest of command */
	return 0;
    if (!(ch = buf[1]))
	return -1;
    cmdstr = savestr(buf+1);

    perform_cnt = 0;
    page_line = 1;

    /* Multiple commands and commands that operate on individual articles
    ** use the article loop.
    */
    if (strlen(cmdstr) > 1 || index("ejmMsSwW|=", ch)) {
	bool want_unread = (unread_selector || ch == 'm');

	for (r = 0; r < total.root; r++) {
	    if (scan_all_roots
	     || (!orig_root_cnt&&root_article_cnts[r]&&!(selected_roots[r]&4))
	     || (selected_roots[r] & (unread_selector+1))) {
		p_art = p_articles + p_roots[r].articles;
		root_limit = upper_limit( p_art, 0 );
		for (; p_art < root_limit; p_art++) {
		    art = p_art->num;
		    if (p_art->subject != -1
		     && (!was_read(art) ^ want_unread)) {
			if (perform(cmdstr, TRUE)) {
			    fputs("\nInterrupted\n", stdout) FLUSH;
			    goto break_out;
			}
		    }
		    if (p_art == Nullart)
			break;
		}/* for all articles */
	    }/* if selected */
	}/* for all threads */
    }				/* other commands get the root loop */
    else if (ch == '+' || ch == '-' || ch == 'J' || ch == 'T' || ch == 't') {
	for (r = 0; r < total.root; r++) {
	    if (scan_all_roots
	     || (!orig_root_cnt&&root_article_cnts[r]&&!(selected_roots[r]&4))
	     || (selected_roots[r] & (unread_selector+1))) {
		if (mode != 't' && ch != 't') {
		    printf("T%-5ld ", (long)p_roots[r].root_num);
		}
		p_art = p_articles + p_roots[r].articles;
		art = p_art->num;
		if (perform(cmdstr, FALSE)) {
		    fputs("\nInterrupted\n", stdout) FLUSH;
		    goto break_out;
		}
#ifdef VERBOSE
		IF(verbose)
		    if (mode != 't' && ch != 't' && ch != 'T')
			putchar('\n') FLUSH;
#endif
	    }
	}
    }
    else if (ch == 'E') {	/* one command needs no looping at all */
	if (decode_fp) {
	    decode_end();
	} else {
	    ret = 2;
	}
    }
    else {
	printf("???%s\n",cmdstr);
	ret = -1;
    }
  break_out:
    free(cmdstr);
    return ret;
}
#endif

int
perform(cmdlst,toplevel)
register char *cmdlst;
int toplevel;
{
    register int ch;
    
    if (toplevel) {
	printf("%-6ld ",art);
	fflush(stdout);
    }
    perform_cnt++;
    for (; ch = *cmdlst; cmdlst++) {
	if (isspace(ch) || ch == ':')
	    continue;
	if (ch == 'j') {
	    if (!was_read(art)) {
		mark_as_read();
#ifdef VERBOSE
		IF(verbose)
		    fputs("\tJunked",stdout);
#endif
	    }
#ifdef USETHREADS
	    else if (unread_selector)
		goto unselect_it;
#endif
	}
#ifdef USETHREADS
	else if (ch == '+') {
	  register char mask = unread_selector+1;
	    find_article(art);
	    if (p_art && !(selected_roots[p_art->root] & mask)) {
	      register int r = p_art->root;
		selected_roots[r] |= mask;
		selected_root_cnt++;
		if (mode == 't') {
		    selected_count += root_article_cnts[r];
		} else {
		    selected_count += count_one_root(r);
#ifdef VERBOSE
		    IF(verbose)
			fputs("\tSelected",stdout);
#endif
		}
	    }
	}
	else if (ch == '-') {
	  register char mask;
	  unselect_it:
	    mask = unread_selector+1;
	    find_article(art);
	    if (p_art && selected_root_cnt
	     && (selected_roots[p_art->root] & mask)) {
	      register int r = p_art->root;
		selected_roots[r] &= ~mask;
		if (unread_selector)
		    selected_roots[r] |= 4;
		selected_root_cnt--;
		if (mode == 't') {
		    selected_count -= root_article_cnts[r];
		} else {
		    selected_count -= count_one_root(r);
#ifdef VERBOSE
		    IF(verbose)
			fputs("\tDeselected",stdout);
#endif
		}
	    }
	}
	else if (ch == 't') {
	    find_article(art);
	    entire_tree();
	}
	else if (ch == 'J' || ch == 'T' || ch == ',') {
	    char tmpbuf[128];
	    ART_NUM oldart = art;

	    find_article(art);
	    if (ch == ',')
		mark_as_read();
	    if (p_art) {
		if (ch == 'T') {
		    sprintf(tmpbuf,"T%ld\t# %s",
			(long)p_roots[p_art->root].root_num,
			subject_ptrs[p_art->subject]);
		    fputs(tmpbuf,stdout);
		    kf_append(tmpbuf);
		}
		follow_thread(ch == ',' ? 'K' : 'J');
		art = oldart;
	    }
	}
#endif
	else if (ch == 'm') {
	    if (was_read(art)) {
		unmark_as_read();
#ifdef VERBOSE
		IF(verbose)
		    fputs("\tMarked unread",stdout);
#endif
	    }
	}
	else if (ch == 'M') {
#ifdef DELAYMARK
	    delay_unmark(art);
#ifdef VERBOSE
	    IF(verbose)
		fputs("\tWill return",stdout);
#endif
#else
	    notincl("M");
	    return -1;
#endif
	}
	else if (ch == '=') {
	    printf("\t%s",fetchsubj(art,FALSE,FALSE));
#ifdef VERBOSE
	    IF(verbose)
		;
	    ELSE
#endif
		putchar('\n') FLUSH;		/* ghad! */
	}
	else if (ch == 'C') {
#ifdef ASYNC_PARSE
	    printf("\t%sancelled",(cancel_article() ? "Not c" : "C"));
#else
	    notincl("C");
	    return -1;
#endif
	}
	else if (ch == '%') {
#ifdef ASYNC_PARSE
	    char tmpbuf[512];

	    if (one_command)
		interp(tmpbuf, (sizeof tmpbuf), cmdlst);
	    else
		cmdlst = dointerp(tmpbuf, (sizeof tmpbuf), cmdlst, ":") - 1;
	    perform_cnt--;
	    if (perform(tmpbuf,FALSE))
		return -1;
#else
	    notincl("%");
	    return -1;
#endif
	}
	else if (index("!&sSwWe|",ch)) {
	    if (one_command)
		strcpy(buf,cmdlst);
	    else
		cmdlst = cpytill(buf,cmdlst,':') - 1;
	    /* we now have the command in buf */
	    if (ch == '!') {
		escapade();
#ifdef VERBOSE
		IF(verbose)
		    fputs("\tShell escaped",stdout);
#endif
	    }
	    else if (ch == '&') {
		switcheroo();
#ifdef VERBOSE
		IF(verbose)
		    if (buf[1] && buf[1] != '&')
			fputs("\tSwitched",stdout);
#endif
	    }
	    else {
		putchar('\t');
		save_article();
#ifdef VERBOSE
		IF(verbose)
		    ;
		ELSE
#endif
		    putchar('\n') FLUSH;
	    }
	}
	else {
	    printf("\t???%s\n",cmdlst);
	    return -1;
	}
#ifdef VERBOSE
	fflush(stdout);
#endif
	if (one_command)
	    break;
    }
    if (toplevel) {
#ifdef VERBOSE
	IF(verbose)
	    putchar('\n') FLUSH;
#endif
    }
    if( int_count ) {
	int_count = 0;
	return -1;
    }
    return 0;
}
