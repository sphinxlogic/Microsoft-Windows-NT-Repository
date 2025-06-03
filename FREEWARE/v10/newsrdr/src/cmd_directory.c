/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Directory commands.
**
**  MODULE DESCRIPTION:
**
**  	This module contains the DIRECTORY command routines.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  09-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	09-SEP-1992 V1.0    Madison 	Initial coding.
**  	15-SEP-1992 V1.0-1  Madison 	Add DIR/GROUP/SORT.
**  	15-FEB-1993 V1.0-2  Madison 	Make compatible with pre-VMS V5.2.
**  	24-FEB-1993 V1.0-3  Madison 	DIR output was incorrect when field width
**  	    	    	    	    	 exceeded string length.
**  	12-APR-1993 V1.1    Madison 	Get_Article_* changed.
**  	17-MAY-1993 V1.1-1  Madison 	Put_Paged wasn't being used right.
**  	22-SEP-1993 V1.2    Madison 	XHDR support.
**  	24-SEP-1993 V1.3    Madison 	More XHDR support.
**  	30-SEP-1993 V1.3-1  Madison 	Fix lines construction.
**  	09-OCT-1993 V1.4    Madison 	Several new features.
**  	21-OCT-1993 V1.4-1  Madison 	Do seen/unseen check before validity check.
**  	22-OCT-1993 V1.4-2  Madison 	Fix subj==0 case in cmd_dir.
**  	27-OCT-1993 V1.4-3  Madison 	Adjust column width computation.
**  	13-APR-1994 V1.4-4  Madison 	Don't get kwds hdr if we don't have to.
**--
*/
#include "newsrdr.h"
#include "globals.h"

/*
** Forward declarations
*/
    unsigned int        cmd_dirgroup(void);
    static unsigned int cmd_dirgrpclup(void);
    unsigned int        cmd_dir(void);
    static unsigned int cmd_dirclup(void);

    EXTERN struct GRP *curgroup;
    EXTERN int         User_Interrupt;
    EXTERN int         cur_artnum;
    EXTERN int         pb_cols;

    static struct QUE kwque, authq, subjq, excfq, excsq;
    static int dir_in_progress = 0;
    static int fromwid, subjwid, doseen, dounseen;
    static struct GRP *curdgrp;
    static int dgctx, doall, print_one, current, dosort, first, final;
    static char curstr[STRING_SIZE];
    static struct dsc$descriptor curdsc;
    static int thread_base, by_thread;
    static char thread_subject[STRING_SIZE], thread_msgid[STRING_SIZE];
    static char *thread_track;

    extern struct GRP *Traverse_Tree(void *);
    extern unsigned int Traverse_Finish(void *);
    extern int Count_Unseen(struct GRP *);
    extern int Next_Unseen_Article(int, int, struct GRP *);
    extern int Ignore_Article(struct GRP *, int);
    extern unsigned int Get_One_Header(struct GRP *, int, int, struct HDR **);
    extern int Is_Valid_Article(struct GRP *, int);
    extern int Article_Seen(struct GRP *, int);
    extern unsigned int Parse_Range(char *, int *, int *, int, struct GRP *);
    extern int Thread_Match(char *, char *);

/*
**++
**  ROUTINE:	cmd_dirgroup
**
**  FUNCTIONAL DESCRIPTION:
**
**  	DIRECTORY/GROUP command.  Also set up to be invoked as a
**  default_action, for paging through the group directory listing.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_dirgroup()
**
**  IMPLICIT INPUTS:	Many.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_dirgroup() {

    char tmp[STRING_SIZE], *s;
    struct dsc$descriptor sdsc;
    short tlen;
    int unseen_count;

/*
** dir_in_progress is set if we're in the middle of a listing.  If so,
** we just continue on.  Otherwise, we parse the command options and
** begin the new listing.
*/
    if (dir_in_progress) {
    	default_action = cleanup_action = NULL;
    	if (dir_in_progress == 1) {
    	    curdgrp = (!dosort && news_prof.glist) ? curdgrp->next :
    	    	    	    Traverse_Tree(&dgctx);
    	}
    	dir_in_progress = 0;
    } else {
    	dosort = cli_present("SORT") == CLI$_PRESENT;
    	doall = cli_present("ALL") == CLI$_PRESENT;
    	dounseen = cli_present("UNSEEN") == CLI$_PRESENT;
    	if (cli_present("GRPNAM") == CLI$_PRESENT) {
    	    cli_get_value("GRPNAM", curstr, sizeof(curstr));
    	} else {
    	    strcpy(curstr, "*");
    	}
    	INIT_SDESC(curdsc, strlen(curstr), curstr);
    	print_one = dgctx = 0;
    	curdgrp = (!dosort && news_prof.glist) ? news_prof.glist :
    	    	    	Traverse_Tree(&dgctx);
    }

/*
** curdgrp is set either by traversing the AVL tree of groups, or by
** going through the linked list headed by news_prof.glist.  The linked
** list is created only if the user had a NEWSRC file read in when we
** started.  If they specify /SORT, then they'll get the tree traversal
** anyway.
*/
    while (curdgrp != NULL) {
    	char gup[STRING_SIZE];
    	strcpy(gup, curdgrp->grpnam);
    	upcase(gup);
    	INIT_SDESC(sdsc, strlen(gup), gup);
    	unseen_count = Count_Unseen(curdgrp);
/*
** Unless they specify /ALL, they'll only see groups they're subscribed to.
*/
    	if ((curdgrp->subscribed || doall) &&
    	    	    (!dounseen || unseen_count > 0) &&
    	    	    OK(str$match_wild(&sdsc, &curdsc))) {
    	    static $DESCRIPTOR(grpfao, "!40AD  !11UL  !11UL  !1AD  !7UL");
    	    INIT_SDESC(sdsc, sizeof(tmp)-1, tmp);
    	    if (!print_one) {
    	    	static $DESCRIPTOR(ctrstr, "!40AD  !11AD  !11AD  !1AD  !AD");
    	    	sys$fao(&ctrstr, &tlen, &sdsc, 10, "Group name",
    	    	    11, "First avail", 10, "Last avail", 1, "S", 7, "#unseen");
    	    	*(tmp+tlen) = '\0';
    	    	Begin_Paged_Output(tmp);
    	    	print_one = 1;
    	    }

    	    s = curdgrp->subscribed ? "x" : " ";
    	    sys$fao(&grpfao, &tlen, &sdsc, strlen(curdgrp->grpnam),
    	    	curdgrp->grpnam, curdgrp->frstavl, curdgrp->lastavl,
    	    	1, s, unseen_count);
    	    *(tmp+tlen) = '\0';
    	    if (!Put_Paged(tmp, -1)) {
    	    	default_action = cmd_dirgroup;
    	    	cleanup_action = cmd_dirgrpclup;
    	    	dir_in_progress = 1;
    	    	return SS$_NORMAL;
    	    }
    	}
    	curdgrp = (!dosort && news_prof.glist) ? curdgrp->next :
    	    	    	Traverse_Tree(&dgctx);
    }

/*
** We get here when we run out of groups.  It's just a matter of finishing
** off any paged output that had to be held because we got to the bottom
** of a screen page, then exiting.  We also let the user know if there was
** no match.
*/

    if (!print_one) {
    	lib$signal(NEWS__NOGRMATCH, 2, strlen(curstr), curstr);
    } else {
    	if (!Paged_Output_Done()) {
    	    default_action = cmd_dirgroup;
    	    cleanup_action = cmd_dirgrpclup;
    	    dir_in_progress = 2;
    	    return SS$_NORMAL;
    	}
    	Finish_Paged_Output(0);
    }

    return SS$_NORMAL;

} /* cmd_dirgroup */

/*
**++
**  ROUTINE:	cmd_dirgrpclup
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Cleans up the context from a pending DIRECTORY/GROUP.
**  Used as a cleanup_action routine.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	dir_grpclup()
**
**  IMPLICIT INPUTS:	Many.
**
**  IMPLICIT OUTPUTS:	Many.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Always returned.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static unsigned int cmd_dirgrpclup() {

    if (dir_in_progress) {
    	dir_in_progress = 0;
    	curdgrp = 0;
    	if (dgctx) Traverse_Finish(&dgctx);
    	Finish_Paged_Output(0);
    	default_action = cleanup_action = NULL;
    }
    return SS$_NORMAL;

} /* cmd_dirgrpclup */

/*
**++
**  ROUTINE:	cmd_dir
**
**  FUNCTIONAL DESCRIPTION:
**
**  	DIRECTORY command.  Also set up to handle continuation of
**  an existing directory listing, invoked via the default_action
**  mechanism.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_dir()
**
**  IMPLICIT INPUTS:	Many.
**
**  IMPLICIT OUTPUTS:	Many.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_dir() {

    struct HDR *hdr;
    struct QUE *hdrqptr;
    char tmp[STRING_SIZE], upfrom[STRING_SIZE], upsubj[STRING_SIZE];
    char upkw[STRING_SIZE], lines[6];
    struct dsc$descriptor sdsc, cand, pattern;
    char *fromstr, *subj, *cp;
    short tlen;
    int doit, reply_code, junk1, junk2, seen;
    unsigned int status;

/*
** Just like DIR/GROUP, dir_in_progress indicates we're in the middle of
** a listing.  If we're not, there's just a lot more options to get through
** on the command.
*/
    if (dir_in_progress) {
    	default_action = cleanup_action = NULL;
    	if (dir_in_progress == 1) current++;
    	dir_in_progress = 0;
    } else {
    	if (curgroup == NULL) {
    	    lib$signal(NEWS__NOCURGROUP, 0);
    	    return SS$_NORMAL;
    	}

    	kwque.head = kwque.tail = &kwque;
    	authq.head = authq.tail = &authq;
    	subjq.head = subjq.tail = &subjq;
    	excsq.head = excsq.tail = &excsq;
    	excfq.head = excfq.tail = &excfq;

    	by_thread = cli_present("THREAD") == CLI$_PRESENT;
    	if (by_thread && cli_present("STRICT") == CLI$_PRESENT) by_thread++;

    	status = cli_present("SEEN");
    	if (status == CLI$_NEGATED) {
    	    doseen = 0;
    	    dounseen = 1;
    	} else {
    	    doseen = status == CLI$_PRESENT;
    	    dounseen = cli_present("UNSEEN") == CLI$_PRESENT;
    	}
    	if (!(doseen || dounseen)) doseen = dounseen = 1;

/*
** The WIDTH stuff lets the user decide how to format the output.
** It can come from the command itself, from the SET DIRECTORY_WIDTH
** settings, or from the default (an even split).  Things are set up
** so that if just one width is specified, the other width (there are
** only two they can pick) is defaulted to be the remaining screen width.
*/
    	if (cli_present("WIDTH") == CLI$_PRESENT) {
    	    if (cli_present("WIDTH.FROM") == CLI$_PRESENT) {
    	    	int i;
    	    	cli_get_value("WIDTH.FROM", tmp, sizeof(tmp));
    	    	lib$cvt_dtb(strlen(tmp), tmp, &i);
    	    	if (i >= 0) fromwid = i;
    	    } else {
    	    	fromwid = -1;
    	    }
    	    if (cli_present("WIDTH.SUBJECT") == CLI$_PRESENT) {
    	    	int i;
    	    	cli_get_value("WIDTH.SUBJECT", tmp, sizeof(tmp));
    	    	lib$cvt_dtb(strlen(tmp), tmp, &i);
    	    	if (i >= 0) subjwid = i;
    	    } else {
    	    	if (fromwid >= 0) {
    	    	    subjwid = pb_cols-17-fromwid;
    	    	    if (subjwid < 0) subjwid = 0;
    	    	} else {
    	    	    subjwid = -1;
    	    	}
    	    }
    	} else {
    	    fromwid = news_prof.dir_fromwid;
    	    subjwid = news_prof.dir_subjwid;
    	}
    	if (fromwid < 0) {
    	    if (subjwid < 0) {
    	    	fromwid = (pb_cols-17)/2 < 1 ? 1 : (pb_cols-17)/2;
    	    } else {
    	    	fromwid = pb_cols-17-subjwid;
    	    	if (fromwid < 0) fromwid = 0;
    	    }
    	}
    	if (subjwid < 0) {
    	    subjwid = pb_cols-17-fromwid;
    	    if (subjwid < 0) subjwid = 0;
    	}

/*
** Make sure they didn't stupidly specify some width that was greater than
** our fixed string length.
*/
    	if (fromwid+subjwid > STRING_SIZE-30) {
    	    if (fromwid > STRING_SIZE-30) fromwid = STRING_SIZE-30;
    	    subjwid = STRING_SIZE-30-fromwid;
    	}


    	tmp[0] = '*';
    	if (cli_present("EXCLUDE") == CLI$_PRESENT) {
    	    if (cli_present("EXCLUDE.FROM") == CLI$_PRESENT) {
    	    	while(OK(cli_get_value("EXCLUDE.FROM", tmp+1, sizeof(tmp)-2))) {
    	    	    upcase(tmp+1);
    	    	    if (strchr(tmp+1, '*') == NULL &&
    	    	    	    strchr(tmp+1, '%') == NULL) {
    	    	    	strcat(tmp+1, "*");
    	    	    	cp = tmp;
    	    	    } else {
    	    	    	cp = tmp+1;
    	    	    }
    	    	    insert_header(cp, excfq.tail, 0);
    	    	}
    	    }

    	    if (cli_present("EXCLUDE.SUBJECT") == CLI$_PRESENT) {
    	    	while(OK(cli_get_value("EXCLUDE.SUBJECT", tmp+1, sizeof(tmp)-2))) {
    	    	    upcase(tmp+1);
    	    	    if (strchr(tmp+1, '*') == NULL &&
    	    	    	    strchr(tmp+1, '%') == NULL) {
    	    	    	strcat(tmp+1, "*");
    	    	    	cp = tmp;
    	    	    } else {
    	    	    	cp = tmp+1;
    	    	    }
    	    	    insert_header(cp, excsq.tail, 0);
    	    	}
    	    }
    	}


    	if (cli_present("KEYWORD") == CLI$_PRESENT) {
    	    while(OK(cli_get_value("KEYWORD", tmp+1, sizeof(tmp)-2))) {
    	    	upcase(tmp+1);
    	    	if (strchr(tmp+1, '*') == NULL && strchr(tmp+1, '%') == NULL) {
    	    	    strcat(tmp+1, "*");
    	    	    cp = tmp;
    	    	} else {
    	    	    cp = tmp+1;
    	    	}
    	    	insert_header(cp, kwque.tail, 0);
    	    }
    	}

    	if (cli_present("FROM") == CLI$_PRESENT) {
    	    while(OK(cli_get_value("FROM", tmp+1, sizeof(tmp)-2))) {
    	    	upcase(tmp+1);
    	    	if (strchr(tmp+1, '*') == NULL && strchr(tmp+1, '%') == NULL) {
    	    	    strcat(tmp+1, "*");
    	    	    cp = tmp;
    	    	} else {
    	    	    cp = tmp+1;
    	    	}
    	    	insert_header(cp, authq.tail, 0);
    	    }
    	}

    	if (cli_present("SUBJECT") == CLI$_PRESENT) {
    	    while(OK(cli_get_value("SUBJECT", tmp+1, sizeof(tmp)-2))) {
    	    	upcase(tmp+1);
    	    	if (strchr(tmp+1, '*') == NULL && strchr(tmp+1, '%') == NULL) {
    	    	    strcat(tmp+1, "*");
    	    	    cp = tmp;
    	    	} else {
    	    	    cp = tmp+1;
    	    	}
    	    	insert_header(cp, subjq.tail, 0);
    	    }
    	}

    	if (cli_present("ARTRNG") == CLI$_PRESENT) {
    	    cli_get_value("ARTRNG", tmp, sizeof(tmp));
    	    Parse_Range(tmp, &current, &final, cur_artnum, curgroup);
    	    if (final == current) final = curgroup->lastavl;
    	    if (final < current) {
    	    	junk1 = final;
    	    	final = current;
    	    	current = junk1;
    	    }
    	} else {
    	    current = cur_artnum;
    	    final = curgroup->lastavl;
    	}
    	if (current < curgroup->frstavl) current = curgroup->frstavl;
    	if (current > curgroup->lastavl) current = curgroup->lastavl;
    	if (dounseen && !doseen) {
    	    junk1 = Next_Unseen_Article(current-1, 1, curgroup);
    	    if (junk1 == 0) {
    	    	cmd_dirclup();
    	    	return SS$_NORMAL;
    	    }
    	    current = junk1;
    	}
    	first = current;
    	if (by_thread) {
    	    thread_track = malloc(final-first+1);
    	    if (thread_track != 0) memset(thread_track, 0, final-first+1);
    	} else thread_track = 0;
    	thread_base = 0;
    	print_one = 0;

    }

/*
** Just loop through all the articles in the group (starting with the
** specified one, if there was one).
*/
    while (1) {
    	if (current > final) {
    	    if (by_thread) {
    	    	if (thread_base == 0) break;
    	    	else {
    	    	    for (current = thread_base + 1;
    	    	    	    current <= final && thread_track[current-first];
    	    	    	    current++);
    	    	    if (current > final) break;
    	    	    thread_base = 0;
    	    	}
    	    } else break;
    	}

/*
** They can use ctrl/c to break out of this, since fetching over all
** those article headers can be time-consuming.
*/
    	if (User_Interrupt) break;

/*
** Figure out whether we've seen this article or not.
*/
    	seen = Article_Seen(curgroup, current);
    	doit = (doseen && seen) || (dounseen && !seen);

/*
** Only display if it is (or may be) a valid article
*/
    	if (doit) doit = Is_Valid_Article(curgroup, current);
/*
** Check to see if we're ignoring this article.  If so, skip it.
*/
    	if (doit) doit = !Ignore_Article(curgroup, current);

/*
**  Threading check
*/
    	if (doit && by_thread) {
    	    struct HDR *shdr, *mhdr, *rhdr;

    	    shdr = rhdr = mhdr = 0;
    	    if (by_thread == 1) Get_One_Header(curgroup, current, NEWS_K_HDR_SUBJECT, &shdr);
    	    Get_One_Header(curgroup, current, NEWS_K_HDR_REFERENCES, &rhdr);
    	    if (rhdr == 0) Get_One_Header(curgroup, current, NEWS_K_HDR_MESSAGE_ID, &mhdr);
    	    if (thread_base == 0) {
    	    	thread_base = current;
    	    	thread_msgid[0] = '\0';
    	    	thread_subject[0] = '\0';
    	    	if (by_thread == 1 && shdr != 0) {
    	    	    strcpy(tmp, shdr->str);
    	    	    upcase(tmp);
    	    	    if (strncmp(tmp, "RE:", 3) == 0) {
    	    	    	cp = tmp + 3;
    	    	    } else cp = tmp;
    	    	    while (isspace(*cp)) cp++;
    	    	    strcpy(thread_subject, cp);
    	    	    for (cp = thread_subject+strlen(thread_subject); cp > thread_subject
    	    	    	    	    && isspace(*(cp-1)); cp--);
    	    	    *cp = '\0'; 
    	    	}
    	    	if (rhdr != 0) strcpy(thread_msgid, rhdr->str);
    	    	else if (mhdr != 0) strcpy(thread_msgid, mhdr->str);
    	    } else {
    	    	if (rhdr == 0 && mhdr == 0 && by_thread > 1) doit = 0;
    	    	if (doit) {
    	    	    if (rhdr != 0) {
    	    	    	if (!Thread_Match(rhdr->str, thread_msgid)) {
    	    	    	    if (mhdr != 0) {
    	    	    	    	if (!Thread_Match(thread_msgid, mhdr->str)) {
    	    	    	    	    if (by_thread > 1) doit = 0;
    	    	    	    	}
    	    	    	    } else if (by_thread > 1) doit = 0;
    	    	    	}
    	    	    } else {
    	    	    	if (mhdr != 0) {
    	    	    	    if (!Thread_Match(thread_msgid, mhdr->str)) {
    	    	    	    	if (by_thread > 1) doit = 0;
    	    	    	    }
    	    	    	} else if (by_thread > 1) doit = 0;
    	    	    }
    	    	}

    	    	if (doit && by_thread == 1) {
    	    	    if (shdr == 0) doit = 0;
    	    	    else {
    	    	    	strcpy(tmp, shdr->str);
    	    	    	upcase(tmp);
    	    	    	doit = (strstr(tmp, thread_subject) != 0) ||
    	    	    	    	       (strstr(thread_subject, tmp) != 0);
    	    	    }
    	    	}
    	    }
    	}

/*
**  Keep track of whether we've done this one
*/
    	if (doit && by_thread && thread_track != 0)
    	    thread_track[current-first] = 1;
/*
** Fetch the headers we're interested in from the header list
*/
    	if (doit) {
    	    int i;
    	    if (OK(Get_One_Header(curgroup, current, NEWS_K_HDR_FROM, &hdr)))
    	    if (hdr != 0) {
    	    	fromstr = hdr->str;
    	    	strcpy(upfrom, fromstr);
    	    	upcase(upfrom);

    	    	upsubj[0] = '\0';
    	    	subj = 0;
    	    	if (OK(Get_One_Header(curgroup, current, NEWS_K_HDR_SUBJECT, &hdr))) {
    	    	    if (hdr != 0) {
    	    	    	subj = hdr->str;
    	    	    	strcpy(upsubj, subj);
    	    	    	upcase(upsubj);
    	    	    } else {
    	    	    	subj = "(none)";
    	    	    	strcpy(upsubj, "(NONE)");
    	    	    }
    	    	} else {
    	    	    subj = "(none)";
    	    	    strcpy(upsubj, "(NONE)");
    	    	}

    	    	upkw[0] = '\0';
    	    	if (kwque.head != &kwque &&
    	    	    	OK(Get_One_Header(curgroup, current, NEWS_K_HDR_KEYWORDS, &hdr))) {
    	    	    if (hdr != 0) {
    	    	    	strcpy(upkw, hdr->str);
    	    	    	upcase(upkw);
    	    	    }
    	    	}

    	    	if (OK(Get_One_Header(curgroup, current, NEWS_K_HDR_LINES, &hdr))) {
    	    	    if (hdr != 0) {
    	    	    	i = strlen(hdr->str);
    	    	    	if (i > 5) {
    	    	    	    strcpy(lines, "*****");
    	    	    	} else {
    	    	    	    strcpy(lines, "     ");
    	    	    	    strncpy(lines+5-i, hdr->str, i);
    	    	    	}
    	    	    } else {
    	    	    	strcpy(lines, "     ");
    	    	    }
    	    	} else {
    	    	    strcpy(lines, "     ");
    	    	}
/*
** Check the /KEYWORDS list to see if we have a match
*/
    	    	doit = kwque.head == &kwque;
    	    	for (hdr = kwque.head; hdr != (struct HDR *) &kwque;
    	    	    	    	hdr = hdr->flink) {
    	    	    if (strstr(upkw, hdr->str)) {
    	    	    	doit = 1;
    	    	    	break;
    	    	    }
    	    	}

/*
** Check thge /FROM list to see if we have a match
*/
    	    	if (doit) {
    	    	    doit = authq.head == &authq;
    	    	    INIT_SDESC(cand, strlen(upfrom), upfrom);
    	    	    for (hdr = authq.head; hdr != (struct HDR *) &authq;
    	    	    	    	hdr = hdr->flink) {
    	    	    	INIT_SDESC(pattern, strlen(hdr->str), hdr->str);
    	    	    	if (OK(str$match_wild(&cand, &pattern))) {
    	    	    	    doit = 1;
    	    	    	    break;
    	    	    	}
    	    	    }
    	    	}

/*
** Check the /EXCLUDE=FROM list to see if we should skip
*/
    	    	if (doit) {
    	    	    INIT_SDESC(cand, strlen(upfrom), upfrom);
    	    	    for (hdr = excfq.head; hdr != (struct HDR *) &excfq;
    	    	    	    	hdr = hdr->flink) {
    	    	    	INIT_SDESC(pattern, strlen(hdr->str), hdr->str);
    	    	    	if (OK(str$match_wild(&cand, &pattern))) {
    	    	    	    doit = 0;
    	    	    	    break;
    	    	    	}
    	    	    }
    	    	}

/*
** Check the /SUBJECT list to see if we have a match
*/
    	    	if (doit) {
    	    	    doit = subjq.head == &subjq;
    	    	    INIT_SDESC(cand, strlen(upsubj), upsubj);
    	    	    for (hdr = subjq.head; hdr != (struct HDR *) &subjq;
    	    	    	    	hdr = hdr->flink) {
    	    	    	INIT_SDESC(pattern, strlen(hdr->str), hdr->str);
    	    	    	if (OK(str$match_wild(&cand, &pattern))) {
    	    	    	    doit = 1;
    	    	    	    break;
    	    	    	}
    	    	    }
    	    	}

/*
** Check the /EXCLUDE=SUBJECT list to see if we should skip
*/
    	    	if (doit) {
    	    	    INIT_SDESC(cand, strlen(upsubj), upsubj);
    	    	    for (hdr = excsq.head; hdr != (struct HDR *) &excsq;
    	    	    	    	hdr = hdr->flink) {
    	    	    	INIT_SDESC(pattern, strlen(hdr->str), hdr->str);
    	    	    	if (OK(str$match_wild(&cand, &pattern))) {
    	    	    	    doit = 0;
    	    	    	    break;
    	    	    	}
    	    	    }
    	    	}

/*
** Ok, we really should display this.  Took long enough to get here.
** Set up the formatting string and display it.
*/
    	    	if (doit) {
    	    	    static $DESCRIPTOR(ctrstr, "!1AD!AD !6UL!AD !5AD !#AD !#AD");
    	    	    static $DESCRIPTOR(hctrstr, "articles [!UL,!UL] (!UL unseen)");
    	    	    DESCRIP hdsc;
    	    	    char htmp[STRING_SIZE], htmp2[STRING_SIZE];
    	    	    unsigned short hlen;

    	    	    if (!print_one) {
    	    	    	print_one = 1;
    	    	    	strcpy(tmp, "   Article  Lines ");
    	    	    	cp = tmp+17;
    	    	    	if (fromwid < 6 && fromwid > 0) {
    	    	    	    strncpy(cp, "From  ", fromwid+2);
    	    	    	    cp += fromwid+2;
    	    	    	} else if (fromwid > 0) {
    	    	    	    int i;
    	    	    	    for (i = fromwid/2-2; i > 0; i--) *cp++ = ' ';
    	    	    	    strcpy(cp, "From");
    	    	    	    cp += 4;
    	    	    	    for (i = fromwid-(fromwid/2-2)-2; i>0; i--) *cp++ = ' ';
    	    	    	}
    	    	    	if (subjwid < 9 && subjwid > 0) {
    	    	    	    strncpy(cp, "Subject  ", subjwid+2);
    	    	    	    cp += subjwid+2;
    	    	    	} else if (subjwid > 0) {
    	    	    	    int i;
    	    	    	    for (i = subjwid/2-3; i > 0; i--) *cp++ = ' ';
    	    	    	    strcpy(cp, "Subject");
    	    	    	    cp += 4;
    	    	    	    for (i = subjwid-(subjwid/2-2)-5; i>0; i--) *cp++ = ' ';
    	    	    	}
    	    	    	*cp = '\0';
    	    	    	INIT_SDESC(hdsc, sizeof(htmp)-1, htmp);
    	    	    	if (OK(sys$fao(&hctrstr, &hlen, &hdsc, curgroup->frstavl,
    	    	    	    	curgroup->lastavl, Count_Unseen(curgroup)))) {
    	    	    	    int i = strlen(curgroup->grpnam);
    	    	    	    htmp[hlen] = '\0';
    	    	    	    if (hlen+i < pb_cols) {
    	    	    	    	strcpy(htmp2, curgroup->grpnam);
    	    	    	    	for (cp = htmp2+i; cp < htmp2+(pb_cols-hlen);
    	    	    	    	    cp++) *cp = ' ';
    	    	    	    	strcpy(cp, htmp);
    	    	    	    } else strcpy(htmp2, curgroup->grpnam);
    	    	    	} else strcpy(htmp2, curgroup->grpnam);
    	    	    	Begin_Paged_Output(htmp2, tmp);
    	    	    }
    	    	    INIT_SDESC(sdsc, sizeof(tmp)-1, tmp);
    	    	    sys$fao(&ctrstr, &tlen, &sdsc, 1, (seen ? "*" : " "),
    	    	    	(by_thread ? (current == thread_base ? 0 : 1) : 0), ">",
    	    	    	current,
    	    	    	(by_thread ? (current == thread_base ? 1 : 0) : 1), " ",
    	    	    	strlen(lines), lines, fromwid, strlen(fromstr),
    	    	    	fromstr, subjwid, strlen(subj), subj);
    	    	    *(tmp+tlen) = '\0';
    	    	    if (!Put_Paged(tmp, -1)) {
    	    	    	default_action = cmd_dir;
    	    	    	cleanup_action = cmd_dirclup;
    	    	    	dir_in_progress = 1;
    	    	    	return SS$_NORMAL;
    	    	    }
    	    	}
    	    }
    	}

    	if (by_thread) {
    	    do {current += 1;} while (current <= final && thread_track[current-first]);
    	} else {
    	    current += 1;
    	}

    }

/*
** We get here when we run out of articles.  Flush any pending output.
*/
    if (print_one) {
    	if (!Paged_Output_Done() && !User_Interrupt) {
    	    default_action = cmd_dir;
    	    cleanup_action = cmd_dirclup;
    	    dir_in_progress = 2;
    	    return SS$_NORMAL;
    	} else {
    	    Finish_Paged_Output(0);
    	}
    }

    cmd_dirclup();

    return SS$_NORMAL;

} /* cmd_dir */

/*
**++
**  ROUTINE:	cmd_dirclup
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Cleans up the static context of a pending directory listing.
**  Invoked through the cleanup_action mechanism.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_dirclup()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Always returned.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static unsigned int cmd_dirclup() {

    struct HDR *hdr;

    if (dir_in_progress > 0) {
    	dir_in_progress = 0;
    	current = 0;
    	Finish_Paged_Output(0);
    	while (queue_remove(kwque.head, &hdr)) mem_freehdr(hdr);
    	while (queue_remove(authq.head, &hdr)) mem_freehdr(hdr);
    	while (queue_remove(subjq.head, &hdr)) mem_freehdr(hdr);
    	while (queue_remove(excfq.head, &hdr)) mem_freehdr(hdr);
    	while (queue_remove(excsq.head, &hdr)) mem_freehdr(hdr);
    	default_action = cleanup_action = NULL;
    	if (thread_track != 0) {
    	    free(thread_track);
    	    thread_track = 0;
    	}
    }

    return SS$_NORMAL;

} /* cmd_dirclup */
