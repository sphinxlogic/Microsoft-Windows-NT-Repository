/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:
**
**  	Commands relating to reading articles.
**
**  MODULE DESCRIPTION:
**
**  	This module contains all the READ commands and some other
**  related commands.
**
**  	All of these routines make extensive use of the module statics
**  and global variables.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, 1994  MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  03-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	03-SEP-1992 V1.0    Madison 	Initial coding.
**  	12-SEP-1992 V1.0-1  Madison 	Support user-specified group ordering.
**  	16-SEP-1992 V1.0-2  Madison 	READ/THREAD wasn't marking articles seen.
**  	23-SEP-1992 V1.0-3  Madison 	Fix EXTRACT's append logic.
**  	21-OCT-1992 V1.0-4  Madison 	Fix looping problem in cmd_readsubj.
**  	15-FEB-1993 V1.0-5  Madison 	Make compatibile with pre-VMS V5.2.
**  	15-FEB-1993 V1.1    Madison 	Add SET [NO]SEE/SUBJ, /CROSS, NOSEE/THR.
**  	12-APR-1993 V1.2    Madison 	Get_Article_* has changed; autoreadnew.
**  	14-MAY-1993 V1.2-1  Madison 	Find_Group_Wild now takes context.
**  	31-MAY-1993 V1.2-2  Madison 	Fix looping bug in cont_readsubj.
**  	22-JUN-1993 V1.3    Madison 	Auto save.
**  	23-SEP-1993 V1.4    Madison 	Add XHDR support.
**  	09-OCT-1993 V1.5    Madison 	Fix MARKCOUNT message, etc.
**  	21-OCT-1993 V1.6    Madison 	Add cont_readthread, fix cmd_skipsubj.
**  	28-OCT-1993 V1.6-1  Madison 	Really fix cmd_skipsubj.
**  	03-NOV-1993 V1.6-2  Madison 	Yet another fix in cmd_skipsubj.
**  	10-NOV-1993 V1.6-3  Madison 	Split GRPSET msg into two parts.
**  	05-DEC-1993 V1.6-4  Madison 	Have EXTRACT do valid article checks.
**  	15-JAN-1994 V1.7    Madison 	More EXTRACT mods.
**  	16-MAY-1994 V1.8    Madison 	Ref-threads, EXTRACT mods.
**  	19-MAY-1994 V1.8-1  Madison 	Add /IGNORE to EXTRACT; fix loop in Find_Next_Unseen.
**  	25-MAY-1994 V1.8-2  Madison 	Fix missing file closure in EXTRACT.
**--
*/
#include "newsrdr.h"
#include "globals.h"

/*
** Forward declarations
*/

    unsigned int cmd_read(void), cmd_readcont(void), cmd_readnew(void);
    unsigned int cmd_readnext(void), cmd_readprev(void), cmd_readnextnew(void);
    unsigned int cmd_readnextnewgroup(void), cmd_readthread(void);
    unsigned int cont_readthread(void);
    unsigned int cmd_readsubj(void), cont_readsubj(void), cmd_current(void);
    unsigned int cmd_skipthread(void), cmd_skipsubj(void), cmd_group(void);
    static unsigned int Find_And_Set_Current_Group(char *);
    unsigned int Set_Current_Group(struct GRP *);
    unsigned int cmd_extract(void);
    unsigned int Clear_ArtInProg(void);
    static unsigned int Find_Next_Unseen(int, int*);

    GLOBAL int cur_artnum = 0;	    	/* Current article number   	      */
    GLOBAL int rotate_text = 0;	    	/* ROT13 flag, used in ARTICLE module */
    GLOBAL int Read_Full_Headers = 0;	/* Header-settings override flag      */
    EXTERN struct GRP *curgroup;    	/* Current group                      */
    EXTERN int User_Interrupt;          /* User pressed CTRL/C                */

/*
** Statics for tracking info for READ/THREAD
*/
    static int thread_base = 0;
    static char thread_subject[STRING_SIZE];
    static char thread_msgid[STRING_SIZE];
    static struct QUE subjq = {&subjq, &subjq};
    static int thread_newonly;
    static int thread_strict;

/*
** Context about our current state, for those multiple-action reads.
** We use saved_default_action to track the routine that we started
** with, so when someone uses READ/NEXT/NEW to read an article,
** just pressing return will continue to use READ/NEXT NEW; likewise
** for READ/NEXT.  We need this because routine cmd_readcont is used
** by all of these commands to handle continuation of the current
** article.
*/
    static unsigned int (*saved_default_action)() = (unsigned int (*)()) 0;
    static unsigned int cur_artctx = 0;
    static int Article_In_Progress = 0;
    static unsigned int rgctx = 0;

/*
** External references, mostly from module ARTICLE.
*/
    extern unsigned int Retrieve_Article(int, void *, char *, char *, char *);
    extern int Mark_Article_Seen(struct GRP *, int);
    extern int Mark_Article_Unseen(struct GRP *, int);
    extern void Mark_Xref(char *, int);
    extern unsigned int Show_Article_Page(void *);
    extern int Ignore_Article(struct GRP *, int);
    extern unsigned int Get_Article_Hdrs(struct GRP *, int, void *, int, int *);
    extern unsigned int Get_One_Header(struct GRP *, int, int, struct HDR **);
    extern unsigned int Get_Article_Body(int, char *, char *, int *, int);
    extern void Rewind_Article(unsigned int *);
    extern void Close_Article(unsigned int *);
    extern struct GRP *Find_Group(char *);
    extern struct GRP *Find_Group_Wild(char *, unsigned int *);
    extern unsigned int parse_group_reply(char *, int *, int *, int *);
    extern unsigned int Parse_Range(char *, int *, int *, int, struct GRP *);
    extern struct GRP *Traverse_Tree(unsigned int *);
    extern void Traverse_Finish(unsigned int *);
    extern struct GRP *Current_TreeNode(unsigned int *);
    extern int Next_Unseen_Article(int, int, struct GRP *);
    extern int Count_Unseen(struct GRP *);
    extern unsigned int Edit_Article(void *);
    extern unsigned int Write_Profile(void);
    extern int Is_Valid_Article(struct GRP *, int);
    extern int Thread_Match(char *, char *);

/*
**++
**  ROUTINE:	cmd_read
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The READ command.  Reads an article by its number.
** Also handles READ/FIRST, READ/LAST, and the synonyms for
** those commands (FIRST, LAST).
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_read()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_read() {

    unsigned int status;
    char xref_hdr[STRING_SIZE], tmp[STRING_SIZE], verb[32];
    int artnum;

/*
** READ/SUBJECT is handled by cmd_readsubj.
*/
    if (cli_present("SUBJECT") == CLI$_PRESENT) return cmd_readsubj();

    Clear_ArtInProg();

    if (cli_present("GROUP") == CLI$_PRESENT) {
    	cli_get_value("GROUP", tmp, sizeof(tmp));
    	status = Find_And_Set_Current_Group(tmp);
    	if (!OK(status)) return SS$_NORMAL;
    }

    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }

    cli_get_value("$VERB", verb, sizeof(verb));
    if (cli_present("ARTNUM") == CLI$_PRESENT) {
    	cli_get_value("ARTNUM", tmp, sizeof(tmp));
    	if (tmp[0] == '.' && tmp[1] == '\0') {
    	    if (cur_artnum == 0) {
    	    	lib$signal(NEWS__NOCURART, 0);
    	    	return SS$_NORMAL;
    	    } else {
    	    	artnum = cur_artnum;
    	    }
    	} else {
    	    status = lib$cvt_dtb(strlen(tmp), tmp, &artnum);
    	    if (!OK(status)) {
    	    	lib$signal(NEWS__CMDERR, 0, status);
    	    	return SS$_NORMAL;
    	    }
    	}
    	if (artnum < curgroup->frstavl ||
    	    	artnum > curgroup->lastavl) {
    	    lib$signal(NEWS__OUTRNG, 3, artnum, curgroup->frstavl,
    	    	    	    curgroup->lastavl);
    	    return SS$_NORMAL;
    	}
    } else if (verb[0] == 'F' || cli_present("FIRST") == CLI$_PRESENT) {
    	artnum = curgroup->frstavl;
    } else if (verb[0] == 'L' || cli_present("LAST") == CLI$_PRESENT) {
    	artnum = curgroup->lastavl;
    } else {
    	lib$signal(NEWS__NOARTSPEC, 0);
    	return SS$_NORMAL;
    }

    cur_artnum = artnum;
    default_action = cmd_readcont;
    cleanup_action = Clear_ArtInProg;
    xref_hdr[0] = '\0';
    status = Retrieve_Article(artnum, &cur_artctx, thread_subject, xref_hdr, thread_msgid);
    Mark_Article_Seen(curgroup, cur_artnum);
    if (OK(status)) {
    	thread_base = cur_artnum;
    	if (xref_hdr[0] != '\0') Mark_Xref(xref_hdr, 0);
    	saved_default_action = cmd_readnext;
    	Article_In_Progress = 1;
    	status = Show_Article_Page(&cur_artctx);
    	if (status == NEWS__EOARTICLE) {
    	    default_action = cmd_readnext;
    	    cleanup_action = Clear_ArtInProg;
    	}
    } else {
    	Clear_ArtInProg();
    }

    return SS$_NORMAL;

} /* cmd_read */

/*
**++
**  ROUTINE:	cmd_readcont
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Continues paging through the current article.
**  At the end of the article, the default action becomes
**  the one we saved when we started the article.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_readcont()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_readcont() {

    unsigned int status;

    if (Article_In_Progress) {
    	status = Show_Article_Page(&cur_artctx);
    	if (status == NEWS__EOARTICLE) {
    	    default_action = saved_default_action;
    	    cleanup_action = Clear_ArtInProg;
    	}
    } else {
    	lib$signal(NEWS__NOCURART, 0);
    }

    return SS$_NORMAL;

} /* cmd_readcont */

/*
**++
**  ROUTINE:	cmd_readnew
**
**  FUNCTIONAL DESCRIPTION:
**
**  	READ/NEW command.  Reads the first new article.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_readnew()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_readnew() {

    unsigned int status;
    char xref_hdr[STRING_SIZE];
    struct GRP *g;

    Clear_ArtInProg();
    if (thread_base != 0) {
    	cur_artnum = thread_base;
    	thread_base = 0;
    }

    g = curgroup;
    while (1) {
    	if (!OK(Find_Next_Unseen(0, &cur_artnum))) {
    	    lib$signal(NEWS__NONEW, 0);
    	    default_action = cleanup_action = (unsigned int (*)()) 0;
    	    return SS$_NORMAL;
    	}
    	if (!Ignore_Article(curgroup, cur_artnum)) break;
    }
    if (g != curgroup) {
    	lib$signal(NEWS__GRPSET, 2, strlen(curgroup->grpnam),
    	    	curgroup->grpnam, NEWS__GRPINFO, 3, curgroup->frstavl,
    	    	curgroup->lastavl, Count_Unseen(curgroup));
    	if (news_prof.autosave) Write_Profile();
    	if (!news_prof.autoreadnew) {
    	    default_action = cmd_readnew;
    	    cleanup_action = 0;
    	    cur_artnum = 0;
    	    return SS$_NORMAL;
    	}
    }

    default_action = cmd_readcont;
    cleanup_action = Clear_ArtInProg;
    xref_hdr[0] = '\0';
    status = Retrieve_Article(cur_artnum, &cur_artctx, thread_subject, xref_hdr, thread_msgid);
    Mark_Article_Seen(curgroup, cur_artnum);
    if (OK(status)) {
    	thread_base = cur_artnum;
    	if (xref_hdr[0] != '\0') Mark_Xref(xref_hdr, 0);
    	Article_In_Progress = 1;
    	saved_default_action = cmd_readnextnew;
    	status = Show_Article_Page(&cur_artctx);
    	if (status == NEWS__EOARTICLE) {
    	    default_action = cmd_readnextnew;
    	    cleanup_action = Clear_ArtInProg;
    	}
    }

    return SS$_NORMAL;

} /* cmd_readnew */

/*
**++
**  ROUTINE:	cmd_readnext
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The READ/NEXT command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_readnext()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_readnext() {

    char xref_hdr[STRING_SIZE];
    unsigned int status;

    Clear_ArtInProg();
    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }

    if (thread_base != 0) {
    	cur_artnum = thread_base;
    	thread_base = 0;
    }

    while (1) {
    	if (cur_artnum >= curgroup->lastavl) {
    	    lib$signal(NEWS__NOMORE, 0);
    	    return SS$_NORMAL;
    	}

    	if (cur_artnum < curgroup->frstavl) {
    	    cur_artnum = curgroup->frstavl;
    	} else {
    	    cur_artnum += 1;
    	}
    	if (!Is_Valid_Article(curgroup, cur_artnum)) continue;
    	if (!Ignore_Article(curgroup, cur_artnum)) break;
    }

    default_action = cmd_readcont;
    cleanup_action = Clear_ArtInProg;
    xref_hdr[0] = '\0';
    status = Retrieve_Article(cur_artnum, &cur_artctx, thread_subject, xref_hdr, thread_msgid);
    Mark_Article_Seen(curgroup, cur_artnum);
    if (OK(status)) {
    	thread_base = cur_artnum;
    	if (xref_hdr[0] != '\0') Mark_Xref(xref_hdr, 0);
    	Article_In_Progress = 1;
    	saved_default_action = cmd_readnext;
    	status = Show_Article_Page(&cur_artctx);
    	if (status == NEWS__EOARTICLE) {
    	    default_action = cmd_readnext;
    	    cleanup_action = Clear_ArtInProg;
    	}
    }

    return SS$_NORMAL;

} /* cmd_readnext */

/*
**++
**  ROUTINE:	cmd_readprev
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The READ/PREVIOUS command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_readprev()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_readprev() {

    char xref_hdr[STRING_SIZE];
    unsigned int status;

    Clear_ArtInProg();
    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }

    if (thread_base != 0) {
    	cur_artnum = thread_base;
    	thread_base = 0;
    }

    while (1) {
    	if (cur_artnum <= curgroup->frstavl) {
    	    lib$signal(NEWS__NOMORE, 0);
    	    return SS$_NORMAL;
    	}

    	if (cur_artnum > curgroup->lastavl) {
    	    cur_artnum = curgroup->lastavl;
    	} else {
    	    cur_artnum -= 1;
    	}
    	if (!Is_Valid_Article(curgroup, cur_artnum)) continue;
    	if (!Ignore_Article(curgroup, cur_artnum)) break;
    }

    default_action = cmd_readcont;
    cleanup_action = Clear_ArtInProg;
    xref_hdr[0] = '\0';
    status = Retrieve_Article(cur_artnum, &cur_artctx, thread_subject, xref_hdr, thread_msgid);
    Mark_Article_Seen(curgroup, cur_artnum);
    if (OK(status)) {
    	thread_base = cur_artnum;
    	if (xref_hdr[0] != '\0') Mark_Xref(xref_hdr, 0);
    	Article_In_Progress = 1;
    	saved_default_action = cmd_readprev;
    	status = Show_Article_Page(&cur_artctx);
    	if (status == NEWS__EOARTICLE) {
    	    default_action = cmd_readprev;
    	    cleanup_action = Clear_ArtInProg;
    	}
    }

    return SS$_NORMAL;

} /* cmd_readprev */

/*
**++
**  ROUTINE:	cmd_readnextnew
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The READ/NEXT/NEW command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_readnextnew()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_readnextnew() {

    struct GRP *g, *cyclepoint;
    char xref_hdr[STRING_SIZE];
    unsigned int status;

    Clear_ArtInProg();
    if (thread_base != 0) {
    	cur_artnum = thread_base;
    	thread_base = 0;
    }

    g = curgroup;
    while (1) {
    	if (!OK(Find_Next_Unseen(cur_artnum, &cur_artnum))) {
    	    lib$signal(NEWS__NONEW, 0);
    	    return SS$_NORMAL;
    	}
    	if (!Ignore_Article(curgroup, cur_artnum)) break;   
    }
    if ((g != curgroup) && (curgroup != NULL)) {
    	if (g != NULL) {
    	    lib$signal(NEWS__EOGROUP, 2, strlen(g->grpnam), g->grpnam,
    	    	NEWS__GRPSET, 2, strlen(curgroup->grpnam),
    	    	curgroup->grpnam, NEWS__GRPINFO, 3, curgroup->frstavl,
    	    	curgroup->lastavl, Count_Unseen(curgroup));
    	} else {
    	    lib$signal(NEWS__GRPSET, 2,  strlen(curgroup->grpnam),
    	    	curgroup->grpnam, NEWS__GRPINFO, 3, curgroup->frstavl,
    	    	curgroup->lastavl, Count_Unseen(curgroup));
    	}
    	if (news_prof.autosave) Write_Profile();
    	if (!news_prof.autoreadnew) {
    	    default_action = cmd_readnextnew;
    	    cleanup_action = 0;
    	    cur_artnum = 0;
    	    return SS$_NORMAL;
    	}
    }

    default_action = cmd_readcont;
    cleanup_action = Clear_ArtInProg;
    xref_hdr[0] = '\0';
    status = Retrieve_Article(cur_artnum, &cur_artctx, thread_subject, xref_hdr, thread_msgid);
    Mark_Article_Seen(curgroup, cur_artnum);
    if (OK(status)) {
    	thread_base = cur_artnum;
    	if (xref_hdr[0] != '\0') Mark_Xref(xref_hdr, 0);
    	Article_In_Progress = 1;
    	saved_default_action = cmd_readnextnew;
    	status = Show_Article_Page(&cur_artctx);
    	if (status == NEWS__EOARTICLE) {
    	    default_action = cmd_readnextnew;
    	    cleanup_action = Clear_ArtInProg;
    	}
    }

    return SS$_NORMAL;

} /* cmd_readnextnew */

/*
**++
**  ROUTINE:	cmd_readnextnewgroup
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The READ/NEXT/NEW/GROUP command.  Skips to next group
**  before looking for new articles.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_readnextnewgroup()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_readnextnewgroup() {

    struct GRP *g, *cyclepoint;
    char xref_hdr[STRING_SIZE];
    unsigned int status;

    if (curgroup == NULL) return cmd_readnextnew();

    Clear_ArtInProg();
    if (thread_base != 0) {
    	cur_artnum = thread_base;
    	thread_base = 0;
    }

    g = curgroup;
    cur_artnum = curgroup->lastavl;
    while (1) {
    	if (!OK(Find_Next_Unseen(cur_artnum, &cur_artnum))) {
    	    lib$signal(NEWS__NONEW, 0);
    	    return SS$_NORMAL;
    	}
    	if (curgroup == g) {
    	    lib$signal(NEWS__NONEW, 0);
    	    return SS$_NORMAL;
    	}
    	if (!Ignore_Article(curgroup, cur_artnum)) break;   
    }
    if ((g != curgroup) && (curgroup != NULL)) {
    	if (g != NULL) {
    	    lib$signal(NEWS__EOGROUP, 2, strlen(g->grpnam), g->grpnam,
    	    	NEWS__GRPSET, 2, strlen(curgroup->grpnam),
    	    	curgroup->grpnam, NEWS__GRPINFO, 3, curgroup->frstavl,
    	    	curgroup->lastavl, Count_Unseen(curgroup));
    	} else {
    	    lib$signal(NEWS__GRPSET, 2, strlen(curgroup->grpnam),
    	    	curgroup->grpnam, NEWS__GRPINFO, 3, curgroup->frstavl,
    	    	curgroup->lastavl, Count_Unseen(curgroup));
    	}
    	if (news_prof.autosave) Write_Profile();
    	if (!news_prof.autoreadnew) {
       	    default_action = cmd_readnextnew;
    	    cleanup_action = 0;
    	    cur_artnum = 0;
    	    return SS$_NORMAL;
    	}
    }

    default_action = cmd_readcont;
    cleanup_action = Clear_ArtInProg;
    xref_hdr[0] = '\0';
    status = Retrieve_Article(cur_artnum, &cur_artctx, thread_subject, xref_hdr, thread_msgid);
    Mark_Article_Seen(curgroup, cur_artnum);
    if (OK(status)) {
    	thread_base = cur_artnum;
    	if (xref_hdr[0] != '\0') Mark_Xref(xref_hdr, 0);
    	Article_In_Progress = 1;
    	saved_default_action = cmd_readnextnew;
    	status = Show_Article_Page(&cur_artctx);
    	if (status == NEWS__EOARTICLE) {
    	    default_action = cmd_readnextnew;
    	    cleanup_action = Clear_ArtInProg;
    	}
    }

    return SS$_NORMAL;

} /* cmd_readnextnewgroup */

/*
**++
**  ROUTINE:	cmd_readthread
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The READ/THREAD command.  Finds the next article
**  with subject "matching" the thread base's subject.  The thread
**  base is established by reading an article with a command other
**  than READ/THREAD.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_readthread()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_readthread() {

    struct GRP *g;
    char tmp[STRING_SIZE], xref_hdr[STRING_SIZE], *tsubj;
    struct HDR *hdr;
    struct QUE *hdrqptr;
    unsigned int status;
    int doit;

    Clear_ArtInProg();
    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }
    if (thread_base == 0) {
    	lib$signal(NEWS__NOTHREAD, 0);
    	return SS$_NORMAL;
    }
    if (thread_msgid[0] == '\0' && thread_subject[0] == '\0') {
    	lib$signal(NEWS__NOSUBJECT, 0);
    	return SS$_NORMAL;
    }

    thread_newonly = cli_present("NEW") == CLI$_PRESENT;
    thread_strict = cli_present("STRICT") == CLI$_PRESENT;

    while (1) {
    	if (thread_newonly) cur_artnum = Next_Unseen_Article(cur_artnum, 0, 0);
    	if (cur_artnum == 0 || cur_artnum >= curgroup->lastavl) {
    	    lib$signal(NEWS__NOMORE, 0);
    	    cur_artnum = thread_base;
    	    thread_base = 0;
    	    default_action = cmd_readnextnew;
    	    cleanup_action = (unsigned int (*)()) 0;
    	    return SS$_NORMAL;
    	}

    	if (cur_artnum < curgroup->frstavl) {
    	    cur_artnum = curgroup->frstavl;
    	} else {
    	    if (!thread_newonly) cur_artnum += 1;
    	}

    	if (!Ignore_Article(curgroup, cur_artnum)) break;

    }

    while (1) {

    	if (User_Interrupt) break;
    	doit = 0;
    	if (OK(Get_One_Header(curgroup, cur_artnum, NEWS_K_HDR_REFERENCES, &hdr))) {
    	    if (hdr != 0) doit = Thread_Match(hdr->str, thread_msgid);
    	    else if (OK(Get_One_Header(curgroup, cur_artnum,
    	    	    	    NEWS_K_HDR_MESSAGE_ID, &hdr))) {
    	    	if (hdr != 0) doit = Thread_Match(thread_msgid, hdr->str);
    	    }
    	}
    	if (!doit && !thread_strict &&
    	    	OK(Get_One_Header(curgroup, cur_artnum, NEWS_K_HDR_SUBJECT, &hdr))) {
    	    if (hdr != 0) {
    	    	strcpy(tmp, hdr->str);
    	    	upcase(tmp);
    	    	if (strstr(tmp, "RE:") == tmp) {
    	    	    for (tsubj = tmp+3; isspace(*tsubj); tsubj++);
    	    	 } else {
    	    	    tsubj = tmp;
    	    	 }
    	    } else {
    	    	tsubj = "(none)";
    	    }
    	    doit = (strstr(tsubj, thread_subject) != 0)
    	    	   || (strstr(thread_subject, tsubj) != 0);
    	}

    	if (doit) {
    	    xref_hdr[0] = '\0';
    	    status = Retrieve_Article(cur_artnum, &cur_artctx, tmp, xref_hdr, tmp);
    	    Mark_Article_Seen(curgroup, cur_artnum);
    	    if (OK(status)) {
    	    	default_action = cmd_readcont;
    	    	cleanup_action = Clear_ArtInProg;
    	    	saved_default_action = cont_readthread;
    	    	if (xref_hdr[0] != '\0') Mark_Xref(xref_hdr, 0);
    	    	Article_In_Progress = 1;
    	    	saved_default_action = cmd_readthread;
    	    	status = Show_Article_Page(&cur_artctx);
    	    	if (status == NEWS__EOARTICLE) {
    	    	    default_action = cont_readthread;
    	    	    cleanup_action = Clear_ArtInProg;
    	    	}
    	    	break;
    	    }
    	}

    	while (1) {
    	    if (thread_newonly) {
    	    	cur_artnum = Next_Unseen_Article(cur_artnum, 0, 0);
    	    } else {
    	    	cur_artnum += 1;
    	    }
    	    if (cur_artnum == 0 || cur_artnum > curgroup->lastavl) break;
    	    if (!Ignore_Article(curgroup, cur_artnum)) break;
    	}
    	if (cur_artnum == 0 || cur_artnum > curgroup->lastavl) break;

    }

    if (cur_artnum == 0 || cur_artnum > curgroup->lastavl) {
    	lib$signal(NEWS__NOMORE, 0);
    	cur_artnum = thread_base;
    	thread_base = 0;
    	default_action = cmd_readnextnew;
    	cleanup_action = (unsigned int (*)()) 0;
    }

    return SS$_NORMAL;

} /* cmd_readthread */

/*
**++
**  ROUTINE:	cont_readthread
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Continuation of a READ/THREAD command.  Finds the next article
**  with subject "matching" the thread base's subject.  The thread
**  base is established by reading an article with a command other
**  than READ/THREAD.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cont_readthread()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cont_readthread() {

    struct GRP *g;
    char tmp[STRING_SIZE], xref_hdr[STRING_SIZE], *tsubj;
    struct HDR *hdr;
    struct QUE *hdrqptr;
    unsigned int status;
    int doit;

    Clear_ArtInProg();
    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }
    if (thread_base == 0) {
    	lib$signal(NEWS__NOTHREAD, 0);
    	return SS$_NORMAL;
    }
    if (thread_msgid[0] == '\0' && thread_subject[0] == '\0') {
    	lib$signal(NEWS__NOSUBJECT, 0);
    	return SS$_NORMAL;
    }

    while (1) {
    	if (thread_newonly) cur_artnum = Next_Unseen_Article(cur_artnum, 0, 0);
    	if (cur_artnum == 0 || cur_artnum >= curgroup->lastavl) {
    	    lib$signal(NEWS__NOMORE, 0);
    	    cur_artnum = thread_base;
    	    thread_base = 0;
    	    default_action = cmd_readnextnew;
    	    cleanup_action = (unsigned int (*)()) 0;
    	    return SS$_NORMAL;
    	}

    	if (cur_artnum < curgroup->frstavl) {
    	    cur_artnum = curgroup->frstavl;
    	} else {
    	    if (!thread_newonly) cur_artnum += 1;
    	}

    	if (!Ignore_Article(curgroup, cur_artnum)) break;

    }

    while (1) {

    	if (User_Interrupt) break;
    	doit = 0;
    	if (OK(Get_One_Header(curgroup, cur_artnum, NEWS_K_HDR_REFERENCES, &hdr))) {
    	    if (hdr != 0) doit = Thread_Match(hdr->str, thread_msgid);
    	    else if (OK(Get_One_Header(curgroup, cur_artnum,
    	    	    	    NEWS_K_HDR_MESSAGE_ID, &hdr))) {
    	    	if (hdr != 0) doit = Thread_Match(thread_msgid, hdr->str);
    	    }
    	}
    	if (!doit && !thread_strict &&
    	    	OK(Get_One_Header(curgroup, cur_artnum, NEWS_K_HDR_SUBJECT, &hdr))) {
    	    if (hdr != 0) {
    	    	strcpy(tmp, hdr->str);
    	    	upcase(tmp);
    	    	if (strstr(tmp, "RE:") == tmp) {
    	    	    for (tsubj = tmp+3; isspace(*tsubj); tsubj++);
    	    	 } else {
    	    	    tsubj = tmp;
    	    	 }
    	    } else {
    	    	tsubj = "(none)";
    	    }
    	    doit = (strstr(tsubj, thread_subject) != 0)
    	    	    || (strstr(thread_subject, tsubj) != 0);
    	}
    	if (doit) {
    	    xref_hdr[0] = '\0';
    	    status = Retrieve_Article(cur_artnum, &cur_artctx, tmp, xref_hdr, tmp);
    	    Mark_Article_Seen(curgroup, cur_artnum);
    	    if (OK(status)) {
    	    	default_action = cmd_readcont;
    	    	cleanup_action = Clear_ArtInProg;
    	    	if (xref_hdr[0] != '\0') Mark_Xref(xref_hdr, 0);
    	    	Article_In_Progress = 1;
    	    	saved_default_action = cont_readthread;
    	    	status = Show_Article_Page(&cur_artctx);
    	    	if (status == NEWS__EOARTICLE) {
    	    	    default_action = cont_readthread;
    	    	    cleanup_action = Clear_ArtInProg;
    	    	}
    	    	break;
    	    }
    	}

    	while (1) {
    	    if (thread_newonly) {
    	    	cur_artnum = Next_Unseen_Article(cur_artnum, 0, 0);
    	    } else {
    	    	cur_artnum += 1;
    	    }
    	    if (cur_artnum == 0 || cur_artnum > curgroup->lastavl) break;
    	    if (!Ignore_Article(curgroup, cur_artnum)) break;
    	}
    	if (cur_artnum == 0 || cur_artnum > curgroup->lastavl) break;

    }

    if (cur_artnum == 0 || cur_artnum > curgroup->lastavl) {
    	lib$signal(NEWS__NOMORE, 0);
    	cur_artnum = thread_base;
    	thread_base = 0;
    	default_action = cmd_readnextnew;
    	cleanup_action = (unsigned int (*)()) 0;
    }

    return SS$_NORMAL;

} /* cont_readthread */

/*
**++
**  ROUTINE:	cmd_readsubj
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The READ/SUBJECT command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_readsubj()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_readsubj() {

    struct GRP *g;
    char xref_hdr[STRING_SIZE], tmp[STRING_SIZE], *tsubj;
    struct HDR *hdr, *shdr;
    struct QUE *hdrqptr;
    struct dsc$descriptor tdsc, sdsc;
    unsigned int status;
    int doit;

    Clear_ArtInProg();

    if (OK(cli_get_value("SUBJECT", tmp+1, sizeof(tmp)-1))) {
    	while (queue_remove(subjq.head, &hdr)) mem_freehdr(hdr);
    	do {
    	    upcase(tmp+1);
    	    if (!strchr(tmp+1, '*') && !strchr(tmp+1, '%')) {
    	    	tmp[0] = '*';
    	    	tsubj = tmp+strlen(tmp);
    	    	*tsubj++ = '*';
    	    	*tsubj = '\0';
    	    	tsubj = tmp;
    	    } else {
    	    	tsubj = tmp+1;
       	    }
    	    insert_header(tsubj, subjq.tail, 0);
    	 } while (OK(cli_get_value("SUBJECT", tmp+1, sizeof(tmp)-1)));
    }

    if (subjq.head == &subjq) {
    	lib$signal(NEWS__NOSUBJSPEC, 0);
    	return SS$_NORMAL;
    }

    if (cli_present("GROUP") == CLI$_PRESENT) {
    	cli_get_value("GROUP", tmp, sizeof(tmp));
    	status = Find_And_Set_Current_Group(tmp);
    	if (!OK(status)) {
    	    while (queue_remove(subjq.head, &hdr)) mem_freehdr(hdr);
    	    return SS$_NORMAL;
    	}   
    	cur_artnum = 0;
    }

    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	while (queue_remove(subjq.head, &hdr)) mem_freehdr(hdr);
    	return SS$_NORMAL;
    }   

    xref_hdr[0] = '\0';
    do {
    	if (cur_artnum < curgroup->frstavl) {
    	    cur_artnum = curgroup->frstavl;
    	} else {
    	    cur_artnum += 1;
    	}
    	if (!Is_Valid_Article(curgroup, cur_artnum)) continue;
    	if (!Ignore_Article(curgroup, cur_artnum)) break;
    } while (cur_artnum <= curgroup->lastavl);

    while (cur_artnum <= curgroup->lastavl) {
    	if (OK(Get_One_Header(curgroup, cur_artnum, NEWS_K_HDR_SUBJECT, &hdr))) {
    	    if (hdr != 0) {
    	    	strcpy(tmp, hdr->str);
    	    	upcase(tmp);
    	    	if (strstr(tmp, "RE:") == tmp) {
    	    	    for (tsubj = tmp+3; isspace(*tsubj); tsubj++);
    	    	} else {
    	    	    tsubj = tmp;
    	    	}
    	    } else {
    	    	tsubj = "(none)";
    	    }

    	    doit = 0;
    	    INIT_SDESC(tdsc, strlen(tsubj), tsubj);
    	    for (shdr = subjq.head; shdr != (struct HDR * ) &subjq; shdr = shdr->flink) {
    	    	INIT_SDESC(sdsc, strlen(shdr->str), shdr->str);
    	    	if (OK(str$match_wild(&tdsc, &sdsc))) {
    	    	    doit = 1;
    	    	    break;
    	    	}
    	    }
    	    if (doit) {
    	    	status = Retrieve_Article(cur_artnum, &cur_artctx, tmp, xref_hdr, tmp);
    	    	Mark_Article_Seen(curgroup, cur_artnum);
    	    	if (OK(status)) {
    	    	    default_action = cmd_readcont;
    	    	    cleanup_action = Clear_ArtInProg;
    	    	    if (xref_hdr[0] != '\0') Mark_Xref(xref_hdr, 0);
    	    	    Article_In_Progress = 1;
    	    	    saved_default_action = cont_readsubj;
    	    	    status = Show_Article_Page(&cur_artctx);
    	    	    if (status == NEWS__EOARTICLE) {
    	    	    	default_action = cont_readsubj;
    	    	    	cleanup_action = Clear_ArtInProg;
    	    	    }
    	    	    break;
    	    	}
    	    }
    	}

    	while (1) {
    	    cur_artnum++;
    	    if (cur_artnum > curgroup->lastavl) break;
    	    if (!Is_Valid_Article(curgroup, cur_artnum)) continue;
    	    if (!Ignore_Article(curgroup, cur_artnum)) break;
    	}

    }

    if (cur_artnum > curgroup->lastavl) {
    	lib$signal(NEWS__NOMOREMATCH, 0);
    	while (queue_remove(subjq.head, &hdr)) mem_freehdr(hdr);
    	cur_artnum = thread_base;
    	thread_base = 0;
    	default_action = cmd_readnextnew;
    	cleanup_action = (unsigned int (*)()) 0;
    }

    return SS$_NORMAL;

} /* cmd_readsubj */

/*
**++
**  ROUTINE:	cont_readsubj
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The continuation of a subject-based read (when invoked as
**  a default_action).
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cont_readsubj()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cont_readsubj() {

    struct GRP *g;
    char xref_hdr[STRING_SIZE], tmp[STRING_SIZE], *tsubj;
    struct HDR *hdr, *shdr;
    struct QUE *hdrqptr;
    struct dsc$descriptor tdsc, sdsc;
    unsigned int status;
    int doit;

    Clear_ArtInProg();


    if (subjq.head == &subjq) {
    	lib$signal(NEWS__NOSUBJSPEC, 0);
    	return SS$_NORMAL;
    }

    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	while (queue_remove(subjq.head, &hdr)) mem_freehdr(hdr);
    	return SS$_NORMAL;
    }   
    xref_hdr[0] = '\0';
    while (1) {
    	if (cur_artnum >= curgroup->lastavl) {
    	    lib$signal(NEWS__NOMOREMATCH, 0);
    	    while (queue_remove(subjq.head, &hdr)) mem_freehdr(hdr);
    	    cur_artnum = thread_base;
    	    thread_base = 0;
    	    default_action = cmd_readnextnew;
    	    cleanup_action = (unsigned int (*)()) 0;
    	    return SS$_NORMAL;
    	}

    	if (cur_artnum < curgroup->frstavl) {
    	    cur_artnum = curgroup->frstavl;
    	} else {
    	    cur_artnum += 1;
    	}
    	if (!Is_Valid_Article(curgroup, cur_artnum)) continue;
    	if (!Ignore_Article(curgroup, cur_artnum)) break;
    }

    while (cur_artnum <= curgroup->lastavl) {
    	if (OK(Get_One_Header(curgroup, cur_artnum, NEWS_K_HDR_SUBJECT, &hdr))) {
    	    if (hdr != 0) {
    	    	strcpy(tmp, hdr->str);
    	    	upcase(tmp);
    	    	if (strstr(tmp, "RE:") == tmp) {
    	    	    for (tsubj = tmp+3; isspace(*tsubj); tsubj++);
    	    	} else {
    	    	    tsubj = tmp;
    	    	}
    	    } else {
    	    	tsubj = "(none)";
    	    }

    	    doit = 0;
    	    INIT_SDESC(tdsc, strlen(tsubj), tsubj);
    	    for (shdr = subjq.head; shdr != (struct HDR * ) &subjq; shdr = shdr->flink) {
    	    	INIT_SDESC(sdsc, strlen(shdr->str), shdr->str);
    	    	if (OK(str$match_wild(&tdsc, &sdsc))) {
    	    	    doit = 1;
    	    	    break;
    	    	}
    	    }
    	    if (doit) {
    	    	status = Retrieve_Article(cur_artnum, &cur_artctx, tmp, xref_hdr, tmp);
    	    	Mark_Article_Seen(curgroup, cur_artnum);
    	    	if (OK(status)) {
    	    	    default_action = cmd_readcont;
    	    	    cleanup_action = Clear_ArtInProg;
    	    	    if (xref_hdr[0] != '\0') Mark_Xref(xref_hdr, 0);
    	    	    Article_In_Progress = 1;
    	    	    saved_default_action = cont_readsubj;
    	    	    status = Show_Article_Page(&cur_artctx);
    	    	    if (status == NEWS__EOARTICLE) {
    	    	    	default_action = cont_readsubj;
    	    	    	cleanup_action = Clear_ArtInProg;
    	    	    }
    	    	    break;
    	    	}
    	    }
    	}

    	while (1) {
    	    cur_artnum++;
    	    if (cur_artnum > curgroup->lastavl) break;
    	    if (!Is_Valid_Article(curgroup, cur_artnum)) continue;
    	    if (!Ignore_Article(curgroup, cur_artnum)) break;
    	}

    }

    if (cur_artnum > curgroup->lastavl) {
    	lib$signal(NEWS__NOMOREMATCH, 0);
    	while (queue_remove(subjq.head, &hdr)) mem_freehdr(hdr);
    	cur_artnum = thread_base;
    	thread_base = 0;
    	default_action = cmd_readnextnew;
    	cleanup_action = (unsigned int (*)()) 0;
    }

    return SS$_NORMAL;

} /* cont_readsubj */

/*
**++
**  ROUTINE:	cmd_current
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The CURRENT command.  Re-displays the current
**  article.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_current()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_current() {

    unsigned int status;
    char tmp[STRING_SIZE];
    int back_to_base;

    back_to_base = cli_present("THREAD_BASE") == CLI$_PRESENT;

    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }

    if (back_to_base) {
    	if (thread_base != 0) cur_artnum = thread_base;
    }

    if (cur_artnum == 0) {
    	lib$signal(NEWS__NOCURART, 0);
    } else {
    	Read_Full_Headers = cli_present("FULL_HEADERS") == CLI$_PRESENT;
    	rotate_text = cli_present("ROTATE") == CLI$_PRESENT;
    	if (cur_artctx == 0) {
    	    status = Retrieve_Article(cur_artnum, &cur_artctx, thread_subject,
    	    	    	    tmp, tmp);
    	} else {
    	    Rewind_Article(&cur_artctx);
    	    status = SS$_NORMAL;
    	}

    	if (OK(status)) {
    	    if (thread_base == 0) thread_base = cur_artnum;
    	    Article_In_Progress = 1;
    	    if (cli_present("EDIT") == CLI$_PRESENT) {
    	    	status = Edit_Article(&cur_artctx);
    	    	default_action = saved_default_action;
    	    } else {
    	    	status = Show_Article_Page(&cur_artctx);
    	    	default_action = (status == NEWS__EOARTICLE) ?
    	    	    saved_default_action : cmd_readcont;
    	    }
    	    cleanup_action = Clear_ArtInProg;
    	} else {
    	    Clear_ArtInProg();
    	}
    }

    return SS$_NORMAL;

} /* cmd_current */

/*
**++
**  ROUTINE:	cmd_skipthread
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The SET SEEN/THREAD command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_skipthread()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_skipthread() {

    struct GRP *g;
    char tmp[STRING_SIZE], *tsubj;
    struct HDR *hdr, *xhdr;
    struct QUE *hdrqptr;
    unsigned int status;
    int count, artnum, unsee, mark_xref, doit;
    int strict;

    Clear_ArtInProg();
    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }
    if (thread_base == 0) {
    	lib$signal(NEWS__NOTHREAD, 0);
    	return SS$_NORMAL;
    }

    if (thread_msgid[0] == '\0' && thread_subject[0] == '\0') {
    	lib$signal(NEWS__NOSUBJECT, 0);
    	return SS$_NORMAL;
    }

    strict = cli_present("STRICT") == CLI$_PRESENT;

    unsee = (cli_present("SEEN") == CLI$_NEGATED) ||
    	    (cli_present("UNSEEN") == CLI$_PRESENT);

    mark_xref = (cli_present("CROSS_REFERENCES") == CLI$_PRESENT);

    count = 0;
    for (artnum = (cur_artnum < curgroup->frstavl ? curgroup->frstavl : cur_artnum);
    	    artnum <= curgroup->lastavl; artnum++) {
    	if (User_Interrupt) break;
    	if (!Is_Valid_Article(curgroup, artnum)) continue;
    	if (!Ignore_Article(curgroup, artnum)) {
    	    doit = 0;
    	    if (OK(Get_One_Header(curgroup, artnum, NEWS_K_HDR_REFERENCES, &hdr))) {
    	    	if (hdr != 0) {
    	    	    doit = Thread_Match(hdr->str, thread_msgid);
    	    	} else if (OK(Get_One_Header(curgroup, artnum,
    	    	    	    	    NEWS_K_HDR_MESSAGE_ID, &hdr))) {
    	    	    if (hdr != 0) doit = Thread_Match(thread_msgid, hdr->str);
    	    	}
    	    }
    	    if (!doit && !strict &&
    	    	    OK(Get_One_Header(curgroup, artnum, NEWS_K_HDR_SUBJECT, &hdr))) {
    	    	tsubj = NULL;
    	    	if (hdr != 0) {
    	    	    strcpy(tmp, hdr->str);
    	    	    upcase(tmp);
    	    	    if (tmp[0] == 'R' && tmp[1] == 'E' && tmp[2] == ':') {
    	    	    	for (tsubj = tmp+3; isspace(*tsubj); tsubj++);
    	    	    } else {
    	    	    	tsubj = tmp;
    	    	    }
    	    	}

    	    	if (tsubj == NULL) continue;

    	    	doit = (strstr(tsubj, thread_subject) != 0) ||
    	    	    	(strstr(thread_subject, tsubj) != 0);
    	    }

    	    if (doit) {
    	    	count += (unsee ? Mark_Article_Unseen(curgroup, artnum) :
    	    	    	    	Mark_Article_Seen(curgroup, artnum));
    	    	if (mark_xref) {
    	    	    if (OK(Get_One_Header(curgroup, artnum, NEWS_K_HDR_XREF, &xhdr))) {
    	    	    	if (xhdr != 0) Mark_Xref(xhdr->str, unsee);
    	    	    }
    	    	}
    	    }
    	}
    }

    lib$signal(NEWS__MARKCOUNT, 3, count, (unsee ? 2 : 0), (unsee ? "un" : ""));

    return SS$_NORMAL;

} /* cmd_skipthread */

/*
**++
**  ROUTINE:	cmd_skipsubj
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The SET SEEN/SUBJECT command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_skipsubj()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_skipsubj() {

    struct GRP *g;
    char tmp[STRING_SIZE], *cp;
    struct HDR *hdr, *subjhdr, *xhdr;
    struct QUE tmpq, *hdrqptr;
    struct dsc$descriptor tdsc, sdsc;
    unsigned int status;
    int count, artnum, unsee, mark_xref;
    int rstart, rend;

    Clear_ArtInProg();
    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }

    tmpq.head = tmpq.tail = &tmpq;

    if (OK(cli_get_value("SUBJECT", tmp+1, sizeof(tmp)-1))) {
    	do {
    	    upcase(tmp+1);
    	    if (!strchr(tmp+1, '*') && !strchr(tmp+1, '%')) {
    	    	tmp[0] = '*';
    	    	cp = tmp + strlen(tmp);
    	    	*cp++ = '*';
    	    	*cp++ = '\0';
    	    	cp = tmp;
    	    } else {
    	    	cp = tmp+1;
    	    }
    	    insert_header(cp, tmpq.tail, 0);
    	} while (OK(cli_get_value("SUBJECT", tmp+1, sizeof(tmp)-1)));
    }

    if (tmpq.head == &tmpq) {
    	lib$signal(NEWS__NOSUBJSPEC, 0);
    	return SS$_NORMAL;
    }

    unsee = (cli_present("SEEN") == CLI$_NEGATED) ||
    	    (cli_present("UNSEEN") == CLI$_PRESENT);

    mark_xref = (cli_present("CROSS_REFERENCES") == CLI$_PRESENT);

    count = 0;

/*
**  Must be able to handle just SET SEEN/SUBJ (implies current->last article)
**  and specification of article ranges.
*/
    while (1) {

    	if (cli_present("ARTRNG") == CLI$_PRESENT) {
    	    if (!OK(cli_get_value("ARTRNG", tmp, sizeof(tmp)))) break;
    	    if (!OK(Parse_Range(tmp, &rstart, &rend, cur_artnum, curgroup))) {
    	    	lib$signal(NEWS__INVARTRNG, 2, strlen(tmp), tmp);
    	    } else {
    	    	int i;
    	    	if (rstart > rend) {
    	    	    i = rstart;
    	    	    rstart = rend;
    	    	    rend = i;
    	    	}
    	    }
    	    if (rend > curgroup->lastavl) rend = curgroup->lastavl;
    	} else {
    	    rstart = cur_artnum;
    	    rend = curgroup->lastavl;
    	}

    	for (artnum = (rstart < curgroup->frstavl ? curgroup->frstavl : rstart);
    	    	    	    	    	    	artnum <= rend; artnum++) {
    	    if (User_Interrupt) break;
    	    if (!Is_Valid_Article(curgroup, artnum)) continue;
    	    if (!Ignore_Article(curgroup, artnum)) {

    	    	if (!OK(Get_One_Header(curgroup, artnum, NEWS_K_HDR_SUBJECT,
    	    	    	    	&subjhdr))) subjhdr = 0;
    	    	if (subjhdr != 0) {

    	    	    strncpy(tmp, subjhdr->str, sizeof(tmp)-1);
    	    	    tmp[sizeof(tmp)-1] = '\0';
    	    	    upcase(tmp);
    	    	    INIT_SDESC(tdsc, strlen(tmp), tmp);
    	    	    for (hdr = tmpq.head; hdr != (struct HDR *) &tmpq; hdr = hdr->flink) {
    	    	    	INIT_SDESC(sdsc, strlen(hdr->str), hdr->str);
    	    	    	if (OK(str$match_wild(&tdsc, &sdsc))) {
    	    	    	    count += (unsee ? Mark_Article_Unseen(curgroup, artnum) :
    	    	    	    	    Mark_Article_Seen(curgroup, artnum));
    	    	    	    if (mark_xref) {
    	    	    	    	if (OK(Get_One_Header(curgroup, artnum,
    	    	    	    	    	NEWS_K_HDR_XREF, &xhdr))) {
    	    	    	    	    if (xhdr != 0) Mark_Xref(xhdr->str, unsee);
    	    	    	    	}
    	    	    	    }
    	    	    	    break;
    	    	    	}
    	    	    }
    	    	}
    	    }
    	}

    	if (cli_present("ARTRNG") != CLI$_PRESENT) break;

    }

    while (queue_remove(tmpq.head, &hdr)) mem_freehdr(hdr);
    lib$signal(NEWS__MARKCOUNT, 3, count, (unsee ? 2 : 0), (unsee ? "un" : ""));

    return SS$_NORMAL;

} /* cmd_skipsubj */

/*
**++
**  ROUTINE:	cmd_group
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The GROUP command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_group()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_group() {

    char tmp[STRING_SIZE];
    unsigned int status;

    cli_get_value("GRPNAM", tmp, sizeof(tmp));
    status = Find_And_Set_Current_Group(tmp);
    if (OK(status)) default_action = cmd_readnew;

    return status;

} /* cmd_group */

/*
**++
**  ROUTINE:	Find_And_Set_Current_Group
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Actually does the real work of a GROUP command... well,
**  at least the lookup part.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Find_And_Set_Current_Group(char *grpnam)
**
** grpnam: character string, read only, by reference (ASCIZ)
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
static unsigned int Find_And_Set_Current_Group(char *grpnam) {

    struct GRP *g;
    char tmp[STRING_SIZE];
    unsigned int status, ctx;

    locase(grpnam);
    g = Find_Group(grpnam);
    if (g == NULL) {
    	strcpy(tmp, grpnam);
    	if (strchr(tmp, '*') == 0 && strchr(tmp, '%') == 0) strcat(tmp, "*");
    	ctx = 0;
    	while ((g = Find_Group_Wild(tmp, &ctx)) != 0) {
    	    if (g->subscribed) {
    	    	Traverse_Finish(&ctx);
    	    	break;
    	    }
    	}
    }
    if (g == NULL) {
    	lib$signal((status = NEWS__NOSUCHGRP), 2, strlen(grpnam), grpnam);
    } else if (!g->subscribed) {
    	lib$signal((status = NEWS__NOTSUBSCR), 2, strlen(grpnam), grpnam);
    } else {
    	status = Set_Current_Group(g);
    	if (news_prof.glist == NULL) {
    	    if (g != Current_TreeNode(&rgctx)) {
    	    	struct GRP *gtmp;
    	    	Traverse_Finish(&rgctx);
    	    	while ((gtmp = Traverse_Tree(&rgctx)) != g);
    	    }
    	}
    	lib$signal(NEWS__GRPSET, 2, strlen(curgroup->grpnam),
    	    	curgroup->grpnam, NEWS__GRPINFO, 3, curgroup->frstavl,
    	    	curgroup->lastavl, Count_Unseen(curgroup));
    }

    return SS$_NORMAL;

} /* Find_And_Set_Current_Group */

/*
**++
**  ROUTINE:	Set_Current_Group
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Makes a group the "current" group.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Set_Current_Group(struct GRP *grp)
**
** grp: GRP structure, modify, by reference
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int Set_Current_Group(struct GRP *grp) {

    char tmp[STRING_SIZE];
    int artcnt, reply_code, first, len, last;

    cur_artnum = 0;
    if (curgroup == grp) return SS$_NORMAL;

    curgroup = grp;
    strcpy(tmp, "GROUP ");
    strcat(tmp, grp->grpnam);
    server_send(tmp);
    server_get_reply(SRV__NOECHO, &reply_code, tmp, sizeof(tmp)-1, &len);
    if (reply_code != NNTP__GRPSELECTED) {
    	lib$signal(NEWS__UNEXPRSP, 3, reply_code, len, tmp);
    	return NEWS__UNEXPRSP;
    }

    tmp[len] = '\0';
    parse_group_reply(tmp, &artcnt, &first, &last);
    if (grp->valid != 0 && (first > grp->frstavl || last != grp->lastavl)) {
    	free(grp->valid);
    	grp->valid = 0;
    }
    grp->frstavl = (first > grp->frstavl) ? first : grp->frstavl;

    return SS$_NORMAL;

} /* Set_Current_Group */

/*
**++
**  ROUTINE:	cmd_extract
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The EXTRACT command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_extract()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int cmd_extract() {

    char fspec[FSPEC_SIZE], rspec[FSPEC_SIZE], tmpfile[FSPEC_SIZE];
    char tmp[STRING_SIZE], tmp2[STRING_SIZE], gnam[STRING_SIZE];
    struct dsc$descriptor dsc1, dsc2;
    struct QUE rangeq, *hdrq;
    struct RNG *rng;
    struct HDR *hdr;
    struct GRP *g, *curgroup_save;
    int rstart, rend, did_one, do_headers, rotate, append, i, tlen, full_headers;
    int do_new, do_mark, do_group, do_wild, do_file, do_ignore;
    unsigned int unit, unit2, status, gctx;
    $DESCRIPTOR(underscores, "______________________");
    $DESCRIPTOR(specials,    "!\"#%&'()*+,/@[\\]^{|}~.");
    $DESCRIPTOR(alphabet, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    $DESCRIPTOR(rot13bet, "NOPQRSTUVWXYZABCDEFGHIJKLMnopqrstuvwxyzabcdefghijklm");

    do_group = cli_present("GROUP") == CLI$_PRESENT;

    if (!do_group && curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }

    do_file = cli_present("FSPEC") == CLI$_PRESENT;
    if (do_file) {
    	cli_get_value("FSPEC", fspec, sizeof(fspec));
    	append = cli_present("APPEND") == CLI$_PRESENT;
    }

    rangeq.head = rangeq.tail = &rangeq;

    do_headers = cli_present("HEADERS") != CLI$_NEGATED;
    if (do_headers) {
    	if (OK(cli_get_value("HEADERS", tmp, sizeof(tmp)))) {
    	    full_headers = tmp[0] == 'F';
    	} else full_headers = 0;
    } else full_headers = 0;

    rotate = cli_present("ROTATE") == CLI$_PRESENT;
    do_new = cli_present("NEW") == CLI$_PRESENT;
    do_mark = cli_present("MARK") == CLI$_PRESENT;
    do_ignore = cli_present("IGNORE") == CLI$_PRESENT;

    if (cli_present("ARTRNG") == CLI$_PRESENT) {
    	while (OK(cli_get_value("ARTRNG", tmp, sizeof(tmp)))) {
    	    if (!OK(Parse_Range(tmp, &rstart, &rend, cur_artnum, curgroup))) {
    	    	lib$signal(NEWS__INVARTRNG, 2, strlen(tmp), tmp);
    	    	while (queue_remove(rangeq.head, &rng)) mem_freerng(rng);
    	    	return SS$_NORMAL;
    	    }
    	    if (rstart > rend) {
    	    	lib$signal(NEWS__INVARTRNG, 2, strlen(tmp), tmp);
    	    	while (queue_remove(rangeq.head, &rng)) mem_freerng(rng);
    	    	return SS$_NORMAL;
    	    }
    	    if (!do_group && (rend > curgroup->lastavl || rend < curgroup->frstavl)) {
    	    	lib$signal(NEWS__OUTRNG, 3, rend, curgroup->frstavl,
    	    	    	    curgroup->lastavl);
    	    	while (queue_remove(rangeq.head, &rng)) mem_freerng(rng);
    	    	return SS$_NORMAL;
    	    }
    	    if (!do_group && (rstart > curgroup->lastavl || rstart < curgroup->frstavl)) {
    	    	lib$signal(NEWS__OUTRNG, 3, rstart, curgroup->frstavl,
    	    	    	    curgroup->lastavl);
    	    	while (queue_remove(rangeq.head, &rng)) mem_freerng(rng);
    	    	return SS$_NORMAL;
    	    }
    	    rng = mem_getrng();
    	    rng->first = rstart;
    	    rng->last = rend;
    	    queue_insert(rng, rangeq.tail);
    	}
    } else if (!do_new) {
    	if (cur_artnum == 0) {
    	    lib$signal(NEWS__NOCURART, 0);
    	    return SS$_NORMAL;
    	}
    	rng = mem_getrng();
    	rng->first = rng->last = cur_artnum;
    	queue_insert(rng, rangeq.tail);
    } else {
    	rng = mem_getrng();
    	rng->first = 1;
    	rng->last = 9999999;
    	queue_insert(rng, rangeq.tail);
    }

    if (do_file) {
    	if (append) {
    	    status = file_exists(fspec, news_prof.archive);
    	    if (!OK(status)) append = 0;
    	}

    	did_one = append;

    	if (append) {
    	    status = file_append(fspec, &unit, news_prof.archive, rspec, 0);
    	} else {
    	    status = file_create(fspec, &unit, news_prof.archive, rspec);
    	}
    	if (!OK(status)) {
    	    lib$signal(NEWS__OPENERR, 2, strlen(fspec), fspec, status);
    	    while (queue_remove(rangeq.head, &rng)) mem_freerng(rng);
    	    return SS$_NORMAL;
    	}
    }

    g = 0;
    gctx = 0;
    curgroup_save = curgroup;

    while (1) {
    	int count;

    	if (User_Interrupt) break;

    	if (do_group) {
    	    if (gctx == 0) {
    	    	if (!OK(cli_get_value("GROUP", gnam, sizeof(gnam)))) break;
    	    	locase(gnam);
    	    	do_wild = strchr(gnam, '%' != 0) || strchr(gnam, '*') != 0;
    	    	if (do_wild) g = Find_Group_Wild(gnam, &gctx);
    	    	else g = Find_Group(gnam);
    	    } else g = Find_Group_Wild(gnam, &gctx);
    	    if (g == 0) continue;
    	    if (do_wild && !g->subscribed) continue;
    	    status = Set_Current_Group(g);
    	    if (!OK(status)) continue;
    	} else {
    	    if (g == curgroup) break;
    	    g = curgroup;
    	}

    	if (!do_file) {
    	    INIT_SDESC(dsc1, strlen(curgroup->grpnam), fspec);
    	    INIT_SDESC(dsc2, strlen(curgroup->grpnam), curgroup->grpnam);
    	    str$translate(&dsc1, &dsc2, &underscores, &specials);
    	    strcpy(fspec+strlen(curgroup->grpnam), ".TXT");
	    append = cli_present("APPEND") != CLI$_NEGATED;

    	    if (append) {
    	    	status = file_exists(fspec, news_prof.archive);
    	    	if (!OK(status)) append = 0;
    	    }

    	    did_one = append;
    	    unit = 0;

    	    if (append) {
    	    	status = file_append(fspec, &unit, news_prof.archive, rspec, 0);
    	    } else {
    	    	status = file_create(fspec, &unit, news_prof.archive, rspec);
    	    }
    	    if (!OK(status)) {
    	    	lib$signal(NEWS__OPENERR, 2, strlen(fspec), fspec, status);
    	    	break;
    	    }
    	}

    	count = 0;

    	for (rng = rangeq.head; rng != (struct RNG *) &rangeq; rng = rng->flink) {

    	    rstart = rng->first;
    	    if (rstart < g->frstavl) rstart = g->frstavl;
    	    if (rstart > g->lastavl) rstart = g->lastavl;
    	    rend = rng->last;
    	    if (rend < g->frstavl) rend = g->frstavl;
    	    if (rend > g->lastavl) rend = g->lastavl;

    	    if (User_Interrupt) break;

    	    status = SS$_NORMAL;

    	    for (i = do_new ? Next_Unseen_Article(rstart-1, 0, 0) : rstart;
    	    	    	i != 0 && i <= rend;
    	    	    	i = do_new ? Next_Unseen_Article(i, 0, 0) : i+1) {

    	    	if (User_Interrupt) break;

    	    	if (!Is_Valid_Article(g, i)) continue;
    	    	if (do_ignore && Ignore_Article(g, i)) continue;
    	    	if (do_mark) Mark_Article_Seen(g, i);
    	    	if (do_headers) {
    	    	    status = Get_Article_Hdrs(g, i, &hdrq, 1, 0);
    	    	    if (!OK(status)) {
    	    	    	if (status == NEWS__NOSUCHARTICLE) continue;
    	    	    	if (status == NEWS__ARTRERR) lib$signal(NEWS__EXTRERR, 1, i, status, 0);
    	    	    	else lib$signal(NEWS__EXTRERR, 1, i, status);
    	    	    	break;
    	    	    }
    	    	}

    	    	if (!do_headers || OK(status)) {
    	    	    make_temp_fspec(tmp, sizeof(tmp));
    	    	    status = Get_Article_Body(i, tmp, tmpfile, 0, 0);
    	    	    if (!OK(status)) {
    	    	    	file_delete(tmp);
    	    	    	if (status == NEWS__NOSUCHARTICLE) continue;
    	    	    	if (status == NEWS__ARTRERR) lib$signal(NEWS__EXTRERR, 1, i, status, 0);
    	    	    	else lib$signal(NEWS__EXTRERR, 1, i, status);
    	    	    	break;
    	    	    } else {
    	    	    	count++;
    	    	    	if (did_one) {
    	    	    	    if (do_headers) {
    	    	    	    	file_write(unit,
"================================================================================", 80);
    	    	    	    }
    	    	    	} else did_one = 1;
    	    	    	if (do_headers) {
    	    	    	    int len;
    	    	    	    struct HDR *h;
    	    	    	    if (full_headers) {
    	    	    	    	len = sprintf(tmp, "Local-Article-ID: %s:%s#%d",
    	    	    	    	    news_cfg.server_name, curgroup->grpnam, i);
    	    	    	    	file_write(unit, tmp, len);
    	    	    	    }
    	    	    	    for (hdr = hdrq->head;  hdr != (struct HDR *) hdrq;
    	    	    	    	    	    	    	hdr = hdr->flink) {
    	    	    	    	if (full_headers) {
    	    	    	    	    Format_Header(hdr, tmp, sizeof(tmp), &len);
    	    	    	    	    status = file_write(unit, tmp, len);
    	    	    	    	} else {
    	    	    	    	    status = SS$_NORMAL;
    	    	    	    	    for (h = news_prof.ehdrlist.head;
    	    	    	    	    	h != (struct HDR *) &news_prof.ehdrlist;
    	    	    	    	    	h = h->flink) {
    	    	    	    	    	if (h->code == hdr->code) {
    	    	    	    	    	    Format_Header(hdr, tmp, sizeof(tmp), &len);
    	    	    	    	    	    status = file_write(unit, tmp, len);
    	    	    	    	    	    break;
    	    	    	    	    	}
    	    	    	    	    }
    	    	    	    	}
    	    	    	    	if (!OK(status)) break;
    	    	    	    }
    	    	    	    if (OK(status)) status = file_write(unit, "", 0);
    	    	    	}
    	    	    	if (OK(status)) status = file_open(tmpfile, &unit2, 0, 0, 0);
    	    	    	if (!OK(status)) {
    	    	    	    lib$signal(NEWS__EXTRERR, 1, i, status);
    	    	    	    file_delete(tmpfile);
    	    	    	    break;
    	    	    	}
    	    	    	while (OK(file_read(unit2, tmp, sizeof(tmp)-1, &tlen))) {
    	    	    	    tmp[tlen] = '\0';
    	    	    	    if (rotate) {
    	    	    	    	INIT_SDESC(dsc1, strlen(tmp), tmp2);
    	    	    	    	INIT_SDESC(dsc2, strlen(tmp), tmp);
    	    	    	    	str$translate(&dsc1, &dsc2, &alphabet, &rot13bet);
    	    	    	    	*(tmp2+strlen(tmp)) = '\0';
    	    	    	    	status = file_write(unit, tmp2, dsc1.dsc$w_length);
    	    	    	    } else status = file_write(unit, tmp, strlen(tmp));
    	    	    	    if (!OK(status)) break;
    	    	    	}
    	    	    	file_close(unit2);
    	    	    	file_delete(tmpfile);
    	    	    	if (!OK(status)) {
    	    	    	    lib$signal(NEWS__EXTRERR, 1, i, status);
    	    	    	    break;
    	    	    	}
    	    	    }
    	    	}
    	    } /* for i */
    	    if (!OK(status)) break;
    	} /* for rng */

    	if (!do_file && unit != 0) {
    	    file_close(unit);
    	    unit = 0;
    	}

    	if (!OK(status)) break;

    	lib$signal(NEWS__EXTRACTED, 3, count, strlen(curgroup->grpnam),
    	    	    	    curgroup->grpnam);

    } /* loop through groups */

    if (do_file) file_close(unit);
    while (queue_remove(rangeq.head, &rng)) mem_freerng(rng);
    if (gctx != 0) Traverse_Finish(&gctx);
    if (!did_one && !append) file_delete(rspec);
    if (curgroup != curgroup_save) {
    	if (curgroup_save == 0) curgroup = curgroup_save;
    	else Set_Current_Group(curgroup_save);
    }

    return SS$_NORMAL;

} /* cmd_extract */

/*
**++
**  ROUTINE:	Clear_ArtInProg
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Clears the article read currently in progress.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Clear_ArtInProg()
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
unsigned int Clear_ArtInProg() {

    if (Article_In_Progress) {
    	if (cur_artctx != 0) {
    	    Close_Article(&cur_artctx);
    	}
    	Article_In_Progress = 0;
    	rotate_text = Read_Full_Headers = 0;
    	Finish_Paged_Output(0);
    	default_action = cleanup_action = (unsigned int (*)()) 0;
    }

    return SS$_NORMAL;

} /* Clear_ArtInProg */

/*
**++
**  ROUTINE:	Find_Next_Unseen
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Finds the next unseen article, following the group tree (or
**  list).
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Find_Next_Unseen(int startpoint, int *newart)
**
** startpoint: article number, read only, by value
** newart:     article number, write only, by reference
**
**  IMPLICIT INPUTS:	Several.
**
**  IMPLICIT OUTPUTS:	Several.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	Several.
**
**--
*/
static unsigned int Find_Next_Unseen(int startpoint, int *newart) {

    struct GRP *g, *hold;
    int a, b, pass2, was_curgroup;
    unsigned int status;

    if (curgroup == NULL) {
    	if (news_prof.glist) {
    	    g = news_prof.glist;
    	} else {
    	    Traverse_Finish(&rgctx);
    	    g = Traverse_Tree(&rgctx);
    	}
    	while (g != NULL) {
    	    if (g->subscribed) {
    	    	a = Next_Unseen_Article(0, 1, g);
    	    	if (a > 0) {
    	    	    status = Set_Current_Group(g);
    	    	    if (OK(status)) {
    	    	    	a = Next_Unseen_Article(0, 0, 0);
    	    	    	if (a > 0) {
    	    	    	    *newart = a;
    	    	    	    return SS$_NORMAL;
    	    	    	}
    	    	    }
    	    	}
    	    }
    	    g = news_prof.glist ? g->next : Traverse_Tree(&rgctx);
    	}
    } else {
    	g = hold = curgroup;
    	a = startpoint;
    	pass2 = 0;
    	do {
    	    if (g->subscribed) {
    	    	if ((b = Next_Unseen_Article(a, 1, g)) > 0) {
    	    	    status = Set_Current_Group(g);
    	    	    if (OK(status)) {
    	    	    	a = Next_Unseen_Article(a, 0, 0);
    	    	    	if (a > 0) {
    	    	    	    *newart = a;
    	    	    	    return SS$_NORMAL;
    	    	    	}
    	    	    }
    	    	}
    	    }
    	    g = news_prof.glist ? g->next : Traverse_Tree(&rgctx);
    	    if (g == NULL && !pass2) {
    	    	g = news_prof.glist ? news_prof.glist : Traverse_Tree(&rgctx);
    	    	pass2 = 1;
    	    }
    	    a = 0;
    	} while (g != NULL);

    	if (hold->subscribed) {
    	    status = Set_Current_Group(hold);
    	    if (OK(status)) {
    	    	a = Next_Unseen_Article(0, 0, 0);
    	    	if (a > 0 && a != startpoint) {
    	    	    *newart = a;
    	    	    return SS$_NORMAL;
    	    	}
    	    }
    	}
    }

    return NEWS__NONEW;

} /* Find_Next_Unseen */
