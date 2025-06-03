#ifndef __NEWSRDR_H
#define __NEWSRDR_H 1

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __GNUC__
#include <vms/descrip.h>
#include <vms/stsdef.h>
#include <vms/ssdef.h>
    extern int cli$_absent();
#define CLI$_ABSENT ((unsigned int) cli$_absent)
    extern int cli$_present();
#define CLI$_PRESENT ((unsigned int) cli$_present)
    extern int cli$_negated();
#define CLI$_NEGATED ((unsigned int) cli$_negated)
    extern int cli$_comma();
#define CLI$_COMMA ((unsigned int) cli$_comma)
    extern int lib$_syntaxerr();
#define LIB$_SYNTAXERR ((unsigned int) lib$_syntaxerr)
#else
#include <descrip.h>
#include <stsdef.h>
#include <ssdef.h>
#include <lib$routines.h>
#include <str$routines.h>
#include <starlet.h>
#pragma nostandard
    globalvalue unsigned int CLI$_ABSENT, CLI$_PRESENT, CLI$_NEGATED, CLI$_COMMA;
    globalvalue unsigned int LIB$_SYNTAXERR;
#pragma standard
#ifdef __DECC
#include <builtins.h>
#else
#pragma builtins
#endif
#endif

#ifdef VAXC
#define EXTERN	    globalref
#define GLOBAL	    globaldef
#else
#define EXTERN	    extern
#define GLOBAL
#endif

#pragma nostandard
#include "news_msg.h"
#pragma standard

#ifdef NULL
#undef NULL
#endif
#define NULL ((void *) 0)

    extern unsigned int cli$present(), cli$get_value(), cli$dcl_parse();

#define SRV__ECHO   	1
#define SRV__NOECHO 	2
#define NNTP__HELPFOLLOWS	100
#define NNTP__DEBUGOUTPUT	199
#define NNTP__HELOPOSTOK	200
#define NNTP__HELONOPOST	201
#define NNTP__SLAVESTATOK	202
#define NNTP__CLOSING   	205
#define NNTP__GRPSELECTED	211
#define NNTP__GLISTFOLLOWS	215
#define NNTP__ARTCLFOLLOWS	220
#define NNTP__HEADFOLLOWS	221
#define NNTP__BODYFOLLOWS	222
#define NNTP__TEXTSEPARATE	223
#define NNTP__DATAFOLLOWS   	224
#define NNTP__ALISTFOLLOWS	230
#define NNTP__NGLSTFOLLOWS	231
#define NNTP__ARTICLEXFERED	235
#define NNTP__ARTICLEPOSTED	240
#define NNTP__AUTHACCEPTED  	281
#define NNTP__SENDXARTICLE	335
#define NNTP__SENDPARTICLE	340
#define NNTP__PASSWREQD	    	381
#define NNTP__SVCDISCONT	400
#define NNTP__NOSUCHGROUP	411
#define NNTP__NOCURGROUP	412
#define NNTP__NOCURARTICLE	420
#define NNTP__NONEXTARTICLE	421
#define NNTP__NOPREVARTICLE	422
#define NNTP__NOSUCHARTNUM	423
#define NNTP__NOSUCHARTICLE	430
#define NNTP__NOWANTARTICLE	435
#define NNTP__XFERFAILED	436
#define NNTP__ARTICLEREJECT	437
#define NNTP__NOPOSTING 	440
#define NNTP__POSTFAILED	441
#define NNTP__AUTHREQD	    	480
#define NNTP__CMDUNRECOGNZD	500
#define NNTP__CMDSYNTAXERR	501
#define NNTP__ACCESSDENIED	502
#define NNTP__PROGRAMERROR	503

#define NEWS_K_HDR_LO   	    1
#define NEWS_K_HDR_FROM 	    1
#define NEWS_K_HDR_DATE 	    2
#define NEWS_K_HDR_NEWSGROUPS       3
#define NEWS_K_HDR_SUBJECT	    4
#define NEWS_K_HDR_MESSAGE_ID       5
#define NEWS_K_HDR_PATH 	    6
#define NEWS_K_HDR_REPLY_TO	    7
#define NEWS_K_HDR_SENDER	    8
#define NEWS_K_HDR_FOLLOWUP_TO      9
#define NEWS_K_HDR_EXPIRES	    10
#define NEWS_K_HDR_REFERENCES       11
#define NEWS_K_HDR_CONTROL	    12
#define NEWS_K_HDR_DISTRIBUTION     13
#define NEWS_K_HDR_ORGANIZATION     14
#define NEWS_K_HDR_KEYWORDS	    15
#define NEWS_K_HDR_SUMMARY	    16
#define NEWS_K_HDR_APPROVED	    17
#define NEWS_K_HDR_LINES	    18
#define NEWS_K_HDR_XREF 	    19
#define NEWS_K_HDR_OTHER	    20
#define NEWS_K_HDR_HI   	    20

typedef unsigned short	WORD;
typedef	unsigned char	BYTE;
typedef void 	    	*POINTER;
typedef struct { unsigned int long1, long2; } TIME;
typedef struct dsc$descriptor DESCRIP;
typedef struct { WORD bufsiz, itmcod; POINTER bufadr, retlen; } ITMLST;

#define FSPEC_SIZE 255
#define STRING_SIZE 1024

    struct QUE {
    	void *head;
    	void *tail;
    };

    struct HDR {
    	struct HDR *flink, *blink;
    	int code;
    	unsigned short len;
    	char str[1];
    };

#define GRP_S_GRPNAM	128
#define GRP_K_VALID 	1
#define GRP_K_INVALID	2
    struct GRP {
    	struct GRP *left, *right;
    	unsigned int reserved_unused;
    	struct GRP *next;
    	struct QUE seenq;
    	struct QUE killq;
    	int frstavl, lastavl;
    	int avail;
    	int subscribed;
    	int inprofile;
    	int directpost;
    	int holecheck_min, holecheck_max;
    	char grpnam[GRP_S_GRPNAM+1];
    	unsigned char *valid;
    };

    struct RNG {
    	struct RNG *flink, *blink;
    	int first, last;
    };

#define NG_K_NONE   	    0
#define NG_K_SUBSCRIBE	    1
#define NG_K_PROMPT 	    2

    struct PROF {
    	TIME lastgrpchk;
    	struct GRP *gtree;
    	struct GRP *glist;
    	struct GRP *glast;
    	struct QUE hdrlist;
    	struct QUE ehdrlist;
    	struct QUE killq;
    	int profread, edit, captive, autosigpost, autosigmail;
    	int csmail, csreply, csfwd, spawnedit, replymail, replypost;
    	int dir_fromwid, dir_subjwid, newsrc_subonly, autoreadnew;
    	int usemailpnam, force_proto, ngaction, autosave, newsrc_purge;
    	int reply_prefix_len;
    	char personalname[STRING_SIZE];
    	char editorname[FSPEC_SIZE+1];
    	char sigfile[FSPEC_SIZE+1];
    	char archive[FSPEC_SIZE+1];
    	char newsrc[FSPEC_SIZE+1];
    	char print_queue[FSPEC_SIZE+1];
    	char print_form[FSPEC_SIZE+1];
    	char reply_prefix[33];
    };

    struct CFG {
    	int cachesize;
    	unsigned int (*namcvt)();
    	unsigned int (*fnmcvt)();
    	unsigned int (*namclup)();
    	unsigned int namctx;
    	unsigned int (*adrcvt)();
    	unsigned int (*adrclup)();
    	unsigned int adrctx;
    	unsigned int (*chrlton)();
    	unsigned int (*chrntol)();
    	int postingok, bangpath, chrcnv, namcnv, adrcnv, neggmtoff;
    	int dst, bangaddr, genmsgid, gendate, newgroups, dopath, xhdr, xover;
    	char server_name[STRING_SIZE];
    	char org_name[STRING_SIZE];
    	char node_name[STRING_SIZE];
    	char username[STRING_SIZE];
    	char mail_proto[FSPEC_SIZE];
    	char reply_to[STRING_SIZE];
    	char mailnode[STRING_SIZE];
    	char gmtoffset[STRING_SIZE];
    	char pathstr[STRING_SIZE];
    };

#define INIT_DYNDSCPTR(str) {str->dsc$w_length = 0; str->dsc$a_pointer = (void *) 0;\
    	    str->dsc$b_class = DSC$K_CLASS_D; str->dsc$b_dtype = DSC$K_DTYPE_T;}
#define INIT_DYNDESC(str) {str.dsc$w_length = 0; str.dsc$a_pointer = (void *) 0;\
    	    str.dsc$b_class = DSC$K_CLASS_D; str.dsc$b_dtype = DSC$K_DTYPE_T;}
#define INIT_SDESC(str,len,ptr) {str.dsc$w_length=(len);str.dsc$a_pointer=(ptr);\
    	    str.dsc$b_class=DSC$K_CLASS_S; str.dsc$b_dtype=DSC$K_DTYPE_T;}
#define ITMLST_INIT(itm,c,s,a,r) {itm.bufsiz=(s); itm.itmcod=(c);\
    	    itm.bufadr=(POINTER)(a); itm.retlen=(POINTER)(r);}
#define OK(x) $VMS_STATUS_SUCCESS(x)
#define min(a,b) ((a) < (b) ? (a) : (b))

#ifdef __ALPHA
#define queue_insert(item,pred) __PAL_INSQUEL((void *)(pred),(void *)(item))
#define queue_remove(entry,addr) (((struct QUE *)entry)->head == entry ? 0 :\
    	    	    	    (__PAL_REMQUEL((void *)(entry),(void *)(addr)),1))
#else
#ifndef __GNUC__
#define queue_insert(item,pred) _INSQUE(item,pred)
#define queue_remove(entry,addr) (((struct QUE *)entry)->head == entry ? 0 :\
    	    	    	    (_REMQUE(entry,addr),1))
#endif
#endif

#endif
/*
** Routines from module MISC
*/
#ifndef MODULE_MISC
    extern void make_temp_fspec(char *, int);
    extern void make_nntp_dtstring(TIME *, char *, int);
    extern unsigned int Compose_Message(char *, char *, int, int *);
    extern void upcase(char *);
    extern void locase(char *);
    extern void Format_Header(struct HDR *, char *, int, int *);
    extern void Make_Date(TIME *, char *, int);
    extern int Yes_Answer(char *, int);
    extern unsigned int cli_get_value(char *, char *, int);
    extern unsigned int cli_present(char *);
    extern unsigned int get_logical(char *, char *);
    extern unsigned int get_system_logical(char *, char *);
    extern int streql_case_blind(char *, char *);
    extern int strneql_case_blind(char *, char *, int);
    extern unsigned int find_image_symbol(char *, char *, void *);
    extern void insert_header(char *, struct HDR *, int);
    extern unsigned int table_parse(void *, void *, void *);
    extern void Check_Connection(void);
#endif

/*
** Routines from module MEM
*/

#ifndef MODULE_MEM
    extern struct HDR	*mem_gethdr(int);
    extern void	    	mem_freehdr(struct HDR *);
    extern struct GRP	*mem_getgrp(void);
    extern void	    	mem_freegrp(struct GRP *);
    extern struct RNG	*mem_getrng(void);
    extern void	    	mem_freerng(struct RNG *);
#endif

/*
** Server_xxxx routines
*/

    extern unsigned int get_hostname(char *, int);
    extern unsigned int server_connect(char *);
    extern unsigned int server_disconnect(void);
    extern unsigned int server_send(char *);
    extern unsigned int server_get_line();
    extern unsigned int server_get_reply();
    extern unsigned int server_check(void);

/*
** Routines from module PAGER
*/
#ifndef MODULE_PAGER
    extern unsigned int Pager_Init(int);
    extern unsigned int Begin_Paged_Output(char *, ...);
    extern int          Put_Paged(char *, int);
    extern int          Paged_Output_Done(void);
    extern void         Discard_Paged_Output(void);
    extern void         Finish_Paged_Output(int);
    extern unsigned int put_output(char *);
    extern unsigned int put_output_counted(char *, int);
    extern unsigned int put_output_nocc(char *);
    extern unsigned int put_output_dx(struct dsc$descriptor *);
    extern unsigned int put_errmsg(struct dsc$descriptor *);
    extern unsigned int get_cmd(char *, int, char *);
    extern unsigned int get_cmd_dx(struct dsc$descriptor *, struct dsc$descriptor *,
    	    	    	    	short *);
    extern unsigned int put_paged_help(char *, char *);
    extern unsigned int Define_Key(char *, int);
#endif

/*
** Routines from module FILEIO
*/
#ifndef MODULE_FILEIO
    extern unsigned int file_create(char *, unsigned int *, char *, char *);
    extern unsigned int file_open(char *, unsigned int *, char *, char *, int *);
    extern unsigned int file_append(char *, unsigned int *, char *, char *, int *);
    extern unsigned int file_find(char *, char *, char *, char *);
    extern unsigned int file_fidopen(char *, unsigned int *);
    extern unsigned int file_exists(char *, char *);
    extern unsigned int file_close(unsigned int);
    extern unsigned int file_dclose(unsigned int);
    extern unsigned int file_read(unsigned int, char *, int, int *);
    extern unsigned int file_write(unsigned int, char *, int);
    extern unsigned int file_getpos(unsigned int, short[3]);
    extern unsigned int file_setpos(unsigned int, short[3]);
    extern unsigned int file_get_rdt(char *, TIME *);
    extern unsigned int file_delete(char *);
    extern unsigned int Copy_File(char *, char *, char *, int);
#endif
