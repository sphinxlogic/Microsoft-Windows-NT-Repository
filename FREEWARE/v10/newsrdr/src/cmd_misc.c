/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Miscellaneous commands.
**
**  MODULE DESCRIPTION:
**
**  	This module contains a bunch of different commands that didn't
**  fit into one of the other CMD_ modules.
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
**  	11-SEP-1992 V1.0-1  Madison 	Added logging to cmd_{un}subscribe.
**  	12-SEP-1992 V1.0-2  Madison 	Added /GOTO on subscribes.
**  	19-SEP-1992 V1.0-3  Madison 	Fixed NOSUCHGRP problem in cmd_sub.
**  	22-SEP-1992 V1.0-4  Madison 	Add hack to work around $FAO weirdness.
**  	15-FEB-1993 V1.0-5  Madison 	Real fix to work around $FAO weirdness.
**  	17-APR-1993 V1.1    Madison 	Add connection check after spawn.
**  	25-MAY-1993 V1.1-1  Madison 	Partial update must set group avail flag.
**  	24-JUN-1993 V1.2    Madison 	Add autosave feature on SPAWN, ATTACH.
**  	24-SEP-1993 V1.3    Madison 	mem_getgrp now inits queues.
**  	28-SEP-1993 V1.3-1  Madison 	Fix LOGICAL_NAMES reference.
**  	16-JUN-1994 V1.3-2  Madison 	Send a GROUP command over after a full update.
**--
*/
#include "newsrdr.h"
#include "globals.h"
#ifdef __GNUC__
#include <vms/jpidef.h>
#include <vms/clidef.h>
#else
#include <jpidef.h>
#include <clidef.h>
#endif

    GLOBAL struct GRP *curgroup;
    static $DESCRIPTOR(nulstr, "");
    static $DESCRIPTOR(newgroup_ctrstr, "  !5UL new !8<article!%S!> in newsgroup !AD");

/*
** We need this structure because LIB$TRAVERSE tree gives us only one
** additional argument to the traversal action routines.  This structure
** is a catchall that handles both SUBSCRIBE and UNSUBSCRIBE.
*/
    struct SUBINFO {
    	int didone, really_didone;
    	int confirm, wildcard, log;
    	struct GRP *g;
    	char *name;
    };

/*
** Forward declarations
*/
    void cmd_initialize(void);
    unsigned int cmd_exit(void), cmd_subscribe(void);
    static unsigned int sub_group(struct GRP *, struct SUBINFO *);
    unsigned int cmd_unsubscribe(void);
    static unsigned int unsub_group(struct GRP *, struct SUBINFO *);
    unsigned int cmd_update(void);
    static unsigned int upd_group(struct GRP *, int *);
    unsigned int cmd_update_full(void);
    unsigned int Do_Full_Update(int);
    static unsigned int print_response(struct GRP *, struct QUE *);
    unsigned int cmd_help(void), cmd_spawn(void), cmd_attach(void);
    unsigned int cmd_defkey(void);

    extern unsigned int cmd_readnew(void);
    extern unsigned int parse_group_reply(char *, int *, int *, int *);
    extern unsigned int Parse_List_Reply(char *, struct GRP *);
    extern void Insert_Group(struct GRP *);
    extern int Count_Unseen(struct GRP *);
    extern struct GRP *Find_Group(char *);
    extern unsigned int Set_Current_Group(struct GRP *);
    extern unsigned int Write_Profile(void);

/*
**++
**  ROUTINE:	cmd_initialize
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Initializes the command processing environment -- specifically,
**  the default_action and cleanup_action pointers, and the current group.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	cmd_initialize()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void cmd_initialize() {

    curgroup = NULL;
    default_action = cmd_readnew;
    cleanup_action = NULL;

} /* cmd_initialize */

/*
**++
**  ROUTINE:	cmd_exit
**
**  FUNCTIONAL DESCRIPTION:
**
**  	EXIT command.  Just returns the appropriate status
**  back to the main routine so it leaves the main command loop.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_exit()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	NEWS__ALLDONE:	Always returned.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_exit() {

    Finish_Paged_Output(1);
    return NEWS__ALLDONE;

} /* cmd_exit */

/*
**++
**  ROUTINE:	cmd_subscribe
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SUBSCRIBE command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_subscribe()
**
**  IMPLICIT INPUTS:	news_prof.
**
**  IMPLICIT OUTPUTS:	news_prof.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_subscribe() {

    char tmp[STRING_SIZE];
    struct SUBINFO info;
    int reply_code, changed_group;
    struct GRP *g;

    info.confirm = cli_present("CONFIRM") == CLI$_PRESENT;
    info.log = cli_present("LOG") != CLI$_ABSENT;
    info.g = NULL;
    changed_group = 0;

/*
** User can specify a list of groups
*/
    while (OK(cli_get_value("GRPNAM", tmp+6, sizeof(tmp)-6))) {
    	locase(tmp+6);
    	info.didone = 0;
    	info.name = tmp+6;

/*
** Find the group that matches, if it's in the tree already
*/
    	lib$traverse_tree(&news_prof.gtree, sub_group, &info);

/*
** We used tmp+6 above so we would have room to prefix the group name
** with the GROUP command to send to the server, just in case we have to.
*/
    	if (!info.didone) {
    	    char tmp2[STRING_SIZE];
    	    memcpy(tmp, "GROUP ", 6);
    	    server_send(tmp);
    	    server_get_reply(SRV__NOECHO, &reply_code, tmp2, sizeof(tmp2), 0);
/*
** If the group is available from the server, but was not in the tree,
** we add it in.
*/
    	    if (reply_code == NNTP__GRPSELECTED) {
    	    	int junk;
    	    	g = mem_getgrp();
    	    	strcpy(g->grpnam, tmp+6);
    	    	g->avail = 1;
    	    	g->subscribed = 1;
    	    	parse_group_reply(tmp2, &junk, &(g->frstavl), &(g->lastavl));
    	    	Insert_Group(g);
    	    	info.didone = 1;
    	    	info.g = g;
    	    	changed_group = 1;
    	    }
    	}

/*
** Group did not exist either in the tree or on the server.
*/
    	if (!info.didone) lib$signal(NEWS__NOSUCHGRP, 2, strlen(tmp+6), tmp+6);

    }

    if (cli_present("GOTO") == CLI$_PRESENT && info.g) {
    	unsigned int status;
    	status = Set_Current_Group(info.g);
    	if (OK(status)) {
    	    lib$signal(NEWS__GRPSET, 2, strlen(curgroup->grpnam),
    	    	    	curgroup->grpnam, NEWS__GRPINFO, 3, curgroup->frstavl,
    	    	    	curgroup->lastavl, Count_Unseen(curgroup));
    	    default_action = cmd_readnew;
    	}
    } else if (changed_group && curgroup) {
    	strcpy(tmp, "GROUP ");	    	    /* must keep curgroup & server */
    	strcat(tmp, curgroup->grpnam);      /* in sync, group-wise         */
    	server_send(tmp);
    	server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);
    }

    return SS$_NORMAL;

} /* cmd_subscribe */

/*
**++
**  ROUTINE:	sub_group
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Routine called through LIB$TRAVERSE_TREE to compare a
**  newsgroup name to the pattern specified by the user on the
**  SUBSCRIBE command.  Also prompts for confirmation and logs
**  the subscription, if desired by the user.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	sub_group(struct GRP *g, struct SUBINFO *info)
**
** g:	    GRP structure, modify, by reference
** info:    SUBINFO structure, modify, by reference
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
static unsigned int sub_group(struct GRP *g, struct SUBINFO *info) {

    struct dsc$descriptor cand, pattern;

    INIT_SDESC(cand, strlen(g->grpnam), g->grpnam);
    INIT_SDESC(pattern, strlen(info->name), info->name);
    if (OK(str$match_wild(&cand, &pattern))) {
    	info->didone = 1;
    	info->g = g;
    	if (g->subscribed) {
    	    lib$signal(NEWS__ALRDYSUB, 2, strlen(g->grpnam), g->grpnam);
    	} else {
    	    if (info->confirm) {
    	    	char tmp[STRING_SIZE];
    	    	strcpy(tmp, "Subscribe to newsgroup ");
    	    	strcat(tmp, g->grpnam);
    	    	strcat(tmp, "?");
    	    	g->subscribed = Yes_Answer(tmp, 1);
    	    } else g->subscribed = 1;
    	    if (info->log && g->subscribed) {
    	    	lib$signal(NEWS__DIDSUB, 2, strlen(g->grpnam), g->grpnam);
    	    }
    	}
    }

    return SS$_NORMAL;

} /* sub_group */

/*
**++
**  ROUTINE:	cmd_unsubscribe
**
**  FUNCTIONAL DESCRIPTION:
**
**  	UNSUBSCRIBE comand.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_unsubscribe()
**
**  IMPLICIT INPUTS:	news_prof.
**
**  IMPLICIT OUTPUTS:	news_prof.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Always returned.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_unsubscribe() {

    char tmp[STRING_SIZE];
    struct SUBINFO info;

    info.confirm = cli_present("CONFIRM") == CLI$_PRESENT;
    info.log = cli_present("LOG") != CLI$_ABSENT;
    if (cli_present("GRPNAM") == CLI$_ABSENT) {
    	if (curgroup == NULL) {
    	    lib$signal(NEWS__NOCURGROUP, 0);
    	} else {
    	    if (info.confirm) {
    	    	char tmp2[STRING_SIZE];
    	    	strcpy(tmp2, "Unsubscribe from newsgroup ");
    	    	strcat(tmp2, curgroup->grpnam);
    	    	strcat(tmp2, "?");
    	    	curgroup->subscribed = !Yes_Answer(tmp2, 0);
    	    } else {
    	    	curgroup->subscribed = 0;
    	    }
    	    curgroup = NULL;
    	}

    	return SS$_NORMAL;

    }
    	    
    while (OK(cli_get_value("GRPNAM", tmp, sizeof(tmp)))) {
    	locase(tmp);
    	info.didone = info.really_didone = 0;
    	info.name = tmp;
    	info.wildcard = strchr(tmp, '*') != NULL || strchr(tmp, '%') != NULL;

    	lib$traverse_tree(&news_prof.gtree, unsub_group, &info);

    	if (!OK(info.didone)) {
    	    lib$signal(NEWS__NOSUCHGRP, 2, strlen(tmp), tmp);
    	} else if (info.wildcard && !info.really_didone) {
    	    lib$signal(NEWS__NOTSUBSCR, 2, strlen(tmp), tmp);
    	}

    }

    return SS$_NORMAL;

} /* cmd_unsubscribe */

/*
**++
**  ROUTINE:	unsub_group
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Tree-traversal action routine that unsubscribes a
**  matching group.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	unsub_group(struct GRP *g, struct SUBINFO *info)
**
** g:	    GRP structure, modify, by reference
** info:    SUBINFO structure, modify, by reference
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
static unsigned int unsub_group(struct GRP *g, struct SUBINFO *info) {

    struct dsc$descriptor cand, pattern;

    INIT_SDESC(cand, strlen(g->grpnam), g->grpnam);
    INIT_SDESC(pattern, strlen(info->name), info->name);
    if (OK(str$match_wild(&cand, &pattern))) {
    	info->didone = 1;
    	if (g->subscribed) {
    	    info->really_didone = 1;
    	    if (info->confirm) {
    	    	char tmp[STRING_SIZE];
    	    	strcpy(tmp, "Unsubscribe from newsgroup ");
    	    	strcat(tmp, g->grpnam);
    	    	strcat(tmp, "?");
    	    	g->subscribed = !Yes_Answer(tmp, 0);
    	    } else g->subscribed = 0;
    	    if (info->log && !g->subscribed) {
    	    	lib$signal(NEWS__DIDUNSUB, 2, strlen(g->grpnam), g->grpnam);
    	    }
    	} else if (!info->wildcard) {
    	    lib$signal(NEWS__NOTSUBSCR, 2, strlen(g->grpnam), g->grpnam);
    	}
    }

    return SS$_NORMAL;

} /* unsub_group */

/*
**++
**  ROUTINE:	cmd_update
**
**  FUNCTIONAL DESCRIPTION:
**
**  	UPDATE/PARTIAL command.  Goes through each subscribed group,
**  does a GROUP command to the server on it, gets the info on articles
**  back.  If it's not the first time we've done a partial update, we
**  disconnect and reconnect to the server, since GROUP info doesn't seem
**  to get updated on the fly while a connection is open.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_update()
**
**  IMPLICIT INPUTS:	None.
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
unsigned int cmd_update() {

    static int first_time = 1;
    struct GRP *g;
    int didone, reply_code;
    char tmp[STRING_SIZE];

    if (!first_time) {
    	server_send("QUIT");
    	server_get_reply(SRV__ECHO, &reply_code, 0, 0, 0);
    	server_disconnect();
    	lib$signal(NEWS__RECONNECTING, 2, strlen(news_cfg.server_name),
    	    	    news_cfg.server_name);
    	server_connect(news_cfg.server_name);
    	server_get_reply(SRV__ECHO, &reply_code, 0, 0, 0);
    	news_cfg.postingok = (reply_code == NNTP__HELOPOSTOK);
    }

    first_time = didone = 0;

    if (news_prof.glist) {
    	struct GRP *g;
    	for (g = news_prof.glist; g != NULL; g = g->next) {
    	    upd_group(g, &didone);
    	}
    } else {
    	lib$traverse_tree(&news_prof.gtree, upd_group, &didone);
    }

    if (curgroup != NULL) {
    	strcpy(tmp, "GROUP ");
    	strcat(tmp, curgroup->grpnam);
    	server_send(tmp);
    	server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);
    }

    if (didone) lib$put_output(&nulstr);

    return SS$_NORMAL;

} /* cmd_update */

/*
**++
**  ROUTINE:	upd_group
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Tree traversal action routine that performs the group
**  update needed by cmd_update().
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	upd_group(struct GRP *g, int *didone)
**
** g:	    GRP structure, modify, by reference
** didone:  boolean, write only, by reference
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
static unsigned int upd_group(struct GRP *g, int *didone) {

    char tmp[STRING_SIZE];
    int reply_code, artcnt, len;

    if (g->subscribed) {
    	strcpy(tmp, "GROUP ");
    	strcat(tmp, g->grpnam);
    	server_send(tmp);
    	server_get_reply(SRV__NOECHO, &reply_code, tmp, sizeof(tmp)-1, &len);
    	if (reply_code != NNTP__GRPSELECTED) {
    	    lib$signal(NEWS__UPDERR, 2, strlen(g->grpnam), g->grpnam,
    	    	NEWS__UNEXPRSP, 3, reply_code, len, tmp);
    	} else {
    	    int f, l;
    	    tmp[len] = '\0';
    	    parse_group_reply(tmp, &artcnt, &f, &l);
    	    if (g->valid != 0 && (f > g->frstavl || l != g->lastavl)) {
    	    	free(g->valid);
    	    	g->valid = 0;
    	    }
    	    g->frstavl = (f > g->frstavl ? f : g->frstavl);
    	    g->lastavl = l;
    	    g->avail = 1;
    	}
    	artcnt = Count_Unseen(g);
    	if (artcnt > 0) {
    	    struct dsc$descriptor tdsc;
    	    short tlen;
    	    unsigned int status;
    	    if (!*didone) lib$put_output(&nulstr);
    	    INIT_SDESC(tdsc, sizeof(tmp)-1, tmp);
    	    status = sys$fao(&newgroup_ctrstr, &tlen, &tdsc, artcnt, strlen(g->grpnam), g->grpnam);
    	    tdsc.dsc$w_length = tlen;
    	    lib$put_output(&tdsc);
    	    *didone = 1;
    	}
    }

    return SS$_NORMAL;

} /* upd_group */

/*
**++
**  ROUTINE:	cmd_update_full
**
**  FUNCTIONAL DESCRIPTION:
**
**  	UPDATE/FULL command.  Just calls Do_Full_Update.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_update_full()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	see Do_Full_Update.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cmd_update_full() {

    return Do_Full_Update(0);

} /* cmd_update_full */

/*
**++
**  ROUTINE:	Do_Full_Update
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Performs a full update by sending a LIST command to the
**  server and getting back the list of all active newsgroups, then
**  updating the group tree with the information.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Do_Full_Update(int silent)
**
** silent:  boolean, read only, by value
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
unsigned int Do_Full_Update(int silent) {

    char tmp[STRING_SIZE];
    struct dsc$descriptor tdsc;
    struct HDR *h;
    struct QUE rspq;
    int reply_code, len;

    rspq.head = rspq.tail = &rspq;
    server_send("LIST");
    server_get_reply(SRV__NOECHO, &reply_code, tmp, sizeof(tmp), &len);
    if (reply_code != NNTP__GLISTFOLLOWS) {
    	lib$signal(NEWS__UNEXPRSP, 3, reply_code, len, tmp);
    	return SS$_NORMAL;
    }

    INIT_SDESC(tdsc, sizeof(tmp)-1, tmp);
    while(server_get_line(tmp, sizeof(tmp), 0) != NEWS__EOLIST) {

    	struct GRP *grp, *g2;

    	grp = mem_getgrp();
    	grp->avail = 1;
    	if (OK(Parse_List_Reply(tmp, grp))) {
    	    g2 = Find_Group(grp->grpnam);
    	    if (g2 != NULL) {
    	    	g2->avail = 1;
    	    	if (g2->valid != 0 && (g2->frstavl < grp->frstavl ||
    	    	    	    g2->lastavl != grp->lastavl)) {
    	    	    free(g2->valid);
    	    	    g2->valid = 0;
    	    	}
    	    	g2->frstavl = (grp->frstavl > g2->frstavl ? grp->frstavl :
    	    	    	    	g2->frstavl);
    	    	g2->lastavl = grp->lastavl;
    	    	g2->directpost = grp->directpost;
    	    	if (g2->subscribed) {
    	    	    int artcnt;
    	    	    artcnt = Count_Unseen(g2);
/*
** If we weren't told to be quiet, build a message to notify the
** user of the new articles.
*/
    	    	    if (artcnt > 0 && !silent) {
    	    	    	short tlen;
    	    	    	unsigned int status;
    	    	    	status = sys$fao(&newgroup_ctrstr, &tlen, &tdsc,
    	    	    	    artcnt, strlen(g2->grpnam), g2->grpnam);
    	    	    	*(tmp+tlen) = '\0';
    	    	    	h = mem_gethdr(tlen+1);
    	    	    	h->code = (int) g2;
    	    	    	h->len = tlen;
    	    	    	strcpy(h->str, tmp);
    	    	    	queue_insert(h, rspq.tail);
    	    	    }
    	    	}
    	    	mem_freegrp(grp);
    	    } else {
    	    	Insert_Group(grp);
    	    	if (news_prof.glist) {
    	    	    news_prof.glast->next = grp;
    	    	    news_prof.glast = grp;
    	    	}
    	    }
    	} else {
    	    mem_freegrp(grp);
    	}
    }

/*
** Traverse the group tree or group list.  If there's a new-articles
** note on the rspq list that matches the group, spit it out.  This way,
** the user gets the notices in the same order she would visit the
** newsgroups.
*/
    if (rspq.head != &rspq) {
    	lib$put_output(&nulstr);
    	if (news_prof.glist) {
    	    struct GRP *g;
    	    for (g = news_prof.glist; g != NULL; g = g->next) {
    	    	print_response(g, &rspq);
    	    }
    	} else {
    	    lib$traverse_tree(&news_prof.gtree, print_response, &rspq);
    	}
    	while (queue_remove(rspq.head, &h)) {
    	    INIT_SDESC(tdsc, h->len, h->str);
    	    lib$put_output(&tdsc);
    	    mem_freehdr(h);
    	}
    	lib$put_output(&nulstr);
    }

    if (curgroup != NULL) {
    	strcpy(tmp, "GROUP ");
    	strcat(tmp, curgroup->grpnam);
    	server_send(tmp);
    	server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);
    }

    return SS$_NORMAL;

} /* Do_Full_Update */

/*
**++
**  ROUTINE:	print_response
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Tree-traversal action routine that prints a new-articles
**  response matching one of the newgroups.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	print_response(struct GRP *g, struct QUE *rspq)
**
** g:	    GRP structure, read only, by reference
** rspq:    QUE structure, modify, by reference
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
static unsigned int print_response(struct GRP *g, struct QUE *rspq) {

    struct HDR *h;
    struct dsc$descriptor sdsc;

    if (!g->subscribed) return SS$_NORMAL;
    for (h = rspq->head; h != (struct HDR *) rspq; h = h->flink) {
    	if ((struct GRP *) (h->code) == g) break;
    }
    if (h == (struct HDR *) rspq) return SS$_NORMAL;
    queue_remove(h, &h);
    INIT_SDESC(sdsc, h->len, h->str);
    lib$put_output(&sdsc);
    mem_freehdr(h);
    return SS$_NORMAL;

}

/*
**++
**  ROUTINE:	cmd_help
**
**  FUNCTIONAL DESCRIPTION:
**
**  	HELP command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_help()
**
**  IMPLICIT INPUTS:	None.
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
unsigned int cmd_help() {

    char tmp[STRING_SIZE];

    if (cli_present("TOPIC") == CLI$_PRESENT) {
    	cli_get_value("TOPIC", tmp, sizeof(tmp));
    } else {
    	tmp[0] = '\0';
    }

    put_paged_help(tmp, "NEWSRDR_HELP");

    return SS$_NORMAL;

} /* cmd_help */

/*
**++
**  ROUTINE:	cmd_spawn
**
**  FUNCTIONAL DESCRIPTION:
**
**  	SPAWN command.  Works like the one in VMS Mail.  On pre-VMS V5.2
**  systems, we check to see if the CAPTIVE flag is set for the process.
**  If so, we disallow the SPAWN.  On V5.2 and later, CAPTIVE is handled
**  automatically by the system.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_spawn()
**
**  IMPLICIT INPUTS:	None.
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
unsigned int cmd_spawn() {

    struct dsc$descriptor cmddsc, *cmddscp;
    struct dsc$descriptor inpdsc, *inpdscp;
    struct dsc$descriptor outdsc, *outdscp;
    struct dsc$descriptor pnmdsc, *pnmdscp;
    char cmdstr[STRING_SIZE], inpstr[STRING_SIZE], outstr[STRING_SIZE];
    char prcnam[STRING_SIZE];
    unsigned int flags, pid, status, image_privs[2];
    int nowait;

    if (news_prof.captive) {
    	lib$signal(NEWS__CAPTIVE, 0);
    	return SS$_NORMAL;
    }

/*
** Just to make sure, we turn off image privileges again before spawning.
** Spawned subprocesses inherit the current privilege mask.
*/
    sys$setprv(0, 0, image_privs);
    flags = 0;

    cmddscp = inpdscp = outdscp = pnmdscp = NULL;

    if (OK(cli_get_value("CMD", cmdstr, sizeof(cmdstr)))) {
    	INIT_SDESC(cmddsc, strlen(cmdstr), cmdstr);
    	cmddscp = &cmddsc;
    }
    if (OK(cli_get_value("INPUT", inpstr, sizeof(inpstr)))) {
    	INIT_SDESC(inpdsc, strlen(inpstr), inpstr);
    	inpdscp = &inpdsc;
    }
    if (OK(cli_get_value("OUTPUT", outstr, sizeof(outstr)))) {
    	INIT_SDESC(outdsc, strlen(outstr), outstr);
    	outdscp = &outdsc;
    }
    if (OK(cli_get_value("PROCESS", prcnam, sizeof(prcnam)))) {
    	INIT_SDESC(pnmdsc, strlen(prcnam), prcnam);
    	pnmdscp = &pnmdsc;
    }

    nowait = 0;
    if (cli_present("WAIT") == CLI$_NEGATED) {
    	nowait = 1;
    	flags |= CLI$M_NOWAIT;
    }
    if (cli_present("SYMBOLS") == CLI$_NEGATED) flags |= CLI$M_NOCLISYM;
    if (cli_present("LOGICAL_NAMES") == CLI$_NEGATED) flags |= CLI$M_NOLOGNAM;

    if (!nowait) Finish_Paged_Output(2);
    status = lib$spawn(cmddscp, inpdscp, outdscp, &flags, pnmdscp, &pid);
    if (!OK(status)) {
    	if (!nowait) {
    	    if (news_prof.autosave) Write_Profile();
    	    Pager_Init(1);
    	    Check_Connection();
    	}
    	lib$signal(NEWS__SPAWNERR, 0, status);
    } else {
    	if (nowait) {
    	    unsigned int jpi_prcnam=JPI$_PRCNAM;
    	    short pnmlen;
    	    INIT_SDESC(pnmdsc, sizeof(prcnam), prcnam);
    	    lib$getjpi(&jpi_prcnam, &pid, 0, 0, &pnmdsc, &pnmlen);
    	    lib$signal(NEWS__SPAWNED, 3, pnmlen, prcnam, pid);
    	} else {
    	    Pager_Init(1);
    	    Check_Connection();
    	}
    }

    return SS$_NORMAL;

} /* cmd_spawn */

/*
**++
**  ROUTINE:	cmd_attach
**
**  FUNCTIONAL DESCRIPTION:
**
**  	ATTACH command.  Attaches to another process in the job tree.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_attach()
**
**  IMPLICIT INPUTS:	None.
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
unsigned int cmd_attach() {

    char tmp[STRING_SIZE];
    unsigned int pid, status;
    static unsigned int jpi_pid=JPI$_PID, jpi_owner=JPI$_OWNER;
    struct dsc$descriptor sdsc;

    if (news_prof.captive) {
    	lib$signal(NEWS__CAPTIVE, 0);
    	return SS$_NORMAL;
    }

    if (cli_present("PRCNAM") == CLI$_PRESENT) {
    	cli_get_value("PRCNAM", tmp, sizeof(tmp));
    	INIT_SDESC(sdsc, strlen(tmp), tmp);
    	status = lib$getjpi(&jpi_pid, 0, &sdsc, &pid);
    	if (!OK(status)) {
    	    lib$signal(NEWS__ATTERR, 0, status);
    	    return SS$_NORMAL;
    	}
    } else {
    	status = lib$getjpi(&jpi_owner, 0, 0, &pid);
    	if (!OK(status)) {
    	    lib$signal(NEWS__ATTERR, 0, status);
    	    return SS$_NORMAL;
    	}
    	if (pid == 0) {
    	    lib$signal(NEWS__NOPARENT, 0);
    	    return SS$_NORMAL;
    	}
    }

    Finish_Paged_Output(1);
    if (news_prof.autosave) Write_Profile();
    status = lib$attach(&pid);
    if (!OK(status)) {
    	lib$signal(NEWS__ATTERR, 0, status);
    }

    Pager_Init(0);
    Check_Connection();
    return SS$_NORMAL;

} /* cmd_attach */

/*
**++
**  ROUTINE:	cmd_defkey
**
**  FUNCTIONAL DESCRIPTION:
**
**  	DEFINE/KEY command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_defkey()
**
**  IMPLICIT INPUTS:	None.
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
unsigned int cmd_defkey() {

    char tmp[STRING_SIZE];
    unsigned int status;

    strcpy(tmp, "DEFINE/KEY ");
    cli_get_value("DKCMD", tmp+11, sizeof(tmp)-11);
    status = Define_Key(tmp, strlen(tmp));
    if (!OK(status)) {
    	lib$signal(NEWS__KEYDEFERR, 0, status);
    }

    return SS$_NORMAL;

} /* cmd_defkey */
