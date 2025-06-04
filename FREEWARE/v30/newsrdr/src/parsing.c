/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Parsing routines.
**
**  MODULE DESCRIPTION:
**
**  	Various routines that parse various things.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, MADGOAT SOFTWARE. 
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  06-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	06-SEP-1992 V1.0    Madison 	Initial coding.
**  	12-SEP-1992 V1.0-1  Madison 	Add support for '.' in parse_range.
**  	08-OCT-1992 V1.0-2  Madison 	Check for tab a.w.a. space in PLR.
**  	12-APR-1993 V1.1    Madison 	Line count added to parse_headers.
**  	19-MAY-1993 V1.1-1  Madison 	Fix Parse_Range bug.
**  	23-JUN-1993 V1.1-2  Madison 	Allow no y/n flag on LIST replies.
**  	29-JUN-1993 V1.2    Madison 	Don't signal SS$_ABORT on parse errors.
**  	09-OCT-1993 V1.3    Madison 	Support for FIRST, LAST in Parse_Range.
**  	13-APR-1994 V1.3-1  Madison 	Set length field in headers.
**--
*/
#include "newsrdr.h"
#include "globals.h"
#include "tpadef.h"

#define TAB '\011'

    extern unsigned int parse822();

    struct PGRBLK {
    	struct tpadef tpa0;
    	int *acnt;
    	int *first;
    	int *last;
    };
    extern int pgr_state(), pgr_key();

    struct PRBLK {
    	struct tpadef tpa0;
    	int *first;
    	int *last;
    	int current;
    	struct GRP *grp;
    };
    extern int parse_range_state(), parse_range_key();

    struct PXBLK {
    	struct tpadef tpa0;
    	struct QUE *hdrq;
    	char *grpp;
    	char *grp;
    	char *nump;
    	char *num;
    };

/*
**  Forward declarations
*/

    unsigned int Parse_List_Reply(char *, struct GRP *);
    unsigned int parse_group_reply(char *, int *, int *, int *);
    unsigned int pgr_store(struct PGRBLK *);
    unsigned int Parse_Range(char *, int *, int *, int, struct GRP *);
    unsigned int parse_range_store(struct PRBLK *);
    unsigned int Parse_ToList(char *, struct QUE *, int);
    unsigned int Parse_Headers(struct QUE *, struct QUE *, int *);
    unsigned int Parse_Xref(char *, struct QUE *);
    unsigned int px_store(struct PXBLK *);
    unsigned int csl_parse(char *, struct QUE *);
    extern int px_state(), px_key();


/*
**++
**  ROUTINE:	Parse_List_Reply
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Parses group information from a LIST command reply, and fills
**  in the appropriate parts of a GRP structure from that information.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Parse_List_Reply(char *str, struct GRP *grp)
**
**  str:    ASCIZ_string, read only, by reference
**  grp:    GRP structure, modify, by reference
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
unsigned int Parse_List_Reply(char *str, struct GRP *grp) {

    char *anchor, *cp, *cp2;
    int remlen, len, copylen;

    anchor = str;
    remlen = strlen(str);

    cp = strchr(anchor, ' ');
    if (!cp) cp = strchr(anchor, TAB);
    if (!cp) return NEWS__LISTREPLYERR;
    len = cp - anchor;
    copylen = (len > sizeof(grp->grpnam)-1 ? sizeof(grp->grpnam)-1 : len);
    strncpy(grp->grpnam, anchor, copylen);
    *(grp->grpnam+copylen) = '\0';

    remlen -= len + 1;
    anchor += len + 1;

    cp = strchr(anchor, ' ');
    if (!cp) cp = strchr(anchor, TAB);
    if (!cp) return NEWS__LISTREPLYERR;
    len = cp - anchor;
    for (cp2 = anchor; *cp2 == '0'; cp2++);
    if (!*cp2) {
    	grp->lastavl = 0;
    } else {
    	lib$cvt_dtb(cp-cp2, cp2, &grp->lastavl);
    }

    remlen -= len + 1;
    anchor += len + 1;

    cp = strchr(anchor, ' ');
    if (!cp) cp = strchr(anchor, TAB);
    if (!cp) return NEWS__LISTREPLYERR;
    len = cp - anchor;
    lib$cvt_dtb(cp-anchor, anchor, &grp->frstavl);

    remlen -= len + 1;
    anchor += len + 1;

    if (remlen < 1) {
    	grp->directpost = 1;
    } else {
    	if (*anchor == '=' || *anchor == 'X' || *anchor == 'x') return 0;
    	grp->directpost = *anchor != 'Y' && *anchor != 'y';
    }

    return SS$_NORMAL;

} /* Parse_List_Reply */

/*
**++
**  ROUTINE:	parse_group_reply
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Uses LIB$T{ABLE_}PARSE to parse the response of a GROUP
**  command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	parse_group_reply(char *str, int *acnt, int *first, int *last)
**
**  str:    ASCIZ_string, read only, by reference
**  acnt:   integer, write only, by reference
**  first:  integer, write only, by reference
**  last:   integer, write only, by reference
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
unsigned int parse_group_reply(char *str, int *acnt, int *first, int *last) {

    struct PGRBLK tpablk;

    memset(&tpablk, 0, sizeof(tpablk));
    tpablk.tpa0.tpa$l_count = TPA$K_COUNT0 + 3;
    tpablk.tpa0.tpa$l_stringcnt = strlen(str);
    tpablk.tpa0.tpa$l_stringptr = str;
    tpablk.acnt = acnt;
    tpablk.first = first;
    tpablk.last = last;

    return table_parse(&tpablk, (void *) pgr_state, (void *) pgr_key);

} /* parse_group_reply */

/*
**++
**  ROUTINE:	pgr_store
**
**  FUNCTIONAL DESCRIPTION:
**
**  	LIB$T{ABLE_}PARSE action routine for use with parse_group_reply.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	pgr_store(...)
**
**  	For TABLE_PARSE, the elements of the PGRBLK structure are the
**  	arguments.
**
**  	For LIB$TABLE_PARSE, the address of the PGRBLK structure is
**  	passed as the only argument.
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
unsigned int pgr_store(struct PGRBLK *tpa) {

    switch (tpa->tpa0.tpa$l_param) {

    case 1:
    	*tpa->acnt = tpa->tpa0.tpa$l_number;
    	break;
    case 2:
    	*tpa->first = tpa->tpa0.tpa$l_number;
    	break;
    case 3:
    	*tpa->last = tpa->tpa0.tpa$l_number;
    	break;
    }

    return SS$_NORMAL;

} /* pgr_store */

/*
**++
**  ROUTINE:	Parse_Range
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Parses an article range specification.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Parse_Range(char *str, int *first, int *last, int current)
**
**  str:    	ASCIZ_string, read only, by reference
**  first:  	integer, write only, by reference
**  last:   	integer, write only, by reference
**  current:	integer, read only, by value
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
unsigned int Parse_Range(char *str, int *first, int *last, int current, struct GRP *grp) {

    struct PRBLK tpablk;

    memset(&tpablk, 0, sizeof(tpablk));
    tpablk.tpa0.tpa$l_count = TPA$K_COUNT0+4;
    tpablk.tpa0.tpa$l_stringcnt = strlen(str);
    tpablk.tpa0.tpa$l_stringptr = str;
    tpablk.first = first;
    tpablk.last = last;
    tpablk.current = current;
    tpablk.grp = grp;

    return table_parse(&tpablk, (void *) parse_range_state, (void *) parse_range_key);

} /* Parse_Range */

/*
**++
**  ROUTINE:	parse_range_store
**
**  FUNCTIONAL DESCRIPTION:
**
**  	LIB$T{}PARSE action routine for use with Parse_Range.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	parse_range_store(...)
**
**  	For TABLE_PARSE, the elements of the PRBLK structure are the
**  	arguments.
**
**  	For LIB$TABLE_PARSE, the address of the PRBLK structure is
**  	passed as the only argument.
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
unsigned int parse_range_store(struct PRBLK *tpa) {

    switch (tpa->tpa0.tpa$l_param) {

    case 1:
    	*tpa->first = *tpa->last = tpa->tpa0.tpa$l_number;
    	break;
    case 2:
    	*tpa->last = tpa->tpa0.tpa$l_number;
    	break;
    case 3:
    	if (tpa->current == 0) return LIB$_SYNTAXERR;
    	*tpa->first = *tpa->last = tpa->current;
    	break;
    case 4:
    	if (tpa->current == 0) return LIB$_SYNTAXERR;
    	*tpa->last = tpa->current;
    	break;
    case 5:
    	if (tpa->grp == 0) return LIB$_SYNTAXERR;
    	*tpa->first = *tpa->last = tpa->grp->frstavl;
    	break;
    case 6:
    	if (tpa->grp == 0) return LIB$_SYNTAXERR;
    	*tpa->first = *tpa->last = tpa->grp->lastavl;
    	break;
    case 7:
    	if (tpa->grp == 0) return LIB$_SYNTAXERR;
    	*tpa->last = tpa->grp->lastavl;
    	break;
    default:
    	break;
    }

    return SS$_NORMAL;

} /* parse_range_store */

/*
**++
**  ROUTINE:	Parse_ToList
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Parses a list of addresses.  Actually, calls parse822 to
**  do that.  Strips all the extraneous stuff off each address,
**  inserts it in a queue.  If keep_name is set, the extraneous
**  stuff is kept.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Parse_ToList(char *str, struct QUE *hdrq, int keep_name)
**
**  str:    	ASCIZ_string, read only, by reference
**  hdrq:   	QUE structure, modify, by reference
**  keep_name:	boolena, read only, by value
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
unsigned int Parse_ToList(char *str, struct QUE *hdrq, int keep_name) {

    unsigned int ctx, status;
    char *address, *fulladdress;

    ctx = 0;
    while (OK((status = parse822(str, &ctx, &fulladdress, &address, 0)))) {
    	insert_header((keep_name ? fulladdress : address), hdrq->tail, 0);
    }

    return (status == SS$_ENDOFFILE) ? SS$_NORMAL : status;

} /* Parse_ToList */

/*
**++
**  ROUTINE:	Parse_Headers
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Parses a queue of headers that are in plain text form into a queue
**  of TLV representations.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Parse_Headers(struct QUE *inq, struct QUE *outq, int *)
**
**  inq:    QUE structure, read only, by reference
**  outq:   QUE structure, modify, by reference
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
unsigned int Parse_Headers(struct QUE *inq, struct QUE *outq, int *xlines) {

    struct HDR *hdr, *hdr2;
    int i, len, lines, j;
    char *cp;
    static char *Tag[] = {"From","Date","Newsgroups","Subject",
    	"Message-ID","Path","Reply-To","Sender","Followup-To",
    	"Expires","References","Control","Distribution",
    	"Organization","Keywords","Summary","Approved","Lines",
    	"Xref"};
    static int Code[] = {NEWS_K_HDR_FROM,NEWS_K_HDR_DATE,NEWS_K_HDR_NEWSGROUPS,
    	NEWS_K_HDR_SUBJECT,NEWS_K_HDR_MESSAGE_ID,NEWS_K_HDR_PATH,
    	NEWS_K_HDR_REPLY_TO,NEWS_K_HDR_SENDER,NEWS_K_HDR_FOLLOWUP_TO,
    	NEWS_K_HDR_EXPIRES,NEWS_K_HDR_REFERENCES,NEWS_K_HDR_CONTROL,
    	NEWS_K_HDR_DISTRIBUTION,NEWS_K_HDR_ORGANIZATION,NEWS_K_HDR_KEYWORDS,
    	NEWS_K_HDR_SUMMARY,NEWS_K_HDR_APPROVED,NEWS_K_HDR_LINES,
    	NEWS_K_HDR_XREF};

    lines = 0;
    for (hdr = inq->head; hdr != (struct HDR *) inq; hdr = hdr->flink) {
    	cp = strchr(hdr->str, ':');
    	len = cp-hdr->str;
    	for (i = 0; i < sizeof(Tag)/sizeof(char *); i++) {
    	    if (len == strlen(Tag[i]) &&
    	    	    strneql_case_blind(hdr->str, Tag[i], len)) {
    	    	while (isspace(*(++cp)));
    	    	j = strlen(cp);
    	    	hdr2 = mem_gethdr(j+1);
    	    	hdr2->code = Code[i];
    	    	hdr2->len = j;
    	    	strcpy(hdr2->str, cp);
    	    	queue_insert(hdr2, outq->tail);
    	    	lines++;
    	    	break;
    	    }
    	}
    }

    if (xlines) *xlines = lines;
    return SS$_NORMAL;

} /* parse_headers */

/*
**++
**  ROUTINE:	Parse_Xref
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Uses LIB$T{}PARSE to parse an Xref: header, building a queue
**  of group/article references.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Parse_Xref(char *str, struct QUE *hdrq)
**
**  str:    ASCIZ_string, read only, by reference
**  hdrq:   QUE structure, modify, by reference
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
unsigned int Parse_Xref(char *str, struct QUE *hdrq) {

    struct PXBLK tpablk;
    char grp[STRING_SIZE];
    char num[STRING_SIZE];

    memset(&tpablk, 0, sizeof(tpablk));
    tpablk.tpa0.tpa$l_count = TPA$K_COUNT0+4;
    tpablk.tpa0.tpa$l_options = TPA$M_BLANKS;
    tpablk.tpa0.tpa$l_stringcnt = strlen(str);
    tpablk.tpa0.tpa$l_stringptr = str;
    tpablk.hdrq = hdrq;
    tpablk.grp = tpablk.grpp = grp;
    tpablk.num = tpablk.nump = num;

    return table_parse(&tpablk, (void *) px_state, (void *) px_key);

} /* Parse_Xref */

/*
**++
**  ROUTINE:	px_store
**
**  FUNCTIONAL DESCRIPTION:
**
**  	LIB$T{}PARSE action routine for Parse_Xref.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	px_store(...)
**
**  	For TABLE_PARSE, the elements of the PXBLK structure are the
**  	arguments.
**
**  	For LIB$TABLE_PARSE, the address of the PXBLK structure is
**  	passed as the only argument.
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
unsigned int px_store(struct PXBLK *tpa) {

    int copylen;
    struct HDR *h;

    switch (tpa->tpa0.tpa$l_param) {

    case 1:
    	strncpy(tpa->grp, tpa->tpa0.tpa$l_tokenptr, tpa->tpa0.tpa$l_tokencnt);
    	tpa->grpp = tpa->grp + tpa->tpa0.tpa$l_tokencnt;
    	*tpa->grpp = '\0';
    	break;

    case 2:
    	copylen = tpa->tpa0.tpa$l_tokencnt;
    	if (copylen > STRING_SIZE-(tpa->grpp-tpa->grp)-1)
    	    	copylen = STRING_SIZE-(tpa->grpp-tpa->grp)-1;
    	if (copylen > 0) {
    	    strncpy(tpa->grpp, tpa->tpa0.tpa$l_tokenptr, copylen);
    	    tpa->grpp += copylen;
    	    *tpa->grpp = '\0';
    	}
    	break;

    case 3:
    	tpa->nump = tpa->num;
    	break;

    case 4:
    	copylen = tpa->tpa0.tpa$l_tokencnt;
    	if (copylen > STRING_SIZE-(tpa->nump-tpa->num)-1)
    	    	copylen = STRING_SIZE-(tpa->nump-tpa->num)-1;
    	if (copylen > 0) {
    	    strncpy(tpa->nump, tpa->tpa0.tpa$l_tokenptr, copylen);
    	    tpa->nump += copylen;
    	    *tpa->nump = '\0';
    	}
    	break;

    case 5:
    	if (tpa->grpp != tpa->grp && tpa->nump != tpa->num) {
    	    int num;
    	    lib$cvt_dtb(strlen(tpa->num), tpa->num, &num);
    	    insert_header(tpa->grp, tpa->hdrq->tail, num);
    	}

    default:
    	break;
    }

    return SS$_NORMAL;

} /* px_store */

/*
**++
**  ROUTINE:	csl_parse
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Parses a comma-separated list of strings, taking into account
**  quoting and comment delimiters (exclamation points).  Builds a queue
**  of the individual items.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	csl_parse(char *str, struct QUE *que)
**
**  str:    ASCIZ_string, read only, by reference
**  que:    QUE structure, modify, by reference
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
unsigned int csl_parse(char *str, struct QUE *que) {

    char tmp[STRING_SIZE], *icp, *ocp, *cp;
    struct HDR *hdr;
    int inq;

    icp = str;
    while (isspace(*icp)) icp++;

    inq = 0;
    ocp = tmp;

    while (*icp) {
    	if (inq) {
    	    if (*icp == '"') inq = 0;
    	    *ocp++ = *icp++;
    	} else {
    	    if (*icp == '!') {
    	    	break;
    	    } else if (*icp == ',') {
    	    	while (ocp > tmp && isspace(*(ocp-1))) ocp--;
    	    	*ocp = '\0';
    	    	if (strneql_case_blind(tmp, news_cfg.mail_proto,
    	    	    	    strlen(news_cfg.mail_proto))) {
    	    	    cp = tmp + strlen(news_cfg.mail_proto);
    	    	    if (*cp == '"' && *(ocp-1) == '"') {
    	    	    	cp++;
    	    	    	*(--ocp) = '\0';
    	    	    }
    	    	} else cp = tmp;
    	    	hdr = mem_gethdr(ocp-cp);
    	    	strcpy(hdr->str, cp);
    	    	queue_insert(hdr, que->tail);
    	    	ocp = tmp;
    	    	icp++;
    	    	while (isspace(*icp)) icp++;
    	    } else {
    	    	if (*icp == '"') inq = 1;
    	    	*ocp++ = *icp++;
    	    }
    	}
    }

    while (ocp > tmp && isspace(*(ocp-1))) ocp--;
    if (ocp > tmp) {
    	*ocp = '\0';
    	if (strneql_case_blind(tmp, news_cfg.mail_proto,
    	    	    	strlen(news_cfg.mail_proto))) {
    	    cp = tmp + strlen(news_cfg.mail_proto);
    	    if (*cp == '"' && *(ocp-1) == '"') {
    	    	cp++;
    	    	*(--ocp) = '\0';
    	    }
    	} else cp = tmp;
    	hdr = mem_gethdr(ocp-cp);
    	strcpy(hdr->str, cp);
    	queue_insert(hdr, que->tail);
    }

    return SS$_NORMAL;

} /* csl_parse */
