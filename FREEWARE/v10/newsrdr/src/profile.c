/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Routines dealing with NEWSRDR profiles.
**
**  MODULE DESCRIPTION:
**
**  	This module contains the routines that read and write
**  NEWSRDR profiles.
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
**  	11-SEP-1992 V1.1    Madison 	Added NEWSRC support.
**  	28-SEP-1992 V1.1-1  Madison 	Header settings weren't stored properly.
**  	29-SEP-1992 V1.1-2  Madison 	Default headers weren't set properly.
**  	12-APR-1993 V1.2    Madison 	Added autoreadnew, usemailpnam flags.
**  	16-APR-1993 V1.2-1  Madison 	Support for faked NEWGROUPS.
**  	23-APR-1993 V1.2-2  Madison 	Add support for force_proto, ngaction.
**  	25-MAY-1993 V1.2-3  Madison 	Make sure directpost flag is 1.
**  	22-JUN-1993 V1.3    Madison 	Don't free stuff on updates.
**  	24-SEP-1993 V1.4    Madison 	mem_getgrp now inits queues.
**  	09-OCT-1993 V1.5    Madison 	Added extract_hdrs support.
**  	01-DEC-1993 V1.5-1  Madison 	Fix captive flag check for OpenVMS AXP.
**  	16-MAY-1994 V1.6    Madison 	reply-prefix suppport.
**  	22-MAY-1994 V1.7    Madison 	"Safer" profile writing code.
**--
*/
#include "newsrdr.h"
#include "globals.h"
#ifdef __GNUC__
#include <vms/rms.h>
#include <vms/syidef.h>
#include <vms/uaidef.h>
#include <vms/maildef.h>
#include <vms/prvdef.h>
#else
#include <rms.h>
#include <syidef.h>
#include <uaidef.h>
#include <maildef.h>
#include <prvdef.h>
#endif

#ifndef UAI$M_RESTRICTED
#define UAI$M_RESTRICTED UAI$M_CAPTIVE
#endif

    unsigned int Read_Profile(void);
    unsigned int Write_Profile(void);
    static unsigned int write_grp_info(struct GRP *, struct RAB *);

    extern void Read_NewsRC(void);
    extern void Write_NewsRC(void);

/*
** Basically, profile information is kept in a private tagged-record
** format, so it's easy to add more profile items in a backward-compatible
** way.  The tags are binary (and so is the information, in a lot of
** cases), so users keep their noses out of it.
*/

#define PFILE__LASTGRPCHK    201
#define PFILE__GROUP	     315
#define PFILE__GROUP_NOSUB   316
#define PFILE__RANGELIST     739
#define PFILE__KILLLIST	     822
#define PFILE__HEADER	     421
#define PFILE__NEWHEADER     351
#define PFILE__PERSONALNAME  167
#define PFILE__EDITORNAME    293
#define PFILE__SIGFILENAME   386
#define PFILE__FLAGS	     550
#define PFILE__GLOBALKILL    679
#define PFILE__ARCHIVE	     484
#define PFILE__NEWSRC	     796
#define PFILE__QUEUE	     809
#define PFILE__FORM           46
#define PFILE__DIRWIDTH       98
#define PFILE__NGACTION	     145
#define PFILE__EXTRACT_HDRS  504
#define PFILE__REPLY_PREFIX  642

    struct PFREC {
    	int code;
    	union {
    	    TIME lgc;
    	    struct {
    	    	short textlen;
    	    	short hdrcode;
    	    	char text[256];
    	    } txt;
    	    struct {
    	    	int rcount;
    	    	int range[1];
    	    } rng;
    	    struct {
    	    	int fromwid;
    	    	int subjwid;
    	    } wid;
    	    unsigned int flags;
    	} u;
    };

#define PFREC_S_FRONT	    (sizeof(int)+sizeof(short)*2)

#define PFREC_M_EDIT	    1
#define PFREC_M_AUTOSIG	    2
#define PFREC_M_SPAWNEDIT   4
#define PFREC_M_NEWFLAGS    8
#define PFREC_M_AUTOSIGP    16
#define PFREC_M_AUTOSIGM    32
#define PFREC_M_REPLYP	    64
#define PFREC_M_REPLYM	    128
#define PFREC_M_NOAUTORN    256
#define PFREC_M_USEMAILPN   512
#define PFREC_M_FORCEPROTO  1024
#define PFREC_M_AUTOSAVE    2048
#define PFREC_M_RCNOPURGE   4096

#define MAXRCT ((sizeof(struct PFREC)-8)/8)
#define MAXHCT ((sizeof(struct PFREC)-8)/4)

    static int default_header[] = {NEWS_K_HDR_FROM, NEWS_K_HDR_DATE,
    	    	    NEWS_K_HDR_NEWSGROUPS, NEWS_K_HDR_SUBJECT,
    	    	    NEWS_K_HDR_MESSAGE_ID, NEWS_K_HDR_DISTRIBUTION,
    	    	    NEWS_K_HDR_ORGANIZATION, NEWS_K_HDR_LINES};
    static char *hdrname[] = {"",
    	"FROM", "DATE", "NEWSGROUPS", "SUBJECT", "MESSAGE-ID",
    	"PATH", "REPLY-TO", "SENDER", "FOLLOWUP-TO", "EXPIRES",
    	"REFERENCES", "CONTROL", "DISTRIBUTION", "ORGANIZATION",
    	"KEYWORDS", "SUMMARY", "APPROVED", "LINES", "XREF"};

    extern void Insert_Group(struct GRP *);
    extern unsigned int mail$user_begin(), mail$user_get_info(), mail$user_end();

/*
**++
**  ROUTINE:	Read_Profile
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Reads in a profile, filling in the appropriate bits of
**  the in-memory profile structure (news_prof) and the group tree.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Read_Profile()
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
unsigned int Read_Profile() {

    ITMLST uailst[2], mailst[2], mailst3[5], mailnul[1];
    struct FAB fab;
    struct RAB rab;
    struct PFREC pfr;
    struct GRP *g;
    struct RNG *rng;
    struct HDR *h, *h2;
    unsigned int uaflags;
    char tmp[STRING_SIZE];
    struct dsc$descriptor sdsc;
    short tlen;
    int vms_v52, i;
    unsigned int mailctx, status;

    vms_v52 = 0;
    memset(&news_prof, 0, sizeof(news_prof));
    news_prof.reply_prefix_len = -1;

/*
** For some reason the captive flag is part of news_prof instead of
** news_cfg.  That should probably change.
*/
#ifdef __ALPHA
    vms_v52 = 1;
#else
    ITMLST_INIT(uailst[0], SYI$_VERSION, sizeof(tmp), tmp, &tlen);
    ITMLST_INIT(uailst[1], 0, 0, 0, 0);
    status = sys$getsyiw(0, 0, 0, uailst, 0, 0, 0);
    if (OK(status) && tlen > 4) {
    	vms_v52 = tmp[1] > '5' || (tmp[1] == '5' && tmp[3] >= '2');
    }
#endif
    ITMLST_INIT(uailst[0], UAI$_FLAGS, sizeof(uaflags), &uaflags, 0);
    ITMLST_INIT(uailst[1], 0, 0, 0, 0);
    INIT_SDESC(sdsc, strlen(news_cfg.username), news_cfg.username);
    status = sys$getuai(0, 0, &sdsc, uailst, 0, 0, 0);
#ifdef __ALPHA
    news_prof.captive = (uaflags & UAI$M_CAPTIVE);
#else
    news_prof.captive = (uaflags & UAI$M_RESTRICTED) && !vms_v52;
#endif

/*
** Initialize news_prof the way we want it to be, even if there's
** no profile.
*/
    news_prof.hdrlist.head = news_prof.hdrlist.tail = &news_prof.hdrlist;
    news_prof.ehdrlist.head = news_prof.ehdrlist.tail = &news_prof.ehdrlist;
    news_prof.killq.head = news_prof.killq.tail = &news_prof.killq;
    sys$gettim(&news_prof.lastgrpchk);
    strcpy(news_prof.print_queue, "SYS$PRINT");
    news_prof.dir_fromwid = news_prof.dir_subjwid = -1;
    news_prof.usemailpnam = 1;
    news_prof.ngaction = NG_K_NONE;

/*
** Open it up and read in the information.
*/
    fab = cc$rms_fab;
    fab.fab$b_fac = FAB$M_GET;
    fab.fab$l_fna = "NEWSRDR_PROFILE";
    fab.fab$b_fns = 15;
    fab.fab$l_dna = "SYS$LOGIN:.NRPF";
    fab.fab$b_dns = 15;
    status = sys$open(&fab, 0, 0);
    if (!OK(status)) {
    	lib$signal(NEWS__NOOPNPROF, 0, status, fab.fab$l_stv);
    } else {
    	rab = cc$rms_rab;
    	rab.rab$l_fab = &fab;
    	rab.rab$l_ubf = (char *) &pfr;
    	rab.rab$w_usz = sizeof(pfr);
    	sys$connect(&rab,0,0);
/*
** g is used to keep track of the group we're currently working on,
** since groups use more than one profile record -- the one with the
** name, the seen list, and the kill list.
*/
    	g = NULL;
    	while (OK(sys$get(&rab,0,0))) {
    	    switch (pfr.code) {

    	    case PFILE__LASTGRPCHK:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	news_prof.lastgrpchk = pfr.u.lgc;
    	    	break;

    	    case PFILE__FLAGS:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	news_prof.edit = pfr.u.flags & PFREC_M_EDIT;
    	    	news_prof.spawnedit = pfr.u.flags & PFREC_M_SPAWNEDIT;
    	    	if (pfr.u.flags & PFREC_M_NEWFLAGS) {
    	    	    news_prof.autosigpost = pfr.u.flags & PFREC_M_AUTOSIGP;
    	    	    news_prof.autosigmail = pfr.u.flags & PFREC_M_AUTOSIGM;
    	    	    news_prof.replypost   = pfr.u.flags & PFREC_M_REPLYP;
    	    	    news_prof.replymail   = pfr.u.flags & PFREC_M_REPLYM;
    	    	} else {
    	    	    news_prof.autosigpost = news_prof.autosigmail =
    	    	    	    pfr.u.flags & PFREC_M_AUTOSIG;
    	    	}
    	    	news_prof.autoreadnew = !(pfr.u.flags & PFREC_M_NOAUTORN);
    	    	news_prof.usemailpnam = pfr.u.flags & PFREC_M_USEMAILPN;
    	    	news_prof.force_proto = pfr.u.flags & PFREC_M_FORCEPROTO;
    	    	news_prof.autosave = pfr.u.flags & PFREC_M_AUTOSAVE;
    	    	news_prof.newsrc_purge = !(pfr.u.flags & PFREC_M_RCNOPURGE);
    	    	break;

    	    case PFILE__NGACTION:
    	    	if (g != NULL) Insert_Group(g);
    	    	news_prof.ngaction = pfr.u.flags;
    	    	break;

    	    case PFILE__GROUP:
    	    case PFILE__GROUP_NOSUB:
    	    	if (g != NULL) Insert_Group(g);
    	    	g = mem_getgrp();
    	    	g->subscribed = (pfr.code == PFILE__GROUP);
    	    	g->inprofile = g->directpost = 1;
    	    	strncpy(g->grpnam, pfr.u.txt.text, pfr.u.txt.textlen);
    	    	*(g->grpnam+pfr.u.txt.textlen) = '\0';
    	    	break;

    	    case PFILE__RANGELIST:
    	    	if (g != NULL) {
    	    	    for (i = 0; i < pfr.u.rng.rcount; i++) {
    	    	    	rng = mem_getrng();
    	    	    	rng->first = pfr.u.rng.range[i*2];
    	    	    	rng->last  = pfr.u.rng.range[i*2+1];
    	    	    	queue_insert(rng, g->seenq.tail);
    	    	    }
    	    	}
    	    	break;

    	    case PFILE__KILLLIST:
    	    	if (g != NULL) {
    	    	    h = mem_gethdr(pfr.u.txt.textlen+1);
    	    	    h->code = pfr.u.txt.hdrcode;
    	    	    strncpy(h->str, pfr.u.txt.text, pfr.u.txt.textlen);
    	    	    *(h->str+pfr.u.txt.textlen) = '\0';
    	    	    queue_insert(h, g->killq.tail);
    	    	}
    	    	break;

    	    case PFILE__GLOBALKILL:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	h = mem_gethdr(pfr.u.txt.textlen+1);
    	    	h->code = pfr.u.txt.hdrcode;
    	    	strncpy(h->str, pfr.u.txt.text, pfr.u.txt.textlen);
    	    	 *(h->str+pfr.u.txt.textlen) = '\0';
    	    	queue_insert(h, news_prof.killq.tail);
    	    	break;

/*
** Very old versions of NEWSRDR stored the headers in plain-text format
** that had to be parsed each time.  This remains for compatibility.
*/

    	    case PFILE__HEADER:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	h = mem_gethdr(0);
    	    	for (i = 1; i < sizeof(hdrname)/sizeof(char *); i++) {
    	    	    if (strlen(hdrname[i]) == pfr.u.txt.textlen &&
    	    	    	    strncmp(hdrname[i], pfr.u.txt.text,
    	    	    	    	    	pfr.u.txt.textlen)) {
    	    	    	h->code = i;	
    	    	    	break;
    	    	    }
    	    	}
    	    	for (h2 = news_prof.hdrlist.head;
    	    	    	h2 != (struct HDR *) &news_prof.hdrlist;
    	    	    	 h2 = h2->flink) {
    	    	    if (h2->code > h->code) break;
    	    	}
    	    	if (h2 == (struct HDR *) &news_prof.hdrlist) {
    	    	    queue_insert(h, news_prof.hdrlist.tail);
    	    	} else {
    	    	    queue_insert(h, h2->blink);
    	    	}
    	    	break;

/*
** Later versions of NEWSRDR use this binary format for storing the SET HEADERS
** information.
*/
    	    case PFILE__NEWHEADER:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	for (i = 0; i < pfr.u.rng.rcount; i++) {
    	    	    if (pfr.u.rng.range[i] >= NEWS_K_HDR_LO &&
    	    	    	    pfr.u.rng.range[i] <= NEWS_K_HDR_HI) {
    	    	    	h = mem_gethdr(0);
    	    	    	h->code = pfr.u.rng.range[i];
    	    	    	for (h2 = news_prof.hdrlist.head;
    	    	    	    	h2 != (struct HDR *) &news_prof.hdrlist;
    	    	    	    	h2 = h2->flink) {
    	    	    	    if (h2->code > h->code) break;
    	    	    	}
    	    	    	if (h2 == (struct HDR *) &news_prof.hdrlist) {
    	    	    	    queue_insert(h, news_prof.hdrlist.tail);
    	    	    	} else {
    	    	    	    queue_insert(h, h2->blink);
    	    	    	}
    	    	    }
    	    	}
    	    	break;

    	    case PFILE__EXTRACT_HDRS:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	for (i = 0; i < pfr.u.rng.rcount; i++) {
    	    	    if (pfr.u.rng.range[i] >= NEWS_K_HDR_LO &&
    	    	    	    pfr.u.rng.range[i] <= NEWS_K_HDR_HI) {
    	    	    	h = mem_gethdr(0);
    	    	    	h->code = pfr.u.rng.range[i];
    	    	    	for (h2 = news_prof.ehdrlist.head;
    	    	    	    	h2 != (struct HDR *) &news_prof.ehdrlist;
    	    	    	    	h2 = h2->flink) {
    	    	    	    if (h2->code > h->code) break;
    	    	    	}
    	    	    	if (h2 == (struct HDR *) &news_prof.ehdrlist) {
    	    	    	    queue_insert(h, news_prof.ehdrlist.tail);
    	    	    	} else {
    	    	    	    queue_insert(h, h2->blink);
    	    	    	}
    	    	    }
    	    	}
    	    	break;

    	    case PFILE__PERSONALNAME:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	strncpy(news_prof.personalname, pfr.u.txt.text,
    	    	    	    pfr.u.txt.textlen);
    	    	*(news_prof.personalname+pfr.u.txt.textlen) = '\0';
    	    	break;

    	    case PFILE__EDITORNAME:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	strncpy(news_prof.editorname, pfr.u.txt.text,
    	    	    	    pfr.u.txt.textlen);
    	    	*(news_prof.editorname+pfr.u.txt.textlen) = '\0';
    	    	break;

    	    case PFILE__SIGFILENAME:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	strncpy(news_prof.sigfile, pfr.u.txt.text,
    	    	    	    pfr.u.txt.textlen);
    	    	*(news_prof.sigfile+pfr.u.txt.textlen) = '\0';
    	    	break;

    	    case PFILE__ARCHIVE:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	strncpy(news_prof.archive, pfr.u.txt.text,
    	    	    	    pfr.u.txt.textlen);
    	    	*(news_prof.archive+pfr.u.txt.textlen) = '\0';
    	    	break;

    	    case PFILE__NEWSRC:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	news_prof.newsrc_subonly = pfr.u.txt.hdrcode;
    	    	strncpy(news_prof.newsrc, pfr.u.txt.text,
    	    	    	    pfr.u.txt.textlen);
    	    	*(news_prof.newsrc+pfr.u.txt.textlen) = '\0';
    	    	break;

    	    case PFILE__QUEUE:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	strncpy(news_prof.print_queue, pfr.u.txt.text,
    	    	    	    pfr.u.txt.textlen);
    	    	*(news_prof.print_queue+pfr.u.txt.textlen) = '\0';
    	    	break;

    	    case PFILE__FORM:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	strncpy(news_prof.print_form, pfr.u.txt.text,
    	    	    	    pfr.u.txt.textlen);
    	    	*(news_prof.print_form+pfr.u.txt.textlen) = '\0';
    	    	break;

    	    case PFILE__DIRWIDTH:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	news_prof.dir_fromwid = pfr.u.wid.fromwid;
    	    	news_prof.dir_subjwid = pfr.u.wid.subjwid;
    	    	break;

    	    case PFILE__REPLY_PREFIX:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    	news_prof.reply_prefix_len = pfr.u.txt.textlen;
    	    	if (news_prof.reply_prefix_len > sizeof(news_prof.reply_prefix)-1)
    	    	    news_prof.reply_prefix_len = sizeof(news_prof.reply_prefix)-1;
    	    	strncpy(news_prof.reply_prefix, pfr.u.txt.text,
    	    	    	    news_prof.reply_prefix_len);
    	    	*(news_prof.reply_prefix+news_prof.reply_prefix_len) = '\0';
    	    	break;

    	    default:
    	    	if (g != NULL) {
    	    	    Insert_Group(g);
    	    	    g = NULL;
    	    	}
    	    }
    	}

    	sys$close(&fab,0,0);
    	if (g != NULL) {
    	    Insert_Group(g);
    	    g = NULL;
    	}

    	news_prof.profread = 1;

    }

/*
** If the user has set a newsrc file, read that in now, letting the
** information in it supersede the information in the profile.
*/
    if (*news_prof.newsrc) Read_NewsRC();

/*
** Now fetch our mail-related information
*/
    mailctx = 0;
    ITMLST_INIT(mailst[0], MAIL$_NOSIGNAL, 0, 0, 0);
    ITMLST_INIT(mailst[1], 0, 0, 0, 0);
    ITMLST_INIT(mailnul[0], 0, 0, 0, 0);
    status = mail$user_begin(&mailctx, mailst, mailnul);
    if (OK(status)) {
    	unsigned short pnlen;
    	ITMLST_INIT(mailst3[0],MAIL$_USER_COPY_FORWARD,
    	    	    sizeof(news_prof.csfwd),&news_prof.csfwd,0);
    	ITMLST_INIT(mailst3[1],MAIL$_USER_COPY_REPLY,
    	    	    sizeof(news_prof.csreply),&news_prof.csreply,0);
    	ITMLST_INIT(mailst3[2],MAIL$_USER_COPY_SEND,
    	    	    sizeof(news_prof.csmail),&news_prof.csmail,0);
    	if (news_prof.usemailpnam) {
    	    ITMLST_INIT(mailst3[3],MAIL$_USER_PERSONAL_NAME,
    	    	    (sizeof(news_prof.personalname)-1 < 255 ?
    	    	     sizeof(news_prof.personalname)-1 : 255),
    	    	    &news_prof.personalname[0],&pnlen);
    	} else {
    	    ITMLST_INIT(mailst[3], 0, 0, 0, 0);
    	}
    	ITMLST_INIT(mailst3[4], 0, 0, 0, 0);
    	status = mail$user_get_info(&mailctx, mailst, mailst3);
    	if (OK(status) && news_prof.usemailpnam) {
    	    news_prof.personalname[pnlen] = '\0';
    	}
    	status = mail$user_end(&mailctx, mailst, mailnul);
    }

    if (!news_prof.replymail || news_prof.replypost) news_prof.replypost = 1;
    if (!*news_prof.editorname) strcpy(news_prof.editorname, "TPU");
    if (news_prof.hdrlist.head == &news_prof.hdrlist) {
    	for (i = 0; i < sizeof(default_header)/sizeof(int); i++) {
    	    h = mem_gethdr(0);
    	    h->code = default_header[i];
    	    queue_insert(h, news_prof.hdrlist.tail);
    	}
    }
    if (news_prof.ehdrlist.head == &news_prof.ehdrlist) {
    	for (i = NEWS_K_HDR_LO; i <= NEWS_K_HDR_HI; i++) {
    	    h = mem_gethdr(0);
    	    h->code = i;
    	    queue_insert(h, news_prof.ehdrlist.tail);
    	}
    }

    if (news_prof.reply_prefix_len < 0) {
    	news_prof.reply_prefix_len = 1;
    	strcpy(news_prof.reply_prefix, ">");
    }

    return SS$_NORMAL;

} /* Read_Profile */

/*
**++
**  ROUTINE:	Write_Profile
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Writes out the user's NEWSRDR profile.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Write_Profile()
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
unsigned int Write_Profile() {

    struct FAB fab;
    struct NAM nam;
    struct RAB rab;
    struct PFREC pfr;
    struct HDR *hdr;
    unsigned int status, pp[2], pstat;
    char espec[255], rspec[255];
    static unsigned int exquota[2] = {PRV$M_EXQUOTA, 0};
    static $DESCRIPTOR(fname, "NEWSRDR_PROFILE");
    static $DESCRIPTOR(dname, "SYS$LOGIN:.NRPF");

    pstat = sys$setprv(1, exquota, 0, pp);

    fab = cc$rms_fab;
    fab.fab$b_fac = FAB$M_PUT;
    fab.fab$l_fna = fname.dsc$a_pointer;
    fab.fab$b_fns = fname.dsc$w_length;
    fab.fab$l_dna = dname.dsc$a_pointer;
    fab.fab$b_dns = dname.dsc$w_length;

    nam = cc$rms_nam;
    nam.nam$l_esa = espec;
    nam.nam$b_ess = sizeof(espec);
    nam.nam$l_rsa = rspec;
    nam.nam$b_rss = sizeof(rspec);
    fab.fab$l_nam = (void *) &nam;

    status = sys$create(&fab,0,0);
    if (!OK(status)) {
    	lib$signal(NEWS__NOOPNPROF, 0, status, fab.fab$l_stv);
    	if (OK(pstat) && !(pp[0] & PRV$M_EXQUOTA)) sys$setprv(0, exquota, 0, 0);
    	return NEWS__NOOPNPROF;
    }
    rab = cc$rms_rab;
    rab.rab$l_fab = &fab;
    rab.rab$l_rbf = (char *) &pfr;
    rab.rab$l_ubf = (char *) &pfr;
    rab.rab$w_usz = sizeof(pfr);
    sys$connect(&rab,0,0);

    pfr.code = PFILE__FLAGS;
    pfr.u.flags = PFREC_M_NEWFLAGS;
    if (news_prof.edit) pfr.u.flags |= PFREC_M_EDIT;
    if (news_prof.spawnedit) pfr.u.flags |= PFREC_M_SPAWNEDIT;
    if (news_prof.autosigmail) pfr.u.flags |= PFREC_M_AUTOSIGM;
    if (news_prof.autosigpost) pfr.u.flags |= PFREC_M_AUTOSIGP;
    if (news_prof.autosigmail || news_prof.autosigpost) pfr.u.flags |= PFREC_M_AUTOSIG;
    if (news_prof.replymail) pfr.u.flags |= PFREC_M_REPLYM;
    if (news_prof.replypost) pfr.u.flags |= PFREC_M_REPLYP;
    if (!news_prof.autoreadnew) pfr.u.flags |= PFREC_M_NOAUTORN;
    if (news_prof.usemailpnam) pfr.u.flags |= PFREC_M_USEMAILPN;
    if (news_prof.force_proto) pfr.u.flags |= PFREC_M_FORCEPROTO;
    if (news_prof.autosave) pfr.u.flags |= PFREC_M_AUTOSAVE;
    if (!news_prof.newsrc_purge) pfr.u.flags |= PFREC_M_RCNOPURGE;
    rab.rab$w_rsz = PFREC_S_FRONT;
    status = sys$put(&rab,0,0);

    if (OK(status)) {
    	pfr.code = PFILE__NGACTION;
    	pfr.u.flags = news_prof.ngaction;
    	rab.rab$w_rsz = PFREC_S_FRONT;
    	status = sys$put(&rab,0,0);
    }

    if (OK(status) && !news_prof.usemailpnam && *news_prof.personalname) {
    	pfr.code = PFILE__PERSONALNAME;
    	strcpy(pfr.u.txt.text, news_prof.personalname);
    	pfr.u.txt.textlen = strlen(news_prof.personalname);
    	rab.rab$w_rsz = pfr.u.txt.textlen + PFREC_S_FRONT;
    	status = sys$put(&rab,0,0);
    }
    if (OK(status) && *news_prof.editorname != '\0') {
    	pfr.code = PFILE__EDITORNAME;
    	strcpy(pfr.u.txt.text, news_prof.editorname);
    	pfr.u.txt.textlen = strlen(news_prof.editorname);
    	rab.rab$w_rsz = pfr.u.txt.textlen + PFREC_S_FRONT;
    	status = sys$put(&rab,0,0);
    }
    if (OK(status) && *news_prof.sigfile != '\0') {
    	pfr.code = PFILE__SIGFILENAME;
    	strcpy(pfr.u.txt.text, news_prof.sigfile);
    	pfr.u.txt.textlen = strlen(news_prof.sigfile);
    	rab.rab$w_rsz = pfr.u.txt.textlen + PFREC_S_FRONT;
    	status = sys$put(&rab,0,0);
    }
    if (OK(status) && *news_prof.archive != '\0') {
    	pfr.code = PFILE__ARCHIVE;
    	strcpy(pfr.u.txt.text, news_prof.archive);
    	pfr.u.txt.textlen = strlen(news_prof.archive);
    	rab.rab$w_rsz = pfr.u.txt.textlen + PFREC_S_FRONT;
    	status = sys$put(&rab,0,0);
    }
    if (OK(status) && *news_prof.newsrc != '\0') {
    	pfr.code = PFILE__NEWSRC;
    	strcpy(pfr.u.txt.text, news_prof.newsrc);
    	pfr.u.txt.textlen = strlen(news_prof.newsrc);
    	pfr.u.txt.hdrcode = news_prof.newsrc_subonly;
    	rab.rab$w_rsz = pfr.u.txt.textlen + PFREC_S_FRONT;
    	status = sys$put(&rab,0,0);
    }
    if (OK(status) && *news_prof.print_queue != '\0') {
    	pfr.code = PFILE__QUEUE;
    	strcpy(pfr.u.txt.text, news_prof.print_queue);
    	pfr.u.txt.textlen = strlen(news_prof.print_queue);
    	rab.rab$w_rsz = pfr.u.txt.textlen + PFREC_S_FRONT;
    	status = sys$put(&rab,0,0);
    }
    if (OK(status) && *news_prof.print_form != '\0') {
    	pfr.code = PFILE__FORM;
    	strcpy(pfr.u.txt.text, news_prof.print_form);
    	pfr.u.txt.textlen = strlen(news_prof.print_form);
    	rab.rab$w_rsz = pfr.u.txt.textlen + PFREC_S_FRONT;
    	status = sys$put(&rab,0,0);
    }

    if (OK(status)) {
    	pfr.code = PFILE__REPLY_PREFIX;
    	strcpy(pfr.u.txt.text, news_prof.reply_prefix);
    	pfr.u.txt.textlen = news_prof.reply_prefix_len;
    	rab.rab$w_rsz = pfr.u.txt.textlen + PFREC_S_FRONT;
    	status = sys$put(&rab,0,0);
    }

    if (OK(status) &&
    	    (news_prof.dir_fromwid >= 0 || news_prof.dir_subjwid >= 0)) {
    	pfr.code = PFILE__DIRWIDTH;
    	pfr.u.wid.fromwid = news_prof.dir_fromwid;
    	pfr.u.wid.subjwid = news_prof.dir_subjwid;
    	rab.rab$w_rsz = sizeof(int) + PFREC_S_FRONT;
    	status = sys$put(&rab,0,0);
    }
    if (OK(status)) {
    	pfr.code = PFILE__GLOBALKILL;
    	for (hdr = news_prof.killq.head;
    	    	    OK(status) && hdr != (struct HDR *) &news_prof.killq;
    	    	    hdr = hdr->flink) {
    	    strcpy(pfr.u.txt.text, hdr->str);
    	    pfr.u.txt.hdrcode = hdr->code;
    	    pfr.u.txt.textlen = strlen(hdr->str);
    	    rab.rab$w_rsz = pfr.u.txt.textlen + PFREC_S_FRONT;
    	    status = sys$put(&rab,0,0);
    	}
    }

    if (OK(status)) {
    	pfr.code = PFILE__NEWHEADER;
    	pfr.u.rng.rcount = 0;
    	for (hdr = news_prof.hdrlist.head;
    	    	    OK(status) && hdr != (struct HDR *) &news_prof.hdrlist;
    	    	    hdr = hdr->flink) {
    	    if (pfr.u.rng.rcount == MAXHCT) {
    	    	rab.rab$w_rsz = PFREC_S_FRONT + pfr.u.rng.rcount*4;
    	    	status = sys$put(&rab,0,0);
    	    	pfr.u.rng.rcount = 0;
    	    }
    	    pfr.u.rng.range[pfr.u.rng.rcount++] = hdr->code;
    	}
    	if (OK(status) && pfr.u.rng.rcount != 0) {
    	    rab.rab$w_rsz = PFREC_S_FRONT + pfr.u.rng.rcount*4;
    	    status = sys$put(&rab,0,0);
    	}
    }

    if (OK(status) && news_prof.ehdrlist.head != &news_prof.ehdrlist) {
    	pfr.code = PFILE__EXTRACT_HDRS;
    	pfr.u.rng.rcount = 0;
    	for (hdr = news_prof.ehdrlist.head;
    	    	    OK(status) && hdr != (struct HDR *) &news_prof.ehdrlist;
    	       	    hdr = hdr->flink) {
    	    if (pfr.u.rng.rcount == MAXHCT) {
    	    	rab.rab$w_rsz = PFREC_S_FRONT + pfr.u.rng.rcount*4;
    	    	status = sys$put(&rab,0,0);
    	    	pfr.u.rng.rcount = 0;
    	    }
    	    pfr.u.rng.range[pfr.u.rng.rcount++] = hdr->code;
    	}
    	if (OK(status) && pfr.u.rng.rcount != 0) {
    	    rab.rab$w_rsz = PFREC_S_FRONT + pfr.u.rng.rcount*4;
    	    status = sys$put(&rab,0,0);
    	}
    }

    if (OK(status)) {
    	pfr.code = PFILE__LASTGRPCHK;
    	pfr.u.lgc = news_prof.lastgrpchk;
    	rab.rab$w_rsz = 12;
    	status = sys$put(&rab,0,0);
    }

    if (OK(status)) {
    	lib$traverse_tree(&news_prof.gtree, write_grp_info, &rab);
    	status = rab.rab$l_sts;
    }

    if (!OK(status)) lib$signal(NEWS__WRITERR, 2,
    	    nam.nam$b_rsl, nam.nam$l_rsa, status, rab.rab$l_stv);

    sys$disconnect(&rab,0,0);

    if (!OK(status)) fab.fab$l_fop |= FAB$M_DLT;
    sys$close(&fab,0,0);

    if (OK(status)) {
    	struct dsc$descriptor ddsc, dsc;
    	unsigned int ctx;
    	static $DESCRIPTOR(dname_del, "SYS$LOGIN:.NRPF;*");
    	static $DESCRIPTOR(fname_ren, "*.*;1");

    	ddsc.dsc$b_dtype = DSC$K_DTYPE_T;
    	ddsc.dsc$b_class = DSC$K_CLASS_D;
    	ddsc.dsc$w_length = 0;
    	ddsc.dsc$a_pointer = 0;
    	INIT_SDESC(dsc, nam.nam$b_rsl, nam.nam$l_rsa);
    	ctx = 0;
    	while (OK(lib$find_file(&fname, &ddsc, &ctx, &dname_del))) {
    	    if (str$compare_eql(&ddsc, &dsc) != 0) lib$delete_file(&ddsc);
    	}
    	lib$rename_file(&dsc, &fname_ren);
    	str$free1_dx(&ddsc);
    }

    if (OK(pstat) && !(pp[0] & PRV$M_EXQUOTA)) sys$setprv(0, exquota, 0, 0);

    if (!OK(status)) return status;
/*
** If the user has set a newsrc file, then also write out the group
** & article information there.
*/

    if (*news_prof.newsrc) {
    	Write_NewsRC();
    }

    return SS$_NORMAL;

} /* Write_Profile */

/*
**++
**  ROUTINE:	write_grp_info
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Tree traversal action routine that writes out group information
**  to the profile.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	write_grp_info(struct GRP *g, struct RAB *rab)
**
**  g:	    GRP structure, modify, by reference
**  rab:    RAB, modify, by reference.
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
static unsigned int write_grp_info(struct GRP *g, struct RAB *rab) {

    struct PFREC *pfr; 
    struct RNG *rng;
    struct HDR *hdr;
    unsigned int status;
    int k;

    if (!g->avail) return SS$_NORMAL;

    pfr = (struct PFREC *) rab->rab$l_rbf;
    if (news_cfg.newgroups != 2 && !g->subscribed) return SS$_NORMAL;

    pfr->code = g->subscribed ? PFILE__GROUP : PFILE__GROUP_NOSUB;
    strcpy(pfr->u.txt.text, g->grpnam);
    pfr->u.txt.textlen = strlen(g->grpnam);
    rab->rab$w_rsz = pfr->u.txt.textlen + PFREC_S_FRONT;
    status = sys$put(rab,0,0);
    if (!OK(status)) return status;

    if (!g->subscribed) return SS$_NORMAL;

    k = 0;
    pfr->code = PFILE__RANGELIST;
    for (rng = g->seenq.head; rng != (struct RNG *) &(g->seenq);
    	    	    	rng = rng->flink) {
    	if (rng->last >= g->frstavl) {
    	    pfr->u.rng.range[k*2] = (rng->first < g->frstavl ? 1 : rng->first);
    	    pfr->u.rng. range[k*2+1] = rng->last;
    	    if (k == MAXRCT-1) {
    	    	pfr->u.rng.rcount = k;
    	    	rab->rab$w_rsz = PFREC_S_FRONT + k*8;
    	    	status = sys$put(rab,0,0);
    	    	if (!OK(status)) return status;
    	    	k = 0;
    	    } else {
    	    	k += 1;
    	    }
    	}
    }
    if (k != 0) {
    	pfr->u.rng.rcount = k;
    	rab->rab$w_rsz = PFREC_S_FRONT + k*8;
    	status = sys$put(rab,0,0);
    	if (!OK(status)) return status;
    }

    pfr->code = PFILE__KILLLIST;
    for (hdr = g->killq.head; hdr != (struct HDR *) &g->killq; hdr = hdr->flink) {
    	pfr->u.txt.textlen = strlen(hdr->str);
    	pfr->u.txt.hdrcode = hdr->code;
    	if (pfr->u.txt.textlen > sizeof(pfr->u.txt.text)-1) {
    	    pfr->u.txt.textlen = sizeof(pfr->u.txt.text)-1;
    	    strncpy(pfr->u.txt.text, hdr->str, pfr->u.txt.textlen);
    	} else {
    	    strcpy(pfr->u.txt.text, hdr->str);
    	}
    	rab->rab$w_rsz = PFREC_S_FRONT + pfr->u.txt.textlen;
    	status = sys$put(rab,0,0);
    	if (!OK(status)) return status;
    }

    return SS$_NORMAL;

} /* write_grp_info */
