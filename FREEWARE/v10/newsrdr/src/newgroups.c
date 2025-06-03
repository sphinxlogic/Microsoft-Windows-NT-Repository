/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Handles newgroups processing.
**
**  MODULE DESCRIPTION:
**
**  	tbs
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  16-APR-1993
**
**  MODIFICATION HISTORY:
**
**  	16-APR-1993 V1.0    Madison 	Initial coding.
**  	23-APR-1993 V1.0-1  Madison 	Support for NEWGROUP_ACTION.
**  	24-SEP-1993 V1.1    Madison 	mem_getgrp() now inits queues.
**--
*/
#include "newsrdr.h"
#include "globals.h"

/*
**  Forward declarations
*/
    void Check_New_Groups(int);

/*
**  External references
*/
    extern void Insert_Group(struct GRP *);
    extern struct GRP *Find_Group(char *);
    extern unsigned int parse_group_reply(char *, int *, int *, int *);
    extern unsigned int Parse_List_Reply(char *, struct GRP *);
    

/*
**++
**  ROUTINE:	Check_New_Groups
**
**  FUNCTIONAL DESCRIPTION:
**
**  	tbs
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	tbs
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void Check_New_Groups(int Xaction) {

    struct QUE newgq;
    struct GRP *g, *g2;
    char tmp[STRING_SIZE];
    int tlen, reply_code, action;
    int count;

    if (news_cfg.newgroups == 0) return;

    action = (Xaction < 0) ? news_prof.ngaction : Xaction;

    newgq.head = newgq.tail = &newgq;
/*
** If we've never done a newgroups check, let's not do one now.
*/
    if (news_prof.lastgrpchk.long1==0 && news_prof.lastgrpchk.long2==0) {
    	    sys$gettim(&news_prof.lastgrpchk);
    	    return;
    }

    if (news_cfg.newgroups == 1) {
    	strcpy(tmp, "NEWGROUPS ");
    	make_nntp_dtstring(&news_prof.lastgrpchk, tmp+10, sizeof(tmp)-10);
    } else {
    	strcpy(tmp, "LIST");
    }

    count = 0;

    server_send(tmp);
    server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);
    if (reply_code == NNTP__NGLSTFOLLOWS || reply_code == NNTP__GLISTFOLLOWS) {

    	sys$gettim(&news_prof.lastgrpchk);
    	while (server_get_line(tmp, sizeof(tmp)-1, &tlen) != NEWS__EOLIST) {
    	    char *sppos;

    	    g = mem_getgrp();
    	    g->avail = 1;
    	    if (news_cfg.newgroups == 1) {
    	    	tmp[tlen] = '\0';
    	    	sppos = strchr(tmp, ' ');
    	    	if (sppos == NULL) {
    	    	    if (tlen > GRP_S_GRPNAM) tmp[GRP_S_GRPNAM] = '\0';
    	    	    strcpy(g->grpnam, tmp);
    	    	} else {
    	    	    int len = sppos-tmp;
    	    	    if (len > GRP_S_GRPNAM) len = GRP_S_GRPNAM;
    	    	    strncpy(g->grpnam, tmp, len);
    	    	    g->grpnam[len] = '\0';
    	    	}
    	    } else {
    	    	if (!OK(Parse_List_Reply(tmp, g))) {
    	    	    mem_freegrp(g);
    	    	    continue;
    	    	}
    	    }
    	    g2 = Find_Group(g->grpnam);
    	    if (g2 == NULL) {
    	    	queue_insert(g, newgq.tail);
    	    	count++;
    	    } else {
    	    	mem_freegrp(g);
    	    }
    	}
    }
/*
**  If there are a lot of new groups and the newgroup action is not NONE,
**  prompt to make sure they really want to do this.
*/
    if (count > 10 && action != NG_K_NONE) {
    	sprintf(tmp, "There are %d new newsgroups.  Do you really want to %s?",
    	    count, (action == NG_K_SUBSCRIBE ? "subscribe to them" :
    	    	    	"be prompted to subscribe to them"));
    	put_output(tmp);
    	if (!Yes_Answer("Enter YES to continue, or NO to skip:", 0))
    	    action = NG_K_NONE;
    }

    while (queue_remove(newgq.head, &g)) {
    	sprintf(tmp, "GROUP %s", g->grpnam);
    	server_send(tmp);
    	server_get_reply(SRV__NOECHO, &reply_code, tmp, sizeof(tmp), 0);
    	if (reply_code == NNTP__GRPSELECTED) {
    	    int article_count;
    	    g->seenq.head = g->seenq.tail = &g->seenq;
    	    g->killq.head = g->killq.tail = &g->killq;
    	    g->avail = 1;
    	    parse_group_reply(tmp, &article_count, &g->frstavl, &g->lastavl);
    	    Insert_Group(g);
    	    lib$signal(NEWS__NEWGROUP, 2, strlen(g->grpnam), g->grpnam);
    	    if (news_prof.glist) {
    	    	news_prof.glast->next = g;
    	    	news_prof.glast = g;
    	    }
    	    switch (action) {
    	    case NG_K_SUBSCRIBE:
    	    	g->subscribed = 1;
    	    	break;
    	    case NG_K_PROMPT:
    	    	sprintf(tmp, "Subscribe to %s?", g->grpnam);
    	    	g->subscribed = Yes_Answer(tmp, 0);
    	    	break;
    	    default:
    	    	break;
    	    }
    	} else {
    	    mem_freegrp(g);
    	}
    }

    return;

} /* Check_New_Groups */
