/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	SET and SHOW commands.
**
**  MODULE DESCRIPTION:
**
**  	This module contains most, if not all, of the SET commands,
**  and the SHOW command.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, 1994  MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  09-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	09-SEP-1992 V1.0    Madison 	Initial coding.
**  	11-SEP-1992 V1.0-1  Madison 	Added SET NEWSRC.
**  	12-SEP-1992 V1.0-2  Madison 	Added SET DIR, SET QUE, SET FORM.
**  	17-SEP-1992 V1.0-3  Madison 	Fixed problem in SET IGNORE.
**  	28-SEP-1992 V1.0-4  Madison 	Fixed big SET HEADER-related problems.
**  	07-OCT-1992 V1.0-5  Madison 	Fix SET SEEN/UNSEEN range problem.
**  	08-OCT-1992 V1.0-6  Madison 	Added SHOW VERSION.
**  	15-FEB-1993 V1.0-7  Madison 	Make compatible with pre-VMS V5.2.
**  	15-FEB-1993 V1.1    Madison 	Added SET SEEN/CROSS.
**  	12-APR-1993 V1.2    Madison 	Get_Article_* changes, SET AUTO_READ.
**  	23-APR-1993 V1.2-1  Madison 	Add FORCE_MAIL_PROTOCOL support.
**  	23-MAY-1993 V1.2-2  Madison 	Allow SET SEEN/THREAD w/range.
**  	22-JUN-1993 V1.3    Madison 	Add SET AUTO_SAVE_PROFILE
**  	09-OCT-1993 V1.3-1  Madison 	Fixed MARKCOUNT message.
**  	16-MAR-1994 V1.4    Madison 	Added SET IGNORE/MESSAGE_ID.
**  	16-MAY-1994 V1.5    Madison 	Customizable REPLY prefix.
**--
*/
#include "newsrdr.h"
#include "globals.h"
#ifdef __GNUC__
#include <vms/fscndef.h>
#include <vms/rmsdef.h>
#else
#include <fscndef.h>
#include <rmsdef.h>
#endif

    EXTERN struct GRP *curgroup;
    EXTERN int        cur_artnum;
    EXTERN int        pb_cols;
    EXTERN int	      User_Interrupt;
    EXTERN char       *$$$Version;

    static char *hdrname[] = {"",
    	"FROM", "DATE", "NEWSGROUPS", "SUBJECT", "MESSAGE_ID",
    	"PATH", "REPLY_TO", "SENDER", "FOLLOWUP_TO", "EXPIRES",
    	"REFERENCES", "CONTROL", "DISTRIBUTION", "ORGANIZATION",
    	"KEYWORDS", "SUMMARY", "APPROVED", "LINES", "XREF", "OTHER"};

    extern int Mark_Article_Seen(struct GRP *, int);
    extern int Mark_Article_Unseen(struct GRP *, int);
    extern int Ignore_Article(struct GRP *, int);
    extern unsigned int Parse_Range(char *, int *, int *, int, struct GRP *);
    extern struct GRP *Find_Group(char *);
    extern struct GRP *Find_Group_Wild(char *, unsigned int *);
    extern unsigned int Traverse_Finish(unsigned int *);
    extern int Count_Unseen(struct GRP *);
    extern unsigned int cmd_skipsubj(void);
    extern unsigned int cmd_skipthread(void);
    extern void Mark_Xref(char *, int);
    extern unsigned int Get_Article_Hdrs(struct GRP *, int,
    	    	    	    struct QUE **, int, int *);
    extern void Get_Mail_Personal_Name(char *, int);

/*
**++
**  ROUTINE:	cmd_set_headers
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET HEADERS command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_headers()
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_headers() {

    char tmp[STRING_SIZE], *cp;
    struct HDR *h, *h2;
    int remove, hdrcode;
    struct QUE *hq;

    hq = cli_present("EXTRACT") == CLI$_PRESENT ? &news_prof.ehdrlist :
    	    	&news_prof.hdrlist;

    while (OK(cli_get_value("HDRNAM", tmp, sizeof(tmp)))) {
    	cp = tmp;
    	remove = strncmp(tmp, "NO", 2) == 0;
    	if (remove) cp += 2;
    	for (hdrcode = 1; hdrcode < sizeof(hdrname)/sizeof(char *); hdrcode++) {
    	    if (strstr(hdrname[hdrcode], cp) == hdrname[hdrcode]) break;
    	}
    	for (h = hq->head; h != (struct HDR *) hq; h = h->flink) {
    	    if (h->code >= hdrcode) break;
    	}
    	if (h == (struct HDR *) hq || h->code != hdrcode) {
    	    if (remove) {
    	    	lib$signal(NEWS__HDRNOTFOUND, 2, strlen(cp), cp);
    	    } else {
    	    	h2 = mem_gethdr(0);
    	    	h2->code = hdrcode;
    	    	queue_insert(h2, (h == (struct HDR *) hq ? hq->tail : h->blink));
    	    }
    	} else {
    	    if (remove) {
    	    	queue_remove(h, &h2);
    	    	mem_freehdr(h2);
    	    } else {
    	    	lib$signal(NEWS__HDRALRDY, 2, strlen(cp), cp);
    	    }
    	}
    }

    return SS$_NORMAL;

} /* cmd_set_headers */

/*
**++
**  ROUTINE:	cmd_set_reply
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET REPLY command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_reply()
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_reply() {

    char tmp[STRING_SIZE];
    int i;
    unsigned int status;
    static struct {
    	int *flag;
    	char *tag;
    } pm[2] = {{&news_prof.replypost,"POST"},{&news_prof.replymail,"MAIL"}};


    if (cli_present("REPLYTYPE") == CLI$_PRESENT) {
    	news_prof.replypost = news_prof.replymail = 0;
    	while (OK(cli_get_value("REPLYTYPE", tmp, sizeof(tmp)))) {
    	    for (i = 0; i < 2; i++) {
    	    	if (strstr(pm[i].tag, tmp) == pm[i].tag) *(pm[i].flag) = 1;
    	    }
    	}
    }

    status = cli_present("PREFIX");
    if (status == CLI$_PRESENT) {
    	cli_get_value("PREFIX", tmp, sizeof(tmp));
    	i = strlen(tmp);
    	if (i > sizeof(news_prof.reply_prefix)-1)
    	    i = sizeof(news_prof.reply_prefix)-1;
    	memcpy(news_prof.reply_prefix, tmp, i);
    	news_prof.reply_prefix[i] = '\0';
    	news_prof.reply_prefix_len = i;
    } else if (status == CLI$_NEGATED) {
    	news_prof.reply_prefix[0] = '\0';
    	news_prof.reply_prefix_len = 0;
    }

    return SS$_NORMAL;

} /* cmd_set_reply */

/*
**++
**  ROUTINE:	cmd_set_persnam
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET PERSONAL_NAME command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_persnam()
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_persnam() {

    unsigned int status;

    status = cli_present("PERSONAL_NAME");
    if (status == CLI$_NEGATED) {
    	*news_prof.personalname = '\0';
    	news_prof.usemailpnam = 0;
    } else {
    	if (cli_present("PERSNAM") == CLI$_PRESENT) {
    	    cli_get_value("PERSNAM", news_prof.personalname,
    	    	    sizeof(news_prof.personalname));
    	    news_prof.usemailpnam = 0;
    	} else if (cli_present("MAIL") == CLI$_PRESENT) {
    	    news_prof.usemailpnam = 1;
    	    Get_Mail_Personal_Name(news_prof.personalname,
    	    	    	    	    sizeof(news_prof.personalname));
    	} else {
    	    char tmp[STRING_SIZE];
    	    *tmp = '\0';
    	    get_cmd(tmp, sizeof(tmp), "Name: ");
    	    if (*tmp) {
    	    	strcpy(news_prof.personalname, tmp);
    	    	news_prof.usemailpnam = 0;
    	    }
    	}
    }

    return SS$_NORMAL;

} /* cmd_set_persnam */

/*
**++
**  ROUTINE:	cmd_set_edit
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET EDIT command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_edit()
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_edit() {

    char tmp[STRING_SIZE], edkwd[STRING_SIZE];
    char *edtype[2] = {"CALLABLE","SPAWN"};
    unsigned int status;

    news_prof.edit = cli_present("EDIT") != CLI$_NEGATED;
    if (cli_present("EDITOR") == CLI$_PRESENT) {
    	status = cli_get_value("EDITOR", tmp, sizeof(tmp));
    	if (status == CLI$_COMMA) {
    	    cli_get_value("EDITOR", edkwd, sizeof(edkwd));
    	} else {
    	    strcpy(edkwd, "CALLABLE");
    	}
    	if (strstr(edtype[0],edkwd) == edtype[0]) {
    	    strcat(tmp, "SHR");
    	    status = file_exists(tmp, "SYS$SHARE:.EXE");
    	    *(tmp+strlen(tmp)-3) = '\0';
    	    if (!OK(status)) {
    	    	lib$signal(NEWS__EDNOTFOUND, 2, strlen(tmp), tmp);
    	    } else {
    	    	news_prof.spawnedit = 0;
    	    	strcpy(news_prof.editorname, tmp);
    	    }
    	} else if (strstr(edtype[1],edkwd) == edtype[1]) {
    	    strcpy(news_prof.editorname, tmp);
    	    news_prof.spawnedit = 1;
    	} else {
    	    lib$signal(NEWS__CMDERR, 0, NEWS__KWDSYNTAX, 2, strlen(edkwd), edkwd);
    	}
    }

    return SS$_NORMAL;

} /* cmd_set_edit */

/*
**++
**  ROUTINE:	cmd_set_seen
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET SEEN/UNSEEN commands.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_seen()
**
**  IMPLICIT INPUTS:	curgroup, cur_artnum
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	curgroup->seenq.
**
**--
*/
unsigned int cmd_set_seen() {

    char tmp[STRING_SIZE], *cp;
    char thread_subj[STRING_SIZE];
    struct RNG *r;
    struct QUE *hdrqptr;
    struct HDR *hdr, *xhdr;
    int unsee, rstart, rend, mark_xref;
    int count, mark_thread;

/*
**  SET SEEN/SUBJECT is handled by cmd_skipsubj.
*/
    if (cli_present("SUBJECT") == CLI$_PRESENT) {
    	return cmd_skipsubj();
    }

    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }

    unsee = (cli_present("SEEN") == CLI$_NEGATED) ||
    	    (cli_present("UNSEEN") == CLI$_PRESENT);

    mark_xref = (cli_present("CROSS_REFERENCES") == CLI$_PRESENT);
    mark_thread = (cli_present("THREAD") == CLI$_PRESENT);
    count = 0;

    if (cli_present("ALL") == CLI$_PRESENT) {

    	while (queue_remove(curgroup->seenq.head, &r)) {
    	    mem_freerng(r);
    	}
    	if (!unsee) {
    	    r = mem_getrng();
    	    r->first = 1;
    	    r->last = curgroup->lastavl;
    	    queue_insert(r, &(curgroup->seenq));
    	}
    	return SS$_NORMAL;
    }

    if (cli_present("ARTRNG") == CLI$_ABSENT) {
    	if (mark_thread) return cmd_skipthread();
    	if (unsee) Mark_Article_Unseen(curgroup, cur_artnum);
    	if (mark_xref) {
    	    if (OK(Get_Article_Hdrs(curgroup, cur_artnum, &hdrqptr, 1, 0))) {
    	    	for (hdr = hdrqptr->head; hdr != (struct HDR *) hdrqptr;
    	    	    	    	    	    	    	hdr = hdr->flink) {
    	    	    if (hdr->code == NEWS_K_HDR_XREF) {
    	    	    	Mark_Xref(hdr->str, unsee);
    	    	    	break;
    	    	    }
    	    	}
    	    }
    	}
    	return SS$_NORMAL;
    }

    while (OK(cli_get_value("ARTRNG", tmp, sizeof(tmp)))) {
    	if (!OK(Parse_Range(tmp, &rstart, &rend, cur_artnum, curgroup))) {
    	    lib$signal(NEWS__INVARTRNG, 2, strlen(tmp), tmp);
    	} else {
    	    int i, j;
    	    if (rstart > rend) {
    	    	i = rstart;
    	    	rstart = rend;
    	    	rend = i;
    	    }
    	    if (rend > curgroup->lastavl) rend = curgroup->lastavl;
    	    for (i = rstart; i <= rend; i++) {
    	    	if (User_Interrupt) break;
    	    	if (unsee) {
    	    	    count += Mark_Article_Unseen(curgroup, i);
    	    	} else {
    	    	    count += Mark_Article_Seen(curgroup, i);
    	    	}
    	    	if (mark_xref || mark_thread) {
    	    	    xhdr = 0;
    	    	    thread_subj[0] = '\0';
    	    	    if (OK(Get_Article_Hdrs(curgroup, i, &hdrqptr, 1, 0))) {
    	    	    	for (hdr = hdrqptr->head; hdr != (struct HDR *) hdrqptr;
    	    	    	    	    	    	    	    hdr = hdr->flink) {
    	    	    	    if (mark_xref && hdr->code == NEWS_K_HDR_XREF) {
    	    	    	    	xhdr = hdr;
    	    	    	    	if (!mark_thread || thread_subj[0] != '\0') break;
    	    	    	    }
    	    	    	    if (mark_thread && hdr->code == NEWS_K_HDR_SUBJECT) {
    	    	    	    	strcpy(tmp, hdr->str);
    	    	    	    	upcase(tmp);
    	    	    	    	if (tmp[0] == 'R' && tmp[1] == 'E' &&
    	    	    	    	    	tmp[2] == ':') {
    	    	    	    	    for (cp = tmp+3; isspace(*cp); cp++);
    	    	    	    	} else cp = tmp;
    	    	    	    	strcpy(thread_subj, tmp);
    	    	    	    	if (!mark_xref || xhdr != 0) break;
    	    	    	    }
    	    	    	}
    	    	    	if (mark_xref && xhdr != 0) Mark_Xref(xhdr->str, unsee);
    	    	    	if (mark_thread && thread_subj[0] != '\0') {
    	    	    	    for (j = i; j <= curgroup->lastavl; j++) {
    	    	    	    	if (!Ignore_Article(curgroup, j)) {
    	    	    	    	    if (OK(Get_Article_Hdrs(curgroup, j,
    	    	    	    	    	    	&hdrqptr, 1, 0))) {
    	    	    	    	    	xhdr = 0;
    	    	    	    	    	cp = 0;
    	    	    	    	    	for (hdr = hdrqptr->head;
    	    	    	    	    	    	hdr != (struct HDR *) hdrqptr;
    	    	    	    	    	    	hdr = hdr->flink) {
    	    	    	    	    	    if (hdr->code == NEWS_K_HDR_SUBJECT) {
    	    	    	    	    	    	strcpy(tmp, hdr->str);
    	    	    	    	    	    	upcase(tmp);
    	    	    	    	    	    	if (tmp[0] == 'R' &&
    	    	    	    	    	    	    	tmp[1] == 'E' &&
    	    	    	    	    	    	    	tmp[2] == ':') {
    	    	    	    	    	    	    for (cp = tmp; isspace(*cp);
    	    	    	    	    	    	    	    cp++);
    	    	    	    	    	    	} else cp = tmp;
    	    	    	    	    	    	if (!mark_xref || xhdr != 0) break;
    	    	    	    	    	    } else if (hdr->code == NEWS_K_HDR_XREF) {
    	    	    	    	    	    	xhdr = hdr;
    	    	    	    	    	    	if (cp != 0) break;
    	    	    	    	    	    }
    	    	    	    	    	}
    	    	    	    	    	if (strstr(cp, thread_subj)
    	    	    	    	    	    	|| strstr(thread_subj, cp)) {
    	    	    	    	    	    count += (unsee ?
    	    	    	    	    	    	Mark_Article_Unseen(curgroup, j)
    	    	    	    	    	    	: Mark_Article_Seen(curgroup, j));
    	    	    	    	    	    if (mark_xref && xhdr != 0) {
    	    	    	    	    	    	Mark_Xref(xhdr->str, unsee);
    	    	    	    	    	    }
    	    	    	    	    	}
    	    	    	    	    }
    	    	    	    	}
    	    	    	    }
    	    	    	}
    	    	    }
    	    	}
    	    }
    	}
    }

    lib$signal(NEWS__MARKCOUNT, 3, count, (unsee ? 2 : 0), (unsee ? "un" : ""));

    return SS$_NORMAL;

} /* cmd_set_seen */

/*
**++
**  ROUTINE:	cmd_set_sig
**
**  FUNCTIONAL DESCRIPTION:
**                              
**  	SET SIGNATURE command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_sig()
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_sig() {

    char tmp[STRING_SIZE];
    unsigned int status;

    if (cli_present("SIGNATURE_FILE") == CLI$_NEGATED) {
    	*news_prof.sigfile = '\0';
    	news_prof.autosigmail = news_prof.autosigpost = 0;
    	return SS$_NORMAL;
    }

    if (cli_present("SIGFILE") == CLI$_ABSENT) {
    	*tmp = '\0';
    	get_cmd(tmp, sizeof(tmp), "Name of signature file: ");
    	if (!*tmp) return SS$_NORMAL;
    	strcpy(news_prof.sigfile, tmp);
    } else {
    	cli_get_value("SIGFILE", news_prof.sigfile, sizeof(news_prof.sigfile));
    }

    status = cli_present("AUTOMATIC");
    if (status == CLI$_NEGATED) {
    	news_prof.autosigpost = news_prof.autosigmail = 0;
    } else if (status == CLI$_PRESENT) {
    	unsigned int statm, statp;
    	statm = cli_present("AUTOMATIC.MAIL");
    	statp = cli_present("AUTOMATIC.POST");
    	news_prof.autosigmail = (statm == CLI$_PRESENT) ||
    	    (statm == CLI$_ABSENT && statp == CLI$_ABSENT);
    	news_prof.autosigpost = (statp == CLI$_PRESENT) ||
    	    (statm == CLI$_ABSENT && statp == CLI$_ABSENT);
    }

    return SS$_NORMAL;

} /* cmd_set_sig */

/*
**++
**  ROUTINE:	cmd_set_archive
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET ARCHIVE command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_archive()
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_archive() {

    char tmp[STRING_SIZE];
    unsigned int flds;
    static struct fscndef fscn = {0,0,0};
    struct dsc$descriptor sdsc;
    unsigned int status;

    if (cli_present("ARCHIVE_DIRECTORY") == CLI$_NEGATED) {
    	*news_prof.archive = '\0';
    	return SS$_NORMAL;
    }

    if (cli_present("ARCHIVE") == CLI$_ABSENT) {
    	*tmp = '\0';
    	get_cmd(tmp, sizeof(tmp), "Name of archive directory: ");
    	if (!*tmp) return SS$_NORMAL;
    } else {
    	cli_get_value("ARCHIVE", tmp, sizeof(tmp));
    }

    while (1) {
    	INIT_SDESC(sdsc, strlen(tmp), tmp);
    	status = sys$filescan(&sdsc, &fscn, &flds);
    	if (!OK(status)) {
    	    lib$signal(NEWS__SETARCHERR, 0, status);
    	    return SS$_NORMAL;
    	}
    	if (flds == FSCN$M_NAME) {
    	    strcat(tmp, ":");
    	} else {
    	    break;
    	}
    }
    if (!((flds & FSCN$M_NODE) || (flds & FSCN$M_DEVICE) ||
    	    (flds & FSCN$M_ROOT) || (flds & FSCN$M_DIRECTORY) ||
    	    (flds & FSCN$M_NAME) || (flds & FSCN$M_TYPE) ||
    	    (flds & FSCN$M_VERSION))) {
    	lib$signal(NEWS__SETARCHERR, 0, RMS$_DIR);
    	return SS$_NORMAL;
    }

    strcpy(news_prof.archive, tmp);

    return SS$_NORMAL;

} /* cmd_set_archive */

/*
**++
**  ROUTINE:	cmd_set_newsrc
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET NEWSRC command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_newsrc
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_newsrc() {

    unsigned int substat, purstat;

    if (cli_present("NEWSRC") == CLI$_NEGATED) {
    	*news_prof.newsrc = '\0';
    	return SS$_NORMAL;
    }

    substat = cli_present("SUBSCRIBED_ONLY");
    purstat = cli_present("PURGE");

    if (cli_present("NEWSRC_FILE") == CLI$_ABSENT) {
    	if (news_prof.newsrc[0] == '\0') {
    	    strcpy(news_prof.newsrc, "SYS$LOGIN:XX.NEWSRC");
    	    news_prof.newsrc_subonly = (substat == CLI$_PRESENT);
    	    news_prof.newsrc_purge = (purstat != CLI$_NEGATED);
    	} else {
    	    if (substat != CLI$_ABSENT)
    	    	news_prof.newsrc_subonly = (substat == CLI$_PRESENT);
    	    if (purstat != CLI$_ABSENT)
    	    	news_prof.newsrc_purge = (purstat == CLI$_PRESENT);
    	}
    } else {
    	cli_get_value("NEWSRC_FILE", news_prof.newsrc, sizeof(news_prof.newsrc));
    	news_prof.newsrc_subonly = (substat == CLI$_PRESENT);
    	news_prof.newsrc_purge = (purstat != CLI$_NEGATED);
    }

    return SS$_NORMAL;

} /* cmd_set_newsrc */

/*
**++
**  ROUTINE:	cmd_set_queue
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET QUEUE command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_queue()
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_queue() {

    cli_get_value("QUENAME", news_prof.print_queue, sizeof(news_prof.print_queue));

    return SS$_NORMAL;

} /* cmd_set_queue */

/*
**++
**  ROUTINE:	cmd_set_form
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET FORM command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_form()
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_form() {

    char tmp[STRING_SIZE];

    if (cli_present("FORM") == CLI$_NEGATED) {
    	*news_prof.print_form = '\0';
    	return SS$_NORMAL;
    }

    if (cli_present("FORMNAME") == CLI$_ABSENT) {
    	*tmp = '\0';
    	get_cmd(tmp, sizeof(tmp), "Form name: ");
    	if (!*tmp) return SS$_NORMAL;
    	strcpy(news_prof.print_form, tmp);
    } else {
    	cli_get_value("FORMNAME", news_prof.print_form, sizeof(news_prof.print_form));
    }

    return SS$_NORMAL;

} /* cmd_set_form */

/*
**++
**  ROUTINE:	cmd_set_dir
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET DIRECTORY command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_dir()
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_dir() {

    char tmp[STRING_SIZE];

    if (cli_present("FROM") == CLI$_PRESENT) {
    	cli_get_value("FROM", tmp, sizeof(tmp));
    	lib$cvt_dtb(strlen(tmp), tmp, &news_prof.dir_fromwid);
    } else {
    	news_prof.dir_fromwid = -1;
    }
    if (cli_present("SUBJECT") == CLI$_PRESENT) {
    	cli_get_value("SUBJECT", tmp, sizeof(tmp));
    	lib$cvt_dtb(strlen(tmp), tmp, &news_prof.dir_subjwid);
    } else {
    	news_prof.dir_subjwid = -1;
    }

    return SS$_NORMAL;

} /* cmd_set_dir */

/*
**++
**  ROUTINE:	cmd_set_ignore
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET IGNORE command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_ignore()
**
**  IMPLICIT INPUTS:	news_prof, curgroup, cur_artnum
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	curgroup->killq could be modified.
**
**--
*/
unsigned int cmd_set_ignore() {

    char tmp[STRING_SIZE];
    struct QUE *q;
    struct HDR *h;
    struct GRP *grp;
    short testcode;
    int remove;

    remove = cli_present("IGNORE") == CLI$_NEGATED;
    if (cli_present("GLOBAL") == CLI$_PRESENT) {
    	q = &news_prof.killq;
    } else {
    	if (cli_present("GRPNAM") == CLI$_PRESENT) {
    	    cli_get_value("GRPNAM", tmp, sizeof(tmp));
    	    locase(tmp);
    	    grp = Find_Group(tmp);
    	    if (grp == NULL) {
    	    	lib$signal(NEWS__NOSUCHGRP, 2, strlen(tmp), tmp);
    	    	return SS$_NORMAL;
    	    }
    	    q = &(grp->killq);
    	} else {
    	    if (curgroup == NULL) {
    	    	lib$signal(NEWS__NOCURGROUP, 0);
    	    	return SS$_NORMAL;
    	    }
    	    q = &(curgroup->killq);
    	}
    }

    *tmp = '\0';
    if (cli_present("SUBJECT") == CLI$_PRESENT) {
    	testcode = NEWS_K_HDR_SUBJECT;
    	cli_get_value("SUBJECT", tmp, sizeof(tmp));
    } else if (cli_present("NEWSGROUPS") == CLI$_PRESENT) {
    	testcode = NEWS_K_HDR_NEWSGROUPS;
    	cli_get_value("NEWSGROUPS", tmp, sizeof(tmp));
    } else if (cli_present("FROM") == CLI$_PRESENT) {
    	testcode = NEWS_K_HDR_FROM;
    	cli_get_value("FROM", tmp, sizeof(tmp));
    } else if (cli_present("MESSAGE_ID") == CLI$_PRESENT) {
    	testcode = NEWS_K_HDR_MESSAGE_ID;
    	cli_get_value("MESSAGE_ID", tmp, sizeof(tmp));
    } else if (remove) {
    	while (queue_remove(q->head, &h)) mem_freehdr(h);
    	return SS$_NORMAL;
    } else {
    	lib$signal(NEWS__NOIGHDSPEC, 0);
    	return SS$_NORMAL;
    }
    upcase(tmp);
    for (h = q->head; h != (struct HDR *) q; h = h->flink) {
    	if (h->code == testcode && strcmp(h->str, tmp) == 0) break;
    }
    if (h == (struct HDR *) q) {
    	if (remove) {
    	    lib$signal(NEWS__NOMATCH, 2, strlen(tmp), tmp);
    	} else {
    	    insert_header(tmp, q->tail, testcode);
    	}
    } else {
    	if (remove) {
    	    queue_remove(h, &h);
    	    mem_freehdr(h);
    	} else {
    	    lib$signal(NEWS__ALRDYIGNORE, 0);
    	}
    }

    return SS$_NORMAL;

} /* cmd_set_ignore */

/*
**++
**  ROUTINE:	cmd_set_arn
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET AUTO_READ_NEW command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_arn()
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_arn() {

    news_prof.autoreadnew = cli_present("AUTO_READ_NEW") != CLI$_NEGATED;

    return SS$_NORMAL;

} /* cmd_set_arn */

/*
**++
**  ROUTINE:	cmd_set_asp
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET AUTO_SAVE_PROFILE command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_asp()
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_asp() {

    news_prof.autosave = cli_present("AUTO_SAVE_PROFILE") != CLI$_NEGATED;

    return SS$_NORMAL;

} /* cmd_set_asp */

/*
**++
**  ROUTINE:	cmd_set_fmp
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET EDIT command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_fmp()
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_fmp() {

    news_prof.force_proto = cli_present("FORCE_MAIL_PROTOCOL") != CLI$_NEGATED;

    return SS$_NORMAL;

} /* cmd_set_fmp */

/*
**++
**  ROUTINE:	cmd_set_nga
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SET EDIT command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_set_nga()
**
**  IMPLICIT INPUTS:	news_prof
**
**  IMPLICIT OUTPUTS:	news_prof
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_set_nga() {

    static char *nganam[] = {"NONE", "PROMPT", "SUBSCRIBE"};
    static int  ngaction[] = {NG_K_NONE, NG_K_PROMPT, NG_K_SUBSCRIBE};
    char tmp[STRING_SIZE];
    int i;

    if (!OK(cli_get_value("NGAKW", tmp, sizeof(tmp)))) return SS$_NORMAL;

    for (i = 0; i < sizeof(nganam)/sizeof(char *); i++) {
    	if (strstr(nganam[i], tmp) == nganam[i]) {
    	    news_prof.ngaction = ngaction[i];
    	    return SS$_NORMAL;
    	}
    }

    /* should never reach here */

    return SS$_NORMAL;

} /* cmd_set_nga */

/*
**++
**  ROUTINE:	cmd_show
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SHOW command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_show
**
**  IMPLICIT INPUTS:	news_prof, news_cfg, curgroup, and others.
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
unsigned int cmd_show() {

    static char *shokwd[] = {"HEADERS", "PERSONAL_NAME", "EDITOR", "SERVER",
        "CURRENT_GROUP", "ALL", "SIGNATURE_FILE", "IGNORE",
        "ARCHIVE_DIRECTORY", "GROUP", "REPLY", "NEWSRC", "QUEUE", "FORM",
    	"DIRECTORY_WIDTH", "VERSION", "AUTO_READ_NEW", "FORCE_MAIL_PROTOCOL",
    	"NEWGROUP_ACTION", "AUTO_SAVE_PROFILE"};
    char tmp[STRING_SIZE], tmp2[STRING_SIZE];
    struct dsc$descriptor tdsc;
    short tlen;
    int kwnum;

    INIT_SDESC(tdsc, sizeof(tmp)-1, tmp);
    put_output("");
    while (OK(cli_get_value("SHOWKW", tmp, sizeof(tmp)))) {
    	for (kwnum = 0; kwnum < sizeof(shokwd)/sizeof(char *); kwnum++) {
    	    if (strstr(shokwd[kwnum], tmp) == shokwd[kwnum]) break;
    	}
    	if (kwnum == 15 || kwnum == 5) {
    	    strcpy(tmp, "This is NEWSRDR ");
    	    strcat(tmp, $$$Version);
    	    put_output(tmp);
    	}
    	if (kwnum == 0 || kwnum == 5) {
    	    if (news_prof.hdrlist.head == &news_prof.hdrlist) {
    	    	put_output("No headers selected for interactive display.");
    	    } else {
    	    	char *cp;
    	    	struct HDR *h;
    	    	int max, indent;

    	    	strcpy(tmp, "Interactive display headers: ");
    	    	cp = tmp + (indent = strlen(tmp));
    	    	max = (pb_cols < 20 ? 10 : pb_cols-20);
    	    	for (h = news_prof.hdrlist.head;
    	    	    	h != (struct HDR *) &news_prof.hdrlist; h = h->flink) {
    	    	    if (cp-tmp > max) {
    	    	    	*cp++ = ',';
    	    	    	*cp = '\0';
    	    	    	put_output(tmp);
    	    	    	strcpy(tmp, "         ");
    	    	    	cp = tmp + (indent = 9);
    	    	    }
    	    	    if (cp != tmp+indent) *cp++ = ',';
    	    	    strcpy(cp, hdrname[h->code]);
    	    	    cp += strlen(hdrname[h->code]);
    	    	}
    	    	if (cp != tmp+indent) {
    	    	    *cp = '\0';
    	    	    put_output(tmp);
    	    	}
    	    }

    	    put_output ("");

    	    if (news_prof.ehdrlist.head == &news_prof.ehdrlist) {
    	    	put_output("No headers selected for extracted articles.");
    	    } else {
    	    	char *cp;
    	    	struct HDR *h;
    	    	int max, indent;

    	    	strcpy(tmp, "Extracted article headers: ");
    	    	cp = tmp + (indent = strlen(tmp));
    	    	max = (pb_cols < 20 ? 10 : pb_cols-20);
    	    	for (h = news_prof.ehdrlist.head;
    	    	    	h != (struct HDR *) &news_prof.ehdrlist; h = h->flink) {
    	    	    if (cp-tmp > max) {
    	    	    	*cp++ = ',';
    	    	    	*cp = '\0';
    	    	    	put_output(tmp);
    	    	    	strcpy(tmp, "         ");
    	    	    	cp = tmp + (indent = 9);
    	    	    }
    	    	    if (cp != tmp+indent) *cp++ = ',';
    	    	    strcpy(cp, hdrname[h->code]);
    	    	    cp += strlen(hdrname[h->code]);
    	    	}
    	    	if (cp != tmp+indent) {
    	    	    *cp = '\0';
    	    	    put_output(tmp);
    	    	}
    	    }
    	}

    	if (kwnum == 1 || kwnum == 5) {
    	    if (*news_prof.personalname) {
    	    	strcpy(tmp, "Personal name: ");
    	    	strcat(tmp, news_prof.personalname);
    	    	if (news_prof.usemailpnam) strcat(tmp, "  (from VMS MAIL)");
    	    	put_output(tmp);
    	    } else {
    	    	put_output("No personal name set.");
    	    }
    	}

    	if (kwnum == 2 || kwnum == 5) {
    	    strcpy(tmp, "Editor: ");
    	    strcat(tmp, news_prof.editorname);
    	    if (news_prof.spawnedit) {
    	    	strcat(tmp, " (spawned), ");
    	    } else {
    	    	strcat(tmp, " (callable), ");
    	    }
    	    if (news_prof.edit) {
    	    	strcat(tmp, "used by default when composing messages.");
    	    } else {
    	    	strcat(tmp, "not used by default.");
    	    }
    	    put_output(tmp);
    	}

    	if (kwnum == 10 || kwnum == 5) {
    	    strcpy(tmp, "REPLY command default: ");
    	    if (news_prof.replypost) strcat(tmp, "POST");
    	    if (news_prof.replymail) {
    	    	if (news_prof.replypost) strcat(tmp, " and ");
    	    	strcat(tmp, "MAIL");
    	    }
    	    put_output(tmp);
    	    strcpy(tmp, "Prefix for messages included with REPLY/EXTRACT: ");
    	    if (news_prof.reply_prefix_len == 0) {
    	    	strcat(tmp, "(none)");
    	    } else {
    	    	strcat(tmp, news_prof.reply_prefix);
    	    }
    	    put_output(tmp);
    	}

    	if (kwnum == 3 || kwnum == 5) {
    	    strcpy(tmp, "News server: ");
    	    strcat(tmp, news_cfg.server_name);
    	    put_output(tmp);
    	}

    	if (kwnum == 11 || kwnum == 5) {
    	    if (*news_prof.newsrc) {
    	    	strcpy(tmp, "Newsrc file: ");
    	    	strcat(tmp, news_prof.newsrc);
    	    	if (news_prof.newsrc_subonly) {
    	    	    strcat(tmp, " (subscribed newsgroups only)");
    	    	}
    	    	put_output(tmp);
    	    	strcpy(tmp, "  Old versions of newsrc file will ");
    	    	if (!news_prof.newsrc_purge) strcat(tmp, "NOT ");
    	    	strcat(tmp, "be purged.");
    	    	put_output(tmp);
    	    } else {
    	    	put_output("No newsrc file set.");
    	    }
    	}

    	if (kwnum == 12 || kwnum == 5) {
    	    if (*news_prof.print_queue) {
    	    	strcpy(tmp, "Print queue: ");
    	    	strcat(tmp, news_prof.print_queue);
    	    	put_output(tmp);
    	    } else {
    	    	put_output("Print queue: SYS$PRINT");
    	    }
    	}

    	if (kwnum == 13 || kwnum == 5) {
    	    if (*news_prof.print_form) {
    	    	strcpy(tmp, "Print form: ");
    	    	strcat(tmp, news_prof.print_form);
    	    	put_output(tmp);
    	    } else {
    	    	put_output("No form set for printing.");
    	    }
    	}

    	if (kwnum == 14 || kwnum == 5) {
    	    strcpy(tmp, "Directory field widths: FROM=");
    	    if (news_prof.dir_fromwid >= 0) {
    	    	sprintf(tmp2, "%d", news_prof.dir_fromwid);
    	    	strcat(tmp, tmp2);
    	    } else {
    	    	strcat(tmp, "default");
    	    }
    	    strcat(tmp, ", SUBJECT=");
    	    if (news_prof.dir_subjwid >= 0) {
    	    	sprintf(tmp2, "%d", news_prof.dir_subjwid);
    	    	strcat(tmp, tmp2);
    	    } else {
    	    	strcat(tmp, "default");
    	    }
    	    put_output(tmp);
    	}

    	if (kwnum == 4 || kwnum == 5 || kwnum == 9) {
    	    if (curgroup == NULL) {
    	    	put_output("No current group set.");
    	    } else {
    	    	static $DESCRIPTOR(ctrstr,
    	    	    	    "Articles available: !UL-!UL, !UL unseen.");
    	    	strcpy(tmp, "Current group:      ");
    	    	strcat(tmp, curgroup->grpnam);
    	    	put_output(tmp);
    	    	sys$fao(&ctrstr, &tlen, &tdsc, curgroup->frstavl,
    	    	    curgroup->lastavl, Count_Unseen(curgroup));
    	    	put_output_counted(tmp, tlen);
    	    }
    	}

    	if (kwnum == 6 || kwnum == 5) {
    	    if (*news_prof.sigfile) {
    	    	strcpy(tmp, "Signature file: ");
    	    	strcat(tmp, news_prof.sigfile);
    	    	put_output(tmp);
    	    	if (!(news_prof.autosigmail || news_prof.autosigpost)) {
    	    	    put_output("  Not automatically appended.");
    	    	} else {
    	    	    strcpy(tmp, "  Automatically appended to ");
    	    	    if (news_prof.autosigmail) {
    	    	    	if (news_prof.autosigpost) {
    	    	    	    strcat(tmp, "mail and postings");
    	    	    	} else {
    	    	    	    strcat(tmp, "mail only");
    	    	    	}
    	    	    } else strcat(tmp, "postings only");
    	    	    put_output(tmp);
    	    	}
    	    } else {
    	    	put_output("No signature file set.");
    	    }
    	}

    	if (kwnum == 8 || kwnum == 5) {
    	    if (*news_prof.archive) {
    	    	strcpy(tmp, "Archive directory: ");
    	    	strcat(tmp, news_prof.archive);
    	    	put_output(tmp);
    	    } else {
    	    	put_output("No archive directory set.");
    	    }
    	}

    	if (kwnum == 16 || kwnum == 5) {
    	    if (news_prof.autoreadnew) {
    	    	put_output("Automatically READ/NEW when visiting a new group.");
    	    } else {
    	    	put_output("Pause for command when visiting a new group.");
    	    }
    	}

    	if (kwnum == 17 || kwnum == 5) {
    	    if (news_prof.force_proto) {
    	    	put_output("Forced use of mail protocol prefix, even on local mail.");
    	    } else {
    	    	put_output("Use mail protocol prefix only on network mail.");
    	    }
    	}

    	if (kwnum == 18 || kwnum == 5) {
    	    switch (news_prof.ngaction) {
    	    case NG_K_SUBSCRIBE:
    	    	put_output("Will automatically subscribe to new newsgroups.");
    	    	break;
    	    case NG_K_PROMPT:
    	    	put_output("Will prompt for subscription to new newsgroups.");
    	    	break;
    	    default:
    	    	put_output("Will not automatically subscribe to new newsgroups.");
    	    	break;
    	    }
    	}

    	if (kwnum == 19 || kwnum == 5) {
    	    if (news_prof.autosave) {
    	    	put_output("Will automatically save profile information on newsgroup change.");
    	    } else {
    	    	put_output("Profile will be saved only on program exit.");
    	    }
    	}

    	if (kwnum == 7 || kwnum == 5) {
    	    struct HDR *h;
    	    int doglobal, dolocal, do_list, do_wild;

    	    if (kwnum == 5) {
    	    	doglobal = dolocal = 1;
    	    	do_list = 0;
    	    } else {
    	    	doglobal = cli_present("GLOBAL") == CLI$_PRESENT;
    	    	dolocal = !doglobal || (cli_present("LOCAL") == CLI$_PRESENT);
    	    	do_list = cli_present("GRPNAM") == CLI$_PRESENT;
    	    }
    	    if (doglobal) {
    	    	if (news_prof.killq.head == &news_prof.killq) {
    	    	    put_output("Global ignore strings: none.");
    	    	} else {
    	    	    put_output("Global ignore strings:");
    	    	    for (h = news_prof.killq.head;
    	    	    	    	h != (struct HDR *) &news_prof.killq;
    	    	    	    	h = h->flink) {
    	    	    	strcpy(tmp, "    ");
    	    	    	strcat(tmp, hdrname[h->code]);
    	    	    	strcat(tmp, "=\"");
    	    	    	strcat(tmp, h->str);
    	    	    	strcat(tmp, "\"");
    	    	    	put_output(tmp);
    	    	    }
    	    	}
    	    }

    	    if (dolocal) {
    	    	unsigned int gctx;
    	    	struct GRP *g;
    	    	char gnam[STRING_SIZE];

    	    	g = 0;
    	    	gctx = 0;
    	    	do_wild = 0;
    	    	while (1) {
    	    	    if (do_list) {
    	    	    	if (gctx == 0) {
    	    	    	    if (!OK(cli_get_value("GRPNAM", gnam, sizeof(gnam)))) break;
    	    	    	    locase(gnam);
    	    	    	    do_wild = strchr(gnam, '%') != 0 ||
    	    	    	    	      strchr(gnam, '*') != 0;
    	    	    	    if (do_wild) g = Find_Group_Wild(gnam, &gctx);
    	    	    	    else g = Find_Group(gnam);
    	    	    	} else g = Find_Group_Wild(gnam, &gctx);
    	    	    	if (g == 0) continue;
    	    	    } else {
    	    	    	if (g == curgroup) break;
    	    	    	if (curgroup == NULL) {
    	    	    	    lib$signal(NEWS__NOCURGROUP, 0);
    	    	    	    break;
    	    	    	}
    	    	    	g = curgroup;
    	    	    }
    	    	    strcpy(tmp, "Ignore strings for group ");
    	    	    strcat(tmp, g->grpnam);
    	    	    strcat(tmp, ": ");
    	    	    if (g->killq.head == &g->killq) {
    	    	    	if (!do_wild) {
    	    	    	    strcat(tmp, "none.");
    	    	    	    if (doglobal) put_output(""); doglobal = 1;  /* now just use it to show we should skip a line */
    	    	    	    put_output(tmp);
    	    	    	}
    	    	    } else {
    	    	    	if (doglobal) put_output(""); doglobal = 1;  /* now just use it to show we should skip a line */
    	    	    	put_output(tmp);
    	    	    	for (h = g->killq.head;
    	    	    	    	h != (struct HDR *) &g->killq;
    	    	    	    	h = h->flink) {
    	    	    	    strcpy(tmp, "    ");
    	    	    	    strcat(tmp, hdrname[h->code]);
    	    	    	    strcat(tmp, "=\"");
    	    	    	    strcat(tmp, h->str);
    	    	    	    strcat(tmp, "\"");
    	    	    	    put_output(tmp);
    	    	    	}
    	    	    }
    	    	}
    	    	if (gctx != 0) Traverse_Finish(&gctx);
    	    }
    	}
    } /* while */

    put_output("");

    return SS$_NORMAL;

} /* cmd_show */    	    
